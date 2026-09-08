/*******************************************************************************
 *   @file   maxim_capi_uart.c
 *   @brief  Implementation of UART functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Common Maxim CAPI UART backend.
 *
 * One file for every MSDK target. The differences between parts are isolated to
 * a handful of static port-shims near the top:
 *
 *   _max_uart_resolve_clk() - map CAPI clk_freq_hz to the MSDK clock enum, only
 *                             on parts whose Init takes a clock argument.
 *   _max_uart_hw_init()     - the four MXC_UART_Init() signature families:
 *                               (uart,baud)                32520/32570/32650
 *                               (uart,baud,map)            32660/32665
 *                               (uart,baud,clock)          majority incl. 32657
 *                               (uart,baud,clock,map)      32662
 *   _max_uart_set_freq()    - MXC_UART_SetFrequency() 2-arg vs 3-arg.
 *   _max_uart_pins_config() - board pin mux (gpio_cfg_uart), gated per board.
 *
 * The interrupt-flag register (member name and MXC_F_UART_INT[_]FL_* bits) is
 * accessed through the MAX_UART_* rev-shim below, which absorbs the flat
 * (e.g. MAX32657) vs RevB (e.g. MAX32655) naming split. Remaining FIFO/status/
 * dma field pokes follow the RevB layout; RevA/RevC field differences are a
 * follow-up.
 */

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "capi_alloc.h"
#include "capi_dma.h"
#include "capi_irq.h"
#include "capi_uart.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_uart.h"
#include "maxim_capi_uart_priv.h"
#include <sys/stat.h>

/** Register rev-shim *********************************************************/

/*
 * MSDK ships SIX mutually exclusive UART interrupt-flag naming conventions
 * across the 16 targets. They do not track the die or the RevA/RevB/RevC
 * driver split -- MAX32657 and MAX32662 are both RevB yet spell the flags
 * differently, and the three RevA parts disagree with each other -- so the
 * convention is selected by FEATURE PROBE on a spelling unique to each, never
 * by TARGET_NUM. Verified against MSDK v2023_10-561-g95b824f8ba:
 *
 *   A flat/INTFL   member `inten`   MXC_F_UART_INTFL_RX_FERR      32657 32662
 *   B RevB/INT_FL  member `int_en`  MXC_F_UART_INT_FL_RX_FERR     32655 32670
 *                                                                 32672 32675
 *                                                                 32680 32690
 *                                                                 78000 78002
 *   C RevA/ME14    member `int_en`  ..._RX_FRAME_ERROR            32665
 *   D RevA/ME13    member `int_en`  ..._FRAME + ..._RX_OVERRUN    32570
 *   E RevA/ME10-11 member `int_en`  ..._FRAME + ..._RX_OVR        32650 32660
 *   F ES17         member `int_stat`, an unrelated register model 32520
 *
 * Everything below is written once against the neutral MAX_UART_FL_* names. A
 * flag a convention does not have collapses to 0 so the OR-masks stay valid;
 * MAX_UART_HAS_INT_FLAGS is 0 where the whole model is absent, and callers
 * degrade to "no interrupt-reason detail" rather than reading a wrong bit.
 *
 * Semantic note for the RevA rows: TX_FIFO_ALMOST_EMPTY is the same condition
 * RevB calls TX_HE, and RX/TX_FIFO_{THRESH,LVL} are its RX/TX_THD. They are
 * mapped accordingly rather than being zeroed, so threshold-driven paths keep
 * working on those parts.
 */
/*
 * Each neutral flag is probed INDEPENDENTLY rather than as part of a
 * convention block, because the conventions are not internally consistent:
 * MAX32662 uses the flat INTFL_ spelling yet has no RX_FULL or TX_THD, while
 * MAX32657 has both and no TX_HE. Probing per flag means a part contributes
 * exactly the bits it really has, and a missing one collapses to 0 so the
 * OR-masks below stay valid.
 *
 * Semantic equivalences used by the RevA rows: TX_FIFO_ALMOST_EMPTY is the
 * condition RevB calls TX_HE, and RX/TX_FIFO_{THRESH,LVL} are its RX/TX_THD.
 */

/* The int-enable member: only the flat layout renames it. */
#ifdef MXC_F_UART_INTFL_RX_FERR
#define MAX_UART_INTEN(u)	((u)->inten)
#else
#define MAX_UART_INTEN(u)	((u)->int_en)
#endif

/*
 * Whether this part has a decodable interrupt-flag model at all. Set in the
 * ladder rather than computed from the flag values: the MXC_F_* macros expand
 * to casts, which #if cannot evaluate.
 */

/* RX framing error */
#if   defined(MXC_F_UART_INTFL_RX_FERR)
#define MAX_UART_HAS_INT_FLAGS	1
#define MAX_UART_FL_RX_FERR	MXC_F_UART_INTFL_RX_FERR
#elif defined(MXC_F_UART_INT_FL_RX_FERR)
#define MAX_UART_HAS_INT_FLAGS	1
#define MAX_UART_FL_RX_FERR	MXC_F_UART_INT_FL_RX_FERR
#elif defined(MXC_F_UART_INT_FL_RX_FRAME_ERROR)
#define MAX_UART_HAS_INT_FLAGS	1
#define MAX_UART_FL_RX_FERR	MXC_F_UART_INT_FL_RX_FRAME_ERROR
#elif defined(MXC_F_UART_INT_FL_FRAME)
#define MAX_UART_HAS_INT_FLAGS	1
#define MAX_UART_FL_RX_FERR	MXC_F_UART_INT_FL_FRAME
#else
#define MAX_UART_HAS_INT_FLAGS	0
#define MAX_UART_FL_RX_FERR	0U
#endif

/* RX parity error */
#if   defined(MXC_F_UART_INTFL_RX_PAR)
#define MAX_UART_FL_RX_PAR	MXC_F_UART_INTFL_RX_PAR
#elif defined(MXC_F_UART_INT_FL_RX_PAR)
#define MAX_UART_FL_RX_PAR	MXC_F_UART_INT_FL_RX_PAR
#elif defined(MXC_F_UART_INT_FL_RX_PARITY_ERROR)
#define MAX_UART_FL_RX_PAR	MXC_F_UART_INT_FL_RX_PARITY_ERROR
#elif defined(MXC_F_UART_INT_FL_PARITY)
#define MAX_UART_FL_RX_PAR	MXC_F_UART_INT_FL_PARITY
#else
#define MAX_UART_FL_RX_PAR	0U
#endif

/* RX overrun */
#if   defined(MXC_F_UART_INTFL_RX_OV)
#define MAX_UART_FL_RX_OV	MXC_F_UART_INTFL_RX_OV
#elif defined(MXC_F_UART_INT_FL_RX_OV)
#define MAX_UART_FL_RX_OV	MXC_F_UART_INT_FL_RX_OV
#elif defined(MXC_F_UART_INT_FL_RX_OVERRUN)
#define MAX_UART_FL_RX_OV	MXC_F_UART_INT_FL_RX_OVERRUN
#elif defined(MXC_F_UART_INT_FL_RX_OVR)
#define MAX_UART_FL_RX_OV	MXC_F_UART_INT_FL_RX_OVR
#else
#define MAX_UART_FL_RX_OV	0U
#endif

