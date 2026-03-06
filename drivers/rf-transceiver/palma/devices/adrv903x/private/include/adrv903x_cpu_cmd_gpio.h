/**
 * \file adrv903x_cpu_cmd_gpio.h
 * 
 * \brief   Command definitions for GPIO.
 *
 * ADRV903X API Version: 2.12.1.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ADRV903X_CPU_CMD_GPIO_H__
#define __ADRV903X_CPU_CMD_GPIO_H__

#include "adi_adrv903x_platform_pack.h"
#include "adi_adrv903x_cpu_error_codes_types.h"

/**
 * \brief ARM controlled GPIO pin control enumeration
 */
typedef enum adrv903x_CpuCmd_GpioPinCtrl
{
    ADRV903X_CPU_CMD_GPIO_PIN_DISABLE, /*!< Disable pin for a given signal */
    ADRV903X_CPU_CMD_GPIO_PIN_ENABLE,  /*!< Enable pin for a signal */
} adrv903x_CpuCmd_GpioPinCtrl_e;

typedef uint8_t adrv903x_CpuCmd_GpioPinCtrl_t;

/**
 * \brief ARM controlled GPIO signal enumeration
 */
typedef enum adrv903x_CpuCmd_GpioSignal
{
    ADRV903X_CPU_CMD_GPIO_SIGNAL_SWBKPT_BKPT_HIT,         /*!< SW breakpoint was hit signal     */
    ADRV903X_CPU_CMD_GPIO_SIGNAL_SWBKPT_RESUME_FROM_BKPT, /*!< Resume from SW breakpoint signal */
} adrv903x_CpuCmd_GpioSignal_e;

typedef uint8_t adrv903x_CpuCmd_GpioSignal_t;

/**
 * \brief ARM controlled GPIO pin polarity enumeration
 */
typedef enum adrv903x_CpuCmd_GpioPinPolarity
{
    ADRV903X_CPU_CMD_GPIO_PIN_POLARITY_NORMAL,      /*!< Normal pin polarity */
    ADRV903X_CPU_CMD_GPIO_PIN_POLARITY_INVERTED,    /*!< Inverted pin polarity */
} adrv903x_CpuCmd_GpioPinPolarity_e;

typedef uint8_t adrv903x_CpuCmd_GpioPinPolarity_t;

/**
 * \brief SET_GPIO command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetGpio
{
    adrv903x_CpuCmd_GpioSignal_t      signal;
    uint8_t                           pin;
    adrv903x_CpuCmd_GpioPinPolarity_t polarity;
    adrv903x_CpuCmd_GpioPinCtrl_t     enable;
} adrv903x_CpuCmd_SetGpio_t;)

/**
 * \brief SET_GPIO command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_SetGpioResp
{
    adrv903x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv903x_CpuCmd_SetGpioResp_t;)

/**
 * \brief GET_GPIO command structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetGpio
{
    adrv903x_CpuCmd_GpioSignal_t signal;
} adrv903x_CpuCmd_GetGpio_t;)


/**
 * \brief GET_GPIO command response structure
 */
ADI_ADRV903X_PACKED(
typedef struct adrv903x_CpuCmd_GetGpioResp
{
    uint8_t                           pin;
    adrv903x_CpuCmd_GpioPinPolarity_t polarity;
    adrv903x_CpuErrorCode_e           cmdStatus;      /*!< Command status */
} adrv903x_CpuCmd_GetGpioResp_t;)


#endif /* __ADRV903X_CPU_CMD_GPIO_H__ */

