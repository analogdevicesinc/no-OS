/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_hal.h
 * \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
 *        functions implemented in adi_adrv904x_hal.c
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_HAL_H_
#define _ADI_ADRV904X_HAL_H_

#include "adi_adrv904x_hal_types.h"
#include "adi_adrv904x_error.h"

/**
 *  /brief Convenience macro for ADRV904X Device API functions to call adi_common_hal_ApiEnter
 */
#define ADI_ADRV904X_API_ENTRY(commonDev)                                                                   \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiEnter((commonDev), __func__, ADI_TRUE);      \
                                                                                                            \
    if(ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                          \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(commonDev, _recoveryAction, "API Enter Issue");                                \
        return (adi_adrv904x_ErrAction_e)_recoveryAction;   /* Device Specific Recovery Action Required */  \
    }                                                                                                       \
}

/**
 *  /brief Convenience macro for ADRV904X Device API functions to call adi_common_hal_ApiExit
 */
#define ADI_ADRV904X_API_EXIT(commonDev, recoveryAction)                                                    \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction = adi_common_hal_ApiExit((commonDev), __func__, ADI_TRUE);       \
                                                                                                            \
    if (ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                         \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(commonDev, _recoveryAction, "API Exit Issue");                                 \
    }                                                                                                       \
                                                                                                            \
    if(ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)                                                         \
    {                                                                                                       \
        return (adi_adrv904x_ErrAction_e) recoveryAction;   /* Device Specific Recovery Action Required */  \
    }                                                                                                       \
                                                                                                            \
    return (adi_adrv904x_ErrAction_e)_recoveryAction;       /* Device Specific Recovery Action Required */  \
}

/**
 *  /brief Convenience macro for ADRV904X Device API functions to call adi_common_hal_ApiEnter_vLogCtl
 *   with ADI_COMMON_DEVICE_LOGCTL_QUIET (i.e. uses logLevel PRIV for recursive calls to the API).
 */
