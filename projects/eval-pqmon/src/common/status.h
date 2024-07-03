/*******************************************************************************
 *   @file   status.h
 *   @brief  Status state definition header file
 *   @author Robert Budai (robert.budai@analog.com)
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#ifndef __STATUS_H__
#define __STATUS_H__

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define SYS_STATUS_SUCCESS 0
#define SYS_STATUS_PQLIB_ERROR 1
#define SYS_STATUS_NO_SIGNAL 2
#define SYS_STATUS_PQLIB_RUNNING 3
#define SYS_STATUS_PQLIB_CONFIG_ERROR 4
#define SYS_STATUS_AFE_CONFIG0_FAILED 5
#define SYS_STATUS_AFE_CONFIG1_FAILED 6
#define SYS_STATUS_AFE_CONFIG2_FAILED 7
#define SYS_STATUS_AFE_ACCMODE_FAILED 8
#define SYS_STATUS_AFE_ZX_LP_SEL_FAILED 9
#define SYS_STATUS_AFE_MASK0_FAILED 10
#define SYS_STATUS_AFE_RUN_FAILED 11
#define SYS_STATUS_AFE_WFB_CFG_FAILED 12
#define SYS_STATUS_AFE_READWAVEFORM_FAILED 13
#define SYS_STATUS_AFE_READRMS1012_FAILED 14
#define SYS_STATUS_AFE_READRMSONE_FAILED 15
#define SYS_STATUS_AFE_READPERIOD_FAILED 16
#define SYS_STATUS_AFE_READANGLE_FAILED 17
#define SYS_STATUS_AFE_READVERSION_FAILED 18
#define SYS_STATUS_AFE_STATUS1_FAILED 19
#define SYS_STATUS_AFE_STATUS0_FAILED 20
#define SYS_STATUS_TIMER_INIT_FAILED 21

#endif /* __AFE_CONFIG_H__ */
