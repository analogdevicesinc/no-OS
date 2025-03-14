/*******************************************************************************
 *   @file   iio_pqm.c
 *   @brief  IIO pqm interfacing functionalities
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#define PQM_VOLTAGE_CHANNEL(_idx, _scan_idx, _name)                            \
  {                                                                            \
    .name = _name, .ch_type = IIO_VOLTAGE, .channel = _idx,                    \
    .scan_index = _scan_idx, .indexed = true, .scan_type = &pqm_scan_type,     \
    .attributes = voltage_pqm_attributes, .ch_out = false                      \
  }

#define PQM_CURRENT_CHANNEL(_idx, _scan_idx, _name)                            \
  {                                                                            \
    .name = _name, .ch_type = IIO_CURRENT, .channel = _idx,                    \
    .scan_index = _scan_idx, .indexed = true, .scan_type = &pqm_scan_type,     \
    .attributes = current_pqm_attributes, .ch_out = false                      \
  }

#define PQM_EVENT_CHANNEL(_idx, _scan_idx, _name, _attrb_def)			\
{										\
    .name = _name, .ch_type = IIO_COUNT, .channel = _idx,                       \
    .scan_index = _scan_idx, .indexed = true,                                   \
    .scan_type = 0, .attributes = _attrb_def,                      		\
    .ch_out = true                                                              \
}

#define EVENT_COMMON_ATTR                                                       \
{                                                                               \
        .name = "countEvent",                                                   \
        .show = read_ch_attr,                                                   \
        .priv = CHAN_EVENT_COUNT,                                               \
},                                                                              \
{                                                                               \
        .name = "startTime",                                                    \
        .show = read_ch_attr,                                                   \
        .priv = CHAN_EVENT_START_TIME,                                          \
},                                                                              \
{                                                                               \
        .name = "endTime",                                                      \
        .show = read_ch_attr,                                                   \
        .priv = CHAN_EVENT_END_TIME,                                            \
},                                                                              \
{                                                                               \
        .name = "durationInCycles",                                             \
        .show = read_ch_attr,                                                   \
        .priv = CHAN_EVENT_DURATION_IN_CYCL,                                    \
}

volatile bool configChanged = false;
volatile bool processData = true;

/**
 * @brief utility function for computing next upcoming channel.
 * @param ch_mask - active channels .
 * @param last_idx -  previous index.
 * @param new_idx - upcoming channel index, return param.
 * @return 1 if there are more channels, 0 if done.
 */
static bool get_next_ch_idx(uint32_t ch_mask, uint32_t last_idx,
			    uint32_t *new_idx)
{
	last_idx++;
	ch_mask >>= last_idx;
	if (!ch_mask) {
		*new_idx = -1;
		return 0;
	}
	while (!(ch_mask & 1)) {
		last_idx++;
		ch_mask >>= 1;
	}
	*new_idx = last_idx;

	return 1;
}

