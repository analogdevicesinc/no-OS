/***************************************************************************//**
 *   @file   maxim_i2c.c
 *   @brief  Implementation of max32670 I2C driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
 *   @author Jose San Buenaventura (jose.sanbuenaventura@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "i2c.h"
#include "i2c_reva.h"
#include "no_os_error.h"
#include "no_os_util.h"
#include "no_os_alloc.h"
#include "maxim_i2c.h"
#include "mxc_errors.h"
#include "max32670.h"

/*
 * The MSDK has no public (non-RevA) versions of the I2C constants used here
 * (MXC_I2C_REVA_ERROR, MXC_I2C_REVA_MAX_FIFO_TRANSACTION,
 * MXC_I2C_REVA_MAX_ADDR_WIDTH and MXC_I2C_REVA_INTFL0/1_MASK), so the RevA
 * ones are used directly.
 */

#define I2C_MASTER_MODE	1
/*
 * Poll iterations for the bus to go idle: several ms at typical CPU clocks,
 * while a STOP condition takes about 10 us at 100 kHz.
 */
#define I2C_BUS_IDLE_TIMEOUT	100000
/*
 * Longest wait for the next byte of a long read, in ms: the SMBus maximum
 * clock low time (tTIMEOUT), after which the bus is considered stuck.
 */
#define I2C_BYTE_TIMEOUT_MS	35

/** Used to know how many instances are created */
static uint32_t nb_created_desc[MXC_I2C_INSTANCES];

/** Set when the last transaction on a bus ended with a repeated START */
static bool restart_pending[MXC_I2C_INSTANCES];

/**
 * @brief I2C0 interrupt handler.
 */
void I2C0_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C0);
}

/**
 * @brief I2C1 interrupt handler.
 */
void I2C1_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C1);
}

#ifdef MXC_I2C2
void I2C2_IRQHandler(void)
{
	MXC_I2C_AsyncHandler(MXC_I2C2);
}
#endif

/**
 * @brief Configure the VDDIO level for a I2C interface
 * @param device_id - the interface number.
 * @param vssel - the VDDIO level.
 * @return 0 in case of success, -EINVAL otherwise.
 */
static int32_t _max_i2c_pins_config(uint32_t device_id, mxc_gpio_vssel_t vssel)
{
	mxc_gpio_cfg_t i2c_pins;

	switch (device_id) {
	case 0:
		i2c_pins = gpio_cfg_i2c0;
		break;
	case 1:
		i2c_pins = gpio_cfg_i2c1;
		break;
	case 2:
		i2c_pins = gpio_cfg_i2c2;
		break;
	default:
		return -EINVAL;
	}

	i2c_pins.vssel = vssel;
	MXC_GPIO_Config(&i2c_pins);

	return 0;
}

/**
 * @brief Wait for the bus to be released by the previous transaction.
 *
 * MXC_I2C_MasterTransaction() can return while the STOP condition is still
 * being generated (STATUS.BUSY set). A START issued in that window is lost by
 * MSDK v2024_10, leaving the transaction polling forever, and rejected with
 * E_BUSY by later MSDK releases (fixed in analogdevicesinc/msdk#1640).
 *
 * After a transaction that ended with a repeated START this controller still
 * holds the bus on purpose, so there is nothing to wait for. Otherwise wait
 * for both STATUS.BUSY and STATUS.MST_BUSY to clear: MST_BUSY is also still
 * set while the STOP is being generated, so it cannot tell the two cases
 * apart on its own.
 *
 * @param device_id - I2C controller index.
 * @param i2c_regs - I2C controller registers.
 * @return 0 when the bus is available, -ETIMEDOUT otherwise.
 */
static int32_t _max_i2c_wait_bus_idle(uint32_t device_id,
				      mxc_i2c_regs_t *i2c_regs)
{
	uint32_t busy = MXC_F_I2C_STATUS_BUSY | MXC_F_I2C_STATUS_MST_BUSY;
	uint32_t timeout = I2C_BUS_IDLE_TIMEOUT;

	if (restart_pending[device_id])
		return 0;

	while (i2c_regs->status & busy) {
		if (!--timeout)
			return -ETIMEDOUT;
	}

	return 0;
}

