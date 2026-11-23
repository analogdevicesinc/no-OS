/*******************************************************************************
 *   @file   maxim_capi_i2c.c
 *   @brief  Implementation of I2C functions with CAPI
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include <errno.h>
#include <string.h>
#include "capi_i2c.h"
#include "capi_dma.h"
#include "capi_irq.h"
#include "maxim_capi_i2c.h"
#include "maxim_capi_i2c_priv.h"
#include "maxim_capi_dma.h"
#include "maxim_capi_irq.h"
#include "i3c.h"

/** Static variables **********************************************************/

static struct capi_i2c_controller_handle *i2c[MXC_CFG_I3C_INSTANCES] = {NULL};

/** Forward declarations ******************************************************/

static void max_capi_i2c_isr(void *handle);
static int max_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed,
		uint8_t duty_cycle);

/** Helper functions **********************************************************/

/**
 * @brief Set TX data for target mode
 * @param i2c_priv The I2C private data
 * @param data Pointer to TX data buffer
 * @param length Number of bytes to transmit
 */
static void _max_capi_i2c_target_set_tx_data(struct max_capi_i2c_priv *i2c_priv,
		const uint8_t *data, uint16_t length)
{
	mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

	i2c_priv->target->tx_buffer = data;
	i2c_priv->target->tx_length = length;
	i2c_priv->target->tx_count = 0;

	MXC_I3C_Target_EnableInt(i3c, MXC_F_I3C_TARG_INTEN_TX_NFULL);
}

/**
 * @brief Get RX data for target mode
 * @param i2c_priv The I2C private data
 * @param[out] data Pointer to RX data buffer
 * @param[out] length Number of bytes received
 */
static void _max_capi_i2c_target_get_rx_data(struct max_capi_i2c_priv *i2c_priv,
		uint8_t **data, uint16_t *length)
{
	*data = i2c_priv->target->rx_buffer;
	*length = i2c_priv->target->rx_count;
}


/**
 * @brief Get PP/OD frequencies for a given I2C speed
 * @param[in] i2c_freq Desired I2C frequency
 * @param[out] pp_freq PP frequency
 * @param[out] od_freq OD frequency
 * @return 0 on success, -EINVAL if unsupported frequency
 */
static int _max_capi_i2c_get_frequencies(uint32_t i2c_freq, uint32_t *pp_freq,
		uint32_t *od_freq)
{
	if (!pp_freq || !od_freq)
		return -EINVAL;

	/**
	 * Values obtained through trial-and-error by sweeping through
	 * PP and OD frequencies and getting the I2C frequency.
	 * Lowest frequencies chosen for power efficiency.
	 * */
	switch (i2c_freq) {
	case MAX_CAPI_I2C_SPEED_STANDARD:
		*pp_freq = 2500000;
		*od_freq =  500000;
		break;
	case MAX_CAPI_I2C_SPEED_FAST:
		*pp_freq = 12500000;
		*od_freq =  1390000;
		break;
	case MAX_CAPI_I2C_SPEED_FAST_PLUS:
		/*
		 * The I2C clock is divided down from the push-pull clock. With a
		 * 2.5 MHz push-pull clock, SDA does not follow the address bits at
		 * 1 MHz and every address goes out as all ones.
		 */
		*pp_freq = 12500000;
		*od_freq =  2500000;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Configure I3C peripheral in I2C mode
 * @param i2c_priv The private handle
 * @param target_mode Target mode: true,
 * 		      Controller mode: false
 * @param addr_or_freq Address if target mode,
 * 		       Frequency if controller mode
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_configure_hardware(struct max_capi_i2c_priv *i2c_priv,
		bool target_mode,
		uint32_t addr_or_freq)
{
	struct max_capi_i2c_target_state *target = i2c_priv->target;
	uint8_t i2c_id = i2c_priv->identifier;
	mxc_i3c_config_t i3c_config;
	mxc_gpio_cfg_t i3c_pins;
	int ret;
	bool target_allocated = false;

	ret = MXC_I3C_Shutdown(MXC_I3C_GET_I3C(i2c_id));
	if (ret != E_NO_ERROR)
		return -EINVAL;

	if (target_mode) {
		i3c_config = (mxc_i3c_config_t) {
			.target_mode = true,
			.static_addr = addr_or_freq,
			.i2c_hz = 0, /* These values are ignored in target mode */
			.od_hz = 0,
			.pp_hz = 0,
		};

		if (!target) {
			target = capi_calloc(1, sizeof(*target));
			if (!target)
				return -ENOMEM;
			i2c_priv->target = target;
			target_allocated = true;
		}
	} else {
		i3c_config = (mxc_i3c_config_t) {
			.target_mode = false,
			.static_addr = 0, /* This value is ignored in controller mode */
			.i2c_hz = addr_or_freq,
		};
		ret = _max_capi_i2c_get_frequencies(addr_or_freq,
						    &i3c_config.pp_hz,
						    &i3c_config.od_hz);
		if (ret)
			return ret;

		i2c_priv->freq = addr_or_freq;
	}

	ret = MXC_I3C_Init(MXC_I3C_GET_I3C(i2c_id), &i3c_config);
	if (ret != E_NO_ERROR) {
		ret = -EIO;
		if (target_mode && target_allocated)
			goto free_target;

		return ret;
	}

	i3c_pins = gpio_cfg_i3c;
	i3c_pins.vssel = (mxc_gpio_vssel_t)i2c_priv->vssel;
	MXC_GPIO_Config(&i3c_pins);

	return 0;

free_target:
	if (i2c_priv->target) {
		capi_free(target);
		i2c_priv->target = NULL;
	}

	return ret;
}

/**
 * @brief Handle target mode interrupts
 *
 * IMPORTANT: The callback is invoked from interrupt context and must:
 * - Execute quickly without blocking
 * - For CAPI_I2C_STXREQ event: Set target->tx_buffer and target->tx_length
 *   before returning, or set tx_buffer to NULL for underrun
 * - For CAPI_I2C_SRXREQ event: Prepare to receive data
 * - Return immediately
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral
 */
static void _max_capi_i2c_target_isr(struct max_capi_i2c_priv *i2c_priv,
				     mxc_i3c_regs_t *i3c)
{
	struct max_capi_i2c_target_state *target = i2c_priv->target;
	uint32_t flags = MXC_I3C_Target_GetFlags(i3c);
	capi_i2c_callback callback = i2c_priv->callback;
	void *callback_arg = i2c_priv->callback_arg;

	if (!target)
		return;

	/* START */
	if (flags & MXC_F_I3C_TARG_INTFL_START) {
		target->rx_count = 0;
		target->tx_count = 0;

		MXC_I3C_ClearRXFIFO(i3c);
		MXC_I3C_ClearTXFIFO(i3c);

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_START);
	}

	/* Address match */
	if (flags & MXC_F_I3C_TARG_INTFL_ADDRMATCH) {
		uint32_t status = i3c->targ_status;

		if (status & MXC_F_I3C_TARG_STATUS_RX_SDR) {
			/* Controller wants to write to us */
			target->state = MAX_CAPI_I2C_TARGET_STATE_RX;

			if (callback)
				callback(CAPI_I2C_SRXREQ, callback_arg, 0);

		} else if (status & MXC_F_I3C_TARG_STATUS_TX_SDR) {
			/* Controller wants to read from us */
			target->state = MAX_CAPI_I2C_TARGET_STATE_TX;

			if (callback)
				callback(CAPI_I2C_STXREQ, callback_arg, 0);
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_ADDRMATCH);
	}

	/* RX ready - controller is writing data to us */
	if (flags & MXC_F_I3C_TARG_INTFL_RX_RDY) {
		if (target->state == MAX_CAPI_I2C_TARGET_STATE_RX) {
			while (i3c->targ_status & MXC_F_I3C_TARG_STATUS_RX_RDY) {
				if (target->rx_count < MAX_CAPI_I2C_TARGET_RX_BUFFER_SIZE) {
					uint8_t byte = (uint8_t)(i3c->targ_rxfifo8 & 0xFF);
					target->rx_buffer[target->rx_count++] = byte;
				} else {
					/* Buffer overflow - read and discard */
					(void)i3c->targ_rxfifo8;

					if (callback)
						callback(CAPI_I2C_RXOVER, callback_arg, 0);
				}
			}
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_RX_RDY);
	}

	/* TX not full - controller is reading from us */
	if (flags & MXC_F_I3C_TARG_INTFL_TX_NFULL) {
		if (target->state == MAX_CAPI_I2C_TARGET_STATE_TX) {
			if (target->tx_buffer == NULL) {
				/* TX underrun */
				MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_TX_NFULL);

				if (callback)
					callback(CAPI_I2C_TXUNDER, callback_arg, 0);
			} else {
				while ((i3c->targ_status & MXC_F_I3C_TARG_STATUS_TX_NFULL) &&
				       (target->tx_count < target->tx_length)) {
					uint8_t byte = target->tx_buffer[target->tx_count++];
					i3c->targ_txfifo8 = byte;
				}

				/* Check if we've sent all data */
				if (target->tx_count >= target->tx_length)
					MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_TX_NFULL);
			}
		}

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_TX_NFULL);
	}

	/* STOP */
	if (flags & MXC_F_I3C_TARG_INTFL_STOP) {
		int event_extra = 0;

		if (target->state == MAX_CAPI_I2C_TARGET_STATE_RX) {
			/* Application should handle rx_buffer and rx_count
			   in callback */
			event_extra = target->rx_count;
		}

		target->state = MAX_CAPI_I2C_TARGET_STATE_IDLE;
		target->tx_buffer = NULL;
		target->tx_length = 0;

		if (callback)
			callback(CAPI_I2C_XFR_DONE, callback_arg, event_extra);

		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_STOP);
	}

	/* Errors/warnings */
	if (flags & MXC_F_I3C_TARG_INTFL_ERRWARN) {
		uint32_t errwarn = i3c->targ_errwarn;

		/* Clear error */
		i3c->targ_errwarn = errwarn;
		MXC_I3C_Target_ClearFlags(i3c, MXC_F_I3C_TARG_INTFL_ERRWARN);

		if (callback)
			callback(CAPI_I2C_NAKD, callback_arg, errwarn);
	}
}

