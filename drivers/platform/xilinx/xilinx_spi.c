/***************************************************************************//**
 *   @file   xilinx/xilinx_spi.c
 *   @brief  Implementation of Xilinx SPI Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
*******************************************************************************/

#include <stdlib.h>

#include <xparameters.h>
#ifdef XPAR_XSPI_NUM_INSTANCES
#include <xspi.h>
#endif
#ifdef XPAR_XSPIPS_NUM_INSTANCES
#include <xspips.h>
#endif

#include "no_os_error.h"
#include "no_os_spi.h"
#include "no_os_alloc.h"
#include "xilinx_spi.h"

#if defined(PLATFORM_ZYNQ)
#define SPI_CLK_FREQ_HZ(dev)	(XPAR_PS7_SPI_ ## dev ## _SPI_CLK_FREQ_HZ)
#define SPI_NUM_INSTANCES	XPAR_XSPIPS_NUM_INSTANCES
#elif defined(PLATFORM_ZYNQMP)
#define SPI_CLK_FREQ_HZ(dev)	(XPAR_PSU_SPI_ ## dev ## _SPI_CLK_FREQ_HZ)
#define SPI_NUM_INSTANCES	XPAR_XSPIPS_NUM_INSTANCES
#else
#define SPI_NUM_INSTANCES	0
#endif

#warning SPI delays are not supported on the xilinx platform

/**
 * @brief Initialize the hardware SPI peripherial
 *
 * @param xdesc Platform specific SPI descriptor
 * @param xinit Platform specific SPI init param
 * @return int32_t -1 if the device couldn't be found or configured
 */
static int32_t spi_init_pl(struct no_os_spi_desc *desc,
			   const struct no_os_spi_init_param *param)
{
#ifdef XSPI_H
	int32_t				ret;
	struct xil_spi_desc 		*xdesc;
	struct xil_spi_init_param	*xinit;

	xdesc = (struct xil_spi_desc*)no_os_malloc(sizeof(struct xil_spi_desc));
	if (!xdesc) {
		no_os_free(xdesc);
		return -1;
	}

	desc->extra = xdesc;
	xinit = param->extra;
	xdesc->type = xinit->type;
	xdesc->flags = xinit->flags;

	xdesc->instance = (XSpi*)no_os_calloc(1, sizeof(XSpi));
	if (!xdesc->instance)
		goto pl_error;

	xdesc->config = XSpi_LookupConfig(param->device_id);
	if (xdesc->config == NULL)
		goto pl_error;

	ret = XSpi_CfgInitialize(xdesc->instance,
				 xdesc->config,
				 ((XSpi_Config*)xdesc->config)
				 ->BaseAddress);
	if (ret != 0)
		goto pl_error;

	ret = XSpi_Initialize(xdesc->instance, param->device_id);
	if (ret != 0)
		goto pl_error;

	ret = XSpi_SetOptions(xdesc->instance,
			      XSP_MASTER_OPTION |
			      ((desc->mode & NO_OS_SPI_CPOL) ?
			       XSP_CLK_ACTIVE_LOW_OPTION : 0) |
			      ((desc->mode & NO_OS_SPI_CPHA) ?
			       XSP_CLK_PHASE_1_OPTION : 0));
	if (ret != 0)
		goto pl_error;

	ret = XSpi_Start(xdesc->instance);
	if (ret != 0)
		goto pl_error;

	XSpi_IntrGlobalDisable((XSpi *)(xdesc->instance));

	return 0;

pl_error:
	no_os_free(xdesc->instance);
	no_os_free(xdesc);
#endif
	return -1;
}

/**
 * @brief Initialize the hardware SPI peripherial
 *
 * @param xdesc Platform specific SPI descriptor
 * @param xinit Platform specific SPI init param
 * @return int32_t -1 if the device couldn't be found or configured
 */
