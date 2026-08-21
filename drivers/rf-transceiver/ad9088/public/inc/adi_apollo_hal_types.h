/*!
 * \brief     HAL API type definition headers
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
#ifndef __ADI_APOLLO_HAL_TYPES_H__
#define __ADI_APOLLO_HAL_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"

/*============= D E F I N E S ==============*/

/* Max len for addresss supported for either SPI or HSCI transactions */
#define ADI_APOLLO_HAL_BUFFER_LEN_HSCI_ADDR     (4U)
/* Max len allocatable for user space by HAL API: 32KB + ADI_APOLLO_HAL_BUFFER_LEN_HSCI_ADDR */
#define ADI_APOLLO_HAL_BUFFER_LEN_MAX           (32768U + ADI_APOLLO_HAL_BUFFER_LEN_HSCI_ADDR)
/* Min len allocatable for user space by HAL API: 128B */
#define ADI_APOLLO_HAL_BUFFER_LEN_MIN           (128U)

/*!
 * \brief Enumerates HAL protocol selection
 */
typedef enum {
    ADI_APOLLO_HAL_PROTOCOL_SPI0 = 0,           /*!< SPI0 protocol */
    ADI_APOLLO_HAL_PROTOCOL_SPI1 = 1,           /*!< SPI1 protocol */
    ADI_APOLLO_HAL_PROTOCOL_HSCI = 2,           /*!< HSCI protocol */
} adi_apollo_hal_protocol_e;

/*!
 * \brief Enumerates HAL protocol Settings
 */
typedef enum {
    ADI_APOLLO_HAL_TXN_MODE_SINGLE = 0,     /*!< Single Transaction */
    ADI_APOLLO_HAL_TXN_MODE_STREAM = 1,     /*!< Contiguous */
    ADI_APOLLO_HAL_TXN_MODE_DISCONTIG = 2,  /*!< Discontiguous */
} adi_apollo_hal_txn_mode_e;

typedef enum {
    ADI_APOLLO_HAL_TXN_SIZE_D8 = 0,
    ADI_APOLLO_HAL_TXN_SIZE_D16 = 1,
    ADI_APOLLO_HAL_TXN_SIZE_D32 = 2,
} adi_apollo_hal_txn_size_e;

/*!
 * \brief Device transaction config structure
 */
typedef struct {
    uint8_t                     addr_len;           /*!< Interface reg address size, in bytes */
    uint8_t                     data_len;           /*!< Interface reg data size, in bytes */
    uint32_t                    stream_len;         /*!< Interface transaction stream length. Number of data words to transfer. */
    uint64_t                    mask;               /*!< Interface reg mask */
    uint8_t                     is_bf_txn;          /*!< 0 = reg txn, mask for reg. 1 = bf txn, mask for bf mask */
} adi_apollo_hal_txn_config_t;

/**
 * \brief Platform dependent hal init function
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 * \param[in]   config          Pointer to platform specific configuration struct
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 *
 * \note Not required by API
 */
typedef int32_t(*adi_apollo_hal_init_t)(void *dev_obj, void *config);

