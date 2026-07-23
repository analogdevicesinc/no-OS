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

#ifndef IOLINK_PL_HW_DRV_H
#define IOLINK_PL_HW_DRV_H

#ifdef __rtk__
#include <drivers/dev.h>
#endif
#include "iolink_types.h"
#include "iolink.h"

#ifdef __cplusplus
extern "C" {
#endif

struct iolink_hw_drv;

typedef struct iolink_hw_ops
{
   iolink_baudrate_t (*get_baudrate) (struct iolink_hw_drv * iolink_hw, void * arg);
   uint8_t (*get_cycletime) (struct iolink_hw_drv * iolink_hw, void * arg);
   void (*set_cycletime) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      uint8_t cycbyte);
   bool (*set_mode) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      iolink_pl_mode_t mode);
   void (*enable_cycle_timer) (struct iolink_hw_drv * iolink_hw, void * arg);
   void (*disable_cycle_timer) (struct iolink_hw_drv * iolink_hw, void * arg);
   void (*get_error) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      uint8_t * cqerr,
      uint8_t * devdly);
   bool (*get_data) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      uint8_t * rxdata,
      uint8_t len);
   void (*dl_msg) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      uint8_t rxbytes,
      uint8_t txbytes,
      uint8_t * data);
   void (*send_msg) (struct iolink_hw_drv * iolink_hw, void * arg);
   void (*transfer_req) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      uint8_t rxbytes,
      uint8_t txbytes,
      uint8_t * data);
   bool (*init_sdci) (struct iolink_hw_drv * iolink_hw, void * arg);
   void (*configure_event) (
      struct iolink_hw_drv * iolink_hw,
      void * arg,
      os_event_t * event,
      uint32_t flag);
   void (*pl_handler) (struct iolink_hw_drv * iolink_hw, void * arg);
} iolink_hw_ops_t;

typedef struct iolink_hw_drv
{
   os_mutex_t *            mtx;
   const iolink_hw_ops_t * ops;
} iolink_hw_drv_t;

void iolink_hw_init (iolink_hw_drv_t * iolink_hw);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_PL_HW_DRV_H */
