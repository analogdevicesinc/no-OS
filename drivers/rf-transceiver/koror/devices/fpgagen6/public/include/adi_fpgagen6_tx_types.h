/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_tx_types.h
 * \brief Contains FPGA tx enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_TX_TYPES_H_
#define _ADI_FPGAGEN6_TX_TYPES_H_

#include "adi_fpgagen6_data_interface_types.h"

#define ADI_FPGAGEN6_TX_DMA_COUNT                       (12U) //TODO: FPGA temporarily reduced number of DMAs from 16 to 10. Change back when DMAs are updated
#define ADI_FPGAGEN6_TX_DMA_RAM_START_ADDR              (0x100000000U) 
#define ADI_FPGAGEN6_TX_DMA_MEM_BLOCK_SIZE              (0x010000000U) /* 256MB allotted to each DMA. Used when calculating start address */
#define ADI_FPGAGEN6_TX_DMA_HALT_TIMEOUT_MS             200U
#define ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA             8U      /*!< FPGA Number of Waveform Sequencer per TX DMA */

/**
* \brief Enumerated list of fpga Tx Tollgata trigger sources
*/
typedef enum adi_fpgagen6_TxTollgateTrigSources
{
    ADI_FPGAGEN6_TX_IMM_TRIG                = 0x01U,   /*!< Immediate trigger. */
    ADI_FPGAGEN6_TX_TDD_SM                  = 0x02U,   /*!< TDD State Machine. */
    ADI_FPGAGEN6_TX_EXT_TRIG                = 0x04U,   /*!< External Trigger external SMA. */
    ADI_FPGAGEN6_TX_GPIO_TRIG               = 0x08U,   /*!< GPIO. */
    ADI_FPGAGEN6_TX_START_OF_WAVEFORM_TRIG  = 0x10U    /*!< Transmit STart of Waveform Trigger. */
} adi_fpgagen6_TxTollgateTrigSources_e;

/**
*  \brief Data structure to hold the Tx capture DMA settings
*/
typedef struct adi_fpgagen6_TxDmaTransmitCfg
{
    uint32_t                length; /*!< Identifies the number of bytes to transfer in the next transaction */
    uint8_t                 continuous; /*! Runtime: When set to 1, the DMA will restart the transfer.  When cleared to 0, the DMA performs one transfer and stops.*/
    uint32_t                triggerSelect; /*!< MA Trigger Select. Allows DMA to begin based on a hardware event. Only valid for the triggering DMA. */
    uint64_t                wsAddress; /*!< Identifies the offset address in wave sequencer */
    uint16_t                wsIterations; /*!< Identifies the number of iterations in wave sequencer */
    uint8_t                 wsIndex; /*!< Identifies the wave sequence index */
    uint8_t                 wsEnable; /*!< Identifies the wave sequence enable */
} adi_fpgagen6_TxDmaTransmitCfg_t;

/**
*  \brief Data structure to hold the Tx specific DMA fields
*/
typedef struct adi_fpgagen6_TxDmaCfg
{
    adi_fpgagen6_DmaCfg_t           dma;                    /*! Common DMA configuration */ 
    uint8_t                         overflow;               /*! Read only: When this bit is 1, an overflow has occurred on the FIFO.  This is a sticky bit.  Write a 1 to this bit to clear it. */
    uint8_t                         validDataWidthStream;   /*!< Valid Data Width Stream. This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits*/
    adi_fpgagen6_TxDmaTransmitCfg_t  txDmaCaptureCfg;       /*! For readback only, ie these are not written during TxCfgSet: holds the last capture settings applied to this DMA */
     
    /* Below are wave sequence read data */
    uint64_t                wsAddress[ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA]; /*!< Identifies the offset address in wave sequencer */
    uint16_t                wsIterations[ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA]; /*!< Identifies the number of iterations in wave sequencer */
    uint32_t                wsLength[ADI_FPGAGEN6_MAX_WAVE_SEQ_PER_TXDMA]; /*!< Identifies the length in wave sequencer */
} adi_fpgagen6_TxDmaCfg_t;

#endif
