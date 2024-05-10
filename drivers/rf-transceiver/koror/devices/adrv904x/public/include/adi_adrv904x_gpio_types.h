/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_gpio_types.h
 * \brief Contains functions to allow control of the General Purpose IO functions on the ADRV904X device
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_GPIO_TYPES_H_
#define _ADI_ADRV904X_GPIO_TYPES_H_

#include <stdint.h>

#define ADI_ADRV904X_GPIO_COUNT 24U
#define ADI_ADRV904X_GPIO_ANALOG_COUNT 16U


#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_0                  (uint64_t)(0x0000000000000001ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_1                  (uint64_t)(0x0000000000000002ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_2                  (uint64_t)(0x0000000000000004ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_3                  (uint64_t)(0x0000000000000008ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_4                  (uint64_t)(0x0000000000000010ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_5                  (uint64_t)(0x0000000000000020ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_6                  (uint64_t)(0x0000000000000040ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_7                  (uint64_t)(0x0000000000000080ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_FRAMER_IRQ_8                  (uint64_t)(0x0000000000000100ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_0                (uint64_t)(0x0000000000000200ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_1                (uint64_t)(0x0000000000000400ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_2                (uint64_t)(0x0000000000000800ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_3                (uint64_t)(0x0000000000001000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_4                (uint64_t)(0x0000000000002000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_5                (uint64_t)(0x0000000000004000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_6                (uint64_t)(0x0000000000008000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_7                (uint64_t)(0x0000000000010000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_8                (uint64_t)(0x0000000000020000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_9                (uint64_t)(0x0000000000040000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_10               (uint64_t)(0x0000000000080000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_11               (uint64_t)(0x0000000000100000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_12               (uint64_t)(0x0000000000200000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_DEFRAMER_IRQ_13               (uint64_t)(0x0000000000400000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX0_STREAM_ERROR              (uint64_t)(0x0000000000800000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX1_STREAM_ERROR              (uint64_t)(0x0000000001000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX2_STREAM_ERROR              (uint64_t)(0x0000000002000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX3_STREAM_ERROR              (uint64_t)(0x0000000004000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX4_STREAM_ERROR              (uint64_t)(0x0000000008000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX5_STREAM_ERROR              (uint64_t)(0x0000000010000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX6_STREAM_ERROR              (uint64_t)(0x0000000020000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_RX7_STREAM_ERROR              (uint64_t)(0x0000000040000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX0_STREAM_ERROR              (uint64_t)(0x0000000080000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX1_STREAM_ERROR              (uint64_t)(0x0000000100000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX2_STREAM_ERROR              (uint64_t)(0x0000000200000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX3_STREAM_ERROR              (uint64_t)(0x0000000400000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX4_STREAM_ERROR              (uint64_t)(0x0000000800000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX5_STREAM_ERROR              (uint64_t)(0x0000001000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX6_STREAM_ERROR              (uint64_t)(0x0000002000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_TX7_STREAM_ERROR              (uint64_t)(0x0000004000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_ORX0_STREAM_ERROR             (uint64_t)(0x0000008000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_ORX1_STREAM_ERROR             (uint64_t)(0x0000010000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_CORE_STREAM_ERROR             (uint64_t)(0x0000020000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_AUX_SPI_CORE_ERROR            (uint64_t)(0x0000040000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_ERROR_SPI_PAGING              (uint64_t)(0x0000080000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_LOWER_ERROR_AUX_SPI_PAGING          (uint64_t)(0x0000100000000000ULL)

#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM1_MEMORY_ECC_ERROR         (uint64_t)(0x0000000000000001ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM1_SYSTEM_ERROR             (uint64_t)(0x0000000000000002ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM1_CALIBRATION_ERROR        (uint64_t)(0x0000000000000004ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM1_WATCHDOG_TIMEOUT         (uint64_t)(0x0000000000000008ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM1_FORCE_GP_INTERRUPT       (uint64_t)(0x0000000000000010ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM0_MEMORY_ECC_ERROR         (uint64_t)(0x0000000000000020ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM0_SYSTEM_ERROR             (uint64_t)(0x0000000000000040ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM0_CALIBRATION_ERROR        (uint64_t)(0x0000000000000080ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM0_WATCHDOG_TIMEOUT         (uint64_t)(0x0000000000000100ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_ARM0_FORCE_GP_INTERRUPT       (uint64_t)(0x0000000000000200ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX0_POWER_ERROR (uint64_t)(0x0000000000000400ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX0_SRD_ERROR   (uint64_t)(0x0000000000000800ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX1_POWER_ERROR (uint64_t)(0x0000000000001000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX1_SRD_ERROR   (uint64_t)(0x0000000000002000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX2_POWER_ERROR (uint64_t)(0x0000000000004000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX2_SRD_ERROR   (uint64_t)(0x0000000000008000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX3_POWER_ERROR (uint64_t)(0x0000000000010000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX3_SRD_ERROR   (uint64_t)(0x0000000000020000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX4_POWER_ERROR (uint64_t)(0x0000000000040000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX4_SRD_ERROR   (uint64_t)(0x0000000000080000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX5_POWER_ERROR (uint64_t)(0x0000000000100000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX5_SRD_ERROR   (uint64_t)(0x0000000000200000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX6_POWER_ERROR (uint64_t)(0x0000000000400000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX6_SRD_ERROR   (uint64_t)(0x0000000000800000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX7_POWER_ERROR (uint64_t)(0x0000000001000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_PA_PROTECTION_TX7_SRD_ERROR   (uint64_t)(0x0000000002000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_SERDES_PLL_UNLOCK             (uint64_t)(0x0000000004000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_CLKPLL_OVR_RANGE              (uint64_t)(0x0000000008000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_RFPLL1_CP_OVR_RANGE           (uint64_t)(0x0000000010000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_RFPLL0_CP_OVR_RANGE           (uint64_t)(0x0000000020000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_CLKPLL_UNLOCK                 (uint64_t)(0x0000000040000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_RFPLL1_UNLOCK                 (uint64_t)(0x0000000080000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_RFPLL0_UNLOCK                 (uint64_t)(0x0000000100000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_SPI_AHB_ERROR                 (uint64_t)(0x0000000200000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_AUX_SPI_AHB_ERROR             (uint64_t)(0x0000000400000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_SPI_CLK_EFUSE_READ_ABORT      (uint64_t)(0x0000000800000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_AUX_SPI_ABORT                 (uint64_t)(0x0000001000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_SPI_ABORT                     (uint64_t)(0x0000002000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_AND_MEM_INT_0         (uint64_t)(0x0000004000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_AND_MEM_INT_1         (uint64_t)(0x0000008000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_AND_MEM_INT_2         (uint64_t)(0x0000010000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_AND_MEM_INT_3         (uint64_t)(0x0000040000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_AND_MEM_INT_4         (uint64_t)(0x0000080000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_PINT_SW0              (uint64_t)(0x0000100000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_DFE_CPU_PINT_SW1              (uint64_t)(0x0000200000000000ULL)
#define ADI_ADRV904X_GPINT_MASK_UPPER_RADIO_SEQ_ERROR               (uint64_t)(0x0000800000000000ULL)


/****************************************************************************
 * Digital and Analog GPIO related types
 ****************************************************************************
 */

/**
 *  \brief Enum to select desired GPIO pin used by the API
 */
