/**
 * \file
 * \brief Functions for FPGA910X Synchronous Serial Interface (SSI) configuration
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
 * Copyright 2022 Analog Devices Inc.
 * Released under the FPGA910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "adi_common_types.h"
#include "adi_fpga910x_ssi.h"
#include "adi_fpga910x_hal.h"
#include "axi_adrv9102.h"
#include "axi_adrv9102_top_regs.h"
#include "fpga910x_utilities.h"

static __maybe_unused int32_t populateSsiParam(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_Port_e port,
                                               uint32_t interfaceRate_Hz,
                                               uint32_t sampleRate_Hz,
                                               adi_fpga910x_SsiConfig_t *ssiConfig,
                                               axi_adrv9102_ssi_params_t *param)
{
    bool ssi_ddr = false;
    uint32_t ss_ratio = 0;
    param->gpio = 0;
    param->tx_clk_msb = 0;
    param->tx_buf_enable = 0x3f;
    param->data_rate = 0;
    param->bit_order = 0;
    param->iq_order = 0;
    param->edge = 0;
    param->ss_ratio = 0;
    param->tx_ss_mode = 0;
    param->orx_ss_ratio = 0;
    
    param->tx_clk_sel = 0;
    if (ssiConfig->txRefClockPin == 0)
    {
        param->tx_clk_sel = 1;
    }

    if (!(0 == interfaceRate_Hz) && !(0 == sampleRate_Hz) && (port == ADI_TX))
    {
        if (sampleRate_Hz <= interfaceRate_Hz)
        {
                int y = interfaceRate_Hz / sampleRate_Hz;
                while (y >>= 1) ++ss_ratio;
        }
        param->ss_ratio = ss_ratio;
        param->tx_ss_mode = 1;
    }
    
    ssi_ddr = ssiConfig->ddrEn;
    param->data_rate = (ssi_ddr == true) ? AXI_ADRV9102_SSI_DATA_RATE_DDR : AXI_ADRV9102_SSI_DATA_RATE_SDR;
    param->mode = ((uint32_t)ssiConfig->ssiType) & 0x1;
    param->num_of_lanes = (uint32_t) ssiConfig->numLaneSel;
    param->strb_type = (ssiConfig->strobeType) ? AXI_ADRV9102_SSI_STRB_TYPE_LEVEL : AXI_ADRV9102_SSI_STRB_TYPE_PULSE;
    
    switch (ssiConfig->ssiDataFormatSel)
    {
    case ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_02;
        break;
    case ADI_FPGA910X_SSI_FORMAT_8_BIT_SYMBOL_DATA:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_08;
        break;
    case ADI_FPGA910X_SSI_FORMAT_16_BIT_SYMBOL_DATA:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_16;
        break;
    case ADI_FPGA910X_SSI_FORMAT_12_BIT_I_Q_DATA:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_24;
        break;
    case ADI_FPGA910X_SSI_FORMAT_16_BIT_I_Q_DATA:
    case ADI_FPGA910X_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_32;
        break;
    case ADI_FPGA910X_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX:
        param->num_of_bits = AXI_ADRV9102_SSI_NUM_OF_BITS_64;
        break;
    default:
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_API_FAIL,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         0,
                         "unrecognized SSI data format type");
        ADI_API_RETURN(fpga910x);   
    };

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Calibrate(adi_fpga910x_Device_t *fpga910x,
                                   adi_common_Port_e port,
                                   adi_common_ChannelNumber_e channel,
                                   uint32_t *delay,
                                   uint32_t *sweep)
{
    uint32_t ssi_id = 0;

    ssi_id = fpga910x_SsiIdGet(fpga910x, port, channel);
    *sweep = (uint32_t) -1;
    if (axi_adrv9102_ssi_delay_calibrate((void *)fpga910x, AXI_ADRV9102_ID, ssi_id, 0, delay) != 0)
    {
            ADI_ERROR_REPORT(&fpga910x->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
                0,
                "function axi_adrv9102_ssi_calibrate has failed");
            ADI_API_RETURN(fpga910x);
    }
    axi_adrv9102_ssi_delay_sweep_get((void *)fpga910x, AXI_ADRV9102_ID, ssi_id, sweep);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_TxClockPolarity_Set_Validate(adi_fpga910x_Device_t *fpga910x,
                                                             adi_common_ChannelNumber_e channel)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_TxClockPolarity_Set(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool inverted)
{
    uint32_t ssiId = 0;
    uint32_t tx_clk_msb = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_TxClockPolarity_Set_Validate, fpga910x, channel);
    
    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_TX, channel);
    if (ssiId == (uint32_t) - 1) return (-1);

    tx_clk_msb = (inverted) ? 1 : 0;
    axi_adrv9102_ssi_clk_msb_set(fpga910x, AXI_ADRV9102_ID, ssiId, tx_clk_msb);

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_TxClockPolarity_Get_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                            adi_common_ChannelNumber_e channel,
                                                                            bool *inverted)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, inverted);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_TxClockPolarity_Get(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool *inverted)
{
    uint32_t ssiId = 0;
    uint32_t data = 0;
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_TxClockPolarity_Get_Validate, fpga910x, channel, inverted);
    
    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_TX, channel);
    if (ssiId == (uint32_t) - 1) return (-1);

    axi_adrv9102_ssi_clk_msb_get(fpga910x, AXI_ADRV9102_ID, ssiId, &data);
    *inverted = (data == 0) ? false : true;

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                   adi_common_Port_e port,
                                                   adi_common_ChannelNumber_e channel,
                                                   adi_fpga910x_SsiConfig_t *ssiCfg)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_TX);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiCfg);
    ADI_RANGE_CHECK(fpga910x, ssiCfg->ssiType, ADI_FPGA910X_SSI_TYPE_DISABLE, ADI_FPGA910X_SSI_TYPE_LVDS);
    ADI_RANGE_CHECK(fpga910x, ssiCfg->ssiDataFormatSel, ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA, ADI_FPGA910X_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX);
    ADI_RANGE_CHECK(fpga910x, ssiCfg->numLaneSel, ADI_FPGA910X_SSI_1_LANE, ADI_FPGA910X_SSI_4_LANE);
    ADI_RANGE_CHECK(fpga910x, ssiCfg->strobeType, ADI_FPGA910X_SSI_SHORT_STROBE, ADI_FPGA910X_SSI_LONG_STROBE);
    if ((ssiCfg->ssiType == ADI_FPGA910X_SSI_TYPE_CMOS) &&
        (ssiCfg->ssiDataFormatSel == ADI_FPGA910X_SSI_FORMAT_12_BIT_I_Q_DATA))
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            0,
            "Unsupported interface mode. 12 bit I/Q is not supported in CMOS.");
        ADI_API_RETURN(fpga910x);
    }
    if ((ssiCfg->ssiType == ADI_FPGA910X_SSI_TYPE_CMOS) &&
        (ssiCfg->numLaneSel == ADI_FPGA910X_SSI_2_LANE))
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            0,
            "Unsupported interface mode. Only 1 lane and 4 lanes are supported in CMOS.");
        ADI_API_RETURN(fpga910x);
    }
    if ((ssiCfg->ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) &&
        (ssiCfg->numLaneSel == ADI_FPGA910X_SSI_4_LANE))
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
            0,
            "Unsupported interface mode. Only 1 lane and 2 lanes are supported in LVDS.");
        ADI_API_RETURN(fpga910x);
    }
    if (ssiCfg->ssiType == ADI_FPGA910X_SSI_TYPE_LVDS)
    {
        if ((ssiCfg->ssiDataFormatSel == ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA) ||
            (ssiCfg->ssiDataFormatSel == ADI_FPGA910X_SSI_FORMAT_8_BIT_SYMBOL_DATA) ||
            (ssiCfg->ssiDataFormatSel == ADI_FPGA910X_SSI_FORMAT_16_BIT_SYMBOL_DATA))
        {
            ADI_ERROR_REPORT(&fpga910x->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
                0,
                "Unsupported interface mode. Only 12 bit and 16 bit I/Q data are supported in LVDS.");
            ADI_API_RETURN(fpga910x);
        }
    }
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Configure(adi_fpga910x_Device_t *fpga910x,
                                   adi_common_Port_e port,
                                   adi_common_ChannelNumber_e channel,
                                   uint32_t interfaceRate_Hz,
                                   uint32_t sampleRate_Hz,
                                   adi_fpga910x_SsiConfig_t *ssiConfig)
{
    uint32_t ssiId = 0;
    axi_adrv9102_ssi_params_t param = { 0 };

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Configure_Validate, fpga910x, port, channel, ssiConfig);

    /* No action taken if ADI_FPGA910X_SSI_TYPE_DISABLE */
    if((ADI_FPGA910X_SSI_TYPE_CMOS == ssiConfig->ssiType) ||
       (ADI_FPGA910X_SSI_TYPE_LVDS == ssiConfig->ssiType))
    {
        ssiId = fpga910x_SsiIdGet(fpga910x, port, channel);
        if (ssiId == (uint32_t) - 1) return (-1);

        ADI_EXPECT(populateSsiParam, fpga910x, port, interfaceRate_Hz, sampleRate_Hz, ssiConfig, &param);
        if (axi_adrv9102_ssi_config((void *)fpga910x, AXI_ADRV9102_ID, ssiId, &param) != 0)
        {
            ADI_ERROR_REPORT(&fpga910x->common,
                ADI_COMMON_ERRSRC_API,
                ADI_COMMON_ERR_API_FAIL,
                ADI_COMMON_ACT_ERR_API_NOT_IMPLEMENTED,
                0,
                "function axi_adrv9102_ssi_config has failed");
            ADI_API_RETURN(fpga910x);
        }
    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Inspect_Validate(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_Port_e port,
                                                  adi_common_ChannelNumber_e channel,
                                                  adi_fpga910x_SsiConfig_t *ssiCfg)
{
    ADI_RANGE_CHECK(fpga910x, port, ADI_RX, ADI_ELB);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiCfg);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t readSsiControlData(adi_fpga910x_Device_t *fpga910x,
                                  adi_common_Port_e port,
                                  adi_common_ChannelNumber_e channel,
                                  uint32_t *read_data)
{
    uint32_t ssiId = 0;
    adi_common_Port_e port_hack = ADI_RX;

    /* hack to keep some tests to happy. */

    port_hack = port;

    if ((port == ADI_ORX) || (port == ADI_ILB) || (port == ADI_ELB)) {
        port_hack = ADI_RX;
    }

    ssiId = fpga910x_SsiIdGet(fpga910x, port_hack, channel);
    if (ssiId == (uint32_t) - 1) return (-1);

    axi_adrv9102_ssi_reg_control_get((void *)fpga910x, AXI_ADRV9102_ID, ssiId, read_data);

    ADI_API_RETURN(fpga910x);
}
int32_t adi_fpga910x_ssi_Inspect(adi_fpga910x_Device_t *fpga910x,
                                 adi_common_Port_e port,
                                 adi_common_ChannelNumber_e channel,
                                 adi_fpga910x_SsiConfig_t *ssiCfg)
{
    uint32_t ssiId = 0;
    uint32_t value = 0;
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Inspect_Validate, fpga910x, port, channel, ssiCfg);
    ssiId = fpga910x_SsiIdGet(fpga910x, port, channel);
    
    axi_adrv9102_ssi_cmos1_lvds0_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    ssiCfg->ssiType = (value == AXI_ADRV9102_SSI_MODE_CMOS) ? ADI_FPGA910X_SSI_TYPE_CMOS : ADI_FPGA910X_SSI_TYPE_LVDS;
    
    axi_adrv9102_ssi_num_of_bits_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    ssiCfg->ssiDataFormatSel = (adi_fpga910x_SsiFormat_e)value;
    
    axi_adrv9102_ssi_lstrb1_pstrb0_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    ssiCfg->strobeType = (adi_fpga910x_SsiStrobeType_e)value;
    
    axi_adrv9102_ssi_sdr1_ddr0_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    ssiCfg->ddrEn = (value == 0) ? true : false;

    axi_adrv9102_ssi_num_of_lanes_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    if (value)
    {
        if (ADI_FPGA910X_SSI_TYPE_CMOS == ssiCfg->ssiType)
        {
            ssiCfg->numLaneSel = ADI_FPGA910X_SSI_4_LANE;
        }
        else
        {
            ssiCfg->numLaneSel = ADI_FPGA910X_SSI_2_LANE;
        }
    }
    else
    {
        ssiCfg->numLaneSel = ADI_FPGA910X_SSI_1_LANE;
    }

    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_BytesPerSample_GetValidate(adi_fpga910x_Device_t *fpga910x, uint8_t *bytesPerSample)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, bytesPerSample);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_BytesPerSample_Get(adi_fpga910x_Device_t *fpga910x,
                                            adi_common_Port_e port,
                                            adi_common_ChannelNumber_e channel,
                                            uint8_t *bytesPerSample)
{
    uint32_t ssiId = 0;
    uint32_t value = 0;
    adi_fpga910x_SsiFormat_e format = ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_BytesPerSample_GetValidate, fpga910x, bytesPerSample);
    
    ssiId = fpga910x_SsiIdGet(fpga910x, port, channel);
    axi_adrv9102_ssi_num_of_bits_get(fpga910x, AXI_ADRV9102_ID, ssiId, &value);
    format = (adi_fpga910x_SsiFormat_e)value;    
    
    switch (format)
    {
    case ADI_FPGA910X_SSI_FORMAT_2_BIT_SYMBOL_DATA:     /* Falls through */
    case ADI_FPGA910X_SSI_FORMAT_8_BIT_SYMBOL_DATA:
        *bytesPerSample = 1;
        break;
    case ADI_FPGA910X_SSI_FORMAT_16_BIT_SYMBOL_DATA:
        *bytesPerSample = 2;
        break;
    case ADI_FPGA910X_SSI_FORMAT_12_BIT_I_Q_DATA:
        *bytesPerSample = 3;
        break;
    case ADI_FPGA910X_SSI_FORMAT_16_BIT_I_Q_DATA:
    case ADI_FPGA910X_SSI_FORMAT_15_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE:
        *bytesPerSample = 4;
        break;
    case ADI_FPGA910X_SSI_FORMAT_22_BIT_I_Q_DATA_1_BIT_GAIN_CHANGE_8_BIT_GAIN_INDEX:
        *bytesPerSample = 8;
        break;
    default:
        ADI_SHOULD_NOT_EXECUTE(fpga910x);
    }

    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Reset(adi_fpga910x_Device_t *fpga910x)
{
    axi_adrv9102_ssi_init_set((void *)fpga910x, AXI_ADRV9102_ID, fpga910x_SsiIdGet(fpga910x, ADI_RX, ADI_CHANNEL_1), 0x1);
    axi_adrv9102_ssi_init_set((void *)fpga910x, AXI_ADRV9102_ID, fpga910x_SsiIdGet(fpga910x, ADI_RX, ADI_CHANNEL_1), 0x1);
    axi_adrv9102_ssi_init_set((void *)fpga910x, AXI_ADRV9102_ID, fpga910x_SsiIdGet(fpga910x, ADI_TX, ADI_CHANNEL_1), 0x1);
    ADI_API_RETURN(fpga910x);
}

