/***************************************************************************//**
 *   @file   imu_single_core_example.c
 *   @brief  Single-core dual-IMU example for the MAX78000.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Both IMUs share SPI0 on the ARM Cortex-M4F (no RISC-V involvement):
 *   ADXL345  — accelerometer,  CS = GPIO P0.19 (active-low)
 *   ADIS16577-2 — 6-axis IMU, CS = GPIO P0.11 (active-low)
 *
 * Pin assignment (MAX78000FTHR):
 *   SCLK P0.7 | MOSI P0.5 | MISO P0.6 | ADXL CS P0.19 | ADIS CS P0.11
 *
 * The ADIS gyro rate is integrated into roll/pitch/yaw angles. Both sensors
 * are read in a tight loop and printed at ~10 Hz over UART0.
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

#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include "no_os_uart.h"
#include "maxim_gpio.h"
#include "maxim_spi.h"
#include "adxl345.h"
#include "adis.h"
#include "adis1657x.h"
#include "common_data.h"
#include "mxc_device.h"
#include "gcr_regs.h"
#include "lpgcr_regs.h"
#include "mxc_sys.h"

/*
 * MXC_SYS_Reset_Periph() in sys_ai85.c writes GCR->rst0/rst1 as a plain
 * single-bit assignment, clearing every other bit in the register. For rst1
 * this silently resets SMPHR (bit 16) and SIMO (bit 25). Although no hardware
 * semaphore is in use here, resetting SIMO mid-transaction can glitch the
 * supply rail and cause SPI corruption. The --wrap linker flag redirects all
 * call sites to this read-modify-write replacement.
 */
void __wrap_MXC_SYS_Reset_Periph(mxc_sys_reset_t reset)
{
	uint32_t bit;

	if (reset > 63) {
		bit = reset - 64;
		MXC_LPGCR->rst |= (1u << bit);
		while (MXC_LPGCR->rst & (1u << bit)) {}
	} else if (reset > 31) {
		bit = reset - 32;
		MXC_GCR->rst1 |= (1u << bit);
		while (MXC_GCR->rst1 & (1u << bit)) {}
	} else {
		bit = (uint32_t)reset;
		MXC_GCR->rst0 |= (1u << bit);
		while (MXC_GCR->rst0 & (1u << bit)) {}
	}
}

/* ------------------------------------------------------------------------- */
/* ADXL345 (accelerometer) on SPI0, chip-select = GPIO P0.19                */
/* ------------------------------------------------------------------------- */

static struct max_gpio_init_param gpio_cs_extra = {
	.vssel = MXC_GPIO_VSSEL_VDDIOH,
};

static struct no_os_gpio_init_param adxl_cs_ip = {
	.port = 0,
	.number = 19,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_cs_extra,
};

static struct no_os_gpio_init_param adis_cs_ip = {
	.port = 0,
	.number = 11,
	.pull = NO_OS_PULL_NONE,
	.platform_ops = &max_gpio_ops,
	.extra = &gpio_cs_extra,
};

/*
 * The MAX SPI controller still requires a native chip-select index when a
 * GPIO-backed CS is configured. Use SS2 so ADXL transfers do not disturb ADIS
 * on P0.11.
 */
static struct max_spi_init_param adxl_spi_extra = {
	.num_slaves = 3,
	.polarity   = SPI_SS_POL_LOW,
	.vssel      = MXC_GPIO_VSSEL_VDDIOH,
	.gpio_cs    = &adxl_cs_ip,
};

static struct adxl345_init_param adxl_ip = {
	.communication_type = ADXL345_SPI_COMM,
	.dev_type           = ID_ADXL345,
	.spi_init = {
		.device_id    = 1,   /* device_id=1 -> MXC_SPI0 (P0.5/6/7) */
		.max_speed_hz = 500000,
		.chip_select  = 2,
		.mode         = NO_OS_SPI_MODE_3,
		.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
		.platform_ops = &max_spi_ops,
		.extra        = &adxl_spi_extra,
	},
};

/* ------------------------------------------------------------------------- */
/* ADIS16577-2 (6-axis IMU) on SPI0, chip-select = GPIO P0.11              */
/* ------------------------------------------------------------------------- */