typedef enum adi_adrv904x_GpioPinSel
{
    ADI_ADRV904X_GPIO_00 = 0U,   /*!< Select GPIO_00*/
    ADI_ADRV904X_GPIO_01,       /*!< Select GPIO_01*/
    ADI_ADRV904X_GPIO_02,       /*!< Select GPIO_02*/
    ADI_ADRV904X_GPIO_03,       /*!< Select GPIO_03*/
    ADI_ADRV904X_GPIO_04,       /*!< Select GPIO_04*/
    ADI_ADRV904X_GPIO_05,       /*!< Select GPIO_05*/
    ADI_ADRV904X_GPIO_06,       /*!< Select GPIO_06*/
    ADI_ADRV904X_GPIO_07,       /*!< Select GPIO_07*/
    ADI_ADRV904X_GPIO_08,       /*!< Select GPIO_08*/
    ADI_ADRV904X_GPIO_09,       /*!< Select GPIO_09*/
    ADI_ADRV904X_GPIO_10,       /*!< Select GPIO_10*/
    ADI_ADRV904X_GPIO_11,       /*!< Select GPIO_11*/
    ADI_ADRV904X_GPIO_12,       /*!< Select GPIO_12*/
    ADI_ADRV904X_GPIO_13,       /*!< Select GPIO_13*/
    ADI_ADRV904X_GPIO_14,       /*!< Select GPIO_14*/
    ADI_ADRV904X_GPIO_15,       /*!< Select GPIO_15*/
    ADI_ADRV904X_GPIO_16,       /*!< Select GPIO_16*/
    ADI_ADRV904X_GPIO_17,       /*!< Select GPIO_17*/
    ADI_ADRV904X_GPIO_18,       /*!< Select GPIO_18*/
    ADI_ADRV904X_GPIO_19,       /*!< Select GPIO_19*/
    ADI_ADRV904X_GPIO_20,       /*!< Select GPIO_20*/
    ADI_ADRV904X_GPIO_21,       /*!< Select GPIO_21*/
    ADI_ADRV904X_GPIO_22,       /*!< Select GPIO_22*/
    ADI_ADRV904X_GPIO_23,       /*!< Select GPIO_23*/
    ADI_ADRV904X_GPIO_INVALID   /*!< Invalid GPIO*/
} adi_adrv904x_GpioPinSel_e;

/**
*  \brief Enum to select desired Analog GPIO pins used by the API
*/
typedef enum adi_adrv904x_GpioAnaPinSel
{
    ADI_ADRV904X_GPIO_ANA_00 = 0U,   /*!< Select GPIO_ANA_00*/
    ADI_ADRV904X_GPIO_ANA_01,       /*!< Select GPIO_ANA_01*/
    ADI_ADRV904X_GPIO_ANA_02,       /*!< Select GPIO_ANA_02*/
    ADI_ADRV904X_GPIO_ANA_03,       /*!< Select GPIO_ANA_03*/
    ADI_ADRV904X_GPIO_ANA_04,       /*!< Select GPIO_ANA_04*/
    ADI_ADRV904X_GPIO_ANA_05,       /*!< Select GPIO_ANA_05*/
    ADI_ADRV904X_GPIO_ANA_06,       /*!< Select GPIO_ANA_06*/
    ADI_ADRV904X_GPIO_ANA_07,       /*!< Select GPIO_ANA_07*/
    ADI_ADRV904X_GPIO_ANA_08,       /*!< Select GPIO_ANA_08*/
    ADI_ADRV904X_GPIO_ANA_09,       /*!< Select GPIO_ANA_09*/
    ADI_ADRV904X_GPIO_ANA_10,       /*!< Select GPIO_ANA_10*/
    ADI_ADRV904X_GPIO_ANA_11,       /*!< Select GPIO_ANA_11*/
    ADI_ADRV904X_GPIO_ANA_12,       /*!< Select GPIO_ANA_12*/
    ADI_ADRV904X_GPIO_ANA_13,       /*!< Select GPIO_ANA_13*/
    ADI_ADRV904X_GPIO_ANA_14,       /*!< Select GPIO_ANA_14*/
    ADI_ADRV904X_GPIO_ANA_15,       /*!< Select GPIO_ANA_15*/
    ADI_ADRV904X_GPIO_ANA_INVALID   /*!< Invalid Analog Gpio*/
} adi_adrv904x_GpioAnaPinSel_e;

/**
 *  \brief Enum to select desired GPIO pin from any type (both Analog and Digital)
 */
typedef enum adi_adrv904x_GpioGenericPinSel
{
    ADI_ADRV904X_GPIO_GENERIC_DIG_00    = 0x1000U,     /*!< Select Digital GPIO 00 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_01    = 0x1001U,     /*!< Select Digital GPIO 01 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_02    = 0x1002U,     /*!< Select Digital GPIO 02 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_03    = 0x1003U,     /*!< Select Digital GPIO 03 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_04    = 0x1004U,     /*!< Select Digital GPIO 04 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_05    = 0x1005U,     /*!< Select Digital GPIO 05 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_06    = 0x1006U,     /*!< Select Digital GPIO 06 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_07    = 0x1007U,     /*!< Select Digital GPIO 07 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_08    = 0x1008U,     /*!< Select Digital GPIO 08 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_09    = 0x1009U,     /*!< Select Digital GPIO 09 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_10    = 0x100aU,     /*!< Select Digital GPIO 10 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_11    = 0x100bU,     /*!< Select Digital GPIO 11 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_12    = 0x100cU,     /*!< Select Digital GPIO 12 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_13    = 0x100dU,     /*!< Select Digital GPIO 13 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_14    = 0x100eU,     /*!< Select Digital GPIO 14 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_15    = 0x100fU,     /*!< Select Digital GPIO 15 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_16    = 0x1010U,     /*!< Select Digital GPIO 16 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_17    = 0x1011U,     /*!< Select Digital GPIO 17 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_18    = 0x1012U,     /*!< Select Digital GPIO 18 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_19    = 0x1013U,     /*!< Select Digital GPIO 19 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_20    = 0x1014U,     /*!< Select Digital GPIO 20 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_21    = 0x1015U,     /*!< Select Digital GPIO 21 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_22    = 0x1016U,     /*!< Select Digital GPIO 22 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_23    = 0x1017U,     /*!< Select Digital GPIO 23 */
    ADI_ADRV904X_GPIO_GENERIC_DIG_START = 0x1000U,
    ADI_ADRV904X_GPIO_GENERIC_DIG_END   = 0x1017U,
    
    ADI_ADRV904X_GPIO_GENERIC_ANA_00    = 0x2000U,     /*!< Select Analog GPIO 00 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_01    = 0x2001U,     /*!< Select Analog GPIO 01 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_02    = 0x2002U,     /*!< Select Analog GPIO 02 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_03    = 0x2003U,     /*!< Select Analog GPIO 03 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_04    = 0x2004U,     /*!< Select Analog GPIO 04 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_05    = 0x2005U,     /*!< Select Analog GPIO 05 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_06    = 0x2006U,     /*!< Select Analog GPIO 06 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_07    = 0x2007U,     /*!< Select Analog GPIO 07 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_08    = 0x2008U,     /*!< Select Analog GPIO 08 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_09    = 0x2009U,     /*!< Select Analog GPIO 09 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_10    = 0x200aU,     /*!< Select Analog GPIO 10 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_11    = 0x200bU,     /*!< Select Analog GPIO 11 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_12    = 0x200cU,     /*!< Select Analog GPIO 12 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_13    = 0x200dU,     /*!< Select Analog GPIO 13 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_14    = 0x200eU,     /*!< Select Analog GPIO 14 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_15    = 0x200fU,     /*!< Select Analog GPIO 15 */
    ADI_ADRV904X_GPIO_GENERIC_ANA_START = 0x2000U,
    ADI_ADRV904X_GPIO_GENERIC_ANA_END   = 0x200fU,
    
    ADI_ADRV904X_GPIO_GENERIC_INVALID   = 0xFFFFU     /*!< Invalid GPIO */
} adi_adrv904x_GpioGenericPinSel_e;

