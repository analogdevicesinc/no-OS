/***************************************************************************//**
 *   @file   adiol100.h
 *   @brief  Implementation of ADIOL100 Driver.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef __ADIOL100_H_
#define __ADIOL100_H_

#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include <stdint.h>
#include "no_os_delay.h"

/* Number of IO-Link channels. */
#define ADIOL100_NUM_CHANNELS  2

/* USER_direct registers — channel A */
#define ADIOL100_REG_INTERRUPTG         0x0000
#define ADIOL100_REG_RXFIFOSTAT_A       0x0001
#define ADIOL100_REG_INTERRUPT_A        0x0002
#define ADIOL100_REG_STATUS_A           0x0003
#define ADIOL100_REG_TXRXDATA_A         0x0004
#define ADIOL100_REG_FRAMCTRL1_A        0x0005
#define ADIOL100_REG_FRAMCTRL2_A        0x0006
#define ADIOL100_REG_CYCLTMR_A          0x0007
#define ADIOL100_REG_CQSET_A            0x0009
#define ADIOL100_REG_CQCURLIM_A         0x000A
#define ADIOL100_REG_INTEN_A            0x000E
#define ADIOL100_REG_LPCFG_A            0x000D

/* USER_direct registers — channel B */
#define ADIOL100_REG_RXFIFOSTAT_B       0x0011
#define ADIOL100_REG_INTERRUPT_B        0x0012
#define ADIOL100_REG_STATUS_B           0x0013
#define ADIOL100_REG_TXRXDATA_B         0x0014
#define ADIOL100_REG_FRAMCTRL1_B        0x0015
#define ADIOL100_REG_FRAMCTRL2_B        0x0016
#define ADIOL100_REG_CYCLTMR_B          0x0017
#define ADIOL100_REG_CQSET_B            0x0019
#define ADIOL100_REG_CQCURLIM_B         0x001A
#define ADIOL100_REG_INTEN_B            0x001E
#define ADIOL100_REG_LPCFG_B            0x001D

/* USER_direct registers — global */
#define ADIOL100_REG_WATCHDOG           0x000F
#define ADIOL100_REG_TRIGGER            0x0010

/* USER_paged registers — global */
#define ADIOL100_REG_INTERRUPTG_EN      0x1F00
#define ADIOL100_REG_CLOCKCFG           0x1F01
#define ADIOL100_REG_REVISIONID         0x1F02
#define ADIOL100_REG_DEVICEID           0x1F03
#define ADIOL100_REG_SPIFIFO_A_BURST    0x1F04
#define ADIOL100_REG_SPIFIFO_B_BURST    0x1F05
#define ADIOL100_REG_LPFETPWRCFG_A      0x1F10
#define ADIOL100_REG_LPCURRENTCFG_A     0x1F11
#define ADIOL100_REG_LPFETPROTECT_A     0x1F12
#define ADIOL100_REG_LPSLOPE_A          0x1F13
#define ADIOL100_REG_LPRETRY_A          0x1F14
#define ADIOL100_REG_LPFETPWRCFG_B      0x1F20
#define ADIOL100_REG_LPCURRENTCFG_B     0x1F21
#define ADIOL100_REG_LPFETPROTECT_B     0x1F22
#define ADIOL100_REG_LPSLOPE_B          0x1F23
#define ADIOL100_REG_LPRETRY_B          0x1F24

/* USER_direct registers — CQ external FET config */
#define ADIOL100_REG_CQEXTCFG_A         0x000B
#define ADIOL100_REG_CQEXTCFG_B         0x001B

/* CQExtCfg bit masks (0x000B / 0x001B) */
#define ADIOL100_CQCL_S                 NO_OS_BIT(13)
#define ADIOL100_CQCL_L                 NO_OS_BIT(12)
#define ADIOL100_CQTO_L                 NO_OS_BIT(11)
#define ADIOL100_CQSLOERR_L             NO_OS_BIT(10)
#define ADIOL100_CQOL_L                 NO_OS_BIT(9)
#define ADIOL100_CQEXTCLNOM_MSK         NO_OS_GENMASK(7, 0)

/* ExtraPage register — gateway to USER_paged map */
#define ADIOL100_REG_EXTRAPAGE          0x001F
#define ADIOL100_PAGED_BASE             0x1F00

/* FramCtrl1 bit masks (0x0005 / 0x0015) */
#define ADIOL100_INSCHKS                NO_OS_BIT(12)
#define ADIOL100_COMRT_MSK              NO_OS_GENMASK(7, 6)
#define ADIOL100_ESTCOM                 NO_OS_BIT(5)
#define ADIOL100_WUPULS                 NO_OS_BIT(4)
#define ADIOL100_TXKEEPMSG              NO_OS_BIT(3)
#define ADIOL100_FRAMEREN               NO_OS_BIT(2)
#define ADIOL100_CYCLETMREN             NO_OS_BIT(1)
#define ADIOL100_CQSEND                 NO_OS_BIT(0)

/* CyclTmr bit masks (0x0007 / 0x0017) */
#define ADIOL100_TCYCLBS_MSK            NO_OS_GENMASK(7, 6)
#define ADIOL100_TCYCLM_MSK             NO_OS_GENMASK(5, 0)
#define ADIOL100_CYCLTMR_MSK            NO_OS_GENMASK(7, 0)

