#ifndef _ADRV9001_CRC32_H_
#define _ADRV9001_CRC32_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief       Run Cyclic Redundancy Check on the specified block of memory in chunk.
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
*
*
*******************************************************************************
*/
uint32_t adrv9001_Crc32ForChunk(const uint8_t buf[], uint32_t bufLen, uint32_t seedCrc, uint8_t finalCrc);

#ifdef __cplusplus
}
#endif

#endif