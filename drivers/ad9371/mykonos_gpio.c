/**
 *\file mykonos_gpio.c
 *
 *\brief Contains Mykonos APIs for transceiver GPIO configuration and control.
 *
 * Mykonos API version: 1.3.1.3534
 */


/**
 * \page Disclaimer Legal Disclaimer
 * WARRANTY DISCLAIMER: THE SOFTWARE AND ANY RELATED INFORMATION AND/OR ADVICE IS PROVIDED ON AN
 * “AS IS” BASIS, WITHOUT REPRESENTATIONS, GUARANTEES OR WARRANTIES OF ANY KIND, EXPRESS OR IMPLIED,
 * ORAL OR WRITTEN, INCLUDING WITHOUT LIMITATION, WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT.
 */

#include <stdint.h>
#include <stddef.h>
#include "common.h"
#include "mykonos.h"
#include "mykonos_gpio.h"
#include "mykonos_macros.h"
#include "mykonos_user.h"


/**
 * \brief Helper function for return of character string based on 32-bit mykonosGpioErr_t enum value
 *
 * To save codespace, these error strings are ifdef'd out unless the user
 * adds a define MYKONOS_VERBOSE to their workspace.  This function can be
 * useful for debug.  Each function also returns unique error codes to
 * make it easier to determine where the code broke.
 *
 * \param errorCode is enumerated error code value
 *
 * \return Returns character string based on enumerated value
 */
const char* getGpioMykonosErrorMessage(mykonosGpioErr_t errorCode)
{
#if MYKONOS_VERBOSE == 0
        return "";

 #else

    switch (errorCode)
    {
        case MYKONOS_ERR_GPIO_OK:
            return "";

        case MYKONOS_ERR_EN_MONITOR_OUT_NOT_ENABLED:
            return "MYKONOS_setGpioMonitorOut() Mykonos monitor output not enable, please run GPIO setup with the correct setup for enabling the monitor output functionality\n";

        case MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE:
            return "MYKONOS_setGpioMonitorOut()The index specified is incorrect, index available are from 0x01 to 0x42\n";

        case MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM:
            return "MYKONOS_getGpioMonitorOut() if the monitorIndex is null.\n";

        case MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM:
            return "MYKONOS_getGpioMonitorOut() if the monitorMask is null.\n";

        case MYKONOS_ERR_MGCRX1_STEP_INV_PARAM:
            return "MYKONOS_setRx1GainCtrlPin() An invalid step size has been passed, valid step sizes for increment/decrement is 0-7\n";

        case MYKONOS_ERR_MGCRX1_GPIO_DECPIN_INV_PARAM:
            return "MYKONOS_setRx1GainCtrlPin() An invalid decrement pin has been passed \n";

        case MYKONOS_ERR_MGCRX1_GPIO_INCPIN_INV_PARAM:
            return "MYKONOS_setRx1GainCtrlPin() An invalid increment pin has been passed, \n";

        case MYKONOS_ERR_MGCRX2_STEP_INV_PARAM:
            return "MYKONOS_setRx2GainCtrlPin() An invalid step size has been passed, valid step sizes for increment/decrement is 0-7\n";

        case MYKONOS_ERR_MGCRX2_GPIO_DECPIN_INV_PARAM:
            return "MYKONOS_setRx2GainCtrlPin() An invalid decrement pin has been passed\n";

        case MYKONOS_ERR_MGCRX2_GPIO_INCPIN_INV_PARAM:
            return "MYKONOS_setRx2GainCtrlPin() An invalid increment pin has been passed\n";

        case MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM:
            return "MYKONOS_getRx1GainCtrlPin() if a null value has been passed to incStep.\n";

        case MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM:
            return "MYKONOS_getRx1GainCtrlPin() if a null value has been passed to decStep.\n";

        case MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM:
            return "MYKONOS_getRx1GainCtrlPin() if a null value has been passed to rx1GainIncPin.\n";

        case MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM:
            return "MYKONOS_getRx1GainCtrlPin() if a null value has been passed to rx1GainDecPin.\n";

        case MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM:
            return "MYKONOS_getRx1GainCtrlPin() if a null value has been passed to enable.\n";

        case MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM:
            return "MYKONOS_getRx2GainCtrlPin() if a null value has been passed to incStep.\n";

        case MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM:
            return "MYKONOS_getRx2GainCtrlPin() if a null value has been passed to decStep.\n";

        case MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM:
            return "MYKONOS_getRx2GainCtrlPin() if a null value has been passed to rx1GainIncPin.\n";

        case MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM:
            return "MYKONOS_getRx2GainCtrlPin() if a null value has been passed to rx1GainDecPin.\n";

        case MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM:
            return "MYKONOS_getRx2GainCtrlPin() if a null value has been passed to enable.\n";

        case MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM:
            return "MYKONOS_setTx1AttenCtrlPin() An invalid step size has been passed, valid step sizes for att are 0x00-0x1F\n";

        case MYKONOS_ERR_TPCTX1_GPIO_INCPIN_INV_PARAM:
            return "MYKONOS_setTx1AttenCtrlPin() An invalid decrement pin has been passed\n";

        case MYKONOS_ERR_TPCTX1_GPIO_DECPIN_INV_PARAM:
            return "MYKONOS_setTx1AttenCtrlPin() An invalid decrement pin has been passed\n";

        case MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM:
            return "MYKONOS_setTx2AttenCtrlPin() An invalid step size has been passed, valid step sizes for att are 0x00-0x1F\n";

        case MYKONOS_ERR_TPCTX2_GPIO_INCPIN_INV_PARAM:
            return "MYKONOS_setTx2AttenCtrlPin() An invalid decrement pin has been passed\n";

        case MYKONOS_ERR_TPCTX2_GPIO_DECPIN_INV_PARAM:
            return "MYKONOS_setTx2AttenCtrlPin() An invalid decrement pin has been passed\n";

        case MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM:
            return "MYKONOS_getTx2AttenCtrlPin() if a null value has been passed to stepSize.\n";

        case MYKONOS_ERR_GETTX2PIN_INC_NULL_PARM:
            return "MYKONOS_getTx2AttenCtrlPin() if a null value has been passed to tx2AttenIncPin.\n";

        case MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM:
            return "MYKONOS_getTx2AttenCtrlPin() if a null value has been passed to tx2AttenDecPin.\n";

        case MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM:
            return "MYKONOS_getTx2AttenCtrlPin() if a null value has been passed to enable.\n";

        case MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM:
            return "MYKONOS_getTx2AttenCtrlPin() if a null value has been passed to useTx1ForTx2.\n";

        case MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM:
            return "MYKONOS_getTx1AttenCtrlPin() if a null value has been passed to stepSize.\n";

        case MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM:
            return "MYKONOS_getTx1AttenCtrlPin() if a null value has been passed to tx1AttenIncPin.\n";

        case MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM:
            return "MYKONOS_getTx1AttenCtrlPin() if a null value has been passed to tx1AttenDecPin.\n";

        case MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM:
            return "MYKONOS_getTx1AttenCtrlPin() if a null value has been passed to enable.\n";

        case MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM:
            return "MYKONOS_getTx1AttenCtrlPin() if a null value has been passed to useTx1ForTx2.\n";

        case MYKONOS_ERR_GPIO_SRC_PARAM_INV:
            return "MYKONOS_setGpioSourceCtrl() An invalid source control parameter has been passed\n";

        case MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM:
            return "MYKONOS_getGpioSetLevel() gpioPinSetLevel pointer is NULL in function parameter\n";

        case MYKONOS_ERR_READGPIOSPI_NULL_PARM:
            return "MYKONOS_readGpioPinLevel() has a null *gpioPinLevel parameter\n";

        case MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM:
            return "MYKONOS_getGpio3v3PinLevel() has a null *gpio3v3PinLevel parameter\n";

        case MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM:
            return "MYKONOS_setupAuxDac() has a null device->auxIo pointer\n";

        case MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM:
            return "MYKONOS_getGpio3v3Oe() has NULL pointer for function parameter gpio3v3OutEn\n;";

        case MYKONOS_ERR_INV_GP_INT_MASK_PARM:
            return "General Purpose Interrupt source mask parameter is invalid\n";

        case MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM:
            return "MYKONOS_readGpInterruptStatus() has NULL status parameter\n";

        case MYKONOS_ERR_SET_GPIO_3V3_INV_MODE:
            return "Invalid GPIO3v3 source control mode\n";

        case MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL:
            return "gpio3v3 members have invalid value for the GPIO3v3 source control mode.\n";

        case MYKONOS_ERR_SET_GPIO_1V8_INV_MODE:
            return "The GPIO mode enum is not a valid value in MYKONOS_setupGpio\n";

        case MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM:
            return "MYKONOS_getGpio3v3SetLevel() has NULL pointer for function parameter gpio3v3SetLevel\n";

        case MYKONOS_ERR_GPIO_OE_INV_PARAM:
            return "MYKONOS_setGpioOe() had invalid parameter gpioOutEn (valid range 0 - 0x07FFFF)\n";

        case MYKONOS_ERR_GETGPIO_OE_NULL_PARM:
            return "MYKONOS_getGpioOe() has NULL function parameter\n";

        case MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO:
            return "device->auxIo structure has NULL pointer in MYKONOS_writeAuxDac()\n";

        case MYKONOS_ERR_READAUXADC_NULL_PARAM:
            return "MYKONOS_readAuxAdc() has NULL function parameter\n";

        case MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER:
            return "Invalid pointer detected in MYKONOS_setupGpio3v3()\n";

        case MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER:
            return "Invalid pointer detected in MYKONOS_setupGpio()\n";

        case MYKONOS_ERR_SET_ARMGPIO_INV_POINTER:
            return "Invalid pointer detected at device->auxIo->armGpio in MYKONOS_setArmGpioPins() \n";

        case MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR:
            return "ARM Command Error in MYKONOS_setArmGpioPins()\n";

        case MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID:
            return "Invalid Signal ID detected in device->auxIo->armGpio \n";

        case MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN:
            return "Out of range GPIO pin detected \n";

        case MYKONOS_ERR_SET_RADIOCTRL_PINS_ARMERROR:
            return "ARM command Error in MYKONOS_setRadioControlPinMode()\n";

        case MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE:
            return "device->auxIo->auxDacValue, AUXDAC value out of range in MYKONOS_setupAuxDacs()\n";

        case MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE:
            return "auxDacCode value out of range in MYKONOS_writeAuxDac()\n";

        case MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX:
            return "auxDacIndex value out of range in MYKONOS_writeAuxDac()\n";

        case MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM:
            return "auxAdcChannel value out of range in MYKONOS_setAuxAdcChannel()\n";

        case MYKONOS_ERR_SETUPAUXADC_INV_VCODIV:
            return "device->clocks->clkPllVcoDiv value not supported in MYKONOS_setupAuxAdcs()\n";

        case MYKONOS_ERR_INV_AUX_ADC_DEC_PARM:
            return "adcDecimation value out of range in MYKONOS_setupAuxAdcs()\n";

        case MYKONOS_ERR_SLICER_INV_RX1_SEL:
            return "invalid RX1 GPIO pin selection for Slicer control in MYKONOS_setRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_INV_RX2_SEL:
            return "invalid RX2 GPIO pin selection for Slicer control in MYKONOS_setRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE:
            return "slicer step is out of range for the SLicer control in MYKONOS_setRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_EN_INV:
            return "invalid enable in MYKONOS_setRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM:
            return "rx1Pins is null pointer for the passed parameter in MYKONOS_getRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM:
            return "rx1Pins is null pointer for the passed parameter in MYKONOS_getRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_STEP_NULL_PARM:
            return "slicerStep is null pointer for the passed parameter in MYKONOS_getRxSlicerCtrl()\n";

        case MYKONOS_ERR_SLICER_EN_NULL_PARM:
            return "enable is null pointer for the passed parameter in MYKONOS_getRxSlicerCtrl()\n";

        case MYKONOS_ERR_GAINCOMP_NULL_STRUCT:
            return "gain compensation structure gainComp is not initialised in MYKONOS_getRxGainCompensation()\n";

        case MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT:
            return "gain compensation structure gainComp is not initialised in MYKONOS_setRxGainCompensation()\n";

        case MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET:
            return "gain compensation structure gainComp->rx1Offset is invalid in MYKONOS_setRxGainCompensation()\n";

        case MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET:
            return "gain compensation structure gainComp->rx2Offset is invalid in MYKONOS_setRxGainCompensation()\n";

        case MYKONOS_ERR_GAINCOMP_INV_STEP:
            return "gain compensation structure gainComp->compStep is invalid in MYKONOS_setRxGainCompensation()\n";

        case MYKONOS_ERR_GAINCOMP_INV_EN:
            return "enable is not valid in MYKONOS_setRxGainCompensation()\n";

        case MYKONOS_ERR_FLOATFRMT_NULL_STRUCT:
            return "floating point formatter structure floatFrmt not initialised in MYKONOS_getFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT:
            return "floating point formatter structure floatFrmt not initialised in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE:
            return "floating point formatter structure floatFrmt.roundMode not valid parameter in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT:
            return "floating point formatter structure floatFrmt.dataFormat not valid parameter in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN:
            return "floating point formatter structure floatFrmt.encNan not valid parameter in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS:
            return "floating point formatter structure floatFrmt.expBits not valid parameter in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_LEADING:
            return "floating point formatter structure floatFrmt.leading not valid parameter in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_RX1ATT:
            return "not valid rx1 attenuation parameter passed in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_INV_RX2ATT:
            return "not valid rx2 attenuation parameter passed in MYKONOS_setFloatPointFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT:
            return "not valid rx2 attenuation parameter passed in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT:
            return "not valid rx1 attenuation parameter passed in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_SET_INV_EN:
            return "not valid enable parameter passed in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT:
            return "null pointer passed for rx1 attenuation in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT:
            return "null pointer passed for rx2 attenuation in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_FLOATFRMT_NULL_ENABLE:
            return "null pointer passed for enable in MYKONOS_setRxEnFloatPntFrmt()\n";

        case MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM:
            return "MYKONOS_setupTempSensor() has NULL function parameter\n";

        case MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION:
            return "tempDecimation value out of range in MYKONOS_setupTempSensor()\n";

        case MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW:
            return "tempWindow value out of range in MYKONOS_setupTempSensor()\n";

        case MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET:
            return "offset value out of range in MYKONOS_setupTempSensor()\n";

        case MYKONOS_ERR_GETTEMPSENSORCFG_NULL_PARAM:
            return "MYKONOS_getTempSensorConfig() has NULL function parameter\n";

        case MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM:
            return "MYKONOS_readTempSensor() has NULL function parameter\n";

        case MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED:
            return "MYKONOS_readTempSensor() temperature sensor reading is not locked\n";

        case MYKONOS_ERR_GPIO_HYBRID_RX1_PIN:
            return "MYKONOS_setRxHybridGainChangePin() not valid pin has been passed for RX1 hybrid gain control.\n";

        case MYKONOS_ERR_GPIO_HYBRID_RX2_PIN:
            return "MYKONOS_setRxHybridGainChangePin() not valid pin has been passed for RX2 hybrid gain control.\n";

        case MYKONOS_ERR_GPIO_HYBRID_ORX_PIN:
            return "MYKONOS_setObsRxHybridGainChangePin() if invalid pin has been passed for ORX hybrid gain control.\n";

        case MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID:
            return "MYKONOS_setObsRxHybridGainChangePin() if the observation gain mode is not set to Hybrid.\n";

        case MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM:
            return "MYKONOS_getRxHybridGainChangePin() null value has been passed to rx1GainChangePin.\n";

        case MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM:
            return "MYKONOS_getRxHybridGainChangePin() null value has been passed to rx1GainChangePin.\n";

        case  MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID:
            return "MYKONOS_setRxHybridGainChangePin() if gain control is not hybrid.\n";

        case MYKONOS_ERR_GAIN_CONTROL_NOT_AGC :
            return "MYKONOS_setRxAgcEnSyncPin() or MYKONOS_getObsRxAgcEnSyncPin() if the observation gain control mode is not set to AGC.\n";

        case MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC:
            return "MYKONOS_getObsRxAgcEnSyncPin() if the observation gain control mode is not set to AGC.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN:
            return "MYKONOS_setRxAgcEnSyncPin() if invalid pin has been passed for RX1 AGC sync gain control.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN:
            return "MYKONOS_setRxAgcEnSyncPin() if invalid pin has been passed for RX2 AGC sync gain control.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM:
            return "MYKONOS_getRxAgcEnSyncPin()  if a null value has been passed to rx1AgcSyncPin.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM:
            return "MYKONOS_getRxAgcEnSyncPin()  if a null value has been passed to rx2AgcSyncPin.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN:
           return "MYKONOS_setObsRxAgcEnSyncPin() if invalid pin has been passed for ORX AGC sync gain control.\n";

        case MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM:
            return "MYKONOS_getObsRxAgcEnSyncPin() if a null value has been passed to obsRxAgcSyncPin.\n";

        case MYKONOS_ERR_SLICER_INV_OBS_RX_SEL:
            return "Invalid observation channel GPIO pin selection for Slicer control passed to MYKONOS_setObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE:
            return "Slicer step is out of range passed to MYKONOS_setObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_SLICER_OBS_RX_EN_INV:
            return "Invalid enable passed to MYKONOS_setObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM:
            return "ObsRxPins is null pointer for the passed parameter passed to MYKONOS_getObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM:
            return "SlicerStep is null pointer for the passed parameter passed to MYKONOS_getObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM:
            return "Enable is null pointer for the passed parameter passed to MYKONOS_getObsRxSlicerCtrl().\n";

        case MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT:
            return "Gain compensation structure gainComp is not initialised in MYKONOS_setObsRxGainCompensation().\n";

        case MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET:
            return "Gain compensation structure gainComp->obsRxOffset is invalid in MYKONOS_setObsRxGainCompensation().\n";

        case MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP:
            return "Gain compensation structure gainComp->compStep is invalid in MYKONOS_setObsRxGainCompensation().\n";

        case MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN:
            return "Enable is not valid in MYKONOS_setObsRxGainCompensation().\n";

        case MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT:
            return "Observation channel gain compensation structure gainComp is not initialised in MYKONOS_getObsRxGainCompensation().\n";

        case MYKONOS_ERR_GETGPIODRV_NULL_PARAM:
            return "Null parameter passed to the function MYKONOS_getGpioDrv().\n";

        case MYKONOS_ERR_GPIO_DRV_INV_PARAM:
            return "GPIO out of range passed to function MYKONOS_setGpioDrv(), valid GPIOs are in the range 0x00000 to 0x7FFFF.\n";

        case MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM:
            return "GPIO out of range -valid GPIOs are in the range 0x00000 to 0x7FFFF.\n";

        case MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM:
            return "Null parameter passed to the function MYKONOS_getGpioSlewRate().\n";

        case MYKONOS_ERR_CMOS_DRV_NULL_PARAM:
            return "Null parameter passed to the function.\n";

        case MYKONOS_ERR_CMOS_DRV_INV_PARAM:
            return "Incorrect drive strength, valid settings are given by mykonosCmosPadDrvStr_t.\n";

        case MYKONOS_ERR_SPI2_INV_GPIO:
            return "if an invalid GPIO pin configuration is passed to MYKONOS_spi2GpioSetup().\n";

        default:
            return "";
    }

 #endif
}

/**
 * \brief This API function configures the monitor output function for the GPIOs
 *
 * The monitor outputs are grouped in set of nibbles, the user can set
 * individual nibbles for having the monitor output function across the available GPIO.
 * In order to enable the GPIO monitor function the function setupGpio has to be run and the
 * structure should have the proper setup:
 * - device->auxIo->gpio->gpioOe = 0xXXXFF the first D7:D0 GPIOs will have the output enable
 * - device->auxIo->gpio->gpioSrcCtrl3_0 = GPIO_MONITOR_MODE
 * - device->auxIo->gpio->gpioSrcCtrl4_7 = GPIO_MONITOR_MODE

 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio->gpioOe
 * - device->auxIo->gpio->gpioSrcCtrl3_0
 * - device->auxIo->gpio->gpioSrcCtrl4_7
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \param monitorIndex which will be the index at which the outputs are going to be set. see table in documentation
 *
 * \param monitorMask which GPIO outputs are going to active, the available GPIO
 * bit 0 will represent MYKGPIO0 and bit 7 will represent MYKGPIO7.
 *
 * \retval MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE if GPIO Monitor output index is not in table
 * \retval MYKONOS_ERR_EN_MONITOR_OUT_NOT_ENABLED if GPIO source control not set for Monitor output
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioMonitorOut(mykonosDevice_t *device, uint8_t monitorIndex, uint8_t monitorMask)
{
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

    const uint8_t INDEX_MASK = 0x42;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioMonitorOut()\n");
#endif

    /* Error checking for correct index. */
    if (monitorIndex > INDEX_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE,
                getGpioMykonosErrorMessage(MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE));
        return MYKONOS_ERR_MONITOR_OUT_INDEX_RANGE;
    }

    /* Set the GPIO monitor index and the required pin configuration. */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_MONITOR_INDEX, monitorIndex);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_MONITOR_ENABLE, monitorMask);

    return error;
}

/**
 * \brief This API function reads the GPIO monitor index from Mykonos
 *
 * The monitor outputs are grouped in two set of nibbles, the user can set
 * individual nibbles for having the monitor output function, this nibbles
 * will output the monitor output as per the index set.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 * - GPIO output enable
 * - GPIO source control
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param monitorIndex which will be the index at which the outputs are set to.
 * \param monitorMask which will be enable the monitor function.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM if the monitorIndex is null.
 * \retval MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM if the monitorMask is null.
 */
