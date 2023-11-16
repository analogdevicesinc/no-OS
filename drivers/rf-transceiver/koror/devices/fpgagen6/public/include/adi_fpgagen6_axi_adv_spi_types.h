/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_axi_adv_spi_types.h
* \brief Contains top level fpga related type definitions for
*        adi_fpgagen6_axi_adv_spi.h
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_AXI_ADV_SPI_TYPES_H_
#define _ADI_FPGAGEN6_AXI_ADV_SPI_TYPES_H_

#include "adi_library_types.h"

typedef struct adi_fpgagen6_AxiAdvSpiFeatConfig
{
    uint16_t clockDiv;          /*!< Advanced Feature Set Clock Divider.*/
    uint8_t  threeWireTriPoint; /*!< Advanced Feature Set Three Wire Tristate Point.*/
    uint8_t  misoSampleSlip;    /*!< Advanced Feature Set MISO Sample Slip.*/
    uint8_t  fourWire;          /*!< Advanced Feature Set Four Wire Mode.*/
    uint8_t  lsbFirst;          /*!< Advanced Feature Set LSB First.*/
    uint8_t  cpha;              /*!< Advanced Feature Set Clock Phase.*/
    uint8_t  cpol;              /*!< Advanced Feature Set Clock Polarity.*/
    uint8_t  enable;            /*!< Advanced Feature Set Enable.*/
} adi_fpgagen6_AxiAdvSpiFeatConfig_t;

#endif /* _ADI_FPGAGEN6_AXI_ADV_SPI_TYPES_H_ */
