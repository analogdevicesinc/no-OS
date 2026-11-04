/***************************************************************************//**
 *   @file   example.h
 *   @author Niel Acuna (niel.acuna@analog.com)
 *   @author Marc Paolo Sosa (MarcPaolo.Sosa@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ******************************************************************************/
#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#define example_main(fn) \
	int start_example(void) __attribute__ ((alias(fn)))

int start_example(void);

int heartbeat_init(void (*cb)(void *context));
void heartbeat_exit(void);

#endif /* EXAMPLE_H_*/