/**
 * @brief Initialize the I2C communication peripheral.
 * Supported bitrates are between 100kHz and 400 kHz.
 * Is slave address is 0, then this instance will be used for general call.
 * 10 bits addressing is not supported.
 * @param desc - Descriptor of the I2C device used in the call of the driver
 * functions.
 * @param param - Parameter used to configure the I2C device. The extra field
 * it is not used and must be set to NULL.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_init(struct no_os_i2c_desc **desc,
			    const struct no_os_i2c_init_param *param)
{
	struct max_i2c_init_param *eparam;
	int32_t ret;
	struct max_i2c_extra *max_i2c;
	mxc_i2c_regs_t *i2c_regs;
	uint32_t freq;

	if (!desc || !param || !param->extra)
		return -EINVAL;
	if (param->device_id >= MXC_I2C_INSTANCES)
		return -EINVAL;

	*desc = no_os_calloc(1, sizeof(**desc));
	if (!(*desc))
		return -ENOMEM;

	max_i2c = no_os_calloc(1, sizeof(*max_i2c));
	if (!max_i2c) {
		ret = -ENOMEM;
		goto error_desc;
	}

	eparam = param->extra;
	i2c_regs = MXC_I2C_GET_I2C(param->device_id);
	max_i2c->handler = i2c_regs;
	(*desc)->device_id = param->device_id;
	if (param->max_speed_hz <= MAX_I2C_STD_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_STD_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_FAST_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_FAST_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_FAST_PLUS_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_FAST_PLUS_MODE;
	} else if (param->max_speed_hz <= MAX_I2C_HS_MODE) {
		(*desc)->max_speed_hz = MAX_I2C_HS_MODE;
	} else {
		ret = -EINVAL;
		goto error_extra;
	}
	(*desc)->slave_address = param->slave_address;
	(*desc)->extra = max_i2c;

	if (nb_created_desc[param->device_id] == 0) {
		MXC_I2C_Shutdown(max_i2c->handler);
		/** The last parameter (slave address) is ignored in master mode */
		if ((MXC_I2C_Init(i2c_regs, I2C_MASTER_MODE, 0)) != E_NO_ERROR) {
			ret = -1;
			goto error_extra;
		}
	}

	ret = _max_i2c_pins_config((*desc)->device_id, eparam->vssel);
	if (ret)
		return ret;

	freq = MXC_I2C_GetFrequency(i2c_regs);
	freq = no_os_min(freq, (*desc)->max_speed_hz);
	ret = MXC_I2C_SetFrequency(i2c_regs, freq);
	if (ret < 0)
		return -EINVAL;

	nb_created_desc[param->device_id]++;

	return 0;
error_extra:
	no_os_free(max_i2c);
error_desc:
	no_os_free(*desc);

	return ret;
}

/**
 * @brief Free the resources allocated by \ref i2c_init
 * @param desc - Descriptor of the I2C device
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_remove(struct no_os_i2c_desc *desc)
{
	struct max_i2c_extra *max_i2c = desc->extra;
	int32_t ret;

	if (!desc)
		return -EINVAL;
	nb_created_desc[desc->device_id]--;

	if (nb_created_desc[desc->device_id] == 0) {
		ret = MXC_I2C_Shutdown(max_i2c->handler);
		if (ret != E_NO_ERROR)
			return ret;
		NVIC_DisableIRQ(MXC_I2C_GET_IRQ(desc->device_id));
	}

	/* A write without a stop bit not followed by a read */
	if (max_i2c->prologue_data)
		no_os_free(max_i2c->prologue_data);
	no_os_free(max_i2c);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Write data to a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control.
 *                   Example: 0 - A stop condition will not be generated;
 *                            1 - A stop condition will be generated.
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_write(struct no_os_i2c_desc *desc,
			     uint8_t *data,
			     uint32_t bytes_number,
			     uint8_t stop_bit)
{
	mxc_i2c_regs_t *i2c_regs;
	mxc_i2c_req_t req;
	int32_t ret;
	struct max_i2c_extra *max_i2c_desc;
	void *ptr;

	if (!desc || !desc->extra)
		return -EINVAL;

	i2c_regs = MXC_I2C_GET_I2C(desc->device_id);
	max_i2c_desc = desc->extra;

	if (stop_bit == 0) {
		/* Drop any previous prologue, even if this one fails */
		if (max_i2c_desc->prologue_data)
			no_os_free(max_i2c_desc->prologue_data);
		max_i2c_desc->prologue_data = NULL;
		max_i2c_desc->prologue_size = 0;

		if (bytes_number) {
			ptr = no_os_malloc(bytes_number);
			if (!ptr)
				return -ENOMEM;
			memcpy(ptr, data, bytes_number);
			max_i2c_desc->prologue_data = ptr;
			max_i2c_desc->prologue_size = bytes_number;
		}

		return 0;
	}

	req.i2c = i2c_regs;
	req.addr = desc->slave_address;
	req.tx_buf = data;
	req.tx_len = bytes_number;
	/** Write transaction */
	req.rx_len = 0;
	req.rx_buf = data;
	req.restart = 0;

	ret = _max_i2c_wait_bus_idle(desc->device_id, i2c_regs);
	if (ret)
		return ret;

	ret = MXC_I2C_MasterTransaction(&req);
	restart_pending[desc->device_id] = false;

	return ret;
}

