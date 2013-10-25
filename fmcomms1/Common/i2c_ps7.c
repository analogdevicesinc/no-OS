/**************************************************************************//**
*   @file   i2c_ps7.c
*   @brief  ZYNQ Hardware I2C functions implementation.
*   @author ATofan (alexandru.tofan@analog.com)
*
*******************************************************************************
* Copyright 2011(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
* INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*******************************************************************************
*   SVN Revision: $WCREV$
******************************************************************************/

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include "i2c_ps7.h"
#include "xparameters.h"
#include "xil_io.h"
#include "timer.h"

#ifndef XPAR_PS7_I2C_0_BASEADDR
	#define XPAR_PS7_I2C_0_BASEADDR 0
#endif
#define AXI_IIC_BASEADDR_0 XPAR_PS7_I2C_0_BASEADDR
#ifdef XPAR_PS7_I2C_1_BASEADDR
    #define AXI_IIC_BASEADDR_1 XPAR_PS7_I2C_1_BASEADDR
#else
    #define AXI_IIC_BASEADDR_1 XPAR_PS7_I2C_0_BASEADDR
#endif

/*****************************************************************************/
/************************ Variables/Constants Definitions ********************/
/*****************************************************************************/
#define I2C_DELAY	 500 		//delay in us between I2C operations
#define I2C_TIMEOUT	 0xFFFFFF 	//timeout for I2C operations

static uint32_t axi_iic_baseaddr;

/**************************************************************************//**
* @brief Delays the program execution with the specified number of us.
*
* @param us_count Number of us with which the program must be delayed.
* @return None.
******************************************************************************/
extern void delay_us(uint32_t us_count);

/**************************************************************************//**
* @brief Initializes the I2C communication multiplexer.
*
* @param sel - Multiplexer selection value.
*
* @return Returns 0 or negative error code.
******************************************************************************/
uint32_t I2C_EnableMux_ps7(uint8_t sel)
{
    uint32_t ret;
    uint8_t retSel;

    // Set the MUX selection
    ret = I2C_Write_ps7(I2C_MUX_ADDR, -1, 1, &sel);

    if(!ret)
        return -1;

    // Read back the MUX selection
    ret = I2C_Read_ps7(I2C_MUX_ADDR, -1, 1, &retSel);

    if(!ret || (sel != retSel))
        return -1;

    return 0;
}

/**************************************************************************//**
* @brief Initializes the communication with the Microblaze I2C peripheral.
*
* @param i2cAddr - The address of the I2C slave.
* @param fmcPort - Set to 0 for LPC, set to 1 for HPC
* @param enableCommMux - Set to 1 if the carrier board has an I2C multiplexer,
*                        set to 0 otherwise
*
* @return Returns 0 or negative error code.
******************************************************************************/
uint32_t I2C_Init_ps7(uint32_t i2cAddr, uint32_t fmcPort, uint32_t enableCommMux,
					  uint32_t carrierBoard)
{
	uint32_t ret = 0;

    //set the I2C core AXI address
    axi_iic_baseaddr = fmcPort == 0 ? AXI_IIC_BASEADDR_0 : AXI_IIC_BASEADDR_1;
    //enable the I2C mux
    if(enableCommMux)
        ret = I2C_EnableMux_ps7(fmcPort == 0 ? (uint8_t)I2C_LPC_PS7 : 
                                               (uint8_t)I2C_HPC_PS7);

	return ret;
}

