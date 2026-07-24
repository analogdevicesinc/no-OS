/***************************************************************************//**
 *   @file   maxim_riscv_compat.h
 *   @brief  RISC-V compatibility definitions for MAX32690 dual-core support.
 *   @author Carlos Jones Jr (carlosjr.jones@analog.com)
 *
 * This header provides compatibility definitions for building no-OS code on the
 * MAX32690 RISC-V core. It addresses SDK differences between ARM and RISC-V builds.
 *
 ******************************************************************************
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

#ifndef __MAXIM_RISCV_COMPAT_H__
#define __MAXIM_RISCV_COMPAT_H__

#ifdef __riscv

/*
 * RISC-V IPC doorbell IRQ number.
 * On MAX32690 RISC-V, the ARM CM4 doorbell is at vector offset 16,
 * but the SDK doesn't define SEMA_IRQn for RISC-V. It's defined as
 * CM4_IRQn in the RISC-V startup vector (startup_riscv_max32690.S).
 *
 * From RISC-V vector table:
 *   Offset 16: CM4_IRQHandler (ARM-to-RISC-V doorbell)
 */
#ifndef SEMA_IRQn
/* Check if CM4_IRQn is defined first */
#ifdef CM4_IRQn
#define SEMA_IRQn CM4_IRQn
#else
/* Fallback: define it directly if CM4_IRQn doesn't exist */
#define CM4_IRQn  16
#define SEMA_IRQn CM4_IRQn
#endif
#endif

/* I2C register compatibility (SDK i2c_me15.c uses different names for RISC-V) */
#ifndef MXC_F_I2C_SLAVE_ADDR
#define MXC_F_I2C_SLAVE_ADDR MXC_F_I2C_REVA_SLAVE_ADDR
#endif

#ifndef MXC_F_I2C_SLAVE_EXT_ADDR_EN
#define MXC_F_I2C_SLAVE_EXT_ADDR_EN MXC_F_I2C_REVA_SLAVE_EXT_ADDR_EN
#endif

/* Helper macro to access i2c->slave0 (RISC-V) vs i2c->slave (ARM) */
#define I2C_SLAVE_REG(i2c)  ((i2c)->slave0)

#else /* ARM core */

#define I2C_SLAVE_REG(i2c)  ((i2c)->slave)

#endif /* __riscv */

#endif /* __MAXIM_RISCV_COMPAT_H__ */
