/*! ****************************************************************************
 *
 * @file:    device_profile_pfir_buffer_t.h
 *
 * @brief:   PFIR driver - buffer structure
 *
 * @details: Structure of the buffer use to send PFIR coefficients to ARM.
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
#ifndef __DEVICE_PROFILE_PFIR_BUFFER_T_H__
#define __DEVICE_PROFILE_PFIR_BUFFER_T_H__

#include "adi_device_profile_pack.h"
#include <stdint.h>
#include "device_profile_pfir_common_t.h"
#include "device_profile_pfir_mag_t.h"
#include "device_profile_pfir_pulse_t.h"
#include "device_profile_pfir_wbnb_t.h"
#include "device_profile_channels_t.h"

/**
 *  @addtogroup pfir
 *  @ingroup drivers
 *  @{
 */

/*! PFIR coefficent buffer */
ADI_NEVIS_PACK_START
typedef struct {
    /*!< During dynamic profile switching, the first two types of PFIRs, RXWbNb and TXWbNbPulShp, would be reloaded for each profile before the switch*/
    /*!<RX WB/NB Compensation PFIR (Channel Filter)coefficient Bank A/B/C/D in rxnb_dem, block30*/
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_A;
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_B;
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_C;
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_D;
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_E;
	pfirWbNbBuffer_t     pfirRxWbNbChFilterCoeff_F;    
    /*!<TX WB/NB Preprocessing pulse shaping PFIR coefficient Bank A/B in TX preproc*/
	pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_A;
	pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_B;
	pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_C;
	pfirWbNbBuffer_t     pfirTxWbNbPulShpCoeff_D;
    /*!< RX NB Pulse Shaping pFIR  128 taps CH1/2 in rxnb_dem  */
    pfirPulseBuffer_t     pfirRxNbPulShpDp1[NUM_RX_CHANNELS];
    pfirPulseBuffer_t 	  pfirRxNbPulShpDp2[NUM_RX_CHANNELS];
    /*!< Channel 1/2 Low/High TIA Bandwidth HP/LP ADC between Mux200 and 201  */
    pfirMag21Buffer_t     pfirRxMagLowTiaLowSR_RX1;
    pfirMag21Buffer_t     pfirRxMagLowTiaHighSR_RX1;
    pfirMag21Buffer_t     pfirRxMagHighTiaHighSR_RX1;
    pfirMag21Buffer_t     pfirRxMagLowTia_RXNB;
    pfirMag21Buffer_t     pfirRxMagHighTia_RXNB;
    /*!< TX Magnitude Compensation PFIR 21 taps at Mux410B */
    pfirMag21Buffer_t     pfirTxMagComp1;
    pfirMag21Buffer_t     pfirTxMagComp2;
    /*!< TX/RX Magnitude Compensation PFIR for NB */
    pfirMag13Buffer_t     pfirTxMagCompNb[NUM_TX_CHANNELS];
    pfirMag13Buffer_t     pfirRxMagCompNb[NUM_RX_CHANNELS];
    /*!< NB AGC mitigation mode PFIR 21 taps */
    pfirMag21Buffer_t pfirRxNbHb2Mitigation;
    uint32_t    checksum;
} pfirBuffer_t;
ADI_NEVIS_PACK_FINISH

/**@}*/
#endif /* __DEVICE_PROFILE_PFIR_BUFFER_T_H__ */
