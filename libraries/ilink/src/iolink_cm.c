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
#define iolink_fetch_avail_job     mock_iolink_fetch_avail_job
#define iolink_fetch_avail_api_job mock_iolink_fetch_avail_api_job
#define iolink_post_job            mock_iolink_post_job
#define SM_Operate                 mock_SM_Operate
#define SM_SetPortConfig_req       mock_SM_SetPortConfig_req
#define DS_Delete                  mock_DS_Delete
#define DS_Startup                 mock_DS_Startup
#define OD_Start                   mock_OD_Start
#define OD_Stop                    mock_OD_Stop
#define PD_Start                   mock_PD_Start
#define PD_Stop                    mock_PD_Stop
// TODO #define DU_Start mock_DU_Start
// TODO #define DU_Stop mock_DU_Stop

#endif /* UNIT_TEST */

#include "iolink_cm.h"
#include "iolink_ds.h"  /* DS_Delete DS_Startup DS_Chk_Cfg DS_Init */
#include "iolink_ode.h" /* OD_Start, OD_Stop */
#include "iolink_pde.h" /* PD_Start, PD_Stop */
#include "iolink_sm.h"  /* SM_Operate, SM_SetPortConfig_req */
#ifdef UNIT_TEST
#include "iolink_al.h"
#endif

#include "osal_log.h"

#include "iolink_types.h"
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_post_job, iolink_get_portnumber, iolink_get_port_info, iolink_get_transmission_rate, iolink_get_paramlist */

/**
 * @file
 * @brief Configuration Manager
 *
 */

static const char * const iolink_cm_state_literals[] = {
   "CheckPortMode",
   "SM_Startup",
   "DS_ParamManager",
   "PortFault",
   "WaitingOnOperate",
   "Port_Active",
   "Port_DIDO",
   "Port_Deactivated",
   "LAST",
};

static const char * const iolink_cm_event_literals[] = {
   "NONE",
   "IOL_MANUAL",    /* T1 */
   "IOL_AUTOSTART", /* T2 */
   "SM_COMREADY",   /* T3 */
   "SM_FAULT",      /* T4 */
   "DS_Ready",      /* T5 */
   "DS_Fault",      /* T6 */
   "SM_OPERATE",    /* T7 */
   "SM_COMLOST",    /* T8 */
   "DI_CQ",         /* T9 */
   "DO_CQ",         /* T10 */
   "DEACTIVATED",   /* T11 */
   "CFG_CHANGE",    /* T12, T13 and T15 */
   "DS_CHANGE",     /* T14 */
   "UNKNOWN",       /* T16 */
   "SM_COMLOST_DS", /* T8 */
   "LAST",
};

typedef struct iolink_fsm_cm_transition
{
   iolink_fsm_cm_event_t event;
   iolink_cm_state_t next_state;
   iolink_fsm_cm_event_t (
      *action) (iolink_port_t * port, iolink_fsm_cm_event_t event);
} iolink_fsm_cm_transition_t;

typedef struct iolink_fsm_cm_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_cm_transition_t * transitions;
} iolink_fsm_cm_state_transitions_t;

/**
 * Trigger CM FSM state transition
 *
 * This function triggers a CM state transition according to the event.
 */
static void iolink_cm_event (iolink_port_t * port, iolink_fsm_cm_event_t event);