/**
 * @brief Read a pqm device attribute.
 *
 * @param device    - The iio device structure
 * @param buf       - Buffer to be filled with requested data
 * @param len 	    - Length of the received command buffer in bytes
 * @param channel   - Command channel info
 * @param attr_id   - attribute descriptor
 * @return          - The length of the buffer in case of success, negative
 * value otherwise
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
					pqlibExample.exampleConfig.nominalVoltage);
		case VOLTAGE_SCALE:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.voltageScale);
		case CURRENT_SCALE:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.currentScale);
		case DIP_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.dipThreshold);
		case DIP_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.dipHysteresis);
		case SWELL_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.swellThreshold);
		case SWELL_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.swellHysteresis);
		case INTERP_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.intrpThreshold);
		case INTERP_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.intrpHysteresis);
		case RVC_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.rvcThreshold);
		case RVC_HYSTERESIS:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.rvcHysteresis);
		case MSV_CARRIER_FREQUENCY:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.msvCarrierFrequency);
		case MSV_RECORDING_LENGTH:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.msvRecordingLength);
		case MSV_THRESHOLD:
			return snprintf(buf, len, "%.2f",
					pqlibExample.exampleConfig.msvThreshold);
		case V_CONSEL:
			return snprintf(buf, len,
					pqm_v_consel_available[pqlibExample.exampleConfig.vconsel]);
		case V_CONSEL_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_v_consel_available); i++) {
				strcat(buf, pqm_v_consel_available[i]);
				if (i != NO_OS_ARRAY_SIZE(pqm_v_consel_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case FLICKER_MODEL:
			return snprintf(buf, len,
					pqm_flicker_model_available[pqlibExample.exampleConfig.flickerModel]);
		case FLICKER_MODEL_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_flicker_model_available); i++) {
				strcat(buf, pqm_flicker_model_available[i]);
				if (i != NO_OS_ARRAY_SIZE(pqm_flicker_model_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case NOMINAL_FREQUENCY:
			return snprintf(buf, len, "%" PRIu32 "",
					pqlibExample.exampleConfig.nominalFrequency);
		case NOMINAL_FREQUENCY_AVAILABLE:
			strcpy(buf, "");
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available); i++) {
				strcat(buf, pqm_nominal_frequency_available[i]);
				if (i != NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available) - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case PROCESS_DATA:
			return snprintf(buf, len, "%s", (processData) ? "1" : "0");
		case FW_VERSION_NR:
			return snprintf(buf, len, "%.1f", FW_VERSION);
		default:
			return snprintf(buf, len, "%.2f", desc->pqm_global_attr[attr_id]);
		}
	} else {
		return -EINVAL;
	}
}

/**
 * @brief Write a pqm device attribute.
 *
 * @param device    - The iio device structure
 * @param buf       - Buffer to be filled with requested data
 * @param len 	    - Length of the received command buffer in bytes
 * @param channel   - Command channel info
 * @param attr_id   - attribute descriptor
 * @return          - The length of the buffer in case of success, negative
 * value otherwise
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
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_v_consel_available); i++) {
				if (strcmp(buf, pqm_v_consel_available[i]) == 0) {
					pqlibExample.exampleConfig.vconsel = i;
					return len;
				}
			}
			return -EINVAL;
		case FLICKER_MODEL:
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_flicker_model_available); i++) {
				if (strcmp(buf, pqm_flicker_model_available[i]) == 0) {
					pqlibExample.exampleConfig.flickerModel = i;
					return len;
				}
			}
			return -EINVAL;
		case NOMINAL_FREQUENCY:
			for (int i = 0; i < NO_OS_ARRAY_SIZE(pqm_nominal_frequency_available); i++) {
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
			else {
				processData = false;
				// Empty waveform iio buffer
				int tmp_ret;
				uint8_t tmp_buff[ADI_PQLIB_WAVEFORM_BLOCK_SIZE
						 * ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS
						 * sizeof(uint16_t)];
				do {
					tmp_ret = no_os_cb_read(pqlibExample.no_os_cb_desc, tmp_buff,
								ADI_PQLIB_WAVEFORM_BLOCK_SIZE
								* ADI_PQLIB_TOTAL_WAVEFORM_CHANNELS
								* sizeof(uint16_t));
				} while (!tmp_ret);
			}
		default:
			desc->pqm_global_attr[attr_id] = value;
		}
		return len;
	} else {
		return -EINVAL;
	}
}

/**
 * @brief Read a channel attribute.
 *
 * @param device    - The iio device structure
 * @param buf       - Buffer to be filled with requested data
 * @param len 	    - Length of the received command buffer in bytes
 * @param channel   - Command channel info
 * @param attr_id   - attribute descriptor
 * @return          - The length of the buffer in case of success, negative
 * value otherwise
 */
