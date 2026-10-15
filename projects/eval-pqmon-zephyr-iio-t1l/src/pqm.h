/*
 * pqm.h - PQM constants and types for IIO device definition
 * Aligned with no-OS eval-pqmon/src/common/common_data.h
 */
#ifndef PQM_H
#define PQM_H

#include <stdint.h>
#include <stdbool.h>
#include "adi_pqlib.h"
#include "afe_calibration.h"

#define FW_VERSION 2.2

#define TOTAL_PQM_CHANNELS 11
#define VOLTAGE_CH_NUMBER 3
#define CURRENT_CH_NUMBER 4
#define MAX_CH_ATTRS 31
#define PQM_DEVICE_ATTR_NUMBER 71
#define WAVEFORM_BUFFER_LENGTH (256 * 7)
#define MAX_EVENT_NUMBER 6

enum availavle_values_type {
	V_CONSEL_TYPE,
	FLICKER_MODEL_TYPE,
	NOMINAL_FREQUENCY_TYPE
};

struct pqm_desc {
	uint8_t reg[TOTAL_PQM_CHANNELS];
	float pqm_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t pqm_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t active_ch;
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

struct pqm_init_para {
	float dev_global_attr[PQM_DEVICE_ATTR_NUMBER];
	uint32_t dev_ch_attr[TOTAL_PQM_CHANNELS][MAX_CH_ATTRS];
	uint32_t ext_buff_len;
	int16_t *ext_buff;
};

int32_t update_pqm_channels(void *dev, uint32_t mask);
int32_t close_pqm_channels(void *dev);

#endif /* PQM_H */
