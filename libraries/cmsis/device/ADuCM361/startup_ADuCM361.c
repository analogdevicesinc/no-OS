/* File: startup_ADuCM361.c
 * Purpose: startup file for Cortex-M3 devices.
 *          Should be used with GCC 'GNU Tools ARM Embedded'
 * Version: V1.01
 * Date: 12 June 2014
 *
 */
/* Copyright (c) 2011 - 2014 ARM LIMITED

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

   Portions Copyright (c) 2017-2018 Analog Devices, Inc.
   ---------------------------------------------------------------------------*/

#include <stdint.h>

// ----------------------------------------------------------------------------
/*----------------------------------------------------------------------------
  Linker generated Symbols
 *----------------------------------------------------------------------------*/
extern uint32_t __etext;
extern uint32_t __data_start__;
extern uint32_t __data_end__;
extern uint32_t __copy_table_start__;
extern uint32_t __copy_table_end__;
extern uint32_t __zero_table_start__;
extern uint32_t __zero_table_end__;
extern uint32_t __bss_start__;
extern uint32_t __bss_end__;
extern uint32_t __StackTop;

extern unsigned int _sidata;
// Begin address for the .data section; defined in linker script
extern unsigned int _sdata;
// End address for the .data section; defined in linker script
extern unsigned int _edata;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler Function Prototype
 *----------------------------------------------------------------------------*/
typedef void( *pFunc )( void );

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
#ifndef __START
extern void  _start(void) __attribute__((noreturn));    /* PreeMain (C library entry point) */
#else
extern int  __START(void) __attribute__((noreturn));    /* main entry point */
#endif

#ifndef __NO_SYSTEM_INIT
extern void SystemInit (void);            /* CMSIS System Initialization      */
#endif


/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Default_Handler(void);                          /* Default empty handler */


/*----------------------------------------------------------------------------
  User Initial Stack & Heap
 *----------------------------------------------------------------------------*/
#ifndef __STACK_SIZE
  #define	__STACK_SIZE  0x00000400
#endif
static uint8_t stack[__STACK_SIZE] __attribute__ ((aligned(8), used, section(".stack")));

#ifndef __HEAP_SIZE
  #define	__HEAP_SIZE   0x00000C00
#endif
#if __HEAP_SIZE > 0
static uint8_t heap[__HEAP_SIZE]   __attribute__ ((aligned(8), used, section(".heap")));
#endif

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
/* Cortex-M3 Processor Exceptions */
void Reset_Handler               (void) __attribute__ ((weak));
void NMI_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void HardFault_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void MemManage_Handler           (void) __attribute__ ((weak, alias("Default_Handler")));
void BusFault_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void UsageFault_Handler          (void) __attribute__ ((weak, alias("Default_Handler")));
void SVC_Handler                 (void) __attribute__ ((weak, alias("Default_Handler")));
void DebugMon_Handler            (void) __attribute__ ((weak, alias("Default_Handler")));
void PendSV_Handler              (void) __attribute__ ((weak, alias("Default_Handler")));
void SysTick_Handler             (void) __attribute__ ((weak, alias("Default_Handler")));

// Forward declaration of the specific IRQ handlers. These are aliased
// to the Default_Handler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//