static uint32_t fpga910x_SsiDataSelGet(adi_fpga910x_Device_t *fpga910x,
                                       adi_fpga910x_SsiTestModeData_e sel)
{
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_NORMAL) return(AXI_ADRV9102_SSI_DATA_SEL_DMA);
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_FIXED_PATTERN) return(AXI_ADRV9102_SSI_DATA_SEL_PATTERN);
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_RAMP_NIBBLE) return(AXI_ADRV9102_SSI_DATA_SEL_NIBBLE_RAMP);
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_RAMP_16_BIT) return(AXI_ADRV9102_SSI_DATA_SEL_RAMP);
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_PRBS15) return(AXI_ADRV9102_SSI_DATA_SEL_PRBS15);
    if (sel == ADI_FPGA910X_SSI_TESTMODE_DATA_PRBS7) return(AXI_ADRV9102_SSI_DATA_SEL_PRBS7);

    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, sel, "Invalid SSI data select.");
    return((uint32_t) -1);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Rx_TestMode_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                               adi_common_ChannelNumber_e channel,
                                                               adi_fpga910x_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_RANGE_CHECK(fpga910x, ssiTestModeConfig->testData, ADI_FPGA910X_SSI_TESTMODE_DATA_NORMAL, ADI_FPGA910X_SSI_TESTMODE_DATA_PRBS7);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Rx_TestMode_Configure(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_RxSsiTestModeCfg_t *ssiTestModeConfig)
{
    uint32_t ssiId = 0;
    uint64_t pattern;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Rx_TestMode_Configure_Validate, fpga910x, channel, ssiTestModeConfig);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_RX, channel);
    if (ssiId == (uint32_t) -1) return(-1);

    pattern = (uint64_t) ssiTestModeConfig->fixedDataPatternToCheck;
    pattern = (pattern << 32) | ssiTestModeConfig->fixedDataPatternToCheck;

    axi_adrv9102_ssi_data_pattern_set((void *)fpga910x, AXI_ADRV9102_ID, ssiId, pattern);
    axi_adrv9102_ssi_data_sel_set((void *)fpga910x, AXI_ADRV9102_ID, ssiId, fpga910x_SsiDataSelGet(fpga910x, ssiTestModeConfig->testData));
    axi_adrv9102_ssi_timer_delay_us((void *)fpga910x, AXI_ADRV9102_ID, ssiId, 1);
    axi_adrv9102_ssi_reg_status_set((void *)fpga910x, AXI_ADRV9102_ID, ssiId, (uint32_t) -1);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Rx_TestMode_Inspect_Validate(adi_fpga910x_Device_t *fpga910x,
                                                             adi_common_ChannelNumber_e channel,
                                                             bool *dataError)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, dataError);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Rx_TestMode_Inspect(adi_fpga910x_Device_t *fpga910x,
                                             adi_common_ChannelNumber_e channel,
                                             bool *dataError)
{
    uint32_t ssiId;
    uint32_t ssi_status = -1;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Rx_TestMode_Inspect_Validate, fpga910x, channel, dataError);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_RX, channel);
    if (ssiId == (uint32_t) -1) return(-1);

    axi_adrv9102_ssi_reg_status_get((void *)fpga910x, AXI_ADRV9102_ID, ssiId, &ssi_status);
    *dataError = (ssi_status == 0) ? false : true;
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Tx_TestMode_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                               adi_common_ChannelNumber_e channel,
                                                               adi_fpga910x_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiTestModeConfig);
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Tx_TestMode_Configure(adi_fpga910x_Device_t *fpga910x,
                                               adi_common_ChannelNumber_e channel,
                                               adi_fpga910x_TxSsiTestModeCfg_t *ssiTestModeConfig)
{
    uint32_t ssiId;
    uint64_t pattern;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Tx_TestMode_Configure_Validate, fpga910x, channel, ssiTestModeConfig);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_TX, channel);
    if (ssiId == (uint32_t) -1) return(-1);

    pattern = (uint64_t) ssiTestModeConfig->fixedDataPatternToTransmit;

    axi_adrv9102_ssi_data_pattern_set((void *)fpga910x, AXI_ADRV9102_ID, ssiId, pattern);
    axi_adrv9102_ssi_data_sel_set((void *)fpga910x, AXI_ADRV9102_ID, ssiId, fpga910x_SsiDataSelGet(fpga910x, ssiTestModeConfig->testData));
    axi_adrv9102_ssi_timer_delay_us((void *)fpga910x, AXI_ADRV9102_ID, ssiId, 1);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Delay_Configure_Validate(adi_fpga910x_Device_t *fpga910x,
                                                         adi_fpga910x_SsiType_e ssiType,
                                                         adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    static const uint8_t MAX_DELAY = 31;

    ADI_RANGE_CHECK(fpga910x, ssiType, ADI_FPGA910X_SSI_TYPE_CMOS, ADI_FPGA910X_SSI_TYPE_LVDS);
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiCalibration);

    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxStrobeDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxStrobeDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxIDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxIDataDelay[1], 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxQDataDelay[0], 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->rxQDataDelay[1], 0, MAX_DELAY);
    
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->txClkDelay, 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->txStrobeDelay, 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->txIDataDelay, 0, MAX_DELAY);
    ADI_RANGE_CHECK(fpga910x, ssiCalibration->txQDataDelay, 0, MAX_DELAY);

    ADI_API_RETURN(fpga910x);
}

