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

#include <string.h>
#include "osal.h"
#include "osal_log.h"
#include "iolink.h"
#include "iolink_main.h" /* TODO Use headers from include/ directory */
#include "iolink_dl.h"   /* TODO Use headers from include/ directory */
#include "app_handler.h"

iolink_smi_errortypes_t app_smi_device_write (
   app_port_ctx_t * app_port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   const uint8_t * data)
{
   uint8_t arg_block_len = sizeof (arg_block_od_t) + len;
   uint8_t buffer[arg_block_len];
   arg_block_od_t * arg_block_od = (arg_block_od_t *)buffer;

   memset (buffer, 0, arg_block_len);
   arg_block_od->arg_block.id = IOLINK_ARG_BLOCK_ID_OD_WR;
   arg_block_od->index        = index;
   arg_block_od->subindex     = subindex;

   memcpy (arg_block_od->data, data, len);

   iolink_error_t err = SMI_DeviceWrite_req (
      app_port->portnumber,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      arg_block_len,
      (arg_block_t *)arg_block_od);
   if (err != IOLINK_ERROR_NONE)
   {
      LOG_WARNING (
         LOG_STATE_ON,
         "%s failed (SMI_DeviceWrite_req returned %d) for port %u\n",
         __func__,
         err,
         app_port->portnumber);

      return (err == IOLINK_ERROR_ODLENGTH)
                ? IOLINK_SMI_ERRORTYPE_VAL_LENOVRRUN
                : IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO
   }

   return app_wait_for_cnf (app_port, SMI_WRITE_CNF, 10000);
}

iolink_smi_errortypes_t app_smi_device_read (
   app_port_ctx_t * app_port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   uint8_t * data,
   uint8_t * actual_len)
{
   if (len == 0)
   {
      LOG_WARNING (
         LOG_STATE_ON,
         "%s: len == 0 for port %u\n",
         __func__,
         app_port->portnumber);

      return IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT; // TODO what to return?
   }

   uint16_t arg_block_len = sizeof (arg_block_od_t) + len;
   uint8_t buffer[arg_block_len];
   arg_block_od_t * arg_block_od = (arg_block_od_t *)buffer;

   memset (buffer, 0, arg_block_len);
   arg_block_od->arg_block.id = IOLINK_ARG_BLOCK_ID_OD_RD;
   arg_block_od->index        = index;
   arg_block_od->subindex     = subindex;

   iolink_smi_errortypes_t errortype = IOLINK_SMI_ERRORTYPE_NONE;
   iolink_error_t err                = SMI_DeviceRead_req (
      app_port->portnumber,
      IOLINK_ARG_BLOCK_ID_OD_RD,
      arg_block_len,
      (arg_block_t *)arg_block_od);
   if (err != IOLINK_ERROR_NONE)
   {
      LOG_WARNING (
         LOG_STATE_ON,
         "%s failed (SMI_DeviceRead_req returned %d) for port %u\n",
         __func__,
         err,
         app_port->portnumber);

      errortype = IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO
   }
   else
   {
      uint32_t event_value;

      /* Wait for SMI_DeviceRead_cnf() */
      if (os_event_wait (app_port->event, SMI_READ_CNF, &event_value, 10000))
      {
         errortype = app_port->errortype;

         LOG_WARNING (
            LOG_STATE_ON,
            "%s: timeout for port %u\n",
            __func__,
            app_port->portnumber);

         if (errortype != IOLINK_SMI_ERRORTYPE_NONE)
         {
            app_port->errortype = IOLINK_SMI_ERRORTYPE_NONE;
         }
         else
         {
            errortype = IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO
         }
      }
      else
      {
         os_event_clr (app_port->event, event_value);

         if (data != NULL)
         {
            if (app_port->param_read.data_len > 0)
            {
               memcpy (data, arg_block_od->data, len);

               if (actual_len != NULL)
               {
                  *actual_len = app_port->param_read.data_len;
               }
            }

            app_port->param_read.data_len = 0;
         }
      }
   }

   return errortype;
}

uint8_t app_smi_pdout (
   app_port_ctx_t * app_port,
   bool output_enable,
   uint8_t len,
   const uint8_t * data)
{
   os_mutex_lock (app_port->pdout_mtx);

   arg_block_pdout_t arg_block_pdout;

   arg_block_pdout.h.arg_block.id = IOLINK_ARG_BLOCK_ID_PD_OUT;
   memcpy (arg_block_pdout.data, data, len);
   arg_block_pdout.h.len = len;
   arg_block_pdout.h.oe  = (output_enable) ? 1 : 0;

   iolink_error_t err = SMI_PDOut_req (
      app_port->portnumber,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      sizeof (arg_block_pdout_head_t) + len,
      (arg_block_t *)&arg_block_pdout);
   os_mutex_unlock (app_port->pdout_mtx);

   return (err == IOLINK_ERROR_NONE) ? 0 : 1;
}

int8_t app_smi_pdin (app_port_ctx_t * app_port, bool * valid, uint8_t * pdin)
{
   arg_block_void_t arg_block_void;
   int8_t len = 0;

   memset (&arg_block_void, 0, sizeof (arg_block_void_t));
   arg_block_void.arg_block.id = IOLINK_ARG_BLOCK_ID_VOID_BLOCK;

   if (
      SMI_PDIn_req (
         app_port->portnumber,
         IOLINK_ARG_BLOCK_ID_PD_IN,
         sizeof (arg_block_void_t),
         (arg_block_t *)&arg_block_void) == IOLINK_ERROR_NONE)
   {
      len = app_port->pdin.data_len;

      memcpy (pdin, app_port->pdin.data, len);

      if (valid != NULL)
      {
         *valid = !(app_port->pdin.pqi & IOLINK_PORT_QUALIFIER_INFO_PQ_INVALID);

         iolink_port_t * port =
            iolink_get_port (app_master.master, app_port->portnumber);
         iolink_get_dl_ctx (port)->pd_handler.pd_valid = *valid;
      }
   }

   return len;
}

uint8_t app_smi_pdinout (app_port_ctx_t * app_port)
{
   arg_block_void_t arg_block_void;

   memset (&arg_block_void, 0, sizeof (arg_block_void_t));
   arg_block_void.arg_block.id = IOLINK_ARG_BLOCK_ID_VOID_BLOCK;

   iolink_error_t err = SMI_PDInOut_req (
      app_port->portnumber,
      IOLINK_ARG_BLOCK_ID_PD_IN_OUT,
      sizeof (arg_block_void_t),
      (arg_block_t *)&arg_block_void);

   return (err == IOLINK_ERROR_NONE) ? 0 : 1;
}
