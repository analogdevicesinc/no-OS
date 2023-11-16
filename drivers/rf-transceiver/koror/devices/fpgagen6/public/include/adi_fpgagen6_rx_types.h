/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_rx_types.h
 * \brief Contains FPGA rx enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_RX_TYPES_H_
#define _ADI_FPGAGEN6_RX_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"

#define ADI_FPGAGEN6_RX_DMA_COUNT                           (16U) 

#define ADI_FPGAGEN6_RX_SAMPLE_SELECT_COUNT_PER_TRANSPORT   (16U)
#define ADI_FPGAGEN6_RX_DMA_RAM_START_ADDR                  (0x000000000U) 
#define ADI_FPGAGEN6_RX_DMA_MEM_BLOCK_SIZE                  (0x010000000U) /* 256MB allotted to each DMA. Used when calculating start address */
#define ADI_FPGAGEN6_RX_DMA_HALT_TIMEOUT_MS                 200U

#define ADI_FPGAGEN6_RX_TRANSPORT_COUNT                     (4U)

/**
* \brief Enumerated list of fpga Rx Tollgate trigger sources
*/
typedef enum adi_fpgagen6_RxTollgateTrigSources
{
    ADI_FPGAGEN6_RX_IMM_TRIG    = 0x1, /*!< Immediate trigger. */
    ADI_FPGAGEN6_RX_TDD_SM      = 0x2, /*!< TDD State Machine. */
    ADI_FPGAGEN6_RX_EXT_TRIG    = 0x4,  /*!< External Trigger external SMA. */
    ADI_FPGAGEN6_RX_ARM_ACK     = 0x8,  /*!< GPIO Trigger */
    ADI_FPGAGEN6_RX_TX1_SOF     = 0x10, /*!< TX1 Start of File Flag from transmit to indicate start of transmit tone.*/
} adi_fpgagen6_RxTollgateTrigSources_e;

/**
*  \brief Data structure to hold the Rx specific DMA fields
*/
typedef struct adi_fpgagen6_RxTransportCfg
{
    uint8_t cfgF;           /*! The number of octets per frame. Value to write is F-1. */
    uint8_t cfgNp;          /*! The resolution of the sample. Value to write is Np-1. */
    uint8_t cfgM;           /*! The number of converters. The value to write is M-1. */
    uint8_t cfgL;           /*! The number of lanes in use. Value to write is L-1. */
    uint8_t cfgLinkSharing; /*! When set to 1, this transport layer is identified as the link sharing configuration. */
} adi_fpgagen6_RxTransportCfg_t;

/**
*  \brief Data structure to hold the Rx capture DMA settings
*/
typedef struct adi_fpgagen6_RxDmaCaptureCfg
{
    uint32_t                                length;                 /*!< Identifies the number of bytes to transfer in the next transaction */
    adi_fpgagen6_RxTollgateTrigSources_e    triggerSelect;          /*!< MA Trigger Select. Allows DMA to begin based on a hardware event. Only valid for the triggering DMA. */
    uint8_t                                 validDataWidthStream;   /*!< Valid Data Width Stream. This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits*/
} adi_fpgagen6_RxDmaCaptureCfg_t;

/**
*  \brief Data structure to hold the Rx specific DMA fields
*/
typedef struct adi_fpgagen6_RxDmaCfg
{
    adi_fpgagen6_DmaCfg_t           dma;                /*! Common DMA configuration */
    uint8_t                         overflow;           /*! Read Only: When this bit is 1, an overflow has occurred on the FIFO.  This is a sticky bit.  Write a 1 to this bit to clear it. */
    uint8_t                         sampleSelect;       /*! The selection of the output of the transport layer to send to each DMA. The selection is identified by the converter in the transportation. See FPGA User Guide for valid values. */
    uint8_t                         inUse;              /*! Indicates if this DMA has been configured for capture in the current profile */
    adi_fpgagen6_RxDmaCaptureCfg_t  rxDmaCaptureCfg;    /*! For readback only, ie these are not written during RxCfgSet: holds the last capture settings applied to this DMA */
} adi_fpgagen6_RxDmaCfg_t;


#endif
