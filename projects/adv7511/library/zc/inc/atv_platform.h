/***************************************************************************//**
 *   @file   atv_platform.h
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "atv_types.h"

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
#define HAL_PORT_IDX_ALL            0xFE
#define TX_BOARD_DEV_ID             0xEA
#define HAL_EnableHDMIPath(a)
#define HAL_AssertHPD(a)
#define HAL_EnableTxHPD(a)
#define HAL_IsHPDOn(a)              FALSE
#define HAL_5VDetected(a)           FALSE

enum
{
    OUTPUT_CHAN_AUTO=0,
    OUTPUT_CHAN_AVO1,
    OUTPUT_CHAN_AVO2,
    OUTPUT_CHAN_DAC,
    OUTPUT_CHAN_ENC
};

enum
{
    STATUS_LED1=1,
    STATUS_LED2,
    STATUS_LED3,
    STATUS_LED4,
    STATUS_LED5,
    STATUS_LED6
};

enum
{
    INT1_RX=0,
    INT2_RX,
    INT3_RX,
    INT1_TX,    
    INT2_TX,
    INT1_MUX,
    INT2_MUX,
    INT3_MUX
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
UCHAR   HAL_I2CReadByte (UCHAR Dev, UCHAR Reg, UCHAR *Data);
UCHAR   HAL_I2CWriteByte (UCHAR Dev, UCHAR Reg, UCHAR Data);
UINT16  HAL_I2CReadBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data, UINT16 NumberBytes);
UINT16  HAL_I2CWriteBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data, UINT16 NumberBytes);
UINT16  HAL_I2C16ReadBlock8 (UCHAR Dev, UINT16 Reg, UCHAR* Data, UINT16 NumberBytes);
UINT16  HAL_I2C16WriteBlock8 (UCHAR Dev, UINT16 Reg, UCHAR* Data, UINT16 NumberBytes);
UINT16  HAL_I2C16ReadBlock16 (UCHAR Dev, UINT16 Reg, UINT16* Data, UINT16 NumberWords);
UINT16  HAL_I2C16WriteBlock16 (UCHAR Dev, UINT16 Reg, UINT16* Data, UINT16 NumberWords);
UINT16  HAL_I2CGenericAccess (UCHAR Addr, UINT16 WriteCount, UCHAR *WriteBuf, 
                              UINT16 ReadCount, UCHAR *ReadBuf);
void HAL_I2C_LogInit(void);
void HAL_I2C_LogStop(void);
void HAL_I2C_GetLogInfo(UINT32*, UINT16*);
UINT16  HAL_SpiWriteBlock (UCHAR *Data, UINT16 NumberBytes);
void    HAL_SpiSetSlave(UCHAR SlaveId);
void    HAL_DelayMs (UINT16 Counter);
UINT32  HAL_GetCurrentMsCount(void);
BOOL    HAL_GetUartByte (UCHAR *UartCh);
BOOL    HAL_SendUartByte (UCHAR UartCh);
BOOL    HAL_GetIrCode (UINT16 *Ir_code);
BOOL    HAL_GetMBSwitchState (void);
UCHAR   HAL_GetMBButtonsState (void);
void    HAL_SetGPIO_State (UCHAR, UCHAR);
BOOL    HAL_RxInt1Pending (void);
BOOL    HAL_RxInt2Pending (void);
BOOL    HAL_TxIntPending (void);
void HAL_PlatformInit(UINT32 i2cBaseAddr,
					  UINT32 timerDevId,
					  UINT32 timerIntCtrlDevId,
					  UINT32 timerIntId);;
UINT16  HAL_GetHwRevision (void);
char   *HAL_GetPlatformName (void);
UCHAR   HAL_GetRxBoardRevision (void);
UCHAR   HAL_GetTxBoardRevision (void);
void    HAL_MainReset (void);
void    HAL_MpuReset (void);
void    HAL_SwitchContext (BOOL RepContext, void (*EntryPoint) (void));
void    HAL_ContextCheck (void);
BOOL    HAL_IntPendingN (UCHAR IntId);
void    HAL_PlatformRXInit (void);
void    HAL_PlatformTXInit (void);
void    HAL_SetLedState (UCHAR LedId, BOOL value);
void    HAL_SetGPIOState (UCHAR GpioId, BOOL value);
void    HAL_AudioCodec(void);
void    HAL_SetAudioChannelMux(UCHAR);
UCHAR   HAL_AudioCodecWr(UCHAR, UCHAR);
UCHAR   HAL_AudioCodecRead(UCHAR);

#endif
