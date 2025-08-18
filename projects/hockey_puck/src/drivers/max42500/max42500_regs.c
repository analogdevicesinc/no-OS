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

#include "max42500_regs.h"

struct max42500_reg_st max42500_regs[MAX42500_REG_NO] = {
    {MAX42500_ID,       0x30},   /* MAX42500_ID */
    {MAX42500_CONFIG1,  0x00},   /* MAX42500_CONFIG1 */
    {MAX42500_CONFIG2,  0x00},   /* MAX42500_CONFIG2 */
    {MAX42500_VMON,     0x00},   /* MAX42500_VMON */
    {MAX42500_RSTMAP,   0x00},   /* MAX42500_RSTMAP */
    {MAX42500_STATOV,   0x00},   /* MAX42500_STATOV */
    {MAX42500_STATUV,   0x00},   /* MAX42500_STATUV */
    {MAX42500_STATOFF,  0x00},   /* MAX42500_STATOFF */
    {MAX42500_VIN1,     0x00},   /* MAX42500_VIN1 */
    {MAX42500_VIN2,     0x00},   /* MAX42500_VIN2 */
    {MAX42500_VIN3,     0x00},   /* MAX42500_VIN3 */
    {MAX42500_VIN4,     0x00},   /* MAX42500_VIN4 */
    {MAX42500_VIN5,     0x00},   /* MAX42500_VIN5 */
    {MAX42500_VINO6,    0x00},   /* MAX42500_VINO6 */
    {MAX42500_VINU6,    0x00},   /* MAX42500_VINU6 */
    {MAX42500_VINO7,    0x00},   /* MAX42500_VINO7 */
    {MAX42500_VINU7,    0x00},   /* MAX42500_VINU7 */
    {MAX42500_OVUV1,    0x00},   /* MAX42500_OVUV1 */
    {MAX42500_OVUV2,    0x00},   /* MAX42500_OVUV2 */
    {MAX42500_OVUV3,    0x00},   /* MAX42500_OVUV3 */
    {MAX42500_OVUV4,    0x00},   /* MAX42500_OVUV4 */
    {MAX42500_OVUV5,    0x00},   /* MAX42500_OVUV5 */
    {MAX42500_FPSSTAT1, 0x00},   /* MAX42500_FPSSTAT1 */
    {MAX42500_FPSCFG1,  0x00},   /* MAX42500_FPSCFG1 */
    {MAX42500_UTIME1,   0x00},   /* MAX42500_UTIME1 */
    {MAX42500_UTIME2,   0x00},   /* MAX42500_UTIME2 */
    {MAX42500_UTIME3,   0x00},   /* MAX42500_UTIME3 */
    {MAX42500_UTIME4,   0x00},   /* MAX42500_UTIME4 */
    {MAX42500_UTIME5,   0x00},   /* MAX42500_UTIME5 */
    {MAX42500_UTIME6,   0x00},   /* MAX42500_UTIME6 */
    {MAX42500_UTIME7,   0x00},   /* MAX42500_UTIME7 */
    {MAX42500_DTIME1,   0x00},   /* MAX42500_DTIME1 */
    {MAX42500_DTIME2,   0x00},   /* MAX42500_DTIME2 */
    {MAX42500_DTIME3,   0x00},   /* MAX42500_DTIME3 */
    {MAX42500_DTIME4,   0x00},   /* MAX42500_DTIME4 */
    {MAX42500_DTIME5,   0x00},   /* MAX42500_DTIME5 */
    {MAX42500_DTIME6,   0x00},   /* MAX42500_DTIME6 */
    {MAX42500_DTIME7,   0x00},   /* MAX42500_DTIME7 */
    {MAX42500_WDSTAT,   0x00},   /* MAX42500_WDSTAT */
    {MAX42500_WDCDIV,   0x00},   /* MAX42500_WDCDIV */
    {MAX42500_WDCFG1,   0x00},   /* MAX42500_WDCFG1 */
    {MAX42500_WDCFG2,   0x00},   /* MAX42500_WDCFG2 */
    {MAX42500_WDKEY,    0x00},   /* MAX42500_WDKEY */
    {MAX42500_WDLOCK,   0x00},   /* MAX42500_WDLOCK */
    {MAX42500_RSTCTRL,  0x00},   /* MAX42500_RSTCTRL */
    {MAX42500_CID,      0x00},   /* MAX42500_CID */
};
