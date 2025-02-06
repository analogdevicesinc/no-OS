/*******************************************************************************
 *   @file   diag_mngr.c
 *   @brief  Diagnostic manager for GMSL parts.
 *   @author Automotive Software and Systems team, Bangalore, India
 ********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

/*================================= INCLUDES ================================*/
#include "diag_mngr.h"
#include "gmsl_dbg.h"
/*================================= DEFINES =================================*/
#define DIAG_MNGR_ROLLOVER_COUNT    (100)

/*================================ DATA TYPES ===============================*/
static struct gmsl_diag_mngr_cfg **diag_mngr_config;
static uint8_t diag_mngr_num_of_devices;
static void *diag_mngr_pay_load_buf;
/*=================================== DATA ==================================*/

/*================================ PROTOTYPES ===============================*/

/*================================== STATIC =================================*/

/*=================================== CODE ==================================*/

/**
 * @brief Invoke diagnostic manager for a specific device and diagnostic type.
 *
 * @param dev           - Pointer to the GMSL device structure.
 * @param diag_sub_type - Diagnostic sub-type to be invoked.
 * @param diag_err      - Pointer to store the diagnostic error status. diag_err will be updated with true if error, and false if no error.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
static int32_t gmsl_diag_mngr_invoke(struct gmsl_dev *dev,
				     enum gmsl_dev_diag_sub_type diag_sub_type, bool *diag_err)
{
	int32_t ret = 0;

	switch (diag_sub_type) {
	case GMSL_DIAG_PART_DEV_ID:
		if (NULL != ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_device_id)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_device_id(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_PART_DEV_REV:
		if (NULL != ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_device_rev)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_device_rev(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_PART_CFG:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_part_config)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_part_config(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LINK_LOCK:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_link_lock_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_link_lock_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LINK_DECODE:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_decode_error_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_decode_error_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_LINK_IDLE:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_idle_err_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_idle_err_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LINK_MAX_RETRANS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_max_retrans_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_max_retrans_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_LINK_LINE_FAULT:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_line_fault_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_line_fault_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_LINK_FEC:
		if (NULL != ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_fec_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_fec_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LINK_EOM:
		if (NULL != ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_eom_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_eom_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_PCLK_LOST:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_pclk_lost_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_pclk_lost_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_OVERFLOW:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_overflow_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_overflow_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_DRIFT:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_drift_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_drift_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_LINE_ENABLED:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_get_is_video_line_crc_enabled)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_get_is_video_line_crc_enabled(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_STREAM_ID:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_stream_id_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_stream_id_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_PARAMETER:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_parameter_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_parameter_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_PPS:
		if (NULL != ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_pps_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_pps_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_STREAM_DSC_COMPRESSION:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_video_stream_dsc_compression)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_video_stream_dsc_compression(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_DP_FEC_SUPPORT:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_dp_fec_support)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_dp_fec_support(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_HDCP:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_hdcp_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_hdcp_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LINK_TRAINING:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_link_training_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_link_training_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_VIDEO_CRC_ERROR_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_get_is_video_line_crc_error_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_get_is_video_line_crc_error_status(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_MIPI_RX:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_mipi_rx_err_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_mipi_rx_err_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_MEM_ECC_2BIT_ERROR:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_mem_ecc_2b_err_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_mem_ecc_2b_err_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_DP_LINK_RATE:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_get_dp_link_rate)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_get_dp_link_rate(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_DP_LANE_COUNT:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_get_dp_lane_count)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_get_dp_lane_count(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_PHY_LOW_POWER_ERR:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_phy_low_power_err_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_phy_low_power_err_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_MIPI_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_mipi_status)
			ret = ((struct gmsl_dev_diag_ops *) dev->dev_diag)->diag_check_mipi_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_REMOTE_ERROR_FLAG:
		if (NULL != ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_remote_err)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_remote_err(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_LMO_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_lmo_status)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_lmo_status(dev,
				diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VID_BLK_LENGTH_ERROR:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_video_block_length_err)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_video_block_length_err(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_VIDEO_PSM_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_programmable_state_machine_error_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_programmable_state_machine_error_status(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_LINE_CRC:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_get_is_video_line_crc_error_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_get_is_video_line_crc_error_status(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_ERROR_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_video_error_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_video_error_status(dev, diag_mngr_pay_load_buf,
				       diag_err);
		break;
	case GMSL_DIAG_VIDEO_TIMING_DETECT_STATUS:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_video_timing_detect_status)
			ret = ((struct gmsl_dev_diag_ops *)
			       dev->dev_diag)->diag_check_video_timing_detect_status(dev,
				       diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_VIDEO_LOCK:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_video_lock_status)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_video_lock_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	case GMSL_DIAG_INFOFR_CC:
		if (NULL != ((struct gmsl_dev_diag_ops *)
			     dev->dev_diag)->diag_check_infofr_cc_status)
			ret = ((struct gmsl_dev_diag_ops *)dev->dev_diag)->diag_check_infofr_cc_status(
				      dev, diag_mngr_pay_load_buf, diag_err);
		break;
	default:
		/* Do Nothing */
		break;
	}
	return ret;
}

