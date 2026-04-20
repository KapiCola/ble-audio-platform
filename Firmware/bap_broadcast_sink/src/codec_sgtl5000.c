/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/sys/printk.h>

#include "codec_sgtl5000.h"
#include "i2c_helpers.h"

#define SGTL5000_ADDR            0x0A

#define CHIP_DIG_POWER           0x0002
#define CHIP_CLK_CTRL            0x0004
#define CHIP_I2S_CTRL            0x0006
#define CHIP_SSS_CTRL            0x000A
#define CHIP_ADCDAC_CTRL         0x000E
#define CHIP_DAC_VOL             0x0010
#define CHIP_ANA_ADC_CTRL        0x0020
#define CHIP_ANA_CTRL            0x0024
#define CHIP_LINREG_CTRL         0x0026
#define CHIP_REF_CTRL            0x0028
#define CHIP_LINE_OUT_CTRL       0x002C
#define CHIP_LINE_OUT_VOL        0x002E
#define CHIP_ANA_POWER           0x0030
#define CHIP_SHORT_CTRL          0x003C

#define BIT_SHIFT(x)             BIT(x)

#define TRY(_expr)                                                                          \
	do {                                                                                \
		int _ret = (_expr);                                                         \
                                                                                         \
		if (_ret != 0) {                                                            \
			return _ret;                                                        \
		}                                                                            \
	} while (false)

static const struct device *const codec_i2c = DEVICE_DT_GET(DT_NODELABEL(i2c1));

static int codec_write(uint16_t reg, uint16_t val)
{
	return i2c_reg_write16_16(codec_i2c, SGTL5000_ADDR, reg, val);
}

static int codec_mod(uint16_t reg, uint16_t clear_mask, uint16_t set_mask)
{
	return i2c_reg_update16_16(codec_i2c, SGTL5000_ADDR, reg, clear_mask, set_mask);
}

static int codec_get_clk_ctrl_value(uint32_t sample_rate_hz, uint16_t *clk_ctrl)
{
	switch (sample_rate_hz) {
	case 16000U:
		/* SYS_FS = 32 kHz, RATE_MODE = 1/2, MCLK = 384 * SYS_FS = 12.288 MHz */
		*clk_ctrl = 0x0011;
		return 0;
	case 24000U:
		/* SYS_FS = 48 kHz, RATE_MODE = 1/2, MCLK = 256 * SYS_FS = 12.288 MHz */
		*clk_ctrl = 0x0018;
		return 0;
	case 32000U:
		/* Original codec_test configuration: SYS_FS = 32 kHz, MCLK = 384 * Fs. */
		*clk_ctrl = 0x0001;
		return 0;
	case 48000U:
		/* SYS_FS = 48 kHz, MCLK = 256 * Fs = 12.288 MHz. */
		*clk_ctrl = 0x0008;
		return 0;
	default:
		return -EINVAL;
	}
}

int codec_init(uint32_t sample_rate_hz)
{
	uint16_t clk_ctrl;

	if (!device_is_ready(codec_i2c)) {
		printk("SGTL5000 I2C not ready\n");
		return -ENODEV;
	}

	if (codec_get_clk_ctrl_value(sample_rate_hz, &clk_ctrl) != 0) {
		printk("Unsupported SGTL5000 sample rate: %u\n", sample_rate_hz);
		return -EINVAL;
	}

	/* Power up bias/reference and keep only the required analog blocks enabled. */
	TRY(codec_write(CHIP_ANA_POWER, 0x4060));
	TRY(codec_mod(CHIP_LINREG_CTRL, ~(BIT_SHIFT(5) | BIT_SHIFT(6)),
		      (BIT_SHIFT(5) | BIT_SHIFT(6))));

	TRY(codec_write(CHIP_REF_CTRL, 0x01DE));
	TRY(codec_mod(CHIP_LINE_OUT_CTRL, ~0x0322, 0x0322));
	TRY(codec_mod(CHIP_REF_CTRL, ~BIT_SHIFT(0), BIT_SHIFT(0)));
	TRY(codec_write(CHIP_SHORT_CTRL, 0x1106));
	TRY(codec_write(CHIP_ANA_CTRL, 0x0137));

	TRY(codec_mod(CHIP_ANA_POWER, ~0x000B, 0x000B));
	TRY(codec_mod(CHIP_ANA_POWER, ~0x0080, 0x0080));
	k_msleep(100);

	/* Power up I2S input and DAC path. */
	TRY(codec_write(CHIP_DIG_POWER, 0x0063));

	TRY(codec_write(CHIP_LINE_OUT_VOL, 0x0E0E));

	/* Program sample-rate-specific SYS_FS/RATE_MODE/MCLK ratio for 12.288 MHz ACLK. */
	TRY(codec_mod(CHIP_CLK_CTRL, ~0x003F, clk_ctrl));

	/* 16-bit samples, I2S format, codec as slave. */
	TRY(codec_write(CHIP_I2S_CTRL, 0x0030));

	/* Route I2S input to DAC and ADC to I2S_DOUT (unused but harmless). */
	TRY(codec_write(CHIP_SSS_CTRL, 0x0010));

	TRY(codec_mod(CHIP_ANA_CTRL, ~0x0101, 0x0000));
	TRY(codec_write(CHIP_ANA_ADC_CTRL, 0x0144));

	TRY(codec_mod(CHIP_ANA_CTRL, ~0x0100, 0x0100));
	TRY(codec_write(CHIP_DAC_VOL, 0x3C3C));
	TRY(codec_mod(CHIP_ADCDAC_CTRL, ~0x000C, 0x0000));
	TRY(codec_mod(CHIP_ADCDAC_CTRL, ~0x0300, 0x0300));
	TRY(codec_mod(CHIP_ANA_CTRL, ~0x0100, 0x0000));

	return 0;
}
