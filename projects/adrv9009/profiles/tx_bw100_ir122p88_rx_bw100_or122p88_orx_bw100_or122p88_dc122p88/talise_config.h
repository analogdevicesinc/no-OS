/**
 * \file adrv9009/profiles/tx_bw100_ir122p88_rx_bw100_or122p88_orx_bw100_or122p88_dc122p88/talise_config.h
 * \brief Talise configuration external declarations
 *
 * \brief Contains structure definitions for tal_config.c
 *
 * Copyright 2015-2017 Analog Devices Inc.
 * Released under the AD9378-AD9379 API license, for more information see the "LICENSE.txt" file in this zip file.
 *
 * The top level structure taliseDevice_t talDevice uses keyword
 * extern to allow the application layer main() to have visibility
 * to these settings.
 */

#ifndef TAL_INIT_H_
#define TAL_INIT_H_

#ifdef __cplusplus
extern "C" {
#endif

extern taliseDevice_t talDevice;
extern taliseInit_t talInit;

#ifdef __cplusplus
}
#endif

#endif