/**
 * @brief Reset async state to IDLE
 * @param i2c_priv I2C private data
 */
static void _max_capi_i2c_reset_async_state(struct max_capi_i2c_priv *i2c_priv)
{
	if (!i2c_priv->async)
		return;

	i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_IDLE;
	i2c_priv->async->target_addr = 0;
	i2c_priv->async->subaddr_buf = NULL;
	i2c_priv->async->subaddr_len = 0;
	i2c_priv->async->subaddr_idx = 0;
	i2c_priv->async->data_buf = NULL;
	i2c_priv->async->data_len = 0;
	i2c_priv->async->data_idx = 0;
	i2c_priv->async->rx_limit = 0;
	i2c_priv->async->send_stop = false;
	i2c_priv->async->result = 0;
	i2c_priv->async->is_transmit = false;
}

/**
 * @brief Write bytes from buffer to TX FIFO
 * @param i3c I3C peripheral registers
 * @param buf Source buffer
 * @param len Number of bytes to write
 * @param idx Pointer to current index
 * @param is_last_chunk True if this is the last data chunk
 * @return Number of bytes actually written
 */
static uint16_t _max_capi_i2c_write_tx_fifo(mxc_i3c_regs_t *i3c,
		const uint8_t *buf, uint16_t len,
		uint16_t *idx, bool is_last_chunk)
{
	uint16_t written = 0;

	while ((*idx < len) &&
	       (MXC_I3C_Controller_GetTXCount(i3c) < MAX_CAPI_I2C_FIFO_DEPTH)) {
		bool is_last = (is_last_chunk && (*idx == (len - 1)));
		int ret = MXC_I3C_WriteTXFIFO(i3c, &buf[*idx], 1, is_last, 0);
		if (ret <= 0)
			break;
		(*idx)++;
		written++;
	}

	return written;
}

/**
 * @brief Check a blocking transfer for a controller error
 *
 * CONT_ERRWARN.timeout is marked reserved in the user guide, and the MSDK
 * reports it as a timeout error. It is cleared here without failing the
 * transfer: a transfer that stops making progress is caught by the bus-speed
 * timeout in _max_capi_i2c_poll() instead. Holding SCL is not an error on
 * its own, as the controller holds it whenever its receive FIFO is full, and
 * a 1.5 ms hold on a full receive FIFO leaves this bit clear.
 *
 * @param i3c I3C peripheral registers
 * @return 0 if no error is pending, -EIO otherwise
 */
static int _max_capi_i2c_bus_error(mxc_i3c_regs_t *i3c)
{
	uint32_t errwarn;

	if (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_ERRWARN))
		return 0;

	errwarn = i3c->cont_errwarn;
	if (errwarn & MXC_F_I3C_CONT_ERRWARN_TO)
		i3c->cont_errwarn = MXC_F_I3C_CONT_ERRWARN_TO;

	return (errwarn & ~MXC_F_I3C_CONT_ERRWARN_TO) ? -EIO : 0;
}

/**
 * @brief Check a blocking transfer for a controller error or a stall
 * @param i3c I3C peripheral registers
 * @param timeout Poll iterations left, decremented on each call
 * @return 0 to keep polling, -EIO on a controller error, -ETIMEDOUT when
 *	   @p timeout runs out
 */
static int _max_capi_i2c_poll(mxc_i3c_regs_t *i3c, uint32_t *timeout)
{
	int ret;

	ret = _max_capi_i2c_bus_error(i3c);
	if (ret)
		return ret;

	if (!--(*timeout))
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Move received bytes from the RX FIFO into a buffer
 * @param i3c I3C peripheral registers
 * @param buf Destination buffer
 * @param idx Index of the next byte in @p buf, advanced by the bytes moved
 * @param limit Index to stop at; later bytes stay in the FIFO
 * @return Number of bytes moved
 */
static uint32_t _max_capi_i2c_drain_rx(mxc_i3c_regs_t *i3c, uint8_t *buf,
				       uint32_t *idx, uint32_t limit)
{
	uint32_t count = MXC_I3C_Controller_GetRXCount(i3c);
	uint32_t i;

	if (count > limit - *idx)
		count = limit - *idx;

	for (i = 0; i < count; i++)
		buf[(*idx)++] = (uint8_t)i3c->cont_rxfifo8;

	return count;
}

/**
 * @brief Get the count to load into CONT_CTRL1.term_rd with a read START
 * @param len Bytes to read
 * @return @p len if term_rd can end the read, otherwise 0, which lets the read
 *	   run until _max_capi_i2c_term_read() ends it
 */
static uint8_t _max_capi_i2c_read_count(uint32_t len)
{
	return (len <= MAX_CAPI_I2C_TERM_RD_MAX) ? (uint8_t)len : 0;
}

/**
 * @brief Get the bytes to receive before a read is ended
 *
 * A read longer than MAX_CAPI_I2C_TERM_RD_MAX bytes leaves its last
 * MAX_CAPI_I2C_FIFO_DEPTH + MAX_CAPI_I2C_TERM_TAIL bytes for
 * _max_capi_i2c_term_read() and _max_capi_i2c_finish_read().
 *
 * @param len Bytes to read
 * @return Bytes to receive before the read is ended
 */
static uint32_t _max_capi_i2c_rx_limit(uint32_t len)
{
	if (len <= MAX_CAPI_I2C_TERM_RD_MAX)
		return len;

	return len - MAX_CAPI_I2C_FIFO_DEPTH - MAX_CAPI_I2C_TERM_TAIL;
}

/**
 * @brief Make the controller NACK the next byte it receives
 * @param i3c I3C peripheral registers
 */
static void _max_capi_i2c_nack_next(mxc_i3c_regs_t *i3c)
{
	/* term_rd can only be changed with req = 0 */
	i3c->cont_ctrl1 = (i3c->cont_ctrl1 & ~(MXC_F_I3C_CONT_CTRL1_REQ |
					       MXC_F_I3C_CONT_CTRL1_TERM_RD)) |
			  MXC_S_I3C_CONT_CTRL1_REQ_NONE |
			  (1U << MXC_F_I3C_CONT_CTRL1_TERM_RD_POS);
}

/**
 * @brief Make the controller NACK the next byte of a read started with
 *	  term_rd = 0
 *
 * With the receive FIFO full the controller holds SCL, so once the FIFO holds
 * MAX_CAPI_I2C_FIFO_DEPTH bytes, the next byte on the bus is known. Writing
 * term_rd = 1 then makes the controller clock in exactly that byte and NACK
 * it.
 *
 * @param i3c I3C peripheral registers
 * @param budget Poll iterations allowed per byte
 * @return 0 on success, -ETIMEDOUT if the FIFO does not fill, -EIO on a bus
 *	   error
 */
static int _max_capi_i2c_term_read(mxc_i3c_regs_t *i3c, uint32_t budget)
{
	uint32_t timeout = budget * MAX_CAPI_I2C_FIFO_DEPTH;
	int ret;

	/* Wait until the FIFO is full and the controller holds SCL */
	while (MXC_I3C_Controller_GetRXCount(i3c) < MAX_CAPI_I2C_FIFO_DEPTH) {
		ret = _max_capi_i2c_poll(i3c, &timeout);
		if (ret)
			return ret;
	}

	_max_capi_i2c_nack_next(i3c);

	return 0;
}

/**
 * @brief End a read that failed while its message is still open
 *
 * A STOP requested in the middle of a message is dropped, and a read started
 * with term_rd = 0 does not end by itself. This makes the controller NACK the
 * next byte and discards what it receives until the message is done, so that
 * the STOP that follows goes out.
 *
 * @param i3c I3C peripheral registers
 * @param budget Poll iterations allowed per byte
 */
static void _max_capi_i2c_abort_read(mxc_i3c_regs_t *i3c, uint32_t budget)
{
	/* The byte on the bus, then the byte that is NACKed */
	uint32_t timeout = budget * 2;

	if (i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE)
		return;

	_max_capi_i2c_nack_next(i3c);

	/* A full FIFO holds SCL, so keep it empty */
	while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE) && --timeout) {
		if (MXC_I3C_Controller_GetRXCount(i3c))
			MXC_I3C_ClearRXFIFO(i3c);
	}
}

/**
 * @brief Receive the rest of a read after _max_capi_i2c_term_read()
 * @param i3c I3C peripheral registers
 * @param buf Destination buffer
 * @param idx Index of the next byte in @p buf, advanced by the bytes moved
 * @param len Bytes to read in total
 * @param budget Poll iterations allowed per byte
 * @return 0 on success, -ETIMEDOUT if the bus stops making progress,
 *	   -EOVERFLOW if the target sent more bytes than requested, -EIO on a
 *	   short read or another bus error
 */
static int _max_capi_i2c_finish_read(mxc_i3c_regs_t *i3c, uint8_t *buf,
				     uint32_t *idx, uint32_t len,
				     uint32_t budget)
{
	uint32_t timeout = budget;
	int ret;

	while (*idx < len) {
		uint32_t status = i3c->cont_status;

		if (_max_capi_i2c_drain_rx(i3c, buf, idx, len)) {
			timeout = budget;
			continue;
		}

		/* The read ended early */
		if (status & MXC_F_I3C_CONT_STATUS_DONE)
			return -EIO;

		ret = _max_capi_i2c_poll(i3c, &timeout);
		if (ret)
			return ret;
	}

	timeout = budget;
	while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE)) {
		ret = _max_capi_i2c_poll(i3c, &timeout);
		if (ret)
			return ret;
	}

	/* A byte past the requested length means the NACK came late */
	if (MXC_I3C_Controller_GetRXCount(i3c)) {
		MXC_I3C_ClearRXFIFO(i3c);
		return -EOVERFLOW;
	}

	return _max_capi_i2c_bus_error(i3c);
}

