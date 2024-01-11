/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2023 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

/**
 * \file adrv904x_cpu_scratch_registers.h
 *
 * \brief   Contains CPU scratch register definitions
 *
 * \details Contains CPU scratch register definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_SCRATCH_REGISTERS_H__
#define __ADRV904X_CPU_SCRATCH_REGISTERS_H__

/* Scratch register 0 contains CPU0 boot status */
#define ADRV904X_CPU_CPU0_BOOT_STATUS_SCRATCH_REG_ID    0

/* Scratch register 1 contains CPU1 boot status */
#define ADRV904X_CPU_CPU1_BOOT_STATUS_SCRATCH_REG_ID    1

/* Scratch register 6 contains Stream/API Tx to ORx Mapping byte */
#define ADRV904X_CPU_TX_ORX_MAPPING_SET_SCRATCH_REG_ID      6

/* Scratch register 7 contains config needed by ORX High Stream for JESD Link Sharing */
#define ADRV904X_CPU_ORX_LINK_SHARING_SCRATCH_REG_ID        7

/* Scratch register 8 contains config needed by RX0-3 High Streams for JESD Link Sharing */
#define ADRV904X_CPU_TX_RX03_MAPPING_SET_SCRATCH_REG_ID     8

/* Scratch register 9 contains config needed by RX4-7 High Streams for JESD Link Sharing */
#define ADRV904X_CPU_TX_RX47_MAPPING_SET_SCRATCH_REG_ID     9

/* Software Breakpoint scratchpad register assignments */
#define ADRV904X_CPU_CPU0_SWBKPT_INDEX_SCRATCH_REG_ID       10
#define ADRV904X_CPU_CPU0_SWBKPT_BKPT_NUM_SCRATCH_REG_ID    11
#define ADRV904X_CPU_CPU0_SWBKPT_CHAN_NUM_SCRATCH_REG_ID    12
#define ADRV904X_CPU_CPU1_SWBKPT_INDEX_SCRATCH_REG_ID       13
#define ADRV904X_CPU_CPU1_SWBKPT_BKPT_NUM_SCRATCH_REG_ID    14
#define ADRV904X_CPU_CPU1_SWBKPT_CHAN_NUM_SCRATCH_REG_ID    15

/* CPU Primary/Secondary boot sequencing registers */
#define ADRV904X_CPU_CPU0_IS_PRIMARY                        16
#define ADRV904X_CPU_CPU1_IS_PRIMARY                        17
#define ADRV904X_CPU_SECONDARY_BOOT_RELEASE    18

/* Scratch registers 19-35 contain Stream/API Tx to ORx Mapping look up table */
/* NOTE: In DFE (RS) mode this table is used to only store GPIO pin values for ORX0 */
#define ADRV904X_CPU_TX_ORX_MAPPING_00    19
#define ADRV904X_CPU_TX_ORX_MAPPING_01    20
#define ADRV904X_CPU_TX_ORX_MAPPING_02    21
#define ADRV904X_CPU_TX_ORX_MAPPING_03    22
#define ADRV904X_CPU_TX_ORX_MAPPING_04    23
#define ADRV904X_CPU_TX_ORX_MAPPING_05    24
#define ADRV904X_CPU_TX_ORX_MAPPING_06    25
#define ADRV904X_CPU_TX_ORX_MAPPING_07    26
#define ADRV904X_CPU_TX_ORX_MAPPING_08    27
#define ADRV904X_CPU_TX_ORX_MAPPING_09    28
#define ADRV904X_CPU_TX_ORX_MAPPING_10    29
#define ADRV904X_CPU_TX_ORX_MAPPING_11    30
#define ADRV904X_CPU_TX_ORX_MAPPING_12    31
#define ADRV904X_CPU_TX_ORX_MAPPING_13    32
#define ADRV904X_CPU_TX_ORX_MAPPING_14    33
#define ADRV904X_CPU_TX_ORX_MAPPING_15    34

/* Scratch registers to hold temporary Tx to ORx NCO and atten values */
#define ADRV904X_CPU_TEMP0                35               /* Tx channel (0 - 7) */
#define ADRV904X_CPU_TEMP1                36               /* LSB of ORx ADC NCO or LOL NCO frequency or ORx atten */
#define ADRV904X_CPU_TEMP2                37               /* ISB of ORx ADC NCO or LOL NCO frequency or ORx atten */
#define ADRV904X_CPU_TEMP3                38               /* MSB of ORx ADC NCO or LOL NCO frequency */
#define ADRV904X_CPU_TEMP4                39               /* LSB of ORx DP NCO or LOL NCO frequency */
#define ADRV904X_CPU_TEMP5                40               /* ISB of ORx DP NCO or LOL NCO frequency */
#define ADRV904X_CPU_TEMP6                41               /* MSB of ORx DP NCO or LOL NCO frequency */

/*
 * Table to hold ADC and DP NCO values for all supported mappings
 */
