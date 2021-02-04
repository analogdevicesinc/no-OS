/*!
 * @brief     SPI Register Definition Header File, automatically generated file at 1/20/2020 6:24:30 AM.
 *
 * @copyright copyright(c) 2018 - Analog Devices Inc.All Rights Reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * @addtogroup AD9081_BF
 * @{
 */
#ifndef __ADI_AD9081_BF_RX_PAGING_H__
#define __ADI_AD9081_BF_RX_PAGING_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"

/*============= D E F I N E S ==============*/
#define REG_ADC_COARSE_PAGE_ADDR 0x00000018
#define BF_ADC0_PAGE_INFO 0x00000100
#define BF_ADC0_PAGE(val) (val & 0x00000001)
#define BF_ADC0_PAGE_GET(val) (val & 0x00000001)
#define BF_ADC1_PAGE_INFO 0x00000101
#define BF_ADC1_PAGE(val) ((val & 0x00000001) << 0x00000001)
#define BF_ADC1_PAGE_GET(val) ((val >> 0x00000001) & 0x00000001)
#define BF_ADC2_PAGE_INFO 0x00000102
#define BF_ADC2_PAGE(val) ((val & 0x00000001) << 0x00000002)
#define BF_ADC2_PAGE_GET(val) ((val >> 0x00000002) & 0x00000001)
#define BF_ADC3_PAGE_INFO 0x00000103
#define BF_ADC3_PAGE(val) ((val & 0x00000001) << 0x00000003)
#define BF_ADC3_PAGE_GET(val) ((val >> 0x00000003) & 0x00000001)
#define BF_COARSE_DDC0_PAGE_INFO 0x00000104
#define BF_COARSE_DDC0_PAGE(val) ((val & 0x00000001) << 0x00000004)
#define BF_COARSE_DDC0_PAGE_GET(val) ((val >> 0x00000004) & 0x00000001)
#define BF_COARSE_DDC1_PAGE_INFO 0x00000105
#define BF_COARSE_DDC1_PAGE(val) ((val & 0x00000001) << 0x00000005)
#define BF_COARSE_DDC1_PAGE_GET(val) ((val >> 0x00000005) & 0x00000001)
#define BF_COARSE_DDC2_PAGE_INFO 0x00000106
#define BF_COARSE_DDC2_PAGE(val) ((val & 0x00000001) << 0x00000006)
#define BF_COARSE_DDC2_PAGE_GET(val) ((val >> 0x00000006) & 0x00000001)
#define BF_COARSE_DDC3_PAGE_INFO 0x00000107
#define BF_COARSE_DDC3_PAGE(val) ((val & 0x00000001) << 0x00000007)
#define BF_COARSE_DDC3_PAGE_GET(val) ((val >> 0x00000007) & 0x00000001)

