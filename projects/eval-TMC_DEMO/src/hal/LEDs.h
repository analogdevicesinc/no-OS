/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef LEDS_H_
#define LEDS_H_

#if defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
	#define LED_ON()            *HAL.IOs->pins->LED_STAT.resetBitRegister   = HAL.IOs->pins->LED_STAT.bitWeight
	#define LED_OFF()           *HAL.IOs->pins->LED_STAT.setBitRegister     = HAL.IOs->pins->LED_STAT.bitWeight
	#define LED_TOGGLE()        HAL.IOs->pins->LED_STAT.GPIOBase->PTOR      ^= GPIO_PTOR_PTTO(HAL.IOs->pins->LED_STAT.bitWeight)

	#define LED_ERROR_ON()      *HAL.IOs->pins->LED_ERROR.resetBitRegister  = HAL.IOs->pins->LED_ERROR.bitWeight
	#define LED_ERROR_OFF()     *HAL.IOs->pins->LED_ERROR.setBitRegister    = HAL.IOs->pins->LED_ERROR.bitWeight
	#define LED_ERROR_TOGGLE()  HAL.IOs->pins->LED_ERROR.GPIOBase->PTOR     ^= GPIO_PTOR_PTTO(HAL.IOs->pins->LED_ERROR.bitWeight)
#elif defined(LandungsbrueckeV3)
	#define LED_ON()            *HAL.IOs->pins->LED_STAT.resetBitRegister   = HAL.IOs->pins->LED_STAT.bitWeight
	#define LED_OFF()           *HAL.IOs->pins->LED_STAT.setBitRegister     = HAL.IOs->pins->LED_STAT.bitWeight
	#define LED_TOGGLE()         GPIO_TG(HAL.IOs->pins->LED_STAT.port)      = HAL.IOs->pins->LED_STAT.bitWeight

	#define LED_ERROR_ON()      *HAL.IOs->pins->LED_ERROR.resetBitRegister  = HAL.IOs->pins->LED_ERROR.bitWeight
	#define LED_ERROR_OFF()     *HAL.IOs->pins->LED_ERROR.setBitRegister    = HAL.IOs->pins->LED_ERROR.bitWeight
	#define LED_ERROR_TOGGLE()  GPIO_TG(HAL.IOs->pins->LED_ERROR.port)      = HAL.IOs->pins->LED_ERROR.bitWeight
#endif

	#include "IOs.h"

	typedef struct
	{
		void (*on)(void);
		void (*off)(void);
		void (*toggle)(void);
	} LEDTypeDef;

	typedef struct
	{
		void (*init)(void);
		LEDTypeDef stat;
		LEDTypeDef error;
	} LEDsTypeDef;

	extern LEDsTypeDef LEDs;

#endif /* LEDS_H_ */
