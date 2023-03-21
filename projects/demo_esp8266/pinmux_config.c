/*
 **
 ** Source file generated on July 29, 2020 at 09:37:28.
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
 ** SPI0 (SCLK, MOSI, MISO, CS_0, CS_1, CS_2, CS_3, RDY)
 ** UART0 (Tx, Rx, UART_SOUT_EN)
 **
 ** GPIO (unavailable)
 ** ------------------
 ** P0_00, P0_01, P0_02, P0_03, P0_10, P0_11, P0_12, P1_10, P1_14, P2_08, P2_09
 */

#include <sys/platform.h>
#include <stdint.h>

#define SPI0_SCLK_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<0))
#define SPI0_MOSI_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<2))
#define SPI0_MISO_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<4))
#define SPI0_CS_0_PORTP0_MUX  ((uint16_t) ((uint16_t) 1<<6))
#define SPI0_CS_1_PORTP1_MUX  ((uint32_t) ((uint32_t) 1<<20))
#define SPI0_CS_2_PORTP2_MUX  ((uint32_t) ((uint32_t) 2<<16))
#define SPI0_CS_3_PORTP2_MUX  ((uint32_t) ((uint32_t) 2<<18))
#define SPI0_RDY_PORTP1_MUX  ((uint32_t) ((uint32_t) 2<<28))
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
						| SPI0_MISO_PORTP0_MUX | SPI0_CS_0_PORTP0_MUX | UART0_TX_PORTP0_MUX
						| UART0_RX_PORTP0_MUX | UART0_UART_SOUT_EN_PORTP0_MUX;
	*((volatile uint32_t *)REG_GPIO1_CFG) = SPI0_CS_1_PORTP1_MUX |
						SPI0_RDY_PORTP1_MUX;
	*((volatile uint32_t *)REG_GPIO2_CFG) = SPI0_CS_2_PORTP2_MUX |
						SPI0_CS_3_PORTP2_MUX;

	return 0;
}

