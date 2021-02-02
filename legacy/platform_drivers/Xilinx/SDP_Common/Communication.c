/***************************************************************************//**
 *   @file   Communication.c
 *   @brief  Implementation of Communication Driver for Xilinx FPGAs.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2014(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/

/******************************************************************************/
/************************ Include Files ***************************************/
/******************************************************************************/
#include "Communication.h"
#include "xil_io.h"
#include "TIME.h"

/******************************************************************************/
/************************* Variables Definitions ******************************/
/******************************************************************************/
static u32 configValue = 0;

/******************************************************************************/
/*********************** Functions Definitions ********************************/
/******************************************************************************/

/***************************************************************************//**
 * @brief Define preprocessor directives to activate some board specific
 *        setups, like I2C or SPI ports, or board specific GPIO pins.
 *        Note that this function is local / protected!
 *
 * @param - None
 *
 * @return - None
*******************************************************************************/
void DefinePorts(void)
{
/*********** Custom setups for Digital-to-Analog Converter Boards *************/
#if(defined(AD5541A) || defined(AD5542A))
    /* Activate DAC buffer */
    GPIO2_PIN_OUT;
    GPIO2_LOW;
    /* Deactivate ADC buffer */
    GPIO3_PIN_OUT;
    GPIO3_HIGH;
#endif
#if(defined(AD5443))
    /* Select the SDO line for the device */
    GPIO0_PIN_OUT;
    GPIO0_HIGH;
#endif
#if(defined(AD5570) || defined(AD5453) || defined(AD5446) || defined(AD5443))
    SPI_BASEADDR  = XPAR_AXI_SPI_0_BASEADDR;
#endif
#if(defined(AD5629R) || defined(AD5669R) || defined(AD5694R) || defined(AD5696R))
    I2C_BASEADDR = XPAR_AXI_IIC_1_BASEADDR;
#endif
#if(defined(AD5668) || defined(AD5648) || defined(AD5628))
    /* Activate DAC buffer */
    GPIO2_PIN_OUT;
    GPIO2_LOW;
    /* Deactivate ADC buffer */
    GPIO3_PIN_OUT;
    GPIO3_HIGH;
    /* Deactivate MUX */
    GPIO4_PIN_OUT;
    GPIO4_HIGH;
#endif
#if(defined(AD5686R) || defined(AD5684R))
    GPIO0_PIN_OUT;
    GPIO0_HIGH;
#endif

/**************** Custom setups for Digital Potentiometers ********************/
/* Select the AD8403 SPI line (SYNC-1) as output of SYNC lines MUX */
#if(defined(AD8403))
    /* Define the level of MUX-AO|CS */
    GPIO5_PIN_OUT;
    GPIO5_LOW;
    /* Define the level of MUX-A1|DACSEL */
    GPIO6_PIN_OUT;
    GPIO6_LOW;
    /* Define the level of MUX-A2|U/D */
    GPIO7_PIN_OUT;
    GPIO7_LOW;
#endif
/* Select the AD5162 SPI line (SYNC-4) as output of SYNC lines MUX */
#if(defined(AD5162))
    /* Define the level of MUX-AO|CS */
    GPIO5_PIN_OUT;
    GPIO5_HIGH;
    /* Define the level of MUX-A1|DACSEL */
    GPIO6_PIN_OUT;
    GPIO6_HIGH;
    /* Define the level of MUX-A2|U/D */
    GPIO7_PIN_OUT;
    GPIO7_LOW;
#endif
/* Select the AD5232 SPI line (SYNC-5) as output of SYNC lines MUX */
#if(defined(AD5232))
    /* Define the level of MUX-AO|CS */
    GPIO5_PIN_OUT;
    GPIO5_LOW;
    /* Define the level of MUX-A1|DACSEL */
    GPIO6_PIN_OUT;
    GPIO6_LOW;
    /* Define the level of MUX-A2|U/D */
    GPIO7_PIN_OUT;
    GPIO7_HIGH;
#endif
/* Select the ADN2850 SPI line (SYNC-6) as output of SYNC lines MUX */
#if(defined(ADN2850))
    /* Define the level of MUX-AO|CS */
    GPIO5_PIN_OUT;
    GPIO5_HIGH;
    /* Define the level of MUX-A1|DACSEL */
    GPIO6_PIN_OUT;
    GPIO6_LOW;
    /* Define the level of MUX-A2|U/D */
    GPIO7_PIN_OUT;
    GPIO7_HIGH;
#endif
/* Select the AD5235 SPI line (SYNC-7) as output of SYNC lines MUX */
#if(defined(AD5235))
    /* Define the level of MUX-AO|CS */
    GPIO5_PIN_OUT;
    GPIO5_LOW;
    /* Define the level of MUX-A1|DACSEL */
    GPIO6_PIN_OUT;
    GPIO6_HIGH;
    /* Define the level of MUX-A2|U/D */
    GPIO7_PIN_OUT;
    GPIO7_HIGH;
#endif

#if(defined(AD5270) || defined(AD5271) || defined(AD5272) || defined(AD5274))
    SPI_BASEADDR  = XPAR_AXI_SPI_0_BASEADDR;
#endif
/**************** Custom setups for PLL Synthesizers **************************/
#if(defined(ADF4001) || defined(ADF4002) || defined(ADF4106) || \
    defined(ADF4153) || defined(ADF4156) || defined(ADF4157))
    SPI_BASEADDR  = XPAR_AXI_SPI_0_BASEADDR;
