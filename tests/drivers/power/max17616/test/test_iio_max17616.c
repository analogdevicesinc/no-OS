/***************************************************************************//**
 *   @file   test_iio_max17616.c
 *   @brief  Unit tests for MAX17616 IIO Driver (iio_max17616.c)
 *   @author Carlos Jones (carlosjr.jones@analog.com)
 *******************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*******************************************************************************
 *    TEST FILE ORGANIZATION
 ******************************************************************************/
/*
 * This test file is organized into the following main sections:
 *
 * 1. INCLUDED FILES & DEFINITIONS
 * 2. TEST SUPPORT VARIABLES & MOCK FUNCTIONS
 * 3. SETUP/TEARDOWN FUNCTIONS
 * 4. INIT/REMOVE TESTS
 * 5. TELEMETRY CHANNEL READ TESTS
 *    - Success Path Tests (VIN, VOUT, IOUT, TEMP, POUT)
 *    - Invalid Data Tests (all telemetry channels)
 *    - Driver Failure Tests (all telemetry channels)
 * 6. STATUS CHANNEL READ TESTS
 *    - Success Path Tests (WORD, VOUT, IOUT, INPUT, TEMP, CML, MFR_SPECIFIC)
 *    - Driver Failure Tests (all status channels)
 * 7. CONFIGURATION CHANNEL READ TESTS
 *    - Success Path Tests (CLMODE, ISTART_RATIO, NOMINAL_VOLTAGE, PGOOD_THRESHOLD)
 * 8. CONFIGURATION CHANNEL WRITE TESTS
 * 9. ERROR/EDGE CASE TESTS
 * 10. INVALID CHANNEL TESTS
 */

/*******************************************************************************
 *    INCLUDED FILES
 ******************************************************************************/

#include "unity.h"
#include "iio_max17616.h"
#include "mock_max17616.h"
#include "mock_no_os_alloc.h"
#include "max17616.h"
#include "iio_types.h"
#include "no_os_util.h"
#include <string.h>
#include <errno.h>

/*******************************************************************************
 *    PRIVATE VARIABLES
 ******************************************************************************/

static struct max17616_iio_desc *test_iio_desc;
static struct max17616_iio_desc_init_param test_iio_init_param;
static struct max17616_init_param test_max17616_init_param;
static struct max17616_dev test_max17616_dev;

/* Mock memory allocation buffer */
static char mock_memory[512] = {0};

/* Mock test data structures */
static struct max17616_telemetry test_telemetry;
static struct max17616_status test_status;

/* IIO Channel enumeration matching the actual implementation */
enum max17616_iio_channels {
	MAX17616_IIO_VIN_CHAN,
	MAX17616_IIO_VOUT_CHAN,
	MAX17616_IIO_IOUT_CHAN,
	MAX17616_IIO_TEMP_CHAN,
	MAX17616_IIO_POUT_CHAN,
	MAX17616_IIO_STATUS_WORD_CHAN,
	MAX17616_IIO_STATUS_VOUT_CHAN,
	MAX17616_IIO_STATUS_IOUT_CHAN,
	MAX17616_IIO_STATUS_INPUT_CHAN,
	MAX17616_IIO_STATUS_TEMP_CHAN,
	MAX17616_IIO_STATUS_CML_CHAN,
	MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN,
	MAX17616_IIO_CLMODE_CHAN,
	MAX17616_IIO_ISTART_RATIO_CHAN,
	MAX17616_IIO_TSTOC_CHAN,
	MAX17616_IIO_ISTLIM_CHAN,
	MAX17616_IIO_NOMINAL_VOLTAGE_CHAN,
	MAX17616_IIO_PGOOD_THRESHOLD_CHAN,
};

/* External declarations for static functions (requires TEST defined in build) */
extern int max17616_iio_read_attr(void *device, char *buf, uint32_t len,
				  const struct iio_ch_info *channel,
				  intptr_t priv);

extern int max17616_iio_write_attr(void *device, char *buf, uint32_t len,
				   const struct iio_ch_info *channel,
				   intptr_t priv);

