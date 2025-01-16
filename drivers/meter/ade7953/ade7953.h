/***************************************************************************//**
 *   @file   ade7953.h
 *   @brief  Header file of ADE7953 Driver.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
#ifndef __ADE7953_H__
#define __ADE7953_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_irq.h"
#include "no_os_delay.h"
#include "no_os_units.h"
#include "no_os_alloc.h"
#include "no_os_crc16.h"
#include "no_os_print_log.h"
#include <stdlib.h>
#include <errno.h>
#include <math.h>

/* SPI commands */
#define ADE7953_SPI_READ					NO_OS_BIT(7)

/* Version product */
#define ADE7953_VERSION						0x40
#define ADE7953_REG_VERSION_PRODUCT     			0x004

/* Miscellaneous Definitions */
#define IS_8BITS_REG(x) ((x >= ADE7953_REG_SAGCYC && x <= ADE7953_REG_LAST_RWDATA) \
        || (x == ADE7953_REG_VERSION) || (x == ADE7953_REG_EX_REF))
#define IS_16BITS_REG(x) ((x >= ADE7953_REG_ZXTOUT && x <= ADE7953_REG_LAST_RWDATA_16))

/* Key needed to unlock register 0x120 */
#define ADE7953_UNLOCK_KEY             				0xAD
/* Value that needs to be write to the 0x120 register */
#define ADE7953_OPT_SETT                			0x30

/* Reset Delay */
#define ADE7953_RESET_DEL					100

/* ENABLE and DISABLE */
#define ENABLE							1u
#define DISABLE							0u

/* ADE7953 Register Map */
/* 8 BIT REGISTERS */
#define ADE7953_REG_SAGCYC					0x000
#define ADE7953_REG_DISNOLOAD					0x001
#define ADE7953_REG_LCYCMODE					0x004
#define ADE7953_REG_PGA_V					0x007
#define ADE7953_REG_PGA_IA					0x008
#define ADE7953_REG_PGA_IB					0x009
#define ADE7953_REG_WRITE_PROTECT				0x040
#define ADE7953_REG_LAST_OP					0x0FD
#define ADE7953_REG_UNLOCK_120              			0x0FE
#define ADE7953_REG_LAST_RWDATA					0x0FF
#define ADE7953_REG_VERSION					0x702
#define ADE7953_REG_EX_REF					0x800
/* 16 BIT REGISTERS	*/
#define ADE7953_REG_ZXTOUT					0x100
#define ADE7953_REG_LINECYC					0x101
#define ADE7953_REG_CONFIG					0x102
#define ADE7953_REG_CF1DEN					0x103
#define ADE7953_REG_CF2DEN					0x104
#define ADE7953_REG_CFMODE					0x107
#define ADE7953_REG_PHCALA					0x108
#define ADE7953_REG_PHCALB					0x109
#define ADE7953_REG_PFA						0x10A
#define ADE7953_REG_PFB						0x10B
#define ADE7953_REG_ANGLE_A					0x10C
#define ADE7953_REG_ANGLE_B					0x10D
#define ADE7953_REG_PERIOD					0x10E
#define ADE7953_REG_ALT_OUTPUT					0x110
#define ADE7953_REG_LAST_ADD					0x1FE
#define ADE7953_REG_LAST_RWDATA_16				0x1FF
#define ADE7953_REG_RESERVED					0x120
/* 24 BIT REGISTERS */
#define ADE7953_REG_SAGLVL					0x200
#define ADE7953_REG_ACCMODE					0x201
#define ADE7953_REG_AP_NOLOAD					0x203
#define ADE7953_REG_VAR_NOLOAD					0x204
#define ADE7953_REG_VA_NOLOAD					0x205
#define ADE7953_REG_AVA						0x210
#define ADE7953_REG_BVA						0x211
#define ADE7953_REG_AWATT					0x212
#define ADE7953_REG_BWATT					0x213
#define ADE7953_REG_AVAR					0x214
#define ADE7953_REG_BVAR					0x215
#define ADE7953_REG_IA						0x216
#define ADE7953_REG_IB						0x217
#define ADE7953_REG_V						0x218
#define ADE7953_REG_IRMSA					0x21A
#define ADE7953_REG_IRMSB					0x21B
#define ADE7953_REG_VRMS					0x21C
#define ADE7953_REG_AENERGYA					0x21E
#define ADE7953_REG_AENERGYB					0x21F
#define ADE7953_REG_RENERGYA					0x220
#define ADE7953_REG_RENERGYB					0x221
#define ADE7953_REG_APENERGYA					0x222
#define ADE7953_REG_APENERGYB					0x223
#define ADE7953_REG_OVLVL					0x224
#define ADE7953_REG_OILVL					0x225
#define ADE7953_REG_VPEAK					0x226
#define ADE7953_REG_RSTVPEAK					0x227
#define ADE7953_REG_IAPEAK					0x228
#define ADE7953_REG_RSTIAPEAK					0x229
#define ADE7953_REG_IBPEAK					0x22A
#define ADE7953_REG_RSTIBPEAK					0x22B
#define ADE7953_REG_IRQENA					0x22C
#define ADE7953_REG_IRQSTATA					0x22D
#define ADE7953_REG_RSTIRQSTATA					0x22E
#define ADE7953_REG_IRQENB					0x22F
#define ADE7953_REG_IRQSTATB					0x230
#define ADE7953_REG_RSTIRQSTATB					0x231
#define ADE7953_REG_AIGAIN					0x280
#define ADE7953_REG_AVGAIN					0x281
#define ADE7953_REG_AWGAIN					0x282
#define ADE7953_REG_AVARGAIN					0x283
#define ADE7953_REG_AVAGAIN					0x284
#define ADE7953_REG_AIRMSOS					0x286
#define ADE7953_REG_VRMSOS					0x288
#define ADE7953_REG_AWATTOS					0x289
#define ADE7953_REG_AVAROS					0x28A
#define ADE7953_REG_AVAOS					0x28B
#define ADE7953_REG_BIGAIN					0x28C
#define ADE7953_REG_BVGAIN					0x28D
#define ADE7953_REG_BWGAIN					0x28E
#define ADE7953_REG_BVARGAIN					0x28F
#define ADE7953_REG_BVAGAIN					0x290
#define ADE7953_REG_BIRMSOS					0x292
#define ADE7953_REG_BWATTOS					0x295
#define ADE7953_REG_BVAROS					0x296
#define ADE7953_REG_BVAOS					0x297
#define ADE7953_REG_LAST_RWDATA_24				0x2FF
/* 32 BIT REGISTERS */
/* Same registers as 24 bit registers +0x100 address offset*/
#define ADE7953_REG_CRC						0x37F

