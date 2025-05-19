/*!
 * \brief     HAL SPI function implementations
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

#include "adi_apollo_config.h"
#include "adi_apollo_bf_core.h"
#include "adi_apollo_hal_regio_local.h"
#include "adi_apollo_hal_regio_spi_types.h"

#define FIFO_MODE_BYTE_CNT_EN_THRESH 1    /* Number of bytes where spi fifo mode enabled */

static int32_t spi_regio_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t data);
static int32_t spi_regio_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data);
static int32_t spi_regio_rmw_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t data, uint8_t mask, uint8_t is_paged);
static int32_t spi_regio_paged_base_addr_set(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t address);
static int32_t spi_regio_paged_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t address, uint8_t data);
static int32_t spi_regio_paged_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t address, uint32_t data);
static int32_t spi_regio_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data);
static int32_t spi_regio_read32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data);
static int32_t spi_regio_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t spi_regio_stream_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
static int32_t spi_regio_stream_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
static int32_t spi_regio_stream_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
static int32_t spi_regio_stream_read32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
static int32_t indirect_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t alt_spi_regio_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t spi_regio_read_bf(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t bf_mask);
static int32_t spi_regio_read_bf_direct(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask);
static int32_t spi_regio_read_bf_indirect(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask);
static int32_t indirect_readx(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t indirect_writex(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t stream_readx(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont);
static int32_t stream_writex(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont);

/* SPI implementations of regio operations */
static adi_apollo_hal_regio_ops_t spi_regio_ops = {
    .wr_reg =               (adi_apollo_hal_regio_wr_reg_t)             &spi_regio_write,
    .wr_reg32 =             (adi_apollo_hal_regio_wr_reg32_t)           &spi_regio_write32,
    .wr_reg_rmw =           (adi_apollo_hal_regio_wr_reg_rmw_t)         &spi_regio_rmw_write,
    .wr_reg_paged =         (adi_apollo_hal_regio_wr_reg_paged_t)       &spi_regio_paged_write,
    .wr_reg32_paged =       (adi_apollo_hal_regio_wr_reg32_paged_t)     &spi_regio_paged_write32,
    .rd_reg =               (adi_apollo_hal_regio_rd_reg_t)             &spi_regio_read,
    .rd_reg32 =             (adi_apollo_hal_regio_rd_reg32_t)           &spi_regio_read32,
    .rd_reg_poll =          (adi_apollo_hal_regio_rd_reg_poll_t)        &spi_regio_poll_read,
    .set_page_base_addr =   (adi_apollo_hal_regio_set_page_base_addr_t) &spi_regio_paged_base_addr_set,
    .stream_wr_reg =        (adi_apollo_hal_regio_stream_wr_reg_t)      &spi_regio_stream_write,
    .stream_wr_reg32 =      (adi_apollo_hal_regio_stream_wr_reg32_t)    &spi_regio_stream_write32,
    .stream_rd_reg =        (adi_apollo_hal_regio_stream_rd_reg_t)      &spi_regio_stream_read,
    .stream_rd_reg32 =      (adi_apollo_hal_regio_stream_rd_reg32_t)    &spi_regio_stream_read32,
    .rd_bf =                (adi_apollo_hal_regio_rd_bf_t)              &spi_regio_read_bf
};

/* Predefined default transaction configs. */
static adi_apollo_hal_txn_config_t default_txn_cfgs[] = {
    {.addr_len = 2, .data_len = 1, .stream_len = 1, .mask = 0x000000ffull, .is_bf_txn = 0},        /* Single 8-bit transaction */
    {.addr_len = 2, .data_len = 2, .stream_len = 1, .mask = 0x0000ffffull, .is_bf_txn = 0},        /* Single 16-bit transaction */
    {.addr_len = 2, .data_len = 4, .stream_len = 1, .mask = 0xffffffffull, .is_bf_txn = 0},        /* Single 32-bit transaction */
};

static int32_t direct_or_paged_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t mask, uint8_t is_paged);
static int32_t indirect_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t direct_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t indirect_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t mask_reg_set(adi_apollo_hal_regio_spi_desc_t *desc, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t mask);
static int32_t indirect_addr_set(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t dma_ctrl);
static int32_t direct_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
static int32_t stream_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont);
static int32_t stream_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont);
static int32_t set_spi_fifo_mode(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t is_fifo_enabled, adi_apollo_hal_regio_spi_bus_size_e bus_size);
static int32_t align_page_base_address(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, adi_apollo_hal_regio_spi_bus_size_e size, uint32_t *reg_offset_p);
static int32_t txn_config_set(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t addr_siz_bytes, uint8_t data_siz_bytes, uint8_t stream_len, uint64_t mask, uint8_t is_bf_txn);
static int32_t txn_config_default_set(adi_apollo_hal_regio_spi_desc_t *desc, adi_apollo_hal_regio_spi_bus_size_e spi_bus_size);
static int32_t spi_write_stream_fifo_mode(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask, uint8_t is_bf);

