/***************************************************************************//**
*   @file    max42500_regs.c
*   @brief   MAX42500 register file.
*   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
* Copyright 2024(c) Analog Devices, Inc.
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

#include "ltc7841_regs.h"

struct ltc7841_reg_st ltc7841_regs[LTC7841_TOTAL_REGISTERS] = 
{
    {LTC7841_OPERATION, 0x00},
	{LTC7841_VOUT_MODE, 0x00},
	{LTC7841_STATUS_WORD, 0x00},
	{LTC7841_READ_VIN, 0x00},
	{LTC7841_READ_IIN, 0x00},
	{LTC7841_READ_VOUT, 0x00},
	{LTC7841_READ_IOUT, 0x00},
	{LTC7841_READ_TEMPERATURE_1, 0x00},
	{LTC7841_PMBUS_REVISION, 0x00},
	{LTC7841_MFR_IOUT_PEAK, 0x00},
	{LTC7841_MFR_VOUT_PEAK, 0x00},
	{LTC7841_MFR_VIN_PEAK, 0x00},
	{LTC7841_MFR_TEMEPRATURE1_PEAK, 0x00},
	{LTC7841_MFR_IIN_PEAK, 0x00},
	{LTC7841_MFR_CLEAR_PEAKS, 0x00},
	{LTC7841_MFR_VOUT_MARGIN_HIGH, 0x00},
	{LTC7841_MFR_SPECIAL_ID, 0x00},
	{LTC7841_MFR_VOUT_COMMAND, 0x00},
	{LTC7841_MFR_CONFIG, 0x00},
	{LTC7841_MFR_VOUT_MARGIN_LOW, 0x00},
	{LTC7841_MFR_RAIL_ADDRESS, 0x00},
	{LTC7841_MFR_RESET, 0x00}
};
