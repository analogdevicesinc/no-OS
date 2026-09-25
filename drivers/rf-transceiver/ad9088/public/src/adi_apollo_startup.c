/*!
 * \brief     Startup sequence APIs
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_STARTUP
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo.h"
#include "adi_utils.h"
#include "adi_apollo_startup_types.h"
#include "adi_apollo_bf_custom.h"

#define SECR_BOOT_HDR_MEM_ADDR (0x01030000U)    // CODE_MEMORY_A_6 APOLLO_CPU0_IRAM_START
#define CORE_0_TYE_FW_MEM_ADDR (0x20000000U)    // SYS_MEMORY_A_0  APOLLO_CPU0_DRAM_START
#define CORE_1_TYE_FW_MEM_ADDR (0x02000000U)    // CODE_MEMORY_B_0 APOLLO_CPU1_IRAM_START
#define TYE_OPER_FW_MEM_ADDR   (0x21000000U)    // SYS_MEMORY_B_0  APOLLO_CPU1_DRAM_START
#define TYE_USER_SCENARIO (4)

#define FW_TRANSFER_CHUNK_SIZE (16 * 1024)      // FW write chunk block size
#define PREFIX "adi_apollo_startup_execute: "

/*============= C O D E ====================*/
static int32_t startup_seq_execute(adi_apollo_device_t *device, adi_apollo_top_t *dev_profile,
                                   adi_apollo_fw_provider_t *fw_provider, adi_apollo_startup_seq_type_e startup_type);
static int32_t fw_load(adi_apollo_device_t *device);
static int32_t tye_fw_load(adi_apollo_device_t *device, uint8_t scenario);
static int32_t profile_load(adi_apollo_device_t *device, adi_apollo_top_t *profile);
static int32_t activate_rx_tx_blocks(adi_apollo_device_t *device);

static int32_t seq_fw_load(adi_apollo_device_t *device, adi_apollo_fw_provider_t *fw_provider);
static int32_t seq_profile_load(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile);
static int32_t seq_tmu_enable(adi_apollo_device_t *device);
static int32_t seq_dp_config(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile);
static int32_t seq_link_en(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile);
static int32_t seq_txrx_activate(adi_apollo_device_t *device);
static int32_t seq_sync(adi_apollo_device_t *device);

int32_t adi_apollo_startup_execute(adi_apollo_device_t *device, adi_apollo_top_t *dev_profile, uint32_t startup_type)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(dev_profile);
    ADI_APOLLO_NULL_POINTER_RETURN(device->startup_info.fw_provider);
    ADI_APOLLO_NULL_POINTER_RETURN(device->startup_info.fw_provider->desc);
    ADI_APOLLO_NULL_POINTER_RETURN(device->startup_info.get);
    ADI_APOLLO_INVALID_PARAM_RETURN((startup_type & ADI_APOLLO_STARTUP_SEQ_DEFAULT) == 0);

    ADI_APOLLO_LOG_MSG_VAR(PREFIX"startup_type 0x%08x", startup_type);
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"profile checksum %lu", dev_profile->profile_checksum);
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"fw_provider desc '%s'", device->startup_info.fw_provider->desc);

    /* Execute the startup sequence */
    err = startup_seq_execute(device, dev_profile, device->startup_info.fw_provider, startup_type);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

static int32_t startup_seq_execute(adi_apollo_device_t *device, adi_apollo_top_t *dev_profile,
                                   adi_apollo_fw_provider_t *fw_provider, adi_apollo_startup_seq_type_e startup_type)
{

    int32_t err = API_CMS_ERROR_OK;

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_FW_LOAD) {
        err = seq_fw_load(device, fw_provider);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_fw_load: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_PROFILE_LOAD) {
        err = seq_profile_load(device, dev_profile);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_profile_load: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_TMU_EN) {
        err = seq_tmu_enable(device);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_tmu_enable: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_DP_CONFIG) {
        err = seq_dp_config(device, dev_profile);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_dp_config: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_LINK_EN) {
        err = seq_link_en(device, dev_profile);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_link_en: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_TXRX_ACTIVATE) {
        err = seq_txrx_activate(device);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_txrx_activate: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (startup_type & ADI_APOLLO_STARTUP_SEQ_SYNC) {
        err = seq_sync(device);
        ADI_APOLLO_LOG_MSG_VAR(PREFIX"seq_sync: %s", (err == API_CMS_ERROR_OK) ? "OK" : "Error");
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return err;
}

static int32_t seq_fw_load(adi_apollo_device_t *device, adi_apollo_fw_provider_t *fw_provider)
{

    int32_t err = API_CMS_ERROR_OK;
    uint8_t tye_bypassed = 0;

    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_FW_LOAD");

    err = adi_apollo_arm_tye_bypassed_get(device, &tye_bypassed);
    ADI_APOLLO_ERROR_RETURN(err);
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"TyE bypassed: %d", tye_bypassed);

    /* Load FW files */
    if (tye_bypassed) {
        /* Load the standard device firmware for core0 and core1.*/
        err = fw_load(device);
        ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"fw_load", "");
    } else {
        /* Load the TyE app pack firmware.*/
        err = tye_fw_load(device, TYE_USER_SCENARIO);
        ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"tye_fw_load %d", err);
    }

    return err;
}

