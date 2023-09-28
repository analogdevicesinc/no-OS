/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_cpu_macros.h
 * \brief Contains ADRV9025 API miscellaneous macro definitions for CPU
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADRV_9025_CPU_MACROS_H_
#define _ADRV_9025_CPU_MACROS_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define ADRV9025_CPU_JESD_FRAMERB_OFFSET                  0x28
#define ADRV9025_CPU_JESD_DEFRAMERB_OFFSET                0x50

/* The mailbox interface 3 bit error flag has a subset of error flags
* that are common to all cpu commands.
*/
#define ADRV9025_CPU_NUM_COMMON_ERR_FLAGS                 0x04
#define ADRV9025_CPU_NUM_COMMON_SET_ERR_FLAGS             0x05
#define ADRV9025_CPU_NUM_COMMON_GET_ERR_FLAGS             0x05

/* Cpu Error Flag Source */
#define ADRV9025_CPU_EFSRC_INITCALS                       0x10
#define ADRV9025_CPU_EFSRC_SETRFPLL                       0x20
#define ADRV9025_CPU_EFSRC_SETPENDING                     0x30
#define ADRV9025_CPU_EFSRC_SETGPIOCTRL                    0x40
#define ADRV9025_CPU_EFSRC_BOOTUP                         0x100

#define ADRV9025_CPU_ABORT_OPCODE                         0x00
#define ADRV9025_CPU_RUNINIT_OPCODE                       0x02
#define ADRV9025_CPU_RADIOON_OPCODE                       0x04
#define ADRV9025_CPU_SET_OPCODE                           0x0A
#define ADRV9025_CPU_GET_OPCODE                           0x0C
#define ADRV9025_CPU_TEST_OPCODE                          0x0E
#define ADRV9025_CPU_STREAM_TRIGGER_OPCODE                0x1F

#define ADRV9025_CPU_ORX1_TX_SEL0_SIGNALID                0x00
#define ADRV9025_CPU_ORX1_TX_SEL1_SIGNALID                0x01
#define ADRV9025_CPU_ORX2_TX_SEL0_SIGNALID                0x02
#define ADRV9025_CPU_ORX2_TX_SEL1_SIGNALID                0x03
#define ADRV9025_CPU_ORX3_TX_SEL0_SIGNALID                0x04
#define ADRV9025_CPU_ORX3_TX_SEL1_SIGNALID                0x05
#define ADRV9025_CPU_ORX4_TX_SEL0_SIGNALID                0x06
#define ADRV9025_CPU_ORX4_TX_SEL1_SIGNALID                0x07
#define ADRV9025_CPU_TX12CAL_ENA_SIGNALID                 0x08
#define ADRV9025_CPU_TX34CAL_ENA_SIGNALID                 0x09
#define ADRV9025_CPU_CAL_UPDATE0_SIGNALID                 0x0A
#define ADRV9025_CPU_CAL_UPDATE1_SIGNALID                 0x0B
#define ADRV9025_CPU_CAL_UPDATE2_SIGNALID                 0x0C
#define ADRV9025_CPU_RF_PLL_FREQ_HOP_SIGNALID             0x0D
#define ADRV9025_CPU_EXT_DPD_CAPTURE_TRIG_SIGNALID        0x0E
#define ADRV9025_CPU_EXT_DPD_MODEL_SEL0_SIGNALID          0x0F
#define ADRV9025_CPU_EXT_DPD_MODEL_SEL1_SIGNALID          0x10
#define ADRV9025_CPU_EXT_DPD_CAPTURE_DONE_SIGNALID        0x11

