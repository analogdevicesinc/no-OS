/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_rx.h
 * \brief Contains prototypes and macro definitions for ADI RX wrapper
 *        functions implemented in adi_fpgagen6_rx.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_RX_H_
#define _ADI_FPGAGEN6_RX_H_

#include "adi_fpgagen6_rx_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief    Writes a 1 to the Capture Control modules Datapath Active bitfield. A capture will then occur
*           based on the Tollgate trigger selections.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxCaptureStart(adi_fpgagen6_Device_t* const device);

/**
* \brief    Called after adi_fpgagen6_RxCaptureStart to block program execution until the capture completes
*           or the timeMsDelay expires. The function waits for the Active bitfield in the Capture Control
*           module to clear.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] timeMsDelay is uint32_t type which conveys the wait time in milliseconds, i.e. 200 = 200 ms
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxCaptureWait(adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  timeMsDelay);

/**
* \brief    Called in preparation for an Rx capture. Resets all elements in the Datachain related to a data capture
*           in preparation for configuration then capture.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDataChainReset(adi_fpgagen6_Device_t* const device);

/**
* \brief    The DMAs on the ADS10 are responsible for getting data in and out of memory. 
*           There are 32 total interfaces into the HBM and half of which are used for Receive DMAs. 
*           
*           The Receive DMAs take a stream of data from the JESD section and write them 
*           into memory beginning at the defined starting address. 
*           Each Rx DMA is attached to a different 
*           interface to the HBM memory. Each interface has a specific range of preferred
*           memory that it accesses with optimized performance. 
*           However, each DMA/interface is capable of accessing all memory space but there is a 
*           cost in performance if it goes outside that preferred range.
*           The first Receive DMA (RX DMA0) has the capture control circuitry. This is special logic to 
*           initiate a capture on all Receive DMAs that are enabled. It ensures that 
*           the capture is aligned in time across all of the Receive DMAs and provides a 
*           single bitfield to query to determine if the capture 
*           is complete. The DMA also contains the tollgate functionality but the Tollgate API
*           functions will be separate from the data management responsibility of the DMA.
*           
*           This API function writes a DMA config struct to the selected DMA. dmaSel
*           is used to index into the DMAs; only a single DMA can be configured using this function
*           because they each take in their own starting address.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        rxDmaSel the integer value of the DMA to configure, eg pass in value of 1 to select rxdma_1
* \param[in]        rxDmaCfg the DMA configuration to write to the selected DMA
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDmaCfgSet(  adi_fpgagen6_Device_t* const            device,
                                                            const uint8_t                           rxDmaSel,
                                                            const adi_fpgagen6_RxDmaCfg_t* const    rxDmaCfg);

/**
* \brief    Reads the configuration for a single DMA module and populates the empty
*           adi_fpgagen6_RxDmaCfg_t structure passed in to the function. This function reads
*           Read-Write & Read-Only bitfields, including status bits.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        rxDmaSel the integer value of the DMA to configure, eg pass in value of 1 to select rxdma_1
* \param[in,out]       rxDmaCfg Pointer to return the adi_fpgagen6_RxDmaCfg_t data structure.
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDmaCfgGet(  adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  rxDmaSel,
                                                            adi_fpgagen6_RxDmaCfg_t* const  rxDmaCfg);

/**
* \brief    Prepares the selected DMAs for an upcoming capture.
* 
*           The rxDmaSelMask parameter selects which DMAs to configure using the array of RxDmaCaptureCfg
*           structures in rxDmaCaptureCfgs. The first element in rxDmaCaptureCfgs will be written to
*           the DMA selected by the first bit set in rxDmaSelMask, eg if rxDmaSelMask is 0x00000002, then
*           rxDmaCaptureCfgs[0] will be applied to Rx DMA 1. The next element in rxDmaCaptureCfgs will be written
*           to the DMA selected by the second bit in rxDmaSelMask, etc. ie the index in RxDmaCaptureCfg does
*           not correspond to the DMA index in the FPGA image. Build up the RxDmaCaptureCfg without any empty
*           entries.
*           
*           After this function call the DMAs selected in the mask are ready for the next
*           capture.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        rxDmaSelMask bitmask where each bit represents a particular DMA to initialize for
*                       an upcoming capture, eg 0x0101 would initialize DMAs 8 & 0. Valid
*                       DMAs are from DMA_0 through DMA_10. The number of bits set here should match numDmasToConfigure
* \param[in]        rxDmaCaptureCfgs An array of Rx DMA Capture settings. Index 0 will be applied to the first bit set in rxDmaSelMask, etc.
* \param[in]        numDmasToConfigure the size of the rxDmaCaptureCfgs structure & the number of bits set in rxDmaSelMask
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxDataChainConfigure( adi_fpgagen6_Device_t* const            device,
                                                                    const uint32_t                          rxDmaSelMask,
                                                                    const adi_fpgagen6_RxDmaCaptureCfg_t    rxDmaCaptureCfgs[],
                                                                    const uint32_t                          numDmasToConfigure);

/**
* \brief    This API function will configure a select transport layer with the supplied JESD parameters in the
*           rxTransportCfg argument.
*           
*           There are a total of four Rx Transport modules instantiated in the ADS10 ADRV904X FPGA image. These four
*           transport modules are always receiving link data and transporting into converter data. 
*           
*           Depending on the Transport instance, the outgoing data streams will either have two or four converter 
*           samples interleaved.
*           
*           In order to route the desired data to the correct location in RAM, it is necessary to correctly configure 
*           the appropriate DMAs. Each DMA can select as its source any output of any one of the four transport modules.
*           
*           Please refer to the ADS10 FPGA User Guide for more detailed description and diagrams.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        rxTransportSel the integer value of the Transport module to configure, eg pass in value of 0 to select Transport 0, etc
* \param[in]        rxTransportCfg the JESD configuration to write to the selected Transport module
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxTransportCfgSet(adi_fpgagen6_Device_t* const                    device,
                                                                const uint8_t                                   rxTransportSel,
                                                                const adi_fpgagen6_RxTransportCfg_t* const      rxTransportCfg);

/**
* \brief    This API function reads the current settings in the selected Transport module and returns them in the supplied
*           rxTransportCfg structure.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device Context variable - Pointer to the FPGAgen6 device data structure.
* \param[in]        rxTransportSel the integer value of the Transport module to configure, eg pass in value of 0 to select Transport 0, etc
* \param[in,out]       rxTransportCfg the JESD configuration to write to the selected Transport module
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RxTransportCfgGet(adi_fpgagen6_Device_t* const            device,
                                                                const uint32_t                          rxTransportSel,
                                                                adi_fpgagen6_RxTransportCfg_t* const    rxTransportCfg);

#endif