#endif
/**************** Custom setups for Circuits from the Lab *********************/
#if(defined(CN0150) || defined(CN0178) || defined(CN0187) || \
    defined(CN0188) || defined(CN0189) || defined(CN0202) || \
    defined(CN0203) || defined(CN0204) || defined(CN0216) || \
    defined(CN0218) || defined(CN0235) || defined(CN0240) || \
    defined(CN0241) || defined(CN0271))
    SPI_BASEADDR  = XPAR_AXI_SPI_0_BASEADDR;
#endif
#if (defined(CN0235))
    /* Define the output pins referring to AD8280. */
    GPIO0_PIN_OUT;  // TEST_IN
    GPIO0_LOW;
    GPIO3_PIN_OUT;  // ENBL_IN
    GPIO3_LOW;
    /* Define the input pins referring to AD8280. */
    GPIO1_PIN_IN;   // ALRM_OV
    GPIO4_PIN_IN;   // ALRM_UV

#endif
}

/***************************************************************************//**
 * @brief Initializes the selected platform.
 *
 * @return - The result of the initialization.
 *              Example: -1 - the platform was not initialized or the specified
 *                            platform is not present.
 *                        0 - the platform was initialized and the specified
 *                            platform is present.
*******************************************************************************/
char PLATFORM_Init(platformBoard platform)
{
    switch (platform )
    {
     case XILINX_KC705:
#ifdef XPAR_AXI_SPORT_0_DEVICE_ID
        SPI_BASEADDR  = XPAR_AXI_SPORT_0_BASEADDR;
#endif
#ifdef XPAR_AXI_IIC_0_DEVICE_ID
        I2C_BASEADDR  = XPAR_AXI_IIC_0_BASEADDR;
#endif
#ifdef XPAR_AXI_GPIO_0_DEVICE_ID
        GPIO_BASEADDR = XPAR_AXI_GPIO_0_BASEADDR;
#endif
#ifdef XPAR_AXI_PAR_0_DEVICE_ID
        PAR_BASEADDR  = XPAR_AXI_PAR_0_BASEADDR;
#endif
        UART_BAUDRATE = 115200;
        break;
     default :
         xil_printf("Selected platform is not supported\n");
        return -1;
    };

    /* Define and activate board specific setups */
    DefinePorts();

return 0;
}
/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst  - Transfer format (0 or 1).
 *                    Example: 0x0 - MSB first.
 *                             0x1 - LSB first.
 * @param clockFreq - SPI clock frequency (Hz).
 *                    The primary and secondary prescalers have to be modified
 *                    in order to change the SPI clock frequency
 * @param clockPol  - SPI clock polarity (0 or 1).
 *                    Example: 0x0 - Idle state for clock is a low level; active
 *                                   state is a high level;
 *                             0x1 - Idle state for clock is a high level; active
 *                                   state is a low level.
 * @param clockEdg  - SPI clock edge (0 or 1).
 *                    Example: 0x0 - Serial output data changes on transition
 *                                   from idle clock state to active clock
 *                                   state;
 *                             0x1 - Serial output data changes on transition
 *                                   from active clock state to idle clock state
 *
 * @return status   - Result of the initialization procedure.
 *                    Example: 0  - if initialization was successful;
 *                            -1  - if initialization was unsuccessful.
 ******************************************************************************/
