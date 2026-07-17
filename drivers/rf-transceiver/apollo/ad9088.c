// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include <string.h>

#include "adi_cms_api_common.h"
#include "adi_apollo_startup.h"
#include "adi_apollo_device.h"
#include "adi_apollo_mailbox.h"

#define INDIRECT_REG_TEST_ADDR  (0x60366045)
#define ARM_REG_TEST_BASE_ADDR  (0x20000000U)

static const uint8_t lanes_all[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
	14, 15, 16, 17, 18, 19, 20, 21, 22, 23
};

static const char *adi_cms_error_to_string(int error_code)
{
	switch (error_code) {
	case API_CMS_ERROR_OK:
		return "No Error";
	case API_CMS_ERROR_ERROR:
		return "General Error";
	case API_CMS_ERROR_NULL_PARAM:
		return "Null parameter";
	case API_CMS_ERROR_OVERFLOW:
		return "General overflow";
	case API_CMS_ERROR_DIV_BY_ZERO:
		return "Divide by zero";
	case API_CMS_ERROR_FEAT_LOCKOUT:
		return "Device feature is locked out";
	case API_CMS_ERROR_SPI_SDO:
		return "Wrong SDO value in device structure";
	case API_CMS_ERROR_INVALID_HANDLE_PTR:
		return "Invalid device handler pointer";
	case API_CMS_ERROR_INVALID_XFER_PTR:
		return "Invalid SPI xfer function pointer";
	case API_CMS_ERROR_INVALID_DELAYUS_PTR:
		return "Invalid delay_us function pointer";
	case API_CMS_ERROR_INVALID_PARAM:
		return "Invalid parameter";
	case API_CMS_ERROR_INVALID_RESET_CTRL_PTR:
		return "Invalid reset control function pointer";
	case API_CMS_ERROR_NOT_SUPPORTED:
		return "Not supported";
	case API_CMS_ERROR_INVALID_MASK_SELECT:
		return "Invalid bitmask select";
	case API_CMS_ERROR_IN_REF_STATUS:
		return "Input reference signal not available";
	case API_CMS_ERROR_VCO_OUT_OF_RANGE:
		return "VCO out of range";
	case API_CMS_ERROR_PLL_NOT_LOCKED:
		return "PLL not locked";
	case API_CMS_ERROR_DLL_NOT_LOCKED:
		return "DLL not locked";
	case API_CMS_ERROR_MODE_NOT_IN_TABLE:
		return "JESD mode not in table";
	case API_CMS_ERROR_CLK_CKT:
		return "Clock circuit error";
	case API_CMS_ERROR_FTW_LOAD_ACK:
		return "FTW acknowledge not received";
	case API_CMS_ERROR_NCO_NOT_ENABLED:
		return "NCO not enabled";
	case API_CMS_ERROR_INIT_SEQ_FAIL:
		return "Initialization sequence failed";
	case API_CMS_ERROR_TEST_FAILED:
		return "Test failed";
	case API_CMS_ERROR_SPI_XFER:
		return "SPI transfer error";
	case API_CMS_ERROR_TX_EN_PIN_CTRL:
		return "TX enable function error";
	case API_CMS_ERROR_RESET_PIN_CTRL:
		return "HW reset function error";
	case API_CMS_ERROR_EVENT_HNDL:
		return "Event handling error";
	case API_CMS_ERROR_HW_OPEN:
		return "HW open function error";
	case API_CMS_ERROR_HW_CLOSE:
		return "HW close function error";
	case API_CMS_ERROR_LOG_OPEN:
		return "Log open error";
	case API_CMS_ERROR_LOG_WRITE:
		return "Log write error";
	case API_CMS_ERROR_LOG_CLOSE:
		return "Log close error";
	case API_CMS_ERROR_DELAY_US:
		return "Delay error";
	case API_CMS_ERROR_HSCI_LINK_UP:
		return "HSCI linkup error";
	case API_CMS_ERROR_SPI_REGIO_XFER:
		return "SPI register transaction error";
	case API_CMS_ERROR_HSCI_REGIO_XFER:
		return "HSCI register transaction error";
	case API_CMS_ERROR_OPERATION_TIMEOUT:
		return "Operation timeout";
	case API_CMS_ERROR_LINK_DOWN:
		return "JESD links down";
	case API_CMS_ERROR_FILE_OPEN:
		return "File open error";
	case API_CMS_ERROR_SERDES_CAL_ERROR:
		return "SERDES cal error";
	case API_CMS_ERROR_SERDES_CAL_TIMEOUT:
		return "SERDES cal timeout";
	case API_CMS_ERROR_PLATFORM_READ:
		return "Platform read error";
	case API_CMS_ERROR_PLATFORM_WRITE:
		return "Platform write error";
	case API_CMS_ERROR_FILE_READ:
		return "File read error";
	case API_CMS_ERROR_FILE_WRITE:
		return "File write error";
	case API_CMS_ERROR_FILE_OPERATION:
		return "General file error";
	case API_CMS_ERROR_PLATFORM_IMAGE_LOAD:
		return "Platform FPGA image load error";
	case API_CMS_ERROR_NOT_IMPLEMENTED:
		return "Not implemented";
	case API_CMS_ERROR_STRUCT_UNPOPULATED:
		return "Struct not populated";
	case API_CMS_ERROR_PROTOCOL_OP_NOT_SUPPORTED:
		return "Protocol not supported for operation";
	case API_CMS_ERROR_INVALID_CLK_OR_REF_PARAM:
		return "Invalid clock or reference parameter";
	case API_CMS_ERROR_MEM_ALLOC:
		return "Memory allocation error";
	case API_CMS_ERROR_MMAP:
		return "Memory mapping error";
	case API_CMS_ERROR_DEV_MEM_OPEN:
		return "Device memory open error";
	case API_CMS_ERROR_I2C_ERROR:
		return "I2C error";
	case API_CMS_ERROR_I2C_WRITE:
		return "I2C write failed";
	case API_CMS_ERROR_I2C_READ:
		return "I2C read failed";
	case API_CMS_ERROR_I2C_BUSY:
		return "I2C busy";
	case API_CMS_ERROR_PMOD_NVM_LOCK:
		return "Power module NVM fault";
	case API_CMS_ERROR_EC_RAM_LOCK_ERROR:
		return "EC ram-lock error";
	case API_CMS_ERROR_PROFILE_CRC:
		return "Profile CRC invalid";
	case API_CMS_ERROR_MAILBOX_RESP_STATUS:
		return "Mailbox response status error";
	case API_CMS_ERROR_MCS_CAL_CONFIG_ERROR:
		return "MCS cal configuration error";
	case API_CMS_ERROR_MCS_INIT_CAL_ERROR:
		return "MCS init cal error";
	case API_CMS_ERROR_MCS_TRACK_CAL_ERROR:
		return "MCS tracking cal error";
	case API_CMS_ERROR_MCS_CAL_TIMEOUT:
		return "MCS cal timeout";
	case API_CMS_ERROR_ADC_INIT_CAL_ERROR:
		return "ADC init cal error";
	case API_CMS_ERROR_ADC_TRACK_CAL_ERROR:
		return "ADC tracking cal error";
	case API_CMS_ERROR_ADC_CAL_TIMEOUT:
		return "ADC cal timeout";
	case API_CMS_ERROR_BAD_STATE:
		return "Device in wrong state";
	case API_CMS_ERROR_STARTUP_FW_RDY_FOR_PROFILE_ERROR:
		return "FW not ready for profile config";
	case API_CMS_ERROR_STARTUP_FW_MAILBOX_RDY_ERROR:
		return "FW mailbox not ready";
	case API_CMS_ERROR_PLATFORM_CAPTURE_INVALID_CONFIG:
		return "Invalid platform capture config";
	default:
		return "Unknown error";
	}
}

