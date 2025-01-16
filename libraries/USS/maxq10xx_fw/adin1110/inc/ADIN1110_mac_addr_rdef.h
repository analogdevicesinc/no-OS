
/* ================================================================================
     Project     :   MAC
     File        :   MAC_addr_rdef.h
     Description :   C header file contains macros for Registers' address relative to instances and plain bit-fields.


     Copyright (c) 2021 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.
   ================================================================================ */

#ifndef ADIN1110_MAC_ADDR_RDEF_H
#define ADIN1110_MAC_ADDR_RDEF_H


#if defined(_LANGUAGE_C) || (defined(__GNUC__) && !defined(__ASSEMBLER__))
#include <stdint.h>
#endif /* _LANGUAGE_C */

/* ====================================================================================================
        MAC Module Instances Address and Mask Definitions
   ==================================================================================================== */




/* ====================================================================================================
        MAC Module Register Address Offset Definitions
   ==================================================================================================== */
#define ADDR_MAC_IDVER                                           (0X000U)    /* Identification Version Register. */
#define ADDR_MAC_PHYID                                           (0X001U)    /* PHY Identification Register. */
#define ADDR_MAC_CAPABILITY                                      (0X002U)    /* Supported Capabilities Register. */
#define ADDR_MAC_RESET                                           (0X003U)    /* Reset Control and Status Register. */
#define ADDR_MAC_CONFIG0                                         (0X004U)    /* Configuration Register 0. */
#define ADDR_MAC_CONFIG2                                         (0X006U)    /* Configuration Register 2. */
#define ADDR_MAC_STATUS0                                         (0X008U)    /* Status Register 0. */
#define ADDR_MAC_STATUS1                                         (0X009U)    /* Status Register 1. */
#define ADDR_MAC_BUFSTS                                          (0X00BU)    /* Buffer Status Register. */
#define ADDR_MAC_IMASK0                                          (0X00CU)    /* Interrupt Mask Register 0. */
#define ADDR_MAC_IMASK1                                          (0X00DU)    /* Mask Bits for Driving the Interrupt Pin Register. */
#define ADDR_MAC_TTSCAH                                          (0X010U)    /* Transmit Timestamp Capture Register A (High). */
#define ADDR_MAC_TTSCAL                                          (0X011U)    /* Transmit Timestamp Capture Register A (Low). */
#define ADDR_MAC_TTSCBH                                          (0X012U)    /* Transmit Timestamp Capture Register B (High). */
#define ADDR_MAC_TTSCBL                                          (0X013U)    /* Transmit Timestamp Capture Register B (Low). */
#define ADDR_MAC_TTSCCH                                          (0X014U)    /* Transmit Timestamp Capture Register C (High). */
#define ADDR_MAC_TTSCCL                                          (0X015U)    /* Transmit Timestamp Capture Register C (Low). */
#define ADDR_MAC_MDIOACC_0_                                      (0X020U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_1_                                      (0X021U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_2_                                      (0X022U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_3_                                      (0X023U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_4_                                      (0X024U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_5_                                      (0X025U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_6_                                      (0X026U)    /* MDIO Access Registers. */
#define ADDR_MAC_MDIOACC_7_                                      (0X027U)    /* MDIO Access Registers. */
#define ADDR_MAC_TX_FSIZE                                        (0X030U)    /* MAC Tx Frame Size Register. */
#define ADDR_MAC_TX                                              (0X031U)    /* MAC Transmit Register. */
#define ADDR_MAC_TX_SPACE                                        (0X032U)    /* Tx FIFO Space Register. */
#define ADDR_MAC_TX_THRESH                                       (0X034U)    /* Transmit Threshold Register. */
#define ADDR_MAC_FIFO_CLR                                        (0X036U)    /* MAC FIFO Clear Register. */
#define ADDR_MAC_SCRATCH_0_                                      (0X037U)    /* Scratch Registers. */
#define ADDR_MAC_SCRATCH_1_                                      (0X038U)    /* Scratch Registers. */
#define ADDR_MAC_SCRATCH_2_                                      (0X039U)    /* Scratch Registers. */
#define ADDR_MAC_SCRATCH_3_                                      (0X03AU)    /* Scratch Registers. */
#define ADDR_MAC_MAC_RST_STATUS                                  (0X03BU)    /* MAC Reset Status. */
#define ADDR_MAC_SOFT_RST                                        (0X03CU)    /* Software Reset Register. */
#define ADDR_MAC_SPI_INJ_ERR                                     (0X03DU)    /* Inject an Error on MISO from the DUT. */
#define ADDR_MAC_FIFO_SIZE                                       (0X03EU)    /* FIFO Sizes Register. */
#define ADDR_MAC_TFC                                             (0X03FU)    /* Tx FIFO Frame Count Register. */
#define ADDR_MAC_TXSIZE                                          (0X040U)    /* Tx FIFO Valid Half Words Register. */
#define ADDR_MAC_HTX_OVF_FRM_CNT                                 (0X041U)    /* Host Tx Frames Dropped Due to FIFO Overflow. */
#define ADDR_MAC_MECC_ERR_ADDR                                   (0X042U)    /* Address of a Detected ECC Error in Memory. */
#define ADDR_MAC_CECC_ERR_0_                                     (0X043U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_1_                                     (0X044U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_2_                                     (0X045U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_3_                                     (0X046U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_4_                                     (0X047U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_5_                                     (0X048U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_CECC_ERR_6_                                     (0X049U)    /* Corrected ECC Error Counters. */
#define ADDR_MAC_ADDR_FILT_UPR_0_                                (0X050U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_1_                                (0X052U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_2_                                (0X054U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_3_                                (0X056U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_4_                                (0X058U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_5_                                (0X05AU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_6_                                (0X05CU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_7_                                (0X05EU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_8_                                (0X060U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_9_                                (0X062U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_10_                               (0X064U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_11_                               (0X066U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_12_                               (0X068U)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_13_                               (0X06AU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_14_                               (0X06CU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_UPR_15_                               (0X06EU)    /* MAC Address Rule and DA Filter Upper 16 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_0_                                (0X051U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_1_                                (0X053U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_2_                                (0X055U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_3_                                (0X057U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_4_                                (0X059U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_5_                                (0X05BU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_6_                                (0X05DU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_7_                                (0X05FU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_8_                                (0X061U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_9_                                (0X063U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_10_                               (0X065U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_11_                               (0X067U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_12_                               (0X069U)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_13_                               (0X06BU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_14_                               (0X06DU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_FILT_LWR_15_                               (0X06FU)    /* MAC Address DA Filter Lower 32 Bits Registers. */
#define ADDR_MAC_ADDR_MSK_UPR_0_                                 (0X070U)    /* Upper 16 Bits of the MAC Address Mask. */
#define ADDR_MAC_ADDR_MSK_UPR_1_                                 (0X072U)    /* Upper 16 Bits of the MAC Address Mask. */
#define ADDR_MAC_ADDR_MSK_LWR_0_                                 (0X071U)    /* Lower 32 Bits of the MAC Address Mask. */
#define ADDR_MAC_ADDR_MSK_LWR_1_                                 (0X073U)    /* Lower 32 Bits of the MAC Address Mask. */
#define ADDR_MAC_TS_ADDEND                                       (0X080U)    /* Timestamp Accumulator Addend Register. */
#define ADDR_MAC_TS_1SEC_CMP                                     (0X081U)    /* Timer Update Compare Register. */
#define ADDR_MAC_TS_SEC_CNT                                      (0X082U)    /* Seconds Counter Register. */
#define ADDR_MAC_TS_NS_CNT                                       (0X083U)    /* Nanoseconds Counter Register. */
#define ADDR_MAC_TS_CFG                                          (0X084U)    /* Timer Configuration Register. */
#define ADDR_MAC_TS_TIMER_HI                                     (0X085U)    /* High Period for TS_TIMER Register. */
#define ADDR_MAC_TS_TIMER_LO                                     (0X086U)    /* Low Period for TS_TIMER Register. */
#define ADDR_MAC_TS_TIMER_QE_CORR                                (0X087U)    /* Quantization Error Correction Register. */
#define ADDR_MAC_TS_TIMER_START                                  (0X088U)    /* TS_TIMER Counter Start Time Register. */
#define ADDR_MAC_TS_EXT_CAPT0                                    (0X089U)    /* TS_CAPT Pin 0 Timestamp Register. */
#define ADDR_MAC_TS_EXT_CAPT1                                    (0X08AU)    /* TS_CAPT Pin 1 Timestamp Register. */
#define ADDR_MAC_TS_FREECNT_CAPT                                 (0X08BU)    /* TS_CAPT Free Running Counter Register. */
#define ADDR_MAC_P1_RX_FSIZE                                     (0X090U)    /* P1 MAC Rx Frame Size Register. */
#define ADDR_MAC_P1_RX                                           (0X091U)    /* P1 MAC Receive Register. */
#define ADDR_MAC_P1_RX_FRM_CNT                                   (0X0A0U)    /* P1 Rx Frame Count Register. */
#define ADDR_MAC_P1_RX_BCAST_CNT                                 (0X0A1U)    /* P1 Rx Broadcast Frame Count Register. */
#define ADDR_MAC_P1_RX_MCAST_CNT                                 (0X0A2U)    /* P1 Rx Multicast Frame Count Register. */
#define ADDR_MAC_P1_RX_UCAST_CNT                                 (0X0A3U)    /* P1 Rx Unicast Frame Count Register. */
#define ADDR_MAC_P1_RX_CRC_ERR_CNT                               (0X0A4U)    /* P1 Rx CRC Errored Frame Count Register. */
#define ADDR_MAC_P1_RX_ALGN_ERR_CNT                              (0X0A5U)    /* P1 Rx Align Error Count Register. */
#define ADDR_MAC_P1_RX_LS_ERR_CNT                                (0X0A6U)    /* P1 Rx Long/Short Frame Error Count Register. */
#define ADDR_MAC_P1_RX_PHY_ERR_CNT                               (0X0A7U)    /* P1 Rx PHY Error Count Register. */
#define ADDR_MAC_P1_TX_FRM_CNT                                   (0X0A8U)    /* P1 Tx Frame Count Register. */
#define ADDR_MAC_P1_TX_BCAST_CNT                                 (0X0A9U)    /* P1 Tx Broadcast Frame Count Register. */
#define ADDR_MAC_P1_TX_MCAST_CNT                                 (0X0AAU)    /* P1 Tx Multicast Frame Count Register. */
#define ADDR_MAC_P1_TX_UCAST_CNT                                 (0X0ABU)    /* P1 Tx Unicast Frame Count Register. */
#define ADDR_MAC_P1_RX_DROP_FULL_CNT                             (0X0ACU)    /* P1 Rx Frames Dropped Due to FIFO Full Register. */
#define ADDR_MAC_P1_RX_DROP_FILT_CNT                             (0X0ADU)    /* P1 Rx Frames Dropped Due to Filtering Register. */
#define ADDR_MAC_P1_RX_IFG_ERR_CNT                               (0X0AEU)    /* Frame Received on Port 1 with IFG Errors. */
#define ADDR_MAC_P1_TX_IFG                                       (0X0B0U)    /* P1 Transmit Inter Frame Gap Register. */
#define ADDR_MAC_P1_LOOP                                         (0X0B3U)    /* P1 MAC Loopback Enable Register. */
#define ADDR_MAC_P1_RX_CRC_EN                                    (0X0B4U)    /* P1 CRC Check Enable on Receive Register. */
#define ADDR_MAC_P1_RX_IFG                                       (0X0B5U)    /* P1 Receive Inter Frame Gap Register. */
#define ADDR_MAC_P1_RX_MAX_LEN                                   (0X0B6U)    /* P1 Max Receive Frame Length Register. */
#define ADDR_MAC_P1_RX_MIN_LEN                                   (0X0B7U)    /* P1 Min Receive Frame Length Register. */
#define ADDR_MAC_P1_LO_RFC                                       (0X0B8U)    /* P1 Rx Low Priority FIFO Frame Count Register. */
#define ADDR_MAC_P1_HI_RFC                                       (0X0B9U)    /* P1 Rx High Priority FIFO Frame Count Register. */
#define ADDR_MAC_P1_LO_RXSIZE                                    (0X0BAU)    /* P1 Low Priority Rx FIFO Valid Half Words Register. */
#define ADDR_MAC_P1_HI_RXSIZE                                    (0X0BBU)    /* P1 High Priority Rx FIFO Valid Half Words Register. */

