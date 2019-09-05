/*!
 * \file mykonos_gpio.h
 * \brief Contains macro definitions and function prototypes for mykonos_gpio.c
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef MYKONOSGPIO_H_
#define MYKONOSGPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "t_mykonos.h"
#include "t_mykonos_gpio.h"

/*
 *****************************************************************************
 * GPIO functions
 *****************************************************************************
 */

/* GPIO CMOS and SPI setting Functions */
mykonosGpioErr_t MYKONOS_setGpioDrv(mykonosDevice_t *device, uint32_t gpioDrv);
mykonosGpioErr_t MYKONOS_getGpioDrv(mykonosDevice_t *device, uint32_t *gpioDrv);

mykonosGpioErr_t MYKONOS_setGpioSlewRate(mykonosDevice_t *device, mykonosGpioSelect_t gpioSelect, mykonosGpioSlewRate_t slewRate);
mykonosGpioErr_t MYKONOS_getGpioSlewRate(mykonosDevice_t *device, mykonosGpioSelect_t gpioSelect, mykonosGpioSlewRate_t *slewRate);

mykonosGpioErr_t MYKONOS_setCmosDrv(mykonosDevice_t *device, mykonosCmosPadDrvStr_t spiDrv);
mykonosGpioErr_t MYKONOS_getCmosDrv(mykonosDevice_t *device, mykonosCmosPadDrvStr_t *spiDrv);


/* Monitor output Functions */
mykonosGpioErr_t MYKONOS_setGpioMonitorOut(mykonosDevice_t *device, uint8_t monitorIndex, uint8_t monitorMask);
mykonosGpioErr_t MYKONOS_getGpioMonitorOut(mykonosDevice_t *device, uint8_t *monitorIndex, uint8_t *monitorMask);

/* RX GPIO Functions */
mykonosGpioErr_t MYKONOS_setRx1GainCtrlPin(mykonosDevice_t *device, uint8_t incStep, uint8_t decStep, mykonosGpioSelect_t rx1GainIncPin, mykonosGpioSelect_t rx1GainDecPin, uint8_t enable);
mykonosGpioErr_t MYKONOS_setRx2GainCtrlPin(mykonosDevice_t *device, uint8_t incStep, uint8_t decStep, mykonosGpioSelect_t rx2GainIncPin, mykonosGpioSelect_t rx2GainDecPin, uint8_t enable);
mykonosGpioErr_t MYKONOS_getRx1GainCtrlPin(mykonosDevice_t *device, uint8_t *incStep, uint8_t *decStep, mykonosGpioSelect_t *rx1GainIncPin, mykonosGpioSelect_t *rx1GainDecPin, uint8_t *enable);
mykonosGpioErr_t MYKONOS_getRx2GainCtrlPin(mykonosDevice_t *device, uint8_t *incStep, uint8_t *decStep, mykonosGpioSelect_t *rx2GainIncPin, mykonosGpioSelect_t *rx2GainDecPin, uint8_t *enable);

mykonosGpioErr_t MYKONOS_setRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t rx1GainChangePin, mykonosGpioSelect_t rx2GainChangePin);
mykonosGpioErr_t MYKONOS_getRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t *rx1GainChangePin, mykonosGpioSelect_t *rx2GainChangePin);
mykonosGpioErr_t MYKONOS_setObsRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t obsRxGainChangePin);
mykonosGpioErr_t MYKONOS_getObsRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t *obsRxGainChangePin);

mykonosGpioErr_t MYKONOS_setRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t rx1AgcSyncPin, mykonosGpioSelect_t rx2AgcSyncPin);
mykonosGpioErr_t MYKONOS_getRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t *rx1AgcSyncPin, mykonosGpioSelect_t *rx2AgcSyncPin);
mykonosGpioErr_t MYKONOS_setObsRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t obsRxAgcSyncPin);
mykonosGpioErr_t MYKONOS_getObsRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t *obsRxAgcSyncPin);

