/***************************************************************************//**
 *   @file   ad9545.h
 *   @brief  Header file for ad9545 Driver.
 *   @author Jonathan Santos (Jonathan.Santos@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef AD9545_H_
#define AD9545_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_clk.h"
#include "no_os_gpio.h"
#include "no_os_i2c.h"
#include "no_os_spi.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* Input Driver Mode */
#define DRIVER_MODE_AC_COUPLED_IF	0
#define DRIVER_MODE_DC_COUPLED_1V2	1
#define DRIVER_MODE_DC_COUPLED_1V8	2
#define DRIVER_MODE_IN_PULL_UP		3

/* Input Driver Mode */
#define DRIVER_MODE_AC_COUPLED		0
#define DRIVER_MODE_DC_COUPLED		1
#define DRIVER_MODE_DC_COUPLED_LVDS	2

/* Output Driver Mode */
#define DRIVER_MODE_SINGLE_DIV_DIF	0
#define DRIVER_MODE_SINGLE_DIV		1
#define DRIVER_MODE_DUAL_DIV		2

/* Clock types */
#define AD9545_CLK_OUT			0
#define AD9545_CLK_PLL			1
#define AD9545_CLK_NCO			2
#define AD9545_CLK_AUX_TDC		3

/* PLL addresses */
#define AD9545_PLL0			0
#define AD9545_PLL1			1

/* Outputs addresses */
#define AD9545_Q0A			0
#define AD9545_Q0AA			1
#define AD9545_Q0B			2
#define AD9545_Q0BB			3
#define AD9545_Q0C			4
#define AD9545_Q0CC			5
#define AD9545_Q1A			6
#define AD9545_Q1AA			7
#define AD9545_Q1B			8
#define AD9545_Q1BB			9

/* NCO addresses */
#define AD9545_NCO0			0
#define AD9545_NCO1			1

/* TDC addresses */
#define AD9545_CLK_AUX_TDC0		0
#define AD9545_CLK_AUX_TDC1		1

/* Ex:
 * Output Q0C clock: <&ad9545_clock AD9545_CLK_OUT AD9545_Q0C>;
 * PLL0 clock: <&ad9545_clock AD9545_CLK_PLL AD9545_PLL0>;
 * NCO1 clock: <&ad9545_clock AD9545_CLK_NCO AD9545_NCO1>;
 */
#define BYTE_ADDR_H				NO_OS_GENMASK(14, 8)
#define BYTE_ADDR_L				NO_OS_GENMASK(7, 0)

/*
 * ad9545 registers definition
 */

