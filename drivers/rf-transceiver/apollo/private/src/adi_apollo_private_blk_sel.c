/*!
 * \brief     Private block select API implementations
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PRIVATE_DEVICE
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_bmem_types.h"
#include "adi_apollo_cddc_types.h"
#include "adi_apollo_cduc_types.h"
#include "adi_apollo_cfir_types.h"
#include "adi_apollo_cnco_types.h"
#include "adi_apollo_fnco_types.h"
#include "adi_apollo_fddc_types.h"
#include "adi_apollo_fduc_types.h"
#include "adi_apollo_fsrc_types.h"
#include "adi_apollo_invsinc_types.h"
#include "adi_apollo_pfilt_types.h"
#include "adi_apollo_sniffer_types.h"
#include "adi_apollo_smon_types.h"
#include "adi_apollo_txmux_types.h"
#include "adi_apollo_private_device.h"

int32_t adi_apollo_private_blk_sel_mask_set(adi_apollo_device_t *device)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);

    /* Set the valid blk select masks based on 4T4R/8T8R device type. */
    device->dev_info.blk_sel_mask.adc       = 0xff;
    device->dev_info.blk_sel_mask.bmem      = (device->dev_info.is_8t8r ? ADI_APOLLO_BMEM_ALL : ADI_APOLLO_BMEM_ALL_4T4R);
    device->dev_info.blk_sel_mask.cddc      = (device->dev_info.is_8t8r ? ADI_APOLLO_CDDC_ALL : ADI_APOLLO_CDDC_ALL_4T4R);
    device->dev_info.blk_sel_mask.cduc      = (device->dev_info.is_8t8r ? ADI_APOLLO_CDUC_ALL : ADI_APOLLO_CDUC_ALL_4T4R);
    device->dev_info.blk_sel_mask.cfir      = (device->dev_info.is_8t8r ? ADI_APOLLO_CFIR_ALL : ADI_APOLLO_CFIR_ALL_4T4R);
    device->dev_info.blk_sel_mask.cnco      = (device->dev_info.is_8t8r ? ADI_APOLLO_CNCO_ALL : ADI_APOLLO_CNCO_ALL_4T4R);
    device->dev_info.blk_sel_mask.dac       = 0xff; 
    device->dev_info.blk_sel_mask.fddc      = (device->dev_info.is_8t8r ? ADI_APOLLO_FDDC_ALL : ADI_APOLLO_FDDC_ALL_4T4R);
    device->dev_info.blk_sel_mask.fduc      = (device->dev_info.is_8t8r ? ADI_APOLLO_FDUC_ALL : ADI_APOLLO_FDUC_ALL_4T4R);
    device->dev_info.blk_sel_mask.fnco      = (device->dev_info.is_8t8r ? ADI_APOLLO_FNCO_ALL : ADI_APOLLO_FNCO_ALL_4T4R);
    device->dev_info.blk_sel_mask.fsrc      = (device->dev_info.is_8t8r ? ADI_APOLLO_FSRC_ALL : ADI_APOLLO_FSRC_ALL_4T4R);
    device->dev_info.blk_sel_mask.invsinc   = (device->dev_info.is_8t8r ? ADI_APOLLO_INVSINC_ALL : ADI_APOLLO_INVSINC_ALL_4T4R);
    device->dev_info.blk_sel_mask.pfilt     = (device->dev_info.is_8t8r ? ADI_APOLLO_PFILT_ALL : ADI_APOLLO_PFILT_ALL_4T4R);
    device->dev_info.blk_sel_mask.rxen      = 0xff;
    device->dev_info.blk_sel_mask.smon      = (device->dev_info.is_8t8r ? ADI_APOLLO_SMON_ALL : ADI_APOLLO_SMON_ALL_4T4R);
    device->dev_info.blk_sel_mask.sniffer   = ADI_APOLLO_SNIFFER_ALL;
    device->dev_info.blk_sel_mask.txen      = 0xff;
    device->dev_info.blk_sel_mask.txmux      = (device->dev_info.is_8t8r ? ADI_APOLLO_TX_SUMMER_ALL : ADI_APOLLO_TX_SUMMER_ALL_4T4R);

    return API_CMS_ERROR_OK;
}

/*! @} */
