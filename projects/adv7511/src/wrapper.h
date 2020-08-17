/**********************************************************************************************
*																						      *
* Copyright (c) 2012 Analog Devices, Inc.  All Rights Reserved.                               *
* This software is proprietary and confidential to Analog Devices, Inc. and its licensors.    *
*                                                                                             *
***********************************************************************************************/
#ifndef _WRAPPER_H_
#define _WRAPPER_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "xil_printf.h"

/* The values of these definitions are currently set as per ADI evaluation platforms and the ADI example application*/
/* If the ADI example application is ported and customised it is important to review and set these options accordingly*/
/* Please refer to release build options file and DVP documentation for futher information*/

#define RX_DEVICE              0  /*Set the Target Rx  device. Tv Driver supports the following devices: 7844, 7842, 7604, 7840, 7612, 7611, 7614, 7622/3, 7619 */

#define TX_DEVICE              7511  /*Set the Target Tx Device. Tv Driver supports the following Tx devices: 7623/2, 7511, 7510, 7520 */
#define TX_USER_CONFIG         1  /*Always set to 1 when using ADI REP Middleware and ADI REP Application. */
#define TX_USER_INIT           0  /*Always set to 0. Set to 1 for customer initialisation custimization only. */
#define TX_CALLBACK_FUNCTION   TRANSMITTER_Notification

#define UART_DEBUG             1  /*Set to 1 to enable Debug message Printouts. Set to 0 to disable.  */
#define IGNORE_INT_LINES       1  /*Set to 1 to ignore hw interrupt pin status to determine if interrupt is pending. Software method used only*/
#define ADVANTIV               1
/*=======================================
 * Data types
 *======================================*/

#define STATIC      static
#define INLINE      inline
#define CONSTANT    const
#define EXTERNAL    extern
#define PACKED      __attribute__((packed))
#define PACKED_STR  struct PACKED

typedef unsigned char   UCHAR;          /* unsigned 8-bit */
typedef unsigned short  UINT16;         /* unsigned 16-bit */
typedef unsigned long   UINT32;         /* unsigned 32-bit */
typedef short int       INT16;
typedef long int        INT32;
typedef char            CHAR;

#define UINT8           UCHAR

#ifndef BOOL
#define BOOL            UCHAR
#endif

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

#ifndef NULL
#define NULL            ((void *)0)
#endif

#define ADV7511_MAIN_I2C_ADDR 0x39

#define ADV7511_MAIN_CEC_TX_STATUS 0x97
#define ADV7511_MAIN_CEC_TX_STATUS_RDY(x) (((x) << 5) & 0x20)
#define ADV7511_MAIN_CEC_TX_STATUS_ALI(x) (((x) << 4) & 0x10)
#define ADV7511_MAIN_CEC_TX_STATUS_RTI(x) (((x) << 3) & 0x08)

#define ADV7511_MAIN_CEC_PWRDWN 0xe2
#define ADV7511_MAIN_CEC_PWRDWN_SET(x) (((x) << 1) & 0x01)

#define ADV7511_CEC_I2C_ADDR  0x3c

#define ADV7511_CEC_TX_FRAME_HEADER 0x00

#define ADV7511_CEC_TX_FRAME_LENGTH       0x10
#define ADV7511_CEC_TX_FRAME_LENGTH_MASK  0x1f
#define ADV7511_CEC_TX_FRAME_LENGTH_SHIFT 0x00

#define ADV7511_CEC_TX_TRANS_ENABLE       0x11
#define ADV7511_CEC_TX_TRANS_ENABLE_MASK  0x01
#define ADV7511_CEC_TX_TRANS_ENABLE_SHIFT 0x00

#define ADV7511_CEC_TX_TRANSMISSION_EN 0x11
#define ADV7511_CEC_TX_TRANSMISSION_EN_SET(x) (((x) << 0) & 0x1)

#define ADV7511_CEC_TX_RETRY_COUNT 0x12
#define ADV7511_CEC_TX_RETRY_COUNT_MASK  0x70
#define ADV7511_CEC_TX_RETRY_COUNT_SHIFT 0x04
#define ADV7511_CEC_TX_RETRY_COUNT_SET(x) (((x) << 4) & 0x70)

