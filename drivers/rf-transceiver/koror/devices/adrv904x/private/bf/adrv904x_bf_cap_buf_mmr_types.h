/** 
 * \file adrv904x_bf_cap_buf_mmr_types.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_CAP_BUF_MMR_TYPES_H_
#define _ADRV904X_BF_CAP_BUF_MMR_TYPES_H_

typedef enum adrv904x_BfCapBufMmrChanAddr
{
    ADRV904X_BF_PROC_DFE_PERIP_CAPBUF_0_    =    (int32_t)0xB2000000,
    ADRV904X_BF_PROC_DFE_PERIP_CAPBUF_1_    =    (int32_t)0xB2100000
} adrv904x_BfCapBufMmrChanAddr_e;

/** 
 * \brief Enumeration for capEnMode
 */

typedef enum adrv904x_Bf_CapBufMmr_CapEnMode
{
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_MODE_MULTI_CAP_PER_ENABLE            =    0,  /*!< Multiple Captures in a Capture Enable Event Duration                                                                */
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_MODE_SINGLE_CAP_PER_ENABLE           =    1   /*!< Single Capture in a Capture Enable Event Duration                                                                   */
} adrv904x_Bf_CapBufMmr_CapEnMode_e;

/** 
 * \brief Enumeration for capEnSrc
 */

typedef enum adrv904x_Bf_CapBufMmr_CapEnSrc
{
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_SW                       =    0,  /*!< Selects Enable from Software Source                                                                                 */
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_RADIO_SEQUENCER          =    1,  /*!< Selects Enable from Radio Sequencer                                                                                 */
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_GPIO                     =    2,  /*!< Selects Enable from GPIO                                                                                            */
    ADRV904X_BF_CAP_BUF_MMR_CAP_EN_SRC_CAP_EN_NONE                     =    3   /*!< Reserved                                                                                                            */
} adrv904x_Bf_CapBufMmr_CapEnSrc_e;

/** 
 * \brief Enumeration for capPauseSrc
 */

typedef enum adrv904x_Bf_CapBufMmr_CapPauseSrc
{
    ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_SW                    =    0,  /*!< Selects Pause from Software Source                                                                                  */
    ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_RADIO_SEQUENCER       =    1,  /*!< Selects Pause from Radio Sequencer                                                                                  */
    ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_GPIO                  =    2,  /*!< Selects Pause from GPIO                                                                                             */
    ADRV904X_BF_CAP_BUF_MMR_CAP_PAUSE_SRC_CAP_PAUSE_SLICE                 =    3   /*!< Selects Pause from TX Slice                                                                                         */
} adrv904x_Bf_CapBufMmr_CapPauseSrc_e;

/** 
 * \brief Enumeration for ch0SrcSel
 */

typedef enum adrv904x_Bf_CapBufMmr_Ch0SrcSel
{
    ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_TX                              =    0,  /*!< Select TX as Channel 0 Data Source                                                                                  */
    ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_ALT_TX                          =    1,  /*!< Select ALT_TX as Channel 0 Data Source                                                                              */
    ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_ORX                             =    2,  /*!< Select ORX as Channel 0 Data Source                                                                                 */
    ADRV904X_BF_CAP_BUF_MMR_CH0_SRC_SEL_RSVD                            =    3   /*!< Reserved                                                                                                            */
} adrv904x_Bf_CapBufMmr_Ch0SrcSel_e;

/** 
 * \brief Enumeration for ch1SrcSel
 */

typedef enum adrv904x_Bf_CapBufMmr_Ch1SrcSel
{
    ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_TX                              =    0,  /*!< Select TX as Channel 1 Data Source                                                                                  */
    ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_ALT_TX                          =    1,  /*!< Select ALT_TX as Channel 1 Data Source                                                                              */
    ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_ORX                             =    2,  /*!< Select ORX as Channel 1 Data Source                                                                                 */
    ADRV904X_BF_CAP_BUF_MMR_CH1_SRC_SEL_RSVD                            =    3   /*!< Reserved                                                                                                            */
} adrv904x_Bf_CapBufMmr_Ch1SrcSel_e;

/** 
 * \brief Enumeration for ch2SrcSel
 */

typedef enum adrv904x_Bf_CapBufMmr_Ch2SrcSel
{
    ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_TX                              =    0,  /*!< Select TX as Channel 2 Data Source                                                                                  */
    ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_ALT_TX                          =    1,  /*!< Select ALT_TX as Channel 2 Data Source                                                                              */
    ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_ORX                             =    2,  /*!< Select ORX as Channel 2 Data Source                                                                                 */
    ADRV904X_BF_CAP_BUF_MMR_CH2_SRC_SEL_RSVD                            =    3   /*!< Reserved                                                                                                            */
} adrv904x_Bf_CapBufMmr_Ch2SrcSel_e;

/** 
 * \brief Enumeration for frameDelayRestartCtrl
 */

typedef enum adrv904x_Bf_CapBufMmr_FrameDelayRestartCtrl
{
    ADRV904X_BF_CAP_BUF_MMR_FRAME_DELAY_RESTART_CTRL_ON_CAP_TRIGGER_RISING_EDGE      =    0,  /*!< Restarts on Capture Trigger Rising Edge                                                                             */
    ADRV904X_BF_CAP_BUF_MMR_FRAME_DELAY_RESTART_CTRL_ON_CAP_EN_RISING_EDGE           =    1   /*!< Restarts on Capture Enable Rising Edge                                                                              */
} adrv904x_Bf_CapBufMmr_FrameDelayRestartCtrl_e;

#endif // _ADRV904X_BF_CAP_BUF_MMR_TYPES_H_

