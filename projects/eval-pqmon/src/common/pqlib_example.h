/*******************************************************************************
 *   @file   pqlib_example.h
 *   @brief  Header file for pqlib display example functions
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __PQLIB_EXAMPLE_H__
#define __PQLIB_EXAMPLE_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "ade9430.h"
#include "adi_pqlib.h"
#include "adi_pqlib_memory.h"
#include "adi_pqlib_profile.h"
#include "pqlib_convert.h"
#include "status.h"
#include <stddef.h>
#include "no_os_circular_buffer.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define PQLIB_MAX_CHANNELS 7
#define ADI_PQLIB_NUM_WAVEFORM_BLOCKS 4
#define PQLIB_MAX_HARMONICS 50
#define PQLIB_MAX_INTER_HARMONICS 50
#define SIZE_OF_INPUT_RTC 19

/**
 * @brief PQLIB Example states
 *
 */
typedef enum {
	PQLIB_STATE_WAITING_FOR_START_CMD,
	PQLIB_STATE_WAITING_FOR_TRIGGER,
	PQLIB_STATE_RUNNING
} PQLIB_STATE;

/**
 * @brief VCONSEL H/W config selection
 *
 */
typedef enum {
	VCONSEL_4W_WYE,
	VCONSEL_3W_DELTA_VB_VA_NEGVC,
	VCONSEL_4W_WYE_VB_NEGVA_NEGVC,
	VCONSEL_4W_DELTA_VB_NEGVA,
	VCONSEL_4W_DELTA_VA_VB_VC,
} VCONSEL_CONFIG;

/**
 * Structure for pqlib example configurations.
 */
typedef struct {
	bool enableRTCSync;
	bool enableIconsel;
	bool useExternalTimestamp;
	uint8_t harmonics[PQLIB_MAX_HARMONICS];
	uint8_t interHarmonics[PQLIB_MAX_INTER_HARMONICS];
	uint8_t maxHarmonics;
	uint8_t maxInterHarmonics;
	uint8_t rtcBuff[SIZE_OF_INPUT_RTC];
	uint16_t currentCorrCoef[PQLIB_MAX_HARMONICS];
	uint16_t voltageCorrCoef[PQLIB_MAX_HARMONICS];
	uint16_t numCorrCoefCurrent50Hz;
	uint16_t numCorrCoefVoltage50Hz;
	uint16_t numCorrCoefCurrent60Hz;
	uint16_t numCorrCoefVoltage60Hz;
	uint32_t cyclesToRun;
	uint32_t nominalFrequency;
	uint32_t correction;
	uint32_t zeroCrossingCheck;
	uint64_t syncTime;
	float dipThreshold;
	float dipHysteresis;
	float swellThreshold;
	float swellHysteresis;
	float intrpThreshold;
	float intrpHysteresis;
	float rvcThreshold;
	float rvcHysteresis;
	float msvThreshold;
	float msvCarrierFrequency;
	float msvRecordingLength;
	float voltageScale;
	float currentScale;
	float nominalVoltage;
	float inputCurrentCorrCoef50Hz[PQLIB_MAX_HARMONICS];
	float inputVoltageCorrCoef50Hz[PQLIB_MAX_HARMONICS];
	float inputCurrentCorrCoef60Hz[PQLIB_MAX_HARMONICS];
	float inputVoltageCorrCoef60Hz[PQLIB_MAX_HARMONICS];
	char *pEnabledChannels[PQLIB_MAX_CHANNELS];
	ADI_PQLIB_REF_CHANNEL refChannel;
	ADI_PQLIB_CORR_COEF correctionData;
	ADI_PQLIB_FLICKER_MODEL flickerModel;
	ADI_PQLIB_PHASE_MAP phaseMap;
	VCONSEL_CONFIG vconsel;

} EXAMPLE_CONFIG; // pqlib example config

typedef struct {

	bool readyToDisplay;
	bool waitingForSync;
	bool calibration;
	uint16_t syncCycles;
	uint32_t processedCycles;
	uint32_t zeroCrossingCount;
	uint32_t timeOutCount;
	uint32_t pqlibStateMemory[ADI_PQLIB_STATE_MEM_NUM_BYTES / 4];
	uint32_t pqlibTempMemory[ADI_PQLIB_TEMP_MEM_NUM_BYTES / 4];
	float calExpectedRms;
	char *pChannel;
	ADI_PQLIB_HANDLE hDevice;
	ADI_PQLIB_CONFIG config;
	ADI_PQLIB_CYCLE_INPUT inputCycle;
	ADI_PQLIB_1012_CYCLE_INPUT input1012Cycles;
	ADI_PQLIB_WAVEFORM inputWaveform;
	EXAMPLE_CONFIG exampleConfig;
	ADI_PQLIB_OUTPUT *output;
	ADI_PQLIB_PHASE_MAP channelMap;
	PQLIB_STATE state;
	struct no_os_circular_buffer *no_os_cb_desc;

} PQLIB_EXAMPLE; // pqlib example struct

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/**
 * @brief One complete cycle for IIO callback function
 *
 * @return 0 - on success, different from 0 otherwise
 */
int pqm_one_cycle(void);

