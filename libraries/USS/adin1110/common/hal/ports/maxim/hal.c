/*
 *---------------------------------------------------------------------------
 *
 * Copyright (c) 2020, 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc.
 * and its licensors.By using this software you agree to the terms of the
 * associated Analog Devices Software License Agreement.
 *
 *---------------------------------------------------------------------------
 */

/** @addtogroup hal HAL API
 *  @{
 */

#include <string.h>
#include "hal.h"
#include "hal_port_specific.h"

#ifdef MDIO_GPIO
#include "../../mdio_gpio/mdio_gpio.h"
#endif

extern void MXC_INT_N_DisableIRQ(void);
extern void MXC_INT_N_EnableIRQ(void);
extern uint32_t MXC_SetPendingIrq(void);
extern uint32_t MXC_GetPendingIrq(void);
extern uint32_t MXC_GetEnableIrq(void);
extern uint32_t MXC_RegisterIRQCallback(HAL_Callback_t const *intCallback,
				void * hDevice);
extern uint32_t MXC_SpiReadWrite(uint8_t *pBufferTx, uint8_t *pBufferRx,
				uint32_t nbBytes, bool useDma);
extern uint32_t MXC_SpiRegisterCallback(HAL_Callback_t const *spiCallback,
				void * hDevice);

uint32_t msgWrite(char * ptr)
{
	uint32_t error = 0;

	if(ptr == NULL)
		error = 1;
	else
		printf((const char *)ptr, strlen(ptr));

	return error;
}

void common_Fail(char *FailureReason)
{
	char fail[] = "Failed: ";

	msgWrite(fail);
	msgWrite(FailureReason);
}

void common_Perf(char *InfoString)
{
	msgWrite(InfoString);
}

#ifdef ADIN1100
/*
 * @brief MDIO Read Clause 45
 *
 * @param [in]  phyAddr - Hardware PHY address
 * @param [in]  phyReg - Register address in clause 45 combined devType and regAddr
 * @param [out] phyData - pointer to the data buffer
 *
 * @return error if TA bit is not pulled down by the slave
 *
 * @details
 *
 * @sa
 */
uint32_t HAL_PhyRead(uint8_t hwAddr, uint32_t RegAddr, uint16_t *data)
{
#ifdef MDIO_GPIO
	return (uint32_t)mdioGPIORead_cl45(hwAddr, RegAddr, data);
#else
	return (uint32_t)adi_MdioRead_Cl45(hwAddr, RegAddr, data);
#endif
}

/*
 * @brief MDIO Write Clause 45
 *
 * @param [in] phyAddr - Hardware Phy address
 * @param [in] phyReg - Register address in clause 45 combined devAddr and regAddr
 * @param [out] phyData -  data
 * @return none
 *
 * @details
 *
 * @sa
 */
uint32_t HAL_PhyWrite(uint8_t hwAddr, uint32_t RegAddr, uint16_t data)
{
#ifdef MDIO_GPIO
	return mdioGPIOWrite_cl45(hwAddr, RegAddr, data);
#else
	return adi_MdioWrite_Cl45(hwAddr, RegAddr, data);
#endif
}
#endif

uint32_t HAL_DisableIrq(void)
{
	MXC_INT_N_DisableIRQ();

	return ADI_HAL_SUCCESS;
}

uint32_t HAL_EnableIrq(void)
{
	MXC_INT_N_EnableIRQ();

	return ADI_HAL_SUCCESS;
}

uint32_t HAL_SetPendingIrq(void)
{
	MXC_SetPendingIrq();

	return ADI_HAL_SUCCESS;
}

uint32_t HAL_GetPendingIrq(void)
{
	return MXC_GetPendingIrq();
}

uint32_t HAL_GetEnableIrq(void)
{
	return MXC_GetEnableIrq();
}

/*
 * @brief  Register Phy IRQ Callback function
 *
 * @param [in] intCallback
 * @param [in] hDevice - Pointer to Phy device handler
 * @param [out] none
 * @return none
 *
 * @details
 *
 * @sa
 */
uint32_t HAL_RegisterCallback(HAL_Callback_t const *intCallback,
				void * hDevice)
{
	return MXC_RegisterIRQCallback(intCallback, hDevice);
}

/*
 * @brief SPI write/read
 *
 * @param [in] pBufferTx - Pointer to transmit buffer
 * @param [in] nbBytes - Number bytes to send
 * @param [in] useDma - Enable/Disable DMA transfer for SPI
 * @param [out] pBufferRx - Pointer to receive buffer
 * @return none
 *
 * @details
 *
 * @sa
 */
uint32_t HAL_SpiReadWrite(uint8_t *pBufferTx, uint8_t *pBufferRx,
				uint32_t nbBytes, bool useDma)
{
	return MXC_SpiReadWrite(pBufferTx, pBufferRx, nbBytes, useDma);
}

/*
 * @brief  Register SPI Callback function
 *
 * @param [in] spiCallback - Register SPI IRQ callback function
 * @param [in] hDevice - Pointer to Phy device handler
 * @param [out] none
 * @return none
 *
 * @details
 *
 * @sa
 */
uint32_t HAL_SpiRegisterCallback(HAL_Callback_t const *spiCallback,
				void * hDevice)
{
	return MXC_SpiRegisterCallback(spiCallback, hDevice);
}

uint32_t HAL_Init_Hook(void)
{
	return ADI_HAL_SUCCESS;
}

uint32_t HAL_UnInit_Hook(void)
{
	return ADI_HAL_SUCCESS;
}

/** @}*/

