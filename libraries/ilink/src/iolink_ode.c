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
#define AL_Read_req                mock_AL_Read_req
#define AL_Write_req               mock_AL_Write_req
#define iolink_post_job            mock_iolink_post_job
#define iolink_fetch_avail_job     mock_iolink_fetch_avail_job
#define iolink_fetch_avail_api_job mock_iolink_fetch_avail_api_job
#endif /* UNIT_TEST */

#include "iolink_ode.h"
#include "iolink_al.h"   /* AL_Read_req AL_Write_req */
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_fetch_avail_api_job, iolink_post_job, iolink_get_portnumber */

#include "osal_log.h"

#include <stdbool.h>

/**
 * @file
 * @brief On-request Data Exchange layer
 *
 */

/**
 * ODE state-machine events.
 *
 * ODE events trigger state transitions in the FSM.
 */
typedef enum iolink_fsm_ode_event
{
   ODE_EVENT_NONE = 0,
   ODE_EVENT_SMI_DEV_RW_1, /* T1 */
   ODE_EVENT_OD_START,     /* T2 */
   ODE_EVENT_OD_STOP,      /* T3 */
   ODE_EVENT_SMI_DEV_RW_4, /* T4 */
   ODE_EVENT_OD_BLOCK,     /* T5 */
   ODE_EVENT_SMI_DEV_RW_6, /* T6 */
   ODE_EVENT_OD_UNBLOCK,   /* T7 */
   ODE_EVENT_LAST
} iolink_fsm_ode_event_t;

static const char * const iolink_ode_state_literals[] = {
   "Inactive",
   "ODactive",
   "ODblocked",
};

static const char * const iolink_ode_event_literals[] = {
   "NONE",
   "SMI_DEV_RW_1", /* T1 */
   "OD_START",     /* T2 */
   "OD_STOP",      /* T3 */
   "SMI_DEV_RW_4", /* T4 */
   "OD_BLOCK",     /* T5 */
   "SMI_DEV_RW_6", /* T6 */
   "OD_UNBLOCK",   /* T7 */
};

typedef struct iolink_fsm_ode_transition
{
   iolink_fsm_ode_event_t event;
   iolink_ode_state_t next_state;
   iolink_fsm_ode_event_t (
      *action) (iolink_port_t * port, iolink_fsm_ode_event_t event);
} iolink_fsm_ode_transition_t;

typedef struct iolink_fsm_ode_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_ode_transition_t * transitions;
} iolink_fsm_ode_state_transitions_t;

/**
 * Trigger OD FSM state transition
 *
 * This function triggers an OD state transition according to the event.
 */
static void iolink_ode_event (iolink_port_t * port, iolink_fsm_ode_event_t event);

/* Actions taken when transitioning to a new state. See iolink_ode_event(). */
static iolink_fsm_ode_event_t ode_smi_od_err (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);
static iolink_fsm_ode_event_t ode_od_start (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);
static iolink_fsm_ode_event_t ode_od_stop (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);
static iolink_fsm_ode_event_t ode_smi_rw (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);
static iolink_fsm_ode_event_t ode_smi_done (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);
static iolink_fsm_ode_event_t ode_wait (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event);

/* Callback functions to run on main thread */
static void ode_read_cnf_cb (iolink_job_t * job);
static void ode_write_cnf_cb (iolink_job_t * job);
static void od_start_cb (iolink_job_t * job);
static void od_stop_cb (iolink_job_t * job);
static void SMI_rw_req_cb (iolink_job_t * job);

/* Other functions */
static iolink_fsm_ode_event_t ode_AL_Read_req (iolink_port_t * port);
static iolink_fsm_ode_event_t ode_AL_Write_req (iolink_port_t * port);
static iolink_error_t ode_SMI_rw_req (
   iolink_job_type_t job_type,
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);
static void ode_AL_Read_cnf (
   iolink_port_t * port,
   uint8_t len,
   const uint8_t * data,
   iolink_smi_errortypes_t errortype);
static void ode_AL_Write_cnf (
   iolink_port_t * port,
   iolink_smi_errortypes_t errortype);
static iolink_error_t ode_SMI_DeviceReadWrite_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block,
   iolink_arg_block_id_t od_or_void,
   iolink_job_type_t type);

