/* ================================================================================

     Project      :   ad74412_regs
     File         :   ad74412_regs.h
     Description  :   Register Definitions

     Date         :   Nov 30, 2017

     Copyright (c) 2017 Analog Devices, Inc.  All Rights Reserved.
     This software is proprietary and confidential to Analog Devices, Inc. and
     its licensors.

     This file was auto-generated. Do not make local changes to this file.

   ================================================================================ */

#ifndef _DEF_AD74412_REGS_H
#define _DEF_AD74412_REGS_H

#if defined(_LANGUAGE_C) || (defined(__GNUC__) && !defined(__ASSEMBLER__))
#include <stdint.h>
#endif /* _LANGUAGE_C */

#ifndef __ADI_GENERATED_DEF_HEADERS__
#define __ADI_GENERATED_DEF_HEADERS__    1
#endif

#define __ADI_HAS__    1

/* ============================================================================================================================
        User and test registers
   ============================================================================================================================ */

/* ============================================================================================================================
        ad74412_mmr0
   ============================================================================================================================ */
#define REG_AD74412_NOP_RESET           0x00000000            /*      Reset Value for NOP  */
#define REG_AD74412_NOP                 0x00000000            /*  ad74412_mmr0 No Operation Register */
#define REG_AD74412_CH_FUNC_SETUPn_RESET 0x00000000            /*      Reset Value for CH_FUNC_SETUP[n]  */
#define REG_AD74412_CH_FUNC_SETUP0_RESET 0x00000000            /*      Reset Value for REG_AD74412_CH_FUNC_SETUP0  */
#define REG_AD74412_CH_FUNC_SETUP1_RESET 0x00000000            /*      Reset Value for REG_AD74412_CH_FUNC_SETUP1  */
#define REG_AD74412_CH_FUNC_SETUP2_RESET 0x00000000            /*      Reset Value for REG_AD74412_CH_FUNC_SETUP2  */
#define REG_AD74412_CH_FUNC_SETUP3_RESET 0x00000000            /*      Reset Value for REG_AD74412_CH_FUNC_SETUP3  */
#define REG_AD74412_CH_FUNC_SETUP0      0x00000001            /*  ad74412_mmr0 Function Setup Register per Channel */
#define REG_AD74412_CH_FUNC_SETUP1      0x00000002            /*  ad74412_mmr0 Function Setup Register per Channel */
#define REG_AD74412_CH_FUNC_SETUP2      0x00000003            /*  ad74412_mmr0 Function Setup Register per Channel */
#define REG_AD74412_CH_FUNC_SETUP3      0x00000004            /*  ad74412_mmr0 Function Setup Register per Channel */
#define REG_AD74412_CH_FUNC_SETUPn(i)   (REG_AD74412_CH_FUNC_SETUP0 + ((i) * 1))
#define REG_AD74412_CH_FUNC_SETUPn_COUNT 4
#define REG_AD74412_ADC_CONFIGn_RESET   0x00000000            /*      Reset Value for ADC_CONFIG[n]  */
#define REG_AD74412_ADC_CONFIG0_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_CONFIG0  */
#define REG_AD74412_ADC_CONFIG1_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_CONFIG1  */
#define REG_AD74412_ADC_CONFIG2_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_CONFIG2  */
#define REG_AD74412_ADC_CONFIG3_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_CONFIG3  */
#define REG_AD74412_ADC_CONFIG0         0x00000005            /*  ad74412_mmr0 ADC Configuration Register per Channel */
#define REG_AD74412_ADC_CONFIG1         0x00000006            /*  ad74412_mmr0 ADC Configuration Register per Channel */
#define REG_AD74412_ADC_CONFIG2         0x00000007            /*  ad74412_mmr0 ADC Configuration Register per Channel */
#define REG_AD74412_ADC_CONFIG3         0x00000008            /*  ad74412_mmr0 ADC Configuration Register per Channel */
#define REG_AD74412_ADC_CONFIGn(i)      (REG_AD74412_ADC_CONFIG0 + ((i) * 1))
#define REG_AD74412_ADC_CONFIGn_COUNT   4
#define REG_AD74412_DIN_CONFIGn_RESET   0x0000000B            /*      Reset Value for DIN_CONFIG[n]  */
#define REG_AD74412_DIN_CONFIG0_RESET   0x0000000B            /*      Reset Value for REG_AD74412_DIN_CONFIG0  */
#define REG_AD74412_DIN_CONFIG1_RESET   0x0000000B            /*      Reset Value for REG_AD74412_DIN_CONFIG1  */
#define REG_AD74412_DIN_CONFIG2_RESET   0x0000000B            /*      Reset Value for REG_AD74412_DIN_CONFIG2  */
#define REG_AD74412_DIN_CONFIG3_RESET   0x0000000B            /*      Reset Value for REG_AD74412_DIN_CONFIG3  */
#define REG_AD74412_DIN_CONFIG0         0x00000009            /*  ad74412_mmr0 Digital Input Configuration Register per Channel */
#define REG_AD74412_DIN_CONFIG1         0x0000000A            /*  ad74412_mmr0 Digital Input Configuration Register per Channel */
#define REG_AD74412_DIN_CONFIG2         0x0000000B            /*  ad74412_mmr0 Digital Input Configuration Register per Channel */
#define REG_AD74412_DIN_CONFIG3         0x0000000C            /*  ad74412_mmr0 Digital Input Configuration Register per Channel */
#define REG_AD74412_DIN_CONFIGn(i)      (REG_AD74412_DIN_CONFIG0 + ((i) * 1))
#define REG_AD74412_DIN_CONFIGn_COUNT   4
#define REG_AD74412_GPO_PARALLEL_RESET  0x00000000            /*      Reset Value for GPO_PARALLEL  */
#define REG_AD74412_GPO_PARALLEL        0x0000000D            /*  ad74412_mmr0 GPO Parallel Data Register */
#define REG_AD74412_GPO_CONFIGn_RESET   0x00000000            /*      Reset Value for GPO_CONFIG[n]  */
#define REG_AD74412_GPO_CONFIG0_RESET   0x00000000            /*      Reset Value for REG_AD74412_GPO_CONFIG0  */
#define REG_AD74412_GPO_CONFIG1_RESET   0x00000000            /*      Reset Value for REG_AD74412_GPO_CONFIG1  */
#define REG_AD74412_GPO_CONFIG2_RESET   0x00000000            /*      Reset Value for REG_AD74412_GPO_CONFIG2  */
#define REG_AD74412_GPO_CONFIG3_RESET   0x00000000            /*      Reset Value for REG_AD74412_GPO_CONFIG3  */
#define REG_AD74412_GPO_CONFIG0         0x0000000E            /*  ad74412_mmr0 General Purpose Output Configuration Register per Channel */
#define REG_AD74412_GPO_CONFIG1         0x0000000F            /*  ad74412_mmr0 General Purpose Output Configuration Register per Channel */
#define REG_AD74412_GPO_CONFIG2         0x00000010            /*  ad74412_mmr0 General Purpose Output Configuration Register per Channel */
#define REG_AD74412_GPO_CONFIG3         0x00000011            /*  ad74412_mmr0 General Purpose Output Configuration Register per Channel */
#define REG_AD74412_GPO_CONFIGn(i)      (REG_AD74412_GPO_CONFIG0 + ((i) * 1))
#define REG_AD74412_GPO_CONFIGn_COUNT   4
#define REG_AD74412_OUTPUT_CONFIGn_RESET 0x00000000            /*      Reset Value for OUTPUT_CONFIG[n]  */
#define REG_AD74412_OUTPUT_CONFIG0_RESET 0x00000000            /*      Reset Value for REG_AD74412_OUTPUT_CONFIG0  */
#define REG_AD74412_OUTPUT_CONFIG1_RESET 0x00000000            /*      Reset Value for REG_AD74412_OUTPUT_CONFIG1  */
#define REG_AD74412_OUTPUT_CONFIG2_RESET 0x00000000            /*      Reset Value for REG_AD74412_OUTPUT_CONFIG2  */
#define REG_AD74412_OUTPUT_CONFIG3_RESET 0x00000000            /*      Reset Value for REG_AD74412_OUTPUT_CONFIG3  */
#define REG_AD74412_OUTPUT_CONFIG0      0x00000012            /*  ad74412_mmr0 Output Configuration Register per Channel */
#define REG_AD74412_OUTPUT_CONFIG1      0x00000013            /*  ad74412_mmr0 Output Configuration Register per Channel */
#define REG_AD74412_OUTPUT_CONFIG2      0x00000014            /*  ad74412_mmr0 Output Configuration Register per Channel */
#define REG_AD74412_OUTPUT_CONFIG3      0x00000015            /*  ad74412_mmr0 Output Configuration Register per Channel */
#define REG_AD74412_OUTPUT_CONFIGn(i)   (REG_AD74412_OUTPUT_CONFIG0 + ((i) * 1))
#define REG_AD74412_OUTPUT_CONFIGn_COUNT 4
#define REG_AD74412_DAC_CODEn_RESET     0x00000000            /*      Reset Value for DAC_CODE[n]  */
#define REG_AD74412_DAC_CODE0_RESET     0x00000000            /*      Reset Value for REG_AD74412_DAC_CODE0  */
#define REG_AD74412_DAC_CODE1_RESET     0x00000000            /*      Reset Value for REG_AD74412_DAC_CODE1  */
#define REG_AD74412_DAC_CODE2_RESET     0x00000000            /*      Reset Value for REG_AD74412_DAC_CODE2  */
#define REG_AD74412_DAC_CODE3_RESET     0x00000000            /*      Reset Value for REG_AD74412_DAC_CODE3  */
#define REG_AD74412_DAC_CODE0           0x00000016            /*  ad74412_mmr0 DAC Code Register per Channel. */
#define REG_AD74412_DAC_CODE1           0x00000017            /*  ad74412_mmr0 DAC Code Register per Channel. */
#define REG_AD74412_DAC_CODE2           0x00000018            /*  ad74412_mmr0 DAC Code Register per Channel. */
#define REG_AD74412_DAC_CODE3           0x00000019            /*  ad74412_mmr0 DAC Code Register per Channel. */
#define REG_AD74412_DAC_CODEn(i)        (REG_AD74412_DAC_CODE0 + ((i) * 1))
#define REG_AD74412_DAC_CODEn_COUNT     4
#define REG_AD74412_DAC_CLR_CODEn_RESET 0x00000000            /*      Reset Value for DAC_CLR_CODE[n]  */
#define REG_AD74412_DAC_CLR_CODE0_RESET 0x00000000            /*      Reset Value for REG_AD74412_DAC_CLR_CODE0  */
#define REG_AD74412_DAC_CLR_CODE1_RESET 0x00000000            /*      Reset Value for REG_AD74412_DAC_CLR_CODE1  */
#define REG_AD74412_DAC_CLR_CODE2_RESET 0x00000000            /*      Reset Value for REG_AD74412_DAC_CLR_CODE2  */
#define REG_AD74412_DAC_CLR_CODE3_RESET 0x00000000            /*      Reset Value for REG_AD74412_DAC_CLR_CODE3  */
#define REG_AD74412_DAC_CLR_CODE0       0x0000001A            /*  ad74412_mmr0 DAC Clear Code Register per Channel */
#define REG_AD74412_DAC_CLR_CODE1       0x0000001B            /*  ad74412_mmr0 DAC Clear Code Register per Channel */
#define REG_AD74412_DAC_CLR_CODE2       0x0000001C            /*  ad74412_mmr0 DAC Clear Code Register per Channel */
#define REG_AD74412_DAC_CLR_CODE3       0x0000001D            /*  ad74412_mmr0 DAC Clear Code Register per Channel */
#define REG_AD74412_DAC_CLR_CODEn(i)    (REG_AD74412_DAC_CLR_CODE0 + ((i) * 1))
#define REG_AD74412_DAC_CLR_CODEn_COUNT 4
#define REG_AD74412_DAC_ACTIVEn_RESET   0x00000000            /*      Reset Value for DAC_ACTIVE[n]  */
#define REG_AD74412_DAC_ACTIVE0_RESET   0x00000000            /*      Reset Value for REG_AD74412_DAC_ACTIVE0  */
#define REG_AD74412_DAC_ACTIVE1_RESET   0x00000000            /*      Reset Value for REG_AD74412_DAC_ACTIVE1  */
#define REG_AD74412_DAC_ACTIVE2_RESET   0x00000000            /*      Reset Value for REG_AD74412_DAC_ACTIVE2  */
#define REG_AD74412_DAC_ACTIVE3_RESET   0x00000000            /*      Reset Value for REG_AD74412_DAC_ACTIVE3  */
#define REG_AD74412_DAC_ACTIVE0         0x0000001E            /*  ad74412_mmr0 DAC Active Code Register per Channel */
#define REG_AD74412_DAC_ACTIVE1         0x0000001F            /*  ad74412_mmr0 DAC Active Code Register per Channel */
#define REG_AD74412_DAC_ACTIVE2         0x00000020            /*  ad74412_mmr0 DAC Active Code Register per Channel */
#define REG_AD74412_DAC_ACTIVE3         0x00000021            /*  ad74412_mmr0 DAC Active Code Register per Channel */
#define REG_AD74412_DAC_ACTIVEn(i)      (REG_AD74412_DAC_ACTIVE0 + ((i) * 1))
#define REG_AD74412_DAC_ACTIVEn_COUNT   4
#define REG_AD74412_DIN_THRESH_RESET    0x00000000            /*      Reset Value for DIN_THRESH  */
#define REG_AD74412_DIN_THRESH          0x00000022            /*  ad74412_mmr0 Digital Input Threshold Register */
#define REG_AD74412_ADC_CONV_CTRL_RESET 0x00000000            /*      Reset Value for ADC_CONV_CTRL  */
#define REG_AD74412_ADC_CONV_CTRL       0x00000023            /*  ad74412_mmr0 ADC Conversion Control Register. */
#define REG_AD74412_DIAG_ASSIGN_RESET   0x00000000            /*      Reset Value for DIAG_ASSIGN  */
#define REG_AD74412_DIAG_ASSIGN         0x00000024            /*  ad74412_mmr0 Diagnostics Select Register */
#define REG_AD74412_DIN_COMP_OUT_RESET  0x00000000            /*      Reset Value for DIN_COMP_OUT  */
#define REG_AD74412_DIN_COMP_OUT        0x00000025            /*  ad74412_mmr0 Digital Output Level Register */
#define REG_AD74412_ADC_RESULTn_RESET   0x00000000            /*      Reset Value for ADC_RESULT[n]  */
#define REG_AD74412_ADC_RESULT0_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_RESULT0  */
#define REG_AD74412_ADC_RESULT1_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_RESULT1  */
#define REG_AD74412_ADC_RESULT2_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_RESULT2  */
#define REG_AD74412_ADC_RESULT3_RESET   0x00000000            /*      Reset Value for REG_AD74412_ADC_RESULT3  */
#define REG_AD74412_ADC_RESULT0         0x00000026            /*  ad74412_mmr0 ADC Conversion Results Register per Channel */
#define REG_AD74412_ADC_RESULT1         0x00000027            /*  ad74412_mmr0 ADC Conversion Results Register per Channel */
#define REG_AD74412_ADC_RESULT2         0x00000028            /*  ad74412_mmr0 ADC Conversion Results Register per Channel */
#define REG_AD74412_ADC_RESULT3         0x00000029            /*  ad74412_mmr0 ADC Conversion Results Register per Channel */
#define REG_AD74412_ADC_RESULTn(i)      (REG_AD74412_ADC_RESULT0 + ((i) * 1))
#define REG_AD74412_ADC_RESULTn_COUNT   4
#define REG_AD74412_DIAG_RESULTn_RESET  0x00000000            /*      Reset Value for DIAG_RESULT[n]  */
#define REG_AD74412_DIAG_RESULT0_RESET  0x00000000            /*      Reset Value for REG_AD74412_DIAG_RESULT0  */
#define REG_AD74412_DIAG_RESULT1_RESET  0x00000000            /*      Reset Value for REG_AD74412_DIAG_RESULT1  */
#define REG_AD74412_DIAG_RESULT2_RESET  0x00000000            /*      Reset Value for REG_AD74412_DIAG_RESULT2  */
#define REG_AD74412_DIAG_RESULT3_RESET  0x00000000            /*      Reset Value for REG_AD74412_DIAG_RESULT3  */
#define REG_AD74412_DIAG_RESULT0        0x0000002A            /*  ad74412_mmr0 Diagnostic Results Registers per Diagnostic Channel. */
#define REG_AD74412_DIAG_RESULT1        0x0000002B            /*  ad74412_mmr0 Diagnostic Results Registers per Diagnostic Channel. */
#define REG_AD74412_DIAG_RESULT2        0x0000002C            /*  ad74412_mmr0 Diagnostic Results Registers per Diagnostic Channel. */
#define REG_AD74412_DIAG_RESULT3        0x0000002D            /*  ad74412_mmr0 Diagnostic Results Registers per Diagnostic Channel. */
#define REG_AD74412_DIAG_RESULTn(i)     (REG_AD74412_DIAG_RESULT0 + ((i) * 1))
#define REG_AD74412_DIAG_RESULTn_COUNT  4
#define REG_AD74412_ALERT_STATUS_RESET  0x00008000            /*      Reset Value for ALERT_STATUS  */
#define REG_AD74412_ALERT_STATUS        0x0000002E            /*  ad74412_mmr0 Alert Status Register */
#define REG_AD74412_LIVE_STATUS_RESET   0x00000000            /*      Reset Value for LIVE_STATUS  */
#define REG_AD74412_LIVE_STATUS         0x0000002F            /*  ad74412_mmr0 Live Status Register */
#define REG_AD74412_ALERT_MASK_RESET    0x00000000            /*      Reset Value for ALERT_MASK  */
#define REG_AD74412_ALERT_MASK          0x0000003C            /*  ad74412_mmr0 Alert Mask Register. */
#define REG_AD74412_READ_SELECT_RESET   0x00000000            /*      Reset Value for READ_SELECT  */
#define REG_AD74412_READ_SELECT         0x00000041            /*  ad74412_mmr0 Readback Select Register */
#define REG_AD74412_ADC_CONV_CTRL_40SPS_RESET 0x00000000            /*      Reset Value for ADC_CONV_CTRL_40SPS  */
#define REG_AD74412_ADC_CONV_CTRL_40SPS 0x00000042            /*  ad74412_mmr0 40SPS ADC Conversion Control Register. */
#define REG_AD74412_THERM_RST_RESET     0x00000000            /*      Reset Value for THERM_RST  */
#define REG_AD74412_THERM_RST           0x00000043            /*  ad74412_mmr0 Thermal Reset Enable Register */
#define REG_AD74412_CMD_KEY_RESET       0x00000000            /*      Reset Value for CMD_KEY  */
#define REG_AD74412_CMD_KEY             0x00000044            /*  ad74412_mmr0 Command Register */
#define REG_AD74412_SCRATCH_RESET       0x00000000            /*      Reset Value for SCRATCH  */
#define REG_AD74412_SCRATCH             0x00000045            /*  ad74412_mmr0 Scratch or Spare Register. */
#define REG_AD74412_SILICON_REV_RESET   0x00000001            /*      Reset Value for SILICON_REV  */
#define REG_AD74412_SILICON_REV         0x00000046            /*  ad74412_mmr0 Silicon Revision Register */

