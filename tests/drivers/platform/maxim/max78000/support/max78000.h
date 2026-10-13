/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef TEST_MAX78000_H_
#define TEST_MAX78000_H_
#include <stdint.h>
#define __weak __attribute__((weak))
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_ISO 0
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_INRO 3
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_IPO 4
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_IBRO 5
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_ERTCO 6
#define MXC_V_GCR_CLKCTRL_SYSCLK_SEL_EXTCLK 7
#define MXC_F_GCR_CLKCTRL_SYSCLK_SEL_POS 9
#define MXC_F_GCR_CLKCTRL_SYSCLK_SEL (7U << 9)
#define MXC_F_GCR_CLKCTRL_SYSCLK_DIV_POS 6
#define MXC_F_GCR_CLKCTRL_SYSCLK_DIV (7U << 6)
#define ISO_FREQ 60000000U
#define IPO_FREQ 100000000U
#define HIRC_FREQ IPO_FREQ
#define INRO_FREQ 8000U
#define IBRO_FREQ 7372800U
#define ERTCO_FREQ 32768U
#define EXTCLK_FREQ 12000000U
typedef struct {
	uint32_t clkctrl;
} test_gcr_t;
extern test_gcr_t test_gcr;
#define MXC_GCR (&test_gcr)
extern uint32_t SystemCoreClock;
void SystemCoreClockUpdate(void);
#endif