/* Scratch registers to hold ADC/DP NCO values for mapping=0x00 (Tx0-to-ORx) */
#define ADRV904X_CPU_TX_0_ORX_NCO_ADC_FREQ_LSB    42
#define ADRV904X_CPU_TX_0_ORX_NCO_ADC_FREQ_ISB    43
#define ADRV904X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB    44
#define ADRV904X_CPU_TX_0_ORX_NCO_DP_FREQ_LSB     45
#define ADRV904X_CPU_TX_0_ORX_NCO_DP_FREQ_ISB     46
#define ADRV904X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB     47
/* Scratch registers to hold ADC/DP NCO values for mapping=0x01 (Tx1-to-ORx) */
#define ADRV904X_CPU_TX_1_ORX_NCO_ADC_FREQ_LSB    48
#define ADRV904X_CPU_TX_1_ORX_NCO_ADC_FREQ_ISB    49
#define ADRV904X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB    50
#define ADRV904X_CPU_TX_1_ORX_NCO_DP_FREQ_LSB     51
#define ADRV904X_CPU_TX_1_ORX_NCO_DP_FREQ_ISB     52
#define ADRV904X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB     53
/* Scratch registers to hold ADC/DP NCO values for mapping=0x02 (Tx2-to-ORx) */
#define ADRV904X_CPU_TX_2_ORX_NCO_ADC_FREQ_LSB    54
#define ADRV904X_CPU_TX_2_ORX_NCO_ADC_FREQ_ISB    55
#define ADRV904X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB    56
#define ADRV904X_CPU_TX_2_ORX_NCO_DP_FREQ_LSB     57
#define ADRV904X_CPU_TX_2_ORX_NCO_DP_FREQ_ISB     58
#define ADRV904X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB     59
/* Scratch registers to hold ADC/DP NCO values for mapping=0x03 (Tx3-to-ORx) */
#define ADRV904X_CPU_TX_3_ORX_NCO_ADC_FREQ_LSB    60
#define ADRV904X_CPU_TX_3_ORX_NCO_ADC_FREQ_ISB    61
#define ADRV904X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB    62
#define ADRV904X_CPU_TX_3_ORX_NCO_DP_FREQ_LSB     63
#define ADRV904X_CPU_TX_3_ORX_NCO_DP_FREQ_ISB     64
#define ADRV904X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB     65
/* Scratch registers to hold ADC/DP NCO values for mapping=0x04 (Tx4-to-ORx) */
#define ADRV904X_CPU_TX_4_ORX_NCO_ADC_FREQ_LSB    66
#define ADRV904X_CPU_TX_4_ORX_NCO_ADC_FREQ_ISB    67
#define ADRV904X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB    68
#define ADRV904X_CPU_TX_4_ORX_NCO_DP_FREQ_LSB     69
#define ADRV904X_CPU_TX_4_ORX_NCO_DP_FREQ_ISB     70
#define ADRV904X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB     71
/* Scratch registers to hold ADC/DP NCO values for mapping=0x05 (Tx5-to-ORx) */
#define ADRV904X_CPU_TX_5_ORX_NCO_ADC_FREQ_LSB    72
#define ADRV904X_CPU_TX_5_ORX_NCO_ADC_FREQ_ISB    73
#define ADRV904X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB    74
#define ADRV904X_CPU_TX_5_ORX_NCO_DP_FREQ_LSB     75
#define ADRV904X_CPU_TX_5_ORX_NCO_DP_FREQ_ISB     76
#define ADRV904X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB     77
/* Scratch registers to hold ADC/DP NCO values for mapping=0x06 (Tx6-to-ORx) */
#define ADRV904X_CPU_TX_6_ORX_NCO_ADC_FREQ_LSB    78
#define ADRV904X_CPU_TX_6_ORX_NCO_ADC_FREQ_ISB    79
#define ADRV904X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB    80
#define ADRV904X_CPU_TX_6_ORX_NCO_DP_FREQ_LSB     81
#define ADRV904X_CPU_TX_6_ORX_NCO_DP_FREQ_ISB     82
#define ADRV904X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB     83
/* Scratch registers to hold ADC/DP NCO values for mapping=0x07 (Tx7-to-ORx) */
#define ADRV904X_CPU_TX_7_ORX_NCO_ADC_FREQ_LSB    84
#define ADRV904X_CPU_TX_7_ORX_NCO_ADC_FREQ_ISB    85
#define ADRV904X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB    86
#define ADRV904X_CPU_TX_7_ORX_NCO_DP_FREQ_LSB     87
#define ADRV904X_CPU_TX_7_ORX_NCO_DP_FREQ_ISB     88
#define ADRV904X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB     89
/* Scratch registers to hold ADC/DP NCO values for mapping=0x08 */
#define ADRV904X_CPU_MAPVAL_8_NCO_ADC_FREQ_LSB    90
#define ADRV904X_CPU_MAPVAL_8_NCO_ADC_FREQ_ISB    91
#define ADRV904X_CPU_MAPVAL_8_NCO_ADC_FREQ_MSB    92
#define ADRV904X_CPU_MAPVAL_8_NCO_DP_FREQ_LSB     93
#define ADRV904X_CPU_MAPVAL_8_NCO_DP_FREQ_ISB     94
#define ADRV904X_CPU_MAPVAL_8_NCO_DP_FREQ_MSB     95
/* Scratch registers to hold ADC/DP NCO values for mapping=0x09 */
#define ADRV904X_CPU_MAPVAL_9_NCO_ADC_FREQ_LSB    96
#define ADRV904X_CPU_MAPVAL_9_NCO_ADC_FREQ_ISB    97
#define ADRV904X_CPU_MAPVAL_9_NCO_ADC_FREQ_MSB    98
#define ADRV904X_CPU_MAPVAL_9_NCO_DP_FREQ_LSB     99
#define ADRV904X_CPU_MAPVAL_9_NCO_DP_FREQ_ISB     100
#define ADRV904X_CPU_MAPVAL_9_NCO_DP_FREQ_MSB     101
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0A */
#define ADRV904X_CPU_MAPVAL_A_NCO_ADC_FREQ_LSB    102
#define ADRV904X_CPU_MAPVAL_A_NCO_ADC_FREQ_ISB    103
#define ADRV904X_CPU_MAPVAL_A_NCO_ADC_FREQ_MSB    104
#define ADRV904X_CPU_MAPVAL_A_NCO_DP_FREQ_LSB     105
#define ADRV904X_CPU_MAPVAL_A_NCO_DP_FREQ_ISB     106
#define ADRV904X_CPU_MAPVAL_A_NCO_DP_FREQ_MSB     107
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0B */
#define ADRV904X_CPU_MAPVAL_B_NCO_ADC_FREQ_LSB    108
#define ADRV904X_CPU_MAPVAL_B_NCO_ADC_FREQ_ISB    109
#define ADRV904X_CPU_MAPVAL_B_NCO_ADC_FREQ_MSB    110
#define ADRV904X_CPU_MAPVAL_B_NCO_DP_FREQ_LSB     111
#define ADRV904X_CPU_MAPVAL_B_NCO_DP_FREQ_ISB     112
#define ADRV904X_CPU_MAPVAL_B_NCO_DP_FREQ_MSB     113
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0C */
#define ADRV904X_CPU_MAPVAL_C_NCO_ADC_FREQ_LSB    114
#define ADRV904X_CPU_MAPVAL_C_NCO_ADC_FREQ_ISB    115
#define ADRV904X_CPU_MAPVAL_C_NCO_ADC_FREQ_MSB    116
#define ADRV904X_CPU_MAPVAL_C_NCO_DP_FREQ_LSB     117
#define ADRV904X_CPU_MAPVAL_C_NCO_DP_FREQ_ISB     118
#define ADRV904X_CPU_MAPVAL_C_NCO_DP_FREQ_MSB     119
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0D */
#define ADRV904X_CPU_MAPVAL_D_NCO_ADC_FREQ_LSB    120
#define ADRV904X_CPU_MAPVAL_D_NCO_ADC_FREQ_ISB    121
#define ADRV904X_CPU_MAPVAL_D_NCO_ADC_FREQ_MSB    122
#define ADRV904X_CPU_MAPVAL_D_NCO_DP_FREQ_LSB     123
#define ADRV904X_CPU_MAPVAL_D_NCO_DP_FREQ_ISB     124
#define ADRV904X_CPU_MAPVAL_D_NCO_DP_FREQ_MSB     125
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0E */
#define ADRV904X_CPU_MAPVAL_E_NCO_ADC_FREQ_LSB    126
#define ADRV904X_CPU_MAPVAL_E_NCO_ADC_FREQ_ISB    127
#define ADRV904X_CPU_MAPVAL_E_NCO_ADC_FREQ_MSB    128
#define ADRV904X_CPU_MAPVAL_E_NCO_DP_FREQ_LSB     129
#define ADRV904X_CPU_MAPVAL_E_NCO_DP_FREQ_ISB     130
#define ADRV904X_CPU_MAPVAL_E_NCO_DP_FREQ_MSB     131
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0F */
#define ADRV904X_CPU_MAPVAL_F_NCO_ADC_FREQ_LSB    132
#define ADRV904X_CPU_MAPVAL_F_NCO_ADC_FREQ_ISB    133
#define ADRV904X_CPU_MAPVAL_F_NCO_ADC_FREQ_MSB    134
#define ADRV904X_CPU_MAPVAL_F_NCO_DP_FREQ_LSB     135
#define ADRV904X_CPU_MAPVAL_F_NCO_DP_FREQ_ISB     136
#define ADRV904X_CPU_MAPVAL_F_NCO_DP_FREQ_MSB     137

