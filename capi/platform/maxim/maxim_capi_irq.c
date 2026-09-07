/***************************************************************************/ /*
 *   @file   maxim_capi_irq.c
 *   @brief  Implementation of IRQ functions with CAPI.
 *   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

/*
 * Common Maxim CAPI IRQ backend.
 *
 * One file for every MSDK target. The NVIC core (init/enable/connect/priority/
 * status) is pure CMSIS and portable across all parts. The per-peripheral pieces
 * that differ between targets are isolated to two places:
 *
 *   1. max_capi_irq_connect() - the switch that auto-enables RTC/timer sources
 *      is gated per instance macro (MXC_RTC, MXC_TMRn).
 *   2. The weak *_IRQHandler vector block - generated per instance and gated on
 *      the MSDK instance macros (MXC_UARTn, MXC_SPIn, MXC_TMRn, MXC_GPIOn, ...).
 *      Single-instance parts that name their vector without an index
 *      (MAX32657: UART_IRQHandler / SPI_IRQHandler) take a dedicated branch.
 *
 * Verified against the RevB register/vector layout (incl. MAX32657). RevA/RevC
 * timer/WDT field differences are handled by the same instance-macro gating; add
 * target branches as those parts are brought up.
 */

#include <string.h>
#include <errno.h>
#include "mxc_device.h"
#include "maxim_capi_gpio.h"
#include "maxim_capi_gpio_priv.h"
#include "maxim_capi_irq.h"
#include "maxim_capi_dma.h"
#include "gpio.h"
#include "uart.h"
#ifdef MXC_RTC
#include "rtc.h"
#endif
#include "tmr.h"
#include "dma.h"
#include "spi.h"
#if defined(MXC_I2C0) || defined(MXC_I2C)
#include "i2c.h"
#endif
#include "wdt.h"
#ifdef MXC_I3C
#include "i3c.h"
#endif

/** Static variables **********************************************************/

static bool irq_initialized = false;
static struct max_capi_irq_entry irq_table[MXC_IRQ_COUNT];
static struct max_capi_irq_entry gpio_irq_table[MXC_CFG_GPIO_PINS_PORT];
static uint32_t default_priority = 0;

/** Helper functions ***********************************************************/

/**
 * @brief Check if IRQ is valid
 * @param irq - the IRQ to check
 * @return true if valid, false if not
 */
static inline bool _max_capi_irq_is_valid_irq(uint32_t irq)
{
	return irq < MXC_IRQ_COUNT;
}

/**
 * @brief Check if IRQ is GPIO0 port interrupt
 * @param irq - the IRQ to check
 * @return true if GPIO0_IRQn, false if not
 */
static inline bool _max_capi_irq_is_gpio_irq(uint32_t irq)
{
	return irq == (uint32_t)GPIO0_IRQn;
}

/**
 * @brief Report whether an IRQ line is currently enabled in the NVIC.
 * @param irq - the IRQ to query
 * @return true if the NVIC ISER bit is set, false otherwise
 *
 * NVIC_GetEnableIRQ() is absent from the older CMSIS core header some MSDK
 * parts still resolve to (e.g. MAX32690, whose max32690.h notes symbols
 * "missing from CMSIS/Core/Include/core_cm4.h"), so read the ISER bit directly
 * -- the same expression modern CMSIS uses internally, valid on every Cortex-M.
 */
static inline bool _max_capi_irq_hw_enabled(uint32_t irq)
{
	return (NVIC->ISER[irq >> 5UL] & (1UL << (irq & 0x1FUL))) != 0UL;
}

/** Function implementations **************************************************/

/**
 * @brief Initialize the IRQ
 * @param config - IRQ configuration struct
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_init(struct capi_irq_config *config)
{
	const struct max_capi_irq_extra_config *extra;

	if (!config)
		return -EINVAL;

	if (irq_initialized)
		return -EBUSY;

	/* Clear callback tables */
	memset(irq_table, 0, sizeof(irq_table));
	memset(gpio_irq_table, 0, sizeof(gpio_irq_table));

	if (config->extra) {
		extra = config->extra;
		default_priority = extra->default_priority;
	}

	irq_initialized = true;

	return 0;
}