#define ADRV9025_CPU_OBJECTID_TXBBF_INIT                  0x00
#define ADRV9025_CPU_OBJECTID_ADCTUNER_INIT               0x01
#define ADRV9025_CPU_OBJECTID_TIA_INIT                    0x02
#define ADRV9025_CPU_OBJECTID_ORXTIA_INIT                 0x03
#define ADRV9025_CPU_OBJECTID_LBRXTIA_INIT                0x04
#define ADRV9025_CPU_OBJECTID_DCOFFSET_INIT               0x05
#define ADRV9025_CPU_OBJECTID_ORXDCOFFSET_INIT            0x06
#define ADRV9025_CPU_OBJECTID_LBRXDCOFFSET_INIT           0x07
#define ADRV9025_CPU_OBJECTID_ADCFLASH_INIT               0x08
#define ADRV9025_CPU_OBJECTID_PATHDELAY_INIT              0x09
#define ADRV9025_CPU_OBJECTID_TXLOLINT_INIT               0x0A
#define ADRV9025_CPU_OBJECTID_TXLOLEXT_INIT               0x0B
#define ADRV9025_CPU_OBJECTID_TXQEC_INIT                  0x0C
#define ADRV9025_CPU_OBJECTID_LBLODELAY_INIT              0x0D
#define ADRV9025_CPU_OBJECTID_LBRXTCAL_INIT               0x0E
#define ADRV9025_CPU_OBJECTID_RXLODELAY_INIT              0x0F
#define ADRV9025_CPU_OBJECTID_RXTCAL_INIT                 0x10
#define ADRV9025_CPU_OBJECTID_ORXLODELAY_INIT             0x11
#define ADRV9025_CPU_OBJECTID_ORXTCAL_INIT                0x12
#define ADRV9025_CPU_OBJECTID_TXDAC_INIT                  0x13
#define ADRV9025_CPU_OBJECTID_DPD_INIT                    0x14
#define ADRV9025_CPU_OBJECTID_CLGC_INIT                   0x15
#define ADRV9025_CPU_OBJECTID_VSWR_INIT                   0x16
#define ADRV9025_CPU_OBJECTID_HD2_INIT                    0x17
#define ADRV9025_CPU_OBJECTID_TXATTENDELAY_INIT           0x18
#define ADRV9025_CPU_OBJECTID_TXATTENTABLECAL_INIT        0x19
#define ADRV9025_CPU_OBJECTID_RXGAINDELAY_INIT            0x1A
#define ADRV9025_CPU_OBJECTID_RXGAINPHASECAL_INIT         0x1B
#define ADRV9025_CPU_OBJECTID_CFR_INIT                    0x1D

#define ADRV9025_CPU_OBJECTID_RXQEC_TRACKING              0x30
#define ADRV9025_CPU_OBJECTID_ORXQEC_TRACKING             0x31
#define ADRV9025_CPU_OBJECTID_TXLOL_TRACKING              0x32
#define ADRV9025_CPU_OBJECTID_TXQEC_TRACKING              0x33
#define ADRV9025_CPU_OBJECTID_DPD_TRACKING                0x34
#define ADRV9025_CPU_OBJECTID_CLGC_TRACKING               0x35
#define ADRV9025_CPU_OBJECTID_VSWR_TRACKING               0x36
#define ADRV9025_CPU_OBJECTID_RXHD2_TRACKING              0x37
#define ADRV9025_CPU_OBJECTID_DES_TRACKING                0x38

#define ADRV9025_CPU_OBJECTID_TEMP_SENSOR                 0x40
#define ADRV9025_CPU_OBJECTID_RSSI                        0x41
#define ADRV9025_CPU_OBJECTID_CAL_STATUS                  0x42
#define ADRV9025_CPU_OBJECTID_INITCAL_STATUS              0x43
#define ADRV9025_CPU_OBJECTID_GO_GET_GP_INT_SOURCE        0x46
#define ADRV9025_CPU_OBJECTID_TRACKING_CALS_STATE         0x47
#define ADRV9025_CPU_OBJECTID_BANDSWITCH_STAT             0x4B