/**
 * @brief Busy-wait for a bus clear half period
 *
 * Counts core cycles rather than calling MXC_Delay(), which needs the SysTick
 * interrupt and so cannot run from the DMA completion callback.
 */
static void _max_capi_i2c_clear_wait(void)
{
	/* Each iteration takes at least one core clock */
	volatile uint32_t n = SystemCoreClock / 1000000U *
			      MAX_CAPI_I2C_CLEAR_HALF_US;

	while (n)
		n--;
}

/**
 * @brief Free a bus that a target holds by driving SDA low
 *
 * A target cut off in the middle of a byte, for example by a read that timed
 * out while SCL was held low, goes on driving SDA once it gets SCL back. The
 * controller cannot clock it out: it reports the bus idle, and the next
 * message reads what the target is still sending. As in the I2C-bus
 * specification (UM10204, bus clear), SCL is clocked with SDA released until
 * the target lets go, for at most a byte and its acknowledge, each pulse
 * ending with a STOP, as in the Linux generic I2C bus recovery. The pins are
 * GPIOs meanwhile, driven low or released to the pull-ups.
 *
 * @param i2c_priv I2C private data
 * @return 0 if the bus is free, -EBUSY if SDA or SCL stays low
 */
static int _max_capi_i2c_clear_bus(struct max_capi_i2c_priv *i2c_priv)
{
	const uint32_t scl = MAX_CAPI_I2C_SCL_PIN, sda = MAX_CAPI_I2C_SDA_PIN;
	mxc_gpio_regs_t *port = gpio_cfg_i3c.port;
	mxc_gpio_cfg_t pins = gpio_cfg_i3c;
	uint32_t i;
	int ret;

	pins.vssel = (mxc_gpio_vssel_t)i2c_priv->vssel;
	pins.func = MXC_GPIO_FUNC_IN;
	MXC_GPIO_Config(&pins);
	/* Driving a pin low is enabling its output */
	MXC_GPIO_OutClr(port, gpio_cfg_i3c.mask);

	/*
	 * SDA follows SCL half a period later, so that every pulse ends with a
	 * STOP once the target lets SDA go. SDA high with SCL high at the end of
	 * a pulse means the STOP went out. Stopping at the first SDA high before
	 * a pulse instead would miss a target in the middle of sending a 1.
	 */
	for (i = 0; i < MAX_CAPI_I2C_CLEAR_PULSES; i++) {
		port->outen_set = scl;
		_max_capi_i2c_clear_wait();
		port->outen_set = sda;
		_max_capi_i2c_clear_wait();
		port->outen_clr = scl;
		_max_capi_i2c_clear_wait();
		port->outen_clr = sda;
		_max_capi_i2c_clear_wait();

		if ((port->in & gpio_cfg_i3c.mask) == gpio_cfg_i3c.mask)
			break;
	}

	ret = ((port->in & gpio_cfg_i3c.mask) == gpio_cfg_i3c.mask) ? 0 : -EBUSY;

	pins.func = gpio_cfg_i3c.func;
	MXC_GPIO_Config(&pins);

	return ret;
}

/**
 * @brief Bring the controller and the bus back to the stopped state
 *
 * An open message is ended with a STOP, and a bus clear frees a target still
 * driving SDA. The controller takes such a target for one making an In-Band
 * Interrupt request, which no I2C target makes: it flags a target START and
 * leaves the stopped state, and can hold SDA itself. MXC_I3C_Recover() is not
 * used, as it has the controller process the request, which clocks the bus
 * while the target is still sending. Only a reset of the peripheral brings
 * the controller back instead, and it comes before the bus clear.
 *
 * @param i2c_priv I2C private data
 * @return 0 if the bus is free, negative error code otherwise
 */
static int _max_capi_i2c_recover(struct max_capi_i2c_priv *i2c_priv)
{
	mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);
	int ret;

	i2c_priv->recover_pending = false;

	if ((i3c->cont_status & MXC_F_I3C_CONT_STATUS_STATE) ==
	    MXC_S_I3C_CONT_STATUS_STATE_SDR_NORM)
		MXC_I3C_EmitI2CStop(i3c);

	/* Reset first: the reset sets up the pins again, which can clock SCL */
	if ((i3c->cont_status & MXC_F_I3C_CONT_STATUS_STATE) !=
	    MXC_S_I3C_CONT_STATUS_STATE_IDLE ||
	    (i3c->cont_status & MXC_F_I3C_CONT_STATUS_TARG_START)) {
		ret = _max_capi_i2c_configure_hardware(i2c_priv, false,
						       i2c_priv->freq);
		if (ret)
			return ret;
	}

	return _max_capi_i2c_clear_bus(i2c_priv);
}

/**
 * @brief Recover the bus before a new message if a target is still sending
 *
 * The signs are an earlier transfer that timed out partway through a message,
 * a target START flagged, the states the controller enters for an In-Band
 * Interrupt request, which no I2C target makes, or SDA low with SCL high in the
 * stopped state. The timeout is the only sign of a target cut off while it
 * sends a 1. While a message is open, the target holds SDA low after it
 * acknowledges a byte, so the pins alone do not count then.
 *
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral registers
 */
static void _max_capi_i2c_check_bus(struct max_capi_i2c_priv *i2c_priv,
				    mxc_i3c_regs_t *i3c)
{
	uint32_t status = i3c->cont_status;
	uint32_t state = status & MXC_F_I3C_CONT_STATUS_STATE;
	mxc_gpio_regs_t *port = gpio_cfg_i3c.port;

	if (i2c_priv->recover_pending ||
	    (status & MXC_F_I3C_CONT_STATUS_TARG_START) ||
	    state == MXC_S_I3C_CONT_STATUS_STATE_TARG_REQ ||
	    state == MXC_S_I3C_CONT_STATUS_STATE_IBI_ACKNACK ||
	    (state == MXC_S_I3C_CONT_STATUS_STATE_IDLE &&
	     (port->in & gpio_cfg_i3c.mask) == MAX_CAPI_I2C_SCL_PIN))
		_max_capi_i2c_recover(i2c_priv);
}

/**
 * @brief End a failed transfer with a STOP
 *
 * A STOP requested while a byte is on the bus is dropped, and after a write
 * times out the controller can still be sending. This discards the rest of
 * the write and requests the STOP again until the controller is idle, for at
 * most the bytes the transmit FIFO and the shift register can hold. A read
 * must be ended with _max_capi_i2c_abort_read() first.
 *
 * @param i3c I3C peripheral registers
 * @param budget Poll iterations allowed per byte
 */
static void _max_capi_i2c_stop_failed(mxc_i3c_regs_t *i3c, uint32_t budget)
{
	uint32_t tries, timeout;

	MXC_I3C_ClearTXFIFO(i3c);

	for (tries = 0; tries <= MAX_CAPI_I2C_FIFO_DEPTH; tries++) {
		i3c->cont_ctrl1 = MXC_S_I3C_CONT_CTRL1_REQ_EMIT_STOP |
				  (1U << MXC_F_I3C_CONT_CTRL1_TYPE_POS);

		for (timeout = budget; timeout; timeout--)
			if ((i3c->cont_status & MXC_F_I3C_CONT_STATUS_STATE) ==
			    MXC_S_I3C_CONT_STATUS_STATE_IDLE)
				return;
	}
}

/**
 * @brief Perform I2C transmit using FIFO (interrupt-driven)
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_transmit_fifo(mxc_i3c_regs_t *i3c,
				       struct max_capi_i2c_async_state *async)
{
	int ret;

	async->is_transmit = true;

	if (async->subaddr_buf)
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR;
	else
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
				async->target_addr, 0);
	if (ret == E_BUSY)
		return -EBUSY;
	if (ret != E_SUCCESS) {
		/* A NACKed address leaves the message open: end it */
		MXC_I3C_EmitI2CStop(i3c);
		return -EIO;
	}

	if (async->state == MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR) {
		_max_capi_i2c_write_tx_fifo(i3c, async->subaddr_buf,
					    async->subaddr_len,
					    &async->subaddr_idx,
					    false);
		if (async->subaddr_idx >= async->subaddr_len)
			async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;
	}

	if (async->state == MAX_CAPI_I2C_ASYNC_STATE_TX_DATA) {
		_max_capi_i2c_write_tx_fifo(i3c, async->data_buf,
					    async->data_len,
					    &async->data_idx,
					    true);
	}

	/*
	 * EmitStart cleared the flags before the message started. Clearing them
	 * again here could drop the DONE of a message that already ended.
	 */
	MXC_I3C_Controller_EnableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL |
				     MXC_F_I3C_CONT_INTEN_DONE |
				     MXC_F_I3C_CONT_INTEN_ERRWARN);

	return 0;
}