/*
 * Table to hold attenuation values for all supported mappings
 */
/* Scratch registers to hold attenuation value for mapping=0x00 (Tx0-to-ORx) */
#define ADRV904X_CPU_TX_0_ORX_ATTEN         138
#define ADRV904X_CPU_TX_0_ORX_ATTEN_CTRL    139
/* Scratch registers to hold attenuation value for mapping=0x01 (Tx1-to-ORx) */
#define ADRV904X_CPU_TX_1_ORX_ATTEN         140
#define ADRV904X_CPU_TX_1_ORX_ATTEN_CTRL    141
/* Scratch registers to hold attenuation value for mapping=0x02 (Tx2-to-ORx) */
#define ADRV904X_CPU_TX_2_ORX_ATTEN         142
#define ADRV904X_CPU_TX_2_ORX_ATTEN_CTRL    143
/* Scratch registers to hold attenuation value for mapping=0x03 (Tx3-to-ORx) */
#define ADRV904X_CPU_TX_3_ORX_ATTEN         144
#define ADRV904X_CPU_TX_3_ORX_ATTEN_CTRL    145
/* Scratch registers to hold attenuation value for mapping=0x04 (Tx4-to-ORx) */
#define ADRV904X_CPU_TX_4_ORX_ATTEN         146
#define ADRV904X_CPU_TX_4_ORX_ATTEN_CTRL    147
/* Scratch registers to hold attenuation value for mapping=0x05 (Tx5-to-ORx) */
#define ADRV904X_CPU_TX_5_ORX_ATTEN         148
#define ADRV904X_CPU_TX_5_ORX_ATTEN_CTRL    149
/* Scratch registers to hold attenuation value for mapping=0x06 (Tx6-to-ORx) */
#define ADRV904X_CPU_TX_6_ORX_ATTEN         150
#define ADRV904X_CPU_TX_6_ORX_ATTEN_CTRL    151
/* Scratch registers to hold attenuation value for mapping=0x07 (Tx7-to-ORx) */
#define ADRV904X_CPU_TX_7_ORX_ATTEN         152
#define ADRV904X_CPU_TX_7_ORX_ATTEN_CTRL    153
/* Scratch registers to hold attenuation value for mapping=0x08 */
#define ADRV904X_CPU_MAPVAL_8_ATTEN         154
#define ADRV904X_CPU_MAPVAL_8_ATTEN_CTRL    155
/* Scratch registers to hold attenuation value for mapping=0x09 */
#define ADRV904X_CPU_MAPVAL_9_ATTEN         156
#define ADRV904X_CPU_MAPVAL_9_ATTEN_CTRL    157
/* Scratch registers to hold attenuation value for mapping=0x0A */
#define ADRV904X_CPU_MAPVAL_A_ATTEN         158
#define ADRV904X_CPU_MAPVAL_A_ATTEN_CTRL    159
/* Scratch registers to hold attenuation value for mapping=0x0B */
#define ADRV904X_CPU_MAPVAL_B_ATTEN         160
#define ADRV904X_CPU_MAPVAL_B_ATTEN_CTRL    161
/* Scratch registers to hold attenuation value for mapping=0x0C */
#define ADRV904X_CPU_MAPVAL_C_ATTEN         162
#define ADRV904X_CPU_MAPVAL_C_ATTEN_CTRL    163
/* Scratch registers to hold attenuation value for mapping=0x0D */
#define ADRV904X_CPU_MAPVAL_D_ATTEN         164
#define ADRV904X_CPU_MAPVAL_D_ATTEN_CTRL    165
/* Scratch registers to hold attenuation value for mapping=0x0E */
#define ADRV904X_CPU_MAPVAL_E_ATTEN         166
#define ADRV904X_CPU_MAPVAL_E_ATTEN_CTRL    167
/* Scratch registers to hold attenuation value for mapping=0x0F */
#define ADRV904X_CPU_MAPVAL_F_ATTEN         168
#define ADRV904X_CPU_MAPVAL_F_ATTEN_CTRL    169

/*
 * Table to hold LOL NCO values for all supported mappings
 */
/* Scratch registers to hold LOL NCO values for mapping=0x00 (Tx0-to-ORx) */
#define ADRV904X_CPU_TX_0_ORX_NCO_LOL_FREQ_LSB    170
#define ADRV904X_CPU_TX_0_ORX_NCO_LOL_FREQ_ISB    171
#define ADRV904X_CPU_TX_0_ORX_NCO_LOL_FREQ_MSB    172
/* Scratch registers to hold LOL NCO values for mapping=0x01 (Tx1-to-ORx) */
#define ADRV904X_CPU_TX_1_ORX_NCO_LOL_FREQ_LSB    173
#define ADRV904X_CPU_TX_1_ORX_NCO_LOL_FREQ_ISB    174
#define ADRV904X_CPU_TX_1_ORX_NCO_LOL_FREQ_MSB    175
/* Scratch registers to hold LOL NCO values for mapping=0x02 (Tx2-to-ORx) */
#define ADRV904X_CPU_TX_2_ORX_NCO_LOL_FREQ_LSB    176
#define ADRV904X_CPU_TX_2_ORX_NCO_LOL_FREQ_ISB    177
#define ADRV904X_CPU_TX_2_ORX_NCO_LOL_FREQ_MSB    178
/* Scratch registers to hold LOL NCO values for mapping=0x03 (Tx3-to-ORx) */
#define ADRV904X_CPU_TX_3_ORX_NCO_LOL_FREQ_LSB    179
#define ADRV904X_CPU_TX_3_ORX_NCO_LOL_FREQ_ISB    180
#define ADRV904X_CPU_TX_3_ORX_NCO_LOL_FREQ_MSB    181
/* Scratch registers to hold LOL NCO values for mapping=0x04 (Tx4-to-ORx) */
#define ADRV904X_CPU_TX_4_ORX_NCO_LOL_FREQ_LSB    182
#define ADRV904X_CPU_TX_4_ORX_NCO_LOL_FREQ_ISB    183
#define ADRV904X_CPU_TX_4_ORX_NCO_LOL_FREQ_MSB    184
/* Scratch registers to hold LOL NCO values for mapping=0x05 (Tx5-to-ORx) */
#define ADRV904X_CPU_TX_5_ORX_NCO_LOL_FREQ_LSB    185
#define ADRV904X_CPU_TX_5_ORX_NCO_LOL_FREQ_ISB    186
#define ADRV904X_CPU_TX_5_ORX_NCO_LOL_FREQ_MSB    187
/* Scratch registers to hold LOL NCO values for mapping=0x06 (Tx6-to-ORx) */
#define ADRV904X_CPU_TX_6_ORX_NCO_LOL_FREQ_LSB    188
#define ADRV904X_CPU_TX_6_ORX_NCO_LOL_FREQ_ISB    189
#define ADRV904X_CPU_TX_6_ORX_NCO_LOL_FREQ_MSB    190
/* Scratch registers to hold LOL NCO values for mapping=0x07 (Tx7-to-ORx) */
#define ADRV904X_CPU_TX_7_ORX_NCO_LOL_FREQ_LSB    191
#define ADRV904X_CPU_TX_7_ORX_NCO_LOL_FREQ_ISB    192
#define ADRV904X_CPU_TX_7_ORX_NCO_LOL_FREQ_MSB    193

