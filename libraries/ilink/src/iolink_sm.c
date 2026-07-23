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
#define DL_Write_req            mock_DL_Write_req
#define DL_Read_req             mock_DL_Read_req
#define DL_SetMode_req          mock_DL_SetMode_req
#define DL_Write_Devicemode_req mock_DL_Write_Devicemode_req
#define PL_SetMode_req          mock_PL_SetMode_req
#define AL_Read_req             mock_AL_Read_req
#define AL_Write_req            mock_AL_Write_req
#define SM_PortMode_ind         mock_SM_PortMode_ind
#define iolink_post_job         mock_iolink_post_job
#define iolink_fetch_avail_job  mock_iolink_fetch_avail_job
#endif /* UNIT_TEST */

#include "iolink_sm.h"
#include "iolink_dl.h" /* DL_Read_req DL_Write_req DL_SetMode_req */
#include "iolink_al.h" /* AL_Read_req AL_Write_req */
#include "iolink_pl.h" /* PL_SetMode_req */
#include "iolink_cm.h" /* SM_PortMode_ind */
#include "iolink_ds.h"
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_post_job, iolink_get_portnumber */
#include "osal.h"
#include "osal_log.h"

#include <string.h> /* memset */

/**
 * @file
 * @brief System Management layer
 *
 */

#define IOL_SM_VENDOR_MESCO_ENGINEERING 0x014E

typedef struct iolink_fsm_sm_transition
{
   iolink_fsm_sm_event_t event;
   iolink_sm_state_t next_state;
   iolink_fsm_sm_event_t (
      *action) (iolink_port_t * port, iolink_fsm_sm_event_t event);
} iolink_fsm_sm_transition_t;

typedef struct iolink_fsm_sm_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_sm_transition_t * transitions;
} iolink_fsm_sm_state_transitions_t;

static const char * const iolink_fsm_sm_event_literals[] = {
   "NONE",
   "DL_Mode_STARTUP",                    /* T1 */
   "CompOK",                             /* T2 */
   "DL_Mode_COMLOST",                    /* T3 */
   "V10CompOK",                          /* T4 */
   "V10CompFault",                       /* T5 */
   "RevisionFault",                      /* T6 */
   "CompFault",                          /* T7 */
   "DL_Mode_PREOPERATE",                 /* T8 */
   "DL_Mode_OPERATE",                    /* T9 and T13 */
   "SerNumOK",                           /* T10 */
   "SerNumFault",                        /* T11 */
   "SM_Operate",                         /* T12 */
   "SM_Operate_v10",                     /* T12 for v1.0 device*/
   "SM_SetPortConfig_INACTIVE",          /* T14 */
   "SM_SetPortConfig_CFGCOM_or_AUTOCOM", /* T15 */
   "SM_SetPortConfig_DI_or_DO",          /* T16 */
   "CycTimeFault",                       /* T17 */
   "CycTimeFault_V10",                   /* T18 */
   "V10",                                /* T20 */
   "NOT_V10",                            /* T21 */
   "REVISION_OK",                        /* T22 */
   "RETRY_STARTUP",                      /* T23 and T25 */
   "WriteDone",                          /* T24 */
   "CNF_COMLOST", /* T3, COMLOST when waiting on DL_{Read,Write}_cnf(),
                     AL_{Read,Write}_cnf() or DL_Write_Devicemode_cnf()*/
   "WRITE_MASTER_CYCL_REQ",  /* Not in spec */
   "WRITE_MASTER_CYCL_DONE", /* Not in spec */
};

static const char * const iolink_sm_state_literals[] = {
   "PortInactive",
   "waitonDLPreoperate",
   "checkSerNum",
   "wait",
   "SMOperate",
   "InspectionFault",
   "waitonDLOperate",
   "DIDO",
   "ReadComParameter",
   "CheckCompV10",
   "CheckVxy",
   "CheckComp",
   "RestartDevice",
   "waitForFallback",
   "write_master_cycl",
   "wait_devmode_cnf",
};

static const char * const iolink_mhmode_literals[] = {
   "INACTIVE",
   "COM1",
   "COM2",
   "COM3",
   "COMLOST",
   "ESTABCOM",
   "STARTUP",
   "PREOPERATE",
   "OPERATE",
};

static const char * const iolink_sm_target_mode_literals[] = {
   "CFGCOM",
   "AUTOCOM",
   "INACTIVE",
   "DI",
   "DO",
};

static const char * const iolink_error_literals[] = {
   "NONE",
   "PDINLENGTH",
   "PDOUTLENGTH",
   "ODLENGTH",
   "ADDRESS_INVALID",
   "COMCHANNEL_INVALID",
   "MODE_INVALID",
   "STATUS_INVALID",
   "CONTROLCODE_INVALID",
   "CONDITIONS_NOT_CORRECT",
   "INCORRECT_DATA",
   "STATE_INVALID",
   "OUT_OF_MEMORY",
   "PARAMETER_CONFLICT",
   "STATE_CONFLICT",
};

/**
 * Trigger SM FSM state transition
 *
 * This function triggers an SM state transition according to the event.
 */
static void iolink_sm_event (iolink_port_t * port, iolink_fsm_sm_event_t event);

