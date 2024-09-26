/**************************************************************************//**
*   @file   ADMP441.h
*   @brief  Implementation of header file for ADMP441.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
* EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************
*   SVN Revision: 468
******************************************************************************/
#ifndef ADMP441_H_
#define ADMP441_H_

/*****************************************************************************/
/************************ Constants Definitions ******************************/
/*****************************************************************************/
// Wave Canonical File Format
// To be written at the beginning of the *.wav file
#define CHUNK_RIFF_ID 	0x52494646													// R I F F 								- big endian
#define CHUNK_SIZE      bigToLittle(4 + 8 + SUB1_SIZE_CALC + 8 + CHUNK2_SZ_CALC) 	// Chunk Size 							- little endian
#define WAVE_FORMAT		0x57415645													// W A V E 								- big endian
#define SUBCHUNK1_ID	0x666d7420													// f m t								- big endian
#define SUBCHUNK1_SIZE  0x10000000      											// 16 for PCM, 							- little endian
#define SUB1_SIZE_CALC	16															// Subchunk1 Size used for calculations
#define AUDIO_FORMAT	0x0100														// Audio Format, 1 for PCM				- little endian
#define NR_CHANNELS 	0x0100														// Number of Channels - 1				- little endian
#define SAMPLE_RATE		0xA0BE0000													// Audio Sample Rate - 48800 			- little endian
#define BYTE_RATE		0x80fa0200					 								// SR * NrCh * NrBits / 8               - little endian
#define BLOCK_ALIGN		0x0400														// NrCh * NrBits / 8                    - little endian
#define NR_BITS			0x2000														// Number of bits						- little endian
#define SUBCHUNK2_ID	0x64617461													// d a t a								- big endian
#define SUBCHUNK2_SIZE  bigToLittle(nrSamples * 1 * 32 / 8)							// size of data							- little endian
#define CHUNK2_SZ_CALC	(nrSamples * 1 * 32 / 8)									// Subchunk2 Size used for calculations

#endif /* ADMP441_H_ */
