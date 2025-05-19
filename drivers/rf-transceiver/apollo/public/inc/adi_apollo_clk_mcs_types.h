/*!
 * \brief     Clock and multi chip sync definitions header
 *
 * \copyright copyright(c) 2018 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_CLK_MCS
 * @{
 */
#ifndef __ADI_APOLLO_CLK_MCS_TYPES_H__
#define __ADI_APOLLO_CLK_MCS_TYPES_H__

/*============= I N C L U D E S ============*/
#include "adi_cms_api_common.h"
#include "adi_apollo_common.h"
#include "apollo_cpu_device_profile_types.h"

/*============= D E F I N E S ==============*/

/*!
 * \brief Data converter fifo Rx:Tx clock ratio
 */
typedef enum {
    ADI_APOLLO_DATA_FIFO_CLK_RATIO_1_TO_1 = 0,       /*!< CLock ration RX:TX = 1:1 */
    ADI_APOLLO_DATA_FIFO_CLK_RATIO_1_TO_2 = 1,       /*!< CLock ration RX:TX = 1:2 */
    ADI_APOLLO_DATA_FIFO_CLK_RATIO_1_TO_4 = 2,       /*!< CLock ration RX:TX = 1:4 */
} adi_apollo_data_fifo_conv_clk_ratio_e;

/*!
 * \brief Data converter fifo mode
 */
typedef enum {
    ADI_APOLLO_DATA_FIFO_MODE_NON_DL = 0x00,        /*!< Debug mode - w/o deterministic latency */
    ADI_APOLLO_DATA_FIFO_MODE_DL = 0x01,            /*!< Normal mode w/ deterministic latency */
    ADI_APOLLO_DATA_FIFO_MODE_FORCE_ON = 0x02,      /*!< Debug mode - force on */
} adi_apollo_data_fifo_mode_e;

/*!
 * \brief Rx and Tx dig sync mask select
 */
typedef enum {
    ADI_APOLLO_SYNC_MASK_RX_TX_NONE = 0x00,         /*!< Select none */
    ADI_APOLLO_SYNC_MASK_RX_DIG_A   = 0x01,         /*!< Rx dig A */
    ADI_APOLLO_SYNC_MASK_RX_DIG_B   = 0x02,         /*!< Rx dig B */
    ADI_APOLLO_SYNC_MASK_TX_DIG_A   = 0x04,         /*!< Tx dig A */
    ADI_APOLLO_SYNC_MASK_TX_DIG_B   = 0x08,         /*!< Tx dig B */
    ADI_APOLLO_SYNC_MASK_RX_TX_ALL  = 0x0F,         /*!< Select all */
} adi_apollo_sync_mask_rxtx_e;

/*!
 * \brief JRx and JTx link sync mask select
 */
typedef enum {
    ADI_APOLLO_SYNC_MASK_RXTX_LINK_NONE = 0x00,     /*!< Select none */
    ADI_APOLLO_SYNC_MASK_JTX_A_LINK0    = 0x01,     /*!< JTx A Link 0 */
    ADI_APOLLO_SYNC_MASK_JTX_A_LINK1    = 0x02,     /*!< JTx A Link 1 */
    ADI_APOLLO_SYNC_MASK_JTX_B_LINK0    = 0x04,     /*!< JTx B Link 0 */
    ADI_APOLLO_SYNC_MASK_JTX_B_LINK1    = 0x08,     /*!< JTx B Link 1 */
    ADI_APOLLO_SYNC_MASK_JRX_A_LINK0    = 0x10,     /*!< JRx A Link 0 */
    ADI_APOLLO_SYNC_MASK_JRX_A_LINK1    = 0x20,     /*!< JRx A Link 1 */
    ADI_APOLLO_SYNC_MASK_JRX_B_LINK0    = 0x40,     /*!< JRx B Link 0 */
    ADI_APOLLO_SYNC_MASK_JRX_B_LINK1    = 0x80,     /*!< JRx B Link 1 */
    ADI_APOLLO_SYNC_MASK_RXTX_LINK_ALL  = 0xFF,     /*!< Select all */
} adi_apollo_sync_mask_rxtx_link_e;