/* Actions taken when transitioning to a new state. See iolink_sm_event(). */
static iolink_fsm_sm_event_t sm_wr_master_cycl (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_checkCompV10_21 (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_readcomparameter (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_restartDevice (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_checkComp (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_checkVxy (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_write_mcmd_preop (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_fallback (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_portinactive (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_comlost (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_smoperate_ignore (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_waitonDLOp (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_inspectFault (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_checkSerNum (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_wait (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_write_devmode (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_smoperate_ind (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_dido (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);
static iolink_fsm_sm_event_t sm_comlost_ignore (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event);

/* Callback functions to run on main thread */
static void sm_write_cycl_cnf_cb (iolink_job_t * job);
static void sm_operate_cb (iolink_job_t * job);
static void sm_setportcfg_req_cb (iolink_job_t * job);
static void sm_AL_Read_cnf_cb (iolink_job_t * job);
static void sm_DL_Mode_ind_cb (iolink_job_t * job);
static void sm_DL_Read_cnf_cb (iolink_job_t * job);
static void sm_DL_Write_cnf_cb (iolink_job_t * job);
static void sm_DL_Write_Devmode_cnf (iolink_job_t * job);

/* Other functions */
static void sm_DL_Mode_ind_cb_preop_op (
   iolink_job_t * job,
   iolink_mhmode_t mode,
   iolink_fsm_sm_event_t event);
static void sm_DL_Read_cnf_check_comp (
   iolink_job_t * job,
   iolink_sm_state_t state);
static void sm_DL_Read_cnf_read_com_param (iolink_job_t * job);
static iolink_fsm_sm_event_t sm_DL_Read_req (iolink_port_t * port, uint8_t addr);
static void sm_DL_Write_cnf_checkvxy (iolink_job_t * job);
static void sm_DL_Write_cnf_restart_dev (iolink_job_t * job);
static void sm_DL_Write_cnf_waiton_operate (iolink_job_t * job);
static iolink_fsm_sm_event_t sm_DL_Read_Write_req (
   iolink_port_t * port,
   uint8_t addr,
   uint8_t value,
   bool read);
static iolink_fsm_sm_event_t sm_DL_Write_req (
   iolink_port_t * port,
   uint8_t addr,
   uint8_t value);
static void sm_DL_Write_Devicemode_req (
   iolink_port_t * port,
   iolink_dl_mode_t devicemode);
static iolink_error_t sm_DL_SetMode_req_startup (iolink_port_t * port);
static iolink_error_t sm_DL_SetMode_req_inactive (iolink_port_t * port);
static iolink_error_t sm_DL_SetMode_req_preop_op (
   iolink_port_t * port,
   iolink_dl_mode_t mode);
static iolink_error_t sm_DL_SetMode_req_preop (iolink_port_t * port);
static iolink_error_t sm_DL_SetMode_req_operate (iolink_port_t * port);
static void sm_AL_Read_cnf (
   iolink_port_t * port,
   uint8_t len,
   const uint8_t * data,
   iolink_smi_errortypes_t errortype);
static void sm_write_master_cycl_cnf (
   iolink_port_t * port,
   iolink_smi_errortypes_t errortype);
static void sm_do_check_comp_v10 (iolink_port_t * port);
static void sm_do_check_comp (iolink_port_t * port);
static void sm_check_sernum (iolink_port_t * port, bool SReadOk);
static void sm_store_parameter_retrieved_from_device (
   iolink_port_t * port, uint8_t addr, uint8_t value);
static void sm_set_cycletime (iolink_port_t * port, uint8_t value);
static void set_type_2_V (
   bool pdin_isbytes,
   bool pdout_isbytes,
   uint8_t pdin,
   uint8_t pdout,
   uint8_t onreqdatalengthpermessage,
   uint8_t cycbyte,
   iolink_mode_vl_t * valuelist);
static void set_valuelist (
   iolink_mode_vl_t * valuelist,
   uint8_t onreqdatalengthpermessage,
   uint8_t pdinputlength,
   uint8_t pdoutputlength,
   uint8_t time,
   iolink_msequencetype_t type);
static iolink_error_t populate_mode_valuelist (
   const uint8_t mscapa,
   const uint8_t revid,
   const uint8_t pdin,
   const uint8_t pdout,
   const uint8_t cycbyte,
   const bool operate,
   iolink_mode_vl_t * const valuelist);
static void populate_preop_mode_valuelist (
   const uint8_t mscapa,
   const uint8_t cycbyte,
   iolink_mode_vl_t * const valuelist);
static void populate_legacy_operate_mode_valuelist (
   const uint8_t mscapa,
   uint8_t pdin,
   uint8_t pdout,
   const uint8_t cycbyte,
   iolink_mode_vl_t * const valuelist);
static void populate_nonlegacy_operate_mode_valuelist (
   const uint8_t mscapa,
   uint8_t pdin,
   uint8_t pdout,
   const uint8_t cycbyte,
   iolink_mode_vl_t * const valuelist);
static void populate_nonlegacy_operate_mode_valuelist_for_code_0 (
   const bool pdin_isbytes,
   const bool pdout_isbytes,
   const uint8_t pdin,
   const uint8_t pdout,
   const uint8_t cycbyte,
   iolink_mode_vl_t * const valuelist);
static iolink_error_t populate_valuelist (
   uint8_t mscapa,
   uint8_t revid,
   uint8_t pdin,
   uint8_t pdout,
   uint8_t cycbyte,
   bool operate,
   iolink_mode_vl_t * valuelist);

static iolink_fsm_sm_event_t sm_DL_Read_Write_req (
   iolink_port_t * port,
   uint8_t addr,
   uint8_t value,
   bool read)
{
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);
   iolink_error_t res;

   sm->dl_addr = addr;

   if (read)
   {
      CC_ASSERT (
         (addr >= IOL_DIR_PARAMA_MIN_CYCL) && (addr <= IOL_DIR_PARAMA_FID_2));

      res = DL_Read_req (port, addr);
   }
   else
   {
      CC_ASSERT (addr <= IOL_DIR_PARAMA_DID_3);

      res = DL_Write_req (port, addr, value);
   }

   if (res != IOLINK_ERROR_NONE)
   {
      LOG_WARNING (
         IOLINK_SM_LOG,
         "SM (%u): DL_%s_req failed: %s\n",
         iolink_get_portnumber (port),
         (read) ? "Read" : "Write",
         iolink_error_literals[res]);
   }

   return SM_EVENT_NONE; /* Always wait for DL_Read_cnf()/DL_Write_cnf() */
}

static iolink_fsm_sm_event_t sm_DL_Read_req (iolink_port_t * port, uint8_t addr)
{
   return sm_DL_Read_Write_req (port, addr, 0, true);
}

static iolink_fsm_sm_event_t sm_DL_Write_req (
   iolink_port_t * port,
   uint8_t addr,
   uint8_t value)
{
   return sm_DL_Read_Write_req (port, addr, value, false);
}

static void sm_DL_Write_Devicemode_req (
   iolink_port_t * port,
   iolink_dl_mode_t devicemode)
{
   iolink_error_t res = DL_Write_Devicemode_req (port, devicemode);

   if (res != IOLINK_ERROR_NONE)
   {
      LOG_WARNING (
         IOLINK_SM_LOG,
         "SM (%u): DL_Write_Devicemode_req failed: %s\n",
         iolink_get_portnumber (port),
         iolink_error_literals[res]);
   }
}

static void sm_write_master_cycl_cnf (
   iolink_port_t * port,
   iolink_smi_errortypes_t errortype)
{
   iolink_job_t * job          = iolink_fetch_avail_job (port);
   job->al_write_cnf.errortype = errortype;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_WRITE_CNF,
      sm_write_cycl_cnf_cb);
}

static iolink_fsm_sm_event_t sm_wr_master_cycl (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   /* Not in spec */
   iolink_error_t res = AL_Write_req (
      port,
      IOL_DIR_PARAMA_MASTER_CYCL,
      0,
      sizeof (real_paramlist->cycletime),
      &real_paramlist->cycletime,
      sm_write_master_cycl_cnf);
   if (res != IOLINK_ERROR_NONE)
   {
      LOG_WARNING (
         IOLINK_SM_LOG,
         "SM (%u): AL_Write_req failed: %s\n",
         iolink_get_portnumber (port),
         iolink_error_literals[res]);
   }

   return SM_EVENT_NONE; /* Always wait for AL_Write_cnf() */
}

static iolink_fsm_sm_event_t sm_checkCompV10_21 (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   /* Read 0x07 to 0x0D */
   return sm_DL_Read_req (port, IOL_DIR_PARAMA_VID_1);
}

static void sm_do_check_comp_v10 (iolink_port_t * port)
{
   iolink_fsm_sm_event_t ret;
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;
   iolink_smp_parameterlist_t * real_paramlist   = &sm->real_paramlist;

   if (
      config_paramlist->cycletime &&
      (config_paramlist->cycletime < real_paramlist->cycletime))
   {
      ret             = SM_EVENT_CycTimeFault_V10; /* T18 */
      sm->error_event = ret;
   }
   else if (config_paramlist->inspectionlevel == IOLINK_INSPECTIONLEVEL_NO_CHECK)
   {
      ret = SM_EVENT_V10CompOK; /* T4 */
   }
   else /* IOLINK_INSPECTIONLEVEL_TYPE_COMP || IOLINK_INSPECTIONLEVEL_IDENTICAL
         */
   {
      if (
         (config_paramlist->vendorid == real_paramlist->vendorid) &&
         (config_paramlist->deviceid == real_paramlist->deviceid))
      {
         ret = SM_EVENT_V10CompOK; /* T4 */
      }
      else
      {
         ret             = SM_EVENT_V10CompFault; /* T5 */
         sm->error_event = ret;
      }
   }

   iolink_sm_event (port, ret);
}

static void set_valuelist (
   iolink_mode_vl_t * valuelist,
   uint8_t onreqdatalengthpermessage,
   uint8_t pdinputlength,
   uint8_t pdoutputlength,
   uint8_t time,
   iolink_msequencetype_t type)
{
   valuelist->onreqdatalengthpermessage = onreqdatalengthpermessage;
   valuelist->pdinputlength             = pdinputlength;
   valuelist->pdoutputlength            = pdoutputlength;
   valuelist->time                      = time;
   valuelist->type                      = type;
}

static iolink_error_t sm_DL_SetMode_req_startup_inactive (
   iolink_port_t * port,
   iolink_dl_mode_t mode)
{
   iolink_mode_vl_t valuelist;

   if (mode == IOLINK_DLMODE_STARTUP)
   {
      set_valuelist (&valuelist, 1, 0, 0, 0, IOLINK_MSEQTYPE_TYPE_0);
   }
   else if (mode == IOLINK_DLMODE_INACTIVE)
   {
      set_valuelist (&valuelist, 0, 0, 0, 0, IOLINK_MSEQTYPE_TYPE_NONE);
   }

   return DL_SetMode_req (port, mode, &valuelist);
}

static iolink_error_t sm_DL_SetMode_req_startup (iolink_port_t * port)
{
   return sm_DL_SetMode_req_startup_inactive (port, IOLINK_DLMODE_STARTUP);
}

static iolink_error_t sm_DL_SetMode_req_inactive (iolink_port_t * port)
{
   return sm_DL_SetMode_req_startup_inactive (port, IOLINK_DLMODE_INACTIVE);
}

static iolink_error_t sm_DL_SetMode_req_preop_op (
   iolink_port_t * port,
   iolink_dl_mode_t mode)
{
   iolink_mode_vl_t valuelist;
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   set_valuelist (&valuelist, 1, 0, 0, 0, IOLINK_MSEQTYPE_TYPE_0);

   populate_valuelist (
      sm->dev_com.mseq_cap,
      real_paramlist->revisionid,
      sm->dev_com.pdi,
      sm->dev_com.pdo,
      real_paramlist->cycletime,
      (mode == IOLINK_DLMODE_OPERATE),
      &valuelist);

   if (valuelist.type == IOLINK_MSEQTYPE_TYPE_1_1)
   {
      iolink_ds_port_t * ds = iolink_get_ds_ctx (port);
      // SDCI_TC_0288
      ds->ds_enable = false;
   }
   else if (valuelist.type == IOLINK_MSEQTYPE_TYPE_NONE)
   {
      LOG_ERROR (
         IOLINK_SM_LOG,
         "SM (%u): unknown msg type: pdin: %d, pdout: %d\n, revid: %d, mscapa: %d",
         iolink_get_portnumber (port),
         sm->dev_com.pdi,
         sm->dev_com.pdo,
         real_paramlist->revisionid,
         sm->dev_com.mseq_cap);
   }

   return DL_SetMode_req (port, mode, &valuelist);
}

static iolink_error_t sm_DL_SetMode_req_preop (iolink_port_t * port)
{
   return sm_DL_SetMode_req_preop_op (port, IOLINK_DLMODE_PREOPERATE);
}

static iolink_error_t sm_DL_SetMode_req_operate (iolink_port_t * port)
{
   return sm_DL_SetMode_req_preop_op (port, IOLINK_DLMODE_OPERATE);
}

static iolink_fsm_sm_event_t sm_readcomparameter (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   if (event == SM_EVENT_DL_Mode_STARTUP)
   {
      iolink_sm_port_t * sm = iolink_get_sm_ctx (port);
      sm->CompRetry         = 0;
   }

   /* Read 0x02 to 0x06 */
   return sm_DL_Read_req (port, IOL_DIR_PARAMA_MIN_CYCL);
}

static iolink_fsm_sm_event_t sm_restartDevice (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;
   // TODO: is this correct?
   // According to Table 84, page 133, increase compretry on T25, not T23??
   // If not, add SM_EVENT_RETRY_STARTUP_23 and SM_EVENT_RETRY_STARTUP_25
   sm->CompRetry++;

   return sm_DL_Write_req (
      port,
      IOL_DIR_PARAMA_REV_ID,
      config_paramlist->revisionid);
}

static iolink_fsm_sm_event_t sm_checkComp (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   /* Read 0x07 to 0x0D */
   return sm_DL_Read_req (port, IOL_DIR_PARAMA_VID_1);
}

static void sm_do_check_comp (iolink_port_t * port)
{
   iolink_fsm_sm_event_t ret;
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;
   iolink_smp_parameterlist_t * real_paramlist   = &sm->real_paramlist;

   if (
      config_paramlist->cycletime &&
      (config_paramlist->cycletime < real_paramlist->cycletime))
   {
      ret             = SM_EVENT_CycTimeFault; /* T17 */
      sm->error_event = ret;
   }
   else if (config_paramlist->inspectionlevel == IOLINK_INSPECTIONLEVEL_NO_CHECK)
   {
      ret = SM_EVENT_CompOK; /* T2 */
   }
   else /* IOLINK_INSPECTIONLEVEL_TYPE_COMP || IOLINK_INSPECTIONLEVEL_IDENTICAL
         */
   {
      if (config_paramlist->vendorid != real_paramlist->vendorid)
      {
         ret             = SM_EVENT_CompFault; /* T7 */
         sm->error_event = ret;
      }
      else if (config_paramlist->deviceid == real_paramlist->deviceid)
      {
         ret = SM_EVENT_CompOK; /* T2 */
      }
      else
      {
         if (sm->CompRetry == 0)
         {
            ret = SM_EVENT_RETRY_STARTUP; /* T23 */
         }
         else
         {
            ret             = SM_EVENT_CompFault; /* T7 */
            sm->error_event = ret;
         }
      }
   }

   iolink_sm_event (port, ret);
}

static iolink_fsm_sm_event_t sm_checkVxy (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   /* T21 */
   return sm_DL_Write_req (
      port,
      IOL_DIR_PARAMA_MASTER_CMD,
      IOL_MASTERCMD_MASTER_IDENT);
}

static iolink_fsm_sm_event_t sm_write_mcmd_preop (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   return sm_DL_Write_req (
      port,
      IOL_DIR_PARAMA_MASTER_CMD,
      IOL_MASTERCMD_DEVICE_PREOP);
}

static iolink_fsm_sm_event_t sm_fallback (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   sm_DL_Write_Devicemode_req (port, IOLINK_DLMODE_INACTIVE);

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_portinactive (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);
   /* We get the first DL_Read_cnf(), after WURQ, without any DL_Read_req() */
   sm->dl_addr     = IOL_DIR_PARAMA_DUMMY_WURQ;
   sm->error_event = SM_EVENT_NONE;

   switch (event)
   {
   case SM_EVENT_SM_SetPortConfig_INACTIVE: /* T14 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_INACTIVE);
      sm_DL_SetMode_req_inactive (port);
      break;
   case SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM: /* T15 */
      PL_SetMode_req (port, iolink_mode_SDCI);
      sm_DL_SetMode_req_startup (port);
      break;
   case SM_EVENT_SM_SetPortConfig_DI_or_DO: /* T16 */
      break;
   default:
      break;
   }

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_comlost (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   sm_DL_SetMode_req_inactive (port);
   SM_PortMode_ind (port, IOLINK_SM_PORTMODE_COMLOST);

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_smoperate_ignore (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   /* It is possible to receive SM_Operate after a COMLOST, when DS is disabled.
    * Hence, we ignore this SM_Operate.
    */
   LOG_DEBUG (
      IOLINK_SM_LOG,
      "SM (%u): ignore SM_Operate for state PortInactive\n",
      iolink_get_portnumber (port));

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_comlost_ignore (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   /* If we receive COMLOST when we are waiting for DL_{Read,Write}_cnf() or
    * AL_{Read,Write}_cnf() or DL_Write_Devicemode_cnf(), we will first receive
    * COMLOST via DL_Mode_ind().
    * Hence, we ignore this COMLOST.
    */
   LOG_DEBUG (
      IOLINK_SM_LOG,
      "SM (%u): ignore COMLOST wait for AL_{Read,Write}_cnf() or "
      "DL_Write_Devicemode_cnf()\n",
      iolink_get_portnumber (port));

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_waitonDLOp (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   return sm_DL_Write_req (
      port,
      IOL_DIR_PARAMA_MASTER_CYCL,
      real_paramlist->cycletime);
}

static iolink_fsm_sm_event_t sm_inspectFault (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   switch (event)
   {
   case SM_EVENT_V10CompFault: /* T5 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_COMP_FAULT);
      sm_DL_SetMode_req_operate (port);
      break;
   case SM_EVENT_RevisionFault: /* T6 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_REVISION_FAULT);
      sm_DL_SetMode_req_preop (port);
      break;
   case SM_EVENT_CompFault: /* T7 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_COMP_FAULT);
      sm_DL_SetMode_req_preop (port);
      break;
   case SM_EVENT_SerNumFault: /* T11 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_SERNUM_FAULT);
      break;
   case SM_EVENT_CycTimeFault: /* T17 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_CYCTIME_FAULT);
      sm_DL_SetMode_req_preop (port);
      break;
   case SM_EVENT_CycTimeFault_V10: /* T18 */
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_CYCTIME_FAULT);
      sm_DL_SetMode_req_inactive (port);
      break;
   default:
      break;
   }

   return SM_EVENT_NONE;
}

static void sm_AL_Read_cnf (
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
      sm_AL_Read_cnf_cb);
}

static iolink_fsm_sm_event_t sm_checkSerNum (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   AL_Read_req (port, IOL_DEV_PARAMA_SERIAL_NUMBER, 0, sm_AL_Read_cnf);

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_wait (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   SM_PortMode_ind (port, IOLINK_SM_PORTMODE_COMREADY);

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_write_devmode (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   sm_DL_Write_Devicemode_req (port, IOLINK_DLMODE_OPERATE);

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_smoperate_ind (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   if (
      (event == SM_EVENT_DL_Mode_OPERATE) || /* T13 */
      (event == SM_EVENT_SM_Operate_v10))    /* v1.0 is already in operate */
   {
      SM_PortMode_ind (port, IOLINK_SM_PORTMODE_OPERATE);
   }

   return SM_EVENT_NONE;
}

static iolink_fsm_sm_event_t sm_dido (
   iolink_port_t * port,
   iolink_fsm_sm_event_t event)
{
   iolink_sm_port_t * sm                      = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * parameterlist = &sm->config_paramlist;
   iolink_pl_mode_t target_pl_mode            = iolink_mode_INACTIVE;
   iolink_sm_portmode_t target_sm_mode        = IOLINK_SM_PORTMODE_INACTIVE;

   if (parameterlist->mode == IOLINK_SMTARGET_MODE_DI)
   {
      target_pl_mode = iolink_mode_DI;
      target_sm_mode = IOLINK_SM_PORTMODE_DI;
   }
   else if (parameterlist->mode == IOLINK_SMTARGET_MODE_DO)
   {
      target_pl_mode = iolink_mode_DO;
      target_sm_mode = IOLINK_SM_PORTMODE_DO;
   }
   else
   {
      LOG_ERROR (
         IOLINK_SM_LOG,
         "SM (%u): dido, invalid iolink_sm_target_mode_t (%u)\n",
         iolink_get_portnumber (port),
         parameterlist->mode);
   }

   PL_SetMode_req (port, target_pl_mode);
   SM_PortMode_ind (port, target_sm_mode);
   sm_DL_SetMode_req_inactive (port);

   return SM_EVENT_NONE;
}

static void sm_DL_Read_cnf_read_com_param (iolink_job_t * job)
{
   iolink_port_t * port = job->port;
   uint8_t addr         = job->dl_rw_cnf.addr;

   if (addr != IOL_DIR_PARAMA_PDO)
   {
      sm_DL_Read_req (port, ++addr);
   }
   else
   {
      iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
      iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;
      iolink_sm_event (
         port,
         (real_paramlist->revisionid == IOL_DIR_PARAM_REV_V10)
            ? SM_EVENT_V10
            : SM_EVENT_NOT_V10);
   }
}

static void sm_DL_Read_cnf_check_comp (
   iolink_job_t * job,
   iolink_sm_state_t state)
{
   iolink_port_t * port = job->port;
   uint8_t addr         = job->dl_rw_cnf.addr;

   if (addr != IOL_DIR_PARAMA_FID_2)
   {
      sm_DL_Read_req (port, ++addr);
   }
   else if (state == SM_STATE_CheckCompV10)
   {
      sm_do_check_comp_v10 (port);
   }
   else
   {
      sm_do_check_comp (port);
   }
}

static void sm_DL_Write_cnf_checkvxy (iolink_job_t * job)
{
   iolink_port_t * port                          = job->port;
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;
   iolink_smp_parameterlist_t * real_paramlist   = &sm->real_paramlist;

   if (
      (config_paramlist->revisionid == 0) ||
      (config_paramlist->revisionid == real_paramlist->revisionid))
   {
      iolink_sm_event (port, SM_EVENT_REVISION_OK); /* T22 */
   }
   else if (sm->CompRetry == 0)
   {
      iolink_sm_event (port, SM_EVENT_RETRY_STARTUP); /* T25 */
   }
   else
   {
      sm->error_event = SM_EVENT_RevisionFault;
      iolink_sm_event (port, SM_EVENT_RevisionFault); /* T6 */
   }
}

static void sm_DL_Write_cnf_restart_dev (iolink_job_t * job)
{
   iolink_port_t * port                          = job->port;
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;

   switch (job->dl_rw_cnf.addr)
   {
   case IOL_DIR_PARAMA_REV_ID:
   {
      if (config_paramlist->mode == IOLINK_SMTARGET_MODE_CFGCOM)
      {
         sm_DL_Write_req (
            port,
            IOL_DIR_PARAMA_DID_1,
            config_paramlist->deviceid >> 16);
      }
      else if (config_paramlist->mode == IOLINK_SMTARGET_MODE_AUTOCOM)
      {
         sm_DL_Write_req (
            port,
            IOL_DIR_PARAMA_MASTER_CMD,
            IOL_MASTERCMD_DEVICE_IDENT);
      }
      else
      {
         LOG_ERROR (
            IOLINK_SM_LOG,
            "SM (%u): %s, unexpected mode (%u)\n",
            iolink_get_portnumber (port),
            __func__,
            config_paramlist->mode);
         CC_ASSERT (0); /* Invalid mode */
      }
      break;
   }
   case IOL_DIR_PARAMA_DID_1:
      sm_DL_Write_req (port, IOL_DIR_PARAMA_DID_2, config_paramlist->deviceid >> 8);
      break;
   case IOL_DIR_PARAMA_DID_2:
      sm_DL_Write_req (port, IOL_DIR_PARAMA_DID_3, config_paramlist->deviceid);
      break;
   case IOL_DIR_PARAMA_DID_3:
      sm_DL_Write_req (port, IOL_DIR_PARAMA_MASTER_CMD, IOL_MASTERCMD_DEVICE_IDENT);
      break;
   case IOL_DIR_PARAMA_MASTER_CMD:
      sm_DL_Read_req (port, IOL_DIR_PARAMA_MIN_CYCL); /* Dummy cycle */
      break;
   default:
      break;
   }
}

static void sm_DL_Write_cnf_waiton_operate (iolink_job_t * job)
{
   iolink_port_t * port = job->port;
   uint8_t addr         = job->dl_rw_cnf.addr;

   if (addr == IOL_DIR_PARAMA_MASTER_CYCL)
   {
      sm_DL_Write_req (
         port,
         IOL_DIR_PARAMA_MASTER_CMD,
         IOL_MASTERCMD_DEVICE_OPERATE);
   }
   else if (addr == IOL_DIR_PARAMA_MASTER_CMD)
   {
      sm_DL_SetMode_req_operate (port);
   }
   else
   {
      LOG_WARNING (
         IOLINK_SM_LOG,
         "SM (%u): %s: Unexpected DL_Write_cnf(), %u\n",
         iolink_get_portnumber (port),
         __func__,
         addr);
   }
}

static void sm_check_sernum (iolink_port_t * port, bool SReadOk)
{
   iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;
   iolink_smp_parameterlist_t * real_paramlist   = &sm->real_paramlist;
   iolink_fsm_sm_event_t ret                     = SM_EVENT_NONE;

   if (config_paramlist->inspectionlevel != IOLINK_INSPECTIONLEVEL_IDENTICAL)
   {
      ret = SM_EVENT_SerNumOK; /* T10 */
   }
   else if (
      !SReadOk || memcmp (
                     config_paramlist->serialnumber,
                     real_paramlist->serialnumber,
                     sizeof (config_paramlist->serialnumber)))
   {
      ret             = SM_EVENT_SerNumFault; /* T11 */
      sm->error_event = ret;
   }
   else
   {
      ret = SM_EVENT_SerNumOK; /* T10 */
   }

   iolink_sm_event (port, ret);
}

/* SM state transitions, IO-Link Interface Spec v1.1.3 Chapter 9.2.3.2 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_sm_transition_t sm_trans_s0[] = {
   /* PortInactive_0 */
   {SM_EVENT_DL_Mode_STARTUP, SM_STATE_ReadComParameter, sm_readcomparameter}, /* T1 */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_PortInactive,
    sm_portinactive}, /* T14 */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive},                                            /* T15 */
   {SM_EVENT_SM_SetPortConfig_DI_or_DO, SM_STATE_DIDO, sm_dido}, /* T16 */

   {SM_EVENT_SM_Operate, SM_STATE_PortInactive, sm_smoperate_ignore}, /* Not in
                                                                         spec */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost_ignore}, /* Not
                                                                            in
                                                                            spec
                                                                          */
};

static const iolink_fsm_sm_transition_t sm_trans_s1[] = {
   /* waitonDLPreoperate_2 */
   {SM_EVENT_DL_Mode_PREOPERATE, SM_STATE_checkSerNum, sm_checkSerNum}, /* T8 */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST,
    SM_STATE_waitonDLPreoperate,
    sm_comlost_ignore}, /* Not in spec  */
};

static const iolink_fsm_sm_transition_t sm_trans_s2[] = {
   /* checkSerNum_3 */
   {SM_EVENT_SerNumOK, SM_STATE_wait, sm_wait},                       /* T10 */
   {SM_EVENT_SerNumFault, SM_STATE_InspectionFault, sm_inspectFault}, /* T11 */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost},         /* T3  */

   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive}, /* SDCI_TC_0167 */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_checkSerNum, sm_comlost_ignore}, /* Not
                                                                           in
                                                                           spec
                                                                         */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_waitForFallback,
    sm_fallback}, /* Not in spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s3[] = {
   /* wait_4 */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive},                                               /* T15 */
   {SM_EVENT_SM_SetPortConfig_DI_or_DO, SM_STATE_DIDO, sm_dido},    /* T16 */
   {SM_EVENT_SM_Operate_v10, SM_STATE_SMOperate, sm_smoperate_ind}, /* T12 for
                                                                       v1.0 */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost},   /* T3  */

   {SM_EVENT_WRITE_MASTER_CYCL_REQ,
    SM_STATE_write_master_cycl,
    sm_wr_master_cycl}, /* Not in spec */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_waitForFallback,
    sm_fallback}, /* Not in spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s4[] = {
   /* SMOperate_5 */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost},    /* T3  */
   {SM_EVENT_DL_Mode_OPERATE, SM_STATE_SMOperate, sm_smoperate_ind}, /* T13 */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_waitForFallback,
    sm_fallback}, /* Not in spec */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive},                                            /* T15 */
   {SM_EVENT_SM_SetPortConfig_DI_or_DO, SM_STATE_DIDO, sm_dido}, /* T16 */
};

static const iolink_fsm_sm_transition_t sm_trans_s5[] = {
   /* InspectionFault_6 */
   {SM_EVENT_RevisionFault, SM_STATE_InspectionFault, sm_inspectFault}, /* T6 */
   {SM_EVENT_CompFault, SM_STATE_InspectionFault, sm_inspectFault},    /* T7  */
   {SM_EVENT_CycTimeFault, SM_STATE_InspectionFault, sm_inspectFault}, /* T17 */

   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_waitForFallback,
    sm_fallback}, /* Not in spec */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive},                                             /* T15 */
   {SM_EVENT_SM_SetPortConfig_DI_or_DO, SM_STATE_DIDO, sm_dido},  /* T16 */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */
};

static const iolink_fsm_sm_transition_t sm_trans_s6[] = {
   /* waitonDLOperate_7 */
   {SM_EVENT_DL_Mode_OPERATE, SM_STATE_checkSerNum, sm_checkSerNum}, /* T9  */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost},        /* T3  */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost},    /* T3  */
};

static const iolink_fsm_sm_transition_t sm_trans_s7[] = {
   /* DIDO_8 */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_PortInactive,
    sm_portinactive}, /* T14 */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive},                                            /* T15 */
   {SM_EVENT_SM_SetPortConfig_DI_or_DO, SM_STATE_DIDO, sm_dido}, /* T16 */
};

