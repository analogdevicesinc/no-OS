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

#ifdef UNIT_TEST
#include "mocks.h"
#define AL_Control_req             mock_AL_Control_req
#define AL_GetInput_req            mock_AL_GetInput_req
#define AL_GetInputOutput_req      mock_AL_GetInputOutput_req
#define AL_SetOutput_req           mock_AL_SetOutput_req
#define iolink_post_job            mock_iolink_post_job
#define iolink_fetch_avail_job     mock_iolink_fetch_avail_job
#define iolink_fetch_avail_api_job mock_iolink_fetch_avail_api_job
#endif /* UNIT_TEST */

#include "iolink_pde.h"
#include "iolink_al.h" /* AL_Control_req, AL_GetInput_req, AL_GetInputOutput_req, AL_SetOutput_req */
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_fetch_avail_api_job, iolink_post_job, iolink_get_portnumber, iolink_get_port_info */

#include "osal_log.h"

#include <stdbool.h>

/**
 * @file
 * @brief Process Data Exchange layer
 *
 */

static iolink_port_qualifier_info_t pde_port_quality_to_qualifier (
   iolink_port_t * port);
static iolink_port_status_info_t pde_get_port_status_info (
   iolink_port_t * port);
static iolink_error_t pde_check_smi_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   iolink_arg_block_id_t ref_arg_block_id,
   uint8_t arg_block_len,
   iolink_arg_block_id_t exp_exp_arg_block_id,
   iolink_arg_block_id_t exp_ref_arg_block_id,
   uint8_t exp_arg_block_len_min,
   uint8_t exp_arg_block_len_max);

static iolink_port_qualifier_info_t pde_port_quality_to_qualifier (
   iolink_port_t * port)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);

   return (port_info->port_quality_info & IOLINK_PORT_QUALITY_INFO_INVALID)
             ? IOLINK_PORT_QUALIFIER_INFO_PQ_INVALID
             : IOLINK_PORT_QUALIFIER_INFO_PQ_VALID;
}

static iolink_port_status_info_t pde_get_port_status_info (
   iolink_port_t * port)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);

   return port_info->port_status_info;
}

static iolink_error_t pde_check_smi_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   iolink_arg_block_id_t ref_arg_block_id,
   uint8_t arg_block_len,
   iolink_arg_block_id_t exp_exp_arg_block_id,
   iolink_arg_block_id_t exp_ref_arg_block_id,
   uint8_t exp_arg_block_len_min,
   uint8_t exp_arg_block_len_max)
{
   iolink_smi_errortypes_t smi_error = IOLINK_SMI_ERRORTYPE_NONE;
   iolink_error_t error              = IOLINK_ERROR_NONE;

   if (exp_ref_arg_block_id != ref_arg_block_id)
   {
      smi_error = IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED;
   }
   else if ((arg_block_len < exp_arg_block_len_min) || (arg_block_len > exp_arg_block_len_max))
   {
      smi_error = IOLINK_SMI_ERRORTYPE_ARGBLOCK_LENGTH_INVALID;
   }
   else if (exp_exp_arg_block_id != exp_arg_block_id)
   {
      smi_error = IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT;
   }

   if (smi_error != IOLINK_SMI_ERRORTYPE_NONE)
   {
      iolink_smi_joberror_ind (port, exp_arg_block_id, ref_arg_block_id, smi_error);
      error = IOLINK_ERROR_PARAMETER_CONFLICT;
   }
   else
   {
      iolink_pde_port_t * pde = iolink_get_pde_ctx (port);
      if (pde->state == PD_STATE_Inactive)
      {
         smi_error = IOLINK_SMI_ERRORTYPE_DEV_NOT_IN_OPERATE; // TODO is this
                                                              // correct?
         iolink_smi_joberror_ind (
            port,
            exp_arg_block_id,
            ref_arg_block_id,
            smi_error);
         error = IOLINK_ERROR_STATE_INVALID; // TODO what to return?
      }
   }

   return error;
}

