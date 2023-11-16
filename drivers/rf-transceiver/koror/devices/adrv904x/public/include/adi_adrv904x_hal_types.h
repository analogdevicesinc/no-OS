/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_adrv904x_hal_types.h
 * \brief Contains prototypes and macro definitions for ADI HAL wrapper
 *        functions implemented in adi_adrv904x_hal.c
 *
 * ADRV904X API Version: 2.9.0.4
 */

#ifndef _ADI_ADRV904X_HAL_TYPES_H_
#define _ADI_ADRV904X_HAL_TYPES_H_

#include "adi_platform.h"

/* TODO: Any user changeable #defines need to be moved to adi_adrv904x_user.h */
#define HAL_TIMEOUT_DEFAULT 100         /* 100ms */
#define HAL_TIMEOUT_NONE 0x0            /* Non-blocking */
#define HAL_TIMEOUT_INFINITE 0xFFFFFFFF /* Blocking */
#define HAL_TIMEOUT_MULT 2              /* HAL timeout worse-case factor */

#define MAXSPILOGMESSAGE 64

#define ADRV904X_DIRECT_SPI_BYTES       0x3      /* Number of bytes required to use non HW_RMW direct */
#define ADRV904X_PAGING_SPI_BYTES       0x1B     /* Number of bytes required to use non HW_RMW indirect: 9 Direct SPI writes (assumed entire word written, ie mask = 0xFFFFFFFF) */
#define ADRV904X_SPI_WRITE_POLARITY     0x00     /* Write bit polarity for ADRV904X */
    
#define ADI_ADRV904X_SPI_READ_BYTE      0
#define ADI_ADRV904X_SPI_READ_HALFWORD  1
#define ADI_ADRV904X_SPI_READ_WORD      2


/**
 *  \brief Structure used to track the SPI cache
 */
typedef struct adi_adrv904x_SpiCache
{
    uint8_t data[ADI_HAL_SPI_FIFO_SIZE];                /*!< SPI data buffer. Should not be written to manually. Data gets updated through calls to adi_adrv904x_Register(s)Write */
    uint32_t count;                                     /*!< The number of bytes filled with data buffer. Should not be written to manually. Gets updated through calls to adi_adrv904x_Register(s)Write */
    uint32_t lastSpiAddr;                               /*!< Address that last byte, at (count - 1) index, in buffer is written to. Should not be written to manually. Gets updated through calls to adi_adrv904x_Register(s)Write */
    uint8_t spiStreamingEn;                             /*!< Flag used to signal SPI streaming is enabled (1) or disabled (0). Can be changed anytime to switch between streaming & non-streaming. */
} adi_adrv904x_SpiCache_t;

/**
 *  \brief Structure to define a register map with start/end addresses
 *
 * Describes a memory region consisting of N separate areas.  Each area has an identical size and the starting address
 * of each area is constant number of bytes (strideValue) from the starting address of the preceeding area.
 * 
 * Visually:
 *
 * +----------------------------------------------------------------------------------------------------------------------------+
 * | start addr + | ... | end addr + | ~ | start addr   | ... | end addr    | ~ | start addr +       | ... | end addr +         |
 * | stride * 0   | ... | stride * 0 | ~ | + stride * 1 | ... | +stride * 1 | ~ | stride *           | ... | stride *           |
 * |              |     |            |   |              |     |             |   | (numInstances - 1) | ... | (numInstances - 1) |
 * +----------------------------------------------------------------------------------------------------------------------------+
 * |< ------------- strideValue -------->|
 */
typedef struct adi_adrv904x_RegisterMap
{
    uint32_t startAddr;       /* 32 bit Starting address of the register map */
    uint32_t endAddr;         /* 32 bit Ending address of the register map */
    uint32_t numOfInstances;  /* Num of instances for this register map */
    uint32_t strideValue;     /* In bytes. This is the offset between start addresses of consecutive instances, it's a 'don't care' when numOfInstances = 1*/
} adi_adrv904x_RegisterMap_t;

#endif
