/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


//#include "hal/HAL.h"
#include "hal/SysTick.h"
#include "timer.h"
#include "sys_platform.h"

volatile uint32_t systick = 0;
struct timer_instance t;

void SysTick_Handler(void)
{
	systick++;
//	printf("systick %d\n",systick);
}

void systick_init()
{
	timer_init(&t, CPU0_INST_PICTIMER_START_ADDR, CPU_FREQUENCY);
	timer_start(&t, SysTick_Handler, (void *) &t, 1, 1);
}

uint32_t systick_getTick()
{
	return systick;
}

uint32_t systick_getMicrosecondTick()
{
	return 0;
}

/* Systick values are in milliseconds, accessing the value is faster. As a result
 * we have a random invisible delay of less than a millisecond whenever we use
 * systicks. This can result in a situation where we access the systick just before it changes:
 *  -> Access at 0,99ms gives systick 0ms
 *  -> Access at 1.01ms gives systick 1ms
 *  -> systick difference of 1ms, even though only 0.02 ms passed
 * To prevent this, we generally apply a correction of -1 to any systick difference.
 * In wait() this is done by using '<=' instead of '<'
 * In timeDiff() the subtraction is carried out on the result.
 * That subtraction is prevented from underflowing to UINT32_MAX, returning 0 in
 * that case (Saturated subtraction).
 *
 */
void wait(uint32_t delay)	// wait for [delay] ms/systicks
{
	uint32_t startTick = systick;
	while((systick-startTick) <= delay) {}
}

uint32_t timeSince(uint32_t tick)	// time difference since the [tick] timestamp in ms/systicks
{
	return timeDiff(systick, tick);
}

uint32_t timeDiff(uint32_t newTick, uint32_t oldTick) // Time difference between newTick and oldTick timestamps
{
	uint32_t tickDiff = newTick - oldTick;

	// Prevent subtraction underflow - saturate to 0 instead
	if(tickDiff != 0)
		return tickDiff - 1;
	else
		return 0;
}
