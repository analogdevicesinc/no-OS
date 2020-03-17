/***************************************************************************//**
 *   @file   xilinx/i2c.c
 *   @brief  Implementation of Xilinx I2C Generic Driver.
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
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
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>

#include <xparameters.h>
#ifdef XPAR_XIIC_NUM_INSTANCES
#include <xiic.h>
#endif
#ifdef XPAR_XIICPS_NUM_INSTANCES
#include <xiicps.h>
#endif

#include "error.h"
#include "i2c.h"
#include "i2c_extra.h"

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** Table of existing BSP driver handlers */
#ifdef XPAR_XIIC_NUM_INSTANCES
/**
 * @struct pl_table_item
 * @brief Table item for PL I2C cores already started.
 */
struct pl_table_item {
	/** Instance the BSP driver uses for this core. */
	XIic *instance;
	/** Core ID in BSP. */
	uint32_t device_id;
	/** Number of ADI driver instances running on this core. */
	uint32_t inst_no;
};
/** Dynamically allocated table for PL I2C BSP driver instances. */
static struct pl_table_item *pl_table = NULL;
/** Number of PL I2C BSP driver instances. */
static uint32_t pl_core_no = 0;
#endif
#ifdef XPAR_XIICPS_NUM_INSTANCES
/**
 * @struct ps_table_item
 * @brief Table item for PS I2C cores already started.
 */
struct ps_table_item {
	/** Instance the BSP driver uses for this core. */
	XIicPs *instance;
	/** Core ID in BSP. */
	uint32_t device_id;
	/** Number of ADI driver instances running on this core. */
	uint32_t inst_no;
};
/** Dynamically allocated table for PS I2C BSP driver instances. */
static struct ps_table_item *ps_table = NULL;
/** Number of PS I2C BSP driver instances. */
static uint32_t ps_core_no = 0;
/** Save the current state of the bitrate to not change it each time */
static uint32_t ps_last_bitrate = 0;
#endif

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

#ifdef XPAR_XIIC_NUM_INSTANCES
/**
 * @brief Find instance in the PL table using the core ID.
 * @param id - ID of the wanted core.
 * @return \ref PL table index of wanted core, \ref -1 if core not found.
 */
static int32_t pl_inst_tab_find(uint32_t id)
{
	int32_t i = 0;

	for (i = 0; i < pl_core_no; i++)
		if (pl_table[i].device_id == id)
			return (int32_t)i;

	return -1;
}
#endif
#ifdef XPAR_XIICPS_NUM_INSTANCES
/**
 * @brief Find instance in the PS table using the core ID.
 * @param id - ID of the wanted core.
 * @return \ref PS table index of wanted core, \ref -1 if core not found.
 */
static int32_t ps_inst_tab_find(uint32_t id)
{
	int32_t i = 0;

	for (i = 0; i < ps_core_no; i++)
		if (ps_table[i].device_id == id)
			return (int32_t)i;

	return -1;
}
#endif

/**
 * @brief Configure slave address and bitrate if needed
 * @param desc - Descriptor of the I2C device
 * @return \ref SUCCESS in case of success, \ref FAILURE otherwise.
 */
