/*******************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/***** Includes *****/

#include "uartLoader.h"
#include "console.h"
#include "tmr.h"
#include "tmr_common.h"
#include "no_os_gpio.h"
#include "maxim_gpio.h"

/***** Definitions *****/

#define CONSOLE_PORT    MXC_UART0
#define DELAY_TIMER	MXC_TMR0
#define DELAY_TIME_US	100000

/***** Globals *****/
bool led_state[3] = {0};
struct max_gpio_init_param xgpio = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_desc *led[3];

struct no_os_gpio_init_param led0_cfg = {
	.port = 2,
	.number = 1,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led1_cfg = {
	.port = 0,
	.number = 11,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led2_cfg = {
	.port = 0,
	.number = 12,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

/***** Functions *****/

int led_init(void)
{
	int ret;

	no_os_gpio_get(&led[0], &led0_cfg);
	no_os_gpio_direction_output(led[0], NO_OS_GPIO_LOW);
	no_os_gpio_get(&led[1], &led1_cfg);
	no_os_gpio_direction_output(led[0], NO_OS_GPIO_LOW);
	no_os_gpio_get(&led[2], &led2_cfg);
	no_os_gpio_direction_output(led[0], NO_OS_GPIO_LOW);
}

int led_toggle(int num)
{
	no_os_gpio_set_value(led[num], !led_state[num]);
	led_state[num] = !led_state[num];
}

int main(void)
{
    uint8_t ch;
    mxc_tmr_cfg_t tmr;
    MXC_TMR_Shutdown(DELAY_TIMER);
    tmr.pres = TMR_PRES_32;
    tmr.mode = TMR_MODE_COUNTER;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_EXT_CLK; /**< 32MHz Clock */
    tmr.cmp_cnt = 0xFFFFFFFF; //SystemCoreClock*(1/interval_time);
    tmr.pol = 0;               // Rising edge

    if (MXC_TMR_Init(DELAY_TIMER, &tmr, false) != E_NO_ERROR) {
        return -1;
    }

    consolePrepare(CONSOLE_PORT);

    consoleStrOut("*********************************\n");
    consoleStrOut("*          Loader Test          *\n");
    consoleStrOut("*********************************\n");
    consoleStrOut("\n");
    consoleStrOut("Press any key to enter loader mode.\n");

    while(1) 
    {
    	// Start a timer to use for toggling the LED
    	MXC_TMR_TO_Start(DELAY_TIMER, DELAY_TIME_US);

    	// Wait for the timer to expire
    	while(MXC_TMR_TO_Check(DELAY_TIMER) != E_TIME_OUT)
    	{
    		// If any key is pressed, abort the timer and enter the loader.
    		if(consoleCharAvailable())
    		{
    			// Stop the timer
    			MXC_TMR_TO_Stop(DELAY_TIMER);

    			// Consume the character that was typed
    			consoleRead(&ch, 1, 0);

    			// Enter the loader.
    			uartLoader();

			    consoleStrOut("Loader terminated.\n");
   				consoleStrOut("Press any key to enter loader mode.\n");
    			break;
    		}
    	}
    	led_toggle(0);
	led_toggle(1);
	led_toggle(2);
    }
}
