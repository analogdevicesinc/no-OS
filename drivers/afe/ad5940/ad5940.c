/***************************************************************************//**
 *   @file   ad5940.c
 *   @brief  Source file of ad5940 driver.
 *   @author Kister Jimenez (kister.jimenez@analog.com)
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
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include "no_os_delay.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_alloc.h"
#include "ad5940.h"

static int AD5940_Initialize(struct ad5940_dev *dev);
static void AD5940_StructInit(void *pStruct, uint32_t StructSize);

/*! \mainpage AD5940 Library Introduction
 *
 * ![AD5940 Eval Board](http://www.analog.com/-/media/analog/en/evaluation-board-images/images/eval-adicup3029-top.jpg?h=270&hash=6C371E73D52EF96EDE04EEF9AC3B1984BAF045E7 "ADI logo")
 *
 * # Introduction
 *
 * The documentation is for AD594x library and examples.
 *
 * # Manual Structure
 *
 * @ref AD5940_Library
 *  - @ref AD5940_Functions
 *  - @ref TypeDefinitions
 *  - @ref AD5940RegistersBitfields
 * @ref AD5940_Standard_Examples
 * @ref AD5940_System_Examples
 *
 */

/* Declare of SPI functions used to read/write registers */
static int AD5940_SPIReadReg(struct no_os_spi_desc *spi, uint16_t RegAddr,
			     uint32_t *RegData);
static int AD5940_SPIWriteReg(struct no_os_spi_desc *spi, uint16_t RegAddr,
			      uint32_t RegData);

/* Initialize AD5940 basic blocks like clock */
int ad5940_init(struct ad5940_dev **device,
		struct ad5940_init_param *init_param)
{
	int ret;
	CLKCfg_Type clk_cfg;
	FIFOCfg_Type fifo_cfg;
	AGPIOCfg_Type gpio_cfg;
	struct ad5940_dev *dev;

	if (!device || !init_param)
		return -EINVAL;

	dev = (struct ad5940_dev *) no_os_calloc(1, sizeof(struct ad5940_dev));
	if (!dev)
		return -ENOMEM;

	ret = no_os_spi_init(&dev->spi, &init_param->spi_init);
	if (ret < 0)
		goto error;

	ret = no_os_gpio_get(&dev->reset_gpio, &init_param->reset_gpio_init);
	if (ret < 0)
		goto error_1;

	ret = no_os_gpio_direction_output(dev->reset_gpio, NO_OS_GPIO_HIGH);
	if (ret < 0)
		goto error_2;

	ret = no_os_gpio_get(&dev->gp0_gpio, &init_param->gp0_gpio_init);
	if (ret < 0)
		goto error_2;

	ret = no_os_gpio_direction_input(dev->gp0_gpio);
	if (ret < 0)
		goto error_3;

	ret = ad5940_HWReset(dev);
	if (ret < 0)
		goto error_3;

	/* Platform configuration */
	ret = AD5940_Initialize(dev);
	if (ret < 0)
		goto error_3;

	/* Step1. Configure clock */
	clk_cfg.ADCClkDiv = ADCCLKDIV_1;
	clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
	clk_cfg.SysClkDiv = SYSCLKDIV_1;
	clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
	clk_cfg.HfOSC32MHzMode = false;
	clk_cfg.HFOSCEn = true;
	clk_cfg.HFXTALEn = false;
	clk_cfg.LFOSCEn = true;
	ret = ad5940_CLKCfg(dev, &clk_cfg);
	if (ret < 0)
		goto error_3;

	/* Step2. Configure FIFO and Sequencer*/
	fifo_cfg.FIFOEn = false;
	fifo_cfg.FIFOMode = FIFOMODE_FIFO;
	fifo_cfg.FIFOSize = FIFOSIZE_4KB; /* 4kB for FIFO, The reset 2kB
	 * for sequencer */
	fifo_cfg.FIFOSrc = FIFOSRC_DFT;
	fifo_cfg.FIFOThresh = 2;   /* DFT result. One pair for RCAL, another for
	 * Rz. One DFT result have real part and
	 * imaginary part */
	ret = ad5940_FIFOCfg(dev, &fifo_cfg); /* Disable to reset FIFO. */
	if (ret < 0)
		goto error_3;
	fifo_cfg.FIFOEn = true;
	ret = ad5940_FIFOCfg(dev, &fifo_cfg); /* Enable FIFO here */
	if (ret < 0)
		goto error_3;

	/* Step3. Interrupt controller */
	/* Enable all interrupt in Interrupt Controller 1, so we can check INTC flags */
	ret = ad5940_INTCCfg(dev, AFEINTC_1, AFEINTSRC_ALLINT, true);
	if (ret < 0)
		goto error_3;
	/* Interrupt Controller 0 will control GP0 to generate interrupt to MCU */
	ret = ad5940_INTCCfg(dev, AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, true);
	if (ret < 0)
		goto error_3;
	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ALLINT);
	if (ret < 0)
		goto error_3;

	/* Step4: Reconfigure GPIO */
	gpio_cfg.FuncSet = GP0_INT | GP1_GPIO;
	gpio_cfg.InputEnSet = 0;
	gpio_cfg.OutputEnSet = AGPIO_Pin0 | AGPIO_Pin1;
	gpio_cfg.OutVal = AGPIO_Pin0 | AGPIO_Pin1;
	gpio_cfg.PullEnSet = AGPIO_Pin0 | AGPIO_Pin1;

	ret = ad5940_AGPIOCfg(dev, &gpio_cfg);
	if (ret)
		goto error_3;
	ret = ad5940_SleepKeyCtrlS(dev,
				   SLPKEY_UNLOCK); /* Allow AFE to enter sleep mode. */
	if (ret < 0)
		goto error_3;

	*device = dev;
	return 0;
error_3:
	no_os_gpio_remove(dev->gp0_gpio);
error_2:
	no_os_gpio_remove(dev->reset_gpio);
error_1:
	no_os_spi_remove(dev->spi);
error:
	no_os_free(dev);
	return ret;
}

int ad5940_remove(struct ad5940_dev *dev)
{
	if (!dev)
		return 0;

	no_os_gpio_remove(dev->reset_gpio);
	no_os_spi_remove(dev->spi);
	dev->spi = NULL;
	no_os_free(dev);

	return 0;
}

/**
 * @addtogroup AD5940_Library
 *  The library functions, structures and constants.
 * @{
 *    @defgroup AD5940_Functions
 *    @{
 *        @defgroup Function_Helpers
 *        @brief The functions with no harware acccess. They are helpers.
 *        @{
 *            @defgroup Sequencer_Generator_Functions
 *            @brief The set of function used to track all register read and write once it's enalbed. It can translate register write operation to sequencer commands.
 *            @{
 */
/* Manually put a command to sequence */
int ad5940_SEQGenInsert(struct ad5940_dev *dev, uint32_t CmdWord)
{
	uint32_t temp;
	temp  = dev->SeqGenDB.RegCount + dev->SeqGenDB.SeqLen;
	/* Generate Sequence command */
	if(temp < dev->SeqGenDB.BufferSize) {
		dev->SeqGenDB.pSeqBuff[dev->SeqGenDB.SeqLen] = CmdWord;
		dev->SeqGenDB.SeqLen ++;
	} else /* There is no buffer */
		dev->SeqGenDB.LastError = -ENOMEM;

	return 0;
}

/* Get the index in reg info */
static int AD5940_SEQGenSearchReg(struct ad5940_dev *dev, uint32_t RegAddr,
				  uint32_t *pIndex)
{
	uint32_t i;

	RegAddr = (RegAddr>>2)&0xff;
	for(i=0; i<dev->SeqGenDB.RegCount; i++) {
		if(RegAddr == dev->SeqGenDB.pRegInfo[i].RegAddr) {
			*pIndex = i;
			return 0;
		}
	}
	return -EINVAL;
}

static int AD5940_SEQGenGetRegDefault(struct ad5940_dev *dev, uint32_t RegAddr,
				      uint32_t *pRegData)
{
	/* @todo If this peace of code runs at PC, then use different method to get register default value */

	return AD5940_SPIReadReg(dev->spi, RegAddr, pRegData);
}

static int AD5940_SEQRegInfoInsert(struct ad5940_dev *dev, uint16_t RegAddr,
				   uint32_t RegData)
{
	uint32_t temp;
	temp = dev->SeqGenDB.RegCount + dev->SeqGenDB.SeqLen;

	if(temp < dev->SeqGenDB.BufferSize) {
		dev->SeqGenDB.pRegInfo --; /* Move back */
		dev->SeqGenDB.pRegInfo[0].RegAddr = (RegAddr>>2)&0xff;
		dev->SeqGenDB.pRegInfo[0].RegValue = RegData&0x00fffff;
		dev->SeqGenDB.RegCount ++;
	} else { /* There is no more buffer  */
		dev->SeqGenDB.LastError = -ENOMEM;
	}

	return 0;
}

static int AD5940_SEQReadReg(struct ad5940_dev *dev, uint16_t RegAddr,
			     uint32_t *RegData)
{
	int ret;
	uint32_t RegIndex;

	if(AD5940_SEQGenSearchReg(dev, RegAddr, &RegIndex) != 0) {
		ret = AD5940_SEQGenGetRegDefault(dev, RegAddr, RegData);
		if (ret < 0)
			return ret;
		ret = AD5940_SEQRegInfoInsert(dev, RegAddr, *RegData);
		if (ret < 0)
			return ret;
	} else {
		*RegData = dev->SeqGenDB.pRegInfo[RegIndex].RegValue;
	}

	return 0;
}

static int AD5940_SEQWriteReg(struct ad5940_dev *dev, uint16_t RegAddr,
			      uint32_t RegData)
{
	int ret;
	uint32_t RegIndex;

	if(RegAddr > 0x21ff) {
		dev->SeqGenDB.LastError = -ERANGE;  /* address out of range  */
		return -ERANGE;
	}

	if(AD5940_SEQGenSearchReg(dev, RegAddr, &RegIndex) == 0) {
		/* Store register value */
		dev->SeqGenDB.pRegInfo[RegIndex].RegValue = RegData;
		/* Generate Sequence command */
		ret = ad5940_SEQGenInsert(dev, SEQ_WR(RegAddr, RegData));
	} else {
		ret = AD5940_SEQRegInfoInsert(dev, RegAddr, RegData);
		if (ret < 0)
			return ret;

		/* Generate Sequence command */
		ret = ad5940_SEQGenInsert(dev, SEQ_WR(RegAddr, RegData));
	}

	return ret;
}

int ad5940_SEQGenInit(struct ad5940_dev *dev, uint32_t *pBuffer,
		      uint32_t BufferSize)
{
	if(BufferSize < 2)
		return -EINVAL;
	dev->SeqGenDB.BufferSize = BufferSize;
	dev->SeqGenDB.pSeqBuff = pBuffer;
	dev->SeqGenDB.pRegInfo = (SEQGenRegInfo_Type*)pBuffer + BufferSize -
				 1; /* Point to the last element in buffer */
	dev->SeqGenDB.SeqLen = 0;

	dev->SeqGenDB.RegCount = 0;
	dev->SeqGenDB.LastError = 0;
	dev->SeqGenDB.EngineStart = false;

	return 0;
}

int ad5940_SEQGenFetchSeq(struct ad5940_dev *dev, const uint32_t **ppSeqCmd,
			  uint32_t *pSeqLen)
{
	int lasterror;

	if(ppSeqCmd)
		*ppSeqCmd = dev->SeqGenDB.pSeqBuff;
	if(pSeqLen)
		*pSeqLen = dev->SeqGenDB.SeqLen;

	//dev->SeqGenDB.SeqLen = 0;  /* Start a new sequence */
	lasterror = dev->SeqGenDB.LastError;
	//dev->SeqGenDB.LastError = 0;  /* Clear error message */
	return lasterror;
}

/* Enable or disable sequence generator */
int ad5940_SEQGenCtrl(struct ad5940_dev *dev, bool enable)
{
	dev->SeqGenDB.EngineStart = enable;
	if (enable) {
		dev->SeqGenDB.SeqLen = 0;
		dev->SeqGenDB.LastError = 0;  /* Clear error message */
	}
	return 0;
}

/**
 * @} Sequencer_Generator_Functions
 */

/**
   @brief void AD5940_ClksCalculate(ClksCalInfo_Type *pFilterInfo, uint32_t *pClocks)
          ====== Calculate how much clocks are needed in sequencer wait command to generate required number of data.
   @param pFilterInfo : {0 - 0xffffffff}
          - Pointer to configuration structure.
   @param pClocks : {0 - 0xffffffff}
          - pointer used to store results.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ClksCalculate(struct ad5940_dev *dev, ClksCalInfo_Type *pFilterInfo,
			 uint32_t *pClocks)
{
	int ret;
	uint32_t temp = 0;
	const uint32_t sin2osr_table[] = {22,44,89,178,267,533,640,667,800,889,1067,1333,0};
	const uint32_t sinc3osr_table[] = {5,4,2,0};

	*pClocks = 0;
	if(pFilterInfo == NULL) return -EINVAL;
	if(pClocks == NULL) return -EINVAL;
	if(pFilterInfo->ADCSinc2Osr > ADCSINC2OSR_1333) return -EINVAL;
	if(pFilterInfo->ADCSinc3Osr > 2)  return -EINVAL; /* 0: OSR5, 1:OSR4, 2:OSR2 */
	if(pFilterInfo->ADCAvgNum > ADCAVGNUM_16) return
			-EINVAL; /* Average number index:0,1,2,3 */
	switch(pFilterInfo->DataType) {
	case DATATYPE_ADCRAW:
		temp = (uint32_t)(20*pFilterInfo->DataCount*pFilterInfo->RatioSys2AdcClk);
		break;
	case DATATYPE_SINC3:
		temp = (uint32_t)(((pFilterInfo->DataCount+2)
				   *sinc3osr_table[pFilterInfo->ADCSinc3Osr]+1)*20*pFilterInfo->RatioSys2AdcClk +
				  0.5f);
		break;
	case DATATYPE_SINC2:
		temp = (pFilterInfo->DataCount+1)*sin2osr_table[pFilterInfo->ADCSinc2Osr] +
		       1;  /* SINC2 input is SINCS */
		pFilterInfo->DataType = DATATYPE_SINC3;
		pFilterInfo->DataCount = temp;
		ret = ad5940_ClksCalculate(dev, pFilterInfo, &temp);
		if (ret < 0)
			return ret;
		break;
	case DATATYPE_DFT:
		switch(pFilterInfo->DftSrc) {
		case DFTSRC_ADCRAW:
			pFilterInfo->DataType = DATATYPE_ADCRAW;
			ret = ad5940_ClksCalculate(dev, pFilterInfo, &temp);
			if (ret < 0)
				return ret;
			break;
		case DFTSRC_SINC3:
			pFilterInfo->DataType = DATATYPE_SINC3;
			ret = ad5940_ClksCalculate(dev, pFilterInfo, &temp);
			if (ret < 0)
				return ret;
			break;
		case DFTSRC_SINC2NOTCH:
			pFilterInfo->DataType = DATATYPE_SINC2;
			ret = ad5940_ClksCalculate(dev, pFilterInfo, &temp);
			if (ret < 0)
				return ret;
			break;
		case DFTSRC_AVG:
			pFilterInfo->DataType = DATATYPE_SINC3;
			pFilterInfo->DataCount *= 1L<<(pFilterInfo->ADCAvgNum
						       +1); /* 0: average2, 1: average4, 2: average8, 3: average16 */
			ret = ad5940_ClksCalculate(dev, pFilterInfo, &temp);
			if (ret < 0)
				return ret;
			break;
		default:
			break;
		}
		temp += 25; /* @todo add how much clocks are safe enough? */
		break;
	default:
		break;
	}
	*pClocks = temp;
	return 0;
}


/**
   @brief void AD5940_SweepNext(SoftSweepCfg_Type *pSweepCfg, float *pNextFreq)
          For sweep function, calculate next frequency point according to pSweepCfg info.
   @return Return next frequency point in Hz.
 */
void ad5940_SweepNext(struct ad5940_dev *dev, SoftSweepCfg_Type *pSweepCfg,
		      float *pNextFreq)
{
	float frequency;

	if(pSweepCfg->SweepLog) { /* Log step */
		if(pSweepCfg->SweepStart<pSweepCfg->SweepStop) { /* Normal */
			if(++pSweepCfg->SweepIndex == pSweepCfg->SweepPoints)
				pSweepCfg->SweepIndex = 0;
			frequency = pSweepCfg->SweepStart*pow(10,
							      pSweepCfg->SweepIndex*log10(pSweepCfg->SweepStop/pSweepCfg->SweepStart)/
							      (pSweepCfg->SweepPoints-1));
		} else {
			pSweepCfg->SweepIndex --;
			if(pSweepCfg->SweepIndex >= pSweepCfg->SweepPoints)
				pSweepCfg->SweepIndex = pSweepCfg->SweepPoints-1;
			frequency = pSweepCfg->SweepStop*pow(10,pSweepCfg->SweepIndex*
							     (log10(pSweepCfg->SweepStart/pSweepCfg->SweepStop)/(pSweepCfg->SweepPoints-1)));
		}
	} else { /* Linear step */
		if(pSweepCfg->SweepStart<pSweepCfg->SweepStop) { /* Normal */
			if(++pSweepCfg->SweepIndex == pSweepCfg->SweepPoints)
				pSweepCfg->SweepIndex = 0;
			frequency = pSweepCfg->SweepStart + pSweepCfg->SweepIndex*(double)(
					    pSweepCfg->SweepStop-pSweepCfg->SweepStart)/(pSweepCfg->SweepPoints-1);
		} else {
			pSweepCfg->SweepIndex --;
			if(pSweepCfg->SweepIndex >= pSweepCfg->SweepPoints)
				pSweepCfg->SweepIndex = pSweepCfg->SweepPoints-1;
			frequency = pSweepCfg->SweepStop + pSweepCfg->SweepIndex*(double)(
					    pSweepCfg->SweepStart - pSweepCfg->SweepStop)/(pSweepCfg->SweepPoints-1);
		}
	}

	*pNextFreq = frequency;
}

/* Initialize Structure members to zero */
static void AD5940_StructInit(void *pStruct, uint32_t StructSize)
{
	memset(pStruct, 0, StructSize);
}

fImpCar_Type ad5940_ComplexDivFloat(fImpCar_Type *a, fImpCar_Type *b)
{
	fImpCar_Type res;
	float temp;
	temp = b->Real*b->Real + b->Image*b->Image;
	res.Real = a->Real*b->Real + a->Image*b->Image;
	res.Real /= temp;
	res.Image = a->Image*b->Real - a->Real*b->Image;
	res.Image /= temp;
	return res;
}

fImpCar_Type ad5940_ComplexMulFloat(fImpCar_Type *a, fImpCar_Type *b)
{
	fImpCar_Type res;

	res.Real = a->Real*b->Real - a->Image*b->Image;
	res.Image = a->Image*b->Real + a->Real*b->Image;

	return res;
}

fImpCar_Type ad5940_ComplexAddFloat(fImpCar_Type *a, fImpCar_Type *b)
{
	fImpCar_Type res;

	res.Real = a->Real + b->Real;
	res.Image = a->Image + b->Image;

	return res;
}

fImpCar_Type ad5940_ComplexSubFloat(fImpCar_Type *a, fImpCar_Type *b)
{
	fImpCar_Type res;

	res.Real = a->Real - b->Real;
	res.Image = a->Image - b->Image;

	return res;
}

fImpCar_Type ad5940_ComplexDivInt(iImpCar_Type *a, iImpCar_Type *b)
{
	fImpCar_Type res;
	float temp;
	temp = (float)b->Real*b->Real + (float)b->Image*b->Image;
	res.Real = (float)a->Real*b->Real + (float)a->Image*b->Image;
	res.Real /= temp;
	res.Image = (float)a->Image*b->Real - (float)a->Real*b->Image;
	res.Image /= temp;
	return res;
}