static int32_t spi_init_ps(struct no_os_spi_desc *desc,
			   const struct no_os_spi_init_param *param)
{
#ifdef XSPIPS_H
	int32_t				ret;
	struct xil_spi_desc 		*xdesc;
	struct xil_spi_init_param	*xinit;
	const uint32_t			prescaler_default = XSPIPS_CLK_PRESCALE_64;
	const uint32_t			prescaler_min = XSPIPS_CLK_PRESCALE_4;
	const uint32_t			prescaler_max = XSPIPS_CLK_PRESCALE_256;
	uint32_t			prescaler = 0u;
	uint32_t			input_clock = 0u;

	xdesc = (struct xil_spi_desc*)no_os_malloc(sizeof(struct xil_spi_desc));
	if (!xdesc) {
		no_os_free(xdesc);
		return -1;
	}

	desc->extra = xdesc;
	xinit = param->extra;
	xdesc->type = xinit->type;
	xdesc->flags = xinit->flags;

	xdesc->instance = (XSpiPs*)no_os_malloc(sizeof(XSpiPs));
	if (!xdesc->instance)
		goto ps_error;

	xdesc->config = XSpiPs_LookupConfig(param->device_id);
	if (xdesc->config == NULL)
		goto ps_error;

	ret = XSpiPs_CfgInitialize(xdesc->instance,
				   xdesc->config,
				   ((XSpiPs_Config*)xdesc->config)
				   ->BaseAddress);
	if (ret != 0)
		goto ps_error;

	switch (param->device_id) {
#if (SPI_NUM_INSTANCES >= 1)
	case 0:
		input_clock = SPI_CLK_FREQ_HZ(0);
		break;
#endif
#if (SPI_NUM_INSTANCES >= 2)
	case 1:
		input_clock = SPI_CLK_FREQ_HZ(1);
		break;
#endif
	default:
		goto ps_error;
	};

	if (desc->max_speed_hz != 0u) {
		uint32_t div = input_clock / desc->max_speed_hz;
		uint32_t rem = input_clock % desc->max_speed_hz;
		uint32_t po2 = !(div & (div - 1)) && !rem;

		// find the power of two just higher than div and
		// store the exponent in prescaler
		while (div) {
			prescaler += 1;
			div >>= 1u;
		}

		// this exponent - 1 is needed because of the way
		// xilinx stores it into registers
		if (prescaler)
			prescaler -= 1;

		// this exponent - 1 is needed when initial div was
		// precisely a power of two
		if (prescaler && po2)
			prescaler -= 1;

		if (prescaler < prescaler_min)
			prescaler = prescaler_min;

		if (prescaler > prescaler_max)
			prescaler = prescaler_max;
	} else
		prescaler = prescaler_default;

	ret = XSpiPs_SetClkPrescaler(xdesc->instance, prescaler);

	if (ret != 0)
		goto ps_error;

	return 0;

ps_error:
	no_os_free(xdesc->instance);
	no_os_free(xdesc);
#endif
	return -1;
}

/**
 * @brief Initialize the SPI communication peripheral.
 * @param desc - The SPI descriptor.
 * @param param - The structure that contains the SPI parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_spi_init(struct no_os_spi_desc **desc,
		     const struct no_os_spi_init_param *param)
{
	int32_t				ret;
	enum xil_spi_type		*spi_type;

	if (!param) {
		return -1;
	}

	*desc = no_os_malloc(sizeof(**desc));
	if (! *desc) {
		no_os_free(*desc);
		return -1;
	}

	spi_type = param->extra;

	if (!spi_type)
		goto init_error;

	(*desc)->max_speed_hz = param->max_speed_hz;
	(*desc)->mode = param->mode;
	(*desc)->bit_order = param->bit_order;
	(*desc)->chip_select = param->chip_select;

	switch (*spi_type) {
	case SPI_PL:
		ret = spi_init_pl(*desc, param);
		break;
	case SPI_PS:
		ret = spi_init_ps(*desc, param);
		break;

	default:
		goto init_error;
		break;
	}

	if (ret != 0) {
init_error:
		no_os_free(*desc);

		return -1;
	}

	return ret;
}

/**
 * @brief Free the resources allocated by no_os_spi_init().
 * @param desc - The SPI descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_spi_remove(struct no_os_spi_desc *desc)
{
#ifdef XSPI_H
	int32_t				ret;
#endif
	struct xil_spi_desc	*xdesc = NULL;
	enum xil_spi_type	*spi_type;

	spi_type = desc->extra;

	if (!spi_type)
		return -1;

	switch (*spi_type) {
	case SPI_PL:
#ifdef XSPI_H
		xdesc = desc->extra;

		if (!xdesc)
			return -1;

		ret = XSpi_Stop((XSpi *)(xdesc->instance));
		if (ret != 0)
			goto error;
#endif
		break;
	case SPI_PS:
#ifdef XSPIPS_H
		xdesc = desc->extra;

		if (!xdesc)
			return -1;
#endif
		break;

		/* Intended fallthrough */