/* FramCtrl2 bit masks (0x0006 / 0x0016) */
#define ADIOL100_CHANRST                NO_OS_BIT(15)
#define ADIOL100_TXFIFORSTN             NO_OS_BIT(13)
#define ADIOL100_RXFIFORSTN             NO_OS_BIT(12)

/* InterruptG_En bit masks (0x1F00) */
#define ADIOL100_EXTCLKOKINTEN          NO_OS_BIT(15)
#define ADIOL100_WDGINTEN               NO_OS_BIT(14)
#define ADIOL100_SPICRCERRINTEN         NO_OS_BIT(13)
#define ADIOL100_EXTCLKMISINTEN         NO_OS_BIT(12)
#define ADIOL100_THSHDNINTEN            NO_OS_BIT(11)
#define ADIOL100_THWARNINTEN            NO_OS_BIT(10)
#define ADIOL100_V24ERRINTEN            NO_OS_BIT(9)
#define ADIOL100_V24WARNINTEN           NO_OS_BIT(8)

/* InterruptG bit masks (0x0000) — bits 15:8 read-to-clear, bits 7:0 live status */
#define ADIOL100_EXTCLKOKINT            NO_OS_BIT(15)
#define ADIOL100_WDGINT                 NO_OS_BIT(14)
#define ADIOL100_SPICRCERRINT           NO_OS_BIT(13)
#define ADIOL100_EXTCLKMISINT           NO_OS_BIT(12)
#define ADIOL100_THSHDNINT              NO_OS_BIT(11)
#define ADIOL100_THWARNINT              NO_OS_BIT(10)
#define ADIOL100_V24ERRINT              NO_OS_BIT(9)
#define ADIOL100_V24WARNINT             NO_OS_BIT(8)
#define ADIOL100_EXTCLKOKST             NO_OS_BIT(7)
#define ADIOL100_WDGST                  NO_OS_BIT(6)
#define ADIOL100_SPICRCERRSTST          NO_OS_BIT(5)
#define ADIOL100_EXTCLKMISST            NO_OS_BIT(4)
#define ADIOL100_THSHDNST               NO_OS_BIT(3)
#define ADIOL100_THWARNST               NO_OS_BIT(2)
#define ADIOL100_V24ERRST               NO_OS_BIT(1)
#define ADIOL100_V24WARNST              NO_OS_BIT(0)

/* Interrupt bit masks (0x0002 / 0x0012) — read-to-clear */
#define ADIOL100_DELAYERRINT            NO_OS_BIT(15)
#define ADIOL100_SAFEPULSRINT           NO_OS_BIT(14)
#define ADIOL100_TXERRINT               NO_OS_BIT(13)
#define ADIOL100_RXERRINT               NO_OS_BIT(12)
#define ADIOL100_ESTCOMSUCINT           NO_OS_BIT(11)
#define ADIOL100_ESTCOMFAILINT          NO_OS_BIT(10)
#define ADIOL100_RXDARDYINT             NO_OS_BIT(9)
#define ADIOL100_DILEVELINT             NO_OS_BIT(8)
#define ADIOL100_LPERRINT               NO_OS_BIT(7)
#define ADIOL100_LPUVINT                NO_OS_BIT(6)
#define ADIOL100_CQDRVERRINT            NO_OS_BIT(5)
#define ADIOL100_CQTHEINT               NO_OS_BIT(4)
#define ADIOL100_CQCURLIMINT            NO_OS_BIT(3)
#define ADIOL100_CQVWINT                NO_OS_BIT(2)
#define ADIOL100_CQRXWINT               NO_OS_BIT(1)
#define ADIOL100_CQLEVELINT             NO_OS_BIT(0)

/* RxFIFOStat bit masks (0x0001 / 0x0011) */
#define ADIOL100_TRANSMERR              NO_OS_BIT(15)
#define ADIOL100_TERRSEND               NO_OS_BIT(14)
#define ADIOL100_TCHKSMERR              NO_OS_BIT(13)
#define ADIOL100_TSIZEERR               NO_OS_BIT(12)
#define ADIOL100_RCHKSMERR              NO_OS_BIT(11)
#define ADIOL100_RSIZEERR               NO_OS_BIT(10)
#define ADIOL100_FRAMEERR               NO_OS_BIT(9)
#define ADIOL100_PARITYERR              NO_OS_BIT(8)
#define ADIOL100_RXFIFOLVL_MSK          NO_OS_GENMASK(7, 0)

/* Status bit masks (0x0003 / 0x0013) */
#define ADIOL100_COMLOST                NO_OS_BIT(13)
#define ADIOL100_RXDARDY                NO_OS_BIT(9)
#define ADIOL100_DILEVEL                NO_OS_BIT(8)
#define ADIOL100_LPERR                  NO_OS_BIT(7)
#define ADIOL100_LPUV                   NO_OS_BIT(6)
#define ADIOL100_CQDRVERR               NO_OS_BIT(5)
#define ADIOL100_CQLEVEL                NO_OS_BIT(0)

