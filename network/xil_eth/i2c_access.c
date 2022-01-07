/*
 * Copyright (C) 2013 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */
/*****************************************************************************/
/**
 * @file i2c_lib.c
 *
 * This file contains library functions to initialize, control and access
 * IIC devices.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date	 Changes
 * ----- ---- -------- ---------------------------------------------------------
 * 1.0   srt  10/19/13 Initial Version
 *
 * </pre>
 *
 ******************************************************************************/

/***************************** Include Files *********************************/
#include "xparameters.h"
#if defined (__arm__) && !defined (ARMR5)
#if XPAR_GIGE_PCS_PMA_SGMII_CORE_PRESENT == 1 || \
	XPAR_GIGE_PCS_PMA_1000BASEX_CORE_PRESENT == 1
#include "xil_exception.h"
#include "xil_printf.h"
#include "xiicps.h"
#include "sleep.h"
#include "xscugic.h"

/************************** Constant Definitions *****************************/
#define IIC_DEVICE_ID   XPAR_XIICPS_0_DEVICE_ID
#define XIIC	XIicPs
#define XIICCFG	XIicPs_Config
#define I2cSetStatusHandler	XIicPs_SetStatusHandler
#define	I2cLookupConfig		XIicPs_LookupConfig
#define	I2cCfgInitialize	XIicPs_CfgInitialize
#define INTC_DEVICE_ID	XPAR_SCUGIC_SINGLE_DEVICE_ID
#define IIC_INTR_ID	XPAR_XIICPS_0_INTR
#define INTC_HANDLER	XScuGic_InterruptHandler
#define IIC_HANDLER	XIicPs_IntrHandler
#define INTC	XScuGic
#define IIC_SCLK_RATE           100000
/**************************** Type Definitions *******************************/
typedef struct {
	XIIC I2cInstance;
	INTC IntcInstance;
	volatile u8 TransmitComplete;   /* Flag to check completion of Transmission */
	volatile u8 ReceiveComplete;    /* Flag to check completion of Reception */
	volatile u32 TotalErrorCount;
} XIIC_LIB;
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
int I2cPhyWrite(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 Data, u16 SlaveAddr);
int I2cPhyRead(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 *Data, u16 SlaveAddr);
int I2cSetupHardware(XIIC_LIB *I2cLibPtr);
int I2cWriteData(XIIC_LIB *I2cLibPtr, u8 *WrBuffer, u16 ByteCount, u16 SlaveAddr);
int I2cReadData(XIIC_LIB *I2cLibPtr, u8 *RdBuffer, u16 ByteCount, u16 SlaveAddr);
static int SetupInterruptSystem(XIIC_LIB *I2cLibPtr);
static void StatusHandler(XIIC_LIB *I2cLibPtr, int Event);

/************************* Global Definitions *****************************/

/************************** Function Definitions *****************************/

