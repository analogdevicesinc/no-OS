/***************************************************************************//**
 *   @file   common.h
 *   @brief  Header file of Common Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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
*******************************************************************************/
#ifndef COMMON_H_
#define COMMON_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define EIO			5	/* I/O error */
#define EAGAIN		11	/* Try again */
#define ENOMEM		12	/* Out of memory */
#define EFAULT		14	/* Bad address */
#define ENODEV		19	/* No such device */
#define EINVAL		22	/* Invalid argument */
#define EOPNOTSUPP	45	/* Operation not supported on transport endpoint */
#define ETIMEDOUT	110	/* Connection timed out */

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/
#if defined (__STDC__) && (__STDC_VERSION__ >= 199901L)
#include <stdbool.h>
#else
typedef enum { false, true } bool;
#endif

struct clk {
	const char	*name;
	uint32_t	rate;
};

struct clk_hw {
		struct clk *clk;
};

struct clk_init_data {
	const char				*name;
	const struct clk_ops	*ops;
	const char				**parent_names;
	uint8_t					num_parents;
	uint32_t				flags;
};

struct clk_onecell_data {
	struct clk		**clks;
	uint32_t		clk_num;
};

#endif
