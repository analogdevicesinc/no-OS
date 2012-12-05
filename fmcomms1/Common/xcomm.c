/**************************************************************************//**
*   @file   xcomm.c
*   @brief  XCOMM interface functions implementation.
*   @author acozma (andrei.cozma@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>
#include "AD9548.h"
#include "AD9523.h"
#include "ADF4351.h"
#include "AD9122.h"
#include "AD9643.h"
#include "AD8366.h"
#include "eeprom.h"
#include "spi_interface.h"
#include "adc_core.h"
#include "xcomm.h"

/****** Global variables ******/
struct fmcomms1_calib_data XCOMM_calData[16];
uint8_t XCOMM_calDataSize;

/****** XCOMM state structure ******/
struct stXCOMM_State
{
    /* Rx state variables */
    int64_t rxFreq;
    int8_t  rxFreqValid;    
    int32_t rxResolution;
    int8_t  rxResolutionValid;    
    int32_t rxGain;
    int8_t  rxGainValid;    
    XCOMM_RxIQCorrection rxIqCorrection;
    int8_t rxIqCorrectionValid;    

    /* Tx state variables */
    int64_t txFreq;
    int8_t  txFreqValid;
    int32_t txResolution;
    int8_t  txResolutionValid;
    XCOMM_TxIQCorrection txIqCorrection;
    int8_t txIqCorrectionValid;

    /* ADC state variables */
    int64_t adcSampleRate;
    int8_t  adcSampleRateValid;
    XCOMM_AdcTestMode adcTestMode;
    int8_t  adcTestModeValid;

    /* DAC state variables */
    int64_t dacSampleRate;
    int8_t  dacSampleRateValid;
    XCOMM_DacIQCorrection dacIQCorrection;
    int8_t  dacIqCorrectionValid;

    /* FRU state variables */
    uint8_t fruData[256];
    uint8_t fruDataValid;

}XCOMM_State;

/**************************************************************************//**
* @brief Initializes the XCOMM board
*
* @param pDefInit - pointer to initialization structure
*
* @return If success, return 0
*         if error, return -1
******************************************************************************/
int32_t XCOMM_Init(XCOMM_DefaultInit* pDefInit)
{

    /* Reset the XCOMM state variables */
    int32_t i = 0;
    int8_t* pData = (int8_t*)&XCOMM_State;
    for(i = 0; i < sizeof(XCOMM_State); i++)
    {
        pData[i] = 0;
    }

    /* Initialize the SPI communication */
    if(SPI_Init() < 0)
    	return -1;

    /* Initialize the AD9548 */
    if(ad9548_setup() < 0)
        return -1;

	/* Initialize the AD9523 */
    if(ad9523_setup() < 0)
        return -1;

    if(XCOMM_SetAdcSamplingRate(pDefInit->adcSamplingRate) < 0)
        return -1;

    if(XCOMM_SetDacSamplingRate(pDefInit->dacSamplingRate) < 0)
        return -1;
	
	/* Initialize the Rx ADF4351 */
    if(adf4351_setup(ADF4351_RX_CHANNEL) < 0)
        return -1;

    if(XCOMM_SetRxFrequency(pDefInit->rxFrequency) < 0)
        return -1;

	/* Initialize the Tx ADF4351 */
    if(adf4351_setup(ADF4351_TX_CHANNEL) < 0)
        return -1;

    if(XCOMM_SetTxFrequency(pDefInit->txFrequency) < 0)
        return -1;
    
	/* Initialize the AD9122 */
    if(ad9122_setup() < 0)
        return -1;

	/* Initialize the AD9643 */
    if(ad9643_setup() < 0)
        return -1;

	/* Initialize the AD8366 */
    if(ad8366_setup() < 0)
        return -1;

    if(XCOMM_SetRxGain(pDefInit->rxGain1000) < 0)
        return -1;

    /* Read the calibration data from the EEPROM */
    if(EEPROM_GetCalData((uint8_t*)XCOMM_calData, &XCOMM_calDataSize) < 0)
        return -1;

    return 0;
}