mykonosGpioErr_t MYKONOS_getGpioMonitorOut(mykonosDevice_t *device, uint8_t *monitorIndex, uint8_t *monitorMask)
{
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

    uint8_t indexRd = 0;
    uint8_t monMaskRd = 0;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioMonitorOut()\n");
#endif

    /* Checking for null passed parameters */
    if (monitorIndex == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM));
        return MYKONOS_ERR_GETGPIOMON_INDEX_NULL_PARM;
    }

    if (monitorMask == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM));
        return MYKONOS_ERR_GETGPIOMON_MONITORMASK_NULL_PARM;
    }

    /* Get GPIO monitor out enable. */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_MONITOR_ENABLE, &monMaskRd);

    /* Get the GPIO monitor index and the required pin configuration. */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_MONITOR_INDEX, &indexRd);

    /* assigning return values to the pointers passed */
    *monitorMask = monMaskRd;
    *monitorIndex = indexRd;

    return error;
}




/**
 * \brief This API function configures the GPIO inputs for controlling RX gain
 *
 * This API function configures the GPIO input pin and step size to allow the BBP to control gain changes in Rx1 signal chain.
 * A high pulse on the 'rx1GainIncPin' in pin control mode will increment the gain by the value set in incStep'.
 * A high pulse on the 'rx1GainDecPin' in pin control mode will decrement the gain by the value set in 'decStep'.
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \param incStep This sets the change (increase) in gain index that is applied when the
 * increment gain pin  (in MGC pin control mode) is pulsed.
 *
 * \param decStep This configures the decrement in gain index that should be applied
 * when the decrement gain pin (in MGC pin control mode) is pulsed.
 *
 * \param rx1GainIncPin mykonosGpioSelect_t These bits select the GPIO used as the enable for
 * the Rx1 Manual Increment gain input according to the following:
 * MYKGPIO0 or MYKGPIO10
 *
 * \param rx1GainDecPin mykonosGpioSelect_t These bits select the GPIO used as the enable for
 * the Rx1 Manual Decrement gain input according to the following:
 * MYKGPIO1 or MYKGPIO11
 *
 * \param enable 0 = Disable the gain pin control for Rx1
 *               1 = enable the gain pin control for Rx1
 *
 * \retval MYKONOS_ERR_MGCRX1_STEP_INV_PARAM if an invalid step size has been passed
 * \retval MYKONOS_ERR_MGCRX1_GPIO_INCPIN_INV_PARAM if invalid increment pin has been passed
 * \retval MYKONOS_ERR_MGCRX1_GPIO_DECPIN_INV_PARAM if invalid decrement pin has been passed
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setRx1GainCtrlPin(mykonosDevice_t *device, uint8_t incStep, uint8_t decStep, mykonosGpioSelect_t rx1GainIncPin, mykonosGpioSelect_t rx1GainDecPin, uint8_t enable)
{
    uint8_t wrtPin = 0x00;
    uint8_t wrtStep = 0x00;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

    const uint8_t SHIFT_CH1 = 0x00;
    const uint8_t SHIFT_INC = 0x05;
    const uint8_t SHIFT_DEC = 0x02;
    const uint8_t MAX_STEP = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRx1GainCtrlPin()\n");
#endif

    /* If enable then check for the next otherwise go directly to disable */
    if (enable > 0)
    {
        /* Error checking for correct step. */
        if ((incStep > MAX_STEP) | (decStep > MAX_STEP))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_MGCRX1_STEP_INV_PARAM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_MGCRX1_STEP_INV_PARAM));
            return MYKONOS_ERR_MGCRX1_STEP_INV_PARAM;
        }

        /* Pin configuration assignment*/
        switch (rx1GainIncPin)
        {
            case MYKGPIO0:
                wrtPin |= 0x00;
                break;
            case MYKGPIO10:
                wrtPin |= 0x04;
                break;

            default:
                return MYKONOS_ERR_MGCRX1_GPIO_INCPIN_INV_PARAM;
        }

        switch (rx1GainDecPin)
        {
            case MYKGPIO1:
                wrtPin |= 0x00;
                break;
            case MYKGPIO11:
                wrtPin |= 0x01;
                break;

            default:
                return MYKONOS_ERR_MGCRX1_GPIO_DECPIN_INV_PARAM;
        }
    }

    /* Setting increment step. */
    wrtStep = (incStep<<SHIFT_INC) | (decStep<<SHIFT_DEC) | (enable<<SHIFT_CH1);

    /* Set the GPIO input pin configuration and the step size. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_CFG, wrtStep, 0xFD, 0);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_GPIO_SEL, wrtPin, 0x0F, 0);

    return error;
}

/**
 * \brief This API function returns the configuration RX1 gain Pin control
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param incStep will contain the step that is used for increment.
 * \param decStep will contain the step that is used for decrement.
 * \param rx1GainIncPin this will have the pin used for increment.
 * \param rx1GainDecPin this will have the pin used for decrement.
 * \param enable will contain the enable status for this channel if it is set to 1 then this
 * function is enable for this channel, if it is 0 it is not enable
 *
 * \retval MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM if a null value has been passed to incStep
 * \retval MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM if a null value has been passed to decStep
 * \retval MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM if a null value has been passed to rx1GainIncPin
 * \retval MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM if a null value has been passed to rx1GainDecPin
 * \retval MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM if a null value has been passed to enable
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getRx1GainCtrlPin(mykonosDevice_t *device, uint8_t *incStep, uint8_t *decStep, mykonosGpioSelect_t *rx1GainIncPin, mykonosGpioSelect_t *rx1GainDecPin, uint8_t *enable)
{
    uint8_t readVal = 0x00;

    const uint8_t MASK_GPIO_CH1 = 0x0F;
    const uint8_t MASK_EN_CH1 = 0x01;
    const uint8_t MASK_STEP_INC = 0xE0;
    const uint8_t MASK_STEP_DEC = 0x1C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRx1GainCtrlPin()\n");
#endif

    /* Checking for null passed parameters */
    if (incStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM));
        return MYKONOS_ERR_GETRX1PIN_INCSTEP_NULL_PARM;
    }

    if (decStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM));
        return MYKONOS_ERR_GETRX1PIN_DECSTEP_NULL_PARM;
    }

    if (rx1GainIncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM));
        return MYKONOS_ERR_GETRX1PIN_INCPIN_NULL_PARM;
    }

    if (rx1GainDecPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM));
        return MYKONOS_ERR_GETRX1PIN_DECPIN_NULL_PARM;
    }

    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM,
               getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM));
        return MYKONOS_ERR_GETRX1PIN_EN_NULL_PARM;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_GPIO_SEL, &readVal);
    readVal = readVal & MASK_GPIO_CH1;

    if (readVal & 0x04)
    {
        *rx1GainIncPin = MYKGPIO10;
    }
    else
    {
        *rx1GainIncPin = MYKGPIO0;
    }

    if (readVal & 0x01)
    {
        *rx1GainDecPin = MYKGPIO11;
    }
    else
    {
        *rx1GainDecPin = MYKGPIO1;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_CFG, &readVal);

    *enable = readVal & MASK_EN_CH1;

    *incStep = (readVal & MASK_STEP_INC) >> 5;

    *decStep = (readVal & MASK_STEP_DEC) >> 2;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function configures the GPIO inputs for controlling RX gain
 *
 * This API function configures the GPIO input pin and step size to allow the BBP to control gain changes in Rx2 signal chain.
 * A high pulse on the 'rx2GainIncPin' in pin control mode will increment the gain by the value set in incStep'.
 * A high pulse on the 'rx2GainDecPin' in pin control mode will decrement the gain by the value set in 'decStep'.
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \param incStep This sets the change (increase) in gain index that is applied when the
 * increment gain pin (in MGC pin control mode) is pulsed.
 *
 * \param decStep This configures the decrement in gain index that should be applied
 * when the decrement gain pin (in MGC pin control mode) is pulsed.
 *
 * \param rx2GainIncPin mykonosGpioSelect_t These bits select the GPIO used as the enable for
 * the Rx2 Manual Increment gain input according to the following:
 * MYKGPIO3 or MYKGPIO13
 *
 * \param rx2GainDecPin mykonosGpioSelect_t These bits select the GPIO used as the enable for
 * the Rx2 Manual Decrement gain input according to the following:
 * MYKGPIO4 or MYKGPIO14
 *
 * \param enable 0 = Disable the gain pin control for Rx2
 *               1 = enable the gain pin control for Rx2
 *
 *
 * \retval MYKONOS_ERR_MGCRX2_STEP_INV_PARAM if an invalid step size is passed
 * \retval MYKONOS_ERR_MGCRX2_GPIO_INCPIN_INV_PARAM if invalid increment pin has been passed
 * \retval MYKONOS_ERR_MGCRX2_GPIO_DECPIN_INV_PARAM if invalid decrement pin has been passed
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setRx2GainCtrlPin(mykonosDevice_t *device, uint8_t incStep, uint8_t decStep, mykonosGpioSelect_t rx2GainIncPin, mykonosGpioSelect_t rx2GainDecPin, uint8_t enable)
{
    uint8_t wrtPin = 0x00;
    uint8_t wrtStep = 0x00;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

    const uint8_t SHIFT_CH2 = 0x01;
    const uint8_t SHIFT_INC = 0x5;
    const uint8_t SHIFT_DEC = 0x2;
    const uint8_t MAX_STEP = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRx2GainCtrlPin()\n");
#endif

    /* If enable then check for the next otherwise go directly to disable */
    if (enable > 0)
    {
        /* Error checking for correct step. */
        if ((incStep > MAX_STEP) | (decStep > MAX_STEP))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_MGCRX2_STEP_INV_PARAM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_MGCRX2_STEP_INV_PARAM));
            return MYKONOS_ERR_MGCRX2_STEP_INV_PARAM;
        }

        /* Pin configuration assignment*/
        switch (rx2GainIncPin)
        {
            case MYKGPIO3:
                wrtPin |= 0x00;
                break;
            case MYKGPIO13:
                wrtPin |= 0x40;
                break;

            default:
                return MYKONOS_ERR_MGCRX2_GPIO_INCPIN_INV_PARAM;
        }

        switch (rx2GainDecPin)
        {
            case MYKGPIO4:
                wrtPin |= 0x00;
                break;
            case MYKGPIO14:
                wrtPin |= 0x10;
                break;

            default:
                return MYKONOS_ERR_MGCRX2_GPIO_DECPIN_INV_PARAM;
        }
    }

    /* Setting increment step. */
    wrtStep = (incStep<<SHIFT_INC) | (decStep<<SHIFT_DEC) | (enable<<SHIFT_CH2);

    /* Set the GPIO input pin configuration and the step size. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_CFG, wrtStep, 0xFE, 0);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_GPIO_SEL, wrtPin, 0xF0, 0);

    return error;
}

/**
 * \brief This API function returns the configuration RX2 gain Pin control
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param incStep will contain the step that is used for increment.
 * \param decStep will contain the step that is used for decrement.
 * \param rx2GainIncPin this will have the pin used for increment.
 * \param rx2GainDecPin this will have the pin used for decrement.
 * \param enable will contain the enable status for this channel if it is set to 1 then this
 * function is enable for this channel, if it is 0 it is not enable
 *
 * \retval MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM if a null value has been passed to incStep
 * \retval MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM if a null value has been passed to decStep
 * \retval MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM if a null value has been passed to rx1GainIncPin
 * \retval MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM if a null value has been passed to rx1GainDecPin
 * \retval MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM if a null value has been passed to enable
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getRx2GainCtrlPin(mykonosDevice_t *device, uint8_t *incStep, uint8_t *decStep, mykonosGpioSelect_t *rx2GainIncPin, mykonosGpioSelect_t *rx2GainDecPin, uint8_t *enable)
{
    uint8_t readVal = 0x00;

    const uint8_t MASK_GPIO_CH2 = 0xF0;
    const uint8_t MASK_EN_CH2 = 0x02;
    const uint8_t MASK_STEP_INC = 0xE0;
    const uint8_t MASK_STEP_DEC = 0x1C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRx2GainCtrlPin()\n");
#endif

    /* Checking for null passed parameters */
    if (incStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM));
        return MYKONOS_ERR_GETRX2PIN_INCSTEP_NULL_PARM;
    }

    if (decStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM));
        return MYKONOS_ERR_GETRX2PIN_DECSTEP_NULL_PARM;
    }

    if (rx2GainIncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM));
        return MYKONOS_ERR_GETRX2PIN_INCPIN_NULL_PARM;
    }

    if (rx2GainDecPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM));
        return MYKONOS_ERR_GETRX2PIN_DECPIN_NULL_PARM;
    }

    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM,
               getGpioMykonosErrorMessage(MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM));
        return MYKONOS_ERR_GETRX2PIN_EN_NULL_PARM;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_GPIO_SEL, &readVal);
    readVal = readVal & MASK_GPIO_CH2;

    if (readVal & 0x40)
    {
        *rx2GainIncPin = MYKGPIO13;
    }
    else
    {
        *rx2GainIncPin = MYKGPIO3;
    }

    if (readVal & 0x10)
    {
        *rx2GainDecPin = MYKGPIO14;
    }
    else
    {
        *rx2GainDecPin = MYKGPIO4;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_MANUAL_GAIN_CFG, &readVal);

    *enable = (readVal & MASK_EN_CH2) >> 1;

    *incStep = (readVal & MASK_STEP_INC) >> 5;

    *decStep = (readVal & MASK_STEP_DEC) >> 2;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function set the pins for hybrid gain control
 *
 * In order to call this function the gain mode should be set to Hybrid.
 * The AGC gain change will be controlled with the selected GPIO pin:
 * A pulse on the 'rx1GainChangePin' in hybrid pin control will enable the AGC gain change for RX1
 * A pulse on the 'rx2GainChangePin' in hybrid pin control will enable the AGC gain change for RX2
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param rx1GainChangePin GPIO pin that will be used for hybrid gain change control, the available pins for RX1 channel
 * hybrid control are:
 * MYKGPIO1
 * MYKGPIO10
 * MYKGPIO11
 * MYKGPIONAN for no GPIO selected
 *
 * \param rx2GainChangePin GPIO pin that will be used for hybrid gain change control, the available pins for RX2 channel
 * hybrid control are:
 * MYKGPIO4
 * MYKGPIO10
 * MYKGPIO13
 * MYKGPIONAN for no GPIO selected
 *
 * \retval MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID if gain control is not hybrid
 * \retval MYKONOS_ERR_GPIO_HYBRID_RX1_PIN if invalid pin has been passed for RX1 hybrid gain control.
 * \retval MYKONOS_ERR_GPIO_HYBRID_RX2_PIN if invalid pin has been passed for RX2 hybrid gain control.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t rx1GainChangePin, mykonosGpioSelect_t rx2GainChangePin)
{
    uint8_t hybridPinWrite = 0x00;
    uint8_t pinRx1 = 0x00;
    uint8_t pinRx2 = 0x00;

    const uint8_t HYBRID_RX_PIN_MASK = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRxHybridGainChangePin()\n");
#endif

    /* Check if in Hybrid Mode */
    if (device->rx->rxGainCtrl->gainMode != HYBRID)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID));
        return MYKONOS_ERR_GAIN_CONTROL_NOT_HYBRID;
    }

    /* Pin configuration assignment for RX1 and RX2 */
    switch (rx1GainChangePin)
    {
        case MYKGPIO1:
            pinRx1 |= 0x00;
            break;
        case MYKGPIO10:
            pinRx1 |= 0x01;
            break;
        case MYKGPIO11:
            pinRx1 |= 0x02;
            break;
        case MYKGPIONAN:
            pinRx1 |= 0x03;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_RX1_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_RX1_PIN));
            return MYKONOS_ERR_GPIO_HYBRID_RX1_PIN;
    }

    switch (rx2GainChangePin)
    {
        case MYKGPIO4:
            pinRx2 |= 0x00;
            break;
        case MYKGPIO10:
            pinRx2 |= 0x04;
            break;
        case MYKGPIO13:
            pinRx2 |= 0x08;
            break;
        case MYKGPIONAN:
            pinRx2 |= 0x0C;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_RX2_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_RX2_PIN));
            return MYKONOS_ERR_GPIO_HYBRID_RX2_PIN;
    }

    /* Setting GPIO control for RX1 and RX2 */
    hybridPinWrite = (pinRx1 | pinRx2);

    /* Writing GPIO pin configuration for hybrid mode. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_GAIN_CHANGE_GPIO_SEL, hybridPinWrite, HYBRID_RX_PIN_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}



/**
 * \brief This API function gets the pin configuration for hybrid gain control of RX1 and RX2
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param rx1GainChangePin Will return the GPIO pin used for Rx1 in hybrid gain control.
 * \param rx2GainChangePin Will return the GPIO pin used for Rx2 in hybrid gain control.
 *
 * \retval MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM if a null value has been passed to rx1GainChangePin
 * \retval MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM if a null value has been passed to rx2GainChangePin
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t *rx1GainChangePin, mykonosGpioSelect_t *rx2GainChangePin)
{
    uint8_t hybridPinRead = 0x00;
    uint8_t pinRx1 = 0x00;
    uint8_t pinRx2 = 0x00;
    mykonosGpioSelect_t rx1Pin = MYKGPIONAN;
    mykonosGpioSelect_t rx2Pin = MYKGPIONAN;

    const uint8_t MASK_RX1_PIN = 0x03;
    const uint8_t MASK_RX2_PIN = 0x0C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRxHybridGainChangePin()\n");
#endif

    /* Checking for null passed parameters */
    if (rx1GainChangePin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_HYBRID_RX1_PIN_NULL_PARM;
    }

    if (rx2GainChangePin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_HYBRID_RX2_PIN_NULL_PARM;
    }

    /* Getting Pin configuration assignment */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_GAIN_CHANGE_GPIO_SEL, &hybridPinRead);
    pinRx1 = hybridPinRead & MASK_RX1_PIN;
    pinRx2 = hybridPinRead & MASK_RX2_PIN;

    /* Pin configuration mapping assignment for RX1 and RX2 */
    switch (pinRx1)
    {
        case 0x00:
            rx1Pin = MYKGPIO1;
            break;
        case 0x01:
            rx1Pin = MYKGPIO10;
            break;
        case 0x02:
            rx1Pin = MYKGPIO11;
            break;
        default:
            rx1Pin = MYKGPIONAN;
            break;
    }

    switch (pinRx2)
    {
        case 0x00:
            rx2Pin = MYKGPIO4;
            break;
        case 0x04:
            rx2Pin = MYKGPIO10;
            break;
        case 0x08:
            rx2Pin = MYKGPIO13;
            break;
        default:
            rx2Pin = MYKGPIONAN;
            break;
    }

    /* Setting pins to passed pointers */
    *rx1GainChangePin = rx1Pin;
    *rx2GainChangePin = rx2Pin;

    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief This API function set the pins for hybrid gain control
 *
 * In order to call this function the gain mode should be set to Hybrid.
 * The AGC gain change will be controlled with the selected GPIO pin:
 * A pulse on the 'obsRxGainChangePin' in hybrid pin control will enable the AGC gain change for observation channel
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxGainChangePin GPIO pin that will be used for hybrid gain change control, the available pins for observation channel
 * hybrid control are:
 * MYKGPIO6
 * MYKGPIO10
 * MYKGPIO17
 * MYKGPIONAN for none selected
 *
 * \retval MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID if the observation gain mode is not set to Hybrid
 * \retval MYKONOS_ERR_GPIO_HYBRID_ORX_PIN if invalid pin has been passed for ORX hybrid gain control.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setObsRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t obsRxGainChangePin)
{
    uint8_t hybridPinWrite = 0x00;
    uint8_t pinOrx = 0x00;

    const uint8_t HYBRID_ORX_PIN_MASK = 0x30;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setObsRxHybridGainChangePin()\n");
#endif

    /* Check if in Hybrid Mode */
    if (device->obsRx->orxGainCtrl->gainMode != HYBRID)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID,
                getGpioMykonosErrorMessage(MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID));
        return MYKONOS_ERR_AGC_OBS_NOT_IN_HYBRID;
    }

    /* Pin configuration assignment for RX1 and RX2 */
    switch (obsRxGainChangePin)
    {
        case MYKGPIO6:
            pinOrx |= 0x00;
            break;
        case MYKGPIO10:
            pinOrx |= 0x10;
            break;
        case MYKGPIO17:
            pinOrx |= 0x20;
            break;
        case MYKGPIONAN:
            pinOrx |= 0x30;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_ORX_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_ORX_PIN));
            return MYKONOS_ERR_GPIO_HYBRID_ORX_PIN;
    }

    /* Setting GPIO control for observation channel */
    hybridPinWrite = pinOrx;

    /* Writing GPIO pin configuration for hybrid mode. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_GPIO_SEL, hybridPinWrite, HYBRID_ORX_PIN_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function gets the pin configuration for hybrid gain control of observation channel
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxGainChangePin Will return the GPIO pin used for observation channel in hybrid gain control.
 *
 * \retval MYKONOS_ERR_GPIO_HYBRID_ORX_PIN_NULL_PARM if a null value has been passed to obsRxGainChangePin
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getObsRxHybridGainChangePin(mykonosDevice_t *device, mykonosGpioSelect_t *obsRxGainChangePin)
{
    uint8_t hybridPinRead = 0x00;
    uint8_t pinOrx = 0x00;
    mykonosGpioSelect_t orxPin = MYKGPIONAN;

    const uint8_t MASK_ORX_PIN = 0x30;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getObsRxHybridGainChangePin()\n");
#endif

    /* Checking for null passed parameters */
    if (obsRxGainChangePin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_HYBRID_ORX_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_HYBRID_ORX_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_HYBRID_ORX_PIN_NULL_PARM;
    }

    /* Getting Pin configuration assignment */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_GPIO_SEL, &hybridPinRead);
    pinOrx = hybridPinRead & MASK_ORX_PIN;

    /* Pin configuration mapping assignment for RX1 and RX2 */
    switch (pinOrx)
    {
        case 0x00:
            orxPin = MYKGPIO6;
            break;
        case 0x10:
            orxPin = MYKGPIO10;
            break;
        case 0x20:
            orxPin = MYKGPIO17;
            break;
        default:
            orxPin = MYKGPIONAN;
            break;
    }

    /* Setting pins to passed pointer */
    *obsRxGainChangePin = orxPin;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function set the pins for sync AGC gain control
 *
 * In order to call this function the RX gain control should be set to AGC mode.
 * The AGC gain sync will be controlled with the selected GPIO pin:
 * A pulse on the 'rx1AgcSyncPin' in hybrid pin control will enable the AGC gain sync for RX1
 * A pulse on the 'rx2AgcSyncPin' in hybrid pin control will enable the AGC gain sync for RX2
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param rx1AgcSyncPin GPIO pin that will be used for AGC sync gain control, the available pins for RX1 channel are:
 * MYKGPIO1
 * MYKGPIO10
 * MYKGPIO11
 * MYKGPIONAN for no GPIO selected
 *
 * \param rx2AgcSyncPin GPIO pin that will be used for AGC sync gain control, the available pins for RX2 channel are:
 * MYKGPIO4
 * MYKGPIO10
 * MYKGPIO13
 * MYKGPIONAN for no GPIO selected
 *
 * \retval MYKONOS_ERR_GAIN_CONTROL_NOT_AGC if the RX gain control mode is not set to AGC.
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN if invalid pin has been passed for RX1 AGC sync gain control.
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN if invalid pin has been passed for RX2 AGC sync gain control.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t rx1AgcSyncPin, mykonosGpioSelect_t rx2AgcSyncPin)
{
    uint8_t agcSyncPinWrite = 0x00;
    uint8_t pinRx1 = 0x00;
    uint8_t pinRx2 = 0x00;

    const uint8_t SYNC_RX_PIN_MASK = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRxAgcEnSyncPin()\n");
#endif

    /* Check if in Hybrid Mode */
    if (device->rx->rxGainCtrl->gainMode != AGC)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAIN_CONTROL_NOT_AGC,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GAIN_CONTROL_NOT_AGC));
        return MYKONOS_ERR_GAIN_CONTROL_NOT_AGC;
    }

    /* Pin configuration assignment for RX1 and RX2 */
    switch (rx1AgcSyncPin)
    {
        case MYKGPIO1:
            pinRx1 |= 0x00;
            break;
        case MYKGPIO10:
            pinRx1 |= 0x01;
            break;
        case MYKGPIO11:
            pinRx1 |= 0x02;
            break;
        case MYKGPIONAN:
            pinRx1 |= 0x03;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN));
            return MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN;
    }

    switch (rx2AgcSyncPin)
    {
        case MYKGPIO4:
            pinRx2 |= 0x00;
            break;
        case MYKGPIO10:
            pinRx2 |= 0x04;
            break;
        case MYKGPIO13:
            pinRx2 |= 0x08;
            break;
        case MYKGPIONAN:
            pinRx2 |= 0x0C;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN));
            return MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN;
    }

    /* Setting GPIO control for RX1 and RX2 */
    agcSyncPinWrite = (pinRx1 | pinRx2);

    /* Writing GPIO pin configuration for Sync AGC. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_GAIN_CHANGE_GPIO_SEL, agcSyncPinWrite, SYNC_RX_PIN_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function gets the GPIO pin configuration for AGC gain sync control of RX1 and RX2
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param rx1AgcSyncPin Will return the GPIO pin used for RX1 channel AGC gain sync control,
 *
 * \param rx2AgcSyncPin Will return the GPIO pin used for RX2 channel AGC gain sync control,
 *
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM if a null value has been passed to rx1AgcSyncPin
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM if a null value has been passed to rx2AgcSyncPin
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t *rx1AgcSyncPin, mykonosGpioSelect_t *rx2AgcSyncPin)
{
    uint8_t agcSyncPinRead = 0x00;
    uint8_t pinRx1 = 0x00;
    uint8_t pinRx2 = 0x00;
    mykonosGpioSelect_t rx1Pin = MYKGPIONAN;
    mykonosGpioSelect_t rx2Pin = MYKGPIONAN;

    const uint8_t MASK_RX1_PIN = 0x03;
    const uint8_t MASK_RX2_PIN = 0x0C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRxHybridGainChangePin()\n");
#endif

    /* Checking for null passed parameters */
    if (rx1AgcSyncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_AGC_SYNC_RX1_PIN_NULL_PARM;
    }

    if (rx2AgcSyncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_AGC_SYNC_RX2_PIN_NULL_PARM;
    }

    /* Getting Pin configuration assignment */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_GAIN_CHANGE_GPIO_SEL, &agcSyncPinRead);
    pinRx1 = agcSyncPinRead & MASK_RX1_PIN;
    pinRx2 = agcSyncPinRead & MASK_RX2_PIN;

    /* Pin configuration mapping assignment for RX1 and RX2 */
    switch (pinRx1)
    {
        case 0x00:
            rx1Pin = MYKGPIO1;
            break;
        case 0x01:
            rx1Pin = MYKGPIO10;
            break;
        case 0x02:
            rx1Pin = MYKGPIO11;
            break;
        default:
            rx1Pin = MYKGPIONAN;
            break;
    }

    switch (pinRx2)
    {
        case 0x00:
            rx2Pin = MYKGPIO4;
            break;
        case 0x04:
            rx2Pin = MYKGPIO10;
            break;
        case 0x08:
            rx2Pin = MYKGPIO13;
            break;
        default:
            rx2Pin = MYKGPIONAN;
            break;
    }

    /* Setting pins to passed pointers */
    *rx1AgcSyncPin = rx1Pin;
    *rx2AgcSyncPin = rx2Pin;

    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief This API function set the pins for AGC gain Sync control
 *
 * In order to call this function the gain mode should be set to AGC mode.
 * The AGC gain sync will be controlled with the selected GPIO pin:
 * A pulse on the 'obsRxGainChangePin' in hybrid pin control will enable the AGC gain change for observation channel
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxAgcSyncPin GPIO pin that will be used for AGC sync gain control, the available pins for observation channel are:
 * MYKGPIO6
 * MYKGPIO10
 * MYKGPIO17
 * MYKGPIONAN for none selected
 *
 * \retval MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC if the observation gain control mode is not set to AGC
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN  if invalid pin has been passed for ORX AGC sync gain control.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setObsRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t obsRxAgcSyncPin)
{
    uint8_t agcSyncPinWrite = 0x00;
    uint8_t pinOrx = 0x00;

    const uint8_t SYNC_ORX_PIN_MASK = 0x30;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setObsRxAgcEnSyncPin()\n");
#endif

    /* Check if in Hybrid Mode */
    if (device->obsRx->orxGainCtrl->gainMode != AGC)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC,
                getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC));
        return MYKONOS_ERR_OBS_GAIN_CONTROL_NOT_AGC;
    }

    /* Pin configuration assignment for observation channel */
    switch (obsRxAgcSyncPin)
    {
        case MYKGPIO6:
            pinOrx |= 0x00;
            break;
        case MYKGPIO10:
            pinOrx |= 0x10;
            break;
        case MYKGPIO17:
            pinOrx |= 0x20;
            break;
        case MYKGPIONAN:
            pinOrx |= 0x30;
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN));
            return MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN;
    }

    /* Setting GPIO control for observation channel */
    agcSyncPinWrite = pinOrx;

    /* Writing GPIO pin configuration for Sync AGC mode. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_GPIO_SEL, agcSyncPinWrite, SYNC_ORX_PIN_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function gets the pin configuration for AGC Sync gain control of observation channel
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param obsRxAgcSyncPin Will return the GPIO pin used for observation channel AGC gain sync control,
 *
 * \retval MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM if a null value has been passed to obsRxAgcSyncPin
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getObsRxAgcEnSyncPin(mykonosDevice_t *device, mykonosGpioSelect_t *obsRxAgcSyncPin)
{
    uint8_t agcSyncPinRead = 0x00;
    uint8_t pinOrx = 0x00;
    mykonosGpioSelect_t orxPin = MYKGPIONAN;

    const uint8_t MASK_ORX_PIN = 0x30;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getObsRxAgcEnSyncPin()\n");
#endif

    /* Checking for null passed parameters */
    if (obsRxAgcSyncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM));
        return MYKONOS_ERR_GPIO_AGC_SYNC_ORX_PIN_NULL_PARM;
    }

    /* Getting Pin configuration assignment */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AGC_ORX_SNRX_GPIO_SEL, &agcSyncPinRead);
    pinOrx = agcSyncPinRead & MASK_ORX_PIN;

    /* Pin configuration mapping assignment for observation channel */
    switch (pinOrx)
    {
        case 0x00:
            orxPin = MYKGPIO6;
            break;
        case 0x10:
            orxPin = MYKGPIO10;
            break;
        case 0x20:
            orxPin = MYKGPIO17;
            break;
        default:
            orxPin = MYKGPIONAN;
            break;
    }

    /* Setting pins to passed pointer */
    *obsRxAgcSyncPin = orxPin;

    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief This API function configures the GPIO inputs for controlling Tx attenuation settings
 *
 * This allows to control the TX attenuation using GPIO inputs. When a low to high transition is
 * applied to the configure GPIO input the attenuation will change by the desire step.
 * The stepSize parameter will set the attenuation change applied.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \param stepSize the step that will increase or decrease the Tx1 channel attenuation.
 * This parameter sets the change in Tx attenuation for each increment or decrement signal received in incr/decr mode.
 * Step of 1 changes attenuation by 0.05dB.
 *
 * \param tx1AttenIncPin This parameter is the GPIO pin configuration that will be controlling the increment
 * of Tx attenuation. Available pins are:
 * Tx1 increment: MYKGPIO4 or MYKGPIO12
 *
 * \param tx1AttenDecPin This parameter is the GPIO pin configuration that will be controlling the decrement
 * of Tx attenuation. Available pins are:
 * Tx1 decrement: MYKGPIO5 or MYKGPIO13
 *
 * \param enable 0 = Disable the attenuation pin control for Tx1
 *               1 = enable the attenuation pin control for Tx1
 *
 * \param useTx1ForTx2 is used to return if TX1 settings are used for TX2 channel.
 *
 * \retval MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM if an invalid step size is passed
 * \retval MYKONOS_ERR_TPCTX1_GPIO_INCPIN_INV_PARAM if an invalid channel for TX is passed
 * \retval MYKONOS_ERR_TPCTX1_GPIO_DECPIN_INV_PARAM if an invalid channel for TX is passed
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setTx1AttenCtrlPin(mykonosDevice_t *device, uint8_t stepSize, mykonosGpioSelect_t tx1AttenIncPin, mykonosGpioSelect_t tx1AttenDecPin, uint8_t enable, uint8_t useTx1ForTx2)
{
    uint8_t wrtPin = 0;
    uint8_t tpcMode = 0x0;
    uint8_t tpcMaskTx2 = 0x00;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

    const uint8_t TX_INCDEC_MASK = 0x1F;
    const uint8_t TX_PIN_MASK = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setTx1AttenCtrlPin()\n");
#endif

    /* If enable then check for the next otherwise go directly to disable */
    if(enable > 0)
    {
        /* Error checking for correct step. */
        if (stepSize >  TX_INCDEC_MASK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM));
            return MYKONOS_ERR_TPCTX1_GPIO_STEP_INV_PARAM;
        }

        /* Pin configuration assignment for Tx1 increments */
        switch (tx1AttenIncPin)
        {
            case MYKGPIO4:
                wrtPin |= 0x00;
                break;
            case MYKGPIO12:
                wrtPin |= 0x01;
                break;

            default:
                return MYKONOS_ERR_TPCTX1_GPIO_INCPIN_INV_PARAM;
        }

        /* Pin configuration assignment for Tx1 decrements */
        switch (tx1AttenDecPin)
        {
            case MYKGPIO5:
                wrtPin |= 0x00;
                break;
            case MYKGPIO13:
                wrtPin |= 0x04;
                break;

            default:
                return MYKONOS_ERR_TPCTX1_GPIO_DECPIN_INV_PARAM;
        }

        /* Setting TPC mode corresponding to the enable */
        tpcMode = 0x03;

        /* Setting TPC control for Tx2 using Tx1 */
        if (useTx1ForTx2 > 0)
        {
            tpcMode |= 0x1C;
            tpcMaskTx2 = 0x1F;
        }
        else
        {
            tpcMaskTx2 = 0x13;
        }
    }
    else
    {
        /* Setting TPC mode corresponding for no Pin control */
        tpcMode = 0x05;
        tpcMaskTx2 = 0x1F;
    }

    /* Setting increment step. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_INCR_DECR_WORD, stepSize, TX_INCDEC_MASK, 0);

    /* Set the TPC mode for GPIO control. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, tpcMode, tpcMaskTx2, 0);

    /* Set the GPIO input pin configuration and the step size. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_GPIO_CFG, wrtPin, TX_PIN_MASK, 0);

    return error;
}

/**
 * \brief This API function returns the configuration TX1 attenuation Pin control
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param stepSize will contain the step that is used for increment and decrement.
 * \param tx1AttenIncPin this will have the pin used for increment.
 * \param tx1AttenDecPin this will have the pin used for decrement.
 * \param enable will contain the enable status for this channel if it is set to 1 then this
 * function is enable for this channel, if it is 0 it is not enable
 * \param useTx1ForTx2 is used to return if TX1 settings are used for TX2 channel.
 *
 * \retval MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM if a null value has been passed to stepSize
 * \retval MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM if a null value has been passed to tx1AttenIncPin
 * \retval MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM if a null value has been passed to tx1AttenDecPin
 * \retval MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM if a null value has been passed to enable
 * \retval MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM if a null value has been passed to useTx1ForTx2
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getTx1AttenCtrlPin(mykonosDevice_t *device, uint8_t *stepSize, mykonosGpioSelect_t *tx1AttenIncPin, mykonosGpioSelect_t *tx1AttenDecPin, uint8_t *enable, uint8_t *useTx1ForTx2)
{
    uint8_t readVal = 0x00;
    uint8_t en = 0x00;

    const uint8_t MASK_TPC_CH1 = 0x03;
    const uint8_t MASK_TPC_CH1FCH2 = 0x10;
    const uint8_t MASK_TPC_STEP = 0x1F;
    const uint8_t MASK_TPC_GPIO_CH1 = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getTx1AttenCtrlPin()\n");
#endif

    /* Checking for null passed parameters */
    if (stepSize == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM));
        return MYKONOS_ERR_GETTX1PIN_STEP_NULL_PARM;
    }

    if (tx1AttenIncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM));
        return MYKONOS_ERR_GETTX1PIN_INC_NULL_PARM;
    }

    if (tx1AttenDecPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM));
        return MYKONOS_ERR_GETTX1PIN_DEC_NULL_PARM;
    }

    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM));
        return MYKONOS_ERR_GETTX1PIN_EN_NULL_PARM;
    }

    if (useTx1ForTx2 == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM,
               getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM));
        return MYKONOS_ERR_GETTX1PIN_TX1TX2_NULL_PARM;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_TPC_GPIO_CFG, &readVal);
    readVal = readVal & MASK_TPC_GPIO_CH1;

    if (readVal & 0x01)
    {
        *tx1AttenIncPin = MYKGPIO12;
    }
    else
    {
        *tx1AttenIncPin = MYKGPIO4;
    }

    if (readVal & 0x04)
    {
        *tx1AttenDecPin = MYKGPIO13;
    }
    else
    {
        *tx1AttenDecPin = MYKGPIO5;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, &readVal);

    en = readVal & MASK_TPC_CH1;

    *enable = (en == MASK_TPC_CH1) ? 1 : 0;

    *useTx1ForTx2 = readVal & MASK_TPC_CH1FCH2;

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_INCR_DECR_WORD, &readVal);

    *stepSize = readVal & MASK_TPC_STEP;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This API function configures the GPIO inputs for controlling Tx attenuation settings
 *
 * This allows to control the TX attenuation using GPIO inputs. when a low to high transition is
 * applied to the configure GPIO input the attenuation will change by the desire step.
 * The stepSize parameter will set the attenuation change applied.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 *
 * \param stepSize the step that will increase or decrease the Tx2 channel attenuation.
 * This parameter sets the change in Tx attenuation for each increment or decrement signal received in incr/decr mode.
 * Step of 1 changes attenuation by 0.05dB.
 *
 * \param tx2AttenIncPin This parameter is the GPIO pin configuration that will be controlling the increment
 * of Tx attenuation. Available pins are:
 * Tx2 increment: MYKGPIO6 or MYKGPIO14
 *
 * \param tx2AttenDecPin This parameter is the GPIO pin configuration that will be controlling the decrement
 * of Tx attenuation. Available pins are:
 * Tx2 decrement: MYKGPIO7 or MYKGPIO15
 *
 * \param enable 0 = Disable the attenuation pin control for Tx2
 *               1 = enable the attenuation pin control for Tx2
 *
 * \retval MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM if an invalid step size is passed
 * \retval MYKONOS_ERR_TPCTX2_GPIO_INCPIN_INV_PARAM if an invalid channel for TX is passed
 * \retval MYKONOS_ERR_TPCTX2_GPIO_INCPIN_INV_PARAM if an invalid channel for TX is passed
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setTx2AttenCtrlPin(mykonosDevice_t *device, uint8_t stepSize, mykonosGpioSelect_t tx2AttenIncPin, mykonosGpioSelect_t tx2AttenDecPin, uint8_t enable)
{
    uint8_t wrtPin = 0;
    uint8_t tpcMode = 0x0;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;


    const uint8_t TX_INCDEC_MASK = 0x1F;
    const uint8_t TX2_TPC_MASK = 0x1C;
    const uint8_t TX2_WRTPIN = 0xf0;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setTx2AttenCtrlPin()\n");
#endif

    /* If enable then check for the next otherwise go directly to disable */
    if(enable > 0)
    {
        /* Error checking for correct step. */
        if (stepSize >  TX_INCDEC_MASK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM));
            return MYKONOS_ERR_TPCTX2_GPIO_STEP_INV_PARAM;
        }

        /* Pin configuration assignment for Tx2 increments */
        switch (tx2AttenIncPin)
        {
            case MYKGPIO6:
                wrtPin |= 0x00;
                break;
            case MYKGPIO14:
                wrtPin |= 0x10;
                break;

            default:
                return MYKONOS_ERR_TPCTX2_GPIO_INCPIN_INV_PARAM;
        }

        /* Pin configuration assignment for Tx2 decrements */
        switch (tx2AttenDecPin)
        {
            case MYKGPIO7:
                wrtPin |= 0x00;
                break;
            case MYKGPIO15:
                wrtPin |= 0x40;
                break;

            default:
                return MYKONOS_ERR_TPCTX2_GPIO_DECPIN_INV_PARAM;
        }

        /* Setting TPC mode corresponding to the enable */
        tpcMode = 0x0C;
    }
    else
    {
          /* Setting TPC mode corresponding for no Pin control */
          tpcMode = 0x05;
    }

    /* Setting increment step. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_INCR_DECR_WORD, stepSize, TX_INCDEC_MASK, 0);

    /* Set the TPC mode for GPIO control. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, tpcMode, TX2_TPC_MASK, 0);

    /* Set the GPIO input pin configuration and the step size. */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_TX_TPC_GPIO_CFG, wrtPin, TX2_WRTPIN, 0);

    return error;
}

