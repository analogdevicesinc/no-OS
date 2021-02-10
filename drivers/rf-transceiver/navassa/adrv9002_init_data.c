/***************************************************************************//**
 *   @file   adrv9002_init_data.c
 *   @brief  adrv9002 initialization data.
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#include "adrv9002.h"
#include "adi_adrv9001_clockSettings_types.h"
#include "adi_adrv9001_deviceSysConfig_types.h"
#include "adi_adrv9001_pfirBuffer_types.h"
#include "adi_adrv9001_profile_types.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_types.h"
#include "adi_adrv9001_txSettings_types.h"
#include "parameters.h"

static struct adi_adrv9001_SpiSettings spiSettings = {
	.msbFirst = 1,
	.enSpiStreaming = 0,
	.autoIncAddrUp = 1,
	.fourWireMode = 1,
	.cmosPadDrvStrength = ADI_ADRV9001_CMOSPAD_DRV_STRONG,
};

struct adi_adrv9001_Init adrv9002_init_lvds = {
	.clocks = {
		.deviceClock_kHz = 38400,
		.clkPllVcoFreq_daHz = 884736000,
		.clkPllHsDiv = ADI_ADRV9001_HSDIV_4,
		.clkPllMode = ADI_ADRV9001_CLK_PLL_HP_MODE,
		.clk1105Div = ADI_ADRV9001_INTERNAL_CLOCK_DIV_2,
		.armClkDiv = ADI_ADRV9001_INTERNAL_CLOCK_DIV_6,
		.armPowerSavingClkDiv = 1,
		.refClockOutEnable = true,
		.auxPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
		.clkPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
		.padRefClkDrv = 0,
		.extLo1OutFreq_kHz = 0,
		.extLo2OutFreq_kHz = 0,
		.rfPll1LoMode = ADI_ADRV9001_INT_LO1,
		.rfPll2LoMode = ADI_ADRV9001_INT_LO1,
		.ext1LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL,
		.ext2LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL,
		.rx1RfInputSel = ADI_ADRV9001_RX_A,
		.rx2RfInputSel = ADI_ADRV9001_RX_A,
		.extLo1Divider = 2,
		.extLo2Divider = 2,
		.rfPllPhaseSyncMode = ADI_ADRV9001_RFPLLMCS_NOSYNC,
		.rx1LoSelect = ADI_ADRV9001_LOSEL_LO2,
		.rx2LoSelect = ADI_ADRV9001_LOSEL_LO2,
		.tx1LoSelect = ADI_ADRV9001_LOSEL_LO1,
		.tx2LoSelect = ADI_ADRV9001_LOSEL_LO1,
		.rx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.rx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.tx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.tx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.loGen1Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO,
		.loGen2Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO
	},
	.rx = {
		.rxInitChannelMask = 195,
		.rxChannelCfg = {
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 9000000,
					.rxOutputRate_Hz = 15360000,
					.rxInterfaceSampleRate_Hz = 15360000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 20000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 40000,
					.rxCorner3dBLp_kHz = 40000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 1,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 9000000,
					.rxOutputRate_Hz = 15360000,
					.rxInterfaceSampleRate_Hz = 15360000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 20000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 40000,
					.rxCorner3dBLp_kHz = 40000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 2,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_C,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 9000000,
					.rxOutputRate_Hz = 15360000,
					.rxInterfaceSampleRate_Hz = 15360000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 50000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 100000,
					.rxCorner3dBLp_kHz = 100000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 64,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_B,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 9000000,
					.rxOutputRate_Hz = 15360000,
					.rxInterfaceSampleRate_Hz = 15360000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 50000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 100000,
					.rxCorner3dBLp_kHz = 100000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 128,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_D,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			}
		}
	},
	.tx = {
		.txInitChannelMask = 12,
		.txProfile = {
			{
				.primarySigBandwidth_Hz = 9000000,
				.txInputRate_Hz = 15360000,
				.txInterfaceSampleRate_Hz = 15360000,
				.txOffsetLo_kHz = 0,
				.validDataDelay = 0,
				.txBbf3dBCorner_kHz = 50000,
				.outputSignaling = ADI_ADRV9001_TX_IQ,
				.txPdBiasCurrent = 1,
				.txPdGainEnable = 0,
				.txPrePdRealPole_kHz = 1000000,
				.txPostPdRealPole_kHz = 530000,
				.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
				.txExtLoopBackType = 0,
				.txExtLoopBackForInitCal = 0,
				.txPeakLoopBackPower = -180,
				.frequencyDeviation_Hz = 0,
				.txDpProfile =
				{
					.txPreProc =
					{
						.txPreProcSymbol0 = 0,
						.txPreProcSymbol1 = 0,
						.txPreProcSymbol2 = 0,
						.txPreProcSymbol3 = 0,
						.txPreProcSymMapDivFactor = 1,
						.txPreProcMode = 1,
						.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_A,
						.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_B
					},
					.txWbIntTop =
					{
						.txInterpBy2Blk30En = 0,
						.txInterpBy2Blk28En = 0,
						.txInterpBy2Blk26En = 0,
						.txInterpBy2Blk24En = 1,
						.txInterpBy2Blk22En = 1,
						.txWbLpfBlk22p1En = 0
					},
					.txNbIntTop =
					{
						.txInterpBy2Blk20En = 0,
						.txInterpBy2Blk18En = 0,
						.txInterpBy2Blk16En = 0,
						.txInterpBy2Blk14En = 0,
						.txInterpBy2Blk12En = 0,
						.txInterpBy3Blk10En = 0,
						.txInterpBy2Blk8En = 0,
						.txScicBlk32En = 0,
						.txScicBlk32DivFactor = 1
					},
					.txIntTop =
					{
						.interpBy3Blk44p1En = 1,
						.sinc3Blk44En = 0,
						.sinc2Blk42En = 0,
						.interpBy3Blk40En = 1,
						.interpBy2Blk38En = 0,
						.interpBy2Blk36En = 0
					},
					.txIntTopFreqDevMap =
					{
						.rrc2Frac = 0,
						.mpll = 0,
						.nchLsw = 0,
						.nchMsb = 0,
						.freqDevMapEn = 0,
						.txRoundEn = 1
					},
					.txIqdmDuc =
					{
						.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE0,
						.iqdmDev = 0,
						.iqdmDevOffset = 0,
						.iqdmScalar = 0,
						.iqdmThreshold = 0,
						.iqdmNco =
						{
							.freq = 0,
							.sampleFreq = 61440000,
							.phase = 0,
							.realOut = 0
						}
					}
				},
				.txSsiConfig =
				{
					.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
					.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
					.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
					.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
					.lsbFirst = 0,
					.qFirst = 0,
					.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED,
					.lvdsBitInversion = 0,
					.lvdsUseLsbIn12bitMode = 0,
					.lvdsTxFullRefClkEn = false,
					.lvdsRxClkInversionEn = false,
					.cmosTxDdrNegStrobeEn = false,
					.cmosDdrPosClkEn = false,
					.cmosDdrClkInversionEn = false,
					.cmosDdrEn = false,
					.rxMaskStrobeEn = false
				}
			},
			{
				.primarySigBandwidth_Hz = 9000000,
				.txInputRate_Hz = 15360000,
				.txInterfaceSampleRate_Hz = 15360000,
				.txOffsetLo_kHz = 0,
				.validDataDelay = 0,
				.txBbf3dBCorner_kHz = 50000,
				.outputSignaling = ADI_ADRV9001_TX_IQ,
				.txPdBiasCurrent = 1,
				.txPdGainEnable = 0,
				.txPrePdRealPole_kHz = 1000000,
				.txPostPdRealPole_kHz = 530000,
				.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
				.txExtLoopBackType = 0,
				.txExtLoopBackForInitCal = 0,
				.txPeakLoopBackPower = -180,
				.frequencyDeviation_Hz = 0,
				.txDpProfile =
				{
					.txPreProc =
					{
						.txPreProcSymbol0 = 0,
						.txPreProcSymbol1 = 0,
						.txPreProcSymbol2 = 0,
						.txPreProcSymbol3 = 0,
						.txPreProcSymMapDivFactor = 1,
						.txPreProcMode = 1,
						.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_C,
						.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_D
					},
					.txWbIntTop =
					{
						.txInterpBy2Blk30En = 0,
						.txInterpBy2Blk28En = 0,
						.txInterpBy2Blk26En = 0,
						.txInterpBy2Blk24En = 1,
						.txInterpBy2Blk22En = 1,
						.txWbLpfBlk22p1En = 0
					},
					.txNbIntTop =
					{
						.txInterpBy2Blk20En = 0,
						.txInterpBy2Blk18En = 0,
						.txInterpBy2Blk16En = 0,
						.txInterpBy2Blk14En = 0,
						.txInterpBy2Blk12En = 0,
						.txInterpBy3Blk10En = 0,
						.txInterpBy2Blk8En = 0,
						.txScicBlk32En = 0,
						.txScicBlk32DivFactor = 1
					},
					.txIntTop =
					{
						.interpBy3Blk44p1En = 1,
						.sinc3Blk44En = 0,
						.sinc2Blk42En = 0,
						.interpBy3Blk40En = 1,
						.interpBy2Blk38En = 0,
						.interpBy2Blk36En = 0
					},
					.txIntTopFreqDevMap =
					{
						.rrc2Frac = 0,
						.mpll = 0,
						.nchLsw = 0,
						.nchMsb = 0,
						.freqDevMapEn = 0,
						.txRoundEn = 1
					},
					.txIqdmDuc =
					{
						.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE0,
						.iqdmDev = 0,
						.iqdmDevOffset = 0,
						.iqdmScalar = 0,
						.iqdmThreshold = 0,
						.iqdmNco =
						{
							.freq = 0,
							.sampleFreq = 61440000,
							.phase = 0,
							.realOut = 0
						}
					}
				},
				.txSsiConfig =
				{
					.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS,
					.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
					.numLaneSel = ADI_ADRV9001_SSI_2_LANE,
					.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
					.lsbFirst = 0,
					.qFirst = 0,
					.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED,
					.lvdsBitInversion = 0,
					.lvdsUseLsbIn12bitMode = 0,
					.lvdsTxFullRefClkEn = false,
					.lvdsRxClkInversionEn = false,
					.cmosTxDdrNegStrobeEn = false,
					.cmosDdrPosClkEn = false,
					.cmosDdrClkInversionEn = false,
					.cmosDdrEn = false,
					.rxMaskStrobeEn = false
				}
			}
		}
	},
	.sysConfig = {
		.duplexMode = ADI_ADRV9001_FDD_MODE,
		.fhModeOn = 0,
		.numDynamicProfile = 1,
		.mcsMode = ADI_ADRV9001_MCSMODE_DISABLED,
		.adcTypeMonitor = ADI_ADRV9001_ADC_HP,
		.pllLockTime_us = 750,
		.pllModulus =
		{
			.modulus = { 8388593, 8388593, 8388593, 8388593, 8388593 },
			.dmModulus = { 8388593, 8388593 }
		}
	},
	.pfirBuffer = {
		.pfirRxWbNbChFilterCoeff_A = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 475, 312, -782, -39, 1201, -777, -1182, 1981, 177, -2874, 1941, 2393, -4416, 225, 5594, -4581, -3668, 8650, -1992, -9342, 9646, 4213, -15137, 6404, 13615,-18199, -2610, 23969, -15142, -17198, 31204, -3269, -34604, 30213, 17955, -49337, 16361, 45636, -53954, -12567, 72920, -40769, -54562, 89506, -4148, -102269, 83183, 57280,-142874, 41767, 139213, -158628, -45955, 231679, -125964, -193870, 320642, -4532, -442087, 390927, 347244, -1055854, 429729, 4391599, 4391599, 429729, -1055854, 347244, 390927,-442087, -4532, 320642, -193870, -125964, 231679, -45955, -158628, 139213, 41767, -142874, 57280, 83183, -102269, -4148, 89506, -54562, -40769, 72920, -12567, -53954, 45636,16361, -49337, 17955, 30213, -34604, -3269, 31204, -17198, -15142, 23969, -2610, -18199, 13615, 6404, -15137, 4213, 9646, -9342, -1992, 8650, -3668, -4581, 5594, 225, -4416,2393, 1941, -2874, 177, 1981, -1182, -777, 1201, -39, -782, 312, 0 }
		},
		.pfirRxWbNbChFilterCoeff_B = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxWbNbChFilterCoeff_C = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 475, 312, -782, -39, 1201, -777, -1182, 1981, 177, -2874, 1941, 2393, -4416, 225, 5594, -4581, -3668, 8650, -1992, -9342, 9646, 4213, -15137, 6404, 13615,-18199, -2610, 23969, -15142, -17198, 31204, -3269, -34604, 30213, 17955, -49337, 16361, 45636, -53954, -12567, 72920, -40769, -54562, 89506, -4148, -102269, 83183, 57280,-142874, 41767, 139213, -158628, -45955, 231679, -125964, -193870, 320642, -4532, -442087, 390927, 347244, -1055854, 429729, 4391599, 4391599, 429729, -1055854, 347244, 390927,-442087, -4532, 320642, -193870, -125964, 231679, -45955, -158628, 139213, 41767, -142874, 57280, 83183, -102269, -4148, 89506, -54562, -40769, 72920, -12567, -53954, 45636,16361, -49337, 17955, 30213, -34604, -3269, 31204, -17198, -15142, 23969, -2610, -18199, 13615, 6404, -15137, 4213, 9646, -9342, -1992, 8650, -3668, -4581, 5594, 225, -4416,2393, 1941, -2874, 177, 1981, -1182, -777, 1201, -39, -782, 312, 0 }
		},
		.pfirRxWbNbChFilterCoeff_D = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_A = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_B = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_C = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_D = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxNbPulShp = {
			{
				.numCoeff = 128,
				.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
				.taps = 128,
				.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 128,
				.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
				.taps = 128,
				.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		},
		.pfirRxMagLowTiaLowSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4351, 5890, -7135, 7006, 26378, 7006, -7135, 5890, -4351, 2883, -1686, 853, -355, 108, -20 }
			},
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4351, 5890, -7135, 7006, 26378, 7006, -7135, 5890, -4351, 2883, -1686, 853, -355, 108, -20 }
			}
		},
		.pfirRxMagLowTiaHighSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 461, -755, 469, 1314, -5308, 11130, -16594, 12348, 26906, 12348, -16594, 11130, -5308, 1314, 469, -755, 461, -166, 30 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 461, -755, 469, 1314, -5308, 11130, -16594, 12348, 26906, 12348, -16594, 11130, -5308, 1314, 469, -755, 461, -166, 30 }
			}
		},
		.pfirRxMagHighTiaHighSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 470, -899, 1050, -225, -2199, 6134, -10228, 10467, 23888, 10467, -10228, 6134, -2199, -225, 1050, -899, 470, -158, 28 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 470, -899, 1050, -225, -2199, 6134, -10228, 10467, 23888, 10467, -10228, 6134, -2199, -225, 1050, -899, 470, -158, 28 }
			}
		},
		.pfirRxMagLowTiaLowSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4350, 5890, -7135, 7010, 26372, 7010, -7135, 5890, -4350, 2883, -1686, 853, -355, 108, -20 }
			},
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4350, 5890, -7135, 7010, 26372, 7010, -7135, 5890, -4350, 2883, -1686, 853, -355, 108, -20 }
			}
		},
		.pfirRxMagLowTiaHighSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 462, -758, 479, 1293, -5268, 11073, -16531, 12321, 26900, 12321, -16531, 11073, -5268, 1293, 479, -758, 462, -166, 30 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 462, -758, 479, 1293, -5268, 11073, -16531, 12321, 26900, 12321, -16531, 11073, -5268, 1293, 479, -758, 462, -166, 30 }
			}
		},
		.pfirRxMagHighTiaHighSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 471, -900, 1054, -236, -2177, 6099, -10187, 10451, 23878, 10451, -10187, 6099, -2177, -236, 1054, -900, 471, -158, 28 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 471, -900, 1054, -236, -2177, 6099, -10187, 10451, 23878, 10451, -10187, 6099, -2177, -236, 1054, -900, 471, -158, 28 }
			}
		},
		.pfirTxMagComp1 = {
			.numCoeff = 21,
			.coefficients = { 8, -20, -56, 533, -1987, 5017, -9518, 13961, -13376, 6782, 30083, 6782, -13376, 13961, -9518, 5017, -1987, 533, -56, -20, 8 }
		},
		.pfirTxMagComp2 = {
			.numCoeff = 21,
			.coefficients = { 8, -20, -56, 533, -1987, 5017, -9518, 13961, -13376, 6782, 30083, 6782, -13376, 13961, -9518, 5017, -1987, 533, -56, -20, 8 }
		},
		.pfirTxMagCompNb = {
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		},
		.pfirRxMagCompNb = {
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		}
	}
};

/* different profile for CMOS - 1.4MHz BW */
static struct adi_adrv9001_Init adrv9002_init_cmos = {
	.clocks =  {
		.deviceClock_kHz = 38400,
		.clkPllVcoFreq_daHz = 884736000,
		.clkPllHsDiv = ADI_ADRV9001_HSDIV_4,
		.clkPllMode = ADI_ADRV9001_CLK_PLL_HP_MODE,
		.clk1105Div = ADI_ADRV9001_INTERNAL_CLOCK_DIV_2,
		.armClkDiv = ADI_ADRV9001_INTERNAL_CLOCK_DIV_6,
		.armPowerSavingClkDiv = 1,
		.refClockOutEnable = true,
		.auxPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
		.clkPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
		.padRefClkDrv = 0,
		.extLo1OutFreq_kHz = 0,
		.extLo2OutFreq_kHz = 0,
		.rfPll1LoMode = ADI_ADRV9001_INT_LO1,
		.rfPll2LoMode = ADI_ADRV9001_INT_LO1,
		.ext1LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL,
		.ext2LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL,
		.rx1RfInputSel = ADI_ADRV9001_RX_A,
		.rx2RfInputSel = ADI_ADRV9001_RX_A,
		.extLo1Divider = 2,
		.extLo2Divider = 2,
		.rfPllPhaseSyncMode = ADI_ADRV9001_RFPLLMCS_NOSYNC,
		.rx1LoSelect = ADI_ADRV9001_LOSEL_LO2,
		.rx2LoSelect = ADI_ADRV9001_LOSEL_LO2,
		.tx1LoSelect = ADI_ADRV9001_LOSEL_LO1,
		.tx2LoSelect = ADI_ADRV9001_LOSEL_LO1,
		.rx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.rx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.tx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.tx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER,
		.loGen1Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO,
		.loGen2Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO
	},
	.rx =  {
		.rxInitChannelMask = 195,
		.rxChannelCfg = {
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 1008000,
					.rxOutputRate_Hz = 1920000,
					.rxInterfaceSampleRate_Hz = 1920000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 20000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 40000,
					.rxCorner3dBLp_kHz = 40000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 1,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 1,
							.decBy2Blk27En = 1,
							.decBy2Blk29En = 1,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 1008000,
					.rxOutputRate_Hz = 1920000,
					.rxInterfaceSampleRate_Hz = 1920000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 20000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 40000,
					.rxCorner3dBLp_kHz = 40000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 2,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 1,
							.decBy2Blk27En = 1,
							.decBy2Blk29En = 1,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_C,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 1008000,
					.rxOutputRate_Hz = 1920000,
					.rxInterfaceSampleRate_Hz = 1920000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 50000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 100000,
					.rxCorner3dBLp_kHz = 100000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 64,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 1,
							.decBy2Blk27En = 1,
							.decBy2Blk29En = 1,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_B,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 1008000,
					.rxOutputRate_Hz = 1920000,
					.rxInterfaceSampleRate_Hz = 1920000,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 50000000,
					.lpAdcCorner = 0,
					.adcClk_kHz = 2211840,
					.rxCorner3dB_kHz = 100000,
					.rxCorner3dBLp_kHz = 100000,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 128,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 1,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 1,
							.decBy2Blk27En = 1,
							.decBy2Blk29En = 1,
							.decBy2Blk31En = 1,
							.decBy2Blk33En = 1,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 1,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_D,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			},
			{
				.profile =
				{
					.primarySigBandwidth_Hz = 12500,
					.rxOutputRate_Hz = 0,
					.rxInterfaceSampleRate_Hz = 0,
					.rxOffsetLo_kHz = 0,
					.rxSignalOnLo = 0,
					.outputSignaling = ADI_ADRV9001_RX_IQ,
					.filterOrder = 1,
					.filterOrderLp = 1,
					.hpAdcCorner = 0,
					.lpAdcCorner = 0,
					.adcClk_kHz = 0,
					.rxCorner3dB_kHz = 0,
					.rxCorner3dBLp_kHz = 0,
					.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
					.channelType = 0,
					.adcType = ADI_ADRV9001_ADC_HP,
					.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC,
					.rxDpProfile =
					{
						.rxNbDecTop =
						{
							.scicBlk23En = 0,
							.scicBlk23DivFactor = 0,
							.scicBlk23LowRippleEn = 0,
							.decBy2Blk35En = 0,
							.decBy2Blk37En = 0,
							.decBy2Blk39En = 0,
							.decBy2Blk41En = 0,
							.decBy2Blk43En = 0,
							.decBy3Blk45En = 0,
							.decBy2Blk47En = 0
						},
						.rxWbDecTop =
						{
							.decBy2Blk25En = 0,
							.decBy2Blk27En = 0,
							.decBy2Blk29En = 0,
							.decBy2Blk31En = 0,
							.decBy2Blk33En = 0,
							.wbLpfBlk33p1En = 0
						},
						.rxDecTop =
						{
							.decBy3Blk15En = 0,
							.decBy2Hb3Blk17p1En = 0,
							.decBy2Hb4Blk17p2En = 0,
							.decBy2Hb5Blk19p1En = 0,
							.decBy2Hb6Blk19p2En = 0
						},
						.rxSincHBTop =
						{
							.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB,
							.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO,
							.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1,
							.isGainCompEnabled = 0,
							.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384 },
							.gainComp9GainQ = { 0, 0, 0, 0, 0, 0 }
						},
						.rxNbDem =
						{
							.dpInFifo =
							{
								.dpInFifoEn = 0,
								.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING,
								.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL
							},
							.rxNbNco =
							{
								.rxNbNcoEn = 0,
								.rxNbNcoConfig =
								{
									.freq = 0,
									.sampleFreq = 0,
									.phase = 0,
									.realOut = 0
								}
							},
							.rxWbNbCompPFir =
							{
								.bankSel = ADI_ADRV9001_PFIR_BANK_A,
								.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN,
								.rxWbNbCompPFirEn = 1
							},
							.resamp =
							{
								.rxResampEn = 0,
								.resampPhaseI = 0,
								.resampPhaseQ = 0
							},
							.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS,
							.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL,
							.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ,
							.dpArmSel = ADI_ADRV9001_DP_SEL
						}
					},
					.rxSsiConfig =
					{
						.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE,
						.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA,
						.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
						.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
						.lsbFirst = 0,
						.qFirst = 0,
						.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED,
						.lvdsBitInversion = 0,
						.lvdsUseLsbIn12bitMode = 0,
						.lvdsTxFullRefClkEn = false,
						.lvdsRxClkInversionEn = false,
						.cmosTxDdrNegStrobeEn = false,
						.cmosDdrPosClkEn = false,
						.cmosDdrClkInversionEn = false,
						.cmosDdrEn = false,
						.rxMaskStrobeEn = false
					}
				}
			}
		}
	},
	.tx =  {
		.txInitChannelMask = 12,
		.txProfile = {
			{
				.primarySigBandwidth_Hz = 1008000,
				.txInputRate_Hz = 1920000,
				.txInterfaceSampleRate_Hz = 1920000,
				.txOffsetLo_kHz = 0,
				.validDataDelay = 0,
				.txBbf3dBCorner_kHz = 50000,
				.outputSignaling = ADI_ADRV9001_TX_IQ,
				.txPdBiasCurrent = 1,
				.txPdGainEnable = 0,
				.txPrePdRealPole_kHz = 1000000,
				.txPostPdRealPole_kHz = 530000,
				.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
				.txExtLoopBackType = 0,
				.txExtLoopBackForInitCal = 0,
				.txPeakLoopBackPower = -180,
				.frequencyDeviation_Hz = 0,
				.txDpProfile =
				{
					.txPreProc =
					{
						.txPreProcSymbol0 = 0,
						.txPreProcSymbol1 = 0,
						.txPreProcSymbol2 = 0,
						.txPreProcSymbol3 = 0,
						.txPreProcSymMapDivFactor = 1,
						.txPreProcMode = 1,
						.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_A,
						.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_B
					},
					.txWbIntTop =
					{
						.txInterpBy2Blk30En = 1,
						.txInterpBy2Blk28En = 1,
						.txInterpBy2Blk26En = 1,
						.txInterpBy2Blk24En = 1,
						.txInterpBy2Blk22En = 1,
						.txWbLpfBlk22p1En = 0
					},
					.txNbIntTop =
					{
						.txInterpBy2Blk20En = 0,
						.txInterpBy2Blk18En = 0,
						.txInterpBy2Blk16En = 0,
						.txInterpBy2Blk14En = 0,
						.txInterpBy2Blk12En = 0,
						.txInterpBy3Blk10En = 0,
						.txInterpBy2Blk8En = 0,
						.txScicBlk32En = 0,
						.txScicBlk32DivFactor = 1
					},
					.txIntTop =
					{
						.interpBy3Blk44p1En = 1,
						.sinc3Blk44En = 0,
						.sinc2Blk42En = 0,
						.interpBy3Blk40En = 1,
						.interpBy2Blk38En = 0,
						.interpBy2Blk36En = 0
					},
					.txIntTopFreqDevMap =
					{
						.rrc2Frac = 0,
						.mpll = 0,
						.nchLsw = 0,
						.nchMsb = 0,
						.freqDevMapEn = 0,
						.txRoundEn = 1
					},
					.txIqdmDuc =
					{
						.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE0,
						.iqdmDev = 0,
						.iqdmDevOffset = 0,
						.iqdmScalar = 0,
						.iqdmThreshold = 0,
						.iqdmNco =
						{
							.freq = 0,
							.sampleFreq = 61440000,
							.phase = 0,
							.realOut = 0
						}
					}
				},
				.txSsiConfig =
				{
					.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
					.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
					.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
					.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
					.lsbFirst = 0,
					.qFirst = 0,
					.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED,
					.lvdsBitInversion = 0,
					.lvdsUseLsbIn12bitMode = 0,
					.lvdsTxFullRefClkEn = false,
					.lvdsRxClkInversionEn = false,
					.cmosTxDdrNegStrobeEn = false,
					.cmosDdrPosClkEn = false,
					.cmosDdrClkInversionEn = false,
					.cmosDdrEn = false,
					.rxMaskStrobeEn = false
				}
			},
			{
				.primarySigBandwidth_Hz = 1008000,
				.txInputRate_Hz = 1920000,
				.txInterfaceSampleRate_Hz = 1920000,
				.txOffsetLo_kHz = 0,
				.validDataDelay = 0,
				.txBbf3dBCorner_kHz = 50000,
				.outputSignaling = ADI_ADRV9001_TX_IQ,
				.txPdBiasCurrent = 1,
				.txPdGainEnable = 0,
				.txPrePdRealPole_kHz = 1000000,
				.txPostPdRealPole_kHz = 530000,
				.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH,
				.txExtLoopBackType = 0,
				.txExtLoopBackForInitCal = 0,
				.txPeakLoopBackPower = -180,
				.frequencyDeviation_Hz = 0,
				.txDpProfile =
				{
					.txPreProc =
					{
						.txPreProcSymbol0 = 0,
						.txPreProcSymbol1 = 0,
						.txPreProcSymbol2 = 0,
						.txPreProcSymbol3 = 0,
						.txPreProcSymMapDivFactor = 1,
						.txPreProcMode = 1,
						.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_C,
						.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_D
					},
					.txWbIntTop =
					{
						.txInterpBy2Blk30En = 1,
						.txInterpBy2Blk28En = 1,
						.txInterpBy2Blk26En = 1,
						.txInterpBy2Blk24En = 1,
						.txInterpBy2Blk22En = 1,
						.txWbLpfBlk22p1En = 0
					},
					.txNbIntTop =
					{
						.txInterpBy2Blk20En = 0,
						.txInterpBy2Blk18En = 0,
						.txInterpBy2Blk16En = 0,
						.txInterpBy2Blk14En = 0,
						.txInterpBy2Blk12En = 0,
						.txInterpBy3Blk10En = 0,
						.txInterpBy2Blk8En = 0,
						.txScicBlk32En = 0,
						.txScicBlk32DivFactor = 1
					},
					.txIntTop =
					{
						.interpBy3Blk44p1En = 1,
						.sinc3Blk44En = 0,
						.sinc2Blk42En = 0,
						.interpBy3Blk40En = 1,
						.interpBy2Blk38En = 0,
						.interpBy2Blk36En = 0
					},
					.txIntTopFreqDevMap =
					{
						.rrc2Frac = 0,
						.mpll = 0,
						.nchLsw = 0,
						.nchMsb = 0,
						.freqDevMapEn = 0,
						.txRoundEn = 1
					},
					.txIqdmDuc =
					{
						.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE0,
						.iqdmDev = 0,
						.iqdmDevOffset = 0,
						.iqdmScalar = 0,
						.iqdmThreshold = 0,
						.iqdmNco =
						{
							.freq = 0,
							.sampleFreq = 61440000,
							.phase = 0,
							.realOut = 0
						}
					}
				},
				.txSsiConfig =
				{
					.ssiType = ADI_ADRV9001_SSI_TYPE_CMOS,
					.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA,
					.numLaneSel = ADI_ADRV9001_SSI_1_LANE,
					.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE,
					.lsbFirst = 0,
					.qFirst = 0,
					.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED,
					.lvdsBitInversion = 0,
					.lvdsUseLsbIn12bitMode = 0,
					.lvdsTxFullRefClkEn = false,
					.lvdsRxClkInversionEn = false,
					.cmosTxDdrNegStrobeEn = false,
					.cmosDdrPosClkEn = false,
					.cmosDdrClkInversionEn = false,
					.cmosDdrEn = false,
					.rxMaskStrobeEn = false
				}
			}
		}
	},
	.sysConfig =  {
		.duplexMode = ADI_ADRV9001_FDD_MODE,
		.fhModeOn = 0,
		.numDynamicProfile = 1,
		.mcsMode = ADI_ADRV9001_MCSMODE_DISABLED,
		.adcTypeMonitor = ADI_ADRV9001_ADC_HP,
		.pllLockTime_us = 750,
		.pllModulus =
		{
			.modulus = { 8388593, 8388593, 8388593, 8388593, 8388593 },
			.dmModulus = { 8388593, 8388593 }
		}
	},
	.pfirBuffer =  {
		.pfirRxWbNbChFilterCoeff_A = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -392, -228, 1164, -426, -2275, 3118, 1517, -7492, 4673, 8616,-16569, 2081, 24816, -27732, -11318, 53353, -35059, -44826, 94798, -27039, -110096, 145573, 15827, -224287, 197850, 132551, -432387, 241249, 465740, -1004873, 265939, 4480229,4480229, 265939, -1004873, 465740, 241249, -432387, 132551, 197850, -224287, 15827, 145573, -110096, -27039, 94798, -44826, -35059, 53353, -11318, -27732, 24816, 2081, -16569,8616, 4673, -7492, 1517, 3118, -2275, -426, 1164, -228, -392, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxWbNbChFilterCoeff_B = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxWbNbChFilterCoeff_C = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -392, -228, 1164, -426, -2275, 3118, 1517, -7492, 4673, 8616,-16569, 2081, 24816, -27732, -11318, 53353, -35059, -44826, 94798, -27039, -110096, 145573, 15827, -224287, 197850, 132551, -432387, 241249, 465740, -1004873, 265939, 4480229,4480229, 265939, -1004873, 465740, 241249, -432387, 132551, 197850, -224287, 15827, 145573, -110096, -27039, 94798, -44826, -35059, 53353, -11318, -27732, 24816, 2081, -16569,8616, 4673, -7492, 1517, 3118, -2275, -426, 1164, -228, -392, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxWbNbChFilterCoeff_D = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_A = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_B = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_C = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirTxWbNbPulShpCoeff_D = {
			.numCoeff = 128,
			.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
			.tapsSel = ADI_ADRV9001_PFIR_128_TAPS,
			.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
			.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
		},
		.pfirRxNbPulShp = {
			{
				.numCoeff = 128,
				.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
				.taps = 128,
				.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 128,
				.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC,
				.taps = 128,
				.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		},
		.pfirRxMagLowTiaLowSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4351, 5890, -7135, 7006, 26378, 7006, -7135, 5890, -4351, 2883, -1686, 853, -355, 108, -20 }
			},
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4351, 5890, -7135, 7006, 26378, 7006, -7135, 5890, -4351, 2883, -1686, 853, -355, 108, -20 }
			}
		},
		.pfirRxMagLowTiaHighSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 461, -755, 469, 1314, -5308, 11130, -16594, 12348, 26906, 12348, -16594, 11130, -5308, 1314, 469, -755, 461, -166, 30 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 461, -755, 469, 1314, -5308, 11130, -16594, 12348, 26906, 12348, -16594, 11130, -5308, 1314, 469, -755, 461, -166, 30 }
			}
		},
		.pfirRxMagHighTiaHighSRHp = {
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 470, -899, 1050, -225, -2199, 6134, -10228, 10467, 23888, 10467, -10228, 6134, -2199, -225, 1050, -899, 470, -158, 28 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 470, -899, 1050, -225, -2199, 6134, -10228, 10467, 23888, 10467, -10228, 6134, -2199, -225, 1050, -899, 470, -158, 28 }
			}
		},
		.pfirRxMagLowTiaLowSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4350, 5890, -7135, 7010, 26372, 7010, -7135, 5890, -4350, 2883, -1686, 853, -355, 108, -20 }
			},
			{
				.numCoeff = 21,
				.coefficients = { -20, 108, -355, 853, -1686, 2883, -4350, 5890, -7135, 7010, 26372, 7010, -7135, 5890, -4350, 2883, -1686, 853, -355, 108, -20 }
			}
		},
		.pfirRxMagLowTiaHighSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 462, -758, 479, 1293, -5268, 11073, -16531, 12321, 26900, 12321, -16531, 11073, -5268, 1293, 479, -758, 462, -166, 30 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 30, -166, 462, -758, 479, 1293, -5268, 11073, -16531, 12321, 26900, 12321, -16531, 11073, -5268, 1293, 479, -758, 462, -166, 30 }
			}
		},
		.pfirRxMagHighTiaHighSRLp = {
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 471, -900, 1054, -236, -2177, 6099, -10187, 10451, 23878, 10451, -10187, 6099, -2177, -236, 1054, -900, 471, -158, 28 }
			},
			{
				.numCoeff = 21,
				.coefficients = { 28, -158, 471, -900, 1054, -236, -2177, 6099, -10187, 10451, 23878, 10451, -10187, 6099, -2177, -236, 1054, -900, 471, -158, 28 }
			}
		},
		.pfirTxMagComp1 = {
			.numCoeff = 21,
			.coefficients = { 8, -20, -56, 533, -1987, 5017, -9518, 13961, -13376, 6782, 30083, 6782, -13376, 13961, -9518, 5017, -1987, 533, -56, -20, 8 }
		},
		.pfirTxMagComp2 = {
			.numCoeff = 21,
			.coefficients = { 8, -20, -56, 533, -1987, 5017, -9518, 13961, -13376, 6782, 30083, 6782, -13376, 13961, -9518, 5017, -1987, 533, -56, -20, 8 }
		},
		.pfirTxMagCompNb = {
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		},
		.pfirRxMagCompNb = {
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			},
			{
				.numCoeff = 13,
				.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
			}
		}
	}
};

