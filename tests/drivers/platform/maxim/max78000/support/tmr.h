/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef TEST_TMR_H_
#define TEST_TMR_H_
#include <stdint.h>
#include <stdbool.h>
typedef struct {
	uint32_t count;
} mxc_tmr_regs_t;
extern mxc_tmr_regs_t test_timer;
#define MXC_TMR_GET_TMR(id) (&test_timer)
#define MXC_TMR_GET_IRQ(id) (8U + (id))
#define TMR_PRES_64 6
#define TMR_MODE_CONTINUOUS 1
#define TMR_BIT_MODE_32 0
#define MXC_TMR_ISO_CLK 2
typedef struct {
	uint32_t pres, mode, bitMode, clock, cmp_cnt;
} mxc_tmr_cfg_t;
int MXC_TMR_Init(mxc_tmr_regs_t *tmr, mxc_tmr_cfg_t *config, bool pins);
void MXC_TMR_SetCount(mxc_tmr_regs_t *tmr, uint32_t count);
uint32_t MXC_TMR_GetCount(mxc_tmr_regs_t *tmr);
uint32_t MXC_TMR_GetFlags(mxc_tmr_regs_t *tmr);
void MXC_TMR_ClearFlags(mxc_tmr_regs_t *tmr);
void MXC_TMR_EnableInt(mxc_tmr_regs_t *tmr);
void MXC_TMR_Start(mxc_tmr_regs_t *tmr);
#endif