/**
 * \brief This API function returns the configuration TX2 attenuation Pin control
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 * - device->spiSettings->chipSelectIndex

 *
 * \param device is structure pointer to the Mykonos data structure containing settings
 * \param stepSize will contain the step that is used for increment and decrement.
 * \param tx2AttenIncPin this will have the pin used for increment.
 * \param tx2AttenDecPin this will have the pin used for decrement.
 * \param enable will contain the enable status for this channel if it is set to 1 then this
 * function is enable for this channel, if it is 0 it is not enable
 * \param useTx1ForTx2 is used to return if TX1 settings are used for TX2 channel.
 *
 * \retval MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM if a null value has been passed to stepSize
 * \retval MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM if a null value has been passed to tx1AttenIncPin
 * \retval MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM if a null value has been passed to tx1AttenDecPin
 * \retval MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM if a null value has been passed to enable
 * \retval MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM if a null value has been passed to useTx1ForTx2
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getTx2AttenCtrlPin(mykonosDevice_t *device, uint8_t *stepSize, mykonosGpioSelect_t *tx2AttenIncPin, mykonosGpioSelect_t *tx2AttenDecPin, uint8_t *enable, uint8_t *useTx1ForTx2)
{
    uint8_t readVal = 0x00;
    uint8_t en = 0x00;

    const uint8_t MASK_TPC_CH2 = 0x0C;
    const uint8_t MASK_TPC_CH1FCH2 = 0x10;
    const uint8_t MASK_TPC_STEP = 0x1F;
    const uint8_t MASK_TPC_GPIO_CH2 = 0xF0;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getTx2AttenCtrlPin()\n");
#endif

    /* Checking for null passed parameters */
    if (stepSize == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM));
        return MYKONOS_ERR_GETTX2PIN_STEP_NULL_PARM;
    }

    if (tx2AttenIncPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2PIN_INC_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX2PIN_INC_NULL_PARM));
        return MYKONOS_ERR_GETTX2PIN_INC_NULL_PARM;
    }

    if (tx2AttenDecPin == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM));
        return MYKONOS_ERR_GETTX2PIN_DEC_NULL_PARM;
    }

    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM));
        return MYKONOS_ERR_GETTX2PIN_EN_NULL_PARM;
    }

    if (useTx1ForTx2 == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM,
               getGpioMykonosErrorMessage(MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM));
        return MYKONOS_ERR_GETTX2PIN_TX1TX2_NULL_PARM;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_TPC_GPIO_CFG, &readVal);
    readVal = readVal & MASK_TPC_GPIO_CH2;

    if (readVal & 0x10)
    {
        *tx2AttenIncPin = MYKGPIO14;
    }
    else
    {
        *tx2AttenIncPin = MYKGPIO6;
    }

    if (readVal & 0x40)
    {
        *tx2AttenDecPin = MYKGPIO15;
    }
    else
    {
        *tx2AttenDecPin = MYKGPIO7;
    }

    /* Getting Pin configuration assignment*/
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_TPC_CONFIG, &readVal);

    en = readVal & MASK_TPC_CH2;

    *enable = (en == MASK_TPC_CH2) ? 1 : 0;

    *useTx1ForTx2 = readVal & MASK_TPC_CH1FCH2;

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TX_INCR_DECR_WORD, &readVal);

    *stepSize = readVal & MASK_TPC_STEP;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets the Mykonos low voltage GPIO output pins level
 *
 * This function will only affect the GPIO pins that have their OE direction set to output and
 * that have the correct source control for the nibbles in GPIO_BITBANG_MODE
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioPinLevel bit per GPIO pin, level to output for each GPIO pin. 0 = low output, 1= high output
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioPinLevel(mykonosDevice_t *device, uint32_t gpioPinLevel)
{

    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioPinLevel()\n");
#endif

    /* writing GPIO configuration registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_7_0, (gpioPinLevel & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_15_8, ((gpioPinLevel >> 8) & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_18_16, ((gpioPinLevel >> 16) & 0x07));

    /* Return */
    return error;
}

