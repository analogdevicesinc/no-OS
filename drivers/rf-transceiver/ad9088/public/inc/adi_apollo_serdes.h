/*!
 * \brief     SerDes related definition headers
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_SERDES
 * @{
 */
#ifndef __ADI_APOLLO_SERDES_H__
#define __ADI_APOLLO_SERDES_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_serdes_types.h"
#include "adi_apollo_cfg_types.h"

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  Execute an Apollo JRx serdes init (foreground) calibration
 * \note   FPGA must be transmitting data for SERDES cal to function successfully.
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_cal(adi_apollo_device_t* device);

/**
 * \brief  Execute an Apollo JRx serdes init (foreground) calibration with config option.
 * \note   FPGA must be transmitting data for SERDES cal to function successfully.
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes         Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[in] init_cal_cfg   Initial cal configuration. Specify init defaults, NVM or user defined. \ref adi_apollo_init_cal_cfg_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_init_cal(adi_apollo_device_t* device, uint16_t serdes, adi_apollo_init_cal_cfg_e init_cal_cfg);

/**
 * \brief  Freeze (Halt) the Apollo JRx serdes background calibration
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes         Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_bgcal_freeze(adi_apollo_device_t* device, uint16_t serdes);

/**
 * \brief  Unfreeze (Start/Restarts) the Apollo JRx serdes background calibration
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] serdes         Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_bgcal_unfreeze(adi_apollo_device_t* device, uint16_t serdes);

/**
 * \brief  Get SERDES JRx background calibration state
 *
 * \param[in]  device       Context variable - Pointer to the APOLLO device data structure
 * \param[in]  serdes       Serdes selection mask \ref adi_apollo_serdes_12pack_select_e
 * \param[out] state        Array of \ref adi_apollo_serdes_bgcal_state_t structs containing per SERDES PACK bgcal error and state info
 *                          Array/Serdes_Pack indexes : 0: SERDES_12PACK_A, 1: SERDES_12PACK_B
 * \param[in]  len          Length of state array (should be 2)
 *
 * Common Serdes bgcal states:
 *	0x1A = > APOLLO_SERDES_BGCAL_STATE_RESUMED   | APOLLO_SERDES_BGCAL_STATE_RUNNING  | APOLLO_SERDES_BGCAL_STATE_ENABLED
 *	0x25 = > APOLLO_SERDES_BGCAL_STATE_SUSPENDED | APOLLO_SERDES_BGCAL_STATE_INACTIVE | APOLLO_SERDES_BGCAL_STATE_DISABLED
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
*/
int32_t adi_apollo_serdes_jrx_bgcal_state_get(adi_apollo_device_t *device, uint16_t serdes, adi_apollo_serdes_bgcal_state_t state[], uint32_t len);