/* ADE7953_REG_DISNOLOAD Bit Definition */
#define ADE7953_DIS_VANLOAD_MSK					NO_OS_BIT(2)
#define ADE7953_DIS_VARNLOAD_MSK				NO_OS_BIT(1)
#define ADE7953_DIS_APNLOAD_MSK					NO_OS_BIT(0)

/* ADE7953_REG_LCYCMODE Bit Definition */
#define ADE7953_RSTREAD_MSK					NO_OS_BIT(6)
#define ADE7953_BLVA_MSK					NO_OS_BIT(5)
#define ADE7953_ALVA_MSK					NO_OS_BIT(4)
#define ADE7953_BLVAR_MSK					NO_OS_BIT(3)
#define ADE7953_ALVAR_MSK					NO_OS_BIT(2)
#define ADE7953_BLWATT_MSK					NO_OS_BIT(1)
#define ADE7953_ALWATT_MSK					NO_OS_BIT(0)

/* ADE7953_REG_PGA_V Bit Definition */
#define ADE7953_PGA_V_MSK					NO_OS_GENMASK(2, 0)

/* ADE7953_REG_PGA_IA / ADE7953_REG_PGA_IB Bit Definition */
#define ADE7953_PGA_IA_MSK					NO_OS_GENMASK(2, 0)

