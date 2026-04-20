/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SAMPLE_BAP_BROADCAST_SINK_I2C_HELPERS_H
#define SAMPLE_BAP_BROADCAST_SINK_I2C_HELPERS_H

#include <stdint.h>

#include <zephyr/device.h>

int i2c_reg_write16(const struct device *i2c, uint8_t addr, uint16_t reg, uint8_t val);
int i2c_reg_read16(const struct device *i2c, uint8_t addr, uint16_t reg, uint8_t *val);
int i2c_reg_write16_16(const struct device *i2c, uint8_t addr, uint16_t reg, uint16_t val);
int i2c_reg_read16_16(const struct device *i2c, uint8_t addr, uint16_t reg, uint16_t *val);
int i2c_reg_update16_16(const struct device *i2c, uint8_t addr, uint16_t reg,
			uint16_t clear_mask, uint16_t set_value);

#endif /* SAMPLE_BAP_BROADCAST_SINK_I2C_HELPERS_H */
