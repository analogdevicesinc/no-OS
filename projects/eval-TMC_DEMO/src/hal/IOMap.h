/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef _IO_PIN_MAP_H_
#define _IO_PIN_MAP_H_

#include "IOs.h"

typedef struct
{
	void (*init) (void);

	IOPinTypeDef **pins; // Map Pin ID <=> Pin

	IOPinTypeDef ID_CLK;
	IOPinTypeDef ID_CH0;
	IOPinTypeDef ID_CH1;

	IOPinTypeDef DIO0;
	IOPinTypeDef DIO1;
	IOPinTypeDef DIO2;
	IOPinTypeDef DIO3;
	IOPinTypeDef DIO4;
	IOPinTypeDef DIO5;
	IOPinTypeDef DIO6;
	IOPinTypeDef DIO7;
	IOPinTypeDef DIO8;
	IOPinTypeDef DIO9;
	IOPinTypeDef DIO10;
	IOPinTypeDef DIO11;
	IOPinTypeDef CLK16;
	IOPinTypeDef SPI2_CSN0;
	IOPinTypeDef SPI2_CSN1;
	IOPinTypeDef SPI2_CSN2;
	IOPinTypeDef SPI2_SCK;
	IOPinTypeDef SPI2_SDO;
	IOPinTypeDef SPI2_SDI;

	IOPinTypeDef SPI1_CSN;
	IOPinTypeDef SPI1_SCK;
	IOPinTypeDef SPI1_SDI;
	IOPinTypeDef SPI1_SDO;

	IOPinTypeDef DIO12;
	IOPinTypeDef DIO13;
	IOPinTypeDef DIO14;
	IOPinTypeDef DIO15;
	IOPinTypeDef DIO16;
	IOPinTypeDef DIO17;
	IOPinTypeDef DIO18;
	IOPinTypeDef DIO19;

	IOPinTypeDef RS232_TX;
	IOPinTypeDef RS232_RX;

	IOPinTypeDef USB_V_BUS;
	IOPinTypeDef USB_V_DM;
	IOPinTypeDef USB_V_DP;

	IOPinTypeDef LED_STAT;
	IOPinTypeDef LED_ERROR;

	IOPinTypeDef EEPROM_SCK;
	IOPinTypeDef EEPROM_SI;
	IOPinTypeDef EEPROM_SO;
	IOPinTypeDef EEPROM_NCS;

#if defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
	IOPinTypeDef WIRELESS_TX;
	IOPinTypeDef WIRELESS_RX;
	IOPinTypeDef WIRELESS_NRST;
	IOPinTypeDef MIXED0;
	IOPinTypeDef MIXED1;
	IOPinTypeDef MIXED2;
	IOPinTypeDef MIXED3;
	IOPinTypeDef MIXED4;
	IOPinTypeDef MIXED5;
	IOPinTypeDef MIXED6;
	IOPinTypeDef ID_HW_0;
	IOPinTypeDef ID_HW_1;
	IOPinTypeDef ID_HW_2;
	IOPinTypeDef EXTIO_2;
	IOPinTypeDef EXTIO_3;
	IOPinTypeDef EXTIO_4;
	IOPinTypeDef EXTIO_5;
	IOPinTypeDef EXTIO_6;
	IOPinTypeDef EXTIO_7;
#endif

#if defined(LandungsbrueckeV3)
	IOPinTypeDef DIO10_PWM_WL;
	IOPinTypeDef DIO10_UART_TX;
	IOPinTypeDef DIO11_PWM_WH;
	IOPinTypeDef DIO11_UART_RX;
	IOPinTypeDef SW_UART_PWM;
	IOPinTypeDef EXT0;
	IOPinTypeDef EXT1;
	IOPinTypeDef EXT2;
	IOPinTypeDef EXT3;
	IOPinTypeDef EXT4;
	IOPinTypeDef ADC_VM;
	IOPinTypeDef AIN0;
	IOPinTypeDef AIN1;
	IOPinTypeDef AIN2;
	IOPinTypeDef AIN_EXT;
	IOPinTypeDef WIFI_EN;
	IOPinTypeDef WIFI_RST;
	IOPinTypeDef WIFI_TX;
	IOPinTypeDef WIFI_RX;
	IOPinTypeDef BUTTON;
#endif

	IOPinTypeDef DUMMY;
} IOPinMapTypeDef;

extern IOPinMapTypeDef IOMap;

#endif /* _IO_PIN_MAP_H_ */