/* RX threshold reached */
#if   defined(MXC_F_UART_INTFL_RX_THD)
#define MAX_UART_FL_RX_THD	MXC_F_UART_INTFL_RX_THD
#elif defined(MXC_F_UART_INT_FL_RX_THD)
#define MAX_UART_FL_RX_THD	MXC_F_UART_INT_FL_RX_THD
#elif defined(MXC_F_UART_INT_FL_RX_FIFO_THRESH)
#define MAX_UART_FL_RX_THD	MXC_F_UART_INT_FL_RX_FIFO_THRESH
#elif defined(MXC_F_UART_INT_FL_RX_FIFO_LVL)
#define MAX_UART_FL_RX_THD	MXC_F_UART_INT_FL_RX_FIFO_LVL
#else
#define MAX_UART_FL_RX_THD	0U
#endif

/* RX FIFO full */
#ifdef MXC_F_UART_INTFL_RX_FULL
#define MAX_UART_FL_RX_FULL	MXC_F_UART_INTFL_RX_FULL
#else
#define MAX_UART_FL_RX_FULL	0U
#endif

/* TX threshold reached */
#if   defined(MXC_F_UART_INTFL_TX_THD)
#define MAX_UART_FL_TX_THD	MXC_F_UART_INTFL_TX_THD
#elif defined(MXC_F_UART_INT_FL_TX_FIFO_THRESH)
#define MAX_UART_FL_TX_THD	MXC_F_UART_INT_FL_TX_FIFO_THRESH
#elif defined(MXC_F_UART_INT_FL_TX_FIFO_LVL)
#define MAX_UART_FL_TX_THD	MXC_F_UART_INT_FL_TX_FIFO_LVL
#else
#define MAX_UART_FL_TX_THD	0U
#endif

/* TX FIFO one byte remaining */
#if   defined(MXC_F_UART_INTFL_TX_OB)
#define MAX_UART_FL_TX_OB	MXC_F_UART_INTFL_TX_OB
#elif defined(MXC_F_UART_INT_FL_TX_OB)
#define MAX_UART_FL_TX_OB	MXC_F_UART_INT_FL_TX_OB
#else
#define MAX_UART_FL_TX_OB	0U
#endif

/* TX FIFO half/almost empty */
#if   defined(MXC_F_UART_INTFL_TX_HE)
#define MAX_UART_FL_TX_HE	MXC_F_UART_INTFL_TX_HE
#elif defined(MXC_F_UART_INT_FL_TX_HE)
#define MAX_UART_FL_TX_HE	MXC_F_UART_INT_FL_TX_HE
#elif defined(MXC_F_UART_INT_FL_TX_FIFO_ALMOST_EMPTY)
#define MAX_UART_FL_TX_HE	MXC_F_UART_INT_FL_TX_FIFO_ALMOST_EMPTY
#elif defined(MXC_F_UART_INT_FL_TX_FIFO_AE)
#define MAX_UART_FL_TX_HE	MXC_F_UART_INT_FL_TX_FIFO_AE
#else
#define MAX_UART_FL_TX_HE	0U
#endif

/* CTS change */
#if   defined(MXC_F_UART_INTFL_CTS_EV)
#define MAX_UART_FL_CTS_EV	MXC_F_UART_INTFL_CTS_EV
#elif defined(MXC_F_UART_INT_FL_CTS_EV)
#define MAX_UART_FL_CTS_EV	MXC_F_UART_INT_FL_CTS_EV
#elif defined(MXC_F_UART_INT_FL_CTS_CHANGE)
#define MAX_UART_FL_CTS_EV	MXC_F_UART_INT_FL_CTS_CHANGE
#elif defined(MXC_F_UART_INT_FL_CTS)
#define MAX_UART_FL_CTS_EV	MXC_F_UART_INT_FL_CTS
#else
#define MAX_UART_FL_CTS_EV	0U
#endif

/*
 * ES17 (MAX32520) models UART interrupts through an int_stat register with an
 * unrelated field set (FFRXIE/FFTXHIE/FRAMIE/...), so every probe above misses
 * and MAX_UART_HAS_INT_FLAGS lands on 0: interrupt-reason and line-status
 * reporting degrade to "no detail" there rather than decoding a wrong bit.
 * Blocking transfers are unaffected.
 */

/*
 * Status-register flags. Four spellings of the same two bits across the
 * matrix, again probed rather than tabulated.
 */
#if   defined(MXC_F_UART_STATUS_TX_EM)
#define MAX_UART_ST_TX_EMPTY	MXC_F_UART_STATUS_TX_EM
#elif defined(MXC_F_UART_STATUS_TX_EMPTY)
#define MAX_UART_ST_TX_EMPTY	MXC_F_UART_STATUS_TX_EMPTY
#elif defined(MXC_F_UART_STAT_TX_EMPTY)
#define MAX_UART_ST_TX_EMPTY	MXC_F_UART_STAT_TX_EMPTY
#elif defined(MXC_F_UART_STAT_TXEMPTY)
#define MAX_UART_ST_TX_EMPTY	MXC_F_UART_STAT_TXEMPTY
#else
#error "No known TX-empty status flag in this target's uart_regs.h"
#endif

#if   defined(MXC_F_UART_STATUS_TX_BUSY)
#define MAX_UART_ST_TX_BUSY	MXC_F_UART_STATUS_TX_BUSY
#elif defined(MXC_F_UART_STAT_TX_BUSY)
#define MAX_UART_ST_TX_BUSY	MXC_F_UART_STAT_TX_BUSY
#elif defined(MXC_F_UART_STAT_TXBUSY)
#define MAX_UART_ST_TX_BUSY	MXC_F_UART_STAT_TXBUSY
#else
#error "No known TX-busy status flag in this target's uart_regs.h"
#endif

/*
 * ME10 and ME14 declare MXC_UART_Clear{RX,TX}FIFO() as void; every other
 * target returns int. Wrap both shapes so the callers stay one expression --
 * on the void parts the operation cannot fail, so success is reported.
 */
#if (TARGET_NUM == 32650) || (TARGET_NUM == 32665)
static inline int _max_uart_clear_rx_fifo(mxc_uart_regs_t *uart)
{
	MXC_UART_ClearRXFIFO(uart);

	return 0;
}

static inline int _max_uart_clear_tx_fifo(mxc_uart_regs_t *uart)
{
	MXC_UART_ClearTXFIFO(uart);

	return 0;
}
#else
static inline int _max_uart_clear_rx_fifo(mxc_uart_regs_t *uart)
{
	return MXC_UART_ClearRXFIFO(uart) == E_NO_ERROR ? 0 : -EIO;
}

static inline int _max_uart_clear_tx_fifo(mxc_uart_regs_t *uart)
{
	return MXC_UART_ClearTXFIFO(uart) == E_NO_ERROR ? 0 : -EIO;
}
#endif

