/*
**
** Copyright (C) 2024 Analog Devices, Inc.
** All Rights Reserved.
** This software is proprietary and confidential to Analog Devices, Inc. and its licensors.
**
*/

#include <MXQ_API.h>
#include <MXQ_Error.h>
#include <strings.h>
#include "mxc_delay.h"

__attribute__ ((noinline, optimize("-O0")))
void HOST_wait_usec(void)
{
    asm volatile("nop");
}


#if defined(USE_PROTOCOL_SPI) || defined(USE_PROTOCOL_I2C)

#include "gpio.h"
#include "i2c.h"
#include "tmr.h"


#define MAX32650 1
#define MAXQ1065 1
#define SWIOT1L 1
#if defined(MAXQ1065)
#define STARTUP_DELAY       180000 //us
#elif defined(MAXQ108x)
#define STARTUP_DELAY       480000//us
#endif
#define PWDN_DELAY         (10000)

#if defined(MAX32650)
#if defined(MAXQ1065)
#if defined(SWIOT1L)
#define MXC_GPIO_PORT_PWDN MXC_GPIO0
#define MXC_GPIO_PIN_PWDN MXC_GPIO_PIN_2
#define MXC_GPIO_PORT_RDY MXC_GPIO2
#define MXC_GPIO_PIN_RDY MXC_GPIO_PIN_16
#else
#define MXC_GPIO_PORT_PWDN MXC_GPIO0
#define MXC_GPIO_PIN_PWDN MXC_GPIO_PIN_30
#define MXC_GPIO_PORT_RDY MXC_GPIO0
#define MXC_GPIO_PIN_RDY MXC_GPIO_PIN_31
#endif //SWIOT1L

#elif defined(MAXQ108x)
#define MXC_GPIO_PORT_RST MXC_GPIO0
#define MXC_GPIO_PIN_RST MXC_GPIO_PIN_30
#define MXC_GPIO_PORT_RDY MXC_GPIO0
#define MXC_GPIO_PIN_RDY MXC_GPIO_PIN_31
#endif //MAXQ1065



#elif defined(MAX32670)

#if defined(MAXQ1065)
#define MXC_GPIO_PORT_PWDN MXC_GPIO0
#define MXC_GPIO_PIN_PWDN MXC_GPIO_PIN_21
#elif defined(MAXQ108x)
#define MXC_GPIO_PORT_RST MXC_GPIO0
#define MXC_GPIO_PIN_RST MXC_GPIO_PIN_21
#endif //MAXQ1065

#define MXC_GPIO_PORT_RDY MXC_GPIO0
#define MXC_GPIO_PIN_RDY MXC_GPIO_PIN_20

#elif defined(MAX32690)

#if defined(APARD)
#if defined(MAXQ1065)
#define MXC_GPIO_PORT_PWDN MXC_GPIO3
#define MXC_GPIO_PIN_PWDN MXC_GPIO_PIN_8
#elif defined(MAXQ108x)
#define MXC_GPIO_PORT_RST MXC_GPIO3
#define MXC_GPIO_PIN_RST MXC_GPIO_PIN_8
#endif

#else //APARD

#if defined(MAXQ1065)
#define MXC_GPIO_PORT_PWDN MXC_GPIO0
#define MXC_GPIO_PIN_PWDN MXC_GPIO_PIN_14
#elif defined(MAXQ108x)
#define MXC_GPIO_PORT_RST MXC_GPIO0
#define MXC_GPIO_PIN_RST MXC_GPIO_PIN_14
#endif
#define MXC_GPIO_PORT_RDY MXC_GPIO2
#define MXC_GPIO_PIN_RDY MXC_GPIO_PIN_11

#endif //#if defined (APARD)

#endif //#if defined(MAX32650)


#if defined (MAXQ1065)
mxc_gpio_cfg_t maxq1065_pwdn = { MXC_GPIO_PORT_PWDN,
        MXC_GPIO_PIN_PWDN,
        MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIOH,
		MXC_GPIO_DRVSTR_0};
#elif defined(MAXQ108x)
mxc_gpio_cfg_t maxq108x_reset = { MXC_GPIO_PORT_RST,
        MXC_GPIO_PIN_RST,
        MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIOH,
		MXC_GPIO_DRVSTR_0};

#endif



#ifndef READY_NOT_CONNECTED
mxc_gpio_cfg_t maxq_rdy = { MXC_GPIO_PORT_RDY,
        MXC_GPIO_PIN_RDY,
        MXC_GPIO_FUNC_IN, MXC_GPIO_PAD_NONE,
        MXC_GPIO_VSSEL_VDDIOH,
		MXC_GPIO_DRVSTR_0};
#endif // READY_NOT_CONNECTED

uint32_t HOST_block_until_pin(mxc_gpio_regs_t *port, uint32_t mask, uint32_t blocking_value, uint32_t waittime)
{
    uint32_t timeout = waittime;

    while((MXC_GPIO_InGet(port, mask) == blocking_value) && (timeout != 0)) {
        MXC_TMR_Delay(MXC_TMR0,100);
        timeout--;
    }
    if(timeout == 0){
        return MXQ_ERR_TRANS_TIMEOUT;
    }
    return MXQ_OK;
}
void HOST_maxq10xx_reset(void)
{
    static uint32_t reset_set = 0;
    mxc_gpio_cfg_t *maxq_reset_pin;

    #if defined(MAXQ1065)
    maxq_reset_pin = &maxq1065_pwdn;
    #elif defined(MAXQ108x)
    maxq_reset_pin = &maxq108x_reset;
    #endif

    if (!reset_set){
        MXC_GPIO_Config(maxq_reset_pin);
        MXC_GPIO_OutClr((*maxq_reset_pin).port, (*maxq_reset_pin).mask);
        for(uint32_t delay = 0; delay < PWDN_DELAY; delay++) {}
        MXC_GPIO_OutSet((*maxq_reset_pin).port, (*maxq_reset_pin).mask);
        reset_set = 1;
        MXC_Delay(STARTUP_DELAY);
    }
}

#endif // defined(USE_PROTOCOL_SPI) || defined(USE_PROTOCOL_I2C)


void* HOST_get_interface(void) {
	return (void*)0;
}

void HOST_set_interface(void* i) {
	(void) i;
}

mxq_err_t HOST_deinit_interface(void)
{
	return MXQ_OK;
}

/******************		Mode		*******************************************/


mxq_err_t HOST_reset_maxq106x(void) {

	return MXQ_OK;
}

mxq_err_t HOST_toggle_wdi(void)
{
    mxq_err_t retVal = MXQ_OK;
//    MXC_WDT_ResetTimer(MXC_WDT0);
    return(retVal);
}

mxq_err_t HOST_activate_tls(void)
{
    mxq_err_t retVal = MXQ_OK;

    //Set MODE pin to 0
    //Assert the RESET pin (0)
    //Hold RESET pin for 100uS
    //De-assert the RESET pin (1)

    return(retVal);
}

mxq_err_t HOST_activate_gcu(void)
{
    mxq_err_t retVal = MXQ_OK;
    
    //Set MODE pin to 1
    //Assert the RESET pin (0)
    //Hold RESET pin for 100uS
    //De-assert the RESET pin (1)

    return(retVal);
}

mxq_err_t HOST_follow_tls(void)
{
#if defined(MAXQ106x)
	GPIOWrite(PIN_MODE,LOW);
	usleep(TIME_WAIT_RESET);
#endif
	return MXQ_OK;
}

