/***************************************************************************//**
 *   @file   display_demo/src/app/parameters.h
 *   @brief  Parameters Definitions.
 *   @author Andrei Porumb (andrei.porumb@analog.com)
********************************************************************************
 * Copyright 2021(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PARAMETERS_H
#define PARAMETERS_H

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
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