static iolink_fsm_ode_event_t ode_smi_od_err (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   iolink_smi_errortypes_t errortype  = IOLINK_SMI_ERRORTYPE_NONE;
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->job_smi_req_busy->smi_req;
   iolink_arg_block_id_t ref_arg_block_id =
      smi_req->arg_block->id;

   /* SMI_ParamReadBatch and SMI_ParamWriteBatch are not supported */
   if (
      ref_arg_block_id == IOLINK_ARG_BLOCK_ID_PORT_INXDEX_LIST ||
      ref_arg_block_id == IOLINK_ARG_BLOCK_ID_DEV_PAR_BAT)
   {
      errortype = IOLINK_SMI_ERRORTYPE_SERVICE_NOT_SUPPORTED;
   }
   else
   {
      switch (event)
      {
      case ODE_EVENT_SMI_DEV_RW_1:
         /* Access blocked (inactive) indicate IDX_NOTAVAIL */
         errortype = IOLINK_SMI_ERRORTYPE_IDX_NOTAVAIL;
         break;
      case ODE_EVENT_SMI_DEV_RW_6:
         /* Access blocked (temporarily) inidcate SERVICE_TEMP_UNAVAILABLE */
         errortype = IOLINK_SMI_ERRORTYPE_SERVICE_TEMP_UNAVAILABLE;
         break;
      default:
         CC_ASSERT (0);
         break;
      }
   }

   switch (ref_arg_block_id)
   {
   case IOLINK_ARG_BLOCK_ID_OD_RD:
   case IOLINK_ARG_BLOCK_ID_OD_WR:
   case IOLINK_ARG_BLOCK_ID_PORT_INXDEX_LIST:
   case IOLINK_ARG_BLOCK_ID_DEV_PAR_BAT:
      break;
   default:
      errortype = IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED;
      break;
   }

   iolink_smi_joberror_ind (
      port,
      smi_req->exp_arg_block_id,
      ref_arg_block_id,
      errortype);

   return ODE_EVENT_NONE;
}

static iolink_fsm_ode_event_t ode_od_start (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   /* Wait for SMI calls */
   return ODE_EVENT_NONE;
}

static iolink_fsm_ode_event_t ode_od_stop (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   return ODE_EVENT_NONE;
}

static iolink_fsm_ode_event_t ode_AL_Read_req (iolink_port_t * port)
{
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->smi_req;
   arg_block_od_t * arg_block_od      = (arg_block_od_t *)smi_req->arg_block;

   AL_Read_req (port, arg_block_od->index, arg_block_od->subindex, ode_AL_Read_cnf);

   return ODE_EVENT_OD_BLOCK;
}

static iolink_fsm_ode_event_t ode_AL_Write_req (iolink_port_t * port)
{
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->smi_req;
   uint8_t arg_block_len              = smi_req->arg_block_len;
   uint8_t arg_block_data_len         = 0;

   arg_block_od_t * arg_block_od = (arg_block_od_t *)smi_req->arg_block;

   if (arg_block_len >= sizeof (arg_block_od_t))
   {
      arg_block_data_len = arg_block_len - sizeof (arg_block_od_t);
   }

   AL_Write_req (
      port,
      arg_block_od->index,
      arg_block_od->subindex,
      arg_block_data_len,
      arg_block_od->data,
      ode_AL_Write_cnf);

   return ODE_EVENT_OD_BLOCK;
}

static iolink_fsm_ode_event_t ode_smi_rw (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   iolink_fsm_ode_event_t res_event       = ODE_EVENT_NONE;
   iolink_ode_port_t * ode                = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req     = &ode->smi_req;
   iolink_arg_block_id_t exp_arg_block_id = smi_req->exp_arg_block_id;
   iolink_arg_block_id_t ref_arg_block_id =
      smi_req->arg_block->id;

   if (
      (exp_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_RD) &&
      (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_RD))
   {
      res_event = ode_AL_Read_req (port);
   }
   else if (
      (exp_arg_block_id == IOLINK_ARG_BLOCK_ID_VOID_BLOCK) &&
      (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_WR))
   {
      res_event = ode_AL_Write_req (port);
   }
   else
   {
      iolink_smi_errortypes_t errortype =
         IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED;

      /* SMI_ParamReadBatch and SMI_ParamWriteBatch are not supported */
      if (
         (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_PORT_INXDEX_LIST) ||
         (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_DEV_PAR_BAT))
      {
         errortype = IOLINK_SMI_ERRORTYPE_SERVICE_NOT_SUPPORTED;
      }

      iolink_smi_joberror_ind (port, exp_arg_block_id, ref_arg_block_id, errortype);
   }

   return res_event;
}

