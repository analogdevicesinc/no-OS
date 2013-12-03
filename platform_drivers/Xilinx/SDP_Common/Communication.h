/***************************************************************************//**
 *   @file   Communication.h
 *   @brief  Header file of Communication Driver.
 *   @author Istvan Csomortani (istvan.csomortani@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
******************************************************************************/
#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

/******************************************************************************/
/************************ Include Files ***************************************/
/******************************************************************************/
#include "xil_types.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_cache.h"

/*****************************************************************************/
/******************* Macros and Constants Definitions ************************/
/*****************************************************************************/
/* DEVICE MACROS : Please uncomment the '#define' with the desired device name,
 * before compilation */
/*****************************************************************************/
/************************** Digital to Analog Converters *********************/
/*****************************************************************************/
//#define AD5415
//#define AD5443
//#define AD5449
//#define AD5421
//#define AD5425
//#define AD5541A
//#define AD5542A
//#define AD5446
//#define AD5453
//#define AD5543
//#define AD5553
//#define AD5629R
//#define AD5669R
//#define AD5668
//#define AD5648
//#define AD5628
//#define AD5684R
//#define AD5686R
//#define AD5694R
//#define AD5696R
//#define AD5755
//#define AD5755_1
//#define AD5757
//#define AD5760
//#define AD5780
//#define AD5781
//#define AD5790
//#define AD5791
//#define AD5570

/*****************************************************************************/
/************************** Digital Potentiometers ***************************/
/*****************************************************************************/
//#define AD5162
//#define AD5172
//#define AD5173
//#define AD5232
//#define AD5235
//#define AD5252
//#define AD5251
//#define AD5254
//#define AD5253
//#define AD5270
//#define AD5271
//#define AD5272
//#define AD5274
//#define AD8403
//#define AD8402
//#define AD8400
//#define ADN2850
//#define AD5110
//#define AD5111

/*****************************************************************************/
/******************* Direct Digital Synthesis & Modulators *******************/
/*****************************************************************************/
//#define AD9833
//#define AD9834
//#define AD9837
//#define AD9838

/*****************************************************************************/
/************************* PLL Synthesizers / VCOs ***************************/
/*****************************************************************************/
//#define ADF4001
//#define ADF4002
//#define ADF4106
//#define ADF4153
//#define ADF4156
//#define ADF4157

/*****************************************************************************/
/**************** Synchro/Resolver to Digital Converters *********************/
/*****************************************************************************/
//#define AD2S1205

/*****************************************************************************/
/************************** Circuits from the Lab ****************************/
/*****************************************************************************/
//#define CN0150
//#define CN0178
//#define CN0187
//#define CN0188
//#define CN0189
//#define CN0194
//#define CN0202
//#define CN0203
//#define CN0204
//#define CN0209
//#define CN0216
//#define CN0218
//#define CN0235
//#define CN0240
//#define CN0241
//#define CN0271

/*****************************************************************************/
/**************************** UART Definitions *******************************/
/*****************************************************************************/
#define UART_BASEADDR       XPAR_RS232_UART_1_BASEADDR
#define UART_DEV_ID         XPAR_RS232_UART_1_DEVICE_ID

/* UART registers*/
#define UART_RX         UART_BASEADDR + 0x0
#define UART_TX         UART_BASEADDR + 0x4
#define UART_STAT       UART_BASEADDR + 0x8
#define UART_CNTRL      UART_BASEADDR + 0xC

/* UART Control Register */
#define UART_RST_TX         (1 << 0x0)
#define UART_RST_RX         (1 << 0x1)
#define UART_EN_INTR        (1 << 0x4)

/* UART Status Register */
#define UART_RX_VALID       (1 << 0x0)
#define UART_RX_FULL        (1 << 0x1)
#define UART_TX_EMPTY       (1 << 0x2)
#define UART_TX_FULL        (1 << 0x3)
#define UART_INTR_EN        (1 << 0x4)
#define UART_OVER_RUN       (1 << 0x5)
#define UART_FRM_ERR        (1 << 0x6)
#define UART_PRT_ERR        (1 << 0x7)

/*****************************************************************************/
/***************************** SPI Definitions *******************************/
/*****************************************************************************/
/* Register address */
#define SRR            0x40
#define SPICR          0x60
#define SPISR          0x64
#define SPIDTR         0x68
#define SPIDRR         0x6C
#define SPISSR         0x70
#define SPI_T_FIFO     0x74
#define SPI_R_FIFO     0x78
#define DGIER          0x1C
#define IPISR          0x20
#define IPIER          0x28