/* Scratch register used to store ORX_TX_AUTO_SWITCH_MODE information */
#define ADRV904X_CPU_ORX_TX_AUTO_SWITCH_MODE      194


/* Scratch register to enable/disable the TxLB ADC DAC - channelMask (see TPGSWE-2397) */
#define ADRV904X_CPU_TXLB_DAC_ENABLE                  195

/* Software Breakpoint GPIO scratchpad register assignments */
#define ADRV904X_CPU_SWBKPT_WAKEUP_GPIO_PIN_REG_ID    196

/* Scratch register to enable/disable the Tx Anttena Cal - channelMask */
#define ADRV904X_GPIO_ANTCAL_TX_MASK                  198

/* Scratch register to enable/disable the Rx Anttena Cal - channelMask */
#define ADRV904X_GPIO_ANTCAL_RX_MASK                  199

/* Scratch register containing slice stream processor stream number to trigger */
#define ADRV904X_TRIGGER_SLICE_STREAM_NUM             204

/* Scratch register containing channel mask of which slice stream processors to trigger stream in */
#define ADRV904X_TRIGGER_SLICE_STREAM_MASK            205

/* Scratch register that holds TRx channels for gpio 0 */
#define ADRV904X_TRIGGER_SLICE_GPIO_0_TRX_CHANNELS    212

/* Scratch register that holds TRx channels for gpio 1 */
#define ADRV904X_TRIGGER_SLICE_GPIO_1_TRX_CHANNELS    213

/* Scratch register that holds TRx channels for gpio 2 */
#define ADRV904X_TRIGGER_SLICE_GPIO_2_TRX_CHANNELS    214

/* Scratch register that holds TRx channels for gpio 3 */
#define ADRV904X_TRIGGER_SLICE_GPIO_3_TRX_CHANNELS    215

/* Scratch register that holds if channels affect are from Rx or Tx */
#define ADRV904X_TRIGGER_SLICE_TR_OR_RX_CHANNELS      216

/* Scratch register that holds the last state of TX0. TX0 high = 0x04, TX0 low = 0x00 */
#define ADRV904X_TX0_STATE                            217

/* Scratch register that holds the Current Value of 'Symbol Ignore Count' for BUF0 */
#define ADRV904X_SBET_CUR_SYMBOL_IGNORE_COUNT_BUF0    218

/* Scratch register that holds the Requested Value of 'Symbol Ignore Count' for BUF0 */
#define ADRV904X_SBET_REQ_SYMBOL_IGNORE_COUNT_BUF0    219

/* Scratch register that holds the Current Value of 'Symbol Ignore Count' for BUF1 */
#define ADRV904X_SBET_CUR_SYMBOL_IGNORE_COUNT_BUF1    220

/* Scratch register that holds the Requested Value of 'Symbol Ignore Count' for BUF1 */
#define ADRV904X_SBET_REQ_SYMBOL_IGNORE_COUNT_BUF1    221

/* Note: The registers 225-233 are currently used as SBET debug counters. They can be re-purposed if needed.
 *       All the code in core SP starting with tag $ENABLE_SBET_DBG$ needs to be removed
 */
/* Scratch register that holds counter registering how many times SBET_CHECK_DPD_MODEL_IDX_MATCHES_TARGET was called */
#define ADRV904X_DBG_SBET_CHECK_DPD_MODEL_IDX_MATCHES_TARGET_INVOKE_CNT    225

/* Scratch register that holds counter registering how many times SBET_CAP_CHECK_AND_UPDATE_IGNORE_COUNT was called */
#define ADRV904X_DBG_SBET_CAP_CHECK_AND_UPDATE_IGNORE_COUNT_INVOKE_CNT     226

/* Scratch register that holds counter registering how many times SBET_CAPTURE_BUF0_SW_TRIGGER_TOGGLE was called */
#define ADRV904X_DBG_SBET_CAPTURE_BUF0_SW_TRIGGER_TOGGLE_INVOKE_CNT        227

/* Scratch register that holds counter registering how many times SBET_CAPTURE_BUF1_SW_TRIGGER_TOGGLE was called */
#define ADRV904X_DBG_SBET_CAPTURE_BUF1_SW_TRIGGER_TOGGLE_INVOKE_CNT        228

/* Scratch register that holds counter registering how many times SBET_CLEAR_CAP_REQUEST_FOR_BUF0 was called */
#define ADRV904X_DBG_SBET_CLEAR_CAP_REQUEST_FOR_BUF0_INVOKE_CNT            230

/* Scratch register that holds counter registering how many times SBET_CLEAR_CAP_REQUEST_FOR_BUF1 was called */
#define ADRV904X_DBG_SBET_CLEAR_CAP_REQUEST_FOR_BUF1_INVOKE_CNT            231

/* Scratch register that holds counter registering how many times CAPTURE_BUF0_SW_TRIGGER_TOGGLE was called */
#define ADRV904X_DBG_CAPTURE_BUF0_SW_TRIGGER_TOGGLE_INVOKE_CNT             232

/* Scratch register that holds counter registering how many times CAPTURE_BUF1_SW_TRIGGER_TOGGLE was called */
#define ADRV904X_DBG_CAPTURE_BUF1_SW_TRIGGER_TOGGLE_INVOKE_CNT             233


/* Scratch register 256 contains DFE CPU0 boot status */
#define ADRV904X_DFE_BOOT_STATUS_SCRATCH_REG_ID        256

/* Scratch register to contains DFE SDK global variable address */
#define ADRV904X_DFE_SDK_DATA_ADDR_0_SCRATCH_REG_ID    257               /* address[7:0] */
#define ADRV904X_DFE_SDK_DATA_ADDR_1_SCRATCH_REG_ID    258               /* address[15:8] */
#define ADRV904X_DFE_SDK_DATA_ADDR_2_SCRATCH_REG_ID    259               /* address[23:16] */
#define ADRV904X_DFE_SDK_DATA_ADDR_3_SCRATCH_REG_ID    260               /* address[31:24] */

/* Scratch register containing DFE DMA request error flag. This register is used by Koror core
 * stream to indicate an error when a new DMA transaction is requested but the previous one
 * is not completed
 */
#define ADRV904X_DFE_DMA_REQUEST_ERROR                 261

/* Scratch register containing the VSWR direction FWD/REV */
#define ADRV904X_DFE_VSWR_DIRECTION_SCRATCH_REG_ID     262

/*
 * Scratch registers used to store the value of  gpio_from_master_set/gpio_from_master_clear
 * which are set by core stream when configuring the Tx-to-Orx0 mapping
 */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_SET_0_7         263               /* for ORx0, gpio_from_master_set[7:0] */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_SET_15_8        264               /* for ORx0, gpio_from_master_set[15:8] */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_SET_23_16       265               /* for ORx0, gpio_from_master_set[23:16] */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_CLEAR_0_7       266               /* for ORx0, gpio_from_master_clear[7:0] */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_CLEAR_15_8      267               /* for ORx0, gpio_from_master_clear[15:8] */
