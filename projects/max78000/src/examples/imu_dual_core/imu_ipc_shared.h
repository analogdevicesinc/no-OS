/***************************************************************************//**
 *   @file   imu_ipc_shared.h
 *   @brief  Shared cross-core layout for the MAX78000 dual-core IMU example.
 *   @author Victor Pascu (victor.pascu@analog.com)
 *
 * Defines the shared-memory contract between the ARM Cortex-M4F (CPU0) and the
 * RISC-V RV32 (CPU1) for the dual-sensor example, plus the freestanding SPI
 * bus-lock helpers used by BOTH cores.
 *
 * Roles:
 *  - RISC-V (CPU1): polls an ADIS16577-2 IMU on the shared SPI pins using a
 *    local bit-banged no-OS SPI backend + adis driver, publishes raw gyro/accel/
 *    data-counter samples, and notifies the ARM.
 *  - ARM (CPU0): reads an ADXL345 on the same shared SPI pins, integrates the RISC-V's
 *    raw ADIS gyro samples into roll/pitch/yaw angles, and logs both sensors.
 *
 * Both sensors share the P0.5/P0.6/P0.7 SPI pins. Because the RISC-V is rv32imc
 * (no atomic extension) a software lock in SRAM is unsafe, so the two cores
 * serialize every SPI transaction with a MAX78000 hardware semaphore: reading
 * MXC_SEMA->semaphores[n] is an atomic test-and-set that returns the previous
 * value (0 => the caller just acquired it); writing 0 releases. This is the same
 * SEMA block used for the doorbells, on a distinct, otherwise-unused index.
 *
 * This header is included by BOTH ARM and RISC-V translation units and uses only
 * C99 stdint types plus the SEMA register block, for portability across the two
 * toolchains.
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

#ifndef _IMU_IPC_SHARED_H_
#define _IMU_IPC_SHARED_H_

#include <stdint.h>
#include "mxc_device.h"
#include "sema_regs.h"

/**
 * @def IMU_SHARED_ADDR
 * Fixed SRAM2 address of the shared table, reached by both cores through an
 * identical fixed-address pointer (no linker section maps here). It sits in the
 * gap in the RISC-V's SRAM2 map: above the RV .data/.bss (which end at
 * ~0x20010480) and well below the RV stack (limit 0x2001B000). It must NOT be in
 * SRAM3: when the RISC-V runs in place from flash, SRAM3 is inaccessible to it as
 * data. This is the same address the doorbell example uses.
 *
 * NOTE: this address lies inside the RV newlib heap range
 * (__HeapBase..__StackLimit). That is safe only because the coprocessor's heap
 * use is tiny and one-time -- adis_init allocates its adis_dev + SPI descriptor
 * (< 1 KB), ending well below 0x20011000. If the RV firmware ever allocates more
 * than ~2 KB, move this table or raise __HeapBase in the RV linker script.
 */
#define IMU_SHARED_ADDR   0x20011000UL

/**
 * @def IMU_SHARED_MAGIC
 * Validity marker written by the RISC-V once the shared table is initialized.
 */
#define IMU_SHARED_MAGIC  0xAD152016UL

/**
 * @def IMU_SPI_SEMA
 * MXC_SEMA->semaphores[] index used as the SPI bus lock. Distinct from the
 * doorbell path (irq0/irq1 + mail0/mail1), which does not touch the semaphore
 * array, so there is no collision.
 */
#define IMU_SPI_SEMA     3

/*
 * Shared bit-banged SPI pins, matching the known-good single-core wiring
 * (imu_single_core_example.c). Each CS is owned and driven by exactly one core
 * -- ADIS by the RISC-V, ADXL by the ARM -- so there is no CS contention and no
 * swap workaround is needed.
 */
#define IMU_SPI_SCLK_GPIO 7u
#define IMU_SPI_MISO_GPIO 6u
#define IMU_SPI_MOSI_GPIO 5u
#define IMU_ADIS_CS_GPIO  11u
#define IMU_ADXL_CS_GPIO  19u

