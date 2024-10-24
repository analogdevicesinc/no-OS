/**************************************************************************//**
 * @file     system_ADuCM360.c
 * @brief    CMSIS Cortex-M3 Device Peripheral Access Layer Source File for
 *           Device ADuCM360
 * @version  V3.10
 * @date     23. November 2012
 *
 * @note
 *
 ******************************************************************************/
/* Copyright (c) 2012 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.

   Portions Copyright (c) 2017 Analog Devices, Inc.
   ---------------------------------------------------------------------------*/

#include <stdint.h>

#include "ADuCM360.h"

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define __HFOSC           (16000000UL)    /* Oscillator frequency             */
#define __XTAL               (32768UL)    /* 32kHz Frequency                  */

#define SYSTEM_CLOCK (__HFOSC)

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /*!< System Clock Frequency (Core Clock)*/
uint32_t uClk;                            /* Undivided System Clock Frequency (UCLK)   */

/* Frequency of the external clock source connected to P1.0 */
uint32_t SystemExtClock;


/*----------------------------------------------------------------------------
  Clock functions
 *----------------------------------------------------------------------------*/
/*!
 * Update the clock
 * 
 * @return  none
 *
 * @brief   Updates the variable SystemCoreClock. It must be called before using
 *          SystemCoreClock variable and whenever the core clock is changed 
 *          during program excution.       
*/
void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
   int iDiv;

   switch (pADI_CLKCTL->CLKCON0 & CLKCON0_CLKMUX_MSK ) {
   case CLKCON0_CLKMUX_HFOSC:
      uClk = __HFOSC >> (pADI_CLKCTL->CLKSYSDIV & CLKSYSDIV_DIV2EN_MSK);
      break;

   case CLKCON0_CLKMUX_LFOSC:
   case CLKCON0_CLKMUX_LFXTAL:
      uClk = __XTAL;
      break;

   case CLKCON0_CLKMUX_EXTCLK:
      uClk = SystemExtClock;
      break;

   default:
      break;
   }

   iDiv = (pADI_CLKCTL->CLKCON0 & CLKCON0_CD_MSK);             // Read CLKCON0 divider bits
   SystemCoreClock = uClk >> iDiv;
}

/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 */
void SystemInit (void)
{
   /* SetSystemExtClkFreq(Freq); // if an external clock is used SetSystemExtClkFreq must be called
                                 // before calling SystemCoreClockUpdate() */

   pADI_WDT->T3CON  = 0;                    /* disable watchdog */
   pADI_CLKCTL->CLKCON0   = 0x0;            /* 16MHz output of UCLK divide */
   pADI_CLKCTL->CLKSYSDIV = 0;              /* No divide of 16MHz system clock */
   pADI_CLKCTL->CLKCON1   = 0x0;            /* PWM = 16MHz, UART = 16MHz, SPI1 = 16MHz, SPI0=16MHz */
   pADI_CLKCTL->CLKDIS    = 0x0;            /* Enable clock to all peripherals */

   /* compute internal clocks */
   SystemCoreClockUpdate();
}

/**
 * @brief  Sets the system external clock frequency
 *
 * @param  ExtClkFreq   External clock frequency in Hz
 * @return none
 *
 * Sets the clock frequency of the source connected to P1.0 clock input source
 */
void SetSystemExtClkFreq (uint32_t ExtClkFreq)
{
   SystemExtClock = ExtClkFreq;
}

/**
 * @brief  Gets the system external clock frequency
 *
 * @return External Clock frequency
 *
 * Gets the clock frequency of the source connected to P1.0 clock input source
 */
uint32_t GetSystemExtClkFreq (void)
{
   return  SystemExtClock;
}
