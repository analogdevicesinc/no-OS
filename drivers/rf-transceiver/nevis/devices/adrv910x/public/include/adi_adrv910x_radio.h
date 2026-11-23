/**
 * \file
 * \brief Contains ADRV910X related function prototypes for adi_adrv910x_radio.c
 *
 *  ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910X_RADIO_H_
#define _ADI_ADRV910X_RADIO_H_

#include "adi_adrv910x_types.h"
#include "adi_adrv910x_common_types.h"
#include "adi_adrv910x_radio_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Minimum supported carrier frequency 
 */
#define ADI_ADRV910X_CARRIER_FREQUENCY_MIN_HZ 25000000llu    /* 25 MHz */
/**
 * \brief Maximum supported carrier frequency
 */
#define ADI_ADRV910X_CARRIER_FREQUENCY_MAX_HZ 6000000000llu  /* 6 GHz */
/**
 * \brief Maximum supported intermediate frequency
 */
#define ADI_ADRV910X_INTERMEDIATE_FREQUENCY_MAX_HZ 20000000  /* 20MHz */

/**
 * \brief Minimum supported external VCO PLL reference clock divider
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_REF_DIV_MIN 0
/**
 * \brief Maximum supported external VCO PLL reference clock divider
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_REF_DIV_MAX 4
		
/**
 * \brief Minimum supported external VCO PLL charge pump current
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MIN 0    /* 0.1mA */
/**
 * \brief Maximum supported external VCO PLL charge pump current
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_CHARGE_PUMP_MAX 63   /* 6.4mA */

/**
 * \brief Minimum supported external VCO PLL settle time(cycles)
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_SETTLE_TIME_MIN 0    /* 256 cycles */
/**
 * \brief Maximum supported external VCO PLL settle time(cycles)
 */
#define ADI_ADRV910X_CARRIER_EXTVCO_PLL_SETTLE_TIME_MAX 10   /* 1024 cycles */
/**
 * \brief Reads the current ARM radio state
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be used after the device has been initialized, ARM binary loaded and the
 *      PLLs configured.
 *
 * \param[in]  adrv910x  Context variable - Pointer to the ADRV910X device settings data structure
 * \param[out] radioState   The current radio state
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_State_Get(adi_adrv910x_Device_t *adrv910x, adi_adrv910x_RadioState_t *radioState);

/**
 * \brief Reads the current channel state 
 * 
 * \note Message type: \ref timing_direct "Direct register access"
 * 
 * \pre This function can be used after the device has been initialized, ARM binary loaded and the
 *      PLLs configured.
 * 
 * \param[in]  adrv910x      Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  port          The port that the channel refers to
 * \param[in]  channel       The channel number of the channel
 * \param[out] channelState  The current state of the channel
 * 
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Channel_State_Get(adi_adrv910x_Device_t *adrv910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            adi_adrv910x_ChannelState_e *channelState);
	
/**
 * \brief Convert a channel and port combination to the appropriate mailbox channel value
 *
 * \param[in] channel   The channel of the specified port
 * \param[in] port      The port that the channel refers to
 *
 * \returns The mailbox channel value corresponding to the channel/port combination
 */
uint8_t adi_adrv910x_Radio_MailboxChannel_Get(adi_common_Port_e port, adi_common_ChannelNumber_e channel);
	