extern int max17616_iio_read_global_attr(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

extern int max17616_iio_write_global_attr(void *device, char *buf, uint32_t len,
		const struct iio_ch_info *channel,
		intptr_t priv);

/*******************************************************************************
 *    MOCK CALLBACK FUNCTIONS
 ******************************************************************************/

/* Mock callback for telemetry reading */
static int mock_read_telemetry_all_callback(struct max17616_dev *dev,
		struct max17616_telemetry *telemetry,
		int num_calls)
{
	if (telemetry && dev) {
		*telemetry = test_telemetry;
		return 0;
	}
	return -EINVAL;
}

/* Mock callback for status reading */
static int mock_read_status_callback(struct max17616_dev *dev,
				     struct max17616_status *status,
				     int num_calls)
{
	if (status && dev) {
		*status = test_status;
		return 0;
	}
	return -EINVAL;
}

/* Mock callbacks for individual status register reads */
static uint8_t mock_status_vout = 0;
static uint8_t mock_status_iout = 0;
static uint8_t mock_status_input = 0;
static uint8_t mock_status_temp = 0;
static uint8_t mock_status_cml = 0;
static uint8_t mock_status_mfr = 0;

static int mock_read_status_vout_callback(struct max17616_dev *dev,
		uint8_t *status_vout, int num_calls)
{
	if (status_vout && dev) {
		*status_vout = mock_status_vout;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_status_iout_callback(struct max17616_dev *dev,
		uint8_t *status_iout, int num_calls)
{
	if (status_iout && dev) {
		*status_iout = mock_status_iout;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_status_input_callback(struct max17616_dev *dev,
		uint8_t *status_input, int num_calls)
{
	if (status_input && dev) {
		*status_input = mock_status_input;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_status_temperature_callback(struct max17616_dev *dev,
		uint8_t *status_temperature, int num_calls)
{
	if (status_temperature && dev) {
		*status_temperature = mock_status_temp;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_status_cml_callback(struct max17616_dev *dev,
		uint8_t *status_cml, int num_calls)
{
	if (status_cml && dev) {
		*status_cml = mock_status_cml;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_status_mfr_specific_callback(struct max17616_dev *dev,
		uint8_t *status_mfr,
		int num_calls)
{
	if (status_mfr && dev) {
		*status_mfr = mock_status_mfr;
		return 0;
	}
	return -EINVAL;
}

/* Mock callbacks for configuration parameter getters */
static enum max17616_current_limit_mode mock_clmode = MAX17616_CLMODE_LATCH_OFF;
static enum max17616_istart_ratio mock_istart_ratio = MAX17616_ISTART_FULL;
static enum max17616_overcurrent_timeout mock_tstoc = MAX17616_TIMEOUT_400US;
static enum max17616_overcurrent_limit mock_istlim = MAX17616_OC_LIMIT_1_25;
static enum max17616_nominal_voltage mock_nominal_voltage = MAX17616_NOMINAL_5V;
static enum max17616_pgood_threshold mock_pgood_threshold =
	MAX17616_PGOOD_MINUS_10_PERCENT;

static int mock_get_current_limit_mode_callback(struct max17616_dev *dev,
		enum max17616_current_limit_mode *clmode,
		int num_calls)
{
	if (clmode && dev) {
		*clmode = mock_clmode;
		return 0;
	}
	return -EINVAL;
}

static int mock_get_istart_ratio_callback(struct max17616_dev *dev,
		enum max17616_istart_ratio *istart_ratio,
		int num_calls)
{
	if (istart_ratio && dev) {
		*istart_ratio = mock_istart_ratio;
		return 0;
	}
	return -EINVAL;
}

static int mock_get_overcurrent_timeout_callback(struct max17616_dev *dev,
		enum max17616_overcurrent_timeout *timeout,
		int num_calls)
{
	if (timeout && dev) {
		*timeout = mock_tstoc;
		return 0;
	}
	return -EINVAL;
}

static int mock_get_overcurrent_limit_callback(struct max17616_dev *dev,
		enum max17616_overcurrent_limit *istlim,
		int num_calls)
{
	if (istlim && dev) {
		*istlim = mock_istlim;
		return 0;
	}
	return -EINVAL;
}

static int mock_get_vout_uv_fault_limit_config_callback(
	struct max17616_dev *dev,
	enum max17616_nominal_voltage *voltage,
	enum max17616_pgood_threshold *threshold,
	int num_calls)
{
	if (voltage && threshold && dev) {
		*voltage = mock_nominal_voltage;
		*threshold = mock_pgood_threshold;
		return 0;
	}
	return -EINVAL;
}

/* Mock callbacks for global attributes */
static bool mock_operation_state = false;
static uint8_t mock_capability = 0;

static int mock_get_operation_state_callback(struct max17616_dev *dev,
		bool *enabled, int num_calls)
{
	if (enabled && dev) {
		*enabled = mock_operation_state;
		return 0;
	}
	return -EINVAL;
}

static int mock_read_capability_callback(struct max17616_dev *dev,
		uint8_t *capability, int num_calls)
{
	if (capability && dev) {
		*capability = mock_capability;
		return 0;
	}
	return -EINVAL;
}

/*******************************************************************************
 *    TEST SETUP AND TEARDOWN
 ******************************************************************************/

void setUp(void)
{
	/* Initialize test structures */
	test_iio_desc = NULL;
	memset(&test_iio_init_param, 0, sizeof(test_iio_init_param));
	memset(&test_max17616_init_param, 0, sizeof(test_max17616_init_param));
	memset(&test_max17616_dev, 0, sizeof(test_max17616_dev));
	memset(mock_memory, 0, sizeof(mock_memory));

	/* Setup initialization parameters */
	test_iio_init_param.max17616_init_param = &test_max17616_init_param;

	/* Initialize test telemetry data */
	memset(&test_telemetry, 0, sizeof(test_telemetry));
	test_telemetry.vin = 12000;   /* 12V in mV */
	test_telemetry.vout = 5000;   /* 5V in mV */
	test_telemetry.iout = 2000;   /* 2A in mA */
	test_telemetry.temp1 = 25;    /* 25°C */
	test_telemetry.pout = 10000;  /* 10W in mW */
	test_telemetry.valid_mask = 0x3F; /* All valid */

	/* Initialize test status data */
	memset(&test_status, 0, sizeof(test_status));
	test_status.word = 0x0000;    /* No faults */

	/* Initialize mock status values */
	mock_status_vout = 0x00;
	mock_status_iout = 0x00;
	mock_status_input = 0x00;
	mock_status_temp = 0x00;
	mock_status_cml = 0x00;
	mock_status_mfr = 0x00;

	/* Initialize mock configuration values */
	mock_clmode = MAX17616_CLMODE_LATCH_OFF;
	mock_istart_ratio = MAX17616_ISTART_FULL;
	mock_tstoc = MAX17616_TIMEOUT_400US;
	mock_istlim = MAX17616_OC_LIMIT_1_25;
	mock_nominal_voltage = MAX17616_NOMINAL_5V;
	mock_pgood_threshold = MAX17616_PGOOD_MINUS_10_PERCENT;

	/* Initialize mock global attribute values */
	mock_operation_state = false;
	mock_capability = 0x80;
}

void tearDown(void)
{
	test_iio_desc = NULL;
	/* Clear all CMock expectations between tests */
	mock_max17616_Verify();
	mock_max17616_Destroy();
	mock_max17616_Init();
}

/*******************************************************************************
 *    IIO INITIALIZATION/REMOVAL TESTS
 ******************************************************************************/

/**
 * Test max17616_iio_init with successful initialization
 */
void test_max17616_iio_init_success(void)
{
	struct max17616_iio_desc *iio_desc = NULL;

	/* Setup mock expectations */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct max17616_iio_desc), mock_memory);
	max17616_init_IgnoreAndReturn(0);

	/* Test the function */
	int result = max17616_iio_init(&iio_desc, &test_iio_init_param);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(0, result);
	TEST_ASSERT_NOT_NULL(iio_desc);
}

/**
 * Test max17616_iio_init with NULL parameters
 */
void test_max17616_iio_init_null_params(void)
{
	struct max17616_iio_desc *iio_desc = NULL;

	/* Test NULL iio_desc */
	int result = max17616_iio_init(NULL, &test_iio_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, result);

	/* Test NULL init_param */
	result = max17616_iio_init(&iio_desc, NULL);
	TEST_ASSERT_EQUAL_INT(-EINVAL, result);

	/* Test NULL max17616_init_param */
	test_iio_init_param.max17616_init_param = NULL;
	result = max17616_iio_init(&iio_desc, &test_iio_init_param);
	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}

/**
 * Test max17616_iio_init with memory allocation failure
 */
void test_max17616_iio_init_malloc_failure(void)
{
	struct max17616_iio_desc *iio_desc = NULL;

	/* Setup mock to return NULL for calloc */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct max17616_iio_desc), NULL);

	int result = max17616_iio_init(&iio_desc, &test_iio_init_param);

	TEST_ASSERT_EQUAL_INT(-ENOMEM, result);
	TEST_ASSERT_NULL(iio_desc);
}

/**
 * Test max17616_iio_init with core driver init failure
 */
void test_max17616_iio_init_core_init_failure(void)
{
	struct max17616_iio_desc *iio_desc = NULL;

	/* Setup mock expectations */
	no_os_calloc_ExpectAndReturn(1, sizeof(struct max17616_iio_desc), mock_memory);
	max17616_init_IgnoreAndReturn(-EIO);
	no_os_free_Expect(mock_memory);

	int result = max17616_iio_init(&iio_desc, &test_iio_init_param);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/**
 * Test max17616_iio_remove with valid descriptor
 */
void test_max17616_iio_remove_success(void)
{
	struct max17616_iio_desc iio_desc;
	iio_desc.max17616_dev = &test_max17616_dev;
	iio_desc.iio_dev = (struct iio_device*)0x1234; /* Non-NULL pointer */

	/* Setup mock expectations */
	max17616_remove_ExpectAndReturn(&test_max17616_dev, 0);
	no_os_free_Expect(&iio_desc);

	int result = max17616_iio_remove(&iio_desc);

	TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * Test max17616_iio_remove with NULL descriptor
 */
void test_max17616_iio_remove_null_desc(void)
{
	int result = max17616_iio_remove(NULL);

	TEST_ASSERT_EQUAL_INT(-ENODEV, result);
}

/*******************************************************************************
 *    TELEMETRY CHANNEL READ TESTS
 ******************************************************************************/
/*
 * Success Path Tests:
 * - test_max17616_iio_read_attr_vin_raw
 * - test_max17616_iio_read_attr_vout_raw
 * - test_max17616_iio_read_attr_iout_raw
 * - test_max17616_iio_read_attr_temp_raw
 * - test_max17616_iio_read_attr_pout_raw
 *
 * Invalid Data Tests:
 * - test_max17616_iio_read_attr_telemetry_invalid_data (VIN)
 * - test_max17616_iio_read_attr_vout_telemetry_invalid_data
 * - test_max17616_iio_read_attr_iout_telemetry_invalid_data
 * - test_max17616_iio_read_attr_temp_telemetry_invalid_data
 * - test_max17616_iio_read_attr_pout_telemetry_invalid_data
 *
 * Driver Failure Tests:
 * - test_max17616_iio_read_attr_telemetry_driver_failure (VIN)
 * - test_max17616_iio_read_attr_vout_telemetry_driver_failure
 * - test_max17616_iio_read_attr_iout_telemetry_driver_failure
 * - test_max17616_iio_read_attr_temp_telemetry_driver_failure
 * - test_max17616_iio_read_attr_pout_telemetry_driver_failure
 */

/*******************************************************************
 *    TELEMETRY SUCCESS PATH TESTS
 *******************************************************************/

/**
 * Test reading VIN channel raw attribute
 */
void test_max17616_iio_read_attr_vin_raw(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VIN_CHAN,
		.address = MAX17616_IIO_VIN_CHAN
	};

	/* Setup test data */
	test_telemetry.vin = 12345;
	test_telemetry.valid_mask = NO_OS_BIT(0); /* VIN valid */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(5, result); /* Length of "12345" */
	TEST_ASSERT_EQUAL_STRING("12345", buffer);
}

/**
 * Test reading VOUT channel raw attribute
 */
void test_max17616_iio_read_attr_vout_raw(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VOUT_CHAN,
		.address = MAX17616_IIO_VOUT_CHAN
	};

	/* Setup test data */
	test_telemetry.vout = 5678;
	test_telemetry.valid_mask = NO_OS_BIT(1); /* VOUT valid */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "5678" */
	TEST_ASSERT_EQUAL_STRING("5678", buffer);
}

/**
 * Test reading IOUT channel raw attribute
 */
void test_max17616_iio_read_attr_iout_raw(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_IOUT_CHAN,
		.address = MAX17616_IIO_IOUT_CHAN
	};

	/* Setup test data */
	test_telemetry.iout = 1500;
	test_telemetry.valid_mask = NO_OS_BIT(3); /* IOUT valid */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "1500" */
	TEST_ASSERT_EQUAL_STRING("1500", buffer);
}

/**
 * Test reading TEMP channel raw attribute
 */
void test_max17616_iio_read_attr_temp_raw(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_TEMP_CHAN,
		.address = MAX17616_IIO_TEMP_CHAN
	};

	/* Setup test data */
	test_telemetry.temp1 = 65;
	test_telemetry.valid_mask = NO_OS_BIT(4); /* TEMP valid */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(2, result); /* Length of "65" */
	TEST_ASSERT_EQUAL_STRING("65", buffer);
}

/**
 * Test reading POUT channel raw attribute
 */
void test_max17616_iio_read_attr_pout_raw(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_POUT_CHAN,
		.address = MAX17616_IIO_POUT_CHAN
	};

	/* Setup test data */
	test_telemetry.pout = 8500;
	test_telemetry.valid_mask = NO_OS_BIT(5); /* POUT valid */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "8500" */
	TEST_ASSERT_EQUAL_STRING("8500", buffer);
}

/*******************************************************************
 *    TELEMETRY INVALID DATA TESTS
 *******************************************************************/

/**
 * Test VIN telemetry read with invalid data (valid mask = 0)
 */
void test_max17616_iio_read_attr_telemetry_invalid_data(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VIN_CHAN,
		.address = MAX17616_IIO_VIN_CHAN
	};

	/* Setup test data with invalid mask */
	test_telemetry.vin = 12000;
	test_telemetry.valid_mask = 0; /* No valid data */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Should return -ENODATA when data is invalid */
	TEST_ASSERT_EQUAL_INT(-ENODATA, result);
}

/*******************************************************************
 *    TELEMETRY DRIVER FAILURE TESTS
 *******************************************************************/

/**
 * Test VIN telemetry read with driver failure
 */
void test_max17616_iio_read_attr_telemetry_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VIN_CHAN,
		.address = MAX17616_IIO_VIN_CHAN
	};

	/* Setup mock to return error */
	max17616_read_telemetry_all_IgnoreAndReturn(-EIO);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/**
 * Test VOUT telemetry read with invalid data
 */
void test_max17616_iio_read_attr_vout_telemetry_invalid_data(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VOUT_CHAN,
		.address = MAX17616_IIO_VOUT_CHAN
	};

	/* Setup test data with invalid mask for VOUT (bit 1) */
	test_telemetry.vout = 3300;
	test_telemetry.valid_mask = 0; /* No valid data */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Should return -ENODATA when data is invalid */
	TEST_ASSERT_EQUAL_INT(-ENODATA, result);
}

/**
 * Test VOUT telemetry read with driver failure
 */
void test_max17616_iio_read_attr_vout_telemetry_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_VOUT_CHAN,
		.address = MAX17616_IIO_VOUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_telemetry_all_IgnoreAndReturn(-EIO);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/**
 * Test IOUT telemetry read with invalid data
 */
void test_max17616_iio_read_attr_iout_telemetry_invalid_data(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_IOUT_CHAN,
		.address = MAX17616_IIO_IOUT_CHAN
	};

	/* Setup test data with invalid mask for IOUT (bit 3) */
	test_telemetry.iout = 1500;
	test_telemetry.valid_mask = 0; /* No valid data */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Should return -ENODATA when data is invalid */
	TEST_ASSERT_EQUAL_INT(-ENODATA, result);
}

