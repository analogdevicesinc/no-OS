/**************************************************************************//**
*   @file   spi_interface.h
*   @brief  SPI interfrace header file.
*   @author acozma (andrei.cozma@analog.com)
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

#ifndef __SPI_INTERFACE_H__
#define __SPI_INTERFACE_H__

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/
#include <stdint.h>

/*****************************************************************************/
/************************** Types Declarations *******************************/
/*****************************************************************************/
typedef struct _stDevConfig
{
    uint32_t addrWidth; /*!< Registers address width in bits */
    uint32_t dataWidth; /*!< Data width in bits */
    uint32_t spiConfig; /*!< SPI configuration */
    uint32_t spiCS;     /*!< SPI CS value */
} stDevConfig;

/*****************************************************************************/
/******************* I2C Addesses and SPI settings ***************************/
/*****************************************************************************/
/* IIC addresses */
#define IICSEL_XCOMM         0x20
#define IICSEL_PIC           0x59

/* SPI selections*/
#define SPI_SEL_AD9122       0x00
#define SPI_SEL_AD9643       0x01
#define SPI_SEL_AD9548       0x02
#define SPI_SEL_AD9523       0x03
#define SPI_SEL_ADF4351_RX   0x04
#define SPI_SEL_ADF4351_TX   0x05
#define SPI_SEL_AD8366       0x06

/* PIC control words*/
#define REV_READ  	0x01
#define CTRL_WRITE  0x03
#define DATA_WRITE  0x04

/* PIC Firmware Revision Size */
#define PIC_FW_REV_LEN	32

/* SPI configuration options */
#define SPI_RX_TRANSFER_CNT(x)      ((x) << 10)
#define SPI_3_WIRE_MODE             (1 << 6) 
#define SPI_4_WIRE_MODE             (0 << 6)
#define SPI_CS_HIGH_AT_TRANFER_END  (1 << 5)
#define SPI_CS_LOW_AT_TRANFER_END   (0 << 5) 
#define SPI_SAMPLE_AT_CLK_END       (1 << 4)
#define SPI_SAMPLE_AT_CLK_MIDDLE    (0 << 4)
#define SPI_TX_ON_CLK_FALL          (1 << 3)
#define SPI_TX_ON_CLK_RISE          (0 << 3)
#define SPI_CLK_IDLE_HIGH           (1 << 2)
#define SPI_CLK_IDLE_LOW            (0 << 2)
#define SPI_CLK_INVALID             0x03
#define SPI_CLK_FOSC_DIV_64         0x02
#define SPI_CLK_FOSC_DIV_16         0x01
#define SPI_CLK_FOSC_DIV_4          0x00

/*****************************************************************************/
/************************ Functions Declarations *****************************/
/*****************************************************************************/
/** Reads the PIC firmware version */
int32_t PIC_ReadFwVersion();
/** Initializes the communication with the PIC */
int32_t SPI_Init(); 
/** Reads data from the selected device */
int32_t SPI_Read(uint32_t spiSel, uint32_t regAddr, uint32_t* data); 
/** Writes data to the selected device */
int32_t SPI_Write(uint32_t spiSel, uint32_t regAddr, uint32_t data); 

#endif /* __SPI_INTERFACE_H__ */
