/*! ****************************************************************************
 * @file:    eit.c
 * @brief:
 * @details:
 * -----------------------------------------------------------------------------
 * Copyright (c) 2020 Analog Devices, Inc.
 *
 * All rights reserved.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "no-os/delay.h"
#include "no-os/uart.h"
#include "no-os/spi.h"
#include "no-os/gpio.h"
#include "stm32_spi.h"
#include "stm32_gpio.h"
#include "bia_measurement.h"
#include "mux_board.h"
#include "app.h"
#include "parameters.h"

extern struct uart_desc *uart;

#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];
struct electrode_combo swComboSeq[1024];

float SinFreqVal = 0.0;
unsigned int SinFreqValUINT = 0;
unsigned int runningCmd = 0;

volatile uint32_t ucInterrupted = 0; /* Flag to indicate interrupt occurred */
extern volatile unsigned char szInSring[32];

/* Since the reset pin is mapped to GPIO of AD5940, Access it through
 * AD5940 GPIO Register. */
int ADG2128_SwRst(struct ad5940_dev *dev)
{
	int ret = ad5940_WriteReg(dev, REG_AGPIO_GP0OUT, AGPIO_Pin1);
	if (ret < 0)
		return ret;
	udelay(1);
	return ad5940_WriteReg(dev, REG_AGPIO_GP0OUT, AGPIO_Pin1);
}

uint32_t GetMCUIntFlag(void)
{
	return ucInterrupted;
}

uint32_t ClrMCUIntFlag(void)
{
	ucInterrupted = 0;
	return 1;
}

void configMeasurement(struct measurement_config *oldCfg,
		       struct measurement_config newCfg)
{
	AppBiaCfg_Type *pBiaCfg;
	AppBiaGetCfg(&pBiaCfg);
	if (oldCfg->nFrequency != newCfg.nFrequency) {
		pBiaCfg->bParamsChanged = true;
		oldCfg->nFrequency = newCfg.nFrequency;
		pBiaCfg->SinFreq = ((float)(newCfg.nFrequency)) * 1000.0;
	}

	if (oldCfg->nAmplitudePP != newCfg.nAmplitudePP) {
		pBiaCfg->bParamsChanged = true;
		oldCfg->nAmplitudePP = newCfg.nAmplitudePP;
		pBiaCfg->DacVoltPP = newCfg.nAmplitudePP*1.0;
	}
	pBiaCfg->bImpedanceReadMode = newCfg.bImpedanceReadMode;
	pBiaCfg->SweepCfg.SweepEn = newCfg.bSweepEn;
	if (newCfg.bImpedanceReadMode) { // Impedance mode
		pBiaCfg->FifoThresh = 4;
	} else {
		pBiaCfg->FifoThresh = 2;
	}
	// Set newCfg as old for next command execution;
	*oldCfg = newCfg;
}

void ParseResultMode(struct measurement_config *pMeasCfg)
{
	char *cmd_ptr;
	char cTmp;
	uint8_t cmd_ok = 0;
	char hex_string_byte_param[10];
	cmd_ptr = strtok(NULL, ",");
	pMeasCfg->bImpedanceReadMode = false; //default
	if (cmd_ptr) { // If last parameter exists read it
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%c", &cTmp);
		if (cmd_ok) {
			if (cTmp == 'Z')
				pMeasCfg->bImpedanceReadMode = true;
		}
	}

	cmd_ptr = strtok(NULL, ",");
	pMeasCfg->bMagnitudeMode = false;
	if (cmd_ptr) { // If parameter exists read it
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%c", &cTmp);
		if (cmd_ok) {
			if (cTmp == 'M')
				pMeasCfg->bMagnitudeMode = true;
		}
	}
}

int32_t ParseConfig(char  *pStr,
		    struct eit_config *pEitCfg,
		    struct measurement_config *pMeasCfg)
{
	char *cmd_ptr;
	uint8_t cmd_ok = 0;
	char hex_string_byte_param[10];
	*pStr = 0;
	cmd_ptr = strtok(pStr + 1, ",");
	strcpy(hex_string_byte_param, cmd_ptr);
	cmd_ok = sscanf(hex_string_byte_param, "%hu", &pMeasCfg->nFrequency);
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu",
				&pEitCfg->nElectrodeCnt);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pEitCfg->nForceDist);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pEitCfg->nSenseDist);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pEitCfg->nRefEl);
	}
	if (cmd_ok)
		ParseResultMode(pMeasCfg);

	if (!cmd_ok) //Error Parsing
		return 1;
	else
		return 0;
}

