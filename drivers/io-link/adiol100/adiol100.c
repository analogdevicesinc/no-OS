/***************************************************************************//**
 *   @file   adiol100.c
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

#include "adiol100.h"
#include "no_os_alloc.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"
#include <errno.h>
#include <stdint.h>

int adiol100_init(struct adiol100_dev **dev, struct adiol100_init_param *ip)
{
    int ret;

    struct adiol100_dev *descriptor = no_os_calloc(1, sizeof(*descriptor));
    if(!descriptor) 
        return -ENOMEM;

    ret = no_os_spi_init(&descriptor->spi_desc, ip->spi_ip);
    if (ret)
    {
        no_os_free(descriptor);
        return ret;
    }

    descriptor->chip_addr = ip->chip_addr;

    *dev = descriptor;
    return 0;
}

int adiol100_remove(struct adiol100_dev *dev)
{
    int ret;

    if (!dev)
        return -ENODEV;

    ret = no_os_spi_remove(dev->spi_desc);
    if (ret)
        return -EINVAL;

    no_os_free(dev);

    return 0;
}

int adiol100_read(struct adiol100_dev *dev, uint16_t reg, uint16_t *value)
{
    uint8_t buf[4];
    uint8_t len;
    int ret;

    if ((reg & 0xFF00) == 0x1F00) {
        buf[0] = (dev->chip_addr << 6) | (0x1F << 1) | 1;
        buf[1] = reg & 0xFF;
        buf[2] = 0x00;
        buf[3] = 0x00;
        len = 4;
    } else {
        buf[0] = (dev->chip_addr << 6) | ((reg & 0xFF) << 1) | 1;
        buf[1] = 0x00;
        buf[2] = 0x00;
        len = 3;
    }

    ret = no_os_spi_write_and_read(dev->spi_desc, buf, len);
    if (ret)
        return ret;

    *value = (buf[len-2] << 8) | buf[len - 1];
    return 0;
}

int adiol100_write(struct adiol100_dev *dev, uint16_t reg, uint16_t value)
{
    uint8_t buf[4];
    uint8_t len;

    if ((reg & 0xFF00) == 0x1F00) {
        buf[0] = (dev->chip_addr << 6) | (0x1F << 1) | 0;
        buf[1] = reg & 0xFF;
        buf[2] = (value >> 8) & 0xFF;
        buf[3] = value & 0xFF;
        len = 4;
    } else {
        buf[0] = (dev->chip_addr << 6) | ((reg & 0xFF) << 1) | 0;
        buf[1] = (value >> 8) & 0xFF;
        buf[2] = value & 0xFF;
        len = 3;
    }

    return no_os_spi_write_and_read(dev->spi_desc, buf, len);
}

int adiol100_fifo_write(struct adiol100_dev *dev, uint8_t *data, uint8_t len,
                        uint16_t trigger)
{
    uint8_t buf[72];
    uint8_t total = len;

    if (len > 66)
        return -EINVAL;

    buf[0] = (dev->chip_addr << 6) | (0x04 << 1) | 0;
    for (int i = 0; i < len; i++)
        buf[1 + i] = data[i];

    int ret = no_os_spi_write_and_read(dev->spi_desc, buf, total + 1);
    if (ret)
        return ret;

    if (trigger)
        return adiol100_write(dev, 0x0005, trigger);

    return 0;
}

int adiol100_fifo_read(struct adiol100_dev *dev, uint8_t *data, uint8_t *len)
{
    uint16_t rxstat;
    uint8_t fifo_lvl;
    uint8_t total;
    uint8_t buf[68];
    int ret;

    ret = adiol100_read(dev, 0x0001, &rxstat);
    if (ret)
        return ret;

    fifo_lvl = rxstat & 0xFF;
    if (fifo_lvl == 0) {
        *len = 0;
        return 0;
    }

    total = fifo_lvl + 2;
    if (total > 66)
        total = 66;

    buf[0] = (dev->chip_addr << 6) | (0x04 << 1) | 1;
    for (int i = 0; i < total; i++)
        buf[1 + i] = 0x00;

    ret = no_os_spi_write_and_read(dev->spi_desc, buf, total + 1);
    if (ret)
        return ret;

    for (int i = 0; i < total; i++)
        data[i] = buf[1 + i];

    *len = total;
    return 0;
}

int adiol100_reset_channel_a(struct adiol100_dev *dev)
{
    int ret;

    ret = adiol100_write(dev, 0x0006, 0x8000);
    if (ret)
        return ret;

    no_os_mdelay(5);

    return 0;
}

int adiol100_reset_channel_b(struct adiol100_dev *dev)
{
    int ret;

    ret = adiol100_write(dev, 0x0016, 0x8000);
    if (ret)
        return ret;

    no_os_mdelay(5);

    return 0;
}
