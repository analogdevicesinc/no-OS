/******************************************************************************
* Copyright (C) 2002 - 2021 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/


#ifndef XDEBUG
#define XDEBUG

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DEBUG) && !defined(NDEBUG)

#ifndef XDEBUG_WARNING
#define XDEBUG_WARNING
#warning DEBUG is enabled
#endif

int printf(const char *format, ...);

#define XDBG_DEBUG_ERROR             0x00000001U    /* error  condition messages */
#define XDBG_DEBUG_GENERAL           0x00000002U    /* general debug  messages */
#define XDBG_DEBUG_ALL               0xFFFFFFFFU    /* all debugging data */

#define XDBG_DEBUG_FIFO_REG          0x00000100U    /* display register reads/writes */
#define XDBG_DEBUG_FIFO_RX           0x00000101U    /* receive debug messages */
#define XDBG_DEBUG_FIFO_TX           0x00000102U    /* transmit debug messages */
#define XDBG_DEBUG_FIFO_ALL          0x0000010FU    /* all fifo debug messages */

#define XDBG_DEBUG_TEMAC_REG         0x00000400U    /* display register reads/writes */
#define XDBG_DEBUG_TEMAC_RX          0x00000401U    /* receive debug messages */
#define XDBG_DEBUG_TEMAC_TX          0x00000402U    /* transmit debug messages */
#define XDBG_DEBUG_TEMAC_ALL         0x0000040FU    /* all temac  debug messages */

#define XDBG_DEBUG_TEMAC_ADPT_RX     0x00000800U    /* receive debug messages */
#define XDBG_DEBUG_TEMAC_ADPT_TX     0x00000801U    /* transmit debug messages */
#define XDBG_DEBUG_TEMAC_ADPT_IOCTL  0x00000802U    /* ioctl debug messages */
#define XDBG_DEBUG_TEMAC_ADPT_MISC   0x00000803U    /* debug msg for other routines */
#define XDBG_DEBUG_TEMAC_ADPT_ALL    0x0000080FU    /* all temac adapter debug messages */


#define xdbg_current_types (XDBG_DEBUG_GENERAL | XDBG_DEBUG_ERROR | XDBG_DEBUG_TEMAC_REG | XDBG_DEBUG_FIFO_RX | XDBG_DEBUG_FIFO_TX | XDBG_DEBUG_FIFO_REG)

#define xdbg_stmnt(x)  x

/* In VxWorks, if _WRS_GNU_VAR_MACROS is defined, special syntax is needed for
 * macros that accept variable number of arguments
 */
#if defined(XENV_VXWORKS) && defined(_WRS_GNU_VAR_MACROS)
#define xdbg_printf(type, args...) (((type) & xdbg_current_types) ? printf (## args) : 0)
#else /* ANSI Syntax */
#define xdbg_printf(type, ...) (((type) & xdbg_current_types) ? printf (__VA_ARGS__) : 0)
#endif

#else /* defined(DEBUG) && !defined(NDEBUG) */

#define xdbg_stmnt(x)

/* See VxWorks comments above */
#if defined(XENV_VXWORKS) && defined(_WRS_GNU_VAR_MACROS)
#define xdbg_printf(type, args...)
#else /* ANSI Syntax */
#define xdbg_printf(...)
#endif
#endif /* defined(DEBUG) && !defined(NDEBUG) */

#ifdef __cplusplus
}
#endif

#endif /* XDEBUG */
