#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include "pmic.h"
#include "i2c_helpers.h"
#include "codec_sgtl5000.h"

#define SAMPLE_RATE 48000
#define BLOCK_SIZE 2048
#define BLOCK_BYTES (BLOCK_SIZE * sizeof(int16_t))
#define NUM_BLOCKS 32
#define PRELOAD_BLOCKS  4

K_MEM_SLAB_DEFINE(i2s_slab, BLOCK_BYTES, NUM_BLOCKS, 4);

const struct device *codec_i2s = DEVICE_DT_GET(DT_NODELABEL(i2s0));

int i2s_start(void)
{
    struct i2s_config cfg = {
        .word_size      = 16,
        .channels       = 2,
        .format         = I2S_FMT_DATA_FORMAT_I2S,
        .options        = I2S_OPT_BIT_CLK_MASTER |
                          I2S_OPT_FRAME_CLK_MASTER,
        .frame_clk_freq = SAMPLE_RATE,
        .mem_slab       = &i2s_slab,
        .block_size     = BLOCK_BYTES,
        .timeout        = 1000,
    };

    int ret;

    ret = i2s_configure(codec_i2s, I2S_DIR_RX, &cfg);
    if (ret < 0) return ret;

    ret = i2s_configure(codec_i2s, I2S_DIR_TX, &cfg);
    if (ret < 0) return ret;

    return 0;
}


int main(void)
{
    int ret;
    void *blk;
    size_t size;

    ret = pmic_init();
    if (ret) return -1;

    pmic_debug_dump();

    codec_init();

    ret = i2s_start();
    if (ret) return -1;

    /* preload TX silence */
    for (int i = 0; i < PRELOAD_BLOCKS; i++) {
        ret = k_mem_slab_alloc(&i2s_slab, &blk, K_FOREVER);
        if (ret) return -1;

        memset(blk, 0, BLOCK_BYTES);

        ret = i2s_write(codec_i2s, blk, BLOCK_BYTES);
        if (ret) return -1;
    }

    /* start streams */
    ret = i2s_trigger(codec_i2s, I2S_DIR_BOTH, I2S_TRIGGER_START);
    if (ret) return -1;

    /* zero-copy pass-through */
    while (1) {
        ret = i2s_read(codec_i2s, &blk, &size);
        if (ret < 0) {
            printk("RX err %d\n", ret);
            break;
        }

        ret = i2s_write(codec_i2s, blk, size);
        if (ret < 0) {
            printk("TX err %d\n", ret);
            break;
        }
    }

    return 0;
}