#define ADV7511_CEC_TX_NACK_CNT       0x14
#define ADV7511_CEC_TX_NACK_CNT_MASK  0x0f
#define ADV7511_CEC_TX_NACK_CNT_SHIFT 0x00

#define ADV7511_CEC_RX_BUFF1_HDR 0x15

#define ADV7511_CEC_RX_ENABLE       0x26
#define ADV7511_CEC_RX_ENABLE_MASK  0x40
#define ADV7511_CEC_RX_ENABLE_SHIFT 0x06

#define ADV7511_CEC_RX_BUFF2_HDR 0x27
#define ADV7511_CEC_RX_BUFF3_HDR 0x38

#define ADV7511_CEC_RX_BUFFER_NUMBER       0x4a
#define ADV7511_CEC_RX_BUFFER_NUMBER_MASK  0x08
#define ADV7511_CEC_RX_BUFFER_NUMBER_SHIFT 0x03

#define ADV7511_CEC_RX_BUFFER1_RDY       0x4a
#define ADV7511_CEC_RX_BUFFER1_RDY_MASK  0x01
#define ADV7511_CEC_RX_BUFFER1_RDY_SHIFT 0x00
#define ADV7511_CEC_RX_BUFFER2_RDY       0x4a
#define ADV7511_CEC_RX_BUFFER2_RDY_MASK  0x02
#define ADV7511_CEC_RX_BUFFER2_RDY_SHIFT 0x01
#define ADV7511_CEC_RX_BUFFER3_RDY       0x4a
#define ADV7511_CEC_RX_BUFFER3_RDY_MASK  0x04
#define ADV7511_CEC_RX_BUFFER3_RDY_SHIFT 0x02

#define ADV7511_CEC_LOGICADDR_DEV01_REG 0x4c
#define ADV7511_CEC_LOGICADDR_DEV2_REG  0x4d

#define ADV7511_CEC_LOGICADDR_MASK       0x4b
#define ADV7511_CEC_LOGICADDR_MASK_MASK  0x70
#define ADV7511_CEC_LOGICADDR_MASK_SHIFT 0x04
#define ADV7511_CEC_LOGICADDR_MASK_ADDR0 0x01
#define ADV7511_CEC_LOGICADDR_MASK_ADDR1 0x02
#define ADV7511_CEC_LOGICADDR_MASK_ADDR2 0x04

#define ADV7511_CEC_LOGICADDR0       0x4c
#define ADV7511_CEC_LOGICADDR0_MASK  0x0f
#define ADV7511_CEC_LOGICADDR0_SHIFT 0x00
#define ADV7511_CEC_LOGICADDR1       0x4c
#define ADV7511_CEC_LOGICADDR1_MASK  0xf0
#define ADV7511_CEC_LOGICADDR1_SHIFT 0x04
#define ADV7511_CEC_LOGICADDR2       0x4d
#define ADV7511_CEC_LOGICADDR2_MASK  0x0f
#define ADV7511_CEC_LOGICADDR2_SHIFT 0x00

#define ADV7511_CEC_POWER_MODE       0x4e
#define ADV7511_CEC_POWER_MODE_MASK  0x03
#define ADV7511_CEC_POWER_MODE_SHIFT 0x00
#define ADV7511_CEC_POWER_MODE_SET(x) (((x) << 0) & 0x01)

#define ADV7511_CEC_SOFT_RESET       0x50
#define ADV7511_CEC_SOFT_RESET_MASK  0x01
#define ADV7511_CEC_SOFT_RESET_SHIFT 0x00
#define ADV7511_CEC_SOFT_RESET_SET(x) (((x) << 0) & 0x01)

typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char u8;

/*===========================================================
 * Defines and Macros
 *==========================================================*/
#define MAX_VIC_VALUE                   64
#define NUM_OF_VICS                     (MAX_VIC_VALUE+1)
#define CEC_RETRY_COUNT                 3
#define CEC_MAX_MSG_SIZE                16
#define CEC_TX_BUF_LEN                  30

