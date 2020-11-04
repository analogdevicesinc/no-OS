/***************************************************************************//**
*   @file   ADP5589.h
*   @brief  Header File for ADP5589 Driver.
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

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "xil_types.h"
#include "xparameters.h"

/*****************************************************************************/
/******************* ADP5589 Registers Definitions ***************************/
/*****************************************************************************/
// ADP5589 IIC Address
#define I2C_BASEADDR 		XPAR_AXI_IIC_1_BASEADDR
#define ADP5589_I2C_ADDR	0x34
// Register address
#define IDReg               0x00
#define INTStatusReg        0x01
#define StatusReg           0x02                            
#define FIFO1Reg            0x03
#define FIFO2Reg            0x04
#define FIFO3Reg            0x05
#define FIFO4Reg            0x06
#define FIFO5Reg            0x07
#define FIFO6Reg            0x08
#define FIFO7Reg            0x09
#define FIFO8Reg            0x0A
#define FIFO9Reg            0x0B
#define FIFO10Reg           0x0C
#define FIFO11Reg           0x0D
#define FIFO12Reg           0x0E
#define FIFO13Reg           0x0F
#define FIFO14Reg           0x10
#define FIFO15Reg           0x11
#define FIFO16Reg           0x12
#define GPIIntStatAReg      0x13
#define GPIIntStatBReg      0x14
#define GPIIntStatCReg      0x15
#define GPIStatAReg         0x16
#define GPIStatBReg         0x17
#define GPIStatCReg         0x18
#define RPullConfigAReg     0x19
#define RPullConfigBReg     0x1A
#define RPullConfigCReg     0x1B
#define RPullConfigDReg     0x1C
#define RPullConfigEReg     0x1D
#define GPIIntLevelAReg     0x1E
#define GPIIntLevelBReg     0x1F
#define GPIIntLevelCReg     0x20
#define GPIEventEnAReg      0x21
#define GPIEventEnBReg      0x22
#define GPIEventEnCReg      0x23
#define GPIIntEnAReg        0x24
#define GPIIntEnBReg        0x25
#define GPIIntEnCReg        0x26
#define DbDisAReg           0x27
#define DbDisBReg           0x28
#define DbDisCReg           0x29
#define GPODataOutAReg      0x2A
#define GPODataOutBReg      0x2B
#define GPODataOutCReg      0x2C
#define GPOOutModeAReg      0x2D
#define GPOOutModeBReg      0x2E
#define GPOOutModeCReg      0x2F
#define GPIODirAReg         0x30
#define GPIODirBReg         0x31
#define GPIODirCReg         0x32
#define Unlock1Reg          0x33
#define Unlock2Reg          0x34
#define ExtLockEvReg        0x35
#define UnlockTimerReg      0x36
#define LockCfgReg          0x37
#define Reset1EvAReg        0x38
#define Reset1EvBReg        0x39
#define Reset1EvCReg        0x3A
#define Reset2EvAReg        0x3B
#define Reset2EvBReg        0x3C
#define ResetCfgReg         0x3D
#define PWMOfstLowReg       0x3E
#define PWMOfstHighReg      0x3F
#define PWMOntLowReg        0x40
#define PWMOntHighReg       0x41
#define PWMCfgReg           0x42
#define ClkDivCfgReg        0x43
#define Logic1CfgReg        0x44
#define Logic2CfgReg        0x45
#define LogicFFCfgReg       0x46
#define LogicIntEvEnReg     0x47
#define PollTimeCfgReg      0x48
#define PinCfgAReg          0x49
#define PinCfgBReg          0x4A
#define PinCfgCReg          0x4B
#define PinCfgDReg          0x4C
#define GenCfgBReg          0x4D     
#define IntENReg            0x4E 
/*****************************************************************************/
/******************* ADP5589 Registers Bits **********************************/
/*****************************************************************************/
// ID Register 0x00
#define MAN_ID              4
#define REV_ID              0
// INT_STATUS Register 0x01
#define LOGIC2_INT          5
#define LOGIC1_INT          4
#define LOCK_INT            3
#define OVERFLOW_INT        2
#define GPI_INT             1
#define EVENT_INT           0
// Status Register 0x02
#define LOGIC2_STAT         7
#define LOGIC1_STAT         6
#define LOCK_STAT           5
#define EC                  0
// FIFO_1 Register 0x03
#define EVENT1_STATE        7
#define EVENT1_ID           0
// FIFO_2 Register 0x04
#define EVENT2_STATE        7
#define EVENT2_ID           0
// FIFO_3 Register 0x05
#define EVENT3_STATE        7
#define EVENT3_ID           0
// FIFO_4 Register 0x06
#define EVENT4_STATE        7
#define EVENT4_ID           0
// FIFO_5 Register 0x07
#define EVENT5_STATE        7
#define EVENT5_ID           0
// FIFO_6 Register 0x08
#define EVENT6_STATE        7
#define EVENT6_ID           0
// FIFO_7 Register 0x09
#define EVENT7_STATE        7
#define EVENT7_ID           0
// FIFO_8 Register 0x0A
#define EVENT8_STATE        7
#define EVENT8_ID           0
// FIFO_9 Register 0x0B
#define EVENT9_STATE        7
#define EVENT9_ID           0
// FIFO_10 Register 0x0C
#define EVENT10_STATE       7
#define EVENT10_ID          0
// FIFO_11 Register 0x0D
#define EVENT11_STATE       7
#define EVENT11_ID          0
// FIFO_12 Register 0x0E
#define EVENT12_STATE       7
#define EVENT12_ID          0
// FIFO_13 Register 0x0F
#define EVENT13_STATE       7
#define EVENT13_ID          0
// FIFO_14 Register 0x10
#define EVENT14_STATE       7
#define EVENT14_ID          0
// FIFO_15 Register 0x11
#define EVENT15_STATE       7
#define EVENT15_ID          0
// FIFO_16 Register 0x12
#define EVENT16_STATE       7
#define EVENT16_ID          0
// GPI_INT_STAT_A Register 0x13
#define GPI_8_INT           7
#define GPI_7_INT           6
#define GPI_6_INT           5
#define GPI_5_INT           4
#define GPI_4_INT           3
#define GPI_3_INT           2
#define GPI_2_INT           1
#define GPI_1_INT           0
// GPI_INT_STAT_B Register 0x14
#define GPI_16_INT          7
#define GPI_15_INT          6
#define GPI_14_INT          5
#define GPI_13_INT          4
#define GPI_12_INT          3
#define GPI_11_INT          2
#define GPI_10_INT          1
#define GPI_9_INT           0
// GPI_INT_STAT_C Register 0x15
#define GPI_19_INT          2
#define GPI_18_INT          1
#define GPI_17_INT          0
// GPI_STATUS_A Register 0x16
#define GPI_8_STAT          7
#define GPI_7_STAT          6
#define GPI_6_STAT          5
#define GPI_5_STAT          4
#define GPI_4_STAT          3
#define GPI_3_STAT          2
#define GPI_2_STAT          1
#define GPI_1_STAT          0
// GPI_STATUS_B Register 0x17
#define GPI_16_STAT         7
#define GPI_15_STAT         6
#define GPI_14_STAT         5
#define GPI_13_STAT         4
#define GPI_12_STAT         3
#define GPI_11_STAT         2
#define GPI_10_STAT         1
#define GPI_9_STAT          0
// GPI_STATUS_C Register 0x18
#define GPI_19_STAT         2
#define GPI_18_STAT         1
#define GPI_17_STAT         0
// RPULL_CONFIG_A Register 0x19
#define R3_PULL_CFG         6
#define R2_PULL_CFG         4
#define R1_PULL_CFG         2
#define R0_PULL_CFG         0
// RPULL_CONFIG_B Register 0x1A
#define R7_PULL_CFG         6
#define R6_PULL_CFG         4
#define R5_PULL_CFG         2
#define R4_PULL_CFG         0
// RPULL_CONFIG_C Register 0x1B
#define C3_PULL_CFG         6
#define C2_PULL_CFG         4
#define C1_PULL_CFG         2
#define C0_PULL_CFG         0
// RPULL_CONFIG_D Register 0x1C
#define C7_PULL_CFG         6
#define C6_PULL_CFG         4
#define C5_PULL_CFG         2
#define C4_PULL_CFG         0
// RPULL_CONFIG_D Register 0x1D
#define C10_PULL_CFG        4
#define C9_PULL_CFG         2
#define C8_PULL_CFG         0
// GPI_INT_LEVEL_A Register 0x1E
#define GPI_8_INT_LEVEL     7
#define GPI_7_INT_LEVEL     6
#define GPI_6_INT_LEVEL     5
#define GPI_5_INT_LEVEL     4
#define GPI_4_INT_LEVEL     3
#define GPI_3_INT_LEVEL     2
#define GPI_2_INT_LEVEL     1
#define GPI_1_INT_LEVEL     0
// GPI_INT_LEVEL_B Register 0x1F
#define GPI_16_INT_LEVEL    7
#define GPI_15_INT_LEVEL    6
#define GPI_14_INT_LEVEL    5
#define GPI_13_INT_LEVEL    4
#define GPI_12_INT_LEVEL    3
#define GPI_11_INT_LEVEL    2
#define GPI_10_INT_LEVEL    1
#define GPI_9_INT_LEVEL     0
// GPI_INT_LEVEL_C Register 0x20
#define GPI_19_INT_LEVEL    2
#define GPI_18_INT_LEVEL    1
#define GPI_17_INT_LEVEL    0
// GPI_EVENT_EN_A Register 0x21
#define GPI_8_EVENT_EN      7
#define GPI_7_EVENT_EN      6
#define GPI_6_EVENT_EN      5
#define GPI_5_EVENT_EN      4
#define GPI_4_EVENT_EN      3
#define GPI_3_EVENT_EN      2
#define GPI_2_EVENT_EN      1
#define GPI_1_EVENT_EN      0
// GPI_EVENT_EN_B Register 0x22
#define GPI_16_EVENT_EN     7
#define GPI_15_EVENT_EN     6
#define GPI_14_EVENT_EN     5
#define GPI_13_EVENT_EN     4
#define GPI_12_EVENT_EN     3
#define GPI_11_EVENT_EN     2
#define GPI_10_EVENT_EN     1
#define GPI_9_EVENT_EN      0
// GPI_EVENT_EN_C Register 0x23
#define GPI_19_EVENT_EN     2
#define GPI_18_EVENT_EN     1
#define GPI_17_EVENT_EN     0
// GPI_INTERRUPT_EN_A Register 0x24
#define GPI_8_INT_EN        7
#define GPI_7_INT_EN        6
#define GPI_6_INT_EN        5
#define GPI_5_INT_EN        4
#define GPI_4_INT_EN        3
#define GPI_3_INT_EN        2
#define GPI_2_INT_EN        1
#define GPI_1_INT_EN        0
// GPI_INTERRUPT_EN_B Register 0x25
#define GPI_16_INT_EN       7
#define GPI_15_INT_EN       6
#define GPI_14_INT_EN       5
#define GPI_13_INT_EN       4
#define GPI_12_INT_EN       3
#define GPI_11_INT_EN       2
#define GPI_10_INT_EN       1
#define GPI_9_INT_EN        0
// GPI_INTERRUPT_EN_C Register 0x26
#define GPI_19_INT_EN       2
#define GPI_18_INT_EN       1
#define GPI_17_INT_EN       0
// DEBOUNCE_DIS_A Register 0x27
#define GPI_8_DEB_DIS       7
#define GPI_7_DEB_DIS       6
#define GPI_6_DEB_DIS       5
#define GPI_5_DEB_DIS       4
#define GPI_4_DEB_DIS       3
#define GPI_3_DEB_DIS       2
#define GPI_2_DEB_DIS       1
#define GPI_1_DEB_DIS       0
// DEBOUNCE_DIS_B Register 0x28
#define GPI_16_DEB_DIS      7
#define GPI_15_DEB_DIS      6
#define GPI_14_DEB_DIS      5
#define GPI_13_DEB_DIS      4
#define GPI_12_DEB_DIS      3
#define GPI_11_DEB_DIS      2
#define GPI_10_DEB_DIS      1
#define GPI_9_DEB_DIS       0
// DEBOUNCE_DIS_C Register 0x29
#define GPI_19_DEB_DIS      2
#define GPI_18_DEB_DIS      1
#define GPI_17_DEB_DIS      0
// GPO_DATA_OUT_A Register 0x2A
#define GPO_8_DATA          7
#define GPO_7_DATA          6
#define GPO_6_DATA          5
#define GPO_5_DATA          4
#define GPO_4_DATA          3
#define GPO_3_DATA          2
#define GPO_2_DATA          1
#define GPO_1_DATA          0
// GPO_DATA_OUT_B Register 0x2B
#define GPO_16_DATA         7
#define GPO_15_DATA         6
#define GPO_14_DATA         5
#define GPO_13_DATA         4
#define GPO_12_DATA         3
#define GPO_11_DATA         2
#define GPO_10_DATA         1
#define GPO_9_DATA          0
// GPO_DATA_OUT_C Register 0x2C
#define GPO_19_DATA         2
#define GPO_18_DATA         1
#define GPO_17_DATA         0
// GPO_OUT_MODE_A Register 0x2D
#define GPO_8_OUT_MODE      7
#define GPO_7_OUT_MODE      6
#define GPO_6_OUT_MODE      5
#define GPO_5_OUT_MODE      4
#define GPO_4_OUT_MODE      3
#define GPO_3_OUT_MODE      2
#define GPO_2_OUT_MODE      1
#define GPO_1_OUT_MODE      0
// GPO_OUT_MODE_B Register 0x2E
#define GPO_16_OUT_MODE     7
#define GPO_15_OUT_MODE     6
#define GPO_14_OUT_MODE     5
#define GPO_13_OUT_MODE     4
#define GPO_12_OUT_MODE     3
#define GPO_11_OUT_MODE     2
#define GPO_10_OUT_MODE     1
#define GPO_9_OUT_MODE      0
// GPO_OUT_MODE_C Register 0x2F
#define GPO_19_OUT_MODE     2
#define GPO_18_OUT_MODE     1
#define GPO_17_OUT_MODE     0
// GPO_DATA_OUT_A Register 0x30
#define GPO_8_DIR           7
#define GPO_7_DIR           6
#define GPO_6_DIR           5
#define GPO_5_DIR           4
#define GPO_4_DIR           3
#define GPO_3_DIR           2
#define GPO_2_DIR           1
#define GPO_1_DIR           0
// GPO_DATA_OUT_B Register 0x31
#define GPO_16_DIR          7
#define GPO_15_DIR          6
#define GPO_14_DIR          5
#define GPO_13_DIR          4
#define GPO_12_DIR          3
#define GPO_11_DIR          2
#define GPO_10_DIR          1
#define GPO_9_DIR           0
// GPO_DATA_OUT_C Register 0x32
#define GPO_19_DIR          2
#define GPO_18_DIR          1
#define GPO_17_DIR          0
// UNLOCK1 Register 0x33
#define UNLOCK1_STATE       7
#define UNLOCK1             0
// UNLOCK2 Register 0x34
#define UNLOCK2_STATE       7
#define UNLOCK2             0
// EXT_LOCK_EVENT Register 0x35
#define EXT_LOCK_STATE      7
#define EXT_LOCK_EVENT      0
// UNLOCK_TIMERS Register 0x36
#define INT_MASK_TIMER      3
#define UNLOCK_TIMER        0
// LOCK_CFG Register 0x37
#define LOCK_EN             0
// RESET1_EVENT_A Register 0x38
#define RESET1_EVENT_A_LVL  7
#define RESET1_EVENT_A      0
// RESET1_EVENT_B Register 0x39
#define RESET1_EVENT_B_LVL  7
#define RESET1_EVENT_B      0
// RESET1_EVENT_A Register 0x3A
#define RESET1_EVENT_C_LVL  7
#define RESET1_EVENT_C      0
// RESET2_EVENT_A Register 0x3B
#define RESET2_EVENT_A_LVL  7
#define RESET2_EVENT_A      0
// RESET2_EVENT_B Register 0x3C
#define RESET2_EVENT_B_LVL  7
#define RESET2_EVENT_B      0
// RESET_CFG Register 0x3D
#define RESET2_POL          7
#define RESET1_POL          6
#define RST_PASSTHRU_EN     5
#define RESET_TRIGGER_RIME  2
#define RESET_PULSE_WIDTH   0
// PWM_OFST_LOW Register 0x3E
#define PWM_OFST_LOW_BYTE   0
// PWM_OFST_HIGH Register 0x3F
#define PWM_OFST_HIGH_BYTE  0
// PWM_ONT_LOW Register 0x40
#define PWM_ONT_LOW_BYTE    0
// PWM_ONT_HIGH Register 0x41
#define PWM_ONT_HIGH_BYTE   0
// PWM_CFG Register 0x42
#define PWM_IN_AND          2
#define PWM_MODE            1
#define PWM_EN              0
// CLOCK_DIV_CFG Register 0x43
#define CLK_INV             6
#define CLK_DIV             1
#define CLK_DIV_EN          0
// LOGIC_1_CFG Register 0x44
#define LY1_INV             6
#define LC1_INV             5
#define LB1_INV             4
#define LA1_INV             3
#define LOGIC1_SEL          0
// LOGIC_2_CFG Register 0x45
#define LY1_CASCADE         7
#define LY2_INV             6
#define LC2_INV             5
#define LB2_INV             4
#define LA2_INV             3
#define LOGIC2_SEL          0
// LOGIC_FF_CFG Register 0x46
#define FF2_SET             3
#define FF2_CLR             2
#define FF1_SET             1
#define FF1_CLR             0
// LOGIC_INT_EVENT_EN Register 0x47
#define LY2_DBNC_DIS        5
#define LOGIC2_EVENT_EN     4
#define LOGIC2_INT_LEVEL    3
#define LY1_DBNC_DIS        2
#define LOGIC1_EVENT_EN     1
#define LOGIC1_INT_LEVEL    0
// POLL_TIME_CFG Register 0x48
#define KEY_POLL_TIME       0
// PIN_CONFIG_A Register 0x49
#define R7_CONFIG           7
#define R6_CONFIG           6
#define R5_CONFIG           5
#define R4_CONFIG           4
#define R3_CONFIG           3
#define R2_CONFIG           2
#define R1_CONFIG           1
#define R0_CONFIG           0
// PIN_CONFIG_C Register 0x4A
#define C7_CONFIG           7
#define C6_CONFIG           6
#define C5_CONFIG           5
#define C4_CONFIG           4
#define C3_CONFIG           3
#define C2_CONFIG           2
#define C1_CONFIG           1
#define C0_CONFIG           0
// PIN_CONFIG_C Register 0x4B
#define C10_CONFIG          2
#define C9_CONFIG           1
#define C8_CONFIG           0
// PIN_CONFIG_D Register 0x4C
#define PULL_SELECT         7
#define C4_EXTEND_CFG       6
#define R4_EXTEND_CFG       5
#define C6_EXTEND_CFG       4
#define R3_EXTEND_CFG       2
#define C9_EXTEND_CFG       1
#define R0_EXTEND_CFG       0
// GENERAL_CFG_B Register 0x4D
#define OSC_EN              7
#define CORE_FREQ           5
#define LCK_TRK_LOGIC       4
#define LCK_TRK_GPI         3
#define INT_CFG             1
#define RST_CFG             0
// INT_EN Register 0x4E
#define LOGIC2_IEN          5
#define LOGIC1_IEN          4
#define LOCK_IEN            3
#define OVERFLOW_IEN        2
#define GPI_IEN             1
#define EVENT_IEN           0

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
char KeyDecode(int rxData);
void FlushFIFO(void);
void ADP5589_Init(void);
void ADP5589_KeyDecodeTest(void);
void ADP5589_KeyLockTest(void);
void DemoProgram(void);
