/**
 * \file ad9371/src/devices/ad9528/t_ad9528.h
 * \brief Contains enum and structure data types for all AD9528 function calls
 */

/**
* Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef _AD9528_TYPES_H_
#define _AD9528_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* AD9528 SPI Address defines */
#define AD9528_ADDR_ADI_SPI_CONFIG_A  0x000
#define AD9528_ADDR_ADI_SPI_CONFIG_B  0x001
#define AD9528_ADDR_IO_UPDATE         0x00F

#define AD9528_ADDR_REF_A_DIVIDER_LSB 0x100
#define AD9528_ADDR_REF_A_DIVIDER_MSB 0x101
#define AD9528_ADDR_REF_B_DIVIDER_LSB 0x102
#define AD9528_ADDR_REF_B_DIVIDER_MSB 0x103
#define AD9528_ADDR_PLL1_N_DIV_LSB    0x104
#define AD9528_ADDR_PLL1_N_DIV_MSB    0x105
#define AD9528_ADDR_PLL1_CHARGEPUMP   0x106
#define AD9528_ADDR_PLL1_CP_CTRL2     0x107
#define AD9528_ADDR_INPUT_RECEIVERS1  0x108
#define AD9528_ADDR_INPUT_RECEIVERS2  0x109
#define AD9528_ADDR_INPUT_RECEIVERS3  0x10A
#define AD9528_ADDR_PLL1_FASTLOCK     0x10B

#define AD9528_ADDR_PLL2_CHARGEPUMP        0x200
#define AD9528_ADDR_PLL2_N_DIV             0x201
#define AD9528_ADDR_PLL2_CTRL              0x202
#define AD9528_ADDR_PLL2_VCO_CTRL          0x203
#define AD9528_ADDR_PLL2_VCO_DIV           0x204
#define AD9528_ADDR_PLL2_LF_CTRL1          0x205
#define AD9528_ADDR_PLL2_LF_CTRL2          0x206
#define AD9528_ADDR_PLL2_RDIV              0x207
#define AD9528_ADDR_PLL2_REPLICA_CHDIV     0x208
#define AD9528_ADDR_PLL2_REPLICA_DIV_PHASE 0x209

#define AD9528_ADDR_CH_OUT0_CTRL1       0x300
#define AD9528_ADDR_CH_OUT0_CTRL2       0x301
#define AD9528_ADDR_CH_OUT0_CHDIV       0x302
#define AD9528_ADDR_OUTPUT_SYNC         0x32A
#define AD9528_ADDR_MASK_SYNC1          0x32B
#define AD9528_ADDR_MASK_SYNC2          0x32C
#define AD9528_ADDR_EN_OUTPUT_PATH_SEL1 0x32D
#define AD9528_ADDR_EN_OUTPUT_PATH_SEL2 0x32E

#define AD9528_ADDR_SYSERF_DIV_LSB      0x400
#define AD9528_ADDR_SYSERF_DIV_MSB      0x401
#define AD9528_ADDR_SYSREF_CTRL3        0x402
#define AD9528_ADDR_SYSREF_CTRL4        0x403
#define AD9528_ADDR_SYSREF_CTRL5        0x404

#define AD9528_ADDR_POWERDOWN_CTRL      0x500
#define AD9528_ADDR_CH_POWERDOWN1       0x501
#define AD9528_ADDR_CH_POWERDOWN2       0x502
#define AD9528_ADDR_LDO_ENABLES1        0x503
#define AD9528_ADDR_LDO_ENABLES2        0x504
#define AD9528_ADDR_STATUS0_CTRL        0x505
#define AD9528_ADDR_STATUS1_CTRL        0x506
#define AD9528_ADDR_STATUS_OE           0x507
#define AD9528_ADDR_STATUS_READBACK0    0x508
#define AD9528_ADDR_STATUS_READBACK1    0x509



///Enum to select what source outputs from each AD9528 output channel
typedef enum
{
    CHANNEL_DIV = 0, PLL1_OUTPUT = 1, //VCXO frequency
    SYSREF = 2,
    INV_PLL1_OUTPUT = 5 //VCXO inverted
} ad9528outSourceSel_t;

