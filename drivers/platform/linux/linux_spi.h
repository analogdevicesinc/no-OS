/*******************************************************************************
 *   @file   linux/linux_spi.h
 *   @brief  Header containing extra types and spi_platform_ops used by the
 *           SPI driver.
 *   @author Dragos Bogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef LINUX_SPI_H_
#define LINUX_SPI_H_

/**
 * @brief Linux specific SPI platform ops structure
 */
extern const struct no_os_spi_platform_ops linux_spi_ops;

#endif // LINUX_SPI_H_
