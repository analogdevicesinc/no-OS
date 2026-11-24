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
// Description:       AXI DMA Driver
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_DMA_TYPES_H
#define AXI_DMA_TYPES_H

/**
 * \brief dma parameters, triggers and flags are defined by the platform, refer to
 *  the hdl documentation for details.
 */

typedef struct axi_dma_params {

    uint64_t start_address; /*!< start address */
    uint32_t length; /*!< length, in bytes */
    uint32_t qthreshold; /*!< queue threshold, pending transactions allowed */
    uint32_t cyclic; /*!< cyclic, streaming if buffers are enabled */
    uint32_t no_of_buffers; /*!< buffer count, streaming mode */
    uint32_t start_sync; /*!< flags, hdl specific */
    uint32_t cyclic_sync; /*!< flags, hdl specific */
    uint32_t triggers[6]; /*!< triggers, hdl specific */

} axi_dma_params_t;

#endif

// **********************************************************************************
// **********************************************************************************