/*
 * The ADIS driver expects CS to toggle per 16-bit word. Use GPIO P0.11 so the
 * shared bus does not depend on native SS state.
 *
 * ADIS16577-2 SPI is mode 3. We stay at 500 kHz for safety with a shared,
 * relatively long bus.
 */
static struct max_spi_init_param adis_spi_extra = {
	.num_slaves = 2,
	.polarity   = SPI_SS_POL_LOW,
	.vssel      = MXC_GPIO_VSSEL_VDDIOH,
	.gpio_cs    = &adis_cs_ip,
};

static struct no_os_spi_init_param adis_spi_ip = {
	.device_id    = 1,   /* device_id=1 -> MXC_SPI0 (P0.5/6/7) */
	.max_speed_hz = 500000,
	.chip_select  = 1,
	.mode         = NO_OS_SPI_MODE_3,
	.bit_order    = NO_OS_SPI_BIT_ORDER_MSB_FIRST,
	.platform_ops = &max_spi_ops,
	.extra        = &adis_spi_extra,
};

static struct adis_init_param adis_ip = {
	.info       = &adis1657x_chip_info,
	.gpio_reset = NULL,  /* no reset pin -> software reset inside adis_init */
	.sync_mode  = ADIS_SYNC_OUTPUT,
	.dev_id     = ADIS16577_2,
};

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

/*
 * Gyro milli-degree divisor.
 * ADIS16577-2 gyro returns 32-bit values with sensitivity (40<<16) LSB per
 * (deg/s) at the 2 kHz internal ODR. Over N sample intervals:
 *   milli_deg = accum / ((40<<16) * 2000 / 1000) = accum / 5242880.
 */
#define ADIS_GYRO_MDEG_DIV  5242880LL

/*
 * ADIS 32-bit accel LSB -> milli-g.
 * Scale is 1 / (800<<16) g/LSB => 52,428,800 LSB/g => 52,428.8 LSB/mg.
 * Use 52429 as integer approximation (< 0.001% error).
 */
#define ADIS_ACCL_LSB_PER_MG  52429LL

/* Print a signed milli-unit value as a fixed-point decimal string. */
static void print_milli(const char *label, int32_t milli)
{
	int32_t whole = milli / 1000;
	int32_t frac  = milli % 1000;
	if (frac < 0)
		frac = -frac;
	if (milli < 0 && whole == 0)
		printf("%s-0.%03ld", label, (long)frac);
	else
		printf("%s%ld.%03ld", label, (long)whole, (long)frac);
}

/* ------------------------------------------------------------------------- */
/* Main                                                                      */
/* ------------------------------------------------------------------------- */

