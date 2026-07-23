/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2019 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

/**
 * @file
 * @brief Physical layer
 *
 */

#ifndef IOLINK_PL_H
#define IOLINK_PL_H

#if IOLINK_HW != IOLINK_HW_MAX14819
#error "IO-Link HW not supported: ""IOLINK_HW"
#endif /* IOLINK_HW */

#include <iolink_dl.h>
#include <iolink_types.h>
#include <iolink_pl_hw_drv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iolink_pl_port
{
   int fd;
   iolink_hw_drv_t * drv;
   void *            arg;
} iolink_pl_port_t;

/*
 * Function interface
 */
void iolink_configure_pl_event (
   iolink_port_t * port,
   os_event_t * event,
   uint32_t flag);
void iolink_pl_handler (iolink_port_t * port);
iolink_baudrate_t iolink_pl_get_baudrate (iolink_port_t * port);
uint8_t iolink_pl_get_cycletime (iolink_port_t * port);
void iolink_pl_set_cycletime (iolink_port_t * port, uint8_t cycbyte);
bool iolink_pl_get_data (iolink_port_t * port, uint8_t * rxdata, uint8_t len);
void iolink_pl_get_error (iolink_port_t * port, uint8_t * cqerr, uint8_t * devdly);
bool iolink_pl_init_sdci (iolink_port_t * port);

/*
 * PL Services
 */
void PL_SetMode_req (iolink_port_t * port, iolink_pl_mode_t mode);
void PL_WakeUp_req (iolink_port_t * port);
#if IOLINK_HW == IOLINK_HW_MAX14819
void PL_Resend (iolink_port_t * port);
void PL_Transfer_req (
   iolink_port_t * port,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data);
void PL_MessageDownload_req (
   iolink_port_t * port,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data);
void PL_EnableCycleTimer (iolink_port_t * port);
void PL_DisableCycleTimer (iolink_port_t * port);
#else
void PL_Transfer_req (iolink_port_t * port, uint8_t data);
#endif
void iolink_pl_init (iolink_port_t * port, iolink_hw_drv_t * drv, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_PL_H */
