// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Description:       AXI SPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_UART_H
#define AXI_PS2P9102_UART_H

#include "axi_common.h"
#include "axi_ps2p9102_top.h"
#include "axi_ps2p9102_uart_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

/**
 * \brief config, common parameters
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] uart_param parameters
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_uart_config(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    axi_ps2p9102_uart_params_t *uart_param);

/**
 * \brief transmit data set
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_uart_tx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint8_t data);

/**
 * \brief transmit data (managed function), it is advised that the system use
 * this function to send 16 or less bytes. a call for a higher count may
 * result in buffer management by the calling party, as the uart may not be
 * able to send all the bytes in the default timeout interval.
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[in] txdata bytes to send
 * \param[in] count number of bytes to send
 * \param[out] send_count actual number of bytes sent
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_uart_send(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t txdata[],
    uint32_t count, uint32_t *send_count);

/**
 * \brief receive data get
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] data register value
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_uart_rx_data(AXI_DEVICE_T *a_device, uint32_t peripheral_id,
    uint8_t *data);

/**
 * \brief receive data (managed function).
 *
 * \param[in] a_device context variable - pointer to axi bus master device
 * \param[in] peripheral_id of this instance
 * \param[out] rxdata bytes to receive
 * \param[in] count number of bytes to collect
 * \param[out] recv_count actual number of bytes received
 * \param[out] recv_status status of bytes received
 * \returns non zero value to indicate bus errors from axi bus master
 */

int32_t axi_ps2p9102_uart_recv(AXI_DEVICE_T *a_device, uint32_t peripheral_id, uint8_t rxdata[],
    uint32_t count, uint32_t *recv_count, uint32_t *recv_status);

// **********************************************************************************
// **********************************************************************************

#ifdef __cplusplus
}
#endif

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************
