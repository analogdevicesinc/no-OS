/***************************************************************************//**
 *   @file   no_os_i3c.h
 *   @brief  Header file of I3C Interface
 *   @author Jorge Marques (jorge.marques@analog.com)
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

#ifndef _NO_OS_I3C_H_
#define _NO_OS_I3C_H_

#include <stdint.h>
#include "no_os_util.h"
#include "no_os_dma.h"

#define NO_OS_I3C_MAX_BUS_NUMBER	3
#define NO_OS_I3C_MAX_DEV_NUMBER	15
/* I²C/I3C address are 7-bit (not considering extended 10-bit I²C) */
#define NO_OS_I3C_I2C_MAX_ADDR		0x7F
#define NO_OS_I3C_MAX_ADDR		NO_OS_I3C_I2C_MAX_ADDR
#define NO_OS_I3C_BCAST_ADDR		0x7E
/* 2-bit flags packed in addrslots */
#define NO_OS_I3C_ADDR_FLAG_SIZE	2
#define NO_OS_I3C_ADDR_PER_SLOT		((unsigned int)(sizeof(unsigned int)*8 / NO_OS_I3C_ADDR_FLAG_SIZE))
#define NO_OS_I3C_ADDR_GET_SLOT(x)	((x) / NO_OS_I3C_ADDR_PER_SLOT)
#define NO_OS_I3C_ADDR_GET_POS(x)	(((x) % NO_OS_I3C_ADDR_PER_SLOT) * NO_OS_I3C_ADDR_FLAG_SIZE)
#define NO_OS_I3C_ADDRSLOTS_SIZE	\
	(NO_OS_I3C_I2C_MAX_ADDR / NO_OS_I3C_ADDR_PER_SLOT) + !!(NO_OS_I3C_I2C_MAX_ADDR % NO_OS_I3C_ADDR_PER_SLOT)

/* I3C CCC (Common Command Codes) related definitions */
#define NO_OS_I3C_CCC_BCAST		0
#define NO_OS_I3C_CCC_DIRECT		NO_OS_BIT(7)

#define NO_OS_I3C_CCC_ID(id, type)	((id) | (type))

#define NO_OS_I3C_CCC_ADDR(x)		((x) & 0xFF)
#define NO_OS_I3C_CCC_GET_LEN(x)	(((x) >> 8) & 0x0000FF)
#define NO_OS_I3C_CCC_SET_LEN(x)	(((x) << 8) & 0x00FF00)
#define NO_OS_I3C_CCC_GET_DEF(x)	(((x) >> 17) & NO_OS_BIT(0))
#define NO_OS_I3C_CCC_SET_DEF   	NO_OS_BIT(17)
#define NO_OS_I3C_CCC_GET_RNW(x)	(((x) >> 16) & NO_OS_BIT(0))
#define NO_OS_I3C_CCC_SET_RNW(x)	(((x) << 16) & NO_OS_BIT(16))

/**
 * Commands valid in both broadcast and direct modes
 * type: NO_OS_I3C_CCC_DIRECT, NO_OS_I3C_CCC_BCAST
 */
#define NO_OS_I3C_CCC_ENEC_DIRECT	0x80 | NO_OS_I3C_CCC_SET_LEN(1)
#define NO_OS_I3C_CCC_DISEC_DIRECT	0x81 | NO_OS_I3C_CCC_SET_LEN(1)
#define NO_OS_I3C_CCC_ENEC_BCAST	0x00 | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_DISEC_BCAST	0x01 | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_ENTAS(as, type)	NO_OS_I3C_CCC_ID((0x2 + (as)), (type))
#define NO_OS_I3C_CCC_RSTDAA(type)	NO_OS_I3C_CCC_ID(0x6, (type))
#define NO_OS_I3C_CCC_SETMWL(type)	NO_OS_I3C_CCC_ID(0x9, (type)) | NO_OS_I3C_CCC_SET_LEN(2)
#define NO_OS_I3C_CCC_SETMRL(type)	NO_OS_I3C_CCC_ID(0xa, (type)) | NO_OS_I3C_CCC_SET_LEN(2)
#define NO_OS_I3C_CCC_SETXTIME_BCAST	0x28 | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_SETXTIME_DIRECT	0x98 | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_RSTACT_BCAST	0x2a | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_RSTACT_DIRECT	0x9a | NO_OS_I3C_CCC_SET_DEF
#define NO_OS_I3C_CCC_RSTACT_I3C_ONLY	    0x1
#define NO_OS_I3C_CCC_RSTACT_WHOLE_TARGET   0x2

