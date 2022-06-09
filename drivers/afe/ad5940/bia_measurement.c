/***************************************************************************//**
 *   @file   bia_measurement.c
 *   @brief  Source file of Bio Impedance Measurement utility.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
 *   @author Darius Berghe (darius.berghe@analog.com)
********************************************************************************
 * Copyright 2022(c) Analog Devices, Inc.
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
#include <errno.h>
#include "bia_measurement.h"

/* Initial AD5940 settings */
AppBiaCfg_Type AppBiaCfg = {
	.SeqStartAddr = 0,
	.MaxSeqLen = 0,

	.SeqStartAddrCal = 0,
	.MaxSeqLenCal = 0,
	.bParamsChanged = false,
	.bImpedanceReadMode = false,
	.ReDoRtiaCal = false,
	.SysClkFreq = 16000000.0,
	.WuptClkFreq = 32000.0,
	.AdcClkFreq = 16000000.0,
	.BiaODR = 20.0, /* 20.0 Hz*/
	.NumOfData = -1,
	.RcalVal = 10000.0, /* 10kOhm */

	.PwrMod = AFEPWR_LP,
	.HstiaRtiaSel = HSTIARTIA_10K,
	.CtiaSel = 16,
	.ExcitBufGain = EXCITBUFGAIN_2,
	.HsDacGain = HSDACGAIN_1,
	.HsDacUpdateRate = 7,
	.DacVoltPP = 800.0, //800.0

	.SinFreq = 10000.0, /* 10000Hz */

	.ADCPgaGain = ADCPGA_1,
	.ADCSinc3Osr = ADCSINC3OSR_2,
	.ADCSinc2Osr = ADCSINC2OSR_22,

	.DftNum = DFTNUM_8192,
	.DftSrc = DFTSRC_SINC3,
	.HanWinEn = true,

	.SweepCfg.SweepEn = false, //true false
	.SweepCfg.SweepStart = 10000,
	.SweepCfg.SweepStop = 80000.0,
	.SweepCfg.SweepPoints = 20,
	.SweepCfg.SweepLog = true,
	.SweepCfg.SweepIndex = 0,

	.FifoThresh = 2,
	.BiaInited = false,
	.StopRequired = false,
};

/**
   This function is provided for upper controllers that want to change
   application parameters specially for user defined parameters.
 */
int AppBiaGetCfg(void *pCfg)
{
	if (pCfg) {
		*(AppBiaCfg_Type **)pCfg = &AppBiaCfg;
		return 0;
	}
	return -EINVAL;
}

int AppBiaCtrl(struct ad5940_dev *dev, int32_t BcmCtrl, void *pPara)
{
	int ret = 0;
	switch (BcmCtrl) {
	case BIACTRL_START: {
		WUPTCfg_Type wupt_cfg = {0};
		ret = ad5940_WakeUp(dev, 10);
		if (ret < 0)
			return ret;
		if (ret > 10) /* Wakup AFE by read register, read 10 times at most */
			return -EIO;  /* Wakeup Failed */
		if (AppBiaCfg.BiaInited == false)
			return -EAGAIN;
		/* Start it */
		wupt_cfg.WuptEn = true;
		wupt_cfg.WuptEndSeq = WUPTENDSEQ_A;
		wupt_cfg.WuptOrder[0] = SEQID_0;
		wupt_cfg.SeqxSleepTime[SEQID_0] = (uint32_t)(AppBiaCfg.WuptClkFreq /
						  AppBiaCfg.BiaODR) - 2 - 1;
		wupt_cfg.SeqxWakeupTime[SEQID_0] =
			1; /* The minimum value is 1. Do not set it to zero. Set it to 1 will spend 2 32kHz clock. */
		ret = ad5940_WUPTCfg(dev, &wupt_cfg);
		if (ret < 0)
			return ret;

		AppBiaCfg.FifoDataCount = 0; /* restart */
		//printf("BIA Start...\n\r");
		break;
	}
	case BIACTRL_STOPNOW: {
		ret = ad5940_WakeUp(dev, 10);
		if (ret < 0)
			return ret;
		if (ret > 10) /* Wakup AFE by read register, read 10 times at most */
			return -EIO;  /* Wakeup Failed */
		/* Start Wupt right now */
		ad5940_WUPTCtrl(dev, false);
		ret = ad5940_WUPTCtrl(dev, false); /* @todo is it sure this will stop Wupt? */
		if (ret < 0)
			return ret;
		//printf("BIA Stop Now...\n\r");
		break;
	}
	case BIACTRL_STOPSYNC: {
		//printf("BIA Stop SYNC...\n\r");
		AppBiaCfg.StopRequired = true;
		break;
	}
	case BIACTRL_GETFREQ:
		if (pPara) {
			if (AppBiaCfg.SweepCfg.SweepEn == true)
				*(float *)pPara = AppBiaCfg.FreqofData;
			else
				*(float *)pPara = AppBiaCfg.SinFreq;
		}
		break;
	case BIACTRL_SHUTDOWN: {
		ret = AppBiaCtrl(dev, BIACTRL_STOPNOW,
				 0); /* Stop the measurment if it's running. */
		if (ret < 0)
			return ret;
		/* Turn off LPloop related blocks which are not controlled automatically by sleep operation */
		AFERefCfg_Type aferef_cfg;
		LPLoopCfg_Type lp_loop;
		memset(&aferef_cfg, 0, sizeof(aferef_cfg));
		ret = ad5940_REFCfgS(dev, &aferef_cfg);
		if (ret < 0)
			return ret;
		memset(&lp_loop, 0, sizeof(lp_loop));
		ret = ad5940_LPLoopCfgS(dev, &lp_loop);
		if (ret < 0)
			return ret;
		ret = ad5940_EnterSleepS(dev); /* Enter Hibernate */
		if (ret < 0)
			return ret;
		printf("BIA Shut down...\n\r");
	}
	break;
	default:
		break;
	}
	return ret;
}

