/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "codec_output.h"
#include "codec_sgtl5000.h"
#include "lc3.h"
#include "pmic.h"

LOG_MODULE_REGISTER(codec_output, CONFIG_LOG_DEFAULT_LEVEL);

#define I2S_CODEC_BLOCK_SIZE_BYTES     (LC3_MAX_NUM_SAMPLES_STEREO * sizeof(int16_t))
#define I2S_CODEC_BLOCK_COUNT          24U
#define I2S_CODEC_DRIVER_PRELOAD_BLOCKS 4U
#define I2S_CODEC_MIN_BUFFER_US        120000U
#define I2S_CODEC_TIMEOUT_MS           1000
#define CODEC_OUTPUT_THREAD_STACK_SIZE 2048
#define CODEC_OUTPUT_THREAD_PRIORITY   2

struct decoded_sdu {
	int16_t right_frames[CONFIG_MAX_CODEC_FRAMES_PER_SDU][LC3_MAX_NUM_SAMPLES_MONO];
	int16_t left_frames[CONFIG_MAX_CODEC_FRAMES_PER_SDU][LC3_MAX_NUM_SAMPLES_MONO];
	size_t right_frames_cnt;
	size_t left_frames_cnt;
	size_t mono_frames_cnt;
	size_t frame_size_bytes;
	uint32_t ts;
};

struct codec_output_block {
	void *mem_block;
	size_t size;
	uint32_t session_id;
};

K_MEM_SLAB_DEFINE_STATIC(i2s_tx_slab, I2S_CODEC_BLOCK_SIZE_BYTES, I2S_CODEC_BLOCK_COUNT, 4);
K_MSGQ_DEFINE(codec_output_msgq, sizeof(struct codec_output_block), I2S_CODEC_BLOCK_COUNT, 4);
K_SEM_DEFINE(codec_output_start_sem, 0, 1);

static const struct device *const codec_i2s = DEVICE_DT_GET(DT_NODELABEL(i2s0));
static struct decoded_sdu decoded_sdu;
static struct k_thread codec_output_thread;
static K_KERNEL_STACK_DEFINE(codec_output_thread_stack, CODEC_OUTPUT_THREAD_STACK_SIZE);
static bool initialized;
static bool pmic_initialized;
static bool tx_started;
static bool worker_initialized;
static uint32_t current_sample_rate_hz;
static uint32_t configured_presentation_delay_us;
static uint32_t current_session_id;
static size_t current_stereo_frame_bytes;

static bool ts_overflowed(uint32_t ts)
{
	/* If the timestamp differs by roughly an order of magnitude, assume wraparound. */
	return ((uint64_t)ts * 10U) < decoded_sdu.ts;
}

static uint32_t next_session_id(uint32_t session_id)
{
	session_id++;

	if (session_id == 0U) {
		session_id++;
	}

	return session_id;
}

static uint32_t codec_output_get_frame_duration_us(size_t stereo_frame_bytes)
{
	const uint64_t mono_samples = stereo_frame_bytes / (2U * sizeof(int16_t));

	if (current_sample_rate_hz == 0U || stereo_frame_bytes == 0U) {
		return 0U;
	}

	return (uint32_t)((mono_samples * USEC_PER_SEC) / current_sample_rate_hz);
}

static size_t codec_output_get_startup_block_target(size_t stereo_frame_bytes)
{
	const uint32_t frame_duration_us = codec_output_get_frame_duration_us(stereo_frame_bytes);
	const uint32_t target_buffer_us =
		MAX(configured_presentation_delay_us, I2S_CODEC_MIN_BUFFER_US);
	size_t target;

	if (frame_duration_us == 0U) {
		return 1U;
	}

	target = DIV_ROUND_UP(target_buffer_us, frame_duration_us);

	if (target >= I2S_CODEC_BLOCK_COUNT) {
		target = I2S_CODEC_BLOCK_COUNT - 1U;
	}

	return MAX(target, 1U);
}