/* Broadcast-only commands */
#define NO_OS_I3C_CCC_ENTDAA		NO_OS_I3C_CCC_ID(0x7, NO_OS_I3C_CCC_BCAST)
#define NO_OS_I3C_CCC_DEFSLVS		NO_OS_I3C_CCC_ID(0x8, NO_OS_I3C_CCC_BCAST)
#define NO_OS_I3C_CCC_ENTTM		NO_OS_I3C_CCC_ID(0xb, NO_OS_I3C_CCC_BCAST)
#define NO_OS_I3C_CCC_ENTHDR(x)		NO_OS_I3C_CCC_ID((0x20 + (x)), NO_OS_I3C_CCC_BCAST)
#define NO_OS_I3C_CCC_SETAASA		NO_OS_I3C_CCC_ID(0x29, NO_OS_I3C_CCC_BCAST)

/* Unicast-only commands */
#define NO_OS_I3C_CCC_SETDASA		NO_OS_I3C_CCC_ID(0x7, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_SETNEWDA		NO_OS_I3C_CCC_ID(0x8, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETMWL		NO_OS_I3C_CCC_ID(0xb, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETMRL		NO_OS_I3C_CCC_ID(0xc, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETPID		(NO_OS_I3C_CCC_ID(0xd, NO_OS_I3C_CCC_DIRECT) | NO_OS_I3C_CCC_SET_LEN(6) | NO_OS_I3C_CCC_SET_RNW(1))
#define NO_OS_I3C_CCC_GETBCR		(NO_OS_I3C_CCC_ID(0xe, NO_OS_I3C_CCC_DIRECT) | NO_OS_I3C_CCC_SET_LEN(1) | NO_OS_I3C_CCC_SET_RNW(1))
#define NO_OS_I3C_CCC_GETDCR		(NO_OS_I3C_CCC_ID(0xf, NO_OS_I3C_CCC_DIRECT) | NO_OS_I3C_CCC_SET_LEN(1) | NO_OS_I3C_CCC_SET_RNW(1))
#define NO_OS_I3C_CCC_GETSTATUS		(NO_OS_I3C_CCC_ID(0x10, NO_OS_I3C_CCC_DIRECT) | NO_OS_I3C_CCC_SET_LEN(2) | NO_OS_I3C_CCC_SET_RNW(1))
#define NO_OS_I3C_CCC_GETACCMST		NO_OS_I3C_CCC_ID(0x11, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_SETBRGTGT		NO_OS_I3C_CCC_ID(0x13, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETMXDS		NO_OS_I3C_CCC_ID(0x14, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETHDRCAP		NO_OS_I3C_CCC_ID(0x15, NO_OS_I3C_CCC_DIRECT)
#define NO_OS_I3C_CCC_GETXTIME		NO_OS_I3C_CCC_ID(0x19, NO_OS_I3C_CCC_DIRECT)

#define NO_OS_I3C_CCC_EVENT_SIR		NO_OS_BIT(0)
#define NO_OS_I3C_CCC_EVENT_MR		NO_OS_BIT(1)
#define NO_OS_I3C_CCC_EVENT_HJ		NO_OS_BIT(3)
/**
 * List of possible IRQ events.
 */
#define NO_OS_I3C_IRQ_IBI   0x1
#define NO_OS_I3C_IRQ_HJ    0x2
#define NO_OS_I3C_IRQ_CR    0x4
/**
 * Extract capabilities from BCR
 */