static int32_t seq_profile_load(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile)
{
    int32_t err = API_CMS_ERROR_OK;
    uint32_t num_tries = 10;
    uint8_t boot_status = 0;
    uint8_t prev_boot_status = 0;
    int32_t fw_err_code = 0;

    ADI_APOLLO_LOG_MSG("ADI_APOLLO_STARTUP_SEQ_PROFILE_LOAD");

    /* Wait for FW ready to accept profile */
    num_tries = 100;
    do {
        err = adi_apollo_hal_delay_us(device, 50000);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_arm_boot_status(device, &boot_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if (boot_status != prev_boot_status) {
            ADI_APOLLO_LOG_MSG_VAR("Core 1 Ram Boot Status: 0x%02X", boot_status);
            prev_boot_status = boot_status;
        }

    } while (boot_status != ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG && num_tries-- > 0);

    if (boot_status != ADI_APOLLO_RAM_BOOT_STEP_WAIT_FOR_CONFIG) {
        err = API_CMS_ERROR_STARTUP_FW_RDY_FOR_PROFILE_ERROR;
        ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"FW didn't reach wait for device profile state. boot_status: 0x%x.", boot_status);
    }

    /*
     * Load the device profile use case into the firmware (arm)
     *
     * Firmware configures:
     *   - Clocks and PLLs
     *   - JESD JRx and JTx
     *
     * The API will configure other functional blocks using the same device profile
     */
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"Loading device profile (ver=%d.%d.%d, %s)...",
                           dev_profile->profile_cfg.profile_version.major,
                           dev_profile->profile_cfg.profile_version.minor,
                           dev_profile->profile_cfg.profile_version.patch,
                           dev_profile->profile_cfg.is_8t8r ? "8T8R" : "4T4R");
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"Profile checksum: %u", dev_profile->profile_checksum);
    err = profile_load(device, dev_profile);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"Profile write to device failed. err: %d", err);

    /* Verify FW mailbox is ready */
    num_tries = 600;
    do {
        err = adi_apollo_hal_delay_us(device, 1 * 50000);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_arm_boot_status(device, &boot_status);
        ADI_APOLLO_ERROR_RETURN(err);

        if (boot_status != prev_boot_status) {
            ADI_APOLLO_LOG_MSG_VAR("Core 1 Ram Boot Status: 0x%02X", boot_status);
            prev_boot_status = boot_status;
        }

    } while (boot_status != ADI_APOLLO_CPU_BOOT_MAILBOX_READY && num_tries-- > 0);

    if (boot_status != ADI_APOLLO_CPU_BOOT_MAILBOX_READY) {
        // Log the FW error code
        fw_err_code = adi_apollo_arm_ram_boot_error_check(device);
        ADI_APOLLO_LOG_ERR_VAR(PREFIX "FW didn't reach mailbox ready state. fw_err_code: 0x%x", fw_err_code);

        err = API_CMS_ERROR_STARTUP_FW_MAILBOX_RDY_ERROR;
        ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"FW didn't reach mailbox ready state. boot_status: 0x%x", boot_status);
    }


    return err;
}

static int32_t seq_tmu_enable(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_TMU_EN");

    /* Enable TMU */
    err = adi_apollo_device_tmu_enable(device);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"adi_apollo_device_tmu_enable. err: %d", err);

    return err;
}

static int32_t seq_dp_config(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_DP_CONFIG");

    /* Configure the Rx and Tx data paths as specified in the device profile */
    err = adi_apollo_cfg_data_path(device, dev_profile);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"adi_apollo_cfg_data_path failed. err: %d", err);

    return err;
}

