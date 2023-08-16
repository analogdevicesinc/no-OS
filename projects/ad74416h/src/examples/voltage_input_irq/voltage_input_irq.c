/***************************************************************************//**
 *   @file   voltage_input_irq.c
 *   @brief  Voltage input with interrupt example code for ad74416h-pmdz project
 *   @author Raquel Grau (raquel.grau@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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
#include "voltage_input_irq.h"
#include "common_data.h"
#include "ad74416h.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"

static struct no_os_callback_desc ext_int_callback_desc = {
	.callback = adc_rdy_event_handler,
};

struct no_os_gpio_desc *trigger_gpio_desc;
struct no_os_irq_ctrl_desc *trigger_irq_desc;

struct ad74416h_desc *ad74416h_desc;

volatile uint8_t sendResultToUart = 0;
volatile int adc_value = 0;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/***************************************************************************//**
 * @brief Voltage input irq example main execution.
 *
 * @return ret - Result of the example execution. If working correctly, will
 *               execute continuously the while(1) loop and will not return.
*******************************************************************************/
int voltage_input_irq_example_main()
{
	//struct ad74416h_desc *ad74416h_desc;
	int ret;

	/**
	  * @brief Initialize the trigger GPIO and associated IRQ event
	  * @return 0 if success, negative error code otherwise
	  */
	ret = gpio_trigger_init();
	if (ret)
		goto error;
	

	ret = ad74416h_init(&ad74416h_desc, &ad74416h_ip);
	if (ret)
		goto error;

	pr_info("ad74416h successfully initialized!\r\n");

	//Configure Channel A as Voltage Input
	ret = ad74416h_set_channel_function(ad74416h_desc, 0, AD74416H_VOLTAGE_IN);
	if (ret) {
		pr_info("Error setting Channel 0 as voltage input\r\n");
		goto error_ad74416h;
	}

	//Configure ADC Conversions
	//Configure the ADC sample rate
	ret = ad74416h_set_adc_rate(ad74416h_desc, 0, AD74416H_20SPS_50_60HZ_REJECTION);
	if (ret)
	{
		pr_info("Error setting sampling rate to 20SPS\r\n");
		goto error_ad74416h;
	}

	//Enable ADC A
	ret = ad74416h_set_adc_channel_enable(ad74416h_desc, 0, 1);
	if (ret)
	{
		pr_info("Error enabling ADC A\r\n");
		goto error_ad74416h;
	}
	//Set ADC A to continuous conversion
	ret = ad74416h_set_adc_conv_seq(ad74416h_desc, AD74416H_START_CONT);
	if (ret)
	{
		pr_info("Error enabling continuous conversions in ADC A\r\n");
		goto error_ad74416h;
	}
	
	//The following functions needs to be in the appropriate place for the application (while loop, interrupt handler, etc.)
	while(1)
	{
		if(sendResultToUart == 1)
		{
		        pr_info("ADC Input value = %0x\r\n", adc_value);
			sendResultToUart = 0;
		}
	}

error_ad74416h:
	ad74416h_remove(ad74416h_desc);
	return 0;
error:
	pr_info("Error!\r\n");
	return 0;
}

int gpio_trigger_init()
{
    int ret;
    /*Configure GPIO as input */
    ret = no_os_gpio_get(&trigger_gpio_desc, &trigger_gpio_param);
    if (ret)
    {
	    pr_info("Error in gpio_get\r\n");
            return ret;
    }

    ret = no_os_gpio_direction_input(trigger_gpio_desc);
    if (ret)
    {
	    pr_info("Error in direction_input\r\n");
            return ret;
    }

    /*Init interrup controller for external interrupt*/
    ret = no_os_irq_ctrl_init(&trigger_irq_desc, &trigger_gpio_irq_params);
    if (ret)
    {
	    pr_info("Error in irq init\r\n");
	    return ret;
    }

    /*Register a callback function for external interrupt */
    ret = no_os_irq_register_callback(trigger_irq_desc, GPIO_IRQ_ID1, &ext_int_callback_desc);
    if (ret)
    {
	    pr_info("error in register callback\r\n");
	    return ret;
    }

    ret = no_os_irq_trigger_level_set(trigger_irq_desc, GPIO_IRQ_ID1, NO_OS_IRQ_EDGE_FALLING);
    if (ret)
    {
	    pr_info("Error in level set\r\n");
	    return ret;
    }

    /*Enable external interrupt*/
    ret = no_os_irq_enable(trigger_irq_desc, GPIO_IRQ_ID1);
    if (ret)
    {
	    pr_info("error in enable irq\r\n");
	    return ret;
    }

    return 0;
}

void adc_rdy_event_handler()
{
	int ret;
        ret = ad74416h_get_raw_adc_result(ad74416h_desc, 0, &adc_value);
        if (ret)
        {
                pr_info("Error getting raw adc result in ADC A\r\n");
        }
	sendResultToUart = 1;
}

