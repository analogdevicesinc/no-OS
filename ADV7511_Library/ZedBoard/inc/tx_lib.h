/***************************************************************************//**
 *   @file   tx_lib.h
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

#ifndef _TX_LIB_H_
#define _TX_LIB_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "atv_common.h"

/******************************************************************************/
/************************** Types Declarations ********************************/
/******************************************************************************/
typedef enum
{
    TX_PD_MODE1,                /* Only HPD, Rx sense and CEC are active */
    TX_PD_MODE2,                /* Only CEC is active */
    TX_PD_MODE3                 /* Entire chip is powered down */
} TX_PD_MODE;

typedef enum
{
    TX_INIT_ON_HPD_LOW      =0x0001,    /* Init TX h/w when HPD goes low    */
    TX_INIT_ON_HPD_HIGH     =0x0002,    /* Init TX h/w when HPD goes hi     */
    TX_INIT_ON_EDID_ERROR   =0x0004,    /* Init TX h/w on bad EDID seg cnt  */
    TX_HDCP_DISABLE_ON_ERROR=0x0008,    /* Disable HDCP on any HDCP error   */
    TX_ENABLE_TMDS_ON_INIT  =0x0010,    /* Enable TMDS lines after init     */
    TX_ENABLE_DBG           =0x1000
} TX_CONFIG;

typedef enum
{
    TX_EVENT_HPD_CHG            = 0x0001,
    TX_EVENT_MSEN_CHG           = 0x0002,
    TX_EVENT_EDID_READY         = 0x0004,
    TX_EVENT_AUTHENTICATED      = 0x0008,
    TX_EVENT_HDCP_ERROR         = 0x0010,
    TX_EVENT_VSYNC_INT          = 0x0020,
    TX_EVENT_AUD_FIFO_FULL      = 0x0040,
    TX_EVENT_EMB_SYNC_ERR       = 0x0080,
    TX_EVENT_BKSV_READY         = 0x0100,
    TX_EVENT_CEC_RX_MSG         = 0x0200,
    TX_EVENT_CEC_TX_DONE        = 0x0400,
    TX_EVENT_CEC_TX_TIMEOUT     = 0x0800,
    TX_EVENT_CEC_TX_ARB_LOST    = 0x1000,
    TX_EVENT_CEC_LOG_ADDR_ALLOC = 0x2000,
    TX_EVENT_CEC_RX_MSG_RESPOND = 0x4000,
    TX_EVENT_HDMI_EVENTS        = 0x011F,
    TX_EVENT_CEC_EVENTS         = 0x7E00,
    TX_EVENT_ALL_EVENTS         = 0xFFFF
} TX_EVENT;

typedef enum
{
    SDR_444_SEP_SYNC,
    SDR_422_SEP_SYNC,
    SDR_422_EMP_SYNC,
    SDR_422_SEP_SYNC_2X_CLK,
    SDR_422_EMB_SYNC_2X_CLK,
    DDR_444_SEP_SYNC,
    DDR_422_SEP_SYNC,
    DDR_422_EMB_SYNC
} TX_IN_FORMAT;

typedef enum
{
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_EVEN
} TX_CHAN_ALIGN;

typedef enum
{
    OUT_ENC_YUV_422,
    OUT_ENC_YUV_444,
    OUT_ENC_RGB_444
} TX_OUT_ENCODING;

typedef enum
{
    REFRESH_NORMAL,
    REFRESH_LOW,
    REFRESH_2X,
    REFRESH_4X
} TX_REFR_RATE;

typedef enum
{
    TX_DC_TRUNCATE,
    TX_DC_ACTIVE_DITHER
} TX_DC_METHOD;

typedef enum
{
    TX_CS_RGB,
    TX_CS_YUV_601,
    TX_CS_YUV_709,
    TX_CS_YCC_601,
    TX_CS_YCC_709,
    TX_CS_RGB_FULL_RANGE,
    TX_CS_YCC_601_FULL_RANGE,
    TX_CS_YCC_709_FULL_RANGE,
    TX_CS_YCC_601_219,
    TX_CS_YCC_709_219,
    TX_CS_YCC_601_255,
    TX_CS_YCC_709_255,
    TX_CS_AUTO
} TX_CS_MODE;

typedef enum
{
    TX_I2S_STD,
    TX_I2S_RJUST,
    TX_I2S_LJUST,
    TX_I2S_AES3,
    TX_I2S_SPDIF,
    TX_SPDIF,
    TX_DSD_NORM,
    TX_DSD_SDIF3,
    TX_DSD_DST,
    TX_DSD_DST_SDR,
    TX_DSD_DST_DDR
} TX_AUD_FORMAT;