#define ADRV9025_CPU_OBJECTID_GPIO_CTRL                   0x60
#define ADRV9025_CPU_OBJECTID_TX_ORX_MAPPING              0x4A
#define ADRV9025_CPU_OBJECTID_TXCAL_ENA_DIS               0x62
#define ADRV9025_CPU_OBJECTID_RFPLL_LO_FREQUENCY          0x63
#define ADRV9025_CPU_OBJECTID_MCS_DONE                    0x64
#define ADRV9025_CPU_OBJECTID_TRACKING_CAL_ENABLE         0x65
#define ADRV9025_CPU_OBJECTID_TRACKING_CAL_CTRL           0x66
#define ADRV9025_CPU_OBJECTID_TRACKING_CAL_PENDING        0x67
#define ADRV9025_CPU_OBJECTID_TRACKING_CAL_UPDATE         0x68
#define ADRV9025_CPU_OBJECTID_CPU_EXCEPTION               0x69
#define ADRV9025_CPU_OBJECTID_PLL_LOOP_BW                 0x6B
#define ADRV9025_CPU_OBJECTID_CONFIG                      0x6F
#define ADRV9025_CPU_OBJECTID_MCS                         0x70
#define ADRV9025_CPU_OBJECTID_TX_NCO                      0x71
#define ADRV9025_CPU_OBJECTID_EXT_DPD_ENABLE_DISABLE      0x73
#define ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_TRIG        0x74
#define ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE_DONE        0x75
#define ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL_SELECT        0x76
#define ADRV9025_CPU_OBJECTID_SRL_CONFIG                  0x77
#define ADRV9025_CPU_OBJECTID_SRL_STATISTICS              0x78
#define ADRV9025_CPU_OBJECTID_GS_RX_DDC                   0x79
#define ADRV9025_CPU_OBJECTID_CAPTURE_ORX                 0x7E
#define ADRV9025_CPU_OBJECTID_GS_TEST_MODE                0x7F

#define ADRV9025_CPU_OBJECTID_DEVICE_PROFILE              0x80
#define ADRV9025_CPU_OBJECTID_RADIO_CONTROL               0x81
#define ADRV9025_CPU_OBJECTID_INITIAL_CAL_SEQUENCE        0x82
#define ADRV9025_CPU_OBJECTID_CAL_SCHEDULER               0x83
#define ADRV9025_CPU_OBJECTID_HEALTH_MONITORING           0x84
#define ADRV9025_CPU_OBJECTID_ECC_CONFIGURATION           0x85
#define ADRV9025_CPU_OBJECTID_SYSTEM_INFO                 0x86
#define ADRV9025_CPU_OBJECTID_EXT_DPD_CAPTURE             0x88
#define ADRV9025_CPU_OBJECTID_EXT_DPD_MODEL               0x89
#define ADRV9025_CPU_OBJECTID_EXT_DPD_SWAP_MODE_EN        0x8A
#define ADRV9025_CPU_OBJECTID_EXT_DPD_GAIN_MONITOR        0x8B
#define ADRV9025_CPU_OBJECTID_DUALBANDCFG                 0x8D
#define ADRV9025_CPU_OBJECTID_EXT_DPD_MULTIMODEL          0x8E

#define ADRV9025_CPU_OBJECTID_NCO_DRIVER                  0xB0
#define ADRV9025_CPU_OBJECTID_STREAM_DRIVER               0xB1
#define ADRV9025_CPU_OBJECTID_PFIR_DRIVER                 0xB2
#define ADRV9025_CPU_OBJECTID_JESD_DRIVER                 0xB3
#define ADRV9025_CPU_OBJECTID_RXQEC_DRIVER                0xB4
#define ADRV9025_CPU_OBJECTID_TXATTEN_DRIVER              0xB5
#define ADRV9025_CPU_OBJECTID_TXQEC_DRIVER                0xB6

#define ADRV9025_CPU_OBJECTID_TM_SERDES_TASK_VERT_ISI     0x04

#define ADRV9025_CPU_OBJECTID_TM_CLKPLL_SWEEP             0xF0
#define ADRV9025_CPU_OBJECTID_TM_TTXBFF_3DB_SWEEP         0xF1
#define ADRV9025_CPU_OBJECTID_TM_TIA_3DB_SWEEP            0xF2
#define ADRV9025_CPU_OBJECTID_TM_LPBK_CTRL                0xF3
#define ADRV9025_CPU_OBJECTID_TM_SERDES                   0xFA
#define ADRV9025_CPU_OBJECTID_TM_EXIT                     0xFE
#define ADRV9025_CPU_OBJECTID_SYSTEM_CONFIG               0xFF

