/***************************************************************************//**
 *   @file   dual_core_ltc4284_ipc.h
 *   @brief  Shared IPC definitions for LTC4284 dual-core example.
 *   @author Analog Devices Inc.
 *
 * This header defines the shared memory structure and IPC protocol used for
 * inter-processor communication between ARM (CPU0) and RISC-V (CPU1) cores
 * in the LTC4284 dual-core monitoring example.
 *
 * Architecture:
 * - ARM (CPU0):  User interface, UART console, telemetry display
 * - RISC-V (CPU1): LTC4284 I2C driver, telemetry acquisition, ALERT monitoring
 *
 * Communication:
 * - ARM requests telemetry via IPC commands
 * - RISC-V reads from LTC4284 over I2C and updates shared memory
 * - RISC-V signals completion via doorbell interrupt
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

#ifndef _DUAL_CORE_LTC4284_IPC_H_
#define _DUAL_CORE_LTC4284_IPC_H_

#include <stdint.h>

/* Shared memory address (in RISC-V SRAM region) */
#define LTC4284_IPC_TABLE_ADDR  0x20110000UL

/* Magic value to validate IPC table initialization */
#define LTC4284_IPC_MAGIC       0xC0DEC0DEUL

/* Command opcodes (ARM → RISC-V) */
#define LTC4284_CMD_NOP                0x00  /* No operation (heartbeat) */
#define LTC4284_CMD_READ_TELEMETRY     0x01  /* Read all telemetry data */
#define LTC4284_CMD_CLEAR_FAULTS       0x02  /* Clear fault registers */
#define LTC4284_CMD_ENABLE_FET         0x03  /* Enable/disable FET (param1: 0/1) */
#define LTC4284_CMD_READ_CONFIG_REGS   0x04  /* Read CONFIG_1/CONTROL_2 + OC snapshot */
#define LTC4284_CMD_SHUTDOWN           0xFF  /* Shutdown RISC-V core */

/* Status flags (RISC-V → ARM) */
#define LTC4284_STATUS_READY   (1 << 0)  /* RISC-V initialized and ready */
#define LTC4284_STATUS_BUSY    (1 << 1)  /* Processing command */
#define LTC4284_STATUS_ERROR   (1 << 2)  /* Last command failed */
#define LTC4284_STATUS_ALERT   (1 << 3)  /* Hardware ALERT pin active */

/* Telemetry data structure */
typedef struct {
	uint32_t vin_mv;                 /* Input voltage (millivolts) */
	uint32_t iin_ma;                 /* Input current (milliamps) */
	uint32_t pin_mw;                 /* Input power (milliwatts) */
	uint32_t vout_mv;                /* Output voltage (millivolts) */
	uint32_t vds_mv;                 /* Drain-source voltage (millivolts) */
	uint8_t  status_reg;             /* SYSTEM_STATUS register */
	uint8_t  fault_reg;              /* FAULT register */
	uint8_t  adc_status;             /* ADC_STATUS register */
	uint8_t  reserved;
	uint64_t energy_code;            /* Raw 48-bit energy accumulator (not mJ) */
	uint32_t timestamp_ms;           /* RISC-V timestamp */
	uint8_t  raw_reserved[40];       /* Reserved for future expansion */
} ltc4284_telemetry_t;

/* Config snapshot (populated by CMD_READ_CONFIG_REGS) */
typedef struct {
	uint8_t  cfg1;                   /* CONFIG_1 register (after programming) */
	uint8_t  ctrl2;                  /* CONTROL_2 register (after programming) */
	uint8_t  ilim_mv;                /* V_ILIM setting in mV (steady-state) */
	uint8_t  foldback_code;          /* LTC4284_FB_* code */
	uint8_t  retry_code;             /* LTC4284_RETRY_* code */
	uint8_t  reserved[3];
	uint32_t rsense_uohm;            /* Sense resistor value */
	uint32_t trip_ma;                /* Computed steady-state trip current */
} ltc4284_config_snapshot_t;

/* Shared IPC table structure (256 bytes total) */
typedef struct {
	/* Header (8 bytes) */
	uint32_t magic;                      /* LTC4284_IPC_MAGIC when initialized */
	volatile uint32_t status;            /* Status flags (LTC4284_STATUS_*) */

	/* ARM → RISC-V Command Queue (16 bytes) */
	volatile uint32_t cmd_opcode;        /* Command type (LTC4284_CMD_*) */
	volatile uint32_t cmd_sequence;      /* Increments with each command */
	volatile uint32_t cmd_param1;        /* Optional parameter 1 */
	volatile uint32_t cmd_param2;        /* Optional parameter 2 */

	/* RISC-V → ARM Response (8 bytes) */
	volatile uint32_t rsp_sequence;      /* Matches cmd_sequence when complete */
	volatile uint32_t rsp_error_code;    /* 0 = success, non-zero = error code */

	/* Telemetry data (72 bytes) */
	ltc4284_telemetry_t telemetry;

	/* Alert event log (64 bytes) */
	volatile uint32_t alert_count;       /* Total alert events */
	volatile uint32_t last_alert_ms;     /* Timestamp of last alert */
	uint8_t alert_reserved[56];

	/* Statistics (64 bytes) */
	volatile uint32_t total_commands;    /* Total commands processed */
	volatile uint32_t failed_commands;   /* Failed command count */
	volatile uint32_t i2c_errors;        /* I2C communication errors */
	ltc4284_config_snapshot_t config;    /* populated by READ_CONFIG_REGS */
	uint8_t stats_reserved[36];

	/* Padding to 256 bytes */
	uint8_t reserved[24];
} ltc4284_ipc_table_t;

#endif /* _DUAL_CORE_LTC4284_IPC_H_ */
