/*!
 * \brief     General device API implementations
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_DEVICE
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_bf_core.h"
#include "adi_apollo_bf_hsci.h"
#include "adi_apollo_hal_regio_local.h"
#include "adi_apollo_device.h"
#include "adi_apollo_bf_mcs_sync.h"
#include "adi_apollo_bf_rsa.h"
#include "adi_apollo_mailbox.h"
#include "adi_apollo_private_device.h"
#include "adi_apollo_private_blk_sel.h"
#include "adi_apollo_bf_ec.h"
#include "adi_apollo_rxmisc.h"
#include "adi_apollo_txmisc.h"

/*============= D A T A ====================*/
static uint16_t apollo_api_revision[3] = { 0, 5, 0 };

static adi_apollo_mailbox_cmd_set_enabled_temp_sensors_t temp_senors_cmd = {
        .temp_sensor_mask = ADI_APOLLO_DEVTEMP_MASK_SERDESPLL |
            ADI_APOLLO_DEVTEMP_MASK_MPU_A | ADI_APOLLO_DEVTEMP_MASK_MPU_B |
            ADI_APOLLO_DEVTEMP_MASK_LPU_VENUS_A | ADI_APOLLO_DEVTEMP_MASK_LPU_VENUS_B |
            ADI_APOLLO_DEVTEMP_MASK_LPU_CK_CORNER_A | ADI_APOLLO_DEVTEMP_MASK_LPU_CK_CORNER_B | ADI_APOLLO_DEVTEMP_MASK_LPU_CK_CENTER
};

/*============= C O D E ====================*/
static int32_t configure_spi(adi_apollo_device_t *device);
static int32_t device_context_init(adi_apollo_device_t* device);

