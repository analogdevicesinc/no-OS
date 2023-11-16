/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_axi_ads10v1_types.h
* \brief Contains top level fpga related type definitions for
*        adi_fpgagen6_axi_adv_spi.h
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_AXI_ADS10V1_TYPES_H_
#define _ADI_FPGAGEN6_AXI_ADS10V1_TYPES_H_

#include "adi_library_types.h"

#define ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN_NUM             4U      /*!< Number of SMA Sources */
#define ADI_FPGAGEN_AXI_ADS10V1_SMA_OUTPUT_MAX_NUM     16U      /* Number of SMA Destinations */

#define ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN0_IDX    0U      /*!< SMA Pin 0 */
#define ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN1_IDX    1U      /*!< SMA Pin 1 */
#define ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN2_IDX    2U      /*!< SMA Pin 2 */
#define ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN3_IDX    3U      /*!< SMA Pin 3 */

#define ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_OUTPUT    0U
#define ADI_FPGAGEN_AXI_ADS10V1_SMA_DIRECTION_INPUT     1U

#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_CONSTANT_LOW           0x0U /*!< Constant Low */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_CONSTANT_HIGH          0x1U /*!< Constant High */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_TDD_EXT                0x2U /*!< TDD External Trigger */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_TX_WAVEFORM_SRT_PULSE  0x3U /*!< Transmit Waveform Start Pulse */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_IDX_MULTI_PLATFORM_SYNC    0x4U /*!< Multi Platform Synchronization */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_SMA_SRC                    0x5U /*!< SMA Source */

#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK0       0x0U  /*!< Receive DMA External Trigger Link 0 */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK1       0x1U  /*!< Receive DMA External Trigger Link 1 */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_RX_DMA_EXT_LINK2       0x2U  /*!< Receive DMA External Trigger Link 2 */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TX_DMA_EXT_LINK0       0x3U  /*!< Transmit DMA External Trigger Link 0 */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TX_DMA_EXT_LINK1       0x4U  /*!< Transmit DMA External Trigger Link 1 */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_TDD_FSM_EXT            0x5U  /*!< Initiate TDD State Machine External Trigger */
#define ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_IDX_MULTI_PLATFORM_SYNC    0x6U  /*!< Multi Platform Synchronization */

#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_DISABLE_ALL   0x00U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_LED1   0x01U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_LED2   0x02U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_LED3   0x04U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_LED4   0x08U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_LED5   0x10U
#define ADI_FPGAGEN6_AXI_ADS10V1_LED_MASK_ENABLE_ALL    0x3FU

/**
* \brief Axi ADS10v1 SMA Pin Selection
*/
typedef enum adi_fpgagen6_AxiAds10v1SmaSel
{
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_NO_SELECTION = 0x0U,           /*!< No Selection; Do Nothing */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_CONSTANT_LOW,           /*!< Constant Low */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_CONSTANT_HIGH,          /*!< Constant High */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_TDD_EXT,                /*!< TDD External Trigger */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_TX_WAVEFORM_SRT_PULSE,  /*!< Transmit Waveform Start Pulse */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_MULTI_PLATFORM_SYNC,    /*!< Multi Platform Synchronization (Output) */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_OUTPUT_GPIO_SRC,               /*!< GPIO Source */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK0,        /*!< Receive DMA External Trigger Link 0 */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK1,        /*!< Receive DMA External Trigger Link 1 */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_RX_DMA_EXT_LINK2,        /*!< Receive DMA External Trigger Link 2 */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TX_DMA_EXT_LINK0,        /*!< Transmit DMA External Trigger Link 0 */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TX_DMA_EXT_LINK1,        /*!< Transmit DMA External Trigger Link 1 */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_TDD_FSM_EXT,             /*!< Initiate TDD State Machine External Trigger */
    ADI_FPGAGEN6_AXI_ADS10V1_SMA_INPUT_MULTI_PLATFORM_SYNC      /*!< Multi Platform Synchronization (Input) */
} adi_fpgagen6_AxiAds10v1SmaSel_e;

/**
*  \brief SMA Configuration
*/
typedef struct adi_fpgagen6_Ads10v1SmaPinCfg
{
    adi_fpgagen6_AxiAds10v1SmaSel_e smaPin[ADI_FPGAGEN_AXI_ADS10V1_SMA_PIN_NUM];
} adi_fpgagen6_Ads10v1SmaPinCfg_t;

/**
{
* \brief Axi ADS10v1 LED Mode
*/
typedef enum adi_fpgagen6_AxiAds10v1LedMode
{
    ADI_FPGAGEN6_AXI_ADS10V1_LED_MODE_NORMAL        = 0x0U, /*!< Normal Operation - LEDs are controlled by module inputs */
    ADI_FPGAGEN6_AXI_ADS10V1_LED_MODE_RUNNING       = 0x1U, /*!< Running pattern (KITT) */
    ADI_FPGAGEN6_AXI_ADS10V1_LED_MODE_BLINK         = 0x2U, /*!< Blink */
    ADI_FPGAGEN6_AXI_ADS10V1_LED_MODE_SW_DEFINED    = 0x3U  /*!< SW Defined */
} adi_fpgagen6_AxiAds10v1LedMode_e;

/**
*  \brief Led Configuration
*/
typedef struct adi_fpgagen6_Ads10v1LedCfg
{
    adi_fpgagen6_AxiAds10v1LedMode_e    mode;           /*!< LED Mode */
    uint16_t                            delayMs;        /*!< Delay (ms) i.e. N+1 * 1ms */
    uint16_t                            blinkDuration;  /*!< Blink Duration (Mode Dependant) */
    uint8_t                             blinkNum;       /*!< Number of Blinks (Mode Dependant)*/
    uint8_t                             mask;           /*!< LED Selection */
    uint8_t                             swDefined;      /*!< Sw Defined (See Register Map for Information) */
} adi_fpgagen6_Ads10v1LedCfg_t;


#endif /* _ADI_FPGAGEN6_AXI_ADS10V1_TYPES_H_ */
