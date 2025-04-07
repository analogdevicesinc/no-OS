/**
 * \file
 * \brief Contains ADRV9001 related function prototypes for adi_adrv9001_radioctrl.c
 *
 *  ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_RADIO_H_
#define _ADI_ADRV9001_RADIO_H_

#include "adi_adrv9001_types.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_tx_types.h"
#include "adrv9001_init_types.h"
#include "adrv9001_radio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Minimum supported carrier frequency
 */
#define ADI_ADRV9001_CARRIER_FREQUENCY_MIN_HZ 25000000llu    /* 25 MHz */
/**
 * \brief Maximum supported carrier frequency
 */
#define ADI_ADRV9001_CARRIER_FREQUENCY_MAX_HZ 6000000000llu  /* 6 GHz */
/**
 * \brief Maximum supported intermediate frequency
 */
#define ADI_ADRV9001_INTERMEDIATE_FREQUENCY_MAX_HZ 20000000  /* 20MHz */
/**
 * \brief Sets the Carrier frequency for the given channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY or CALIBRATED
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] port      The port of interest
 * \param[in] channel   The channel of interest
 * \param[in] carrier   Desired carrier configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Carrier_Configure(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_Carrier_t *carrier);

/**
 * \brief Gets the Carrier frequency for the specified channel.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001	 Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  port      The port of interest
 * \param[in]  channel   The channel of interest
 * \param[out] carrier   Current carrier configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Radio_Carrier_Inspect(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv9001_Carrier_t *carrier);

/**
 * \brief Sets the parameters for the given pll
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre This function should be called before running init calibration when all the channels are in STANDBY state.
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pllId     The PLL of interest
 * \param[in] pllConfig Desired pll configuration parameter
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
    int32_t adi_adrv9001_Radio_Pll_Configure(adi_adrv9001_Device_t *adrv9001,
                                             adi_adrv9001_Pll_e pllId,
                                             adi_adrv9001_PllConfig_t *pllConfig);

/**
 * \brief Gets the parameters for the given pll
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pllId     The PLL of interest
 * \param[in] pllConfig Desired pll configuration parameter
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
    int32_t adi_adrv9001_Radio_Pll_Inspect(adi_adrv9001_Device_t *adrv9001,
                                           adi_adrv9001_Pll_e pllId,
                                           adi_adrv9001_PllConfig_t *pllConfig);
/**
 * \brief Check if the specified PLL is locked
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This may be called any time after the PLLs have been configured and are operational
 *
 * \param[in]  adrv9001	    Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  pll          The PLL of interest
 * \param[out] locked       Whether the PLL is locked (true)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_PllStatus_Get(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_Pll_e pll, bool *locked);

/**
 * \brief Configure how the specified channel is enabled
 *
 * The ADRV9001 defaults to SPI mode on power up. This function is used to reconfigure
 * the signal path control mode of Rx, ORx, and Tx signal chains. SPI mode control can
 * be accomplished through adi_adrv9001_ChannelEnableSet(). Pin mode control is through
 * dedicated input pins to the ADRV9001 device.
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function should be called after initialization and loading the stream
 * processor.
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[in] mode      The desired enable mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_ChannelEnableMode_Set(adi_adrv9001_Device_t *adrv9001,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_ChannelEnableMode_e mode);

/**
 * \brief Read the current enable mode for the specified channel
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function should be called after initialization and loading the stream
 * processor.
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[out] mode      The current enable mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_ChannelEnableMode_Get(adi_adrv9001_Device_t *adrv9001,
                                                 adi_common_Port_e port,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_ChannelEnableMode_e *mode);

/**
 * \brief Reads the current ARM radio state
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be used after the device has been initialized, ARM binary loaded and the
 *      PLLs configured.
 *
 * \param[in]  adrv9001  Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[out] radioState   The current radio state
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_State_Get(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_RadioState_t *radioState);

/**
 * \brief Reads the current channel state
 *
 * \note Message type: \ref timing_direct "Direct register access"
 *
 * \pre This function can be used after the device has been initialized, ARM binary loaded and the
 *      PLLs configured.
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  port             The port of the channel
 * \param[in]  channel          The channel number of the channel
 * \param[out] channelState     The current state of the channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_State_Get(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel,
                                             adi_adrv9001_ChannelState_e *channelState);

/**
 * \brief Prime the specified channel
 *
 * This function transitions the specified channel from the CALIBRATED state to the PRIMED state.
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to prime
 * \param[in] prime     Whether to prime (true) or unprime (false) the channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_Prime(adi_adrv9001_Device_t *adrv9001,
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
 * \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] ports         Set of port/channel combinations to prime
 * \param[in] channels      Set of port/channel combinations to prime
 * \param[in] length        Length of ports and channels arrays - the number of port/channel combinations to prime;
 *                          Each index i of ports and channels correspond to a single port/channel combination
 * \param[in] prime         Whether to prime (true) or unprime (false) the channels
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channels_Prime(adi_adrv9001_Device_t *adrv9001,
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
 *      See adi_adrv9001_ChannelEnableModeSet()
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port for which to enable RF operation
 * \param[in] enable    Whether to enable (true) or disable (false) RF for the channel
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_EnableRf(adi_adrv9001_Device_t *adrv9001,
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
 *      See adi_adrv9001_ChannelEnableModeSet()
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] ports     Set of port/channel combinations
 * \param[in] channels  Set of port/channel combinations for which to enable RF operation
 * \param[in] length    Length of ports and channels arrays - the number of port/channel combinations to enable;
 *                      Each index i of ports and channels correspond to a single port/channel combination
 * \param[in] enable    Whether to enable (true) or disable (false) RF for the channels
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channels_EnableRf(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_Port_e ports[],
                                             adi_common_ChannelNumber_e channels[],
                                             uint32_t length,
                                             bool enable);

/**
 * \brief Power down the specified channel, preserving init calibration information
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is CALIBRATED
 *
 * \note adi_adrv9001_Radio_State_Get will return a channelState value of CALIBRATED after powering down.
 * \note A powered down channel must first be powered up with adi_adrv9001_Radio_Channel_PowerUp before it can be
 *       primed (adi_adrv9001_Radio_Channel_Prime).
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      Port/channel combination to power down
 * \param[in] channel   Port/channel combination to power down
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_PowerDown(adi_adrv9001_Device_t *adrv9001,
                                             adi_common_Port_e port,
                                             adi_common_ChannelNumber_e channel);

/**
 * \brief Power down the specified channels, preserving init calibration information
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel states are CALIBRATED
 *
 * \note adi_adrv9001_Radio_State_Get will return a channelState value of CALIBRATED after powering down.
 * \note Powered down channels must first be powered up with adi_adrv9001_Radio_Channels_PowerUp before it can be
 *       primed (adi_adrv9001_Radio_Channels_Prime).
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] ports     Set of port/channel combinations to power down
 * \param[in] channels  Set of port/channel combinations to power down
 * \param[in] length    Length of ports and channels arrays - the number of port/channel combinations to power down;
 *                      Each index i of ports and channels correspond to a single port/channel combination
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channels_PowerDown(adi_adrv9001_Device_t *adrv9001,
                                              adi_common_Port_e ports[],
                                              adi_common_ChannelNumber_e channels[],
                                              uint32_t length);

/**
 * \brief Power up the specified channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \post Channel will be returned to the CALIBRATED state, preserving init calibration information
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      Port/channel combination to power up
 * \param[in] channel   Port/channel combination to power up
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_PowerUp(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel);

/**
 * \brief Power up the specified channels
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \post Channels will be returned to the CALIBRATED state, preserving init calibration information
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] ports     Set of port/channel combinations to power up
 * \param[in] channels  Set of port/channel combinations to power up
 * \param[in] length    Length of ports and channels arrays - the number of port/channel combinations to power down;
 *                      Each index i of ports and channels correspond to a single port/channel combination
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channels_PowerUp(adi_adrv9001_Device_t *adrv9001,
                                            adi_common_Port_e ports[],
                                            adi_common_ChannelNumber_e channels[],
                                            uint32_t length);


/**
 * \brief Transition the specified channel to the specified state
 *
 * This function will transition the specified channel to the specified state from any state where it is valid to do so.
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      The port that the channel refers to
 * \param[in] channel   The channel of the specified port to transition to the specified state
 * \param[in] state     The state to transition the specified channel to
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_Channel_ToState(adi_adrv9001_Device_t *adrv9001,
                                           adi_common_Port_e port,
                                           adi_common_ChannelNumber_e channel,
                                           adi_adrv9001_ChannelState_e state);

/**
 * \brief Configures the loop filter for the specified PLL
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] pll                   The PLL for which to configure the loop filter
 * \param[in] pllLoopFilterConfig   The desired loop filter configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_PllLoopFilter_Set(adi_adrv9001_Device_t *adrv9001,
                                             adi_adrv9001_Pll_e pll,
                                             adi_adrv9001_PllLoopFilterCfg_t *pllLoopFilterConfig);

/**
 * \brief Gets the loop filter configuration for the specified PLL
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] pll                   The PLL for which to configure the loop filter
 * \param[out] pllLoopFilterConfig   The current loop filter configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_PllLoopFilter_Get(adi_adrv9001_Device_t *adrv9001,
                                             adi_adrv9001_Pll_e pll,
                                             adi_adrv9001_PllLoopFilterCfg_t *pllLoopFilterConfig);

/**
 * \brief Convert a channel and port combination to the appropriate mailbox channel value
 *
 * \param[in] channel   The channel of the specified port
 * \param[in] port      The port that the channel refers to
 *
 * \returns The mailbox channel value corresponding to the channel/port combination
 */