int32_t ParseQuery(char *pStr,
		   struct electrode_combo *pElCmb,
		   struct measurement_config *pMeasCfg)
{
	char *cmd_ptr;
	uint8_t cmd_ok = 0;
	char hex_string_byte_param[10];
	*pStr = 0;
	cmd_ptr = strtok(pStr + 1, ",");
	strcpy(hex_string_byte_param, cmd_ptr);
	cmd_ok = sscanf(hex_string_byte_param, "%hu", &pMeasCfg->nFrequency);
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pElCmb->F_plus);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pElCmb->F_minus);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pElCmb->S_plus);
	}
	if (cmd_ok) {
		cmd_ptr = strtok(NULL, ",");
		strcpy(hex_string_byte_param, cmd_ptr);
		cmd_ok = sscanf(hex_string_byte_param, "%hu", &pElCmb->S_minus);
	}
	if (cmd_ok)
		ParseResultMode(pMeasCfg);

	if (!cmd_ok) //Error Parsing
		return 1;
	else
		return 0;
}

void SendResultUint32(uint32_t *pData, uint32_t nDataCount)
{
	uint32_t i;

	for (i = 0; i < nDataCount - 1; i++) {
		printf("%lx,", pData[i]);
	}

	printf("%lx", pData[i]);
}

void SendResultFloat32(float *data, uint32_t DataCount)
{
	uint32_t i;

	for (i = 0; i < DataCount - 1; i++) {
		printf("%0.4f,", data[i]);
	}

	printf("%0.4f", data[i]);
}

void SendResultIeee754(float *data, uint32_t DataCount)
{

	uint32_t *pVal = (uint32_t *)((void *)data);
	uint32_t i;

	for (i = 0; i < DataCount - 1; i++) {
		printf("%lx,", pVal[i]);
	}

	printf("%lx", pVal[i]);
}

void SendResult(uint32_t *pData, uint16_t len,
		bool bImpedanceReadMode, bool bMagnitudeMode)
{
	float fMagVal = 0;
	fImpCar_Type fCarZval;
	iImpCar_Type iCarVval;
	signExtend18To32(pData, len);
	if (bImpedanceReadMode && (len == 4)) { // Send Impedance
		fCarZval = computeImpedance(pData);
		if (bMagnitudeMode) { // Complex to Magnitude
			fMagVal = sqrt(fCarZval.Real*fCarZval.Real +
				       fCarZval.Image*fCarZval.Image);
			SendResultIeee754(&fMagVal, 1); // Impedance Magnitude only. Float
		} else { // Complex Impedance in IEE754 uint32 hex string.
			SendResultIeee754((float *)&fCarZval, 2);
		}
	} else if((!bImpedanceReadMode) && (len == 2)) { // Send Voltage
		if (bMagnitudeMode) { // Complex to Magnitude
			iCarVval = *((iImpCar_Type *)pData);
			fMagVal = sqrt((iCarVval.Real*1.0)*(iCarVval.Real*1.0) +
				       (iCarVval.Image*1.0)*(iCarVval.Image*1.0));
			SendResultIeee754(&fMagVal, 1); // Voltage Magnitude only. Float
		} else { // Complex Voltage in uint32 hex string.
			SendResultUint32(pData, 2);
		}
	}
}

void MuxSupportedElectrodeCounts()
{
	//uint8_t outBuff[20]={0};
	printf("%s","08,10,20");
}

/* !!Change the application parameters here if you want to change it to
 * none-default value */
void AD5940BiaStructInit(void)
{
	AppBiaCfg_Type *pBiaCfg;

	AppBiaGetCfg(&pBiaCfg);

	pBiaCfg->SeqStartAddr = 0;
	pBiaCfg->MaxSeqLen = 512; /** @todo add checker in function */

	pBiaCfg->RcalVal = 10000.0;
	pBiaCfg->DftNum = DFTNUM_8192;
	//pBiaCfg->DftNum = DFTNUM_2048;
	pBiaCfg->NumOfData = -1; /* Never stop until you stop it manually by
							  * AppBiaCtrl(dev, ) function */
	pBiaCfg->BiaODR = 20;	 /* ODR(Sample Rate) 20Hz */
	pBiaCfg->FifoThresh = 2; /* 4 */
	pBiaCfg->ADCSinc3Osr = ADCSINC3OSR_2;

	pBiaCfg->DacVoltPP = 300.0; //800.0
	pBiaCfg->SinFreq = 10000.0; /* 10000Hz */
	pBiaCfg->SweepCfg.SweepEn = false;
	pBiaCfg->SweepCfg.SweepStart = 10000;
	pBiaCfg->SweepCfg.SweepStop = 80000.0;
	pBiaCfg->SweepCfg.SweepPoints = 20;
	pBiaCfg->SweepCfg.SweepLog = true;
	pBiaCfg->SweepCfg.SweepIndex = 0;
}

