/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef __UART_H_
#define __UART_H_

#include "RXTX.h"

// Switchable UART pin configuration due to pinout changes in TMC2208 v1.2 -> TMC2208 v1.3 aswell as TMC2209
typedef enum {
	UART_PINS_1, // Default UART pinout (<= TMC2208 v1.2, UART_TXD = DIO17, UART_RXD = DIO18)
	UART_PINS_2  // Alternate UART pinout (>= TMC2208 v1.3, UART_TXD = DIO10, UART_RXD = DIO11)
} UART_Pins;

typedef enum {
	UART_MODE_DUAL_WIRE,
	UART_MODE_SINGLE_WIRE,
	UART_MODE_DUAL_WIRE_PushPull, // Use PushPull for TX instead of OpenDrain
} UART_Mode;

typedef struct {
	UART_Pins pinout;
	UART_Mode mode;
	RXTXTypeDef rxtx;
} UART_Config;

extern UART_Config UART;

void UART0_RX_TX_IRQHandler_UART(void);
int32_t UART_readWrite(UART_Config *uart, uint8_t *data, size_t writeLength, uint8_t readLength);
void UART_readInt(UART_Config *channel, uint8_t slave, uint8_t address, int32_t *value);
void UART_writeInt(UART_Config *channel, uint8_t slave, uint8_t address, int32_t value);
void UART_setEnabled(UART_Config *channel, uint8_t enabled);

#endif /* __UART_H_ */
