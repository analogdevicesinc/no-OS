/***************************************************************************//**
 *   @file   altera/altera_uart.c
 *   @brief  Altera / Nios V Avalon JTAG-UART driver.
 *   @author Mihaela-Georgeta Petrea (Mihaela-georgeta.Petrea@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
*******************************************************************************/

/*
 * The whole translation unit is a no-op unless building for the Altera / Nios V
 * platform, so the file can sit in a shared driver directory without affecting
 * MicroBlaze/Xilinx or any other target.
 */
#if defined(CONFIG_ALTERA_PLATFORM_NIOSV)

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdlib.h>
#include <stdbool.h>
#include "altera_uart.h"
#include "altera_irq.h"
#include "no_os_uart.h"
#include "no_os_alloc.h"
#include "no_os_error.h"
#include "no_os_util.h"

/* BSP HAL: core-local interrupt controller registration / line control. */
#include "sys/alt_irq.h"

/******************************************************************************/
/************************* Register / bit definitions *************************/
/******************************************************************************/
/*
 * Avalon JTAG-UART register map (word-addressed, 32-bit):
 *   +0x00 DATA    : write -> TX byte in bits[7:0];
 *                   read  -> bit15 RVALID, bits[7:0] RX byte (read pops FIFO).
 *   +0x04 CONTROL : bits[31:16] WSPACE (TX FIFO free space, read-only),
 *                   bit0 RE (RX interrupt enable), bit1 WE (TX interrupt enable),
 *                   bit8 RI (RX interrupt pending), bit9 WI (TX interrupt pending).
 */
#define ALT_JUART_DATA_OFFSET		0x00
#define ALT_JUART_CTRL_OFFSET		0x04
#define ALT_JUART_DATA_RVALID		NO_OS_BIT(15)
#define ALT_JUART_CTRL_WSPACE_SHIFT	16
#define ALT_JUART_CTRL_RE		NO_OS_BIT(0)

/*
 * TX polling budget: must exceed an attached juart-terminal's FIFO-drain
 * latency, yet stay bounded so a host-less run does not hang. Matches the
 * budget used by the parameters.c _write() console shim.
 */
#define ALT_JUART_WSPACE_SPINS		200000000UL

/******************************************************************************/
/************************ Static helper functions *****************************/
/******************************************************************************/

static inline volatile uint32_t *altera_uart_data_reg(
	const struct altera_uart_desc *aldesc)
{
	return (volatile uint32_t *)(aldesc->base_address + ALT_JUART_DATA_OFFSET);
}

static inline volatile uint32_t *altera_uart_ctrl_reg(
	const struct altera_uart_desc *aldesc)
{
	return (volatile uint32_t *)(aldesc->base_address + ALT_JUART_CTRL_OFFSET);
}

/**
 * @brief Push one received byte into the software RX ring (drop on overrun).
 */
static inline void altera_uart_ring_push(struct altera_uart_desc *aldesc,
		uint8_t byte)
{
	uint32_t next = (aldesc->rx_head + 1) % ALTERA_UART_RX_RING_SIZE;

	if (next == aldesc->rx_tail) {
		aldesc->rx_overruns++;
		return;
	}

	aldesc->rx_ring[aldesc->rx_head] = byte;
	aldesc->rx_head = next;
}

/**
 * @brief Pop one byte from the software RX ring.
 * @return true if a byte was returned, false if the ring was empty.
 */
static inline bool altera_uart_ring_pop(struct altera_uart_desc *aldesc,
					uint8_t *byte)
{
	if (aldesc->rx_head == aldesc->rx_tail)
		return false;

	*byte = aldesc->rx_ring[aldesc->rx_tail];
	aldesc->rx_tail = (aldesc->rx_tail + 1) % ALTERA_UART_RX_RING_SIZE;

	return true;
}

/**
 * @brief Drain every byte currently in the JTAG-UART RX FIFO into the ring.
 *
 * Reading DATA pops the hardware FIFO, so RVALID must be captured from the same
 * read whose byte is consumed.
 */
static inline void altera_uart_drain_hw(struct altera_uart_desc *aldesc)
{
	volatile uint32_t *data = altera_uart_data_reg(aldesc);
	uint32_t v;

	while ((v = *data) & ALT_JUART_DATA_RVALID)
		altera_uart_ring_push(aldesc, (uint8_t)(v & 0xFF));
}

/**
 * @brief JTAG-UART RX interrupt service routine.
 *
 * Registered through the HAL; ctx is the altera_uart_desc. Drains the hardware
 * FIFO into the software ring, which also clears the RX interrupt condition.
 */