/* ============================================================================================================================
        ad74412_mmr Register BitMasks, Positions & Enumerations 
   ============================================================================================================================ */
/* -------------------------------------------------------------------------------------------------------------------------
          NOP                      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_NOP_NOP              0            /*  Write 0x0000 to Perform a NOP Command. */
#define BITM_NOP_NOP             0x0000FFFF    /*  Write 0x0000 to Perform a NOP Command. */

/* -------------------------------------------------------------------------------------------------------------------------
          CH_FUNC_SETUP[n]         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_CH_FUNC_SETUP_CH_FUNC  0            /*  Set the Channel Function. */
#define BITM_CH_FUNC_SETUP_CH_FUNC 0x0000000F    /*  Set the Channel Function. */
#define ENUM_CH_FUNC_SETUP_HIGH_IMP 0x00000000            /*  CH_FUNC: High Impedance */
#define ENUM_CH_FUNC_SETUP_VOUT  0x00000001            /*  CH_FUNC: Voltage Output */
#define ENUM_CH_FUNC_SETUP_IOUT  0x00000002            /*  CH_FUNC: Current Output */
#define ENUM_CH_FUNC_SETUP_VIN   0x00000003            /*  CH_FUNC: Voltage Input. */
#define ENUM_CH_FUNC_SETUP_IIN_EXT_PWR 0x00000004            /*  CH_FUNC: Current Input Externally powered. */
#define ENUM_CH_FUNC_SETUP_IIN_LOOP_PWR 0x00000005            /*  CH_FUNC: Current Input Loop powered. */
#define ENUM_CH_FUNC_SETUP_RES_MEAS 0x00000006            /*  CH_FUNC: Resistance Measurement. */
#define ENUM_CH_FUNC_SETUP_DIN_LOGIC 0x00000007            /*  CH_FUNC: Digital Input (Logic). */
#define ENUM_CH_FUNC_SETUP_DIN_LOOP 0x00000008            /*  CH_FUNC: Digital Input (Loop powered). */