EXTERNAL CONSTANT UINT16 VicInfo[];

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
#define PKT_AUDIO_SAMPLE_PACKET         0x0800
#define PKT_ACR_PACKET                  0x1000
#define PKT_ALL_PACKETS                 0xffff

#define CEC_TRIPLE_NUMBER               3

#if UART_DEBUG
#define DBG_MSG                 xil_printf
#else
#define DBG_MSG(...)
#endif

//#define memcpy 	adi_memcpy
//#define memset	adi_memset


//void adi_memcpy(void *dst,void* src, UINT32 count);
//void adi_memset(void *dst,UINT8 data, UINT32 count);


#if (RX_DEVICE == 7623) || (RX_DEVICE == 7622) || (RX_DEVICE == 76221)
#define RX_I2C_IO_MAP_ADDR              0xB2
#define RX2_I2C_IO_MAP_ADDR             0xB0
#elif (RX_DEVICE == 7612) || (RX_DEVICE == 7611) || (RX_DEVICE == 7619)
#define RX_I2C_IO_MAP_ADDR              0x98
#define RX2_I2C_IO_MAP_ADDR             0x98
#elif (RX_DEVICE == 7630)
#define RX_I2C_IO_MAP_ADDR              0xB0
#else
#define RX_I2C_IO_MAP_ADDR              0x40
#define RX2_I2C_IO_MAP_ADDR             0xB4
#endif

#define RX_I2C_SDP_VDP_MAP_ADDR         0x22
#define RX_I2C_AFE_DPLL_MAP_ADDR        0x30
#define RX_I2C_ESDP_MAP_ADDR            0x34    /* Def 0x70 */
#define RX_I2C_SDP_IO_MAP_ADDR          0x42
#define RX_I2C_CP_MAP_ADDR              0x44
#define RX_I2C_VDP_MAP_ADDR             0x48
#define RX_I2C_TEST_MAP3_ADDR           0x52
#define RX_I2C_TEST_MAP1_ADDR           0x60
#define RX_I2C_TEST_MAP2_ADDR           0x62
#define RX_I2C_REPEATER_MAP_ADDR        0x64
#define RX_I2C_HDMI_MAP_ADDR            0x68
#define RX_I2C_EDID_MAP_ADDR            0x6C
#define RX_I2C_DPP_MAP_ADDR             0x74    /* Def 0x78 */
#define RX_I2C_INFOFRAME_MAP_ADDR       0x76    /* 0x7C on ATV_MB seems to cause readback of all 0x00, occasionally. Happens more often when no Sink is connected */
#define RX_I2C_CEC_MAP_ADDR             0x80
#define RX_I2C_SDP_MAP_ADDR             0x82
#define RX_I2C_AVLINK_MAP_ADDR          0x84
#define RX_I2C_OSD_MAP_ADDR             0x88
#define RX_I2C_AUDIO_CODEC_MAP_ADDR     0x5C
#define RX_I2C_XMEM_MAP_ADDR            0xA8
#define RX_I2C_VFE_MAP_ADDR             0xA0



#define RX2_I2C_SDP_VDP_MAP_ADDR        0xC0
#define RX2_I2C_AFE_DPLL_MAP_ADDR       0xC2
#define RX2_I2C_ESDP_MAP_ADDR           0xC4    /* Def 0x70 */
#define RX2_I2C_SDP_IO_MAP_ADDR         0xC6
#define RX2_I2C_CP_MAP_ADDR             0xC8
#define RX2_I2C_VDP_MAP_ADDR            0xCA
#define RX2_I2C_TEST_MAP3_ADDR          0xCC
#define RX2_I2C_TEST_MAP1_ADDR          0xCE
#define RX2_I2C_TEST_MAP2_ADDR          0xD0
#define RX2_I2C_REPEATER_MAP_ADDR       0xD2
#define RX2_I2C_HDMI_MAP_ADDR           0xD4
#define RX2_I2C_EDID_MAP_ADDR           0xD6
#define RX2_I2C_DPP_MAP_ADDR            0xD8    /* Def 0x78 */
#define RX2_I2C_INFOFRAME_MAP_ADDR      0xDA
#define RX2_I2C_CEC_MAP_ADDR            0xDC
#define RX2_I2C_SDP_MAP_ADDR            0xDE
#define RX2_I2C_AVLINK_MAP_ADDR         0xE0
#define RX2_I2C_OSD_MAP_ADDR            0xE2
#define RX2_I2C_VFE_MAP_ADDR            RX_I2C_VFE_MAP_ADDR
#define RX2_I2C_AUDIO_CODEC_MAP_ADDR    RX_I2C_AUDIO_CODEC_MAP_ADDR
#define RX2_I2C_XMEM_GAMMA_MAP_ADDR     RX_I2C_XMEM_MAP_ADDR