static const iolink_fsm_sm_transition_t sm_trans_s8[] = {
   /* CheckCompatibility_1 ReadComParameter_20 */
   {SM_EVENT_NOT_V10, SM_STATE_CheckVxy, sm_checkVxy},        /* T21 */
   {SM_EVENT_V10, SM_STATE_CheckCompV10, sm_checkCompV10_21}, /* T20 */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST,
    SM_STATE_ReadComParameter,
    sm_comlost_ignore}, /* Not in spec */
   {SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM,
    SM_STATE_PortInactive,
    sm_portinactive}, /* Not in spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s9[] = {
   /* CheckCompatibility_1 CheckCompV10_21 */
   {SM_EVENT_V10CompOK, SM_STATE_waitonDLOperate, sm_waitonDLOp},      /* T4  */
   {SM_EVENT_V10CompFault, SM_STATE_InspectionFault, sm_inspectFault}, /* T5  */
   {SM_EVENT_CycTimeFault_V10, SM_STATE_InspectionFault, sm_inspectFault}, /* T18
                                                                            */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_CheckCompV10, sm_comlost_ignore}, /* Not
                                                                            in
                                                                            spec
                                                                          */
};

static const iolink_fsm_sm_transition_t sm_trans_s10[] = {
   /* CheckCompatibility_1 CheckVxy_22 */
   {SM_EVENT_REVISION_OK, SM_STATE_CheckComp, sm_checkComp}, /* T22  */
   {SM_EVENT_RETRY_STARTUP, SM_STATE_RestartDevice, sm_restartDevice}, /* T25 */
   {SM_EVENT_RevisionFault, SM_STATE_InspectionFault, sm_write_mcmd_preop}, /* T6
                                                                             */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_CheckVxy, sm_comlost_ignore}, /* Not in
                                                                        spec  */
};

