  /*!
 * \brief     API implementation ARM CPU
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_ARM
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_arm.h"
#include "adi_apollo_config.h"

#include "adi_apollo_bf_custom.h"
#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_mb_regs.h"

#include "adi_apollo_hal.h"

#define SIM_BF_ALT18_FW_STATUS                              REG_ALT_BOOT_ADDR(18), 0x00000800
#define SIM_BF_RAM_BOOT_STATUS                              REG_ALT_BOOT_ADDR(21), 0x00000800
#define SIM_BF_RAM_BOOT_ERROR                               REG_ALT_BOOT_ADDR(22), 0x00000800

static int32_t adi_apollo_arm_fw_load_check(adi_apollo_device_t *device, uint8_t *status);
static int32_t adi_apollo_arm_profile_load_check(adi_apollo_device_t *device, uint8_t *status);
static int32_t adi_apollo_arm_boot_status_clear(adi_apollo_device_t *device);
static int32_t adi_apollo_arm_core_reset(adi_apollo_device_t *device, uint8_t core);
static int32_t adi_apollo_arm_ram_lock_check(adi_apollo_device_t *device);
static int32_t adi_apollo_arm_stack_ptr_boot_addr_set(adi_apollo_device_t *device, uint8_t core, adi_apollo_arm_binary_info_t *binary_info);
static int32_t get_cpu_device_profile_start_address(adi_apollo_device_t *device, uint32_t *cpu_device_profile_addr);
static int32_t adi_apollo_arm_tye_bootstage_check(adi_apollo_device_t *device, uint8_t stage, uint8_t is_encrypted);
static int32_t device_feat_lockout_set(adi_apollo_device_t *device);

/*==================== P U B L I C   A P I   C O D E ====================*/