/** Static declarations *******************************************************/

static struct capi_uart_handle *uart[MXC_UART_INSTANCES] = {NULL};
static int8_t stdio_index = -1;

/** Forward declarations ******************************************************/

void max_capi_uart_isr(void *handle);

/** Port shims ****************************************************************/

/**
 * @brief Resolve the requested peripheral clock into the MSDK clock enum.
 * @param clk_freq_hz - the requested clock rate (0 = default).
 * @param clk_out - where to store the resolved mxc_uart_clock_t value.
 * @return 0 on success, negative error code otherwise.
 *
 * On parts whose MXC_UART_Init() takes no clock argument this is a no-op.
 */
static int _max_uart_resolve_clk(uint32_t clk_freq_hz, uint32_t *clk_out)
{
#if MAX_CAPI_UART_CLOCK_ARG
	if (clk_freq_hz == 0 || clk_freq_hz == PeripheralClock)
		/** Default / APB clock */
		*clk_out = (uint32_t)MXC_UART_APB_CLK;
	else if (clk_freq_hz == IBRO_FREQ)
		/** IBRO = 7.3728 MHz */
		*clk_out = (uint32_t)MXC_UART_IBRO_CLK;
	else
		return -ENOTSUP;
#else
	(void)clk_freq_hz;
	*clk_out = 0;
#endif
	return 0;
}

/**
 * @brief Initialize the UART hardware, absorbing per-part Init signatures.
 * @param regs - the UART registers.
 * @param baud - the baud rate.
 * @param clk - the resolved clock source (ignored where unused).
 * @return MSDK status code.
 */
static int _max_uart_hw_init(mxc_uart_regs_t *regs, unsigned int baud,
			     uint32_t clk)
{
#if (TARGET_NUM == 32520) || (TARGET_NUM == 32570) || (TARGET_NUM == 32650)
	(void)clk;
	return MXC_UART_Init(regs, baud);
#elif (TARGET_NUM == 32660) || (TARGET_NUM == 32665)
	(void)clk;
	return MXC_UART_Init(regs, baud, MAP_A);
#elif (TARGET_NUM == 32662)
	return MXC_UART_Init(regs, baud, (mxc_uart_clock_t)clk, MAP_A);
#else
	return MXC_UART_Init(regs, baud, (mxc_uart_clock_t)clk);
#endif
}

/**
 * @brief Set the UART baud rate, absorbing the 2-arg vs 3-arg signature.
 * @param regs - the UART registers.
 * @param baud - the baud rate.
 * @param clk - the resolved clock source (ignored where unused).
 * @return MSDK status code (actual baud, or negative on error).
 */
static int _max_uart_set_freq(mxc_uart_regs_t *regs, unsigned int baud,
			      uint32_t clk)
{
#if MAX_CAPI_UART_CLOCK_ARG
	return MXC_UART_SetFrequency(regs, baud, (mxc_uart_clock_t)clk);
#else
	(void)clk;
	return MXC_UART_SetFrequency(regs, baud);
#endif
}

/** Helper functions **********************************************************/

/**
 * @brief Configure the VDDIO for the UART pins.
 * @param vssel - the voltage level of the interface.
 * @return 0 in case of success, errno codes otherwise.
 */
static int32_t _max_uart_pins_config(enum max_capi_gpio_vssel vssel)
{
#if MAX_CAPI_UART_CFG_PINS
	mxc_gpio_cfg_t uart_pins = gpio_cfg_uart;

	uart_pins.vssel = (mxc_gpio_vssel_t)vssel;

	return MXC_GPIO_Config(&uart_pins);
#else
	(void)vssel;
	return 0;
#endif
}

#if MAX_CAPI_UART_HAS_DMA
/**
 * @brief Release a DMA channel
 * @param channel Pointer to a channel pointer
 */
static void _max_capi_uart_dma_cleanup_channel(struct capi_dma_chan **channel)
{
	if (channel && *channel) {
		capi_dma_xfer_abort(*channel);
		capi_dma_deinit_chan(*channel);
		*channel = NULL;
	}
}
#endif /* MAX_CAPI_UART_HAS_DMA */

/**
 * @brief MSDK completion callback function
 * @param req The UART
 * @param result Result
 */
static void _max_capi_uart_msdk_callback(mxc_uart_req_t *req, int result)
{
	uint32_t id = MXC_UART_GET_IDX(req->uart);
	struct capi_uart_handle *handle = uart[id];
	struct max_capi_uart_priv *uart_priv;
	enum capi_uart_async_event event;
	bool is_tx;

	if (!handle || !handle->priv)
		return;

	uart_priv = handle->priv;
	is_tx = (req->txData != NULL);

	if (result == E_NO_ERROR)
		event = is_tx ? CAPI_UART_EVENT_TX_DONE : CAPI_UART_EVENT_RX_DONE;
	else if (result == E_ABORT && is_tx)
		event = CAPI_UART_EVENT_TX_ABORTED;
	else
		event = CAPI_UART_EVENT_INTERRUPT;

	/*
	 * MSDK invokes this per direction with the direction's own request, so
	 * clear only the flag for the leg that just finished; the other may
	 * still be in flight (loopback runs TX and RX concurrently).
	 */
	if (is_tx)
		uart_priv->tx_in_progress = false;
	else
		uart_priv->rx_in_progress = false;

	if (uart_priv->callback)
		uart_priv->callback(event, uart_priv->callback_arg, result);
}

#if MAX_CAPI_UART_HAS_DMA
/**
 * @brief DMA transfer complete callback function
 * @param event The event indicating completion status
 * @param ctx The private struct passed into the callback
 */
static void _max_capi_uart_dma_complete(uint32_t event, void *ctx)
{
	struct max_capi_uart_priv *uart_priv = (struct max_capi_uart_priv *)ctx;
	capi_uart_callback callback;
	void *callback_arg;

	if (!uart_priv)
		return;

	callback = uart_priv->callback;
	callback_arg = uart_priv->callback_arg;

	uart_priv->dma_completed = true;

	uart_priv->uart->dma &= ~(MXC_F_UART_DMA_TX_EN | MXC_F_UART_DMA_RX_EN);
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_rx);
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_tx);

	/* The DMA completion tears down both channels, so both legs are idle. */
	uart_priv->tx_in_progress = false;
	uart_priv->rx_in_progress = false;

	if (callback)
		callback(event, callback_arg, 0);
}

/**
 * @brief Receive with DMA
 * @param priv The private structure
 * @param buf The buffer to store the data to
 * @param len The length of the data
 */
