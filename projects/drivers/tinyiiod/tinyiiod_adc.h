 /***************************************************************************//**
 *   @file   tinyiiod_adc.h
 *   @brief  Header file of tinyiiod_adc
********************************************************************************
 * Copyright 2019(c) Analog Devices, Inc.
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
#ifndef __TINYIIOD_ADC_H__
#define __TINYIIOD_ADC_H__

#include <stdio.h>
#include <stdbool.h>
#include "tinyiiod_util.h"

attrtibute_map *get_ch_read_adc_attr_map();

ssize_t tinyiiod_adc_configure(struct axi_adc *adc, struct axi_dmac	*dmac, uint32_t adc_ddr_base);
ssize_t ch_read_adc_attr(const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len);
ssize_t ch_write_adc_attr(const char *channel,
			     bool ch_out, const char *attr, const char *buf, size_t len);
ssize_t read_adc_attr(const char *attr,
			 char *buf, size_t len, bool debug);
ssize_t write_adc_attr(const char *attr,
			  const char *buf, size_t len, bool debug);
ssize_t transfer_dev_to_mem(const char *device, size_t bytes_count);
ssize_t read_dev(const char *device, char *pbuf, size_t offset,
			size_t bytes_count);
#endif /* __TINYIIOD_ADC_H__ */
