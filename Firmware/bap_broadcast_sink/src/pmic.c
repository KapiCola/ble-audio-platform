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
#include <zephyr/sys/printk.h>

#include "pmic.h"

/* nPM1304 PMIC on the custom audio board */
#define NPM1304_ADDR            0x6B

#define BUCK1_VOUT_REG          0x0408
#define BUCK2_VOUT_REG          0x040A
#define BUCK_SW_CTRL_REG        0x040F
#define BUCK1_VOUT_VAL          24U
#define BUCK2_VOUT_VAL          24U
#define BUCK_SW_ENABLE_ALL      0x03

#define LDO1_MODE_REG           0x0808
#define LDO2_MODE_REG           0x0809
#define LDO1_VOUT_REG           0x080C
#define LDO2_VOUT_REG           0x080D
#define LDO1_SET_REG            0x0800
#define LDO2_SET_REG            0x0802
#define LDO1_VOUT_VAL           10U
#define LDO2_VOUT_VAL           8U
#define LDO_MODE_ENABLE         1U
#define LDO_ENABLE              1U

#define LED0_MODE_REG           0x0A00
#define LED1_MODE_REG           0x0A01
#define LED2_MODE_REG           0x0A02
#define LED_MODE_HOST           0x02

#define TRY(_expr)                                                                          \
	do {                                                                                \
		int _ret = (_expr);                                                         \
                                                                                         \
		if (_ret != 0) {                                                            \
			return _ret;                                                        \
		}                                                                            \
	} while (false)

static const struct device *const pmic_i2c = DEVICE_DT_GET(DT_NODELABEL(i2c2));

static int pmic_reg_write(uint16_t reg, uint8_t val)
{
	uint8_t buf[3];

	buf[0] = (uint8_t)(reg >> 8);
	buf[1] = (uint8_t)reg;
	buf[2] = val;

	return i2c_write(pmic_i2c, buf, sizeof(buf), NPM1304_ADDR);
}

static int pmic_init_bucks(void)
{
	TRY(pmic_reg_write(BUCK1_VOUT_REG, 10U));
	k_msleep(2);

	TRY(pmic_reg_write(BUCK2_VOUT_REG, BUCK2_VOUT_VAL));
	k_msleep(2);

	TRY(pmic_reg_write(BUCK_SW_CTRL_REG, BUCK_SW_ENABLE_ALL));
	k_msleep(2);

	for (uint8_t voltage = 11U; voltage <= BUCK1_VOUT_VAL; voltage++) {
		TRY(pmic_reg_write(BUCK1_VOUT_REG, voltage));
		k_msleep(2);
	}

	return 0;
}

static int pmic_init_ldos(void)
{
	TRY(pmic_reg_write(LDO1_VOUT_REG, LDO1_VOUT_VAL));
	k_msleep(2);

	TRY(pmic_reg_write(LDO2_VOUT_REG, LDO2_VOUT_VAL));
	k_msleep(2);

	TRY(pmic_reg_write(LDO1_MODE_REG, LDO_MODE_ENABLE));
	k_msleep(2);

	TRY(pmic_reg_write(LDO2_MODE_REG, LDO_MODE_ENABLE));
	k_msleep(2);

	TRY(pmic_reg_write(LDO1_SET_REG, LDO_ENABLE));
	k_msleep(2);

	for (uint8_t voltage = 2U; voltage <= 22U; voltage++) {
		TRY(pmic_reg_write(LDO1_VOUT_REG, voltage));
		k_msleep(10);
	}

	TRY(pmic_reg_write(LDO2_SET_REG, LDO_ENABLE));
	k_msleep(20);

	return 0;
}

static int pmic_init_leds(void)
{
	TRY(pmic_reg_write(LED0_MODE_REG, LED_MODE_HOST));
	k_msleep(2);
	TRY(pmic_reg_write(LED1_MODE_REG, LED_MODE_HOST));
	k_msleep(2);
	TRY(pmic_reg_write(LED2_MODE_REG, LED_MODE_HOST));
	k_msleep(2);

	return 0;
}

int pmic_init(void)
{
	int ret;

	if (!device_is_ready(pmic_i2c)) {
		printk("PMIC I2C not ready\n");
		return -ENODEV;
	}

	ret = pmic_init_bucks();
	if (ret != 0) {
		printk("PMIC BUCK init failed: %d\n", ret);
		return ret;
	}

	ret = pmic_init_ldos();
	if (ret != 0) {
		printk("PMIC LDO init failed: %d\n", ret);
		return ret;
	}

	ret = pmic_init_leds();
	if (ret != 0) {
		printk("PMIC LED init failed: %d\n", ret);
		return ret;
	}

	return 0;
}
