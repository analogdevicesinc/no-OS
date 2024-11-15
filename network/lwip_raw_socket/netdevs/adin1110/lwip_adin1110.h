/***************************************************************************//**
 *   @file   lwip_adin1110.h
 *   @brief  Header file for the ADIN1110 LWIP implementation.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2023(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _LWIP_ADIN1110_H_
#define _LWIP_ADIN1110_H_

#ifdef NO_OS_LWIP_NETWORKING

#define ADIN1110_LWIP_BUFF_SIZE 2000

extern const struct no_os_lwip_ops adin1110_lwip_ops;

#endif /* NO_OS_LWIP_NETWORKING */
#endif /* _LWIP_ADIN1110_H_ */