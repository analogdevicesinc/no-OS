/**
 * \file
 * \brief Functions to configure and control the FPGA910X GPIO pins
 *
 * Copyright (c) 2022 - 2026 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 * Released under the ADRV910X API license.
 * For more information, see the "LICENSE.txt" file.
 */

#ifndef _ADI_FPGA910X_GPIO_H_
#define _ADI_FPGA910X_GPIO_H_

#include "adi_fpga910x_gpio_types.h"
#include "adi_fpga910x_types.h"
#include "adi_fpga910x.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Set the operationMode for the pin control
 *
 * \param[in] fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[in] operationMode  The operation mode to set
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_OperationMode_Set(adi_fpga910x_Device_t *fpga910x,
                                            adi_fpga910x_OperationModes_e operationMode);

/**
 * \brief Get the current operationMode
 *
 * \param[in]  fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[out] operationMode  The current operation mode
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_OperationMode_Get(adi_fpga910x_Device_t *fpga910x,
                                            adi_fpga910x_OperationModes_e *operationMode);

/**
 * \brief Read the values on the GPIO input register
 *
 * \param[in]  fpga910x  Context variable - Pointer to the FPGA910X device data structure
 * \param[in]  group     GPIO group to read: DGPIO, RX0,, RX1, TX0, TX1
 * \param[out] rdData    The current input values
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_Read(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioGroup_e group, uint16_t *rdData);

/**
 * \brief Configure a single GPIO pin
 *
 * \param[in] fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[in] gpioPinConfig  The desired GPIO pin configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_pin_Configure(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioPinCfg_t *gpioPinConfig);

/**
 * \brief Inspect a single GPIO pin
 *
 * \param[in]  fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[out] gpioPinConfig  The current GPIO pin configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_pin_Inspect(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioPinCfg_t *gpioPinConfig);

/**
 * \brief Configures the specified FPGA910x GPIO pin with the desired source mode
 *
 * \param[in] fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[in] gpioPinConfig  GPIO pin configurations to write
 * \param[in] length         Number of GPIO pin configurations to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_pin_SourceMode_Configure(adi_fpga910x_Device_t *fpga910x,
                                                   adi_fpga910x_GpioPinCfg_t gpioPinConfig[],
                                                   uint32_t length);

/**
 * \brief Inspect the FPGA910x GPIO pins
 *
 * \param[in]  fpga910x       Context variable - Pointer to the FPGA910X device data structure
 * \param[out] gpioPinConfig  GPIO pin configurations to write
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_pin_SourceMode_Inspect(adi_fpga910x_Device_t *fpga910x,
                                                 adi_fpga910x_GpioPinCfg_t *gpioPinConfig);

/**
 * \brief Configure FPGA to assign GPIO pins as observation GPIO to capture gain index
 *
 * \param[in] fpga910x         Context variable - Pointer to the FPGA910X device data structure
 * \param[in] gainIndexConfig  The desired configuration to assign FPGA GPIO pins as observation GPIO
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_GainIndex_Configure(adi_fpga910x_Device_t *fpga910x,
                                              adi_fpga910x_GpioGainIndexCfg_t *gainIndexConfig);

/**
 * \brief Read observation GPIO data from FPGA capture memory
 *
 * \param[in]  fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[out] gainIndex  Buffer to store the captured gain index
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_GainIndex_Get(adi_fpga910x_Device_t *fpga910x, uint32_t *gainIndex);

/**
 * \brief Configure FPGA to assign GPIO pins as trigger source
 *
 * \param[in] fpga910x    Context variable - Pointer to the FPGA910X device data structure
 * \param[in] triggerCfg  The desired configuration to assign FPGA GPIO pins as trigger source
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_TriggerSource_Configure(adi_fpga910x_Device_t *fpga910x,
                                                  adi_fpga910x_GpioTriggerCfg_t *triggerCfg);

/**
 * \brief Configure SMA pin 1 and 2
 *
 * \param[in] fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[in] smaConfig  The desired SMA pin configuration
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_smapin_Configure(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioSmaCfg_t *smaConfig);

/**
 * \brief Inspect SMA pin 1 and 2
 *
 * \param[in]  fpga910x   Context variable - Pointer to the FPGA910X device data structure
 * \param[out] smaConfig  The current direction of SMA pins 1 and 2
 *
 * \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
 */
int32_t adi_fpga910x_gpio_smapin_Inspect(adi_fpga910x_Device_t *fpga910x, adi_fpga910x_GpioSmaCfg_t *smaConfig);

#ifdef __cplusplus
}
#endif

#endif /* _ADI_FPGA910X_GPIO_H_ */
