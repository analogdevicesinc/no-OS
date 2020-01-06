/**
 * \file ad9371/src/devices/adi_hal/common.h
 * \brief Contains type definitions and prototype declarations for common.c
 */

/**
* Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef _COMMON_H_
#define _COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

/* build project settings include the path to the desired platform folder for correct includes */
#include "stdint.h"

#define THROW_ERROR()

#define SPIARRAYSIZE 1024

/* assuming 3 byte SPI message - integer math enforces floor() */
#define SPIARRAYTRIPSIZE ((SPIARRAYSIZE / 3) * 3)

/*========================================
 * Enums and structures
 *=======================================*/
/* proposed to increase number of error return values to make unique for each return */
typedef enum {
	ADIERR_OK=0,
	ADIERR_INV_PARM,
	ADIERR_FAILED
} ADI_ERR;

/*!< \brief COMMON layer error reporting enumerated types */
typedef enum {
	COMMONERR_OK=0,
	COMMONERR_FAILED
} commonErr_t;

/* bit 0 is MESSAGE, bit 1 is WARNING, bit 2 is ERROR */
typedef enum {
	ADIHAL_LOG_NONE	= 0x0,
	ADIHAL_LOG_MESSAGE = 0x1,
	ADIHAL_LOG_WARNING = 0x2,
	ADIHAL_LOG_ERROR   = 0x4,
	ADIHAL_LOG_SPI	 = 0x8,
	ADIHAL_LOG_AXI_REG = 0x10,
	ADIHAL_LOG_AXI_MEM = 0x20,
	ADIHAL_LOG_ALL	 = 0x3F
} ADI_LOGLEVEL;

/**
 * \brief Data structure to hold SPI settings for all system device types
 */
typedef struct {
	uint8_t chipSelectIndex;		///< valid 1~8
	uint8_t writeBitPolarity;		///< the level of the write bit of a SPI write instruction word, value is inverted for SPI read operation
	uint8_t longInstructionWord;	///< 1 = 16bit instruction word, 0 = 8bit instruction word
	uint8_t MSBFirst;				///< 1 = MSBFirst, 0 = LSBFirst
	uint8_t CPHA;					///< clock phase, sets which clock edge the data updates (valid 0 or 1)
	uint8_t CPOL;					///< clock polarity 0 = clock starts low, 1 = clock starts high
	uint8_t enSpiStreaming;			///< Not implemented. SW feature to improve SPI throughput.
	uint8_t autoIncAddrUp;			///< Not implemented. For SPI Streaming, set address increment direction. 1= next addr = addr+1, 0:addr = addr-1
	uint8_t fourWireMode;			///< 1: Use 4-wire SPI, 0: 3-wire SPI (SDIO pin is bidirectional). NOTE: ADI's FPGA platform always uses 4-wire mode.
	uint32_t spiClkFreq_Hz;			///< SPI Clk frequency in Hz (default 25000000), platform will use next lowest frequency that it's baud rate generator can create */
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
commonErr_t CMB_setSPIOptions(spiSettings_t
			      *spiSettings); /* allows the platform HAL to work with devices with various SPI settings */
commonErr_t CMB_setSPIChannel(uint16_t
			      chipSelectIndex );  /* value of 0 deasserts all chip selects */
commonErr_t CMB_SPIWriteByte(spiSettings_t *spiSettings, uint16_t addr,
			     uint8_t data); /* single SPI byte write function */
commonErr_t CMB_SPIWriteBytes(spiSettings_t *spiSettings, uint16_t *addr,
			      uint8_t *data, uint32_t count);
commonErr_t CMB_SPIReadByte (spiSettings_t *spiSettings, uint16_t addr,
			     uint8_t *readdata); /* single SPI byte read function */
commonErr_t CMB_SPIWriteField(spiSettings_t *spiSettings, uint16_t addr,
			      uint8_t  field_val, uint8_t mask,
			      uint8_t start_bit); /* write a field in a single register */
commonErr_t CMB_SPIReadField (spiSettings_t *spiSettings, uint16_t addr,
			      uint8_t *field_val, uint8_t mask,
			      uint8_t start_bit);	/* read a field in a single register */

/* platform timer functions */
commonErr_t CMB_wait_ms(uint32_t time_ms);
commonErr_t CMB_wait_us(uint32_t time_us);
commonErr_t CMB_setTimeout_ms(uint32_t timeOut_ms);
commonErr_t CMB_setTimeout_us(uint32_t timeOut_us);
commonErr_t CMB_hasTimeoutExpired();

/* platform logging functions */
commonErr_t CMB_openLog(const char *filename);
commonErr_t CMB_closeLog(void);
commonErr_t CMB_writeToLog(ADI_LOGLEVEL level, uint8_t deviceIndex,
			   uint32_t errorCode, const char *comment);
commonErr_t CMB_flushLog(void);

/* platform FPGA AXI register read/write functions */
commonErr_t CMB_regRead(uint32_t offset, uint32_t *data);
commonErr_t CMB_regWrite(uint32_t offset, uint32_t data);

/* platform DDR3 memory read/write functions */
commonErr_t CMB_memRead(uint32_t offset, uint32_t *data, uint32_t len);
commonErr_t CMB_memWrite(uint32_t offset, uint32_t *data, uint32_t len);

int32_t platform_init(void);
int32_t platform_remove(void);

#ifdef __cplusplus
}
#endif
#endif