#define NO_OS_I3C_BCR_IBI_REQUEST_CAPABLE(bcr)    !!((bcr) & (1 << 1))
#define NO_OS_I3C_BCR_IBI_PAYLOAD(bcr)            !!((bcr) & (1 << 2))
#define NO_OS_I3C_BCR_DEVICE_ROLE(bcr)            (((bcr) && NO_OS_GENMASK(7,6)) >> 6)

/**
 * @enum no_os_i3c_slot_status - I3C address slot status
 *
 * @brief On an I3C bus, addresses are assigned dynamically, and we need to
 * know which addresses are free to use and which ones are already assigned.
 *
 * Addresses marked as reserved are those reserved by the I3C protocol
 * (broadcast address, ...).
 */
enum no_os_i3c_slot_status {
	/** Address is free */
	NO_OS_I3C_ADDR_SLOT_FREE,
	/** Address is reserved */
	NO_OS_I3C_ADDR_SLOT_RSVD,
	/** Address is assigned to an I2C device */
	NO_OS_I3C_ADDR_SLOT_I2C_DEV,
	/** Address is assigned to an I3C device */
	NO_OS_I3C_ADDR_SLOT_I3C_DEV,
	/** Address slot mask */
	NO_OS_I3C_ADDR_SLOT_STATUS_MASK = 3,
};

/**
 * @union no_os_i3c_ccc_info
 */
struct no_os_i3c_ccc_info {
	/** I3C bus receive Target Address during IBI or Bus Role Request event. */
	uint32_t ibi_cr_addr;
	/** I3C bus get Number of Data Payload after an IBI event. */
	uint32_t ibi_payload_len;
	/** I3C bus received IBI Payload. */
	uint32_t ibi_payload;
};

/**
 * @struct no_os_i3c_daa_lut
 * @brief Stores the PID + DA information to look-up during the DAA.
 * Should be used only for device initialization, since it may
 * go outdated.
 */
struct no_os_i3c_daa_lut {
	bool is_attached;
	uint8_t addr;
	uint64_t pid;
};

/**
 * @struct no_os_i3c_platform_ops
 * @brief Structure holding I3C function pointers that point to the platform
 * specific function.
 */
struct no_os_i3c_platform_ops;

/**
 * @struct no_os_i3c_msg
 * @brief Item describing an I3C transfer
 */
struct no_os_i3c_msg {
	/** Buffer with data to send */
	uint8_t			*tx_buff;
	/** Size of Tx buffer in number of bytes */
	uint32_t		tx_size;
	/** Buffer where to store data */
	uint8_t			*rx_buff;
	/** Size of Rx buffer in number of bytes */
	uint32_t		rx_size;
};

/**
 * @struct no_os_i3c_bus_desc
 * @brief Structure holding I3C bus descriptor.
 */
struct no_os_i3c_bus_desc {
	/** I3C bus mutex(lock)*/
	void*		mutex;
	/** Device ID*/
	uint8_t		device_id;
	/** Non-Expected number of devices on bus */
	uint8_t		num_devs_unknown;
	/** Expected number of devices on bus */
	uint8_t		num_devs;
	/**
	 * A bitmap with 2-bits per-slot to encode the address status and
	 * ease the DAA (Dynamic Address Assignment) procedure (see
	 * enum ::no_os_i3c_slot_status).
	 */
	unsigned int addrslots[NO_OS_I3C_ADDRSLOTS_SIZE];
	/** PID + DA LUT for DAA procedure */
	struct no_os_i3c_daa_lut    daa_candidates[NO_OS_I3C_MAX_DEV_NUMBER];
	/** To store unknown devices assigned during DAA procedure */
	struct no_os_i3c_daa_lut    daa_unknown[NO_OS_I3C_MAX_DEV_NUMBER];
	/** Devices descriptors */
	struct no_os_i3c_desc	*devs[NO_OS_I3C_MAX_DEV_NUMBER];
	/** IRQ flags */
	uint8_t irq_events;
	/** CCC info obtained after an IRQ */
	struct no_os_i3c_ccc_info	ccc_info;
	/** Trigger IRQ callback asyncronous (non-blocking) */
	bool                            async_irq;
	/** I3C platform specific functions */
	const struct no_os_i3c_platform_ops *platform_ops;
	/** I3C bus extra parameters */
	void		*extra;
};