#define AD9545_CONFIG_0			0x0000
#define AD9545_PRODUCT_ID_LOW		0x0004
#define AD9545_PRODUCT_ID_HIGH		0x0005
#define AD9545_IO_UPDATE		0x000F
#define AD9545_M0_PIN			0x0102
#define AD9545_CHIP_ID			0x0121
#define AD9545_SYS_CLK_FB_DIV		0x0200
#define AD9545_SYS_CLK_INPUT		0x0201
#define AD9545_SYS_CLK_REF_FREQ		0x0202
#define AD9545_STABILITY_TIMER		0x0207
#define AD9545_COMPENSATE_TDCS		0x0280
#define AD9545_COMPENSATE_NCOS		0x0281
#define AD9545_COMPENSATE_DPLL		0x0282
#define AD9545_AUX_DPLL_CHANGE_LIMIT	0x0283
#define AD9545_AUX_DPLL_SOURCE		0x0284
#define AD9545_AUX_DPLL_LOOP_BW		0x0285
#define AD9545_REF_A_CTRL		0x0300
#define AD9545_REF_A_RDIV		0x0400
#define AD9545_REF_A_PERIOD		0x0404
#define AD9545_REF_A_OFFSET_LIMIT	0x040C
#define AD9545_REF_A_MONITOR_HYST	0x040F
#define AD9545_REF_A_VALID_TIMER	0x0410
#define AD9545_PHASE_LOCK_THRESH	0x0800
#define AD9545_PHASE_LOCK_FILL_RATE	0x0803
#define AD9545_PHASE_LOCK_DRAIN_RATE	0x0804
#define AD9545_FREQ_LOCK_THRESH		0x0805
#define AD9545_FREQ_LOCK_FILL_RATE	0x0808
#define AD9545_FREQ_LOCK_DRAIN_RATE	0x0809
#define AD9545_DPLL0_FTW		0x1000
#define AD9545_DPLL0_SLEW_RATE		0x1011
#define AD9545_MODULATION_COUNTER_A0	0x10C2
#define AD9545_MODULATION_COUNTER_B0	0x10C6
#define AD9545_MODULATION_COUNTER_C0	0x10CA
#define AD9545_MODULATOR_A0		0x10CF
#define AD9545_MODULATOR_B0		0x10D0
#define AD9545_MODULATOR_C0		0x10D1
#define	AD9545_NSHOT_REQ_CH0		0x10D3
#define AD9545_NSHOT_EN_AB0		0x10D4
#define AD9545_NSHOT_EN_C0		0x10D5
#define AD9545_DRIVER_0A_CONF		0x10D7
#define AD9545_SYNC_CTRL0		0x10DB
#define AD9545_APLL0_M_DIV		0x1081
#define AD9545_Q0A_DIV			0x1100
#define AD9545_Q0A_PHASE		0x1104
#define AD9545_Q0A_PHASE_CONF		0x1108
#define AD9545_DPLL0_EN			0x1200
#define AD9545_DPLL0_SOURCE		0x1201
#define AD9545_DPLL0_ZERO_DELAY_FB	0x1202
#define AD9545_DPLL0_FB_MODE		0x1203
#define AD9545_DPLL0_LOOP_BW		0x1204
#define AD9545_DPLL0_HITLESS_N		0x1208
#define AD9545_DPLL0_N_DIV		0x120C
#define AD9545_DPLL0_FRAC		0x1210
#define AD9545_DPLL0_MOD		0x1213
#define AD9545_DPLL0_FAST_L1		0x1216
#define AD9545_DPLL0_FAST_L2		0x1217
#define AD9545_MODULATION_COUNTER_A1	0x14C2
#define AD9545_MODULATION_COUNTER_B1	0x14C6
#define AD9545_MODULATOR_A1		0x14CF
#define AD9545_MODULATOR_B1		0x14D0
#define AD9545_NSHOT_EN_AB1		0x14D4
#define AD9545_DRIVER_1A_CONF		0x14D7
#define AD9545_Q1A_DIV			0x1500
#define AD9545_Q1A_PHASE		0x1504
#define AD9545_Q1A_PHASE_CONF		0x1508
#define AD9545_CALIB_CLK		0x2000
#define AD9545_POWER_DOWN_REF		0x2001
#define AD9545_PWR_CALIB_CH0		0x2100
#define AD9545_CTRL_CH0			0x2101
#define AD9545_DIV_OPS_Q0A		0x2102
#define AD9545_DPLL0_MODE		0x2105
#define AD9545_DPLL0_FAST_MODE		0x2106
#define AD9545_DIV_OPS_Q1A		0x2202
#define AD9545_NCO0_CENTER_FREQ		0x2800
#define AD9545_NCO0_OFFSET_FREQ		0x2807
#define AD9545_NCO0_TAG_RATIO		0x280B
#define AD9545_NCO0_TAG_DELTA		0x280D
#define AD9545_NCO0_TYPE_ADJUST		0x280F
#define AD9545_NCO0_DELTA_RATE_LIMIT	0x2810
#define AD9545_NCO0_DELTA_ADJUST	0x2814
#define AD9545_NCO0_CYCLE_ADJUST	0x2819
#define AD9545_TDC0_DIV			0x2A00
#define AD9545_TDC0_PERIOD		0x2A01
#define AD9545_PLL_STATUS		0x3001
#define AD9545_MISC			0x3002
#define AD9545_TEMP0			0x3003
#define AD9545_REFA_STATUS		0x3005
#define AD9545_PLL0_STATUS		0x3100
#define AD9545_PLL0_OPERATION		0x3101

#define AD9545_SYS_CLK_STABILITY_PERIOD_MASK	NO_OS_GENMASK(19, 0)

#define AD9545_REF_CTRL_DIF_MSK			NO_OS_GENMASK(3, 2)
#define AD9545_REF_CTRL_REFA_MSK		NO_OS_GENMASK(5, 4)
#define AD9545_REF_CTRL_REFAA_MSK		NO_OS_GENMASK(7, 6)

#define AD9545_UPDATE_REGS			0x1
#define AD9545_RESET_REGS			0x81

#define AD9545_MX_PIN(x)			(AD9545_M0_PIN + (x))

#define AD9545_SYNC_CTRLX(x)			(AD9545_SYNC_CTRL0 + ((x) * 0x400))
#define AD9545_REF_X_RDIV(x)			(AD9545_REF_A_RDIV + ((x) * 0x20))
#define AD9545_REF_X_PERIOD(x)			(AD9545_REF_A_PERIOD + ((x) * 0x20))
#define AD9545_REF_X_OFFSET_LIMIT(x)		(AD9545_REF_A_OFFSET_LIMIT + ((x) * 0x20))
#define AD9545_REF_X_MONITOR_HYST(x)		(AD9545_REF_A_MONITOR_HYST + ((x) * 0x20))
#define AD9545_REF_X_VALID_TIMER(x)		(AD9545_REF_A_VALID_TIMER + ((x) * 0x20))
#define AD9545_REF_X_PHASE_LOCK_FILL(x)		(AD9545_PHASE_LOCK_FILL_RATE + ((x) * 0x20))
#define AD9545_REF_X_PHASE_LOCK_DRAIN(x)	(AD9545_PHASE_LOCK_DRAIN_RATE + ((x) * 0x20))
#define AD9545_REF_X_FREQ_LOCK_FILL(x)		(AD9545_FREQ_LOCK_FILL_RATE + ((x) * 0x20))
#define AD9545_REF_X_FREQ_LOCK_DRAIN(x)		(AD9545_FREQ_LOCK_DRAIN_RATE + ((x) * 0x20))

