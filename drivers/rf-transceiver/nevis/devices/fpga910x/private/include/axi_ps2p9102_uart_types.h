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
// Description:       AXI UART Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_UART_TYPES_H
#define AXI_PS2P9102_UART_TYPES_H

/**
 * \brief uart parameters, these must be set and config function be called,
 *  until then, all data are ignored.
 */

typedef struct axi_ps2p9102_uart_params {

    uint32_t clk_freq_hz; /*!< clock frequency in Hz */
    uint32_t baud_rate; /*!< baud rate in bits per second */
    uint8_t bit_width; /*!< bits per character 5 (0), 6 (1), 7 (2), or 8 (3). */
    uint8_t stop_bits; /*!< stop bits 1 (0), or 2 (1) */
    uint8_t parity_enable; /*!< parity enable (0x1), or disable (0x0) */
    uint8_t parity_type; /*!< parity type odd (0x0), or even (0x1) */
    uint8_t ready_polarity; /*!< polarity active low (0), or active high (1) */
    uint8_t ready_mode; /*!< hardware (0), or software (1) flow control */
    uint8_t loopback; /*!< loopback enabled (1) or disabled (0) */
    uint8_t div_mode; /*!< baud rate is a divider (1) or not (0) */

} axi_ps2p9102_uart_params_t;

#endif

// **********************************************************************************
// **********************************************************************************
