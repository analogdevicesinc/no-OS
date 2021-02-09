/**
* \file
* \brief Contains ADRV9001 API miscellaneous macro definitions for ARM
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADRV9001_ARM_MACROS_H
#define _ADRV9001_ARM_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* ADI specific header files */


/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

/* The mailbox interface 3 bit error flag has a subset of error flags
 * that are common to all ARM commands.
 */
#define ADRV9001_NUM_COMMON_ARMERR_FLAGS					0x04
#define ADRV9001_NUM_COMMON_SETARMERR_FLAGS					0x05
#define ADRV9001_NUM_COMMON_GETARMERR_FLAGS					0x05

/* ARM Error Flag Source */
#define ADRV9001_ARM_EFSRC_INITCALS							0x10
#define ADRV9001_ARM_EFSRC_SETRFPLL							0x20
#define ADRV9001_ARM_EFSRC_SETPENDING						0x30
#define ADRV9001_ARM_EFSRC_SETGPIOCTRL						0x40
#define ADRV9001_ARM_EFSRC_BOOTUP							0x100

#define ADRV9001_ARM_ABORT_OPCODE							0x00
#define ADRV9001_ARM_RUNINIT_OPCODE							0x02
#define ADRV9001_ARM_RADIOON_OPCODE							0x04
#define ADRV9001_ARM_RADIOOFF_OPCODE						0x06
#define ADRV9001_ARM_STANDBY_OPCODE							0x08
#define ADRV9001_ARM_SET_OPCODE								0x0A
#define ADRV9001_ARM_GET_OPCODE								0x0C
#define ADRV9001_ARM_MCS_OPCODE                             0x0E
#define ADRV9001_ARM_HIGHPRIORITY_OPCODE					0x10
#define ADRV9001_ARM_POWERDOWN_OPCODE                       0x12
#define ADRV9001_ARM_POWERUP_OPCODE                         0x14
#define ADRV9001_ARM_STREAM_TRIGGER_OPCODE					0x1F
    
#define ADRV9001_ARM_HIGHPRIORITY_SET_RX_FREQCORRECTION		0x00
#define ADRV9001_ARM_HIGHPRIORITY_SET_POWER_SAVING_CONFIG   0x02
#define ADRV9001_ARM_HIGHPRIORITY_SET_MONITOR_MODE_CONFIG   0x03
#define ADRV9001_ARM_HIGHPRIORITY_SET_TX_FREQCORRECTION     0x05
#define ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SELECT   0x06
#define ADRV9001_ARM_HIGHPRIORITY_SET_FH_HOP_TABLE_SET      0x07

