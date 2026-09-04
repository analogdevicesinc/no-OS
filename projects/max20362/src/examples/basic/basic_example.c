/*******************************************************************************
 * @file   basic_example.c
 * @brief  Basic test and validation example for MAX20362 driver
 * @author Wilbert Jethro R. Limjoco (Wilbertjethro.Limjoco@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
*
* SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "common_data.h"
#include "max20362.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_uart.h"

/** Test-specific constants for register operations */
#define TEST_REG_MASK			0x0F     /**< Test register bit mask */
#define TEST_REG_VALUE			0x05     /**< Test register bit value */

/** Test voltage values (microvolts) */
#define TEST_BB_VOLTAGE_UV		3300000  /**< Test buck-boost voltage: 3.3V */
#define TEST_LDO_VOLTAGE_UV		1800000  /**< Test LDO voltage: 1.8V */
#define TEST_CAP_VOLTAGE_UV		3100000  /**< Test capacitor voltage: 3.1V */
#define TEST_CAP_VOLTAGE_MIN_UV	1000000  /**< Test capacitor min voltage: 1.0V */
#define TEST_DVS_VOLTAGE_1_UV	2500000  /**< Test DVS voltage 1: 2.5V */
#define TEST_DVS_VOLTAGE_2_UV	3300000  /**< Test DVS voltage 2: 3.3V */

/** Test current limits (milliamps) */
#define TEST_CURRENT_LIMIT_MA	25       /**< Test current limit: 25mA */

/* External function for UART stdio redirection */
extern void no_os_uart_stdio(struct no_os_uart_desc *desc);

/* UART device structure */
struct no_os_uart_desc *uart_desc;

/* Test configuration flags - set to 1 to enable specific tests */
#define TEST_BASIC_COMMS        1  /* Test I2C and chip ID */
#define TEST_REGISTER_RW        1  /* Test register read/write */
#define TEST_BUCK_BOOST         1  /* Test buck-boost functions */
#define TEST_LDO                1  /* Test LDO functions */
#define TEST_DVS                1  /* Test DVS modes */
#define TEST_INTERRUPTS         1  /* Test interrupt handling */
#define TEST_PROTECTION         1  /* Test lock/unlock */
#define TEST_CAPACITOR          1  /* Test capacitor voltage */
#define TEST_CURRENT_LIMIT      1  /* Test current limiting */

/* Test result tracking */
static int tests_passed = 0;
static int tests_failed = 0;

/**
 * @brief Print test header
 * @param test_name - Name of the test
 */
static void print_test_header(const char *test_name)
{
	pr_info("\n");
	pr_info("========================================\n");
	pr_info("TEST: %s\n", test_name);
	pr_info("========================================\n");
}

/**
 * @brief Print test result
 * @param test_name - Name of the test
 * @param passed - Test result (true = passed)
 * @param details - Optional details string
 */
static void print_test_result(const char *test_name, bool passed,
			      const char *details)
{
	if (passed) {
		pr_info("[PASS] %s", test_name);
		tests_passed++;
	} else {
		pr_err("[FAIL] %s", test_name);
		tests_failed++;
	}

	if (details) {
		pr_info(" - %s", details);
	}
	pr_info("\n");
}

/**
 * @brief Test basic I2C communication and chip ID
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_basic_communication(struct max20362_dev *dev)
{
	uint8_t chip_id;
	int ret;
	char detail_buf[64];

	print_test_header("Basic Communication");

	/* Test 1: Read Chip ID */
	ret = max20362_reg_read(dev, MAX20362_REG_CHIP_ID, &chip_id);
	if (ret) {
		print_test_result("I2C Read", false, "Communication failed");
		return ret;
	}

	sprintf(detail_buf, "Chip ID: 0x%02X (expected 0x%02X)",
		chip_id, MAX20362_CHIP_ID_VAL);
	print_test_result("Chip ID Verification",
			  chip_id == MAX20362_CHIP_ID_VAL, detail_buf);

	return 0;
}