/* ====================================================================================================
        MAC Module Register ResetValue Definitions
   ==================================================================================================== */
#define RSTVAL_MAC_IDVER                                           (0X10)
#define RSTVAL_MAC_PHYID                                           (0X283BC91)
#define RSTVAL_MAC_CAPABILITY                                      (0X6C3)
#define RSTVAL_MAC_RESET                                           (0X0)
#define RSTVAL_MAC_CONFIG0                                         (0X6)
#define RSTVAL_MAC_CONFIG2                                         (0X800)
#define RSTVAL_MAC_STATUS0                                         (0X40)
#define RSTVAL_MAC_STATUS1                                         (0X0)
#define RSTVAL_MAC_BUFSTS                                          (0X7700)
#define RSTVAL_MAC_IMASK0                                          (0X1FBF)
#define RSTVAL_MAC_IMASK1                                          (0X43FA1F1A)
#define RSTVAL_MAC_TTSCAH                                          (0X0)
#define RSTVAL_MAC_TTSCAL                                          (0X0)
#define RSTVAL_MAC_TTSCBH                                          (0X0)
#define RSTVAL_MAC_TTSCBL                                          (0X0)
#define RSTVAL_MAC_TTSCCH                                          (0X0)
#define RSTVAL_MAC_TTSCCL                                          (0X0)
#define RSTVAL_MAC_MDIOACC_0_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_1_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_2_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_3_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_4_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_5_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_6_                                      (0X8C000000)
#define RSTVAL_MAC_MDIOACC_7_                                      (0X8C000000)
#define RSTVAL_MAC_TX_FSIZE                                        (0X0)
#define RSTVAL_MAC_TX                                              (0X0)
#define RSTVAL_MAC_TX_SPACE                                        (0XFFF)
#define RSTVAL_MAC_TX_THRESH                                       (0X41)
#define RSTVAL_MAC_FIFO_CLR                                        (0X0)
#define RSTVAL_MAC_SCRATCH_0_                                      (0X0)
#define RSTVAL_MAC_SCRATCH_1_                                      (0X0)
#define RSTVAL_MAC_SCRATCH_2_                                      (0X0)
#define RSTVAL_MAC_SCRATCH_3_                                      (0X0)
#define RSTVAL_MAC_MAC_RST_STATUS                                  (0X3)
#define RSTVAL_MAC_SOFT_RST                                        (0X0)
#define RSTVAL_MAC_SPI_INJ_ERR                                     (0X0)
#define RSTVAL_MAC_FIFO_SIZE                                       (0X464)
#define RSTVAL_MAC_TFC                                             (0X0)
#define RSTVAL_MAC_TXSIZE                                          (0X0)
#define RSTVAL_MAC_HTX_OVF_FRM_CNT                                 (0X0)
#define RSTVAL_MAC_MECC_ERR_ADDR                                   (0X0)
#define RSTVAL_MAC_CECC_ERR_0_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_1_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_2_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_3_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_4_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_5_                                     (0X0)
#define RSTVAL_MAC_CECC_ERR_6_                                     (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_0_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_1_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_2_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_3_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_4_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_5_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_6_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_7_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_8_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_9_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_10_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_11_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_12_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_13_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_14_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_UPR_15_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_0_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_1_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_2_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_3_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_4_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_5_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_6_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_7_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_8_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_9_                                (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_10_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_11_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_12_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_13_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_14_                               (0X0)
#define RSTVAL_MAC_ADDR_FILT_LWR_15_                               (0X0)
#define RSTVAL_MAC_ADDR_MSK_UPR_0_                                 (0XFFFF)
#define RSTVAL_MAC_ADDR_MSK_UPR_1_                                 (0XFFFF)
#define RSTVAL_MAC_ADDR_MSK_LWR_0_                                 (0XFFFFFFFF)
#define RSTVAL_MAC_ADDR_MSK_LWR_1_                                 (0XFFFFFFFF)
#define RSTVAL_MAC_TS_ADDEND                                       (0X85555555)
#define RSTVAL_MAC_TS_1SEC_CMP                                     (0X3B9ACA00)
#define RSTVAL_MAC_TS_SEC_CNT                                      (0X0)
#define RSTVAL_MAC_TS_NS_CNT                                       (0X0)
#define RSTVAL_MAC_TS_CFG                                          (0X0)
#define RSTVAL_MAC_TS_TIMER_HI                                     (0X0)
#define RSTVAL_MAC_TS_TIMER_LO                                     (0X0)
#define RSTVAL_MAC_TS_TIMER_QE_CORR                                (0X0)
#define RSTVAL_MAC_TS_TIMER_START                                  (0X0)
#define RSTVAL_MAC_TS_EXT_CAPT0                                    (0X0)
#define RSTVAL_MAC_TS_EXT_CAPT1                                    (0X0)
#define RSTVAL_MAC_TS_FREECNT_CAPT                                 (0X0)
#define RSTVAL_MAC_P1_RX_FSIZE                                     (0X0)
#define RSTVAL_MAC_P1_RX                                           (0X0)
#define RSTVAL_MAC_P1_RX_FRM_CNT                                   (0X0)
#define RSTVAL_MAC_P1_RX_BCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_RX_MCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_RX_UCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_RX_CRC_ERR_CNT                               (0X0)
#define RSTVAL_MAC_P1_RX_ALGN_ERR_CNT                              (0X0)
#define RSTVAL_MAC_P1_RX_LS_ERR_CNT                                (0X0)
#define RSTVAL_MAC_P1_RX_PHY_ERR_CNT                               (0X0)
#define RSTVAL_MAC_P1_TX_FRM_CNT                                   (0X0)
#define RSTVAL_MAC_P1_TX_BCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_TX_MCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_TX_UCAST_CNT                                 (0X0)
#define RSTVAL_MAC_P1_RX_DROP_FULL_CNT                             (0X0)
#define RSTVAL_MAC_P1_RX_DROP_FILT_CNT                             (0X0)
#define RSTVAL_MAC_P1_RX_IFG_ERR_CNT                               (0X0)
#define RSTVAL_MAC_P1_TX_IFG                                       (0XB)
#define RSTVAL_MAC_P1_LOOP                                         (0X0)
#define RSTVAL_MAC_P1_RX_CRC_EN                                    (0X1)
#define RSTVAL_MAC_P1_RX_IFG                                       (0XA)
#define RSTVAL_MAC_P1_RX_MAX_LEN                                   (0X618)
#define RSTVAL_MAC_P1_RX_MIN_LEN                                   (0X40)
#define RSTVAL_MAC_P1_LO_RFC                                       (0X0)
#define RSTVAL_MAC_P1_HI_RFC                                       (0X0)
#define RSTVAL_MAC_P1_LO_RXSIZE                                    (0X0)
#define RSTVAL_MAC_P1_HI_RXSIZE                                    (0X0)

/* ====================================================================================================
        MAC Module Register BitPositions, Lengths, Masks and Enumerations Definitions
   ==================================================================================================== */

/* ----------------------------------------------------------------------------------------------------
          IDVER                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_IDVER_MINVER                                 (0U)           /* OA Minor Version. */
#define BITL_MAC_IDVER_MINVER                                 (4U)           /* OA Minor Version. */
#define BITM_MAC_IDVER_MINVER                                 (0X0000000FU)  /* OA Minor Version. */
#define BITP_MAC_IDVER_MAJVER                                 (4U)           /* OA Major Version. */
#define BITL_MAC_IDVER_MAJVER                                 (4U)           /* OA Major Version. */
#define BITM_MAC_IDVER_MAJVER                                 (0X000000F0U)  /* OA Major Version. */

/* ----------------------------------------------------------------------------------------------------
          PHYID                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_PHYID_REVISION                               (0U)           /* Manufacturer’s Revision Number. */
#define BITL_MAC_PHYID_REVISION                               (4U)           /* Manufacturer’s Revision Number. */
#define BITM_MAC_PHYID_REVISION                               (0X0000000FU)  /* Manufacturer’s Revision Number. */
#define BITP_MAC_PHYID_MODEL                                  (4U)           /* Manufacturer’s Model Number. */
#define BITL_MAC_PHYID_MODEL                                  (6U)           /* Manufacturer’s Model Number. */
#define BITM_MAC_PHYID_MODEL                                  (0X000003F0U)  /* Manufacturer’s Model Number. */
#define BITP_MAC_PHYID_OUI                                    (10U)          /* Organizationally Unique Identifier (Bits 2:23). */
#define BITL_MAC_PHYID_OUI                                    (22U)          /* Organizationally Unique Identifier (Bits 2:23). */
#define BITM_MAC_PHYID_OUI                                    (0XFFFFFC00U)  /* Organizationally Unique Identifier (Bits 2:23). */

/* ----------------------------------------------------------------------------------------------------
          CAPABILITY                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_CAPABILITY_MINCPS                            (0U)           /* Minimum Supported Chunk Payload Size. */
#define BITL_MAC_CAPABILITY_MINCPS                            (3U)           /* Minimum Supported Chunk Payload Size. */
#define BITM_MAC_CAPABILITY_MINCPS                            (0X00000007U)  /* Minimum Supported Chunk Payload Size. */
#define BITP_MAC_CAPABILITY_SEQC                              (4U)           /* TX Data Chunk Sequence and Retry Capability. */
#define BITL_MAC_CAPABILITY_SEQC                              (1U)           /* TX Data Chunk Sequence and Retry Capability. */
#define BITM_MAC_CAPABILITY_SEQC                              (0X00000010U)  /* TX Data Chunk Sequence and Retry Capability. */
#define BITP_MAC_CAPABILITY_AIDC                              (5U)           /* Address Increment Disable Capability. */
#define BITL_MAC_CAPABILITY_AIDC                              (1U)           /* Address Increment Disable Capability. */
#define BITM_MAC_CAPABILITY_AIDC                              (0X00000020U)  /* Address Increment Disable Capability. */
#define BITP_MAC_CAPABILITY_FTSC                              (6U)           /* Frame Timestamp Capability. */
#define BITL_MAC_CAPABILITY_FTSC                              (1U)           /* Frame Timestamp Capability. */
#define BITM_MAC_CAPABILITY_FTSC                              (0X00000040U)  /* Frame Timestamp Capability. */
#define BITP_MAC_CAPABILITY_CTC                               (7U)           /* Cut-Through Capability. */
#define BITL_MAC_CAPABILITY_CTC                               (1U)           /* Cut-Through Capability. */
#define BITM_MAC_CAPABILITY_CTC                               (0X00000080U)  /* Cut-Through Capability. */
#define BITP_MAC_CAPABILITY_DPRAC                             (8U)           /* Direct PHY Register Access Capability. */
#define BITL_MAC_CAPABILITY_DPRAC                             (1U)           /* Direct PHY Register Access Capability. */
#define BITM_MAC_CAPABILITY_DPRAC                             (0X00000100U)  /* Direct PHY Register Access Capability. */
#define BITP_MAC_CAPABILITY_IPRAC                             (9U)           /* Indirect PHY Register Access Capability. */
#define BITL_MAC_CAPABILITY_IPRAC                             (1U)           /* Indirect PHY Register Access Capability. */
#define BITM_MAC_CAPABILITY_IPRAC                             (0X00000200U)  /* Indirect PHY Register Access Capability. */
#define BITP_MAC_CAPABILITY_TXFCSVC                           (10U)          /* Transmit Frame Check Sequence Validation Capability. */
#define BITL_MAC_CAPABILITY_TXFCSVC                           (1U)           /* Transmit Frame Check Sequence Validation Capability. */
#define BITM_MAC_CAPABILITY_TXFCSVC                           (0X00000400U)  /* Transmit Frame Check Sequence Validation Capability. */