/* ADE7953_REG_CONFIG Bit Definition */
#define ADE7953_COMM_LOCK_MSK					NO_OS_BIT(15)
#define ADE7953_ZX_EDGE_MSK					NO_OS_GENMASK(13, 12)
#define ADE7953_ZX_I_MSK					NO_OS_BIT(11)
#define ADE7953_CRC_ENABLE_MSK					NO_OS_BIT(8)
#define ADE7953_SWRST_MSK					NO_OS_BIT(7)
#define ADE7953_ZXLPF_MSK					NO_OS_BIT(6)
#define ADE7953_REV_PULSE_MSK					NO_OS_BIT(5)
#define ADE7953_REVP_CF_MSK					NO_OS_BIT(4)
#define ADE7953_PFMODE_MSK					NO_OS_BIT(3)
#define ADE7953_HPFEN_MSK					NO_OS_BIT(2)
#define ADE7953_INTENB_MSK					NO_OS_BIT(1)
#define ADE7953_INTENA_MSK					NO_OS_BIT(0)

/* ADE7953_REG_CFMODE Bit Definition */
#define ADE7953_CF2DIS_MSK					NO_OS_BIT(9)
#define ADE7953_CF1DIS_MSK					NO_OS_BIT(8)
#define ADE7953_CF2SEL_MSK					NO_OS_GENMASK(7, 4)
#define ADE7953_CF1SEL_MSK					NO_OS_GENMASK(3, 0)

/* ADE7953_REG_ALT_OUTPUT Bit Definition */
#define ADE7953_REVP_ALT_MSK					NO_OS_GENMASK(11, 8)
#define ADE7953_ZXI_ALT_MSK					NO_OS_GENMASK(7, 4)
#define ADE7953_ZX_ALT_MSK					NO_OS_GENMASK(3, 0)

/* ADE7953_REG_ACCMODE Bit Definition */
#define ADE7953_VARNLOAD_B_MSK					NO_OS_BIT(21)
#define ADE7953_VANLOAD_B_MSK					NO_OS_BIT(20)
#define ADE7953_ACTNLOAD_B_MSK					NO_OS_BIT(19)
#define ADE7953_VARNLOAD_A_MSK					NO_OS_BIT(18)
#define ADE7953_VANLOAD_A_MSK					NO_OS_BIT(17)
#define ADE7953_ACTNLOAD_A_MSK					NO_OS_BIT(16)
#define ADE7953_VARSIGN_B_MSK					NO_OS_BIT(13)
#define ADE7953_VARSIGN_A_MSK					NO_OS_BIT(12)
#define ADE7953_APSIGN_B_MSK					NO_OS_BIT(11)
#define ADE7953_APSIGN_A_MSK					NO_OS_BIT(10)
#define ADE7953_BVAACC_MSK					NO_OS_BIT(9)
#define ADE7953_AVAACC_MSK					NO_OS_BIT(8)
#define ADE7953_BVARACC_MSK					NO_OS_GENMASK(7, 6)
#define ADE7953_AVARACC_MSK					NO_OS_GENMASK(5, 4)
#define ADE7953_BWATTACC_MSK					NO_OS_GENMASK(3, 2)
#define ADE7953_AWATTACC_MSK					NO_OS_GENMASK(1, 0)

/* ADE7953_REG_IRQENA / ADE7953_REG_IRQSTATA Bit Definition */
#define ADE7953_CRC_MSK						NO_OS_BIT(21)
#define ADE7953_RESET_MSK					NO_OS_BIT(20)
#define ADE7953_SAG_MSK						NO_OS_BIT(19)
#define ADE7953_CYCEND_MSK					NO_OS_BIT(18)
#define ADE7953_WSMP_MSK					NO_OS_BIT(17)
#define ADE7953_OV_MSK						NO_OS_BIT(16)
#define ADE7953_ZXV_MSK						NO_OS_BIT(15)
#define ADE7953_ZXTO_MSK					NO_OS_BIT(14)
#define ADE7953_OIA_MSK						NO_OS_BIT(13)
#define ADE7953_ZXIA_MSK					NO_OS_BIT(12)
#define ADE7953_ZXTO_IA_MSK					NO_OS_BIT(11)
#define ADE7953_VARSIGN_A_MSK					NO_OS_BIT(10)
#define ADE7953_APSIGN_A_MSK					NO_OS_BIT(9)
#define ADE7953_VA_NOLOADA_MSK					NO_OS_BIT(8)
#define ADE7953_VAR_NOLOADA_MSK					NO_OS_BIT(7)
#define ADE7953_AP_NOLOADA_MSK					NO_OS_BIT(6)
#define ADE7953_VAEOFA_MSK					NO_OS_BIT(5)
#define ADE7953_VAREOFA_MSK					NO_OS_BIT(4)
#define ADE7953_AEOFA_MSK					NO_OS_BIT(3)
#define ADE7953_VAEHFA_MSK					NO_OS_BIT(2)
#define ADE7953_VAREHFA_MSK					NO_OS_BIT(1)
#define ADE7953_AEHFA_MSK					NO_OS_BIT(0)

