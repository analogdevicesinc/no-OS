/**
 * \file
 * \brief Template customer implementation of the common HAL
 * 
 * Copyright (c) 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#include "adi_common_hal_customer.h"

int32_t customer_LogWrite(void *devHalCfg, uint32_t logLevel, const char *comment, va_list argp)
{
    /* Customer code goes here */
    return 0;
}
int32_t customer_TimerWait_us(void *devHalCfg, uint32_t time_us)
{
    /* Customer code goes here */
    return 0;
}