/* CQSet bit masks (0x0009 / 0x0019) */
#define ADIOL100_IEC3TH                 NO_OS_BIT(15)
#define ADIOL100_SOURCESINK             NO_OS_BIT(14)
#define ADIOL100_SINKSEL_MSK            NO_OS_GENMASK(13, 12)
#define ADIOL100_GLITCHFILTER_MSK       NO_OS_GENMASK(11, 10)
#define ADIOL100_CQSLEW_MSK             NO_OS_GENMASK(9, 8)
#define ADIOL100_NPN                    NO_OS_BIT(7)
#define ADIOL100_PUSHPUL                NO_OS_BIT(6)
#define ADIOL100_DRVEN                  NO_OS_BIT(5)
#define ADIOL100_CQEXT                  NO_OS_BIT(3)
#define ADIOL100_INVCQ                  NO_OS_BIT(2)
#define ADIOL100_TXEN                   NO_OS_BIT(1)

/* LPCfg bit masks (0x000D / 0x001D) */
#define ADIOL100_LPFETREVEN             NO_OS_BIT(15)
#define ADIOL100_LPEN                   NO_OS_BIT(8)
#define ADIOL100_LPCLNOM_MSK            NO_OS_GENMASK(7, 0)

/* LPAFetPwrCfg bit masks (0x1F10 / 0x1F20) */
#define ADIOL100_LPFETPWRMAX_MSK        NO_OS_GENMASK(7, 0)

/* LPACurrentCfg bit masks (0x1F11 / 0x1F21) */
#define ADIOL100_LPCURRMAX_MSK          NO_OS_GENMASK(15, 8)
#define ADIOL100_LPCURRMIN_MSK          NO_OS_GENMASK(7, 0)

/* LPAFETProtect bit masks (0x1F12 / 0x1F22) */
#define ADIOL100_LPOLTIMOUT_MSK         NO_OS_GENMASK(15, 8)
#define ADIOL100_LPCLTIMOUT_MSK         NO_OS_GENMASK(7, 0)

/* LPASlope bit masks (0x1F13 / 0x1F23) */
#define ADIOL100_LPSLOPE_MSK            NO_OS_GENMASK(15, 8)
#define ADIOL100_LPSLOPEBL_MSK          NO_OS_GENMASK(7, 0)

/* LPARetry bit masks (0x1F14 / 0x1F24) */
#define ADIOL100_LPARTIM_MSK            NO_OS_GENMASK(13, 8)
#define ADIOL100_LPAR_MSK               NO_OS_GENMASK(3, 0)

/* Watchdog bit masks (0x000F) */
#define ADIOL100_WDGTIMEBASE_MSK        NO_OS_GENMASK(11, 10)
#define ADIOL100_WDGTIME_MSK            NO_OS_GENMASK(9, 4)
#define ADIOL100_WDGLOCK                NO_OS_BIT(3)
#define ADIOL100_WDGMODE                NO_OS_BIT(2)
#define ADIOL100_WDGENABLE              NO_OS_BIT(1)
#define ADIOL100_WDGCLEAR               NO_OS_BIT(0)

/* ClockCfg bit masks (0x1F01) */
#define ADIOL100_SPICRCEN               NO_OS_BIT(7)
#define ADIOL100_CLKOEN                 NO_OS_BIT(4)
#define ADIOL100_CLKDIV_MSK             NO_OS_GENMASK(3, 2)
#define ADIOL100_CLOCKCFG_MSK           NO_OS_GENMASK(1, 0)

/* CQCurLim bit masks (0x000A / 0x001A) */
#define ADIOL100_CQVTHRH_MSK            NO_OS_GENMASK(15, 14)
#define ADIOL100_CQVTHRL_MSK            NO_OS_GENMASK(13, 12)
#define ADIOL100_RXVTHRH_MSK            NO_OS_GENMASK(11, 10)
#define ADIOL100_RXVTHRL_MSK            NO_OS_GENMASK(9, 8)
#define ADIOL100_CQCL_MSK               NO_OS_GENMASK(7, 5)
#define ADIOL100_CLBL_MSK               NO_OS_GENMASK(4, 3)
#define ADIOL100_ARTTMO_MSK             NO_OS_GENMASK(2, 1)
#define ADIOL100_ARTEN                  NO_OS_BIT(0)

/* USER_paged registers — CQ external FET protection, channel A */
#define ADIOL100_REG_CQFETPWRCFG_A      0x1F18
#define ADIOL100_REG_CQCURRENTCFG_A     0x1F19
#define ADIOL100_REG_CQFETPROTECT_A     0x1F1A
#define ADIOL100_REG_CQSLOPE_A          0x1F1B
#define ADIOL100_REG_CQRETRY_A          0x1F1C

/* USER_paged registers — CQ external FET protection, channel B */
#define ADIOL100_REG_CQFETPWRCFG_B      0x1F28
#define ADIOL100_REG_CQCURRENTCFG_B     0x1F29
#define ADIOL100_REG_CQFETPROTECT_B     0x1F2A
#define ADIOL100_REG_CQSLOPE_B          0x1F2B
#define ADIOL100_REG_CQRETRY_B          0x1F2C

/* CQFETPwrCfg bit masks (0x1F18 / 0x1F28) */
#define ADIOL100_CQFETPWRMAX_MSK        NO_OS_GENMASK(7, 0)

/* CQCurrentCfg bit masks (0x1F19 / 0x1F29) */
#define ADIOL100_CQCURRMAX_MSK          NO_OS_GENMASK(15, 8)
#define ADIOL100_CQCURRMIN_MSK          NO_OS_GENMASK(7, 0)