/**
 * @brief Prints error message for errors from the library. Serves
 * as an examples on how to handle the errors from the library.
 * @param[in] pExample     - Pointer to PQLIB_EXAMPLE structure.
 * @param[in] pqlibStatus  - return code for the library.
 * @return 0 - Error handled successfully.
 *         1  - Fatal error.
 */
int process_pqlib_error(PQLIB_EXAMPLE *pExample,
			ADI_PQLIB_RESULT pqlibStatus);

/**
 * @brief Call adi_pqlib_Open with required buffers. Serves
 * as an example how to call adi_pqlib_Open API
 * @param[in] pExample - Pointer to main example structure.
 */
int open_pqlib(PQLIB_EXAMPLE *pExample);

/**
 * @brief Sets library internal time using RTC time. Serves as an example
 * how to call the APIs. This also syncs the library time to the
 * user provided time.
 *
 * @param[in] pExample     - Pointer to example structure.
 * @param[in] checkRtcTime - Checks RTC time.
 * @return 0 on success
 *         1 on failure
 */
int SyncLibTime(PQLIB_EXAMPLE *pExample, bool checkRtcTime);

/**
 * @brief Configures AFE and the library . This serves as an
 * example on how to call APIs adi_pqlib_SetConfig and
 * adi_pqlib_SetCorrectionCoefficients.
 * @param[in] pExample - Pointer to structure for example
 *                         configurations.
 * @return 0 on success
 *         1 on failure
 *
 */
int config_measurement(PQLIB_EXAMPLE *pExample);

/**
 * @brief Sets default configuration of the example as floating points
 * Note that this function doesn't configure the library or AFE.
 *
 * @param[in] pConfig - Pointer to example configuration structure.
 */
void set_default_config(EXAMPLE_CONFIG *pConfig);

/**
 * @brief Populates event configuration using  user configuration
 * Serves as an example on how to convert thresholds and hysteresis
 * to the form required by the API.
 *
 * @param[in] pExampleConfig - pointer to structure for example
 *                            configurations.
 */
void populate_event_config(EXAMPLE_CONFIG *pExampleConfig);

/**
 * @brief Copies the user provided correction factors to the interface
 * structure. This shows how floating point correction factors can be
 * converted to the format required by adi_pqlib_SetCorrectionCoefficients
 * Requires both current and voltage pointers should be populated.
 * The code assumes numCorrCoef is same for both and if it is less than 40
 * these are last 'N' coefficients
 *
 * @param[in]  nominalFrequency - structure for nominal frequency
 * @param[in]  pCorrCoef - pointer to structure for correctionCoefficients
 * @return     0 on Success
 *             1 on Copy is not success because both current and voltage
 *             correction coefficients are not available.
 */
int cpy_correction_coeffs(ADI_PQLIB_NOMINAL_FREQUENCY nominalFrequency,
			  ADI_PQLIB_CORR_COEF *pCorrCoef);

/**
 * @brief Looks at the return code from library and checks whether there is
 * a zero crossing timeout. This is only needed for testing. We wanted
 * to know when actual signal comes and start monitoring test output only after
 * a predefined settling time.
 *
 * @param[in] pExample     - Pointer to PQLIB_EXAMPLE structure
 * @param[in] pqlibStatus  - return code for the library
 * @return   SYS_STATUS_NO_SIGNAL  - If no signal comes within timeout period
 *           SYS_STATUS_PQLIB_RUNNING - We are waiting for zerocrossing
 *           SYS_STATUS_SUCCESS -We have received enough zero crossing and
 * good to process 10/12 cycle inputs from the library
 */
int wait_for_zero_crossing(PQLIB_EXAMPLE *pExample,
			   ADI_PQLIB_RESULT pqlibStatus);

/**
 * @brief Sync the time to a dip or interruption event. Serves as an example
 * how to sync to an external time event.
 * @param[in] pExample - Pointer to example structure.
 * @param[in] pqlibStatus - Status from adi_pqlib_ProcessOneCycle.
 */
void SyncToDip(PQLIB_EXAMPLE *pExample, ADI_PQLIB_RESULT pqlibStatus);

extern PQLIB_EXAMPLE pqlibExample;

/**
 * @brief Initializes Power Quality Measurement.
 * @return   0 on Success
 *           1 on Failure
 */
int pqm_measurement_init(void);

/**
 * @brief Starts Power Quality measurement as per configured cycles.
 * @param[in]  waitingForSync - Flag to indicate whether the code
 * should look for a sync event to synchronize the RTC time.
 * @return  0 on Success
 *          1 on Failure
 *
 */
int pqm_start_measurement(bool waitingForSync);

/**
 * @brief The function gets AFE input and calls main processing APIs of
 * the library. This also gets the measurement parameter output from the
 * library and displays it on the console.
 *
 * This serves as an example for the APIs
 * adi_pqlib_ProcessOneCycle
 * adi_pqlib_ProcessWaveform
 * adi_pqlib_Process1012Cycles
 * adi_pqlib_GetOutputStatus
 *
 * @return 0 on Success
 *         1 on Failure
 */
int process_and_prepare_output();

#endif /* __PQLIB_EXAMPLE_H__ */
