/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/
 
/**
* \file adi_fpgagen6_gpio_types.h
* \brief Contains FPGAGEN6 GPIO enum and struct definitions
*
* FPGA API Version: 2.9.0.4
*/


#ifndef _ADI_FPGAGEN6_GPIO_TYPES_H_
#define _ADI_FPGAGEN6_GPIO_TYPES_H_

#include "adi_library_types.h"

#define ADI_FPGAGEN6_GPIO_MASK_MAX_VALUE 0xFFFFFFU
#define ADI_FPGAGEN6_NUM_SPI2_PINS 4U
#define ADI_FPGAGEN6_NUM_GPINT_PINS 2U

/**
* \brief Enum that holds the valid GPIO modes
*/
typedef enum adi_fpgagen6_GpioModes
{
    /**
    * \brief    Default mode. Setting this mode in adi_fpgagen6_GpioModeSet
    *           will clear any JTAG and SPI2 settings and return those GPIO
    *           pins to inputs. This will not clear Tdd mode however, use
    *           adi_fpgagen6_TddGpioAssignClear.
    */
    ADI_FPGAGEN6_GPIO_MODE_NONE = 0x0U,
    
    /**
    * \brief    FW JTAG debug mode. When in JTAG mode
    *           GPIO pins 7:0 are reserved.
    */
    ADI_FPGAGEN6_GPIO_MODE_JTAG,
    
    /**
    * \brief    SPI2 mode. When in SPI2 mode
    *           GPIO pins 3:0 are reserved.
    */
    ADI_FPGAGEN6_GPIO_MODE_SPI2,
    
    /**
    * \brief    TDD mode. Any GPIO pins can
    *           be used for TDD mode. Use adi_fpgagen6_TddGpioAssignSet
    *           to set a GPIO pin in this mode.
    */
    ADI_FPGAGEN6_GPIO_MODE_TDD,
            /**
    * \brief    GPIO Sel Trigger mode. Any GPIO pins can
    *           be used for GPIO Sel Trigger mode. Use adi_fpgagen6_TddGpioAssignSet
    *           to set a GPIO pin in this mode.
    */
    ADI_FPGAGEN6_GPIO_MODE_SEL_TRIGGER
} adi_fpgagen6_GpioModes_e;

/**
* \brief Enum that abstracts input and output functionality per pin
*/
typedef enum adi_fpgagen6_GpioPinType
{
    ADI_FPGAGEN6_GPIO_PIN_OUTPUT = 0x0U,
    ADI_FPGAGEN6_GPIO_PIN_INPUT = 0x1U
} adi_fpgagen6_GpioPinType_e;

/**
* \brief Enum that holds all the valid GPIOs and a single invalid selection
*/
typedef enum adi_fpgagen6_GpioPinSel
{
    ADI_FPGAGEN6_GPIO_00 = 0x0U,    /*!< Select GPIO_00 */
    ADI_FPGAGEN6_GPIO_01,           /*!< Select GPIO_01 */
    ADI_FPGAGEN6_GPIO_02,           /*!< Select GPIO_02 */
    ADI_FPGAGEN6_GPIO_03,           /*!< Select GPIO_03 */
    ADI_FPGAGEN6_GPIO_04,           /*!< Select GPIO_04 */
    ADI_FPGAGEN6_GPIO_05,           /*!< Select GPIO_05 */
    ADI_FPGAGEN6_GPIO_06,           /*!< Select GPIO_06 */
    ADI_FPGAGEN6_GPIO_07,           /*!< Select GPIO_07 */
    ADI_FPGAGEN6_GPIO_08,           /*!< Select GPIO_08 */
    ADI_FPGAGEN6_GPIO_09,           /*!< Select GPIO_09 */
    ADI_FPGAGEN6_GPIO_10,           /*!< Select GPIO_10 */
    ADI_FPGAGEN6_GPIO_11,           /*!< Select GPIO_11 */
    ADI_FPGAGEN6_GPIO_12,           /*!< Select GPIO_12 */
    ADI_FPGAGEN6_GPIO_13,           /*!< Select GPIO_13 */
    ADI_FPGAGEN6_GPIO_14,           /*!< Select GPIO_14 */
    ADI_FPGAGEN6_GPIO_15,           /*!< Select GPIO_15 */
    ADI_FPGAGEN6_GPIO_16,           /*!< Select GPIO_16 */
    ADI_FPGAGEN6_GPIO_17,           /*!< Select GPIO_17 */
    ADI_FPGAGEN6_GPIO_18,           /*!< Select GPIO_18 */
    ADI_FPGAGEN6_GPIO_19,           /*!< Select GPIO_19 */
    ADI_FPGAGEN6_GPIO_20,           /*!< Select GPIO_20 */
    ADI_FPGAGEN6_GPIO_21,           /*!< Select GPIO_21 */
    ADI_FPGAGEN6_GPIO_22,           /*!< Select GPIO_22 */
    ADI_FPGAGEN6_GPIO_23,           /*!< Select GPIO_23 */
    ADI_FPGAGEN6_GPIO_INVALID       /*!< Invalid GPIO */
} adi_fpgagen6_GpioPinSel_e;