/* Generate init sequence */
static int AppBiaSeqCfgGen(struct ad5940_dev *dev)
{
	int ret = 0;
	uint32_t const *pSeqCmd;
	uint32_t SeqLen;

	AFERefCfg_Type aferef_cfg;
	HSLoopCfg_Type hs_loop;
	LPLoopCfg_Type lp_loop;
	DSPCfg_Type dsp_cfg;
	float sin_freq;

	/* Start sequence generator here */
	ret = ad5940_SEQGenCtrl(dev, true);
	if (ret < 0)
		return ret;

	//AD5940_AFECtrlS(AFECTRL_ALL, false);  /* Init all to disable state */

	aferef_cfg.HpBandgapEn = true;
	aferef_cfg.Hp1V1BuffEn = true;
	aferef_cfg.Hp1V8BuffEn = true;
	aferef_cfg.Disc1V1Cap = false;
	aferef_cfg.Disc1V8Cap = false;
	aferef_cfg.Hp1V8ThemBuff = false;
	aferef_cfg.Hp1V8Ilimit = false;
	aferef_cfg.Lp1V1BuffEn = false;
	aferef_cfg.Lp1V8BuffEn = false;
	/* LP reference control - turn off them to save powr*/
	aferef_cfg.LpBandgapEn = true;
	aferef_cfg.LpRefBufEn = true;
	aferef_cfg.LpRefBoostEn = false;
	ret = ad5940_REFCfgS(dev, &aferef_cfg);
	if (ret < 0)
		return ret;

	hs_loop.HsDacCfg.ExcitBufGain = AppBiaCfg.ExcitBufGain;
	hs_loop.HsDacCfg.HsDacGain = AppBiaCfg.HsDacGain;
	hs_loop.HsDacCfg.HsDacUpdateRate = AppBiaCfg.HsDacUpdateRate;

	hs_loop.HsTiaCfg.DiodeClose = false;
	hs_loop.HsTiaCfg.HstiaBias = HSTIABIAS_1P1;
	hs_loop.HsTiaCfg.HstiaCtia = AppBiaCfg.CtiaSel; /* 31pF + 2pF */
	hs_loop.HsTiaCfg.HstiaDeRload = HSTIADERLOAD_OPEN;
	hs_loop.HsTiaCfg.HstiaDeRtia = HSTIADERTIA_TODE;
	hs_loop.HsTiaCfg.HstiaRtiaSel = AppBiaCfg.HstiaRtiaSel;

	hs_loop.SWMatCfg.Dswitch = SWD_OPEN;
	hs_loop.SWMatCfg.Pswitch = SWP_PL | SWP_PL2;
	hs_loop.SWMatCfg.Nswitch = SWN_NL | SWN_NL2;
	hs_loop.SWMatCfg.Tswitch = SWT_TRTIA;

	hs_loop.WgCfg.WgType = WGTYPE_SIN;
	hs_loop.WgCfg.GainCalEn = false;
	hs_loop.WgCfg.OffsetCalEn = false;
	if (AppBiaCfg.SweepCfg.SweepEn == true) {
		AppBiaCfg.FreqofData = AppBiaCfg.SweepCfg.SweepStart;
		AppBiaCfg.SweepCurrFreq = AppBiaCfg.SweepCfg.SweepStart;
		ad5940_SweepNext(dev, &AppBiaCfg.SweepCfg, &AppBiaCfg.SweepNextFreq);
		sin_freq = AppBiaCfg.SweepCurrFreq;
	} else {
		sin_freq = AppBiaCfg.SinFreq;
		AppBiaCfg.FreqofData = sin_freq;
	}
	hs_loop.WgCfg.SinCfg.SinFreqWord = ad5940_WGFreqWordCal(sin_freq,
					   AppBiaCfg.SysClkFreq);
	hs_loop.WgCfg.SinCfg.SinAmplitudeWord = (uint32_t)(AppBiaCfg.DacVoltPP / 800.0f
						* 2047 + 0.5f);
	hs_loop.WgCfg.SinCfg.SinOffsetWord = 0;
	hs_loop.WgCfg.SinCfg.SinPhaseWord = 0;
	ret = ad5940_HSLoopCfgS(dev, &hs_loop);
	if (ret < 0)
		return ret;

	lp_loop.LpDacCfg.LpDacSrc = LPDACSRC_MMR;
	lp_loop.LpDacCfg.LpDacSW = LPDACSW_VBIAS2LPPA | LPDACSW_VBIAS2PIN |
				   LPDACSW_VZERO2LPTIA | LPDACSW_VZERO2PIN;
	lp_loop.LpDacCfg.LpDacVzeroMux = LPDACVZERO_6BIT;
	lp_loop.LpDacCfg.LpDacVbiasMux = LPDACVBIAS_12BIT;
	lp_loop.LpDacCfg.LpDacRef = LPDACREF_2P5;
	lp_loop.LpDacCfg.DataRst = false;
	lp_loop.LpDacCfg.PowerEn = true;
	lp_loop.LpDacCfg.DacData12Bit = (uint32_t)((1100 - 200) / 2200.0 * 4095);
	lp_loop.LpDacCfg.DacData6Bit = 31;

	lp_loop.LpAmpCfg.LpAmpPwrMod = LPAMPPWR_NORM;
	lp_loop.LpAmpCfg.LpPaPwrEn = true;
	lp_loop.LpAmpCfg.LpTiaPwrEn = true;
	lp_loop.LpAmpCfg.LpTiaRf = LPTIARF_20K;
	lp_loop.LpAmpCfg.LpTiaRload = LPTIARLOAD_SHORT;
	lp_loop.LpAmpCfg.LpTiaRtia = LPTIARTIA_OPEN;
	lp_loop.LpAmpCfg.LpTiaSW = LPTIASW(5) | LPTIASW(6) | LPTIASW(7) | LPTIASW(
					   8) | LPTIASW(9) | LPTIASW(12) | LPTIASW(
					   13); /* @todo Optimizanation needed for new silicon */
	ret = ad5940_LPLoopCfgS(dev, &lp_loop);
	if (ret < 0)
		return ret;

	dsp_cfg.ADCBaseCfg.ADCMuxN = ADCMUXN_HSTIA_N;
	dsp_cfg.ADCBaseCfg.ADCMuxP = ADCMUXP_HSTIA_P;
	dsp_cfg.ADCBaseCfg.ADCPga = AppBiaCfg.ADCPgaGain;

	memset(&dsp_cfg.ADCDigCompCfg, 0, sizeof(dsp_cfg.ADCDigCompCfg));

	dsp_cfg.ADCFilterCfg.ADCAvgNum =
		ADCAVGNUM_16; /* Don't care becase it's disabled */
	dsp_cfg.ADCFilterCfg.ADCRate =
		ADCRATE_800KHZ; /* @todo Add explanation in UG that SINC3 filter clock is same as ADC, when ADC runs at 32MHz, clear this bit to enable clock divider for SINC3 filter. Make sure SINC3 clock is below 16MHz. */
	dsp_cfg.ADCFilterCfg.ADCSinc2Osr = AppBiaCfg.ADCSinc2Osr;
	dsp_cfg.ADCFilterCfg.ADCSinc3Osr = AppBiaCfg.ADCSinc3Osr;
	dsp_cfg.ADCFilterCfg.BpSinc3 = false;
	dsp_cfg.ADCFilterCfg.BpNotch = true;
	dsp_cfg.ADCFilterCfg.Sinc2NotchEnable = true;
	dsp_cfg.ADCFilterCfg.Sinc2NotchClkEnable = true;
	dsp_cfg.ADCFilterCfg.Sinc3ClkEnable = true;
	dsp_cfg.ADCFilterCfg.WGClkEnable = true;
	dsp_cfg.ADCFilterCfg.DFTClkEnable = true;
	dsp_cfg.DftCfg.DftNum = AppBiaCfg.DftNum;
	dsp_cfg.DftCfg.DftSrc = AppBiaCfg.DftSrc;
	dsp_cfg.DftCfg.HanWinEn = AppBiaCfg.HanWinEn;

	memset(&dsp_cfg.StatCfg, 0,
	       sizeof(dsp_cfg.StatCfg)); /* Don't care about Statistic */
	ret = ad5940_DSPCfgS(dev, &dsp_cfg);
	if (ret < 0)
		return ret;

	/* Enable all of them. They are automatically turned off during hibernate mode to save power */
	ret = ad5940_AFECtrlS(dev,
			      AFECTRL_HPREFPWR | AFECTRL_HSTIAPWR | AFECTRL_INAMPPWR | AFECTRL_EXTBUFPWR |
			      AFECTRL_WG | AFECTRL_DACREFPWR | AFECTRL_HSDACPWR |
			      AFECTRL_SINC2NOTCH,
			      true);
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGpioCtrlS(dev,
				  0 /*AGPIO_Pin6|AGPIO_Pin5|AGPIO_Pin1*/); //GP6->endSeq, GP5 -> AD8233=OFF, GP1->RLD=OFF .
	if (ret < 0)
		return ret;

	/* Sequence end. */
	ret = ad5940_SEQGenInsert(dev,
				  SEQ_STOP()); /* Add one extra command to disable sequencer for initialization sequence because we only want it to run one time. */
	if (ret < 0)
		return ret;

	/* Stop here */
	ret = ad5940_SEQGenFetchSeq(dev, &pSeqCmd, &SeqLen);
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGenCtrl(dev, false); /* Stop sequence generator */
	if (ret < 0)
		return ret;

	AppBiaCfg.InitSeqInfo.SeqId = SEQID_1;
	AppBiaCfg.InitSeqInfo.SeqRamAddr = AppBiaCfg.SeqStartAddr;
	AppBiaCfg.InitSeqInfo.pSeqCmd = pSeqCmd;
	AppBiaCfg.InitSeqInfo.SeqLen = SeqLen;
	/* Write command to SRAM */
	return ad5940_SEQCmdWrite(dev, AppBiaCfg.InitSeqInfo.SeqRamAddr, pSeqCmd,
				  SeqLen);
}