/**
 * @brief Perform I2C receive using FIFO (interrupt-driven)
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_receive_fifo(mxc_i3c_regs_t *i3c,
				      struct max_capi_i2c_async_state *async)
{
	int ret;
	uint32_t int_mask;

	async->is_transmit = false;

	if (async->subaddr_buf) {
		async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR;

		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
					async->target_addr, 0);
		if (ret == E_BUSY)
			return -EBUSY;
		if (ret != E_SUCCESS) {
			/* A NACKed address leaves the message open: end it */
			MXC_I3C_EmitI2CStop(i3c);
			return -EIO;
		}

		_max_capi_i2c_write_tx_fifo(i3c, async->subaddr_buf,
					    async->subaddr_len,
					    &async->subaddr_idx,
					    true);

		int_mask = MXC_F_I3C_CONT_INTEN_TX_NFULL |
			   MXC_F_I3C_CONT_INTEN_DONE |
			   MXC_F_I3C_CONT_INTEN_ERRWARN;
	} else {
		async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;

		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ,
					async->target_addr,
					_max_capi_i2c_read_count(async->data_len));
		if (ret == E_BUSY)
			return -EBUSY;
		if (ret != E_SUCCESS) {
			/* A NACKed address leaves the message open: end it */
			MXC_I3C_EmitI2CStop(i3c);
			return -EIO;
		}

		int_mask = MXC_F_I3C_CONT_INTEN_RX_RDY |
			   MXC_F_I3C_CONT_INTEN_DONE |
			   MXC_F_I3C_CONT_INTEN_ERRWARN;
	}

	/*
	 * EmitStart cleared the flags before the message started. Clearing them
	 * again here could drop the DONE of a message that already ended.
	 */
	MXC_I3C_Controller_EnableInt(i3c, int_mask);

	return 0;
}

/**
 * @brief Cleanup and deinit a DMA channel
 * @param channel The DMA channel
 */
void _max_capi_i2c_dma_cleanup_channel(struct capi_dma_chan **channel)
{
	if (channel && *channel) {
		capi_dma_xfer_abort(*channel);
		capi_dma_deinit_chan(*channel);
		*channel = NULL;
	}
}

/**
 * @brief DMA transfer completion callback
 * @param event The event indicating completion status
 * @param ctx The user data passed (struct max_capi_i2c_priv)
 */
static void _max_capi_i2c_dma_complete_callback(uint32_t event, void *ctx)
{
	struct max_capi_i2c_priv *i2c_priv = ctx;

	if (!i2c_priv || !i2c_priv->async)
		return;

	i2c_priv->callback_active = true;

	uint8_t i2c_id = i2c_priv->identifier;
	mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_id);
	capi_i2c_callback callback;
	void *callback_arg;
	uint32_t budget, timeout;
	int result = 0;

	callback = i2c_priv->callback;
	callback_arg = i2c_priv->callback_arg;

	i2c_priv->dma_completed = true;

	/*
	 * The DMA is done with memory, not with the bus: a write still has up to
	 * MAX_CAPI_I2C_FIFO_DEPTH bytes queued, and a read ends with the NACK of
	 * its last byte. A STOP requested before DONE, in the middle of the
	 * message, is dropped, so wait for DONE with timeouts that follow the bus
	 * speed. Each poll iteration takes at least one core clock.
	 */
	budget = (SystemCoreClock / i2c_priv->freq) * MAX_CAPI_I2C_IRQ_IDLE_BITS;

	if (i2c_priv->async->is_transmit && i2c_priv->async->dma_tx_end_pending) {
		timeout = budget;
		while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_TX_NFULL) &&
		       !result)
			result = _max_capi_i2c_poll(i3c, &timeout);

		/* The last byte goes to the END register to close the message */
		if (!result)
			i3c->cont_txfifo8e = i2c_priv->async->dma_tx_end_byte;
		i2c_priv->async->dma_tx_end_pending = false;
	}

	if (!i2c_priv->async->is_transmit &&
	    i2c_priv->async->rx_limit < i2c_priv->async->data_len) {
		uint32_t idx = i2c_priv->async->rx_limit;

		/*
		 * The DMA moved the first rx_limit bytes of a read too long for
		 * term_rd. The last bytes have to stay in the FIFO until term_rd
		 * is written, so take the DMA off the FIFO and end the read here.
		 */
		i3c->cont_dmactrl = 0;

		result = _max_capi_i2c_term_read(i3c, budget);
		if (!result)
			result = _max_capi_i2c_finish_read(i3c,
							   i2c_priv->async->data_buf,
							   &idx,
							   i2c_priv->async->data_len,
							   budget);
	}

	timeout = budget * (MAX_CAPI_I2C_FIFO_DEPTH + 1);
	while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE) && !result)
		result = _max_capi_i2c_poll(i3c, &timeout);

	if (!result)
		result = _max_capi_i2c_bus_error(i3c);

	if (result && !i2c_priv->async->is_transmit)
		_max_capi_i2c_abort_read(i3c, budget);

	/* Bus errors clear at the STOP, so they are read before it */
	if (result)
		_max_capi_i2c_stop_failed(i3c, budget);
	else if (i2c_priv->async->send_stop)
		MXC_I3C_EmitI2CStop(i3c);
	if (result == -ETIMEDOUT)
		i2c_priv->recover_pending = true;

	/*
	 * Disable the controller RX/TX DMA request. Leaving it enabled lets a
	 * stale request carry into the next transfer: when the following read
	 * arms its channel, that pending request fires one phantom transfer off
	 * the (empty) RX FIFO, shifting the data by one byte and stranding the
	 * last byte in the FIFO.
	 */
	i3c->cont_dmactrl = 0;

	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_rx);
	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_tx);

	i2c_priv->dma_completed = false;
	i2c_priv->async_transfer_in_progress = false;

	if (i2c_priv->async->dma_allocated_buffer) {
		capi_free(i2c_priv->async->dma_allocated_buffer);
		i2c_priv->async->dma_allocated_buffer = NULL;
	}

	_max_capi_i2c_reset_async_state(i2c_priv);

	if (callback)
		callback(CAPI_I2C_XFR_DONE, callback_arg, result);

	i2c_priv->callback_active = false;
}

/**
 * @brief Perform I2C transmit using DMA
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_transmit_dma(struct max_capi_i2c_priv *i2c_priv,
				      mxc_i3c_regs_t *i3c,
				      struct max_capi_i2c_async_state *async)
{
	int ret;
	struct capi_dma_handle *dma_handle = i2c_priv->dma_handle;
	uint8_t *tx_buffer;
	uint32_t tx_len;
	struct max_capi_dma_xfer_extra dma_tx_extra;
	struct capi_dma_transfer dma_tx_xfer;

	if (i2c_priv->async_transfer_in_progress)
		return -EBUSY;

	i2c_priv->async_transfer_in_progress = true;

	if (async->subaddr_buf) {
		tx_len = async->subaddr_len + async->data_len;
		tx_buffer = capi_malloc(tx_len);
		if (!tx_buffer) {
			i2c_priv->async_transfer_in_progress = false;
			return -ENOMEM;
		}
		memcpy(tx_buffer, async->subaddr_buf, async->subaddr_len);
		memcpy(tx_buffer + async->subaddr_len, async->data_buf, async->data_len);
		i2c_priv->async->dma_allocated_buffer = tx_buffer;
	} else {
		tx_buffer = async->data_buf;
		tx_len = async->data_len;
		i2c_priv->async->dma_allocated_buffer = NULL;
	}

	async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;
	async->is_transmit = true;

	/*
	 * An I3C write terminates only when its last byte carries the END
	 * marker, which the hardware sets when the CPU writes a byte to the END
	 * FIFO register (cont_txfifo8e) rather than the ordinary one
	 * (cont_txfifo8o). A DMA write to cont_txfifo8e does not set it: the byte
	 * goes out, but DONE never follows. The last byte is therefore always
	 * written by _max_capi_i2c_dma_complete_callback().
	 */
	async->dma_tx_end_byte = tx_buffer[tx_len - 1U];
	async->dma_tx_end_pending = true;

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
				async->target_addr, 0);
	if (ret != E_SUCCESS) {
		/* A NACKed address leaves the message open: end it */
		MXC_I3C_EmitI2CStop(i3c);
		if (tx_buffer != async->data_buf) {
			capi_free(tx_buffer);
			i2c_priv->async->dma_allocated_buffer = NULL;
		}
		i2c_priv->async_transfer_in_progress = false;
		return -EIO;
	}

	ret = capi_dma_init_chan(dma_handle, &i2c_priv->dma_channel_tx, 0);
	if (ret)
		goto error_cleanup;

	dma_tx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_I3C_TX_CONTROLLER,
	};

	if (tx_len > 1U) {
		i3c->cont_dmactrl = MXC_S_I3C_CONT_DMACTRL_TX_EN_EN;

		dma_tx_xfer = (struct capi_dma_transfer) {
			.src = (capi_dma_glbl_addr_t)tx_buffer,
			.dst = (capi_dma_glbl_addr_t)&i3c->cont_txfifo8o,
			.src_inc = CAPI_DMA_BYTE_INCREMENT,
			.dst_inc = CAPI_DMA_NO_INCREMENT,
			.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
			.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
			.length = tx_len - 1U,
			.extra = &dma_tx_extra,
			.xfer_type = CAPI_DMA_MEM_TO_DEV,
			.user_data = i2c_priv,
		};
	} else {
		/*
		 * A one-byte write leaves the DMA nothing to move to the FIFO.
		 * It copies the byte onto dma_tx_end_byte instead, only to raise
		 * the completion callback, which writes the byte.
		 */
		dma_tx_xfer = (struct capi_dma_transfer) {
			.src = (capi_dma_glbl_addr_t)tx_buffer,
			.dst = (capi_dma_glbl_addr_t)&async->dma_tx_end_byte,
			.src_inc = CAPI_DMA_BYTE_INCREMENT,
			.dst_inc = CAPI_DMA_BYTE_INCREMENT,
			.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
			.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
			.length = 1U,
			.extra = &dma_tx_extra,
			.xfer_type = CAPI_DMA_MEM_TO_MEM,
			.user_data = i2c_priv,
		};
	}

	ret = capi_dma_config_xfer(i2c_priv->dma_channel_tx, &dma_tx_xfer);
	if (ret)
		goto error_deinit_tx;

	ret = capi_dma_register_complete_callback(i2c_priv->dma_channel_tx,
			_max_capi_i2c_dma_complete_callback,
			i2c_priv);
	if (ret)
		goto error_deinit_tx;

	i2c_priv->dma_completed = false;

	ret = capi_dma_xfer_start(i2c_priv->dma_channel_tx);
	if (ret)
		goto error_deinit_tx;

	return 0;

