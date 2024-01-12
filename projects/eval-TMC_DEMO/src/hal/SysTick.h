/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef SysTick_H
#define SysTick_H

#define CPU_FREQUENCY 108000000

	#include "tmc/helpers/API_Header.h"

	void systick_init();
	uint32_t systick_getTick();
	void wait(uint32_t delay);
	uint32_t timeSince(uint32_t tick);
	uint32_t timeDiff(uint32_t newTick, uint32_t oldTick);

#if defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
	uint32_t systick_getMicrosecondTick();
#endif

#endif /* SysTick_H */