/**************************************************************************//**
* @brief Resync driver cached values by reading the XCOMM board 
*
* @return Returns 0 in case of success or -1 if error
******************************************************************************/
int32_t XCOMM_Sync(void)
{
    /* Resync Rx state variables */
    XCOMM_GetRxResolution(XCOMM_ReadMode_FromHW);
    XCOMM_GetRxGain(XCOMM_ReadMode_FromHW);
    XCOMM_GetRxIqCorrection(0, XCOMM_ReadMode_FromHW);
    
    /* Resync Tx state variables */
    XCOMM_GetTxResolution(XCOMM_ReadMode_FromHW);
    XCOMM_GetTxIqCorrection(0, XCOMM_ReadMode_FromHW);

    /* Resync ADC state variables */
    XCOMM_GetAdcSamplingRate(XCOMM_ReadMode_FromHW);
    XCOMM_GetAdcTestMode(XCOMM_ReadMode_FromHW);

    /* Resync DAC state variables */
    XCOMM_GetDacSamplingRate(XCOMM_ReadMode_FromHW);
    XCOMM_GetDacIqCorrection(XCOMM_ReadMode_FromHW);

    return 0;
}

/**************************************************************************//**
* @brief Gets the XCOMM board version string
*
* @param readMode - Read version from driver or HW
*
* @return If success, return version struct with version string and error set to 0
          If error, return version struct with error set to -1
******************************************************************************/
XCOMM_Version XCOMM_GetBoardVersion(XCOMM_ReadMode readMode)
{
    int32_t ret;
    uint8_t len;
    uint8_t idx = 0;
    uint8_t* ptr = XCOMM_State.fruData;
    XCOMM_Version ver;

    ver.error = -1;

    /* Read the FRU data */
    if((!XCOMM_State.fruDataValid) || (readMode == XCOMM_ReadMode_FromHW))
    {
        ret = EEPROM_Read(IICSEL_FRU, 0x00, XCOMM_State.fruData, 255);
        if((ret < 0) || (XCOMM_State.fruData[0] != 0x01))
            return ver;
        XCOMM_State.fruDataValid = 1;
    }

    /* Move to the Board Area offset from the FRU */
    ptr += ptr[3] * 8 + 6;

    /* Read the Board Manufacturer */
    len = *ptr & 0x3F;
    ptr++;
    while(len--)
    {
        ver.value[idx++] = *ptr;
        ptr++;
        if(!len)
        {
            ver.value[idx++] = ',';
            ver.value[idx++] = ' ';
        }
    }    

    /* Read the Board Product Name */
    len = *ptr & 0x3F;
    ptr++;
    while(len--)
    {
        ver.value[idx++] = *ptr;
        ptr++;
        if(!len)
        {
            ver.value[idx++] = ',';
            ver.value[idx++] = ' ';
        }
    }

    /* Read the Board Serial Number */
    len = *ptr & 0x3F;
    ptr++;
    while(len--)
    {
        ver.value[idx++] = *ptr;
        ptr++;
        if(!len)
        {
            ver.value[idx++] = ',';
            ver.value[idx++] = ' ';
        }
    }

    /* Read the Board Part Number */
    len = *ptr & 0x3F;
    ptr++;
    while(len--)
    {
        ver.value[idx++] = *ptr;
        ptr++;
        if(!len)
        {
            ver.value[idx++] = ',';
            ver.value[idx++] = ' ';
        }
    }

    /* Read the Board Revision */
    len = *ptr & 0x3F;
    ptr += 1 + len;
    len = *ptr & 0x3F;
    ptr++;
    if(*ptr == 0)
    {
        ptr++;
        len--;
        if(len)
        {
            ver.value[idx++] = 'R';
            ver.value[idx++] = 'e';
            ver.value[idx++] = 'v';
            ver.value[idx++] = '.';
        }
        while(len--)
        {
            ver.value[idx++] = *ptr;
            ptr++;
        }
    }

    ver.value[idx] = 0;
    ver.error = 0;

    return ver;
}

