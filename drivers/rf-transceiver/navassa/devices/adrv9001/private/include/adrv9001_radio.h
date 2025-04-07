/**
* \file
* \brief Contains ADRV9001 Rx/Tx WB/NB PFIR Channel filter related function prototypes for
*        adrv9001_radio.c
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*
* Copyright 2015-2023 Analog Devices Inc.
* Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file.
*/

#ifndef _ADRV9001_RADIO_H_
#define _ADRV9001_RADIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* ADI specific header files */
#include "adi_common_error.h"
#include "adi_adrv9001_radio_types.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_pfirBuffer_types.h"
#include "adi_adrv9001_arm_types.h"
#include "adi_adrv9001_tx_types.h"
#include "adrv9001_init_types.h"
#include "adrv9001_init.h"
#include "adi_adrv9001_spi.h"
#include "adi_common_macros.h"

/**
 * \brief Write PFIR coefficients directly to memory via SPI
 *
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] device	  Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] pfirCoeff   PFIR configuration to write
 * \param[in] pfirTypeId  The PFIR type of interest 
 *                        (As of now only TX/RX WB/NB compensation PFIR types supported)
 * \param[in] port        The port of interest
 * \param[in] channel     The channel of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adrv9001_Radio_Pfir_Write(adi_adrv9001_Device_t *device,
	                              const adi_adrv9001_PfirWbNbBuffer_t *pfirCoeff,
	                              adi_adrv9001_PfirTypeId_e pfirTypeId,
	                              adi_common_Port_e port,
	                              adi_common_ChannelNumber_e channel);

/**
 * \brief Configure PFIR coefficients Bank selection mode
 *
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] device	Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] bankSel   The PFIR Bank of interest
 * \param[in] enable    Enable/Disable PFIR coefficients Bank selection
 * \param[in] port      The port of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adrv9001_Radio_PfirWbNbCompCoeffBankSelMode_Set(adi_adrv9001_Device_t *device,
	                                                    adi_adrv9001_PfirBank_e bankSel,
	                                                    uint8_t enable,
	                                                    adi_common_Port_e port);

/**
 * \brief Configure PFIR gain
 *
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] device	 Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] bankSel    The PFIR Bank of interest
 * \param[in] gain       The PFIR gain of interest
 * \param[in] port       The port of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adrv9001_Radio_PfirWbNbCompGain_Set(adi_adrv9001_Device_t *device,
	                                        adi_adrv9001_PfirBank_e bankSel,
	                                        adi_adrv9001_PfirGain_e gain,
		                                    adi_common_Port_e port);

/**
 * \brief Configure number of PFIR filter taps
 * 
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] device	 Context variable - Pointer to the ADRV9001 device settings data structure
 * \param[in] bankSel    The PFIR Bank of interest
 * \param[in] taps       The number of PFIR filter taps
 * \param[in] port       The port of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adrv9001_Radio_PfirWbNbCompTaps_Set(adi_adrv9001_Device_t *device,
	                                        adi_adrv9001_PfirBank_e bankSel,
	                                        adi_adrv9001_PfirNumTaps_e taps,
		                                    adi_common_Port_e port);

/**
 * \brief Configure PFIR coefficients symmetricity
 * 
 * \pre Channel state is RF_ENABLED
 *
 * \param[in] device	  Context variable - Pointer to the ADRV9001 device settings data structure              
 * \param[in] bankSel     The PFIR Bank of interest
 * \param[in] symmetric   The symmetricity of interest for PFIR coefficients
 * \param[in] port        The port of interest
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adrv9001_Radio_PfirWbNbCompSymmetric_Set(adi_adrv9001_Device_t *device,
	                                             adi_adrv9001_PfirBank_e bankSel,
	                                             adi_adrv9001_PfirSymmetric_e symmetric,
		                                         adi_common_Port_e port);

#ifdef __cplusplus
}
#endif

#endif /* _ADRV9001_RADIO_H_ */
