/***************************************************************************//**
 *   @file   adiol100.h
 *   @brief  Implementation of ADIOL100 Driver.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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

#ifndef __ADIOL100_H_
#define __ADIOL100_H_

#include "no_os_spi.h"
#include <stdint.h>
#include "no_os_delay.h"

struct adiol100_init_param{
    struct no_os_spi_init_param *spi_ip;
    uint8_t chip_addr;
};

struct adiol100_dev{
    struct no_os_spi_desc *spi_desc;
    uint8_t chip_addr;
};

int adiol100_init(struct adiol100_dev **dev, struct adiol100_init_param *ip);

int adiol100_remove(struct adiol100_dev *dev);

int adiol100_read(struct adiol100_dev *dev, uint16_t reg, uint16_t *value);

int adiol100_write(struct adiol100_dev *dev, uint16_t reg, uint16_t value);

int adiol100_fifo_write(struct adiol100_dev *dev, uint8_t *data, uint8_t len,
                        uint16_t trigger);

int adiol100_fifo_read(struct adiol100_dev *dev, uint8_t *data, uint8_t *len);

int adiol100_reset_channel_a(struct adiol100_dev *dev);

int adiol100_reset_channel_b(struct adiol100_dev *dev);

#endif