fImpCar_Type ad5940_ComplexMulInt(iImpCar_Type *a, iImpCar_Type *b)
{
	fImpCar_Type res;

	res.Real = (float)a->Real*b->Real - (float)a->Image*b->Image;
	res.Image = (float)a->Image*b->Real + (float)a->Real*b->Image;

	return res;
}

float ad5940_ComplexMag(fImpCar_Type *a)
{
	return sqrt(a->Real*a->Real + a->Image*a->Image);
}

float ad5940_ComplexPhase(fImpCar_Type *a)
{
	return atan2(a->Image, a->Real);
}

static int AD5940_SPIWriteReg(struct no_os_spi_desc *spi, uint16_t RegAddr,
			      uint32_t RegData)
{
	int ret;
	uint8_t iobuf[5];
	unsigned int i = 0;
	iobuf[i++] = SPICMD_SETADDR;
	iobuf[i++] = RegAddr >> 8;
	iobuf[i++] = RegAddr & 0xff;

	ret = no_os_spi_write_and_read(spi, iobuf, i);
	if (ret)
		return ret;

	i = 0;
	iobuf[i++] = SPICMD_WRITEREG;

	if(((RegAddr>=0x1000)&&(RegAddr<=0x3014))) {
		iobuf[i++] = RegData >> 24;
		iobuf[i++] = RegData >> 16;
	}

	iobuf[i++] = RegData >> 8;
	iobuf[i++] = RegData & 0xff;

	return no_os_spi_write_and_read(spi, iobuf, i);
}

static int AD5940_SPIReadReg(struct no_os_spi_desc *spi, uint16_t RegAddr,
			     uint32_t *RegData)
{
	int ret;
	uint8_t iobuf[6];
	unsigned int i = 0;
	iobuf[i++] = SPICMD_SETADDR;

	iobuf[i++] = RegAddr >> 8;
	iobuf[i++] = RegAddr & 0xff;

	ret = no_os_spi_write_and_read(spi, iobuf, i);
	if (ret)
		return ret;

	i = 0;
	iobuf[i++] = SPICMD_READREG;
	iobuf[i++] = 0; // don't care

	if(((RegAddr>=0x1000)&&(RegAddr<=0x3014))) {
		iobuf[i++] = 0; // don't care
		iobuf[i++] = 0; // don't care
	}

	iobuf[i++] = 0; // don't care
	iobuf[i++] = 0; // don't care

	ret = no_os_spi_write_and_read(spi, iobuf, i);
	if (ret)
		return ret;

	*RegData = 0;
	i = 2;
	if(((RegAddr>=0x1000)&&(RegAddr<=0x3014))) {
		*RegData |= iobuf[i++] << 24;
		*RegData |= iobuf[i++] << 16;
	}
	*RegData |= iobuf[i++] << 8;
	*RegData |= iobuf[i++] & 0xff;

	return 0;
}

static int AD5940_FIFORd_Fast(struct no_os_spi_desc *spi, uint32_t *pBuffer,
			      uint32_t uiReadCount)
{
	int ret;
	static uint32_t iobuf_alloc_sz = 0;
	uint32_t iobuf_sz = 7 + uiReadCount * sizeof(uiReadCount);
	static uint8_t *iobuf;
	uint8_t *buf;
	uint32_t i = 0;
	uint32_t s = 0;

	if (!iobuf_alloc_sz) {
		iobuf = no_os_malloc(iobuf_sz);
		if (!iobuf)
			return -ENOMEM;
	}

	if (iobuf_alloc_sz < iobuf_sz) {
		buf = realloc(iobuf, iobuf_sz);
		if (!buf)
			return -ENOMEM;

		iobuf = buf;
	}

	// zero-out everything, needed for bytes 1 through 6 (dummy bytes).
	memset(iobuf, 0, iobuf_sz);
	// set the command.
	iobuf[0] = SPICMD_READFIFO;
	// set the MOSI output during last two samples to 0x44444444 for each.
	memset(&iobuf[iobuf_sz - 8], 0x44, 8);

	ret = no_os_spi_write_and_read(spi, iobuf, iobuf_sz);
	if (ret)
		return ret;

	for (i = 7; i < iobuf_sz; i += sizeof(uiReadCount))
		pBuffer[s++] = iobuf[i] << 24 |
			       iobuf[i + 1] << 16 |
			       iobuf[i + 2] << 8 |
			       iobuf[i + 3];
	return 0;
}

/**
  @brief Read specific number of data from FIFO with optimized SPI access.
  @param pBuffer: Pointer to a buffer that used to store data read back.
  @param uiReadCount: Hou much data to be read.
  @return 0 in case of success, negative error code otherwise.
 **/
int ad5940_FIFORd(struct ad5940_dev *dev, uint32_t *pBuffer,
		  uint32_t uiReadCount)
{
	int ret;
	if (!dev)
		return -EINVAL;

	if(uiReadCount < 4) {
		uint8_t i;
		for (i = 0; i < uiReadCount; i++) {
			ret = AD5940_SPIReadReg(dev->spi, REG_AFE_DATAFIFORD, &pBuffer[i]);
			if (ret)
				break;
		}
	} else {
		uint8_t iobuf[] = {SPICMD_SETADDR, 0, 0, 0, 0, (uint16_t)REG_AFE_DATAFIFORD >> 8, (uint8_t)REG_AFE_DATAFIFORD};
		ret = no_os_spi_write_and_read(dev->spi, iobuf, sizeof(iobuf));
		if (ret)
			return ret;

		ret = AD5940_FIFORd_Fast(dev->spi, pBuffer, uiReadCount);
	}

	return ret;
}

/** Write to address @ref RegAddr with data @RegData  */
int ad5940_WriteReg(struct ad5940_dev *dev, uint16_t RegAddr, uint32_t RegData)
{
	if (!dev)
		return -EINVAL;

	if(dev->SeqGenDB.EngineStart == true)
		return AD5940_SEQWriteReg(dev, RegAddr, RegData);
	else
		return AD5940_SPIWriteReg(dev->spi, RegAddr, RegData);
}

/** Read register data from address @ref RegAddr */
int ad5940_ReadReg(struct ad5940_dev *dev, uint16_t RegAddr, uint32_t *RegData)
{
	if (!dev)
		return -EINVAL;

	if(dev->SeqGenDB.EngineStart == true)
		return AD5940_SEQReadReg(dev, RegAddr, RegData);
	else
		return AD5940_SPIReadReg(dev->spi, RegAddr, RegData);
}

/** Write only masked bits to address @ref RegAddr with data @RegData  */
int ad5940_WriteReg_mask(struct ad5940_dev *dev, uint16_t RegAddr,
			 uint32_t mask, uint32_t RegData)
{
	int ret;
	uint32_t reg;

	ret = ad5940_ReadReg(dev, RegAddr, &reg);
	if (ret)
		return ret;

	reg &= ~mask;
	reg |= RegData;
	return ad5940_WriteReg(dev, RegAddr, reg);
}
/**
 * @} SPI_Block_Functions
 * @} SPI_Block
 */


/**
 * @defgroup AFE_Control
 * @brief Some functions to control the whole AFE. They are top level switches.
 * @{
 *    @defgroup AFE_Control_Functinos
 *    The top-level control functions for whole AFE perspective.
 *    @details  This function set is used to control the whole AFE block by block. It's a top-level configuration.
 *              It's convient when do initialization work with the functions called BLOCK**Cfg**. You can tune the parameters at run-time using more detailed
 *              functions from each block. rather than top-level functions where you need to configure all parameters.
 *    @{
 */

/**
 * @brief void AD5940_Initialize(void)
 *          ========== Some new features(not by default) and bugs need software to deal with.
 * @return return 1
 **/
static int AD5940_Initialize(struct ad5940_dev *dev)
{
	int ret;

	ret = ad5940_WriteReg(dev, 0x0908, 0x2c9);   /* Stop watchdog timer */
	if (ret)
		return ret;

	/*
	This register default value should be 0x10, and must be 0x10 when repeat function is not used.
	 */
	ret = ad5940_WriteReg(dev, REG_AFE_REPEATADCCNV, 0x10);
	if (ret)
		return ret;

	/*
	Gate unused clock to save power.
	 */
	ret = ad5940_WriteReg(dev, REG_AFECON_CLKEN1, 0x2c9);
	if (ret)
		return ret;

	/*
	Set ADC Chop function @todo add register.
	 */
	ret = ad5940_WriteReg(dev, 0x238c, 0x104); /* Set ADC Chop functions */
	if (ret)
		return ret;

	/* Enable CSpin falling edge to wakeup AFE */
	ret = ad5940_WriteReg(dev, REG_ALLON_EI2CON,
			      BITM_ALLON_EI2CON_BUSINTEN|(1 << BITP_ALLON_EI2CON_BUSINTMDE));
	if (ret)
		return ret;

	ret = ad5940_WriteReg(dev, 0xa04, 0x4859); /* REG_ALLON_PWRKEY */
	if (ret)
		return ret;

	ret = ad5940_WriteReg(dev, 0xa04, 0xF27B);
	if (ret)
		return ret;

	ret = ad5940_WriteReg(dev, 0xa00,
			      0x8009); /* REG_ALLON_PWRMOD,  Enable RAM retain function and RAM Retention function */
	if (ret)
		return ret;

	/* Enable PMBW to control AFE power mode and Bandwidth */
	return ad5940_WriteReg(dev, REG_AFE_PMBW,
			       0); /** @todo Remove this once OTP corrected. Clear bit19, bit15, bit11 to allow PMBW to control RCF, AAF, HSTIA */
}

/**
 * @brief Control most AFE digital and analog block within one register access.
 * @param AfeCtrlSet: A set of blocks that will be controlled select it from @ref AFECtrlSet Below is two examples to use it.
 *        - AFECTRL_HPREFPWR: Control high power reference(bandgap).
 *        - AFECTRL_WG|AFECTRL_ADCPWR: The OR'ed control set. Control Waveform generator and ADC power.
 * @param State: Enable or disable seletect control set signal. Select from @bool
 *        - false: Disable or power down selected block(s).
 *        - true:  Enalbe all selected block(s).
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_AFECtrlS(struct ad5940_dev *dev, uint32_t AfeCtrlSet, bool State)
{
	int ret;
	uint32_t tempreg;

	ret = ad5940_ReadReg(dev, REG_AFE_AFECON, &tempreg);
	if (ret)
		return ret;

	if (State == true) {
		/* Clear bits to enable HPREF and ALDOLimit*/
		if (AfeCtrlSet & AFECTRL_HPREFPWR) {
			tempreg &= ~BITM_AFE_AFECON_HPREFDIS;
			AfeCtrlSet &= ~AFECTRL_HPREFPWR;
		}
		if(AfeCtrlSet & AFECTRL_ALDOLIMIT) {
			tempreg &= ~BITM_AFE_AFECON_ALDOILIMITEN;
			AfeCtrlSet &= ~AFECTRL_ALDOLIMIT;
		}
		tempreg |= AfeCtrlSet;
	} else {
		/* Set bits to Disable HPREF and ALDOLimit*/
		/*! @todo check ALDOLimit bit definition. Set to enalbe limitation or clear. */
		if(AfeCtrlSet & AFECTRL_HPREFPWR) {
			tempreg |= BITM_AFE_AFECON_HPREFDIS;
			AfeCtrlSet &= ~AFECTRL_HPREFPWR;
		}
		if(AfeCtrlSet & AFECTRL_ALDOLIMIT) {
			tempreg |= BITM_AFE_AFECON_ALDOILIMITEN;
			AfeCtrlSet &= ~AFECTRL_ALDOLIMIT;
		}
		tempreg &= ~AfeCtrlSet;
	}
	return ad5940_WriteReg(dev, REG_AFE_AFECON, tempreg);
}
/** When LP mode is enalbed, some functions are under control of LPMODECON, rather than original registers.  */
/** @warning LPMODE is key protected, this function only takes effect after AD5940_LPModeEnS(true) */
/**
 * @brief For LP mode, use one register to control most AFE digital and analog block.
 * @details The parameter means the blocks. The selected block will be enalbed. All others will be disabled.
 *          The method to enable/disable blocks are defined by register LPMODECON, either by clearing or setting bits.
 * @param EnSet: A set of blocks that will be enabled. Select it from @ref LPMODECTRL_Const. All others not selected in EnSet will be disabled.
 *        - LPMODECTRL_ALDOPWR|LPMODECTRL_HFOSCEN: Turn on ALDO and HFOSC, disable all others.
 *        - LPMODECTRL_ALL: Enalbe all blocks.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPModeCtrlS(struct ad5940_dev *dev, uint32_t EnSet)
{
	int ret;
	uint32_t tempreg;
	uint32_t DisSet; /* The blocks to be disabled */

	DisSet = LPMODECTRL_ALL & (~EnSet);
	ret = ad5940_ReadReg(dev, REG_AFE_LPMODECON, &tempreg);
	if (ret)
		return ret;

	/* Enable selected set */
	{
		/* Clear bits to enable HFOSC, HPREF, ALDO */
		if (EnSet & LPMODECTRL_HFOSCEN) {
			tempreg &= ~BITM_AFE_LPMODECON_HFOSCPD;
			EnSet &= ~LPMODECTRL_HFOSCEN;
		}
		if(EnSet & LPMODECTRL_HPREFPWR) {
			tempreg &= ~BITM_AFE_LPMODECON_HPREFDIS;
			EnSet &= ~LPMODECTRL_HPREFPWR;
		}
		if(EnSet & LPMODECTRL_ALDOPWR) {
			tempreg &= ~BITM_AFE_LPMODECON_ALDOEN;
			EnSet &= ~LPMODECTRL_ALDOPWR;
		}
		tempreg |= EnSet; /* Set other bits to enable function */
	}
	/* Disable other blocks */
	{
		/* Set bits to disable HFOSC, HPREF, ALDO */
		if (DisSet & LPMODECTRL_HFOSCEN) {
			tempreg |= BITM_AFE_LPMODECON_HFOSCPD;
			DisSet &= ~LPMODECTRL_HFOSCEN;
		}
		if(DisSet & LPMODECTRL_HPREFPWR) {
			tempreg |= BITM_AFE_LPMODECON_HPREFDIS;
			DisSet &= ~LPMODECTRL_HPREFPWR;
		}
		if(DisSet & LPMODECTRL_ALDOPWR) {
			tempreg |= BITM_AFE_LPMODECON_ALDOEN;
			DisSet &= ~LPMODECTRL_ALDOPWR;
		}
		tempreg &= ~DisSet; /* Clear other bits to disable function */
	}
	return ad5940_WriteReg(dev, REG_AFE_LPMODECON, tempreg);
}