/**************************************************************************//**
* @brief Reads data from an I2C slave.
*
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read.
*				   Must be set to -1 if it is not used.
* @param rxSize - Number of bytes to read from the slave.
* @param rxBuf - Buffer to store the read data.
*
* @return Returns the number of bytes read.
******************************************************************************/
uint32_t I2C_Read_ps7(uint32_t i2cAddr, uint32_t regAddr, 
                      uint32_t rxSize, uint8_t* rxBuf)
{
    uint32_t timeout = I2C_TIMEOUT;
    uint32_t cfgValue = 0x00;
    uint32_t rxBufIndex = 0x00;

    // Write the desired register address if required
    if(regAddr != -1)
    {
        I2C_Write_ps7(i2cAddr, regAddr, 0, NULL);
    }
    /* Write to the Control Register to set up SCL Speed and addressing mode
          Set the MS, ACKEN, CLR_FIFO and RW bit */
    cfgValue = (0       << DIV_A)     |
               (0x3f    << DIV_B)     |
               (1       << CLR_FIFO)  |
               (0       << SLVMON)    |
               (0       << HOLD)      |
               (1       << ACK_EN)    |
               (1       << NEA)       |
               (1       << MS)        |
               (1       << RW);
    Xil_Out32(axi_iic_baseaddr + HW_I2C_CONTROL_REG, cfgValue);
    // Clear all Interrupts
    Xil_Out32(axi_iic_baseaddr + HW_I2C_INTR_STATUS_REG, 0xFF);
    // Write the slave address into the I2C address register. This initiates the I2C Transfer.
    Xil_Out32(axi_iic_baseaddr + HW_I2C_ADDRESS_REG, i2cAddr);
    // Write the number of requested bytes
    Xil_Out32(axi_iic_baseaddr + HW_I2C_TX_SIZE_REG, rxSize & 0xFF);
    // Write zeros to TXDATA Register
    Xil_Out32(axi_iic_baseaddr + HW_I2C_DATA_REG, 0x00);
    delay_us(I2C_DELAY);
    // Read data from FIFO
    while(rxBufIndex < rxSize)
    {
    	// Wait for data to be available
    	while (((Xil_In32(axi_iic_baseaddr + HW_I2C_STATUS_REG) & 0x20) == 0x00) && (timeout--));
        if(timeout == - 1)
        {
            return(rxBufIndex);
        }
        timeout = I2C_TIMEOUT;
        
        rxBuf[rxBufIndex] = Xil_In32(axi_iic_baseaddr + HW_I2C_DATA_REG) & 0xFF;
        rxBufIndex += 1;
    }

    return(rxBufIndex);
}

/**************************************************************************//**
* @brief Writes data to an I2C slave.
*
* @param i2cAddr - The address of the I2C slave.
* @param regAddr - Address of the I2C register to be read.
*				   Must be set to -1 if it is not used.
* @param txSize - Number of bytes to write to the slave.
* @param txBuf - Buffer to store the data to be transmitted.
*
* @return Returns the number of bytes written.
******************************************************************************/
uint32_t I2C_Write_ps7(uint32_t i2cAddr, uint32_t regAddr, 
                       uint32_t txSize, uint8_t* txBuf)
{
    uint32_t timeout = I2C_TIMEOUT;
    uint32_t cfgValue = 0x00;
    uint32_t txBufIndex = 0x00;

    /* Write to the Control Register to set up SCL Speed and addressing mode
          Set the MS, ACKEN, CLR_FIFO bits and clear the RW bit */
    cfgValue = (0       << DIV_A)     |
               (0x3f    << DIV_B)     |
               (1       << CLR_FIFO)  |
               (0       << SLVMON)    |
               (0       << HOLD)      |
               (1       << ACK_EN)    |
               (1       << NEA)       |
               (1       << MS)        |
               (0       << RW);
    Xil_Out32(axi_iic_baseaddr + HW_I2C_CONTROL_REG, cfgValue);
    // Clear all Interrupts
    Xil_Out32(axi_iic_baseaddr + HW_I2C_INTR_STATUS_REG, 0xFF);
    // Set the transfer size
    if(regAddr != -1)
    {
        Xil_Out32(axi_iic_baseaddr + HW_I2C_TX_SIZE_REG, txSize + 1);
    }
    else
    {
        Xil_Out32(axi_iic_baseaddr + HW_I2C_TX_SIZE_REG, txSize);
    }
    // Write the first byte of data to the I2C Data Register
    if(regAddr != -1)
    {
        Xil_Out32(axi_iic_baseaddr + HW_I2C_DATA_REG, regAddr);
    }
    else
    {
        Xil_Out32(axi_iic_baseaddr + HW_I2C_DATA_REG, txBuf[txBufIndex]);
        txBufIndex += 1;
    }
    // Write the rest of the data into the FIFO
    while(txBufIndex < txSize)
    {
        Xil_Out32(axi_iic_baseaddr + HW_I2C_DATA_REG, txBuf[txBufIndex]);
        txBufIndex += 1;
    }
    // Write the slave address into the I2C address register. This initiates the I2C Transfer.
    Xil_Out32(axi_iic_baseaddr + HW_I2C_ADDRESS_REG, i2cAddr);
    // Wait for data transmission to be complete
    while(((Xil_In32(axi_iic_baseaddr + HW_I2C_INTR_STATUS_REG) & 0x01) == 0x00) && (--timeout));

    delay_us(I2C_DELAY);

    return(timeout ? txBufIndex : 0);
}
