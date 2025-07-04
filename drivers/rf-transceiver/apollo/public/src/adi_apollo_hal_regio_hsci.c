/*!
 * \brief     HAL HSCI function implementations
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

#include "adi_apollo_config.h"
#include "adi_apollo_hal_regio_local.h"
#include "adi_apollo_hal_regio_hsci_types.h"

static int32_t hsci_regio_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t data);
static int32_t hsci_regio_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t data);
static int32_t hsci_regio_rmw_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t data, uint8_t mask, uint8_t is_paged);
//static int32_t hsci_regio_rmw_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t data, uint32_t mask, uint8_t is_paged);
static int32_t hsci_regio_paged_base_addr_set(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t address);
static int32_t hsci_regio_paged_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t address, uint8_t data);
static int32_t hsci_regio_paged_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t address, uint32_t data);
static int32_t hsci_regio_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data);
static int32_t hsci_regio_read32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data);
static int32_t hsci_regio_stream_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
static int32_t hsci_regio_stream_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
static int32_t hsci_regio_stream_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
static int32_t hsci_regio_stream_read32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
static int32_t hsci_regio_poll_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t alt_hsci_regio_poll_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t hsci_regio_read_bf(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask);
static int32_t hsci_regio_manual_linkup(adi_apollo_hal_regio_hsci_desc_t *desc, uint8_t enable, uint16_t link_up_signal_bits);
static int32_t hsci_regio_auto_linkup(adi_apollo_hal_regio_hsci_desc_t *desc, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv);
static int32_t hsci_regio_alink_tbl_get(adi_apollo_hal_regio_hsci_desc_t *desc, uint16_t *hscim_alink_table);

/* HSCI implementations of regio operations */
static adi_apollo_hal_regio_ops_t hsci_regio_ops = {
    .wr_reg =               (adi_apollo_hal_regio_wr_reg_t)             &hsci_regio_write,
    .wr_reg32 =             (adi_apollo_hal_regio_wr_reg32_t)           &hsci_regio_write32,
    .wr_reg_rmw =           (adi_apollo_hal_regio_wr_reg_rmw_t)         &hsci_regio_rmw_write,
    .wr_reg_paged =         (adi_apollo_hal_regio_wr_reg_paged_t)       &hsci_regio_paged_write,
    .wr_reg32_paged =       (adi_apollo_hal_regio_wr_reg32_paged_t)     &hsci_regio_paged_write32,
    .rd_reg =               (adi_apollo_hal_regio_rd_reg_t)             &hsci_regio_read,
    .rd_reg32 =             (adi_apollo_hal_regio_rd_reg32_t)           &hsci_regio_read32,
    .rd_reg_poll =          (adi_apollo_hal_regio_rd_reg_poll_t)        &hsci_regio_poll_read,
    .set_page_base_addr =   (adi_apollo_hal_regio_set_page_base_addr_t) &hsci_regio_paged_base_addr_set,
    .stream_wr_reg =        (adi_apollo_hal_regio_stream_wr_reg_t)      &hsci_regio_stream_write,
    .stream_wr_reg32 =      (adi_apollo_hal_regio_stream_wr_reg32_t)    &hsci_regio_stream_write32,
    .stream_rd_reg =        (adi_apollo_hal_regio_stream_rd_reg_t)      &hsci_regio_stream_read,
    .stream_rd_reg32 =      (adi_apollo_hal_regio_stream_rd_reg32_t)    &hsci_regio_stream_read32,
    .rd_bf =                (adi_apollo_hal_regio_rd_bf_t)              &hsci_regio_read_bf,
    .manual_linkup =        (adi_apollo_hal_regio_manual_linkup_t)      &hsci_regio_manual_linkup,
    .auto_linkup =          (adi_apollo_hal_regio_auto_linkup_t)        &hsci_regio_auto_linkup,
    .alink_tbl_get =        (adi_apollo_hal_regio_alink_tbl_get_t)      &hsci_regio_alink_tbl_get,
};