/* Default poll read if not defined */
static int32_t alt_spi_regio_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    return spi_regio_read(desc, reg, data);
}

int32_t adi_apollo_l_hal_regio_spi_init(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t is_spi1)
{
    if (desc == NULL) {
        return API_CMS_ERROR_NULL_PARAM;
    }

    desc->base_regio.ops = &spi_regio_ops;     // interface
    desc->base_regio.child_desc = desc;        // instance (child)
    desc->base_regio.protocol_id = is_spi1 ? ADI_APOLLO_HAL_PROTOCOL_SPI1 : ADI_APOLLO_HAL_PROTOCOL_SPI0;
    desc->is_spi1 = is_spi1;

    /* If the user didn't specify an optional poll read hal for SPI0, use the built in alternate */
    if (!is_spi1 && desc->poll_read == NULL) {
        spi_regio_ops.rd_reg_poll = (adi_apollo_hal_regio_rd_reg_poll_t) &alt_spi_regio_poll_read;
        desc->base_regio.poll_read_returns_val = 1;
    }
    desc->base_regio.poll_read_en = (desc->poll_read != NULL);

    return API_CMS_ERROR_OK;
}

/* 8-bit register write */
static int32_t spi_regio_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t data)
{
    int32_t err;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = direct_or_paged_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
    } else {
        /* spi indirect */
        err = indirect_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 32-bit register write */
static int32_t spi_regio_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data)
{
    int32_t err;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = direct_or_paged_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
    } else {
        /* spi indirect */
        err = indirect_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 8-bit read-modify-write */
static int32_t spi_regio_rmw_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t data, uint8_t mask, uint8_t is_paged)
{
    int32_t err = API_CMS_ERROR_OK;
    uint32_t reg_offset = reg;

    if ((reg >> 24) == 0x47 && !is_paged) {
        reg_offset = reg;
    } else if (is_paged) {
        err = align_page_base_address(desc, reg, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, &reg_offset);
    } else {
        /* spi indirect doesn't support masked writes */
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    if (err == API_CMS_ERROR_OK) {
        err = direct_or_paged_write(desc, reg_offset, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, mask, is_paged);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

static int32_t spi_regio_paged_base_addr_set(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg)
{
    int32_t err;
    uint32_t reg_word_aligned = reg & ~0x03ul;    /* reg address must be on 32-bit boundary */

    // Program the spi paged base address
    uint32_t spi_page_base_reg = desc->is_spi1 ?  REG_SPI1_PAGE_ADDRESS_31TO24_ADDR : REG_SPI0_BASE_PAGE_ADDRESS_31TO24_ADDR;

    //TODO: work in progress err = spi_write_stream_fifo_mode(desc, spi_page_base_reg, &reg_word_aligned, 4, 0xffffffff, 0);

    if (err = spi_regio_write(desc, spi_page_base_reg + 0, (uint8_t)(reg_word_aligned >> 24)), err != API_CMS_ERROR_OK) {
        return err;
    }
    if (err = spi_regio_write(desc, spi_page_base_reg + 1, (uint8_t)(reg_word_aligned >> 16)), err != API_CMS_ERROR_OK) {
        return err;
    }
    if (err = spi_regio_write(desc, spi_page_base_reg + 2, (uint8_t)(reg_word_aligned >>  8)), err != API_CMS_ERROR_OK) {
        return err;
    }
    if (err = spi_regio_write(desc, spi_page_base_reg + 3, (uint8_t)(reg_word_aligned >>  0)), err != API_CMS_ERROR_OK) {
        return err;
    }

    desc->page_base_addr = reg_word_aligned;

    return API_CMS_ERROR_OK;
}

/* 8-bit paged write */
static int32_t spi_regio_paged_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t data)
{
    int32_t err;
    uint32_t reg_offset;

    /* spi paged write - base addr is on 32-bit boundary. Offsets can be 0 to 0x3fff */
    err = align_page_base_address(desc, reg, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, &reg_offset);
    if (err != API_CMS_ERROR_OK) {
        return err;
    }

    err = direct_or_paged_write(desc, reg_offset, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_PAGED);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 32-bit paged write */
static int32_t spi_regio_paged_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data)
{
    int32_t err;
    uint32_t reg_offset;

    /* spi paged write - base addr is on 32-bit boundary. Offsets can be 0 to 0x3fff */
    err = align_page_base_address(desc, reg, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, &reg_offset);
    if (err != API_CMS_ERROR_OK) {
        return err;
    }

    /* spi paged write 32 */
    err = direct_or_paged_write(desc, reg_offset, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_PAGED);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 8-bit register read */
static int32_t spi_regio_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data)
{
    int32_t err;
    uint32_t rd_val;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = direct_read(desc, reg, &rd_val, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
    } else {
        /* spi indirect */
        err = indirect_read(desc, reg, &rd_val, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
    }

    *data = (uint8_t) rd_val;

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 32-bit register read */
static int32_t spi_regio_read32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data)
{
    int32_t err;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = direct_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32);
    } else {
        /* spi indirect */
        err = indirect_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

static int32_t spi_regio_read_bf(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask)
{
    int32_t err = API_CMS_ERROR_OK;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = spi_regio_read_bf_direct(desc, reg, data, data_len_bytes, mask);
    } else {
        /* spi indirect */
        err = spi_regio_read_bf_indirect(desc, reg, data, data_len_bytes, mask);
    }

    return err;
}

static int32_t spi_regio_read_bf_direct(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask)
{
    int32_t i, err;

    uint8_t in_data[16] = { 0 };
    uint8_t out_data[16] = { 0 };
    uint8_t xfer_sz_bytes;

    /* Set txn config info for bitfield */
    txn_config_set(desc, 2, 1, data_len_bytes, mask, 1);        /* 2 addr, 1 data, data_len_bytes */

    /* Direct address (from 0x470000000) */
    in_data[0] = ((reg >> 8) & 0x3F) | 0x80;
    in_data[1] = (reg >> 0) & 0xFF;

    for (i = 0; i < data_len_bytes; i++) {
        in_data[2 + i] = data[i];
    }
    xfer_sz_bytes = 2 + data_len_bytes;

    err = desc->read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, &desc->txn_config);

    for (i = 0; i < data_len_bytes; i++) {
        data[i] = out_data[i + 2];
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}


static int32_t spi_regio_read_bf_indirect(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask)
{
    int32_t dma_dat0, i, err;

    uint8_t spidma_ahb_bus_select = 0;
    uint8_t spidma_auto_incr = 1;
    uint8_t spidma_bus_size = ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
    uint8_t spidma_sys_codeb = 0;
    uint8_t spidma_rd_wrb = 1;
    uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);
    uint8_t in_data[16] = { 0 };
    uint8_t out_data[16] = { 0 };
    uint8_t xfer_sz_bytes;

    indirect_addr_set(desc, reg, spidma_ctl_data);

    /* FIFO mode is efficient if #bytes > 4 */
    if (data_len_bytes > FIFO_MODE_BYTE_CNT_EN_THRESH) {
        set_spi_fifo_mode(desc, 1, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
    }

    /* Set txn config info for bitfield */
    txn_config_set(desc, 2, 1, data_len_bytes, mask, 1);        /* 2 addr, 1 data, data_len_bytes */

    /* Set SPI address to spi dma lsb. Using auto increment mode */
    dma_dat0 = desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR;
    in_data[0] = ((dma_dat0 >> 8) & 0x3F) | 0x80;
    in_data[1] =  (dma_dat0 >> 0) & 0xFF;

    for (i = 0; i < data_len_bytes; i++) {
        in_data[2 + i] = data[i];
    }
    xfer_sz_bytes = 2 + data_len_bytes;
    err = desc->read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, &desc->txn_config);

    for (i = 0; i < data_len_bytes; i++) {
        data[i] = out_data[i + 2];
    }

    /* Reset txn spi fifo mode */
    if (data_len_bytes > FIFO_MODE_BYTE_CNT_EN_THRESH) {
        set_spi_fifo_mode(desc, 0, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}










/* 8 bit stream write */
static int32_t spi_regio_stream_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    if (desc->wr_stream_en) {
        err = stream_write(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, data_len_bytes, is_cont);
    } else {
        err = stream_writex(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, data_len_bytes, is_cont);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 32 bit stream write */
static int32_t spi_regio_stream_write32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;

    if (desc->wr_stream_en) {
        err = stream_write(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, data_len_words * 4, is_cont);
    } else {
        err = stream_writex(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, data_len_words * 4, is_cont);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 8 bit stream read */
static int32_t spi_regio_stream_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont)
{
    int32_t err;
    if (desc->rd_stream_en) {
        err = stream_read(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, data_len_bytes, is_cont);
    } else {
        err = stream_readx(desc, reg, data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, data_len_bytes, is_cont);
    }
    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

/* 32 bit stream read */
static int32_t spi_regio_stream_read32(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, uint32_t data_len_words, uint8_t is_cont)
{
    int32_t err;
    if (desc->rd_stream_en) {
        err = stream_read(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, data_len_words * 4, is_cont);
    } else {
        err = stream_readx(desc, reg, (uint8_t *)data, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32, data_len_words * 4, is_cont);
    }

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

static int32_t indirect_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    int32_t err;
    uint8_t spidma_ahb_bus_select = 0;
    uint8_t spidma_auto_incr = 0;
    uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
    uint8_t spidma_sys_codeb = 0;
    uint8_t spidma_rd_wrb = 1;
    uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);
    uint8_t in_data[10] = {0};
    uint8_t out_data[10] = {0};
    uint8_t xfer_sz_bytes;

    /* Set the dma ctl and address */
    err = indirect_addr_set(desc, reg, spidma_ctl_data);
    ADI_APOLLO_ERROR_RETURN(err);

    /* in_data: [DMA address last 8 bits] [reg address] */
    in_data[0] = REG_SPIDMA0_ADDR3_ADDR & 0xFFU;
    in_data[1] = REG_SPIDMA0_ADDR2_ADDR & 0xFFU;
    in_data[2] = REG_SPIDMA0_ADDR1_ADDR & 0xFFU;
    in_data[3] = REG_SPIDMA0_ADDR0_ADDR & 0xFFU;

    in_data[4] = (reg >> 24) & 0xFFU;
    in_data[5] = (reg >> 16) & 0xFFU;
    in_data[6] = (reg >> 8) & 0xFFU;
    in_data[7] = (reg >> 0) & 0xFFU;

    xfer_sz_bytes = 8;

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        xfer_sz_bytes += 1;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        xfer_sz_bytes += 4;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        xfer_sz_bytes += 2;
    } else {
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    // platform driver with ATE mods - needs work
    err = desc->poll_read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, timeout_us, &reg_expect, &reg_mask, &default_txn_cfgs[bus_size]);
    ADI_APOLLO_ERROR_RETURN(err);

    *data = 0;
    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        *data |= out_data[2];
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
        *data |= out_data[4] << 16;
        *data |= out_data[5] << 24;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
    } else {
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    return API_CMS_ERROR_OK;
}

/* 8-bit register polled read */
static int32_t spi_regio_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    int32_t err;
    uint32_t rd_val;

    if ((reg >> 24) == 0x47) {
        /* spi direct */
        err = direct_poll_read(desc, reg, &rd_val, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, timeout_us, reg_expect, reg_mask);
    } else {
        /* spi indirect */
        err = indirect_poll_read(desc, reg, &rd_val, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8, timeout_us, reg_expect, reg_mask);
    }

    *data = (uint8_t) rd_val;

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

static int32_t direct_or_paged_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t mask, uint8_t is_paged)
{
    int32_t err;
    uint8_t in_data[6] = {0};
    uint8_t out_data[6] = {0};
    uint8_t xfer_sz_bytes;

    if (desc->base_regio.rmw_enabled && (mask != ADI_APOLLO_HAL_REGIO_NOMASK))
    {
        err = mask_reg_set(desc, bus_size, mask);   /* sets mask reg if necessary */
    }

    /* Direct address (from 0x470000000) */
    in_data[0] = ((reg >> 8) & 0x3F) | (is_paged ? 0x40 : 0);
    in_data[1] = (reg >> 0) & 0xFF;
    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        in_data[2] = (uint8_t)(data >> 0);
        xfer_sz_bytes = 3;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        in_data[2] = (uint8_t)(data >> 0);
        in_data[3] = (uint8_t)(data >> 8);
        in_data[4] = (uint8_t)(data >> 16);
        in_data[5] = (uint8_t)(data >> 24);
        xfer_sz_bytes = 6;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        in_data[2] = (uint8_t)(data >> 0);
        in_data[3] = (uint8_t)(data >> 8);
        xfer_sz_bytes = 4;
    } else {
        return -1;
    }

    /* write method takes precedence over xfer if both defined */
    if (desc->write != NULL) {
        err = desc->write(desc->dev_obj, in_data, xfer_sz_bytes, &default_txn_cfgs[bus_size]);
        ADI_APOLLO_ERROR_RETURN(err);
    } else {
        err = desc->xfer(desc->dev_obj, in_data, out_data, xfer_sz_bytes);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    return err;
}

static int32_t indirect_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size)
{
    if (desc->wr_stream_en)
    {
        return stream_write(desc, reg, (uint8_t *) &data, bus_size, (1 << bus_size), 0);
    } else {
        return indirect_writex(desc, reg, data, bus_size);
    }
}

static int32_t mask_reg_set(adi_apollo_hal_regio_spi_desc_t *desc, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t mask)
{
    int32_t err;

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        if (mask == 0xffffffff) {
            return API_CMS_ERROR_OK;
        }
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPI1_MASK_31TO24_ADDR : REG_SPI0_MASK_31TO24_ADDR, (mask >> 24) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPI1_MASK_23TO16_ADDR : REG_SPI0_MASK_23TO16_ADDR, (mask >> 16) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if ((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) {
        if ((mask & 0xffff) == 0xffff) {
            return API_CMS_ERROR_OK;
        }
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPI1_MASK_15TO8_ADDR : REG_SPI0_MASK_15TO8_ADDR, (mask >> 8) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if ((mask & 0xff) != 0xff) {
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPI1_MASK_7TO0_ADDR : REG_SPI0_MASK_7TO0_ADDR, (mask >> 0) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
    }


    return API_CMS_ERROR_OK;
}

static int32_t direct_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size)
{
    int32_t err;
    uint8_t in_data[6] = {0};
    uint8_t out_data[6] = {0};
    uint8_t xfer_sz_bytes;

    // ADI_APOLLO_NULL_POINTER_RETURN(desc);

    /* Direct address (from 0x470000000) */
    in_data[0] = ((reg >> 8) & 0x3F) | 0x80;
    in_data[1] = (reg >> 0) & 0xFF;

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        xfer_sz_bytes = 3;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        xfer_sz_bytes = 6;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        xfer_sz_bytes = 4;
    } else {
        return -1;
    }

    /* read method takes precedence over xfer if both defined */
    if (desc->read != NULL) {
        err = desc->read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, &default_txn_cfgs[bus_size]);
        ADI_APOLLO_ERROR_RETURN(err);
    } else {
        err = desc->xfer(desc->dev_obj, in_data, out_data, xfer_sz_bytes);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    *data = 0;
    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        *data |= out_data[2];
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
        *data |= out_data[4] << 16;
        *data |= out_data[5] << 24;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
    } else {
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    return API_CMS_ERROR_OK;
}

static int32_t indirect_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size)
{
    if (desc->rd_stream_en) {
        return stream_read(desc, reg, (uint8_t *)data, bus_size, (1 << bus_size), 0);
    } else {
        return indirect_readx(desc, reg, data, bus_size);
    }
}