/* we have a very badly thought out structure here, parameters must be grouped within interface */
/* if we grouped them by interface, all the things below could have been just a single function */

static void fpga910x_SsiDelaySet_1(adi_fpga910x_Device_t *fpga910x,
                                      adi_common_Port_e port,
                                      adi_common_ChannelNumber_e channel,
                                      uint32_t *data)
{
    uint32_t i = 0;

    for (i = 0; i < 8; i++) {
        axi_adrv9102_ssi_wrdelay_set((void*)fpga910x, AXI_ADRV9102_ID, fpga910x_SsiIdGet(fpga910x, port, channel), i, *(data + i));
    }
}

static void fpga910x_SsiDelaySet(adi_fpga910x_Device_t *fpga910x,
                                 adi_fpga910x_SsiType_e ssiType,
                                 adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    uint32_t ssiDelay[8];
    uint32_t ssiId = 0;

    ssiDelay[0] = ssiCalibration->rxIDataDelay[0];
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->rxQDataDelay[0];
    else
        ssiDelay[1] = ssiCalibration->rxIDataDelay[0];
    ssiDelay[2] = ssiCalibration->rxQDataDelay[0];
    ssiDelay[3] = ssiCalibration->rxQDataDelay[0];
    ssiDelay[4] = ssiCalibration->rxStrobeDelay[0];
    ssiDelay[5] = 0;
    ssiDelay[6] = 0;
    ssiDelay[7] = 0;
    fpga910x_SsiDelaySet_1(fpga910x, ADI_RX, ADI_CHANNEL_1, &ssiDelay[0]);

    ssiDelay[0] = ssiCalibration->rxIDataDelay[1];
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->rxQDataDelay[1];
    else
        ssiDelay[1] = ssiCalibration->rxIDataDelay[1];
    ssiDelay[2] = ssiCalibration->rxQDataDelay[1];
    ssiDelay[3] = ssiCalibration->rxQDataDelay[1];
    ssiDelay[4] = ssiCalibration->rxStrobeDelay[1];
    ssiDelay[5] = 0;
    ssiDelay[6] = 0;
    ssiDelay[7] = 0;
    fpga910x_SsiDelaySet_1(fpga910x, ADI_RX, ADI_CHANNEL_2, &ssiDelay[0]);

    ssiDelay[0] = ssiCalibration->txIDataDelay;
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiDelay[1] = ssiCalibration->txQDataDelay;
    else
        ssiDelay[1] = ssiCalibration->txIDataDelay;
    ssiDelay[2] = ssiCalibration->txQDataDelay;
    ssiDelay[3] = ssiCalibration->txQDataDelay;
    ssiDelay[4] = ssiCalibration->txStrobeDelay;
    ssiDelay[5] = ssiCalibration->txClkDelay;
    ssiDelay[6] = 0;
    ssiDelay[7] = ssiCalibration->txClkDelay;
    fpga910x_SsiDelaySet_1(fpga910x, ADI_TX, ADI_CHANNEL_1, &ssiDelay[0]);
}

