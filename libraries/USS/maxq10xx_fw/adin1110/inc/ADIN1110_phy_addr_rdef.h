
/* ================================================================================
     Project     :   ADIN1110
     File        :   ADIN1110_addr_rdef.h
     Description :   C header file contains macros for Registers' address relative to instances and plain bit-fields.


     Copyright (c) 2021 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.
   ================================================================================ */

#ifndef ADIN1110_ADDR_RDEF_H
#define ADIN1110_ADDR_RDEF_H


#if defined(_LANGUAGE_C) || (defined(__GNUC__) && !defined(__ASSEMBLER__))
#include <stdint.h>
#endif /* _LANGUAGE_C */

/* ====================================================================================================
        ADIN1110 Module Instances Address and Mask Definitions
   ==================================================================================================== */


#ifndef ADIN1110_ADDR_RDEF_H_
#define ADIN1110_ADDR_RDEF_H_    /* ADIN1110: 10BASE-T1L Single Pair Ethernet PHY core */


/* ====================================================================================================
        ADIN1110 Module Register Address Offset Definitions
   ==================================================================================================== */
#define ADDR_PMA_PMD_CNTRL1                                      (0X010000U)    /* PMA/PMD Control 1 Register */
#define ADDR_PMA_PMD_STAT1                                       (0X010001U)    /* PMA/PMD Status 1 Register */
#define ADDR_PMA_PMD_DEVS_IN_PKG1                                (0X010005U)    /* PMA/PMD MMD Devices in Package 1 */
#define ADDR_PMA_PMD_DEVS_IN_PKG2                                (0X010006U)    /* PMA/PMD MMD Devices in Package 2 Register */
#define ADDR_PMA_PMD_CNTRL2                                      (0X010007U)    /* PMA/PMD Control 2 Register */
#define ADDR_PMA_PMD_STAT2                                       (0X010008U)    /* PMA/PMD Status 2 */
#define ADDR_PMA_PMD_TX_DIS                                      (0X010009U)    /* PMA/PMD Transmit Disable Register */
#define ADDR_PMA_PMD_EXT_ABILITY                                 (0X01000BU)    /* PMA/PMD Extended Abilities Register */
#define ADDR_PMA_PMD_BT1_ABILITY                                 (0X010012U)    /* BASE-T1 PMA/PMD Extended Ability Register */
#define ADDR_PMA_PMD_BT1_CONTROL                                 (0X010834U)    /* BASE-T1 PMA/PMD Control Register */
#define ADDR_B10L_PMA_CNTRL                                      (0X0108F6U)    /* 10BASE-T1L PMA Control Register */
#define ADDR_B10L_PMA_STAT                                       (0X0108F7U)    /* 10BASE-T1L PMA Status Register */
#define ADDR_B10L_TEST_MODE_CNTRL                                (0X0108F8U)    /* 10BASE-T1L Test Mode Control Register */
#define ADDR_CR_STBL_CHK_FOFFS_SAT_THR                           (0X018015U)    /* Frequency Offset Saturation Threshold for CR Stability Check Register */
#define ADDR_SLV_FLTR_ECHO_ACQ_CR_KP                             (0X0181E7U)    /* Slave IIR Filter Change Echo Acquisition Clock Recovery Proportional Gain Register */
#define ADDR_B10L_PMA_LINK_STAT                                  (0X018302U)    /* 10BASE-T1L PMA Link Status Register */
#define ADDR_MSE_VAL                                             (0X01830BU)    /* MSE Value Register */
#define ADDR_PCS_CNTRL1                                          (0X030000U)    /* PCS Control 1 Register */
#define ADDR_PCS_STAT1                                           (0X030001U)    /* PCS Status 1 Register */
#define ADDR_PCS_DEVS_IN_PKG1                                    (0X030005U)    /* PCS MMD Devices in Package 1 Register */
#define ADDR_PCS_DEVS_IN_PKG2                                    (0X030006U)    /* PCS MMD Devices in Package 2 Register */
#define ADDR_PCS_STAT2                                           (0X030008U)    /* PCS Status 2 Register */
#define ADDR_B10L_PCS_CNTRL                                      (0X0308E6U)    /* 10BASE-T1L PCS Control Register */
#define ADDR_B10L_PCS_STAT                                       (0X0308E7U)    /* 10BASE-T1L PCS Status Register */
#define ADDR_AN_DEVS_IN_PKG1                                     (0X070005U)    /* AUTO-_NEGOTIATION MMD Devices in Package 1 Register */
#define ADDR_AN_DEVS_IN_PKG2                                     (0X070006U)    /* AUTO-_NEGOTIATION MMD Devices in Package 2 Register */
#define ADDR_AN_CONTROL                                          (0X070200U)    /* BASE-T1 Autonegotiation Control Register */
#define ADDR_AN_STATUS                                           (0X070201U)    /* BASE-T1 Autonegotiation Status Register */
#define ADDR_AN_ADV_ABILITY_L                                    (0X070202U)    /* BASE-T1 Autonegotiation Advertisement [15:0] Register */
#define ADDR_AN_ADV_ABILITY_M                                    (0X070203U)    /* BASE-T1 Autonegotiation Advertisement [31:16] Register */
#define ADDR_AN_ADV_ABILITY_H                                    (0X070204U)    /* BASE-T1 Autonegotiation Advertisement [47:32] Register */
#define ADDR_AN_LP_ADV_ABILITY_L                                 (0X070205U)    /* BASE-T1 Autonegotiation Link Partner Base Page Ability [15:0] Register */
#define ADDR_AN_LP_ADV_ABILITY_M                                 (0X070206U)    /* BASE-T1 Autonegotiation Link Partner Base Page Ability [31:16] Register */
#define ADDR_AN_LP_ADV_ABILITY_H                                 (0X070207U)    /* BASE-T1 Autonegotiation Link Partner Base Page Ability [47:32] Register */
#define ADDR_AN_NEXT_PAGE_L                                      (0X070208U)    /* BASE-T1 Autonegotiation Next Page Transmit [15:0] Register */
#define ADDR_AN_NEXT_PAGE_M                                      (0X070209U)    /* BASE-T1 Autonegotiation Next Page Transmit [31:16] Register */
#define ADDR_AN_NEXT_PAGE_H                                      (0X07020AU)    /* BASE-T1 Autonegotiation Next Page Transmit [47:32] Register */
#define ADDR_AN_LP_NEXT_PAGE_L                                   (0X07020BU)    /* BASE-T1 Autonegotiation Link Partner Next Page Ability [15:0] Register */
#define ADDR_AN_LP_NEXT_PAGE_M                                   (0X07020CU)    /* BASE-T1 Autonegotiation Link Partner Next Page Ability [31:16] Register */
#define ADDR_AN_LP_NEXT_PAGE_H                                   (0X07020DU)    /* BASE-T1 Autonegotiation Link Partner Next Page Ability [47:32] Register */
#define ADDR_AN_B10_ADV_ABILITY                                  (0X07020EU)    /* 10BASE-T1 Autonegotiation Control Register */
#define ADDR_AN_B10_LP_ADV_ABILITY                               (0X07020FU)    /* 10BASE-T1 Autonegotiation Status Register */
#define ADDR_AN_FRC_MODE_EN                                      (0X078000U)    /* Autonegotiation Force Mode Enable Register */
#define ADDR_AN_STATUS_EXTRA                                     (0X078001U)    /* Extra Autonegotiation Status Register */
#define ADDR_AN_PHY_INST_STATUS                                  (0X078030U)    /* PHY Instantaneous Status. */
#define ADDR_MMD1_DEV_ID1                                        (0X1E0002U)    /* Vendor Specific MMD 1 Device Identifier High Register */
#define ADDR_MMD1_DEV_ID2                                        (0X1E0003U)    /* Vendor Specific MMD 1 Device Identifier Low Register */
#define ADDR_MMD1_DEVS_IN_PKG1                                   (0X1E0005U)    /* Vendor Specific 1 MMD Devices in Package Register */
#define ADDR_MMD1_DEVS_IN_PKG2                                   (0X1E0006U)    /* Vendor Specific 1 MMD Devices in Package Register */
#define ADDR_MMD1_STATUS                                         (0X1E0008U)    /* Vendor Specific MMD 1 Status Register */
#define ADDR_CRSM_IRQ_STATUS                                     (0X1E0010U)    /* System Interrupt Status Register */
#define ADDR_CRSM_IRQ_MASK                                       (0X1E0020U)    /* System Interrupt Mask Register */
#define ADDR_CRSM_SFT_RST                                        (0X1E8810U)    /* Software Reset Register */
#define ADDR_CRSM_SFT_PD_CNTRL                                   (0X1E8812U)    /* Software Power-down Control Register */
#define ADDR_CRSM_PHY_SUBSYS_RST                                 (0X1E8814U)    /* PHY Subsystem Reset Register */
#define ADDR_CRSM_MAC_IF_RST                                     (0X1E8815U)    /* PHY MAC Interface Reset Register */
#define ADDR_CRSM_STAT                                           (0X1E8818U)    /* System Status Register */
#define ADDR_CRSM_PMG_CNTRL                                      (0X1E8819U)    /* CRSM Power Management Control Register. */
#define ADDR_CRSM_DIAG_CLK_CTRL                                  (0X1E882CU)    /* CRSM Diagnostics Clock Control. */
#define ADDR_MGMT_PRT_PKG                                        (0X1E8C22U)    /* Package Configuration Values Register */
#define ADDR_MGMT_MDIO_CNTRL                                     (0X1E8C30U)    /* MDIO Control Register */
#define ADDR_DIGIO_PINMUX                                        (0X1E8C56U)    /* Pinmux Configuration 1 Register */
#define ADDR_LED0_BLINK_TIME_CNTRL                               (0X1E8C80U)    /* LED 0 ON/_OFF Blink Time Register */
#define ADDR_LED1_BLINK_TIME_CNTRL                               (0X1E8C81U)    /* LED 1 ON/_OFF Blink Time Register */
#define ADDR_LED_CNTRL                                           (0X1E8C82U)    /* LED Control Register */
#define ADDR_LED_POLARITY                                        (0X1E8C83U)    /* LED Polarity Register */
#define ADDR_MMD2_DEV_ID1                                        (0X1F0002U)    /* Vendor Specific MMD 2 Device Identifier High Register. */
#define ADDR_MMD2_DEV_ID2                                        (0X1F0003U)    /* Vendor Specific MMD 2 Device Identifier Low Register. */
#define ADDR_MMD2_DEVS_IN_PKG1                                   (0X1F0005U)    /* Vendor Specific 2 MMD Devices in Package Register */
#define ADDR_MMD2_DEVS_IN_PKG2                                   (0X1F0006U)    /* Vendor Specific 2 MMD Devices in Package Register */
#define ADDR_MMD2_STATUS                                         (0X1F0008U)    /* Vendor Specific MMD 2 Status Register */
#define ADDR_PHY_SUBSYS_IRQ_STATUS                               (0X1F0011U)    /* PHY Subsystem Interrupt Status Register */
#define ADDR_PHY_SUBSYS_IRQ_MASK                                 (0X1F0021U)    /* PHY Subsystem Interrupt Mask Register */
#define ADDR_FC_EN                                               (0X1F8001U)    /* Frame Checker Enable Register */
#define ADDR_FC_IRQ_EN                                           (0X1F8004U)    /* Frame Checker Interrupt Enable Register */
#define ADDR_FC_TX_SEL                                           (0X1F8005U)    /* Frame Checker Transmit Select Register */
#define ADDR_RX_ERR_CNT                                          (0X1F8008U)    /* Receive Error Count Register */
#define ADDR_FC_FRM_CNT_H                                        (0X1F8009U)    /* Frame Checker Count High Register */
#define ADDR_FC_FRM_CNT_L                                        (0X1F800AU)    /* Frame Checker Count Low Register */
#define ADDR_FC_LEN_ERR_CNT                                      (0X1F800BU)    /* Frame Checker Length Error Count Register */
#define ADDR_FC_ALGN_ERR_CNT                                     (0X1F800CU)    /* Frame Checker Alignment Error Count Register */
#define ADDR_FC_SYMB_ERR_CNT                                     (0X1F800DU)    /* Frame Checker Symbol Error Count Register */
#define ADDR_FC_OSZ_CNT                                          (0X1F800EU)    /* Frame Checker Oversized Frame Count Register */
#define ADDR_FC_USZ_CNT                                          (0X1F800FU)    /* Frame Checker Undersized Frame Count Register */
#define ADDR_FC_ODD_CNT                                          (0X1F8010U)    /* Frame Checker Odd Nibble Frame Count Register */
#define ADDR_FC_ODD_PRE_CNT                                      (0X1F8011U)    /* Frame Checker Odd Preamble Packet Count Register */
#define ADDR_FC_FALSE_CARRIER_CNT                                (0X1F8013U)    /* Frame Checker False Carrier Count Register */
#define ADDR_FG_EN                                               (0X1F8020U)    /* Frame Generator Enable Register */
#define ADDR_FG_CNTRL_RSTRT                                      (0X1F8021U)    /* Frame Generator CONTROL/_RESTART Register */
#define ADDR_FG_CONT_MODE_EN                                     (0X1F8022U)    /* Frame Generator Continuous Mode Enable Register */
#define ADDR_FG_IRQ_EN                                           (0X1F8023U)    /* Frame Generator Interrupt Enable Register */
#define ADDR_FG_FRM_LEN                                          (0X1F8025U)    /* Frame Generator Frame Length Register */
#define ADDR_FG_IFG_LEN                                          (0X1F8026U)    /* Frame Generator INTER-_FRAME Gap Length Register */
#define ADDR_FG_NFRM_H                                           (0X1F8027U)    /* Frame Generator Number of Frames High Register */
#define ADDR_FG_NFRM_L                                           (0X1F8028U)    /* Frame Generator Number of Frames Low Register */
#define ADDR_FG_DONE                                             (0X1F8029U)    /* Frame Generator Done Register */
#define ADDR_MAC_IF_LOOPBACK                                     (0X1F8055U)    /* MAC Interface Loopbacks Configuration Register */
#define ADDR_MAC_IF_SOP_CNTRL                                    (0X1F805AU)    /* MAC Start Of Packet (SOP) Generation Control Register */

/* ====================================================================================================
        ADIN1110 Module Register ResetValue Definitions
   ==================================================================================================== */