/* CPU-C memory map */
#define ADRV9025_CPU_C_ADDR_PROG_START                    0x01018000
#define ADRV9025_CPU_C_ADDR_PROG_END                      0x0104FFFF
#define ADRV9025_CPU_C_ADDR_DATA_START                    0x20028000
#define ADRV9025_CPU_C_ADDR_DATA_END                      0x2004FFFF
#define ADRV9025_CPU_C_ADDR_VERSION                       0x01018240
#define ADRV9025_CPU_C_ADDR_CFR_PULSE                     0x010182A8
#define ADRV9025_CPU_C_ADDR_DEV_PROFILE                   0x01018274
#define ADRV9025_CPU_C_ADDR_ADC_PROFILE                   0x01018278
#define ADRV9025_CPU_C_ADDR_DEBUG_PTR                     0x0101827C
#define ADRV9025_CPU_C_ADDR_BUILD_CHKSUM                  0x01047FFC
#define ADRV9025_CPU_C_ADDR_CALC_CHKSUM_PTR               0x01018270
#define ADRV9025_CPU_C_ADDR_MAILBOX_SET                   0x20028000
#define ADRV9025_CPU_C_ADDR_MAILBOX_GET                   0x20028100
#define ADRV9025_CPU_C_ADDR_MAILBOX_RUN_INIT              0x20028200
#define ADRV9025_CPU_C_ADDR_MAILBOX_FHM                   0x20028208

/* CPU-D memory map */
#define ADRV9025_CPU_D_INVALID_ADDRESS                    0xFFFFFFFF
#define ADRV9025_CPU_D_ADDR_PROG_START                    0x01000000
#define ADRV9025_CPU_D_ADDR_PROG_END                      0x01017FFF
#define ADRV9025_CPU_D_ADDR_DATA_START                    0x20000000
#define ADRV9025_CPU_D_ADDR_DATA_END                      0x20027FFF
#define ADRV9025_CPU_D_ADDR_VERSION                       0x01000240
#define ADRV9025_CPU_D_ADDR_CFR_PULSE                     ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_DEBUG_PTR                     ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_BUILD_CHKSUM                  ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_CALC_CHKSUM_PTR               ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_MAILBOX_SET                   ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_MAILBOX_GET                   ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_MAILBOX_RUN_INIT              ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */
#define ADRV9025_CPU_D_ADDR_MAILBOX_FHM                   ADRV9025_CPU_D_INVALID_ADDRESS /* ARM-D doesn't have this address */

/* ARM average temperature measurement */
#define ADRV9025_CPU_ADDR_AVG_TEMP_ADDR                   0x010002B0

/*StringVersion address in ARM*/
#define ADRV9025_CPU_ADDR_STREAM_VERSION                    0x2004E808
/* Cpu MailBox */
#define ADRV9025_CPU_ADDR_MAILBOX_STATUS_OFFSET           0x00
#define ADRV9025_CPU_MAILBOX_STATUS_OK                    0x00
#define ADRV9025_CPU_ADDR_MAILBOX_ERROR_OFFSET            0x01
#define ADRV9025_CPU_MAILBOX_ERROR_DEVICE_PROFILE         0x00
#define ADRV9025_CPU_MAILBOX_ERROR_ADC_PROFILE            0x01
#define ADRV9025_CPU_MAILBOX_ERROR_VCOFREGKHZ             0x02
#define ADRV9025_CPU_MAILBOX_ERROR_HSDIGHZ_API            0x03
#define ADRV9025_CPU_MAILBOX_ERROR_DEVICECLOCKHZ          0x04
#define ADRV9025_CPU_MAILBOX_ERROR_RXOUTPUTRATEHZ         0x05
#define ADRV9025_CPU_MAILBOX_ERROR_TXINPUTRATEHZ          0x06
#define ADRV9025_CPU_MAILBOX_ERROR_ORXOUTPUTRATEHZ        0x07

