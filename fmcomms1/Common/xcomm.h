/**************************************************************************//**
*   @file   xcomm.h
*   @brief  XCOMM interface header file.
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

#ifndef __XCOMM_H__
#define __XCOMM_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***************************** XCOMM *****************************************/
/******************************************************************************/

/** ADC Test Modes Definitions */
typedef enum
{
XCOMM_AdcTestMode_Off                     = 0x00,
XCOMM_AdcTestMode_MidscaleShort           = 0x01,
XCOMM_AdcTestMode_PositiveFs              = 0x02,
XCOMM_AdcTestMode_NegativeFs              = 0x03,
XCOMM_AdcTestMode_AlternatingCheckerboard = 0x04,
XCOMM_AdcTestMode_Pn23Seq                 = 0x05,
XCOMM_AdcTestMode_Pn9Seq                  = 0x06,
XCOMM_AdcTestMode_OneZeroToggle           = 0x07,
XCOMM_AdcTestMode_UserMode                = 0x08,
XCOMM_AdcTestMode_Ramp                    = 0x0F,
}XCOMM_AdcTestMode;

/** ADC Channels */
typedef enum
{
XCOMM_AdcChannel_A		= 0x01,
XCOMM_AdcChannel_B		= 0x02,
XCOMM_AdcChannel_All	= 0x03
}XCOMM_AdcChannel;

/** Read Modes Definitions */
typedef enum
{
    XCOMM_ReadMode_FromDriver = 0,
    XCOMM_ReadMode_FromHW     = 1,
}XCOMM_ReadMode;

/** Version Buffer Definitions */
typedef struct
{
    int8_t      value[128];
    int32_t     error;
}XCOMM_Version;

/** Tx IQ Correction Definitions */
typedef struct
{
    int16_t     offsetI; 
    int16_t     phaseAdjI; 
    int32_t     fsAdjI;
    int16_t     offsetQ; 
    int16_t     phaseAdjQ; 
    int32_t     fsAdjQ;
    int32_t     error;
}XCOMM_TxIQCorrection;


/** Rx IQ Correction Definitions */
typedef struct
{
    int16_t     gainI;
    int16_t     offsetI; 
    int16_t     gainQ;
    int16_t     offsetQ; 
    int32_t     error;
}XCOMM_RxIQCorrection;

typedef XCOMM_TxIQCorrection XCOMM_DacIQCorrection;

/** XCOMM Default Initialization Structure */
typedef struct 
{	
	uint64_t	adcSamplingRate;
	uint64_t	dacSamplingRate;
	int32_t		rxGain1000;
	uint64_t	rxFrequency;
	uint64_t	txFrequency;
}XCOMM_DefaultInit;

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/

/** Initializes the XCOMM board */
/*  ** if success, return 0 */
/*  ** if error, return -1 */
int32_t XCOMM_Init(XCOMM_DefaultInit* pDefInit);

/** Resync driver cached values by reading the XCOMM board */
/*  ** if success, return 0 */
/*  ** if error, return -1 */
int32_t XCOMM_Sync(void);

/** Gets the XCOMM board version string*/
/*  ** readMode: read version from driver or HW */
/*  ** if success, return version struct with version string and error set to 0 */
/*  ** if error, return version struct with error set to -1 */
XCOMM_Version XCOMM_GetBoardVersion(XCOMM_ReadMode readMode);

/** Gets the PIC firmware version*/
/*  ** if success, returns the PIC firmware version number - 0 to 100 */
/*  ** if error returns -1 */
int32_t XCOMM_GetPicFwVersion();

/************************ Rx Functions *****************************/

/** Sets the Rx center frequency */
/*  ** frequency: desired frequency in Hz */
/*  ** if success, return exact calculated frequency in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_SetRxFrequency(uint64_t frequency);

/** Gets the Rx center frequency */
/*  ** if success, return frequency in Hz stored in driver */
/*  ** if error, return -1 */
int64_t XCOMM_GetRxFrequency(void);

/** Sets the Rx center frequency resolution */
/*  ** resolution: desired frequency resolution in Hz */
/*  ** if success, return exact calculated resolution in Hz */
/*  ** if error, return -1 */
int32_t XCOMM_SetRxResolution(uint32_t resolution);

/** Gets the Rx center frequency resolution */
/*  ** readMode: read frequency resolution from driver or HW */
/*  ** if success, return frequency resolution in Hz */
/*  ** if error, return -1 */
int32_t XCOMM_GetRxResolution(XCOMM_ReadMode readMode);

/** Sets the Rx gain */
/*  ** gain1000: desired gain (x1000) in dB */
/*  ** if success, return calculated gain (x1000) in dB */
/*  ** if error, return -1 */
int32_t XCOMM_SetRxGain(int32_t gain1000);

/** Gets the Rx gain */
/*  ** readMode: read gain from driver or HW */
/*  ** if success, return gain (x1000) in dB */
/*  ** if error, return -1 */
int32_t XCOMM_GetRxGain(XCOMM_ReadMode readMode);

