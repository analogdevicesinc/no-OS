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

#ifndef ADI_SPI_OA_H
#define ADI_SPI_OA_H

#include "adi_mac.h"
#include "hal.h"
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! SPI header size, in bytes. */
#define ADI_SPI_HEADER_SIZE                 (4)
/*! Frame header size, in bytes. */
#define ADI_FRAME_HEADER_SIZE               (2)

/*! Maximum number of chunks that will be packed into a single SPI transaction */
#define ADI_OA_MAX_CHUNK_COUNT              (31)
#define ADI_OA_MAX_CHUNK64_COUNT            (16)

/*! When a header parity error is detected, the MAC replies with this pattern. */
#define ADI_OA_HEADER_BAD                   (0x40000000)

typedef struct
{
    union {
        struct {
            uint32_t P      : 1;
            uint32_t LEN    : 7;
            uint32_t ADDR   : 16;
            uint32_t MMS    : 4;
            uint32_t AID    : 1;
            uint32_t WNR    : 1;
            uint32_t HDRB   : 1;
            uint32_t DNC    : 1;
        };
        uint32_t VALUE32;
    };
} adi_mac_OaCtrlCmdHeader_t;


typedef struct
{
    union {
        struct {
            uint32_t P      : 1;
            uint32_t RSVD0  : 5;
            uint32_t TMSC   : 2;
            uint32_t EBO    : 6;
            uint32_t EV     : 1;
            uint32_t RSVD1  : 1;
            uint32_t SWO    : 4;
            uint32_t SV     : 1;
            uint32_t DV     : 1;
            uint32_t VS     : 2;
            uint32_t RSVD2  : 5;
            uint32_t NORX   : 1;
            uint32_t SEQ    : 1;
            uint32_t DNC    : 1;
        };
        uint32_t VALUE32;
    };
} adi_mac_OaTxHeader_t;

typedef struct
{
    union {
        struct {
            uint32_t P      : 1;
            uint32_t TXC    : 5;
            uint32_t RTSP   : 1;
            uint32_t RTSA   : 1;
            uint32_t EBO    : 6;
            uint32_t EV     : 1;
            uint32_t FD     : 1;
            uint32_t SWO    : 4;
            uint32_t SV     : 1;
            uint32_t DV     : 1;
            uint32_t VS     : 2;
            uint32_t RCA    : 5;
            uint32_t SYNC   : 1;
            uint32_t HDRB   : 1;
            uint32_t EXST   : 1;
        };
        uint32_t VALUE32;
    };
} adi_mac_OaRxFooter_t;

#ifdef __cplusplus
}
#endif

#endif /* ADI_SPI_OA_H */