/* -------------------------------------------------------------------------------------------------------------------------
          ADC_CONFIG[n]            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ADC_CONFIG_RANGE     5            /*  Selects the ADC Range. */
#define BITP_ADC_CONFIG_EN_50_60_HZ  3            /*  Enable 50/60 Hz Rejection and Set the ADC Conversion Rate for channel conversions(Separate bit in "ADC_CONV_CTRL" for diganostic conversions) */
#define BITP_ADC_CONFIG_CH_200K_TO_GND  2            /*  Enable the 200kO Resistor to Ground. */
#define BITP_ADC_CONFIG_ADC_MUX   0            /*  Selects the ADC Input Node. */
#define BITM_ADC_CONFIG_RANGE    0x000000E0    /*  Selects the ADC Range. */
#define BITM_ADC_CONFIG_EN_50_60_HZ 0x00000018    /*  Enable 50/60 Hz Rejection and Set the ADC Conversion Rate for channel conversions(Separate bit in "ADC_CONV_CTRL" for diganostic conversions) */
#define BITM_ADC_CONFIG_CH_200K_TO_GND 0x00000004    /*  Enable the 200kO Resistor to Ground. */
#define BITM_ADC_CONFIG_ADC_MUX  0x00000003    /*  Selects the ADC Input Node. */
#define ENUM_ADC_CONFIG_RNG_0_10V 0x00000000            /*  RANGE: 0V to 10V */
#define ENUM_ADC_CONFIG_RNG_0_2P5V 0x00000020            /*  RANGE: 0V to 2.5V */
#define ENUM_ADC_CONFIG_RNG_NEG2P5_0V 0x00000040            /*  RANGE: -2.5V to 0V */
#define ENUM_ADC_CONFIG_RNG_NEG2P5_2P5V 0x00000060            /*  RANGE: -2.5V to 2.5V */
#define ENUM_ADC_CONFIG_SPS_20   0x00000000            /*  EN_50_60_HZ: Enables the 50/60 Hz rejection resulting in a sampling rate of 20 Samples Per Second. */
#define ENUM_ADC_CONFIG_SPS_4K   0x00000008            /*  EN_50_60_HZ: Disables the 50/60 Hz rejection resulting in a sampling rate of 4k8 Samples Per Second. */
#define ENUM_ADC_CONFIG_V_ACROSS_SCREW 0x00000000            /*  ADC_MUX: Voltage between I/OP and AGND_SENSE */
#define ENUM_ADC_CONFIG_V_ACROSS_100OHM 0x00000001            /*  ADC_MUX: Voltage across 100O resistor. */

/* -------------------------------------------------------------------------------------------------------------------------
          DIN_CONFIG[n]            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DIN_CONFIG_COMP_INPUT_FILTERED 14            /*  Set to 1 to Select the Filtered Input to the Comparator. */
#define BITP_DIN_CONFIG_INV_DIN_COMP_OUT 13            /*  Set to 1 to Invert the Output from the Comparator. */
#define BITP_DIN_CONFIG_COMPARATOR_EN 12            /*  Set to 1 to Enable the Comparator. */
#define BITP_DIN_CONFIG_DIN_SINK  6            /*  Sets the Sink Current on the DIN Pins. */
#define BITP_DIN_CONFIG_DEBOUNCE_MODE  5            /*  This Bit Determines the Operation of the DIN Debounce Logic. */
#define BITP_DIN_CONFIG_DEBOUNCE_TIME  0            /*  This Bit Field Configures the Debounce Time on the DIN Pins. */
#define BITM_DIN_CONFIG_COMP_INPUT_FILTERED 0x00004000    /*  Set to 1 to Select the Filtered Input to the Comparator. */
#define BITM_DIN_CONFIG_INV_DIN_COMP_OUT 0x00002000    /*  Set to 1 to Invert the Output from the Comparator. */
#define BITM_DIN_CONFIG_COMPARATOR_EN 0x00001000    /*  Set to 1 to Enable the Comparator. */
#define BITM_DIN_CONFIG_DIN_SINK 0x000003C0    /*  Sets the Sink Current on the DIN Pins. */
#define BITM_DIN_CONFIG_DEBOUNCE_MODE 0x00000020    /*  This Bit Determines the Operation of the DIN Debounce Logic. */
#define BITM_DIN_CONFIG_DEBOUNCE_TIME 0x0000001F    /*  This Bit Field Configures the Debounce Time on the DIN Pins. */
#define ENUM_DIN_CONFIG_MODE_0   0x00000000            /*  DEBOUNCE_MODE: Debounce Mode 0 */
#define ENUM_DIN_CONFIG_MODE_1   0x00000020            /*  DEBOUNCE_MODE: Debounce Mode 1 */

/* -------------------------------------------------------------------------------------------------------------------------
          GPO_PARALLEL             Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_GPO_PARALLEL_GPO_PAR_DATA_D  3            /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_D pin. */
#define BITP_GPO_PARALLEL_GPO_PAR_DATA_C  2            /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_C pin. */
#define BITP_GPO_PARALLEL_GPO_PAR_DATA_B  1            /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_B pin. */
#define BITP_GPO_PARALLEL_GPO_PAR_DATA_A  0            /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_A pin. */
#define BITM_GPO_PARALLEL_GPO_PAR_DATA_D 0x00000008    /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_D pin. */
#define BITM_GPO_PARALLEL_GPO_PAR_DATA_C 0x00000004    /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_C pin. */
#define BITM_GPO_PARALLEL_GPO_PAR_DATA_B 0x00000002    /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_B pin. */
#define BITM_GPO_PARALLEL_GPO_PAR_DATA_A 0x00000001    /*  When a pad is configured for parallel GPO data, this Bitfield Sets the logic Level of the GPO_A pin. */

