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
// Description:       AXI SSPI Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_PS2P9102_SSPI_TYPES_H
#define AXI_PS2P9102_SSPI_TYPES_H

/**
 * \brief gpio based slave spi parameters, these controls how spi miso/mosi data is
 *  sampled. data is sampled at a selected edge of the spi clock.
 */

typedef struct axi_ps2p9102_sspi_params {

    uint32_t  mosi_edge; /*!< edge of spi clock to sample mosi data */
    uint32_t  miso_edge; /*!< edge of spi clock to drive miso data */
    uint32_t  bidir_enable; /*!< if set, mosi is used to drive miso data */
    uint32_t  bidir_count; /*!< number of bytes after which miso is driven on mosi data */

} axi_ps2p9102_sspi_params_t;

#endif

// **********************************************************************************
// **********************************************************************************