/*****************************************************************************/
/**
 * This function configures the IIC hardware.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int I2cSetupHardware(XIIC_LIB *I2cLibPtr)
{
	int Status;
	XIICCFG *ConfigPtr;
	XIIC *I2cInstancePtr;

	I2cInstancePtr = &I2cLibPtr->I2cInstance;

	/*
	 * Initialize the IIC driver so that it is ready to use.
	 */
	ConfigPtr = I2cLookupConfig(IIC_DEVICE_ID);
	if (ConfigPtr == NULL) {
		return XST_FAILURE;
	}

	Status = I2cCfgInitialize(I2cInstancePtr, ConfigPtr,
			ConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * GPIO Code to pull MUX out of reset.
	 */
	Xil_Out32(0xe000a204, 0x2000);
	Xil_Out32(0xe000a208, 0x2000);
	Xil_Out32(0xe000a040, 0x2000);

	/*
	 * Setup the Interrupt System.
	 */
	Status = SetupInterruptSystem(I2cLibPtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	I2cSetStatusHandler(I2cInstancePtr, I2cLibPtr, (IIC_HANDLER) StatusHandler);

	/*
	 * Set the IIC serial clock rate.
	 */
	XIicPs_SetSClk(I2cInstancePtr, IIC_SCLK_RATE);

	I2cLibPtr->TotalErrorCount = 0;
	I2cLibPtr->TransmitComplete = FALSE;
	I2cLibPtr->ReceiveComplete = FALSE;

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function writes data to the PHY.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 * @param 	PhyAddr is the address of PHY to be written
 * @param	Reg is the register address to be written to
 * @param 	Data is the pointer which contains the data to be written
 * @param	SlaveAddr is the address of the slave we are sending to.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int I2cPhyWrite(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 Data, u16 SlaveAddr)
{
	int Status;
	u8 WrBuffer[3];

	WrBuffer[0] = Reg;
	WrBuffer[1] = Data >> 8;
	WrBuffer[2] = Data;

	Status = I2cWriteData(I2cLibPtr, WrBuffer, 3, SlaveAddr);
	if (Status != XST_SUCCESS) {
		xil_printf("PhyWrite: Writing data failed\n\r");
		return Status;
	}

	return Status;
}

/*****************************************************************************/
/**
 * This function reads data from the PHY.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 * @param 	PhyAddr is the address of PHY to be read from
 * @param	Reg is the register address to be read from
 * @param 	Data is the pointer which stores the data read
 * @param	SlaveAddr is the address of the slave we are sending to.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int I2cPhyRead(XIIC_LIB *I2cLibPtr, u8 PhyAddr, u8 Reg, u16 *Data, u16 SlaveAddr)
{
	int Status;
	u8 WrBuffer[2];
	u8 RdBuffer[2];

	WrBuffer[0] = Reg;

	Status = I2cWriteData(I2cLibPtr, WrBuffer, 1, SlaveAddr);
	if (Status != XST_SUCCESS) {
		xil_printf("PhyWrite: Writing data failed\n\r");
		return Status;
	}

	Status = I2cReadData(I2cLibPtr, RdBuffer, 2, SlaveAddr);
	if (Status != XST_SUCCESS) {
		xil_printf("PhyRead: Reading data failed\n\r");
		return Status;
	}

	*Data = RdBuffer[0] << 8 | RdBuffer[1];

	return Status;
}

/*****************************************************************************/
/**
 * This function writes a buffer of data to the IIC Device.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 * @param 	WrBuffer is the buffer which contains data to be written
 * @param	ByteCount contains the number of bytes in the buffer to be
 *			written.
 * @param	SlaveAddr is the address of the slave we are sending to.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int I2cWriteData(XIIC_LIB *I2cLibPtr, u8 *WrBuffer, u16 ByteCount,
		u16 SlaveAddr)
{
	XIIC *I2cInstancePtr;

	I2cInstancePtr = &I2cLibPtr->I2cInstance;

	I2cLibPtr->TransmitComplete = FALSE;

	/*
	 * Send the Data.
	 */
	XIicPs_MasterSend(I2cInstancePtr, WrBuffer, ByteCount, SlaveAddr);

	/*
	 * Wait for the entire buffer to be sent, letting the interrupt
	 * processing work in the background, this function may get
	 * locked up in this loop if the interrupts are not working
	 * correctly.
	 */
	while (I2cLibPtr->TransmitComplete == FALSE) {
		if (0 != I2cLibPtr->TotalErrorCount) {
			xil_printf("I2cWriteData: Failed due to errors\n\r");
			return XST_FAILURE;
		}
	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(I2cInstancePtr));

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function read data from the IIC Device.
 *
 * @param	I2cLibPtr contains a pointer to the instance of the IIC library
 * @param 	RdBuffer is the buffer into which data read
 * @param	ByteCount contains the number of bytes in the buffer to be
 *			written.
 * @param	SlaveAddr is the address of the slave we are sending to.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 ******************************************************************************/
int I2cReadData(XIIC_LIB *I2cLibPtr, u8 *RdBuffer, u16 ByteCount, u16 SlaveAddr)
{
	XIIC *I2cInstancePtr;

	I2cInstancePtr = &I2cLibPtr->I2cInstance;

	I2cLibPtr->ReceiveComplete = FALSE;

	/*
	 * Receive the Data.
	 */
	XIicPs_MasterRecv(I2cInstancePtr, RdBuffer, ByteCount, SlaveAddr);

	while (I2cLibPtr->ReceiveComplete == FALSE) {
		if (0 != I2cLibPtr->TotalErrorCount) {
			xil_printf("I2cReadData: Failed due to errors %d\n\r",
					I2cLibPtr->TotalErrorCount);
			return XST_FAILURE;
		}
	}

	/*
	 * Wait until bus is idle to start another transfer.
	 */
	while (XIicPs_BusIsBusy(I2cInstancePtr))
		;

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This function setups the interrupt system so interrupts can occur for the
 * IIC device. The function is application-specific since the actual system may
 * or may not have an interrupt controller. The IIC device could be directly
 * connected to a processor without an interrupt controller. The user should
 * modify this function to fit the application.
 *
 * @param	IicInstPtr contains a pointer to the instance of the IIC device
 *		which is going to be connected to the interrupt controller.
 *
 * @return	XST_SUCCESS if successful else XST_FAILURE.
 *
 * @note		None.
 *
 ******************************************************************************/
static int SetupInterruptSystem(XIIC_LIB *I2cLibPtr)
{
	int Status;
	XIIC *I2cInstancePtr;
	INTC *IntcPtr;

	I2cInstancePtr = &I2cLibPtr->I2cInstance;
	IntcPtr = &I2cLibPtr->IntcInstance;

	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcPtr, IntcConfig,
			IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XScuGic_SetPriorityTriggerType(IntcPtr, IIC_INTR_ID, 0xA0, 0x3);

	/*
	 * Connect the interrupt handler that will be called when an
	 * interrupt occurs for the device.
	 */
	Status = XScuGic_Connect(IntcPtr, IIC_INTR_ID,
			(Xil_InterruptHandler) XIicPs_MasterInterruptHandler,
			I2cInstancePtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the IIC device.
	 */
	XScuGic_Enable(IntcPtr, IIC_INTR_ID);

	/*
	 * Initialize the exception table and register the interrupt
	 * controller handler with the exception table
	 */
	Xil_ExceptionInit();

	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) INTC_HANDLER, IntcPtr);

	/* Enable non-critical exceptions */Xil_ExceptionEnable();

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
 * This Status handler is called asynchronously from an interrupt
 * context and indicates the events that have occurred.
 *
 * @param	InstancePtr is a pointer to the IIC driver instance for which
 *		the handler is being called for.
 * @param	Event indicates the condition that has occurred.
 *
 * @return	None.
 *
 * @note		None.
 *
 ******************************************************************************/
static void StatusHandler(XIIC_LIB *I2cLibPtr, int Event)
{
	/*
	 * All of the data transfer has been finished.
	 */
	if (0 != (Event & XIICPS_EVENT_COMPLETE_RECV)) {
		I2cLibPtr->ReceiveComplete = TRUE;
	} else if (0 != (Event & XIICPS_EVENT_COMPLETE_SEND)) {
		I2cLibPtr->TransmitComplete = TRUE;
	} else if (0 == (Event & XIICPS_EVENT_SLAVE_RDY)) {
		/*
		 * If it is other interrupt but not slave ready interrupt, it is
		 * an error.
		 * Data was received with an error.
		 */
		I2cLibPtr->TotalErrorCount++;
	}
}
#endif
#endif
