/*******************************************************************************
 *   @file   iio_pqm.c
 *   @brief  IIO PQM device with 11 channels and all PQ attributes
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *******************************************************************************/

#include "iio_pqm.h"
#include "pqm.h"
#include "pqlib_afe.h"
#include "flash_storage.h"
#include "ptp_status.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PQM_VOLTAGE_CHANNEL(_idx, _scan_idx, _name) \
	{ .name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx, \
	  .scan_index = _scan_idx, .indexed = true, .scan_type = &pqm_scan_type, \
	  .attributes = voltage_pqm_attributes, .ch_out = false }

#define PQM_CURRENT_CHANNEL(_idx, _scan_idx, _name) \
	{ .name = _name, .ch_type = IIO_CURRENT, .channel = _idx, \
	  .scan_index = _scan_idx, .indexed = true, .scan_type = &pqm_scan_type, \
	  .attributes = current_pqm_attributes, .ch_out = false }

#define PQM_EVENT_CHANNEL(_idx, _scan_idx, _name, _attrb_def) \
	{ .name = _name, .ch_type = IIO_COUNT, .channel = _idx, \
	  .scan_index = _scan_idx, .indexed = true, \
	  .scan_type = 0, .attributes = _attrb_def, \
	  .ch_out = true }

#define EVENT_COMMON_ATTR \
	{ .name = "countEvent", .show = read_ch_attr, .priv = CHAN_EVENT_COUNT }, \
	{ .name = "startTime", .show = read_ch_attr, .priv = CHAN_EVENT_START_TIME }, \
	{ .name = "endTime", .show = read_ch_attr, .priv = CHAN_EVENT_END_TIME }, \
	{ .name = "durationInCycles", .show = read_ch_attr, .priv = CHAN_EVENT_DURATION_IN_CYCL }

/* Macro compatibility */
#define NO_OS_ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Available-value string arrays (used by attribute read/write) */
static const char *const pqm_v_consel_available[] = {
	[VCONSEL_4W_WYE] = "4W_WYE",
	[VCONSEL_3W_DELTA_VB_VA_NEGVC] = "3W_DELTA_VA_VB_NEGVC",
	[VCONSEL_4W_WYE_VB_NEGVA_NEGVC] = "4W_WYE_VB_NEGVA_NEGVC",
	[VCONSEL_4W_DELTA_VB_NEGVA] = "4W_DELTA_VB_NEGVA",
	[VCONSEL_4W_DELTA_VA_VB_VC] = "4W_DELTA_VA_VB_VC",
};

static const char *const pqm_flicker_model_available[] = {
	[ADI_PQLIB_FLICKER_MODEL_230V_50HZ] = "230V_50HZ",
	[ADI_PQLIB_FLICKER_MODEL_120V_50HZ] = "120V_50HZ",
	[ADI_PQLIB_FLICKER_MODEL_230V_60HZ] = "230V_60HZ",
	[ADI_PQLIB_FLICKER_MODEL_120V_60HZ] = "120V_60HZ",
};

static const char *const pqm_nominal_frequency_available[] = {
	[ADI_PQLIB_NOMINAL_FREQUENCY_50HZ] = "50",
	[ADI_PQLIB_NOMINAL_FREQUENCY_60HZ] = "60",
};

static const char *const pqm_calibration_type_available[] = {
	[CALIBRATION_TYPE_GAIN] = "GAIN",
	[CALIBRATION_TYPE_OFFSET] = "OFFSET",
};

static const char *const pqm_calibration_phase_available[] = {
	[CALIBRATION_PHASE_A] = "A",
	[CALIBRATION_PHASE_B] = "B",
	[CALIBRATION_PHASE_C] = "C",
};

/* Global PQM device instance */
static struct pqm_desc pqm_desc_storage;
struct pqm_desc *pqm_desc_inst = &pqm_desc_storage;

volatile bool processData = true;

/**
 * @brief Read a pqm device attribute.
 */
int read_pqm_attr(void *device, char *buf, uint32_t len,
		  const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct pqm_desc *desc;

	if (!device)
		return -ENODEV;
	desc = device;
	if (attr_id < PQM_DEVICE_ATTR_NUMBER) {
		switch (attr_id) {
		case NOMINAL_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.nominalVoltage);
		case VOLTAGE_SCALE:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.voltageScale);
		case CURRENT_SCALE:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.currentScale);
		case DIP_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.dipThreshold);
		case DIP_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.dipHysteresis);
		case SWELL_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.swellThreshold);
		case SWELL_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.swellHysteresis);
		case INTERP_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.intrpThreshold);
		case INTERP_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.intrpHysteresis);
		case RVC_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.rvcThreshold);
		case RVC_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.rvcHysteresis);
		case MSV_CARRIER_FREQUENCY:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.msvCarrierFrequency);
		case MSV_RECORDING_LENGTH:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.msvRecordingLength);
		case MSV_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.msvThreshold);
		case V_CONSEL:
			return snprintf(buf, len, "%s",
					pqm_v_consel_available[pqlibExample.exampleConfig.vconsel]);
		case V_CONSEL_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_v_consel_available); i++) {
				strcat(buf, pqm_v_consel_available[i]);
				if (i != (int)NO_OS_ARRAY_SIZE(pqm_v_consel_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case FLICKER_MODEL:
			return snprintf(buf, len, "%s",
					pqm_flicker_model_available[pqlibExample.exampleConfig.flickerModel]);
		case FLICKER_MODEL_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_flicker_model_available); i++) {
				strcat(buf, pqm_flicker_model_available[i]);
				if (i != (int)NO_OS_ARRAY_SIZE(pqm_flicker_model_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case NOMINAL_FREQUENCY:
			return snprintf(buf, len, "%" PRIu32 "",
					pqlibExample.exampleConfig.nominalFrequency);
		case NOMINAL_FREQUENCY_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available); i++) {
				strcat(buf, pqm_nominal_frequency_available[i]);
				if (i != (int)NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case PROCESS_DATA:
			return snprintf(buf, len, "%s", (processData) ? "1" : "0");
		case FW_VERSION_NR:
			return snprintf(buf, len, "%.1f", (double)FW_VERSION);
#if ADI_PQLIB_CFG_DISABLE_SYMM_COMP == 0
		case NEG_UNB_VOLTAGE_RATIO:
			return snprintf(buf, len, "%.2f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.negUnbRatio));
		case ZERO_UNB_VOLTAGE_RATIO:
			return snprintf(buf, len, "%.2f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.zeroUnbRatio));
		case SZRO_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.zeroSeqMag));
		case SZRO_VOLTAGE_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.zeroSeqAngle));
		case SPOS_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.posSeqMag));
		case SPOS_VOLTAGE_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.posSeqAngle));
		case SNEG_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.negSeqMag));
		case SNEG_VOLTAGE_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.voltageUnb.negSeqAngle));
		case NEG_UNB_CURRENT_RATIO:
			return snprintf(buf, len, "%.2f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.negUnbRatio));
		case ZERO_UNB_CURRENT_RATIO:
			return snprintf(buf, len, "%.2f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.zeroUnbRatio));
		case SZRO_CURRENT:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.zeroSeqMag));
		case SZRO_CURRENT_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.zeroSeqAngle));
		case SPOS_CURRENT:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.posSeqMag));
		case SPOS_CURRENT_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.posSeqAngle));
		case SNEG_CURRENT:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.negSeqMag));
		case SNEG_CURRENT_ANGLE:
			return snprintf(buf, len, "%.2f",
					(double)convert_fract_angle_type(
						pqlibExample.output->params1012Cycles
						.currentUnb.negSeqAngle));
