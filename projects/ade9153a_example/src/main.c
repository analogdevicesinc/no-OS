/***************************************************************************//**
 *   @file   main.c
 *   @brief  Main function of ADE9153A example.
 *   @author Radu Etz (radu.etz@analog.com)
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
#include "no_os_uart.h"
#include "no_os_pwm.h"
#include "no_os_delay.h"
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_print_log.h"
#include "no_os_units.h"
#include "no_os_util.h"
#include "no_os_error.h"
#include "maxim_uart.h"
#include "maxim_gpio.h"
#include "maxim_uart_stdio.h"
#include "maxim_pwm.h"
#include "maxim_spi.h"
#include "ade9153a.h"
#include "common_data.h"

int main(void)
{
	uint8_t i = 0;
	uint8_t ret;
	uint32_t val = 0, val1 = 0;
	uint8_t data[16] = {0};

	struct ade9153a_init_param ade9153a_ip;
	struct ade9153a_dev *ade9153a_dev;

	struct no_os_uart_desc *uart_desc;
	struct no_os_gpio_desc *gpio_desc;
	struct no_os_gpio_desc *reset_desc;
	struct no_os_gpio_desc *ss_desc;
	struct no_os_gpio_desc *sck_desc;
	struct no_os_spi_desc *spi_desc;

	struct ade9153a_temperature_value temp_val;
	struct ade9153a_energy_values energy_vals;
	struct ade9153a_power_values power_vals;
	struct ade9153a_rms_values rms_vals;
	struct ade9153a_half_rms_values hrms_vals;
	struct ade9153a_pq_values pq_vals;
	// struct no_os_irq_desc *ade9153a_gpio_irq_desc;

	struct max_uart_init_param max_uart_extra_ip = {
		.flow = UART_FLOW_DIS
	};

	struct max_gpio_init_param gpio_extra = {
		.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct max_spi_init_param ade9153a_spi_extra_ip  = {
	.num_slaves = 1,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
	};

	struct no_os_uart_init_param uart_ip = {
		.device_id = 0,
		.baud_rate = 115200,
		.size = NO_OS_UART_CS_8,
		.parity = NO_OS_UART_PAR_NO,
		.stop = NO_OS_UART_STOP_1_BIT,
		.platform_ops = &max_uart_ops,
		.extra = &max_uart_extra_ip,
	};

	struct no_os_gpio_init_param gpio_led1_ip = {  //led 1
		.port = GPIO_LED_PORT,
		.number = GPIO_LED_PIN,
		.pull = NO_OS_PULL_UP,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra,
	};

	struct no_os_gpio_init_param gpio_ss_ip = {  //ss pin
		.port = GPIO_SS_PORT,
		.number = GPIO_SS_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra,
	};

	struct no_os_gpio_init_param gpio_sck_ip = {  //sck pin
		.port = GPIO_SCK_PORT,
		.number = GPIO_SCK_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra,
	};
	
	struct no_os_gpio_init_param gpio_reset_ip = {  //reset pin
		.port = GPIO_RESET_PORT,
		.number = GPIO_RESET_PIN,
		.pull = NO_OS_PULL_NONE,
		.platform_ops = &max_gpio_ops,
		.extra = &gpio_extra,
	};

	// struct no_os_gpio_init_param ade9153a_gpio_rdy_ip = {
	// .port = GPIO_RDY_PORT,
	// .number = GPIO_RDY_PIN,
	// .pull = NO_OS_PULL_NONE,
	// .platform_ops = &max_gpio_ops,
	// .extra = &gpio_extra,
	// };

	struct no_os_spi_init_param ade9153a_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS,
	.extra = &ade9153a_spi_extra_ip,
	};

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret < 0)
		return ret;

	ret = no_os_gpio_get_optional(&gpio_desc,
			&gpio_led1_ip);
	if (ret)
		return ret;
	
	ret = no_os_gpio_get_optional(&ss_desc,
			&gpio_ss_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get_optional(&sck_desc,
			&gpio_sck_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get_optional(&reset_desc,
			&gpio_reset_ip);
	if (ret)
		return ret;

	if (gpio_desc)
		ret = no_os_gpio_direction_output(gpio_desc,
			NO_OS_GPIO_LOW);
	if (ret)
		return ret;


	/* Initialize GPIO IRQ controller */
	// ret = no_os_irq_ctrl_init(&ade9153a_gpio_irq_desc, &ade9153a_gpio_irq_ip);
	// if (ret)
	// 	return ret;

	// ade9153a_ip.irq_ctrl = ade9153a_gpio_irq_desc;
	// ade9153a_ip.drdy_callback = supply_irq_handler;

	/* Initialize SPI */
	ade9153a_ip.spi_init = &ade9153a_spi_ip;
	// ADE9153a dev DATA_RDY init params
	//ade9153a_ip.gpio_rdy = &ade9153a_gpio_rdy_ip;;
	// ADE9153a dev RESET init params
	ade9153a_ip.gpio_reset = &gpio_reset_ip;
	ade9153a_ip.gpio_ss = &gpio_ss_ip;
	ade9153a_ip.gpio_sck = &gpio_sck_ip;
	ade9153a_ip.spi_rs232_select = SPI_SERIAL;
	ade9153a_ip.reset_delay = RESET_TIME;

	// initialization for EV_ADE9153a_SHIELDZ
	ade9153a_ip.ai_swap = ENABLE;
	// PGA gain set to 16
	ade9153a_ip.ai_pga_gain = ADE9153a_AI_GAIN_16;
	// hpf corner frequency
	ade9153a_ip.hpf_crn = ADE9153a_HPF_CORNER_0_625_HZ;
	// frequency select
	ade9153a_ip.freq = F_50_HZ;
	// Vlevel value assuming 1/2 full scale
	ade9153a_ip.vlevel = ADE9153A_VLEVEL;
	// V_div rsmall value
	ade9153a_ip.vlevel = ADE9153A_RSMALL;
	// Energy accumulate set to 4000
	ade9153a_ip.no_samples = ADE9153A_NO_SAMPLES;
	// AIGAIN for IAP-IAN swap
	ade9153a_ip.ai_gain = ADE9153A_AIGAIN;
	
	no_os_uart_stdio(uart_desc);

	pr_info(" \n");
	pr_info("\n");
	pr_info("Hello \n");
	

	/* Init ade9153a struct */
	ade9153a_dev = (struct ade9153a_dev *)no_os_calloc(1, sizeof(*ade9153a_dev));
	if (!ade9153a_dev)
		return -ENOMEM;

	ret = ade9153a_hw_reset(ade9153a_dev);

	no_os_mdelay(ade9153a_dev->reset_delay);

	ret = ade9153a_init(&ade9153a_dev, ade9153a_ip);

	no_os_mdelay(ade9153a_dev->reset_delay);
	
	// ret = ade9153a_temp_en(ade9153a_dev);

	// ret = ade9153a_temp_start(ade9153a_dev);
	
	if (ret)
		return ret;

	// uint32_t *data5, *data6, *data9;
	// uint32_t data7, data8;
	// uint32_t offset;
	// uint32_t gain;
	float tempvalue;
	// double temperature2, *ptr;
	// ptr = &temperature2;
	// int16_t temperature;

	// ret = ade9153a_temp_offset_val(ade9153a_dev, &data5);

	// offset = data5;

	// printf("offset %d: \n", offset);

	// ret = ade9153a_temp_gain_val(ade9153a_dev, &data6);

	// gain = data6;

	// printf("gain %d: \n", gain);

	// no_os_mdelay(ade9153a_dev->reset_delay);

	// ret = ade9153a_temp_result(ade9153a_dev, &data7);

	// temperature = data7;

	// tempvalue = ((float)offset / (float)( 1 << 5 )) - (((float)temperature * (float)gain) / (float)( 1 << 17 ));

	// printf("Temperature: %f \n", tempvalue);

	// set the number of samples used for temperature value
	ret = ade9153a_setup(ade9153a_dev, ade9153a_ip);

	ret = ade9153a_temp_time(ade9153a_dev, ADE9153a_TEMP_TIME_SAMPLES_1024);

	ret = ade9153a_oi_lvl_val (ade9153a_dev, &val);
	pr_info("VAL %x \n", val);
	// ret = ade9153a_temp_val(ade9153a_dev, &temp_val);

	// printf("Temperature: %lf deg C \n", temp_val.temperature_val);

	// ret = ade9153a_energy_vals(ade9153a_dev, &energy_vals);

	// printf("Active energy: %lf mWhr \n", energy_vals.active_energy);

	// printf("Fundamental reactive energy: %lf mVARhr \n", energy_vals.fundamental_reactive_energy);

	// printf("Apparent energy: %lf mVAhr \n", energy_vals.apparent_energy);

	// ret = ade9153a_power_vals(ade9153a_dev, &power_vals);

	// printf("Active power: %lf mW \n", power_vals.active_power);

	// printf("Fundamental reactive power: %lf mVAR \n", power_vals.fundamental_reactive_power);

	// printf("Apparent power: %lf mVA \n", power_vals.apparent_power);

	// ret = ade9153a_rms_vals(ade9153a_dev, &rms_vals);

	// printf("I rms: %lf mA \n", rms_vals.current_rms_val);

	// printf("V rms: %lf mV \n", rms_vals.voltage_rms_val);

	// ret = ade9153a_half_rms_vals(ade9153a_dev, &hrms_vals);

	// printf("I half rms: %lf mA \n", hrms_vals.current_h_rms_val);

	// printf("V half rms: %lf mV \n", hrms_vals.voltage_h_rms_val);

	// ret = ade9153a_power_quality_vals(ade9153a_dev, &pq_vals);

	// printf("Power factor: %lf \n", pq_vals.power_factor_val);

	// printf("Frequency %lf Hz \n", pq_vals.frequency_val);

	// printf("Angle: %lf deg \n", pq_vals.angle_ai_av_val);

	float temperature_val;
	float active_energy, reactive_energy, apparent_energy;
	float active_power, reactive_power, apparent_power;
	float current_rms_val, voltage_rms_val;
	float current_h_rms_val, voltage_h_rms_val;
	float constant, power_factor_val, frequency_val, angle_ai_av_val;

	while(1){
		no_os_udelay(2000000);
	
	ret = ade9153a_temp_val(ade9153a_dev, &temp_val);

	// compute the deg C value using the trim values
	temperature_val = ((float)temp_val.offset_reg_val / (float)( 1 << 5 )) - (((float)temp_val.temperature_reg_val * (float)temp_val.gain_reg_val) / (float)( 1 << 17 ));
	pr_info("Temperature: %.2f deg C \n", temperature_val);

	// compute energy values
	ret = ade9153a_energy_vals(ade9153a_dev, &energy_vals);

	active_energy = (float)energy_vals.active_energy_reg_val * 0.858307 / 1000;
	// value in mWhr

	reactive_energy = (float)energy_vals.fundamental_reactive_energy_reg_val * 0.858307 / 1000;
	// value in mVARhr

	apparent_energy = (float)(energy_vals.apparent_energy_reg_val) * 0.858307 / 1000;
	// value in mVAhr

	pr_info("Active energy: %.2f Whr \n", active_energy / 1000);
	pr_info("Fundamental reactive energy: %.2f mVARhr \n", reactive_energy / 1000);
	pr_info("Apparent energy: %.2f mVAhr \n", apparent_energy / 1000);

	// compute power values
	ret = ade9153a_power_vals(ade9153a_dev, &power_vals);

	active_power = (float)power_vals.active_power_reg_val * 1508.743 / 1000;
	// value in mW

	reactive_power = (float)power_vals.fundamental_reactive_power_reg_val * 1508.73 / 1000;
	// value in mVAR
	
	apparent_power = (float)power_vals.apparent_power_reg_val * 1508.73 / 1000;
	// value in mVA

	pr_info("Active power: %.2f W \n", active_power / 1000);
	pr_info("Fundamental reactive power: %.2f VAR \n", reactive_power / 1000);
	pr_info("Apparent power: %.2f VA \n", apparent_power / 1000);

	//compute rms values
	ret = ade9153a_rms_vals(ade9153a_dev, &rms_vals);

	current_rms_val = (float)(rms_vals.current_rms_reg_val) * 0.838190 / 1000;
	// value in mA

	voltage_rms_val = (float)rms_vals.voltage_rms_reg_val * 13.41105 / 1000;
	// value in mV

	pr_info("I rms: %lf mA \n", current_rms_val);
	pr_info("V rms: %lf mV \n", voltage_rms_val);

	//compute half rms values
	ret = ade9153a_half_rms_vals(ade9153a_dev, &hrms_vals);

	current_h_rms_val = (float)hrms_vals.current_h_rms_reg_val * 0.838190 / 1000;
	// value in mA

	voltage_h_rms_val = (float)hrms_vals.voltage_h_rms_reg_val * 13.41105 / 1000;
	// value in mV

	pr_info("I half rms: %lf mA \n", current_h_rms_val);
	pr_info("V half rms: %lf mV \n", voltage_h_rms_val);

	//compute power quality values
	ret = ade9153a_power_quality_vals(ade9153a_dev, &pq_vals);

	power_factor_val = (float)pq_vals.power_factor_reg_val / (float)134217728;

	frequency_val = (float)(4000 * 65536) / (float)(pq_vals.period_reg_val +1);
	// frequency value Hz

	/* status of freq select */
	uint8_t status;

	// multiplier constant for selected frequency
	ret = ade9153a_freq_s_status(ade9153a_dev, &status);
	if (!status)
		constant = 0.017578125;
	else
		constant = 0.02109375;
	angle_ai_av_val = (float)pq_vals.angle_ai_av_reg_val * constant;
	// angle value in degrees
	
	pr_info("Power factor: %lf \n", power_factor_val);
	pr_info("Frequency %lf Hz \n", frequency_val);
	pr_info("Angle: %lf deg \n", angle_ai_av_val);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");
		interface_toggle_led(gpio_desc);
	}