/* Predefined transaction configs */
static adi_apollo_hal_txn_config_t txn_cfgs[] = {
    {.addr_len = 4, .data_len = 1, .stream_len = 1},        /* Single 8-bit transaction */
    {.addr_len = 4, .data_len = 2, .stream_len = 1},        /* Single 16-bit transaction */
    {.addr_len = 4, .data_len = 4, .stream_len = 1},        /* Single 32-bit transaction */
};

/* Default poll read if not define */
static int32_t alt_hsci_regio_poll_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    return hsci_regio_read(desc, reg, data);
}

int32_t adi_apollo_l_hal_regio_hsci_init(adi_apollo_hal_regio_hsci_desc_t *desc)
{
    if (desc == NULL) {
        return API_CMS_ERROR_NULL_PARAM;
    }

    desc->base_regio.ops = &hsci_regio_ops;        // interface
    desc->base_regio.child_desc = desc;            // instance
    desc->base_regio.protocol_id = ADI_APOLLO_HAL_PROTOCOL_HSCI;

    /* If the user didn't specify an optional poll read hal, use the built in alternate */
    if (desc->poll_read == NULL) {
        hsci_regio_ops.rd_reg_poll = (adi_apollo_hal_regio_rd_reg_poll_t) &alt_hsci_regio_poll_read;
        desc->base_regio.poll_read_returns_val = 1;
    }
    desc->base_regio.poll_read_en = (desc->poll_read != NULL);

    return API_CMS_ERROR_OK;
}

static int32_t hsci_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t addr, uint32_t data, adi_apollo_hal_regio_hsci_bus_size_e buf_size)
{
    int32_t err;
    uint8_t xfer_sz_bytes = 0;
    uint8_t in_data[8] = {0};
    uint8_t out_data[8] = {0};

    if (!desc)
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    /* Assign 32-bit reg address */
    in_data[0] = (addr >> 0) & 0xFF;
    in_data[1] = (addr >> 8) & 0xFF;
    in_data[2] = (addr >> 16) & 0xFF;
    in_data[3] = (addr >> 24) & 0xFF;
    xfer_sz_bytes += 4;

    /* Assign data */
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8)
    {
        in_data[4] = data & 0xFF;
        xfer_sz_bytes += 1;
    }
    else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16)
    {
        in_data[4] = data & 0xFF;
        in_data[5] = (data >> 8) & 0xFF;
        xfer_sz_bytes += 2;
    }
    else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32)
    {
        in_data[4] = data & 0xFF;
        in_data[5] = (data >> 8) & 0xFF;
        in_data[6] = (data >> 16) & 0xFF;
        in_data[7] = (data >> 24) & 0xFF;
        xfer_sz_bytes += 4;
    }
    /* write method takes precedence over xfer if both defined */
    if (desc->write != NULL) {
        err = desc->write(desc->dev_obj, in_data, xfer_sz_bytes, &txn_cfgs[buf_size]);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else if (desc->xfer != NULL) {
        err = desc->xfer(desc->dev_obj, in_data, out_data, xfer_sz_bytes);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    return err;
}


static int32_t hsci_rmw(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t addr, uint32_t data, uint32_t mask, adi_apollo_hal_regio_hsci_bus_size_e buf_size)
{
    int32_t err;
    uint8_t xfer_sz_bytes = 0;
    uint8_t in_data[12] = {0};
    uint8_t out_data[12] = {0};
    adi_apollo_hal_txn_config_t txn_config;
    
    if (!desc)
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    /* Assign 32-bit reg address */
    in_data[0] = (addr >> 0) & 0xFF;
    in_data[1] = (addr >> 8) & 0xFF;
    in_data[2] = (addr >> 16) & 0xFF;
    in_data[3] = (addr >> 24) & 0xFF;
    xfer_sz_bytes += 4;

    /* Assign data */
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8)
    {
        in_data[4] = mask & 0xFF;
        in_data[5] = data & 0xFF;
        xfer_sz_bytes += 2;
    }
    else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16)
    {
        in_data[4] = mask & 0xFF;
        in_data[5] = (mask >> 8) & 0xFF;
        in_data[6] = data & 0xFF;
        in_data[7] = (data >> 8) & 0xFF;
        xfer_sz_bytes += 4;
    }
    else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32)
    {
        in_data[4] = mask & 0xFF;
        in_data[5] = (mask >> 8) & 0xFF;
        in_data[6] = (mask >> 16) & 0xFF;
        in_data[7] = (mask >> 24) & 0xFF;

        in_data[8] = data & 0xFF;
        in_data[9] = (data >> 8) & 0xFF;
        in_data[10] = (data >> 16) & 0xFF;
        in_data[11] = (data >> 24) & 0xFF;
        xfer_sz_bytes += 8;
    }
    /* write method takes precedence over xfer if both defined */
    if (desc->write != NULL) {
        txn_config = txn_cfgs[buf_size];
        txn_config.mask = mask;
        txn_config.is_bf_txn = 0x81; // is_bf_txn[7] = rmw, is_bf_txn[0] = is bf transaction
        err = desc->write(desc->dev_obj, in_data, xfer_sz_bytes, &txn_config);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else if (desc->xfer != NULL) {
        err = desc->xfer(desc->dev_obj, in_data, out_data, xfer_sz_bytes);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    return err;
}

static int32_t hsci_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t addr, uint32_t *data, adi_apollo_hal_regio_hsci_bus_size_e buf_size)
{
    int32_t err;
    uint8_t xfer_sz_bytes = 0;
    uint8_t in_data[8] = {0};
    uint8_t out_data[8] = {0};

    /* Assign 32-bit reg address */
    in_data[0] = (addr >> 0) & 0xFF;
    in_data[1] = (addr >> 8) & 0xFF;
    in_data[2] = (addr >> 16) & 0xFF;
    in_data[3] = (addr >> 24) & 0xFF;
    xfer_sz_bytes += 4;
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8) {
        xfer_sz_bytes += 1;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32) {
        xfer_sz_bytes += 4;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16) {
        xfer_sz_bytes += 2;
    } else {
        return API_CMS_ERROR_HSCI_REGIO_XFER;
    }

    /* read method takes precedence over xfer if both defined */
    if (desc->read != NULL) {
        err = desc->read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, &txn_cfgs[buf_size]);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else if (desc->xfer != NULL) {
        err = desc->xfer(desc->dev_obj, in_data, out_data, xfer_sz_bytes);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    *data = 0;
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8) {
        *data |= out_data[4];
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32) {
        *data |= out_data[4] << 24;
        *data |= out_data[5] << 16;
        *data |= out_data[6] << 8;
        *data |= out_data[7] << 0;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16) {
        *data |= out_data[4] << 8;
        *data |= out_data[5] << 0;
    } else {
        return API_CMS_ERROR_HSCI_REGIO_XFER;
    }
    return err;
}