static iolink_fsm_ode_event_t ode_smi_done (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   iolink_ode_port_t * ode                = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req     = &ode->smi_req;
   iolink_arg_block_id_t exp_arg_block_id = smi_req->exp_arg_block_id;
   iolink_arg_block_id_t ref_arg_block_id =
      smi_req->arg_block->id;

   if (smi_req->result != IOLINK_SMI_ERRORTYPE_NONE)
   {
      iolink_smi_joberror_ind (
         port,
         exp_arg_block_id,
         ref_arg_block_id,
         smi_req->result);
   }
   else
   {
      if (
         (exp_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_RD) &&
         (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_RD))
      {
         iolink_smi_cnf (
            port,
            ref_arg_block_id,
            smi_req->arg_block_len,
            smi_req->arg_block);
      }
      else if (
         (exp_arg_block_id == IOLINK_ARG_BLOCK_ID_VOID_BLOCK) &&
         (ref_arg_block_id == IOLINK_ARG_BLOCK_ID_OD_WR))
      {
         iolink_smi_voidblock_cnf (port, ref_arg_block_id);
      }
      else
      {
         CC_ASSERT (0);
      }
   }

   return ODE_EVENT_NONE;
}

static iolink_fsm_ode_event_t ode_wait (
   iolink_port_t * port,
   iolink_fsm_ode_event_t event)
{
   /* Wait for AL_{Read,Write}_cnf */
   return ODE_EVENT_NONE;
}

/* ODE FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 11.5 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_ode_transition_t ode_trans_s0[] = {
   /* Inactive_0 */
   {ODE_EVENT_OD_START, ODE_STATE_ODactive, ode_od_start},       /* T2 */
   {ODE_EVENT_SMI_DEV_RW_1, ODE_STATE_Inactive, ode_smi_od_err}, /* T1 */

   {ODE_EVENT_OD_STOP, ODE_STATE_Inactive, ode_od_stop}, /* NOTE: Not in spec */
};

static const iolink_fsm_ode_transition_t ode_trans_s1[] = {
   /* ODactive_1 */
   {ODE_EVENT_SMI_DEV_RW_4, ODE_STATE_ODactive, ode_smi_rw}, /* T4 */
   {ODE_EVENT_OD_BLOCK, ODE_STATE_ODblocked, ode_wait},      /* T5 */
   {ODE_EVENT_OD_STOP, ODE_STATE_Inactive, ode_od_stop},     /* T3 */
};

static const iolink_fsm_ode_transition_t ode_trans_s2[] = {
   /* ODblocked_2 */
   {ODE_EVENT_OD_UNBLOCK, ODE_STATE_ODactive, ode_smi_done},      /* T7 */
   {ODE_EVENT_SMI_DEV_RW_6, ODE_STATE_ODblocked, ode_smi_od_err}, /* T6 */

   {ODE_EVENT_OD_STOP, ODE_STATE_Inactive, ode_od_stop}, /* Not part of FSM */
};

/* The index is the state in this array */
static const iolink_fsm_ode_state_transitions_t iolink_ode_fsm[] = {
   {/* Inactive_0 */
    .number_of_trans = NELEMENTS (ode_trans_s0),
    .transitions     = ode_trans_s0},
   {/* ODactive_1 */
    .number_of_trans = NELEMENTS (ode_trans_s1),
    .transitions     = ode_trans_s1},
   {/* ODblocked_2 */
    .number_of_trans = NELEMENTS (ode_trans_s2),
    .transitions     = ode_trans_s2},
};

static void iolink_ode_event (iolink_port_t * port, iolink_fsm_ode_event_t event)
{
   do
   {
      int i;
      iolink_ode_port_t * ode     = iolink_get_ode_ctx (port);
      iolink_ode_state_t previous = ode->state;
      const iolink_fsm_ode_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_ode_fsm[previous].number_of_trans; i++)
      {
         if (iolink_ode_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_ode_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         LOG_ERROR (
            IOLINK_ODE_LOG,
            "ODE (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_ode_state_literals[previous],
            iolink_ode_event_literals[event]);
         return;
      }

      /* Transition to next state */
      ode->state = next_trans->next_state;

      LOG_DEBUG (
         IOLINK_ODE_LOG,
         "ODE (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_ode_event_literals[event],
         iolink_ode_state_literals[previous],
         iolink_ode_state_literals[ode->state]);

      event = next_trans->action (port, event);
   } while (event != ODE_EVENT_NONE);
}

/* AL job callback functions */
static iolink_error_t ode_SMI_rw_req (
   iolink_job_type_t job_type,
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_job_t * job            = iolink_fetch_avail_api_job (port);
   job->smi_req.exp_arg_block_id = exp_arg_block_id;
   job->smi_req.arg_block_len    = arg_block_len;
   job->smi_req.arg_block        = arg_block;

   iolink_post_job_with_type_and_callback (port, job, job_type, SMI_rw_req_cb);

   return IOLINK_ERROR_NONE;
}