typedef enum
{
    AUD_SAMP_PKT,
    HBR_STRM_PKT,
    ONE_BIT_ASP,
    DST_AUD_PKT
} TX_AUD_PKT_TYPE;

typedef enum
{
    CH0_LEFT=0,             /* Sequential value is used */
    CH0_RIGHT,
    CH1_LEFT,
    CH1_RIGHT,
    CH2_LEFT,
    CH2_RIGHT,
    CH3_LEFT,
    CH3_RIGHT
} TX_AUD_CHAN;

typedef enum
{
    TX_MCLK_128FS=0,
    TX_MCLK_256FS,
    TX_MCLK_384FS,
    TX_MCLK_512FS,
    TX_MCLK_HBR,
    TX_MCLK_AUTO
} TX_MCLK_FREQ;

typedef enum
{
    TX_FS_32KHZ,
    TX_FS_44KHZ,
    TX_FS_48KHZ,
    TX_FS_88KHZ,
    TX_FS_96KHZ,
    TX_FS_176KHZ,
    TX_FS_192KHZ,
    TX_FS_HBR,
    TX_FS_FROM_STRM
} TX_AUD_FS;

typedef struct
{
    UCHAR   CsConsumer;
    UCHAR   CsSampType;
    UCHAR   CsCopyright;
    UCHAR   CsEmphasis;
    UCHAR   CsCatCode;
    UCHAR   CsSrcNum;
    UCHAR   CsChanNum;
    UCHAR   CsSampFreq;
    UCHAR   CsClkAccur;
    UCHAR   CsWordLen;
}TX_CHAN_STATUS;

typedef enum
{
    TX_AUD_IN_I2S0,
    TX_AUD_IN_I2S1,
    TX_AUD_IN_I2S2,
    TX_AUD_IN_I2S3,
    TX_AUD_IN_I2S,
    TX_AUD_IN_SPDIF,
    TX_AUD_IN_DSD0,
    TX_AUD_IN_DSD1,
    TX_AUD_IN_DSD2,
    TX_AUD_IN_DSD3,
    TX_AUD_IN_DSD4,
    TX_AUD_IN_DSD5,
    TX_AUD_IN_DSD6,
    TX_AUD_IN_DSD7,
    TX_AUD_IN_DSD,
    TX_AUD_IN_DST,
    TX_AUD_IN_ALL
} TX_AUD_INTERFACE;

typedef enum
{
    TX_OUT_MODE_HDMI,
    TX_OUT_MODE_DVI
} TX_OUTPUT_MODE;

typedef enum
{
    TX_HDCP_NO_DS_DEVICE,
    TX_HDCP_DISABLED,
    TX_HDCP_BSTATUS_READY,
    TX_HDCP_BKSV_LIST_READY,
    TX_HDCP_AUTHENTICATED,
    TX_HDCP_RESET
} TX_HDCP_STATE;

typedef enum
{
    TX_HDCP_ERR_NONE,
    TX_HDCP_ERR_BAD_RECV_BKSV,
    TX_HDCP_ERR_RI_MISMATCH,
    TX_HDCP_ERR_PJ_MISMATCH,
    TX_HDCP_ERR_I2C_ERROR,
    TX_HDCP_ERR_REP_DONE_TIMEOUT,
    TX_HDCP_ERR_MAX_CASCADE_EXCEEDED,
    TX_HDCP_ERR_V_DASH_CHECK_FAILED,
    TX_HDCP_ERR_MAX_DEVICE_EXCEEDED
} TX_HDCP_ERR;

typedef enum
{
    STATE_RESET     = 0,
    STATE_READ_EDID = 1,
    STATE_IDLE      = 2,
    STATE_INIT      = 3,
    STATE_DONE      = 4,
    STATE_REP_INIT  = 5
} TX_EDID_CTRL_STATE;