/**
 * @struct no_os_i3c_bus_init_param
 * @brief Structure holding the parameters for I3C initialization.
 */
struct no_os_i3c_bus_init_param {
	/** Device ID */
	const uint32_t	device_id;
	/** I3C platform specific functions */
	const struct	no_os_i3c_platform_ops *platform_ops;
	/** Expected number of devices on bus */
	const uint8_t	num_devs;
	/** Expected devices on bus */
	const struct	no_os_i3c_init_param **devs;
	/** I3C extra parameters (device specific) */
	void		*extra;
};

/**
 * @struct no_os_i3c_init_param
 * @brief Structure holding the parameters for I3C initialization.
 * Works like a device tree, the values are copied to no_os_i3c_desc
 * during I3C initialization.
 * Set is_static to 1 if the device provides a Static Address and is
 * desired to use it instead of assigning a Dynamic Address during the
 * DAA.
 * is_i3c has higher precedence than is_static, and therefore
 * is_static does not matter when is_i3c is 0.
 */
struct no_os_i3c_init_param {
	/** Pointer to bus init param */
	struct no_os_i3c_bus_init_param	*bus;
	/** Provisioned ID */
	uint64_t	pid;
	/** Dynamic or static address */
	uint8_t		addr;
	/** Is the address static */
	bool		is_static;
	/** Is I3C or I2C */
	bool		is_i3c;
};

/**
 * @struct no_os_i3c_desc
 * @brief Structure holding I3C device descriptor.
 */
struct no_os_i3c_desc {
	/** Provisioned ID */
	uint64_t	pid;
	/** Dynamic or static address*/
	uint8_t		addr;
	/** Is I3C or I2C? */
	bool		is_i3c;
	/** Is attached? */
	bool		is_attached;
	/** Is the address static? */
	bool		is_static;
	/** I3C extra parameters (device specific) */
	void		*extra;
	/** Callback on event*/
	void (*event_callback)(struct no_os_i3c_desc*, uint32_t, uint32_t);
	/** I3C platform specific functions */
	const struct no_os_i3c_platform_ops *platform_ops;
	/** I3C bus */
	struct no_os_i3c_bus_desc *bus;
};

/**
 * @struct no_os_i3c_platform_ops
 * @brief Structure holding I3C function pointers that point to the platform
 * specific function.
 */
struct no_os_i3c_platform_ops {
	/** I3C initialization function pointer */
	int (*i3c_ops_init_bus)(struct no_os_i3c_bus_desc *,
				const struct no_os_i3c_bus_init_param *);
	/** I3C initialization function pointer */
	int (*i3c_ops_init)(struct no_os_i3c_desc *,
			    const struct no_os_i3c_init_param *);
	/** I3C do daa function pointer */
	int (*i3c_ops_do_daa)(struct no_os_i3c_bus_desc *, bool);
	/** I3C send ccc function pointer */
	int (*i3c_ops_send_ccc)(struct no_os_i3c_bus_desc *, uint8_t, uint8_t,
				bool, bool, uint8_t *, uint8_t);
	/** I3C private write function pointer */
	int (*i3c_ops_write)(struct no_os_i3c_desc *, uint8_t *, uint8_t);
	/** I3C private read function pointer */
	int (*i3c_ops_read)(struct no_os_i3c_desc *, uint8_t *, uint8_t);
	/** I3C private write and read function pointer */
	int (*i3c_ops_write_and_read)(struct no_os_i3c_desc *, uint8_t *, uint8_t,
				      uint8_t *, uint8_t);
	/** I3C remove function pointer */
	int (*i3c_ops_remove_bus)(struct no_os_i3c_bus_desc *);
	/** I3C remove function pointer */
	int (*i3c_ops_remove)(struct no_os_i3c_desc *);
	/** I3C check is device on the bus is ready */
	int (*i3c_ops_is_dev_ready)(struct no_os_i3c_desc *);
	/** I3C configure the enabled irq */
	int (*i3c_ops_conf_irq)(struct no_os_i3c_bus_desc *,
				uint8_t, bool);
	/** I3C fetch CCC info */
	int (*i3c_ops_get_ccc_info)(struct no_os_i3c_bus_desc *,
				    uint8_t);
	/** I3C DMA transfer */
	int32_t (*i3c_ops_transfer_dma_async)(struct no_os_i3c_desc *,
					      struct no_os_i3c_msg *,
					      uint32_t,
					      void (*)(void *),
					      void *);
	/** Abort DMA I3C trnasaction */
	int32_t (*i3c_ops_abort_dma)(struct no_os_i3c_desc *);
};