int32_t adi_fpga910x_ssi_Delay_Configure(adi_fpga910x_Device_t *fpga910x,
                                         adi_fpga910x_SsiType_e ssiType,
                                         adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Delay_Configure_Validate, fpga910x, ssiType, ssiCalibration);
    fpga910x_SsiDelaySet(fpga910x, ssiType, ssiCalibration);
    ADI_EXPECT(adi_fpga910x_ssi_TxClockPolarity_Set, fpga910x, ADI_CHANNEL_1, ssiCalibration->txClkInverted);
    ADI_API_RETURN(fpga910x);
}

static void fpga910x_SsiDelayGet_1(adi_fpga910x_Device_t *fpga910x,
                                      adi_common_Port_e port,
                                      adi_common_ChannelNumber_e channel,
                                      uint32_t *data)
{
    int32_t i;

    for (i = 0; i < 6; i++) {
        axi_adrv9102_ssi_wrdelay_get((void*)fpga910x, AXI_ADRV9102_ID, fpga910x_SsiIdGet(fpga910x, port, channel), i, (data + i));
        *(data + i) = *(data + i) & 0xff;
    }
}

static void fpga910x_SsiDelayGet(adi_fpga910x_Device_t *fpga910x,
                                 adi_fpga910x_SsiType_e ssiType,
                                 adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    uint32_t ssiDelay[6];

    fpga910x_SsiDelayGet_1(fpga910x, ADI_RX, ADI_CHANNEL_1, &ssiDelay[0]);
    ssiCalibration->rxIDataDelay[0] = ssiDelay[0];
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiCalibration->rxQDataDelay[0] = ssiDelay[1];
    else
        ssiCalibration->rxQDataDelay[0] = ssiDelay[2];
    ssiCalibration->rxStrobeDelay[0] = ssiDelay[4];

    fpga910x_SsiDelayGet_1(fpga910x, ADI_RX, ADI_CHANNEL_2, &ssiDelay[0]);
    ssiCalibration->rxIDataDelay[1] = ssiDelay[0];
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiCalibration->rxQDataDelay[1] = ssiDelay[1];
    else
        ssiCalibration->rxQDataDelay[1] = ssiDelay[2];
    ssiCalibration->rxStrobeDelay[1] = ssiDelay[4];

    fpga910x_SsiDelayGet_1(fpga910x, ADI_TX, ADI_CHANNEL_1, &ssiDelay[0]);
    ssiCalibration->txIDataDelay = ssiDelay[0];
    if (ssiType == ADI_FPGA910X_SSI_TYPE_LVDS) 
        ssiCalibration->txQDataDelay = ssiDelay[1];
    else
        ssiCalibration->txQDataDelay = ssiDelay[2];
    ssiCalibration->txStrobeDelay = ssiDelay[4];
    ssiCalibration->txClkDelay = ssiDelay[5];
}