static int AppBiaSeqMeasureGen(struct ad5940_dev *dev, bool bImpedanceMode)
{
	int ret;
	uint32_t const *pSeqCmd;
	uint32_t SeqLen;

	uint32_t WaitClks;
	SWMatrixCfg_Type sw_cfg;
	ClksCalInfo_Type clks_cal;

	clks_cal.DataType = DATATYPE_DFT;
	clks_cal.DftSrc = AppBiaCfg.DftSrc;
	clks_cal.DataCount = 1L << (AppBiaCfg.DftNum + 2); /* 2^(DFTNUMBER+2) */
	clks_cal.ADCSinc2Osr = AppBiaCfg.ADCSinc2Osr;
	clks_cal.ADCSinc3Osr = AppBiaCfg.ADCSinc3Osr;
	clks_cal.ADCAvgNum = 0;
	clks_cal.RatioSys2AdcClk = AppBiaCfg.SysClkFreq / AppBiaCfg.AdcClkFreq;
	ret = ad5940_ClksCalculate(dev, &clks_cal, &WaitClks);
	if (ret < 0)
		return ret;

	/* Start sequence generator here */
	ret = ad5940_SEQGenCtrl(dev, true);
	if (ret < 0)
		return ret;

	/*************************************/
	sw_cfg.Dswitch = SWD_CE0;
	sw_cfg.Pswitch = SWP_CE0;
	sw_cfg.Nswitch = SWN_AIN1;             //AIN0 AIN1
	sw_cfg.Tswitch = SWT_AIN0 | SWT_TRTIA; //AIN0 AIN1
	ret = ad5940_SWMatrixCfgS(dev, &sw_cfg);
	if (ret < 0)
		return ret;

	/*************************************/

	ret = ad5940_SEQGpioCtrlS(dev,
				  AGPIO_Pin6 /*|AGPIO_Pin5|AGPIO_Pin1*/); //GP6->endSeq, GP5 -> AD8233=OFF, GP1->RLD=OFF .
	if (ret < 0)
		return ret;

	ret = ad5940_ADCMuxCfgS(dev, ADCMUXP_AIN2, ADCMUXN_AIN3);
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_WG | AFECTRL_ADCPWR,
			      true);  //Enable Waveform generator, ADC power
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGenInsert(dev,
				  SEQ_WAIT(16 * 50));               //delay for signal settling DFT_WAIT
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV | AFECTRL_DFT,
			      true); // Start ADC convert and DFT
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGenInsert(dev,
				  SEQ_WAIT(WaitClks));              // wait for first data ready
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev,
			      AFECTRL_ADCCNV | AFECTRL_DFT | AFECTRL_WG | AFECTRL_ADCPWR, false);
	if (ret < 0)
		return ret;

	if (bImpedanceMode) {
		/*************************************/
		ret = ad5940_ADCMuxCfgS(dev, ADCMUXP_HSTIA_P, ADCMUXN_HSTIA_N);
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_WG | AFECTRL_ADCPWR, true);
		if (ret < 0)
			return ret;

		ret = ad5940_SEQGenInsert(dev, SEQ_WAIT(16 * 50));
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV | AFECTRL_DFT, true);
		if (ret < 0)
			return ret;

		ret = ad5940_SEQGenInsert(dev, SEQ_WAIT(WaitClks));
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev,
				      AFECTRL_ADCCNV | AFECTRL_DFT | AFECTRL_WG | AFECTRL_ADCPWR, false);
		if (ret < 0)
			return ret;

		/*************************************/
	}
	sw_cfg.Dswitch = SWD_OPEN;
	sw_cfg.Pswitch = SWP_PL | SWP_PL2;
	sw_cfg.Nswitch = SWN_NL | SWN_NL2;
	sw_cfg.Tswitch = SWT_TRTIA;
	ret = ad5940_SWMatrixCfgS(dev, &sw_cfg); /* Float switches */
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGpioCtrlS(dev,
				  0 /*AGPIO_Pin6|AGPIO_Pin5|AGPIO_Pin1*/); //GP6->endSeq, GP5 -> AD8233=OFF, GP1->RLD=OFF .
	if (ret < 0)
		return ret;

	ret = ad5940_EnterSleepS(
		      dev);                                        /* Goto hibernate */
	if (ret < 0)
		return ret;

	/* Sequence end. */
	ret = ad5940_SEQGenFetchSeq(dev, &pSeqCmd, &SeqLen);
	if (ret < 0)
		return ret;

	ret = ad5940_SEQGenCtrl(dev, false); /* Stop sequencer generator */
	if (ret < 0)
		return ret;

	AppBiaCfg.MeasureSeqInfo.SeqId = SEQID_0;
	AppBiaCfg.MeasureSeqInfo.SeqRamAddr = AppBiaCfg.InitSeqInfo.SeqRamAddr +
					      AppBiaCfg.InitSeqInfo.SeqLen;
	AppBiaCfg.MeasureSeqInfo.pSeqCmd = pSeqCmd;
	AppBiaCfg.MeasureSeqInfo.SeqLen = SeqLen;
	/* Write command to SRAM */
	return ad5940_SEQCmdWrite(dev, AppBiaCfg.MeasureSeqInfo.SeqRamAddr, pSeqCmd,
				  SeqLen);
}

