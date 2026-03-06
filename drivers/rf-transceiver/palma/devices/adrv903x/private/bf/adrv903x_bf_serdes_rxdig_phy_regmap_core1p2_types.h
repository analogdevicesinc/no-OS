/** 
 * \file adrv903x_bf_serdes_rxdig_phy_regmap_core1p2_types.h Automatically generated file with generator ver
 * 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADRV903X_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2_TYPES_H_
#define _ADRV903X_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2_TYPES_H_

typedef enum adrv903x_BfSerdesRxdigPhyRegmapCore1p2ChanAddr
{
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_0_    =    0x48080000,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_1_    =    0x48080800,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_2_    =    0x48081000,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_3_    =    0x48081800,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_4_    =    0x48082000,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_5_    =    0x48082800,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_6_    =    0x48083000,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_7_    =    0x48083800,
    ADRV903X_BF_DIGITAL_CORE_JESD_DESERIALIZER_DESER_PHY_ALL    =    0x48084000
} adrv903x_BfSerdesRxdigPhyRegmapCore1p2ChanAddr_e;

/** 
 * \brief Enumeration for softreset
 */

typedef enum adrv903x_Bf_SerdesRxdigPhyRegmapCore1p2_Softreset
{
    ADRV903X_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2_SOFTRESET_SOFT_RESETB      =    0,  /*!< Deassert Soft Reset.    */
    ADRV903X_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2_SOFTRESET_SOFT_RESET       =    1   /*!< Assert Soft Reset.      */
} adrv903x_Bf_SerdesRxdigPhyRegmapCore1p2_Softreset_e;

#endif // _ADRV903X_BF_SERDES_RXDIG_PHY_REGMAP_CORE1P2_TYPES_H_