#define AD9545_SOURCEX_PHASE_THRESH(x)		(AD9545_PHASE_LOCK_THRESH + ((x) * 0x20))
#define AD9545_SOURCEX_FREQ_THRESH(x)		(AD9545_FREQ_LOCK_THRESH + ((x) * 0x20))
#define AD9545_NCOX_PHASE_THRESH(x)		(AD9545_SOURCEX_PHASE_THRESH((x) + 4))
#define AD9545_NCOX_FREQ_THRESH(x)		(AD9545_SOURCEX_FREQ_THRESH((x) + 4))

#define AD9545_APLLX_M_DIV(x)			(AD9545_APLL0_M_DIV + ((x) * 0x400))

#define AD9545_Q0_DIV(x)			(AD9545_Q0A_DIV + ((x) * 0x9))
#define AD9545_Q1_DIV(x)			(AD9545_Q1A_DIV + ((x) * 0x9))
#define AD9545_QX_DIV(x) ({					\
	typeof(x) x_ = (x);					\
								\
	(x_ > 5) ? AD9545_Q1_DIV(x_ - 6) : AD9545_Q0_DIV(x_);	\
})

#define AD9545_Q0_PHASE(x)			(AD9545_Q0A_PHASE + ((x) * 0x9))
#define AD9545_Q1_PHASE(x)			(AD9545_Q1A_PHASE + ((x) * 0x9))
#define AD9545_QX_PHASE(x) ({						\
	typeof(x) x_ = (x);						\
									\
	(x_ > 5) ? AD9545_Q1_PHASE(x_ - 6) : AD9545_Q0_PHASE(x_);	\
})

#define AD9545_Q0_PHASE_CONF(x)			(AD9545_Q0A_PHASE_CONF + ((x) * 0x9))
#define AD9545_Q1_PHASE_CONF(x)			(AD9545_Q1A_PHASE_CONF + ((x) * 0x9))
#define AD9545_QX_PHASE_CONF(x) ({						\
	typeof(x) x_ = (x);							\
										\
	(x_ > 5) ? AD9545_Q1_PHASE_CONF(x_ - 6) : AD9545_Q0_PHASE_CONF(x_);	\
})

#define AD9545_NSHOT_REQ_CH(x)			(AD9545_NSHOT_REQ_CH0 + ((x) * 0x400))
#define AD9545_DPLLX_FTW(x)			(AD9545_DPLL0_FTW + ((x) * 0x400))
#define AD9545_DPLLX_SLEW_RATE(x)		(AD9545_DPLL0_SLEW_RATE + ((x) * 0x400))
#define AD9545_DPLLX_EN(x, y)			(AD9545_DPLL0_EN + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_SOURCE(x, y)		(AD9545_DPLL0_SOURCE + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_FB_PATH(x, y)		(AD9545_DPLL0_ZERO_DELAY_FB + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_FB_MODE(x, y)		(AD9545_DPLL0_FB_MODE + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_LOOP_BW(x, y)		(AD9545_DPLL0_LOOP_BW + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_HITLESS_N(x, y)		(AD9545_DPLL0_HITLESS_N + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_N_DIV(x, y)		(AD9545_DPLL0_N_DIV + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_FRAC_DIV(x, y)		(AD9545_DPLL0_FRAC + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_MOD_DIV(x, y)		(AD9545_DPLL0_MOD + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_FAST_L1(x, y)		(AD9545_DPLL0_FAST_L1 + ((x) * 0x400) + ((y) * 0x20))
#define AD9545_DPLLX_FAST_L2(x, y)		(AD9545_DPLL0_FAST_L2 + ((x) * 0x400) + ((y) * 0x20))

#define AD9545_DIV_OPS_Q0(x)			(AD9545_DIV_OPS_Q0A + (x))
#define AD9545_DIV_OPS_Q1(x)			(AD9545_DIV_OPS_Q1A + (x))
#define AD9545_DIV_OPS_QX(x) ({						\
	typeof(x) x_ = (x) / 2;						\
									\
	(x_ > 2) ? AD9545_DIV_OPS_Q1(x_ - 3) : AD9545_DIV_OPS_Q0(x_);	\
})

#define AD9545_PWR_CALIB_CHX(x)			(AD9545_PWR_CALIB_CH0 + ((x) * 0x100))
#define AD9545_PLLX_STATUS(x)			(AD9545_PLL0_STATUS + ((x) * 0x100))
#define AD9545_PLLX_OPERATION(x)		(AD9545_PLL0_OPERATION + ((x) * 0x100))
#define AD9545_CTRL_CH(x)			(AD9545_CTRL_CH0 + ((x) * 0x100))
#define AD9545_DPLLX_FAST_MODE(x)		(AD9545_DPLL0_FAST_MODE + ((x) * 0x100))
#define AD9545_REFX_STATUS(x)			(AD9545_REFA_STATUS + (x))

