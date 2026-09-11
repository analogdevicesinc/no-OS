/***************************************************************************//**
 *   @file   iolink_options.h
 *   @brief  i-link stack build configuration — hand-written from the upstream
 *           iolink_options.h.in template (cmake @VARIABLE@ placeholders
 *           replaced with constants for the no-OS build).
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2021 rt-labs AB, Sweden.
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-only
*******************************************************************************/

#ifndef OPTIONS_H
#define OPTIONS_H

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

#endif  /* OPTIONS_H */