/**
 * @brief Deinitialize the IRQ
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_deinit(void)
{
	uint32_t i;

	if (!irq_initialized)
		return -EINVAL;

	for (i = 0; i < MXC_IRQ_COUNT; i++) {
		if (irq_table[i].enabled) {
			NVIC_DisableIRQ((IRQn_Type)i);
			irq_table[i].enabled = false;
		}
		irq_table[i].callback = NULL;
		irq_table[i].arg = NULL;
	}

	for (i = 0; i < MXC_CFG_GPIO_PINS_PORT; i++) {
		gpio_irq_table[i].callback = NULL;
		gpio_irq_table[i].arg = NULL;
	}

	irq_initialized = false;

	return 0;
}

/**
 * @brief Enable the IRQ globally
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_global_enable(void)
{
	if (!irq_initialized)
		return -EINVAL;

	__enable_irq();
	return 0;
}

/**
 * @brief Disable the IRQ globally
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_global_disable(void)
{
	if (!irq_initialized)
		return -EINVAL;

	__disable_irq();
	return 0;
}

/**
 * @brief Enable an IRQ
 * @param irq - the IRQ number to enable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_enable(uint32_t irq)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	if (!irq_table[irq].enabled && !_max_capi_irq_hw_enabled(irq))
		NVIC_SetPriority(irq, default_priority);

	NVIC_EnableIRQ(irq);
	irq_table[irq].enabled = true;

	return 0;
}

/**
 * @brief Disable an IRQ
 * @param irq - the IRQ to disable
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_disable(uint32_t irq)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	NVIC_DisableIRQ(irq);
	irq_table[irq].enabled = false;

	return 0;
}

/**
 * @brief Connect an IRQ to a callback. Does not automatically enable it.
 * @param irq - the IRQ to connect to
 * @param isr - the callback function to connect to the IRQ
 * @param arg - the arguments to pass to the callback function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!isr)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	irq_table[irq].callback = isr;
	irq_table[irq].arg = arg;

	/*
	 * Some sources need their peripheral-level interrupt enabled here so the
	 * weak handler below actually fires. Gated per instance so the switch only
	 * references vectors/instances the target actually has.
	 */
	switch (irq) {
#ifdef MXC_RTC
	case RTC_IRQn: {
		int ret = MXC_RTC_EnableInt(MXC_RTC_INT_FL_LONG |
					    MXC_RTC_INT_FL_SHORT |
					    MXC_RTC_INT_FL_READY);
		if (ret)
			return -EBUSY;
		break;
	}
#endif /* MXC_RTC */
		/*
		 * MXC_TMR_EnableInt() does not exist on ME10 (MAX32650) and ME14 (MAX32665):
		 * their TMR block has no interrupt-enable field at all (tmr_regs.h offers only
		 * MXC_F_TMR_INTR_IRQ, a write-1-to-clear flag), so the timer interrupt is always
		 * asserted at the peripheral and is maskable only at the NVIC -- which
		 * capi_irq_enable() already does. A no-op here is the correct behaviour, not a
		 * stub: there is no peripheral-level enable to perform.
		 */
		/*
		 * MXC_TMR_EnableInt() is DECLARED in every part's tmr.h but not DEFINED by
		 * every rev's driver, so a call links only where the implementation exists --
		 * the header is not a usable feature test here, and neither is a #ifdef.
		 *
		 * Derived by checking which TMR/tmr_<rev>.c actually define it and mapping the
		 * revs back to parts through the MSDK <part>_files.mk:
		 *   es17 -> 32520, me10 -> 32650, me11 -> 32660,
		 *   me13 -> 32570, me14 -> 32665, me55 -> 32572
		 * On those the timer's interrupt-enable is a no-op; the NVIC line is still
		 * connected and enabled by the caller, so the vector fires. Re-derive this list
		 * on an MSDK bump rather than assuming it is stable.
		 */
#if (TARGET_NUM == 32520) || (TARGET_NUM == 32570) || \
   (TARGET_NUM == 32650) || (TARGET_NUM == 32660) || \
   (TARGET_NUM == 32665) || (TARGET_NUM == 32572)
#define MAX_CAPI_TMR_ENABLE_INT(t)	((void)(t))
#else
#define MAX_CAPI_TMR_ENABLE_INT(t)	MXC_TMR_EnableInt(t)
#endif

#ifdef MXC_TMR0
	case TMR0_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR0);
		break;
#endif /* MXC_TMR0 */
#ifdef MXC_TMR1
	case TMR1_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR1);
		break;
#endif /* MXC_TMR1 */
#ifdef MXC_TMR2
	case TMR2_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR2);
		break;
#endif /* MXC_TMR2 */
#ifdef MXC_TMR3
	case TMR3_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR3);
		break;
#endif /* MXC_TMR3 */
#ifdef MXC_TMR4
	case TMR4_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR4);
		break;
#endif /* MXC_TMR4 */
#ifdef MXC_TMR5
	case TMR5_IRQn:
		MAX_CAPI_TMR_ENABLE_INT(MXC_TMR5);
		break;
#endif /* MXC_TMR5 */
	default:
		break;
	}

	return 0;
}