static void codec_output_free_block(const struct codec_output_block *block)
{
	if (block->mem_block != NULL) {
		k_mem_slab_free(&i2s_tx_slab, block->mem_block);
	}
}

static int codec_output_alloc_silence_block(size_t size, struct codec_output_block *block)
{
	void *mem_block;
	int ret;

	if (size == 0U || size > I2S_CODEC_BLOCK_SIZE_BYTES || block == NULL) {
		return -EINVAL;
	}

	ret = k_mem_slab_alloc(&i2s_tx_slab, &mem_block, K_NO_WAIT);
	if (ret != 0) {
		return ret;
	}

	memset(mem_block, 0, size);

	block->mem_block = mem_block;
	block->size = size;
	block->session_id = current_session_id;

	return 0;
}

static void codec_output_purge_pending_blocks(void)
{
	struct codec_output_block block;

	while (k_msgq_get(&codec_output_msgq, &block, K_NO_WAIT) == 0) {
		codec_output_free_block(&block);
	}
}

static int codec_output_drop_tx(void)
{
	int ret;

	if (!initialized) {
		tx_started = false;
		return 0;
	}

	ret = i2s_trigger(codec_i2s, I2S_DIR_TX, I2S_TRIGGER_DROP);
	tx_started = false;
	if (ret != 0) {
		LOG_WRN("Failed to drop I2S TX stream: %d", ret);
	}

	return ret;
}

static int codec_output_write_block_to_i2s(const struct codec_output_block *block)
{
	int ret;

	ret = i2s_write(codec_i2s, block->mem_block, block->size);
	if (ret != 0) {
		LOG_WRN("Failed to queue I2S frame: %d", ret);
		codec_output_free_block(block);
	}

	return ret;
}

static int codec_output_start_tx_if_ready(void)
{
	struct codec_output_block block;
	const uint32_t target_buffer_us =
		MAX(configured_presentation_delay_us, I2S_CODEC_MIN_BUFFER_US);
	size_t preloaded_blocks = 0U;
	int ret;

	if (!initialized || tx_started) {
		return 0;
	}

	while (preloaded_blocks < I2S_CODEC_DRIVER_PRELOAD_BLOCKS) {
		ret = k_msgq_get(&codec_output_msgq, &block, K_NO_WAIT);
		if (ret != 0) {
			break;
		}

		if (block.session_id != current_session_id || !initialized) {
			codec_output_free_block(&block);
			continue;
		}

		ret = codec_output_write_block_to_i2s(&block);
		if (ret != 0) {
			(void)codec_output_drop_tx();
			codec_output_purge_pending_blocks();
			return ret;
		}

		preloaded_blocks++;
	}

	if (preloaded_blocks == 0U) {
		return -ENOENT;
	}

	LOG_INF("Starting I2S TX with %u buffered frame blocks (%u us target)",
		(unsigned int)(preloaded_blocks + k_msgq_num_used_get(&codec_output_msgq)),
		target_buffer_us);

	ret = i2s_trigger(codec_i2s, I2S_DIR_TX, I2S_TRIGGER_START);
	if (ret != 0) {
		LOG_ERR("Failed to start I2S TX: %d", ret);
		(void)codec_output_drop_tx();
		codec_output_purge_pending_blocks();
		return ret;
	}

	tx_started = true;

	return 0;
}

static int codec_output_queue_block(void *mem_block, size_t size)
{
	struct codec_output_block block = {
		.mem_block = mem_block,
		.size = size,
		.session_id = current_session_id,
	};
	const size_t startup_block_target = codec_output_get_startup_block_target(size);
	int ret;

	ret = k_msgq_put(&codec_output_msgq, &block, K_NO_WAIT);
	if (ret != 0) {
		codec_output_free_block(&block);
		return ret;
	}

	current_stereo_frame_bytes = size;

	if (!tx_started && k_msgq_num_used_get(&codec_output_msgq) >= startup_block_target) {
		k_sem_give(&codec_output_start_sem);
	}

	return 0;
}

