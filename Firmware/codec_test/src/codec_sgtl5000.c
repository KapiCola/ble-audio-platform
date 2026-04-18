#include <zephyr/kernel.h>
#include <zephyr/drivers/i2c.h>
#include <zephyr/drivers/i2s.h>
#include <zephyr/sys/printk.h>
#include <zephyr/device.h>
#include "i2c_helpers.h"

/* ===================== DEVICE ADDRESS ===================== */

#define SGTL5000_ADDR 0x0A

/* ===================== REGISTERS ===================== */

#define CHIP_ANA_POWER 0x0030
#define CHIP_LINREG_CTRL 0x0026
#define CHIP_REF_CTRL 0x0028
#define CHIP_LINE_OUT_CTRL 0x002C
#define CHIP_SHORT_CTRL 0X03C
#define CHIP_ANA_CTRL 0x0024
#define CHIP_DIG_POWER 0x0002
#define CHIP_LINE_OUT_VOL 0x002E
#define CHIP_CLK_CTRL 0x0004
#define CHIP_I2S_CTRL 0x0006
#define CHIP_SSS_CTRL 0x000A
#define CHIP_ANA_ADC_CTRL 0x0020
#define CHIP_DAC_VOL 0x0010
#define CHIP_ADCDAC_CTRL 0x000E

/* ===================== I2C ===================== */

const struct device *codec_i2c = DEVICE_DT_GET(DT_NODELABEL(i2c1));

/* ===================== HELPERS ===================== */

#define BIT_SHIFT(x) (1 << (x))

int codec_write(uint16_t reg, uint16_t val)
{
    return i2c_reg_write16_16(codec_i2c, SGTL5000_ADDR, reg, val);
}

int codec_mod(uint16_t reg, uint16_t clear_mask, uint16_t set_mask)
{
    return i2c_reg_update16_16(codec_i2c, SGTL5000_ADDR, reg, clear_mask, set_mask);
}

/* ===================== INIT SECTION ===================== */

void codec_init(void)
{
        //--------------- Power Supply Configuration----------------

        // Power up Bias reference and set stereo ADC/DAC, everything else power down
        codec_write(CHIP_ANA_POWER, 0x4060);

        // Configure the charge pump to use the VDDIO rail (set bit_ 5 and bit_SHIFT 6)
        codec_mod(CHIP_LINREG_CTRL, ~(BIT_SHIFT(5) | BIT_SHIFT(6)), (BIT_SHIFT(5) | BIT_SHIFT(6)));

        //---- Reference Voltage and Bias Current Configuration----

        // Set ground, ADC, DAC reference voltage (bits 8:4). Value 1.525V
        // The bias current should be set to 50% of the nominal value (bits 3:1)
        codec_write(CHIP_REF_CTRL, 0x1DE);

        // Set LINEOUT reference voltage to VDDIO/2 (1.65 V) (bits 5:0) and bias current (bits 11:8) to the recommended value of 0.36 mA for 10 kOhm load with 1.0 nF capacitance
        codec_mod(CHIP_LINE_OUT_CTRL, ~0x0322, 0x0322);

        //------------Other Analog Block Configurations--------------

        // Configure slow ramp up rate to minimize pop (bit 0)
        codec_mod(CHIP_REF_CTRL, ~BIT_SHIFT(0), BIT_SHIFT(0));

        // Enable short detect mode for headphone left/right
        // and center channel and set short detect current trip level
        // to 75 mA
        codec_write(CHIP_SHORT_CTRL, 0x1106);

        // Enable Zero-cross detect if needed for HP_OUT (bit 5) and ADC (bit 1)
        // ADC - mute, ZDC, LINEIN
        // HP - mute, ZDC, DAC
        // LINEOUT - mute
        codec_write(CHIP_ANA_CTRL, 0x0137);

        //------------Power up Inputs/Outputs/Digital Blocks---------
        // Power up LINEOUT, ADC, DAC
        codec_mod(CHIP_ANA_POWER, ~(0xB), 0xB);
        // Power up VAG (recommended sequence by datasheet)
        codec_mod(CHIP_ANA_POWER, ~0x80, 0x80);
        k_msleep(100);

        // Power up desired digital blocks
        // I2S IN/OUT - enabled
        // ADC/DAC - enabled
        codec_write(CHIP_DIG_POWER, 0x0063);

        //----------------Set LINEOUT Volume Level-------------------

        // Set the LINEOUT volume level based on voltage reference (VAG)
        // values using this formula
        // Value = (int)(40*log(VAG_VAL/LO_VAGCNTRL) + 15)
        // Assuming VAG_VAL and LO_VAGCNTRL is set to 0.9 V and
        //1.65 V respectively, the // left LO vol (bits 12:8) and right LO
        //volume (bits 4:0) value should be set // to 5
        // Recommended for 3.3V VDDIO and 3.2V VDDIO 
        codec_write(CHIP_LINE_OUT_VOL, 0x0E0E);

        //-------------------------Other-----------------------------

        // Configure SYS_FS clock to 48 kHz (bits 3:2)
        // Configure MCLK_FREQ to 256*Fs (bits 1:0)
        codec_mod(CHIP_CLK_CTRL, ~0xF, 0x8);

        // Configure the I2S in slave mode (bit 7)
        // Data length 16 bit
        // SCLK 64 * Fs
        codec_write(CHIP_I2S_CTRL, 0x30);

        //-------------------Input/Output Routing---------------------

        // I2S_DOUT <- ADC
        // DAC <- I2S_IN
        // DAP <- ADC (Default)
        // DAP_MIX <- ADC (Default)
        codec_write(CHIP_SSS_CTRL, 0x10);

        // Unmute ADC (bit 0) and LINEOUT (bit 8)
        codec_mod(CHIP_ANA_CTRL, ~0x101, 0x0);

        //------------------ Input Volume Control---------------------

        // Configure ADC left and right analog volume to desired default.
        // ADC range reduced by 6.0 dB (bit 8)
        // ADC Right/Left channel volume 0 dB (bits 7:4 and 3:0)
        codec_write(CHIP_ANA_ADC_CTRL, 0x144);

        //---------------- Volume and Mute Control---------------------

        // LINEOUT and DAC volume control
        // Mute Line Out
        codec_mod(CHIP_ANA_CTRL, ~0x100, 0x100);

        // Set volume of 0 dB
        codec_write(CHIP_DAC_VOL, 0x3C3C);

        // Unmute DAC left/right
        codec_mod(CHIP_ADCDAC_CTRL, ~0xC, 0x00);

        // Enable volume ramp exponential
        codec_mod(CHIP_ADCDAC_CTRL, ~0x300, 0x300);

        // Umnute Line Out
        codec_mod(CHIP_ANA_CTRL, ~0x100, 0x000);
}

/* ===================== PUBLIC API ===================== */

//TODO
int codec_set_input_gain();

//TODO
int codec_set_output_volume();
