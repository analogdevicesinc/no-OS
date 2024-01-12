/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef ADC_H
#define ADC_H

#include <stdint.h>

#define N_O_ADC_CHANNELS 7

static volatile uint16_t ADCValue[N_O_ADC_CHANNELS];

typedef struct
{
	volatile uint16_t *AIN0;
	volatile uint16_t *AIN1;
	volatile uint16_t *AIN2;
	volatile uint16_t *DIO4;
	volatile uint16_t *DIO5;
	volatile uint16_t *VM;
	volatile uint16_t *AIN_EXT; // Only LB_V3
	void (*init)();
	void (*deInit)();
} ADCTypeDef;

extern ADCTypeDef ADCs;

#endif /* ADC_H */