/* -------------------------------------------------------------------------------------------------------------------------
          GPO_CONFIG[n]            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_GPO_CONFIG_GPO_DATA  3            /*  This Bit Sets the GPO Logic state When GPO_SELECT = 001 */
#define BITP_GPO_CONFIG_GPO_SELECT  0            /*  Select the General Purpose Output Mode. */
#define BITM_GPO_CONFIG_GPO_DATA 0x00000008    /*  This Bit Sets the GPO Logic state When GPO_SELECT = 001 */
#define BITM_GPO_CONFIG_GPO_SELECT 0x00000007    /*  Select the General Purpose Output Mode. */
#define ENUM_GPO_CONFIG_GP_LOW   0x00000000            /*  GPO_DATA: Drive a logic low on GPO_n pin. */
#define ENUM_GPO_CONFIG_GP_HIGH  0x00000008            /*  GPO_DATA: Drive a logic high on GPO_n pin. */
#define ENUM_GPO_CONFIG_SEL_PULLD 0x00000000            /*  GPO_SELECT: The GPO pin is configured with a 100k pulldown resistor. */
#define ENUM_GPO_CONFIG_SEL_GPDATA 0x00000001            /*  GPO_SELECT: The GPO pin Logic state is set by the GPO_DATA bit. */
#define ENUM_GPO_CONFIG_SEL_GPDATA_PAR 0x00000002            /*  GPO_SELECT: The GPO pin is configured by the GPO_PAR_DATA bit in the GPO_PARALLEL register */
#define ENUM_GPO_CONFIG_SEL_DIN  0x00000003            /*  GPO_SELECT: The GPO pin is configured to output the debounced comparator output of the Digital Input Circuit */
#define ENUM_GPO_CONFIG_SEL_HIZ  0x00000004            /*  GPO_SELECT: The GPO pin is configured in a high impedance state. */

/* -------------------------------------------------------------------------------------------------------------------------
          OUTPUT_CONFIG[n]         Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_OUTPUT_CONFIG_SLEW_EN  6            /*  Set to 1 to Slew to the Requested DAC Code. */
#define BITP_OUTPUT_CONFIG_SLEW_LIN_STEP  4            /*  Step Size for Digital Linear Slew. */
#define BITP_OUTPUT_CONFIG_SLEW_LIN_RATE  2            /*  Update Rate for Digital Linear Slew. */
#define BITP_OUTPUT_CONFIG_CLR_EN  1            /*  Enable Clear for the Channel. */
#define BITP_OUTPUT_CONFIG_I_LIMIT  0            /*  This Bit Sets the Source Current Limit in VOUT Mode. */
#define BITM_OUTPUT_CONFIG_SLEW_EN 0x000000C0    /*  Set to 1 to Slew to the Requested DAC Code. */
#define BITM_OUTPUT_CONFIG_SLEW_LIN_STEP 0x00000030    /*  Step Size for Digital Linear Slew. */
#define BITM_OUTPUT_CONFIG_SLEW_LIN_RATE 0x0000000C    /*  Update Rate for Digital Linear Slew. */
#define BITM_OUTPUT_CONFIG_CLR_EN 0x00000002    /*  Enable Clear for the Channel. */
#define BITM_OUTPUT_CONFIG_I_LIMIT 0x00000001    /*  This Bit Sets the Source Current Limit in VOUT Mode. */
#define ENUM_OUTPUT_CONFIG_SLEW_CTRL_OFF 0x00000000            /*  SLEW_EN: Slewing disabled. */
#define ENUM_OUTPUT_CONFIG_SLEW_LINEAR 0x00000040            /*  SLEW_EN: Enable linear slew on the DAC output. */
#define ENUM_OUTPUT_CONFIG_STEP_64 0x00000000            /*  SLEW_LIN_STEP: Digital Linear slew step size of 64 codes. */
#define ENUM_OUTPUT_CONFIG_STEP_120 0x00000010            /*  SLEW_LIN_STEP: Digital Linear slew step size of 120 codes. */
#define ENUM_OUTPUT_CONFIG_STEP_500 0x00000020            /*  SLEW_LIN_STEP: Digital Linear slew step size of 500 codes. */
#define ENUM_OUTPUT_CONFIG_STEP_1820 0x00000030            /*  SLEW_LIN_STEP: Digital Linear slew step size of 1820 codes. */
#define ENUM_OUTPUT_CONFIG_RATE_4KHZ 0x00000000            /*  SLEW_LIN_RATE: The Digital Linear Slew controller updates at a rate of 4kHz. */
#define ENUM_OUTPUT_CONFIG_RATE_64KHZ 0x00000004            /*  SLEW_LIN_RATE: The Digital Linear Slew controller updates at a rate of 64kHz. */
#define ENUM_OUTPUT_CONFIG_RATE_150KHZ 0x00000008            /*  SLEW_LIN_RATE: The Digital Linear Slew controller updates at a rate of 150kHz. */
#define ENUM_OUTPUT_CONFIG_RATE_240KHZ 0x0000000C            /*  SLEW_LIN_RATE: The Digital Linear Slew controller updates at a rate of 240kHz. */
#define ENUM_OUTPUT_CONFIG_I_LIMIT_30MA 0x00000000            /*  I_LIMIT: 30mA Current Limit */
#define ENUM_OUTPUT_CONFIG_I_LIMIT_7P5MA 0x00000001            /*  I_LIMIT: 7.5mA Current Limit */

/* -------------------------------------------------------------------------------------------------------------------------
          DAC_CODE[n]              Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DAC_CODE_DAC_CODE    0            /*  13bit DAC Code Data for the Channel. */
#define BITM_DAC_CODE_DAC_CODE   0x00001FFF    /*  13bit DAC Code Data for the Channel. */

/* -------------------------------------------------------------------------------------------------------------------------
          DAC_CLR_CODE[n]          Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DAC_CLR_CODE_CLR_CODE  0            /*  DAC Clear Code for the Channel. */
#define BITM_DAC_CLR_CODE_CLR_CODE 0x00001FFF    /*  DAC Clear Code for the Channel. */

/* -------------------------------------------------------------------------------------------------------------------------
          DAC_ACTIVE[n]            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DAC_ACTIVE_DAC_ACTIVE_CODE  0            /*  The Active DAC Code on the Channel. */
#define BITM_DAC_ACTIVE_DAC_ACTIVE_CODE 0x00001FFF    /*  The Active DAC Code on the Channel. */

/* -------------------------------------------------------------------------------------------------------------------------
          DIN_THRESH               Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DIN_THRESH_COMP_THRESH  1            /*  Comparator Threshold. */
#define BITP_DIN_THRESH_DIN_THRESH_MODE  0            /*  This Bitfield Sets the Reference to the DIN Threshold DAC. */
#define BITM_DIN_THRESH_COMP_THRESH 0x0000003E    /*  Comparator Threshold. */
#define BITM_DIN_THRESH_DIN_THRESH_MODE 0x00000001    /*  This Bitfield Sets the Reference to the DIN Threshold DAC. */
#define ENUM_DIN_THRESH_REL_TO_AVDD 0x00000000            /*  DIN_THRESH_MODE: Threshold is set between gnd and AVDD. */
#define ENUM_DIN_THRESH_REL_TO_VREF 0x00000001            /*  DIN_THRESH_MODE: Threshold is set between gnd and a fixed voltage(TBD). */

/* -------------------------------------------------------------------------------------------------------------------------
          ADC_CONV_CTRL            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ADC_CONV_CTRL_EN_50_60_HZ_REJ_DIAG 10            /*  Enable 50/60Hz Rejection for Diagnostics. */
