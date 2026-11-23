/*! ****************************************************************************
 *
 * @file:    device_profile_pll_common_t.h
 *
 * @brief:   Common PLL def'n
 *
 * @details: This file contains common PLL type def'n
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_PLL_COMMON_T_H__
#define __DEVICE_PROFILE_PLL_COMMON_T_H__

#include "adi_device_profile_pack.h"
#include "device_profile_loop_filter_t.h"
#include "device_profile_phase_sync_t.h"
#include "device_profile_pll_reconfig_t.h"

#define PLL_LOCK_WAIT_TIME_US						(750u)
#define CLK_PLL_MODULUS                             (8388593u)      /* old value: 8386560u */

/* Enumeration for PLLs */
typedef enum
{
    PLL_LO1_PLL     = 0u,                           /*!< Ref for the LO1 Internal PLL. Always keep it at index 0 */
    PLL_LO2_PLL     = 1u,                           /*!< Ref for the LO2 Internal PLL    */    
    PLL_NUM_INT_PLL = 2u,                           /*!< Number of Internal  RF PLLs for algorithm */
    PLL_LO1_PLL_EXT = 2u,                           /*!< Ref for the ext LO1/VCO1*/
    PLL_LO2_PLL_EXT = 3u,                           /*!< Ref for the ext LO2/VCO2*/  
    PLL_NUM_RFPLL   = 4u,      
    PLL_CLKGEN_PLL  = 4u,                           /*!< Ref for the Clock Gen PLL */
    PLL_LAST_PLL    = 5u,                           /*!< Ref for last COMMON PLL   */
    NUM_PLLS        = 5u,
    PLL_NA          = 0xFFu,                 /*!< PLL not active or not relevant */
} ADI_NEVIS_PACK_ENUM pllId_e;

/*! PllSelName_e is defined as an alias of pllId_e to be compatible with Tokelau code */
typedef pllId_e     PllSelName_e;

/*! PLL bit mask definition */
#define PLL_BITM_RFPLL1     (1u << PLL_LO1_PLL)     /*!< bit mask for RFPLL 1 */
#define PLL_BITM_RFPLL2     (1u << PLL_LO2_PLL)     /*!< bit mask for RFPLL 2 */
#define PLL_BITM_ALL        (PLL_BITM_RFPLL1 | PLL_BITM_RFPLL2)

#define REGISTER_WRITE_MODE_SPI     (0u)            /*!< Write SPI register with SPI */
#define REGISTER_WRITE_MODE_DMA     (1u)            /*!< Write SPI register with DMA */
#define REGISTER_WRITE_MODE_SP      (2u)            /*!< Write SPI register with stream processor */

#define REGISTER_WRITE_MODE         REGISTER_WRITE_MODE_DMA

#define PLL_RX1_CHAN_FLAG             (0x1U)
#define PLL_TX1_CHAN_FLAG             (0x4U)
#define PLL_ORX1_CHAN_FLAG            (0x10U)
#define PLL_ILB1_CHAN_FLAG            (0x40U)

typedef enum
{
    CAL_TONE_MUX,                                   /*!< Calibration tone mux */
    MIXER_MUX,                                      /*!< Mixer mux */
} ADI_NEVIS_PACK_ENUM destinationMux_e;

typedef enum
{
    VCO_CAL_MODE    = 0x00,                 /*!< PLL VCO calibration needed (either Normal or Fast)  */
    NO_VCO_CAL_MODE = 0x01,                 /*!< PLL VCO calibration NOT needed */
} ADI_NEVIS_PACK_ENUM VcoCalMode_e;

typedef enum
{
    IQ,
    FM_IQ,
    FM_DM,
} ADI_NEVIS_PACK_ENUM signalType_e;

typedef enum
{
    LO_DIV_MODE_HP_BEST_PHASE_NOISE,        /*!< High performance mode (best phase noise)  */
    LO_DIV_MODE_LP_BEST_POWER_SAVING,       /*!< Low power mode (best power saving) */
    LO_DIV_MODE_POWER_OF_TWO,               /*!< Power of 2 for TX frequency/phase sync in A0 */
} ADI_NEVIS_PACK_ENUM dividerMode_e;

typedef enum
{
    COMPONENT_POWER_LEVEL_LOW,                  /*!< Low power */
    COMPONENT_POWER_LEVEL_MEDIUM,               /*!< Medium power */
    COMPONENT_POWER_LEVEL_HIGH,                 /*!< High power */
} ADI_NEVIS_PACK_ENUM componentPowerLevel_e;

/* PLL modulus settings */
ADI_NEVIS_PACK_START
typedef struct {
    uint32_t      modulus[NUM_PLLS];        /*!< PLL moduli */
    uint32_t      dmModulus[PLL_NUM_RFPLL]; /*!< RF PLL moduli in DM mode */
} pllModulus_t;
ADI_NEVIS_PACK_FINISH

#endif /* __DEVICE_PROFILE_PLL_COMMON_T_H__ */