#define RSTVAL_PMA_PMD_CNTRL1                                      (0X0)
#define RSTVAL_PMA_PMD_STAT1                                       (0X2)
#define RSTVAL_PMA_PMD_DEVS_IN_PKG1                                (0X8B)
#define RSTVAL_PMA_PMD_DEVS_IN_PKG2                                (0XC000)
#define RSTVAL_PMA_PMD_CNTRL2                                      (0X3D)
#define RSTVAL_PMA_PMD_STAT2                                       (0X8301)
#define RSTVAL_PMA_PMD_TX_DIS                                      (0X0)
#define RSTVAL_PMA_PMD_EXT_ABILITY                                 (0X800)
#define RSTVAL_PMA_PMD_BT1_ABILITY                                 (0X4)
#define RSTVAL_PMA_PMD_BT1_CONTROL                                 (0X8002)
#define RSTVAL_B10L_PMA_CNTRL                                      (0X0)
#define RSTVAL_B10L_PMA_STAT                                       (0X2800)
#define RSTVAL_B10L_TEST_MODE_CNTRL                                (0X0)
#define RSTVAL_CR_STBL_CHK_FOFFS_SAT_THR                           (0X8)
#define RSTVAL_SLV_FLTR_ECHO_ACQ_CR_KP                             (0X400)
#define RSTVAL_B10L_PMA_LINK_STAT                                  (0X0)
#define RSTVAL_MSE_VAL                                             (0X0)
#define RSTVAL_PCS_CNTRL1                                          (0X0)
#define RSTVAL_PCS_STAT1                                           (0X2)
#define RSTVAL_PCS_DEVS_IN_PKG1                                    (0X8B)
#define RSTVAL_PCS_DEVS_IN_PKG2                                    (0XC000)
#define RSTVAL_PCS_STAT2                                           (0X8000)
#define RSTVAL_B10L_PCS_CNTRL                                      (0X0)
#define RSTVAL_B10L_PCS_STAT                                       (0X0)
#define RSTVAL_AN_DEVS_IN_PKG1                                     (0X8B)
#define RSTVAL_AN_DEVS_IN_PKG2                                     (0XC000)
#define RSTVAL_AN_CONTROL                                          (0X1000)
#define RSTVAL_AN_STATUS                                           (0X8)
#define RSTVAL_AN_ADV_ABILITY_L                                    (0X1)
#define RSTVAL_AN_ADV_ABILITY_M                                    (0X4000)
#define RSTVAL_AN_ADV_ABILITY_H                                    (0X0)
#define RSTVAL_AN_LP_ADV_ABILITY_L                                 (0X0)
#define RSTVAL_AN_LP_ADV_ABILITY_M                                 (0X0)
#define RSTVAL_AN_LP_ADV_ABILITY_H                                 (0X0)
#define RSTVAL_AN_NEXT_PAGE_L                                      (0X2001)
#define RSTVAL_AN_NEXT_PAGE_M                                      (0X0)
#define RSTVAL_AN_NEXT_PAGE_H                                      (0X0)
#define RSTVAL_AN_LP_NEXT_PAGE_L                                   (0X0)
#define RSTVAL_AN_LP_NEXT_PAGE_M                                   (0X0)
#define RSTVAL_AN_LP_NEXT_PAGE_H                                   (0X0)
#define RSTVAL_AN_B10_ADV_ABILITY                                  (0X8000)
#define RSTVAL_AN_B10_LP_ADV_ABILITY                               (0X0)
#define RSTVAL_AN_FRC_MODE_EN                                      (0X0)
#define RSTVAL_AN_STATUS_EXTRA                                     (0X0)
#define RSTVAL_AN_PHY_INST_STATUS                                  (0X10)
#define RSTVAL_MMD1_DEV_ID1                                        (0X283)
#define RSTVAL_MMD1_DEV_ID2                                        (0XBC91)
#define RSTVAL_MMD1_DEVS_IN_PKG1                                   (0X8B)
#define RSTVAL_MMD1_DEVS_IN_PKG2                                   (0XC000)
#define RSTVAL_MMD1_STATUS                                         (0X8000)
#define RSTVAL_CRSM_IRQ_STATUS                                     (0X1000)
#define RSTVAL_CRSM_IRQ_MASK                                       (0X1FFE)
#define RSTVAL_CRSM_SFT_RST                                        (0X0)
#define RSTVAL_CRSM_SFT_PD_CNTRL                                   (0X0)
#define RSTVAL_CRSM_PHY_SUBSYS_RST                                 (0X0)
#define RSTVAL_CRSM_MAC_IF_RST                                     (0X0)
#define RSTVAL_CRSM_STAT                                           (0X0)
#define RSTVAL_CRSM_PMG_CNTRL                                      (0X0)
#define RSTVAL_CRSM_DIAG_CLK_CTRL                                  (0X2)
#define RSTVAL_MGMT_PRT_PKG                                        (0X0)
#define RSTVAL_MGMT_MDIO_CNTRL                                     (0X0)
#define RSTVAL_DIGIO_PINMUX                                        (0XFE)
#define RSTVAL_LED0_BLINK_TIME_CNTRL                               (0X3636)
#define RSTVAL_LED1_BLINK_TIME_CNTRL                               (0X3636)
#define RSTVAL_LED_CNTRL                                           (0X8480)
#define RSTVAL_LED_POLARITY                                        (0X0)
#define RSTVAL_MMD2_DEV_ID1                                        (0X283)
#define RSTVAL_MMD2_DEV_ID2                                        (0XBC91)
#define RSTVAL_MMD2_DEVS_IN_PKG1                                   (0X8B)
#define RSTVAL_MMD2_DEVS_IN_PKG2                                   (0XC000)
#define RSTVAL_MMD2_STATUS                                         (0X8000)
#define RSTVAL_PHY_SUBSYS_IRQ_STATUS                               (0X0)
#define RSTVAL_PHY_SUBSYS_IRQ_MASK                                 (0X2402)
#define RSTVAL_FC_EN                                               (0X1)
#define RSTVAL_FC_IRQ_EN                                           (0X1)
#define RSTVAL_FC_TX_SEL                                           (0X0)
#define RSTVAL_RX_ERR_CNT                                          (0X0)
#define RSTVAL_FC_FRM_CNT_H                                        (0X0)
#define RSTVAL_FC_FRM_CNT_L                                        (0X0)
#define RSTVAL_FC_LEN_ERR_CNT                                      (0X0)
#define RSTVAL_FC_ALGN_ERR_CNT                                     (0X0)
#define RSTVAL_FC_SYMB_ERR_CNT                                     (0X0)
#define RSTVAL_FC_OSZ_CNT                                          (0X0)
#define RSTVAL_FC_USZ_CNT                                          (0X0)
#define RSTVAL_FC_ODD_CNT                                          (0X0)
#define RSTVAL_FC_ODD_PRE_CNT                                      (0X0)
#define RSTVAL_FC_FALSE_CARRIER_CNT                                (0X0)
#define RSTVAL_FG_EN                                               (0X0)
#define RSTVAL_FG_CNTRL_RSTRT                                      (0X1)
#define RSTVAL_FG_CONT_MODE_EN                                     (0X0)
#define RSTVAL_FG_IRQ_EN                                           (0X0)
#define RSTVAL_FG_FRM_LEN                                          (0X6B)
#define RSTVAL_FG_IFG_LEN                                          (0XC)
#define RSTVAL_FG_NFRM_H                                           (0X0)
#define RSTVAL_FG_NFRM_L                                           (0X100)
#define RSTVAL_FG_DONE                                             (0X0)
#define RSTVAL_MAC_IF_LOOPBACK                                     (0XA)
#define RSTVAL_MAC_IF_SOP_CNTRL                                    (0X1B)

/* ====================================================================================================
        ADIN1110 Module Register BitPositions, Lengths, Masks and Enumerations Definitions
   ==================================================================================================== */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_CNTRL1                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_CNTRL1_LB_PMA_LOC_EN                     (0U)           /* Enables PMA Local Loopback. */
#define BITL_PMA_PMD_CNTRL1_LB_PMA_LOC_EN                     (1U)           /* Enables PMA Local Loopback. */
#define BITM_PMA_PMD_CNTRL1_LB_PMA_LOC_EN                     (0X0001U)      /* Enables PMA Local Loopback. */
#define BITP_PMA_PMD_CNTRL1_PMA_SFT_PD                        (11U)          /* PMA Software Power-down. */
#define BITL_PMA_PMD_CNTRL1_PMA_SFT_PD                        (1U)           /* PMA Software Power-down. */
#define BITM_PMA_PMD_CNTRL1_PMA_SFT_PD                        (0X0800U)      /* PMA Software Power-down. */
#define BITP_PMA_PMD_CNTRL1_PMA_SFT_RST                       (15U)          /* PMA Software Reset. */
#define BITL_PMA_PMD_CNTRL1_PMA_SFT_RST                       (1U)           /* PMA Software Reset. */
#define BITM_PMA_PMD_CNTRL1_PMA_SFT_RST                       (0X8000U)      /* PMA Software Reset. */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_STAT1                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_STAT1_PMA_SFT_PD_ABLE                    (1U)           /* PMA Software Powerdown Able */
#define BITL_PMA_PMD_STAT1_PMA_SFT_PD_ABLE                    (1U)           /* PMA Software Powerdown Able */
#define BITM_PMA_PMD_STAT1_PMA_SFT_PD_ABLE                    (0X0002U)      /* PMA Software Powerdown Able */
#define BITP_PMA_PMD_STAT1_PMA_LINK_STAT_OK_LL                (2U)           /* PMA Link Status. */
#define BITL_PMA_PMD_STAT1_PMA_LINK_STAT_OK_LL                (1U)           /* PMA Link Status. */
#define BITM_PMA_PMD_STAT1_PMA_LINK_STAT_OK_LL                (0X0004U)      /* PMA Link Status. */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_DEVS_IN_PKG1                                  Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_DEVS_IN_PKG1_PMA_PMD_DEVS_IN_PKG1        (0U)           /* PMA/PMD MMD Devices in Package */
#define BITL_PMA_PMD_DEVS_IN_PKG1_PMA_PMD_DEVS_IN_PKG1        (16U)          /* PMA/PMD MMD Devices in Package */
#define BITM_PMA_PMD_DEVS_IN_PKG1_PMA_PMD_DEVS_IN_PKG1        (0XFFFFU)      /* PMA/PMD MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_DEVS_IN_PKG2                                  Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_DEVS_IN_PKG2_PMA_PMD_DEVS_IN_PKG2        (0U)           /* PMA/PMD MMD Devices in Package */
#define BITL_PMA_PMD_DEVS_IN_PKG2_PMA_PMD_DEVS_IN_PKG2        (16U)          /* PMA/PMD MMD Devices in Package */
#define BITM_PMA_PMD_DEVS_IN_PKG2_PMA_PMD_DEVS_IN_PKG2        (0XFFFFU)      /* PMA/PMD MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_CNTRL2                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL                  (0U)           /* PMA/PMD Type Selection */
#define BITL_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL                  (7U)           /* PMA/PMD Type Selection */
#define BITM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL                  (0X007FU)      /* PMA/PMD Type Selection */

#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_CX4_PMA_PMD (0X0000U)      /* TS_10GBASE_CX4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_EW_PMA_PMD (0X0001U)      /* TS_10GBASE_EW_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_LW_PMA_PMD (0X0002U)      /* TS_10GBASE_LW_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_SW_PMA_PMD (0X0003U)      /* TS_10GBASE_SW_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_LX4_PMA_PMD (0X0004U)      /* TS_10GBASE_LX4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_ER_PMA_PMD (0X0005U)      /* TS_10GBASE_ER_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_LR_PMA_PMD (0X0006U)      /* TS_10GBASE_LR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_SR_PMA_PMD (0X0007U)      /* TS_10GBASE_SR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_LRM_PMA_PMD (0X0008U)      /* TS_10GBASE_LRM_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_T_PMA (0X0009U)      /* TS_10GBASE_T_PMA */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_KX4_PMA_PMD (0X000AU)      /* TS_10GBASE_KX4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_KR_PMA_PMD (0X000BU)      /* TS_10GBASE_KR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_1000BASE_T_PMA_PMD (0X000CU)      /* TS_1000BASE_T_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_1000BASE_KX_PMA_PMD (0X000DU)      /* TS_1000BASE_KX_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100BASE_TX_PMA_PMD (0X000EU)      /* TS_100BASE_TX_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10BASE_T_PMA_PMD (0X000FU)      /* TS_10BASE_T_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_D1 (0X0010U)      /* TS_10_1GBASE_PRX_D1 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_D2 (0X0011U)      /* TS_10_1GBASE_PRX_D2 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_D3 (0X0012U)      /* TS_10_1GBASE_PRX_D3 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_D1 (0X0013U)      /* TS_10GBASE_PR_D1 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_D2 (0X0014U)      /* TS_10GBASE_PR_D2 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_D3 (0X0015U)      /* TS_10GBASE_PR_D3 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_U1 (0X0016U)      /* TS_10_1GBASE_PRX_U1 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_U2 (0X0017U)      /* TS_10_1GBASE_PRX_U2 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_U3 (0X0018U)      /* TS_10_1GBASE_PRX_U3 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_U1 (0X0019U)      /* TS_10GBASE_PR_U1 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_U3 (0X001AU)      /* TS_10GBASE_PR_U3 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_RESERVED      (0X001BU)      /* TS_RESERVED */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_D4 (0X001CU)      /* TS_10GBASE_PR_D4 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_D4 (0X001DU)      /* TS_10_1GBASE_PRX_D4 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GBASE_PR_U4 (0X001EU)      /* TS_10GBASE_PR_U4 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10_1GBASE_PRX_U4 (0X001FU)      /* TS_10_1GBASE_PRX_U4 */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_KR4_PMA_PMD (0X0020U)      /* TS_40GBASE_KR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_CR4_PMA_PMD (0X0021U)      /* TS_40GBASE_CR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_SR4_PMA_PMD (0X0022U)      /* TS_40GBASE_SR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_LR4_PMA_PMD (0X0023U)      /* TS_40GBASE_LR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_FR_PMA_PMD (0X0024U)      /* TS_40GBASE_FR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_ER4_PMA_PMD (0X0025U)      /* TS_40GBASE_ER4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_40GBASE_T_PMA (0X0026U)      /* TS_40GBASE_T_PMA */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_CR10_PMA_PMD (0X0028U)      /* TS_100GBASE_CR10_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_SR10_PMA_PMD (0X0029U)      /* TS_100GBASE_SR10_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_LR4_PMA_PMD (0X002AU)      /* TS_100GBASE_LR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_ER4_PMA_PMD (0X002BU)      /* TS_100GBASE_ER4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_KP4_PMA_PMD (0X002CU)      /* TS_100GBASE_KP4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_KR4_PMA_PMD (0X002DU)      /* TS_100GBASE_KR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_CR4_PMA_PMD (0X002EU)      /* TS_100GBASE_CR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_100GBASE_SR4_PMA_PMD (0X002FU)      /* TS_100GBASE_SR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_2_5GBASE_T_PMA (0X0030U)      /* TS_2_5GBASE_T_PMA */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_5GBASE_T_PMA  (0X0031U)      /* TS_5GBASE_T_PMA */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GPASS_XR_D_PMA_PMD (0X0032U)      /* TS_10GPASS_XR_D_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_10GPASS_XR_U_PMA_PMD (0X0033U)      /* TS_10GPASS_XR_U_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_BASE_H_PMA_PMD (0X0034U)      /* TS_BASE_H_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_LR_PMA_PMD (0X0035U)      /* TS_25GBASE_LR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_ER_PMA_PMD (0X0036U)      /* TS_25GBASE_ER_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_T_PMA (0X0037U)      /* TS_25GBASE_T_PMA */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_CR_OR_25GBASE_CR_S_PMA_PMD (0X0038U)      /* TS_25GBASE_CR_OR_25GBASE_CR_S_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_KR_OR_25GBASE_KR_S_PMA_PMD (0X0039U)      /* TS_25GBASE_KR_OR_25GBASE_KR_S_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_25GBASE_SR_PMA_PMD (0X003AU)      /* TS_25GBASE_SR_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_BASE_T1_PMA_PMD (0X003DU)      /* TS_BASE_T1_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_200GBASE_DR4_PMA_PMD (0X0053U)      /* TS_200GBASE_DR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_200GBASE_FR4_PMA_PMD (0X0054U)      /* TS_200GBASE_FR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_200GBASE_LR4_PMA_PMD (0X0055U)      /* TS_200GBASE_LR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_400GBASE_SR16_PMA_PMD (0X0059U)      /* TS_400GBASE_SR16_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_400GBASE_DR4_PMA_PMD (0X005AU)      /* TS_400GBASE_DR4_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_400GBASE_FR8_PMA_PMD (0X005BU)      /* TS_400GBASE_FR8_PMA_PMD */
#define ENUM_PMA_PMD_CNTRL2_PMA_PMD_TYPE_SEL_TS_400GBASE_LR8_PMA_PMD (0X005CU)      /* TS_400GBASE_LR8_PMA_PMD */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_STAT2                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_STAT2_LB_PMA_LOC_ABLE                    (0U)           /* PMA Local Loopback Able */
#define BITL_PMA_PMD_STAT2_LB_PMA_LOC_ABLE                    (1U)           /* PMA Local Loopback Able */
#define BITM_PMA_PMD_STAT2_LB_PMA_LOC_ABLE                    (0X0001U)      /* PMA Local Loopback Able */
#define BITP_PMA_PMD_STAT2_PMA_PMD_TX_DIS_ABLE                (8U)           /* PMA/PMD Tx Disable */
#define BITL_PMA_PMD_STAT2_PMA_PMD_TX_DIS_ABLE                (1U)           /* PMA/PMD Tx Disable */
#define BITM_PMA_PMD_STAT2_PMA_PMD_TX_DIS_ABLE                (0X0100U)      /* PMA/PMD Tx Disable */
#define BITP_PMA_PMD_STAT2_PMA_PMD_EXT_ABLE                   (9U)           /* PHY Extended Abilities Support */
#define BITL_PMA_PMD_STAT2_PMA_PMD_EXT_ABLE                   (1U)           /* PHY Extended Abilities Support */
#define BITM_PMA_PMD_STAT2_PMA_PMD_EXT_ABLE                   (0X0200U)      /* PHY Extended Abilities Support */
#define BITP_PMA_PMD_STAT2_PMA_PMD_PRESENT                    (14U)          /* PMA/PMD Present */
#define BITL_PMA_PMD_STAT2_PMA_PMD_PRESENT                    (2U)           /* PMA/PMD Present */
#define BITM_PMA_PMD_STAT2_PMA_PMD_PRESENT                    (0XC000U)      /* PMA/PMD Present */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_TX_DIS                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_TX_DIS_PMA_TX_DIS                        (0U)           /* PMD Transmit Disable. */
#define BITL_PMA_PMD_TX_DIS_PMA_TX_DIS                        (1U)           /* PMD Transmit Disable. */
#define BITM_PMA_PMD_TX_DIS_PMA_TX_DIS                        (0X0001U)      /* PMD Transmit Disable. */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_EXT_ABILITY                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_EXT_ABILITY_PMA_PMD_BT1_ABLE             (11U)          /* PHY Supports BASE-T1 */
#define BITL_PMA_PMD_EXT_ABILITY_PMA_PMD_BT1_ABLE             (1U)           /* PHY Supports BASE-T1 */
#define BITM_PMA_PMD_EXT_ABILITY_PMA_PMD_BT1_ABLE             (0X0800U)      /* PHY Supports BASE-T1 */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_BT1_ABILITY                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_BT1_ABILITY_B100_ABILITY                 (0U)           /* 100BASE-T1 Ability. */
#define BITL_PMA_PMD_BT1_ABILITY_B100_ABILITY                 (1U)           /* 100BASE-T1 Ability. */
#define BITM_PMA_PMD_BT1_ABILITY_B100_ABILITY                 (0X0001U)      /* 100BASE-T1 Ability. */
#define BITP_PMA_PMD_BT1_ABILITY_B1000_ABILITY                (1U)           /* 1000BASE-T1 Ability. */
#define BITL_PMA_PMD_BT1_ABILITY_B1000_ABILITY                (1U)           /* 1000BASE-T1 Ability. */
#define BITM_PMA_PMD_BT1_ABILITY_B1000_ABILITY                (0X0002U)      /* 1000BASE-T1 Ability. */
#define BITP_PMA_PMD_BT1_ABILITY_B10L_ABILITY                 (2U)           /* 10BASE-T1L Ability. */
#define BITL_PMA_PMD_BT1_ABILITY_B10L_ABILITY                 (1U)           /* 10BASE-T1L Ability. */
#define BITM_PMA_PMD_BT1_ABILITY_B10L_ABILITY                 (0X0004U)      /* 10BASE-T1L Ability. */
#define BITP_PMA_PMD_BT1_ABILITY_B10S_ABILITY                 (3U)           /* 10BASE-T1S Ability. */
#define BITL_PMA_PMD_BT1_ABILITY_B10S_ABILITY                 (1U)           /* 10BASE-T1S Ability. */
#define BITM_PMA_PMD_BT1_ABILITY_B10S_ABILITY                 (0X0008U)      /* 10BASE-T1S Ability. */

