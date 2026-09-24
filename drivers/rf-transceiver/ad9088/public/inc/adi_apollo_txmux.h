/*!
 * \brief     TX MUX Block definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_TXMUX
 * @{
 */
#ifndef __ADI_APOLLO_TXMUX_H__
#define __ADI_APOLLO_TXMUX_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_txmux_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief  Sets the fine DUC to summer/coarse DUC connection path
 *
 * \param[in]  device           Context variable - Pointer to the APOLLO device data structure
 * \param[in]  summers          Tx path summer selection, bitwise-or. ADI_APOLLO_TX_SUMMER_A0, A1, ... A4, ADI_APOLLO_TX_SUMMER_B0, B1, ... B4 (A3-A4, B3-B4 for 8T8R) \ref adi_apollo_summer_select_e
 * \param[in]  fducs_connected  FDUCs connected to the summers, bitwise-or. ADI_APOLLO_FDUC_A0, A1, ... A7, ADI_APOLLO_FDUC_B0, B1, ... B7 (A4-A7, B4-B7 for 8T8R)
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmux_summer_block_set(adi_apollo_device_t *device, const uint16_t summers, const uint16_t fducs_connected);

/**
 * \brief  Sets HSDOUT MUX path
 * 
 *  Array index selects DAC (both 4T4R and 8T8R)
 *  dac[0]  DAC-A0
 *  dac[1]  DAC-A1
 *  dac[2]  DAC-A2
 *  dac[3]  DAC-A3
 * 
 * Values (4T4R)
 *    0     Slice 0-x       (e.g. CDUC-A0)
 *    1     Slice 1-x       (e.g. CDUC-A1)
 *    2     0s
 *    2     0s
 * 
 * Values (8T8R)
 *    0     Slice 0-0       (e.g. CDUC-A0)
 *    1     Slice 0-1       (e.g. CDUC-A2)
 *    2     Slice 1-0       (e.g. CDUC-A1)
 *    2     Slice 1-1       (e.g. CDUC-A3)
 * 
 * Same applies to side B
 * 
 *
 * Example Usage (8T8R)
 * 
 *   // Mux CDUC outputs to DAC - A0
 *   uint8_t dac_to_cduc0[] = {0, 1, 2, 3};      // DAC-A0 => CDUC-A0
 *   adi_apollo_txmux_hsdout_set(device, ADI_APOLLO_SIDE_A, dac_to_cduc0, 4);
 * 
 *   uint8_t dac_to_cduc1[] = {1, 1, 2, 3};      // DAC-A0 => CDUC-A2
 *   adi_apollo_txmux_hsdout_set(device, ADI_APOLLO_SIDE_A, dac_to_cduc1, 4);
 * 
 *   uint8_t dac_to_cduc2[] = {2, 1, 2, 3};      // DAC-A0 => CDUC-A1
 *   adi_apollo_txmux_hsdout_set(device, ADI_APOLLO_SIDE_A, dac_to_cduc2, 4);
 * 
 *   uint8_t dac_to_cduc3[] = {3, 1, 2, 3};      // DAC-A0 => CDUC-A3
 *   adi_apollo_txmux_hsdout_set(device, ADI_APOLLO_SIDE_A, dac_to_cduc3, 4);
 * 
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel     Device side(s) to set HSDOUT \ref adi_apollo_side_select_e.
 * \param[in]  dac          Array indices are DAC index for a side (0-3) and values are corresponding HSDOUT xbar input channels(0-3) to be connected
 * \param[in]  length       Length of the dac array, max value ADI_APOLLO_NUM_DAC
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmux_hsdout_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, const uint8_t dac[], const uint32_t length);

/**
 * \brief  Sets the modulator switch (MODSW)
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  side_sel     Device side(s) to set MODSW \ref adi_apollo_side_select_e
 * \param[in]  modsw0       Modulator switch configuration \ref adi_apollo_modsw_select_e
 * \param[in]  modsw1       Modulator switch configuration (for 8T8R only) \ref adi_apollo_modsw_select_e
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_txmux_modsw_set(adi_apollo_device_t *device, adi_apollo_side_select_e side_sel, adi_apollo_modsw_select_e modsw0, adi_apollo_modsw_select_e modsw1);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_TXMUX_H__ */
/*! @} */
