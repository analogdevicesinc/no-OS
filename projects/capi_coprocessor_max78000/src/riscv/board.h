/***************************************************************************//**
 * @file board.h
 * @brief Minimal MAX78000EVKIT camera BSP surface used by CPU1.
 ******************************************************************************/

#ifndef CAPI_COPROCESSOR_MAX78000_RISCV_BOARD_H
#define CAPI_COPROCESSOR_MAX78000_RISCV_BOARD_H

#include "mxc_device.h"
#include "gpio.h"

#ifndef BOARD_EVKIT_V1
#define BOARD_EVKIT_V1
#endif

#define SCCB_SCL_PORT MXC_GPIO0
#define SCCB_SCL_PIN  MXC_GPIO_PIN_30
#define SCCB_SDA_PORT MXC_GPIO0
#define SCCB_SDA_PIN  MXC_GPIO_PIN_31

#define POWER_OFF 0
#define POWER_ON  1

int Camera_Power(int on);

#endif /* CAPI_COPROCESSOR_MAX78000_RISCV_BOARD_H */