/**
 * @brief Clear a pending IRQ
 * @param irq - the IRQ to clear
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_clear_pending(uint32_t irq)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	NVIC_ClearPendingIRQ(irq);

	return 0;
}

/**
 * @brief Get the status of an IRQ
 * @param irq - the IRQ
 * @param pactive - pointer to where the status will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq) || !pactive)
		return -EINVAL;

	*pactive = NVIC_GetPendingIRQ(irq) || NVIC_GetActive(irq);

	return 0;
}

/**
 * @brief Set the priority of an IRQ
 * @param irq - the IRQ
 * @param priority - the priority
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	NVIC_SetPriority(irq, priority);

	return 0;
}

/**
 * @brief Get the priority of an IRQ
 * @param irq - the IRQ
 * @param priority - pointer to where the priority will be stored
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	*priority = NVIC_GetPriority(irq);

	return 0;
}

/**
 * @brief Set the level/edge trigger for all pins on the GPIO IRQ. If other IRQs
 * 	  are specified, the function will return -ENOTSUP.
 * @param irq - the IRQ
 * @param trigger - the level/edge trigger
 * @return 0 on success, negative error code otherwise
 */
int max_capi_irq_set_level_edge_trigger(uint32_t irq,
					enum capi_irq_trig_level trigger)
{
	mxc_gpio_cfg_t config;
	int32_t trig;

	if (!irq_initialized)
		return -EINVAL;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	if (!_max_capi_irq_is_gpio_irq(irq))
		return -ENOTSUP;

	switch (trigger) {
	case CAPI_IRQ_EDGE_RISING:
		trig = MXC_GPIO_INT_RISING;
		break;
	case CAPI_IRQ_EDGE_FALLING:
		trig = MXC_GPIO_INT_FALLING;
		break;
	case CAPI_IRQ_EDGE_BOTH:
		trig = MXC_GPIO_INT_BOTH;
		break;
	case CAPI_IRQ_LEVEL_LOW:
		trig = MXC_GPIO_INT_LOW;
		break;
	case CAPI_IRQ_LEVEL_HIGH:
		trig = MXC_GPIO_INT_HIGH;
		break;
	default:
		return -EINVAL;
	}

	config = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(0),
		/**
		 * Let's set the trigger for all the pins for now since there is
		 * no mechanism to select the pin with capi_irq_set_level_edge_trigger
		 **/
		.mask = 0xFFFFFFFF,
		.func = MXC_GPIO_FUNC_IN,
	};
	MXC_GPIO_IntConfig(&config, trig);

	return 0;
}

/** Platform-specific functions ***********************************************/

#if defined(MXC_DMA)
/**
 * @brief Callback for DMA
 * @param ch DMA channel
 * @param reason Not used
 *
 * Maps an MSDK channel to its per-channel NVIC line and runs the registered CAPI
 * callback. MXC_DMA_CH_GET_IRQ takes the controller on the dual-secure part and
 * just the channel on single-controller parts (see maxim_capi_dma.h).
 */
void max_capi_dma_callback(int ch, int reason)
{
	IRQn_Type irq;

	(void)reason;

	if (ch < 0 || ch >= MXC_DMA_CHANNELS)
		return;

#if defined(MXC_DMA1_S)
	irq = MXC_DMA_CH_GET_IRQ(MXC_DMA1_S, ch);
#else
	irq = MXC_DMA_CH_GET_IRQ(ch);
#endif

	if (irq_table[irq].callback)
		irq_table[irq].callback(irq_table[irq].arg);
}
#endif /* MXC_DMA */

/**
 * @brief Connect a GPIO pin to a callback
 * @param pin - the GPIO pin
 * @param isr - the callback function to connect to the IRQ
 * @param arg - the arguments to pass to the callback function
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_irq_connect(struct capi_gpio_pin *pin,
			      capi_isr_callback_t isr, void *arg)
{
	mxc_gpio_cfg_t config;
	struct max_capi_gpio_port_priv *gpio_priv;

	if (!irq_initialized)
		return -EINVAL;

	if (!pin || !pin->port_handle || !pin->port_handle->priv || !isr)
		return -EINVAL;

	if (pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	gpio_priv = pin->port_handle->priv;

	config = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(gpio_priv->id),
		.mask = (1U << pin->number),
	};

	gpio_irq_table[pin->number].callback = isr;
	gpio_irq_table[pin->number].arg = arg;

	MXC_GPIO_RegisterCallback(&config, gpio_irq_table[pin->number].callback,
				  gpio_irq_table[pin->number].arg);

	return 0;
}

/**
 * @brief Disconnect a GPIO from a callback
 * @param pin - the GPIO pin
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_irq_disconnect(const struct capi_gpio_pin *pin)
{
	if (!irq_initialized)
		return -EINVAL;

	if (!pin)
		return -EINVAL;

	if (pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	if (gpio_irq_table[pin->number].enabled)
		gpio_irq_table[pin->number].enabled = false;

	gpio_irq_table[pin->number].callback = NULL;
	gpio_irq_table[pin->number].arg = NULL;

	return 0;
}

/**
 * @brief Enable an interrupt on a specific pin
 * @param pin - the GPIO pin
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_irq_enable(struct capi_gpio_pin *pin)
{
	struct max_capi_gpio_port_priv *gpio_priv;

	if (!irq_initialized)
		return -EINVAL;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	if (pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	gpio_priv = pin->port_handle->priv;

	MXC_GPIO_EnableInt(MXC_GPIO_GET_GPIO(gpio_priv->id),
			   (1U << pin->number));
	gpio_irq_table[pin->number].enabled = true;

	return 0;
}

/**
 * @brief Disable an interrupt on a specific pin
 * @param pin - the GPIO pin
 * @return 0 on success, negative error code oherwise
 */
