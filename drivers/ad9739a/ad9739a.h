/***************************************************************************//**
 *   @file   AD9739A.h
 *   @brief  Header file of AD9739A Driver.
 *   @author Bancisor Mihai
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/
#ifndef __AD9739A_H__
#define __AD9739A_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***************************** AD9739A ****************************************/
/******************************************************************************/

#define AD9739A_READ				(1 << 7)
#define AD9739A_WRITE				(0 << 7)

/* Registers */
#define AD9739A_REG_MODE			0x00
#define AD9739A_REG_POWER_DOWN		0x01
#define AD9739A_REG_CNT_CLK_DIS		0x02
#define AD9739A_REG_IRQ_EN			0x03
#define AD9739A_REG_IRQ_REQ			0x04	
#define AD9739A_REG_FSC_1			0x06
#define AD9739A_REG_FSC_2			0x07	
#define AD9739A_REG_DEC_CNT			0x08
#define AD9739A_REG_LVDS_STAT1		0x0C
#define AD9739A_REG_LVDS_REC_CNT1	0x10
#define AD9739A_REG_LVDS_REC_CNT2	0x11
#define AD9739A_REG_LVDS_REC_CNT3	0x12
#define AD9739A_REG_LVDS_REC_CNT4	0x13
#define AD9739A_REG_LVDS_REC_CNT5	0x14
#define AD9739A_REG_LVDS_REC_STAT1	0x19
#define AD9739A_REG_LVDS_REC_STAT2	0x1A
#define AD9739A_REG_LVDS_REC_STAT3	0x1B
#define AD9739A_REG_LVDS_REC_STAT4	0x1C
#define AD9739A_REG_LVDS_REC_STAT9	0x21
#define AD9739A_REG_CROSS_CNT1		0x22
#define AD9739A_REG_CROSS_CNT2		0x23
#define AD9739A_REG_PHS_DET			0x24
#define AD9739A_REG_MU_DUTY			0x25
#define AD9739A_REG_MU_CNT1			0x26
#define AD9739A_REG_MU_CNT2			0x27
#define AD9739A_REG_MU_CNT3			0x28
#define AD9739A_REG_MU_CNT4			0x29
#define AD9739A_REG_MU_STAT1		0x2A
#define AD9739A_REG_PART_ID			0x35

/* AD9739A_REG_MODE definitions, address 0x00 */
#define AD9739A_MODE_SDIO_DIR			((1 << 7) | (1 << 0))
#define AD9739A_MODE_LSB				((1 << 6) | (1 << 1))
#define AD9739A_MODE_RESET				((1 << 5) | (1 << 2))

/* AD9739A_REG_POWER_DOWN definitions, address 0x01 */
#define AD9739A_POWER_DOWN_LVDS_DRVR_PD	(1 << 5)
#define AD9739A_POWER_DOWN_LVDS_RCVR_PD	(1 << 4)
#define AD9739A_POWER_DOWN_CLK_RCVR_PD	(1 << 1)
#define AD9739A_POWER_DOWN_DAC_BIAS_PD	(1 << 0)

/* AD9739A_REG_CNT_CLK_DIS definitions, address 0x02 */
#define AD9739A_CNT_CLK_DIS_CLKGEN_PD	(1 << 3)
#define AD9739A_CNT_CLK_DIS_REC_CNT_CLK	(1 << 1)
#define AD9739A_CNT_CLK_DIS_MU_CNT_CLK	(1 << 0)

/* AD9739A_REG_IRQ_EN definitions, address 0x03 */
#define AD9739A_IRQ_EN_MU_LST_EN		(1 << 3)
#define AD9739A_IRQ_EN_MU_LCK_EN		(1 << 2)
#define AD9739A_IRQ_EN_RCV_LST_EN		(1 << 1)
#define AD9739A_IRQ_EN_RCV_LCK_EN		(1 << 0)

/* AD9739A_REG_IRQ_REQ definitions, address 0x04 */
#define AD9739A_IRQ_REQ_MU_LST_IRQ		(1 << 3)
#define AD9739A_IRQ_REQ_MU_LCK_IRQ		(1 << 2)
#define AD9739A_IRQ_REQ_RCV_LST_IRQ		(1 << 1)
#define AD9739A_IRQ_REQ_RCV_LCK_IRQ		(1 << 0)