/**
 * Test IOUT telemetry read with driver failure
 */
void test_max17616_iio_read_attr_iout_telemetry_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_IOUT_CHAN,
		.address = MAX17616_IIO_IOUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_telemetry_all_IgnoreAndReturn(-EIO);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/**
 * Test TEMP telemetry read with invalid data
 */
void test_max17616_iio_read_attr_temp_telemetry_invalid_data(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_TEMP_CHAN,
		.address = MAX17616_IIO_TEMP_CHAN
	};

	/* Setup test data with invalid mask for TEMP (bit 4) */
	test_telemetry.temp1 = 25;
	test_telemetry.valid_mask = 0; /* No valid data */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Should return -ENODATA when data is invalid */
	TEST_ASSERT_EQUAL_INT(-ENODATA, result);
}

/**
 * Test TEMP telemetry read with driver failure
 */
void test_max17616_iio_read_attr_temp_telemetry_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_TEMP_CHAN,
		.address = MAX17616_IIO_TEMP_CHAN
	};

	/* Setup mock to return error */
	max17616_read_telemetry_all_IgnoreAndReturn(-EIO);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/**
 * Test POUT telemetry read with invalid data
 */
void test_max17616_iio_read_attr_pout_telemetry_invalid_data(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_POUT_CHAN,
		.address = MAX17616_IIO_POUT_CHAN
	};

	/* Setup test data with invalid mask for POUT (bit 5) */
	test_telemetry.pout = 4950;
	test_telemetry.valid_mask = 0; /* No valid data */

	/* Setup mock */
	max17616_read_telemetry_all_Stub(mock_read_telemetry_all_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Should return -ENODATA when data is invalid */
	TEST_ASSERT_EQUAL_INT(-ENODATA, result);
}

