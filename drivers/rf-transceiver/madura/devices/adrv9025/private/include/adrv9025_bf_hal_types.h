/* SPDX-License-Identifier: GPL-2.0 */
/**
* \brief Contains BitField definitions to support ADRV9025 transciever device.
*
*/

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#ifndef __ADRV9025_BF_HAL_TYPES_H__
#define __ADRV9025_BF_HAL_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief Enum of all possible states of SPI write Caching.
*/
typedef enum adrv9025_Bf_WrCache_State
{
    ADRV9025_WR_CACHE_OFF       = 0x0,  /*!< disable caching. spi writes will occur immediately and HW-RMW will not be used*/
    ADRV9025_WR_HW_RMW_ON       = 0x1,  /*!< use the Hardware Read-Modify-write if it is enabled.*/
    ADRV9025_WR_BF_CACHE_ON     = 0x2,  /*!< use caching only within a bitfield function. cache is auto flushed at the end of the function.*/
    ADRV9025_WR_GBL_CACHE_ON    = 0x4,  /*!< use caching for multiple bitfield functions. cache is auto flushed when cache is full.*/
    ADRV9025_WR_GBL_MERGE_ON    = 0x8   /*!< use merging to combine multiple bitfield field functions. cache is auto flushed when cache is full.*/
} adrv9025_Bf_WrCache_State_e;

/**
* \brief Enum of all possible states of SPI Read Caching .
*/
typedef enum adrv9025_Bf_RdCache_State
{
    ADRV9025_RD_CACHE_OFF       = 0x0,  /*!< disable caching. spi reads will occurr immediately*/
    ADRV9025_RD_BF_CACHE_ON     = 0x1,  /*!< use caching only within a bitfield function. cache is auto flushed at the end of the function.*/
    ADRV9025_RD_GBL_CACHE_ON    = 0x2   /*!< use caching for multiple bitfield functions. the user must managing flushing*/
} adrv9025_Bf_RdCache_State_e;

#ifdef __cplusplus
}
#endif

#endif // __ADRV9025_BF_HAL_TYPES_H__