/* TX GPIO Functions */
mykonosGpioErr_t MYKONOS_setTx1AttenCtrlPin(mykonosDevice_t *device, uint8_t stepSize, mykonosGpioSelect_t tx1AttenIncPin, mykonosGpioSelect_t tx1AttenDecPin, uint8_t enable, uint8_t useTx1ForTx2);
mykonosGpioErr_t MYKONOS_setTx2AttenCtrlPin(mykonosDevice_t *device, uint8_t stepSize, mykonosGpioSelect_t tx2AttenIncPin, mykonosGpioSelect_t tx2AttenDecPin, uint8_t enable);
mykonosGpioErr_t MYKONOS_getTx1AttenCtrlPin(mykonosDevice_t *device, uint8_t *stepSize, mykonosGpioSelect_t *tx1AttenIncPin, mykonosGpioSelect_t *tx1AttenDecPin, uint8_t *enable, uint8_t *useTx1ForTx2);
mykonosGpioErr_t MYKONOS_getTx2AttenCtrlPin(mykonosDevice_t *device, uint8_t *stepSize, mykonosGpioSelect_t *tx2AttenIncPin, mykonosGpioSelect_t *tx2AttenDecPin, uint8_t *enable, uint8_t *useTx1ForTx2);

mykonosGpioErr_t MYKONOS_spi2GpioSetup(mykonosDevice_t *device, uint8_t enable, uint8_t updateTxAttenPinSelect);

/* GPIO memory source Functions */
mykonosGpioErr_t MYKONOS_setGpioPinLevel(mykonosDevice_t *device, uint32_t gpioPinLevel);
mykonosGpioErr_t MYKONOS_getGpioPinLevel(mykonosDevice_t *device, uint32_t *gpioPinLevel);
mykonosGpioErr_t MYKONOS_getGpioSetLevel(mykonosDevice_t *device, uint32_t *gpioPinSetLevel);

/* Gain Compensation Functions */
mykonosGpioErr_t MYKONOS_setRxGainCompensation (mykonosDevice_t *device, mykonosGainComp_t *gainComp, uint8_t enable);
mykonosGpioErr_t MYKONOS_getRxGainCompensation (mykonosDevice_t *device, mykonosGainComp_t *gainComp, uint8_t *enabled);

mykonosGpioErr_t MYKONOS_setObsRxGainCompensation (mykonosDevice_t *device, mykonosObsRxGainComp_t *gainComp, uint8_t enable);
mykonosGpioErr_t MYKONOS_getObsRxGainCompensation (mykonosDevice_t *device, mykonosObsRxGainComp_t *gainComp, uint8_t *enabled);

/* SLICER Functions */
mykonosGpioErr_t MYKONOS_setRxSlicerCtrl(mykonosDevice_t *device, uint8_t slicerStep, mykonosRxSlicer_t rx1Pins, mykonosRxSlicer_t rx2Pins, uint8_t enable);
mykonosGpioErr_t MYKONOS_getRxSlicerCtrl(mykonosDevice_t *device, uint8_t *slicerStep, mykonosRxSlicer_t *rx1Pins, mykonosRxSlicer_t *rx2Pins, uint8_t *enable);

mykonosGpioErr_t MYKONOS_setObsRxSlicerCtrl(mykonosDevice_t *device, uint8_t slicerStep, mykonosObsRxSlicer_t obsRxPins, uint8_t enable);
mykonosGpioErr_t MYKONOS_getObsRxSlicerCtrl(mykonosDevice_t *device, uint8_t *slicerStep, mykonosObsRxSlicer_t *obsRxPins, uint8_t *enable);

/* Floating Point Formatter Functions */
mykonosGpioErr_t MYKONOS_setFloatPointFrmt (mykonosDevice_t *device, mykonosFloatPntFrmt_t *floatFrmt);
mykonosGpioErr_t MYKONOS_getFloatPointFrmt (mykonosDevice_t *device, mykonosFloatPntFrmt_t *floatFrmt);

mykonosGpioErr_t MYKONOS_setRxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t rx1Att, uint8_t rx2Att, uint8_t enable);
mykonosGpioErr_t MYKONOS_getRxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t *rx1Att, uint8_t *rx2Att, uint8_t *enable);