/**************************************************************************//**
* @brief Gets the PIC firmware version
*
* @return If success, returns the PIC firmware version number - 0 to 100
*		  If error returns -1
******************************************************************************/
int32_t XCOMM_GetPicFwVersion()
{
	return PIC_ReadFwVersion();
}

/************************ Rx Functions ***************************************/


/**************************************************************************//**
* @brief Sets the Rx center frequency
*
* @param frequency - desired frequency value in Hz
*
* @return If success, return exact calculated frequency in Hz
*         if error, return -1
******************************************************************************/
int64_t XCOMM_SetRxFrequency(uint64_t frequency)
{
    int64_t freq = adf4351_out_altvoltage0_frequency(frequency, ADF4351_RX_CHANNEL);    
    if(freq < 0)
        return -1;

    XCOMM_State.rxFreq = freq;
    XCOMM_State.rxFreqValid = 1;

    return XCOMM_State.rxFreq;
}

/**************************************************************************//**
* @brief Gets the Rx center frequency 
*
* @return If success, return frequency in Hz stored in driver,
*         if error, return -1
******************************************************************************/
int64_t XCOMM_GetRxFrequency(void)
{
    return XCOMM_State.rxFreqValid ? XCOMM_State.rxFreq : -1;
}

/**************************************************************************//**
* @brief Sets the Rx center frequency resolution
*
* @param resolution: desired frequency resolution in Hz
*  
* @return If success, return exact calculated resolution in Hz,
*         if error, return -1
******************************************************************************/
int32_t XCOMM_SetRxResolution(uint32_t resolution)
{
    int32_t res = adf4351_out_altvoltage0_frequency_resolution(resolution, 
                                                               ADF4351_RX_CHANNEL);
    if(res < 0)
        return -1;

    XCOMM_State.rxResolution = res;
    XCOMM_State.rxResolutionValid = 1;

    return XCOMM_State.rxResolution;
}

/**************************************************************************//**
* @brief Gets the Rx center frequency resolution
*
* @param readMode: read frequency resolution from driver or HW
*
* @return If success, return frequency resolution in Hz,
*         if error, return -1
******************************************************************************/
int32_t XCOMM_GetRxResolution(XCOMM_ReadMode readMode)
{
    int32_t res;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        res = adf4351_out_altvoltage0_frequency_resolution(INT32_MAX, 
                                                           ADF4351_RX_CHANNEL);
        if(res < 0)
            return -1;
        
        XCOMM_State.rxResolution = res;
        XCOMM_State.rxResolutionValid = 1;
    }

    return XCOMM_State.rxResolutionValid ? XCOMM_State.rxResolution : -1;
}

/**************************************************************************//**
* @brief Sets the Rx gain
*
* @param gain1000: desired gain (x1000) in dB
*
* @return If success, return calculated gain (x1000) in dB,
*         if error, return -1
******************************************************************************/
int32_t XCOMM_SetRxGain(int32_t gain1000)
{
    int32_t retGain = 0;

    retGain = ad8366_out_voltage0_hardwaregain(gain1000);
    retGain = ad8366_out_voltage1_hardwaregain(gain1000);
    if(retGain < 0)
        return -1;
    
    XCOMM_State.rxGain = retGain;
    XCOMM_State.rxGainValid = 1;
    
    return XCOMM_State.rxGain;
}

/**************************************************************************//**
* @brief Gets the Rx gain
*
* @param readMode: read gain from driver or HW
*
* @return If success, return gain (x1000) in dB
*         if error, return -1
******************************************************************************/
int32_t XCOMM_GetRxGain(XCOMM_ReadMode readMode)
{
    int32_t gain;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        gain = ad8366_out_voltage0_hardwaregain(2 * (int32_t)AD8366_MAX_GAIN * 1000);
        if(gain < 0)
            return -1;        
        XCOMM_State.rxGain = gain;
        XCOMM_State.rxGainValid = 1;
    }

    return XCOMM_State.rxGainValid ? XCOMM_State.rxGain : -1;
}

