/**
* \file
* \brief Contains ARM private features related function implementation defined in
*        adrv9001_crc32.h
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001_error.h"
#include "adrv9001_crc32.h"

static const uint32_t LUT_CRC32[16] = {
    0x00000000u, 0x04c11db7u, 0x09823b6eu, 0x0d4326d9u,
    0x130476dcu, 0x17c56b6bu, 0x1a864db2u, 0x1e475005u,
    0x2608edb8u, 0x22c9f00fu, 0x2f8ad6d6u, 0x2b4bcb61u,
    0x350c9b64u, 0x31cd86d3u, 0x3c8ea00au, 0x384fbdbdu
};

/*
 * \brief       Run Cyclic Redundancy Check on the specified block of memory in chunk.
 *
 * This function was based on the Crc32 function but can be used on chunk of memory block.
 * The function can be call multiple times.
 * The first call, set seedCrc to 0, finalCrc to 0.
 * The return CRC is use as seedCrc for the next call.
 * The last call, set finalCrc to 1.
 *
 * \details     CRC32 algorithm, operating on 8-bit words
 *
 * Parameters:
 * \param buf - array of bytes on which CRC is run
 * \param bufLen - length of the input array in bytes
 * \param seedCrc - Seed for the next block of memory, use 0 for initial seedCrc.
 * \param finalCrc - 0: return the CRC use for seedSrc. 1: return the final CRC32.
 *
 * \retval     32-bit checksum
 */
uint32_t adrv9001_Crc32ForChunk(const uint8_t buf[], uint32_t bufLen, uint32_t seedCrc, uint8_t finalCrc)
{
    uint32_t i;
    uint32_t a, b, c, d;

    a = seedCrc;

    for (i = 0u; i < bufLen; i++) {
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        d = buf[i];
        a = (a << 4u) | (d >> 4u);
        a = a ^ c;
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        a = (a << 4u) | (d & 0xfu);
        a = a ^ c;
    }

    if (finalCrc > 0)
    {
        for (i = 0u; i < 4u; i++) {
            b = (a >> 28u) & 0xfu;
            c = LUT_CRC32[b];
            a = (a << 4u);
            a = a ^ c;
            b = (a >> 28u) & 0xfu;
            c = LUT_CRC32[b];
            a = (a << 4u);
            a = a ^ c;
        }
    }

    return a;
}



/*! ****************************************************************************
* Function:    Crc32
*
* brief       Run Cyclic Redundancy Check on the specified block of memory
*
* details     CRC32 algorithm, operating on 8-bit words
*
* Parameters:
* param [in]  buf - array of bytes on which CRC is run
* param [in]  bufLen - length of the input array in bytes
*
* return     32-bit checksum
*
* Reference to other related functions
* @sa
*
*******************************************************************************
*/
/*
uint32_t Crc32(const uint8_t buf[], uint32_t bufLen)
{
    uint32_t i;
    uint32_t a, b, c, d;

    a = 0x00000000u;

    for (i = 0u; i < bufLen; i++) {
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        d = buf[i];
        a = (a << 4u) | (d >> 4u);
        a = a ^ c;
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        a = (a << 4u) | (d & 0xfu);
        a = a ^ c;
    }

    for (i = 0u; i < 4u; i++) {
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        a = (a << 4u);
        a = a ^ c;
        b = (a >> 28u) & 0xfu;
        c = LUT_CRC32[b];
        a = (a << 4u);
        a = a ^ c;
    }

    return a;
}
*/