/**
 * Test POUT telemetry read with driver failure
 */
void test_max17616_iio_read_attr_pout_telemetry_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_POUT_CHAN,
		.address = MAX17616_IIO_POUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_telemetry_all_IgnoreAndReturn(-EIO);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EIO, result);
}

/*******************************************************************************
 *    STATUS CHANNEL READ TESTS
 ******************************************************************************/
/*
 * Success Path Tests:
 * - test_max17616_iio_read_attr_status_word
 * - test_max17616_iio_read_attr_status_vout
 * - test_max17616_iio_read_attr_status_iout
 * - test_max17616_iio_read_attr_status_input
 * - test_max17616_iio_read_attr_status_temp
 * - test_max17616_iio_read_attr_status_cml
 * - test_max17616_iio_read_attr_status_mfr_specific
 *
 * Driver Failure Tests:
 * - test_max17616_iio_read_attr_status_driver_failure (WORD)
 * - test_max17616_iio_read_attr_status_vout_driver_failure
 * - test_max17616_iio_read_attr_status_iout_driver_failure
 * - test_max17616_iio_read_attr_status_input_driver_failure
 * - test_max17616_iio_read_attr_status_temp_driver_failure
 * - test_max17616_iio_read_attr_status_cml_driver_failure
 * - test_max17616_iio_read_attr_status_mfr_specific_driver_failure
 */

