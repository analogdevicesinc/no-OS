/** 
 * \file fpgagen6_bf_axi_memory_pager.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _FPGAGEN6_BF_AXI_MEMORY_PAGER_H_
#define _FPGAGEN6_BF_AXI_MEMORY_PAGER_H_

#include "adi_fpgagen6_types.h"
#include "adi_fpgagen6_error.h"
#include "../../private/bf/fpgagen6_bf_axi_memory_pager_types.h"

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
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_BlockId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                       uint8_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint8_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_InstanceId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                          const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                          uint8_t* const bfValue);



/**
 * \brief The Page ID identifies the page in physical memory to which the accesses are directed. The Page ID must be zero padded in the least significant bits that correspond to the data being written to the page. For example, if the page is 64kB, Page ID bits 15:0 must be 0. The size of the Page ID is truncated based on the number of bits of the AXI Master Interface. This is set when the core is instantiated. It can be between 32 and 64 bits.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint64_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_PageId_BfSet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                      const uint64_t bfValue);


/**
 * \brief The Page ID identifies the page in physical memory to which the accesses are directed. The Page ID must be zero padded in the least significant bits that correspond to the data being written to the page. For example, if the page is 64kB, Page ID bits 15:0 must be 0. The size of the Page ID is truncated based on the number of bits of the AXI Master Interface. This is set when the core is instantiated. It can be between 32 and 64 bits.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint64_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_PageId_BfGet(adi_fpgagen6_Device_t* const device,
                                                                      const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                      uint64_t* const bfValue);



/**
 * \brief The page size sets the number of bits that are passed through the module. The value should be page size minus 1 and should be a power of 2. That is, all bits from least significant should be 1. This bit field will be used as a mask to pass the addressing within the page through to the AXI Master physical address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_PageSize_BfSet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                        const uint32_t bfValue);


/**
 * \brief The page size sets the number of bits that are passed through the module. The value should be page size minus 1 and should be a power of 2. That is, all bits from least significant should be 1. This bit field will be used as a mask to pass the addressing within the page through to the AXI Master physical address.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_PageSize_BfGet(adi_fpgagen6_Device_t* const device,
                                                                        const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                        uint32_t* const bfValue);



/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_Scratch_BfSet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                       const uint32_t bfValue);


/**
 * \brief No documentation provided
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_Scratch_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);



/**
 * \brief This is a local parameter in HDL and follows a MAJOR.MINOR.PATCH format, integers only.
 * 
 * 
 * \param device   Pointer to the FPGAGEN6 device data structure.
 * \param baseAddr Base Address of instance to be configured.
 *          Parameter is of type fpgagen6_BfAxiMemoryPagerChanAddr_e.
 * 
 * \param bfValue  Data to be configured. Parameter is of type uint32_t.
 * 
 * \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if successful, no action required
 */
ADI_API adi_fpgagen6_ErrAction_e fpgagen6_AxiMemoryPager_Version_BfGet(adi_fpgagen6_Device_t* const device,
                                                                       const fpgagen6_BfAxiMemoryPagerChanAddr_e baseAddr,
                                                                       uint32_t* const bfValue);
#endif // _FPGAGEN6_BF_AXI_MEMORY_PAGER_H_

