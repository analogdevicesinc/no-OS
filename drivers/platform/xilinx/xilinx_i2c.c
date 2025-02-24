/***************************************************************************//**
 *   @file   xilinx/xilinx_i2c.c
 *   @brief  Implementation of Xilinx I2C Generic Driver.
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
#ifdef XPAR_XIIC_NUM_INSTANCES
#include <xiic.h>
#endif
#ifdef XPAR_XIICPS_NUM_INSTANCES
#include <xiicps.h>
#endif

#include "no_os_error.h"
#include "no_os_i2c.h"
#include "xilinx_i2c.h"
#include "no_os_list.h"
#include "no_os_alloc.h"

static struct no_os_list_desc *pl_list = NULL;
static struct no_os_iterator  *pl_it = NULL;

#ifdef XIICPS_H
static struct no_os_list_desc *ps_list = NULL;
static struct no_os_iterator  *ps_it = NULL;
#endif /* XIICPS_H */

/**
 * @struct inst_table_item
 * @brief Table item for I2C cores already started.
 */
struct inst_table_item {
	/** Instance the BSP driver uses for this core. */
	void *instance;
	/** Core ID in BSP. */
	uint32_t device_id;
	/** Number of ADI driver instances running on this core. */
	uint32_t inst_no;
};

#ifdef XPAR_XIICPS_NUM_INSTANCES
/** Save the current state of the bitrate to not change it each time */
static uint32_t ps_last_bitrate = 0;
#endif

/**
 * @brief Xilinx platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops xil_i2c_ops = {
	.i2c_ops_init = &xil_i2c_init,
	.i2c_ops_write = &xil_i2c_write,
	.i2c_ops_read = &xil_i2c_read,
	.i2c_ops_remove = &xil_i2c_remove
};

/**
 * @brief Compare two I2C cores by ID.
 * @param el1 - First I2C core to compare.
 * @param el2 - Second I2C core to compare.
 * @return \ref 0 if the two cores have the same ID,
 *         \ref difference between the first and the second IDs otherwise.
 */
static int32_t xil_i2c_cmp(void *el1, void *el2)
{
	struct inst_table_item *sel1 = el1, *sel2 = el2;

	return (sel1->device_id - sel2->device_id);
}