remove_uart:
	no_os_uart_remove(uart_desc);

	return ret;

}

/**
 * @brief Toggle Led
 * @param gpio_led_desc - led descriptor
 * @return 0 in case of success, negative error code otherwise.
 */
int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
{
	uint8_t val;
	int ret;

	ret = no_os_gpio_get_value(gpio_led_desc, &val);
	if (ret)
		return ret;

	if (val == NO_OS_GPIO_LOW)
		val = NO_OS_GPIO_HIGH;
	else
		val = NO_OS_GPIO_LOW;

	return no_os_gpio_set_value(gpio_led_desc, val);
}


// 	uint32_t data_read;
// 	/* Init the device */
// 	ret = ade9153a_init(&ade9153a_dev, ade9153a_ip);
// 	if (ret)
// 		return ret;

// 	//ret = ade9153a_run(ade9153a_dev);
// 	ret = ade9153a_write(ade9153a_dev, ADE9153A_REG_RUN, 0x0001);

// 	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_RUN, &data_read);

// 	printf("run: %x\n", data_read);
// 	// ret = ade9153a_setup(ade9153a_dev);
// 	// if (ret)
// 	// 	return ret;

	

// 	no_os_udelay(200000);

//  	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_VERSION_PRODUCT , &data_read);


// 	printf("version: %x\n", data_read);
// 	/* Wait to recover after reset */
// 	no_os_udelay(500000);
	