static const iolink_fsm_sm_transition_t sm_trans_s11[] = {
   /* CheckCompatibility_1 CheckComp_23 */
   {SM_EVENT_CompOK, SM_STATE_waitonDLPreoperate, sm_write_mcmd_preop}, /* T2 */
   {SM_EVENT_RETRY_STARTUP, SM_STATE_RestartDevice, sm_restartDevice}, /* T23 */
   {SM_EVENT_CompFault, SM_STATE_InspectionFault, sm_write_mcmd_preop}, /* T7 */
   {SM_EVENT_CycTimeFault, SM_STATE_InspectionFault, sm_write_mcmd_preop}, /* T17
                                                                            */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_CheckComp, sm_comlost_ignore}, /* Not in
                                                                         spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s12[] = {
   /* CheckCompatibility_1 RestartDevice_24 */
   {SM_EVENT_WriteDone, SM_STATE_ReadComParameter, sm_readcomparameter}, /* T24
                                                                          */
   {SM_EVENT_CNF_COMLOST, SM_STATE_RestartDevice, sm_comlost}, /* T3  */

   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_CheckComp, sm_comlost_ignore}, /* Not in
                                                                         spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s13[] = {
   /* WaitForFallback not in spec */
   {SM_EVENT_SM_SetPortConfig_INACTIVE,
    SM_STATE_PortInactive,
    sm_portinactive},                                             /* T14 */
   {SM_EVENT_DL_Mode_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */
};