static int _max_capi_uart_receive_dma(struct max_capi_uart_priv *priv,
				      uint8_t *buf, uint32_t len)
{
	int ret;

	ret = capi_dma_init_chan(priv->dma_handle, &priv->dma_channel_rx, 0);
	if (ret)
		goto error;

	priv->dma_xfer_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_UART_RX,
	};
	priv->dma_xfer = (struct capi_dma_transfer) {
		.src = (capi_dma_glbl_addr_t)&priv->uart->fifo,
		.dst = (capi_dma_glbl_addr_t)buf,
		.src_inc = CAPI_DMA_NO_INCREMENT,
		.dst_inc = CAPI_DMA_BYTE_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = len,
		.xfer_type = CAPI_DMA_DEV_TO_MEM,
		.user_data = priv,
		.extra = &priv->dma_xfer_extra,
	};

	priv->uart->dma |= MXC_F_UART_DMA_RX_EN;

	ret = capi_dma_config_xfer(priv->dma_channel_rx, &priv->dma_xfer);
	if (ret)
		goto error_deinit;

	ret = capi_dma_register_complete_callback(priv->dma_channel_rx,
			_max_capi_uart_dma_complete,
			priv);
	if (ret)
		goto error_deinit;

	priv->dma_completed = false;
	priv->rx_in_progress = true;

	ret = capi_dma_xfer_start(priv->dma_channel_rx);
	if (ret)
		goto error_deinit;

	return 0;

error_deinit:
	priv->uart->dma &= ~MXC_F_UART_DMA_RX_EN;
	_max_capi_uart_dma_cleanup_channel(&priv->dma_channel_rx);
error:
	priv->rx_in_progress = false;

	return ret;
}

/**
 * @brief Transmit with DMA
 * @brief priv The private structure
 * @param buf The buffer to get data from
 * @param len The length of the data
 */
static int _max_capi_uart_transmit_dma(struct max_capi_uart_priv *priv,
				       uint8_t *buf, uint32_t len)
{
	int ret;

	ret = capi_dma_init_chan(priv->dma_handle, &priv->dma_channel_tx, 0);
	if (ret)
		goto error;

	priv->dma_xfer_extra = (struct max_capi_dma_xfer_extra) {
		.reqsel = MAX_CAPI_DMA_REQUEST_UART_TX,
	};
	priv->dma_xfer = (struct capi_dma_transfer) {
		.src = (capi_dma_glbl_addr_t)buf,
		.dst = (capi_dma_glbl_addr_t)&priv->uart->fifo,
		.src_inc = CAPI_DMA_BYTE_INCREMENT,
		.dst_inc = CAPI_DMA_NO_INCREMENT,
		.src_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.dst_size = CAPI_DMA_XFER_SIZE_1_BYTE,
		.length = len,
		.xfer_type = CAPI_DMA_MEM_TO_DEV,
		.user_data = priv,
		.extra = &priv->dma_xfer_extra,
	};

	priv->uart->dma |= MXC_F_UART_DMA_TX_EN;

	ret = capi_dma_config_xfer(priv->dma_channel_tx, &priv->dma_xfer);
	if (ret)
		goto error_deinit;

	ret = capi_dma_register_complete_callback(priv->dma_channel_tx,
			_max_capi_uart_dma_complete,
			priv);
	if (ret)
		goto error_deinit;

	priv->dma_completed = false;
	priv->tx_in_progress = true;

	ret = capi_dma_xfer_start(priv->dma_channel_tx);
	if (ret)
		goto error_deinit;

	return 0;

error_deinit:
	priv->uart->dma &= ~MXC_F_UART_DMA_TX_EN;
	_max_capi_uart_dma_cleanup_channel(&priv->dma_channel_tx);
error:
	priv->tx_in_progress = false;

	return ret;
}
#endif /* MAX_CAPI_UART_HAS_DMA */

/**
 * @brief Converts CAPI enum values to msdk values for line config
 * @param line_config The line config
 * @param[out] size Number of data bits
 * @param[out] parity Parity
 * @param[out] stop Stop bits
 * @return 0 on success, negative error code otherwise
 */
static int _max_capi_uart_map_line_config(const struct capi_uart_line_config
		*line_config,
		uint32_t *size,
		mxc_uart_parity_t *parity,
		mxc_uart_stop_t *stop)
{
	switch (line_config->size) {
	case CAPI_UART_DATA_BITS_5:
		*size = 5;
		break;
	case CAPI_UART_DATA_BITS_6:
		*size = 6;
		break;
	case CAPI_UART_DATA_BITS_7:
		*size = 7;
		break;
	case CAPI_UART_DATA_BITS_8:
		*size = 8;
		break;
	default:
		return -EINVAL;
	}

	switch (line_config->parity) {
	case CAPI_UART_PARITY_NONE:
		*parity = MXC_UART_PARITY_DISABLE;
		break;
	case CAPI_UART_PARITY_ODD:
		*parity = MXC_UART_PARITY_ODD_1;
		break;
	case CAPI_UART_PARITY_EVEN:
		*parity = MXC_UART_PARITY_EVEN_1;
		break;
	case CAPI_UART_PARITY_MARK:
	case CAPI_UART_PARITY_SPACE:
		return -ENOTSUP;
	default:
		return -EINVAL;
	}

	switch (line_config->stop_bits) {
	case CAPI_UART_STOP_1_BIT:
		*stop = MXC_UART_STOP_1;
		break;
	case CAPI_UART_STOP_2_BIT:
		*stop = MXC_UART_STOP_2;
		break;
	default:
		return -EINVAL;
	}

	return 0;
}

/** UART implementation *******************************************************/