/**
 * \brief Platform dependent spi or hsci hal transfer function
 *
 * \param[in]   dev_obj     Pointer to platform specific data
 * \param[in]   in_data     Pointer to array of data to send
 * \param[out]  out_data    Pointer to array of data to receive
 * \param[in]   size_bytes  The size in bytes allocated for each of the in_data and out_data arrays.
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_xfer_t)(void *dev_obj, uint8_t *in_data, uint8_t *out_data, uint32_t size_bytes);

/**
 * \brief Platform dependent spi or hsci hal write function
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 * \param[in]   tx_data         Pointer to array of data to send
 * \param[in]   num_tx_bytes    Number of bytes in tx_data array
 * \param[in]   txn_config      Pointer to structure describing transaction \ref adi_apollo_hal_txn_config_t
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_write_t)(void *dev_obj, const uint8_t tx_data[], uint32_t num_tx_bytes, adi_apollo_hal_txn_config_t *txn_config);

/**
 * \brief Platform dependent spi or hsci hal read function
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 * \param[in]   tx_data         Pointer to array of data to send
 * \param[out]  rx_data         Pointer to array of data to receive
 * \param[in]   num_tx_rx_bytes Number of bytes for both tx_data and rx_data arrays
 * \param[in]   txn_config      Pointer to structure describing transaction \ref adi_apollo_hal_txn_config_t
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_read_t)(void *dev_obj, const uint8_t tx_data[], uint8_t rx_data[], uint32_t num_tx_rx_bytes, adi_apollo_hal_txn_config_t *txn_config);

/**
 * \brief Platform dependent spi or hsci hal polling read function
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 * \param[in]   tx_data         Pointer to array of data to send
 * \param[out]  rx_data         Pointer to array of data to receive
 * \param[in]   num_tx_rx_bytes Number of bytes for both tx_data and rx_data arrays
 * \param[in]   timeout_us      Timeout in us
 * \param[in]   reg_expect      Pointer to expected register value
 * \param[in]   reg_mask        Pointer to mask of relevant bits in register
 * \param[in]   txn_config      Pointer to structure describing transaction \ref adi_apollo_hal_txn_config_t
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_poll_read_t)(void *dev_obj, const uint8_t tx_data[], uint8_t rx_data[], uint32_t num_tx_rx_bytes,
        uint32_t timeout_us, void *reg_expect, void *reg_mask, adi_apollo_hal_txn_config_t *txn_config);


/**
 * \brief Platform dependent hsci hal manual linkup function
 *
 * \param[in]   dev_obj              Pointer to platform specific data
 * \param[in]   enable               Enable manual linkup
 * \param[in]   link_up_signal_bits  Link up signal
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_manual_linkup_t)(void *dev_obj, uint8_t enable, uint16_t link_up_signal_bits);

/**
 * \brief Platform dependent hsci hal auto linkup function
 *
 * \param[in]   dev_obj             Pointer to platform specific data
 * \param[in]   enable              Enable auto linkup
 * \param[in]   hscim_mosi_clk_inv  Set 1, inverts HSCIM's mosi_clk output clk to Apollo
 * \param[in]   hscim_miso_clk_inv  Set 1, inverts HSCIM's miso_clk input clk from Apollo
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_auto_linkup_t)(void *dev_obj, uint8_t enable, uint8_t hscim_mosi_clk_inv, uint8_t hscim_miso_clk_inv);

/**
 * \brief Platform dependent hsci hal auto link_table get function
 * \note  Retrive HSCIM ALINK_TABLE after Apollo's link_active is set
 *
 * \param[in]   dev_obj             Pointer to platform specific data
 * \param[out]  hscim_alink_table   Pointer to a variable that stores link table value from HSCI_M
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_alink_tbl_get_t)(void *dev_obj, uint16_t *hscim_alink_table);

/**
 * \brief Platform dependent hal open
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_open_t)(void *dev_obj);

/**
 * \brief Platform dependent hal close
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_close_t)(void *dev_obj);

/**
 * \brief  Platform dependent delay for specified number of microseconds.
 *
 * \param[in]   dev_obj         Pointer to platform specific data
 * \param[in]   us              Time to delay/sleep in microseconds
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_delay_us_t)(void *dev_obj, uint32_t us);

/**
 * \brief Platform dependent reset pin control function
 *
 * \param[in]   dev_obj Pointer to platform specific data
 * \param[in]   enable  Indicates the desired enable/disable reset via the ADI device RESETB pin
 *                      0: indicates RESETB pin is set HIGH
 *                      1: indicates RESETB pin is set LOW
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_reset_pin_ctrl_t)(void *dev_obj, uint8_t enable);

/**
 * \brief  Platform dependent write log message function
 *
 * \param[in]   dev_obj     Pointer to platform specific data
 * \param[in]   log_type    \ref adi_cms_log_type_e
 * \param[in]   message     Format string
 * \param[in]   argp        Variable message
 *
 * \return 0 for success
 * \return Any non-zero value indicates an error
 */
typedef int32_t(*adi_apollo_hal_log_write_t)(void *dev_obj, int32_t log_type, const char *message, va_list argp);

#endif /* __ADI_APOLLO_HAL_TYPES_H__ */

/*! @} */
