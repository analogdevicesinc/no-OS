/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adi_adrv9025_hal.h
 * \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
 *        functions implemented in adi_adrv9025_hal.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef _ADI_ADRV9025_HAL_H_
#define _ADI_ADRV9025_HAL_H_

#include "adi_adrv9025_hal_types.h"
#include "adi_platform.h"
#include "adi_adrv9025.h"

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Any user changeable #defines need to be moved to adi_adrv9025_user.h */
#define HAL_TIMEOUT_DEFAULT 100         /* 100ms */
#define HAL_TIMEOUT_NONE 0x0            /* Non-blocking */
#define HAL_TIMEOUT_INFINITE 0xFFFFFFFF /* Blocking */
#define HAL_TIMEOUT_MULT 2              /* HAL timeout worse-case factor */

#define MAXSPILOGMESSAGE 64

/* TODO: add #defines */
#define ADRV9025_HW_RMW_LO_ADDR     0xFF        /* Hardware Read Modify Write address */
#define ADRV9025_HW_RMW_HI_ADDR     0x100       /* Hardware Read Modify Write address */
#define ADRV9025_HW_RMW_MASK        0x101       /* Hardware Read Modify Write address */
#define ADRV9025_HW_RMW_DATA        0x102       /* Hardware Read Modify Write address */
#define ADRV9025_HW_RMW_BYTES       0xC         /* Number of bytes required to use HW_RMW */
#define ADRV9025_SPI_BYTES          0x3         /* Number of bytes required to use non HW_RMW */
#define ADRV9025_SPI_WRITE_POLARITY 0x00        /* Write bit polarity for ADRV9025 */

