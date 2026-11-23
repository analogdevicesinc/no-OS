/*!
 *
 * @file
 *
 * @brief
 *
 * @detail
 *
 */
/*******************************************************************************
  Copyright(c) 2019 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 ******************************************************************************/

#ifndef __DEVICE_PROFILE_PLL_RECONFIG_T_H__
#define __DEVICE_PROFILE_PLL_RECONFIG_T_H__

#include <stdint.h>
#include <stdbool.h>
#include "adi_device_profile_pack.h"
//#include "device_addr_def.h"

/********************************************************************
 * PLL RECONFIG STATUS REGISTERS
 ********************************************************************/
/* Absolute address */
#define ADDR_SPI_TX1_PLL_RECONFIG_STATUS_REGISTER    ADDR_SPI_TX1_CORE_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1
#define ADDR_SPI_TX2_PLL_RECONFIG_STATUS_REGISTER    ADDR_SPI_TX2_CORE_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1
#define ADDR_SPI_RX1_PLL_RECONFIG_STATUS_REGISTER    ADDR_SPI_RXB1_CORE_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1
#define ADDR_SPI_RX2_PLL_RECONFIG_STATUS_REGISTER    ADDR_SPI_RXB2_CORE_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1
/* Bitfield Definitions */
#define BITP_NVS_REGMAP_PLL_RECONFIG_STATUS          0x00u /* 0: no dynamic reconfiguration, 1: dynamic reconfiguration, 2: no dynamic reconfiguration but wait for LO lock */
#define BITM_NVS_REGMAP_PLL_RECONFIG_STATUS          0x03u
#define BITP_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING      0x01u /* 0: LO 1, 1: LO2 */
#define BITM_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING      0x02u
/*  BF definitions */
/* INST_SPI_TX1_CORE    (just the first instance) */
#define WRITE_NVS_REGMAP_TX_PLL_RECONFIG_STATUS_BIT(base, val) WRITE_BF_8BIT_REG(\
    pREG_NVS_REGMAP_TX_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    BITM_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    (val))
/* INST_SPI_TX1_CORE    (just the first instance) */
#define READ_NVS_REGMAP_TX_PLL_RECONFIG_STATUS_BIT(base) READ_BF_8BIT_REG(\
    pREG_NVS_REGMAP_TX_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    BITM_NVS_REGMAP_PLL_RECONFIG_STATUS)
/* INST_SPI_RXB1_CORE    (just the first instance) */
#define WRITE_NVS_REGMAP_RX_PLL_RECONFIG_STATUS_BIT(base, val) WRITE_BF_8BIT_REG(\
    pREG_RXB_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    BITM_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    (val))
/* INST_SPI_RXB1_CORE    (just the first instance) */
#define READ_NVS_REGMAP_RX_PLL_RECONFIG_STATUS_BIT(base) READ_BF_8BIT_REG(\
    pREG_RXB_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_STATUS, \
    BITM_NVS_REGMAP_PLL_RECONFIG_STATUS)

/* INST_SPI_TX1_CORE    (just the first instance) */
#define WRITE_NVS_REGMAP_TX_LO_MAPPING_BIT(base, val) WRITE_BF_8BIT_REG(\
    pREG_NVS_REGMAP_TX_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    BITM_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    (val))
/* INST_SPI_TX1_CORE    (just the first instance) */
#define READ_NVS_REGMAP_TX_LO_MAPPING_BIT(base) READ_BF_8BIT_REG(\
    pREG_NVS_REGMAP_TX_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    BITM_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING)
/* INST_SPI_RXB1_CORE    (just the first instance) */
#define WRITE_NVS_REGMAP_RX_LO_MAPPING_BIT(base, val) WRITE_BF_8BIT_REG(\
    pREG_RXB_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    BITM_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    (val))
/* INST_SPI_RXB1_CORE    (just the first instance) */
#define READ_NVS_REGMAP_RX_LO_MAPPING_BIT(base) READ_BF_8BIT_REG(\
    pREG_RXB_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_1(base), \
    BITP_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING, \
    BITM_NVS_REGMAP_PLL_RECONFIG_LO_MAPPING)

#endif /* __DEVICE_PROFILE_PLL_RECONFIG_T_H__ */