int max_capi_gpio_irq_disable(struct capi_gpio_pin *pin)
{
	struct max_capi_gpio_port_priv *gpio_priv;

	if (!irq_initialized)
		return -EINVAL;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	if (pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	gpio_priv = pin->port_handle->priv;

	MXC_GPIO_DisableInt(MXC_GPIO_GET_GPIO(gpio_priv->id),
			    (1U << pin->number));
	gpio_irq_table[pin->number].enabled = false;

	return 0;
}

/**
 * @brief Set level/edge trigger for a specific pin
 * @param pin - the GPIO pin
 * @param trigger  - the trigger to set the pin to
 * @return 0 on success, negative error code otherwise
 */
int max_capi_gpio_irq_set_level_edge_trigger(struct capi_gpio_pin *pin,
		enum capi_irq_trig_level trigger)
{
	mxc_gpio_cfg_t config;
	mxc_gpio_int_pol_t trig;
	struct max_capi_gpio_port_priv *gpio_priv;

	if (!irq_initialized)
		return -EINVAL;

	if (!pin || !pin->port_handle || !pin->port_handle->priv)
		return -EINVAL;

	if (pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	switch (trigger) {
	case CAPI_IRQ_LEVEL_LOW:
		trig = MXC_GPIO_INT_LOW;
		break;
	case CAPI_IRQ_LEVEL_HIGH:
		trig = MXC_GPIO_INT_HIGH;
		break;
	case CAPI_IRQ_EDGE_FALLING:
		trig = MXC_GPIO_INT_FALLING;
		break;
	case CAPI_IRQ_EDGE_RISING:
		trig = MXC_GPIO_INT_RISING;
		break;
	case CAPI_IRQ_EDGE_BOTH:
		trig = MXC_GPIO_INT_BOTH;
		break;
	default:
		return -EINVAL;
	}

	gpio_priv = pin->port_handle->priv;

	config = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(gpio_priv->id),
		.mask = (1U << pin->number),
	};

	MXC_GPIO_IntConfig(&config, trig);

	return 0;
}

/**
 * @brief Enable interrupts on all pins
 * @return 0
 */
int max_capi_gpio_irq_global_enable(void)
{
	MXC_GPIO_EnableInt(MXC_GPIO_GET_GPIO(0), 0xFFFFFFFF);

	return 0;
}

/**
 * @brief Disable interrupts on all pins
 * @return 0
 */
int max_capi_gpio_irq_global_disable(void)
{
	MXC_GPIO_DisableInt(MXC_GPIO_GET_GPIO(0), 0xFFFFFFFF);

	return 0;
}

/** Peripheral IRQ Handlers ***************************************************/

/*
 * The vectors below are weak-overridden here. Each routes to the CAPI callback
 * registered in irq_table[], falling back to the MSDK async/clear handler when
 * no CAPI callback is bound. Handlers are generated per instance and gated on
 * the target's MSDK instance macros; single-instance parts that name the vector
 * without an index take the "unnumbered" branch.
 */

/* --- UART -------------------------------------------------------------- */
#if defined(MXC_UART) && !defined(MXC_UART0)
/* Single unnumbered UART instance (e.g. MAX32657). */
void UART_IRQHandler(void)
{
	if (irq_table[UART_IRQn].callback)
		irq_table[UART_IRQn].callback(irq_table[UART_IRQn].arg);
	else
		MXC_UART_AsyncHandler(MXC_UART);
}
#else
#define MAX_CAPI_DEFINE_UART_HANDLER(n)					\
	void UART##n##_IRQHandler(void)					\
	{								\
		uint32_t irq = (uint32_t)MXC_UART_GET_IRQ(n);		\
		if (irq_table[irq].callback)				\
			irq_table[irq].callback(irq_table[irq].arg);	\
		else							\
			MXC_UART_AsyncHandler(MXC_UART_GET_UART(n));	\
	}
#ifdef MXC_UART0
MAX_CAPI_DEFINE_UART_HANDLER(0)
#endif
#ifdef MXC_UART1
MAX_CAPI_DEFINE_UART_HANDLER(1)
#endif
#ifdef MXC_UART2
MAX_CAPI_DEFINE_UART_HANDLER(2)
#endif
#ifdef MXC_UART3
MAX_CAPI_DEFINE_UART_HANDLER(3)
#endif
#endif /* UART */

/* --- DMA --------------------------------------------------------------- */
/*
 * Two controller families (see maxim_capi_dma.h). Each per-channel vector runs
 * the CAPI callback registered in irq_table[<channel IRQn>] first, then
 * MXC_DMA_Handler() (which clears the flags) -- the DMA backend relies on that
 * ordering and therefore never calls MXC_DMA_SetCallback().
 *
 *   Dual-controller secure (MAX32657): DMA1_CHn vectors; Handler(controller).
 *   Single-controller (MAX32655/MAX32690): DMAn vectors; Handler(void). The ARM
 *     vector table names one handler per channel (DMA0..DMA3 on the 32655,
 *     DMA0..DMA15 on the 32690), so the X-macro emits exactly MXC_DMA_CHANNELS
 *     of them; MXC_DMA_CH_GET_IRQ(n) resolves channel n to DMAn_IRQn.
 */
#if defined(MXC_DMA1_S)
void _dma_handler(mxc_dma_regs_t *dma, uint32_t channel)
{
	IRQn_Type irq;

	switch (channel) {
	case 0:
		irq = DMA1_CH0_IRQn;
		break;
	case 1:
		irq = DMA1_CH1_IRQn;
		break;
	case 2:
		irq = DMA1_CH2_IRQn;
		break;
	case 3:
		irq = DMA1_CH3_IRQn;
		break;
	default:
		return;
	}

	if (irq_table[irq].callback)
		irq_table[irq].callback(irq_table[irq].arg);

	MXC_DMA_Handler(dma);
}

void DMA1_CH0_IRQHandler(void)
{
	_dma_handler(MXC_DMA1_S, 0);
}

void DMA1_CH1_IRQHandler(void)
{
	_dma_handler(MXC_DMA1_S, 1);
}

void DMA1_CH2_IRQHandler(void)
{
	_dma_handler(MXC_DMA1_S, 2);
}

void DMA1_CH3_IRQHandler(void)
{
	_dma_handler(MXC_DMA1_S, 3);
}
#elif defined(MXC_DMA)
void _dma_handler(uint32_t channel)
{
	IRQn_Type irq;

	if (channel >= MXC_DMA_CHANNELS)
		return;

#if MAX_CAPI_DMA_INT_INST_ARG
	irq = MXC_DMA_CH_GET_IRQ(MAX_CAPI_DMA_REGS, channel);
#else
	irq = MXC_DMA_CH_GET_IRQ(channel);
#endif

	if (irq_table[irq].callback)
		irq_table[irq].callback(irq_table[irq].arg);

#if MAX_CAPI_DMA_INST_ARG
	MXC_DMA_Handler(MAX_CAPI_DMA_REGS);
#else
	MXC_DMA_Handler();
#endif
}

#define MAX_CAPI_DEFINE_DMA_HANDLER(n)			\
	void DMA##n##_IRQHandler(void)			\
	{						\
		_dma_handler(n);			\
	}

MAX_CAPI_DEFINE_DMA_HANDLER(0)
MAX_CAPI_DEFINE_DMA_HANDLER(1)
MAX_CAPI_DEFINE_DMA_HANDLER(2)
MAX_CAPI_DEFINE_DMA_HANDLER(3)
#if MXC_DMA_CHANNELS > 4
MAX_CAPI_DEFINE_DMA_HANDLER(4)
MAX_CAPI_DEFINE_DMA_HANDLER(5)
MAX_CAPI_DEFINE_DMA_HANDLER(6)
MAX_CAPI_DEFINE_DMA_HANDLER(7)
MAX_CAPI_DEFINE_DMA_HANDLER(8)
MAX_CAPI_DEFINE_DMA_HANDLER(9)
MAX_CAPI_DEFINE_DMA_HANDLER(10)
MAX_CAPI_DEFINE_DMA_HANDLER(11)
MAX_CAPI_DEFINE_DMA_HANDLER(12)
MAX_CAPI_DEFINE_DMA_HANDLER(13)
MAX_CAPI_DEFINE_DMA_HANDLER(14)
MAX_CAPI_DEFINE_DMA_HANDLER(15)
#endif /* MXC_DMA_CHANNELS > 4 */
#endif /* DMA family */

/* --- RTC --------------------------------------------------------------- */
#ifdef MXC_RTC
void RTC_IRQHandler(void)
{
	if (irq_table[RTC_IRQn].callback)
		irq_table[RTC_IRQn].callback(irq_table[RTC_IRQn].arg);

	MXC_RTC_ClearFlags(MXC_RTC_INT_FL_LONG |
			   MXC_RTC_INT_FL_SHORT |
			   MXC_RTC_INT_FL_READY);
}
#endif /* MXC_RTC */

/* --- Timers ------------------------------------------------------------ */
void _timer_handler(mxc_tmr_regs_t *tmr)
{
	uint32_t irq = MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(tmr));
	if (irq_table[irq].callback)
		irq_table[irq].callback(irq_table[irq].arg);

	MXC_TMR_ClearFlags(tmr);
}

