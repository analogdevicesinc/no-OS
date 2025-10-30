/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv903x_cpu_scratch_registers.h
 *
 * \brief   Contains CPU scratch register definitions
 *
 * \details Contains CPU scratch register definitions
 *
 * ADRV903X API Version: 2.12.1.4
 */

#ifndef __ADRV903X_CPU_SCRATCH_REGISTERS_H__
#define __ADRV903X_CPU_SCRATCH_REGISTERS_H__

/* Scratch register 0 contains CPU0 boot status */
#define ADRV903X_CPU_CPU0_BOOT_STATUS_SCRATCH_REG_ID    0

/* Scratch register 1 contains CPU1 boot status */
#define ADRV903X_CPU_CPU1_BOOT_STATUS_SCRATCH_REG_ID    1

/* Scratch register 6 contains Stream/API Tx to ORx Mapping byte */
#define ADRV903X_CPU_TX_ORX_MAPPING_SET_SCRATCH_REG_ID      6

/* Scratch register 7 contains config needed by ORX High Stream for JESD Link Sharing */
#define ADRV903X_CPU_ORX_LINK_SHARING_SCRATCH_REG_ID        7

/* Scratch register 8 contains config needed by RX0-3 High Streams for JESD Link Sharing */
#define ADRV903X_CPU_TX_RX03_MAPPING_SET_SCRATCH_REG_ID     8

/* Scratch register 9 contains config needed by RX4-7 High Streams for JESD Link Sharing */
#define ADRV903X_CPU_TX_RX47_MAPPING_SET_SCRATCH_REG_ID     9

/* Software Breakpoint scratchpad register assignments */
#define ADRV903X_CPU_CPU0_SWBKPT_INDEX_SCRATCH_REG_ID       10
#define ADRV903X_CPU_CPU0_SWBKPT_BKPT_NUM_SCRATCH_REG_ID    11
#define ADRV903X_CPU_CPU0_SWBKPT_CHAN_NUM_SCRATCH_REG_ID    12
#define ADRV903X_CPU_CPU1_SWBKPT_INDEX_SCRATCH_REG_ID       13
#define ADRV903X_CPU_CPU1_SWBKPT_BKPT_NUM_SCRATCH_REG_ID    14
#define ADRV903X_CPU_CPU1_SWBKPT_CHAN_NUM_SCRATCH_REG_ID    15

/* CPU Primary/Secondary boot sequencing registers */
#define ADRV903X_CPU_CPU0_IS_PRIMARY                        16
#define ADRV903X_CPU_CPU1_IS_PRIMARY                        17
#define ADRV903X_CPU_SECONDARY_BOOT_RELEASE    18

/* Scratch registers 19-35 contain Stream/API Tx to ORx Mapping look up table */
#define ADRV903X_CPU_TX_ORX_MAPPING_00    19
#define ADRV903X_CPU_TX_ORX_MAPPING_01    20
#define ADRV903X_CPU_TX_ORX_MAPPING_02    21
#define ADRV903X_CPU_TX_ORX_MAPPING_03    22
#define ADRV903X_CPU_TX_ORX_MAPPING_04    23
#define ADRV903X_CPU_TX_ORX_MAPPING_05    24
#define ADRV903X_CPU_TX_ORX_MAPPING_06    25
#define ADRV903X_CPU_TX_ORX_MAPPING_07    26
#define ADRV903X_CPU_TX_ORX_MAPPING_08    27
#define ADRV903X_CPU_TX_ORX_MAPPING_09    28
#define ADRV903X_CPU_TX_ORX_MAPPING_10    29
#define ADRV903X_CPU_TX_ORX_MAPPING_11    30
#define ADRV903X_CPU_TX_ORX_MAPPING_12    31
#define ADRV903X_CPU_TX_ORX_MAPPING_13    32
#define ADRV903X_CPU_TX_ORX_MAPPING_14    33
#define ADRV903X_CPU_TX_ORX_MAPPING_15    34