/* ----------------------------------------------------------------------------------------------------
          PMA_PMD_BT1_CONTROL                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PMA_PMD_BT1_CONTROL_BT1_TYPE_SEL                 (0U)           /* BASE-T1 Type Selection */
#define BITL_PMA_PMD_BT1_CONTROL_BT1_TYPE_SEL                 (4U)           /* BASE-T1 Type Selection */
#define BITM_PMA_PMD_BT1_CONTROL_BT1_TYPE_SEL                 (0X000FU)      /* BASE-T1 Type Selection */
#define BITP_PMA_PMD_BT1_CONTROL_CFG_MST                      (14U)          /* Master-slave Config. */
#define BITL_PMA_PMD_BT1_CONTROL_CFG_MST                      (1U)           /* Master-slave Config. */
#define BITM_PMA_PMD_BT1_CONTROL_CFG_MST                      (0X4000U)      /* Master-slave Config. */

#define ENUM_PMA_PMD_BT1_CONTROL_BT1_TYPE_SEL_TS_10BASE_T1L   (0X0002U)      /* 10BASE-T1L */

/* ----------------------------------------------------------------------------------------------------
          B10L_PMA_CNTRL                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN                (0U)           /* 10BASE-T1L PMA Loopback. */
#define BITL_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN                (1U)           /* 10BASE-T1L PMA Loopback. */
#define BITM_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN                (0X0001U)      /* 10BASE-T1L PMA Loopback. */
#define BITP_B10L_PMA_CNTRL_B10L_EEE                          (10U)          /* 10BASE-T1L EEE Enable. */
#define BITL_B10L_PMA_CNTRL_B10L_EEE                          (1U)           /* 10BASE-T1L EEE Enable. */
#define BITM_B10L_PMA_CNTRL_B10L_EEE                          (0X0400U)      /* 10BASE-T1L EEE Enable. */
#define BITP_B10L_PMA_CNTRL_B10L_TX_LVL_HI                    (12U)          /* 10BASE-T1L Transmit Voltage Amplitude Control. */
#define BITL_B10L_PMA_CNTRL_B10L_TX_LVL_HI                    (1U)           /* 10BASE-T1L Transmit Voltage Amplitude Control. */
#define BITM_B10L_PMA_CNTRL_B10L_TX_LVL_HI                    (0X1000U)      /* 10BASE-T1L Transmit Voltage Amplitude Control. */
#define BITP_B10L_PMA_CNTRL_B10L_TX_DIS_MODE_EN               (14U)          /* 10BASE-T1L Transmit Disable Mode. */
#define BITL_B10L_PMA_CNTRL_B10L_TX_DIS_MODE_EN               (1U)           /* 10BASE-T1L Transmit Disable Mode. */
#define BITM_B10L_PMA_CNTRL_B10L_TX_DIS_MODE_EN               (0X4000U)      /* 10BASE-T1L Transmit Disable Mode. */

/* ----------------------------------------------------------------------------------------------------
          B10L_PMA_STAT                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_PMA_STAT_B10L_EEE_ABLE                      (10U)          /* 10BASE-T1L EEE Ability */
#define BITL_B10L_PMA_STAT_B10L_EEE_ABLE                      (1U)           /* 10BASE-T1L EEE Ability */
#define BITM_B10L_PMA_STAT_B10L_EEE_ABLE                      (0X0400U)      /* 10BASE-T1L EEE Ability */
#define BITP_B10L_PMA_STAT_B10L_PMA_SFT_PD_ABLE               (11U)          /* PMA Supports Powerdown */
#define BITL_B10L_PMA_STAT_B10L_PMA_SFT_PD_ABLE               (1U)           /* PMA Supports Powerdown */
#define BITM_B10L_PMA_STAT_B10L_PMA_SFT_PD_ABLE               (0X0800U)      /* PMA Supports Powerdown */
#define BITP_B10L_PMA_STAT_B10L_TX_LVL_HI_ABLE                (12U)          /* 10BASE-T1L High Voltage Tx Ability */
#define BITL_B10L_PMA_STAT_B10L_TX_LVL_HI_ABLE                (1U)           /* 10BASE-T1L High Voltage Tx Ability */
#define BITM_B10L_PMA_STAT_B10L_TX_LVL_HI_ABLE                (0X1000U)      /* 10BASE-T1L High Voltage Tx Ability */
#define BITP_B10L_PMA_STAT_B10L_LB_PMA_LOC_ABLE               (13U)          /* 10BASE-T1L PMA Loopback Ability */
#define BITL_B10L_PMA_STAT_B10L_LB_PMA_LOC_ABLE               (1U)           /* 10BASE-T1L PMA Loopback Ability */
#define BITM_B10L_PMA_STAT_B10L_LB_PMA_LOC_ABLE               (0X2000U)      /* 10BASE-T1L PMA Loopback Ability */

/* ----------------------------------------------------------------------------------------------------
          B10L_TEST_MODE_CNTRL                                  Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE           (13U)          /* 10BASE-T1L Transmitter Test Mode. */
#define BITL_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE           (3U)           /* 10BASE-T1L Transmitter Test Mode. */
#define BITM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE           (0XE000U)      /* 10BASE-T1L Transmitter Test Mode. */

#define ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_NONE (0X0000U)      /* Normal operation. */
#define ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_JITTER (0X0001U)      /* Test mode 1 - Transmitter output voltage and timing jitter test mode.  When test mode 1 is enabled, the PHY shall repeatedly transmit the data symbol sequence (+1, -1). */
#define ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_DROOP (0X0002U)      /* Test mode 2 - Transmitter output droop test mode.  When test mode 2 is enabled, the PHY shall transmit ten "+1" symbols followed by ten "-1" symbols. */
#define ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_IDLE (0X0003U)      /* Test mode 3 - Normal operation in Idle mode.  When test mode 3 is enabled, the PHY shall transmit as in non-test operation and in the MASTER data mode with data set to normal Inter-Frame idle signals. */

/* ----------------------------------------------------------------------------------------------------
          CR_STBL_CHK_FOFFS_SAT_THR                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CR_STBL_CHK_FOFFS_SAT_THR_CR_STBL_CHK_FOFFS_SAT_THR (0U)           /* Frequency Offset Saturation Threshold for CR Stability Check. */
#define BITL_CR_STBL_CHK_FOFFS_SAT_THR_CR_STBL_CHK_FOFFS_SAT_THR (11U)          /* Frequency Offset Saturation Threshold for CR Stability Check. */
#define BITM_CR_STBL_CHK_FOFFS_SAT_THR_CR_STBL_CHK_FOFFS_SAT_THR (0X07FFU)      /* Frequency Offset Saturation Threshold for CR Stability Check. */

/* ----------------------------------------------------------------------------------------------------
          SLV_FLTR_ECHO_ACQ_CR_KP                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_SLV_FLTR_ECHO_ACQ_CR_KP_SLV_FLTR_ECHO_ACQ_CR_KP  (0U)           /* Slave IIR Filter Change Echo Acquisition Clock Recovery Proportional Gain. */
#define BITL_SLV_FLTR_ECHO_ACQ_CR_KP_SLV_FLTR_ECHO_ACQ_CR_KP  (16U)          /* Slave IIR Filter Change Echo Acquisition Clock Recovery Proportional Gain. */
#define BITM_SLV_FLTR_ECHO_ACQ_CR_KP_SLV_FLTR_ECHO_ACQ_CR_KP  (0XFFFFU)      /* Slave IIR Filter Change Echo Acquisition Clock Recovery Proportional Gain. */

/* ----------------------------------------------------------------------------------------------------
          B10L_PMA_LINK_STAT                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK             (0U)           /* Link Status OK */
#define BITL_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK             (1U)           /* Link Status OK */
#define BITM_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK             (0X0001U)      /* Link Status OK */
#define BITP_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK_LL          (1U)           /* Link Status Ok Latch Low */
#define BITL_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK_LL          (1U)           /* Link Status Ok Latch Low */
#define BITM_B10L_PMA_LINK_STAT_B10L_LINK_STAT_OK_LL          (0X0002U)      /* Link Status Ok Latch Low */
#define BITP_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK             (4U)           /* 10BASE-T1L Descrambler Status Ok */
#define BITL_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK             (1U)           /* 10BASE-T1L Descrambler Status Ok */
#define BITM_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK             (0X0010U)      /* 10BASE-T1L Descrambler Status Ok */
#define BITP_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK_LL          (5U)           /* BASE-T1L Descrambler Status Ok Latch Low */
#define BITL_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK_LL          (1U)           /* BASE-T1L Descrambler Status Ok Latch Low */
#define BITM_B10L_PMA_LINK_STAT_B10L_DSCR_STAT_OK_LL          (0X0020U)      /* BASE-T1L Descrambler Status Ok Latch Low */
#define BITP_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK         (6U)           /* 10BASE-T1L Local Receiver Status Ok */
#define BITL_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK         (1U)           /* 10BASE-T1L Local Receiver Status Ok */
#define BITM_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK         (0X0040U)      /* 10BASE-T1L Local Receiver Status Ok */
#define BITP_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK_LL      (7U)           /* 10BASE-T1L Local Receiver Status Ok Latch Low */
#define BITL_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK_LL      (1U)           /* 10BASE-T1L Local Receiver Status Ok Latch Low */
#define BITM_B10L_PMA_LINK_STAT_B10L_LOC_RCVR_STAT_OK_LL      (0X0080U)      /* 10BASE-T1L Local Receiver Status Ok Latch Low */
#define BITP_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK         (8U)           /* 10BASE-T1L Remote Receiver Status Ok */
#define BITL_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK         (1U)           /* 10BASE-T1L Remote Receiver Status Ok */
#define BITM_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK         (0X0100U)      /* 10BASE-T1L Remote Receiver Status Ok */
#define BITP_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK_LL      (9U)           /* 10BASE-T1L Remote Receiver Status Ok Latch Low */
#define BITL_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK_LL      (1U)           /* 10BASE-T1L Remote Receiver Status Ok Latch Low */
#define BITM_B10L_PMA_LINK_STAT_B10L_REM_RCVR_STAT_OK_LL      (0X0200U)      /* 10BASE-T1L Remote Receiver Status Ok Latch Low */

/* ----------------------------------------------------------------------------------------------------
          MSE_VAL                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MSE_VAL_MSE_VAL                                  (0U)           /* MSE Value */
#define BITL_MSE_VAL_MSE_VAL                                  (16U)          /* MSE Value */
#define BITM_MSE_VAL_MSE_VAL                                  (0XFFFFU)      /* MSE Value */

/* ----------------------------------------------------------------------------------------------------
          PCS_CNTRL1                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PCS_CNTRL1_PCS_SFT_PD                            (11U)          /* PCS Software Power-down. */