static bool codec_output_frames_ready(void)
{
	if (decoded_sdu.mono_frames_cnt != 0U) {
		return true;
	}

	if (decoded_sdu.left_frames_cnt == 0U || decoded_sdu.right_frames_cnt == 0U) {
		return false;
	}

	return decoded_sdu.left_frames_cnt == decoded_sdu.right_frames_cnt;
}

static int codec_output_send_frames(void)
{
	const bool is_left_only =
		(decoded_sdu.right_frames_cnt == 0U) && (decoded_sdu.mono_frames_cnt == 0U);
	const bool is_right_only =
		(decoded_sdu.left_frames_cnt == 0U) && (decoded_sdu.mono_frames_cnt == 0U);
	const bool is_mono_only =
		(decoded_sdu.left_frames_cnt == 0U) && (decoded_sdu.right_frames_cnt == 0U);
	const bool is_single_channel = is_left_only || is_right_only || is_mono_only;
	const size_t frame_cnt = MAX(decoded_sdu.mono_frames_cnt,
				     MAX(decoded_sdu.left_frames_cnt, decoded_sdu.right_frames_cnt));
	const size_t mono_samples = decoded_sdu.frame_size_bytes / sizeof(int16_t);
	const size_t stereo_frame_bytes = decoded_sdu.frame_size_bytes * 2U;

	if (frame_cnt == 0U || decoded_sdu.frame_size_bytes == 0U || mono_samples == 0U) {
		codec_output_clear_frames();
		return 0;
	}

	if (!initialized) {
		codec_output_clear_frames();
		return -EACCES;
	}

	for (size_t i = 0U; i < frame_cnt; i++) {
		int16_t *stereo_frame;
		const int16_t *right_frame = decoded_sdu.right_frames[i];
		const int16_t *left_frame = decoded_sdu.left_frames[i];
		const int16_t *mono_frame = decoded_sdu.left_frames[i];
		int ret;

		ret = k_mem_slab_alloc(&i2s_tx_slab, (void **)&stereo_frame, K_NO_WAIT);
		if (ret != 0) {
			LOG_WRN("No I2S TX block available: %d", ret);
			codec_output_clear_frames();
			return ret;
		}

		for (size_t j = 0U; j < mono_samples; j++) {
			if (is_single_channel) {
				const int16_t sample =
					is_left_only ? left_frame[j] :
					is_right_only ? right_frame[j] : mono_frame[j];

				stereo_frame[j * 2U] = sample;
				stereo_frame[j * 2U + 1U] = sample;
			} else {
				stereo_frame[j * 2U] = left_frame[j];
				stereo_frame[j * 2U + 1U] = right_frame[j];
			}
		}

		ret = codec_output_queue_block(stereo_frame, stereo_frame_bytes);
		if (ret != 0) {
			LOG_WRN("Failed to queue PCM block for I2S worker: %d", ret);
			codec_output_clear_frames();
			return ret;
		}
	}

	codec_output_clear_frames();

	return 0;
}

