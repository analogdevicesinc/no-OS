/***************************************************************************//**
 *   @file   no_os_esh.c
 *   @brief  Implementation of stub functions required by esh.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#include <stdio.h>
#include "no_os_esh.h"
#include "shell.h"

void esh_write_char(char c)
{
	printf("%c", c);
}

int esh_read_char()
{
	int ret;
	char c;

	ret = scanf("%c", &c);
	if (ret < 0)
		return -1;

	return c;
}

void initial_setup()
{
	set_write_char(esh_write_char);
	set_read_char(esh_read_char);
}

void platform_init()
{

}
