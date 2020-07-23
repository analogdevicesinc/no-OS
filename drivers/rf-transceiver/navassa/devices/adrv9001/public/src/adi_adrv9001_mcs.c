/**
* \file
* \brief FPGA9001 Multi-Chip Synchronization (MCS) functions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "adi_adrv9001_user.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_spi.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf.h"

static int32_t adi_adrv9001_Mcs_Prepare_Validate(adi_adrv9001_Device_t *device, adi_adrv9001_Pll_e pll)
{
    ADI_RANGE_CHECK(device, pll, ADI_ADRV9001_PLL_LO1, ADI_ADRV9001_PLL_CLK_LP);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Prepare(adi_adrv9001_Device_t *device, adi_adrv9001_Pll_e pll)
{
    static const adrv9001_BfNvsPllMemMap_e instances[] = {
        ADRV9001_BF_RF1_PLL,
        ADRV9001_BF_RF2_PLL,
        ADRV9001_BF_AUX_PLL,
        ADRV9001_BF_CLK_PLL,
        ADRV9001_BF_CLK_PLL_LP
    };
    adrv9001_BfNvsPllMemMap_e instance = ADRV9001_BF_CLK_PLL;

    static const uint8_t SYSREF_BUFFER_ENABLE         = 0x40;
    static const uint8_t SYSREF_SAMPLE_ENABLE         = 0x10;
    static const uint8_t SYSREF_REGISTER_ENABLE       = 0x08;
    static const uint8_t MCS_CMOS_MODE_ENABLE         = 0x80;
    static const uint8_t MCS_CONTROL_SET              = 0x3F;
    static const uint8_t MCS_CONTROL_4_SET            = 0x00;
    static const uint8_t MCS_CONTROL_5_SET            = 0x0C;
    static const uint8_t CLKGEN_RESET                 = 0x01;
    static const uint8_t DEVICE_CLK_CONTROL_1         = 0x12;
    static const uint8_t ANALOG_SPARE_6               = 0x0F;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Mcs_Prepare_Validate, device, pll);
    
    instance = instances[pll];

    /*
     *SPI:
     *  core_bf.sysref_register_enable = 1'b1
        core_bf.sysref_sample_enable = 1
        core_bf.sysref_buffer_enable = 1
     */
    ADRV9001_SPIWRITEBYTE(device,
                          "SYSREF_CONTROL_1",
                          ADRV9001_ADDR_SYSREF_CONTROL_1,
                          (SYSREF_BUFFER_ENABLE | SYSREF_SAMPLE_ENABLE | SYSREF_REGISTER_ENABLE));

    /*
        core_bf.dig_device_clk_buffer_enable = 1
        core_bf.dig_sysref_sample_enable = 1
     */
    ADRV9001_SPIWRITEBYTE(device,
                          "ANALOG_CONTROL_18",
                          ADRV9001_ADDR_ANALOG_CONTROL_18,
                          (ANALOG_CONTROL_18_RESET | DIG_SYSREF_SAMPLE_ENABLE | DIG_DEVICE_CLK_BUFFER_ENABLE));

    /*
        core_1_bf.mcs_cmos_mode_enable
        clk_pll_bf.mcs_device_clk_divider_sync_enable = 1
        clk_pll_bf.mcs_sdm_sync_enable = 1
        clk_pll_bf.mcs_clkgen_sync_enable = 1
        clk_pll_bf.mcs_digital_clock_sync_enable = 1
        clk_pll_bf.mcs_jesd_sysref_enable = 1
        clk_pll_bf.mcs_enable = 1
    */
    ADRV9001_SPIWRITEBYTE(device, "MCS_CMOS_MODE_ENABLE", ADRV9001_ADDR_MCS_REG0, MCS_CMOS_MODE_ENABLE);

    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL", instance + ADRV9001_ADDR_MCS_CONTROL_OFFSET, MCS_CONTROL_SET);

    /*
        clk_pll_bf.mcs_serdes_align = 2'b00
        clk_pll_bf.clkgen_mcs2dig_count = x00C
    */
    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_4", instance + ADRV9001_ADDR_MCS_CONTROL_4_OFFSET, MCS_CONTROL_4_SET);

    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_5", instance + ADRV9001_ADDR_MCS_CONTROL_5_OFFSET, MCS_CONTROL_5_SET);

    /*
        clk_pll_bf.clkgen_use_backup_reset = 0
        core_bf.devclk_divider_mcs_resetb = 1
        core_1_bf.analog_spare_6= 0x0F
    */
    ADRV9001_SPIWRITEBYTE(device, "CLKGEN_RESET", instance + ADRV9001_ADDR_CLKGEN_RESET_OFFSET, CLKGEN_RESET);

    ADRV9001_SPIWRITEBYTE(device, "DEVICE_CLK_CONTROL_1", ADRV9001_ADDR_DEVICE_CLK_CONTROL_1, DEVICE_CLK_CONTROL_1);

    ADRV9001_SPIWRITEBYTE(device, "ANALOG_SPARE_6", ADRV9001_ADDR_ANALOG_SPARE_6, ANALOG_SPARE_6);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_PrepareArm(adi_adrv9001_Device_t *device)
{
    ADI_EXPECT(adi_adrv9001_arm_Cmd_Write, device, ADRV9001_ARM_MCS_OPCODE, NULL, 0);

    /* Wait for command to finish executing */
    ADRV9001_ARM_CMD_STATUS_WAIT_EXPECT(device,
                                        ADRV9001_ARM_MCS_OPCODE,
                                        0,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_TIMEOUT_US,
                                        (uint32_t)ADI_ADRV9001_DEFAULT_INTERVAL_US);

    ADI_API_RETURN(device);
}