int read_ch_attr(void *device, char *buf, uint32_t len,
		 const struct iio_ch_info *channel, intptr_t attr_id)
{
	struct pqm_desc *desc;
	char buffTmp[10];

	if (!device)
		return -ENODEV;
	desc = device;
	if (attr_id >= MAX_CH_ATTRS) {
		return -EINVAL;
	}

	switch (channel->type) {
	case IIO_VOLTAGE:
		switch (attr_id) {
		case CHAN_RMS:
			return snprintf(
				       buf, len, "%.2f",
				       convert_rms_type(pqlibExample.output->params1012Cycles
							.voltageParams[channel->ch_num]
							.mag,
							pqlibExample.exampleConfig.voltageScale));
		case CHAN_ANGLE:
			switch (channel->ch_num) {
			case 0: // VA -> always 0
				return snprintf(buf, len, "%" PRIu16 "", 0);
			case 1: // VB -> ANGL_VA_VB
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VB));
			case 2: // VC -> ANGL_VA_VC
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_VA_VC));
			}

		case CHAN_HARMONICS:
			strcpy(buf, "");
			// Adding fundamental waveform, 0% & 100% always
			sprintf(buffTmp, "%f %f", 0.0f, 100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");

			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					convert_pct_type(pqlibExample.output->params1012Cycles
							 .voltageParams[channel->ch_num]
							 .harmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 1)
					strcat(buf, " ");
			}
			return strlen(buf);

		case CHAN_INTER_HARMONICS:
			strcpy(buf, "");
			// Adding fundamental waveform, 0% & 100% always
			sprintf(buffTmp, "%f %f", 0.0f, 100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");

			for (int i = 0; i < PQLIB_MAX_INTER_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					convert_pct_type(pqlibExample.output->params1012Cycles
							 .voltageParams[channel->ch_num]
							 .interHarmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_INTER_HARMONICS - 1)
					strcat(buf, " ");
			}
			return strlen(buf);

		case CHAN_SCALE:
			return snprintf(buf, len, "%.5f",
					(pqlibExample.exampleConfig.voltageScale /
					 ((float)(RESAMPLED_WAVEFORM_FULL_SCALE))));

		case CHAN_OFFSET:
			return snprintf(buf, len, "%" PRIu32 "", 0);
		case CHAN_THD:
			return snprintf(
				       buf, len, "%f",
				       ((float)(pqlibExample.output->params1012Cycles
						.voltageParams[channel->ch_num]
						.thd)
					/ 100.0f));

		case CHAN_VOLTAGE_UNDER_DEV:
			return snprintf(
				       buf, len, "%.2f",
				       convert_rms_type(pqlibExample.output->params1012Cycles
							.voltageParams[channel->ch_num]
							.udod.uRmsUnder,
							pqlibExample.exampleConfig.voltageScale));

		case CHAN_VOLTAGE_OVER_DEV:
			return snprintf(
				       buf, len, "%.2f",
				       convert_rms_type(pqlibExample.output->params1012Cycles
							.voltageParams[channel->ch_num]
							.udod.uRmsOver,
							pqlibExample.exampleConfig.voltageScale));

		case CHAN_VOLTAGE_MAGNITUDE1012:
			return snprintf(buf, len, "%" PRIu16 "",
					pqlibExample.output->msvMagnitude[channel->ch_num].magnitude1012);

		case CHAN_VOLTAGE_MAX_MAGNITUDE:
			return snprintf(buf, len, "%" PRIu16 "",
					pqlibExample.output->msvMagnitude[channel->ch_num].maxMagnitude);

		default:
			return snprintf(buf, len, "%" PRIu32 "",
					desc->pqm_ch_attr[channel->ch_num][attr_id]);
		}

	case IIO_CURRENT:
		switch (attr_id) {
		case CHAN_RMS:
			return snprintf(
				       buf, len, "%.2f",
				       convert_rms_type(pqlibExample.output->params1012Cycles
							.currentParams[channel->ch_num]
							.mag,
							pqlibExample.exampleConfig.currentScale));

		case CHAN_ANGLE:
			switch (channel->ch_num) {
			case 0: // IA -> always 0
				return snprintf(buf, len, "%" PRIu16 "", 0);
			case 1: // IB -> ANGL_IA_IB
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IB));
			case 2: // IC -> ANGLE_IA_IC
				return snprintf(buf, len, "%" PRIu16 "",
						convert_angle_type(pqlibExample.inputCycle.ANGL_IA_IC));
			default:
				return -EINVAL;
			}

		case CHAN_HARMONICS:
			strcpy(buf, "");
			// Adding fundamental waveform, 0% & 100% always
			sprintf(buffTmp, "%f %f", 0.0f, 100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");

			for (int i = 0; i < PQLIB_MAX_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					convert_pct_type(pqlibExample.output->params1012Cycles
							 .currentParams[channel->ch_num]
							 .harmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_HARMONICS - 1)
					strcat(buf, " ");
			}
			return strlen(buf);

		case CHAN_INTER_HARMONICS:
			strcpy(buf, "");
			// Adding fundamental waveform, 0% & 100% always
			sprintf(buffTmp, "%f %f", 0.0f, 100.0f);
			strcat(buf, buffTmp);
			strcat(buf, " ");

			for (int i = 0; i < PQLIB_MAX_INTER_HARMONICS - 1; i++) {
				sprintf(buffTmp, "%f",
					convert_pct_type(pqlibExample.output->params1012Cycles
							 .currentParams[channel->ch_num]
							 .interHarmonics[i]));
				strcat(buf, buffTmp);
				if (i != PQLIB_MAX_INTER_HARMONICS - 1)
					strcat(buf, " ");
			}
			return strlen(buf);
		case CHAN_SCALE:
			return snprintf(buf, len, "%.5f",
					(pqlibExample.exampleConfig.currentScale /
					 (float)(RESAMPLED_WAVEFORM_FULL_SCALE)));

		case CHAN_OFFSET:
			return snprintf(buf, len, "%" PRIu32 "", 0);

		case CHAN_THD:
			return snprintf(
				       buf, len, "%f",
				       (float)(pqlibExample.output->params1012Cycles
					       .currentParams[channel->ch_num]
					       .thd)
				       / 100.0f);

		default:
			return snprintf(
				       buf, len, "%" PRIu32 "",
				       desc->pqm_ch_attr[channel->ch_num + VOLTAGE_CH_NUMBER][attr_id]);
		}
	case IIO_COUNT:
		switch (channel->ch_num) {
		case 0: // Dips
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "", pqlibExample.output->events.dipCount);
			case CHAN_EVENT_START_TIME:
				return (prepara_string(EVENT_DIPS, CHAN_EVENT_START_TIME, buf));
			case CHAN_EVENT_END_TIME:
				return (prepara_string(EVENT_DIPS, CHAN_EVENT_END_TIME, buf));
			case CHAN_EVENT_DURATION_IN_CYCL:
				return (prepara_string(EVENT_DIPS, CHAN_EVENT_DURATION_IN_CYCL, buf));
			case CHAN_EVENT_MIN_MAG:
				return (prepara_string(EVENT_DIPS, CHAN_EVENT_MIN_MAG, buf));
			default:
				return -EINVAL;
			}
		case 1: // Swells
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "", pqlibExample.output->events.swellCount);
			case CHAN_EVENT_START_TIME:
				return (prepara_string(EVENT_SWELL, CHAN_EVENT_START_TIME, buf));
			case CHAN_EVENT_END_TIME:
				return (prepara_string(EVENT_SWELL, CHAN_EVENT_END_TIME, buf));
			case CHAN_EVENT_DURATION_IN_CYCL:
				return (prepara_string(EVENT_SWELL, CHAN_EVENT_DURATION_IN_CYCL, buf));
			case CHAN_EVENT_MAX_MAG:
				return (prepara_string(EVENT_SWELL, CHAN_EVENT_MAX_MAG, buf));
			default:
				return -EINVAL;
			}
		case 2: // RVC
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "", pqlibExample.output->events.rvcCount);
			case CHAN_EVENT_START_TIME:
				return (prepara_string(EVENT_RVC, CHAN_EVENT_START_TIME, buf));
			case CHAN_EVENT_END_TIME:
				return (prepara_string(EVENT_RVC, CHAN_EVENT_END_TIME, buf));
			case CHAN_EVENT_DURATION_IN_CYCL:
				return (prepara_string(EVENT_RVC, CHAN_EVENT_DURATION_IN_CYCL, buf));
			case CHAN_EVENT_DELTA_U_MAX:
				return (prepara_string(EVENT_RVC, CHAN_EVENT_DELTA_U_MAX, buf));
			case CHAN_EVENT_DELTA_U_SS:
				return (prepara_string(EVENT_RVC, CHAN_EVENT_DELTA_U_SS, buf));
			default:
				return -EINVAL;
			}
		case 3: // Intrpt
			switch (attr_id) {
			case CHAN_EVENT_COUNT:
				return snprintf(buf, len, "%" PRIu8 "", pqlibExample.output->events.intrpCount);
			case CHAN_EVENT_START_TIME:
				return (prepara_string(EVENT_INTRPS, CHAN_EVENT_START_TIME, buf));
			case CHAN_EVENT_END_TIME:
				return (prepara_string(EVENT_INTRPS, CHAN_EVENT_END_TIME, buf));
			case CHAN_EVENT_DURATION_IN_CYCL:
				return (prepara_string(EVENT_INTRPS, CHAN_EVENT_DURATION_IN_CYCL, buf));
			default:
				return -EINVAL;
			}
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}