static int32_t i2c_set_transmission_config(struct i2c_desc *desc)
{
	struct xil_i2c_desc *xil_i2c_desc = desc->extra;
	int32_t ret;

	switch (xil_i2c_desc->type) {
	case IIC_PL:
#ifdef XIIC_H
		break;
#endif
		return FAILURE;
	case IIC_PS:
#ifdef XIICPS_H
		if (desc->max_speed_hz != ps_last_bitrate) {
			ret = XIicPs_SetSClk(xil_i2c_desc->instance, desc->max_speed_hz);
			if (ret != XST_SUCCESS)
				return FAILURE;
			ps_last_bitrate = desc->max_speed_hz;
		}
		break;
#endif
		return FAILURE;
	default:

		return FAILURE;
	}

	return SUCCESS;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * @param desc - The I2C descriptor.
 * @param param - The structure that contains the I2C parameters.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_init(struct i2c_desc **desc,
		 const struct i2c_init_param *param)
{
	int32_t		ret;
	i2c_desc	*idesc;
	xil_i2c_desc	*xdesc;
	xil_i2c_init	*xinit;
	int32_t tab_inst;

	idesc = (struct i2c_desc *)malloc(sizeof(*idesc));
	xdesc = (struct xil_i2c_desc *)malloc(sizeof(*xdesc));

	if(!idesc || !xdesc)
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
		struct pl_table_item *tab_check_pl;
		tab_inst = pl_inst_tab_find(xinit->device_id);
		if (tab_inst != -1) {
			xdesc->instance = pl_table[tab_inst].instance;
			pl_table[tab_inst].inst_no++;
			break;
		}

		xdesc->instance = (XIic *)malloc(sizeof(XIic));
		if(!xdesc->instance)
			goto pl_error;

		xdesc->config = XIic_LookupConfig(xinit->device_id);
		if(xdesc->config == NULL)
			goto pl_error;

		ret = XIic_CfgInitialize(xdesc->instance,
					 xdesc->config,
					 ((XIic_Config*)xdesc->config)
					 ->BaseAddress);
		if(ret != SUCCESS)
			goto pl_error;

		ret = XIic_Start(xdesc->instance);
		if(ret != SUCCESS)
			goto pl_error;

		ret = XIic_SetAddress(xdesc->instance,
				      XII_ADDR_TO_SEND_TYPE,
				      param->slave_address);
		if(ret != SUCCESS)
			goto pl_error;

		ret = XIic_SelfTest(xdesc->instance);
		if(ret < 0)
			goto pl_error;

		ret = XIic_SetGpOutput(xdesc->instance, 1);
		if(ret < 0)
			goto pl_error;

		if (!pl_table) {
			pl_table = (struct pl_table_item*)calloc(1, sizeof(*pl_table));
		} else {
			tab_check_pl = (struct pl_table_item*)realloc(pl_table,
					(pl_core_no + 1) * sizeof(*pl_table));
			if (!tab_check_pl)
				goto pl_error;
			pl_table = tab_check_pl;
		}
		pl_table[pl_core_no].device_id = xinit->device_id;
		pl_table[pl_core_no].instance = xdesc->instance;
		pl_table[pl_core_no].inst_no = 1;
		pl_core_no++;

		break;
pl_error:
		free(xdesc->instance);
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		; /** Intentional empty statement */
		struct ps_table_item *tab_check_ps;
		tab_inst = ps_inst_tab_find(xinit->device_id);
		if (tab_inst != -1) {
			xdesc->instance = ps_table[tab_inst].instance;
			ps_table[tab_inst].inst_no++;
			break;
		}

		xdesc->instance = (XIicPs *)malloc(sizeof(XIicPs));
		if(!xdesc->instance)
			goto ps_error;

		xdesc->config = XIicPs_LookupConfig(xinit->device_id);
		if(xdesc->config == NULL)
			goto ps_error;

		ret = XIicPs_CfgInitialize(xdesc->instance,
					   xdesc->config,
					   ((XIicPs_Config*)xdesc->config)
					   ->BaseAddress);
		if(ret != SUCCESS)
			goto ps_error;

		XIicPs_SetSClk(xdesc->instance, param->max_speed_hz);

		if (!ps_table) {
			ps_table = (struct ps_table_item*)calloc(1, sizeof(*ps_table));
		} else {
			tab_check_ps = (struct ps_table_item*)realloc(ps_table,
					(ps_core_no + 1) * sizeof(*ps_table));
			if (!tab_check_ps)
				goto pl_error;
			ps_table = tab_check_ps;
		}
		ps_table[ps_core_no].device_id = xinit->device_id;
		ps_table[ps_core_no].instance = xdesc->instance;
		ps_table[ps_core_no].inst_no = 1;
		ps_core_no++;

		break;
ps_error:
		free(xdesc->instance);
#endif
		goto error;

	default:
		goto error;
		break;
	}

	*desc = idesc;

	return SUCCESS;

error:
	free(idesc);
	free(xdesc);

	return FAILURE;
}