void WakeUp_Int_Handler          (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 0]
void Ext_Int0_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 1]
void Ext_Int1_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 2]
void Ext_Int2_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 3]
void Ext_Int3_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 4]
void Ext_Int4_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 5]
void Ext_Int5_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 6]
void Ext_Int6_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 7]
void Ext_Int7_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 8]
void WDog_Tmr_Int_Handler        (void) __attribute__ ((weak, alias ("Default_Handler"))); //[ 9]
void GP_Tmr0_Int_Handler         (void) __attribute__ ((weak, alias ("Default_Handler"))); //[11]
void GP_Tmr1_Int_Handler         (void) __attribute__ ((weak, alias ("Default_Handler"))); //[12]
void ADC0_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[13]
void ADC1_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[14]
void SINC2_Int_Handler           (void) __attribute__ ((weak, alias ("Default_Handler"))); //[15]
void Flsh_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[16]
void UART_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[17]
void SPI0_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[18]
void SPI1_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[19]
void I2C0_Slave_Int_Handler      (void) __attribute__ ((weak, alias ("Default_Handler"))); //[20]
void I2C0_Master_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[21]
void DMA_Err_Int_Handler         (void) __attribute__ ((weak, alias ("Default_Handler"))); //[22]
void DMA_SPI1_TX_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[23]
void DMA_SPI1_RX_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[24]
void DMA_UART_TX_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[25]
void DMA_UART_RX_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[26]
void DMA_I2C0_STX_Int_Handler    (void) __attribute__ ((weak, alias ("Default_Handler"))); //[27]
void DMA_I2C0_SRX_Int_Handler    (void) __attribute__ ((weak, alias ("Default_Handler"))); //[28]
void DMA_I2C0_MTX_Int_Handler    (void) __attribute__ ((weak, alias ("Default_Handler"))); //[29]
void DMA_I2C0_MRX_Int_Handler    (void) __attribute__ ((weak, alias ("Default_Handler"))); //[30]
void DMA_DAC_Out_Int_Handler     (void) __attribute__ ((weak, alias ("Default_Handler"))); //[31]
void DMA_ADC0_Int_Handler        (void) __attribute__ ((weak, alias ("Default_Handler"))); //[32]
void DMA_ADC1_Int_Handler        (void) __attribute__ ((weak, alias ("Default_Handler"))); //[33]
void DMA_SINC2_Int_Handler       (void) __attribute__ ((weak, alias ("Default_Handler"))); //[34]
void PWMTRIP_Int_Handler         (void) __attribute__ ((weak, alias ("Default_Handler"))); //[35]
void PWM0_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[36]
void PWM1_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[37]
void PWM2_Int_Handler            (void) __attribute__ ((weak, alias ("Default_Handler"))); //[38]

// ----------------------------------------------------------------------------

// The vector table.
// This relies on the linker script to place at correct location in memory.
const pFunc __Vectors[] __attribute__ ((section(".vectors"))) = {
   //
   (pFunc) &__StackTop,                          // The initial stack pointer
   Reset_Handler,                            // The reset handler

   NMI_Handler,                              // The NMI handler
   HardFault_Handler,                        // The hard fault handler

#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
   MemManage_Handler,                        // The MPU fault handler
   BusFault_Handler,// The bus fault handler
   UsageFault_Handler,// The usage fault handler
#else
   0, 0, 0,             // Reserved
#endif
   0,                                        // Reserved
   0,                                        // Reserved
   0,                                        // Reserved
   0,                                        // Reserved
   SVC_Handler,                              // SVCall handler
#if defined(__ARM_ARCH_7M__) || defined(__ARM_ARCH_7EM__)
   DebugMon_Handler,                         // Debug monitor handler
#else
   0,                // Reserved
#endif
   0,                                        // Reserved
   PendSV_Handler,                           // The PendSV handler
   SysTick_Handler,                          // The SysTick handler

   // ----------------------------------------------------------------------
   // ADuCM361 vectors
   WakeUp_Int_Handler,        //[ 0]
   Ext_Int0_Handler,          //[ 1]
   Ext_Int1_Handler,          //[ 2]
   Ext_Int2_Handler,          //[ 3]
   Ext_Int3_Handler,          //[ 4]
   Ext_Int4_Handler,          //[ 5]
   Ext_Int5_Handler,          //[ 6]
   Ext_Int6_Handler,          //[ 7]
   Ext_Int7_Handler,          //[ 8]
   WDog_Tmr_Int_Handler,      //[ 9]
   0,                         //[10]
   GP_Tmr0_Int_Handler,       //[11]
   GP_Tmr1_Int_Handler,       //[12]
   ADC0_Int_Handler,          //[13]
   ADC1_Int_Handler,          //[14]
   SINC2_Int_Handler,         //[15]
   Flsh_Int_Handler,          //[16]
   UART_Int_Handler,          //[17]
   SPI0_Int_Handler,          //[18]
   SPI1_Int_Handler,          //[19]
   I2C0_Slave_Int_Handler,    //[20]
   I2C0_Master_Int_Handler,   //[21]
   DMA_Err_Int_Handler,       //[22]
   DMA_SPI1_TX_Int_Handler,   //[23]
   DMA_SPI1_RX_Int_Handler,   //[24]
   DMA_UART_TX_Int_Handler,   //[25]
   DMA_UART_RX_Int_Handler,   //[26]
   DMA_I2C0_STX_Int_Handler,  //[27]
   DMA_I2C0_SRX_Int_Handler,  //[28]
   DMA_I2C0_MTX_Int_Handler,  //[29]
   DMA_I2C0_MRX_Int_Handler,  //[30]
   DMA_DAC_Out_Int_Handler,   //[31]
   DMA_ADC0_Int_Handler,      //[32]
   DMA_ADC1_Int_Handler,      //[33]
   DMA_SINC2_Int_Handler,     //[34]
   PWMTRIP_Int_Handler,       //[35]
   PWM0_Int_Handler,          //[36]
   PWM1_Int_Handler,          //[37]
   PWM2_Int_Handler,          //[38]
   0,                         //[39]
};