static void altera_uart_rx_isr(void *ctx)
{
	altera_uart_drain_hw((struct altera_uart_desc *)ctx);
}

/**
 * @brief Block until one byte can be returned from the RX path.
 *
 * Fast path pops the ring (filled by the ISR). When the ring is empty the UART
 * interrupt line is briefly masked so this consumer becomes the sole reader of
 * the DATA register (race-free against the ISR), the hardware FIFO is polled
 * directly, then the line is re-enabled. This mirrors the hybrid ISR + polling
 * read of Altera's own JTAG-UART driver and is immune to the RX FIFO read
 * threshold (short IIOD commands would not otherwise raise the interrupt).
 */
static uint8_t altera_uart_getc(struct altera_uart_desc *aldesc)
{
	volatile uint32_t *data = altera_uart_data_reg(aldesc);
	uint8_t byte;
	uint32_t v;

	if (altera_uart_ring_pop(aldesc, &byte))
		return byte;

	if (!aldesc->irq_rx) {
		/* Pure polled mode: wait on the hardware FIFO directly. */
		while (!((v = *data) & ALT_JUART_DATA_RVALID))
			;
		return (uint8_t)(v & 0xFF);
	}

	/* Interrupt mode: enter a race-free critical section vs the ISR. */
	alt_ic_irq_disable(aldesc->irq_ctrl_id, aldesc->irq_id);

	/* The ISR may have filled the ring just before we masked the line. */
	if (!altera_uart_ring_pop(aldesc, &byte)) {
		while (!((v = *data) & ALT_JUART_DATA_RVALID))
			;
		altera_uart_ring_push(aldesc, (uint8_t)(v & 0xFF));
		altera_uart_drain_hw(aldesc);
		altera_uart_ring_pop(aldesc, &byte);
	}

	alt_ic_irq_enable(aldesc->irq_ctrl_id, aldesc->irq_id);

	return byte;
}

/**
 * @brief Write one byte, polling the TX FIFO free space (bounded).
 * @return 0 if the byte was written, -ETIMEDOUT if no host was draining.
 */
static int altera_uart_putc(struct altera_uart_desc *aldesc, uint8_t byte)
{
	volatile uint32_t *data = altera_uart_data_reg(aldesc);
	volatile uint32_t *ctrl = altera_uart_ctrl_reg(aldesc);
	unsigned long spins = ALT_JUART_WSPACE_SPINS;

	while (((*ctrl) >> ALT_JUART_CTRL_WSPACE_SHIFT) == 0) {
		if (--spins == 0)
			return -ETIMEDOUT; /* no host draining - drop */
	}

	*data = byte;

	return 0;
}

/******************************************************************************/
/************************ Platform ops implementations ************************/
/******************************************************************************/

/**
 * @brief Initialize the Altera / Nios V JTAG UART.
 * @param desc  - Pointer where the descriptor is stored.
 * @param param - Initialization parameters (extra -> altera_uart_init_param).
 * @return 0 in case of success, negative error code otherwise.
 */
static int32_t altera_uart_init(struct no_os_uart_desc **desc,
				const struct no_os_uart_init_param *param)
{
	struct altera_uart_init_param *alt_param;
	struct no_os_uart_desc *ldesc;
	struct altera_uart_desc *aldesc;

	if (!desc || !param || !param->extra)
		return -EINVAL;

	alt_param = param->extra;

	ldesc = no_os_calloc(1, sizeof(*ldesc));
	if (!ldesc)
		return -ENOMEM;

	aldesc = no_os_calloc(1, sizeof(*aldesc));
	if (!aldesc) {
		no_os_free(ldesc);
		return -ENOMEM;
	}

	aldesc->base_address = alt_param->base_address;
	aldesc->irq_id = alt_param->irq_id;
	aldesc->irq_ctrl_id = alt_param->irq_ctrl_id;
	aldesc->irq_rx = param->asynchronous_rx;

	ldesc->device_id = param->device_id;
	ldesc->irq_id = param->irq_id;
	ldesc->baud_rate = param->baud_rate;
	ldesc->platform_ops = param->platform_ops;
	ldesc->extra = aldesc;

	if (aldesc->irq_rx) {
		volatile uint32_t *ctrl = altera_uart_ctrl_reg(aldesc);
		int ret;

		/* Bring up the interrupt controller HW exactly once. */
		altera_irq_hw_init_once();

		/*
		 * Register the RX handler. On success the HAL also enables the
		 * interrupt line at the controller.
		 */
		ret = alt_ic_isr_register(aldesc->irq_ctrl_id, aldesc->irq_id,
					  altera_uart_rx_isr, aldesc, NULL);
		if (ret) {
			no_os_free(aldesc);
			no_os_free(ldesc);
			return ret;
		}

		/* Arm the JTAG-UART RX interrupt output (CONTROL bit0 RE). */
		*ctrl |= ALT_JUART_CTRL_RE;
	}

	*desc = ldesc;

	return 0;
}