#define AD9545_PROFILE_SEL_MODE_MSK		NO_OS_GENMASK(3, 2)
#define AD9545_PROFILE_SEL_MODE(x)		no_os_field_prep(AD9545_PROFILE_SEL_MODE_MSK, x)

#define AD9545_NCOX_CENTER_FREQ(x)		(AD9545_NCO0_CENTER_FREQ + ((x) * 0x40))
#define AD9545_NCOX_OFFSET_FREQ(x)		(AD9545_NCO0_OFFSET_FREQ + ((x) * 0x40))
#define AD9545_NCOX_TAG_RATIO(x)		(AD9545_NCO0_TAG_RATIO + ((x) * 0x40))
#define AD9545_NCOX_TAG_DELTA(x)		(AD9545_NCO0_TAG_DELTA + ((x) * 0x40))
#define AD9545_NCOX_TYPE_ADJUST(x)		(AD9545_NCO0_TYPE_ADJUST + ((x) * 0x40))
#define AD9545_NCOX_DELTA_RATE_LIMIT(x)		(AD9545_NCO0_DELTA_RATE_LIMIT + ((x) * 0x40))
#define AD9545_NCOX_DELTA_ADJUST(x)		(AD9545_NCO0_DELTA_ADJUST + ((x) * 0x40))
#define AD9545_NCOX_CYCLE_ADJUST(x)		(AD9545_NCO0_CYCLE_ADJUST + ((x) * 0x40))

/*
 * AD9545 AUX NCO center frequency register has 16-bit integer part and
 * 40-bit fractional part.
 */
#define AD9545_NCO_CENTER_FREQ_INT_WIDTH	16
#define AD9545_NCO_CENTER_FREQ_FRAC_WIDTH	40
#define AD9545_NCO_CENTER_FREQ_WIDTH		(AD9545_NCO_CENTER_FREQ_INT_WIDTH + \
						 AD9545_NCO_CENTER_FREQ_FRAC_WIDTH)

#define AD9545_NCO_CENTER_FREQ_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_CENTER_FREQ_WIDTH - 1, 0)
#define AD9545_NCO_CENTER_FREQ_INT_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_CENTER_FREQ_WIDTH - 1, \
							    AD9545_NCO_CENTER_FREQ_FRAC_WIDTH)
#define AD9545_NCO_CENTER_FREQ_FRAC_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_CENTER_FREQ_FRAC_WIDTH - 1, 0)

#define AD9545_NCO_CENTER_FREQ_MAX		no_os_field_max_u64(AD9545_NCO_CENTER_FREQ_MSK)
#define AD9545_NCO_CENTER_FREQ_INT_MAX		no_os_field_max_u64(AD9545_NCO_CENTER_FREQ_INT_MSK)
#define AD9545_NCO_CENTER_FREQ_FRAC_MAX		no_os_field_max_u64(AD9545_NCO_CENTER_FREQ_FRAC_MSK)

/*
 * AD9545 AUX NCO offset frequency register has 8-bit integer part and
 * 24-bit fractional part.
 */
#define AD9545_NCO_OFFSET_FREQ_INT_WIDTH	8
#define AD9545_NCO_OFFSET_FREQ_FRAC_WIDTH	24
#define AD9545_NCO_OFFSET_FREQ_WIDTH		(AD9545_NCO_OFFSET_FREQ_INT_WIDTH + \
						 AD9545_NCO_OFFSET_FREQ_FRAC_WIDTH)

#define AD9545_NCO_OFFSET_FREQ_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_OFFSET_FREQ_WIDTH - 1, 0)
#define AD9545_NCO_OFFSET_FREQ_INT_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_OFFSET_FREQ_WIDTH - 1, \
							    AD9545_NCO_OFFSET_FREQ_FRAC_WIDTH)
#define AD9545_NCO_OFFSET_FREQ_FRAC_MSK		NO_OS_GENMASK_ULL(AD9545_NCO_OFFSET_FREQ_FRAC_WIDTH - 1, 0)

#define AD9545_NCO_OFFSET_FREQ_MAX		no_os_field_max(AD9545_NCO_OFFSET_FREQ_MSK)
#define AD9545_NCO_OFFSET_FREQ_INT_MAX		no_os_field_max(AD9545_NCO_OFFSET_FREQ_INT_MSK)
#define AD9545_NCO_OFFSET_FREQ_FRAC_MAX		no_os_field_max(AD9545_NCO_OFFSET_FREQ_FRAC_MSK)

#define AD9545_NCO_FREQ_INT_MAX			(AD9545_NCO_CENTER_FREQ_INT_MAX + \
						 AD9545_NCO_OFFSET_FREQ_INT_MAX)

#define AD9545_TDCX_DIV(x)			(AD9545_TDC0_DIV + ((x) * 0x9))
#define AD9545_TDCX_PERIOD(x)			(AD9545_TDC0_PERIOD + ((x) * 0x9))

/* AD9545 MX PIN bitfields */
#define AD9545_MX_TO_TDCX(x)			(0x30 + (x))

/* AD9545 COMPENSATE TDCS bitfields */
#define AD9545_COMPENSATE_TDCS_VIA_AUX_DPLL	0x4