error_deinit_tx:
	/* Clears the pointer too: the next init must not reuse the freed channel */
	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_tx);
error_cleanup:
	/* The message is already open: end it, as the receive path does */
	i3c->cont_dmactrl = 0;
	MXC_I3C_EmitI2CStop(i3c);
	if (tx_buffer != async->data_buf) {
		capi_free(tx_buffer);
		i2c_priv->async->dma_allocated_buffer = NULL;
	}
	i2c_priv->async_transfer_in_progress = false;

	return ret;
}

/**
 * @brief Perform I2C receive using DMA
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral registers
 * @param async Async state
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_i2c_receive_dma(struct max_capi_i2c_priv *i2c_priv,
				     mxc_i3c_regs_t *i3c,
				     struct max_capi_i2c_async_state *async)
{
	int ret;
	struct capi_dma_handle *dma_handle = i2c_priv->dma_handle;
	struct max_capi_dma_xfer_extra dma_rx_extra;
	struct capi_dma_transfer dma_rx_xfer;
	uint32_t budget, timeout;

	if (i2c_priv->async_transfer_in_progress)
		return -EBUSY;

	i2c_priv->async_transfer_in_progress = true;

	async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;
	async->is_transmit = false;

	if (async->subaddr_buf) {
		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
					async->target_addr, 0);
		if (ret != E_SUCCESS) {
			/* A NACKed address leaves the message open: end it */
			MXC_I3C_EmitI2CStop(i3c);
			i2c_priv->async_transfer_in_progress = false;
			return -EIO;
		}

		/*
		 * As in max_capi_i2c_receive(): a sub-address longer than the
		 * transmit FIFO goes out as the FIFO drains, so wait for room with a
		 * timeout that follows the bus speed. Each poll iteration takes at
		 * least one core clock.
		 */
		budget = (SystemCoreClock / i2c_priv->freq) * MAX_CAPI_I2C_IDLE_BITS;
		timeout = budget * async->subaddr_len;
		if (MXC_I3C_WriteTXFIFO(i3c, async->subaddr_buf,
					async->subaddr_len, true,
					timeout) != async->subaddr_len) {
			_max_capi_i2c_stop_failed(i3c, budget);
			i2c_priv->async_transfer_in_progress = false;
			i2c_priv->recover_pending = true;
			return -ETIMEDOUT;
		}

		/*
		 * Write completion is signalled in cont_status (CONT_STATUS_DONE),
		 * which is what the MSDK's own Controller_Transaction polls. The
		 * cont_intfl DONE flag (MXC_I3C_Controller_GetFlags) is not set on
		 * this path since the DONE interrupt is never armed here, so polling
		 * it hangs until timeout.
		 */
		while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE)) {
			ret = _max_capi_i2c_poll(i3c, &timeout);
			if (ret) {
				_max_capi_i2c_stop_failed(i3c, budget);
				i2c_priv->async_transfer_in_progress = false;
				i2c_priv->recover_pending = ret == -ETIMEDOUT;
				return ret;
			}
		}
		MXC_I3C_Controller_ClearFlags(i3c, MXC_F_I3C_CONT_STATUS_DONE);
	}

	/* Arm the RX DMA channel BEFORE triggering the read. */
	MXC_I3C_ClearRXFIFO(i3c);
	i3c->cont_dmactrl = MXC_S_I3C_CONT_DMACTRL_RX_EN_EN;

	ret = capi_dma_init_chan(dma_handle, &i2c_priv->dma_channel_rx, 0);
	if (ret)
		goto error_cleanup;

	dma_rx_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_I3C_RX_CONTROLLER,
	};

	dma_rx_xfer = (struct capi_dma_transfer) {
		.src = (capi_dma_glbl_addr_t)&i3c->cont_rxfifo8,
		.dst = (capi_dma_glbl_addr_t)async->data_buf,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		/* A long read is finished in _max_capi_i2c_dma_complete_callback() */
		.length = async->rx_limit,
		.extra = &dma_rx_extra,
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.user_data = i2c_priv,
	};

	ret = capi_dma_config_xfer(i2c_priv->dma_channel_rx, &dma_rx_xfer);
	if (ret)
		goto error_deinit_rx;

	ret = capi_dma_register_complete_callback(i2c_priv->dma_channel_rx,
			_max_capi_i2c_dma_complete_callback,
			i2c_priv);
	if (ret)
		goto error_deinit_rx;

	i2c_priv->dma_completed = false;

	ret = capi_dma_xfer_start(i2c_priv->dma_channel_rx);
	if (ret)
		goto error_deinit_rx;

	/* DMA is armed and waiting; now trigger the read. */
	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ,
				async->target_addr,
				_max_capi_i2c_read_count(async->data_len));
	if (ret != E_SUCCESS) {
		/* A NACKed address leaves the message open: end it */
		MXC_I3C_EmitI2CStop(i3c);
		ret = -EIO;
		goto error_deinit_rx;
	}

	return 0;

error_deinit_rx:
	/* Clears the pointer too: the next init must not reuse the freed channel */
	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_rx);
error_cleanup:
	i3c->cont_dmactrl = 0;
	i2c_priv->async_transfer_in_progress = false;

	return ret;
}

/**
 * @brief Handle controller mode interrupts
 * @param i2c_priv I2C private data
 * @param i3c I3C peripheral
 */
static void _max_capi_i2c_controller_isr(struct max_capi_i2c_priv *i2c_priv,
		mxc_i3c_regs_t *i3c)
{
	uint32_t flags, budget, idx;
	int ret, result;
	bool operation_complete;
	capi_i2c_callback callback;
	void *callback_arg;

	if (!i2c_priv->async ||
	    i2c_priv->async->state == MAX_CAPI_I2C_ASYNC_STATE_IDLE) {
		/* Disable and clear interrupts so it cannot re-fire. */
		MXC_I3C_Controller_DisableInt(i3c, 0xFFFFFFFF);
		MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);
		return;
	}

	operation_complete = false;
	flags = MXC_I3C_Controller_GetFlags(i3c);
	/* Each poll iteration takes at least one core clock */
	budget = (SystemCoreClock / i2c_priv->freq) * MAX_CAPI_I2C_IRQ_IDLE_BITS;

	if (flags & MXC_F_I3C_CONT_INTFL_ERRWARN) {
		uint32_t errwarn = i3c->cont_errwarn;
		i3c->cont_errwarn = errwarn;

		/* As in _max_capi_i2c_bus_error(), a timeout is not an error */
		if (errwarn & ~MXC_F_I3C_CONT_ERRWARN_TO) {
			if (i2c_priv->async->state == MAX_CAPI_I2C_ASYNC_STATE_RX_DATA ||
			    i2c_priv->async->state == MAX_CAPI_I2C_ASYNC_STATE_RX_TERM)
				_max_capi_i2c_abort_read(i3c, budget);

			i2c_priv->async->result = -EIO;
			i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;

			/* Release the bus even if the caller asked to keep it */
			MXC_I3C_EmitI2CStop(i3c);

			operation_complete = true;
			goto invoke_callback;
		}
	}

	switch (i2c_priv->async->state) {
	case MAX_CAPI_I2C_ASYNC_STATE_TX_SUBADDR:
		if (flags & MXC_F_I3C_CONT_INTFL_TX_NFULL) {
			/* Before a read, the last sub-address byte ends the write */
			_max_capi_i2c_write_tx_fifo(i3c,
						    i2c_priv->async->subaddr_buf,
						    i2c_priv->async->subaddr_len,
						    &i2c_priv->async->subaddr_idx,
						    !i2c_priv->async->is_transmit);

			if (i2c_priv->async->subaddr_idx >= i2c_priv->async->subaddr_len) {
				if (i2c_priv->async->is_transmit) {
					i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_TX_DATA;
				} else {
					/*
					 * TX_NFULL only means the sub-address is queued.
					 * A repeated START requested now, while it is
					 * still on the bus, breaks the read, so wait for
					 * the write to finish (DONE).
					 */
					MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL);
					i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_WAIT_SUBADDR_DONE;
				}
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_WAIT_SUBADDR_DONE:
		if (flags & MXC_F_I3C_CONT_INTFL_DONE) {
			ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ,
						i2c_priv->async->target_addr,
						_max_capi_i2c_read_count(i2c_priv->async->data_len));
			if (ret != E_SUCCESS) {
				i2c_priv->async->result = -EIO;
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;
				MXC_I3C_EmitI2CStop(i3c);
				operation_complete = true;
				break;
			}

			MXC_I3C_Controller_EnableInt(i3c, MXC_F_I3C_CONT_INTEN_RX_RDY);
			i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_TX_DATA:
		if (flags & MXC_F_I3C_CONT_INTFL_TX_NFULL) {
			_max_capi_i2c_write_tx_fifo(i3c,
						    i2c_priv->async->data_buf,
						    i2c_priv->async->data_len,
						    &i2c_priv->async->data_idx,
						    true);

			if (i2c_priv->async->data_idx >= i2c_priv->async->data_len) {
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE;
				MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_TX_NFULL);
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_RX_DATA:
		if (flags & (MXC_F_I3C_CONT_INTFL_RX_RDY | MXC_F_I3C_CONT_INTFL_DONE)) {
			/*
			 * Take all that is there: if DONE is set, the last bytes
			 * are already in the FIFO.
			 */
			idx = i2c_priv->async->data_idx;
			_max_capi_i2c_drain_rx(i3c, i2c_priv->async->data_buf,
					       &idx, i2c_priv->async->rx_limit);
			i2c_priv->async->data_idx = idx;

			if (i2c_priv->async->data_idx >= i2c_priv->async->data_len) {
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE;
				MXC_I3C_Controller_DisableInt(i3c, MXC_F_I3C_CONT_INTEN_RX_RDY);
			} else if (i2c_priv->async->data_idx >= i2c_priv->async->rx_limit) {
				/*
				 * A read too long for term_rd: the last bytes stay in
				 * the FIFO until it is full. With the threshold at 3/4,
				 * RX_RDY fires again with at most two bytes to go.
				 */
				MXC_I3C_SetRXTXThreshold(i3c, MXC_I3C_RX_TH_3_4_FULL,
							 MXC_I3C_TX_TH_HALF_FULL);
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_TERM;
			} else if (flags & MXC_F_I3C_CONT_INTFL_DONE) {
				/* The read ended early */
				i2c_priv->async->result = -EIO;
				i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;
				MXC_I3C_EmitI2CStop(i3c);
				operation_complete = true;
			}
		}
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_RX_TERM:
		if (flags & MXC_F_I3C_CONT_INTFL_DONE) {
			/* The read ended early */
			ret = -EIO;
		} else if (flags & MXC_F_I3C_CONT_INTFL_RX_RDY) {
			ret = _max_capi_i2c_term_read(i3c, budget);
		} else {
			break;
		}

		MXC_I3C_SetRXTXThreshold(i3c, MXC_I3C_RX_TH_NOT_EMPTY,
					 MXC_I3C_TX_TH_HALF_FULL);

		if (ret) {
			i2c_priv->async->result = ret;
			i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_ERROR;
			_max_capi_i2c_abort_read(i3c, budget);
			MXC_I3C_EmitI2CStop(i3c);
			if (ret == -ETIMEDOUT)
				i2c_priv->recover_pending = true;
			operation_complete = true;
			break;
		}

		/* RX_RDY stays set on the full FIFO, so the ISR runs again */
		i2c_priv->async->rx_limit = i2c_priv->async->data_len;
		i2c_priv->async->state = MAX_CAPI_I2C_ASYNC_STATE_RX_DATA;
		break;

	case MAX_CAPI_I2C_ASYNC_STATE_WAIT_DONE:
		if (flags & MXC_F_I3C_CONT_INTFL_DONE) {
			operation_complete = true;

			/* A byte past the requested length means the NACK came late */
			if (!i2c_priv->async->is_transmit &&
			    MXC_I3C_Controller_GetRXCount(i3c)) {
				MXC_I3C_ClearRXFIFO(i3c);
				i2c_priv->async->result = -EOVERFLOW;
				MXC_I3C_EmitI2CStop(i3c);
				break;
			}

			i2c_priv->async->result = 0;

			if (i2c_priv->async->send_stop)
				MXC_I3C_EmitI2CStop(i3c);
		}
		break;

	default:
		break;
	}

invoke_callback:
	if (operation_complete) {
		MXC_I3C_Controller_DisableInt(i3c, 0xFFFFFFFF);
		MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);

		result = i2c_priv->async->result;
		callback = i2c_priv->callback;
		callback_arg = i2c_priv->callback_arg;

		_max_capi_i2c_reset_async_state(i2c_priv);

		if (callback) {
			callback(CAPI_I2C_XFR_DONE, callback_arg, result);
		}
	}
}

