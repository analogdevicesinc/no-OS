// ***************************************************************************
// ***************************************************************************
// Copyright 2011(c) Analog Devices, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//     - Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     - Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in
//       the documentation and/or other materials provided with the
//       distribution.
//     - Neither the name of Analog Devices, Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//     - The use of this software may or may not infringe the patent rights
//       of one or more patent holders.  This license does not release you
//       from the requirement that you obtain separate licenses from these
//       patent holders to use this software.
//     - Use of the software either in source or binary form, must be run
//       on or directly connected to an Analog Devices Inc. component.
//
// THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED.
//
// IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
// RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
// THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// ***************************************************************************
// ***************************************************************************

#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdint.h"

#define AD9528_RESET_B      59
#define AD9528_SYSREF_REQ   58
#define AD9371_TX1_ENABLE   57
#define AD9371_TX2_ENABLE   56
#define AD9371_RX1_ENABLE   55
#define AD9371_RX2_ENABLE   54
#define AD9371_TEST         53
#define AD9371_RESET_B      52
#define AD9371_GPINT        51
#define AD9371_GPIO_00      50
#define AD9371_GPIO_01      49
#define AD9371_GPIO_02      48
#define AD9371_GPIO_03      47
#define AD9371_GPIO_04      46
#define AD9371_GPIO_05      45
#define AD9371_GPIO_06      44
#define AD9371_GPIO_07      43
#define AD9371_GPIO_15      42
#define AD9371_GPIO_08      41
#define AD9371_GPIO_09      40
#define AD9371_GPIO_10      39
#define AD9371_GPIO_11      38
#define AD9371_GPIO_12      37
#define AD9371_GPIO_14      36
#define AD9371_GPIO_13      35
#define AD9371_GPIO_17      34
#define AD9371_GPIO_16      33
#define AD9371_GPIO_18      32

#define SPIARRAYSIZE 1024
#define SPIARRAYTRIPSIZE ((SPIARRAYSIZE / 3) * 3)

/* all fancy software stuff */

typedef enum
{
  ADIERR_OK=0,
  ADIERR_INV_PARM,
  ADIERR_FAILED
} ADI_ERR;

typedef enum
{
  COMMONERR_OK=0,
  COMMONERR_FAILED
} commonErr_t;

typedef enum
{
  ADIHAL_LOG_NONE    = 0x0,
  ADIHAL_LOG_MESSAGE = 0x1,
  ADIHAL_LOG_WARNING = 0x2,
  ADIHAL_LOG_ERROR   = 0x4,
  ADIHAL_LOG_SPI     = 0x8,
  ADIHAL_LOG_AXI_REG = 0x10,
  ADIHAL_LOG_AXI_MEM = 0x20,
  ADIHAL_LOG_ALL     = 0x3F
} ADI_LOGLEVEL;

/* spi settings */

typedef struct
{
  uint8_t chipSelectIndex;        // system chip-select bus (or index) of ad9371
  uint8_t writeBitPolarity;       // must be 0x0 for write (inverted for read, so read msb is 0x1)
  uint8_t longInstructionWord;    // must be 0x1 for 16bit addresses (not sure if api ever does 8 bit addressing)
  uint8_t MSBFirst;               // must be 0x1 for msb first (forget about lsb-first)
  uint8_t CPHA;                   // set it to 0x0, you may override in your actual function call
  uint8_t CPOL;                   // set it to 0x0, you may override in your actual function call
  uint8_t enSpiStreaming;         // must be 0x0 (not sure if api does it-- allows a bunch of writes/reads)
  uint8_t autoIncAddrUp;          // must be 0x0 (probably ignored because no streaming is supported)
  uint8_t fourWireMode;           // set it to 0x1, unless you are using 3 wire mode
  uint32_t spiClkFreq_Hz;         // set it to the spi frequency
} spiSettings_t;

/* global variable so application layer can set the log level */

extern ADI_LOGLEVEL CMB_LOGLEVEL;

/* close hardware pointers */

commonErr_t CMB_closeHardware(void);

/* GPIO function */

commonErr_t CMB_setGPIO(uint32_t GPIO);

/* hardware reset function */

commonErr_t CMB_hardReset(uint8_t spiChipSelectIndex);

/* SPI read/write functions */

commonErr_t CMB_setSPIOptions(spiSettings_t *spiSettings);
commonErr_t CMB_setSPIChannel(uint16_t chipSelectIndex );
commonErr_t CMB_SPIWriteByte(spiSettings_t *spiSettings, uint16_t addr, uint8_t data);
commonErr_t CMB_SPIWriteBytes(spiSettings_t *spiSettings, uint16_t *addr, uint8_t *data, uint32_t count);
commonErr_t CMB_SPIReadByte (spiSettings_t *spiSettings, uint16_t addr, uint8_t *readdata);
commonErr_t CMB_SPIWriteField(spiSettings_t *spiSettings, uint16_t addr, uint8_t  field_val, uint8_t mask, uint8_t start_bit);
commonErr_t CMB_SPIReadField (spiSettings_t *spiSettings, uint16_t addr, uint8_t *field_val, uint8_t mask, uint8_t start_bit);

/* platform timer functions */

commonErr_t CMB_wait_ms(uint32_t time_ms);
commonErr_t CMB_wait_us(uint32_t time_us);
commonErr_t CMB_setTimeout_ms(uint32_t timeOut_ms);
commonErr_t CMB_setTimeout_us(uint32_t timeOut_us);
commonErr_t CMB_hasTimeoutExpired();

/* platform logging functions */

commonErr_t CMB_openLog(const char *filename);
commonErr_t CMB_closeLog(void);
commonErr_t CMB_writeToLog(ADI_LOGLEVEL level, uint8_t deviceIndex, uint32_t errorCode, const char *comment);
commonErr_t CMB_flushLog(void);

/* platform FPGA AXI register read/write functions */

commonErr_t CMB_regRead(uint32_t offset, uint32_t *data);
commonErr_t CMB_regWrite(uint32_t offset, uint32_t data);

/* platform DDR3 memory read/write functions */

commonErr_t CMB_memRead(uint32_t offset, uint32_t *data, uint32_t len);
commonErr_t CMB_memWrite(uint32_t offset, uint32_t *data, uint32_t len);

#endif

// ***************************************************************************
// ***************************************************************************