#define BITL_PCS_CNTRL1_PCS_SFT_PD                            (1U)           /* PCS Software Power-down. */
#define BITM_PCS_CNTRL1_PCS_SFT_PD                            (0X0800U)      /* PCS Software Power-down. */
#define BITP_PCS_CNTRL1_LB_PCS_EN                             (14U)          /* PCS Loopback Enable */
#define BITL_PCS_CNTRL1_LB_PCS_EN                             (1U)           /* PCS Loopback Enable */
#define BITM_PCS_CNTRL1_LB_PCS_EN                             (0X4000U)      /* PCS Loopback Enable */
#define BITP_PCS_CNTRL1_PCS_SFT_RST                           (15U)          /* PCS Software Reset. */
#define BITL_PCS_CNTRL1_PCS_SFT_RST                           (1U)           /* PCS Software Reset. */
#define BITM_PCS_CNTRL1_PCS_SFT_RST                           (0X8000U)      /* PCS Software Reset. */

/* ----------------------------------------------------------------------------------------------------
          PCS_STAT1                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PCS_STAT1_PCS_SFT_PD_ABLE                        (1U)           /* PCS Software Powerdown Able */
#define BITL_PCS_STAT1_PCS_SFT_PD_ABLE                        (1U)           /* PCS Software Powerdown Able */
#define BITM_PCS_STAT1_PCS_SFT_PD_ABLE                        (0X0002U)      /* PCS Software Powerdown Able */

/* ----------------------------------------------------------------------------------------------------
          PCS_DEVS_IN_PKG1                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PCS_DEVS_IN_PKG1_PCS_DEVS_IN_PKG1                (0U)           /* PCS MMD Devices in Package */
#define BITL_PCS_DEVS_IN_PKG1_PCS_DEVS_IN_PKG1                (16U)          /* PCS MMD Devices in Package */
#define BITM_PCS_DEVS_IN_PKG1_PCS_DEVS_IN_PKG1                (0XFFFFU)      /* PCS MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          PCS_DEVS_IN_PKG2                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PCS_DEVS_IN_PKG2_PCS_DEVS_IN_PKG2                (0U)           /* PCS MMD Devices in Package */
#define BITL_PCS_DEVS_IN_PKG2_PCS_DEVS_IN_PKG2                (16U)          /* PCS MMD Devices in Package */
#define BITM_PCS_DEVS_IN_PKG2_PCS_DEVS_IN_PKG2                (0XFFFFU)      /* PCS MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          PCS_STAT2                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PCS_STAT2_PCS_PRESENT                            (14U)          /* PCS Present */
#define BITL_PCS_STAT2_PCS_PRESENT                            (2U)           /* PCS Present */
#define BITM_PCS_STAT2_PCS_PRESENT                            (0XC000U)      /* PCS Present */

/* ----------------------------------------------------------------------------------------------------
          B10L_PCS_CNTRL                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_PCS_CNTRL_B10L_LB_PCS_EN                    (14U)          /* PCS Loopback Enable */
#define BITL_B10L_PCS_CNTRL_B10L_LB_PCS_EN                    (1U)           /* PCS Loopback Enable */
#define BITM_B10L_PCS_CNTRL_B10L_LB_PCS_EN                    (0X4000U)      /* PCS Loopback Enable */

/* ----------------------------------------------------------------------------------------------------
          B10L_PCS_STAT                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_B10L_PCS_STAT_B10L_PCS_DSCR_STAT_OK_LL           (2U)           /* PCS Descrambler Status. */
#define BITL_B10L_PCS_STAT_B10L_PCS_DSCR_STAT_OK_LL           (1U)           /* PCS Descrambler Status. */
#define BITM_B10L_PCS_STAT_B10L_PCS_DSCR_STAT_OK_LL           (0X0004U)      /* PCS Descrambler Status. */

/* ----------------------------------------------------------------------------------------------------
          AN_DEVS_IN_PKG1                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_DEVS_IN_PKG1_AN_DEVS_IN_PKG1                  (0U)           /* Autonegotiation MMD Devices in Package */
#define BITL_AN_DEVS_IN_PKG1_AN_DEVS_IN_PKG1                  (16U)          /* Autonegotiation MMD Devices in Package */
#define BITM_AN_DEVS_IN_PKG1_AN_DEVS_IN_PKG1                  (0XFFFFU)      /* Autonegotiation MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          AN_DEVS_IN_PKG2                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_DEVS_IN_PKG2_AN_DEVS_IN_PKG2                  (0U)           /* Autonegotiation MMD Devices in Package */
#define BITL_AN_DEVS_IN_PKG2_AN_DEVS_IN_PKG2                  (16U)          /* Autonegotiation MMD Devices in Package */
#define BITM_AN_DEVS_IN_PKG2_AN_DEVS_IN_PKG2                  (0XFFFFU)      /* Autonegotiation MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          AN_CONTROL                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_CONTROL_AN_RESTART                            (9U)           /* Autonegotiation Restart */
#define BITL_AN_CONTROL_AN_RESTART                            (1U)           /* Autonegotiation Restart */
#define BITM_AN_CONTROL_AN_RESTART                            (0X0200U)      /* Autonegotiation Restart */
#define BITP_AN_CONTROL_AN_EN                                 (12U)          /* Autonegotiation Enable. */
#define BITL_AN_CONTROL_AN_EN                                 (1U)           /* Autonegotiation Enable. */
#define BITM_AN_CONTROL_AN_EN                                 (0X1000U)      /* Autonegotiation Enable. */

/* ----------------------------------------------------------------------------------------------------
          AN_STATUS                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_STATUS_AN_LINK_STATUS                         (2U)           /* Link Status. */
#define BITL_AN_STATUS_AN_LINK_STATUS                         (1U)           /* Link Status. */
#define BITM_AN_STATUS_AN_LINK_STATUS                         (0X0004U)      /* Link Status. */
#define BITP_AN_STATUS_AN_ABLE                                (3U)           /* Autonegotiation Ability. */
#define BITL_AN_STATUS_AN_ABLE                                (1U)           /* Autonegotiation Ability. */
#define BITM_AN_STATUS_AN_ABLE                                (0X0008U)      /* Autonegotiation Ability. */
#define BITP_AN_STATUS_AN_REMOTE_FAULT                        (4U)           /* Autonegotiation Remote Fault */
#define BITL_AN_STATUS_AN_REMOTE_FAULT                        (1U)           /* Autonegotiation Remote Fault */
#define BITM_AN_STATUS_AN_REMOTE_FAULT                        (0X0010U)      /* Autonegotiation Remote Fault */
#define BITP_AN_STATUS_AN_COMPLETE                            (5U)           /* Autonegotiation Complete. */
#define BITL_AN_STATUS_AN_COMPLETE                            (1U)           /* Autonegotiation Complete. */
#define BITM_AN_STATUS_AN_COMPLETE                            (0X0020U)      /* Autonegotiation Complete. */
#define BITP_AN_STATUS_AN_PAGE_RX                             (6U)           /* Page Received */
#define BITL_AN_STATUS_AN_PAGE_RX                             (1U)           /* Page Received */
#define BITM_AN_STATUS_AN_PAGE_RX                             (0X0040U)      /* Page Received */

/* ----------------------------------------------------------------------------------------------------
          AN_ADV_ABILITY_L                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_SELECTOR                 (0U)           /* Selector. */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_SELECTOR                 (5U)           /* Selector. */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_SELECTOR                 (0X001FU)      /* Selector. */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_PAUSE                    (10U)          /* Pause Ability. */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_PAUSE                    (2U)           /* Pause Ability. */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_PAUSE                    (0X0C00U)      /* Pause Ability. */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_FORCE_MS                 (12U)          /* Force Master/slave Configuration. */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_FORCE_MS                 (1U)           /* Force Master/slave Configuration. */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_FORCE_MS                 (0X1000U)      /* Force Master/slave Configuration. */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_REMOTE_FAULT             (13U)          /* Remote Fault. */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_REMOTE_FAULT             (1U)           /* Remote Fault. */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_REMOTE_FAULT             (0X2000U)      /* Remote Fault. */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_ACK                      (14U)          /* Acknowledge (ACK). */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_ACK                      (1U)           /* Acknowledge (ACK). */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_ACK                      (0X4000U)      /* Acknowledge (ACK). */
#define BITP_AN_ADV_ABILITY_L_AN_ADV_NEXT_PAGE_REQ            (15U)          /* Next Page Request. */
#define BITL_AN_ADV_ABILITY_L_AN_ADV_NEXT_PAGE_REQ            (1U)           /* Next Page Request. */
#define BITM_AN_ADV_ABILITY_L_AN_ADV_NEXT_PAGE_REQ            (0X8000U)      /* Next Page Request. */

/* ----------------------------------------------------------------------------------------------------
          AN_ADV_ABILITY_M                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_ADV_ABILITY_M_AN_ADV_MST                      (4U)           /* Master/slave Configuration. */
#define BITL_AN_ADV_ABILITY_M_AN_ADV_MST                      (1U)           /* Master/slave Configuration. */
#define BITM_AN_ADV_ABILITY_M_AN_ADV_MST                      (0X0010U)      /* Master/slave Configuration. */
#define BITP_AN_ADV_ABILITY_M_AN_ADV_B10L                     (14U)          /* 10BASE-T1L Ability. */
#define BITL_AN_ADV_ABILITY_M_AN_ADV_B10L                     (1U)           /* 10BASE-T1L Ability. */
#define BITM_AN_ADV_ABILITY_M_AN_ADV_B10L                     (0X4000U)      /* 10BASE-T1L Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_ADV_ABILITY_H                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_REQ       (12U)          /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITL_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_REQ       (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITM_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_REQ       (0X1000U)      /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITP_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_ABL       (13U)          /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITL_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_ABL       (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITM_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_ABL       (0X2000U)      /* 10BASE-T1L High Level Transmit Operating Mode Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_ADV_ABILITY_L                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_SELECTOR           (0U)           /* Link Partner Selector. */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_SELECTOR           (5U)           /* Link Partner Selector. */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_SELECTOR           (0X001FU)      /* Link Partner Selector. */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_PAUSE              (10U)          /* Link Partner Pause Ability. */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_PAUSE              (2U)           /* Link Partner Pause Ability. */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_PAUSE              (0X0C00U)      /* Link Partner Pause Ability. */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_FORCE_MS           (12U)          /* Link Partner Force Master/slave Configuration. */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_FORCE_MS           (1U)           /* Link Partner Force Master/slave Configuration. */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_FORCE_MS           (0X1000U)      /* Link Partner Force Master/slave Configuration. */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_REMOTE_FAULT       (13U)          /* Link Partner Remote Fault. */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_REMOTE_FAULT       (1U)           /* Link Partner Remote Fault. */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_REMOTE_FAULT       (0X2000U)      /* Link Partner Remote Fault. */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_ACK                (14U)          /* Link Partner Acknowledge (ACK). */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_ACK                (1U)           /* Link Partner Acknowledge (ACK). */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_ACK                (0X4000U)      /* Link Partner Acknowledge (ACK). */
#define BITP_AN_LP_ADV_ABILITY_L_AN_LP_ADV_NEXT_PAGE_REQ      (15U)          /* Link Partner Next Page Request. */
#define BITL_AN_LP_ADV_ABILITY_L_AN_LP_ADV_NEXT_PAGE_REQ      (1U)           /* Link Partner Next Page Request. */
#define BITM_AN_LP_ADV_ABILITY_L_AN_LP_ADV_NEXT_PAGE_REQ      (0X8000U)      /* Link Partner Next Page Request. */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_ADV_ABILITY_M                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_ADV_ABILITY_M_AN_LP_ADV_MST                (4U)           /* Link Partner MASTER/_SLAVE Configuration. */
#define BITL_AN_LP_ADV_ABILITY_M_AN_LP_ADV_MST                (1U)           /* Link Partner MASTER/_SLAVE Configuration. */
#define BITM_AN_LP_ADV_ABILITY_M_AN_LP_ADV_MST                (0X0010U)      /* Link Partner MASTER/_SLAVE Configuration. */
#define BITP_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B100               (5U)           /* Link Partner 100BASE-T1 Ability. */
#define BITL_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B100               (1U)           /* Link Partner 100BASE-T1 Ability. */
#define BITM_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B100               (0X0020U)      /* Link Partner 100BASE-T1 Ability. */
#define BITP_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10S_FD            (6U)           /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITL_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10S_FD            (1U)           /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITM_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10S_FD            (0X0040U)      /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITP_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B1000              (7U)           /* Link Partner 1000BASE-T1 Ability. */
#define BITL_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B1000              (1U)           /* Link Partner 1000BASE-T1 Ability. */
#define BITM_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B1000              (0X0080U)      /* Link Partner 1000BASE-T1 Ability. */
#define BITP_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10L               (14U)          /* Link Partner 10BASE-T1L Ability. */
#define BITL_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10L               (1U)           /* Link Partner 10BASE-T1L Ability. */
#define BITM_AN_LP_ADV_ABILITY_M_AN_LP_ADV_B10L               (0X4000U)      /* Link Partner 10BASE-T1L Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_ADV_ABILITY_H                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10S_HD            (11U)          /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITL_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10S_HD            (1U)           /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITM_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10S_HD            (0X0800U)      /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITP_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_REQ (12U)          /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITL_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_REQ (1U)           /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITM_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_REQ (0X1000U)      /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITP_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_ABL (13U)          /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITL_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_ABL (1U)           /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITM_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_TX_LVL_HI_ABL (0X2000U)      /* Link Partner 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITP_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_EEE           (14U)          /* Link Partner 10BASE-T1L EEE Ability. */
#define BITL_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_EEE           (1U)           /* Link Partner 10BASE-T1L EEE Ability. */
#define BITM_AN_LP_ADV_ABILITY_H_AN_LP_ADV_B10L_EEE           (0X4000U)      /* Link Partner 10BASE-T1L EEE Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_NEXT_PAGE_L                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE                (0U)           /* Message/unformatted Code Field. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE                (11U)          /* Message/unformatted Code Field. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE                (0X07FFU)      /* Message/unformatted Code Field. */
#define BITP_AN_NEXT_PAGE_L_AN_NP_TOGGLE                      (11U)          /* Toggle Bit. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_TOGGLE                      (1U)           /* Toggle Bit. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_TOGGLE                      (0X0800U)      /* Toggle Bit. */
#define BITP_AN_NEXT_PAGE_L_AN_NP_ACK2                        (12U)          /* Acknowledge 2. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_ACK2                        (1U)           /* Acknowledge 2. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_ACK2                        (0X1000U)      /* Acknowledge 2. */
#define BITP_AN_NEXT_PAGE_L_AN_NP_MESSAGE_PAGE                (13U)          /* Next Page Encoding. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_MESSAGE_PAGE                (1U)           /* Next Page Encoding. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_PAGE                (0X2000U)      /* Next Page Encoding. */
#define BITP_AN_NEXT_PAGE_L_AN_NP_ACK                         (14U)          /* Next Page Acknowledge. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_ACK                         (1U)           /* Next Page Acknowledge. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_ACK                         (0X4000U)      /* Next Page Acknowledge. */
#define BITP_AN_NEXT_PAGE_L_AN_NP_NEXT_PAGE_REQ               (15U)          /* Next Page Request. */
#define BITL_AN_NEXT_PAGE_L_AN_NP_NEXT_PAGE_REQ               (1U)           /* Next Page Request. */
#define BITM_AN_NEXT_PAGE_L_AN_NP_NEXT_PAGE_REQ               (0X8000U)      /* Next Page Request. */