#if ( (TX_DEVICE == 7511) && (MULTI_RX_NUM))
#define TX_I2C_MAIN_MAP_ADDR            0x7A
#define TX2_I2C_MAIN_MAP_ADDR           0x72
#else
#define TX_I2C_MAIN_MAP_ADDR            0x72
#define TX2_I2C_MAIN_MAP_ADDR           0x7A
#endif

#define TX_I2C_PKT_MEM_MAP_ADDR         0x70
#define TX_I2C_CEC_MAP_ADDR             0x78
#if ( (TX_DEVICE == 7511) && (TX_NUM_OF_DEVICES > 1))
#define TX_I2C_EDID_MAP_ADDR            0xFE
#else
#define TX_I2C_EDID_MAP_ADDR            0x7E
#endif
#if ( RX_NUM_OF_DEVICES > 1 )
#define TX2_I2C_PKT_MEM_MAP_ADDR        0x7C
#else
#define TX2_I2C_PKT_MEM_MAP_ADDR        0x76
#endif
#define TX2_I2C_CEC_MAP_ADDR            0x82
#define TX2_I2C_EDID_MAP_ADDR           0x86

#define TX_INCLUDE_CEC                  1
#define TX_EDID_RETRY_COUNT             8

#define TX_NUM_OF_DEVICES               1

/*==========================================
 * System wide configurations
 *=========================================*/
#if (RX_DEVICE == 7612) || (RX_DEVICE == 7611) || (RX_DEVICE == 7619)
#define REP_SUPPORTED_DS_DEVICE_COUNT   12
#elif (RX_DEVICE == 7850)
#define REP_SUPPORTED_DS_DEVICE_COUNT   127
#else
#define REP_SUPPORTED_DS_DEVICE_COUNT   12
/* Maximum is 24, limited by RX capacity */
#endif

#define REP_SUPPORTED_EDID_SEGMENTS     2
/* Maximum is 2, limited by RX capacity */


#define TX_SUPPORTED_DS_DEVICE_COUNT    REP_SUPPORTED_DS_DEVICE_COUNT
#define TX_SUPPORTED_EDID_SEGMENTS      REP_SUPPORTED_EDID_SEGMENTS



/*========================================
 * CEC macros
 *=======================================*/
#define CEC_RX_BUFFER1                         0
#define CEC_RX_BUFFER2                         1
#define CEC_RX_BUFFER3                         2
#define CEC_TRIPLE_NUMBER                      3


/*========================================
 * Enums and structures
 *=======================================*/
typedef enum {
	ATVERR_OK=0,
	ATVERR_FALSE=0,
	ATVERR_TRUE=1,
	ATVERR_INV_PARM,
	ATVERR_NOT_AVAILABLE,
	ATVERR_FAILED
} ATV_ERR;


enum {
	CEC_EVENT_RX_MSG,
	CEC_EVENT_TX_DONE,
	CEC_EVENT_TX_TIMEOUT,
	CEC_EVENT_TX_ARB_LOST,
	CEC_EVENT_LOG_ADDR_ALLOC,
	CEC_EVENT_LOG_ADDR_LIST,
	CEC_EVENT_RX_MSG_RESPOND
};

typedef struct {
	BOOL TxReady;
	BOOL RxReady;
	BOOL ArbLost;
	BOOL Timeout;
	BOOL RxReady1;
	BOOL RxReady2;
	BOOL RxReady3;
	UCHAR RxFrameOrder[CEC_TRIPLE_NUMBER];
} CEC_INTERRUPTS;

