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

#include "iolink_pl.h"

#include "osal.h"
#include "osal_log.h"

#ifdef __rtk__
#include <kern/types.h>
#include <drivers/dev.h>
#include <drivers/fd.h>
#endif

#include <fcntl.h>  /* O_RDWR */
#include <unistd.h> /* close */

#include "iolink_main.h" /* iolink_port_t */

/**
 * @file
 * @brief Physical layer
 *
 */

#define IOLINK_RXERR_CHKSM  BIT (0)
#define IOLINK_RXERR_SIZE   BIT (1)
#define IOLINK_RXERR_FRAME  BIT (2)
#define IOLINK_RXERR_PARITY BIT (3)
#define IOLINK_RXERR_TDLY   BIT (4)
#define IOLINK_TXERR_TRANSM BIT (5)
#define IOLINK_TXERR_CYCL   BIT (6)
#define IOLINK_TXERR_CHKSM  BIT (7)
#define IOLINK_TXERR_SIZE   BIT (8)

/*
 * Wrapper functions
 */
void iolink_configure_pl_event (
   iolink_port_t * port,
   os_event_t * event,
   uint32_t flag)
{
   iolink_pl_port_t * pl = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->configure_event (pl->drv, pl->arg, event, flag);
      os_mutex_unlock (drv->mtx);
   }
}

void iolink_pl_handler (iolink_port_t * port)
{
   iolink_pl_port_t * pl = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->pl_handler (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
}

iolink_baudrate_t iolink_pl_get_baudrate (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;
   iolink_baudrate_t  res;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      res = drv->ops->get_baudrate (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
   else
   {
      res  = IOLINK_BAUDRATE_NONE;
   }

   return res;
}

uint8_t iolink_pl_get_cycletime (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;
   uint8_t            res;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      res = drv->ops->get_cycletime (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
   else
   {
      res  = 0u;
   }

   return res;
}

void iolink_pl_set_cycletime (iolink_port_t * port, uint8_t cycbyte)
{
   iolink_pl_port_t * pl = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->set_cycletime (pl->drv, pl->arg, cycbyte);
      os_mutex_unlock (drv->mtx);
   }
}

bool iolink_pl_get_data (iolink_port_t * port, uint8_t * rxdata, uint8_t len)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;
   bool               res;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      res = drv->ops->get_data (pl->drv, pl->arg, rxdata, len);
      os_mutex_unlock (drv->mtx);
   }
   else
   {
      res  = false;
   }

   return res;
}

void iolink_pl_get_error (iolink_port_t * port, uint8_t * cqerr, uint8_t * devdly)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->get_error (pl->drv, pl->arg, cqerr, devdly);
      os_mutex_unlock (drv->mtx);
   }
}

bool iolink_pl_init_sdci (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;
   bool               res;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      res = drv->ops->init_sdci (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
   else
   {
      res  = false;
   }

   return res;
}
/*
 * PL services
 */
void PL_SetMode_req (iolink_port_t * port, iolink_pl_mode_t mode)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->set_mode (pl->drv, pl->arg, mode);
      os_mutex_unlock (drv->mtx);
   }
}
void PL_WakeUp_req (iolink_port_t * port)
{
#if IOLINK_HW == IOLINK_HW_MAX14819
#endif
}
#if IOLINK_HW == IOLINK_HW_MAX14819
void PL_Resend (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->send_msg (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
}

void PL_Transfer_req (
   iolink_port_t * port,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->transfer_req (pl->drv, pl->arg, rxbytes, txbytes, data);
      os_mutex_unlock (drv->mtx);
   }
}

void PL_MessageDownload_req (
   iolink_port_t * port,
   uint8_t rxbytes,
   uint8_t txbytes,
   uint8_t * data)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->dl_msg (pl->drv, pl->arg, rxbytes, txbytes, data);
      os_mutex_unlock (drv->mtx);
   }
}

void PL_EnableCycleTimer (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->enable_cycle_timer (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
}

void PL_DisableCycleTimer (iolink_port_t * port)
{
   iolink_pl_port_t * pl  = iolink_get_pl_ctx (port);
   iolink_hw_drv_t *  drv = pl->drv;

   if (drv != NULL)
   {
      os_mutex_lock (drv->mtx);
      drv->ops->disable_cycle_timer (pl->drv, pl->arg);
      os_mutex_unlock (drv->mtx);
   }
}

#else
void PL_Transfer_req (iolink_port_t * port, uint8_t data)
{
}
#endif

void iolink_pl_init (iolink_port_t * port, iolink_hw_drv_t * drv, void * arg)
{
   iolink_pl_port_t * pl = iolink_get_pl_ctx (port);

   memset (pl, 0, sizeof (iolink_pl_port_t));

   pl->drv = drv;
   pl->arg = arg;
}