static int32_t direct_poll_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask)
{
    int32_t err;
    uint8_t in_data[6] = {0};
    uint8_t out_data[6] = {0};
    uint8_t xfer_sz_bytes;

    /* Direct address (from 0x470000000) */
    in_data[0] = ((reg >> 8) & 0x3F) | 0x80;
    in_data[1] = (reg >> 0) & 0xFF;

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        xfer_sz_bytes = 3;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        xfer_sz_bytes = 6;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        xfer_sz_bytes = 4;
    } else {
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    err = desc->poll_read(desc->dev_obj, in_data, out_data, xfer_sz_bytes, timeout_us, &reg_expect, &reg_mask, &default_txn_cfgs[bus_size]);
    ADI_APOLLO_ERROR_RETURN(err);

    *data = 0;
    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
        *data |= out_data[2];
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
        *data |= out_data[4] << 16;
        *data |= out_data[5] << 24;
    } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
        *data |= out_data[2] << 0;
        *data |= out_data[3] << 8;
    } else {
        return API_CMS_ERROR_SPI_REGIO_XFER;
    }

    return API_CMS_ERROR_OK;
}

static int32_t indirect_addr_set(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t dma_ctrl)
{
    int32_t err;

    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_CTL_ADDR : REG_SPIDMA0_CTL_ADDR, dma_ctrl);
    ADI_APOLLO_ERROR_RETURN(err);
    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_ADDR3_ADDR : REG_SPIDMA0_ADDR3_ADDR, (reg >> 24) & 0xFF);
    ADI_APOLLO_ERROR_RETURN(err);
    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_ADDR2_ADDR : REG_SPIDMA0_ADDR2_ADDR, (reg >> 16) & 0xFF);
    ADI_APOLLO_ERROR_RETURN(err);
    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_ADDR1_ADDR : REG_SPIDMA0_ADDR1_ADDR, (reg >>  8) & 0xFF);
    ADI_APOLLO_ERROR_RETURN(err);
    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_ADDR0_ADDR : REG_SPIDMA0_ADDR0_ADDR, (reg >>  0) & 0xFF);
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