/**
 * @brief Initialize the UART peripheral
 * @param handle Pointer to a UART handle pointer
 * @param config UART config struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_init(struct capi_uart_handle **handle,
		       const struct capi_uart_config *config)
{
	int ret;

	struct capi_uart_handle *uart_handle;
	struct max_capi_uart_priv *uart_priv;
	struct max_capi_uart_extra *uart_extra;
	uint32_t baud_rate, size;
	mxc_uart_parity_t parity;
	mxc_uart_stop_t stop;
	struct capi_uart_line_config line_cfg;

	if (!handle || !config || !config->extra)
		return -EINVAL;

	if (config->identifier >= MXC_UART_INSTANCES)
		return -EINVAL;

	if (uart[config->identifier] != NULL) {
		*handle = uart[config->identifier];
		return 0;
	}

	if (*handle == NULL) {
		uart_handle = capi_calloc(1, sizeof(*uart_handle));
		if (!uart_handle)
			return -ENOMEM;

		uart_priv = capi_calloc(1, sizeof(*uart_priv));
		if (!uart_priv) {
			capi_free(uart_handle);
			return -ENOMEM;
		}

		uart_handle->priv = uart_priv;
		uart_handle->init_allocated = true;
	} else {
		uart_handle = *handle;

		if (!uart_handle->priv)
			return -EINVAL;

		uart_priv = uart_handle->priv;
		uart_handle->init_allocated = false;
	}

	uart_handle->ops = config->ops;

	uart_extra = config->extra;

	uart_priv->uart = MXC_UART_GET_UART(config->identifier);
	uart_priv->id = config->identifier;
	uart_priv->tx_in_progress = false;
	uart_priv->rx_in_progress = false;
	uart_priv->dma_completed = false;
	uart_priv->dma_channel_rx = NULL;
	uart_priv->dma_channel_tx = NULL;

	ret = _max_uart_resolve_clk(config->clk_freq_hz, &uart_priv->clk_src);
	if (ret)
		goto free_handle;

	baud_rate = MAX_CAPI_UART_DEFAULT_BAUD;
	if (config->line_config) {
		line_cfg = *config->line_config;
		baud_rate = config->line_config->baudrate;
	} else {
		line_cfg = (struct capi_uart_line_config) {
			.baudrate = MAX_CAPI_UART_DEFAULT_BAUD,
			.size = CAPI_UART_DATA_BITS_8,
			.parity = CAPI_UART_PARITY_NONE,
			.stop_bits = CAPI_UART_STOP_1_BIT,
		};
	}

	ret = _max_capi_uart_map_line_config(&line_cfg, &size, &parity, &stop);
	if (ret)
		goto free_handle;

	uart_priv->line_config = line_cfg;

	ret = _max_uart_hw_init(uart_priv->uart, baud_rate, uart_priv->clk_src);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_handle;
	}

	ret = _max_uart_pins_config(uart_extra->vssel);
	if (ret)
		goto free_handle;

	ret = MXC_UART_SetDataSize(uart_priv->uart, size);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_handle;
	}

	ret = MXC_UART_SetParity(uart_priv->uart, parity);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_handle;
	}

	ret = MXC_UART_SetStopBits(uart_priv->uart, stop);
	if (ret != E_NO_ERROR) {
		ret = -EINVAL;
		goto free_handle;
	}

#if MAX_CAPI_UART_HAS_DMA
	if (config->dma_handle && uart_extra->dma_config) {
		uart_priv->dma_handle = config->dma_handle;
		ret = capi_dma_init(&uart_priv->dma_handle,
				    uart_extra->dma_config);
		if (ret)
			goto shutdown_uart;
	}
#endif

	IRQn_Type irq = MXC_UART_GET_IRQ(config->identifier);
	ret = capi_irq_connect(irq, max_capi_uart_isr, uart_handle);
	if (ret)
		goto cleanup_channels;

	ret = capi_irq_enable(irq);
	if (ret)
		goto cleanup_channels;

	uart[config->identifier] = uart_handle;
	*handle = uart_handle;

	return 0;

cleanup_channels:
#if MAX_CAPI_UART_HAS_DMA
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_tx);
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_rx);
shutdown_uart:
#endif
	MXC_UART_Shutdown(uart_priv->uart);
free_handle:
	if (uart_handle->init_allocated) {
		capi_free(uart_priv);
		capi_free(uart_handle);
	}

	uart[config->identifier] = NULL;

	return ret;
}

/**
 * @brief Deinitialize the UART peripheral
 * @param handle The UART handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_deinit(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;
	uint8_t id;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;
	id = uart_priv->id;

	/*
	 * Cancel any async transfer still pending in the MSDK driver. Its
	 * per-instance request globals (AsyncTx/RxRequests[]) are cleared only by
	 * an explicit abort/stop -- MXC_UART_Shutdown() below leaves them set --
	 * so a receive that never completed (e.g. an unstrapped loopback, or a
	 * transfer the caller deinits before completion) would otherwise make the
	 * NEXT MXC_UART_TransactionAsync() on this instance return E_BUSY. Abort
	 * here, while async_tx_req/async_rx_req storage is still valid (before any
	 * free below), so the instance is clean for the next init.
	 */
	(void)MXC_UART_AbortAsync(uart_priv->uart);

#if MAX_CAPI_UART_HAS_DMA
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_rx);
	_max_capi_uart_dma_cleanup_channel(&uart_priv->dma_channel_tx);
	uart_priv->uart->dma &= ~(MXC_F_UART_DMA_RX_EN | MXC_F_UART_DMA_TX_EN);
#endif
	uart_priv->tx_in_progress = false;
	uart_priv->rx_in_progress = false;
	uart_priv->dma_completed = false;

	capi_irq_disable(MXC_UART_GET_IRQ(id));

	MXC_UART_Shutdown(uart_priv->uart);
	if (handle->init_allocated) {
		capi_free(uart_priv);
		capi_free(handle);
	}

	if (stdio_index == id)
		stdio_index = -1;

	uart[id] = NULL;

	return 0;
}

/**
 * @brief Read data from the UART device. Blocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to read.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_receive(struct capi_uart_handle *handle, uint8_t *buf,
			  uint32_t len)
{
	int ret, n;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv ||  !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	n = (int)len;

	ret = MXC_UART_Read(uart_priv->uart, buf, &n);
	if (ret != E_SUCCESS)
		return -EIO;

	return 0;
}

/**
 * @brief Write data to the UART device. Blocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to write.
 * @return 0 on success, negative error code otherwise
 *
 * Rev-sensitive: FIFO depth and TX status flags follow the RevB layout.
 */
int max_capi_uart_transmit(struct capi_uart_handle *handle, uint8_t *buf,
			   uint32_t len)
{
	struct max_capi_uart_priv *uart_priv;
	int32_t transferred = 0;
	int block_size, ret;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	while (len) {
		block_size = (MXC_UART_FIFO_DEPTH < len ? MXC_UART_FIFO_DEPTH : len);
		while (!(MXC_UART_GetStatus(uart_priv->uart) &
			 MAX_UART_ST_TX_EMPTY));
		ret = MXC_UART_Write(uart_priv->uart,
				     (uint8_t *)(buf + transferred),
				     &block_size);
		transferred += block_size;
		len -= block_size;

		if (ret != E_SUCCESS)
			return -EIO;
	}

	while (MXC_UART_GetStatus(uart_priv->uart) & MAX_UART_ST_TX_BUSY);

	return 0;
}

/**
 * @brief Read data from the UART device. Nonblocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer containing data.
 * @param len - Number of bytes to read.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_receive_async(struct capi_uart_handle *handle, uint8_t *buf,
				uint32_t len)
{
	int ret;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	if (uart_priv->rx_in_progress)
		return -EBUSY;

#if MAX_CAPI_UART_HAS_DMA
	if (uart_priv->dma_handle)
		return _max_capi_uart_receive_dma(uart_priv, buf, len);
#endif

	uart_priv->async_rx_req = (mxc_uart_req_t) {
		.uart = uart_priv->uart,
		.rxData = buf,
		.rxLen = len,
		.rxCnt = 0,
		.txData = NULL,
		.txLen = 0,
		.txCnt = 0,
		.callback = _max_capi_uart_msdk_callback,
	};

	uart_priv->rx_in_progress = true;

	ret = MXC_UART_TransactionAsync(&uart_priv->async_rx_req);
	if (ret != E_NO_ERROR) {
		uart_priv->rx_in_progress = false;
		return (ret == E_BUSY ? -EBUSY : -EIO);
	}

	return 0;
}

/**
 * @brief Write data from the UART device. Nonblocking function.
 * @param handle - UART handle.
 * @param buf - Pointer to buffer to store the data.
 * @param len - Number of bytes to write.
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_transmit_async(struct capi_uart_handle *handle,
				 uint8_t *buf, uint32_t len)
{
	int ret;
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !buf || !len)
		return -EINVAL;

	uart_priv = handle->priv;

	if (uart_priv->tx_in_progress)
		return -EBUSY;

#if MAX_CAPI_UART_HAS_DMA
	if (uart_priv->dma_handle)
		return _max_capi_uart_transmit_dma(uart_priv, buf, len);
#endif

	uart_priv->async_tx_req = (mxc_uart_req_t) {
		.uart = uart_priv->uart,
		.txData = buf,
		.txLen = len,
		.txCnt = 0,
		.rxData = NULL,
		.rxLen = 0,
		.rxCnt = 0,
		.callback = _max_capi_uart_msdk_callback,
	};

	uart_priv->tx_in_progress = true;

	ret = MXC_UART_TransactionAsync(&uart_priv->async_tx_req);
	if (ret != E_NO_ERROR) {
		uart_priv->tx_in_progress = false;
		return (ret == E_BUSY ? -EBUSY : -EIO);
	}

	return 0;
}

/**
 * @brief Register a callback
 * @param handle The UART handle
 * @param callback The callback function
 * @param callback_arg Callback arg
 */
