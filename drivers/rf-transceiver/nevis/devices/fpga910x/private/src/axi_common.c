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
// Description:       Platform Common
// ----------------------------------------------------------------------------------
// **********************************************************************************
// **********************************************************************************

#include <stdarg.h>
#include "axi_common.h"
#include "adi_fpga910x_hal_wrapper.h"

// **********************************************************************************
// **********************************************************************************

void axi_log_printf(void *device, uint32_t peripheral_id, uint32_t level,
  const char *format, ...) {

  adi_fpga910x_Device_t *m_device;
  char message[1024];

  va_list arglist;

  va_start(arglist, format);
  vsprintf(message, format, arglist);
  va_end(arglist);

  m_device = (adi_fpga910x_Device_t *)device;
  message[1023] = 0;

  if ((level == AXI_LOG_FATAL) || (level == AXI_LOG_ERROR)) {
    ADI_ERROR_REPORT(&m_device->common, ADI_COMMON_ERRSRC_API, ADI_COMMON_ERR_API_FAIL,
      ADI_COMMON_ACT_ERR_RESET_FULL, 0, message);
  }
}

// **********************************************************************************
// **********************************************************************************

int32_t axi_reg_write(void *device, uint32_t base, uint32_t offset, uint32_t data) {

  return(adi_fpga910x_hal_wrapper_Register_Write(((adi_fpga910x_Device_t *)device),
    (base + offset), data));
}

int32_t axi_reg_read(void *device, uint32_t base, uint32_t offset, uint32_t *data) {

  return(adi_fpga910x_hal_wrapper_Register_Read(((adi_fpga910x_Device_t *)device),
    (base + offset), data));
}

int32_t axi_mem_write(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  return(adi_fpga910x_hal_wrapper_Ram_Write(((adi_fpga910x_Device_t *)device),
    (base + offset), data, count));
}

int32_t axi_mem_read(void *device, uint32_t base, uint32_t offset,
  uint32_t *data, uint32_t count) {

  return(adi_fpga910x_hal_wrapper_Ram_Read(((adi_fpga910x_Device_t *)device),
    (base + offset), data, count));
}

// **********************************************************************************
// **********************************************************************************

