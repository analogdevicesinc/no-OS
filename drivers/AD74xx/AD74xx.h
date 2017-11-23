/***************************************************************************//**
 *   @file   AD74xx.h
 *   @brief  Header file of AD74xx Driver.
 *   @author DNechita(Dan.Nechita@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
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
********************************************************************************
 *   SVN Revision: $WCREV$
*******************************************************************************/

#ifndef __AD74XX_H__
#define __AD74XX_H__

/******************************************************************************/
/******************************** AD74XX **************************************/
/******************************************************************************/

/* AD74XX Chip Select Pin declaration */
#define AD74XX_CS_LOW           gpio_set_value(dev->gpio_cs,  \
			        GPIO_LOW)
#define AD74XX_CS_HIGH          gpio_set_value(dev->gpio_cs,  \
			        GPIO_HIGH)

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/* Supported devices */
typedef enum {
	ID_AD7466  = 12,
	ID_AD7467  = 10,
	ID_AD7468  = 8,
	ID_AD7475  = 12,
	ID_AD7476  = 12,
	ID_AD7476A = 12,
	ID_AD7477  = 10,
	ID_AD7477A = 10,
	ID_AD7478  = 8,
	ID_AD7478A = 8,
	ID_AD7495  = 12
} ad74xx_type;

typedef struct {
	/* SPI */
	spi_desc	*spi_desc;
	/* GPIO */
	gpio_desc	*gpio_cs;
	/* Device Settings */
	char		deviceBitsNumber;
	ad74xx_type	partNumber;
} ad74xx_dev;

typedef struct {
	/* SPI */
	spi_init_param	spi_init;
	/* GPIO */
	uint8_t		gpio_cs;
	/* Device Settings */
	char		deviceBitsNumber;
	ad74xx_type	partNumber;
} ad74xx_init_param;

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes device. */
char AD74xx_Init(ad74xx_dev **device,
		 ad74xx_init_param init_param);

/*! Free the resources allocated by AD74xx_Init(). */
int32_t AD74xx_remove(ad74xx_dev *dev);

/*! Powers down the device. */
void AD74xx_PowerDown(ad74xx_dev *dev);

/*! Powers up the device by performing a dummy read. */
void AD74xx_PowerUp(ad74xx_dev *dev);

/*! Reads the conversion value. */
unsigned short AD74xx_GetRegisterValue(ad74xx_dev *dev);

/*! Converts a raw sample to volts. */
float AD74xx_ConvertToVolts(ad74xx_dev *dev,
			    unsigned short rawValue,
			    float vRef);

#endif /* __AD74XX_H__ */