int max_capi_uart_register_callback(struct capi_uart_handle *handle,
				    capi_uart_callback const callback,
				    void *const callback_arg)
{
	struct max_capi_uart_priv *uart_priv;
	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	uart_priv->callback = callback;
	uart_priv->callback_arg = callback_arg;

	return 0;
}

/**
 * @brief Interrupt function for UART
 * @param handle The UART handle
 */
void max_capi_uart_isr(void *handle)
{
	struct capi_uart_handle *uart_handle = (struct capi_uart_handle *)handle;
	struct max_capi_uart_priv *uart_priv;

	if (!uart_handle || !uart_handle->priv)
		return;

	uart_priv = uart_handle->priv;

	MXC_UART_AsyncHandler(uart_priv->uart);
}

/**
 * @brief Set the line config for UART
 * @param handle The UART handle
 * @param line_config The line config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_set_line_config(struct capi_uart_handle *handle,
				  struct capi_uart_line_config *line_config)
{
	struct max_capi_uart_priv *uart_priv;
	mxc_uart_parity_t parity;
	mxc_uart_stop_t stop;
	int ret;
	uint32_t size;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	uart_priv = handle->priv;

	ret = _max_capi_uart_map_line_config(line_config, &size, &parity, &stop);
	if (ret)
		return ret;

	ret = _max_uart_set_freq(uart_priv->uart, line_config->baudrate,
				 uart_priv->clk_src);
	if (ret < 0)
		return -EINVAL;

	ret = MXC_UART_SetDataSize(uart_priv->uart, size);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	ret = MXC_UART_SetParity(uart_priv->uart, parity);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	ret = MXC_UART_SetStopBits(uart_priv->uart, stop);
	if (ret != E_NO_ERROR)
		return -EINVAL;

	uart_priv->line_config = *line_config;

	return 0;
}

/**
 * @brief Get the line config for UART
 * @param handle The UART handle
 * @param line_config Where to store the line config
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_line_config(struct capi_uart_handle *handle,
				  struct capi_uart_line_config *line_config)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !line_config)
		return -EINVAL;

	uart_priv = handle->priv;

	*line_config = uart_priv->line_config;
	line_config->baudrate = MXC_UART_GetFrequency(uart_priv->uart);

	return 0;
}

/**
 * @brief Flush the RX FIFO
 * @param handle The UART handle
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_flush_rx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	return _max_uart_clear_rx_fifo(uart_priv->uart);
}

/**
 * @brief Flush the TX FIFO
 * @param handle The UART handle
 * @return 0 on succes, negative error code otherwise
 */
int max_capi_uart_flush_tx_fifo(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	return _max_uart_clear_tx_fifo(uart_priv->uart);
}

/**
 * @brief Get the number of bytes available in the RX FIFO
 * @param handle The UART handle
 * @param count Where to store the number of bytes
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_rx_fifo_count(struct capi_uart_handle *handle,
				    uint16_t *count)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	*count = MXC_UART_GetRXFIFOAvailable(uart_priv->uart);

	return 0;
}

/**
 * @brief Get the number of queued bytes in the TX FIFO
 * @param handle The UART handle
 * @param count Where to store the number of bytes
 * @return 0 on success, negative error code otherwise
 */
int max_capi_uart_get_tx_fifo_count(struct capi_uart_handle *handle,
				    uint16_t *count)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	*count = MXC_UART_FIFO_DEPTH - MXC_UART_GetTXFIFOAvailable(uart_priv->uart);

	return 0;
}

/**
 * @brief Get the reason for the interrupt
 * @param handle The UART handle
 * @param reason Where to store the interrupt reason
 * @return 0 on success, negative error code otherwise
 *
 * The interrupt-flag bits are read through the MAX_UART_FL_* rev-shim, so this
 * works on both the flat (INTFL) and RevB (INT_FL) register layouts.
 */
int max_capi_uart_get_interrupt_reason(struct capi_uart_handle *handle,
				       enum capi_uart_interrupt_reason *reason)
{
	struct max_capi_uart_priv *uart_priv;
	uint32_t flags;

	if (!handle || !handle->priv || !reason)
		return -EINVAL;

	uart_priv = handle->priv;

	flags = MXC_UART_GetFlags(uart_priv->uart) & MAX_UART_INTEN(uart_priv->uart);

	if (flags & (MAX_UART_FL_RX_FERR | MAX_UART_FL_RX_PAR |
		     MAX_UART_FL_RX_OV))
		*reason = CAPI_UART_INTR_RX_LINE_STATUS;
	else if (flags & (MAX_UART_FL_RX_THD | MAX_UART_FL_RX_FULL))
		*reason = CAPI_UART_INTR_RX_BUFFER_FULL;
	else if (flags & (MAX_UART_FL_TX_THD | MAX_UART_FL_TX_OB |
			  MAX_UART_FL_TX_HE))
		*reason = CAPI_UART_INTR_TX_BUFFER_EMPTY;
	else if (flags & MAX_UART_FL_CTS_EV)
		*reason = CAPI_UART_INTR_MODEM_STATUS;
	else
		return -ENODATA;

	return 0;
}

/**
 * @brief Get UART line status flags
 * @param handle The UART handle
 * @param status_flags Where to store the status flags
 * @return 0 on success, negative error code otherwise
 *
 * The interrupt-flag bits are read through the MAX_UART_FL_* rev-shim, so this
 * works on both the flat (INTFL) and RevB (INT_FL) register layouts.
 */
