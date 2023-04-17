/***************************************************************************//**
 *   @file   ad4080_regs.c
 *   @brief  Source file for the ad4080 registers map
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
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

#include "ad4080_regs.h"

const uint32_t ad4080_regs[] = {
	AD4080_REG_INTERFACE_CONFIG_A,
	AD4080_REG_INTERFACE_CONFIG_B,
	AD4080_REG_DEVICE_CONFIG,
	AD4080_REG_CHIP_TYPE,
	AD4080_REG_PRODUCT_ID_L,
	AD4080_REG_PRODUCT_ID_H,
	AD4080_REG_CHIP_GRADE,
	AD4080_REG_SCRATCH_PAD,
	AD4080_REG_SPI_REVISION,
	AD4080_REG_VENDOR_L,
	AD4080_REG_VENDOR_H,
	AD4080_REG_STREAM_MODE,
	AD4080_REG_TRANSFER_CONFIG,
	AD4080_REG_INTERFACE_CONFIG_C,
	AD4080_REG_INTERFACE_STATUS_A,
	AD4080_REG_DEVICE_STATUS,
	AD4080_REG_DATA_INTF_CONFIG_A,
	AD4080_REG_DATA_INTF_CONFIG_B,
	AD4080_REG_DATA_INTF_CONFIG_C,
	AD4080_REG_PWR_CTRL,
	AD4080_REG_GPIO_CONFIG_A,
	AD4080_REG_GPIO_CONFIG_B,
	AD4080_REG_GPIO_CONFIG_C,
	AD4080_REG_FIFO_CONFIG,
	AD4080_REG_FIFO_WATERMARK,
	AD4080_REG_FIFO_STATUS,
	AD4080_REG_OFFSET,
	AD4080_REG_GAIN,
};
