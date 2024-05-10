/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file adi_adrv904x_dfe_svc_radio_ctrl_sequencer_t.h
 * \brief Contains data types for DFE SDK Radio Sequencer Service
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef __ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__
#define __ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__

#include <stddef.h>

/**
 *  \brief Enum of Radio Sequencers
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_0, /*!< ID for Radio Sequencer 0 (Frame Timing 0) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_1, /*!< ID for Radio Sequencer 1 (Frame Timing 1) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_2, /*!< ID for Radio Sequencer 2 (Frame Timing 2) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_3, /*!< ID for Radio Sequencer 3 (Frame Timing 3) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_4, /*!< ID for Radio Sequencer 4 (Frame Timing 4) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_5, /*!< ID for Radio Sequencer 5 (Frame Timing 5) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_6, /*!< ID for Radio Sequencer 6 (Frame Timing 6) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_7, /*!< ID for Radio Sequencer 7 (Frame Timing 7) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_8, /*!< ID for Radio Sequencer 8 (Frame Timing 8) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_9, /*!< ID for Radio Sequencer 9 (Frame Timing 9) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_0_CAPTURE,  /*!< ID for Radio Sequencer 10 (ORX 0 capture) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ORX_1_CAPTURE,  /*!< ID for Radio Sequencer 11 (ORX 1 capture) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ID_RESERVED_0,  /*!< ID for Radio Sequencer 12 (Reserved 0) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ID_RESERVED_1,  /*!< ID for Radio Sequencer 13 (Reserved 1) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ID_RESERVED_2,  /*!< ID for Radio Sequencer 14 (Reserved 2) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_ID_RESERVED_3,  /*!< ID for Radio Sequencer 15 (Reserved 3) */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_SEQUENCERS  /*!< Total number of sequencers */
} adi_adrv904x_DfeSvcRadioCtrlSequencerId_e;

/**
 *  \brief Enum of possible Radio Sequencers enable mask
 */
typedef enum
{
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_OFF      = 0x00,     /*!< No sequencers are enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_0   = 0x01,     /*!< Sequencer 0 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_1   = 0x02,     /*!< Sequencer 1 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_2   = 0x04,     /*!< Sequencer 2 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_3   = 0x08,     /*!< Sequencer 3 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_4   = 0x10,     /*!< Sequencer 4 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_5   = 0x20,     /*!< Sequencer 5 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_6   = 0x40,     /*!< Sequencer 6 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_7   = 0x80,     /*!< Sequencer 7 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_8   = 0x100,    /*!< Sequencer 8 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_9   = 0x200,    /*!< Sequencer 9 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_10  = 0x400,    /*!< Sequencer 10 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_11  = 0x800,    /*!< Sequencer 11 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_12  = 0x1000,   /*!< Sequencer 12 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_13  = 0x2000,   /*!< Sequencer 13 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_14  = 0x4000,   /*!< Sequencer 14 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_15  = 0x8000,   /*!< Sequencer 15 enabled */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_ALL = (ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_0 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_1 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_2 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_3 |
                                             ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_4 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_5 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_6 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_7 |
                                             ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_8 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_9 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_10 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_11 |
                                             ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_12 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_13 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_14 | ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_MASK_15)
                                            /*!< All Sequencers enabled */
} adi_adrv904x_DfeSvcRadioCtrlSequencersMask_e;

/**
 *  \brief Set of system signals controllable by the radio sequencer
 */