/**
   @brief void AD5940_AFEPwrBW(uint32_t AfePwr, uint32_t AfeBw)
          ====== Set AFE power mode and system bandwidth
   @param AfePwr : {AFEPWR_LP, AFEPWR_HP}
          Select parameters from @ref AFEPowerMode
          - AFEPWR_LP: Set AFE to low power mode
          - AFEPWR_HP: Set AFE to High speed mode to support 200kHz.
   @param AfeBw : {AFEBW_NOACT, AFEBW_50KHZ, AFEBW_100KHZ, AFEBW_250KHZ}
          - AFEBW_NOACT: Keep current system bandwidth settings
          - AFEBW_50KHZ: Set system bandwidth to 50kHz.
          - AFEBW_100KHZ: Set system bandwidth to 100kHz.
          - AFEBW_250KHZ: Set system bandwidth to 250kHz.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_AFEPwrBW(struct ad5940_dev *dev, uint32_t AfePwr, uint32_t AfeBw)
{
	uint32_t tempreg;
	tempreg = AfePwr;
	tempreg |= AfeBw << BITP_AFE_PMBW_SYSBW;
	return ad5940_WriteReg(dev, REG_AFE_PMBW, tempreg);
}


/**
   @brief void AD5940_REFCfgS(RefBufCfg_Type *pBufCfg)
          ======Configure reference buffer include 1.8V/1.1V high/low power buffers.
   @param pBufCfg : {0 - 0xffffffff}
          - Pointer to buffer configure structure;
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_REFCfgS(struct ad5940_dev *dev, AFERefCfg_Type *pBufCfg)
{
	int ret;
	uint32_t tempreg;

	/* HP Reference(bandgap) */
	ret = ad5940_ReadReg(dev, REG_AFE_AFECON, &tempreg);
	if (ret < 0)
		return ret;

	tempreg &= ~BITM_AFE_AFECON_HPREFDIS;
	if(pBufCfg->HpBandgapEn == false)
		tempreg |= BITM_AFE_AFECON_HPREFDIS;
	ret = ad5940_WriteReg(dev, REG_AFE_AFECON, tempreg);
	if (ret < 0)
		return ret;

	/* Reference buffer configure */
	ret = ad5940_ReadReg(dev, REG_AFE_BUFSENCON, &tempreg);
	if (ret < 0)
		return ret;

	if(pBufCfg->Hp1V8BuffEn == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P8HPADCEN;
	if(pBufCfg->Hp1V1BuffEn == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P1HPADCEN;
	if(pBufCfg->Lp1V8BuffEn == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P8LPADCEN;
	if(pBufCfg->Lp1V1BuffEn == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P1LPADCEN;
	if(pBufCfg->Hp1V8ThemBuff == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P8THERMSTEN;
	if(pBufCfg->Hp1V8Ilimit == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P8HPADCILIMITEN;
	if(pBufCfg->Disc1V8Cap == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P8HPADCCHGDIS;
	if(pBufCfg->Disc1V1Cap == true)
		tempreg |= BITM_AFE_BUFSENCON_V1P1LPADCCHGDIS;
	ret = ad5940_WriteReg(dev, REG_AFE_BUFSENCON, tempreg);
	if (ret < 0)
		return ret;

	/* LPREFBUFCON */
	tempreg = 0;
	if(pBufCfg->LpRefBufEn == false)
		tempreg |= BITM_AFE_LPREFBUFCON_LPBUF2P5DIS;
	if(pBufCfg->LpBandgapEn == false)
		tempreg |= BITM_AFE_LPREFBUFCON_LPREFDIS;
	if(pBufCfg->LpRefBoostEn == true)
		tempreg |= BITM_AFE_LPREFBUFCON_BOOSTCURRENT;
	return ad5940_WriteReg(dev, REG_AFE_LPREFBUFCON, tempreg);
}
/**
 * @} End of AFE_Control_Functinos
 * @} End of AFE_Control
 * */

/**
 * @defgroup High_Speed_Loop
 * @brief The high speed loop
 * @{
 *    @defgroup High_Speed_Loop_Functions
 *    @{
 */

/**
   @brief void AD5940_HSLoopCfgS(HSLoopCfg_Type *pHsLoopCfg)
          ====== Configure High speed loop(high bandwidth loop or
          called excitation loop). This configuration includes HSDAC, HSTIA and Switch matrix.
   @param pHsLoopCfg : {0 - 0xffffffff}
          - Pointer to configure structure;
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_HSLoopCfgS(struct ad5940_dev *dev, HSLoopCfg_Type *pHsLoopCfg)
{
	int ret;

	ret = ad5940_HSDacCfgS(dev, &pHsLoopCfg->HsDacCfg);
	if (ret < 0)
		return ret;

	ret = ad5940_HSTIACfgS(dev, &pHsLoopCfg->HsTiaCfg);
	if (ret < 0)
		return ret;

	ret = ad5940_SWMatrixCfgS(dev, &pHsLoopCfg->SWMatCfg);
	if (ret < 0)
		return ret;

	return ad5940_WGCfgS(dev, &pHsLoopCfg->WgCfg);
}

/**
   @brief void AD5940_SWMatrixCfgS(SwMatrixCfg_Type *pSwMatrix)
          ====== Initialize switch matrix
   @param pSwMatrix : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SWMatrixCfgS(struct ad5940_dev *dev, SWMatrixCfg_Type *pSwMatrix)
{
	int ret;

	ret = ad5940_WriteReg(dev, REG_AFE_DSWFULLCON, pSwMatrix->Dswitch);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_PSWFULLCON, pSwMatrix->Pswitch);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_NSWFULLCON, pSwMatrix->Nswitch);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_TSWFULLCON, pSwMatrix->Tswitch);
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_AFE_SWCON,
			       BITM_AFE_SWCON_SWSOURCESEL); /* Update switch configuration */

	return 0;
}

/**
   @brief void AD5940_HSDacCfgS(HSDACCfg_Type *pHsDacCfg)
          ====== Initialize HSDAC
   @param pWGInit : {0 - 0xffffffff}
          - Pointer to configuration structure
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_HSDacCfgS(struct ad5940_dev *dev, HSDACCfg_Type *pHsDacCfg)
{
	uint32_t tempreg = 0;

	if(pHsDacCfg->ExcitBufGain == EXCITBUFGAIN_0P25)
		tempreg |= BITM_AFE_HSDACCON_INAMPGNMDE; /* Enable attenuator */
	if(pHsDacCfg->HsDacGain == HSDACGAIN_0P2)
		tempreg |= BITM_AFE_HSDACCON_ATTENEN; /* Enable attenuator */
	tempreg |= (pHsDacCfg->HsDacUpdateRate&0xff)<<BITP_AFE_HSDACCON_RATE;
	return ad5940_WriteReg(dev, REG_AFE_HSDACCON, tempreg);
}

/**
   @brief void AD5940_HSTIACfgS(HSTIACfg_Type *pHsTiaCfg)
          ====== Initialize High speed TIA amplifier
   @param pWGInit : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_HSTIACfgS(struct ad5940_dev *dev, HSTIACfg_Type *pHsTiaCfg)
{
	int ret;
	uint32_t tempreg;

	if(pHsTiaCfg == NULL) return -EINVAL;
	/* Available parameter is 1k, 5k,...,160k, short, OPEN */
	if(pHsTiaCfg->HstiaDeRtia < HSTIADERTIA_1K)
		return -EINVAL;
	if(pHsTiaCfg->HstiaDeRtia > HSTIADERTIA_OPEN)
		return -EINVAL;  /* Parameter is invalid */

	if(pHsTiaCfg->HstiaDeRload > HSTIADERLOAD_OPEN)
		return -EINVAL;  /* Available parameter is OPEN, 0R,..., 100R */

	tempreg = 0;
	tempreg |= pHsTiaCfg->HstiaBias;
	ret = ad5940_WriteReg(dev, REG_AFE_HSTIACON, tempreg);
	if (ret < 0)
		return ret;

	/* HSRTIACON */
	/* Calculate CTIA value */
	tempreg = pHsTiaCfg->HstiaCtia << BITP_AFE_HSRTIACON_CTIACON;
	tempreg |= pHsTiaCfg->HstiaRtiaSel;
	if(pHsTiaCfg->DiodeClose == true)
		tempreg |= BITM_AFE_HSRTIACON_TIASW6CON; /* Close switch 6 */
	ret = ad5940_WriteReg(dev, REG_AFE_HSRTIACON, tempreg);
	if (ret < 0)
		return ret;

	/* HSTIARES03CON */
	tempreg = 0;
	/* deal with HSTIA DE RTIA */
	if(pHsTiaCfg->HstiaDeRtia >= HSTIADERTIA_OPEN)
		tempreg = 0x1f << 3;  /* bit field HPTIRES03CON[7:3] */
	else if(pHsTiaCfg->HstiaDeRtia >= HSTIADERTIA_1K) {
		tempreg = (pHsTiaCfg->HstiaDeRtia - 3 + 11) << 3;
	} else { /* DERTIA 50/100/200Ohm */
		const uint8_t DeRtiaTable[3][5] = {
			/*Rload  0      10    30    50    100 */
			{ 0x00, 0x01, 0x02, 0x03, 0x06,}, /* RTIA 50Ohm */
			{ 0x03, 0x04, 0x05, 0x06, 0x07,},  /* RTIA 100Ohm */
			{ 0x07, 0x07, 0x09, 0x09, 0x0a,} /* RTIA 200Ohm */
		};
		if(pHsTiaCfg->HstiaDeRload < HSTIADERLOAD_OPEN)
			tempreg = (uint32_t)(
					  DeRtiaTable[pHsTiaCfg->HstiaDeRtia][pHsTiaCfg->HstiaDeRload])<<3;
		else
			tempreg = (0x1f)<<3;  /* Set it to HSTIADERTIA_OPEN. This setting is illegal */
	}
	/* deal with HSTIA Rload */
	tempreg |= pHsTiaCfg->HstiaDeRload;

	return ad5940_WriteReg(dev, REG_AFE_DE0RESCON, tempreg);
}

/**
 * @defgroup Waveform_Generator_Functions
 * @{
 */
/**
   @brief void AD5940_WGCfg(WGBaseCfg_Type *pWGInit)
          ====== Initialize waveform generator
   @param pWGInit : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_WGCfgS(struct ad5940_dev *dev, WGCfg_Type *pWGInit)
{
	int ret;
	uint32_t tempreg;

	if(pWGInit->WgType == WGTYPE_SIN) {
		/* Configure Sine wave Generator */
		ret = ad5940_WriteReg(dev, REG_AFE_WGFCW, pWGInit->SinCfg.SinFreqWord);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGAMPLITUDE,
				      pWGInit->SinCfg.SinAmplitudeWord);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGOFFSET, pWGInit->SinCfg.SinOffsetWord);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGPHASE, pWGInit->SinCfg.SinPhaseWord);
		if (ret < 0)
			return ret;
	} else if(pWGInit->WgType == WGTYPE_TRAPZ) {
		/* Configure Trapezoid Generator */
		ret = ad5940_WriteReg(dev, REG_AFE_WGDCLEVEL1,
				      pWGInit->TrapzCfg.WGTrapzDCLevel1);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGDCLEVEL2,
				      pWGInit->TrapzCfg.WGTrapzDCLevel2);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGDELAY1, pWGInit->TrapzCfg.WGTrapzDelay1);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGDELAY2, pWGInit->TrapzCfg.WGTrapzDelay2);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGSLOPE1, pWGInit->TrapzCfg.WGTrapzSlope1);
		if (ret < 0)
			return ret;

		ret = ad5940_WriteReg(dev, REG_AFE_WGSLOPE2, pWGInit->TrapzCfg.WGTrapzSlope2);
		if (ret < 0)
			return ret;
	} else {
		/* Write DAC data. It's only have effect when WgType set to WGTYPE_MMR */
		ret = ad5940_WriteReg(dev, REG_AFE_HSDACDAT, pWGInit->WgCode);
		if (ret < 0)
			return ret;
	}
	tempreg = 0;

	if(pWGInit->GainCalEn == true)
		tempreg |= BITM_AFE_WGCON_DACGAINCAL;
	if(pWGInit->OffsetCalEn == true)
		tempreg |= BITM_AFE_WGCON_DACOFFSETCAL;
	tempreg |= (pWGInit->WgType) << BITP_AFE_WGCON_TYPESEL;
	return ad5940_WriteReg(dev, REG_AFE_WGCON, tempreg);

}

/* Directly write DAC code when WG configured to MMR type */
int ad5940_WGDACCodeS(struct ad5940_dev *dev, uint32_t code)
{
	code &= 0xfff;
	return ad5940_WriteReg(dev, REG_AFE_HSDACDAT, code);
}

int ad5940_WGFreqCtrlS(struct ad5940_dev *dev, float SinFreqHz, float WGClock)
{
	uint32_t freq_word;
	freq_word = ad5940_WGFreqWordCal(SinFreqHz, WGClock);
	return ad5940_WriteReg(dev, REG_AFE_WGFCW, freq_word);
}
/**
   @brief uint32_t AD5940_WGFreqWordCal(float SinFreqHz, float WGClock)
          ====== Calculate sine wave generator frequency word. The maxim frequency is 250kHz-1LSB
   @param SinFreqHz : {0 - 250000}
          - Target frequency in Hz unit.
   @param WGClock : {0 - 0xffffffff}
          - Waveform generator clock frequency in Hz unit. The clock is sourced from system clock.

   @return return 0 in case of success, negative error code otherwise.
 */
uint32_t ad5940_WGFreqWordCal(float SinFreqHz, float WGClock)
{
	uint32_t temp;

	if(WGClock == 0) return 0;
	temp = (uint32_t)(SinFreqHz*(1UL<<__BITWIDTH_WGFCW)/WGClock + 0.5f);
	if(temp > ((__BITWIDTH_WGFCW == 26)?0xfffff:0xffffff))
		temp = (__BITWIDTH_WGFCW == 26)?0xfffff:0xffffff;

	return temp;
}

/**
 * @} Waveform_Generator_Functions
 * @} High_Speed_Loop_Functions
 * @} High_Speed_Loop
 */


/**
 * @defgroup Low_Power_Loop
 * @brief The low power loop.
 * @{
 *    @defgroup Low_Power_Loop_Functions
 *    @{
 */

/**
   @brief void AD5940_LPLoopCfgS(LPLoopCfg_Type *pLpLoopCfg)
          ====== Configure low power loop include LPDAC LPAmp(PA and TIA)
   @param pHsLoopCfg : {0 - 0xffffffff}
          - Pointer to configure structure;
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPLoopCfgS(struct ad5940_dev *dev, LPLoopCfg_Type *pLpLoopCfg)
{
	int ret;

	ret = ad5940_LPDACCfgS(dev, &pLpLoopCfg->LpDacCfg);
	if (ret < 0)
		return ret;

	return ad5940_LPAMPCfgS(dev, &pLpLoopCfg->LpAmpCfg);
}

/**
   @brief void AD5940_LPDACCfgS(LPDACCfg_Type *pLpDacCfg)
          ====== Initialize LPDAC
   @param pLpDacCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPDACCfgS(struct ad5940_dev *dev, LPDACCfg_Type *pLpDacCfg)
{
	int ret;
	uint32_t tempreg = 0;

	tempreg = (pLpDacCfg->LpDacSrc)<<BITP_AFE_LPDACCON0_WAVETYPE;
	tempreg |= (pLpDacCfg->LpDacVzeroMux)<<BITP_AFE_LPDACCON0_VZEROMUX;
	tempreg |= (pLpDacCfg->LpDacVbiasMux)<<BITP_AFE_LPDACCON0_VBIASMUX;
	tempreg |= (pLpDacCfg->LpDacRef)<<BITP_AFE_LPDACCON0_REFSEL;
	if(pLpDacCfg->DataRst == false)
		tempreg |= BITM_AFE_LPDACCON0_RSTEN;
	if(pLpDacCfg->PowerEn == false)
		tempreg |= BITM_AFE_LPDACCON0_PWDEN;
	ret = ad5940_WriteReg(dev, REG_AFE_LPDACCON0, tempreg);
	if (ret < 0)
		return ret;

	ret = ad5940_LPDACWriteS(dev, pLpDacCfg->DacData12Bit, pLpDacCfg->DacData6Bit);
	if (ret)
		return ret;

	/* Overwrite LPDACSW settings. On Si1, this register is not accessible. */
	return ad5940_WriteReg(dev, REG_AFE_LPDACSW0,
			       pLpDacCfg->LpDacSW|BITM_AFE_LPDACSW0_LPMODEDIS);
}

/**
   @brief void AD5940_LPDACWriteS(uint16_t Data12Bit, uint8_t Data6Bit)
          ====== Write LPDAC data
   @param Data12Bit : {0 - 4095}
          - 12Bit DAC data
   @param Data6Bit : {0 - 63}
          - 6Bit DAC data
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPDACWriteS(struct ad5940_dev *dev, uint16_t Data12Bit,
		       uint8_t Data6Bit)
{
	Data6Bit &= 0x3f;
	Data12Bit &= 0xfff;
	return ad5940_WriteReg(dev, REG_AFE_LPDACDAT0,
			       ((uint32_t)Data6Bit<<12)|Data12Bit);
}

/**
   @brief void AD5940_LPAMPCfgS(LpAmpCfg_Type *pLpAmpCfg)
          ====== Initialize LP TIA and PA
   @param pLpDacCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPAMPCfgS(struct ad5940_dev *dev, LPAmpCfg_Type *pLpAmpCfg)
{
	int ret;
	uint32_t tempreg = 0;

	if(pLpAmpCfg->LpPaPwrEn == false)
		tempreg |= BITM_AFE_LPTIACON0_PAPDEN;
	if(pLpAmpCfg->LpTiaPwrEn == false)
		tempreg |= BITM_AFE_LPTIACON0_TIAPDEN;
	if(pLpAmpCfg->LpAmpPwrMod == LPAMPPWR_HALF)
		tempreg |= BITM_AFE_LPTIACON0_HALFPWR;
	else {
		tempreg |= pLpAmpCfg->LpAmpPwrMod<<BITP_AFE_LPTIACON0_IBOOST;
	}
	tempreg |= pLpAmpCfg->LpTiaRtia<<BITP_AFE_LPTIACON0_TIAGAIN;
	tempreg |= pLpAmpCfg->LpTiaRload<<BITP_AFE_LPTIACON0_TIARL;
	tempreg |= pLpAmpCfg->LpTiaRf<<BITP_AFE_LPTIACON0_TIARF;
	ret = ad5940_WriteReg(dev, REG_AFE_LPTIACON0, tempreg);
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_AFE_LPTIASW0, pLpAmpCfg->LpTiaSW);
}


/**
 * @} Low_Power_Loop_Functions
 * @} Low_Power_Loop
 */


/**
 * @defgroup DSP_Block
 * @brief DSP block includes ADC, filters, DFT and statistic functions.
 * @{
 *    @defgroup DSP_Block_Functions
 *    @{
 * */

/**
   @brief void AD5940_DSPCfgS(LPLoopCfg_Type *pLpLoopCfg)
          ====== Configure low power loop include LPDAC LPAmp(PA and TIA)
   @param pHsLoopCfg : {0 - 0xffffffff}
          - Pointer to configure structure;
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_DSPCfgS(struct ad5940_dev *dev, DSPCfg_Type *pDSPCfg)
{
	int ret;

	ret = ad5940_ADCBaseCfgS(dev, &pDSPCfg->ADCBaseCfg);
	if (ret < 0)
		return ret;

	ad5940_ADCFilterCfgS(dev, &pDSPCfg->ADCFilterCfg);
	if (ret < 0)
		return ret;

	ad5940_ADCDigCompCfgS(dev, &pDSPCfg->ADCDigCompCfg);
	if (ret < 0)
		return ret;

	ad5940_DFTCfgS(dev, &pDSPCfg->DftCfg);
	if (ret < 0)
		return ret;

	return ad5940_StatisticCfgS(dev, &pDSPCfg->StatCfg);
}

/**
   @brief uint32_t AD5940_ReadAfeResult(uint32_t AfeResultSel)
          ====== Read AFE data
   @param AfeResultSel : {AFERESULT_SINC3, AFERESULT_SINC2, ,,, ,AFERESULT_STATSVAR}
          - AFERESULT_SINC3: Read SINC3 filter data result
          - AFERESULT_SINC2: Read SINC2+NOTCH filter result, when Notch filter is bypassed, the result is SINC2
          - AFERESULT_STATSVAR: Statistic variance result

   @return return data read back.
 */