/**************************************************************************//**
* @brief Gets the Rx gain and phase correction for I and Q
*
* @param frequency: center frequency used for the correction in Hz
* @param readMode: read gain and phase correction from driver or HW
*
* @return If success, return RxIQCorrection struct with error set to 0
*         If error, return RxIQCorrection struct with error set to -1
******************************************************************************/
XCOMM_RxIQCorrection XCOMM_GetRxIqCorrection(uint64_t frequency, XCOMM_ReadMode readMode)
{
    uint8_t idx       = 0;
    uint8_t calIdx    = 0;
    uint32_t delta    = 0;
    int32_t signeddelta    = 0;
    uint32_t minDelta = UINT32_MAX;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        EEPROM_GetCalData((uint8_t*)XCOMM_calData, &XCOMM_calDataSize);
    }

    if(!XCOMM_calDataSize)
    {
        XCOMM_State.rxIqCorrection.error = -1;
    }
    else
    {
        frequency /= 1000000;
        while(idx < XCOMM_calDataSize)
        {
            signeddelta = (int32_t)(frequency - XCOMM_calData[idx].cal_frequency_MHz);
            delta = (uint32_t)(signeddelta>0?signeddelta:-signeddelta);
            if(delta < minDelta)
            {
                minDelta = delta;
                calIdx = idx;
            }
            idx++;
        }
        if ((XCOMM_calData[calIdx].adi_magic0 != ADI_MAGIC_0) ||
            (XCOMM_calData[calIdx].adi_magic1 != ADI_MAGIC_1) /*||
            (XCOMM_calData[calIdx].version != ADI_VERSION(VERSION_SUPPORTED)*/)
        {
            XCOMM_State.rxIqCorrection.error = -1;
        }
        else
        {
            XCOMM_State.rxIqCorrection.gainI = XCOMM_calData[calIdx].i_adc_gain_adj;
            XCOMM_State.rxIqCorrection.offsetI = XCOMM_calData[calIdx].i_adc_offset_adj;
            XCOMM_State.rxIqCorrection.gainQ = XCOMM_calData[calIdx].q_adc_gain_adj;
            XCOMM_State.rxIqCorrection.offsetQ = XCOMM_calData[calIdx].q_adc_offset_adj;
            XCOMM_State.rxIqCorrection.error = 0;
        }
    }
    
    return XCOMM_State.rxIqCorrection;
}

/************************ Tx Functions ***************************************/


/**************************************************************************//**
* @brief Sets the Tx center frequency
*
* @param frequency: desired frequency in Hz
*
* @return If success, return calculated frequency in Hz
*         if error, return -1
******************************************************************************/
int64_t XCOMM_SetTxFrequency(uint64_t frequency)
{
    int64_t freq = adf4351_out_altvoltage0_frequency(frequency, ADF4351_TX_CHANNEL);

    if(freq < 0)
        return -1;

    XCOMM_State.txFreq = freq;
    XCOMM_State.txFreqValid = 1;

    return XCOMM_State.txFreq;
}

/**************************************************************************//**
* @brief Gets the Tx center frequency 
*
* @return If success, return frequency in Hz stored in driver,
*         if error, return -1
******************************************************************************/
int64_t XCOMM_GetTxFrequency(void)
{
    return XCOMM_State.txFreqValid ? XCOMM_State.txFreq : -1;
}

/**************************************************************************//**
* @brief Sets the Tx center frequency resolution
*
* @param resolution: desired frequency resolution in Hz
*  
* @return If success, return exact calculated resolution in Hz,
*         if error, return -1
******************************************************************************/
int32_t XCOMM_SetTxResolution(uint32_t resolution)
{
    int32_t res = adf4351_out_altvoltage0_frequency_resolution(resolution, 
                                                               ADF4351_TX_CHANNEL);
    if(res < 0)
        return -1;

    XCOMM_State.txResolution = res;
    XCOMM_State.txResolutionValid = 1;

    return XCOMM_State.txResolution;
}