/* Scratch registers to hold temporary Tx to ORx NCO and atten values */
#define ADRV903X_CPU_TEMP0                35               /* Tx channel (0 - 7) */
#define ADRV903X_CPU_TEMP1                36               /* LSB of ORx ADC NCO or LOL NCO frequency or ORx atten */
#define ADRV903X_CPU_TEMP2                37               /* ISB of ORx ADC NCO or LOL NCO frequency or ORx atten */
#define ADRV903X_CPU_TEMP3                38               /* MSB of ORx ADC NCO or LOL NCO frequency */
#define ADRV903X_CPU_TEMP4                39               /* LSB of ORx DP NCO or LOL NCO frequency */
#define ADRV903X_CPU_TEMP5                40               /* ISB of ORx DP NCO or LOL NCO frequency */
#define ADRV903X_CPU_TEMP6                41               /* MSB of ORx DP NCO or LOL NCO frequency */

/*
 * Table to hold ADC and DP NCO values for all supported mappings
 */
/* Scratch registers to hold ADC/DP NCO values for mapping=0x00 (Tx0-to-ORx) */
#define ADRV903X_CPU_TX_0_ORX_NCO_ADC_FREQ_LSB    42
#define ADRV903X_CPU_TX_0_ORX_NCO_ADC_FREQ_ISB    43
#define ADRV903X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB    44
#define ADRV903X_CPU_TX_0_ORX_NCO_DP_FREQ_LSB     45
#define ADRV903X_CPU_TX_0_ORX_NCO_DP_FREQ_ISB     46
#define ADRV903X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB     47
/* Scratch registers to hold ADC/DP NCO values for mapping=0x01 (Tx1-to-ORx) */
#define ADRV903X_CPU_TX_1_ORX_NCO_ADC_FREQ_LSB    48
#define ADRV903X_CPU_TX_1_ORX_NCO_ADC_FREQ_ISB    49
#define ADRV903X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB    50
#define ADRV903X_CPU_TX_1_ORX_NCO_DP_FREQ_LSB     51
#define ADRV903X_CPU_TX_1_ORX_NCO_DP_FREQ_ISB     52
#define ADRV903X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB     53
/* Scratch registers to hold ADC/DP NCO values for mapping=0x02 (Tx2-to-ORx) */
#define ADRV903X_CPU_TX_2_ORX_NCO_ADC_FREQ_LSB    54
#define ADRV903X_CPU_TX_2_ORX_NCO_ADC_FREQ_ISB    55
#define ADRV903X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB    56
#define ADRV903X_CPU_TX_2_ORX_NCO_DP_FREQ_LSB     57
#define ADRV903X_CPU_TX_2_ORX_NCO_DP_FREQ_ISB     58
#define ADRV903X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB     59
/* Scratch registers to hold ADC/DP NCO values for mapping=0x03 (Tx3-to-ORx) */
#define ADRV903X_CPU_TX_3_ORX_NCO_ADC_FREQ_LSB    60
#define ADRV903X_CPU_TX_3_ORX_NCO_ADC_FREQ_ISB    61
#define ADRV903X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB    62
#define ADRV903X_CPU_TX_3_ORX_NCO_DP_FREQ_LSB     63
#define ADRV903X_CPU_TX_3_ORX_NCO_DP_FREQ_ISB     64
#define ADRV903X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB     65
/* Scratch registers to hold ADC/DP NCO values for mapping=0x04 (Tx4-to-ORx) */
#define ADRV903X_CPU_TX_4_ORX_NCO_ADC_FREQ_LSB    66
#define ADRV903X_CPU_TX_4_ORX_NCO_ADC_FREQ_ISB    67
#define ADRV903X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB    68
#define ADRV903X_CPU_TX_4_ORX_NCO_DP_FREQ_LSB     69
#define ADRV903X_CPU_TX_4_ORX_NCO_DP_FREQ_ISB     70
#define ADRV903X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB     71
/* Scratch registers to hold ADC/DP NCO values for mapping=0x05 (Tx5-to-ORx) */
#define ADRV903X_CPU_TX_5_ORX_NCO_ADC_FREQ_LSB    72
#define ADRV903X_CPU_TX_5_ORX_NCO_ADC_FREQ_ISB    73
#define ADRV903X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB    74
#define ADRV903X_CPU_TX_5_ORX_NCO_DP_FREQ_LSB     75
#define ADRV903X_CPU_TX_5_ORX_NCO_DP_FREQ_ISB     76
#define ADRV903X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB     77
/* Scratch registers to hold ADC/DP NCO values for mapping=0x06 (Tx6-to-ORx) */
#define ADRV903X_CPU_TX_6_ORX_NCO_ADC_FREQ_LSB    78
#define ADRV903X_CPU_TX_6_ORX_NCO_ADC_FREQ_ISB    79
#define ADRV903X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB    80
#define ADRV903X_CPU_TX_6_ORX_NCO_DP_FREQ_LSB     81
#define ADRV903X_CPU_TX_6_ORX_NCO_DP_FREQ_ISB     82
#define ADRV903X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB     83
/* Scratch registers to hold ADC/DP NCO values for mapping=0x07 (Tx7-to-ORx) */
#define ADRV903X_CPU_TX_7_ORX_NCO_ADC_FREQ_LSB    84
#define ADRV903X_CPU_TX_7_ORX_NCO_ADC_FREQ_ISB    85
#define ADRV903X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB    86
#define ADRV903X_CPU_TX_7_ORX_NCO_DP_FREQ_LSB     87
#define ADRV903X_CPU_TX_7_ORX_NCO_DP_FREQ_ISB     88
#define ADRV903X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB     89
/* Scratch registers to hold ADC/DP NCO values for mapping=0x08 */
#define ADRV903X_CPU_MAPVAL_8_NCO_ADC_FREQ_LSB    90
#define ADRV903X_CPU_MAPVAL_8_NCO_ADC_FREQ_ISB    91
#define ADRV903X_CPU_MAPVAL_8_NCO_ADC_FREQ_MSB    92
#define ADRV903X_CPU_MAPVAL_8_NCO_DP_FREQ_LSB     93
#define ADRV903X_CPU_MAPVAL_8_NCO_DP_FREQ_ISB     94
#define ADRV903X_CPU_MAPVAL_8_NCO_DP_FREQ_MSB     95
/* Scratch registers to hold ADC/DP NCO values for mapping=0x09 */
#define ADRV903X_CPU_MAPVAL_9_NCO_ADC_FREQ_LSB    96
#define ADRV903X_CPU_MAPVAL_9_NCO_ADC_FREQ_ISB    97
#define ADRV903X_CPU_MAPVAL_9_NCO_ADC_FREQ_MSB    98
#define ADRV903X_CPU_MAPVAL_9_NCO_DP_FREQ_LSB     99
#define ADRV903X_CPU_MAPVAL_9_NCO_DP_FREQ_ISB     100
#define ADRV903X_CPU_MAPVAL_9_NCO_DP_FREQ_MSB     101
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0A */
#define ADRV903X_CPU_MAPVAL_A_NCO_ADC_FREQ_LSB    102
#define ADRV903X_CPU_MAPVAL_A_NCO_ADC_FREQ_ISB    103
#define ADRV903X_CPU_MAPVAL_A_NCO_ADC_FREQ_MSB    104
#define ADRV903X_CPU_MAPVAL_A_NCO_DP_FREQ_LSB     105
#define ADRV903X_CPU_MAPVAL_A_NCO_DP_FREQ_ISB     106
#define ADRV903X_CPU_MAPVAL_A_NCO_DP_FREQ_MSB     107
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0B */
#define ADRV903X_CPU_MAPVAL_B_NCO_ADC_FREQ_LSB    108
#define ADRV903X_CPU_MAPVAL_B_NCO_ADC_FREQ_ISB    109
#define ADRV903X_CPU_MAPVAL_B_NCO_ADC_FREQ_MSB    110
#define ADRV903X_CPU_MAPVAL_B_NCO_DP_FREQ_LSB     111
#define ADRV903X_CPU_MAPVAL_B_NCO_DP_FREQ_ISB     112
#define ADRV903X_CPU_MAPVAL_B_NCO_DP_FREQ_MSB     113
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0C */
#define ADRV903X_CPU_MAPVAL_C_NCO_ADC_FREQ_LSB    114
#define ADRV903X_CPU_MAPVAL_C_NCO_ADC_FREQ_ISB    115
#define ADRV903X_CPU_MAPVAL_C_NCO_ADC_FREQ_MSB    116
#define ADRV903X_CPU_MAPVAL_C_NCO_DP_FREQ_LSB     117
#define ADRV903X_CPU_MAPVAL_C_NCO_DP_FREQ_ISB     118
#define ADRV903X_CPU_MAPVAL_C_NCO_DP_FREQ_MSB     119
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0D */
#define ADRV903X_CPU_MAPVAL_D_NCO_ADC_FREQ_LSB    120
#define ADRV903X_CPU_MAPVAL_D_NCO_ADC_FREQ_ISB    121
#define ADRV903X_CPU_MAPVAL_D_NCO_ADC_FREQ_MSB    122
#define ADRV903X_CPU_MAPVAL_D_NCO_DP_FREQ_LSB     123
#define ADRV903X_CPU_MAPVAL_D_NCO_DP_FREQ_ISB     124
#define ADRV903X_CPU_MAPVAL_D_NCO_DP_FREQ_MSB     125
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0E */
#define ADRV903X_CPU_MAPVAL_E_NCO_ADC_FREQ_LSB    126
#define ADRV903X_CPU_MAPVAL_E_NCO_ADC_FREQ_ISB    127
#define ADRV903X_CPU_MAPVAL_E_NCO_ADC_FREQ_MSB    128
#define ADRV903X_CPU_MAPVAL_E_NCO_DP_FREQ_LSB     129
#define ADRV903X_CPU_MAPVAL_E_NCO_DP_FREQ_ISB     130
#define ADRV903X_CPU_MAPVAL_E_NCO_DP_FREQ_MSB     131
/* Scratch registers to hold ADC/DP NCO values for mapping=0x0F */
#define ADRV903X_CPU_MAPVAL_F_NCO_ADC_FREQ_LSB    132
#define ADRV903X_CPU_MAPVAL_F_NCO_ADC_FREQ_ISB    133
#define ADRV903X_CPU_MAPVAL_F_NCO_ADC_FREQ_MSB    134
#define ADRV903X_CPU_MAPVAL_F_NCO_DP_FREQ_LSB     135
#define ADRV903X_CPU_MAPVAL_F_NCO_DP_FREQ_ISB     136
#define ADRV903X_CPU_MAPVAL_F_NCO_DP_FREQ_MSB     137