/**
 * \brief Reads the Mykonos low voltage GPIO pin levels and returns their contents in a single 32-bit word
 *
 *  The GPIO pins that are set to be inputs in BITBANG mode will read back and be returned
 *  in the gpioPinLevel parameter. The return value is a bit per pin.  GPIO 0 returns on bit 0 of
 *  the gpioPinLevel parameter.  A logic low level returns a 0, a logic high level returns a 1.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioPinLevel Input Gpio pin levels read back on the pins assigned as inputs (bit per pin)
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_READGPIOSPI_NULL_PARM The pointer passed for gpioPinLevel parameter is NULL
 */
mykonosGpioErr_t MYKONOS_getGpioPinLevel(mykonosDevice_t *device, uint32_t *gpioPinLevel)
{
    uint8_t readBytes[3] = {0, 0, 0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioPinLevel()\n");
#endif

    if (gpioPinLevel == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READGPIOSPI_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_READGPIOSPI_NULL_PARM));
        return MYKONOS_ERR_READGPIOSPI_NULL_PARM;
    }

    /* reading the registers into three-byte array */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_READ_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_READ_15_8, &readBytes[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_READ_18_16, &readBytes[2]);

    /* performing concatenation and assigning value to gpio1v8SpiRead */
    *gpioPinLevel = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the GPIO output pins are
 *  set to drive out the pins.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioPinSetLevel is a unit32_t pointer which contains the level of each GPIO pin (bit per pin)
 *
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM gpioPinSetLevel pointer is NULL in function parameter
 *
 */
mykonosGpioErr_t MYKONOS_getGpioSetLevel(mykonosDevice_t *device, uint32_t *gpioPinSetLevel)
{
    uint8_t readBytes[3] = {0, 0, 0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioSetLevel()\n");
#endif

    if (gpioPinSetLevel == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM));
        return MYKONOS_ERR_GETGPIOSETLEVEL_NULL_PARM;
    }

    /* reading the registers into two-byte array */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_15_8, &readBytes[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_SPI_SRC_18_16, &readBytes[2]);

    /* performing concatenation and assigning value to gpioPinSetLevel */
    *gpioPinSetLevel = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

    return MYKONOS_ERR_GPIO_OK;
}



/**
 * \brief Sets the General Purpose (GP) interrupt register bit mask to enable interrupt sources to assert the GP Interrupt pin
 *
 *  Mykonos has a single output pin called General Purpose Interrupt that asserts to a logic high level
 *  when certain events occur.  The events that cause the GP Interrupt pin to assert are user
 *  selectable by setting the gpMask parameter in this function.  Device default is mask = x1FF = ignore
 *  all events.  The ARM Error interrupt can not be ignored and can always assert the GP interrupt pin.
 *
 *     gpMask  |  Bit description
 * ------------|------------
 *        [0]  | TXPLL LOCK - 0 = allow PLL unlocking to assert GP Interrupt pin, 1 = ignore TXPLL LOCK
 *        [1]  | RXPLL LOCK - 0 = allow PLL unlocking to assert GP Interrupt pin, 1 = ignore RXPLL LOCK
 *        [2]  | SNIFFER PLL LOCK - 0 = allow PLL unlocking to assert GP Interrupt pin, 1 = ignore SNIFFER PLL LOCK
 *        [3]  | CALIBRATION PLL LOCK - 0 = allow PLL unlocking to assert GP Interrupt pin, 1 = ignore CALIBRATION PLL LOCK
 *        [4]  | CLKPLL LOCK - 0 = allow PLL unlocking to assert GP Interrupt pin, 1 = ignore CLKPLL LOCK
 *        [5]  | 0 = Allow JESD204 deframer interrupt to assert GP Interrupt pin, 1 = ignore JESD204 deframer interrupt
 *        [6]  | Tx1 PA protection - 0 = allow Tx1 PA protection event to assert GP Interrupt pin, 1 = ignore Tx1 PA protection
 *        [7]  | Tx2 PA protection - 0 = allow Tx2 PA protection event to assert GP Interrupt pin, 1 = ignore Tx2 PA protection
 *        [8]  | Mykonos ARM Watchdog - 0 = allow Mykonos ARM Watchdog timeout to assert GP Interrupt pin, 1 = ignore Watchdog timeout event
 *     [15-9]  | Reserved for future use
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device Pointer to the device settings structure
 * \param gpMask Value is passed to enable one or more general purpose interrupt sources
 *               (1=ignore source, 0 = enable source interrupt to GP Interrupt pin)
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_INV_GP_INT_MASK_PARM if invalid interrupt mask is passed
 */
mykonosGpioErr_t MYKONOS_configGpInterrupt(mykonosDevice_t *device, uint16_t gpMask)
{
    const uint16_t GP_INT_VALID_MASK = 0x1FF;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_configGpInterrupt()\n");
#endif

    /* checking for valid mask setting */
    if (gpMask & ~GP_INT_VALID_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_GP_INT_MASK_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_INV_GP_INT_MASK_PARM));
        return MYKONOS_ERR_INV_GP_INT_MASK_PARM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GP_INTERRUPT_MASK_1, (gpMask & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GP_INTERRUPT_MASK_0, ((gpMask >> 8) & 0x01));

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the General Purpose (GP) interrupt status to determine what caused the GP Interrupt pin to assert
 *
 *  When the BBIC detects a rising edge on the General Purpose Interrupt pin, this function
 *  allows the BBIC to determine the source of the interrupt.  The value returned in the status parameter
 *  will show one or more sources for the interrupt based on the following table.
 *
 *  The PLL unlock bits are not sticky.  They will follow the current status of the PLLs.  If the PLL relocks, the
 *  status bit will clear as well.  The GP Interrupt pin is the logical OR of all the sources.  When all the status
 *  bits are low, the GP Interrupt pin will be low.  The status word readback will show the current value
 *  for all interrupt sources, even if they are disabled by the masked. However, the GP Interrupt pin will only assert
 *  for the enabled sources.
 *
 *     status  |  Bit description
 * ------------|------------
 *        [0]  | 1 = TXPLL UNLOCK
 *        [1]  | 1 = RXPLL UNLOCK
 *        [2]  | 1 = SNIFFER PLL UNLOCK
 *        [3]  | 1 = CALIBRATION PLL UNLOCK
 *        [4]  | 1 = CLK PLL UNLOCK
 *        [5]  | 1 = JESD204 deframer interrupt occurred
 *        [6]  | 1 = Tx1 PA protection event
 *        [7]  | 1 = Tx2 PA protection event
 *        [8]  | 1 = Mykonos ARM Watchdog timeout
 *        [9]  | 1 = ARM interrupt occurred
 *    [15-10]  | Reserved for future use
 *
 * <B>Dependencies</B>
 * - device->spiSettings
 *
 * \param device Pointer to the device settings structure
 * \param status parameter to return the IRQ source(s) that caused the GP Interrpt pin to assert.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM if null *status pointer is passed
 */
mykonosGpioErr_t MYKONOS_readGpInterruptStatus(mykonosDevice_t *device, uint16_t *status)
{
    uint8_t readStatus1 = 0;
    uint8_t readStatus0 = 0;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_readGpInterruptStatus()\n");
#endif
    /* checking for null pointer */
    if (status == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM));
        return MYKONOS_ERR_GP_INT_STATUS_NULL_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GP_INTERRUPT_READ_1, &readStatus1);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GP_INTERRUPT_READ_0, &readStatus0);

    /* PLL Lock status bits are high when locked, invert to match up with other bits */
    *status = ((((uint16_t)(readStatus1) & 0xE0U) | ((~(uint16_t)(readStatus1)) & 0x1F)) | (((uint16_t)(readStatus0) & 0x0003) << 8)) ;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets the input and output GPIO pin selections for ARM related signals.
 *
 * The BBP should not have to call this as it will automatically be setup during the
 * MYKONOS_loadArmFromBinary() function call.  If the BBP wishes to change the GPIO
 * assignments this function can be called again to change the configuration while
 * the ARM is in the radioOff state.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->armGpio : all members in structure
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SET_ARMGPIO_INV_POINTER device->auxIo->armGpio pointer is null
 * \retval MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID Invalid ARM GPIO pin signal ID
 * \retval MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN Invalid GPIO pin selected for ARM output (valid 0-15 + output enable in bit[4])
 * \retval MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR ARM returned error setting GPIO pins
 */
mykonosGpioErr_t MYKONOS_setArmGpioPins(mykonosDevice_t *device)
{
    uint8_t i = 0;
    uint8_t gpioConfig[3] = {0};
    uint8_t signalId[12] = {ORX_TRIGGER_SIGNALID, ORX_MODE_0_SIGNALID, ORX_MODE_1_SIGNALID, ORX_MODE_2_SIGNALID, RX1_ENABLE_ACK_SIGNALID,
            RX2_ENABLE_ACK_SIGNALID, TX1_ENABLE_ACK_SIGNALID, TX2_ENABLE_ACK_SIGNALID, ORX1_ENABLE_ACK_SIGNALID, ORX2_ENABLE_ACK_SIGNALID,
            SRX_ENABLE_ACK_SIGNALID, TX_OBS_SELECT_SIGNALID};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    uint32_t gpioOe = 0;
    uint32_t gpioUsedMask = 0;
    mykonosGpioErr_t retval = MYKONOS_ERR_GPIO_OK;

    const uint8_t GPIO_CTRL_OBJECTID = 0x60;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setArmGpioPins()\n");
#endif

    if (device->auxIo->armGpio == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_ARMGPIO_INV_POINTER,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_INV_POINTER));
        return MYKONOS_ERR_SET_ARMGPIO_INV_POINTER;
    }

    /* Write ARM set command to setup which ARM signals are using which GPIO pins */
    /* Setup input pins for ORX_MODE[2:0] and ORX_MODE_Trigger if orx is in pin mode */
    for(i = 0; i < sizeof(signalId); i++)
    {
        gpioConfig[0] = GPIO_CTRL_OBJECTID;
        gpioConfig[1] = signalId[i];

        switch(signalId[i])
        {
            case ORX_TRIGGER_SIGNALID:      gpioConfig[2] = device->auxIo->armGpio->orxTriggerPin;   break;
            case ORX_MODE_0_SIGNALID:       gpioConfig[2] = device->auxIo->armGpio->orxMode0Pin;     break;
            case ORX_MODE_1_SIGNALID:       gpioConfig[2] = device->auxIo->armGpio->orxMode1Pin;     break;
            case ORX_MODE_2_SIGNALID:       gpioConfig[2] = device->auxIo->armGpio->orxMode2Pin;     break;
            case RX1_ENABLE_ACK_SIGNALID:   gpioConfig[2] = device->auxIo->armGpio->rx1EnableAck;    break;
            case RX2_ENABLE_ACK_SIGNALID:   gpioConfig[2] = device->auxIo->armGpio->rx2EnableAck;    break;
            case TX1_ENABLE_ACK_SIGNALID:   gpioConfig[2] = device->auxIo->armGpio->tx1EnableAck;    break;
            case TX2_ENABLE_ACK_SIGNALID:   gpioConfig[2] = device->auxIo->armGpio->tx2EnableAck;    break;
            case ORX1_ENABLE_ACK_SIGNALID:  gpioConfig[2] = device->auxIo->armGpio->orx1EnableAck;   break;
            case ORX2_ENABLE_ACK_SIGNALID:  gpioConfig[2] = device->auxIo->armGpio->orx2EnableAck;   break;
            case SRX_ENABLE_ACK_SIGNALID:   gpioConfig[2] = device->auxIo->armGpio->srxEnableAck;    break;
            case TX_OBS_SELECT_SIGNALID:    gpioConfig[2] = device->auxIo->armGpio->txObsSelect;     break;
            default:
            {
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex,  MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID,
                        getGpioMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID));
                return MYKONOS_ERR_SET_ARMGPIO_PINS_INV_SIGNALID;
            }
        }

        if (gpioConfig[2] > 0x1F)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex,  MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN));
            return MYKONOS_ERR_SET_ARMGPIO_PINS_INV_GPIOPIN;
        }

        /* if output signal */
        if ((signalId[i] >= RX1_ENABLE_ACK_SIGNALID) && (gpioConfig[2] != 0))
        {
            gpioOe |= (((gpioConfig[2] >> 4) & 0x01) << (gpioConfig[2] & 0x0F)); /* 1 = output */
            gpioUsedMask |= (1 << (gpioConfig[2] & 0x0F));
        }

        /* if input signal and orx Pin mode enabled - currently only input pins are orx pin mode control */
        if ((signalId[i] < RX1_ENABLE_ACK_SIGNALID) && (device->auxIo->armGpio->orxPinMode > 0))
        {
            gpioUsedMask |= (1 << (gpioConfig[2] & 0x1F));
        }

        retval = MYKONOS_sendArmCommand(device, MYKONOS_ARM_SET_OPCODE, &gpioConfig[0], sizeof(gpioConfig));
        if (retval != MYKONOS_ERR_GPIO_OK)
        {
            return retval;
        }

        timeoutMs = 1000;
        retval = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_SET_OPCODE, timeoutMs, &cmdStatusByte);
        if (retval != MYKONOS_ERR_GPIO_OK)
        {
            return retval;
        }

        if (cmdStatusByte > 0)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex,  MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR));
            return MYKONOS_ERR_SET_ARMGPIO_PINS_ARMERROR;
        }
    }

    /* Mykonos SPI regs to set GPIO OE direction only for pins used by ARM */
    retval = MYKONOS_setGpioOe(device, gpioOe, gpioUsedMask);
    if (retval != MYKONOS_ERR_GPIO_OK)
    {
        return retval;
    }

    //if any output pins enabled, write GPIO nibble source control.  ARM outputs only allowed on GPIO[15:0]
    if (gpioOe & 0x000F)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_LOWER_BYTE, 0x09, 0x0F, 0);
    }

    if (gpioOe & 0x00F0)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_LOWER_BYTE, 0x09, 0xF0, 4);
    }

    if (gpioOe & 0x0F00)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_UPPER_BYTE, 0x09, 0x0F, 0);
    }

    if (gpioOe & 0xF000)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_UPPER_BYTE, 0x09, 0xF0, 4);
    }

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Configures the Radio power up/down control for Rx and Tx paths to be controlled by pins
 *        (TX1/2_ENABLE, RX1/2_ENABLE, and GPIO pins) or an API function call.
 *
 * The BBP should not have to call this as it will automatically be setup at the end of the
 * MYKONOS_loadArmFromBinary() function call.  If the BBP wishes to change the radio power up/down
 * control method this function can be called again to change the configuration while
 * the ARM is in the radioOff state.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->armGpio->useRx2EnablePin
 * - device->auxIo->armGpio->useTx2EnablePin
 * - device->auxIo->armGpio->txRxPinMode
 * - device->auxIo->armGpio->orxPinMode
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SET_RADIOCTRL_PINS_ARMERROR ARM returned an error and did not accept the command.
 */
mykonosGpioErr_t MYKONOS_setRadioControlPinMode(mykonosDevice_t *device)
{
    uint8_t extData[4] = {0x81, 0, 0, 4}; //Object ID 0x81 (radio control structure), offset lsb, offset msb, length
    uint8_t armRadioControlStruct[4] = {0};
    uint32_t timeoutMs = 0;
    uint8_t cmdStatusByte = 0;
    mykonosGpioErr_t retval = MYKONOS_ERR_GPIO_OK;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRadioControlPinMode()\n");
#endif

    /* write ARM radio control structure to enable pin mode/command mode */
    if (device->auxIo->armGpio->useRx2EnablePin > 0)
    {
        armRadioControlStruct[0] = 0x01;
    }

    if (device->auxIo->armGpio->useTx2EnablePin > 0)
    {
        armRadioControlStruct[1] = 0x01;
    }

    if (device->auxIo->armGpio->txRxPinMode > 0)
    {
        armRadioControlStruct[2] = 0x01;
    }

    if (device->auxIo->armGpio->orxPinMode > 0)
    {
        armRadioControlStruct[3] = 0x01;
    }

    retval = MYKONOS_writeArmMem(device, MYKONOS_ADDR_ARM_START_DATA_ADDR, &armRadioControlStruct[0], sizeof(armRadioControlStruct));
    if (retval != MYKONOS_ERR_GPIO_OK)
    {
        return retval;
    }

    retval = MYKONOS_sendArmCommand(device, MYKONOS_ARM_WRITECFG_OPCODE, &extData[0], sizeof(extData));
    if (retval != MYKONOS_ERR_GPIO_OK)
    {
        return retval;
    }

    timeoutMs = 1000;
    retval = MYKONOS_waitArmCmdStatus(device, MYKONOS_ARM_WRITECFG_OPCODE, timeoutMs, &cmdStatusByte);
    if (retval != MYKONOS_ERR_GPIO_OK)
    {
        return retval;
    }

    if (cmdStatusByte > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex,  MYKONOS_ERR_SET_RADIOCTRL_PINS_ARMERROR,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_RADIOCTRL_PINS_ARMERROR));
        return MYKONOS_ERR_SET_RADIOCTRL_PINS_ARMERROR;
    }

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets up the auxiliary ADC
 *
 *  This function configures the AuxADC with the requested decimation.  The AuxADC clock is set as close
 *  as possible to 40MHz.  The AuxADC conversion time = (1/40Mhz) * decimation, where the decimation ranges
 *  from 256 AuxADC clock cycles to 32768 AuxADc clock cycles.
 *
 *  Note: The AuxADC is intended for relative measurements.  Two back to back measurements can allow a delta
 *  measurement with 12bit resolution.  If absolute measurements are required, an accurate
 *  reference should be first measured on AuxADC0 input and used to calibrate the offset/gain error of the AuxADC.
 *  The reference would need to be measured before each measurement to account for measurement variations caused
 *  by the transmitter/receiver transients as other circuits in the device are being powered up/down.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to the device settings structure
 * \param adcDecimation ADC decimation factor (0-7). Decimates by 256 * 2^(adcDecimation) ADC clock cycles.
 * \param enable Stop/run ADC (0/1)
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SETUPAUXADC_INV_VCODIV CLKPLL VCO divider is invalid - Check CLKPLL setup
 * \retval MYKONOS_ERR_INV_AUX_ADC_DEC_PARM AuxADC decimation out of range (valid 0-7)
 */
mykonosGpioErr_t MYKONOS_setupAuxAdcs(mykonosDevice_t *device, uint8_t adcDecimation, uint8_t enable)
{
    uint32_t hsDigClk_kHz = 0;
    uint32_t auxAdcDiv = 0;
    uint32_t auxAdcClk_kHz = 40000;
    uint32_t vcoDiv = device->clocks->clkPllVcoDiv;
    uint8_t vcoDivTimes10 = 10;

    const uint8_t AUXADC_POWER_BIT_MASK = 0x80;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setupAuxAdcs()\n");
#endif

    if(enable == 0)
    {
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_BUFFER_CONFIG_0, 0xCF);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_CFG, 0x01);

        return MYKONOS_ERR_GPIO_OK;
    }
    else
    {
        switch(vcoDiv)
        {
            case VCODIV_1:
                vcoDivTimes10 = 10;
                break;
            case VCODIV_1p5:
                vcoDivTimes10 = 15;
                break;
            case VCODIV_2:
                vcoDivTimes10 = 20;
                break;
            case VCODIV_3:
                vcoDivTimes10 = 30;
                break;

            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPAUXADC_INV_VCODIV,
                        getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPAUXADC_INV_VCODIV));
                return MYKONOS_ERR_SETUPAUXADC_INV_VCODIV;
        }

        hsDigClk_kHz = (device->clocks->clkPllVcoFreq_kHz / vcoDivTimes10 / device->clocks->clkPllHsDiv) * 10;
        auxAdcDiv = ((hsDigClk_kHz / 2) / (auxAdcClk_kHz)) - 1;

        if(auxAdcDiv > 63)
        {
            auxAdcDiv = 63;
        }

        if(adcDecimation > 0x07)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AUX_ADC_DEC_PARM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_INV_AUX_ADC_DEC_PARM));
            return MYKONOS_ERR_INV_AUX_ADC_DEC_PARM;
        }

        CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADR_AUX_ADC_CLOCK_DIVIDE, (uint8_t)(auxAdcDiv));
        CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADDR_AUX_ADC_CFG, (uint8_t)((adcDecimation << 1)));
        CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADDR_AUX_ADC_SEL, 0x00); /* Set AuxADC select to AuxADC0 */
        CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_AUX_ADC_BUFFER_CONFIG_0, 0, AUXADC_POWER_BIT_MASK, 7);

        /* Set Clock enable bit to latch divider */
        CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADR_AUX_ADC_CLOCK_DIVIDE, (uint8_t)(auxAdcDiv | 0x80));
    }
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets the selected channel of the auxiliary ADC
 *
 *  After setting the AuxADC channel, wait at least 1 AuxADC conversion time before
 *  reading back the AuxADC value.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param auxAdcChannel desired Aux ADC input(0-4 and 16 = temperature sensor)
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM Invalid AuxADC channel (valid 0-4 and 16)
 */
