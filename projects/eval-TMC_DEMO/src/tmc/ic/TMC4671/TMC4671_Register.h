/*******************************************************************************
* Copyright © 2016 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC4671_REGISTERS_H_
#define TMC4671_REGISTERS_H_

	// ===== TMC4671 register set =====

	#define TMC4671_CHIPINFO_DATA                      0x00
	#define TMC4671_CHIPINFO_ADDR                      0x01

	#define TMC4671_ADC_RAW_DATA                       0x02
	#define TMC4671_ADC_RAW_ADDR                       0x03

	#define TMC4671_dsADC_MCFG_B_MCFG_A                0x04
	#define TMC4671_dsADC_MCLK_A                       0x05
	#define TMC4671_dsADC_MCLK_B                       0x06
	#define TMC4671_dsADC_MDEC_B_MDEC_A                0x07

	#define TMC4671_ADC_I1_SCALE_OFFSET                0x08
	#define TMC4671_ADC_I0_SCALE_OFFSET                0x09
	#define TMC4671_ADC_I_SELECT                       0x0A
	#define TMC4671_ADC_I1_I0_EXT                      0x0B

	#define TMC4671_DS_ANALOG_INPUT_STAGE_CFG          0x0C

	#define TMC4671_AENC_0_SCALE_OFFSET                0x0D
	#define TMC4671_AENC_1_SCALE_OFFSET                0x0E
	#define TMC4671_AENC_2_SCALE_OFFSET                0x0F

	#define TMC4671_AENC_SELECT                        0x11

	#define TMC4671_ADC_IWY_IUX                        0x12
	#define TMC4671_ADC_IV                             0x13
	#define TMC4671_AENC_WY_UX                         0x15
	#define TMC4671_AENC_VN                            0x16

	#define TMC4671_PWM_POLARITIES                     0x17
	#define TMC4671_PWM_MAXCNT                         0x18
	#define TMC4671_PWM_BBM_H_BBM_L                    0x19
	#define TMC4671_PWM_SV_CHOP                        0x1A
	#define TMC4671_MOTOR_TYPE_N_POLE_PAIRS            0x1B

	#define TMC4671_PHI_E_EXT                          0x1C
	#define TMC4671_PHI_M_EXT                          0x1D
	#define TMC4671_POSITION_EXT                       0x1E
	#define TMC4671_OPENLOOP_MODE                      0x1F
	#define TMC4671_OPENLOOP_ACCELERATION              0x20
	#define TMC4671_OPENLOOP_VELOCITY_TARGET           0x21
	#define TMC4671_OPENLOOP_VELOCITY_ACTUAL           0x22
	#define TMC4671_OPENLOOP_PHI                       0x23
	#define TMC4671_UQ_UD_EXT                          0x24

	#define TMC4671_ABN_DECODER_MODE                   0x25
	#define TMC4671_ABN_DECODER_PPR                    0x26
	#define TMC4671_ABN_DECODER_COUNT                  0x27
	#define TMC4671_ABN_DECODER_COUNT_N                0x28
	#define TMC4671_ABN_DECODER_PHI_E_PHI_M_OFFSET     0x29
	#define TMC4671_ABN_DECODER_PHI_E_PHI_M            0x2A

	#define TMC4671_ABN_2_DECODER_MODE                 0x2C
	#define TMC4671_ABN_2_DECODER_PPR                  0x2D
	#define TMC4671_ABN_2_DECODER_COUNT                0x2E
	#define TMC4671_ABN_2_DECODER_COUNT_N              0x2F
	#define TMC4671_ABN_2_DECODER_PHI_M_OFFSET         0x30
	#define TMC4671_ABN_2_DECODER_PHI_M                0x31

	#define TMC4671_HALL_MODE                          0x33
	#define TMC4671_HALL_POSITION_060_000              0x34
	#define TMC4671_HALL_POSITION_180_120              0x35
	#define TMC4671_HALL_POSITION_300_240              0x36
	#define TMC4671_HALL_PHI_E_PHI_M_OFFSET            0x37
	#define TMC4671_HALL_DPHI_MAX                      0x38
	#define TMC4671_HALL_PHI_E_INTERPOLATED_PHI_E      0x39
	#define TMC4671_HALL_PHI_M                         0x3A

	#define TMC4671_AENC_DECODER_MODE                  0x3B
	#define TMC4671_AENC_DECODER_N_MASK_N_THRESHOLD    0x3C
	#define TMC4671_AENC_DECODER_PHI_A_RAW             0x3D
	#define TMC4671_AENC_DECODER_PHI_A_OFFSET          0x3E
	#define TMC4671_AENC_DECODER_PHI_A                 0x3F

	#define TMC4671_AENC_DECODER_PPR                   0x40
	#define TMC4671_AENC_DECODER_COUNT                 0x41
	#define TMC4671_AENC_DECODER_COUNT_N               0x42
	#define TMC4671_AENC_DECODER_PHI_E_PHI_M_OFFSET    0x45
	#define TMC4671_AENC_DECODER_PHI_E_PHI_M           0x46
	#define TMC4671_AENC_DECODER_POSITION              0x47

	#define TMC4671_CONFIG_DATA                        0x4D
	#define TMC4671_CONFIG_ADDR                        0x4E

	#define TMC4671_VELOCITY_SELECTION                 0x50
	#define TMC4671_POSITION_SELECTION                 0x51
	#define TMC4671_PHI_E_SELECTION                    0x52
	#define TMC4671_PHI_E                              0x53

	#define TMC4671_PID_FLUX_P_FLUX_I                  0x54
	#define TMC4671_PID_TORQUE_P_TORQUE_I              0x56
	#define TMC4671_PID_VELOCITY_P_VELOCITY_I          0x58
	#define TMC4671_PID_POSITION_P_POSITION_I          0x5A
	#define TMC4671_PID_TORQUE_FLUX_TARGET_DDT_LIMITS  0x5C
	#define TMC4671_PIDOUT_UQ_UD_LIMITS                0x5D
	#define TMC4671_PID_TORQUE_FLUX_LIMITS             0x5E
	#define TMC4671_PID_ACCELERATION_LIMIT             0x5F
	#define TMC4671_PID_VELOCITY_LIMIT                 0x60
	#define TMC4671_PID_POSITION_LIMIT_LOW             0x61
	#define TMC4671_PID_POSITION_LIMIT_HIGH            0x62

	#define TMC4671_MODE_RAMP_MODE_MOTION              0x63
	#define TMC4671_PID_TORQUE_FLUX_TARGET             0x64
	#define TMC4671_PID_TORQUE_FLUX_OFFSET             0x65
	#define TMC4671_PID_VELOCITY_TARGET                0x66
	#define TMC4671_PID_VELOCITY_OFFSET                0x67
	#define TMC4671_PID_POSITION_TARGET                0x68

	#define TMC4671_PID_TORQUE_FLUX_ACTUAL             0x69
	#define TMC4671_PID_VELOCITY_ACTUAL                0x6A
	#define TMC4671_PID_POSITION_ACTUAL                0x6B

	#define TMC4671_PID_ERROR_DATA                     0x6C
	#define TMC4671_PID_ERROR_ADDR                     0x6D
	#define TMC4671_INTERIM_DATA                       0x6E
	#define TMC4671_INTERIM_ADDR                       0x6F

	#define TMC4671_WATCHDOG_CFG                       0x74
	#define TMC4671_ADC_VM_LIMITS                      0x75
	#define TMC4671_INPUTS_RAW                         0x76
	#define TMC4671_OUTPUTS_RAW                        0x77

	#define TMC4671_STEP_WIDTH                         0x78

	#define TMC4671_UART_BPS                           0x79
	#define TMC4671_UART_ADDRS                         0x7A

	#define TMC4671_GPIO_dsADCI_CONFIG                 0x7B

	#define TMC4671_STATUS_FLAGS                       0x7C
	#define TMC4671_STATUS_MASK                        0x7D

	// motor types
	#define TMC4671_NO_MOTOR					0
	#define TMC4671_SINGLE_PHASE_DC				1
	#define TMC4671_TWO_PHASE_STEPPER			2
	#define TMC4671_THREE_PHASE_BLDC			3

	// motion modes
	#define TMC4671_MOTION_MODE_STOPPED    		0
	#define TMC4671_MOTION_MODE_TORQUE     		1
	#define TMC4671_MOTION_MODE_VELOCITY   		2
	#define TMC4671_MOTION_MODE_POSITION   		3
	#define TMC4671_MOTION_MODE_PRBS_FLUX       4
	#define TMC4671_MOTION_MODE_PRBS_TORQUE     5
	#define TMC4671_MOTION_MODE_PRBS_VELOCITY   6
	#define TMC4671_MOTION_MODE_PRBS_POSITION   7
	#define TMC4671_MOTION_MODE_UQ_UD_EXT  		8

	// phi_e selections
	#define TMC4671_PHI_E_EXTERNAL   			1
	#define TMC4671_PHI_E_OPEN_LOOP  			2
	#define TMC4671_PHI_E_ABN        			3
	#define TMC4671_PHI_E_HALL       			5
	#define TMC4671_PHI_E_AENC      			6
	#define TMC4671_PHI_A_AENC       			7

	// velocity selection
    #define TMC4671_VELOCITY_PHI_E_SELECTION	0
	#define TMC4671_VELOCITY_PHI_E_EXT			1
	#define TMC4671_VELOCITY_PHI_E_OPENLOOP		2
	#define TMC4671_VELOCITY_PHI_E_ABN			3

	#define TMC4671_VELOCITY_PHI_E_HAL			5
	#define TMC4671_VELOCITY_PHI_E_AENC			6
	#define TMC4671_VELOCITY_PHI_A_AENC			7

	#define TMC4671_VELOCITY_PHI_M_ABN			9
	#define TMC4671_VELOCITY_PHI_M_ABN_2		10
	#define TMC4671_VELOCITY_PHI_M_AENC			11
	#define TMC4671_VELOCITY_PHI_M_HAL			12

	// position selection
	#define TMC4671_POSITION_PHI_E_SELECTION	0
	#define TMC4671_POSITION_PHI_E_EXT			1
	#define TMC4671_POSITION_PHI_E_OPENLOOP		2
	#define TMC4671_POSITION_PHI_E_ABN			3

	#define TMC4671_POSITION_PHI_E_HAL			5
	#define TMC4671_POSITION_PHI_E_AENC			6
	#define TMC4671_POSITION_PHI_A_AENC			7

	#define TMC4671_POSITION_PHI_M_ABN			9
	#define TMC4671_POSITION_PHI_M_ABN_2		10
	#define TMC4671_POSITION_PHI_M_AENC			11
	#define TMC4671_POSITION_PHI_M_HAL			12

#endif /* TMC4671_REGISTERS_H_ */