#ifdef MXC_TMR0
void TMR0_IRQHandler(void)
{
	_timer_handler(MXC_TMR0);
}
#endif /* MXC_TMR0 */

#ifdef MXC_TMR1
void TMR1_IRQHandler(void)
{
	_timer_handler(MXC_TMR1);
}
#endif /* MXC_TMR1 */

#ifdef MXC_TMR2
void TMR2_IRQHandler(void)
{
	_timer_handler(MXC_TMR2);
}
#endif /* MXC_TMR2 */

#ifdef MXC_TMR3
void TMR3_IRQHandler(void)
{
	_timer_handler(MXC_TMR3);
}
#endif /* MXC_TMR3 */

#ifdef MXC_TMR4
void TMR4_IRQHandler(void)
{
	_timer_handler(MXC_TMR4);
}
#endif /* MXC_TMR4 */

#ifdef MXC_TMR5
void TMR5_IRQHandler(void)
{
	_timer_handler(MXC_TMR5);
}
#endif /* MXC_TMR5 */

/* --- GPIO -------------------------------------------------------------- */
/*
 * Port 0 exists on every Maxim target. Higher-numbered ports are generated for
 * multi-port parts. The per-pin callback table (gpio_irq_table) is still
 * port-0 scoped - multi-port pin routing is a GPIO-module follow-up.
 *
 * The table index is taken from MXC_GPIO_GET_IRQ(n), the MSDK-portable
 * port->IRQn accessor, rather than the literal GPIOn_IRQn enum: a port can be
 * instantiated (MXC_GPIOn defined) without a dedicated vector. MAX32655, for
 * example, has MXC_GPIO3 but routes GPIO3 through the shared GPIOWAKE vector, so
 * GPIO3_IRQn does not exist. Keying on the enum literal fails to compile there;
 * MXC_GPIO_GET_IRQ(3) resolves to that part's actual (shared) IRQn. On parts
 * that lack a standalone GPIO3 vector this handler is simply never dispatched.
 */