#define BITP_ADC_CONV_CTRL_CONV_SEQ  8            /*  Selects Single or Continuous Mode. */
#define BITP_ADC_CONV_CTRL_DIAG_3_EN  7            /*  Enable Conversions on Diagnostic 3 */
#define BITP_ADC_CONV_CTRL_DIAG_2_EN  6            /*  Enable Conversions on Diagnostic 2 */
#define BITP_ADC_CONV_CTRL_DIAG_1_EN  5            /*  Enable Conversions on Diagnostic 1 */
#define BITP_ADC_CONV_CTRL_DIAG_0_EN  4            /*  Enable Conversions on Diagnostic 0 */
#define BITP_ADC_CONV_CTRL_CH_D_EN  3            /*  Enable Conversions on ChannelD */
#define BITP_ADC_CONV_CTRL_CH_C_EN  2            /*  Enable Conversions on ChannelC */
#define BITP_ADC_CONV_CTRL_CH_B_EN  1            /*  Enable Conversions on ChannelB */
#define BITP_ADC_CONV_CTRL_CH_A_EN  0            /*  Enable Conversions on ChannelA */
#define BITM_ADC_CONV_CTRL_EN_50_60_HZ_REJ_DIAG 0x00000400    /*  Enable 50/60Hz Rejection for Diagnostics. */
#define BITM_ADC_CONV_CTRL_CONV_SEQ 0x00000300    /*  Selects Single or Continuous Mode. */
#define BITM_ADC_CONV_CTRL_DIAG_3_EN 0x00000080    /*  Enable Conversions on Diagnostic 3 */
#define BITM_ADC_CONV_CTRL_DIAG_2_EN 0x00000040    /*  Enable Conversions on Diagnostic 2 */
#define BITM_ADC_CONV_CTRL_DIAG_1_EN 0x00000020    /*  Enable Conversions on Diagnostic 1 */
#define BITM_ADC_CONV_CTRL_DIAG_0_EN 0x00000010    /*  Enable Conversions on Diagnostic 0 */
#define BITM_ADC_CONV_CTRL_CH_D_EN 0x00000008    /*  Enable Conversions on ChannelD */
#define BITM_ADC_CONV_CTRL_CH_C_EN 0x00000004    /*  Enable Conversions on ChannelC */
#define BITM_ADC_CONV_CTRL_CH_B_EN 0x00000002    /*  Enable Conversions on ChannelB */
#define BITM_ADC_CONV_CTRL_CH_A_EN 0x00000001    /*  Enable Conversions on ChannelA */
#define ENUM_ADC_CONV_CTRL_IDLE  0x00000000            /*  CONV_SEQ: Stop continuous conversions and leave the ADC powered up or power up the ADC. */
#define ENUM_ADC_CONV_CTRL_SINGLE 0x00000100            /*  CONV_SEQ: Start single sequence conversion. */
#define ENUM_ADC_CONV_CTRL_CONTINUOUS 0x00000200            /*  CONV_SEQ: Start Continuous Conversions. */
#define ENUM_ADC_CONV_CTRL_ADC_PWRDWN 0x00000300            /*  CONV_SEQ: Stop continuous conversions and power down the ADC  or power down the ADC. */

#if 0
/* -------------------------------------------------------------------------------------------------------------------------
          DIAG_ASSIGN              Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DIAG_ASSIGN_DIAG3_ASSIGN 12            /*  Selects the Diagnostic Assigned to DIAG_RESULT[3]. */
#define BITP_DIAG_ASSIGN_DIAG2_ASSIGN  8            /*  Selects the Diagnostic Assigned to DIAG_RESULT[2]. */
#define BITP_DIAG_ASSIGN_DIAG1_ASSIGN  4            /*  Selects the Diagnostic Assigned to DIAG_RESULT[1]. */
#define BITP_DIAG_ASSIGN_DIAG0_ASSIGN  0            /*  Selects the Diagnostic Assigned to DIAG_RESULT[0]. */
#define BITM_DIAG_ASSIGN_DIAG3_ASSIGN 0x0000F000    /*  Selects the Diagnostic Assigned to DIAG_RESULT[3]. */
#define BITM_DIAG_ASSIGN_DIAG2_ASSIGN 0x00000F00    /*  Selects the Diagnostic Assigned to DIAG_RESULT[2]. */
#define BITM_DIAG_ASSIGN_DIAG1_ASSIGN 0x000000F0    /*  Selects the Diagnostic Assigned to DIAG_RESULT[1]. */
#define BITM_DIAG_ASSIGN_DIAG0_ASSIGN 0x0000000F    /*  Selects the Diagnostic Assigned to DIAG_RESULT[0]. */
#define ENUM_DIAG_ASSIGN_AGND    0x00000000            /*  DIAG3_ASSIGN: Assign AGND to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_TEMP    0x00001000            /*  DIAG3_ASSIGN: Assign the Temperature Sensor to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_AVDD    0x00002000            /*  DIAG3_ASSIGN: Assign AVDD/16 to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_CHRG_PUMP 0x00003000            /*  DIAG3_ASSIGN: Assign the Charge Pump Voltage (AVSS) to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_REFOUT  0x00004000            /*  DIAG3_ASSIGN: Assign REFOUT to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_ALDO5V  0x00005000            /*  DIAG3_ASSIGN: Assign ALDO5V to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_ALDO1V8 0x00006000            /*  DIAG3_ASSIGN: Assign ALDO1V8 to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_DLDO1V8 0x00007000            /*  DIAG3_ASSIGN: Assign DLDO1V8 to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_DVCC    0x00008000            /*  DIAG3_ASSIGN: Assign DVCC to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_IOVDD   0x00009000            /*  DIAG3_ASSIGN: Assign IOVDD to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_SENSEL_A 0x0000A000            /*  DIAG3_ASSIGN: Assign SensL_A to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_SENSEL_B 0x0000B000            /*  DIAG3_ASSIGN: Assign SensL_B to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_SENSEL_C 0x0000C000            /*  DIAG3_ASSIGN: Assign SensL_C to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_SENSEL_D 0x0000D000            /*  DIAG3_ASSIGN: Assign SensL_D to Diagnostic 3. */
#define ENUM_DIAG_ASSIGN_AGND    0x00000000            /*  DIAG2_ASSIGN: Assign AGND to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_TEMP    0x00000100            /*  DIAG2_ASSIGN: Assign the Temperature Sensor to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_AVDD    0x00000200            /*  DIAG2_ASSIGN: Assign AVDD/16 to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_CHRG_PUMP 0x00000300            /*  DIAG2_ASSIGN: Assign the Charge Pump Voltage (AVSS) to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_REFOUT  0x00000400            /*  DIAG2_ASSIGN: Assign REFOUT to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_ALDO5V  0x00000500            /*  DIAG2_ASSIGN: Assign ALDO5V to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_ALDO1V8 0x00000600            /*  DIAG2_ASSIGN: Assign ALDO1V8 to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_DLDO1V8 0x00000700            /*  DIAG2_ASSIGN: Assign DLDO1V8 to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_DVCC    0x00000800            /*  DIAG2_ASSIGN: Assign DVCC to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_IOVDD   0x00000900            /*  DIAG2_ASSIGN: Assign IOVDD to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_SENSEL_A 0x00000A00            /*  DIAG2_ASSIGN: Assign SensL_A to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_SENSEL_B 0x00000B00            /*  DIAG2_ASSIGN: Assign SensL_B to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_SENSEL_C 0x00000C00            /*  DIAG2_ASSIGN: Assign SensL_C to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_SENSEL_D 0x00000D00            /*  DIAG2_ASSIGN: Assign SensL_D to Diagnostic 2. */
#define ENUM_DIAG_ASSIGN_AGND    0x00000000            /*  DIAG1_ASSIGN: Assign AGND to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_TEMP    0x00000010            /*  DIAG1_ASSIGN: Assign the Temperature Sensor to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_AVDD    0x00000020            /*  DIAG1_ASSIGN: Assign AVDD/16 to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_CHRG_PUMP 0x00000030            /*  DIAG1_ASSIGN: Assign the Charge Pump Voltage (AVSS) to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_REFOUT  0x00000040            /*  DIAG1_ASSIGN: Assign REFOUT to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_ALDO5V  0x00000050            /*  DIAG1_ASSIGN: Assign ALDO5V to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_ALDO1V8 0x00000060            /*  DIAG1_ASSIGN: Assign ALDO1V8 to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_DLDO1V8 0x00000070            /*  DIAG1_ASSIGN: Assign DLDO1V8 to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_DVCC    0x00000080            /*  DIAG1_ASSIGN: Assign DVCC to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_IOVDD   0x00000090            /*  DIAG1_ASSIGN: Assign IOVDD to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_SENSEL_A 0x000000A0            /*  DIAG1_ASSIGN: Assign SensL_A to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_SENSEL_B 0x000000B0            /*  DIAG1_ASSIGN: Assign SensL_B to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_SENSEL_C 0x000000C0            /*  DIAG1_ASSIGN: Assign SensL_C to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_SENSEL_D 0x000000D0            /*  DIAG1_ASSIGN: Assign SensL_D to Diagnostic 1. */
#define ENUM_DIAG_ASSIGN_AGND    0x00000000            /*  DIAG0_ASSIGN: Assign AGND to Diagnostic 0 */
#define ENUM_DIAG_ASSIGN_TEMP    0x00000001            /*  DIAG0_ASSIGN: Assign the Temperature Sensor to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_AVDD    0x00000002            /*  DIAG0_ASSIGN: Assign AVDD/16 to Diagnostic 0 */
#define ENUM_DIAG_ASSIGN_CHRG_PUMP 0x00000003            /*  DIAG0_ASSIGN: Assign the Charge Pump Voltage (AVSS) to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_REFOUT  0x00000004            /*  DIAG0_ASSIGN: Assign REFOUT*0.8 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_ALDO5V  0x00000005            /*  DIAG0_ASSIGN: Assign ALDO5V/7 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_ALDO1V8 0x00000006            /*  DIAG0_ASSIGN: Assign ALDO1V8/2.33 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_DLDO1V8 0x00000007            /*  DIAG0_ASSIGN: Assign DLDO1V8/3 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_DVCC    0x00000008            /*  DIAG0_ASSIGN: Assign DVCC/3.3 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_IOVDD   0x00000009            /*  DIAG0_ASSIGN: Assign IOVDD/3.3 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_SENSEL_A 0x0000000A            /*  DIAG0_ASSIGN: Assign SensL_A/12 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_SENSEL_B 0x0000000B            /*  DIAG0_ASSIGN: Assign SensL_B/12 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_SENSEL_C 0x0000000C            /*  DIAG0_ASSIGN: Assign SensL_C/12 to Diagnostic 0. */
#define ENUM_DIAG_ASSIGN_SENSEL_D 0x0000000D            /*  DIAG0_ASSIGN: Assign SensL_D/12 to Diagnostic 0. */
#endif
/* -------------------------------------------------------------------------------------------------------------------------
          DIN_COMP_OUT             Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DIN_COMP_OUT_DIN_COMP_OUT_D  3            /*  Debounced Digital Input State of ChannelD */