#define ENUM_MAC_CAPABILITY_TXFCSVC_TXFCSVC_0                 (0X00000000U)  /* Transmit FCS Validation is Not Supported. */
#define ENUM_MAC_CAPABILITY_TXFCSVC_TXFCSVC_1                 (0X00000001U)  /* Transmit FCS Validation is Supported. */
#define ENUM_MAC_CAPABILITY_IPRAC_IPRAC_0                     (0X00000000U)  /* PHY Registers are Not Indirectly Accessible. */
#define ENUM_MAC_CAPABILITY_IPRAC_IPRAC_1                     (0X00000001U)  /* PHY Registers are Indirectly Accessible. */
#define ENUM_MAC_CAPABILITY_DPRAC_DPRAC_0                     (0X00000000U)  /* PHY Registers are Not Directly Accessible. */
#define ENUM_MAC_CAPABILITY_DPRAC_DPRAC_1                     (0X00000001U)  /* PHY Registers are Directly Accessible. */
#define ENUM_MAC_CAPABILITY_CTC_CTC_0                         (0X00000000U)  /* Cut-Through Not Supported. */
#define ENUM_MAC_CAPABILITY_CTC_CTC_1                         (0X00000001U)  /* Cut-Through Supported. */
#define ENUM_MAC_CAPABILITY_FTSC_FTSC_1                       (0X00000001U)  /* IEEE 1588 Timestamp Capture on Frame Tx/Rx is Supported. */
#define ENUM_MAC_CAPABILITY_FTSC_FTSC_0                       (0X00000000U)  /* IEEE 1588 Timestamp Capture on Frame Tx/Rx is Not Supported. */
#define ENUM_MAC_CAPABILITY_SEQC_SEQC_1                       (0X00000001U)  /* TX Data Chunk Sequence and Retry is Supported. */
#define ENUM_MAC_CAPABILITY_SEQC_SEQC_0                       (0X00000000U)  /* TX Data Chunk Sequence and Retry is Not Supported. */
#define ENUM_MAC_CAPABILITY_MINCPS_CPS64                      (0X00000006U)  /* Minimum Supported Chunk Payload Size is 64 Bytes. */
#define ENUM_MAC_CAPABILITY_MINCPS_CPS32                      (0X00000005U)  /* Minimum Supported Chunk Payload Size is 32 Bytes. */
#define ENUM_MAC_CAPABILITY_MINCPS_CPS16                      (0X00000004U)  /* Minimum Supported Chunk Payload Size is 16 Bytes. */
#define ENUM_MAC_CAPABILITY_MINCPS_CPS8                       (0X00000003U)  /* Minimum Supported Chunk Payload Size is 8 Bytes. */

/* ----------------------------------------------------------------------------------------------------
          RESET                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_RESET_SWRESET                                (0U)           /* Software Reset. */
#define BITL_MAC_RESET_SWRESET                                (1U)           /* Software Reset. */
#define BITM_MAC_RESET_SWRESET                                (0X00000001U)  /* Software Reset. */

/* ----------------------------------------------------------------------------------------------------
          CONFIG0                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_CONFIG0_CPS                                  (0U)           /* Chunk Payload Selector (N). */
#define BITL_MAC_CONFIG0_CPS                                  (3U)           /* Chunk Payload Selector (N). */
#define BITM_MAC_CONFIG0_CPS                                  (0X00000007U)  /* Chunk Payload Selector (N). */
#define BITP_MAC_CONFIG0_SEQE                                 (4U)           /* Enable TX Data Chunk Sequence and Retry. */
#define BITL_MAC_CONFIG0_SEQE                                 (1U)           /* Enable TX Data Chunk Sequence and Retry. */
#define BITM_MAC_CONFIG0_SEQE                                 (0X00000010U)  /* Enable TX Data Chunk Sequence and Retry. */
#define BITP_MAC_CONFIG0_PROTE                                (5U)           /* Enable Control Data Read Write Protection. */
#define BITL_MAC_CONFIG0_PROTE                                (1U)           /* Enable Control Data Read Write Protection. */
#define BITM_MAC_CONFIG0_PROTE                                (0X00000020U)  /* Enable Control Data Read Write Protection. */
#define BITP_MAC_CONFIG0_FTSS                                 (6U)           /* Receive Frame Timestamp Select. */
#define BITL_MAC_CONFIG0_FTSS                                 (1U)           /* Receive Frame Timestamp Select. */
#define BITM_MAC_CONFIG0_FTSS                                 (0X00000040U)  /* Receive Frame Timestamp Select. */
#define BITP_MAC_CONFIG0_FTSE                                 (7U)           /* Frame Timestamp Enable. */
#define BITL_MAC_CONFIG0_FTSE                                 (1U)           /* Frame Timestamp Enable. */
#define BITM_MAC_CONFIG0_FTSE                                 (0X00000080U)  /* Frame Timestamp Enable. */
#define BITP_MAC_CONFIG0_RXCTE                                (8U)           /* Receive Cut-Through Enable. */
#define BITL_MAC_CONFIG0_RXCTE                                (1U)           /* Receive Cut-Through Enable. */
#define BITM_MAC_CONFIG0_RXCTE                                (0X00000100U)  /* Receive Cut-Through Enable. */
#define BITP_MAC_CONFIG0_TXCTE                                (9U)           /* Transmit Cut-Through Enable. */
#define BITL_MAC_CONFIG0_TXCTE                                (1U)           /* Transmit Cut-Through Enable. */
#define BITM_MAC_CONFIG0_TXCTE                                (0X00000200U)  /* Transmit Cut-Through Enable. */
#define BITP_MAC_CONFIG0_TXCTHRESH                            (10U)          /* Transmit Credit Threshold. */
#define BITL_MAC_CONFIG0_TXCTHRESH                            (2U)           /* Transmit Credit Threshold. */
#define BITM_MAC_CONFIG0_TXCTHRESH                            (0X00000C00U)  /* Transmit Credit Threshold. */
#define BITP_MAC_CONFIG0_ZARFE                                (12U)          /* Zero-Align Receive Frame Enable. */
#define BITL_MAC_CONFIG0_ZARFE                                (1U)           /* Zero-Align Receive Frame Enable. */
#define BITM_MAC_CONFIG0_ZARFE                                (0X00001000U)  /* Zero-Align Receive Frame Enable. */
#define BITP_MAC_CONFIG0_CSARFE                               (13U)          /* CSn Align Receive Frame Enable. */
#define BITL_MAC_CONFIG0_CSARFE                               (1U)           /* CSn Align Receive Frame Enable. */
#define BITM_MAC_CONFIG0_CSARFE                               (0X00002000U)  /* CSn Align Receive Frame Enable. */
#define BITP_MAC_CONFIG0_TXFCSVE                              (14U)          /* Transmit Frame Check Sequence Validation Enable. */
#define BITL_MAC_CONFIG0_TXFCSVE                              (1U)           /* Transmit Frame Check Sequence Validation Enable. */
#define BITM_MAC_CONFIG0_TXFCSVE                              (0X00004000U)  /* Transmit Frame Check Sequence Validation Enable. */
#define BITP_MAC_CONFIG0_SYNC                                 (15U)          /* Configuration Synchronization. */
#define BITL_MAC_CONFIG0_SYNC                                 (1U)           /* Configuration Synchronization. */
#define BITM_MAC_CONFIG0_SYNC                                 (0X00008000U)  /* Configuration Synchronization. */

#define ENUM_MAC_CONFIG0_SYNC_SYNC_0                          (0X00000000U)  /* The MACPHY Has Been Reset and is Not Configured. */
#define ENUM_MAC_CONFIG0_SYNC_SYNC_1                          (0X00000001U)  /* The MACPHY is Configured. */
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_1                   (0X00000000U)  /* ≥ 1 Credit. */
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_4                   (0X00000001U)  /* ≥ 4 Credits. */
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_8                   (0X00000002U)  /* ≥ 8 Credits. */
#define ENUM_MAC_CONFIG0_TXCTHRESH_CREDIT_16                  (0X00000003U)  /* ≥ 16 Credits. */
#define ENUM_MAC_CONFIG0_FTSE_FTSE_0                          (0X00000000U)  /* Frame Receive/Transmit Timestamps are Disabled. */
#define ENUM_MAC_CONFIG0_FTSE_FTSE_1                          (0X00000001U)  /* Frame Receive/Transmit Timestamps are Enabled. */
#define ENUM_MAC_CONFIG0_FTSS_FTSS_0                          (0X00000000U)  /* 32-bit Timestamps. */
#define ENUM_MAC_CONFIG0_FTSS_FTSS_1                          (0X00000001U)  /* 64-bit Timestamps. */
#define ENUM_MAC_CONFIG0_CPS_CHUNK_8BYTE                      (0X00000003U)  /* No description provided */
#define ENUM_MAC_CONFIG0_CPS_CHUNK_16BYTE                     (0X00000004U)  /* No description provided */
#define ENUM_MAC_CONFIG0_CPS_CHUNK_32BYTE                     (0X00000005U)  /* No description provided */
#define ENUM_MAC_CONFIG0_CPS_CHUNK_64BYTE                     (0X00000006U)  /* No description provided */

/* ----------------------------------------------------------------------------------------------------
          CONFIG2                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_CONFIG2_MSPEED                               (0U)           /* SPI to MDIO Bridge MDC Clock Speed. */
#define BITL_MAC_CONFIG2_MSPEED                               (2U)           /* SPI to MDIO Bridge MDC Clock Speed. */
#define BITM_MAC_CONFIG2_MSPEED                               (0X00000003U)  /* SPI to MDIO Bridge MDC Clock Speed. */
#define BITP_MAC_CONFIG2_P1_FWD_UNK2HOST                      (2U)           /* Forward Frames Not Matching Any MAC Address to the Host. */
#define BITL_MAC_CONFIG2_P1_FWD_UNK2HOST                      (1U)           /* Forward Frames Not Matching Any MAC Address to the Host. */
#define BITM_MAC_CONFIG2_P1_FWD_UNK2HOST                      (0X00000004U)  /* Forward Frames Not Matching Any MAC Address to the Host. */
#define BITP_MAC_CONFIG2_P1_RCV_IFG_ERR_FRM                   (4U)           /* Admit Frames with IFG Errors on P1. */
#define BITL_MAC_CONFIG2_P1_RCV_IFG_ERR_FRM                   (1U)           /* Admit Frames with IFG Errors on P1. */
#define BITM_MAC_CONFIG2_P1_RCV_IFG_ERR_FRM                   (0X00000010U)  /* Admit Frames with IFG Errors on P1. */
#define BITP_MAC_CONFIG2_CRC_APPEND                           (5U)           /* Enable CRC Append. */
#define BITL_MAC_CONFIG2_CRC_APPEND                           (1U)           /* Enable CRC Append. */
#define BITM_MAC_CONFIG2_CRC_APPEND                           (0X00000020U)  /* Enable CRC Append. */
#define BITP_MAC_CONFIG2_STATS_CLR_ON_RD                      (6U)           /* Statistics Clear on Reading. */
#define BITL_MAC_CONFIG2_STATS_CLR_ON_RD                      (1U)           /* Statistics Clear on Reading. */
#define BITM_MAC_CONFIG2_STATS_CLR_ON_RD                      (0X00000040U)  /* Statistics Clear on Reading. */
#define BITP_MAC_CONFIG2_SFD_DETECT_SRC                       (7U)           /* Determines If the SFD is Detected in the PHY or MAC. */
#define BITL_MAC_CONFIG2_SFD_DETECT_SRC                       (1U)           /* Determines If the SFD is Detected in the PHY or MAC. */
#define BITM_MAC_CONFIG2_SFD_DETECT_SRC                       (0X00000080U)  /* Determines If the SFD is Detected in the PHY or MAC. */
#define BITP_MAC_CONFIG2_TX_RDY_ON_EMPTY                      (8U)           /* Assert TX_RDY When the Tx FIFO is Empty. */
#define BITL_MAC_CONFIG2_TX_RDY_ON_EMPTY                      (1U)           /* Assert TX_RDY When the Tx FIFO is Empty. */
#define BITM_MAC_CONFIG2_TX_RDY_ON_EMPTY                      (0X00000100U)  /* Assert TX_RDY When the Tx FIFO is Empty. */