/*
    This mode is introduced in SPI Slave to improve SPI Indirect DMA efficiency when it is in auto-increment/decrement mode.

    spi_fifo_mode[2:0]  Description
        000             SPI usual behavior (no change)
        001             8-bit access
        010             16-bit access
        011             32-bit access

*/
static __maybe_unused int32_t set_spi_fifo_mode(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t is_fifo_enabled, adi_apollo_hal_regio_spi_bus_size_e bus_size)
{
    int32_t err;

    if (is_fifo_enabled) {
        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
            err = spi_regio_write(desc, REG_SPI_FIFO_MODE_REG_ADDR, 0x03u);
        } else if(bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
            err = spi_regio_write(desc, REG_SPI_FIFO_MODE_REG_ADDR, 0x02u);
        } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) {
            err = spi_regio_write(desc, REG_SPI_FIFO_MODE_REG_ADDR, 0x01u);
        }
    } else {
        err = spi_regio_write(desc, REG_SPI_FIFO_MODE_REG_ADDR, 0x00u);
    }

    return err;
}

static int32_t stream_write(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont)
{
    int32_t err;

    uint32_t pending_sz_bytes = stream_bytes;
    uint32_t data_index = 0;
    uint32_t data32;

    if(!is_cont) {
        uint8_t spidma_ahb_bus_select = 0;
        uint8_t spidma_auto_incr = 1;
        uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
        uint8_t spidma_sys_codeb = 0;
        uint8_t spidma_rd_wrb = 0;
        uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);

        //set_spi_fifo_mode(desc, 4);     // 4 = no fifo mode
        indirect_addr_set(desc, reg, spidma_ctl_data);
    }

    while (pending_sz_bytes > 0) {
        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32 && pending_sz_bytes > 3) {
            data32 = (data[data_index + 3] << 0) | (data[data_index + 2] << 8) | (data[data_index + 1] << 16) | (data[data_index + 0] << 24);
            err = direct_or_paged_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR,
                                        data32, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32,
                                        ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 4;
            data_index += 4;
        } else if (((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) && pending_sz_bytes > 1) {
            data32 = (data[data_index + 1] << 0) | (data[data_index + 0] << 8);
            err = direct_or_paged_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR,
                                        data32 & 0x0000ffff, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16,
                                        ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 2;
            data_index += 2;
        } else {
            data32 = data[data_index + 0];
            err = direct_or_paged_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR,
                                        data32 & 0x000000ff, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8,
                                        ADI_APOLLO_HAL_REGIO_NOMASK, ADI_APOLLO_HAL_REGIO_NOT_PAGED);
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 1;
            data_index += 1;
        }
    }

    return err;
}

