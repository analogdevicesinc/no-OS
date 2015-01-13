/*
 * fru_tools.h
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

#ifndef __fru_tools__
#define __fru_tools__

#include <stdbool.h>
#include <time.h>

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

struct BOARD_INFO {
	unsigned char language_code;
	unsigned int mfg_date;
	unsigned char *manufacturer;
	unsigned char *product_name;
	unsigned char *serial_number;
	unsigned char *part_number;
	unsigned char *FRU_file_ID;
	unsigned char *custom[CUSTOM_FIELDS];
};

#define NUM_MULTI     3
#define NUM_SUPPLIES 12

struct MULTIRECORD_INFO {
	unsigned char *supplies[NUM_SUPPLIES];
	unsigned char *connector;
	unsigned char *i2c_devices;
};

#define MULTIRECORD_I2C 1
#define MULTIRECORD_CONNECTOR 0

#define MULTIRECORD_DC_OUTPUT 1
#define MULTIRECORD_DC_INPUT  2
/* 0xfa is the FMC-specific MultiRecords, see Rule Rule 5.77 in the FMC spec */
#define MULTIRECORD_FMC       0xFA
/* VITA’s Organizationally Unique Identifier - see rule 5.77 in the FMC spec */
#define VITA_OUI 0x0012A2

struct FRU_DATA {
	char *Internal_Area;
	char *Chassis_Info;
	struct BOARD_INFO *Board_Area;
	char *Product_Info;
	struct MULTIRECORD_INFO *MultiRecord_Area;
};

#define printf_err(args...)		printf(args)
#define printf_warn(args...)	printf(args)
struct FRU_DATA * parse_FRU (unsigned char *);
void free_FRU (struct FRU_DATA * fru);
unsigned char * build_FRU_blob (struct FRU_DATA *, size_t *, bool);
time_t min2date(unsigned int mins);

#endif  /* __fru_tools__ */