/**
 *  \brief Enum for ADRV904X GPIO Signals. Must Map to Shared Resource Manager Feature
 */
typedef enum adi_adrv904x_GpioSignal
{
    ADI_ADRV904X_GPIO_SIGNAL_UNUSED = 0U,                                       /*!< Select UNUSED signal */
    ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDIO,                                      /*!< Aux SPI port signal: SDIO */
    ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_SDO,                                       /*!< Aux SPI port signal: SDO */
    ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CLK,                                       /*!< Aux SPI port signal: CLK */
    ADI_ADRV904X_GPIO_SIGNAL_AUX_SPI_CSB,                                       /*!< Aux SPI port signal: CSB */
    ADI_ADRV904X_GPIO_SIGNAL_UART_PADRXSIN,                                     /*!< Select UART Receive Input signal                */
    ADI_ADRV904X_GPIO_SIGNAL_UART_PADCTS,                                       /*!< Select UART Clear to Send Flow control signal   */
    ADI_ADRV904X_GPIO_SIGNAL_UART_PADRTSOUT,                                    /*!< Select UART Request to Send Flow control signal */
    ADI_ADRV904X_GPIO_SIGNAL_UART_PADTXSOUT,                                    /*!< Select UART Transmit Output signal              */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_CLK_OUT,                                 /*!< Trace Clk OUT for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_0,                              /*!< Trace Data Out0 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_1,                              /*!< Trace Data Out1 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_2,                              /*!< Trace Data Out2 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_3,                              /*!< Trace Data Out3 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_4,                              /*!< Trace Data Out4 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_5,                              /*!< Trace Data Out5 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_6,                              /*!< Trace Data Out6 for DFE FW Debug */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_TRACE_DATA_OUT_7,                              /*!< Trace Data Out7 for DFE FW Debug */

    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_0,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_1,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_2,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_3,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_4,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_5,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_6,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_7,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_8,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_9,                              /*!< GPIO Manual Output (Drive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_10,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_11,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_12,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_13,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_14,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_15,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_16,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_17,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_18,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_19,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_20,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_21,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_22,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_OUTPUT_23,                             /*!< GPIO Manual Output (Drive) Mode: GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_0,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_1,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_2,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_3,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_4,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_5,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_6,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_7,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_8,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_9,                               /*!< GPIO Manual Input (Receive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_10,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_11,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_12,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_13,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_14,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_15,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_16,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_17,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_18,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_19,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_20,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_21,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_22,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_MANUAL_MODE_INPUT_23,                              /*!< GPIO Manual Input (Receive) Mode: GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_0,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_1,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_2,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_3,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_4,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_5,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_6,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_7,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_8,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_9,                                      /*!< GPIO ARM Output (Drive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_10,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_11,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_12,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_13,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_14,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_15,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_16,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_17,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_18,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_19,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_20,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_21,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_22,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_OUTPUT_23,                                     /*!< GPIO ARM Output (Drive) Mode: GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_0,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_1,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_2,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_3,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_4,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_5,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_6,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_7,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_8,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_9,                                       /*!< GPIO ARM Input (Receive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_10,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_11,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_12,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_13,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_14,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_15,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_16,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_17,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_18,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_19,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_20,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_21,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_22,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_ARM_INPUT_23,                                      /*!< GPIO ARM Input (Receive) Mode: GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_2,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_3,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_4,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_FINE_BAND_OUT_5,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_CALBITS_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_PLL_LOCKED,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_OVERRANGE_LOW_FLAG,                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_CP_OVERRANGE_HIGH_FLAG,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_COMP_OUT,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_CAL_IN_PROGRESS,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_0,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_1,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_2,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_3,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_4,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_5,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_6,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_7,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_FREQ_COARSE_BAND_OUT_8,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_0,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_1,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_2,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_3,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_4,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_5,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_6,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_7,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_8,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_9,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_ALC_WORD_OUT_10,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_6,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_7,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_8,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_9,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_10,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_VCO_TCIDAC_11,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_0,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_1,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_2,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_3,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_4,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_5,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_6,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CLKPLL_SPARE_7,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_2,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_3,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_4,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_FINE_BAND_OUT_5,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_CALBITS_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_PLL_LOCKED,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_OVERRANGE_LOW_FLAG,                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_CP_OVERRANGE_HIGH_FLAG,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_COMP_OUT,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_CAL_IN_PROGRESS,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_0,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_1,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_2,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_3,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_4,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_5,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_6,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_7,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_FREQ_COARSE_BAND_OUT_8,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_0,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_1,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_2,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_3,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_4,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_5,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_6,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_7,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_8,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_9,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_ALC_WORD_OUT_10,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_6,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_7,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_8,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_9,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_10,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_VCO_TCIDAC_11,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_0,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_1,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_2,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_3,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_4,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_5,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_6,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL0_SPARE_7,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_2,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_3,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_4,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_FINE_BAND_OUT_5,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_CALBITS_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_PLL_LOCKED,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_OVERRANGE_LOW_FLAG,                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_CP_OVERRANGE_HIGH_FLAG,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_COMP_OUT,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_CAL_IN_PROGRESS,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_0,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_1,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_2,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_3,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_4,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_5,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_6,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_7,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_FREQ_COARSE_BAND_OUT_8,                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_0,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_1,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_2,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_3,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_4,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_5,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_6,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_7,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_8,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_9,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_ALC_WORD_OUT_10,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_3,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_4,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_5,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_6,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_7,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_8,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_9,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_10,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_VCO_TCIDAC_11,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_0,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_1,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_2,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_3,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_4,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_5,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_6,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFPLL1_SPARE_7,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_0,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_1,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_2,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_3,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_4,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_5,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_6,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_N_DES_CH_7,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_0,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_1,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_2,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_3,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_4,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_5,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_6,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTAG_DETECTOR_P_DES_CH_7,                          /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_0,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_1,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_P_2,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_0,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_1,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SYNC_JTAG_DETECTOR_N_2,                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_2,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_3,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_4,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_5,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_6,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JRX_READ_DATA_7,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_2,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_3,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_4,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_5,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_6,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_OUT_MUX_JTX_READ_DATA_7,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_0,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_1,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_2,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CONTROL_UPDATE_3,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_CORE,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_REF_CLK,                                           /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_ARM0_ERROR,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_ARM1_ERROR,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DIG_POWERGOOD,                                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MASTER_BIAS_CLK,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MBIAS_IGEN_PD_0,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MBIAS_IGEN_PD_1,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MBIAS_COMP_OUT_ANA_0,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MBIAS_COMP_OUT_ANA_1,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_MBIAS_IGEN_PTATR_TRIM_DONE_0,              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SPI_REG_MBIAS_IGEN_PTATR_TRIM_DONE_1,              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_0,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_1,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_2,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_3,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_4,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_5,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_6,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_7,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_8,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_9,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_10,                                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_11,                                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_12,                                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JRX_IRQ_13,                                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_0,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_1,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_2,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_3,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_4,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_5,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_6,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_7,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JTX_IRQ_8,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_NEW_PHASE_TOGGLE,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_JESD_MCS_CLK_IND,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SERDES_PLL_PLL_LOCKED,                             /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SERDES_PLL_VCO_COMP_OUT,                           /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_POWER_ON_RESET_N,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_0,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_1,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_2,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_3,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_4,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_5,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_6,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_7,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_8,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_9,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_10,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_11,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_12,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_13,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_14,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MISR_SIGNATURE_15,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_0,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_1,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_2,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_3,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_4,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_5,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_6,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RX_SPI_SOURCE_7,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_0,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_1,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_2,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_POSITION_3,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_RX,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_UPDATE,                                       /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_SLICER_OVERFLOW,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_SEC_HIGH,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_SEC_HIGH_COUNTER_EXCEEDED,                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TIA_VALID,                                         /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RXFE_VALID,                                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RSSI_DECPWR_READY,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_PEAK_COUNT_EXCEEDED_QEC_ANA,                       /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_PEAK_COUNT_EXCEEDED_QEC_DIG,                       /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RSSI_SYMBOL_READY,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RXON,                                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ADC_OVERLOAD_RESET_ADC,                            /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_0,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_1,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_2,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_STATE_3,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_UPDATE_COUNTER_EXPIRED,                       /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_MEASURE,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_CAL_DONE,                                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_RFDC_MEASURE_COUNTER_EXPIRED,                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_INT1_COUNTER_EXCEEDED,                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_INT0_COUNTER_EXCEEDED,                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DIG_GAIN_SAT,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_INT1_LOW,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_INT0_LOW,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_STATE_0,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_STATE_1,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_GAINUPDATE_COUNTER_EXPIRED,                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_LOW_TH_EXCEEDED,                            /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_SL_HIGH_TH_EXCEEDED,                           /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LLB_COUNTER_EXCEEDED,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_LOW,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_ADCOVRG_HIGH,                                  /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_GT_GAIN_CHANGE,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_ULB_COUNTER_EXCEEDED,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_LOW_COUNTER_EXCEEDED,                     /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_OVRG_HIGH_COUNTER_EXCEEDED,                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_LOW_0,                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_LOW_1,                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_HIGH_0,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_PEAK_COUNT_EXCEEDED_HIGH_1,                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_TOGGLE_0,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_TOGGLE_1,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_0,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_BAND_DECPWR_READY_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_TX,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_SRERROR,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_PPERROR_OR_APERROR,                        /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ATTEN_RAMP_UP_PROT,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ATTEN_RAMP_DOWN_PROT,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ATTEN_TDD_RAMP_DOWN,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ATTEN_TDD_RAMP_UP,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ATTEN_LATCH_EN,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DTX_POWER_UP_TRIGGER,                              /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DTX_POWER_DOWN_TRIGGER,                            /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_GAIN_CHANGE,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_POWER_READY,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_PA_PROT_SRD_IRQ,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_TSSI_DUC0_READY,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ADC_SAMPLE_OVR,                                    /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_MCS_CLK_IND_ORX,                                   /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ORX_ON,                                            /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DECPWR_READY,                                      /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_ANY_ADC_SAMPLE_OVR,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,                            /*!< Stream Processor Trigger for GPIO 0 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,                            /*!< Stream Processor Trigger for GPIO 1 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,                            /*!< Stream Processor Trigger for GPIO 2 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,                            /*!< Stream Processor Trigger for GPIO 3 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,                            /*!< Stream Processor Trigger for GPIO 4 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,                            /*!< Stream Processor Trigger for GPIO 5 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,                            /*!< Stream Processor Trigger for GPIO 6 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,                            /*!< Stream Processor Trigger for GPIO 7 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,                            /*!< Stream Processor Trigger for GPIO 8 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,                            /*!< Stream Processor Trigger for GPIO 9 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,                           /*!< Stream Processor Trigger for GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,                           /*!< Stream Processor Trigger for GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,                           /*!< Stream Processor Trigger for GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,                           /*!< Stream Processor Trigger for GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,                           /*!< Stream Processor Trigger for GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,                           /*!< Stream Processor Trigger for GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,                           /*!< Stream Processor Trigger for GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,                           /*!< Stream Processor Trigger for GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,                           /*!< Stream Processor Trigger for GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,                           /*!< Stream Processor Trigger for GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,                           /*!< Stream Processor Trigger for GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,                           /*!< Stream Processor Trigger for GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,                           /*!< Stream Processor Trigger for GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23,                           /*!< Stream Processor Trigger for GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_0,                    /*!< Stream Processor Output Signal to Digital GPIO 0 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_1,                    /*!< Stream Processor Output Signal to Digital GPIO 1 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_2,                    /*!< Stream Processor Output Signal to Digital GPIO 2 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_3,                    /*!< Stream Processor Output Signal to Digital GPIO 3 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_4,                    /*!< Stream Processor Output Signal to Digital GPIO 4 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_5,                    /*!< Stream Processor Output Signal to Digital GPIO 5 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_6,                    /*!< Stream Processor Output Signal to Digital GPIO 6 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_7,                    /*!< Stream Processor Output Signal to Digital GPIO 7 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_8,                    /*!< Stream Processor Output Signal to Digital GPIO 8 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_9,                    /*!< Stream Processor Output Signal to Digital GPIO 9 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_10,                   /*!< Stream Processor Output Signal to Digital GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_11,                   /*!< Stream Processor Output Signal to Digital GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_12,                   /*!< Stream Processor Output Signal to Digital GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_13,                   /*!< Stream Processor Output Signal to Digital GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_14,                   /*!< Stream Processor Output Signal to Digital GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_15,                   /*!< Stream Processor Output Signal to Digital GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_16,                   /*!< Stream Processor Output Signal to Digital GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_17,                   /*!< Stream Processor Output Signal to Digital GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_18,                   /*!< Stream Processor Output Signal to Digital GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_19,                   /*!< Stream Processor Output Signal to Digital GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_20,                   /*!< Stream Processor Output Signal to Digital GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_21,                   /*!< Stream Processor Output Signal to Digital GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_22,                   /*!< Stream Processor Output Signal to Digital GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_23,                   /*!< Stream Processor Output Signal to Digital GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_0,                     /*!< Stream Processor Output Signal to Analog GPIO 0 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_1,                     /*!< Stream Processor Output Signal to Analog GPIO 1 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_2,                     /*!< Stream Processor Output Signal to Analog GPIO 2 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_3,                     /*!< Stream Processor Output Signal to Analog GPIO 3 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_4,                     /*!< Stream Processor Output Signal to Analog GPIO 4 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_5,                     /*!< Stream Processor Output Signal to Analog GPIO 5 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_6,                     /*!< Stream Processor Output Signal to Analog GPIO 6 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_7,                     /*!< Stream Processor Output Signal to Analog GPIO 7 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_8,                     /*!< Stream Processor Output Signal to Analog GPIO 8 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_9,                     /*!< Stream Processor Output Signal to Analog GPIO 9 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_10,                     /*!< Stream Processor Output Signal to Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_11,                     /*!< Stream Processor Output Signal to Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_12,                     /*!< Stream Processor Output Signal to Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_13,                     /*!< Stream Processor Output Signal to Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_14,                     /*!< Stream Processor Output Signal to Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_15,                     /*!< Stream Processor Output Signal to Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PEB,                                         /*!< PPI PEB */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_CLK,                                         /*!< PPI CLK */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_0,                                       /*!< PPI PDI 0 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_1,                                       /*!< PPI PDI 1 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_2,                                       /*!< PPI PDI 2 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_3,                                       /*!< PPI PDI 3 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_4,                                       /*!< PPI PDI 4 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_5,                                       /*!< PPI PDI 5 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_6,                                       /*!< PPI PDI 6 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_7,                                       /*!< PPI PDI 7 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_8,                                       /*!< PPI PDI 8 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_9,                                       /*!< PPI PDI 9 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_10,                                      /*!< PPI PDI 10 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_11,                                      /*!< PPI PDI 11 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_12,                                      /*!< PPI PDI 12 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_13,                                      /*!< PPI PDI 13 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_14,                                      /*!< PPI PDI 14 */
    ADI_ADRV904X_GPIO_SIGNAL_PPI16_PDI_15,                                      /*!< PPI PDI 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ADC_TEST_GEN_ENABLE,                               /*!< Rx Channel(s) Input ADC Test Gen Enable */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_GAIN_CHANGE,                                   /*!< Rx Channel(s) Input AGC Gain Change */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_DEC_GAIN,                                      /*!< Rx Channel(s) Input AGC Decrement Gain */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_INC_GAIN,                                      /*!< Rx Channel(s) Input AGC Increment Gain */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_SLOWLOOP_FREEZE_ENABLE,                        /*!< Rx Channel(s) Input AGC Slowloop Freeze Enable */
    ADI_ADRV904X_GPIO_SIGNAL_AGC_MANUAL_GAIN_LOCK,                              /*!< Rx Channel(s) Input AGC Manual Gain Lock */
    ADI_ADRV904X_GPIO_SIGNAL_SELECT_S1,                                         /*!< Tx Channel(s) Input Select S1 for Tx atten signal */
    ADI_ADRV904X_GPIO_SIGNAL_DTX_FORCE_PIN,                                     /*!< Tx Channel(s) Input DTX Force Pin */
    ADI_ADRV904X_GPIO_SIGNAL_TX_ATTEN_UPD_GPIO,                                 /*!< Tx Channel(s) Input Atten Update */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_0,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_1,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_2,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_3,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_4,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_5,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_6,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_7,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_8,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_9,                       /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_10,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_11,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_12,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_13,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_14,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_OUTPUT_15,                      /*!< GPIO Analog Manual Output (Drive) Mode: Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_0,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_1,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_2,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_3,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_4,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_5,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_6,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_7,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_8,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_9,                        /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_10,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_11,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_12,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_13,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_14,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_MANUAL_MODE_INPUT_15,                       /*!< GPIO Analog Manual Input (Receive) Mode: Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_0,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_1,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_2,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_3,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_4,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_5,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_6,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_7,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_8,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_9,                               /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_10,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_11,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_12,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_13,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_14,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_OUTPUT_15,                              /*!< GPIO Analog ARM Output (Drive) Mode: Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_0,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_1,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_2,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_3,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_4,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_5,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_6,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_7,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_8,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_9,                                /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_10,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_11,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_12,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_13,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_14,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_ARM_INPUT_15,                               /*!< GPIO Analog ARM Input (Receive) Mode: Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX0 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX0 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX1 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX1 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX2 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX2 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX3 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX3 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX4 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX4 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX5 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX5 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX6 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX6 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_0,                                 /*!< Analog GPIO output signal for RX7 Gain Control Word Bit0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_EXT_CONTROL_1,                                 /*!< Analog GPIO output signal for RX7 Gain Control Word Bit1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx0 Band 0 Bit 0. Only for Analog GPIO 0 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx0 Band 0 Bit 1. Only for Analog GPIO 1 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx0 Band 1 Bit 0. Only for Analog GPIO 2 */
    ADI_ADRV904X_GPIO_SIGNAL_RX0_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx0 Band 1 Bit 1. Only for Analog GPIO 3 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx1 Band 0 Bit 0. Only for Analog GPIO 4 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx1 Band 0 Bit 1. Only for Analog GPIO 5 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx1 Band 1 Bit 0. Only for Analog GPIO 6 */
    ADI_ADRV904X_GPIO_SIGNAL_RX1_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx1 Band 1 Bit 1. Only for Analog GPIO 7 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx4 Band 0 Bit 0. Only for Analog GPIO 8 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx4 Band 0 Bit 1. Only for Analog GPIO 9 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx4 Band 1 Bit 0. Only for Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_RX4_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx4 Band 1 Bit 1. Only for Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx5 Band 0 Bit 0. Only for Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx5 Band 0 Bit 1. Only for Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx5 Band 1 Bit 0. Only for Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_RX5_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx5 Band 1 Bit 1. Only for Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx2 Band 0 Bit 0. Only for Analog GPIO 0  */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx2 Band 0 Bit 1. Only for Analog GPIO 1  */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx2 Band 1 Bit 0. Only for Analog GPIO 2  */
    ADI_ADRV904X_GPIO_SIGNAL_RX2_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx2 Band 1 Bit 1. Only for Analog GPIO 3  */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx3 Band 0 Bit 0. Only for Analog GPIO 4  */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx3 Band 0 Bit 1. Only for Analog GPIO 5  */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx3 Band 1 Bit 0. Only for Analog GPIO 6  */
    ADI_ADRV904X_GPIO_SIGNAL_RX3_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx3 Band 1 Bit 1. Only for Analog GPIO 7  */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx6 Band 0 Bit 0. Only for Analog GPIO 8  */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx6 Band 0 Bit 1. Only for Analog GPIO 9  */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx6 Band 1 Bit 0. Only for Analog GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_RX6_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx6 Band 1 Bit 1. Only for Analog GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_0,                      /*!< Dual Band Control Rx7 Band 0 Bit 0. Only for Analog GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND0_1,                      /*!< Dual Band Control Rx7 Band 0 Bit 1. Only for Analog GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_0,                      /*!< Dual Band Control Rx7 Band 1 Bit 0. Only for Analog GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_RX7_DUALBAND_CONTROL_BAND1_1,                      /*!< Dual Band Control Rx7 Band 1 Bit 1. Only for Analog GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_SSB_SYNC,                                /*!< Radio Sequencer SSB SYNC input Signal from a Digital GPIO. Valid on any Digital GPIO 0-23. Particular Radio Seq selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_0,                           /*!< Radio Sequencer Output Signal to Digital GPIO[0]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_1,                           /*!< Radio Sequencer Output Signal to Digital GPIO[1]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_2,                           /*!< Radio Sequencer Output Signal to Digital GPIO[2]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_3,                           /*!< Radio Sequencer Output Signal to Digital GPIO[3]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_4,                           /*!< Radio Sequencer Output Signal to Digital GPIO[4]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_5,                           /*!< Radio Sequencer Output Signal to Digital GPIO[5]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_6,                           /*!< Radio Sequencer Output Signal to Digital GPIO[6]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_7,                           /*!< Radio Sequencer Output Signal to Digital GPIO[7]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_8,                           /*!< Radio Sequencer Output Signal to Digital GPIO[8]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_9,                           /*!< Radio Sequencer Output Signal to Digital GPIO[9]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_10,                          /*!< Radio Sequencer Output Signal to Digital GPIO[10]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_11,                          /*!< Radio Sequencer Output Signal to Digital GPIO[11]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_12,                          /*!< Radio Sequencer Output Signal to Digital GPIO[12]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_13,                          /*!< Radio Sequencer Output Signal to Digital GPIO[13]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_14,                          /*!< Radio Sequencer Output Signal to Digital GPIO[14]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_15,                          /*!< Radio Sequencer Output Signal to Digital GPIO[15]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_16,                          /*!< Radio Sequencer Output Signal to Digital GPIO[16]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_17,                          /*!< Radio Sequencer Output Signal to Digital GPIO[17]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_18,                          /*!< Radio Sequencer Output Signal to Digital GPIO[18]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_19,                          /*!< Radio Sequencer Output Signal to Digital GPIO[19]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_20,                          /*!< Radio Sequencer Output Signal to Digital GPIO[20]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_21,                          /*!< Radio Sequencer Output Signal to Digital GPIO[21]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_22,                          /*!< Radio Sequencer Output Signal to Digital GPIO[22]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_23,                          /*!< Radio Sequencer Output Signal to Digital GPIO[23]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_0,                            /*!< Radio Sequencer Output Signal to Analog GPIO[0]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_1,                            /*!< Radio Sequencer Output Signal to Analog GPIO[1]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_2,                            /*!< Radio Sequencer Output Signal to Analog GPIO[2]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_3,                            /*!< Radio Sequencer Output Signal to Analog GPIO[3]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_4,                            /*!< Radio Sequencer Output Signal to Analog GPIO[4]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_5,                            /*!< Radio Sequencer Output Signal to Analog GPIO[5]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_6,                            /*!< Radio Sequencer Output Signal to Analog GPIO[6]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_7,                            /*!< Radio Sequencer Output Signal to Analog GPIO[7]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_8,                            /*!< Radio Sequencer Output Signal to Analog GPIO[8]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_9,                            /*!< Radio Sequencer Output Signal to Analog GPIO[9]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_10,                           /*!< Radio Sequencer Output Signal to Analog GPIO[10]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_11,                           /*!< Radio Sequencer Output Signal to Analog GPIO[11]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_12,                           /*!< Radio Sequencer Output Signal to Analog GPIO[12]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_13,                           /*!< Radio Sequencer Output Signal to Analog GPIO[13]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_14,                           /*!< Radio Sequencer Output Signal to Analog GPIO[14]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_15,                           /*!< Radio Sequencer Output Signal to Analog GPIO[15]. Particular Radio Seq signal selectable in Radio Seq Xbar Config */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_ENABLE,                                  /*!< CDDC RSSI Enable input Signal from a Digital GPIO. Valid on any Digital GPIO 0-23.*/
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_0,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_1,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_2,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_3,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_4,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_5,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_6,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDDC_RSSI_READY_7,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_TSSI_DUC1_READY,                                /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_0,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_1,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_2,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_3,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_4,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_5,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_6,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_CDUC_TSSI_READY_7,                                 /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_DPD_IN_POWER_READY,                             /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_DPD_OUT_POWER_READY,                            /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_DFE_TSSI_READY_1,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_TX_DFE_TSSI_READY_2,                               /*!< Monitor Out */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRXSIN,                                /*!< Select DFE UART0 Receive Input signal                */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADCTS,                                  /*!< Select DFE UART0 Clear to Send Flow control signal   */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADRTSOUT,                               /*!< Select DFE UART0 Request to Send Flow control signal */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_UART0_PADTXSOUT,                               /*!< Select DFE UART0 Transmit Output signal              */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TRST,                                      /*!< Select DFE ARM TRST signal. For Test mode only */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDO_SWO_MUX,                               /*!< Select DFE ARM TDO SWO MUX signal. For Test mode only */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TDI,                                       /*!< Select DFE ARM TDI signal. For Test mode only */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TMS,                                       /*!< Select DFE ARM TMS signal. For Test mode only */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_ARM_TCK,                                       /*!< Select DFE ARM TCK signal. For Test mode only */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_0,                                            /*!< No-op for GPIO 0. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_1,                                            /*!< No-op for GPIO 1. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_2,                                            /*!< No-op for GPIO 2. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_3,                                            /*!< No-op for GPIO 3. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_4,                                            /*!< No-op for GPIO 4. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_5,                                            /*!< No-op for GPIO 5. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_6,                                            /*!< No-op for GPIO 6. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_7,                                            /*!< No-op for GPIO 7. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_8,                                            /*!< No-op for GPIO 8. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_9,                                            /*!< No-op for GPIO 9. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_10,                                           /*!< No-op for GPIO 10. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_11,                                           /*!< No-op for GPIO 11. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_12,                                           /*!< No-op for GPIO 12. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_13,                                           /*!< No-op for GPIO 13. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_14,                                           /*!< No-op for GPIO 14. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_15,                                           /*!< No-op for GPIO 15. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_16,                                           /*!< No-op for GPIO 16. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_17,                                           /*!< No-op for GPIO 17. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_18,                                           /*!< No-op for GPIO 18. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_19,                                           /*!< No-op for GPIO 19. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_20,                                           /*!< No-op for GPIO 20. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_21,                                           /*!< No-op for GPIO 21. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_22,                                           /*!< No-op for GPIO 22. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_NOOP_23,                                           /*!< No-op for GPIO 23. Unconnected to any function in the device, but treated as allocated. */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_0,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_1,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_2,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_3,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_4,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_5,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_6,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_7,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_8,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_9,                                 /*!< GPIO DFE Control Output (Drive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_10,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_11,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_12,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_13,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_14,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_15,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 15 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_16,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 16 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_17,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 17 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_18,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 18 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_19,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 19 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_20,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 20 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_21,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 21 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_22,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 22 */
    ADI_ADRV904X_GPIO_SIGNAL_DFE_CTRL_OUTPUT_23,                                /*!< GPIO DFE Control Output (Drive) Mode: GPIO 23 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_0,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 00 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_1,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 01 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_2,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 02 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_3,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 03 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_4,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 04 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_5,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 05 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_6,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 06 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_7,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 07 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_8,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 08 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_9,                          /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 09 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_10,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 10 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_11,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 11 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_12,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 12 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_13,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 13 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_14,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 14 */
    ADI_ADRV904X_GPIO_SIGNAL_ANALOG_DFE_CTRL_OUTPUT_15,                         /*!< Analog GPIO DFE Control Output (Drive) Mode: GPIO 15 */
    /***Please add GPIO SIGNALS above this line***/
    ADI_ADRV904X_GPIO_SIGNAL_NUM_SIGNALS,                                       /*!< Total Number of GPIO Signals  */
    ADI_ADRV904X_GPIO_SIGNAL_INVALID                                            
} adi_adrv904x_GpioSignal_e;


