/**
 * \file
 * \brief Contains private fpga910x utility related functions
 *
 * FPGA910X API Version: $ADI_FPGA910X_API_VERSION$
 */

/**
* Copyright 2022 Analog Devices Inc.
* Released under the ADRV910x API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#include "fpga910x_utilities.h"
#include "adi_fpga910x_hal.h"

int32_t fpga910x_Utilities_CalculateLcm(adi_fpga910x_Device_t *fpga910x,
                                        uint32_t number1,
                                        uint32_t number2,
                                        uint32_t *lcm)
{
    uint32_t num1 = 0;
    uint32_t num2 = 0;
    uint8_t i = 0;
    
    *lcm = 0;
    
    if (number1 == 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         number1,
                         "Cannot take LCM with first number equal to 0");
    }
    
    if (number2 == 0)
    {
        ADI_ERROR_REPORT(&fpga910x->common,
                         ADI_COMMON_ERRSRC_API,
                         ADI_COMMON_ERR_INV_PARAM,
                         ADI_COMMON_ACT_ERR_CHECK_PARAM,
                         number2,
                         "Cannot take LCM with second number equal to 0");
    }

    if (number1 > number2)
    {
        num1 = number1;
        num2 = number2;
    }
    else
    {
        num1 = number2;
        num2 = number1;
    }

    for (i = 1; i < num2; i++)
    {
        if (((num1 * i) % num2) == 0)
        {
            *lcm = num1 * i;
            ADI_API_RETURN(fpga910x);

        }
    }

    *lcm = num1 * num2;
    ADI_API_RETURN(fpga910x);
}

uint32_t fpga910x_SsiIdGet(adi_fpga910x_Device_t *fpga910x,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return(AXI_ADRV9102_SSI_RX_OFFSET_0);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return(AXI_ADRV9102_SSI_RX_OFFSET_1);
    if (port == ADI_TX) return(AXI_ADRV9102_SSI_TX_OFFSET);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga910x_TddIdGet(adi_fpga910x_Device_t *fpga910x,
                           adi_common_Port_e port,
                           adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

	if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9102_TDD_RX0_DEVICE);
	if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9102_TDD_RX1_DEVICE);
	if (port == ADI_TX) return (AXI_ADRV9102_TDD_TX0_DEVICE);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9102_TDD_ORX0_GPIO);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9102_TDD_ORX1_GPIO);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga910x_TddDmaIdGet(adi_fpga910x_Device_t *fpga910x,
                              adi_common_Port_e port,
                              adi_common_ChannelNumber_e channel)
{
    char error_message[1024];

	if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9102_TDD_RX0_DMA_DATA);
	if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9102_TDD_RX1_DMA_DATA);
	if (port == ADI_TX) return (AXI_ADRV9102_TDD_TX0_DMA_DATA);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return (AXI_ADRV9102_TDD_ORX0_DMA_DATA);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return (AXI_ADRV9102_TDD_ORX1_DMA_DATA);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return ((uint32_t) - 1);
}

uint32_t fpga910x_DMAIdGet(adi_fpga910x_Device_t *fpga910x, adi_common_Port_e port, adi_common_ChannelNumber_e channel)
{
    char error_message[1024] = { 0 };

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return(AXI_DMA_RX0_ID);
    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return(AXI_DMA_RX1_ID);
    if (port == ADI_TX) return(AXI_DMA_TX0_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return(AXI_DMA_ORX0_ID);
    if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return(AXI_DMA_ORX1_ID);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return((uint32_t) -1);
}

uint32_t fpga910x_Dma_Get(adi_fpga910x_Device_t *fpga910x, adi_common_Port_e port, adi_common_ChannelNumber_e channel)
{
    char error_message[1024] = { 0 };

    if ((port == ADI_RX)  && (channel == ADI_CHANNEL_1)) return (ADI_FPGA910X_HAL_DMA_RX1);
	if ((port == ADI_RX)  && (channel == ADI_CHANNEL_2)) return (ADI_FPGA910X_HAL_DMA_RX2);
	if (port == ADI_TX) return (ADI_FPGA910X_HAL_DMA_TX);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_1)) return (ADI_FPGA910X_HAL_DMA_ORX1);
	if ((port == ADI_ORX) && (channel == ADI_CHANNEL_2)) return (ADI_FPGA910X_HAL_DMA_ORX2);

    sprintf(error_message, "Invalid port (%d) or channel (%d) parameter.", (int) port, (int) channel);
    ADI_ERROR_REPORT(&fpga910x->common, ADI_COMMON_ERRSRC_API, 0, 0, 0, error_message);
    return ((uint32_t) - 1);
}
