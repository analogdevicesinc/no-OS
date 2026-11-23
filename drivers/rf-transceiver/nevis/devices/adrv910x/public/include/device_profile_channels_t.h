/*!
 * @brief   channel enum module header
 *
 * @details  This file provides definitions for channel specific objects
 *
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software
  is proprietary & confidential to Analog Devices, Inc. and its licensors. By
  using this software you agree to the terms of the associated Analog Devices
  License Agreement.
 ******************************************************************************/


#ifndef __DEVICE_PROFILE_CHANNELS_T_HEADER__
#define __DEVICE_PROFILE_CHANNELS_T_HEADER__

#include <stdint.h>
#include <stdbool.h>
#include "adi_device_profile_pack.h"

/*!
 *  @addtogroup channels
 *  @ingroup system
 *  This is the channel module that provides data struct and functions for
 *  implementing channel specific features and functions.
 *  @{
 */

/*! Channel Index Enum
  Usage examples:
  1. Loop for all channels:
      for (chId = CHANNEL_ID_START; chId <= CHANNEL_ID_END; chId++)
      {...}

  2. Loop for all TX channels:
      for (chId = CHANNEL_ID_TX_START; chId <= CHANNEL_ID_TX_END; chId++)
      {...}
*/
typedef enum
{

    CHANNEL_ID_RX1      = 0u,  /*!< ID for RX1                   */
    CHANNEL_ID_START    = 0u,  /*!< First ID for all channels    */
    CHANNEL_ID_RX_START = 0u,  /*!< First ID for all RX channels */
    
    CHANNEL_ID_RXNB      = 1u,  /*!< ID for RX NB                */
    CHANNEL_ID_RX_END   = 1u,  /*!< Last ID for all RX channels  */

    CHANNEL_ID_TX1      = 2u,  /*!< ID for TX1                   */
    CHANNEL_ID_TX_START = 2u,  /*!< First ID for all TX channels */
    
    CHANNEL_ID_TXNB      = 3u,  /*!< ID for TX NB                */
    CHANNEL_ID_TX_END   = 3u,  /*!< Last ID for all TX channels  */

    CHANNEL_ID_END      = 3u,  /*!< LAST ID for all channels     */

    MAX_NUM_CHANNELS    = 4u,  /*!< Number of channels           */

} ADI_NEVIS_PACK_ENUM channelId_e;

#define IS_RX_CHANNEL_ID(chanId) ((chanId <= CHANNEL_ID_RX_END))
#define IS_TX_CHANNEL_ID(chanId) ((chanId >= CHANNEL_ID_TX_START) && (chanId <= CHANNEL_ID_TX_END))
#define IS_VALID_CHANNEL_ID(chanId) ((chanId >= CHANNEL_ID_START) && (chanId <= CHANNEL_ID_END))

/*! Number of RX channels */
#define NUM_RX_CHANNELS (CHANNEL_ID_RX_END - CHANNEL_ID_RX_START + 1u)
/*! Number of TX channels */
#define NUM_TX_CHANNELS (CHANNEL_ID_TX_END - CHANNEL_ID_TX_START + 1u)
#define NUM_CHANNELS ((NUM_TX_CHANNELS > NUM_RX_CHANNELS) ? NUM_TX_CHANNELS : NUM_RX_CHANNELS)

/*! LO ID Enum */
typedef enum {
    LO_ID_START    = 0u,   /*!< First ID for all LOs         */

    LO_ID_LO1      = 0u,   /*!< ID for Internal LO1          */
    LO_ID_LO2      = 1u,   /*!< ID for Internal LO2          */
    LO_ID_LO1_EXT  = 2u,   /*!< ID for External LO1          */
    LO_ID_LO2_EXT  = 3u,   /*!< ID for External LO2          */

    LO_ID_END      = 3u,   /*!< Last ID for all LOs          */
    MAX_NUM_LOS    = 4u,   /*!< Number of LOs                */
} ADI_NEVIS_PACK_ENUM LoId_e;