mykonosGpioErr_t MYKONOS_setOrxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t orxAtt, uint8_t enable);
mykonosGpioErr_t MYKONOS_getOrxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t *orxAtt, uint8_t *enable);

/*
 *****************************************************************************
 * General Purpose Interrupt functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_configGpInterrupt(mykonosDevice_t *device, uint16_t gpMask);
mykonosGpioErr_t MYKONOS_readGpInterruptStatus(mykonosDevice_t *device, uint16_t *status);

/*
 *****************************************************************************
 * ARM GPIO usage functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_setArmGpioPins(mykonosDevice_t *device);

/*
 *****************************************************************************
 * AuxADC/AuxDAC functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_setupAuxAdcs(mykonosDevice_t *device, uint8_t adcDecimation, uint8_t enable);
mykonosGpioErr_t MYKONOS_setAuxAdcChannel(mykonosDevice_t *device, mykonosAuxAdcChannels_t auxAdcChannel);
mykonosGpioErr_t MYKONOS_readAuxAdc(mykonosDevice_t *device, uint16_t *adcCode);
mykonosGpioErr_t MYKONOS_setupAuxDacs(mykonosDevice_t *device);
mykonosGpioErr_t MYKONOS_writeAuxDac(mykonosDevice_t *device, uint8_t auxDacIndex, uint16_t auxDacCode);

/*
 *****************************************************************************
 * GPIO3v3 functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_setupGpio3v3(mykonosDevice_t *device);

mykonosGpioErr_t MYKONOS_setGpio3v3PinLevel(mykonosDevice_t *device, uint16_t gpio3v3PinLevel);
mykonosGpioErr_t MYKONOS_getGpio3v3SetLevel(mykonosDevice_t *device, uint16_t *gpio3v3SetLevel);

mykonosGpioErr_t MYKONOS_getGpio3v3PinLevel(mykonosDevice_t *device, uint16_t *gpio3v3PinLevel);

mykonosGpioErr_t MYKONOS_setGpio3v3Oe(mykonosDevice_t *device, uint16_t gpio3v3OutEn);
mykonosGpioErr_t MYKONOS_getGpio3v3Oe(mykonosDevice_t *device, uint16_t *gpio3v3OutEn);

mykonosGpioErr_t MYKONOS_setGpio3v3SourceCtrl(mykonosDevice_t *device, uint16_t gpio3v3SrcCtrl);
mykonosGpioErr_t MYKONOS_getGpio3v3SourceCtrl(mykonosDevice_t *device, uint16_t *gpio3v3SrcCtrl);

/*
 *****************************************************************************
 * GPIO helper functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_setupGpio(mykonosDevice_t *device);
mykonosGpioErr_t MYKONOS_setGpioOe(mykonosDevice_t *device, uint32_t gpioOutEn, uint32_t gpioUsedMask);
mykonosGpioErr_t MYKONOS_getGpioOe(mykonosDevice_t *device, uint32_t *gpioOutEn);

mykonosGpioErr_t MYKONOS_setGpioSourceCtrl(mykonosDevice_t *device, uint32_t gpioSrcCtrl);
mykonosGpioErr_t MYKONOS_getGpioSourceCtrl(mykonosDevice_t *device, uint32_t *gpioSrcCtrl);

/*
 *****************************************************************************
 * Temperature Sensor functions
 *****************************************************************************
 */
mykonosGpioErr_t MYKONOS_setupTempSensor(mykonosDevice_t *device, mykonosTempSensorConfig_t *tempSensor);
mykonosGpioErr_t MYKONOS_getTempSensorConfig(mykonosDevice_t *device, mykonosTempSensorConfig_t *tempSensor);

mykonosGpioErr_t MYKONOS_startTempMeasurement(mykonosDevice_t *device);
mykonosGpioErr_t MYKONOS_readTempSensor(mykonosDevice_t *device, mykonosTempSensorStatus_t *tempStatus);

const char* getGpioMykonosErrorMessage(mykonosGpioErr_t errorCode);

#ifdef __cplusplus
}
#endif

#endif /* MYKONOSGPIO_H_ */
