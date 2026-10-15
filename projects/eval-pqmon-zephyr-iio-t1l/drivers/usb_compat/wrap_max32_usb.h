/*
 * wrap_max32_usb.h - Local override adding MAX32650 support
 *
 * Upstream HAL only supports MAX32665 and MAX32690.
 * This adds a MAX32650 path (no MCR LDO setup needed).
 */

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