char SPI_Init(unsigned char lsbFirst,
              unsigned long clockFreq,
              unsigned char clockPol,
              unsigned char clockEdg)
{
    /* Configuration Register Settings */
    configValue = (lsbFirst    << LSBFirst)         | // MSB First transfer format
                  (1           << MasterTranInh)    | // Master transactions disabled
                  (1           << ManualSlaveAssEn) | // Slave select output follows data in slave select register
                  (1           << RxFifoReset)      | // Receive FIFO normal operation
                  (0           << TxFifoReset)      | // Transmit FIFO normal operation
                  (!clockEdg   << CHPA)             | // Data valid on first SCK edge
                  (clockPol    << CPOL)             | // Active high clock, SCK idles low
                  (1           << Master)           | // SPI in Master configuration mode
                  (1           << SPE)              | // SPI enabled
                  (0           << LOOP);              // Normal operation

    /* Set the slave select register to all ones */
    Xil_Out32(SPI_BASEADDR + SPISSR, 0xFFFFFFFF);

    /* Set corresponding value to the Configuration Register */
    Xil_Out32(SPI_BASEADDR + SPICR, configValue);

    return 0;
}

/***************************************************************************//**
 * @brief Writes multiple bytes to SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data          - Data represents the write buffer.
 * @param bytesNumber   - Number of bytes to write.
 *
 * @return : number of written bytes, if the write was successfulness
             -1,                      if the write was unsuccessful
*******************************************************************************/
char SPI_Write(unsigned char slaveDeviceId,
               unsigned char* data,
               unsigned char bytesNumber)
{
    u32 cfgValue  = configValue;
    u32 SPIStatus = 0;
    u32 rxCnt     = 0;
    u32 txCnt     = 0;
    u32 timeout   = 0xFFFF;

    /* Write configuration data to master SPI device SPICR */
    Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

    /* Write to SPISSR to manually assert SSn */
    Xil_Out32(SPI_BASEADDR + SPISSR, ~(0x00000001 << (slaveDeviceId-1)));

    /* Write initial data to master SPIDTR register */
    Xil_Out32(SPI_BASEADDR + SPIDTR, data[0]);

    /* Enable the master transactions */
    cfgValue &= ~(1 << MasterTranInh);
    Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

    /* Send and receive the data */
    while(txCnt < bytesNumber)
    {
        /* Poll status for completion */
        do
        {
            SPIStatus = Xil_In32(SPI_BASEADDR + SPISR);
        }
        while(((SPIStatus & 0x01) == 1) && timeout--);
        if(timeout == -1)
        {
            /* Disable the master transactions */
            cfgValue |= (1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Reset the SPI core */
            Xil_Out32(SPI_BASEADDR + SRR, 0x0000000A);

            /* Set the slave select register to all ones */
            Xil_Out32(SPI_BASEADDR + SPISSR, 0xFFFFFFFF);

            /* Set corresponding value to the Configuration Register */
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Return error */
            return -1;
        }
        timeout = 0xFFFF;

        /* Read received data from SPI Core buffer */
        if(rxCnt < bytesNumber)
        {
            data[rxCnt] = Xil_In32(SPI_BASEADDR + SPIDRR);
            rxCnt++;
        }
        /* Send next data */
        txCnt++;
        if(txCnt < bytesNumber)
        {
            /* Disable the master transactions */
            cfgValue |= (1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Write data */
            Xil_Out32(SPI_BASEADDR + SPIDTR, data[txCnt]);

            /* Enable the master transactions */
            cfgValue &= ~(1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);
        }
    }

    /* Disable the master transactions */
    cfgValue |= (1 << MasterTranInh);
    Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

    /* Write all ones to SPISSR */
    Xil_Out32(SPI_BASEADDR + SPISSR, 0xFFFFFFFF);

    /* Return the number of bytes written */
    return bytesNumber;
}

/***************************************************************************//**
 * @brief Reads multiple bytes from SPI.
 *
 * @param slaveDeviceId - The ID of the selected slave device.
 * @param data          - Data represents the read buffer.
 * @param bytesNumber   - Number of bytes to read.
 *
 * @return Number of read bytes.
*******************************************************************************/
char SPI_Read(unsigned char slaveDeviceId,
              unsigned char* data,
              unsigned char bytesNumber)
{
    u32 cfgValue  = configValue;
    u32 SPIStatus = 0;
    u32 rxCnt     = 0;
    u32 txCnt     = 0;
    u32 timeout   = 0xFFFF;

    /* Write configuration data to master SPI device SPICR */
    Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

    /* Write to SPISSR to manually assert SSn */
    Xil_Out32(SPI_BASEADDR + SPISSR, ~(0x00000001 << (slaveDeviceId-1)));

    /* Write initial data to master SPIDTR register */
    Xil_Out32(SPI_BASEADDR + SPIDTR, data[0]);

    /* Enable the master transactions */
    cfgValue &= ~(1 << MasterTranInh);
    Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

    /* Send and receive the data */
    while(txCnt < bytesNumber)
    {
        /* Poll status for completion */
        do
        {
            SPIStatus = Xil_In32(SPI_BASEADDR + SPISR);
        }
        while(((SPIStatus & 0x01) == 1) && timeout--);
        if(timeout == -1)
        {
            /* Disable the master transactions */
            cfgValue |= (1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Reset the SPI core */
            Xil_Out32(SPI_BASEADDR + SRR, 0x0000000A);

            /* Set the slave select register to all ones */
            Xil_Out32(SPI_BASEADDR + SPISSR, 0xFFFFFFFF);

            /* Set corresponding value to the Configuration Register */
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Return error */
            return -1;
        }
        timeout = 0xFFFF;

        /* Read received data from SPI Core buffer */
        if(rxCnt < bytesNumber)
        {
            data[rxCnt] = Xil_In32(SPI_BASEADDR + SPIDRR);
            rxCnt++;
        }
        /* Send next data */
        txCnt++;
        if(txCnt < bytesNumber)
        {
            /* Disable the master transactions */
            cfgValue |= (1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

            /* Write data */
            Xil_Out32(SPI_BASEADDR + SPIDTR, data[txCnt]);

            /* Enable the master transactions */
            cfgValue &= ~(1 << MasterTranInh);
            Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);
        }
   }

   /* Disable the master transactions */
   cfgValue |= (1 << MasterTranInh);
   Xil_Out32(SPI_BASEADDR + SPICR, cfgValue);

   /* Write all ones to SPISSR */
   Xil_Out32(SPI_BASEADDR + SPISSR, 0xFFFFFFFF);

   /* Return the number of bytes read */
   return bytesNumber;
}

/***************************************************************************//**
 * @brief Initializes the UART communication peripheral. If the value of the
 *          baud rate is not equal with the ipcore's baud rate the
 *          initialization going to FAIL.
 *
 * @param baudRate - Baud rate value.
 *                   Example: 9600 - 9600 bps.
 *
 * @return status  - Result of the initialization procedure.
 *                   Example: 0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
 *
*******************************************************************************/
char UART_Init(unsigned long baudRate)
{
    /* Disable interrupt and reset the Rx and Tx FIFO */
    Xil_Out32(UART_CNTRL,  ( UART_RST_TX | \
                             UART_RST_RX | \
                             ~UART_EN_INTR) );

    return 0;
}

/***************************************************************************//**
 * @brief Writes one character to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void UART_WriteChar(char data)
{
    /* Wait the Tx FIFO empty indication */
    while( (Xil_In32(UART_STAT) & 0x4) == 0x0);
    /* Put the data to the FIFO */
    Xil_Out32(UART_TX, data);
    /* Wait the Tx FIFO empty indication */
    while( (Xil_In32(UART_STAT) & 0x4) == 0x0);
}

/***************************************************************************//**
 * @brief Reads one character from UART.
 *
 * @return receivedChar    - Read character.
 *
 * Note: Blocking function - waits until get a valid data
*******************************************************************************/
void UART_ReadChar(char * data)
{
    /* Wait for a valid data */
    while((Xil_In32(UART_STAT) & UART_RX_VALID) == 0x00);
    /* Read the character */
    *data = (char)(Xil_In32(UART_RX) & 0xFF);
    /* Flush the Rx FIFO at carriage return */
    if((*data == '\n') || (*data == '\r'))
    {
        Xil_Out32(UART_CNTRL, UART_RST_RX | UART_RST_TX);
    }
}

/***************************************************************************//**
 * @brief Writes one character string to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void UART_WriteString(const char* string)
{
    while(*string)
    {
        UART_WriteChar(*string++);
    }
}

/***************************************************************************//**
 * @brief Initializes the I2C communication peripheral.
 *
 * @param clockFreq - I2C clock frequency (Hz).
 *                    Example: 100000 - I2C clock frequency is 100 kHz.
 * @return status   - Result of the initialization procedure.
 *                    Example:  0 - if initialization was successful;
 *                             -1 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char I2C_Init(unsigned long clockFreq)
{
    /* Disable the I2C core */
    Xil_Out32((I2C_BASEADDR + CR), 0x00);
    /* Set the Rx FIFO depth to maximum */
    Xil_Out32((I2C_BASEADDR + RX_FIFO_PIRQ), 0x0F);
    /* Reset the I2C core and flush the Tx fifo */
    Xil_Out32((I2C_BASEADDR + CR), 0x02);
    /* Enable the I2C core */
    Xil_Out32((I2C_BASEADDR + CR), 0x01);

    return 0;
}

/***************************************************************************//**
 * @brief Reads data from a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer   - Pointer to a buffer that will store the received data.
 * @param bytesNumber  - Number of bytes to read.
 * @param stopBit      - Stop condition control.
 *                       Example: 0 - A stop condition will not be sent;
 *                                1 - A stop condition will be sent.
 *
 * @return status      - Number of read bytes or 0xFF if the slave address was not
 *                       acknowledged by the device.
*******************************************************************************/
unsigned char I2C_Read(unsigned char slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char bytesNumber,
                       unsigned char stopBit)
{
    u32 rxCnt = 0;
    u32 timeout = 0xFFFFFF;

    /* Reset tx fifo */
    Xil_Out32((I2C_BASEADDR + CR), 0x002);
    /* Enable iic */
    Xil_Out32((I2C_BASEADDR + CR), 0x001);
    TIME_DelayMs(10);

    /* Set the slave I2C address */
    Xil_Out32((I2C_BASEADDR + TX_FIFO), (0x101 | (slaveAddress << 1)));
    /* Start a read transaction */
    Xil_Out32((I2C_BASEADDR + TX_FIFO), 0x200 + bytesNumber);

    /* Read data from the I2C slave */
    while(rxCnt < bytesNumber)
    {
        /* Wait for data to be available in the RxFifo */
        while((Xil_In32(I2C_BASEADDR + SR) & 0x00000040) && (timeout--));
        if(timeout == -1)
        {
            /* Disable the I2C core */
            Xil_Out32((I2C_BASEADDR + CR), 0x00);
            /* Set the Rx FIFO depth to maximum */
            Xil_Out32((I2C_BASEADDR + RX_FIFO_PIRQ), 0x0F);
            /* Reset the I2C core and flush the Tx fifo */
            Xil_Out32((I2C_BASEADDR + CR), 0x02);
            /* Enable the I2C core */
            Xil_Out32((I2C_BASEADDR + CR), 0x01);
            return rxCnt;
        }
        timeout = 0xFFFFFF;

        /* Read the data */
        dataBuffer[rxCnt] = Xil_In32(I2C_BASEADDR + RX_FIFO) & 0xFFFF;

        /* Increment the receive counter */
        rxCnt++;
    }

    TIME_DelayMs(10);

    return rxCnt;
}

/***************************************************************************//**
 * @brief Writes data to a slave device.
 *
 * @param slaveAddress - Adress of the slave device.
 * @param dataBuffer   - Pointer to a buffer storing the transmission data.
 * @param bytesNumber  - Number of bytes to write.
 * @param stopBit      - Stop condition control.
 *                       Example: 0 - A stop condition will not be sent;
 *                                1 - A stop condition will be sent.
 *
 * @return status      - Number of read bytes or 0xFF if the slave address was not
 *                       acknowledged by the device.
*******************************************************************************/
unsigned char I2C_Write(unsigned char slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char bytesNumber,
                        unsigned char stopBit)
{
    u32 txCnt = 0;

    /* Reset tx fifo */
    Xil_Out32((I2C_BASEADDR + CR), 0x002);
    /* Enable iic */
    Xil_Out32((I2C_BASEADDR + CR), 0x001);
    TIME_DelayMs(10);

    /* Set the I2C address */
    Xil_Out32((I2C_BASEADDR + TX_FIFO), (0x100 | (slaveAddress << 1)));

    /* Write data to the I2C slave */
    while(txCnt < bytesNumber)
    {
        /* Put the Tx data into the Tx FIFO */
        Xil_Out32((I2C_BASEADDR + TX_FIFO), (txCnt == bytesNumber - 1) ?
                        (0x200 | dataBuffer[txCnt]) : dataBuffer[txCnt]);
        txCnt++;
    }
    TIME_DelayMs(10);

    return txCnt;
}
