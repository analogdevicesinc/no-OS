/***************************************************************************//**
 *   @file   aducm3029/uart.c
 *   @brief  Implementation of UART driver for ADuCM302x.
 *   @author Mihail Chindris (mihail.chindris@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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

#include "uart.h"
#include "uart_extra.h"
#include <stdlib.h>

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @struct baud_desc
 * @brief Structure of an element from \ref baud_rates_26MHz.
 * The baud rate is calculated with the formula:
 * Baudrate = (CLK_FREQ / (div_m + div_n/2048) * pow(2,osr+2) * div_c)).
 */
struct baud_desc {
	/** Calculated baud rate from the following parameters */
	uint32_t	baud_rate;
	/** From 0 to 2047 */
	uint16_t	div_n;
	/** From 1 to 3 */
	uint8_t		div_m;
	/** From 1 to 65535 */
	uint16_t	div_c;
	/** From 0 to 3 */
	uint8_t		osr;
};

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define MAX_BYTES	1024u
#define CLK_FREQ	26000000u
#define BAUDS_NB	10u
#if defined (__ADUCM4x50__)
/** In ADuCM302x UART0 and UART1 are available */
# define NUM_UART_DEVICES 2
#else
/** In ADuCM302x UART0 is available */
# define NUM_UART_DEVICES 1
#endif

/** Stores calculated baud rates and parameters */
const struct baud_desc baud_rates_26MHz[BAUDS_NB] = {
	{9600, 1078, 3, 24, 3},
	{19200, 1078, 3, 12, 3},
	{38400, 1321, 3, 8, 2},
	{57600, 1078, 3, 4, 3},
	{115200, 1563, 1, 4, 3},
	{230400, 1563, 1, 2, 3},
	{460800, 1563, 1, 1, 3},
	{921600, 1563, 1, 1, 2},
	{1000000,1280, 1, 1, 2},
	{1500000, 171, 1, 1, 2}
};

/**
 * Used to check if device already initialized
 */
static uint32_t initialized[NUM_UART_DEVICES];

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Allocates the memory needed for the UART descriptor
 * @return Address to the allocated memory, NULL if the allocation fails.
 */
static struct uart_desc *alloc_desc_mem(void)
{
	struct uart_desc	*desc;
	struct aducm_uart_desc	*aducm_desc;
	uint32_t		mem;

	desc = calloc(1, sizeof(*desc));
	if (!desc)
		return NULL;
	aducm_desc = calloc(1, sizeof(*aducm_desc));
	if (!aducm_desc) {
		free(desc);
		return NULL;
	}
	desc->extra = aducm_desc;

	/* Allocate and align buffer to 32 bits */
	aducm_desc->adi_uart_buffer = calloc(1, ADI_UART_BIDIR_MEMORY_SIZE + 3);
	if (!aducm_desc->adi_uart_buffer) {
		free(aducm_desc);
		free(desc);
		return NULL;
	}

	mem = (uint32_t)aducm_desc->adi_uart_buffer;
	aducm_desc->adi_uart_buffer = (uint8_t *)((mem+3u) & (~(3u)));
	aducm_desc->adi_uart_buffer_offset =
		(uint32_t)aducm_desc->adi_uart_buffer - mem;

	return desc;
}

/**
 * @brief Deallocates the memory needed for the UART descriptor
 * @param desc - Descriptor to be deallocated
 */
static void free_desc_mem(struct uart_desc *desc)
{
	struct aducm_uart_desc	*aducm_desc = desc->extra;

	free((void *)((uint32_t)aducm_desc->adi_uart_buffer -
		      aducm_desc->adi_uart_buffer_offset));
	aducm_desc->adi_uart_buffer = NULL;
	aducm_desc->adi_uart_buffer_offset = 0;
	free(desc->extra);
	free(desc);
}

/**
 * @brief Call the user defined callback when a read/write operation completed
 * @param desc:		Descriptor of the UART device
 * @param event:	Event ID from ADI_UART_EVENT
 * @param buff:		Pointer to the handled buffer or to an error code
 */
