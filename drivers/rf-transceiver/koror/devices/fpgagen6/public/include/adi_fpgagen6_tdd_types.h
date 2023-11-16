/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_tdd_types.h
 * \brief Contains FPGA TDD enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_TDD_TYPES_H_
#define _ADI_FPGAGEN6_TDD_TYPES_H_

#include "adi_library_types.h"
#include "adi_fpgagen6_gpio_types.h"

#define ADI_FPGAGEN6_NUM_TDD_REGIONS                4U

#define ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX     8U
#define ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX     2U
#define ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO    8U
#define ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT     4U

#define ADI_FPGAGEN6_NUM_TDD_DATAPATH_TX            2U
#define ADI_FPGAGEN6_NUM_TDD_DATAPATH_RX            3U


/**
*  \brief TDD Manual Enable Masks
*/
typedef struct adi_fpgagen6_TddManualEnableMasks
{
    uint8_t trxMask;    /*!< TRX Manual Enable Mask */
    uint8_t orxMask;    /*!< ORX Manual Enable Mask */
    uint8_t gpioMask;   /*!< GPIO Manual Enable Mask */
    uint8_t extMask;    /*!< External Manual Enable Mask */
} adi_fpgagen6_TddManualEnableMasks_t;


/**
*  \brief   FPGAGEN6 TDD Enable Control Module Configuration
*/
typedef struct adi_fpgagen6_TddFrameCounter
{
    uint32_t    endCount;           /*!< The value at which the frame counter truncates and loops again */
    uint16_t    loopCount;          /*!< Number of roll overs the frame counter undergo; 0 equals infinite */
    uint8_t     loopCountEnable;    /*!< Enable bit for the frame count roll over */
    uint8_t     extTriggerEnable;   /*!< This bit enables the TDD frame counter to start on the assertion of external trigger pulse */
} adi_fpgagen6_TddFrameCounter_t;

/**
*  \brief   FPGAGEN6 TDD Enable Control Module Configuration
*/
typedef struct adi_fpgagen6_TddEnableControl
{
    uint32_t    start[ADI_FPGAGEN6_NUM_TDD_REGIONS];        /*!< Start Value for Enable Signal */
    uint32_t    duration[ADI_FPGAGEN6_NUM_TDD_REGIONS];     /*!< Duration Value for Enable Signal (N-1) */
    uint16_t    framesOffset;                               /*!< Indicates the number of offset frames after which the enable signals are generated */
    uint16_t    framesActive;                               /*!< Indicates the number of frames only in which enables are generated */
    uint16_t    framesInactive;                             /*!< Indicates the number of frames in which the generation of enable signals is absent */
} adi_fpgagen6_TddEnableControl_t;

/**
*  \brief   FPGAGEN6 TDD Datapath Control Module Configuration
*/
typedef struct adi_fpgagen6_TddDatapathControl
{
    uint32_t start;     /*!< Start Value for Datapath Control Module. Effectively the Datapath Delay value */
} adi_fpgagen6_TddDatapathControl_t;

/**
*  \brief   FPGAGEN6 TDD GPIO Sync
*/
typedef enum adi_fpgagen6_TddGpioSync
{
    ADI_FPGAGEN6_TDD_GPIO_SYNC_DISABLE = 0x00,     /*!< Disable  */
    ADI_FPGAGEN6_TDD_GPIO_SYNC_ENABLE = 0x01,      /*!< Enable  */
    ADI_FPGAGEN6_TDD_GPIO_SYNC_CONTINUOUS = 0x03   /*!< Continuous sync  */
} adi_fpgagen6_TddGpioSync_e;

/**
*  \brief   FPGAGEN6 TDD GPIO Sync Select enable
*/
typedef struct adi_fpgagen6_TddGpioSyncSelEn
{
    uint8_t                      gpioSyncSel;        /*!< The selected GPIO pin */
    adi_fpgagen6_TddGpioSync_e   gpioSyncEnable;     /*!< GPIO Sync setting as defined in adi_fpgagen6_TddGpioSync_e */
} adi_fpgagen6_TddGpioSyncSelEn_t;

/**
*  \brief   FPGAGEN6 TDD State Machine Configuration
*/
typedef struct adi_fpgagen6_TddStateMachineCfg
{
    adi_fpgagen6_TddFrameCounter_t      frameCounter;                                       /*!< Frame Counter */
    adi_fpgagen6_TddEnableControl_t     trx[ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_TRX];       /*!< TRX TDD Enable Control Module */
    adi_fpgagen6_TddEnableControl_t     orx[ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_ORX];       /*!< ORX TDD Enable Control Module */
    adi_fpgagen6_TddEnableControl_t     gpio[ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_GPIO];     /*!< GPIO TDD Enable Control Module */
    adi_fpgagen6_TddEnableControl_t     ext[ADI_FPGAGEN6_NUM_TDD_ENABLE_CONTROL_EXT];       /*!< EXT TDD Enable Control Module */
    adi_fpgagen6_TddDatapathControl_t   tx[ADI_FPGAGEN6_NUM_TDD_DATAPATH_TX];               /*!< TX TDD Datapath Control Module */
    adi_fpgagen6_TddDatapathControl_t   rx[ADI_FPGAGEN6_NUM_TDD_DATAPATH_RX];               /*!< RX TDD Datapath Control Module */
    adi_fpgagen6_TddGpioSyncSelEn_t     gpioSyncSelEnable;                                  /*!< GPIO TDD Sync Select enable */
} adi_fpgagen6_TddStateMachineCfg_t;


#endif /* _ADI_FPGAGEN6_TDD_TYPES_H_ */
