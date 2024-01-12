/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef _HAL_H_
#define _HAL_H_

#include "derivative.h"
#include "IOs.h"
#include "IOMap.h"
#include "SPI.h"
#include "ADCs.h"
#include "USB.h"
#include "LEDs.h"
#include "RS232.h"
#include "WLAN.h"
#include "Timer.h"
#include "SysTick.h"
#include "UART.h"

typedef struct
{
	IOsTypeDef       *config;
	IOPinMapTypeDef  *pins;
} IOsFunctionsTypeDef;

typedef struct
{
	void                       (*init) (void);
	void                       (*reset) (uint8_t ResetPeripherals);
	void                       (*NVIC_DeInit)(void);
	const IOsFunctionsTypeDef  *IOs;
	SPITypeDef                 *SPI;
	RXTXTypeDef                *USB;
	LEDsTypeDef                *LEDs;
	ADCTypeDef                 *ADCs;
	RXTXTypeDef                *RS232;
	RXTXTypeDef                *WLAN;
	TimerTypeDef               *Timer;
	UART_Config                *UART;
} HALTypeDef;

extern const HALTypeDef HAL;

extern uint8_t hwid;

#endif /* _HAL_H_ */