/* Actions taken when transitioning to a new state. See iolink_cm_event(). */
static iolink_fsm_cm_event_t cm_startup (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_dido (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_check_mode (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_comlost (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_comlost_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_cfg_change_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_ds_change (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_start_ds (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_ds_ready (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_ds_ready_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_sm_operate (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_port_fault (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);
static iolink_fsm_cm_event_t cm_deactivate (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event);

/* Callback functions to run on main thread */
static void cm_sm_port_mode_ind_cb (iolink_job_t * job);
static void cm_ds_ready_cb (iolink_job_t * job);
static void cm_ds_change_cb (iolink_job_t * job);
static void cm_ds_fault_cb (iolink_job_t * job);
static void cm_smi_masterident_cb (iolink_job_t * job);
static void cm_smi_portconfiguration_cb (iolink_job_t * job);
static void cm_smi_readbackportconfiguration_cb (iolink_job_t * job);
static void cm_smi_portstatus_cb (iolink_job_t * job);

/* Other functions */
static void iolink_cm_do_smi_port_event (
   iolink_port_t * port,
   iolink_eventcode_t eventcode);
static iolink_sm_target_mode_t portmode_to_target_mode (iolink_portmode_t portmode);
static void portconfiglist_to_parameterlist (
   const portconfiglist_t * cfg_list,
   iolink_smp_parameterlist_t * paraml);
static bool check_arg_block (
   iolink_job_t * job,
   iolink_arg_block_id_t cmp_ref_arg_block_id,
   uint16_t arg_block_len,
   iolink_arg_block_id_t cmp_exp_arg_block_id,
   iolink_smi_errortypes_t errortype);
static void set_port_config (
   iolink_port_t * port, bool ds_init);
static void init_port_info (
   iolink_port_t * port,
   iolink_port_status_info_t port_status_info,
   iolink_port_quality_info_t port_quality_info);
static iolink_error_t init_and_post_job (
   iolink_port_t * port,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job),
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

static iolink_sm_target_mode_t portmode_to_target_mode (iolink_portmode_t portmode)
{
   iolink_sm_target_mode_t target_mode = IOLINK_SMTARGET_MODE_INACTIVE;

   switch (portmode)
   {
   case IOLINK_PORTMODE_IOL_MAN:
      target_mode = IOLINK_SMTARGET_MODE_CFGCOM;
      break;
   case IOLINK_PORTMODE_IOL_AUTO:
      target_mode = IOLINK_SMTARGET_MODE_AUTOCOM;
      break;
   case IOLINK_PORTMODE_DI_CQ:
      target_mode = IOLINK_SMTARGET_MODE_DI;
      break;
   case IOLINK_PORTMODE_DO_CQ:
      target_mode = IOLINK_SMTARGET_MODE_DO;
      break;
   case IOLINK_PORTMODE_DEACTIVE:
      break;
   default:
      CC_ASSERT (0);
   }

   return target_mode;
}

static void portconfiglist_to_parameterlist (
   const portconfiglist_t * cfg_list,
   iolink_smp_parameterlist_t * paraml)
{
   memset (paraml, 0, sizeof (iolink_smp_parameterlist_t));

   paraml->cycletime = cfg_list->port_cycle_time;
   paraml->vendorid  = cfg_list->vendorid;
   paraml->deviceid  = cfg_list->deviceid;
   paraml->mode      = portmode_to_target_mode (cfg_list->portmode);

   switch (cfg_list->validation_backup)
   {
   case IOLINK_VALIDATION_CHECK_NO:
      paraml->inspectionlevel = IOLINK_INSPECTIONLEVEL_NO_CHECK;
      break;
   case IOLINK_VALIDATION_CHECK_V10:
      paraml->inspectionlevel = IOLINK_INSPECTIONLEVEL_IDENTICAL;
      paraml->revisionid      = IOL_DIR_PARAM_REV_V10;
      break;
   case IOLINK_VALIDATION_CHECK_V11_BAK_RESTORE:
   case IOLINK_VALIDATION_CHECK_V11_RESTORE:
   case IOLINK_VALIDATION_CHECK_V11:
      paraml->inspectionlevel = IOLINK_INSPECTIONLEVEL_TYPE_COMP;
      paraml->revisionid      = IOL_DIR_PARAM_REV_V11;
      break;
   default:
      CC_ASSERT (0);
   }
}

static void set_port_config (iolink_port_t * port, bool ds_init)
{
   iolink_cm_port_t * cm       = iolink_get_cm_ctx (port);
   portconfiglist_t * cfg_list = &cm->cfg_list;
   iolink_smp_parameterlist_t paraml;

   portconfiglist_to_parameterlist (cfg_list, &paraml);

   if (ds_init)
   {
      DS_Init (port, cfg_list);
   }
   else
   {
      paraml.mode = IOLINK_SMTARGET_MODE_CFGCOM;
   }

   SM_SetPortConfig_req (port, &paraml);
}

static iolink_fsm_cm_event_t cm_startup (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   iolink_cm_port_t * cm = iolink_get_cm_ctx (port);

   if (event == CM_EVENT_IOL_AUTOSTART || !DS_Chk_Cfg (port, &cm->cfg_list)) /* CM_EVENT_IOL_MANUAL */
   {
      DS_Delete (port);
   }

   set_port_config (port, true);

   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_dido (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);

   if (event == CM_EVENT_DI_CQ)
   {
      port_info->port_status_info = IOLINK_PORT_STATUS_INFO_DI;
   }
   else if (event == CM_EVENT_DO_CQ)
   {
      port_info->port_status_info = IOLINK_PORT_STATUS_INFO_DO;
   }

   DS_Delete (port);

   port_info->revisionid        = 0;
   port_info->transmission_rate = 0;
   port_info->vendorid          = 0;
   port_info->deviceid          = 0;
   port_info->port_quality_info = IOLINK_PORT_QUALITY_INFO_INVALID;

   set_port_config (port, true);

   return CM_EVENT_NONE;
}

/* New port configuration or COMLOST */
static iolink_fsm_cm_event_t cm_check_mode (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   portconfiglist_t * cfg_list;
   iolink_fsm_cm_event_t res_event = CM_EVENT_NONE;
   iolink_cm_port_t * cm           = iolink_get_cm_ctx (port);
   arg_block_portconfiglist_t * arg_block_portconfiglist =
      (arg_block_portconfiglist_t *)cm->smi_req.arg_block;

   if (event == CM_EVENT_CFG_CHANGE)
   {
      cfg_list = &arg_block_portconfiglist->configlist;
   }
   else // COMLOST
   {
      return CM_EVENT_NONE;
   }

   switch (cfg_list->portmode)
   {
   case IOLINK_PORTMODE_DEACTIVE: /* T11 */
      res_event = CM_EVENT_DEACTIVATED;
      break;
   case IOLINK_PORTMODE_DI_CQ: /* T9 */
      res_event = CM_EVENT_DI_CQ;
      break;
   case IOLINK_PORTMODE_DO_CQ: /* T10 */
      res_event = CM_EVENT_DO_CQ;
      break;
   case IOLINK_PORTMODE_IOL_AUTO: /* T2 */
      res_event = CM_EVENT_IOL_AUTOSTART;
      break;
   case IOLINK_PORTMODE_IOL_MAN: /* T1 */
      res_event = CM_EVENT_IOL_MANUAL;
      break;
   default:
      res_event = CM_EVENT_UNKNOWN; /* T16 */
      break;
   }

   switch (cfg_list->validation_backup)
   {
   case IOLINK_VALIDATION_CHECK_NO:
   case IOLINK_VALIDATION_CHECK_V10:
   case IOLINK_VALIDATION_CHECK_V11:
   case IOLINK_VALIDATION_CHECK_V11_BAK_RESTORE:
   case IOLINK_VALIDATION_CHECK_V11_RESTORE:
      break;
   default:
      res_event = CM_EVENT_UNKNOWN; /* T16 */
      break;
   }

   switch (cfg_list->iq_behavior)
   {
   case IOLINK_IQ_BEHAVIOR_NO_SUPPORT:
   case IOLINK_IQ_BEHAVIOR_DI:
   case IOLINK_IQ_BEHAVIOR_DO:
   case IOLINK_IQ_BEHAVIOR_RES1:
   case IOLINK_IQ_BEHAVIOR_RES2:
   case IOLINK_IQ_BEHAVIOR_POWER2:
      break;
   default:
      res_event = CM_EVENT_UNKNOWN; /* T16 */
      break;
   }

   // DU_Stop (port); // TODO
   PD_Stop (port);
   OD_Stop (port);

   if (event == CM_EVENT_CFG_CHANGE)
   {
      iolink_cm_port_t * cm              = iolink_get_cm_ctx (port);
      iolink_smi_service_req_t * smi_req = &cm->smi_req;
      iolink_arg_block_id_t ref_arg_block_id =
         smi_req->arg_block->id;

      if (res_event == CM_EVENT_UNKNOWN)
      {
         iolink_arg_block_id_t exp_arg_block_id = smi_req->exp_arg_block_id;

         iolink_smi_joberror_ind (
            port,
            exp_arg_block_id,
            ref_arg_block_id,
            IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT);
      }
      else
      {
         memcpy (&cm->cfg_list, cfg_list, sizeof (portconfiglist_t));
         iolink_smi_voidblock_cnf (port, ref_arg_block_id);
      }
   }

   return res_event;
}

static void iolink_cm_do_smi_port_event (
   iolink_port_t * port,
   iolink_eventcode_t eventcode)
{
   arg_block_portevent_t port_event;

   memset (&port_event, 0, sizeof (arg_block_portevent_t));
   port_event.arg_block.id = IOLINK_ARG_BLOCK_ID_PORT_EVENT;
   port_event.event.event_qualifier |= IOLINK_EVENT_INSTANCE_APPLICATION;
   port_event.event.event_qualifier |= IOLINK_EVENT_SOURCE_MASTER << 3;
   port_event.event.event_qualifier |= IOLINK_EVENT_TYPE_NOTIFICATION << 4;
   port_event.event.event_qualifier |= IOLINK_EVENT_MODE_SINGLE_SHOT << 6;

   port_event.event.event_code = eventcode;

   /* SMI_PortEvent_ind */
   iolink_smi_cnf (
      port,
      IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
      sizeof (arg_block_portevent_t),
      (arg_block_t *)&port_event);
}

static iolink_fsm_cm_event_t cm_comlost (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);

   port_info->port_status_info = IOLINK_PORT_STATUS_INFO_NO_DEV;
   /* SMI_PortEvent_ind */
   iolink_cm_do_smi_port_event (port, IOLINK_EVENTCODE_NO_DEV);

   return cm_check_mode (port, event);
}

static iolink_fsm_cm_event_t cm_comlost_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   /* Ignore COMLOST when we are in state DS_ParamManager_2
    * Let DS report the fault, then handle it as COMLOST
    */
   LOG_ERROR (IOLINK_CM_LOG, "CM: Ignore COMLOST when in DS_ParamManager_2\n");
   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_cfg_change_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   /* Ignore CFG_CHANGE when we are in state DS_ParamManager_2
    * We cannot abort/stop
    */
   LOG_ERROR (IOLINK_CM_LOG, "CM: Ignore CFG_CHANGE when in DS_ParamManager_2\n");
   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_ds_change (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   set_port_config (port, false);

   return CM_EVENT_NONE;
}

static void init_port_info (
   iolink_port_t * port,
   iolink_port_status_info_t port_status_info,
   iolink_port_quality_info_t port_quality_info)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);
   const iolink_smp_parameterlist_t * real_paramlist =
      iolink_get_paramlist (port);

   port_info->port_status_info = port_status_info;

   if (port_status_info != IOLINK_PORT_STATUS_INFO_DEACTIVATED)
   {
      port_info->revisionid        = real_paramlist->revisionid;
      port_info->transmission_rate = iolink_get_transmission_rate (port);
      port_info->vendorid          = real_paramlist->vendorid;
      port_info->deviceid          = real_paramlist->deviceid;
   }
   else
   {
      port_info->revisionid        = 0;
      port_info->transmission_rate = IOLINK_TRANSMISSION_RATE_NOT_DETECTED;
      port_info->vendorid          = 0;
      port_info->deviceid          = 0;
      port_info->cycletime         = 0;
   }

   port_info->port_quality_info = port_quality_info;

   if (port_status_info == IOLINK_PORT_STATUS_INFO_PREOP)
   {
      port_info->cycletime = real_paramlist->cycletime;
   }
}

static iolink_fsm_cm_event_t cm_start_ds (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   init_port_info (
      port,
      IOLINK_PORT_STATUS_INFO_PREOP,
      IOLINK_PORT_QUALITY_INFO_INVALID);

   DS_Startup (port);

   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_ds_ready (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   SM_Operate (port);

   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_ds_ready_ignore (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   /* Ignore DS_Ready when we are in state Port_Active_5, since this was
    * triggered by device DS_UPLOAD_REQ
    */
   LOG_INFO (IOLINK_CM_LOG, "CM: Ignore DS_Ready when in Port_Active_5\n");
   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_sm_operate (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   // TODO Update parameter elements of PortStatusList
   // Port QualityINfo = x
   init_port_info (port, IOLINK_PORT_STATUS_INFO_OP, IOLINK_PORT_QUALITY_INFO_VALID);

   OD_Start (port);
   PD_Start (port);
   // DU_Start (port); TODO

   /* SMI_PortEvent_ind */
   iolink_cm_do_smi_port_event (port, IOLINK_EVENTCODE_PORT_STATUS_CHANGE);

   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_port_fault (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   iolink_eventcode_t eventcode = IOLINK_EVENTCODE_NONE;

   switch (event)
   {
   case CM_EVENT_DS_Fault:
   {
      iolink_cm_port_t * cm = iolink_get_cm_ctx (port);

      if (cm->ds_fault == IOLINK_DS_FAULT_COM_ERR)
      {
         return CM_EVENT_SM_COMLOST;
      }
      else
         switch (cm->ds_fault)
         {
         case IOLINK_DS_FAULT_ID:
            eventcode = IOLINK_EVENTCODE_BACKUP_INCON_ID;
            break;
         case IOLINK_DS_FAULT_SIZE:
            eventcode = IOLINK_EVENTCODE_BACKUP_INCON_SIZE;
            break;
         case IOLINK_DS_FAULT_LOCK:
            eventcode = IOLINK_EVENTCODE_BACKUP_INCON;
            break;
         default:
            break;
         }
      // TODO DS storage failed. Rollback to previous parameter set
      break;
   }
   case CM_EVENT_SM_FAULT:
      break;
   default:
      CC_ASSERT (0);
   }

   init_port_info (
      port,
      IOLINK_PORT_STATUS_INFO_PORT_DIAG,
      IOLINK_PORT_QUALITY_INFO_INVALID);

   if (eventcode == IOLINK_EVENTCODE_NONE)
   {
      eventcode = IOLINK_EVENTCODE_BAD_DID; // TODO select correct code!
   }

   /* SMI_PortEvent_ind */
   iolink_cm_do_smi_port_event (port, eventcode);

   return CM_EVENT_NONE;
}

static iolink_fsm_cm_event_t cm_deactivate (
   iolink_port_t * port,
   iolink_fsm_cm_event_t event)
{
   iolink_cm_port_t * cm       = iolink_get_cm_ctx (port);
   portconfiglist_t * cfg_list = &cm->cfg_list;
   iolink_smp_parameterlist_t paraml;

   init_port_info (
      port,
      IOLINK_PORT_STATUS_INFO_DEACTIVATED,
      IOLINK_PORT_QUALITY_INFO_INVALID);

   memset (&paraml, 0, sizeof (iolink_smp_parameterlist_t));
   paraml.mode = portmode_to_target_mode (cfg_list->portmode);

   if (event == CM_EVENT_UNKNOWN)
   {
      /* According to spec. we should invoke DS_Delete() for both event Unknown
       * and Deactivated. However, TC0272, TC0273 and TC0275, expects the
       * previous DS to be available after deactivation.
       */
      DS_Delete (port);
   }
   SM_SetPortConfig_req (port, &paraml);

   return CM_EVENT_NONE;
}

/* CM FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 11.3.2 */
/* since we iterate through the list of events put the most likely in the top of
 * the list. */
static const iolink_fsm_cm_transition_t cm_trans_s0[] = {
   /* CheckPortMode_0 */
   {CM_EVENT_IOL_MANUAL, CM_STATE_SM_Startup, cm_startup},           /* T1 */
   {CM_EVENT_IOL_AUTOSTART, CM_STATE_SM_Startup, cm_startup},        /* T2 */
   {CM_EVENT_DI_CQ, CM_STATE_Port_DIDO, cm_dido},                    /* T9 */
   {CM_EVENT_DO_CQ, CM_STATE_Port_DIDO, cm_dido},                    /* T10 */
   {CM_EVENT_DEACTIVATED, CM_STATE_Port_Deactivated, cm_deactivate}, /* T11 */
   {CM_EVENT_UNKNOWN, CM_STATE_Port_Deactivated, cm_deactivate},     /* T16 */

   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* Not in FSM
                                                                  */
   {CM_EVENT_SM_COMLOST, CM_STATE_CheckPortMode, cm_comlost}, /* Not in FSM */
};
static const iolink_fsm_cm_transition_t cm_trans_s1[] = {
   /* SM_Startup_1 */
   {CM_EVENT_SM_COMREADY, CM_STATE_DS_ParamManager, cm_start_ds}, /* T3 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode},  /* T13 */
   {CM_EVENT_SM_FAULT, CM_STATE_PortFault, cm_port_fault},        /* T4 */
   {CM_EVENT_SM_COMLOST, CM_STATE_CheckPortMode, cm_comlost},     /* T8 */
};
static const iolink_fsm_cm_transition_t cm_trans_s2[] = {
   /* DS_ParamManager_2 */
   {CM_EVENT_DS_Ready, CM_STATE_WaitingOnOperate, cm_ds_ready}, /* T5 */
   {CM_EVENT_DS_CHANGE, CM_STATE_SM_Startup, cm_ds_change},     /* T14 */
   {CM_EVENT_DS_Fault, CM_STATE_PortFault, cm_port_fault},      /* T6 */

   {CM_EVENT_CFG_CHANGE, CM_STATE_DS_ParamManager, cm_cfg_change_ignore}, /* Not
                                                                             in
                                                                             spec
                                                                           */
   {CM_EVENT_SM_COMLOST, CM_STATE_DS_ParamManager, cm_comlost_ignore}, /* Not in
                                                                          spec
                                                                        */
};
static const iolink_fsm_cm_transition_t cm_trans_s3[] = {
   /* PortFault_3 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* T13 */
   {CM_EVENT_DS_CHANGE, CM_STATE_SM_Startup, cm_ds_change},      /* T14 */
   {CM_EVENT_SM_COMLOST, CM_STATE_CheckPortMode, cm_comlost},    /* T8 */
};
static const iolink_fsm_cm_transition_t cm_trans_s4[] = {
   /* WaitingOnOperate_4 */
   {CM_EVENT_SM_OPERATE, CM_STATE_Port_Active, cm_sm_operate},   /* T7 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* T13 */
   {CM_EVENT_DS_CHANGE, CM_STATE_SM_Startup, cm_ds_change},      /* T14 */
   {CM_EVENT_SM_COMLOST, CM_STATE_CheckPortMode, cm_comlost},    /* T8 */

   {CM_EVENT_DS_Fault, CM_STATE_PortFault, cm_port_fault}, /* Not in spec. */
};
static const iolink_fsm_cm_transition_t cm_trans_s5[] = {
   /* Port_Active_5 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* T13 */
   {CM_EVENT_DS_CHANGE, CM_STATE_SM_Startup, cm_ds_change},      /* T14 */
   {CM_EVENT_SM_COMLOST, CM_STATE_CheckPortMode, cm_comlost},    /* T8 */

   {CM_EVENT_DS_Ready, CM_STATE_Port_Active, cm_ds_ready_ignore}, /* Not in spec
                                                                   */
   {CM_EVENT_DS_Fault, CM_STATE_PortFault, cm_port_fault}, /* Not in spec. */
};
static const iolink_fsm_cm_transition_t cm_trans_s6[] = {
   /* Port_DIDO_6 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* T12 */
};
static const iolink_fsm_cm_transition_t cm_trans_s7[] = {
   /* Port_Deactivated_8 */
   {CM_EVENT_CFG_CHANGE, CM_STATE_CheckPortMode, cm_check_mode}, /* T15 */
};

/* The index is the state in this array */
static const iolink_fsm_cm_state_transitions_t iolink_cm_fsm[] = {
   {/* CheckPortMode_0 */
    .number_of_trans = NELEMENTS (cm_trans_s0),
    .transitions     = cm_trans_s0},
   {/* SM_Startup_1 */
    .number_of_trans = NELEMENTS (cm_trans_s1),
    .transitions     = cm_trans_s1},
   {/* DS_ParamManager_2 */
    .number_of_trans = NELEMENTS (cm_trans_s2),
    .transitions     = cm_trans_s2},
   {/* PortFault_3 */
    .number_of_trans = NELEMENTS (cm_trans_s3),
    .transitions     = cm_trans_s3},
   {/* WaitingOnOperate_4 */
    .number_of_trans = NELEMENTS (cm_trans_s4),
    .transitions     = cm_trans_s4},
   {/* Port_Active_5 */
    .number_of_trans = NELEMENTS (cm_trans_s5),
    .transitions     = cm_trans_s5},
   {/* Port_DIDO_6 */
    .number_of_trans = NELEMENTS (cm_trans_s6),
    .transitions     = cm_trans_s6},
   {/* Port_Deactivated_8 */
    .number_of_trans = NELEMENTS (cm_trans_s7),
    .transitions     = cm_trans_s7},
};

static void iolink_cm_event (iolink_port_t * port, iolink_fsm_cm_event_t event)
{
   do
   {
      int i;
      iolink_cm_port_t * cm                         = iolink_get_cm_ctx (port);
      iolink_cm_state_t previous                    = cm->state;
      const iolink_fsm_cm_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_cm_fsm[previous].number_of_trans; i++)
      {
         if (iolink_cm_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_cm_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         LOG_ERROR (
            IOLINK_CM_LOG,
            "CM (%u): next_trans == NULL: state %s - event %s\n",
            iolink_get_portnumber (port),
            iolink_cm_state_literals[previous],
            iolink_cm_event_literals[event]);
         return;
      }

      /* Transition to next state */
      cm->state = next_trans->next_state;
      LOG_DEBUG (
         IOLINK_CM_LOG,
         "CM (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_cm_event_literals[event],
         iolink_cm_state_literals[previous],
         iolink_cm_state_literals[cm->state]);
      event = next_trans->action (port, event);
   } while (event != CM_EVENT_NONE);
}

/* CM job callback functions */
static void cm_sm_port_mode_ind_cb (iolink_job_t * job)
{
   iolink_fsm_cm_event_t res_event = CM_EVENT_NONE;
   iolink_port_t * port            = job->port;

   switch (job->sm_port_mode_ind.mode)
   {
   case IOLINK_SM_PORTMODE_COMREADY:
      /* T3 */
      res_event = CM_EVENT_SM_COMREADY;
      break;
   case IOLINK_SM_PORTMODE_REVISION_FAULT:
   case IOLINK_SM_PORTMODE_COMP_FAULT:
   case IOLINK_SM_PORTMODE_SERNUM_FAULT:
   case IOLINK_SM_PORTMODE_CYCTIME_FAULT:
      /* T4 */
      res_event = CM_EVENT_SM_FAULT;
      break;
   case IOLINK_SM_PORTMODE_OPERATE:
      /* T7 */
      res_event = CM_EVENT_SM_OPERATE;
      break;
   case IOLINK_SM_PORTMODE_COMLOST:
      /* T8 */
      res_event = CM_EVENT_SM_COMLOST;
      break;
   case IOLINK_SM_PORTMODE_INACTIVE:
   {
      iolink_port_info_t * port_info = iolink_get_port_info (port);

      if (port_info->port_status_info != IOLINK_PORT_STATUS_INFO_DEACTIVATED)
      {
         port_info->port_status_info = IOLINK_PORT_STATUS_INFO_DEACTIVATED;
         /* SMI_PortEvent_ind */
         iolink_cm_do_smi_port_event (port, IOLINK_EVENTCODE_PORT_STATUS_CHANGE);
      }
   }
   break;
   case IOLINK_SM_PORTMODE_DI:
   case IOLINK_SM_PORTMODE_DO:
      break;
   }

   if (res_event != CM_EVENT_NONE)
   {
      iolink_cm_event (port, res_event);
   }
}

static void cm_ds_ready_cb (iolink_job_t * job)
{
   iolink_cm_event (job->port, CM_EVENT_DS_Ready);
}

static void cm_ds_change_cb (iolink_job_t * job)
{
   iolink_cm_event (job->port, CM_EVENT_DS_CHANGE);
}

static void cm_ds_fault_cb (iolink_job_t * job)
{
   iolink_cm_port_t * cm = iolink_get_cm_ctx (job->port);

   cm->ds_fault = job->ds_fault.fault;
   iolink_cm_event (job->port, CM_EVENT_DS_Fault);
}

static bool check_arg_block (
   iolink_job_t * job,
   iolink_arg_block_id_t cmp_ref_arg_block_id,
   uint16_t arg_block_len,
   iolink_arg_block_id_t cmp_exp_arg_block_id,
   iolink_smi_errortypes_t errortype)
{
   iolink_port_t * port                   = job->port;
   iolink_smi_service_req_t * job_smi_req = &job->smi_req;
   iolink_arg_block_id_t exp_arg_block_id = job_smi_req->exp_arg_block_id;
   iolink_arg_block_id_t ref_arg_block_id =
      job_smi_req->arg_block->id;

   if (ref_arg_block_id != cmp_ref_arg_block_id)
   {
      iolink_smi_joberror_ind (
         port,
         exp_arg_block_id,
         ref_arg_block_id,
         IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED);

      return false;
   }
   else if (job_smi_req->arg_block_len != arg_block_len)
   {
      iolink_smi_joberror_ind (
         port,
         exp_arg_block_id,
         ref_arg_block_id,
         IOLINK_SMI_ERRORTYPE_ARGBLOCK_LENGTH_INVALID);

      return false;
   }
   else if (exp_arg_block_id != cmp_exp_arg_block_id)
   {
      if (errortype != IOLINK_SMI_ERRORTYPE_NONE)
      {
         iolink_smi_joberror_ind (
            port,
            exp_arg_block_id,
            ref_arg_block_id,
            errortype);
      }
      else
      {
         // TODO what to return?
      }

      return false;
   }

   return true;
}

static void cm_smi_masterident_cb (iolink_job_t * job)
{
   iolink_port_t * port                   = job->port;
   iolink_smi_service_req_t * job_smi_req = &job->smi_req;
   iolink_arg_block_id_t ref_arg_block_id =
      job_smi_req->arg_block->id;

   if (check_arg_block (
          job,
          IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
          sizeof (arg_block_void_t),
          IOLINK_ARG_BLOCK_ID_MASTERIDENT,
          IOLINK_SMI_ERRORTYPE_NONE))
   {
      arg_block_masterident_t master_ident;
      int i;
      uint8_t max_number_of_ports = iolink_get_port_cnt (port);
      uint8_t arg_block_len       = sizeof (arg_block_masterident_head_t) +
                              sizeof (iolink_port_types_t) * max_number_of_ports;

      memset (&master_ident, 0, sizeof (arg_block_masterident_t));

      master_ident.h.arg_block.id        = IOLINK_ARG_BLOCK_ID_MASTERIDENT;
      master_ident.h.vendorid            = 1171; // TODO
      master_ident.h.masterid            = 123;  // TODO
      master_ident.h.master_type         = IOLINK_MASTER_TYPE_MASTER_ACC;
      master_ident.h.features_1          = 0;
      master_ident.h.features_2          = 0;
      master_ident.h.max_number_of_ports = max_number_of_ports;

      for (i = 0; i < max_number_of_ports; i++)
      {
         master_ident.port_types[i] = IOLINK_PORT_TYPES_A; // TODO correct?
      }

      iolink_smi_cnf (
         port,
         ref_arg_block_id,
         arg_block_len,
         (arg_block_t *)&master_ident);
   }
}

static void cm_smi_portconfiguration_cb (iolink_job_t * job)
{
   iolink_port_t * port                   = job->port;
   iolink_smi_service_req_t * job_smi_req = &job->smi_req;
   iolink_arg_block_id_t exp_arg_block_id = job_smi_req->exp_arg_block_id;

   if (check_arg_block (
          job,
          IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST,
          sizeof (arg_block_portconfiglist_t),
          IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
          IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT))
   {
      iolink_cm_port_t * cm              = iolink_get_cm_ctx (port);
      iolink_smi_service_req_t * smi_req = &cm->smi_req;

      smi_req->result           = IOLINK_SMI_ERRORTYPE_NONE;
      smi_req->exp_arg_block_id = exp_arg_block_id;
      smi_req->arg_block_len    = job_smi_req->arg_block_len;
      smi_req->arg_block        = job_smi_req->arg_block;

      iolink_cm_event (port, CM_EVENT_CFG_CHANGE);
   }
}

static void cm_smi_readbackportconfiguration_cb (iolink_job_t * job)
{
   iolink_port_t * port                   = job->port;
   iolink_smi_service_req_t * job_smi_req = &job->smi_req;
   iolink_arg_block_id_t ref_arg_block_id =
      job_smi_req->arg_block->id;

   if (check_arg_block (
          job,
          IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
          sizeof (arg_block_void_t),
          IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST,
          IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT))
   {
      iolink_cm_port_t * cm = iolink_get_cm_ctx (port);
      arg_block_portconfiglist_t arg_block_portcfg;

      memset (&arg_block_portcfg, 0, sizeof (arg_block_portconfiglist_t));
      memcpy (
         &arg_block_portcfg.configlist,
         &cm->cfg_list,
         sizeof (portconfiglist_t));
      arg_block_portcfg.arg_block.id = IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST;

      iolink_smi_cnf (
         port,
         ref_arg_block_id,
         sizeof (arg_block_portconfiglist_t),
         (arg_block_t *)&arg_block_portcfg);
   }
}

static void cm_smi_portstatus_cb (iolink_job_t * job)
{
   iolink_port_t * port                   = job->port;
   iolink_smi_service_req_t * job_smi_req = &job->smi_req;
   iolink_arg_block_id_t ref_arg_block_id =
      job_smi_req->arg_block->id;

   if (check_arg_block (
          job,
          IOLINK_ARG_BLOCK_ID_VOID_BLOCK,
          sizeof (arg_block_void_t),
          IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST,
          IOLINK_SMI_ERRORTYPE_NONE))
   {
      arg_block_portstatuslist_t port_status;
      iolink_port_info_t * port_info = iolink_get_port_info (port);

      memset (&port_status, 0, sizeof (arg_block_portstatuslist_t));
      port_status.arg_block.id      = IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST;
      port_status.port_status_info  = port_info->port_status_info;
      port_status.transmission_rate = port_info->transmission_rate;
      port_status.vendorid          = port_info->vendorid;
      port_status.deviceid          = port_info->deviceid;
      port_status.revision_id       = port_info->revisionid;

      // TODO how to handle master_cycle_time?!
      if (
         port_status.port_status_info == IOLINK_PORT_STATUS_INFO_NO_DEV ||
         port_status.port_status_info == IOLINK_PORT_STATUS_INFO_DEACTIVATED ||
         port_status.port_status_info == IOLINK_PORT_STATUS_INFO_DI ||
         port_status.port_status_info == IOLINK_PORT_STATUS_INFO_DO)
         port_status.master_cycle_time = 0;
      else
         port_status.master_cycle_time = port_info->cycletime;

      port_status.port_quality_info = port_info->port_quality_info;

      // TODO for each diag entry .. number_of_diags++ and append to
      // diag_entries
      port_status.number_of_diags = 0;

      iolink_smi_cnf (
         port,
         ref_arg_block_id,
         sizeof (arg_block_portstatuslist_t),
         (arg_block_t *)&port_status);
   }
}

/* Stack internal API */
void iolink_cm_init (iolink_port_t * port)
{
   iolink_cm_port_t * cm = iolink_get_cm_ctx (port);

   init_port_info (
      port,
      IOLINK_PORT_STATUS_INFO_DEACTIVATED,
      IOLINK_PORT_QUALITY_INFO_INVALID);

   cm->ds_fault = IOLINK_DS_FAULT_NONE;

   cm->state = CM_STATE_Port_Deactivated;
}

void SM_PortMode_ind (iolink_port_t * port, iolink_sm_portmode_t mode)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   job->sm_port_mode_ind.mode = mode;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_SM_PORT_MODE_IND,
      cm_sm_port_mode_ind_cb);
}

void DS_Ready (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_READY,
      cm_ds_ready_cb);
}

void DS_Change (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_CHANGE,
      cm_ds_change_cb);
}

void DS_Fault (iolink_port_t * port, iolink_ds_fault_t fault)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   job->ds_fault.fault = fault;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_FAULT,
      cm_ds_fault_cb);
}

static iolink_error_t init_and_post_job (
   iolink_port_t * port,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job),
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_job_t * job            = iolink_fetch_avail_api_job (port);
   job->smi_req.exp_arg_block_id = exp_arg_block_id;
   job->smi_req.arg_block_len    = arg_block_len;
   job->smi_req.arg_block        = arg_block;

   iolink_post_job_with_type_and_callback (port, job, type, callback);

   return IOLINK_ERROR_NONE;
}

iolink_error_t cm_SMI_MasterIdentification_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return init_and_post_job (
      port,
      IOLINK_JOB_SMI_MASTERIDENT,
      cm_smi_masterident_cb,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}

iolink_error_t cm_SMI_PortConfiguration_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return init_and_post_job (
      port,
      IOLINK_JOB_SMI_PORTCONFIGURATION,
      cm_smi_portconfiguration_cb,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}

iolink_error_t cm_SMI_ReadbackPortConfiguration_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return init_and_post_job (
      port,
      IOLINK_JOB_SMI_READBACKPORTCONFIGURATION,
      cm_smi_readbackportconfiguration_cb,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}

iolink_error_t cm_SMI_PortStatus_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return init_and_post_job (
      port,
      IOLINK_JOB_SMI_PORTSTATUS,
      cm_smi_portstatus_cb,
      exp_arg_block_id,
      arg_block_len,
      arg_block);
}
