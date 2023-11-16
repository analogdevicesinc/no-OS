/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_axi_adv_spi.h
* \brief Contains top level fpga related function prototypes for
*        adi_fpga.c
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_AXI_ADV_SPI_H_
#define _ADI_FPGAGEN6_AXI_ADV_SPI_H_

#include "adi_fpgagen6_axi_adv_spi_types.h"
#include "adi_fpgagen6_error.h"

/**
* \brief Configures the SPI module for the selected slave select.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[in] advSpiCfg Pointer to structure holding the SPI configuration to be set
* \param[in] slaveSelect SPI Slave select to be configured
* \param[in] masterSelect SPI Master select in the FPGA to be configured
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_AxiAdvSpiCfgSet(  adi_fpgagen6_Device_t* const                    device,
                                                                const adi_fpgagen6_AxiAdvSpiFeatConfig_t* const advSpiCfg,
                                                                const uint8_t                                   slaveSelect,
                                                                const uint8_t                                   masterSelect);

/**
* \brief Reads back the SPI module settings for the selected slave select.
*
* \dep_begin
* \dep{device}
* \dep_end
*
* \param[in,out] device Context variable - device Pointer to the FPGA device data structure
* \param[out] advSpiCfg Pointer to structure that will hold the returned sSPI configuration
* \param[in] slaveSelect SPI Slave select for which to readback the configuration
* \param[in] masterSelect SPI Master select for which to readback the configuration
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_AxiAdvSpiCfgGet(  adi_fpgagen6_Device_t* const                    device,
                                                                adi_fpgagen6_AxiAdvSpiFeatConfig_t* const       advSpiCfg,
                                                                const uint8_t                                   slaveSelect,
                                                                const uint8_t                                   masterSelect);

#endif  /* _ADI_FPGAGEN6_AXI_ADV_SPI_H_ */
