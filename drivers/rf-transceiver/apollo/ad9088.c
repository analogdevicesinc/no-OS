// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for AD9088 and similar mixed signal front end (MxFE®)
 *
 * Copyright 2022 Analog Devices Inc.
 */

#include "ad9088.h"
#include "no_os_delay.h"

//#define DEBUG

#define APOLLO_FW_CPU0_NAME "APOLLO_FW_CPU0_B.bin"
#define APOLLO_FW_CPU1_NAME "APOLLO_FW_CPU1_B.bin"
#define INDIRECT_REG_TEST_ADDR  (0x60366045)
#define ARM_REG_TEST_BASE_ADDR  (0x20000000U)

#define SECR_BOOT_HDR_BIN	"app_signed_encrypted_B/flash_image_0x01030000.bin"
#define CORE_0_TYE_FW_BIN	"app_signed_encrypted_B/flash_image_0x20000000.bin"
#define CORE_1_TYE_FW_BIN	"app_signed_encrypted_B/flash_image_0x02000000.bin"
#define TYE_OPER_FW_BIN		"app_signed_encrypted_B/flash_image_0x21000000.bin"

#define PROD_SECR_BOOT_HDR_BIN	"app_signed_encrypted_prod_B/flash_image_0x01030000.bin"
#define PROD_CORE_0_TYE_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x20000000.bin"
#define PROD_CORE_1_TYE_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x02000000.bin"
#define PROD_TYE_OPER_FW_BIN	"app_signed_encrypted_prod_B/flash_image_0x21000000.bin"


#define SECR_BOOT_HDR_MEM_ADDR  (0x01030000U)
#define CORE_0_TYE_FW_MEM_ADDR  (0x20000000U)
#define CORE_1_TYE_FW_MEM_ADDR  (0x02000000U)
#define TYE_OPER_FW_MEM_ADDR    (0x21000000U)

static const uint8_t lanes_all[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};
//static int ad9088_trig_sync(struct ad9088_phy *phy, int val);

static int ad9088_jesd204_post_setup_stage1(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);
static int ad9088_jesd204_post_setup_stage2(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);
static int ad9088_jesd204_post_setup_stage3(struct jesd204_dev *jdev,
				       enum jesd204_state_op_reason reason);


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

	*rbuf = *wbuf; 
	
	return 0;
}

static int ad9088_spi_read(void *dev_obj, const uint8_t tx_data[],
			   uint8_t rx_data[], uint32_t num_tx_rx_bytes,
			   adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t *tx = tx_data;
	uint32_t idx;
	int ret;
	
	ret = no_os_spi_write_and_read(phy->spi, tx, num_tx_rx_bytes);
	if (ret) {
		pr_err("SPI transfer failed: %d\n", ret);
		return ret;
	}

	for (idx = 0; idx < num_tx_rx_bytes; idx++)
		rx_data[idx] = tx[idx];

	return 0;
}

static int ad9088_spi_write(void *dev_obj, const uint8_t tx_data[],
			    uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config)
{
	struct ad9088_phy *phy = dev_obj;
	uint8_t *tx = tx_data;
	
	return no_os_spi_write_and_read(phy->spi, tx, num_tx_bytes);

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

	pr_err("Test direct register %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
	if (stat != 0) {
		return API_CMS_ERROR_ERROR;
	}

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

	pr_err("Test indirect register %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
	if (stat != 0) {
		return API_CMS_ERROR_ERROR;
	}

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

	pr_err("Test ARM memory %s\n", (0 == stat) ? "Passed" : "*** FAILED ***");
	if (stat != 0) {
		return API_CMS_ERROR_ERROR;
	}

	return API_CMS_ERROR_OK;
}

int ad9088_init(struct ad9088_phy **device, 
		const struct ad9088_init_param *init_param)
{
	struct ad9088_phy *phy;
	int ret;

	ret = ad9088_parse_struct(&phy, init_param);

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

	ret = adi_apollo_device_hw_open(&phy->ad9088, 
					phy->reset_gpio ? ADI_APOLLO_HARD_RESET_AND_INIT :
					ADI_APOLLO_SOFT_RESET_AND_INIT);
	if (ret < 0) {
		pr_err("reset/init failed (%d)\n", ret);
		return -ENODEV;
	}

	ret = adi_apollo_hal_active_protocol_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_SPI0);
	if (ret < 0) {
		pr_err("SPI active protocol set failed (%d)\n", ret);
		return -ENODEV;
	}

	ret = adi_apollo_hal_rmw_enable_set(&phy->ad9088, ADI_APOLLO_HAL_PROTOCOL_SPI0, 0);
	if (ret < 0) {
		pr_err("SPI rmw enable failed (%d)\n", ret);
		return -ENODEV;
	}

	ret = ad9088_reg_test(&phy->ad9088);
	if (ret < 0) {
		pr_err("Register test failed (%d)\n", ret);
		return -ENODEV;
	}

	*device = phy;

	return 0;
}