///Enum to select what output buffer standard is used for the clock outputs
typedef enum
{
    LVDS = 0, LVDS_BOOST = 1, HSTL = 2
} ad9528outBufferControl_t;

///Enum to select the clock input buffer mode.
typedef enum
{
    DISABLED = 0, SINGLE_ENDED, NEG_SINGLE_ENDED, DIFFERENTIAL
} ad9528RefBuffer_t;

/// Structure to hold AD9528 PLL1 settings
typedef struct
{
    uint32_t refA_Frequency_Hz;
    uint16_t refA_Divider;
    ad9528RefBuffer_t refA_bufferCtrl;

    uint32_t refB_Frequency_Hz;
    uint16_t refB_Divider;
    ad9528RefBuffer_t refB_bufferCtrl;

    uint32_t vcxo_Frequency_Hz;
    ad9528RefBuffer_t vcxoBufferCtrl;

    uint16_t nDivider;
    uint8_t bypassPll1;
} ad9528pll1Settings_t;

/// Structure to hold AD9528 PLL2 settings
typedef struct
{
    uint8_t rfDivider; //VCO divider: Valid range (3,4,5)
    uint8_t n2Divider; //PLL2 N2 Divider
    uint8_t totalNdiv; //NDiv = 4*Bdiv + Adiv //Bdiv valid range (3 to 63), Adiv valid range (0-3)
} ad9528pll2Settings_t;

/// Structure to hold AD9528 Output Clock settings
typedef struct
{
    uint16_t outPowerDown; //bit per output, if 1 power down that output.
    ad9528outSourceSel_t outSource[14]; //CHANNEL_DIV, PLL1_OUTPUT, SYSREF, INV_PLL1_OUTPUT
    ad9528outBufferControl_t outBufferCtrl[14]; //LVDS, LVDS_BOOST, HSTL
    uint8_t outAnalogDelay[14]; //5 bits
    uint8_t outDigitalDelay[14]; //6 bits, 1/2 clock resolution @ channel div input frequency
    uint8_t outChannelDiv[14]; //8 bit channel divider
    uint32_t outFrequency_Hz[14]; //output clock frequency per clock output
} ad9528outputSettings_t;

///Enum to select the SYSREF output source
typedef enum
{
    EXTERNAL = 0, EXT_RESAMPLED = 1, INTERNAL = 2
} sysrefSourceSelection_t;

///Enum to choose the SYSREF pattern mode.
typedef enum
{
    NSHOT = 0, CONTINUOUS = 1, PRBS = 2, STOP = 3
} sysrefPatternMode_t;

///Enum to select the SYSREF input pin mode.
typedef enum
{
    LEVEL_ACTIVE_HIGH = 0, LEVEL_ACTIVE_LOW = 1, RISING_EDGE = 2, FALLING_EDGE = 3
} sysrefPinEdgeMode_t;

///Enum to select the SYSREF output # of pulses for NSHOT mode.
typedef enum
{
    ONE_PULSE = 0, TWO_PULSES = 2, FOUR_PULSES = 3, SIX_PULSES = 4, EIGHT_PULSES = 5
} sysrefNshotMode_t;

//Enum to select how a SYSREF is requested.
typedef enum
{
    SPI = 0, PIN = 1
} sysrefRequestMethod_t;

/// Structure to hold AD9528 SYSREF output settings
typedef struct
{
    sysrefRequestMethod_t sysrefRequestMethod;
    sysrefSourceSelection_t sysrefSource;
    sysrefPinEdgeMode_t sysrefPinEdgeMode;
    ad9528RefBuffer_t sysrefPinBufferMode;
    sysrefPatternMode_t sysrefPatternMode;
    sysrefNshotMode_t sysrefNshotMode;
    uint16_t sysrefDivide;

} ad9528sysrefSettings_t;

/// Structure to hold AD9528 settings
typedef struct
{
    spiSettings_t *spiSettings;
    ad9528pll1Settings_t *pll1Settings;
    ad9528pll2Settings_t *pll2Settings;
    ad9528outputSettings_t *outputSettings;
    ad9528sysrefSettings_t *sysrefSettings;
} ad9528Device_t;

#ifdef __cplusplus
}
#endif

#endif