/*
 * Table to hold attenuation values for all supported mappings
 */
/* Scratch registers to hold attenuation value for mapping=0x00 (Tx0-to-ORx) */
#define ADRV903X_CPU_TX_0_ORX_ATTEN         138
#define ADRV903X_CPU_TX_0_ORX_ATTEN_CTRL    139
/* Scratch registers to hold attenuation value for mapping=0x01 (Tx1-to-ORx) */
#define ADRV903X_CPU_TX_1_ORX_ATTEN         140
#define ADRV903X_CPU_TX_1_ORX_ATTEN_CTRL    141
/* Scratch registers to hold attenuation value for mapping=0x02 (Tx2-to-ORx) */
#define ADRV903X_CPU_TX_2_ORX_ATTEN         142
#define ADRV903X_CPU_TX_2_ORX_ATTEN_CTRL    143
/* Scratch registers to hold attenuation value for mapping=0x03 (Tx3-to-ORx) */
#define ADRV903X_CPU_TX_3_ORX_ATTEN         144
#define ADRV903X_CPU_TX_3_ORX_ATTEN_CTRL    145
/* Scratch registers to hold attenuation value for mapping=0x04 (Tx4-to-ORx) */
#define ADRV903X_CPU_TX_4_ORX_ATTEN         146
#define ADRV903X_CPU_TX_4_ORX_ATTEN_CTRL    147
/* Scratch registers to hold attenuation value for mapping=0x05 (Tx5-to-ORx) */
#define ADRV903X_CPU_TX_5_ORX_ATTEN         148
#define ADRV903X_CPU_TX_5_ORX_ATTEN_CTRL    149
/* Scratch registers to hold attenuation value for mapping=0x06 (Tx6-to-ORx) */
#define ADRV903X_CPU_TX_6_ORX_ATTEN         150
#define ADRV903X_CPU_TX_6_ORX_ATTEN_CTRL    151
/* Scratch registers to hold attenuation value for mapping=0x07 (Tx7-to-ORx) */
#define ADRV903X_CPU_TX_7_ORX_ATTEN         152
#define ADRV903X_CPU_TX_7_ORX_ATTEN_CTRL    153
/* Scratch registers to hold attenuation value for mapping=0x08 */
#define ADRV903X_CPU_MAPVAL_8_ATTEN         154
#define ADRV903X_CPU_MAPVAL_8_ATTEN_CTRL    155
/* Scratch registers to hold attenuation value for mapping=0x09 */
#define ADRV903X_CPU_MAPVAL_9_ATTEN         156
#define ADRV903X_CPU_MAPVAL_9_ATTEN_CTRL    157
/* Scratch registers to hold attenuation value for mapping=0x0A */
#define ADRV903X_CPU_MAPVAL_A_ATTEN         158
#define ADRV903X_CPU_MAPVAL_A_ATTEN_CTRL    159
/* Scratch registers to hold attenuation value for mapping=0x0B */
#define ADRV903X_CPU_MAPVAL_B_ATTEN         160
#define ADRV903X_CPU_MAPVAL_B_ATTEN_CTRL    161
/* Scratch registers to hold attenuation value for mapping=0x0C */
#define ADRV903X_CPU_MAPVAL_C_ATTEN         162
#define ADRV903X_CPU_MAPVAL_C_ATTEN_CTRL    163
/* Scratch registers to hold attenuation value for mapping=0x0D */
#define ADRV903X_CPU_MAPVAL_D_ATTEN         164
#define ADRV903X_CPU_MAPVAL_D_ATTEN_CTRL    165
/* Scratch registers to hold attenuation value for mapping=0x0E */
#define ADRV903X_CPU_MAPVAL_E_ATTEN         166
#define ADRV903X_CPU_MAPVAL_E_ATTEN_CTRL    167
/* Scratch registers to hold attenuation value for mapping=0x0F */
#define ADRV903X_CPU_MAPVAL_F_ATTEN         168
#define ADRV903X_CPU_MAPVAL_F_ATTEN_CTRL    169