/**
 * @brief Free the resources allocated by i2c_init().
 * @param desc - The I2C descriptor.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_remove(struct i2c_desc *desc)
{
	xil_i2c_desc	*xdesc;
	int32_t		ret;
	int32_t tab_inst;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		; /** Intentional empty statement */
		struct pl_table_item *tab_check_pl;
		tab_inst = pl_inst_tab_find(xdesc->device_id);
		pl_table[tab_inst].inst_no--;
		if (pl_table[tab_inst].inst_no != 0)
			break;

		ret = XIic_Stop(((XIic *)xdesc->instance));

		if(ret != SUCCESS)
			goto error;
		if (pl_table[tab_inst].inst_no == 0) {
			free(xdesc->instance);
			tab_check_pl = (struct pl_table_item *)calloc((pl_core_no - 1),
					sizeof(*tab_check_pl));
			if (!tab_check_pl)
				return FAILURE;
			for (uint32_t i = 0; i < pl_core_no; i++) {
				if (i < tab_inst)
					memcpy(&tab_check_pl[i], &pl_table[i], sizeof(*pl_table));
				else
					memcpy(&tab_check_pl[i], &pl_table[(i + 1)],
					       sizeof(*pl_table));
			}
			free(pl_table);
			pl_table = tab_check_pl;
			pl_core_no--;
		}
		break;
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		; /** Intentional empty statement */
		struct ps_table_item *tab_check_ps;
		tab_inst = ps_inst_tab_find(xdesc->device_id);
		ps_table[tab_inst].inst_no--;
		if (ps_table[tab_inst].inst_no != 0)
			break;

		if (ps_table[tab_inst].inst_no == 0) {
			free(xdesc->instance);
			tab_check_ps = (struct ps_table_item *)calloc((ps_core_no - 1),
					sizeof(*tab_check_ps));
			if (!tab_check_ps)
				return FAILURE;
			for (uint32_t i = 0; i < ps_core_no; i++) {
				if (i < tab_inst)
					memcpy(&tab_check_ps[i], &ps_table[i], sizeof(*ps_table));
				else
					memcpy(&tab_check_ps[i], &ps_table[(i + 1)],
					       sizeof(*ps_table));
			}
			free(ps_table);
			ps_table = tab_check_ps;
			ps_core_no--;
		}
		break;
#endif
		/* Intended fallthrough */
error:
	default:

		return FAILURE;
		break;
	}

	free(desc->extra);
	free(desc);

	return SUCCESS;
}

/**
 * @brief Write data to a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_write(struct i2c_desc *desc,
		  uint8_t *data,
		  uint8_t bytes_number,
		  uint8_t stop_bit)
{
	xil_i2c_desc	*xdesc;
	int32_t		ret;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		ret = i2c_set_transmission_config(desc);
		if (ret != SUCCESS)
			return FAILURE;

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
		ret = i2c_set_transmission_config(desc);
		if (ret != SUCCESS)
			return FAILURE;

		ret = XIicPs_SetOptions(xdesc->instance,
					stop_bit ? 0 : XIIC_REPEATED_START);
		if(ret != SUCCESS)
			goto error;

		XIicPs_MasterSend(xdesc->instance,
				  data,
				  bytes_number,
				  desc->slave_address);
		if(ret != SUCCESS)
			goto error;

		break;
#endif
		/* Intended fallthrough */
error:
	default:

		return FAILURE;
		break;
	}

	return SUCCESS;
}

/**
 * @brief Read data from a slave device.
 * @param desc - The I2C descriptor.
 * @param data - Buffer that will store the received data.
 * @param bytes_number - Number of bytes to read.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return SUCCESS in case of success, FAILURE otherwise.
 */
int32_t i2c_read(struct i2c_desc *desc,
		 uint8_t *data,
		 uint8_t bytes_number,
		 uint8_t stop_bit)
{
	xil_i2c_desc	*xdesc;
	int32_t		ret;

	xdesc = desc->extra;

	switch (xdesc->type) {
	case IIC_PL:
#ifdef XIIC_H
		ret = i2c_set_transmission_config(desc);
		if (ret != SUCCESS)
			return FAILURE;

		ret = XIic_Recv(((XIic*)xdesc->instance)->BaseAddress,
				desc->slave_address,
				data,
				bytes_number,
				stop_bit ? XIIC_STOP : XIIC_REPEATED_START);
		if(ret != bytes_number)
			goto error;

		break;
#endif
		goto error;
	case IIC_PS:
#ifdef XIICPS_H
		ret = i2c_set_transmission_config(desc);
		if (ret != SUCCESS)
			return FAILURE;

		ret = XIicPs_SetOptions(xdesc->instance,
					stop_bit ? 0 : XIIC_REPEATED_START);
		if(ret != SUCCESS)
			goto error;

		XIicPs_MasterRecv(xdesc->instance,
				  data,
				  bytes_number,
				  desc->slave_address);
		if(ret != SUCCESS)
			goto error;

		break;
#endif
		/* Intended fallthrough */
error:
	default:
		return FAILURE;

		break;
	}

	return SUCCESS;
}