/* CQFETProtect bit masks (0x1F1A / 0x1F2A) */
#define ADIOL100_CQOLTIMOUT_MSK         NO_OS_GENMASK(15, 8)
#define ADIOL100_CQCLTIMOUT_MSK         NO_OS_GENMASK(7, 0)

/* CQFETSlope bit masks (0x1F1B / 0x1F2B) */
#define ADIOL100_CQFETSLOPE_MSK         NO_OS_GENMASK(15, 8)
#define ADIOL100_CQFETSLOPEBL_MSK       NO_OS_GENMASK(7, 0)

/* CQRetry bit masks (0x1F1C / 0x1F2C) */
#define ADIOL100_CQARTIM_MSK            NO_OS_GENMASK(13, 8)
#define ADIOL100_CQAR_MSK               NO_OS_GENMASK(3, 0)

/* ADC data registers — global */
#define ADIOL100_REG_V24VOLT            0x1F30
#define ADIOL100_REG_V24VOLTMIN         0x1F31
#define ADIOL100_REG_V24VOLTMAX         0x1F32
#define ADIOL100_REG_TEMP               0x1F33
#define ADIOL100_REG_TEMPMIN            0x1F34
#define ADIOL100_REG_TEMPMAX            0x1F35

/* ADC data registers — channel A */
#define ADIOL100_REG_CQAVOLT            0x1F40
#define ADIOL100_REG_CQAVOLTMIN         0x1F41
#define ADIOL100_REG_CQAVOLTMAX         0x1F42
#define ADIOL100_REG_CQACUR             0x1F43
#define ADIOL100_REG_CQACURMIN          0x1F44
#define ADIOL100_REG_CQACURMAX          0x1F45
#define ADIOL100_REG_LPAVOLT            0x1F46
#define ADIOL100_REG_LPAVOLTMIN         0x1F47
#define ADIOL100_REG_LPAVOLTMAX         0x1F48
#define ADIOL100_REG_LPACUR             0x1F49
#define ADIOL100_REG_LPACURMIN          0x1F4A
#define ADIOL100_REG_LPACURMAX          0x1F4B

/* ADC data registers — channel B */
#define ADIOL100_REG_CQBVOLT            0x1F50
#define ADIOL100_REG_CQBVOLTMIN         0x1F51
#define ADIOL100_REG_CQBVOLTMAX         0x1F52
#define ADIOL100_REG_CQBCUR             0x1F53
#define ADIOL100_REG_CQBCURMIN          0x1F54
#define ADIOL100_REG_CQBCURMAX          0x1F55
#define ADIOL100_REG_LPBVOLT            0x1F56
#define ADIOL100_REG_LPBVOLTMIN         0x1F57
#define ADIOL100_REG_LPBVOLTMAX         0x1F58
#define ADIOL100_REG_LPBCUR             0x1F59
#define ADIOL100_REG_LPBCURMIN          0x1F5A
#define ADIOL100_REG_LPBCURMAX          0x1F5B

/* CQMeasConf register (0x1F5F) — reset min/max statistics */
#define ADIOL100_REG_CQMEASCONF         0x1F5F
#define ADIOL100_TEMPRESSTAT            NO_OS_BIT(5)
#define ADIOL100_RESV24STAT             NO_OS_BIT(4)
#define ADIOL100_RESLPSTATB             NO_OS_BIT(3)
#define ADIOL100_RESLPSTATA             NO_OS_BIT(2)
#define ADIOL100_RESCQSTATB             NO_OS_BIT(1)
#define ADIOL100_RESCQSTATA             NO_OS_BIT(0)

/* ADC data mask (13-bit, bits [12:0]) */
#define ADIOL100_ADC_DATA_MSK           NO_OS_GENMASK(12, 0)

/* LED registers */
#define ADIOL100_REG_LED12BGT           0x1F60
#define ADIOL100_REG_LED1RSEQ           0x1F61
#define ADIOL100_REG_LED1GSEQ           0x1F62
#define ADIOL100_REG_LED2RSEQ           0x1F63
#define ADIOL100_REG_LED2GSEQ           0x1F64
#define ADIOL100_REG_LED34BGT           0x1F65
#define ADIOL100_REG_LED3RSEQ           0x1F66
#define ADIOL100_REG_LED3GSEQ           0x1F67
#define ADIOL100_REG_LED4RSEQ           0x1F68
#define ADIOL100_REG_LED4GSEQ           0x1F69
#define ADIOL100_REG_LEDCONFIG          0x1F6A

/* LEDConfig bit masks (0x1F6A) */
#define ADIOL100_LED4CFG_MSK            NO_OS_GENMASK(14, 12)
#define ADIOL100_LED3CFG_MSK            NO_OS_GENMASK(10, 8)
#define ADIOL100_LED2CFG_MSK            NO_OS_GENMASK(6, 4)
#define ADIOL100_LED1CFG_MSK            NO_OS_GENMASK(2, 0)

/* Device IDs */
#define ADIOL100_DEVICE_ID              0x0100
#define ADIOL101_DEVICE_ID              0x0101

/* FIFO constants */
#define ADIOL100_FIFO_MAX_LEN           67

/* R/W direction */
#define ADIOL100_READ                   1
#define ADIOL100_WRITE                  0

/* Channel selection. */
enum adiol100_channel {
	ADIOL100_CH_A = 0,
	ADIOL100_CH_B = 1,
};

