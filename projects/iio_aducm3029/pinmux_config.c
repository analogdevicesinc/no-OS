/*
 **
 ** Source file generated on December 18, 2020 at 17:18:55.
 **
 ** Copyright (C) 2011-2020 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in
 ** the Pin Multiplexing configuration editor. Changes to the Pin Multiplexing
 ** configuration should be made by changing the appropriate options rather
 ** than editing this file.
 **
 ** Selected Peripherals
 ** --------------------
 ** UART0 (Tx, Rx, UART_SOUT_EN)
 ** TIMER0 (TIMER0_OUT)
 ** TIMER1 (TIMER1_OUT)
 ** TIMER2 (TIMER2_OUT)
 ** ADC0_IN (ADC_CONVERT, ADC0_IN0, ADC0_IN1, ADC0_IN2, ADC0_IN3, ADC0_IN4, ADC0_IN5, ADC0_IN6, ADC0_IN7)
 **
 ** GPIO (unavailable)
 ** ------------------
 ** P0_10, P0_11, P0_12, P0_14, P1_11, P1_13, P2_01, P2_03, P2_04, P2_05, P2_06, P2_07,
 ** P2_08, P2_09, P2_10
 */

#include <sys/platform.h>
#include <stdint.h>

#define UART0_TX_PORTP0_MUX  ((uint32_t) ((uint32_t) 1<<20))
#define UART0_RX_PORTP0_MUX  ((uint32_t) ((uint32_t) 1<<22))
#define UART0_UART_SOUT_EN_PORTP0_MUX  ((uint32_t) ((uint32_t) 3<<24))
#define TIMER0_TIMER0_OUT_PORTP0_MUX  ((uint32_t) ((uint32_t) 1<<28))
#define TIMER1_TIMER1_OUT_PORTP1_MUX  ((uint32_t) ((uint32_t) 2<<22))
#define TIMER2_TIMER2_OUT_PORTP2_MUX  ((uint16_t) ((uint16_t) 2<<2))
#define ADC0_IN_ADC_CONVERT_PORTP1_MUX  ((uint32_t) ((uint32_t) 1<<26))
#define ADC0_IN_ADC0_IN0_PORTP2_MUX  ((uint16_t) ((uint16_t) 1<<6))
#define ADC0_IN_ADC0_IN1_PORTP2_MUX  ((uint16_t) ((uint16_t) 1<<8))
#define ADC0_IN_ADC0_IN2_PORTP2_MUX  ((uint16_t) ((uint16_t) 1<<10))
#define ADC0_IN_ADC0_IN3_PORTP2_MUX  ((uint16_t) ((uint16_t) 1<<12))
#define ADC0_IN_ADC0_IN4_PORTP2_MUX  ((uint16_t) ((uint16_t) 1<<14))
#define ADC0_IN_ADC0_IN5_PORTP2_MUX  ((uint32_t) ((uint32_t) 1<<16))
#define ADC0_IN_ADC0_IN6_PORTP2_MUX  ((uint32_t) ((uint32_t) 1<<18))
#define ADC0_IN_ADC0_IN7_PORTP2_MUX  ((uint32_t) ((uint32_t) 1<<20))

int32_t adi_initpinmux(void);

/*
 * Initialize the Port Control MUX Registers
 */
int32_t adi_initpinmux(void)
{
	/* PORTx_MUX registers */
	*((volatile uint32_t *)REG_GPIO0_CFG) = UART0_TX_PORTP0_MUX |
						UART0_RX_PORTP0_MUX
						| UART0_UART_SOUT_EN_PORTP0_MUX | TIMER0_TIMER0_OUT_PORTP0_MUX;
	*((volatile uint32_t *)REG_GPIO1_CFG) = TIMER1_TIMER1_OUT_PORTP1_MUX |
						ADC0_IN_ADC_CONVERT_PORTP1_MUX;
	*((volatile uint32_t *)REG_GPIO2_CFG) = TIMER2_TIMER2_OUT_PORTP2_MUX |
						ADC0_IN_ADC0_IN0_PORTP2_MUX
						| ADC0_IN_ADC0_IN1_PORTP2_MUX | ADC0_IN_ADC0_IN2_PORTP2_MUX |
						ADC0_IN_ADC0_IN3_PORTP2_MUX
						| ADC0_IN_ADC0_IN4_PORTP2_MUX | ADC0_IN_ADC0_IN5_PORTP2_MUX |
						ADC0_IN_ADC0_IN6_PORTP2_MUX
						| ADC0_IN_ADC0_IN7_PORTP2_MUX;

	return 0;
}