/*******************************************************************
 *    STATUS SUCCESS PATH TESTS
 *******************************************************************/

/**
 * Test reading STATUS_WORD channel
 */
void test_max17616_iio_read_attr_status_word(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_WORD_CHAN,
		.address = MAX17616_IIO_STATUS_WORD_CHAN
	};

	/* Setup test data */
	test_status.word = 0x1234;

	/* Setup mock */
	max17616_read_status_Stub(mock_read_status_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(6, result); /* Length of "0x1234" */
	TEST_ASSERT_EQUAL_STRING("0x1234", buffer);
}

/**
 * Test reading STATUS_VOUT channel
 */
void test_max17616_iio_read_attr_status_vout(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_VOUT_CHAN,
		.address = MAX17616_IIO_STATUS_VOUT_CHAN
	};

	/* Setup test data */
	mock_status_vout = 0xAB;

	/* Setup mock */
	max17616_read_status_vout_Stub(mock_read_status_vout_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0xAB" */
	TEST_ASSERT_EQUAL_STRING("0xAB", buffer);
}

/**
 * Test reading STATUS_IOUT channel
 */
void test_max17616_iio_read_attr_status_iout(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_IOUT_CHAN,
		.address = MAX17616_IIO_STATUS_IOUT_CHAN
	};

	/* Setup test data */
	mock_status_iout = 0xCD;

	/* Setup mock */
	max17616_read_status_iout_Stub(mock_read_status_iout_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0xCD" */
	TEST_ASSERT_EQUAL_STRING("0xCD", buffer);
}

/*******************************************************************
 *    STATUS DRIVER FAILURE TESTS
 *******************************************************************/

/**
 * Test STATUS_WORD read with driver failure
 */
void test_max17616_iio_read_attr_status_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_WORD_CHAN,
		.address = MAX17616_IIO_STATUS_WORD_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test STATUS_VOUT read with driver failure
 */
void test_max17616_iio_read_attr_status_vout_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_VOUT_CHAN,
		.address = MAX17616_IIO_STATUS_VOUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_vout_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test STATUS_IOUT read with driver failure
 */
void test_max17616_iio_read_attr_status_iout_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_IOUT_CHAN,
		.address = MAX17616_IIO_STATUS_IOUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_iout_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test reading STATUS_INPUT channel
 */
void test_max17616_iio_read_attr_status_input(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_INPUT_CHAN,
		.address = MAX17616_IIO_STATUS_INPUT_CHAN
	};

	/* Setup test data */
	mock_status_input = 0x42;

	/* Setup mock */
	max17616_read_status_input_Stub(mock_read_status_input_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0x42" */
	TEST_ASSERT_EQUAL_STRING("0x42", buffer);
}

/**
 * Test STATUS_INPUT read with driver failure
 */
void test_max17616_iio_read_attr_status_input_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_INPUT_CHAN,
		.address = MAX17616_IIO_STATUS_INPUT_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_input_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test reading STATUS_TEMP channel
 */
void test_max17616_iio_read_attr_status_temp(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_TEMP_CHAN,
		.address = MAX17616_IIO_STATUS_TEMP_CHAN
	};

	/* Setup test data */
	mock_status_temp = 0x85;

	/* Setup mock */
	max17616_read_status_temperature_Stub(mock_read_status_temperature_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0x85" */
	TEST_ASSERT_EQUAL_STRING("0x85", buffer);
}

/**
 * Test STATUS_TEMP read with driver failure
 */