typedef enum
{
    /* TX capture buffer enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_0_CAP_BUF_EN                = 0x00, /*!< TX Channel 0 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_1_CAP_BUF_EN                = 0x01, /*!< TX Channel 1 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_2_CAP_BUF_EN                = 0x02, /*!< TX Channel 2 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_3_CAP_BUF_EN                = 0x03, /*!< TX Channel 3 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_4_CAP_BUF_EN                = 0x04, /*!< TX Channel 4 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_5_CAP_BUF_EN                = 0x05, /*!< TX Channel 5 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_6_CAP_BUF_EN                = 0x06, /*!< TX Channel 6 capture buffer enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_7_CAP_BUF_EN                = 0x07, /*!< TX Channel 7 capture buffer enable */

    /* TX capture buffer triggers */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_0_CAP_BUF_TRIG              = 0x08, /*!< TX Channel 0 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_1_CAP_BUF_TRIG              = 0x09, /*!< TX Channel 1 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_2_CAP_BUF_TRIG              = 0x0A, /*!< TX Channel 2 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_3_CAP_BUF_TRIG              = 0x0B, /*!< TX Channel 3 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_4_CAP_BUF_TRIG              = 0x0C, /*!< TX Channel 4 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_5_CAP_BUF_TRIG              = 0x0D, /*!< TX Channel 5 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_6_CAP_BUF_TRIG              = 0x0E, /*!< TX Channel 6 capture buffer trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_7_CAP_BUF_TRIG              = 0x0F, /*!< TX Channel 7 capture buffer trigger */

    /* Core stream triggers */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_0_TRIG             = 0x10, /*!< Core Stream 0 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_1_TRIG             = 0x11, /*!< Core Stream 1 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_2_TRIG             = 0x12, /*!< Core Stream 2 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_3_TRIG             = 0x13, /*!< Core Stream 3 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_4_TRIG             = 0x14, /*!< Core Stream 4 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_5_TRIG             = 0x15, /*!< Core Stream 5 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_6_TRIG             = 0x16, /*!< Core Stream 6 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_7_TRIG             = 0x17, /*!< Core Stream 7 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_8_TRIG             = 0x18, /*!< Core Stream 8 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_9_TRIG             = 0x19, /*!< Core Stream 9 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_10_TRIG            = 0x1A, /*!< Core Stream 10 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_11_TRIG            = 0x1B, /*!< Core Stream 11 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_12_TRIG            = 0x1C, /*!< Core Stream 12 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_13_TRIG            = 0x1D, /*!< Core Stream 13 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_14_TRIG            = 0x1E, /*!< Core Stream 14 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_15_TRIG            = 0x1F, /*!< Core Stream 15 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_16_TRIG            = 0x20, /*!< Core Stream 16 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_17_TRIG            = 0x21, /*!< Core Stream 17 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_18_TRIG            = 0x22, /*!< Core Stream 18 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_19_TRIG            = 0x23, /*!< Core Stream 19 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_20_TRIG            = 0x24, /*!< Core Stream 20 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_21_TRIG            = 0x25, /*!< Core Stream 21 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_22_TRIG            = 0x26, /*!< Core Stream 22 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_23_TRIG            = 0x27, /*!< Core Stream 23 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_24_TRIG            = 0x28, /*!< Core Stream 24 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_25_TRIG            = 0x29, /*!< Core Stream 25 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_26_TRIG            = 0x2A, /*!< Core Stream 26 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_27_TRIG            = 0x2B, /*!< Core Stream 27 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_28_TRIG            = 0x2C, /*!< Core Stream 28 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_29_TRIG            = 0x2D, /*!< Core Stream 29 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_30_TRIG            = 0x2E, /*!< Core Stream 30 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_CORE_STREAM_31_TRIG            = 0x2F, /*!< Core Stream 31 trigger */

    /* Radio processor interrupts */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_0               = 0x30, /*!< Radio processor interrupt 0 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_1               = 0x31, /*!< Radio processor interrupt 1 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_2               = 0x32, /*!< Radio processor interrupt 2 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_3               = 0x33, /*!< Radio processor interrupt 3 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_4               = 0x34, /*!< Radio processor interrupt 4 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_5               = 0x35, /*!< Radio processor interrupt 5 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_6               = 0x36, /*!< Radio processor interrupt 6 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_7               = 0x37, /*!< Radio processor interrupt 7 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_8               = 0x38, /*!< Radio processor interrupt 8 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_9               = 0x39, /*!< Radio processor interrupt 9 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_10              = 0x3A, /*!< Radio processor interrupt 10 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_11              = 0x3B, /*!< Radio processor interrupt 11 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_12              = 0x3C, /*!< Radio processor interrupt 12 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_13              = 0x3D, /*!< Radio processor interrupt 13 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_14              = 0x3E, /*!< Radio processor interrupt 14 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RADIO_PROC_INT_15              = 0x3F, /*!< Radio processor interrupt 15 */

    /* DFE processor interrupts */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_0                 = 0x40, /*!< DFE processor interrupt 0 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_1                 = 0x41, /*!< DFE processor interrupt 1 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_2                 = 0x42, /*!< DFE processor interrupt 2 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_3                 = 0x43, /*!< DFE processor interrupt 3 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_4                 = 0x44, /*!< DFE processor interrupt 4 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_5                 = 0x45, /*!< DFE processor interrupt 5 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_6                 = 0x46, /*!< DFE processor interrupt 6 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_7                 = 0x47, /*!< DFE processor interrupt 7 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_8                 = 0x48, /*!< DFE processor interrupt 8 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_9                 = 0x49, /*!< DFE processor interrupt 9 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_10                = 0x4A, /*!< DFE processor interrupt 10 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_11                = 0x4B, /*!< DFE processor interrupt 11 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_12                = 0x4C, /*!< DFE processor interrupt 12 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_13                = 0x4D, /*!< DFE processor interrupt 13 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_14                = 0x4E, /*!< DFE processor interrupt 14 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_15                = 0x4F, /*!< DFE processor interrupt 15 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_16                = 0x50, /*!< DFE processor interrupt 16 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_17                = 0x51, /*!< DFE processor interrupt 17 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_18                = 0x52, /*!< DFE processor interrupt 18 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_19                = 0x53, /*!< DFE processor interrupt 19 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_20                = 0x54, /*!< DFE processor interrupt 20 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_21                = 0x55, /*!< DFE processor interrupt 21 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_22                = 0x56, /*!< DFE processor interrupt 22 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_23                = 0x57, /*!< DFE processor interrupt 23 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_24                = 0x58, /*!< DFE processor interrupt 24 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_25                = 0x59, /*!< DFE processor interrupt 25 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_26                = 0x5A, /*!< DFE processor interrupt 26 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_27                = 0x5B, /*!< DFE processor interrupt 27 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_28                = 0x5C, /*!< DFE processor interrupt 28 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_29                = 0x5D, /*!< DFE processor interrupt 29 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_30                = 0x5E, /*!< DFE processor interrupt 30 */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DFE_PROC_INT_31                = 0x5F, /*!< DFE processor interrupt 31 */

    /* Digital GPIO enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_0_EN                  = 0x60, /*!< Digial GPIO 0 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_1_EN                  = 0x61, /*!< Digial GPIO 1 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_2_EN                  = 0x62, /*!< Digial GPIO 2 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_3_EN                  = 0x63, /*!< Digial GPIO 3 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_4_EN                  = 0x64, /*!< Digial GPIO 4 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_5_EN                  = 0x65, /*!< Digial GPIO 5 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_6_EN                  = 0x66, /*!< Digial GPIO 6 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_7_EN                  = 0x67, /*!< Digial GPIO 7 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_8_EN                  = 0x68, /*!< Digial GPIO 8 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_9_EN                  = 0x69, /*!< Digial GPIO 9 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_10_EN                 = 0x6A, /*!< Digial GPIO 10 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_11_EN                 = 0x6B, /*!< Digial GPIO 11 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_12_EN                 = 0x6C, /*!< Digial GPIO 12 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_13_EN                 = 0x6D, /*!< Digial GPIO 13 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_14_EN                 = 0x6E, /*!< Digial GPIO 14 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_15_EN                 = 0x6F, /*!< Digial GPIO 15 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_16_EN                 = 0x70, /*!< Digial GPIO 16 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_17_EN                 = 0x71, /*!< Digial GPIO 17 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_18_EN                 = 0x72, /*!< Digial GPIO 18 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_19_EN                 = 0x73, /*!< Digial GPIO 19 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_20_EN                 = 0x74, /*!< Digial GPIO 20 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_21_EN                 = 0x75, /*!< Digial GPIO 21 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_22_EN                 = 0x76, /*!< Digial GPIO 22 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_23_EN                 = 0x77, /*!< Digial GPIO 23 enable */

    /* Analog GPIO enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_0_EN               = 0x78, /*!< Analog GPIO 0 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_1_EN               = 0x79, /*!< Analog GPIO 1 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_2_EN               = 0x7A, /*!< Analog GPIO 2 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_3_EN               = 0x7B, /*!< Analog GPIO 3 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_4_EN               = 0x7C, /*!< Analog GPIO 4 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_5_EN               = 0x7D, /*!< Analog GPIO 5 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_6_EN               = 0x7E, /*!< Analog GPIO 6 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_7_EN               = 0x7F, /*!< Analog GPIO 7 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_8_EN               = 0x80, /*!< Analog GPIO 8 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_9_EN               = 0x81, /*!< Analog GPIO 9 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_10_EN              = 0x82, /*!< Analog GPIO 10 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_11_EN              = 0x83, /*!< Analog GPIO 11 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_12_EN              = 0x84, /*!< Analog GPIO 12 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_13_EN              = 0x85, /*!< Analog GPIO 13 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_14_EN              = 0x86, /*!< Analog GPIO 14 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_15_EN              = 0x87, /*!< Analog GPIO 15 enable */

    /* TX channel enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_0_CH_EN                     = 0x88, /*!< TX Channel 0 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_1_CH_EN                     = 0x89, /*!< TX Channel 1 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_2_CH_EN                     = 0x8A, /*!< TX Channel 2 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_3_CH_EN                     = 0x8B, /*!< TX Channel 3 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_4_CH_EN                     = 0x8C, /*!< TX Channel 4 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_5_CH_EN                     = 0x8D, /*!< TX Channel 5 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_6_CH_EN                     = 0x8E, /*!< TX Channel 6 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_7_CH_EN                     = 0x8F, /*!< TX Channel 7 enable */

    /* TX antenna enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_0_ANT_EN                    = 0x90, /*!< TX Channel 0 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_1_ANT_EN                    = 0x91, /*!< TX Channel 1 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_2_ANT_EN                    = 0x92, /*!< TX Channel 2 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_3_ANT_EN                    = 0x93, /*!< TX Channel 3 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_4_ANT_EN                    = 0x94, /*!< TX Channel 4 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_5_ANT_EN                    = 0x95, /*!< TX Channel 5 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_6_ANT_EN                    = 0x96, /*!< TX Channel 6 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_7_ANT_EN                    = 0x97, /*!< TX Channel 7 Antenna enable */

    /* RX channel enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_0_CH_EN                     = 0x98, /*!< RX Channel 0 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_1_CH_EN                     = 0x99, /*!< RX Channel 1 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_2_CH_EN                     = 0x9A, /*!< RX Channel 2 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_3_CH_EN                     = 0x9B, /*!< RX Channel 3 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_4_CH_EN                     = 0x9C, /*!< RX Channel 4 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_5_CH_EN                     = 0x9D, /*!< RX Channel 5 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_6_CH_EN                     = 0x9E, /*!< RX Channel 6 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_7_CH_EN                     = 0x9F, /*!< RX Channel 7 enable */

    /* RX antenna enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_0_ANT_EN                    = 0xA0, /*!< RX Channel 0 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_1_ANT_EN                    = 0xA1, /*!< RX Channel 1 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_2_ANT_EN                    = 0xA2, /*!< RX Channel 2 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_3_ANT_EN                    = 0xA3, /*!< RX Channel 3 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_4_ANT_EN                    = 0xA4, /*!< RX Channel 4 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_5_ANT_EN                    = 0xA5, /*!< RX Channel 5 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_6_ANT_EN                    = 0xA6, /*!< RX Channel 6 Antenna enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_7_ANT_EN                    = 0xA7, /*!< RX Channel 7 Antenna enable */

    /* ORX channel enables */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_0_CH_EN                    = 0xA8, /*!< ORX Channel 0 enable */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_1_CH_EN                    = 0xA9, /*!< ORX Channel 0 enable */

    /* TX special function triggers
     *
     * The primary function of these signals is to trigger slice streams.
     *
     * A secondary function can be enabled depending on "Radio_Sequencer_Mux_Registers" setting to:
     * 1. Select VSWR as TXON
     * 2. Update trigger for tx attenuation block
     * 3. Reset of measurement for DFE TSSI 2 Block
     * 4. Reset of measurement for DFE TSSI 1 Block
     *
     * See "Radio_Sequencer_Mux_Registers" for details
     */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_0_TRIG                 = 0xAA, /*!< TX special function #0 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_1_TRIG                 = 0xAB, /*!< TX special function #1 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_2_TRIG                 = 0xAC, /*!< TX special function #2 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_3_TRIG                 = 0xAD, /*!< TX special function #3 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_4_TRIG                 = 0xAE, /*!< TX special function #4 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_5_TRIG                 = 0xAF, /*!< TX special function #5 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_6_TRIG                 = 0xB0, /*!< TX special function #6 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_TX_FUNC_7_TRIG                 = 0xB1, /*!< TX special function #7 trigger */

    /* RX special function triggers
     *
     * The primary function of these signals is to trigger RX slice streams
     */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_0_TRIG                 = 0xB2, /*!< RX special function #0 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_1_TRIG                 = 0xB3, /*!< RX special function #1 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_2_TRIG                 = 0xB4, /*!< RX special function #2 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_3_TRIG                 = 0xB5, /*!< RX special function #3 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_4_TRIG                 = 0xB6, /*!< RX special function #4 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_5_TRIG                 = 0xB7, /*!< RX special function #5 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_6_TRIG                 = 0xB8, /*!< RX special function #6 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_RX_FUNC_7_TRIG                 = 0xB9, /*!< RX special function #7 trigger */

    /* ORX special function triggers
     *
     * The primary function of these signals is to trigger ORX slice streams
     *
     * A secondary function can be enabled depending on ORX "Radio_Sequencer_Mux_Registers" setting to:
     * 1. Reset of measurement for the DFE TSSI Block
     *
     * See ORX "Radio_Sequencer_Mux_Registers" for details
     */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_0_TRIG                = 0xBA, /*!< ORX special function #0 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_1_TRIG                = 0xBB, /*!< ORX special function #1 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_2_TRIG                = 0xBC, /*!< ORX special function #2 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_3_TRIG                = 0xBD, /*!< ORX special function #3 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_4_TRIG                = 0xBE, /*!< ORX special function #4 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_5_TRIG                = 0xBF, /*!< ORX special function #5 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_6_TRIG                = 0xC0, /*!< ORX special function #6 trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_FUNC_7_TRIG                = 0xC1, /*!< ORX special function #7 trigger */

    /* TX/ORX mapping update triggers */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_0_MAP_UPDATE_TRIG          = 0xC2, /*!< ORX Channel 0 Tx/Orx mapping update trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_1_MAP_UPDATE_TRIG          = 0xC3, /*!< ORX Channel 1 Tx/Orx mapping update trigger */

    /* GPIO update triggers */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_DIG_GPIO_UPDATE_TRIG           = 0xC4, /*!< Digital GPIO update trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ANALOG_GPIO_UPDATE_TRIG        = 0xC5, /*!< Analog GPIO update trigger */

    /* VSWR direction update triggers */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_0_VSWR_DIR_SEL_UPDATE_TRIG = 0xC6, /*!< ORX Channel 0 VSWR direction update trigger */
    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_ORX_1_VSWR_DIR_SEL_UPDATE_TRIG = 0xC7, /*!< ORX Channel 1 VSWR direction update trigger */

    ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SIGNAL_UNUSED_BEGIN                   = 0xC8  /*!< Beginning of unused/unmapped signals */
} adi_adrv904x_DfeSvcRadioCtrlSequencerSignal_e;

/**
 * \brief Structure for holding different errors reported by radio sequencer hardware.
 *
 */
typedef struct
{
    uint8_t  ssbSyncPhaseError;        /*!< Incoming SSB_SYNC from pin has shifted from original SSB_SYNC. 1 - Error Occured, 0 - No Error. */
    uint16_t busErrorMask;             /*!< AHB bus has returned an error response.Mask is bit mapped to 16 radio sequencers.Refer adi_adrv904x_DfeSvcRadioCtrlSequencersMask_e to generate the mask. 1 - Error Occured, 0 - No Error. */
    uint16_t fetchErrorMask;           /*!< Sequencer was still in FETCH state during a symbol boundary. This is a catastrophic error.Mask is bit mapped to 16 radio sequencers.Refer adi_adrv904x_DfeSvcRadioCtrlSequencersMask_e to generate the mask. 1 - Error Occured, 0 - No Error. */
} adi_adrv904x_DfeSvcRadioCtrlSequencerErrorInfo_t;

/**
 * \brief Radio sequencer pattern identifier
 */
typedef size_t adi_adrv904x_DfeSvcRadioCtrlSequencerPatternId_t;

#endif /* __ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_T_H__ */