static int AppBiaRtiaCal(struct ad5940_dev *dev)
{
	int ret;
	HSRTIACal_Type hsrtia_cal;

	hsrtia_cal.AdcClkFreq = AppBiaCfg.AdcClkFreq;
	hsrtia_cal.ADCSinc2Osr = AppBiaCfg.ADCSinc2Osr;
	hsrtia_cal.ADCSinc3Osr = AppBiaCfg.ADCSinc3Osr;
	hsrtia_cal.bPolarResult = false; /* We need Real and Imag here */
	hsrtia_cal.DftCfg.DftNum = AppBiaCfg.DftNum;
	hsrtia_cal.DftCfg.DftSrc = AppBiaCfg.DftSrc;
	hsrtia_cal.DftCfg.HanWinEn = AppBiaCfg.HanWinEn;
	hsrtia_cal.fRcal = AppBiaCfg.RcalVal;
	hsrtia_cal.HsTiaCfg.DiodeClose = false;
	hsrtia_cal.HsTiaCfg.HstiaBias = HSTIABIAS_1P1;
	hsrtia_cal.HsTiaCfg.HstiaCtia = AppBiaCfg.CtiaSel;
	hsrtia_cal.HsTiaCfg.HstiaDeRload = HSTIADERLOAD_OPEN;
	hsrtia_cal.HsTiaCfg.HstiaDeRtia = HSTIADERTIA_TODE;
	hsrtia_cal.HsTiaCfg.HstiaRtiaSel = HSTIARTIA_10K;
	hsrtia_cal.SysClkFreq = AppBiaCfg.SysClkFreq;

	if (AppBiaCfg.SweepCfg.SweepEn == true) {
		uint32_t i;
		AppBiaCfg.SweepCfg.SweepIndex = 0; /* Reset index */
		for (i = 0; i < AppBiaCfg.SweepCfg.SweepPoints; i++) {
			ad5940_SweepNext(dev, &AppBiaCfg.SweepCfg, &hsrtia_cal.fFreq);
			ret = ad5940_HSRtiaCal(dev, &hsrtia_cal, AppBiaCfg.RtiaCalTable[i]);
			if (ret < 0)
				return ret;
			printf("Freq:%.2f,Mag:%.2f,Phase:%fDegree\n", hsrtia_cal.fFreq,
			       AppBiaCfg.RtiaCalTable[i][0], AppBiaCfg.RtiaCalTable[i][1] * 180 / MATH_PI);
		}
		AppBiaCfg.RtiaCurrValue[AppBiaCfg.SweepCfg.SweepIndex] =
			AppBiaCfg.RtiaCalTable[i][0];
		AppBiaCfg.RtiaCurrValue[AppBiaCfg.SweepCfg.SweepIndex] =
			AppBiaCfg.RtiaCalTable[i][0];
		AppBiaCfg.SweepCfg.SweepIndex = 0; /* Reset index */
	} else {
		hsrtia_cal.fFreq = AppBiaCfg.SinFreq;
		ret = ad5940_HSRtiaCal(dev, &hsrtia_cal, AppBiaCfg.RtiaCurrValue);
		if (ret < 0)
			return ret;
		printf("RtiaReal:%.2f,Imag:%f\n", AppBiaCfg.RtiaCurrValue[0],
		       AppBiaCfg.RtiaCurrValue[1]);
	}
	return 0;
}

