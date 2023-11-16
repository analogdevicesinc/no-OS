/**
 * \file adrv904x_cpu_cmd_gpio.h
 * 
 * \brief   Command definitions for GPIO.
 *
 * ADRV904X API Version: 2.9.0.4
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

#ifndef __ADRV904X_CPU_CMD_GPIO_H__
#define __ADRV904X_CPU_CMD_GPIO_H__

#include "adi_adrv904x_platform_pack.h"
#include "adi_adrv904x_cpu_error_codes_types.h"

/**
 * \brief ARM controlled GPIO pin control enumeration
 */
typedef enum adrv904x_CpuCmd_GpioPinCtrl
{
    ADRV904X_CPU_CMD_GPIO_PIN_DISABLE, /*!< Disable pin for a given signal */
    ADRV904X_CPU_CMD_GPIO_PIN_ENABLE,  /*!< Enable pin for a signal */
} adrv904x_CpuCmd_GpioPinCtrl_e;

typedef uint8_t adrv904x_CpuCmd_GpioPinCtrl_t;

/**
 * \brief ARM controlled GPIO signal enumeration
 */
typedef enum adrv904x_CpuCmd_GpioSignal
{
    ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_BKPT_HIT,         /*!< SW breakpoint was hit signal     */
    ADRV904X_CPU_CMD_GPIO_SIGNAL_SWBKPT_RESUME_FROM_BKPT, /*!< Resume from SW breakpoint signal */
} adrv904x_CpuCmd_GpioSignal_e;

typedef uint8_t adrv904x_CpuCmd_GpioSignal_t;

/**
 * \brief ARM controlled GPIO pin polarity enumeration
 */
typedef enum adrv904x_CpuCmd_GpioPinPolarity
{
    ADRV904X_CPU_CMD_GPIO_PIN_POLARITY_NORMAL,      /*!< Normal pin polarity */
    ADRV904X_CPU_CMD_GPIO_PIN_POLARITY_INVERTED,    /*!< Inverted pin polarity */
} adrv904x_CpuCmd_GpioPinPolarity_e;

typedef uint8_t adrv904x_CpuCmd_GpioPinPolarity_t;

/**
 * \brief SET_GPIO command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetGpio
{
    adrv904x_CpuCmd_GpioSignal_t      signal;
    uint8_t                           pin;
    adrv904x_CpuCmd_GpioPinPolarity_t polarity;
    adrv904x_CpuCmd_GpioPinCtrl_t     enable;
} adrv904x_CpuCmd_SetGpio_t;)

/**
 * \brief SET_GPIO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_SetGpioResp
{
    adrv904x_CpuErrorCode_e cmdStatus;      /*!< Command status */
} adrv904x_CpuCmd_SetGpioResp_t;)

/**
 * \brief GET_GPIO command structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetGpio
{
    adrv904x_CpuCmd_GpioSignal_t signal;
} adrv904x_CpuCmd_GetGpio_t;)


/**
 * \brief GET_GPIO command response structure
 */
ADI_ADRV904X_PACKED(
typedef struct adrv904x_CpuCmd_GetGpioResp
{
    uint8_t                           pin;
    adrv904x_CpuCmd_GpioPinPolarity_t polarity;
    adrv904x_CpuErrorCode_e           cmdStatus;      /*!< Command status */
} adrv904x_CpuCmd_GetGpioResp_t;)


#endif /* __ADRV904X_CPU_CMD_GPIO_H__ */