static int32_t stream_read(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont)
{
    int32_t err;

    uint32_t pending_sz_bytes = stream_bytes;
    uint32_t data32;
    uint8_t *data_ptr;

    if(!is_cont) {
        uint8_t spidma_ahb_bus_select = 0;
        uint8_t spidma_auto_incr = 1;
        uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
        uint8_t spidma_sys_codeb = 0;
        uint8_t spidma_rd_wrb = 1;
        uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);

        indirect_addr_set(desc, reg, spidma_ctl_data);
    }

    *data = 0;

    data_ptr = (uint8_t *) data;

    while (pending_sz_bytes > 0) {
        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32 && pending_sz_bytes > 3) {
            err = direct_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR,
                                        &data32, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32);
            ADI_APOLLO_ERROR_RETURN(err);
            *(data_ptr + 0) = (data32 >> 24) & 0xff;
            *(data_ptr + 1) = (data32 >> 16) & 0xff;
            *(data_ptr + 2) = (data32 >> 8) & 0xff;
            *(data_ptr + 3) = (data32 >> 0) & 0xff;
            pending_sz_bytes -= 4;
            data_ptr += 4;
        } else if (((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) && pending_sz_bytes > 1) {
            err = direct_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR,
                                        &data32, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16);
            ADI_APOLLO_ERROR_RETURN(err);
            *(data_ptr + 0) = (data32 >>  8) & 0xff;
            *(data_ptr + 1) = (data32 >>  0) & 0xff;
            pending_sz_bytes -= 2;
            data_ptr += 2;
        } else {
            err = direct_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR,
                                        &data32, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);
            ADI_APOLLO_ERROR_RETURN(err);
            *(data_ptr + 0) = (data32 >>  0) & 0xff;
            pending_sz_bytes -= 1;
            data_ptr += 1;
        }
    }

    return err;
}