static int32_t adi_adrv9001_Mcs_AnalogStatus_Get_Validate(adi_adrv9001_Device_t *device, adi_adrv9001_Pll_e pll, uint32_t *mcsStatus)
{
    ADI_RANGE_CHECK(device, pll, ADI_ADRV9001_PLL_LO1, ADI_ADRV9001_PLL_CLK_LP);
    ADI_NULL_PTR_RETURN(&device->common, mcsStatus);
    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_AnalogStatus_Get(adi_adrv9001_Device_t *device, adi_adrv9001_Pll_e pll, uint32_t *mcsStatus)
{
    uint8_t mcsStatusRead = 0;
    static const adrv9001_BfNvsPllMemMap_e instances[] = {
        ADRV9001_BF_RF1_PLL,
        ADRV9001_BF_RF2_PLL,
        ADRV9001_BF_AUX_PLL,
        ADRV9001_BF_CLK_PLL,
        ADRV9001_BF_CLK_PLL_LP
    };
    adrv9001_BfNvsPllMemMap_e instance = ADRV9001_BF_CLK_PLL;

    ADI_PERFORM_VALIDATION(adi_adrv9001_Mcs_AnalogStatus_Get_Validate, device, pll, mcsStatus);

    instance = instances[pll];
    
    ADRV9001_SPIREADBYTE(device, "MCS_CONTROL_3", instance + ADRV9001_ADDR_MCS_CONTROL_3_OFFSET, &mcsStatusRead);

    *mcsStatus = (uint32_t)mcsStatusRead;

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Digital_Reset(adi_adrv9001_Device_t *device)
{
    uint8_t mcsClearRegisterRead = 0;
    uint8_t mcsClearBitValue = 0;

    static const uint8_t MCS_CAPTURE_RECEIVED_CLEAR        = 0x01;
    static const uint8_t MCS_SECOND_CAPTURE_RECEIVED_CLEAR = 0x02;
    static const uint8_t RX1_SECOND_CAPTURE_RECEIVED_CLEAR = 0x10;
    static const uint8_t RX2_SECOND_CAPTURE_RECEIVED_CLEAR = 0x20;

    ADI_API_ENTRY_EXPECT(device);

    mcsClearBitValue = (RX2_SECOND_CAPTURE_RECEIVED_CLEAR | RX1_SECOND_CAPTURE_RECEIVED_CLEAR |
                        MCS_SECOND_CAPTURE_RECEIVED_CLEAR | MCS_CAPTURE_RECEIVED_CLEAR);

    /* Read SW interrupt 4 register */
    ADRV9001_SPIREADBYTE(device, "MCS_CLEAR_0", ADRV9001_ADDR_MCS_CLEAR_0, &mcsClearRegisterRead);

    /* Set '1' in the following bit positions in MCS_CLEAR_0 register to clear
       1. MCS_CAPTURE_RECEIVED_CLEAR
       2. MCS_SECOND_CAPTURE_RECEIVED_CLEAR
       3. RX1_SECOND_CAPTURE_RECEIVED_CLEAR
       4. RX2_SECOND_CAPTURE_RECEIVED_CLEAR
    */
    ADRV9001_SPIWRITEBYTE(device, "MCS_CLEAR_0", ADRV9001_ADDR_MCS_CLEAR_0, (mcsClearRegisterRead | mcsClearBitValue));

    /* Read SW interrupt 4 register */
    ADRV9001_SPIREADBYTE(device, "MCS_CLEAR_0", ADRV9001_ADDR_MCS_CLEAR_0, &mcsClearRegisterRead);

    /* Clear all bits that are set to '1' to restore back */
    ADRV9001_SPIWRITEBYTE(device, "MCS_CLEAR_0", ADRV9001_ADDR_MCS_CLEAR_0, mcsClearRegisterRead & ~mcsClearBitValue);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Internal_Select(adi_adrv9001_Device_t *device, bool selectInternal)
{
    uint8_t mcsCtrlReg0 = 0;

    static const uint8_t MCS_INTERNAL_GEN_SEL         = 0x01;

    ADRV9001_SPIREADBYTE(device, "MCS_CONTROL_0", ADRV9001_ADDR_MCS_CONTROL_0, &mcsCtrlReg0);

    if (selectInternal)
    {
        mcsCtrlReg0 = mcsCtrlReg0 | MCS_INTERNAL_GEN_SEL;
    }
    else
    {
        mcsCtrlReg0 = mcsCtrlReg0 & ~MCS_INTERNAL_GEN_SEL;
    }

    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_0", ADRV9001_ADDR_MCS_CONTROL_0, mcsCtrlReg0);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Capture_Enable(adi_adrv9001_Device_t *device)
{
    uint8_t mcsCtrlReg1 = 0;

    static const uint8_t MCS_CAPTURE_ENABLE           = 0x01;
    static const uint8_t MCS_INTERNAL_GEN             = 0x02;

    /* TODO (JS): Evaluate if this can just be done via _Set functions
    ADI_EXPECT(adrv9001_NvsRegmapCoreMcsCaptureEnable_Set, device, 1);
    */
    ADRV9001_SPIREADBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, &mcsCtrlReg1);
    mcsCtrlReg1 = (mcsCtrlReg1 & (~MCS_INTERNAL_GEN)) | MCS_CAPTURE_ENABLE;
    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, mcsCtrlReg1);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Internal_Sync(adi_adrv9001_Device_t *device)
{
    uint8_t mcsCtrlReg1 = 0;

    static const uint8_t MCS_INTERNAL_GEN             = 0x02;

    /* TODO (JS): Evaluate if this can just be done via _Set functions
    ADI_EXPECT(adrv9001_NvsRegmapCoreMcsInternalGen_Set, device, 1);
    ADI_EXPECT(adrv9001_NvsRegmapCoreMcsInternalGen_Set, device, 0);
    */
    /* Set MCS_INTERNAL_GEN field */
    ADRV9001_SPIREADBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, &mcsCtrlReg1);
    mcsCtrlReg1 = mcsCtrlReg1 | MCS_INTERNAL_GEN;
    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, mcsCtrlReg1);

    /* Clear MCS_INTERNAL_GEN field */
    ADRV9001_SPIREADBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, &mcsCtrlReg1);
    mcsCtrlReg1 = (mcsCtrlReg1 & (~MCS_INTERNAL_GEN));
    ADRV9001_SPIWRITEBYTE(device, "MCS_CONTROL_1", ADRV9001_ADDR_MCS_CONTROL_1, mcsCtrlReg1);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_DigitalInt_Set(adi_adrv9001_Device_t *device, uint8_t numberOfPulses)
{
    uint8_t i = 0;

    ADI_API_ENTRY_EXPECT(device);

    ADI_RANGE_CHECK(device, numberOfPulses, 1, 2);

    ADI_EXPECT(adi_adrv9001_Mcs_Internal_Select, device, true);
    ADI_EXPECT(adi_adrv9001_Mcs_Capture_Enable, device);


    for (i = 0; i < numberOfPulses; i++)
    {
        ADI_EXPECT(adi_adrv9001_Mcs_Internal_Sync, device);
    }

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_DigitalStatus_Get(adi_adrv9001_Device_t *device, uint32_t *mcsStatus)
{
    uint8_t mcsStatusRead = 0;

    ADI_API_ENTRY_PTR_EXPECT(device, mcsStatus);

    ADRV9001_SPIREADBYTE(device, "MCS_STATUS_0", ADRV9001_ADDR_MCS_STATUS_0, &mcsStatusRead);

    *mcsStatus = (uint32_t)mcsStatusRead;
    ADI_API_RETURN(device);
}

static int32_t __maybe_unused Mcs_Execute_Validate(adi_adrv9001_Device_t *device, adi_adrv9001_McsAction_e mcsAction)
{
    ADI_RANGE_CHECK(device, mcsAction, ADI_ADRV9001_MCS_INT_DIGITAL_ALL, ADI_ADRV9001_MCS_EXT_ANALOG_3);

    ADI_API_RETURN(device);
}

int32_t adi_adrv9001_Mcs_Execute(adi_adrv9001_Device_t *device, adi_adrv9001_McsAction_e mcsAction, bool bbicInitiated)
{
    uint8_t regVal = 0;
    int32_t halError = ADI_COMMON_ACT_NO_ACTION;
    adrv9001_BfNvsPllMemMap_e baseAddr = ADRV9001_BF_CLK_PLL_LP;
    
    static const uint8_t ANALOG_DEVCLK_REFCLK_DIV_SYNCED = 0x10;
    static const uint8_t ANALOG_SDM_SYNCED = 0x08;
    static const uint8_t ANALOG_CLKGEN_SYNCED = 0x04;

    ADI_PERFORM_VALIDATION(Mcs_Execute_Validate, device, mcsAction);

    if (ADI_ADRV9001_CLK_PLL_HP_MODE == device->devStateInfo.clkPllMode)
    {
        baseAddr = ADRV9001_BF_CLK_PLL;
    }

    if (true == bbicInitiated)
    {
        /* Prepare ADRV9001 to receive MCS and wait until ready */
        ADI_EXPECT(adi_adrv9001_Mcs_PrepareArm, device);
        halError = adi_common_hal_Wait_us(&device->common, ADI_ADRV9001_READY_FOR_MCS_DELAY_US);
        ADI_ERROR_REPORT(&device->common,
                            ADI_ADRV9001_SRC_ARMCMD,
                            halError,
                            ADI_COMMON_ACT_ERR_CHECK_TIMER,
                            NULL,
                            "Timer not working");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_EXPECT(adi_adrv9001_arm_Disable, device);

    switch (mcsAction)
    {
    case ADI_ADRV9001_MCS_INT_DIGITAL_ALL:
        ADI_EXPECT(adi_adrv9001_Mcs_Digital_Reset, device);
        ADI_EXPECT(adi_adrv9001_Mcs_DigitalInt_Set, device, 2);
        break;
    case ADI_ADRV9001_MCS_INT_DIGITAL_CLK:
        ADI_EXPECT(adi_adrv9001_Mcs_Digital_Reset, device);
        ADI_EXPECT(adi_adrv9001_Mcs_DigitalInt_Set, device, 1);
        break;
    case ADI_ADRV9001_MCS_INT_DIGITAL_SSI:
        ADI_EXPECT(adi_adrv9001_Mcs_DigitalInt_Set, device, 1);
        break;
    case ADI_ADRV9001_MCS_EXT_DIGITAL_ALL:
        ADI_EXPECT(adi_adrv9001_Mcs_Digital_Reset, device);
        ADI_EXPECT(adi_adrv9001_Mcs_Internal_Select, device, false);
        ADI_EXPECT(adi_adrv9001_Mcs_Capture_Enable, device);
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 2);
        break;
    case ADI_ADRV9001_MCS_EXT_DIGITAL_CLK:
        ADI_EXPECT(adi_adrv9001_Mcs_Digital_Reset, device);
        ADI_EXPECT(adi_adrv9001_Mcs_Internal_Select, device, false);
        ADI_EXPECT(adi_adrv9001_Mcs_Capture_Enable, device);
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 1);
        break;
    case ADI_ADRV9001_MCS_EXT_DIGITAL_SSI:
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 1);
        break;
    case ADI_ADRV9001_MCS_EXT_ANALOG_ALL:
        ADI_EXPECT(adi_adrv9001_Mcs_Prepare, device, ADI_ADRV9001_PLL_CLK);
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 4);
        ADI_ERROR_REPORT(&device->common, 
                         ADI_COMMON_ERR_API_FAIL, 
                         halError, 
                         ADI_COMMON_ACT_ERR_RESET_FULL, 
                         NULL,
                         "Error occurred in adi_hal_Mcs_Pulse");
        ADI_ERROR_RETURN(device->common.error.newAction);
        ADI_EXPECT(adrv9001_NvsPllMemMap_McsSpiStatus_Get, device, baseAddr, &regVal);
        ADI_EXPECT(adrv9001_NvsPllMemMap_ClkgenUseSerdesMcs_Set, device, baseAddr, 1);
        break;
    case ADI_ADRV9001_MCS_EXT_ANALOG_1:
        ADI_EXPECT(adi_adrv9001_Mcs_Prepare, device, ADI_ADRV9001_PLL_CLK);
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 2);
        ADI_EXPECT(adrv9001_NvsPllMemMap_McsSpiStatus_Get, device, baseAddr, &regVal);
        if ((regVal & ANALOG_DEVCLK_REFCLK_DIV_SYNCED) != ANALOG_DEVCLK_REFCLK_DIV_SYNCED)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             NULL,
                             "Analog MCS 1 failed");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    case ADI_ADRV9001_MCS_EXT_ANALOG_2:
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 1);
        ADI_EXPECT(adrv9001_NvsPllMemMap_McsSpiStatus_Get, device, baseAddr, &regVal);
        if ((regVal & ANALOG_SDM_SYNCED) != ANALOG_SDM_SYNCED)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             NULL,
                             "Analog MCS 2 failed");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        break;
    case ADI_ADRV9001_MCS_EXT_ANALOG_3:
        halError = adi_hal_Mcs_Pulse(device->common.devHalInfo, 1);
        ADI_EXPECT(adrv9001_NvsPllMemMap_McsSpiStatus_Get, device, baseAddr, &regVal);
        if ((regVal & ANALOG_CLKGEN_SYNCED) != ANALOG_CLKGEN_SYNCED)
        {
            ADI_ERROR_REPORT(&device->common,
                             ADI_COMMON_ERRSRC_API,
                             ADI_COMMON_ERR_API_FAIL,
                             ADI_COMMON_ACT_ERR_RESET_FULL,
                             NULL,
                             "Analog MCS 3 failed");
            ADI_ERROR_RETURN(device->common.error.newAction);
        }
        ADI_EXPECT(adrv9001_NvsPllMemMap_ClkgenUseSerdesMcs_Set, device, baseAddr, 1);
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_RESET_FEATURE,
                         NULL,
                         "ADRV9001 did not request a known MCS action");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERR_API_FAIL,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_FULL,
                     NULL,
                     "Error occurred in adi_hal_Mcs_Pulse");
    ADI_ERROR_RETURN(device->common.error.newAction);

    halError = adi_hal_ssi_Reset(device->common.devHalInfo);
    ADI_ERROR_REPORT(&device->common,
                     ADI_COMMON_ERRSRC_DEVICEHAL,
                     halError,
                     ADI_COMMON_ACT_ERR_RESET_FEATURE,
                     NULL,
                     "Error occurred in adi_hal_ssi_Reset");

    ADI_EXPECT(adi_adrv9001_arm_Enable, device);

    ADI_API_RETURN(device);
}