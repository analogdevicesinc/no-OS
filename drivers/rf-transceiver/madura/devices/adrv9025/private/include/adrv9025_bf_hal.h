/* SPDX-License-Identifier: GPL-2.0 */
#ifndef ADRV9025_BF_HAL_H_
#define ADRV9025_BF_HAL_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include <stddef.h>

#include "adi_adrv9025.h"
#include "adi_common_hal.h"
#include "./../../private/include/adrv9025_bf_hal_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief allows the user to set the Write cache state.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param wrCacheState the cache state to be set.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfWriteCacheEn(adi_adrv9025_Device_t*      device,
                                adrv9025_Bf_WrCache_State_e wrCacheState);

/**
* \brief allows the user to set the read cache state.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param rdCacheState the cache state to be set.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfReadCacheEn(adi_adrv9025_Device_t*      device,
                               adrv9025_Bf_RdCache_State_e rdCacheState);

/**
* \brief ensures the cache is empty. if not it writes the cache to the part.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
*
* \retval ADRV9025_ Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfReadCacheInit(adi_adrv9025_Device_t* device);

/**
* \brief ensures the cache is empty. if not it writes the cache to the part.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfWriteCacheInit(adi_adrv9025_Device_t* device);

/**
* \brief Used to read a register.
* Merges the write if merging is on
* Caches the write if caching is on,
* Immediately writes the value if caching is off.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep{SPIARRAYSIZE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param addr The address of the Register to read.
* \param regVal The value to be written to the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfByteWrite(adi_adrv9025_Device_t* device,
                             uint16_t               addr,
                             uint8_t                regVal);

/**
* \brief Used to read a register.
* Caches the read if caching is on, immediately reads the value with caching off.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep{ADRV9025_BF_RDCACHE_ARRAY_TRIP_SIZE}
* \dep{SPIARRAYSIZE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param addr The address of the Register to read.
* \param regVal The value of the register just read.
* \param endOfBf when using read caching the upper nibble of this field indicates the end of bitfield with the Value F.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfByteRead(adi_adrv9025_Device_t* device,
                            uint16_t               addr,
                            uint8_t*               regVal,
                            uint8_t                endOfBf);

/**
* \brief Used to write to a sub part of a register.
* Caches the write if caching is on, immediately writes the value with caching off.
* Uses the HW_RMW function if the HW_RMW is enabled otherwise used a spi transaction to read the register
* and a spi write transaction to write the modified byte back to the register.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep{SPIARRAYSIZE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param addr The Register address that contains the field.
* \param fieldVal The value of the field to be written.
* \param mask The mask to use when inserting the data into the register.
* \param startBit The location of the first bit of the field within the register.
* when using read caching the upper nibble of this field indicates the end of bitfield with the Value F.
* This value is used to shift the value so that it aligns with the location of the field within the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfFieldWrite(adi_adrv9025_Device_t* device,
                              uint16_t               addr,
                              uint8_t                fieldVal,
                              uint8_t                mask,
                              uint8_t                startBit);

/**
* \brief Used to read a sub part of a register
* caches the read if caching is on, immediately read the value with caching off
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep{ADRV9025_BF_RDCACHE_ARRAY_TRIP_SIZE}
* \dep{SPIARRAYSIZE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param addr The Register address that contains the field.
* \param fieldVal The value of the field to be written.
* \param mask The mask to use when inserting the data into the register.
* \param startBit The location of the first bit of the field within the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfFieldRead(adi_adrv9025_Device_t* device,
                             uint16_t               addr,
                             uint8_t*               fieldVal,
                             uint8_t                mask,
                             uint8_t                startBit);

/**
* \brief Used by the bitField API to ensure the cache is flushed when using BitField Caching
* and to assemble the register bytes when caching is not used.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rxBfData The raw data read from the registers prior to concatenation.
* \param numRdBfData The number of elements used in the rdBfData.
* \param rdData The assembled bitfield value.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfReadAssembleData(adi_adrv9025_Device_t* device,
                                    uint8_t*               rxBfData,
                                    uint8_t                numRdBfData,
                                    uint64_t*              rdData);

/**
* \brief Used to send multiple spi register reads in one spi transaction.
*  multiple register data and sub register data is concatenated and shifted appropriately.
*
* warning ensure the rdData is big enough to contain the expected number of bitfield values
*
* \dep_begin
* \dep{SPIARRAYSIZE}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
* \param rdData The bytefield data being returned in the order it was read.
* \param numRxBitFields The number of bitfields being returned
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfReadCacheFlush(adi_adrv9025_Device_t* device,
                                  uint64_t*              rdData,
                                  uint16_t*              numRxBitFields);

/**
* \brief Used to send multiple spi register writes in one spi transaction.
*  if the spi buffer size is exceeded the data will be sent in buffer size blocks.
*  Only full bytes are written ie there are no RMW operations
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adrv9025_BfWriteCacheFlush(adi_adrv9025_Device_t* device);

#ifdef __cplusplus
}
#endif

#endif /* ADRV9025_BF_HAL_H_ */