/* Stack internal API */
void iolink_pde_init (iolink_port_t * port)
{
   iolink_pde_port_t * pde = iolink_get_pde_ctx (port);

   pde->output_enable = false;
   pde->state         = PD_STATE_Inactive;
}

iolink_error_t PD_Start (iolink_port_t * port)
{
   iolink_pde_port_t * pde = iolink_get_pde_ctx (port);

   pde->state = PD_STATE_PDactive;

   return IOLINK_ERROR_NONE;
}

iolink_error_t PD_Stop (iolink_port_t * port)
{
   iolink_pde_init (port);

   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_NewInput_ind (iolink_port_t * port)
{
   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_Control_ind (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);

   switch (controlcode)
   {
   case IOLINK_CONTROLCODE_VALID:
      port_info->port_quality_info &= ~IOLINK_PORT_QUALITY_INFO_INVALID;
      break;
   case IOLINK_CONTROLCODE_INVALID:
      port_info->port_quality_info |= IOLINK_PORT_QUALITY_INFO_INVALID;
      break;
   case IOLINK_CONTROLCODE_PDOUTVALID:
      port_info->port_quality_info &= ~IOLINK_PORT_QUALITY_INFO_PDO_INVALID;
      break;
   case IOLINK_CONTROLCODE_PDOUTINVALID:
      port_info->port_quality_info |= IOLINK_PORT_QUALITY_INFO_PDO_INVALID;
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   return IOLINK_ERROR_NONE;
}

iolink_error_t pde_SMI_PDIn_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_arg_block_id_t ref_arg_block_id = arg_block->id;
   arg_block_pdin_t arg_block_pdin;
   memset (&arg_block_pdin, 0, sizeof (arg_block_pdin_t));
   uint8_t pdin_len     = 1;
   iolink_error_t error = pde_check_smi_req (
      port,
      exp_arg_block_id,
      ref_arg_block_id,
      arg_block_len,
      IOLINK_ARG_BLOCK_ID_PD_IN,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      sizeof (arg_block_void_t),
      sizeof (arg_block_void_t));

   if (error != IOLINK_ERROR_NONE)
   {
      iolink_port_status_info_t port_status_info =
         pde_get_port_status_info (port);

      if (
         (error == IOLINK_ERROR_STATE_INVALID) &&
         ((port_status_info == IOLINK_PORT_STATUS_INFO_DI) ||
          (port_status_info == IOLINK_PORT_STATUS_INFO_DEACTIVATED)))
      {
         // TODO Copy result from Get signal status DI_C/Q
         arg_block_pdin.data[0] = 1;
         error                  = IOLINK_ERROR_NONE;
      }
   }
   else
   {
      arg_block_pdin.h.port_qualifier_info = pde_port_quality_to_qualifier (port);
      error = AL_GetInput_req (port, &pdin_len, arg_block_pdin.data);

      if (error != IOLINK_ERROR_NONE)
      {
         iolink_smi_errortypes_t smi_error =
            IOLINK_SMI_ERRORTYPE_DEV_NOT_ACCESSIBLE;
         iolink_smi_joberror_ind (
            port,
            exp_arg_block_id,
            ref_arg_block_id,
            smi_error);
      }
   }

   if (error == IOLINK_ERROR_NONE)
   {
      arg_block_pdin.h.arg_block.id = exp_arg_block_id;
      arg_block_pdin.h.len          = pdin_len;
      iolink_smi_cnf (
         port,
         ref_arg_block_id,
         sizeof (arg_block_pdin_head_t) + pdin_len,
         (arg_block_t *)&arg_block_pdin);
   }

   return error;
}

iolink_error_t pde_SMI_PDOut_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_arg_block_id_t ref_arg_block_id = arg_block->id;
   iolink_error_t error                   = pde_check_smi_req (
      port,
      exp_arg_block_id,
      ref_arg_block_id,
      arg_block_len,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      IOLINK_ARG_BLOCK_ID_PD_OUT,
      sizeof (arg_block_pdout_head_t) + 1,
      sizeof (arg_block_pdout_t));

   if (error == IOLINK_ERROR_NONE)
   {
      if (pde_get_port_status_info (port) == IOLINK_PORT_STATUS_INFO_DO)
      {
         // TODO Set signal DO_C/Q, arg_block_pdout->data[0] & BIT(0)
      }
      else
      {
         arg_block_pdout_t * arg_block_pdout = (arg_block_pdout_t *)arg_block;
         bool output_enable                  = arg_block_pdout->h.oe & 1;
         iolink_pde_port_t * pde             = iolink_get_pde_ctx (port);
         iolink_smi_errortypes_t smi_error =
            IOLINK_SMI_ERRORTYPE_DEV_NOT_ACCESSIBLE;

         error = AL_SetOutput_req (port, arg_block_pdout->data);

         if (error != IOLINK_ERROR_NONE)
         {
            iolink_smi_joberror_ind (
               port,
               exp_arg_block_id,
               ref_arg_block_id,
               smi_error);
         }
         else if (pde->output_enable != output_enable)
         {
            error = AL_Control_req (
               port,
               (output_enable) ? IOLINK_CONTROLCODE_PDOUTVALID
                               : IOLINK_CONTROLCODE_PDOUTINVALID);

            if (error != IOLINK_ERROR_NONE)
            {
               iolink_smi_joberror_ind (
                  port,
                  exp_arg_block_id,
                  ref_arg_block_id,
                  smi_error);
            }
            else
            {
               pde->output_enable = output_enable;
            }
         }
      }
   }

   if (error == IOLINK_ERROR_NONE)
   {
      iolink_smi_voidblock_cnf (port, ref_arg_block_id);
   }

   return error;
}

