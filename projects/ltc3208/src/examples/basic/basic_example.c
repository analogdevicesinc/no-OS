/*******************************************************************************
 *   @file   basic_example.c
 *   @brief  Basic example code for LTC3208 project
 *   @author Edelweise Escala (edelweise.escala@analog.com)
 ********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "basic_example.h"
#include "ltc3208.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"

/*****************************************************************************
 * @brief Basic example RGB LED test.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int test_rgb(struct ltc3208_dev *dev)
{
	int ret;

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 8, 0);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 0, 8);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 0, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(dev, AUX_BLUE_REG, 0, 8);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 8, 8);
	if (ret)
		return ret;

	return 0;
}

/*****************************************************************************
 * @brief Basic example AUX LED test.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int test_aux(struct ltc3208_dev *dev)
{
	int ret;

	ret = ltc3208_set_4_bit_dac(dev, AUX_BLUE_REG, 8, 8);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_all_aux_led_dac(dev, MAIN, MAIN, MAIN, MAIN);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_all_aux_led_dac(dev, SUB, SUB, SUB, SUB);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_all_aux_led_dac(dev, CAM, CAM, CAM, CAM);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_aux_led_dac(dev, MAIN, 2);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_aux_led_dac(dev, SUB, 1);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_aux_led_dac(dev, AUX, 3);
	if (ret)
		return ret;

	return 0;
}

/*****************************************************************************
 * @brief Basic example CAM LED test.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int test_cam(struct ltc3208_dev *dev)
{
	int ret;

	ret = ltc3208_set_4_bit_dac(dev, CAM_REG, 14, 1);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_cam_high(dev, true);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_cam_high(dev, false);
	if (ret)
		return ret;

	no_os_mdelay(1000);


	return 0;
}

/*****************************************************************************
 * @brief Basic example ENRGBS pin test.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int test_enrgbs(struct ltc3208_dev *dev)
{
	int ret;

	ret = ltc3208_set_sub_enable(dev, false);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_sub_enable(dev, true);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_sub_enable(dev, false);
	if (ret)
		return ret;

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_LOW);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = no_os_gpio_set_value(dev->gpio_enrgbs_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	return 0;
}

/*****************************************************************************
 * @brief Basic example Dropout test.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int test_dropout(struct ltc3208_dev *dev)
{
	int ret;

	ret = ltc3208_set_8_bit_dac(dev, SUB_REG, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(dev, CAM_REG, 0, 0);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(dev, AUX_BLUE_REG, 8, 15);
	if (ret)
		return ret;

	ret = ltc3208_set_droupout_disable(dev, true);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 8, 13);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, GREEN_RED_REG, 8, 9);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_4_bit_dac(dev, AUX_BLUE_REG, 8, 13);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	ret = ltc3208_set_all_aux_led_dac(dev, AUX, AUX, AUX, AUX);
	if (ret)
		return ret;

	ret = ltc3208_set_4_bit_dac(dev, AUX_BLUE_REG, 8, 9);
	if (ret)
		return ret;

	no_os_mdelay(1000);

	return 0;
}

/*****************************************************************************
 * @brief Basic example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
 *******************************************************************************/
int example_main()
{
	int ret;
	struct ltc3208_dev *dev;

	ret = ltc3208_init(&dev, ltc3208_user_init);
	if (ret)
		goto error;

	while (1) {
		ret = ltc3208_reset(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = ltc3208_set_8_bit_dac(dev, MAIN_REG, 40);
		if (ret)
			goto free_dev;

		no_os_mdelay(500);

		ret = ltc3208_set_8_bit_dac(dev, SUB_REG, 20);
		if (ret)
			goto free_dev;

		no_os_mdelay(500);

		ret = test_cam(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = test_rgb(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = test_aux(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = test_enrgbs(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);

		ret = test_dropout(dev);
		if (ret)
			goto free_dev;

		no_os_mdelay(1000);
	}

free_dev:
	ltc3208_remove(dev);
error:
	pr_info("Error!\r\n");
	return 0;
}