#define ADRV9025_CPU_ADC_PROFILE_SIZE                     56
/* number of uint16_t words per ADC profile */

#define ADRV9025_CPU_ADCP_ROFILE_NUM_CAP_COND_CURR_VALUES 16
#define ADRV9025_CPU_ADC_PROFILE_NUM_BIAS_VBG_VALUES      22
#define ADRV9025_CPU_ADC_PROFILE_NUM_DACG_VALUES          2
#define ADRV9025_CPU_ADC_PROFILE_NUM_ES1_VALUES           1
#define ADRV9025_CPU_ADC_PROFILE_NUM_ALTG_VALUES          1

/* Firmware Status */
#define ADRV9025_CPU_FW_STATUS_POWERUP                    0
#define ADRV9025_CPU_FW_STATUS_READY                      1
#define ADRV9025_CPU_FW_STATUS_FW_CHECKSUM_ERROR          2
#define ADRV9025_CPU_FW_STATUS_DATA_MEMORY_ERROR          3
#define ADRV9025_CPU_FW_STATUS_STREAM_CHECKSUM_ERROR      4
#define ADRV9025_CPU_FW_STATUS_PROFILE_ERROR              5
#define ADRV9025_CPU_FW_STATUS_CLKPLL_PROGRAMING_ERROR    6
#define ADRV9025_CPU_FW_STATUS_JESD_SETUP_CONFIG_ERROR    7
#define ADRV9025_CPU_FW_STATUS_POWER_UP_INIT_ERROR        8
#define ADRV9025_CPU_FW_STATUS_CPU_DEBUG_READY            9
#define ADRV9025_CPU_FW_STATUS_CLKLOGEN_ERROR             10
#define ADRV9025_CPU_FW_STATUS_RXQEC_HARDWARE_ERROR       11
#define ADRV9025_CPU_FW_STATUS_HW_TIMER_ERROR             12
#define ADRV9025_CPU_FW_STATUS_ADC_RCAL_ERROR             13
#define ADRV9025_CPU_FW_STATUS_STREAM_RUNTIME_ERROR       14
#define ADRV9025_CPU_FW_STATUS_BOOT_CLKGEN_RCAL_ERROR     15
#define ADRV9025_CPU_FW_STATUS_BOOT_LDO_CONFIG_ERROR      16
#define ADRV9025_CPU_FW_STATUS_INVCHN_CONFIG_ERROR        17
#define ADRV9025_CPU_D_FW_STATUS_CHECKSUM_ERROR           19
#define ADRV9025_CPU_D_FW_STATUS_BOOT_ERROR               20

#define ADRV9025_CPU_OPCODE_MASK                          0xFFFF0000
#define ADRV9025_CPU_OPCODE_SHIFT                         16
#define ADRV9025_CPU_OBJ_ID_MASK                          0x0000FF00
#define ADRV9025_CPU_OBJ_ID_SHIFT                         8
#define ADRV9025_CPU_ERROR_MASK                           0x000000FF
#define ADRV9025_CPU_ERROR_SHIFT                          1

#define ADRV9025_CPU_CMD_ERRCODE(armOpCode, armObjId, armErrorFlag)  \
    ((armOpCode << ADRV9025_CPU_OPCODE_SHIFT) |                      \
     (armObjId << ADRV9025_CPU_OBJ_ID_SHIFT) |                       \
     armErrorFlag)

#define ADRV9025_CPU_OPCODE_VALID(a) \
    (((a) != 0) && (((a) % 2) || ((a) > 30)))

#define ADRV9025_MAX_CPUS 2u
#define ADRV9025_MAX_CPU_FW_PATH 128u
#define ADRV9025_CPU_FW_PATH_SEP ';'

#ifdef __cplusplus
}
#endif

#endif /* _ADRV_9025_CPU__MACROS_H_ */
