/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_tx.h
 * \brief Contains prototypes and macro definitions for ADI TX wrapper
 *        functions implemented in adi_fpgagen6_tx.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_TX_H_
#define _ADI_FPGAGEN6_TX_H_

#include "adi_fpgagen6_tx_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief    Initiates a data transmission. Should be called after adi_fpgaxxx_TxDataChainConfigure.
*           Tx data will start after the trigger has occurred.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataStart(adi_fpgagen6_Device_t* const device);

/**
* \brief    Halts a current data transmission immediately.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataStop(adi_fpgagen6_Device_t* const device);

/**
* \brief    Called in preparation for an data transmission. Resets all elements in the Datachain 
*           related to a data transmission.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataChainReset(adi_fpgagen6_Device_t* const device);

/**
* \brief    The DMAs on the ADS10 are responsible for getting data in and out of memory. 
*           There are 32 total interfaces into the HBM and half of which are used for Transmit DMAs. 
*           
*           The Transmit DMAs take a stream of data starting at their defined starting address
*           from memory and direct that stream to their assigned JESD Link Layer. 
*           Each Tx DMA is attached to a different 
*           interface to the HBM memory. Each interface has a specific range of preferred
*           memory that it accesses with optimized performance. 
*           However, each DMA/interface is capable of accessing all memory space but there is a 
*           cost in performance if it goes outside that preferred range.
*           The first Transmit DMA 
*           (TX DMA0) has the capture control circuitry. This is special logic to 
*           initiate a transmit on all Transmit DMAs that are enabled. It ensures that 
*           the transmit is aligned in time across all of the Transmit DMAs and provides a 
*           single bitfield to query to determine if the transmit is complete. The DMA also
*           contains the tollgate functionality but the Tollgate API
*           functions will be separate from the data management responsibility of the DMA.
*           
*           This function writes a DMA config struct to the selected DMA. txDmaSel
*           is used to index into the DMAs; only a single DMA can be configured using this function
*           because they each take in their own starting address.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        txDmaSel the integer value of the DMA to configure, eg pass in value of 1 to select rxdma_1
* \param[in]        txDmaCfg the DMA configuration to write to the selected DMA
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_TxDmaCfgSet( adi_fpgagen6_Device_t* const            device,
                                                            const uint8_t                           txDmaSel,
                                                            const adi_fpgagen6_TxDmaCfg_t* const    txDmaCfg);

/**
* \brief    Reads the configuration for a single DMA module and populates the empty
*           adi_fpgagen6_TxDmaCfg_t structure passed in to the function. This function reads
*           Read-Write & Read-Only bitfields, including status bits.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        txDmaSel the integer value of the DMA to configure, eg pass in value of 1 to select txdma_1
* \param[in,out]       txDmaCfg Pointer to return the adi_fpgagen6_TxDmaCfg_t data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e  adi_fpgagen6_TxDmaCfgGet( adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  txDmaSel,
                                                            adi_fpgagen6_TxDmaCfg_t* const  txDmaCfg);

/**
* \brief    Prepares the selected DMAs for an upcoming transmit.
* 
*           The txDmaSelMask parameter selects which DMAs to configure using the array of TxDmaCaptureCfg
*           structures in txDmaCaptureCfgs. The first element in txDmaCaptureCfgs will be written to
*           the DMA selected by the first bit set in txDmaSelMask, eg if txDmaSelMask is 0x00000002, then
*           txDmaCaptureCfgs[0] will be applied to Tx DMA 1.
*           
*           After this function call the DMAs selected in the mask are ready for the next
*           transmit.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        txDmaSelMask bitmask where each bit represents a particular DMA to initialize for
*                       an upcoming transmit, eg 0x0101 would initialize DMAs 8 & 0. Valid
*                       DMAs are from DMA_0 through DMA_10. The number of bits set here should match numDmasToConfigure
* \param[in]        txDmaCaptureCfgs An array of Tx DMA Capture settings. Index 0 will be applied to the first bit set in txDmaSelMask, etc.
* \param[in]        numDmasToConfigure the size of the txDmaCaptureCfgs structure & the number of bits set in txDmaSelMask
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataChainConfigure( adi_fpgagen6_Device_t* const            device,
                                                                    const uint32_t                          txDmaSelMask,
                                                                    const adi_fpgagen6_TxDmaTransmitCfg_t    txDmaCaptureCfgs[],
                                                                    const uint32_t                          numDmasToConfigure);

/**
* \brief    Enable waveform sequencer manual mode. Should be called before adi_fpgaxxx_TxDataChainConfigure.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in]        txDmaSelMask bitmask where each bit represents a particular DMA to initialize for
*                       an upcoming transmit, eg 0x0101 would initialize DMAs 8 & 0. Valid
*                       DMAs are from DMA_0 through DMA_10.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataWsMmEnable(adi_fpgagen6_Device_t* const device,
                                                               const uint32_t         txDmaSelMask);

/**
* \brief    Initiates a data transmission waveform sequencer manual mode advance. Should be called after adi_fpgaxxx_TxDataChainConfigure.
*           Tx data will start after the trigger has occurred.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in]        txDmaSelMask bitmask where each bit represents a particular DMA to initialize for
*                       an upcoming transmit, eg 0x0101 would initialize DMAs 8 & 0. Valid
*                       DMAs are from DMA_0 through DMA_10.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_TxDataWsMmAdvance(adi_fpgagen6_Device_t* const device,
                                                                const uint32_t         txDmaSelMask);

#endif