/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC4671_VARIANTS_H_
#define TMC4671_VARIANTS_H_

#define CHIPINFO_ADDR_SI_TYPE                                   0
#define CHIPINFO_ADDR_SI_VERSION                                1
#define CHIPINFO_ADDR_SI_DATA                                   2
#define CHIPINFO_ADDR_SI_TIME                                   3
#define CHIPINFO_ADDR_SI_VARIANT                                4
#define CHIPINFO_ADDR_SI_BUILD                                  5
#define ADC_RAW_ADDR_ADC_I1_RAW_ADC_I0_RAW                      0
#define ADC_RAW_ADDR_ADC_AGPI_A_RAW_ADC_VM_RAW                  1
#define ADC_RAW_ADDR_ADC_AENC_UX_RAW_ADC_AGPI_B_RAW             2
#define ADC_RAW_ADDR_ADC_AENC_WY_RAW_ADC_AENC_VN_RAW            3
#define CONFIG_ADDR_biquad_x_a_1                                1
#define CONFIG_ADDR_biquad_x_a_2                                2
#define CONFIG_ADDR_biquad_x_b_0                                4
#define CONFIG_ADDR_biquad_x_b_1                                5
#define CONFIG_ADDR_biquad_x_b_2                                6
#define CONFIG_ADDR_biquad_x_enable                             7
#define CONFIG_ADDR_biquad_v_a_1                                9
#define CONFIG_ADDR_biquad_v_a_2                                10
#define CONFIG_ADDR_biquad_v_b_0                                12
#define CONFIG_ADDR_biquad_v_b_1                                13
#define CONFIG_ADDR_biquad_v_b_2                                14
#define CONFIG_ADDR_biquad_v_enable                             15
#define CONFIG_ADDR_biquad_t_a_1                                17
#define CONFIG_ADDR_biquad_t_a_2                                18
#define CONFIG_ADDR_biquad_t_b_0                                20
#define CONFIG_ADDR_biquad_t_b_1                                21
#define CONFIG_ADDR_biquad_t_b_2                                22
#define CONFIG_ADDR_biquad_t_enable                             23
#define CONFIG_ADDR_biquad_f_a_1                                25
#define CONFIG_ADDR_biquad_f_a_2                                26
#define CONFIG_ADDR_biquad_f_b_0                                28
#define CONFIG_ADDR_biquad_f_b_1                                29
#define CONFIG_ADDR_biquad_f_b_2                                30
#define CONFIG_ADDR_biquad_f_enable                             31
#define CONFIG_ADDR_prbs_amplitude                              32
#define CONFIG_ADDR_prbs_down_sampling_ratio                    33
#define CONFIG_ADDR_feed_forward_velocity_gain                  40
#define CONFIG_ADDR_feed_forward_velicity_filter_constant       41
#define CONFIG_ADDR_feed_forward_torque_gain                    42
#define CONFIG_ADDR_feed_forward_torgue_filter_constant         43
#define CONFIG_ADDR_VELOCITY_METER_PPTM_MIN_POS_DEV             50
#define CONFIG_ADDR_ref_switch_config                           51
#define CONFIG_ADDR_Encoder_Init_hall_Enable                    52
#define CONFIG_ADDR_SINGLE_PIN_IF_STATUS_CFG                    60
#define CONFIG_ADDR_SINGLE_PIN_IF_SCALE_OFFSET                  61
#define PID_ERROR_ADDR_PID_TORQUE_ERROR                         0
#define PID_ERROR_ADDR_PID_FLUX_ERROR                           1
#define PID_ERROR_ADDR_PID_VELOCITY_ERROR                       2
#define PID_ERROR_ADDR_PID_POSITION_ERROR                       3
#define PID_ERROR_ADDR_PID_TORQUE_ERROR_SUM                     4
#define PID_ERROR_ADDR_PID_FLUX_ERROR_SUM                       5
#define PID_ERROR_ADDR_PID_VELOCITY_ERROR_SUM                   6
#define PID_ERROR_ADDR_PID_POSITION_ERROR_SUM                   7
#define INTERIM_ADDR_PIDIN_TARGET_TORQUE                        0
#define INTERIM_ADDR_PIDIN_TARGET_FLUX                          1
#define INTERIM_ADDR_PIDIN_TARGET_VELOCITY                      2
#define INTERIM_ADDR_PIDIN_TARGET_POSITION                      3
#define INTERIM_ADDR_PIDOUT_TARGET_TORQUE                       4
#define INTERIM_ADDR_PIDOUT_TARGET_FLUX                         5
#define INTERIM_ADDR_PIDOUT_TARGET_VELOCITY                     6
#define INTERIM_ADDR_PIDOUT_TARGET_POSITION                     7
#define INTERIM_ADDR_FOC_IWY_IUX                                8
#define INTERIM_ADDR_FOC_IV                                     9
#define INTERIM_ADDR_FOC_IB_IA                                  10
#define INTERIM_ADDR_FOC_IQ_ID                                  11
#define INTERIM_ADDR_FOC_UQ_UD                                  12
#define INTERIM_ADDR_FOC_UQ_UD_LIMITED                          13
#define INTERIM_ADDR_FOC_UB_UA                                  14
#define INTERIM_ADDR_FOC_UWY_UUX                                15
#define INTERIM_ADDR_FOC_UV                                     16
#define INTERIM_ADDR_PWM_WY_UX                                  17
#define INTERIM_ADDR_PWM_UV                                     18
#define INTERIM_ADDR_ADC_I1_I0                                  19
#define INTERIM_ADDR_PID_TORQUE_TARGET_FLUX_TARGET_TORQUE_ACTUAL_FLUX_ACTUAL_DIV256 20
#define INTERIM_ADDR_PID_TORQUE_TARGET_TORQUE_ACTUAL            21
#define INTERIM_ADDR_PID_FLUX_TARGET_FLUX_ACTUAL                22
#define INTERIM_ADDR_PID_VELOCITY_TARGET_VELOCITY_ACTUAL_DIV256 23
#define INTERIM_ADDR_PID_VELOCITY_TARGET_VELOCITY_ACTUAL        24
#define INTERIM_ADDR_PID_POSITION_TARGET_POSITION_ACTUAL_DIV256 25
#define INTERIM_ADDR_PID_POSITION_TARGET_POSITION_ACTUAL        26
#define INTERIM_ADDR_FF_VELOCITY                                27
#define INTERIM_ADDR_FF_TORQUE                                  28
#define INTERIM_ADDR_ACTUAL_VELOCITY_PPTM                       29
#define INTERIM_ADDR_REF_SWITCH_STATUS                          30
#define INTERIM_ADDR_HOME_POSITION                              31
#define INTERIM_ADDR_LEFT_POSITION                              32
#define INTERIM_ADDR_RIGHT_POSITION                             33
#define INTERIM_ADDR_ENC_INIT_HALL_STATUS                       34
#define INTERIM_ADDR_ENC_INIT_HALL_PHI_E_ABN_OFFSET             35
#define INTERIM_ADDR_ENC_INIT_HALL_PHI_E_AENC_OFFSET            36
#define INTERIM_ADDR_ENC_INIT_HALL_PHI_A_AENC_OFFSET            37
#define INTERIM_ADDR_enc_init_mini_move_u_d_status              40
#define INTERIM_ADDR_enc_init_mini_move_phi_e_phi_e_offset      41
#define INTERIM_ADDR_SINGLE_PIN_IF_PWM_DUTY_CYCLE_TORQUE_TARGET 42
#define INTERIM_ADDR_SINGLE_PIN_IF_VELOCITY_TARGET              43
#define INTERIM_ADDR_SINGLE_PIN_IF_POSITION_TARGET              44
#define INTERIM_ADDR_DEBUG_VALUE_1_0                            192
#define INTERIM_ADDR_DEBUG_VALUE_3_2                            193
#define INTERIM_ADDR_DEBUG_VALUE_5_4                            194
#define INTERIM_ADDR_DEBUG_VALUE_7_6                            195
#define INTERIM_ADDR_DEBUG_VALUE_9_8                            196
#define INTERIM_ADDR_DEBUG_VALUE_11_10                          197
#define INTERIM_ADDR_DEBUG_VALUE_13_12                          198
#define INTERIM_ADDR_DEBUG_VALUE_15_14                          199
#define INTERIM_ADDR_DEBUG_VALUE_16                             200
#define INTERIM_ADDR_DEBUG_VALUE_17                             201
#define INTERIM_ADDR_DEBUG_VALUE_18                             202
#define INTERIM_ADDR_DEBUG_VALUE_19                             203
#define INTERIM_ADDR_CONFIG_REG_0                               208
#define INTERIM_ADDR_CONFIG_REG_1                               209
#define INTERIM_ADDR_CTRL_PARAM_10                              210
#define INTERIM_ADDR_CTRL_PARAM_32                              211
#define INTERIM_ADDR_STATUS_REG_0                               212
#define INTERIM_ADDR_STATUS_REG_1                               213
#define INTERIM_ADDR_STATUS_PARAM_10                            214
#define INTERIM_ADDR_STATUS_PARAM_32                            215

#endif /* TMC4671_VARIANTS_H_ */
