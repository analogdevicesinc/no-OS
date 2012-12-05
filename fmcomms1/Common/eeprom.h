/**************************************************************************//**
*   @file   eeprom.h
*   @brief  EEPROM header file.
*   @author mhennerich
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

#ifndef __XCOMM_EEPROM_H___
#define __XCOMM_EEPROM_H___

#include <stdint.h>

#define IICSEL_FRU  0x51
#define IICSEL_CAL  0x55

#define CURRENT_VERSION 0
#define MAX_SIZE_CAL_EEPROM	254
#define FAB_SIZE_CAL_EEPROM	256
#define NEXT_TERMINATION	0

#define ADI_MAGIC_0	'A'
#define ADI_MAGIC_1	'D'
#define ADI_VERSION(v)	('0' + (v))

#pragma pack(push, 1) // exact fit - no padding
struct fmcomms1_calib_data 
{
		int8_t   adi_magic0;
		int8_t   adi_magic1;
		int8_t   version;
		uint8_t  next;
		uint16_t cal_frequency_MHz;

		/* DAC Calibration Data */
        int16_t  i_phase_adj;     /* 10-bit */
        int16_t  q_phase_adj;     /* 10-bit */
        uint16_t i_dac_offset;    /* 16-bit */
        uint16_t q_dac_offset;    /* 16-bit */
        uint16_t i_dac_fs_adj;    /* 10-bit */
        uint16_t q_dac_fs_adj;    /* 10-bit */
        
        /* ADC Calibration Data */
        int16_t  i_adc_offset_adj;  /* 16-bit signed */
        uint16_t i_adc_gain_adj;    /* 16-bit fract 1.15 */
        int16_t  q_adc_offset_adj;  /* 16-bit signed */
        uint16_t q_adc_gain_adj;    /* 16-bit fract 1.15 */
};
#pragma pack(pop) //back to whatever the previous packing mode was  

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
/** Reads data from the selected EEPROM device */
int32_t EEPROM_Read(uint8_t i2cAddr, uint8_t eepromAddr, 
                    uint8_t* pData, uint16_t size);
/** Write data to the selected EEPROM device */
int32_t EEPROM_Write(uint8_t i2cAddr, uint8_t eepromAddr, 
                     uint8_t* pData, uint16_t size);
/** Reads the calibration data from the calibration EEPROM */
int32_t EEPROM_GetCalData(uint8_t* pData, uint8_t* pSize);

#endif /* __XCOMM_EEPROM_H__ */