mykonosGpioErr_t MYKONOS_setAuxAdcChannel(mykonosDevice_t *device, mykonosAuxAdcChannels_t auxAdcChannel)
{
    uint8_t currentAuxAdcChan = 0x00;

    const uint8_t CHANNEL_MASK = 0x17;
    const uint8_t POWER_UP_AUXADC = 0x00;
    const uint8_t POWER_DOWN_AUXADC = 0x01;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setAuxAdcChannel()\n");
#endif

    if ((auxAdcChannel & ~CHANNEL_MASK) > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM));
        return MYKONOS_ERR_INV_AUX_ADC_CHAN_PARM;
    }

    /* Read current selected channel, if different power down AUXADC and change AUXADC */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_SEL, &currentAuxAdcChan);
    if (currentAuxAdcChan != auxAdcChannel)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AUX_ADC_CFG, POWER_DOWN_AUXADC, 0x01, 0);
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_SEL, auxAdcChannel);
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AUX_ADC_CFG, POWER_UP_AUXADC, 0x01, 0);
    }

    /* Invalid AuxADC channel, the only valid channel for external ref is channel 0 */
    if (auxAdcChannel == MYK_AUXADC_0_DIV2)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AUX_ADC_BUFFER_CONFIG_1, 0x01, 0x04, 2);
    }
    else if (auxAdcChannel == MYK_AUXADC_0)
    {
        CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AUX_ADC_BUFFER_CONFIG_1, 0x00, 0x04, 2);
    }

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads from the selected channel of the auxiliary ADC
 *
 *  Before using this function to read back the AuxADC value of the
 *  currently selected AuxADC, make sure that at least 1 conversion time
 *  of the ADC has passed since setting the AuxADC channel.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param adcCode is a pointer for return of the 12bit ADC read value
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_READAUXADC_NULL_PARAM Function parameter adcCode is a NULL pointer
 */
mykonosGpioErr_t MYKONOS_readAuxAdc(mykonosDevice_t *device, uint16_t *adcCode)
{
    uint8_t adcNibble = 0;

    const uint8_t AUXADC_LOCK_BIT_MASK = 0x80;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_readAuxAdcs()\n");
#endif

    if (adcCode == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READAUXADC_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_READAUXADC_NULL_PARAM));
        return MYKONOS_ERR_READAUXADC_NULL_PARAM;
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_AUX_ADC_CFG, 1, AUXADC_LOCK_BIT_MASK, 7);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_READ_MSB, &adcNibble);
    *adcCode = adcNibble << 4;
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_AUX_ADC_READ_LSB, &adcNibble);
    *adcCode |= (adcNibble & 0x0F);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets up the 10 AuxDACs on the Mykonos device.
 *
 * This function uses the configuration in the Mykonos device data structure AuxIO substructure to
 * setup which of the ten AuxDACs are enabled, their slope, Vref(mid point) and their initial DAC code.
 *
 *  This function can be called any time after MYKONOS_initialize() to reconfigure, enable, disable the
 *  different AuxDAC outputs. The AuxDACs are used in manual control mode.  After calling this setup
 *  function, it is possible to change a particular AuxDAC code by calling the writeAuxDac() function.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->auxDacEnable: 1= enabled, 0 = disabled
 * - device->auxIo->auxDacSlope[i]: 0 = 1.404mv/code, 1= 0.702mV/code
 * - device->auxIo->auxDacVref: 0 = 1v midpoint, 1 = 1.5v midpoint, 2 = 2v midpoint, 3 = 2.5v midpoint
 * - device->auxIo->auxDacValue[i]: 10bit DAC code (0-1023)
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM device->auxIo structure has a NULL pointer
 * \retval MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE auxDAC code is out of range (valid 0-1023)
 */
mykonosGpioErr_t MYKONOS_setupAuxDacs(mykonosDevice_t *device)
{
    uint8_t i = 0; /* for loop index */
    uint8_t auxDacConfig = 0 ;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setupAuxDacs()\n");
#endif

    if (device->auxIo == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM));
        return MYKONOS_ERR_SETUPAUXDAC_NULL_PARAM;
    }

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_CONTROL_5_0, 0x3F); /* Enable Manual AuxDAC control for all AuxDACs[5:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_CONTROL_9_6, 0x0F); /* Enable Manual AuxDAC control for all AuxDACs[9:6] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_IN_5_0, 0x3F); /* Power down all AuxDACs[5:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_IN_9_6, 0x0F); /* Power down all AuxDACs[9:6] */

    for (i = 0; i < 10; i++)
    {
        /* If auxDac enabled, setup AuxDAC configuration */
        if ((device->auxIo->auxDacEnable >> i) & 0x01)
        {
            if (device->auxIo->auxDacValue[i] > 1023)
            {
                device->auxIo->auxDacValue[i] = 1023; /* clip value to max */
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE,
                        getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE));
                error = MYKONOS_ERR_SETUPAUXDAC_INV_AUXDACCODE; /* Complete AuxDAC configuration before returning error code */
            }

            auxDacConfig = 0x00; /* initialize config variable */

            if (device->auxIo->auxDacSlope[i] > 0)
            {
                auxDacConfig = 0x40; /* bit [6] sets the slope */
            }

            auxDacConfig |= ((device->auxIo->auxDacVref[i] & 0x03) << 4);

            /* Write AuxDAC config and DAC code */
            CMB_SPIWriteByte(device->spiSettings, (MYKONOS_ADDR_AUXDAC_0_WORD_MSB + (i * 2)), (auxDacConfig | ((device->auxIo->auxDacValue[i] >> 8) & 0x0F)));
            CMB_SPIWriteByte(device->spiSettings, (MYKONOS_ADDR_AUXDAC_0_WORD_MSB + (i * 2 ) + 1), (device->auxIo->auxDacValue[i] & 0xFF));
        }
    }

    /* Write enable bit to latch DAC codes into DACs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AUX_DAC_LATCH_CONTROL, 0x01);

    /* Power up selected AuxDacs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_IN_5_0, ((~device->auxIo->auxDacEnable) & 0x3F)); /* Power up enabled AuxDACs[5:0] */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_PDAUXDAC_MANUAL_IN_9_6, (((~device->auxIo->auxDacEnable) >> 6) & 0x0F)); /* Power up enabled AuxDACs[9:6] */

    return error;
}

/**
 * \brief Writes the current AuxDAC code for a particular AuxDAC
 *
 * This function updates the 10bit code that controls the AuxDAC output voltage.
 * The auxDacCode is updated for the specified auxDAC.  Also the auxDacCode is written
 * to the device data structure for future reference.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->auxDacValue[i]
 *
 * \param device is a pointer to the device settings structure
 * \param auxDacIndex AuxDAC to set the DAC code for (0-9)
 * \param auxDacCode DAC code to update the AuxDAC to.  Sets the output voltage of the DAC (valid 0-1023)
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE AuxDac code invalid (valid 0-1023)
 * \retval MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX AuxDAC index out of range (valid 0-9)
 * \retval MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO device->auxIo has NULL pointer
 */
mykonosGpioErr_t MYKONOS_writeAuxDac(mykonosDevice_t *device, uint8_t auxDacIndex, uint16_t auxDacCode)
{
    uint16_t auxDacAddr = 0;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_writeAuxDac()\n");
#endif

    if (auxDacCode > 1023)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE,
                getGpioMykonosErrorMessage(MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE));
        return MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACCODE;
    }

    if (auxDacIndex > 9)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX,
                getGpioMykonosErrorMessage(MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX));
        return MYKONOS_ERR_WRITEAUXDAC_INV_AUXDACINDEX;
    }

    if (device->auxIo == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO,
                getGpioMykonosErrorMessage(MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO));
        return MYKONOS_ERR_WRITEAUXDAC_NULL_AUXIO;
    }

    device->auxIo->auxDacValue[auxDacIndex] = auxDacCode;

    auxDacAddr = MYKONOS_ADDR_AUXDAC_0_WORD_MSB + (auxDacIndex * 2);

    /* Write AuxDAC config and DAC code */
    CMB_SPIWriteField(device->spiSettings, auxDacAddr, (auxDacCode >> 8), 0x0F, 0);
    CMB_SPIWriteByte(device->spiSettings, (auxDacAddr + 1), (auxDacCode & 0xFF));

    /* Write enable bit to latch DAC codes into DACs */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_AUX_DAC_LATCH_CONTROL, 0x01);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Sets the Mykonos low voltage GPIO configuration registers
 *
 * Sets the low voltage GPIO pin direction for each low voltage GPIO pin and
 * sets the source control mode (feature) for each group of 4 GPIO pins.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio - all members
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER device->auxIo->gpio pointer is NULL
 * \retval MYKONOS_ERR_SET_GPIO_1V8_INV_MODE gpio structure members have invalid enum value for the GPIO source control mode.
 */
mykonosGpioErr_t MYKONOS_setupGpio(mykonosDevice_t *device)
{
    uint32_t srcWrite = 0x000000;
    uint32_t oEnMask = 0x7FFFF;
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setupGpio()\n");
#endif

    if (device->auxIo->gpio == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER));
        return MYKONOS_ERR_SET_GPIO_1V8_INV_POINTER;
    }

    /* write GPIO pin direction registers */
    error = MYKONOS_setGpioOe(device, device->auxIo->gpio->gpioOe, oEnMask);

    /* Check and return if error */
    if (error)
    {
        return error;
    }

    /* write GPIO source control mode */
    if ((device->auxIo->gpio->gpioSrcCtrl3_0 > 15) || (device->auxIo->gpio->gpioSrcCtrl7_4 > 15) ||
        (device->auxIo->gpio->gpioSrcCtrl11_8 > 15) || (device->auxIo->gpio->gpioSrcCtrl15_12 > 15) ||
        (device->auxIo->gpio->gpioSrcCtrl18_16 > 15))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_GPIO_1V8_INV_MODE,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_GPIO_1V8_INV_MODE));
        return MYKONOS_ERR_SET_GPIO_1V8_INV_MODE;
    }

    srcWrite = device->auxIo->gpio->gpioSrcCtrl3_0 + (device->auxIo->gpio->gpioSrcCtrl7_4 << 4) +
            (device->auxIo->gpio->gpioSrcCtrl11_8 << 8) + (device->auxIo->gpio->gpioSrcCtrl15_12 << 12) +
            (device->auxIo->gpio->gpioSrcCtrl18_16 << 16);

    error = MYKONOS_setGpioSourceCtrl(device, srcWrite);

    return error;
}

/**
 * \brief Sets the Mykonos low voltage GPIO output pins direction
 *
 * This function will set the GPIO direction given by the passed parameter,
 * the direction can be either output or input.  The gpioUsedMask parameter
 * allows the function to only affect the GPIO pins of interest.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio->gpioOe
 *
 * \param device is a pointer to the device settings structure
 * \param gpioOutEn (valid range 0 - 0x07FFFF), bit per GPIO pin, the direction is
 *
 * gpioOutEn[bit]  |  GPIO[bit] direction
 * ----------------|-------------------
 *        0        |        input
 *        1        |        output
 *
 * \param gpioUsedMask Mask used to control which Oe bits are set/cleared.  If
 *                     mask bit =1, that bit will be modified by gpioOutEn bit
 *
 * \retval MYKONOS_ERR_GPIO_OE_INV_PARAM If the Output enable parameter is invalid
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioOe(mykonosDevice_t *device, uint32_t gpioOutEn, uint32_t gpioUsedMask)
{
    uint32_t error = MYKONOS_ERR_GPIO_OK;

    const uint32_t GPIO_OE_MASK = 0x7FFFF;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioOe()\n");
#endif

    /* Error checking for correct number of GPIOs available. */
    if (gpioOutEn > GPIO_OE_MASK )
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OE_INV_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_OE_INV_PARAM));
        return MYKONOS_ERR_GPIO_OE_INV_PARAM;
    }

    /* Mykonos SPI regs to set GPIO OE direction */
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_7_0, (gpioOutEn & 0xFF), (gpioUsedMask & 0xFF), 0);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_15_8, ((gpioOutEn >> 8) & 0xFF), ((gpioUsedMask >> 8) & 0xFF), 0);
    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_18_16, ((gpioOutEn >> 16) & 0xFF), ((gpioUsedMask >> 16) & 0xFF), 0);

    /* Updating gpioConfig->gpioSetup->gpioOe output enable */
    error = MYKONOS_getGpioOe(device, &gpioOutEn);
    if (error)
    {
        return error;
    }

    device->auxIo->gpio->gpioOe = gpioOutEn;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Gets the Mykonos low voltage GPIO output pins direction
 *
 * This function will get the GPIO direction currently set in the device,
 * the direction can be either output or input. The return gpioOutEn function
 * parameter returns a bit per GPIO pin.  1 = output from the Mykonos Device,
 * 0 = input into the Mykonos device.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param *gpioOutEn a pointer to the data to be returned with the output enable reading
 *
 * gpioOutEn[bit]  |  GPIO[bit] direction
 * ----------------|-------------------
 *        0        |        input
 *        1        |        output
 *
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETGPIO_OE_NULL_PARM gpioOutEn function parameter is NULL
 */
mykonosGpioErr_t MYKONOS_getGpioOe(mykonosDevice_t *device, uint32_t *gpioOutEn)
{
    uint8_t readBytes[3] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioOe()\n");
#endif

    if (gpioOutEn == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIO_OE_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIO_OE_NULL_PARM));
        return MYKONOS_ERR_GETGPIO_OE_NULL_PARM;
    }

    /* Reading GPIO output enable registers */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_15_8, &readBytes[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_DIR_CTL_18_16, &readBytes[2]);

    /* Updating gpioConfig->gpioSetup->gpioOe output enable */
    *gpioOutEn = ((uint32_t)(readBytes[2] & 0x07) << 16) | ((uint32_t)(readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}



/**
 * \brief Sets the Mykonos GPIO source control for different GPIO functionality
 *
 * This function will only affect the GPIO pins that have their OE direction set to output.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioSrcCtrl nibble based source control, this is a 32 bit containing
 * 5 nibbles that will be set the source control.
 *
 * \retval MYKONOS_ERR_GPIO_SRC_PARAM_INV If the source control parameter is invalid
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioSourceCtrl(mykonosDevice_t *device, uint32_t gpioSrcCtrl)
{
    const uint32_t GPIO_SRC_MASK = 0xFFFFF;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioSourceCtrl()\n");
#endif

    /* writing GPIO configuration registers */
    if (gpioSrcCtrl > GPIO_SRC_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_SRC_PARAM_INV,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_SRC_PARAM_INV));
        return MYKONOS_ERR_GPIO_SRC_PARAM_INV;
    }

    /* writing GPIO configuration registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_LOWER_BYTE, (gpioSrcCtrl & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_UPPER_BYTE, ((gpioSrcCtrl >> 8) & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_EXTRA_BITS, ((gpioSrcCtrl >> 16) & 0x0F));

    /* Updating gpioConfig->gpioSetup source control */
    device->auxIo->gpio->gpioSrcCtrl3_0 = gpioSrcCtrl & 0x0F;
    device->auxIo->gpio->gpioSrcCtrl7_4 = (gpioSrcCtrl >> 4) & 0x0F;
    device->auxIo->gpio->gpioSrcCtrl11_8 = (gpioSrcCtrl >> 8) & 0x0F;
    device->auxIo->gpio->gpioSrcCtrl15_12 = (gpioSrcCtrl >> 12) & 0x0F;
    device->auxIo->gpio->gpioSrcCtrl18_16 = (gpioSrcCtrl >> 16) & 0x0F;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos GPIO source control for different GPIO functionality
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioSrcCtrl nibble based source control, this is a 32 bit containing
 * 5 nibbles that will be set the source control.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getGpioSourceCtrl(mykonosDevice_t *device, uint32_t *gpioSrcCtrl)
{
    uint8_t readBytes[3] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioSourceCtrl()\n");
#endif

    /* Reading GPIO output enable registers */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_LOWER_BYTE, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_UPPER_BYTE, &readBytes[1]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_SOURCE_CONTROL_EXTRA_BITS, &readBytes[2]);

    /* Updating gpioConfig->gpioSetup->gpioOe output enable */
    *gpioSrcCtrl = ((uint32_t)(readBytes[2] & 0x0F) << 16) | ((uint32_t)(readBytes[1]) << 8) | (uint32_t)(readBytes[0]);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}



/**
 * \brief Sets the Mykonos 3.3 VDC GPIO configuration registers
 *
 *  This function sets the GPIO 3.3v pin direction (1=output, 0=input) and
 *  sets the mode of each nibble of GPIO 3.3v pins. See the mykonosGpio3v3Mode_t
 *  enum for possible modes.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio3v3 - all members
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER device->auxIo->gpio3v3 pointer is NULL
 * \retval MYKONOS_ERR_SET_GPIO_3V3_INV_MODE gpio3v3 members have invalid enum value for the GPIO3v3 source control mode.
 */
mykonosGpioErr_t MYKONOS_setupGpio3v3(mykonosDevice_t *device)
{
    mykonosGpioErr_t error = MYKONOS_ERR_GPIO_OK;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setupGpio3v3()\n");
#endif

    if (device->auxIo->gpio3v3 == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER));
        return MYKONOS_ERR_SET_GPIO_3V3_INV_POINTER;
    }
    else
    {
        /* write GPIO pin direction registers */
        error = MYKONOS_setGpio3v3Oe(device, device->auxIo->gpio3v3->gpio3v3Oe);

        /* write GPIO3v3 mode */
        if ((device->auxIo->gpio3v3->gpio3v3SrcCtrl3_0 > 15) ||
                (device->auxIo->gpio3v3->gpio3v3SrcCtrl7_4 > 15) ||
                (device->auxIo->gpio3v3->gpio3v3SrcCtrl11_8 > 15))
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_GPIO_3V3_INV_MODE,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SET_GPIO_3V3_INV_MODE));
            return MYKONOS_ERR_SET_GPIO_3V3_INV_MODE;
        }

        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_LSB_SRC_CTL, ((device->auxIo->gpio3v3->gpio3v3SrcCtrl7_4 << 4) | device->auxIo->gpio3v3->gpio3v3SrcCtrl3_0));
        CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_MSB_SRC_CTL, (device->auxIo->gpio3v3->gpio3v3SrcCtrl11_8 & 0x0F));

        return error;
    }
}

/**
 * \brief If the GPIO3v3 pins are setup for BITBANG mode, this function sets
 *        the output pin levels per pin
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio3v3 - all members
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3PinLevel Bit per pin to set the level of each GPIO3v3 output pin
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpio3v3PinLevel(mykonosDevice_t *device, uint16_t gpio3v3PinLevel)
{

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpio3v3PinLevel()\n");
#endif

    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_SRC_7_0, (gpio3v3PinLevel & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_SRC_15_8, ((gpio3v3PinLevel >> 8) & 0x0F));

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos 3.3 VDC GPIO pin output levels for BITBANG mode
 *
 *  This function allows reading the value that the 3.3v GPIO output pins are
 *  set to drive out the pins.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3SetLevel is a unit16_t pointer which contains the level of each GPIO3V3 pin (bit per pin)
 *
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM gpio3v3SetLevel pointer is NULL in function parameter
 */
mykonosGpioErr_t MYKONOS_getGpio3v3SetLevel(mykonosDevice_t *device, uint16_t *gpio3v3SetLevel)
{
    uint8_t readBytes[2] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpio3v3SetLevel()\n");
#endif

    if (gpio3v3SetLevel == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM));
        return MYKONOS_ERR_GETGPIO3V3OUT_NULL_PARM;
    }

    /* reading the registers into two-byte array */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_SRC_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_SRC_15_8, &readBytes[1]);

    /* performing concatenation and assigning value to gpio3v3SpiRead */
    *gpio3v3SetLevel = ((uint16_t)(readBytes[1] & 0x0F) << 8) | (uint16_t)(readBytes[0]);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos 3.3 VDC GPIO pin levels for BITBANG mode
 *
 *  Note: this function is only capable of reading back pin levels for the
 *  3.3v pins set to be inputs.  Any pins set to be a GPIO output will read
 *  back as zero.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3PinLevel is a unit16_t pointer which contains the level of each GPIO3V3 pin
 *                        that is defined as an input
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM gpio3v3PinLevel pointer is NULL in function parameter
 */