#define ADI_FPGAGEN6_GPIO_COUNT ADI_FPGAGEN6_GPIO_INVALID

/**
* \brief Enum that holds FPGA GP Interrupt Detection Type setting: Level vs. Edge
*/
typedef enum adi_fpgagen6_GpIntDetectionType
{
    ADI_FPGAGEN6_GPINT_DETECT_TYPE_LEVEL = 0U,   /*!< Detect levels of a GP Interrupt signal */
    ADI_FPGAGEN6_GPINT_DETECT_TYPE_EDGE  = 1U    /*!< Detect edges of a GP Interrupt signal */
} adi_fpgagen6_GpIntDetectionType_e;

/**
* \brief Enum that holds FPGA GP Interrupt Detection Polarity setting: Low vs. High
*/
typedef enum adi_fpgagen6_GpIntDetectionPolarity
{
    ADI_FPGAGEN6_GPINT_DETECT_POLARITY_LOW  = 0U,    /*!< Detect low level or falling edge of a GP Interrupt signal */
    ADI_FPGAGEN6_GPINT_DETECT_POLARITY_HIGH = 1U     /*!< Detect high level or rising edge of a GP Interrupt signal */
} adi_fpgagen6_GpIntDetectionPolarity_e;

/**
*  \brief Data structure for the configuration of a single FPGA GP Interrupt pin
*/
typedef struct adi_fpgagen6_GpIntCfgPin
{
    adi_fpgagen6_GpIntDetectionType_e       detectionType;      /*!< GP Interrupt Pin Detection Type */
    adi_fpgagen6_GpIntDetectionPolarity_e   detectionPolarity;  /*!< GP Interrupt Pin Detection Polarity */
} adi_fpgagen6_GpIntCfgPin_t;

/**
*  \brief Data structure for the configuration of all FPGA GP Interrupt pins
*/
typedef struct adi_fpgagen6_GpIntCfg
{
    adi_fpgagen6_GpIntCfgPin_t gpIntPin[ADI_FPGAGEN6_NUM_GPINT_PINS];    /*!< Array of all GP Interrupt Pin Configurations */
} adi_fpgagen6_GpIntCfg_t;

/**
*  \brief Data structure for the configuration of all FPGA GP Interrupt pins
*/
typedef struct adi_fpgagen6_GpIntStatus
{
    uint8_t gpIntPin[ADI_FPGAGEN6_NUM_GPINT_PINS];  /*!< Array of GP Interrupt Pin Status Bits */
} adi_fpgagen6_GpIntStatus_t;

/**
* \brief Enum that defines GPIO SMA PIN
*/
typedef enum adi_fpgagen6_GpioSmaPin
{
    ADI_FPGAGEN6_GPIO_SMA_PIN_0 = 0U,    /*!< GPIO SMA Pin 0 */
    ADI_FPGAGEN6_GPIO_SMA_PIN_1,         /*!< GPIO SMA Pin 1 */
    ADI_FPGAGEN6_GPIO_SMA_PIN_2,         /*!< GPIO SMA Pin 2 */
    ADI_FPGAGEN6_GPIO_SMA_PIN_3          /*!< GPIO SMA Pin 3 */
} adi_fpgagen6_GpioSmaPin_e;

/**
* \brief Enum that defines Rx Datapath
*/
typedef enum adi_fpgagen6_RxDatapath
{
    ADI_FPGAGEN6_RX_DATAPATH_0 = 0U, /*!< Rx Datapath 0 */
    ADI_FPGAGEN6_RX_DATAPATH_1,      /*!< Rx Datapath 1 */
    ADI_FPGAGEN6_RX_DATAPATH_2,      /*!< Rx Datapath 2 */
} adi_fpgagen6_RxDatapath_e;

/**
* \brief Enum that defines Tx Datapath
*/
typedef enum adi_fpgagen6_TxDatapath
{
    ADI_FPGAGEN6_TX_DATAPATH_0 = 0U,    /*!< Tx Datapath 0 */
    ADI_FPGAGEN6_TX_DATAPATH_1,         /*!< Tx Datapath 1 */
} adi_fpgagen6_TxDatapath_e;


#endif  /* _ADI_FPGAGEN6_GPIO_TYPES_H_ */