#endif
		case CAL_TYPE:
			if (pqlibExample.exampleConfig.calibrationType < NO_OS_ARRAY_SIZE(
				    pqm_calibration_type_available))
				return snprintf(buf, len, "%s",
						pqm_calibration_type_available[pqlibExample.exampleConfig.calibrationType]);
			return snprintf(buf, len, "GAIN");
		case CAL_TYPE_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_calibration_type_available); i++) {
				strcat(buf, pqm_calibration_type_available[i]);
				if (i != (int)NO_OS_ARRAY_SIZE(pqm_calibration_type_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CAL_STATUS:
			return snprintf(buf, len, "%s", calibration_get_status_string());
		case CAL_PHASE:
			if (calibrationCtx.phase < NO_OS_ARRAY_SIZE(pqm_calibration_phase_available))
				return snprintf(buf, len, "%s",
						pqm_calibration_phase_available[calibrationCtx.phase]);
			return snprintf(buf, len, "A");
		case CAL_PHASE_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_calibration_phase_available); i++) {
				strcat(buf, pqm_calibration_phase_available[i]);
				if (i != (int)NO_OS_ARRAY_SIZE(pqm_calibration_phase_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CAL_NOMINAL_CURRENT:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.calNominalCurrent);
		case CAL_NOMINAL_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.calNominalVoltage);
		case CAL_OFFSET_CURRENT:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.calOffsetCurrent);
		case CAL_OFFSET_VOLTAGE:
			return snprintf(buf, len, "%.2f",
					(double)pqlibExample.exampleConfig.calOffsetVoltage);
		case CAL_START:
			return snprintf(buf, len, "%s",
					calibration_is_active() ? "1" : "0");
		case CAL_GAIN_I_ERROR_BEFORE:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.gain_i_error_before);
		case CAL_GAIN_V_ERROR_BEFORE:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.gain_v_error_before);
		case CAL_GAIN_I_ERROR_AFTER:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.gain_i_error_after);
		case CAL_GAIN_V_ERROR_AFTER:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.gain_v_error_after);
		case CAL_OFFSET_I_ERROR_BEFORE:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.offset_i_error_before);
		case CAL_OFFSET_V_ERROR_BEFORE:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.offset_v_error_before);
		case CAL_OFFSET_I_ERROR_AFTER:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.offset_i_error_after);
		case CAL_OFFSET_V_ERROR_AFTER:
			return snprintf(buf, len, "%.6f",
					(double)calibrationCtx.result.offset_v_error_after);
		case FLASH_CAL_SAVE:
			return snprintf(buf, len, "0");
		case FLASH_CAL_LOAD:
			return snprintf(buf, len, "0");
		case FLASH_CAL_ERASE:
			return snprintf(buf, len, "0");
		case FLASH_STAT: {
			FLASH_CALIBRATION_DATA tmp_data;
			int flash_ret = flash_read_calibration(&tmp_data);
			switch (flash_ret) {
			case FLASH_STATUS_OK:
				return snprintf(buf, len, "ok");
			case FLASH_STATUS_NO_DATA:
				return snprintf(buf, len, "no_data");
			case FLASH_STATUS_INVALID_CRC:
				return snprintf(buf, len, "crc_error");
			case FLASH_STATUS_INVALID_VERSION:
				return snprintf(buf, len, "version_error");
			default:
				return snprintf(buf, len, "error");
			}
		}
		case FLASH_CAL_DATA: {
			FLASH_CALIBRATION_DATA flash_data;
			int ret;
			int offset = 0;

			if (!flash_storage_is_initialized()) {
				ret = flash_storage_init();
				if (ret != FLASH_STATUS_OK)
					return snprintf(buf, len, "flash_init_error");
			}

			ret = flash_read_calibration(&flash_data);
			if (ret != FLASH_STATUS_OK)
				return snprintf(buf, len, "no_data");

			offset += snprintf(buf + offset, len - offset,
					   "CH_A:gain=%d,off=%d,ig=0x%x,vg=0x%x;",
					   flash_data.channel[0].gain_calibrated ? 1 : 0,
					   flash_data.channel[0].offset_calibrated ? 1 : 0,
					   (unsigned)flash_data.channel[0].i_gain,
					   (unsigned)flash_data.channel[0].v_gain);
			offset += snprintf(buf + offset, len - offset,
					   "CH_B:gain=%d,off=%d,ig=0x%x,vg=0x%x;",
					   flash_data.channel[1].gain_calibrated ? 1 : 0,
					   flash_data.channel[1].offset_calibrated ? 1 : 0,
					   (unsigned)flash_data.channel[1].i_gain,
					   (unsigned)flash_data.channel[1].v_gain);
			offset += snprintf(buf + offset, len - offset,
					   "CH_C:gain=%d,off=%d,ig=0x%x,vg=0x%x;",
					   flash_data.channel[2].gain_calibrated ? 1 : 0,
					   flash_data.channel[2].offset_calibrated ? 1 : 0,
					   (unsigned)flash_data.channel[2].i_gain,
					   (unsigned)flash_data.channel[2].v_gain);
			return offset;
		}
		case PTP_STATE: {
			struct ptp_sync_status ptp_st;
			if (ptp_get_sync_status(&ptp_st) == 0)
				return snprintf(buf, len, "%s",
						ptp_port_state_str(ptp_st.port_state));
			return snprintf(buf, len, "UNKNOWN");
		}
		case PTP_OFFSET_NS: {
			struct ptp_sync_status ptp_st;
			if (ptp_get_sync_status(&ptp_st) == 0)
				return snprintf(buf, len, "%lld",
						(long long)ptp_st.offset_from_tt);
			return snprintf(buf, len, "0");
		}
		case PTP_MEAN_DELAY_NS: {
			struct ptp_sync_status ptp_st;
			if (ptp_get_sync_status(&ptp_st) == 0)
				return snprintf(buf, len, "%lld",
						(long long)ptp_st.mean_delay);
			return snprintf(buf, len, "0");
		}
		case PTP_CONVERGED: {
			struct ptp_sync_status ptp_st;
			if (ptp_get_sync_status(&ptp_st) == 0)
				return snprintf(buf, len, "%s",
						ptp_st.converged ? "1" : "0");
			return snprintf(buf, len, "0");
		}
		case MEASUREMENT_PTP_TIMESTAMP:
			return snprintf(buf, len, "%u.%09u",
					(unsigned)last_measurement_ptp_ts.second,
					last_measurement_ptp_ts.nanosecond);
		case MEASUREMENT_1012_PTP_TIMESTAMP:
			return snprintf(buf, len, "%u.%09u",
					(unsigned)last_1012cycle_ptp_ts.second,
					last_1012cycle_ptp_ts.nanosecond);
		case EVENT_TIME_BASE:
			return snprintf(buf, len, "ptp_ms");
		case CLEAR_EVENTS:
			return snprintf(buf, len, "d%u s%u i%u r%u",
					accumulated_events.dipCount,
					accumulated_events.swellCount,
					accumulated_events.intrpCount,
					accumulated_events.rvcCount);
		default:
			return snprintf(buf, len, "%.2f",
					(double)desc->pqm_global_attr[attr_id]);
		}
	}
	return -EINVAL;
}