int ad5940_ReadAfeResult(struct ad5940_dev *dev, uint32_t AfeResultSel,
			 uint32_t *rd)
{
	int ret;
	//PARA_CHECK((AfeResultSel)); ///@todo add parameter check
	switch (AfeResultSel) {
	case AFERESULT_SINC3:
		ret = ad5940_ReadReg(dev, REG_AFE_ADCDAT, rd);
		break;
	case AFERESULT_SINC2:
		ret = ad5940_ReadReg(dev, REG_AFE_SINC2DAT, rd);
		break;
	case AFERESULT_TEMPSENSOR:
		ret = ad5940_ReadReg(dev, REG_AFE_TEMPSENSDAT, rd);
		break;
	case AFERESULT_DFTREAL:
		ret = ad5940_ReadReg(dev, REG_AFE_DFTREAL, rd);
		break;
	case AFERESULT_DFTIMAGE:
		ret = ad5940_ReadReg(dev, REG_AFE_DFTIMAG, rd);
		break;
	case AFERESULT_STATSMEAN:
		ret = ad5940_ReadReg(dev, REG_AFE_STATSMEAN, rd);
		break;
	case AFERESULT_STATSVAR:
		ret = ad5940_ReadReg(dev, REG_AFE_STATSVAR, rd);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

/**
 *  @defgroup ADC_Block_Functions
 *  @{
 */

/**
   @brief void AD5940_ADCBaseCfgS(ADCBaseCfg_Type *pADCInit)
          ======Initializes ADC peripheral according to the specified parameters in the pADCInit.
   @param pADCInit : {0 - 0xffffffff}
          - pADCInit: Pointer to ADC initialize structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCBaseCfgS(struct ad5940_dev *dev, ADCBaseCfg_Type *pADCInit)
{
	uint32_t tempreg = 0;
	//PARA_CHECK(IS_ADCMUXP(pADCInit->ADCMuxP));
	//PARA_CHECK(IS_ADCMUXN(pADCInit->ADCMuxN)); ///@todo add parameter check
	PARA_CHECK(IS_ADCPGA(pADCInit->ADCPga));
	PARA_CHECK(IS_ADCAAF(pADCInit->ADCAAF));

	tempreg = pADCInit->ADCMuxP;
	tempreg |= (uint32_t)(pADCInit->ADCMuxN)<<BITP_AFE_ADCCON_MUXSELN;
	//if(pADCInit->OffCancEnable == true)
	//  tempreg |= BITM_AFE_ADCCON_GNOFSELPGA;
	tempreg |= (uint32_t)(pADCInit->ADCPga)<<BITP_AFE_ADCCON_GNPGA;

	return ad5940_WriteReg(dev, REG_AFE_ADCCON, tempreg);
}

/**
   @brief void AD5940_ADCFilterCfgS(ADCFilterCfg_Type *pFiltCfg)
          ======Initializes ADC filter according to the specified parameters in the pFiltCfg.
   @param pFiltCfg : {0 - 0xffffffff}
          - pFiltCfg: Pointer to filter initialize structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCFilterCfgS(struct ad5940_dev *dev, ADCFilterCfg_Type *pFiltCfg)
{
	int ret;
	uint32_t tempreg;
	PARA_CHECK(IS_ADCSINC3OSR(pFiltCfg->ADCSinc3Osr));
	PARA_CHECK(IS_ADCSINC2OSR(pFiltCfg->ADCSinc2Osr));
	PARA_CHECK(IS_ADCAVGNUM(pFiltCfg->ADCAvgNum));
	PARA_CHECK(IS_ADCRATE(pFiltCfg->ADCRate));

	ret = ad5940_ReadReg(dev, REG_AFE_ADCFILTERCON, &tempreg);
	if (ret < 0)
		return ret;

	tempreg &= BITM_AFE_ADCFILTERCON_AVRGEN; /* Keep this bit setting. */

	tempreg |= pFiltCfg->ADCRate;
	if(pFiltCfg->BpNotch == true)
		tempreg |= BITM_AFE_ADCFILTERCON_LPFBYPEN;
	if(pFiltCfg->BpSinc3 == true)
		tempreg |= BITM_AFE_ADCFILTERCON_SINC3BYP;
	//if(pFiltCfg->AverageEnable == true)
	//  tempreg |= BITM_AFE_ADCFILTERCON_AVRGEN;
	tempreg |= (uint32_t)(pFiltCfg->ADCSinc2Osr)<<BITP_AFE_ADCFILTERCON_SINC2OSR;
	tempreg |= (uint32_t)(pFiltCfg->ADCSinc3Osr)<<BITP_AFE_ADCFILTERCON_SINC3OSR;
	tempreg |= (uint32_t)(pFiltCfg->ADCAvgNum)<<BITP_AFE_ADCFILTERCON_AVRGNUM;

	if(pFiltCfg->Sinc2NotchClkEnable ==
	    false)            /* Need to false clock, set bit to 1 to false clock */
		tempreg |= BITM_AFE_ADCFILTERCON_SINC2CLKENB;   /* false SINC2 CLK */

	if(pFiltCfg->WGClkEnable == false)
		tempreg |= BITM_AFE_ADCFILTERCON_DACWAVECLKENB; /* false WG CLK */
	if(pFiltCfg->DFTClkEnable == false)
		tempreg |= BITM_AFE_ADCFILTERCON_DFTCLKENB;     /* false DFT CLK */

	ret = ad5940_WriteReg(dev, REG_AFE_ADCFILTERCON, tempreg);
	if (ret < 0)
		return ret;

	/* SINC2+Notch has a block enable/disable bit in AFECON register */
	if(pFiltCfg->Sinc2NotchEnable) {
		ret = ad5940_AFECtrlS(dev, AFECTRL_SINC2NOTCH,true);
		if (ret < 0)
			return ret;
	}

	return 0;
}

/**
   @brief void AD5940_ADCPowerCtrlS(bool State)
          ======Power up or power down ADC block(including ADC PGA and frontbuf).
   @param State : {true, false}
          - true: Power up ADC
          - false: Power down ADC
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCPowerCtrlS(struct ad5940_dev *dev, bool State)
{
	int ret;
	uint32_t tempreg;
	ret = ad5940_ReadReg(dev, REG_AFE_AFECON, &tempreg);
	if (ret < 0)
		return ret;

	if(State == true) {
		tempreg |= BITM_AFE_AFECON_ADCEN;
	} else {
		tempreg &= ~BITM_AFE_AFECON_ADCEN;
	}
	return ad5940_WriteReg(dev, REG_AFE_AFECON, tempreg);
}

/**
   @brief void AD5940_ADCConvtCtrlS(bool State)
          ======Start or stop ADC convert.
   @param State : {true, false}
          - true: Start ADC convert
          - false: Stop ADC convert
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCConvtCtrlS(struct ad5940_dev *dev, bool State)
{
	int ret;
	uint32_t tempreg;
	ret = ad5940_ReadReg(dev, REG_AFE_AFECON, &tempreg);
	if (ret < 0)
		return ret;

	if(State == true) {
		tempreg |= BITM_AFE_AFECON_ADCCONVEN;
	} else {
		tempreg &= ~BITM_AFE_AFECON_ADCCONVEN;
	}
	return ad5940_WriteReg(dev, REG_AFE_AFECON, tempreg);
}

/**
   @brief void AD5940_ADCMuxCfgS(uint32_t ADCMuxP, uint32_t ADCMuxN)
          ====== Configure ADC input MUX
   @param ADCMuxP : {ADCMUXP_FLOAT, ADCMUXP_HSTIA_P, ,,, ,ADCMUXP_P_NODE}
          - ADCMUXP_FLOAT: float ADC MUX positive input
          - ADCMUXP_HSTIA_P: High speed TIA output sense terminal
          - ADCMUXP_P_NODE: Excitation loop P node
   @param ADCMuxN : {ADCMUXP_FLOAT, ADCMUXP_HSTIA_P, ,,, ,ADCMUXP_P_NODE}
          - ADCMUXP_FLOAT: float ADC MUX positive input
          - ADCMUXP_HSTIA_P: High speed TIA output sense terminal
          - ADCMUXP_P_NODE: Excitation loop P node

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCMuxCfgS(struct ad5940_dev *dev, uint32_t ADCMuxP,
		      uint32_t ADCMuxN)
{
	int ret;
	uint32_t tempreg;
	//PARA_CHECK(IS_ADCMUXP(ADCMuxP));
	//PARA_CHECK(IS_ADCMUXN(ADCMuxN)); ///@todo add parameter check

	ret = ad5940_ReadReg(dev, REG_AFE_ADCCON, &tempreg);
	if (ret < 0)
		return ret;

	tempreg &= ~(BITM_AFE_ADCCON_MUXSELN|BITM_AFE_ADCCON_MUXSELP);
	tempreg |= ADCMuxP<<BITP_AFE_ADCCON_MUXSELP;
	tempreg |= ADCMuxN<<BITP_AFE_ADCCON_MUXSELN;
	return ad5940_WriteReg(dev, REG_AFE_ADCCON, tempreg);
}

/**
   @brief void AD5940_ADCDigCompCfg(ADCDigComp_Type *pCompCfg)
          ====== Set ADC digital comparator function
   @param pCompCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCDigCompCfgS(struct ad5940_dev *dev, ADCDigComp_Type *pCompCfg)
{
	int ret;
	//PARA_CHECK((AfeResultSel)); ///@todo add parameter check
	ret = ad5940_WriteReg(dev, REG_AFE_ADCMIN, pCompCfg->ADCMin);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_ADCMINSM, pCompCfg->ADCMinHys);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_ADCMAX, pCompCfg->ADCMax);
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_AFE_ADCMAXSMEN, pCompCfg->ADCMaxHys);
}
/** @} ADC_Block_Functions */

/**
   @brief void AD5940_StatisticCfgS(StatCfg_Type *pStatCfg)
          ====== Configure statistic functions
   @param pCompCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_StatisticCfgS(struct ad5940_dev *dev, StatCfg_Type *pStatCfg)
{
	uint32_t tempreg = 0;

	if(pStatCfg->StatEnable == true)
		tempreg |= BITM_AFE_STATSCON_STATSEN;
	tempreg |= (pStatCfg->StatSample) << BITP_AFE_STATSCON_SAMPLENUM;
	tempreg |= (pStatCfg->StatDev) << BITP_AFE_STATSCON_STDDEV;
	return ad5940_WriteReg(dev, REG_AFE_STATSCON, tempreg);
}

/**
   @brief void AD5940_ADCRepeatCfgS(uint32_t Number)
          ====== Se ADC Repeat convert function number. Turn off ADC automatically after Number samples of ADC raw data are ready
   @param Number : {0 - 255}
          - Specify after how much ADC raw data need to sample before shutdown ADC

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ADCRepeatCfgS(struct ad5940_dev *dev, uint32_t Number)
{
	//check parameter if(number<255)
	return ad5940_WriteReg(dev, REG_AFE_REPEATADCCNV,
			       Number<<BITP_AFE_REPEATADCCNV_NUM);
}

/**
   @brief void AD5940_DFTCfgS(DFTCfg_Type *pDftCfg)
          ====== Configure DFT number and source and hanning window
   @param pCompCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_DFTCfgS(struct ad5940_dev *dev, DFTCfg_Type *pDftCfg)
{
	int ret;
	uint32_t reg_dftcon, reg_adcfilter;

	reg_dftcon = 0;
	/* Deal with DFTSRC_AVG. Once average function is enabled, it's automatically set as DFT source */
	ret = ad5940_ReadReg(dev, REG_AFE_ADCFILTERCON, &reg_adcfilter);
	if(pDftCfg->DftSrc == DFTSRC_AVG) {
		reg_adcfilter |= BITM_AFE_ADCFILTERCON_AVRGEN;
		ret = ad5940_WriteReg(dev, REG_AFE_ADCFILTERCON, reg_adcfilter);
		if (ret < 0)
			return ret;
	} else {
		/* Disable Average function and set correct DFT source */
		reg_adcfilter &= ~BITM_AFE_ADCFILTERCON_AVRGEN;
		ret = ad5940_WriteReg(dev, REG_AFE_ADCFILTERCON, reg_adcfilter);
		if (ret < 0)
			return ret;

		/* Set new DFT source */
		reg_dftcon |= (pDftCfg->DftSrc) << BITP_AFE_DFTCON_DFTINSEL;
	}
	/* Set DFT number */
	reg_dftcon |= (pDftCfg->DftNum) << BITP_AFE_DFTCON_DFTNUM;

	if(pDftCfg->HanWinEn == true)
		reg_dftcon |= BITM_AFE_DFTCON_HANNINGEN;

	return ad5940_WriteReg(dev, REG_AFE_DFTCON, reg_dftcon);
}

/**
 * @} DSP_Block_Functions
 * @} DSP_Block
 */

/**
 * @defgroup Sequencer_FIFO
 * @brief Sequencer and FIFO.
 * @{
 *    @defgroup Sequencer_FIFO_Functions
 *    @{
 */

/**
   @brief void AD5940_FIFOCfg(FIFOCfg_Type *pFifoCfg)
          ====== Configure AD5940 FIFO
   @param pClkCfg : {0 - 0xffffffff}
          - Pointer to configuration structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_FIFOCfg(struct ad5940_dev *dev, FIFOCfg_Type *pFifoCfg)
{
	int ret;
	uint32_t tempreg;

	ret = ad5940_WriteReg(dev, REG_AFE_FIFOCON, 0);  /* Disable FIFO firstly! */
	if (ret < 0)
		return ret;

	/* CMDDATACON register. Configure this firstly */
	ret = ad5940_ReadReg(dev, REG_AFE_CMDDATACON, &tempreg);
	if (ret < 0)
		return ret;
	tempreg &=
		BITM_AFE_CMDDATACON_CMD_MEM_SEL|BITM_AFE_CMDDATACON_CMDMEMMDE; /* Keep sequencer memory settings */
	tempreg |= pFifoCfg->FIFOMode <<
		   BITP_AFE_CMDDATACON_DATAMEMMDE; 				  /* Data FIFO mode: stream or FIFO */
	tempreg |= pFifoCfg->FIFOSize <<
		   BITP_AFE_CMDDATACON_DATA_MEM_SEL;  		  /* Data FIFO memory size */
	/* The reset memory can be used for sequencer, configure it by function AD5940_SEQCfg() */
	ret = ad5940_WriteReg(dev, REG_AFE_CMDDATACON, tempreg);
	if (ret < 0)
		return ret;

	/* FIFO Threshold */
	ret = ad5940_WriteReg(dev, REG_AFE_DATAFIFOTHRES,
			      pFifoCfg->FIFOThresh << BITP_AFE_DATAFIFOTHRES_HIGHTHRES);
	if (ret < 0)
		return ret;
	/* FIFOCON register. Final step is to enable FIFO */
	tempreg = 0;
	if(pFifoCfg->FIFOEn == true)
		tempreg |=
			BITM_AFE_FIFOCON_DATAFIFOEN;																/* Enable FIFO after everything set. */
	tempreg |= pFifoCfg->FIFOSrc << BITP_AFE_FIFOCON_DATAFIFOSRCSEL;
	return ad5940_WriteReg(dev, REG_AFE_FIFOCON, tempreg);
}

/**
   @brief int AD5940_FIFOGetCfg(FIFOCfg_Type *pFifoCfg)
          ====== Configure AD5940 FIFO
   @param pClkCfg : {0 - 0xffffffff}
          - Pointer to configuration structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_FIFOGetCfg(struct ad5940_dev *dev, FIFOCfg_Type *pFifoCfg)
{
	int ret;
	uint32_t tempreg;

	if(pFifoCfg == NULL)
		return -EINVAL;

	/* CMDDATACON register. */
	ret = ad5940_ReadReg(dev, REG_AFE_CMDDATACON, &tempreg);
	if (ret < 0)
		return ret;

	pFifoCfg->FIFOMode = (tempreg&BITM_AFE_CMDDATACON_DATAMEMMDE)
			     >>BITP_AFE_CMDDATACON_DATAMEMMDE;
	pFifoCfg->FIFOSize = (tempreg&BITM_AFE_CMDDATACON_DATA_MEM_SEL)
			     >>BITP_AFE_CMDDATACON_DATA_MEM_SEL;

	/* FIFO Threshold */
	ret = ad5940_ReadReg(dev, REG_AFE_DATAFIFOTHRES, &tempreg);
	if (ret < 0)
		return ret;

	pFifoCfg->FIFOThresh = (tempreg&BITM_AFE_DATAFIFOTHRES_HIGHTHRES)
			       >>BITP_AFE_DATAFIFOTHRES_HIGHTHRES;
	/* FIFOCON register. */
	ret = ad5940_ReadReg(dev, REG_AFE_FIFOCON, &tempreg);
	if (ret < 0)
		return ret;
	pFifoCfg->FIFOEn = (tempreg&BITM_AFE_FIFOCON_DATAFIFOEN)?true:false;
	pFifoCfg->FIFOSrc = (tempreg&BITM_AFE_FIFOCON_DATAFIFOSRCSEL)
			    >>BITP_AFE_FIFOCON_DATAFIFOSRCSEL;

	return 0;
}

/**
   @brief void AD5940_FIFOCtrlS(uint32_t FifoSrc, bool FifoEn);
          ====== Configure AD5940 FIFO Source and enable it in the same time.
   @param FifoSrc : {FIFOSRC_SINC3, FIFOSRC_DFT, FIFOSRC_SINC2NOTCH, FIFOSRC_VAR, FIFOSRC_MEAN}
          - Pointer to configuration structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_FIFOCtrlS(struct ad5940_dev *dev, uint32_t FifoSrc, bool FifoEn)
{
	uint32_t tempreg;

	tempreg = 0;
	if(FifoEn == true)
		tempreg |= BITM_AFE_FIFOCON_DATAFIFOEN;
	tempreg |= FifoSrc << BITP_AFE_FIFOCON_DATAFIFOSRCSEL;
	return ad5940_WriteReg(dev, REG_AFE_FIFOCON, tempreg);
}

/**
   @brief void AD5940_FIFOThrshSet(uint32_t FIFOThresh)
          ====== Configure AD5940 Data FIFO threshold value
   @param FIFOThresh : {0 - 0xffffffff}
          - FIFO threshold value
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_FIFOThrshSet(struct ad5940_dev *dev, uint32_t FIFOThresh)
{
	/* @todo add parameter check */
	/* FIFO Threshold */
	return ad5940_WriteReg(dev, REG_AFE_DATAFIFOTHRES,
			       FIFOThresh << BITP_AFE_DATAFIFOTHRES_HIGHTHRES);
}

/**
   @brief uint32_t AD5940_FIFOGetCnt(void)
          ====== Get Data count in FIFO
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_FIFOGetCnt(struct ad5940_dev *dev, uint32_t *cnt)
{
	int ret;
	uint32_t tempreg;

	ret = ad5940_ReadReg(dev, REG_AFE_FIFOCNTSTA, &tempreg);
	if (ret < 0)
		return ret;
	*cnt = tempreg >> BITP_AFE_FIFOCNTSTA_DATAFIFOCNTSTA;

	return 0;
}


/* Sequencer */
/**
   @brief void AD5940_SEQCfg(SEQCfg_Type *pSeqCfg)
          ====== Initialize Sequencer
   @param pLpDacCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SEQCfg(struct ad5940_dev *dev, SEQCfg_Type *pSeqCfg)
{
	int ret;
	uint32_t tempreg, fifocon;

	ret = ad5940_ReadReg(dev, REG_AFE_FIFOCON, &fifocon);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AFE_FIFOCON,
			      0);  /* Disable FIFO before changing memory configuration */
	if (ret < 0)
		return ret;

	/* Configure CMDDATACON register */
	ret = ad5940_ReadReg(dev, REG_AFE_CMDDATACON, &tempreg);
	if (ret < 0)
		return ret;
	tempreg &= ~
		   (BITM_AFE_CMDDATACON_CMDMEMMDE|BITM_AFE_CMDDATACON_CMD_MEM_SEL);  /* Clear settings for sequencer memory */
	tempreg |= (1L) <<
		   BITP_AFE_CMDDATACON_CMDMEMMDE;    										  /* Sequencer is always in memory mode */
	tempreg |= (pSeqCfg->SeqMemSize) << BITP_AFE_CMDDATACON_CMD_MEM_SEL;
	ret = ad5940_WriteReg(dev, REG_AFE_CMDDATACON, tempreg);
	if (ret < 0)
		return ret;

	if(pSeqCfg->SeqCntCRCClr) {
		ret = ad5940_WriteReg(dev, REG_AFE_SEQCON, 0);  /* Disable sequencer firstly */
		ret = ad5940_WriteReg(dev, REG_AFE_SEQCNT,
				      0);  /* When sequencer is disabled, any write to SEQCNT will clear CNT and CRC register */
	}
	tempreg = 0;
	if(pSeqCfg->SeqEnable == true)
		tempreg |= BITM_AFE_SEQCON_SEQEN;
	tempreg |= (pSeqCfg->SeqWrTimer) << BITP_AFE_SEQCON_SEQWRTMR;
	ret = ad5940_WriteReg(dev, REG_AFE_SEQCON, tempreg);
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_AFE_FIFOCON,
			       fifocon);  /* restore FIFO configuration */
}
/**
   @brief int AD5940_SEQGetCfg(SEQCfg_Type *pSeqCfg)
          ====== Read back current sequencer configuration and store it to pSeqCfg
   @param pLpDacCfg : {0 - 0xffffffff}
          - Pointer to structure

   @return return 0 if succeed.
 */

int ad5940_SEQGetCfg(struct ad5940_dev *dev, SEQCfg_Type *pSeqCfg)
{
	int ret;
	uint32_t tempreg;
	if(pSeqCfg == NULL)
		return -EINVAL;
	/* Read CMDDATACON register */
	ret = ad5940_ReadReg(dev, REG_AFE_CMDDATACON, &tempreg);
	if (ret < 0)
		return ret;

	pSeqCfg->SeqMemSize = (tempreg&BITM_AFE_CMDDATACON_CMD_MEM_SEL) >>
			      BITP_AFE_CMDDATACON_CMD_MEM_SEL;
	pSeqCfg->SeqCntCRCClr = false; /* Has no meaning */
	/* SEQCON register */
	ret = ad5940_ReadReg(dev, REG_AFE_SEQCON, &tempreg);
	if (ret < 0)
		return ret;

	pSeqCfg->SeqEnable = (tempreg&BITM_AFE_SEQCON_SEQEN)?true:false;
	pSeqCfg->SeqWrTimer = (tempreg&BITM_AFE_SEQCON_SEQWRTMR) >>
			      BITP_AFE_SEQCON_SEQWRTMR;
	return 0;
}

/**
   @brief void AD5940_SEQCtrlS(bool SeqEn)
          ====== Enable or Disable sequencer. Note: only after valid trigger signal, sequencer will run.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SEQCtrlS(struct ad5940_dev *dev, bool SeqEn)
{
	int ret;
	uint32_t tempreg;
	ret = ad5940_ReadReg(dev, REG_AFE_SEQCON, &tempreg);
	if (ret < 0)
		return ret;
	if(SeqEn == true)
		tempreg |= BITM_AFE_SEQCON_SEQEN;
	else
		tempreg &= ~BITM_AFE_SEQCON_SEQEN;

	return ad5940_WriteReg(dev, REG_AFE_SEQCON, tempreg);
}

/**
   @brief void AD5940_SEQHaltS(void)
          ====== Halt sequencer immediately. Use this to debug. In normal application, there is no situation that can use this function.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SEQHaltS(struct ad5940_dev *dev)
{
	return ad5940_WriteReg(dev, REG_AFE_SEQCON,
			       BITM_AFE_SEQCON_SEQHALT|BITM_AFE_SEQCON_SEQEN);
}

/**
   @brief void AD5940_SEQMmrTrig(uint32_t SeqId)
         ========== Trigger sequencer by register write.
   @return return 0 in case of success, negative error code otherwise.

 **/
