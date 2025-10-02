/***************************************************************************//**
 *   @file   pal_flash.c
 *   @brief  Flash platform specific implementation used by Cordio.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "pal_flash.h"
#include <string.h>

static uint8_t flash_memory[1024 * 16]; // 16KB simulated flash

void PalFlashInit(PalFlashCback_t actCback)
{
	(void)actCback;
	memset(flash_memory, 0xFF, sizeof(flash_memory));
}

void PalFlashDeInit(void)
{
	// Stub implementation
}

PalFlashState_t PalNvmGetState(void)
{
	return PAL_FLASH_STATE_READY;
}

uint32_t PalNvmGetTotalSize(void)
{
	return sizeof(flash_memory);
}

uint32_t PalNvmGetSectorSize(void)
{
	return 4096; // 4KB sectors
}

void PalFlashRead(void *pBuf, uint32_t size, uint32_t srcAddr)
{
	if (srcAddr + size <= sizeof(flash_memory)) {
		memcpy(pBuf, &flash_memory[srcAddr], size);
	}
}

void PalFlashWrite(void *pBuf, uint32_t size, uint32_t dstAddr)
{
	if (dstAddr + size <= sizeof(flash_memory)) {
		memcpy(&flash_memory[dstAddr], pBuf, size);
	}
}

void PalFlashEraseSector(uint32_t size, uint32_t startAddr)
{
	if (startAddr + size <= sizeof(flash_memory)) {
		memset(&flash_memory[startAddr], 0xFF, size);
	}
}

void PalFlashEraseChip(void)
{
	memset(flash_memory, 0xFF, sizeof(flash_memory));
}