mykonosGpioErr_t MYKONOS_getGpio3v3PinLevel(mykonosDevice_t *device, uint16_t *gpio3v3PinLevel)
{
    uint8_t readBytes[2] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpio3v3PinLevel()\n");
#endif

    if (gpio3v3PinLevel == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM));
        return MYKONOS_ERR_GETGPIO3V3SPI_NULL_PARM;
    }

    /* reading the registers for input pin levels */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_READ_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_SPI_READ_15_8, &readBytes[1]);

    *gpio3v3PinLevel = ((uint16_t)(readBytes[1] & 0x0F) << 8) | (uint16_t)(readBytes[0]);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This function sets the Pin direction control for the Mykonos 3.3 VDC GPIO.
 *
 * The bits in gpio3v3OutEn are used to configure each corresponding pin as an input or an output.
 * If the bit is set, the pin is configured as an output, and if it is clear the pin is configured as an input.
 * For example, setting gpio3v3OutEn = 0x02  will configure GPIO_3p3_2 as an output and the rest pins as inputs.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - device->auxIo->gpio3v3 - all members
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3OutEn Bit per pin to set the level of each GPIO3v3 output pin
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpio3v3Oe(mykonosDevice_t *device, uint16_t gpio3v3OutEn)
{

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpio3v3Oe()\n");
#endif

    /* write GPIO pin direction registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_DIR_CTL_7_0, (gpio3v3OutEn & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_DIR_CTL_15_8, ((gpio3v3OutEn >> 8) & 0x0F));

    /* update device->auxIo->gpio3v3 structure */
    device->auxIo->gpio3v3->gpio3v3Oe = gpio3v3OutEn;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos 3.3 VDC GPIO pin direction
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3OutEn is a unit16_t pointer which will contain the output enable parameter,
 * this will be a bit field, if a bit is set then the corresponding GPIO pin is an output,
 * if the bit is not set then the corresponding pin is an input
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM gpio3v3OutEn pointer is NULL in function parameter
 */
mykonosGpioErr_t MYKONOS_getGpio3v3Oe(mykonosDevice_t *device, uint16_t *gpio3v3OutEn)
{
    uint8_t readBytes[2] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpio3v3Oe()\n");
#endif

    /* checking for null parameter */
    if (gpio3v3OutEn == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM));
        return MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM;
    }

    /* reading the registers for input pin levels */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_DIR_CTL_7_0, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_DIR_CTL_15_8, &readBytes[1]);

    *gpio3v3OutEn = ((uint16_t)(readBytes[1] & 0x0F) << 8) | (uint16_t)(readBytes[0]);

    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Sets the Mykonos GPIO 3.3 VDC source control for different GPIO fucntionality
 *
 * This function will only affect the GPIO pins that have their OE direction set to output.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3SrcCtrl nibble based source control, this is a 12 bit containing
 * 3 nibbles that will be set the source control.
 *
 * \retval MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL gpio3v3 members have invalid value for the GPIO3v3 source control mode.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpio3v3SourceCtrl(mykonosDevice_t *device, uint16_t gpio3v3SrcCtrl)
{
    const uint16_t GPIO_SRC_MASK = 0x0FFF;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpio3v3SourceCtrl()\n");
#endif

    /* writing GPIO configuration registers */
    if (gpio3v3SrcCtrl > GPIO_SRC_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL));
        return MYKONOS_ERR_SET_GPIO_3V3_INV_SRC_CTRL;
    }

    /* writing GPIO configuration registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_LSB_SRC_CTL, (gpio3v3SrcCtrl & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_MSB_SRC_CTL, ((gpio3v3SrcCtrl >> 8) & 0xFF));

    /* Updating gpioConfig->gpioSetup source control */
    device->auxIo->gpio3v3->gpio3v3SrcCtrl3_0 = gpio3v3SrcCtrl & 0x0F;
    device->auxIo->gpio3v3->gpio3v3SrcCtrl7_4 = gpio3v3SrcCtrl & 0xF0;
    device->auxIo->gpio3v3->gpio3v3SrcCtrl11_8 = (gpio3v3SrcCtrl >> 8) & 0x0F;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads the Mykonos 3v3 GPIO source control for different GPIO functionality
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpio3v3SrcCtrl nibble based source control, this is a 12 bit containing
 * 3 nibbles that will be set the source control.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getGpio3v3SourceCtrl(mykonosDevice_t *device, uint16_t *gpio3v3SrcCtrl)
{
    uint8_t readBytes[2] = {0};

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpio3v3SourceCtrl()\n");
#endif

    /* checking for null parameter */
    if (gpio3v3SrcCtrl == NULL)
    {
       CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM,
               getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM));
       return MYKONOS_ERR_GPIO3V3OUTEN_NULL_PARM;
    }

    /* Reading GPIO output enable registers */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_LSB_SRC_CTL, &readBytes[0]);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_GPIO_3V3_MSB_SRC_CTL, &readBytes[1]);

    /* Updating gpioConfig->gpioSetup->gpioOe output enable */
    *gpio3v3SrcCtrl = ((uint16_t)(readBytes[1] & 0xFF) << 8) | ((uint16_t)(readBytes[0]));

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Gain compensation enable and setup function.
 *
 * The gain compensation block is a function that compensates for the attenuation in the internal attenuator for the Rx channels.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gainComp which is a mykonosGainComp_t structure.
 * \param enable this parameter enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT gain compensation structure gainComp is not initialised
 * \retval MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET gain compensation structure gainComp->rx1Offset is invalid
 * \retval MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET gain compensation structure gainComp->rx2Offset is invalid
 * \retval MYKONOS_ERR_GAINCOMP_INV_STEP gain compensation structure gainComp->compStep is invalid
 * \retval MYKONOS_ERR_GAINCOMP_INV_EN enable is not valid
 */
mykonosGpioErr_t MYKONOS_setRxGainCompensation (mykonosDevice_t *device, mykonosGainComp_t *gainComp, uint8_t enable)
{
    uint8_t regWr2 = 0x00;
    uint8_t regWr3 = 0x00;
    uint8_t regWr4 = 0x00;

    /* Max parameter values for error checking */
    const uint8_t RXOFFSET_RANGE = 0x1F;
    const uint8_t STEP_RANGE = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRxGainCompensation()\n");
#endif

    /* Check for gainComp initialised */
    if (gainComp == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT));
        return MYKONOS_ERR_GAINCOMP_SET_NULL_STRUCT;
    }

    /* Check for enable */
    if (enable > 1)
    {
      CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_INV_EN,
                         getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_INV_EN));
      return MYKONOS_ERR_GAINCOMP_INV_EN;
    }
    else if (enable == 1)
    {
        /* Check for gain compensation Rx1 offset range */
        if (gainComp->rx1Offset > RXOFFSET_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET,
                               getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET));
                return MYKONOS_ERR_GAINCOMP_INV_RX1_OFFSET;
        }

        /* Check for gain compensation Rx2 offset range */
        if (gainComp->rx2Offset > RXOFFSET_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET,
                              getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET));
            return MYKONOS_ERR_GAINCOMP_INV_RX2_OFFSET;
        }

        /* Check for gain compensation step range */
        if (gainComp->compStep > STEP_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_INV_STEP,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_INV_STEP));
            return MYKONOS_ERR_GAINCOMP_INV_STEP;
        }

        /* Enabling gain compensation block */
        regWr4 = 0x88 | gainComp->compStep;
        regWr3 = gainComp->rx2Offset;
        regWr2 = gainComp->rx1Offset;
    }

    /* Write gain compensation setup to device */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX1_GAIN_COMP_OFFSET, regWr2);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX2_GAIN_COMP_OFFSET, regWr3);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_GAIN_COMP_CFG, regWr4);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Get gain compensation setup and enabled function.
 *
 * The gain compensation block is a function that compensates for the attenuation in the internal attenuator for the Rx channels.
 * This function will get the current setup and the enable state of the block.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gainComp pointer to a mykonosGainComp_t structure, will held the current device gain compensation settings.
 * \param enabled pointer this parameter will contain the enable state of the gain compensation block.
 * enabled = 1
 * disabled = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GAINCOMP_NULL_STRUCT gain compensation structure gainComp is not initialised
 */
mykonosGpioErr_t MYKONOS_getRxGainCompensation (mykonosDevice_t *device, mykonosGainComp_t *gainComp, uint8_t *enabled)
{
    uint8_t regWr2 = 0x00;
    uint8_t regWr3 = 0x00;
    uint8_t regWr4 = 0x00;

    /* Mask parameter values populating mykonosGainComp_t structure */
    const uint8_t ENABLED_MASK = 0x80;
    const uint8_t STEP_MASK = 0x07;

    /* Shift values for writing gain values */
    const uint8_t ENABLED_SHIFT = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRxGainCompensation()\n");
#endif

    /* Check for gainComp for Null */
    if (gainComp == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GAINCOMP_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GAINCOMP_NULL_STRUCT));
        return MYKONOS_ERR_GAINCOMP_NULL_STRUCT;
    }

    /* Read gain compensation setup from device */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX1_GAIN_COMP_OFFSET, &regWr2);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX2_GAIN_COMP_OFFSET, &regWr3);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX_GAIN_COMP_CFG, &regWr4);


    /* Parsing read data to passed by reference parameters */
    *enabled = (regWr4 & ENABLED_MASK) >> ENABLED_SHIFT;
    gainComp->compStep = (regWr4 & STEP_MASK);
    gainComp->rx2Offset = regWr3;
    gainComp->rx1Offset = regWr2;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Gain compensation enable and setup function for the observation channel.
 *
 * The gain compensation block is a function that compensates for the attenuation in the internal attenuator for the observation channels.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gainComp is a pointer to the mykonosObsRxGainComp_t structure.
 * \param enable enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT gain compensation structure gainComp is not initialised
 * \retval MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET gain compensation structure gainComp->obsRxOffset is invalid
 * \retval MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP gain compensation structure gainComp->compStep is invalid
 * \retval MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN enable is not valid
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setObsRxGainCompensation (mykonosDevice_t *device, mykonosObsRxGainComp_t *gainComp, uint8_t enable)
{
    uint8_t regWr2 = 0x00;
    uint8_t regWr3 = 0x00;

    /* Max parameter values for error checking */
    const uint8_t RXOFFSET_RANGE = 0x1F;

    /* Shift values for writing gain values */
    const uint8_t STEP_RANGE = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setObsRxGainCompensation()\n");
#endif

    /* Check for gainComp initialised */
    if (gainComp == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT));
        return MYKONOS_ERR_OBS_RX_GAINCOMP_SET_NULL_STRUCT;
    }

    /* Check for enable */
    if (enable > 1)
    {
      CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN,
                         getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN));
      return MYKONOS_ERR_OBS_RX_GAINCOMP_INV_EN;
    }
    else if (enable == 1)
    {
        /* Check for gain compensation offset range */
        if (gainComp->obsRxOffset > RXOFFSET_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET,
                               getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET));
                return MYKONOS_ERR_OBS_RX_GAINCOMP_INV_OFFSET;
        }

        /* Check for gain compensation step range */
        if (gainComp->compStep > STEP_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP));
            return MYKONOS_ERR_OBS_RX_GAINCOMP_INV_STEP;
        }

        /* Enabling gain compensation block */
        regWr3 = 0x88 | gainComp->compStep;
        regWr2 = ((gainComp->obsRxOffset & 0x1E) >> 1) | ((gainComp->obsRxOffset & 0x01) << 4);
    }

    /* Write gain compensation setup to device */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_GAIN_COMP_OFFSET, regWr2);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_GAIN_COMP_CFG, regWr3);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Get gain compensation setup and enabled function for the observation channel.
 *
 * The gain compensation block is a function that compensates for the attenuation in the internal attenuator for the observation
 * channels.
 * This function will get the current setup and the enable state of the block.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gainComp pointer to a mykonosObsRxGainComp_t structure, will held the current device gain compensation settings.
 * \param enabled pointer this parameter will contain the enable state of the gain compensation block.
 * enabled = 1
 * disabled = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT observation channel gain compensation structure gainComp is not initialised
 */
mykonosGpioErr_t MYKONOS_getObsRxGainCompensation (mykonosDevice_t *device, mykonosObsRxGainComp_t *gainComp, uint8_t *enabled)
{
    uint8_t regRd2 = 0x00;
    uint8_t regRd3 = 0x00;

    /* Masks values for populating mykonosObsRxGainComp_t structure */
    const uint8_t ENABLED_MASK = 0x80;
    const uint8_t STEP_MASK = 0x07;

    /* Shift values for populating mykonosObsRxGainComp_t structure */
    const uint8_t ENABLED_SHIFT = 0x07;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getObsRxGainCompensation()\n");
#endif

    /* Check for gainComp for Null */
    if (gainComp == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT));
        return MYKONOS_ERR_OBS_RX_GAINCOMP_NULL_STRUCT;
    }

    /* Read gain compensation setup from device */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_GAIN_COMP_OFFSET, &regRd2);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_GAIN_COMP_CFG, &regRd3);


    /* Parsing read data to passed by reference parameters */
    *enabled = (regRd3 & ENABLED_MASK) >> ENABLED_SHIFT;
    gainComp->compStep = (regRd3 & STEP_MASK);
    gainComp->obsRxOffset = ((regRd2 & 0x0F) << 1) | ((regRd2 & 0x10) >> 4);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Slicer control over GPIO inputs.
 *
 * The user can control the slicer position via 3 GPIO inputs per channel.
 * There are various configurations for the GPIO pins, this configurations are enumerated in the mykonosRxSlicer_t.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - Gain compensation block has to be enabled in order to user the slicer control
 *
 * \param device is a pointer to the device settings structure
 * \param slicerStep configures the step size of the slicer when external pin contol mode is enabled.
 * Slicer gain in this mode is determined by multiplying the step size by the input bit code from the BBP.
 * Step sizes are as follows:
 * sllicerStep  |  step
 * -------------|------------
 *       0      |   1
 *       1      |   2
 *       2      |   3
 *       3      |   4
 *
 * \param rx1Pins Rx1 slicer inputs can take values from one of the following combinations that are specified in mykonosRxSlicer_t enum:
 * -GPIO_2, GPIO_1, and GPIO_0
 * -GPIO_7, GPIO_6, and GPIO_5
 * -GPIO_10, GPIO_9, and GPIO_8
 * \param rx2Pins Rx2 slicer inputs can take values from one of the following combinations that are specified in mykonosRxSlicer_t enum:
 * -GPIO_7, GPIO_6, and GPIO_5
 * -GPIO_13, GPIO_12, and GPIO_11
 * \param enable this parameter enables the external pin control mode so the BBP can control the slicer setting.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SLICER_INV_RX1_SEL invalid RX1 GPIO pin selection for Slicer control
 * \retval MYKONOS_ERR_SLICER_INV_RX2_SEL invalid RX2 GPIO pin selection for Slicer control
 * \retval MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE slicer step is out of range
 * \retval MYKONOS_ERR_SLICER_EN_INV invalid enable
 */
mykonosGpioErr_t MYKONOS_setRxSlicerCtrl(mykonosDevice_t *device, uint8_t slicerStep, mykonosRxSlicer_t rx1Pins, mykonosRxSlicer_t rx2Pins, uint8_t enable)
{
    uint8_t regWr = 0x00;

    const uint8_t STEP_MASK = 0x03;
    const uint8_t STEP_SHIFT = 0x05;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRxSlicerCtrl()\n");
#endif

    if (enable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_EN_INV,
                             getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_EN_INV));
        return MYKONOS_ERR_SLICER_EN_INV;
    }
    else if (enable == 1)
    {
        /* Enabling Slicer input control */
        regWr = 0x80;

        /* Check for Rx1 Slicer input control */
        switch (rx1Pins)
        {
            case GPIO_0_1_2:
                regWr |= 0x00;
                break;
            case GPIO_5_6_7:
                regWr |= 0x01;
                break;
            case GPIO_8_9_10:
                regWr |= 0x02;
                break;
            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_INV_RX1_SEL,
                               getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_INV_RX1_SEL));
                return MYKONOS_ERR_SLICER_INV_RX1_SEL;
        }

        /* Check for Rx2 Slicer input control */
        switch (rx2Pins)
        {
            case GPIO_5_6_7:
               regWr |= 0x00;
               break;
           case GPIO_11_12_13:
               regWr |= 0x04;
               break;
           default:
               CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_INV_RX2_SEL,
                              getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_INV_RX2_SEL));
               return MYKONOS_ERR_SLICER_INV_RX2_SEL;
        }

        /* Check for Slicer input step size control */
        if (slicerStep > STEP_MASK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE));
            return MYKONOS_ERR_SLICER_STEP_OUT_OF_RANGE;
        }
        else
        {
            regWr |= (slicerStep << STEP_SHIFT);
        }
    }

    /* Write to device the slicer configuration */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_RX_SLCR_PIN_CFG, regWr);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This function will get the programmed Slicer control for Rx1 and Rx2 channels.
 *
 * The user can control the slicer position via 3 GPIO inputs per channel.
 * There are various configurations for the GPIO pins, this configurations are enumerated in the mykonosRxSlicer_t.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param slicerStep will contain the configured step size
 * \param rx1Pins will contain the configured GPIO combination for Rx1
 * \param rx2Pins will contain the configured GPIO combination for Rx2
 * \param enable will contain the programmed enable setting
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM rx1Pins is null pointer for the passed parameter
 * \retval MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM rx1Pins is null pointer for the passed parameter
 * \retval MYKONOS_ERR_SLICER_STEP_NULL_PARM slicerStep is null pointer for the passed parameter
 * \retval MYKONOS_ERR_SLICER_EN_NULL_PARM enable is null pointer for the passed parameter
 */
mykonosGpioErr_t MYKONOS_getRxSlicerCtrl(mykonosDevice_t *device, uint8_t *slicerStep, mykonosRxSlicer_t *rx1Pins, mykonosRxSlicer_t *rx2Pins, uint8_t *enable)
{
    uint8_t regRd = 0x00;

    /* Mask parameter values populating parameters */
    const uint8_t EN_MASK = 0x80;
    const uint8_t STEP_MASK = 0x60;
    const uint8_t RX1_MASK = 0x03;
    const uint8_t RX2_MASK = 0x0C;

    /* Shift values for populating passed parameters */
    const uint8_t EN_SHIFT = 0x07;
    const uint8_t STEP_SHIFT = 0x05;
    const uint8_t RX1_SHIFT = 0x00;
    const uint8_t RX2_SHIFT = 0x02;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRxSlicerCtrl()\n");
#endif

    /* Check for Rx1 Slicer input control null parameter */
    if (rx1Pins == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM));
        return MYKONOS_ERR_SLICER_RX1PIN_NULL_PARM;
    }

    /* Check for Rx2 Slicer input control null parameter */
    if (rx2Pins == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM));
        return MYKONOS_ERR_SLICER_RX2PIN_NULL_PARM;
    }

    /* Check for Slicer step control null parameter */
    if (slicerStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_STEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_STEP_NULL_PARM));
        return MYKONOS_ERR_SLICER_STEP_NULL_PARM;
    }

    /* Check for Slicer step control null parameter */
    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_EN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_EN_NULL_PARM));
        return MYKONOS_ERR_SLICER_EN_NULL_PARM;
    }

    /* Reading SLicer control register */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_RX_SLCR_PIN_CFG, &regRd);

    /* Parsing values */
    *enable = (regRd & EN_MASK) >> EN_SHIFT;
    *slicerStep = (regRd & STEP_MASK) >> STEP_SHIFT;

    /* Map to enum */
    switch ((regRd & RX2_MASK) >> RX2_SHIFT)
    {
        case 0:
            *rx2Pins = GPIO_5_6_7;
            break;
        case 1:
            *rx2Pins = GPIO_11_12_13;
            break;

        default:
            *rx2Pins = GPIO_5_6_7;
    }

    *rx1Pins = (regRd & RX1_MASK) >> RX1_SHIFT;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Slicer control over GPIO inputs for the observation channel.
 *
 * The user can control the slicer position via 3 GPIO inputs for the observation channel.
 * There are various configurations for the GPIO pins, this configurations are enumerated in the mykonosObsRxSlicer_t.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - Gain compensation block has to be enabled in order to user the slicer control
 *
 * \param device is a pointer to the device settings structure
 * \param slicerStep configures the step size of the slicer when external pin contol mode is enabled.
 * Slicer gain in this mode is determined by multiplying the step size by the input bit code from the BBP.
 * Step sizes are as follows:
 * sllicerStep  |  step
 * -------------|------------
 *       0      |   1
 *       1      |   2
 *       2      |   3
 *       3      |   4
 *
 * \param obsRxPins observation slicer inputs can take values from one of the following combinations that are specified in mykonosObsRxSlicer_t enum:
 * -GPIO_18, GPIO_17 and GPIO_16
 * -GPIO_16, GPIO_15 and GPIO_14
 * \param enable enables the external pin control mode so the BBP can control the slicer setting.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_SLICER_INV_OBS_RX_SEL invalid observation channel GPIO pin selection for Slicer control
 * \retval MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE slicer step is out of range
 * \retval MYKONOS_ERR_SLICER_OBS_RX_EN_INV invalid enable
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setObsRxSlicerCtrl(mykonosDevice_t *device, uint8_t slicerStep, mykonosObsRxSlicer_t obsRxPins, uint8_t enable)
{
    uint8_t regWr = 0x00;

    const uint8_t STEP_MASK = 0x03;
    const uint8_t STEP_SHIFT = 0x05;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setObsRxSlicerCtrl()\n");
#endif

    if (enable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_OBS_RX_EN_INV,
                             getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_OBS_RX_EN_INV));
        return MYKONOS_ERR_SLICER_OBS_RX_EN_INV;
    }
    else if (enable == 1)
    {
        /* Enabling Slicer input control */
        regWr = 0x80;

        /* Check for Rx1 Slicer input control */
        switch (obsRxPins)
        {
            case GPIO_18_17_16:
                regWr |= 0x00;
                break;
            case GPIO_16_15_14:
                regWr |= 0x01;
                break;

            default:
                CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_INV_OBS_RX_SEL,
                               getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_INV_OBS_RX_SEL));
                return MYKONOS_ERR_SLICER_INV_OBS_RX_SEL;
        }

        /* Check for Slicer input step size control */
        if (slicerStep & ~STEP_MASK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE));
            return MYKONOS_ERR_SLICER_OBS_RX_STEP_OUT_OF_RANGE;
        }
        else
        {
            regWr |= (slicerStep << STEP_SHIFT);
        }
    }

    /* Write to device the slicer configuration */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_SLCR_PIN_CFG, regWr);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This function will get the programmed Slicer control for observation channel.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param slicerStep will contain the configured step size
 * \param obsRxPins will contain the configured GPIO combination
 * \param enable will contain the programmed enable setting
 *
 * \retval MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM obsRxPins is null pointer for the passed parameter
 * \retval MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM slicerStep is null pointer for the passed parameter
 * \retval MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM, enable is null pointer for the passed parameter
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getObsRxSlicerCtrl(mykonosDevice_t *device, uint8_t *slicerStep, mykonosObsRxSlicer_t *obsRxPins, uint8_t *enable)
{
    uint8_t regRd = 0x00;

    /* Mask parameter values populating slicer parameters */
    const uint8_t EN_MASK = 0x80;
    const uint8_t STEP_MASK = 0x60;
    const uint8_t OBS_RX_MASK = 0x03;

    /* Shift values for writing slicer parameters */
    const uint8_t EN_SHIFT = 0x07;
    const uint8_t STEP_SHIFT = 0x05;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getObsRxSlicerCtrl()\n");
#endif

    /* Check for Rx1 Slicer input control null parameter */
    if (obsRxPins == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM));
        return MYKONOS_ERR_SLICER_OBS_RXPIN_NULL_PARM;
    }

    /* Check for Slicer step control null parameter */
    if (slicerStep == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM));
        return MYKONOS_ERR_SLICER_OBS_RX_STEP_NULL_PARM;
    }

    /* Check for Slicer step control null parameter */
    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM));
        return MYKONOS_ERR_SLICER_OBS_RX_EN_NULL_PARM;
    }

    /* Reading Slicer control register */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DPD_SNF_RX_SLCR_PIN_CFG, &regRd);

    /* Parsing values */
    *enable = (regRd & EN_MASK) >> EN_SHIFT;
    *slicerStep = (regRd & STEP_MASK) >> STEP_SHIFT;

    /* Map to enum */
    switch (regRd & OBS_RX_MASK)
    {
        case 0:
            *obsRxPins = GPIO_18_17_16;
            break;
        case 1:
            *obsRxPins = GPIO_16_15_14;
            break;

        default:
            *obsRxPins = 0;
            break;
    }

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Floating point formatter enable and setup function.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range (total gain range on AD9370 is 42dB)
 * which increases the bitwidth in the digital datapath.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - Gain compensation block has to be enabled in order to user floating point formatter
 *
 * \param device is a pointer to the device settings structure
 * \param floatFrmt which is a mykonosFloatPntFrmt_t structure.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT floating point formatter structure floatFrmt not initialized
 * \retval MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE floating point formatter structure floatFrmt.roundMode not valid parameter
 * \retval MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT floating point formatter structure floatFrmt.dataFormat not valid parameter
 * \retval MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN floating point formatter structure floatFrmt.encNan not valid parameter
 * \retval MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS floating point formatter structure floatFrmt.expBits not valid parameter
 * \retval MYKONOS_ERR_FLOATFRMT_INV_LEADING floating point formatter structure floatFrmt.leading not valid parameter
 */
