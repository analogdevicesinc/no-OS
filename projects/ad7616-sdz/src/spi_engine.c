/***************************************************************************//**
 *   @file   spi_engine.c
 *   @brief  Implementation of SPI Engine Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2016(c) Analog Devices, Inc.
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
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sleep.h>
#include <xil_io.h>
#include <xscugic.h>
#include <xparameters.h>
#include "spi_engine.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define SPI_ENGINE_INT_ID		88

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
spi_engine spi;
XScuGic_Config *gic_config;
XScuGic gic;

/***************************************************************************//**
* @brief spi_engine_read
*******************************************************************************/
int32_t spi_engine_read(uint32_t reg_addr,
						uint32_t *reg_data)
{
	*reg_data = Xil_In32((spi.baseaddr + reg_addr));

	return 0;
}

/***************************************************************************//**
* @brief spi_engine_write
*******************************************************************************/
int32_t spi_engine_write(uint32_t reg_addr,
						 uint32_t reg_data)
{
	Xil_Out32((spi.baseaddr + reg_addr), reg_data);

	return 0;
}

/***************************************************************************//**
* @brief spi_engine_program_add_cmd
*******************************************************************************/
void spi_engine_program_add_cmd(spi_engine_program *p,
								uint8_t dry,
								uint16_t cmd)
{
	if (!dry)
		p->instructions[p->length] = cmd;
	p->length++;
}

/***************************************************************************//**
* @brief spi_engine_compile_message
*******************************************************************************/
int32_t spi_engine_compile_message(spi_message *msg,
								   uint8_t dry,
								   spi_engine_program *p)
{
	uint32_t clk_div = 0;

	spi_engine_program_add_cmd(p, dry,
			SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CONFIG, msg->spi_config));

	clk_div = (spi.spi_ref_clk / msg->spi_clk) - 1;
	spi_engine_program_add_cmd(p, dry,
			SPI_ENGINE_CMD_WRITE(SPI_ENGINE_CMD_REG_CLK_DIV, clk_div));

	if (msg->cs_change) {
		spi_engine_program_add_cmd(p, dry,
				SPI_ENGINE_CMD_ASSERT(1, (0xff ^ BIT(msg->chip_select))));
	}

	spi_engine_program_add_cmd(p, dry,
			SPI_ENGINE_CMD_TRANSFER(msg->tx_length ? 1 : 0,
					msg->rx_length ? 1 : 0,
					max(msg->tx_length, msg->rx_length) - 1));

	if (msg->cs_change) {
		spi_engine_program_add_cmd(p, dry,
				SPI_ENGINE_CMD_ASSERT(1, 0xff));
	}

	return 0;
}

/***************************************************************************//**
* @brief spi_engine_write_cmd_fifo
*******************************************************************************/
int32_t spi_engine_write_cmd_fifo(void)
{
	uint32_t n, m, i;
	const uint16_t *buf;

	spi_engine_read(SPI_ENGINE_REG_CMD_FIFO_ROOM, &n);

	while (n && spi.cmd_length) {
		m = min(n, spi.cmd_length);
		buf = spi.cmd_buf;
		for (i = 0; i < m; i++)
			spi_engine_write(SPI_ENGINE_REG_CMD_FIFO, buf[i]);
		spi.cmd_length -= m;
		n -= m;
	}

	return spi.cmd_length != 0;
}

/***************************************************************************//**
* @brief spi_engine_write_tx_fifo
*******************************************************************************/
int32_t spi_engine_write_tx_fifo(void)
{
	uint32_t n, m, i;
	const uint8_t *buf;

	spi_engine_read(SPI_ENGINE_REG_SDO_FIFO_ROOM, &n);

	while (n && spi.msg->tx_length) {
		m = min(n, spi.msg->tx_length);
		buf = spi.msg->tx_buf;
		for (i = 0; i < m; i++)
			spi_engine_write(SPI_ENGINE_REG_SDO_DATA_FIFO, buf[i]);
		spi.msg->tx_length -= m;
		n -= m;
	}

	return spi.msg->tx_length != 0;
}