/**************************************************************************//**
* @brief Gets the Tx center frequency resolution
*
* @param readMode: read frequency resolution from driver or HW
*
* @return If success, return frequency resolution in Hz,
*         if error, return -1
******************************************************************************/
int32_t XCOMM_GetTxResolution(XCOMM_ReadMode readMode)
{
    int32_t res;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        res = adf4351_out_altvoltage0_frequency_resolution(INT32_MAX, 
                                                           ADF4351_TX_CHANNEL);
        if(res < 0)
            return -1;
        
        XCOMM_State.txResolution = res;
        XCOMM_State.txResolutionValid = 1;
    }

    return XCOMM_State.txResolutionValid ? XCOMM_State.txResolution : -1;
}

/**************************************************************************//**
* @brief Gets the Tx gain and phase correction for I and Q
*
* @param frequency: center frequency used for the correction in Hz
* @param readMode: read gain and phase correction from driver or HW
*
* @return If success, return TxIQCorrection struct with error set to 0
*         If error, return TxIQCorrection struct with error set to -1
******************************************************************************/
XCOMM_TxIQCorrection XCOMM_GetTxIqCorrection(uint64_t frequency, XCOMM_ReadMode readMode)
{
    uint8_t idx       = 0;
    uint8_t calIdx    = 0;
    uint32_t delta    = 0;
    int32_t signeddelta    = 0;
    uint32_t minDelta = UINT32_MAX;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        EEPROM_GetCalData((uint8_t*)XCOMM_calData, &XCOMM_calDataSize);
    }

    if(!XCOMM_calDataSize)
    {
        XCOMM_State.txIqCorrection.error = -1;
    }
    else
    {
        frequency /= 1000000;
        while(idx < XCOMM_calDataSize)
        {
            signeddelta = (int32_t)(frequency - XCOMM_calData[idx].cal_frequency_MHz);
            delta = (uint32_t)(signeddelta>0?signeddelta:-signeddelta);
            if(delta < minDelta)
            {
                minDelta = delta;
                calIdx = idx;
            }
            idx++;
        }
        if ((XCOMM_calData[calIdx].adi_magic0 != ADI_MAGIC_0) ||
            (XCOMM_calData[calIdx].adi_magic1 != ADI_MAGIC_1) /*||
            (XCOMM_calData[calIdx].adi_magic1 != ADI_VERSION(VERSION_SUPPORTED)*/)
        {
            XCOMM_State.txIqCorrection.error = -1;
        }
        else
        {
            XCOMM_State.txIqCorrection.phaseAdjI = XCOMM_calData[calIdx].i_phase_adj;
            XCOMM_State.txIqCorrection.offsetI   = XCOMM_calData[calIdx].i_dac_offset;
            XCOMM_State.txIqCorrection.fsAdjI    = XCOMM_calData[calIdx].i_dac_fs_adj;
            
            XCOMM_State.txIqCorrection.phaseAdjQ = XCOMM_calData[calIdx].q_phase_adj;
            XCOMM_State.txIqCorrection.offsetQ   = XCOMM_calData[calIdx].q_dac_offset;
            XCOMM_State.txIqCorrection.fsAdjQ    = XCOMM_calData[calIdx].q_dac_fs_adj;

            XCOMM_State.txIqCorrection.error = 0;
        }
    }
    
    return XCOMM_State.txIqCorrection;
}

/************************ ADC Functions ***************************************/


/**************************************************************************//**
* @brief Sets the sampling rate of the ADC
*
* @param rate: desired rate in Hz
*
* @return  If success, return exact calculated rate in Hz
*          if error, return -1
******************************************************************************/
int64_t XCOMM_SetAdcSamplingRate(uint64_t rate)
{
    int64_t sampleRate = ad9523_out_altvoltage2_ADC_CLK_frequency(rate);

    if(sampleRate < 0)
        return -1;

    XCOMM_State.adcSampleRate = sampleRate;
    XCOMM_State.adcSampleRateValid = 1;

    return XCOMM_State.adcSampleRate;
}

