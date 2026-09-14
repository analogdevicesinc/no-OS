/*!
 * \brief     Register IO local defines and prototypes
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

#ifndef __ADI_APOLLO_HAL_REGIO_LOCAL_H__
#define __ADI_APOLLO_HAL_REGIO_LOCAL_H__

/*============= I N C L U D E S ============*/
#include "adi_apollo_hal_regio_spi_types.h"
#include "adi_apollo_hal_regio_hsci_types.h"

/*============= D E F I N E S ==============*/

#define ADI_APOLLO_HAL_REGIO_NOMASK 0
#define ADI_APOLLO_HAL_REGIO_NOT_PAGED 0
#define ADI_APOLLO_HAL_REGIO_PAGED  1

typedef int32_t (*adi_apollo_hal_regio_wr_reg_t)                (void *desc, uint32_t address, uint8_t data);
typedef int32_t (*adi_apollo_hal_regio_wr_reg32_t)              (void *desc, uint32_t address, uint32_t data);
typedef int32_t (*adi_apollo_hal_regio_rd_reg_t)                (void *desc, uint32_t address, uint8_t *data);
typedef int32_t (*adi_apollo_hal_regio_rd_reg32_t)              (void *desc, uint32_t address, uint32_t *data);
typedef int32_t (*adi_apollo_hal_regio_wr_reg_rmw_t)            (void *desc, uint32_t address, uint8_t data, uint8_t mask, uint8_t is_paged);
typedef int32_t (*adi_apollo_hal_regio_rd_reg_poll_t)           (void *desc, uint32_t address, uint8_t *data, uint32_t timeout_us, uint8_t reg_expect, uint8_t reg_mask);
typedef int32_t (*adi_apollo_hal_regio_set_page_base_addr_t)    (void *desc, uint32_t address);
typedef int32_t (*adi_apollo_hal_regio_wr_reg_paged_t)          (void *desc, uint32_t address, uint8_t data);
typedef int32_t (*adi_apollo_hal_regio_wr_reg32_paged_t)        (void *desc, uint32_t address, uint32_t data);
typedef int32_t (*adi_apollo_hal_regio_stream_wr_reg_t)         (void *desc, uint32_t address, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
typedef int32_t (*adi_apollo_hal_regio_stream_wr_reg32_t)       (void *desc, uint32_t address, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
typedef int32_t (*adi_apollo_hal_regio_stream_rd_reg_t)         (void *desc, uint32_t address, uint8_t *data, uint32_t data_len_bytes, uint8_t is_cont);
typedef int32_t (*adi_apollo_hal_regio_stream_rd_reg32_t)       (void *desc, uint32_t address, uint32_t *data, uint32_t data_len_words, uint8_t is_cont);
typedef int32_t (*adi_apollo_hal_regio_manual_linkup_t)         (void *desc, uint8_t enable, uint16_t link_up_signal_bits);
typedef int32_t (*adi_apollo_hal_regio_auto_linkup_t)           (void *desc, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv);
typedef int32_t (*adi_apollo_hal_regio_alink_tbl_get_t)         (void *desc, uint16_t *hscim_alink_table);
typedef int32_t (*adi_apollo_hal_regio_rd_bf_t)                 (void *desc, uint32_t address, uint8_t *data, uint32_t data_len_bytes, uint64_t bf_mask);

/*!
 * \brief Functions that are implemented by all regio protocol types (e.g. spi, hsci)
 */
typedef struct {
    adi_apollo_hal_regio_wr_reg_t wr_reg;
    adi_apollo_hal_regio_wr_reg32_t wr_reg32;
    adi_apollo_hal_regio_rd_reg_t rd_reg;
    adi_apollo_hal_regio_rd_reg32_t rd_reg32;
    adi_apollo_hal_regio_wr_reg_rmw_t wr_reg_rmw;

    adi_apollo_hal_regio_rd_reg_poll_t rd_reg_poll;
    adi_apollo_hal_regio_rd_bf_t rd_bf;

    /* Paging ops (Apollo supports paging for writes only) */
    adi_apollo_hal_regio_set_page_base_addr_t set_page_base_addr;
    adi_apollo_hal_regio_wr_reg_paged_t wr_reg_paged;
    adi_apollo_hal_regio_wr_reg32_paged_t wr_reg32_paged;

    /* Streaming ops */
    adi_apollo_hal_regio_stream_wr_reg_t stream_wr_reg;
    adi_apollo_hal_regio_stream_wr_reg32_t stream_wr_reg32;
    adi_apollo_hal_regio_stream_rd_reg_t stream_rd_reg;
    adi_apollo_hal_regio_stream_rd_reg32_t stream_rd_reg32;

    /* Manual linkup */
    adi_apollo_hal_regio_manual_linkup_t manual_linkup;
    
    /* Auto linkup */
    adi_apollo_hal_regio_auto_linkup_t auto_linkup;
    adi_apollo_hal_regio_alink_tbl_get_t alink_tbl_get;
} adi_apollo_hal_regio_ops_t;

/*============= E X P O R T S ==============*/
#ifdef __cplusplus
extern "C" {
#endif

int32_t adi_apollo_l_hal_regio_spi_init(adi_apollo_hal_regio_spi_desc_t *desc, uint8_t is_spi1);
int32_t adi_apollo_l_hal_regio_hsci_init(adi_apollo_hal_regio_hsci_desc_t *desc);

#ifndef CLIENT_IGNORE
#endif /* CLIENT_IGNORE*/

#ifdef __cplusplus
}
#endif

#endif /* __ADI_APOLLO_HAL_REGIO_LOCAL_H__ */