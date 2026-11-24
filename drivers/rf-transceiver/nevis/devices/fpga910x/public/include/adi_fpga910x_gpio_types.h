/**
 * \file
 * \brief Contains FPGA910X GPIO enum and struct definitions
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_GPIO_TYPES_H_
#define _ADI_FPGA910X_GPIO_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

#define ADI_FPGA910X_NUM_GPIO_PINS 12

/**
 * \brief Possible modes of operation for FPGA910X GPIO
 */
typedef enum adi_fpga910x_OperationModes
{
    ADI_FPGA910X_OPERATION_MODE_NORMAL = 0,
    ADI_FPGA910X_OPERATION_MODE_JTAG
} adi_fpga910x_OperationModes_e;

/**
 * \brief Possible modes to configure the FPGA GPIOs
 */
typedef enum adi_fpga910x_GpioMode
{
    ADI_FPGA910X_GPIO_MODE_SOFTWARE = 0,
    ADI_FPGA910X_GPIO_MODE_HARDWARE
} adi_fpga910x_GpioMode_e;

/**
 * \brief Possible direction to configure the FPGA GPIOs
 */
typedef enum adi_fpga910x_GpioDirection
{
    ADI_FPGA910X_GPIO_DIRECTION_INPUT = 0,
    ADI_FPGA910X_GPIO_DIRECTION_OUTPUT
} adi_fpga910x_GpioDirection_e;

/**
 * \brief Possible data out for FPGA GPIOs 0: outputs continuous ZEROs 1: outputs continuous ONEs
 */
typedef enum adi_fpga910x_GpioDataOut
{
    ADI_FPGA910X_GPIO_DATAOUT_CLEAR = 0,
    ADI_FPGA910X_GPIO_DATAOUT_SET
} adi_fpga910x_GpioDataOut_e;

/**
 * \brief Possible HW source select modes for FPGA910X GPIO
 */
typedef enum adi_fpga910x_GpioHWSourceModes
{
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_TX1_ENABLE = 0x00,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_RX1_ENABLE = 0x01,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_ORX1_ENABLE = 0x02,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_1 = 0x03,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_RX2_ENABLE = 0x04,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_ORX2_ENABLE = 0x05,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_2 = 0x06,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_3 = 0x07,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_4 = 0x08,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_5 = 0x09,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_GENERAL_PURPOSE_6 = 0x0A,
    ADI_FPGA910X_GPIOHWSOURCEMODE_SELECT_TDD_UNASSIGNED = 0x0B
} adi_fpga910x_GpioHWSourceModes_e;

/**
 * \brief FPGA910X GPIO pin selections
 */
typedef enum adi_fpga910x_GpioPin
{
    ADI_FPGA910X_GPIO_UNASSIGNED,
    ADI_FPGA910X_GPIO_00,
    ADI_FPGA910X_GPIO_01,
    ADI_FPGA910X_GPIO_02,
    ADI_FPGA910X_GPIO_03,
    ADI_FPGA910X_GPIO_04,
    ADI_FPGA910X_GPIO_05,
    ADI_FPGA910X_GPIO_06,
    ADI_FPGA910X_GPIO_07,
    ADI_FPGA910X_GPIO_08,
    ADI_FPGA910X_GPIO_09,
    ADI_FPGA910X_GPIO_10,
    ADI_FPGA910X_GPIO_11,
    ADI_FPGA910X_GPIO_12,
    ADI_FPGA910X_GPIO_13,
    ADI_FPGA910X_GPIO_14,
    ADI_FPGA910X_GPIO_15,
    ADI_FPGA910X_GPIO_16,
    ADI_FPGA910X_GPIO_17,
    ADI_FPGA910X_GPIO_18,
    ADI_FPGA910X_GPIO_19
} adi_fpga910x_GpioPin_e;

/**
 * \brief FPGA910X GPIO group selections
 */
typedef enum adi_fpga910x_GpioGroup
{
    ADI_FPGA910X_GPIO_GROUP_DGPIO = 0,
    ADI_FPGA910X_GPIO_GROUP_RX0,
    ADI_FPGA910X_GPIO_GROUP_RX1,
    ADI_FPGA910X_GPIO_GROUP_TX0
} adi_fpga910x_GpioGroup_e;

/**
 * \brief The desired configuration to assign FPGA GPIO pins as digital GPIO
 */
typedef struct adi_fpga910x_GpioPinCfg
{
    adi_fpga910x_GpioMode_e mode;                  /*!< Software or Hardware mode */
    adi_fpga910x_GpioDirection_e direction;        /*!< Input or Output */
    adi_fpga910x_GpioDataOut_e dataOut;            /*!< Set or Clear i.e. transmit 1 or 0 */
    adi_fpga910x_GpioPin_e pin;                    /*!< FPGA GPIO Pins to be used to route the signals from/to ADRV910X */
    adi_fpga910x_GpioHWSourceModes_e hwSourceMode; /*!< HW source select mode for the specified FPGA910X GPIO */
} adi_fpga910x_GpioPinCfg_t;

/**
 * \brief The desired configuration to assign FPGA GPIO pins as observation GPIO to capture gain index
 */
typedef struct adi_fpga910x_GpioGainIndexCfg
{
    adi_fpga910x_GpioGroup_e group;          /*!< GPIO group for observation */
    adi_fpga910x_GpioPin_e readGainIndex[8]; /*!< True: Gain Index; False: Other observation signal
  *!< The gain index is a collection of 7 GPIO ports.
  *!< Any GPIO port may be mapped to any one of the gain index data bits.
  *!< The core outputs an 8 bit gain index with the MSB set to 0x0 */
} adi_fpga910x_GpioGainIndexCfg_t;

/**
 * \brief The desired configuration for FPGA GPIO trigger source
 */
typedef struct adi_fpga910x_GpioTriggerCfg
{
    adi_fpga910x_GpioGroup_e group;       /*!< GPIO group for observation */
    adi_fpga910x_GpioPin_e triggerSource; /*!< GPIO pin trigger source */
} adi_fpga910x_GpioTriggerCfg_t;

/**
 * \brief The desired configuration for SMA pin configuration
 */
typedef struct adi_fpga910x_GpioSmaCfg
{
    bool smaPin1; /*!< Direction of SMA Pin1; True:Output, False:Input */
    bool smaPin2; /*!< Direction of SMA Pin2; True:Output, False:Input */
} adi_fpga910x_GpioSmaCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_GPIO_TYPES_H_ */