static int32_t hsci_poll_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t addr, uint32_t *data, adi_apollo_hal_regio_hsci_bus_size_e buf_size, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    int32_t err;
    uint8_t xfer_sz_bytes = 0;
    uint8_t in_data[8] = {0};
    uint8_t out_data[8] = {0};

    /* Assign 32-bit reg address */
    in_data[0] = (addr >> 0) & 0xFF;
    in_data[1] = (addr >> 8) & 0xFF;
    in_data[2] = (addr >> 16) & 0xFF;
    in_data[3] = (addr >> 24) & 0xFF;
    xfer_sz_bytes += 4;
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8) {
        xfer_sz_bytes += 1;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32) {
        xfer_sz_bytes += 4;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16) {
        xfer_sz_bytes += 2;
    } else {
        return API_CMS_ERROR_HSCI_REGIO_XFER;
    }

    /* read method takes precedence over xfer if both defined */
    if (desc->poll_read != NULL) {
        err = desc->poll_read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, timeout_us, &reg_expect, &reg_mask, &txn_cfgs[buf_size]);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    *data = 0;
    if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8) {
        *data |= out_data[4];
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32) {
        *data |= out_data[4] << 24;
        *data |= out_data[5] << 16;
        *data |= out_data[6] << 8;
        *data |= out_data[7] << 0;
    } else if (buf_size == ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D16) {
        *data |= out_data[4] << 8;
        *data |= out_data[5] << 0;
    } else {
        return API_CMS_ERROR_HSCI_REGIO_XFER;
    }
    return err;
}