/**
* \brief Data structure to hold a full status of all GPIO configurations, both Digital and Analog
*/
typedef struct adi_adrv904x_GpioStatus
{
    uint32_t                    digPinAllocated;                            /*!< Pinmask containing the Digital GPIOs currently allocated for use */
    adi_adrv904x_GpioSignal_e   digSignal[ADI_ADRV904X_GPIO_COUNT];         /*!< Array of all Digital GPIO Signals */
    uint32_t                    digChMask[ADI_ADRV904X_GPIO_COUNT];         /*!< Array of all Digital GPIO Channel Masks */
    uint32_t                    anaPinAllocated;                            /*!< Pinmask containing the Analog GPIOs currently allocated for use */
    adi_adrv904x_GpioSignal_e   anaSignal[ADI_ADRV904X_GPIO_ANALOG_COUNT];  /*!< Array of all Analog GPIO Signals */
    uint32_t                    anaChMask[ADI_ADRV904X_GPIO_ANALOG_COUNT];  /*!< Array of all Analog GPIO Channel Masks */
} adi_adrv904x_GpioStatus_t;

/****************************************************************************
 * GP Int (General Purpose Interrupt) related types
 ****************************************************************************
 */

/**
 * \brief Enum for selecting the GP_INT channel(s)
 */
