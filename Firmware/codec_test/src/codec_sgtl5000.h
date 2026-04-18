#pragma once

/**
 * @brief Initialize SGTL5000
 *
 * Must be called once at startup.
 *
 * TODO full spec
 * 
 * @return 0 on success, negative error code on failure
 */
int codec_init();

/**
 * @brief Sets input gain
 *
 * TODO accepted values
 *
 * @return 0 on success, negative error code on failure
 */
int codec_set_input_gain();

/**
 * @brief Changes output voltage
 *
 * TODO accepted values
 *
 * @return 0 on success, negative error code on failure
 */
int codec_set_output_volume();