int ad9088_check_apollo_error(int ret, const char *api_name)
{
	if (ret != API_CMS_ERROR_OK) {
		pr_err("%s failed: %s (%d)\n",
		       api_name, adi_cms_error_to_string(ret), ret);
		return -EIO;
	}

	return 0;
}
struct fw_entry {
	const uint8_t *start;
	const uint8_t *end;
};

static const struct fw_entry fw_table[ADI_APOLLO_FW_ID_MAX] = {
	[ADI_APOLLO_FW_ID_SECR_BOOT_HDR_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x01030000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x01030000_bin_end,
	},
	[ADI_APOLLO_FW_ID_CORE_0_TYE_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x20000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x20000000_bin_end,
	},
	[ADI_APOLLO_FW_ID_CORE_1_TYE_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x02000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x02000000_bin_end,
	},
	[ADI_APOLLO_FW_ID_TYE_OPER_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x21000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_B_flash_image_0x21000000_bin_end,
	},
	[ADI_APOLLO_FW_ID_PROD_SECR_BOOT_HDR_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x01030000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x01030000_bin_end,
	},
	[ADI_APOLLO_FW_ID_PROD_CORE_0_TYE_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x20000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x20000000_bin_end,
	},
	[ADI_APOLLO_FW_ID_PROD_CORE_1_TYE_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x02000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x02000000_bin_end,
	},
	[ADI_APOLLO_FW_ID_PROD_TYE_OPER_FW_BIN] = {
		.start = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x21000000_bin_start,
		.end   = _binary_drivers_rf_transceiver_apollo_firmware_app_signed_encrypted_prod_B_flash_image_0x21000000_bin_end,
	},
};

