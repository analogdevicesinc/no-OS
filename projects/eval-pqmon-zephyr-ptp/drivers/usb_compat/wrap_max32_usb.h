/********************************************************************************
 *   @file   wrap_max32_usb.h
 *   @brief  USB HAL wrapper adding MAX32650 support
 *   @author Radu Etz (radu.etz@analog.com)
 ********************************************************************************
 * Copyright (c) 2026 Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

#ifndef LIBRARIES_ZEPHYR_MAX_INCLUDE_WRAP_MAX32_USB_H_
#define LIBRARIES_ZEPHYR_MAX_INCLUDE_WRAP_MAX32_USB_H_

#include <usb.h>
#include <usb_event.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_SOC_MAX32665) || defined(CONFIG_SOC_MAX32666)

static inline int Wrap_MXC_USB_Init(maxusb_cfg_options_t *options)
{
    return MXC_USB_Init(options);
}

#elif defined(CONFIG_SOC_MAX32690)

#include_next <mcr_regs.h>

static inline int Wrap_MXC_USB_Init(maxusb_cfg_options_t *options)
{
    MXC_MCR->ldoctrl |= MXC_F_MCR_LDOCTRL_0P9EN;
    return MXC_USB_Init(options);
}

#elif defined(CONFIG_SOC_MAX32650)

static inline int Wrap_MXC_USB_Init(maxusb_cfg_options_t *options)
{
    return MXC_USB_Init(options);
}

/* MAX32650 uses MXC_SYS_RESET_USB, not MXC_SYS_RESET0_USB */
#ifndef MXC_SYS_RESET0_USB
#define MXC_SYS_RESET0_USB MXC_SYS_RESET_USB
#endif

#endif /* part number */

#ifdef __cplusplus
}
#endif

#endif /* LIBRARIES_ZEPHYR_MAX_INCLUDE_WRAP_MAX32_USB_H_ */