int imu_single_core_example_main(void)
{
	struct no_os_uart_desc *uart_desc;
	struct adxl345_dev *adxl_dev;
	struct adis_dev *adis_dev;
	int ret;

	MXC_SYS_Clock_Select(MXC_SYS_CLOCK_IPO);
	SystemCoreClockUpdate();

	ret = no_os_uart_init(&uart_desc, &uart_ip);
	if (ret)
		return ret;
	no_os_uart_stdio(uart_desc);

	printf("\r\n");
	printf("MAX78000 Single-Core Dual-IMU Example\r\n");
	printf("  ADXL345  : SPI0, CS=P0.19 (GPIO)\r\n");
	printf("  ADIS16577-2: SPI0, CS=P0.11 (GPIO)\r\n");

	/* Initialize both devices before entering the sampling loop. */
	ret = adxl345_init(&adxl_dev, adxl_ip);
	if (ret) {
		printf("[ERR] ADXL345 init failed (%d) -- check power/CS/wiring\r\n", ret);
		return ret;
	}
	adxl345_set_power_mode(adxl_dev, 1);
	printf("[OK]  ADXL345 initialized\r\n");

	/*
	 * adis_init() performs a software reset followed by self-test and diagnostics
	 * reads. ADIS register accesses require CS to toggle for every 16-bit word.
	 */
	adis_ip.spi_init = &adis_spi_ip;
	ret = adis_init(&adis_dev, &adis_ip);
	if (ret) {
		printf("[ERR] ADIS16577-2 init failed (%d) -- check wiring\r\n", ret);
		return ret;
	}

	{
		uint32_t prod_id = 0, diag = 0;
		adis_read_prod_id(adis_dev, &prod_id);
		adis_read_reg(adis_dev, 0x02, &diag, 2);
		printf("[OK]  ADIS16577-2 initialized: PROD_ID=%lu DIAG=0x%04lx\r\n",
		       (unsigned long)prod_id, (unsigned long)diag);
	}

	printf("\r\n[Streaming: ADIS roll/pitch/yaw + accel, ADXL accel, ~10 Hz]\r\n\r\n");

	int64_t accum[3]    = {0, 0, 0};
	uint64_t total_samp = 0;
	uint32_t prev_dcntr = 0;
	int have_prev       = 0;
	uint32_t print_div  = 0;

	for (;;) {
		int32_t gx = 0, gy = 0, gz = 0;
		int32_t adis_ax = 0, adis_ay = 0, adis_az = 0;
		uint32_t dc = 0;
		uint16_t delta;
		float adxl_gx = 0.0f, adxl_gy = 0.0f, adxl_gz = 0.0f;

		/* Read all ADIS channels (gyro + accel + data counter). */
		ret  = adis_read_x_gyro(adis_dev, &gx);
		ret |= adis_read_y_gyro(adis_dev, &gy);
		ret |= adis_read_z_gyro(adis_dev, &gz);
		ret |= adis_read_x_accl(adis_dev, &adis_ax);
		ret |= adis_read_y_accl(adis_dev, &adis_ay);
		ret |= adis_read_z_accl(adis_dev, &adis_az);
		ret |= adis_read_data_cntr(adis_dev, &dc);
		if (ret)
			continue; /* transient SPI error; retry */

		if (!have_prev) {
			prev_dcntr = dc;
			have_prev = 1;
		}
		/* Samples elapsed since last read, wrapping mod 2^16. */
		delta = (uint16_t)((uint16_t)dc - (uint16_t)prev_dcntr);
		prev_dcntr = dc;

		if (delta) {
			accum[0] += (int64_t)gx * delta;
			accum[1] += (int64_t)gy * delta;
			accum[2] += (int64_t)gz * delta;
			total_samp += delta;
		}

		/* Read the ADXL345 on the same SPI0 bus using its GPIO CS. */
		adxl345_get_g_xyz(adxl_dev, &adxl_gx, &adxl_gy, &adxl_gz);

		/* Decimate console output to ~10 Hz (2 kHz ODR / 200 samples). */
		if (++print_div < 200)
			continue;
		print_div = 0;

		int32_t roll_md  = (int32_t)(accum[0] / ADIS_GYRO_MDEG_DIV);
		int32_t pitch_md = (int32_t)(accum[1] / ADIS_GYRO_MDEG_DIV);
		int32_t yaw_md   = (int32_t)(accum[2] / ADIS_GYRO_MDEG_DIV);

		int32_t a_mgx = (int32_t)((int64_t)adis_ax / ADIS_ACCL_LSB_PER_MG);
		int32_t a_mgy = (int32_t)((int64_t)adis_ay / ADIS_ACCL_LSB_PER_MG);
		int32_t a_mgz = (int32_t)((int64_t)adis_az / ADIS_ACCL_LSB_PER_MG);

		int32_t px_mg = (int32_t)(adxl_gx * 1000.0f);
		int32_t py_mg = (int32_t)(adxl_gy * 1000.0f);
		int32_t pz_mg = (int32_t)(adxl_gz * 1000.0f);

		print_milli("[IMU] rpy=", roll_md);
		print_milli(",", pitch_md);
		print_milli(",", yaw_md);
		print_milli(" deg  adis_a=", a_mgx);
		print_milli(",", a_mgy);
		print_milli(",", a_mgz);
		print_milli(" g  adxl=", px_mg);
		print_milli(",", py_mg);
		print_milli(",", pz_mg);
		printf(" g (n=%lu)\r\n", (unsigned long)total_samp);
	}

	return 0;
}