void GPIO0_IRQHandler(void)
{
	MXC_GPIO_Handler(0);

	if (irq_table[MXC_GPIO_GET_IRQ(0)].callback)
		irq_table[MXC_GPIO_GET_IRQ(0)].callback(
			irq_table[MXC_GPIO_GET_IRQ(0)].arg);
}

#ifdef MXC_GPIO1
void GPIO1_IRQHandler(void)
{
	MXC_GPIO_Handler(1);

	if (irq_table[MXC_GPIO_GET_IRQ(1)].callback)
		irq_table[MXC_GPIO_GET_IRQ(1)].callback(
			irq_table[MXC_GPIO_GET_IRQ(1)].arg);
}
#endif /* MXC_GPIO1 */

#ifdef MXC_GPIO2
void GPIO2_IRQHandler(void)
{
	MXC_GPIO_Handler(2);

	if (irq_table[MXC_GPIO_GET_IRQ(2)].callback)
		irq_table[MXC_GPIO_GET_IRQ(2)].callback(
			irq_table[MXC_GPIO_GET_IRQ(2)].arg);
}
#endif /* MXC_GPIO2 */

#ifdef MXC_GPIO3
void GPIO3_IRQHandler(void)
{
	MXC_GPIO_Handler(3);

	if (irq_table[MXC_GPIO_GET_IRQ(3)].callback)
		irq_table[MXC_GPIO_GET_IRQ(3)].callback(
			irq_table[MXC_GPIO_GET_IRQ(3)].arg);
}
#endif /* MXC_GPIO3 */

/* --- SPI --------------------------------------------------------------- */
#if defined(MXC_SPI) && !defined(MXC_SPI0)
/* Single unnumbered SPI instance (e.g. MAX32657). */
void SPI_IRQHandler(void)
{
	if (irq_table[SPI_IRQn].callback)
		irq_table[SPI_IRQn].callback(irq_table[SPI_IRQn].arg);
	else
		MXC_SPI_ClearFlags(MXC_SPI_GET_SPI(0));
}
#else
/*
 * Key on the literal SPI<n>_IRQn, not MXC_SPI_GET_IRQ(n). Across all 15 supported
 * parts MXC_SPI<n> and SPI<n>_IRQn come and go together, so the literal is always
 * declared wherever this macro is instantiated -- while the MSDK accessor is
 * absent on MAX32660 and, on MAX32675, expands to a ternary naming an SPI3_IRQn
 * that its own part header never declares.
 */