static int ad9088_fw_provider_get(adi_apollo_fw_provider_t *obj,
				  adi_apollo_startup_fw_id_e fw_id,
				  uint8_t **byte_arr, uint32_t *bytes_read)
{
	if (fw_id >= ADI_APOLLO_FW_ID_MAX || !fw_table[fw_id].start) {
		pr_err("Unknown firmware ID %d\n", fw_id);
		return API_CMS_ERROR_INVALID_PARAM;
	}

	*byte_arr = (uint8_t *)fw_table[fw_id].start;
	*bytes_read = fw_table[fw_id].end - fw_table[fw_id].start;

	return API_CMS_ERROR_OK;
}

static int ad9088_fw_provider_close(adi_apollo_fw_provider_t *obj,
				    adi_apollo_startup_fw_id_e fw_id)
{
	return API_CMS_ERROR_OK;
}


static int ad9088_spi_xfer(void *dev_obj, uint8_t *wbuf, uint8_t *rbuf,
			   uint32_t len)
{
	struct ad9088_phy *phy = dev_obj;
	int ret;
	
	ret = no_os_spi_write_and_read(phy->spi, wbuf, len);
	if (ret) {
		pr_err("SPI transfer failed: %d\n", ret);
		return ret;
	}

	memcpy(rbuf, wbuf, len);

	return 0;
}

static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[],
			   uint8_t rx_data[], uint32_t num_tx_rx_bytes,
			   adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t buf[16];
	int ret;

	if (num_tx_rx_bytes > sizeof(buf))
		return -EINVAL;

	memcpy(buf, tx_data, num_tx_rx_bytes);
	ret = no_os_spi_write_and_read(phy->spi, buf, num_tx_rx_bytes);
	if (ret) {
		pr_err("SPI transfer failed: %d\n", ret);
		return ret;
	}

	memcpy(rx_data, buf, num_tx_rx_bytes);

	return 0;
}

static int ad9088_spi_write(void *dev_obj, const uint8_t tx_data[],
			    uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t buf[16];

	if (num_tx_bytes > sizeof(buf))
		return -EINVAL;

	memcpy(buf, tx_data, num_tx_bytes);

	return no_os_spi_write_and_read(phy->spi, buf, num_tx_bytes);
}

static int ad9088_reset_pin_ctrl(void *user_data, uint8_t enable)
{
	struct ad9088_phy *phy = user_data;

	return no_os_gpio_set_value(phy->reset_gpio, enable);	
}