static const iolink_fsm_sm_transition_t sm_trans_s14[] = {
   /* SM_STATE_write_master_cycl Write master cycle time and wait for the cnf,
      not in spec */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost}, /* T3  */

   {SM_EVENT_WRITE_MASTER_CYCL_DONE,
    SM_STATE_wait_devmode_cnf,
    sm_write_devmode}, /* Not in spec */
   {SM_EVENT_DL_Mode_COMLOST,
    SM_STATE_write_master_cycl,
    sm_comlost_ignore}, /* Not in spec */
};

static const iolink_fsm_sm_transition_t sm_trans_s15[] = {
   /* SM_STATE_wait_devmode_cnf Write master devicemode operate and wait for the
      cnf, not in spec */
   {SM_EVENT_SM_Operate_v10, SM_STATE_SMOperate, sm_smoperate_ind}, /* T12 for v1.0 */
   {SM_EVENT_SM_Operate, SM_STATE_SMOperate, sm_smoperate_ind}, /* T12 */
   {SM_EVENT_CNF_COMLOST, SM_STATE_PortInactive, sm_comlost},   /* T3  */

   {SM_EVENT_DL_Mode_COMLOST,
    SM_STATE_wait_devmode_cnf,
    sm_comlost_ignore}, /* Not in spec */
};

/* The index is the state in this array */
static const iolink_fsm_sm_state_transitions_t iolink_sm_fsm[] = {
   {/* PortInactive_0 */
    .number_of_trans = NELEMENTS (sm_trans_s0),
    .transitions     = sm_trans_s0},
   {/* waitonDLPreoperate_2 */
    .number_of_trans = NELEMENTS (sm_trans_s1),
    .transitions     = sm_trans_s1},
   {/* checkSerNum_3 */
    .number_of_trans = NELEMENTS (sm_trans_s2),
    .transitions     = sm_trans_s2},
   {/* wait_4 */
    .number_of_trans = NELEMENTS (sm_trans_s3),
    .transitions     = sm_trans_s3},
   {/* SMOperate_5 */
    .number_of_trans = NELEMENTS (sm_trans_s4),
    .transitions     = sm_trans_s4},
   {/* InspectionFault_6 */
    .number_of_trans = NELEMENTS (sm_trans_s5),
    .transitions     = sm_trans_s5},
   {/* waitonDLOperate_7 */
    .number_of_trans = NELEMENTS (sm_trans_s6),
    .transitions     = sm_trans_s6},
   {/* DIDO_8 */
    .number_of_trans = NELEMENTS (sm_trans_s7),
    .transitions     = sm_trans_s7},
   {/* CheckCompatibility_1 ReadComParameter_20 */
    .number_of_trans = NELEMENTS (sm_trans_s8),
    .transitions     = sm_trans_s8},
   {/* CheckCompatibility_1 CheckCompV10_21 */
    .number_of_trans = NELEMENTS (sm_trans_s9),
    .transitions     = sm_trans_s9},
   {/* CheckCompatibility_1 CheckVxy_22 */
    .number_of_trans = NELEMENTS (sm_trans_s10),
    .transitions     = sm_trans_s10},
   {/* CheckCompatibility_1 CheckComp_23 */
    .number_of_trans = NELEMENTS (sm_trans_s11),
    .transitions     = sm_trans_s11},
   {/* CheckCompatibility_1 RestartDevice_24 */
    .number_of_trans = NELEMENTS (sm_trans_s12),
    .transitions     = sm_trans_s12},
   {/* WaitForFallback not in spec */
    .number_of_trans = NELEMENTS (sm_trans_s13),
    .transitions     = sm_trans_s13},
   {/* SM_STATE_write_master_cycl not in spec */
    .number_of_trans = NELEMENTS (sm_trans_s14),
    .transitions     = sm_trans_s14},
   {/* SM_STATE_wait_devmode_cnf not in spec */
    .number_of_trans = NELEMENTS (sm_trans_s15),
    .transitions     = sm_trans_s15},
};

