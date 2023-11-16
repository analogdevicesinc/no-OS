/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_hal.h
 * \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
 *        functions implemented in adi_fpgagen6_hal.c
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_HAL_H_
#define _ADI_FPGAGEN6_HAL_H_

#include "adi_fpgagen6_hal_types.h"
#include "adi_fpgagen6_error.h"

/**
 *  /brief Convenience macro for FPGAGEN6 Device API functions to call adi_common_hal_ApiEnter
 */
#define ADI_FPGAGEN6_API_ENTRY(commonDev)                                                                   \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiEnter((commonDev), __func__, ADI_TRUE);      \
                                                                                                            \
    if(ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                          \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Enter Issue");                          \
        return (adi_fpgagen6_ErrAction_e)_recoveryAction;   /* Device Specific Recovery Action Required */  \
    }                                                                                                       \
}

/**
 *  /brief Convenience macro for FPGAGEN6 Device API functions to call adi_common_hal_ApiExit
 */
#define ADI_FPGAGEN6_API_EXIT(commonDev, recoveryAction)                                                    \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiExit((commonDev), __func__, ADI_TRUE);       \
                                                                                                            \
    if (ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                         \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Exit Issue");                           \
    }                                                                                                       \
                                                                                                            \
    if (ADI_FPGAGEN6_ERR_ACT_NONE != recoveryAction)                                                        \
    {                                                                                                       \
        return recoveryAction;                                                                              \
    }                                                                                                       \
                                                                                                            \
    return (adi_fpgagen6_ErrAction_e)_recoveryAction; /* Device Specific Recovery Action Required */        \
}

/**
* \brief Performs a read of a single 32-bit register
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] addr Address of the register to be read. Must be on a word boundary
* \param[out] data Pointer where the register value will be assigned
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterRead( adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  addr,
                                                            uint32_t* const                 data);

/**
* \brief Performs a write of a single 32-bit register
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the FPGA device data structure.
* \param[in] addr Address of the register to be written. Must be on a word boundary
* \param[in] data The data to write
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterWrite(adi_fpgagen6_Device_t* const    device,
                                                            const uint32_t                  addr,
                                                            const uint32_t                  data);

/**
* \brief    Reads from FPGA RAM starting from ramAddress length number of four-byte words.
*           data parameter should contain space for length number of data words.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the fpga device data structure.
* \param[in] ramAddress the 64bits address of the register to start reading
* \param[out] data      pointer to a location to write the register data to. Must be large enough for length * sizeof(uint32_t)
* \param[in] length     number of 4-byte words to read into data
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RamRead(  adi_fpgagen6_Device_t* const    device,
                                                        const uint64_t                  ramAddress,
                                                        uint32_t                        data[],
                                                        const uint32_t                  length);

/**
* \brief    Writes to FPGA RAM starting from ramAddress length number of four-byte words.
*           data parameter should contain space for length number of data words.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the fpga device data structure.
* \param[in] ramAddress the 64bits address of the register to start reading
* \param[in] data       pointer to a location that holds the write data. Must contain length number of entries.
* \param[in] length     number of 4-byte words to write.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RamWrite( adi_fpgagen6_Device_t* const    device,
                                                        const uint64_t                  ramAddress,
                                                        const uint32_t                  data[],
                                                        const uint32_t                  length);

/**
* \brief Service for FPGAGEN6 Bitfield Read
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device      Context variable -Pointer to the FPGA device data structure.
* \param[in]        addr        Address of the register where the field resides
* \param[out]       fieldVal    A pointer to store the value in the field
* \param[in]        mask        A bit-wise mask that determines which bits are part of the field
* \param[in]        startBit    The bit location in the addr where the field starts
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterFieldRead(adi_fpgagen6_Device_t* const    device,
                                                                uint32_t                        addr,
                                                                uint32_t* const                 fieldVal,
                                                                uint32_t                        mask,
                                                                uint8_t                         startBit);

/**
* \brief Service for FPGAGEN6 Bitfield Write
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out]    device      Context variable -Pointer to the FPGA device data structure.
* \param[in]        addr        Address of the register where the field resides
* \param[in]        fieldVal    Field Value to be Written
* \param[in]        mask        A bit-wise mask that determines which bits are part of the field
* \param[in]        startBit    The bit location in the addr where the field starts
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegisterFieldWrite(   adi_fpgagen6_Device_t* const    device,
                                                                    uint32_t                        addr,
                                                                    const uint32_t                  fieldVal,
                                                                    uint32_t                        mask,
                                                                    uint8_t                         startBit);


/**
* \brief    Writes to FPGA RAM starting from ramAddress length number of four-byte words.
*           DEPRECATED: this function does not update paging interface, please use RamWrite.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the fpga device data structure.
* \param[in] ramAddress the 64bits address of the register to start reading
* \param[in] data       pointer to a location that holds the write data. Must contain length number of entries.
* \param[in] length     number of 4-byte words to write.
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegistersWrite(adi_fpgagen6_Device_t* const    device,
                                                            const uint64_t                  ramAddress,
                                                            const uint32_t                  data[],
                                                            const uint32_t                  length);

/**
* \brief    Reads from FPGA RAM starting from ramAddress length number of four-byte words.
*           DEPRECATED: this function does not update paging interface, please use RamRead.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the fpga device data structure.
* \param[in] ramAddress the 64bits address of the register to start reading
* \param[out] data      pointer to a location to write the register data to. Must be large enough for length * sizeof(uint32_t)
* \param[in] length     number of 4-byte words to read into data
* 
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_RegistersRead( adi_fpgagen6_Device_t* const    device,
                                                            const uint64_t                  ramAddress,
                                                            uint32_t                        data[],
                                                            const uint32_t                  length);

#endif