int ad5940_SEQMmrTrig(struct ad5940_dev *dev, uint32_t SeqId)
{
	if(SeqId > SEQID_3)
		return -EINVAL;
	return ad5940_WriteReg(dev, REG_AFECON_TRIGSEQ, 1<<SeqId);
}

/* Write sequencer commands to AD5940 SRAM */
int ad5940_SEQCmdWrite(struct ad5940_dev *dev, uint32_t StartAddr,
		       const uint32_t *pCommand, uint32_t CmdCnt)
{
	int ret;

	while(CmdCnt--) {
		ret = ad5940_WriteReg(dev, REG_AFE_CMDFIFOWADDR, StartAddr++);
		if (ret < 0)
			return ret;
		ret = ad5940_WriteReg(dev, REG_AFE_CMDFIFOWRITE, *pCommand++);
		if (ret < 0)
			return ret;
	}

	return 0;
}

/**
   @brief void AD5940_SEQInfoCfg(SEQInfo_Type *pSeq)
          ====== Initialize Sequence INFO. There are four set of register record sequence
          command start address in SRAM and sequence length. Hardware has feature to automatically
          manage these four sequences. If the application require more than 4 sequences, user
          manually record the sequence Info(address and length) in MCU.
   @param pSeq : {0 - 0xffffffff}
          - Pointer to configuration structure. Specify sequence start address in SRAM and sequence length.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SEQInfoCfg(struct ad5940_dev *dev, SEQInfo_Type *pSeq)
{
	int ret;
	switch(pSeq->SeqId) {
	case SEQID_0:
		/* Configure SEQINFO register */
		ret = ad5940_WriteReg(dev, REG_AFE_SEQ0INFO,
				      (pSeq->SeqLen<< 16) | pSeq->SeqRamAddr);
		break;
	case SEQID_1:
		ret = ad5940_WriteReg(dev, REG_AFE_SEQ1INFO,
				      (pSeq->SeqLen<< 16) | pSeq->SeqRamAddr);
		break;
	case SEQID_2:
		ret = ad5940_WriteReg(dev, REG_AFE_SEQ2INFO,
				      (pSeq->SeqLen<< 16) | pSeq->SeqRamAddr);
		break;
	case SEQID_3:
		ret = ad5940_WriteReg(dev, REG_AFE_SEQ3INFO,
				      (pSeq->SeqLen<< 16) | pSeq->SeqRamAddr);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	if (ret < 0)
		return ret;

	if(pSeq->WriteSRAM == true) {
		ret = ad5940_SEQCmdWrite(dev, pSeq->SeqRamAddr, pSeq->pSeqCmd, pSeq->SeqLen);
		if (ret < 0)
			return ret;
	}

	return 0;
}

/**
   @brief int AD5940_SEQInfoGet(uint32_t SeqId, SEQInfo_Type *pSeqInfo)
          ====== Get sequence info: start address and sequence length.
   @param SeqId : {SEQID_0, SEQID_1, SEQID_2, SEQID_3}
          - Select which sequence we want to get the information.
   @param pSeq : {0 - 0xffffffff}
          - Pointer to sequence info structure.
   @return return 0 when succeed.
 */
int ad5940_SEQInfoGet(struct ad5940_dev *dev, uint32_t SeqId,
		      SEQInfo_Type *pSeqInfo)
{
	int ret;
	uint32_t tempreg;
	if(pSeqInfo == NULL)
		return -EINVAL;
	switch(SeqId) {
	case SEQID_0:
		ret = ad5940_ReadReg(dev, REG_AFE_SEQ0INFO, &tempreg);
		break;
	case SEQID_1:
		ret = ad5940_ReadReg(dev, REG_AFE_SEQ1INFO, &tempreg);
		break;
	case SEQID_2:
		ret = ad5940_ReadReg(dev, REG_AFE_SEQ2INFO, &tempreg);
		break;
	case SEQID_3:
		ret = ad5940_ReadReg(dev, REG_AFE_SEQ3INFO, &tempreg);
		break;
	default:
		ret = -EINVAL;
		break;
	}
	if (ret < 0)
		return ret;

	pSeqInfo->pSeqCmd =
		0;    /* We don't know where you store the sequence in MCU SRAM */
	pSeqInfo->SeqId = SeqId;
	pSeqInfo->SeqLen = (tempreg>>16)&0x7ff;
	pSeqInfo->SeqRamAddr = tempreg&0x7ff;
	pSeqInfo->WriteSRAM = false;  /* Don't care */

	return 0;
}


/**
   @brief void AD5940_SEQGpioCtrlS(uint32_t Gpio)
         ========== Control GPIO with register SYNCEXTDEVICE. Because sequencer have no ability to access register GPIOOUT.
         So use this register for sequencer.
         This function is normally used for generating sequencer commands.
   @param Gpio : {AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2|AGPIO_Pin3|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6|AGPIO_Pin7}
          - The combination of GPIO pins. The selected pins will be set to High. Others will be pulled low.
   @return return 0 in case of success, negative error code otherwise.

 **/
int ad5940_SEQGpioCtrlS(struct ad5940_dev *dev, uint32_t Gpio)
{
	return ad5940_WriteReg(dev, REG_AFE_SYNCEXTDEVICE, Gpio);
}

/**
   @brief uint32_t AD5940_SEQTimeOutRd(void)
         ========== Read back current count down timer value for Sequencer Timer Out command.
   @return return register value of Sequencer Timer out value.

 **/
int ad5940_SEQTimeOutRd(struct ad5940_dev *dev, uint32_t *cnt)
{
	return ad5940_ReadReg(dev, REG_AFE_SEQTIMEOUT, cnt);
}