/*!
 * \brief Loopback FIFO sync mask select
 */
typedef enum {
    ADI_APOLLO_SYNC_MASK_LPBKFIFO_NONE = 0x0,       /*!< Select none */
    ADI_APOLLO_SYNC_MASK_LPBKFIFO_A    = 0x1,       /*!< Loopback FIFO A */
    ADI_APOLLO_SYNC_MASK_LPBKFIFO_B    = 0x2,       /*!< Loopback FIFO B */
    ADI_APOLLO_SYNC_MASK_LPBKFIFO_ALL  = 0x3,       /*!< Select all */
} adi_apollo_sync_mask_lpbkfifo_e;

/*!
 * \brief Rx and Tx dig select
 */
typedef enum {
    ADI_APOLLO_RX_TX_NONE = 0x00,         /*!< Select none */
    ADI_APOLLO_RX_DIG_A   = 0x01,         /*!< Rx dig A */
    ADI_APOLLO_RX_DIG_B   = 0x02,         /*!< Rx dig B */
    ADI_APOLLO_TX_DIG_A   = 0x04,         /*!< Tx dig A */
    ADI_APOLLO_TX_DIG_B   = 0x08,         /*!< Tx dig B */
    ADI_APOLLO_RX_TX_ALL  = 0x0F,         /*!< Select all Rx and Tx */
} adi_apollo_rx_tx_dig_select_e;

/*!
 * \brief MCS trigger pin selection
 */
typedef enum {
    ADI_APOLLO_TRIG_PIN_A0      = 0,                /*!< MCS trigger Pin A0 */
    ADI_APOLLO_TRIG_PIN_A1      = 1,                /*!< MCS trigger Pin A1 */
    ADI_APOLLO_TRIG_PIN_B0      = 2,                /*!< MCS trigger Pin B0 */
    ADI_APOLLO_TRIG_PIN_B1      = 3,                /*!< MCS trigger Pin B1 */
    ADI_APOLLO_TRIG_PIN_NUM     = 4                 /*!< Number of trigger pins */
} adi_apollo_trig_pin_e;

/*!
 * \brief Clock input power detection status 
 */
typedef enum {
    ADI_APOLLO_CLK_PWR_GOOD = 0,          /*!< Clock input power is good */
    ADI_APOLLO_CLK_PWR_UNDERDRIVEN,       /*!< Clock input power is underdriven */
    ADI_APOLLO_CLK_PWR_OVERDRIVEN,        /*!< Clock input power is overdriven */
    ADI_APOLLO_CLK_PWR_UNUSED,            /*!< No reading for clock input power */
} adi_apollo_clk_input_power_status_e;

/*!
 * \brief Converter data fifo configuration.
 */
typedef struct adi_apollo_data_fifo_config {
    uint8_t                                 enable;             /*!< Data fifo enable enable */
    adi_apollo_data_fifo_mode_e             mode;               /*!< Fifo mode. \ref adi_apollo_data_fifo_mode_e */
    adi_apollo_data_fifo_conv_clk_ratio_e   conv_clk_ratio;     /*!< Rx:Tx converter clock ratio */
    uint8_t                                 lat_override;       /*!< If 1, programs lat_rd_offset and lat_wr_offset vals, else use defaults */
    uint8_t                                 lat_rd_offset;      /*!< Latency read offset value, if lat_override is 1 */
    uint8_t                                 lat_wr_offset;      /*!< Latency write offset, if lat_override is 1 */
} adi_apollo_data_fifo_config_t;

#endif /* __ADI_APOLLO_CLK_MCS_TYPES_H__ */

/*! @} */