static void codec_output_thread_func(void *arg1, void *arg2, void *arg3)
{
	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);
	ARG_UNUSED(arg3);

	while (true) {
		struct codec_output_block block;
		uint32_t frame_duration_us;
		k_timeout_t wait_timeout;
		int ret;

		if (!initialized || !tx_started) {
			k_sem_take(&codec_output_start_sem, K_FOREVER);

			if (!initialized || tx_started) {
				continue;
			}

			ret = codec_output_start_tx_if_ready();
			if (ret != 0 && ret != -ENOENT) {
				LOG_WRN("Failed to start buffered I2S TX: %d", ret);
			}

			continue;
		}

		frame_duration_us = codec_output_get_frame_duration_us(current_stereo_frame_bytes);
		if (frame_duration_us == 0U) {
			wait_timeout = K_MSEC(5);
		} else {
			wait_timeout = K_USEC(MAX(frame_duration_us / 2U, 1000U));
		}

		ret = k_msgq_get(&codec_output_msgq, &block, wait_timeout);
		if (ret != 0) {
			if (current_stereo_frame_bytes != 0U) {
				ret = codec_output_alloc_silence_block(current_stereo_frame_bytes,
								       &block);
				if (ret == 0) {
					ret = codec_output_write_block_to_i2s(&block);
					if (ret != 0) {
						(void)codec_output_drop_tx();
						codec_output_purge_pending_blocks();
					}
				}
			}

			continue;
		}

		if (!initialized || block.session_id != current_session_id) {
			codec_output_free_block(&block);
			continue;
		}

		if (!tx_started) {
			ret = k_msgq_put(&codec_output_msgq, &block, K_NO_WAIT);
			if (ret != 0) {
				codec_output_free_block(&block);
			}

			continue;
		}

		ret = codec_output_write_block_to_i2s(&block);
		if (ret != 0) {
			(void)codec_output_drop_tx();
			codec_output_purge_pending_blocks();
		}
	}
}

int codec_output_add_frame(enum bt_audio_location chan_allocation, const int16_t *frame,
			   size_t frame_size, uint32_t ts)
{
	const bool is_left = (chan_allocation & BT_AUDIO_LOCATION_FRONT_LEFT) != 0;
	const bool is_right = (chan_allocation & BT_AUDIO_LOCATION_FRONT_RIGHT) != 0;
	const bool is_mono = chan_allocation == BT_AUDIO_LOCATION_MONO_AUDIO;
	const uint8_t ts_jitter_us = 100U;

	if (frame == NULL || frame_size == 0U ||
	    frame_size > (LC3_MAX_NUM_SAMPLES_MONO * sizeof(int16_t))) {
		return -EINVAL;
	}

	if (bt_audio_get_chan_count(chan_allocation) != 1U) {
		return -EINVAL;
	}

	if (((is_left || is_right) && decoded_sdu.mono_frames_cnt != 0U) ||
	    (is_mono &&
	     (decoded_sdu.left_frames_cnt != 0U || decoded_sdu.right_frames_cnt != 0U))) {
		return -EINVAL;
	}

	if (decoded_sdu.frame_size_bytes != 0U && decoded_sdu.frame_size_bytes != frame_size) {
		(void)codec_output_send_frames();
	}

	if (ts + ts_jitter_us < decoded_sdu.ts && !ts_overflowed(ts)) {
		return -ENOEXEC;
	} else if (ts > decoded_sdu.ts + ts_jitter_us || ts_overflowed(ts)) {
		(void)codec_output_send_frames();
	} else {
		bool send = false;

		if (is_left && decoded_sdu.left_frames_cnt > decoded_sdu.right_frames_cnt) {
			send = true;
		} else if (is_right && decoded_sdu.right_frames_cnt > decoded_sdu.left_frames_cnt) {
			send = true;
		} else if (is_mono) {
			send = true;
		}

		if (send) {
			(void)codec_output_send_frames();
		}
	}

	if (is_left) {
		if (decoded_sdu.left_frames_cnt >= ARRAY_SIZE(decoded_sdu.left_frames)) {
			return -ENOMEM;
		}

		memcpy(decoded_sdu.left_frames[decoded_sdu.left_frames_cnt++], frame, frame_size);
	} else if (is_right) {
		if (decoded_sdu.right_frames_cnt >= ARRAY_SIZE(decoded_sdu.right_frames)) {
			return -ENOMEM;
		}

		memcpy(decoded_sdu.right_frames[decoded_sdu.right_frames_cnt++], frame, frame_size);
	} else if (is_mono) {
		if (decoded_sdu.mono_frames_cnt >= ARRAY_SIZE(decoded_sdu.left_frames)) {
			return -ENOMEM;
		}

		memcpy(decoded_sdu.left_frames[decoded_sdu.mono_frames_cnt++], frame, frame_size);
	} else {
		return -EINVAL;
	}

	decoded_sdu.frame_size_bytes = frame_size;
	decoded_sdu.ts = ts;

	if (codec_output_frames_ready()) {
		return codec_output_send_frames();
	}

	return 0;
}