/**
 * @brief function for reading samples from the device.
 * @param dev_data  - The iio device data structure.
 * @return the number of read samples.
 */
int32_t read_samples(struct iio_device_data *dev_data)
{
	if (!dev_data)
		return -ENODEV;
	if (pqlibExample.no_os_cb_desc == NULL) {
		pqlibExample.no_os_cb_desc = dev_data->buffer->buf;
	}

	return dev_data->buffer->size / dev_data->buffer->bytes_per_scan;
}

/**
 * @brief Handles trigger: reads one data-set and writes it to the buffer.
 *
 * @param dev_data  - The iio device data structure.
 *
 * @return ret - Result of the handling procedure. In case of success, the size
 * 				 of the written data is returned.
 */
int32_t pqm_trigger_handler(struct iio_device_data *dev_data)
{
	struct pqm_desc *desc;
	uint32_t k = 0;
	uint32_t ch = -1;
	uint32_t buff[TOTAL_PQM_CHANNELS];
	static uint32_t i = 0;
	uint32_t *ch_buf_ptr;

	if (!dev_data)
		return -EINVAL;

	desc = (struct pqm_desc *)dev_data->dev;

	if (desc->ext_buff == NULL) {
		return iio_buffer_push_scan(dev_data->buffer, buff);
	}

	while (get_next_ch_idx(desc->active_ch, ch, &ch)) {
		ch_buf_ptr = (uint32_t *)desc->ext_buff + (ch * desc->ext_buff_len);
		buff[k++] = ch_buf_ptr[i];
	}
	if (i == (desc->ext_buff_len - 1))
		i = 0;
	else
		i++;

	return iio_buffer_push_scan(dev_data->buffer, buff);
}

