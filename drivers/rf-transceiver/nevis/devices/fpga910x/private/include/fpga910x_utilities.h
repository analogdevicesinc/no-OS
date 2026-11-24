/**
 * \file
 * \brief Contains FPGA910X utility functions like common math routines.
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
* Copyright 2022 Analog Devices Inc.
* Released under the FPGA910X API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _FPGA910X_UTILITIES_H_
#define _FPGA910X_UTILITIES_H_

#include <stdbool.h>
#include "adi_fpga910x_types.h"
#include "adi_common_error.h"
#include "adi_common_macros.h"
#include "adrv9102_zcu102.h"
#include "axi_common.h"
#include "axi_dma.h"
#include "axi_sysid.h"
#include "axi_adrv9102.h"
#include "axi_adrv9102_top_regs.h"
#include "axi_adrv9102_gpio_regs.h"
#include "axi_adrv9102_ssi_regs.h"
#include "axi_adrv9102_tdd_regs.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief This utility function calculates the LCM of two unsigned integers
*
* \param[in]  fpga910x       Context variable - Pointer to FPGA910X device data structure
* \param[in]  number1        First number of LCM function
* \param[in]  number2        Second number of LCM function
* \param[out] lcm            The least common multiple of number1 and number2
*
* \returns A code indicating success (ADI_COMMON_ACT_NO_ACTION) or the required action to recover
*/
int32_t fpga910x_Utilities_CalculateLcm(adi_fpga910x_Device_t *fpga910x,
                                        uint32_t number1,
                                        uint32_t number2,
                                        uint32_t *lcm);

uint32_t fpga910x_SsiIdGet(adi_fpga910x_Device_t *fpga910x,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel);

uint32_t fpga910x_TddIdGet(adi_fpga910x_Device_t *fpga910x,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel);
    
uint32_t fpga910x_TddDmaIdGet(adi_fpga910x_Device_t *fpga910x,
                              adi_common_Port_e port,
                              adi_common_ChannelNumber_e channel);

uint32_t fpga910x_DMAIdGet(adi_fpga910x_Device_t *fpga910x, adi_common_Port_e port, adi_common_ChannelNumber_e channel);

uint32_t fpga910x_Dma_Get(adi_fpga910x_Device_t *fpga910x, adi_common_Port_e port, adi_common_ChannelNumber_e channel);

#ifdef __cplusplus
}
#endif

#endif /* _FPGA910X_UTILITIES_H_ */
