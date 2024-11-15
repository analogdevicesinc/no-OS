/***************************************************************************//**
 *   @file   aducm3029_flash.h
 *   @brief  Header file of the flash driver for ADuCM302x
 *   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ADUCM3029_FLASH_H_
#define ADUCM3029_FLASH_H_

#define ADUCM3029_FLASH_SIZE_BYTES (0x40000u)

#define FLASH_PAGE_SIZE_BYTES 2048
#define FLASH_PAGE_SIZE_WORDS 512
#define FLASH_PAGE_ADDR_SHIFT 11

#define FLASH_PAGE_START_ADDRESS_MASK 0x7FF
#define FLASH_OFFSET_IN_PAGE(x) (((x) & FLASH_PAGE_START_ADDRESS_MASK) / 4)
#define FLASH_ADDRESS_PAGE_START(x) ((x) & ~FLASH_PAGE_START_ADDRESS_MASK)

#endif /* ADUCM3029_FLASH_H_ */