/* Checks paged address alignment and updates address if out of active page range */
static int32_t align_page_base_address(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, adi_apollo_hal_regio_spi_bus_size_e size, uint32_t *reg_offset_p)
{
    int32_t err;
    uint32_t range =  (size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8) ? 0 : 3;   /* 8 bit, 32 bit */
    int32_t reg_offset = reg - desc->page_base_addr;

    if ( (reg_offset < 0) || ((reg_offset + range) > (ADI_APOLLO_DIRECT_SPI_REGION_LEN - 1)) )
    {
        err = spi_regio_paged_base_addr_set(desc, reg & 0xffffc000);   // Page offset out of range for this transaction. Updates desc->page_base_addr.
        if (err != API_CMS_ERROR_OK) {
            return err;
        }
        reg_offset = reg - desc->page_base_addr;
    }

    /* reg offset from page base */
    *reg_offset_p = (uint32_t)reg_offset;

    return API_CMS_ERROR_OK;
}

static __maybe_unused int32_t spi_write_stream_fifo_mode(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, uint32_t data_len_bytes, uint64_t mask, uint8_t is_bf)
{
    int32_t dma_dat0, err, i;
    uint8_t in_data[16] = { 0 };
    uint8_t xfer_sz_bytes;

    set_spi_fifo_mode(desc, 1, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);

    /* Set txn config info for bitfield */
    txn_config_set(desc, 2, 1, data_len_bytes, mask, is_bf); /* 2 addr, 1 data, data_len_bytes */

    /* Write the SPI DMA control reg data with first byte */
    dma_dat0 = desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR;
    in_data[0] = ((dma_dat0 >> 8) & 0x3F) | 0x80;
    in_data[1] =  (dma_dat0 >> 0) & 0xFF;

    for (i = 0; i < data_len_bytes; i++) {
        in_data[2 + i] = data[i];
    }
    xfer_sz_bytes = 2 + data_len_bytes;
    err = desc->write(desc->dev_obj, in_data, xfer_sz_bytes, &desc->txn_config);

    /* Reset txn spi fifo mode */
    set_spi_fifo_mode(desc, 0, ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D8);

    return (err == API_CMS_ERROR_OK) ? API_CMS_ERROR_OK : API_CMS_ERROR_SPI_REGIO_XFER;
}

