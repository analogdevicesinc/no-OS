/*******************************************************************************
 *   @file   maxim_delay.c
 *   @brief  Implementation of maxim delay functions.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "no_os_delay.h"
#include "no_os_util.h"
#include "mxc_delay.h"
#include "mxc_sys.h"

static volatile unsigned long long _system_ticks = 0;
/* **** File Scope Variables **** */
static volatile int overflows = -1;
static uint32_t endtick;
static uint32_t ctrl_save;
static mxc_delay_complete_t cbFunc;

static void MXC_DelayInit(unsigned long us);

extern void SysTick_Handler(void);

/* ************************************************************************** */
void SysTick_Handler(void)
{
	MXC_DelayHandler();
	_system_ticks++;
}

/**
 * @brief Generate microseconds delay.
 * @param usecs - Delay in microseconds.
 */
void no_os_udelay(uint32_t usecs)
{
	MXC_Delay(MXC_DELAY_USEC(usecs));
}

/**
 * @brief Generate miliseconds delay.
 * @param msecs - Delay in miliseconds.
 */
void no_os_mdelay(uint32_t msecs)
{
	MXC_Delay(MXC_DELAY_MSEC(msecs));
}

/**
 * @brief Get current time.
 * @return Current time structure from system start (seconds, microseconds).
 */
struct no_os_time no_os_get_time(void)
{
	struct no_os_time t;
	uint64_t sub_ms;
	uint32_t systick_val;
	uint64_t ticks;

	SysTick->CTRL &= ~(SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk);
	systick_val = SysTick->VAL;
	ticks = _system_ticks;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

	sub_ms = ((SysTick->LOAD - systick_val) * 1000) / SysTick->LOAD;
	t.s = ticks / 1000;
	t.us = (ticks - t.s * 1000) * 1000 + sub_ms;

	return t;
}

/* ************************************************************************** */
void MXC_DelayHandler(void)
{
    // Check and clear overflow flag
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        // Decrement overflow flag if delay is still ongoing
        if (overflows > 0) {
            overflows--;
        }
        else {
            MXC_DelayAbort();
            
            if (cbFunc != NULL) {
                cbFunc(E_NO_ERROR);
                cbFunc = NULL;
            }
        }
    }
}


/* ************************************************************************** */
static void MXC_DelayInit(unsigned long us)
{
    uint32_t starttick, reload, ticks, lastticks;
    
    // Record the current tick value and clear the overflow flag
    starttick = SysTick->VAL;
    
    // Save the state of control register (and clear the overflow flag)
    ctrl_save = SysTick->CTRL & ~SysTick_CTRL_COUNTFLAG_Msk;
    
    // If the SysTick is not running, configure and start it
    if (!(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk)) {
        SysTick->LOAD = SysTick_LOAD_RELOAD_Msk;
        SysTick->VAL = SysTick_VAL_CURRENT_Msk;
        SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk;
        starttick = SysTick_VAL_CURRENT_Msk;
        reload = SysTick_LOAD_RELOAD_Msk + 1;
    }
    else {
        reload = SysTick->LOAD + 1; // get the current reload value
    }
    
    // Calculate the total number of ticks to delay
    ticks = (uint32_t)(((uint64_t) us * (uint64_t) SystemCoreClock) / 1000000);
    
    // How many overflows of the SysTick will occur
    overflows = ticks / reload;
    
    // How many remaining ticks after the last overflow
    lastticks = ticks % reload;
    
    // Check if there will be another overflow due to the current value of the SysTick
    if (lastticks >= starttick) {
        overflows++;
        endtick = reload - (lastticks - starttick);
    }
    else {
        endtick = starttick - lastticks;
    }
}

/* ************************************************************************** */
int MXC_DelayAsync(unsigned long us, mxc_delay_complete_t callback)
{
    cbFunc = callback;
    
    // Check if timeout currently ongoing
    if (overflows > 0) {
        return E_BUSY;
    }
    
    // Check if there is nothing to do
    if (us == 0) {
        return E_NO_ERROR;
    }
    
    // Calculate the necessary delay and start the timer
    MXC_DelayInit(us);
    
    // Enable SysTick interrupt if necessary
    if (overflows > 0) {
        SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    }
    
    return E_NO_ERROR;
}

/* ************************************************************************** */
int MXC_DelayCheck(void)
{
    // Check if timeout currently ongoing
    if (overflows < 0) {
        if (cbFunc != NULL) {
            cbFunc(E_NO_ERROR);
            cbFunc = NULL;
        }
        
        return E_NO_ERROR;
    }
    
    // Check the global values
    if ((overflows == 0) && (SysTick->VAL <= endtick)) {
        MXC_DelayAbort();
        
        if (cbFunc != NULL) {
            cbFunc(E_NO_ERROR);
            cbFunc = NULL;
        }
        
        return E_NO_ERROR;
    }
    
    return E_BUSY;
}

/* ************************************************************************** */
void MXC_DelayAbort(void)
{
    if (cbFunc != NULL) {
        cbFunc(E_ABORT);
        cbFunc = NULL;
    }
    
    SysTick->CTRL = ctrl_save;
    overflows = -1;
}

/* ************************************************************************** */
int MXC_Delay(unsigned long us)
{
    // Check if timeout currently ongoing
    if (overflows > 0) {
        return E_BUSY;
    }
    
    // Check if there is nothing to do
    if (us == 0) {
        return E_NO_ERROR;
    }
    
    // Calculate the necessary delay and start the timer
    MXC_DelayInit(us);
    
    // Wait for the number of overflows
    while (overflows > 0) {
        // If SysTick interrupts are enabled, COUNTFLAG will never be set here and
        // overflows will be decremented in the ISR. If SysTick interrupts are
        // disabled, overflows is decremented here.
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
            overflows--;
        }
    }
    
    // Wait for the counter value
    while (SysTick->VAL > endtick);
    
    MXC_DelayAbort();
    return E_NO_ERROR;
}