static struct adi_adrv9001_GainControlCfg agc_defaults = {
	.peakWaitTime = 4,
	.maxGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
	.minGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MIN,
	.gainUpdateCounter = 11520,
	.attackDelay_us = 10,
	.lowThreshPreventGainInc = false,
	.slowLoopSettlingDelay = 16,
	.changeGainIfThreshHigh = 3,
	.agcMode = 1,
	.resetOnRxon = false,
	.resetOnRxonGainIndex = ADI_ADRV9001_RX_GAIN_INDEX_MAX,
	.enableSyncPulseForGainCounter = false,
	.enableFastRecoveryLoop = false,
	.power = {
		.powerEnableMeasurement = true,
		.underRangeHighPowerThresh = 10,
		.underRangeLowPowerThresh = 4,
		.underRangeHighPowerGainStepRecovery = 2,
		.underRangeLowPowerGainStepRecovery = 4,
		.powerMeasurementDuration = 10,
		.powerMeasurementDelay = 2,
		.rxTddPowerMeasDuration = 0,
		.rxTddPowerMeasDelay = 0,
		.overRangeHighPowerThresh = 0,
		.overRangeLowPowerThresh = 7,
		.overRangeHighPowerGainStepAttack = 4,
		.overRangeLowPowerGainStepAttack = 4,
		.feedback_inner_high_inner_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_apd_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
	},
	.peak = {
		.agcUnderRangeLowInterval = 50,
		.agcUnderRangeMidInterval = 2,
		.agcUnderRangeHighInterval = 4,
		.apdHighThresh = 21,
		.apdLowThresh = 12,
		.apdUpperThreshPeakExceededCount = 6,
		.apdLowerThreshPeakExceededCount = 3,
		.apdGainStepAttack = 2,
		.apdGainStepRecovery = 0,
		.enableHbOverload = true,
		.hbOverloadDurationCount = 1,
		.hbOverloadThreshCount = 1,
		.hbHighThresh = 13044,
		.hbUnderRangeLowThresh = 5826,
		.hbUnderRangeMidThresh = 8230,
		.hbUnderRangeHighThresh = 7335,
		.hbUpperThreshPeakExceededCount = 6,
		.hbUnderRangeHighThreshExceededCount = 3,
		.hbGainStepHighRecovery = 2,
		.hbGainStepLowRecovery = 6,
		.hbGainStepMidRecovery = 4,
		.hbGainStepAttack = 2,
		.hbOverloadPowerMode = 0,
		.hbUnderRangeMidThreshExceededCount = 3,
		.hbUnderRangeLowThreshExceededCount = 3,
		.feedback_apd_low_hb_low = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
		.feedback_apd_high_hb_high = ADI_ADRV9001_GPIO_PIN_CRUMB_UNASSIGNED,
	},
};

struct adi_adrv9001_SpiSettings *adrv9002_spi_settings_get(void)
{
	return &spiSettings;
}

struct adi_adrv9001_Init *adrv9002_init_get(void)
{
	uint32_t *axi_config = (uint32_t *)(RX1_ADC_BASEADDR + 0xC);
	if (*axi_config & (1<<7))
		return &adrv9002_init_cmos;
	else
		return &adrv9002_init_lvds;
}

struct adi_adrv9001_GainControlCfg *adrv9002_agc_settings_get(void)
{
	return &agc_defaults;
}
