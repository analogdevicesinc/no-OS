/*******************************************************************************
 * Copyright (C) 2019 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 ******************************************************************************/

/***** Includes *****/

#include "uartLoader.h"
#include "console.h"
#include "tmr.h"
#include "tmr_common.h"
#include "no_os_gpio.h"
#include "maxim_gpio.h"

/***** Definitions *****/

#define CONSOLE_PORT    MXC_UART0
#define DELAY_TIMER	MXC_TMR0

// Set delay time based on compile-time flag
#if defined(BLINKING_FAST)
    #define DELAY_TIME_US	20000
#elif defined(BLINKING_SLOW)
    #define DELAY_TIME_US	800000
#else // BLINKING_NORMAL (default)
    #define DELAY_TIME_US	100000
#endif

/***** Globals *****/
bool led_state[3] = {0};
struct max_gpio_init_param xgpio = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

struct no_os_gpio_desc *led[3];

struct no_os_gpio_init_param led0_cfg = {
	.port = 0,
	.number = 22,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led1_cfg = {
	.port = 0,
	.number = 23,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

struct no_os_gpio_init_param led2_cfg = {
	.port = 0,
	.number = 12,
	.platform_ops = &max_gpio_ops,
	.extra = &xgpio,
};

/***** Functions *****/

int led_init(void)
{
	int ret;

	no_os_gpio_get(&led[0], &led0_cfg);
	no_os_gpio_direction_output(led[0], NO_OS_GPIO_LOW);
	no_os_gpio_get(&led[1], &led1_cfg);
	no_os_gpio_direction_output(led[1], NO_OS_GPIO_LOW);
	no_os_gpio_get(&led[2], &led2_cfg);
	no_os_gpio_direction_output(led[2], NO_OS_GPIO_LOW);
}

int led_toggle(int num)
{
	no_os_gpio_set_value(led[num], !led_state[num]);
	led_state[num] = !led_state[num];
}

void enable_usage_fault_handler(void) {
    // Enable UsageFault in the System Handler Control and State Register (SHCSR)
    // SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk; // If you have CMSIS macros
    // Or, hardcoded:
    SCB->SHCSR |= (1UL << 18); // Set bit 18 (USGFAULTENA)

    // Ensure the change takes effect immediately
    __DSB();
    __ISB();
}

int main(void)
{
    uint8_t ch;
    mxc_tmr_cfg_t tmr;

enable_usage_fault_handler();

    MXC_TMR_Shutdown(DELAY_TIMER);
    tmr.pres = TMR_PRES_8;
    tmr.mode = TMR_MODE_COMPARE;
    tmr.bitMode = TMR_BIT_MODE_32;
    tmr.clock = MXC_TMR_8M_CLK; /**< 32MHz Clock */
    tmr.cmp_cnt = DELAY_TIME_US; //SystemCoreClock*(1/interval_time);
    tmr.pol = 0;               // Rising edge

    if (MXC_TMR_Init(DELAY_TIMER, &tmr, false) != E_NO_ERROR) {
        return -1;
    }
    led_init();
    consolePrepare(CONSOLE_PORT);

    consoleStrOut("*********************************\n");
    consoleStrOut("*          Loader Test          *\n");
    consoleStrOut("*********************************\n");
    consoleStrOut("\n");
    consoleStrOut("Press any key to enter loader mode.\n");

    while(1) 
    {
    	// Start a timer to use for toggling the LED
    	MXC_TMR_TO_Start(DELAY_TIMER, DELAY_TIME_US);

    	// Wait for the timer to expire
    	while(MXC_TMR_TO_Check(DELAY_TIMER) != E_TIME_OUT)
    	{
    		// If any key is pressed, abort the timer and enter the loader.
    		if(consoleCharAvailable())
    		{
    			// Stop the timer
    			MXC_TMR_TO_Stop(DELAY_TIMER);

    			// Consume the character that was typed
    			consoleRead(&ch, 1, 0);

    			// Enter the loader.
    			uartLoader();

			    consoleStrOut("Loader terminated.\n");
   				consoleStrOut("Press any key to enter loader mode.\n");
    			break;
    		}
    	}
    	led_toggle(0);
	led_toggle(1);
	led_toggle(2);
    }
}

// Structure representing the exception stack frame
// These are the registers automatically pushed by the CPU on exception entry.
typedef struct {
    uint32_t r0;
    uint32_t r1;
    uint32_t r2;
    uint32_t r3;
    uint32_t r12;
    uint32_t lr;  // Link Register (EXC_RETURN value)
    uint32_t pc;  // Program Counter (faulting instruction address)
    uint32_t psr; // Program Status Register
} StackFrame;

void HardFault_Handler(void) {
    StackFrame *stack_frame; // Pointer to the stacked registers
    volatile uint32_t _CFSR;
    volatile uint32_t _HFSR;
    volatile uint32_t _MMFAR;
    volatile uint32_t _BFAR;
    volatile uint32_t _AFSR;

    // Use inline assembly to get the correct stack pointer (MSP or PSP)
    // and store it into the 'stack_frame' pointer.
    __asm volatile (
        "TST LR, #4\n"        // Test EXC_RETURN[2] (bit 2 of LR, which was pushed)
                              // 0 if MSP was used (privileged access)
                              // 1 if PSP was used (unprivileged access)
        "ITE EQ\n"            // If equal (MSP used)
        "MRSEQ R0, MSP\n"     // Then move MSP to R0
        "MRSNE R0, PSP\n"     // Else (PSP used) move PSP to R0
        "MOV %0, R0\n"        // Move R0 (the determined stack pointer) to stack_frame
        : "=r" (stack_frame)  // Output: stack_frame will receive the stack pointer
        :                     // No direct C inputs for this assembly
        : "r0"                // Clobbered: R0 is modified by the assembly
    );

    // Now, you can access the stacked registers directly through the stack_frame pointer
    // Declare them volatile to ensure they are visible in the debugger
    volatile uint32_t fault_r0  = stack_frame->r0;
    volatile uint32_t fault_r1  = stack_frame->r1;
    volatile uint32_t fault_r2  = stack_frame->r2;
    volatile uint32_t fault_r3  = stack_frame->r3;
    volatile uint32_t fault_r12 = stack_frame->r12;
    volatile uint32_t fault_lr  = stack_frame->lr;
    volatile uint32_t fault_pc  = stack_frame->pc;  // This is the crucial faulting PC!
    volatile uint32_t fault_psr = stack_frame->psr;

    // Read fault status registers directly
    _HFSR  = SCB->HFSR;
    _CFSR  = SCB->CFSR;
    _MMFAR = SCB->MMFAR;
    _BFAR  = SCB->BFAR;
    _AFSR  = SCB->AFSR; // This register is key for TrustZone fault info on MAX32690

    // Set a hardware breakpoint (BKPT) here. Your debugger should stop here.
    // You can inspect all the 'fault_rX', 'fault_pc', 'fault_lr', and the fault registers
    // (_HFSR, _CFSR, _AFSR, _BFAR, _MMFAR) from your debugger's Locals/Watch window.
    __asm volatile ("BKPT #0");

    // Loop indefinitely to hold the CPU here after the breakpoint
    while (1) {
        // Add debug output (e.g., via UART) here if not using a debugger
    }
}


void MemManage_Handler(void)
{
     while(1);
}

void BusFault_Handler(void)
{
     while(1);
}

void UsageFault_Handler(void)
{
    while(1);
}