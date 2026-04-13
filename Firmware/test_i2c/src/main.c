#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/sys/printk.h>
#include "pmic.h"
#include "i2c_helpers.h"
#include <zephyr/drivers/pwm.h>

#define SGTL5000_ADDR 0x0A

const struct device *codec = DEVICE_DT_GET(DT_NODELABEL(i2c1));

const struct device *pwm = DEVICE_DT_GET(DT_NODELABEL(pwm0));

int start_mclk(void)
{
    uint32_t period_ns = 62;   // ~16 MHz (62.5 ns)
    uint32_t pulse_ns  = 31;   // 50%

    return pwm_set(pwm, 0, period_ns, pulse_ns, PWM_POLARITY_NORMAL);
}

void i2c_scan(void)
{
    printk("Scanning I2C bus...\n");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        uint8_t dummy;
        int ret = i2c_read(codec, &dummy, 1, addr);
        if (ret == 0) {
            printk("  Found device at 0x%02X\n", addr);
        }
    }
    printk("Scan done.\n");
}

int main(void)
{
    int ret = pmic_init();

    /*
    if (ret) {
        return 0;
    }

    pmic_debug_dump();

    pmic_led_on(0);

    if (!device_is_ready(pwm)) {
        printk("PWM device not ready\n");
        return -1;
    }

    ret = start_mclk();
    if (ret < 0) {
        printk("Failed to start MCLK: %d\n", ret);
        return ret;
    }

    printk("MCLK 16 MHz started\n");

    k_msleep(50);

    uint16_t val;

    ret = i2c_reg_read16_16(codec, SGTL5000_ADDR, 0x0000, &val);

    if (ret == 0) {
        printk("SGTL5000 detected! CHIP_ID=0x%04X\n", val);
    } else {
        printk("SGTL5000 not responding (%d)\n", ret);
    }

    i2c_scan();
    */
    while (1) {
        k_msleep(1000);
        //printk("Very long text to test uart blinking you know man, no hard feelings\n");
    }
}
