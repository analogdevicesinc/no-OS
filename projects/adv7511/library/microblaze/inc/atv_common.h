/***************************************************************************//**
 *   @file   atv_common.h
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

#ifndef _ATV_COMMON_H_
#define _ATV_COMMON_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "atv_types.h"

/******************************************************************************/
/************************** Macros Definitions ********************************/
/******************************************************************************/
#define PKT_AV_INFO_FRAME               0x0001
#define PKT_AUDIO_INFO_FRAME            0x0002
#define PKT_ACP_PACKET                  0x0004
#define PKT_SPD_PACKET                  0x0008
#define PKT_ISRC1_PACKET                0x0010
#define PKT_ISRC2_PACKET                0x0020
#define PKT_GMD_PACKET                  0x0040
#define PKT_GC_PACKET                   0x0080
#define PKT_MPEG_PACKET                 0x0100
#define PKT_VS_PACKET                   0x0200
#define PKT_AUDIO_CHANNEL_STATUS        0x0400
#define PKT_AUDIO_SAMPLE_PACKET         0x0800
#define PKT_ACR_PACKET                  0x1000
#define PKT_UNKNOWN_PACKET              0x8000
#define PKT_ALL_PACKETS                 0xffff

/******************************************************************************/
/************************** Types Declarations ********************************/
/******************************************************************************/
typedef enum
{
    ATVERR_OK=0,
    ATVERR_FALSE=0,
    ATVERR_TRUE=1,
    ATVERR_INV_PARM,
    ATVERR_NOT_AVAILABLE,
    ATVERR_FAILED
} ATV_ERR;

typedef struct
{
    UCHAR   StrgId;
    char    *Strg;
} STRG_TABLE;

typedef struct
{
    UINT32  StartTime;      /* Will be set to 0 when threshold expires */
    UINT16  Threshold;      /* Timeout in ms from StartTime */
    BOOL    TimedOut;       /* Will be set to TRUE when timeout expires */
} TIMEOUT_TABLE;

typedef struct
{
    UCHAR   RegAddr;        /* Register address */
    UCHAR   Mask;           /* Mask for the bits in RegAddr */
    UCHAR   LShift;         /* Absolute left shift (in 2's complement) of */
                            /* field bits in RegAddr */
    UCHAR   Reserved;
} I2C_FIELD_INFO;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
UCHAR   ATV_I2CReadField8   (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
                             UCHAR BitPos);
void    ATV_I2CWriteField8  (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
                             UCHAR BitPos,  UCHAR FieldVal);
void   ATV_I2CWriteOField8  (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
                             UCHAR BitPos, UCHAR *CurrVal, UCHAR FieldVal);
UINT32  ATV_I2CReadField32  (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
                             UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
UINT32  ATV_I2CReadFragField32 (UCHAR DevAddr, UCHAR *AddrTbl, UCHAR MsbMask,
                                UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan);
UINT32  ATV_I2CReadField32LE   (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
                                UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan);
UINT32  ATV_I2CReadFragField32LE (UCHAR DevAddr, UCHAR *AddrTbl, UCHAR MsbMask,
                                  UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan);
void    ATV_I2CWriteField32      (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
                                  UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan,
                                  UINT32 Val);
void    ATV_I2CWriteField32LE    (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
                                  UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan,
                                  UINT32 Val);
void    ATV_I2CWriteFragField32  (UCHAR DevAddr, UCHAR *AddrTbl, UCHAR MsbMask,
                                  UCHAR LsbMask, UCHAR LsbPos,   UCHAR FldSpan,
                                  UINT32 Val);
void    ATV_I2CWriteFragField32LE (UCHAR DevAddr, UCHAR *AddrTbl,
                                   UCHAR MsbMask, UCHAR LsbMask,
                                   UCHAR LsbPos,  UCHAR FldSpan, UINT32 Val);
UINT32  ATV_I2CReadRandField32  (UCHAR DevAddr, I2C_FIELD_INFO *FldInfo,
                                 UCHAR FldSpan);
void    ATV_I2CWriteRandField32 (UCHAR DevAddr, I2C_FIELD_INFO *FldInfo,
                                 UCHAR FldSpan, UINT32 Val);
UCHAR   ATV_I2C16ReadField8 (UCHAR DevAddr, UINT16 RegAddr, UCHAR Mask,
                             UCHAR BitPos);
void    ATV_I2C16WriteField8 (UCHAR DevAddr, UINT16 RegAddr, UCHAR Mask,
                              UCHAR BitPos, UCHAR FieldVal);
void    ATV_I2C16WriteOField8 (UCHAR DevAddr, UINT16 RegAddr, UCHAR Mask,
                               UCHAR BitPos, UCHAR *CurrVal, UCHAR FieldVal);
UINT32  ATV_I2C16ReadField32 (UCHAR DevAddr, UINT16 RegAddr, UCHAR MsbMask,
                              UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
UINT32  ATV_I2C16ReadFragField32 (UCHAR DevAddr, UINT16 *AddrTbl, UCHAR MsbMask,
                                  UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
UINT32  ATV_I2C16ReadField32LE (UCHAR DevAddr, UINT16 RegAddr, UCHAR MsbMask,
                                UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
UINT32  ATV_I2C16ReadFragField32LE (UCHAR DevAddr, UINT16 *AddrTbl, UCHAR MsbMask,
                                    UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
void    ATV_I2C16WriteField32 (UCHAR DevAddr, UINT16 RegAddr, UCHAR MsbMask,
                               UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val);
void    ATV_I2C16WriteField32LE (UCHAR DevAddr, UINT16 RegAddr, UCHAR MsbMask,
                                 UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val);
void    ATV_I2C16WriteFragField32 (UCHAR DevAddr, UINT16 *AddrTbl, UCHAR MsbMask,
                                   UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val);
void    ATV_I2C16WriteFragField32LE (UCHAR DevAddr, UINT16 *AddrTbl, UCHAR MsbMask,
                                     UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan, UINT32 Val);
UCHAR   ATV_Reg (UCHAR Dev, UCHAR Reg);
void    ATV_DelaySec (UINT16 Counter);
UINT32  ATV_GetElapsedMs (UINT32 StartCount, UINT32 *CurrMsCount);
UINT32  ATV_GetMsCountNZ (void);
UINT16  ATV_LookupValue8 (UCHAR *Table, UCHAR Value, UCHAR EndVal, UINT16 Step);
void    ATV_I2CWriteTable (UCHAR *Table, UCHAR EndVal);
void    ATV_I2C16WriteTable (UCHAR *Table, UCHAR EndVal);
void    ATV_I2CWriteFields (UCHAR *Table, UCHAR EndVal);
char   *ATV_LookupStrgTable (STRG_TABLE *Table, UCHAR StrgId, UCHAR EndVal);
void    ATV_PrintTime (char *Prefix, UCHAR Gran, char *Postfix);
void    ATV_CheckTimeOut (TIMEOUT_TABLE *Table);
ATV_ERR ATV_CecValidMsgSize (UCHAR *Table, UCHAR TableSize, UCHAR Opcode,
                             UCHAR MsgSize, UCHAR *ExpMsgSize);

#endif
