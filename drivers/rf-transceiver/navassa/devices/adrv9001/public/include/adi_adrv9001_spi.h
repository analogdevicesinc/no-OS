/**
 * \file
 * \brief Contains prototypes and macro definitions for Private ADI HAL wrapper
 *        functions implemented in adi_adrv9001_hal.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_SPI_H_
#define _ADI_ADRV9001_SPI_H_

#include "adi_adrv9001_spi_types.h"

#ifdef __KERNEL__
#include <linux/types.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: Any user changeable #defines need to be moved to adi_adrv9001_user.h */
#define HAL_TIMEOUT_DEFAULT 100         /* 100ms */
#define HAL_TIMEOUT_NONE 0x0            /* Non-blocking */
#define HAL_TIMEOUT_INFINITE 0xFFFFFFFF /* Blocking */
#define HAL_TIMEOUT_MULT 2              /* HAL timeout worse-case factor */

#define MAXSPILOGMESSAGE 64

#define SPIARRAYSIZE 1024
#define SPIARRAYTRIPSIZE ((SPIARRAYSIZE / 3) * 3)

#define ADRV9001_HW_RMW_LO_ADDR     0x113        /* Hardware Read Modify Write address */
#define ADRV9001_HW_RMW_HI_ADDR     0x114        /* Hardware Read Modify Write address */
#define ADRV9001_HW_RMW_MASK        0x115        /* Hardware Read Modify Write address */
#define ADRV9001_HW_RMW_DATA        0x116        /* Hardware Read Modify Write address */
#define ADRV9001_HW_RMW_BYTES       0xC         /* Number of bytes required to use HW_RMW */
#define ADRV9001_SPI_BYTES          0x3          /* Number of bytes required to use non HW_RMW */
#define ADRV9001_SPI_WRITE_POLARITY 0x00         /* Write bit polarity for ADRV9001 */


/**
* \brief creates an array acceptable to the ADIHAL layer.
* SPI writes require 3 bytes two address and one data in order to change
* all 8 bits in register. if only a few bits are to be changed then a
* read Modify Write (RMW) operation is needed.
* The ADRV9001 provides a write only RMW which reduces spi transactions.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[out]    wrData         The resulting array to be set to the HAL layer.
* \param[in,out] numWrBytes     The number of elements filled in the wrData array
* \param[in]     addr           The address to be added to wrData.
* \param[in]     mask           The mask to be added to wrData, this will be 0 if packing a full byte.
* \param[in]     data           The data to be added to wrData.
* \param[in]     writeFlag      The value to be bitwise or'd into the MSB of the 16-bit address
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_DataPack(adi_adrv9001_Device_t *adrv9001, uint8_t *wrData, uint16_t *numWrBytes, uint16_t addr, uint8_t mask, uint8_t data, uint8_t writeFlag);

/**
* \brief creates an array acceptable to the ADIHAL layer.
* SPI write in ADRV9001 stream mode requires 6 bytes, two address and four data in order to change
* all 8 bits in register. if only a few bits are to be changed then a
* read Modify Write (RMW) operation is needed.
* The ADRV9001 provides a write only RMW which reduces spi transactions.
*
* \param[in]     adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[out]    wrData         The resulting array to be set to the HAL layer.
* \param[in,out] numWrBytes     The number of elements filled in the wrData array
* \param[in]     addr           The address to be added to wrData.
* \param[in]     data           The data to be added to wrData.
* \param[in]     count          The number of register addresses to write data to.
* \param[in]     writeFlag      The value to be bitwise or'd into the MSB of the 16-bit address
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Stream_DataPack(adi_adrv9001_Device_t *adrv9001, uint8_t *wrData, uint32_t *numWrBytes, uint16_t addr, const uint8_t data[], uint32_t count, uint8_t writeFlag);

/**
* \brief writes a byte of data to the part.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[in] addr           The address of the register to write to.
* \param[in] data           The value to write to the register.
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Byte_Write(adi_adrv9001_Device_t *adrv9001, uint16_t addr, uint8_t data);

/**
* \brief writes a byte of data to the part in ADRV9001 stream mode.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[in] addr           The address of the register to write to.
* \param[in] data           The value to write to the register.
* \param[in] count          The number of register addresses to write data to.
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Bytes_Stream_Write(adi_adrv9001_Device_t *adrv9001, uint16_t addr, const uint8_t data[], uint32_t count);

/**
* \brief writes an array of bytes of data to the part.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[in] addr           The addresses of the registers to write to.
* \param[in] data           The values to write to the registers.
* \param[in] count          The number of register addresses to write data to.
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Bytes_Write(adi_adrv9001_Device_t *adrv9001, const uint16_t addr[], const uint8_t data[], uint32_t count);

/**
* \brief reads a byte of data from the part.
*
* \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
* \param[in]  addr          The address of the register to read from
* \param[out] readData      The value read from the register
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Byte_Read(adi_adrv9001_Device_t *adrv9001, uint16_t addr, uint8_t *readData);

/**
* \brief reads an array of bytes of data from the part.
*
* \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
* \param[in]  addr          The addresses of the registers to read from
* \param[out] readData      The values to read from the registers
* \param[in]  count         The number of register addresses to read data from
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Bytes_Read(adi_adrv9001_Device_t *adrv9001, const uint16_t addr[], uint8_t readData[], uint32_t count);

/**
* \brief writes a field of data to the part.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[in] addr           The address of the register to write to
* \param[in] fieldVal       The value to write to the register
* \param[in] mask           The mask to use when writing the value to the register
* \param[in] startBit       The bit number where the field starts within the register
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Field_Write(adi_adrv9001_Device_t *adrv9001, uint16_t addr, uint8_t  fieldVal, uint8_t mask, uint8_t startBit);

/**
* \brief reads a field of data from the part.
*
* \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
* \param[in]  addr          The address of the register to read from
* \param[out] fieldVal      The value read from the register
* \param[in]  mask          The mask to use when decoding the value to the register
* \param[in]  startBit      The bit number where the field starts within the register
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Field_Read(adi_adrv9001_Device_t *adrv9001, uint16_t addr, uint8_t *fieldVal, uint8_t mask, uint8_t startBit);
    
    int32_t adi_adrv9001_spi_Mask_Write(adi_adrv9001_Device_t *adrv9001, uint16_t addr, uint8_t data, uint8_t mask);

/**
* \brief writes an array of bytes of data to the part.
*
* \param[in] adrv9001       Context variable - Pointer to the ADRV9001 device data structure
* \param[in] wrCache        The values to write to the registers
* \param[in] count          The number of register addresses to write data to
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Cache_Write(adi_adrv9001_Device_t *adrv9001, const uint32_t wrCache[], uint32_t count);

/**
* \brief reads an array of bytes of data from the part.
*
* \param[in]  adrv9001      Context variable - Pointer to the ADRV9001 device data structure
* \param[in]  rdCache       The addresses of the registers to read from
* \param[out] readData      The location to put the values to read from the registers
* \param[in]  count         The number of register addresses to read data from
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_spi_Cache_Read(adi_adrv9001_Device_t *adrv9001, const uint32_t rdCache[], uint8_t readData[], uint32_t count);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_SPI_H_ */