/**
 * @brief Setup before an async transmit/receive function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
int _max_capi_i2c_setup_async(struct capi_i2c_device *device,
			      struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	/* The async state tracks the data length in a uint16_t. */
	if (!transfer->buf || transfer->len == 0 || transfer->len > UINT16_MAX)
		return -EINVAL;

	i2c_priv = device->controller->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	/* Allocate async state on first use */
	if (!i2c_priv->async) {
		i2c_priv->async = capi_calloc(1, sizeof(*i2c_priv->async));
		if (!i2c_priv->async)
			return -ENOMEM;
	}

	if (i2c_priv->target && i2c_priv->target->enabled)
		return -ENOTSUP;

	if (i2c_priv->async->state != MAX_CAPI_I2C_ASYNC_STATE_IDLE)
		return -EBUSY;

	i2c_priv->async->target_addr = transfer->target_addr ?
				       transfer->target_addr : device->address;
	i2c_priv->async->send_stop = !transfer->no_stop;
	i2c_priv->async->result = 0;

	if (transfer->sub_address && transfer->sub_address_len > 0) {
		i2c_priv->async->subaddr_buf = transfer->sub_address;
		i2c_priv->async->subaddr_len = transfer->sub_address_len;
		i2c_priv->async->subaddr_idx = 0;
	} else {
		i2c_priv->async->subaddr_buf = NULL;
		i2c_priv->async->subaddr_len = 0;
	}

	i2c_priv->async->data_buf = transfer->buf;
	i2c_priv->async->data_len = transfer->len;
	i2c_priv->async->data_idx = 0;
	i2c_priv->async->rx_limit = _max_capi_i2c_rx_limit(transfer->len);

	/* Before the FIFOs are cleared: a recovery can leave bytes in them */
	_max_capi_i2c_check_bus(i2c_priv, i3c);
	MXC_I3C_ClearRXFIFO(i3c);
	MXC_I3C_ClearTXFIFO(i3c);

	ret = MXC_I3C_SetRXTXThreshold(i3c, MXC_I3C_RX_TH_NOT_EMPTY,
				       MXC_I3C_TX_TH_HALF_FULL);
	if (ret != E_SUCCESS) {
		_max_capi_i2c_reset_async_state(i2c_priv);
		return -EIO;
	}

	return 0;
}

/** I2C functions implementation **********************************************/

/**
 * @brief Initialize the I3C peripheral in I2C mode
 * @param handle The I2C handle
 * @param config The I2C initialization config
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_init(struct capi_i2c_controller_handle **handle,
			     const struct capi_i2c_config *config)
{
	int ret;
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *i2c_priv;
	enum max_capi_gpio_vssel vssel;
	bool use_irq;
	uint8_t i2c_id;

	if (!handle || !config)
		return -EINVAL;

	if (config->identifier >= MXC_CFG_I3C_INSTANCES)
		return -EINVAL;

	if (i2c[config->identifier] != NULL) {
		*handle = i2c[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		i2c_handle = capi_calloc(1, sizeof(*i2c_handle));
		if (!i2c_handle)
			return -ENOMEM;

		i2c_priv = capi_calloc(1, sizeof(*i2c_priv));
		if (!i2c_priv) {
			capi_free(i2c_handle);
			return -ENOMEM;
		}

		i2c_handle->priv = i2c_priv;
		i2c_handle->init_allocated = true;
	} else {
		i2c_handle = *handle;

		if (!i2c_handle->priv)
			return -EINVAL;

		i2c_priv = i2c_handle->priv;

		i2c_handle->init_allocated = false;
	}

	i2c_handle->ops = config->ops;

	if (config->extra) {
		struct max_capi_i2c_extra *extra = config->extra;

		vssel = extra->vssel;
		use_irq = extra->use_irq;
	} else {
		vssel = MAX_CAPI_GPIO_VSSEL_VDDIO;
		use_irq = false;
	}

	i2c_priv->identifier = config->identifier;
	i2c_priv->vssel = vssel;
	i2c_priv->use_irq = false;
	i2c_priv->dma_handle = NULL;
	i2c_priv->callback = NULL;
	i2c_priv->callback_arg = NULL;
	i2c_priv->callback_active = false;
	i2c_priv->dma_completed = false;
	i2c_priv->async_transfer_in_progress = false;

	i2c_id = i2c_priv->identifier;

	if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_STANDARD) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_STANDARD;
	} else if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_FAST) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_FAST;
	} else if (config->clk_freq_hz <= MAX_CAPI_I2C_SPEED_FAST_PLUS) {
		i2c_priv->freq = MAX_CAPI_I2C_SPEED_FAST_PLUS;
	} else {
		ret = -EINVAL;
		goto free_handle;
	}

	ret = _max_capi_i2c_configure_hardware(i2c_priv, !config->initiator,
					       config->initiator ? i2c_priv->freq : config->address);
	if (ret)
		goto shutdown_i2c;

	if (config->dma_handle)
		i2c_priv->dma_handle = config->dma_handle;

	if (use_irq) {
		IRQn_Type i2c_irq = MXC_I3C_GET_IRQ(i2c_id);
		ret = capi_irq_connect(i2c_irq, max_capi_i2c_isr, i2c_handle);
		if (ret)
			goto cleanup_channels;

		ret = capi_irq_enable(i2c_irq);
		if (ret)
			goto cleanup_channels;

		i2c_priv->use_irq = true;
	}

	i2c[config->identifier] = i2c_handle;
	*handle = i2c_handle;
	i2c_priv->i2c_handle = i2c_handle;

	return 0;

cleanup_channels:
	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_tx);
	_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_rx);
shutdown_i2c:
	MXC_I3C_Shutdown(MXC_I3C_GET_I3C(i2c_id));
	if (i2c_priv->target) {
		capi_free(i2c_priv->target);
		i2c_priv->target = NULL;
	}
free_handle:
	if (i2c_handle->init_allocated) {
		capi_free(i2c_priv);
		capi_free(i2c_handle);
	}

	i2c[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the I3C peripheral
 * @param handle The I2C handle
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_deinit(struct capi_i2c_controller_handle *handle)
{
	int ret;
	struct max_capi_i2c_priv *i2c_priv;
	uint8_t i2c_id;
	mxc_i3c_regs_t *i3c;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	MXC_I3C_Controller_DisableInt(i3c, 0xFFFFFFFF);
	MXC_I3C_Controller_ClearFlags(i3c, 0xFFFFFFFF);
	MXC_I3C_Target_DisableInt(i3c, 0xFFFFFFFF);
	MXC_I3C_Target_ClearFlags(i3c, 0xFFFFFFFF);

	if (i2c_priv->async) {
		if (i2c_priv->async->state != MAX_CAPI_I2C_ASYNC_STATE_IDLE) {
			if (i2c_priv->dma_channel_rx)
				capi_dma_xfer_abort(i2c_priv->dma_channel_rx);
			if (i2c_priv->dma_channel_tx)
				capi_dma_xfer_abort(i2c_priv->dma_channel_tx);

			uint32_t timeout = MAX_CAPI_I2C_DMA_CALLBACK_CLEANUP_TIMEOUT;
			while (i2c_priv->callback_active && --timeout);

			if (!timeout)
				i2c_priv->callback_active = false;

			MXC_I3C_EmitI2CStop(i3c);
		}

		capi_free(i2c_priv->async);
		i2c_priv->async = NULL;
	}

	if (i2c_priv->dma_handle) {
		_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_rx);
		_max_capi_i2c_dma_cleanup_channel(&i2c_priv->dma_channel_tx);
		i2c_priv->dma_completed = false;
		i2c_priv->async_transfer_in_progress = false;
	}

	if (i2c_priv->use_irq)
		capi_irq_disable(MXC_I3C_GET_IRQ(i2c_id));

	ret = MXC_I3C_Shutdown(i3c);

	if (i2c_priv->target) {
		capi_free(i2c_priv->target);
		i2c_priv->target = NULL;
	}

	if (handle->init_allocated) {
		capi_free(i2c_priv);
		capi_free(handle);
	}

	i2c[i2c_id] = NULL;

	if (ret != E_NO_ERROR)
		return (ret == E_TIME_OUT ? -ETIMEDOUT : -EIO);

	return 0;
}

/**
 * @brief Transmit function
 *
 * In controller mode the bytes are written as one message. The MSDK write is
 * not used: it polls a fixed 1000 times for the message to finish before it
 * requests the STOP, which at 100 kHz can end while bytes are still queued in
 * the transmit FIFO, and a STOP requested in the middle of a message is
 * dropped, leaving the message open. This waits for the message to end, with
 * a timeout that follows the bus speed, before the STOP.
 *
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, -ETIMEDOUT if the bus stops making progress, -EIO on
 *	   a NACK or another bus error, other negative error codes otherwise
 */