void test_max17616_iio_read_attr_status_temp_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_TEMP_CHAN,
		.address = MAX17616_IIO_STATUS_TEMP_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_temperature_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test reading STATUS_CML channel
 */
void test_max17616_iio_read_attr_status_cml(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_CML_CHAN,
		.address = MAX17616_IIO_STATUS_CML_CHAN
	};

	/* Setup test data */
	mock_status_cml = 0x1A;

	/* Setup mock */
	max17616_read_status_cml_Stub(mock_read_status_cml_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0x1A" */
	TEST_ASSERT_EQUAL_STRING("0x1A", buffer);
}

/**
 * Test STATUS_CML read with driver failure
 */
void test_max17616_iio_read_attr_status_cml_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_CML_CHAN,
		.address = MAX17616_IIO_STATUS_CML_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_cml_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/**
 * Test reading STATUS_MFR_SPECIFIC channel
 */
void test_max17616_iio_read_attr_status_mfr_specific(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN,
		.address = MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN
	};

	/* Setup test data */
	mock_status_mfr = 0xF3;

	/* Setup mock */
	max17616_read_status_mfr_specific_Stub(mock_read_status_mfr_specific_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(4, result); /* Length of "0xF3" */
	TEST_ASSERT_EQUAL_STRING("0xF3", buffer);
}

/**
 * Test STATUS_MFR_SPECIFIC read with driver failure
 */
void test_max17616_iio_read_attr_status_mfr_specific_driver_failure(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN,
		.address = MAX17616_IIO_STATUS_MFR_SPECIFIC_CHAN
	};

	/* Setup mock to return error */
	max17616_read_status_mfr_specific_IgnoreAndReturn(-ECOMM);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-ECOMM, result);
}

/*******************************************************************************
 *    CONFIGURATION CHANNEL READ TESTS
 ******************************************************************************/
/*
 * Success Path Tests:
 * - test_max17616_iio_read_attr_clmode
 * - test_max17616_iio_read_attr_istart_ratio
 * - test_max17616_iio_read_attr_tstoc
 * - test_max17616_iio_read_attr_istlim
 * - test_max17616_iio_read_attr_nominal_voltage
 * - test_max17616_iio_read_attr_pgood_threshold
 */

/*******************************************************************
 *    CONFIGURATION SUCCESS PATH TESTS
 *******************************************************************/

/**
 * Test reading CLMODE channel
 */
void test_max17616_iio_read_attr_clmode(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_CLMODE_CHAN,
		.address = MAX17616_IIO_CLMODE_CHAN
	};

	/* Setup test data */
	mock_clmode = MAX17616_CLMODE_CONTINUOUS; /* Value 0x40 -> 64 */

	/* Setup mock */
	max17616_get_current_limit_mode_Stub(mock_get_current_limit_mode_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(2, result); /* Length of "64" */
	TEST_ASSERT_EQUAL_STRING("64", buffer);
}

/**
 * Test reading ISTART_RATIO channel
 */
void test_max17616_iio_read_attr_istart_ratio(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_ISTART_RATIO_CHAN,
		.address = MAX17616_IIO_ISTART_RATIO_CHAN
	};

	/* Setup test data */
	mock_istart_ratio = MAX17616_ISTART_HALF; /* Value 1 */

	/* Setup mock */
	max17616_get_istart_ratio_Stub(mock_get_istart_ratio_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(1, result); /* Length of "1" */
	TEST_ASSERT_EQUAL_STRING("1", buffer);
}

/**
 * Test reading TSTOC (Overcurrent Timeout) channel
 */
void test_max17616_iio_read_attr_tstoc(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_TSTOC_CHAN,
		.address = MAX17616_IIO_TSTOC_CHAN
	};

	/* Setup test data */
	mock_tstoc = MAX17616_TIMEOUT_1MS; /* Value 1 -> returns "1" */

	/* Setup mock */
	max17616_get_overcurrent_timeout_Stub(mock_get_overcurrent_timeout_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(1, result); /* Length of "1" */
	TEST_ASSERT_EQUAL_STRING("1", buffer);
}

/**
 * Test reading ISTLIM (Overcurrent Limit) channel
 */
void test_max17616_iio_read_attr_istlim(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_ISTLIM_CHAN,
		.address = MAX17616_IIO_ISTLIM_CHAN
	};

	/* Setup test data */
	mock_istlim = MAX17616_OC_LIMIT_1_50; /* Value 1 -> returns "1" */

	/* Setup mock */
	max17616_get_overcurrent_limit_Stub(mock_get_overcurrent_limit_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(1, result); /* Length of "1" */
	TEST_ASSERT_EQUAL_STRING("1", buffer);
}

/**
 * Test reading NOMINAL_VOLTAGE channel
 */
void test_max17616_iio_read_attr_nominal_voltage(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = 16, /* nominal_voltage channel index */
		.address = 17 /* MAX17616_IIO_NOMINAL_VOLTAGE_CHAN enum value */
	};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	/* Set mock values for nominal voltage test */
	mock_nominal_voltage = MAX17616_NOMINAL_12V; /* enum value 2 -> returns "2" */
	mock_pgood_threshold = MAX17616_PGOOD_MINUS_20_PERCENT; /* enum value 1 */

	/* Setup mock to use callback for this test only */
	max17616_get_vout_uv_fault_limit_config_Stub(
		mock_get_vout_uv_fault_limit_config_callback);

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Clean up the stub immediately */
	max17616_get_vout_uv_fault_limit_config_Stub(NULL);

	/* Verify results */
	TEST_ASSERT_GREATER_THAN(0, result); /* Should return positive length */
	TEST_ASSERT_EQUAL_STRING("2", buffer); /* MAX17616_NOMINAL_12V = 2 */
}

