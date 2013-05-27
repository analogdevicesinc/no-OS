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
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"

/******************************************************************************/
/******************************** AD74XX **************************************/
/******************************************************************************/

/* Supported devices. */
#define AD7466          12
#define AD7467          10
#define AD7468          8
#define AD7475          12
#define AD7476          12
#define AD7476A         12
#define AD7477          10
#define AD7477A         10
#define AD7478          8
#define AD7478A         8
#define AD7495          12

/* SPI slave device ID */
#define AD74XX_SLAVE_ID         1

/* AD74XX Chip Select Pin declaration */
#define AD74XX_CS_LOW           SPI_CS_LOW
#define AD74XX_CS_HIGH          SPI_CS_HIGH

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/*! Initializes device. */
char AD74xx_Init(char partNumber);

/*! Powers down the device. */
void AD74xx_PowerDown(void);

/*! Powers up the device by performing a dummy read. */
void AD74xx_PowerUp(void);

/*! Reads the conversion value. */
unsigned short AD74xx_GetRegisterValue(void);

/*! Converts a raw sample to volts. */
float AD74xx_ConvertToVolts(unsigned short rawValue, float vRef);

#endif /* __AD74XX_H__ */