static int max_capi_i2c_transmit(struct capi_i2c_device *device,
				 struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	uint32_t total, budget, timeout, i;
	mxc_i3c_regs_t *i3c;
	uint16_t addr;
	uint8_t byte;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer
	    || !transfer->buf)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - set TX data*/
		_max_capi_i2c_target_set_tx_data(i2c_priv, transfer->buf, transfer->len);

		return 0;
	}

	if (transfer->sub_address_len > 0 && !transfer->sub_address)
		return -EINVAL;

	if (!transfer->len && !transfer->sub_address_len)
		return -EINVAL;

	if (transfer->len > UINT32_MAX - transfer->sub_address_len)
		return -EINVAL;

	i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);
	addr = transfer->target_addr ? transfer->target_addr : device->address;
	total = transfer->sub_address_len + transfer->len;
	/* Each poll iteration takes at least one core clock */
	budget = (SystemCoreClock / i2c_priv->freq) * MAX_CAPI_I2C_IDLE_BITS;

	/* Before the FIFOs are cleared: a recovery can leave bytes in them */
	_max_capi_i2c_check_bus(i2c_priv, i3c);
	MXC_I3C_ClearTXFIFO(i3c);

	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE, addr, 0);
	if (ret) {
		ret = (ret == E_TIME_OUT) ? -ETIMEDOUT : -EIO;
		goto stop;
	}

	for (i = 0; i < total; i++) {
		byte = (i < transfer->sub_address_len) ? transfer->sub_address[i] :
		       transfer->buf[i - transfer->sub_address_len];

		timeout = budget;
		while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_TX_NFULL)) {
			ret = _max_capi_i2c_poll(i3c, &timeout);
			if (ret)
				goto stop;
		}

		/* The last byte goes to the END register to close the message */
		if (i == total - 1)
			i3c->cont_txfifo8e = byte;
		else
			i3c->cont_txfifo8o = byte;
	}

	/* Let the queued bytes go out before the STOP */
	timeout = budget * (MAX_CAPI_I2C_FIFO_DEPTH + 1);
	while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE)) {
		ret = _max_capi_i2c_poll(i3c, &timeout);
		if (ret)
			goto stop;
	}

	ret = _max_capi_i2c_bus_error(i3c);

stop:
	/* Bus errors clear at the STOP, so they are read before it */
	if (ret)
		_max_capi_i2c_stop_failed(i3c, budget);
	else if (!transfer->no_stop)
		MXC_I3C_EmitI2CStop(i3c);

	if (ret == -ETIMEDOUT)
		i2c_priv->recover_pending = true;

	return ret;
}

/**
 * @brief Receive function
 *
 * In controller mode, a read of up to MAX_CAPI_I2C_TERM_RD_MAX bytes loads its
 * length into CONT_CTRL1.term_rd, which sets the byte the controller NACKs.
 * term_rd is 8 bits wide and can only be loaded with the START, so a longer
 * read cannot end that way. The MSDK splits such reads into messages joined by
 * repeated STARTs, which a target streaming one block (an SMBus block read)
 * does not accept. MXC_I3C_Controller_Transaction() is not used for short
 * reads either: it gives the sub-address write a fixed number of polls rather
 * than a time, so at 100 kHz a sub-address longer than a few bytes is still on
 * the bus when it requests the repeated START, and the read returns garbage.
 *
 * A longer read starts with term_rd = 0, which lets it run, and is ended by
 * writing term_rd = 1, which makes the controller NACK the next byte it
 * receives. The last bytes are left in the receive FIFO so that "the next
 * byte" is known: with the FIFO full the controller holds SCL, so once the
 * FIFO holds MAX_CAPI_I2C_FIFO_DEPTH bytes and the buffer holds the rest,
 * exactly len - MAX_CAPI_I2C_TERM_TAIL bytes have been received.
 *
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, -ETIMEDOUT if the bus stops making progress,
 *	   -EOVERFLOW if the target sent more bytes than requested, -EIO on a
 *	   NACK, a short read or another bus error, other negative error codes
 *	   otherwise
 */
static int max_capi_i2c_receive(struct capi_i2c_device *device,
				struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;
	uint32_t hold, budget, timeout;
	mxc_i3c_regs_t *i3c;
	uint32_t idx = 0;
	bool reading = false;
	uint16_t addr;
	int ret;

	if (!device || !device->controller || !device->controller->priv || !transfer
	    || !transfer->buf)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - get RX data */
		uint8_t *rx_data;
		uint16_t rx_len;

		_max_capi_i2c_target_get_rx_data(i2c_priv, &rx_data, &rx_len);

		if (rx_len > transfer->len)
			return -EOVERFLOW;

		memcpy(transfer->buf, rx_data, rx_len);
		transfer->len = rx_len;

		return 0;
	}

	if (transfer->sub_address_len > 0 && !transfer->sub_address)
		return -EINVAL;

	if (!transfer->len && !transfer->sub_address_len)
		return -EINVAL;

	i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);
	addr = transfer->target_addr ? transfer->target_addr : device->address;
	hold = _max_capi_i2c_rx_limit(transfer->len);
	/* Each poll iteration takes at least one core clock */
	budget = (SystemCoreClock / i2c_priv->freq) * MAX_CAPI_I2C_IDLE_BITS;

	/* Before the FIFOs are cleared: a recovery can leave bytes in them */
	_max_capi_i2c_check_bus(i2c_priv, i3c);
	MXC_I3C_ClearTXFIFO(i3c);
	MXC_I3C_ClearRXFIFO(i3c);

	if (transfer->sub_address_len) {
		ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_WRITE,
					addr, 0);
		if (ret) {
			ret = (ret == E_TIME_OUT) ? -ETIMEDOUT : -EIO;
			goto stop;
		}

		timeout = budget * transfer->sub_address_len;
		if (MXC_I3C_WriteTXFIFO(i3c, transfer->sub_address,
					transfer->sub_address_len, true,
					timeout) != transfer->sub_address_len) {
			ret = -ETIMEDOUT;
			goto stop;
		}

		/* A repeated START requested before DONE breaks the read */
		while (!(i3c->cont_status & MXC_F_I3C_CONT_STATUS_DONE)) {
			ret = _max_capi_i2c_poll(i3c, &timeout);
			if (ret)
				goto stop;
		}

		if (!transfer->len) {
			ret = _max_capi_i2c_bus_error(i3c);
			goto stop;
		}
	}

	/*
	 * term_rd = len makes the controller NACK the last byte by itself. With
	 * term_rd = 0, it ACKs every byte until told to stop.
	 */
	ret = MXC_I3C_EmitStart(i3c, true, MXC_I3C_TRANSFER_TYPE_READ, addr,
				_max_capi_i2c_read_count(transfer->len));
	if (ret) {
		ret = (ret == E_TIME_OUT) ? -ETIMEDOUT : -EIO;
		goto stop;
	}
	reading = true;

	timeout = budget;
	while (idx < hold) {
		if (_max_capi_i2c_drain_rx(i3c, transfer->buf, &idx, hold)) {
			timeout = budget;
			continue;
		}

		ret = _max_capi_i2c_poll(i3c, &timeout);
		if (ret)
			goto stop;
	}

	if (hold < transfer->len) {
		ret = _max_capi_i2c_term_read(i3c, budget);
		if (ret)
			goto stop;
	}

	ret = _max_capi_i2c_finish_read(i3c, transfer->buf, &idx, transfer->len,
					budget);

stop:
	if (ret && reading)
		_max_capi_i2c_abort_read(i3c, budget);

	/* Bus errors clear at the STOP, so they are read before it */
	if (ret)
		_max_capi_i2c_stop_failed(i3c, budget);
	else if (!transfer->no_stop)
		MXC_I3C_EmitI2CStop(i3c);

	if (ret == -ETIMEDOUT)
		i2c_priv->recover_pending = true;

	return ret;
}