int max_capi_uart_get_line_status(struct capi_uart_handle *handle,
				  uint32_t *status_flags)
{
	struct max_capi_uart_priv *uart_priv;
	uint32_t flags;
	uint32_t clear = 0;
	uint32_t status = 0;

	if (!handle || !handle->priv || !status_flags)
		return -EINVAL;

	uart_priv = handle->priv;

	flags = MXC_UART_GetFlags(uart_priv->uart);

	if (flags & MAX_UART_FL_RX_FERR) {
		status |= CAPI_UART_LINE_STAT_FRAMING_ERROR;
		clear |= MAX_UART_FL_RX_FERR;
	}
	if (flags & MAX_UART_FL_RX_PAR) {
		status |= CAPI_UART_LINE_STAT_PARITY_ERROR;
		clear |= MAX_UART_FL_RX_PAR;
	}
	if (flags & MAX_UART_FL_RX_OV) {
		status |= CAPI_UART_LINE_STAT_OVERRUN_ERROR;
		clear |= MAX_UART_FL_RX_OV;
	}
	/* No Maxim UART exposes a break-detect flag. BREAK_IND is never reported. */

	if (clear)
		MXC_UART_ClearFlags(uart_priv->uart, clear);

	*status_flags = status;

	return 0;
}

/*
 * ---- Per-source interrupt masking and level queries -------------------------
 *
 * These sit on the interrupt-enable register through MAX_UART_INTEN() and on
 * the neutral MAX_UART_FL_* / MAX_UART_ST_* flag names, so one implementation
 * covers every register convention (§2.3). A part that has no decodable
 * interrupt-flag model at all (MAX_UART_HAS_INT_FLAGS == 0, e.g. the ES17
 * MAX32520) cannot express "which source", so the maskers report -ENOTSUP
 * rather than writing bits whose meaning is unknown. The level queries below
 * do not depend on the flag model -- they go through the MSDK FIFO/status
 * API -- so they stay available everywhere.
 *
 * Grouping: "err" is the three receive-side error sources, "rx" is data
 * arrival, "tx" is space/completion. A group whose flags all collapse to 0 on
 * this part writes nothing, which is the correct no-op.
 */
#define MAX_UART_IE_ERR		(MAX_UART_FL_RX_FERR | MAX_UART_FL_RX_PAR | \
				 MAX_UART_FL_RX_OV)
#define MAX_UART_IE_RX		(MAX_UART_FL_RX_THD | MAX_UART_FL_RX_FULL)
#define MAX_UART_IE_TX		(MAX_UART_FL_TX_THD | MAX_UART_FL_TX_HE | \
				 MAX_UART_FL_TX_OB)

#if MAX_UART_HAS_INT_FLAGS
/**
 * @brief Set or clear a group of bits in the interrupt-enable register.
 *
 * Read-modify-write: the three groups are masked independently by the caller
 * and must not clobber each other, and the async engine's own enables live in
 * the same register.
 */
static int _max_uart_set_irq_group(struct capi_uart_handle *handle,
				   uint32_t mask, bool enable)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv)
		return -EINVAL;

	uart_priv = handle->priv;

	if (enable)
		MAX_UART_INTEN(uart_priv->uart) |= mask;
	else
		MAX_UART_INTEN(uart_priv->uart) &= ~mask;

	return 0;
}
#else
static int _max_uart_set_irq_group(struct capi_uart_handle *handle,
				   uint32_t mask, bool enable)
{
	(void)mask;
	(void)enable;

	if (!handle || !handle->priv)
		return -EINVAL;

	return -ENOTSUP;
}
#endif

/**
 * @brief Mask/unmask the receive error sources (framing, parity, overrun).
 * @param handle The UART handle
 * @param enable true to unmask, false to mask
 * @return 0 on success, -EINVAL on a bad handle, -ENOTSUP where the part has
 *         no decodable interrupt-flag model
 */
int max_capi_uart_set_irq_err(struct capi_uart_handle *handle, bool enable)
{
	return _max_uart_set_irq_group(handle, MAX_UART_IE_ERR, enable);
}

/**
 * @brief Mask/unmask the receive data-available sources.
 * @param handle The UART handle
 * @param enable true to unmask, false to mask
 * @return 0 on success, -EINVAL on a bad handle, -ENOTSUP where unsupported
 */
int max_capi_uart_set_irq_rx(struct capi_uart_handle *handle, bool enable)
{
	return _max_uart_set_irq_group(handle, MAX_UART_IE_RX, enable);
}

/**
 * @brief Mask/unmask the transmit space/completion sources.
 * @param handle The UART handle
 * @param enable true to unmask, false to mask
 * @return 0 on success, -EINVAL on a bad handle, -ENOTSUP where unsupported
 */
int max_capi_uart_set_irq_tx(struct capi_uart_handle *handle, bool enable)
{
	return _max_uart_set_irq_group(handle, MAX_UART_IE_TX, enable);
}

/**
 * @brief Report whether the transmitter can accept at least one more byte.
 *
 * "Ready" is FIFO space, not idleness -- a partially drained FIFO is ready.
 * Asked of the FIFO through the MSDK API rather than of a threshold flag,
 * because the threshold level is configurable and would make the answer
 * depend on a setting the caller never chose.
 *
 * @param handle The UART handle
 * @param ready Set to true when the TX FIFO has room
 * @return 0 on success, -EINVAL on a bad handle or NULL out-pointer
 */
int max_capi_uart_irq_tx_ready(struct capi_uart_handle *handle, bool *ready)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !ready)
		return -EINVAL;

	uart_priv = handle->priv;
	*ready = (MXC_UART_GetTXFIFOAvailable(uart_priv->uart) > 0);

	return 0;
}

/**
 * @brief Report whether the transmitter has fully drained.
 *
 * Both conditions are required: the FIFO empty AND the shift register idle.
 * TX_EMPTY alone goes true while the last frame is still being clocked out, so
 * a caller using it to decide "safe to disable the UART" would truncate that
 * frame. Where the part spells no TX_BUSY the flag collapses to 0 and the test
 * degrades to FIFO-empty, which is the best that layout can answer.
 *
 * @param handle The UART handle
 * @param complete Set to true when the transmitter is idle
 * @return 0 on success, -EINVAL on a bad handle or NULL out-pointer
 */
int max_capi_uart_irq_tx_complete(struct capi_uart_handle *handle,
				  bool *complete)
{
	struct max_capi_uart_priv *uart_priv;
	uint32_t status;

	if (!handle || !handle->priv || !complete)
		return -EINVAL;

	uart_priv = handle->priv;
	/*
	 * Through the MSDK API, not uart->status: the register member is named
	 * `stat` on ME10/ME11 (MAX32650, MAX32660) and `status` elsewhere, and
	 * a struct member cannot be feature-probed by the preprocessor.
	 * MXC_UART_GetStatus() exists on every part and hides the difference,
	 * which is why the blocking TX path already uses it.
	 */
	status = MXC_UART_GetStatus(uart_priv->uart);

	*complete = ((status & MAX_UART_ST_TX_EMPTY) != 0U) &&
		    ((status & MAX_UART_ST_TX_BUSY) == 0U);

	return 0;
}

/**
 * @brief Report whether at least one received byte is waiting.
 * @param handle The UART handle
 * @param ready Set to true when the RX FIFO holds data
 * @return 0 on success, -EINVAL on a bad handle or NULL out-pointer
 */
int max_capi_uart_irq_rx_ready(struct capi_uart_handle *handle, bool *ready)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !ready)
		return -EINVAL;

	uart_priv = handle->priv;
	*ready = (MXC_UART_GetRXFIFOAvailable(uart_priv->uart) > 0);

	return 0;
}