#define ENUM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_PAGE_NP_UNFORMATTED_NEXT_PAGE (0X0000U)      /* Unformatted next page */
#define ENUM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_PAGE_NP_MESSAGE_NEXT_PAGE (0X0001U)      /* Message next page */
#define ENUM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE_NP_MESSAGE_NULL (0X0001U)      /* Null Message */
#define ENUM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE_NP_MESSAGE_OUI (0X0005U)      /* Organizationally Unique Identifier Tagged Message */
#define ENUM_AN_NEXT_PAGE_L_AN_NP_MESSAGE_CODE_NP_MESSAGE_AN_DEV_ID (0X0006U)      /* AN Device Identifier Tag Code */

/* ----------------------------------------------------------------------------------------------------
          AN_NEXT_PAGE_M                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_NEXT_PAGE_M_AN_NP_UNFORMATTED1                (0U)           /* Unformatted Code Field 1 */
#define BITL_AN_NEXT_PAGE_M_AN_NP_UNFORMATTED1                (16U)          /* Unformatted Code Field 1 */
#define BITM_AN_NEXT_PAGE_M_AN_NP_UNFORMATTED1                (0XFFFFU)      /* Unformatted Code Field 1 */

/* ----------------------------------------------------------------------------------------------------
          AN_NEXT_PAGE_H                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_NEXT_PAGE_H_AN_NP_UNFORMATTED2                (0U)           /* Unformatted Code Field 2 */
#define BITL_AN_NEXT_PAGE_H_AN_NP_UNFORMATTED2                (16U)          /* Unformatted Code Field 2 */
#define BITM_AN_NEXT_PAGE_H_AN_NP_UNFORMATTED2                (0XFFFFU)      /* Unformatted Code Field 2 */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_NEXT_PAGE_L                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE          (0U)           /* Link Partner Message/unformatted Code Field. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE          (11U)          /* Link Partner Message/unformatted Code Field. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE          (0X07FFU)      /* Link Partner Message/unformatted Code Field. */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_TOGGLE                (11U)          /* Link Partner Toggle Bit. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_TOGGLE                (1U)           /* Link Partner Toggle Bit. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_TOGGLE                (0X0800U)      /* Link Partner Toggle Bit. */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK2                  (12U)          /* Link Partner Acknowledge 2. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK2                  (1U)           /* Link Partner Acknowledge 2. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK2                  (0X1000U)      /* Link Partner Acknowledge 2. */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_PAGE          (13U)          /* Link Partner Next Page Encoding. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_PAGE          (1U)           /* Link Partner Next Page Encoding. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_PAGE          (0X2000U)      /* Link Partner Next Page Encoding. */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK                   (14U)          /* Link Partner Next Page Acknowledge. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK                   (1U)           /* Link Partner Next Page Acknowledge. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_ACK                   (0X4000U)      /* Link Partner Next Page Acknowledge. */
#define BITP_AN_LP_NEXT_PAGE_L_AN_LP_NP_NEXT_PAGE_REQ         (15U)          /* Next Page Request. */
#define BITL_AN_LP_NEXT_PAGE_L_AN_LP_NP_NEXT_PAGE_REQ         (1U)           /* Next Page Request. */
#define BITM_AN_LP_NEXT_PAGE_L_AN_LP_NP_NEXT_PAGE_REQ         (0X8000U)      /* Next Page Request. */

#define ENUM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_PAGE_NP_UNFORMATTED_NEXT_PAGE (0X0000U)      /* Unformatted next page */
#define ENUM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_PAGE_NP_MESSAGE_NEXT_PAGE (0X0001U)      /* Message next page */
#define ENUM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE_NP_MESSAGE_NULL (0X0001U)      /* Null Message */
#define ENUM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE_NP_MESSAGE_OUI (0X0005U)      /* Organizationally Unique Identifier Tagged Message */
#define ENUM_AN_LP_NEXT_PAGE_L_AN_LP_NP_MESSAGE_CODE_NP_MESSAGE_AN_DEV_ID (0X0006U)      /* AN Device Identifier Tag Code */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_NEXT_PAGE_M                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_NEXT_PAGE_M_AN_LP_NP_UNFORMATTED1          (0U)           /* Link Partner Unformatted Code Field 1 */
#define BITL_AN_LP_NEXT_PAGE_M_AN_LP_NP_UNFORMATTED1          (16U)          /* Link Partner Unformatted Code Field 1 */
#define BITM_AN_LP_NEXT_PAGE_M_AN_LP_NP_UNFORMATTED1          (0XFFFFU)      /* Link Partner Unformatted Code Field 1 */

/* ----------------------------------------------------------------------------------------------------
          AN_LP_NEXT_PAGE_H                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_LP_NEXT_PAGE_H_AN_LP_NP_UNFORMATTED2          (0U)           /* Link Partner Unformatted Code Field 2 */
#define BITL_AN_LP_NEXT_PAGE_H_AN_LP_NP_UNFORMATTED2          (16U)          /* Link Partner Unformatted Code Field 2 */
#define BITM_AN_LP_NEXT_PAGE_H_AN_LP_NP_UNFORMATTED2          (0XFFFFU)      /* Link Partner Unformatted Code Field 2 */

/* ----------------------------------------------------------------------------------------------------
          AN_B10_ADV_ABILITY                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_REQ (12U)          /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITL_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_REQ (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITM_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_REQ (0X1000U)      /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITP_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_ABL (13U)          /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITL_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_ABL (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITM_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_TX_LVL_HI_ABL (0X2000U)      /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITP_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_EEE           (14U)          /* 10BASE-T1L EEE Ability. */
#define BITL_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_EEE           (1U)           /* 10BASE-T1L EEE Ability. */
#define BITM_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L_EEE           (0X4000U)      /* 10BASE-T1L EEE Ability. */
#define BITP_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L               (15U)          /* 10BASE-T1L Ability. */
#define BITL_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L               (1U)           /* 10BASE-T1L Ability. */
#define BITM_AN_B10_ADV_ABILITY_AN_B10_ADV_B10L               (0X8000U)      /* 10BASE-T1L Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_B10_LP_ADV_ABILITY                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_HD      (6U)           /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_HD      (1U)           /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_HD      (0X0040U)      /* Link Partner 10BASE-T1S Half Duplex Ability. */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_FD      (7U)           /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_FD      (1U)           /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10S_FD      (0X0080U)      /* Link Partner 10BASE-T1S Full Duplex Ability. */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_REQ (12U)          /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_REQ (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_REQ (0X1000U)      /* 10BASE-T1L High Level Transmit Operating Mode Request. */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_ABL (13U)          /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_ABL (1U)           /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_TX_LVL_HI_ABL (0X2000U)      /* 10BASE-T1L High Level Transmit Operating Mode Ability. */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_EEE     (14U)          /* 10BASE-T1L EEE Ability. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_EEE     (1U)           /* 10BASE-T1L EEE Ability. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L_EEE     (0X4000U)      /* 10BASE-T1L EEE Ability. */
#define BITP_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L         (15U)          /* 10BASE-T1L Ability. */
#define BITL_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L         (1U)           /* 10BASE-T1L Ability. */
#define BITM_AN_B10_LP_ADV_ABILITY_AN_B10_LP_ADV_B10L         (0X8000U)      /* 10BASE-T1L Ability. */

/* ----------------------------------------------------------------------------------------------------
          AN_FRC_MODE_EN                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_FRC_MODE_EN_AN_FRC_MODE_EN                    (0U)           /* Autonegotiation Forced Mode. */
#define BITL_AN_FRC_MODE_EN_AN_FRC_MODE_EN                    (1U)           /* Autonegotiation Forced Mode. */
#define BITM_AN_FRC_MODE_EN_AN_FRC_MODE_EN                    (0X0001U)      /* Autonegotiation Forced Mode. */

/* ----------------------------------------------------------------------------------------------------
          AN_STATUS_EXTRA                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_STATUS_EXTRA_AN_LINK_GOOD                     (0U)           /* Autonegotiation Complete Indication. */
#define BITL_AN_STATUS_EXTRA_AN_LINK_GOOD                     (1U)           /* Autonegotiation Complete Indication. */
#define BITM_AN_STATUS_EXTRA_AN_LINK_GOOD                     (0X0001U)      /* Autonegotiation Complete Indication. */
#define BITP_AN_STATUS_EXTRA_AN_HCD_TECH                      (1U)           /* Highest Common Denominator (HCD) PHY Technology. */
#define BITL_AN_STATUS_EXTRA_AN_HCD_TECH                      (4U)           /* Highest Common Denominator (HCD) PHY Technology. */
#define BITM_AN_STATUS_EXTRA_AN_HCD_TECH                      (0X001EU)      /* Highest Common Denominator (HCD) PHY Technology. */
#define BITP_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN               (5U)           /* Master/slave Resolution Result. */
#define BITL_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN               (2U)           /* Master/slave Resolution Result. */
#define BITM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN               (0X0060U)      /* Master/slave Resolution Result. */
#define BITP_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN                  (7U)           /* Autonegotiation Tx Level Result */
#define BITL_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN                  (2U)           /* Autonegotiation Tx Level Result */
#define BITM_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN                  (0X0180U)      /* Autonegotiation Tx Level Result */
#define BITP_AN_STATUS_EXTRA_AN_INC_LINK                      (9U)           /* Incompatible Link Indication. */
#define BITL_AN_STATUS_EXTRA_AN_INC_LINK                      (1U)           /* Incompatible Link Indication. */
#define BITM_AN_STATUS_EXTRA_AN_INC_LINK                      (0X0200U)      /* Incompatible Link Indication. */
#define BITP_AN_STATUS_EXTRA_AN_LP_NP_RX                      (10U)          /* Next Page Request Received from Link Partner. */
#define BITL_AN_STATUS_EXTRA_AN_LP_NP_RX                      (1U)           /* Next Page Request Received from Link Partner. */
#define BITM_AN_STATUS_EXTRA_AN_LP_NP_RX                      (0X0400U)      /* Next Page Request Received from Link Partner. */

#define ENUM_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN_TX_LVL_NOT_RUN   (0X0000U)      /* Not run */
#define ENUM_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN_TX_LVL_LOW       (0X0002U)      /* Success, low transmit levels (1.0 Vpp) selected */
#define ENUM_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN_TX_LVL_HIGH      (0X0003U)      /* Success, high transmit levels (2.4 Vpp) selected */
#define ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_NOT_RUN    (0X0000U)      /* Not run */
#define ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_FAULT      (0X0001U)      /* Configuration fault */
#define ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_SLAVE      (0X0002U)      /* Success, PHY is configured as SLAVE */
#define ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_MASTER     (0X0003U)      /* Success, PHY is configured as MASTER */
#define ENUM_AN_STATUS_EXTRA_AN_HCD_TECH_HCD_NULL             (0X0000U)      /* NULL (not run) */
#define ENUM_AN_STATUS_EXTRA_AN_HCD_TECH_HCD_B10L             (0X0001U)      /* 10BASE-T1L */

/* ----------------------------------------------------------------------------------------------------
          AN_PHY_INST_STATUS                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_AN_PHY_INST_STATUS_IS_TX_LVL_LO                  (0U)           /* Tx Level Low Status */
#define BITL_AN_PHY_INST_STATUS_IS_TX_LVL_LO                  (1U)           /* Tx Level Low Status */
#define BITM_AN_PHY_INST_STATUS_IS_TX_LVL_LO                  (0X0001U)      /* Tx Level Low Status */
#define BITP_AN_PHY_INST_STATUS_IS_TX_LVL_HI                  (1U)           /* Tx Level High Status */
#define BITL_AN_PHY_INST_STATUS_IS_TX_LVL_HI                  (1U)           /* Tx Level High Status */
#define BITM_AN_PHY_INST_STATUS_IS_TX_LVL_HI                  (0X0002U)      /* Tx Level High Status */
#define BITP_AN_PHY_INST_STATUS_IS_CFG_SLV                    (2U)           /* Slave Status */
#define BITL_AN_PHY_INST_STATUS_IS_CFG_SLV                    (1U)           /* Slave Status */
#define BITM_AN_PHY_INST_STATUS_IS_CFG_SLV                    (0X0004U)      /* Slave Status */
#define BITP_AN_PHY_INST_STATUS_IS_CFG_MST                    (3U)           /* Master Status */
#define BITL_AN_PHY_INST_STATUS_IS_CFG_MST                    (1U)           /* Master Status */
#define BITM_AN_PHY_INST_STATUS_IS_CFG_MST                    (0X0008U)      /* Master Status */
#define BITP_AN_PHY_INST_STATUS_IS_AN_TX_EN                   (4U)           /* Autonegotiation Tx Enable Status */
#define BITL_AN_PHY_INST_STATUS_IS_AN_TX_EN                   (1U)           /* Autonegotiation Tx Enable Status */
#define BITM_AN_PHY_INST_STATUS_IS_AN_TX_EN                   (0X0010U)      /* Autonegotiation Tx Enable Status */

/* ----------------------------------------------------------------------------------------------------
          MMD1_DEV_ID1                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD1_DEV_ID1_MMD1_DEV_ID1                        (0U)           /* Organizationally Unique Identifier */
#define BITL_MMD1_DEV_ID1_MMD1_DEV_ID1                        (16U)          /* Organizationally Unique Identifier */
#define BITM_MMD1_DEV_ID1_MMD1_DEV_ID1                        (0XFFFFU)      /* Organizationally Unique Identifier */

/* ----------------------------------------------------------------------------------------------------
          MMD1_DEV_ID2                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD1_DEV_ID2_MMD1_REV_NUM                        (0U)           /* Revision Number */
#define BITL_MMD1_DEV_ID2_MMD1_REV_NUM                        (4U)           /* Revision Number */
#define BITM_MMD1_DEV_ID2_MMD1_REV_NUM                        (0X000FU)      /* Revision Number */
#define BITP_MMD1_DEV_ID2_MMD1_MODEL_NUM                      (4U)           /* Model Number */
#define BITL_MMD1_DEV_ID2_MMD1_MODEL_NUM                      (6U)           /* Model Number */
#define BITM_MMD1_DEV_ID2_MMD1_MODEL_NUM                      (0X03F0U)      /* Model Number */
#define BITP_MMD1_DEV_ID2_MMD1_DEV_ID2_OUI                    (10U)          /* Organizationally Unique Identifier */
#define BITL_MMD1_DEV_ID2_MMD1_DEV_ID2_OUI                    (6U)           /* Organizationally Unique Identifier */
#define BITM_MMD1_DEV_ID2_MMD1_DEV_ID2_OUI                    (0XFC00U)      /* Organizationally Unique Identifier */

/* ----------------------------------------------------------------------------------------------------
          MMD1_DEVS_IN_PKG1                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD1_DEVS_IN_PKG1_MMD1_DEVS_IN_PKG1              (0U)           /* Vendor specific 1 MMD Devices in Package */
#define BITL_MMD1_DEVS_IN_PKG1_MMD1_DEVS_IN_PKG1              (16U)          /* Vendor specific 1 MMD Devices in Package */
#define BITM_MMD1_DEVS_IN_PKG1_MMD1_DEVS_IN_PKG1              (0XFFFFU)      /* Vendor specific 1 MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          MMD1_DEVS_IN_PKG2                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD1_DEVS_IN_PKG2_MMD1_DEVS_IN_PKG2              (0U)           /* Vendor specific 1 MMD Devices in Package */
#define BITL_MMD1_DEVS_IN_PKG2_MMD1_DEVS_IN_PKG2              (16U)          /* Vendor specific 1 MMD Devices in Package */
#define BITM_MMD1_DEVS_IN_PKG2_MMD1_DEVS_IN_PKG2              (0XFFFFU)      /* Vendor specific 1 MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          MMD1_STATUS                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD1_STATUS_MMD1_STATUS                          (14U)          /* Vendor Specific 1 MMD Status */
#define BITL_MMD1_STATUS_MMD1_STATUS                          (2U)           /* Vendor Specific 1 MMD Status */
#define BITM_MMD1_STATUS_MMD1_STATUS                          (0XC000U)      /* Vendor Specific 1 MMD Status */

