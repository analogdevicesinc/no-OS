// SPDX-License-Identifier: GPL-2.0
/**
 * \file adrv9025_init.c
 * \brief Contains ADRV9025 init related private function implementations
 *
 * ADRV9025 API Version: 6.4.0.14
 */

/**
* Copyright 2015 - 2020 Analog Devices Inc.
* Released under the ADRV9025 API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

#include "adi_adrv9025_user.h"
#include "adi_adrv9025.h"
#include "adi_adrv9025_hal.h"
#include "adi_adrv9025_error.h"
#include "../devices/adrv9025/private/include/adrv9025_init.h"
#include "../devices/adrv9025/private/include/adrv9025_rx.h"
#include "../devices/adrv9025/private/include/adrv9025_reg_addr_macros.h"
#include "../devices/adrv9025/private/include/adrv9025_cpu_macros.h"

#include "no_os_util.h"

#line __LINE__ "adrv9025_init.c"

int32_t adrv9025_LdoEnable(adi_adrv9025_Device_t* device,
                           uint8_t                ldoSelect)
{
    static const uint8_t NONBYPASS_DEVCLK_LDO        = 0x04;
    static const uint8_t BYPASS_DEVCLK_LDO           = 0x84;
    static const uint8_t BYPASS_DEVCLK_LDO_VOUT_TRIM = 0x00;
    static const uint8_t NONBYPASS_GP_LDO_1P0V_OUT   = 0x04;

    int32_t recoveryAction = ADI_COMMON_ACT_NO_ACTION;

    /* If LDO Select = 1, set LDO's to BYPASS */
    if (ldoSelect == 1)
    {
        ADRV9025_SPIWRITEBYTE("DEV_CLK_LDO_BYTE1",
                              ADRV9025_ADDR_DEV_CLK_LDO_BYTE1,
                              BYPASS_DEVCLK_LDO);

        /* write the LDO Vout Trim to allow a lower reference voltage to the amp */
        ADRV9025_SPIWRITEBYTE("DEV_CLK_LDO_BYTE2",
                              ADRV9025_ADDR_DEV_CLK_LDO_BYTE2,
                              BYPASS_DEVCLK_LDO_VOUT_TRIM);

        /* Enable GP_LDO_BYTE1N (non bypass) */
        ADRV9025_SPIWRITEBYTE("GP_LDO_BYTE1N",
                              ADRV9025_ADDR_GP_LDO_BYTE1N,
                              NONBYPASS_GP_LDO_1P0V_OUT);
        ADRV9025_SPIWRITEBYTE("GP_LDO_BYTE1N+1",
                              (ADRV9025_ADDR_GP_LDO_BYTE1N + 1),
                              NONBYPASS_GP_LDO_1P0V_OUT);

        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    else if (ldoSelect == 0) /* If LDO Select = 0, set LDO's to NON-BYPASS */
    {
        ADRV9025_SPIWRITEBYTE("DEV_CLK_LDO_BYTE1",
                              ADRV9025_ADDR_DEV_CLK_LDO_BYTE1,
                              NONBYPASS_DEVCLK_LDO);

        /* Set GP_LDO_BYTE1N non-bypass */
        ADRV9025_SPIWRITEBYTE("GP_LDO_BYTE1N",
                              ADRV9025_ADDR_GP_LDO_BYTE1N,
                              NONBYPASS_GP_LDO_1P0V_OUT);
        ADRV9025_SPIWRITEBYTE("GP_LDO_BYTE1N+1",
                              (ADRV9025_ADDR_GP_LDO_BYTE1N + 1),
                              NONBYPASS_GP_LDO_1P0V_OUT);

        /* Allow time for internal LDOs to power up */


        ADI_ERROR_RETURN(device->common.error.newAction);

    }
    else /* Report Error if LDO Select is not either 0 or 1 */
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid LDO Configuration Specified");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    recoveryAction = adi_common_hal_Wait_ms(&device->common, 200);
    if (recoveryAction != ADI_COMMON_ACT_NO_ACTION)
    {
        return (recoveryAction);
    }

    return (device->common.error.newAction);
}

uint32_t adrv9025_ClockDiv(uint32_t clock,
                           uint32_t clock_max)
{
    uint32_t              div                 = 0;
    static const uint32_t ADRV9025_POWER2_MAX = 8;

    for (div = 0; div < ADRV9025_POWER2_MAX; div++)
    {
        if (clock <= (clock_max << div))
        {
            return div;
        }
    }

    return div;
}

