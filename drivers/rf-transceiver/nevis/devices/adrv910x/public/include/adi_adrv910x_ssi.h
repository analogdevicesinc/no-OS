/**
 * \file
 * \brief Contains ADRV9001 receive related function prototypes for adi_adrv910x_ssi.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV910x_SSI_H_
#define _ADI_ADRV910x_SSI_H_

#include "adi_adrv910x_ssi_types.h"
#include "adi_common_error_types.h"

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * \brief Configure the SSI test mode for the specified Rx channel
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 * \note In CMOS mode, a 4-bit value is set as 'fixedDataPatternToTransmit' in adi_adrv910x_RxSsiTestModeCfg_t.
	 *       This value is repeated over 16 bits for I and Q
	 *
	 * \param[in] adrv910x             Context variable - Pointer to the ADRV910x device data structure
	 * \param[in] channel              The Rx channel for which to configure SSI test mode
	 * \param[in] ssiType              The type (CMOS/LVDS) of SSI interface
	 * \param[in] dataFormat            The data format (I-data or I/Q-data) of the SSI interface
	 * \param[in] ssiTestModeConfig    The desired SSI test mode to configure
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_ssi_Rx_TestMode_Configure(adi_adrv910x_Device_t *adrv910x,
		adi_common_ChannelNumber_e channel,
		ssiType_e ssiType,
		ssiDataFormat_e dataFormat,
		adi_adrv910x_RxSsiTestModeCfg_t *ssiTestModeConfig);

	/**
	 * \brief Configure the SSI test mode for the specified Tx channel
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv910x             Context variable - Pointer to the ADRV910x device data structure
	 * \param[in] channel              The Tx channel for which to configure SSI test mode
	 * \param[in] ssiType              The type (CMOS/LVDS) of SSI interface
	 * \param[in] dataFormat           The data format (I-data or I/Q-data) of the SSI interface
	 * \param[in] ssiTestModeConfig    The desired SSI test mode to configure
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_Ssi_Tx_TestMode_Configure(adi_adrv910x_Device_t *adrv910x,
		adi_common_ChannelNumber_e channel,
		ssiType_e ssiType,
		ssiDataFormat_e dataFormat,
		adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig);

	/**
	 * \brief Inspect the SSI test mode status for the specified Tx channel
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv910x              Context variable - Pointer to the ADRV910x device data structure
	 * \param[in] channel               The Tx channel for which to inspect SSI test mode status
	 * \param[in] ssiType               The type (CMOS/LVDS) of SSI interface
	 * \param[in] dataFormat            The data format (I-data or I/Q-data) of the SSI interface
	 * \param[in] ssiTestModeConfig     The SSI test mode configuration for which to check status
	 * \param[out] ssiTestModeStatus    The current SSI test mode status
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_Ssi_Tx_TestMode_Status_Inspect(adi_adrv910x_Device_t *adrv910x,
		adi_common_ChannelNumber_e channel,
		ssiType_e ssiType,
		ssiDataFormat_e dataFormat,
		adi_adrv910x_TxSsiTestModeCfg_t *ssiTestModeConfig,
		adi_adrv910x_TxSsiTestModeStatus_t *ssiTestModeStatus);

	/**
	 * \brief Set the enabledness of Tx SSI to Rx SSI interface loopback
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv910x              Context variable - Pointer to the ADRV910x device data structure
	 * \param[in] channel               The channel for which to set SSI interface loopback
	 * \param[in] profile               Pointer to the ADRV910X profile settings data
	 * \param[in] loopbackEnable        A boolean flag to enable or disable ADRV910x Tx SSI to Rx SSI interface loopback
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_Ssi_Loopback_Set(adi_adrv910x_Device_t *adrv910x,
		adi_common_ChannelNumber_e channel,
		const deviceProfile_t *profile,
		bool loopbackEnable);

	/**
	 * \brief This function programs the SSI delay configuration in ADRV910x device through SPI.
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv910x          Context variable - Pointer to the ADRV910x device data structure
	 * \param[in] ssiType           LVDS or CMOS mode
	 * \param[in] ssiCalibration    The desired SSI calibration
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */

	int32_t adi_adrv910x_Ssi_Delay_Configure(adi_adrv910x_Device_t *adrv910x,
		ssiType_e ssiType,
		adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration);

	/**
	 * \brief This function gets the SSI delay configuration from ADRV910x device through SPI.
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in]  adrv910x         Context variable - Pointer to the ADRV910x device data structure
	 * \param[in]  ssiType          LVDS or CMOS mode
	 * \param[out] ssiCalibration   The current SSI calibration
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */

	int32_t adi_adrv910x_Ssi_Delay_Inspect(adi_adrv910x_Device_t *adrv910x,
		ssiType_e ssiType,
		adi_adrv910x_SsiCalibrationCfg_t *ssiCalibration);
	
	
	/**
	 * \brief Disable of Tx SSI pads
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
	 * \param[in] channel               The channel for which SSI pads to be disabled
	 * \param[in] ssiType               The type (CMOS/LVDS) of SSI interface
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_Ssi_Tx_Disable_LSSI_Pads(adi_adrv910x_Device_t *adrv9001,
		adi_common_ChannelNumber_e channel,
		ssiType_e ssiType);

	/**
	 * \brief Enable of Tx SSI pads
	 *
	 * \note Message type: \ref timing_direct "Direct register acccess"
	 *
	 * \param[in] adrv9001              Context variable - Pointer to the ADRV9001 device data structure
	 * \param[in] channel               The channel for which SSI pads to be enabled
	 * \param[in] ssiType               The type (CMOS/LVDS) of SSI interface
	 *
	 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
	 */
	int32_t adi_adrv910x_Ssi_Tx_Enable_LSSI_Pads(adi_adrv910x_Device_t *adrv9001,
		adi_common_ChannelNumber_e channel,
		ssiType_e ssiType);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV910x_SSI_H_ */