struct iio_attribute voltage_pqm_attributes[] = {
	{
		.name = "rms",
		.show = read_ch_attr,
		.priv = CHAN_RMS,
	},
	{
		.name = "angle",
		.show = read_ch_attr,
		.priv = CHAN_ANGLE,
	},
	{
		.name = "harmonics",
		.show = read_ch_attr,
		.priv = CHAN_HARMONICS,
	},
	{
		.name = "inter_harmonics",
		.show = read_ch_attr,
		.priv = CHAN_INTER_HARMONICS,
	},
	{
		.name = "scale",
		.show = read_ch_attr,
		.priv = CHAN_SCALE,
	},
	{
		.name = "offset",
		.show = read_ch_attr,
		.priv = CHAN_OFFSET,
	},
	{
		.name = "thd",
		.show = read_ch_attr,
		.priv = CHAN_THD,
	},
	{
		.name = "raw",
		.show = read_ch_attr,
		.priv = CHAN_RAW,
	},
	{
		.name = "deviation_under",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_UNDER_DEV,
	},
	{
		.name = "deviation_over",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_OVER_DEV,
	},
	{
		.name = "pinst",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_PINST,
	},
	{
		.name = "pst",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_PST,
	},
	{
		.name = "plt",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_PLT,
	},
	{
		.name = "magnitude1012",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_MAGNITUDE1012,
	},
	{
		.name = "maxMagnitude",
		.show = read_ch_attr,
		.priv = CHAN_VOLTAGE_MAX_MAGNITUDE,
	},