/**
 * @brief Read a number of bytes (blocking) from the JTAG UART.
 * @param desc        - The UART descriptor.
 * @param data        - Destination buffer.
 * @param bytes_number - Number of bytes to read.
 * @return Number of bytes read (== bytes_number), negative error otherwise.
 */
static int32_t altera_uart_read(struct no_os_uart_desc *desc, uint8_t *data,
				uint32_t bytes_number)
{
	struct altera_uart_desc *aldesc;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	aldesc = desc->extra;

	for (i = 0; i < bytes_number; i++)
		data[i] = altera_uart_getc(aldesc);

	return (int32_t)bytes_number;
}

/**
 * @brief Write a number of bytes (blocking) to the JTAG UART.
 * @param desc        - The UART descriptor.
 * @param data        - Source buffer.
 * @param bytes_number - Number of bytes to write.
 * @return Number of bytes written, negative error otherwise.
 */
static int32_t altera_uart_write(struct no_os_uart_desc *desc,
				 const uint8_t *data, uint32_t bytes_number)
{
	struct altera_uart_desc *aldesc;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	aldesc = desc->extra;

	for (i = 0; i < bytes_number; i++) {
		if (altera_uart_putc(aldesc, data[i]))
			break; /* host not draining - return short count */
	}

	return (int32_t)i;
}

/**
 * @brief Non-blocking read: return whatever is already available in the ring.
 */
static int32_t altera_uart_read_nonblocking(struct no_os_uart_desc *desc,
		uint8_t *data, uint32_t bytes_number)
{
	struct altera_uart_desc *aldesc;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	aldesc = desc->extra;

	/* Fold in anything sitting in the hardware FIFO first (polled mode). */
	if (!aldesc->irq_rx)
		altera_uart_drain_hw(aldesc);

	for (i = 0; i < bytes_number; i++) {
		if (!altera_uart_ring_pop(aldesc, &data[i]))
			break;
	}

	return (int32_t)i;
}

/**
 * @brief Non-blocking write: push only as many bytes as the TX FIFO can take.
 */
static int32_t altera_uart_write_nonblocking(struct no_os_uart_desc *desc,
		const uint8_t *data, uint32_t bytes_number)
{
	struct altera_uart_desc *aldesc;
	volatile uint32_t *ctrl;
	volatile uint32_t *dreg;
	uint32_t i;

	if (!desc || !desc->extra || !data)
		return -EINVAL;

	aldesc = desc->extra;
	ctrl = altera_uart_ctrl_reg(aldesc);
	dreg = altera_uart_data_reg(aldesc);

	for (i = 0; i < bytes_number; i++) {
		if (((*ctrl) >> ALT_JUART_CTRL_WSPACE_SHIFT) == 0)
			break;
		*dreg = data[i];
	}

	return (int32_t)i;
}

/**
 * @brief Free resources allocated by altera_uart_init().
 */
static int32_t altera_uart_remove(struct no_os_uart_desc *desc)
{
	struct altera_uart_desc *aldesc;

	if (!desc)
		return -EINVAL;

	aldesc = desc->extra;
	if (aldesc) {
		if (aldesc->irq_rx) {
			volatile uint32_t *ctrl = altera_uart_ctrl_reg(aldesc);

			*ctrl &= ~ALT_JUART_CTRL_RE;
			alt_ic_irq_disable(aldesc->irq_ctrl_id, aldesc->irq_id);
		}
		no_os_free(aldesc);
	}

	no_os_free(desc);

	return 0;
}

/**
 * @brief Return the count of bytes dropped due to RX ring overrun.
 */
static uint32_t altera_uart_get_errors(struct no_os_uart_desc *desc)
{
	struct altera_uart_desc *aldesc;

	if (!desc || !desc->extra)
		return 0;

	aldesc = desc->extra;

	return aldesc->rx_overruns;
}

/**
 * @brief Altera / Nios V specific UART platform ops structure.
 */
const struct no_os_uart_platform_ops altera_uart_ops = {
	.init = &altera_uart_init,
	.read = &altera_uart_read,
	.write = &altera_uart_write,
	.read_nonblocking = &altera_uart_read_nonblocking,
	.write_nonblocking = &altera_uart_write_nonblocking,
	.remove = &altera_uart_remove,
	.get_errors = &altera_uart_get_errors,
};

#endif /* CONFIG_ALTERA_PLATFORM_NIOSV */