/* ADE7953_REG_IRQENB / ADE7953_REG_IRQSTATB Bit Definition */
#define ADE7953_OIB_MSK						NO_OS_BIT(13)
#define ADE7953_ZXIB_MSK					NO_OS_BIT(12)
#define ADE7953_ZXTO_IB_MSK					NO_OS_BIT(11)
#define ADE7953_VARSIGN_B_MSK					NO_OS_BIT(10)
#define ADE7953_APSIGN_B_MSK					NO_OS_BIT(9)
#define ADE7953_VA_NOLOADB_MSK					NO_OS_BIT(8)
#define ADE7953_VAR_NOLOADB_MSK					NO_OS_BIT(7)
#define ADE7953_AP_NOLOADB_MSK					NO_OS_BIT(6)
#define ADE7953_VAEOFB_MSK					NO_OS_BIT(5)
#define ADE7953_VAREOFB_MSK					NO_OS_BIT(4)
#define ADE7953_AEOFB_MSK					NO_OS_BIT(3)
#define ADE7953_VAEHFB_MSK					NO_OS_BIT(2)
#define ADE7953_VAREHFB_MSK					NO_OS_BIT(1)
#define ADE7953_AEHFB_MSK					NO_OS_BIT(0)

/* ADE7953_REG_WRITE_PROTECT Bit Definition */
#define ADE7953_24_32_BITS_PROTECT            			NO_OS_BIT(2)
#define ADE7953_16_BITS_PROTECT                 		NO_OS_BIT(1)
#define ADE7953_8_BITS_PROTECT                			NO_OS_BIT(0)

/**
 * @enum ade7953_i_ch_e
 * @brief ADE7953 current channel.
 */
enum ade7953_i_ch_e {
	/* Current channel selection */
	/* current ch A */
	ADE7953_I_CH1,
	/* current ch B */
	ADE7953_I_CH2
};

/**
 * @enum ade7953_cf_pin_e
 * @brief ADE7953 cf pin selection.
 */
enum ade7953_cf_pin_e {
	/* Current channel selection */
	/* CF 1 pin */
	ADE7953_CF1_PIN,
	/* CF 2 pin */
	ADE7953_CF2_PIN
};

/**
 * @enum ade7953_write_protect_e
 * @brief ADE7953 write protect select.
 */
enum ade7953_write_protect_e {
	/* Select the registers that have write protect enabled */
	/* 8 bits regs */
	ADE7953_8BITS_REGS,
	/* 16 bits regs */
	ADE7953_16BITS_REGS,
	/* 24/32 bits regs */
	ADE7953_24_32BITS_REGS
};

/**
 * @enum ade7953_zx_edge_e
 * @brief ADE7953 zero-crossing interrupt edge selection.
 */
enum ade7953_zx_edge_e {
	/* Zero-crossing interrupt edge selection */
	/* Interrupt is issued on both positive-going and
	negative-going zero crossing */
	ADE7953_ZX_BOTH_1,
	/* Interrupt is issued on negative-going zero crossing */
	ADE7953_ZX_NEG,
	/* Interrupt is issued on positive-going zero crossing */
	ADE7953_ZX_POS,
	/* Interrupt is issued on both positive-going and
	negative-going zero crossing */
	ADE7953_ZX_BOTH_2
};

/**
 * @enum ade7953_cfsel_e
 * @brief ADE7953 configuration of output signal on CF1/CF2 pin
 */