/* AD9739A_REG_FSC_1 definitions, address 0x06 */
#define AD9739A_FSC_1_FSC_1(x)			(((x) & 0xFF) << 0)

/* AD9739A_REG_FSC_2 definitions, address 0x07 */
#define AD9739A_FSC_2_FSC_2(x)			(((x) & 0x3) << 0)
#define AD9739A_FSC_2_Sleep				(1 << 7)

/* AD9739A_REG_DEC_CNT definitions, address 0x08 */
#define AD9739A_DEC_CNT_DAC_DEC(x)			(((x) & 0x3) << 0)
#define AD9739A_DAC_DEC_NORMAL_BASEBAND		0	// AD9739A_DEC_CNT_DAC_DEC(x) option.
#define AD9739A_DAC_DEC_MIX_MODE			2	// AD9739A_DEC_CNT_DAC_DEC(x) option.

/* AD9739A_REG_LVDS_STAT1 definitions, address 0x0C */
#define AD9739A_LVDS_STAT1_DCI_PRE_PH0	(1 << 2)
#define AD9739A_LVDS_STAT1_DCI_PST_PH0	(1 << 0)

/* AD9739A_REG_LVDS_REC_CNT1 definitions, address 0x10 */
#define AD9739A_LVDS_REC_CNT1_RCVR_FLG_RST	(1 << 2)
#define AD9739A_LVDS_REC_CNT1_RCVR_LOOP_ON	(1 << 1)
#define AD9739A_LVDS_REC_CNT1_RCVR_CNT_ENA	(1 << 0)

/* AD9739A_REG_LVDS_REC_CNT2 definitions, address 0x11 */
#define AD9739A_LVDS_REC_CNT2_SMP_DEL(x)		(((x) & 0x3) << 6)

/* AD9739A_REG_LVDS_REC_CNT3 definitions, address 0x12 */
#define AD9739A_LVDS_REC_CNT3_SMP_DEL(x)		(((x) & 0xFF) << 0)

/* AD9739A_REG_LVDS_REC_CNT4 definitions, address 0x13 */
#define AD9739A_LVDS_REC_CNT4_DCI_DEL(x)		(((x) & 0xF) << 4)
#define AD9739A_LVDS_REC_CNT4_FINE_DEL_SKEW(x)	(((x) & 0xF) << 0)

/* AD9739A_REG_LVDS_REC_CNT5 definitions, address 0x14 */
#define AD9739A_LVDS_REC_CNT5_DCI_DEL(x)		(((x) & 0x3F) << 0)	

/* AD9739A_REG_LVDS_REC_STAT1 definitions, address 0x19 */
#define AD9739A_LVDS_REC_STAT1_SMP_DEL(x)		(((x) & 0x3) << 6)	

/* AD9739A_REG_LVDS_REC_STAT2 definitions, address 0x1A */
#define AD9739A_LVDS_REC_STAT2_SMP_DEL(x)		(((x) & 0xFF) << 0)

/* AD9739A_REG_LVDS_REC_STAT3 definitions, address 0x1B */
#define AD9739A_LVDS_REC_STAT3_DCI_DEL(x)		(((x) & 0x3) << 6)

/* AD9739A_REG_LVDS_REC_STAT4 definitions, address 0x1C */
#define AD9739A_LVDS_REC_STAT4_DCI_DEL(x)		(((x) & 0xFF) << 0)

/* AD9739A_REG_LVDS_REC_STAT9 definitions, address 0x21 */
#define AD9739A_LVDS_REC_STAT9_RCVR_TRK_ON		(1 << 3)
#define AD9739A_LVDS_REC_STAT9_RCVR_FE_ON	   (1 << 2)
#define AD9739A_LVDS_REC_STAT9_RCVR_LST			(1 << 1)
#define AD9739A_LVDS_REC_STAT9_RCVR_LCK			(1 << 0)

/* AD9739A_REG_CROSS_CNT1 definitions, address 0x22 */
#define AD9739A_CROSS_CNT1_DIR_P				(1 << 4)
#define AD9739A_CROSS_CNT1_CLKP_OFFSET(x)		(((x) & 0xF) << 0)