uint8_t adi_adrv9001_Radio_MailboxChannel_Get(adi_common_Port_e port, adi_common_ChannelNumber_e channel);

/**
 * \brief Convert a set of port/channel combinations to the appropriate mailbox channel mask
 *
 * \param[in] ports         Set of ports to convert to a mask
 * \param[in] channels      Set of channels to convert to a mask
 * \param[in] length        Length of ports and channels arrays - the number of port/channel combinations to convert;
 *                          Each index i of ports and channels correspond to a single port/channel combination
 *
 * \returns The mailbox channel value corresponding to the channel/port combination
 */
uint8_t adi_adrv9001_Radio_MailboxChannelMask_Get(adi_common_Port_e ports[],
                                                  adi_common_ChannelNumber_e channels[],
                                                  uint32_t length);

/**
 * \brief Configure channel enable delays for the specified channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      The port of the channel
 * \param[in] channel   The channel number
 * \param[in] delays    The desired delay settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_ChannelEnablementDelays_Configure(adi_adrv9001_Device_t *adrv9001,
                                                             adi_common_Port_e port,
                                                             adi_common_ChannelNumber_e channel,
                                                             adi_adrv9001_ChannelEnablementDelays_t *delays);

/**
 * \brief Inspect channel enable delays for the specified channel
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of PRIMED, RF_ENABLED
 *
 * \param[in] adrv9001  Context variable - Pointer to the ADRV9001 device data structure containing settings
 * \param[in] port      The port of the channel
 * \param[in] channel   The channel number
 * \param[out] delays   The desired delay settings
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_ChannelEnablementDelays_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                           adi_common_Port_e port,
                                                           adi_common_ChannelNumber_e channel,
                                                           adi_adrv9001_ChannelEnablementDelays_t *delays);

/**
* \brief Set channels to MCS Ready substate
*
* This function transitions the all channels from the CALIBRATED state to the MCS
* ready sub state.
*
* \note Message type: \ref timing_mailbox "Mailbox command"
* \pre  All channels must be in CALIBRATED state before transitioning to MCS_READY
*
* \param[in] adrv9001      Context variable - Pointer to the ADRV9001 device data structure containing settings
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t adi_adrv9001_Radio_ToMcsReady(adi_adrv9001_Device_t *adrv9001);

/**
 * \brief Gets the Logen Divider for the specified PLL (RF1/RF2)
 *
 * The device contains two RF PLLs(RF1/RF2).
 * The RF LO frequency for each of the PLLs is derived by dividing down the
 * VCO output in the LOGEN block . The corresponding Logen Divider for each RF PLL
 * is calculated as : ((2^(RFx Logen divide mode + 1) * (RFx Logen divide ratio))
 *
 * \pre Channel state any of CALIBRATED, PRIMED or RF_ENABLED
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in]  pll              The PLL of interest among RF1 and RF2
 * \param[out] RfLogenDivider   The calculated value of Logen Divider for the specified PLL (RF1/RF2)
 *                              ranging from 0 to 130816(256*511)
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_RfLogenDivider_Get(adi_adrv9001_Device_t *adrv9001, adi_adrv9001_Pll_e pll, uint32_t *RfLogenDivider);

/**
 * \brief Write the TX/RX WB/NB PFIR channel filter Banks
 *
 * \note Used with TX/RX WB/NB compensation PFIR types only
 *
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] adrv9001	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pfirCoeff PFIR configuration to write
 * \param[in] bankSel   The PFIR Bank of interest
 * \param[in] port      The port of interest
 * \param[in] channel   The channel of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Radio_PfirWbNbCompChFilter_Set(adi_adrv9001_Device_t *adrv9001,
		                                            const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff,
		                                            adi_adrv9001_PfirBank_e bankSel, adi_common_Port_e port,
	                                                adi_common_ChannelNumber_e channel);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_RADIO_H_ */