static void uart_callback(void *desc, uint32_t event, void *buff)
{
	struct aducm_uart_desc *extra = ((struct uart_desc *)desc)->extra;

	switch(event) {
	case ADI_UART_EVENT_RX_BUFFER_PROCESSED:
		extra->waiting_read_callback--;
		extra->callback(extra->param, READ_DONE, buff);
		break;
	case ADI_UART_EVENT_TX_BUFFER_PROCESSED:
		extra->waiting_write_callback--;
		extra->callback(extra->param, WRITE_DONE, buff);
		break;
	default: //Error
		extra->waiting_read_callback = 0;
		extra->waiting_write_callback = 0;
		extra->errors |= (uint32_t)buff;
		extra->callback(extra->param, ERROR, buff);
		break;
	}
}

/**
 * @brief Calls the low level read functions depending on the mode
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer where data will be read
 * @param bytes_number:	Number of bytes to be read. Between 1 and 1024
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_read(struct uart_desc *desc, uint8_t *data,
		  uint32_t bytes_number)
{
	/* TODO Improve to read more than 1024 chars */
	uint32_t errors;
	struct aducm_uart_desc *extra;

	if (!desc || !data)
		return FAILURE;

	extra = desc->extra;
	if (bytes_number == 0 || bytes_number > MAX_BYTES) {
		errors = BAD_INPUT_PARAMETERS;
		goto failure;
	}

	if (!extra->callback) { //Blocking mode
		if (ADI_UART_SUCCESS != adi_uart_Read(
			    (ADI_UART_HANDLE const)extra->uart_handler,
			    (void *const)data,
			    (uint32_t const)bytes_number,
			    bytes_number > 4 ? true : false,
			    &errors))
			goto failure;
	} else { //Callback mode
		if (extra->waiting_read_callback) {
			errors = WAITING_FOR_CALLBACK;
			goto failure;
		}
		extra->waiting_read_callback++;
		adi_uart_SubmitRxBuffer(
			(ADI_UART_HANDLE const)extra->uart_handler,
			(void *const)data,
			(uint32_t const)bytes_number,
			bytes_number > 4 ? true : false);
	}

	return SUCCESS;
failure:
	extra->errors |= errors;
	return FAILURE;
}

