#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include "pmic.h"

#define SLEEP_TIME_MS (1000)

#define NUM_BUTTONS 5
#define NUM_LEDS    9

static int64_t last_press[NUM_BUTTONS];

static const struct gpio_dt_spec buttons[NUM_BUTTONS] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw3), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(sw4), gpios),
};

static const struct gpio_dt_spec leds[NUM_LEDS] = {
    GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led4), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led5), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led6), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led7), gpios),
    GPIO_DT_SPEC_GET(DT_ALIAS(led8), gpios),
};

static struct gpio_callback button_cbs[NUM_BUTTONS];

void button_pressed(const struct device *dev,
                    struct gpio_callback *cb,
                    uint32_t pins)
{
        int64_t now = k_uptime_get();
          
        for (int i = 0; i < NUM_BUTTONS; i++) {
                if ((cb == &button_cbs[i]) &&(pins & BIT(buttons[i].pin))) {

                        if (now - last_press[i] > 200) {
                                gpio_pin_toggle_dt(&leds[i+4]);
                                last_press[i] = now;
                        }

                break;
                }
        }
}

int main(void)
{
        int ret;
        //Inicjalizacja przetwornicy
        if (pmic_init() != 0) {
                return -1;
        }

        for (int i = 0; i < NUM_LEDS; i++) {
                if (!device_is_ready(leds[i].port)) return -1;
                gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE);
        }

	for (int i = 0; i < NUM_BUTTONS; i++) {
        if (!device_is_ready(buttons[i].port)) return -1;

                gpio_pin_configure_dt(&buttons[i], GPIO_INPUT | GPIO_PULL_UP);

                gpio_pin_interrupt_configure_dt(&buttons[i], GPIO_INT_EDGE_TO_ACTIVE);

                gpio_init_callback(&button_cbs[i],
                                button_pressed,
                                BIT(buttons[i].pin));

                gpio_add_callback(buttons[i].port, &button_cbs[i]);
        }

        pmic_led_off(0);
        pmic_led_off(1);
        pmic_led_off(2);

        while (1) {
		k_msleep(SLEEP_TIME_MS);
	}
}