/* AD9739A_REG_CROSS_CNT2 definitions, address 0x23 */
#define AD9739A_CROSS_CNT2_DIR_N				(1 << 4)
#define AD9739A_CROSS_CNT2_CLKN_OFFSET(x)		(((x) & 0xF) << 0)

/* AD9739A_REG_PHS_DET definitions, address 0x24 */
#define AD9739A_PHS_DET_CMP_BST					(1 << 5)
#define AD9739A_PHS_DET_PHS_DET_AUTO_EN			(1 << 4)

/* AD9739A_REG_MU_DUTY definitions, address 0x25 */
#define AD9739A_MU_DUTY_MU_DUTY_AUTO_EN			(1 << 7)

/* AD9739A_REG_MU_CNT1 definitions, address 0x26 */
#define AD9739A_MU_CNT1_SLOPE					(1 << 6)
#define AD9739A_MU_CNT1_MODE(x)					(((x) & 0x3) << 4)
#define AD9739A_MU_CNT1_READ					(1 << 3)
#define AD9739A_MU_CNT1_GAIN(x)					(((x) & 0x3) << 1)
#define AD9739A_MU_CNT1_ENABLE					(1 << 0)

/* AD9739A_REG_MU_CNT2 definitions, address 0x27 */
#define AD9739A_MU_CNT2_MUDEL					(1 << 7)
#define AD9739A_MU_CNT2_SRCH_MODE(x)			((((x) & 0x3) << 5))
#define AD9739A_MU_CNT2_SET_PHS(x)				((((x) & 0x1F) << 0))

/* AD9739A_REG_MU_CNT3 definitions, address 0x28 */
#define AD9739A_MU_CNT3_MUDEL(x)				((((x) & 0xFF) << 0))

/* AD9739A_REG_MU_CNT4 definitions, address 0x29 */
#define AD9739A_MU_CNT4_SEARCH_TOL				(1 << 7)
#define AD9739A_MU_CNT4_RETRY					(1 << 6)
#define AD9739A_MU_CNT4_CONTRST					(1 << 5)
#define AD9739A_MU_CNT4_GUARD(x)				((((x) & 0x1F) << 0))

/* AD9739A_REG_MU_STAT1 definitions, address 0x2A */
#define AD9739A_MU_STAT1_MU_LST					(1 << 1)
#define AD9739A_MU_STAT1_MU_LKD					(1 << 0)

/* AD9739A_REG_PART_ID definitions, address 0x35 */
#define AD9739A_PART_ID_PART_ID(x)				((((x) & 0xFF) << 0))

/******************************************************************************/
/************************ Types Definitions ***********************************/
/******************************************************************************/

/**
 * struct ad9739a_platform_data - Platform specific information.
 *
 * @common_mode_voltage_dacclk_p: magnitude of the offset for the DACCLK_P.
 * @common_mode_voltage_dacclk_n: magnitude of the offset for the DACCLK_N. 

 */
struct ad9739a_platform_data 
{
	uint8_t common_mode_voltage_dacclk_p;
	uint8_t common_mode_voltage_dacclk_n;
	float   full_scale_current;
};

typedef struct
{
	uint8_t common_mode_voltage_dacclk_p;
	uint8_t common_mode_voltage_dacclk_n;
	float   full_scale_current;
}ad9739a_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Writes a value to the selected register. */
int32_t ad9739a_write(unsigned char registerAddress, 
					  unsigned char registerValue);
/*! Reads the value of the selected register. */
int32_t ad9739a_read(unsigned char registerAddress);
/*! Resets the device. */
int32_t ad9739a_reset(void);
/*! Powers down LVDS interface and TxDAC. */
int32_t ad9739a_power_down(unsigned char pwrConfig);
/*! Sets the normal baseband mode or mix-mode. */
int32_t ad9739a_operation_mode(unsigned char mode);
/*! Sets the full-scale output current for the DAC.  */
float ad9739a_DAC_fs_current(float fs_val);
/*! Delay for a number of fdata clock cycles. */
int32_t delay_fdata_cycles(uint32_t cycles);
/*! Initializes the AD9739A. */
int32_t ad9739a_setup(uint32_t spi_device_id, uint8_t slave_select,
		ad9739a_init_param init_param);

#endif /* __AD9739A_H__ */