#define ADRV904X_ORX0_GPIO_FROM_MASTER_CLEAR_23_16     268               /* for ORx0, gpio_from_master_clear[23:16] */

/*
 * Scratch registers used to store the value of  gpio_from_master_set/gpio_from_master_clear
 * which are set by core stream when configuring the Tx-to-Orx1 mapping
 */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_SET_0_7         269               /* for ORx1, gpio_from_master_set[7:0] */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_SET_15_8        270               /* for ORx1, gpio_from_master_set[15:8] */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_SET_23_16       271               /* for ORx1, gpio_from_master_set[23:16] */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_CLEAR_0_7       272               /* for ORx1, gpio_from_master_clear[7:0] */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_CLEAR_15_8      273               /* for ORx1, gpio_from_master_clear[15:8] */
#define ADRV904X_ORX1_GPIO_FROM_MASTER_CLEAR_23_16     274               /* for ORx1, gpio_from_master_clear[23:16] */

/* Scratch registers to contain the ORx to Tx mapping channel number to channel mask look-up table */
#define ADRV904X_CHAN_NUM_TO_MASK_0                    275               /* 0x1 */
#define ADRV904X_CHAN_NUM_TO_MASK_1                    276               /* 0x2 */
#define ADRV904X_CHAN_NUM_TO_MASK_2                    277               /* 0x4 */
#define ADRV904X_CHAN_NUM_TO_MASK_3                    278               /* 0x8 */

/* Scratch registers containing flags indicating first TX to ORX mapping transition for ORX0 and 1 */
#define ADRV904X_FIRST_ORX0_MAP_TRANS_FLAG             279
#define ADRV904X_FIRST_ORX1_MAP_TRANS_FLAG             280

/* @todo: Investigation needed why writing to scratch registers 281-288 is causing Tx QEC calibration to fail
 *        and the written value is modified by 0x0E mask */

/* Scratch registers to store info about External Alarm types */
#define ADRV904X_EA_TYPE_TBL_ALARM_0                   281
#define ADRV904X_EA_TYPE_TBL_ALARM_1                   282
#define ADRV904X_EA_TYPE_TBL_ALARM_2                   283
#define ADRV904X_EA_TYPE_TBL_ALARM_3                   284
#define ADRV904X_EA_TYPE_TBL_ALARM_4                   285
#define ADRV904X_EA_TYPE_TBL_ALARM_5                   286
#define ADRV904X_EA_TYPE_TBL_ALARM_6                   287
#define ADRV904X_EA_TYPE_TBL_ALARM_7                   288
#define ADRV904X_EA_TYPE_TBL_ALARM_8                   289
#define ADRV904X_EA_TYPE_TBL_ALARM_9                   290

/* Scratch registers to store info about which AUX_GRP1 outputs are impacted by a given External Alarm */
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_0    291
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_1    292
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_2    293
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_3    294
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_4    295
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_5    296
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_6    297
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_7    298
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_8    299
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_9    300

/* Scratch registers to store info about which AUX_GRP0 outputs are impacted by a given External Alarm */
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_0    301
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_1    302
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_2    303
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_3    304
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_4    305
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_5    306
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_6    307
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_7    308
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_8    309
#define ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_9    310

/* Scratch registers to store info about which LNA_EN outputs are currently affected by an active alarm */
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_0      311
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_1      312
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_2      313
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_3      314
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_4      315
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_5      316
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_6      317
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_7      318
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_8      319
#define ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_9      320

/* Scratch registers to store info about which PA_EN outputs are currently affected by an active alarm */
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_0       321
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_1       322
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_2       323
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_3       324
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_4       325
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_5       326
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_6       327
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_7       328
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_8       329
#define ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_9       330

/* Rename to ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_0 once OCP group added to alarms) */
/* Scratch registers to store info about which OC_FUSE_EN outputs are currently affected by an active alarm */
/* Currently not used for OC_FUSE_EN feature */

#define ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_0                   331
#define ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_1                   332
#define ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_2                   333
#define ADRV904X_SBET_DPD_TO_CFR_TRANSLATION_TBL_3                   334
#define ADRV904X_SBET_INTERNAL_USE_0                                 335
#define ADRV904X_SBET_INTERNAL_USE_1                                 336
#define ADRV904X_SBET_INTERNAL_USE_2                                 337
#define ADRV904X_SBET_INTERNAL_USE_3                                 338

/*
 * Bit 0-1: Target DPD Model Index, Bit 2: SBET Capture Request Active, Bit 3: SBET Capture Delay Set Flag,
 * Bit 4: SBET Capture Delay Type, Bit 5-7: TX Channel
 */
#define ADRV904X_SBET_CAP_REQ_FOR_BUF0                               339
#define ADRV904X_SBET_CAP_REQ_FOR_BUF1                               340

/* DTX fanout to PA_EN table */
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX0                         341
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX1                         342
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX2                         343
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX3                         344
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX4                         345
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX5                         346
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX6                         347
#define ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX7                         348

/* Bitmap of flags indicating a non-zero DTX to PA_EN DTX fanout for all TXes */
#define ADRV904X_NONZERO_DTX_FANOUT_TO_PA_EN_FOR_TX_BM               349  /* 0 - zero fanout, 1 - non-zero fanout */

#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY0_TX3_TX0                 350
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY0_TX7_TX4                 351
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY1_TX3_TX0                 352
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY1_TX7_TX4                 353
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY2_TX3_TX0                 354
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY2_TX7_TX4                 355
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY3_TX3_TX0                 356
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY3_TX7_TX4                 357
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY4_TX3_TX0                 358
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY4_TX7_TX4                 359
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY5_TX3_TX0                 360
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY5_TX7_TX4                 361
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY6_TX3_TX0                 362
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY6_TX7_TX4                 363
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY7_TX3_TX0                 364
#define ADRV904X_SBET_CIRCULAR_BUFFER_ENTRY7_TX7_TX4                 365
#define ADRV904X_SBET_CIRCULAR_BUFFER_INDEX                          366

/* SBET CAP flags: 'Target DPD Model Index' (bit0..1), 'Capture Requested Flag' (bit 2), 'Delay Set Flag' (bit 3), 'Delay Type' (bit 4) for BUF0 */
#define ADRV904X_SBET_TEMP_CAP_REQ_FOR_BUF0                          367

/* SBET CAP flags: 'Target DPD Model Index' (bit0..1), 'Capture Requested Flag' (bit 2), 'Delay Set Flag' (bit 3), 'Delay Type' (bit 4) for BUF1 */
#define ADRV904X_SBET_TEMP_CAP_REQ_FOR_BUF1                          368

/* SBET DPD Model Index for for all TXes */
#define ADRV904X_SBET_DPD_MODEL_IDX_LSB                              369     /* TX3 TX2 TX1 TX0 */
#define ADRV904X_SBET_DPD_MODEL_IDX_MSB                              370     /* TX7 TX6 TX5 TX4 */

/* Scratch register to store info about which outputs have the 'LNA_EN Always ON' feature activated */
#define ADRV904X_EA_LNA_EN_ALWAYS_ON                                 371

