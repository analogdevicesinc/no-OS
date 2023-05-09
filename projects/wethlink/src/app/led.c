#include <stdio.h>
#include "no_os_gpio.h"
#include "parameters.h"
#include "tmr.h"
#include "mxc_sys.h"
#include "led.h"

#if (TARGET_NUM == 32650)
struct no_os_gpio_desc *tx_det_red;
struct no_os_gpio_desc *tx_det_green;
struct no_os_gpio_desc *tx_lock;
struct no_os_gpio_desc *rx_lock;
struct no_os_gpio_desc *rx_det;

// Parameters for PWM output
#define PORT_PWM   MXC_GPIO3       //port
#define PIN_PWM    MXC_GPIO_PIN_4 //pin
#define FREQ       200             // (Hz)
#define DUTY_CYCLE 2              // (%)
#define PWM_TIMER  MXC_TMR0        // must change PWM_PORT and PWM_PIN if changed
void PWMTimer()
{
    // Declare variables
    mxc_tmr_cfg_t tmr; // to configure timer
    unsigned int periodTicks = PeripheralClock / FREQ;

    /*
    Steps for configuring a timer for PWM mode:
    1. Disable the timer
    2. Set the pre-scale value
    3. Set polarity, PWM parameters
    4. Configure the timer for PWM mode
    5. Enable Timer
    */

    MXC_TMR_Shutdown(PWM_TIMER);

    tmr.pres    = TMR_PRES_1;
    tmr.mode    = TMR_MODE_PWM;
    tmr.cmp_cnt = periodTicks;
    tmr.pol     = 1;

    MXC_TMR_Init(PWM_TIMER, &tmr);

    led_rx_det(0);

    MXC_TMR_Start(PWM_TIMER);

    printf("PWM started.\n\n");
}

int led_init(void)
{
	int ret;

	ret = no_os_gpio_get(&tx_lock, &led_tx_lock_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rx_lock, &led_rx_lock_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&tx_det_red, &led_tx_det_red_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&tx_det_green, &led_tx_det_green_gpio_ip);
	if (ret)
		return ret;

	ret = no_os_gpio_get(&rx_det, &led_rx_det_gpio_ip);
	if (ret)
		return ret;

	// Turn LED's off
	no_os_gpio_direction_output(tx_lock, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rx_lock, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(tx_det_red, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(tx_det_green, NO_OS_GPIO_LOW);
	no_os_gpio_direction_output(rx_det, NO_OS_GPIO_HIGH);

	PWMTimer();

	return 0;
}

void led_tx_lock(bool on)
{
	no_os_gpio_set_value(tx_lock, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_rx_lock(bool on)
{
	no_os_gpio_set_value(rx_lock, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_tx_det_green(bool on)
{
	no_os_gpio_set_value(tx_det_green, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_tx_det_red(bool on)
{
	no_os_gpio_set_value(tx_det_red, on ? NO_OS_GPIO_HIGH : NO_OS_GPIO_LOW);
}

void led_rx_det(int brightness)
{
	if (brightness > 100)
		brightness = 100;
	if (brightness < 0)
		brightness = 0;

	unsigned int periodTicks = PeripheralClock / FREQ;
	unsigned int dutyTicks   = periodTicks * (100 - brightness) / 100;
	MXC_TMR_SetPWM(PWM_TIMER, dutyTicks);
}

#endif
