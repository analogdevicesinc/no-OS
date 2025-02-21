/***************************************************************************//**
 *   @file   iio_example.c
 *   @brief  Source file for iio example.
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
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
#include "iio_example.h"
#include "iio_admt4000.h"
#include "platform_includes.h"
#include "no_os_print_log.h"
#include "iio_app.h"
#include "no_os_util.h"

int iio_example_main()
{
    int ret;

    /** IIO descriptor and initialization parameter. */
    struct admt4000_iio_dev *admt4000_iio_desc;

    struct admt4000_iio_dev_init_param admt4000_iio_ip = {
        .admt4000_dev_init = &admt_ip,
    };

    /** IIO app. */
    struct iio_app_desc *app;
    struct iio_app_init_param app_init_param = { 0 };

    ret = admt4000_iio_init(&admt4000_iio_desc, &admt4000_iio_ip);
    if (ret)
        goto exit;

    /** Declaring iio_devices structure */
    struct iio_app_device iio_devices[] = {
        {
            .name = "admt4000",
            .dev = admt4000_iio_desc,
            .dev_descriptor = admt4000_iio_desc->iio_dev,
        },
    };

    /** Initializing IIO app init param. */
    app_init_param.devices = iio_devices;
    app_init_param.nb_devices = NO_OS_ARRAY_SIZE(iio_devices);
    app_init_param.uart_init_params = uart_ip;

    /** Initializing IIO app. */
    ret = iio_app_init(&app, app_init_param);
    if (ret)
        goto iio_admt4000_remove;

    /** Running the IIO app (use iio_info or osc in terminal). */
    ret = iio_app_run(app);

    iio_app_remove(app);

iio_admt4000_remove:
    admt4000_iio_remove(admt4000_iio_desc);
exit:
    if (ret)
        pr_info("Error!\n");
    return ret;
}
