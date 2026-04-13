#include <zephyr/drivers/i2c.h>
#include "i2c_helpers.h"

int i2c_reg_write16(const struct device *i2c,
                    uint8_t addr,
                    uint16_t reg,
                    uint8_t val)
{
    uint8_t buf[3];

    buf[0] = (reg >> 8) & 0xFF;
    buf[1] = reg & 0xFF;
    buf[2] = val;

    return i2c_write(i2c, buf, 3, addr);
}

int i2c_reg_read16(const struct device *i2c,
                   uint8_t addr,
                   uint16_t reg,
                   uint8_t *val)
{
    uint8_t addr_buf[2];

    addr_buf[0] = (reg >> 8) & 0xFF;
    addr_buf[1] = reg & 0xFF;

    return i2c_write_read(i2c, addr, addr_buf, 2, val, 1);
}

int i2c_reg_write16_16(const struct device *i2c,
                       uint8_t addr,
                       uint16_t reg,
                       uint16_t val)
{
    uint8_t buf[4];

    buf[0] = (reg >> 8) & 0xFF;
    buf[1] = reg & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;

    return i2c_write(i2c, buf, 4, addr);
}

int i2c_reg_read16_16(const struct device *i2c,
                      uint8_t addr,
                      uint16_t reg,
                      uint16_t *val)
{
    uint8_t addr_buf[2];
    uint8_t data[2];
    int ret;

    addr_buf[0] = (reg >> 8) & 0xFF;
    addr_buf[1] = reg & 0xFF;

    ret = i2c_write_read(i2c, addr, addr_buf, 2, data, 2);
    if (ret) {
        return ret;
    }

    *val = (data[0] << 8) | data[1];

    return 0;
}

int i2c_reg_update16_16(const struct device *i2c,
                        uint8_t addr,
                        uint16_t reg,
                        uint16_t clear_mask,
                        uint16_t set_mask)
{
    int ret;
    uint16_t val;

    ret = i2c_reg_read16_16(i2c, addr, reg, &val);
    if (ret) return ret;

    val = (val & ~clear_mask) | set_mask;

    return i2c_reg_write16_16(i2c, addr, reg, val);
}