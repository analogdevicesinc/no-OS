/***************************************************************************//**
 *   @file   main.c
 *   @brief  Demo for ad5592r
 *   @author Villyam (vilmoscsaba.jozsa@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#include <stdio.h>
#include <stdlib.h>
#include "sys_platform.h"
#include "gpio.h"
#include "local_uart.h"
#include "i2c_master.h"
#include "spi_master.h"
#include "hal.h"

#include "no_os_spi.h"
#include "no_os_delay.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "lattice_spi.h"
#include "ad5592r.h"

struct gpio_instance gpio0_inst;
struct local_uart_instance uart0_inst;
struct spim_instance spi0_inst;
struct i2cm_instance i2c0_inst;

#define CPU_FREQUENCY 100000000

static int l_uart_putc(char c, FILE *file){
	int ret = EOF;
	ret = local_uart_putc(&uart0_inst, c);
	if (c == '\n' && ret == 0)
		ret = local_uart_putc(&uart0_inst, '\r');
	return ret;
}
static int l_uart_getc(FILE *file){
	return local_uart_getc(&uart0_inst, (unsigned char *) file);
}

void system_init()
{

    gpio_init(&gpio0_inst,
					GPIO0_INST_BASE_ADDR,
					GPIO0_INST_LINES_NUM,
					GPIO0_INST_GPIO_DIRS);
	local_uart_init(&uart0_inst,
					CPU0_INST_LOCAL_UART_BASE_ADDR,
					CPU_FREQUENCY,
					CPU0_INST_BAUD_RATE,
					CPU0_INST_LCR_STOP_BITS,
					CPU0_INST_LCR_DATA_BITS);
	i2c_master_init(&i2c0_inst, I2C0_INST_BASE_ADDR);

	iob_init(l_uart_putc, l_uart_getc, (lscc_flush_func) NULL);
}

int main(void)
{
    system_init();

	static uint8_t idx = 0;
	static uint8_t pin_state = 0xFF;
	uint16_t window[1000];
	uint8_t *spi_buff = malloc(10*sizeof(uint8_t));
	if(!spi_buff){
		printf("error spi_buff malloc\n");
	}
	uint8_t *i2c_buff = malloc(8*sizeof(uint8_t));
	if(!i2c_buff){
		printf("error i2c_buff malloc\n");
	}
	unsigned int i = 0;
	unsigned int j = 0;

	spi_buff[0] = 127;
	spi_buff[1] = 128;

	i2c_buff[0] = 127;
	i2c_buff[1] = 127;

	uint16_t value[8];

	struct ad5592r_dev my_ad5592;

	struct latt_spi_init_param latt_spi_init = {
		.input_clock = 100000000,
		.slave_count = 1,
		.base_addr = SPI0_INST_BASE_ADDR,
		.type = SPI_PL
	};

	struct no_os_spi_init_param init_spi = {
			.device_id = SPI0_INST_BASE_ADDR,
			.max_speed_hz = 1000000u,
			.chip_select = 0,
			.mode = NO_OS_SPI_MODE_2,
			.platform_ops = &latt_spi_ops,
			.extra = &latt_spi_init
	};
	struct ad5592r_init_param default_init_param;

	my_ad5592.num_channels = 8;
	my_ad5592.channel_modes[0] = 1;
	my_ad5592.channel_modes[1] = 1;
	my_ad5592.channel_modes[2] = 1;
	my_ad5592.channel_modes[3] = 1;
	my_ad5592.channel_modes[4] = 1;
	my_ad5592.channel_modes[5] = 1;
	my_ad5592.channel_modes[6] = 1;
	my_ad5592.channel_modes[7] = 1;

	int ret;

	ret = no_os_spi_init(&my_ad5592.spi, &init_spi);
	if(ret != 0){
		printf("something error no_os_spi_init\n");
	}

	ret = ad5592r_init(&my_ad5592,&default_init_param);
	if(ret != 0){
		printf("something error ad5592r_init\n");
	}

	ad5592r_reg_write(&my_ad5592, AD5592R_REG_CTRL, 0x20);
	ad5592r_reg_write(&my_ad5592, AD5592R_REG_DAC_EN, 0x0);
	ad5592r_reg_write(&my_ad5592, AD5592R_REG_ADC_EN, 0xff);
	ad5592r_reg_write(&my_ad5592, AD5592R_REG_ADC_SEQ, 0xff);
	ad5592r_reg_write(&my_ad5592, AD5592R_REG_PD, 0x200);

    while(1){
		for(int i=0; i<1000;i++){
			ad5592r_read_adc(&my_ad5592,0,&window[i]);
		}
		for(int i=0; i<1000;i++){
	    	printf("%u\n",window[i]&0x0fff);
		}
    }

	return EXIT_SUCCESS;
}



