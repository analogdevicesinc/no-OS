/***************************************************************************//**
 *   @file   display_demo/src/app/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef PARAMETERS_H
#define PARAMETERS_H

#define SPI_DEVICE_ID 0U
#define GPIO_OFFSET   54U

/* SSD 1306 display controller specific pins */
#define DC_PIN        GPIO_OFFSET + 5U
#define RST_PIN       GPIO_OFFSET + 6U
#define VBAT_PIN      GPIO_OFFSET + 9U
#define VDD_PIN       GPIO_OFFSET + 10U

/* Defines for ZedBoard specific pin level output */
#define VDD_ON     0U   //p-channel MOSFET
#define VDD_OFF    1U
#define VBAT_ON    0U   //p-channel MOSFET
#define VBAT_OFF   1U

#endif