/* ----------------------------------------------------------------------------------------------------
          CRSM_IRQ_STATUS                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_IRQ_STATUS_CRSM_HRD_RST_IRQ_LH              (12U)          /* Hardware Reset Interrupt */
#define BITL_CRSM_IRQ_STATUS_CRSM_HRD_RST_IRQ_LH              (1U)           /* Hardware Reset Interrupt */
#define BITM_CRSM_IRQ_STATUS_CRSM_HRD_RST_IRQ_LH              (0X1000U)      /* Hardware Reset Interrupt */
#define BITP_CRSM_IRQ_STATUS_CRSM_SW_IRQ_LH                   (15U)          /* Software Requested Interrupt Event. */
#define BITL_CRSM_IRQ_STATUS_CRSM_SW_IRQ_LH                   (1U)           /* Software Requested Interrupt Event. */
#define BITM_CRSM_IRQ_STATUS_CRSM_SW_IRQ_LH                   (0X8000U)      /* Software Requested Interrupt Event. */

/* ----------------------------------------------------------------------------------------------------
          CRSM_IRQ_MASK                                         Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN                (12U)          /* Enable Hardware Reset Interrupt. */
#define BITL_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN                (1U)           /* Enable Hardware Reset Interrupt. */
#define BITM_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN                (0X1000U)      /* Enable Hardware Reset Interrupt. */
#define BITP_CRSM_IRQ_MASK_CRSM_SW_IRQ_REQ                    (15U)          /* Software Interrupt Request. */
#define BITL_CRSM_IRQ_MASK_CRSM_SW_IRQ_REQ                    (1U)           /* Software Interrupt Request. */
#define BITM_CRSM_IRQ_MASK_CRSM_SW_IRQ_REQ                    (0X8000U)      /* Software Interrupt Request. */

/* ----------------------------------------------------------------------------------------------------
          CRSM_SFT_RST                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_SFT_RST_CRSM_SFT_RST                        (0U)           /* Software Reset Register. */
#define BITL_CRSM_SFT_RST_CRSM_SFT_RST                        (1U)           /* Software Reset Register. */
#define BITM_CRSM_SFT_RST_CRSM_SFT_RST                        (0X0001U)      /* Software Reset Register. */

/* ----------------------------------------------------------------------------------------------------
          CRSM_SFT_PD_CNTRL                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_SFT_PD_CNTRL_CRSM_SFT_PD                    (0U)           /* Software Power-down */
#define BITL_CRSM_SFT_PD_CNTRL_CRSM_SFT_PD                    (1U)           /* Software Power-down */
#define BITM_CRSM_SFT_PD_CNTRL_CRSM_SFT_PD                    (0X0001U)      /* Software Power-down */

/* ----------------------------------------------------------------------------------------------------
          CRSM_PHY_SUBSYS_RST                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_PHY_SUBSYS_RST_CRSM_PHY_SUBSYS_RST          (0U)           /* PHY Subsystem Reset */
#define BITL_CRSM_PHY_SUBSYS_RST_CRSM_PHY_SUBSYS_RST          (1U)           /* PHY Subsystem Reset */
#define BITM_CRSM_PHY_SUBSYS_RST_CRSM_PHY_SUBSYS_RST          (0X0001U)      /* PHY Subsystem Reset */

/* ----------------------------------------------------------------------------------------------------
          CRSM_MAC_IF_RST                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_MAC_IF_RST_CRSM_MAC_IF_RST                  (0U)           /* PHY MAC Interface Reset */
#define BITL_CRSM_MAC_IF_RST_CRSM_MAC_IF_RST                  (1U)           /* PHY MAC Interface Reset */
#define BITM_CRSM_MAC_IF_RST_CRSM_MAC_IF_RST                  (0X0001U)      /* PHY MAC Interface Reset */

/* ----------------------------------------------------------------------------------------------------
          CRSM_STAT                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_STAT_CRSM_SYS_RDY                           (0U)           /* System Ready */
#define BITL_CRSM_STAT_CRSM_SYS_RDY                           (1U)           /* System Ready */
#define BITM_CRSM_STAT_CRSM_SYS_RDY                           (0X0001U)      /* System Ready */
#define BITP_CRSM_STAT_CRSM_SFT_PD_RDY                        (1U)           /* Software Power-down Status */
#define BITL_CRSM_STAT_CRSM_SFT_PD_RDY                        (1U)           /* Software Power-down Status */
#define BITM_CRSM_STAT_CRSM_SFT_PD_RDY                        (0X0002U)      /* Software Power-down Status */

/* ----------------------------------------------------------------------------------------------------
          CRSM_PMG_CNTRL                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_PMG_CNTRL_CRSM_FRC_OSC_EN                   (0U)           /* Force Digital Boot Oscillator Clock Enable */
#define BITL_CRSM_PMG_CNTRL_CRSM_FRC_OSC_EN                   (1U)           /* Force Digital Boot Oscillator Clock Enable */
#define BITM_CRSM_PMG_CNTRL_CRSM_FRC_OSC_EN                   (0X0001U)      /* Force Digital Boot Oscillator Clock Enable */

/* ----------------------------------------------------------------------------------------------------
          CRSM_DIAG_CLK_CTRL                                    Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CRSM_DIAG_CLK_CTRL_CRSM_DIAG_CLK_EN              (0U)           /* Enable the Diagnostics Clock */
#define BITL_CRSM_DIAG_CLK_CTRL_CRSM_DIAG_CLK_EN              (1U)           /* Enable the Diagnostics Clock */
#define BITM_CRSM_DIAG_CLK_CTRL_CRSM_DIAG_CLK_EN              (0X0001U)      /* Enable the Diagnostics Clock */

/* ----------------------------------------------------------------------------------------------------
          MGMT_PRT_PKG                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MGMT_PRT_PKG_MGMT_PRT_PKG_VAL                    (0U)           /* Package Type */
#define BITL_MGMT_PRT_PKG_MGMT_PRT_PKG_VAL                    (6U)           /* Package Type */
#define BITM_MGMT_PRT_PKG_MGMT_PRT_PKG_VAL                    (0X003FU)      /* Package Type */

/* ----------------------------------------------------------------------------------------------------
          MGMT_MDIO_CNTRL                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MGMT_MDIO_CNTRL_MGMT_GRP_MDIO_EN                 (0U)           /* Enable MDIO PHY/_PORT Group Address Mode */
#define BITL_MGMT_MDIO_CNTRL_MGMT_GRP_MDIO_EN                 (1U)           /* Enable MDIO PHY/_PORT Group Address Mode */
#define BITM_MGMT_MDIO_CNTRL_MGMT_GRP_MDIO_EN                 (0X0001U)      /* Enable MDIO PHY/_PORT Group Address Mode */

/* ----------------------------------------------------------------------------------------------------
          DIGIO_PINMUX                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_DIGIO_PINMUX_DIGIO_LINK_ST_POLARITY              (0U)           /* LINK_ST Polarity */
#define BITL_DIGIO_PINMUX_DIGIO_LINK_ST_POLARITY              (1U)           /* LINK_ST Polarity */
#define BITM_DIGIO_PINMUX_DIGIO_LINK_ST_POLARITY              (0X0001U)      /* LINK_ST Polarity */
#define BITP_DIGIO_PINMUX_DIGIO_LED1_PINMUX                   (1U)           /* Pin Mux Select for LED_1 */
#define BITL_DIGIO_PINMUX_DIGIO_LED1_PINMUX                   (3U)           /* Pin Mux Select for LED_1 */
#define BITM_DIGIO_PINMUX_DIGIO_LED1_PINMUX                   (0X000EU)      /* Pin Mux Select for LED_1 */
#define BITP_DIGIO_PINMUX_DIGIO_TSCAPT_PINMUX                 (4U)           /* Pin Mux Select for TS_CAPT */
#define BITL_DIGIO_PINMUX_DIGIO_TSCAPT_PINMUX                 (2U)           /* Pin Mux Select for TS_CAPT */
#define BITM_DIGIO_PINMUX_DIGIO_TSCAPT_PINMUX                 (0X0030U)      /* Pin Mux Select for TS_CAPT */
#define BITP_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX                (6U)           /* Pin Mux Select for TS_TIMER */
#define BITL_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX                (2U)           /* Pin Mux Select for TS_TIMER */
#define BITM_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX                (0X00C0U)      /* Pin Mux Select for TS_TIMER */

#define ENUM_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX_RXD_1          (0X0000U)      /* RXD_1 */
#define ENUM_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX_LED_0          (0X0001U)      /* LED_0 */
#define ENUM_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX_INT_N          (0X0002U)      /* INT_N */
#define ENUM_DIGIO_PINMUX_DIGIO_TSTIMER_PINMUX_TS_TIMER_NA    (0X0003U)      /* TS_TIMER not assigned */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_LED_1             (0X0000U)      /* LED_1 */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_TX_ER             (0X0001U)      /* TX_ER */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_TX_EN             (0X0002U)      /* TX_EN */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_TX_CLK            (0X0003U)      /* TX_CLK */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_TXD_0             (0X0004U)      /* TXD_0 */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_TXD_2             (0X0005U)      /* TXD_2 */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_LINK_ST           (0X0006U)      /* LINK_ST */
#define ENUM_DIGIO_PINMUX_DIGIO_LED1_PINMUX_LED_1_NA          (0X0007U)      /* LED_1 output not enabled */
#define ENUM_DIGIO_PINMUX_DIGIO_LINK_ST_POLARITY_ASSERT_HIGH  (0X0000U)      /* ASSERT_HIGH */
#define ENUM_DIGIO_PINMUX_DIGIO_LINK_ST_POLARITY_ASSERT_LOW   (0X0001U)      /* ASSERT_LOW */

/* ----------------------------------------------------------------------------------------------------
          LED0_BLINK_TIME_CNTRL                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_LED0_BLINK_TIME_CNTRL_LED0_OFF_N4MS              (0U)           /* Led 0 Off Blink Time. */
#define BITL_LED0_BLINK_TIME_CNTRL_LED0_OFF_N4MS              (8U)           /* Led 0 Off Blink Time. */
#define BITM_LED0_BLINK_TIME_CNTRL_LED0_OFF_N4MS              (0X00FFU)      /* Led 0 Off Blink Time. */
#define BITP_LED0_BLINK_TIME_CNTRL_LED0_ON_N4MS               (8U)           /* LED 0 On Blink Time */
#define BITL_LED0_BLINK_TIME_CNTRL_LED0_ON_N4MS               (8U)           /* LED 0 On Blink Time */
#define BITM_LED0_BLINK_TIME_CNTRL_LED0_ON_N4MS               (0XFF00U)      /* LED 0 On Blink Time */

/* ----------------------------------------------------------------------------------------------------
          LED1_BLINK_TIME_CNTRL                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_LED1_BLINK_TIME_CNTRL_LED1_OFF_N4MS              (0U)           /* LED 1 Off Blink Time */
#define BITL_LED1_BLINK_TIME_CNTRL_LED1_OFF_N4MS              (8U)           /* LED 1 Off Blink Time */
#define BITM_LED1_BLINK_TIME_CNTRL_LED1_OFF_N4MS              (0X00FFU)      /* LED 1 Off Blink Time */
#define BITP_LED1_BLINK_TIME_CNTRL_LED1_ON_N4MS               (8U)           /* LED 1 On Blink Time */
#define BITL_LED1_BLINK_TIME_CNTRL_LED1_ON_N4MS               (8U)           /* LED 1 On Blink Time */
#define BITM_LED1_BLINK_TIME_CNTRL_LED1_ON_N4MS               (0XFF00U)      /* LED 1 On Blink Time */

/* ----------------------------------------------------------------------------------------------------
          LED_CNTRL                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_LED_CNTRL_LED0_FUNCTION                          (0U)           /* LED 0 Pin Function */
#define BITL_LED_CNTRL_LED0_FUNCTION                          (5U)           /* LED 0 Pin Function */
#define BITM_LED_CNTRL_LED0_FUNCTION                          (0X001FU)      /* LED 0 Pin Function */
#define BITP_LED_CNTRL_LED0_MODE                              (5U)           /* LED 0 Mode Selection */
#define BITL_LED_CNTRL_LED0_MODE                              (1U)           /* LED 0 Mode Selection */
#define BITM_LED_CNTRL_LED0_MODE                              (0X0020U)      /* LED 0 Mode Selection */
#define BITP_LED_CNTRL_LED0_LINK_ST_QUALIFY                   (6U)           /* Qualify Certain LED 0 Options with LINK_STATUS */
#define BITL_LED_CNTRL_LED0_LINK_ST_QUALIFY                   (1U)           /* Qualify Certain LED 0 Options with LINK_STATUS */
#define BITM_LED_CNTRL_LED0_LINK_ST_QUALIFY                   (0X0040U)      /* Qualify Certain LED 0 Options with LINK_STATUS */
#define BITP_LED_CNTRL_LED0_EN                                (7U)           /* LED 0 Enable */
#define BITL_LED_CNTRL_LED0_EN                                (1U)           /* LED 0 Enable */
#define BITM_LED_CNTRL_LED0_EN                                (0X0080U)      /* LED 0 Enable */
#define BITP_LED_CNTRL_LED1_FUNCTION                          (8U)           /* LED 1 Pin Function */
#define BITL_LED_CNTRL_LED1_FUNCTION                          (5U)           /* LED 1 Pin Function */
#define BITM_LED_CNTRL_LED1_FUNCTION                          (0X1F00U)      /* LED 1 Pin Function */
#define BITP_LED_CNTRL_LED1_MODE                              (13U)          /* LED 1 Mode Selection */
#define BITL_LED_CNTRL_LED1_MODE                              (1U)           /* LED 1 Mode Selection */
#define BITM_LED_CNTRL_LED1_MODE                              (0X2000U)      /* LED 1 Mode Selection */
#define BITP_LED_CNTRL_LED1_LINK_ST_QUALIFY                   (14U)          /* Qualify Certain LED 1 Options with LINK_STATUS */
#define BITL_LED_CNTRL_LED1_LINK_ST_QUALIFY                   (1U)           /* Qualify Certain LED 1 Options with LINK_STATUS */
#define BITM_LED_CNTRL_LED1_LINK_ST_QUALIFY                   (0X4000U)      /* Qualify Certain LED 1 Options with LINK_STATUS */
#define BITP_LED_CNTRL_LED1_EN                                (15U)          /* LED 1 Enable */
#define BITL_LED_CNTRL_LED1_EN                                (1U)           /* LED 1 Enable */
#define BITM_LED_CNTRL_LED1_EN                                (0X8000U)      /* LED 1 Enable */

