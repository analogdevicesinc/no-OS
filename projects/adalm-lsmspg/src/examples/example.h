/***************************************************************************//**
 *   @file   example.h
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifndef __EXAMPLE_H__
#define __EXAMPLE_H__

#define example_main(fn) \
	int start_example(void) __attribute__ ((alias(fn)))

int start_example(void);

int heartbeat_init(void (*cb)(void *context));
void heartbeat_exit(void);

#endif /* __EXAMPLE_H__*/