/*========================================
 * Auxiliary 8-bit I2C field access macros
 *=======================================*/
#define ATV_I2CIsField8                             (BOOL)ATV_I2CReadField8
#define ATV_I2CGetField8(d,r,m,b,p)                 *p=ATV_I2CReadField8(d,r,m,b)
#define ATV_I2CGetField32(d,r,Mm,Lm,b,s,p)          *p=ATV_I2CReadField32(d,r,Mm,Lm,b,s)
#define ATV_I2CGetField32LE(d,r,Mm,Lm,b,s,p)        *p=ATV_I2CReadField32LE(d,r,Mm,Lm,b,s)
#define ATV_I2CGetMultiField(d,r,s,p)               HAL_I2CReadBlock(d,r,p,(UINT16)s)

struct cec_status {
	uint8_t tx_header;
	uint8_t tx_tail;
	uint8_t tx_cnt;
	uint8_t tx_buf[CEC_TX_BUF_LEN];
};

enum cec_tx_state_machine {
	CEC_TX_STATE_DONE,
	CEC_TX_STATE_BUSY
};

enum cec_operations {
	CEC_OP_NONE,
	CEC_OP_LOG_ADDR_ALLOC,
	CEC_OP_GET_LOG_ADDR_LIST
};

void    HAL_DelayMs (UINT16 Counter);
UCHAR   HAL_I2CReadByte (UCHAR Dev, UCHAR Reg, UCHAR *Data);
UCHAR   HAL_I2CWriteByte (UCHAR Dev, UCHAR Reg, UCHAR Data);
UINT16  HAL_I2CReadBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data,
			  UINT16 NumberBytes);
UINT16  HAL_I2CWriteBlock (UCHAR Dev, UCHAR Reg, UCHAR *Data,
			   UINT16 NumberBytes);
UCHAR HAL_SetRxChipSelect(UCHAR DevIdx);
void WaitMilliSec(unsigned int msec);
void DBG_Printf(const char *data, ...);


UCHAR   ATV_I2CReadField8   (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
			     UCHAR BitPos);
void    ATV_I2CWriteField8  (UCHAR DevAddr, UCHAR RegAddr, UCHAR Mask,
			     UCHAR BitPos,  UCHAR FieldVal);
UINT32  ATV_I2CReadField32  (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
			     UCHAR LsbMask, UCHAR LsbPos, UCHAR FldSpan);
UINT32  ATV_I2CReadField32LE   (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
				UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan);
void    ATV_I2CWriteField32      (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
				  UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan,
				  UINT32 Val);
void    ATV_I2CWriteField32LE    (UCHAR DevAddr, UCHAR RegAddr, UCHAR MsbMask,
				  UCHAR LsbMask, UCHAR LsbPos,  UCHAR FldSpan,
				  UINT32 Val);
void    ATV_I2CWriteFields (UCHAR *Table, UCHAR EndVal);
void    ATV_I2CWriteTable (UCHAR *Table, UCHAR EndVal);
UINT16  ATV_LookupValue8 (UCHAR *Table, UCHAR Value, UCHAR EndVal, UINT16 Step);
void    ATV_PrintTime (char *Prefix, UCHAR Gran, char *Postfix);

UINT32  ATV_GetElapsedMs (UINT32 StartCount, UINT32 *CurrMsCount);
UINT32  ATV_GetMsCountNZ (void);

ATV_ERR CEC_Reset (void);
ATV_ERR CEC_Enable (BOOL Enable);
ATV_ERR CEC_SetLogicalAddr (UCHAR LogAddr, UCHAR DevId, BOOL Enable);
ATV_ERR CEC_SendMessage (UCHAR *MsgPtr, UCHAR MsgLen);
ATV_ERR CEC_SendMessageOut (void);
ATV_ERR CEC_ResendLastMessage (void);
ATV_ERR CEC_AllocateLogAddr (UCHAR *LogAddrList);
void  CEC_Isr (CEC_INTERRUPTS *CecInts);
BOOL HAL_GetMBSwitchState();
uint32_t HAL_GetCurrentMsCount();
void uart_int_handler(void *instance);
bool HAL_TxIntPending();

#endif