/**
 * @brief Write a pqm device attribute.
 */
int write_pqm_attr(void *device, char *buf, uint32_t len,
		   const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct pqm_desc *desc;
	float value = atof(buf);

	if (!device)
		return -ENODEV;
	desc = device;
	if (attr_id < PQM_DEVICE_ATTR_NUMBER) {
		configChanged = true;

		switch (attr_id) {
		case NOMINAL_VOLTAGE:
			pqlibExample.exampleConfig.nominalVoltage = value;
			break;
		case VOLTAGE_SCALE:
			pqlibExample.exampleConfig.voltageScale = value;
			break;
		case CURRENT_SCALE:
			pqlibExample.exampleConfig.currentScale = value;
			break;
		case DIP_THRESHOLD:
			pqlibExample.exampleConfig.dipThreshold = value;
			break;
		case DIP_HYSTERESIS:
			pqlibExample.exampleConfig.dipHysteresis = value;
			break;
		case SWELL_THRESHOLD:
			pqlibExample.exampleConfig.swellThreshold = value;
			break;
		case SWELL_HYSTERESIS:
			pqlibExample.exampleConfig.swellHysteresis = value;
			break;
		case INTERP_THRESHOLD:
			pqlibExample.exampleConfig.intrpThreshold = value;
			break;
		case INTERP_HYSTERESIS:
			pqlibExample.exampleConfig.intrpHysteresis = value;
			break;
		case RVC_THRESHOLD:
			pqlibExample.exampleConfig.rvcThreshold = value;
			break;
		case RVC_HYSTERESIS:
			pqlibExample.exampleConfig.rvcHysteresis = value;
			break;
		case MSV_CARRIER_FREQUENCY:
			pqlibExample.exampleConfig.msvCarrierFrequency = value;
			break;
		case MSV_RECORDING_LENGTH:
			pqlibExample.exampleConfig.msvRecordingLength = value;
			break;
		case MSV_THRESHOLD:
			pqlibExample.exampleConfig.msvThreshold = value;
			break;
		case V_CONSEL:
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_v_consel_available); i++) {
				if (strcmp(buf, pqm_v_consel_available[i]) == 0) {
					pqlibExample.exampleConfig.vconsel = i;
					return len;
				}
			}
			return -EINVAL;
		case FLICKER_MODEL:
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_flicker_model_available); i++) {
				if (strcmp(buf, pqm_flicker_model_available[i]) == 0) {
					pqlibExample.exampleConfig.flickerModel = i;
					return len;
				}
			}
			return -EINVAL;
		case NOMINAL_FREQUENCY:
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available); i++) {
				if (strcmp(buf, pqm_nominal_frequency_available[i]) == 0) {
					pqlibExample.exampleConfig.nominalFrequency = value;
					return len;
				}
			}
			return -EINVAL;
		case PROCESS_DATA:
			configChanged = false;
			if (!strcmp(buf, "1"))
				processData = true;
			else
				processData = false;
			break;
		case CAL_TYPE:
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_calibration_type_available); i++) {
				if (strcmp(buf, pqm_calibration_type_available[i]) == 0) {
					pqlibExample.exampleConfig.calibrationType = i;
					configChanged = false;
					return len;
				}
			}
			return -EINVAL;
		case CAL_START:
			if (value == 1) {
				configChanged = false;
				calibrationCtx.input.nominal_current =
					pqlibExample.exampleConfig.calNominalCurrent;
				calibrationCtx.input.nominal_voltage =
					pqlibExample.exampleConfig.calNominalVoltage;
				calibrationCtx.input.offset_current =
					pqlibExample.exampleConfig.calOffsetCurrent;
				calibrationCtx.input.offset_voltage =
					pqlibExample.exampleConfig.calOffsetVoltage;
				calibrationCtx.input.current_pga_gain =
					pqlibExample.exampleConfig.currentPgaGain;
				calibrationCtx.input.voltage_pga_gain =
					pqlibExample.exampleConfig.voltagePgaGain;

				calibration_start(
					(CALIBRATION_TYPE)pqlibExample.exampleConfig.calibrationType,
					(CALIBRATION_PHASE)calibrationCtx.phase);
				pqlibExample.calibrationRequested = true;
				return len;
			}
			return -EINVAL;
		case CAL_PHASE:
			for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(pqm_calibration_phase_available); i++) {
				if (strcmp(buf, pqm_calibration_phase_available[i]) == 0) {
					calibrationCtx.phase = (CALIBRATION_PHASE)i;
					configChanged = false;
					return len;
				}
			}
			if (value >= 0 && value <= 2) {
				calibrationCtx.phase = (CALIBRATION_PHASE)(int)value;
				configChanged = false;
				return len;
			}
			return -EINVAL;
		case CAL_NOMINAL_CURRENT:
			pqlibExample.exampleConfig.calNominalCurrent = value;
			configChanged = false;
			break;
		case CAL_NOMINAL_VOLTAGE:
			pqlibExample.exampleConfig.calNominalVoltage = value;
			configChanged = false;
			break;
		case CAL_OFFSET_CURRENT:
			pqlibExample.exampleConfig.calOffsetCurrent = value;
			configChanged = false;
			break;
		case CAL_OFFSET_VOLTAGE:
			pqlibExample.exampleConfig.calOffsetVoltage = value;
			configChanged = false;
			break;
		case FLASH_CAL_SAVE:
			if (value == 1) {
				if (!flash_storage_is_initialized()) {
					if (flash_storage_init() != FLASH_STATUS_OK)
						return -EIO;
				}
				if (flash_save_calibration_channel(calibrationCtx.phase) != FLASH_STATUS_OK)
					return -EIO;
				calibration_clear_done();
			}
			break;
		case FLASH_CAL_LOAD:
			if (value == 1) {
				if (!flash_storage_is_initialized()) {
					if (flash_storage_init() != FLASH_STATUS_OK)
						return -EIO;
				}
				if (flash_load_and_apply_calibration() != FLASH_STATUS_OK)
					return -EIO;
			}
			break;
		case FLASH_CAL_ERASE:
			if (value == 1) {
				if (!flash_storage_is_initialized()) {
					if (flash_storage_init() != FLASH_STATUS_OK)
						return -EIO;
				}
				if (flash_erase_calibration() != FLASH_STATUS_OK)
					return -EIO;
			}
			break;
		case CLEAR_EVENTS:
			if ((int)value == 1) {
				pqlib_clear_accumulated_events();
			}
			configChanged = false;
			break;
		default:
			desc->pqm_global_attr[attr_id] = value;
		}
		return len;
	}
	return -EINVAL;
}