/**
 * Test reading PGOOD_THRESHOLD channel
 */
void test_max17616_iio_read_attr_pgood_threshold(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = 17, /* pgood_threshold channel index */
		.address = 18 /* MAX17616_IIO_PGOOD_THRESHOLD_CHAN enum value */
	};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	/* Set mock values for pgood threshold test */
	mock_nominal_voltage = MAX17616_NOMINAL_24V; /* enum value 2 */
	mock_pgood_threshold =
		MAX17616_PGOOD_MINUS_30_PERCENT; /* enum value 2 -> returns "2" */

	/* Setup mock to use callback for this test only */
	max17616_get_vout_uv_fault_limit_config_Stub(
		mock_get_vout_uv_fault_limit_config_callback);

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	/* Clean up the stub immediately */
	max17616_get_vout_uv_fault_limit_config_Stub(NULL);

	/* Verify results */
	TEST_ASSERT_GREATER_THAN(0, result); /* Should return positive length */
	TEST_ASSERT_EQUAL_STRING("2", buffer); /* MAX17616_PGOOD_MINUS_30_PERCENT = 2 */
}

/*******************************************************************************
 *    CONFIGURATION CHANNEL WRITE TESTS
 ******************************************************************************/

/**
 * Test writing CLMODE channel - Note: Due to channel address shift issue mentioned in code
 */
void test_max17616_iio_write_attr_clmode(void)
{
	char input[] = "128"; /* MAX17616_CLMODE_AUTO_RETRY = 0x80 = 128 */
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_CLMODE_CHAN,
		.address = MAX17616_IIO_CLMODE_CHAN
	};

	/* Setup mock expectation - Note: per the IIO code comment, there's a channel shift issue */
	max17616_set_current_limit_mode_ExpectAndReturn(&test_max17616_dev,
			(enum max17616_current_limit_mode)128, 0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(strlen(input), result);
}

/**
 * Test writing TSTOC (Overcurrent Timeout) channel
 */
void test_max17616_iio_write_attr_tstoc(void)
{
	char input[] = "2"; /* MAX17616_TIMEOUT_4MS = 2 */
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_TSTOC_CHAN,
		.address = MAX17616_IIO_TSTOC_CHAN
	};

	/* Setup mock to expect the set call */
	max17616_set_overcurrent_timeout_IgnoreAndReturn(0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	/* Should return number of characters written */
	TEST_ASSERT_EQUAL_INT(strlen(input), result);
}

/**
 * Test writing ISTLIM (Overcurrent Limit) channel
 */
void test_max17616_iio_write_attr_istlim(void)
{
	char input[] = "2"; /* MAX17616_OC_LIMIT_1_75 = 2 */
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_ISTLIM_CHAN,
		.address = MAX17616_IIO_ISTLIM_CHAN
	};

	/* Setup mock to expect the set call */
	max17616_set_overcurrent_limit_IgnoreAndReturn(0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	/* Should return number of characters written */
	TEST_ASSERT_EQUAL_INT(strlen(input), result);
}

/**
 * Test writing ISTART_RATIO channel
 */
void test_max17616_iio_write_attr_istart_ratio(void)
{
	char input[] = "3"; /* MAX17616_ISTART_EIGHTH = 3 */
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_ISTART_RATIO_CHAN,
		.address = MAX17616_IIO_ISTART_RATIO_CHAN
	};

	/* Setup mock to expect the set call */
	max17616_set_istart_ratio_IgnoreAndReturn(0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	/* Should return number of characters written */
	TEST_ASSERT_EQUAL_INT(strlen(input), result);
}

/**
 * Test write with invalid input format
 */
void test_max17616_iio_write_attr_invalid_format(void)
{
	char input[] = "abc"; /* Non-numeric input */
	struct iio_ch_info channel = {
		.ch_num = MAX17616_IIO_CLMODE_CHAN,
		.address = MAX17616_IIO_CLMODE_CHAN
	};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}

/**
 * Test write with invalid channel
 */
void test_max17616_iio_write_attr_invalid_channel(void)
{
	char input[] = "5";
	struct iio_ch_info channel = {
		.ch_num = 99, /* Invalid channel */
		.address = 99
	};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_attr(&iio_desc, input, strlen(input), &channel,
					     0);

	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}

/*******************************************************************************
 *    GLOBAL ATTRIBUTE TESTS
 ******************************************************************************/

/**
 * Test reading operation global attribute (enabled)
 */
