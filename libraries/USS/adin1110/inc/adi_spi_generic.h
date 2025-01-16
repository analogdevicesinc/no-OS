/*
 *---------------------------------------------------------------------------
 *
 * Copyright (c) 2020, 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc.
 * and its licensors.By using this software you agree to the terms of the
 * associated Analog Devices Software License Agreement.
 *
 *---------------------------------------------------------------------------
 */

#ifndef ADI_SPI_GENERIC_H
#define ADI_SPI_GENERIC_H

#include "adi_mac.h"
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! SPI generic protocol selection. */
#define ADI_MAC_SPI_PROT_GENERIC            (1)
/*! SPI duplex selection. */
#define ADI_MAC_SPI_FULL_DUPLEX             (1)
#define ADI_MAC_SPI_HALF_DUPLEX             (0)

/*! SPI header size, in bytes. */
#define ADI_SPI_HEADER_SIZE                 (2)
/*! Frame header size, in bytes. */
#define ADI_FRAME_HEADER_SIZE               (2)

/*! Number of buffer bytes in TxFIFO to provide a margin for the frame writes. */
#define ADI_SPI_TX_FIFO_BUFFER              (4)

/*!
 * @brief SPI header.
 */
typedef struct
{
    union {
        struct {
             uint16_t   ADDR      : 13; /*!< SPI Register Address.          */
             uint16_t   RW        : 1;  /*!< 0 => Read, 1 => Write          */
             uint16_t   FD        : 1;  /*!< Enable Full Duplex.            */
             uint16_t   CD        : 1;  /*!< 0 => Data, 1 => Control        */
        };
        uint16_t VALUE16;
    };
} adi_mac_SpiHeader_t;

#ifdef __cplusplus
}
#endif

#endif /* ADI_SPI_GENERIC_H */