/***************************************************************************//**
* @brief spi_engine_write_tx_fifo
*******************************************************************************/
int32_t spi_engine_read_rx_fifo()
{
	uint32_t n, m, i;
	uint32_t data;
	uint8_t *buf;

	spi_engine_read(SPI_ENGINE_REG_SDI_FIFO_LEVEL, &n);

	while (n && spi.msg->rx_length) {
		m = min(n, spi.msg->rx_length);
		buf = spi.msg->rx_buf;
		for (i = 0; i < m; i++) {
			spi_engine_read(SPI_ENGINE_REG_SDI_DATA_FIFO, &data);
			buf[i] = (uint8_t) data;
		}
		spi.msg->rx_buf += m;
		spi.msg->rx_length -= m;
		n -= m;
	}

	return spi.msg->rx_length != 0;
}

/***************************************************************************//**
* @brief spi_engine_transfer_one_message
*******************************************************************************/
int32_t spi_engine_transfer_one_message(spi_message *msg)
{
	spi_engine_program p_dry, *p;
	size_t size;

	p_dry.length = 0;
	spi_engine_compile_message(msg, true, &p_dry);

	size = sizeof(*p->instructions) * (p_dry.length + 1);
	p = malloc(sizeof(*p) + size);
	if (!p)
		return -1;
	p->length = 0;
	spi_engine_compile_message(msg, false, p);

	spi.sync_id = (spi.sync_id + 1) & 0xff;
	spi_engine_program_add_cmd(p, false,
			SPI_ENGINE_CMD_SYNC(spi.sync_id));

	spi.msg = msg;
	spi.p = p;

	spi.cmd_buf = p->instructions;
	spi.cmd_length = p->length;
	if (spi_engine_write_cmd_fifo())
		spi.int_enable |= SPI_ENGINE_INT_CMD_ALMOST_EMPTY;

	if (spi_engine_write_tx_fifo())
		spi.int_enable |= SPI_ENGINE_INT_SDO_ALMOST_EMPTY;

	if (spi.msg->rx_length != 0)
		spi.int_enable |= SPI_ENGINE_INT_SDI_ALMOST_FULL;

	spi.int_enable |= SPI_ENGINE_INT_SYNC;

	spi_engine_write(SPI_ENGINE_REG_INT_ENABLE, spi.int_enable);

	return 0;
}

/***************************************************************************//**
* @brief spi_engine_isr
*******************************************************************************/
void spi_engine_isr(void *instance)
{
	uint32_t pending;
	uint32_t disable_int = 0;

	spi_engine_read(SPI_ENGINE_REG_INT_PENDING, &pending);

	if (pending & SPI_ENGINE_INT_SYNC) {
		spi_engine_write(SPI_ENGINE_REG_INT_PENDING, SPI_ENGINE_INT_SYNC);
		spi_engine_read(SPI_ENGINE_REG_SYNC_ID, &spi.completed_id);
	}

	if (pending & SPI_ENGINE_INT_CMD_ALMOST_EMPTY) {
		if (!spi_engine_write_cmd_fifo())
			disable_int |= SPI_ENGINE_INT_CMD_ALMOST_EMPTY;
	}

	if (pending & SPI_ENGINE_INT_SDO_ALMOST_EMPTY) {
		if (!spi_engine_write_tx_fifo())
			disable_int |= SPI_ENGINE_INT_SDO_ALMOST_EMPTY;
	}

	if (pending & (SPI_ENGINE_INT_SDI_ALMOST_FULL | SPI_ENGINE_INT_SYNC)) {
		if (!spi_engine_read_rx_fifo())
			disable_int |= SPI_ENGINE_INT_SDI_ALMOST_FULL;
	}

	if (pending & SPI_ENGINE_INT_SYNC) {
		disable_int |= SPI_ENGINE_INT_SYNC;
	}

	if (disable_int) {
		spi.int_enable &= ~disable_int;
		spi_engine_write(SPI_ENGINE_REG_INT_ENABLE, spi.int_enable);
	}

}

/***************************************************************************//**
* @brief spi_engine_offload_enable
*******************************************************************************/
void spi_engine_offload_enable(uint8_t enable)
{
	uint32_t reg;

	spi_engine_read(SPI_ENGINE_REG_OFFLOAD_CTRL(0), &reg);
	if (enable)
		reg |= SPI_ENGINE_OFFLOAD_CTRL_ENABLE;
	else
		reg &= ~SPI_ENGINE_OFFLOAD_CTRL_ENABLE;
	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CTRL(0), reg);
}

