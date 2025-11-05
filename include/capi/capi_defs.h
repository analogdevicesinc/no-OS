/*
 * Copyright (c) 2024-2025 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief CAPI common definitions
 */

#ifndef _CAPI_DEFS_H
#define _CAPI_DEFS_H

#if defined(__cplusplus)
extern "C" {
#endif /* _cplusplus */

#ifdef CAPI_DIRECT_API /** fops and capi thin layer is not populated,                              \
			function names of driver will directly same with capi */

#define CAPI_FUNC(name) name

#else /** capi thin layer is populated for the cases multiple drivers implement same API           \
	  so function names will has drivers' prefixes.                                            \
	  drivers shall declare CAPI_DRIVER_NAME to use this feature */

#define _CAPI_FUNC_NAME_EXPAND(a, b)    a##_##b
#define _CAPI_FUNC_NAME(drv_name, name) _CAPI_FUNC_NAME_EXPAND(drv_name, name)
#define CAPI_FUNC(name)                 _CAPI_FUNC_NAME(CAPI_DRIVER_NAME, name)

#endif /* CAPI_DIRECT_API_WDT */

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_DEFS_H */