int32_t adi_apollo_arm_boot_status(adi_apollo_device_t *device, uint8_t *status)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    // Core1 Boot Status
    err = adi_apollo_hal_bf_get(device, BF_RAM_BOOT_CORE1_STATUS, status, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_APOLLO_LOG_MSG_VAR("Core 1 Ram Boot Status @ 0x%X: 0x%02X.", REG_ADDR_EXTRACT(BF_RAM_BOOT_CORE1_STATUS), *status);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_ram_boot_error_check(adi_apollo_device_t *device)
{
    int32_t err;
    uint32_t status = 0x00;
    adi_apollo_cpu_errors_t cpu_errors;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_arm_err_codes_get(device, &cpu_errors);
    ADI_APOLLO_ERROR_RETURN(err);
    /* boot errors are stored under the system category */
    status = cpu_errors.system;
    ADI_APOLLO_LOG_MSG_VAR("REG_RAM_BOOT_ERROR 0x%X", status);

    /* use only 16-bits of status */
    status = status & 0xFFFF;
    ADI_APOLLO_ERROR_RETURN(status);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_rom_ec_transfer_check(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t status;
    uint32_t retry_count = 50;
    uint32_t poll_delay_us = 100000;    // 100ms
    uint32_t i;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_hal_regio_t *desc = (adi_apollo_hal_regio_t *) device->hal_info.active_regio;

    for (i = 0; i < retry_count; i++) {

        err = adi_apollo_hal_reg_poll_get(device, REG_ADDR_EXTRACT(BF_BOOT_STATUS), &status, retry_count * poll_delay_us, ADI_APOLLO_ROM_BOOT_STEP_EC_TRANSFER, 0xff);
        ADI_APOLLO_ERROR_RETURN(err);

        if (status >= ADI_APOLLO_ROM_BOOT_STEP_EC_TRANSFER || !desc->poll_read_returns_val) {
            err = API_CMS_ERROR_OK;
            break;
        }

        // Delay before next poll read
        err = adi_apollo_hal_delay_us(device, poll_delay_us);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (status < ADI_APOLLO_ROM_BOOT_STEP_EC_TRANSFER) {
        err = API_CMS_ERROR_ERROR;
    }

    ADI_APOLLO_LOG_MSG_VAR("EC Transfer Flag : %d.", status);
    ADI_APOLLO_LOG_MSG_VAR("EC Transfer Check : %s.", (status >= ADI_APOLLO_ROM_BOOT_STEP_EC_TRANSFER) ? "Passed" : "*** FAILED ***");

    return err;
}

int32_t adi_apollo_arm_profile_crc_valid_get(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t val;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_get(device, BF_DEVICE_PROFILE_CRC_CHECK_STATUS, (uint8_t *)&val, sizeof(val));
    ADI_APOLLO_ERROR_RETURN(err);

    return val ? API_CMS_ERROR_PROFILE_CRC : API_CMS_ERROR_OK;

}

int32_t adi_apollo_arm_fwload_pre_config(adi_apollo_device_t *device)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    /* Check for RAM Lock */
    err = adi_apollo_arm_ram_lock_check(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Stop ARM core 0 */
    err = adi_apollo_hal_bf_set(device, BF_ARM0_M3_RUN_INFO, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Stop ARM core 1 */
    err = adi_apollo_hal_bf_set(device, BF_ARM1_M3_RUN_INFO, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Clear boot status */
    err = adi_apollo_arm_boot_status_clear(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Memory Bank division control set (Core 0)*/
    err = adi_apollo_hal_bf_set(device, BF_L1MEM0_SHARED_BANK_DIV_INFO, ADI_APOLLO_DIV_IRAM_384K_DRAM_320K);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Memory Bank division control set (Core 1)*/
    err = adi_apollo_hal_bf_set(device, BF_L1MEM1_SHARED_BANK_DIV_INFO, ADI_APOLLO_DIV_IRAM_352K_DRAM_352K);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Make core 1 Primary */
    err = adi_apollo_hal_bf_set(device, BF_CPU_0_PRIMARY, 0);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_bf_set(device, BF_CPU_1_PRIMARY, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_fwload_post_config(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    /* Reset core 0 */
    err = adi_apollo_arm_core_reset(device, ADI_APOLLO_CPU_ID_0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Start core 1 */
    err = adi_apollo_hal_bf_set(device, BF_ARM1_M3_RUN_INFO, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Start core 0 */
    err = adi_apollo_hal_bf_set(device, BF_ARM0_M3_RUN_INFO, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    // Delay 100ms to start the core
    err = adi_apollo_hal_delay_us(device, 100000);

    /* Check the FW boot status is waiting for device profile config */
    err = adi_apollo_arm_fw_load_check(device, &status);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_profile_write(adi_apollo_device_t *device, adi_apollo_top_t *profile, uint16_t chunk_sz_bytes)
{
    int32_t err;
    adi_apollo_arm_binary_info_t binary_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(profile);

    binary_info.binary_ptr = (uint8_t *)profile;
    binary_info.binary_sz_bytes = sizeof(struct adi_apollo_top);
    binary_info.is_cont = 0;
    get_cpu_device_profile_start_address(device, &(binary_info.cpu_start_addr));

    err = adi_apollo_arm_memload(device, &binary_info, chunk_sz_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    device->dev_info.is_dual_clk = profile->clk_cfg.single_dual_clk_sel;
    device->dev_info.dev_freq_hz = (uint64_t)profile->clk_cfg.dev_clk_freq_kHz * 1000;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_profile_write_post_config(adi_apollo_device_t *device, adi_apollo_top_t *profile)
{
    int32_t err;
    uint8_t status;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(profile);

    /* set transfer done flag */
    err = adi_apollo_hal_bf_set(device, BF_CONFIG_TRANSFER_DONE, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    /* read boot status */
    err = adi_apollo_arm_profile_load_check(device, &status);
    ADI_APOLLO_ERROR_RETURN(err);

    /* check CRC */
    if (err = adi_apollo_arm_profile_crc_valid_get(device), err == API_CMS_ERROR_PROFILE_CRC) {
        ADI_APOLLO_LOG_WARN("profile crc is invalid.");
    }

    /* Store device feature enable configurations */
    device_feat_lockout_set(device);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_profile_buf_write(adi_apollo_device_t *device, uint8_t profile_buf[], uint32_t profile_size_bytes, uint16_t chunk_sz_bytes)
{
    int32_t err;
    adi_apollo_arm_binary_info_t binary_info;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(profile_buf);
    ADI_APOLLO_INVALID_PARAM_RETURN(profile_size_bytes != sizeof(adi_apollo_top_t));

    binary_info.binary_ptr = (uint8_t *)profile_buf;
    binary_info.binary_sz_bytes = profile_size_bytes;
    binary_info.is_cont = 0;
    get_cpu_device_profile_start_address(device, &(binary_info.cpu_start_addr));

    err = adi_apollo_arm_memload(device, &binary_info, chunk_sz_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_firmware_write(adi_apollo_device_t *device, uint8_t fw_image_buf[], uint32_t fw_image_size_bytes, uint16_t chunk_sz_bytes)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fw_image_buf);

    err = adi_apollo_arm_firmware_buf_write(device, ADI_APOLLO_CPU_ID_1, fw_image_buf, fw_image_size_bytes, chunk_sz_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_firmware_buf_write(adi_apollo_device_t *device, uint8_t core, uint8_t fw_image_buf[], uint32_t fw_image_size_bytes, uint16_t chunk_sz_bytes)
{
    adi_apollo_arm_binary_info_t binary_info;
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fw_image_buf);
    ADI_APOLLO_INVALID_PARAM_RETURN(core > ADI_APOLLO_CPU_ID_1);

    binary_info.binary_ptr = &fw_image_buf[0];
    binary_info.binary_sz_bytes = fw_image_size_bytes;
    binary_info.cpu_start_addr = (core == ADI_APOLLO_CPU_ID_0) ? ADI_APOLLO_CPU0_PM_START : ADI_APOLLO_CPU1_PM_START;
    binary_info.is_cont = 0;

    /* Set Core's Stack ptr and Boot Addr */
    err = adi_apollo_arm_stack_ptr_boot_addr_set(device, core, &binary_info);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_arm_memload(device, &binary_info, chunk_sz_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_tye_firmware_buf_write(adi_apollo_device_t *device, uint32_t mem_addr_to_load, uint8_t fw_image_buf[], uint32_t fw_image_size_bytes, uint16_t chunk_sz_bytes)
{
    adi_apollo_arm_binary_info_t binary_info;
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fw_image_buf);

    binary_info.binary_ptr = &fw_image_buf[0];
    binary_info.binary_sz_bytes = fw_image_size_bytes;
    binary_info.cpu_start_addr = mem_addr_to_load;
    binary_info.is_cont = 0;

    err = adi_apollo_arm_memload(device, &binary_info, chunk_sz_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_memload(adi_apollo_device_t *device, adi_apollo_arm_binary_info_t *binary_info, uint16_t chunk_sz_bytes)
{
    int32_t err;
    uint32_t cpu_address_to_load;
    int32_t pending_bytes;
    uint8_t *data_ptr;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(binary_info);
    ADI_APOLLO_NULL_POINTER_RETURN(binary_info->binary_ptr);
    ADI_APOLLO_INVALID_PARAM_RETURN(binary_info->binary_sz_bytes < 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(chunk_sz_bytes < 1 || chunk_sz_bytes > (16 * 1024)); // A0 has max stream limit of 16k
    ADI_APOLLO_INVALID_PARAM_RETURN(chunk_sz_bytes % 4);

    pending_bytes = binary_info->binary_sz_bytes;

    /* chunk size should not be bigger then total size and should always be % 4 */
    if (chunk_sz_bytes > binary_info->binary_sz_bytes) {
        chunk_sz_bytes = binary_info->binary_sz_bytes - (binary_info->binary_sz_bytes % 4);
    }

    cpu_address_to_load = binary_info->cpu_start_addr;

    data_ptr = binary_info->binary_ptr;

    ADI_APOLLO_LOG_MSG_VAR("Loading %d bytes", pending_bytes);

    /* Send data in chunk_sz_bytes */
    while (pending_bytes > 0 && pending_bytes / chunk_sz_bytes > 0) {
        err = adi_apollo_hal_stream_reg32_set(device, cpu_address_to_load, (uint32_t *)data_ptr, chunk_sz_bytes / 4, binary_info->is_cont);
        ADI_APOLLO_ERROR_RETURN(err);

        pending_bytes -= chunk_sz_bytes;
        binary_info->is_cont = 1;
        data_ptr += chunk_sz_bytes;
        cpu_address_to_load += chunk_sz_bytes;
    }

    /* send remaining bytes */
    if (pending_bytes > 0) {
        /* have to reset the FIFO and reconfigure the SPI bus to 8-bit txn */
        binary_info->is_cont = 0;
        err = adi_apollo_hal_stream_reg_set(device, cpu_address_to_load, data_ptr, pending_bytes, binary_info->is_cont);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_tye_bypassed_get(adi_apollo_device_t *device, uint8_t *is_bypassed)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(is_bypassed);

    err = adi_apollo_hal_bf_get(device, BF_NVMB_TE_BYPASS_INFO, is_bypassed, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_tye_boot_ready_set(adi_apollo_device_t *device)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_TE_BOOT_READY_SET, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_tye_validate_boot_completion(adi_apollo_device_t *device, uint8_t is_encrypted)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(is_encrypted > 1);

    err = adi_apollo_arm_tye_bootstage_check(device, 0, is_encrypted);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_arm_tye_bootstage_check(device, 1, is_encrypted);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_arm_err_codes_get(adi_apollo_device_t *device, adi_apollo_cpu_errors_t *cpu_errs)
{
    int32_t err = API_CMS_ERROR_OK;
    uint32_t err_ptr = 0x00;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(cpu_errs);

    /* Get location of error struct */
    err = adi_apollo_hal_reg32_get(device, REG_RAM_BOOT_ERROR_PTR, &err_ptr);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg32_get(device, err_ptr + 0, (uint32_t *) &(cpu_errs->last_cmd));
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_reg32_get(device, err_ptr + 4, (uint32_t*) &(cpu_errs->system));
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_hal_reg32_get(device, err_ptr + 8, (uint32_t*) &(cpu_errs->track_cal));
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_arm_fw_irq_out_get(adi_apollo_device_t *device, uint8_t *fw_irq_out, uint32_t *irq_source)
{
    int32_t err = API_CMS_ERROR_OK;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(fw_irq_out);
    ADI_APOLLO_NULL_POINTER_RETURN(irq_source);

    err = adi_apollo_hal_bf_get(device, BF_FW_IRQ_OUT_INFO, fw_irq_out, sizeof(*fw_irq_out));
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_get(device, BF_FW_IRQ_OUT_SOURCE_INFO, (uint8_t*)irq_source, sizeof(*irq_source));
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

int32_t adi_apollo_arm_fw_irq_out_clear(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_OK;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_FW_IRQ_OUT_INFO, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_bf_set(device, BF_FW_IRQ_OUT_SOURCE_INFO, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

/* Wait for firmware boot status to be wait_for_config */
static int32_t adi_apollo_arm_fw_load_check(adi_apollo_device_t *device, uint8_t *status)
{
    int32_t err;
    uint8_t retry_count = ADI_APOLLO_ARM_BOOT_STATUS_READ_RETRY_COUNT;
    uint32_t poll_delay_us = 100000;    // 100ms

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_hal_regio_t *desc = (adi_apollo_hal_regio_t *) device->hal_info.active_regio;

    /* Read boot status - wait for state ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG */
    do {

        err = adi_apollo_hal_reg_poll_get(device, REG_ADDR_EXTRACT(BF_RAM_BOOT_CORE1_STATUS), status,
                ADI_APOLLO_ARM_BOOT_STATUS_READ_RETRY_COUNT* poll_delay_us,
                ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG, 0xff);
        ADI_APOLLO_ERROR_RETURN(err);

        ADI_APOLLO_LOG_MSG_VAR("BF_RAM_BOOT_CORE1_STATUS: %d.", *status);

        if (*status >= ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG || !desc->poll_read_returns_val) {
            return API_CMS_ERROR_OK;
        }

        // Delay 'poll_delay_us' to wait before reading status again
        err = adi_apollo_hal_delay_us(device, poll_delay_us);
        ADI_APOLLO_ERROR_RETURN(err);

        retry_count--;
    } while (retry_count);

    return API_CMS_ERROR_ERROR;
}

/* Wait for firmware boot status to be >= profile crc check passed */
static int32_t adi_apollo_arm_profile_load_check(adi_apollo_device_t *device, uint8_t *status)
{
    int32_t err;
    uint8_t retry_count = ADI_APOLLO_ARM_BOOT_STATUS_READ_RETRY_COUNT;
    uint32_t poll_delay_us = 100000;    // 100ms

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_hal_regio_t *desc = (adi_apollo_hal_regio_t *) device->hal_info.active_regio;

    /* Read boot status - wait for state ADI_APOLLO_RAM_BOOT_STEP_DEVICE_CONFIG_CRC_CHECK */
    do {
        err = adi_apollo_hal_reg_poll_get(device, REG_ADDR_EXTRACT(BF_RAM_BOOT_CORE1_STATUS), status,
            ADI_APOLLO_ARM_BOOT_STATUS_READ_RETRY_COUNT * poll_delay_us,
            ADI_APOLLO_RAM_BOOT_STEP_DEVICE_CONFIG_CRC_CHECK, 0xff);
        ADI_APOLLO_ERROR_RETURN(err);

        if (*status >= ADI_APOLLO_RAM_BOOT_STEP_DEVICE_CONFIG_CRC_CHECK || !desc->poll_read_returns_val) {
            return API_CMS_ERROR_OK;
        }

        // Delay 'poll_delay_us' to wait before reading status again
        err = adi_apollo_hal_delay_us(device, poll_delay_us);
        ADI_APOLLO_ERROR_RETURN(err);

        retry_count--;
    } while (retry_count);

    return API_CMS_ERROR_ERROR;
}

static int32_t adi_apollo_arm_tye_bootstage_check(adi_apollo_device_t *device, uint8_t stage, uint8_t is_encrypted)
{
    int32_t err;
    uint8_t status;
    uint8_t error;
    uint8_t expected_value;
    uint32_t bootstage_reg_addr;
    uint32_t error_reg_addr;
    uint32_t error_bf_info;
    uint8_t retry_count = ADI_APOLLO_ARM_BOOT_STATUS_READ_RETRY_COUNT;
    uint32_t poll_delay_us = 100000;    // 100ms
    uint8_t i;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(is_encrypted > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(stage > 1);

    expected_value = (is_encrypted == 0) ? ADI_SECURE_BOOT_STAGE_CHECK : ADI_ENCR_SECURE_BOOT_STAGE_CHECK;
    bootstage_reg_addr = (stage == 0) ? REG_SECURE_BOOT_STAGE0 : REG_SECURE_BOOT_STAGE1;
    error_reg_addr = (stage == 0) ? REG_ADDR_EXTRACT(BF_ERROR_CODE_0_INFO) : REG_ADDR_EXTRACT(BF_ERROR_CODE_1_INFO);
    error_bf_info = (stage == 0) ? BF_INFO_EXTRACT(BF_ERROR_CODE_0_INFO) : BF_INFO_EXTRACT(BF_ERROR_CODE_1_INFO);

    adi_apollo_hal_regio_t *desc = (adi_apollo_hal_regio_t *) device->hal_info.active_regio;

    /* Read secure_boot_stage_x status - wait for state 'expected_value' */
    for (i = 0; i < retry_count; i++) {

        err = adi_apollo_hal_reg_poll_get(device, bootstage_reg_addr, &status,
                                        retry_count * poll_delay_us, expected_value, 0xff);
        ADI_APOLLO_ERROR_RETURN(err);

        if (status == expected_value || !desc->poll_read_returns_val) {
            err = API_CMS_ERROR_OK;
            break;
        }

        // Wait 'poll_delay_us' before next poll read
        err = adi_apollo_hal_delay_us(device, poll_delay_us);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    err = adi_apollo_hal_bf_get(device, error_reg_addr, error_bf_info, &error, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    ADI_APOLLO_LOG_MSG_VAR( "SECURE_BOOT_STAGE_%d CHECK: %s.", stage, (status == expected_value) ? "Passed" : "*** FAILED ***");

    if (status != expected_value) {
        err = API_CMS_ERROR_ERROR;
    }

    return err;
}

static int32_t adi_apollo_arm_boot_status_clear(adi_apollo_device_t *device)
{
    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_bf_set(device, BF_RAM_BOOT_CORE1_STATUS, 0);
    ADI_APOLLO_ERROR_RETURN(err)

    err = adi_apollo_hal_bf_set(device, BF_RAM_BOOT_CORE0_STATUS, 0);
    ADI_APOLLO_ERROR_RETURN(err)

    return API_CMS_ERROR_OK;
}

static int32_t adi_apollo_arm_core_reset(adi_apollo_device_t* device, uint8_t core)
{
    int32_t err;
    uint32_t reg_addr = 0;
    adi_apollo_hal_protocol_e protocol;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(core > ADI_APOLLO_CPU_ID_1);

    reg_addr = (core == ADI_APOLLO_CPU_ID_0) ? REG_ARM0_RESET_ADDR : REG_ARM1_RESET_ADDR;

    /*
    Resetting ARM core w/ HSCI resets the uP_subsystem, which resets the HSCI configuration itself
    resulting in further HSCI transactions to not go through hence switching to SPI during the reset
    */
    adi_apollo_hal_active_protocol_get(device, &protocol);
    adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_SPI0);

    err = adi_apollo_hal_reg_set(device, reg_addr, 1);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_delay_us(device, 100000);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_hal_reg_set(device, reg_addr, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Restore active protocol */
    adi_apollo_hal_active_protocol_set(device, protocol);

    return API_CMS_ERROR_OK;
}

static int32_t adi_apollo_arm_ram_lock_check(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t ram_lock;
    uint8_t retry_count = ADI_APOLLO_ARM_EC_TRANSFER_STATUS_READ_RETRY_COUNT;
    uint32_t poll_delay_us = 10000;    // 10ms

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_hal_regio_t *desc = (adi_apollo_hal_regio_t *) device->hal_info.active_regio;

    /* EC RAM lock status - wait for FW to release the EC ram lock */
    do {
        err = adi_apollo_hal_reg_poll_get(device, REG_ADDR_EXTRACT(BF_EC_RAM_LOCK_INFO), &ram_lock,
            ADI_APOLLO_ARM_EC_TRANSFER_STATUS_READ_RETRY_COUNT * poll_delay_us,
            0x00, BF_EC_RAM_LOCK_MASK);
        ADI_APOLLO_ERROR_RETURN(err);

        if (!(ram_lock & BF_EC_RAM_LOCK_MASK) || !desc->poll_read_returns_val) {
            return API_CMS_ERROR_OK;
        }

        // Delay 'poll_delay_us' to wait before reading status again
        err = adi_apollo_hal_delay_us(device, poll_delay_us);
        ADI_APOLLO_ERROR_RETURN(err);

        retry_count--;
    } while (retry_count);

    return API_CMS_ERROR_EC_RAM_LOCK_ERROR;
}


static int32_t adi_apollo_arm_stack_ptr_boot_addr_set(adi_apollo_device_t *device, uint8_t core, adi_apollo_arm_binary_info_t *binary_info)
{
    int32_t err;

    uint8_t *fw_image_buf = binary_info->binary_ptr;
    uint32_t reg_addr = 0;
    uint32_t bf_info = 0;
    uint32_t reg_val = 0;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(binary_info);
    ADI_APOLLO_NULL_POINTER_RETURN(binary_info->binary_ptr);
    ADI_APOLLO_INVALID_PARAM_RETURN(binary_info->binary_sz_bytes < 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(core > ADI_APOLLO_CPU_ID_1);

    /* Extract Stack pointer and update */
    reg_val = ( (fw_image_buf[3] << 24) | (fw_image_buf[2] << 16) | (fw_image_buf[1] << 8) | fw_image_buf[0] );

    reg_addr = (core == ADI_APOLLO_CPU_ID_0) ? REG_ADDR_EXTRACT(BF_ARM0_STACK_PTR_INFO) : REG_ADDR_EXTRACT(BF_ARM1_STACK_PTR_INFO);
    bf_info = (core == ADI_APOLLO_CPU_ID_0) ? BF_INFO_EXTRACT(BF_ARM0_STACK_PTR_INFO) : BF_INFO_EXTRACT(BF_ARM1_STACK_PTR_INFO);

    err = adi_apollo_hal_bf_set(device, reg_addr, bf_info, reg_val);
    ADI_APOLLO_ERROR_RETURN(err);

    ADI_APOLLO_LOG_MSG_VAR("Core%d Stack Pointer: \treg_addr: 0x%08X.\treg_val: 0x%08X.", core, reg_addr, reg_val);


    /* Extract Boot address and update */
    reg_val = ( (fw_image_buf[7] << 24) | (fw_image_buf[6] << 16) | (fw_image_buf[5] << 8) | fw_image_buf[4] );

    reg_addr = (core == ADI_APOLLO_CPU_ID_0) ? REG_ADDR_EXTRACT(BF_ARM0_BOOT_ADDR_INFO) : REG_ADDR_EXTRACT(BF_ARM1_BOOT_ADDR_INFO);
    bf_info = (core == ADI_APOLLO_CPU_ID_0) ? BF_INFO_EXTRACT(BF_ARM0_BOOT_ADDR_INFO) : BF_INFO_EXTRACT(BF_ARM1_BOOT_ADDR_INFO);

    err = adi_apollo_hal_bf_set(device, reg_addr, bf_info, reg_val);
    ADI_APOLLO_ERROR_RETURN(err);

    ADI_APOLLO_LOG_MSG_VAR("Core%d Boot Addr: \treg_addr: 0x%08X.\treg_val: 0x%08X.", core, reg_addr, reg_val);

    return API_CMS_ERROR_OK;
}

static int32_t get_cpu_device_profile_start_address(adi_apollo_device_t *device, uint32_t *cpu_device_profile_addr)
{
    uint32_t address = ADI_APOLLO_FW_DEVICE_PROFILE_PTR;
    int32_t err;

    err = adi_apollo_hal_reg32_get(device, address, cpu_device_profile_addr);
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_APOLLO_LOG_MSG_VAR("Device Profile address: 0x%x\n", *cpu_device_profile_addr);

    return API_CMS_ERROR_OK;
}

static int32_t device_feat_lockout_set(adi_apollo_device_t *device)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);

    err = adi_apollo_hal_bf_get(device, BF_EC_RX_INFO, (uint8_t *)&device->dev_info.lockout_mask.rx_lockout_mask, sizeof(device->dev_info.lockout_mask.rx_lockout_mask));
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Device feat lockout mask (rx): 0x%08x", device->dev_info.lockout_mask.rx_lockout_mask);

    err = adi_apollo_hal_bf_get(device, BF_EC_TX_INFO, (uint8_t *)&device->dev_info.lockout_mask.tx_lockout_mask, sizeof(device->dev_info.lockout_mask.tx_lockout_mask));
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Device feat lockout mask (tx): 0x%08x", device->dev_info.lockout_mask.tx_lockout_mask);

    return API_CMS_ERROR_OK;
}

/*! @} */