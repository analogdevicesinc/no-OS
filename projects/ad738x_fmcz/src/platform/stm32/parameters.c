/***************************************************************************//**
 *   @file   parameters.c
 *   @brief  Definition of STM32 platform data used by eval-ad738x project.
 *   @author Axel Haslam (ahaslam@baylibre.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 * Copyright 2024(c) BayLibre, SAS.
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

#include "parameters.h"

#include "stm32_dma.h"
#include "no_os_pwm.h"
extern DMA_HandleTypeDef hdma_tim8_ch1;
extern DMA_HandleTypeDef hdma_spi1_rx;

struct stm32_uart_init_param uart_extra_ip = {
	.huart = &huart5,
};

struct no_os_dma_init_param dma_init = {
	.id = 0,
	.num_ch = 2,
	.platform_ops = &stm32_dma_ops,
};

struct stm32_dma_channel txdma_channel = {
	.hdma = &hdma_tim8_ch1,
	.ch_num = DMA_CHANNEL_7,
	.mem_increment = false,
	.mem_data_alignment = DATA_ALIGN_BYTE,
	.per_data_alignment = DATA_ALIGN_BYTE,
	.dma_mode = DMA_CIRCULAR_MODE
};

struct stm32_dma_channel rxdma_channel = {
	.hdma = &hdma_spi1_rx,
	.ch_num = DMA_CHANNEL_3,
	.mem_increment = true,
	.mem_data_alignment = DATA_ALIGN_BYTE,
	.per_data_alignment = DATA_ALIGN_BYTE,
	.dma_mode = DMA_NORMAL_MODE,
};

/* TRIGGER PWM */
struct stm32_pwm_init_param trigger_pwm_extra_init_params = {
	.htimer = &htim1,
	.pwm_timer = STM32_PWM_TIMER_TIM,
	.prescaler = 1,
	.timer_autoreload = true,
	.mode = TIM_OC_PWM1,
	.timer_chn = 3,
	.complementary_channel = false,
	.get_timer_clock = HAL_RCC_GetPCLK2Freq,
	.clock_divider = 2,
	.trigger_output = PWM_TRGO_UPDATE,
};

/* CS PWM */
struct stm32_pwm_init_param cs_pwm_extra_init_params = {
	.htimer = &htim2,
	.pwm_timer = STM32_PWM_TIMER_TIM,
	.prescaler = 1,
	.timer_autoreload = true,
	.mode = TIM_OC_PWM1,
	.timer_chn = 1,
	.complementary_channel = false,
	.get_timer_clock = HAL_RCC_GetPCLK1Freq,
	.clock_divider = 2,
	.onepulse_enable = true,
	.trigger_enable = true,
	.trigger_source = PWM_TS_ITR0,
	.trigger_output = PWM_TRGO_ENABLE,
};

struct no_os_pwm_init_param cs_pwm_init = {
	.id = 2,
	/* time to xfer bytes */
	.period_ns = CS_PWM_PERIOD_NS,
	.duty_cycle_ns = CS_PWM_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = &stm32_pwm_ops,
	.extra = &cs_pwm_extra_init_params
};

/* TX PWM */
struct stm32_pwm_init_param tx_pwm_extra_init_params = {
	.htimer = &htim8,
	.pwm_timer = STM32_PWM_TIMER_TIM,
	.prescaler = 1,
	.timer_autoreload = true,
	.mode = TIM_OC_PWM1,
	.timer_chn = 1,
	.complementary_channel = false,
	.get_timer_clock = HAL_RCC_GetPCLK2Freq,
	.clock_divider = 2,
	.onepulse_enable = true,
	.trigger_enable = true,
	.trigger_source = PWM_TS_ITR0,
	.dma_enable = true,
	.repetitions = TX_PWM_REPS,
};

struct no_os_pwm_init_param tx_pwm_init = {
	.id = 8,
	/* time to xfer 8 bits */
	.period_ns = TX_PWM_PERIOD_NS,
	.duty_cycle_ns = TX_PWM_DUTY_NS,
	.polarity = NO_OS_PWM_POLARITY_HIGH,
	.platform_ops = PWM_OPS,
	.extra = &tx_pwm_extra_init_params
};
struct stm32_spi_init_param spi_extra_ip  = {
	.chip_select_port = SPI_CS_PORT,
	.get_input_clock = HAL_RCC_GetPCLK2Freq,
	.dma_init = &dma_init,
	.txdma_ch = &txdma_channel,
	.rxdma_ch = &rxdma_channel,
	.irq_num = DMA2_Stream0_IRQn,
	.pwm_init = &cs_pwm_init,
	.tx_pwm_init = &tx_pwm_init,
	.alternate = GPIO_AF1_TIM1,
};