/*
 * Table to hold LOL NCO values for all supported mappings
 */
/* Scratch registers to hold LOL NCO values for mapping=0x00 (Tx0-to-ORx) */
#define ADRV903X_CPU_TX_0_ORX_NCO_LOL_FREQ_LSB    170
#define ADRV903X_CPU_TX_0_ORX_NCO_LOL_FREQ_ISB    171
#define ADRV903X_CPU_TX_0_ORX_NCO_LOL_FREQ_MSB    172
/* Scratch registers to hold LOL NCO values for mapping=0x01 (Tx1-to-ORx) */
#define ADRV903X_CPU_TX_1_ORX_NCO_LOL_FREQ_LSB    173
#define ADRV903X_CPU_TX_1_ORX_NCO_LOL_FREQ_ISB    174
#define ADRV903X_CPU_TX_1_ORX_NCO_LOL_FREQ_MSB    175
/* Scratch registers to hold LOL NCO values for mapping=0x02 (Tx2-to-ORx) */
#define ADRV903X_CPU_TX_2_ORX_NCO_LOL_FREQ_LSB    176
#define ADRV903X_CPU_TX_2_ORX_NCO_LOL_FREQ_ISB    177
#define ADRV903X_CPU_TX_2_ORX_NCO_LOL_FREQ_MSB    178
/* Scratch registers to hold LOL NCO values for mapping=0x03 (Tx3-to-ORx) */
#define ADRV903X_CPU_TX_3_ORX_NCO_LOL_FREQ_LSB    179
#define ADRV903X_CPU_TX_3_ORX_NCO_LOL_FREQ_ISB    180
#define ADRV903X_CPU_TX_3_ORX_NCO_LOL_FREQ_MSB    181
/* Scratch registers to hold LOL NCO values for mapping=0x04 (Tx4-to-ORx) */
#define ADRV903X_CPU_TX_4_ORX_NCO_LOL_FREQ_LSB    182
#define ADRV903X_CPU_TX_4_ORX_NCO_LOL_FREQ_ISB    183
#define ADRV903X_CPU_TX_4_ORX_NCO_LOL_FREQ_MSB    184
/* Scratch registers to hold LOL NCO values for mapping=0x05 (Tx5-to-ORx) */
#define ADRV903X_CPU_TX_5_ORX_NCO_LOL_FREQ_LSB    185
#define ADRV903X_CPU_TX_5_ORX_NCO_LOL_FREQ_ISB    186
#define ADRV903X_CPU_TX_5_ORX_NCO_LOL_FREQ_MSB    187
/* Scratch registers to hold LOL NCO values for mapping=0x06 (Tx6-to-ORx) */
#define ADRV903X_CPU_TX_6_ORX_NCO_LOL_FREQ_LSB    188
#define ADRV903X_CPU_TX_6_ORX_NCO_LOL_FREQ_ISB    189
#define ADRV903X_CPU_TX_6_ORX_NCO_LOL_FREQ_MSB    190
/* Scratch registers to hold LOL NCO values for mapping=0x07 (Tx7-to-ORx) */
#define ADRV903X_CPU_TX_7_ORX_NCO_LOL_FREQ_LSB    191
#define ADRV903X_CPU_TX_7_ORX_NCO_LOL_FREQ_ISB    192
#define ADRV903X_CPU_TX_7_ORX_NCO_LOL_FREQ_MSB    193