static int32_t seq_link_en(adi_apollo_device_t *device,  adi_apollo_top_t *dev_profile)
{
    int32_t err;

    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_LINK_EN");

    /* Enable Apollo JTx and JRx links based on device profile */
    err = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_A0, dev_profile->jtx[0].tx_link_cfg[0].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_A1, dev_profile->jtx[0].tx_link_cfg[1].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_B0, dev_profile->jtx[1].tx_link_cfg[0].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jtx_link_enable_set(device, ADI_APOLLO_LINK_B1, dev_profile->jtx[1].tx_link_cfg[1].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);

    err = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A0, dev_profile->jrx[0].rx_link_cfg[0].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_A1, dev_profile->jrx[0].rx_link_cfg[1].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_B0, dev_profile->jrx[1].rx_link_cfg[0].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_jrx_link_enable_set(device, ADI_APOLLO_LINK_B1, dev_profile->jrx[1].rx_link_cfg[1].link_in_use);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

static int32_t seq_txrx_activate(adi_apollo_device_t *device)
{
    int32_t err;
    
    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_TXRX_ACTIVATE");

    /* Activate the Rx and Tx blocks */
    err = activate_rx_tx_blocks(device);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"activate_rx_tx_blocks failed. err: %d", err);

    return err;
}

static int32_t seq_sync(adi_apollo_device_t *device)
{
    int32_t err;

    ADI_APOLLO_LOG_MSG(PREFIX"ADI_APOLLO_STARTUP_SEQ_SYNC");

    /* Datapath reset */
    err = adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Dynamic sync sequence - gradual enabling of blocks to mitigate dynamic power */
    err = adi_apollo_clk_mcs_dyn_sync_sequence_run(device);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"adi_apollo_clk_mcs_dyn_sync_sequence_run failed. err: %d\n", err);

    return err;
}

static int32_t fw_load(adi_apollo_device_t *device)
{
    int32_t err;
    uint8_t *core1_fw_buf = NULL;
    uint8_t *core0_fw_buf = NULL;
    uint32_t core1_size_bytes = 0;
    uint32_t core0_size_bytes = 0;
    adi_apollo_fw_provider_t *fw_provider;

    fw_provider = device->startup_info.fw_provider;

    err = adi_apollo_arm_fwload_pre_config(device);
    ADI_CMS_ERROR_RETURN(err);

    ADI_APOLLO_LOG_MSG(PREFIX"Loading firmware for Core1...");
    err = device->startup_info.open ? device->startup_info.open(fw_provider, ADI_APOLLO_FW_ID_CORE_1_STD_FW_BIN) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.get(fw_provider, ADI_APOLLO_FW_ID_CORE_1_STD_FW_BIN, &core1_fw_buf, &core1_size_bytes);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_arm_firmware_buf_write(device, ADI_APOLLO_CPU_ID_1, core1_fw_buf, core1_size_bytes, FW_TRANSFER_CHUNK_SIZE);
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.close ? device->startup_info.close(fw_provider, ADI_APOLLO_FW_ID_CORE_1_STD_FW_BIN) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    ADI_APOLLO_LOG_MSG(PREFIX"Loading firmware for Core0...");
    err = device->startup_info.open ? device->startup_info.open(fw_provider, ADI_APOLLO_FW_ID_CORE_0_STD_FW_BIN) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.get(fw_provider, ADI_APOLLO_FW_ID_CORE_0_STD_FW_BIN, &core0_fw_buf, &core0_size_bytes);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_arm_firmware_buf_write(device, ADI_APOLLO_CPU_ID_0, core0_fw_buf, core0_size_bytes, FW_TRANSFER_CHUNK_SIZE);
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.close ? device->startup_info.close(fw_provider, ADI_APOLLO_FW_ID_CORE_0_STD_FW_BIN) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_arm_fwload_post_config(device);
    ADI_CMS_ERROR_RETURN(err);

    return err;
}