mykonosGpioErr_t MYKONOS_setFloatPointFrmt (mykonosDevice_t *device, mykonosFloatPntFrmt_t *floatFrmt)
{
    uint8_t regWr1 = 0x00;

    /* Max parameter values for error checking */
    const uint8_t ROUND_RANGE = 0x04;
    const uint8_t DATA_FRMT_RANGE = 0x01;
    const uint8_t ENCODE_RANGE = 0x01;
    const uint8_t EXPBITS_RANGE = 0x03;
    const uint8_t LEADING_RANGE = 0x01;

    /* Shift values for writing mykonosFloatPntFrmt_t values */
    const uint8_t ROUND_SHIFT = 0x05;
    const uint8_t DATA_FRMT_SHIFT = 0x04;
    const uint8_t ENCODE_SHIFT = 0x03;
    const uint8_t EXPBITS_SHIFT = 0x01;
    const uint8_t LEADING_SHIFT = 0x00;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setFloatPointFrmt()\n");
#endif

    /* Check for gainComp initialised */
    if (&floatFrmt->dataFormat == 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT));
        return MYKONOS_ERR_FLOATFRMT_SET_NULL_STRUCT;
    }

    /* Check for floating point round mode */
    if (floatFrmt->roundMode > ROUND_RANGE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE));
        return MYKONOS_ERR_FLOATFRMT_INV_ROUND_MODE;
    }

    /* Check for floating point data format */
    if (floatFrmt->dataFormat > DATA_FRMT_RANGE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT,
                           getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT));
            return MYKONOS_ERR_FLOATFRMT_INV_DATA_FORMAT;
    }

    /* Check for floating point encode NaN */
    if (floatFrmt->encNan > ENCODE_RANGE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN,
                          getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN));
        return MYKONOS_ERR_FLOATFRMT_INV_ENC_NAN;
    }

    /* Check for floating point exponent bits range */
    if (floatFrmt->expBits > EXPBITS_RANGE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS));
        return MYKONOS_ERR_FLOATFRMT_INV_EXP_BITS;
    }

    /* Check for floating point hide leading ones */
    if (floatFrmt->leading > LEADING_RANGE)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_LEADING,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_LEADING));
        return MYKONOS_ERR_FLOATFRMT_INV_LEADING;
    }

    regWr1 = (floatFrmt->roundMode << ROUND_SHIFT) | (floatFrmt->dataFormat << DATA_FRMT_SHIFT) |
             (floatFrmt->encNan << ENCODE_SHIFT) | (floatFrmt->expBits << EXPBITS_SHIFT) |
             (floatFrmt->leading << LEADING_SHIFT);


    /* Write gain compensation setup to device */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_CFG, regWr1);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Floating point formatter setup function.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range
 * which increases the bit-width in the digital data-path.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - Gain compensation block has to be enabled in order to user floating point formatter
 *
 * \param device is a pointer to the device settings structure
 * \param floatFrmt which is a mykonosFloatPntFrmt_t structure.
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_STRUCT floating point formatter structure floatFrmt not initialized
 *
 */
mykonosGpioErr_t MYKONOS_getFloatPointFrmt (mykonosDevice_t *device, mykonosFloatPntFrmt_t *floatFrmt)
{
    uint8_t regRd1 = 0x00;

    /* Mask parameter values populating mykonosGainComp_t structure */
    const uint8_t ROUND_MASK = 0xE0;
    const uint8_t DATA_FRMT_MASK = 0x10;
    const uint8_t ENCODE_MASK = 0x08;
    const uint8_t EXPBITS_MASK = 0x06;
    const uint8_t LEADING_MASK = 0x01;

    /* Shift values for writing gain values */
    const uint8_t ROUND_SHIFT = 0x05;
    const uint8_t DATA_FRMT_SHIFT = 0x04;
    const uint8_t ENCODE_SHIFT = 0x03;
    const uint8_t EXPBITS_SHIFT = 0x01;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getFloatPointFrmt()\n");
#endif

    /* Check floatFrmt for Null */
    if (floatFrmt == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_STRUCT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_STRUCT));
        return MYKONOS_ERR_FLOATFRMT_NULL_STRUCT;
    }

    /* Read gain compensation setup from device */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_CFG, &regRd1);

    /* Parsing read data to passed by reference parameters */
    floatFrmt->roundMode = (regRd1 & ROUND_MASK) >> ROUND_SHIFT;
    floatFrmt->dataFormat = (regRd1 & DATA_FRMT_MASK) >> DATA_FRMT_SHIFT;
    floatFrmt->encNan = (regRd1 & ENCODE_MASK) >> ENCODE_SHIFT;
    floatFrmt->expBits = (regRd1 & EXPBITS_MASK) >> EXPBITS_SHIFT;
    floatFrmt->leading = regRd1 & LEADING_MASK;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Floating point formatter enable/disable Rx1 and Rx2 function.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range
 * which increases the bit-width in the digital data-path.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - Gain compensation block has to be enabled in order to user floating point formatter
 *
 * \param device is a pointer to the device settings structure
 * \param rx1Att this parameter sets the integer data attenuation for the Rx1 channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * \param rx2Att this parameter sets the integer data attenuation for the Rx2 channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * \param enable this parameter enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT not valid rx1 attenuation parameter passed in MYKONOS_setRxEnFloatPntFrmt()
 * \retval MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT not valid rx2 attenuation parameter passed in MYKONOS_setRxEnFloatPntFrmt()
 * \retval MYKONOS_ERR_FLOATFRMT_SET_INV_EN not valid enable parameter passed in MYKONOS_setRxEnFloatPntFrmt()
 */
mykonosGpioErr_t MYKONOS_setRxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t rx1Att, uint8_t rx2Att, uint8_t enable)
{
    uint8_t regWr = 0x00;

    const uint8_t RXATT_RANGE = 0x07;
    const uint8_t RX1ATT_SHIFT = 0x01;
    const uint8_t RX2ATT_SHIFT = 0x05;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setRxEnFloatPntFrmt()\n");
#endif

    if (enable > 1)
    {
       CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_SET_INV_EN,
                          getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_SET_INV_EN));
       return MYKONOS_ERR_FLOATFRMT_SET_INV_EN;
    }
    else if (enable == 1)
    {
        /* Check for rx1Att range */
        if (rx1Att > RXATT_RANGE)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT));
            return MYKONOS_ERR_FLOATFRMT_SET_INV_RX1ATT;
        }

        /* Check for rx2Att range */
        if (rx2Att > RXATT_RANGE)
        {
           CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT,
                   getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT));
           return MYKONOS_ERR_FLOATFRMT_SET_INV_RX2ATT;
        }

        /* Enabling floating point formatter for Rx1 and Rx2 */
        regWr = 0x11;
        regWr |= (rx1Att << RX1ATT_SHIFT) | (rx2Att << RX2ATT_SHIFT);
    }

    /* Writing floating point formatter enables for Rx1 and Rx2 */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_RX_CTRL, regWr);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Floating point formatter setup function.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range (total gain range on AD9370 is 42dB)
 * which increases the bitwidth in the digital datapath.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param rx1Att this parameter sets the integer data attenuation for the Rx1 channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * \param rx2Att this parameter sets the integer data attenuation for the Rx2 channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * \param enable this parameter enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT null pointer passed for rx1 attenuation in MYKONOS_setRxEnFloatPntFrmt()
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT null pointer passed for rx2 attenuation in MYKONOS_setRxEnFloatPntFrmt()
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_ENABLE null pointer passed for enable in MYKONOS_setRxEnFloatPntFrmt()
 */
mykonosGpioErr_t MYKONOS_getRxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t *rx1Att, uint8_t *rx2Att, uint8_t *enable)
{
    uint8_t regRd = 0x00;

    const uint8_t EN_MASK = 0x11;
    const uint8_t RX1ATT_MASK = 0x0E;
    const uint8_t RX1ATT_SHIFT = 0x01;
    const uint8_t RX2ATT_MASK = 0xE0;
    const uint8_t RX2ATT_SHIFT = 0x05;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getRxEnFloatPntFrmt()\n");
#endif

    /* Check for rx1Att NULL parameter */
    if (rx1Att == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT));
        return MYKONOS_ERR_FLOATFRMT_NULL_RX1ATT;
    }

    /* Check for rx2Att NULL parameter */
    if (rx2Att == NULL)
    {
       CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT,
               getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT));
       return MYKONOS_ERR_FLOATFRMT_NULL_RX2ATT;
    }

    /* Check for enable NULL parameter */
    if (enable == NULL)
    {
      CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_ENABLE,
              getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_ENABLE));
      return MYKONOS_ERR_FLOATFRMT_NULL_ENABLE;
    }

    /* Read Rx1 and Rx2 floating point enables */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_RX_CTRL, &regRd);

    *rx1Att = (regRd & RX1ATT_MASK) >> RX1ATT_SHIFT;
    *rx2Att = (regRd & RX2ATT_MASK) >> RX2ATT_SHIFT ;
    *enable = (regRd & EN_MASK) ? 1 : 0;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}



/**
 * \brief Floating point formatter enable function for ORx channel.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range
 * which increases the bit-width in the digital data-path.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param orxAtt this parameter sets the integer data attenuation for the ORx channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * * \param enable this parameter enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_INV_ORXATT not valid Orx attenuation parameter passed in MYKONOS_setOrxEnFloatPntFrmt()
 * \retval MYKONOS_ERR_FLOATFRMT_INV_ORXEN not valid enable parameter passed in MYKONOS_setOrxEnFloatPntFrmt()
 */
mykonosGpioErr_t MYKONOS_setOrxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t orxAtt, uint8_t enable)
{
    uint8_t regWr = 0x00;

    const uint8_t ORXATT_RANGE = 0x07;
    const uint8_t ORXATT_SHIFT = 0x01;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setOrxEnFloatPntFrmt()\n");
#endif

    /* Check for enable */
    if (enable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_ORXEN,
                          getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_ORXEN));
        return MYKONOS_ERR_FLOATFRMT_INV_ORXEN;
    }
    else if (enable == 1)
    {
        /* Check for rx1Att range */
        if (orxAtt > ORXATT_RANGE)
        {
           CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_INV_ORXATT,
                   getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_INV_ORXATT));
           return MYKONOS_ERR_FLOATFRMT_INV_ORXATT;
        }

        /* Enabling floating point formatter for Rx1 and Rx2 */
        regWr = 0x01;
        regWr |= orxAtt << ORXATT_SHIFT;
    }

    /* Writing floating point formatter enable for ORX channel */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_ORX_CTRL, regWr);

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Floating point formatter enable and setup function.
 *
 * The floating point formatter block is a function that works in conjunction with the gain
 * compensating block, as the gain compensation requires increased dynamic range (total gain range on AD9370 is 42dB)
 * which increases the bitwidth in the digital datapath.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param orxAtt this parameter sets the integer data attenuation for the ORx channel in 6dB steps
 * to enable the entire data range to be represented in the selected floating point format.
 * \param enable this parameter enables or disables the gain compensation block.
 * enable = 1
 * disable = 0
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_ORXATT null pointer passed for orxAtt
 * \retval MYKONOS_ERR_FLOATFRMT_NULL_ORXENABLE null pointer passed for enable
 */
mykonosGpioErr_t MYKONOS_getOrxEnFloatPntFrmt (mykonosDevice_t *device, uint8_t *orxAtt, uint8_t *enable)
{
    uint8_t regRd = 0x00;

    const uint8_t EN_MASK = 0x01;
    const uint8_t ORXATT_MASK = 0x0E;
    const uint8_t ORXATT_SHIFT = 0x01;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getOrxEnFloatPntFrmt()\n");
#endif

    /* Check for orxAtt NULL parameter */
    if (orxAtt == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_ORXATT,
                getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_ORXATT));
        return MYKONOS_ERR_FLOATFRMT_NULL_ORXATT;
    }

    /* Check for enable NULL parameter */
    if (enable == NULL)
    {
      CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_FLOATFRMT_NULL_ORXENABLE,
              getGpioMykonosErrorMessage(MYKONOS_ERR_FLOATFRMT_NULL_ORXENABLE));
      return MYKONOS_ERR_FLOATFRMT_NULL_ORXENABLE;
    }

    /* Read Rx1 and Rx2 floating point enables */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_FLOATING_POINT_ORX_CTRL, &regRd);

    *orxAtt = (regRd  & ORXATT_MASK) >> ORXATT_SHIFT;
    *enable = (regRd & EN_MASK) ? 1 : 0;

    /* Return */
    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief Sets up the Temperature Sensor
 *
 *  Before using this function to set up the temp sensor,
 *  make sure that you call the MYKONOS_setupAuxADC() function
 *  to ensure that the AuxADC clock is setup and running correctly at 40 MHz,
 *  and that the structure mykonosTempSensorConfig_t tempSensor is populated.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param tempSensor is a pointer to the mykonosTempSensorConfig_t which holds
 * the configuration settings for the temperature sensor
 *
 * \retval MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM Function parameter tempSensor is a NULL pointer
 * \retval MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION tempDecimation value out of range (0-7)
 * \retval MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET offset value out of range (0-255)
 * \retval MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW tempWindow out of range (0-15)
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setupTempSensor(mykonosDevice_t *device, mykonosTempSensorConfig_t *tempSensor)
{
    uint8_t offsetFromDevice = 0;

    const uint8_t OFFSET_API = 0x43;
    const uint8_t TEMP_OFFSET_MASK = 0xFF;
    const uint8_t TEMP_WINDOW_MASK = 0xF0;
    const uint8_t TEMP_WINDOW_CHECK = 0x0F;
    const uint8_t TEMP_DEC_MASK = 0x07;
    const uint8_t OFFSET_OVERRIDE_MASK = 0x10;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setupTempSensor()\n");
#endif

    /* Check for structure initialisation mykonosTempSensorConfig_t */
    if (tempSensor == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM));
        return MYKONOS_ERR_SETUPTEMPSENSOR_NULL_PARAM;
    }

    /* Check Temperature sensor decimation range */
    if ((tempSensor->tempDecimation & ~TEMP_DEC_MASK) > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION));
        return MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPDECIMATION;
    }

    /* Override fused offset */
    if (tempSensor->overrideFusedOffset > 0)
    {
        /* Check Temperature sensor offset range */
        if (tempSensor->offset & ~TEMP_OFFSET_MASK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET));
            return MYKONOS_ERR_SETUPTEMPSENSOR_INV_OFFSET;
        }

        CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONFIG, 1, OFFSET_OVERRIDE_MASK, 4);
        CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_OFFSET, tempSensor->offset);
    }
    else
    {
        /* If manual override was not requested, read back device offset value.
         * If factory fused it is non-zero; else API adds a typical offset value */
        CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_OFFSET, &(offsetFromDevice));
        if (offsetFromDevice == 0)
        {
            CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_OFFSET, OFFSET_API);
        }
    }

    /* Check Temperature sensor window range */
    if ((tempSensor->tempWindow & ~TEMP_WINDOW_CHECK) > 0)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW,
                getGpioMykonosErrorMessage(MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW));
        return MYKONOS_ERR_SETUPTEMPSENSOR_INV_TEMPWINDOW;
    }
    else
    {
        CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_LSB, tempSensor->tempWindow, TEMP_WINDOW_MASK, 4);
    }

    /* Single shot setup */
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_MSB, 0x00);

    /* Write decimation factor for the Temperature sensor */
    CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONFIG, tempSensor->tempDecimation, TEMP_DEC_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads from the Temperature Sensor registers and populates device data structure
 *
 *  After this function is executed, the tempSensor parameter holds updated values
 *  from the device registers.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param tempSensor is a pointer to the mykonosTempSensorConfig_t which holds
 * the configuration settings for the tempSensor
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_GETTEMPSENSORCONFIG_NULL_PARAM Function parameter tempSensor is a NULL pointer
 */
mykonosGpioErr_t MYKONOS_getTempSensorConfig(mykonosDevice_t *device, mykonosTempSensorConfig_t *tempSensor)
{
    uint8_t tempConfig = 0;

    const uint8_t TEMP_WINDOW_MASK = 0xF0;
    const uint8_t TEMP_FUSE_MASK = 0x10;
    const uint8_t TEMP_DECIMATION_MASK = 0x07;
    const uint8_t TEMP_FUSE_SHIFT = 0x04;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getTempSensorConfig()\n");
#endif

    /* Check tempSensor null parameter passed */
    if (tempSensor == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETTEMPSENSORCFG_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETTEMPSENSORCFG_NULL_PARAM));
        return MYKONOS_ERR_GETTEMPSENSORCFG_NULL_PARAM;
    }

    /* Read back factory offset, if fused; else manual offset value */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_OFFSET, &(tempSensor->offset));

    /* Read back Decimation and override Offset Temperature sensor parameters */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONFIG, &tempConfig);
    tempSensor->overrideFusedOffset = (tempConfig & TEMP_FUSE_MASK) >> TEMP_FUSE_SHIFT;
    tempSensor->tempDecimation = (tempConfig & TEMP_DECIMATION_MASK);

    /* Read back tempWindow */
    CMB_SPIReadField(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_LSB, &tempConfig, TEMP_WINDOW_MASK, 4);
    tempSensor->tempWindow = tempConfig;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Initiates temperature measurement
 *
 * This function will initiate the Temperature sensor measurement.
 *
 * \pre
 * MYKONOS_setupTempSensor() function to set the temperature sensor.
 * MYKONOS_setAuxAdcChannel() function with the AuxADC setting for Temperature sensor channel MYK_TEMPSENSOR
 * from the enum type mykonosAuxAdcChannels_t.
 *
 * \post
 * Internal temperature sensor will perform measurement and updated register values,
 * read back using MYKONOS_readTempSensor() function.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_startTempMeasurement(mykonosDevice_t *device)
{
    const uint8_t TEMP_LOCK_MASK = 0x80;
    const uint8_t TEMP_LOCK_BIT = 0x01;
    const uint8_t MEASUREMENT_MASK = 0x01;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_startTempMeasurement()\n");
#endif

    /* Write the lock bit to ensure that a valid measurement is achieved */
    CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONFIG, TEMP_LOCK_BIT, TEMP_LOCK_MASK, 7);

    /* Initiate Temperature measurement */
    CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_LSB, 1, MEASUREMENT_MASK, 0);
    CMB_SPIWriteField (device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_LSB, 0, MEASUREMENT_MASK, 0);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief Reads temperature code and statuses from the Temperature Sensor registers
 *
 *  This function reads the temperature sensor's statuses and temperature code.
 *  Allow at least one computation period to elapse.
 *  The temperature sensor computation period is as 6 times the AuxADC conversion time.
 *  AuxADC conversion time = (1/40Mhz) * (256 * 2^(tempDecimation))
 *
 *  Before using this function to read back the temperature, make sure that temperature sensor
 *  is setup using the MYKONOS_setupTempSensor() function.
 *
 * \pre Before using this function to read back the sensor value:
 *  MYKONOS_setupTempSensor() function is needed in order to set up the Temperature sensor.
 *  MYKONOS_setAuxAdcChannel() function has to be called in order to set the proper AuxADC
 *  channel. AuxADC channel MYK_TEMPSENSOR is used for the Temperature sensor.
 *  MYKONOS_startTempMeasurement() this function will start the Temperature sensor, this
 *  function is needed to be called for every reading that needs to be performed.
 *
 * \post "tempStatus" structure will contain the status information and the temperature
 *  code.
 *  If a valid measurement is achieved then tempStatus->tempValid will be set and
 *  tempStatus->tempCode will contain the actual reading.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param tempStatus is a pointer to the mykonosTempSensorStatus_t structure which will be updated
 * with the temp sensor readings
 *
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 * \retval MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM Function parameter tempSensor is a NULL pointer
 * \retval MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED temperature sensor reading is not locked
 */
