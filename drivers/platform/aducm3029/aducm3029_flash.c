/***************************************************************************//**
 *   @file   aducm3029_flash.c
 *   @brief  Implementation of the flash driver for ADuCM302x.
 *   @author Andrei Drimbarean (adnrei.drimbarean@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
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
 *    from this software without specific prior written permission.
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

/******************************************************************************/
/************************* Include Files **************************************/
/******************************************************************************/

#include <drivers/flash/adi_flash.h>
#include <drivers/dma/adi_dma.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "no_os_flash.h"
#include "aducm3029_flash.h"
#include "no_os_error.h"
#include "no_os_alloc.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct adicup_flash_dev
 * @brief Aducm3029 flash controller handler
 */
struct adicup_flash_dev {
	/** ADuCM3029 DFP driver handler */
	ADI_FEE_HANDLE instance;
	/** Necessary memory for DFP driver */
	uint8_t dfp_memory[ADI_FEE_MEMORY_SIZE];
	/** Buffer to read one flash page */
	uint32_t temp_ptr[FLASH_PAGE_SIZE_WORDS] __attribute__ ((aligned (sizeof(
				uint32_t))));
};

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * Initialize flash controller.
 *
 * @param [out] device     - Pointer to the driver handler.
 * @param [in]  init_param - Pointer to the initialization structure.
 *
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_flash_init(struct no_os_flash_dev **device,
			 struct no_os_flash_init_param *init_param)
{
	int32_t ret;
	struct no_os_flash_dev *dev;
	struct adicup_flash_dev *adicup_extra;

	dev = (struct no_os_flash_dev *)no_os_calloc(1, sizeof(*dev));
	if (!dev)
		return -1;

	dev->id = init_param->id;
	adicup_extra = (struct adicup_flash_dev *)no_os_calloc(1,
			sizeof(*adicup_extra));
	if (!adicup_extra)
		goto error_dev;

	dev->extra = adicup_extra;
	dev->flash_size = ADUCM3029_FLASH_SIZE_BYTES;
	dev->page_size = FLASH_PAGE_SIZE_BYTES;

	/* Open flash device */
	ret = adi_fee_Open(dev->id, adicup_extra->dfp_memory,
			   ADI_FEE_MEMORY_SIZE,
			   &adicup_extra->instance);
	if(ret != ADI_FEE_SUCCESS)
		goto error_adicup;

	*device = dev;

	return 0;

error_adicup:
	no_os_free(adicup_extra);
error_dev:
	no_os_free(dev);

	return -1;
}

/**
 * Free memory allocated by no_os_flash_init().
 *
 * @param [in] dev - Pointer to the driver handler.
 *
 * @return 0 in case of success, error code otherwise.
 */
int32_t no_os_flash_remove(struct no_os_flash_dev *dev)
{
	int32_t ret;
	struct adicup_flash_dev *adicup_extra;

	if (!dev)
		return -1;

	adicup_extra = dev->extra;

	ret = adi_fee_Close(adicup_extra->instance);
	if (ret != ADI_FEE_SUCCESS)
		return -1;

	no_os_free(adicup_extra);
	no_os_free(dev);

	return 0;
}