void test_max17616_iio_read_global_attr_operation_enabled(void)
{
	char buffer[32];
	struct iio_ch_info channel = {0}; /* Not used for global attributes */

	/* Setup test data */
	mock_operation_state = true;

	/* Setup mock */
	max17616_get_operation_state_Stub(mock_get_operation_state_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(7, result); /* Length of "enabled" */
	TEST_ASSERT_EQUAL_STRING("enabled", buffer);
}

/**
 * Test reading operation global attribute (disabled)
 */
void test_max17616_iio_read_global_attr_operation_disabled(void)
{
	char buffer[32];
	struct iio_ch_info channel = {0};

	/* Setup test data */
	mock_operation_state = false;

	/* Setup mock */
	max17616_get_operation_state_Stub(mock_get_operation_state_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 0);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(8, result); /* Length of "disabled" */
	TEST_ASSERT_EQUAL_STRING("disabled", buffer);
}

/**
 * Test reading device_info global attribute
 */
void test_max17616_iio_read_global_attr_device_info(void)
{
	char buffer[128];
	struct iio_ch_info channel = {0};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 2);

	/* Verify results */
	const char *expected = "MAX17616/MAX17616A Protection IC";
	TEST_ASSERT_EQUAL_INT(strlen(expected), result);
	TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

/**
 * Test reading fault_summary global attribute (no faults)
 */
void test_max17616_iio_read_global_attr_fault_summary_no_faults(void)
{
	char buffer[128];
	struct iio_ch_info channel = {0};

	/* Setup test data */
	test_status.word = 0; /* No faults */

	/* Setup mock */
	max17616_read_status_Stub(mock_read_status_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 3);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(9, result); /* Length of "No faults" */
	TEST_ASSERT_EQUAL_STRING("No faults", buffer);
}

/**
 * Test reading fault_summary global attribute (multiple faults)
 */
void test_max17616_iio_read_global_attr_fault_summary_faults(void)
{
	char buffer[256];
	struct iio_ch_info channel = {0};

	/* Setup test data with faults */
	test_status.word = 0x1234; /* Has faults */
	test_status.vout = 1;      /* VOUT fault */
	test_status.iout = 1;      /* IOUT fault */
	test_status.input = 0;
	test_status.temperature = 1; /* TEMP fault */
	test_status.cml = 0;
	test_status.mfr_specific = 0;

	/* Setup mock */
	max17616_read_status_Stub(mock_read_status_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 3);

	/* Verify results */
	const char *expected = "VOUT_FAULT IOUT_FAULT TEMP_FAULT ";
	TEST_ASSERT_EQUAL_INT(strlen(expected), result);
	TEST_ASSERT_EQUAL_STRING(expected, buffer);
}

/**
 * Test reading capability global attribute
 */
void test_max17616_iio_read_global_attr_capability(void)
{
	char buffer[32];
	struct iio_ch_info channel = {0};

	/* Setup test data */
	mock_capability = 0x80; /* 128 in decimal */

	/* Setup mock */
	max17616_read_capability_Stub(mock_read_capability_callback);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 4);

	/* Verify results */
	TEST_ASSERT_EQUAL_INT(3, result); /* Length of "128" */
	TEST_ASSERT_EQUAL_STRING("128", buffer);
}

/**
 * Test reading global attribute with invalid priv
 */
void test_max17616_iio_read_global_attr_invalid_priv(void)
{
	char buffer[32];
	struct iio_ch_info channel = {0};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_global_attr(&iio_desc, buffer, sizeof(buffer),
			&channel, 99);

	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}

/**
 * Test writing operation global attribute (enable with "1")
 */
void test_max17616_iio_write_global_attr_operation_enable_1(void)
{
	char input[] = "1";
	struct iio_ch_info channel = {0};

	/* Setup mock expectation */
	max17616_set_operation_state_ExpectAndReturn(&test_max17616_dev, 1, 0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_global_attr(&iio_desc, input, strlen(input),
			&channel, 0);

	TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * Test writing operation global attribute (enable with "enable")
 */
void test_max17616_iio_write_global_attr_operation_enable_string(void)
{
	char input[] = "enable";
	struct iio_ch_info channel = {0};

	/* Setup mock expectation */
	max17616_set_operation_state_ExpectAndReturn(&test_max17616_dev, 1, 0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_global_attr(&iio_desc, input, strlen(input),
			&channel, 0);

	TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * Test writing operation global attribute (disable with "0")
 */
void test_max17616_iio_write_global_attr_operation_disable_0(void)
{
	char input[] = "0";
	struct iio_ch_info channel = {0};

	/* Setup mock expectation */
	max17616_set_operation_state_ExpectAndReturn(&test_max17616_dev, 0, 0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_global_attr(&iio_desc, input, strlen(input),
			&channel, 0);

	TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * Test writing clear_faults global attribute
 */
void test_max17616_iio_write_global_attr_clear_faults(void)
{
	char input[] = "1"; /* Input value doesn't matter for clear_faults */
	struct iio_ch_info channel = {0};

	/* Setup mock expectation */
	max17616_clear_faults_ExpectAndReturn(&test_max17616_dev, 0);

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_global_attr(&iio_desc, input, strlen(input),
			&channel, 1);

	TEST_ASSERT_EQUAL_INT(0, result);
}

/**
 * Test writing global attribute with invalid priv
 */
void test_max17616_iio_write_global_attr_invalid_priv(void)
{
	char input[] = "test";
	struct iio_ch_info channel = {0};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_write_global_attr(&iio_desc, input, strlen(input),
			&channel, 99);

	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}

/*******************************************************************************
 *    ERROR CONDITION TESTS
 ******************************************************************************/

/**
 * Test invalid channel address
 */
void test_max17616_iio_read_attr_invalid_channel(void)
{
	char buffer[32];
	struct iio_ch_info channel = {
		.ch_num = 99, /* Invalid channel */
		.address = 99
	};

	/* Create IIO descriptor */
	struct max17616_iio_desc iio_desc = {
		.max17616_dev = &test_max17616_dev
	};

	int result = max17616_iio_read_attr(&iio_desc, buffer, sizeof(buffer), &channel,
					    0);

	TEST_ASSERT_EQUAL_INT(-EINVAL, result);
}
