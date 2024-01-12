/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef SPI_H
#define SPI_H

	#include "derivative.h"
	#include "IOs.h"

	typedef struct
	{
		#if defined(Landungsbruecke) || defined(LandungsbrueckeSmall)
			SPI_MemMapPtr periphery; // pointer to freescale SPI memory base pointer
		#elif defined(LandungsbrueckeV3)
			uint32_t periphery; // GD32 SPI parameters
		#endif

		IOPinTypeDef *CSN;
		unsigned char (*readWrite) (unsigned char data, unsigned char lastTransfer);
		void (*readWriteArray) (uint8_t *data, size_t length);
		void (*reset) (void);
	} SPIChannelTypeDef;

	typedef struct
	{
		SPIChannelTypeDef ch1;
		SPIChannelTypeDef ch2;
		void (*init) (void);
	} SPITypeDef;

	extern SPITypeDef SPI;

	uint32_t spi_getFrequency(SPIChannelTypeDef *SPIChannel);
	uint32_t spi_setFrequency(SPIChannelTypeDef *SPIChannel, uint32_t desiredFrequency);

	uint8_t spi_getMode(SPIChannelTypeDef *SPIChannel);
	bool spi_setMode(SPIChannelTypeDef *SPIChannel, uint8_t mode);

	// read/write 32 bit value at address
	int32_t spi_readInt(SPIChannelTypeDef *SPIChannel, uint8_t address);
	void spi_writeInt(SPIChannelTypeDef *SPIChannel, uint8_t address, int32_t value);

	// for default channels
	uint8_t spi_ch1_readWriteByte(uint8_t data, uint8_t lastTransfer);
	// for TM02
	uint8_t spi_ch2_readWriteByte(SPIChannelTypeDef *SPIChannel, uint8_t data, uint8_t lastTransfer);


	int32_t spi_ch1_readInt(uint8_t address);
	void spi_ch1_writeInt(uint8_t address, int32_t value);

	int32_t spi_ch2_readInt(uint8_t address);
	void spi_ch2_writeInt(uint8_t address, int32_t value);

#endif /* SPI_H */