static int32_t tye_fw_load(adi_apollo_device_t *device, uint8_t scenario)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t si_grade;

    adi_apollo_startup_fw_id_e secr_hdr_filename;
    adi_apollo_startup_fw_id_e core0_fw_filename;
    adi_apollo_startup_fw_id_e core1_fw_filename;
    adi_apollo_startup_fw_id_e oper_fw_filename;

    uint8_t *secr_hdr_buf = NULL;
    uint8_t *core0_fw_buf = NULL;
    uint8_t *core1_fw_buf = NULL;
    uint8_t *oper_fw_buf = NULL;

    uint32_t secr_hdr_size_bytes = 0;
    uint32_t core0_fw_size_bytes = 0;
    uint32_t core1_fw_size_bytes = 0;
    uint32_t oper_fw_size_bytes = 0;

    adi_apollo_fw_provider_t *fw_provider = device->startup_info.fw_provider;

    ADI_APOLLO_LOG_MSG_VAR(PREFIX"Running scenario = %d", scenario);

    err = adi_apollo_device_si_grade_get(device, &si_grade);
    ADI_CMS_ERROR_RETURN(err);
    ADI_APOLLO_LOG_MSG_VAR(PREFIX"Apollo Silicon Grade = %02d", si_grade);

    secr_hdr_filename = (si_grade == 0) ? ADI_APOLLO_FW_ID_SECR_BOOT_HDR_BIN : ADI_APOLLO_FW_ID_PROD_SECR_BOOT_HDR_BIN;
    core0_fw_filename = (si_grade == 0) ? ADI_APOLLO_FW_ID_CORE_0_TYE_FW_BIN : ADI_APOLLO_FW_ID_PROD_CORE_0_TYE_FW_BIN;
    core1_fw_filename = (si_grade == 0) ? ADI_APOLLO_FW_ID_CORE_1_TYE_FW_BIN : ADI_APOLLO_FW_ID_PROD_CORE_1_TYE_FW_BIN;
    oper_fw_filename = (si_grade == 0) ? ADI_APOLLO_FW_ID_TYE_OPER_FW_BIN : ADI_APOLLO_FW_ID_PROD_TYE_OPER_FW_BIN;

    err = adi_apollo_arm_fwload_pre_config(device);
    ADI_CMS_ERROR_RETURN(err);

    /* Load the firmware header to memory. */
    ADI_APOLLO_LOG_MSG(PREFIX"Loading TyE SecureBoot Header image...");
    err = device->startup_info.open ? device->startup_info.open(fw_provider, secr_hdr_filename) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.get(fw_provider, secr_hdr_filename, &secr_hdr_buf, &secr_hdr_size_bytes);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_arm_tye_firmware_buf_write(device, SECR_BOOT_HDR_MEM_ADDR, secr_hdr_buf, secr_hdr_size_bytes, FW_TRANSFER_CHUNK_SIZE);
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.close ? device->startup_info.close(fw_provider, secr_hdr_filename) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    // Scenarios 2 and 4 app_pack contains the core 0 firmware image.
    if (scenario == 2 || scenario == 4) {
        ADI_APOLLO_LOG_MSG(PREFIX"Loading Core0 firmware image...");
        err = device->startup_info.open ? device->startup_info.open(fw_provider, core0_fw_filename) : API_CMS_ERROR_OK;
        ADI_CMS_ERROR_RETURN(err);

        err = device->startup_info.get(fw_provider, core0_fw_filename, &core0_fw_buf, &core0_fw_size_bytes);
        ADI_CMS_ERROR_RETURN(err);

        err = adi_apollo_arm_tye_firmware_buf_write(device, CORE_0_TYE_FW_MEM_ADDR, core0_fw_buf, core0_fw_size_bytes, FW_TRANSFER_CHUNK_SIZE);
        ADI_CMS_ERROR_RETURN(err);

        err = device->startup_info.close ? device->startup_info.close(fw_provider, core0_fw_filename) : API_CMS_ERROR_OK;
        ADI_CMS_ERROR_RETURN(err);
    }

    // Scenarios 3 and 4 app_pack contains the core 1 firmware image.
    if (scenario == 3 || scenario == 4) {
        ADI_APOLLO_LOG_MSG(PREFIX"Loading Core1 firmware image...");
        err = device->startup_info.open ? device->startup_info.open(fw_provider, core1_fw_filename) : API_CMS_ERROR_OK;
        ADI_CMS_ERROR_RETURN(err);

        err = device->startup_info.get(fw_provider, core1_fw_filename, &core1_fw_buf, &core1_fw_size_bytes);
        ADI_CMS_ERROR_RETURN(err);

        err = adi_apollo_arm_tye_firmware_buf_write(device, CORE_1_TYE_FW_MEM_ADDR, core1_fw_buf, core1_fw_size_bytes, FW_TRANSFER_CHUNK_SIZE);
        ADI_CMS_ERROR_RETURN(err);

        err = device->startup_info.close ? device->startup_info.close(fw_provider, core1_fw_filename) : API_CMS_ERROR_OK;
        ADI_CMS_ERROR_RETURN(err);
    }

    /* Load the operation firmware image to memory */
    ADI_APOLLO_LOG_MSG(PREFIX"Loading TinyE Operation firmware image...");
    err = device->startup_info.open ? device->startup_info.open(fw_provider, oper_fw_filename) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    device->startup_info.get(fw_provider, oper_fw_filename, &oper_fw_buf, &oper_fw_size_bytes);
    ADI_CMS_ERROR_RETURN(err);

    err = adi_apollo_arm_tye_firmware_buf_write(device, TYE_OPER_FW_MEM_ADDR, oper_fw_buf, oper_fw_size_bytes, FW_TRANSFER_CHUNK_SIZE);
    ADI_CMS_ERROR_RETURN(err);

    err = device->startup_info.close ? device->startup_info.close(fw_provider, oper_fw_filename) : API_CMS_ERROR_OK;
    ADI_CMS_ERROR_RETURN(err);

    /* Set boot ready bit */
    err = adi_apollo_arm_tye_boot_ready_set(device);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"Error setting boot ready bit. Err: %d.", err);
    ADI_APOLLO_LOG_MSG(PREFIX"Boot Ready bit has been set");

    /* Delay 0.1s */
    adi_apollo_hal_delay_us(device, 1 * 10000);

    err = adi_apollo_arm_fwload_post_config(device);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"adi_apollo_arm_fwload_post_config error. Err: %d.", err);

    err = adi_apollo_arm_tye_validate_boot_completion(device, 1);
    ADI_APOLLO_LOG_ERROR_RETURN_VAR(err, PREFIX"Error: adi_apollo_arm_tye_validate_boot_completion. Err: %d.", err);

    return err;
}

