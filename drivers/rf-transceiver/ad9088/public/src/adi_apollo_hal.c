/*!
 * \brief     HAL function implementations
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_HAL
 * @{
 */

/*============= I N C L U D E S ============*/
#include "adi_apollo_config.h"
#include "adi_apollo_hal.h"
#include "adi_apollo_hal_regio_local.h"

/*============= C O D E ====================*/
static int32_t bf_set(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint64_t value, uint8_t is_paged);
static int32_t bf_set_rmw(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint64_t value, uint8_t is_paged);
static int32_t bf_get(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint8_t *value, uint8_t value_size_bytes);
static int32_t bf_wait(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly, uint8_t match);


int32_t adi_apollo_hal_hw_open(adi_apollo_device_t *device)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    if (device->hal_info.hw_open != NULL) {
        if (API_CMS_ERROR_OK != device->hal_info.hw_open(device->hal_info.dev_hal_info))
            return API_CMS_ERROR_HW_OPEN;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_hw_close(adi_apollo_device_t *device)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    if (device->hal_info.hw_close != NULL) {
        if (API_CMS_ERROR_OK != device->hal_info.hw_close(device->hal_info.dev_hal_info))
            return API_CMS_ERROR_HW_CLOSE;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_reset_pin_ctrl(adi_apollo_device_t *device, uint8_t enable)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.reset_pin_ctrl);
    if (API_CMS_ERROR_OK != device->hal_info.reset_pin_ctrl(device->hal_info.user_data, enable)){
        return API_CMS_ERROR_RESET_PIN_CTRL;
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_hal_delay_us(adi_apollo_device_t *device, uint32_t us)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.delay_us);
    if (API_CMS_ERROR_OK != device->hal_info.delay_us(device->hal_info.user_data, us)) {
        return API_CMS_ERROR_DELAY_US;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_log_write(adi_apollo_device_t *device, adi_cms_log_type_e log_type, const char* comment, ...)
{
    va_list argp;
    ADI_APOLLO_NULL_POINTER_RETURN(device);

    if (device->hal_info.log_write != NULL) {
        va_start(argp, comment);
        if (API_CMS_ERROR_OK != device->hal_info.log_write(device->hal_info.user_data, log_type, comment, argp))
            return API_CMS_ERROR_LOG_WRITE;
        va_end(argp);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_error_report(adi_apollo_device_t* device, adi_cms_log_type_e log_type,
        int32_t error, const char* file_name, const char* func_name, uint32_t line_num,
        const char* var_name, const char* comment)
{
    (void)error;        // Prevent compile errors til used.
    if (device == NULL)
        return API_CMS_ERROR_NULL_PARAM;

    if (API_CMS_ERROR_OK != adi_apollo_hal_log_write(device, log_type,
                "%s, \"%s\" in %s(...), line%d in %s",
                comment, var_name, func_name, line_num, file_name)) {
        return API_CMS_ERROR_LOG_WRITE;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_bf_wait_to_clear(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly)
{
    return bf_wait(device, reg, info, timeout, poll_dly, 0x00);
}

int32_t adi_apollo_hal_bf_wait_to_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly)
{
    return bf_wait(device, reg, info, timeout, poll_dly, 0x01);
}

int32_t adi_apollo_hal_bf_get(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint8_t value[], uint8_t value_size_bytes)
{
    int32_t err;
    uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
    uint8_t reg_bytes = ((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);
    ADI_APOLLO_INVALID_PARAM_RETURN(width > 64);
    ADI_APOLLO_INVALID_PARAM_RETURN(width < 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(reg_bytes > sizeof(uint64_t) + 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(reg_bytes < 1);

    desc = device->hal_info.active_regio;

    err = bf_get(desc, reg, offset, width, reg_bytes, value, value_size_bytes);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg(desc->child_desc, reg, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg32(desc->child_desc, reg, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_stream_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data[], uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->stream_wr_reg(desc->child_desc, reg, data, data_len_bytes, is_cont);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_stream_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data[], uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->stream_wr_reg32(desc->child_desc, reg, data, data_len_words, is_cont);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_stream_reg_get(adi_apollo_device_t *device, uint32_t reg, uint8_t data[], uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->stream_rd_reg(desc->child_desc, reg, data, data_len_bytes, is_cont);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_stream_reg32_get(adi_apollo_device_t *device, uint32_t reg, uint32_t data[], uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->stream_rd_reg32(desc->child_desc, reg, data, data_len_words, is_cont);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_masked_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data, uint8_t mask)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg_rmw(desc->child_desc, reg, data, mask, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_paged_masked_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data, uint8_t mask)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg_rmw(desc->child_desc, reg, data, mask, ADI_APOLLO_HAL_REGIO_PAGED);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_paged_base_addr_set(adi_apollo_device_t *device, uint32_t base_addr)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->set_page_base_addr(desc->child_desc, base_addr);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_paged_reg_set(adi_apollo_device_t *device, uint32_t reg, uint8_t data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg_paged(desc->child_desc, reg, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_paged_reg32_set(adi_apollo_device_t *device, uint32_t reg, uint32_t data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg32_paged(desc->child_desc, reg, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_reg_get(adi_apollo_device_t *device, uint32_t reg, uint8_t *data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg(desc->child_desc, reg, data);

    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_reg32_get(adi_apollo_device_t *device, uint32_t reg, uint32_t *data)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg32(desc->child_desc, reg, data);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_bf_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint64_t value)
{
    int32_t err;
    uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
    uint8_t reg_bytes = ((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);
    ADI_APOLLO_INVALID_PARAM_RETURN(width > 64);
    ADI_APOLLO_INVALID_PARAM_RETURN(width < 1);

    desc = device->hal_info.active_regio;

    if (desc->rmw_enabled) {
        err = bf_set_rmw(desc, reg, offset, width, reg_bytes, value, ADI_APOLLO_HAL_REGIO_PAGED);
        ADI_APOLLO_ERROR_RETURN(err);
    } else {
        err = bf_set(desc, reg, offset, width, reg_bytes, value, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_paged_bf_set(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint64_t value)
{
    int32_t err;
    uint8_t offset = (uint8_t)(info >> 0), width = (uint8_t)(info >> 8);
    uint8_t reg_bytes = ((width + offset) >> 3) + (((width + offset) & 7) == 0 ? 0 : 1);
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);
    ADI_APOLLO_INVALID_PARAM_RETURN(width > 64);
    ADI_APOLLO_INVALID_PARAM_RETURN(width < 1);

    desc = device->hal_info.active_regio;

    if (desc->rmw_enabled) {
        err = bf_set_rmw(desc, reg, offset, width, reg_bytes, value, ADI_APOLLO_HAL_REGIO_PAGED);
        ADI_APOLLO_ERROR_RETURN(err);
    } else {
        err = bf_set(desc, reg, offset, width, reg_bytes, value, ADI_APOLLO_HAL_REGIO_PAGED);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_hal_reg_poll_get(adi_apollo_device_t *device, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    int32_t err;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg_poll(desc->child_desc, reg, data, timeout_us, reg_expect, reg_mask);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_active_protocol_set(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);

    if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI0) {
        device->hal_info.active_regio = &device->hal_info.spi0_desc.base_regio;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI1) {
        device->hal_info.active_regio = &device->hal_info.spi1_desc.base_regio;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) {
        device->hal_info.active_regio = &device->hal_info.hsci_desc.base_regio;
    } else {
        return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_active_protocol_get(adi_apollo_device_t *device, adi_apollo_hal_protocol_e *protocol)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(protocol);

    *protocol = ((adi_apollo_hal_regio_t *)device->hal_info.active_regio)->protocol_id;

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_rmw_enable_set(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol, uint8_t rmw_en)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);

    if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI0) {
        device->hal_info.spi0_desc.base_regio.rmw_enabled = rmw_en;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI1) {
        device->hal_info.spi1_desc.base_regio.rmw_enabled = rmw_en;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) {
        device->hal_info.hsci_desc.base_regio.rmw_enabled = rmw_en;
    } else {
        return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_rmw_enable_get(adi_apollo_device_t *device, adi_apollo_hal_protocol_e protocol, uint8_t *rmw_en)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(rmw_en);

    if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI0) {
        *rmw_en = device->hal_info.spi0_desc.base_regio.rmw_enabled;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_SPI1) {
        *rmw_en = device->hal_info.spi1_desc.base_regio.rmw_enabled;
    } else if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) {
        *rmw_en = device->hal_info.hsci_desc.base_regio.rmw_enabled;
    } else {
        return API_CMS_ERROR_INVALID_PARAM;
    }

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_manual_linkup(adi_apollo_device_t *device, uint8_t enable, uint16_t link_up_signal_bits)
{
    int32_t err;
    adi_apollo_hal_protocol_e protocol;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    // Save the current protocol
    err = adi_apollo_hal_active_protocol_get(device, &protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    // Change the protocol to HSCI to access hooks for manual linkup HAL function
    err = adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_HSCI);
    ADI_APOLLO_ERROR_RETURN(err);

    // Invoke manual link-up method
    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->manual_linkup(desc->child_desc, enable, link_up_signal_bits);
    ADI_APOLLO_ERROR_RETURN(err);

    // Revert protocol
    err = adi_apollo_hal_active_protocol_set(device, protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_auto_linkup(adi_apollo_device_t *device, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv)
{
    int32_t err;
    adi_apollo_hal_protocol_e protocol;
    adi_apollo_hal_regio_t *desc;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    // Save the current protocol
    err = adi_apollo_hal_active_protocol_get(device, &protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    // Change the protocol to HSCI to access hooks for auto linkup HAL function
    err = adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_HSCI);
    ADI_APOLLO_ERROR_RETURN(err);

    // Invoke auto link-up method
    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->auto_linkup(desc->child_desc, enable, hscim_mosi_clk_inv, hscim_miso_clk_inv);
    ADI_APOLLO_ERROR_RETURN(err);

    // Revert protocol
    err = adi_apollo_hal_active_protocol_set(device, protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

int32_t adi_apollo_hal_alink_tbl_get(adi_apollo_device_t *device, uint16_t *hscim_alink_table)
{
    int32_t err;
    adi_apollo_hal_protocol_e protocol;
    adi_apollo_hal_regio_t *desc;
    
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(device->hal_info.active_regio);

    // Save the current protocol
    err = adi_apollo_hal_active_protocol_get(device, &protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    // Change the protocol to HSCI to access hooks for auto linkup HAL function
    err = adi_apollo_hal_active_protocol_set(device, ADI_APOLLO_HAL_PROTOCOL_HSCI);
    ADI_APOLLO_ERROR_RETURN(err);

    // Invoke auto link-up method
    desc = device->hal_info.active_regio;

    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->alink_tbl_get(desc->child_desc, hscim_alink_table);
    ADI_APOLLO_ERROR_RETURN(err);

    // Revert protocol
    err = adi_apollo_hal_active_protocol_set(device, protocol);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

static int32_t bf_set(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint64_t value, uint8_t is_paged)
{
    int32_t  err;
    uint32_t mask = 0;
    uint8_t  reg_offset = (offset/8), data8 = 0;
    offset = offset % 8;

    for (;reg_offset < reg_bytes; reg_offset++) {
        if ((offset + width) <= 8) { /* last 8bits */
            if ((offset > 0) || ((offset + width) < 8)) {
                err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg(desc->child_desc, reg + reg_offset, &data8);
                ADI_APOLLO_ERROR_RETURN(err);
            }
            mask = (1 << width) - 1;
            data8 = data8 & (~(mask << offset));
            data8 = data8 | ((value & mask) << offset);
        }
        else {
            if (offset > 0) {
                err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg(desc->child_desc, reg + reg_offset, &data8);
                ADI_APOLLO_ERROR_RETURN(err);
            }
            mask = (1 << (8 - offset)) - 1;
            data8 = data8 & (~(mask << offset));
            data8 = data8 | ((value & mask) << offset);
            value = value >> (8 - offset);
            width = width - (8 - offset);
            offset = 0;
        }
        if (is_paged) {
            err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg_paged(desc->child_desc, reg + reg_offset, data8);
        } else {
            err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg(desc->child_desc, reg + reg_offset, data8);
        }
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

static int32_t bf_set_rmw(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint64_t value, uint8_t is_paged)
{
    int32_t err;
    uint8_t mask = 0;
    uint8_t  reg_offset = (offset / 8), data8 = 0;
    offset = offset % 8;

    for (; reg_offset < reg_bytes; reg_offset++) {
        if ((offset + width) <= 8) {
            /* last 8bits */
            mask = ((1 << width) - 1) << offset;
            data8 = (uint8_t)(value << offset);
        }
        else {
            mask = ~((1 << offset) - 1); // rmw mask - 1's represent bits to set
            data8 = (uint8_t)(value << offset);
            value = value >> (8 - offset);
            width = width - (8 - offset);
            offset = 0;
        }

        err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->wr_reg_rmw(desc->child_desc, reg + reg_offset, data8, mask, is_paged);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return API_CMS_ERROR_OK;
}

static int32_t bf_get(adi_apollo_hal_regio_t *desc, uint32_t reg, uint8_t offset, uint8_t width, uint8_t reg_bytes, uint8_t value[], uint8_t value_size_bytes)
{
    int32_t  err;
    uint8_t  reg_offset = 0, data8 = 0;
    uint32_t mask = 0, endian_test_val = 0x11223344;
    uint64_t bf_val = 0;
    uint8_t  i = 0, j = 0, filled_bits = 0;

    uint64_t bf_mask64 = (reg_bytes >= sizeof(uint64_t)) ? UINT64_MAX : ((1ull<<width)-1) << offset; // Sent to txn
    uint8_t bf_reg_buff[sizeof(uint64_t) + 1] = { 0 };          // Num of bytes a bitfield can span. +1 required for offset in reg of lsb

    /* Read bytes containing the bitfield */
    err = ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_bf(desc->child_desc, reg, bf_reg_buff, reg_bytes, bf_mask64);
    ADI_APOLLO_ERROR_RETURN(err);

    reg_offset = (offset / 8);
    offset = offset % 8;

    for (; reg_offset < reg_bytes; reg_offset++) {
        if ((offset + width) <= 8) { /* last 8bits */
            mask   = (1 << width) - 1;
            data8 = bf_reg_buff[reg_offset];
            data8  = (data8 >> offset) & mask;
            bf_val = bf_val + ((uint64_t)data8 << filled_bits);
            filled_bits = filled_bits + width;
        }
        else {
            mask   = (1 << (8 - offset)) - 1;
            data8 = bf_reg_buff[reg_offset];
            data8  = (data8 >> offset) & mask;
            bf_val = bf_val + ((uint64_t)data8 << filled_bits);
            width = width - (8 - offset);
            filled_bits = filled_bits + (8 - offset);
            offset = 0;
        }
    }

    /* Save bitfield value to buffer with proper endianness */
    for(i = 0; i < value_size_bytes; i++) {
        j = (*(uint8_t *)&endian_test_val == 0x44) ? (i) : (value_size_bytes - 1 - i);
        value[j] = (uint8_t)(bf_val >> (i << 3));
    }

    return API_CMS_ERROR_OK;
}

static int32_t bf_wait(adi_apollo_device_t *device, uint32_t reg, uint32_t info, uint32_t timeout, uint32_t poll_dly, uint8_t match)
{
    int32_t err;
    uint8_t reg_or_bf_value = 0;
    uint8_t offset = (uint8_t)info;
    uint8_t width = (uint8_t)(info >> 8);
    uint8_t reg_mask = (1 << offset);
    uint8_t reg_exp = (match << offset);
    uint8_t reg_offset = offset / 8;
    uint32_t poll_dly_cnt = 0;
    adi_apollo_hal_regio_t *desc;

    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_INVALID_PARAM_RETURN(poll_dly < 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(width > 1);
    ADI_APOLLO_INVALID_PARAM_RETURN(match > 1);

    desc = device->hal_info.active_regio;

    if (desc->poll_read_en) {
        /* Poll read function handles timeout condition */
        return ((adi_apollo_hal_regio_ops_t*)desc->ops)->rd_reg_poll(desc->child_desc, reg + reg_offset, &reg_or_bf_value, timeout, reg_exp, reg_mask);
    } else {
        for (poll_dly_cnt = 0; poll_dly_cnt < timeout; poll_dly_cnt += poll_dly) {
            err = adi_apollo_hal_delay_us(device, poll_dly);
            ADI_APOLLO_ERROR_RETURN(err);
            err = adi_apollo_hal_bf_get(device, reg, info, &reg_or_bf_value, 1);
            ADI_APOLLO_ERROR_RETURN(err);

            if (reg_or_bf_value == match) {
               return API_CMS_ERROR_OK;
            }
        }
    }

    return API_CMS_ERROR_OPERATION_TIMEOUT;
}

int32_t adi_apollo_hal_buffer_set(adi_apollo_device_t* device, adi_apollo_hal_protocol_e protocol, uint8_t *buff, uint32_t buff_len)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(buff);
    ADI_APOLLO_INVALID_PARAM_RETURN(buff_len > ADI_APOLLO_HAL_BUFFER_LEN_MAX);
    ADI_APOLLO_INVALID_PARAM_RETURN(buff_len < ADI_APOLLO_HAL_BUFFER_LEN_MIN);

    if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) {
        device->hal_info.hsci_desc.buff = buff;
        device->hal_info.hsci_desc.buff_len = buff_len;
    } else {
        return API_CMS_ERROR_NOT_SUPPORTED;
    }

    return API_CMS_ERROR_OK;
}


int32_t adi_apollo_hal_buffer_get(adi_apollo_device_t* device, adi_apollo_hal_protocol_e protocol, uint8_t **buff, uint32_t *buff_len)
{
    ADI_APOLLO_NULL_POINTER_RETURN(device);
    ADI_APOLLO_NULL_POINTER_RETURN(buff);
    ADI_APOLLO_NULL_POINTER_RETURN(buff_len);

    if (protocol == ADI_APOLLO_HAL_PROTOCOL_HSCI) {
        *buff = device->hal_info.hsci_desc.buff;
        *buff_len = device->hal_info.hsci_desc.buff_len;
    } else {
        *buff = NULL;
        *buff_len = 0;
        return API_CMS_ERROR_NOT_SUPPORTED;
    }

    return API_CMS_ERROR_OK;
}

/*! @} */
