/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adi_adrv904x_radio_seq_image_format_types.h
 *
 * \brief   Contains ADRV904X Radio Sequencer Download File data structures.
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_RADIO_SEQ_IMAGE_FORMAT_TYPES_H__
#define __ADI_ADRV904X_RADIO_SEQ_IMAGE_FORMAT_TYPES_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_version_types.h"

/**
 *   This header defines the Radio Sequencer Image header structure.
 *   The data structures are broken up into several portions since some
 *   some of the data is not exposed to the customer and some portions are.
 *   The final image format is defined as:
 *
 *      uint32_t  magicNumber                             // Unique tag to define file type
 *      adrv904x_Version_t  revision                      // File revision
 *      uint32_t  numberPatterns                          // total number of patterns in sequenceLookupTable[] (max 2048 )
 *      uint32_t  sequenceSize                            // Number of uint32_t in the sequenceInstructions[] (each instructions is 4 uint32_t)
 *      uint32_t  seqStartAddr                            // Start addr of the seq memory region (0x4d300000)
 *      uint32_t  lookupCrc                               // CRC sequenceLookupTable[]
 *      uint32_t  SequenceCrc                             // CRC sequenceInstructions[]
 *      uint32_t  fileImageCrc                            // CRC of the entire file image
 *      uint8_t   xbarSettings[256]                       // cross bar settings (each byte is encoded as: xbar_ctrl_sel[7:4], xbar_sequencer_sel[3:0])
 *      uint32_t  sequenceLookupTable[NumberPatterns]     // Start addr for each seq
 *      uint32_t  sequenceInstructions[sequenceSize*4]    // max 32K bytes.
 */

/**
 *  \brief Definition of the Radio Sequencer Magic file number
 */
#define    ADI_ADRV904X_RADIO_SEQ_MAGIC_NUMBER              (0xAD1000DBu)

/**
 *  \brief Definition of the Radio Sequencer Revision
 */
#define    ADI_ADRV904X_RADIO_SEQ_REVISION                  (0x01u)

/**
 *  \brief Size of the Xbar settings array
 */
#define    ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE                 (256u)

/**
 *  \brief Max size of the lookup table
 */
#define    ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE       (2048u)

/**
 *  \brief Max size of the pattern image array
 */
#define    ADI_ADRV904X_RADIO_SEQ_IMAGE_MAX_SIZE            (32768u)

/**
 *  \brief Memory region of the pattern image
 */
#define    ADI_ADRV904X_RADIO_SEQ_IMAGE_MEM_REGION_START    (0x4d300000U)

/**
 *  \brief Invalid/Disabled setting (xbar value = 255 i.e Sequencer 15 Signal 15) for Radio Sequencer Crossbar
 */
#define    ADI_ADRV904X_RADIO_SEQ_XBAR_INVALID              (255U)


/**
 * \brief Data structure defining the Radio Sequencer image header
 */
ADI_ADRV904X_PACK_START
typedef struct adi_adrv904x_RadioSeqImageHeader
{
    uint32_t               magicNumber;             /*!< Unique tag to define file type */
    adi_adrv904x_Version_t revision;                /*!< Radio Seq image revision */
    uint32_t               numberPatterns;          /*!< Number of patterns defined in the sequence image */
    uint32_t               sequenceSize;            /*!< Number of uint32_t in the sequence (each instructions is 4 uint32_t) */
    uint32_t               seqStartAddr;            /*!< Start addr of the seq memory region */
    uint32_t               lookupCrc;               /*!< Lookup table CRC */
    uint32_t               sequenceCrc;             /*!< Sequence Image CRC */
    uint32_t               fileImageCrc;            /*!< CRC of entire file iamge */
} adi_adrv904x_RadioSeqImageHeader_t;
ADI_ADRV904X_PACK_FINISH

/**
 *  \brief Max size of the radio sequencer image file
 */
#define    ADI_ADRV904X_RADIO_SEQ_IMAGE_FILE_MAX_SIZE    (ADI_ADRV904X_RADIO_SEQ_IMAGE_MAX_SIZE + (ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE * sizeof(uint32_t)) + ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE + sizeof(adi_adrv904x_RadioSeqImageHeader_t))

/**
 * \brief Data structure defining the Radio Sequencer Crossbar Settings
 */
typedef uint8_t*   adi_adrv904x_RadioSeqXbarSettingsPtr_t;

/**
 * \brief Data structure defining the Radio Sequencer Start Address Lookup Table
 */
typedef uint32_t*  adi_adrv904x_RadioSeqLookupTablePtr_t;

/**
 * \brief Data structure defining the Radio Sequencer Pattern Image
 */
typedef uint32_t*  adi_adrv904x_RadioSeqPatternImagePtr_t;

#endif /* __ADI_ADRV904X_RADIO_SEQ_IMAGE_FORMAT_TYPES_H__ */


