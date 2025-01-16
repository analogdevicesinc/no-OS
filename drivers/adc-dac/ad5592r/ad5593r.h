/***************************************************************************//**
 *   @file   ad5593r.h
 *   @brief  Header file of AD5593R driver.
 *   @author Mircea Caprioru (mircea.caprioru@analog.com)
********************************************************************************
 * Copyright 2018, 2020, 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef AD5593R_H_
#define AD5593R_H_

#include "stdint.h"
#include "ad5592r-base.h"

#define AD5593R_MODE_CONF		(0 << 4)
#define AD5593R_MODE_DAC_WRITE		(1 << 4)
#define AD5593R_MODE_ADC_READBACK	(4 << 4)
#define AD5593R_MODE_DAC_READBACK	(5 << 4)
#define AD5593R_MODE_GPIO_READBACK	(6 << 4)
#define AD5593R_MODE_REG_READBACK	(7 << 4)

#define AD5593R_STOP_BIT	1
#define AD5593R_RESTART_BIT	0
#define AD5593R_ADC_VALUES_BUFF_SIZE	    18

int32_t ad5593r_write_dac(struct ad5592r_dev *dev, uint8_t chan,
			  uint16_t value);
int32_t ad5593r_read_adc(struct ad5592r_dev *dev, uint8_t chan,
			 uint16_t *value);
int32_t ad5593r_multi_read_adc(struct ad5592r_dev *dev,
			       uint16_t chans, uint16_t *value);
int32_t ad5593r_reg_write(struct ad5592r_dev *dev, uint8_t reg,
			  uint16_t value);
int32_t ad5593r_reg_read(struct ad5592r_dev *dev, uint8_t reg,
			 uint16_t *value);
int32_t ad5593r_gpio_read(struct ad5592r_dev *dev, uint8_t *value);
int32_t ad5593r_init(struct ad5592r_dev **dev,
		     struct ad5592r_init_param *init_param);

#endif /* AD5593R_H_ */