/* AD9545 COMPENSATE NCOS bitfields */
#define AD9545_COMPENSATE_NCOS_VIA_AUX_DPLL	0x44

/* AD9545 COMPENSATE DPLL bitfields */
#define AD9545_COMPNESATE_VIA_AUX_DPLL		0x44

/* define AD9545_DPLLX_EN bitfields */
#define AD9545_EN_PROFILE_MSK			NO_OS_BIT(0)
#define AD9545_SEL_PRIORITY_MSK			NO_OS_GENMASK(5, 1)

/* define AD9545_DPLLX_FB_MODE bitfields */
#define AD9545_EN_HITLESS_MSK			NO_OS_BIT(0)
#define AD9545_TAG_MODE_MSK			NO_OS_GENMASK(4, 2)
#define AD9545_BASE_FILTER_MSK			NO_OS_BIT(7)

/* AD9545_PWR_CALIB_CHX bitfields */
#define AD9545_PWR_DOWN_CH			NO_OS_BIT(0)
#define AD9545_CALIB_APLL			NO_OS_BIT(1)

/* AD9545_SYNC_CTRLX bitfields */
#define AD9545_SYNC_CTRL_DPLL_REF_MSK		NO_OS_BIT(2)
#define AD9545_SYNC_CTRL_MODE_MSK		NO_OS_GENMASK(1, 0)

/* AD9545_QX_PHASE_CONF bitfields */
#define AD9545_QX_HALF_DIV_MSK			NO_OS_BIT(5)
#define AD9545_QX_PHASE_32_MSK			NO_OS_BIT(6)

/* AD9545_DIV_OPS_QX bitfields */
#define AD9545_DIV_OPS_MUTE_A_MSK		NO_OS_BIT(2)
#define AD9545_DIV_OPS_MUTE_AA_MSK		NO_OS_BIT(3)

/* AD9545 Modulator bitfields */
#define AD9545_MODULATOR_EN			NO_OS_BIT(0)

/* AD9545_NSHOT_REQ_CH bitfields */
#define AD9545_NSHOT_NR_MSK			NO_OS_GENMASK(5, 0)

/* AD9545_CTRL_CH bitfields */
#define AD9545_CTRL_CH_NSHOT_MSK		NO_OS_BIT(0)

/* AD9545_PLL_STATUS bitfields */
#define AD9545_PLLX_LOCK(x, y)			((1 << (4 + (x))) & (y))

/* AD9545_MISC bitfields */
#define AD9545_MISC_AUX_NC0_ERR_MSK		NO_OS_GENMASK(5, 4)
#define AD9545_MISC_AUX_NC1_ERR_MSK		NO_OS_GENMASK(7, 6)
#define AD9545_AUX_DPLL_LOCK_MSK		NO_OS_BIT(1)
#define AD9545_AUX_DPLL_REF_FAULT		NO_OS_BIT(2)

/* AD9545_REFX_STATUS bitfields */
#define AD9545_REFX_SLOW_MSK			NO_OS_BIT(0)
#define AD9545_REFX_FAST_MSK			NO_OS_BIT(1)
#define AD9545_REFX_JITTER_MSK			NO_OS_BIT(2)
#define AD9545_REFX_FAULT_MSK			NO_OS_BIT(3)
#define AD9545_REFX_VALID_MSK			NO_OS_BIT(4)
#define AD9545_REFX_LOS_MSK			NO_OS_BIT(5)

/* AD9545_PLL0_STATUS bitfields */
#define AD9545_PLL_LOCKED			NO_OS_BIT(0)

/* AD9545_PLL0_OPERATION bitfields */
#define AD9545_PLL_FREERUN			NO_OS_BIT(0)
#define AD9545_PLL_HOLDOVER			NO_OS_BIT(1)
#define AD9545_PLL_ACTIVE			NO_OS_BIT(3)
#define AD9545_PLL_ACTIVE_PROFILE		NO_OS_GENMASK(6, 4)

#define AD9545_SYS_PLL_STABLE_MSK		NO_OS_GENMASK(1, 0)
#define AD9545_SYS_PLL_STABLE(x)		(((x) & AD9545_SYS_PLL_STABLE_MSK) == 0x3)

#define AD9545_APLL_LOCKED(x)			((x) & NO_OS_BIT(3))

/*  AD9545 tagging modes */
#define AD9545_NO_TAGGING		0
#define AD9545_FB_PATH_TAG		2

#define AD9545_SYS_CLK_STABILITY_MS	50

#define AD9545_R_DIV_MSK		NO_OS_GENMASK(29, 0)
#define AD9545_R_DIV_MAX		0x40000000
#define AD9545_IN_MAX_TDC_FREQ_HZ	200000

#define AD9545_MAX_REFS			4

#define AD9545_APLL_M_DIV_MIN		1
#define AD9545_APLL_M_DIV_MAX		255

#define AD9545_DPLL_MAX_N		1073741823
#define AD9545_DPLL_MAX_FRAC		16777215
#define AD9545_DPLL_MAX_MOD		16777215
#define AD9545_MAX_DPLL_PROFILES	6