#define IS_VALID_LO_ID(loId) ((loId >= LO_ID_START) && (loId <= LO_ID_END))
#define IS_INT_LO_ID(loId) ((loId >= LO_ID_LO1) && (loId <= LO_ID_LO2))
#define IS_EXT_LO_ID(loId) ((loId >= LO_ID_LO1_EXT) && (loId <= LO_ID_LO2_EXT))
#define NUM_LOS 2 /* used in FW */
#define NUM_EXT_LOS 2

/*! Enumeration for channel numbers - DO NOT CHANGE VALUE OR ORDER !!! */
typedef enum
{
    CH_1 = 0u,         /*!< Tx/Rx channel # 1 index   */
    CH_2 = 1u,         /*!< Tx/Rx channel # 2 index   */
    NUM_CHANNEL_NUMBERS = 2u,
} ADI_NEVIS_PACK_ENUM ChannelNum_e;

#define GET_RX_CHAN_ID_FROM_CHAN_NUM(chanNum) ((channelId_e)((uint8_t)chanNum + (uint8_t)CHANNEL_ID_RX_START ))
#define GET_TX_CHAN_ID_FROM_CHAN_NUM(chanNum) ((channelId_e)((uint8_t)chanNum + (uint8_t)CHANNEL_ID_TX_START ))

#define GET_CHAN_NUM_FROM_CHAN_ID(chanId)     ((uint8_t)((uint8_t)chanId % (uint8_t)CHANNEL_ID_TX_START))

/*! Channel bit mask for RX1 */
#define CHANNEL_BITM_RX1 (1u << CHANNEL_ID_RX1)
/*! Channel bit mask for RXNB */
#define CHANNEL_BITM_RXNB (1u << CHANNEL_ID_RXNB)
/*! Channel bit mask for TX1 */
#define CHANNEL_BITM_TX1 (1u << CHANNEL_ID_TX1)
/*! Channel bit mask for TXNB */
#define CHANNEL_BITM_TXNB (1u << CHANNEL_ID_TXNB)

/*! Channel bit mask for all channels */
#define CHANNEL_BITM_RX1TX1 (CHANNEL_BITM_RX1         \
                             | CHANNEL_BITM_TX1 )

/*! Channel bit mask for all channels */
#define CHANNEL_BITM_RX2TX2 (CHANNEL_BITM_RXNB         \
                             | CHANNEL_BITM_TXNB )

/*! Channel bit mask for all RX channels */
#define CHANNEL_BITM_ALL_RX_CHANNELS (CHANNEL_BITM_RX1      \
                                      | CHANNEL_BITM_RXNB )
/*! Channel bit mask for all TX channels */
#define CHANNEL_BITM_ALL_TX_CHANNELS (CHANNEL_BITM_TX1      \
                                      | CHANNEL_BITM_TXNB )
/*! Channel bit mask for all channels */
#define CHANNEL_BITM_ALL_CHANNELS (CHANNEL_BITM_RX1         \
                                   | CHANNEL_BITM_RXNB       \
                                   | CHANNEL_BITM_TX1       \
                                   | CHANNEL_BITM_TXNB )


/* Test if the channel is enabled (NOT masked off) */
#define IS_CHANNEL_ENABLED(chanId, chanMask) (((chanMask) & ((1u)<<(chanId))) != 0u)

#define IS_ANY_TX_CHANNEL_ENABLED(chanMask) ((chanMask & CHANNEL_BITM_ALL_TX_CHANNELS) != 0)
#define IS_ANY_RX_CHANNEL_ENABLED(chanMask) ((chanMask & CHANNEL_BITM_ALL_RX_CHANNELS) != 0)
/* Test if the channel is valid by comparing to the channel mask */
#define IS_CHANNEL_VALID IS_CHANNEL_ENABLED

/*! Channel Profile Type

  Usage examples:
  1. Loop for all profiles:
      for (chanProfId = CHANNEL_PROFILE_START; chanProfId <= CHANNEL_PROFILE_END; chanProfId++)
      {...}

  2. Loop for all RX profiles:
      for (rxProfId = CHANNEL_PROFILE_RX_START; rxProfId <= CHANNEL_PROFILE_RX_END; rxProfId++)
      {...}
 */