/** Gets the Rx gain and phase correction for I and Q */
/*  ** frequency: center frequency used for the correction in Hz */
/*  ** readMode: read gain and phase correction from driver or HW */
/*  ** if success, return IQCorrection struct with gain and phase correction for the frequency and error set to 0 */
/*  ** if error, return IQCorrection struct with error set to -1 */
XCOMM_RxIQCorrection XCOMM_GetRxIqCorrection(uint64_t frequency, XCOMM_ReadMode readMode);

/************************ Tx Functions *****************************/

/** Sets the Tx center frequency */
/*  ** frequency: desired frequency in Hz */
/*  ** if success, return calculated frequency in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_SetTxFrequency(uint64_t frequency);

/** Gets the Tx center frequency */
/*  ** if success, return frequency in Hz stored in driver */
/*  ** if error, return -1 */
int64_t XCOMM_GetTxFrequency(void);

/** Sets the Tx center frequency resolution */
/*  ** resolution: desired frequency resolution in Hz */
/*  ** if success, return exact calculated resolution in Hz */
/*  ** if error, return -1 */
int32_t XCOMM_SetTxResolution(uint32_t resolution);

/** Gets the Tx center frequency resolution */
/*  ** readMode: read frequency resolution from driver or HW */
/*  ** if success, return frequency resolution in Hz */
/*  ** if error, return -1 */
int32_t XCOMM_GetTxResolution(XCOMM_ReadMode readMode);

/** Gets the Tx gain and phase correction for I and Q */
/*  ** frequency: center frequency used for the correction in Hz*/
/*  ** readMode: read gain and phase correction from driver or HW */
/*  ** if success, return IQCorrection struct with gain and phase correction for the frequency and error set to 0 */
/*  ** if error, return IQCorrection struct with error set to -1 */
XCOMM_TxIQCorrection XCOMM_GetTxIqCorrection(uint64_t frequency, XCOMM_ReadMode readMode);

/************************ ADC Functions *****************************/

/** Sets the sampling rate of the ADC */
/*  ** rate: desired rate in Hz */
/*  ** if success, return exact calculated rate in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_SetAdcSamplingRate(uint64_t rate);

/** Gets the sampling rate of the ADC */
/*  ** readMode: read rate from driver or HW */
/*  ** if success, return rate in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_GetAdcSamplingRate(XCOMM_ReadMode);

/** Sets the test mode of the ADC */
/*  ** testMode: desired ADC test mode */
/*  ** channel: ADC channel to set the test mode for */
/*  ** if success, return new ADC test mode if success */
/*  ** if error, return -1 */
XCOMM_AdcTestMode XCOMM_SetAdcTestMode(XCOMM_AdcTestMode testMode, XCOMM_AdcChannel channel);

/** Gets the test mode of the ADC */
/*  ** readMode: read ADC test mode from driver or HW */
/*  ** if success, return ADC test mode */
/*  ** if error,return -1 */
XCOMM_AdcTestMode XCOMM_GetAdcTestMode(XCOMM_ReadMode readMode);

/** Sets the user test pattern of the ADC */
/*  ** pattern: buffer containing the test pattern (8 bytes) */
/*  ** if success, returns 0 */
/*  ** if error,return -1 */
int32_t XCOMM_SetAdcUserTestPattern(uint8_t* pattern);

/** Calibrates the ADC DCO clock delay */
/*  ** if success, returns DCO clock delay code */
/*  ** if error,return -1 */
int32_t XCOMM_CalibrateAdcDco(void);

/** Checks if the ADC DCO is locked */
/*  ** if the DCO is locked returns 1 */
/*  ** if the DCO is not locked returns 0 */
int32_t XCOMM_IsAdcDcoLocked(void);

/************************ DAC Functions *****************************/

/** Sets the sampling rate of the DAC */
/*  ** rate: desired rate in Hz */
/*  ** if success, return exact calculated rate in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_SetDacSamplingRate(uint64_t rate);

/** Gets the sampling rate of the DAC */
/*  ** readMode: read rate from driver or HW */
/*  ** if success, return rate in Hz */
/*  ** if error, return -1 */
int64_t XCOMM_GetDacSamplingRate(XCOMM_ReadMode readMode);


/** Sets offset and phase correction for I and Q in DAC*/
/*  ** daciqCorrection: desired correction*/
/*  ** if success, return IQCorrection struct with offset and phase correction and error set to 0 */
/*  ** if error, return IQCorrection struct with error set to -1 */
XCOMM_DacIQCorrection XCOMM_SetDacIqCorrection(XCOMM_DacIQCorrection daciqCorrection);

/** Gets offset and phase correction for I and Q from DAC*/
/*  ** readMode: read DAC correction from driver or HW */
/*  ** if success, return IQCorrection struct with offset and phase correction and error set to 0 */
/*  ** if error, return IQCorrection struct with error set to -1 */
XCOMM_DacIQCorrection XCOMM_GetDacIqCorrection(XCOMM_ReadMode readMode);

#endif /* __XCOMM_H__ */