/* CQ driver current limit (CQCurLim register, CQ_CL[2:0]). */
enum adiol100_cq_current_limit {
	ADIOL100_CQCL_10MA  = 0,
	ADIOL100_CQCL_15MA  = 1,
	ADIOL100_CQCL_20MA  = 2,
	ADIOL100_CQCL_50MA  = 3,
	ADIOL100_CQCL_100MA = 4,
	ADIOL100_CQCL_200MA = 5,
	ADIOL100_CQCL_300MA = 6,
	ADIOL100_CQCL_500MA = 7,
};

/* CQ current limit blanking time. */
enum adiol100_blanking_time {
	ADIOL100_CLBL_150US = 0,
	ADIOL100_CLBL_500US = 1,
	ADIOL100_CLBL_1MS   = 2,
	ADIOL100_CLBL_5MS   = 3,
};

/* CQ autoretry timeout after current limit event. */
enum adiol100_autoretry_timeout {
	ADIOL100_ARTTMO_5MS   = 0,
	ADIOL100_ARTTMO_10MS  = 1,
	ADIOL100_ARTTMO_50MS  = 2,
	ADIOL100_ARTTMO_100MS = 3,
};

/* CQ/RX voltage threshold selection. */
enum adiol100_voltage_threshold {
	ADIOL100_VTHR_2V = 0,
	ADIOL100_VTHR_4V = 1,
	ADIOL100_VTHR_6V = 2,
	ADIOL100_VTHR_8V = 3,
};

/* CQ driver output mode. */
enum adiol100_cq_mode {
	ADIOL100_CQ_NPN      = 0,
	ADIOL100_CQ_PUSHPULL = 1,
};

/* CQ driver enable/disable. */
enum adiol100_cq_drv {
	ADIOL100_CQ_DRV_DIS = 0,
	ADIOL100_CQ_DRV_EN  = 1,
};

/* CQ autoretry enable/disable. */
enum adiol100_autoretry {
	ADIOL100_AUTORETRY_DIS = 0,
	ADIOL100_AUTORETRY_EN  = 1,
};

/* L+ sensor supply enable/disable. */
enum adiol100_lp_en {
	ADIOL100_LP_DIS = 0,
	ADIOL100_LP_EN  = 1,
};

/* L+ reverse polarity protection enable/disable. */
enum adiol100_lp_rev {
	ADIOL100_LP_REV_DIS = 0,
	ADIOL100_LP_REV_EN  = 1,
};

/* IO-Link checksum insertion enable/disable. */
enum adiol100_ins_chks {
	ADIOL100_CHKS_DIS = 0,
	ADIOL100_CHKS_EN  = 1,
};

/* IO-Link framer enable/disable. */
enum adiol100_framer {
	ADIOL100_FRAMER_DIS = 0,
	ADIOL100_FRAMER_EN  = 1,
};

/* Watchdog timer time base. */
enum adiol100_wdg_timebase {
	ADIOL100_WDG_100US = 0,
	ADIOL100_WDG_500US = 1,
	ADIOL100_WDG_2MS   = 2,
};

/* Watchdog mode: SPI activity or explicit clear. */
enum adiol100_wdg_mode {
	ADIOL100_WDG_MODE_SPI   = 0,
	ADIOL100_WDG_MODE_CLEAR = 1,
};

/* Watchdog enable/disable. */
enum adiol100_wdg_en {
	ADIOL100_WDG_DIS = 0,
	ADIOL100_WDG_EN  = 1,
};

/* Watchdog lock (prevents further config changes). */
enum adiol100_wdg_lock {
	ADIOL100_WDG_UNLOCKED = 0,
	ADIOL100_WDG_LOCKED   = 1,
};

/* TxFIFO message retention for cyclic operation. */
enum adiol100_keep_msg {
	ADIOL100_DISCARD_MSG = 0,
	ADIOL100_KEEP_MSG    = 1,
};

/* CQ driver slew rate. */
enum adiol100_cq_slew_rate {
	ADIOL100_CQSLEW_250NS  = 0,
	ADIOL100_CQSLEW_500NS  = 1,
	ADIOL100_CQSLEW_1250NS = 2,
	ADIOL100_CQSLEW_5000NS = 3,
};

/* CQ input sink current selection. */
enum adiol100_sink_sel {
	ADIOL100_SINKSEL_OFF   = 0,
	ADIOL100_SINKSEL_5MA   = 1,
	ADIOL100_SINKSEL_2MA   = 2,
	ADIOL100_SINKSEL_150UA = 3,
};

/* Clock source selection. */
enum adiol100_clock_src {
	ADIOL100_CLK_INTERNAL = 0,
	ADIOL100_CLK_CRYSTAL  = 1,
	ADIOL100_CLK_EXTERNAL = 2,
};

/* Global ADC sources (V24 supply voltage, die temperature). */
enum adiol100_adc_global {
	ADIOL100_ADC_V24_VOLT     = 0x1F30,
	ADIOL100_ADC_V24_VOLT_MIN = 0x1F31,
	ADIOL100_ADC_V24_VOLT_MAX = 0x1F32,
	ADIOL100_ADC_TEMP         = 0x1F33,
	ADIOL100_ADC_TEMP_MIN     = 0x1F34,
	ADIOL100_ADC_TEMP_MAX     = 0x1F35,
};

/* Per-channel ADC sources (CQ/LP voltage and current).
 * Values are channel A register addresses; channel B = A + 0x10. */