/**
 * \brief Sets the Carrier frequency for the given channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY or CALIBRATED
 *
 * \param[in] adrv910x	Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] port      The port of interest
 * \param[in] channel   The channel of interest
 * \param[in] carrier   Desired carrier configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Carrier_Configure(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv910x_Carrier_t *carrier);

/**
 * \brief Gets the Carrier frequency for the specified channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv910x	 Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in]  port      The port of interest
 * \param[in]  channel   The channel of interest
 * \param[out] carrier   Current carrier configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv910x_Radio_Carrier_Inspect(adi_adrv910x_Device_t *adrv910x,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv910x_Carrier_t *carrier);
	
/**
 * \brief Configure how the specified channel is enabled
 *
 * The ADRV910X defaults to SPI mode on power up. This function is used to reconfigure
 * the signal path control mode of Rx, ORx, and Tx signal chains. SPI mode control can
 * be accomplished through adi_adrv910x_ChannelEnableSet(). Pin mode control is through
 * dedicated input pins to the ADRV910X device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function should be called after initialization and loading the stream
 * processor.
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[in] mode      The desired enable mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_ChannelEnableMode_Set(adi_adrv910x_Device_t *adrv910x,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv910x_ChannelEnableMode_e mode);

/**
 * \brief Read the current enable mode for the specified channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function should be called after initialization and loading the stream
 * processor.
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device settings data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[out] mode      The current enable mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_ChannelEnableMode_Get(adi_adrv910x_Device_t *adrv910x,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv910x_ChannelEnableMode_e *mode);
	
/**
 * \brief Prime the specified channel
 *
 * This function transitions the specified channel from the CALIBRATED state to the PRIMED state.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[in] prime     Whether to prime (true) or unprime (false) the channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Channel_Prime(adi_adrv910x_Device_t *adrv910x,
                                         adi_common_Port_e port,
                                         adi_common_ChannelNumber_e channel,
                                         bool prime);
	
/**
 * \brief Prime the specified channels
 *
 * This function transitions the specified channels from the CALIBRATED state to the PRIMED state.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv910x      Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] ports         Set of port/channel combinations to prime
 * \param[in] channels      Set of port/channel combinations to prime
 * \param[in] length        Length of ports and channels arrays - the number of port/channel combinations to prime;
 *                          Each index i of ports and channels correspond to a single port/channel combination
 * \param[in] prime         Whether to prime (true) or unprime (false) the channels
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Channels_Prime(adi_adrv910x_Device_t *adrv910x,
                                          adi_common_Port_e ports[],
                                          adi_common_ChannelNumber_e channels[],
                                          uint32_t length,
                                          bool prime);

/**
 * \brief Enable RF operation for the specified channel if in SPI enable mode
 *
 * This function transitions the specified channel from the PRIMED state to the RF ENABLED state.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre For this function to have any effect, the channel must be in SPI enable mode.
 *      See adi_adrv910x_ChannelEnableModeSet()
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port for which to enable RF operation
 * \param[in] enable    Whether to enable (true) or disable (false) RF for the channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Channel_EnableRf(adi_adrv910x_Device_t *adrv910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            bool enable);
	
/**
 * \brief Enable RF operation for the specified channels if in SPI enable mode
 *
 * This function transitions the specified channels from the PRIMED state to the RF ENABLED state.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre For this function to have any effect, the channel must be in SPI enable mode.
 *      See adi_adrv910x_ChannelEnableModeSet()
 *
 * \param[in] adrv910x  Context variable - Pointer to the ADRV910X device data structure containing settings
 * \param[in] ports     Set of port/channel combinations
 * \param[in] channels  Set of port/channel combinations for which to enable RF operation
 * \param[in] length    Length of ports and channels arrays - the number of port/channel combinations to enable;
 *                      Each index i of ports and channels correspond to a single port/channel combination
 * \param[in] enable    Whether to enable (true) or disable (false) RF for the channels
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv910x_Radio_Channels_EnableRf(adi_adrv910x_Device_t *adrv910x,
                                             adi_common_Port_e ports[],
                                             adi_common_ChannelNumber_e channels[],
                                             uint32_t length,
                                             bool enable);
		
/**
* \brief Configures the loop filter for the specified PLL
*
* \note Message type: \ref timing_mailbox "Mailbox command"
*
* \pre Channel state is STANDBY
*
* \param[in] adrv910x              Context variable - Pointer to the adrv910x device data structure containing settings
* \param[in] pll                   The PLL for which to configure the loop filter
* \param[in] pllLoopFilterConfig   The desired loop filter configuration
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv910x_Radio_PllLoopFilter_Set(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig);

/**
* \brief Gets the loop filter configuration for the specified PLL
*
* \note Message type: \ref timing_mailbox "Mailbox command"
*
* \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
*
* \param[in] adrv910x              Context variable - Pointer to the adrv910x device data structure containing settings
* \param[in] pll                   The PLL for which to configure the loop filter
* \param[out] pllLoopFilterConfig   The current loop filter configuration
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv910x_Radio_PllLoopFilter_Get(adi_adrv910x_Device_t *adrv910x,
	adi_adrv910x_Pll_e pll,
	adi_adrv910x_PllLoopFilterCfg_t *pllLoopFilterConfig);
	
	
#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910X_RADIO_H_ */