typedef enum adi_adrv904x_GpIntPinSelect
{
    ADI_ADRV904X_GPINT0,        /*!< GP Interrupt Pin0 select */
    ADI_ADRV904X_GPINT1,        /*!< GP Interrupt Pin1 select */
    ADI_ADRV904X_GPINTALL       /*!< All GP Interrupt pins */
} adi_adrv904x_GpIntPinSelect_e;

/**
 * \brief Data structure to hold a 96bit GP Interrupt Word. Interrupts are split into 48bit Lower/Upper Words.
 * The following table specifies each GP Interrupt source and how it maps to the adi_adrv904x_GpIntWord structure:
 * 
 * | Full(96bit)<br>Word Bit[n] | GP Interrupt<br>Mask Bit      | Word  | Half(48bit)<br>Word Bit[n] | Byte  | Byte<br>Byte Bit[n] |
 * |:--------------------------:|:-----------------------------:|:-----:|:--------------------------:|:-----:|:-------------------:|
 * | 0                          | FRAMER_IRQ_0                  | Lower | 0                          | 0     | 0                   |
 * | 1                          | FRAMER_IRQ_1                  | Lower | 1                          | 0     | 1                   |
 * | 2                          | FRAMER_IRQ_2                  | Lower | 2                          | 0     | 2                   |
 * | 3                          | FRAMER_IRQ_3                  | Lower | 3                          | 0     | 3                   |
 * | 4                          | FRAMER_IRQ_4                  | Lower | 4                          | 0     | 4                   |
 * | 5                          | FRAMER_IRQ_5                  | Lower | 5                          | 0     | 5                   |
 * | 6                          | FRAMER_IRQ_6                  | Lower | 6                          | 0     | 6                   |
 * | 7                          | FRAMER_IRQ_7                  | Lower | 7                          | 0     | 7                   |
 * | 8                          | FRAMER_IRQ_8                  | Lower | 8                          | 1     | 0                   |
 * | 9                          | DEFRAMER_IRQ_0                | Lower | 9                          | 1     | 1                   |
 * | 10                         | DEFRAMER_IRQ_1                | Lower | 10                         | 1     | 2                   |
 * | 11                         | DEFRAMER_IRQ_2                | Lower | 11                         | 1     | 3                   |
 * | 12                         | DEFRAMER_IRQ_3                | Lower | 12                         | 1     | 4                   |
 * | 13                         | DEFRAMER_IRQ_4                | Lower | 13                         | 1     | 5                   |
 * | 14                         | DEFRAMER_IRQ_5                | Lower | 14                         | 1     | 6                   |
 * | 15                         | DEFRAMER_IRQ_6                | Lower | 15                         | 1     | 7                   |
 * | 16                         | DEFRAMER_IRQ_7                | Lower | 16                         | 2     | 0                   |
 * | 17                         | DEFRAMER_IRQ_8                | Lower | 17                         | 2     | 1                   |
 * | 18                         | DEFRAMER_IRQ_9                | Lower | 18                         | 2     | 2                   |
 * | 19                         | DEFRAMER_IRQ_10               | Lower | 19                         | 2     | 3                   |
 * | 20                         | DEFRAMER_IRQ_11               | Lower | 20                         | 2     | 4                   |
 * | 21                         | DEFRAMER_IRQ_12               | Lower | 21                         | 2     | 5                   |
 * | 22                         | DEFRAMER_IRQ_13               | Lower | 22                         | 2     | 6                   |
 * | 23                         | RX0_STREAM_ERROR              | Lower | 23                         | 2     | 7                   |
 * | 24                         | RX1_STREAM_ERROR              | Lower | 24                         | 3     | 0                   |
 * | 25                         | RX2_STREAM_ERROR              | Lower | 25                         | 3     | 1                   |
 * | 26                         | RX3_STREAM_ERROR              | Lower | 26                         | 3     | 2                   |
 * | 27                         | RX4_STREAM_ERROR              | Lower | 27                         | 3     | 3                   |
 * | 28                         | RX5_STREAM_ERROR              | Lower | 28                         | 3     | 4                   |
 * | 29                         | RX6_STREAM_ERROR              | Lower | 29                         | 3     | 5                   |
 * | 30                         | RX7_STREAM_ERROR              | Lower | 30                         | 3     | 6                   |
 * | 31                         | TX0_STREAM_ERROR              | Lower | 31                         | 3     | 7                   |
 * | 32                         | TX1_STREAM_ERROR              | Lower | 32                         | 4     | 0                   |
 * | 33                         | TX2_STREAM_ERROR              | Lower | 33                         | 4     | 1                   |
 * | 34                         | TX3_STREAM_ERROR              | Lower | 34                         | 4     | 2                   |
 * | 35                         | TX4_STREAM_ERROR              | Lower | 35                         | 4     | 3                   |
 * | 36                         | TX5_STREAM_ERROR              | Lower | 36                         | 4     | 4                   |
 * | 37                         | TX6_STREAM_ERROR              | Lower | 37                         | 4     | 5                   |
 * | 38                         | TX7_STREAM_ERROR              | Lower | 38                         | 4     | 6                   |
 * | 39                         | ORX0_STREAM_ERROR             | Lower | 39                         | 4     | 7                   |
 * | 40                         | ORX1_STREAM_ERROR             | Lower | 40                         | 5     | 0                   |
 * | 41                         | CORE_STREAM_ERROR             | Lower | 41                         | 5     | 1                   |
 * | 42                         | AUX_SPI_CORE_ERROR            | Lower | 42                         | 5     | 2                   |
 * | 43                         | ERROR_SPI_PAGING              | Lower | 43                         | 5     | 3                   |
 * | 44                         | ERROR_AUX_SPI_PAGING          | Lower | 44                         | 5     | 4                   |
 * | 45                         | RESERVED                      | Lower | 45                         | 5     | 5                   |
 * | 46                         | RESERVED                      | Lower | 46                         | 5     | 6                   |
 * | 47                         | RESERVED                      | Lower | 47                         | 5     | 7                   |
 * |----------------------------|-------------------------------|-------|----------------------------|-------|---------------------|
 * | 48                         | ARM1_MEMORY_ECC_ERROR         | Upper | 0                          | 6     | 0                   |
 * | 49                         | ARM1_SYSTEM_ERROR             | Upper | 1                          | 6     | 1                   |
 * | 50                         | ARM1_CALIBRATION_ERROR        | Upper | 2                          | 6     | 2                   |
 * | 51                         | ARM1_WATCHDOG_TIMEOUT         | Upper | 3                          | 6     | 3                   |
 * | 52                         | ARM1_FORCE_GP_INTERRUPT       | Upper | 4                          | 6     | 4                   |
 * | 53                         | ARM0_MEMORY_ECC_ERROR         | Upper | 5                          | 6     | 5                   |
 * | 54                         | ARM0_SYSTEM_ERROR             | Upper | 6                          | 6     | 6                   |
 * | 55                         | ARM0_CALIBRATION_ERROR        | Upper | 7                          | 6     | 7                   |
 * | 56                         | ARM0_WATCHDOG_TIMEOUT         | Upper | 8                          | 7     | 0                   |
 * | 57                         | ARM0_FORCE_GP_INTERRUPT       | Upper | 9                          | 7     | 1                   |
 * | 58                         | PA_PROTECTION_TX0_POWER_ERROR | Upper | 10                         | 7     | 2                   |
 * | 59                         | PA_PROTECTION_TX0_SRD_ERROR   | Upper | 11                         | 7     | 3                   |
 * | 60                         | PA_PROTECTION_TX1_POWER_ERROR | Upper | 12                         | 7     | 4                   |
 * | 61                         | PA_PROTECTION_TX1_SRD_ERROR   | Upper | 13                         | 7     | 5                   |
 * | 62                         | PA_PROTECTION_TX2_POWER_ERROR | Upper | 14                         | 7     | 6                   |
 * | 63                         | PA_PROTECTION_TX2_SRD_ERROR   | Upper | 15                         | 7     | 7                   |
 * | 64                         | PA_PROTECTION_TX3_POWER_ERROR | Upper | 16                         | 8     | 0                   |
 * | 65                         | PA_PROTECTION_TX3_SRD_ERROR   | Upper | 17                         | 8     | 1                   |
 * | 66                         | PA_PROTECTION_TX4_POWER_ERROR | Upper | 18                         | 8     | 2                   |
 * | 67                         | PA_PROTECTION_TX4_SRD_ERROR   | Upper | 19                         | 8     | 3                   |
 * | 68                         | PA_PROTECTION_TX5_POWER_ERROR | Upper | 20                         | 8     | 4                   |
 * | 69                         | PA_PROTECTION_TX5_SRD_ERROR   | Upper | 21                         | 8     | 5                   |
 * | 70                         | PA_PROTECTION_TX6_POWER_ERROR | Upper | 22                         | 8     | 6                   |
 * | 71                         | PA_PROTECTION_TX6_SRD_ERROR   | Upper | 23                         | 8     | 7                   |
 * | 72                         | PA_PROTECTION_TX7_POWER_ERROR | Upper | 24                         | 9     | 0                   |
 * | 73                         | PA_PROTECTION_TX7_SRD_ERROR   | Upper | 25                         | 9     | 1                   |
 * | 74                         | SERDES_PLL_UNLOCK             | Upper | 26                         | 9     | 2                   |
 * | 75                         | CLKPLL_OVR_RANGE              | Upper | 27                         | 9     | 3                   |
 * | 76                         | RFPLL1_CP_OVR_RANGE           | Upper | 28                         | 9     | 4                   |
 * | 77                         | RFPLL0_CP_OVR_RANGE           | Upper | 29                         | 9     | 5                   |
 * | 78                         | CLKPLL_UNLOCK                 | Upper | 30                         | 9     | 6                   |
 * | 79                         | RFPLL1_UNLOCK                 | Upper | 31                         | 9     | 7                   |
 * | 80                         | RFPLL0_UNLOCK                 | Upper | 32                         | 10    | 0                   |
 * | 81                         | SPI_AHB_ERROR                 | Upper | 33                         | 10    | 1                   |
 * | 82                         | AUX_SPI_AHB_ERROR             | Upper | 34                         | 10    | 2                   |
 * | 83                         | SPI_CLK_EFUSE_READ_ABORT      | Upper | 35                         | 10    | 3                   |
 * | 84                         | AUX_SPI_ABORT                 | Upper | 36                         | 10    | 4                   |
 * | 85                         | SPI_ABORT                     | Upper | 37                         | 10    | 5                   |
 * | 86                         | DFE_CPU_AND_MEM_INT_0         | Upper | 38                         | 10    | 6                   |
 * | 87                         | DFE_CPU_AND_MEM_INT_1         | Upper | 39                         | 10    | 7                   |
 * | 88                         | DFE_CPU_AND_MEM_INT_2         | Upper | 40                         | 11    | 0                   |
 * | 89                         | RESERVED                      | Upper | 41                         | 11    | 1                   |
 * | 90                         | DFE_CPU_AND_MEM_INT_3         | Upper | 42                         | 11    | 2                   |
 * | 91                         | DFE_CPU_AND_MEM_INT_4         | Upper | 43                         | 11    | 3                   |
 * | 92                         | DFE_CPU_PINT_SW0              | Upper | 44                         | 11    | 4                   |
 * | 93                         | DFE_CPU_PINT_SW1              | Upper | 45                         | 11    | 5                   |
 * | 94                         | RESERVED                      | Upper | 46                         | 11    | 6                   |
 * | 95                         | RADIO_SEQ_ERROR               | Upper | 47                         | 11    | 7                   |
 * |----------------------------|-------------------------------|-------|----------------------------|-------|---------------------|
*/
typedef struct adi_adrv904x_GpIntMask
{
    uint64_t lowerMask;         /*!< Lower 48 GP Interrupt bits of entire 96 Bit Mask */
    uint64_t upperMask;         /*!< Upper 48 GP Interrupt bits of entire 96 Bit Mask */
} adi_adrv904x_GpIntMask_t;

