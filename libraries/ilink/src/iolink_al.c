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
#define DL_ReadParam_req           mock_DL_ReadParam_req
#define DL_WriteParam_req          mock_DL_WriteParam_req
#define DL_ISDUTransport_req       mock_DL_ISDUTransport_req
#define DL_Control_req             mock_DL_Control_req
#define DL_EventConf_req           mock_DL_EventConf_req
#define DL_PDOutputGet_req         mock_DL_PDOutputGet_req
#define DL_PDOutputUpdate_req      mock_DL_PDOutputUpdate_req
#define AL_Event_ind               mock_AL_Event_ind
#define AL_Control_ind             mock_AL_Control_ind
#define AL_NewInput_ind            mock_AL_NewInput_ind
#define iolink_post_job            mock_iolink_post_job
#define iolink_fetch_avail_job     mock_iolink_fetch_avail_job
#define iolink_fetch_avail_api_job mock_iolink_fetch_avail_api_job
#endif /* UNIT_TEST */

#include "iolink_al.h"
#include "iolink_ds.h" /* DS_Upload */
#include "iolink_dl.h" /* DL_ReadParam_req, DL_WriteParam_req, DL_ISDUTransport_req, DL_Control_req, DL_EventConf_req, DL_PDOutputUpdate_req DL_PDOutputGet_req */
#include "iolink_pde.h"  /* AL_Control_ind, AL_NewInput_ind */
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_post_job, iolink_post_job_pd_event, iolink_get_portnumber */

#include "osal_log.h"

#include <stdbool.h>

/**
 * @file
 * @brief Application layer
 *
 */

static const char * const iolink_al_od_state_literals[] = {
   "OnReq_Idle",
   "Build_DL_Service",
   "Await_DL_param_cnf",
   "Await_DL_ISDU_cnf",
};

static const char * const iolink_al_od_event_literals[] = {
   "NONE",
   "service",           /* T1", T16", T8 and T12 */
   "arg_err",           /* T2 */
   "param_read",        /* T3 */
   "param_write_1",     /* T4 */
   "param_write_2",     /* T5 */
   "isdu_read",         /* T6 */
   "isdu_write",        /* T7 */
   "abort",             /* T9 and T11 */
   "rwparam",           /* T10 */
   "readparam_cnf",     /* T13 */
   "writeparam_cnf",    /* T14 */
   "isdutransport_cnf", /* T15 */
   "abort_port",        /* T17 */
};

static const char * const iolink_al_event_state_literals[] = {
   "Event_inactive",
   "Event_idle",
   "Read_Event_Set",
   "DU_Event_handling",
   "LAST",
};

static const char * const iolink_al_event_event_literals[] = {
   "NONE",
   "dl_event_ind_more", /* T3 or T4 */
   "dl_event_ind_done", /* T5 or T3 + T5 */
   "al_event_rsp",      /* T6 */
   "al_event_req",      /* T7 */
   "LAST",
};

typedef struct iolink_fsm_al_od_transition
{
   iolink_fsm_al_od_event_t event;
   iolink_al_od_state_t next_state;
   iolink_fsm_al_od_event_t (
      *action) (iolink_port_t * port, iolink_fsm_al_od_event_t event);
} iolink_fsm_al_od_transition_t;

typedef struct iolink_fsm_al_od_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_al_od_transition_t * transitions;
} iolink_fsm_al_od_state_transitions_t;

typedef struct iolink_fsm_al_event_transition
{
   iolink_fsm_al_event_event_t event;
   iolink_al_event_state_t next_state;
   iolink_fsm_al_event_event_t (
      *action) (iolink_port_t * port, iolink_fsm_al_event_event_t event);
} iolink_fsm_al_event_transition_t;

typedef struct iolink_fsm_al_event_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_al_event_transition_t * transitions;
} iolink_fsm_al_event_state_transitions_t;

/* Declare prototypes of static functions as to comply with MISRA-071_a-3 */
static iolink_smi_errortypes_t al_status_to_errortype (
   const iolink_status_t stat);
static iolink_smi_errortypes_t al_err_to_errortype (
   iolink_al_port_t * al,
   bool is_isdu);