// 	// data[1] = 0xF8;
// 	// data[0] = 0x40;

// 	//ret = no_os_spi_write_and_read(ade9153a_dev->spi_desc, data, 8);
// 	// if (ret)
// 	//	return ret;

// 	// val1 = (uint32_t)no_os_get_unaligned_le32(&data[2]);
// 	// no_os_memswap64(&val1, 4, 4);
// 	// val = (uint32_t) (data[5] | ((uint16_t)data[4] << 8) |
// 	// 	((uint32_t)data[3] << 16) | ((uint32_t)data[2] << 24));

// 	// printf("Received data: ");
// 	// for (i = 0; i < 8; i++)
// 	// 		printf (" 0x%x ", data[i]);
// 	// printf("\n");


// 	// ret = ade9153a_run(ade9153a_dev);

// 	// no_os_udelay(200000);

//  	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_VERSION_PRODUCT , &data_read);


// 	// printf("version: %x\n", data_read);

// // 	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_CONFIG0, &data_read);

// // 	printf("config0 reg init: %x\n", data_read);

// // //ret = ade9153a_wr_lock(ade9153a_dev);

// // 	//ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_WR_LOCK, &data_read);


// // 	//printf("read lock dev: %x\n", data_read);

// // 	ret = ade9153a_hpf_disable(ade9153a_dev);