/**************************************************************************//**
* @brief Gets the sampling rate of the ADC
*
* @param readMode: read rate from driver or HW
*
* @return If success, return rate in Hz
*         if error, return -1
******************************************************************************/
int64_t XCOMM_GetAdcSamplingRate(XCOMM_ReadMode readMode)
{
    int64_t sampleRate;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        sampleRate = ad9523_out_altvoltage2_ADC_CLK_frequency(INT64_MAX);
        if(sampleRate < 0)
            return -1;
        
        XCOMM_State.adcSampleRate = sampleRate;
        XCOMM_State.adcSampleRateValid = 1;
    }

    return XCOMM_State.adcSampleRateValid ? XCOMM_State.adcSampleRate : -1;
}

/**************************************************************************//**
* @brief Sets the test mode of the ADC
*
* @param testMode - desired ADC test mode
*
* @param channel - ADC channel to set the test mode for
*
* @return If success, return new ADC test mode if success
*         if error, return -1
******************************************************************************/
XCOMM_AdcTestMode XCOMM_SetAdcTestMode(XCOMM_AdcTestMode testMode, XCOMM_AdcChannel channel)
{
    int32_t mode = ad9643_test_mode(testMode, channel);

    if(mode < 0)
        return (XCOMM_AdcTestMode)-1;

    XCOMM_State.adcTestMode = (XCOMM_AdcTestMode)mode;
    XCOMM_State.adcTestModeValid = 1;

    return XCOMM_State.adcTestMode;
}

/**************************************************************************//**
* @brief Gets the test mode of the ADC
*
* @param readMode: read ADC test mode from driver or HW
*
* @return If success, return ADC test mode
*         if error,return -1
******************************************************************************/
XCOMM_AdcTestMode XCOMM_GetAdcTestMode(XCOMM_ReadMode readMode)
{
    int32_t mode;

    if(readMode == XCOMM_ReadMode_FromHW)
    {
        mode = ad9643_test_mode(XCOMM_AdcTestMode_Ramp + 1, XCOMM_AdcChannel_All);
        if(mode < 0)
            return (XCOMM_AdcTestMode)-1;

        XCOMM_State.adcTestMode = (XCOMM_AdcTestMode)mode;
        XCOMM_State.adcTestModeValid = 1;
    }

    return XCOMM_State.adcTestModeValid ? XCOMM_State.adcTestMode : (XCOMM_AdcTestMode)-1;
}

/**************************************************************************//**
* @brief Sets the user test pattern of the ADC 
*
* @param pattern: Buffer containing the test pattern (8 bytes)
*
* @return If success, returns 0 
*         if error, returns -1 
******************************************************************************/
int32_t XCOMM_SetAdcUserTestPattern(uint8_t* pattern)
{
    int32_t ret;

    ret = ad9643_user_test_pattern(pattern);

    return ret;
}

/**************************************************************************//**
* @brief Calibrates the ADC DCO clock delay 
* 
* @return If success, returns DCO clock delay code
*		  if error,return -1
******************************************************************************/
int32_t XCOMM_CalibrateAdcDco(void)
{
	int32_t ret;

	ADC_Core_Write(ADC_CORE_DMA_CHAN_SEL,0x00);

	ad9643_dco_clock_invert(0);
	ret = ad9643_dco_calibrate_2c();

	if(ret<0)
	{
		ad9643_dco_clock_invert(1);
		ret = ad9643_dco_calibrate_2c();
                if (!(ret<0))
                {
                    ret |= 0x0100;
                }

	}

	ADC_Core_Write(ADC_CORE_DMA_CHAN_SEL,0x02);

	return ret;
}

/**************************************************************************//**
* @brief Checks if the ADC DCO is locked
*
* @return If the DCO is locked returns 1
* 		  if the DCO is not locked returns 0
******************************************************************************/
int32_t XCOMM_IsAdcDcoLocked(void)
{
	return ad9643_is_dco_locked();
}

/************************ DAC Functions **************************************/


