/***************************************************************************//**
*   @file    ad7124_regs.c
*   @brief   AD7124 register file.
********************************************************************************
* Copyright 2015-2019(c) Analog Devices, Inc.
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
*******************************************************************************/


#include "ad7124_regs.h"

struct ad7124_st_reg ad7124_init_regs_default[AD7124_REG_NO] = {
	{0x00, 0x00,   1, 2}, /* AD7124_Status */
	/*
	 * 7:6 - 11 : Full power mode
	 * 8 - 1: Internale ref enabled
	 * 1:0 - 00 Internal 614.4 clock
	 */
	{0x01, 0x05C0, 2, 1}, /* AD7124_ADC_Control */
	{0x02, 0x0000, 3, 2}, /* AD7124_Data */
	{0x03, 0x0000, 3, 1}, /* AD7124_IOCon1 */
	{0x04, 0x0000, 2, 1}, /* AD7124_IOCon2 */
	{0x05, 0x02,   1, 2}, /* AD7124_ID */
	{0x06, 0x0000, 3, 2}, /* AD7124_Error */
	{0x07, 0x0000, 3, 1}, /* AD7124_Error_En */
	{0x08, 0x00,   1, 2}, /* AD7124_Mclk_Count */
	{0x09, 0x0001, 2, 1}, /* AD7124_Channel_0 */
	{0x0A, 0x0043, 2, 1}, /* AD7124_Channel_1 */
	{0x0B, 0x0043, 2, 1}, /* AD7124_Channel_2 */
	{0x0C, 0x00C7, 2, 1}, /* AD7124_Channel_3 */
	{0x0D, 0x0085, 2, 1}, /* AD7124_Channel_4 */
	{0x0E, 0x014B, 2, 1}, /* AD7124_Channel_5 */
	{0x0F, 0x00C7, 2, 1}, /* AD7124_Channel_6 */
	{0x10, 0x01CF, 2, 1}, /* AD7124_Channel_7 */
	{0x11, 0x0109, 2, 1}, /* AD7124_Channel_8 */
	{0x12, 0x0031, 2, 1}, /* AD7124_Channel_9 */
	{0x13, 0x014B, 2, 1}, /* AD7124_Channel_10 */
	{0x14, 0x0071, 2, 1}, /* AD7124_Channel_11 */
	{0x15, 0x018D, 2, 1}, /* AD7124_Channel_12 */
	{0x16, 0x00B1, 2, 1}, /* AD7124_Channel_13 */
	{0x17, 0x01CF, 2, 1}, /* AD7124_Channel_14 */
	{0x18, 0x00F1, 2, 1}, /* AD7124_Channel_15 */
	{0x19, 0x0980, 2, 1}, /* AD7124_Config_0 */
	{0x1A, 0x0981, 2, 1}, /* AD7124_Config_1 */
	{0x1B, 0x0982, 2, 1}, /* AD7124_Config_2 */
	{0x1C, 0x0983, 2, 1}, /* AD7124_Config_3 */
	{0x1D, 0x0984, 2, 1}, /* AD7124_Config_4 */
	{0x1E, 0x0985, 2, 1}, /* AD7124_Config_5 */
	{0x1F, 0x0986, 2, 1}, /* AD7124_Config_6 */
	{0x20, 0x0987, 2, 1}, /* AD7124_Config_7 */
	/*
	 * 10:0 - 1 - FS = 1 from filter 0 to 7
	 */
	{0x21, 0x060001, 3, 1}, /* AD7124_Filter_0 */
	{0x22, 0x060001, 3, 1}, /* AD7124_Filter_1 */
	{0x23, 0x060001, 3, 1}, /* AD7124_Filter_2 */
	{0x24, 0x060001, 3, 1}, /* AD7124_Filter_3 */
	{0x25, 0x060001, 3, 1}, /* AD7124_Filter_4 */
	{0x26, 0x060001, 3, 1}, /* AD7124_Filter_5 */
	{0x27, 0x060001, 3, 1}, /* AD7124_Filter_6 */
	{0x28, 0x060001, 3, 1}, /* AD7124_Filter_7 */
	{0x29, 0x800000, 3, 1}, /* AD7124_Offset_0 */
	{0x2A, 0x800000, 3, 1}, /* AD7124_Offset_1 */
	{0x2B, 0x800000, 3, 1}, /* AD7124_Offset_2 */
	{0x2C, 0x800000, 3, 1}, /* AD7124_Offset_3 */
	{0x2D, 0x800000, 3, 1}, /* AD7124_Offset_4 */
	{0x2E, 0x800000, 3, 1}, /* AD7124_Offset_5 */
	{0x2F, 0x800000, 3, 1}, /* AD7124_Offset_6 */
	{0x30, 0x800000, 3, 1}, /* AD7124_Offset_7 */
	{0x31, 0x500000, 3, 1}, /* AD7124_Gain_0 */
	{0x32, 0x500000, 3, 1}, /* AD7124_Gain_1 */
	{0x33, 0x500000, 3, 1}, /* AD7124_Gain_2 */
	{0x34, 0x500000, 3, 1}, /* AD7124_Gain_3 */
	{0x35, 0x500000, 3, 1}, /* AD7124_Gain_4 */
	{0x36, 0x500000, 3, 1}, /* AD7124_Gain_5 */
	{0x37, 0x500000, 3, 1}, /* AD7124_Gain_6 */
	{0x38, 0x500000, 3, 1}, /* AD7124_Gain_7 */
};