/**
 * \brief  Configures the tx serdes pll
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] config         Serdes pll configuration parameters. \ref adi_apollo_serdes_pll_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_pll_config(adi_apollo_device_t *device, adi_apollo_serdes_pll_pgm_t *config);

/**
 * \brief Power up/down the serdes pll
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] pwr_up         Power up or down \ref adi_apollo_serdes_pll_power_up_down_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_pll_power(adi_apollo_device_t *device, adi_apollo_serdes_pll_power_up_down_e pwr_up);

/**
 * \brief Serdes PLL reset
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] reset_type     Type of serdes reset.  \ref adi_apollo_serdes_pll_reset_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_pll_reset(adi_apollo_device_t *device, adi_apollo_serdes_pll_reset_e reset_type);

/**
 * \brief  Configures the tx serializer
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] sides          Serializer select. There is on per side.
 * \param[in] config         Serializer configuration parameters. \ref adi_apollo_serializer_pgm_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_serializer_config(adi_apollo_device_t *device, const uint16_t sides, adi_apollo_serializer_pgm_t *config);

/**
 * \brief  Configures the PRBS Generator
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 * \param[in] config         \ref adi_apollo_serdes_prbs_generator_enable_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_prbs_generator_enable(adi_apollo_device_t *device,
            uint8_t lanes[], uint32_t lanes_len,
            adi_apollo_serdes_prbs_generator_enable_t *config);

/**
 * \brief  Configures the JRx PRBS Checker
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 * \param[in] config         The PRBS configuration to apply to lanes requested \ref adi_apollo_serdes_prbs_checker_enable_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_prbs_checker_enable(adi_apollo_device_t *device,
            uint8_t lanes[], uint32_t lanes_len,
            adi_apollo_serdes_prbs_checker_enable_t *config);

/**
 * \brief  Reads the PRBS Checker status
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] status         Array of lane status result structs, one for lane \ref adi_apollo_serdes_prbs_checker_status_t
 * \param[in] lanes_len      Length of lanes and status arrays
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_prbs_checker_status(adi_apollo_device_t *device,
            uint8_t lanes[],
            adi_apollo_serdes_prbs_checker_status_t status[],
            uint32_t lanes_len);

/**
 * \brief  Apply the JRx SPO clock strobe
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 * \param[in] spo_strobe     Apply strobe to lanes requested \ref adi_apollo_serdes_ck_spo_strobe_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_clock_strobe(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len,
        adi_apollo_serdes_ck_spo_strobe_e spo_strobe);

/**
 * \brief  Set the JRx DFE flash mask
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 * \param[in] flash_mask     Digital counting of flash outputs toward DLL phase accumulator. 4-bit value.
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_set_flash_mask(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len, uint8_t flash_mask);

/**
 * \brief  Set the JRx SPO direction
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 * \param[in] dir            SPO direction. 1=UP, 0=DOWN
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_spo_dir_set(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len, uint8_t dir);

/**
 * \brief  Clear the PRBS error count
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] lanes          Array containing lanes for PRBS testing
 * \param[in] lanes_len      Length of lanes array
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_prbs_clear_error(adi_apollo_device_t *device,
        uint8_t lanes[], uint32_t lanes_len);

/**
 * \brief  Perform a Horizontal Eye Sweep for select JRx Serdes lane.
 * \note   Only 1 lane selection per call.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   lane            Target Serdes Lane. \ref adi_apollo_mailbox_serdes_lane_num_e
 * \param[in]   prbs_pattern    PRBS Pattern being used. \ref adi_apollo_serdes_jrx_prbs_mode_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_horiz_eye_sweep(adi_apollo_device_t *device,
                                              const uint16_t lane,
                                              const uint8_t prbs_pattern);

/**
 * \brief  Get Response of a Horizontal Eye Monitor Test for select JRx serdes lane.
 * \note   Only 1 lane selection per call.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   lane            Target Serdes Lane. \ref adi_apollo_mailbox_serdes_lane_num_e
 * \param[out]  resp            Response of a Horizontal Eye Monitor Test. See \ref adi_apollo_serdes_jrx_horiz_eye_resp_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_horiz_eye_sweep_resp_get(adi_apollo_device_t *device,
                                                       const uint16_t lane,
                                                       adi_apollo_serdes_jrx_horiz_eye_resp_t *resp);

/**
 * \brief  Perform a Vertical Eye Sweep for select JRx Serdes lane.
 * \note   Only 1 lane selection per call.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   lane            Target Serdes Lane. \ref adi_apollo_mailbox_serdes_lane_num_e
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_vert_eye_sweep(adi_apollo_device_t *device,
                                             const uint16_t lane);

/**
 * \brief  Get Response of a Vertical Eye Monitor Test for select JRx serdes lane.
 * \note   Only 1 lane selection per call.
 *
 * \param[in]   device          Context variable - Pointer to the APOLLO device data structure
 * \param[in]   lane            Target Serdes Lane. \ref adi_apollo_mailbox_serdes_lane_num_e
 * \param[out]  resp            Response of a Vertical Eye Monitor Test. See \ref adi_apollo_serdes_jrx_vert_eye_resp_t
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_jrx_vert_eye_sweep_resp_get(adi_apollo_device_t *device,
                                                      const uint16_t lane,
                                                      adi_apollo_serdes_jrx_vert_eye_resp_t *resp);

/**
 * \brief  Reads serdes PLL lock status
 *
 * \param[in] device         Context variable - Pointer to the APOLLO device data structure
 * \param[in] side_index     Apollo side index \ref adi_apollo_sides_e
 * \param[in] lane_mask      Lane mask to read the status
 * \param[out] status        PLL lock status
 *
 * \return API_CMS_ERROR_OK                     API Completed Successfully
 * \return <0                                   Failed. \ref adi_cms_error_e for details.
 */
int32_t adi_apollo_serdes_pll_locked_status(adi_apollo_device_t *device,
            adi_apollo_sides_e side_index,
            uint16_t lane_mask,
            uint16_t *status);

#ifndef CLIENT_IGNORE

#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_SERDES_H__ */
/*! @} */