/**
 * @brief Initialize the diagnostic manager.
 *
 * @param pconfig         - Pointer to the diagnostic manager configuration.
 * @param num_dev         - Number of devices.
 * @param pay_load_buffer - Pointer to the payload buffer.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_diag_mngr_init(struct gmsl_diag_mngr_cfg **pconfig,
			    uint8_t num_dev, void *pay_load_buffer)
{
	/* Call init startup diag calls here */
	struct gmsl_diag_mngr_cfg * mngr_cfg;
	enum gmsl_dev_diag_sub_type diag_sub_type;
	uint8_t dev_index;
	int32_t ret;
	bool diag_err;

	diag_mngr_config = pconfig;
	diag_mngr_num_of_devices = num_dev;
	diag_mngr_pay_load_buf = pay_load_buffer;

	/* Do the tests configured for startup for each device */
	for (dev_index = 0; dev_index < diag_mngr_num_of_devices; dev_index++) {
		mngr_cfg = diag_mngr_config[dev_index];

		/* Run if test is configured to be run on startup*/
		for (diag_sub_type = 0; diag_sub_type < GMSL_DIAG_SUB_TYPE_MAX_COUNT;
		     diag_sub_type++) {
			if ((mngr_cfg->diag_type_cfg[diag_sub_type] != NULL)
			    && (mngr_cfg->diag_type_cfg[diag_sub_type]->interval_in_sec == 0)) {
				diag_err = false; /* Clear before invoking */

				ret = gmsl_diag_mngr_invoke(mngr_cfg->dev, diag_sub_type, &diag_err);

				if ((mngr_cfg->diag_type_cfg[diag_sub_type]->call_back != NULL)) {
					if (ret != 0) {
						mngr_cfg->diag_type_cfg[diag_sub_type]->call_back(mngr_cfg->dev, diag_sub_type,
								diag_mngr_pay_load_buf, true);
					} else if (diag_err == true) {
						mngr_cfg->diag_type_cfg[diag_sub_type]->call_back(mngr_cfg->dev, diag_sub_type,
								diag_mngr_pay_load_buf, false);
					} else {
						/* Do Nothing */
					}
				}
			}
		}
	}

	GMSL_LOG_INFO("Diagnostics Init done");
	return ret;
}

/**
 * @brief Process the diagnostic manager.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_diag_mngr_proc(void)
{
	static time_t ref_time_sec;
	uint8_t dev_index;
	static uint8_t counter_index = 0;
	struct gmsl_diag_mngr_cfg * mngr_cfg;
	enum gmsl_dev_diag_sub_type diag_sub_type;
	int32_t ret;
	bool diag_err;

	if (difftime(time(NULL), ref_time_sec) != 0) {
		ref_time_sec = time(NULL);
		GMSL_LOG_DEBUG("Diagnostic Manager Process Invoked");
		/* Do the tests for each device */
		for (dev_index = 0; dev_index < diag_mngr_num_of_devices; dev_index++) {
			mngr_cfg = diag_mngr_config[dev_index];

			/* Run if configured period has expired for each test and run it*/
			for (diag_sub_type = 0; diag_sub_type < GMSL_DIAG_SUB_TYPE_MAX_COUNT;
			     diag_sub_type++) {
				if ((mngr_cfg->diag_type_cfg[diag_sub_type] != NULL)
				    && (mngr_cfg->diag_type_cfg[diag_sub_type]->interval_in_sec != 0) &&
				    ((counter_index % mngr_cfg->diag_type_cfg[diag_sub_type]->interval_in_sec) ==
				     0)) {
					diag_err = false; /* Clear before invoking */

					ret = gmsl_diag_mngr_invoke(mngr_cfg->dev, diag_sub_type, &diag_err);
					if ((mngr_cfg->diag_type_cfg[diag_sub_type]->call_back != NULL)) {
						if (ret != 0) {
							mngr_cfg->diag_type_cfg[diag_sub_type]->call_back(mngr_cfg->dev, diag_sub_type,
									diag_mngr_pay_load_buf, true);
						} else if (diag_err == true) {
							mngr_cfg->diag_type_cfg[diag_sub_type]->call_back(mngr_cfg->dev, diag_sub_type,
									diag_mngr_pay_load_buf, false);
						} else {
							/* Do Nothing */
						}
					}
				}
			}
		}

		/* Increment counter index and reset it after pre-defined seconds */
		counter_index++;
		if (counter_index >= DIAG_MNGR_ROLLOVER_COUNT) {
			counter_index = 0;
		}
	}
	return ret;
}

/**
 * @brief Perform a one-shot diagnostic check.
 *
 * @param mngr_cfg - Pointer to the diagnostic manager configuration.
 * @param call_back - Callback function to be called on fault detection.
 * @param fault_type - Fault type to be checked.
 *
 * @return 0 in case of success, negative error code in case of PAL errors.
 */
int32_t gmsl_diag_mngr_oneshot(struct gmsl_diag_mngr_cfg *mngr_cfg,
			       fault_call_back call_back, enum gmsl_dev_diag_sub_type fault_type)
{
	int32_t ret = 0;
	bool diag_err;

	diag_err = false; /* Clear before invoking */

	if (mngr_cfg->diag_type_cfg[fault_type] != NULL) {
		ret = gmsl_diag_mngr_invoke(mngr_cfg->dev, fault_type, &diag_err);

		if ((call_back != NULL)) {
			if (ret != 0) {
				call_back(mngr_cfg->dev, fault_type, diag_mngr_pay_load_buf, true);
			} else if (diag_err) {
				call_back(mngr_cfg->dev, fault_type, diag_mngr_pay_load_buf, false);
			} else {
				/* Do Nothing */
			}
		}
	}
	return ret;
}