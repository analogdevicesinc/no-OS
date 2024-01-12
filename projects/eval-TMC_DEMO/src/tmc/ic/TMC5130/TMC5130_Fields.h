/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC5130_FIELDS_H
#define TMC5130_FIELDS_H

#define TMC5130_SW_STOPL_ENABLE_MASK                  0x0001
#define TMC5130_SW_STOPL_ENABLE_SHIFT                 0
#define TMC5130_SW_STOPL_ENABLE_FIELD TMC5130_SW_STOPL_ENABLE_MASK, TMC5130_SW_STOPL_ENABLE_SHIFT
#define TMC5130_SW_STOPR_ENABLE_MASK                  0x0002
#define TMC5130_SW_STOPR_ENABLE_SHIFT                 1
#define TMC5130_SW_STOPR_ENABLE_FIELD TMC5130_SW_STOPR_ENABLE_MASK, TMC5130_SW_STOPR_ENABLE_SHIFT
#define TMC5130_SW_STOPL_POLARITY_MASK                0x0004
#define TMC5130_SW_STOPL_POLARITY_SHIFT               2
#define TMC5130_SW_STOPL_POLARITY_FIELD TMC5130_SW_STOPL_POLARITY_MASK, TMC5130_SW_STOPL_POLARITY_SHIFT
#define TMC5130_SW_STOPR_POLARITY_MASK                0x0008
#define TMC5130_SW_STOPR_POLARITY_SHIFT               3
#define TMC5130_SW_STOPR_POLARITY_FIELD TMC5130_SW_STOPR_POLARITY_MASK, TMC5130_SW_STOPR_POLARITY_SHIFT
#define TMC5130_SW_SWAP_LR_MASK                       0x0010
#define TMC5130_SW_SWAP_LR_SHIFT                      4
#define TMC5130_SW_SWAP_LR_FIELD TMC5130_SW_SWAP_LR_MASK, TMC5130_SW_SWAP_LR_SHIFT
#define TMC5130_SW_LATCH_L_ACT_MASK                   0x0020
#define TMC5130_SW_LATCH_L_ACT_SHIFT                  5
#define TMC5130_SW_LATCH_L_ACT_FIELD TMC5130_SW_LATCH_L_ACT_MASK, TMC5130_SW_LATCH_L_ACT_SHIFT
#define TMC5130_SW_LATCH_L_INACT_MASK                 0x0040
#define TMC5130_SW_LATCH_L_INACT_SHIFT                6
#define TMC5130_SW_LATCH_L_INACT_FIELD TMC5130_SW_LATCH_L_INACT_MASK, TMC5130_SW_LATCH_L_INACT_SHIFT
#define TMC5130_SW_LATCH_R_ACT_MASK                   0x0080
#define TMC5130_SW_LATCH_R_ACT_SHIFT                  7
#define TMC5130_SW_LATCH_R_ACT_FIELD TMC5130_SW_LATCH_R_ACT_MASK, TMC5130_SW_LATCH_R_ACT_SHIFT
#define TMC5130_SW_LATCH_R_INACT_MASK                 0x0100
#define TMC5130_SW_LATCH_R_INACT_SHIFT                8
#define TMC5130_SW_LATCH_R_INACT_FIELD TMC5130_SW_LATCH_R_INACT_MASK, TMC5130_SW_LATCH_R_INACT_SHIFT
#define TMC5130_SW_LATCH_ENC_MASK                     0x0200
#define TMC5130_SW_LATCH_ENC_SHIFT                    9
#define TMC5130_SW_LATCH_ENC_FIELD TMC5130_SW_LATCH_ENC_MASK, TMC5130_SW_LATCH_ENC_SHIFT
#define TMC5130_SW_SG_STOP_MASK                       0x0400
#define TMC5130_SW_SG_STOP_SHIFT                      10
#define TMC5130_SW_SG_STOP_FIELD TMC5130_SW_SG_STOP_MASK, TMC5130_SW_SG_STOP_SHIFT
#define TMC5130_SW_SOFTSTOP_MASK                      0x0800
#define TMC5130_SW_SOFTSTOP_SHIFT                     11
#define TMC5130_SW_SOFTSTOP_FIELD TMC5130_SW_SOFTSTOP_MASK, TMC5130_SW_SOFTSTOP_SHIFT
#define TMC5130_RS_STOPL_MASK                         0x0001
#define TMC5130_RS_STOPL_SHIFT                        0
#define TMC5130_RS_STOPL_FIELD TMC5130_RS_STOPL_MASK, TMC5130_RS_STOPL_SHIFT
#define TMC5130_RS_STOPR_MASK                         0x0002
#define TMC5130_RS_STOPR_SHIFT                        1
#define TMC5130_RS_STOPR_FIELD TMC5130_RS_STOPR_MASK, TMC5130_RS_STOPR_SHIFT
#define TMC5130_RS_LATCHL_MASK                        0x0004
#define TMC5130_RS_LATCHL_SHIFT                       2
#define TMC5130_RS_LATCHL_FIELD TMC5130_RS_LATCHL_MASK, TMC5130_RS_LATCHL_SHIFT
#define TMC5130_RS_LATCHR_MASK                        0x0008
#define TMC5130_RS_LATCHR_SHIFT                       3
#define TMC5130_RS_LATCHR_FIELD TMC5130_RS_LATCHR_MASK, TMC5130_RS_LATCHR_SHIFT
#define TMC5130_RS_EV_STOPL_MASK                      0x0010
#define TMC5130_RS_EV_STOPL_SHIFT                     4
#define TMC5130_RS_EV_STOPL_FIELD TMC5130_RS_EV_STOPL_MASK, TMC5130_RS_EV_STOPL_SHIFT
#define TMC5130_RS_EV_STOPR_MASK                      0x0020
#define TMC5130_RS_EV_STOPR_SHIFT                     5
#define TMC5130_RS_EV_STOPR_FIELD TMC5130_RS_EV_STOPR_MASK, TMC5130_RS_EV_STOPR_SHIFT
#define TMC5130_RS_EV_STOP_SG_MASK                    0x0040
#define TMC5130_RS_EV_STOP_SG_SHIFT                   6
#define TMC5130_RS_EV_STOP_SG_FIELD TMC5130_RS_EV_STOP_SG_MASK, TMC5130_RS_EV_STOP_SG_SHIFT
#define TMC5130_RS_EV_POSREACHED_MASK                 0x0080
#define TMC5130_RS_EV_POSREACHED_SHIFT                7
#define TMC5130_RS_EV_POSREACHED_FIELD TMC5130_RS_EV_POSREACHED_MASK, TMC5130_RS_EV_POSREACHED_SHIFT
#define TMC5130_RS_VELREACHED_MASK                    0x0100
#define TMC5130_RS_VELREACHED_SHIFT                   8
#define TMC5130_RS_VELREACHED_FIELD TMC5130_RS_VELREACHED_MASK, TMC5130_RS_VELREACHED_SHIFT
#define TMC5130_RS_POSREACHED_MASK                    0x0200
#define TMC5130_RS_POSREACHED_SHIFT                   9
#define TMC5130_RS_POSREACHED_FIELD TMC5130_RS_POSREACHED_MASK, TMC5130_RS_POSREACHED_SHIFT
#define TMC5130_RS_VZERO_MASK                         0x0400
#define TMC5130_RS_VZERO_SHIFT                        10
#define TMC5130_RS_VZERO_FIELD TMC5130_RS_VZERO_MASK, TMC5130_RS_VZERO_SHIFT
#define TMC5130_RS_ZEROWAIT_MASK                      0x0800
#define TMC5130_RS_ZEROWAIT_SHIFT                     11
#define TMC5130_RS_ZEROWAIT_FIELD TMC5130_RS_ZEROWAIT_MASK, TMC5130_RS_ZEROWAIT_SHIFT
#define TMC5130_RS_SECONDMOVE_MASK                    0x1000
#define TMC5130_RS_SECONDMOVE_SHIFT                   12
#define TMC5130_RS_SECONDMOVE_FIELD TMC5130_RS_SECONDMOVE_MASK, TMC5130_RS_SECONDMOVE_SHIFT
#define TMC5130_RS_SG_MASK                            0x2000
#define TMC5130_RS_SG_SHIFT                           13
#define TMC5130_RS_SG_FIELD TMC5130_RS_SG_MASK, TMC5130_RS_SG_SHIFT
#define TMC5130_I_SCALE_ANALOG_MASK                   0x01 // GCONF //
#define TMC5130_I_SCALE_ANALOG_SHIFT                  0 // min.: 0, max.: 1, default: 0
#define TMC5130_I_SCALE_ANALOG_FIELD TMC5130_I_SCALE_ANALOG_MASK, TMC5130_I_SCALE_ANALOG_SHIFT
#define TMC5130_INTERNAL_RSENSE_MASK                  0x02 // GCONF //
#define TMC5130_INTERNAL_RSENSE_SHIFT                 1 // min.: 0, max.: 1, default: 0
#define TMC5130_INTERNAL_RSENSE_FIELD TMC5130_INTERNAL_RSENSE_MASK, TMC5130_INTERNAL_RSENSE_SHIFT
#define TMC5130_EN_PWM_MODE_MASK                      0x04 // GCONF //
#define TMC5130_EN_PWM_MODE_SHIFT                     2 // min.: 0, max.: 1, default: 0
#define TMC5130_EN_PWM_MODE_FIELD TMC5130_EN_PWM_MODE_MASK, TMC5130_EN_PWM_MODE_SHIFT
#define TMC5130_ENC_COMMUTATION_MASK                  0x08 // GCONF //
#define TMC5130_ENC_COMMUTATION_SHIFT                 3 // min.: 0, max.: 1, default: 0
#define TMC5130_ENC_COMMUTATION_FIELD TMC5130_ENC_COMMUTATION_MASK, TMC5130_ENC_COMMUTATION_SHIFT
#define TMC5130_SHAFT_MASK                            0x10 // GCONF //
#define TMC5130_SHAFT_SHIFT                           4 // min.: 0, max.: 1, default: 0
#define TMC5130_SHAFT_FIELD TMC5130_SHAFT_MASK, TMC5130_SHAFT_SHIFT
#define TMC5130_DIAG0_ERROR_ONLY_WITH_SD_MODE1_MASK   0x20 // GCONF //
#define TMC5130_DIAG0_ERROR_ONLY_WITH_SD_MODE1_SHIFT  5 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_ERROR_ONLY_WITH_SD_MODE1_FIELD TMC5130_DIAG0_ERROR_ONLY_WITH_SD_MODE1_MASK, TMC5130_DIAG0_ERROR_ONLY_WITH_SD_MODE1_SHIFT
#define TMC5130_DIAG0_OTPW_ONLY_WITH_SD_MODE1_MASK    0x40 // GCONF //
#define TMC5130_DIAG0_OTPW_ONLY_WITH_SD_MODE1_SHIFT   6 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_OTPW_ONLY_WITH_SD_MODE1_FIELD TMC5130_DIAG0_OTPW_ONLY_WITH_SD_MODE1_MASK, TMC5130_DIAG0_OTPW_ONLY_WITH_SD_MODE1_SHIFT
#define TMC5130_DIAG0_STALL_MASK                      0x80 // GCONF //
#define TMC5130_DIAG0_STALL_SHIFT                     7 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_STALL_FIELD TMC5130_DIAG0_STALL_MASK, TMC5130_DIAG0_STALL_SHIFT
#define TMC5130_DIAG1_STALL_MASK                      0x0100 // GCONF //
#define TMC5130_DIAG1_STALL_SHIFT                     8 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_STALL_FIELD TMC5130_DIAG1_STALL_MASK, TMC5130_DIAG1_STALL_SHIFT
#define TMC5130_DIAG1_INDEX_MASK                      0x0200 // GCONF //
#define TMC5130_DIAG1_INDEX_SHIFT                     9 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_INDEX_FIELD TMC5130_DIAG1_INDEX_MASK, TMC5130_DIAG1_INDEX_SHIFT
#define TMC5130_DIAG1_ONSTATE_MASK                    0x0400 // GCONF //
#define TMC5130_DIAG1_ONSTATE_SHIFT                   10 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_ONSTATE_FIELD TMC5130_DIAG1_ONSTATE_MASK, TMC5130_DIAG1_ONSTATE_SHIFT
#define TMC5130_DIAG1_STEPS_SKIPPED_MASK              0x0800 // GCONF //
#define TMC5130_DIAG1_STEPS_SKIPPED_SHIFT             11 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_STEPS_SKIPPED_FIELD TMC5130_DIAG1_STEPS_SKIPPED_MASK, TMC5130_DIAG1_STEPS_SKIPPED_SHIFT
#define TMC5130_DIAG0_INT_PUSHPULL_MASK               0x1000 // GCONF //
#define TMC5130_DIAG0_INT_PUSHPULL_SHIFT              12 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_INT_PUSHPULL_FIELD TMC5130_DIAG0_INT_PUSHPULL_MASK, TMC5130_DIAG0_INT_PUSHPULL_SHIFT
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_MASK           0x2000 // GCONF //
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_SHIFT          13 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_FIELD TMC5130_DIAG1_POSCOMP_PUSHPULL_MASK, TMC5130_DIAG1_POSCOMP_PUSHPULL_SHIFT
#define TMC5130_SMALL_HYSTERESIS_MASK                 0x4000 // GCONF //
#define TMC5130_SMALL_HYSTERESIS_SHIFT                14 // min.: 0, max.: 1, default: 0
#define TMC5130_SMALL_HYSTERESIS_FIELD TMC5130_SMALL_HYSTERESIS_MASK, TMC5130_SMALL_HYSTERESIS_SHIFT
#define TMC5130_STOP_ENABLE_MASK                      0x8000 // GCONF //
#define TMC5130_STOP_ENABLE_SHIFT                     15 // min.: 0, max.: 1, default: 0
#define TMC5130_STOP_ENABLE_FIELD TMC5130_STOP_ENABLE_MASK, TMC5130_STOP_ENABLE_SHIFT
#define TMC5130_DIRECT_MODE_MASK                      0x010000 // GCONF //
#define TMC5130_DIRECT_MODE_SHIFT                     16 // min.: 0, max.: 1, default: 0
#define TMC5130_DIRECT_MODE_FIELD TMC5130_DIRECT_MODE_MASK, TMC5130_DIRECT_MODE_SHIFT
#define TMC5130_TEST_MODE_MASK                        0x020000 // GCONF //
#define TMC5130_TEST_MODE_SHIFT                       17 // min.: 0, max.: 1, default: 0
#define TMC5130_TEST_MODE_FIELD TMC5130_TEST_MODE_MASK, TMC5130_TEST_MODE_SHIFT
#define TMC5130_I_SCALE_ANALOG_MASK                   0x01 // GCONF //
#define TMC5130_I_SCALE_ANALOG_SHIFT                  0 // min.: 0, max.: 1, default: 0
#define TMC5130_I_SCALE_ANALOG_FIELD TMC5130_I_SCALE_ANALOG_MASK, TMC5130_I_SCALE_ANALOG_SHIFT
#define TMC5130_INTERNAL_RSENSE_MASK                  0x02 // GCONF //
#define TMC5130_INTERNAL_RSENSE_SHIFT                 1 // min.: 0, max.: 1, default: 0
#define TMC5130_INTERNAL_RSENSE_FIELD TMC5130_INTERNAL_RSENSE_MASK, TMC5130_INTERNAL_RSENSE_SHIFT
#define TMC5130_EN_PWM_MODE_MASK                      0x04 // GCONF //
#define TMC5130_EN_PWM_MODE_SHIFT                     2 // min.: 0, max.: 1, default: 0
#define TMC5130_EN_PWM_MODE_FIELD TMC5130_EN_PWM_MODE_MASK, TMC5130_EN_PWM_MODE_SHIFT
#define TMC5130_ENC_COMMUTATION_MASK                  0x08 // GCONF //
#define TMC5130_ENC_COMMUTATION_SHIFT                 3 // min.: 0, max.: 1, default: 0
#define TMC5130_ENC_COMMUTATION_FIELD TMC5130_ENC_COMMUTATION_MASK, TMC5130_ENC_COMMUTATION_SHIFT
#define TMC5130_SHAFT_MASK                            0x10 // GCONF //
#define TMC5130_SHAFT_SHIFT                           4 // min.: 0, max.: 1, default: 0
#define TMC5130_SHAFT_FIELD TMC5130_SHAFT_MASK, TMC5130_SHAFT_SHIFT
#define TMC5130_DIAG0_STEP_MASK                       0x80 // GCONF //
#define TMC5130_DIAG0_STEP_SHIFT                      7 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_STEP_FIELD TMC5130_DIAG0_STEP_MASK, TMC5130_DIAG0_STEP_SHIFT
#define TMC5130_DIAG1_DIR_MASK                        0x0100 // GCONF //
#define TMC5130_DIAG1_DIR_SHIFT                       8 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_DIR_FIELD TMC5130_DIAG1_DIR_MASK, TMC5130_DIAG1_DIR_SHIFT
#define TMC5130_DIAG0_INT_PUSHPULL_MASK               0x1000 // GCONF //
#define TMC5130_DIAG0_INT_PUSHPULL_SHIFT              12 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG0_INT_PUSHPULL_FIELD TMC5130_DIAG0_INT_PUSHPULL_MASK, TMC5130_DIAG0_INT_PUSHPULL_SHIFT
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_MASK           0x2000 // GCONF //
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_SHIFT          13 // min.: 0, max.: 1, default: 0
#define TMC5130_DIAG1_POSCOMP_PUSHPULL_FIELD TMC5130_DIAG1_POSCOMP_PUSHPULL_MASK, TMC5130_DIAG1_POSCOMP_PUSHPULL_SHIFT
#define TMC5130_SMALL_HYSTERESIS_MASK                 0x4000 // GCONF //
#define TMC5130_SMALL_HYSTERESIS_SHIFT                14 // min.: 0, max.: 1, default: 0
#define TMC5130_SMALL_HYSTERESIS_FIELD TMC5130_SMALL_HYSTERESIS_MASK, TMC5130_SMALL_HYSTERESIS_SHIFT
#define TMC5130_STOP_ENABLE_MASK                      0x8000 // GCONF //
#define TMC5130_STOP_ENABLE_SHIFT                     15 // min.: 0, max.: 1, default: 0
#define TMC5130_STOP_ENABLE_FIELD TMC5130_STOP_ENABLE_MASK, TMC5130_STOP_ENABLE_SHIFT
#define TMC5130_DIRECT_MODE_MASK                      0x010000 // GCONF //
#define TMC5130_DIRECT_MODE_SHIFT                     16 // min.: 0, max.: 1, default: 0
#define TMC5130_DIRECT_MODE_FIELD TMC5130_DIRECT_MODE_MASK, TMC5130_DIRECT_MODE_SHIFT
#define TMC5130_TEST_MODE_MASK                        0x020000 // GCONF //
#define TMC5130_TEST_MODE_SHIFT                       17 // min.: 0, max.: 1, default: 0
#define TMC5130_TEST_MODE_FIELD TMC5130_TEST_MODE_MASK, TMC5130_TEST_MODE_SHIFT
#define TMC5130_RESET_MASK                            0x01 // GSTAT //
#define TMC5130_RESET_SHIFT                           0 // min.: 0, max.: 1, default: 0
#define TMC5130_RESET_FIELD TMC5130_RESET_MASK, TMC5130_RESET_SHIFT
#define TMC5130_DRV_ERR_MASK                          0x02 // GSTAT //
#define TMC5130_DRV_ERR_SHIFT                         1 // min.: 0, max.: 1, default: 0
#define TMC5130_DRV_ERR_FIELD TMC5130_DRV_ERR_MASK, TMC5130_DRV_ERR_SHIFT
#define TMC5130_UV_CP_MASK                            0x04 // GSTAT //
#define TMC5130_UV_CP_SHIFT                           2 // min.: 0, max.: 1, default: 0
#define TMC5130_UV_CP_FIELD TMC5130_UV_CP_MASK, TMC5130_UV_CP_SHIFT
#define TMC5130_IFCNT_MASK                            0xFF // IFCNT // Interface transmission counter. This register becomes incremented with each successful UART interface write access. Read out to check the serial transmission for lost data. Read accesses do not change the content. The counter wraps around from 255 to 0.
#define TMC5130_IFCNT_SHIFT                           0 // min.: 0, max.: 255, default: 0
#define TMC5130_IFCNT_FIELD TMC5130_IFCNT_MASK, TMC5130_IFCNT_SHIFT
#define TMC5130_SLAVEADDR_MASK                        0xFF // SLAVECONF // These eight bits set the address of unit for the UART interface. The address becomes incremented by one when the external address pin NEXTADDR is active. Range: 0-253 (254 cannot be incremented)
#define TMC5130_SLAVEADDR_SHIFT                       0 // min.: 0, max.: 255, default: 0
#define TMC5130_SLAVEADDR_FIELD TMC5130_SLAVEADDR_MASK, TMC5130_SLAVEADDR_SHIFT
#define TMC5130_SENDDELAY_MASK                        0x0F00 // SLAVECONF // SENDDELAY for read access (time until reply is sent): 0, 1:  8 bit times 2, 3:  3*8 bit times 4, 5:  5*8 bit times 6, 7:  7*8 bit times 8, 9:  9*8 bit times 10, 11: 11*8 bit times 12, 13: 13*8 bit times 14, 15: 15*8 bit times
#define TMC5130_SENDDELAY_SHIFT                       8 // min.: 0, max.: 15, default: 0
#define TMC5130_SENDDELAY_FIELD TMC5130_SENDDELAY_MASK, TMC5130_SENDDELAY_SHIFT
#define TMC5130_REFL_STEP_MASK                        0x01 // IOIN / OUTPUT // REFL_STEP
#define TMC5130_REFL_STEP_SHIFT                       0 // min.: 0, max.: 1, default: 0
#define TMC5130_REFL_STEP_FIELD TMC5130_REFL_STEP_MASK, TMC5130_REFL_STEP_SHIFT
#define TMC5130_REFR_DIR_MASK                         0x02 // IOIN / OUTPUT // REFR_DIR
#define TMC5130_REFR_DIR_SHIFT                        1 // min.: 0, max.: 1, default: 0
#define TMC5130_REFR_DIR_FIELD TMC5130_REFR_DIR_MASK, TMC5130_REFR_DIR_SHIFT
#define TMC5130_ENCB_DCEN_CFG4_MASK                   0x04 // IOIN / OUTPUT // ENCB_DCEN_CFG4
#define TMC5130_ENCB_DCEN_CFG4_SHIFT                  2 // min.: 0, max.: 1, default: 0
#define TMC5130_ENCB_DCEN_CFG4_FIELD TMC5130_ENCB_DCEN_CFG4_MASK, TMC5130_ENCB_DCEN_CFG4_SHIFT
#define TMC5130_ENCA_DCIN_CFG5_MASK                   0x08 // IOIN / OUTPUT // ENCA_DCIN_CFG5
#define TMC5130_ENCA_DCIN_CFG5_SHIFT                  3 // min.: 0, max.: 1, default: 0
#define TMC5130_ENCA_DCIN_CFG5_FIELD TMC5130_ENCA_DCIN_CFG5_MASK, TMC5130_ENCA_DCIN_CFG5_SHIFT
#define TMC5130_DRV_ENN_CFG6_MASK                     0x10 // IOIN / OUTPUT // DRV_ENN_CFG6
#define TMC5130_DRV_ENN_CFG6_SHIFT                    4 // min.: 0, max.: 1, default: 0
#define TMC5130_DRV_ENN_CFG6_FIELD TMC5130_DRV_ENN_CFG6_MASK, TMC5130_DRV_ENN_CFG6_SHIFT
#define TMC5130_ENC_N_DCO_MASK                        0x20 // IOIN / OUTPUT // ENC_N_DCO
#define TMC5130_ENC_N_DCO_SHIFT                       5 // min.: 0, max.: 1, default: 0
#define TMC5130_ENC_N_DCO_FIELD TMC5130_ENC_N_DCO_MASK, TMC5130_ENC_N_DCO_SHIFT
#define TMC5130_SD_MODE_MASK                          0x40 // IOIN / OUTPUT // SD_MODE
#define TMC5130_SD_MODE_SHIFT                         6 // min.: 0, max.: 1, default: 0
#define TMC5130_SD_MODE_FIELD TMC5130_SD_MODE_MASK, TMC5130_SD_MODE_SHIFT
#define TMC5130_SWCOMP_IN_MASK                        0x80 // IOIN / OUTPUT // SWCOMP_IN
#define TMC5130_SWCOMP_IN_SHIFT                       7 // min.: 0, max.: 1, default: 0
#define TMC5130_SWCOMP_IN_FIELD TMC5130_SWCOMP_IN_MASK, TMC5130_SWCOMP_IN_SHIFT
#define TMC5130_VERSION_MASK                          0xFF000000 // IOIN / OUTPUT // VERSION: 0x11=first version of the IC; Identical numbers mean full digital compatibility.
#define TMC5130_VERSION_SHIFT                         24 // min.: 0, max.: 255, default: 0
#define TMC5130_VERSION_FIELD TMC5130_VERSION_MASK, TMC5130_VERSION_SHIFT
#define TMC5130_OUTPUT_PIN_POLARITY_MASK              0x01 // IOIN / OUTPUT // Output polarity of SDO_CFG0 pin in UART mode. Its main purpose it to use SDO_CFG0 as NAO next address output signal for chain addressing of multiple ICs. Attention: Reset Value is 1 for use as NAO to next IC in single wire chain
#define TMC5130_OUTPUT_PIN_POLARITY_SHIFT             0 // min.: 0, max.: 1, default: 0
#define TMC5130_OUTPUT_PIN_POLARITY_FIELD TMC5130_OUTPUT_PIN_POLARITY_MASK, TMC5130_OUTPUT_PIN_POLARITY_SHIFT
#define TMC5130_X_COMPARE_MASK                        0xFFFFFFFF // X_COMPARE // Position comparison register for motion controller position strobe. The Position pulse is available on output SWP_DIAG1.
#define TMC5130_X_COMPARE_SHIFT                       0 // min.: 0, max.: 4294967295, default: 0
#define TMC5130_X_COMPARE_FIELD TMC5130_X_COMPARE_MASK, TMC5130_X_COMPARE_SHIFT
#define TMC5130_IHOLD_MASK                            0x1F // IHOLD_IRUN // Standstill current (0=1/32...31=32/32) In combination with stealthChop mode, setting IHOLD=0 allows to choose freewheeling or coil short circuit (passive braking) for motor stand still.
#define TMC5130_IHOLD_SHIFT                           0 // min.: 0, max.: 31, default: 0
#define TMC5130_IHOLD_FIELD TMC5130_IHOLD_MASK, TMC5130_IHOLD_SHIFT
#define TMC5130_IRUN_MASK                             0x1F00 // IHOLD_IRUN // Motor run current (0=1/32...31=32/32) Hint: Choose sense resistors in a way, that normal IRUN is 16 to 31 for best microstep performance.
#define TMC5130_IRUN_SHIFT                            8 // min.: 0, max.: 31, default: 0
#define TMC5130_IRUN_FIELD TMC5130_IRUN_MASK\, TMC5130_IRUN_SHIFT
#define TMC5130_IHOLDDELAY_MASK                       0x0F0000 // IHOLD_IRUN // Controls the number of clock cycles for motor power down after standstill is detected (stst=1) and TPOWERDOWN has expired. The smooth transition avoids a motor jerk upon power down. 0:  instant power down 1..15:  Delay per current reduction step in multiple of 2^18 clocks
#define TMC5130_IHOLDDELAY_SHIFT                      16 // min.: 0, max.: 15, default: 0
#define TMC5130_IHOLDDELAY_FIELD TMC5130_IHOLDDELAY_MASK, TMC5130_IHOLDDELAY_SHIFT
#define TMC5130_TPOWERDOWN_MASK                       0xFF // TPOWERDOWN // (Reset default=20) Sets the delay time from stand still (stst) detection to motor current power down. Time range is about 0 to 5.6 seconds. 0...((2^8)-1) * 2^18 tclk Attention: A minimum setting of 2 is required to allow automatic tuning of stealthChop PWM_OFFS_AUTO.
#define TMC5130_TPOWERDOWN_SHIFT                      0 // min.: 0, max.: 255, default: 0
#define TMC5130_TPOWERDOWN_FIELD TMC5130_TPOWERDOWN_MASK, TMC5130_TPOWERDOWN_SHIFT
#define TMC5130_TSTEP_MASK                            0x0FFFFF // TSTEP // Actual measured time between two 1/256 microsteps derived from the step input frequency in units of 1/fCLK. Measured value is (2^20)-1 in case of overflow or stand still. The TSTEP related threshold uses a hysteresis of 1/16 of the compare value to compensate for jitter in the clock or the step frequency: (Txxx*15/16)-1 is the lower compare value for each TSTEP based comparison. This means, that the lower switching velocity equals the calculated setting, but the upper switching velocity is higher as defined by the hysteresis setting.
#define TMC5130_TSTEP_SHIFT                           0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_TSTEP_FIELD TMC5130_TSTEP_MASK, TMC5130_TSTEP_SHIFT
#define TMC5130_TPWMTHRS_MASK                         0x0FFFFF // TPWMTHRS // Sets the upper velocity for stealthChop voltage PWM mode.     For TSTEP = TPWMTHRS, stealthChop PWM mode is enabled, if configured. When the velocity exceeds the limit set by TPWMTHRS, the driver switches to spreadCycle. 0 = Disabled
#define TMC5130_TPWMTHRS_SHIFT                        0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_TPWMTHRS_FIELD TMC5130_TPWMTHRS_MASK, TMC5130_TPWMTHRS_SHIFT
#define TMC5130_TCOOLTHRS_MASK                        0x0FFFFF // TCOOLTHRS // This is the lower threshold velocity for switching on smart energy coolStep and stallGuard feature. (unsigned) Set this parameter to disable coolStep at low speeds, where it cannot work reliably. The stop on stall function (enable with sg_stop when using internal motion controller) and the stall output signal become enabled when exceeding this velocity. In non-dcStep mode, it becomes disabled again once the velocity falls below this threshold. TCOOLTHRS = TSTEP = THIGH: - coolStep is enabled, if configured - stealthChop voltage PWM mode is disabled TCOOLTHRS = TSTEP - Stop on stall and stall output signal is enabled, if configured
#define TMC5130_TCOOLTHRS_SHIFT                       0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_TCOOLTHRS_FIELD TMC5130_TCOOLTHRS_MASK, TMC5130_TCOOLTHRS_SHIFT
#define TMC5130_THIGH_MASK                            0x0FFFFF // THIGH // This velocity setting allows velocity dependent switching into a different chopper mode and fullstepping to maximize torque. (unsigned) The stall detection feature becomes switched off for 2-3 electrical periods whenever passing THIGH threshold to compensate for the effect of switching modes. TSTEP = THIGH: - coolStep is disabled (motor runs with normal current scale) - stealthChop voltage PWM mode is disabled - If vhighchm is set, the chopper switches to chm=1 with TFD=0 (constant off time with slow decay, only). - chopSync2 is switched off (SYNC=0) - If vhighfs is set, the motor operates in fullstep mode and the stall detection becomes switched over to dcStep stall detection.
#define TMC5130_THIGH_SHIFT                           0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_THIGH_FIELD TMC5130_THIGH_MASK, TMC5130_THIGH_SHIFT
#define TMC5130_RAMPMODE_MASK                         0x03 // RAMPMODE // 0: Positioning mode (using all A, D and V parameters) 1: Velocity mode to positive VMAX (using AMAX acceleration) 2: Velocity mode to negative VMAX (using AMAX acceleration) 3: Hold mode (velocity remains unchanged, unless stop event occurs)
#define TMC5130_RAMPMODE_SHIFT                        0 // min.: 0, max.: 3, default: 0
#define TMC5130_RAMPMODE_FIELD TMC5130_RAMPMODE_MASK, TMC5130_RAMPMODE_SHIFT
#define TMC5130_XACTUAL_MASK                          0xFFFFFFFF // XACTUAL // Actual motor position (signed) Hint: This value normally should only be modified, when homing the drive. In positioning mode, modifying the register content will start a motion.
#define TMC5130_XACTUAL_SHIFT                         0 // min.: -2147483648, max.: 2147483647, default: 0
#define TMC5130_XACTUAL_FIELD TMC5130_XACTUAL_MASK, TMC5130_XACTUAL_SHIFT
#define TMC5130_VACTUAL_MASK                          0xFFFFFF // VACTUAL // Actual motor velocity from ramp generator (signed) The sign matches the motion direction. A negative sign means motion to lower XACTUAL.
#define TMC5130_VACTUAL_SHIFT                         0 // min.: -8388607, max.: 8388607, default: 0
#define TMC5130_VACTUAL_FIELD TMC5130_VACTUAL_MASK, TMC5130_VACTUAL_SHIFT
#define TMC5130_VSTART_MASK                           0x03FFFF // VSTART // Motor start velocity (unsigned) Set VSTOP = VSTART!
#define TMC5130_VSTART_SHIFT                          0 // min.: 0, max.: 262143, default: 0
#define TMC5130_VSTART_FIELD TMC5130_VSTART_MASK, TMC5130_VSTART_SHIFT
#define TMC5130_A1_MASK                               0xFFFF // A1 // First acceleration between VSTART and V1 (unsigned)
#define TMC5130_A1_SHIFT                              0 // min.: 0, max.: 65535, default: 0
#define TMC5130_A1_FIELD TMC5130_A1_MASK, TMC5130_A1_SHIFT
#define TMC5130_V1_MASK                               0x0FFFFF // V1 // First acceleration / deceleration phase threshold velocity (unsigned) 0: Disables A1 and D1 phase, use AMAX, DMAX only
#define TMC5130_V1_SHIFT                              0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_V1_FIELD TMC5130_V1_MASK, TMC5130_V1_SHIFT
#define TMC5130_AMAX_MASK                             0xFFFF // AMAX // Second acceleration between V1 and VMAX (unsigned) This is the acceleration and deceleration value for velocity mode.
#define TMC5130_AMAX_SHIFT                            0 // min.: 0, max.: 65535, default: 0
#define TMC5130_AMAX_FIELD TMC5130_AMAX_MASK, TMC5130_AMAX_SHIFT
#define TMC5130_VMAX_MASK                             0x7FFFFF // VMAX // Motion ramp target velocity (for positioning ensure VMAX = VSTART) (unsigned) This is the target velocity in velocity mode. It can be changed any time during a motion.
#define TMC5130_VMAX_SHIFT                            0 // min.: 0, max.: 8388096, default: 0
#define TMC5130_VMAX_FIELD TMC5130_VMAX_MASK, TMC5130_VMAX_SHIFT
#define TMC5130_DMAX_MASK                             0xFFFF // DMAX // Deceleration between VMAX and V1 (unsigned)
#define TMC5130_DMAX_SHIFT                            0 // min.: 0, max.: 65535, default: 0
#define TMC5130_DMAX_FIELD TMC5130_DMAX_MASK, TMC5130_DMAX_SHIFT
#define TMC5130_D1_MASK                               0xFFFF // D1 // Deceleration between V1 and VSTOP (unsigned) Attention: Do not set 0 in positioning mode, even if V1=0!
#define TMC5130_D1_SHIFT                              0 // min.: 1, max.: 65535, default: 0
#define TMC5130_D1_FIELD TMC5130_D1_MASK, TMC5130_D1_SHIFT
#define TMC5130_VSTOP_MASK                            0x03FFFF // VSTOP // Motor stop velocity (unsigned) Attention: Set VSTOP = VSTART! Attention: Do not set 0 in positioning mode, minimum 10 recommend!
#define TMC5130_VSTOP_SHIFT                           0 // min.: 1, max.: 262143, default: 0
#define TMC5130_VSTOP_FIELD TMC5130_VSTOP_MASK, TMC5130_VSTOP_SHIFT
#define TMC5130_TZEROWAIT_MASK                        0xFFFF // TZEROWAIT // Defines the waiting time after ramping down to zero velocity before next movement or direction inversion can start. Time range is about 0 to 2 seconds. This setting avoids excess acceleration e.g. from VSTOP to -VSTART.
#define TMC5130_TZEROWAIT_SHIFT                       0 // min.: 0, max.: 65535, default: 0
#define TMC5130_TZEROWAIT_FIELD TMC5130_TZEROWAIT_MASK, TMC5130_TZEROWAIT_SHIFT
#define TMC5130_XTARGET_MASK                          0xFFFFFFFF // XTARGET // Target position for ramp mode (signed). Write a new target position to this register in order to activate the ramp generator positioning in RAMPMODE=0. Initialize all velocity, acceleration and deceleration parameters before. Hint: The position is allowed to wrap around, thus, XTARGET value optionally can be treated as an unsigned number. Hint: The maximum possible displacement is +/-((2^31)-1). Hint: When increasing V1, D1 or DMAX during a motion, rewrite XTARGET afterwards in order to trigger a second acceleration phase, if desired.
#define TMC5130_XTARGET_SHIFT                         0 // min.: -2147483648, max.: 2147483647, default: 0
#define TMC5130_XTARGET_FIELD TMC5130_XTARGET_MASK, TMC5130_XTARGET_SHIFT
#define TMC5130_VDCMIN_MASK                           0x7FFFFF // VDCMIN // Automatic commutation dcStep becomes enabled above velocity VDCMIN (unsigned) (only when using internal ramp generator, not for STEP/DIR interface - in STEP/DIR mode, dcStep becomes enabled by the external signal DCEN) In this mode, the actual position is determined by the sensorless motor commutation and becomes fed back to XACTUAL. In case the motor becomes heavily loaded, VDCMIN also is used as the minimum step velocity. Activate stop on stall (sg_stop) to detect step loss. 0: Disable, dcStep off |VACT| = VDCMIN = 256: - Triggers the same actions as exceeding THIGH setting. - Switches on automatic commutation dcStep Hint: Also set DCCTRL parameters in order to operate dcStep. (Only bits 22 - 8 are used for value and for comparison)
#define TMC5130_VDCMIN_SHIFT                          0 // min.: 0, max.: 8388607, default: 0
#define TMC5130_VDCMIN_FIELD TMC5130_VDCMIN_MASK, TMC5130_VDCMIN_SHIFT
#define TMC5130_STOP_L_ENABLE_MASK                    0x01 // SW_MODE //
#define TMC5130_STOP_L_ENABLE_SHIFT                   0 // min.: 0, max.: 1, default: 0
#define TMC5130_STOP_L_ENABLE_FIELD TMC5130_STOP_L_ENABLE_MASK, TMC5130_STOP_L_ENABLE_SHIFT
#define TMC5130_STOP_R_ENABLE_MASK                    0x02 // SW_MODE //
#define TMC5130_STOP_R_ENABLE_SHIFT                   1 // min.: 0, max.: 1, default: 0
#define TMC5130_STOP_R_ENABLE_FIELD TMC5130_STOP_R_ENABLE_MASK, TMC5130_STOP_R_ENABLE_SHIFT
#define TMC5130_POL_STOP_L_MASK                       0x04 // SW_MODE // Sets the active polarity of the left reference switch input
#define TMC5130_POL_STOP_L_SHIFT                      2 // min.: 0, max.: 1, default: 0
#define TMC5130_POL_STOP_L_FIELD TMC5130_POL_STOP_L_MASK, TMC5130_POL_STOP_L_SHIFT
#define TMC5130_POL_STOP_R_MASK                       0x08 // SW_MODE // Sets the active polarity of the right reference switch input
#define TMC5130_POL_STOP_R_SHIFT                      3 // min.: 0, max.: 1, default: 0
#define TMC5130_POL_STOP_R_FIELD TMC5130_POL_STOP_R_MASK, TMC5130_POL_STOP_R_SHIFT
#define TMC5130_SWAP_LR_MASK                          0x10 // SW_MODE //
#define TMC5130_SWAP_LR_SHIFT                         4 // min.: 0, max.: 1, default: 0
#define TMC5130_SWAP_LR_FIELD TMC5130_SWAP_LR_MASK, TMC5130_SWAP_LR_SHIFT
#define TMC5130_LATCH_L_ACTIVE_MASK                   0x20 // SW_MODE //
#define TMC5130_LATCH_L_ACTIVE_SHIFT                  5 // min.: 0, max.: 1, default: 0
#define TMC5130_LATCH_L_ACTIVE_FIELD TMC5130_LATCH_L_ACTIVE_MASK, TMC5130_LATCH_L_ACTIVE_SHIFT
#define TMC5130_LATCH_L_INACTIVE_MASK                 0x40 // SW_MODE //
#define TMC5130_LATCH_L_INACTIVE_SHIFT                6 // min.: 0, max.: 1, default: 0
#define TMC5130_LATCH_L_INACTIVE_FIELD TMC5130_LATCH_L_INACTIVE_MASK, TMC5130_LATCH_L_INACTIVE_SHIFT
#define TMC5130_LATCH_R_ACTIVE_MASK                   0x80 // SW_MODE //
#define TMC5130_LATCH_R_ACTIVE_SHIFT                  7 // min.: 0, max.: 1, default: 0
#define TMC5130_LATCH_R_ACTIVE_FIELD TMC5130_LATCH_R_ACTIVE_MASK, TMC5130_LATCH_R_ACTIVE_SHIFT
#define TMC5130_LATCH_R_INACTIVE_MASK                 0x0100 // SW_MODE //
#define TMC5130_LATCH_R_INACTIVE_SHIFT                8 // min.: 0, max.: 1, default: 0
#define TMC5130_LATCH_R_INACTIVE_FIELD TMC5130_LATCH_R_INACTIVE_MASK, TMC5130_LATCH_R_INACTIVE_SHIFT
#define TMC5130_EN_LATCH_ENCODER_MASK                 0x0200 // SW_MODE //
#define TMC5130_EN_LATCH_ENCODER_SHIFT                9 // min.: 0, max.: 1, default: 0
#define TMC5130_EN_LATCH_ENCODER_FIELD TMC5130_EN_LATCH_ENCODER_MASK, TMC5130_EN_LATCH_ENCODER_SHIFT
#define TMC5130_SG_STOP_MASK                          0x0400 // SW_MODE //
#define TMC5130_SG_STOP_SHIFT                         10 // min.: 0, max.: 1, default: 0
#define TMC5130_SG_STOP_FIELD TMC5130_SG_STOP_MASK, TMC5130_SG_STOP_SHIFT
#define TMC5130_EN_SOFTSTOP_MASK                      0x0800 // SW_MODE // The soft stop mode always uses the deceleration ramp settings DMAX, V1, D1, VSTOP and TZEROWAIT for stopping the motor. A stop occurs when the velocity sign matches the reference switch position (REFL for negative velocities, REFR for positive velocities) and the respective switch stop function is enabled. A hard stop also uses TZEROWAIT before the motor becomes released. Attention: Do not use soft stop in combination with stallGuard2.
#define TMC5130_EN_SOFTSTOP_SHIFT                     11 // min.: 0, max.: 1, default: 0
#define TMC5130_EN_SOFTSTOP_FIELD TMC5130_EN_SOFTSTOP_MASK, TMC5130_EN_SOFTSTOP_SHIFT
#define TMC5130_STATUS_STOP_L_MASK                    0x01 // RAMP_STAT //
#define TMC5130_STATUS_STOP_L_SHIFT                   0 // min.: 0, max.: 1, default: 0
#define TMC5130_STATUS_STOP_L_FIELD TMC5130_STATUS_STOP_L_MASK, TMC5130_STATUS_STOP_L_SHIFT
#define TMC5130_STATUS_STOP_R_MASK                    0x02 // RAMP_STAT //
#define TMC5130_STATUS_STOP_R_SHIFT                   1 // min.: 0, max.: 1, default: 0
#define TMC5130_STATUS_STOP_R_FIELD TMC5130_STATUS_STOP_R_MASK, TMC5130_STATUS_STOP_R_SHIFT
#define TMC5130_STATUS_LATCH_L_MASK                   0x04 // RAMP_STAT //
#define TMC5130_STATUS_LATCH_L_SHIFT                  2 // min.: 0, max.: 1, default: 0
#define TMC5130_STATUS_LATCH_L_FIELD TMC5130_STATUS_LATCH_L_MASK, TMC5130_STATUS_LATCH_L_SHIFT
#define TMC5130_STATUS_LATCH_R_MASK                   0x08 // RAMP_STAT //
#define TMC5130_STATUS_LATCH_R_SHIFT                  3 // min.: 0, max.: 1, default: 0
#define TMC5130_STATUS_LATCH_R_FIELD TMC5130_STATUS_LATCH_R_MASK, TMC5130_STATUS_LATCH_R_SHIFT
#define TMC5130_EVENT_STOP_L_MASK                     0x10 // RAMP_STAT //
#define TMC5130_EVENT_STOP_L_SHIFT                    4 // min.: 0, max.: 1, default: 0
#define TMC5130_EVENT_STOP_L_FIELD TMC5130_EVENT_STOP_L_MASK, TMC5130_EVENT_STOP_L_SHIFT
#define TMC5130_EVENT_STOP_R_MASK                     0x20 // RAMP_STAT //
#define TMC5130_EVENT_STOP_R_SHIFT                    5 // min.: 0, max.: 1, default: 0
#define TMC5130_EVENT_STOP_R_FIELD TMC5130_EVENT_STOP_R_MASK, TMC5130_EVENT_STOP_R_SHIFT
#define TMC5130_EVENT_STOP_SG_MASK                    0x40 // RAMP_STAT //
#define TMC5130_EVENT_STOP_SG_SHIFT                   6 // min.: 0, max.: 1, default: 0
#define TMC5130_EVENT_STOP_SG_FIELD TMC5130_EVENT_STOP_SG_MASK, TMC5130_EVENT_STOP_SG_SHIFT
#define TMC5130_EVENT_POS_REACHED_MASK                0x80 // RAMP_STAT //
#define TMC5130_EVENT_POS_REACHED_SHIFT               7 // min.: 0, max.: 1, default: 0
#define TMC5130_EVENT_POS_REACHED_FIELD TMC5130_EVENT_POS_REACHED_MASK, TMC5130_EVENT_POS_REACHED_SHIFT
#define TMC5130_VELOCITY_REACHED_MASK                 0x0100 // RAMP_STAT //
#define TMC5130_VELOCITY_REACHED_SHIFT                8 // min.: 0, max.: 1, default: 0
#define TMC5130_VELOCITY_REACHED_FIELD TMC5130_VELOCITY_REACHED_MASK, TMC5130_VELOCITY_REACHED_SHIFT
#define TMC5130_POSITION_REACHED_MASK                 0x0200 // RAMP_STAT //
#define TMC5130_POSITION_REACHED_SHIFT                9 // min.: 0, max.: 1, default: 0
#define TMC5130_POSITION_REACHED_FIELD TMC5130_POSITION_REACHED_MASK, TMC5130_POSITION_REACHED_SHIFT
#define TMC5130_VZERO_MASK                            0x0400 // RAMP_STAT //
#define TMC5130_VZERO_SHIFT                           10 // min.: 0, max.: 1, default: 0
#define TMC5130_VZERO_FIELD TMC5130_VZERO_MASK, TMC5130_VZERO_SHIFT
#define TMC5130_T_ZEROWAIT_ACTIVE_MASK                0x0800 // RAMP_STAT //
#define TMC5130_T_ZEROWAIT_ACTIVE_SHIFT               11 // min.: 0, max.: 1, default: 0
#define TMC5130_T_ZEROWAIT_ACTIVE_FIELD TMC5130_T_ZEROWAIT_ACTIVE_MASK, TMC5130_T_ZEROWAIT_ACTIVE_SHIFT
#define TMC5130_SECOND_MOVE_MASK                      0x1000 // RAMP_STAT //
#define TMC5130_SECOND_MOVE_SHIFT                     12 // min.: 0, max.: 1, default: 0
#define TMC5130_SECOND_MOVE_FIELD TMC5130_SECOND_MOVE_MASK, TMC5130_SECOND_MOVE_SHIFT
#define TMC5130_STATUS_SG_MASK                        0x2000 // RAMP_STAT //
#define TMC5130_STATUS_SG_SHIFT                       13 // min.: 0, max.: 1, default: 0
#define TMC5130_STATUS_SG_FIELD TMC5130_STATUS_SG_MASK, TMC5130_STATUS_SG_SHIFT
#define TMC5130_XLATCH_MASK                           0xFFFFFFFF // XLATCH // Ramp generator latch position, latches XACTUAL upon a programmable switch event (see SW_MODE). Hint: The encoder position can be latched to ENC_LATCH together with XLATCH to allow consistency checks.
#define TMC5130_XLATCH_SHIFT                          0 // min.: 0, max.: 4294967295, default: 0
#define TMC5130_XLATCH_FIELD TMC5130_XLATCH_MASK, TMC5130_XLATCH_SHIFT
#define TMC5130_POL_A_MASK                            0x01 // ENCMODE // Required A polarity for an N channel event
#define TMC5130_POL_A_SHIFT                           0 // min.: 0, max.: 1, default: 0
#define TMC5130_POL_A_FIELD TMC5130_POL_A_MASK, TMC5130_POL_A_SHIFT
#define TMC5130_POL_B_MASK                            0x02 // ENCMODE // Required B polarity for an N channel event
#define TMC5130_POL_B_SHIFT                           1 // min.: 0, max.: 1, default: 0
#define TMC5130_POL_B_FIELD TMC5130_POL_B_MASK, TMC5130_POL_B_SHIFT
#define TMC5130_POL_N_MASK                            0x04 // ENCMODE // Defines active polarity of N
#define TMC5130_POL_N_SHIFT                           2 // min.: 0, max.: 1, default: 0
#define TMC5130_POL_N_FIELD TMC5130_POL_N_MASK, TMC5130_POL_N_SHIFT
#define TMC5130_IGNORE_AB_MASK                        0x08 // ENCMODE //
#define TMC5130_IGNORE_AB_SHIFT                       3 // min.: 0, max.: 1, default: 0
#define TMC5130_IGNORE_AB_FIELD TMC5130_IGNORE_AB_MASK, TMC5130_IGNORE_AB_SHIFT
#define TMC5130_CLR_CONT_MASK                         0x10 // ENCMODE //
#define TMC5130_CLR_CONT_SHIFT                        4 // min.: 0, max.: 1, default: 0
#define TMC5130_CLR_CONT_FIELD TMC5130_CLR_CONT_MASK, TMC5130_CLR_CONT_SHIFT
#define TMC5130_CLR_ONCE_MASK                         0x20 // ENCMODE //
#define TMC5130_CLR_ONCE_SHIFT                        5 // min.: 0, max.: 1, default: 0
#define TMC5130_CLR_ONCE_FIELD TMC5130_CLR_ONCE_MASK, TMC5130_CLR_ONCE_SHIFT
#define TMC5130_POS_EDGENEG_EDGE_MASK                 0xC0 // ENCMODE //
#define TMC5130_POS_EDGENEG_EDGE_SHIFT                6 // min.: 0, max.: 3, default: 0
#define TMC5130_POS_EDGENEG_EDGE_FIELD TMC5130_POS_EDGENEG_EDGE_MASK, TMC5130_POS_EDGENEG_EDGE_SHIFT
#define TMC5130_CLR_ENC_X_MASK                        0x0100 // ENCMODE //
#define TMC5130_CLR_ENC_X_SHIFT                       8 // min.: 0, max.: 1, default: 0
#define TMC5130_CLR_ENC_X_FIELD TMC5130_CLR_ENC_X_MASK, TMC5130_CLR_ENC_X_SHIFT
#define TMC5130_LATCH_X_ACT_MASK                      0x0200 // ENCMODE //
#define TMC5130_LATCH_X_ACT_SHIFT                     9 // min.: 0, max.: 1, default: 0
#define TMC5130_LATCH_X_ACT_FIELD TMC5130_LATCH_X_ACT_MASK, TMC5130_LATCH_X_ACT_SHIFT
#define TMC5130_ENC_SEL_DECIMAL_MASK                  0x0400 // ENCMODE //
#define TMC5130_ENC_SEL_DECIMAL_SHIFT                 10 // min.: 0, max.: 1, default: 0
#define TMC5130_ENC_SEL_DECIMAL_FIELD TMC5130_ENC_SEL_DECIMAL_MASK, TMC5130_ENC_SEL_DECIMAL_SHIFT
#define TMC5130_X_ENC_MASK                            0xFFFFFFFF // X_ENC // Actual encoder position
#define TMC5130_X_ENC_SHIFT                           0 // min.: -2147483648, max.: 2147483647, default: 0
#define TMC5130_X_ENC_FIELD TMC5130_X_ENC_MASK, TMC5130_X_ENC_SHIFT
#define TMC5130_INTEGER_MASK                          0xFFFF0000 // ENC_CONST // Integer part of encoder constant
#define TMC5130_INTEGER_SHIFT                         16 // min.: 0, max.: 65535, default: 0
#define TMC5130_INTEGER_FIELD TMC5130_INTEGER_MASK, TMC5130_INTEGER_SHIFT
#define TMC5130_FRACTIONAL_MASK                       0xFFFF // ENC_CONST // Fractional part of encoder constant
#define TMC5130_FRACTIONAL_SHIFT                      0 // min.: 0, max.: 65535, default: 0
#define TMC5130_FRACTIONAL_FIELD TMC5130_FRACTIONAL_MASK, TMC5130_FRACTIONAL_SHIFT
#define TMC5130_ENC_STATUS_MASK                       0x01 // ENC_STATUS // Encoder status
#define TMC5130_ENC_STATUS_SHIFT                      0 // min.: 0, max.: 1, default: 0
#define TMC5130_ENC_STATUS_FIELD TMC5130_ENC_STATUS_MASK, TMC5130_ENC_STATUS_SHIFT
#define TMC5130_ENC_LATCH_MASK                        0xFFFFFFFF // ENC_LATCH // Encoder position X_ENC latched on N event
#define TMC5130_ENC_LATCH_SHIFT                       0 // min.: -2147483648, max.: 2147483647, default: 0
#define TMC5130_ENC_LATCH_FIELD TMC5130_ENC_LATCH_MASK, TMC5130_ENC_LATCH_SHIFT
#define TMC5130_OFS0_MASK                             0x01 // MSLUT[0] // microstep table entry 0
#define TMC5130_OFS0_SHIFT                            0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS0_FIELD TMC5130_OFS0_MASK, TMC5130_OFS0_SHIFT
#define TMC5130_OFS1_MASK                             0x02 // MSLUT[0] // microstep table entry 1
#define TMC5130_OFS1_SHIFT                            1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS1_FIELD TMC5130_OFS1_MASK, TMC5130_OFS1_SHIFT
#define TMC5130_OFS2_MASK                             0x04 // MSLUT[0] // microstep table entry 2
#define TMC5130_OFS2_SHIFT                            2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS2_FIELD TMC5130_OFS2_MASK, TMC5130_OFS2_SHIFT
#define TMC5130_OFS3_MASK                             0x08 // MSLUT[0] // microstep table entry 3
#define TMC5130_OFS3_SHIFT                            3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS3_FIELD TMC5130_OFS3_MASK, TMC5130_OFS3_SHIFT
#define TMC5130_OFS4_MASK                             0x10 // MSLUT[0] // microstep table entry 4
#define TMC5130_OFS4_SHIFT                            4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS4_FIELD TMC5130_OFS4_MASK, TMC5130_OFS4_SHIFT
#define TMC5130_OFS5_MASK                             0x20 // MSLUT[0] // microstep table entry 5
#define TMC5130_OFS5_SHIFT                            5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS5_FIELD TMC5130_OFS5_MASK, TMC5130_OFS5_SHIFT
#define TMC5130_OFS6_MASK                             0x40 // MSLUT[0] // microstep table entry 6
#define TMC5130_OFS6_SHIFT                            6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS6_FIELD TMC5130_OFS6_MASK, TMC5130_OFS6_SHIFT
#define TMC5130_OFS7_MASK                             0x80 // MSLUT[0] // microstep table entry 7
#define TMC5130_OFS7_SHIFT                            7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS7_FIELD TMC5130_OFS7_MASK, TMC5130_OFS7_SHIFT
#define TMC5130_OFS8_MASK                             0x0100 // MSLUT[0] // microstep table entry 8
#define TMC5130_OFS8_SHIFT                            8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS8_FIELD TMC5130_OFS8_MASK, TMC5130_OFS8_SHIFT
#define TMC5130_OFS9_MASK                             0x0200 // MSLUT[0] // microstep table entry 9
#define TMC5130_OFS9_SHIFT                            9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS9_FIELD TMC5130_OFS9_MASK, TMC5130_OFS9_SHIFT
#define TMC5130_OFS10_MASK                            0x0400 // MSLUT[0] // microstep table entry 10
#define TMC5130_OFS10_SHIFT                           10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS10_FIELD TMC5130_OFS10_MASK, TMC5130_OFS10_SHIFT
#define TMC5130_OFS11_MASK                            0x0800 // MSLUT[0] // microstep table entry 11
#define TMC5130_OFS11_SHIFT                           11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS11_FIELD TMC5130_OFS11_MASK, TMC5130_OFS11_SHIFT
#define TMC5130_OFS12_MASK                            0x1000 // MSLUT[0] // microstep table entry 12
#define TMC5130_OFS12_SHIFT                           12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS12_FIELD TMC5130_OFS12_MASK, TMC5130_OFS12_SHIFT
#define TMC5130_OFS13_MASK                            0x2000 // MSLUT[0] // microstep table entry 13
#define TMC5130_OFS13_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS13_FIELD TMC5130_OFS13_MASK, TMC5130_OFS13_SHIFT
#define TMC5130_OFS14_MASK                            0x4000 // MSLUT[0] // microstep table entry 14
#define TMC5130_OFS14_SHIFT                           14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS14_FIELD TMC5130_OFS14_MASK, TMC5130_OFS14_SHIFT
#define TMC5130_OFS15_MASK                            0x8000 // MSLUT[0] // microstep table entry 15
#define TMC5130_OFS15_SHIFT                           15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS15_FIELD TMC5130_OFS15_MASK, TMC5130_OFS15_SHIFT
#define TMC5130_OFS16_MASK                            0x010000 // MSLUT[0] // microstep table entry 16
#define TMC5130_OFS16_SHIFT                           16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS16_FIELD TMC5130_OFS16_MASK, TMC5130_OFS16_SHIFT
#define TMC5130_OFS17_MASK                            0x020000 // MSLUT[0] // microstep table entry 17
#define TMC5130_OFS17_SHIFT                           17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS17_FIELD TMC5130_OFS17_MASK, TMC5130_OFS17_SHIFT
#define TMC5130_OFS18_MASK                            0x040000 // MSLUT[0] // microstep table entry 18
#define TMC5130_OFS18_SHIFT                           18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS18_FIELD TMC5130_OFS18_MASK, TMC5130_OFS18_SHIFT
#define TMC5130_OFS19_MASK                            0x080000 // MSLUT[0] // microstep table entry 19
#define TMC5130_OFS19_SHIFT                           19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS19_FIELD TMC5130_OFS19_MASK, TMC5130_OFS19_SHIFT
#define TMC5130_OFS20_MASK                            0x100000 // MSLUT[0] // microstep table entry 20
#define TMC5130_OFS20_SHIFT                           20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS20_FIELD TMC5130_OFS20_MASK, TMC5130_OFS20_SHIFT
#define TMC5130_OFS21_MASK                            0x200000 // MSLUT[0] // microstep table entry 21
#define TMC5130_OFS21_SHIFT                           21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS21_FIELD TMC5130_OFS21_MASK, TMC5130_OFS21_SHIFT
#define TMC5130_OFS22_MASK                            0x400000 // MSLUT[0] // microstep table entry 22
#define TMC5130_OFS22_SHIFT                           22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS22_FIELD TMC5130_OFS22_MASK, TMC5130_OFS22_SHIFT
#define TMC5130_OFS23_MASK                            0x800000 // MSLUT[0] // microstep table entry 23
#define TMC5130_OFS23_SHIFT                           23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS23_FIELD TMC5130_OFS23_MASK, TMC5130_OFS23_SHIFT
#define TMC5130_OFS24_MASK                            0x01000000 // MSLUT[0] // microstep table entry 24
#define TMC5130_OFS24_SHIFT                           24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS24_FIELD TMC5130_OFS24_MASK, TMC5130_OFS24_SHIFT
#define TMC5130_OFS25_MASK                            0x02000000 // MSLUT[0] // microstep table entry 25
#define TMC5130_OFS25_SHIFT                           25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS25_FIELD TMC5130_OFS25_MASK, TMC5130_OFS25_SHIFT
#define TMC5130_OFS26_MASK                            0x04000000 // MSLUT[0] // microstep table entry 26
#define TMC5130_OFS26_SHIFT                           26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS26_FIELD TMC5130_OFS26_MASK, TMC5130_OFS26_SHIFT
#define TMC5130_OFS27_MASK                            0x08000000 // MSLUT[0] // microstep table entry 27
#define TMC5130_OFS27_SHIFT                           27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS27_FIELD TMC5130_OFS27_MASK, TMC5130_OFS27_SHIFT
#define TMC5130_OFS28_MASK                            0x10000000 // MSLUT[0] // microstep table entry 28
#define TMC5130_OFS28_SHIFT                           28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS28_FIELD TMC5130_OFS28_MASK, TMC5130_OFS28_SHIFT
#define TMC5130_OFS29_MASK                            0x20000000 // MSLUT[0] // microstep table entry 29
#define TMC5130_OFS29_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS29_FIELD TMC5130_OFS29_MASK, TMC5130_OFS29_SHIFT
#define TMC5130_OFS30_MASK                            0x40000000 // MSLUT[0] // microstep table entry 30
#define TMC5130_OFS30_SHIFT                           30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS30_FIELD TMC5130_OFS30_MASK, TMC5130_OFS30_SHIFT
#define TMC5130_OFS31_MASK                            0x80000000 // MSLUT[0] // microstep table entry 31
#define TMC5130_OFS31_SHIFT                           31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS31_FIELD TMC5130_OFS31_MASK, TMC5130_OFS31_SHIFT
#define TMC5130_OFS32_MASK                            0x01 // MSLUT[1] // microstep table entry 32
#define TMC5130_OFS32_SHIFT                           0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS32_FIELD TMC5130_OFS32_MASK, TMC5130_OFS32_SHIFT
#define TMC5130_OFS33_MASK                            0x02 // MSLUT[1] // microstep table entry 33
#define TMC5130_OFS33_SHIFT                           1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS33_FIELD TMC5130_OFS33_MASK, TMC5130_OFS33_SHIFT
#define TMC5130_OFS34_MASK                            0x04 // MSLUT[1] // microstep table entry 34
#define TMC5130_OFS34_SHIFT                           2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS34_FIELD TMC5130_OFS34_MASK, TMC5130_OFS34_SHIFT
#define TMC5130_OFS35_MASK                            0x08 // MSLUT[1] // microstep table entry 35
#define TMC5130_OFS35_SHIFT                           3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS35_FIELD TMC5130_OFS35_MASK, TMC5130_OFS35_SHIFT
#define TMC5130_OFS36_MASK                            0x10 // MSLUT[1] // microstep table entry 36
#define TMC5130_OFS36_SHIFT                           4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS36_FIELD TMC5130_OFS36_MASK, TMC5130_OFS36_SHIFT
#define TMC5130_OFS37_MASK                            0x20 // MSLUT[1] // microstep table entry 37
#define TMC5130_OFS37_SHIFT                           5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS37_FIELD TMC5130_OFS37_MASK, TMC5130_OFS37_SHIFT
#define TMC5130_OFS38_MASK                            0x40 // MSLUT[1] // microstep table entry 38
#define TMC5130_OFS38_SHIFT                           6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS38_FIELD TMC5130_OFS38_MASK, TMC5130_OFS38_SHIFT
#define TMC5130_OFS39_MASK                            0x80 // MSLUT[1] // microstep table entry 39
#define TMC5130_OFS39_SHIFT                           7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS39_FIELD TMC5130_OFS39_MASK, TMC5130_OFS39_SHIFT
#define TMC5130_OFS40_MASK                            0x0100 // MSLUT[1] // microstep table entry 40
#define TMC5130_OFS40_SHIFT                           8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS40_FIELD TMC5130_OFS40_MASK, TMC5130_OFS40_SHIFT
#define TMC5130_OFS41_MASK                            0x0200 // MSLUT[1] // microstep table entry 41
#define TMC5130_OFS41_SHIFT                           9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS41_FIELD TMC5130_OFS41_MASK, TMC5130_OFS41_SHIFT
#define TMC5130_OFS42_MASK                            0x0400 // MSLUT[1] // microstep table entry 42
#define TMC5130_OFS42_SHIFT                           10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS42_FIELD TMC5130_OFS42_MASK, TMC5130_OFS42_SHIFT
#define TMC5130_OFS43_MASK                            0x0800 // MSLUT[1] // microstep table entry 43
#define TMC5130_OFS43_SHIFT                           11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS43_FIELD TMC5130_OFS43_MASK, TMC5130_OFS43_SHIFT
#define TMC5130_OFS44_MASK                            0x1000 // MSLUT[1] // microstep table entry 44
#define TMC5130_OFS44_SHIFT                           12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS44_FIELD TMC5130_OFS44_MASK, TMC5130_OFS44_SHIFT
#define TMC5130_OFS45_MASK                            0x2000 // MSLUT[1] // microstep table entry 45
#define TMC5130_OFS45_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS45_FIELD TMC5130_OFS45_MASK, TMC5130_OFS45_SHIFT
#define TMC5130_OFS46_MASK                            0x4000 // MSLUT[1] // microstep table entry 46
#define TMC5130_OFS46_SHIFT                           14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS46_FIELD TMC5130_OFS46_MASK, TMC5130_OFS46_SHIFT
#define TMC5130_OFS47_MASK                            0x8000 // MSLUT[1] // microstep table entry 47
#define TMC5130_OFS47_SHIFT                           15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS47_FIELD TMC5130_OFS47_MASK, TMC5130_OFS47_SHIFT
#define TMC5130_OFS48_MASK                            0x010000 // MSLUT[1] // microstep table entry 48
#define TMC5130_OFS48_SHIFT                           16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS48_FIELD TMC5130_OFS48_MASK, TMC5130_OFS48_SHIFT
#define TMC5130_OFS49_MASK                            0x020000 // MSLUT[1] // microstep table entry 49
#define TMC5130_OFS49_SHIFT                           17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS49_FIELD TMC5130_OFS49_MASK, TMC5130_OFS49_SHIFT
#define TMC5130_OFS50_MASK                            0x040000 // MSLUT[1] // microstep table entry 50
#define TMC5130_OFS50_SHIFT                           18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS50_FIELD TMC5130_OFS50_MASK, TMC5130_OFS50_SHIFT
#define TMC5130_OFS51_MASK                            0x080000 // MSLUT[1] // microstep table entry 51
#define TMC5130_OFS51_SHIFT                           19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS51_FIELD TMC5130_OFS51_MASK, TMC5130_OFS51_SHIFT
#define TMC5130_OFS52_MASK                            0x100000 // MSLUT[1] // microstep table entry 52
#define TMC5130_OFS52_SHIFT                           20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS52_FIELD TMC5130_OFS52_MASK, TMC5130_OFS52_SHIFT
#define TMC5130_OFS53_MASK                            0x200000 // MSLUT[1] // microstep table entry 53
#define TMC5130_OFS53_SHIFT                           21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS53_FIELD TMC5130_OFS53_MASK, TMC5130_OFS53_SHIFT
#define TMC5130_OFS54_MASK                            0x400000 // MSLUT[1] // microstep table entry 54
#define TMC5130_OFS54_SHIFT                           22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS54_FIELD TMC5130_OFS54_MASK, TMC5130_OFS54_SHIFT
#define TMC5130_OFS55_MASK                            0x800000 // MSLUT[1] // microstep table entry 55
#define TMC5130_OFS55_SHIFT                           23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS55_FIELD TMC5130_OFS55_MASK, TMC5130_OFS55_SHIFT
#define TMC5130_OFS56_MASK                            0x01000000 // MSLUT[1] // microstep table entry 56
#define TMC5130_OFS56_SHIFT                           24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS56_FIELD TMC5130_OFS56_MASK, TMC5130_OFS56_SHIFT
#define TMC5130_OFS57_MASK                            0x02000000 // MSLUT[1] // microstep table entry 57
#define TMC5130_OFS57_SHIFT                           25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS57_FIELD TMC5130_OFS57_MASK, TMC5130_OFS57_SHIFT
#define TMC5130_OFS58_MASK                            0x04000000 // MSLUT[1] // microstep table entry 58
#define TMC5130_OFS58_SHIFT                           26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS58_FIELD TMC5130_OFS58_MASK, TMC5130_OFS58_SHIFT
#define TMC5130_OFS59_MASK                            0x08000000 // MSLUT[1] // microstep table entry 59
#define TMC5130_OFS59_SHIFT                           27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS59_FIELD TMC5130_OFS59_MASK, TMC5130_OFS59_SHIFT
#define TMC5130_OFS60_MASK                            0x10000000 // MSLUT[1] // microstep table entry 60
#define TMC5130_OFS60_SHIFT                           28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS60_FIELD TMC5130_OFS60_MASK, TMC5130_OFS60_SHIFT
#define TMC5130_OFS61_MASK                            0x20000000 // MSLUT[1] // microstep table entry 61
#define TMC5130_OFS61_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS61_FIELD TMC5130_OFS61_MASK, TMC5130_OFS61_SHIFT
#define TMC5130_OFS62_MASK                            0x40000000 // MSLUT[1] // microstep table entry 62
#define TMC5130_OFS62_SHIFT                           30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS62_FIELD TMC5130_OFS62_MASK, TMC5130_OFS62_SHIFT
#define TMC5130_OFS63_MASK                            0x80000000 // MSLUT[1] // microstep table entry 63
#define TMC5130_OFS63_SHIFT                           31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS63_FIELD TMC5130_OFS63_MASK, TMC5130_OFS63_SHIFT
#define TMC5130_OFS64_MASK                            0x01 // MSLUT[2] // microstep table entry 64
#define TMC5130_OFS64_SHIFT                           0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS64_FIELD TMC5130_OFS64_MASK, TMC5130_OFS64_SHIFT
#define TMC5130_OFS65_MASK                            0x02 // MSLUT[2] // microstep table entry 65
#define TMC5130_OFS65_SHIFT                           1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS65_FIELD TMC5130_OFS65_MASK, TMC5130_OFS65_SHIFT
#define TMC5130_OFS66_MASK                            0x04 // MSLUT[2] // microstep table entry 66
#define TMC5130_OFS66_SHIFT                           2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS66_FIELD TMC5130_OFS66_MASK, TMC5130_OFS66_SHIFT
#define TMC5130_OFS67_MASK                            0x08 // MSLUT[2] // microstep table entry 67
#define TMC5130_OFS67_SHIFT                           3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS67_FIELD TMC5130_OFS67_MASK, TMC5130_OFS67_SHIFT
#define TMC5130_OFS68_MASK                            0x10 // MSLUT[2] // microstep table entry 68
#define TMC5130_OFS68_SHIFT                           4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS68_FIELD TMC5130_OFS68_MASK, TMC5130_OFS68_SHIFT
#define TMC5130_OFS69_MASK                            0x20 // MSLUT[2] // microstep table entry 69
#define TMC5130_OFS69_SHIFT                           5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS69_FIELD TMC5130_OFS69_MASK, TMC5130_OFS69_SHIFT
#define TMC5130_OFS70_MASK                            0x40 // MSLUT[2] // microstep table entry 70
#define TMC5130_OFS70_SHIFT                           6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS70_FIELD TMC5130_OFS70_MASK, TMC5130_OFS70_SHIFT
#define TMC5130_OFS71_MASK                            0x80 // MSLUT[2] // microstep table entry 71
#define TMC5130_OFS71_SHIFT                           7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS71_FIELD TMC5130_OFS71_MASK, TMC5130_OFS71_SHIFT
#define TMC5130_OFS72_MASK                            0x0100 // MSLUT[2] // microstep table entry 72
#define TMC5130_OFS72_SHIFT                           8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS72_FIELD TMC5130_OFS72_MASK, TMC5130_OFS72_SHIFT
#define TMC5130_OFS73_MASK                            0x0200 // MSLUT[2] // microstep table entry 73
#define TMC5130_OFS73_SHIFT                           9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS73_FIELD TMC5130_OFS73_MASK, TMC5130_OFS73_SHIFT
#define TMC5130_OFS74_MASK                            0x0400 // MSLUT[2] // microstep table entry 74
#define TMC5130_OFS74_SHIFT                           10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS74_FIELD TMC5130_OFS74_MASK, TMC5130_OFS74_SHIFT
#define TMC5130_OFS75_MASK                            0x0800 // MSLUT[2] // microstep table entry 75
#define TMC5130_OFS75_SHIFT                           11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS75_FIELD TMC5130_OFS75_MASK, TMC5130_OFS75_SHIFT
#define TMC5130_OFS76_MASK                            0x1000 // MSLUT[2] // microstep table entry 76
#define TMC5130_OFS76_SHIFT                           12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS76_FIELD TMC5130_OFS76_MASK, TMC5130_OFS76_SHIFT
#define TMC5130_OFS77_MASK                            0x2000 // MSLUT[2] // microstep table entry 77
#define TMC5130_OFS77_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS77_FIELD TMC5130_OFS77_MASK, TMC5130_OFS77_SHIFT
#define TMC5130_OFS78_MASK                            0x4000 // MSLUT[2] // microstep table entry 78
#define TMC5130_OFS78_SHIFT                           14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS78_FIELD TMC5130_OFS78_MASK, TMC5130_OFS78_SHIFT
#define TMC5130_OFS79_MASK                            0x8000 // MSLUT[2] // microstep table entry 79
#define TMC5130_OFS79_SHIFT                           15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS79_FIELD TMC5130_OFS79_MASK, TMC5130_OFS79_SHIFT
#define TMC5130_OFS80_MASK                            0x010000 // MSLUT[2] // microstep table entry 80
#define TMC5130_OFS80_SHIFT                           16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS80_FIELD TMC5130_OFS80_MASK, TMC5130_OFS80_SHIFT
#define TMC5130_OFS81_MASK                            0x020000 // MSLUT[2] // microstep table entry 81
#define TMC5130_OFS81_SHIFT                           17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS81_FIELD TMC5130_OFS81_MASK, TMC5130_OFS81_SHIFT
#define TMC5130_OFS82_MASK                            0x040000 // MSLUT[2] // microstep table entry 82
#define TMC5130_OFS82_SHIFT                           18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS82_FIELD TMC5130_OFS82_MASK, TMC5130_OFS82_SHIFT
#define TMC5130_OFS83_MASK                            0x080000 // MSLUT[2] // microstep table entry 83
#define TMC5130_OFS83_SHIFT                           19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS83_FIELD TMC5130_OFS83_MASK, TMC5130_OFS83_SHIFT
#define TMC5130_OFS84_MASK                            0x100000 // MSLUT[2] // microstep table entry 84
#define TMC5130_OFS84_SHIFT                           20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS84_FIELD TMC5130_OFS84_MASK, TMC5130_OFS84_SHIFT
#define TMC5130_OFS85_MASK                            0x200000 // MSLUT[2] // microstep table entry 85
#define TMC5130_OFS85_SHIFT                           21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS85_FIELD TMC5130_OFS85_MASK, TMC5130_OFS85_SHIFT
#define TMC5130_OFS86_MASK                            0x400000 // MSLUT[2] // microstep table entry 86
#define TMC5130_OFS86_SHIFT                           22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS86_FIELD TMC5130_OFS86_MASK, TMC5130_OFS86_SHIFT
#define TMC5130_OFS87_MASK                            0x800000 // MSLUT[2] // microstep table entry 87
#define TMC5130_OFS87_SHIFT                           23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS87_FIELD TMC5130_OFS87_MASK, TMC5130_OFS87_SHIFT
#define TMC5130_OFS88_MASK                            0x01000000 // MSLUT[2] // microstep table entry 88
#define TMC5130_OFS88_SHIFT                           24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS88_FIELD TMC5130_OFS88_MASK, TMC5130_OFS88_SHIFT
#define TMC5130_OFS89_MASK                            0x02000000 // MSLUT[2] // microstep table entry 89
#define TMC5130_OFS89_SHIFT                           25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS89_FIELD TMC5130_OFS89_MASK, TMC5130_OFS89_SHIFT
#define TMC5130_OFS90_MASK                            0x04000000 // MSLUT[2] // microstep table entry 90
#define TMC5130_OFS90_SHIFT                           26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS90_FIELD TMC5130_OFS90_MASK, TMC5130_OFS90_SHIFT
#define TMC5130_OFS91_MASK                            0x08000000 // MSLUT[2] // microstep table entry 91
#define TMC5130_OFS91_SHIFT                           27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS91_FIELD TMC5130_OFS91_MASK, TMC5130_OFS91_SHIFT
#define TMC5130_OFS92_MASK                            0x10000000 // MSLUT[2] // microstep table entry 92
#define TMC5130_OFS92_SHIFT                           28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS92_FIELD TMC5130_OFS92_MASK, TMC5130_OFS92_SHIFT
#define TMC5130_OFS93_MASK                            0x20000000 // MSLUT[2] // microstep table entry 93
#define TMC5130_OFS93_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS93_FIELD TMC5130_OFS93_MASK, TMC5130_OFS93_SHIFT
#define TMC5130_OFS94_MASK                            0x40000000 // MSLUT[2] // microstep table entry 94
#define TMC5130_OFS94_SHIFT                           30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS94_FIELD TMC5130_OFS94_MASK, TMC5130_OFS94_SHIFT
#define TMC5130_OFS95_MASK                            0x80000000 // MSLUT[2] // microstep table entry 95
#define TMC5130_OFS95_SHIFT                           31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS95_FIELD TMC5130_OFS95_MASK, TMC5130_OFS95_SHIFT
#define TMC5130_OFS96_MASK                            0x01 // MSLUT[3] // microstep table entry 96
#define TMC5130_OFS96_SHIFT                           0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS96_FIELD TMC5130_OFS96_MASK, TMC5130_OFS96_SHIFT
#define TMC5130_OFS97_MASK                            0x02 // MSLUT[3] // microstep table entry 97
#define TMC5130_OFS97_SHIFT                           1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS97_FIELD TMC5130_OFS97_MASK, TMC5130_OFS97_SHIFT
#define TMC5130_OFS98_MASK                            0x04 // MSLUT[3] // microstep table entry 98
#define TMC5130_OFS98_SHIFT                           2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS98_FIELD TMC5130_OFS98_MASK, TMC5130_OFS98_SHIFT
#define TMC5130_OFS99_MASK                            0x08 // MSLUT[3] // microstep table entry 99
#define TMC5130_OFS99_SHIFT                           3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS99_FIELD TMC5130_OFS99_MASK, TMC5130_OFS99_SHIFT
#define TMC5130_OFS100_MASK                           0x10 // MSLUT[3] // microstep table entry 100
#define TMC5130_OFS100_SHIFT                          4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS100_FIELD TMC5130_OFS100_MASK, TMC5130_OFS100_SHIFT
#define TMC5130_OFS101_MASK                           0x20 // MSLUT[3] // microstep table entry 101
#define TMC5130_OFS101_SHIFT                          5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS101_FIELD TMC5130_OFS101_MASK, TMC5130_OFS101_SHIFT
#define TMC5130_OFS102_MASK                           0x40 // MSLUT[3] // microstep table entry 102
#define TMC5130_OFS102_SHIFT                          6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS102_FIELD TMC5130_OFS102_MASK, TMC5130_OFS102_SHIFT
#define TMC5130_OFS103_MASK                           0x80 // MSLUT[3] // microstep table entry 103
#define TMC5130_OFS103_SHIFT                          7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS103_FIELD TMC5130_OFS103_MASK, TMC5130_OFS103_SHIFT
#define TMC5130_OFS104_MASK                           0x0100 // MSLUT[3] // microstep table entry 104
#define TMC5130_OFS104_SHIFT                          8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS104_FIELD TMC5130_OFS104_MASK, TMC5130_OFS104_SHIFT
#define TMC5130_OFS105_MASK                           0x0200 // MSLUT[3] // microstep table entry 105
#define TMC5130_OFS105_SHIFT                          9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS105_FIELD TMC5130_OFS105_MASK, TMC5130_OFS105_SHIFT
#define TMC5130_OFS106_MASK                           0x0400 // MSLUT[3] // microstep table entry 106
#define TMC5130_OFS106_SHIFT                          10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS106_FIELD TMC5130_OFS106_MASK, TMC5130_OFS106_SHIFT
#define TMC5130_OFS107_MASK                           0x0800 // MSLUT[3] // microstep table entry 107
#define TMC5130_OFS107_SHIFT                          11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS107_FIELD TMC5130_OFS107_MASK, TMC5130_OFS107_SHIFT
#define TMC5130_OFS108_MASK                           0x1000 // MSLUT[3] // microstep table entry 108
#define TMC5130_OFS108_SHIFT                          12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS108_FIELD TMC5130_OFS108_MASK, TMC5130_OFS108_SHIFT
#define TMC5130_OFS109_MASK                           0x2000 // MSLUT[3] // microstep table entry 109
#define TMC5130_OFS109_SHIFT                          13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS109_FIELD TMC5130_OFS109_MASK, TMC5130_OFS109_SHIFT
#define TMC5130_OFS110_MASK                           0x4000 // MSLUT[3] // microstep table entry 110
#define TMC5130_OFS110_SHIFT                          14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS110_FIELD TMC5130_OFS110_MASK, TMC5130_OFS110_SHIFT
#define TMC5130_OFS111_MASK                           0x8000 // MSLUT[3] // microstep table entry 111
#define TMC5130_OFS111_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS111_FIELD TMC5130_OFS111_MASK, TMC5130_OFS111_SHIFT
#define TMC5130_OFS112_MASK                           0x010000 // MSLUT[3] // microstep table entry 112
#define TMC5130_OFS112_SHIFT                          16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS112_FIELD TMC5130_OFS112_MASK, TMC5130_OFS112_SHIFT
#define TMC5130_OFS113_MASK                           0x020000 // MSLUT[3] // microstep table entry 113
#define TMC5130_OFS113_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS113_FIELD TMC5130_OFS113_MASK, TMC5130_OFS113_SHIFT
#define TMC5130_OFS114_MASK                           0x040000 // MSLUT[3] // microstep table entry 114
#define TMC5130_OFS114_SHIFT                          18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS114_FIELD TMC5130_OFS114_MASK, TMC5130_OFS114_SHIFT
#define TMC5130_OFS115_MASK                           0x080000 // MSLUT[3] // microstep table entry 115
#define TMC5130_OFS115_SHIFT                          19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS115_FIELD TMC5130_OFS115_MASK, TMC5130_OFS115_SHIFT
#define TMC5130_OFS116_MASK                           0x100000 // MSLUT[3] // microstep table entry 116
#define TMC5130_OFS116_SHIFT                          20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS116_FIELD TMC5130_OFS116_MASK, TMC5130_OFS116_SHIFT
#define TMC5130_OFS117_MASK                           0x200000 // MSLUT[3] // microstep table entry 117
#define TMC5130_OFS117_SHIFT                          21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS117_FIELD TMC5130_OFS117_MASK, TMC5130_OFS117_SHIFT
#define TMC5130_OFS118_MASK                           0x400000 // MSLUT[3] // microstep table entry 118
#define TMC5130_OFS118_SHIFT                          22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS118_FIELD TMC5130_OFS118_MASK, TMC5130_OFS118_SHIFT
#define TMC5130_OFS119_MASK                           0x800000 // MSLUT[3] // microstep table entry 119
#define TMC5130_OFS119_SHIFT                          23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS119_FIELD TMC5130_OFS119_MASK, TMC5130_OFS119_SHIFT
#define TMC5130_OFS120_MASK                           0x01000000 // MSLUT[3] // microstep table entry 120
#define TMC5130_OFS120_SHIFT                          24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS120_FIELD TMC5130_OFS120_MASK, TMC5130_OFS120_SHIFT
#define TMC5130_OFS121_MASK                           0x02000000 // MSLUT[3] // microstep table entry 121
#define TMC5130_OFS121_SHIFT                          25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS121_FIELD TMC5130_OFS121_MASK, TMC5130_OFS121_SHIFT
#define TMC5130_OFS122_MASK                           0x04000000 // MSLUT[3] // microstep table entry 122
#define TMC5130_OFS122_SHIFT                          26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS122_FIELD TMC5130_OFS122_MASK, TMC5130_OFS122_SHIFT
#define TMC5130_OFS123_MASK                           0x08000000 // MSLUT[3] // microstep table entry 123
#define TMC5130_OFS123_SHIFT                          27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS123_FIELD TMC5130_OFS123_MASK, TMC5130_OFS123_SHIFT
#define TMC5130_OFS124_MASK                           0x10000000 // MSLUT[3] // microstep table entry 124
#define TMC5130_OFS124_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS124_FIELD TMC5130_OFS124_MASK, TMC5130_OFS124_SHIFT
#define TMC5130_OFS125_MASK                           0x20000000 // MSLUT[3] // microstep table entry 125
#define TMC5130_OFS125_SHIFT                          29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS125_FIELD TMC5130_OFS125_MASK, TMC5130_OFS125_SHIFT
#define TMC5130_OFS126_MASK                           0x40000000 // MSLUT[3] // microstep table entry 126
#define TMC5130_OFS126_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS126_FIELD TMC5130_OFS126_MASK, TMC5130_OFS126_SHIFT
#define TMC5130_OFS127_MASK                           0x80000000 // MSLUT[3] // microstep table entry 127
#define TMC5130_OFS127_SHIFT                          31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS127_FIELD TMC5130_OFS127_MASK, TMC5130_OFS127_SHIFT
#define TMC5130_OFS128_MASK                           0x01 // MSLUT[4] // microstep table entry 128
#define TMC5130_OFS128_SHIFT                          0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS128_FIELD TMC5130_OFS128_MASK, TMC5130_OFS128_SHIFT
#define TMC5130_OFS129_MASK                           0x02 // MSLUT[4] // microstep table entry 129
#define TMC5130_OFS129_SHIFT                          1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS129_FIELD TMC5130_OFS129_MASK, TMC5130_OFS129_SHIFT
#define TMC5130_OFS130_MASK                           0x04 // MSLUT[4] // microstep table entry 130
#define TMC5130_OFS130_SHIFT                          2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS130_FIELD TMC5130_OFS130_MASK, TMC5130_OFS130_SHIFT
#define TMC5130_OFS131_MASK                           0x08 // MSLUT[4] // microstep table entry 131
#define TMC5130_OFS131_SHIFT                          3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS131_FIELD TMC5130_OFS131_MASK, TMC5130_OFS131_SHIFT
#define TMC5130_OFS132_MASK                           0x10 // MSLUT[4] // microstep table entry 132
#define TMC5130_OFS132_SHIFT                          4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS132_FIELD TMC5130_OFS132_MASK, TMC5130_OFS132_SHIFT
#define TMC5130_OFS133_MASK                           0x20 // MSLUT[4] // microstep table entry 133
#define TMC5130_OFS133_SHIFT                          5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS133_FIELD TMC5130_OFS133_MASK, TMC5130_OFS133_SHIFT
#define TMC5130_OFS134_MASK                           0x40 // MSLUT[4] // microstep table entry 134
#define TMC5130_OFS134_SHIFT                          6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS134_FIELD TMC5130_OFS134_MASK, TMC5130_OFS134_SHIFT
#define TMC5130_OFS135_MASK                           0x80 // MSLUT[4] // microstep table entry 135
#define TMC5130_OFS135_SHIFT                          7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS135_FIELD TMC5130_OFS135_MASK, TMC5130_OFS135_SHIFT
#define TMC5130_OFS136_MASK                           0x0100 // MSLUT[4] // microstep table entry 136
#define TMC5130_OFS136_SHIFT                          8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS136_FIELD TMC5130_OFS136_MASK, TMC5130_OFS136_SHIFT
#define TMC5130_OFS137_MASK                           0x0200 // MSLUT[4] // microstep table entry 137
#define TMC5130_OFS137_SHIFT                          9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS137_FIELD TMC5130_OFS137_MASK, TMC5130_OFS137_SHIFT
#define TMC5130_OFS138_MASK                           0x0400 // MSLUT[4] // microstep table entry 138
#define TMC5130_OFS138_SHIFT                          10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS138_FIELD TMC5130_OFS138_MASK, TMC5130_OFS138_SHIFT
#define TMC5130_OFS139_MASK                           0x0800 // MSLUT[4] // microstep table entry 139
#define TMC5130_OFS139_SHIFT                          11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS139_FIELD TMC5130_OFS139_MASK, TMC5130_OFS139_SHIFT
#define TMC5130_OFS140_MASK                           0x1000 // MSLUT[4] // microstep table entry 140
#define TMC5130_OFS140_SHIFT                          12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS140_FIELD TMC5130_OFS140_MASK, TMC5130_OFS140_SHIFT
#define TMC5130_OFS141_MASK                           0x2000 // MSLUT[4] // microstep table entry 141
#define TMC5130_OFS141_SHIFT                          13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS141_FIELD TMC5130_OFS141_MASK, TMC5130_OFS141_SHIFT
#define TMC5130_OFS142_MASK                           0x4000 // MSLUT[4] // microstep table entry 142
#define TMC5130_OFS142_SHIFT                          14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS142_FIELD TMC5130_OFS142_MASK, TMC5130_OFS142_SHIFT
#define TMC5130_OFS143_MASK                           0x8000 // MSLUT[4] // microstep table entry 143
#define TMC5130_OFS143_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS143_FIELD TMC5130_OFS143_MASK, TMC5130_OFS143_SHIFT
#define TMC5130_OFS144_MASK                           0x010000 // MSLUT[4] // microstep table entry 144
#define TMC5130_OFS144_SHIFT                          16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS144_FIELD TMC5130_OFS144_MASK, TMC5130_OFS144_SHIFT
#define TMC5130_OFS145_MASK                           0x020000 // MSLUT[4] // microstep table entry 145
#define TMC5130_OFS145_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS145_FIELD TMC5130_OFS145_MASK, TMC5130_OFS145_SHIFT
#define TMC5130_OFS146_MASK                           0x040000 // MSLUT[4] // microstep table entry 146
#define TMC5130_OFS146_SHIFT                          18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS146_FIELD TMC5130_OFS146_MASK, TMC5130_OFS146_SHIFT
#define TMC5130_OFS147_MASK                           0x080000 // MSLUT[4] // microstep table entry 147
#define TMC5130_OFS147_SHIFT                          19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS147_FIELD TMC5130_OFS147_MASK, TMC5130_OFS147_SHIFT
#define TMC5130_OFS148_MASK                           0x100000 // MSLUT[4] // microstep table entry 148
#define TMC5130_OFS148_SHIFT                          20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS148_FIELD TMC5130_OFS148_MASK, TMC5130_OFS148_SHIFT
#define TMC5130_OFS149_MASK                           0x200000 // MSLUT[4] // microstep table entry 149
#define TMC5130_OFS149_SHIFT                          21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS149_FIELD TMC5130_OFS149_MASK, TMC5130_OFS149_SHIFT
#define TMC5130_OFS150_MASK                           0x400000 // MSLUT[4] // microstep table entry 150
#define TMC5130_OFS150_SHIFT                          22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS150_FIELD TMC5130_OFS150_MASK, TMC5130_OFS150_SHIFT
#define TMC5130_OFS151_MASK                           0x800000 // MSLUT[4] // microstep table entry 151
#define TMC5130_OFS151_SHIFT                          23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS151_FIELD TMC5130_OFS151_MASK, TMC5130_OFS151_SHIFT
#define TMC5130_OFS152_MASK                           0x01000000 // MSLUT[4] // microstep table entry 152
#define TMC5130_OFS152_SHIFT                          24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS152_FIELD TMC5130_OFS152_MASK, TMC5130_OFS152_SHIFT
#define TMC5130_OFS153_MASK                           0x02000000 // MSLUT[4] // microstep table entry 153
#define TMC5130_OFS153_SHIFT                          25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS153_FIELD TMC5130_OFS153_MASK, TMC5130_OFS153_SHIFT
#define TMC5130_OFS154_MASK                           0x04000000 // MSLUT[4] // microstep table entry 154
#define TMC5130_OFS154_SHIFT                          26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS154_FIELD TMC5130_OFS154_MASK, TMC5130_OFS154_SHIFT
#define TMC5130_OFS155_MASK                           0x08000000 // MSLUT[4] // microstep table entry 155
#define TMC5130_OFS155_SHIFT                          27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS155_FIELD TMC5130_OFS155_MASK, TMC5130_OFS155_SHIFT
#define TMC5130_OFS156_MASK                           0x10000000 // MSLUT[4] // microstep table entry 156
#define TMC5130_OFS156_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS156_FIELD TMC5130_OFS156_MASK, TMC5130_OFS156_SHIFT
#define TMC5130_OFS157_MASK                           0x20000000 // MSLUT[4] // microstep table entry 157
#define TMC5130_OFS157_SHIFT                          29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS157_FIELD TMC5130_OFS157_MASK, TMC5130_OFS157_SHIFT
#define TMC5130_OFS158_MASK                           0x40000000 // MSLUT[4] // microstep table entry 158
#define TMC5130_OFS158_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS158_FIELD TMC5130_OFS158_MASK, TMC5130_OFS158_SHIFT
#define TMC5130_OFS159_MASK                           0x80000000 // MSLUT[4] // microstep table entry 159
#define TMC5130_OFS159_SHIFT                          31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS159_FIELD TMC5130_OFS159_MASK, TMC5130_OFS159_SHIFT
#define TMC5130_OFS160_MASK                           0x01 // MSLUT[5] // microstep table entry 160
#define TMC5130_OFS160_SHIFT                          0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS160_FIELD TMC5130_OFS160_MASK, TMC5130_OFS160_SHIFT
#define TMC5130_OFS161_MASK                           0x02 // MSLUT[5] // microstep table entry 161
#define TMC5130_OFS161_SHIFT                          1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS161_FIELD TMC5130_OFS161_MASK, TMC5130_OFS161_SHIFT
#define TMC5130_OFS162_MASK                           0x04 // MSLUT[5] // microstep table entry 162
#define TMC5130_OFS162_SHIFT                          2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS162_FIELD TMC5130_OFS162_MASK, TMC5130_OFS162_SHIFT
#define TMC5130_OFS163_MASK                           0x08 // MSLUT[5] // microstep table entry 163
#define TMC5130_OFS163_SHIFT                          3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS163_FIELD TMC5130_OFS163_MASK, TMC5130_OFS163_SHIFT
#define TMC5130_OFS164_MASK                           0x10 // MSLUT[5] // microstep table entry 164
#define TMC5130_OFS164_SHIFT                          4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS164_FIELD TMC5130_OFS164_MASK, TMC5130_OFS164_SHIFT
#define TMC5130_OFS165_MASK                           0x20 // MSLUT[5] // microstep table entry 165
#define TMC5130_OFS165_SHIFT                          5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS165_FIELD TMC5130_OFS165_MASK, TMC5130_OFS165_SHIFT
#define TMC5130_OFS166_MASK                           0x40 // MSLUT[5] // microstep table entry 166
#define TMC5130_OFS166_SHIFT                          6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS166_FIELD TMC5130_OFS166_MASK, TMC5130_OFS166_SHIFT
#define TMC5130_OFS167_MASK                           0x80 // MSLUT[5] // microstep table entry 167
#define TMC5130_OFS167_SHIFT                          7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS167_FIELD TMC5130_OFS167_MASK, TMC5130_OFS167_SHIFT
#define TMC5130_OFS168_MASK                           0x0100 // MSLUT[5] // microstep table entry 168
#define TMC5130_OFS168_SHIFT                          8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS168_FIELD TMC5130_OFS168_MASK, TMC5130_OFS168_SHIFT
#define TMC5130_OFS169_MASK                           0x0200 // MSLUT[5] // microstep table entry 169
#define TMC5130_OFS169_SHIFT                          9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS169_FIELD TMC5130_OFS169_MASK, TMC5130_OFS169_SHIFT
#define TMC5130_OFS170_MASK                           0x0400 // MSLUT[5] // microstep table entry 170
#define TMC5130_OFS170_SHIFT                          10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS170_FIELD TMC5130_OFS170_MASK, TMC5130_OFS170_SHIFT
#define TMC5130_OFS171_MASK                           0x0800 // MSLUT[5] // microstep table entry 171
#define TMC5130_OFS171_SHIFT                          11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS171_FIELD TMC5130_OFS171_MASK, TMC5130_OFS171_SHIFT
#define TMC5130_OFS172_MASK                           0x1000 // MSLUT[5] // microstep table entry 172
#define TMC5130_OFS172_SHIFT                          12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS172_FIELD TMC5130_OFS172_MASK, TMC5130_OFS172_SHIFT
#define TMC5130_OFS173_MASK                           0x2000 // MSLUT[5] // microstep table entry 173
#define TMC5130_OFS173_SHIFT                          13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS173_FIELD TMC5130_OFS173_MASK, TMC5130_OFS173_SHIFT
#define TMC5130_OFS174_MASK                           0x4000 // MSLUT[5] // microstep table entry 174
#define TMC5130_OFS174_SHIFT                          14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS174_FIELD TMC5130_OFS174_MASK, TMC5130_OFS174_SHIFT
#define TMC5130_OFS175_MASK                           0x8000 // MSLUT[5] // microstep table entry 175
#define TMC5130_OFS175_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS175_FIELD TMC5130_OFS175_MASK, TMC5130_OFS175_SHIFT
#define TMC5130_OFS176_MASK                           0x010000 // MSLUT[5] // microstep table entry 176
#define TMC5130_OFS176_SHIFT                          16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS176_FIELD TMC5130_OFS176_MASK, TMC5130_OFS176_SHIFT
#define TMC5130_OFS177_MASK                           0x020000 // MSLUT[5] // microstep table entry 177
#define TMC5130_OFS177_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS177_FIELD TMC5130_OFS177_MASK, TMC5130_OFS177_SHIFT
#define TMC5130_OFS178_MASK                           0x040000 // MSLUT[5] // microstep table entry 178
#define TMC5130_OFS178_SHIFT                          18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS178_FIELD TMC5130_OFS178_MASK, TMC5130_OFS178_SHIFT
#define TMC5130_OFS179_MASK                           0x080000 // MSLUT[5] // microstep table entry 179
#define TMC5130_OFS179_SHIFT                          19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS179_FIELD TMC5130_OFS179_MASK, TMC5130_OFS179_SHIFT
#define TMC5130_OFS180_MASK                           0x100000 // MSLUT[5] // microstep table entry 180
#define TMC5130_OFS180_SHIFT                          20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS180_FIELD TMC5130_OFS180_MASK, TMC5130_OFS180_SHIFT
#define TMC5130_OFS181_MASK                           0x200000 // MSLUT[5] // microstep table entry 181
#define TMC5130_OFS181_SHIFT                          21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS181_FIELD TMC5130_OFS181_MASK, TMC5130_OFS181_SHIFT
#define TMC5130_OFS182_MASK                           0x400000 // MSLUT[5] // microstep table entry 182
#define TMC5130_OFS182_SHIFT                          22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS182_FIELD TMC5130_OFS182_MASK, TMC5130_OFS182_SHIFT
#define TMC5130_OFS183_MASK                           0x800000 // MSLUT[5] // microstep table entry 183
#define TMC5130_OFS183_SHIFT                          23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS183_FIELD TMC5130_OFS183_MASK, TMC5130_OFS183_SHIFT
#define TMC5130_OFS184_MASK                           0x01000000 // MSLUT[5] // microstep table entry 184
#define TMC5130_OFS184_SHIFT                          24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS184_FIELD TMC5130_OFS184_MASK, TMC5130_OFS184_SHIFT
#define TMC5130_OFS185_MASK                           0x02000000 // MSLUT[5] // microstep table entry 185
#define TMC5130_OFS185_SHIFT                          25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS185_FIELD TMC5130_OFS185_MASK, TMC5130_OFS185_SHIFT
#define TMC5130_OFS186_MASK                           0x04000000 // MSLUT[5] // microstep table entry 186
#define TMC5130_OFS186_SHIFT                          26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS186_FIELD TMC5130_OFS186_MASK, TMC5130_OFS186_SHIFT
#define TMC5130_OFS187_MASK                           0x08000000 // MSLUT[5] // microstep table entry 187
#define TMC5130_OFS187_SHIFT                          27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS187_FIELD TMC5130_OFS187_MASK, TMC5130_OFS187_SHIFT
#define TMC5130_OFS188_MASK                           0x10000000 // MSLUT[5] // microstep table entry 188
#define TMC5130_OFS188_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS188_FIELD TMC5130_OFS188_MASK, TMC5130_OFS188_SHIFT
#define TMC5130_OFS189_MASK                           0x20000000 // MSLUT[5] // microstep table entry 189
#define TMC5130_OFS189_SHIFT                          29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS189_FIELD TMC5130_OFS189_MASK, TMC5130_OFS189_SHIFT
#define TMC5130_OFS190_MASK                           0x40000000 // MSLUT[5] // microstep table entry 190
#define TMC5130_OFS190_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS190_FIELD TMC5130_OFS190_MASK, TMC5130_OFS190_SHIFT
#define TMC5130_OFS191_MASK                           0x80000000 // MSLUT[5] // microstep table entry 191
#define TMC5130_OFS191_SHIFT                          31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS191_FIELD TMC5130_OFS191_MASK, TMC5130_OFS191_SHIFT
#define TMC5130_OFS192_MASK                           0x01 // MSLUT[6] // microstep table entry 192
#define TMC5130_OFS192_SHIFT                          0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS192_FIELD TMC5130_OFS192_MASK, TMC5130_OFS192_SHIFT
#define TMC5130_OFS193_MASK                           0x02 // MSLUT[6] // microstep table entry 193
#define TMC5130_OFS193_SHIFT                          1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS193_FIELD TMC5130_OFS193_MASK, TMC5130_OFS193_SHIFT
#define TMC5130_OFS194_MASK                           0x04 // MSLUT[6] // microstep table entry 194
#define TMC5130_OFS194_SHIFT                          2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS194_FIELD TMC5130_OFS194_MASK, TMC5130_OFS194_SHIFT
#define TMC5130_OFS195_MASK                           0x08 // MSLUT[6] // microstep table entry 195
#define TMC5130_OFS195_SHIFT                          3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS195_FIELD TMC5130_OFS195_MASK, TMC5130_OFS195_SHIFT
#define TMC5130_OFS196_MASK                           0x10 // MSLUT[6] // microstep table entry 196
#define TMC5130_OFS196_SHIFT                          4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS196_FIELD TMC5130_OFS196_MASK, TMC5130_OFS196_SHIFT
#define TMC5130_OFS197_MASK                           0x20 // MSLUT[6] // microstep table entry 197
#define TMC5130_OFS197_SHIFT                          5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS197_FIELD TMC5130_OFS197_MASK, TMC5130_OFS197_SHIFT
#define TMC5130_OFS198_MASK                           0x40 // MSLUT[6] // microstep table entry 198
#define TMC5130_OFS198_SHIFT                          6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS198_FIELD TMC5130_OFS198_MASK, TMC5130_OFS198_SHIFT
#define TMC5130_OFS199_MASK                           0x80 // MSLUT[6] // microstep table entry 199
#define TMC5130_OFS199_SHIFT                          7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS199_FIELD TMC5130_OFS199_MASK, TMC5130_OFS199_SHIFT
#define TMC5130_OFS200_MASK                           0x0100 // MSLUT[6] // microstep table entry 200
#define TMC5130_OFS200_SHIFT                          8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS200_FIELD TMC5130_OFS200_MASK, TMC5130_OFS200_SHIFT
#define TMC5130_OFS201_MASK                           0x0200 // MSLUT[6] // microstep table entry 201
#define TMC5130_OFS201_SHIFT                          9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS201_FIELD TMC5130_OFS201_MASK, TMC5130_OFS201_SHIFT
#define TMC5130_OFS202_MASK                           0x0400 // MSLUT[6] // microstep table entry 202
#define TMC5130_OFS202_SHIFT                          10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS202_FIELD TMC5130_OFS202_MASK, TMC5130_OFS202_SHIFT
#define TMC5130_OFS203_MASK                           0x0800 // MSLUT[6] // microstep table entry 203
#define TMC5130_OFS203_SHIFT                          11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS203_FIELD TMC5130_OFS203_MASK, TMC5130_OFS203_SHIFT
#define TMC5130_OFS204_MASK                           0x1000 // MSLUT[6] // microstep table entry 204
#define TMC5130_OFS204_SHIFT                          12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS204_FIELD TMC5130_OFS204_MASK, TMC5130_OFS204_SHIFT
#define TMC5130_OFS205_MASK                           0x2000 // MSLUT[6] // microstep table entry 205
#define TMC5130_OFS205_SHIFT                          13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS205_FIELD TMC5130_OFS205_MASK, TMC5130_OFS205_SHIFT
#define TMC5130_OFS206_MASK                           0x4000 // MSLUT[6] // microstep table entry 206
#define TMC5130_OFS206_SHIFT                          14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS206_FIELD TMC5130_OFS206_MASK, TMC5130_OFS206_SHIFT
#define TMC5130_OFS207_MASK                           0x8000 // MSLUT[6] // microstep table entry 207
#define TMC5130_OFS207_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS207_FIELD TMC5130_OFS207_MASK, TMC5130_OFS207_SHIFT
#define TMC5130_OFS208_MASK                           0x010000 // MSLUT[6] // microstep table entry 208
#define TMC5130_OFS208_SHIFT                          16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS208_FIELD TMC5130_OFS208_MASK, TMC5130_OFS208_SHIFT
#define TMC5130_OFS209_MASK                           0x020000 // MSLUT[6] // microstep table entry 209
#define TMC5130_OFS209_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS209_FIELD TMC5130_OFS209_MASK, TMC5130_OFS209_SHIFT
#define TMC5130_OFS210_MASK                           0x040000 // MSLUT[6] // microstep table entry 210
#define TMC5130_OFS210_SHIFT                          18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS210_FIELD TMC5130_OFS210_MASK, TMC5130_OFS210_SHIFT
#define TMC5130_OFS211_MASK                           0x080000 // MSLUT[6] // microstep table entry 211
#define TMC5130_OFS211_SHIFT                          19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS211_FIELD TMC5130_OFS211_MASK, TMC5130_OFS211_SHIFT
#define TMC5130_OFS212_MASK                           0x100000 // MSLUT[6] // microstep table entry 212
#define TMC5130_OFS212_SHIFT                          20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS212_FIELD TMC5130_OFS212_MASK, TMC5130_OFS212_SHIFT
#define TMC5130_OFS213_MASK                           0x200000 // MSLUT[6] // microstep table entry 213
#define TMC5130_OFS213_SHIFT                          21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS213_FIELD TMC5130_OFS213_MASK, TMC5130_OFS213_SHIFT
#define TMC5130_OFS214_MASK                           0x400000 // MSLUT[6] // microstep table entry 214
#define TMC5130_OFS214_SHIFT                          22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS214_FIELD TMC5130_OFS214_MASK, TMC5130_OFS214_SHIFT
#define TMC5130_OFS215_MASK                           0x800000 // MSLUT[6] // microstep table entry 215
#define TMC5130_OFS215_SHIFT                          23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS215_FIELD TMC5130_OFS215_MASK, TMC5130_OFS215_SHIFT
#define TMC5130_OFS216_MASK                           0x01000000 // MSLUT[6] // microstep table entry 216
#define TMC5130_OFS216_SHIFT                          24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS216_FIELD TMC5130_OFS216_MASK, TMC5130_OFS216_SHIFT
#define TMC5130_OFS217_MASK                           0x02000000 // MSLUT[6] // microstep table entry 217
#define TMC5130_OFS217_SHIFT                          25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS217_FIELD TMC5130_OFS217_MASK, TMC5130_OFS217_SHIFT
#define TMC5130_OFS218_MASK                           0x04000000 // MSLUT[6] // microstep table entry 218
#define TMC5130_OFS218_SHIFT                          26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS218_FIELD TMC5130_OFS218_MASK, TMC5130_OFS218_SHIFT
#define TMC5130_OFS219_MASK                           0x08000000 // MSLUT[6] // microstep table entry 219
#define TMC5130_OFS219_SHIFT                          27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS219_FIELD TMC5130_OFS219_MASK, TMC5130_OFS219_SHIFT
#define TMC5130_OFS220_MASK                           0x10000000 // MSLUT[6] // microstep table entry 220
#define TMC5130_OFS220_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS220_FIELD TMC5130_OFS220_MASK, TMC5130_OFS220_SHIFT
#define TMC5130_OFS221_MASK                           0x20000000 // MSLUT[6] // microstep table entry 221
#define TMC5130_OFS221_SHIFT                          29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS221_FIELD TMC5130_OFS221_MASK, TMC5130_OFS221_SHIFT
#define TMC5130_OFS222_MASK                           0x40000000 // MSLUT[6] // microstep table entry 222
#define TMC5130_OFS222_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS222_FIELD TMC5130_OFS222_MASK, TMC5130_OFS222_SHIFT
#define TMC5130_OFS223_MASK                           0x80000000 // MSLUT[6] // microstep table entry 223
#define TMC5130_OFS223_SHIFT                          31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS223_FIELD TMC5130_OFS223_MASK, TMC5130_OFS223_SHIFT
#define TMC5130_OFS224_MASK                           0x01 // MSLUT[7] // microstep table entry 224
#define TMC5130_OFS224_SHIFT                          0 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS224_FIELD TMC5130_OFS224_MASK, TMC5130_OFS224_SHIFT
#define TMC5130_OFS225_MASK                           0x02 // MSLUT[7] // microstep table entry 225
#define TMC5130_OFS225_SHIFT                          1 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS225_FIELD TMC5130_OFS225_MASK, TMC5130_OFS225_SHIFT
#define TMC5130_OFS226_MASK                           0x04 // MSLUT[7] // microstep table entry 226
#define TMC5130_OFS226_SHIFT                          2 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS226_FIELD TMC5130_OFS226_MASK, TMC5130_OFS226_SHIFT
#define TMC5130_OFS227_MASK                           0x08 // MSLUT[7] // microstep table entry 227
#define TMC5130_OFS227_SHIFT                          3 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS227_FIELD TMC5130_OFS227_MASK, TMC5130_OFS227_SHIFT
#define TMC5130_OFS228_MASK                           0x10 // MSLUT[7] // microstep table entry 228
#define TMC5130_OFS228_SHIFT                          4 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS228_FIELD TMC5130_OFS228_MASK, TMC5130_OFS228_SHIFT
#define TMC5130_OFS229_MASK                           0x20 // MSLUT[7] // microstep table entry 229
#define TMC5130_OFS229_SHIFT                          5 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS229_FIELD TMC5130_OFS229_MASK, TMC5130_OFS229_SHIFT
#define TMC5130_OFS230_MASK                           0x40 // MSLUT[7] // microstep table entry 230
#define TMC5130_OFS230_SHIFT                          6 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS230_FIELD TMC5130_OFS230_MASK, TMC5130_OFS230_SHIFT
#define TMC5130_OFS231_MASK                           0x80 // MSLUT[7] // microstep table entry 231
#define TMC5130_OFS231_SHIFT                          7 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS231_FIELD TMC5130_OFS231_MASK, TMC5130_OFS231_SHIFT
#define TMC5130_OFS232_MASK                           0x0100 // MSLUT[7] // microstep table entry 232
#define TMC5130_OFS232_SHIFT                          8 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS232_FIELD TMC5130_OFS232_MASK, TMC5130_OFS232_SHIFT
#define TMC5130_OFS233_MASK                           0x0200 // MSLUT[7] // microstep table entry 233
#define TMC5130_OFS233_SHIFT                          9 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS233_FIELD TMC5130_OFS233_MASK, TMC5130_OFS233_SHIFT
#define TMC5130_OFS234_MASK                           0x0400 // MSLUT[7] // microstep table entry 234
#define TMC5130_OFS234_SHIFT                          10 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS234_FIELD TMC5130_OFS234_MASK, TMC5130_OFS234_SHIFT
#define TMC5130_OFS235_MASK                           0x0800 // MSLUT[7] // microstep table entry 235
#define TMC5130_OFS235_SHIFT                          11 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS235_FIELD TMC5130_OFS235_MASK, TMC5130_OFS235_SHIFT
#define TMC5130_OFS236_MASK                           0x1000 // MSLUT[7] // microstep table entry 236
#define TMC5130_OFS236_SHIFT                          12 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS236_FIELD TMC5130_OFS236_MASK, TMC5130_OFS236_SHIFT
#define TMC5130_OFS237_MASK                           0x2000 // MSLUT[7] // microstep table entry 237
#define TMC5130_OFS237_SHIFT                          13 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS237_FIELD TMC5130_OFS237_MASK, TMC5130_OFS237_SHIFT
#define TMC5130_OFS238_MASK                           0x4000 // MSLUT[7] // microstep table entry 238
#define TMC5130_OFS238_SHIFT                          14 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS238_FIELD TMC5130_OFS238_MASK, TMC5130_OFS238_SHIFT
#define TMC5130_OFS239_MASK                           0x8000 // MSLUT[7] // microstep table entry 239
#define TMC5130_OFS239_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS239_FIELD TMC5130_OFS239_MASK, TMC5130_OFS239_SHIFT
#define TMC5130_OFS240_MASK                           0x010000 // MSLUT[7] // microstep table entry 240
#define TMC5130_OFS240_SHIFT                          16 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS240_FIELD TMC5130_OFS240_MASK, TMC5130_OFS240_SHIFT
#define TMC5130_OFS241_MASK                           0x020000 // MSLUT[7] // microstep table entry 241
#define TMC5130_OFS241_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS241_FIELD TMC5130_OFS241_MASK, TMC5130_OFS241_SHIFT
#define TMC5130_OFS242_MASK                           0x040000 // MSLUT[7] // microstep table entry 242
#define TMC5130_OFS242_SHIFT                          18 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS242_FIELD TMC5130_OFS242_MASK, TMC5130_OFS242_SHIFT
#define TMC5130_OFS243_MASK                           0x080000 // MSLUT[7] // microstep table entry 243
#define TMC5130_OFS243_SHIFT                          19 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS243_FIELD TMC5130_OFS243_MASK, TMC5130_OFS243_SHIFT
#define TMC5130_OFS244_MASK                           0x100000 // MSLUT[7] // microstep table entry 244
#define TMC5130_OFS244_SHIFT                          20 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS244_FIELD TMC5130_OFS244_MASK, TMC5130_OFS244_SHIFT
#define TMC5130_OFS245_MASK                           0x200000 // MSLUT[7] // microstep table entry 245
#define TMC5130_OFS245_SHIFT                          21 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS245_FIELD TMC5130_OFS245_MASK, TMC5130_OFS245_SHIFT
#define TMC5130_OFS246_MASK                           0x400000 // MSLUT[7] // microstep table entry 246
#define TMC5130_OFS246_SHIFT                          22 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS246_FIELD TMC5130_OFS246_MASK, TMC5130_OFS246_SHIFT
#define TMC5130_OFS247_MASK                           0x800000 // MSLUT[7] // microstep table entry 247
#define TMC5130_OFS247_SHIFT                          23 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS247_FIELD TMC5130_OFS247_MASK, TMC5130_OFS247_SHIFT
#define TMC5130_OFS248_MASK                           0x01000000 // MSLUT[7] // microstep table entry 248
#define TMC5130_OFS248_SHIFT                          24 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS248_FIELD TMC5130_OFS248_MASK, TMC5130_OFS248_SHIFT
#define TMC5130_OFS249_MASK                           0x02000000 // MSLUT[7] // microstep table entry 249
#define TMC5130_OFS249_SHIFT                          25 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS249_FIELD TMC5130_OFS249_MASK, TMC5130_OFS249_SHIFT
#define TMC5130_OFS250_MASK                           0x04000000 // MSLUT[7] // microstep table entry 250
#define TMC5130_OFS250_SHIFT                          26 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS250_FIELD TMC5130_OFS250_MASK, TMC5130_OFS250_SHIFT
#define TMC5130_OFS251_MASK                           0x08000000 // MSLUT[7] // microstep table entry 251
#define TMC5130_OFS251_SHIFT                          27 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS251_FIELD TMC5130_OFS251_MASK, TMC5130_OFS251_SHIFT
#define TMC5130_OFS252_MASK                           0x10000000 // MSLUT[7] // microstep table entry 252
#define TMC5130_OFS252_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS252_FIELD TMC5130_OFS252_MASK, TMC5130_OFS252_SHIFT
#define TMC5130_OFS253_MASK                           0x20000000 // MSLUT[7] // microstep table entry 253
#define TMC5130_OFS253_SHIFT                          29 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS253_FIELD TMC5130_OFS253_MASK, TMC5130_OFS253_SHIFT
#define TMC5130_OFS254_MASK                           0x40000000 // MSLUT[7] // microstep table entry 254
#define TMC5130_OFS254_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS254_FIELD TMC5130_OFS254_MASK, TMC5130_OFS254_SHIFT
#define TMC5130_OFS255_MASK                           0x80000000 // MSLUT[7] // microstep table entry 255
#define TMC5130_OFS255_SHIFT                          31 // min.: 0, max.: 1, default: 0
#define TMC5130_OFS255_FIELD TMC5130_OFS255_MASK, TMC5130_OFS255_SHIFT
#define TMC5130_W0_MASK                               0x03 // MSLUTSEL // LUT width select from ofs0 to ofs(X1-1)
#define TMC5130_W0_SHIFT                              0 // min.: 0, max.: 3, default: 0
#define TMC5130_W0_FIELD TMC5130_W0_MASK, TMC5130_W0_SHIFT
#define TMC5130_W1_MASK                               0x0C // MSLUTSEL // LUT width select from ofs(X1) to ofs(X2-1)
#define TMC5130_W1_SHIFT                              2 // min.: 0, max.: 3, default: 0
#define TMC5130_W1_FIELD TMC5130_W1_MASK, TMC5130_W1_SHIFT
#define TMC5130_W2_MASK                               0x30 // MSLUTSEL // LUT width select from ofs(X2) to ofs(X3-1)
#define TMC5130_W2_SHIFT                              4 // min.: 0, max.: 3, default: 0
#define TMC5130_W2_FIELD TMC5130_W2_MASK, TMC5130_W2_SHIFT
#define TMC5130_W3_MASK                               0xC0 // MSLUTSEL // LUT width select from ofs(X3) to ofs255
#define TMC5130_W3_SHIFT                              6 // min.: 0, max.: 3, default: 0
#define TMC5130_W3_FIELD TMC5130_W3_MASK, TMC5130_W3_SHIFT
#define TMC5130_X1_MASK                               0xFF00 // MSLUTSEL // LUT segment 1 start
#define TMC5130_X1_SHIFT                              8 // min.: 0, max.: 255, default: 0
#define TMC5130_X1_FIELD TMC5130_X1_MASK, TMC5130_X1_SHIFT
#define TMC5130_X2_MASK                               0xFF0000 // MSLUTSEL // LUT segment 2 start
#define TMC5130_X2_SHIFT                              16 // min.: 0, max.: 255, default: 0
#define TMC5130_X2_FIELD TMC5130_X2_MASK, TMC5130_X2_SHIFT
#define TMC5130_X3_MASK                               0xFF000000 // MSLUTSEL // LUT segment 3 start
#define TMC5130_X3_SHIFT                              24 // min.: 0, max.: 255, default: 0
#define TMC5130_X3_FIELD TMC5130_X3_MASK, TMC5130_X3_SHIFT
#define TMC5130_START_SIN_MASK                        0xFF // MSLUTSTART // Absolute current at microstep table entry 0.
#define TMC5130_START_SIN_SHIFT                       0 // min.: 0, max.: 255, default: 0
#define TMC5130_START_SIN_FIELD TMC5130_START_SIN_MASK, TMC5130_START_SIN_SHIFT
#define TMC5130_START_SIN90_MASK                      0xFF0000 // MSLUTSTART // Absolute current for microstep table entry at positions 256.
#define TMC5130_START_SIN90_SHIFT                     16 // min.: 0, max.: 255, default: 0
#define TMC5130_START_SIN90_FIELD TMC5130_START_SIN90_MASK, TMC5130_START_SIN90_SHIFT
#define TMC5130_MSCNT_MASK                            0x03FF // MSCNT // Microstep counter. Indicates actual position in the microstep table for CUR_A. CUR_B uses an offset of 256 (2 phase motor). Hint: Move to a position where MSCNT is zero before re-initializing MSLUTSTART or MSLUT and MSLUTSEL.
#define TMC5130_MSCNT_SHIFT                           0 // min.: 0, max.: 1023, default: 0
#define TMC5130_MSCNT_FIELD TMC5130_MSCNT_MASK, TMC5130_MSCNT_SHIFT
#define TMC5130_CUR_A_MASK                            0x01FF // MSCURACT // Actual microstep current for motor phase A as read from MSLUT (not scaled by current)
#define TMC5130_CUR_A_SHIFT                           0 // min.: -256, max.: 255, default: 0
#define TMC5130_CUR_A_FIELD TMC5130_CUR_A_MASK, TMC5130_CUR_A_SHIFT
#define TMC5130_CUR_B_MASK                            0x01FF0000 // MSCURACT // Actual microstep current for motor phase B as read from MSLUT (not scaled by current)
#define TMC5130_CUR_B_SHIFT                           16 // min.: -256, max.: 255, default: 0
#define TMC5130_CUR_B_FIELD TMC5130_CUR_B_MASK, TMC5130_CUR_B_SHIFT
#define TMC5130_TOFF_MASK                             0x0F // CHOPCONF // off time and driver enable
#define TMC5130_TOFF_SHIFT                            0 // min.: 0, max.: 15, default: 0
#define TMC5130_TOFF_FIELD TMC5130_TOFF_MASK, TMC5130_TOFF_SHIFT
#define TMC5130_TFD_ALL_MASK                          0x70 // CHOPCONF // fast decay time setting
#define TMC5130_TFD_ALL_SHIFT                         4 // min.: 0, max.: 7, default: 0
#define TMC5130_TFD_ALL_FIELD TMC5130_TFD_ALL_MASK, TMC5130_TFD_ALL_SHIFT
#define TMC5130_OFFSET_MASK                           0x0780 // CHOPCONF // sine wave offset
#define TMC5130_OFFSET_SHIFT                          7 // min.: 0, max.: 15, default: 0
#define TMC5130_OFFSET_FIELD TMC5130_OFFSET_MASK, TMC5130_OFFSET_SHIFT
#define TMC5130_TFD_3_MASK                            0x0800 // CHOPCONF // MSB of fast decay time setting
#define TMC5130_TFD_3_SHIFT                           11 // min.: 0, max.: 1, default: 0
#define TMC5130_TFD_3_FIELD TMC5130_TFD_3_MASK, TMC5130_TFD_3_SHIFT
#define TMC5130_DISFDCC_MASK                          0x1000 // CHOPCONF // fast decay mode
#define TMC5130_DISFDCC_SHIFT                         12 // min.: 0, max.: 1, default: 0
#define TMC5130_DISFDCC_FIELD TMC5130_DISFDCC_MASK, TMC5130_DISFDCC_SHIFT
#define TMC5130_RNDTF_MASK                            0x2000 // CHOPCONF // random TOFF time
#define TMC5130_RNDTF_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_RNDTF_FIELD TMC5130_RNDTF_MASK, TMC5130_RNDTF_SHIFT
#define TMC5130_CHM_MASK                              0x4000 // CHOPCONF // chopper mode
#define TMC5130_CHM_SHIFT                             14 // min.: 0, max.: 1, default: 0
#define TMC5130_CHM_FIELD TMC5130_CHM_MASK, TMC5130_CHM_SHIFT
#define TMC5130_TBL_MASK                              0x018000 // CHOPCONF // blank time select
#define TMC5130_TBL_SHIFT                             15 // min.: 0, max.: 3, default: 0
#define TMC5130_TBL_FIELD TMC5130_TBL_MASK, TMC5130_TBL_SHIFT
#define TMC5130_VSENSE_MASK                           0x020000 // CHOPCONF // sense resistor voltage based current scaling
#define TMC5130_VSENSE_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_VSENSE_FIELD TMC5130_VSENSE_MASK, TMC5130_VSENSE_SHIFT
#define TMC5130_VHIGHFS_MASK                          0x040000 // CHOPCONF // high velocity fullstep selection
#define TMC5130_VHIGHFS_SHIFT                         18 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHFS_FIELD TMC5130_VHIGHFS_MASK, TMC5130_VHIGHFS_SHIFT
#define TMC5130_VHIGHCHM_MASK                         0x080000 // CHOPCONF // high velocity chopper mode
#define TMC5130_VHIGHCHM_SHIFT                        19 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHCHM_FIELD TMC5130_VHIGHCHM_MASK, TMC5130_VHIGHCHM_SHIFT
#define TMC5130_SYNC_MASK                             0xF00000 // CHOPCONF // PWM synchronization clock
#define TMC5130_SYNC_SHIFT                            20 // min.: 0, max.: 15, default: 0
#define TMC5130_SYNC_FIELD TMC5130_SYNC_MASK, TMC5130_SYNC_SHIFT
#define TMC5130_MRES_MASK                             0x0F000000 // CHOPCONF // Micro step resolution; The resolution gives the number of microstep entries per sine quarter wave. The driver automatically uses microstep positions which result in a symmetrical wave, when choosing a lower microstep resolution.
#define TMC5130_MRES_SHIFT                            24 // min.: 0, max.: 8, default: 0
#define TMC5130_MRES_FIELD TMC5130_MRES_MASK, TMC5130_MRES_SHIFT
#define TMC5130_INTPOL_MASK                           0x10000000 // CHOPCONF // interpolation to 256 microsteps
#define TMC5130_INTPOL_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_INTPOL_FIELD TMC5130_INTPOL_MASK, TMC5130_INTPOL_SHIFT
#define TMC5130_DEDGE_MASK                            0x20000000 // CHOPCONF // enable double edge step pulses
#define TMC5130_DEDGE_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_DEDGE_FIELD TMC5130_DEDGE_MASK, TMC5130_DEDGE_SHIFT
#define TMC5130_DISS2G_MASK                           0x40000000 // CHOPCONF // short to GND protection disable
#define TMC5130_DISS2G_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_DISS2G_FIELD TMC5130_DISS2G_MASK, TMC5130_DISS2G_SHIFT
#define TMC5130_TOFF_MASK                             0x0F // CHOPCONF // off time and driver enable
#define TMC5130_TOFF_SHIFT                            0 // min.: 0, max.: 15, default: 0
#define TMC5130_TOFF_FIELD TMC5130_TOFF_MASK, TMC5130_TOFF_SHIFT
#define TMC5130_TFD_ALL_MASK                          0x70 // CHOPCONF // fast decay time setting
#define TMC5130_TFD_ALL_SHIFT                         4 // min.: 0, max.: 7, default: 0
#define TMC5130_TFD_ALL_FIELD TMC5130_TFD_ALL_MASK, TMC5130_TFD_ALL_SHIFT
#define TMC5130_OFFSET_MASK                           0x0780 // CHOPCONF // sine wave offset
#define TMC5130_OFFSET_SHIFT                          7 // min.: 0, max.: 15, default: 0
#define TMC5130_OFFSET_FIELD TMC5130_OFFSET_MASK, TMC5130_OFFSET_SHIFT
#define TMC5130_TFD_3_MASK                            0x0800 // CHOPCONF // MSB of fast decay time setting
#define TMC5130_TFD_3_SHIFT                           11 // min.: 0, max.: 1, default: 0
#define TMC5130_TFD_3_FIELD TMC5130_TFD_3_MASK, TMC5130_TFD_3_SHIFT
#define TMC5130_DISFDCC_MASK                          0x1000 // CHOPCONF // fast decay mode
#define TMC5130_DISFDCC_SHIFT                         12 // min.: 0, max.: 1, default: 0
#define TMC5130_DISFDCC_FIELD TMC5130_DISFDCC_MASK, TMC5130_DISFDCC_SHIFT
#define TMC5130_RNDTF_MASK                            0x2000 // CHOPCONF // random TOFF time
#define TMC5130_RNDTF_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_RNDTF_FIELD TMC5130_RNDTF_MASK, TMC5130_RNDTF_SHIFT
#define TMC5130_CHM_MASK                              0x4000 // CHOPCONF // chopper mode
#define TMC5130_CHM_SHIFT                             14 // min.: 0, max.: 1, default: 0
#define TMC5130_CHM_FIELD TMC5130_CHM_MASK, TMC5130_CHM_SHIFT
#define TMC5130_TBL_MASK                              0x018000 // CHOPCONF // blank time select
#define TMC5130_TBL_SHIFT                             15 // min.: 0, max.: 3, default: 0
#define TMC5130_TBL_FIELD TMC5130_TBL_MASK, TMC5130_TBL_SHIFT
#define TMC5130_VSENSE_MASK                           0x020000 // CHOPCONF // sense resistor voltage based current scaling
#define TMC5130_VSENSE_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_VSENSE_FIELD TMC5130_VSENSE_MASK, TMC5130_VSENSE_SHIFT
#define TMC5130_VHIGHFS_MASK                          0x040000 // CHOPCONF // high velocity fullstep selection
#define TMC5130_VHIGHFS_SHIFT                         18 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHFS_FIELD TMC5130_VHIGHFS_MASK, TMC5130_VHIGHFS_SHIFT
#define TMC5130_VHIGHCHM_MASK                         0x080000 // CHOPCONF // high velocity chopper mode
#define TMC5130_VHIGHCHM_SHIFT                        19 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHCHM_FIELD TMC5130_VHIGHCHM_MASK, TMC5130_VHIGHCHM_SHIFT
#define TMC5130_SYNC_MASK                             0xF00000 // CHOPCONF // PWM synchronization clock
#define TMC5130_SYNC_SHIFT                            20 // min.: 0, max.: 15, default: 0
#define TMC5130_SYNC_FIELD TMC5130_SYNC_MASK, TMC5130_SYNC_SHIFT
#define TMC5130_MRES_MASK                             0x0F000000 // CHOPCONF // Micro step resolution; The resolution gives the number of microstep entries per sine quarter wave. The driver automatically uses microstep positions which result in a symmetrical wave, when choosing a lower microstep resolution.
#define TMC5130_MRES_SHIFT                            24 // min.: 0, max.: 8, default: 0
#define TMC5130_MRES_FIELD TMC5130_MRES_MASK, TMC5130_MRES_SHIFT
#define TMC5130_INTPOL_MASK                           0x10000000 // CHOPCONF // interpolation to 256 microsteps
#define TMC5130_INTPOL_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_INTPOL_FIELD TMC5130_INTPOL_MASK, TMC5130_INTPOL_SHIFT
#define TMC5130_DEDGE_MASK                            0x20000000 // CHOPCONF // enable double edge step pulses
#define TMC5130_DEDGE_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_DEDGE_FIELD TMC5130_DEDGE_MASK, TMC5130_DEDGE_SHIFT
#define TMC5130_DISS2G_MASK                           0x40000000 // CHOPCONF // short to GND protection disable
#define TMC5130_DISS2G_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_DISS2G_FIELD TMC5130_DISS2G_MASK, TMC5130_DISS2G_SHIFT
#define TMC5130_TOFF_MASK                             0x0F // CHOPCONF // off time and driver enable
#define TMC5130_TOFF_SHIFT                            0 // min.: 0, max.: 15, default: 0
#define TMC5130_TOFF_FIELD TMC5130_TOFF_MASK, TMC5130_TOFF_SHIFT
#define TMC5130_HSTRT_MASK                            0x70 // CHOPCONF // hysteresis start value added to HEND; Attention: Effective HEND+HSTRT less than or equal to 16. Hint: Hysteresis decrement is done each 16 clocks
#define TMC5130_HSTRT_SHIFT                           4 // min.: 0, max.: 7, default: 0
#define TMC5130_HSTRT_FIELD TMC5130_HSTRT_MASK, TMC5130_HSTRT_SHIFT
#define TMC5130_HEND_MASK                             0x0780 // CHOPCONF // hysteresis low value
#define TMC5130_HEND_SHIFT                            7 // min.: 0, max.: 15, default: 0
#define TMC5130_HEND_FIELD TMC5130_HEND_MASK, TMC5130_HEND_SHIFT
#define TMC5130_RNDTF_MASK                            0x2000 // CHOPCONF // random TOFF time
#define TMC5130_RNDTF_SHIFT                           13 // min.: 0, max.: 1, default: 0
#define TMC5130_RNDTF_FIELD TMC5130_RNDTF_MASK, TMC5130_RNDTF_SHIFT
#define TMC5130_CHM_MASK                              0x4000 // CHOPCONF // chopper mode
#define TMC5130_CHM_SHIFT                             14 // min.: 0, max.: 1, default: 0
#define TMC5130_CHM_FIELD TMC5130_CHM_MASK, TMC5130_CHM_SHIFT
#define TMC5130_TBL_MASK                              0x018000 // CHOPCONF // blank time select
#define TMC5130_TBL_SHIFT                             15 // min.: 0, max.: 3, default: 0
#define TMC5130_TBL_FIELD TMC5130_TBL_MASK, TMC5130_TBL_SHIFT
#define TMC5130_VSENSE_MASK                           0x020000 // CHOPCONF // sense resistor voltage based current scaling
#define TMC5130_VSENSE_SHIFT                          17 // min.: 0, max.: 1, default: 0
#define TMC5130_VSENSE_FIELD TMC5130_VSENSE_MASK, TMC5130_VSENSE_SHIFT
#define TMC5130_VHIGHFS_MASK                          0x040000 // CHOPCONF // high velocity fullstep selection
#define TMC5130_VHIGHFS_SHIFT                         18 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHFS_FIELD TMC5130_VHIGHFS_MASK, TMC5130_VHIGHFS_SHIFT
#define TMC5130_VHIGHCHM_MASK                         0x080000 // CHOPCONF // high velocity chopper mode
#define TMC5130_VHIGHCHM_SHIFT                        19 // min.: 0, max.: 1, default: 0
#define TMC5130_VHIGHCHM_FIELD TMC5130_VHIGHCHM_MASK, TMC5130_VHIGHCHM_SHIFT
#define TMC5130_SYNC_MASK                             0xF00000 // CHOPCONF // PWM synchronization clock
#define TMC5130_SYNC_SHIFT                            20 // min.: 0, max.: 15, default: 0
#define TMC5130_SYNC_FIELD TMC5130_SYNC_MASK, TMC5130_SYNC_SHIFT
#define TMC5130_MRES_MASK                             0x0F000000 // CHOPCONF // Micro step resolution; The resolution gives the number of microstep entries per sine quarter wave. The driver automatically uses microstep positions which result in a symmetrical wave, when choosing a lower microstep resolution.
#define TMC5130_MRES_SHIFT                            24 // min.: 0, max.: 8, default: 0
#define TMC5130_MRES_FIELD TMC5130_MRES_MASK, TMC5130_MRES_SHIFT
#define TMC5130_INTPOL_MASK                           0x10000000 // CHOPCONF // interpolation to 256 microsteps
#define TMC5130_INTPOL_SHIFT                          28 // min.: 0, max.: 1, default: 0
#define TMC5130_INTPOL_FIELD TMC5130_INTPOL_MASK, TMC5130_INTPOL_SHIFT
#define TMC5130_DEDGE_MASK                            0x20000000 // CHOPCONF // enable double edge step pulses
#define TMC5130_DEDGE_SHIFT                           29 // min.: 0, max.: 1, default: 0
#define TMC5130_DEDGE_FIELD TMC5130_DEDGE_MASK, TMC5130_DEDGE_SHIFT
#define TMC5130_DISS2G_MASK                           0x40000000 // CHOPCONF // short to GND protection disable
#define TMC5130_DISS2G_SHIFT                          30 // min.: 0, max.: 1, default: 0
#define TMC5130_DISS2G_FIELD TMC5130_DISS2G_MASK, TMC5130_DISS2G_SHIFT
#define TMC5130_SEMIN_MASK                            0x0F // COOLCONF // minimum stallGuard2 value - If the stallGuard2 result falls below SEMIN*32, the motor current becomes increased to reduce motor load angle. 0: smart current control coolStep off
#define TMC5130_SEMIN_SHIFT                           0 // min.: 0, max.: 15, default: 0
#define TMC5130_SEMIN_FIELD TMC5130_SEMIN_MASK, TMC5130_SEMIN_SHIFT
#define TMC5130_SEUP_MASK                             0x60 // COOLCONF // Current increment steps per measured stallGuard2 value
#define TMC5130_SEUP_SHIFT                            5 // min.: 0, max.: 3, default: 0
#define TMC5130_SEUP_FIELD TMC5130_SEUP_MASK, TMC5130_SEUP_SHIFT
#define TMC5130_SEMAX_MASK                            0x0F00 // COOLCONF // stallGuard2 hysteresis value - If the stallGuard2 result is equal to or above (SEMIN+SEMAX+1)*32, the motor current becomes decreased to save energy.
#define TMC5130_SEMAX_SHIFT                           8 // min.: 0, max.: 15, default: 0
#define TMC5130_SEMAX_FIELD TMC5130_SEMAX_MASK, TMC5130_SEMAX_SHIFT
#define TMC5130_SEDN_MASK                             0x6000 // COOLCONF // Current down step speed; For each n stallGuard2 values decrease by one
#define TMC5130_SEDN_SHIFT                            13 // min.: 0, max.: 3, default: 0
#define TMC5130_SEDN_FIELD TMC5130_SEDN_MASK, TMC5130_SEDN_SHIFT
#define TMC5130_SEIMIN_MASK                           0x8000 // COOLCONF // minimum current for smart current control
#define TMC5130_SEIMIN_SHIFT                          15 // min.: 0, max.: 1, default: 0
#define TMC5130_SEIMIN_FIELD TMC5130_SEIMIN_MASK, TMC5130_SEIMIN_SHIFT
#define TMC5130_SGT_MASK                              0x7F0000 // COOLCONF // stallGuard2 threshold value This signed value controls stallGuard2 level for stall output and sets the optimum measurement range for readout. A lower value gives a higher sensitivity. Zero is the starting value working with most motors. A higher value makes stallGuard2 less sensitive and requires more torque to indicate a stall.
#define TMC5130_SGT_SHIFT                             16 // min.: -64, max.: 63, default: 0
#define TMC5130_SGT_FIELD TMC5130_SGT_MASK, TMC5130_SGT_SHIFT
#define TMC5130_SFILT_MASK                            0x01000000 // COOLCONF // stallGuard2 filter enable
#define TMC5130_SFILT_SHIFT                           24 // min.: 0, max.: 1, default: 0
#define TMC5130_SFILT_FIELD TMC5130_SFILT_MASK, TMC5130_SFILT_SHIFT
#define TMC5130_DC_TIME_MASK                          0x03FF // DCCTRL // Upper PWM on time limit for commutation (DC_TIME * 1/f CLK ). Set slightly above effective blank time TBL.
#define TMC5130_DC_TIME_SHIFT                         0 // min.: 0, max.: 1023, default: 0
#define TMC5130_DC_TIME_FIELD TMC5130_DC_TIME_MASK, TMC5130_DC_TIME_SHIFT
#define TMC5130_DC_SG_MASK                            0xFF0000 // DCCTRL // Max. PWM on time for step loss detection using dcStep stallGuard2 in dcStep mode. (DC_SG * 16/f CLK); Set slightly higher than DC_TIME/16; 0=disable
#define TMC5130_DC_SG_SHIFT                           16 // min.: 0, max.: 255, default: 0
#define TMC5130_DC_SG_FIELD TMC5130_DC_SG_MASK, TMC5130_DC_SG_SHIFT
#define TMC5130_SG_RESULT_MASK                        0x03FF // DRV_STATUS // Mechanical load measurement: The stallGuard2 result gives a means to measure mechanical motor load. A higher value means lower mechanical load. A value of 0 signals highest load. With optimum SGT setting, this is an indicator for a motor stall. The stall detection compares SG_RESULT to 0 in order to detect a stall. SG_RESULT is used as a base for coolStep operation, by comparing it to a programmable upper and a lower limit. It is not applicable in stealthChop mode. SG_RESULT is ALSO applicable when dcStep is active. stallGuard2 works best with microstep operation. Temperature measurement: In standstill, no stallGuard2 result can be obtained. SG_RESULT shows the chopper on-time for motor coil A instead. If the motor is moved to a determined microstep position at a certain current setting, a comparison of the chopper on-time can help to get a rough estimation of motor temperature. As the motor heats up, its coil resistance rises and the chopper on-time increases.
#define TMC5130_SG_RESULT_SHIFT                       0 // min.: 0, max.: 1023, default: 0
#define TMC5130_SG_RESULT_FIELD TMC5130_SG_RESULT_MASK, TMC5130_SG_RESULT_SHIFT
#define TMC5130_FSACTIVE_MASK                         0x8000 // DRV_STATUS // Full step active indicator
#define TMC5130_FSACTIVE_SHIFT                        15 // min.: 0, max.: 1, default: 0
#define TMC5130_FSACTIVE_FIELD TMC5130_FSACTIVE_MASK, TMC5130_FSACTIVE_SHIFT
#define TMC5130_CS_ACTUAL_MASK                        0x1F0000 // DRV_STATUS // Actual current control scaling, for monitoring smart energy current scaling controlled via settings in register COOLCONF, or for monitoring the function of the automatic current scaling.
#define TMC5130_CS_ACTUAL_SHIFT                       16 // min.: 0, max.: 31, default: 0
#define TMC5130_CS_ACTUAL_FIELD TMC5130_CS_ACTUAL_MASK, TMC5130_CS_ACTUAL_SHIFT
#define TMC5130_STALLGUARD_MASK                       0x01000000 // DRV_STATUS //
#define TMC5130_STALLGUARD_SHIFT                      24 // min.: 0, max.: 1, default: 0
#define TMC5130_STALLGUARD_FIELD TMC5130_STALLGUARD_MASK, TMC5130_STALLGUARD_SHIFT
#define TMC5130_OT_MASK                               0x02000000 // DRV_STATUS //
#define TMC5130_OT_SHIFT                              25 // min.: 0, max.: 1, default: 0
#define TMC5130_OT_FIELD TMC5130_OT_MASK, TMC5130_OT_SHIFT
#define TMC5130_OTPW_MASK                             0x04000000 // DRV_STATUS //
#define TMC5130_OTPW_SHIFT                            26 // min.: 0, max.: 1, default: 0
#define TMC5130_OTPW_FIELD TMC5130_OTPW_MASK, TMC5130_OTPW_SHIFT
#define TMC5130_S2GA_MASK                             0x08000000 // DRV_STATUS //
#define TMC5130_S2GA_SHIFT                            27 // min.: 0, max.: 1, default: 0
#define TMC5130_S2GA_FIELD TMC5130_S2GA_MASK, TMC5130_S2GA_SHIFT
#define TMC5130_S2GB_MASK                             0x10000000 // DRV_STATUS //
#define TMC5130_S2GB_SHIFT                            28 // min.: 0, max.: 1, default: 0
#define TMC5130_S2GB_FIELD TMC5130_S2GB_MASK, TMC5130_S2GB_SHIFT
#define TMC5130_OLA_MASK                              0x20000000 // DRV_STATUS //
#define TMC5130_OLA_SHIFT                             29 // min.: 0, max.: 1, default: 0
#define TMC5130_OLA_FIELD TMC5130_OLA_MASK, TMC5130_OLA_SHIFT
#define TMC5130_OLB_MASK                              0x40000000 // DRV_STATUS //
#define TMC5130_OLB_SHIFT                             30 // min.: 0, max.: 1, default: 0
#define TMC5130_OLB_FIELD TMC5130_OLB_MASK, TMC5130_OLB_SHIFT
#define TMC5130_STST_MASK                             0x80000000 // DRV_STATUS //
#define TMC5130_STST_SHIFT                            31 // min.: 0, max.: 1, default: 0
#define TMC5130_STST_FIELD TMC5130_STST_MASK, TMC5130_STST_SHIFT
#define TMC5130_PWM_AMPL_MASK                         0xFF // PWMCONF // User defined PWM amplitude offset (0-255) The resulting amplitude (limited to 0-255) is: PWM_AMPL + PWM_GRAD * 256 / TSTEP
#define TMC5130_PWM_AMPL_SHIFT                        0 // min.: 0, max.: 255, default: 0
#define TMC5130_PWM_AMPL_FIELD TMC5130_PWM_AMPL_MASK, TMC5130_PWM_AMPL_SHIFT
#define TMC5130_PWM_GRAD_MASK                         0xFF00 // PWMCONF // Velocity dependent gradient for PWM amplitude: PWM_GRAD * 256 / TSTEP is added to PWM_AMPL
#define TMC5130_PWM_GRAD_SHIFT                        8 // min.: 0, max.: 255, default: 0
#define TMC5130_PWM_GRAD_FIELD TMC5130_PWM_GRAD_MASK, TMC5130_PWM_GRAD_SHIFT
#define TMC5130_PWM_FREQ_MASK                         0x030000 // PWMCONF // PWM frequency selection
#define TMC5130_PWM_FREQ_SHIFT                        16 // min.: 0, max.: 3, default: 0
#define TMC5130_PWM_FREQ_FIELD TMC5130_PWM_FREQ_MASK, TMC5130_PWM_FREQ_SHIFT
#define TMC5130_PWM_AUTOSCALE_MASK                    0x040000 // PWMCONF // PWM automatic amplitude scaling
#define TMC5130_PWM_AUTOSCALE_SHIFT                   18 // min.: 0, max.: 1, default: 0
#define TMC5130_PWM_AUTOSCALE_FIELD TMC5130_PWM_AUTOSCALE_MASK, TMC5130_PWM_AUTOSCALE_SHIFT
#define TMC5130_PWM_SYMMETRIC_MASK                    0x080000 // PWMCONF // Force symmetric PWM
#define TMC5130_PWM_SYMMETRIC_SHIFT                   19 // min.: 0, max.: 1, default: 0
#define TMC5130_PWM_SYMMETRIC_FIELD TMC5130_PWM_SYMMETRIC_MASK, TMC5130_PWM_SYMMETRIC_SHIFT
#define TMC5130_FREEWHEEL_MASK                        0x300000 // PWMCONF // Allows different standstill modes
#define TMC5130_FREEWHEEL_SHIFT                       20 // min.: 0, max.: 3, default: 0
#define TMC5130_FREEWHEEL_FIELD TMC5130_FREEWHEEL_MASK, TMC5130_FREEWHEEL_SHIFT
#define TMC5130_PWM_AMPL_MASK                         0xFF // PWMCONF // User defined maximum PWM amplitude when switching back from current chopper mode to voltage PWM mode (switch over velocity defined by TPWMTHRS). Do not set too low values, as the regulation cannot measure the current when the actual PWM value goes below a setting specific value. Settings above 0x40 recommended
#define TMC5130_PWM_AMPL_SHIFT                        0 // min.: 0, max.: 255, default: 0
#define TMC5130_PWM_AMPL_FIELD TMC5130_PWM_AMPL_MASK, TMC5130_PWM_AMPL_SHIFT
#define TMC5130_PWM_GRAD_MASK                         0xFF00 // PWMCONF // User defined maximum PWM amplitude change per half wave (1 to 15)
#define TMC5130_PWM_GRAD_SHIFT                        8 // min.: 0, max.: 15, default: 0
#define TMC5130_PWM_GRAD_FIELD TMC5130_PWM_GRAD_MASK, TMC5130_PWM_GRAD_SHIFT
#define TMC5130_PWM_FREQ_MASK                         0x030000 // PWMCONF // PWM frequency selection
#define TMC5130_PWM_FREQ_SHIFT                        16 // min.: 0, max.: 3, default: 0
#define TMC5130_PWM_FREQ_FIELD TMC5130_PWM_FREQ_MASK, TMC5130_PWM_FREQ_SHIFT
#define TMC5130_PWM_AUTOSCALE_MASK                    0x040000 // PWMCONF // PWM automatic amplitude scaling
#define TMC5130_PWM_AUTOSCALE_SHIFT                   18 // min.: 0, max.: 1, default: 0
#define TMC5130_PWM_AUTOSCALE_FIELD TMC5130_PWM_AUTOSCALE_MASK, TMC5130_PWM_AUTOSCALE_SHIFT
#define TMC5130_PWM_SYMMETRIC_MASK                    0x080000 // PWMCONF // Force symmetric PWM
#define TMC5130_PWM_SYMMETRIC_SHIFT                   19 // min.: 0, max.: 1, default: 0
#define TMC5130_PWM_SYMMETRIC_FIELD TMC5130_PWM_SYMMETRIC_MASK, TMC5130_PWM_SYMMETRIC_SHIFT
#define TMC5130_FREEWHEEL_MASK                        0x300000 // PWMCONF // Allows different standstill modes
#define TMC5130_FREEWHEEL_SHIFT                       20 // min.: 0, max.: 3, default: 0
#define TMC5130_FREEWHEEL_FIELD TMC5130_FREEWHEEL_MASK, TMC5130_FREEWHEEL_SHIFT
#define TMC5130_PWM_SCALE_MASK                        0xFF // PWM_SCALE // Actual PWM amplitude scaler (255=max. Voltage) In voltage mode PWM, this value allows to detect a motor stall.
#define TMC5130_PWM_SCALE_SHIFT                       0 // min.: 0, max.: 255, default: 0
#define TMC5130_PWM_SCALE_FIELD TMC5130_PWM_SCALE_MASK, TMC5130_PWM_SCALE_SHIFT
#define TMC5130_INV_MASK                              0x01 // ENCM_CTRL //
#define TMC5130_INV_SHIFT                             0 // min.: 0, max.: 1, default: 0
#define TMC5130_INV_FIELD TMC5130_INV_MASK, TMC5130_INV_SHIFT
#define TMC5130_MAXSPEED_MASK                         0x02 // ENCM_CTRL //
#define TMC5130_MAXSPEED_SHIFT                        1 // min.: 0, max.: 1, default: 0
#define TMC5130_MAXSPEED_FIELD TMC5130_MAXSPEED_MASK, TMC5130_MAXSPEED_SHIFT
#define TMC5130_LOST_STEPS_MASK                       0x0FFFFF // LOST_STEPS // Number of input steps skipped due to higher load in dcStep operation, if step input does not stop when DC_OUT is low. This counter wraps around after 2^20 steps. Counts up or down depending on direction.
#define TMC5130_LOST_STEPS_SHIFT                      0 // min.: 0, max.: 1048575, default: 0
#define TMC5130_LOST_STEPS_FIELD TMC5130_LOST_STEPS_MASK, TMC5130_LOST_STEPS_SHIFT

#endif /* TMC5130_FIELDS_H */