static iolink_fsm_al_od_event_t al_od_busy (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_build_dl_service (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_send_al_cnf (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_send_abort_al_cnf (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_isdu_read_write (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection);
static iolink_fsm_al_od_event_t al_od_isdu_read (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_isdu_write (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_param_read (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_param_write (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_od_event_t al_od_param_next_rw (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static void iolink_al_od_event (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event);
static iolink_fsm_al_event_event_t al_event_wait_read (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event);
static iolink_fsm_al_event_event_t al_event_du_handle (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event);
static iolink_fsm_al_event_event_t al_event_idle (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event);
static void iolink_al_event_event (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event);
static void al_read_write_cb (iolink_job_t * job);
static void al_dl_event_ind_cb (iolink_job_t * job);
static void dl_control_ind_cb (iolink_job_t * job);
static void al_dl_readparam_cnf_cb (iolink_job_t * job);
static void al_dl_writeparam_cnf_cb (iolink_job_t * job);
static void al_dl_isdu_transport_cnf_cb (iolink_job_t * job);
static void al_abort_cb (iolink_job_t * job);
static void al_event_rsp_cb (iolink_job_t * job);
static void al_event_req_cb (iolink_job_t * job);

static iolink_smi_errortypes_t al_err_to_errortype (
   iolink_al_port_t * al,
   bool is_isdu)
{
   iolink_rwdirection_t direction = al->service.direction;
   iolink_status_t stat           = al->service.errinfo;
   iservice_t qualifier           = al->service.qualifier;

   iolink_smi_errortypes_t errortype = IOLINK_SMI_ERRORTYPE_NONE;

   if (is_isdu)
   {
      /* If we got checksum error, we cannot read back errortype */
      if (stat == IOLINK_STATUS_ISDU_CHECKSUM_ERROR)
      {
         errortype = IOLINK_SMI_ERRORTYPE_M_ISDU_CHECKSUM;
      }
      else if (direction == IOLINK_RWDIRECTION_READ)
      {
         switch (qualifier)
         {
         case IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG:
         {
            if (al->service.data_len == 2)
            {
               errortype = al->service.data_read[0] << 8 |
                           al->service.data_read[1];
            }
            else
            {
               errortype = IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO how to know
                                                         // error type?
            }
            break;
         }
         case IOL_ISERVICE_DEVICE_READ_RESPONSE_POS:
            break; /* ISDU read ok */
         default:
            LOG_ERROR (
               IOLINK_AL_LOG,
               "AL: ISDU_read_cnf: Got unexpected qualifier 0x%02x\n",
               qualifier);
            errortype = IOLINK_SMI_ERRORTYPE_APP_DEV;
            break;
         }
      }
      else if (direction == IOLINK_RWDIRECTION_WRITE)
      {
         switch (qualifier)
         {
         case IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG:
         {
            if (al->service.data_len == 2)
            {
               errortype = al->service.data_read[0] << 8 |
                           al->service.data_read[1];
            }
            else
            {
               errortype = IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO how to know
                                                         // error type?
            }
            break;
         }
         case IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS:
            break; /* ISDU write ok */
         case IOL_ISERVICE_DEVICE_NO_SERVICE:
            if (stat == IOLINK_STATUS_NO_ERROR)
            {
               errortype = IOLINK_SMI_ERRORTYPE_COM_ERR;
            }
            break;
         default:
            LOG_DEBUG (
               IOLINK_AL_LOG,
               "AL: ISDU_write_cnf: Got unexpected qualifier 0x%02x\n",
               qualifier);
            break;
         }
      }
   }

   if (errortype == IOLINK_SMI_ERRORTYPE_NONE)
   {
      errortype = al_status_to_errortype (stat);
   }

   return errortype;
}

static iolink_smi_errortypes_t al_status_to_errortype (
   const iolink_status_t stat)
{
   iolink_smi_errortypes_t errortype;

   switch (stat)
   {
   case IOLINK_STATUS_NO_ERROR:
      errortype = IOLINK_SMI_ERRORTYPE_NONE;
      break;
   case IOLINK_STATUS_ISDU_ILLEGAL_SERVICE_PRIMITIVE:
      errortype = IOLINK_SMI_ERRORTYPE_M_ISDU_ILLEGAL;
      break;
   case IOLINK_STATUS_PARITY_ERROR:
   case IOLINK_STATUS_FRAMING_ERROR:
   case IOLINK_STATUS_OVERRUN:
   case IOLINK_STATUS_NO_COMM:
   case IOLINK_STATUS_STATE_CONFLICT:
   case IOLINK_STATUS_VALID:
   case IOLINK_STATUS_PARAMETER_CONFLICT:
   case IOLINK_STATUS_ISDU_NOT_SUPPORTED:
   case IOLINK_STATUS_VALUE_OUT_OF_RANGE:
   case IOLINK_STATUS_ISDU_ABORT:
      errortype = IOLINK_SMI_ERRORTYPE_APP_DEV; // TODO how to know error
                                                // type?
      break;
   case IOLINK_STATUS_INVALID:
      /* No ISDU service */
      errortype = IOLINK_SMI_ERRORTYPE_COM_ERR;
      break;
   case IOLINK_STATUS_ISDU_CHECKSUM_ERROR:
      errortype = IOLINK_SMI_ERRORTYPE_M_ISDU_CHECKSUM;
      break;
   case IOLINK_STATUS_ISDU_TIMEOUT:
      errortype = IOLINK_SMI_ERRORTYPE_I_SERVICE_TIMEOUT;
      break;
   default:
      LOG_ERROR (IOLINK_AL_LOG, "AL: Got unexpected IOLINK_ERROR 0x%02x\n", stat);
      errortype = IOLINK_SMI_ERRORTYPE_APP_DEV;
      break;
   }

   return errortype;
}

static iolink_fsm_al_od_event_t al_od_busy (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);
   iolink_job_t * job    = al->service.job_req;

   switch (job->type)
   {
   case IOLINK_JOB_AL_READ_REQ:
      job->al_read_req.al_read_cb (
         port,
         0,
         NULL,
         IOLINK_SMI_ERRORTYPE_SERVICE_TEMP_UNAVAILABLE);
      break;
   case IOLINK_JOB_AL_WRITE_REQ:
      job->al_write_req.al_write_cb (
         port,
         IOLINK_SMI_ERRORTYPE_SERVICE_TEMP_UNAVAILABLE);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_build_dl_service (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   iolink_al_port_t * al              = iolink_get_al_ctx (port);
   iolink_job_t * job                 = al->service.job_req;
   uint16_t index                     = 0;
   bool isdu_flag                     = true; // TODO where do we get this from?
   iolink_fsm_al_od_event_t res_event = AL_OD_EVENT_arg_err;

   switch (job->type)
   {
   case IOLINK_JOB_AL_READ_REQ:
      al->service.data_len = 0;
      index = al->service.index  = job->al_read_req.index;
      al->service.subindex       = job->al_read_req.subindex;
      al->service.direction      = IOLINK_RWDIRECTION_READ;
      al->service.al_read_cnf_cb = job->al_read_req.al_read_cb;

      if (index <= 1)
      {
         res_event = AL_OD_EVENT_param_read;
      }
      else if (index >= 2 && isdu_flag)
      {
         res_event = AL_OD_EVENT_isdu_read;
      }
      break;
   case IOLINK_JOB_AL_WRITE_REQ:
      index = al->service.index   = job->al_write_req.index;
      al->service.subindex        = job->al_write_req.subindex;
      al->service.data_len        = job->al_write_req.length;
      al->service.direction       = IOLINK_RWDIRECTION_WRITE;
      al->service.data_write      = job->al_write_req.data;
      al->service.al_write_cnf_cb = job->al_write_req.al_write_cb;

      if (index == 1)
      {
         res_event = AL_OD_EVENT_param_write_1;
      }
      else if ((index == 2) && !isdu_flag)
      {
         res_event = AL_OD_EVENT_param_write_2;
      }
      else if ((index >= 2) && isdu_flag)
      {
         res_event = AL_OD_EVENT_isdu_write;
      }
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   if (res_event == AL_OD_EVENT_arg_err)
   {
      al->service.errinfo = IOLINK_STATUS_PARAMETER_CONFLICT;
   }

   return res_event;
}

static iolink_fsm_al_od_event_t al_od_send_al_cnf (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   iolink_al_port_t * al             = iolink_get_al_ctx (port);
   iolink_smi_errortypes_t errortype = IOLINK_SMI_ERRORTYPE_NONE;

   switch (event)
   {
   case AL_OD_EVENT_abort: /* T9 and T11, + T16 */
      // TODO All current DL service actions are abandoned.... -> and a negative
      // AL service confirmation is prepared
      /* fall through */
   case AL_OD_EVENT_arg_err: /* T2 + T16 */
      errortype = al_err_to_errortype (al, false);

      /* Prepare negative AL service confirmation */
      if (al->service.direction == IOLINK_RWDIRECTION_READ)
      {
         al->service.al_read_cnf_cb (port, 0, NULL, errortype);
      }
      else if (al->service.direction == IOLINK_RWDIRECTION_WRITE)
      {
         al->service.al_write_cnf_cb (port, errortype);
      }
      break;
   case AL_OD_EVENT_readparam_cnf: /* T13 + T16 */
      errortype = al_err_to_errortype (al, false);
      al->service.al_read_cnf_cb (
         port,
         al->service.data_len,
         al->service.data_read,
         errortype);
      break;
   case AL_OD_EVENT_isdutransport_cnf: /* T15 + T16 */
   {
      errortype = al_err_to_errortype (al, true);

      if (al->service.direction == IOLINK_RWDIRECTION_READ)
      {
         al->service.al_read_cnf_cb (
            port,
            al->service.data_len,
            al->service.data_read,
            errortype);
      }
      else if (al->service.direction == IOLINK_RWDIRECTION_WRITE)
      {
         al->service.al_write_cnf_cb (port, errortype);
      }
      break;
   }
   case AL_OD_EVENT_writeparam_cnf: /* T14 + T16 */
      errortype = al_err_to_errortype (al, false);
      al->service.al_write_cnf_cb (port, errortype);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_send_abort_al_cnf (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   if (event == AL_OD_EVENT_abort_port) /* T17 */
   {
      // TODO send negative response for portx AL_Read_cnf/AL_Write_cnf!?
   }

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_isdu_read_write (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection)
{
   iolink_isdu_vl_t valuelist;
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   valuelist.index     = al->service.index;
   valuelist.subindex  = al->service.subindex;
   valuelist.readwrite = rwdirection;

   if (rwdirection == IOLINK_RWDIRECTION_READ)
   {
      valuelist.data_read = al->service.data_read;
   }
   else
   {
      valuelist.data_write = al->service.data_write;
   }
   valuelist.length = al->service.data_len;

   if (DL_ISDUTransport_req (port, &valuelist) != IOLINK_ERROR_NONE)
   {
      LOG_ERROR (
         IOLINK_AL_LOG,
         "AL: bad %s DL_ISDUTransport_req!\n",
         (rwdirection == IOLINK_RWDIRECTION_READ) ? "Read" : "Write");
   }

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_isdu_read (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   return al_od_isdu_read_write (port, IOLINK_RWDIRECTION_READ);
}

static iolink_fsm_al_od_event_t al_od_isdu_write (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   return al_od_isdu_read_write (port, IOLINK_RWDIRECTION_WRITE);
}

static iolink_fsm_al_od_event_t al_od_param_read (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   CC_ASSERT (al != NULL);

   DL_ReadParam_req (port, al->service.index);

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_param_write (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   // TODO should we write multiple bytes?
   switch (event)
   {
   case AL_OD_EVENT_param_write_1: /* T4 */
      // Prepare DL_WriteParam of index 1
      CC_ASSERT (al->service.index == 1);
      break;
   case AL_OD_EVENT_param_write_2: /* T5 */
      CC_ASSERT (al->service.index == 2);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   DL_WriteParam_req (port, al->service.index, al->service.data_write[0]);

   return AL_OD_EVENT_NONE;
}

static iolink_fsm_al_od_event_t al_od_param_next_rw (
   iolink_port_t * port,
   iolink_fsm_al_od_event_t event)
{
   // TODO Call next DL_ReadParam or DL_WriteParam service, if not all OD are
   // transferred
   return AL_OD_EVENT_NONE;
}

/* AL OD FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 8.3.2.1 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_al_od_transition_t al_od_trans_s0[] = {
   /* OnReq_Idle_0 */
   {AL_OD_EVENT_service, AL_OD_STATE_Build_DL_Service, al_od_build_dl_service}, /* T1 */
   {AL_OD_EVENT_abort_port, AL_OD_STATE_OnReq_Idle, al_od_send_abort_al_cnf}, /* T17 */
};

static const iolink_fsm_al_od_transition_t al_od_trans_s1[] = {
   /* Build_DL_Service_1 */
   {AL_OD_EVENT_arg_err, AL_OD_STATE_OnReq_Idle, al_od_send_al_cnf}, /* T2 + T16
                                                                      */
   {AL_OD_EVENT_param_read, AL_OD_STATE_Await_DL_param_cnf, al_od_param_read}, /* T3 */
   {AL_OD_EVENT_param_write_1,
    AL_OD_STATE_Await_DL_param_cnf,
    al_od_param_write}, /* T4 */
   {AL_OD_EVENT_param_write_2,
    AL_OD_STATE_Await_DL_param_cnf,
    al_od_param_write}, /* T5 */
   {AL_OD_EVENT_isdu_read, AL_OD_STATE_Await_DL_ISDU_cnf, al_od_isdu_read}, /* T6
                                                                             */
   {AL_OD_EVENT_isdu_write, AL_OD_STATE_Await_DL_ISDU_cnf, al_od_isdu_write}, /* T7 */
};

static const iolink_fsm_al_od_transition_t al_od_trans_s2[] = {
   /* Await_DL_Param_cnf_2 */
   {AL_OD_EVENT_service, AL_OD_STATE_Await_DL_param_cnf, al_od_busy}, /* T8 */
   {AL_OD_EVENT_abort, AL_OD_STATE_OnReq_Idle, al_od_send_al_cnf}, /* T9 + T16
                                                                    */
   {AL_OD_EVENT_rwparam, AL_OD_STATE_Await_DL_param_cnf, al_od_param_next_rw}, /* T10 */
   {AL_OD_EVENT_readparam_cnf, AL_OD_STATE_OnReq_Idle, al_od_send_al_cnf}, /* T13
                                                                              +
                                                                              T16
                                                                            */
   {AL_OD_EVENT_writeparam_cnf, AL_OD_STATE_OnReq_Idle, al_od_send_al_cnf}, /* T14
                                                                               + T16 */
};

static const iolink_fsm_al_od_transition_t al_od_trans_s3[] = {
   /* Await_DL_ISDU_cnf_3 */
   {AL_OD_EVENT_abort, AL_OD_STATE_OnReq_Idle, al_od_send_al_cnf}, /* T11 + T16
                                                                    */
   {AL_OD_EVENT_service, AL_OD_STATE_Await_DL_ISDU_cnf, al_od_busy}, /* T12 */
   {AL_OD_EVENT_isdutransport_cnf,
    AL_OD_STATE_OnReq_Idle,
    al_od_send_al_cnf}, /* T15 + T16 */
};

/* The index is the state in this array */
static const iolink_fsm_al_od_state_transitions_t iolink_al_od_fsm[] = {
   {/* OnReq_Idle_0 */
    .number_of_trans = NELEMENTS (al_od_trans_s0),
    .transitions     = al_od_trans_s0},
   {/* Build_DL_Service_1 */
    .number_of_trans = NELEMENTS (al_od_trans_s1),
    .transitions     = al_od_trans_s1},
   {/* Await_DL_Param_cnf_2 */
    .number_of_trans = NELEMENTS (al_od_trans_s2),
    .transitions     = al_od_trans_s2},
   {/* Await_DL_ISDU_cnf_3 */
    .number_of_trans = NELEMENTS (al_od_trans_s3),
    .transitions     = al_od_trans_s3},
};

/**
 * Trigger AL OD FSM state transition
 *
 * This function triggers an AL OD state transition according to the event.
 *
 * @param port          Port handle
 * @param event         AL OD event
 */
static void iolink_al_od_event (iolink_port_t * port, iolink_fsm_al_od_event_t event)
{
   do
   {
      int i;
      iolink_al_port_t * al         = iolink_get_al_ctx (port);
      iolink_al_od_state_t previous = al->od_state;
      const iolink_fsm_al_od_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_al_od_fsm[previous].number_of_trans; i++)
      {
         if (iolink_al_od_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_al_od_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         LOG_ERROR (
            IOLINK_AL_LOG,
            "AL OD (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_al_od_state_literals[previous],
            iolink_al_od_event_literals[event]);
         return;
      }

      /* Transition to next state */
      al->od_state = next_trans->next_state;

      LOG_DEBUG (
         IOLINK_AL_LOG,
         "AL OD (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_al_od_event_literals[event],
         iolink_al_od_state_literals[previous],
         iolink_al_od_state_literals[al->od_state]);

      event = next_trans->action (port, event);
   } while (event != AL_OD_EVENT_NONE);
}

static iolink_fsm_al_event_event_t al_event_wait_read (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event)
{
   /* Wait for more data */
   return AL_EVENT_EVENT_NONE;
}

static iolink_fsm_al_event_event_t al_event_du_handle (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   AL_Event_ind (port, al->event.event_cnt, al->event.events);
   /* Now wait for diagnosis unit to acknowledge, with AL_Event_rsp() */
   return AL_EVENT_EVENT_NONE;
}

static iolink_fsm_al_event_event_t al_event_idle (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   switch (event)
   {
   case AL_EVENT_EVENT_al_event_rsp:
      /* Event is confirmed in DL */
      break;
   case AL_EVENT_EVENT_al_event_req:
      // TODO call AL_Event_ind (port, ???);
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   /* Ready to accept DL_Events from DL */
   al->event.event_cnt = 0;

   return AL_EVENT_EVENT_NONE;
}

/* AL Event FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 8.3.3.1
 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_al_event_transition_t al_event_trans_s0[] = {
   /* Event_idle_1 */
   {AL_EVENT_EVENT_dl_event_ind_more,
    AL_EVENT_STATE_Read_Event_Set,
    al_event_wait_read}, /* T3 */
   {AL_EVENT_EVENT_dl_event_ind_done,
    AL_EVENT_STATE_DU_Event_handling,
    al_event_du_handle}, /* T3 + T5 */
   {AL_EVENT_EVENT_al_event_req, AL_EVENT_STATE_Event_idle, al_event_idle}, /* T7
                                                                             */
};

static const iolink_fsm_al_event_transition_t al_event_trans_s1[] = {
   /* Read_Event_Set_2 */
   {AL_EVENT_EVENT_dl_event_ind_more,
    AL_EVENT_STATE_Read_Event_Set,
    al_event_wait_read}, /* T4 */
   {AL_EVENT_EVENT_dl_event_ind_done,
    AL_EVENT_STATE_DU_Event_handling,
    al_event_du_handle}, /* T5 */
};

static const iolink_fsm_al_event_transition_t al_event_trans_s2[] = {
   /* DU_Event_handling_3 */
   {AL_EVENT_EVENT_al_event_rsp, AL_EVENT_STATE_Event_idle, al_event_idle}, /* T6
                                                                             */
};

/* The index is the state in this array */
static const iolink_fsm_al_event_state_transitions_t iolink_al_event_fsm[] = {

   {/* Event_Inactive_0 */
    .number_of_trans = 0,
    .transitions     = NULL},
   {/* Event_idle_1 */
    .number_of_trans = NELEMENTS (al_event_trans_s0),
    .transitions     = al_event_trans_s0},
   {/* Read_Event_Set_2 */
    .number_of_trans = NELEMENTS (al_event_trans_s1),
    .transitions     = al_event_trans_s1},
   {/* DU_Event_handling_3 */
    .number_of_trans = NELEMENTS (al_event_trans_s2),
    .transitions     = al_event_trans_s2},
};

/**
 * Trigger AL Event FSM state transition
 *
 * This function triggers an AL Event state transition according to the event.
 *
 * @param port          Port handle
 * @param event         AL Event event
 */
static void iolink_al_event_event (
   iolink_port_t * port,
   iolink_fsm_al_event_event_t event)
{
   do
   {
      int i;
      iolink_al_port_t * al            = iolink_get_al_ctx (port);
      iolink_al_event_state_t previous = al->event_state;
      const iolink_fsm_al_event_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_al_event_fsm[previous].number_of_trans; i++)
      {
         if (iolink_al_event_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_al_event_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         LOG_ERROR (
            IOLINK_AL_LOG,
            "AL Event (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_al_event_state_literals[previous],
            iolink_al_event_event_literals[event]);
         return;
      }

      /* Transition to next state */
      al->event_state = next_trans->next_state;

      LOG_DEBUG (
         IOLINK_AL_LOG,
         "AL Event (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_al_event_event_literals[event],
         iolink_al_event_state_literals[previous],
         iolink_al_event_state_literals[al->event_state]);

      event = next_trans->action (port, event);
   } while (event != AL_EVENT_EVENT_NONE);
}

/* AL job callback functions */
static void al_read_write_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   al->service.job_req = job;
   iolink_al_od_event (port, AL_OD_EVENT_service);
}

static void al_dl_event_ind_cb (iolink_job_t * job)
{
   iolink_fsm_al_event_event_t res_event;
   iolink_port_t * port  = job->port;
   iolink_al_port_t * al = iolink_get_al_ctx (port);
   diag_entry_t * evt    = &(al->event.events[al->event.event_cnt]);

   // Collect the complete set (1 to 6) of DL_Events of the current EventTrigger
   CC_ASSERT (al->event.event_cnt < 6);
   memcpy (evt, &job->dl_event_ind.event, sizeof (diag_entry_t));
   al->event.event_cnt++;

   if (job->dl_event_ind.eventsleft)
   {
      res_event = AL_EVENT_EVENT_dl_event_ind_more;
   }
   else
   {
      res_event = AL_EVENT_EVENT_dl_event_ind_done;
   }

   iolink_al_event_event (port, res_event);
}

static void dl_control_ind_cb (iolink_job_t * job)
{
   AL_Control_ind (job->port, job->dl_control_ind.controlcode);
}

static void al_dl_readparam_cnf_cb (iolink_job_t * job)
{
   iolink_al_port_t * al = iolink_get_al_ctx (job->port);

   al->service.data_read[al->service.data_len++] = job->dl_rw_cnf.val;
   al->service.errinfo                           = job->dl_rw_cnf.stat;

   iolink_al_od_event (job->port, AL_OD_EVENT_readparam_cnf);
}

static void al_dl_writeparam_cnf_cb (iolink_job_t * job)
{
   bool octets_left               = false;
   iolink_fsm_al_od_event_t event = AL_OD_EVENT_writeparam_cnf;
   iolink_al_port_t * al          = iolink_get_al_ctx (job->port);

   // TODO check job->dl_rw_cnf.stat;

   al->service.errinfo = job->dl_rw_cnf.stat;

   if ((al->service.errinfo == IOLINK_STATUS_NO_ERROR) && octets_left) // TODO
                                                                       // increase
                                                                       // write
                                                                       // pointer
   {
      event = AL_OD_EVENT_rwparam;
   }

   iolink_al_od_event (job->port, event);
}

static void al_dl_isdu_transport_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   al->service.errinfo   = job->dl_rw_cnf.stat;
   al->service.qualifier = job->dl_rw_cnf.qualifier;
   iolink_al_od_event (port, AL_OD_EVENT_isdutransport_cnf);
}

static void al_abort_cb (iolink_job_t * job)
{
   iolink_fsm_al_od_event_t event = AL_OD_EVENT_abort;
   iolink_port_t * port           = job->port;
   iolink_al_port_t * al          = iolink_get_al_ctx (port);

   al->service.errinfo  = IOLINK_STATUS_ISDU_ABORT; // TODO what to use here?
   al->service.data_len = 0;

   if (al->od_state == AL_OD_STATE_OnReq_Idle)
   {
      event = AL_OD_EVENT_abort_port;
   }

   iolink_al_od_event (port, event);
}

static void al_event_rsp_cb (iolink_job_t * job)
{
   iolink_al_event_event (job->port, AL_EVENT_EVENT_al_event_rsp);
}

static void al_event_req_cb (iolink_job_t * job)
{
   iolink_al_event_event (job->port, AL_EVENT_EVENT_al_event_req);
}

/* Stack internal API */
void iolink_al_init (iolink_port_t * port)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   memset (al, 0, sizeof (iolink_al_port_t));
   al->mtx_pdin    = os_mutex_create();
   al->od_state    = AL_OD_STATE_OnReq_Idle;
   al->event_state = AL_EVENT_STATE_Event_idle;
}

iolink_error_t AL_Read_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   void (*al_read_cnf_cb) (
      iolink_port_t * port,
      uint8_t len,
      const uint8_t * data,
      iolink_smi_errortypes_t errortype))
{
   iolink_job_t * job        = iolink_fetch_avail_job (port);
   job->al_read_req.index    = index;
   job->al_read_req.subindex = subindex;
   CC_ASSERT (al_read_cnf_cb != NULL);
   job->al_read_req.al_read_cb = al_read_cnf_cb;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_READ_REQ,
      al_read_write_cb);

   return IOLINK_ERROR_NONE;
}

/* Requirement: Changes to data is not allowed, until after al_Write_cnf_cb */
iolink_error_t AL_Write_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   const uint8_t * data,
   void (*al_write_cnf_cb) (iolink_port_t * port, iolink_smi_errortypes_t errortype))
{
   iolink_job_t * job         = iolink_fetch_avail_job (port);
   job->al_write_req.index    = index;
   job->al_write_req.subindex = subindex;
   job->al_write_req.length   = len;
   job->al_write_req.data     = data;
   CC_ASSERT (al_write_cnf_cb != NULL);
   job->al_write_req.al_write_cb = al_write_cnf_cb;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_WRITE_REQ,
      al_read_write_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_SetOutput_req (iolink_port_t * port, uint8_t * data)
{
   return DL_PDOutputUpdate_req (port, data);
}

iolink_error_t AL_Control_req (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   return DL_Control_req (port, controlcode);
}

iolink_error_t AL_GetInput_req (iolink_port_t * port, uint8_t * len, uint8_t * data)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   os_mutex_lock (al->mtx_pdin);
   *len = al->pdin_data_len;
   memcpy (data, al->pdin_data, *len);
   os_mutex_unlock (al->mtx_pdin);

   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_GetInputOutput_req (
   iolink_port_t * port,
   uint8_t * len,
   uint8_t * data)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   os_mutex_lock (al->mtx_pdin);
   /* PDIn data length */
   uint8_t pdin_len = al->pdin_data_len;
   data[0]          = pdin_len;
   /* PDIn_data[0] */
   uint8_t * pdin_data = &data[1];
   memcpy (pdin_data, al->pdin_data, pdin_len);
   /* PDOut data length */
   uint8_t * pdout_len = &data[pdin_len + 1];
   /* PDOut_data[0] */
   uint8_t * pdout_data = &data[pdin_len + 2];

   iolink_error_t error = DL_PDOutputGet_req (port, pdout_len, pdout_data);

   if (error == IOLINK_ERROR_NONE)
   {
      *len = 1 + pdin_len + 1 + *pdout_len;
   }
   os_mutex_unlock (al->mtx_pdin);

   return error;
}

void DL_Event_ind (
   iolink_port_t * port,
   uint16_t eventcode,
   uint8_t event_qualifier,
   uint8_t eventsleft)
{
   iolink_job_t * job                      = iolink_fetch_avail_job (port);
   job->dl_event_ind.eventsleft            = eventsleft;
   job->dl_event_ind.event.event_code      = eventcode;
   job->dl_event_ind.event.event_qualifier = event_qualifier;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_EVENT_IND,
      al_dl_event_ind_cb);
}

void DL_Control_ind (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   // TODO check why this is called frequently
   iolink_job_t * job              = iolink_fetch_avail_job (port);
   job->dl_control_ind.controlcode = controlcode;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_CONTROL_IND,
      dl_control_ind_cb);
}

void DL_ReadParam_cnf (iolink_port_t * port, uint8_t value, iolink_status_t errinfo)
{
   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.val  = value;
   job->dl_rw_cnf.stat = errinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_READPARAM_CNF,
      al_dl_readparam_cnf_cb);
}

void DL_WriteParam_cnf (iolink_port_t * port, iolink_status_t errinfo)
{
   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.stat = errinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_WRITEPARAM_CNF,
      al_dl_writeparam_cnf_cb);
}

void DL_ISDUTransport_cnf (
   iolink_port_t * port,
   uint8_t * data,
   uint8_t length,
   iservice_t qualifier,
   iolink_status_t errinfo)
{
   iolink_job_t * job       = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.stat      = errinfo;
   job->dl_rw_cnf.qualifier = qualifier;
   iolink_al_port_t * al    = iolink_get_al_ctx (port);

   if (al->service.direction == IOLINK_RWDIRECTION_READ)
   {
      if (errinfo == IOLINK_STATUS_NO_ERROR)
      {
         if ((data != NULL) && (length <= sizeof (al->service.data_read)))
         {
            // TODO can't we pass data_read pointer when we make the initial
            // ISDUTransport_req call?
            memcpy (al->service.data_read, data, length);
            al->service.data_len = length;
         }
         else
         {
            LOG_ERROR (
               IOLINK_AL_LOG,
               "%u: AL: %s, data = %p, length (%u) > %u\n",
               iolink_get_portnumber (port),
               __func__,
               data,
               length,
               (unsigned int)sizeof (al->service.data_read));
         }
      }
   }
   else if (
      (al->service.direction == IOLINK_RWDIRECTION_WRITE) &&
      (qualifier == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG))
   {
      if (data && length <= sizeof (al->service.data_read))
      {
         // TODO can't we pass data_read pointer when we make the initial
         // ISDUTransport_req call?
         memcpy (al->service.data_read, data, length);
         al->service.data_len = length;
      }
      else
      {
         LOG_ERROR (
            IOLINK_AL_LOG,
            "%u: AL: %s, data = %p, length (%u) > %u\n",
            iolink_get_portnumber (port),
            __func__,
            data,
            length,
            (unsigned int)sizeof (al->service.data_read));
      }
   }

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_ISDU_TRANS_CNF,
      al_dl_isdu_transport_cnf_cb);
}

void DL_PDInputTransport_ind (iolink_port_t * port, uint8_t * pdin_data, uint8_t length)
{
   iolink_al_port_t * al = iolink_get_al_ctx (port);

   CC_ASSERT (length <= IOLINK_PD_MAX_SIZE);

   os_mutex_lock (al->mtx_pdin);
   memcpy (al->pdin_data, pdin_data, length);
   al->pdin_data_len = length;
   iolink_post_job_pd_event (port, 100, length, pdin_data);
   os_mutex_unlock (al->mtx_pdin);
   AL_NewInput_ind (port);
}

#ifndef UNIT_TEST
/* TODO move this to diagnosis unit? */
__attribute__ ((weak)) void AL_Event_ind (
   iolink_port_t * port,
   uint8_t event_cnt,
   diag_entry_t events[6])
{
   arg_block_devevent_t arg_block_devevent;
   uint8_t i;

   LOG_DEBUG (IOLINK_AL_LOG, "Got eventcnt = %u\n", event_cnt);

   AL_Event_rsp (port);

   for (i = 0; i < 6; i++)
   {
      if (events[i].event_code == IOLINK_EVENTCODE_DEV_DS_UPLOAD_REQ)
      {
         DS_Upload (port);
         // TODO report the remaining events
         return;
      }
   }

   memset (&arg_block_devevent, 0, sizeof (arg_block_devevent_t));
   arg_block_devevent.arg_block.id = IOLINK_ARG_BLOCK_ID_DEV_EVENT;
   if (event_cnt > 6)
   {
      LOG_ERROR (IOLINK_AL_LOG, "event_cnt (%u) > 6\n", event_cnt);
      return;
   }
   memcpy (arg_block_devevent.diag_entry, events, sizeof (diag_entry_t) * event_cnt);
   arg_block_devevent.event_count = event_cnt;

   iolink_smi_cnf (
      port,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      sizeof (arg_block_devevent_t),
      (arg_block_t *)&arg_block_devevent);
}
#endif /* UNIT_TEST */

/* Public API? */
iolink_error_t AL_Abort (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_api_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_ABORT,
      al_abort_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_Event_rsp (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_EVENT_RSP,
      al_event_rsp_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t AL_Event_req (iolink_port_t * port /* TODO */)
{
   iolink_job_t * job = iolink_fetch_avail_api_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_EVENT_REQ,
      al_event_req_cb);

   return IOLINK_ERROR_NONE;
}

#ifdef UNIT_TEST
void iolink_post_job_with_type_and_callback (
   iolink_port_t * port,
   iolink_job_t * job,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job))
{
   job->type     = type;
   job->callback = callback;

   iolink_post_job (port, job);
}
#endif