/* This function provide application initialize.   */
int AppBiaInit(struct ad5940_dev *dev, uint32_t *pBuffer, uint32_t BufferSize)
{
	int ret;
	SEQCfg_Type seq_cfg;
	FIFOCfg_Type fifo_cfg;

	ret = ad5940_WakeUp(dev, 10);
	if (ret < 0)
		return ret;
	if (ret > 10) /* Wakup AFE by read register, read 10 times at most */
		return -EIO;  /* Wakeup Failed */

	/* Configure sequencer and stop it */
	seq_cfg.SeqMemSize =
		SEQMEMSIZE_2KB; /* 2kB SRAM is used for sequencer, others for data FIFO */
	seq_cfg.SeqBreakEn = false;
	seq_cfg.SeqIgnoreEn = false;
	seq_cfg.SeqCntCRCClr = true;
	seq_cfg.SeqEnable = false;
	seq_cfg.SeqWrTimer = 0;
	ret = ad5940_SEQCfg(dev, &seq_cfg);
	if (ret < 0)
		return ret;

	/* Do RTIA calibration */

	if ((AppBiaCfg.bParamsChanged == true) || (AppBiaCfg.ReDoRtiaCal == true) ||
	    AppBiaCfg.BiaInited == false) { /* Do calibration on the first initializaion */
		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
		if (ret < 0)
			return ret;
		ret = AppBiaRtiaCal(dev);
		if (ret < 0)
			return ret;
		AppBiaCfg.ReDoRtiaCal = false;
	}
	/* Reconfigure FIFO */
	ret = ad5940_FIFOCtrlS(dev, FIFOSRC_DFT, false); /* Disable FIFO firstly */
	if (ret < 0)
		return ret;
	fifo_cfg.FIFOEn = true;
	fifo_cfg.FIFOMode = FIFOMODE_FIFO;
	fifo_cfg.FIFOSize =
		FIFOSIZE_4KB; /* 4kB for FIFO, The reset 2kB for sequencer */
	fifo_cfg.FIFOSrc = FIFOSRC_DFT;
	fifo_cfg.FIFOThresh =
		AppBiaCfg.FifoThresh; //AppBiaCfg.FifoThresh;              /* DFT result. One pair for RCAL, another for Rz. One DFT result have real part and imaginary part */
	ret = ad5940_FIFOCfg(dev, &fifo_cfg);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ALLINT);
	if (ret < 0)
		return ret;

	/* Start sequence generator */
	/* Initialize sequencer generator */
	if ((AppBiaCfg.BiaInited == false) ||
	    (AppBiaCfg.bParamsChanged == true)) {
		if (pBuffer == 0)
			return -EINVAL;
		if (BufferSize == 0)
			return -EINVAL;
		ret = ad5940_SEQGenInit(dev, pBuffer, BufferSize);
		if (ret < 0)
			return ret;

		/* Generate initialize sequence */
		ret = AppBiaSeqCfgGen(
			      dev); /* Application initialization sequence using either MCU or sequencer */
		if (ret < 0)
			return ret;

		/* Generate measurement sequence */
		ret = AppBiaSeqMeasureGen(dev, AppBiaCfg.bImpedanceReadMode);
		if (ret < 0)
			return ret;
	}

	/* Initialization sequencer  */
	AppBiaCfg.InitSeqInfo.WriteSRAM = false;
	ret = ad5940_SEQInfoCfg(dev,&AppBiaCfg.InitSeqInfo);
	if (ret < 0)
		return ret;
	seq_cfg.SeqEnable = true;
	ret = ad5940_SEQCfg(dev, &seq_cfg); /* Enable sequencer */
	if (ret < 0)
		return ret;
	ret = ad5940_SEQMmrTrig(dev, AppBiaCfg.InitSeqInfo.SeqId);
	if (ret < 0)
		return ret;
	while (ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_ENDSEQ) == false)
		;

	/* Measurment sequence  */
	AppBiaCfg.MeasureSeqInfo.WriteSRAM = false;
	ad5940_SEQInfoCfg(dev, &AppBiaCfg.MeasureSeqInfo);

	seq_cfg.SeqEnable = true;
	ret = ad5940_SEQCfg(dev, &seq_cfg); /* Enable sequencer, and wait for trigger */
	if (ret < 0)
		return ret;
	ret = ClrMCUIntFlag();  /* Clear interrupt flag generated before */
	if (ret < 0)
		return ret;

	ret = ad5940_AFEPwrBW(dev, AppBiaCfg.PwrMod, AFEBW_250KHZ);
	if (ret < 0)
		return ret;
	ret = ad5940_WriteReg(dev, REG_AFE_SWMUX, 1 << 3);
	if (ret < 0)
		return ret;
	AppBiaCfg.BiaInited = true;       /* Bia application has been initialized. */
	AppBiaCfg.bParamsChanged =
		false; /* Clear this flag as we already implemented the new configuration */
	return 0;
}

