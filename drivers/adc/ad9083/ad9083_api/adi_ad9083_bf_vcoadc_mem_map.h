/*!
 * @brief     SPI Register Definition Header File, automatically generated by
 *            yoda2h v1.1.2 at 1/2/2020 9:13:37 AM.
 *
 * @copyright copyright(c) 2018 - Analog Devices Inc.All Rights Reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9083_BF
 * @{
 */
#ifndef __ADI_AD9083_BF_VCOADC_MEM_MAP_H__
#define __ADI_AD9083_BF_VCOADC_MEM_MAP_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9083_config.h"

/*============= D E F I N E S ==============*/
#define REG_POWER_DOWN_REG_ADDR 0x00000B90
#define BF_EN_VTI_INFO 0x00000B90, 0x00000100
#define BF_EN_ADCCLK_INFO 0x00000B90, 0x00000101
#define BF_EN_BIAS_INFO 0x00000B90, 0x00000102
#define BF_PIN_PD_MODE_INFO 0x00000B90, 0x00000203
#define BF_EN_34_INFO 0x00000B90, 0x00000105
#define BF_EN_CAL_CLK_INFO 0x00000B90, 0x00000106
#define BF_EN_CAL_ANA_INFO 0x00000B90, 0x00000107

#define REG_ENABLE_CH7_0_REG_ADDR 0x00000B91
#define BF_EN_ADC0_INFO 0x00000B91, 0x00000100
#define BF_EN_ADC1_INFO 0x00000B91, 0x00000101
#define BF_EN_ADC2_INFO 0x00000B91, 0x00000102
#define BF_EN_ADC3_INFO 0x00000B91, 0x00000103
#define BF_EN_ADC4_INFO 0x00000B91, 0x00000104
#define BF_EN_ADC5_INFO 0x00000B91, 0x00000105
#define BF_EN_ADC6_INFO 0x00000B91, 0x00000106
#define BF_EN_ADC7_INFO 0x00000B91, 0x00000107

#define REG_ENABLE_CH15_8_REG_ADDR 0x00000B92
#define BF_EN_ADC8_INFO 0x00000B92, 0x00000100
#define BF_EN_ADC9_INFO 0x00000B92, 0x00000101
#define BF_EN_ADC10_INFO 0x00000B92, 0x00000102
#define BF_EN_ADC11_INFO 0x00000B92, 0x00000103
#define BF_EN_ADC12_INFO 0x00000B92, 0x00000104
#define BF_EN_ADC13_INFO 0x00000B92, 0x00000105
#define BF_EN_ADC14_INFO 0x00000B92, 0x00000106
#define BF_EN_ADC15_INFO 0x00000B92, 0x00000107

#define REG_VTI_GAIN_REG_ADDR 0x00000B94
#define BF_BGAIN_INFO 0x00000B94, 0x00000600
#define BF_SPARE_REG_04_6_INFO 0x00000B94, 0x00000106
#define BF_SPARE_REG_04_7_INFO 0x00000B94, 0x00000107

#define REG_VTI_LPF_CAP_REG_ADDR 0x00000B95
#define BF_BCAP_INFO 0x00000B95, 0x00000600
#define BF_SPARE_REG_05_6_INFO 0x00000B95, 0x00000106
#define BF_SPARE_REG_05_7_INFO 0x00000B95, 0x00000107

#define REG_DITHER_DAC_CURRENT1_REG_ADDR 0x00000B99
#define BF_BDITHDAC1_INFO 0x00000B99, 0x00000800

#define REG_DITHER_DAC_CURRENT2_REG_ADDR 0x00000B9A
#define BF_BDITHDAC2_INFO 0x00000B9A, 0x00000800

#define REG_VTI_SHIFT_CURRENT_MSB_REG_ADDR 0x00000BA2
#define BF_RTERM_INFO 0x00000BA2, 0x00000204
#define BF_SPARE_REG_12_6_INFO 0x00000BA2, 0x00000106
#define BF_SPARE_REG_12_7_INFO 0x00000BA2, 0x00000107

#define REG_CAL_EN_ADDR 0x00000BB3
#define BF_ENABLE_CLOCK_INFO 0x00000BB3, 0x00000107

#define REG_KGAIN_VAL0_ADDR 0x00000BBA
#define BF_KGAIN_VAL_INFO 0x00000BBA, 0x00001600

#define REG_BCENTER_1_ADDR 0x00000BC3
#define BF_BCENTER_OFFSET_INFO 0x00000BC3, 0x00000602

#define REG_DIVM_RC_ADDR 0x00000BC8
#define BF_DIVM_RC_INFO 0x00000BC8, 0x00000800

#endif /* __ADI_AD9083_BF_VCOADC_MEM_MAP_H__ */
       /*! @} */