#define AD9545_MAX_NSHOT_PULSES		63

#define AD9545_MAX_ZERO_DELAY_RATE	200000000

#define AD9545_MIN_SYS_CLK_FREQ		2250
#define AD9545_MAX_SYS_CLK_FREQ		2415
#define AD9545_MIN_DIV_RATIO		4
#define AD9545_MAX_DIV_RATIO		256

enum ad9545_comm_type {
	SPI,
	I2C,
};

static const unsigned int ad9545_apll_rate_ranges_hz[2][2] = {
	{2424000000U, 3232000000U}, {3232000000U, 4040000000U}
};

static const unsigned int ad9545_apll_pfd_rate_ranges_hz[2] = {
	162000000U, 350000000U
};

static const unsigned short ad9545_vco_calibration_op[][2] = {
	{AD9545_CALIB_CLK, 0},
	{AD9545_IO_UPDATE, AD9545_UPDATE_REGS},
	{AD9545_CALIB_CLK, NO_OS_BIT(2)},
	{AD9545_IO_UPDATE, AD9545_UPDATE_REGS},
};

static const uint8_t ad9545_tdc_source_mapping[] = {
	0, 1, 2, 3, 8, 9,
};

static const uint32_t ad9545_fast_acq_excess_bw_map[] = {
	0, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024,
};

static const uint32_t ad9545_fast_acq_timeout_map[] = {
	1, 10, 50, 100, 500, 1000, 10000, 50000,
};

static const uint32_t ad9545_hyst_scales_bp[] = {
	0, 3125, 6250, 12500, 25000, 50000, 75000, 87500
};

static const uint32_t ad9545_out_source_ua[] = {
	7500, 12500, 15000
};

static const uint32_t ad9545_rate_change_limit_map[] = {
	715, 1430, 2860, 5720, 11440, 22880, 45760,
};

static const char * const ad9545_ref_m_clk_names[] = {
	"Ref-M0", "Ref-M1", "Ref-M2",
};

static const char * const ad9545_ref_clk_names[] = {
	"Ref-A", "Ref-AA", "Ref-B", "Ref-BB",
};

static const char * const ad9545_in_clk_names[] = {
	"Ref-A-Div", "Ref-AA-Div", "Ref-B-Div", "Ref-BB-Div",
};

static const char * const ad9545_out_clk_names[] = {
	"Q0A-div", "Q0AA-div", "Q0B-div", "Q0BB-div", "Q0C-div", "Q0CC-div", "Q1A-div", "Q1AA-div",
	"Q1B-div", "Q1BB-div",
};

static const char * const ad9545_pll_clk_names[] = {
	"PLL0", "PLL1",
};

static const char * const ad9545_aux_nco_clk_names[] = {
	"AUX_NCO0", "AUX_NCO1",
};

static const char * const ad9545_aux_tdc_clk_names[] = {
	"AUX_TDC0", "AUX_TDC1",
};

enum ad9545_ref_mode {
	AD9545_SINGLE_ENDED = 0,
	AD9545_DIFFERENTIAL,
};

enum ad9545_single_ended_config {
	AD9545_AC_COUPLED_IF = 0,
	AD9545_DC_COUPLED_1V2,
	AD9545_DC_COUPLED_1V8,
	AD9545_IN_PULL_UP,
};

enum ad9545_diferential_config {
	AD9545_AC_COUPLED = 0,
	AD9545_DC_COUPLED,
	AD9545_DC_COUPLED_LVDS,
};

enum ad9545_output_mode {
	AD9545_SINGLE_DIV_DIF = 0,
	AD9545_SINGLE_DIV,
	AD9545_DUAL_DIV,
};

struct ad9545_outputs_regs {
	uint16_t	modulator_reg;
	uint16_t	modulation_counter_reg;
	uint16_t	nshot_en_reg;
	uint8_t	nshot_en_msk;
};