#define MAX_CAPI_DEFINE_SPI_HANDLER(n)					\
	void SPI##n##_IRQHandler(void)					\
	{								\
		uint32_t irq = (uint32_t)SPI##n##_IRQn;			\
		if (irq_table[irq].callback)				\
			irq_table[irq].callback(irq_table[irq].arg);	\
		else							\
			MXC_SPI_ClearFlags(MXC_SPI_GET_SPI(n));		\
	}
#ifdef MXC_SPI0
MAX_CAPI_DEFINE_SPI_HANDLER(0)
#endif
#ifdef MXC_SPI1
MAX_CAPI_DEFINE_SPI_HANDLER(1)
#endif
#ifdef MXC_SPI2
MAX_CAPI_DEFINE_SPI_HANDLER(2)
#endif
#ifdef MXC_SPI3
MAX_CAPI_DEFINE_SPI_HANDLER(3)
#endif
#ifdef MXC_SPI4
MAX_CAPI_DEFINE_SPI_HANDLER(4)
#endif
#endif /* SPI */

/* --- I2C --------------------------------------------------------------- */
#if defined(MXC_I2C) && !defined(MXC_I2C0)
/* Single unnumbered I2C instance. */
void I2C_IRQHandler(void)
{
	if (irq_table[I2C_IRQn].callback)
		irq_table[I2C_IRQn].callback(irq_table[I2C_IRQn].arg);
	else
		MXC_I2C_ClearFlags(MXC_I2C_GET_I2C(0), 0xFFFFFFFF, 0xFFFFFFFF);
}
#else
/*
 * Key on the literal I2C<n>_IRQn, matching the SPI rationale above: MXC_I2C<n>
 * and I2C<n>_IRQn are declared together on every supported part, whereas the
 * MSDK accessor can expand to a ternary naming an instance the part header does
 * not declare.
 */
#define MAX_CAPI_DEFINE_I2C_HANDLER(n)					\
	void I2C##n##_IRQHandler(void)					\
	{								\
		uint32_t irq = (uint32_t)I2C##n##_IRQn;			\
		if (irq_table[irq].callback)				\
			irq_table[irq].callback(irq_table[irq].arg);	\
		else							\
			MXC_I2C_ClearFlags(MXC_I2C_GET_I2C(n),		\
					   0xFFFFFFFF, 0xFFFFFFFF);	\
	}
#ifdef MXC_I2C0
MAX_CAPI_DEFINE_I2C_HANDLER(0)
#endif
#ifdef MXC_I2C1
MAX_CAPI_DEFINE_I2C_HANDLER(1)
#endif
#ifdef MXC_I2C2
MAX_CAPI_DEFINE_I2C_HANDLER(2)
#endif
#ifdef MXC_I2C3
MAX_CAPI_DEFINE_I2C_HANDLER(3)
#endif
#endif /* I2C */

/* --- Watchdog ---------------------------------------------------------- */
/*
 * Watchdog vector naming splits by part. Most parts number the instance
 * (WDT0_IRQn / WDT0_IRQHandler, e.g. MAX32655, MAX32690 -- the latter also
 * aliases MXC_WDT -> MXC_WDT0 and adds a second LP watchdog, WDT1), while a few
 * single-watchdog parts use the un-numbered WDT_IRQn / WDT_IRQHandler (e.g.
 * MAX32657, MAX32662). Gate each handler on the register-instance macro that
 * actually exists so the emitted symbol matches that part's startup vector
 * table -- keying on the bare MXC_WDT alias would reference WDT_IRQn on parts
 * whose only vector is WDT0_IRQn and fail to link.
 */
/*
 * Clear the interrupt/reset flags with direct register writes (the exact bit
 * ops MXC_WDT_ClearIntFlag/ClearResetFlag perform) rather than calling the WDT
 * peripheral driver: this selftest links only the drivers it exercises, and no
 * build here pulls in wdt_*.c, so referencing those helpers would be an
 * undefined symbol at link. All parts with a watchdog expose these ctrl-flag
 * field macros.
 */
/*
 * Two WDT ctrl-register generations ship, and they share no flag spelling:
 *
 *   windowed (MAX32655/32657/32662/32670/32672/32675/32680/32690/78000/78002):
 *     CLKRDY_IE + INT_EARLY/INT_LATE/RST_EARLY/RST_LATE.
 *   legacy   (MAX32520/32570/32650/32660/32665): a single INT_FLAG/RST_FLAG
 *     pair and no clock-ready interrupt at all.
 *
 * Probe on CLKRDY_IE -- a field macro, so the preprocessor can see it -- and
 * clear whichever flags that generation actually has.
 */