typedef struct
{
    BOOL    ChipPd;             /* TRUE/FALSE = Power down is On/Off    */
    BOOL    TmdsPd;             /* TRUE/FALSE = TMDS Power is Off/On    */
    BOOL    Hpd;                /* TRUE/FALSE = HPD is On/Off           */
    BOOL    MonSen;             /* TRUE/FALSE = Monitor sense is On/Off */
    BOOL    OutputHdmi;         /* TRUE/FALSE = Output is HDMI/DVI      */
    BOOL    PllLocked;          /* TRUE/FALSE = PLL locked/unlocked     */
    BOOL    VideoMuted;         /* TRUE/FALSE = Video is muted/un-muted */
    BOOL    ClearAVMute;        /* TRUE/FALSE = Clear AVMUTE On/Off     */
    BOOL    SetAVMute;          /* TRUE/FALSE = Set AVMUTE On/Off       */
    BOOL    AudioRep;           /* TRUE/FALSE = Audio repeat En/Dis     */
    BOOL    SpdifEnable;        /* TRUE/FALSE = SPDIF enabled/Disable   */
    UCHAR   I2SEnable;          /* Bits 0-3 = I2S 0-3 enable status     */
    UCHAR   DetectedVic;        /* Detected VIC                         */
    UINT16  LastHdcpErr;        /* Last HDCP errors                     */
} TX_STATUS;

typedef enum
{
    TX_AVMUTE_ON,
    TX_AVMUTE_OFF,
    TX_AVMUTE_NONE,
    TX_AVMUTE_BOTH
} TX_AVMUTE;

typedef enum
{
    TX_ARC_OFF = 0,
    TX_ARC_SINGLE,
    TX_ARC_COMMON
} TX_ARC_MODE;

typedef enum
{
    TX_VIDEO_CLK_DELAY_M_1200PS = 0,
    TX_VIDEO_CLK_DELAY_M_800PS,
    TX_VIDEO_CLK_DELAY_M_400PS,
    TX_VIDEO_CLK_DELAY_NULL,
    TX_VIDEO_CLK_DELAY_P_400PS,
    TX_VIDEO_CLK_DELAY_P_800PS,
    TX_VIDEO_CLK_DELAY_P_1200PS,
    TX_VIDEO_CLK_DELAY_P_1600PS
} TX_VIDEO_CLK_DELAY;