#define ENUM_MAC_CONFIG2_SFD_DETECT_SRC_PHY                   (0X00000000U)  /* Select the SFD from the PHY. */
#define ENUM_MAC_CONFIG2_SFD_DETECT_SRC_MAC                   (0X00000001U)  /* Select the SFD from the MAC. */
#define ENUM_MAC_CONFIG2_STATS_CLR_ON_RD_HOLD_STAT            (0X00000000U)  /* Statistic Counter is Not Cleared on Reading. */
#define ENUM_MAC_CONFIG2_STATS_CLR_ON_RD_CLR_STAT             (0X00000001U)  /* Clear Statistics Counters on Reading. */
#define ENUM_MAC_CONFIG2_MSPEED_MHZ_2P5                       (0X00000000U)  /* 2.5 MHz. */
#define ENUM_MAC_CONFIG2_MSPEED_MHZ_4P166                     (0X00000001U)  /* 4.166 MHz. */

/* ----------------------------------------------------------------------------------------------------
          STATUS0                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_STATUS0_TXPE                                 (0U)           /* Transmit Protocol Error. */
#define BITL_MAC_STATUS0_TXPE                                 (1U)           /* Transmit Protocol Error. */
#define BITM_MAC_STATUS0_TXPE                                 (0X00000001U)  /* Transmit Protocol Error. */
#define BITP_MAC_STATUS0_TXBOE                                (1U)           /* Host Tx FIFO Overflow. */
#define BITL_MAC_STATUS0_TXBOE                                (1U)           /* Host Tx FIFO Overflow. */
#define BITM_MAC_STATUS0_TXBOE                                (0X00000002U)  /* Host Tx FIFO Overflow. */
#define BITP_MAC_STATUS0_TXBUE                                (2U)           /* Host Tx FIFO Underrun Error. */
#define BITL_MAC_STATUS0_TXBUE                                (1U)           /* Host Tx FIFO Underrun Error. */
#define BITM_MAC_STATUS0_TXBUE                                (0X00000004U)  /* Host Tx FIFO Underrun Error. */
#define BITP_MAC_STATUS0_RXBOE                                (3U)           /* Receive Buffer Overflow Error. */
#define BITL_MAC_STATUS0_RXBOE                                (1U)           /* Receive Buffer Overflow Error. */
#define BITM_MAC_STATUS0_RXBOE                                (0X00000008U)  /* Receive Buffer Overflow Error. */
#define BITP_MAC_STATUS0_LOFE                                 (4U)           /* Loss of Frame Error. */
#define BITL_MAC_STATUS0_LOFE                                 (1U)           /* Loss of Frame Error. */
#define BITM_MAC_STATUS0_LOFE                                 (0X00000010U)  /* Loss of Frame Error. */
#define BITP_MAC_STATUS0_HDRE                                 (5U)           /* Header Error. */
#define BITL_MAC_STATUS0_HDRE                                 (1U)           /* Header Error. */
#define BITM_MAC_STATUS0_HDRE                                 (0X00000020U)  /* Header Error. */
#define BITP_MAC_STATUS0_RESETC                               (6U)           /* Reset Complete. */
#define BITL_MAC_STATUS0_RESETC                               (1U)           /* Reset Complete. */
#define BITM_MAC_STATUS0_RESETC                               (0X00000040U)  /* Reset Complete. */
#define BITP_MAC_STATUS0_PHYINT                               (7U)           /* PHY Interrupt for Port1. */
#define BITL_MAC_STATUS0_PHYINT                               (1U)           /* PHY Interrupt for Port1. */
#define BITM_MAC_STATUS0_PHYINT                               (0X00000080U)  /* PHY Interrupt for Port1. */
#define BITP_MAC_STATUS0_TTSCAA                               (8U)           /* Transmit Timestamp Capture Available A. */
#define BITL_MAC_STATUS0_TTSCAA                               (1U)           /* Transmit Timestamp Capture Available A. */
#define BITM_MAC_STATUS0_TTSCAA                               (0X00000100U)  /* Transmit Timestamp Capture Available A. */
#define BITP_MAC_STATUS0_TTSCAB                               (9U)           /* Transmit Timestamp Capture Available B. */
#define BITL_MAC_STATUS0_TTSCAB                               (1U)           /* Transmit Timestamp Capture Available B. */
#define BITM_MAC_STATUS0_TTSCAB                               (0X00000200U)  /* Transmit Timestamp Capture Available B. */
#define BITP_MAC_STATUS0_TTSCAC                               (10U)          /* Transmit Timestamp Capture Available C. */
#define BITL_MAC_STATUS0_TTSCAC                               (1U)           /* Transmit Timestamp Capture Available C. */
#define BITM_MAC_STATUS0_TTSCAC                               (0X00000400U)  /* Transmit Timestamp Capture Available C. */
#define BITP_MAC_STATUS0_TXFCSE                               (11U)          /* Transmit Frame Check Sequence Error. */
#define BITL_MAC_STATUS0_TXFCSE                               (1U)           /* Transmit Frame Check Sequence Error. */
#define BITM_MAC_STATUS0_TXFCSE                               (0X00000800U)  /* Transmit Frame Check Sequence Error. */
#define BITP_MAC_STATUS0_CDPE                                 (12U)          /* Control Data Protection Error. */
#define BITL_MAC_STATUS0_CDPE                                 (1U)           /* Control Data Protection Error. */
#define BITM_MAC_STATUS0_CDPE                                 (0X00001000U)  /* Control Data Protection Error. */

/* ----------------------------------------------------------------------------------------------------
          STATUS1                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_STATUS1_P1_LINK_STATUS                       (0U)           /* Port 1 Link Status. */
#define BITL_MAC_STATUS1_P1_LINK_STATUS                       (1U)           /* Port 1 Link Status. */
#define BITM_MAC_STATUS1_P1_LINK_STATUS                       (0X00000001U)  /* Port 1 Link Status. */
#define BITP_MAC_STATUS1_LINK_CHANGE                          (1U)           /* Link Status Changed. */
#define BITL_MAC_STATUS1_LINK_CHANGE                          (1U)           /* Link Status Changed. */
#define BITM_MAC_STATUS1_LINK_CHANGE                          (0X00000002U)  /* Link Status Changed. */
#define BITP_MAC_STATUS1_TX_RDY                               (3U)           /* Tx Ready. */
#define BITL_MAC_STATUS1_TX_RDY                               (1U)           /* Tx Ready. */
#define BITM_MAC_STATUS1_TX_RDY                               (0X00000008U)  /* Tx Ready. */
#define BITP_MAC_STATUS1_P1_RX_RDY                            (4U)           /* Port 1 Rx FIFO Contains Data. */
#define BITL_MAC_STATUS1_P1_RX_RDY                            (1U)           /* Port 1 Rx FIFO Contains Data. */
#define BITM_MAC_STATUS1_P1_RX_RDY                            (0X00000010U)  /* Port 1 Rx FIFO Contains Data. */
#define BITP_MAC_STATUS1_P1_RX_RDY_HI                         (5U)           /* Port1 Rx Ready High Priority. */
#define BITL_MAC_STATUS1_P1_RX_RDY_HI                         (1U)           /* Port1 Rx Ready High Priority. */
#define BITM_MAC_STATUS1_P1_RX_RDY_HI                         (0X00000020U)  /* Port1 Rx Ready High Priority. */
#define BITP_MAC_STATUS1_P1_RX_IFG_ERR                        (8U)           /* Rx MAC Inter Frame Gap Error. */
#define BITL_MAC_STATUS1_P1_RX_IFG_ERR                        (1U)           /* Rx MAC Inter Frame Gap Error. */
#define BITM_MAC_STATUS1_P1_RX_IFG_ERR                        (0X00000100U)  /* Rx MAC Inter Frame Gap Error. */
#define BITP_MAC_STATUS1_SPI_ERR                              (10U)          /* Detected an Error on an SPI Transaction. */
#define BITL_MAC_STATUS1_SPI_ERR                              (1U)           /* Detected an Error on an SPI Transaction. */
#define BITM_MAC_STATUS1_SPI_ERR                              (0X00000400U)  /* Detected an Error on an SPI Transaction. */
#define BITP_MAC_STATUS1_RX_ECC_ERR                           (11U)          /* ECC Error on Reading the Frame Size from an Rx FIFO. */
#define BITL_MAC_STATUS1_RX_ECC_ERR                           (1U)           /* ECC Error on Reading the Frame Size from an Rx FIFO. */
#define BITM_MAC_STATUS1_RX_ECC_ERR                           (0X00000800U)  /* ECC Error on Reading the Frame Size from an Rx FIFO. */
#define BITP_MAC_STATUS1_TX_ECC_ERR                           (12U)          /* ECC Error on Reading the Frame Size from a Tx FIFO. */
#define BITL_MAC_STATUS1_TX_ECC_ERR                           (1U)           /* ECC Error on Reading the Frame Size from a Tx FIFO. */
#define BITM_MAC_STATUS1_TX_ECC_ERR                           (0X00001000U)  /* ECC Error on Reading the Frame Size from a Tx FIFO. */

#define ENUM_MAC_STATUS1_P1_LINK_STATUS_DOWN                  (0X00000000U)  /* Link Down. */
#define ENUM_MAC_STATUS1_P1_LINK_STATUS_UP                    (0X00000001U)  /* Link up. */

/* ----------------------------------------------------------------------------------------------------
          BUFSTS                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_BUFSTS_RCA                                   (0U)           /* Receive Chunks Available. */
#define BITL_MAC_BUFSTS_RCA                                   (8U)           /* Receive Chunks Available. */
#define BITM_MAC_BUFSTS_RCA                                   (0X000000FFU)  /* Receive Chunks Available. */
#define BITP_MAC_BUFSTS_TXC                                   (8U)           /* Transmit Credits Available. */
#define BITL_MAC_BUFSTS_TXC                                   (8U)           /* Transmit Credits Available. */
#define BITM_MAC_BUFSTS_TXC                                   (0X0000FF00U)  /* Transmit Credits Available. */

/* ----------------------------------------------------------------------------------------------------
          IMASK0                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_IMASK0_TXPEM                                 (0U)           /* Transmit Protocol Error Mask. */