/* Modify registers when AFE wakeup */
static int AppEITRegModify(struct ad5940_dev *dev, int32_t * const pData,
			   uint32_t *pDataCount)
{
	if(AppBiaCfg.NumOfData > 0) {
		AppBiaCfg.FifoDataCount += *pDataCount/4;
		if((int32_t)AppBiaCfg.FifoDataCount >= AppBiaCfg.NumOfData) {
			return ad5940_WUPTCtrl(dev, false);
		}
	}
	if(AppBiaCfg.StopRequired == true) {
		return ad5940_WUPTCtrl(dev, false);
	}
	if(AppBiaCfg.SweepCfg.SweepEn) { /* Need to set new frequency and set power mode */
		return ad5940_WGFreqCtrlS(dev, AppBiaCfg.SweepNextFreq, AppBiaCfg.SysClkFreq);
	}
	return 0;
}

void signExtend18To32(uint32_t *const pData, uint16_t nLen)
{
	/* Convert Voltage values to int32_t type */
	for (uint32_t i = 0; i < nLen; i++) {
		pData[i] &= 0x3ffff;      /* @todo option to check ECC */
		if (pData[i] & (1 << 17)) { /* Bit17 is sign bit */
			pData[i] |=
				0xfffc0000; /* Data is 18bit in two's complement, bit17 is the sign bit */
		}
	}
}