/* Scratch register used to store ORX_TX_AUTO_SWITCH_MODE information */
#define ADRV903X_CPU_ORX_TX_AUTO_SWITCH_MODE      194


/* Scratch register to enable/disable the TxLB ADC DAC - channelMask (see TPGSWE-2397) */
#define ADRV903X_CPU_TXLB_DAC_ENABLE                  195

/* Software Breakpoint GPIO scratchpad register assignments */
#define ADRV903X_CPU_SWBKPT_WAKEUP_GPIO_PIN_REG_ID    196

/* Set by powerup stream if SBET is enabled in stream settings */
#define ADRV903X_SCRATCH_ID_SBET_MODE                 197

/* Scratch register to enable/disable the Tx Anttena Cal - channelMask */
#define ADRV903X_GPIO_ANTCAL_TX_MASK                  198

/* Scratch register to enable/disable the Rx Anttena Cal - channelMask */
#define ADRV903X_GPIO_ANTCAL_RX_MASK                  199

/* Unused */
#define ADRV903X_SCRATCH_ID_UNUSED_01                 200
#define ADRV903X_SCRATCH_ID_UNUSED_02                 201
#define ADRV903X_SCRATCH_ID_UNUSED_03                 202
#define ADRV903X_SCRATCH_ID_UNUSED_04                 203

