/***************************************************************************//**
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
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
#ifndef __COMMON_DATA_H__
#define __COMMON_DATA_H__

extern struct no_os_timer_init_param timer_init_param;
extern struct no_os_irq_init_param timer_irq_ip;

extern struct no_os_gpio_init_param led_ctrl_init_param ;
extern struct no_os_gpio_init_param afe_ctrl_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q4_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q3_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q2_init_param;
extern struct no_os_gpio_init_param gate_ctrl_q1_init_param;

extern struct no_os_gpio_init_param gp3_init_param;

extern struct no_os_gpio_init_param data_ss_init_param;
extern struct no_os_spi_init_param data_spi_init_param;

extern struct no_os_gpio_init_param cfg_ss_init_param;
extern struct no_os_spi_init_param cfg_spi_init_param;

extern struct no_os_uart_init_param uart_init_param;

extern struct no_os_uart_init_param serial_log_init_param;

#endif /* __COMMON_DATA_H__ */