void codec_output_clear_frames(void)
{
	decoded_sdu.mono_frames_cnt = 0U;
	decoded_sdu.right_frames_cnt = 0U;
	decoded_sdu.left_frames_cnt = 0U;
	decoded_sdu.frame_size_bytes = 0U;
	decoded_sdu.ts = 0U;
}

int codec_output_init(uint32_t sample_rate_hz, uint32_t presentation_delay_us)
{
	struct i2s_config cfg = {
		.word_size = 16,
		.channels = 2,
		.format = I2S_FMT_DATA_FORMAT_I2S,
		.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER,
		.frame_clk_freq = sample_rate_hz,
		.mem_slab = &i2s_tx_slab,
		.block_size = I2S_CODEC_BLOCK_SIZE_BYTES,
		.timeout = I2S_CODEC_TIMEOUT_MS,
	};
	int ret;

	if (sample_rate_hz == 0U) {
		return -EINVAL;
	}

	if (!device_is_ready(codec_i2s)) {
		return -ENODEV;
	}

	if (!worker_initialized) {
		k_thread_create(&codec_output_thread, codec_output_thread_stack,
				K_KERNEL_STACK_SIZEOF(codec_output_thread_stack),
				codec_output_thread_func, NULL, NULL, NULL,
				K_PRIO_PREEMPT(CODEC_OUTPUT_THREAD_PRIORITY), 0, K_NO_WAIT);
		k_thread_name_set(&codec_output_thread, "codec_output");
		worker_initialized = true;
	}

	if (initialized && current_sample_rate_hz == sample_rate_hz) {
		configured_presentation_delay_us = presentation_delay_us;
		return 0;
	}

	if (!pmic_initialized) {
		ret = pmic_init();
		if (ret != 0) {
			LOG_ERR("Failed to initialize PMIC: %d", ret);
			return ret;
		}

		pmic_initialized = true;
	}

	if (initialized) {
		ret = codec_output_drop_tx();
		if (ret != 0) {
			LOG_ERR("Failed to stop active I2S TX before reconfigure: %d", ret);
			return ret;
		}
	}

	codec_output_purge_pending_blocks();
	codec_output_clear_frames();
	k_sem_reset(&codec_output_start_sem);
	current_session_id = next_session_id(current_session_id);

	ret = codec_init(sample_rate_hz);
	if (ret != 0) {
		LOG_ERR("Failed to initialize SGTL5000: %d", ret);
		return ret;
	}

	ret = i2s_configure(codec_i2s, I2S_DIR_TX, &cfg);
	if (ret != 0) {
		LOG_ERR("Failed to configure I2S TX: %d", ret);
		return ret;
	}

	initialized = true;
	tx_started = false;
	current_sample_rate_hz = sample_rate_hz;
	configured_presentation_delay_us = presentation_delay_us;
	current_stereo_frame_bytes = 0U;

	return 0;
}

int codec_output_stop(void)
{
	int ret = 0;

	if (initialized) {
		ret = codec_output_drop_tx();
	}

	codec_output_purge_pending_blocks();
	codec_output_clear_frames();
	k_sem_reset(&codec_output_start_sem);
	initialized = false;
	tx_started = false;
	current_sample_rate_hz = 0U;
	configured_presentation_delay_us = 0U;
	current_session_id = next_session_id(current_session_id);
	current_stereo_frame_bytes = 0U;

	return ret;
}