/**
 * @brief Report whether any unmasked interrupt source is currently raised.
 *
 * Raised AND unmasked -- the raw flag register alone would report sources the
 * caller deliberately masked off. Same intersection get_interrupt_reason()
 * uses, so the two agree.
 *
 * @param handle The UART handle
 * @param pending Set to true when a live source is asserted
 * @return 0 on success, -EINVAL on a bad handle or NULL out-pointer,
 *         -ENOTSUP where the part has no interrupt-flag model
 */
int max_capi_uart_is_irq_pending(struct capi_uart_handle *handle, bool *pending)
{
#if MAX_UART_HAS_INT_FLAGS
	struct max_capi_uart_priv *uart_priv;

	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	uart_priv = handle->priv;
	*pending = ((MXC_UART_GetFlags(uart_priv->uart) &
		     MAX_UART_INTEN(uart_priv->uart)) != 0U);

	return 0;
#else
	if (!handle || !handle->priv || !pending)
		return -EINVAL;

	return -ENOTSUP;
#endif
}

/**
 * @brief Enable/disable FIFO - not supported. UART FIFOs are always on
 * @param handle The UART handle
 * @param enable Whether to enable or disable
 * @return -ENOSYS
 */
int max_capi_uart_enable_fifo(struct capi_uart_handle *handle, bool enable)
{
	return -ENOSYS;
}

/**
 * @brief Transmit 9-bit data - not supported. 9-bit mode is not supported.
 * @param handle The UART handle
 * @param data The data to transmit
 * @param is_address Whether it is an address or data byte
 * @return -ENOSYS
 */
int max_capi_uart_transmit_9bit(struct capi_uart_handle *handle, uint16_t data,
				bool is_address)
{
	return -ENOSYS;
}

/**
 * @brief Receive 9-bit data - not supported. 9-bit mode is not supported.
 * @param handle The UART handle
 * @param data Where to store the data
 * @param is_address Whether it is an address or data byte
 * @return -ENOSYS
 */
int max_capi_uart_receive_9bit(struct capi_uart_handle *handle, uint16_t *data,
			       bool *is_address)
{
	return -ENOSYS;
}

/**
 * @brief Set flow control state - not supported. Flow control is not supported.
 * @param handle The UART handle
 * @param rts_state RTS signal state
 * @param cts_state CTS signal state
 * @return -ENOSYS
 */
int max_capi_uart_set_flow_control_state(struct capi_uart_handle *handle,
		bool rts_state, bool cts_state)
{
	return -ENOSYS;
}

/**
 * @brief Get flow control state - not supported. Flow control is not supported.
 * @param handle The UART handle
 * @param rts_state Where to store the RTS signal state
 * @param cts_state Where to store the CTS signal state
 * @return -ENOSYS
 */
int max_capi_uart_get_flow_control_state(struct capi_uart_handle *handle,
		bool *rts_state, bool *cts_state)
{
	return -ENOSYS;
}

struct capi_uart_ops max_capi_uart_ops = {
	.init = max_capi_uart_init,
	.deinit = max_capi_uart_deinit,
	.transmit = max_capi_uart_transmit,
	.receive = max_capi_uart_receive,
	.transmit_async = max_capi_uart_transmit_async,
	.receive_async = max_capi_uart_receive_async,
	.register_callback = max_capi_uart_register_callback,
	.isr = max_capi_uart_isr,
	.set_line_config = max_capi_uart_set_line_config,
	.get_line_config = max_capi_uart_get_line_config,
	.flush_rx_fifo = max_capi_uart_flush_rx_fifo,
	.flush_tx_fifo = max_capi_uart_flush_tx_fifo,
	.get_rx_fifo_count = max_capi_uart_get_rx_fifo_count,
	.get_tx_fifo_count = max_capi_uart_get_tx_fifo_count,
	.get_interrupt_reason = max_capi_uart_get_interrupt_reason,
	.get_line_status = max_capi_uart_get_line_status,
	.set_irq_err = max_capi_uart_set_irq_err,
	.set_irq_rx = max_capi_uart_set_irq_rx,
	.set_irq_tx = max_capi_uart_set_irq_tx,
	.irq_tx_ready = max_capi_uart_irq_tx_ready,
	.irq_tx_complete = max_capi_uart_irq_tx_complete,
	.irq_rx_ready = max_capi_uart_irq_rx_ready,
	.is_irq_pending = max_capi_uart_is_irq_pending,
	/* Everything below cannot be implemented; returns -ENOSYS */
	.enable_fifo = max_capi_uart_enable_fifo,
	.transmit_9bit = max_capi_uart_transmit_9bit,
	.receive_9bit = max_capi_uart_receive_9bit,
	.set_flow_control_state = max_capi_uart_set_flow_control_state,
	.get_flow_control_state = max_capi_uart_get_flow_control_state,
};

/** Platform-specific functions ***********************************************/

int max_capi_uart_stdio_enable(struct capi_uart_handle *handle)
{
	struct max_capi_uart_priv *uart_priv;

	if (!handle)
		return -EINVAL;

	uart_priv = handle->priv;
	stdio_index = uart_priv->id;

	if (handle != uart[uart_priv->id])
		return -EINVAL;

	setvbuf(stdout, NULL, _IONBF, 0);

	return 0;
}

/** stdio Redirect implementation *********************************************/

#define STDIN_FILENO	0   /**> Definition of stdin */
#define STDOUT_FILENO   1   /**> Definition of stdout */
#define STDERR_FILENO   2   /**> Definition of stderr */

int _isatty(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 1;

	errno = EBADF;
	return 0;
}

int _write(int file, char *ptr, int len)
{
	int ret;

	if (stdio_index == -1)
		return -1;

	if (file == STDOUT_FILENO || file == STDERR_FILENO) {
		ret = max_capi_uart_transmit(uart[stdio_index], (uint8_t *)ptr, len);
		if (ret < 0) {

			errno = -ret;
			return -1;
		}

		return len;
	}
	errno = EBADF;
	return -1;
}

int _close(int file)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO)
		return 0;

	errno = EBADF;
	return -1;
}

int _lseek(int file, off_t offset, int whence)
{
	(void) file;
	(void) offset;
	(void) whence;

	errno = EBADF;
	return -1;
}

int _read(int file, char *ptr, int len)
{
	int ret;

	if (stdio_index == -1)
		return -1;

	if (file == STDIN_FILENO) {
		ret = max_capi_uart_receive(uart[stdio_index], (uint8_t *)ptr, 1);
		if (ret < 0) {
			errno = -ret;
			return -1;
		}

		/* receive() blocks until the byte lands and returns 0 on success;
		 * newlib expects the number of bytes read, so report the 1 read. */
		return 1;
	}
	errno = EBADF;
	return -1;
}

int _fstat(int file, struct stat *st)
{
	if (file >= STDIN_FILENO && file <= STDERR_FILENO) {
		st->st_mode = S_IFCHR;
		return 0;
	}

	errno = EBADF;
	return 0;
}