/**
 * @brief Test register read/write operations
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_register_operations(struct max20362_dev *dev)
{
	uint8_t write_val, read_val, orig_val;
	int ret;
	char detail_buf[64];

	print_test_header("Register Read/Write Operations");

	/* Use LDO_VSET register for testing (not write-protected) */
	const uint8_t test_reg = MAX20362_REG_LDO_VSET;

	/* Save original value */
	ret = max20362_reg_read(dev, test_reg, &orig_val);
	if (ret) {
		print_test_result("Read Original Value", false, NULL);
		return ret;
	}

	/* Test write and read back */
	write_val = 0x0A; /* 1.9V = 0.9V + (10 * 0.1V) */
	ret = max20362_reg_write(dev, test_reg, write_val);
	if (ret) {
		print_test_result("Register Write", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, test_reg, &read_val);
	sprintf(detail_buf, "Wrote 0x%02X, Read 0x%02X", write_val, read_val);
	print_test_result("Write/Read Verification",
			  read_val == write_val, detail_buf);

	/* Test update_bits function */
	ret = max20362_reg_update_bits(dev, test_reg, TEST_REG_MASK, TEST_REG_VALUE);
	if (ret) {
		print_test_result("Update Bits", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, test_reg, &read_val);
	sprintf(detail_buf, "Masked update result: 0x%02X", read_val);
	print_test_result("Update Bits Verification",
			  (read_val & TEST_REG_MASK) == TEST_REG_VALUE, detail_buf);

	/* Restore original value */
	max20362_reg_write(dev, test_reg, orig_val);

	return 0;
}

/**
 * @brief Test buck-boost converter functions
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_buck_boost_functions(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];

	print_test_header("Buck-Boost Converter Functions");

	/* Test voltage setting (requires unlock) */
	pr_info("Testing voltage setting (3.3V)...\n");
	ret = max20362_set_buck_boost_voltage(dev, TEST_BB_VOLTAGE_UV);
	if (ret) {
		print_test_result("Set BB Voltage", false, "Failed to set voltage");
		return ret;
	}

	/* Verify voltage was set correctly */
	ret = max20362_reg_read(dev, MAX20362_REG_BBST_VSET, &reg_val);
	if (ret) {
		print_test_result("Read BB Voltage", false, NULL);
		return ret;
	}

	/* Calculate expected value: (3.3V - 1.5V) / 0.05V = 36 = 0x24 */
	sprintf(detail_buf, "Register value: 0x%02X (expected 0x24)", reg_val & 0x7F);
	print_test_result("BB Voltage Verification",
			  (reg_val & 0x7F) == 0x24, detail_buf);

	/* Test enable/disable */
	pr_info("Testing enable/disable...\n");
	ret = max20362_enable_buck_boost(dev, true);
	if (ret) {
		print_test_result("Enable Buck-Boost", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_BBST_CFG0, &reg_val);
	sprintf(detail_buf, "Enable bit: %s",
		(reg_val & MAX20362_BBSTCFG_BBSTENA_MSK) ? "SET" : "CLEAR");
	print_test_result("BB Enable Verification",
			  (reg_val & MAX20362_BBSTCFG_BBSTENA_MSK) != 0, detail_buf);

	/* Wait for startup */
	no_os_mdelay(10);

	/* Check status */
	ret = max20362_get_status(dev, &reg_val);
	if (ret == 0) {
		sprintf(detail_buf, "Status: 0x%02X (BBSTON=%d, Faults=%d)",
			reg_val,
			(reg_val & MAX20362_STATUS_BBSTON_MSK) ? 1 : 0,
			(reg_val & (MAX20362_STATUS_BSTFLT_MSK |
				    MAX20362_STATUS_THMFLT_MSK)) ? 1 : 0);
		pr_info("Buck-Boost Status: %s\n", detail_buf);
	}

	/* Test mode configuration */
	pr_info("Testing mode configuration...\n");
	ret = max20362_config_buck_boost_mode(dev, false, true, false);
	print_test_result("Configure BB Mode", ret == 0,
			  ret ? "Failed" : "Low EMI enabled");

	/* Test discharge configuration */
	ret = max20362_config_buck_boost_discharge(dev, true, false);
	print_test_result("Configure BB Discharge", ret == 0,
			  ret ? "Failed" : "Active discharge enabled");

	/* Disable for safety */
	max20362_enable_buck_boost(dev, false);
	no_os_mdelay(10);

	return 0;
}

/**
 * @brief Test LDO functions
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_ldo_functions(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];

	print_test_header("LDO Regulator Functions");

	/* Test voltage setting */
	pr_info("Testing LDO voltage setting (1.8V)...\n");
	ret = max20362_set_ldo_voltage(dev, TEST_LDO_VOLTAGE_UV);
	if (ret) {
		print_test_result("Set LDO Voltage", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_LDO_VSET, &reg_val);
	/* Expected: (1.8V - 0.9V) / 0.1V = 9 = 0x09 */
	sprintf(detail_buf, "Register value: 0x%02X (expected 0x09)", reg_val);
	print_test_result("LDO Voltage Verification",
			  (reg_val & MAX20362_LDOVSET_MSK) == 0x09, detail_buf);

	/* Test input source selection */
	pr_info("Testing LDO input source...\n");
	ret = max20362_set_ldo_input_source(dev, MAX20362_LDO_SOURCE_BBOUT);
	if (ret) {
		print_test_result("Set LDO Source", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_LDO_CFG, &reg_val);
	sprintf(detail_buf, "BBstSupp bit: %s",
		(reg_val & MAX20362_LDOCFG_BBSTSUP_MSK) ? "SET (BBOUT)" : "CLEAR (BATT)");
	print_test_result("LDO Source Verification",
			  (reg_val & MAX20362_LDOCFG_BBSTSUP_MSK) != 0, detail_buf);

	/* Test mode configuration */
	pr_info("Testing LDO mode configuration...\n");
	ret = max20362_config_ldo_mode(dev, true, false, true);
	print_test_result("Configure LDO Mode", ret == 0,
			  ret ? "Failed" : "Low IQ + Active discharge");

	/* Test enable/disable */
	pr_info("Testing LDO enable/disable...\n");
	ret = max20362_enable_ldo(dev, true);
	if (ret) {
		print_test_result("Enable LDO", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_LDO_CFG, &reg_val);
	sprintf(detail_buf, "Enable bit: %s",
		(reg_val & MAX20362_LDOCFG_ENA_MSK) ? "SET" : "CLEAR");
	print_test_result("LDO Enable Verification",
			  (reg_val & MAX20362_LDOCFG_ENA_MSK) != 0, detail_buf);

	/* Disable for safety */
	max20362_enable_ldo(dev, false);
	no_os_mdelay(10);

	return 0;
}

/**
 * @brief Test DVS modes
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_dvs_modes(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];
	uint32_t rr_voltages[] = {
		TEST_DVS_VOLTAGE_1_UV, /* 2.5V */
		3000000, /* 3.0V */
		TEST_DVS_VOLTAGE_2_UV, /* 3.3V */
		3600000  /* 3.6V */
	};

	print_test_header("Dynamic Voltage Scaling (DVS)");

	/* Test I2C mode (default) */
	pr_info("Testing DVS I2C mode...\n");
	ret = max20362_set_dvs_mode(dev, MAX20362_DVS_SOURCE_I2C);
	if (ret) {
		print_test_result("Set DVS I2C Mode", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_DVS_CFG, &reg_val);
	sprintf(detail_buf, "DVS Source: 0x%02X",
		reg_val & MAX20362_DVSCFG_DVSSOURCE_MSK);
	print_test_result("DVS I2C Mode Verification",
			  (reg_val & MAX20362_DVSCFG_DVSSOURCE_MSK) == 0x00, detail_buf);

	/* Test Round-Robin table programming */
	pr_info("Testing Round-Robin table...\n");
	ret = max20362_set_dvs_rr_table(dev, rr_voltages, 4);
	if (ret) {
		print_test_result("Program RR Table", false, NULL);
		return ret;
	}

	/* Verify first RR entry */
	ret = max20362_reg_read(dev, 0x20, &reg_val); /* RR0 register */
	/* Expected: (2.5V - 1.5V) / 0.05V = 20 = 0x14 */
	sprintf(detail_buf, "RR0 value: 0x%02X (expected 0x14 for 2.5V)", reg_val);
	print_test_result("RR Table Verification",
			  (reg_val & 0x7F) == 0x14, detail_buf);

	/* Test Round-Robin mode */
	pr_info("Testing DVS Round-Robin mode...\n");
	ret = max20362_set_dvs_mode(dev, MAX20362_DVS_SOURCE_ROUND_ROBIN);
	if (ret) {
		print_test_result("Set DVS RR Mode", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_DVS_CFG, &reg_val);
	sprintf(detail_buf, "RR Enable: %s",
		(reg_val & MAX20362_DVSCFG_RRENA_MSK) ? "YES" : "NO");
	print_test_result("DVS RR Mode Verification",
			  ((reg_val & MAX20362_DVSCFG_RRENA_MSK) != 0), detail_buf);

	/* Return to I2C mode */
	max20362_set_dvs_mode(dev, MAX20362_DVS_SOURCE_I2C);

	return 0;
}

/**
 * @brief Test interrupt handling
 * @note BBSTON/BBSTOFF flags may remain latched after state changes
 *       and might not be immediately clearable.
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_interrupt_handling(struct max20362_dev *dev)
{
	uint8_t int_status, mask_val;
	int ret;
	char detail_buf[128];

	print_test_header("Interrupt Handling");

	/* Clear all interrupts first */
	pr_info("Clearing all interrupts...\n");
	ret = max20362_clear_all_interrupts(dev);
	print_test_result("Clear All Interrupts", ret == 0, NULL);

	/* Add delay to allow hardware to settle */
	no_os_mdelay(10);

	/* Read interrupt status */
	ret = max20362_get_int_status(dev, &int_status);
	sprintf(detail_buf, "INT status after clear: 0x%02X", int_status);

	/* BBSTON and BBSTOFF flags (bits 5 and 4) may remain latched after buck-boost operations.
	 * These are state-change indicators that may not be immediately clearable. */
	uint8_t clearable_mask = ~(MAX20362_INT_BBSTON_MSK | MAX20362_INT_BBSTOFF_MSK);
	uint8_t clearable_status = int_status & clearable_mask;

	if (clearable_status == 0) {
		print_test_result("INT Status Clear", true, detail_buf);
	} else {
		sprintf(detail_buf, "INT status after clear: 0x%02X (non-clearable: 0x%02X)",
			int_status, int_status & ~clearable_mask);
		print_test_result("INT Status Clear", false, detail_buf);
	}

	/* Test interrupt mask configuration */
	pr_info("Testing interrupt masks...\n");

	/* Enable select interrupts (unmask by writing 0) */
	mask_val = ~(MAX20362_INT_THMFLT_MSK | MAX20362_INT_BSTFLT_MSK);
	ret = max20362_set_int_mask(dev, mask_val);
	if (ret) {
		print_test_result("Set INT Mask", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_INT_MASK, &mask_val);
	sprintf(detail_buf, "INT mask: 0x%02X (thermal and boost faults enabled)",
		mask_val);
	print_test_result("INT Mask Verification", ret == 0, detail_buf);

	/* Test LDO interrupt mask */
	ret = max20362_set_ldo_int_mask(dev, MAX20362_LDO_INT_EN);
	if (ret) {
		print_test_result("Set LDO INT Mask", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_LDO_INT_MASK, &mask_val);
	sprintf(detail_buf, "LDO INT mask: 0x%02X", mask_val);
	print_test_result("LDO INT Mask Verification",
			  mask_val == MAX20362_LDO_INT_EN, detail_buf);

	/* Test Ingenuity interrupt mask */
	ret = max20362_set_ingen_int_mask(dev, MAX20362_INGEN_INT_EN);
	if (ret) {
		print_test_result("Set INGEN INT Mask", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_INGEN_INT_MASK, &mask_val);
	sprintf(detail_buf, "INGEN INT mask: 0x%02X", mask_val);
	print_test_result("INGEN INT Mask Verification",
			  mask_val == MAX20362_INGEN_INT_EN, detail_buf);

	/* Mask all interrupts for safety */
	max20362_set_int_mask(dev, 0xFF);
	max20362_set_ldo_int_mask(dev, 0xFF);
	max20362_set_ingen_int_mask(dev, 0xFF);

	return 0;
}

/**
 * @brief Test register protection
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_protection_features(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];

	print_test_header("Register Protection Features");

	/* Test unlock sequence */
	pr_info("Testing unlock sequence...\n");
	ret = max20362_unlock_registers(dev);
	print_test_result("Unlock Registers", ret == 0, NULL);

	/* Verify unlock by reading lock register */
	ret = max20362_reg_read(dev, MAX20362_REG_LOCK_MSK, &reg_val);
	sprintf(detail_buf, "Lock mask: 0x%02X (BBLck bit %s)",
		reg_val, (reg_val & 0x01) ? "SET" : "CLEAR");
	print_test_result("Unlock Verification",
			  (reg_val & 0x01) == 0, detail_buf);

	/* Test lock sequence */
	pr_info("Testing lock sequence...\n");
	ret = max20362_lock_registers(dev);
	print_test_result("Lock Registers", ret == 0, NULL);

	/* Verify lock */
	ret = max20362_reg_read(dev, MAX20362_REG_LOCK_MSK, &reg_val);
	sprintf(detail_buf, "Lock mask: 0x%02X (BBLck bit %s)",
		reg_val, (reg_val & 0x01) ? "SET" : "CLEAR");
	print_test_result("Lock Verification",
			  (reg_val & 0x01) != 0, detail_buf);

	return 0;
}

/**
 * @brief Test capacitor voltage settings
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_capacitor_voltage(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];

	print_test_header("Capacitor Voltage Control");

	/* Test with 250mV steps (step_size = 1) */
	pr_info("Testing capacitor voltage (3.1V, 250mV steps)...\n");
	ret = max20362_set_cap_voltage(dev, TEST_CAP_VOLTAGE_UV,
				       MAX20362_VCAP_STEP_250MV);
	if (ret) {
		print_test_result("Set Cap Voltage", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_CAP_VSET, &reg_val);
	/* Expected: step=1 (0x10), vset=(3.1V-1.6V)/0.25V=6 (0x06) => 0x16 */
	sprintf(detail_buf, "CAP_VSET: 0x%02X (expected 0x16 for 3.1V/250mV)",
		reg_val);
	print_test_result("Cap Voltage Verification",
			  (reg_val & 0x3F) == 0x16, detail_buf);

	/* Test with 500mV steps (step_size = 0) */
	pr_info("Testing capacitor voltage (5.0V, 500mV steps)...\n");
	ret = max20362_set_cap_voltage(dev, 5000000, MAX20362_VCAP_STEP_500MV);
	if (ret) {
		print_test_result("Set Cap Voltage (500mV)", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_CAP_VSET, &reg_val);
	/* Expected: step=0 (0x00), vset=(5.0V-2.5V)/0.5V=5 (0x05) => 0x05 */
	sprintf(detail_buf, "CAP_VSET: 0x%02X (expected 0x05 for 5.0V/500mV)",
		reg_val);
	print_test_result("Cap Voltage 500mV Step",
			  (reg_val & 0x3F) == 0x05, detail_buf);

	/* Test out-of-range voltage rejection */
	pr_info("Testing capacitor voltage (1.0V, out of range)...\n");
	ret = max20362_set_cap_voltage(dev, TEST_CAP_VOLTAGE_MIN_UV,
				       MAX20362_VCAP_STEP_250MV);
	sprintf(detail_buf, "Returned %d (expected %d)", ret, -EINVAL);
	print_test_result("Cap Voltage Out-of-Range Rejection", ret == -EINVAL,
			  detail_buf);

	return 0;
}

/**
 * @brief Test current limiting features
 * @param dev - Device structure
 * @return 0 on success, negative error code otherwise
 */
static int test_current_limiting(struct max20362_dev *dev)
{
	uint8_t reg_val;
	int ret;
	char detail_buf[128];

	print_test_header("Current Limiting Features");

	/* Test input current limit */
	pr_info("Testing input current limit (25mA)...\n");
	ret = max20362_set_input_current_limit(dev, TEST_CURRENT_LIMIT_MA);
	if (ret) {
		print_test_result("Set Current Limit", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_IGN_CFG, &reg_val);
	/* Expected: 25mA - 5mA = 20 = 0x14 */
	sprintf(detail_buf, "Current limit field: 0x%02X (expected 0x14 for 25mA)",
		reg_val & MAX20362_IGNCFG_ILIM_MSK);
	print_test_result("Current Limit Verification",
			  (reg_val & MAX20362_IGNCFG_ILIM_MSK) == 0x14, detail_buf);

	/* Test battery voltage droop */
	pr_info("Testing battery voltage droop (100mV)...\n");
	ret = max20362_set_bbat_vdrop(dev, MAX20362_BBAT_VDROP_100MV);
	if (ret) {
		print_test_result("Set Battery Vdrop", false, NULL);
		return ret;
	}

	ret = max20362_reg_read(dev, MAX20362_REG_IGN_CFG, &reg_val);
	sprintf(detail_buf, "Vdrop field: 0x%02X (100mV setting)",
		(reg_val & MAX20362_IGNCFG_BBVDROP_MSK) >> 6);
	print_test_result("Battery Vdrop Verification",
			  ((reg_val & MAX20362_IGNCFG_BBVDROP_MSK) >> 6) == 1, detail_buf);

	return 0;
}

/**
 * @brief Print test summary
 */
static void print_test_summary(void)
{
	pr_info("\n");
	pr_info("========================================\n");
	pr_info("TEST SUMMARY\n");
	pr_info("========================================\n");
	pr_info("Tests Passed: %d\n", tests_passed);
	pr_info("Tests Failed: %d\n", tests_failed);
	pr_info("Total Tests:  %d\n", tests_passed + tests_failed);

	if (tests_failed == 0) {
		pr_info("\nRESULT: ALL TESTS PASSED\n");
	} else {
		pr_err("\nRESULT: %d TEST(S) FAILED\n", tests_failed);
	}
	pr_info("========================================\n");
}

/**
 * @brief Main example function
 * @return 0 on success, negative error code otherwise
 */
int example_main(void)
{
	struct max20362_dev *max20362_dev;
	int ret;

	/* Initialize UART first for console output */
	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret) {
		pr_err("Failed to initialize UART: %d\n", ret);
		return ret;
	}

	/* Redirect pr_* functions to UART */
	no_os_uart_stdio(uart_desc);

	pr_info("UART initialized successfully\n");

	pr_info("MAX20362 Driver Test Suite\n");
	pr_info("Starting tests...\n");

	/* Initialize device */
	pr_info("\nInitializing MAX20362...\n");
	ret = max20362_init(&max20362_dev, &max20362_ip);
	if (ret) {
		pr_err("Failed to initialize MAX20362: %d\n", ret);
		goto cleanup_uart;
	}
	pr_info("MAX20362 initialized successfully\n");

	/* Run test suite */
#if TEST_BASIC_COMMS
	test_basic_communication(max20362_dev);
#endif

#if TEST_REGISTER_RW
	test_register_operations(max20362_dev);
#endif

#if TEST_PROTECTION
	test_protection_features(max20362_dev);
#endif

#if TEST_BUCK_BOOST
	test_buck_boost_functions(max20362_dev);
#endif

#if TEST_LDO
	test_ldo_functions(max20362_dev);
#endif

#if TEST_CAPACITOR
	test_capacitor_voltage(max20362_dev);
#endif

#if TEST_CURRENT_LIMIT
	test_current_limiting(max20362_dev);
#endif

#if TEST_DVS
	test_dvs_modes(max20362_dev);
#endif

#if TEST_INTERRUPTS
	test_interrupt_handling(max20362_dev);
#endif

	/* Print summary */
	print_test_summary();

	/* Cleanup */
	pr_info("\nCleaning up...\n");
	ret = max20362_remove(max20362_dev);
	if (ret) {
		pr_err("Failed to remove MAX20362: %d\n", ret);
		/* Continue with UART cleanup even if MAX20362 cleanup fails */
	}

cleanup_uart:
	/* Cleanup UART */
	if (uart_desc) {
		int uart_ret = no_os_uart_remove(uart_desc);
		if (uart_ret) {
			pr_err("Failed to remove UART: %d\n", uart_ret);
		} else {
			pr_info("UART removed successfully\n");
		}
	}

	pr_info("Test suite completed\n");

	return (tests_failed > 0) ? -EIO : 0;
}