#define BITL_MAC_IMASK0_TXPEM                                 (1U)           /* Transmit Protocol Error Mask. */
#define BITM_MAC_IMASK0_TXPEM                                 (0X00000001U)  /* Transmit Protocol Error Mask. */
#define BITP_MAC_IMASK0_TXBOEM                                (1U)           /* Transmit Buffer Overflow Error Mask. */
#define BITL_MAC_IMASK0_TXBOEM                                (1U)           /* Transmit Buffer Overflow Error Mask. */
#define BITM_MAC_IMASK0_TXBOEM                                (0X00000002U)  /* Transmit Buffer Overflow Error Mask. */
#define BITP_MAC_IMASK0_TXBUEM                                (2U)           /* Transmit Buffer Underflow Error Mask. */
#define BITL_MAC_IMASK0_TXBUEM                                (1U)           /* Transmit Buffer Underflow Error Mask. */
#define BITM_MAC_IMASK0_TXBUEM                                (0X00000004U)  /* Transmit Buffer Underflow Error Mask. */
#define BITP_MAC_IMASK0_RXBOEM                                (3U)           /* Receive Buffer Overflow Error Mask. */
#define BITL_MAC_IMASK0_RXBOEM                                (1U)           /* Receive Buffer Overflow Error Mask. */
#define BITM_MAC_IMASK0_RXBOEM                                (0X00000008U)  /* Receive Buffer Overflow Error Mask. */
#define BITP_MAC_IMASK0_LOFEM                                 (4U)           /* Loss of Frame Error Mask. */
#define BITL_MAC_IMASK0_LOFEM                                 (1U)           /* Loss of Frame Error Mask. */
#define BITM_MAC_IMASK0_LOFEM                                 (0X00000010U)  /* Loss of Frame Error Mask. */
#define BITP_MAC_IMASK0_HDREM                                 (5U)           /* Header Error Mask. */
#define BITL_MAC_IMASK0_HDREM                                 (1U)           /* Header Error Mask. */
#define BITM_MAC_IMASK0_HDREM                                 (0X00000020U)  /* Header Error Mask. */
#define BITP_MAC_IMASK0_RESETCM                               (6U)           /* RESET Complete Mask. */
#define BITL_MAC_IMASK0_RESETCM                               (1U)           /* RESET Complete Mask. */
#define BITM_MAC_IMASK0_RESETCM                               (0X00000040U)  /* RESET Complete Mask. */
#define BITP_MAC_IMASK0_PHYINTM                               (7U)           /* Physical Layer Interrupt Mask. */
#define BITL_MAC_IMASK0_PHYINTM                               (1U)           /* Physical Layer Interrupt Mask. */
#define BITM_MAC_IMASK0_PHYINTM                               (0X00000080U)  /* Physical Layer Interrupt Mask. */
#define BITP_MAC_IMASK0_TTSCAAM                               (8U)           /* Transmit Timestamp Capture Available A Mask. */
#define BITL_MAC_IMASK0_TTSCAAM                               (1U)           /* Transmit Timestamp Capture Available A Mask. */
#define BITM_MAC_IMASK0_TTSCAAM                               (0X00000100U)  /* Transmit Timestamp Capture Available A Mask. */
#define BITP_MAC_IMASK0_TTSCABM                               (9U)           /* Transmit Timestamp Capture Available B Mask. */
#define BITL_MAC_IMASK0_TTSCABM                               (1U)           /* Transmit Timestamp Capture Available B Mask. */
#define BITM_MAC_IMASK0_TTSCABM                               (0X00000200U)  /* Transmit Timestamp Capture Available B Mask. */
#define BITP_MAC_IMASK0_TTSCACM                               (10U)          /* Transmit Timestamp Capture Available C Mask. */
#define BITL_MAC_IMASK0_TTSCACM                               (1U)           /* Transmit Timestamp Capture Available C Mask. */
#define BITM_MAC_IMASK0_TTSCACM                               (0X00000400U)  /* Transmit Timestamp Capture Available C Mask. */
#define BITP_MAC_IMASK0_TXFCSEM                               (11U)          /* Transmit Frame Check Sequence Error Mask. */
#define BITL_MAC_IMASK0_TXFCSEM                               (1U)           /* Transmit Frame Check Sequence Error Mask. */
#define BITM_MAC_IMASK0_TXFCSEM                               (0X00000800U)  /* Transmit Frame Check Sequence Error Mask. */
#define BITP_MAC_IMASK0_CDPEM                                 (12U)          /* Control Data Protection Error Mask. */
#define BITL_MAC_IMASK0_CDPEM                                 (1U)           /* Control Data Protection Error Mask. */
#define BITM_MAC_IMASK0_CDPEM                                 (0X00001000U)  /* Control Data Protection Error Mask. */

/* ----------------------------------------------------------------------------------------------------
          IMASK1                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_IMASK1_LINK_CHANGE_MASK                      (1U)           /* Mask Bit for LINK_CHANGE. */
#define BITL_MAC_IMASK1_LINK_CHANGE_MASK                      (1U)           /* Mask Bit for LINK_CHANGE. */
#define BITM_MAC_IMASK1_LINK_CHANGE_MASK                      (0X00000002U)  /* Mask Bit for LINK_CHANGE. */
#define BITP_MAC_IMASK1_TX_RDY_MASK                           (3U)           /* Mask Bit for TX_FRM_DONE. */
#define BITL_MAC_IMASK1_TX_RDY_MASK                           (1U)           /* Mask Bit for TX_FRM_DONE. */
#define BITM_MAC_IMASK1_TX_RDY_MASK                           (0X00000008U)  /* Mask Bit for TX_FRM_DONE. */
#define BITP_MAC_IMASK1_P1_RX_RDY_MASK                        (4U)           /* Mask Bit for P1_RX_RDY. */
#define BITL_MAC_IMASK1_P1_RX_RDY_MASK                        (1U)           /* Mask Bit for P1_RX_RDY. */
#define BITM_MAC_IMASK1_P1_RX_RDY_MASK                        (0X00000010U)  /* Mask Bit for P1_RX_RDY. */
#define BITP_MAC_IMASK1_P1_RX_IFG_ERR_MASK                    (8U)           /* Mask Bit for RX_IFG_ERR. */
#define BITL_MAC_IMASK1_P1_RX_IFG_ERR_MASK                    (1U)           /* Mask Bit for RX_IFG_ERR. */
#define BITM_MAC_IMASK1_P1_RX_IFG_ERR_MASK                    (0X00000100U)  /* Mask Bit for RX_IFG_ERR. */
#define BITP_MAC_IMASK1_SPI_ERR_MASK                          (10U)          /* Mask Bit for SPI_ERR. */
#define BITL_MAC_IMASK1_SPI_ERR_MASK                          (1U)           /* Mask Bit for SPI_ERR. */
#define BITM_MAC_IMASK1_SPI_ERR_MASK                          (0X00000400U)  /* Mask Bit for SPI_ERR. */
#define BITP_MAC_IMASK1_RX_ECC_ERR_MASK                       (11U)          /* Mask Bit for RXF_ECC_ERR. */
#define BITL_MAC_IMASK1_RX_ECC_ERR_MASK                       (1U)           /* Mask Bit for RXF_ECC_ERR. */
#define BITM_MAC_IMASK1_RX_ECC_ERR_MASK                       (0X00000800U)  /* Mask Bit for RXF_ECC_ERR. */
#define BITP_MAC_IMASK1_TX_ECC_ERR_MASK                       (12U)          /* Mask Bit for TXF_ECC_ERR. */
#define BITL_MAC_IMASK1_TX_ECC_ERR_MASK                       (1U)           /* Mask Bit for TXF_ECC_ERR. */
#define BITM_MAC_IMASK1_TX_ECC_ERR_MASK                       (0X00001000U)  /* Mask Bit for TXF_ECC_ERR. */

/* ----------------------------------------------------------------------------------------------------
          TTSCAH                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCAH_TTSCH_A                               (0U)           /* Transmit Timestamp A Bits 63-32. */
#define BITL_MAC_TTSCAH_TTSCH_A                               (32U)          /* Transmit Timestamp A Bits 63-32. */
#define BITM_MAC_TTSCAH_TTSCH_A                               (0XFFFFFFFFU)  /* Transmit Timestamp A Bits 63-32. */

/* ----------------------------------------------------------------------------------------------------
          TTSCAL                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCAL_TTSCL_A                               (0U)           /* Transmit Timestamp A Bits 31-0. */
#define BITL_MAC_TTSCAL_TTSCL_A                               (32U)          /* Transmit Timestamp A Bits 31-0. */
#define BITM_MAC_TTSCAL_TTSCL_A                               (0XFFFFFFFFU)  /* Transmit Timestamp A Bits 31-0. */

/* ----------------------------------------------------------------------------------------------------
          TTSCBH                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCBH_TTSCH_B                               (0U)           /* Transmit Timestamp B Bits 63-32. */
#define BITL_MAC_TTSCBH_TTSCH_B                               (32U)          /* Transmit Timestamp B Bits 63-32. */
#define BITM_MAC_TTSCBH_TTSCH_B                               (0XFFFFFFFFU)  /* Transmit Timestamp B Bits 63-32. */

/* ----------------------------------------------------------------------------------------------------
          TTSCBL                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCBL_TTSCL_B                               (0U)           /* Transmit Timestamp B Bits 31-0. */
#define BITL_MAC_TTSCBL_TTSCL_B                               (32U)          /* Transmit Timestamp B Bits 31-0. */
#define BITM_MAC_TTSCBL_TTSCL_B                               (0XFFFFFFFFU)  /* Transmit Timestamp B Bits 31-0. */

/* ----------------------------------------------------------------------------------------------------
          TTSCCH                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCCH_TTSCH_C                               (0U)           /* Transmit Timestamp C Bits 63-32. */
#define BITL_MAC_TTSCCH_TTSCH_C                               (32U)          /* Transmit Timestamp C Bits 63-32. */
#define BITM_MAC_TTSCCH_TTSCH_C                               (0XFFFFFFFFU)  /* Transmit Timestamp C Bits 63-32. */

/* ----------------------------------------------------------------------------------------------------
          TTSCCL                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TTSCCL_TTSCL_C                               (0U)           /* Transmit Timestamp C Bits 31-0. */
#define BITL_MAC_TTSCCL_TTSCL_C                               (32U)          /* Transmit Timestamp C Bits 31-0. */
#define BITM_MAC_TTSCCL_TTSCL_C                               (0XFFFFFFFFU)  /* Transmit Timestamp C Bits 31-0. */

/* ----------------------------------------------------------------------------------------------------
          MDIOACC_0_                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_MDIOACC_N__MDIO_DATA                         (0U)           /* Data/Address Value. */
#define BITL_MAC_MDIOACC_N__MDIO_DATA                         (16U)          /* Data/Address Value. */
#define BITM_MAC_MDIOACC_N__MDIO_DATA                         (0X0000FFFFU)  /* Data/Address Value. */
#define BITP_MAC_MDIOACC_N__MDIO_DEVAD                        (16U)          /* MDIO Device Address. */
#define BITL_MAC_MDIOACC_N__MDIO_DEVAD                        (5U)           /* MDIO Device Address. */
#define BITM_MAC_MDIOACC_N__MDIO_DEVAD                        (0X001F0000U)  /* MDIO Device Address. */
#define BITP_MAC_MDIOACC_N__MDIO_PRTAD                        (21U)          /* MDIO Port Address. */
#define BITL_MAC_MDIOACC_N__MDIO_PRTAD                        (5U)           /* MDIO Port Address. */
#define BITM_MAC_MDIOACC_N__MDIO_PRTAD                        (0X03E00000U)  /* MDIO Port Address. */
#define BITP_MAC_MDIOACC_N__MDIO_OP                           (26U)          /* Operation Code. */
#define BITL_MAC_MDIOACC_N__MDIO_OP                           (2U)           /* Operation Code. */
#define BITM_MAC_MDIOACC_N__MDIO_OP                           (0X0C000000U)  /* Operation Code. */
#define BITP_MAC_MDIOACC_N__MDIO_ST                           (28U)          /* Start of Frame. */
#define BITL_MAC_MDIOACC_N__MDIO_ST                           (2U)           /* Start of Frame. */
#define BITM_MAC_MDIOACC_N__MDIO_ST                           (0X30000000U)  /* Start of Frame. */
#define BITP_MAC_MDIOACC_N__MDIO_TAERR                        (30U)          /* Turnaround Error. */
#define BITL_MAC_MDIOACC_N__MDIO_TAERR                        (1U)           /* Turnaround Error. */
#define BITM_MAC_MDIOACC_N__MDIO_TAERR                        (0X40000000U)  /* Turnaround Error. */
#define BITP_MAC_MDIOACC_N__MDIO_TRDONE                       (31U)          /* Transaction Done. */
#define BITL_MAC_MDIOACC_N__MDIO_TRDONE                       (1U)           /* Transaction Done. */
#define BITM_MAC_MDIOACC_N__MDIO_TRDONE                       (0X80000000U)  /* Transaction Done. */