mykonosGpioErr_t MYKONOS_readTempSensor(mykonosDevice_t *device, mykonosTempSensorStatus_t *tempStatus)
{
    int16_t temperature = 0;
    uint8_t statusWord = 0;
    uint8_t rgRd = 0x00;

    const uint8_t CODE_TO_DEGREE_CELSIUS = 0x67;
    const uint8_t TEMP_VALID_MASK = 0x02;
    const uint8_t WINDOW_EXCEED_MASK = 0x08;
    const uint8_t WINDOW_HI_LO_MASK = 0x04;
    const uint8_t TEMP_LOCK = 0x80;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_readTempSensor()\n");
#endif

    if (tempStatus == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM));
        return MYKONOS_ERR_READTEMPSENSOR_NULL_PARAM;
    }

    /* Check for temperature sensor value lock */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONFIG, &rgRd);
    if ((rgRd & TEMP_LOCK) == TEMP_LOCK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED,
                        getGpioMykonosErrorMessage(MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED));
        return MYKONOS_ERR_READTEMPSENSOR_NOT_LOCKED;
    }

    /* Perform temperature status reading */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_CONTROL_LSB, &statusWord);
    tempStatus->windowExceeded = (statusWord & WINDOW_EXCEED_MASK) >> 3;
    tempStatus->windowHiLo = (statusWord & WINDOW_HI_LO_MASK) >> 2;
    tempStatus->tempValid = (0x01 & ~((statusWord & TEMP_VALID_MASK) >> 1));

    /* Perform temperature reading */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_TEMP_SENSOR_READ, &rgRd);
    temperature = rgRd - CODE_TO_DEGREE_CELSIUS;
    tempStatus->tempCode = temperature;

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This function sets the drive strength for the required GPIOs.
 *
 * This function will set the required drive strength, the settings for GPIO drive strength are normal drive and double drive.
 *
 * GPIO pins that can be assigned independent drive strength are:
 * -MYKGPIO0 to MYKGPIO7 and MYKGPIO18
 *
 * The rest of GPIOs are paired as:
 * -MYKGPIO17 and MYKGPIO16
 * -MYKGPIO8 and MYKGPIO15
 * -MYKGPIO14 and MYKGPIO13
 * -MYKGPIO10 and MYKGPIO9
 * -MYKGPIO12 and MYKGPIO11
 *
 * This means that if it is required to double the drive strength for MYKGPIO17 then the drive strength will also be doubled for MYKGPIO16.
 *
 * If the particular bitfield for gpioDrv is set then the drive strength will be doubled. Note that multiple GPIOs can be set at a time.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioDrv GPIO drive strength setup, valid range is from 0x00000 to 0x7FFFF.
 *
 * \retval MYKONOS_ERR_GPIO_DRV_INV_PARAM GPIO out of range- valid GPIOs are in the range 0x00000 to 0x7FFFF.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioDrv(mykonosDevice_t *device, mykonosGpioSelect_t gpioDrv)
{
    uint32_t error = MYKONOS_ERR_GPIO_OK;
    uint8_t reg0 = 0x00;
    uint8_t reg1 = 0x00;

    const uint32_t GPIO_MASK = 0x7FFFF;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioDrv()\n");
#endif

    /* Error checking for correct number of GPIOs. */
    if (gpioDrv > GPIO_MASK )
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_DRV_INV_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_DRV_INV_PARAM));
        return MYKONOS_ERR_GPIO_DRV_INV_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_DRV_CTL_0, &reg0);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_DRV_CTL_1, &reg1);

    reg1 = reg1 & 0xC0;

    if (gpioDrv & 0xFF)
    {
        reg0 = (uint8_t)(gpioDrv & 0xFF);
    }

    if (gpioDrv & MYKGPIO18)
    {
        reg1 |= 0x01;
    }

    if ((gpioDrv & MYKGPIO17) || (gpioDrv & MYKGPIO16))
    {
        reg1 |= 0x02;
    }

    if ((gpioDrv & MYKGPIO15) || (gpioDrv & MYKGPIO8))
    {
        reg1 |= 0x04;
    }

    if ((gpioDrv & MYKGPIO14) || (gpioDrv & MYKGPIO13))
    {
        reg1 |= 0x08;
    }

    if ((gpioDrv & MYKGPIO10) || (gpioDrv & MYKGPIO9))
    {
        reg1 |= 0x10;
    }

    if ((gpioDrv & MYKGPIO12) || (gpioDrv & MYKGPIO11))
    {
        reg1 |= 0x20;
    }

    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_DRV_CTL_0, reg0);
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_DRV_CTL_1, reg1);

    return error;
}


/**
 * \brief This function gets the GPIOs drive strength.
 *
 * This function will get the programmed drive strength for all the GPIOs
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure.
 * \param gpioDrv will contain the current GPIOs drive strength setting.
 *
 * \retval MYKONOS_ERR_GETGPIODRV_NULL_PARAM null parameter passed to the function.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getGpioDrv(mykonosDevice_t *device, mykonosGpioSelect_t *gpioDrv)
{
    uint32_t error = MYKONOS_ERR_GPIO_OK;
    uint8_t reg0 = 0x00;
    uint8_t reg1 = 0x00;
    uint32_t gpioDrvRd = 0x00;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_getGpioDrv()\n");
#endif

    /* Error checking for correct number of GPIOs. */
    if (gpioDrv == NULL )
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GETGPIODRV_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GETGPIODRV_NULL_PARAM));
        return MYKONOS_ERR_GETGPIODRV_NULL_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_DRV_CTL_0, &reg0);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_DRV_CTL_1, &reg1);

    gpioDrvRd = (uint32_t)reg0 & 0xFF;

    if (reg1 & 0x01)
    {
        gpioDrvRd |= MYKGPIO18;
    }

    if (reg1 & 0x02)
    {
        gpioDrvRd |= MYKGPIO17 | MYKGPIO16;
    }

    if (reg1 & 0x04)
    {
        gpioDrvRd |= MYKGPIO8 | MYKGPIO15;
    }

    if (reg1 & 0x08)
    {
        gpioDrvRd |= MYKGPIO14 | MYKGPIO13;
    }

    if (reg1 & 0x10)
    {
        gpioDrvRd |= MYKGPIO10 | MYKGPIO9;
    }

    if (reg1 & 0x20)
    {
        gpioDrvRd |= MYKGPIO12 | MYKGPIO11;
    }

    *gpioDrv = gpioDrvRd;

    return error;
}

/**
 * \brief This function sets the slew rate for the required GPIOs.
 *
 * This function will set the required slew rate, the settings for GPIO slew rate are given by the enumeration type mykonosGpioSlewRate_t.
 *
 * GPIO pins that can be assigned independent slew rate are:
 * -MYKGPIO0 to MYKGPIO7 and MYKGPIO18
 *
 * The rest of GPIOs are paired as:
 * -MYKGPIO17 and MYKGPIO16
 * -MYKGPIO8 and MYKGPIO15
 * -MYKGPIO14 and MYKGPIO13
 * -MYKGPIO10 and MYKGPIO9
 * -MYKGPIO12 and MYKGPIO11
 *
 * This means that if it is required to set the slew rate for MYKGPIO17 then the same slew rate will be applied for MYKGPIO16.
 *
 * If the particular bitfield for gpioSelect is set then slewRate will be set for that GPIO. Note that multiple GPIOs can be set at a time.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param gpioSelect GPIO for which the slew rate will be changed, valid range 0x00000 to 0x7FFFF.
 * \param slewRate slew rate setting, valid slew rate settings are given by the enumeration type mykonosGpioSlewRate_t
 *
 * \retval MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM GPIO out of range- valid GPIOs are in the range 0x00000 to 0x7FFFF.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setGpioSlewRate(mykonosDevice_t *device, mykonosGpioSelect_t gpioSelect, mykonosGpioSlewRate_t slewRate)
{
    uint32_t error = MYKONOS_ERR_GPIO_OK;
    uint8_t reg0 = 0x00;
    uint8_t reg1 = 0x00;
    uint8_t reg2 = 0x00;
    uint8_t reg3 = 0x00;

    const uint32_t GPIO_MASK = 0x7FFFF;
    const uint8_t SLEWRATE_MASK = 0x03;
    const uint32_t SHIFT4NIBLE = 0x06;
    const uint32_t SHIFT3NIBLE = 0x04;
    const uint32_t SHIFT2NIBLE = 0x02;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioSlewRate()\n");
#endif

    /* Error checking for correct number of GPIOs. */
    if ((gpioSelect > GPIO_MASK) || (slewRate > SLEWRATE_MASK))
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM));
        return MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_0, &reg0);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_1, &reg1);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_2, &reg2);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_3, &reg3);

    if (gpioSelect & MYKGPIO18)
    {
        reg0 = (reg0 & ~(SLEWRATE_MASK << SHIFT4NIBLE)) | (slewRate << SHIFT4NIBLE);
    }
    if ((gpioSelect & MYKGPIO17) || (gpioSelect & MYKGPIO16))
    {
        reg0 = (reg0 & ~(SLEWRATE_MASK << SHIFT3NIBLE)) | (slewRate << SHIFT3NIBLE);
    }
    if ((gpioSelect & MYKGPIO15) || (gpioSelect & MYKGPIO8))
    {
        reg0 = (reg0 & ~(SLEWRATE_MASK << SHIFT2NIBLE)) | (slewRate << SHIFT2NIBLE);
    }
    if ((gpioSelect & MYKGPIO14) || (gpioSelect & MYKGPIO13))
    {
        reg0 = (reg0 & ~(SLEWRATE_MASK)) | slewRate;
    }
    if ((gpioSelect & MYKGPIO11) || (gpioSelect & MYKGPIO12))
    {
        reg1 = (reg1 & ~(SLEWRATE_MASK << SHIFT4NIBLE)) | (slewRate << SHIFT4NIBLE);
    }
    if ((gpioSelect & MYKGPIO9) || (gpioSelect & MYKGPIO10))
    {
        reg1 = (reg1 & ~(SLEWRATE_MASK << SHIFT3NIBLE)) | (slewRate << SHIFT3NIBLE);
    }
    if (gpioSelect & MYKGPIO7)
    {
        reg1 = (reg1 & ~(SLEWRATE_MASK << SHIFT2NIBLE)) | (slewRate << SHIFT2NIBLE);
    }
    if (gpioSelect & MYKGPIO6)
    {
        reg1 = (reg1 & ~(SLEWRATE_MASK)) | slewRate;
    }
    if (gpioSelect & MYKGPIO5)
    {
        reg2 = (reg2 & ~(SLEWRATE_MASK << SHIFT4NIBLE)) | (slewRate << SHIFT4NIBLE);
    }
    if (gpioSelect & MYKGPIO4)
    {
        reg2 = (reg2 & ~(SLEWRATE_MASK << SHIFT3NIBLE)) | (slewRate << SHIFT3NIBLE);
    }
    if (gpioSelect & MYKGPIO3)
    {
        reg2 = (reg2 & ~(SLEWRATE_MASK << SHIFT2NIBLE)) | (slewRate << SHIFT2NIBLE);
    }
    if (gpioSelect & MYKGPIO2)
    {
        reg2 = (reg2 & ~(SLEWRATE_MASK)) | slewRate;
    }
    if (gpioSelect & MYKGPIO1)
    {
        reg3 = (reg3 & ~(SLEWRATE_MASK << SHIFT2NIBLE)) | (slewRate << SHIFT2NIBLE);
    }
    if (gpioSelect & MYKGPIO0)
    {
        reg3 = (reg3 & ~(SLEWRATE_MASK)) | slewRate;
    }

    /* Prepare registers */
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_SLEW_CTL_0, reg0);
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_SLEW_CTL_1, reg1);
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_SLEW_CTL_2, reg2);
    CMB_SPIWriteByte (device->spiSettings, MYKONOS_GPIO_SLEW_CTL_3, reg3);

    return error;
}


/**
 * \brief This function gets the GPIO slew rate setting.
 *
 * This function will get the programmed slew rate for the GPIO given by slewRateGpio.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure.
 * \param gpioSelect GPIO selection for getting the slew rate setting.
 * \param slewRate will contain the GPIO slew rate setting.
 *
 * \retval MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM null parameter passed to the function.
 * \retval MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM GPIO out of range- valid GPIOs are in the range 0x00000 to 0x7FFFF.
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getGpioSlewRate(mykonosDevice_t *device, mykonosGpioSelect_t gpioSelect, mykonosGpioSlewRate_t *slewRate)
{
    uint8_t reg0 = 0x00;
    uint8_t reg1 = 0x00;
    uint8_t reg2 = 0x00;
    uint8_t reg3 = 0x00;
    mykonosGpioSlewRate_t slewSetting = MYK_SLEWRATE_NONE;

    const uint32_t SLEWRATE_MASK = 0x03;
    const uint32_t SHIFT4NIBLE = 0x06;
    const uint32_t SHIFT3NIBLE = 0x04;
    const uint32_t SHIFT2NIBLE = 0x02;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioSlewRate()\n");
#endif

    /* Error checking for NULL parameter. */
    if (slewRate == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM));
        return MYKONOS_ERR_GPIO_GETSLEW_NULL_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_0, &reg0);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_1, &reg1);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_2, &reg2);
    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_3, &reg3);

    switch (gpioSelect)
    {
        case MYKGPIO18:
            slewSetting = (reg0 & (SLEWRATE_MASK << SHIFT4NIBLE)) >> SHIFT4NIBLE;
            break;
        case MYKGPIO17:
        case MYKGPIO16:
            slewSetting = (reg0 & (SLEWRATE_MASK << SHIFT3NIBLE)) >> SHIFT3NIBLE;
            break;
        case MYKGPIO15:
        case MYKGPIO8:
            slewSetting = (reg0 & (SLEWRATE_MASK << SHIFT2NIBLE)) >> SHIFT2NIBLE;
            break;
        case MYKGPIO14:
        case MYKGPIO13:
            slewSetting = (reg0 & (SLEWRATE_MASK));
            break;
        case MYKGPIO11:
        case MYKGPIO12:
            slewSetting = (reg1 & (SLEWRATE_MASK << SHIFT4NIBLE)) >> SHIFT4NIBLE;
            break;
        case MYKGPIO9:
        case MYKGPIO10:
            slewSetting = (reg1 & (SLEWRATE_MASK << SHIFT3NIBLE)) >> SHIFT3NIBLE;
            break;
        case MYKGPIO7:
            slewSetting = (reg1 & (SLEWRATE_MASK << SHIFT2NIBLE)) >> SHIFT2NIBLE;
            break;
        case MYKGPIO6:
            slewSetting = (reg1 & (SLEWRATE_MASK));
            break;
        case MYKGPIO5:
            slewSetting = (reg2 & (SLEWRATE_MASK << SHIFT4NIBLE)) >> SHIFT4NIBLE;
            break;
        case MYKGPIO4:
            slewSetting = (reg2 & (SLEWRATE_MASK << SHIFT3NIBLE)) >> SHIFT3NIBLE;
            break;
        case MYKGPIO3:
            slewSetting = (reg2 & (SLEWRATE_MASK << SHIFT2NIBLE)) >> SHIFT2NIBLE;
            break;
        case MYKGPIO2:
            slewSetting = (reg2 & (SLEWRATE_MASK));
            break;
        case MYKGPIO1:
            slewSetting = (reg3 & (SLEWRATE_MASK << SHIFT2NIBLE)) >> SHIFT2NIBLE;
            break;
        case MYKGPIO0:
            slewSetting = (reg3 & (SLEWRATE_MASK));
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM,
                    getGpioMykonosErrorMessage(MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM));
            return MYKONOS_ERR_GPIO_SLEW_RATE_INV_PARAM;
    }

    *slewRate = slewSetting;

    return MYKONOS_ERR_GPIO_OK;
}


/**
 * \brief This function sets the CMOS output driver.
 *
 * This function sets the CMOS output driver to increase pad drive strength for the SPI_DIO and GP_INTERRUPT pads
 * according to the following settings of the enumeration type mykonosCmosPadDrvStr_t
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param cmosDrv drive for the CMOS output driver
 *
 * \retval MYKONOS_ERR_CMOS_DRV_INV_PARAM invalid drive strength, valid settings are given by mykonosCmosPadDrvStr_t
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_setCmosDrv(mykonosDevice_t *device, mykonosCmosPadDrvStr_t cmosDrv)
{
    const uint8_t CMOS_DRV_MASK = 0x0F;
    const uint8_t CMOS_DRV_SHIFT = 0x04;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setSpiDrv()\n");
#endif

    /* Error checking for correct drive */
    switch (cmosDrv)
    {   /* fall through since this is only checking */
        case MYK_CMOSPAD_DRV_1X:
        case MYK_CMOSPAD_DRV_2X:
        case MYK_CMOSPAD_DRV_3X:
        case MYK_CMOSPAD_DRV_4X:
        case MYK_CMOSPAD_DRV_5X:
            break;
        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CMOS_DRV_INV_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_CMOS_DRV_INV_PARAM));
            return MYKONOS_ERR_CMOS_DRV_INV_PARAM;
            break;
    }

    CMB_SPIWriteField(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_3, cmosDrv, (~CMOS_DRV_MASK), CMOS_DRV_SHIFT);

    return MYKONOS_ERR_GPIO_OK;
}

/**
 * \brief This function gets the programmed CMOS output driver
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device is a pointer to the device settings structure
 * \param cmosDrv will be the programmed drive for the CMOS output driver
 *
 * \retval MYKONOS_ERR_CMOS_DRV_NULL_PARAM Null parameter passed to the function
 * \retval MYKONOS_ERR_GPIO_OK Function completed successfully
 */
mykonosGpioErr_t MYKONOS_getCmosDrv(mykonosDevice_t *device, mykonosCmosPadDrvStr_t *cmosDrv)
{
    uint8_t regRd = 0x00;

    const uint8_t CMOS_DRV_MASK = 0xF0;
    const uint8_t CMOS_DRV_SHIFT = 0x04;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_GPIO_OK, "MYKONOS_setGpioSlewRate()\n");
#endif

    if (cmosDrv == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_CMOS_DRV_NULL_PARAM,
                getGpioMykonosErrorMessage(MYKONOS_ERR_CMOS_DRV_NULL_PARAM));
        return MYKONOS_ERR_CMOS_DRV_NULL_PARAM;
    }

    CMB_SPIReadByte(device->spiSettings, MYKONOS_GPIO_SLEW_CTL_3, &regRd);

    *cmosDrv = ((regRd & CMOS_DRV_MASK) >> CMOS_DRV_SHIFT);

    return MYKONOS_ERR_GPIO_OK;
}

/**
* \brief This API function configures and enables the secondary SPI port.
*
* This port allows control compatibility with BBPs that employ dual SPI ports.
* The GPIO mapping for the SPI2 is fixed:
*  SPI signal  |   GPIO
* -------------|-------------------
*  CSB_2       |   GPIO 3
*  SCLK_2      |   GPIO 2
*  SDO_2       |   GPIO 1
*  SDO_2/SDI2  |   GPIO 0
*
* The secondary SPI port only has a small subset of registers that affect the
* attenuation of the TX channels. It uses a fifth GPIO pin in order to decide
* which TxAttenuation word is active. The Tx Attenuation words in the 2nd SPI
* port are different than the first SPI port.  On the 2nd SPI port, each
* transmitter has two Tx Attenuation words (an active word and an inactive
* word).  The BBIC should write to the inactive TxAttenuation word. Then the
* fifth GPIO pin should be asserted to make the inactive TxAttenuation word
* active.  This allows the BBIC to tightly control in real time when the
* TxAttenuation updates.
*
* <B>Dependencies</B>
* - device->spiSettings
* - device->spiSettings->chipSelectIndex
*
* \param device is a pointer to the device settings structure
* \param enable This is parameter will enable the secondary SPI port: 1 = enable, 0 = disable
* \param updateTxAttenPinSelect This parameter set the GPIO pin to be toggle for using the inactive attenuation words for both channels
*  tx update  |   GPIO
* ------------|-------------------
*     0x00    |   GPIO 4
*     0x01    |   GPIO 8
*     0x02    |   GPIO 14
*     0x03    |   none selected
*
* \retval MYKONOS_ERR_SPI2_INV_GPIO if an invalid GPIO pin configuration is passed
* \retval MYKONOS_ERR_HAL_LAYER if HAL function error is passed
* \retval MYKONOS_ERR_OK Function completed successfully
*/
mykonosGpioErr_t MYKONOS_spi2GpioSetup(mykonosDevice_t *device, uint8_t enable, uint8_t updateTxAttenPinSelect)
{
    uint8_t regWrite = 0;
    uint8_t enableBit = 0;
    mykonosErr_t error = MYKONOS_ERR_OK;
    uint32_t halError = COMMONERR_OK;

    const uint32_t SPI2_PIN_MASK = 0x03;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_OK, "MYKONOS_txGpioAttControl()\n");
#endif

    /* Error checking for correctness of GPIO to control attenuation word. */
    if (updateTxAttenPinSelect & ~SPI2_PIN_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex,  MYKONOS_ERR_SPI2_INV_GPIO,
                getMykonosErrorMessage(MYKONOS_ERR_SPI2_INV_GPIO));
        return MYKONOS_ERR_SPI2_INV_GPIO;
    }

    /* masking the enable bit and the required pin */
    enableBit = (enable > 0) ? 1 : 0;
    regWrite = (updateTxAttenPinSelect << 4) | (enableBit << 3);

    /* Set the SPI2 enable and the GPIO pin associated. */
    halError = CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_CONFIGURATION_CONTROL_1, regWrite, 0x38, 0);
    if (halError)
    {
        return MYKONOS_ERR_HAL_LAYER;
    }

    return error;
}