/**
 * @brief Configure slave address and bitrate if needed
 * @param desc - Descriptor of the I2C device
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t xil_i2c_set_transmission_config(struct no_os_i2c_desc *desc)
{
	struct xil_i2c_desc *xil_i2c_desc = desc->extra;
#ifdef XIICPS_H
	int32_t ret;
#endif

	switch (xil_i2c_desc->type) {
	case IIC_PL:
#ifdef XIIC_H
		break;
#endif
		return -1;
	case IIC_PS:
#ifdef XIICPS_H
		if (desc->max_speed_hz != ps_last_bitrate) {
			ret = XIicPs_SetSClk(xil_i2c_desc->instance, desc->max_speed_hz);
			if (ret != XST_SUCCESS)
				return -1;
			ps_last_bitrate = desc->max_speed_hz;
		}
		break;
#endif
		return -1;
	default:

		return -1;
	}

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_i2c_init(struct no_os_i2c_desc **desc,
		     const struct no_os_i2c_init_param *param)
{
	int32_t		ret;
	struct no_os_i2c_desc	*idesc;
	struct xil_i2c_desc	*xdesc;
	struct xil_i2c_init_param	*xinit;

	idesc = (struct no_os_i2c_desc *)no_os_malloc(sizeof(*idesc));
	xdesc = (struct xil_i2c_desc *)no_os_malloc(sizeof(*xdesc));

	if (!idesc || !xdesc)
		goto error;

	idesc->max_speed_hz = param->max_speed_hz;
	idesc->slave_address = param->slave_address;
	xinit = param->extra;

	idesc->extra = xdesc;
	xdesc->type = xinit->type;
	xdesc->device_id = xinit->device_id;
	switch (xinit->type) {
	case IIC_PL:
#ifdef XIIC_H
		; /** Intentional empty statement */
		struct inst_table_item tab_check_pl;
		struct inst_table_item *temp_el_pl;

		if (!pl_list)
			no_os_list_init(&pl_list, NO_OS_LIST_DEFAULT, xil_i2c_cmp);
		if (!pl_it)
			no_os_iterator_init(&pl_it, pl_list, true);

		tab_check_pl.device_id = xdesc->device_id;

		if (!NO_OS_IS_ERR_VALUE(no_os_list_read_find(pl_list, (void **)&temp_el_pl,
					&tab_check_pl))) {
			xdesc->instance = temp_el_pl->instance;
			temp_el_pl->inst_no++;
			break;
		}

		xdesc->instance = (XIic *)no_os_malloc(sizeof(XIic));
		if (!xdesc->instance)
			goto pl_error;

		xdesc->config = XIic_LookupConfig(xinit->device_id);
		if (xdesc->config == NULL)
			goto pl_error;

		ret = XIic_CfgInitialize(xdesc->instance,
					 xdesc->config,
					 ((XIic_Config*)xdesc->config)
					 ->BaseAddress);
		if (ret != 0)
			goto pl_error;

		ret = XIic_Start(xdesc->instance);
		if (ret != 0)
			goto pl_error;

		ret = XIic_SetAddress(xdesc->instance,
				      XII_ADDR_TO_SEND_TYPE,
				      param->slave_address);
		if (ret != 0)
			goto pl_error;

		ret = XIic_SelfTest(xdesc->instance);
		if (ret < 0)
			goto pl_error;

		ret = XIic_SetGpOutput(xdesc->instance, 1);
		if (ret < 0)
			goto pl_error;

		temp_el_pl = (struct inst_table_item*)no_os_calloc(1, sizeof(*temp_el_pl));
		temp_el_pl->device_id = xinit->device_id;
		temp_el_pl->inst_no = 1;
		temp_el_pl->instance = xdesc->instance;
		pl_list->push(pl_list, temp_el_pl);

		break;
pl_error:
		no_os_free(xdesc->instance);
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		; /** Intentional empty statement */
		struct inst_table_item tab_check_ps;
		struct inst_table_item *temp_el_ps;

		if (!pl_list)
			no_os_list_init(&ps_list, NO_OS_LIST_DEFAULT, xil_i2c_cmp);
		if (!pl_it)
			no_os_iterator_init(&ps_it, ps_list, true);

		tab_check_ps.device_id = xdesc->device_id;

		if (!NO_OS_IS_ERR_VALUE(no_os_list_read_find(ps_list,
					(void **)&temp_el_ps,
					&tab_check_ps))) {
			xdesc->instance = temp_el_ps->instance;
			temp_el_ps->inst_no++;
			break;
		}

		xdesc->instance = (XIicPs *)no_os_malloc(sizeof(XIicPs));
		if (!xdesc->instance)
			goto ps_error;

		xdesc->config = XIicPs_LookupConfig(xinit->device_id);
		if (xdesc->config == NULL)
			goto ps_error;

		ret = XIicPs_CfgInitialize(xdesc->instance,
					   xdesc->config,
					   ((XIicPs_Config*)xdesc->config)
					   ->BaseAddress);
		if (ret != 0)
			goto ps_error;

		XIicPs_SetSClk(xdesc->instance, param->max_speed_hz);

		temp_el_ps = (struct inst_table_item*)no_os_calloc(1, sizeof(*temp_el_ps));
		temp_el_ps->device_id = xinit->device_id;
		temp_el_ps->inst_no = 1;
		temp_el_ps->instance = xdesc->instance;
		ps_list->push(ps_list, temp_el_ps);

		break;
ps_error:
		no_os_free(xdesc->instance);
#endif
		goto error;

	default:
		goto error;
		break;
	}

	*desc = idesc;

	return 0;

