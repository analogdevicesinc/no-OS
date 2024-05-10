/**
* Copyright 2023 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adrv904x_dfe_vswr.h
* \brief Contains ADRV904X VSWR function prototypes for
*    adrv904x_dfe_vswr.c
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADRV904X_DFE_VSWR_H_
#define _ADRV904X_DFE_VSWR_H_

#include "adi_adrv904x_error.h"



/**
* \brief Init Vswr number of samples to 0.
* 
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep{init-> (most members)}
* \dep_end
*
* \param[in,out] device Context variable -Pointer to the ADRV904X device settings data structure
* \param[in]     chanSel - Tx channel selection
*
* \retval ADI_ADRV904X_ERR_ACT_NONE if Successful
*/
ADI_API adi_adrv904x_ErrAction_e adrv904x_VswrPlaybackDatNumOfSamplesInit(adi_adrv904x_Device_t* const device,
                                                                          const adi_adrv904x_TxChannels_e chanSel);


#endif /* _ADRV904X_DFE_VSWR_H_ */