static __maybe_unused int32_t txn_config_default_set(adi_apollo_hal_regio_spi_desc_t *desc, adi_apollo_hal_regio_spi_bus_size_e spi_bus_size)
{
    adi_apollo_hal_txn_config_t *txn_config_p = &desc->txn_config;
    adi_apollo_hal_txn_config_t *default_txn_config_p = &default_txn_cfgs[spi_bus_size];

    txn_config_p->addr_len = default_txn_config_p->addr_len;
    txn_config_p->data_len = default_txn_config_p->data_len;
    txn_config_p->stream_len = default_txn_config_p->stream_len;
    txn_config_p->mask = default_txn_config_p->mask;
    txn_config_p->is_bf_txn = default_txn_config_p->is_bf_txn;

    return API_CMS_ERROR_OK;
}


static int32_t txn_config_set(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t addr_siz_bytes, uint8_t data_siz_bytes, uint8_t stream_len, uint64_t mask, uint8_t is_bf_txn)
{
    adi_apollo_hal_txn_config_t *txn_config_p = &desc->txn_config;

    txn_config_p->addr_len = addr_siz_bytes;
    txn_config_p->data_len = data_siz_bytes;
    txn_config_p->stream_len = stream_len;
    txn_config_p->mask = mask;
    txn_config_p->is_bf_txn = is_bf_txn;

    return API_CMS_ERROR_OK;
}

/*
 * indirect_readx - Indirect register read that doesn't generate any streaming transactions. Use when rd_stream_en = 0.
 */
static int32_t indirect_readx(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size) {
    int32_t err;
    uint8_t spidma_ahb_bus_select = 0;
    uint8_t spidma_auto_incr = 0;
    uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
    uint8_t spidma_sys_codeb = 0;
    uint8_t spidma_rd_wrb = 1;
    uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);
    uint8_t *data_ptr;

    /* Set the dma ctl and address */
    err = indirect_addr_set(desc, reg, spidma_ctl_data);
    ADI_APOLLO_ERROR_RETURN(err);

    *data = 0;

    data_ptr = (uint8_t *) data;

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR, data_ptr + 3); /* msb */
        ADI_APOLLO_ERROR_RETURN(err);
        err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA2_ADDR : REG_SPIDMA0_DATA2_ADDR, data_ptr + 2);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if ((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) {
        err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR, data_ptr + 1);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR, data_ptr + 0); /* lsb */
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

/*
 * indirect_writex - Indirect register write that doesn't generate any streaming transactions. Use when wr_stream_en = 0.
 */