/**
   @brief void AD5940_WUPTCfg(WUPTCfg_Type *pWuptCfg)
          ====== Configure Wakeup Timer
   @param pSeq : {0 - 0xffffffff}
          - Pointer to configuration structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_WUPTCfg(struct ad5940_dev *dev, WUPTCfg_Type *pWuptCfg)
{
	int ret;
	uint32_t tempreg;

	/* Sleep and Wakeup time */
	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ0WUPL,
			      (pWuptCfg->SeqxWakeupTime[0] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ0WUPH,
			      (pWuptCfg->SeqxWakeupTime[0] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ0SLEEPL,
			      (pWuptCfg->SeqxSleepTime[0] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ0SLEEPH,
			      (pWuptCfg->SeqxSleepTime[0] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ1WUPL,
			      (pWuptCfg->SeqxWakeupTime[1] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ1WUPH,
			      (pWuptCfg->SeqxWakeupTime[1] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ1SLEEPL,
			      (pWuptCfg->SeqxSleepTime[1] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ1SLEEPH,
			      (pWuptCfg->SeqxSleepTime[1] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ2WUPL,
			      (pWuptCfg->SeqxWakeupTime[2] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ2WUPH,
			      (pWuptCfg->SeqxWakeupTime[2] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ2SLEEPL,
			      (pWuptCfg->SeqxSleepTime[2] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ2SLEEPH,
			      (pWuptCfg->SeqxSleepTime[2] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ3WUPL,
			      (pWuptCfg->SeqxWakeupTime[3] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ3WUPH,
			      (pWuptCfg->SeqxWakeupTime[3] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ3SLEEPL,
			      (pWuptCfg->SeqxSleepTime[3] & 0xFFFF));
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ3SLEEPH,
			      (pWuptCfg->SeqxSleepTime[3] & 0xF0000)>>16);
	if (ret < 0)
		return ret;

	/* TMRCON register */
	//if(pWuptCfg->WakeupEn == true)  /* enable use Wupt to wakeup AFE */
	/* We always allow Wupt to wakeup AFE automatically. */
	ret = ad5940_WriteReg(dev, REG_ALLON_TMRCON, BITM_ALLON_TMRCON_TMRINTEN);
	if (ret < 0)
		return ret;

	/* Wupt order */
	tempreg = 0;
	tempreg |= (pWuptCfg->WuptOrder[0]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQA; /* position A */
	tempreg |= (pWuptCfg->WuptOrder[1]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQB; /* position B */
	tempreg |= (pWuptCfg->WuptOrder[2]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQC; /* position C */
	tempreg |= (pWuptCfg->WuptOrder[3]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQD; /* position D */
	tempreg |= (pWuptCfg->WuptOrder[4]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQE; /* position E */
	tempreg |= (pWuptCfg->WuptOrder[5]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQF; /* position F */
	tempreg |= (pWuptCfg->WuptOrder[6]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQG; /* position G */
	tempreg |= (pWuptCfg->WuptOrder[7]&0x03) <<
		   BITP_WUPTMR_SEQORDER_SEQH; /* position H */
	ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQORDER, tempreg);
	if (ret < 0)
		return ret;

	tempreg = 0;
	if(pWuptCfg->WuptEn == true)
		tempreg |= BITM_WUPTMR_CON_EN;
	//if(pWuptCfg->TrigSeqEn == false)
	/* We always allow Wupt to trigger sequencer */
	///tempreg |= BITM_WUPTMR_CON_MSKTRG;  /* Disable path of trigger signal to sequencer */
	tempreg |= pWuptCfg->WuptEndSeq << BITP_WUPTMR_CON_ENDSEQ;
	//tempreg |= 1<<4;
	return ad5940_WriteReg(dev, REG_WUPTMR_CON, tempreg);
}

/**
   @brief void AD5940_WUPTCtrl(bool Enable)
          ====== Enable or disable wakeup timer
   @param Enable : {true, false}
          - true: enable wakeup timer
          - false: Disable wakeup timer
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_WUPTCtrl(struct ad5940_dev *dev, bool Enable)
{
	int ret;
	uint32_t tempreg;
	ret = ad5940_ReadReg(dev, REG_WUPTMR_CON, &tempreg);
	if (ret < 0)
		return ret;
	tempreg &= ~BITM_WUPTMR_CON_EN;

	if(Enable == true)
		tempreg |= BITM_WUPTMR_CON_EN;

	return ad5940_WriteReg(dev, REG_WUPTMR_CON, tempreg);
}

int ad5940_WUPTTime(struct ad5940_dev *dev, uint32_t SeqId, uint32_t SleepTime,
		    uint32_t WakeupTime)
{
	int ret;
	switch (SeqId) {
	case SEQID_0: {
		ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ0WUPL, (WakeupTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ0WUPH, (WakeupTime & 0xF0000)>>16);
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ0SLEEPL, (SleepTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ0SLEEPH, (SleepTime & 0xF0000)>>16);
		break;
	}
	case SEQID_1: {
		ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ1WUPL, (WakeupTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ1WUPH, (WakeupTime & 0xF0000)>>16);
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ1SLEEPL, (SleepTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ1SLEEPH, (SleepTime & 0xF0000)>>16);
		break;
	}
	case SEQID_2: {
		ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ2WUPL, (WakeupTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ2WUPH, (WakeupTime & 0xF0000)>>16);
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ2SLEEPL, (SleepTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ2SLEEPH, (SleepTime & 0xF0000)>>16);
		break;
	}
	case SEQID_3: {
		ret = ad5940_WriteReg(dev, REG_WUPTMR_SEQ3WUPL, (WakeupTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ3WUPH, (WakeupTime & 0xF0000)>>16);
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ3SLEEPL, (SleepTime & 0xFFFF));
		ret |= ad5940_WriteReg(dev, REG_WUPTMR_SEQ3SLEEPH, (SleepTime & 0xF0000)>>16);
		break;
	}
	default:
		return -EINVAL;
	}
	return ret;
}

/**
 * @} endof Sequencer_FIFO_Functions
 * @} endof Sequencer_FIFO
 */

/**
 * @defgroup MISC_Block
 * @brief Other functions not included in above blocks. Clock, GPIO, INTC etc.
 * @{
 *    @defgroup MISC_Block_Functions
 *    @{
 */

/**
   @brief void AD5940_CLKCfg(CLKCfg_Type *pClkCfg)
          ====== Configure AD5940 clock
   @param pClkCfg : {0 - 0xffffffff}
          - Pointer to configuration structure.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_CLKCfg(struct ad5940_dev *dev, CLKCfg_Type *pClkCfg)
{
	int ret;
	uint32_t tempreg, reg_osccon;

	ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &reg_osccon);
	if (ret < 0)
		return ret;
	/* Enable clocks */
	if(pClkCfg->HFXTALEn == true) {
		reg_osccon |= BITM_ALLON_OSCCON_HFXTALEN;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCKEY,KEY_OSCCON); /* Write Key */
		if (ret < 0)
			return ret;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCCON, reg_osccon); /* Enable HFXTAL */
		if (ret < 0)
			return ret;
		tempreg = 0;
		while(!(tempreg & BITM_ALLON_OSCCON_HFXTALOK)) {
			/* Wait for clock ready */
			ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &tempreg);
			if (ret < 0)
				return ret;
		}
	}

	if(pClkCfg->HFOSCEn == true) {
		reg_osccon |= BITM_ALLON_OSCCON_HFOSCEN;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCKEY,KEY_OSCCON); /* Write Key */
		if (ret < 0)
			return ret;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCCON, reg_osccon); /* Enable HFOSC */
		if (ret < 0)
			return ret;
		tempreg = 0;
		while(!(tempreg & BITM_ALLON_OSCCON_HFOSCOK)) {
			/* Wait for clock ready */
			ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &tempreg);
			if (ret < 0)
				return ret;
		}
	}

	if(pClkCfg->LFOSCEn == true) {
		reg_osccon |= BITM_ALLON_OSCCON_LFOSCEN;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCKEY,KEY_OSCCON); /* Write Key */
		if (ret < 0)
			return ret;
		ret = ad5940_WriteReg(dev, REG_ALLON_OSCCON, reg_osccon); /* Enable LFOSC */
		if (ret < 0)
			return ret;
		tempreg = 0;
		while(!(tempreg & BITM_ALLON_OSCCON_LFOSCOK)) {
			/* Wait for clock ready */
			ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &tempreg);
			if (ret < 0)
				return ret;
		}
	}

	if(pClkCfg->HfOSC32MHzMode  == true)
		ret = ad5940_HFOSC32MHzCtrl(dev, true);
	else
		ret = ad5940_HFOSC32MHzCtrl(dev, false);

	if (ret < 0)
		return ret;

	/* Switch clocks */
	/* step1. Set clock divider */
	///@todo CLKCON0 bit 10~15 is reserved
	tempreg = pClkCfg->SysClkDiv&0x3f;
	tempreg |= (pClkCfg->SysClkDiv&0x3f) << BITP_AFECON_CLKCON0_SYSCLKDIV;
	ret = ad5940_WriteReg(dev, REG_AFECON_CLKCON0, tempreg);
	if (ret < 0)
		return ret;

	/* Step2. set clock source */
	tempreg = pClkCfg->SysClkSrc;
	tempreg |= pClkCfg->ADCCLkSrc << BITP_AFECON_CLKSEL_ADCCLKSEL;
	ret = ad5940_WriteReg(dev, REG_AFECON_CLKSEL, tempreg);
	if (ret < 0)
		return ret;

	/* Disable clocks */
	if(pClkCfg->HFXTALEn == false)
		reg_osccon &= ~BITM_ALLON_OSCCON_HFXTALEN;
	if(pClkCfg->HFOSCEn == false)
		reg_osccon &= ~BITM_ALLON_OSCCON_HFOSCEN;
	if(pClkCfg->LFOSCEn == false)
		reg_osccon &= ~BITM_ALLON_OSCCON_LFOSCEN;
	ret = ad5940_WriteReg(dev, REG_ALLON_OSCKEY,KEY_OSCCON); /* Write Key */
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_ALLON_OSCCON, reg_osccon);
}

/**
   @brief void AD5940_HFOSC32MHzCtrl(bool Mode32MHz)
          ====== Configure Internal HFOSC to output 32MHz or 16MHz.
   @param Mode32MHz : {true, false}
          - true: HFOSC 32MHz mode.
          - false: HFOSC 16MHz mode.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_HFOSC32MHzCtrl(struct ad5940_dev *dev, bool Mode32MHz)
{
	int ret;
	uint32_t tempreg;
	uint32_t RdCLKEN1;
	uint32_t RdHPOSCCON;

	uint32_t bit8,bit9;

	ret = ad5940_ReadReg(dev, REG_AFECON_CLKEN1, &RdCLKEN1);
	if (ret < 0)
		return ret;

	bit8 = (RdCLKEN1>>9)&0x01;
	bit9 = (RdCLKEN1>>8)
	       &0x01;  /* Fix bug in silicon, bit8 and bit9 is swapped when read back. */
	RdCLKEN1 = RdCLKEN1&0xff;
	RdCLKEN1 |= (bit8<<8)|(bit9<<9);
	ret = ad5940_WriteReg(dev, REG_AFECON_CLKEN1,
			      RdCLKEN1|BITM_AFECON_CLKEN1_ACLKDIS); /* Disable ACLK during clock changing */
	if (ret < 0)
		return ret;

	ret = ad5940_ReadReg(dev, REG_AFE_HPOSCCON, &RdHPOSCCON);
	if (ret < 0)
		return ret;

	if(Mode32MHz == true) {
		ret = ad5940_WriteReg(dev, REG_AFE_HPOSCCON,
				      RdHPOSCCON&
				      (~BITM_AFE_HPOSCCON_CLK32MHZEN)); /* Enable 32MHz output(bit definition-0: 32MHz, 1: 16MHz) */
		if (ret < 0)
			return ret;

		tempreg = 0;
		while(!(tempreg & BITM_ALLON_OSCCON_HFOSCOK)) {
			/* Wait for clock ready */
			ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &tempreg);
			if (ret < 0)
				return ret;
		}
	} else {
		ret = ad5940_WriteReg(dev, REG_AFE_HPOSCCON,
				      RdHPOSCCON|BITM_AFE_HPOSCCON_CLK32MHZEN); /* Enable 16MHz output(bit definition-0: 32MHz, 1: 16MHz) */
		if (ret < 0)
			return ret;

		tempreg = 0;
		while(!(tempreg & BITM_ALLON_OSCCON_HFOSCOK)) {
			/* Wait for clock ready */
			ret = ad5940_ReadReg(dev, REG_ALLON_OSCCON, &tempreg);
			if (ret < 0)
				return ret;
		}
	}

	return ad5940_WriteReg(dev, REG_AFECON_CLKEN1,
			       RdCLKEN1&(~BITM_AFECON_CLKEN1_ACLKDIS)); /* Enable ACLK */
}


/**
 * @defgroup Interrupt_Controller_Functions
 * @{
 */
/* AFE Interrupt Controller */
/**
   @brief void AD5940_INTCCfg(uint32_t AfeIntcSel, uint32_t AFEIntSrc, bool State)
          ====== Enable or Disable selected interrupt source(s)
   @param AfeIntcSel : {AFEINTC_0, AFEINTC_1}
          - AFEINTC_0: Configure Interrupt Controller 0
          - AFEINTC_1: Configure Interrupt Controller 1

   @param AFEIntSrc:{AFE_INTC_ADCRDY|AFE_INTC_DFTRDY|AFE_INTC_SUPPLYFILTRDY|AFE_INTC_TEMPRDY|AFE_INTC_ADCMINERR|AFE_INTC_ADCMAXERR|AFE_INTC_ADCDIFFERR
                   |AFE_INTC_MEANRDY|AFE_INTC_VARRDY|AFE_INTC_DLYCMDDONE|AFE_INTC_HWSETUPDONE|AFE_INTC_BRKSEQ|AFE_INTC_CUSTOMINS|AFE_INTC_BOOTLDDONE
                   |AFE_INTC_WAKEUP|AFE_INTC_ENDSEQ|AFE_INTC_SEQTIMEOUT|AFE_INTC_SEQTIMEOUTERR|AFE_INTC_CMDFIFOFULL|AFE_INTC_CMDFIFOEMPTY|AFE_INTC_CMDFIFOTHRESH
                   |AFE_INTC_CMDFIFOOF|AFE_INTC_CMDFIFOUF|AFE_INTC_DATAFIFOFULL|AFE_INTC_DATAFIFOEMPTY|AFE_INTC_DATAFIFOTHRESH|AFE_INTC_DATAFIFOOF
                   |AFE_INTC_DATAFIFOUF|AFE_INTC_WDTIRQ|AFE_INTC_CRC_OUTLIER|AFE_INTC_GPT0INT_SLPWUT|AFE_INTC_GPT1INT_TRYBRK|AFE_INTC_ALLINT}
         - AFE_INTC_ADCRDY        : Bit0 ADC Result Ready Status
         - AFE_INTC_DFTRDY        : Bit1 DFT Result Ready Status
         - AFE_INTC_SUPPLYFILTRDY : Bit2 Low Pass Filter Result Status
         - AFE_INTC_TEMPRDY       : Bit3, Temp Sensor Result Ready
         - AFE_INTC_ADCMINERR     : Bit4, ADC Minimum Value
         - AFE_INTC_ADCMAXERR     : Bit5, ADC Maximum Value
         - AFE_INTC_ADCDIFFERR    : Bit6, ADC Delta Ready
         - AFE_INTC_MEANRDY       : Bit7, Mean Result Ready
         - AFE_INTC_VARRDY       : Bit8, Variance Result Ready
         - AFE_INTC_DLYCMDDONE   : Bit9, User controlled interrupt by writing AFEGENINTSTA. Provides an Early Indication for the End of the Test _Block.
         - AFE_INTC_HWSETUPDONE  : Bit10, User controlled interrupt by writing AFEGENINTSTA. Indicates the MMR Setup for the Measurement Step Finished
         - AFE_INTC_BRKSEQ       : Bit11, User controlled interrupt by writing AFEGENINTSTA.
         - AFE_INTC_CUSTOMINS    : Bit12, User controlled interrupt by writing AFEGENINTSTA. General Purpose Custom Interrupt.
         - AFE_INTC_BOOTLDDONE   : Bit13, OTP Boot Loading Done
         - AFE_INTC_WAKEUP       : Bit14, AFE Woken up
         - AFE_INTC_ENDSEQ       : Bit15, End of Sequence Interrupt.
         - AFE_INTC_SEQTIMEOUT   : Bit16, Sequencer Timeout Command Finished.
         - AFE_INTC_SEQTIMEOUTERR : Bit17, Sequencer Timeout Command Error.
         - AFE_INTC_CMDFIFOFULL  : Bit18, Command FIFO Full Interrupt.
         - AFE_INTC_CMDFIFOEMPTY : Bit19, Command FIFO Empty
         - AFE_INTC_CMDFIFOTHRESH: Bit20, Command FIFO Threshold Interrupt.
         - AFE_INTC_CMDFIFOOF    : Bit21, Command FIFO Overflow Interrupt.
         - AFE_INTC_CMDFIFOUF    : Bit22, Command FIFO Underflow Interrupt.
         - AFE_INTC_DATAFIFOFULL : Bit23, Data FIFO Full Interrupt.
         - AFE_INTC_DATAFIFOEMPTY: Bit24, Data FIFO Empty
         - AFE_INTC_DATAFIFOTHRESH: Bit25, Data FIFO Threshold Interrupt.
         - AFE_INTC_DATAFIFOOF   : Bit26, Data FIFO Overflow Interrupt.
         - AFE_INTC_DATAFIFOUF   : Bit27, Data FIFO Underflow Interrupt.
         - AFE_INTC_WDTIRQ       : Bit28, WDT Timeout Interrupt.
         - AFE_INTC_CRC_OUTLIER  : Bit29, CRC interrupt for M355, Outliers Int for AD5940
         - AFE_INTC_GPT0INT_SLPWUT: Bit30, General Purpose Timer0 IRQ for M355. Sleep or Wakeup Timer timeout for AD5940
         - AFE_INTC_GPT1INT_TRYBRK: Bit31, General Purpose Timer1 IRQ for M355. Tried to Break IRQ for AD5940
         - AFE_INTC_ALLINT        : All interrupts
          -
   @param State : {true, false}
          - true: Enable these interrupt source(s)
          - false: Disable interrupt source(s)

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_INTCCfg(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		   uint32_t AFEIntSrc, bool State)
{
	int ret;
	uint32_t tempreg;
	uint32_t regaddr = REG_INTC_INTCSEL0;

	if(AfeIntcSel == AFEINTC_1)
		regaddr = REG_INTC_INTCSEL1;

	ret = ad5940_ReadReg(dev, regaddr, &tempreg);
	if (ret < 0)
		return ret;
	if(State == true)
		tempreg |= AFEIntSrc;    /* Enable this interrupt */
	else
		tempreg &= ~(AFEIntSrc); /* Disable this interrupt  */

	return ad5940_WriteReg(dev, regaddr, tempreg);
}

/**
   @brief uint32_t AD5940_INTCGetCfg(uint32_t AfeIntcSel);
          ====== Check if current interrupt configuration.
   @param AfeIntcSel : {AFEINTC_0, AFEINTC_1}
          - AFEINTC_0: Configure Interrupt Controller 0
          - AFEINTC_1: Configure Interrupt Controller 1

 */
int ad5940_INTCGetCfg(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		      uint32_t *cfg)
{
	int ret;
	uint32_t tempreg;
	if(AfeIntcSel == AFEINTC_0)
		ret = ad5940_ReadReg(dev, REG_INTC_INTCSEL0, &tempreg);
	else
		ret = ad5940_ReadReg(dev, REG_INTC_INTCSEL1, &tempreg);
	return ret;
}

/**
   @brief uint32_t AD5940_INTCClrFlag(uint32_t AfeIntSrcSel)
         ========== Clear selected interrupt(s) flag(INTC0Flag and INTC1Flag are both cleared).
   @param AfeIntSrcSel:{AFE_INTC_ADCRDY|AFE_INTC_DFTRDY|AFE_INTC_SUPPLYFILTRDY|AFE_INTC_TEMPRDY|AFE_INTC_ADCMINERR|AFE_INTC_ADCMAXERR|AFE_INTC_ADCDIFFERR
                   |AFE_INTC_MEANRDY|AFE_INTC_VARRDY|AFE_INTC_DLYCMDDONE|AFE_INTC_HWSETUPDONE|AFE_INTC_BRKSEQ|AFE_INTC_CUSTOMINS|AFE_INTC_BOOTLDDONE
                   |AFE_INTC_WAKEUP|AFE_INTC_ENDSEQ|AFE_INTC_SEQTIMEOUT|AFE_INTC_SEQTIMEOUTERR|AFE_INTC_CMDFIFOFULL|AFE_INTC_CMDFIFOEMPTY|AFE_INTC_CMDFIFOTHRESH
                   |AFE_INTC_CMDFIFOOF|AFE_INTC_CMDFIFOUF|AFE_INTC_DATAFIFOFULL|AFE_INTC_DATAFIFOEMPTY|AFE_INTC_DATAFIFOTHRESH|AFE_INTC_DATAFIFOOF
                   |AFE_INTC_DATAFIFOUF|AFE_INTC_WDTIRQ|AFE_INTC_CRC_OUTLIER|AFE_INTC_GPT0INT_SLPWUT|AFE_INTC_GPT1INT_TRYBRK|AFE_INTC_ALLINT}

  @return return 0 in case of success, negative error code otherwise.

 **/
int ad5940_INTCClrFlag(struct ad5940_dev *dev, uint32_t AfeIntSrcSel)
{
	return ad5940_WriteReg(dev, REG_INTC_INTCCLR, AfeIntSrcSel);
}

/**
   @brief bool AD5940_INTCTestFlag(uint32_t AfeIntcSel, uint32_t AfeIntSrcSel)
         ========== Test if selected interrupt source(s) is(are) true.
   @param AfeIntcSel : {AFEINTC_0, AFEINTC_1}
          - AFEINTC_0: Read Interrupt Controller 0 flag
          - AFEINTC_1: Read Interrupt Controller 1 flag
   @param AfeIntSrcSel:{AFE_INTC_ADCRDY|AFE_INTC_DFTRDY|AFE_INTC_SUPPLYFILTRDY|AFE_INTC_TEMPRDY|AFE_INTC_ADCMINERR|AFE_INTC_ADCMAXERR|AFE_INTC_ADCDIFFERR
                   |AFE_INTC_MEANRDY|AFE_INTC_VARRDY|AFE_INTC_DLYCMDDONE|AFE_INTC_HWSETUPDONE|AFE_INTC_BRKSEQ|AFE_INTC_CUSTOMINS|AFE_INTC_BOOTLDDONE
                   |AFE_INTC_WAKEUP|AFE_INTC_ENDSEQ|AFE_INTC_SEQTIMEOUT|AFE_INTC_SEQTIMEOUTERR|AFE_INTC_CMDFIFOFULL|AFE_INTC_CMDFIFOEMPTY|AFE_INTC_CMDFIFOTHRESH
                   |AFE_INTC_CMDFIFOOF|AFE_INTC_CMDFIFOUF|AFE_INTC_DATAFIFOFULL|AFE_INTC_DATAFIFOEMPTY|AFE_INTC_DATAFIFOTHRESH|AFE_INTC_DATAFIFOOF
                   |AFE_INTC_DATAFIFOUF|AFE_INTC_WDTIRQ|AFE_INTC_CRC_OUTLIER|AFE_INTC_GPT0INT_SLPWUT|AFE_INTC_GPT1INT_TRYBRK|AFE_INTC_ALLINT}

  @return If selected interrupt source(s) are all cleared, return false. Otherwise return true.

 **/
bool ad5940_INTCTestFlag(struct ad5940_dev *dev, uint32_t AfeIntcSel,
			 uint32_t AfeIntSrcSel)
{
	uint32_t tempreg;
	uint32_t regaddr = (AfeIntcSel == AFEINTC_0)? REG_INTC_INTCFLAG0:
			   REG_INTC_INTCFLAG1;

	ad5940_ReadReg(dev, regaddr, &tempreg);
	if(tempreg & AfeIntSrcSel)
		return true;
	else
		return false;
}
/**
   @brief bool AD5940_INTCTestFlag(uint32_t AfeIntcSel, uint32_t AfeIntSrcSel)
         ========== return register value of REG_INTC_INTCFLAGx
   @param AfeIntcSel : {AFEINTC_0, AFEINTC_1}
          - AFEINTC_0: Read Interrupt Controller 0 flag
          - AFEINTC_1: Read Interrupt Controller 1 flag
  @return register value of REG_INTC_INTCFLAGx.

 **/
int ad5940_INTCGetFlag(struct ad5940_dev *dev, uint32_t AfeIntcSel,
		       uint32_t *flag)
{
	uint32_t regaddr = (AfeIntcSel == AFEINTC_0)? REG_INTC_INTCFLAG0:
			   REG_INTC_INTCFLAG1;
	return ad5940_ReadReg(dev, regaddr, flag);
}

/**
 * @} Interrupt_Controller_Functions
 */

/**
 * @defgroup GPIO_Block_Functions
 * @{
 */

/**
   @brief void AD5940_AGPIOCfg(AGPIOCfg_Type *pAgpioCfg)
          ====== Initialize AFE GPIO
   @param pAgpioCfg : {0 - 0xffffffff}
          - Pointer to configuration structure

   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_AGPIOCfg(struct ad5940_dev *dev, AGPIOCfg_Type *pAgpioCfg)
{
	int ret;

	ret = ad5940_WriteReg(dev, REG_AGPIO_GP0CON, pAgpioCfg->FuncSet);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AGPIO_GP0OEN, pAgpioCfg->OutputEnSet);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AGPIO_GP0IEN, pAgpioCfg->InputEnSet);
	if (ret < 0)
		return ret;

	ret = ad5940_WriteReg(dev, REG_AGPIO_GP0PE, pAgpioCfg->PullEnSet);
	if (ret < 0)
		return ret;

	return ad5940_WriteReg(dev, REG_AGPIO_GP0OUT, pAgpioCfg->OutVal);
}

/**
   @brief uint32_t AD5940_AGPIOFuncCfg(uint32_t uiCfgSet)
      Configure the function of GP0 to GP7.
   @param uiCfgSet :{GP0_INT,GP0_TRIG,GP0_SYNC,GP0_GPIO|
                  GP1_GPIO,GP1_TRIG,GP1_SYNC,GP1_SLEEP|
                  GP2_PORB,GP2_TRIG,GP2_SYNC,GP2_EXTCLK|
                  GP3_GPIO,GP3_TRIG,GP3_SYNC,GP3_INT0|
                  GP4_GPIO,GP4_TRIG,GP4_SYNC,GP4_INT1|
                  GP5_GPIO,GP5_TRIG,GP5_SYNC,GP5_EXTCLK|
                  GP6_GPIO,GP6_TRIG,GP6_SYNC,GP6_INT0|
                  GP7_GPIO,GP7_TRIG,GP7_SYNC,GP7_INT}
   @return return 1
 **/
int ad5940_AGPIOFuncCfg(struct ad5940_dev *dev, uint32_t uiCfgSet)
{
	return ad5940_WriteReg(dev, REG_AGPIO_GP0CON, uiCfgSet);
}

/**
   @brief uint32_t AfeGpioOen(uint32_t uiPinSet)
      GPIO selected in uiPinSet will enable the output functions, other pins output will be disabled.
   @param uiPinSet :{AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2|AGPIO_Pin3|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6|AGPIO_Pin7}
   @return return 1
 **/
int ad5940_AGPIOOen(struct ad5940_dev *dev, uint32_t uiPinSet)
{
	return ad5940_WriteReg(dev, REG_AGPIO_GP0OEN, uiPinSet);
}

/**
   @brief uint32_t AD5940_AGPIOIen(uint32_t uiPinSet)
      GPIO selected in uiPinSet will enable the input functions, other pins input will be disabled.
   @param uiPinSet :{AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2|AGPIO_Pin3|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6|AGPIO_Pin7}
   @return return 1
 **/
int ad5940_AGPIOIen(struct ad5940_dev *dev, uint32_t uiPinSet)
{
	return ad5940_WriteReg(dev, REG_AGPIO_GP0IEN, uiPinSet);
}

/**
   @brief uint32_t AfeGpioPen(uint32_t uiPinSet)
      GPIO selected in uiPinSet will enable the pull up(down?) functions, other pins pull function will be disabled.
   @param uiPinSet :{AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin2|AGPIO_Pin3|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6|AGPIO_Pin7}
   @return return 1
 **/
int ad5940_AGPIOPen(struct ad5940_dev *dev, uint32_t uiPinSet)
{
	return ad5940_WriteReg(dev, REG_AGPIO_GP0PE, uiPinSet);
}

/**
 * @} GPIO_Block_Functions
 */

/**
 * @defgroup LPMode_Block_Functions
 * @{
 */

/** Enable LP mode or disable it. */
int ad5940_LPModeEnS(struct ad5940_dev *dev, bool LPModeEn)
{
	int ret;
	if(LPModeEn == true)
		ret = ad5940_WriteReg(dev, REG_AFE_LPMODEKEY,
				      KEY_LPMODEKEY);  /* Enter LP mode by right key. */
	else
		ret = ad5940_WriteReg(dev, REG_AFE_LPMODEKEY,
				      0); /* Write wrong key to exit LP mode */
	return ret;
}

/**
 * @brief Select system clock source for LP mode.
 * @note Only in LP Mode, this operation takes effect. Firslty we need enter LP mode.
 * @param LPModeClk: Select from @ref LPMODECLK_Const
 *       - LPMODECLK_LFOSC: Select LFOSC 32kHz for system clock
 *       - LPMODECLK_HFOSC: Select HFOSC 16MHz/32MHz for system clock
 * @return 0 in case of success, negative error code otherwise.
 */
int ad5940_LPModeClkS(struct ad5940_dev *dev, uint32_t LPModeClk)
{
	return ad5940_WriteReg(dev, REG_AFE_LPMODECLKSEL, LPModeClk);
}

/**
 * @} LPMode_Block_Functions
 */

/**
   @brief void AD5940_SleepKeyCtrlS(uint32_t SlpKey)
          ====== Enter sleep mode key to unlock it or enter incorrect key to lock it.
          Once key is unlocked, it will always be effect until manually lock it
   @param SlpKey : {SLPKEY_UNLOCK, SLPKEY_LOCK}
          - Pointer to structure
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_SleepKeyCtrlS(struct ad5940_dev *dev, uint32_t SlpKey)
{
	return ad5940_WriteReg(dev, REG_AFE_SEQSLPLOCK, SlpKey);
}

/**
   @brief void AD5940_EnterSleepS(void)
          ====== Put AFE to hibernate. There is a switch to enable it. We do all this in function AFEInitialize().
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_EnterSleepS(struct ad5940_dev *dev)
{
	return ad5940_WriteReg(dev, REG_AFE_SEQTRGSLP, 1);
}

/**
   @brief void AD5940_ShutDownS(void)
          ====== Turn off LP-Loop and put AFE to hibernate mode;
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_ShutDownS(struct ad5940_dev *dev)
{
	int ret;
	/* Turn off LPloop related blocks which are not controlled automatically by hibernate operation */
	AFERefCfg_Type aferef_cfg;
	LPLoopCfg_Type lp_loop;
	/* Turn off LP-loop manually because it's not affected by sleep/hibernate mode */
	AD5940_StructInit(&aferef_cfg, sizeof(aferef_cfg));
	AD5940_StructInit(&lp_loop, sizeof(lp_loop));
	ret = ad5940_REFCfgS(dev, &aferef_cfg);
	if (ret < 0)
		return ret;

	ad5940_LPLoopCfgS(dev, &lp_loop);
	if (ret < 0)
		return ret;

	ad5940_SleepKeyCtrlS(dev, SLPKEY_UNLOCK);  /* Unlock the key */
	if (ret < 0)
		return ret;

	return ad5940_EnterSleepS(dev);  /* Enter Hibernate */
}

/**
 * @brief Try to wakeup AD5940 by read register.
 * @details Any SPI operation can wakeup AD5940. AD5940_Initialize must be called to enable this function.
 * @param TryCount Specify how many times we will read register. Zero or negative number means always waiting here.
 * @return How many times register is read. If returned value is bigger than TryCount, it means wakeup failed.
 */
int ad5940_WakeUp(struct ad5940_dev *dev, int32_t TryCount)
{
	int ret;
	uint32_t tempreg;
	int32_t count = 0;
	while(1) {
		count++;
		ret = ad5940_ReadReg(dev, REG_AFECON_ADIID, &tempreg);
		if (ret < 0)
			return ret;
		if (tempreg == AD5940_ADIID)
			break;    /* Succeed */
		if(TryCount <= 0)
			continue; /* Always try to wakup AFE */
		if(count > TryCount)
			break;    /* Failed */
	}
	return count;
}

/**
   @brief Reset AD5940 with RESET pin.
   @return return 0 in case of success, negative error code otherwise.
 */
int ad5940_HWReset(struct ad5940_dev *dev)
{
	int ret;
	uint32_t tempreg;

	no_os_gpio_set_value(dev->reset_gpio, NO_OS_GPIO_LOW);
	no_os_udelay(10); /* Delay some time */
	no_os_gpio_set_value(dev->reset_gpio, NO_OS_GPIO_HIGH);
	no_os_mdelay(1); /* AD5940 need some time to exit reset status */

	ret = ad5940_ReadReg(dev, REG_AFECON_ADIID, &tempreg);
	if (ret < 0)
		return ret;
	if (tempreg != AD5940_ADIID)
		return -EFAULT;

	return 0;
}

/**
 * @} MISC_Block_Functions
 * @} MISC_Block
 */

/**
 * @defgroup Calibration_Block
 * @brief The non-factory calibration routines.
 * @{
 *    @defgroup Calibration_Functions
 *    @{
 *
 *
 */

int ad5940_HSRtiaCal(struct ad5940_dev *dev, HSRTIACal_Type *pCalCfg,
		     void *pResult)
{
	int ret;
	AFERefCfg_Type aferef_cfg;
	HSLoopCfg_Type hs_loop;
	DSPCfg_Type dsp_cfg;
	bool bADCClk32MHzMode = false;
	uint32_t ExcitBuffGain = EXCITBUFGAIN_2;
	uint32_t HsDacGain = HSDACGAIN_1;

	float ExcitVolt; /* Excitation voltage, unit is mV */
	uint32_t RtiaVal;
	static uint32_t const HpRtiaTable[]= {200,1000,5000,10000,20000,40000,80000,160000,0};
	uint32_t WgAmpWord;

	iImpCar_Type DftRcal, DftRtia;

	if(pCalCfg == NULL) return -EINVAL;
	if(pCalCfg->fRcal == 0)
		return -EINVAL;
	if(pCalCfg->HsTiaCfg.HstiaRtiaSel > HSTIARTIA_160K)
		return -EINVAL;
	if(pCalCfg->HsTiaCfg.HstiaRtiaSel == HSTIARTIA_OPEN)
		return -EINVAL; /* Do not support calibrating DE0-RTIA */
	if(pResult == NULL)
		return -EINVAL;

	if(pCalCfg->AdcClkFreq > (32000000*0.8))
		bADCClk32MHzMode = true;

	/* Calculate the excitation voltage we should use based on RCAL/Rtia */
	RtiaVal = HpRtiaTable[pCalCfg->HsTiaCfg.HstiaRtiaSel];
	/*
	DAC output voltage calculation
	Note: RCAL value should be similar to RTIA so the accuracy is best.
	HSTIA output voltage should be limited to 0.2V to AVDD-0.2V, with 1.1V bias. We use 80% of this range for safe.
	Because the bias voltage is fixed to 1.1V, so for AC signal maximum amplitude is 1.1V-0.2V = 0.9Vp. That's 1.8Vpp.
	Formula is:    ExcitVolt(in mVpp) = (1800mVpp*80% / RTIA) * RCAL
	ADC input range is +-1.5V which is enough for calibration.

	 */
	ExcitVolt = 1800*0.8*pCalCfg->fRcal/RtiaVal;

	if(ExcitVolt <=
	    800*0.05) { /* Voltage is so small that we can enable the attenuator of DAC(1/5) and Excitation buffer(1/4). 800mVpp is the DAC output voltage */
		ExcitBuffGain = EXCITBUFGAIN_0P25;
		HsDacGain = HSDACGAIN_0P2;
		/* Excitation buffer voltage full range is 800mVpp*0.05 = 40mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/40*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	} else if(ExcitVolt <= 800*0.25) { /* Enable Excitation buffer attenuator */
		ExcitBuffGain = EXCITBUFGAIN_0P25;
		HsDacGain = HSDACGAIN_1;
		/* Excitation buffer voltage full range is 800mVpp*0.25 = 200mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/200*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	} else if(ExcitVolt <= 800*0.4) { /* Enable DAC attenuator */
		ExcitBuffGain = EXCITBUFGAIN_2;
		HsDacGain = HSDACGAIN_0P2;
		/* Excitation buffer voltage full range is 800mVpp*0.4 = 320mV */
		WgAmpWord = ((uint32_t)(ExcitVolt/320*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	} else { /* No attenuator is needed. This is the best condition which means RTIA is close to RCAL */
		ExcitBuffGain = EXCITBUFGAIN_2;
		HsDacGain = HSDACGAIN_1;
		/* Excitation buffer voltage full range is 800mVpp*2=1600mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/1600*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	}

	if(WgAmpWord > 0x7ff)
		WgAmpWord = 0x7ff;

	ret = ad5940_AFECtrlS(dev, AFECTRL_ALL, false);  /* Init all to disable state */
	if (ret < 0)
		return ret;

	/* Configure reference system */
	aferef_cfg.HpBandgapEn = true;
	aferef_cfg.Hp1V1BuffEn = true;
	aferef_cfg.Hp1V8BuffEn = true;
	aferef_cfg.Disc1V1Cap = false;
	aferef_cfg.Disc1V8Cap = false;
	aferef_cfg.Hp1V8ThemBuff = false;
	aferef_cfg.Hp1V8Ilimit = false;
	aferef_cfg.Lp1V1BuffEn = false;
	aferef_cfg.Lp1V8BuffEn = false;
	aferef_cfg.LpBandgapEn = false;
	aferef_cfg.LpRefBufEn = false;
	aferef_cfg.LpRefBoostEn = false;
	ret = ad5940_REFCfgS(dev, &aferef_cfg);
	if (ret < 0)
		return ret;

	/* Configure HP Loop */
	hs_loop.HsDacCfg.ExcitBufGain = ExcitBuffGain;
	hs_loop.HsDacCfg.HsDacGain = HsDacGain;
	hs_loop.HsDacCfg.HsDacUpdateRate = 7; /* Set it to highest update rate */
	memcpy(&hs_loop.HsTiaCfg, &pCalCfg->HsTiaCfg, sizeof(pCalCfg->HsTiaCfg));
	hs_loop.SWMatCfg.Dswitch = SWD_RCAL0;
	hs_loop.SWMatCfg.Pswitch = SWP_RCAL0;
	hs_loop.SWMatCfg.Nswitch = SWN_RCAL1;
	hs_loop.SWMatCfg.Tswitch = SWT_RCAL1|SWT_TRTIA;
	hs_loop.WgCfg.WgType = WGTYPE_SIN;
	hs_loop.WgCfg.GainCalEn =
		false;      /* @todo. If we have factory calibration data, enable it */
	hs_loop.WgCfg.OffsetCalEn = false;
	hs_loop.WgCfg.SinCfg.SinFreqWord = ad5940_WGFreqWordCal(pCalCfg->fFreq,
					   pCalCfg->SysClkFreq);
	hs_loop.WgCfg.SinCfg.SinAmplitudeWord = WgAmpWord;
	hs_loop.WgCfg.SinCfg.SinOffsetWord = 0;
	hs_loop.WgCfg.SinCfg.SinPhaseWord = 0;
	ret = ad5940_HSLoopCfgS(dev, &hs_loop);
	if (ret < 0)
		return ret;

	/* Configure DSP */
	dsp_cfg.ADCBaseCfg.ADCMuxN = ADCMUXN_N_NODE;
	dsp_cfg.ADCBaseCfg.ADCMuxP = ADCMUXP_P_NODE;
	dsp_cfg.ADCBaseCfg.ADCPga = ADCPGA_1; /* @todo Change the gain? */
	AD5940_StructInit(&dsp_cfg.ADCDigCompCfg, sizeof(dsp_cfg.ADCDigCompCfg));
	dsp_cfg.ADCFilterCfg.ADCAvgNum =
		ADCAVGNUM_16;  /* Don't care because it's disabled */
	dsp_cfg.ADCFilterCfg.ADCRate = bADCClk32MHzMode?ADCRATE_1P6MHZ:ADCRATE_800KHZ;
	dsp_cfg.ADCFilterCfg.ADCSinc2Osr = pCalCfg->ADCSinc2Osr;
	dsp_cfg.ADCFilterCfg.ADCSinc3Osr = pCalCfg->ADCSinc3Osr;
	dsp_cfg.ADCFilterCfg.BpNotch = true;
	dsp_cfg.ADCFilterCfg.BpSinc3 = false;
	dsp_cfg.ADCFilterCfg.DFTClkEnable = true;
	dsp_cfg.ADCFilterCfg.Sinc2NotchClkEnable = true;
	dsp_cfg.ADCFilterCfg.Sinc2NotchEnable = true;
	dsp_cfg.ADCFilterCfg.Sinc3ClkEnable = true;
	dsp_cfg.ADCFilterCfg.WGClkEnable = true;

	memcpy(&dsp_cfg.DftCfg, &pCalCfg->DftCfg, sizeof(pCalCfg->DftCfg));
	memset(&dsp_cfg.StatCfg, 0, sizeof(dsp_cfg.StatCfg));
	ret = ad5940_DSPCfgS(dev, &dsp_cfg);
	if (ret < 0)
		return ret;

	/* Enable all of them. They are automatically turned off during hibernate mode to save power */
	ret = ad5940_AFECtrlS(dev, AFECTRL_HSTIAPWR|AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|\
			      /*AFECTRL_WG|*/AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|\
			      AFECTRL_SINC2NOTCH, true);
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
			      true);  /* Enable Waveform generator, ADC power */
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT,
			      true);  /* Start ADC convert and DFT */
	if (ret < 0)
		return ret;

	/* Wait until DFT ready */
	while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_DFTRDY) == false);

	ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG|AFECTRL_ADCPWR,
			      false);  /* Stop ADC convert and DFT */
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
	if (ret < 0)
		return ret;

	ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTREAL, (uint32_t *)&DftRcal.Real);
	if (ret < 0)
		return ret;

	ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTIMAGE, (uint32_t *)&DftRcal.Image);
	if (ret < 0)
		return ret;

	ret = ad5940_ADCMuxCfgS(dev, ADCMUXP_HSTIA_P, ADCMUXN_HSTIA_N);
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
			      true);  /* Enable Waveform generator, ADC power */
	if (ret < 0)
		return ret;

	ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT,
			      true);  /* Start ADC convert and DFT */
	if (ret < 0)
		return ret;

	/* Wait until DFT ready */
	while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_DFTRDY) == false);
	ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG|AFECTRL_ADCPWR,
			      false);  /* Stop ADC convert and DFT */
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
	if (ret < 0)
		return ret;

	ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTREAL, (uint32_t *)&DftRtia.Real);
	if (ret < 0)
		return ret;

	ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTIMAGE, (uint32_t *)&DftRtia.Image);
	if (ret < 0)
		return ret;

	if(DftRcal.Real&(1<<17))
		DftRcal.Real |= 0xfffc0000;
	if(DftRcal.Image&(1<<17))
		DftRcal.Image |= 0xfffc0000;
	if(DftRtia.Real&(1<<17))
		DftRtia.Real |= 0xfffc0000;
	if(DftRtia.Image&(1<<17))
		DftRtia.Image |= 0xfffc0000;
	/*
	ADC MUX is set to HSTIA_P and HSTIA_N.
	While the current flow through RCAL and then into RTIA, the current direction should be from HSTIA_N to HSTIA_P if we
	measure the voltage across RCAL by MUXSELP_P_NODE and MUXSELN_N_NODE.
	So here, we add a negative sign to results
	 */
	DftRtia.Image = -DftRtia.Image;
	DftRtia.Real =
		-DftRtia.Real; /* Current is measured by MUX HSTIA_P-HSTIA_N. It should be  */
	/*
	The impedance engine inside of AD594x give us Real part and Imaginary part of DFT. Due to technology used, the Imaginary
	part in register is the opposite number. So we add a negative sign on the Imaginary part of results.
	 */
	DftRtia.Image = -DftRtia.Image;
	DftRcal.Image = -DftRcal.Image;

	if(pCalCfg->bPolarResult == false) {
		float temp = (float)DftRcal.Real*DftRcal.Real + (float)
			     DftRcal.Image*DftRcal.Image;

		/* RTIA = (DftRtia.Real, DftRtia.Image)/(DftRcal.Real, DftRcal.Image)*fRcal */
		((fImpCar_Type*)pResult)->Real = ((float)DftRtia.Real*DftRcal.Real+
						  (float)DftRtia.Image*DftRcal.Image)/temp*pCalCfg->fRcal; /* Real Part */
		((fImpCar_Type*)pResult)->Image = ((float)DftRtia.Image*DftRcal.Real-
						   (float)DftRtia.Real*DftRcal.Image)/temp*pCalCfg->fRcal; /* Imaginary Part */
		//printf("RTIA mag:%f,",sqrt(pResult[0]*pResult[0]+pResult[1]*pResult[1]));
		//printf("phase:%f\n",atan2(pResult[1],pResult[0])/3.1415926*180);
	} else {
		float RcalMag,RtiaMag,RtiaPhase;
		RcalMag = sqrt((float)DftRcal.Real*DftRcal.Real+(float)
			       DftRcal.Image*DftRcal.Image);
		RtiaMag = sqrt((float)DftRtia.Real*DftRtia.Real+(float)
			       DftRtia.Image*DftRtia.Image);
		RtiaMag = (RtiaMag/RcalMag)*pCalCfg->fRcal;
		RtiaPhase = atan2(DftRtia.Image,DftRtia.Real) - atan2(DftRcal.Image,
				DftRcal.Real);

		((fImpPol_Type*)pResult)->Magnitude = RtiaMag;
		((fImpPol_Type*)pResult)->Phase = RtiaPhase;
		//printf("RTIA mag:%f,",RtiaMag);
		//printf("phase:%f\n",RtiaPhase*180/MATH_PI);
	}
	return 0;
}