static void iolink_sm_event (iolink_port_t * port, iolink_fsm_sm_event_t event)
{
   do
   {
      int i;
      iolink_sm_port_t * sm                         = iolink_get_sm_ctx (port);
      iolink_sm_state_t previous                    = sm->state;
      const iolink_fsm_sm_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_sm_fsm[previous].number_of_trans; i++)
      {
         if (iolink_sm_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_sm_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (next_trans == NULL)
      {
         LOG_ERROR (
            IOLINK_SM_LOG,
            "SM (%u): next_trans == NULL: SM state = %s, SM event = %s\n",
            iolink_get_portnumber (port),
            iolink_sm_state_literals[previous],
            iolink_fsm_sm_event_literals[event]);
         return;
      }

      /* Transition to next state */
      sm->state = next_trans->next_state;

      LOG_DEBUG (
         IOLINK_SM_LOG,
         "SM (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_fsm_sm_event_literals[event],
         iolink_sm_state_literals[previous],
         iolink_sm_state_literals[sm->state]);

      event = next_trans->action (port, event);
   } while (event != SM_EVENT_NONE);
}

static void set_type_0_or_type_1_2 (
   uint8_t operate,
   uint8_t cycbyte,
   iolink_mode_vl_t * valuelist)
{
   CC_ASSERT ((operate == 0) || (operate == 1));

   set_valuelist (
      valuelist,
      operate + 1,
      0,
      0,
      cycbyte,
      (operate == 0) ? IOLINK_MSEQTYPE_TYPE_0 : IOLINK_MSEQTYPE_TYPE_1_2);
}

static void set_type_2_V (
   bool pdin_isbytes,
   bool pdout_isbytes,
   uint8_t pdin,
   uint8_t pdout,
   uint8_t onreqdatalengthpermessage,
   uint8_t cycbyte,
   iolink_mode_vl_t * valuelist)
{
   if ((pdin_isbytes && (pdin != 1)) || (!pdin_isbytes && (pdin < 17)))
   {
      valuelist->pdinputlength = (pdin_isbytes) ? (pdin + 1) : ((pdin + 7) >> 3);

      if ((pdout_isbytes && (pdout != 1)) || (!pdout_isbytes && (pdout < 17)))
      {
         valuelist->pdoutputlength = (pdout_isbytes) ? (pdout + 1)
                                                     : ((pdout + 7) >> 3);
         valuelist->onreqdatalengthpermessage = onreqdatalengthpermessage;
         valuelist->type                      = IOLINK_MSEQTYPE_TYPE_2_V;
         valuelist->time                      = cycbyte;
      }
   }
}

static iolink_error_t populate_valuelist (
   uint8_t mscapa,
   uint8_t revid,
   uint8_t pdin,
   uint8_t pdout,
   uint8_t cycbyte,
   bool operate,
   iolink_mode_vl_t * valuelist)
{
   valuelist->type = IOLINK_MSEQTYPE_TYPE_NONE;

   if (!operate)
   {
      uint8_t preop_m = (mscapa >> 4) & 0x03;

      if (preop_m < 2)
      {
         set_type_0_or_type_1_2 (preop_m, cycbyte, valuelist);
      }
      else
      {
         set_valuelist (
            valuelist,
            (preop_m == 2) ? 8 : 32,
            0,
            0,
            cycbyte,
            IOLINK_MSEQTYPE_TYPE_1_V);
      }
   }
   else
   {
      bool pdin_isbytes  = ((pdin & BIT (7)) != 0);
      bool pdout_isbytes = ((pdout & BIT (7)) != 0);
      pdin               = (pdin & 0x1F);
      pdout              = (pdout & 0x1F);

      uint8_t oper_m = (mscapa >> 1) & 0x07;
      bool legacy    = (revid != IOLINK_REVISION_1_1);

      if (legacy)
      {
         if ((pdin == 0) && (pdout == 0))
         {
            set_type_0_or_type_1_2 (oper_m, cycbyte, valuelist);
         }
         else
         {
            if ((pdin_isbytes || pdout_isbytes) && ((pdin >= 3) || (pdout >= 3)))
            {
               // SDCI_TC_0286
               set_valuelist (valuelist, 2, 0, 0, cycbyte, IOLINK_MSEQTYPE_TYPE_1_1);
            }
            else
            {
               if (pdout == 0)
               {
                  if ((pdin > 0) && (pdin <= 8))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        1,
                        0,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_1);
                  }
                  else if ((pdin >= 9) && (pdin <= 16))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        2,
                        0,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_2);
                  }
               }
               else if (pdin == 0)
               {
                  if ((pdout > 0) && (pdout <= 8))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        0,
                        1,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_3);
                  }
                  else if ((pdout >= 9) && (pdout <= 16))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        0,
                        2,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_4);
                  }
               }
               else if ((pdin >= 1) && (pdin <= 8) && (pdout >= 1) && (pdout <= 8))
               {
                  set_valuelist (
                     valuelist,
                     1,
                     1,
                     1,
                     cycbyte,
                     IOLINK_MSEQTYPE_TYPE_2_5);
               }
               else if ((pdin == 16) && (pdout == 16))  /* Not in spec for legacy devices, exist in the wild */
               {
                  set_valuelist (
                     valuelist,
                     1,
                     2,
                     2,
                     cycbyte,
                     IOLINK_MSEQTYPE_TYPE_2_6);
               }
            }
         }
      }
      else
      {
         switch (oper_m)
         {
         case 0:
            if ((pdin == 0) && (pdout == 0))
            {
               set_valuelist (valuelist, 1, 0, 0, cycbyte, IOLINK_MSEQTYPE_TYPE_0);
            }
            else if (!pdin_isbytes && !pdout_isbytes)
            {
               if (pdout == 0)
               {
                  if ((pdin >= 1) && (pdin <= 8))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        1,
                        0,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_1);
                  }
                  else if ((pdin >= 9) && (pdin <= 16))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        2,
                        0,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_2);
                  }
               }
               else
               {
                  if (pdin == 0)
                  {
                     if ((pdout >= 1) && (pdout <= 8))
                     {
                        set_valuelist (
                           valuelist,
                           1,
                           0,
                           1,
                           cycbyte,
                           IOLINK_MSEQTYPE_TYPE_2_3);
                     }
                     else if ((pdout >= 9) && (pdout <= 16))
                     {
                        set_valuelist (
                           valuelist,
                           1,
                           0,
                           2,
                           cycbyte,
                           IOLINK_MSEQTYPE_TYPE_2_4);
                     }
                  }
                  else if ((pdin >= 1) && (pdin <= 8) && (pdout >= 1) && (pdout <= 8))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        1,
                        1,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_5);
                  }
                  else if (
                     ((pdin >= 9) && (pdin <= 16) && (pdout >= 1) &&
                      (pdout <= 16)) ||
                     ((pdin >= 1) && (pdin <= 16) && (pdout >= 9) &&
                      (pdout <= 16)))
                  {
                     set_valuelist (
                        valuelist,
                        1,
                        (pdin >= 9) ? 2 : 1,
                        (pdout >= 9) ? 2 : 1,
                        cycbyte,
                        IOLINK_MSEQTYPE_TYPE_2_V);
                  }
               }
            }
            else
            {
               // SDCI_TC_0173
               set_valuelist (valuelist, 2, 0, 0, cycbyte, IOLINK_MSEQTYPE_TYPE_1_1);
            }
            break;
         case 1:
            if ((pdin == 0) && (pdout == 0))
            {
               set_valuelist (valuelist, 2, 0, 0, cycbyte, IOLINK_MSEQTYPE_TYPE_1_2);
            }
            break;
         case 4:
            if (pdin_isbytes && pdout_isbytes)
            {
               if ((pdout >= 3) || (pdin >= 3))
               {
                  set_valuelist (
                     valuelist,
                     1,
                     pdin + 1,
                     pdout + 1,
                     cycbyte,
                     IOLINK_MSEQTYPE_TYPE_2_V);
               }
            }
            else
            {
               set_type_2_V (
                  pdin_isbytes,
                  pdout_isbytes,
                  pdin,
                  pdout,
                  1,
                  cycbyte,
                  valuelist);
            }
            break;
         case 5:
         case 6:
         case 7:
         {
            uint8_t onreqdatalength = 1 << ((2 * oper_m) - 9);

            if ((pdin > 0) || (pdout > 0))
            {
               set_type_2_V (
                  pdin_isbytes,
                  pdout_isbytes,
                  pdin,
                  pdout,
                  onreqdatalength,
                  cycbyte,
                  valuelist);
            }
            else if (oper_m != 5)
            {
               set_valuelist (
                  valuelist,
                  onreqdatalength,
                  0,
                  0,
                  cycbyte,
                  IOLINK_MSEQTYPE_TYPE_1_V);
            }
         }
         break;
         }
      }
   }

   return (valuelist->type == IOLINK_MSEQTYPE_TYPE_NONE)
             ? IOLINK_ERROR_INCORRECT_DATA
             : IOLINK_ERROR_NONE;
}

/* SM job callback functions */
static void sm_write_cycl_cnf_cb (iolink_job_t * job)
{
   iolink_sm_event (
      job->port,
      (job->al_write_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE)
         ? SM_EVENT_CNF_COMLOST
         : SM_EVENT_WRITE_MASTER_CYCL_DONE);
}

static void sm_AL_Read_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port                        = job->port;
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   if(job->al_read_cnf.errortype == IOLINK_SMI_ERRORTYPE_IDX_NOTAVAIL)
   {
       memset (real_paramlist->serialnumber, 0, sizeof (real_paramlist->serialnumber));
       sm_check_sernum (port, true);
   }

   else if (job->al_read_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE)
   {
      iolink_sm_event (
         port,
         (real_paramlist->revisionid == IOL_DIR_PARAM_REV_V11)
            ? SM_EVENT_CNF_COMLOST
            : SM_EVENT_DL_Mode_COMLOST);
   }
   else
   {
      uint8_t len = job->al_read_cnf.data_len;

      if (len > sizeof (real_paramlist->serialnumber))
      {
         iolink_sm_event (port, SM_EVENT_CNF_COMLOST);
      }
      else
      {
         memset (
            real_paramlist->serialnumber,
            0,
            sizeof (real_paramlist->serialnumber));
         memcpy (real_paramlist->serialnumber, job->al_read_cnf.data, len);
         sm_check_sernum (port, true);
      }
   }
}

static void sm_DL_Mode_ind_cb_preop_op (
   iolink_job_t * job,
   iolink_mhmode_t mode,
   iolink_fsm_sm_event_t event)
{
   iolink_mhmode_t realmode = job->dl_mode;

   if (realmode == mode)
   {
      iolink_sm_event (job->port, event);
   }
   else
   {
      LOG_ERROR (
         IOLINK_SM_LOG,
         "SM (%u): %s: bad DL_Mode = MH mode %s (%u)\n",
         iolink_get_portnumber (job->port),
         __func__,
         iolink_mhmode_literals[realmode],
         realmode);
   }
}

