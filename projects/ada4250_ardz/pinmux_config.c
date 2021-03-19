/*
 **
 ** Source file generated on March 19, 2021 at 13:15:59.
 **
 ** Copyright (C) 2011-2021 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in
 ** the Pin Multiplexing configuration editor. Changes to the Pin Multiplexing
 ** configuration should be made by changing the appropriate options rather
 ** than editing this file.
 **
 ** Selected Peripherals
 ** --------------------
 ** SPI0 (SCLK, MOSI, MISO, CS_1)
 ** UART0 (Tx, Rx, UART_SOUT_EN)
 **
 ** GPIO (unavailable)
 ** ------------------
 ** P0_00, P0_01, P0_02, P0_10, P0_11, P0_12, P1_10
 */

#include <sys/platform.h>
#include <stdint.h>

#define SPI0_SCLK_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<0))
#define SPI0_MOSI_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<2))
#define SPI0_MISO_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<4))
#define SPI0_CS_1_PORTP1_MUX  ((uint32_t) ((uint32_t) 1<<20))
#define UART0_TX_PORTP0_MUX  ((uint32_t) ((uint32_t) 1<<20))
#define UART0_RX_PORTP0_MUX  ((uint32_t) ((uint32_t) 1<<22))
#define UART0_UART_SOUT_EN_PORTP0_MUX  ((uint32_t) ((uint32_t) 3<<24))

int32_t adi_initpinmux(void);

/*
 * Initialize the Port Control MUX Registers
 */
int32_t adi_initpinmux(void)
{
	/* PORTx_MUX registers */
	*((volatile uint32_t *)REG_GPIO0_CFG) = SPI0_SCLK_PORTP0_MUX |
						SPI0_MOSI_PORTP0_MUX
						| SPI0_MISO_PORTP0_MUX | UART0_TX_PORTP0_MUX | UART0_RX_PORTP0_MUX
						| UART0_UART_SOUT_EN_PORTP0_MUX;
	*((volatile uint32_t *)REG_GPIO1_CFG) = SPI0_CS_1_PORTP1_MUX;

	return 0;
}