/* SPI Control Register (SPICR) */
#define LSBFirst            9
#define MasterTranInh       8
#define ManualSlaveAssEn    7
#define RxFifoReset         6
#define TxFifoReset         5
#define CHPA                4
#define CPOL                3
#define Master              2
#define SPE                 1
#define LOOP                0

/* SPI Status Register (SPISR) */
#define SlaveModeSel        5
#define MODF                4
#define TxFull              3
#define TxEmpty             2
#define RxFull              1
#define RxEmpty             0

/* IP Interrupt Status Register */
#define DDRNotEmpty         8
#define SlaveModeSel_int    7
#define TxFifoHalfEmpty     6
#define DDROverRun          5
#define DDRFull             4
#define DTRUnderRun         3
#define DTREmpty            2
#define SlaveMODF           1
#define MODF_int            0

/*****************************************************************************/
/*********************** I2C Registers Definitions ***************************/
/*****************************************************************************/
#define GIE            0x01C
#define ISR            0x020
#define IER            0x028
#define SOFTR          0x040
#define CR             0x100
#define SR             0x104
#define TX_FIFO        0x108
#define RX_FIFO        0x10C
#define ADR            0x110
#define TX_FIFO_OCY    0x114
#define RX_FIFO_OCY    0x118
#define TEN_ADDR       0x11C
#define RX_FIFO_PIRQ   0x120
#define GPO            0x124

/*****************************************************************************/
/*********************** GPIO Registers Definitions **************************/
/*****************************************************************************/
#define GPIO_DATA      0x00
#define GPIO_TRI       0x04

/****************************************************************************/
/********************** GPIO macros redefinitions ***************************/
/****************************************************************************/
#define GPIO_SET_PINS_OUT(GPIO_BASEADDR, cfgWord) \
{ \
  int currCfgWord = 0; \
  currCfgWord = Xil_In32((GPIO_BASEADDR + GPIO_TRI)); \
  Xil_Out32((GPIO_BASEADDR + GPIO_TRI), (currCfgWord & ~cfgWord)); \
}
#define GPIO_SET_PINS_IN(GPIO_BASEADDR, cfgWord) \
{ \
  int currCfgWord = 0; \
  currCfgWord = Xil_In32((GPIO_BASEADDR + GPIO_TRI)); \
  Xil_Out32((GPIO_BASEADDR + GPIO_TRI), (currCfgWord | cfgWord)); \
}
#define GPIO_CLEAR_BITS(GPIO_BASEADDR, cfgWord) \
{ \
  int currCfgWord = 0; \
  currCfgWord = Xil_In32((GPIO_BASEADDR + GPIO_DATA)); \
  Xil_Out32((GPIO_BASEADDR + GPIO_DATA), currCfgWord & ~cfgWord); \
}
#define GPIO_SET_BITS(GPIO_BASEADDR, cfgWord) \
{ \
  int currCfgWord = 0; \
  currCfgWord = Xil_In32((GPIO_BASEADDR + GPIO_DATA)); \
  Xil_Out32((GPIO_BASEADDR + GPIO_DATA), currCfgWord | cfgWord); \
}
#define GPIO_GET_DATA(GPIO_BASE_ADDR) \
	Xil_In32((GPIO_BASE_ADDR + GPIO_DATA))

#define GPIO_SET_DATA(GPIO_BASE_ADDR, cfgWord) \
	Xil_Out32((GPIO_BASE_ADDR + GPIO_DATA), cfgWord);

#define GET_REGISTER(AXI_REG_ADDR) \
  Xil_In32((AXI_REG_ADDR))

#define SET_REGISTER(AXI_REG_ADDR, AXI_REG_DATA) \
  Xil_Out32((AXI_REG_ADDR), (AXI_REG_DATA))

#define FLUSH_CACHE() \
{ \
  microblaze_flush_dcache(); \
  microblaze_invalidate_dcache(); \
}

#if(defined(AD5541A) || defined(AD5542A))
#define GPIO_0_MASK   0x02
#define GPIO_1_MASK   0x01
#define GPIO_2_MASK   0x04
#define GPIO_3_MASK   0x08
#else
#if (defined(AD5760) || defined(AD5780) || defined(AD5781) || defined(AD5790) || defined(AD5791))
#define GPIO_2_MASK   0x04
#define GPIO_3_MASK   0x02
#define GPIO_4_MASK   0x01
#else
#define GPIO_0_MASK   0x01
#define GPIO_1_MASK   0x02
#define GPIO_2_MASK   0x04
#define GPIO_3_MASK   0x08
#define GPIO_4_MASK   0x10
#define GPIO_5_MASK   0x20
#define GPIO_6_MASK   0x40
#define GPIO_7_MASK   0x80
#endif
#endif