/**
 * @brief Read a channel attribute.
 */
int read_ch_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct pqm_desc *desc;
	char buffTmp[16];

	if (!device)
		return -ENODEV;
	desc = device;
	if (attr_id >= MAX_CH_ATTRS)
		return -EINVAL;

	/* PQLib output may be NULL before first measurement completes */
	if (!pqlibExample.output)
		return snprintf(buf, len, "0");

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (attr_id) {
		case CHAN_RMS:
			return snprintf(buf, len, "%.2f",
					(double)convert_rms_type(
						pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num].mag,
						pqlibExample.exampleConfig.voltageScale));
		case CHAN_ANGLE:
			switch (channel->ch_num) {
			case 0:
				return snprintf(buf, len, "%" PRIu16 "", (uint16_t)0);
			case 1:
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VB));
			case 2:
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VC));
			}
			break;
		case CHAN_HARMONICS:
			strcpy(buf, "");
			sprintf(buffTmp, "%f %f", (double)0.0f, (double)100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");
			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num]
						.harmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 2)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CHAN_INTER_HARMONICS:
			strcpy(buf, "");
			sprintf(buffTmp, "%f %f", (double)0.0f, (double)100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");
			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num]
						.interHarmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 2)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CHAN_SCALE:
			return snprintf(buf, len, "%.5f",
					(double)(pqlibExample.exampleConfig.voltageScale /
						 ((float)(RESAMPLED_WAVEFORM_FULL_SCALE))));
		case CHAN_OFFSET:
			return snprintf(buf, len, "%" PRIu32 "", (uint32_t)0);
		case CHAN_THD:
			return snprintf(buf, len, "%f",
					(double)((float)(pqlibExample.output->params1012Cycles
							 .voltageParams[channel->ch_num]
							 .thd)
						 / 100.0f));
		case CHAN_VOLTAGE_UNDER_DEV:
			return snprintf(buf, len, "%.2f",
					(double)convert_rms_type(
						pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num]
						.udod.uRmsUnder,
						pqlibExample.exampleConfig.voltageScale));
		case CHAN_VOLTAGE_OVER_DEV:
			return snprintf(buf, len, "%.2f",
					(double)convert_rms_type(
						pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num]
						.udod.uRmsOver,
						pqlibExample.exampleConfig.voltageScale));
		case CHAN_VOLTAGE_MAGNITUDE1012:
			return snprintf(buf, len, "%" PRIu16 "",
					pqlibExample.output->msvMagnitude[channel->ch_num].magnitude1012);
		case CHAN_VOLTAGE_MAX_MAGNITUDE:
			return snprintf(buf, len, "%" PRIu16 "",
					pqlibExample.output->msvMagnitude[channel->ch_num].maxMagnitude);
		case CHAN_ACTIVE_POWER:
			return snprintf(buf, len, "%.4f",
					(double)convert_power_type(
						pqlibExample.powerEnergy.activePower[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_ACTIVE_ENERGY:
			return snprintf(buf, len, "%.6f",
					(double)convert_energy_type(
						pqlibExample.powerEnergy.activeEnergyHi[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_REACTIVE_ENERGY:
			return snprintf(buf, len, "%.6f",
					(double)convert_energy_type(
						pqlibExample.powerEnergy.reactiveEnergyHi[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_POWER_FACTOR:
			return snprintf(buf, len, "%.4f",
					(double)compute_power_factor(
						pqlibExample.powerEnergy.activeEnergyHi[channel->ch_num],
						pqlibExample.powerEnergy.reactiveEnergyHi[channel->ch_num]));
		case CHAN_FUND_ACTIVE_POWER:
			return snprintf(buf, len, "%.4f",
					(double)convert_power_type(
						pqlibExample.powerEnergy.fundActivePower[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_FUND_ACTIVE_ENERGY:
			return snprintf(buf, len, "%.6f",
					(double)convert_energy_type(
						pqlibExample.powerEnergy.fundActiveEnergyHi[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_FUND_REACTIVE_ENERGY:
			return snprintf(buf, len, "%.6f",
					(double)convert_energy_type(
						pqlibExample.powerEnergy.fundReactiveEnergyHi[channel->ch_num],
						pqlibExample.exampleConfig.voltageScale *
						pqlibExample.exampleConfig.currentScale));
		case CHAN_DISPLACEMENT_PF:
			return snprintf(buf, len, "%.4f",
					(double)compute_power_factor(
						pqlibExample.powerEnergy.fundActiveEnergyHi[channel->ch_num],
						pqlibExample.powerEnergy.fundReactiveEnergyHi[channel->ch_num]));
		default:
			return snprintf(buf, len, "%" PRIu32 "",
					desc->pqm_ch_attr[channel->ch_num][attr_id]);
		}
		break;

	case IIO_CURRENT:
		switch (attr_id) {
		case CHAN_RMS:
			return snprintf(buf, len, "%.2f",
					(double)convert_rms_type(
						pqlibExample.output->params1012Cycles
						.currentParams[channel->ch_num].mag,
						pqlibExample.exampleConfig.currentScale));
		case CHAN_ANGLE:
			switch (channel->ch_num) {
			case 0:
				return snprintf(buf, len, "%" PRIu16 "", (uint16_t)0);
			case 1:
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IB));
			case 2:
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IC));
			default:
				return -EINVAL;
			}
		case CHAN_HARMONICS:
			strcpy(buf, "");
			sprintf(buffTmp, "%f %f", (double)0.0f, (double)100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");
			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.currentParams[channel->ch_num]
						.harmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 2)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CHAN_INTER_HARMONICS:
			strcpy(buf, "");
			sprintf(buffTmp, "%f %f", (double)0.0f, (double)100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");
			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					(double)convert_pct_type(
						pqlibExample.output->params1012Cycles
						.currentParams[channel->ch_num]
						.interHarmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 2)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CHAN_SCALE:
			return snprintf(buf, len, "%.5f",
					(double)(pqlibExample.exampleConfig.currentScale /
						 (float)(RESAMPLED_WAVEFORM_FULL_SCALE)));
		case CHAN_OFFSET:
			return snprintf(buf, len, "%" PRIu32 "", (uint32_t)0);
		case CHAN_THD:
			return snprintf(buf, len, "%f",
					(double)((float)(pqlibExample.output->params1012Cycles
							 .currentParams[channel->ch_num]
							 .thd)
						 / 100.0f));
		default:
			return snprintf(buf, len, "%" PRIu32 "",
					desc->pqm_ch_attr[channel->ch_num + VOLTAGE_CH_NUMBER][attr_id]);
		}

	case IIO_COUNT:
		switch (channel->ch_num) {
		case 0: /* Dips */
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "",
						accumulated_events.dipCount);
			case CHAN_EVENT_START_TIME:
				return prepara_string(EVENT_DIPS, CHAN_EVENT_START_TIME, buf,
						     accumulated_events.dipCount);
			case CHAN_EVENT_END_TIME:
				return prepara_string(EVENT_DIPS, CHAN_EVENT_END_TIME, buf,
						     accumulated_events.dipCount);
			case CHAN_EVENT_DURATION_IN_CYCL:
				return prepara_string(EVENT_DIPS, CHAN_EVENT_DURATION_IN_CYCL, buf,
						     accumulated_events.dipCount);
			case CHAN_EVENT_MIN_MAG:
				return prepara_string(EVENT_DIPS, CHAN_EVENT_MIN_MAG, buf,
						     accumulated_events.dipCount);
			default:
				return -EINVAL;
			}
		case 1: /* Swells */
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "",
						accumulated_events.swellCount);
			case CHAN_EVENT_START_TIME:
				return prepara_string(EVENT_SWELL, CHAN_EVENT_START_TIME, buf,
						     accumulated_events.swellCount);
			case CHAN_EVENT_END_TIME:
				return prepara_string(EVENT_SWELL, CHAN_EVENT_END_TIME, buf,
						     accumulated_events.swellCount);
			case CHAN_EVENT_DURATION_IN_CYCL:
				return prepara_string(EVENT_SWELL, CHAN_EVENT_DURATION_IN_CYCL, buf,
						     accumulated_events.swellCount);
			case CHAN_EVENT_MAX_MAG:
				return prepara_string(EVENT_SWELL, CHAN_EVENT_MAX_MAG, buf,
						     accumulated_events.swellCount);
			default:
				return -EINVAL;
			}
		case 2: /* RVC */
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "",
						accumulated_events.rvcCount);
			case CHAN_EVENT_START_TIME:
				return prepara_string(EVENT_RVC, CHAN_EVENT_START_TIME, buf,
						     accumulated_events.rvcCount);
			case CHAN_EVENT_END_TIME:
				return prepara_string(EVENT_RVC, CHAN_EVENT_END_TIME, buf,
						     accumulated_events.rvcCount);
			case CHAN_EVENT_DURATION_IN_CYCL:
				return prepara_string(EVENT_RVC, CHAN_EVENT_DURATION_IN_CYCL, buf,
						     accumulated_events.rvcCount);
			case CHAN_EVENT_DELTA_U_MAX:
				return prepara_string(EVENT_RVC, CHAN_EVENT_DELTA_U_MAX, buf,
						     accumulated_events.rvcCount);
			case CHAN_EVENT_DELTA_U_SS:
				return prepara_string(EVENT_RVC, CHAN_EVENT_DELTA_U_SS, buf,
						     accumulated_events.rvcCount);
			default:
				return -EINVAL;
			}
		case 3: /* Intrpt */
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "",
						accumulated_events.intrpCount);
			case CHAN_EVENT_START_TIME:
				return prepara_string(EVENT_INTRPS, CHAN_EVENT_START_TIME, buf,
						     accumulated_events.intrpCount);
			case CHAN_EVENT_END_TIME:
				return prepara_string(EVENT_INTRPS, CHAN_EVENT_END_TIME, buf,
						     accumulated_events.intrpCount);
			case CHAN_EVENT_DURATION_IN_CYCL:
				return prepara_string(EVENT_INTRPS, CHAN_EVENT_DURATION_IN_CYCL, buf,
						     accumulated_events.intrpCount);
			default:
				return -EINVAL;
			}
		default:
			return -EINVAL;
		}

	default:
		return -EINVAL;
	}
	return -EINVAL;
}

/* Stubs for buffer operations (phase 1: attributes only) */
int32_t update_pqm_channels(void *dev, uint32_t mask)
{
	struct pqm_desc *desc = dev;

	if (desc)
		desc->active_ch = mask;
	return 0;
}

int32_t close_pqm_channels(void *dev)
{
	return 0;
}

/*
 * Channel attribute arrays
 */
struct iio_attribute voltage_pqm_attributes[] = {
	{ .name = "rms", .show = read_ch_attr, .priv = CHAN_RMS },
	{ .name = "angle", .show = read_ch_attr, .priv = CHAN_ANGLE },
	{ .name = "harmonics", .show = read_ch_attr, .priv = CHAN_HARMONICS },
	{ .name = "inter_harmonics", .show = read_ch_attr, .priv = CHAN_INTER_HARMONICS },
	{ .name = "scale", .show = read_ch_attr, .priv = CHAN_SCALE },
	{ .name = "offset", .show = read_ch_attr, .priv = CHAN_OFFSET },
	{ .name = "thd", .show = read_ch_attr, .priv = CHAN_THD },
	{ .name = "raw", .show = read_ch_attr, .priv = CHAN_RAW },
	{ .name = "deviation_under", .show = read_ch_attr, .priv = CHAN_VOLTAGE_UNDER_DEV },
	{ .name = "deviation_over", .show = read_ch_attr, .priv = CHAN_VOLTAGE_OVER_DEV },
	{ .name = "pinst", .show = read_ch_attr, .priv = CHAN_VOLTAGE_PINST },
	{ .name = "pst", .show = read_ch_attr, .priv = CHAN_VOLTAGE_PST },
	{ .name = "plt", .show = read_ch_attr, .priv = CHAN_VOLTAGE_PLT },
	{ .name = "magnitude1012", .show = read_ch_attr, .priv = CHAN_VOLTAGE_MAGNITUDE1012 },
	{ .name = "maxMagnitude", .show = read_ch_attr, .priv = CHAN_VOLTAGE_MAX_MAGNITUDE },
	{ .name = "active_power", .show = read_ch_attr, .priv = CHAN_ACTIVE_POWER },
	{ .name = "active_energy", .show = read_ch_attr, .priv = CHAN_ACTIVE_ENERGY },
	{ .name = "reactive_energy", .show = read_ch_attr, .priv = CHAN_REACTIVE_ENERGY },
	{ .name = "power_factor", .show = read_ch_attr, .priv = CHAN_POWER_FACTOR },
	{ .name = "fund_active_power", .show = read_ch_attr, .priv = CHAN_FUND_ACTIVE_POWER },
	{ .name = "fund_active_energy", .show = read_ch_attr, .priv = CHAN_FUND_ACTIVE_ENERGY },
	{ .name = "fund_reactive_energy", .show = read_ch_attr, .priv = CHAN_FUND_REACTIVE_ENERGY },
	{ .name = "displacement_pf", .show = read_ch_attr, .priv = CHAN_DISPLACEMENT_PF },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute current_pqm_attributes[] = {
	{ .name = "rms", .show = read_ch_attr, .priv = CHAN_RMS },
	{ .name = "angle", .show = read_ch_attr, .priv = CHAN_ANGLE },
	{ .name = "harmonics", .show = read_ch_attr, .priv = CHAN_HARMONICS },
	{ .name = "inter_harmonics", .show = read_ch_attr, .priv = CHAN_INTER_HARMONICS },
	{ .name = "scale", .show = read_ch_attr, .priv = CHAN_SCALE },
	{ .name = "offset", .show = read_ch_attr, .priv = CHAN_OFFSET },
	{ .name = "thd", .show = read_ch_attr, .priv = CHAN_THD },
	{ .name = "raw", .show = read_ch_attr, .priv = CHAN_RAW },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute global_pqm_attributes[] = {
#if ADI_PQLIB_CFG_DISABLE_SYMM_COMP == 0
	{ .name = "u2", .show = read_pqm_attr, .priv = NEG_UNB_VOLTAGE_RATIO },
	{ .name = "u0", .show = read_pqm_attr, .priv = ZERO_UNB_VOLTAGE_RATIO },
	{ .name = "sneg_voltage", .show = read_pqm_attr, .priv = SNEG_VOLTAGE },
	{ .name = "spos_voltage", .show = read_pqm_attr, .priv = SPOS_VOLTAGE },
	{ .name = "szro_voltage", .show = read_pqm_attr, .priv = SZRO_VOLTAGE },
	{ .name = "sneg_current", .show = read_pqm_attr, .priv = SNEG_CURRENT },
	{ .name = "spos_current", .show = read_pqm_attr, .priv = SPOS_CURRENT },
	{ .name = "szro_current", .show = read_pqm_attr, .priv = SZRO_CURRENT },
	{ .name = "i2", .show = read_pqm_attr, .priv = NEG_UNB_CURRENT_RATIO },
	{ .name = "i0", .show = read_pqm_attr, .priv = ZERO_UNB_CURRENT_RATIO },
	{ .name = "sneg_voltage_angle", .show = read_pqm_attr, .priv = SNEG_VOLTAGE_ANGLE },
	{ .name = "spos_voltage_angle", .show = read_pqm_attr, .priv = SPOS_VOLTAGE_ANGLE },
	{ .name = "szro_voltage_angle", .show = read_pqm_attr, .priv = SZRO_VOLTAGE_ANGLE },
	{ .name = "sneg_current_angle", .show = read_pqm_attr, .priv = SNEG_CURRENT_ANGLE },
	{ .name = "spos_current_angle", .show = read_pqm_attr, .priv = SPOS_CURRENT_ANGLE },
	{ .name = "szro_current_angle", .show = read_pqm_attr, .priv = SZRO_CURRENT_ANGLE },
#endif
	{ .name = "nominal_voltage", .show = read_pqm_attr, .store = write_pqm_attr, .priv = NOMINAL_VOLTAGE },
	{ .name = "voltage_scale", .show = read_pqm_attr, .store = write_pqm_attr, .priv = VOLTAGE_SCALE },
	{ .name = "current_scale", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CURRENT_SCALE },
	{ .name = "i_consel_en", .show = read_pqm_attr, .store = write_pqm_attr, .priv = I_CONSEL_ENABLE },
	{ .name = "dip_threshold", .show = read_pqm_attr, .store = write_pqm_attr, .priv = DIP_THRESHOLD },
	{ .name = "dip_hysteresis", .show = read_pqm_attr, .store = write_pqm_attr, .priv = DIP_HYSTERESIS },
	{ .name = "swell_threshold", .show = read_pqm_attr, .store = write_pqm_attr, .priv = SWELL_THRESHOLD },
	{ .name = "swell_hysteresis", .show = read_pqm_attr, .store = write_pqm_attr, .priv = SWELL_HYSTERESIS },
	{ .name = "intrp_threshold", .show = read_pqm_attr, .store = write_pqm_attr, .priv = INTERP_THRESHOLD },
	{ .name = "intrp_hysteresis", .show = read_pqm_attr, .store = write_pqm_attr, .priv = INTERP_HYSTERESIS },
	{ .name = "rvc_threshold", .show = read_pqm_attr, .store = write_pqm_attr, .priv = RVC_THRESHOLD },
	{ .name = "rvc_hysteresis", .show = read_pqm_attr, .store = write_pqm_attr, .priv = RVC_HYSTERESIS },
	{ .name = "msv_carrier_frequency", .show = read_pqm_attr, .store = write_pqm_attr, .priv = MSV_CARRIER_FREQUENCY },
	{ .name = "msv_record_length", .show = read_pqm_attr, .store = write_pqm_attr, .priv = MSV_RECORDING_LENGTH },
	{ .name = "msv_threshold", .show = read_pqm_attr, .store = write_pqm_attr, .priv = MSV_THRESHOLD },
	{ .name = "sampling_frequency", .show = read_pqm_attr, .store = write_pqm_attr, .priv = SAMPLING_FREQUENCY },
	{ .name = "v_consel", .show = read_pqm_attr, .store = write_pqm_attr, .priv = V_CONSEL },
	{ .name = "v_consel_available", .show = read_pqm_attr, .priv = V_CONSEL_AVAILABLE },
	{ .name = "flicker_model", .show = read_pqm_attr, .store = write_pqm_attr, .priv = FLICKER_MODEL },
	{ .name = "flicker_model_available", .show = read_pqm_attr, .priv = FLICKER_MODEL_AVAILABLE },
	{ .name = "nominal_frequency", .show = read_pqm_attr, .store = write_pqm_attr, .priv = NOMINAL_FREQUENCY },
	{ .name = "nominal_frequency_available", .show = read_pqm_attr, .priv = NOMINAL_FREQUENCY_AVAILABLE },
	{ .name = "process_data", .show = read_pqm_attr, .store = write_pqm_attr, .priv = PROCESS_DATA },
	{ .name = "fw_version", .show = read_pqm_attr, .priv = FW_VERSION_NR },
	/* Calibration attributes */
	{ .name = "cal_type", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_TYPE },
	{ .name = "cal_type_available", .show = read_pqm_attr, .priv = CAL_TYPE_AVAILABLE },
	{ .name = "cal_start", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_START },
	{ .name = "cal_status", .show = read_pqm_attr, .priv = CAL_STATUS },
	{ .name = "cal_phase", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_PHASE },
	{ .name = "cal_phase_available", .show = read_pqm_attr, .priv = CAL_PHASE_AVAILABLE },
	{ .name = "cal_nominal_current", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_NOMINAL_CURRENT },
	{ .name = "cal_nominal_voltage", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_NOMINAL_VOLTAGE },
	{ .name = "cal_offset_current", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_OFFSET_CURRENT },
	{ .name = "cal_offset_voltage", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CAL_OFFSET_VOLTAGE },
	{ .name = "cal_gain_i_error_before", .show = read_pqm_attr, .priv = CAL_GAIN_I_ERROR_BEFORE },
	{ .name = "cal_gain_v_error_before", .show = read_pqm_attr, .priv = CAL_GAIN_V_ERROR_BEFORE },
	{ .name = "cal_gain_i_error_after", .show = read_pqm_attr, .priv = CAL_GAIN_I_ERROR_AFTER },
	{ .name = "cal_gain_v_error_after", .show = read_pqm_attr, .priv = CAL_GAIN_V_ERROR_AFTER },
	{ .name = "cal_offset_i_error_before", .show = read_pqm_attr, .priv = CAL_OFFSET_I_ERROR_BEFORE },
	{ .name = "cal_offset_v_error_before", .show = read_pqm_attr, .priv = CAL_OFFSET_V_ERROR_BEFORE },
	{ .name = "cal_offset_i_error_after", .show = read_pqm_attr, .priv = CAL_OFFSET_I_ERROR_AFTER },
	{ .name = "cal_offset_v_error_after", .show = read_pqm_attr, .priv = CAL_OFFSET_V_ERROR_AFTER },
	/* Flash storage attributes */
	{ .name = "flash_cal_save", .show = read_pqm_attr, .store = write_pqm_attr, .priv = FLASH_CAL_SAVE },
	{ .name = "flash_cal_load", .show = read_pqm_attr, .store = write_pqm_attr, .priv = FLASH_CAL_LOAD },
	{ .name = "flash_cal_erase", .show = read_pqm_attr, .store = write_pqm_attr, .priv = FLASH_CAL_ERASE },
	{ .name = "flash_status", .show = read_pqm_attr, .priv = FLASH_STAT },
	{ .name = "flash_cal_data", .show = read_pqm_attr, .priv = FLASH_CAL_DATA },
	/* PTP status attributes (read-only) */
	{ .name = "ptp_state", .show = read_pqm_attr, .priv = PTP_STATE },
	{ .name = "ptp_offset_ns", .show = read_pqm_attr, .priv = PTP_OFFSET_NS },
	{ .name = "ptp_mean_delay_ns", .show = read_pqm_attr, .priv = PTP_MEAN_DELAY_NS },
	{ .name = "ptp_converged", .show = read_pqm_attr, .priv = PTP_CONVERGED },
	/* Measurement PTP timestamps (read-only, nanosecond resolution) */
	{ .name = "measurement_ptp_timestamp", .show = read_pqm_attr, .priv = MEASUREMENT_PTP_TIMESTAMP },
	{ .name = "measurement_1012_ptp_timestamp", .show = read_pqm_attr, .priv = MEASUREMENT_1012_PTP_TIMESTAMP },
	{ .name = "event_time_base", .show = read_pqm_attr, .priv = EVENT_TIME_BASE },
	{ .name = "clear_events", .show = read_pqm_attr, .store = write_pqm_attr, .priv = CLEAR_EVENTS },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute event_pqm_dips_attribute[] = {
	EVENT_COMMON_ATTR,
	{ .name = "minMag", .show = read_ch_attr, .priv = CHAN_EVENT_MIN_MAG },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute event_pqm_swell_attribute[] = {
	EVENT_COMMON_ATTR,
	{ .name = "maxMag", .show = read_ch_attr, .priv = CHAN_EVENT_MAX_MAG },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute event_pqm_rvc_attribute[] = {
	EVENT_COMMON_ATTR,
	{ .name = "deltaUmax", .show = read_ch_attr, .priv = CHAN_EVENT_DELTA_U_MAX },
	{ .name = "deltaUss", .show = read_ch_attr, .priv = CHAN_EVENT_DELTA_U_SS },
	END_ATTRIBUTES_ARRAY,
};

struct iio_attribute event_pqm_intrpr_attribute[] = {
	EVENT_COMMON_ATTR,
	END_ATTRIBUTES_ARRAY,
};

struct scan_type pqm_scan_type = {
	.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian = true,
};

static struct iio_channel iio_pqm_channels[] = {
	PQM_CURRENT_CHANNEL(0, 1, "ia"),
	PQM_VOLTAGE_CHANNEL(0, 2, "ua"),
	PQM_CURRENT_CHANNEL(1, 3, "ib"),
	PQM_VOLTAGE_CHANNEL(1, 4, "ub"),
	PQM_CURRENT_CHANNEL(2, 5, "ic"),
	PQM_VOLTAGE_CHANNEL(2, 6, "uc"),
	PQM_CURRENT_CHANNEL(3, 7, "in"),
	PQM_EVENT_CHANNEL(0, 8, "dips", event_pqm_dips_attribute),
	PQM_EVENT_CHANNEL(1, 9, "swells", event_pqm_swell_attribute),
	PQM_EVENT_CHANNEL(2, 10, "rvc", event_pqm_rvc_attribute),
	PQM_EVENT_CHANNEL(3, 11, "intrpt", event_pqm_intrpr_attribute),
};

struct iio_device pqm_iio_descriptor = {
	.num_ch = TOTAL_PQM_CHANNELS,
	.channels = iio_pqm_channels,
	.attributes = global_pqm_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = update_pqm_channels,
	.post_disable = close_pqm_channels,
};