/**
 * @brief Wait for a controller flag, failing on a bus error.
 * @param regs - I2C peripheral registers.
 * @param flag - INTFL0 flag to wait for.
 * @return 0 when the flag is set, -EIO on a bus error.
 */
static int32_t _max_i2c_wait_flag(mxc_i2c_regs_t *regs, uint32_t flag)
{
	while (!(regs->intfl0 & flag)) {
		if (regs->intfl0 & MXC_I2C_REVA_ERROR)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Wait for a requested repeated START to be sent.
 * @param regs - I2C peripheral registers.
 * @return 0 once the repeated START is sent, -EIO on a bus error.
 */
static int32_t _max_i2c_wait_restart(mxc_i2c_regs_t *regs)
{
	while (regs->mstctrl & MXC_F_I2C_MSTCTRL_RESTART) {
		if (regs->intfl0 & MXC_I2C_REVA_ERROR)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Wait for a requested STOP to be on the bus.
 *
 * MSTCTRL.stop clears when the STOP condition starts, not when it is done,
 * so wait for the bus to go idle instead.
 * @param regs - I2C peripheral registers.
 * @return 0 once the bus is idle, -EIO on a bus error.
 */
static int32_t _max_i2c_wait_idle(mxc_i2c_regs_t *regs)
{
	while (regs->status & MXC_F_I2C_STATUS_BUSY) {
		if (regs->intfl0 & MXC_I2C_REVA_ERROR)
			return -EIO;
	}

	return 0;
}

/**
 * @brief Read more than MXC_I2C_REVA_MAX_FIFO_TRANSACTION bytes as one message.
 *
 * The receive byte count (RXCTRL1.cnt) holds at most 256 bytes, and
 * MXC_I2C_MasterTransaction() splits a longer read into messages joined by
 * repeated STARTs, which a target streaming one block (an SMBus block read)
 * does not accept.
 *
 * Interactive receive mode (IRXM) ignores the byte count. With IRXM enabled
 * the controller holds SCL before the ACK bit of each received byte, sets
 * INTFL0.irxm and leaves the byte in the receive shift register, which FIFO
 * reads directly (the receive FIFO stays empty). Clearing INTFL0.irxm sends
 * the ACK selected by CTRL.irxm_ack and releases SCL.
 *
 * The first rx_len - MXC_I2C_REVA_MAX_FIFO_TRANSACTION bytes are received in
 * IRXM. While the last of them still waits for its ACK, IRXM is disabled and
 * the byte count is loaded with the 256 bytes that remain, so they are
 * received through the FIFO and the controller NACKs the last one itself.
 *
 * CTRL.irxm_en must only be set while the bus is idle, so a read that
 * continues a transaction left open by a repeated START is not supported.
 * With the bus timeout enabled (TIMEOUT.scl_to_val), the time spent handling
 * each IRXM byte counts towards it, since the controller holds SCL low
 * meanwhile.
 *
 * @param regs - I2C peripheral, in controller mode.
 * @param addr - 7-bit target address.
 * @param tx - Bytes to write before the read, or NULL.
 * @param tx_len - Number of bytes in tx. When non-zero, the read follows
 *                 after a repeated START.
 * @param rx - Output, rx_len bytes.
 * @param rx_len - Number of bytes to read, more than
 *                 MXC_I2C_REVA_MAX_FIFO_TRANSACTION.
 * @param restart - End with a repeated START instead of a STOP.
 * @return 0 on success, -EINVAL for invalid arguments, -EBUSY if the bus is
 *         not idle, -EIO on a NACK or another bus error, -ETIMEDOUT if no
 *         byte arrives for I2C_BYTE_TIMEOUT_MS.
 */
static int32_t _max_i2c_long_read(mxc_i2c_regs_t *regs, uint8_t addr,
				  uint8_t *tx, uint32_t tx_len, uint8_t *rx,
				  uint32_t rx_len, bool restart)
{
	uint32_t irxm_len, written = 0, read = 0;
	uint32_t timeout, byte_timeout;
	bool irxm = true;
	int32_t ret;
	int count;

	if (!rx || rx_len <= MXC_I2C_REVA_MAX_FIFO_TRANSACTION ||
	    (tx_len && !tx) || addr > MXC_I2C_REVA_MAX_ADDR_WIDTH)
		return -EINVAL;

	if (!(regs->ctrl & MXC_F_I2C_CTRL_MST_MODE))
		return -EINVAL;

	if (regs->status & (MXC_F_I2C_STATUS_BUSY |
			    MXC_F_I2C_STATUS_MST_BUSY))
		return -EBUSY;

	irxm_len = rx_len - MXC_I2C_REVA_MAX_FIFO_TRANSACTION;

	MXC_I2C_ClearFlags(regs, MXC_I2C_REVA_INTFL0_MASK,
			   MXC_I2C_REVA_INTFL1_MASK);
	MXC_I2C_ClearTXFIFO(regs);
	MXC_I2C_ClearRXFIFO(regs);
	regs->inten0 = 0;
	regs->inten1 = 0;

	/* IRXM must be enabled before the transfer starts; it ACKs each byte */
	regs->ctrl = (regs->ctrl & ~MXC_F_I2C_CTRL_IRXM_ACK) |
		     MXC_F_I2C_CTRL_IRXM_EN;

	if (tx_len) {
		regs->fifo = addr << 1;
		regs->mstctrl |= MXC_F_I2C_MSTCTRL_START;

		while (written < tx_len) {
			if (regs->intfl0 & MXC_I2C_REVA_ERROR) {
				ret = -EIO;
				goto stop;
			}

			if (regs->intfl0 & MXC_F_I2C_INTFL0_TX_THD) {
				written += MXC_I2C_WriteTXFIFO(regs, &tx[written],
							       tx_len - written);
				regs->intfl0 = MXC_F_I2C_INTFL0_TX_THD;
			}
		}

		regs->intfl0 = MXC_F_I2C_INTFL0_DONE |
			       MXC_F_I2C_INTFL0_RX_THD;
		regs->mstctrl |= MXC_F_I2C_MSTCTRL_RESTART;
		ret = _max_i2c_wait_restart(regs);
		if (ret)
			goto stop;

		regs->fifo = (addr << 1) | 1;

		/* DONE marks the end of the write message */
		ret = _max_i2c_wait_flag(regs, MXC_F_I2C_INTFL0_DONE);
		if (ret)
			goto stop;
		regs->intfl0 = MXC_F_I2C_INTFL0_DONE;
	} else {
		regs->fifo = (addr << 1) | 1;
		regs->mstctrl |= MXC_F_I2C_MSTCTRL_START;
	}

	/*
	 * Each poll takes at least one core clock cycle, so this waits at least
	 * I2C_BYTE_TIMEOUT_MS. It restarts with every byte received, so it
	 * doesn't grow with rx_len.
	 */
	byte_timeout = SystemCoreClock / 1000 * I2C_BYTE_TIMEOUT_MS;
	timeout = byte_timeout;

	while (read < rx_len) {
		if (regs->intfl0 & MXC_I2C_REVA_ERROR) {
			ret = -EIO;
			goto stop;
		}

		if (!--timeout) {
			ret = -ETIMEDOUT;
			goto stop;
		}

		if (!irxm) {
			count = MXC_I2C_ReadRXFIFO(regs, &rx[read], rx_len - read);
			if (count > 0) {
				read += count;
				timeout = byte_timeout;
			}
			continue;
		}

		if (!(regs->intfl0 & MXC_F_I2C_INTFL0_IRXM))
			continue;

		/* Read the byte before IRXM is disabled, or it is lost */
		rx[read++] = regs->fifo & MXC_F_I2C_FIFO_DATA;
		timeout = byte_timeout;

		if (read == irxm_len) {
			regs->ctrl &= ~MXC_F_I2C_CTRL_IRXM_EN;
			/* 0 means 256, the bytes still to receive */
			regs->rxctrl1 = 0;
			irxm = false;
		}

		/* Send the ACK and release SCL */
		regs->intfl0 = MXC_F_I2C_INTFL0_IRXM;
	}

	/*
	 * DONE is already set once the byte count runs out, so wait for the
	 * STOP or repeated START itself. It is already requested, so an error
	 * here must not request another STOP.
	 */
	if (restart) {
		regs->mstctrl |= MXC_F_I2C_MSTCTRL_RESTART;
		ret = _max_i2c_wait_restart(regs);
	} else {
		regs->mstctrl |= MXC_F_I2C_MSTCTRL_STOP;
		ret = _max_i2c_wait_idle(regs);
	}

	regs->intfl0 = MXC_F_I2C_INTFL0_DONE | MXC_F_I2C_INTFL0_STOP;

	return ret;

stop:
	/* A lost arbitration or a bus timeout already released the bus */
	if (!(regs->intfl0 & (MXC_F_I2C_INTFL0_ARB_ERR |
			      MXC_F_I2C_INTFL0_TO_ERR)))
		MXC_I2C_Stop(regs);
	regs->ctrl &= ~(MXC_F_I2C_CTRL_IRXM_EN | MXC_F_I2C_CTRL_IRXM_ACK);

	return ret;
}

/**
 * @brief Read data from a slave device
 * @param desc - Descriptor of the I2C device
 * @param data - Buffer that stores the transmission data.
 * @param bytes_number - Number of bytes to write.
 * @param stop_bit - Stop condition control. NOTE: not applicable in this case
 *                   Example: 0 - A stop condition will not be generated.
 *                            1 - A stop condition will be generated
 * @return 0 in case of success, -1 otherwise.
 */
static int32_t max_i2c_read(struct no_os_i2c_desc *desc,
			    uint8_t *data,
			    uint32_t bytes_number,
			    uint8_t stop_bit)
{
	mxc_i2c_regs_t *i2c_regs;
	mxc_i2c_req_t req;
	int32_t ret;
	struct max_i2c_extra *max_i2c_desc;

	if (!desc || !desc->extra)
		return -EINVAL;

	max_i2c_desc = desc->extra;
	i2c_regs = MXC_I2C_GET_I2C(desc->device_id);
	req.i2c = i2c_regs;
	req.addr = desc->slave_address;
	/** Read transaction */
	req.tx_len = 0;
	req.tx_buf = NULL;
	req.rx_buf = data;
	req.rx_len = bytes_number;
	req.restart = stop_bit ? 0 : 1;

	if (max_i2c_desc->prologue_size != 0) {
		req.restart = 0;
		req.tx_len = max_i2c_desc->prologue_size;
		req.tx_buf = max_i2c_desc->prologue_data;
	}

	/*
	 * A long read can't continue a transaction left open by a repeated
	 * START. Reject it without touching the bus, which stays held for the
	 * next transfer to close.
	 */
	if (bytes_number > MXC_I2C_REVA_MAX_FIFO_TRANSACTION &&
	    restart_pending[desc->device_id])
		ret = -EBUSY;
	else
		ret = _max_i2c_wait_bus_idle(desc->device_id, i2c_regs);

	if (!ret) {
		/* MXC_I2C_MasterTransaction() splits longer reads with repeated STARTs */
		if (bytes_number > MXC_I2C_REVA_MAX_FIFO_TRANSACTION)
			ret = _max_i2c_long_read(i2c_regs, desc->slave_address,
						 req.tx_buf, req.tx_len, data,
						 bytes_number, req.restart);
		else
			ret = MXC_I2C_MasterTransaction(&req);
		restart_pending[desc->device_id] = !ret && req.restart;
	}

	if (max_i2c_desc->prologue_size != 0) {
		no_os_free(max_i2c_desc->prologue_data);
		max_i2c_desc->prologue_size = 0;
		max_i2c_desc->prologue_data = NULL;
	}

	return ret;
}

/**
 * @brief MAXIM platform specific I2C platform ops structure
 */
const struct no_os_i2c_platform_ops max_i2c_ops = {
	.i2c_ops_init = &max_i2c_init,
	.i2c_ops_write = &max_i2c_write,
	.i2c_ops_read = &max_i2c_read,
	.i2c_ops_remove = &max_i2c_remove
};