#define ADI_ADRV904X_API_ENTRY_QUIET(commonDev)                                                             \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction =                                                                \
        adi_common_hal_ApiEnter_vLogCtl((commonDev), __func__, ADI_TRUE, ADI_COMMON_DEVICE_LOGCTL_QUIET);   \
                                                                                                            \
    if(ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                          \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Enter Issue");                          \
        return (adi_adrv904x_ErrAction_e)_recoveryAction;   /* Device Specific Recovery Action Required */  \
    }                                                                                                       \
}

/**
 *  /brief Convenience macro for ADRV904X Device API functions to call adi_common_hal_ApiExit_vLogCtl
 *   with ADI_COMMON_DEVICE_LOGCTL_QUIET (i.e. uses logLevel PRIV for recursive calls to the API).
 */
#define ADI_ADRV904X_API_EXIT_QUIET(commonDev, recoveryAction)                                              \
{                                                                                                           \
    adi_common_ErrAction_e _recoveryAction =                                                                \
        adi_common_hal_ApiExit_vLogCtl((commonDev), __func__, ADI_TRUE, ADI_COMMON_DEVICE_LOGCTL_QUIET);    \
                                                                                                            \
    if (ADI_COMMON_ERR_ACT_NONE != _recoveryAction)                                                         \
    {                                                                                                       \
        ADI_API_ERROR_REPORT(&device->common, _recoveryAction, "API Exit Issue");                           \
    }                                                                                                       \
                                                                                                            \
    if(ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)                                                         \
    {                                                                                                       \
        return (adi_adrv904x_ErrAction_e) recoveryAction;   /* Device Specific Recovery Action Required */  \
    }                                                                                                       \
                                                                                                            \
    return (adi_adrv904x_ErrAction_e)_recoveryAction;       /* Device Specific Recovery Action Required */  \
}

/**
* \brief Writes out the data array of size count. Must be packed according ADI HAL layer specs.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[in] data byte array containing the data to write to SPI. Must be packed correctly.
* \param[in,out] count pointer to the number of valid bytes in data parameter. On successful
*   completion it is set to 0.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_SpiFlush(adi_adrv904x_Device_t* const    device,
                                                       const uint8_t                   data[],
                                                       uint32_t* const                 count);

/**
* \brief Writes data from memory to destination registers on the device.
* 
* Supports a contiguous writes with a single start address or scattered write to several addresses.
* 
* See adi_adrv904x_Registers32Read for important details.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device   Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache If supplied writes operations are cached until adi_adrv904x_SpiFlush
*                         is used to flush the cache or a subsequent call to this function decides to flush
*                         the cache. Set to NULL to avoid caching.
* \param[in] addr         For a contiguous write supply an array of one element containing the dest addr at which
*                         to start the write; The Nth element in writeData is written to addr[0] + n.
*                         For scattered writes supply an array of param count elements; the Nth element in writeData is written
*                         to addr[n].  All register addresses are 32bit regardless of the data-width of the register.
* \param[in] writeData    The data to write. Each element is 32bits wide but depending on the destination
*                         register address either all 32bits (for addresses >= 0x4000) or only the least-significant
*                         8 bits will be written (for addresses 0x0 - 0x3FFF).
* \param[in] mask         NULL indicates a contiguous write (see param addr) and no masking occurs - all bits of each dest register
*                         will be written to. 
*                         If not NULL each register write for each element in writeData is masked with the
*                         corresponding element of this array. Only the register bits set to 1 in the
*                         corresponding mask element are updated. Other register bits are left unchanged.
* \param[in] count        Number of elements writeData array and if mask is not NULL also mask and addr arrays.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32Write(adi_adrv904x_Device_t* const    device,
                                                               adi_adrv904x_SpiCache_t* const  spiCache,
                                                               const uint32_t                  addr[],
                                                               const uint32_t                  writeData[],
                                                               const uint32_t                  mask[],
                                                               const uint32_t                  count);

/**
* \brief Writes a 32bits register with 32bits of data to the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes. If there is no caching passing NULL is valid.
* \param[in] addr       the 32bits address of the register to write to.
* \param[in] writeData  the 32bits value to write to the register.
* \param[in] mask       the 32bits mask to write to the register.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Register32Write(adi_adrv904x_Device_t* const    device,
                                                              adi_adrv904x_SpiCache_t* const  spiCache,
                                                              uint32_t                        addr,
                                                              const uint32_t                  writeData,
                                                              const uint32_t                  mask);

/**
* \brief This function serves as a helper function for adi_adrv904x_Registers32Read. This will call Registers32Read with a count value
*        set to one. Depending on if addr is Direct SPI (< 0x4000) or AHB access ( all other addresses) that one will correspond to a single
*        byte or a full word (4 bytes), respectively.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes, the cache will be written out prior to performing the read. If there is no caching passing NULL is valid.
* \param[in] addr       the 32bits address of the register to read from.
* \param[out] readData   a pointer to a location to write the register data to.
* \param[in] mask       the desired mask to logically-and the register value with before returning in readData
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Register32Read(adi_adrv904x_Device_t* const    device,
                                                             adi_adrv904x_SpiCache_t* const  spiCache,
                                                             uint32_t                        addr,
                                                             uint32_t* const                 readData,
                                                             const uint32_t                  mask);


/**
* \brief Read from a contiguous range of device registers into memory.
* 
*        This function is used to access all SPI registers, both Core SPI addresses (addresses < 0x4000) and AHB memory (addresses >= 0x4000). 
*        For Core SPI addresses it uses direct SPI access. For AHB addresses, it uses the AHB interface registers. 
*        The only practical difference between the two for the user is the size of the registers. For Core SPI access the register size is 8-bits.
*        For AHB access, the register size is 32-bits. This means for Core SPI addresses, the upper three bytes of each readData element are
*        not written.
*        
*        Both Core & AHB register access can take advantage of SPI streaming. There is a local cache that will build up as big of a cache of SPI
*        reads as possible before accessing the SPI platform layer. The spiCache parameter is only as a convenience and to keep the prototype across
*        API HAL functions similar, ie if a user has an existing spiCache from previous Register(s)32Write they can pass it in here to have it be flushed
*        prior to reads being performed. However, SPI reads cannot be cached across Register(s)32Read function calls.
*        
*       In addition to SPI streaming, there is also AHB auto-increment mode and SPI FIFO mode available for AHB address regions.
*       AHB auto-increment allows reading the AHB interface data registers continuously without having to set the AHB address registers each time.
*       SPI FIFO allows for SPI streaming and AHB auto-increment to be used together- it allows SPI streaming mode when reading from the same
*       register over and over, eg AHB Data interface register.
*       The user can set the preference for AHB auto-increment and SPI FIFO mode through adi_adrv904x_AhbReadConfigure. SPI streaming is set when
*       the SPI is configured.
*       Note: SPI FIFO can only be utilized if auto-increment and SPI streaming is enabled.
*       
*       The mask parameter allows for mask values to be applied to each register while reading. However, if masking is not necessary a NULL value
*       can be used and no masking will be performed. It is recommended for large transactions that mask be NULL unless absolutely necessary.
*       Keep in mind that for Direct SPI access, only the least-significant byte will have the mask applied. This means that if there's garbage passed
*       into the readData array, there will still be garbage in bytes[3:1] of each readData entry. It is recommended to zero out the memory space
*       allocated for readData prior to calling this function if that's a concern.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes, the cache will be written out prior to performing the read.
*                           If there is no caching passing NULL is valid.
* \param[in] addr           The 32bit address of the first source register on the device from which to read.
* \param[in,out] readData   The destination location to which to write the register data. Must of size at least 'count * sizeof(uint32_t)'. Each register
*                           is written to a separate element in readData regardless of the size of the device register.
* \param[in] mask           If no masking is desired pass in NULL. Otherwise, it should be the same size as count with mask values for each register.
* \param[in] count          The number of registers to read. Does NOT indicate number of bytes to read. Registers 0 - 0x3FFF are 8 bit, other registers are 32bit.
* 
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32Read(adi_adrv904x_Device_t* const    device,
                                                              adi_adrv904x_SpiCache_t* const  spiCache,
                                                              const uint32_t                  addr,
                                                              uint32_t                        readData[],
                                                              uint32_t                        mask[],
                                                              const uint32_t                  count);

/**
* \brief Writes data from memory to destination registers on the device using byte arrays.
* 
* See adi_adrv904x_Registers32Read for important details.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device   Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache Optional pointer to a spiCache structure if caching writes. If there is no
*                         caching passing NULL is valid.
* \param[in] addr         The destination address on the device.
* \param[in] writeData    The data to write.
* \param[in] count        The number of bytes to write to the device.
*
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RegistersByteWrite(adi_adrv904x_Device_t* const    device,
                                                                 adi_adrv904x_SpiCache_t* const  spiCache,
                                                                 const uint32_t                  addr,
                                                                 const uint8_t                   writeData[],
                                                                 const uint32_t                  count);

/**
* \brief Read from a contiguous range of device registers into memory.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes, the cache will be written out prior to performing the read.
*                           If there is no caching passing NULL is valid.
* \param[in] addr           The 32bit source address of the first source register on the device from which to read.
* \param[in, out] readData  The destination to which to write the data.
* \param[in] mask           After it is read the Nth element of readData is OR'd with Nth element of mask. Pass NULL for no masking.
* \param[in] count          The number of bytes to read.
* 
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RegistersByteRead(adi_adrv904x_Device_t* const    device,
                                                                adi_adrv904x_SpiCache_t* const  spiCache,
                                                                const uint32_t                  addr,
                                                                uint8_t                         readData[],
                                                                const uint8_t                   mask[],
                                                                const uint32_t                  count);

/**
* \brief Read from a contiguous range of device registers into memory.
* 
*       This function can only access registers that require a 32-bit read/writes. Furthermore both addr and count
*       arguments must be divisible by 4. If you don't know whether a memory region is 32bOnly or not use
*       Registers32Read() which will invoke this function if required.
*       
*       Calling flushes the spiCache before transaction to make sure the intended order of SPI transactions isn't
*       changed.
*       
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes, the cache will be written
*                           out prior to performing the read. If there is no caching passing NULL is valid.
* \param[in] addr           The 32bit source address on the device. Must be divisible by 4.
* \param[in,out] readData   The destination buffer. Each four bytes read from the device is treated as a single 32bit
*                           integer and device to host endianess conversion is applied if required.
* \param[in] count          The number of *bytes* to read. Must be divisible by 4. All source addresses
*                           from addr to addr + count - 1 must be in a 32bOnly region.
* 
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32bOnlyRead(adi_adrv904x_Device_t* const    device,
                                                                   adi_adrv904x_SpiCache_t* const  spiCache,
                                                                   const uint32_t                  addr,
                                                                   uint8_t                         readData[],
                                                                   const uint32_t                  count);

/**
* \brief Writes to a contiguous range of device registers into memory.
* 
*       This function is used to access only registers that *require* a 32-bit SPI transaction. This function
*       flushes the spiCache before transaction to make sure the intended order of SPI transactions isn't changed.
*       
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   Optional pointer to a spiCache structure if caching writes, the cache will be written out prior to performing the read.
*                           If there is no caching passing NULL is valid.
* \param[in] addr           The 32bit address of the first source register on the device to which to write.
* \param[in] writeData      The data to write. Each four elements is treated as a single 32bit integer and host to
*                           device endianess conversion is applied if required.
* \param[in] count          The number of bytes to write. Must be divisible by 4.
* 
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_Registers32bOnlyWrite(adi_adrv904x_Device_t* const    device,
                                                                    adi_adrv904x_SpiCache_t* const  spiCache,
                                                                    const uint32_t                  addr,
                                                                    const uint8_t                   writeData[],
                                                                    const uint32_t                  count);

/**
* \brief Identical to adrv904x_Registers32bOnlyRead but allows the caller to specify if the read data should be treated
*     as raw byte oriented data or as 32bit integers. This influences endianess fix-ups performed.
*
*     As indicated by the lack of 'adi_' prefix this function may be called only from other API functions and not from
*     applications. It's interface may change without notice.
*       
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param[in,out] device     Context variable - Pointer to the ADRV904X device data structure.
* \param[in,out] spiCache   As per adrv904x_Registers32bOnlyRead.
* \param[in] addr           As per adrv904x_Registers32bOnlyRead.
* \param[in,out] readData   As per adrv904x_Registers32bOnlyRead.
* \param[in] numBytesToRead As per adrv904x_Registers32bOnlyRead.
* \param[in] isByteData     If set the data read is treated as 8bit integers otherwise is treated as 32bit integers.
*                           32bit integer data is corrected to account for any difference in host and device endianess as it is read.
*                           8bit data requires no correction and is placed in memory in the same order as it is in the device's memory.
* 
* \retval adi_adrv904x_ErrAction_e, ADI_ADRV904X_ERR_ACT_NONE if successful
*
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_Registers32bOnlyRead_vEndian(adi_adrv904x_Device_t* const    device,
                                                                       adi_adrv904x_SpiCache_t* const  spiCache,
                                                                       const uint32_t                  addr,
                                                                       uint8_t                         readData[],
                                                                       const uint32_t                  numBytesToRead,
                                                                       const uint8_t                   isByteData);

#endif
