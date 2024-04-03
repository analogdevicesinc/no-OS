/***************************************************************************//**
 *   @file   maxim_usb_uart.c
 *   @brief  Source file of USB CDC ACM driver for maxim platform.
 *   @author Darius Berghe (darius.berghe@analog.com)
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
#include <errno.h>
#include <stdlib.h>
#include <stddef.h>
#include "maxim_irq.h"
#include "maxim_usb_uart.h"
#include "no_os_irq.h"
#include "no_os_util.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_lf256fifo.h"
#include "gcr_regs.h"
#include "mxc_sys.h"
#include "mxc_delay.h"
#include "usb.h"
#include "usb_event.h"
#include "enumerate.h"
#include "cdc_acm.h"
#include "maxim_usb_uart_descriptors.h"

#define EVENT_ENUM_COMP MAXUSB_NUM_EVENTS
#define EVENT_REMOTE_WAKE (EVENT_ENUM_COMP + 1)

static int setconfig_callback(MXC_USB_SetupPkt *sud, void *cbdata);
static int setfeatureCallback(MXC_USB_SetupPkt *sud, void *cbdata);
static int clrfeatureCallback(MXC_USB_SetupPkt *sud, void *cbdata);
static int event_callback(maxusb_event_t evt, void *data);
static void usbAppSleep(void);
static void usbAppWakeup(void);
static int usb_read_callback(void);
int usbStartupCallback();
int usbShutdownCallback();

static void usb_callback(void *context)
{
	MXC_USB_EventHandler();
}

/* This EP assignment must match the Configuration Descriptor */
static acm_cfg_t acm_cfg = {
	1, /* EP OUT */
	MXC_USBHS_MAX_PACKET, /* OUT max packet size */
	2, /* EP IN */
	MXC_USBHS_MAX_PACKET, /* IN max packet size */
	3, /* EP Notify */
	MXC_USBHS_MAX_PACKET, /* Notify max packet size */
};
static int configured = 0;
static int suspended = 0;
static int event_flags = 0;
static int remote_wake_en = 0;

static volatile int usb_read_complete;

void delay_us(unsigned int usec)
{
	/* mxc_delay() takes unsigned long, so can't use it directly */
	MXC_Delay(usec);
}

/**
 * @brief Read data from UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
static int32_t max_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
			     uint32_t bytes_number)
{
	return acm_read(data, bytes_number);
}

/**
 * @brief Write data to UART device. Blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_write(struct no_os_uart_desc *desc, const uint8_t *data,
			      uint32_t bytes_number)
{
	if (acm_present())
		return acm_write((uint8_t *)data, bytes_number);

	return -ENODEV;
}

/**
 * @brief Read data from UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return positive number of received bytes in case of success, negative error code otherwise.
 */
static int32_t max_uart_read_nonblocking(struct no_os_uart_desc *desc,
		uint8_t *data,
		uint32_t bytes_number)
{
	return -ENOSYS;
}

/**
 * @brief Write data to UART device. Non blocking function.
 * @param desc - Instance of UART.
 * @param data - Pointer to buffer containing data.
 * @param bytes_number - Number of bytes to read.
 * @return 0 in case of success, errno codes otherwise.
 */

static int32_t max_uart_write_nonblocking(struct no_os_uart_desc *desc,
		const uint8_t *data,
		uint32_t bytes_number)
{
	return -ENOSYS;
}

