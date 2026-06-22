/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Xilinx platform I/O macro definitions for CAPI.
 *
 * Provides the Xilinx-backed implementations of the CAPI_PLATFORM_IO_*
 * read/write macros, mapping them onto the BSP Xil_In / Xil_Out primitives.
 */

#ifndef _XILINX_CAPI_PLATFORM_H_
#define _XILINX_CAPI_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "xil_io.h"
#include "xil_types.h"
#include <stdint.h>

#define CAPI_PLATFORM_IO_READ8(addr)  Xil_In8((UINTPTR)(addr))
#define CAPI_PLATFORM_IO_READ16(addr) Xil_In16((UINTPTR)(addr))
#define CAPI_PLATFORM_IO_READ32(addr) Xil_In32((UINTPTR)(addr))
#define CAPI_PLATFORM_IO_READ64(addr) Xil_In64((UINTPTR)(addr))

#define CAPI_PLATFORM_IO_WRITE8(addr, val)  Xil_Out8((UINTPTR)(addr), (u8)(val))
#define CAPI_PLATFORM_IO_WRITE16(addr, val) Xil_Out16((UINTPTR)(addr), (u16)(val))
#define CAPI_PLATFORM_IO_WRITE32(addr, val) Xil_Out32((UINTPTR)(addr), (u32)(val))
#define CAPI_PLATFORM_IO_WRITE64(addr, val) Xil_Out64((UINTPTR)(addr), (u64)(val))

#ifdef __cplusplus
}
#endif

#endif /* _XILINX_CAPI_PLATFORM_H_ */