uint16_t generateSwitchCombination(struct eit_config eitCfg,
				   struct electrode_combo *swSeq)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint8_t F_plus;
	uint8_t F_minus;
	uint8_t S_plus;
	uint8_t S_minus;
	uint16_t seqCtr = 0;
	for (i = 0; i < eitCfg.nElectrodeCnt; i++) {
		F_plus = i;
		F_minus = (i + eitCfg.nForceDist) % eitCfg.nElectrodeCnt;
		for (j = 0; j < eitCfg.nElectrodeCnt; j++) {
			S_plus = j % eitCfg.nElectrodeCnt;
			if (S_plus == F_plus || S_plus == F_minus)
				continue;
			S_minus = (S_plus + eitCfg.nSenseDist) % eitCfg.nElectrodeCnt;
			if (S_minus == F_plus || S_minus == F_minus)
				continue;

			swSeq[seqCtr].F_plus = F_plus;
			swSeq[seqCtr].F_minus = F_minus;
			swSeq[seqCtr].S_plus = S_plus;
			swSeq[seqCtr++].S_minus = S_minus;
		}
	}
	return seqCtr;
}

int app_main(struct i2c_desc *i2c)
{
	int ret;
	static uint32_t IntCount;
	struct eit_config oldEitCfg;
	struct electrode_combo oldElCfg;
	struct measurement_config oldMeasCfg;

	struct eit_config newEitCfg;
	struct electrode_combo newElCfg;
	struct measurement_config newMeasCfg;

	char *buffStr=0;
	uint32_t temp;
	uint16_t switchSeqCnt = 0;
	uint16_t switchSeqNum = 0;

	int32_t cmd_err = 0;
	uint8_t lastConfig = 'C';

	struct stm32_spi_init_param xsip  = {
		.chip_select_port = SPI_CS_PORT,
		.get_input_clock = HAL_RCC_GetPCLK1Freq,
	};

	struct spi_init_param sip = {
		.device_id = SPI_DEVICE_ID,
		.max_speed_hz = 3000000,
		.bit_order = SPI_BIT_ORDER_MSB_FIRST,
		.mode = SPI_MODE_0,
		.extra = &xsip,
		.platform_ops = &stm32_spi_ops,
		.chip_select = SPI_CS,
	};

	struct stm32_gpio_init_param xgip = {
		.port = GPIOD,
		.mode = GPIO_MODE_OUTPUT_PP,
		.pull = GPIO_NOPULL,
		.speed = GPIO_SPEED_FREQ_VERY_HIGH,
	};

	struct gpio_init_param reset_gip = {
		.number = 12,
		.platform_ops = &stm32_gpio_ops,
		.extra = &xgip,
	};

	struct ad5940_init_param ad5940_ip = {
		.spi_init = sip,
		.reset_gpio_init = reset_gip,
	};
	struct ad5940_dev *ad5940;
	ret = ad5940_init(&ad5940, &ad5940_ip);
	if (ret)
		return ret;
	AD5940BiaStructInit(); /* Configure your parameters in this function */

	oldMeasCfg.bImpedanceReadMode = true;
	oldMeasCfg.bMagnitudeMode = false;
	oldMeasCfg.nFrequency = 10;	// default 10 Khz Excitation
	oldMeasCfg.nAmplitudePP = 300; // default 300mV peak to peak excitation
	oldMeasCfg.bSweepEn = false;

	oldElCfg.F_plus = 0;
	oldElCfg.F_minus = 3;
	oldElCfg.S_plus = 1;
	oldElCfg.S_minus = 2;

	oldEitCfg.nElectrodeCnt = 16;
	oldEitCfg.nForceDist = 1;
	oldEitCfg.nSenseDist = 1;
	oldEitCfg.nRefEl = 0;

	switchSeqNum = 0;
	switchSeqCnt = generateSwitchCombination(oldEitCfg, swComboSeq);

	char szInSring[32];

	while (1) {
		// execute these commands regardless of state
		memset(szInSring, 0, sizeof(szInSring));
		fgets(szInSring, sizeof(szInSring), stdin);
		if (szInSring[0] == 'O') { // STOP
			ADG2128_SwRst(ad5940);
			AppBiaCtrl(ad5940, BIACTRL_STOPNOW, 0);
			printf("%s","\n!O ");
			runningCmd = 0;
			fflush(stdin);
			printf("%s","\n");
		}
		if (szInSring[0] == 'R') { // RESET
			ADG2128_SwRst(ad5940);
			printf("%s","!R \n");
			runningCmd = 0;
			fflush(stdin);
		}

		if (!runningCmd) {
			if (szInSring[0] == 'B') { //
				ADG2128_SwRst(ad5940);
				printf("%s","!B ");
				runningCmd = 0;
				fflush(stdin);
				MuxSupportedElectrodeCounts();
				putchar('\n');
			}

			if (szInSring[0] == 'Q') { //Run Specific combination
				AppBiaCtrl(ad5940, BIACTRL_STOPNOW, 0);
				szInSring[0] = 0;
				newElCfg = oldElCfg;
				newMeasCfg = oldMeasCfg;
				buffStr = (char *) (szInSring+1);
				cmd_err = ParseQuery(buffStr,
						     &newElCfg,
						     &newMeasCfg);
				fflush(stdin);
				// No errors during parsing.
				if (!cmd_err) {
					runningCmd = 'Q';
					printf("%s","!CMD Q OK\n");
					configMeasurement(&oldMeasCfg, newMeasCfg);
					AppBiaInit(ad5940, AppBuff, APPBUFF_SIZE);
					udelay(10);
					printf("%s","!Q ");
					setMuxSwitch(i2c, ad5940, newElCfg, MUXBOARD_SIZE);
					udelay(3);
					AppBiaCtrl(ad5940, BIACTRL_START, 0);
					lastConfig = 'Q';
				} else {
					printf("%s","!CMD Q ERROR\n");
				}
			}

			if (szInSring[0] == 'C') { //Configure
				AppBiaCtrl(ad5940, BIACTRL_STOPNOW, 0);
				szInSring[0] = 0;
				newEitCfg = oldEitCfg;
				newMeasCfg = oldMeasCfg;
				buffStr = (char *) (szInSring+1);
				cmd_err = ParseConfig(buffStr,
						      &newEitCfg,
						      &newMeasCfg);
				fflush(stdin);
				//all command params are valid, execute command
				if (!cmd_err) {
					runningCmd = 0;
					switchSeqNum = 0;
					printf("%s","!CMD C OK\n");
					switchSeqCnt = generateSwitchCombination(newEitCfg,
							swComboSeq);
					configMeasurement(&oldMeasCfg, newMeasCfg);
					AppBiaInit(ad5940, AppBuff, APPBUFF_SIZE);
					udelay(10);
					printf("%s","!C OK\n");
					lastConfig = 'C';
				} else {
					printf("%s","!CMD C ERROR\n");
				}
			}

			if (szInSring[0] == 'V') { //Start boundary voltage query sequence
				fflush(stdin);
				if (lastConfig == 'C') {
					printf("%s","!CMD V OK\n");
					runningCmd = 'V';
					switchSeqNum = 0;
					setMuxSwitch(i2c, ad5940, swComboSeq[switchSeqNum++], newEitCfg.nElectrodeCnt);
					AppBiaInit(ad5940, AppBuff, APPBUFF_SIZE);
					udelay(10);
					AppBiaCtrl(ad5940, BIACTRL_START, 0);
					printf("%s","!V ");
				} else
					printf("%s","!Send C Command first to configure!\n");
			}
		}

		/* Check if interrupt flag which will be set when interrupt occured. */
		if (GetMCUIntFlag()) {
			IntCount++;
			ClrMCUIntFlag(); /* Clear this flag */
			temp = APPBUFF_SIZE;
			AppBiaISR(ad5940, AppBuff,
				  &temp); /* Deal with it and provide a buffer to store data we got */
			AppBiaCtrl(ad5940, BIACTRL_STOPNOW, 0);
			if (runningCmd == 'V' || runningCmd == 'Q') {
				//If Q command is being ran return result
				if (runningCmd == 'Q') {
					SendResult(AppBuff, temp, newMeasCfg.bImpedanceReadMode,
						   newMeasCfg.bMagnitudeMode);
					putchar('\n');
					runningCmd = 0;
				}
				//If V or Z command is being ran and this is the last set of ADC, send a terminator character
				if ((runningCmd == 'V') && switchSeqNum >= switchSeqCnt) {
					SendResult(AppBuff, temp, newMeasCfg.bImpedanceReadMode,
						   newMeasCfg.bMagnitudeMode);
					putchar('\n');;
					runningCmd = 0;
				}

				//if V is still running and switch combinations are not exhausted, restart AFE Seq with new switch combo
				if ((runningCmd == 'V') && switchSeqNum < switchSeqCnt) {
					SendResult(AppBuff, temp, newMeasCfg.bImpedanceReadMode,
						   newMeasCfg.bMagnitudeMode);
					putchar(',');
					setMuxSwitch(i2c, ad5940, swComboSeq[switchSeqNum++], newEitCfg.nElectrodeCnt);
					udelay(3);
					AppBiaCtrl(ad5940, BIACTRL_START, 0);
				}
			}
		}
	}
}