typedef enum
{
  
    CHANNEL_PROFILE_RX       = 0u,  /*!< Normal RX path with high power ADC */
    CHANNEL_PROFILE_START    = 0u,  /*!< First ID of all the profiles       */
    CHANNEL_PROFILE_RX_START = 0u,  /*!< First ID of all the RX profiles    */
    CHANNEL_PROFILE_ORX      = 1u,  /*!< Observe RX path                    */
    CHANNEL_PROFILE_ILB      = 2u,  /*!< RX in internal loopback path       */
    CHANNEL_PROFILE_ELB      = 3u,  /*!< RX in external loopback path       */
    CHANNEL_PROFILE_RX_END   = 4u,  /*!< Last ID of all the RX profiles     */
    
    CHANNEL_PROFILE_TX       = 5u,  /*!< Normal TX path                     */
    CHANNEL_PROFILE_TX_START = 5u,  /*!< First ID of all the TX profiles    */
    CHANNEL_PROFILE_TX_END   = 5u,  /*!< Last ID of all the TX profiles     */

    CHANNEL_PROFILE_END      = 5u,  /*!< Last ID of all the profiles        */
    NUM_CHANNEL_PROFILE      = 6u,  /*!< Number of channel profiles         */
    CHANNEL_PROFILE_NA       = -1 /*!< Channel Profile Not Set or Not Relavent */
} ADI_NEVIS_PACK_ENUM channelProfileId_e;


#define NUM_RX_CHANNEL_PROFILES (CHANNEL_PROFILE_RX_END - CHANNEL_PROFILE_RX_START + 1u)
#define NUM_TX_CHANNEL_PROFILES (CHANNEL_PROFILE_TX_END - CHANNEL_PROFILE_TX_START + 1u)

/*! Channel profile bit mask for RX */
#define PROFILE_BITM_RX (1u << CHANNEL_PROFILE_RX)
/*! Channel profile bit mask for ORX */
#define PROFILE_BITM_ORX (1u << CHANNEL_PROFILE_ORX)
/*! Channel profile bit mask for ILB */
#define PROFILE_BITM_ILB (1u << CHANNEL_PROFILE_ILB)
/*! Channel profile bit mask for ELB */
#define PROFILE_BITM_ELB (1u << CHANNEL_PROFILE_ELB)
/*! Channel profile bit mask for TX */
#define PROFILE_BITM_TX   (1u << CHANNEL_PROFILE_TX)

/*! RX Profile ID bit mask */
#define PROFILE_BITM_RXDP    ( PROFILE_BITM_RX  \
                             | PROFILE_BITM_ORX \
                             | PROFILE_BITM_ILB \
                             | PROFILE_BITM_ELB )
/*! TX Profile ID bit mask - Add OTX here if ever used */
#define PROFILE_BITM_TXDP    ( CHANNEL_PROFILE_TX)

/*! Loopback Profile ID bit mask */
#define PROFILE_BITM_LBDP    ( PROFILE_BITM_ORX \
                             | PROFILE_BITM_ILB \
                             | PROFILE_BITM_ELB )
 
/* Test if the channel is valid by comparing to the channel mask */
#define IS_RX_DP_PROFILE(profId) (((PROFILE_BITM_RXDP) & ((1u)<<(profId))) != 0u)
#define IS_TX_DP_PROFILE(profId) (((PROFILE_BITM_TXDP) & ((1u)<<(profId))) != 0u)

/* The number of recent frames for which the framework keeps RF_ON duration. */
#define MEASURE_RFON_DUR_NUM_FRAMES 3u

#define DEFAULT_RX_SSI_ENABLE_MASK (PROFILE_BITM_RX  \
                                  | PROFILE_BITM_ORX )
#define DEFAULT_TX_SSI_ENABLE_MASK (PROFILE_BITM_TX)


/*! @} */

#endif /* __DEVICE_PROFILE_CHANNELS_T_HEADER__ */