fImpCar_Type computeImpedance(uint32_t *const pData)
{
	// 4 data. VRe, VIm, IRe, IIm
	iImpCar_Type *pSrcData = (iImpCar_Type *)pData;
	iImpCar_Type *pDftVolt, *pDftCurr;
	fImpCar_Type fCarZval;
	float a,b;
	float c,d;
	float denum=1;

	pDftVolt = pSrcData++;
	pDftCurr = pSrcData;

	// Compute Impedance from Voltage and Current
	//fCarZval = AD5940_ComplexDivInt(pDftVolt, pDftCurr);
	//printf("V:%d,%d,I:%d,%d ",pDftVolt->Real,pDftVolt->Image, pDftCurr->Real, pDftCurr->Image);
	//(a+bi)/(c+di) = (a+bi)(c-di)/((c+di)(c-di)) = (a*c+b*d+(-a*d+b*c)i)/(c^2+d^2)
	a = (float)pDftVolt->Real;
	b = -(float)pDftVolt->Image;
	c = -(float)pDftCurr->Real;
	d = (float)pDftCurr->Image;

	//(c^2+d^2)
	denum = (c*c+d*d);
	//RE((a+bi)(c-di)) = RE(a*c-b*d(-1)+(-a*d+b*c)i) = a*c+b*d
	fCarZval.Real = (a*c + b*d)/denum;
	////IM((a+bi)(c-di)) = IM(a*c-b*d(-1)+(-a*d+b*c)i) = (-a*d+b*c)
	fCarZval.Image = (-a*d + b*c)/denum;

	//Apply Rtia cal values;
	a = fCarZval.Real * AppBiaCfg.RtiaCurrValue[0] - fCarZval.Image *
	    AppBiaCfg.RtiaCurrValue[1];
	b = fCarZval.Image * AppBiaCfg.RtiaCurrValue[0] + fCarZval.Real *
	    AppBiaCfg.RtiaCurrValue[1];
	fCarZval.Real = a;
	fCarZval.Image = b;
	return fCarZval;
}