error:
	no_os_free(idesc);
	no_os_free(xdesc);

	return -1;
}

/**
 * @brief Free the resources allocated by no_os_i2c_init().
 * @param desc - The I2C descriptor.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct xil_i2c_desc	*xdesc;
	int32_t		ret;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		; /** Intentional empty statement */
		struct inst_table_item tab_check_pl;
		struct inst_table_item *temp_el_pl;
		tab_check_pl.device_id = xdesc->device_id;
		ret = no_os_iterator_find(pl_it, &tab_check_pl);
		if (ret != 0)
			goto error;
		no_os_iterator_read(pl_it, (void **)&temp_el_pl);
		temp_el_pl->inst_no--;
		if (temp_el_pl->inst_no != 0)
			break;

		ret = XIic_Stop(((XIic *)xdesc->instance));

		if (ret != 0)
			goto error;

		/** Remove list element */
		no_os_iterator_get(pl_it, (void **)&temp_el_pl);
		no_os_free(temp_el_pl);
		break;
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		; /** Intentional empty statement */
		struct inst_table_item tab_check_ps;
		struct inst_table_item *temp_el_ps;
		tab_check_ps.device_id = xdesc->device_id;
		ret = no_os_iterator_find(ps_it, &tab_check_ps);
		if (ret != 0)
			goto error;
		no_os_iterator_read(ps_it, (void **)&temp_el_ps);
		temp_el_ps->inst_no--;
		if (temp_el_ps->inst_no != 0)
			break;

		/** Remove list element */
		no_os_iterator_get(ps_it, (void **)&temp_el_ps);
		no_os_free(temp_el_ps);
		break;
#endif
		/* Intended fallthrough */
error:
	default:

		return -1;
	}

	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_i2c_write(struct no_os_i2c_desc *desc,
		      uint8_t *data,
		      uint8_t bytes_number,
		      uint8_t stop_bit)
{
	struct xil_i2c_desc	*xdesc;
	int32_t		ret;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		ret = xil_i2c_set_transmission_config(desc);
		if (ret != 0)
			return -1;

		XIic_Send(((XIic*)xdesc->instance)->BaseAddress,
			  desc->slave_address,
			  data,
			  bytes_number,
			  stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
		break;
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		ret = xil_i2c_set_transmission_config(desc);
		if (ret != 0)
			return -1;

		ret = XIicPs_SetOptions(xdesc->instance,
					stop_bit ? 0 : XIICPS_REP_START_OPTION);
		if (ret != 0)
			goto error;

		XIicPs_MasterSend(xdesc->instance,
				  data,
				  bytes_number,
				  desc->slave_address);
		if (ret != 0)
			goto error;

		break;
#endif
		/* Intended fallthrough */
error:
	default:

		return -1;
	}

	return 0;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
int32_t xil_i2c_read(struct no_os_i2c_desc *desc,
		     uint8_t *data,
		     uint8_t bytes_number,
		     uint8_t stop_bit)
{
	struct xil_i2c_desc	*xdesc;
	int32_t		ret;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		ret = xil_i2c_set_transmission_config(desc);
		if (ret != 0)
			return -1;

		ret = XIic_Recv(((XIic*)xdesc->instance)->BaseAddress,
				desc->slave_address,
				data,
				bytes_number,
				stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
		if (ret != bytes_number)
			goto error;

		break;
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		ret = xil_i2c_set_transmission_config(desc);
		if (ret != 0)
			return -1;

		ret = XIicPs_SetOptions(xdesc->instance,
					stop_bit ? 0 : XIICPS_REP_START_OPTION);
		if (ret != 0)
			goto error;

		XIicPs_MasterRecv(xdesc->instance,
				  data,
				  bytes_number,
				  desc->slave_address);
		if (ret != 0)
			goto error;

		break;
#endif
		/* Intended fallthrough */
error:
	default:
		return -1;

		break;
	}

	return 0;
}