// ----------------------------------------------------------------------------

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) {
  uint32_t *pSrc, *pDest;
  uint32_t *pTable __attribute__((unused));

/*  Firstly it copies data from read only memory to RAM. There are two schemes
 *  to copy. One can copy more than one sections. Another can only copy
 *  one section.  The former scheme needs more instructions and read-only
 *  data to implement than the latter.
 *  Macro __STARTUP_COPY_MULTIPLE is used to choose between two schemes.  */

#ifdef __STARTUP_COPY_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of triplets, each of which specify:
 *    offset 0: LMA of start of a section to copy from
 *    offset 4: VMA of start of a section to copy to
 *    offset 8: size of the section to copy. Must be multiply of 4
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pTable = &__copy_table_start__;

  for (; pTable < &__copy_table_end__; pTable = pTable + 3) {
		pSrc  = (uint32_t*)*(pTable + 0);
		pDest = (uint32_t*)*(pTable + 1);
		for (; pDest < (uint32_t*)(*(pTable + 1) + *(pTable + 2)) ; ) {
      *pDest++ = *pSrc++;
		}
	}
#else
/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */
  pSrc  = &__etext;
  pDest = &__data_start__;

  for ( ; pDest < &__data_end__ ; ) {
    *pDest++ = *pSrc++;
  }
#endif /*__STARTUP_COPY_MULTIPLE */

/*  This part of work usually is done in C library startup code. Otherwise,
 *  define this macro to enable it in this startup.
 *
 *  There are two schemes too. One can clear multiple BSS sections. Another
 *  can only clear one section. The former is more size expensive than the
 *  latter.
 *
 *  Define macro __STARTUP_CLEAR_BSS_MULTIPLE to choose the former.
 *  Otherwise efine macro __STARTUP_CLEAR_BSS to choose the later.
 */
#ifdef __STARTUP_CLEAR_BSS_MULTIPLE
/*  Multiple sections scheme.
 *
 *  Between symbol address __copy_table_start__ and __copy_table_end__,
 *  there are array of tuples specifying:
 *    offset 0: Start of a BSS section
 *    offset 4: Size of this BSS section. Must be multiply of 4
 */
  pTable = &__zero_table_start__;

  for (; pTable < &__zero_table_end__; pTable = pTable + 2) {
		pDest = (uint32_t*)*(pTable + 0);
		for (; pDest < (uint32_t*)(*(pTable + 0) + *(pTable + 1)) ; ) {
      *pDest++ = 0;
		}
	}
#elif defined (__STARTUP_CLEAR_BSS)
/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */
  pDest = &__bss_start__;

  for ( ; pDest < &__bss_end__ ; ) {
    *pDest++ = 0ul;
  }
#endif /* __STARTUP_CLEAR_BSS_MULTIPLE || __STARTUP_CLEAR_BSS */

#ifndef __NO_SYSTEM_INIT
	SystemInit();
#endif

#ifndef __START
#define __START _start
#endif
	__START();

}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void) {

	while(1);
}


// ----------------------------------------------------------------------------