int ad5940_LPRtiaCal(struct ad5940_dev *dev, LPRTIACal_Type *pCalCfg,
		     void *pResult)
{
	int ret;
	AFERefCfg_Type aferef_cfg;
	HSLoopCfg_Type hs_loop;
	LPLoopCfg_Type lp_loop;
	DSPCfg_Type dsp_cfg;

	bool bADCClk32MHzMode = false;
	bool bDCMode =
		false;                /* Indicate if frequency is 0, which means we calibrate at DC. */
	uint32_t ExcitBuffGain = EXCITBUFGAIN_2;
	uint32_t HsDacGain = HSDACGAIN_1;

	float ExcitVolt; /* Excitation voltage, unit is mV */
	uint32_t RtiaVal;
	/* RTIA value table when RLOAD set to 100Ohm */
	static uint32_t const LpRtiaTable[]= {0,200,1000,2000,3000,4000,6000,8000,1000,1200,16000,20000,24000,30000,32000,40000,48000,64000,85000,96000,100000,120000,128000,160000,196000,256000,512000};
	uint32_t WgAmpWord;

	iImpCar_Type DftRcal, DftRtia;

	if(pCalCfg == NULL)  return -EINVAL;  /* Parameters illegal */

	if(pCalCfg->fRcal == 0)
		return -EINVAL;
	if(pCalCfg->LpAmpCfg.LpTiaRtia > LPTIARTIA_512K)
		return -EINVAL;
	if(pCalCfg->LpAmpCfg.LpTiaRtia == LPTIARTIA_OPEN)
		return -EINVAL; /* Not supported now. By setting RTIA to open and set corresponding switches can calibrate external RTIA */
	if(pResult == NULL)
		return -EINVAL;

	if(pCalCfg->AdcClkFreq > (32000000*0.8))
		bADCClk32MHzMode = true;   /* Clock frequency is high. */
	if(pCalCfg->fFreq ==
	    0.0f)    /* Frequency is zero means we calibrate RTIA at DC. */
		bDCMode = true;

	/* Calculate the excitation voltage we should use based on RCAL/Rtia */
	RtiaVal = LpRtiaTable[pCalCfg->LpAmpCfg.LpTiaRtia];
	/*
	DAC output voltage calculation
	Note: RCAL value should be similar to RTIA so the accuracy is best.
	LPTIA output voltage should be limited to 0.2V to AVDD-0.2V, with 1.35V bias. We use 80% of this range for safe.
	That's 2.3Vpp*80%@2.7V AVDD
	Formula is:    ExcitVolt(in mVpp) = (2300mVpp*80% / RTIA) * RCAL
	ADC input range is +-1.5V which is enough for calibration.

	 */
	ExcitVolt = 2300*0.8*pCalCfg->fRcal/RtiaVal; /*@todo check equation. */

	if(ExcitVolt <=
	    800*0.05) { /* Voltage is so small that we can enable the attenuator of DAC(1/5) and Excitation buffer(1/4). 800mVpp is the DAC output voltage */
		ExcitBuffGain = EXCITBUFGAIN_0P25;
		HsDacGain = HSDACGAIN_0P2;
		/* Excitation buffer voltage full range is 800mVpp*0.05 = 40mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/40*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error). ((int)(x*2)+1)/2 */
	} else if(ExcitVolt <= 800*0.25) { /* Enable Excitation buffer attenuator */
		ExcitBuffGain = EXCITBUFGAIN_0P25;
		HsDacGain = HSDACGAIN_1;
		/* Excitation buffer voltage full range is 800mVpp*0.25 = 200mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/200*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	} else if(ExcitVolt <= 800*0.4) { /* Enable DAC attenuator */
		ExcitBuffGain = EXCITBUFGAIN_2;
		HsDacGain = HSDACGAIN_0P2;
		/* Excitation buffer voltage full range is 800mVpp*0.4 = 320mV */
		WgAmpWord = ((uint32_t)(ExcitVolt/320*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	} else { /* No attenuator is needed. This is the best condition which means RTIA is close to RCAL */
		ExcitBuffGain = EXCITBUFGAIN_2;
		HsDacGain = HSDACGAIN_1;
		/* Excitation buffer voltage full range is 800mVpp*2=1600mVpp */
		WgAmpWord = ((uint32_t)(ExcitVolt/1600*2047*2)+1)
			    >>1; /* Assign value with rounding (0.5 LSB error) */
	}

	if(WgAmpWord > 0x7ff)
		WgAmpWord = 0x7ff;

	ret = ad5940_AFECtrlS(dev, AFECTRL_ALL, false);  /* Init all to disable state */
	if (ret < 0)
		return ret;

	/* Configure reference system */
	AD5940_StructInit(&aferef_cfg, sizeof(aferef_cfg));
	aferef_cfg.HpBandgapEn = true;
	aferef_cfg.Hp1V1BuffEn = true;
	aferef_cfg.Hp1V8BuffEn = true;
	aferef_cfg.LpBandgapEn = true;
	aferef_cfg.LpRefBufEn = true;
	ret = ad5940_REFCfgS(dev, &aferef_cfg);
	if (ret < 0)
		return ret;

	/* Configure LP Loop */
	AD5940_StructInit(&lp_loop, sizeof(lp_loop));
	/* Configure LP Amplifies(LPPA and LPTIA). We won't use LP-PA */
	memcpy(&lp_loop.LpAmpCfg, &pCalCfg->LpAmpCfg, sizeof(pCalCfg->LpAmpCfg));
	lp_loop.LpDacCfg.DacData12Bit = 0x800;  /* Set it to middle scale. */
	lp_loop.LpDacCfg.DacData6Bit = 31;      /* Set it to middle scale. */
	lp_loop.LpDacCfg.DataRst = false;      /* Do not reset data register */
	lp_loop.LpDacCfg.LpDacSW =
		LPDACSW_VBIAS2LPPA|LPDACSW_VBIAS2PIN|LPDACSW_VZERO2LPTIA|LPDACSW_VZERO2PIN;
	lp_loop.LpDacCfg.LpDacRef = LPDACREF_2P5;
	lp_loop.LpDacCfg.LpDacSrc =
		LPDACSRC_MMR;     /* Use MMR data, we use LPDAC to generate bias voltage for LPTIA - the Vzero */
	lp_loop.LpDacCfg.LpDacVbiasMux = LPDACVBIAS_12BIT;
	lp_loop.LpDacCfg.LpDacVzeroMux =
		LPDACVZERO_6BIT;  /* Use 12bit DAC output data */
	lp_loop.LpDacCfg.PowerEn = true;       /* Power up LPDAC */
	ret = ad5940_LPLoopCfgS(dev, &lp_loop);
	if (ret < 0)
		return ret;

	/* Configure HP Loop */
	AD5940_StructInit(&hs_loop, sizeof(hs_loop));
	/* Take care of HSTIA, we need to disconnect internal RTIA because it connects to Tswitch directly. */
	hs_loop.HsTiaCfg.HstiaDeRload = HSTIADERLOAD_OPEN;
	hs_loop.HsTiaCfg.HstiaDeRtia = HSTIADERTIA_OPEN;
	hs_loop.HsTiaCfg.HstiaRtiaSel = HSTIARTIA_OPEN;
	/* Configure HSDAC */
	hs_loop.HsDacCfg.ExcitBufGain = ExcitBuffGain;
	hs_loop.HsDacCfg.HsDacGain = HsDacGain;
	hs_loop.HsDacCfg.HsDacUpdateRate =
		27; /* @todo ?Set it to highest update rate */
	hs_loop.SWMatCfg.Dswitch = SWD_RCAL0;
	hs_loop.SWMatCfg.Pswitch = SWP_RCAL0;
	hs_loop.SWMatCfg.Nswitch = SWN_RCAL1;
	hs_loop.SWMatCfg.Tswitch =
		SWT_RCAL1|SWT_SE0LOAD; /* Let current flow to SE0, then to RTIA of LPTIA */
	/* Configure HSDAC */
	if(bDCMode) {
		///@todo check accuracy of DC calibration. @todo need calibrate ADC/LPTIA offset firstly.
		hs_loop.WgCfg.WgType = WGTYPE_MMR;
		hs_loop.WgCfg.WgCode =
			WgAmpWord;   /* Amplitude word is exactly the maximum DC voltage we could use */
		hs_loop.WgCfg.GainCalEn =
			false;      /* @todo. If we have factory calibration data, enable it */
		hs_loop.WgCfg.OffsetCalEn = false;
		ret = ad5940_HSLoopCfgS(dev, &hs_loop);
		if (ret < 0)
			return ret;

		/* Configure DSP */
		AD5940_StructInit(&dsp_cfg, sizeof(dsp_cfg));
		dsp_cfg.ADCBaseCfg.ADCMuxN = ADCMUXN_N_NODE;
		dsp_cfg.ADCBaseCfg.ADCMuxP = ADCMUXP_P_NODE;
		dsp_cfg.ADCBaseCfg.ADCPga = ADCPGA_1; /* @todo Change the gain? */
		dsp_cfg.ADCFilterCfg.ADCAvgNum =
			ADCAVGNUM_16;  /* Don't care because it's disabled */
		dsp_cfg.ADCFilterCfg.ADCRate = bADCClk32MHzMode?ADCRATE_1P6MHZ:ADCRATE_800KHZ;
		dsp_cfg.ADCFilterCfg.ADCSinc2Osr = pCalCfg->ADCSinc2Osr;
		dsp_cfg.ADCFilterCfg.ADCSinc3Osr = pCalCfg->ADCSinc3Osr;
		dsp_cfg.ADCFilterCfg.BpNotch = true;
		dsp_cfg.ADCFilterCfg.BpSinc3 = false;
		dsp_cfg.ADCFilterCfg.DFTClkEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc2NotchClkEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc2NotchEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc3ClkEnable = true;
		dsp_cfg.ADCFilterCfg.WGClkEnable = true;
		ret = ad5940_DSPCfgS(dev, &dsp_cfg);
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_SINC2RDY);
		if (ret < 0)
			return ret;

		/* Enable all of them. They are automatically turned off during hibernate mode to save power */
		ret = ad5940_AFECtrlS(dev,
				      AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|AFECTRL_SINC2NOTCH,
				      true);
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
				      true);  /* Enable Waveform generator, ADC power */
		if (ret < 0)
			return ret;

		//wait for sometime. @todo add delay here
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV,
				      true);  /* Start ADC convert and DFT */
		if (ret < 0)
			return ret;

		/* Wait until DFT ready */
		///@todo Enable INTC1 firstly.
		while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_SINC2RDY) == false);

		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_WG|AFECTRL_ADCPWR,
				      false);  /* Stop ADC convert and DFT */
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_SINC2RDY);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_SINC2, (uint32_t *)&DftRcal.Real);
		if (ret < 0)
			return ret;
		DftRcal.Real -= 32768;
		DftRcal.Image = 0;

		ret = ad5940_ADCMuxCfgS(dev, ADCMUXP_LPTIA0_P, ADCMUXN_LPTIA0_N);
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
				      true);  /* Enable Waveform generator, ADC power */
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV,
				      true);  /* Start ADC convert and DFT */
		if (ret < 0)
			return ret;

		/* Wait until DFT ready */
		while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_SINC2RDY) == false);
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_WG|AFECTRL_ADCPWR,
				      false);  /* Stop ADC convert and DFT */
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_SINC2RDY);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_SINC2, (uint32_t *)&DftRcal.Real);
		if (ret < 0)
			return ret;
		DftRtia.Real -= 32768;
		DftRtia.Image = 0;
	} else {
		hs_loop.WgCfg.WgType = WGTYPE_SIN;
		hs_loop.WgCfg.SinCfg.SinFreqWord = ad5940_WGFreqWordCal(pCalCfg->fFreq,
						   pCalCfg->SysClkFreq);
		hs_loop.WgCfg.SinCfg.SinAmplitudeWord = WgAmpWord;
		hs_loop.WgCfg.SinCfg.SinOffsetWord = 0;
		hs_loop.WgCfg.SinCfg.SinPhaseWord = 0;
		hs_loop.WgCfg.GainCalEn =
			false;      /* @todo. If we have factory calibration data, enable it */
		hs_loop.WgCfg.OffsetCalEn = false;
		ret = ad5940_HSLoopCfgS(dev, &hs_loop);
		if (ret < 0)
			return ret;

		/* Configure DSP */
		AD5940_StructInit(&dsp_cfg, sizeof(dsp_cfg));
		dsp_cfg.ADCBaseCfg.ADCMuxN = ADCMUXN_N_NODE;
		dsp_cfg.ADCBaseCfg.ADCMuxP = ADCMUXP_P_NODE;
		dsp_cfg.ADCBaseCfg.ADCPga = ADCPGA_1; /* @todo Change the gain? */
		dsp_cfg.ADCFilterCfg.ADCAvgNum =
			ADCAVGNUM_16;  /* Don't care because it's disabled */
		dsp_cfg.ADCFilterCfg.ADCRate = bADCClk32MHzMode?ADCRATE_1P6MHZ:ADCRATE_800KHZ;
		dsp_cfg.ADCFilterCfg.ADCSinc2Osr = pCalCfg->ADCSinc2Osr;
		dsp_cfg.ADCFilterCfg.ADCSinc3Osr = pCalCfg->ADCSinc3Osr;
		dsp_cfg.ADCFilterCfg.BpNotch = true;
		dsp_cfg.ADCFilterCfg.BpSinc3 = false;
		dsp_cfg.ADCFilterCfg.DFTClkEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc2NotchClkEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc2NotchEnable = true;
		dsp_cfg.ADCFilterCfg.Sinc3ClkEnable = true;
		dsp_cfg.ADCFilterCfg.WGClkEnable = true;
		memcpy(&dsp_cfg.DftCfg, &pCalCfg->DftCfg, sizeof(pCalCfg->DftCfg));
		ret = ad5940_DSPCfgS(dev, &dsp_cfg);
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
		if (ret < 0)
			return ret;

		/* Enable all of them. They are automatically turned off during hibernate mode to save power */
		ret = ad5940_AFECtrlS(dev,
				      AFECTRL_INAMPPWR|AFECTRL_EXTBUFPWR|AFECTRL_DACREFPWR|AFECTRL_HSDACPWR|AFECTRL_SINC2NOTCH,
				      true);
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
				      true);  /* Enable Waveform generator, ADC power */
		if (ret < 0)
			return ret;

		//wait for sometime. @todo add delay here
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT,
				      true);  /* Start ADC convert and DFT */
		if (ret < 0)
			return ret;

		/* Wait until DFT ready */
		///@todo Enable INTC1 firstly.
		while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_DFTRDY) == false);
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG|AFECTRL_ADCPWR,
				      false);  /* Stop ADC convert and DFT */
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTREAL, (uint32_t *)&DftRcal.Real);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTIMAGE, (uint32_t *)&DftRcal.Image);
		if (ret < 0)
			return ret;

		ret = ad5940_ADCMuxCfgS(dev, ADCMUXP_LPTIA0_P, ADCMUXN_LPTIA0_N);
		if (ret < 0)
			return ret;

		ret = ad5940_AFECtrlS(dev, AFECTRL_WG|AFECTRL_ADCPWR,
				      true);  /* Enable Waveform generator, ADC power */
		if (ret < 0)
			return ret;

		//wait for sometime.
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT,
				      true);  /* Start ADC convert and DFT */
		if (ret < 0)
			return ret;

		/* Wait until DFT ready */
		while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_DFTRDY) == false);
		ret = ad5940_AFECtrlS(dev, AFECTRL_ADCCNV|AFECTRL_DFT|AFECTRL_WG|AFECTRL_ADCPWR,
				      false);  /* Stop ADC convert and DFT */
		if (ret < 0)
			return ret;

		ret = ad5940_INTCClrFlag(dev, AFEINTSRC_DFTRDY);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTREAL, (uint32_t *)&DftRtia.Real);
		if (ret < 0)
			return ret;

		ret = ad5940_ReadAfeResult(dev, AFERESULT_DFTIMAGE, (uint32_t *)&DftRtia.Image);
		if (ret < 0)
			return ret;

		if(DftRcal.Real&(1<<17))
			DftRcal.Real |= 0xfffc0000;
		if(DftRcal.Image&(1<<17))
			DftRcal.Image |= 0xfffc0000;
		if(DftRtia.Real&(1<<17))
			DftRtia.Real |= 0xfffc0000;
		if(DftRtia.Image&(1<<17))
			DftRtia.Image |= 0xfffc0000;
	}

	/*
	ADC MUX is set to LPTIA_P and LPTIA_N.
	While the current flow through RCAL and then into RTIA, the current direction should be from LPTIA_N to LPTIA_P if we
	measure the voltage across RCAL by MUXSELP_P_NODE and MUXSELN_N_NODE.
	So here, we add a negative sign to results
	 */
	DftRtia.Image = -DftRtia.Image;
	DftRtia.Real = -DftRtia.Real; /*  */
	/*
	The impedance engine inside of AD594x give us Real part and Imaginary part of DFT. Due to technology used, the Imaginary
	part in register is the opposite number. So we add a negative sign on the Imaginary part of results.
	 */
	DftRtia.Image = -DftRtia.Image;
	DftRcal.Image = -DftRcal.Image;

	if(pCalCfg->bPolarResult == false) {
		fImpCar_Type res;
		/* RTIA = (DftRtia.Real, DftRtia.Image)/(DftRcal.Real, DftRcal.Image)*fRcal */
		res = ad5940_ComplexDivInt(&DftRtia, &DftRcal);
		res.Real *= pCalCfg->fRcal;
		res.Image *= pCalCfg->fRcal;
		((fImpCar_Type*)pResult)->Real = res.Real;
		((fImpCar_Type*)pResult)->Image = res.Image;
	} else {
		float RcalMag,RtiaMag,RtiaPhase;
		RcalMag = sqrt((float)DftRcal.Real*DftRcal.Real+(float)
			       DftRcal.Image*DftRcal.Image);
		RtiaMag = sqrt((float)DftRtia.Real*DftRtia.Real+(float)
			       DftRtia.Image*DftRtia.Image);
		RtiaMag = (RtiaMag/RcalMag)*pCalCfg->fRcal;
		RtiaPhase = atan2(DftRtia.Image,DftRtia.Real) - atan2(DftRcal.Image,
				DftRcal.Real);

		((fImpPol_Type*)pResult)->Magnitude = RtiaMag;
		((fImpPol_Type*)pResult)->Phase = RtiaPhase;
		//printf("RTIA mag:%f,",RtiaMag);
		//printf("phase:%f\n",RtiaPhase*180/MATH_PI);
	}
	return 0;
}