#define ENUM_LED_CNTRL_LED1_MODE_LED_MODE1                    (0X0000U)      /* LED_MODE1 */
#define ENUM_LED_CNTRL_LED1_MODE_LED_MODE2                    (0X0001U)      /* LED_MODE2 */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_TXRX_ACTIVITY     (0X0000U)      /* LINKUP_TXRX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_TX_ACTIVITY       (0X0001U)      /* LINKUP_TX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_RX_ACTIVITY       (0X0002U)      /* LINKUP_RX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_ONLY              (0X0003U)      /* LINKUP_ONLY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TXRX_ACTIVITY            (0X0004U)      /* TXRX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TX_ACTIVITY              (0X0005U)      /* TX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_RX_ACTIVITY              (0X0006U)      /* RX_ACTIVITY */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_RX_ER             (0X0007U)      /* LINKUP_RX_ER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LINKUP_RX_TX_ER          (0X0008U)      /* LINKUP_RX_TX_ER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_RX_ER                    (0X0009U)      /* RX_ER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_RX_TX_ER                 (0X000AU)      /* RX_TX_ER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TX_SOP                   (0X000BU)      /* TX_SOP */
#define ENUM_LED_CNTRL_LED1_FUNCTION_RX_SOP                   (0X000CU)      /* RX_SOP */
#define ENUM_LED_CNTRL_LED1_FUNCTION_ON                       (0X000DU)      /* ON */
#define ENUM_LED_CNTRL_LED1_FUNCTION_OFF                      (0X000EU)      /* OFF */
#define ENUM_LED_CNTRL_LED1_FUNCTION_BLINK                    (0X000FU)      /* BLINK */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TX_LEVEL_2P4             (0X0010U)      /* TX_LEVEL_2P4 */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TX_LEVEL_1P0             (0X0011U)      /* TX_LEVEL_1P0 */
#define ENUM_LED_CNTRL_LED1_FUNCTION_MASTER                   (0X0012U)      /* MASTER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_SLAVE                    (0X0013U)      /* SLAVE */
#define ENUM_LED_CNTRL_LED1_FUNCTION_INCOMPATIBLE_LINK_CFG    (0X0014U)      /* INCOMPATIBLE_LINK_CFG */
#define ENUM_LED_CNTRL_LED1_FUNCTION_AN_LINK_GOOD             (0X0015U)      /* AN_LINK_GOOD */
#define ENUM_LED_CNTRL_LED1_FUNCTION_AN_COMPLETE              (0X0016U)      /* AN_COMPLETE */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TS_TIMER                 (0X0017U)      /* TS_TIMER */
#define ENUM_LED_CNTRL_LED1_FUNCTION_LOC_RCVR_STATUS          (0X0018U)      /* LOC_RCVR_STATUS */
#define ENUM_LED_CNTRL_LED1_FUNCTION_REM_RCVR_STATUS          (0X0019U)      /* REM_RCVR_STATUS */
#define ENUM_LED_CNTRL_LED1_FUNCTION_CLK25_REF                (0X001AU)      /* CLK25_REF */
#define ENUM_LED_CNTRL_LED1_FUNCTION_TX_TCLK                  (0X001BU)      /* TX_TCLK */
#define ENUM_LED_CNTRL_LED1_FUNCTION_CLK_120MHZ               (0X001CU)      /* CLK_120MHZ */
#define ENUM_LED_CNTRL_LED0_MODE_LED_MODE1                    (0X0000U)      /* LED_MODE1 */
#define ENUM_LED_CNTRL_LED0_MODE_LED_MODE2                    (0X0001U)      /* LED_MODE2 */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_TXRX_ACTIVITY     (0X0000U)      /* LINKUP_TXRX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_TX_ACTIVITY       (0X0001U)      /* LINKUP_TX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_RX_ACTIVITY       (0X0002U)      /* LINKUP_RX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_ONLY              (0X0003U)      /* LINKUP_ONLY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TXRX_ACTIVITY            (0X0004U)      /* TXRX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TX_ACTIVITY              (0X0005U)      /* TX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_RX_ACTIVITY              (0X0006U)      /* RX_ACTIVITY */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_RX_ER             (0X0007U)      /* LINKUP_RX_ER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LINKUP_RX_TX_ER          (0X0008U)      /* LINKUP_RX_TX_ER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_RX_ER                    (0X0009U)      /* RX_ER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_RX_TX_ER                 (0X000AU)      /* RX_TX_ER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TX_SOP                   (0X000BU)      /* TX_SOP */
#define ENUM_LED_CNTRL_LED0_FUNCTION_RX_SOP                   (0X000CU)      /* RX_SOP */
#define ENUM_LED_CNTRL_LED0_FUNCTION_ON                       (0X000DU)      /* ON */
#define ENUM_LED_CNTRL_LED0_FUNCTION_OFF                      (0X000EU)      /* OFF */
#define ENUM_LED_CNTRL_LED0_FUNCTION_BLINK                    (0X000FU)      /* BLINK */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TX_LEVEL_2P4             (0X0010U)      /* TX_LEVEL_2P4 */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TX_LEVEL_1P0             (0X0011U)      /* TX_LEVEL_1P0 */
#define ENUM_LED_CNTRL_LED0_FUNCTION_MASTER                   (0X0012U)      /* MASTER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_SLAVE                    (0X0013U)      /* SLAVE */
#define ENUM_LED_CNTRL_LED0_FUNCTION_INCOMPATIBLE_LINK_CFG    (0X0014U)      /* INCOMPATIBLE_LINK_CFG */
#define ENUM_LED_CNTRL_LED0_FUNCTION_AN_LINK_GOOD             (0X0015U)      /* AN_LINK_GOOD */
#define ENUM_LED_CNTRL_LED0_FUNCTION_AN_COMPLETE              (0X0016U)      /* AN_COMPLETE */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TS_TIMER                 (0X0017U)      /* TS_TIMER */
#define ENUM_LED_CNTRL_LED0_FUNCTION_LOC_RCVR_STATUS          (0X0018U)      /* LOC_RCVR_STATUS */
#define ENUM_LED_CNTRL_LED0_FUNCTION_REM_RCVR_STATUS          (0X0019U)      /* REM_RCVR_STATUS */
#define ENUM_LED_CNTRL_LED0_FUNCTION_CLK25_REF                (0X001AU)      /* CLK25_REF */
#define ENUM_LED_CNTRL_LED0_FUNCTION_TX_TCLK                  (0X001BU)      /* TX_TCLK */
#define ENUM_LED_CNTRL_LED0_FUNCTION_CLK_120MHZ               (0X001CU)      /* CLK_120MHZ */

/* ----------------------------------------------------------------------------------------------------
          LED_POLARITY                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_LED_POLARITY_LED0_POLARITY                       (0U)           /* LED 0 Polarity */
#define BITL_LED_POLARITY_LED0_POLARITY                       (2U)           /* LED 0 Polarity */
#define BITM_LED_POLARITY_LED0_POLARITY                       (0X0003U)      /* LED 0 Polarity */
#define BITP_LED_POLARITY_LED1_POLARITY                       (2U)           /* LED 1 Polarity */
#define BITL_LED_POLARITY_LED1_POLARITY                       (2U)           /* LED 1 Polarity */
#define BITM_LED_POLARITY_LED1_POLARITY                       (0X000CU)      /* LED 1 Polarity */

#define ENUM_LED_POLARITY_LED1_POLARITY_LED_AUTO_SENSE        (0X0000U)      /* LED Auto Sense */
#define ENUM_LED_POLARITY_LED1_POLARITY_LED_ACTIVE_HI         (0X0001U)      /* LED Active High */
#define ENUM_LED_POLARITY_LED1_POLARITY_LED_ACTIVE_LO         (0X0002U)      /* LED Active Low */
#define ENUM_LED_POLARITY_LED0_POLARITY_LED_AUTO_SENSE        (0X0000U)      /* LED Auto Sense */
#define ENUM_LED_POLARITY_LED0_POLARITY_LED_ACTIVE_HI         (0X0001U)      /* LED Active High */
#define ENUM_LED_POLARITY_LED0_POLARITY_LED_ACTIVE_LO         (0X0002U)      /* LED Active Low */

/* ----------------------------------------------------------------------------------------------------
          MMD2_DEV_ID1                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD2_DEV_ID1_MMD2_DEV_ID1                        (0U)           /* Vendor specific 2 MMD device identifier */
#define BITL_MMD2_DEV_ID1_MMD2_DEV_ID1                        (16U)          /* Vendor specific 2 MMD device identifier */
#define BITM_MMD2_DEV_ID1_MMD2_DEV_ID1                        (0XFFFFU)      /* Vendor specific 2 MMD device identifier */

/* ----------------------------------------------------------------------------------------------------
          MMD2_DEV_ID2                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD2_DEV_ID2_MMD2_REV_NUM                        (0U)           /* Revision number */
#define BITL_MMD2_DEV_ID2_MMD2_REV_NUM                        (4U)           /* Revision number */
#define BITM_MMD2_DEV_ID2_MMD2_REV_NUM                        (0X000FU)      /* Revision number */
#define BITP_MMD2_DEV_ID2_MMD2_MODEL_NUM                      (4U)           /* Model Number */
#define BITL_MMD2_DEV_ID2_MMD2_MODEL_NUM                      (6U)           /* Model Number */
#define BITM_MMD2_DEV_ID2_MMD2_MODEL_NUM                      (0X03F0U)      /* Model Number */
#define BITP_MMD2_DEV_ID2_MMD2_DEV_ID2_OUI                    (10U)          /* OUI bits */
#define BITL_MMD2_DEV_ID2_MMD2_DEV_ID2_OUI                    (6U)           /* OUI bits */
#define BITM_MMD2_DEV_ID2_MMD2_DEV_ID2_OUI                    (0XFC00U)      /* OUI bits */

/* ----------------------------------------------------------------------------------------------------
          MMD2_DEVS_IN_PKG1                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD2_DEVS_IN_PKG1_MMD2_DEVS_IN_PKG1              (0U)           /* Vendor Specific 2 MMD Devices in Package */
#define BITL_MMD2_DEVS_IN_PKG1_MMD2_DEVS_IN_PKG1              (16U)          /* Vendor Specific 2 MMD Devices in Package */
#define BITM_MMD2_DEVS_IN_PKG1_MMD2_DEVS_IN_PKG1              (0XFFFFU)      /* Vendor Specific 2 MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          MMD2_DEVS_IN_PKG2                                     Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD2_DEVS_IN_PKG2_MMD2_DEVS_IN_PKG2              (0U)           /* Vendor Specific 2 MMD Devices in Package */
#define BITL_MMD2_DEVS_IN_PKG2_MMD2_DEVS_IN_PKG2              (16U)          /* Vendor Specific 2 MMD Devices in Package */
#define BITM_MMD2_DEVS_IN_PKG2_MMD2_DEVS_IN_PKG2              (0XFFFFU)      /* Vendor Specific 2 MMD Devices in Package */

/* ----------------------------------------------------------------------------------------------------
          MMD2_STATUS                                           Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MMD2_STATUS_MMD2_STATUS                          (14U)          /* Vendor specific 2 MMD Status */
#define BITL_MMD2_STATUS_MMD2_STATUS                          (2U)           /* Vendor specific 2 MMD Status */
#define BITM_MMD2_STATUS_MMD2_STATUS                          (0XC000U)      /* Vendor specific 2 MMD Status */

/* ----------------------------------------------------------------------------------------------------
          PHY_SUBSYS_IRQ_STATUS                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PHY_SUBSYS_IRQ_STATUS_LINK_STAT_CHNG_LH          (1U)           /* Link Status Change. */
#define BITL_PHY_SUBSYS_IRQ_STATUS_LINK_STAT_CHNG_LH          (1U)           /* Link Status Change. */
#define BITM_PHY_SUBSYS_IRQ_STATUS_LINK_STAT_CHNG_LH          (0X0002U)      /* Link Status Change. */
#define BITP_PHY_SUBSYS_IRQ_STATUS_AN_STAT_CHNG_IRQ_LH        (11U)          /* Autonegotiation Status Change Interrupt. */
#define BITL_PHY_SUBSYS_IRQ_STATUS_AN_STAT_CHNG_IRQ_LH        (1U)           /* Autonegotiation Status Change Interrupt. */
#define BITM_PHY_SUBSYS_IRQ_STATUS_AN_STAT_CHNG_IRQ_LH        (0X0800U)      /* Autonegotiation Status Change Interrupt. */
#define BITP_PHY_SUBSYS_IRQ_STATUS_MAC_IF_EBUF_ERR_IRQ_LH     (13U)          /* Mac Interface Buffers Overflow/underflow Interrupt */
#define BITL_PHY_SUBSYS_IRQ_STATUS_MAC_IF_EBUF_ERR_IRQ_LH     (1U)           /* Mac Interface Buffers Overflow/underflow Interrupt */
#define BITM_PHY_SUBSYS_IRQ_STATUS_MAC_IF_EBUF_ERR_IRQ_LH     (0X2000U)      /* Mac Interface Buffers Overflow/underflow Interrupt */
#define BITP_PHY_SUBSYS_IRQ_STATUS_MAC_IF_FC_FG_IRQ_LH        (14U)          /* Mac Interface Frame CHECKER/_GENERATOR Interrupt */
#define BITL_PHY_SUBSYS_IRQ_STATUS_MAC_IF_FC_FG_IRQ_LH        (1U)           /* Mac Interface Frame CHECKER/_GENERATOR Interrupt */
#define BITM_PHY_SUBSYS_IRQ_STATUS_MAC_IF_FC_FG_IRQ_LH        (0X4000U)      /* Mac Interface Frame CHECKER/_GENERATOR Interrupt */

/* ----------------------------------------------------------------------------------------------------
          PHY_SUBSYS_IRQ_MASK                                   Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_PHY_SUBSYS_IRQ_MASK_LINK_STAT_CHNG_IRQ_EN        (1U)           /* Enable Link Status Change Interrupt. */
#define BITL_PHY_SUBSYS_IRQ_MASK_LINK_STAT_CHNG_IRQ_EN        (1U)           /* Enable Link Status Change Interrupt. */
#define BITM_PHY_SUBSYS_IRQ_MASK_LINK_STAT_CHNG_IRQ_EN        (0X0002U)      /* Enable Link Status Change Interrupt. */
#define BITP_PHY_SUBSYS_IRQ_MASK_AN_STAT_CHNG_IRQ_EN          (11U)          /* Enable Autonegotiation Status Change Interrupt. */
#define BITL_PHY_SUBSYS_IRQ_MASK_AN_STAT_CHNG_IRQ_EN          (1U)           /* Enable Autonegotiation Status Change Interrupt. */
#define BITM_PHY_SUBSYS_IRQ_MASK_AN_STAT_CHNG_IRQ_EN          (0X0800U)      /* Enable Autonegotiation Status Change Interrupt. */
#define BITP_PHY_SUBSYS_IRQ_MASK_MAC_IF_EBUF_ERR_IRQ_EN       (13U)          /* Enable Mac Interface Buffers Overflow/underflow Interrupt */
#define BITL_PHY_SUBSYS_IRQ_MASK_MAC_IF_EBUF_ERR_IRQ_EN       (1U)           /* Enable Mac Interface Buffers Overflow/underflow Interrupt */
#define BITM_PHY_SUBSYS_IRQ_MASK_MAC_IF_EBUF_ERR_IRQ_EN       (0X2000U)      /* Enable Mac Interface Buffers Overflow/underflow Interrupt */
#define BITP_PHY_SUBSYS_IRQ_MASK_MAC_IF_FC_FG_IRQ_EN          (14U)          /* Enable Mac Interface Frame CHECKER/_GENERATOR Interrupt */
#define BITL_PHY_SUBSYS_IRQ_MASK_MAC_IF_FC_FG_IRQ_EN          (1U)           /* Enable Mac Interface Frame CHECKER/_GENERATOR Interrupt */
#define BITM_PHY_SUBSYS_IRQ_MASK_MAC_IF_FC_FG_IRQ_EN          (0X4000U)      /* Enable Mac Interface Frame CHECKER/_GENERATOR Interrupt */