static int ad9088_udelay(void *user_data, unsigned int us)
{
	//us = us * 2;
	no_os_udelay(us);
	return 0;
}

int ad9088_log_write(void *user_data, int32_t log_type, const char *message,
		     va_list argp)
{
	struct ad9088_phy *phy = user_data;
	char logMessage[160];

	if (log_type == ADI_CMS_LOG_SPI)
		return 0;

	vsnprintf(logMessage, sizeof(logMessage), message, argp);

	switch (log_type) {
	case ADI_CMS_LOG_NONE:
		break;
	case ADI_CMS_LOG_MSG:
		pr_info("%s", logMessage);
		break;
	case ADI_CMS_LOG_WARN:
		pr_warning("%s", logMessage);
		break;
	case ADI_CMS_LOG_ERR:
		pr_err("%s", logMessage);
		break;
	case ADI_CMS_LOG_SPI:
		pr_debug("%s", logMessage);
		break;
	case ADI_CMS_LOG_API:
		pr_debug("%s", logMessage);
		break;
	case ADI_CMS_LOG_ALL:
		printf(logMessage);
		break;
	}

	return 0;
}

static int ad9088_reg_test(adi_apollo_device_t *device)
{
	int32_t err;
	uint32_t i, data32;
	uint8_t data8, stat;
	adi_apollo_hal_protocol_e protocol;

	const uint32_t direct_addr[] = { 0x4700000a, 0x4700000a, 0x47000200, 0x47000200 };    // HSCI can't write 0x4700000A, SPI only
	const uint8_t  direct_data[] = { 0x55,       0xaa,       0xcc,       0x33 };

	uint32_t indirect_addr[] = { INDIRECT_REG_TEST_ADDR + 0, INDIRECT_REG_TEST_ADDR + 1,
					INDIRECT_REG_TEST_ADDR + 2, INDIRECT_REG_TEST_ADDR + 3 }; /* indirect register address */
	const uint8_t  indirect_data[] = { 0x12,       0x34,       0x56,       0x78 };

	uint32_t arm_addr[] = { ARM_REG_TEST_BASE_ADDR + 0, ARM_REG_TEST_BASE_ADDR + 4,
				ARM_REG_TEST_BASE_ADDR + 8, ARM_REG_TEST_BASE_ADDR + 12 }; /* ARM core1 register addresses */
	const uint32_t arm_data[] = { 0x55aa55aa, 0xdeadbeef, 0xbeefdead, 0xaa55aa55 };

	adi_apollo_hal_active_protocol_get(device, &protocol);

	/* Direct register SPI scratch loop rd/wr test */
	stat = 0;
	for (i=0; i<sizeof(direct_addr) / sizeof(direct_addr[0]); i++) {

		if ((protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) && (direct_addr[i] <= 0x4700000Fu)) {
			continue;
		}

		if (err = adi_apollo_hal_reg_set(device, direct_addr[i], direct_data[i]), err != API_CMS_ERROR_OK) {
			return err;
		}
		if (err = adi_apollo_hal_reg_get(device, direct_addr[i], &data8), err != API_CMS_ERROR_OK) {
			return err;
		}

		if (data8 != direct_data[i]) {
			pr_err("data8 0x%X != direct_data[i] 0x%X\n", data8, direct_data[i]);
			stat = 1;
		}
	}

	if (stat != 0) {
		pr_err("Test direct register *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test direct register Passed\n");

	/* Indirect register SPI loop rd/wr test */
	stat = 0;
	for (i = 0; i < sizeof(indirect_addr) / sizeof(indirect_addr[0]); i++) {
		if (err = adi_apollo_hal_reg_set(device, indirect_addr[i], indirect_data[i]), err != API_CMS_ERROR_OK) {
			return err;
		}
		if (err = adi_apollo_hal_reg_get(device, indirect_addr[i], &data8), err != API_CMS_ERROR_OK) {
			return err;
		}

		if (data8 != indirect_data[i]) {
			pr_err("Test indirect register 0x%X - 0x%X\n", data8, indirect_data[i]);
			stat = 1;
		}
	}

	if (stat != 0) {
		pr_err("Test indirect register *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test indirect register Passed\n");

	/* 32-bit ARM mem rd/wr test */
	stat = 0;
	for (i = 0; i < sizeof(arm_addr) / sizeof(arm_addr[0]); i++) {
		if (err = adi_apollo_hal_reg32_set(device, arm_addr[i], arm_data[i]), err != API_CMS_ERROR_OK) {
			return err;
		}
		if (err = adi_apollo_hal_reg32_get(device, arm_addr[i], &data32), err != API_CMS_ERROR_OK) {
			return err;
		}


		if (data32 != arm_data[i]) {
			pr_err("Test ARM memory 0x%X - 0x%X\n", data32, arm_data[i]);
			stat = 1;
		} else {
			stat = 0;
		}
	}

	if (stat != 0) {
		pr_err("Test ARM memory *** FAILED ***\n");
		return API_CMS_ERROR_ERROR;
	}
	pr_info("Test ARM memory Passed\n");

	return API_CMS_ERROR_OK;
}

static int ad9088_version_info(struct ad9088_phy *phy)
{
	adi_apollo_mailbox_resp_get_fw_version_t fw_ver;
	adi_apollo_mailbox_cmd_ping_t ping_cmd;
	adi_apollo_mailbox_resp_ping_t ping_resp;
	adi_apollo_device_t *device = &phy->ad9088;
	int ret;
	uint16_t maj, min, rc;

	ret = adi_apollo_device_api_revision_get(device, &maj, &min, &rc);
	if (ret) {
		pr_err("API revision get error (%d)\n", ret);
		return ret;
	}

	pr_info("API ver: %d.%d.%d\n", maj, min, rc);

	ret = adi_apollo_mailbox_ready_check(device);
	if (ret) {
		pr_err("Mailbox Ready error (%d)\n", ret);
		return ret;
	}

	ping_cmd.echo_data = 0x00000000;
	ret = adi_apollo_mailbox_ping(device, &ping_cmd, &ping_resp);
	if (ret) {
		pr_err("Mailbox ping error (%d)\n", ret);
		return ret;
	}

	pr_info("Ping (core 1) cmd/res: 0x%08x/0x%08x\n",
		(unsigned int)ping_cmd.echo_data,
		(unsigned int)ping_resp.echo_data);

	ping_cmd.echo_data = 0x12345678;
	ret = adi_apollo_mailbox_ping(device, &ping_cmd, &ping_resp);
	if (ret) {
		pr_err("Mailbox ping error (%d)\n", ret);
		return ret;
	}

	pr_info("Ping (core 0) cmd/res: 0x%08x/0x%08x\n",
		(unsigned int)ping_cmd.echo_data,
		(unsigned int)ping_resp.echo_data);

	ret = adi_apollo_mailbox_get_fw_version(device, &fw_ver);
	if (ret) {
		pr_err("Mailbox fw error (%d)\n", ret);
		return ret;
	}

	pr_info("FW ver: %d.%d.%d\n", fw_ver.major, fw_ver.minor,
		fw_ver.patch);

	return 0;
}

int ad9088_init(struct ad9088_phy **device,
		const struct ad9088_init_param *init_param)
{
	struct ad9088_phy *phy;
	uint16_t api_rev[3];
	int ret;

	ret = ad9088_parse_struct(&phy, init_param);
	if (ret)
		return ret;

	phy->ad9088.hal_info.spi0_desc.spi_config.sdo = (phy->spi_3wire_en) ?
			ADI_APOLLO_DEVICE_SPI_SDIO : ADI_APOLLO_DEVICE_SPI_SDO;
	phy->ad9088.hal_info.spi0_desc.spi_config.msb = ADI_APOLLO_DEVICE_SPI_MSB_FIRST;
	phy->ad9088.hal_info.spi0_desc.spi_config.addr_inc = ADI_APOLLO_DEVICE_SPI_ADDR_INC_AUTO;
	phy->ad9088.hal_info.spi0_desc.is_used = 1;
	phy->ad9088.hal_info.spi0_desc.dev_obj = phy;
	phy->ad9088.hal_info.user_data = phy;
	phy->ad9088.hal_info.spi0_desc.read = ad9088_spi_read;
	phy->ad9088.hal_info.spi0_desc.write = ad9088_spi_write;
	phy->ad9088.hal_info.spi0_desc.xfer = ad9088_spi_xfer;
	phy->ad9088.hal_info.delay_us = ad9088_udelay;
	phy->ad9088.hal_info.reset_pin_ctrl = ad9088_reset_pin_ctrl;
	phy->ad9088.hal_info.log_write = ad9088_log_write;

	phy->fw_provider.desc = "no-OS Embedded FW Provider";
	phy->fw_provider.tag = phy;
	phy->ad9088.startup_info.fw_provider = &phy->fw_provider;
	phy->ad9088.startup_info.get = ad9088_fw_provider_get;
	phy->ad9088.startup_info.close = ad9088_fw_provider_close;
	phy->ad9088.startup_info.open = NULL;

	ret = adi_apollo_device_hw_open(&phy->ad9088,
					phy->reset_gpio ? ADI_APOLLO_HARD_RESET_AND_INIT :
					ADI_APOLLO_SOFT_RESET_AND_INIT);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_device_hw_open");
	if (ret)
		goto error;

	ret = adi_apollo_hal_active_protocol_set(&phy->ad9088,
						 ADI_APOLLO_HAL_PROTOCOL_SPI0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_hal_active_protocol_set");
	if (ret)
		goto error_hw_close;

	ret = adi_apollo_hal_rmw_enable_set(&phy->ad9088,
					    ADI_APOLLO_HAL_PROTOCOL_SPI0, 0);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_hal_rmw_enable_set");
	if (ret)
		goto error_hw_close;

	ret = ad9088_reg_test(&phy->ad9088);
	if (ret) {
		pr_err("Register test failed (%d)\n", ret);
		ret = -EIO;
		goto error_hw_close;
	}

	ret = adi_apollo_startup_execute(&phy->ad9088, &phy->profile,
					 ADI_APOLLO_STARTUP_SEQ_DEFAULT);
	ret = ad9088_check_apollo_error(ret, "adi_apollo_startup_execute");
	if (ret)
		goto error_hw_close;

	ret = ad9088_version_info(phy);
	if (ret)
		goto error_hw_close;

	ret = adi_apollo_device_chip_id_get(&phy->ad9088, &phy->chip_id);
	if (ret) {
		pr_err("chip_id failed (%d)\n", ret);
		goto error_hw_close;
	}

	adi_apollo_device_api_revision_get(&phy->ad9088,
					   &api_rev[0], &api_rev[1],
					   &api_rev[2]);

	pr_info("AD%X Rev. %u Grade %u (API %u.%u.%u) probed\n",
		phy->chip_id.prod_id, phy->chip_id.dev_revision,
		phy->chip_id.prod_grade, api_rev[0], api_rev[1],
		api_rev[2]);

	*device = phy;

	return 0;

error_hw_close:
	adi_apollo_device_hw_close(&phy->ad9088);
error:
	no_os_gpio_remove(phy->reset_gpio);
	no_os_spi_remove(phy->spi);
	no_os_free(phy);

	return ret;
}

int ad9088_remove(struct ad9088_phy *phy)
{
	if (!phy)
		return -EINVAL;

	adi_apollo_device_hw_close(&phy->ad9088);

	no_os_gpio_remove(phy->tx2_en_gpio);
	no_os_gpio_remove(phy->tx1_en_gpio);
	no_os_gpio_remove(phy->rx2_en_gpio);
	no_os_gpio_remove(phy->rx1_en_gpio);
	no_os_gpio_remove(phy->triq_req_gpio);
	no_os_gpio_remove(phy->reset_gpio);
	no_os_spi_remove(phy->spi);
	no_os_free(phy);

	return 0;
}
