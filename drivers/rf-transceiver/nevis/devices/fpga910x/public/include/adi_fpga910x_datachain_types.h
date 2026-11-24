/**
 * \file
 * \brief Contains FPGA910X Data Chain types
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_DATACHAIN_TYPES_H_
#define _ADI_FPGA910X_DATACHAIN_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "stdint.h"

#define ADI_FPGA910X_NUM_RX_DATA_CHAINS 2
#define ADI_FPGA910X_NUM_TX_DATA_CHAINS 1
#define ADI_FPGA910X_NUM_DATA_CHAINS (ADI_FPGA910X_NUM_RX_DATA_CHAINS + ADI_FPGA910X_NUM_TX_DATA_CHAINS)

/**
 * \brief Enumerated list of valid FPGA Rx capture type
 */
typedef enum adi_fpga910x_CaptureType
{
    ADI_FPGA910X_CAPTURE_TYPE_DISABLE = 0x0,
    ADI_FPGA910X_CAPTURE_TYPE_RX_DATA = 0x1,
    ADI_FPGA910X_CAPTURE_TYPE_RX_GAIN = 0x2
} adi_fpga910x_CaptureType_e;

/**
 * \brief Enumerated list of FPGA DMA trigger sources
 */
typedef enum adi_fpga910x_DmaTrigger
{
    ADI_FPGA910X_DMA_TRIGGER_SMA_1 = 0,
    ADI_FPGA910X_DMA_TRIGGER_SMA_2 = 1,
    ADI_FPGA910X_DMA_TRIGGER_SYNC = 2,  /*!< DMA Trigger source is fpga910X MCS signal. Used for synchronous transfer */
    ADI_FPGA910X_DMA_TRIGGER_GPIO = 3,
    ADI_FPGA910X_DMA_TRIGGER_TDD_ENABLE = 4,
    ADI_FPGA910X_DMA_TRIGGER_IMMEDIATE = 5
} adi_fpga910x_DmaTrigger_e;

/**
 * \brief Data structure to hold the FPGA Data Chain DMA configuration
 */
typedef struct adi_fpga910x_DmaCfg
{
    uint32_t length;                        /*!< The number of bytes to transfer in the simple mode and enhanced scatter-gather mode.
  *  Must be a multiple of 64. Ignored in Tx */
    bool continuous;                        /*!< NOTE: This field is meaningless for Rx DMA modules
  *  When set to 1, the DMA will restart the transfer.
  *  When cleared to 0, the DMA performs one transfer and stops. */
    uint32_t timeout_ms;                    /*!< NOTE: This field is meaningless for 'Inspect' functions
  *   The maximum time (in milliseconds) to wait for the capture to complete; Ignored in Tx */
    adi_fpga910x_DmaTrigger_e trigger;      /*!< NOTE: This field is meaningless for 'Inspect' functions
  *   The tollgate trigger source to use */
    adi_fpga910x_CaptureType_e captureType; /*!< NOTE: This field is meaningless for 'Inspect' functions
  *   The type of data transfer to be performed */
    bool dmaEnableSync;                     /*!< If true the first sample of the DMA will always be coincident with the rise of the channel TDDSELECT_DMA_Enable signal */
} adi_fpga910x_DmaCfg_t;

/**
 * \brief Data structure to get status of FPGA Data Chain DMA configuration
 */
typedef struct adi_fpga910x_DmaStatus
{
    uint8_t active;        /*!< When this bit is 1, the DMA is actively transferring data. Read-only */
    uint8_t complete;      /*!< When this bit is 1, the transfer has completed successfully.
  *  This bit will clear when run_stop is cleared to '0'. Read-only */
    uint8_t haltComplete;  /*!< When this bit is 1, the halt that was previously issued has completed.
  *  This module must be reset after a halt completes. Read-only */
    uint8_t overUnderflow; /*!< Overflow for Rx. Underflow for Tx. 1 means an over- or underflow has occurred on the FIFO.
  *  This is a sticky bit.  Write a 1 to this bit to clear it. Read-only */
} adi_fpga910x_DmaStatus_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_DATACHAIN_TYPES_H_ */
