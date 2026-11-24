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

#ifndef AXI_MSPI_TYPES_H
#define AXI_MSPI_TYPES_H

/**
 * \brief spi parameters, these must be set and config function be called before a spi
 *  transfer can begin. the clock init and miso sample edge are loosely equivalent to
 *  polarity and phase, with the exception that mosi is always driven aligned with clock.
 *  so phase setting affects the mosi sampling edge at the slave.
 */

typedef struct axi_mspi_params {

    uint32_t clk_period; /*!< clock period, 50/(period+1)MHz. */
    uint32_t clk_init; /*!< inactive/initial level of clock. */
    uint32_t bidir_enable; /*!< sdio mode. */
    uint32_t bidir_offset; /*!< sdio offset, address phase */
    uint32_t miso_sample_edge; /*!< miso sampling edge */

} axi_mspi_params_t;

#endif

// **********************************************************************************
// **********************************************************************************