/**

 */
int AppBiaISR(struct ad5940_dev *dev, void *pBuff, uint32_t *pCount)
{
	int ret;
	uint32_t BuffCount;
	uint32_t FifoCnt;
	if (!pBuff || !pCount)
		return -EINVAL;
	BuffCount = *pCount;
	if (AppBiaCfg.BiaInited == false)
		return -EAGAIN;
	ret = ad5940_WakeUp(dev, 10);
	if (ret < 0)
		return ret;
	if (ret > 10) /* Wakup AFE by read register, read 10 times at most */
		return -EIO;  /* Wakeup Failed */
	ret = ad5940_SleepKeyCtrlS(dev, SLPKEY_LOCK); /* Don't enter hibernate */
	if (ret < 0)
		return ret;
	*pCount = 0;

	if (ad5940_INTCTestFlag(dev, AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH) == true) {
		/* Now there should be 2 or 4 data in FIFO */
		ret = ad5940_FIFOGetCnt(dev, &FifoCnt); //(AD5940_FIFOGetCnt()/4)*4;
		if (ret < 0)
			return ret;
		if (FifoCnt > BuffCount) {
			///@todo buffer is limited.
			FifoCnt = AppBiaCfg.FifoThresh;
		}
		// Read FifoCnt of fifo contents and store to pBuff
		ret = ad5940_FIFORd(dev, (uint32_t *)pBuff, FifoCnt);
		if (ret < 0)
			return ret;
		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DATAFIFOTHRESH);
		if (ret < 0)
			return ret;
		ret = AppEITRegModify(dev, pBuff,
				      &FifoCnt);   /* If there is need to do AFE re-configure, do it here when AFE is in active state */
		if (ret < 0)
			return ret;
		ret = ad5940_SleepKeyCtrlS(dev,
					   SLPKEY_UNLOCK); /* Allow AFE to enter hibernate mode */
		if (ret < 0)
			return ret;
		*pCount = FifoCnt;
	}

	return 0;
}

/**
 * @}
 */