/* Scratch register to store info about which outputs have the 'LNA_EN Always OFF' feature activated */
#define ADRV904X_EA_LNA_EN_ALWAYS_OFF                                372

/* Scratch register to store info about which outputs have the 'PA_EN Always ON' feature activated */
#define ADRV904X_EA_PA_EN_ALWAYS_ON                                  373

/* Scratch register to store info about which outputs have the 'PA_EN Always ON' feature activated */
#define ADRV904X_EA_PA_EN_ALWAYS_OFF                                 374

/* Scratch register to store info about which PA_EN signals have the DTX output enabled */
#define ADRV904X_EA_PA_EN_WITH_DTX_OUTPUT                            375 /* 0 - PA mode (driven by RS), 1 - DTX mode */

/* Scratch register to temporarily store External Alarm ID or LNA_EN/PA_EN bitmaps */
#define ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM                           376

/* Scratch register used by API to store info about which PA_EN signals have the DTX output enabled */
#define ADRV904X_EA_PA_EN_WITH_DTX_OUTPUT_SHADOW                     377 /* 0 - PA mode (driven by RS), 1 - DTX mode */

/* Scratch register to support Buffer0 Rx captures which toggles the capture SW trigger - set any bit */
#define ADRV904X_PENDING_BUF0_RX_CAPTURE_REQUEST                     378

/* Scratch register to support Buffer1 Rx captures which toggles the capture SW trigger - set any bit */
#define ADRV904X_PENDING_BUF1_RX_CAPTURE_REQUEST                     379

/* Scratch register to store info about which AUX_GRP0 outputs are frozen by a debug command */
#define ADRV904X_EA_AUX_GRP0_OUTPUT_FROZEN                           380

/* Scratch register to store info about which PA_EN outputs are frozen by a debug command */
#define ADRV904X_EA_PA_EN_OUTPUT_FROZEN                              381

/* Scratch register to store info about which LNA_EN outputs are frozen by a debug command */
#define ADRV904X_EA_LNA_EN_OUTPUT_FROZEN                             382

/* Scratch register to store info about which AUX_GRP1 outputs are frozen by a debug command */
#define ADRV904X_EA_AUX_GRP1_OUTPUT_FROZEN                           383

/* Scratch register used to store Tx-to-Orx mapping pin state information for ORX0 All Off state */
#define ADRV904X_CPU_TX_ORX_MAPPING_PIN_STATE_ORX0_ALL_OFF           384

/*
 * Scratch registers used to store the value of gpio_analog_from_master_set/gpio_analog_from_master_clear
 * bitfield which are set by core stream when configuring the Tx-to-Orx0 mapping
 */
#define ADRV904X_ORX0_GPIO_ANALOG_FROM_MASTER_SET_0_7                385 /* for ORx0, gpio_analog_from_master_set[7:0] */
#define ADRV904X_ORX0_GPIO_ANALOG_FROM_MASTER_SET_15_8               386 /* for ORx0, gpio_analog_from_master_set[15:8] */
#define ADRV904X_ORX0_GPIO_ANALOG_FROM_MASTER_CLEAR_0_7              387 /* for ORx0, gpio_analog_from_master_clear[7:0] */
#define ADRV904X_ORX0_GPIO_ANALOG_FROM_MASTER_CLEAR_15_8             388 /* for ORx0, gpio_analog_from_master_clear[15:8] */

/*
 * Scratch registers used to store the value of gpio_analog_from_master_set/gpio_analog_from_master_clear
 * bitfield which are set by core stream when configuring the Tx-to-Orx1 mapping
 */
#define ADRV904X_ORX1_GPIO_ANALOG_FROM_MASTER_SET_0_7                389 /* for ORx1, gpio_analog_from_master_set[7:0] */
#define ADRV904X_ORX1_GPIO_ANALOG_FROM_MASTER_SET_15_8               390 /* for ORx1, gpio_analog_from_master_set[15:8] */
#define ADRV904X_ORX1_GPIO_ANALOG_FROM_MASTER_CLEAR_0_7              391 /* for ORx1, gpio_analog_from_master_clear[7:0] */
#define ADRV904X_ORX1_GPIO_ANALOG_FROM_MASTER_CLEAR_15_8             392 /* for ORx1, gpio_analog_from_master_clear[15:8] */

/* Scratch registers to store carrierX gain for common channel during antenna cal */
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH0_CARRIER_GAIN_BYTE_0    393
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH0_CARRIER_GAIN_BYTE_1    394
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH0_CARRIER_GAIN_BYTE_2    395
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH1_CARRIER_GAIN_BYTE_0    396
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH1_CARRIER_GAIN_BYTE_1    397
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH1_CARRIER_GAIN_BYTE_2    398
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH2_CARRIER_GAIN_BYTE_0    399
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH2_CARRIER_GAIN_BYTE_1    400
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH2_CARRIER_GAIN_BYTE_2    401
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH3_CARRIER_GAIN_BYTE_0    402
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH3_CARRIER_GAIN_BYTE_1    403
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH3_CARRIER_GAIN_BYTE_2    404
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH4_CARRIER_GAIN_BYTE_0    405
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH4_CARRIER_GAIN_BYTE_1    406
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH4_CARRIER_GAIN_BYTE_2    407
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH5_CARRIER_GAIN_BYTE_0    408
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH5_CARRIER_GAIN_BYTE_1    409
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH5_CARRIER_GAIN_BYTE_2    410
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH6_CARRIER_GAIN_BYTE_0    411
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH6_CARRIER_GAIN_BYTE_1    412
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH6_CARRIER_GAIN_BYTE_2    413
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH7_CARRIER_GAIN_BYTE_0    414
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH7_CARRIER_GAIN_BYTE_1    415
#define ADRV904X_ANTENNA_CAL_STORE_COMMON_CH7_CARRIER_GAIN_BYTE_2    416