static int32_t stream_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_hsci_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont)
{
    int32_t err;
    uint32_t i;
    uint32_t xfer_sz_bytes = 0;
    
    uint32_t in_data_len;
    adi_apollo_hal_txn_config_t stream_txn_cfgs = {.addr_len = 4, .data_len = 1, .stream_len = stream_bytes};        /* Continuous 8-bit transaction */

#if defined(__KERNEL__) || defined(ADI_MIN_STACK_ALLOC)
    uint8_t *in_data;
    /* Take optional buffer in case it exists and size can, at least, transmit HSCI header */
    if (desc->buff != NULL) {
        if (desc->buff_len > ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD) {
            in_data_len = desc->buff_len - ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD;
            in_data = desc->buff;            
        } else {
            return API_CMS_ERROR_ERROR; // Chuck size not supported
        }            
    } else {
        return API_CMS_ERROR_NULL_PARAM;
    }
#else
    uint8_t in_data[ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_SIZE];
    in_data_len = ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_PAYLOAD_SIZE;
#endif
    
    if (stream_bytes > in_data_len) {
        return API_CMS_ERROR_ERROR; // Chuck size not supported
    }

    /* Assign 32-bit reg address */
    in_data[0] = (reg >> 0) & 0xFF;
    in_data[1] = (reg >> 8) & 0xFF;
    in_data[2] = (reg >> 16) & 0xFF;
    in_data[3] = (reg >> 24) & 0xFF;
    xfer_sz_bytes += ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD;
    xfer_sz_bytes += stream_bytes;

    for (i=0; i<stream_bytes; i++)
    {
        in_data[i+ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD] =  *(data+i);
    }

    /* write method takes precedence over xfer if both defined */
    if (desc->write != NULL) {
        err = desc->write(desc->dev_obj, in_data, xfer_sz_bytes, &stream_txn_cfgs);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else if (desc->xfer != NULL) {
        return API_CMS_ERROR_NOT_SUPPORTED;
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    return err;
}

static int32_t stream_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_hsci_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont)
{
    int32_t err;
    uint32_t i;
    uint32_t xfer_sz_bytes = 0;
    uint8_t in_data[8] = {0};
    uint32_t out_data_len;

    adi_apollo_hal_txn_config_t stream_txn_cfgs = {.addr_len = 4, .data_len = 1, .stream_len = stream_bytes};        /* Continuous 8-bit transaction */

#if defined(__KERNEL__) || defined(ADI_MIN_STACK_ALLOC)
    uint8_t *out_data;
    /* Take optional buffer in case it exists and size can, at least, transmit HSCI header */
    if (desc->buff != NULL) {
        if (desc->buff_len > ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD) {
            out_data_len = desc->buff_len - ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD;
            out_data = desc->buff;            
        } else {
            return API_CMS_ERROR_ERROR; // Chuck size not supported
        }            
    } else {
        return API_CMS_ERROR_NULL_PARAM;
    }
#else
    uint8_t out_data[ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_SIZE];
    out_data_len = ADI_APOLLO_HAL_REGIO_HSCI_STREAM_DEFAULT_PAYLOAD_SIZE;
#endif

    if (stream_bytes > out_data_len) {
        return API_CMS_ERROR_ERROR; // Chuck size not supported
    }

    /* Assign 32-bit reg address */
    in_data[0] = (reg >> 0) & 0xFF;
    in_data[1] = (reg >> 8) & 0xFF;
    in_data[2] = (reg >> 16) & 0xFF;
    in_data[3] = (reg >> 24) & 0xFF;
    xfer_sz_bytes += ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD;
    xfer_sz_bytes += stream_bytes;

    /* read method takes precedence over xfer if both defined */
    if (desc->read != NULL) {
        err = desc->read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, &stream_txn_cfgs);
        ADI_APOLLO_ERROR_RETURN(err);
    }
    else if (desc->xfer != NULL) {
        return API_CMS_ERROR_NOT_SUPPORTED;
    }
    else
    {
        return API_CMS_ERROR_NULL_PARAM;
    }

    for (i=0; i<stream_bytes; i++)
    {
        *(data+i) = out_data[i+ ADI_APOLLO_HAL_REGIO_HSCI_STREAM_HEADER_OVERHEAD];
    }

    return err;
}