#define ADRV9001_ARM_OBJECTID_TEMP_SENSOR                   0x60
#define ADRV9001_ARM_OBJECTID_RSSI                          0x61
#define ADRV9001_ARM_OBJECTID_TX_PATH_DELAY_READ            0x66
#define ADRV9001_ARM_OBJECTID_RX_PATH_DELAY_READ            0x67
#define ADRV9001_ARM_OBJECTID_GET_POWER_SAVING_CONFIG       0x68
#define ADRV9001_ARM_OBJECTID_GET_MONITOR_MODE_CONFIG       0x69
#define ADRV9001_ARM_OBJECTID_ILB_ELB_PATH_DELAY_DIFF       0x6A
#define ADRV9001_ARM_OBJECTID_GET_CURRENT_ADC_TYPE          0x6B
#define ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE_SELECT       0x6C
#define ADRV9001_ARM_OBJECTID_GET_FH_HOP_TABLE              0x6D
#define ADRV9001_ARM_OBJECTID_GET_FH_FRAME_INFO             0x6E
#define ADRV9001_ARM_OBJECTID_MCS                           0x70
#define ADRV9001_ARM_OBJECTID_GPIO_CTRL                     0x80
#define ADRV9001_ARM_OBJECTID_CHANNEL_CARRIER_FREQUENCY     0x83
#define ADRV9001_ARM_OBJECTID_CHANNEL_PERFORM_ADC_SWITCH    0x84
#define ADRV9001_ARM_OBJECTID_EXTERNAL_PATH_DELAY           0x85
#define ADRV9001_ARM_OBJECTID_TRACKING_CALIBRATIONS_ENABLE  0x88
#define ADRV9001_ARM_OBJECTID_ARM_EXCEPTION                 0x89
#define ADRV9001_ARM_OBJECTID_PLL_LOOPFILTER                0x8B
#define ADRV9001_ARM_OBJECTID_DEVICE_PROFILE                0x8D
#define ADRV9001_ARM_OBJECTID_BBDC_ENABLE                   0x8E
#define ADRV9001_ARM_OBJECTID_CONFIG						0x8F
#define ADRV9001_ARM_OBJECTID_SRL_CONTROL                   0x90
#define ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN_CONTROL     0x91
#define ADRV9001_ARM_OBJECTID_RX_INTERFACE_GAIN             0x92
#define ADRV9001_ARM_OBJECTID_SYSTEM_CONFIG                 0x93    
#define ADRV9001_ARM_OBJECTID_CLOCK_ENABLE                  0x94
#define ADRV9001_ARM_OBJECTID_DYNAMIC_PROFILE               0x9A
#define ADRV9001_ARM_OBJECTID_FREQ_HOP_CONFIGURE            0x9B
#define ADRV9001_ARM_OBJECTID_TDD_TIMING_PARAMS             0x9C
#define ADRV9001_ARM_OBJECTID_CFG_RADIO_EVENT               0xA1
#define ADRV9001_ARM_OBJECTID_ADC_SWITCHING                 0xA6
#define ADRV9001_ARM_OBJECTID_LSSI_PADS_POWERDOWN           0xAF
#define ADRV9001_ARM_OBJECTID_PLL_CONFIG                    0xB1

/* ARM memory */
#define ADRV9001_ADDR_ARM_START_PROG					    0x01000000
#define ADRV9001_ADDR_ARM_CALC_CHKSUM_PTR				    0x01000400
#define ADRV9001_ADDR_ARM_VERSION						    0x0100040C
#define LDO_POWER_SAVING_MODES_LOCATION                     0x01000444
#define ADRV9001_ADDR_P_FH_PRECAL_TABLE                     0x0100044C
#define ADRV9001_ADDR_ARM_BUILD_CHKSUM					    0x01037FFC
//#define ADRV9001_ADDR_ARM_END_PROG						    0x0103FFFF  // TODO: Conditional for B0(this)/C0(below)
#define ADRV9001_ADDR_ARM_END_PROG						    0x01047FFF
#define ADRV9001_ADDR_ARM_START_DATA					    0x20000000
#define ADRV9001_ADDR_ARM_END_DATA						    0x2004FFFF

/* ARM program memory offset */
#define ADRV9001_ADDR_DEVICE_PROFILE_OFFSET                 0x00000404
#define ADRV9001_ADDR_PFIR_PROFILE_BUFFER_OFFSET            0x00000408
#define ADRV9001_ADDR_FH_HOP_TABLE_A_OFFSET                 0x00000450
#define ADRV9001_ADDR_FH_HOP_TABLE_B_OFFSET                 0x00000454

/* ARM checksum MailBox */
#define ADRV9001_ADDR_ARM_MAILBOX_SET					    0x20000000
#define ADRV9001_ADDR_ARM_MAILBOX_GET					    0x20000100
#define ADRV9001_ADDR_ARM_MAILBOX_RUN_INIT                  0x20000200
#define ADRV9001_ADDR_ARM_HIGHPRIORITY_MAILBOX_SET          0x20000210
#define ADRV9001_ADDR_ARM_MAILBOX_DYN_PROF                  0x20000230
#define ADRV9001_ADDR_ARM_MAILBOX_STATUS_OFFSET				0x00
#define ADRV9001_MAILBOX_STATUS_OK							0x00
#define ADRV9001_ADDR_ARM_MAILBOX_ERROR_OFFSET				0x01
#define ADRV9001_MAILBOX_ERROR_DEVICE_PROFILE				0x00

#define ADRV9001_ARM_MEM_READ_AUTOINCR                      0x1

#ifdef __cplusplus
}
#endif

#endif
