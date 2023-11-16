/*
 * adi_fmc_fru.h
 * Copyright (C) 2012 Analog Devices
 * Author : Robin Getz <robin.getz@analog.com>
 *
 * This file is maintained as part of:
 *    https://github.com/analogdevicesinc/fru_tools
 * but is released under this license, so you can use it without having 
 * your software fall under the GPL. If you make improvements to this,
 * although you are not required, it would be nice if you sent me a patch.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of Analog Devices, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 * - The use of this software may or may not infringe the patent rights
 *   of one or more patent holders.  This license does not release you
 *   from the requirement that you obtain separate licenses from these
 *   patent holders to use this software.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 */

#ifndef _ADI_FMC_FRU_H__
#define _ADI_FMC_FRU_H__

#include "adi_platform.h"
#include <stdbool.h>

#ifndef UNUSED
#  ifdef __GNUC__
#    define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#  else
#    define UNUSED(x) UNUSED_ ## x
#  endif
#endif

/* 
 * These structures/data are based from:
 * Platform Management FRU Information
 * Storage Definition
 * Document Revision 1.1, September 27, 1999
 * http://download.intel.com/design/servers/ipmi/FRU1011.pdf
 * 
 * ANSI/VITA 57.1
 * FPGA Mezzanine Card (FMC) Standard
 * Approved July 2008 (Revised February 2010)
 * Used with permission
 */

#define CUSTOM_FIELDS 10

/* Defines from section 13 "TYPE/LENGTH BYTE FORMAT"  of FRU spec) */
#define FRU_STRING_BINARY  0
#define FRU_STRING_BCD     1
#define FRU_STRING_SIXBIT  2
#define FRU_STRING_ASCII   3

#define TYPE_CODE(x)  ((x[0] >> 6) & 0x3)
#define FIELD_LEN(x)  (x[0] & 0x3F)

typedef struct BOARD_INFO
{
    unsigned char language_code;
    unsigned int mfg_date;
    unsigned char *manufacturer;
    unsigned char *product_name;
    unsigned char *serial_number;
    unsigned char *part_number;
    unsigned char *FRU_file_ID;
    unsigned char *custom[CUSTOM_FIELDS];
} BOARD_INFO_t;

#define NUM_MULTI     3
#define NUM_SUPPLIES 12

typedef struct MULTIRECORD_INFO
{
    unsigned char *supplies[NUM_SUPPLIES];
    unsigned char *connector;
    unsigned char *i2c_devices;
} MULTIRECORD_INFO_t;

#define MULTIRECORD_I2C 1
#define MULTIRECORD_CONNECTOR 0

#define MULTIRECORD_DC_OUTPUT 1
#define MULTIRECORD_DC_INPUT  2
/* 0xfa is the FMC-specific MultiRecords, see Rule Rule 5.77 in the FMC spec */
#define MULTIRECORD_FMC       0xFA
/* VITA's Organizationally Unique Identifier - see rule 5.77 in the FMC spec */
#define VITA_OUI 0x0012A2

typedef struct FRU_DATA
{
    char* Internal_Area;
    char* Chassis_Info;
    BOARD_INFO_t* Board_Area;
    char* Product_Info;
    MULTIRECORD_INFO_t* MultiRecord_Area;
} FRU_DATA_t;

/**
* \brief Service to Pass String Message to Standard Error I/O Device
*
* \param[in] msg String Message
* \param[in] ... Variadic Argument(s)
*
*/
ADI_API_EX void fru_PrintError(const char* const msg, ...);

/**
* \brief Service to Pass String Message to Standard Output I/O Device
*
* NOTE: Quiet & Verbose Flags Checked
*
* \param[in] msg String Message
* \param[in] ... Variadic Argument(s)
*
*/
ADI_API_EX void fru_PrintWarning(const char* const msg, ...);

/**
* \brief Service to Pass String Message to Standard Output I/O Device
*
* NOTE: Quiet Flag Checked
*
* \param[in] msg String Message
* \param[in] ... Variadic Argument(s)
*
*/
ADI_API_EX void fru_PrintInfo(const char* const msg, ...);

