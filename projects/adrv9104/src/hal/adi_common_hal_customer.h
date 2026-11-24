/**
 * \file
 * \brief Template customer implementation of the common HAL
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#ifndef _ADI_COMMON_HAL_CUSTOMER_H_
#define _ADI_COMMON_HAL_CUSTOMER_H_

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t customer_LogWrite(void *devHalCfg, uint32_t logLevel, const char *comment, va_list argp);
int32_t customer_TimerWait_us(void *devHalCfg, uint32_t time_us);

#ifdef __cplusplus
}
#endif

#endif