/**************************************************************************//**
* @brief Sets the sampling rate of the DAC
*
* @param rate: desired rate in Hz
*  
* @return If success, return exact calculated rate in Hz
*         if error, return -1
******************************************************************************/
int64_t XCOMM_SetDacSamplingRate(uint64_t rate)
{
    int64_t sampleRate = ad9523_out_altvoltage1_DAC_CLK_frequency(rate);

    if(sampleRate < 0)
        return -1;

    XCOMM_State.dacSampleRate = sampleRate;
    XCOMM_State.dacSampleRateValid = 1;

    return XCOMM_State.dacSampleRate;
}

/**************************************************************************//**
* @brief Gets the sampling rate of the DAC
*
* @param readMode: read rate from driver or HW
*
* @return If success, return rate in Hz
*         if error, return -1
******************************************************************************/
int64_t XCOMM_GetDacSamplingRate(XCOMM_ReadMode readMode)
{
    int64_t sampleRate;
    
    if(readMode == XCOMM_ReadMode_FromHW)
    {
		sampleRate = ad9523_out_altvoltage1_DAC_CLK_frequency(INT64_MAX);
		if(sampleRate < 0)
			return -1;

		XCOMM_State.dacSampleRate = sampleRate;
        XCOMM_State.dacSampleRateValid = 1;
    }
    
    return XCOMM_State.dacSampleRateValid ? XCOMM_State.dacSampleRate : -1;
}

/**************************************************************************//**
* @brief Sets offset and phase correction for I and Q in DAC
*
* @param iqCorrection: desired correction
*
* @return If success, return IQCorrection struct with error set to 0
*         if error, return IQCorrection struct with error set to -1
******************************************************************************/
XCOMM_DacIQCorrection XCOMM_SetDacIqCorrection(XCOMM_DacIQCorrection daciqCorrection)
{
    
    XCOMM_State.dacIQCorrection.fsAdjI = ad9122_fs_adj_I_DAC(daciqCorrection.fsAdjI);
    XCOMM_State.dacIQCorrection.fsAdjQ = ad9122_fs_adj_Q_DAC(daciqCorrection.fsAdjQ);

    XCOMM_State.dacIQCorrection.offsetI = ad9122_offset_I_DAC(daciqCorrection.offsetI);
    XCOMM_State.dacIQCorrection.offsetQ = ad9122_offset_Q_DAC(daciqCorrection.offsetQ);

    XCOMM_State.dacIQCorrection.phaseAdjI = ad9122_phaseAdj_I_DAC(daciqCorrection.phaseAdjI);
    XCOMM_State.dacIQCorrection.phaseAdjQ = ad9122_phaseAdj_Q_DAC(daciqCorrection.phaseAdjQ);

    XCOMM_State.dacIQCorrection.error = 0;
    XCOMM_State.dacIqCorrectionValid = 1;

    return XCOMM_State.dacIQCorrection;
}

/**************************************************************************//**
* @brief Gets offset and phase correction for I and Q from DAC
*
* @param readMode: read rate from driver or HW
*
* @return If success, return IQCorrection struct with error set to 0
*         if error, return IQCorrection struct with error set to -1
******************************************************************************/
XCOMM_DacIQCorrection XCOMM_GetDacIqCorrection(XCOMM_ReadMode readMode)
{
    if(readMode == XCOMM_ReadMode_FromHW)
    {
        XCOMM_State.dacIQCorrection.fsAdjI = ad9122_fs_adj_I_DAC(INT32_MAX);
        XCOMM_State.dacIQCorrection.fsAdjQ = ad9122_fs_adj_Q_DAC(INT32_MAX);

        XCOMM_State.dacIQCorrection.offsetI = ad9122_offset_I_DAC(INT32_MAX);
        XCOMM_State.dacIQCorrection.offsetQ = ad9122_offset_Q_DAC(INT32_MAX);

        XCOMM_State.dacIQCorrection.phaseAdjI = ad9122_phaseAdj_I_DAC(INT32_MAX);
        XCOMM_State.dacIQCorrection.phaseAdjQ = ad9122_phaseAdj_Q_DAC(INT32_MAX);

        XCOMM_State.dacIQCorrection.error = 0;
        XCOMM_State.dacIqCorrectionValid = 1;
    }

    return XCOMM_State.dacIQCorrection;
}
