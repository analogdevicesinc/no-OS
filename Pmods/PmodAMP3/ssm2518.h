/***************************************************************************//**
*   @file   ssm2518.h
*   @brief  Header File for SSM2518 Driver.
*   @author ATofan (alexandru.tofan@analog.com)
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

#include "xparameters.h"

#ifndef SSM2518_H_
#define SSM2518_H_
/* I2C Core Base Address */
#define I2C_BASEADDR		XPAR_AXI_IIC_0_BASEADDR

/* SSM2518 I2C Address = 001101x0 */
#define SSM2518_I2C_ADDR	0x36

/* SSM2518 Registers Address */
#define SSM2518_Reset_Power_Control							0x00
#define SSM2518_Edge_Clock_Control							0x01
#define SSM2518_Serial_Interface_Sample_Rate_Control		0x02
#define SSM2518_Serial_Interface_Control					0x03
#define SSM2518_Channel_Mapping_Control						0x04
#define SSM2518_Left_Volume_Control							0x05
#define SSM2518_Right_Volume_Control						0x06
#define SSM2518_Volume_Mute_Control							0x07
#define SSM2518_Fault_Control_1								0x08
#define SSM2518_Power_Fault_Control  						0x09
#define SSM2518_DRC_Control_1								0x0A
#define SSM2518_DRC_Control_2								0x0B
#define SSM2518_DRC_Control_3								0x0C
#define SSM2518_DRC_Control_4								0x0D
#define SSM2518_DRC_Control_5								0x0E
#define SSM2518_DRC_Control_6								0x0F
#define SSM2518_DRC_Control_7								0x10
#define SSM2518_DRC_Control_8								0x11
#define SSM2518_DRC_Control_9								0x12

/* SSM2518 Register Specific Bits */
// Reset Power Control 0x00
#define S_RST			7
#define NO_BCLK			5
#define MCS				1
#define SPWDN			0
// Edge Speed and Clocking Control 0x01
#define EDGE			1
#define ASR				0
// Serial Audio Interface and Sample Rate Control Register 0x02
#define SDATA_FMT		5
#define SAI				2
#define	FS				0
// Serial Audio Interface Control Register 0x03
#define BCLK_GEN		7
#define LRCLK_MODE		6
#define LRCLK_POL		5
#define SAI_MSB			4
#define SLOT_WIDTH		2
#define BCLK_EDGE		1
// Channel Mapping Control Register 0x04
#define CH_SEL_R		4
#define CH_SEL_L		0
// Left Channel Volume Control Register 0x05
#define L_VOL			0
// Right Channel Volume Control Register 0x06
#define R_VOL			0
// Volume and Mute Control Register 0x07
#define AMUTE			7
#define ANA_GAIN		5
#define DEEMP_EN		4
#define VOL_LINK		3
#define R_MUTE			2
#define L_MUTE			1
#define M_MUTE			0
// Fault Control Register 0x08
#define OC_L			7
#define OC_R			6
#define OT				5
#define MRCV			4
#define MAX_AR			2
#define ARCV			0
// Power and Fault Control Register 0x09
#define AR_TIME			6
#define AMP_LPM			4
#define DAC_LPM			3
#define R_PWDN			2
#define L_PWDN			1
#define APWDN_EN		0
// DRC Control Register 1 0x0A
#define PRE_VOL			6
#define LIM_EN			5
#define COMP_EN			4
#define EXP_EN			3
#define NG_EN			2
#define DRC_EN			0
// DRC Control Register 2 0x0B
#define PEAK_ATT		4
#define PEAK_REL		0
// DRC Control Register 3 0x0C
#define DRC_LT			4
#define DRC_CT			0
// DRC Control Register 4 0x0D
#define DRC_ET			4
#define DRC_NT			0
// DRC Control Register 5 0x0E
#define DRC_SMAX		4
#define DRC_SMIN		0
// DRC Control Register 6 0x0F
#define DRC_ATT			4
#define DRC_DET			0
// DRC Control Register 7 0x10
#define HDT_NOR			4
#define HDT_NG			0
// DRC Control Register 8 0x11
#define DRC_POST_G		2
// DRC Control Register 9 0x12
#define RMS_TAV			0

void SSM2518_WriteReg(char regAddr, unsigned char data);
unsigned char SSM2518_ReadReg(char regAddr);
void SSM2518_Init(void);
void EnableIntc(void);
void EnableUartIntr(void);
void EnableExtIntr(void);

#endif /* SSM2518_H_ */