static __maybe_unused int32_t adi_fpga910x_ssi_Delay_Inspect_Validate(adi_fpga910x_Device_t *fpga910x,
                                                       adi_fpga910x_SsiType_e ssiType,
                                                       adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_RANGE_CHECK(fpga910x, ssiType, ADI_FPGA910X_SSI_TYPE_CMOS, ADI_FPGA910X_SSI_TYPE_LVDS);
    ADI_NULL_PTR_RETURN(&fpga910x->common, ssiCalibration);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Delay_Inspect(adi_fpga910x_Device_t *fpga910x,
                                       adi_fpga910x_SsiType_e ssiType,
                                       adi_fpga910x_SsiCalibrationCfg_t *ssiCalibration)
{
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Delay_Inspect_Validate, fpga910x, ssiType, ssiCalibration);
    fpga910x_SsiDelayGet(fpga910x, ssiType, ssiCalibration);
    ADI_EXPECT(adi_fpga910x_ssi_TxClockPolarity_Get, fpga910x, ADI_CHANNEL_1, &ssiCalibration->txClkInverted);
    ADI_API_RETURN(fpga910x);
}

static __maybe_unused int32_t adi_fpga910x_ssi_Delay_Calibrate_Validate(adi_fpga910x_Device_t *fpga910x,
                                                                        adi_common_ChannelNumber_e channel,
                                                                        uint32_t *ssiDelay)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    ADI_NULL_PTR_RETURN(fpga910x, ssiDelay);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_Delay_Calibrate(adi_fpga910x_Device_t *fpga910x,
                                         adi_common_ChannelNumber_e channel,
                                         uint32_t *ssiDelay)
{
    uint32_t ssiId = 0;
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_Delay_Calibrate_Validate, fpga910x, channel, ssiDelay);
 

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_RX, channel);
    
    ADI_EXPECT(axi_adrv9102_ssi_delay_calibrate, fpga910x, AXI_ADRV9102_ID, ssiId, 1, ssiDelay);
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_TxSs_Ratio_Set_Validate(adi_fpga910x_Device_t *fpga910x,
                                                 adi_common_ChannelNumber_e channel,
                                                 uint32_t interfaceRate_Hz,
                                                 uint32_t sampleRate_Hz)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    
    if (0 == interfaceRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            interfaceRate_Hz,
            "interfaceRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    if (0 == sampleRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            sampleRate_Hz,
            "sampleRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_TxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_ChannelNumber_e channel,
                                        uint32_t interfaceRate_Hz,
                                        uint32_t sampleRate_Hz)
{
    uint32_t ssiId = 0;
    uint32_t ss_ratio = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_TxSs_Ratio_Set_Validate, fpga910x, channel, interfaceRate_Hz, sampleRate_Hz);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_TX, channel);
   
    if (sampleRate_Hz <= interfaceRate_Hz)
    {
        int y = interfaceRate_Hz / sampleRate_Hz;
        while (y >>= 1) ++ss_ratio;
    }
    
    axi_adrv9102_ssi_ss_ratio_set(fpga910x, AXI_ADRV9102_ID, ssiId, ss_ratio);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_TxSs_Mode_Set(adi_fpga910x_Device_t *fpga910x,
                                       adi_common_ChannelNumber_e channel,
                                       uint32_t data)
{
    uint32_t ssiId = 0;

    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_TX, channel);
    axi_adrv9102_ssi_ss_mode_set(fpga910x, AXI_ADRV9102_ID, ssiId, data);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_RxSs_Ratio_Set_Validate(adi_fpga910x_Device_t *fpga910x,
                                                 adi_common_ChannelNumber_e channel,
                                                 uint32_t interfaceRate_Hz,
                                                 uint32_t sampleRate_Hz)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    
    if (0 == interfaceRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            interfaceRate_Hz,
            "interfaceRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    if (0 == sampleRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            sampleRate_Hz,
            "sampleRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_RxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                        adi_common_ChannelNumber_e channel,
                                        uint32_t interfaceRate_Hz,
                                        uint32_t sampleRate_Hz)
{
    uint32_t ssiId = 0;
    uint32_t ss_ratio = 0;

    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_RxSs_Ratio_Set_Validate, fpga910x, channel, interfaceRate_Hz, sampleRate_Hz);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_RX, channel);
    axi_adrv9102_ssi_ss_ratio_set(fpga910x, AXI_ADRV9102_ID, ssiId, ss_ratio);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_OrxSs_Ratio_Set_Validate(adi_fpga910x_Device_t *fpga910x,
                                                  adi_common_ChannelNumber_e channel,
                                                  uint32_t interfaceRate_Hz,
                                                  uint32_t sampleRate_Hz)
{
    ADI_RANGE_CHECK(fpga910x, channel, ADI_CHANNEL_1, ADI_CHANNEL_2);
    
    if (0 == interfaceRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            interfaceRate_Hz,
            "interfaceRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    if (0 == sampleRate_Hz)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_API_FAIL,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            sampleRate_Hz,
            "sampleRate_Hz cannot be 0");
        ADI_API_RETURN(fpga910x);   
    }
    
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_OrxSs_Ratio_Set(adi_fpga910x_Device_t *fpga910x,
                                         adi_common_ChannelNumber_e channel,
                                         uint32_t interfaceRate_Hz,
                                         uint32_t sampleRate_Hz)
{
    uint32_t ssiId = 0;
    uint32_t orx_ss_ratio = 0;
    
    ADI_PERFORM_VALIDATION(adi_fpga910x_ssi_OrxSs_Ratio_Set_Validate, fpga910x, channel, interfaceRate_Hz, sampleRate_Hz);

    ssiId = fpga910x_SsiIdGet(fpga910x, ADI_ORX, channel);
    axi_adrv9102_ssi_ss_ratio_set(fpga910x, AXI_ADRV9102_ID, ssiId, orx_ss_ratio);
    ADI_API_RETURN(fpga910x);
}