	END_ATTRIBUTES_ARRAY,
}; // voltage channel attributes

struct iio_attribute current_pqm_attributes[] = {

	{
		.name = "rms",
		.show = read_ch_attr,
		.priv = CHAN_RMS,
	},
	{
		.name = "angle",
		.show = read_ch_attr,
		.priv = CHAN_ANGLE,
	},
	{
		.name = "harmonics",
		.show = read_ch_attr,
		.priv = CHAN_HARMONICS,
	},
	{
		.name = "inter_harmonics",
		.show = read_ch_attr,
		.priv = CHAN_INTER_HARMONICS,
	},
	{
		.name = "scale",
		.show = read_ch_attr,
		.priv = CHAN_SCALE,
	},
	{
		.name = "offset",
		.show = read_ch_attr,
		.priv = CHAN_OFFSET,
	},
	{
		.name = "thd",
		.show = read_ch_attr,
		.priv = CHAN_THD,
	},
	{
		.name = "raw",
		.show = read_ch_attr,
		.priv = CHAN_RAW,
	},

	END_ATTRIBUTES_ARRAY,
}; // current channel attributes

struct iio_attribute global_pqm_attributes[] = {
	{
		.name = "u2",
		.show = read_pqm_attr,
		.priv = ATTR_U2,
	},
	{
		.name = "u0",
		.show = read_pqm_attr,
		.priv = ATTR_U0,
	},
	{
		.name = "sneg_voltage",
		.show = read_pqm_attr,
		.priv = SNEG_VOLTAGE,
	},
	{
		.name = "spos_voltage",
		.show = read_pqm_attr,
		.priv = SPOS_VOLTAGE,
	},
	{
		.name = "szro_voltage",
		.show = read_pqm_attr,
		.priv = SZRO_VOLTAGE,
	},
	{
		.name = "i2",
		.show = read_pqm_attr,
		.priv = ATTR_I2,
	},
	{
		.name = "i0",
		.show = read_pqm_attr,
		.priv = ATTR_I0,
	},
	{
		.name = "sneg_current",
		.show = read_pqm_attr,
		.priv = SNEG_CURRENT,
	},
	{
		.name = "spos_current",
		.show = read_pqm_attr,
		.priv = SPOS_CURRENT,
	},
	{
		.name = "szro_current",
		.show = read_pqm_attr,
		.priv = SZRO_CURRENT,
	},
	{
		.name = "nominal_voltage",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = NOMINAL_VOLTAGE,
	},
	{
		.name = "voltage_scale",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = VOLTAGE_SCALE,
	},
	{
		.name = "current_scale",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = CURRENT_SCALE,
	},
	{
		.name = "i_consel_en",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = I_CONSEL_ENABLE,
	},
	{
		.name = "dip_threshold",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = DIP_THRESHOLD,
	},
	{
		.name = "dip_hysteresis",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = DIP_HYSTERESIS,
	},
	{
		.name = "swell_threshold",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = SWELL_THRESHOLD,
	},
	{
		.name = "swell_hysteresis",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = SWELL_HYSTERESIS,
	},
	{
		.name = "intrp_threshold",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = INTERP_THRESHOLD,
	},
	{
		.name = "intrp_hysteresis",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = INTERP_HYSTERESIS,
	},
	{
		.name = "rvc_threshold",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = RVC_THRESHOLD,
	},
	{
		.name = "rvc_hysteresis",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = RVC_HYSTERESIS,
	},
	{
		.name = "msv_carrier_frequency",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = MSV_CARRIER_FREQUENCY,
	},
	{
		.name = "msv_record_length",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = MSV_RECORDING_LENGTH,
	},
	{
		.name = "msv_threshold",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = MSV_THRESHOLD,
	},
	{
		.name = "sampling_frequency",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = SAMPLING_FREQUENCY,
	},
	{
		.name = "v_consel",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = V_CONSEL,
	},
	{
		.name = "v_consel_available",
		.show = read_pqm_attr,
		.priv = V_CONSEL_AVAILABLE,
	},
	{
		.name = "flicker_model",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = FLICKER_MODEL,
	},
	{
		.name = "flicker_model_available",
		.show = read_pqm_attr,
		.priv = FLICKER_MODEL_AVAILABLE,
	},
	{
		.name = "nominal_frequency",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = NOMINAL_FREQUENCY,
	},
	{
		.name = "nominal_frequency_available",
		.show = read_pqm_attr,
		.priv = NOMINAL_FREQUENCY_AVAILABLE,
	},
	{
		.name = "process_data",
		.show = read_pqm_attr,
		.store = write_pqm_attr,
		.priv = PROCESS_DATA,
	},
	{
		.name = "fw_version",
		.show = read_pqm_attr,
		.priv = FW_VERSION_NR,
	},
	END_ATTRIBUTES_ARRAY,
}; // global attributes for device

