/** 
 * \file fpgagen6_bf_axi_txdma.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support FPGAGEN6 transceiver device.
 * 
 * FPGAGEN6 BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2023 Analog Devices Inc.
 * 
 * Released under the FPGAGEN6 API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _FPGAGEN6_BF_AXI_TXDMA_H_
#define _FPGAGEN6_BF_AXI_TXDMA_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_txdma_types.h"

#ifndef ADI_API
  #ifdef __cplusplus
    #define ADI_API extern "C"
  #else
    #define ADI_API
  #endif
#endif



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AddrdecError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      const uint8_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AddrdecError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief Allows DMA to align the captured data to the first valid data after the Start of Frame is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AlignToSof_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);


/**
 * \brief Allows DMA to align the captured data to the first valid data after the Start of Frame is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_AlignToSof_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief This allows arbitration of transactions among multiple DMA cores. 0x0: Default, always granted access to AXI bus. 0x1: Data-Threshold, posts transaction when a predefined number of samples are collected (fifo-not-empty). 0x2: Token-Ring, posts transaction when a predefined token is received, after that token is released. The master core always initiates the token. 0x3: Token-Ring + Data-Threshold, posts transaction when a predefined token is received and a predefined number of samples are collected (fifo-not-empty), after that token is released. The master core always initiates the token.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ArbitrationSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                           const uint8_t bfValue);


/**
 * \brief This allows arbitration of transactions among multiple DMA cores. 0x0: Default, always granted access to AXI bus. 0x1: Data-Threshold, posts transaction when a predefined number of samples are collected (fifo-not-empty). 0x2: Token-Ring, posts transaction when a predefined token is received, after that token is released. The master core always initiates the token. 0x3: Token-Ring + Data-Threshold, posts transaction when a predefined token is received and a predefined number of samples are collected (fifo-not-empty), after that token is released. The master core always initiates the token.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ArbitrationSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                           const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                           uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Busy_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                              uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Complete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief Allows DMA to be cycled through until terminated with the same settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Cyclic_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                const uint8_t bfValue);


/**
 * \brief Allows DMA to be cycled through until terminated with the same settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Cyclic_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                uint8_t* const bfValue);



/**
 * \brief When written to a 1, the data paths will activate as long as the DMAs are active. Important Note, the DMAs must be configured and activated prior to activating the data paths. The bit will automatically clear when all of the DMAs are no longer active. This bit can be written to 0 to force the data paths to deactivate.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DatapathEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        const uint8_t bfValue);


/**
 * \brief When written to a 1, the data paths will activate as long as the DMAs are active. Important Note, the DMAs must be configured and activated prior to activating the data paths. The bit will automatically clear when all of the DMAs are no longer active. This bit can be written to 0 to force the data paths to deactivate.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DatapathEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint8_t* const bfValue);



/**
 * \brief The incoming data stream can be masked based on one of 32 signals such as TDD enables.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataMaskSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        const uint32_t bfValue);


/**
 * \brief The incoming data stream can be masked based on one of 32 signals such as TDD enables.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataMaskSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint32_t* const bfValue);



/**
 * \brief Software may use this information to compute the granularity of an AXI transaction. This may be needed to compute burst size and length settings.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DataWidth_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint32_t* const bfValue);



/**
 * \brief When this bit is set to 1, it indicates that one or more DMAs are active.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_DmaStatus_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_HaltComplete_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t* const bfValue);



/**
 * \brief If 1 DMA includes the capture control module and if 0 Capture control is bypassed
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_IncludeCaptureCtrl_BfGet(adi_fpgagen6_Device_t* const device,
                                                                            const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                            uint8_t* const bfValue);



/**
 * \brief If this read only bit field is a 1, the Waveform Sequencer module is included in the design.  If it is 0, the WS has been omitted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_IncludeWs_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Length in bytes (granularity of DATA_WIDTH)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Length_BfSet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                const uint32_t bfValue);


/**
 * \brief Length in bytes (granularity of DATA_WIDTH)
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Length_BfGet(adi_fpgagen6_Device_t* const device,
                                                                const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_LengthError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_LengthError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



/**
 * \brief When written to a 1, the data path capture control passes the token to the next DMA in the arbitration token ring. For Transmit control, setting this bit to a 1 before the data path is activated, allows the DMAs to begin fetching data from memory and fill the FIFOs in front of the data path tollgates.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_PassToken_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief When written to a 1, the data path capture control passes the token to the next DMA in the arbitration token ring. For Transmit control, setting this bit to a 1 before the data path is activated, allows the DMAs to begin fetching data from memory and fill the FIFOs in front of the data path tollgates.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_PassToken_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief This bit may be set to the desired outstanding transactions in the data mover queue. This allows multiple transactions posted in the AXI bus. The number of outstanding transactions is value + 1 for up to 8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Qthreshold_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);


/**
 * \brief This bit may be set to the desired outstanding transactions in the data mover queue. This allows multiple transactions posted in the AXI bus. The number of outstanding transactions is value + 1 for up to 8.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Qthreshold_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_SlaveError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_SlaveError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Software may set this bit to initiate a DMA request. This bit is cleared when the DMA is completed or terminated (see STOP bit below).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Start_BfSet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                               const uint8_t bfValue);


/**
 * \brief Software may set this bit to initiate a DMA request. This bit is cleared when the DMA is completed or terminated (see STOP bit below).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Start_BfGet(adi_fpgagen6_Device_t* const device,
                                                               const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                               uint8_t* const bfValue);



/**
 * \brief Combined with START_ADDR_LOW to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrHigh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                       const uint32_t bfValue);


/**
 * \brief Combined with START_ADDR_LOW to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrHigh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief Combined with START_ADDR_HIGH to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrLow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      const uint32_t bfValue);


/**
 * \brief Combined with START_ADDR_HIGH to create the start address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_StartAddrLow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint32_t* const bfValue);



/**
 * \brief Software may set this bit to initiate a premature DMA termination. This bit is cleared when the request is completed. The stop bit is effective only if the START bit is set.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Stop_BfSet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                              const uint8_t bfValue);


/**
 * \brief Software may set this bit to initiate a premature DMA termination. This bit is cleared when the request is completed. The stop bit is effective only if the START bit is set.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Stop_BfGet(adi_fpgagen6_Device_t* const device,
                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                              uint8_t* const bfValue);



/**
 * \brief This is self-clearing and resets the core. There is no need to reset the core between multiple DMA requests. All DMA requests is gracefully completed and all internal elements are cleared (no stale data).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Swreset_BfSet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 const uint8_t bfValue);


/**
 * \brief This is self-clearing and resets the core. There is no need to reset the core between multiple DMA requests. All DMA requests is gracefully completed and all internal elements are cleared (no stale data).
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Swreset_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TlastError_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TlastError_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TransSize_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   const uint32_t bfValue);


/**
 * \brief Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TransSize_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint32_t* const bfValue);



/**
 * \brief Allows DMA to begin based on a hardware event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TriggerSelect_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                       const uint32_t bfValue);


/**
 * \brief Allows DMA to begin based on a hardware event.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_TriggerSelect_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Underflow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   const uint8_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Underflow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                   const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                   uint8_t* const bfValue);



/**
 * \brief This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ValidDataWidthStream_BfSet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                              const uint8_t bfValue);


/**
 * \brief This field indicates what the width of valid data is on the stream interface. This field is used to pack/unpack the data from the memory. The valid values are: 0: 32 bits; 1: 64 bits; 3: 128 bits; 7: 256 bits; F: 512 bits
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_ValidDataWidthStream_BfGet(adi_fpgagen6_Device_t* const device,
                                                                              const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                              uint8_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                 const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                 uint32_t* const bfValue);



/**
 * \brief Writing a 1 to the Waveform Sequencer Enable will use the start addresses, lengths, and iterations that are defined for the Waveform Sequencer in place of the default start address and length.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  const uint8_t bfValue);


/**
 * \brief Writing a 1 to the Waveform Sequencer Enable will use the start addresses, lengths, and iterations that are defined for the Waveform Sequencer in place of the default start address and length.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t* const bfValue);



/**
 * \brief Number of iterations to transmit the waveform before moving to the next entry within the Waveform Sequencer.  If a 0 is written, this entry will be skipped.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsIterations_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      const uint16_t bfValue);


/**
 * \brief Number of iterations to transmit the waveform before moving to the next entry within the Waveform Sequencer.  If a 0 is written, this entry will be skipped.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint16_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsIterations_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                      uint8_t channelId, 
                                                                      uint16_t* const bfValue);



/**
 * \brief Length in bytes (granularity of DATA_WIDTH) of waveform in Waveform Sequencer Entry
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsLength_BfSet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  const uint32_t bfValue);


/**
 * \brief Length in bytes (granularity of DATA_WIDTH) of waveform in Waveform Sequencer Entry
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsLength_BfGet(adi_fpgagen6_Device_t* const device,
                                                                  const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                  uint8_t channelId, 
                                                                  uint32_t* const bfValue);



/**
 * \brief Writing a 1 advances the Waveform Sequencer to the next waveform with a non-zero number of iterations.  This bit if self-clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmAdvance_BfSet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                     const uint8_t bfValue);


/**
 * \brief Writing a 1 advances the Waveform Sequencer to the next waveform with a non-zero number of iterations.  This bit if self-clearing.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmAdvance_BfGet(adi_fpgagen6_Device_t* const device,
                                                                     const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                     uint8_t* const bfValue);



/**
 * \brief Writing a 1 enables the Manual Mode.  In Manual Mode, the Waveform Sequencer dwells on any waveform definition with a non-zero number of iterations until the Manual Mode Advance bit field is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmEnable_BfSet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    const uint8_t bfValue);


/**
 * \brief Writing a 1 enables the Manual Mode.  In Manual Mode, the Waveform Sequencer dwells on any waveform definition with a non-zero number of iterations until the Manual Mode Advance bit field is asserted.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsMmEnable_BfGet(adi_fpgagen6_Device_t* const device,
                                                                    const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                    uint8_t* const bfValue);



/**
 * \brief Combined with WS_START_ADDR_LOW to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrHigh_BfSet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         const uint32_t bfValue);


/**
 * \brief Combined with WS_START_ADDR_LOW to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrHigh_BfGet(adi_fpgagen6_Device_t* const device,
                                                                         const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                         uint8_t channelId, 
                                                                         uint32_t* const bfValue);



/**
 * \brief Combined with WS_START_ADDR_HIGH to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrLow_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        const uint32_t bfValue);


/**
 * \brief Combined with WS_START_ADDR_HIGH to create the start address of the waveform in the Waveform Sequencer Entry.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiTxdmaChanAddr_e.
 * \param channelId Channel ID to be configured. Parameter is of type uint8_t
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiTxdma_WsStartAddrLow_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiTxdmaChanAddr_e baseAddr,
                                                                        uint8_t channelId, 
                                                                        uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_TXDMA_H_