#define ENUM_MAC_MDIOACC_N__MDIO_ST_CLAUSE22                  (0X00000001U)  /* No description provided */
#define ENUM_MAC_MDIOACC_N__MDIO_ST_CLAUSE45                  (0X00000000U)  /* No description provided */
#define ENUM_MAC_MDIOACC_N__MDIO_OP_MD_ADDR                   (0X00000000U)  /* MD Address Command. */
#define ENUM_MAC_MDIOACC_N__MDIO_OP_MD_WR                     (0X00000001U)  /* Write Command. */
#define ENUM_MAC_MDIOACC_N__MDIO_OP_MD_RD                     (0X00000003U)  /* Read Command. */
#define ENUM_MAC_MDIOACC_N__MDIO_OP_MD_INC_RD                 (0X00000002U)  /* Incremental Read Command. */

/* ----------------------------------------------------------------------------------------------------
          TX_FSIZE                                              Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TX_FSIZE_TX_FRM_SIZE                         (0U)           /* Transmit Frame Size. */
#define BITL_MAC_TX_FSIZE_TX_FRM_SIZE                         (11U)          /* Transmit Frame Size. */
#define BITM_MAC_TX_FSIZE_TX_FRM_SIZE                         (0X000007FFU)  /* Transmit Frame Size. */

/* ----------------------------------------------------------------------------------------------------
          TX                                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TX_TDR                                       (0U)           /* Transmit Data Register. */
#define BITL_MAC_TX_TDR                                       (32U)          /* Transmit Data Register. */
#define BITM_MAC_TX_TDR                                       (0XFFFFFFFFU)  /* Transmit Data Register. */

/* ----------------------------------------------------------------------------------------------------
          TX_SPACE                                              Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TX_SPACE_TX_SPACE                            (0U)           /* Transmit FIFO Space Available in Half Words (16 Bits). */
#define BITL_MAC_TX_SPACE_TX_SPACE                            (14U)          /* Transmit FIFO Space Available in Half Words (16 Bits). */
#define BITM_MAC_TX_SPACE_TX_SPACE                            (0X00003FFFU)  /* Transmit FIFO Space Available in Half Words (16 Bits). */

/* ----------------------------------------------------------------------------------------------------
          TX_THRESH                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TX_THRESH_HOST_TX_THRESH                     (0U)           /* Host Transmit Start Threshold in Cut Through. */
#define BITL_MAC_TX_THRESH_HOST_TX_THRESH                     (6U)           /* Host Transmit Start Threshold in Cut Through. */
#define BITM_MAC_TX_THRESH_HOST_TX_THRESH                     (0X0000003FU)  /* Host Transmit Start Threshold in Cut Through. */

/* ----------------------------------------------------------------------------------------------------
          FIFO_CLR                                              Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_FIFO_CLR_MAC_RXF_CLR                         (0U)           /* Clear the Receive FIFO(s). */
#define BITL_MAC_FIFO_CLR_MAC_RXF_CLR                         (1U)           /* Clear the Receive FIFO(s). */
#define BITM_MAC_FIFO_CLR_MAC_RXF_CLR                         (0X00000001U)  /* Clear the Receive FIFO(s). */
#define BITP_MAC_FIFO_CLR_MAC_TXF_CLR                         (1U)           /* Clear the Host Transmit FIFO. */
#define BITL_MAC_FIFO_CLR_MAC_TXF_CLR                         (1U)           /* Clear the Host Transmit FIFO. */
#define BITM_MAC_FIFO_CLR_MAC_TXF_CLR                         (0X00000002U)  /* Clear the Host Transmit FIFO. */

/* ----------------------------------------------------------------------------------------------------
          SCRATCH_0_                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_SCRATCH_N__SCRATCH_DATA                      (0U)           /* Scratch Data. */
#define BITL_MAC_SCRATCH_N__SCRATCH_DATA                      (32U)          /* Scratch Data. */
#define BITM_MAC_SCRATCH_N__SCRATCH_DATA                      (0XFFFFFFFFU)  /* Scratch Data. */

/* ----------------------------------------------------------------------------------------------------
          MAC_RST_STATUS                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_MAC_RST_STATUS_MAC_OSC_CLK_RDY               (0U)           /* MAC Oscillator Clock Ready. */
#define BITL_MAC_MAC_RST_STATUS_MAC_OSC_CLK_RDY               (1U)           /* MAC Oscillator Clock Ready. */
#define BITM_MAC_MAC_RST_STATUS_MAC_OSC_CLK_RDY               (0X00000001U)  /* MAC Oscillator Clock Ready. */
#define BITP_MAC_MAC_RST_STATUS_MAC_CRYSL_CLK_RDY             (1U)           /* MAC Crystal Clock Ready. */
#define BITL_MAC_MAC_RST_STATUS_MAC_CRYSL_CLK_RDY             (1U)           /* MAC Crystal Clock Ready. */
#define BITM_MAC_MAC_RST_STATUS_MAC_CRYSL_CLK_RDY             (0X00000002U)  /* MAC Crystal Clock Ready. */

/* ----------------------------------------------------------------------------------------------------
          SOFT_RST                                              Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_SOFT_RST_RST_KEY                             (0U)           /* Software Reset. */
#define BITL_MAC_SOFT_RST_RST_KEY                             (16U)          /* Software Reset. */
#define BITM_MAC_SOFT_RST_RST_KEY                             (0X0000FFFFU)  /* Software Reset. */

#define ENUM_MAC_SOFT_RST_RST_KEY_RST_MAC_ONLY_KEY1           (0X00004F1CU)  /* Key1 to Reset the MAC Logic Only. */
#define ENUM_MAC_SOFT_RST_RST_KEY_RST_MAC_ONLY_KEY2           (0X0000C1F4U)  /* Key 2 to Reset the MAC Logic Only. */
#define ENUM_MAC_SOFT_RST_RST_KEY_MAC_RST_EXIT_REQ_KEY1       (0X00006F1AU)  /* Key 1 to Request Release of Reset to the MAC Core Logic. */
#define ENUM_MAC_SOFT_RST_RST_KEY_MAC_RST_EXIT_REQ_KEY2       (0X0000A1F6U)  /* Key 2 to Request Release of Reset to the MAC Core Logic. */

/* ----------------------------------------------------------------------------------------------------
          SPI_INJ_ERR                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                 (0U)           /* Inject an Error on the SPI MISO Path. */
#define BITL_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                 (1U)           /* Inject an Error on the SPI MISO Path. */
#define BITM_MAC_SPI_INJ_ERR_TEST_SPI_INJ_ERR                 (0X00000001U)  /* Inject an Error on the SPI MISO Path. */

/* ----------------------------------------------------------------------------------------------------
          FIFO_SIZE                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_FIFO_SIZE_HTX_SIZE                           (0U)           /* Host Transmit FIFO Size. */
#define BITL_MAC_FIFO_SIZE_HTX_SIZE                           (4U)           /* Host Transmit FIFO Size. */
#define BITM_MAC_FIFO_SIZE_HTX_SIZE                           (0X0000000FU)  /* Host Transmit FIFO Size. */
#define BITP_MAC_FIFO_SIZE_P1_RX_LO_SIZE                      (4U)           /* Port 1 Rx Low Priority FIFO Size. */
#define BITL_MAC_FIFO_SIZE_P1_RX_LO_SIZE                      (4U)           /* Port 1 Rx Low Priority FIFO Size. */
#define BITM_MAC_FIFO_SIZE_P1_RX_LO_SIZE                      (0X000000F0U)  /* Port 1 Rx Low Priority FIFO Size. */
#define BITP_MAC_FIFO_SIZE_P1_RX_HI_SIZE                      (8U)           /* Port 1 Rx High Priority FIFO Size. */
#define BITL_MAC_FIFO_SIZE_P1_RX_HI_SIZE                      (4U)           /* Port 1 Rx High Priority FIFO Size. */
#define BITM_MAC_FIFO_SIZE_P1_RX_HI_SIZE                      (0X00000F00U)  /* Port 1 Rx High Priority FIFO Size. */

#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_0K            (0X00000000U)  /* 0K Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_2K            (0X00000001U)  /* 2K Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_4K            (0X00000002U)  /* 4k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_6K            (0X00000003U)  /* 6k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_8K            (0X00000004U)  /* 8k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_10K           (0X00000005U)  /* 10k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_12K           (0X00000006U)  /* 12k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_14K           (0X00000007U)  /* 14k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_HI_SIZE_RXSIZE_16K           (0X00000008U)  /* 16k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_0K            (0X00000000U)  /* 0K Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_2K            (0X00000001U)  /* 2k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_4K            (0X00000002U)  /* 4k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_6K            (0X00000003U)  /* 6k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_8K            (0X00000004U)  /* 8k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_10K           (0X00000005U)  /* 10k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_12K           (0X00000006U)  /* 12k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_14K           (0X00000007U)  /* 14k Bytes. */
#define ENUM_MAC_FIFO_SIZE_P1_RX_LO_SIZE_RXSIZE_16K           (0X00000008U)  /* 16k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_0K                 (0X00000000U)  /* 0k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_2K                 (0X00000001U)  /* 2k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_4K                 (0X00000002U)  /* 4k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_6K                 (0X00000003U)  /* 6k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_8K                 (0X00000004U)  /* 8k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_10K                (0X00000005U)  /* 10k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_12K                (0X00000006U)  /* 12k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_14K                (0X00000007U)  /* 14k Bytes. */
#define ENUM_MAC_FIFO_SIZE_HTX_SIZE_TXSIZE_16K                (0X00000008U)  /* 16k Byte. */

/* ----------------------------------------------------------------------------------------------------
          TFC                                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TFC_TFC                                      (0U)           /* Number of Frames in the Tx FIFO. */
#define BITL_MAC_TFC_TFC                                      (9U)           /* Number of Frames in the Tx FIFO. */
#define BITM_MAC_TFC_TFC                                      (0X000001FFU)  /* Number of Frames in the Tx FIFO. */

/* ----------------------------------------------------------------------------------------------------
          TXSIZE                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TXSIZE_TX_SIZE                               (0U)           /* Data in the Tx FIFO. Number of Half Words(16 Bit). */
#define BITL_MAC_TXSIZE_TX_SIZE                               (14U)          /* Data in the Tx FIFO. Number of Half Words(16 Bit). */
#define BITM_MAC_TXSIZE_TX_SIZE                               (0X00003FFFU)  /* Data in the Tx FIFO. Number of Half Words(16 Bit). */

/* ----------------------------------------------------------------------------------------------------
          HTX_OVF_FRM_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT              (0U)           /* Counts Host Tx Frames Dropped Due to FIFO Overflow. */
#define BITL_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT              (32U)          /* Counts Host Tx Frames Dropped Due to FIFO Overflow. */
#define BITM_MAC_HTX_OVF_FRM_CNT_HTX_OVF_FRM_CNT              (0XFFFFFFFFU)  /* Counts Host Tx Frames Dropped Due to FIFO Overflow. */

/* ----------------------------------------------------------------------------------------------------
          MECC_ERR_ADDR                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                  (0U)           /* Address of an Uncorrectable ECC Error in Memory. */
#define BITL_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                  (14U)          /* Address of an Uncorrectable ECC Error in Memory. */
#define BITM_MAC_MECC_ERR_ADDR_MECC_ERR_ADDR                  (0X00003FFFU)  /* Address of an Uncorrectable ECC Error in Memory. */

/* ----------------------------------------------------------------------------------------------------
          CECC_ERR_0_                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_CECC_ERR_N__CECC_ERR_CNT                     (0U)           /* Corrected ECC Error Count. */
#define BITL_MAC_CECC_ERR_N__CECC_ERR_CNT                     (10U)          /* Corrected ECC Error Count. */
#define BITM_MAC_CECC_ERR_N__CECC_ERR_CNT                     (0X000003FFU)  /* Corrected ECC Error Count. */

/* ----------------------------------------------------------------------------------------------------
          ADDR_FILT_UPR_0_                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_ADDR_FILT_UPR_N__MAC_ADDR                    (0U)           /* MAC Address. */