/**
 * @brief Initialize the UART communication peripheral.
 * @param desc - The UART descriptor.
 * @param param - The structure that contains the UART parameters.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_init(struct no_os_uart_desc **desc,
			     struct no_os_uart_init_param *param)
{
	int32_t ret;
	struct max_usb_uart_init_param *eparam;
	struct no_os_uart_desc *descriptor;
	struct max_usb_uart_desc *max_usb_uart;
	maxusb_cfg_options_t usb_opts;

	if (!param || !param->extra)
		return -EINVAL;

	// force the asynchronous operation which requires the user to provide the irq_id of the USB
	if (!param->asynchronous_rx)
		return -EINVAL;

	descriptor = no_os_calloc(1, sizeof(*descriptor));
	if (!descriptor)
		return -ENOMEM;

	max_usb_uart = no_os_calloc(1, sizeof(*max_usb_uart));
	if (!max_usb_uart) {
		ret = -ENOMEM;
		goto error_max_usb_uart;
	}
	descriptor->extra = max_usb_uart;
	eparam = param->extra;
	if (eparam) {
		device_descriptor.idProduct = eparam->pid;
		device_descriptor.idVendor = eparam->vid;
	}

	/* Initialize state */
	configured = 0;
	suspended = 0;
	event_flags = 0;
	remote_wake_en = 0;

	/* Start out in full speed */
	usb_opts.enable_hs = 0;
	usb_opts.delay_us = delay_us; /* Function which will be used for delays */
	usb_opts.init_callback = usbStartupCallback;
	usb_opts.shutdown_callback = usbShutdownCallback;

	/* Initialize the usb module */
	if (MXC_USB_Init(&usb_opts) != 0) {
		ret = -EFAULT;
		goto error_usb;
	}

	/* Initialize the enumeration module */
	if (enum_init() != 0) {
		ret = -EFAULT;
		goto error_usb;
	}

	/* Register enumeration data */
	enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t *)&device_descriptor, 0);
	enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t *)&config_descriptor, 0);

	enum_register_descriptor(ENUM_DESC_STRING, lang_id_desc, 0);
	enum_register_descriptor(ENUM_DESC_STRING, mfg_id_desc, 1);
	enum_register_descriptor(ENUM_DESC_STRING, prod_id_desc, 2);

	/* Handle configuration */
	enum_register_callback(ENUM_SETCONFIG, setconfig_callback, NULL);

	/* Handle feature set/clear */
	enum_register_callback(ENUM_SETFEATURE, setfeatureCallback, NULL);
	enum_register_callback(ENUM_CLRFEATURE, clrfeatureCallback, NULL);

	/* Initialize the class driver */
	if (acm_init(&config_descriptor.comm_interface_descriptor) != 0) {
		ret = -EFAULT;
		goto error_usb;
	}

	/* Register callbacks */
	MXC_USB_EventEnable(MAXUSB_EVENT_NOVBUS, event_callback, NULL);
	MXC_USB_EventEnable(MAXUSB_EVENT_VBUS, event_callback, NULL);
	acm_register_callback(ACM_CB_READ_READY, usb_read_callback);

	usb_read_complete = 0;

	/* Start with USB in low power mode */
	usbAppSleep();

	struct no_os_irq_init_param nvic_ip = {
		.platform_ops = &max_irq_ops,
	};

	ret = no_os_irq_ctrl_init(&max_usb_uart->nvic, &nvic_ip);
	if (ret)
		goto error_usb;

	struct no_os_callback_desc uart_rx_cb = {
		.callback = usb_callback,
		.event = NO_OS_EVT_USB,
		.peripheral = NO_OS_USB_IRQ,
		.handle = MXC_USBHS,
	};

	ret = no_os_irq_register_callback(max_usb_uart->nvic,
					  param->irq_id,
					  &uart_rx_cb);
	if (ret)
		goto error_irq;

	ret = no_os_irq_enable(max_usb_uart->nvic, param->irq_id);
	if (ret)
		goto error_irq;

	*desc = descriptor;

	return 0;
error_irq:
	no_os_irq_ctrl_remove(max_usb_uart->nvic);
error_usb:
	no_os_free(max_usb_uart);
error_max_usb_uart:
	no_os_free(descriptor);

	return ret;
}

/**
 * @brief Free the resources allocated by max_uart_init().
 * @param desc - The UART descriptor.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t max_uart_remove(struct no_os_uart_desc *desc)
{
	struct max_usb_uart_desc *max_usb_uart;

	if (!desc)
		return -EINVAL;

	max_usb_uart = desc->extra;

	no_os_irq_ctrl_remove(max_usb_uart->nvic);
	no_os_free(desc->extra);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Not implemented.
 * @param desc - The UART descriptor.
 * @return -ENOSYS
 */
static uint32_t max_uart_get_errors(struct no_os_uart_desc *desc)
{
	return -ENOSYS;
}

/**
 * @brief Maxim platform specific UART platform ops structure
 */
const struct no_os_uart_platform_ops max_usb_uart_ops = {
	.init = &max_uart_init,
	.read = &max_uart_read,
	.write = &max_uart_write,
	.read_nonblocking = &max_uart_read_nonblocking,
	.write_nonblocking = &max_uart_write_nonblocking,
	.get_errors = &max_uart_get_errors,
	.remove = &max_uart_remove
};

