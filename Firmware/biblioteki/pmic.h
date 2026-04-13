#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize PMIC (BUCKs, LDOs, LEDs)
 *
 * Must be called once at startup.
 *
 * @return 0 on success, negative error code on failure
 */
int pmic_init(void);

/**
 * @brief Turn ON PMIC LED
 *
 * @param led LED index (0..2)
 */
void pmic_led_on(int led);

/**
 * @brief Turn OFF PMIC LED
 *
 * @param led LED index (0..2)
 */
void pmic_led_off(int led);

void pmic_debug_dump(void);

#ifdef __cplusplus
}
#endif