enum ade7953_cfsel_e {
	/* CF is proportional to active power (Current Channel A). */
	CF_APA,
	/* CF is proportional to reactive power (Current Channel A). */
	CF_RPA,
	/* CF is proportional to apparent power (Current Channel A). */
	CF_APPA,
	/* CF is proportional to IRMS (Current Channel A). */
	CF_IRMSA,
	/* CF is proportional to active power (Current Channel B). */
	CF_BPB,
	/* CF is proportional to reactive power (Current Channel B). */
	CF_RPB,
	/* CF is proportional to apparent power (Current Channel B). */
	CF_APPB,
	/* CF is proportional to IRMS (Current Channel B). */
	CF_IRMSB,
	/* CF is proportional to
	IRMS (Current Channel A) + IRMS (Current Channel B). */
	CF_IRMSA_IRMSB,
	/* CF is proportional to
	active power (Current Channel A) + active power (Current Channel B). */
	CF_APA_APB
};

/**
 * @enum ade7953_zx_alt_e
 * @brief ADE7953 configuration of ZX pin (Pin 1)
 */
enum ade7953_zx_alt_e {
	/* ZX detection is output on Pin 1 (default) */
	ZX_ALT_ZX,
	/* Sag detection is output on Pin 1 */
	ZX_ALT_SAG,
	/* Active power no-load detection
	(Current Channel A) is output on Pin 1 */
	ZX_ALT_APNLOAD_A = 5,
	/* Active power no-load detection
	(Current Channel B) is output on Pin 1 */
	ZX_ALT_APNLOAD_B,
	/* Reactive power no-load detection
	(Current Channel A) is output on Pin 1 */
	ZX_ALT_VARNLOAD_A,
	/* Reactive power no-load detection
	(Current Channel B) is output on Pin 1 */
	ZX_ALT_VARNLOAD_B,
	/* Unlatched waveform sampling signal is output on Pin 1 */
	ZX_ALT_WSMP,
	/* IRQsignal is output on Pin 1 */
	ZX_ALT_IRQ,
	/* ZX_I detection is output on Pin 1 */
	ZX_ALT_ZX_I,
	/* REVP detection is output on Pin 1 */
	ZX_ALT_REVP
};

/**
 * @enum ade7953_zxi_alt_e
 * @brief ADE7953 configuration of ZX_I pin (Pin 21)
 */
enum ade7953_zxi_alt_e {
	/* ZXI detection is output on Pin 21 (default) */
	ZXI_ALT_ZX_I,
	/* Sag detection is output on Pin 21 */
	ZXI_ALT_SAG,
	/* Active power no-load detection
	(Current Channel A) is output on Pin 21 */
	ZXI_ALT_APNLOAD_A = 5,
	/* Active power no-load detection
	(Current Channel B) is output on Pin 21 */
	ZXI_ALT_APNLOAD_B,
	/* Reactive power no-load detection
	(Current Channel A) is output on Pin 21 */
	ZXI_ALT_VARNLOAD_A,
	/* Reactive power no-load detection
	(Current Channel B) is output on Pin 21 */
	ZXI_ALT_VARNLOAD_B,
	/* Unlatched waveform sampling signal is output on Pin 21 */
	ZXI_ALT_WSMP,
	/* IRQsignal is output on Pin 21 */
	ZXI_ALT_IRQ,
	/* ZX detection is output on Pin 21 */
	ZXI_ALT_ZX,
	/* REVP detection is output on Pin 21 */
	ZXI_ALT_REVP
};

/**
 * @enum ade7953_revp_alt_e
 * @brief ADE7953 configuration of REVP pin (Pin 20)
 */
enum ade7953_revp_alt_e {
	/* REVP detection is output on Pin 20 (default) */
	REVP_ALT_REVP,
	/* Sag detection is output on Pin 20 */
	REVP_ALT_SAG,
	/* Active power no-load detection
	(Current Channel A) is output on Pin 20 */
	REVP_ALT_APNLOAD_A = 5,
	/* Active power no-load detection
	(Current Channel B) is output on Pin 20 */
	REVP_ALT_APNLOAD_B,
	/* Reactive power no-load detection
	(Current Channel A) is output on Pin 20 */
	REVP_ALT_VARNLOAD_A,
	/* Reactive power no-load detection
	(Current Channel B) is output on Pin 20 */
	REVP_ALT_VARNLOAD_B,
	/* Unlatched waveform sampling signal is output on Pin 20 */
	REVP_ALT_WSMP,
	/* IRQsignal is output on Pin 20 */
	REVP_ALT_IRQ,
	/* ZX detection is output on Pin 20 */
	REVP_ALT_ZX,
	/* ZX_I detection is output on Pin 20 */
	REVP_ALT_ZX_I
};