#ifdef XSPI_H
error:
#endif
	default:
		return -1;
	}

	if (xdesc)
		no_os_free(xdesc->instance);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write and read data to/from SPI.
 * @param desc - The SPI descriptor.
 * @param data - The buffer with the transmitted/received data.
 * @param bytes_number - Number of bytes to write/read.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_spi_write_and_read(struct no_os_spi_desc *desc,
			       uint8_t *data,
			       uint16_t bytes_number)
{
	int32_t			ret;
#ifdef XSPI_H
	uint32_t		ctrl_reg;
#endif
	struct xil_spi_desc	*xdesc;
	enum xil_spi_type	*spi_type;

	ret = -1;

	spi_type = desc->extra;

	if (!spi_type)
		return -1;

	xdesc = desc->extra;

	switch (*spi_type) {
	case SPI_PL:
#ifdef XSPI_H
		ret = XSpi_SetOptions(xdesc->instance,
				      XSP_MASTER_OPTION |
				      ((desc->mode & NO_OS_SPI_CPOL) ?
				       XSP_CLK_ACTIVE_LOW_OPTION : 0) |
				      ((desc->mode & NO_OS_SPI_CPHA) ?
				       XSP_CLK_PHASE_1_OPTION : 0));
		if (ret != 0)
			goto error;

		ctrl_reg = XSpi_GetControlReg(((XSpi*)xdesc->instance));
		if (desc->bit_order == NO_OS_SPI_BIT_ORDER_LSB_FIRST)
			ctrl_reg |= XSP_CR_LSB_MSB_FIRST_MASK;
		else
			ctrl_reg &= ~ XSP_CR_LSB_MSB_FIRST_MASK;
		XSpi_SetControlReg(((XSpi*)xdesc->instance), ctrl_reg);

		ret = XSpi_SetSlaveSelect(xdesc->instance,
					  0x01 << desc->chip_select);
		if (ret != 0)
			goto error;

		ret = XSpi_Transfer(xdesc->instance,
				    data,
				    data,
				    bytes_number);
		if (ret != 0)
			goto error;
#endif
		break;
	case SPI_PS:
#ifdef XSPIPS_H
		ret = XSpiPs_SetOptions(xdesc->instance,
					XSPIPS_MASTER_OPTION |
					((xdesc->flags & SPI_CS_DECODE) ?
					 XSPIPS_DECODE_SSELECT_OPTION : 0) |
					XSPIPS_FORCE_SSELECT_OPTION |
					((desc->mode & NO_OS_SPI_CPOL) ?
					 XSPIPS_CLK_ACTIVE_LOW_OPTION : 0) |
					((desc->mode & NO_OS_SPI_CPHA) ?
					 XSPIPS_CLK_PHASE_1_OPTION : 0));
		if (ret != 0)
			goto error;

		ret = XSpiPs_SetSlaveSelect(xdesc->instance,
					    desc->chip_select);
		if (ret != 0)
			goto error;
		ret = XSpiPs_PolledTransfer(xdesc->instance,
					    data,
					    data,
					    bytes_number);
		if (ret != 0)
			goto error;
		ret = XSpiPs_SetSlaveSelect(xdesc->instance, SPI_DEASSERT_CURRENT_SS);
		if (ret != 0)
			goto error;
#endif
		break;
error:
	default:
		return -1;
	}

	return ret;
}

/**
 * @brief Xilinx platform specific SPI platform ops structure
 */
const struct no_os_spi_platform_ops xil_spi_ops = {
	.init = &xil_spi_init,
	.write_and_read = &xil_spi_write_and_read,
	.remove = &xil_spi_remove
};