/* Scratch register containing slice stream processor stream number to trigger */
#define ADRV903X_TRIGGER_SLICE_STREAM_NUM             204

/* Scratch register containing channel mask of which slice stream processors to trigger stream in */
#define ADRV903X_TRIGGER_SLICE_STREAM_MASK            205

/* Unused */
#define ADRV903X_SCRATCH_ID_UNUSED_05                 206
#define ADRV903X_SCRATCH_ID_UNUSED_06                 207
#define ADRV903X_SCRATCH_ID_UNUSED_07                 208
#define ADRV903X_SCRATCH_ID_UNUSED_08                 209
#define ADRV903X_SCRATCH_ID_UNUSED_09                 210
#define ADRV903X_SCRATCH_ID_UNUSED_10                 211

/* Scratch register that holds TRx channels for gpio 0 */
#define ADRV903X_TRIGGER_SLICE_GPIO_0_TRX_CHANNELS    212

/* Scratch register that holds TRx channels for gpio 1 */
#define ADRV903X_TRIGGER_SLICE_GPIO_1_TRX_CHANNELS    213

/* Scratch register that holds TRx channels for gpio 2 */
#define ADRV903X_TRIGGER_SLICE_GPIO_2_TRX_CHANNELS    214

/* Scratch register that holds TRx channels for gpio 3 */
#define ADRV903X_TRIGGER_SLICE_GPIO_3_TRX_CHANNELS    215

/* Scratch register that holds if channels affect are from Rx or Tx */
#define ADRV903X_TRIGGER_SLICE_TR_OR_RX_CHANNELS      216

/* Unused scratch registers from 229 to 255 */

#endif /* __ADRV903X_CPU_SCRATCH_REGISTERS_H__ */