/**
* \brief creates an array acceptable to the ADIHAL layer.
* SPI writes require 3 bytes two address and one data in order to change
* all 8 bits in register. if only a few bits are to be changed then a
* read Modify Write (RMW) operation is needed.
* The ADRV9025 provides a write only RMW which reduces spi transactions.
*
* \dep_begin
* \dep{device->common.cacheInfo}
* \dep{SPIARRAYTRIPSIZE}
* \dep{ADRV9025_HW_RMW_LO_ADDR}
* \dep{ADRV9025_HW_RMW_HI_ADDR}
* \dep{ADRV9025_HW_RMW_MASK}
* \dep{ADRV9025_HW_RMW_DATA}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param wrData the resulting array to be set to the HAL layer.
* \param numWrBytes The number of elements filled in the wrData array
* in a zero indexed array this points to the next empty location.
* \param addr The address to be added to wrData.
* \param mask The mask to be added to wrData, this will be 0 if packing a full byte.
* \param data The data to be added to wrData.
* \param writeFlag The value to be bitwise or'd into the MSB of the 16-bit address
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiDataPack(adi_adrv9025_Device_t* device,
                                 uint8_t*               wrData,
                                 uint16_t*              numWrBytes,
                                 uint16_t               addr,
                                 uint8_t                mask,
                                 uint8_t                data,
                                 uint8_t                writeFlag);

/**
* \brief writes a byte of data to the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr the address of the register to write to.
* \param data the value to write to the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiByteWrite(adi_adrv9025_Device_t* device,
                                  uint16_t               addr,
                                  uint8_t                data);

/**
* \brief Writes a full word to DMA registers .
* This function only supports B0 silicon
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param data A full word to write.
* \param startAddress Address of most significant byte in full word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiDmaWordWrite(adi_adrv9025_Device_t* device,
                                     const uint32_t         data,
                                     uint16_t               startAddress);

/**
* \brief Writes a data array with Spi streaming by using custom hal layer function (adi_hal_CustomSpiStreamWrite).
* To call this function, adi_hal_CustomSpiStreamWrite function pointer should point to custom spi stream write hal
* function which will transfer the "count" number of data bytes with A A D D D D packages. DMA Auto increment
* should be enabled to use this function.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param data Data array to write.
* \param count The number of bytes within data array. This must be multiple of 4.
* \param address Lowest address value for target DMA word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiWriteStreamWithCustomHalFunction(adi_adrv9025_Device_t* device,
                                                         const uint8_t          data[],
                                                         uint32_t               count,
                                                         uint16_t               address);

/**
* \brief Reads data array with Spi streaming by using custom hal layer function (adi_hal_CustomSpiStreamRead).
* To call this function, adi_hal_CustomSpiStreamRead function pointer should point to custom spi stream read hal
* function which will read the "count" number of data bytes with A A D D D D packages. DMA Auto increment
* should also be enabled to use this function.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param readData Data array to read.
* \param count The number of bytes within data array. This must be multiple of 4.
* \param address Lowest address value for target DMA word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiReadStreamWithCustomHalFunction(adi_adrv9025_Device_t* device,
                                                        uint8_t                readData[],
                                                        uint32_t               count,
                                                        uint16_t               address);

/**
* \brief Writes a data array with Spi streaming.
* This function allows reduction of API address overhead for writing consecutive DMA registers
* Streaming should have been enabled before calling this function. It only supports single/multiple full
* DMA word transfers. It holds spi CS low, transfer the data package and then pulls CS high to end SPI stream.
* Data package is transferred as => A A D D D D | A A D D D D | .... | A A D D D D |
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param data Data array to write.
* \param count The number of bytes within data array. This must be multiple of 4.
* \param address Lowest address value for target DMA word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiDmaWordWriteStream(adi_adrv9025_Device_t* device,
                                           const uint8_t          data[],
                                           uint32_t               count,
                                           uint16_t               address);

/**
* \brief Read a data array with Spi streaming .
* This function allows reduction of API address overhead for reading consecutive DMA registers
* Streaming should have been enabled before calling this function. It only supports single/multiple full
* DMA word transfers. It holds spi CS low, reads the data package and then pulls CS high to end SPI stream.
* Data package is transferred as => A A D D D D | A A D D D D | .... | A A D D D D |
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param readData Read back data is returned in this array.
* \param count The number of bytes to read. This must be multiple of 4.
* \param address Lowest address value for target DMA word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiDmaWordReadStream(adi_adrv9025_Device_t* device,
                                          uint8_t                readData[],
                                          uint32_t               count,
                                          uint16_t               address);

/**
* \brief Reads a full word from DMA registers .
* This function only supports B0 silicon
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param data Pointer to memory location storing the word value read from memory.
* \param startAddress Address of most significant byte in full word
*
* \retval ADI_COMMON_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiDmaWordRead(adi_adrv9025_Device_t* device,
                                    uint32_t*              data,
                                    uint16_t               startAddress);

/**
* \brief writes an array of bytes of data to the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr[] the addresses of the registers to write to.
* \param data[] the values to write to the registers.
* \param count the number of register addresses to write data to.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiBytesWrite(adi_adrv9025_Device_t* device,
                                   const uint16_t         addr[],
                                   const uint8_t          data[],
                                   uint32_t               count);

/**
* \brief reads a byte of data from the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr the address of the register to read from.
* \param readData a pointer to a location to write the register data to.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiByteRead(adi_adrv9025_Device_t* device,
                                 uint16_t               addr,
                                 uint8_t*               readData);

/**
* \brief reads an array of bytes of data from the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr[] the addresses of the registers to read from.
* \param readData[] the location to put the values to read from the registers.
* \param count the number of register addresses to read data from.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiBytesRead(adi_adrv9025_Device_t* device,
                                  const uint16_t         addr[],
                                  uint8_t                readData[],
                                  uint32_t               count);

/**
* \brief writes a field of data to the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr the address of the register to write to.
* \param fieldVal the value to write to the register.
* \param mask the mask to use when writing the value to the register.
* \param startBit the bit number where the field starts within the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiFieldWrite(adi_adrv9025_Device_t* device,
                                   uint16_t               addr,
                                   uint8_t                fieldVal,
                                   uint8_t                mask,
                                   uint8_t                startBit);

/**
* \brief reads a field of data from the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param addr the address of the register to read from.
* \param fieldVal pointer to a location to put the value read from the register.
* \param mask the mask to use when decoding the value to the register.
* \param startBit the bit number where the field starts within the register.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiFieldRead(adi_adrv9025_Device_t* device,
                                  uint16_t               addr,
                                  uint8_t*               fieldVal,
                                  uint8_t                mask,
                                  uint8_t                startBit);

/**
* \brief writes an array of bytes of data to the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param wrCache[] the values to write to the registers.
* \param count the number of register addresses to write data to.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiCacheWrite(adi_adrv9025_Device_t* device,
                                   const uint32_t         wrCache[],
                                   uint32_t               count);

/**
* \brief reads an array of bytes of data from the part.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9025 device data structure.
* \param rdCache[] the addresses of the registers to read from.
* \param readData[] the location to put the values to read from the registers.
* \param count the number of register addresses to read data from.
*
* \retval ADRV9025_ACT_WARN_RESET_LOG Recovery action for log reset
* \retval ADRV9025_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADRV9025_ACT_ERR_RESET_SPI Recovery action for SPI reset required
* \retval ADRV9025_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_SpiCacheRead(adi_adrv9025_Device_t* device,
                                  const uint32_t         rdCache[],
                                  uint8_t                readData[],
                                  uint32_t               count);

/**
* \brief verifies that hardware resources are available and have valid pointers allocated.
*
* \dep_begin
* \dep{device->halInfo}
* \dep_end
*
* \param device Pointer to the ADRV9010 device data structure.
*
* \retval ADI_COMMON_ACT_ERR_RESET_INTERFACE Recovery action for SPI reset required
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*
*/
int32_t adi_adrv9025_HwVerify(adi_adrv9025_Device_t *device);


#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9025_HAL_H_ */