#define BITP_DIN_COMP_OUT_DIN_COMP_OUT_C  2            /*  Debounced Digital Input State of ChannelC */
#define BITP_DIN_COMP_OUT_DIN_COMP_OUT_B  1            /*  Debounced Digital Input State of ChannelB */
#define BITP_DIN_COMP_OUT_DIN_COMP_OUT_A  0            /*  Debounced Digital Input State of ChannelA */
#define BITM_DIN_COMP_OUT_DIN_COMP_OUT_D 0x00000008    /*  Debounced Digital Input State of ChannelD */
#define BITM_DIN_COMP_OUT_DIN_COMP_OUT_C 0x00000004    /*  Debounced Digital Input State of ChannelC */
#define BITM_DIN_COMP_OUT_DIN_COMP_OUT_B 0x00000002    /*  Debounced Digital Input State of ChannelB */
#define BITM_DIN_COMP_OUT_DIN_COMP_OUT_A 0x00000001    /*  Debounced Digital Input State of ChannelA */

/* -------------------------------------------------------------------------------------------------------------------------
          ADC_RESULT[n]            Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ADC_RESULT_CH_ADC_RESULT  0            /*  Contains the 16bit Result of the ADC Conversion on Channeln */
#define BITM_ADC_RESULT_CH_ADC_RESULT 0x0000FFFF    /*  Contains the 16bit Result of the ADC Conversion on Channeln */

/* -------------------------------------------------------------------------------------------------------------------------
          DIAG_RESULT[n]           Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_DIAG_RESULT_DIAG_RESULT  0            /*  Contains the 16bit Diagnostic Result on Diagnostic Channeln */
#define BITM_DIAG_RESULT_DIAG_RESULT 0x0000FFFF    /*  Contains the 16bit Diagnostic Result on Diagnostic Channeln */

/* -------------------------------------------------------------------------------------------------------------------------
          ALERT_STATUS             Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALERT_STATUS_RESET_OCCURRED 15            /*  Reset Occurred. */
#define BITP_ALERT_STATUS_CAL_MEM_ERR 14            /*  Calibration Memory Error. */
#define BITP_ALERT_STATUS_SPI_CRC_ERR 13            /*  SPI CRC Error Detected */
#define BITP_ALERT_STATUS_SPI_SCLK_CNT_ERR 12            /*  SPI SCLK Count Error Detected */
#define BITP_ALERT_STATUS_ADC_SAT_ERR 11            /*  ADC Saturation Error */
#define BITP_ALERT_STATUS_ADC_CONV_ERR 10            /*  ADC Conversion Error */
#define BITP_ALERT_STATUS_ALDO1V8_ERR  9            /*  ALDO1V8 Power Supply Monitor Error */
#define BITP_ALERT_STATUS_DVCC_ERR  8            /*  DVCC Power Supply Monitor Error */
#define BITP_ALERT_STATUS_AVDD_ERR  7            /*  AVDD Power Supply Monitor Error */
#define BITP_ALERT_STATUS_ALDO5V_ERR  6            /*  ALDO5V Power Supply Monitor Error */
#define BITP_ALERT_STATUS_CHARGE_PUMP_ERR  5            /*  Charge Pump Error Detected */
#define BITP_ALERT_STATUS_HI_TEMP_ERR  4            /*  High Temperature Detected */
#define BITP_ALERT_STATUS_VI_ERR_D  3            /*  Voltage or Current Error Detected on ChannelD. */
#define BITP_ALERT_STATUS_VI_ERR_C  2            /*  Voltage or Current Error Detected on ChannelC. */
#define BITP_ALERT_STATUS_VI_ERR_B  1            /*  Voltage or Current Error Detected on ChannelB. */
#define BITP_ALERT_STATUS_VI_ERR_A  0            /*  Voltage or Current Error Detected on ChannelA. */
#define BITM_ALERT_STATUS_RESET_OCCURRED 0x00008000    /*  Reset Occurred. */
#define BITM_ALERT_STATUS_CAL_MEM_ERR 0x00004000    /*  Calibration Memory Error. */
#define BITM_ALERT_STATUS_SPI_CRC_ERR 0x00002000    /*  SPI CRC Error Detected */
#define BITM_ALERT_STATUS_SPI_SCLK_CNT_ERR 0x00001000    /*  SPI SCLK Count Error Detected */
#define BITM_ALERT_STATUS_ADC_SAT_ERR 0x00000800    /*  ADC Saturation Error */
#define BITM_ALERT_STATUS_ADC_CONV_ERR 0x00000400    /*  ADC Conversion Error */
#define BITM_ALERT_STATUS_ALDO1V8_ERR 0x00000200    /*  ALDO1V8 Power Supply Monitor Error */
#define BITM_ALERT_STATUS_DVCC_ERR 0x00000100    /*  DVCC Power Supply Monitor Error */
#define BITM_ALERT_STATUS_AVDD_ERR 0x00000080    /*  AVDD Power Supply Monitor Error */
#define BITM_ALERT_STATUS_ALDO5V_ERR 0x00000040    /*  ALDO5V Power Supply Monitor Error */
#define BITM_ALERT_STATUS_CHARGE_PUMP_ERR 0x00000020    /*  Charge Pump Error Detected */
#define BITM_ALERT_STATUS_HI_TEMP_ERR 0x00000010    /*  High Temperature Detected */
#define BITM_ALERT_STATUS_VI_ERR_D 0x00000008    /*  Voltage or Current Error Detected on ChannelD. */
#define BITM_ALERT_STATUS_VI_ERR_C 0x00000004    /*  Voltage or Current Error Detected on ChannelC. */
#define BITM_ALERT_STATUS_VI_ERR_B 0x00000002    /*  Voltage or Current Error Detected on ChannelB. */
#define BITM_ALERT_STATUS_VI_ERR_A 0x00000001    /*  Voltage or Current Error Detected on ChannelA. */

/* -------------------------------------------------------------------------------------------------------------------------
          LIVE_STATUS              Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_LIVE_STATUS_ADC_DATA_RDY 14            /*  ADC Data Ready. */
#define BITP_LIVE_STATUS_ADC_BUSY 13            /*  ADC Busy Status Bit. */
#define BITP_LIVE_STATUS_ADC_CH_CURR 10            /*  Address of the Channel Currently Being Converted by the ADC. */
#define BITP_LIVE_STATUS_ALDO1V8_STATUS  9            /*  Current Status of ALDO1V8_ERR */
#define BITP_LIVE_STATUS_DVCC_STATUS  8            /*  Current Status of DVCC_ERR */
#define BITP_LIVE_STATUS_AVDD_STATUS  7            /*  Current Status of AVDD_ERR */
#define BITP_LIVE_STATUS_ALDO5V_STATUS  6            /*  Current Status of ALDO5V_ERR */
#define BITP_LIVE_STATUS_CHARGE_PUMP_STATUS  5            /*  Current Status of CHARGE_PUMP_ERR. */
#define BITP_LIVE_STATUS_HI_TEMP_STATUS  4            /*  Current Status of HI_TEMP_ERR */
#define BITP_LIVE_STATUS_VI_ERR_CURR_D  3            /*  Current Status of VI_ERR_D */
#define BITP_LIVE_STATUS_VI_ERR_CURR_C  2            /*  Current Status of VI_ERR_C */
#define BITP_LIVE_STATUS_VI_ERR_CURR_B  1            /*  Current Status of VI_ERR_B */
#define BITP_LIVE_STATUS_VI_ERR_CURR_A  0            /*  Current Status of VI_ERR_A */
#define BITM_LIVE_STATUS_ADC_DATA_RDY 0x00004000    /*  ADC Data Ready. */
#define BITM_LIVE_STATUS_ADC_BUSY 0x00002000    /*  ADC Busy Status Bit. */
#define BITM_LIVE_STATUS_ADC_CH_CURR 0x00001C00    /*  Address of the Channel Currently Being Converted by the ADC. */
#define BITM_LIVE_STATUS_ALDO1V8_STATUS 0x00000200    /*  Current Status of ALDO1V8_ERR */
#define BITM_LIVE_STATUS_DVCC_STATUS 0x00000100    /*  Current Status of DVCC_ERR */
#define BITM_LIVE_STATUS_AVDD_STATUS 0x00000080    /*  Current Status of AVDD_ERR */
#define BITM_LIVE_STATUS_ALDO5V_STATUS 0x00000040    /*  Current Status of ALDO5V_ERR */
#define BITM_LIVE_STATUS_CHARGE_PUMP_STATUS 0x00000020    /*  Current Status of CHARGE_PUMP_ERR. */
#define BITM_LIVE_STATUS_HI_TEMP_STATUS 0x00000010    /*  Current Status of HI_TEMP_ERR */
#define BITM_LIVE_STATUS_VI_ERR_CURR_D 0x00000008    /*  Current Status of VI_ERR_D */
#define BITM_LIVE_STATUS_VI_ERR_CURR_C 0x00000004    /*  Current Status of VI_ERR_C */
#define BITM_LIVE_STATUS_VI_ERR_CURR_B 0x00000002    /*  Current Status of VI_ERR_B */
#define BITM_LIVE_STATUS_VI_ERR_CURR_A 0x00000001    /*  Current Status of VI_ERR_A */
#define ENUM_LIVE_STATUS_CH_A    0x00000000            /*  ADC_CH_CURR: Channel A */
#define ENUM_LIVE_STATUS_CH_B    0x00000400            /*  ADC_CH_CURR: Channel B */
#define ENUM_LIVE_STATUS_CH_C    0x00000800            /*  ADC_CH_CURR: Channel C */
#define ENUM_LIVE_STATUS_CH_D    0x00000C00            /*  ADC_CH_CURR: Channel D */
#define ENUM_LIVE_STATUS_DIAG0   0x00001000            /*  ADC_CH_CURR: Diagnostics 0 */
#define ENUM_LIVE_STATUS_DIAG1   0x00001400            /*  ADC_CH_CURR: Diagnostics 1 */
#define ENUM_LIVE_STATUS_DIAG2   0x00001800            /*  ADC_CH_CURR: Diagnostics 2 */
#define ENUM_LIVE_STATUS_DIAG3   0x00001C00            /*  ADC_CH_CURR: Diagnostics 3 */

