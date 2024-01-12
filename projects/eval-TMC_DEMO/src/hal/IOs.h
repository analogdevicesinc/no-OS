/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef _IO_H_
#define _IO_H_

#include "derivative.h"

typedef enum { // Give bits explicitly, because IDE relies on it.
	IOS_LOW      = 0b00,
	IOS_HIGH     = 0b01,
	IOS_OPEN     = 0b10,
	IOS_NOCHANGE = 0b11
} IO_States;

#if defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
	// use ST like configuration structures also for Landungsbruecke

	typedef enum
	{
	  GPIO_Mode_AN   = 0x00,  /*!< GPIO Analog Mode, Pin disabled */
	  GPIO_Mode_AF1  = 0x01,  /*!< GPIO Alternate function Mode GPIO*/
	  GPIO_Mode_AF2  = 0x02,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_AF3  = 0x03,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_AF4  = 0x04,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_AF5  = 0x05,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_AF6  = 0x06,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_AF7  = 0x07,  /*!< GPIO Alternate function Mode*/
	  GPIO_Mode_IN   = 0x08,  /*!< GPIO Input Mode */
	  GPIO_Mode_OUT  = 0x09   /*!< GPIO Output Mode */
	} GPIOMode_TypeDef;

	typedef enum
	{
		GPIO_OType_PP = 0x00,
		GPIO_OType_OD = 0x01
	} GPIOOType_TypeDef;

	typedef enum
	{
		GPIO_PuPd_NOPULL  = 0x00,
		GPIO_PuPd_UP      = 0x01,
		GPIO_PuPd_DOWN    = 0x02
	} GPIOPuPd_TypeDef;

	typedef enum
	{
		GPIO_Speed_2MHz    = 0x00, /*!< Low speed */
		GPIO_Speed_25MHz   = 0x01, /*!< Medium speed */
		GPIO_Speed_50MHz   = 0x02, /*!< Fast speed */
		GPIO_Speed_100MHz  = 0x03  /*!< High speed on 30 pF (80 MHz Output max speed on 15 pF) */
	} GPIOSpeed_TypeDef;

	#include "hal/Landungsbruecke/freescale/PDD/GPIO_PDD.h"
#elif defined(LandungsbrueckeV3)
//	typedef enum
//		{
//		GPIO_MODE_INPUT   = 0x00,  /*!< GPIO Input Mode */
//		GPIO_MODE_OUTPUT  = 0x01,  /*!< GPIO Output Mode */
//		GPIO_MODE_AF  	  = 0x02,  /*!< GPIO Alternate function Mode*/
//		GPIO_MODE_ANALOG  = 0x03,  /*!< GPIO Analog Mode*/
//		} GPIOMode_TypeDef;
//
//		typedef enum
//		{
//			GPIO_OTYPE_PP = 0x00,
//			GPIO_OTYPE_OD = 0x01
//		} GPIOOType_TypeDef;
//
//		typedef enum
//		{
//			GPIO_PUPD_NONE  = 0x00,
//			GPIO_PUPD_PULLUP      = 0x01,
//			GPIO_PUPD_PULLDOWN    = 0x02
//		} GPIOPuPd_TypeDef;
//
//		typedef enum
//		{
//			GPIO_OSPEED_2MHZ    = 0x00, /*!< Low speed */
//			GPIO_OSPEED_25MHZ   = 0x01, /*!< Medium speed */
//			GPIO_OSPEED_50MHZ   = 0x02, /*!< Fast speed */
//			GPIO_OSPEED_MAX     = 0x03  /*!< GPIO very high output speed, max speed more than 50MHz */
//		} GPIOSpeed_TypeDef;
	typedef uint32_t GPIOMode_TypeDef;
	typedef uint32_t GPIOOType_TypeDef;
	typedef uint32_t GPIOPuPd_TypeDef;
	typedef uint32_t GPIOSpeed_TypeDef;


#endif


enum IOsHighLevelFunctions { IO_DEFAULT, IO_DI, IO_AI, IO_DO, IO_PWM, IO_SD, IO_CLK16, IO_SPI };

typedef struct
{
	const uint8_t DEFAULT;
	const uint8_t DI;
	const uint8_t AI;
	const uint8_t DO;
	const uint8_t PWM;
	const uint8_t SD;
	const uint8_t CLK16;
	const uint8_t SPI;
} IOsHighLevelFunctionTypeDef;

typedef struct
{
	GPIOMode_TypeDef   GPIO_Mode;
	GPIOSpeed_TypeDef  GPIO_Speed;
	GPIOOType_TypeDef  GPIO_OType;
	GPIOPuPd_TypeDef   GPIO_PuPd;
} IOPinInitTypeDef;

typedef struct
{
	#if defined(LandungsbrueckeV3)
		uint32_t				  port;
		volatile uint32_t         *setBitRegister;
		volatile uint32_t         *resetBitRegister;
	#elif defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
		PORT_MemMapPtr          portBase;
		GPIO_MemMapPtr          GPIOBase;
		volatile uint32_t         *setBitRegister;
		volatile uint32_t         *resetBitRegister;
	#endif
	uint32_t                      bitWeight;
	unsigned char               bit;
	IOPinInitTypeDef            configuration;
	IOPinInitTypeDef            resetConfiguration;
	enum IOsHighLevelFunctions  highLevelFunction;
	IO_States state;
} IOPinTypeDef;

typedef struct
{
	void (*set)(IOPinTypeDef *pin);
	void (*copy)(IOPinInitTypeDef *from, IOPinTypeDef*to);
	void (*reset)(IOPinTypeDef *pin);
	void (*toOutput)(IOPinTypeDef *pin);
	void (*toInput)(IOPinTypeDef *pin);

	void (*setHigh)(IOPinTypeDef *pin);
	void (*setLow)(IOPinTypeDef *pin);
	void (*setToState)(IOPinTypeDef *pin, IO_States state);
	IO_States (*getState)(IOPinTypeDef *pin);
	unsigned char (*isHigh)(IOPinTypeDef *pin);
	void (*init)(void);
	IOsHighLevelFunctionTypeDef HIGH_LEVEL_FUNCTIONS;
} IOsTypeDef;

extern IOsTypeDef IOs;

// A bit weight of 0 is used to indicate a nonexistent pin
#define DUMMY_BITWEIGHT 0
#define IS_DUMMY_PIN(pin) (pin->bitWeight == DUMMY_BITWEIGHT)

#endif /* _IO_H_ */
