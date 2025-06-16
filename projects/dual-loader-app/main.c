#include "console.h"
#include "tmr.h"
#include "tmr_common.h"
#include "no_os_gpio.h"
#include "maxim_gpio.h"

#define CONSOLE_PORT    MXC_UART0
#define DELAY_TIMER	MXC_TMR0

// Set delay time based on compile-time flag
#if defined(BLINKING_FAST)
    #define DELAY_TIME_US	20000
#elif defined(BLINKING_SLOW)
    #define DELAY_TIME_US	800000
#else // BLINKING_NORMAL (default)
    #define DELAY_TIME_US	100000
#endif

bool led_state[2] = {0};
struct max_gpio_init_param xgpio = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_desc *led[3];

#if defined(TARGET_MAX32672)
// MAX32672 EvKit LED pins
struct no_os_gpio_init_param led0_cfg = {
	.port = 0,
	.number = 22,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led1_cfg = {
	.port = 0,
	.number = 23,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

#elif defined(TARGET_MAX32690)
// MAX32690 EvKit LED pins - need to verify these
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
#endif

extern void uart_loader(void);

int led_init(void)
{
	int ret;

	no_os_gpio_get(&led[0], &led0_cfg);
	no_os_gpio_direction_output(led[0], NO_OS_GPIO_LOW);
	no_os_gpio_get(&led[1], &led1_cfg);
	no_os_gpio_direction_output(led[1], NO_OS_GPIO_LOW);
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
    tmr.pres = TMR_PRES_8;
    tmr.mode = TMR_MODE_COMPARE;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_8M_CLK;
    tmr.cmp_cnt = DELAY_TIME_US;
    tmr.pol = 0;

    if (MXC_TMR_Init(DELAY_TIMER, &tmr, false) != E_NO_ERROR) {
        return -1;
    }
    led_init();
    console_prepare(CONSOLE_PORT);

    console_str_out("*********************************\n");
    console_str_out("*          Loader Test          *\n");
    console_str_out("*********************************\n");
    console_str_out("\n");
    console_str_out("Press any key to enter loader mode.\n");

    while(1) 
    {
    	// Start a timer to use for toggling the LED
    	MXC_TMR_TO_Start(DELAY_TIMER, DELAY_TIME_US);

    	// Wait for the timer to expire
    	while(MXC_TMR_TO_Check(DELAY_TIMER) != E_TIME_OUT)
    	{
    		// If any key is pressed, abort the timer and enter the loader.
    		if(console_char_available())
    		{
    			// Stop the timer
    			MXC_TMR_TO_Stop(DELAY_TIMER);

    			// Consume the character that was typed
    			console_read(&ch, 1, 0);

    			// Enter the loader.
    			uart_loader();

			    console_str_out("Loader terminated.\n");
   				console_str_out("Press any key to enter loader mode.\n");
    			break;
    		}
    	}
    	led_toggle(0);
	    led_toggle(1);
    }
}