#define BITL_MAC_ADDR_FILT_UPR_N__MAC_ADDR                    (16U)          /* MAC Address. */
#define BITM_MAC_ADDR_FILT_UPR_N__MAC_ADDR                    (0X0000FFFFU)  /* MAC Address. */
#define BITP_MAC_ADDR_FILT_UPR_N__TO_HOST                     (16U)          /* Forward Frames Matching This MAC Address to the Host. */
#define BITL_MAC_ADDR_FILT_UPR_N__TO_HOST                     (1U)           /* Forward Frames Matching This MAC Address to the Host. */
#define BITM_MAC_ADDR_FILT_UPR_N__TO_HOST                     (0X00010000U)  /* Forward Frames Matching This MAC Address to the Host. */
#define BITP_MAC_ADDR_FILT_UPR_N__HOST_PRI                    (19U)          /* Host Rx Port Priority. */
#define BITL_MAC_ADDR_FILT_UPR_N__HOST_PRI                    (1U)           /* Host Rx Port Priority. */
#define BITM_MAC_ADDR_FILT_UPR_N__HOST_PRI                    (0X00080000U)  /* Host Rx Port Priority. */
#define BITP_MAC_ADDR_FILT_UPR_N__APPLY2PORT1                 (30U)          /* Apply to Port 1. */
#define BITL_MAC_ADDR_FILT_UPR_N__APPLY2PORT1                 (1U)           /* Apply to Port 1. */
#define BITM_MAC_ADDR_FILT_UPR_N__APPLY2PORT1                 (0X40000000U)  /* Apply to Port 1. */

#define ENUM_MAC_ADDR_FILT_UPR_N__APPLY2PORT1_NOTAPPLY        (0X00000000U)  /* Do Not Apply to Port 1. */
#define ENUM_MAC_ADDR_FILT_UPR_N__APPLY2PORT1_APPLY           (0X00000001U)  /* Apply to Port 1. */

/* ----------------------------------------------------------------------------------------------------
          ADDR_FILT_LWR_0_                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_ADDR_FILT_LWR_N__MAC_ADDR                    (0U)           /* MAC Address. */
#define BITL_MAC_ADDR_FILT_LWR_N__MAC_ADDR                    (32U)          /* MAC Address. */
#define BITM_MAC_ADDR_FILT_LWR_N__MAC_ADDR                    (0XFFFFFFFFU)  /* MAC Address. */

/* ----------------------------------------------------------------------------------------------------
          ADDR_MSK_UPR_0_                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_ADDR_MSK_UPR_N__MAC_ADDR_MASK                (0U)           /* MAC Address Bit Mask for the Address Table. */
#define BITL_MAC_ADDR_MSK_UPR_N__MAC_ADDR_MASK                (16U)          /* MAC Address Bit Mask for the Address Table. */
#define BITM_MAC_ADDR_MSK_UPR_N__MAC_ADDR_MASK                (0X0000FFFFU)  /* MAC Address Bit Mask for the Address Table. */

/* ----------------------------------------------------------------------------------------------------
          ADDR_MSK_LWR_0_                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_ADDR_MSK_LWR_N__MAC_ADDR_MASK                (0U)           /* MAC Address Bit Mask for the Address Table. */
#define BITL_MAC_ADDR_MSK_LWR_N__MAC_ADDR_MASK                (32U)          /* MAC Address Bit Mask for the Address Table. */
#define BITM_MAC_ADDR_MSK_LWR_N__MAC_ADDR_MASK                (0XFFFFFFFFU)  /* MAC Address Bit Mask for the Address Table. */

/* ----------------------------------------------------------------------------------------------------
          TS_ADDEND                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_ADDEND_TS_ADDEND                          (0U)           /* Timestamp Accumulator Addend. */
#define BITL_MAC_TS_ADDEND_TS_ADDEND                          (32U)          /* Timestamp Accumulator Addend. */
#define BITM_MAC_TS_ADDEND_TS_ADDEND                          (0XFFFFFFFFU)  /* Timestamp Accumulator Addend. */

/* ----------------------------------------------------------------------------------------------------
          TS_1SEC_CMP                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_1SEC_CMP_TS_1SEC_CMP                      (0U)           /* Timestamp 1 Second Compare Value. */
#define BITL_MAC_TS_1SEC_CMP_TS_1SEC_CMP                      (32U)          /* Timestamp 1 Second Compare Value. */
#define BITM_MAC_TS_1SEC_CMP_TS_1SEC_CMP                      (0XFFFFFFFFU)  /* Timestamp 1 Second Compare Value. */

/* ----------------------------------------------------------------------------------------------------
          TS_SEC_CNT                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_SEC_CNT_TS_SEC_CNT                        (0U)           /* Write to the Seconds Counter. */
#define BITL_MAC_TS_SEC_CNT_TS_SEC_CNT                        (32U)          /* Write to the Seconds Counter. */
#define BITM_MAC_TS_SEC_CNT_TS_SEC_CNT                        (0XFFFFFFFFU)  /* Write to the Seconds Counter. */

/* ----------------------------------------------------------------------------------------------------
          TS_NS_CNT                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_NS_CNT_TS_NS_CNT                          (0U)           /* Write to the Nanoseconds Counter. */
#define BITL_MAC_TS_NS_CNT_TS_NS_CNT                          (32U)          /* Write to the Nanoseconds Counter. */
#define BITM_MAC_TS_NS_CNT_TS_NS_CNT                          (0XFFFFFFFFU)  /* Write to the Nanoseconds Counter. */

/* ----------------------------------------------------------------------------------------------------
          TS_CFG                                                Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_CFG_TS_EN                                 (0U)           /* Enable the 1588 Timestamp Counter. */
#define BITL_MAC_TS_CFG_TS_EN                                 (1U)           /* Enable the 1588 Timestamp Counter. */
#define BITM_MAC_TS_CFG_TS_EN                                 (0X00000001U)  /* Enable the 1588 Timestamp Counter. */
#define BITP_MAC_TS_CFG_TS_CLR                                (1U)           /* Clear the 1588 Timestamp Counters. */
#define BITL_MAC_TS_CFG_TS_CLR                                (1U)           /* Clear the 1588 Timestamp Counters. */
#define BITM_MAC_TS_CFG_TS_CLR                                (0X00000002U)  /* Clear the 1588 Timestamp Counters. */
#define BITP_MAC_TS_CFG_TS_TIMER_DEF                          (2U)           /* The Default Value for the TS_TIMER Output. */
#define BITL_MAC_TS_CFG_TS_TIMER_DEF                          (1U)           /* The Default Value for the TS_TIMER Output. */
#define BITM_MAC_TS_CFG_TS_TIMER_DEF                          (0X00000004U)  /* The Default Value for the TS_TIMER Output. */
#define BITP_MAC_TS_CFG_TS_TIMER_STOP                         (3U)           /* Stop Toggling the TS_TIMER Output. */
#define BITL_MAC_TS_CFG_TS_TIMER_STOP                         (1U)           /* Stop Toggling the TS_TIMER Output. */
#define BITM_MAC_TS_CFG_TS_TIMER_STOP                         (0X00000008U)  /* Stop Toggling the TS_TIMER Output. */
#define BITP_MAC_TS_CFG_TS_CAPT_FREE_CNT                      (4U)           /* Capture the Free Running Counter. */
#define BITL_MAC_TS_CFG_TS_CAPT_FREE_CNT                      (1U)           /* Capture the Free Running Counter. */
#define BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT                      (0X00000010U)  /* Capture the Free Running Counter. */

/* ----------------------------------------------------------------------------------------------------
          TS_TIMER_HI                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_TIMER_HI_TS_TIMER_HI                      (0U)           /* TS_TIMER High Period. */
#define BITL_MAC_TS_TIMER_HI_TS_TIMER_HI                      (32U)          /* TS_TIMER High Period. */
#define BITM_MAC_TS_TIMER_HI_TS_TIMER_HI                      (0XFFFFFFFFU)  /* TS_TIMER High Period. */

/* ----------------------------------------------------------------------------------------------------
          TS_TIMER_LO                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_TIMER_LO_TS_TIMER_LO                      (0U)           /* TS_TIMER Low Period. */
#define BITL_MAC_TS_TIMER_LO_TS_TIMER_LO                      (32U)          /* TS_TIMER Low Period. */
#define BITM_MAC_TS_TIMER_LO_TS_TIMER_LO                      (0XFFFFFFFFU)  /* TS_TIMER Low Period. */

/* ----------------------------------------------------------------------------------------------------
          TS_TIMER_QE_CORR                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR            (0U)           /* TS_TIMER Quantization Error Correction Value. */
#define BITL_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR            (8U)           /* TS_TIMER Quantization Error Correction Value. */
#define BITM_MAC_TS_TIMER_QE_CORR_TS_TIMER_QE_CORR            (0X000000FFU)  /* TS_TIMER Quantization Error Correction Value. */

/* ----------------------------------------------------------------------------------------------------
          TS_TIMER_START                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_TIMER_START_TS_TSTART                     (0U)           /* Point in Time at Which to Start the TS_TIMER Counter. */
#define BITL_MAC_TS_TIMER_START_TS_TSTART                     (32U)          /* Point in Time at Which to Start the TS_TIMER Counter. */
#define BITM_MAC_TS_TIMER_START_TS_TSTART                     (0XFFFFFFFFU)  /* Point in Time at Which to Start the TS_TIMER Counter. */

/* ----------------------------------------------------------------------------------------------------
          TS_EXT_CAPT0                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                    (0U)           /* Timestamp Captured on the Assertion of TS_CAPT Pin. */
#define BITL_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                    (32U)          /* Timestamp Captured on the Assertion of TS_CAPT Pin. */
#define BITM_MAC_TS_EXT_CAPT0_TS_EXT_CAPTD                    (0XFFFFFFFFU)  /* Timestamp Captured on the Assertion of TS_CAPT Pin. */

/* ----------------------------------------------------------------------------------------------------
          TS_EXT_CAPT1                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                    (0U)           /* Timestamp Captured on the Assertion of TS_CAPT Pin. */
#define BITL_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                    (32U)          /* Timestamp Captured on the Assertion of TS_CAPT Pin. */
#define BITM_MAC_TS_EXT_CAPT1_TS_EXT_CAPTD                    (0XFFFFFFFFU)  /* Timestamp Captured on the Assertion of TS_CAPT Pin. */

/* ----------------------------------------------------------------------------------------------------
          TS_FREECNT_CAPT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                 (0U)           /* Captured Free Running Counter. */
