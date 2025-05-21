/***************************************************************************//**
 *   @file   common_data.c
 *   @brief  Defines common data to be used by ADE9153A example project
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "common_data.h"

struct max_uart_init_param max_uart_extra_ip = {
	.flow = MAX_UART_FLOW_DIS
};

struct max_gpio_init_param gpio_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_spi_init_param ade9153a_spi_extra_ip  = {
	.num_slaves = SPI_SLAVE_NUM,
	.polarity = SPI_SS_POL_LOW,
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct max_gpio_init_param ade9153a_gpio_extra_ip = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_uart_init_param uart_ip = {
	.device_id = UART_DEV_ID,
	.baud_rate = UART_BAUD,
	.size = NO_OS_UART_CS_8,
	.parity = NO_OS_UART_PAR_NO,
	.stop = NO_OS_UART_STOP_1_BIT,
	.platform_ops = &max_uart_ops,
	.extra = &max_uart_extra_ip,
};

// user led pin
struct no_os_gpio_init_param gpio_led1_ip = {
	.port = GPIO_LED_PORT,
	.number = GPIO_LED_PIN,
	.pull = NO_OS_PULL_UP,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// ss pin
struct no_os_gpio_init_param gpio_ss_ip = {
	.port = GPIO_SS_PORT,
	.number = GPIO_SS_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// sck pin
struct no_os_gpio_init_param gpio_sck_ip = {
	.port = GPIO_SCK_PORT,
	.number = GPIO_SCK_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// reset pin
struct no_os_gpio_init_param gpio_reset_ip = {
	.port = GPIO_RESET_PORT,
	.number = GPIO_RESET_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

// rdy pin
struct no_os_gpio_init_param ade9153a_gpio_rdy_ip = {
	.port = GPIO_RDY_PORT,
	.number = GPIO_RDY_PIN,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_extra,
};

struct no_os_spi_init_param ade9153a_spi_ip = {
	.device_id = SPI_DEVICE_ID,
	.max_speed_hz = SPI_BAUDRATE,
	.bit_order = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.mode = NO_OS_SPI_MODE_0,
	.platform_ops = &max_spi_ops,
	.chip_select = SPI_CS,
	.extra = &ade9153a_spi_extra_ip,
};

struct no_os_irq_init_param ade9153a_gpio_irq_ip = {
	.platform_ops = GPIO_IRQ_OPS,
	.irq_ctrl_id = GPIO_CTRL_IRQ_ID,
	.extra = GPIO_IRQ_EXTRA,
};

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

/**
 * @brief Read measurements
* @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int read_measurements(struct ade9153a_dev *dev)
{
	int ret;
	struct ade9153a_temperature_value temp_val;
	struct ade9153a_energy_values energy_vals;
	struct ade9153a_power_values power_vals;
	struct ade9153a_rms_values rms_vals;
	struct ade9153a_half_rms_values hrms_vals;
	struct ade9153a_pq_values pq_vals;
	float temperature_val;
	float active_energy, reactive_energy, apparent_energy;
	float active_power, reactive_power, apparent_power;
	float current_rms_val, voltage_rms_val;
	float current_h_rms_val, voltage_h_rms_val;
	float constant, power_factor_val, frequency_val, angle_ai_av_val;
	// status of freq select
	uint8_t status;
	// skip temp if timeout
	uint8_t skip_temp;


	ret = ade9153a_temp_val(dev, &temp_val);
	if (ret)
		if (ret == -ENODATA) {
			pr_err(" Timeout temp read \n");
			skip_temp = 1;
		} else {
			return ret;
		} else
		skip_temp = 0;

	if (!(skip_temp)) {
		// compute the deg C value using the trim values
		temperature_val = ((float)temp_val.offset_reg_val / (float)(1 << 5)) - (((
					  float)temp_val.temperature_reg_val * (float)temp_val.gain_reg_val)
				  / (float)(1 << 17));
		pr_info("Temperature: %.2f deg C \n", temperature_val);
	}
	// compute energy values
	ret = ade9153a_energy_vals(dev, &energy_vals);
	if (ret)
		return ret;

	active_energy = (float)energy_vals.active_energy_reg_val * ENERGY_CC / 1000;
	// value in mWhr

	reactive_energy = (float)energy_vals.fundamental_reactive_energy_reg_val *
			  ENERGY_CC / 1000;
	// value in mVARhr

	apparent_energy = (float)(energy_vals.apparent_energy_reg_val) * ENERGY_CC /
			  1000;
	// value in mVAhr

	pr_info("Active energy: %.2f Whr \n", active_energy / 1000);
	pr_info("Fundamental reactive energy: %.2f mVARhr \n", reactive_energy / 1000);
	pr_info("Apparent energy: %.2f mVAhr \n", apparent_energy / 1000);

	// compute power values
	ret = ade9153a_power_vals(dev, &power_vals);
	if (ret)
		return ret;

	active_power = (float)power_vals.active_power_reg_val * POWER_CC / 1000;
	// value in mW

	reactive_power = (float)power_vals.fundamental_reactive_power_reg_val *
			 POWER_CC / 1000;
	// value in mVAR

	apparent_power = (float)power_vals.apparent_power_reg_val * POWER_CC / 1000;
	// value in mVA

	pr_info("Active power: %.2f W \n", active_power / 1000);
	pr_info("Fundamental reactive power: %.2f VAR \n", reactive_power / 1000);
	pr_info("Apparent power: %.2f VA \n", apparent_power / 1000);

	//compute rms values
	ret = ade9153a_rms_vals(dev, &rms_vals);
	if (ret)
		return ret;

	current_rms_val = (float)(rms_vals.current_rms_reg_val) * RMS_CURENT_CC / 1000;
	// value in mA

	voltage_rms_val = (float)rms_vals.voltage_rms_reg_val * RMS_VOLTAGE_CC / 1000;
	// value in mV

	pr_info("I rms: %.2f mA \n", current_rms_val);
	pr_info("V rms: %.2f mV \n", voltage_rms_val);

	//compute half rms values
	ret = ade9153a_half_rms_vals(dev, &hrms_vals);
	if (ret)
		return ret;

	current_h_rms_val = (float)hrms_vals.current_h_rms_reg_val * RMS_CURENT_CC /
			    1000;
	// value in mA

	voltage_h_rms_val = (float)hrms_vals.voltage_h_rms_reg_val * RMS_VOLTAGE_CC /
			    1000;
	// value in mV

	pr_info("I half rms: %.2f mA \n", current_h_rms_val);
	pr_info("V half rms: %.2f mV \n", voltage_h_rms_val);

	//compute power quality values
	ret = ade9153a_power_quality_vals(dev, &pq_vals);
	if (ret)
		return ret;

	power_factor_val = (float)pq_vals.power_factor_reg_val / (float)PF_CC;

	frequency_val = (float)(FREQ_CC) / (float)(pq_vals.period_reg_val + 1);
	// frequency value Hz

	// multiplier constant for selected frequency
	ret = ade9153a_freq_s_status(dev, &status);
	if (ret)
		return ret;
	if (!status)
		constant = MULT_CC_50HZ;
	else
		constant = MULT_CC_60HZ;
	angle_ai_av_val = (float)pq_vals.angle_ai_av_reg_val * constant;
	// angle value in degrees

	pr_info("Power factor: %.2f \n", power_factor_val);
	pr_info("Frequency %.2f Hz \n", frequency_val);
	pr_info("Angle: %.2f deg \n", angle_ai_av_val);

	pr_info("\n");
	pr_info("\n");
	pr_info("\n");

	return 0;
}

/**
 * @brief Autocalibration read values
* @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int autocalibration_read_vals(struct ade9153a_dev *dev)
{
	int ret;
	float i_gain, v_gain, aicc, avcc;
	struct ade9153a_autocal_vals autocal_vals;

	pr_info("Auto Calibration running \n");

	ret = autocalibration_start(dev);
	if (ret)
		return ret;
	ret = ade9153a_read_autocal_vals(dev, &autocal_vals);
	if (ret)
		return ret;

	pr_info("AICC: %.2f \n", (float)autocal_vals.aicc_reg_val / (1 << 11));
	pr_info("AVCC: %.2f \n", (float)autocal_vals.avcc_reg_val / (1 << 11));

	aicc = (float)autocal_vals.aicc_reg_val / (float)(1 << 11);
	i_gain = aicc / (RMS_CURENT_CC * 1000);
	i_gain = (-i_gain - 1);
	i_gain = i_gain * (float)(1 << 27);

	avcc = (float)autocal_vals.avcc_reg_val / (float)(1 << 11);
	v_gain = (float)((avcc / ((float)RMS_VOLTAGE_CC * (float)1000)) - 1) *
		 (1 << 27);

	pr_info("AI cert: %d \n", autocal_vals.aicert_reg_val);
	pr_info("AV cert: %d \n", autocal_vals.avcert_reg_val);

	ret = ade9153a_stop(dev);
	if (ret)
		return ret;

	ret = ade9153a_write(dev, ADE9153A_REG_AIGAIN, (int32_t)i_gain);
	if (ret)
		return ret;
	ret = ade9153a_write(dev, ADE9153A_REG_AVGAIN, (uint32_t)v_gain);
	if (ret)
		return ret;

	ret = ade9153a_run(dev);
	if (ret)
		return ret;

	pr_info("AI_GAIN: %d \n", (int32_t)i_gain);
	pr_info("AV_GAIN: %d \n", (uint32_t)v_gain);
	pr_info("Auto Calibration stop \n");

	return 0;
}

/**
 * @brief Autocalibration start
 * @param dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int autocalibration_start(struct ade9153a_dev *dev)
{
	int ret;

	ret = ade9153a_start_autocal_ai(dev, NORMAL);
	if (ret) {
		pr_info("ERROR %d \n", ret);
		return ret;
	}

	pr_info("IA autocalibration started \n");

	// wait 2s so the current autocalibration is ready
	no_os_mdelay(20000);

	ret = ade9153a_write(dev, ADE9153A_REG_MS_ACAL_CFG, 0x00000000);
	if (ret)
		return ret;

	no_os_mdelay(200);

	ret = ade9153a_start_autocal_av(dev, NORMAL);
	if (ret) {
		pr_info("ERROR %d \n", ret);
		return ret;
	}

	pr_info("VA autocalibration started \n");

	// wait 5s so the voltage autocalibration is ready
	no_os_mdelay(50000);

	ret = ade9153a_write(dev, ADE9153A_REG_MS_ACAL_CFG, 0x00000000);
	if (ret)
		return ret;

	no_os_mdelay(100);

	return 0;
}