/**
* \brief Data structure holding the GP Interrupt Mask Words for all GP Int Pins
*/
typedef struct adi_adrv904x_GpIntPinMaskCfg
{
    adi_adrv904x_GpIntMask_t gpInt0Mask;    /*!< 96 bit GP Interrupt Mask Word for GP Int Pin0 */
    adi_adrv904x_GpIntMask_t gpInt1Mask;    /*!< 96 bit GP Interrupt Mask Word for GP Int Pin1 */
} adi_adrv904x_GpIntPinMaskCfg_t;



/**
 * \brief Digital Pin Name
 */
typedef enum adi_adrv904x_GpioDigitalPin
{
    ADI_ADRV904X_IO_GPIO_0               = 0x02U,
    ADI_ADRV904X_IO_GPIO_1               = 0x01U,
    ADI_ADRV904X_IO_GPIO_2               = 0x40U,
    ADI_ADRV904X_IO_GPIO_3               = 0x42U,
    ADI_ADRV904X_IO_GPIO_4               = 0x12U,
    ADI_ADRV904X_IO_GPIO_5               = 0x00U,
    ADI_ADRV904X_IO_GPIO_6               = 0x41U,
    ADI_ADRV904X_IO_GPIO_7               = 0x50U,
    ADI_ADRV904X_IO_GPIO_8               = 0x13U,
    ADI_ADRV904X_IO_GPIO_9               = 0x51U,
    ADI_ADRV904X_IO_GPIO_10              = 0x10U,
    ADI_ADRV904X_IO_GPIO_11              = 0x52U,
    ADI_ADRV904X_IO_GPIO_12              = 0x23U,
    ADI_ADRV904X_IO_GPIO_13              = 0x65U,
    ADI_ADRV904X_IO_GPIO_14              = 0x21U,
    ADI_ADRV904X_IO_GPIO_15              = 0x22U,
    ADI_ADRV904X_IO_GPIO_16              = 0x63U,
    ADI_ADRV904X_IO_GPIO_17              = 0x60U,
    ADI_ADRV904X_IO_GPIO_18              = 0x20U,
    ADI_ADRV904X_IO_GPIO_19              = 0x24U,
    ADI_ADRV904X_IO_GPIO_20              = 0x62U,
    ADI_ADRV904X_IO_GPIO_21              = 0x61U,
    ADI_ADRV904X_IO_GPIO_22              = 0x35U,
    ADI_ADRV904X_IO_GPIO_23              = 0x70U,
    ADI_ADRV904X_O_GPINT_0               = 0x72U,
    ADI_ADRV904X_O_GPINT_1               = 0x33U,
    ADI_ADRV904X_I_TRX0_EN               = 0x03U,
    ADI_ADRV904X_I_TRX1_EN               = 0x13U,
    ADI_ADRV904X_I_TRX2_EN               = 0x15U,
    ADI_ADRV904X_I_TRX3_EN               = 0x25U,
    ADI_ADRV904X_I_TRX4_EN               = 0x43U,
    ADI_ADRV904X_I_TRX5_EN               = 0x54U,
    ADI_ADRV904X_I_TRX6_EN               = 0x55U,
    ADI_ADRV904X_I_TRX7_EN               = 0x64U,
    ADI_ADRV904X_I_ORX0_EN               = 0x14U,
    ADI_ADRV904X_I_ORX1_EN               = 0x53U,
    ADI_ADRV904X_I_RESETB                = 0x30U,
    ADI_ADRV904X_I_SPI_ENB               = 0x75U,
    ADI_ADRV904X_I_SPI_CLK               = 0x31U,
    ADI_ADRV904X_IO_SPI_DIO              = 0x32U,
    ADI_ADRV904X_O_SPI_DO                = 0x71U,
} adi_adrv904x_GpioDigitalPin_e;

#endif /* _ADI_ADRV904X_GPIO_TYPES_H_ */
