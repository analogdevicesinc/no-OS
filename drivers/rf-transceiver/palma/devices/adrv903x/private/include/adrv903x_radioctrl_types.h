/**
 * Copyright 2015 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_radioctrl_types.h
 * \brief Contains ADRV903X RADIOCTRL related private data prototypes for
 *        adrv903x_radioctrl.c
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef _ADRV903X_RADIOCTRL_TYPES_H_
#define _ADRV903X_RADIOCTRL_TYPES_H_

typedef enum adrv903x_StreamGpioFeatureSelection
{
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT0    = 0U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT1    = 1U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT2    = 2U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT3    = 3U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT4    = 4U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT5    = 5U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT6    = 6U,
    ADRV903X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT7    = 7U,
    ADRV903X_STREAM_GPIO_TX_ANTENNA_CAL            = 8U,
    ADRV903X_STREAM_GPIO_RX_ANTENNA_CAL            = 9U,
    ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO0_UNLOCK = 10U,
    ADRV903X_STREAM_GPIO_TX_PAP_FOR_EXT_LO1_UNLOCK = 11U,
    ADRV903X_STREAM_GPIO_0_TRX_TRIGGER_CHANNEL              = 130,
    ADRV903X_STREAM_GPIO_1_TRX_TRIGGER_CHANNEL              = 131,
    ADRV903X_STREAM_GPIO_2_TRX_TRIGGER_CHANNEL              = 132,
    ADRV903X_STREAM_GPIO_3_TRX_TRIGGER_CHANNEL              = 133,
    ADRV903X_STREAM_GPIO_UNUSED                             = 255U
} adrv903x_StreamGpioFeatureSelection_e;

typedef enum adrv903x_StreamAnaGpioFeatureSelection
{
    ADRV903X_STREAM_ANA_GPIO_UNUSED                              = 255U
} adrv903x_StreamAnaGpioFeatureSelection_e;
#endif /* _ADRV903X_RADIOCTRL_TYPES_H_ */
