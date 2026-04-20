/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SAMPLE_BAP_BROADCAST_SINK_CODEC_OUTPUT_H
#define SAMPLE_BAP_BROADCAST_SINK_CODEC_OUTPUT_H

#include <stddef.h>
#include <stdint.h>

#include <zephyr/bluetooth/audio/audio.h>

int codec_output_init(uint32_t sample_rate_hz, uint32_t presentation_delay_us);
int codec_output_stop(void);
int codec_output_add_frame(enum bt_audio_location chan_allocation, const int16_t *frame,
			   size_t frame_size, uint32_t ts);
void codec_output_clear_frames(void);

#endif /* SAMPLE_BAP_BROADCAST_SINK_CODEC_OUTPUT_H */