/* 8-bit register write */
static int32_t hsci_regio_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t data)
{
    return hsci_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8);
}

/* 32-bit register write */
static int32_t hsci_regio_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t data)
{
    return hsci_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32);
}

/* 8-bit read-modify-write */
static int32_t hsci_regio_rmw_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t data, uint8_t mask, uint8_t is_paged)
{
    return hsci_rmw(desc, reg, data, mask, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8);
}

/* 8-bit read-modify-write */
int32_t hsci_regio_rmw_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t data, uint32_t mask, uint8_t is_paged)
{
    return hsci_rmw(desc, reg, data, mask, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32);
}

static int32_t hsci_regio_paged_base_addr_set(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg)
{

    /**
        HSCI doesn't support paged write in hardware. However, because the full 32-bit register address
        is passed to the write function, HSCI can treat it as a normal write transaction. This prevents
        special handling in the APIs
    */

    return 0;
}

/* 8-bit paged write */
static int32_t hsci_regio_paged_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t data)
{
    /**
        HSCI doesn't support paged writes in hardware. However, because the full 32-bit register address
        is passed to the write function, HSCI can treat it as a normal write transaction. This prevents
        special handling in the APIs
    */

    return hsci_regio_write(desc, reg, data);
}

/* 32-bit paged write */
static int32_t hsci_regio_paged_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t data)
{
    /**
        HSCI doesn't support paged write in hardware. However, because the full 32-bit register address
        is passed to the write function, HSCI can treat it as a normal write transaction. This prevents
        special handling in the APIs
    */

    return hsci_regio_write32(desc, reg, data);
}

/* 8-bit register read */
static int32_t hsci_regio_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data)
{
    int32_t err;
    uint32_t rd_val;

    err = hsci_read(desc, reg, &rd_val, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8);
    *data = (uint8_t) rd_val;

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

/* 32-bit register read */
static int32_t hsci_regio_read32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data)
{
    return hsci_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32);
}

/* 8 bit stream write */
static int32_t hsci_regio_stream_write(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    err = stream_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8, data_len_bytes, is_cont);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

/* 32 bit stream write */
static int32_t hsci_regio_stream_write32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;
    err = stream_write(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32, data_len_words*4, is_cont);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

/* 8 bit stream read */
static int32_t hsci_regio_stream_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    err = stream_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8, data_len_bytes, is_cont);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

/* 32 bit stream read */
static int32_t hsci_regio_stream_read32(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;
    err = stream_read(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D32, data_len_words*4, is_cont);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

/* 8-bit register polled read */
static int32_t hsci_regio_poll_read(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{

    *data = 0;

    return hsci_poll_read(desc, reg, (uint32_t *)data, timeout_us, reg_expect, reg_mask, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8);
}

static int32_t hsci_regio_read_bf(adi_apollo_hal_regio_hsci_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask)
{
    int32_t err;

    err = stream_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_HSCI_BUS_SIZE_D8, data_len_bytes, false);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_HSCI_REGIO_XFER;
}

static int32_t hsci_regio_manual_linkup(adi_apollo_hal_regio_hsci_desc_t *desc, uint8_t enable, uint16_t link_up_signal_bits)
{
    int32_t err;

    err = desc->manual_linkup(desc->dev_obj, enable, link_up_signal_bits);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

static int32_t hsci_regio_auto_linkup(adi_apollo_hal_regio_hsci_desc_t *desc, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv)
{
    int32_t err;

    err = desc->auto_linkup(desc->dev_obj, enable, hscim_mosi_clk_inv, hscim_miso_clk_inv);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}

static int32_t hsci_regio_alink_tbl_get(adi_apollo_hal_regio_hsci_desc_t *desc, uint16_t *hscim_alink_table)
{
    int32_t err;

    err = desc->alink_tbl_get(desc->dev_obj, hscim_alink_table);
    ADI_APOLLO_ERROR_RETURN(err);

    return err;
}