struct iio_attribute event_pqm_dips_attribute[] = {
	EVENT_COMMON_ATTR,
	{
		.name = "minMag",
		.show = read_ch_attr,
		.priv = CHAN_EVENT_MIN_MAG,
	},
	END_ATTRIBUTES_ARRAY,
}; // event channel attribute for dips events

struct iio_attribute event_pqm_swell_attribute[] = {
	EVENT_COMMON_ATTR,
	{
		.name = "maxMag",
		.show = read_ch_attr,
		.priv = CHAN_EVENT_MAX_MAG,
	},
	END_ATTRIBUTES_ARRAY,
}; // event channel attribute for swell events

struct iio_attribute event_pqm_rvc_attribute[] = {
	EVENT_COMMON_ATTR,
	{
		.name = "deltaUmax",
		.show = read_ch_attr,
		.priv = CHAN_EVENT_DELTA_U_MAX,
	},
	{
		.name = "deltaUss",
		.show = read_ch_attr,
		.priv = CHAN_EVENT_DELTA_U_SS,
	},
	END_ATTRIBUTES_ARRAY,
}; // event channel attribute for rvc events

struct iio_attribute event_pqm_intrpr_attribute[] = {
	EVENT_COMMON_ATTR,
	END_ATTRIBUTES_ARRAY,
}; // event channel attribute

struct scan_type pqm_scan_type = {.sign = 's',
	.realbits = 16,
	.storagebits = 16,
	.shift = 0,
	.is_big_endian =
		true
}; // generic waveform channel definition

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
}; // channel definitions for device

struct iio_device pqm_iio_descriptor = {
	.num_ch = TOTAL_PQM_CHANNELS,
	.channels = iio_pqm_channels,
	.attributes = global_pqm_attributes,
	.debug_attributes = NULL,
	.buffer_attributes = NULL,
	.pre_enable = update_pqm_channels,
	.post_disable = close_pqm_channels,
	.trigger_handler = (int32_t(*)())pqm_trigger_handler,
	.submit = (int32_t(*)())read_samples,
}; // pqm iio descriptor