iolink_error_t pde_SMI_PDInOut_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_arg_block_id_t ref_arg_block_id = arg_block->id;
   iolink_error_t error                   = pde_check_smi_req (
      port,
      exp_arg_block_id,
      ref_arg_block_id,
      arg_block_len,
      IOLINK_ARG_BLOCK_ID_PD_IN_OUT,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      sizeof (arg_block_void_t),
      sizeof (arg_block_void_t));

   if (error == IOLINK_ERROR_NONE)
   {
      if (pde_get_port_status_info (port) == IOLINK_PORT_STATUS_INFO_DI)
      {
         // TODO Copy result from Get signal status DI_C/Q
      }
      else
      {
         arg_block_pdinout_t arg_block_pdinout;
         uint8_t pdlen;
         iolink_pde_port_t * pde = iolink_get_pde_ctx (port);

         memset (&arg_block_pdinout, 0, sizeof (arg_block_pdinout_t));
         arg_block_pdinout.h.port_qualifier_info =
            pde_port_quality_to_qualifier (port);
         arg_block_pdinout.h.oe = pde->output_enable;

         error = AL_GetInputOutput_req (port, &pdlen, arg_block_pdinout.data);

         if (error != IOLINK_ERROR_NONE)
         {
            iolink_smi_errortypes_t smi_error =
               IOLINK_SMI_ERRORTYPE_DEV_NOT_ACCESSIBLE;
            iolink_smi_joberror_ind (
               port,
               exp_arg_block_id,
               ref_arg_block_id,
               smi_error);
         }
         else
         {
            arg_block_pdinout.h.arg_block.id = exp_arg_block_id;
            uint8_t arg_block_pdinout_len =
               sizeof (arg_block_pdinout_head_t) + pdlen;
            iolink_smi_cnf (
               port,
               ref_arg_block_id,
               arg_block_pdinout_len,
               (arg_block_t *)&arg_block_pdinout);
         }
      }
   }

   return error;
}