/*
*    Configure the pad DEV_CLK divider so that output of pad divider is less than 500MHz.
*    Configure ARM_clk_divide_ratio and reg_clk_divide_ratio (register bus clock). Maximum rate for ARM clock and REGBUS clock is 500MHz. Use DEVCLK to clock the HSDIGCLK input bypassing the PLL (write clock_config_6[7] to 1)
*    The clock_config_6 register needs to be written to configure the clock divide ratios indicated <SPI doc>
*    Write SPI bit "digital_clock_power_up" to enable digital clocks (core.clock_control_6[4])
*/
int32_t adrv9025_ClocksSet(adi_adrv9025_Device_t* device,
                           adi_adrv9025_Init_t*   init)
{
    adi_adrv9025_ClockSettings_t* clocks;
    uint32_t                      digDeviceClockDiv     = 0;
    uint32_t                      refDeviceClockDiv     = 0;
    uint32_t                      regDeviceClockDiv     = 0;
    uint32_t                      armDeviceClockDiv     = 0;
    uint32_t                      agcDeviceClockDiv     = 0;
    uint32_t                      txAttenDeviceClockDiv = 0;
    uint32_t                      digRefClock_MHz       = 0;
    uint8_t                       deviceClkControl1     = 0;
    uint8_t                       clockControl7         = 0;
    uint8_t                       clockControl0         = 0;
    uint8_t                       clockConfig0          = 0;
    uint8_t                       clockConfig6          = 0;
    uint8_t                       txClockControl0       = 0;
    uint8_t                       txClockControl2       = 0;
    uint8_t                       txClockControl3       = 0;
    uint8_t                       i                     = 0;
    uint8_t                       armCtl1               = 0;
    uint32_t                      refClockDiv           = 0;
    uint8_t                       refClkByte0           = 0;

    static const uint32_t DEV_CLOCK_MAX        = 310000;
    static const uint32_t REF_CLOCK_MAX        = 250000;
    static const uint32_t DIG_CLOCK_MAX        = 500000;
    static const uint32_t PFIR_MIN_CLOCK_RATE  = 0x03;
    static const uint32_t DIGDEVICECLOCKDIVMAX = 3;
    static const uint32_t REFDEVICECLOCKDIVMAX = 7;
    static const uint32_t REFCLOCKDIVMAX       = 1;
    static const uint32_t REGDEVICECLOCKDIVMAX = 3;

    static const uint32_t ADRV9025_ADDR_CH_RX_SIZE = 6;
    static const uint32_t ADRV9025_ADDR_CH_TX_SIZE = 4;
    static const uint8_t  TXATTEN_CLK_ENABLE       = 1;

    static const uint16_t ADRV9025_ADDR_CH_RX_BASE[6] =
    {
        ADRV9025_ADDR_CH0_RX,
        ADRV9025_ADDR_CH1_RX,
        ADRV9025_ADDR_CH2_RX,
        ADRV9025_ADDR_CH3_RX,
        ADRV9025_ADDR_CH0_ORX,
        ADRV9025_ADDR_CH1_ORX
    };

    static const uint16_t ADRV9025_ADDR_CH_TX_BASE[4] =
    {ADRV9025_ADDR_CH0_TX, ADRV9025_ADDR_CH1_TX, ADRV9025_ADDR_CH2_TX, ADRV9025_ADDR_CH3_TX};

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADRV9025_BUGINFO(__FUNCTION__);

    clocks = &init->clocks;

    /* Set Digital device clock divider */
    digDeviceClockDiv = adrv9025_ClockDiv(clocks->deviceClock_kHz,
                                          DEV_CLOCK_MAX);
    if (digDeviceClockDiv > DIGDEVICECLOCKDIVMAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid clocks deviceClock_kHz");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Set Digital Ref device clock divider */
    refDeviceClockDiv = adrv9025_ClockDiv(clocks->deviceClock_kHz,
                                          REF_CLOCK_MAX);
    if (refDeviceClockDiv > REFDEVICECLOCKDIVMAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid clocks deviceClock_kHz");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }
    digRefClock_MHz = (((clocks->deviceClock_kHz / 1000) >> digDeviceClockDiv) >> refDeviceClockDiv);

    /* Set Digital Reg device clock divider  */
    regDeviceClockDiv = adrv9025_ClockDiv(device->devStateInfo.hsDigClk_kHz,
                                          DIG_CLOCK_MAX);
    if (regDeviceClockDiv > REGDEVICECLOCKDIVMAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid clocks hsDigClk_kHz");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    armDeviceClockDiv     = regDeviceClockDiv;
    agcDeviceClockDiv     = regDeviceClockDiv;
    txAttenDeviceClockDiv = regDeviceClockDiv;

    /* Set Ref clock divider  */
    refClockDiv = adrv9025_ClockDiv((clocks->deviceClock_kHz >> digDeviceClockDiv),
                                    DEV_CLOCK_MAX);
    if (refClockDiv > REFCLOCKDIVMAX)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid clocks deviceClock_kHz");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Set Device clock divider ratio */
    deviceClkControl1 = 0;
    deviceClkControl1 = ADRV9025_BF_ENCODE(digDeviceClockDiv,
                                           ADRV9025_DCC1_DEVICE_CLK_DIVIDE_RATIO_MASK,
                                           ADRV9025_DCC1_DEVICE_CLK_DIVIDE_RATIO_SHIFT);
    deviceClkControl1 |= ADRV9025_DCC1_DEVICE_CLK_BUFFER_ENABLE;
    deviceClkControl1 |= ADRV9025_DCC1_RESETB_TFLASH;
    deviceClkControl1 |= ADRV9025_DCC1_DEVCLK_DIVIDER_MCS_RESETB;
    ADRV9025_SPIWRITEBYTE("DEVICE_CLK_CONTROL_1",
                          ADRV9025_ADDR_DEVICE_CLK_CONTROL_1,
                          deviceClkControl1);

    /* core.clock_control_6  use_device_clk_as_hsdigclk  1 */
    clockControl7 = 0;
    clockControl7 |= ADRV9025_CC7_USE_DEVICE_CLK_AS_HSDIGCLK;
    ADRV9025_SPIWRITEBYTE("CLOCK_CONTROL_7",
                          ADRV9025_ADDR_CLOCK_CONTROL_7,
                          clockControl7);

    /* core.clock_control_0  arm_clk_divide_ratio  0 ; same as hsdigclk */
    clockControl0 = 0;
    clockControl0 |= ADRV9025_BF_ENCODE(armDeviceClockDiv,
                                        ADRV9025_CC0_CPU_CLK_DIVIDE_RATIO_MASK,
                                        ADRV9025_CC0_CPU_CLK_DIVIDE_RATIO_SHIFT);
    /* SIM +coreArmClkEnable*/
    clockControl0 |= ADRV9025_CC0_CPU_CLK_ENABLE;
    /* core.clock_control_0  reg_clk_divide_ratio  0 ; same as hsdigclk */
    clockControl0 |= ADRV9025_BF_ENCODE(regDeviceClockDiv,
                                        ADRV9025_CC0_REG_CLK_DIVIDE_RATIO_MASK,
                                        ADRV9025_CC0_REG_CLK_DIVIDE_RATIO_SHIFT);

    /* core.clock_control_0  dig_ref_clk_div_ratio  0 */
    clockControl0 |= ADRV9025_BF_ENCODE(refDeviceClockDiv,
                                        ADRV9025_CC0_DIG_REF_CLK_DIV_RATIO_MASK,
                                        ADRV9025_CC0_DIG_REF_CLK_DIV_RATIO_SHIFT);
    ADRV9025_SPIWRITEBYTE("CLOCK_CONTROL_0",
                          ADRV9025_ADDR_CLOCK_CONTROL_0,
                          clockControl0);

    /* clkpll.ref_clk_divide_ratio  */
    refClkByte0 |= ADRV9025_BF_ENCODE(refClockDiv,
                                      ADRV9025_REF_CLK_DIVIDE_RATIO_MASK,
                                      ADRV9025_REF_CLK_DIVIDE_RATIO_SHIFT);
    ADRV9025_SPIWRITEBYTE("PLL_REF_CLK_REG",
                          ADRV9025_ADDR_PLL_REF_CLK_REG,
                          refClkByte0);

    ADRV9025_SPIWRITEBYTE("REFERENCE_CLOCK_CYCLE",
                          ADRV9025_ADDR_REFERENCE_CLOCK_CYCLE,
                          (digRefClock_MHz - 1));

    /* All RX */
    clockConfig0 = 0;
    clockConfig0 |= ADRV9025_BF_ENCODE(armDeviceClockDiv,
                                       ADRV9025_RCC0_CPU_CLK_DIVIDE_RATIO_MASK,
                                       ADRV9025_RCC0_CPU_CLK_DIVIDE_RATIO_SHIFT);
    clockConfig0 |= ADRV9025_RCC0_CPU_CLK_ENABLE;
    clockConfig0 |= ADRV9025_BF_ENCODE(regDeviceClockDiv,
                                       ADRV9025_RCC0_REG_CLK_DIVIDE_RATIO_MASK,
                                       ADRV9025_RCC0_REG_CLK_DIVIDE_RATIO_SHIFT);
    clockConfig0 |= ADRV9025_BF_ENCODE(agcDeviceClockDiv,
                                       ADRV9025_RCC0_AGC_CLK_DIVIDE_RATIO_MASK,
                                       ADRV9025_RCC0_AGC_CLK_DIVIDE_RATIO_SHIFT);

    clockConfig6 = 0;
    clockConfig6 |= ADRV9025_BF_ENCODE(PFIR_MIN_CLOCK_RATE,
                                       ADRV9025_RCC6_RPFIR_CLK_DIVIDE_RATIO_MASK,
                                       ADRV9025_RCC6_RPFIR_CLK_DIVIDE_RATIO_SHIFT);

    for (i = 0; i < ADRV9025_ADDR_CH_RX_SIZE; i++)
    {
        ADRV9025_SPIWRITEBYTE("RX_CLOCK_CONFIG_0",
                              (ADRV9025_ADDR_CH_RX_BASE[i] + ADRV9025_ADDR_RX_CLOCK_CONFIG_0_OFFSET),
                              clockConfig0);

        ADRV9025_SPIWRITEBYTE("RX_CLOCK_CONFIG_6",
                              (ADRV9025_ADDR_CH_RX_BASE[i] + ADRV9025_ADDR_RX_CLOCK_CONFIG_6_OFFSET),
                              clockConfig6);
        if (i < 4)
        {
            ADRV9025_SPIWRITEBYTE("RX_REFERENCE_CLOCK_CYCLE",
                                  (ADRV9025_ADDR_CH_RX_BASE[i] + ADRV9025_ADDR_RX_REFERENCE_CLOCK_CYCLE_OFFSET),
                                  (digRefClock_MHz - 1));
        }
        else
        {
            ADRV9025_SPIWRITEBYTE("RX_REFERENCE_CLOCK_CYCLE",
                                  (ADRV9025_ADDR_CH_RX_BASE[i] + ADRV9025_ADDR_ORX_REFERENCE_CLOCK_CYCLE_OFFSET),
                                  (digRefClock_MHz - 1));
        }
    }

    /* All TX */
    txClockControl0 = 0;
    txClockControl0 |= ADRV9025_BF_ENCODE(armDeviceClockDiv,
                                          ADRV9025_TCC0_CPU_CLK_DIVIDE_RATIO_MASK,
                                          ADRV9025_TCC0_CPU_CLK_DIVIDE_RATIO_SHIFT);
    txClockControl0 |= ADRV9025_TCC0_CPU_CLK_ENABLE;
    txClockControl0 |= ADRV9025_BF_ENCODE(regDeviceClockDiv,
                                          ADRV9025_TCC0_REG_CLK_DIVIDE_RATIO_MASK,
                                          ADRV9025_TCC0_REG_CLK_DIVIDE_RATIO_SHIFT);
    txClockControl0 |= ADRV9025_BF_ENCODE(txAttenDeviceClockDiv,
                                          ADRV9025_TCC0_TX_ATTEN_CLK_DIVIDE_RATIO_MASK,
                                          ADRV9025_TCC0_TX_ATTEN_CLK_DIVIDE_RATIO_SHIFT);

    txClockControl2 = 0;
    txClockControl2 |= ADRV9025_BF_ENCODE(PFIR_MIN_CLOCK_RATE,
                                          ADRV9025_TCC2_TPFIR_CLK_DIVIDE_RATIO_MASK,
                                          ADRV9025_TCC2_TPFIR_CLK_DIVIDE_RATIO_SHIFT);

    txClockControl3 = 0;
    txClockControl3 |= ADRV9025_BF_ENCODE(TXATTEN_CLK_ENABLE,
                                          ADRV9025_TCC3_TX_ATTEN_CLK_ENABLE_MASK,
                                          ADRV9025_TCC3_TX_ATTEN_CLK_ENABLE_SHIFT);

    for (i = 0; i < ADRV9025_ADDR_CH_TX_SIZE; i++)
    {
        ADRV9025_SPIWRITEBYTE("TX_CLOCK_CONTROL_0",
                              (ADRV9025_ADDR_CH_TX_BASE[i] + ADRV9025_ADDR_TX_CLOCK_CONTROL_0_OFFSET),
                              txClockControl0);

        ADRV9025_SPIWRITEBYTE("TX_CLOCK_CONTROL_2",
                              (ADRV9025_ADDR_CH_TX_BASE[i] + ADRV9025_ADDR_TX_CLOCK_CONTROL_2_OFFSET),
                              txClockControl2);

        ADRV9025_SPIWRITEBYTE("TX_REFERENCE_CLOCK_CYCLE",
                              (ADRV9025_ADDR_CH_TX_BASE[i] + ADRV9025_ADDR_TX_REFERENCE_CLOCK_CYCLE_OFFSET),
                              (digRefClock_MHz - 1));

        /* Enable Tx Atten table clock for Tx channels */
        ADRV9025_SPIWRITEBYTE("TX_ATTEN_CLK_ENABLE",
                              (ADRV9025_ADDR_CH_TX_BASE[i] + ADRV9025_ADDR_TX_CLOCK_CONTROL_3_OFFSET),
                              txClockControl3);
    }

    /* core.clock_control_6 digital_clock_power_up */
    clockControl7 |= ADRV9025_CC7_DIGITAL_CLOCK_POWER_UP;
    ADRV9025_SPIWRITEBYTE("CLOCK_CONTROL_7",
                          ADRV9025_ADDR_CLOCK_CONTROL_7,
                          clockControl7);

    /* Enable SPI AHB bridge for ARM */
    ADRV9025_SPIWRITEBYTE("AHB_SPI_BRIDGE",
                          ADRV9025_ADDR_AHB_SPI_BRIDGE,
                          (ADRV9025_AHB_SPI_BRIDGE_RESET | ADRV9025_AHB_SPI_BRIDGE_EN));

    /* Init ARM C */
    armCtl1 = ADRV9025_CPU_MASK_CTL1_DEBUG_ENABLE | ADRV9025_CPU_MASK_CTL1_MEM_HRESP;
    armCtl1 &= ~ADRV9025_CPU_MASK_CTL1_M3_RUN;
    ADRV9025_SPIWRITEBYTE("ADRV9025_ADDR_CPU_CTL_1",
                          ADRV9025_CPU_C_ADDR_CTL_1,
                          armCtl1);

    /* Init ARM D */
    armCtl1 = ADRV9025_CPU_MASK_CTL1_DEBUG_ENABLE; /* ARM-D doesn't have HRESP mask, HRESP mask bit for ARM-C is being shared by 2 CPUs */
    armCtl1 &= ~ADRV9025_CPU_MASK_CTL1_M3_RUN;
    ADRV9025_SPIWRITEBYTE("ADRV9025_ADDR_CPU_D_CTL_1",
                          ADRV9025_CPU_D_ADDR_CTL_1,
                          armCtl1);

    device->devStateInfo.clkDivideRatios.agcClkDivideRatio             = agcDeviceClockDiv;
    device->devStateInfo.clkDivideRatios.armClkDivideRatio             = armDeviceClockDiv;
    device->devStateInfo.clkDivideRatios.regClkDivideRatio             = regDeviceClockDiv;
    device->devStateInfo.clkDivideRatios.txAttenDeviceClockDivideRatio = txAttenDeviceClockDiv;

    return (device->common.error.newAction);
}

/**
* \brief Verifies the Tx profile members are valid (in range) in the init structure
*
* If the Tx profile IQ data rate = 0, it is assumed that the Tx profile is
* not used.  If Tx IQ data rate > 0, and Tx profile members are out of range,
* the function stores the error code describing the error, and returns a
* Recovery action to check the configuration settings.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param txProfile txProfile settings to be verified
* \param txHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Tx profile
* \param txChannelBitMask the bit mask position corresponding the TX Channel.
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static int32_t adrv9025_VerifyTxProfile(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_TxProfile_t* txProfile,
                                        uint32_t*                 txHsDigClk_kHz,
                                        int32_t                   txChannelBitMask)
{
    static const uint32_t TXINPUTRATEMIN = 61440;
    static const uint32_t TXINPUTRATEMAX = 500000;

    static const uint32_t RFBANDWIDTHMIN = 20000;
    static const uint32_t RFBANDWIDTHMAX = 450000;

    static const uint32_t TXSRLRATEMAX1 = 500000;
    static const uint32_t TXSRLRATEMAX2 = 625000;
    uint32_t              srlInputRate  = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        txProfile);

    ADI_NULL_PTR_RETURN(&device->common,
                        txHsDigClk_kHz);

    ADRV9025_BUGINFO(__FUNCTION__);

    *txHsDigClk_kHz = 0;
    /********************************/
    /* Check for a valid Tx profile */
    /********************************/
    if ((txProfile->txInputRate_kHz < TXINPUTRATEMIN) ||
        (txProfile->txInputRate_kHz > TXINPUTRATEMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile IQRate is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->rfBandwidth_kHz < RFBANDWIDTHMIN) ||
        (txProfile->rfBandwidth_kHz > RFBANDWIDTHMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile RF bandwidth is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->thb1Interpolation != 1) &&
        (txProfile->thb1Interpolation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile halfband 1 FIR filter interpolation is out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->thb2Interpolation != 1) &&
        (txProfile->thb2Interpolation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile halfband 2 FIR filter interpolation is out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->thb3Interpolation != 1) &&
        (txProfile->thb3Interpolation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile halfband 3 FIR filter interpolation is out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->txInt5Interpolation != 1) &&
        (txProfile->txInt5Interpolation != 5))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile Interpolate by 5 FIR filter interpolation is out of range (1,5)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Int5 and thb2/3 should be mutually exclusive */
    if ((txProfile->txInt5Interpolation == 5) &&
        ((txProfile->thb2Interpolation != 1) ||
            (txProfile->thb3Interpolation != 1)))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile Interpolate by 5 FIR filter should be mutually exclusive to halfband 1,2,3 filters");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->txFirInterpolation != 1) &&
        (txProfile->txFirInterpolation != 2) &&
        (txProfile->txFirInterpolation != 4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile programmable FIR filter interpolation is out of range(1,2,4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->txFir.numFirCoefs == 0) &&
        (txProfile->txFirInterpolation != 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile PFIR coef array is NULL and filter is not disabled");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->dpdHb1Interpolation != 1) &&
        (txProfile->dpdHb1Interpolation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile dpd halfband 1 FIR filter interpolation is out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->dpdHb2Interpolation != 1) &&
        (txProfile->dpdHb2Interpolation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Tx Profile dpd halfband 2 FIR filter interpolation is out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((txProfile->txBbfPowerMode != 0) && (txProfile->txBbfPowerMode != 8))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Invalid Tx Profile BBF power mode. Allowable values are 0 and 8");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    uint32_t HsDigClk_kHz = (txProfile->txInputRate_kHz * txProfile->txFirInterpolation *
        txProfile->dpdHb1Interpolation * txProfile->dpdHb2Interpolation *
        txProfile->thb1Interpolation * txProfile->thb2Interpolation *
        txProfile->thb3Interpolation * txProfile->txInt5Interpolation);

    if (txProfile->txInt5Interpolation == 5)
    {
        HsDigClk_kHz /= 5;
    }
    else
    {
        HsDigClk_kHz /= 4;
    }

    *txHsDigClk_kHz = HsDigClk_kHz;

    /* Checking for SRL limitations */
    srlInputRate = txProfile->txInputRate_kHz * txProfile->dpdHb1Interpolation * txProfile->dpdHb2Interpolation;
    if (((txProfile->txInt5Interpolation == 5) && (srlInputRate > TXSRLRATEMAX1)) ||
        ((txProfile->txInt5Interpolation == 1) && (srlInputRate > TXSRLRATEMAX2)))
    {
        device->devStateInfo.txSrlRateInvalidMask |= (1 << txChannelBitMask);
    }

    return (device->common.error.newAction);
}

/**
* \brief Verifies the Rx profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
*
* Private helper function to verify the Rx profile members are valid (in range)
* and calculates HS Dig Clock require for the Rx Profile
* If the Rx profile IQ data rate = 0, it is assumed that the Rx profile is
* not used.  If Rx IQ data rate > 0, and Rx profile members are out of range,
* the function stores the error code describing the error, and returns a
* Recovery action to check the configuration settings.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param rxProfile rxProfile settings to be verified
* \param rxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Rx profile
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static int32_t adrv9025_VerifyRxProfile(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_RxProfile_t* rxProfile,
                                        uint32_t*                 rxHsDigClk_kHz)
{
    uint8_t ddcMultiply = 1;
    uint8_t ddcDivide   = 1;

    static const uint32_t RXOUTPUTRATEMIN = 61440;
    static const uint32_t RXOUTPUTRATEMAX = 500000;

    static const uint32_t RXBANDWIDTHMIN = 20000;
    static const uint32_t RXBANDWIDTHMAX = 300000;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxProfile);

    ADI_NULL_PTR_RETURN(&device->common,
                        rxHsDigClk_kHz);

    ADRV9025_BUGINFO(__FUNCTION__);

    *rxHsDigClk_kHz = 0;

    /********************************/
    /* Check for a valid Rx profile */
    /********************************/
    if ((rxProfile->rxOutputRate_kHz < RXOUTPUTRATEMIN) ||
        (rxProfile->rxOutputRate_kHz > RXOUTPUTRATEMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile IQRate is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rfBandwidth_kHz < RXBANDWIDTHMIN) ||
        (rxProfile->rfBandwidth_kHz > RXBANDWIDTHMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile RF bandwidth is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rhb1Decimation != 1) &&
        (rxProfile->rhb1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile halfband 1 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rhb2Decimation != 1) &&
        (rxProfile->rhb2Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile halfband 2 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rhb3Decimation != 1) &&
        (rxProfile->rhb3Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile halfband 3 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rxDec5Decimation != 1) &&
        (rxProfile->rxDec5Decimation != 5))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile Dec5 FIR filter decimation out of range(1,5)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rxFirDecimation != 1) &&
        (rxProfile->rxFirDecimation != 2) &&
        (rxProfile->rxFirDecimation != 4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile programmable FIR decimation out of range (1,2,4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rxFir1Decimation != 1) &&
        (rxProfile->rxFir1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile FIR 1 decimation out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rxFir2Decimation != 1) &&
        (rxProfile->rxFir2Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile FIR 2 decimation out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((rxProfile->rxFir.numFirCoefs == 0) &&
        (rxProfile->rxFirDecimation != 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile PFIR coefficient array is NULL and filter is not bypassed");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (rxProfile->tiaPowerMode > 4)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile TIA Power mode is out of range (0 - 4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (rxProfile->rxDdcMode)
    {
    case ADI_ADRV9025_RXDDC_BYPASS_REALIF:
    case ADI_ADRV9025_RXDDC_BYPASS:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_DEC2_REALIF:
    case ADI_ADRV9025_RXDDC_DEC2:
        ddcMultiply = 2;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_INT2_REALIF:
    case ADI_ADRV9025_RXDDC_INT2:
        ddcMultiply = 1;
        ddcDivide = 2;
        break;
    case ADI_ADRV9025_RXDDC_FILTERONLY_REALIF:
    case ADI_ADRV9025_RXDDC_FILTERONLY:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "Rx Profile DDC mode is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    uint32_t HsDigClk_kHz = (rxProfile->rxOutputRate_kHz * rxProfile->rxFirDecimation *
        rxProfile->rxFir1Decimation * rxProfile->rxFir2Decimation *
        rxProfile->rhb1Decimation * rxProfile->rhb2Decimation * rxProfile->rhb3Decimation *
        rxProfile->rxDec5Decimation * ddcMultiply) / ddcDivide;

    if (rxProfile->rxDec5Decimation == 5)
    {
        HsDigClk_kHz /= 5;
    }
    else
    {
        HsDigClk_kHz /= 4;
    }

    *rxHsDigClk_kHz = HsDigClk_kHz;

    return (device->common.error.newAction);
}

/**
* \brief Verifies the Rx profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
*
* Private helper function to verify the Rx profile members are valid (in range)
* and calculates HS Dig Clock require for the Rx Profile
* If the Rx profile IQ data rate = 0, it is assumed that the Rx profile is
* not used.  If Rx IQ data rate > 0, and Rx profile members are out of range,
* the function stores the error code describing the error, and returns a
* Recovery action to check the configuration settings.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param orxProfile rxProfile settings to be verified
* \param orxHsDigClk_kHz Return value of the calculated HS Dig Clock required by the Rx profile
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static int32_t adrv9025_VerifyOrxProfile(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxProfile_t* orxProfile,
                                         uint32_t*                 orxHsDigClk_kHz)
{
    uint8_t ddcMultiply = 1;
    uint8_t ddcDivide   = 1;

    static const uint32_t RXOUTPUTRATEMIN = 122880;
    static const uint32_t RXOUTPUTRATEMAX = 500000;

    static const uint32_t RXBANDWIDTHMIN = 20000;
    static const uint32_t RXBANDWIDTHMAX = 450000;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        orxProfile);

    ADI_NULL_PTR_RETURN(&device->common,
                        orxHsDigClk_kHz);

    ADRV9025_BUGINFO(__FUNCTION__);

    *orxHsDigClk_kHz = 0;

    /********************************/
    /* Check for a valid ORx profile */
    /********************************/
    if ((orxProfile->rxOutputRate_kHz < RXOUTPUTRATEMIN) ||
        (orxProfile->rxOutputRate_kHz > RXOUTPUTRATEMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile IQRate is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rfBandwidth_kHz < RXBANDWIDTHMIN) ||
        (orxProfile->rfBandwidth_kHz > RXBANDWIDTHMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile RF bandwidth is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rhb1Decimation != 1) &&
        (orxProfile->rhb1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile halfband 1 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rhb2Decimation != 1) &&
        (orxProfile->rhb2Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile halfband 2 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rhb3Decimation != 1) &&
        (orxProfile->rhb3Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile halfband 3 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rxDec5Decimation != 1) &&
        (orxProfile->rxDec5Decimation != 5))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile Dec5 FIR filter decimation out of range(1,5)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rxFirDecimation != 1) &&
        (orxProfile->rxFirDecimation != 2) &&
        (orxProfile->rxFirDecimation != 4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile programmable FIR decimation out of range (1,2,4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rxFir1Decimation != 1) &&
        (orxProfile->rxFir1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile FIR 1 decimation out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((orxProfile->rxFir.numFirCoefs == 0) &&
        (orxProfile->rxFirDecimation != 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile PFIR coefficient array is NULL and filter is not bypassed");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if (orxProfile->tiaPowerMode > 4)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile TIA Power mode is out of range (0 - 4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (orxProfile->rxDdcMode)
    {
    case ADI_ADRV9025_RXDDC_BYPASS_REALIF:
    case ADI_ADRV9025_RXDDC_BYPASS:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_DEC2_REALIF:
    case ADI_ADRV9025_RXDDC_DEC2:
        ddcMultiply = 2;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_INT2_REALIF:
    case ADI_ADRV9025_RXDDC_INT2:
        ddcMultiply = 1;
        ddcDivide = 2;
        break;
    case ADI_ADRV9025_RXDDC_FILTERONLY_REALIF:
    case ADI_ADRV9025_RXDDC_FILTERONLY:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "ORx Profile DDC mode is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    uint32_t HsDigClk_kHz = (orxProfile->rxOutputRate_kHz * orxProfile->rxFirDecimation *
                             orxProfile->rxFir1Decimation *
                             orxProfile->rhb1Decimation * orxProfile->rhb2Decimation * orxProfile->rhb3Decimation *
                             orxProfile->rxDec5Decimation * ddcMultiply) / ddcDivide;

    if (orxProfile->rxDec5Decimation == 5)
    {
        HsDigClk_kHz /= 5;
    }
    else
    {
        HsDigClk_kHz /= 4;
    }

    *orxHsDigClk_kHz = HsDigClk_kHz;

    return (device->common.error.newAction);
}

/**
* \brief Verifies the Loopback profile members are valid (in range) and calculates HS Dig Clock require for the Rx Profile
*
* Private helper function to verify the Loopback profile members are valid (in range)
* and calculates HS Dig Clock require for the Loopback Profile
* If the LB profile IQ data rate = 0, it is assumed that the LB profile is
* not used.  If LB IQ data rate > 0, and LB profile members are out of range,
* the function stores the error code describing the error, and returns a
* Recovery action to check the configuration settings.
*
* \pre This function is private and is not called directly by the user.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param device Structure pointer to ADRV9025 device data structure
* \param lbProfile lbProfile settings to be verified
* \param lbHsDigClk_kHz Return value of the calculated HS Dig Clock required by the LB profile
*
* \retval ADI_COMMON_ACT_ERR_CHECK_PARAM Recovery action for bad parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
static int32_t adrv9025_VerifyLbProfile(adi_adrv9025_Device_t*    device,
                                        adi_adrv9025_RxProfile_t* lbProfile,
                                        uint32_t*                 lbHsDigClk_kHz)
{
    uint8_t ddcMultiply = 1;
    uint8_t ddcDivide   = 1;

    static const uint32_t RXOUTPUTRATEMIN = 61440;
    static const uint32_t RXOUTPUTRATEMAX = 500000;

    static const uint32_t RXBANDWIDTHMIN = 20000;
    static const uint32_t RXBANDWIDTHMAX = 455000;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        lbProfile);

    ADI_NULL_PTR_RETURN(&device->common,
                        lbHsDigClk_kHz);

    ADRV9025_BUGINFO(__FUNCTION__);

    *lbHsDigClk_kHz = 0;

    /********************************/
    /* Check for a valid ORx profile */
    /********************************/
    if ((lbProfile->rxOutputRate_kHz < RXOUTPUTRATEMIN) ||
        (lbProfile->rxOutputRate_kHz > RXOUTPUTRATEMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile IQRate is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rfBandwidth_kHz < RXBANDWIDTHMIN) ||
        (lbProfile->rfBandwidth_kHz > RXBANDWIDTHMAX))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile RF bandwidth is out of range");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rhb1Decimation != 1) &&
        (lbProfile->rhb1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile halfband 1 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rhb2Decimation != 1) &&
        (lbProfile->rhb2Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile halfband 2 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rhb3Decimation != 1) &&
        (lbProfile->rhb3Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile halfband 3 FIR filter decimation out of range(1, 2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rxDec5Decimation != 1) &&
        (lbProfile->rxDec5Decimation != 5))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile Dec5 FIR filter decimation out of range(1,5)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rxFirDecimation != 1) &&
        (lbProfile->rxFirDecimation != 2) &&
        (lbProfile->rxFirDecimation != 4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile programmable FIR decimation out of range (1,2,4)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rxFir1Decimation != 1) &&
        (lbProfile->rxFir1Decimation != 2))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile FIR 1 decimation out of range (1,2)");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((lbProfile->rxFir.numFirCoefs == 0) &&
        (lbProfile->rxFirDecimation != 1))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile PFIR coefficient array is NULL and filter is not bypassed");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    switch (lbProfile->rxDdcMode)
    {
    case ADI_ADRV9025_RXDDC_BYPASS_REALIF:
    case ADI_ADRV9025_RXDDC_BYPASS:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_DEC2_REALIF:
    case ADI_ADRV9025_RXDDC_DEC2:
        ddcMultiply = 2;
        ddcDivide = 1;
        break;
    case ADI_ADRV9025_RXDDC_INT2_REALIF:
    case ADI_ADRV9025_RXDDC_INT2:
        ddcMultiply = 1;
        ddcDivide = 2;
        break;
    case ADI_ADRV9025_RXDDC_FILTERONLY_REALIF:
    case ADI_ADRV9025_RXDDC_FILTERONLY:
        ddcMultiply = 1;
        ddcDivide = 1;
        break;
    default:
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "LB Profile DDC mode is invalid");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    uint32_t HsDigClk_kHz = (lbProfile->rxOutputRate_kHz * lbProfile->rxFirDecimation *
        lbProfile->rxFir1Decimation *
        lbProfile->rhb1Decimation * lbProfile->rhb2Decimation * lbProfile->rhb3Decimation *
        lbProfile->rxDec5Decimation * ddcMultiply) / ddcDivide;

    if (lbProfile->rxDec5Decimation == 5)
    {
        HsDigClk_kHz /= 5;
    }
    else
    {
        HsDigClk_kHz /= 4;
    }

    *lbHsDigClk_kHz = HsDigClk_kHz;

    return (device->common.error.newAction);
}

int32_t adrv9025_ProfilesVerify(adi_adrv9025_Device_t* device,
                                adi_adrv9025_Init_t*   init)
{
    uint32_t                  rxHsDigClk_kHz  = 0;
    uint32_t                  orxHsDigClk_kHz = 0;
    uint32_t                  txHsDigClk_kHz  = 0;
    uint32_t                  lbxHsDigClk_kHz = 0;
    uint32_t                  hsDigClk_kHz    = 0;
    adi_adrv9025_RxProfile_t* rxProfile       = NULL;
    adi_adrv9025_TxProfile_t* txProfile       = NULL;
    int                       i               = 0;
    uint32_t                  maskBit         = 0;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    ADI_FUNCTION_ENTRY_LOG(&device->common,
                           ADI_COMMON_LOG_API_PRIV);

    ADI_NULL_PTR_RETURN(&device->common,
                        init);

    ADRV9025_BUGINFO(__FUNCTION__);

    device->devStateInfo.profilesValid = 0;

    hsDigClk_kHz = 0;
    if (init->tx.txInitChannelMask != ADI_ADRV9025_TXOFF)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_TXCHANNELS; i++)
        {
            maskBit = (1 << i);
            if (ADRV9025_BF_EQUAL(init->tx.txInitChannelMask,
                                  maskBit))
            {
                txProfile = &init->tx.txChannelCfg[i].profile;
                adrv9025_VerifyTxProfile(device,
                                         txProfile,
                                         &txHsDigClk_kHz,
                                         i);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (hsDigClk_kHz == 0)
                {
                    hsDigClk_kHz = txHsDigClk_kHz;
                }

                if (hsDigClk_kHz != txHsDigClk_kHz)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     txHsDigClk_kHz,
                                     "Tx Profile IQrate and filter settings are not possible with current CLKPLL frequency");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    hsDigClk_kHz = 0;
    if (init->rx.rxInitChannelMask != ADI_ADRV9025_RXOFF)
    {
        for (i = 0; i < ADI_ADRV9025_MAX_RX_ONLY; i++)
        {
            maskBit = (1 << i);
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  maskBit))
            {
                rxProfile = &init->rx.rxChannelCfg[i].profile;
                adrv9025_VerifyRxProfile(device,
                                         rxProfile,
                                         &rxHsDigClk_kHz);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (hsDigClk_kHz == 0)
                {
                    hsDigClk_kHz = rxHsDigClk_kHz;
                }

                if (hsDigClk_kHz != rxHsDigClk_kHz)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     rxHsDigClk_kHz,
                                     "Rx Profile IQrate and filter settings are not possible with current CLKPLL frequency");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    hsDigClk_kHz = 0;
    if (init->rx.rxInitChannelMask != ADI_ADRV9025_RXOFF)
    {
        for (i = ADI_ADRV9025_MAX_RX_ONLY; i < ADI_ADRV9025_MAX_LB_CHANNEL_START; i++)
        {
            maskBit = (1 << i);
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  maskBit))
            {
                rxProfile = &init->rx.rxChannelCfg[i].profile;
                adrv9025_VerifyOrxProfile(device,
                                          rxProfile,
                                          &orxHsDigClk_kHz);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (hsDigClk_kHz == 0)
                {
                    hsDigClk_kHz = orxHsDigClk_kHz;
                }

                if (hsDigClk_kHz != orxHsDigClk_kHz)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     orxHsDigClk_kHz,
                                     "ORx Profile IQrate and filter settings are not possible with current CLKPLL frequency");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    hsDigClk_kHz = 0;
    if (init->rx.rxInitChannelMask != ADI_ADRV9025_RXOFF)
    {
        for (i = ADI_ADRV9025_MAX_LB_CHANNEL_START; i < ADI_ADRV9025_MAX_RXCHANNELS; i++)
        {
            maskBit = (1 << i);
            if (ADRV9025_BF_EQUAL(init->rx.rxInitChannelMask,
                                  maskBit))
            {
                rxProfile = &init->rx.rxChannelCfg[i].profile;
                adrv9025_VerifyLbProfile(device,
                                         rxProfile,
                                         &lbxHsDigClk_kHz);
                ADI_ERROR_RETURN(device->common.error.newAction);

                if (hsDigClk_kHz == 0)
                {
                    hsDigClk_kHz = lbxHsDigClk_kHz;
                }

                if (hsDigClk_kHz != lbxHsDigClk_kHz)
                {
                    ADI_ERROR_REPORT(&device->common,
                                     ADI_COMMON_ERRSRC_API,
                                     ADI_COMMON_ERR_INV_PARAM,
                                     ADI_COMMON_ACT_ERR_CHECK_PARAM,
                                     orxHsDigClk_kHz,
                                     "LBx Profile IQrate and filter settings are not possible with current CLKPLL frequency");
                    ADI_ERROR_RETURN(device->common.error.newAction);
                }
            }
        }
    }

    if ((init->clocks.rfPllPhaseSyncMode != ADI_ADRV9025_RFPLLMCS_NOSYNC)
        && (init->clocks.rfPllPhaseSyncMode != ADI_ADRV9025_RFPLLMCS_INIT_AND_SYNC)
        && (init->clocks.rfPllPhaseSyncMode != ADI_ADRV9025_RFPLLMCS_INIT_AND_CONTTRACK))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         device,
                         "init->clocks.rfPllPhaseSyncMode structure member has invalid option for RFPLL phase sync mode");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /* Set the maximum hsDigClk_kHz */
    if (txHsDigClk_kHz > 0)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV9025_TX_PROFILE_VALID;
        device->devStateInfo.hsDigClk_kHz = txHsDigClk_kHz;
    }

    /* Verify Rx profile is valid */
    if (rxHsDigClk_kHz > 0)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV9025_RX_PROFILE_VALID;
        if (device->devStateInfo.hsDigClk_kHz < rxHsDigClk_kHz)
        {
            device->devStateInfo.hsDigClk_kHz = rxHsDigClk_kHz;
        }
    }

    /* Verify ORx profile is valid */
    if (orxHsDigClk_kHz > 0)
    {
        device->devStateInfo.profilesValid |= ADI_ADRV9025_ORX_PROFILE_VALID;
        if (device->devStateInfo.hsDigClk_kHz < orxHsDigClk_kHz)
        {
            device->devStateInfo.hsDigClk_kHz = orxHsDigClk_kHz;
        }
    }

    /* Verify LBx profile is valid */
    if ((lbxHsDigClk_kHz > 0) && (txHsDigClk_kHz > 0))
    {
        device->devStateInfo.profilesValid |= ADI_ADRV9025_LB_PROFILE_VALID;
    }

    return (device->common.error.newAction);
}

int32_t adrv9025_RxOverloadProtectionSet(adi_adrv9025_Device_t*    device,
                                         adi_adrv9025_RxChannels_e rxChannel)
{
    static const uint8_t ALL_RX_MASK = (uint32_t)(ADI_ADRV9025_RX1 | ADI_ADRV9025_RX2 | ADI_ADRV9025_RX3 | ADI_ADRV9025_RX4);
    static const uint8_t ALL_ORX_MASK = (uint32_t)(ADI_ADRV9025_ORX1 | ADI_ADRV9025_ORX2 | ADI_ADRV9025_ORX3 | ADI_ADRV9025_ORX4);
    static const uint8_t  MANUAL_GAIN_CTRL_CONFIG_REG_VAL                = 0x02;
    static const uint16_t MANUAL_GAIN_CTRL_CONFIG_REG_RX_OFFSET          = 0x52;
    static const uint16_t MANUAL_GAIN_CTRL_CONFIG_REG_ORX_OFFSET         = 0x74;
    static const uint8_t  ULB_THRESHOLD_EXCEEDED_CNTR_REG_VAL            = 0x03;
    static const uint16_t ULB_THRESHOLD_EXCEEDED_CNTR_REG_RX_OFFSET      = 0x6E;
    static const uint16_t ULB_THRESHOLD_EXCEEDED_CNTR_REG_ORX_OFFSET     = 0x8B;
    static const uint8_t  AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_VAL        = 0x03;
    static const uint16_t AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_RX_OFFSET  = 0x70;
    static const uint16_t AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_ORX_OFFSET = 0x8D;
    static const uint64_t AGC_GAIN_UPDATE_CNTR_uS                        = 500;
    static const uint32_t MAX_AGC_GAIN_UPDATE_CNTR_REG_VAL               = 0x03FFFFFF;
    static const uint16_t AGC_GAIN_UPDATE_CNTR1_RX_OFFSET                = 0x77;
    static const uint16_t AGC_GAIN_UPDATE_CNTR2_RX_OFFSET                = 0x78;
    static const uint16_t AGC_GAIN_UPDATE_CNTR3_RX_OFFSET                = 0x79;
    static const uint16_t AGC_GAIN_UPDATE_CNTR1_ORX_OFFSET               = 0x94;
    static const uint16_t AGC_GAIN_UPDATE_CNTR2_ORX_OFFSET               = 0x95;
    static const uint16_t AGC_GAIN_UPDATE_CNTR3_ORX_OFFSET               = 0x96;
    static const uint8_t  AGC_SLOWLOOP_SETTLING_DELAY_REG_VAL            = 0x10;
    static const uint16_t AGC_SLOWLOOP_SETTLING_DELAY_REG_RX_OFFSET      = 0x7B;
    static const uint16_t AGC_SLOWLOOP_SETTLING_DELAY_REG_ORX_OFFSET     = 0x9B;
    static const uint8_t  DEC_OVERLOAD_CONFIG1_REG_VAL                   = 0xA1;
    static const uint16_t DEC_OVERLOAD_CONFIG1_REG_RX_OFFSET             = 0xF4;
    static const uint16_t DEC_OVERLOAD_CONFIG1_REG_ORX_OFFSET            = 0xC0;
    static const uint8_t  DEC_OVERLOAD_CONFIG2_REG_VAL                   = 0x07;
    static const uint16_t DEC_OVERLOAD_CONFIG2_REG_RX_OFFSET             = 0xF5;
    static const uint16_t DEC_OVERLOAD_CONFIG2_REG_ORX_OFFSET            = 0xC1;
    static const uint8_t  DEC_OVERLOAD_UPPER_THRESHOLD1_REG_VAL          = 0x94;
    static const uint16_t DEC_OVERLOAD_UPPER_THRESHOLD1_REG_RX_OFFSET    = 0xF6;
    static const uint16_t DEC_OVERLOAD_UPPER_THRESHOLD1_REG_ORX_OFFSET   = 0xC2;
    static const uint8_t  DEC_OVERLOAD_UPPER_THRESHOLD2_REG_VAL          = 0x28;
    static const uint16_t DEC_OVERLOAD_UPPER_THRESHOLD2_REG_RX_OFFSET    = 0xF7;
    static const uint16_t DEC_OVERLOAD_UPPER_THRESHOLD2_REG_ORX_OFFSET   = 0xC3;
    static const uint8_t  ANALOG_RX_BLOCK_DET_ULBTH_REG_VAL              = 0x2A; /*-2dBFS Threshold, Assuming FS of ADC is 0.85V*/
    static const uint8_t  ANALOG_RX_BLOCK_DET_LLBTH_REG_VAL              = 0x1E; /*-5dBFS Threshold, Assuming FS of ADC is 0.85V*/
    static const uint16_t ANALOG_RX_BLOCK_DET_ULBTH_REG_RX_OFFSET        = 0x65;
    static const uint16_t ANALOG_RX_BLOCK_DET_LLBTH_REG_RX_OFFSET        = 0x63;
    static const uint16_t ANALOG_RX_BLOCK_DET_ULBTH_REG_ORX_OFFSET       = 0x9E;
    static const uint16_t ANALOG_RX_BLOCK_DET_LLBTH_REG_ORX_OFFSET       = 0x9A;
    static const uint16_t RX_BLOCK_TIA_VALID_OVERRIDE_OFFSET             = 0x5A;
    static const uint16_t ORX_BLOCK_TIA_VALID_OVERRIDE_OFFSET            = 0x7C;
    static const uint8_t  SLOWLOOP_CONFIG_REG_VAL                        = 0x3C;
    static const uint16_t SLOWLOOP_CONFIG_REG_RX_OFFSET                  = 0x7A;
    static const uint16_t SLOWLOOP_CONFIG_REG_ORX_OFFSET                 = 0x9A;
    static const uint8_t  TIA_VALID_OVERRIDE_MASK                        = 0x02;
    static const uint8_t  TIA_VALID_OVERRIDE_SHIFT                       = 0x01;

    uint32_t                             agcClkRate_kHz                   = (device->devStateInfo.hsDigClk_kHz >> device->devStateInfo.clkDivideRatios.agcClkDivideRatio);
    uint64_t                             agcGainUpdateCounter             = NO_OS_DIV_U64((AGC_GAIN_UPDATE_CNTR_uS * agcClkRate_kHz), 1000);
    uint8_t                              agcGainUpdateCntr1RegVal         = (uint8_t)(agcGainUpdateCounter & (uint32_t)0x000000FF);
    uint8_t                              agcGainUpdateCntr2RegVal         = (uint8_t)((uint32_t)(agcGainUpdateCounter & (uint32_t)0x0000FF00) >> 8);
    uint8_t                              agcGainUpdateCntr3RegVal         = (uint8_t)((uint32_t)(agcGainUpdateCounter & (uint32_t)0x00FF0000) >> 16);
    uint16_t                             manualGainConfigRegAddr          = 0;
    uint16_t                             ulbThresholdExceededCntrRegAddr  = 0;
    uint16_t                             adcHighOvrgExceededCntrRegAddr   = 0;
    uint16_t                             agcGainUpdateCntr1RegAddr        = 0;
    uint16_t                             agcGainUpdateCntr2RegAddr        = 0;
    uint16_t                             agcGainUpdateCntr3RegAddr        = 0;
    uint16_t                             agcSlowLoopSettlingDelayRegAddr  = 0;
    uint16_t                             decOverloadConfig1RegAddr        = 0;
    uint16_t                             decOverloadConfig2RegAddr        = 0;
    uint16_t                             decOverloadUpperThresh1RegAddr   = 0;
    uint16_t                             decOverloadUpperThresh2RegAddr   = 0;
    uint16_t                             analogRxBlockDetUlbthRegAddr     = 0;
    uint16_t                             analogRxBlockDetLlbthRegAddr     = 0;
    uint16_t                             slowLoopConfigRegAddr            = 0;
    uint16_t                             tiaValidOverrideRegAddr          = 0;
    adrv9025_BfRxChanAddr_e              rxChannelBitfieldBaseAddr        = (adrv9025_BfRxChanAddr_e)0;
    adrv9025_BfOrxChanAddr_e             orxChannelBitfieldBaseAddr       = (adrv9025_BfOrxChanAddr_e)0;
    adrv9025_BfAnalogRxMemMapChanAddr_e  rxChannelAnalogBitfieldBaseAddr  = (adrv9025_BfAnalogRxMemMapChanAddr_e)0;
    adrv9025_BfAnalogOrxMemMapChanAddr_e orxChannelAnalogBitfieldBaseAddr = (adrv9025_BfAnalogOrxMemMapChanAddr_e)0;

    /*Check that the requested rxChannel is valid*/
    if ((rxChannel != ADI_ADRV9025_RX1) &&
        (rxChannel != ADI_ADRV9025_RX2) &&
        (rxChannel != ADI_ADRV9025_RX3) &&
        (rxChannel != ADI_ADRV9025_RX4) &&
        (rxChannel != ADI_ADRV9025_ORX1) &&
        (rxChannel != ADI_ADRV9025_ORX2) &&
        (rxChannel != ADI_ADRV9025_ORX3) &&
        (rxChannel != ADI_ADRV9025_ORX4))
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         rxChannel,
                         "Invalid Rx Channel Requested for Rx Overload Protection");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Range check that gain update counter is less than pow(2,22)-1 since the reg is 22 bits wide*/
    if (agcGainUpdateCounter > MAX_AGC_GAIN_UPDATE_CNTR_REG_VAL)
    {
        ADI_ERROR_REPORT(&device->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         AGC_GAIN_UPDATE_CNTR_uS,
                         "AGC Gain Update counter interval too large for the given agc clk rate. Max allowed value = 0x03FFFFFF.");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    if ((ALL_RX_MASK & (uint32_t)rxChannel) > 0)
    {
        adrv9025_RxBitfieldAddressGet(device,
                                      rxChannel,
                                      &rxChannelBitfieldBaseAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_RxAnalogBitfieldAddressGet(device,
                                            rxChannel,
                                            &rxChannelAnalogBitfieldBaseAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        manualGainConfigRegAddr         = ((uint16_t)rxChannelBitfieldBaseAddr + MANUAL_GAIN_CTRL_CONFIG_REG_RX_OFFSET);
        ulbThresholdExceededCntrRegAddr = ((uint16_t)rxChannelBitfieldBaseAddr + ULB_THRESHOLD_EXCEEDED_CNTR_REG_RX_OFFSET);
        adcHighOvrgExceededCntrRegAddr  = ((uint16_t)rxChannelBitfieldBaseAddr + AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_RX_OFFSET);
        agcGainUpdateCntr1RegAddr       = ((uint16_t)rxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR1_RX_OFFSET);
        agcGainUpdateCntr2RegAddr       = ((uint16_t)rxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR2_RX_OFFSET);
        agcGainUpdateCntr3RegAddr       = ((uint16_t)rxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR3_RX_OFFSET);
        agcSlowLoopSettlingDelayRegAddr = ((uint16_t)rxChannelBitfieldBaseAddr + AGC_SLOWLOOP_SETTLING_DELAY_REG_RX_OFFSET);
        decOverloadConfig1RegAddr       = ((uint16_t)rxChannelBitfieldBaseAddr + DEC_OVERLOAD_CONFIG1_REG_RX_OFFSET);
        decOverloadConfig2RegAddr       = ((uint16_t)rxChannelBitfieldBaseAddr + DEC_OVERLOAD_CONFIG2_REG_RX_OFFSET);
        decOverloadUpperThresh1RegAddr  = ((uint16_t)rxChannelBitfieldBaseAddr + DEC_OVERLOAD_UPPER_THRESHOLD1_REG_RX_OFFSET);
        decOverloadUpperThresh2RegAddr  = ((uint16_t)rxChannelBitfieldBaseAddr + DEC_OVERLOAD_UPPER_THRESHOLD2_REG_RX_OFFSET);
        analogRxBlockDetUlbthRegAddr    = ((uint16_t)rxChannelAnalogBitfieldBaseAddr + ANALOG_RX_BLOCK_DET_ULBTH_REG_RX_OFFSET);
        analogRxBlockDetLlbthRegAddr    = ((uint16_t)rxChannelAnalogBitfieldBaseAddr + ANALOG_RX_BLOCK_DET_LLBTH_REG_RX_OFFSET);
        slowLoopConfigRegAddr           = ((uint16_t)rxChannelBitfieldBaseAddr + SLOWLOOP_CONFIG_REG_RX_OFFSET);
        tiaValidOverrideRegAddr         = (uint16_t)rxChannelBitfieldBaseAddr + RX_BLOCK_TIA_VALID_OVERRIDE_OFFSET;
    }
    else if ((ALL_ORX_MASK & (uint32_t)rxChannel) > 0)
    {
        adrv9025_OrxBitfieldAddressGet(device,
                                       rxChannel,
                                       &orxChannelBitfieldBaseAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        adrv9025_OrxAnalogBitfieldAddressGet(device,
                                             rxChannel,
                                             &orxChannelAnalogBitfieldBaseAddr);
        ADI_ERROR_RETURN(device->common.error.newAction);

        manualGainConfigRegAddr         = ((uint16_t)orxChannelBitfieldBaseAddr + MANUAL_GAIN_CTRL_CONFIG_REG_ORX_OFFSET);
        ulbThresholdExceededCntrRegAddr = ((uint16_t)orxChannelBitfieldBaseAddr + ULB_THRESHOLD_EXCEEDED_CNTR_REG_ORX_OFFSET);
        adcHighOvrgExceededCntrRegAddr  = ((uint16_t)orxChannelBitfieldBaseAddr + AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_ORX_OFFSET);
        agcGainUpdateCntr1RegAddr       = ((uint16_t)orxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR1_ORX_OFFSET);
        agcGainUpdateCntr2RegAddr       = ((uint16_t)orxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR2_ORX_OFFSET);
        agcGainUpdateCntr3RegAddr       = ((uint16_t)orxChannelBitfieldBaseAddr + AGC_GAIN_UPDATE_CNTR3_ORX_OFFSET);
        agcSlowLoopSettlingDelayRegAddr = ((uint16_t)orxChannelBitfieldBaseAddr + AGC_SLOWLOOP_SETTLING_DELAY_REG_ORX_OFFSET);
        decOverloadConfig1RegAddr       = ((uint16_t)orxChannelBitfieldBaseAddr + DEC_OVERLOAD_CONFIG1_REG_ORX_OFFSET);
        decOverloadConfig2RegAddr       = ((uint16_t)orxChannelBitfieldBaseAddr + DEC_OVERLOAD_CONFIG2_REG_ORX_OFFSET);
        decOverloadUpperThresh1RegAddr  = ((uint16_t)orxChannelBitfieldBaseAddr + DEC_OVERLOAD_UPPER_THRESHOLD1_REG_ORX_OFFSET);
        decOverloadUpperThresh2RegAddr  = ((uint16_t)orxChannelBitfieldBaseAddr + DEC_OVERLOAD_UPPER_THRESHOLD2_REG_ORX_OFFSET);
        analogRxBlockDetLlbthRegAddr    = ((uint16_t)orxChannelAnalogBitfieldBaseAddr + ANALOG_RX_BLOCK_DET_LLBTH_REG_ORX_OFFSET);
        analogRxBlockDetUlbthRegAddr    = ((uint16_t)orxChannelAnalogBitfieldBaseAddr + ANALOG_RX_BLOCK_DET_ULBTH_REG_ORX_OFFSET);
        slowLoopConfigRegAddr           = ((uint16_t)orxChannelBitfieldBaseAddr + SLOWLOOP_CONFIG_REG_ORX_OFFSET);
        tiaValidOverrideRegAddr         = (uint16_t)orxChannelBitfieldBaseAddr + ORX_BLOCK_TIA_VALID_OVERRIDE_OFFSET;
    }

    ADRV9025_SPIWRITEBYTE("MANUAL_GAIN_CTRL_CONFIG_REG",
                          manualGainConfigRegAddr,
                          MANUAL_GAIN_CTRL_CONFIG_REG_VAL);
    ADRV9025_SPIWRITEBYTE("ULB_THRESHOLD_EXCEEDED_CNTR_REG",
                          ulbThresholdExceededCntrRegAddr,
                          ULB_THRESHOLD_EXCEEDED_CNTR_REG_VAL);
    ADRV9025_SPIWRITEBYTE("AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG",
                          adcHighOvrgExceededCntrRegAddr,
                          AGC_ADC_HIGH_OVRG_EXCEEDED_CNTR_REG_VAL);
    ADRV9025_SPIWRITEBYTE("AGC_GAIN_UPDATE_CNTR",
                          agcGainUpdateCntr1RegAddr,
                          agcGainUpdateCntr1RegVal);
    ADRV9025_SPIWRITEBYTE("AGC_GAIN_UPDATE_CNTR",
                          agcGainUpdateCntr2RegAddr,
                          agcGainUpdateCntr2RegVal);
    ADRV9025_SPIWRITEBYTE("AGC_GAIN_UPDATE_CNTR",
                          agcGainUpdateCntr3RegAddr,
                          agcGainUpdateCntr3RegVal);
    ADRV9025_SPIWRITEBYTE("AGC_SLOWLOOP_SETTLING_DELAY_REG",
                          agcSlowLoopSettlingDelayRegAddr,
                          AGC_SLOWLOOP_SETTLING_DELAY_REG_VAL);
    ADRV9025_SPIWRITEBYTE("DEC_OVERLOAD_CONFIG1_REG",
                          decOverloadConfig1RegAddr,
                          DEC_OVERLOAD_CONFIG1_REG_VAL);
    ADRV9025_SPIWRITEBYTE("DEC_OVERLOAD_CONFIG2_REG",
                          decOverloadConfig2RegAddr,
                          DEC_OVERLOAD_CONFIG2_REG_VAL);
    ADRV9025_SPIWRITEBYTE("DEC_OVERLOAD_UPPER_THRESHOLD1_REG",
                          decOverloadUpperThresh1RegAddr,
                          DEC_OVERLOAD_UPPER_THRESHOLD1_REG_VAL);
    ADRV9025_SPIWRITEBYTE("DEC_OVERLOAD_UPPER_THRESHOLD2_REG",
                          decOverloadUpperThresh2RegAddr,
                          DEC_OVERLOAD_UPPER_THRESHOLD2_REG_VAL);
    ADRV9025_SPIWRITEBYTE("ANALOG_RX_BLOCK_DET_ULBTH_REG",
                          analogRxBlockDetUlbthRegAddr,
                          ANALOG_RX_BLOCK_DET_ULBTH_REG_VAL);
    ADRV9025_SPIWRITEBYTE("ANALOG_RX_BLOCK_DET_LLBTH_REG",
                          analogRxBlockDetLlbthRegAddr,
                          ANALOG_RX_BLOCK_DET_LLBTH_REG_VAL);
    ADRV9025_SPIWRITEBYTE("SLOWLOOP_CONFIG_REG",
                          slowLoopConfigRegAddr,
                          SLOWLOOP_CONFIG_REG_VAL); /* Disables fast loop recovery so that the threshold exceeded cntr updates correctly */
    ADRV9025_SPIFIELDWRITE(device,
                           tiaValidOverrideRegAddr,
                           ADI_TRUE,
                           TIA_VALID_OVERRIDE_MASK,
                           TIA_VALID_OVERRIDE_SHIFT,
                           "TIA_VALID_OVERWRITE"); /* Set this bit to latch new threshold values */
    ADRV9025_SPIFIELDWRITE(device,
                           tiaValidOverrideRegAddr,
                           ADI_FALSE,
                           TIA_VALID_OVERRIDE_MASK,
                           TIA_VALID_OVERRIDE_SHIFT,
                           "TIA_VALID_OVERWRITE"); /* Clear this bit to latch new threshold values */

    return (device->common.error.newAction);
}
