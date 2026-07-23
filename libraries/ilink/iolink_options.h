/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2021 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

/* Hand-written from iolink_options.h.in for the ADIOL100 no-OS port.
 * The original template uses cmake @VARIABLE@ placeholders which are
 * not available in the no-OS build system. */

#ifndef OPTIONS_H
#define OPTIONS_H

#define LOG_ENABLE
#define WITH_MALLOC

/*
 * Supported IO-Link HW
 */
#define IOLINK_HW_NONE     0
#define IOLINK_HW_MAX14819 1

#ifndef LOG_LEVEL
#define LOG_LEVEL               (LOG_LEVEL_DEBUG)
#endif

#ifndef IOLINK_PL_LOG
#define IOLINK_PL_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_DL_LOG
#define IOLINK_DL_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_AL_LOG
#define IOLINK_AL_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_SM_LOG
#define IOLINK_SM_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_CM_LOG
#define IOLINK_CM_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_DS_LOG
#define IOLINK_DS_LOG      (LOG_STATE_ON)
#endif

#ifndef IOLINK_ODE_LOG
#define IOLINK_ODE_LOG     (LOG_STATE_ON)
#endif

#ifndef IOLINK_PDE_LOG
#define IOLINK_PDE_LOG     (LOG_STATE_ON)
#endif

#ifndef IOLINK_APP_LOG
#define IOLINK_APP_LOG     (LOG_STATE_ON)
#endif

#ifndef IOLINK_NUM_PORTS
#define IOLINK_NUM_PORTS      (2)
#endif

#ifndef IOLINK_NUM_DIAG_ENTRIES
#define IOLINK_NUM_DIAG_ENTRIES (64)
#endif

#ifndef IOLINK_MAX_EVENTS
#define IOLINK_MAX_EVENTS (20)
#endif

/*
 * IO-Link HW — set to MAX14819 (framer-based transceiver). The ADIOL100 uses
 * the same framer-based code path so this is correct despite the name.
 */
#ifndef IOLINK_HW
#define IOLINK_HW (IOLINK_HW_MAX14819)
#endif

#if IOLINK_HW == IOLINK_HW_MAX14819
#define IOLINK_MAX14819_IRQ_GPIO
//#define IOLINK_MAX14819_IRQ_SPI

//#define IOLINK_MAX14819_RXRDY_GPIO
#define IOLINK_MAX14819_RXRDY_SPI

//#define IOLINK_MAX14819_RXERR_GPIO
#define IOLINK_MAX14819_RXERR_SPI
#endif

#endif  /* OPTIONS_H */