#define BITL_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                 (32U)          /* Captured Free Running Counter. */
#define BITM_MAC_TS_FREECNT_CAPT_TS_CNT_CAPTD                 (0XFFFFFFFFU)  /* Captured Free Running Counter. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_FSIZE                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_FSIZE_P1_RX_FRM_SIZE                   (0U)           /* Receive Frame Size. */
#define BITL_MAC_P1_RX_FSIZE_P1_RX_FRM_SIZE                   (11U)          /* Receive Frame Size. */
#define BITM_MAC_P1_RX_FSIZE_P1_RX_FRM_SIZE                   (0X000007FFU)  /* Receive Frame Size. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_P1_RDR                                 (0U)           /* Receive Data Register. */
#define BITL_MAC_P1_RX_P1_RDR                                 (32U)          /* Receive Data Register. */
#define BITM_MAC_P1_RX_P1_RDR                                 (0XFFFFFFFFU)  /* Receive Data Register. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_FRM_CNT                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_FRM_CNT_P1_RX_FRM_CNT                  (0U)           /* Rx Frame Count. */
#define BITL_MAC_P1_RX_FRM_CNT_P1_RX_FRM_CNT                  (32U)          /* Rx Frame Count. */
#define BITM_MAC_P1_RX_FRM_CNT_P1_RX_FRM_CNT                  (0XFFFFFFFFU)  /* Rx Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_BCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_BCAST_CNT_P1_RX_BCAST_CNT              (0U)           /* Rx Broadcast Frame Count. */
#define BITL_MAC_P1_RX_BCAST_CNT_P1_RX_BCAST_CNT              (32U)          /* Rx Broadcast Frame Count. */
#define BITM_MAC_P1_RX_BCAST_CNT_P1_RX_BCAST_CNT              (0XFFFFFFFFU)  /* Rx Broadcast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_MCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_MCAST_CNT_P1_RX_MCAST_CNT              (0U)           /* Rx Multicast Frame Count. */
#define BITL_MAC_P1_RX_MCAST_CNT_P1_RX_MCAST_CNT              (32U)          /* Rx Multicast Frame Count. */
#define BITM_MAC_P1_RX_MCAST_CNT_P1_RX_MCAST_CNT              (0XFFFFFFFFU)  /* Rx Multicast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_UCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_UCAST_CNT_P1_RX_UCAST_CNT              (0U)           /* Rx Unicast Frame Count. */
#define BITL_MAC_P1_RX_UCAST_CNT_P1_RX_UCAST_CNT              (32U)          /* Rx Unicast Frame Count. */
#define BITM_MAC_P1_RX_UCAST_CNT_P1_RX_UCAST_CNT              (0XFFFFFFFFU)  /* Rx Unicast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_CRC_ERR_CNT                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_CRC_ERR_CNT_P1_RX_CRC_ERR_CNT          (0U)           /* Rx CRC Errored Frame Count. */
#define BITL_MAC_P1_RX_CRC_ERR_CNT_P1_RX_CRC_ERR_CNT          (32U)          /* Rx CRC Errored Frame Count. */
#define BITM_MAC_P1_RX_CRC_ERR_CNT_P1_RX_CRC_ERR_CNT          (0XFFFFFFFFU)  /* Rx CRC Errored Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_ALGN_ERR_CNT                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_ALGN_ERR_CNT_P1_RX_ALGN_ERR_CNT        (0U)           /* Rx Align Error Count. */
#define BITL_MAC_P1_RX_ALGN_ERR_CNT_P1_RX_ALGN_ERR_CNT        (32U)          /* Rx Align Error Count. */
#define BITM_MAC_P1_RX_ALGN_ERR_CNT_P1_RX_ALGN_ERR_CNT        (0XFFFFFFFFU)  /* Rx Align Error Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_LS_ERR_CNT                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_LS_ERR_CNT_P1_RX_LS_ERR_CNT            (0U)           /* Rx Long/Short Frame Error Count. */
#define BITL_MAC_P1_RX_LS_ERR_CNT_P1_RX_LS_ERR_CNT            (32U)          /* Rx Long/Short Frame Error Count. */
#define BITM_MAC_P1_RX_LS_ERR_CNT_P1_RX_LS_ERR_CNT            (0XFFFFFFFFU)  /* Rx Long/Short Frame Error Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_PHY_ERR_CNT                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_PHY_ERR_CNT_P1_RX_PHY_ERR_CNT          (0U)           /* Rx PHY Error Count. */
#define BITL_MAC_P1_RX_PHY_ERR_CNT_P1_RX_PHY_ERR_CNT          (32U)          /* Rx PHY Error Count. */
#define BITM_MAC_P1_RX_PHY_ERR_CNT_P1_RX_PHY_ERR_CNT          (0XFFFFFFFFU)  /* Rx PHY Error Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_TX_FRM_CNT                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_TX_FRM_CNT_P1_TX_FRM_CNT                  (0U)           /* Tx Frame Count. */
#define BITL_MAC_P1_TX_FRM_CNT_P1_TX_FRM_CNT                  (32U)          /* Tx Frame Count. */
#define BITM_MAC_P1_TX_FRM_CNT_P1_TX_FRM_CNT                  (0XFFFFFFFFU)  /* Tx Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_TX_BCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_TX_BCAST_CNT_P1_TX_BCAST_CNT              (0U)           /* Tx Broadcast Frame Count. */
#define BITL_MAC_P1_TX_BCAST_CNT_P1_TX_BCAST_CNT              (32U)          /* Tx Broadcast Frame Count. */
#define BITM_MAC_P1_TX_BCAST_CNT_P1_TX_BCAST_CNT              (0XFFFFFFFFU)  /* Tx Broadcast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_TX_MCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_TX_MCAST_CNT_P1_TX_MCAST_CNT              (0U)           /* Tx Multicast Frame Count. */
#define BITL_MAC_P1_TX_MCAST_CNT_P1_TX_MCAST_CNT              (32U)          /* Tx Multicast Frame Count. */
#define BITM_MAC_P1_TX_MCAST_CNT_P1_TX_MCAST_CNT              (0XFFFFFFFFU)  /* Tx Multicast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_TX_UCAST_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_TX_UCAST_CNT_P1_TX_UCAST_CNT              (0U)           /* Tx Unicast Frame Count. */
#define BITL_MAC_P1_TX_UCAST_CNT_P1_TX_UCAST_CNT              (32U)          /* Tx Unicast Frame Count. */
#define BITM_MAC_P1_TX_UCAST_CNT_P1_TX_UCAST_CNT              (0XFFFFFFFFU)  /* Tx Unicast Frame Count. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_DROP_FULL_CNT                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_DROP_FULL_CNT_P1_RX_DROP_FULL_CNT      (0U)           /* Rx Frames Dropped Due to FIFO Full. */
#define BITL_MAC_P1_RX_DROP_FULL_CNT_P1_RX_DROP_FULL_CNT      (32U)          /* Rx Frames Dropped Due to FIFO Full. */
#define BITM_MAC_P1_RX_DROP_FULL_CNT_P1_RX_DROP_FULL_CNT      (0XFFFFFFFFU)  /* Rx Frames Dropped Due to FIFO Full. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_DROP_FILT_CNT                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_DROP_FILT_CNT_P1_RX_DROP_FILT_CNT      (0U)           /* Rx Frames Dropped Due to Filtering. */
#define BITL_MAC_P1_RX_DROP_FILT_CNT_P1_RX_DROP_FILT_CNT      (32U)          /* Rx Frames Dropped Due to Filtering. */
#define BITM_MAC_P1_RX_DROP_FILT_CNT_P1_RX_DROP_FILT_CNT      (0XFFFFFFFFU)  /* Rx Frames Dropped Due to Filtering. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_IFG_ERR_CNT                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_IFG_ERR_CNT_P1_RX_IFG_ERR_CNT          (0U)           /* IFG Error Counter for Port 1 Received Frames. */
#define BITL_MAC_P1_RX_IFG_ERR_CNT_P1_RX_IFG_ERR_CNT          (32U)          /* IFG Error Counter for Port 1 Received Frames. */
#define BITM_MAC_P1_RX_IFG_ERR_CNT_P1_RX_IFG_ERR_CNT          (0XFFFFFFFFU)  /* IFG Error Counter for Port 1 Received Frames. */

/* ----------------------------------------------------------------------------------------------------
          P1_TX_IFG                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_TX_IFG_P1_TX_IFG                          (0U)           /* Inter Frame Gap. */
#define BITL_MAC_P1_TX_IFG_P1_TX_IFG                          (8U)           /* Inter Frame Gap. */
#define BITM_MAC_P1_TX_IFG_P1_TX_IFG                          (0X000000FFU)  /* Inter Frame Gap. */

/* ----------------------------------------------------------------------------------------------------
          P1_LOOP                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_LOOP_P1_LOOPBACK_EN                       (0U)           /* MAC Loopback. */
#define BITL_MAC_P1_LOOP_P1_LOOPBACK_EN                       (1U)           /* MAC Loopback. */
#define BITM_MAC_P1_LOOP_P1_LOOPBACK_EN                       (0X00000001U)  /* MAC Loopback. */

#define ENUM_MAC_P1_LOOP_P1_LOOPBACK_EN_LOOPB_DIS             (0X00000000U)  /* Normal Operation - Loopback Disabled. */
#define ENUM_MAC_P1_LOOP_P1_LOOPBACK_EN_LOOPB_EN              (0X00000001U)  /* Loopback Enabled. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_CRC_EN                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_CRC_EN_P1_CRC_CHK_EN                   (0U)           /* CRC Check Enable on Receive. */
#define BITL_MAC_P1_RX_CRC_EN_P1_CRC_CHK_EN                   (1U)           /* CRC Check Enable on Receive. */
#define BITM_MAC_P1_RX_CRC_EN_P1_CRC_CHK_EN                   (0X00000001U)  /* CRC Check Enable on Receive. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_IFG                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_IFG_P1_RX_IFG                          (0U)           /* Inter Frame Gap. */
#define BITL_MAC_P1_RX_IFG_P1_RX_IFG                          (6U)           /* Inter Frame Gap. */
#define BITM_MAC_P1_RX_IFG_P1_RX_IFG                          (0X0000003FU)  /* Inter Frame Gap. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_MAX_LEN                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_MAX_LEN_P1_MAX_FRM_LEN                 (0U)           /* Max Frame Length on Receive. */
#define BITL_MAC_P1_RX_MAX_LEN_P1_MAX_FRM_LEN                 (16U)          /* Max Frame Length on Receive. */
#define BITM_MAC_P1_RX_MAX_LEN_P1_MAX_FRM_LEN                 (0X0000FFFFU)  /* Max Frame Length on Receive. */

/* ----------------------------------------------------------------------------------------------------
          P1_RX_MIN_LEN                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_RX_MIN_LEN_P1_MIN_FRM_LEN                 (0U)           /* Min Frame Length on Receive. */
#define BITL_MAC_P1_RX_MIN_LEN_P1_MIN_FRM_LEN                 (16U)          /* Min Frame Length on Receive. */
#define BITM_MAC_P1_RX_MIN_LEN_P1_MIN_FRM_LEN                 (0X0000FFFFU)  /* Min Frame Length on Receive. */

/* ----------------------------------------------------------------------------------------------------
          P1_LO_RFC                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_LO_RFC_P1_LO_RFC                          (0U)           /* Receive Frame Count for the Low Priority FIFO. */
#define BITL_MAC_P1_LO_RFC_P1_LO_RFC                          (9U)           /* Receive Frame Count for the Low Priority FIFO. */
#define BITM_MAC_P1_LO_RFC_P1_LO_RFC                          (0X000001FFU)  /* Receive Frame Count for the Low Priority FIFO. */

/* ----------------------------------------------------------------------------------------------------
          P1_HI_RFC                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_HI_RFC_P1_HI_RFC                          (0U)           /* Receive Frame Count for the High Priority FIFO. */
#define BITL_MAC_P1_HI_RFC_P1_HI_RFC                          (9U)           /* Receive Frame Count for the High Priority FIFO. */
#define BITM_MAC_P1_HI_RFC_P1_HI_RFC                          (0X000001FFU)  /* Receive Frame Count for the High Priority FIFO. */

/* ----------------------------------------------------------------------------------------------------
          P1_LO_RXSIZE                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_LO_RXSIZE_P1_LO_RXSIZE                    (0U)           /* Data in the Rx FIFO. Number of Half Words(16 Bit). */
#define BITL_MAC_P1_LO_RXSIZE_P1_LO_RXSIZE                    (14U)          /* Data in the Rx FIFO. Number of Half Words(16 Bit). */
#define BITM_MAC_P1_LO_RXSIZE_P1_LO_RXSIZE                    (0X00003FFFU)  /* Data in the Rx FIFO. Number of Half Words(16 Bit). */

/* ----------------------------------------------------------------------------------------------------
          P1_HI_RXSIZE                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_P1_HI_RXSIZE_P1_HI_RXSIZE                    (0U)           /* Data in the Rx FIFO. Number of Half Words(16 Bit). */
#define BITL_MAC_P1_HI_RXSIZE_P1_HI_RXSIZE                    (14U)          /* Data in the Rx FIFO. Number of Half Words(16 Bit). */
#define BITM_MAC_P1_HI_RXSIZE_P1_HI_RXSIZE                    (0X00003FFFU)  /* Data in the Rx FIFO. Number of Half Words(16 Bit). */


#endif /* ADIN1110_MAC_ADDR_RDEF_H */

