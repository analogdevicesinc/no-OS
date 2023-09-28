/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_analog_rx_mem_map_types.h Automatically generated file with generator ver 0.0.1.0.
 *
 * \brief Contains BitField functions to support ADRV9025 transceiver device.
 *
 * ADRV9025 BITFIELD VERSION: 0.0.0.1
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADRV9025_BF_ANALOG_RX_MEM_MAP_TYPES_H__
#define __ADRV9025_BF_ANALOG_RX_MEM_MAP_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfAnalogRxMemMapChanAddr
{
    ADRV9025_BF_ALL_ANALOG_RX = 0xC00,
    ADRV9025_BF_ANALOG_RX_CH1 = 0x2C00,
    ADRV9025_BF_ANALOG_RX_CH2 = 0x3400,
    ADRV9025_BF_ANALOG_RX_CH3 = 0x3C00,
    ADRV9025_BF_ANALOG_RX_CH0 = 0x4400
} adrv9025_BfAnalogRxMemMapChanAddr_e;

#ifdef __cplusplus
}
#endif

#endif /* __ADRV9025_BF_ANALOG_RX_MEM_MAP_TYPES_H__ */

/*  EOF: adrv9025_bf_analog_rx_mem_map_types.h */