enum adiol100_adc_chan {
	ADIOL100_ADC_CQ_VOLT      = 0x1F40,
	ADIOL100_ADC_CQ_VOLT_MIN  = 0x1F41,
	ADIOL100_ADC_CQ_VOLT_MAX  = 0x1F42,
	ADIOL100_ADC_CQ_CUR       = 0x1F43,
	ADIOL100_ADC_CQ_CUR_MIN   = 0x1F44,
	ADIOL100_ADC_CQ_CUR_MAX   = 0x1F45,
	ADIOL100_ADC_LP_VOLT      = 0x1F46,
	ADIOL100_ADC_LP_VOLT_MIN  = 0x1F47,
	ADIOL100_ADC_LP_VOLT_MAX  = 0x1F48,
	ADIOL100_ADC_LP_CUR       = 0x1F49,
	ADIOL100_ADC_LP_CUR_MIN   = 0x1F4A,
	ADIOL100_ADC_LP_CUR_MAX   = 0x1F4B,
};

/* LED index (1–4). */
enum adiol100_led {
	ADIOL100_LED1 = 0,
	ADIOL100_LED2 = 1,
	ADIOL100_LED3 = 2,
	ADIOL100_LED4 = 3,
};

/* LED operating mode (LEDConfig register, 3-bit field per LED). */
enum adiol100_led_mode {
	ADIOL100_LED_BLINK      = 0,
	ADIOL100_LED_BLINK_EDGE = 1,
	ADIOL100_LED_ON_HIGH    = 2,
	ADIOL100_LED_ON_LOW     = 3,
	ADIOL100_LED_BLINK_FAST = 4,
};

/* LED brightness (percentage of 50% duty cycle, 16 steps). */
enum adiol100_led_brightness {
	ADIOL100_LED_BGT_6    = 0,
	ADIOL100_LED_BGT_12   = 1,
	ADIOL100_LED_BGT_18   = 2,
	ADIOL100_LED_BGT_25   = 3,
	ADIOL100_LED_BGT_31   = 4,
	ADIOL100_LED_BGT_37   = 5,
	ADIOL100_LED_BGT_43   = 6,
	ADIOL100_LED_BGT_50   = 7,
	ADIOL100_LED_BGT_56   = 8,
	ADIOL100_LED_BGT_62   = 9,
	ADIOL100_LED_BGT_68   = 10,
	ADIOL100_LED_BGT_75   = 11,
	ADIOL100_LED_BGT_81   = 12,
	ADIOL100_LED_BGT_87   = 13,
	ADIOL100_LED_BGT_93   = 14,
	ADIOL100_LED_BGT_100  = 15,
};

/* Common LED blink sequences. Each bit = 63ms, scanned MSB first, 16-bit cycle.
 * Pass any uint16_t for custom patterns. */
enum adiol100_led_seq {
	ADIOL100_LED_SEQ_OFF        = 0x0000,
	ADIOL100_LED_SEQ_ON         = 0xFFFF,
	ADIOL100_LED_SEQ_BLINK_SLOW = 0xFF00,  /* 504ms on, 504ms off */
	ADIOL100_LED_SEQ_BLINK_FAST = 0xF0F0,  /* 252ms on, 252ms off */
	ADIOL100_LED_SEQ_PULSE      = 0x8000,  /* 63ms on, 945ms off */
};

/* IRQ callback type — called from ISR context, must be fast. */
typedef void (*adiol100_irq_cb_t)(void *ctx);

/**
 * @struct adiol100_init_param
 * @brief Initialization parameters for the ADIOL100 driver.
 */
struct adiol100_init_param {
	/* SPI initialization parameters. */
	struct no_os_spi_init_param *spi_ip;
	/* Device address set by ADRSEL pin (0–3). */
	uint8_t chip_addr;
	/* Clock source (see enum adiol100_clock_src). */
	uint8_t clock_src;
	/* Clock divider (ClkDiv[1:0]). */
	uint8_t clk_div;
	/* Optional IRQ GPIO for global interrupts (NULL = no driver-managed IRQ). */
	struct no_os_gpio_init_param *irq_gpio_g;
	/* Optional IRQ GPIO for channel A (NULL = no driver-managed IRQ). */
	struct no_os_gpio_init_param *irq_gpio_a;
	/* Optional IRQ GPIO for channel B (NULL = no driver-managed IRQ). */
	struct no_os_gpio_init_param *irq_gpio_b;
	/* IRQ controller init (required if any irq_gpio is set). */
	struct no_os_irq_init_param *irq_ip;
	/* NVIC IRQ controller init (required if any irq_gpio is set). */
	struct no_os_irq_init_param *nvic_ip;
	/* NVIC IRQ vector number (e.g. GPIO0_IRQn). Required if nvic_ip is set. */
	uint32_t nvic_irq_id;
};

/**
 * @struct adiol100_dev
 * @brief ADIOL100 device descriptor, created by adiol100_init().
 */
