#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include "i2c_helpers.h"

/* ===================== CONFIG ===================== */

#define NPM1304_ADDR        0x6B

/* ===================== BUCKs ===================== */

#define BUCK_STATUS_REG     0x0434
#define BUCK1_VOUT_REG      0x0408
#define BUCK2_VOUT_REG      0x040A
#define BUCK_SW_CTRL_REG    0x040F

#define BUCK1_VOUT_VAL      24U
#define BUCK2_VOUT_VAL      24U
#define BUCK_SW_ENABLE_ALL  0x03

/* ===================== LDOs ====================== */

#define LDO_STATUS_REG      0x0804
#define LDO1_MODE_REG       0x0808
#define LDO2_MODE_REG       0x0809
#define LDO1_VOUT_REG       0x080C
#define LDO2_VOUT_REG       0x080D
#define LDO1_SET_REG        0x0800
#define LDO2_SET_REG        0x0802

#define LDO1_VOUT_VAL       10U
#define LDO2_VOUT_VAL       8U

#define LDO_MODE_ENABLE     1U
#define LDO_ENABLE          1U

/* ===================== LEDs ====================== */

#define LED0_MODE_REG       0x0A00
#define LED1_MODE_REG       0x0A01
#define LED2_MODE_REG       0x0A02

#define LED0_SET_REG        0x0A03
#define LED0_CLR_REG        0x0A04
#define LED1_SET_REG        0x0A05
#define LED1_CLR_REG        0x0A06
#define LED2_SET_REG        0x0A07
#define LED2_CLR_REG        0x0A08

#define LED_MODE_HOST       0x02

static const uint16_t led_mode_regs[] = {
    LED0_MODE_REG,
    LED1_MODE_REG,
    LED2_MODE_REG
};

static const uint16_t led_set_regs[] = {
    LED0_SET_REG,
    LED1_SET_REG,
    LED2_SET_REG
};

static const uint16_t led_clr_regs[] = {
    LED0_CLR_REG,
    LED1_CLR_REG,
    LED2_CLR_REG
};

/* ===================== I2C ====================== */

static const struct device *i2c = DEVICE_DT_GET(DT_NODELABEL(i2c2));

/* ===================== HELPERS ===================== */

#define TRY(x) do { int r = (x); if (r) return r; } while (0)

static int pmic_reg_write(uint16_t reg, uint8_t val)
{
    return i2c_reg_write16(i2c, NPM1304_ADDR, reg, val);
}

static int pmic_reg_read(uint16_t reg, uint8_t *val)
{
    return i2c_reg_read16(i2c, NPM1304_ADDR, reg, val);
}

/* ===================== INIT SECTIONS ===================== */

static int pmic_init_bucks(void)
{
    TRY(pmic_reg_write(BUCK1_VOUT_REG, 10U));
    k_msleep(2);

    TRY(pmic_reg_write(BUCK2_VOUT_REG, BUCK2_VOUT_VAL));
    k_msleep(2);

    TRY(pmic_reg_write(BUCK_SW_CTRL_REG, BUCK_SW_ENABLE_ALL));
    k_msleep(2);

    if (BUCK1_VOUT_VAL>10)
    {
        for (unsigned int voltage = 11U; voltage <= BUCK1_VOUT_VAL; ++voltage)
        {
            TRY(pmic_reg_write(BUCK1_VOUT_REG, voltage));
            k_msleep(2);
        }
    }else
    {
        printk("Forbidden output voltage value!");
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
    
    for (unsigned int voltage = 2U; voltage <= 22U; ++voltage)
        {
            TRY(pmic_reg_write(LDO1_VOUT_REG, voltage));
            k_msleep(10);
        }

    TRY(pmic_reg_write(LDO2_SET_REG, LDO_ENABLE));
    k_msleep(20);
  
    return 0;
}

static int pmic_init_leds(void)
{
    for (int i = 0; i < 3; i++) {
        TRY(pmic_reg_write(led_mode_regs[i], LED_MODE_HOST));
        k_msleep(2);
    }

    return 0;
}

/* ===================== PUBLIC API ===================== */

int pmic_init(void)
{
    if (!device_is_ready(i2c)) {
        printk("PMIC: I2C not ready\n");
        return -ENODEV;
    }

    int ret;

    ret = pmic_init_bucks();
    if (ret) goto error;

    
    ret = pmic_init_ldos();
    if (ret) goto error;
    

    ret = pmic_init_leds();
    if (ret) goto error;

    printk("PMIC init OK\n");
    return 0;

error:
    printk("PMIC init failed: %d\n", ret);
    return ret;
}

void pmic_debug_dump(void)
{
    uint8_t val;

    printk("\n=== PMIC STATUS ===\n");

    if (!pmic_reg_read(BUCK_STATUS_REG, &val))
        printk("BUCK STATUS: 0x%02X\n", val);
    else
        printk("BUCK STATUS: ERROR\n");

    if (!pmic_reg_read(LDO_STATUS_REG, &val))
        printk("LDO STATUS: 0x%02X\n", val);
    else
        printk("LDO STATUS: ERROR\n");

    if (!pmic_reg_read(BUCK1_VOUT_REG, &val))
        printk("BUCK1 VOUT: %d\n", val);

    if (!pmic_reg_read(BUCK2_VOUT_REG, &val))
        printk("BUCK2 VOUT: %d\n", val);

    if (!pmic_reg_read(LDO1_VOUT_REG, &val))
        printk("LDO1 VOUT: %d\n", val);

    if (!pmic_reg_read(LDO2_VOUT_REG, &val))
        printk("LDO2 VOUT: %d\n", val);

    printk("====================\n\n");
}

/* ===================== LED CONTROL ===================== */

void pmic_led_on(int led)
{
    if (led < 0 || led > 2)
        return;

    pmic_reg_write(led_set_regs[led], 1);
}

void pmic_led_off(int led)
{
    if (led < 0 || led > 2)
        return;

    pmic_reg_write(led_clr_regs[led], 1);
}