/* Scratch registers to store carrierX gain for all channels */
#define ADRV904X_ANTENNA_CAL_STORE_CH0_CARRIER_GAIN_BYTE_0           417
#define ADRV904X_ANTENNA_CAL_STORE_CH0_CARRIER_GAIN_BYTE_1           418
#define ADRV904X_ANTENNA_CAL_STORE_CH0_CARRIER_GAIN_BYTE_2           419
#define ADRV904X_ANTENNA_CAL_STORE_CH1_CARRIER_GAIN_BYTE_0           420
#define ADRV904X_ANTENNA_CAL_STORE_CH1_CARRIER_GAIN_BYTE_1           421
#define ADRV904X_ANTENNA_CAL_STORE_CH1_CARRIER_GAIN_BYTE_2           422
#define ADRV904X_ANTENNA_CAL_STORE_CH2_CARRIER_GAIN_BYTE_0           423
#define ADRV904X_ANTENNA_CAL_STORE_CH2_CARRIER_GAIN_BYTE_1           424
#define ADRV904X_ANTENNA_CAL_STORE_CH2_CARRIER_GAIN_BYTE_2           425
#define ADRV904X_ANTENNA_CAL_STORE_CH3_CARRIER_GAIN_BYTE_0           426
#define ADRV904X_ANTENNA_CAL_STORE_CH3_CARRIER_GAIN_BYTE_1           427
#define ADRV904X_ANTENNA_CAL_STORE_CH3_CARRIER_GAIN_BYTE_2           428
#define ADRV904X_ANTENNA_CAL_STORE_CH4_CARRIER_GAIN_BYTE_0           429
#define ADRV904X_ANTENNA_CAL_STORE_CH4_CARRIER_GAIN_BYTE_1           430
#define ADRV904X_ANTENNA_CAL_STORE_CH4_CARRIER_GAIN_BYTE_2           431
#define ADRV904X_ANTENNA_CAL_STORE_CH5_CARRIER_GAIN_BYTE_0           432
#define ADRV904X_ANTENNA_CAL_STORE_CH5_CARRIER_GAIN_BYTE_1           433
#define ADRV904X_ANTENNA_CAL_STORE_CH5_CARRIER_GAIN_BYTE_2           434
#define ADRV904X_ANTENNA_CAL_STORE_CH6_CARRIER_GAIN_BYTE_0           435
#define ADRV904X_ANTENNA_CAL_STORE_CH6_CARRIER_GAIN_BYTE_1           436
#define ADRV904X_ANTENNA_CAL_STORE_CH6_CARRIER_GAIN_BYTE_2           437
#define ADRV904X_ANTENNA_CAL_STORE_CH7_CARRIER_GAIN_BYTE_0           438
#define ADRV904X_ANTENNA_CAL_STORE_CH7_CARRIER_GAIN_BYTE_1           439
#define ADRV904X_ANTENNA_CAL_STORE_CH7_CARRIER_GAIN_BYTE_2           440

/* Scratch registers to store pattern addresses for RS0 to switch to during antenna cal */
#define ADRV904X_ANTENNA_CAL_RS0_PATTERN0_ADDR_LSB                   441 /* LSB of default pattern */
#define ADRV904X_ANTENNA_CAL_RS0_PATTERN0_ADDR_MSB                   442 /* MSB of default pattern */
#define ADRV904X_ANTENNA_CAL_RS0_PATTERN1_ADDR_LSB                   443
#define ADRV904X_ANTENNA_CAL_RS0_PATTERN1_ADDR_MSB                   444

/* Scratch register to store number of gpio rising edges to switch to next carrier gain */
#define ADRV904X_ANTENNA_CAL_NUMBER_OF_GPIO_TO_SWITCH_CARRIER        445

/* Scratch register to store common Rx channel mask */
#define ADRV904X_ANTENNA_CAL_COMMON_RX_CHANNEL_MASK                  446

/* Scratch register to store common Tx channel mask */
#define ADRV904X_ANTENNA_CAL_COMMON_TX_CHANNEL_MASK                  447

/* Scratch register to store carrier gain table to be used during antenna cal */
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_0               448
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_1               449
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_2               450
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_3               451
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_4               452
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_5               453
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_6               454
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_7               455

/* Scratch register to store number of loops for the first pattern during antenna cal */
#define ADRV904X_ANTENNA_CAL_FIRST_PATTERN_NUMBER_OF_LOOPS           456

/* Scratch register to store number of loops for the second pattern during antenna cal */
#define ADRV904X_ANTENNA_CAL_SECOND_PATTERN_NUMBER_OF_LOOPS          457

/* Scratch register to store antenna cal GPIO rising edge counter */
#define ADRV904X_ANTENNA_CAL_GPIO_RISING_EDGE_COUNTER                458

/* Scratch register to store antenna cal carrier gain table index */
#define ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_INDEX                459

/* In DFE (RS) mode this table is used to store GPIO pin values for ORX1 */
#define ADRV904X_CPU_TX_ORX1_MAPPING_00                              460
#define ADRV904X_CPU_TX_ORX1_MAPPING_01                              461
#define ADRV904X_CPU_TX_ORX1_MAPPING_02                              462
#define ADRV904X_CPU_TX_ORX1_MAPPING_03                              463
#define ADRV904X_CPU_TX_ORX1_MAPPING_04                              464
#define ADRV904X_CPU_TX_ORX1_MAPPING_05                              465
#define ADRV904X_CPU_TX_ORX1_MAPPING_06                              466
#define ADRV904X_CPU_TX_ORX1_MAPPING_07                              467
#define ADRV904X_CPU_TX_ORX1_MAPPING_08                              468
#define ADRV904X_CPU_TX_ORX1_MAPPING_09                              469
#define ADRV904X_CPU_TX_ORX1_MAPPING_10                              470
#define ADRV904X_CPU_TX_ORX1_MAPPING_11                              471
#define ADRV904X_CPU_TX_ORX1_MAPPING_12                              472
#define ADRV904X_CPU_TX_ORX1_MAPPING_13                              473
#define ADRV904X_CPU_TX_ORX1_MAPPING_14                              474
#define ADRV904X_CPU_TX_ORX1_MAPPING_15                              475

/* Scratch register used to store Tx-to-Orx mapping pin state information for All Off state */
#define ADRV904X_CPU_TX_ORX_MAPPING_PIN_STATE_ORX1_ALL_OFF           476

/* Scratch register used for A55 to request an Actuator LUT Copy DMA request in TDD mode */
#define ADRV904X_DFE_ACT_LUT_COPY_TX_REQUEST_TEMP                    477
#define ADRV904X_DFE_ACT_LUT_COPY_TX_REQUEST_LIVE                    478
#define ADRV904X_DFE_ACT_LUT_COPY_TX_DMA_IN_PROGRESS                 479
#define ADRV904X_DFE_ACT_LUT_COPY_TX_DMA_DONE_TEMP                   480

/*
 * Scratch registers used to store channel masks for DTX input pins.
 * Koror only, pin used as PA_EN mux update stream trigger.
 */
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_0                            481
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_1                            482
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_2                            483
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_3                            484
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_4                            485
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_5                            486
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_6                            487
#define ADRV904X_DTX_INPUT_CHANNEL_MASK_7                            488

/* Scratch registers used by 'DTX Event Wait Time' method */
#define ADRV904X_PA_EN_AFTER_DTX_DEACTIVATED_BM                      489
#define ADRV904X_PA_EN_AFTER_DTX_ACTIVATED_BM                        490
#define ADRV904X_UPDATE_PA_EN_AFTER_DTX_DEACTIVATED_TIMER            491
#define ADRV904X_UPDATE_PA_EN_AFTER_DTX_ACTIVATED_TIMER              492

/* Scratch registers containing Tx channel selection for CLGC_POWER_METERS_RESTART_ORX0/1 streams (RCI mode only)*/
#define ADRV904X_CLGC_POWER_METER_ORX0_TX_CHANNEL_SEL                493
#define ADRV904X_CLGC_POWER_METER_ORX1_TX_CHANNEL_SEL                494

/* Scratch registers to store pattern addresses for RS1 to switch to during antenna cal */
#define ADRV904X_ANTENNA_CAL_RS1_PATTERN0_ADDR_LSB                   495 /* LSB of default pattern */
#define ADRV904X_ANTENNA_CAL_RS1_PATTERN0_ADDR_MSB                   496 /* MSB of default pattern */
#define ADRV904X_ANTENNA_CAL_RS1_PATTERN1_ADDR_LSB                   497
#define ADRV904X_ANTENNA_CAL_RS1_PATTERN1_ADDR_MSB                   498