#define REG_FINE_DDC_PAGE_ADDR 0x00000019
#define BF_FINE_DDC0_PAGE_INFO 0x00000100
#define BF_FINE_DDC0_PAGE(val) (val & 0x00000001)
#define BF_FINE_DDC0_PAGE_GET(val) (val & 0x00000001)
#define BF_FINE_DDC1_PAGE_INFO 0x00000101
#define BF_FINE_DDC1_PAGE(val) ((val & 0x00000001) << 0x00000001)
#define BF_FINE_DDC1_PAGE_GET(val) ((val >> 0x00000001) & 0x00000001)
#define BF_FINE_DDC2_PAGE_INFO 0x00000102
#define BF_FINE_DDC2_PAGE(val) ((val & 0x00000001) << 0x00000002)
#define BF_FINE_DDC2_PAGE_GET(val) ((val >> 0x00000002) & 0x00000001)
#define BF_FINE_DDC3_PAGE_INFO 0x00000103
#define BF_FINE_DDC3_PAGE(val) ((val & 0x00000001) << 0x00000003)
#define BF_FINE_DDC3_PAGE_GET(val) ((val >> 0x00000003) & 0x00000001)
#define BF_FINE_DDC4_PAGE_INFO 0x00000104
#define BF_FINE_DDC4_PAGE(val) ((val & 0x00000001) << 0x00000004)
#define BF_FINE_DDC4_PAGE_GET(val) ((val >> 0x00000004) & 0x00000001)
#define BF_FINE_DDC5_PAGE_INFO 0x00000105
#define BF_FINE_DDC5_PAGE(val) ((val & 0x00000001) << 0x00000005)
#define BF_FINE_DDC5_PAGE_GET(val) ((val >> 0x00000005) & 0x00000001)
#define BF_FINE_DDC6_PAGE_INFO 0x00000106
#define BF_FINE_DDC6_PAGE(val) ((val & 0x00000001) << 0x00000006)
#define BF_FINE_DDC6_PAGE_GET(val) ((val >> 0x00000006) & 0x00000001)
#define BF_FINE_DDC7_PAGE_INFO 0x00000107
#define BF_FINE_DDC7_PAGE(val) ((val & 0x00000001) << 0x00000007)
#define BF_FINE_DDC7_PAGE_GET(val) ((val >> 0x00000007) & 0x00000001)

#define REG_JTX_PAGE_ADDR 0x0000001A
#define BF_JTX_LINK0_PAGE_INFO 0x00000100
#define BF_JTX_LINK0_PAGE(val) (val & 0x00000001)
#define BF_JTX_LINK0_PAGE_GET(val) (val & 0x00000001)
#define BF_JTX_LINK1_PAGE_INFO 0x00000101
#define BF_JTX_LINK1_PAGE(val) ((val & 0x00000001) << 0x00000001)
#define BF_JTX_LINK1_PAGE_GET(val) ((val >> 0x00000001) & 0x00000001)

#define REG_PFILT_CTL_PAGE_ADDR 0x0000001E
#define BF_PFILT_ADC_PAIR0_PAGE_INFO 0x00000100
#define BF_PFILT_ADC_PAIR0_PAGE(val) (val & 0x00000001)
#define BF_PFILT_ADC_PAIR0_PAGE_GET(val) (val & 0x00000001)
#define BF_PFILT_ADC_PAIR1_PAGE_INFO 0x00000101
#define BF_PFILT_ADC_PAIR1_PAGE(val) ((val & 0x00000001) << 0x00000001)
#define BF_PFILT_ADC_PAIR1_PAGE_GET(val) ((val >> 0x00000001) & 0x00000001)

#define REG_PFILT_COEFF_PAGE_ADDR 0x0000001F
#define BF_PFILT_COEFF_PAGE0_INFO 0x00000100
#define BF_PFILT_COEFF_PAGE0(val) (val & 0x00000001)
#define BF_PFILT_COEFF_PAGE0_GET(val) (val & 0x00000001)
#define BF_PFILT_COEFF_PAGE1_INFO 0x00000101
#define BF_PFILT_COEFF_PAGE1(val) ((val & 0x00000001) << 0x00000001)
#define BF_PFILT_COEFF_PAGE1_GET(val) ((val >> 0x00000001) & 0x00000001)
#define BF_PFILT_COEFF_PAGE2_INFO 0x00000102
#define BF_PFILT_COEFF_PAGE2(val) ((val & 0x00000001) << 0x00000002)
#define BF_PFILT_COEFF_PAGE2_GET(val) ((val >> 0x00000002) & 0x00000001)
#define BF_PFILT_COEFF_PAGE3_INFO 0x00000103
#define BF_PFILT_COEFF_PAGE3(val) ((val & 0x00000001) << 0x00000003)
#define BF_PFILT_COEFF_PAGE3_GET(val) ((val >> 0x00000003) & 0x00000001)

#endif /* __ADI_AD9081_BF_RX_PAGING_H__ */
/*! @} */