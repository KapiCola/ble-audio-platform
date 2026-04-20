/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/drivers/i2c.h>

#include "i2c_helpers.h"

int i2c_reg_write16(const struct device *i2c, uint8_t addr, uint16_t reg, uint8_t val)
{
	uint8_t buf[3];

	buf[0] = (uint8_t)(reg >> 8);
	buf[1] = (uint8_t)reg;
	buf[2] = val;

	return i2c_write(i2c, buf, sizeof(buf), addr);
}

int i2c_reg_read16(const struct device *i2c, uint8_t addr, uint16_t reg, uint8_t *val)
{
	uint8_t reg_buf[2];

	reg_buf[0] = (uint8_t)(reg >> 8);
	reg_buf[1] = (uint8_t)reg;

	return i2c_write_read(i2c, addr, reg_buf, sizeof(reg_buf), val, sizeof(*val));
}

int i2c_reg_write16_16(const struct device *i2c, uint8_t addr, uint16_t reg, uint16_t val)
{
	uint8_t buf[4];

	buf[0] = (uint8_t)(reg >> 8);
	buf[1] = (uint8_t)reg;
	buf[2] = (uint8_t)(val >> 8);
	buf[3] = (uint8_t)val;

	return i2c_write(i2c, buf, sizeof(buf), addr);
}

int i2c_reg_read16_16(const struct device *i2c, uint8_t addr, uint16_t reg, uint16_t *val)
{
	uint8_t reg_buf[2];
	uint8_t data[2];
	int ret;

	reg_buf[0] = (uint8_t)(reg >> 8);
	reg_buf[1] = (uint8_t)reg;

	ret = i2c_write_read(i2c, addr, reg_buf, sizeof(reg_buf), data, sizeof(data));
	if (ret != 0) {
		return ret;
	}

	*val = ((uint16_t)data[0] << 8) | data[1];

	return 0;
}

int i2c_reg_update16_16(const struct device *i2c, uint8_t addr, uint16_t reg,
			uint16_t clear_mask, uint16_t set_value)
{
	uint16_t val;
	int ret;

	ret = i2c_reg_read16_16(i2c, addr, reg, &val);
	if (ret != 0) {
		return ret;
	}

	val = (val & clear_mask) | set_value;

	return i2c_reg_write16_16(i2c, addr, reg, val);
}