static const struct ad9545_outputs_regs ad9545_out_regs[] = {
	{
		.modulator_reg = AD9545_MODULATOR_A0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_A0,
		.nshot_en_reg = AD9545_NSHOT_EN_AB0,
		.nshot_en_msk = NO_OS_BIT(0),
	},
	{
		.modulator_reg = AD9545_MODULATOR_A0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_A0,
		.nshot_en_reg = AD9545_NSHOT_EN_AB0,
		.nshot_en_msk = NO_OS_BIT(2),
	},
	{
		.modulator_reg = AD9545_MODULATOR_B0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_B0,
		.nshot_en_reg = AD9545_NSHOT_EN_AB0,
		.nshot_en_msk = NO_OS_BIT(4),
	},
	{
		.modulator_reg = AD9545_MODULATOR_B0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_B0,
		.nshot_en_reg = AD9545_NSHOT_EN_AB0,
		.nshot_en_msk = NO_OS_BIT(6),
	},
	{
		.modulator_reg = AD9545_MODULATOR_C0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_C0,
		.nshot_en_reg = AD9545_NSHOT_EN_C0,
		.nshot_en_msk = NO_OS_BIT(0),
	},
	{
		.modulator_reg = AD9545_MODULATOR_C0,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_C0,
		.nshot_en_reg = AD9545_NSHOT_EN_C0,
		.nshot_en_msk = NO_OS_BIT(2),
	},
	{
		.modulator_reg = AD9545_MODULATOR_A1,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_A1,
		.nshot_en_reg = AD9545_NSHOT_EN_AB1,
		.nshot_en_msk = NO_OS_BIT(0),
	},
	{
		.modulator_reg = AD9545_MODULATOR_A1,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_A1,
		.nshot_en_reg = AD9545_NSHOT_EN_AB1,
		.nshot_en_msk = NO_OS_BIT(2),
	},
	{
		.modulator_reg = AD9545_MODULATOR_B1,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_B1,
		.nshot_en_reg = AD9545_NSHOT_EN_AB1,
		.nshot_en_msk = NO_OS_BIT(4),
	},
	{
		.modulator_reg = AD9545_MODULATOR_B1,
		.modulation_counter_reg = AD9545_MODULATION_COUNTER_B1,
		.nshot_en_reg = AD9545_NSHOT_EN_AB1,
		.nshot_en_msk = NO_OS_BIT(6),
	},
};

struct ad9545_out_clk {
	struct ad9545_dev	*dev;
	bool			output_used;
	bool			source_current;
	enum ad9545_output_mode	output_mode;
	uint32_t		source_ua;
	struct no_os_clk_desc	*hw;
	unsigned int		address;
	uint64_t		rate_requested_hz;
	struct no_os_clk_desc	*parent_clk;
};

struct ad9545_dpll_profile {
	unsigned int	address;
	unsigned int	parent_index;
	unsigned int	priority;
	unsigned int	loop_bw_uhz;
	unsigned int	fast_acq_excess_bw;
	unsigned int	fast_acq_timeout_ms;
	unsigned int	fast_acq_settle_ms;
	bool		en;
	uint8_t		tdc_source;
	bool		fb_tagging;
};

struct ad9545_pll_clk {
	struct ad9545_dev		*dev;
	bool				pll_used;
	unsigned int			address;
	struct no_os_clk_desc		*hw;
	uint8_t				num_parents;
	struct no_os_clk_desc		**parents;
	struct ad9545_dpll_profile	profiles[AD9545_MAX_DPLL_PROFILES];
	unsigned int			free_run_freq;
	unsigned int			fast_acq_trigger_mode;
	uint64_t			rate_requested_hz;
	bool				internal_zero_delay;
	uint8_t				internal_zero_delay_source;
	uint64_t			internal_zero_delay_source_rate_hz;
	uint32_t			slew_rate_limit_ps;
};

struct ad9545_ref_in_clk {
	struct no_os_clk_desc		*hw;
	struct ad9545_dev		*dev;
	uint32_t			r_div_ratio;
	bool				ref_used;
	uint32_t			d_tol_ppb;
	uint8_t				monitor_hyst_scale;
	uint32_t			valid_t_ms;
	struct no_os_clk_desc		*parent_clk;
	unsigned int			address;
	enum ad9545_ref_mode		mode;
	unsigned int			freq_thresh_ps;
	unsigned int			phase_thresh_ps;
	unsigned int			phase_lock_fill_rate;
	unsigned int			phase_lock_drain_rate;
	unsigned int			freq_lock_fill_rate;
	unsigned int			freq_lock_drain_rate;
	union {
		enum ad9545_single_ended_config		s_conf;
		enum ad9545_diferential_config		d_conf;
	};
};

struct ad9545_aux_nco_clk {
	struct no_os_clk_desc		*hw;
	bool				nco_used;
	struct ad9545_dev		*dev;
	unsigned int			address;
	unsigned int			freq_thresh_ps;
	unsigned int			phase_thresh_ps;
};

struct ad9545_aux_tdc_clk {
	struct no_os_clk_desc		*hw;
	bool				tdc_used;
	struct ad9545_dev		*dev;
	unsigned int			address;
	unsigned int			pin_nr;
	struct no_os_clk_desc		*parent_clk;
};

struct ad9545_aux_dpll_clk {
	struct no_os_clk_desc		*hw;
	bool				dpll_used;
	struct ad9545_dev		*dev;
	unsigned int			source;
	unsigned int			loop_bw_mhz;
	unsigned int			rate_change_limit;
	struct no_os_clk_desc		*parent_clk;
};

struct ad9545_sys_clk {
	bool				sys_clk_freq_doubler;
	bool				sys_clk_crystal;
	uint32_t			ref_freq_hz;
	uint32_t			sys_freq_hz;
};

struct ad9545_dev;

typedef int32_t (*ad9545_reg_read_func)(struct ad9545_dev *dev,
					uint16_t reg_addr,
					uint8_t *reg_data);
typedef int32_t (*ad9545_reg_write_func)(struct ad9545_dev *dev,
		uint16_t reg_addr,
		uint8_t reg_data);