/*
 * RISC-V ADIS poll period, in microseconds. Matched to the ADIS 2 kHz internal
 * ODR (~500 us/sample): reading faster only re-reads the same sample, and -- more
 * importantly -- back-to-back bus-locked reads with no idle gap starve the ARM,
 * which shares the bus through the same (unfair) hardware-semaphore lock. This
 * delay is taken OUTSIDE the lock, giving the ARM a window to read the ADXL and
 * print. Raise it to hand the ARM more bus time; lower it toward 0 only if the
 * ARM does not need the bus.
 */
#define IMU_RV_LOOP_PERIOD_US 500u

/*
 * Consecutive RISC-V publishes with no DATA_CNTR change after which the ARM warns
 * the ADIS sample counter is stuck. Sized above the per-print publish count so a
 * transient hiccup does not trip it.
 */
#define IMU_DCNTR_STUCK_SAMPLES 400u

/**
 * RISC-V lifecycle milestones, published (monotonically) in imu_shared_t.state so
 * the ARM can gate its own bring-up on the RISC-V's progress. These are ordered
 * so the ARM can wait with `state < IMU_STATE_x`. Whether the ADIS is the right
 * part is reported separately via imu_shared_t.prod_id (compared by the ARM), not
 * encoded in the state, to avoid racing the IMU_DETECTED -> RUNNING transition.
 */
#define IMU_STATE_BOOT         0u  /* RISC-V started, nothing done yet */
#define IMU_STATE_SPI_OK       1u  /* SPI peripheral configured */
#define IMU_STATE_IMU_DETECTED 2u  /* ADIS PROD_ID/DIAG read into the table */
#define IMU_STATE_RUNNING      3u  /* integration loop live */
#define IMU_STATE_ERROR        0x80u /* fatal init failure (SPI/ADIS, ret in fault_info) */
#define IMU_STATE_FAULT        0x81u /* CPU trap/exception (see fault_* fields) */

/** Expected ADIS16577 product id (decimal, as reported by register 0x72). */
#define IMU_ADIS_PROD_ID       16577u

/**
 * @def IMU_FAULT_MAGIC
 * Written to imu_shared_t.fault_magic by the RISC-V trap handler so the ARM can
 * distinguish a genuine fault report from zeroed/garbage memory before trusting
 * the fault_cause / fault_epc fields.
 */
#define IMU_FAULT_MAGIC        0xDEADFA17UL

/**
 * @brief Bring-up stage tag written to imu_shared_t.stage before each risky step,
 * so a fault or hang can be localized to the last stage the RISC-V entered even if
 * it never returns. Distinct from `state` (which only advances on success).
 */
#define IMU_STAGE_START        0u
#define IMU_STAGE_SPI_INIT     1u  /* inside no_os_spi_init */
#define IMU_STAGE_ADIS_SPI     2u  /* inside ADIS no_os_spi_init */
#define IMU_STAGE_ADIS_RESET_CMD 3u  /* ADIS software reset SPI write */
#define IMU_STAGE_ADIS_RESET_WAIT 4u /* ADIS software reset delay */
#define IMU_STAGE_ADIS_TEST    5u  /* ADIS self-test command/delay */
#define IMU_STAGE_ADIS_DIAG    6u  /* reading ADIS diagnostic status */
#define IMU_STAGE_ADIS_SYNC    7u  /* writing ADIS sync mode */
#define IMU_STAGE_READ_ID      8u  /* reading PROD_ID/DIAG */
#define IMU_STAGE_LOOP         9u  /* steady-state read loop */
#define IMU_STAGE_LOOP_READ_ERROR 30u /* steady-state ADIS read failed */

/* Debug trace event codes for code-flow inspection */
#define IMU_TRACE_MAIN_ENTER   0x0001
#define IMU_TRACE_SHARED_INIT  0x0010
#define IMU_TRACE_SPI_LOCK     0x0101
#define IMU_TRACE_SPI_UNLOCK   0x0102
#define IMU_TRACE_ADIS_LOCK    0x0111
#define IMU_TRACE_ADIS_UNLOCK  0x0112
#define IMU_TRACE_LOOP_READ    0x0201
#define IMU_TRACE_LOOP_PUBLISH 0x0210
#define IMU_TRACE_FAULT        0xF000

