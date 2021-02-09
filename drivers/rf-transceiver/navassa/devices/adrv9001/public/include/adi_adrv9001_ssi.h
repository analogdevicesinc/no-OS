/**
 * \file
 * \brief Contains ADRV9001 receive related function prototypes for adi_adrv9001_ssi.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_SSI_H_
#define _ADI_ADRV9001_SSI_H_

#include "adi_adrv9001_ssi_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Configure the SSI test mode for the specified Rx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 * \note In CMOS mode, a 4-bit value is set as 'fixedDataPatternToTransmit' in adi_adrv9001_RxSsiTestModeCfg_t.
 *       This value is repeated over 16 bits for I and Q
 *
 * \param[in] adrv9001             Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel              The Rx channel for which to configure SSI test mode
 * \param[in] ssiType              The type (CMOS/LVDS) of SSI interface
 * \param[in] dataFormat            The data format (I-data or I/Q-data) of the SSI interface
 * \param[in] ssiTestModeConfig    The desired SSI test mode to configure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_Rx_TestMode_Configure(adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SsiType_e ssiType,
                                               adi_adrv9001_SsiDataFormat_e dataFormat,
                                               adi_adrv9001_RxSsiTestModeCfg_t *ssiTestModeConfig);

/**
 * \brief Configure the SSI test mode for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001             Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel              The Tx channel for which to configure SSI test mode
 * \param[in] ssiType              The type (CMOS/LVDS) of SSI interface
 * \param[in] dataFormat           The data format (I-data or I/Q-data) of the SSI interface
 * \param[in] ssiTestModeConfig    The desired SSI test mode to configure
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_Tx_TestMode_Configure(adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               adi_adrv9001_SsiType_e ssiType,
                                               adi_adrv9001_SsiDataFormat_e dataFormat,
                                               adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig);

/**
 * \brief Inspect the SSI test mode status for the specified Tx channel
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel               The Tx channel for which to inspect SSI test mode status
 * \param[in] ssiType               The type (CMOS/LVDS) of SSI interface
 * \param[in] dataFormat            The data format (I-data or I/Q-data) of the SSI interface
 * \param[in] ssiTestModeConfig     The SSI test mode configuration for which to check status
 * \param[out] ssiTestModeStatus    The current SSI test mode status
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_Tx_TestMode_Status_Inspect(adi_adrv9001_Device_t *adrv9001,
                                                    adi_common_ChannelNumber_e channel,
                                                    adi_adrv9001_SsiType_e ssiType,
                                                    adi_adrv9001_SsiDataFormat_e dataFormat,
                                                    adi_adrv9001_TxSsiTestModeCfg_t *ssiTestModeConfig,
                                                    adi_adrv9001_TxSsiTestModeStatus_t *ssiTestModeStatus);

/**
 * \brief Set the enabledness of Tx SSI to Rx SSI interface loopback
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] channel               The channel for which to set SSI interface loopback
 * \param[in] ssiType               The type (CMOS/LVDS) of SSI interface
 * \param[in] loopbackEnable        A boolean flag to enable or disable ADRV9001 Tx SSI to Rx SSI interface loopback
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_Loopback_Set(adi_adrv9001_Device_t *adrv9001,
                                      adi_common_ChannelNumber_e channel,
                                      adi_adrv9001_SsiType_e ssiType,
                                      bool loopbackEnable);

/**
 * \brief This function programs the SSI delay configuration in ADRV9001 device through SPI.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] ssiType           LVDS or CMOS mode
 * \param[in] ssiCalibration    The desired SSI calibration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */

int32_t adi_adrv9001_Ssi_Delay_Configure(adi_adrv9001_Device_t *adrv9001,
                                         adi_adrv9001_SsiType_e ssiType,
                                         adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration);

/**
 * \brief This function gets the SSI delay configuration from ADRV9001 device through SPI.
 *
 * \note Message type: \ref timing_direct "Direct register acccess"
 *
 * \param[in]  adrv9001         Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  ssiType          LVDS or CMOS mode
 * \param[out] ssiCalibration   The current SSI calibration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */

int32_t adi_adrv9001_Ssi_Delay_Inspect(adi_adrv9001_Device_t *adrv9001,
                                       adi_adrv9001_SsiType_e ssiType,
                                       adi_adrv9001_SsiCalibrationCfg_t *ssiCalibration);

/**
 * \brief Set the power down mode for the specified channel and SSI type
 * 
 * SSI pad power down enablement is valid only for LVDS. For CMOS, power down is always disabled 
 *
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state is STANDBY
 *
 * \param[in] adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in] port              The port that the channel refers to
 * \param[in] channel           The channel for which to set the power down mode
 * \param[in] powerDownMode     The desired power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_PowerDown_Set(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_Port_e port,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_SsiPowerDown_e powerDownMode);

/**
 * \brief Get the power down mode for the specified channel
 * 
 * \note Message type: \ref timing_mailbox "Mailbox command"
 *
 * \pre Channel state any of STANDBY, CALIBRATED, PRIMED, RF_ENABLED
 *
 * \param[in]  adrv9001          Context variable - Pointer to the ADRV9001 device data structure
 * \param[in]  port              The port that the channel refers to
 * \param[in]  channel           The channel for which to get the power down mode
 * \param[out] powerDownMode     The current power down mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_adrv9001_Ssi_PowerDown_Get(adi_adrv9001_Device_t *adrv9001,
                                       adi_common_Port_e port,
                                       adi_common_ChannelNumber_e channel,
                                       adi_adrv9001_SsiPowerDown_e *powerDownMode);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_SSI_H_ */