/**
 * @enum ade7953_awattacc_e
 * @brief ADE7953 Current Channel A and B active energy accumulation mode
 */
enum ade7953_awattacc_e {
	/* Normal mode. */
	ADE7953_NORMAL_ACC_MODE_AWATT,
	/* Positive-olny accumulation mode */
	ADE7953_POSITIVE_ACC_MODE,
	/* Absolute accumulation mode */
	ADE7953_ABSOLUTE_ACC_MODE_AWATT
};

/**
 * @enum ade7953_avaracc_e
 * @brief ADE7953 Current Channel A and B reactive energy accumulation mode
 */
enum ade7953_avaracc_e {
	/* Normal mode. */
	ADE7953_NORMAL_ACC_MODE_AVAR,
	/* Antitamper accumulation mode */
	ADE7953_ANTITAMP_ACC_MODE,
	/* Absolute accumulation mode */
	ADE7953_ABSOLUTE_ACC_MODE_AVAR
};

/**
 * @enum ade7953_pga_gain_e
 * @brief ADE7953 PGA gain for Current Channel A, B and Voltage Channel
 */
enum ade7953_pga_gain_e {
	/* Gain = 1 */
	ADE7953_GAIN_1,
	/* Gain = 2 */
	ADE7953_GAIN_2,
	/* Gain = 4 */
	ADE7953_GAIN_4,
	/* Gain = 8 */
	ADE7953_GAIN_8,
	/* Gain = 16 */
	ADE7953_GAIN_16,
	/* Gain = 22 */
	/* applicable only for Current Channel A */
	ADE7953_GAIN_22
};
/**
* @struct ade7953_init_param
* @brief ADE7953 Device initialization parameters.
*/
struct ade7953_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_init_param  	*gpio_reset;
	/** Enable 24 bits registers access */
	uint8_t				en_24_bit;
	/** IRQ device descriptor used to handle interrupt routine for GPIO RDY */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/**
* @struct ade7953_dev
* @brief ADE7953 Device structure.
*/
struct ade7953_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/** GPIO RESET descriptor used to reset device (HW reset) */
	struct no_os_gpio_desc  	*gpio_reset;
	/** IRQ device descriptor used to handle interrupt routine for IRQN */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
	/** IRQ device descriptor used to handle interrupt routine for ZX */
	struct no_os_irq_ctrl_desc 	*zx_ctrl;
	/** Enable 24 bits registers access */
	uint8_t				en_24_bit;
};

/**
* @struct ade7953_energy_values
* @brief ADE7953 energy registers values
*/
struct ade7953_energy_values {
	/** Active energy register value */
	int32_t active_energy_reg_val;
	/** Fundamental reactive energy register value */
	int32_t fundamental_reactive_energy_reg_val;
	/** Apparent energy register value */
	int32_t apparent_energy_reg_val;
};

/**
 * @struct ade7953_power_values
 * @brief ADE7953 power registers values
 */
struct ade7953_power_values {
	/** Active power register value */
	int32_t active_power_reg_val;
	/** Fundamental reactive power register value */
	int32_t reactive_power_reg_val;
	/** Apparent power register value */
	int32_t apparent_power_reg_val;
};

/**
 * @struct ade7953_rms_values
 * @brief ADE7953 rms registers values
 */
struct ade7953_rms_values {
	/** Current chA rms register value */
	int32_t current_chA_rms_reg_val;
	/** Current chB rms register value */
	int32_t current_chB_rms_reg_val;
	/** Voltage rms register value */
	int32_t voltage_rms_reg_val;
};

/**
 * @struct ade7953_pq_values
 * @brief ADE7953 power quality registers values
 */
struct ade7953_pq_values {
	/** Power factor register value */
	int32_t power_factor_reg_val;
	/** Period register value */
	int32_t period_reg_val;
};

/* Initialize the device. */
int ade7953_init(struct ade7953_dev **device,
		 struct ade7953_init_param init_param);