#ifdef MXC_F_WDT_CTRL_CLKRDY_IE
#define MAX_CAPI_WDT_CLEAR(regs)					\
	do {								\
		if ((regs)->ctrl &					\
		    (MXC_F_WDT_CTRL_CLKRDY_IE | MXC_F_WDT_CTRL_CLKRDY))	\
			(regs)->ctrl &= ~MXC_F_WDT_CTRL_CLKRDY_IE;	\
		(regs)->ctrl &= ~(MXC_F_WDT_CTRL_INT_LATE |		\
				  MXC_F_WDT_CTRL_INT_EARLY);		\
		(regs)->ctrl &= ~(MXC_F_WDT_CTRL_RST_LATE |		\
				  MXC_F_WDT_CTRL_RST_EARLY);		\
	} while (0)
#else
#define MAX_CAPI_WDT_CLEAR(regs)					\
	do {								\
		(regs)->ctrl &= ~(MXC_F_WDT_CTRL_INT_FLAG |		\
				  MXC_F_WDT_CTRL_RST_FLAG);		\
	} while (0)
#endif

#define MAX_CAPI_DEFINE_WDT_HANDLER(handler, regs, irqn)		\
	void handler(void)						\
	{								\
		MAX_CAPI_WDT_CLEAR(regs);				\
									\
		if (irq_table[irqn].callback)				\
			irq_table[irqn].callback(irq_table[irqn].arg);	\
	}

/*
 * MAX32662 defines BOTH MXC_WDT and MXC_WDT0 yet its vector table names only the
 * un-numbered WDT_IRQn -- so the register-instance macro cannot decide this one
 * and TARGET_NUM must. IRQn_Type members are enum constants, invisible to the
 * preprocessor, so there is nothing else to probe.
 */
#if (TARGET_NUM == 32662)
MAX_CAPI_DEFINE_WDT_HANDLER(WDT_IRQHandler, MXC_WDT0, WDT_IRQn)
#elif defined(MXC_WDT0)
MAX_CAPI_DEFINE_WDT_HANDLER(WDT0_IRQHandler, MXC_WDT0, WDT0_IRQn)
#ifdef MXC_WDT1
MAX_CAPI_DEFINE_WDT_HANDLER(WDT1_IRQHandler, MXC_WDT1, WDT1_IRQn)
#endif
#ifdef MXC_WDT2
MAX_CAPI_DEFINE_WDT_HANDLER(WDT2_IRQHandler, MXC_WDT2, WDT2_IRQn)
#endif
#elif defined(MXC_WDT)
MAX_CAPI_DEFINE_WDT_HANDLER(WDT_IRQHandler, MXC_WDT, WDT_IRQn)
#endif

/* --- TRNG -------------------------------------------------------------- */
#ifdef MXC_TRNG
void TRNG_IRQHandler(void)
{
	if (irq_table[TRNG_IRQn].callback)
		irq_table[TRNG_IRQn].callback(irq_table[TRNG_IRQn].arg);
}
#endif /* MXC_TRNG */

/* --- I3C (MAX32657) ---------------------------------------------------- */
#ifdef MXC_I3C
void I3C_IRQHandler(void)
{
	if (irq_table[I3C_IRQn].callback)
		irq_table[I3C_IRQn].callback(irq_table[I3C_IRQn].arg);
}
#endif /* MXC_I3C */

/** CAPI functions implementation **********************************************/

int capi_irq_init(struct capi_irq_config *config)
{
	return max_capi_irq_init(config);
}

int capi_irq_deinit(void)
{
	return max_capi_irq_deinit();
}

int capi_irq_global_enable(void)
{
	return max_capi_irq_global_enable();
}

int capi_irq_global_disable(void)
{
	return max_capi_irq_global_disable();
}

int capi_irq_enable(uint32_t irq)
{
	return max_capi_irq_enable(irq);
}

int capi_irq_disable(uint32_t irq)
{
	return max_capi_irq_disable(irq);
}

int capi_irq_connect(uint32_t irq, capi_isr_callback_t isr, void *arg)
{
	return max_capi_irq_connect(irq, isr, arg);
}

int capi_irq_clear_pending(uint32_t irq)
{
	return max_capi_irq_clear_pending(irq);
}

int capi_irq_get_status(uint32_t irq, uint32_t *pactive)
{
	return max_capi_irq_get_status(irq, pactive);
}

int capi_irq_set_priority(uint32_t irq, uint32_t priority)
{
	return max_capi_irq_set_priority(irq, priority);
}

int capi_irq_get_priority(uint32_t irq, uint32_t *priority)
{
	return max_capi_irq_get_priority(irq, priority);
}

int capi_irq_set_level_edge_trigger(uint32_t irq,
				    enum capi_irq_trig_level trigger)
{
	return max_capi_irq_set_level_edge_trigger(irq, trigger);
}