static void sm_DL_Mode_ind_cb_preop (iolink_job_t * job)
{
   sm_DL_Mode_ind_cb_preop_op (
      job,
      IOLINK_MHMODE_PREOPERATE,
      SM_EVENT_DL_Mode_PREOPERATE);
}

static void sm_DL_Mode_ind_cb_operate (iolink_job_t * job)
{
   sm_DL_Mode_ind_cb_preop_op (job, IOLINK_MHMODE_OPERATE, SM_EVENT_DL_Mode_OPERATE);
}

static void sm_DL_Mode_ind_cb_inactive (iolink_job_t * job)
{
   iolink_mhmode_t realmode = job->dl_mode;

   switch (realmode)
   {
   case IOLINK_MHMODE_STARTUP: /* T1 */
      iolink_sm_event (job->port, SM_EVENT_DL_Mode_STARTUP);
      break;
   case IOLINK_MHMODE_INACTIVE: /* Not in FSM */
   {
      iolink_sm_port_t * sm = iolink_get_sm_ctx (job->port);
      iolink_smp_parameterlist_t * config_paramlist = &sm->config_paramlist;

      /* Check if requested mode is INACTIVE */
      if (config_paramlist->mode == IOLINK_SMTARGET_MODE_INACTIVE)
      {
         SM_PortMode_ind (job->port, IOLINK_SM_PORTMODE_INACTIVE);
      }
      else
      {
         /* WURQ failed. Signal it as COMLOST. The application is responsible
          * for attempting to start the port again.
          */
         SM_PortMode_ind (job->port, IOLINK_SM_PORTMODE_COMLOST);
      }
      break;
   }
   default:
      LOG_ERROR (
         IOLINK_SM_LOG,
         "SM (%u): %s: bad DL_Mode = MH mode %s (%u)\n",
         iolink_get_portnumber (job->port),
         __func__,
         iolink_mhmode_literals[realmode],
         realmode);
      break;
   }
}

static void sm_DL_Mode_ind_cb_inspectionfault (iolink_job_t * job)
{
   iolink_mhmode_t realmode = job->dl_mode;

   if ((realmode != IOLINK_MHMODE_PREOPERATE) && (realmode != IOLINK_MHMODE_OPERATE))
   {
      LOG_ERROR (
         IOLINK_SM_LOG,
         "SM (%u): %s: unexpected DL_Mode = MH mode %s (%u)\n",
         iolink_get_portnumber (job->port),
         __func__,
         iolink_mhmode_literals[realmode],
         realmode);
   }
}

static void sm_DL_Mode_ind_cb (iolink_job_t * job)
{
   iolink_port_t * port     = job->port;
   iolink_mhmode_t realmode = job->dl_mode;
   iolink_sm_port_t * sm    = iolink_get_sm_ctx (port);

   LOG_DEBUG (
      IOLINK_SM_LOG,
      "SM (%u): %s: DL_Mode = MH mode %s (%u)\n",
      iolink_get_portnumber (port),
      __func__,
      iolink_mhmode_literals[realmode],
      realmode);

   if (
      (realmode == IOLINK_MHMODE_COM1) || (realmode == IOLINK_MHMODE_COM2) ||
      (realmode == IOLINK_MHMODE_COM3))
   {
      sm->comrate = realmode;
      LOG_INFO (
         IOLINK_SM_LOG,
         "SM (%u), %s: Bad realmode MH mode %s\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_mhmode_literals[realmode]);
   }
   else
   {
      sm->mh_mode = realmode;
   }

   if (realmode == IOLINK_MHMODE_COMLOST)
   {
      switch (sm->state)
      {
      case SM_STATE_PortInactive:
      case SM_STATE_DIDO:
         LOG_ERROR (
            IOLINK_SM_LOG,
            "SM (%u): %s: COMLOST when in SM state %s\n",
            iolink_get_portnumber (port),
            __func__,
            iolink_sm_state_literals[sm->state]);
         break;
      default:
         iolink_sm_event (port, SM_EVENT_DL_Mode_COMLOST);
         break;
      }
   }
   else
   {
      switch (sm->state)
      {
      case SM_STATE_PortInactive:
         sm_DL_Mode_ind_cb_inactive (job);
         break;
      case SM_STATE_waitonDLPreoperate:
         sm_DL_Mode_ind_cb_preop (job);
         break;
      case SM_STATE_SMOperate:
      case SM_STATE_waitonDLOperate:
         sm_DL_Mode_ind_cb_operate (job);
         break;
      case SM_STATE_InspectionFault:
         sm_DL_Mode_ind_cb_inspectionfault (job);
         break;
      default:
         LOG_ERROR (
            IOLINK_SM_LOG,
            "SM (%u): %s: bad DL_Mode = MH mode %s (%u)\n",
            iolink_get_portnumber (port),
            __func__,
            iolink_mhmode_literals[realmode],
            realmode);
         iolink_sm_event (port, SM_EVENT_DL_Mode_COMLOST);
         break;
      }
   }
}

static void sm_operate_cb (iolink_job_t * job)
{
   iolink_port_t * port                        = job->port;
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   if (
      (real_paramlist->revisionid != IOL_DIR_PARAM_REV_V10) ||
      (real_paramlist->vendorid != IOL_SM_VENDOR_MESCO_ENGINEERING)) // SDCI_TC_0196
   {
      iolink_sm_event (port, SM_EVENT_WRITE_MASTER_CYCL_REQ);
   }
   else
   {
      iolink_sm_event (port, SM_EVENT_SM_Operate_v10); /* T12 */
   }

   // ErrorInfo:
   // Permitted values: TIMING_CONFLICT (the requested combination of cycle
   // times for the activated ports return IOLINK_ERROR_INFO_TIMING_CONFLICT;
}

static void sm_setportcfg_req_cb (iolink_job_t * job)
{
   iolink_port_t * port        = job->port;
   iolink_fsm_sm_event_t event = SM_EVENT_SM_SetPortConfig_INACTIVE;
   iolink_sm_port_t * sm       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * parameterlist = &job->sm_setportcfg_req.paramlist;

   switch (sm->state)
   {
   case SM_STATE_PortInactive:
   case SM_STATE_wait:
   case SM_STATE_SMOperate:
   case SM_STATE_InspectionFault:
   case SM_STATE_DIDO:
   case SM_STATE_checkSerNum:
      break;
   default:
      LOG_INFO (
         IOLINK_SM_LOG,
         "SM (%u): state (%s) does not allow setportcfg\n",
         iolink_get_portnumber (port),
         iolink_sm_state_literals[sm->state]);
      // iolink_sm_event (port, SM_EVENT_DL_Mode_COMLOST); // TODO ?!?
      // TODO return busy

      return;
   }

   switch (parameterlist->mode)
   {
   case IOLINK_SMTARGET_MODE_CFGCOM:
   case IOLINK_SMTARGET_MODE_AUTOCOM:
      event = SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM;
      break;
   case IOLINK_SMTARGET_MODE_DI:
   case IOLINK_SMTARGET_MODE_DO:
      event = SM_EVENT_SM_SetPortConfig_DI_or_DO;
      break;
   default:
      break;
   }

   LOG_DEBUG (
      IOLINK_SM_LOG,
      "SM (%u): %s: iolink_sm_target_mode_t = %s\n",
      iolink_get_portnumber (port),
      __func__,
      iolink_sm_target_mode_literals[parameterlist->mode]);

   char save_serial_number[16] = {0};
   if (sm->config_paramlist.serialnumber[0] != 0)
   {
      // SDCI_TC_0196: save configured serial number
      memcpy (save_serial_number, sm->config_paramlist.serialnumber, 16);
   }

   /* Store configuration */
   memcpy (
      &sm->config_paramlist,
      parameterlist,
      sizeof (iolink_smp_parameterlist_t));

   if (save_serial_number[0] != 0)
   {
      // SDCI_TC_0196: restore configured serial number
      memcpy (sm->config_paramlist.serialnumber, save_serial_number, 16);
   }

   iolink_sm_event (port, event);
}

static void sm_DL_Read_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port                        = job->port;
   iolink_sm_port_t * sm                       = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;
   uint8_t addr                                = job->dl_rw_cnf.addr;
   uint8_t value                               = job->dl_rw_cnf.val;

   if (job->dl_rw_cnf.stat != IOLINK_STATUS_NO_ERROR)
   {
      iolink_sm_event (port, SM_EVENT_CNF_COMLOST);

      return;
   }

   if (addr == IOL_DIR_PARAMA_DUMMY_WURQ)
   {
      /* DL message handler will respond with a test message (MIN_CYCL) after
       * successful WURQ.
       * Ignore it this time, and don't expect until after PortInactive
       */
      return;
   }

   switch (addr)
   {
   case IOL_DIR_PARAMA_MIN_CYCL:
   {
#define MIN_CYCL_TIME_1   92
#define MIN_CYCL_TIME_2_3 79
#define FOUR_MS           40
#define MIN_CYCL_TIME_32MS 0x80

      uint8_t mincycletime = (sm->comrate == IOLINK_MHMODE_COM1)
                                ? MIN_CYCL_TIME_1
                                : MIN_CYCL_TIME_2_3;

      uint8_t new_cycletime     = (value < mincycletime) ? mincycletime : value;

#ifdef ILINK_USB_MODE_ENABLE
      if (new_cycletime < MIN_CYCL_TIME_32MS)
      {
         new_cycletime = MIN_CYCL_TIME_32MS;
         LOG_DEBUG (
            IOLINK_SM_LOG,
            "SM (%u): reducing cycle time to %x due to slow os/interface\n",
            iolink_get_portnumber (port),
            new_cycletime);
      }
#endif

      real_paramlist->cycletime = new_cycletime;
   }
      sm->dev_com.mincycle = value;
      break;
   case IOL_DIR_PARAMA_MSEQ_CAP:
      sm->dev_com.mseq_cap = value;
      break;
   case IOL_DIR_PARAMA_REV_ID:
      real_paramlist->revisionid = value;
      break;
   case IOL_DIR_PARAMA_PDI:
      sm->dev_com.pdi = value;
      break;
   case IOL_DIR_PARAMA_PDO:
      sm->dev_com.pdo = value;
      break;
   case IOL_DIR_PARAMA_VID_1:
      real_paramlist->vendorid = value << 8;
      break;
   case IOL_DIR_PARAMA_VID_2:
      real_paramlist->vendorid |= value;
      break;
   case IOL_DIR_PARAMA_DID_1:
      real_paramlist->deviceid = value << 16;
      break;
   case IOL_DIR_PARAMA_DID_2:
      real_paramlist->deviceid |= value << 8;
      break;
   case IOL_DIR_PARAMA_DID_3:
      real_paramlist->deviceid |= value;
      LOG_DEBUG (
         IOLINK_SM_LOG,
         "SM (%u): vendorid = 0x%04x deviceid = 0x%06lx\n",
         iolink_get_portnumber (port),
         real_paramlist->vendorid,
         (unsigned long)real_paramlist->deviceid);
      break;
   case IOL_DIR_PARAMA_FID_1:
      real_paramlist->functionid = value << 8;
      break;
   case IOL_DIR_PARAMA_FID_2:
      real_paramlist->functionid |= value;
      break;
   default:
      CC_ASSERT (0);
      break;
   }

   switch (sm->state)
   {
   case SM_STATE_ReadComParameter:
      sm_DL_Read_cnf_read_com_param (job);
      break;
   case SM_STATE_CheckComp:
   case SM_STATE_CheckCompV10:
      sm_DL_Read_cnf_check_comp (job, sm->state);
      break;
   case SM_STATE_RestartDevice:
      iolink_sm_event (port, SM_EVENT_WriteDone);
      break;
   default:
      CC_ASSERT (0); // TODO
      break;
   }
}