/* Scratch registers to store pattern id to switch to during antenna cal */
#define ADRV904X_ANTENNA_CAL_PATTERN_ID_TO_SWITCH_BYTE_0             499

/* Scratch register used to store bitmap of 'forced' by API FB_EN outputs (Tx-to-ORx mapping output gpios) so they're not updated by stream */
#define ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX0                      500
#define ADRV904X_FORCED_FB_SW_STATE_BITMAP_ORX1                      501

/* Scratch register to store External Alarm Input Pin Polarity bitmap */
#define ADRV904X_EA_INPUT_PIN_POLARITY_BM_0                          502
#define ADRV904X_EA_INPUT_PIN_POLARITY_BM_1                          503

/* Scratch register used to store antenna cal truth table selection. 0=Main path truth table, 1=Secondary path truth table */
#define ADRV904X_ANTENNA_CAL_GPIO_TRUTH_TABLE_SEL                    504

/* Scratch register containing slice stream processor stream number to trigger. Used by DFE */
#define ADRV904X_TRIGGER_SLICE_STREAM_NUM_USED_BY_DFE                505

/* Scratch register containing channel mask of which slice stream processors to trigger stream in. Used by DFE */
#define ADRV904X_TRIGGER_SLICE_STREAM_MASK_USED_BY_DFE               506

/* EXTENDED CORE SCRATCH REGISTERS STORED IN M4 HEAP MEMORY - There are 128 words (512 bytes) reserved. */
/* This section of memory is word accessible and it's read-only for core stream. This is to avoid
 * race conditions between M4 ECC scrubbing and core stream write access. BBIC can read/write these scratch registers.
 * When BBIC will write these extended scratch registers, M4 ECC scrubbing needs to be disabled with mailbox command to Radio FW. */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_0_GAIN    0             /* Extended core scratch registers Word 0 - Bit0-22:Tx Carrier0 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_1_GAIN    1             /* Extended core scratch registers Word 1 - Bit0-22:Tx Carrier1 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_2_GAIN    2             /* Extended core scratch registers Word 2 - Bit0-22:Tx Carrier2 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_3_GAIN    3             /* Extended core scratch registers Word 3 - Bit0-22:Tx Carrier3 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_4_GAIN    4             /* Extended core scratch registers Word 4 - Bit0-22:Tx Carrier4 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_5_GAIN    5             /* Extended core scratch registers Word 5 - Bit0-22:Tx Carrier5 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_6_GAIN    6             /* Extended core scratch registers Word 6 - Bit0-22:Tx Carrier6 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_TX_CARRIER_7_GAIN    7             /* Extended core scratch registers Word 7 - Bit0-22:Tx Carrier7 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_0_GAIN    8             /* Extended core scratch registers Word 8 - Bit0-22:Rx Carrier0 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_1_GAIN    9             /* Extended core scratch registers Word 9 - Bit0-22:Rx Carrier1 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_2_GAIN    10            /* Extended core scratch registers Word 10- Bit0-22:Rx Carrier2 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_3_GAIN    11            /* Extended core scratch registers Word 11- Bit0-22:Rx Carrier3 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_4_GAIN    12            /* Extended core scratch registers Word 12- Bit0-22:Rx Carrier4 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_5_GAIN    13            /* Extended core scratch registers Word 13- Bit0-22:Rx Carrier5 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_6_GAIN    14            /* Extended core scratch registers Word 14- Bit0-22:Rx Carrier6 gain. Bit23-31:Reserved */
#define ADRV904X_ANTENNA_CAL_RX_CARRIER_7_GAIN    15            /* Extended core scratch registers Word 15- Bit0-22:Rx Carrier7 gain. Bit23-31:Reserved */

#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_0        16            /* Extended core scratch registers Word 16- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_1        17            /* Extended core scratch registers Word 17- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_2        18            /* Extended core scratch registers Word 18- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_3        19            /* Extended core scratch registers Word 19- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_4        20            /* Extended core scratch registers Word 20- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_5        21            /* Extended core scratch registers Word 21- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_6        22            /* Extended core scratch registers Word 22- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_7        23            /* Extended core scratch registers Word 23- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_8        24            /* Extended core scratch registers Word 24- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_9        25            /* Extended core scratch registers Word 25- Bit0-7:Impacted LNA_EN by given alarm. Bit8-31:Rsrvd */

#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_0         26            /* Extended core scratch registers Word 26- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_1         27            /* Extended core scratch registers Word 27- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_2         28            /* Extended core scratch registers Word 28- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_3         29            /* Extended core scratch registers Word 29- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_4         30            /* Extended core scratch registers Word 30- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_5         31            /* Extended core scratch registers Word 31- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_6         32            /* Extended core scratch registers Word 32- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_7         33            /* Extended core scratch registers Word 33- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_8         34            /* Extended core scratch registers Word 34- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_PA_EN_TBL_ALARM_9         35            /* Extended core scratch registers Word 35- Bit0-7:Impacted PA_EN by given alarm. Bit8-31:Rsrvd */

#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_0      36            /* Extended core scratch registers Word 36- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_1      37            /* Extended core scratch registers Word 37- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_2      38            /* Extended core scratch registers Word 38- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_3      39            /* Extended core scratch registers Word 39- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_4      40            /* Extended core scratch registers Word 40- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_5      41            /* Extended core scratch registers Word 41- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_6      42            /* Extended core scratch registers Word 42- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_7      43            /* Extended core scratch registers Word 43- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_8      44            /* Extended core scratch registers Word 44- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_9      45            /* Extended core scratch registers Word 45- Bit0-7:Impacted AUX_GRP1 by given alarm. Bit8-31:Rsrvd */

#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_0      46            /* Extended core scratch registers Word 46- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_1      47            /* Extended core scratch registers Word 47- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_2      48            /* Extended core scratch registers Word 48- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_3      49            /* Extended core scratch registers Word 49- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_4      50            /* Extended core scratch registers Word 50- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_5      51            /* Extended core scratch registers Word 51- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_6      52            /* Extended core scratch registers Word 52- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_7      53            /* Extended core scratch registers Word 53- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_8      54            /* Extended core scratch registers Word 54- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_9      55            /* Extended core scratch registers Word 55- Bit0-7:Impacted AUX_GRP0 by given alarm. Bit8-31:Rsrvd */

#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_0    56            /* Extended core scratch registers Word 56- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_1    57            /* Extended core scratch registers Word 57- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_2    58            /* Extended core scratch registers Word 58- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_3    59            /* Extended core scratch registers Word 59- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_4    60            /* Extended core scratch registers Word 60- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_5    61            /* Extended core scratch registers Word 61- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_6    62            /* Extended core scratch registers Word 62- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_7    63            /* Extended core scratch registers Word 63- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_8    64            /* Extended core scratch registers Word 64- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */
#define ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_9    65            /* Extended core scratch registers Word 65- Bit0-7:Impacted OC_FUSE_EN by given alarm. Bit8-31:Rsrvd */

#endif /* __ADRV904X_CPU_SCRATCH_REGISTERS_H__ */