static int32_t indirect_writex(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint32_t data, adi_apollo_hal_regio_spi_bus_size_e bus_size) {
    int32_t err;
    uint8_t spidma_ahb_bus_select = 0;
    uint8_t spidma_auto_incr = 0;
    uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
    uint8_t spidma_sys_codeb = 0;
    uint8_t spidma_rd_wrb = 0;
    uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);

    /* Set the dma ctl and address */
    indirect_addr_set(desc, reg, spidma_ctl_data);

    if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR, (data >> 24) & 0xFF);      // MSB
        ADI_APOLLO_ERROR_RETURN(err);
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA2_ADDR : REG_SPIDMA0_DATA2_ADDR, (data >> 16) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    if ((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) {
        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR, (data >> 8) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
    }

    err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR, (data >> 0) & 0xFF);           // LSB
    ADI_APOLLO_ERROR_RETURN(err);

    return API_CMS_ERROR_OK;
}

/*
 * stream_readx - pseudo read stream function that doesn't generate any streaming transactions. Use when rd_stream_en = 0.
 */
static int32_t stream_readx(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont) {
    int32_t err;

    uint32_t pending_sz_bytes = stream_bytes;
    uint8_t *data_ptr;

    if (!is_cont) {
        uint8_t spidma_ahb_bus_select = 0;
        uint8_t spidma_auto_incr = 1;
        uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
        uint8_t spidma_sys_codeb = 0;
        uint8_t spidma_rd_wrb = 1;
        uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);

        indirect_addr_set(desc, reg, spidma_ctl_data);
    }

    *data = 0;

    data_ptr = (uint8_t *) data;

    while (pending_sz_bytes > 0) {
        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32 && pending_sz_bytes > 3) {
            err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR, (data_ptr + 3));
            ADI_APOLLO_ERROR_RETURN(err);
            err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA2_ADDR : REG_SPIDMA0_DATA2_ADDR, (data_ptr + 2));
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 2;
        }

        if (((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) && pending_sz_bytes > 1) {
            err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR, (data_ptr + 1));
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 1;
        }

        err = spi_regio_read(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR, (data_ptr + 0));
        ADI_APOLLO_ERROR_RETURN(err);
        pending_sz_bytes -= 1;

        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
            data_ptr += 4;
        } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
            data_ptr += 2;
        } else {
            data_ptr += 1;
        }
    }
    return err;
}

/*
 * stream_writex - pseudo write stream function that doesn't generate any streaming transactions. Use when wr_stream_en = 0.
 */
static int32_t stream_writex(adi_apollo_hal_regio_spi_desc_t *desc, uint32_t reg, uint8_t *data, adi_apollo_hal_regio_spi_bus_size_e bus_size, uint32_t stream_bytes, uint8_t is_cont) {
    int32_t err;

    uint32_t pending_sz_bytes = stream_bytes;
    uint32_t data_index = 0;

    if (!is_cont) {
        uint8_t spidma_ahb_bus_select = 0;
        uint8_t spidma_auto_incr = 1;
        uint8_t spidma_bus_size = bus_size; // Size of read/write. 0 = byte, 1 = half-word, 2 = full-word, 3 = invalid.
        uint8_t spidma_sys_codeb = 0;
        uint8_t spidma_rd_wrb = 0;
        uint8_t spidma_ctl_data = (spidma_rd_wrb << 7) | (spidma_sys_codeb << 6) | (spidma_bus_size << 2) | (spidma_auto_incr << 1) | (spidma_ahb_bus_select << 0);

        indirect_addr_set(desc, reg, spidma_ctl_data);
    }

    while (pending_sz_bytes > 0) {
        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32 && pending_sz_bytes > 3) {
            err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA3_ADDR : REG_SPIDMA0_DATA3_ADDR, (data[data_index + 3]) & 0xFF);
            ADI_APOLLO_ERROR_RETURN(err);
            err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA2_ADDR : REG_SPIDMA0_DATA2_ADDR, (data[data_index + 2]) & 0xFF);
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 2;
        }

        if (((bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) || (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16)) && pending_sz_bytes > 1) {
            err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA1_ADDR : REG_SPIDMA0_DATA1_ADDR, (data[data_index + 1]) & 0xFF);
            ADI_APOLLO_ERROR_RETURN(err);
            pending_sz_bytes -= 1;
        }

        err = spi_regio_write(desc, desc->is_spi1 ? REG_SPIDMA1_DATA0_ADDR : REG_SPIDMA0_DATA0_ADDR, (data[data_index]) & 0xFF);
        ADI_APOLLO_ERROR_RETURN(err);
        pending_sz_bytes -= 1;

        if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D32) {
            data_index += 4;
        } else if (bus_size == ADI_APOLLO_HAL_REGIO_SPI_BUS_SIZE_D16) {
            data_index += 2;
        } else {
            data_index += 1;
        }
    }

    return err;
}