// // 	//ret = ade9153a_write(ade9153a_dev, ADE9153A_REG_OI_LVL, 0x005B);

// // 	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_CONFIG0, &data_read);

// // 	printf("val 32 after write: %x\n", data_read);

// 	uint32_t temperature;
// 	uint32_t gain, offset;
// 	uint8_t status;

// // 	ret = ade9153a_compmode(ade9153a_dev);

// // //	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_COMPMODE, &data_read);

// //  //	printf("comp mode: %x\n", data_read);

	

// // 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_RUN, &data_read);

// //  	// printf("run cmd: %d\n", data_read);

// // 	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_VLEVEL, &data_read);

// //  	 printf("VLEVEL: %x\n", data_read);

// // 	//ret = ade9153a_enable_temp_rdy_int(ade9153a_dev);

	

// 	ret = ade9153a_temp_en(ade9153a_dev);

// 	//ret = ade9153a_run(ade9153a_dev);
// 	no_os_udelay(20000);

// 	ret = ade9153a_temp_time(ade9153a_dev, ADE9153a_TEMP_TIME_SAMPLES_512);

// 	ret = ade9153a_temp_start(ade9153a_dev);

// 	//ret = ade9153a_enable_temp_rdy_int(ade9153a_dev);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_LAST_CMD, &data_read);

// 	// printf("data read 0x%x \n", data_read);



// 	//ret = ade9153a_write(ade9153a_dev, ADE9153A_REG_TEMP_CFG, 0x000C);