/**
 * @brief Register a callback
 * @param handle The I2C controller handle
 * @param callback The callback function
 * @param callback_arg Argument for the callback function
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_register_callback(struct capi_i2c_controller_handle
		*handle,
		capi_i2c_callback const callback,
		void *const callback_arg)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i2c_priv->callback = callback;
	i2c_priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Configures the I2C bus speed
 * @param handle The I2C controller handle
 * @param speed The clock speed
 * @param duty_cycle Ignored - automatically determined by hardware
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_configure_bus_speed(struct capi_i2c_controller_handle
		*handle,
		enum capi_i2c_speed speed,
		uint8_t duty_cycle)
{
	int ret;
	uint32_t pp_freq, od_freq, i2c_freq;
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;
	i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

	switch (speed) {
	case CAPI_I2C_SPEED_STANDARD:
		i2c_freq = MAX_CAPI_I2C_SPEED_STANDARD;
		break;
	case CAPI_I2C_SPEED_FAST:
		i2c_freq = MAX_CAPI_I2C_SPEED_FAST;
		break;
	case CAPI_I2C_SPEED_FAST_PLUS:
		i2c_freq = MAX_CAPI_I2C_SPEED_FAST_PLUS;
		break;
	default:
		return -EINVAL;
	}

	ret = _max_capi_i2c_get_frequencies(i2c_freq, &pp_freq, &od_freq);
	if (ret)
		return ret;

	i2c_priv->freq = i2c_freq;
	ret = MXC_I3C_SetPPFrequency(i3c, pp_freq);
	if (ret < 0)
		return -EINVAL;
	ret = MXC_I3C_SetODFrequency(i3c, od_freq, false);
	if (ret < 0)
		return -EINVAL;
	ret = MXC_I3C_SetI2CFrequency(i3c, i2c_freq);
	if (ret < 0)
		return -EINVAL;

	return 0;
}

/**
 * @brief Async transmit function
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_transmit_async(struct capi_i2c_device *device,
				       struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - set TX data */
		_max_capi_i2c_target_set_tx_data(i2c_priv, transfer->buf, transfer->len);

		return 0;
	} else {
		/* Controller mode - do the transmit async */
		int ret = _max_capi_i2c_setup_async(device, transfer);
		if (ret)
			return ret;

		mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

		if (i2c_priv->dma_handle)
			ret = _max_capi_i2c_transmit_dma(i2c_priv, i3c, i2c_priv->async);
		else if (i2c_priv->use_irq)
			ret = _max_capi_i2c_transmit_fifo(i3c, i2c_priv->async);
		else
			ret = -ENOTSUP;

		if (ret)
			_max_capi_i2c_reset_async_state(i2c_priv);

		return ret;
	}
}

/**
 * @brief Async receive function
 *
 * In controller mode, a read longer than MAX_CAPI_I2C_TERM_RD_MAX bytes is
 * ended with term_rd = 1 as in max_capi_i2c_receive(). With FIFO interrupts,
 * the controller interrupt does this, polling for at most two byte times. With
 * DMA, the DMA moves all but the last MAX_CAPI_I2C_FIFO_DEPTH +
 * MAX_CAPI_I2C_TERM_TAIL bytes, and the DMA completion callback polls for
 * those, which takes about as many byte times.
 *
 * @param device The I2C device
 * @param transfer The I2C transfer
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_receive_async(struct capi_i2c_device *device,
				      struct capi_i2c_transfer *transfer)
{
	struct max_capi_i2c_priv *i2c_priv;

	if (!device || !device->controller || !device->controller->priv || !transfer)
		return -EINVAL;

	i2c_priv = device->controller->priv;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled) {
		/* Target mode - get RX data */
		uint8_t *rx_data;
		uint16_t rx_len;

		_max_capi_i2c_target_get_rx_data(i2c_priv, &rx_data, &rx_len);

		if (rx_len > transfer->len)
			return -EOVERFLOW;

		memcpy(transfer->buf, rx_data, rx_len);
		transfer->len = rx_len;

		return 0;
	} else {
		/* Controller mode - do the receive async */
		int ret;

		ret = _max_capi_i2c_setup_async(device, transfer);
		if (ret)
			return ret;

		mxc_i3c_regs_t *i3c = MXC_I3C_GET_I3C(i2c_priv->identifier);

		if (i2c_priv->dma_handle)
			ret = _max_capi_i2c_receive_dma(i2c_priv, i3c, i2c_priv->async);
		else if (i2c_priv->use_irq)
			ret = _max_capi_i2c_receive_fifo(i3c, i2c_priv->async);
		else
			ret = -ENOTSUP;

		if (ret)
			_max_capi_i2c_reset_async_state(i2c_priv);

		return ret;
	}
}

/**
 * @brief Attempt to recover the I2C bus
 * @param handle The I2C controller handle
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_recover_bus(struct capi_i2c_controller_handle *handle)
{
	if (!handle || !handle->priv)
		return -EINVAL;

	return _max_capi_i2c_recover(handle->priv);
}

/**
 * @brief Register the I2C controller in target mode
 * @param handle The I2C controller handle
 * @param addr Valid 7-bit or 10-bit I2C address
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_register_target(struct capi_i2c_controller_handle
					*handle,
					uint16_t addr)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	/* Validate address */
	if (addr > 0x3FF)
		return -EINVAL;
	if (addr <= 0x7F) {
		if (addr <= 0x07 || addr >= 0x78)
			return -EINVAL;
	}

	i2c_priv = handle->priv;
	i2c_id = i2c_priv->identifier;

	if (!i2c_priv->use_irq)
		return -ENOTSUP;

	if (i2c_priv->target != NULL && i2c_priv->target->enabled)
		return -EBUSY;

	ret = _max_capi_i2c_configure_hardware(i2c_priv, true, addr);
	if (ret)
		return ret;

	if (!i2c_priv->target)
		return -ENOMEM;

	i2c_priv->target->enabled = true;
	i2c_priv->target->address = addr;
	i2c_priv->target->b10addr = (addr > 0x7F);
	i2c_priv->target->state = MAX_CAPI_I2C_TARGET_STATE_IDLE;
	i2c_priv->target->rx_count = 0;
	i2c_priv->target->tx_count = 0;
	i2c_priv->target->tx_buffer = NULL;
	i2c_priv->target->tx_length = 0;

	i3c = MXC_I3C_GET_I3C(i2c_id);
	MXC_I3C_Target_ClearFlags(i3c, 0xFFFFFFFF);
	MXC_I3C_Target_EnableInt(i3c, MXC_F_I3C_TARG_INTEN_START |
				 MXC_F_I3C_TARG_INTEN_ADDRMATCH |
				 MXC_F_I3C_TARG_INTEN_STOP |
				 MXC_F_I3C_TARG_INTEN_RX_RDY |
				 MXC_F_I3C_TARG_INTEN_ERRWARN);

	return 0;
}

/**
 * @brief Unregister the I2C controller from target mode
 * @param handle The I2C controller handle
 * @return 0 on success, negative error code otherwise
 */
static int max_capi_i2c_unregister_target(struct capi_i2c_controller_handle
		*handle)
{
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;
	int ret;

	if (!handle || !handle->priv)
		return -EINVAL;

	i2c_priv = handle->priv;

	if (i2c_priv->target == NULL || !i2c_priv->target->enabled)
		return -EINVAL;

	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	MXC_I3C_Target_DisableInt(i3c, MXC_F_I3C_TARG_INTCLR_START |
				  MXC_F_I3C_TARG_INTCLR_ADDRMATCH |
				  MXC_F_I3C_TARG_INTCLR_STOP |
				  MXC_F_I3C_TARG_INTCLR_RX_RDY |
				  MXC_F_I3C_TARG_INTCLR_TX_NFULL |
				  MXC_F_I3C_TARG_INTCLR_ERRWARN);

	i2c_priv->target->enabled = false;

	ret = _max_capi_i2c_configure_hardware(i2c_priv, false, i2c_priv->freq);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief The function called during an interrupt
 * @param handle The I2C handle
 */
static void max_capi_i2c_isr(void *handle)
{
	struct capi_i2c_controller_handle *i2c_handle;
	struct max_capi_i2c_priv *i2c_priv;
	mxc_i3c_regs_t *i3c;
	uint8_t i2c_id;

	if (!handle)
		return;

	i2c_handle = (struct capi_i2c_controller_handle *)handle;
	if (!i2c_handle->priv)
		return;

	i2c_priv = i2c_handle->priv;
	i2c_id = i2c_priv->identifier;
	i3c = MXC_I3C_GET_I3C(i2c_id);

	if (i2c_priv->target && (MXC_I3C_Target_GetFlags(i3c) != 0)) {
		_max_capi_i2c_target_isr(i2c_priv, i3c);
	} else if (MXC_I3C_Controller_GetFlags(i3c) != 0) {
		_max_capi_i2c_controller_isr(i2c_priv, i3c);
	} else {
		uint32_t target_flags = MXC_I3C_Target_GetFlags(i3c);
		uint32_t controller_flags = MXC_I3C_Controller_GetFlags(i3c);

		if (target_flags)
			MXC_I3C_Target_ClearFlags(i3c, target_flags);
		if (controller_flags)
			MXC_I3C_Controller_ClearFlags(i3c, controller_flags);
	}
}

const struct capi_i2c_ops max_capi_i2c_ops = {
	.init = max_capi_i2c_init,
	.deinit = max_capi_i2c_deinit,
	.transmit = max_capi_i2c_transmit,
	.receive = max_capi_i2c_receive,
	.register_callback = max_capi_i2c_register_callback,
	.configure_bus_speed = max_capi_i2c_configure_bus_speed,
	.transmit_async = max_capi_i2c_transmit_async,
	.receive_async = max_capi_i2c_receive_async,
	.recover_bus = max_capi_i2c_recover_bus,
	.register_target = max_capi_i2c_register_target,
	.unregister_target = max_capi_i2c_unregister_target,
	.isr = max_capi_i2c_isr,
};
