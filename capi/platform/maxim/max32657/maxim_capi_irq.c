/***************************************************************************/ /*
*   @file   maxim_capi_irq.c
*   @brief  Implementation of IRQ functions with CAPI.
*   @author Ramon Miguel Imbao (ramonmiguel.imbao@analog.com)
********************************************************************************
* Copyright 2026(c) Analog Devices, Inc.
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
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

#include <string.h>
#include <errno.h>
#include "maxim_capi_irq.h"
#include "max32657.h"
#include "gpio.h"
#include "uart.h"
#include "rtc.h"
#include "tmr.h"
#include "dma.h"
#include "spi.h"
#include "wdt.h"

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
		return 0;

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
 * @return 0 on success, -ENODEV otherwise
 */
int max_capi_irq_deinit(void)
{
	uint32_t i;

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
 * @return 0
 */
int max_capi_irq_global_enable(void)
{
	__enable_irq();
	return 0;
}

/**
 * @brief Disable the IRQ globally
 * @return 0
 */
int max_capi_irq_global_disable(void)
{
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
		return -ENODEV;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	if (!irq_table[irq].enabled && !NVIC_GetEnableIRQ(irq))
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
		return -ENODEV;

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
	int ret;

	if (!irq_initialized)
		return -ENODEV;

	if (!_max_capi_irq_is_valid_irq(irq))
		return -EINVAL;

	irq_table[irq].callback = isr;
	irq_table[irq].arg = arg;

	switch (irq) {
	case RTC_IRQn:
		ret = MXC_RTC_EnableInt(MXC_RTC_INT_FL_LONG |
					MXC_RTC_INT_FL_SHORT |
					MXC_RTC_INT_FL_READY);
		if (ret)
			return -EBUSY;
		break;
	case TMR0_IRQn:
		MXC_TMR_EnableInt(MXC_TMR0);
		break;
	case TMR1_IRQn:
		MXC_TMR_EnableInt(MXC_TMR1);
		break;
	case TMR2_IRQn:
		MXC_TMR_EnableInt(MXC_TMR2);
		break;
	case TMR3_IRQn:
		MXC_TMR_EnableInt(MXC_TMR3);
		break;
	case TMR4_IRQn:
		MXC_TMR_EnableInt(MXC_TMR4);
		break;
	case TMR5_IRQn:
		MXC_TMR_EnableInt(MXC_TMR5);
		break;
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
		return -ENODEV;

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
		return -ENODEV;

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
		return -ENODEV;

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
		return -ENODEV;

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
		return -ENODEV;

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

/**
 * @brief Callback for DMA
 * @param ch DMA channel
 * @param reason Not used
 */
void max_capi_dma_callback(int ch, int reason)
{
	IRQn_Type irq;

	switch (ch) {
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
}

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

	if (!irq_initialized)
		return -ENODEV;

	if (!pin || !isr)
		return -EINVAL;

	if (pin->number < 0 || pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	config = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(pin->port_handle->identifier),
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
		return -ENODEV;

	if (!pin)
		return -EINVAL;

	if (pin->number < 0 || pin->number >= MXC_CFG_GPIO_PINS_PORT)
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
	if (!irq_initialized)
		return -ENODEV;

	if (!pin)
		return -EINVAL;

	if (pin->number < 0 || pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	MXC_GPIO_EnableInt(MXC_GPIO_GET_GPIO(pin->port_handle->identifier),
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
	if (!irq_initialized)
		return -ENODEV;

	if (!pin)
		return -EINVAL;

	if (pin->number < 0 || pin->number >= MXC_CFG_GPIO_PINS_PORT)
		return -EINVAL;

	MXC_GPIO_DisableInt(MXC_GPIO_GET_GPIO(pin->port_handle->identifier),
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

	if (!irq_initialized)
		return -ENODEV;

	if (!pin)
		return -EINVAL;

	if (pin->number < 0 || pin->number >= MXC_CFG_GPIO_PINS_PORT)
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

	config = (mxc_gpio_cfg_t) {
		.port = MXC_GPIO_GET_GPIO(pin->port_handle->identifier),
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

void UART_IRQHandler()
{
	if (irq_table[UART_IRQn].callback)
		irq_table[UART_IRQn].callback(irq_table[UART_IRQn].arg);
	else
		MXC_UART_AsyncHandler(MXC_UART);
}

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

void DMA1_CH0_IRQHandler()
{
	_dma_handler(MXC_DMA1_S, 0);
}

void DMA1_CH1_IRQHandler()
{
	_dma_handler(MXC_DMA1_S, 1);
}

void DMA1_CH2_IRQHandler()
{
	_dma_handler(MXC_DMA1_S, 2);
}

void DMA1_CH3_IRQHandler()
{
	_dma_handler(MXC_DMA1_S, 3);
}

void RTC_IRQHandler()
{
	if (irq_table[RTC_IRQn].callback)
		irq_table[RTC_IRQn].callback(irq_table[RTC_IRQn].arg);

	MXC_RTC_ClearFlags(MXC_RTC_INT_FL_LONG |
			   MXC_RTC_INT_FL_SHORT |
			   MXC_RTC_INT_FL_READY);
}

void _timer_handler(mxc_tmr_regs_t *tmr)
{
	uint32_t irq = MXC_TMR_GET_IRQ(MXC_TMR_GET_IDX(tmr));
	if (irq_table[irq].callback)
		irq_table[irq].callback(irq_table[irq].arg);

	MXC_TMR_ClearFlags(tmr);
}

#ifdef MXC_TMR0
void TMR0_IRQHandler()
{
	_timer_handler(MXC_TMR0);
}
#endif /* MXC_TMR0 */

#ifdef MXC_TMR1
void TMR1_IRQHandler()
{
	_timer_handler(MXC_TMR1);
}
#endif /* MXC_TMR1 */

#ifdef MXC_TMR2
void TMR2_IRQHandler()
{
	_timer_handler(MXC_TMR2);
}
#endif /* MXC_TMR2 */

#ifdef MXC_TMR3
void TMR3_IRQHandler()
{
	_timer_handler(MXC_TMR3);
}
#endif /* MXC_TMR3 */

#ifdef MXC_TMR4
void TMR4_IRQHandler()
{
	_timer_handler(MXC_TMR4);
}
#endif /* MXC_TMR4 */

#ifdef MXC_TMR5
void TMR5_IRQHandler()
{
	_timer_handler(MXC_TMR5);
}
#endif /* MXC_TMR5 */

void GPIO0_IRQHandler()
{
	MXC_GPIO_Handler(0);
}

void SPI_IRQHandler()
{
	if (irq_table[SPI_IRQn].callback)
		irq_table[SPI_IRQn].callback(irq_table[SPI_IRQn].arg);
	else
		MXC_SPI_ClearFlags(MXC_SPI_GET_SPI(0));
}

void WDT_IRQHandler()
{
	if (MXC_WDT->ctrl & (MXC_F_WDT_CTRL_CLKRDY_IE | MXC_F_WDT_CTRL_CLKRDY))
		MXC_WDT->ctrl &= ~MXC_F_WDT_CTRL_CLKRDY_IE;

	MXC_WDT_ClearIntFlag(MXC_WDT);
	MXC_WDT_ClearResetFlag(MXC_WDT);

	if (irq_table[WDT_IRQn].callback)
		irq_table[WDT_IRQn].callback(irq_table[WDT_IRQn].arg);
}

void TRNG_IRQHandler()
{
	if (irq_table[TRNG_IRQn].callback)
		irq_table[TRNG_IRQn].callback(irq_table[TRNG_IRQn].arg);
}

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
