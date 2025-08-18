/***************************************************************************//**
 *   @file   max6613.h
 *   @brief  Header for the MAX6613 driver
 *   @author Mark Sapungan (Mark.Sapungan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
#ifndef __MAX6613_H__
#define __MAX6613_H__

#include "mxc_delay.h"
#include "mxc_errors.h"
#include "adc.h"
#include "dma.h"
#include "tmr.h"

/**
 * @brief MAX6613 descriptor
 */
struct max6613_dev {
    mxc_adc_req_t adc_cfg;
    mxc_adc_slot_req_t adc_slot;
    mxc_adc_conversion_req_t adc_conv;
};

/**
 * @brief MAX6613 init parameter
 */
struct max6613_init_param {
    mxc_adc_req_t adc_cfg;
    mxc_adc_slot_req_t adc_slot;
    mxc_adc_conversion_req_t adc_conv;
};

/** Device init function */
int max6613_init(struct max6613_dev **, struct max6613_init_param *);

/** Remove resources allocated by the init function */
int max6613_remove(struct max6613_dev *device);

/** Read raw ADC temperature value */
int max6613_read_raw(struct max6613_dev *device, int32_t *value);

/** Convert raw ADC value to temperature in Celsius */
float max6613_raw_to_celsius(int32_t value);

#endif /* __MAX6613_H__ */