/* Read device register. */
int ade7953_read(struct ade7953_dev *dev, uint16_t reg_addr,
		 int32_t *reg_data);

/* Write device register. */
int ade7953_write(struct ade7953_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Update specific register bits. */
static int ade7953_update_bits(struct ade7953_dev *dev, uint16_t reg_addr,
			       uint32_t mask, uint32_t reg_data);

/* Remove the device and release resources. */
int ade7953_remove(struct ade7953_dev *dev);

/* Reset the device using SW reset. */
int ade7953_sw_reset(struct ade7953_dev *dev);

/* Reset the device using HW reset. */
int ade7953_hw_reset(struct ade7953_dev *dev);

/* Lock write to registers. */
int ade7953_wr_lock_8bit(struct ade7953_dev *dev,
			 enum ade7953_write_protect_e regs_select);

/* Version product */
int ade7953_version_product(struct ade7953_dev *dev, uint32_t *data_read);

/* reseat IApeak val */
int ade7953_reset_iapk_val(struct ade7953_dev *dev, uint32_t *val);

/* reseat IBpeak val */
int ade7953_reset_ibpk_val(struct ade7953_dev *dev, uint32_t *val);

/* reseat Vpeak val */
int ade7953_reset_vpk_val(struct ade7953_dev *dev, uint32_t *val);

/* Get interrupt indicator from STATA register. */
int ade7953_get_int_stata(struct ade7953_dev *dev, uint32_t msk,
			  uint8_t *status);

/* Get interrupt indicator from STATB register. */
int ade7953_get_int_statb(struct ade7953_dev *dev, uint32_t msk,
			  uint8_t *status);

/* Clear irq STATA flags. */
int ade7953_clear_irq_stata(struct ade7953_dev *dev);

/* Clear irq STATB flags. */
int ade7953_clear_irq_statb(struct ade7953_dev *dev);

/* Enable interrupt voltage ch and current ch A. */
int ade7953_enable_int_a(struct ade7953_dev *dev, uint32_t msk, uint8_t en);

/* Enable/disable interrupt voltage ch and current ch B. */
int ade7953_enable_int_b(struct ade7953_dev *dev, uint32_t msk, uint8_t en);

/* Zero-crossing interrupt edge selection */
int ade7953_zx_int_edge_set(struct ade7953_dev *dev,
			    enum ade7953_zx_edge_e sel);

/* Configure output signal on CF1/CF2 pin */
int ade7953_cf_output_set(struct ade7953_dev *dev,
			  enum ade7953_cfsel_e sel, enum ade7953_cf_pin_e cf_pin);

/* Configure of ZX pin (Pin1) */
int ade7953_zx_config_pin(struct ade7953_dev *dev,
			  enum ade7953_zx_alt_e sel);

/* Configure of ZXI pin (Pin21) */
int ade7953_zxi_config_pin(struct ade7953_dev *dev,
			   enum ade7953_zxi_alt_e sel);

/* Configure of REVP pin (Pin20) */
int ade7953_revp_pin_config(struct ade7953_dev *dev,
			    enum ade7953_revp_alt_e sel);

/* ACC mode selection for active energy */
int ade7953_acc_active_engy_mode(struct ade7953_dev *dev,
				 enum ade7953_awattacc_e mode, enum ade7953_i_ch_e channel);

/* ACC mode selection for reactive energy */
int ade7953_acc_reactive_engy_mode(struct ade7953_dev *dev,
				   enum ade7953_avaracc_e mode, enum ade7953_i_ch_e channel);

/* Read energy values */
int ade7953_energy_vals(struct ade7953_dev *dev,
			struct ade7953_energy_values *data, enum ade7953_i_ch_e channel);

/* Read power values */
int ade7953_power_vals(struct ade7953_dev *dev,
		       struct ade7953_power_values *data, enum ade7953_i_ch_e channel);

/* Read rms values */
int ade7953_rms_vals(struct ade7953_dev *dev,
		     struct ade7953_rms_values *data, enum ade7953_i_ch_e channel);

/* Read power quaility values */
int ade7953_power_quality_vals(struct ade7953_dev *dev,
			       struct ade7953_pq_values *data);

#endif /* __ADE7953_H__ */