static int32_t profile_load(adi_apollo_device_t *device, adi_apollo_top_t *profile)
{
    int32_t err;
    uint8_t boot_status;
    uint32_t device_profile_start_addr;

    err = adi_apollo_hal_bf_set(device, BF_CONFIG_TRANSFER_DONE, 0);

    /* Write the profile configuration to ARM memory for firmware processing */
    if (err = adi_apollo_arm_profile_write(device, profile, 16 * 1024), err != API_CMS_ERROR_OK) {
        ADI_APOLLO_LOG_ERR_VAR(PREFIX"adi_apollo_arm_profile_write error: 0x%x", err);
        return err;
    }

    adi_apollo_hal_reg32_get(device, ADI_APOLLO_FW_DEVICE_PROFILE_PTR, &device_profile_start_addr);

    /* Notify firmware that profile load has completed */
    if (err = adi_apollo_arm_profile_write_post_config(device, profile), err != API_CMS_ERROR_OK) {
        adi_apollo_arm_boot_status(device, &boot_status);
        ADI_APOLLO_LOG_ERR_VAR(PREFIX"Profile write completed with error code: %d. boot_status: 0x%x", err, boot_status);
        return err;
    }

    return API_CMS_ERROR_OK;
}

static int32_t activate_rx_tx_blocks(adi_apollo_device_t *device)
{
    int32_t err;

    /* Enable Rx blocks - enable/disable via spi */
    adi_apollo_rxen_pwrup_ctrl_t rxen_config = {
        .sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
        .sm_en = 0, .spi_rxen = 1, .spi_rxen_en = 1
    };

    err = adi_apollo_rxen_pwrup_ctrl_set(device, ADI_APOLLO_RXEN_ADC_ALL, &rxen_config);
    ADI_CMS_ERROR_RETURN(err);

    /* Enable Tx blocks - enable/disable via spi */
    adi_apollo_txen_pwrup_ctrl_t txen_config = {
        .sm_clk_rate = ADI_APOLLO_PUC_CLK_RATE_FS_DIV_32,
        .sm_en = 0, .spi_txen = 1, .spi_txen_en = 1
    };

    err = adi_apollo_txen_pwrup_ctrl_set(device, ADI_APOLLO_TXEN_DAC_ALL, &txen_config);
    ADI_CMS_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

/*! @} */