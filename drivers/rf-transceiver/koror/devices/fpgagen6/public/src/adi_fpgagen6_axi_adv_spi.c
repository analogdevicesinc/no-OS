/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_axi_adv_spi.c
* \brief Contains FPGA AXI Advanced SPI function implementations
*
* FPGA API Version: 2.9.0.4
*/

#include "adi_fpgagen6_axi_adv_spi.h"
#include "adi_fpgagen6_hal.h"
#include "adi_platform.h"

#include "../../private/bf/fpgagen6_bf_axi_adv_spi_types.h"
#include "../../private/include/fpgagen6_data_interface.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_AXI_ADV_SPI

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_AxiAdvSpiCfgSet   (adi_fpgagen6_Device_t* const                   device,
                                                                const adi_fpgagen6_AxiAdvSpiFeatConfig_t* const advSpiCfg,
                                                                const uint8_t                                   slaveSelect,
                                                                const uint8_t                                   masterSelect)
{
    const uint32_t ADVSPI_REG_OFFSET = 0x80U;
    
    adi_fpgagen6_ErrAction_e        recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e                   halError            = ADI_HAL_ERR_PARAM;
    fpgagen6_BfAxiAdvSpiChanAddr_e  baseAddr            = FPGAGEN6_BF_ADS10_TOP_SPI;
    uint32_t                        registerValue       = 0U;
    uint32_t                        registerAddress     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, advSpiCfg, cleanup);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    /* Select the correct base address for SPI vs. SPI2 master */
    if (masterSelect == 0U)
    {
        baseAddr = FPGAGEN6_BF_ADS10_TOP_SPI;
    }
    else if (masterSelect == 1U)
    {
        baseAddr = FPGAGEN6_BF_ADS10_TOP_SPI2;
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            masterSelect,
            "FPGA only has 2 SPI masters. masterSelect must be 0 or 1.");
    }
    
    /* starting address is slaveSelect0's address. Multiply parameter by 4 since it's byte-addressing and each SS gets a full register */
    registerAddress = baseAddr + ADVSPI_REG_OFFSET + (slaveSelect * 4U);

    /* Build up the registerValue */
    registerValue |= ((uint32_t)advSpiCfg->enable & 0x1U) << 0U;
    registerValue |= ((uint32_t)advSpiCfg->cpol & 0x1U) << 1U;
    registerValue |= ((uint32_t)advSpiCfg->cpha & 0x1U) << 2U;
    registerValue |= ((uint32_t)advSpiCfg->lsbFirst & 0x1U) << 3U;
    registerValue |= ((uint32_t)advSpiCfg->fourWire & 0x1U) << 4U;
    registerValue |= ((uint32_t)advSpiCfg->misoSampleSlip & 0x3U) << 5U;
    registerValue |= ((uint32_t)advSpiCfg->threeWireTriPoint & 0xFFU) << 8U;
    registerValue |= ((uint32_t)advSpiCfg->clockDiv & 0xFFFFU) << 16U;

    halError = adi_hal_BbicRegisterWrite(   device->common.devHalInfo,
                                            registerAddress,
                                            registerValue);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   &device->common,
                            ADI_FPGAGEN6_ERRSRC_DEVICEHAL,
                            halError,
                            recoveryAction,
                            registerAddress,
                            "RAM Write Issue Axi Adv Spi");
        goto cleanup;
    }

cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_AxiAdvSpiCfgGet   (adi_fpgagen6_Device_t* const                   device,
                                                                adi_fpgagen6_AxiAdvSpiFeatConfig_t* const       advSpiCfg,
                                                                const uint8_t                                   slaveSelect,
                                                                const uint8_t                                   masterSelect)
{
    const uint32_t ADVSPI_REG_OFFSET = 0x80U;
    
    adi_fpgagen6_ErrAction_e        recoveryAction      = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_hal_Err_e                   halError            = ADI_HAL_ERR_PARAM;
    fpgagen6_BfAxiAdvSpiChanAddr_e  baseAddr            = FPGAGEN6_BF_ADS10_TOP_SPI;
    uint32_t                        registerValue       = 0U;
    uint32_t                        registerAddress     = 0U;

    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_NULL_PTR_REPORT_GOTO(&device->common, advSpiCfg, cleanup);

    ADI_FPGAGEN6_API_ENTRY(&device->common);
    
    /* Select the correct base address for SPI vs. SPI2 master */
    if (masterSelect == 0U)
    {
        baseAddr = FPGAGEN6_BF_ADS10_TOP_SPI;
    }
    else if (masterSelect == 1U)
    {
        baseAddr = FPGAGEN6_BF_ADS10_TOP_SPI2;
    }
    else
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                masterSelect,
                                "FPGA only has 2 SPI masters. masterSelect must be 0 or 1.");
    }
    
    /* starting address is slaveSelect0's address. Multiply parameter by 4 since it's byte-addressing and each SS gets a full register */
    registerAddress = baseAddr + ADVSPI_REG_OFFSET + (slaveSelect * 4U);
    
    /* Readback register */
    halError = adi_hal_BbicRegisterRead(   device->common.devHalInfo,
                                           registerAddress,
                                          &registerValue);
    if (ADI_HAL_ERR_OK == halError)
    {
        recoveryAction = ADI_FPGAGEN6_ERR_ACT_NONE;
    }
    else
    {
        recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_hal_ErrCodeConvert(halError);
        ADI_ERROR_REPORT(   &device->common,
                            ADI_FPGAGEN6_ERRSRC_DEVICEHAL,
                            halError,
                            recoveryAction,
                            registerAddress,
                            "RAM Read Issue Axi Adv Spi");
        goto cleanup;
    }
    
    /* Build up output structure */
    advSpiCfg->enable               = (registerValue >> 0U)  & 0x1U;
    advSpiCfg->cpol                 = (registerValue >> 1U)  & 0x1U;
    advSpiCfg->cpha                 = (registerValue >> 2U)  & 0x1U;
    advSpiCfg->lsbFirst             = (registerValue >> 3U)  & 0x1U;
    advSpiCfg->fourWire             = (registerValue >> 4U)  & 0x1U;
    advSpiCfg->misoSampleSlip       = (registerValue >> 5U)  & 0x3U;
    advSpiCfg->threeWireTriPoint    = (registerValue >> 8U)  & 0xFFU;
    advSpiCfg->clockDiv             = (registerValue >> 16U) & 0xFFFFU;
    
cleanup :
    ADI_FPGAGEN6_API_EXIT(&device->common, recoveryAction);
}