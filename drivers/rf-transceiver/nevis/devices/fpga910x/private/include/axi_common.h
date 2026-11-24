// **********************************************************************************
// **********************************************************************************
// ----------------------------------------------------------------------------------
// ################
// ##   ###########   Analog Devices Inc.
// ##      ########
// ##         #####   Copyright (c) 2019 Analog Devices Inc. All rights reserved.
// ##            ##   This file is the confidential and proprietary property of ADI.
// ##         #####   Possession or use of this file requires a written license.
// ##      ########   The licensing information may be found at: www.analog.com
// ##   ###########
// ################
// ----------------------------------------------------------------------------------
// Author:            Rejeesh Kutty
// Description:       ADI Platform
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#ifndef AXI_COMMON_H
#define AXI_COMMON_H

// **********************************************************************************
// **********************************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// **********************************************************************************
// **********************************************************************************

#define AXI_LOG_NOTE     0x0
#define AXI_LOG_INFO     0x1
#define AXI_LOG_WARNING  0x2
#define AXI_LOG_ERROR    0x3
#define AXI_LOG_FATAL    0x4

#define AXI_DEVICE_T      void
#define AXI_REG_WRITE     axi_reg_write
#define AXI_REG_READ      axi_reg_read
#define AXI_LOG_PRINTF    axi_log_printf

#define AXI_TRIGGER_MODE_SET(n, m) ((m & 0x3) << (n * 2))
#define AXI_TRIGGER_MODE_GET(n, t) ((t >> (n * 2)) & 0x3)
#define AXI_FLAG_VALUE_SET(n, v) ((v & 0x1) << (n * 1))
#define AXI_FLAG_VALUE_GET(n, f) ((f >> (n * 1)) & 0x1)

// **********************************************************************************
// **********************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

// **********************************************************************************
// **********************************************************************************

void axi_log_printf(void *device, uint32_t peripheral_id, uint32_t level,
  const char *format, ...);

// **********************************************************************************
// **********************************************************************************

int32_t axi_reg_write(void *device, uint32_t base, uint32_t offset, uint32_t data);
int32_t axi_reg_read(void *device, uint32_t base, uint32_t offset, uint32_t *data);
int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count);
int32_t axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count);

// **********************************************************************************
// **********************************************************************************

#ifdef __cplusplus
}
#endif

// **********************************************************************************
// **********************************************************************************

#endif

// **********************************************************************************
// **********************************************************************************