/**
 * @brief Calls the low level write functions depending on the mode
 * @param desc:	Descriptor of the UART device
 * @param data:	Buffer with the data to be written
 * @param bytes_number:	Number of bytes to be written. Between 1 and 1024
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_write(struct uart_desc *desc, const uint8_t *data,
		   uint32_t bytes_number)
{
	uint32_t errors;
	struct aducm_uart_desc *extra;

	if (!desc || !data)
		return FAILURE;

	extra = desc->extra;
	if (bytes_number == 0 || bytes_number > MAX_BYTES) {
		errors = BAD_INPUT_PARAMETERS;
		goto failure;
	}

	if (!extra->callback) { //Blocking mode
		if (ADI_UART_SUCCESS != adi_uart_Write(
			    (ADI_UART_HANDLE const)extra->uart_handler,
			    (void *const)data,
			    (uint32_t const)bytes_number,
			    bytes_number > 4 ? true : false,
			    &errors))
			goto failure;
	} else { //Callback mode
		if (extra->waiting_write_callback) {
			errors = WAITING_FOR_CALLBACK;
			goto failure;
		}
		extra->waiting_write_callback++;
		adi_uart_SubmitTxBuffer(
			(ADI_UART_HANDLE const)extra->uart_handler,
			(void *const)data,
			(uint32_t const)bytes_number,
			bytes_number > 4 ? true : false);
	}

	return SUCCESS;
failure:
	extra->errors |= errors;
	return FAILURE;
}

/**
 * @brief Initialize the UART communication peripheral.
 *
 * To configure the UART, the user must set the extra parameter from param with
 * a pointer to the configured platform specific structure
 * \ref aducm_uart_init_param .\n
 * The driver have to operating modes:
 * - __Callback mode__: The read/write functions will return immediately and
 * the user callback will be called when the buffer is processed.
 * - __Blocking mode__: The read/write functions will only return when the
 * buffer is processed.
 * .
 * If the callback parameter is set, the driver will work on __callback mode__.
 * If callback is NULL, then it will work in __blocking mode__.\n
 * The __callback mode__ it is not recommended to use, except for "even-driven"
 * peripherals. For more information refer to ADuCM302x DFP Device Driver Users
 * Guide.\n
 * @param desc:  Descriptor of the UART device used in the call of the drivers
 * functions
 * @param param: Descriptor used to configure the UART device
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_init(struct uart_desc **desc, struct uart_init_param *param)
{
	ADI_UART_RESULT			uart_ret;
	struct aducm_uart_desc		*aducm_desc;
	struct aducm_uart_init_param	*aducm_init_param;

	if (!desc || !param || !(param->extra) ||
	    param->device_id >= NUM_UART_DEVICES || //
	    initialized[param->device_id] != 0) //Already initialized
		return FAILURE;

	initialized[param->device_id] = 1;
	*desc = alloc_desc_mem();
	if (!(*desc))
		return FAILURE;
	aducm_desc = (*desc)->extra;
	aducm_init_param = param->extra;

	(*desc)->baud_rate = param->baud_rate;
	(*desc)->device_id = param->device_id;
	aducm_desc->callback = aducm_init_param->callback;
	aducm_desc->param = aducm_init_param->param;
	aducm_desc->waiting_read_callback = 0;
	aducm_desc->waiting_write_callback = 0;

	uart_ret = adi_uart_Open(param->device_id, ADI_UART_DIR_BIDIRECTION,
				 aducm_desc->adi_uart_buffer,
				 ADI_UART_BIDIR_MEMORY_SIZE,
				 &aducm_desc->uart_handler);
	if (uart_ret != ADI_UART_SUCCESS)
		goto failure;

	/* Think about the option of implementing default configuration */
	uart_ret = adi_uart_SetConfiguration(aducm_desc->uart_handler,
					     aducm_init_param->parity,
					     aducm_init_param->stop_bits,
					     aducm_init_param->word_length);
	if (uart_ret != ADI_UART_SUCCESS)
		goto failure;

	/* Configure baud rate */
	uint32_t i, freq;
	for (i = 0; i < BAUDS_NB; i++)
		if (baud_rates_26MHz[i].baud_rate == param->baud_rate)
			break;
	adi_pwr_GetClockFrequency(ADI_CLOCK_PCLK, &freq);
	if (i == BAUDS_NB || freq != CLK_FREQ)
		goto failure;
	uart_ret = adi_uart_ConfigBaudRate(aducm_desc->uart_handler,
					   baud_rates_26MHz[i].div_c,
					   baud_rates_26MHz[i].div_m,
					   baud_rates_26MHz[i].div_n,
					   baud_rates_26MHz[i].osr);
	if (uart_ret != ADI_UART_SUCCESS)
		goto failure;

	/* Register callback */
	if (aducm_desc->callback)
		adi_uart_RegisterCallback(aducm_desc->uart_handler,
					  uart_callback,
					  (*desc));
	return SUCCESS;
failure:
	free_desc_mem(*desc);
	*desc = NULL;
	return FAILURE;
}

/**
 * @brief Free the resources allocated by \ref uart_init()
 * @param desc: Descriptor of the UART device
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
int32_t uart_remove(struct uart_desc *desc)
{
	struct aducm_uart_desc *aducm_desc;

	if (desc == NULL || desc->extra == NULL)
		return FAILURE;
	aducm_desc = desc->extra;

	adi_uart_Close(aducm_desc->uart_handler);
	free_desc_mem(desc);
	initialized[desc->device_id] = 0;

	return SUCCESS;
}

/**
 * @brief Free the resources allocated by \ref uart_init()
 * @param desc: Descriptor of the UART device
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
uint32_t uart_get_errors(struct uart_desc *desc)
{
	struct aducm_uart_desc *extra = desc->extra;
	uint32_t ret = extra->errors;
	extra->errors = 0;

	return ret;
}
