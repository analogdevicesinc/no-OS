// ***************************************************************************
// ***************************************************************************
// Copyright 2014 - 2017 (c) Analog Devices, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in
// the documentation and/or other materials provided with the
// distribution.
// - Neither the name of Analog Devices, Inc. nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
// - The use of this software may or may not infringe the patent rights
// of one or more patent holders. This license does not release you
// from the requirement that you obtain separate licenses from these
// patent holders to use this software.
// - Use of the software either in source or binary form, must be run
// on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#ifdef XILINX
#ifndef XCVR_CORE_XILINX_H
#define XCVR_CORE_XILINX_H

#include "xcvr_core.h"

#define XCVR_GT_INFO_ADDR			0x0004
#define XCVR_GT_DEVICE(x)			(((x) >> 24) & 0xf)
#define XCVR_7SERIES				0x0
#define XCVR_ULTRASCALE				0x1
#define XCVR_ULTRASCALE_PLUS			0x2
#define XCVR_GT_TYPE(x)				((x) & 0xff)
#define XCVR_GTXE2				0x02
#define XCVR_GTHE2				0x03
#define XCVR_GTHE3				0x05
#define XCVR_GTYE3				0x06
#define XCVR_GTHE4				0x07

#define XCVR_TIMEOUT_ENABLE_ADDR		0x0014
#define XCVR_TIMEOUT_COUNT_ADDR			0x001c
#define XCVR_GT_COMMON_SEL_ADDR			0x0020
#define XCVR_GT_CHANNEL_SEL_ADDR		0x0024

#define XCVR_STATUS_ADDR			0x0080
#define XCVR_TXRESET_STATUS_MASK		(1<<4)
#define XCVR_RXRESET_STATUS_MASK		(1<<3)
#define XCVR_CPLL_STATUS_MASK			(1<<2)
#define XCVR_QPLL_STATUS_MASK			(1<<1)
#define XCVR_QPLL1_STATUS_MASK			(1<<0)

#define XCVR_TXPLL_TYPE_ADDR			0x00a0
#define XCVR_RXPLL_TYPE_ADDR			0x00c0
#define XCVR_CPLL				0x0
#define XCVR_QPLL				0x3
#define XCVR_QPLL1				0x2

#define XCVR_GT_COMMON_ADDR_ADDR		0x0104
#define XCVR_GT_COMMON_WRDATA_ADDR		0x0108
#define XCVR_GT_COMMON_RDDATA_ADDR		0x010c
#define XCVR_GT_COMMON_RESET_ADDR		0x0110
#define XCVR_GT_COMMON_STATUS_ADDR		0x0114
#define XCVR_GT_COMMON_DONE_ADDR		0x011c
#define XCVR_GT_CHANNEL_ADDR_ADDR		0x0204
#define XCVR_GT_CHANNEL_WRDATA_ADDR		0x0208
#define XCVR_GT_CHANNEL_RDDATA_ADDR		0x020c
#define XCVR_GT_CHANNEL_RESET_ADDR		0x0210
#define XCVR_GT_CHANNEL_STATUS_ADDR		0x0214
#define XCVR_GT_CHANNEL_DONE_ADDR		0x021c
#define XCVR_GT_WRADDR(x)			(((x) & 0x3fffffff) | 0x80000000)
#define XCVR_GT_RDADDR(x)			(((x) & 0x3fffffff) | 0x40000000)
#define XCVR_GT_STATUS(x)			((x) & 0x1)

#define XCVR_QPLL_PWDN_ADDR			0x0304
#define XCVR_QPLL1_PWDN_ADDR			0x0308

#define XCVR_RX_PWDN_ADDR			0x0404
#define XCVR_CPLL_PWDN_ADDR			0x0408
#define XCVR_TXPLL_SEL_ADDR			0x040c
#define XCVR_RXPLL_SEL_ADDR			0x0410
#define XCVR_TX_POSTCURSOR_ADDR			0x0414
#define XCVR_TX_PRECURSOR_ADDR			0x0418

#define XCVR_TX_RESET_ADDR			0x0420
#define XCVR_RX_RESET_ADDR			0x0424

#define XCVR_CPLL_CALPERIOD_ADDR		0x0430
#define XCVR_CPLL_CALTOLERANCE_ADDR		0x0434

#define XCVR_TX_PWDN_ADDR			0x0504
#define XCVR_TX_DIFFCNTRL_ADDR			0x0508
#define XCVR_TX_INHIBIT_ADDR			0x050c
#define XCVR_TX_POLARITY_ADDR			0x0510
#define XCVR_RX_POLARITY_ADDR			0x0604
#define XCVR_RX_LPMEN_ADDR			0x0608
#define XCVR_RX_DFELPM_RESET_ADDR		0x060c

#endif
#endif

// ***************************************************************************
// ***************************************************************************