/* -------------------------------------------------------------------------------------------------------------------------
          ALERT_MASK               Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ALERT_MASK_CAL_MEM_ERR_MASK 14            /*  Mask Bit for CAL_MEM_ERR. */
#define BITP_ALERT_MASK_SPI_CRC_ERR_MASK 13            /*  Mask Bit for SPI_CRC_ERR. */
#define BITP_ALERT_MASK_SPI_SCLK_CNT_ERR_MASK 12            /*  Mask Bit for SPI_SCLK_CNT_ERR */
#define BITP_ALERT_MASK_ADC_SAT_ERR_MASK 11            /*  Mask Bit for ADC_SAT_ERR. */
#define BITP_ALERT_MASK_ADC_CONV_ERR_MASK 10            /*  Mask Bit for ADC_CONV_ERR. */
#define BITP_ALERT_MASK_ALDO1V8_ERR_MASK  9            /*  Mask Bit for ALDO1V8_ERR. */
#define BITP_ALERT_MASK_DVCC_ERR_MASK  8            /*  Mask Bit for DVCC_ERR. */
#define BITP_ALERT_MASK_AVDD_ERR_MASK  7            /*  Mask Bit for AVDD_ERR. */
#define BITP_ALERT_MASK_ALDO5V_ERR_MASK  6            /*  Mask Bit for ALDO5V_ERR. */
#define BITP_ALERT_MASK_CHARGE_PUMP_ERR_MASK  5            /*  Mask Bit for CHARGE_PUMP_ERR. */
#define BITP_ALERT_MASK_HI_TEMP_ERR_MASK  4            /*  Mask Bit for HI_TEMP_ERR. */
#define BITP_ALERT_MASK_VI_ERR_MASK_D  3            /*  Mask Bit for VI_ERR_D. */
#define BITP_ALERT_MASK_VI_ERR_MASK_C  2            /*  Mask Bit for VI_ERR_C. */
#define BITP_ALERT_MASK_VI_ERR_MASK_B  1            /*  Mask Bit for VI_ERR_B. */
#define BITP_ALERT_MASK_VI_ERR_MASK_A  0            /*  Mask Bit for VI_ERR_A. */
#define BITM_ALERT_MASK_CAL_MEM_ERR_MASK 0x00004000    /*  Mask Bit for CAL_MEM_ERR. */
#define BITM_ALERT_MASK_SPI_CRC_ERR_MASK 0x00002000    /*  Mask Bit for SPI_CRC_ERR. */
#define BITM_ALERT_MASK_SPI_SCLK_CNT_ERR_MASK 0x00001000    /*  Mask Bit for SPI_SCLK_CNT_ERR */
#define BITM_ALERT_MASK_ADC_SAT_ERR_MASK 0x00000800    /*  Mask Bit for ADC_SAT_ERR. */
#define BITM_ALERT_MASK_ADC_CONV_ERR_MASK 0x00000400    /*  Mask Bit for ADC_CONV_ERR. */
#define BITM_ALERT_MASK_ALDO1V8_ERR_MASK 0x00000200    /*  Mask Bit for ALDO1V8_ERR. */
#define BITM_ALERT_MASK_DVCC_ERR_MASK 0x00000100    /*  Mask Bit for DVCC_ERR. */
#define BITM_ALERT_MASK_AVDD_ERR_MASK 0x00000080    /*  Mask Bit for AVDD_ERR. */
#define BITM_ALERT_MASK_ALDO5V_ERR_MASK 0x00000040    /*  Mask Bit for ALDO5V_ERR. */
#define BITM_ALERT_MASK_CHARGE_PUMP_ERR_MASK 0x00000020    /*  Mask Bit for CHARGE_PUMP_ERR. */
#define BITM_ALERT_MASK_HI_TEMP_ERR_MASK 0x00000010    /*  Mask Bit for HI_TEMP_ERR. */
#define BITM_ALERT_MASK_VI_ERR_MASK_D 0x00000008    /*  Mask Bit for VI_ERR_D. */
#define BITM_ALERT_MASK_VI_ERR_MASK_C 0x00000004    /*  Mask Bit for VI_ERR_C. */
#define BITM_ALERT_MASK_VI_ERR_MASK_B 0x00000002    /*  Mask Bit for VI_ERR_B. */
#define BITM_ALERT_MASK_VI_ERR_MASK_A 0x00000001    /*  Mask Bit for VI_ERR_A. */

/* -------------------------------------------------------------------------------------------------------------------------
          READ_SELECT              Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_READ_SELECT_AUTO_RD_EN  9            /*  Automatic read enabled. */