static void sm_DL_Write_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port = job->port;

   if (job->dl_rw_cnf.stat != IOLINK_STATUS_NO_ERROR)
   {
      iolink_sm_event (port, SM_EVENT_CNF_COMLOST);
   }
   else
   {
      iolink_sm_port_t * sm = iolink_get_sm_ctx (port);

      switch (sm->state)
      {
      case SM_STATE_waitonDLPreoperate:
         sm_DL_SetMode_req_preop (port); /* T2 */
         break;
      case SM_STATE_InspectionFault:
         iolink_sm_event (port, sm->error_event);
         break;
      case SM_STATE_CheckVxy:
         sm_DL_Write_cnf_checkvxy (job);
         break;
      case SM_STATE_RestartDevice:
         sm_DL_Write_cnf_restart_dev (job);
         break;
      case SM_STATE_waitonDLOperate:
         sm_DL_Write_cnf_waiton_operate (job);
         break;
      default:
         CC_ASSERT (0);
         break;
      }
   }
}

static void sm_DL_Write_Devmode_cnf (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);
   iolink_smp_parameterlist_t * real_paramlist = &sm->real_paramlist;

   if (job->dl_write_devicemode_cnf.errorinfo == IOLINK_STATUS_NO_ERROR)
   {
      if (sm->state == SM_STATE_wait_devmode_cnf)
      {
         sm_DL_SetMode_req_operate (port);
         iolink_sm_event (
            port,
            (real_paramlist->revisionid == IOL_DIR_PARAM_REV_V10)
            ? SM_EVENT_SM_Operate_v10
            : SM_EVENT_SM_Operate); /* T12 */
         return;
      }
      else if (sm->state == SM_STATE_waitForFallback)
      {
         iolink_sm_event (job->port, SM_EVENT_SM_SetPortConfig_INACTIVE); /* Not
                                                                             in
                                                                             spec
                                                                           */

         return;
      }
   }

   iolink_sm_event (
      port,
      (sm->state == SM_STATE_wait_devmode_cnf) ? SM_EVENT_CNF_COMLOST
                                               : SM_EVENT_DL_Mode_COMLOST);
}

/* Stack internal API */
void iolink_sm_init (iolink_port_t * port)
{
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);

   sm->state   = SM_STATE_PortInactive;
   sm->dl_addr = IOL_DIR_PARAMA_DUMMY_WURQ;
}

iolink_error_t SM_Operate (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_SM_OPERATE_REQ,
      sm_operate_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t SM_SetPortConfig_req (
   iolink_port_t * port,
   iolink_smp_parameterlist_t * parameterlist)
{
   if (!((parameterlist->inspectionlevel == IOLINK_INSPECTIONLEVEL_NO_CHECK) ||
         (parameterlist->inspectionlevel == IOLINK_INSPECTIONLEVEL_TYPE_COMP) ||
         (parameterlist->inspectionlevel == IOLINK_INSPECTIONLEVEL_IDENTICAL)))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }
   else
   {
      switch (parameterlist->mode)
      {
      case IOLINK_SMTARGET_MODE_INACTIVE:
      case IOLINK_SMTARGET_MODE_CFGCOM:
      case IOLINK_SMTARGET_MODE_AUTOCOM:
      case IOLINK_SMTARGET_MODE_DI:
      case IOLINK_SMTARGET_MODE_DO:
         break;
      default:
         return IOLINK_ERROR_PARAMETER_CONFLICT;
      }
   }

   iolink_job_t * job = iolink_fetch_avail_job (port);
   /* Copy configuration */
   memcpy (
      &job->sm_setportcfg_req.paramlist,
      parameterlist,
      sizeof (iolink_smp_parameterlist_t));

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_SM_SET_PORT_CFG_REQ,
      sm_setportcfg_req_cb);

   return IOLINK_ERROR_NONE;
}

void DL_Read_cnf (iolink_port_t * port, uint8_t value, iolink_status_t errorinfo)
{
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);

   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.addr = sm->dl_addr;
   job->dl_rw_cnf.val  = value;
   job->dl_rw_cnf.stat = errorinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_READ_CNF,
      sm_DL_Read_cnf_cb);
}

void DL_Write_cnf (iolink_port_t * port, iolink_status_t errorinfo)
{
   iolink_sm_port_t * sm = iolink_get_sm_ctx (port);

   iolink_job_t * job  = iolink_fetch_avail_job (port);
   job->dl_rw_cnf.addr = sm->dl_addr;
   job->dl_rw_cnf.stat = errorinfo;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_WRITE_CNF,
      sm_DL_Write_cnf_cb);
}

void DL_Write_Devicemode_cnf (
   iolink_port_t * port,
   iolink_status_t errorinfo,
   iolink_dl_mode_t devicemode)
{
   iolink_job_t * job                     = iolink_fetch_avail_job (port);
   job->dl_write_devicemode_cnf.errorinfo = errorinfo;
   job->dl_write_devicemode_cnf.mode      = devicemode;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_WRITE_DEVMODE_CNF,
      sm_DL_Write_Devmode_cnf);
}

void DL_Mode_ind_baud (iolink_port_t * port, iolink_mhmode_t realmode)
{
   if (
      (realmode == IOLINK_MHMODE_COM1) || (realmode == IOLINK_MHMODE_COM2) ||
      (realmode == IOLINK_MHMODE_COM3))
   {
      iolink_sm_port_t * sm = iolink_get_sm_ctx (port);

      sm->comrate = realmode;
      LOG_DEBUG (
         IOLINK_SM_LOG,
         "SM (%u): %s: Set realmode MH mode %s\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_mhmode_literals[realmode]);
   }
   else
   {
      LOG_INFO (
         IOLINK_SM_LOG,
         "SM (%u): %s: Bad realmode MH mode %s\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_mhmode_literals[realmode]);
   }
}

void DL_Mode_ind (iolink_port_t * port, iolink_mhmode_t realmode)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);
   job->dl_mode       = realmode;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DL_MODE_IND,
      sm_DL_Mode_ind_cb);
}