/* Use System clock(16MHz/32MHz or external crystal) to measure LFOSC frequency */
/**@note this function takes 3 sequencer commands. Start address is specified by parameter */
int ad5940_LFOSCMeasure(struct ad5940_dev *dev, LFOSCMeasure_Type *pCfg,
			float *pFreq) /* Measure current LFOSC frequency. */
{
	////@todo after function call, some registers are modified and user should take responsibility to re-init it.

	/**
	 * @code
	 *  Sleep wakeup timer running...
	 *  -SLP----WKP----SLP----WKP----SLP----WKP
	 *  --|-----|-------------|-------------|------------Trigger sequencer when Wakeup Timer over.
	 *  --------|SEQA---------|SEQB----------------------Execute SeqA then SeqB
	 *  ---------|InitT--------|StopT--------------------SeqA start timer and SeqB trigger interrupt so MCU read back current count
	 *  ------------------------|INT---------------------
	 *  -----------------------------------------|Read---We read SEQTIMEOUT register here
	 *  ---------|-----TimerCount----------------|-------
	 *  ---------|--------------|---TimerCount2--|-------We change SeqB to reset timer so we measure how much time needed for MCU to read back SEQTIMEOUT register(TimerCount2)
	 * @endcode
	 * **/
	int ret;
	uint32_t TimerCount, TimerCount2;
	SEQCfg_Type seq_cfg, seq_cfg_backup;
	SEQInfo_Type seqinfo;
	WUPTCfg_Type wupt_cfg;
	uint32_t INTCCfg;
	uint32_t WuptPeriod;

	static const uint32_t SeqA[]= {
		SEQ_TOUT(0x3fffffff),   /* Set time-out timer. It will always run until disable Sequencer by SPI interface. */
	};
	static const uint32_t SeqB[]= {
		/**
		 * Interrupt flag AFEINTSRC_ENDSEQ will be set after this command. So We can inform MCU to read back
		 * current timer value. MCU will need some additional time to read back time count.
		 * So we use SeqB to measure how much time needed for MCU to read back
		 * */
		SEQ_STOP(),
	};
	static const uint32_t SeqBB[]= {
		SEQ_TOUT(0x3fffffff),   /* Re-Set time-out timer, so we can measure the time needed for MCU to read out Timer Count register. */
		SEQ_STOP(),             /* Interrupt flag AFEINTSRC_ENDSEQ will be set here */
	};

	if(pCfg == NULL) return -EINVAL;
	if(pFreq == NULL) return -EINVAL;
	if(pCfg->CalDuration <
	    1.0f)  /* @todo check the minimum value. The minimum value is 1.0 ms */
		return -EINVAL;
	ret = ad5940_SEQGetCfg(dev, &seq_cfg_backup);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCGetCfg(dev, AFEINTC_1, &INTCCfg);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCCfg(dev, AFEINTC_1, AFEINTSRC_ENDSEQ, true);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ALLINT);
	if (ret < 0)
		return ret;

	seq_cfg.SeqMemSize = SEQMEMSIZE_2KB;  /* 2kB SRAM is used for sequencer */
	seq_cfg.SeqBreakEn = false;
	seq_cfg.SeqIgnoreEn = false;
	seq_cfg.SeqCntCRCClr = false;
	seq_cfg.SeqEnable = true;
	seq_cfg.SeqWrTimer = 0;
	ret = ad5940_SEQCfg(dev, &seq_cfg);          /* Enable sequencer */
	if (ret < 0)
		return ret;

	seqinfo.pSeqCmd = SeqA;
	seqinfo.SeqId = SEQID_0;
	seqinfo.SeqLen = SEQ_LEN(SeqA);
	seqinfo.SeqRamAddr = pCfg->CalSeqAddr;
	seqinfo.WriteSRAM = true;
	ret = ad5940_SEQInfoCfg(dev, &seqinfo);
	if (ret < 0)
		return ret;

	seqinfo.SeqId = SEQID_1;
	seqinfo.SeqRamAddr = pCfg->CalSeqAddr + SEQ_LEN(SeqA) ;
	seqinfo.SeqLen = SEQ_LEN(SeqB);
	seqinfo.pSeqCmd = SeqB;
	ret = ad5940_SEQInfoCfg(dev,
				&seqinfo);      /* Configure sequence0 and sequence1 with command SeqA and SeqB */
	if (ret < 0)
		return ret;

	wupt_cfg.WuptEn = false;
	wupt_cfg.WuptOrder[0] = SEQID_0;
	wupt_cfg.WuptOrder[1] = SEQID_1;
	wupt_cfg.WuptEndSeq = WUPTENDSEQ_B;
	wupt_cfg.SeqxWakeupTime[0] = 4;       /* Don't care. >4 is acceptable */
	wupt_cfg.SeqxSleepTime[0] = (uint32_t)((pCfg->CalDuration)*32 + 0.5f) - 1 - 4;
	wupt_cfg.SeqxWakeupTime[1] = 4-1;
	wupt_cfg.SeqxSleepTime[1] = 0xffffffff; /* Don't care */
	WuptPeriod = (wupt_cfg.SeqxSleepTime[0]+1) + (wupt_cfg.SeqxWakeupTime[1]+1);
	ret = ad5940_WUPTCfg(dev, &wupt_cfg);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ENDSEQ);
	if (ret < 0)
		return ret;

	ret = ad5940_WUPTCtrl(dev, true);
	if (ret < 0)
		return ret;

	while(ad5940_INTCTestFlag(dev, AFEINTC_1,
				  AFEINTSRC_ENDSEQ) ==
	      false);  /* @todo if SPI speed is too slow, it will affect frequency measurement accuracy */
	ret = ad5940_SEQTimeOutRd(dev, &TimerCount);
	if (ret < 0)
		return ret;

	ret = ad5940_WUPTCtrl(dev, false);
	if (ret < 0)
		return ret;

	ret = ad5940_WUPTTime(dev, SEQID_0, 4,
			      4);	/* Set it to minimum value because we don't care about sequence0 now. We only want to measure how much time MCU will need to read register */
	if (ret < 0)
		return ret;

	seqinfo.SeqId = SEQID_1;
	seqinfo.SeqRamAddr = pCfg->CalSeqAddr + SEQ_LEN(SeqA) ;
	seqinfo.SeqLen = SEQ_LEN(SeqBB);
	seqinfo.pSeqCmd = SeqBB;
	seqinfo.WriteSRAM = true;
	ret = ad5940_SEQInfoCfg(dev, &seqinfo);
	if (ret < 0)
		return ret;

	ret = ad5940_SEQCtrlS(dev, true); /* Enable Sequencer again */
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ENDSEQ);
	if (ret < 0)
		return ret;

	ret = ad5940_WUPTCtrl(dev, true);
	if (ret < 0)
		return ret;

	while(ad5940_INTCTestFlag(dev, AFEINTC_1, AFEINTSRC_ENDSEQ) == false);
	ret = ad5940_SEQTimeOutRd(dev, &TimerCount2);
	if (ret < 0)
		return ret;

	ret = ad5940_INTCTestFlag(dev, AFEINTC_0, AFEINTSRC_ENDSEQ);
	if (ret < 0)
		return ret;

	ret = ad5940_WUPTCtrl(dev, false);
	if (ret < 0)
		return ret;

	ret = ad5940_SEQCfg(dev,
			    &seq_cfg_backup);          /* restore sequencer configuration */
	if (ret < 0)
		return ret;

	ret = ad5940_INTCCfg(dev, AFEINTC_1, AFEINTSRC_ENDSEQ,
			     (INTCCfg&AFEINTSRC_ENDSEQ)?true:false); /* Restore interrupt configuration */
	if (ret < 0)
		return ret;

	ret = ad5940_INTCClrFlag(dev, AFEINTSRC_ENDSEQ);
	if (ret < 0)
		return ret;

	//printf("Time duration:%d ", (TimerCount2 - TimerCount));
	*pFreq = pCfg->SystemClkFreq*WuptPeriod/(TimerCount2 - TimerCount);
	return 0;
}

/**
 * @} Calibration
 * @} Calibration_Block
 */

/**
 * @} AD5940_Functions
 * @} AD5940_Library
 */