/**
* \brief Service to Parse Data Stream into FRU Structure
*
* Common Header Format
* Section 8 in the Platform Management FRU Information Storage Definition
*
* \param[in] rawEepromData Raw EEPROM Data Stream
* \param[out] fruRecord FRU Record Data Structure ( NOTE: Dynamically Allocated)
*
* \retval ADI_HAL_ERR_OK Function completed successfully
*/
ADI_API_EX adi_hal_Err_e fru_FruParse(const unsigned char* const rawEepromData, FRU_DATA_t** const fruRecord);

/**
* \brief Service to Write FRU Structure to EEPROM
*
* \param[in] fru FRU Structure to be written to File
* \param[in] file_name File Path for EEPROM File
*
*/
ADI_API_EX void fru_FruWrite(const FRU_DATA_t* const fru, const char* const file_name, const bool packed);


/**
* \brief Service to Parse Data into Board Area Format
*
* NOTE: Platform Management FRU Information Storage Definition: Section 11
*
* \param[in] data - Data to be Parsed
*
* \retval BOARD_INFO_t Board Info Structure (Dynamically Allocated)
*/
ADI_API_EX BOARD_INFO_t* fru_BoardAreaParse(const unsigned char* const data);

/**
* \brief Service to Parse Data into MultiBoard Area Format
*
* Each record in this area begins with a pre-defined header as specified in the
* section 16 in the Platform Management FRU Information Storage Definition.
* This header contains a 'Type' field that identifies what information is
* contained in the record.
*
* There are some FMC specific headers, defined
* in section 5.5.1 of the FMC specification "IPMI Support". These FMC specific
* sections have a 1 byte sub-type, and a 3 byte Unique Organization Identifier
*
* \param[in] data - Data to be Parsed
*
* \retval MULTIRECORD_INFO_t Multi Board Area Structure (Dynamically Allocated)
*/
ADI_API_EX MULTIRECORD_INFO_t* fru_MultiboardAreaParse(const unsigned char* const data);

/**
* \brief Service to Free FRU Structure
*
* \param[in] fru FRU Structure
*
*/
ADI_API_EX void fru_FruFree(FRU_DATA_t* fru);

/**
* \brief Service to Parse FRU structure into a Binary Blob
*
* \param[in] fru FRU Structure
* \param[out] length Size of Binary Blob
* \param[in] packed Flag for 6-bit ASCII where possible
*
* \retval unsigned char* Pointer to Binary Blob (Dynamically Allocated)
*/
ADI_API_EX unsigned char* fru_FruBlobBuild(const FRU_DATA_t* const fru, size_t* const length, const bool packed);

/**
* \brief Service to Dynamically Allocate Memory
*
* \param[in] nmemb number of elements to be allocated
* \param[in] size size of elements
*
* \retval void* pointer to the allocated memory 
*/
ADI_API_EX void* fru_Calloc(const size_t nmemb, const size_t size);

/**
* \brief Service to Write Field to Standard Output I/O Device
*
* \param[in] description - Description of Field being Dumped
* \param[in] offset - Field Offset
* \param[in] field - Field Parameter to be Dumped
*
*/
ADI_API void fru_FruFieldDump(const char* const description, const size_t offset, const unsigned char* const field);

/**
* \brief Service to Write Board Info to Standard Output I/O Device
*
* \param[in] boardInfo - Board Info Structure to be Dumped
*
*/
ADI_API void fru_BoardDump(const BOARD_INFO_t* const boardInfo);

/**
* \brief Service to Write Multirecord Info to Standard Output I/O Device
*
* \param[in] multirecordInfo - Multirecord Structure to be Dumped
*
*/
ADI_API void fru_MultiRecordDump(const MULTIRECORD_INFO_t* const multirecordInfo);

/**
* \brief Service to Write I2C Info to Standard Output I/O Device
*
* \param[in] multirecordInfo - Multirecord Structure with I2C Info to be Dumped
*
*/
ADI_API void fru_I2cDump(const MULTIRECORD_INFO_t* const multirecordInfo);

/**
* \brief Service to Write FMC Info to Standard Output I/O Device
*
* \param[in] multirecordInfo - Multirecord Structure with FMC Info to be Dumped
*
*/
ADI_API void fru_FmcDump(const MULTIRECORD_INFO_t* const multirecordInfo);

#endif  /* _FMC_FRU_H_ */