/* ----------------------------------------------------------------------------------------------------
          FC_EN                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_EN_FC_EN                                      (0U)           /* Frame Checker Enable */
#define BITL_FC_EN_FC_EN                                      (1U)           /* Frame Checker Enable */
#define BITM_FC_EN_FC_EN                                      (0X0001U)      /* Frame Checker Enable */

/* ----------------------------------------------------------------------------------------------------
          FC_IRQ_EN                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_IRQ_EN_FC_IRQ_EN                              (0U)           /* Frame Checker Interrupt Enable */
#define BITL_FC_IRQ_EN_FC_IRQ_EN                              (1U)           /* Frame Checker Interrupt Enable */
#define BITM_FC_IRQ_EN_FC_IRQ_EN                              (0X0001U)      /* Frame Checker Interrupt Enable */

/* ----------------------------------------------------------------------------------------------------
          FC_TX_SEL                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_TX_SEL_FC_TX_SEL                              (0U)           /* Frame Checker Transmit Select */
#define BITL_FC_TX_SEL_FC_TX_SEL                              (1U)           /* Frame Checker Transmit Select */
#define BITM_FC_TX_SEL_FC_TX_SEL                              (0X0001U)      /* Frame Checker Transmit Select */

/* ----------------------------------------------------------------------------------------------------
          RX_ERR_CNT                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_RX_ERR_CNT_RX_ERR_CNT                            (0U)           /* Receive Error Count */
#define BITL_RX_ERR_CNT_RX_ERR_CNT                            (16U)          /* Receive Error Count */
#define BITM_RX_ERR_CNT_RX_ERR_CNT                            (0XFFFFU)      /* Receive Error Count */

/* ----------------------------------------------------------------------------------------------------
          FC_FRM_CNT_H                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_FRM_CNT_H_FC_FRM_CNT_H                        (0U)           /* Bits [31:16] of Latched Copy of the Number of Received Frames */
#define BITL_FC_FRM_CNT_H_FC_FRM_CNT_H                        (16U)          /* Bits [31:16] of Latched Copy of the Number of Received Frames */
#define BITM_FC_FRM_CNT_H_FC_FRM_CNT_H                        (0XFFFFU)      /* Bits [31:16] of Latched Copy of the Number of Received Frames */

/* ----------------------------------------------------------------------------------------------------
          FC_FRM_CNT_L                                          Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_FRM_CNT_L_FC_FRM_CNT_L                        (0U)           /* Bits [15:0] of Latched Copy of the Number of Received Frames */
#define BITL_FC_FRM_CNT_L_FC_FRM_CNT_L                        (16U)          /* Bits [15:0] of Latched Copy of the Number of Received Frames */
#define BITM_FC_FRM_CNT_L_FC_FRM_CNT_L                        (0XFFFFU)      /* Bits [15:0] of Latched Copy of the Number of Received Frames */

/* ----------------------------------------------------------------------------------------------------
          FC_LEN_ERR_CNT                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_LEN_ERR_CNT_FC_LEN_ERR_CNT                    (0U)           /* Latched Copy of the Frame Length Error Counter */
#define BITL_FC_LEN_ERR_CNT_FC_LEN_ERR_CNT                    (16U)          /* Latched Copy of the Frame Length Error Counter */
#define BITM_FC_LEN_ERR_CNT_FC_LEN_ERR_CNT                    (0XFFFFU)      /* Latched Copy of the Frame Length Error Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_ALGN_ERR_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_ALGN_ERR_CNT_FC_ALGN_ERR_CNT                  (0U)           /* Latched Copy of the Frame Alignment Error Counter */
#define BITL_FC_ALGN_ERR_CNT_FC_ALGN_ERR_CNT                  (16U)          /* Latched Copy of the Frame Alignment Error Counter */
#define BITM_FC_ALGN_ERR_CNT_FC_ALGN_ERR_CNT                  (0XFFFFU)      /* Latched Copy of the Frame Alignment Error Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_SYMB_ERR_CNT                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_SYMB_ERR_CNT_FC_SYMB_ERR_CNT                  (0U)           /* Latched Copy of the Symbol Error Counter */
#define BITL_FC_SYMB_ERR_CNT_FC_SYMB_ERR_CNT                  (16U)          /* Latched Copy of the Symbol Error Counter */
#define BITM_FC_SYMB_ERR_CNT_FC_SYMB_ERR_CNT                  (0XFFFFU)      /* Latched Copy of the Symbol Error Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_OSZ_CNT                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_OSZ_CNT_FC_OSZ_CNT                            (0U)           /* Latched copy of the Overisized Frame Error Counter */
#define BITL_FC_OSZ_CNT_FC_OSZ_CNT                            (16U)          /* Latched copy of the Overisized Frame Error Counter */
#define BITM_FC_OSZ_CNT_FC_OSZ_CNT                            (0XFFFFU)      /* Latched copy of the Overisized Frame Error Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_USZ_CNT                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_USZ_CNT_FC_USZ_CNT                            (0U)           /* Latched Copy of the Undersized Frame Error Counter */
#define BITL_FC_USZ_CNT_FC_USZ_CNT                            (16U)          /* Latched Copy of the Undersized Frame Error Counter */
#define BITM_FC_USZ_CNT_FC_USZ_CNT                            (0XFFFFU)      /* Latched Copy of the Undersized Frame Error Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_ODD_CNT                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_ODD_CNT_FC_ODD_CNT                            (0U)           /* Latched Copy of the Odd Nibble Counter */
#define BITL_FC_ODD_CNT_FC_ODD_CNT                            (16U)          /* Latched Copy of the Odd Nibble Counter */
#define BITM_FC_ODD_CNT_FC_ODD_CNT                            (0XFFFFU)      /* Latched Copy of the Odd Nibble Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_ODD_PRE_CNT                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_ODD_PRE_CNT_FC_ODD_PRE_CNT                    (0U)           /* Latched Copy of the Odd Preamble Packet Counter */
#define BITL_FC_ODD_PRE_CNT_FC_ODD_PRE_CNT                    (16U)          /* Latched Copy of the Odd Preamble Packet Counter */
#define BITM_FC_ODD_PRE_CNT_FC_ODD_PRE_CNT                    (0XFFFFU)      /* Latched Copy of the Odd Preamble Packet Counter */

/* ----------------------------------------------------------------------------------------------------
          FC_FALSE_CARRIER_CNT                                  Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FC_FALSE_CARRIER_CNT_FC_FALSE_CARRIER_CNT        (0U)           /* Latched Copy of the False Carrier Events Counter */
#define BITL_FC_FALSE_CARRIER_CNT_FC_FALSE_CARRIER_CNT        (16U)          /* Latched Copy of the False Carrier Events Counter */
#define BITM_FC_FALSE_CARRIER_CNT_FC_FALSE_CARRIER_CNT        (0XFFFFU)      /* Latched Copy of the False Carrier Events Counter */

/* ----------------------------------------------------------------------------------------------------
          FG_EN                                                 Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_EN_FG_EN                                      (0U)           /* Frame Generator Enable */
#define BITL_FG_EN_FG_EN                                      (1U)           /* Frame Generator Enable */
#define BITM_FG_EN_FG_EN                                      (0X0001U)      /* Frame Generator Enable */

/* ----------------------------------------------------------------------------------------------------
          FG_CNTRL_RSTRT                                        Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_CNTRL_RSTRT_FG_CNTRL                          (0U)           /* Frame Generator Control */
#define BITL_FG_CNTRL_RSTRT_FG_CNTRL                          (3U)           /* Frame Generator Control */
#define BITM_FG_CNTRL_RSTRT_FG_CNTRL                          (0X0007U)      /* Frame Generator Control */
#define BITP_FG_CNTRL_RSTRT_FG_RSTRT                          (3U)           /* Frame Generator Restart */
#define BITL_FG_CNTRL_RSTRT_FG_RSTRT                          (1U)           /* Frame Generator Restart */
#define BITM_FG_CNTRL_RSTRT_FG_RSTRT                          (0X0008U)      /* Frame Generator Restart */

#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_NONE              (0X0000U)      /* No description provided */
#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_RANDOM_PAYLOAD    (0X0001U)      /* No description provided */
#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0X00_PAYLOAD      (0X0002U)      /* No description provided */
#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0XFF_PAYLOAD      (0X0003U)      /* No description provided */
#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0X55_PAYLOAD      (0X0004U)      /* No description provided */
#define ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_DECR_PAYLOAD      (0X0005U)      /* No description provided */

/* ----------------------------------------------------------------------------------------------------
          FG_CONT_MODE_EN                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_CONT_MODE_EN_FG_CONT_MODE_EN                  (0U)           /* Frame Generator Continuous Mode Enable */
#define BITL_FG_CONT_MODE_EN_FG_CONT_MODE_EN                  (1U)           /* Frame Generator Continuous Mode Enable */
#define BITM_FG_CONT_MODE_EN_FG_CONT_MODE_EN                  (0X0001U)      /* Frame Generator Continuous Mode Enable */

/* ----------------------------------------------------------------------------------------------------
          FG_IRQ_EN                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_IRQ_EN_FG_IRQ_EN                              (0U)           /* Frame Generator Interrupt Enable */
#define BITL_FG_IRQ_EN_FG_IRQ_EN                              (1U)           /* Frame Generator Interrupt Enable */
#define BITM_FG_IRQ_EN_FG_IRQ_EN                              (0X0001U)      /* Frame Generator Interrupt Enable */

/* ----------------------------------------------------------------------------------------------------
          FG_FRM_LEN                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_FRM_LEN_FG_FRM_LEN                            (0U)           /* The Data Field Frame Length in Bytes */
#define BITL_FG_FRM_LEN_FG_FRM_LEN                            (16U)          /* The Data Field Frame Length in Bytes */
#define BITM_FG_FRM_LEN_FG_FRM_LEN                            (0XFFFFU)      /* The Data Field Frame Length in Bytes */

/* ----------------------------------------------------------------------------------------------------
          FG_IFG_LEN                                            Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_IFG_LEN_FG_IFG_LEN                            (0U)           /* Frame Generator Inter-frame Gap Length */
#define BITL_FG_IFG_LEN_FG_IFG_LEN                            (16U)          /* Frame Generator Inter-frame Gap Length */
#define BITM_FG_IFG_LEN_FG_IFG_LEN                            (0XFFFFU)      /* Frame Generator Inter-frame Gap Length */

/* ----------------------------------------------------------------------------------------------------
          FG_NFRM_H                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_NFRM_H_FG_NFRM_H                              (0U)           /* Bits [31:16] of the Number of Frames to be Generated */
#define BITL_FG_NFRM_H_FG_NFRM_H                              (16U)          /* Bits [31:16] of the Number of Frames to be Generated */
#define BITM_FG_NFRM_H_FG_NFRM_H                              (0XFFFFU)      /* Bits [31:16] of the Number of Frames to be Generated */

/* ----------------------------------------------------------------------------------------------------
          FG_NFRM_L                                             Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_NFRM_L_FG_NFRM_L                              (0U)           /* Bits [15:0] of the Number of Frames to be Generated */
#define BITL_FG_NFRM_L_FG_NFRM_L                              (16U)          /* Bits [15:0] of the Number of Frames to be Generated */
#define BITM_FG_NFRM_L_FG_NFRM_L                              (0XFFFFU)      /* Bits [15:0] of the Number of Frames to be Generated */

/* ----------------------------------------------------------------------------------------------------
          FG_DONE                                               Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_FG_DONE_FG_DONE                                  (0U)           /* Frame Generator Done */
#define BITL_FG_DONE_FG_DONE                                  (1U)           /* Frame Generator Done */
#define BITM_FG_DONE_FG_DONE                                  (0X0001U)      /* Frame Generator Done */

/* ----------------------------------------------------------------------------------------------------
          MAC_IF_LOOPBACK                                       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_IF_LOOPBACK_MAC_IF_LB_EN                     (0U)           /* MAC Interface Loopback Enable. */
#define BITL_MAC_IF_LOOPBACK_MAC_IF_LB_EN                     (1U)           /* MAC Interface Loopback Enable. */
#define BITM_MAC_IF_LOOPBACK_MAC_IF_LB_EN                     (0X0001U)      /* MAC Interface Loopback Enable. */
#define BITP_MAC_IF_LOOPBACK_MAC_IF_LB_TX_SUP_EN              (1U)           /* Suppress Transmission Enable. */
#define BITL_MAC_IF_LOOPBACK_MAC_IF_LB_TX_SUP_EN              (1U)           /* Suppress Transmission Enable. */
#define BITM_MAC_IF_LOOPBACK_MAC_IF_LB_TX_SUP_EN              (0X0002U)      /* Suppress Transmission Enable. */
#define BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN                 (2U)           /* MAC Interface Remote Loopback Enable. */
#define BITL_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN                 (1U)           /* MAC Interface Remote Loopback Enable. */
#define BITM_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN                 (0X0004U)      /* MAC Interface Remote Loopback Enable. */
#define BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_RX_SUP_EN          (3U)           /* Suppress RX Enable */
#define BITL_MAC_IF_LOOPBACK_MAC_IF_REM_LB_RX_SUP_EN          (1U)           /* Suppress RX Enable */
#define BITM_MAC_IF_LOOPBACK_MAC_IF_REM_LB_RX_SUP_EN          (0X0008U)      /* Suppress RX Enable */

/* ----------------------------------------------------------------------------------------------------
          MAC_IF_SOP_CNTRL                                      Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_DET_EN            (0U)           /* Enable the Generation of the Rx SOP Indication Signal. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_DET_EN            (1U)           /* Enable the Generation of the Rx SOP Indication Signal. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_DET_EN            (0X0001U)      /* Enable the Generation of the Rx SOP Indication Signal. */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_SFD_EN            (1U)           /* Enable Rx SOP Signal Indication on SFD Reception. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_SFD_EN            (1U)           /* Enable Rx SOP Signal Indication on SFD Reception. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_SFD_EN            (0X0002U)      /* Enable Rx SOP Signal Indication on SFD Reception. */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_LEN_CHK_EN        (2U)           /* Enable Rx SOP Preamble Length Check. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_LEN_CHK_EN        (1U)           /* Enable Rx SOP Preamble Length Check. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_RX_SOP_LEN_CHK_EN        (0X0004U)      /* Enable Rx SOP Preamble Length Check. */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_DET_EN            (3U)           /* Enable the Generation of the Tx SOP Indication Signal. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_DET_EN            (1U)           /* Enable the Generation of the Tx SOP Indication Signal. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_DET_EN            (0X0008U)      /* Enable the Generation of the Tx SOP Indication Signal. */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_SFD_EN            (4U)           /* Enable Tx SOP Signal Indication on SFD. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_SFD_EN            (1U)           /* Enable Tx SOP Signal Indication on SFD. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_SFD_EN            (0X0010U)      /* Enable Tx SOP Signal Indication on SFD. */
#define BITP_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_LEN_CHK_EN        (5U)           /* Enable Tx SOP Preamble Length Check. */
#define BITL_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_LEN_CHK_EN        (1U)           /* Enable Tx SOP Preamble Length Check. */
#define BITM_MAC_IF_SOP_CNTRL_MAC_IF_TX_SOP_LEN_CHK_EN        (0X0020U)      /* Enable Tx SOP Preamble Length Check. */

#endif  /* end ifndef ADIN1110_ADDR_RDEF_H_ */

#endif /* ADIN1110_ADDR_RDEF_H */

