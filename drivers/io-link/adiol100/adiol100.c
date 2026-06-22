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
#include <errno.h>

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

int adiol100_read()
{
    return 0;
}

int adiol100_write()
{
    return 0;
}