#define BITP_READ_SELECT_SPI_RD_RET_INFO  8            /*  Determines the Content of the MSBs in the SPI Read Frame. */
#define BITP_READ_SELECT_READBACK_ADDR  0            /*  D7 to D0 contains the register address to be read. */
#define BITM_READ_SELECT_AUTO_RD_EN 0x00000200    /*  Automatic read enabled. */
#define BITM_READ_SELECT_SPI_RD_RET_INFO 0x00000100    /*  Determines the Content of the MSBs in the SPI Read Frame. */
#define BITM_READ_SELECT_READBACK_ADDR 0x000000FF    /*  D7 to D0 contains the register address to be read. */
#define ENUM_READ_SELECT_NOP     0x00000000            /*  READBACK_ADDR: NOP */
#define ENUM_READ_SELECT_CH_FUNC_SETUPA 0x00000001            /*  READBACK_ADDR: CH_FUNC_SETUPA */
#define ENUM_READ_SELECT_CH_FUNC_SETUPB 0x00000002            /*  READBACK_ADDR: CH_FUNC_SETUPB */
#define ENUM_READ_SELECT_CH_FUNC_SETUPC 0x00000003            /*  READBACK_ADDR: CH_FUNC_SETUPC */
#define ENUM_READ_SELECT_CH_FUNC_SETUPD 0x00000004            /*  READBACK_ADDR: CH_FUNC_SETUPD */
#define ENUM_READ_SELECT_ADC_CONFIGA 0x00000005            /*  READBACK_ADDR: ADC_CONFIGA */
#define ENUM_READ_SELECT_ADC_CONFIGB 0x00000006            /*  READBACK_ADDR: ADC_CONFIGB */
#define ENUM_READ_SELECT_ADC_CONFIGC 0x00000007            /*  READBACK_ADDR: ADC_CONFIGC */
#define ENUM_READ_SELECT_ADC_CONFIGD 0x00000008            /*  READBACK_ADDR: ADC_CONFIGD */
#define ENUM_READ_SELECT_DIN_CONFIGA 0x00000009            /*  READBACK_ADDR: DIN_CONFIGA */
#define ENUM_READ_SELECT_DIN_CONFIGB 0x0000000A            /*  READBACK_ADDR: DIN_CONFIGB */
#define ENUM_READ_SELECT_DIN_CONFIGC 0x0000000B            /*  READBACK_ADDR: DIN_CONFIGC */
#define ENUM_READ_SELECT_DIN_CONFIGD 0x0000000C            /*  READBACK_ADDR: DIN_CONFIGD */
#define ENUM_READ_SELECT_GPO_PARALLEL 0x0000000D            /*  READBACK_ADDR: GPO_PARALLEL */
#define ENUM_READ_SELECT_GPO_CONFIGA 0x0000000E            /*  READBACK_ADDR: GPO_CONFIGA */
#define ENUM_READ_SELECT_GPO_CONFIGB 0x0000000F            /*  READBACK_ADDR: GPO_CONFIGB */
#define ENUM_READ_SELECT_GPO_CONFIGC 0x00000010            /*  READBACK_ADDR: GPO_CONFIGC */
#define ENUM_READ_SELECT_GPO_CONFIGD 0x00000011            /*  READBACK_ADDR: GPO_CONFIGD */
#define ENUM_READ_SELECT_OUTPUT_CONFIGA 0x00000012            /*  READBACK_ADDR: OUTPUT_CONFIGA */
#define ENUM_READ_SELECT_OUTPUT_CONFIGB 0x00000013            /*  READBACK_ADDR: OUTPUT_CONFIGB */
#define ENUM_READ_SELECT_OUTPUT_CONFIGC 0x00000014            /*  READBACK_ADDR: OUTPUT_CONFIGC */
#define ENUM_READ_SELECT_OUTPUT_CONFIGD 0x00000015            /*  READBACK_ADDR: OUTPUT_CONFIGD */
#define ENUM_READ_SELECT_DAC_CODEA 0x00000016            /*  READBACK_ADDR: DAC_CODEA */
#define ENUM_READ_SELECT_DAC_CODEB 0x00000017            /*  READBACK_ADDR: DAC_CODEB */
#define ENUM_READ_SELECT_DAC_CODEC 0x00000018            /*  READBACK_ADDR: DAC_CODEC */
#define ENUM_READ_SELECT_DAC_CODED 0x00000019            /*  READBACK_ADDR: DAC_CODED */
#define ENUM_READ_SELECT_DAC_CLR_CODEA 0x0000001A            /*  READBACK_ADDR: DAC_CLR_CODEA */
#define ENUM_READ_SELECT_DAC_CLR_CODEB 0x0000001B            /*  READBACK_ADDR: DAC_CLR_CODEB */
#define ENUM_READ_SELECT_DAC_CLR_CODEC 0x0000001C            /*  READBACK_ADDR: DAC_CLR_CODEC */
#define ENUM_READ_SELECT_DAC_CLR_CODED 0x0000001D            /*  READBACK_ADDR: DAC_CLR_CODED */
#define ENUM_READ_SELECT_DAC_ACTIVEA 0x0000001E            /*  READBACK_ADDR: DAC_ACTIVEA */
#define ENUM_READ_SELECT_DAC_ACTIVEB 0x0000001F            /*  READBACK_ADDR: DAC_ACTIVEB */
#define ENUM_READ_SELECT_DAC_ACTIVEC 0x00000020            /*  READBACK_ADDR: DAC_ACTIVEC */
#define ENUM_READ_SELECT_DAC_ACTIVED 0x00000021            /*  READBACK_ADDR: DAC_ACTIVED */
#define ENUM_READ_SELECT_DIN_THRESH 0x00000022            /*  READBACK_ADDR: DIN_THRESH */
#define ENUM_READ_SELECT_ADC_CONV_CTRL 0x00000023            /*  READBACK_ADDR: ADC_CONV_CTRL */
#define ENUM_READ_SELECT_DIAG_ASSIGN 0x00000024            /*  READBACK_ADDR: DIAG_ASSIGN */
#define ENUM_READ_SELECT_DIN_COMP_OUT 0x00000025            /*  READBACK_ADDR: DIN_COMP_OUT */
#define ENUM_READ_SELECT_ADC_RESULTA 0x00000026            /*  READBACK_ADDR: ADC_RESULTA */
#define ENUM_READ_SELECT_ADC_RESULTB 0x00000027            /*  READBACK_ADDR: ADC_RESULTB */
#define ENUM_READ_SELECT_ADC_RESULTC 0x00000028            /*  READBACK_ADDR: ADC_RESULTC */
#define ENUM_READ_SELECT_ADC_RESULTD 0x00000029            /*  READBACK_ADDR: ADC_RESULTD */
#define ENUM_READ_SELECT_DIAG_RESULTA 0x0000002A            /*  READBACK_ADDR: DIAG_RESULTA */
#define ENUM_READ_SELECT_DIAG_RESULTB 0x0000002B            /*  READBACK_ADDR: DIAG_RESULTB */
#define ENUM_READ_SELECT_DIAG_RESULTC 0x0000002C            /*  READBACK_ADDR: DIAG_RESULTC */
#define ENUM_READ_SELECT_DIAG_RESULTD 0x0000002D            /*  READBACK_ADDR: DIAG_RESULTD */
#define ENUM_READ_SELECT_ALERT_STATUS 0x0000002E            /*  READBACK_ADDR: ALERT_STATUS */
#define ENUM_READ_SELECT_LIVE_STATUS 0x0000002F            /*  READBACK_ADDR: LIVE_STATUS */
#define ENUM_READ_SELECT_ALERT_MASK 0x0000003C            /*  READBACK_ADDR: ALERT_MASK */
#define ENUM_READ_SELECT_READ_SELECT 0x00000041            /*  READBACK_ADDR: READ_SELECT */
#define ENUM_READ_SELECT_ADC_CONV_CTRL_40SPS 0x00000042            /*  READBACK_ADDR: ADC_CONV_CTRL_40SPS */
#define ENUM_READ_SELECT_THERM_RST 0x00000043            /*  READBACK_ADDR: THERM_RST */
#define ENUM_READ_SELECT_CMD_KEY 0x00000044            /*  READBACK_ADDR: CMD_KEY */
#define ENUM_READ_SELECT_SCRATCH 0x00000045            /*  READBACK_ADDR: SCRATCH */
#define ENUM_READ_SELECT_SILICON_REV 0x00000046            /*  READBACK_ADDR: SILICON_REV */

/* -------------------------------------------------------------------------------------------------------------------------
          ADC_CONV_CTRL_40SPS      Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_ADC_CONV_CTRL_40SPS_CONV_SEQ_40SPS  0            /*  Selects Single or Continuous Mode. */
#define BITM_ADC_CONV_CTRL_40SPS_CONV_SEQ_40SPS 0x00000003    /*  Selects Single or Continuous Mode. */
#define ENUM_ADC_CONV_CTRL_40SPS_SPS40_IDLE 0x00000000            /*  CONV_SEQ_40SPS: Stop continuous conversions and leave the ADC powered up or power up the ADC. */
#define ENUM_ADC_CONV_CTRL_40SPS_SPS40_SINGLE 0x00000001            /*  CONV_SEQ_40SPS: Start single sequence conversion. */
#define ENUM_ADC_CONV_CTRL_40SPS_SPS40_CONTINUOUS 0x00000002            /*  CONV_SEQ_40SPS: Start Continuous Conversions. */
#define ENUM_ADC_CONV_CTRL_40SPS_SPS40_ADC_PWRDWN 0x00000003            /*  CONV_SEQ_40SPS: Stop continuous conversions and power down the ADC  or power down the ADC. */

/* -------------------------------------------------------------------------------------------------------------------------
          THERM_RST                Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_THERM_RST_EN_THERM_RST  0            /*  Set to 1 to Enable Thermal Reset Functionality. */
#define BITM_THERM_RST_EN_THERM_RST 0x00000001    /*  Set to 1 to Enable Thermal Reset Functionality. */

/* -------------------------------------------------------------------------------------------------------------------------
          CMD_KEY                  Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_CMD_KEY_CMD_KEY      0            /*  Enter a Key to Execute a Command. */
#define BITM_CMD_KEY_CMD_KEY     0x0000FFFF    /*  Enter a Key to Execute a Command. */
#define ENUM_CMD_KEY_NOP         0x00000000            /*  CMD_KEY: No Operation */
#define ENUM_CMD_KEY_SW_RST_KEY1 0x000015FA            /*  CMD_KEY: Software Reset Key1. */
#define ENUM_CMD_KEY_DAC_CLR_KEY 0x000073D1            /*  CMD_KEY: DAC Clear Key */
#define ENUM_CMD_KEY_LDAC_KEY    0x0000953A            /*  CMD_KEY: DAC Update Key */
#define ENUM_CMD_KEY_SW_RST_KEY2 0x0000AF51            /*  CMD_KEY: Software Reset Key2. */

/* -------------------------------------------------------------------------------------------------------------------------
          SCRATCH                  Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SCRATCH_SCRATCH_BITS  0            /*  Scratch or spare register field. */
#define BITM_SCRATCH_SCRATCH_BITS 0x0000FFFF    /*  Scratch or spare register field. */

/* -------------------------------------------------------------------------------------------------------------------------
          SILICON_REV              Pos/Masks         Description
   ------------------------------------------------------------------------------------------------------------------------- */
#define BITP_SILICON_REV_SILICON_REV_ID  0            /*  Silicon Revision Identification. */
#define BITM_SILICON_REV_SILICON_REV_ID 0x000000FF    /*  Silicon Revision Identification. */


#endif	/* end ifndef _DEF_AD74412_REGS_H */