struct adiol100_dev {
	/* SPI descriptor. */
	struct no_os_spi_desc *spi_desc;
	/* Device address set by ADRSEL pin (0–3). */
	uint8_t chip_addr;
	/* Auto-incrementing FIFO message ID. */
	uint8_t msg_id;
	/* IRQ GPIO descriptors (NULL if not configured). */
	struct no_os_gpio_desc *irq_gpio_g;
	struct no_os_gpio_desc *irq_gpio_a;
	struct no_os_gpio_desc *irq_gpio_b;
	/* IRQ controller descriptor (NULL if not configured). */
	struct no_os_irq_ctrl_desc *irq_ctrl;
	/* NVIC IRQ controller descriptor (NULL if not configured). */
	struct no_os_irq_ctrl_desc *nvic_ctrl;
	/* Per-channel user callbacks and contexts. */
	adiol100_irq_cb_t irq_cb[ADIOL100_NUM_CHANNELS];
	void *irq_cb_ctx[ADIOL100_NUM_CHANNELS];
	/* Global IRQ callback and context. */
	adiol100_irq_cb_t irq_cb_g;
	void *irq_cb_ctx_g;
};

/** Read the chip RevisionID register. */
int adiol100_read_revid(struct adiol100_dev *dev, uint16_t *rev_id);

/** Read the chip DeviceID register. */
int adiol100_read_devid(struct adiol100_dev *dev, uint16_t *dev_id);

/** Configure the clock source, divider, and enable clock output. */
int adiol100_setup_clock(struct adiol100_dev *dev,
			 uint8_t clock_src, uint8_t clk_div);

/** Initialize the ADIOL100 device. */
int adiol100_init(struct adiol100_dev **dev, struct adiol100_init_param *ip);

/** Free resources allocated by adiol100_init(). */
int adiol100_remove(struct adiol100_dev *dev);

/** Read a 16-bit register. Handles paged access automatically. */
int adiol100_read(struct adiol100_dev *dev, uint16_t reg, uint16_t *value);

/** Write a 16-bit register. Handles paged access automatically. */
int adiol100_write(struct adiol100_dev *dev, uint16_t reg, uint16_t value);

/** Read-modify-write a register using a bit mask. */
int adiol100_update(struct adiol100_dev *dev, uint16_t reg, uint16_t mask,
		    uint16_t value);

/** Load IO-Link payload into TxFIFO without triggering CQSend. */
int adiol100_load_msg(struct adiol100_dev *dev, enum adiol100_channel ch,
		      uint8_t *data, uint8_t txbytes, uint8_t rxbytes,
		      enum adiol100_keep_msg keep);

/** Assert CQSend to trigger transmission of the message in the TxFIFO. */
int adiol100_send_msg(struct adiol100_dev *dev, enum adiol100_channel ch);

/** Load IO-Link payload into TxFIFO and trigger CQSend. */
int adiol100_load_and_send_msg(struct adiol100_dev *dev,
			       enum adiol100_channel ch,
			       uint8_t *data, uint8_t txbytes, uint8_t rxbytes,
			       enum adiol100_keep_msg keep);

/** Read device response from RxFIFO, stripping the MsgID/RxBytesAct header. */
int adiol100_read_msg(struct adiol100_dev *dev, enum adiol100_channel ch,
		      uint8_t *data, uint8_t len);

/** Turn off all four LEDs by clearing their sequences and resetting modes. */
int adiol100_reset_leds(struct adiol100_dev *dev);

/** Disable global interrupts, clear trigger, read-clear all IRQ registers, and turn off LEDs. */
int adiol100_global_reset(struct adiol100_dev *dev);

/** Reset the transmit FIFO for a channel. */
int adiol100_reset_tx_fifo(struct adiol100_dev *dev, enum adiol100_channel ch);

/** Reset the receive FIFO for a channel. */
int adiol100_reset_rx_fifo(struct adiol100_dev *dev, enum adiol100_channel ch);

/** Assert CHANRST to reset all channel-specific registers. */
int adiol100_reset_channel(struct adiol100_dev *dev, enum adiol100_channel ch);

/** Configure the CQ driver: output mode, enable, sink current, slew rate. */
int adiol100_config_cq(struct adiol100_dev *dev, enum adiol100_channel ch,
		       enum adiol100_cq_mode mode,
		       enum adiol100_cq_drv drv_en,
		       enum adiol100_sink_sel sink_sel,
		       enum adiol100_cq_slew_rate slew_rate);

/** Configure CQ protection: current limit, blanking, thresholds, autoretry. */
int adiol100_config_cq_protection(struct adiol100_dev *dev,
				  enum adiol100_channel ch,
				  enum adiol100_cq_current_limit current_limit,
				  enum adiol100_blanking_time blanking,
				  enum adiol100_autoretry_timeout retry_tmo,
				  enum adiol100_autoretry retry_en,
				  enum adiol100_voltage_threshold tx_vthr_h,
				  enum adiol100_voltage_threshold tx_vthr_l,
				  enum adiol100_voltage_threshold rx_vthr_h,
				  enum adiol100_voltage_threshold rx_vthr_l);

/** Configure the L+ sensor supply: enable and reverse polarity protection. */
int adiol100_config_lp(struct adiol100_dev *dev, enum adiol100_channel ch,
		       enum adiol100_lp_en enable,
		       enum adiol100_lp_rev rev_en);

/** Configure L+ FET protection: current, power, timeouts, slope, retry. */
int adiol100_config_lp_protection(struct adiol100_dev *dev,
				  enum adiol100_channel ch,
				  uint8_t cl_nom, uint8_t pwr_max,
				  uint8_t curr_max, uint8_t curr_min,
				  uint8_t ol_timeout, uint8_t cl_timeout,
				  uint8_t slope, uint8_t slope_bl,
				  uint8_t ar_time, uint8_t ar_count);

