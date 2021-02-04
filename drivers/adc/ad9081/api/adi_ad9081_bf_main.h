/*!
 * @brief     SPI Register Definition Header File, automatically generated file at 1/20/2020 6:24:26 AM.
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
#ifndef __ADI_AD9081_BF_MAIN_H__
#define __ADI_AD9081_BF_MAIN_H__

/*============= I N C L U D E S ============*/
#include "adi_ad9081_config.h"

/*============= D E F I N E S ==============*/
#define REG_DEVICE_CONFIG_ADDR 0x00000002
#define BF_OP_MODE_INFO 0x00000200
#define BF_OP_MODE(val) (val & 0x00000003)
#define BF_OP_MODE_GET(val) (val & 0x00000003)
#define BF_CUST_OP_MODE_INFO 0x00000202
#define BF_CUST_OP_MODE(val) ((val & 0x00000003) << 0x00000002)
#define BF_CUST_OP_MODE_GET(val) ((val >> 0x00000002) & 0x00000003)
#define BF_DEVICE_STATUS_INFO 0x00000404
#define BF_DEVICE_STATUS(val) ((val & 0x0000000F) << 0x00000004)
#define BF_DEVICE_STATUS_GET(val) ((val >> 0x00000004) & 0x0000000F)

#define REG_CHIP_TYPE_ADDR 0x00000003
#define BF_CHIP_TYPE_INFO 0x00000800
#define BF_CHIP_TYPE(val) (val & 0x000000FF)
#define BF_CHIP_TYPE_GET(val) (val & 0x000000FF)

#define REG_PROD_ID_LSB_ADDR 0x00000004
#define BF_PROD_ID_INFO 0x00001000
#define BF_PROD_ID(val) (val & 0x0000FFFF)
#define BF_PROD_ID_GET(val) (val & 0x0000FFFF)

#define REG_PROD_ID_MSB_ADDR 0x00000005

#define REG_CHIP_GRADE_ADDR 0x00000006
#define BF_CHIP_GRADE_INFO 0x00000800
#define BF_CHIP_GRADE(val) (val & 0x000000FF)
#define BF_CHIP_GRADE_GET(val) (val & 0x000000FF)

#define REG_DEVICE_INDEX1_ADDR 0x00000008
#define BF_DEV_INDEX1_INFO 0x00000800
#define BF_DEV_INDEX1(val) (val & 0x000000FF)
#define BF_DEV_INDEX1_GET(val) (val & 0x000000FF)

#define REG_DEVICE_INDEX2_ADDR 0x00000009
#define BF_DEV_INDEX2_INFO 0x00000800
#define BF_DEV_INDEX2(val) (val & 0x000000FF)
#define BF_DEV_INDEX2_GET(val) (val & 0x000000FF)

#define REG_CHIP_SCRATCH_ADDR 0x0000000A
#define BF_CHIP_SCRATCH_INFO 0x00000800
#define BF_CHIP_SCRATCH(val) (val & 0x000000FF)
#define BF_CHIP_SCRATCH_GET(val) (val & 0x000000FF)

#define REG_SPI_REVISION_ADDR 0x0000000B
#define BF_SPI_REVISION_INFO 0x00000800
#define BF_SPI_REVISION(val) (val & 0x000000FF)
#define BF_SPI_REVISION_GET(val) (val & 0x000000FF)

#define REG_VENDOR_ID_LSB_ADDR 0x0000000C
#define BF_CHIP_VENDOR_ID_INFO 0x00001000
#define BF_CHIP_VENDOR_ID(val) (val & 0x0000FFFF)
#define BF_CHIP_VENDOR_ID_GET(val) (val & 0x0000FFFF)

#define REG_VENDOR_ID_MSB_ADDR 0x0000000D

#define REG_CHIP_ID_L_ADDR 0x00000010
#define BF_CHIP_ID_L_INFO 0x00000800
#define BF_CHIP_ID_L(val) (val & 0x000000FF)
#define BF_CHIP_ID_L_GET(val) (val & 0x000000FF)

#define REG_CHIP_ID_M1_ADDR 0x00000011
#define BF_CHIP_ID_M1_INFO 0x00000800
#define BF_CHIP_ID_M1(val) (val & 0x000000FF)
#define BF_CHIP_ID_M1_GET(val) (val & 0x000000FF)

#define REG_CHIP_ID_M2_ADDR 0x00000012
#define BF_CHIP_ID_M2_INFO 0x00000800
#define BF_CHIP_ID_M2(val) (val & 0x000000FF)
#define BF_CHIP_ID_M2_GET(val) (val & 0x000000FF)

#define REG_CHIP_ID_H_ADDR 0x00000013
#define BF_CHIP_ID_H_INFO 0x00000800
#define BF_CHIP_ID_H(val) (val & 0x000000FF)
#define BF_CHIP_ID_H_GET(val) (val & 0x000000FF)

#endif /* __ADI_AD9081_BF_MAIN_H__ */
/*! @} */