/* Initialize the I3C device. */
int no_os_i3c_init(struct no_os_i3c_desc **desc,
		   const struct no_os_i3c_init_param *param);

/* Initialize the I3C bus. */
int no_os_i3c_init_bus(struct no_os_i3c_bus_desc **desc,
		       const struct no_os_i3c_bus_init_param *param);

/* Free the resources allocated by no_os_i3c_init. */
int no_os_i3c_remove(struct no_os_i3c_desc *desc);

/* Free the resources allocated by no_os_i3c_init_bus + no_os_i3c_init. */
int no_os_i3c_remove_bus(struct no_os_i3c_bus_desc *desc);

/* Do I3C DAA to assign the dynamic addresses. */
int no_os_i3c_do_daa(struct no_os_i3c_bus_desc *desc, bool rstdaa);

/* Send CCC on the bus (direct or broadcast). */
int no_os_i3c_send_ccc(struct no_os_i3c_bus_desc *desc,
		       uint8_t addr, uint32_t ccc, uint8_t *data);

/* Send CCC to a device device. */
int no_os_i3c_send_direct_ccc(struct no_os_i3c_desc *desc,
			      uint32_t ccc, uint8_t *data);

/* Write data to a device device. */
int no_os_i3c_write(struct no_os_i3c_desc *desc,
		    uint8_t *data,
		    uint8_t size);

/* Read data from a device device. */
int no_os_i3c_read(struct no_os_i3c_desc *desc,
		   uint8_t *data,
		   uint8_t size);

/* Write and read data from a device device. */
int no_os_i3c_write_and_read(struct no_os_i3c_desc *desc,
			     uint8_t *tx_data,
			     uint8_t tx_data_len,
			     uint8_t *rx_data,
			     uint8_t rx_data_len);

/* Configure enabled interrupt. */
int no_os_i3c_conf_irq(struct no_os_i3c_bus_desc *desc,
		       uint8_t irq, bool en);

/* Wait I3C interrupt. */
int no_os_i3c_wait_irq(struct no_os_i3c_bus_desc* desc,
		       uint8_t irq);

/* Non-blocking I3C interrupt. */
int no_os_i3c_call_irq(struct no_os_i3c_bus_desc* desc);

/* Enable I3C non-blocking interrupts. */
int no_os_i3c_async_irq(struct no_os_i3c_bus_desc *desc, bool en);

/* Get a free address. */
uint8_t no_os_i3c_addr_get_free(struct no_os_i3c_bus_desc *desc,
				uint8_t start_addr);

/* Set status of an address. */
void no_os_i3c_addr_set_status(struct no_os_i3c_bus_desc *desc,
			       uint8_t addr, enum no_os_i3c_slot_status status);

/* Attach event callback. */
void no_os_i3c_attach_callback(struct no_os_i3c_desc *desc,
			       void (*callback)(struct no_os_i3c_desc*, uint32_t, uint32_t));

/* Detach event callback. */
void no_os_i3c_detach_callback(struct no_os_i3c_desc *desc);

/*
 * Transfer a list of messages using DMA.
 * Non-blocking, invokes a callback after the last message is concluded.
 */
int32_t no_os_i3c_transfer_dma_async(struct no_os_i3c_desc *desc,
				     struct no_os_i3c_msg *msgs,
				     uint32_t len,
				     void (*callback)(void *),
				     void *ctx);

/* Abort the ongoing DMA transaction */
int32_t no_os_i3c_abort_dma(struct no_os_i3c_desc *desc);

#endif // _NO_OS_I3C_H_