// 	//ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_TEMP_CFG, &data_read);

//  //	printf("temp cfg: %x\n", data_read);

// 	no_os_udelay(2000000);

// 	while(!(status)){
// 		ret = ade9153a_get_temp_rdy(ade9153a_dev, &status);
// 		// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_STATUS, &data_read);
// 		// printf("Status reg %d, status %d\n", data_read & 0x00001000, status);
// 		no_os_udelay(500000);

// 	}

	



	
// 	printf("status %d \n", status);

// 	ret = ade9153a_temp_offset_val(ade9153a_dev, &offset);

// 	ret = ade9153a_temp_gain_val(ade9153a_dev, &gain);

// 	printf("offset, gain: %d , %d \n", offset, gain);

// 	// 	ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_LAST_DATA_32, &data_read);

// 	// printf("data read 32 0x%x \n", data_read);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_LAST_DATA_16, &data_read);

// 	// printf("data read 16 0x%x \n", data_read);

// 	//ret = ade9153a_get_temp_rdy(ade9153a_dev, &status);

// 	//printf("status: 0x%x\n", status);
// 	ret = ade9153a_clear_temp_rdy(ade9153a_dev);
// 	status = 0;

// 	while(1){

	
// 	//ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_LAST_DATA_16, &data_read);

// 	// printf("data read 16 0x%x \n", data_read);

// 	ret = ade9153a_temp_start(ade9153a_dev);

// //	no_os_udelay(2500000);

// 	while(!(status)){

// 	ret = ade9153a_get_temp_rdy(ade9153a_dev, &status);

// 	//printf("status %d \n", status);
// 	}

// 	ret = ade9153a_temp_result(ade9153a_dev, &temperature);

// 	ret = ade9153a_clear_temp_rdy(ade9153a_dev);
// 	status = 0;

// 	printf("temperature: %d\n", temperature);

// 	//int32_t temp1, temp2;
// 	float tempvalue;

// 	tempvalue = ((float)offset / (float)( 32 )) - (((float)temperature * ((float)gain) / (float)( 131072 )));

// 	// temp1 = no_os_sign_extend32((int32_t) gain / (1 << 17),16);
// 	// temp1 = -temp1;
// 	// temp2 = (int32_t)((int32_t) temperature * temp1);
// 	// temp2 = temp2 + no_os_sign_extend32((int32_t) offset / (1 << 5),16);

// 	//printf("Temperature : %d \n", temp2);


// 	printf("temperature float: %f\n", tempvalue);
// 	}


// 	// ret = ade9153a_wr_unlock(ade9153a_dev);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_WR_LOCK, &data_read);


// 	// printf("read lock dev: %x\n", data_read);

// 	// ret = ade9153a_write(ade9153a_dev, ADE9153A_REG_OI_LVL, 0x005B);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_OI_LVL, &data_read);

// 	// printf("val 32 after write: %x\n", data_read);


	
// 	//ret = ade9153a_sw_reset(ade9153a_dev);

// 	//ret = ade9153a_hw_reset(ade9153a_dev);

	
// 	//no_os_udelay(500000);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_OI_LVL, &data_read);

// 	// printf("val 32 after reset: %x\n", data_read);



// 	// ret = ade9153a_write(ade9153a_dev, ADE9153A_REG_OI_LVL, 0x005B);

// 	// ret = ade9153a_read(ade9153a_dev, ADE9153A_REG_OI_LVL, &data_read);

// 	// printf("val 32 after write: %x\n", data_read);

// 	uint32_t ch;


// 	ch = no_os_test_bit(no_os_find_first_set_bit(ADE9153A_IPPHASE_MSK), VALOARE);

// 	printf("ch: 0x%d", ch);

// 	no_os_udelay(10000);

// 	while(1){
// 		no_os_udelay(500000);
// 		interface_toggle_led(gpio_desc);
// 	}

// remove_uart:
// 	no_os_uart_remove(uart_desc);

// 	return ret;
// }

// /**
//  * @brief Toggle Led
//  * @param gpio_led_desc - led descriptor
//  * @return 0 in case of success, negative error code otherwise.
//  */
// int interface_toggle_led(struct no_os_gpio_desc *gpio_led_desc)
// {
// 	uint8_t val;
// 	int ret;

// 	ret = no_os_gpio_get_value(gpio_led_desc, &val);
// 	if (ret)
// 		return ret;

// 	if (val == NO_OS_GPIO_LOW)
// 		val = NO_OS_GPIO_HIGH;
// 	else
// 		val = NO_OS_GPIO_LOW;

// 	return no_os_gpio_set_value(gpio_led_desc, val);
// }