int32_t adi_apollo_device_spi_register_set(adi_apollo_device_t *device, uint32_t addr, uint8_t data)
{
    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    err = adi_apollo_hal_reg_set(device, addr, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_spi_register_get(adi_apollo_device_t *device, uint32_t addr, uint8_t *data)
{
    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(data);

    err = adi_apollo_hal_reg_get(device, addr, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_api_revision_get(adi_apollo_device_t *device, uint16_t *rev_major, uint16_t *rev_minor, uint16_t *rev_rc)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(rev_major);
    ADI_APOLLO_NULL_POINTER_RETURN(rev_minor);
    ADI_APOLLO_NULL_POINTER_RETURN(rev_rc);

    *rev_major = apollo_api_revision[0];
    *rev_minor = apollo_api_revision[1];
    *rev_rc    = apollo_api_revision[2];

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_reset(adi_apollo_device_t *device, adi_apollo_reset_e reset_opt)
{
    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(reset_opt > ADI_APOLLO_HARD_RESET_AND_INIT);

    /* Datapath reset - mitigates power supply current change */
    err = adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_rxmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_txmisc_dp_reset(device, ADI_APOLLO_SIDE_ALL, 0);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Device reset */
    if ((reset_opt == ADI_APOLLO_SOFT_RESET) || (reset_opt == ADI_APOLLO_SOFT_RESET_AND_INIT)) {
        err = adi_apollo_hal_reg_set(device, REG_SPI_IFACE_CONFIG_A_ADDR, 0x81);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_reg_set(device, REG_SPI_IFACE_CONFIG_A_ADDR, 0x00);
        ADI_APOLLO_ERROR_RETURN(err);
    } else if ((reset_opt == ADI_APOLLO_HARD_RESET) || (reset_opt == ADI_APOLLO_HARD_RESET_AND_INIT)) {
        err = adi_apollo_hal_reset_pin_ctrl(device, 0);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_delay_us(device, 1000);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_reset_pin_ctrl(device, 1);
        ADI_APOLLO_ERROR_RETURN(err);
        err = adi_apollo_hal_delay_us(device, 1000);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* do init */
    if ((reset_opt == ADI_APOLLO_SOFT_RESET_AND_INIT) || (reset_opt == ADI_APOLLO_HARD_RESET_AND_INIT)) {
        err = adi_apollo_device_init(device);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_init(adi_apollo_device_t *device)
{
    int32_t  err;
    uint32_t endian_test_val = 0x11223344;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    /* log API information */
    err = adi_apollo_hal_log_write(device, ADI_CMS_LOG_MSG, "api v%d.%d.%d ",
        apollo_api_revision[0], apollo_api_revision[1], apollo_api_revision[2]);
    ADI_APOLLO_ERROR_RETURN(err);

    /* get host cpu endian mode */
    if (*(uint8_t *)&endian_test_val == 0x44)
        ADI_APOLLO_LOG_MSG("host is using little endian mode.");
    else
        ADI_APOLLO_LOG_MSG("host is using big endian mode.");

    /* program device spi config */
    err = configure_spi(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Configure device hsci */
    if (device->hal_info.hsci_desc.is_used) {
        err = adi_apollo_device_hsci_configure(device, &device->hal_info.hsci_desc.hsci_config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_deinit(adi_apollo_device_t *device)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_chip_id_get(adi_apollo_device_t *device, adi_cms_chip_id_t *chip_id)
{
    int32_t err;
    uint8_t reg_val = 0x0;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(chip_id);

    err = adi_apollo_hal_reg_get(device, REG_CHIP_TYPE_ADDR, &reg_val);
    ADI_APOLLO_ERROR_RETURN(err);
    chip_id->chip_type = reg_val;

    err = adi_apollo_hal_reg_get(device, REG_PRODUCT_ID_0_ADDR, &reg_val);
    ADI_APOLLO_ERROR_RETURN(err);
    chip_id->prod_id = reg_val;

    err = adi_apollo_hal_reg_get(device, REG_PRODUCT_ID_1_ADDR, &reg_val);
    ADI_APOLLO_ERROR_RETURN(err);
    chip_id->prod_id |= (reg_val << 8);

    err = adi_apollo_hal_reg_get(device, REG_CHIP_GRADE_ADDR, &reg_val);
    ADI_APOLLO_ERROR_RETURN(err);
    chip_id->prod_grade   = (reg_val >> 4);
    chip_id->dev_revision = (reg_val & 0x0F);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_laminate_id_get(adi_apollo_device_t *device, uint8_t *id)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(id);

    return adi_apollo_hal_reg_get(device, REG_LAMINATE_ID_ADDR, (uint8_t *)id);
}

int32_t adi_apollo_device_die_id_get(adi_apollo_device_t *device, uint8_t *id)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(id);

    return adi_apollo_hal_reg_get(device, REG_DIE_ID_ADDR, (uint8_t *)id);
}

int32_t adi_apollo_device_si_grade_get(adi_apollo_device_t *device, uint8_t *si_grade)
{
    int32_t err;
    uint8_t reg_val;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(si_grade);

    err = adi_apollo_hal_reg_get(device, REG_EC_ANA_BYTE2_ADDR, &reg_val);
    ADI_APOLLO_ERROR_RETURN(err);

    *si_grade = (reg_val & 0x01);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_hw_open(adi_apollo_device_t *device, adi_apollo_reset_e reset_opt)
{

    int32_t err;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(reset_opt < ADI_APOLLO_NO_RESET)
    ADI_APOLLO_INVALID_PARAM_RETURN(reset_opt > ADI_APOLLO_HARD_RESET_AND_INIT)
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.delay_us);   // Check delay driver function assignment

    err = adi_apollo_hal_hw_open(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Internal regio configuration */
    err = adi_apollo_l_hal_regio_spi_init(&device->hal_info.spi0_desc, 0);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_l_hal_regio_spi_init(&device->hal_info.spi1_desc, 1);
    ADI_APOLLO_ERROR_RETURN(err);
    err = adi_apollo_l_hal_regio_hsci_init(&device->hal_info.hsci_desc);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Configure device spi */
    err = configure_spi(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Do any device context one-time initialization */
    err = device_context_init(device);
    ADI_APOLLO_ERROR_RETURN(err);

    /* Do optional reset */
    if (reset_opt != ADI_APOLLO_NO_RESET) {
        err = adi_apollo_device_reset(device, reset_opt);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Store the block selection mask for device type (is_8t8r mask be established) */
    adi_apollo_private_blk_sel_mask_set(device);

    /* Configure device hsci */
    if (device->hal_info.hsci_desc.is_used) {
        err = adi_apollo_device_hsci_configure(device, &device->hal_info.hsci_desc.hsci_config);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_hw_close(adi_apollo_device_t *device)
{
    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_spi_configure(adi_apollo_device_t *device, adi_apollo_device_spi_settings_t *spi_config, uint8_t spi_num)
{
    int32_t err = API_CMS_ERROR_OK;
    uint8_t reg_val = 0x00;
    void *active_regio_save;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(spi_config);

    /* To init spi0 or spi1, spi0 must be active */
    if (!device->hal_info.spi0_desc.is_used) {
        return API_CMS_ERROR_ERROR;
    }

    /* Assemble spi init register val */
    reg_val |= (spi_config->sdo == ADI_APOLLO_DEVICE_SPI_SDIO) ? 0x00 : 0x18;                   /* 3-wire : 4-wire */
    reg_val |= (spi_config->addr_inc == ADI_APOLLO_DEVICE_SPI_ADDR_DEC_AUTO) ? 0x00 : 0x24;     /* dec : incr */
    reg_val |= (spi_config->msb == ADI_APOLLO_DEVICE_SPI_MSB_FIRST) ? 0x00 : 0x42;              /* MSB first : LSB first */

    /* Set active regio to spi0, save current active one (e.g. hsci) */
    active_regio_save = device->hal_info.active_regio;
    device->hal_info.active_regio = &device->hal_info.spi0_desc.base_regio;

    if (spi_num == 1) {
        if (device->hal_info.spi1_desc.is_used) {
            err = adi_apollo_hal_reg_set(device, REG_SPI1_ENABLE_ADDR, 0x01);       /* Enable spi1 (using spi0) */
            device->hal_info.active_regio = &device->hal_info.spi1_desc.base_regio;
        }
        else {
            err = API_CMS_ERROR_ERROR;
        }
    }

    if (err == API_CMS_ERROR_OK) {
        err = adi_apollo_hal_reg_set(device, REG_SPI_FIFO_MODE_REG_ADDR, 0);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_hal_reg_set(device, REG_SPI_IFACE_CONFIG_A_ADDR, reg_val);     /* Set config reg for spi0 or spi1 */
        ADI_APOLLO_ERROR_RETURN(err);
    }

    /* Restore the original active regio */
    if (active_regio_save != NULL) {
        device->hal_info.active_regio = active_regio_save;
    }

    return err;
}

int32_t adi_apollo_device_hsci_configure(adi_apollo_device_t *device, adi_apollo_device_hsci_settings_t *hsci_config)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(hsci_config);

    if (hsci_config->auto_linkup_en == 1) {
        err = adi_apollo_private_device_hsci_auto_linkup_configure(device);
        ADI_APOLLO_ERROR_RETURN(err);
    } else {
        err = adi_apollo_private_device_hsci_manual_linkup_configure(device);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_uuid_get(adi_apollo_device_t *device, uint8_t uuid[], uint32_t uuid_len)
{
    int32_t err, i;
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_INVALID_PARAM_RETURN(uuid_len != ADI_APOLLO_UUID_NUM_BYTES);

    for (i = 0; i < uuid_len; i++) {
        err = adi_apollo_hal_reg_get(device, REG_CHIPID_0_RSA_ADDR + i, (uint8_t *)uuid + i);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_tmu_enable(adi_apollo_device_t *device)
{
    int32_t err = API_CMS_ERROR_OK;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();

    adi_apollo_mailbox_resp_set_enabled_temp_sensors_t temp_sensors_resp;
    err = adi_apollo_mailbox_set_enabled_temp_sensors(device, &temp_senors_cmd, &temp_sensors_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_device_tmu_get(adi_apollo_device_t *device, adi_apollo_device_tmu_data_t *tmu_data)
{
    int32_t err = API_CMS_ERROR_OK;
    adi_apollo_device_tmu_index_e temp_degrees_celsius_index;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_LOG_FUNC();
    ADI_APOLLO_NULL_POINTER_RETURN(tmu_data);

    /*
     * Read device temperature sensors.
     */
    adi_apollo_mailbox_cmd_get_device_temperature_t get_temp_cmd = {
        .avg_mask = temp_senors_cmd.temp_sensor_mask
    };
    adi_apollo_mailbox_resp_get_device_temperature_t get_temp_resp;
    err = adi_apollo_mailbox_get_device_temperature(device, &get_temp_cmd, &get_temp_resp);
    ADI_APOLLO_ERROR_RETURN(err);

    // Extract values to the tmu_data struct
    tmu_data->avg_mask = get_temp_resp.temp_data.avg_mask;
    tmu_data->max_temp_degrees_celsius = get_temp_resp.temp_data.max_temp_degrees_celsius;
    tmu_data->min_temp_degrees_celsius = get_temp_resp.temp_data.min_temp_degrees_celsius;
    for (temp_degrees_celsius_index = ADI_APOLLO_DEVICE_TMU_SERDES_PLL; temp_degrees_celsius_index <= ADI_APOLLO_DEVICE_TMU_CLK_C; temp_degrees_celsius_index++) {
        tmu_data->temp_degrees_celsius[temp_degrees_celsius_index] = get_temp_resp.temp_data.temp_degrees_celsius[temp_degrees_celsius_index];
    }

    tmu_data->temp_degrees_celsius_avg = get_temp_resp.temp_data.temp_degrees_celsius_avg;

    return API_CMS_ERROR_OK;
}

/* Configure device spi */
static int32_t configure_spi(adi_apollo_device_t *device)
{
    int32_t err;

    ADI_APOLLO_NULL_POINTER_RETURN(device);

    if (device->hal_info.spi0_desc.is_used) {
        err = adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_SPI0);
        ADI_APOLLO_ERROR_RETURN(err);

        err = adi_apollo_device_spi_configure(device, &device->hal_info.spi0_desc.spi_config, 0);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if (device->hal_info.spi1_desc.is_used) {
        err = adi_apollo_device_spi_configure(device, &device->hal_info.spi1_desc.spi_config, 1);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

/* Initialize device context info */
static int32_t device_context_init(adi_apollo_device_t* device)
{
    int32_t err;
    uint8_t rev_major;

    ADI_APOLLO_NULL_POINTER_RETURN(device);

    /* Set 4t4r/8t8r flag */
    err = adi_apollo_hal_bf_get(device, BF_MASK_REVISION_MAJOR_INFO, &rev_major, sizeof(rev_major));
    ADI_APOLLO_ERROR_RETURN(err);

    device->dev_info.is_8t8r = (rev_major >> 3) & 1;
    ADI_APOLLO_LOG_VAR(ADI_CMS_LOG_API, "Device config: %s", device->dev_info.is_8t8r ? "8T8R" : "4T4R");

    return API_CMS_ERROR_OK;
}

/*! @} */