/** Configure CQ external high-side FET protection: current, power, timeouts, slope, retry. */
int adiol100_config_cq_protection_ext(struct adiol100_dev *dev,
				      enum adiol100_channel ch,
				      uint8_t cl_nom, uint8_t pwr_max,
				      uint8_t curr_max, uint8_t curr_min,
				      uint8_t ol_timeout, uint8_t cl_timeout,
				      uint8_t slope, uint8_t slope_bl,
				      uint8_t ar_time, uint8_t ar_count);

/** Configure the IO-Link framer: checksum insertion and framer enable. */
int adiol100_config_framer(struct adiol100_dev *dev, enum adiol100_channel ch,
			   enum adiol100_ins_chks ins_chks,
			   enum adiol100_framer framer_en);

/** Set the SPI burst length for FIFO access. */
int adiol100_set_burst_len(struct adiol100_dev *dev, enum adiol100_channel ch,
			   int write_len, int read_len);

/** Get the communication rate determined by EstCom (COM1/2/3). */
int adiol100_get_comrt(struct adiol100_dev *dev, enum adiol100_channel ch,
		       uint8_t *comrt);

/** Set the IO-Link cycle timer value (IO-Link encoded byte). */
int adiol100_set_cycle_tmr(struct adiol100_dev *dev, enum adiol100_channel ch,
			   uint8_t value);

/** Get the current cycle timer register value. */
int adiol100_get_cycle_tmr(struct adiol100_dev *dev, enum adiol100_channel ch,
			   uint16_t *value);

/** Enable the cycle timer for autonomous cyclic operation. */
int adiol100_enable_cycle_timer(struct adiol100_dev *dev,
				enum adiol100_channel ch);

/** Disable the cycle timer. */
int adiol100_disable_cycle_timer(struct adiol100_dev *dev,
				 enum adiol100_channel ch);

/** Read and clear the channel interrupt register. */
int adiol100_get_channel_irq(struct adiol100_dev *dev, enum adiol100_channel ch,
			     uint16_t *flags);

/** Read and clear the global interrupt register. */
int adiol100_get_global_irq(struct adiol100_dev *dev, uint16_t *flags);

/** Set the global interrupt enable mask. */
int adiol100_enable_global_irq(struct adiol100_dev *dev, uint16_t mask);

/** Set the channel interrupt enable mask. */
int adiol100_enable_channel_irq(struct adiol100_dev *dev,
				enum adiol100_channel ch, uint16_t mask);

/** Read the RxFIFO status register (level + error flags). */
int adiol100_get_fifo_status(struct adiol100_dev *dev, enum adiol100_channel ch,
			     uint16_t *flags);

/** Read the channel status register. */
int adiol100_get_status(struct adiol100_dev *dev, enum adiol100_channel ch,
			uint16_t *flags);

/** Configure the watchdog timer: time base, period, mode, enable, lock. */
int adiol100_config_watchdog(struct adiol100_dev *dev,
			     enum adiol100_wdg_timebase timebase,
			     uint8_t time,
			     enum adiol100_wdg_mode mode,
			     enum adiol100_wdg_en enable,
			     enum adiol100_wdg_lock lock);

/** Clear the watchdog timer (write-1-to-clear). */
int adiol100_clear_watchdog(struct adiol100_dev *dev);

/** Register an IRQ callback for a channel (called from ISR context). */
int adiol100_register_irq_callback(struct adiol100_dev *dev,
				   enum adiol100_channel ch,
				   adiol100_irq_cb_t cb,
				   void *ctx);

/** Register an IRQ callback for global interrupts (called from ISR context). */
int adiol100_register_global_irq_callback(struct adiol100_dev *dev,
		adiol100_irq_cb_t cb,
		void *ctx);

/** Run EstablishCommunication: wake-up, poll for success/fail, return baud. */
int adiol100_estcom(struct adiol100_dev *dev, enum adiol100_channel ch);

/** Read a raw 13-bit global ADC value (V24 voltage or temperature). */
int adiol100_read_adc_global(struct adiol100_dev *dev,
			     enum adiol100_adc_global source, uint16_t *value);

/** Read a raw 13-bit per-channel ADC value (CQ/LP voltage or current). */
int adiol100_read_adc_chan(struct adiol100_dev *dev,
			   enum adiol100_adc_chan source,
			   enum adiol100_channel ch, uint16_t *value);

/** Reset all ADC min/max statistics (V24, Temp, CQ and LP for both channels). */
int adiol100_reset_adc_stats(struct adiol100_dev *dev);

/** Set the brightness for one LED. */
int adiol100_set_led_brightness(struct adiol100_dev *dev, enum adiol100_led led,
				enum adiol100_led_brightness red_bgt,
				enum adiol100_led_brightness green_bgt);

/** Set the blink sequence for one LED (16-bit pattern, each bit = 63ms). */
int adiol100_set_led_sequence(struct adiol100_dev *dev, enum adiol100_led led,
			      enum adiol100_led_seq red_seq,
			      enum adiol100_led_seq green_seq);

/** Set the operating mode for one LED. */
int adiol100_set_led_mode(struct adiol100_dev *dev, enum adiol100_led led,
			  enum adiol100_led_mode mode);

#endif