/***************************************************************************//**
* @brief spi_engine_offload_load_msg
*******************************************************************************/
int32_t spi_engine_offload_load_msg(spi_message *msg)
{
	spi_engine_program p_dry, *p;
	size_t size;
	uint32_t i;

	p_dry.length = 0;
	spi_engine_compile_message(msg, true, &p_dry);

	size = sizeof(*p->instructions) * (p_dry.length + 2);
	p = malloc(sizeof(*p) + size);
	if (!p)
		return -1;
	p->length = 0;
	spi_engine_compile_message(msg, false, p);
	spi_engine_program_add_cmd(p, false, SPI_ENGINE_CMD_SLEEP(10));
	spi_engine_program_add_cmd(p, false, SPI_ENGINE_CMD_SYNC(0));

	spi_engine_write(SPI_ENGINE_REG_OFFLOAD_RESET(0), 1);
	for (i = 0; i < msg->tx_length; i++)
		spi_engine_write(SPI_ENGINE_REG_OFFLOAD_SDO_MEM(0), msg->tx_buf[i]);

	for (i = 0; i < p->length; i++)
		spi_engine_write(SPI_ENGINE_REG_OFFLOAD_CMD_MEM(0), p->instructions[i]);

	free(p);

	return 0;
}

/***************************************************************************//**
* @brief spi_engine_setup
*******************************************************************************/
int32_t spi_engine_setup(uint32_t baseaddr)
{
	uint32_t version;
	int32_t	 status;

	spi.baseaddr = baseaddr;
	spi.spi_ref_clk = 100000000;
	spi.sync_id = 0;
	spi.int_enable = 0;

	spi_engine_read(SPI_ENGINE_REG_VERSION, &version);
	if (SPI_ENGINE_VERSION_MAJOR(version) != 1) {
		printf("Unsupported peripheral version %u.%u.%c\n",
				(unsigned int)SPI_ENGINE_VERSION_MAJOR(version),
				(unsigned int)SPI_ENGINE_VERSION_MINOR(version),
				(int)SPI_ENGINE_VERSION_PATCH(version));
		return -1;
	}

	spi_engine_write(SPI_ENGINE_REG_RESET, 0x01);
	usleep(100000);
	spi_engine_write(SPI_ENGINE_REG_RESET, 0x00);
	spi_engine_write(SPI_ENGINE_REG_INT_PENDING, 0xff);
	spi_engine_write(SPI_ENGINE_REG_INT_ENABLE, 0x00);

	gic_config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
	if(gic_config == NULL)
		printf("Error\n");

	status = XScuGic_CfgInitialize(&gic, gic_config,
			gic_config->CpuBaseAddress);
	if(status)
		printf("Error\n");

	XScuGic_SetPriorityTriggerType(&gic, SPI_ENGINE_INT_ID, 0x0, 0x3);
	status = XScuGic_Connect(&gic, SPI_ENGINE_INT_ID,
			(Xil_ExceptionHandler)spi_engine_isr, NULL);
	if(status)
		printf("Error\n");

	XScuGic_Enable(&gic, SPI_ENGINE_INT_ID);

	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XScuGic_InterruptHandler, (void *)&gic);
	Xil_ExceptionEnable();

	printf("SPI Engine version %u.%u.%c successfully initialized\n",
			(unsigned int)SPI_ENGINE_VERSION_MAJOR(version),
			(unsigned int)SPI_ENGINE_VERSION_MINOR(version),
			(int)SPI_ENGINE_VERSION_PATCH(version));

	return 0;
}

/***************************************************************************//**
* @brief spi_write_and_read
*******************************************************************************/
int32_t spi_engine_write_and_read(uint8_t ss,
								  uint8_t *data,
								  uint8_t bytes_number)
{
	uint32_t expected_id = 0;
	uint32_t i;
	spi_message message;
	uint8_t tx[8];
	uint8_t rx[8];

	for (i = 0; i < bytes_number; i++)
		tx[i] = data[i];

	message.chip_select = ss;
	message.cs_change = true;
	message.spi_clk = 1000000;
	message.spi_config = SPI_ENGINE_CONFIG_CPOL;
	message.tx_buf = tx;
	message.rx_buf = rx;

	expected_id = spi.sync_id + 1;
	spi.completed_id = 0xFFFFFFFF;

	message.tx_length = bytes_number;
	message.rx_length = bytes_number;
	message.rx_buf = rx;
	spi_engine_transfer_one_message(&message);

	while (spi.completed_id != expected_id);

	for (i = 0; i < bytes_number; i++)
		data[i] = rx[i];

	return 0;
}