/**
 * @struct imu_shared_t
 * @brief Cross-core payload in shared SRAM2.
 *
 * The RISC-V writes lifecycle, fault, and raw ADIS sample fields. The ARM reads
 * those raw samples and owns gyro integration, keeping the math in one place.
 *
 * Ordering: the RISC-V updates the payload then bumps `seq` and rings the host
 * doorbell; the ARM waits the doorbell, then reads a coherent snapshot (both
 * cores are data-cacheless, so volatile access + the doorbell handshake are
 * sufficient — see the dual_core example notes on why the RISC-V issues no fence).
 */
typedef struct {
	volatile uint32_t magic;     /* IMU_SHARED_MAGIC once initialized */
	volatile uint32_t state;     /* IMU_STATE_* lifecycle / error code */
	volatile uint32_t prod_id;   /* ADIS PROD_ID read at bring-up */
	volatile uint32_t diag;      /* ADIS DIAG_STAT read at bring-up */
	volatile uint32_t seq;       /* incremented on every 100 Hz publish */
	volatile uint32_t host_ready; /* ARM set after ADXL init; releases RV polling */
	volatile uint32_t data_cntr; /* latest ADIS data counter */
	volatile int32_t  gyro_x;    /* latest ADIS X gyro, raw 32-bit counts */
	volatile int32_t  gyro_y;    /* latest ADIS Y gyro, raw 32-bit counts */
	volatile int32_t  gyro_z;    /* latest ADIS Z gyro, raw 32-bit counts */
	volatile int32_t  accel_x;   /* latest ADIS accel X, raw 32-bit counts */
	volatile int32_t  accel_y;   /* latest ADIS accel Y, raw 32-bit counts */
	volatile int32_t  accel_z;   /* latest ADIS accel Z, raw 32-bit counts */

	/* --- Fault / progress reporting (RISC-V -> ARM) ---------------------- */
	volatile uint32_t stage;       /* IMU_STAGE_* : last risky step entered */
	volatile int32_t  init_ret;    /* return code of the failed init call */
	volatile uint32_t fault_magic; /* IMU_FAULT_MAGIC when a trap was captured */
	volatile uint32_t fault_cause; /* RISC-V mcause of the trap */
	volatile uint32_t fault_epc;   /* RISC-V mepc (faulting instruction addr) */
	volatile uint32_t fault_tval;  /* RISC-V mtval (bad address/instruction) */

	/* --- Debug trace buffer (RISC-V -> ARM) ------------------------------ */
	volatile uint32_t trace_idx;   /* next write position in trace_log */
	volatile uint32_t trace_log[16]; /* circular trace buffer (up to 16 events) */
} imu_shared_t;

static inline void imu_trace_event(uint32_t event_code)
{
	imu_shared_t *shared = (imu_shared_t *)IMU_SHARED_ADDR;
	uint32_t idx = shared->trace_idx & 0xF;
	shared->trace_log[idx] = event_code;
	shared->trace_idx++;
}

/**
 * @brief Try to acquire the SPI bus lock (non-blocking).
 * Reading the semaphore register is an atomic test-and-set that returns the
 * previous value; a previous value of 0 means this core just took the lock.
 * @return 1 if the lock was acquired, 0 if it is held by the other core.
 */
static inline int imu_spi_trylock(void)
{
	return (MXC_SEMA->semaphores[IMU_SPI_SEMA] == 0) ? 1 : 0;
}

/**
 * @brief Acquire the SPI bus lock, spinning until it is free.
 * Transactions hold the lock only for the microseconds of a single transfer, so
 * the spin is short and neither core starves.
 */
static inline void imu_spi_lock(void)
{
	while (!imu_spi_trylock())
		;
}

/**
 * @brief Release the SPI bus lock. Only the current holder should call this.
 */
static inline void imu_spi_unlock(void)
{
	MXC_SEMA->semaphores[IMU_SPI_SEMA] = 0;
}

#endif /* _IMU_IPC_SHARED_H_ */