/* GPIO[0] */
#define GPIO0_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_0_MASK)
#define GPIO0_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_0_MASK)
#define GPIO0_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_0_MASK)
#define GPIO0_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_0_MASK)

/* GPIO[1] */
#define GPIO1_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_1_MASK)
#define GPIO1_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_1_MASK)
#define GPIO1_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_1_MASK)
#define GPIO1_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_1_MASK)

/* GPIO[2] */
#define GPIO2_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_2_MASK)
#define GPIO2_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_2_MASK)
#define GPIO2_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_2_MASK)
#define GPIO2_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_2_MASK)

/* GPIO[3] */
#define GPIO3_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_3_MASK)
#define GPIO3_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_3_MASK)
#define GPIO3_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_3_MASK)
#define GPIO3_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_3_MASK)

/* GPIO[4] */
#define GPIO4_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_4_MASK)
#define GPIO4_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_4_MASK)
#define GPIO4_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_4_MASK)
#define GPIO4_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_4_MASK)

/* GPIO[5] */
#define GPIO5_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_5_MASK)
#define GPIO5_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_5_MASK)
#define GPIO5_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_5_MASK)
#define GPIO5_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_5_MASK)

/* GPIO[6] */
#define GPIO6_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_6_MASK)
#define GPIO6_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_6_MASK)
#define GPIO6_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_6_MASK)
#define GPIO6_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_6_MASK)

/* GPIO[7] */
#define GPIO7_PIN_OUT   GPIO_SET_PINS_OUT(GPIO_BASEADDR, GPIO_7_MASK)
#define GPIO7_PIN_IN    GPIO_SET_PINS_IN(GPIO_BASEADDR, GPIO_7_MASK)
#define GPIO7_LOW       GPIO_CLEAR_BITS(GPIO_BASEADDR, GPIO_7_MASK)
#define GPIO7_HIGH      GPIO_SET_BITS(GPIO_BASEADDR, GPIO_7_MASK)

/*****************************************************************************/
/*************************** Types Declarations ******************************/
/*****************************************************************************/
typedef enum
{
    XILINX_KC705 = 1
}platformBoard;

/*****************************************************************************/
/************************* Variables Declarations ****************************/
/*****************************************************************************/
u32 SPI_BASEADDR;
u32 I2C_BASEADDR;
u32 GPIO_BASEADDR;
u32 PAR_BASEADDR;
u32 UART_BAUDRATE;

/*****************************************************************************/
/************************* Functions Declarations ****************************/
/*****************************************************************************/
/* Initializes the selected platform. */
char PLATFORM_Init(platformBoard);

/* Initializes the SPI communication peripheral. */
char SPI_Init(unsigned char lsbFirst,
              unsigned long clockFreq,
              unsigned char clockPol,
              unsigned char clockEdg);

/* Writes multiple bytes to SPI. */
char SPI_Write(unsigned char   slaveDeviceId,
               unsigned char*  data,
               unsigned char   bytesNumber);

/* Reads multiple bytes from SPI. */
char SPI_Read(unsigned char   slaveDeviceId,
              unsigned char*  data,
              unsigned char   bytesNumber);

/* Initializes the UART communication peripheral. */
char UART_Init(unsigned long baudRate);

/* Reads one character from UART.. */
void UART_ReadChar(char* data);

/* Writes one character to UART. */
void UART_WriteChar(char data);

/* Writes one character string to UART. */
void UART_WriteString(const char* string);

/* Initializes the I2C communication peripheral. */
unsigned char I2C_Init(unsigned long clockFreq);

/* Reads data from a slave device. */
unsigned char I2C_Read(unsigned char  slaveAddress,
                       unsigned char* dataBuffer,
                       unsigned char  bytesNumber,
                       unsigned char  stopBit);

/* Writes data to a slave device. */
unsigned char I2C_Write(unsigned char  slaveAddress,
                        unsigned char* dataBuffer,
                        unsigned char  bytesNumber,
                        unsigned char  stopBit);

#endif  /*_COMMUNICATION_H_*/
