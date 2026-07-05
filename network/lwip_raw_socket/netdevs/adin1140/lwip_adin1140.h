/***************************************************************************//**
 *   @file   lwip_adin1140.h
 *   @brief  Header file for the ADIN1140 LWIP implementation.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023-2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#ifndef LWIP_ADIN1140_H_
#define LWIP_ADIN1140_H_

#ifdef NO_OS_LWIP_NETWORKING

#define ADIN1140_LWIP_BUFF_SIZE 2000

extern const struct no_os_lwip_ops adin1140_lwip_ops;

#endif /* NO_OS_LWIP_NETWORKING */
#endif /* LWIP_ADIN1140_H_ */