static void ode_read_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port               = job->port;
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->smi_req;

   smi_req->result = job->al_read_cnf.errortype;
   if (smi_req->result == IOLINK_SMI_ERRORTYPE_NONE)
   {
      uint8_t arg_block_len      = smi_req->arg_block_len;
      uint8_t arg_block_data_len = arg_block_len - sizeof (arg_block_od_t);
      uint8_t len                = job->al_read_cnf.data_len;

      if (arg_block_data_len >= len)
      {
         arg_block_od_t * arg_block_od = (arg_block_od_t *)smi_req->arg_block;

         memset (arg_block_od->data, 0, arg_block_data_len);
         memcpy (arg_block_od->data, job->al_read_cnf.data, len);

         arg_block_len          = len + sizeof (arg_block_od_t);
         smi_req->arg_block_len = arg_block_len;
      }
      else
      {
         smi_req->result = IOLINK_SMI_ERRORTYPE_ARGBLOCK_LENGTH_INVALID;
      }
   }

   iolink_ode_event (port, ODE_EVENT_OD_UNBLOCK);
}

static void ode_AL_Read_cnf (
   iolink_port_t * port,
   uint8_t len,
   const uint8_t * data,
   iolink_smi_errortypes_t errortype)
{
   iolink_job_t * job         = iolink_fetch_avail_job (port);
   job->al_read_cnf.data      = data;
   job->al_read_cnf.data_len  = len;
   job->al_read_cnf.errortype = errortype;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_READ_CNF,
      ode_read_cnf_cb);
}

static void ode_write_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port               = job->port;
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->smi_req;

   if (job->al_write_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE)
   {
      smi_req->result = job->al_write_cnf.errortype;
   }

   iolink_ode_event (port, ODE_EVENT_OD_UNBLOCK);
}

static void ode_AL_Write_cnf (iolink_port_t * port, iolink_smi_errortypes_t errortype)
{
   iolink_job_t * job          = iolink_fetch_avail_job (port);
   job->al_write_cnf.errortype = errortype;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_WRITE_CNF,
      ode_write_cnf_cb);
}

static void od_start_cb (iolink_job_t * job)
{
   iolink_ode_event (job->port, ODE_EVENT_OD_START);
}

static void od_stop_cb (iolink_job_t * job)
{
   iolink_ode_event (job->port, ODE_EVENT_OD_STOP);
}

static void SMI_rw_req_cb (iolink_job_t * job)
{
   iolink_fsm_ode_event_t event       = ODE_EVENT_NONE;
   iolink_port_t * port               = job->port;
   iolink_ode_port_t * ode            = iolink_get_ode_ctx (port);
   iolink_smi_service_req_t * smi_req = &ode->smi_req;

   switch (ode->state)
   {
   case ODE_STATE_ODactive:
      smi_req->result           = IOLINK_SMI_ERRORTYPE_NONE;
      smi_req->exp_arg_block_id = job->smi_req.exp_arg_block_id;
      smi_req->arg_block_len    = job->smi_req.arg_block_len;
      smi_req->arg_block        = job->smi_req.arg_block;
      event                     = ODE_EVENT_SMI_DEV_RW_4;
      break;
   case ODE_STATE_Inactive:
   case ODE_STATE_ODblocked:
      /* Store the job, for a negative response */
      ode->job_smi_req_busy = job;
      event = (ode->state == ODE_STATE_Inactive) ? ODE_EVENT_SMI_DEV_RW_1
                                                 : ODE_EVENT_SMI_DEV_RW_6;
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   iolink_ode_event (port, event);
}

/* Stack internal API */
void iolink_ode_init (iolink_port_t * port)
{
   iolink_ode_port_t * ode = iolink_get_ode_ctx (port);

   ode->state = ODE_STATE_Inactive;
}

iolink_error_t OD_Start (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_OD_START,
      od_start_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t OD_Stop (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_OD_STOP,
      od_stop_cb);

   return IOLINK_ERROR_NONE;
}

static iolink_error_t ode_SMI_DeviceReadWrite_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block,
   iolink_arg_block_id_t od_or_void,
   iolink_job_type_t type)
{
   if (exp_arg_block_id != od_or_void)
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   return ode_SMI_rw_req (type, port, exp_arg_block_id, arg_block_len, arg_block);
}

iolink_error_t ode_SMI_DeviceRead_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return ode_SMI_DeviceReadWrite_req (
      port,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      IOLINK_ARG_BLOCK_ID_OD_RD,
      IOLINK_JOB_SMI_DEVICE_READ);
}

iolink_error_t ode_SMI_DeviceWrite_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return ode_SMI_DeviceReadWrite_req (
      port,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      IOLINK_JOB_SMI_DEVICE_WRITE);
}

iolink_error_t ode_SMI_ParamReadBatch_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return ode_SMI_rw_req (
      IOLINK_JOB_SMI_PARAM_READ,
      port,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}

iolink_error_t ode_SMI_ParamWriteBatch_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return ode_SMI_rw_req (
      IOLINK_JOB_SMI_PARAM_WRITE,
      port,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}