int32_t adi_fpga910x_ssi_ConvertSsiConfig(adi_fpga910x_Device_t *fpga910x,
	                                      ssiConfig_t *profileConfig, 
	                                      adi_fpga910x_SsiConfig_t *fpgaConfig)
{
	fpgaConfig->ssiType = (adi_fpga910x_SsiType_e)profileConfig->ssiType ;
	fpgaConfig->ssiDataFormatSel = (adi_fpga910x_SsiFormat_e)profileConfig->ssiDataFormatSel;
	fpgaConfig->numLaneSel = (adi_fpga910x_SsiNumLane_e)profileConfig->numLaneSel;
	fpgaConfig->strobeType = (adi_fpga910x_SsiStrobeType_e)profileConfig->strobeType;
	fpgaConfig->lsbFirst = profileConfig->lsbFirst;
	fpgaConfig->qFirst = profileConfig->qFirst;
	fpgaConfig->txRefClockPin = (adi_fpga910x_SsiTxRefClockPin_e)profileConfig->txRefClockPin;
	fpgaConfig->lvdsIBitInversion = profileConfig->lvdsBitInversion;
	fpgaConfig->lvdsQBitInversion = profileConfig->lvdsBitInversion;
	fpgaConfig->lvdsStrobeBitInversion = profileConfig->lvdsBitInversion;
	fpgaConfig->lvdsUseLsbIn12bitMode = profileConfig->lvdsUseLsbIn12bitMode;
	fpgaConfig->lvdsRxClkInversionEn = profileConfig->lvdsRxClkInversionEn;
	fpgaConfig->cmosDdrPosClkEn = profileConfig->cmosDdrPosClkEn;
	fpgaConfig->cmosClkInversionEn = profileConfig->cmosClkInversionEn;
	fpgaConfig->ddrEn = profileConfig->ddrEn;
	fpgaConfig->rxMaskStrobeEn = profileConfig->rxMaskStrobeEn;
	ADI_API_RETURN(fpga910x);
}