typedef int32_t (*ad9545_reg_read_multi_func)(struct ad9545_dev *dev,
		uint16_t reg_addr,
		uint8_t *reg_data,
		uint16_t count);
typedef int32_t (*ad9545_reg_write_multi_func)(struct ad9545_dev *dev,
		uint16_t reg_addr,
		uint8_t *reg_data,
		uint16_t count);

struct ad9545_dev {
	/* SPI */
	struct no_os_spi_desc		*spi_desc;
	/* I2C */
	struct no_os_i2c_desc		*i2c_desc;
	/* Device Settings */
	ad9545_reg_read_func		reg_read;
	ad9545_reg_write_func		reg_write;
	ad9545_reg_read_multi_func	reg_read_multiple;
	ad9545_reg_write_multi_func	reg_write_multiple;
	/* Device Settings */
	enum ad9545_comm_type		comm_type;
	struct ad9545_sys_clk		sys_clk;
	struct ad9545_aux_dpll_clk	aux_dpll_clk;
	struct ad9545_pll_clk		pll_clks[NO_OS_ARRAY_SIZE(ad9545_pll_clk_names)];
	struct ad9545_ref_in_clk	ref_in_clks[NO_OS_ARRAY_SIZE(ad9545_ref_clk_names)];
	struct ad9545_out_clk		out_clks[NO_OS_ARRAY_SIZE(ad9545_out_clk_names)];
	struct ad9545_aux_nco_clk	aux_nco_clks[NO_OS_ARRAY_SIZE(
				ad9545_aux_nco_clk_names)];
	struct ad9545_aux_tdc_clk	aux_tdc_clks[NO_OS_ARRAY_SIZE(
				ad9545_aux_tdc_clk_names)];
	/* CLK descriptors */
	struct no_os_clk_desc		**clks[4];

};

struct ad9545_init_param {
	/* SPI */
	struct no_os_spi_init_param	*spi_init;
	/* I2C */
	struct no_os_i2c_init_param	*i2c_init;
	/* Device Settings */
	enum ad9545_comm_type		comm_type;
	struct ad9545_sys_clk		sys_clk;
	struct ad9545_aux_dpll_clk	aux_dpll_clk;
	struct ad9545_pll_clk		pll_clks[NO_OS_ARRAY_SIZE(ad9545_pll_clk_names)];
	struct ad9545_ref_in_clk	ref_in_clks[NO_OS_ARRAY_SIZE(ad9545_ref_clk_names)];
	struct ad9545_out_clk		out_clks[NO_OS_ARRAY_SIZE(ad9545_out_clk_names)];
	struct ad9545_aux_nco_clk	aux_nco_clks[NO_OS_ARRAY_SIZE(
				ad9545_aux_nco_clk_names)];
	struct ad9545_aux_tdc_clk	aux_tdc_clks[NO_OS_ARRAY_SIZE(
				ad9545_aux_tdc_clk_names)];

};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
/* Reads a single byte from the specified SPI register address */
int32_t ad9545_spi_reg_read(struct ad9545_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data);

/* Reads multiple bytes starting from the specified SPI register address */
int32_t ad9545_spi_reg_read_multiple(struct ad9545_dev *dev,
				     uint16_t reg_addr,
				     uint8_t *reg_data,
				     uint16_t count);
/* Writes a single byte to the specified SPI register address */
int32_t ad9545_spi_reg_write(struct ad9545_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data);
/* Writes multiple bytes starting from the specified SPI register address */
int32_t ad9545_spi_reg_write_multiple(struct ad9545_dev *dev,
				      uint16_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
/* Reads a single byte from the specified I2C register address */
int32_t ad9545_i2c_reg_read(struct ad9545_dev *dev,
			    uint16_t reg_addr,
			    uint8_t *reg_data);
/* Writes a single byte to the specified I2C register address */
int32_t ad9545_i2c_reg_write(struct ad9545_dev *dev,
			     uint16_t reg_addr,
			     uint8_t reg_data);
/* Reads multiple bytes starting from the specified I2C register address */
int32_t ad9545_i2c_reg_read_multiple(struct ad9545_dev *dev,
				     uint16_t reg_addr,
				     uint8_t *reg_data,
				     uint16_t count);
/* Writes multiple bytes starting from the specified I2C register address */
int32_t ad9545_i2c_reg_write_multiple(struct ad9545_dev *dev,
				      uint16_t reg_addr,
				      uint8_t *reg_data,
				      uint16_t count);
/* Modifies specific bits in the specified register and writes the new value. */
int32_t ad9545_write_mask(struct ad9545_dev *dev,
			  uint16_t reg_addr,
			  uint32_t mask,
			  uint8_t data);

/* Calibrates the APLLs of the AD9545 device. */
int ad9545_calib_aplls(struct ad9545_dev *dev);

/* Device Setup */
int32_t ad9545_setup(struct ad9545_dev *dev);

/* Device Initialization */
int32_t ad9545_init(struct ad9545_dev **device,
		    struct ad9545_init_param *init_param);

/* Free resources */
int32_t ad9545_remove(struct ad9545_dev *dev);


#endif // AD9545_H_