/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
ATV_ERR ADIAPI_TxSetDeviceIndex (UCHAR DevIdx);
ATV_ERR ADIAPI_TxGetDeviceIndex (UCHAR *DevIdx);
ATV_ERR ADIAPI_TxInit (BOOL FullInit);
ATV_ERR ADIAPI_TxShutdown (TX_PD_MODE PdMode);
ATV_ERR ADIAPI_TxSetConfig (TX_CONFIG UserConfig, BOOL Set);
ATV_ERR ADIAPI_TxIsr (void);
ATV_ERR ADIAPI_TxIntPending (void);
ATV_ERR ADIAPI_TxSetEnabledEvents (TX_EVENT Events, BOOL Enable);
ATV_ERR ADIAPI_TxGetChipRevision (UINT16 *TxRev);
ATV_ERR ADIAPI_TxOverrideHpdPd (BOOL Override);
ATV_ERR ADIAPI_TxEnableTmds (BOOL Enable, BOOL SoftOn);
ATV_ERR ADIAPI_TxSetInputPixelFormat (UCHAR BitsPerColor, TX_IN_FORMAT Format, UCHAR Style, TX_CHAN_ALIGN Align, BOOL RisingEdge, BOOL BitSwap);
ATV_ERR ADIAPI_TxSetInputVideoClock (UCHAR ClkDivide);
ATV_ERR ADIAPI_TxSetOutputPixelFormat (TX_OUT_ENCODING OutEnc, BOOL Interpolate);
ATV_ERR ADIAPI_TxSetManualPixelRepeat (UCHAR Vic, UCHAR Factor, UCHAR PrValue);
ATV_ERR ADIAPI_TxSetAutoPixelRepeat (UCHAR Mode, UCHAR Vic, TX_REFR_RATE RefRate, UCHAR AspectRatio);
ATV_ERR ADIAPI_TxSetOutputColorDepth (UCHAR Depth, TX_DC_METHOD DcMethod);
ATV_ERR ADIAPI_TxSetCSC (TX_CS_MODE InColorSpace, TX_CS_MODE OutColorSpace);
ATV_ERR ADIAPI_TxSetAudioInterface (TX_AUD_FORMAT InputFormat, TX_AUD_PKT_TYPE OutType, UCHAR HbrStrmCount);
ATV_ERR ADIAPI_TxSetAudChanMapping (TX_AUD_CHAN InChan, TX_AUD_CHAN OutSample);
ATV_ERR ADIAPI_TxSetAudNValue (UINT32 NValue);
ATV_ERR ADIAPI_TxSetAudCTS (UINT32 CTS);
ATV_ERR ADIAPI_TxSetAudMCLK (TX_MCLK_FREQ MClk);
ATV_ERR ADIAPI_TxSetAudClkPolarity (BOOL RisingEdge);
ATV_ERR ADIAPI_TxSetAudChStatSampFreq (TX_AUD_FS SampFreq);
ATV_ERR ADIAPI_TxSetAudChanStatus (BOOL FromStream, TX_CHAN_STATUS *ChanStat);
ATV_ERR ADIAPI_TxAudInputEnable (TX_AUD_INTERFACE Interface, BOOL Enable);
ATV_ERR ADIAPI_TxSetI2sInput(UCHAR ChanCount, UCHAR ChanAlloc, TX_AUD_PKT_TYPE AudType);
ATV_ERR ADIAPI_TxSetOutputMode (TX_OUTPUT_MODE OutMode);
ATV_ERR ADIAPI_TxHdcpEnable (BOOL EncEnable, BOOL FrameEncEnable);
ATV_ERR ADIAPI_TxGetBksvList (UCHAR *BksvList, UCHAR *NumOfBksvs);
ATV_ERR ADIAPI_TxGetBstatus (UINT16 *Bstatus, UCHAR *Bcaps);
ATV_ERR ADIAPI_TxGetHdcpState (TX_HDCP_STATE *HdcpState);
ATV_ERR ADIAPI_TxGetLastHdcpError (TX_HDCP_ERR *Error);
ATV_ERR ADIAPI_TxGetEdidSegment (UCHAR SegNum, UCHAR *SegBuf);
ATV_ERR ADIAPI_TxGetHpdMsenState (BOOL *Hpd, BOOL *Msen);
ATV_ERR ADIAPI_TxGetEdidControllerState (TX_EDID_CTRL_STATE *State);
ATV_ERR ADIAPI_TxOutputModeHdmi (BOOL *IsHdmi);
ATV_ERR ADIAPI_TxHdcpEnabled (BOOL *HdcpOn);
ATV_ERR ADIAPI_TxOutputEncrypted (BOOL *Encrypted);
ATV_ERR ADIAPI_TxPllLocked (BOOL *Locked);
ATV_ERR ADIAPI_TxGetStatus (TX_STATUS *TxStat);
ATV_ERR ADIAPI_TxMuteAudio (BOOL Mute);
ATV_ERR ADIAPI_TxMuteVideo (BOOL Mute);
ATV_ERR ADIAPI_TxSetAvmute (TX_AVMUTE State);
ATV_ERR ADIAPI_TxGetAvmute (TX_AVMUTE *State);
ATV_ERR ADIAPI_TxEnablePackets (UINT16 Packets, BOOL Enable);
ATV_ERR ADIAPI_TxGetEnabledPackets (UINT16 *Packets);
ATV_ERR ADIAPI_TxSendAVInfoFrame (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendAudioInfoFrame (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendACPPacket (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendSPDPacket (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendISRC1Packet (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendISRC2Packet (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendGMDPacket (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendMpegPacket (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendSpare1Packet (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxSendSpare2Packet (UCHAR *Packet, UCHAR Size);
ATV_ERR ADIAPI_TxCecSetActiveDevice (UCHAR DevIndex);
ATV_ERR ADIAPI_TxCecEnable (BOOL Enable);
ATV_ERR ADIAPI_TxCecReset (void);
ATV_ERR ADIAPI_TxCecSendMessage (UCHAR *MsgPtr, UCHAR MsgLen);
ATV_ERR ADIAPI_TxCecSendMessageOut(void);
ATV_ERR ADIAPI_TxCecResendLastMessage (void);
ATV_ERR ADIAPI_TxCecReadMessage (UCHAR *MsgPtr, UCHAR *MsgLen);
ATV_ERR ADIAPI_TxCecSetLogicalAddr (UCHAR LogAddr, UCHAR DevId, BOOL Enable);
ATV_ERR ADIAPI_TxCecAllocateLogAddr (UCHAR *LogAddrList);
ATV_ERR ADIAPI_TxCecGetStatus (UCHAR *Status);
ATV_ERR ADIAPI_TxArcSetMode (TX_ARC_MODE Mode);
ATV_ERR ADIAPI_TxSetVideoClkDelay (TX_VIDEO_CLK_DELAY Delay);
ATV_ERR ADIAPI_TxAdjustFreqRange (UCHAR Vic);

#endif