/**
 * Erase a flash page.
 *
 * @param [in] dev        - Pointer to the flash device handler.
 * @param [in] page_no    - Page number.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_flash_clear_page(struct no_os_flash_dev *dev, int32_t page_no)
{
	struct adicup_flash_dev *adicup_extra = dev->extra;
	uint32_t fee_hw_error;
	int32_t ret;

	if (page_no > (dev->flash_size / dev->page_size))
		return -1;

	ret = adi_fee_PageErase(adicup_extra->instance, page_no, page_no,
				&fee_hw_error);
	if(ret != ADI_FEE_SUCCESS)
		return -1;

	return 0;
}

/**
 * Write a page in flash memory.
 *
 * @param [in] dev        - Pointer to the flash device handler.
 * @param [in] page_no    - Page number.
 * @param [in] data       - Pointer to the data to be written.
 *                          data length must be FLASH_PAGE_SIZE_WORDS in size.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_flash_write_page(struct no_os_flash_dev *dev, int32_t page_no,
			       uint32_t *data)
{
	ADI_FEE_TRANSACTION transaction;
	struct adicup_flash_dev *adicup_extra = dev->extra;
	uint32_t fee_hw_error;
	int32_t ret;

	if (page_no > (dev->flash_size / dev->page_size))
		return -1;

	transaction.bUseDma = true;
	transaction.nSize = FLASH_PAGE_SIZE_BYTES;
	transaction.pWriteAddr = (uint32_t *)(page_no << FLASH_PAGE_ADDR_SHIFT);
	transaction.pWriteData = data;

	ret = adi_fee_Write(adicup_extra->instance, &transaction,
			    &fee_hw_error);
	if(ret != ADI_FEE_SUCCESS)
		return -1;

	return 0;
}

/**
 * Read-modify-write helper function for no_os_flash_write(). Since the smallest write
 * unit for the ADuCM3029 is the page this function helps access data smaller than
 * a page or accross multiple pages.
 */
static int32_t flash_read_then_write(struct no_os_flash_dev *dev,
				     uint32_t flash_addr,
				     uint32_t *array, uint32_t array_size)
{
	struct adicup_flash_dev *adicup_extra = dev->extra;
	uint32_t page_nr, fee_hw_error;
	int32_t ret;

	if ((flash_addr & 0x3) != 0)
		return -1;
	if ((FLASH_OFFSET_IN_PAGE(flash_addr) + array_size) >
	    FLASH_PAGE_SIZE_WORDS)
		return -1;

	no_os_flash_read(dev, FLASH_ADDRESS_PAGE_START(flash_addr),
			 adicup_extra->temp_ptr,
			 FLASH_PAGE_SIZE_WORDS);

	memcpy(adicup_extra->temp_ptr + FLASH_OFFSET_IN_PAGE(flash_addr), array,
	       array_size * sizeof(uint32_t));

	/* Get the page number */
	ret = adi_fee_GetPageNumber(adicup_extra->instance, flash_addr,
				    &page_nr);
	if(ret != ADI_FEE_SUCCESS)
		return -1;

	/* First erase page */
	ret = adi_fee_PageErase(adicup_extra->instance, page_nr, page_nr,
				&fee_hw_error);
	if(ret != ADI_FEE_SUCCESS)
		return -1;

	return no_os_flash_write_page(dev, page_nr, adicup_extra->temp_ptr);
}

/**
 * Write data in flash memory.
 *
 * @param [in] dev        - Pointer to the flash device handler.
 * @param [in] flash_addr - Start address of the write.
 * @param [in] array      - Pointer to the data to be written.
 * @param [in] array_size - Size of the written data in 32-bit words.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_flash_write(struct no_os_flash_dev *dev, uint32_t flash_addr,
			  uint32_t *array, uint32_t array_size)
{
	int32_t ret;
	uint32_t i, len;

	i = 0;
	while (i < array_size) {
		len = FLASH_PAGE_SIZE_WORDS - FLASH_OFFSET_IN_PAGE(flash_addr + i);
		if (len > array_size - i)
			len = array_size - i;
		ret = flash_read_then_write(dev, flash_addr + i, array + i, len);
		if (ret != 0)
			return -1;
		i += len;
	}

	return 0;
}

/**
 * Read data from the flash memory.
 *
 * @param [in] dev        - Pointer to the flash device handler.
 * @param [in] flash_addr - Start address of the read.
 * @param [out] array     - Pointer to the container for the read data.
 * @param [in]  size      - Size of the read data in 32-bit words.
 *
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t no_os_flash_read(struct no_os_flash_dev *dev, uint32_t flash_addr,
			 uint32_t *array,
			 uint32_t size)
{
	memcpy(array, (uint32_t *)flash_addr, size * sizeof(uint32_t));

	return 0;
}