int usbStartupCallback()
{
	// Startup the HIRC96M clock if it's not on already
	if (!(MXC_GCR->clkcn & MXC_F_GCR_CLKCN_HIRC96M_EN)) {
		MXC_GCR->clkcn |= MXC_F_GCR_CLKCN_HIRC96M_EN;

		if (MXC_SYS_Clock_Timeout(MXC_F_GCR_CLKCN_HIRC96M_RDY) != E_NO_ERROR) {
			return E_TIME_OUT;
		}
	}

	MXC_SYS_ClockEnable(MXC_SYS_PERIPH_CLOCK_USB);

	return E_NO_ERROR;
}

int usbShutdownCallback()
{
	MXC_SYS_ClockDisable(MXC_SYS_PERIPH_CLOCK_USB);

	return E_NO_ERROR;
}

static int setconfig_callback(MXC_USB_SetupPkt *sud, void *cbdata)
{
	/* Confirm the configuration value */
	if (sud->wValue == config_descriptor.config_descriptor.bConfigurationValue) {
		configured = 1;
		MXC_SETBIT(&event_flags, EVENT_ENUM_COMP);

		acm_cfg.out_ep = config_descriptor.endpoint_descriptor_1.bEndpointAddress & 0x7;
		acm_cfg.out_maxpacket = config_descriptor.endpoint_descriptor_1.wMaxPacketSize;
		acm_cfg.in_ep = config_descriptor.endpoint_descriptor_2.bEndpointAddress & 0x7;
		acm_cfg.in_maxpacket = config_descriptor.endpoint_descriptor_2.wMaxPacketSize;
		acm_cfg.notify_ep = config_descriptor.endpoint_descriptor_3.bEndpointAddress &
				    0x7;
		acm_cfg.notify_maxpacket =
			config_descriptor.endpoint_descriptor_3.wMaxPacketSize;

		return acm_configure(&acm_cfg); /* Configure the device class */
	} else if (sud->wValue == 0) {
		configured = 0;
		return acm_deconfigure();
	}

	return -1;
}

static int setfeatureCallback(MXC_USB_SetupPkt *sud, void *cbdata)
{
	if (sud->wValue == FEAT_REMOTE_WAKE) {
		remote_wake_en = 1;
	} else {
		/* Unknown callback */
		return -1;
	}

	return 0;
}

static int clrfeatureCallback(MXC_USB_SetupPkt *sud, void *cbdata)
{
	if (sud->wValue == FEAT_REMOTE_WAKE) {
		remote_wake_en = 0;
	} else {
		/* Unknown callback */
		return -1;
	}

	return 0;
}

static void usbAppSleep(void)
{
	suspended = 1;
}

static void usbAppWakeup(void)
{
	suspended = 0;
}

static int event_callback(maxusb_event_t evt, void *data)
{
	/* Set event flag */
	MXC_SETBIT(&event_flags, evt);

	switch (evt) {
	case MAXUSB_EVENT_NOVBUS:
		MXC_USB_EventDisable(MAXUSB_EVENT_BRST);
		MXC_USB_EventDisable(MAXUSB_EVENT_SUSP);
		MXC_USB_EventDisable(MAXUSB_EVENT_DPACT);
		MXC_USB_Disconnect();
		configured = 0;
		enum_clearconfig();
		acm_deconfigure();
		usbAppSleep();
		break;

	case MAXUSB_EVENT_VBUS:
		MXC_USB_EventClear(MAXUSB_EVENT_BRST);
		MXC_USB_EventEnable(MAXUSB_EVENT_BRST, event_callback, NULL);
		MXC_USB_EventClear(MAXUSB_EVENT_SUSP);
		MXC_USB_EventEnable(MAXUSB_EVENT_SUSP, event_callback, NULL);
		MXC_USB_Connect();
		usbAppSleep();
		break;

	case MAXUSB_EVENT_BRST:
		usbAppWakeup();
		enum_clearconfig();
		acm_deconfigure();
		configured = 0;
		suspended = 0;
		break;

	case MAXUSB_EVENT_SUSP:
		usbAppSleep();
		break;

	case MAXUSB_EVENT_DPACT:
		usbAppWakeup();
		break;

	default:
		break;
	}

	return 0;
}

static int usb_read_callback(void)
{
	usb_read_complete = 1;

	return 0;
}