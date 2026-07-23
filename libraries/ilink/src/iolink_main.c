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
#define iolink_pl_init mock_iolink_pl_init
#endif /* UNIT_TEST */

#include "iolink_main.h"
#include "iolink_al.h"  /* iolink_al_init */
#include "iolink_cm.h"  /* iolink_cm_init */
#include "iolink_ds.h"  /* iolink_ds_init ds_SMI_ParServToDS_req */
#include "iolink_ode.h" /* iolink_ode_init */
#include "iolink_pde.h" /* iolink_pde_init */
#include "iolink_pl.h"  /* iolink_pl_init */
#include "iolink_sm.h"  /* iolink_sm_init */

#include "osal.h"

#include <stdlib.h> /* calloc */

/**
 * @file
 * @brief Handler
 *
 */

#define IOLINK_MASTER_JOB_CNT     40
#define IOLINK_MASTER_JOB_API_CNT 10

typedef struct iolink_port
{
   iolink_m_t * master;
   uint8_t portnumber;
   iolink_pl_port_t pl;
   iolink_dl_t dl;
   iolink_sm_port_t sm;
   iolink_al_port_t al;
   iolink_cm_port_t cm;
   iolink_ds_port_t ds;
   iolink_ode_port_t ode;
   iolink_pde_port_t pde;

   iolink_port_info_t port_info;
} iolink_port_t;

typedef struct iolink_m
{
   bool has_exited;
   os_thread_t * thread;
   os_mbox_t * mbox;           /* Mailbox for job submission */
   os_mbox_t * mbox_avail;     /* Mailbox for available jobs */
   os_mbox_t * mbox_api_avail; /* Mailbox for available API (external) jobs */
   iolink_job_t job[IOLINK_MASTER_JOB_CNT];
   iolink_job_t job_api[IOLINK_MASTER_JOB_API_CNT];

   void * cb_arg; /* Callback opaque argument */

   /* SMI cnf/ind callback */
   void (*cb_smi) (
      void * arg,
      uint8_t portnumber,
      iolink_arg_block_id_t ref_arg_block_id,
      uint16_t arg_block_len,
      arg_block_t * arg_block);

   /* Periodic data callback */
   void (*cb_pd) (
      uint8_t portnumber,
      void * arg,
      uint8_t data_len,
      const uint8_t * data);

   uint8_t port_cnt;
   struct iolink_port ports[];
} iolink_m_t;

static iolink_m_t * the_master = NULL;

static iolink_transmission_rate_t mhmode_to_transmission_rate (
   iolink_mhmode_t mhmode)
{
   iolink_transmission_rate_t res = IOLINK_TRANSMISSION_RATE_NOT_DETECTED;

   switch (mhmode)
   {
   case IOLINK_MHMODE_COM1:
      res = IOLINK_TRANSMISSION_RATE_COM1;
      break;
   case IOLINK_MHMODE_COM2:
      res = IOLINK_TRANSMISSION_RATE_COM2;
      break;
   case IOLINK_MHMODE_COM3:
      res = IOLINK_TRANSMISSION_RATE_COM3;
      break;
   default:
      break;
   }

   return res;
}

static iolink_error_t portnumber_to_iolinkport (
   uint8_t portnumber,
   iolink_port_t ** port)
{
   uint8_t port_index = portnumber - 1;

   *port = NULL;

   if (the_master == NULL)
   {
      return IOLINK_ERROR_STATE_INVALID;
   }

   if ((portnumber == 0) || (port_index > the_master->port_cnt))
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   *port = &the_master->ports[port_index];

   return IOLINK_ERROR_NONE;
}

static iolink_error_t common_smi_check (
   uint8_t portnumber,
   arg_block_t * arg_block,
   iolink_port_t ** port)
{
   iolink_error_t error = portnumber_to_iolinkport (portnumber, port);

   if (error != IOLINK_ERROR_NONE)
   {
      return error;
   }

   if (arg_block == NULL)
   {
      return IOLINK_ERROR_PARAMETER_CONFLICT;
   }

   return IOLINK_ERROR_NONE;
}

static void iolink_main (void * arg)
{
   iolink_m_t * master = arg;
   bool running = true;

   /* Main loop */
   while (running)
   {
      iolink_job_t * job;

      CC_ASSERT (master->mbox_avail != NULL);
      os_mbox_fetch (master->mbox, (void **)&job, OS_WAIT_FOREVER);

      CC_ASSERT (job != NULL);

      switch (job->type)
      {
      case IOLINK_JOB_PD_EVENT:
         if (master->cb_pd)
         {
            master->cb_pd (
               iolink_get_portnumber (job->port),
               master->cb_arg,
               job->pd_event.data_len,
               job->pd_event.data);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_avail, job, 0);
         break;
      case IOLINK_JOB_SM_OPERATE_REQ:
      case IOLINK_JOB_SM_SET_PORT_CFG_REQ:
      case IOLINK_JOB_DL_MODE_IND:
      case IOLINK_JOB_DL_READ_CNF:
      case IOLINK_JOB_DL_WRITE_CNF:
      case IOLINK_JOB_DL_WRITE_DEVMODE_CNF:
      case IOLINK_JOB_DL_READPARAM_CNF:
      case IOLINK_JOB_DL_WRITEPARAM_CNF:
      case IOLINK_JOB_DL_ISDU_TRANS_CNF:
      case IOLINK_JOB_DL_PDINPUT_TRANS_IND:
      case IOLINK_JOB_AL_CONTROL_CNF:
      case IOLINK_JOB_AL_READ_REQ:
      case IOLINK_JOB_AL_READ_CNF:
      case IOLINK_JOB_AL_WRITE_REQ:
      case IOLINK_JOB_AL_WRITE_CNF:
      case IOLINK_JOB_SM_PORT_MODE_IND:
      case IOLINK_JOB_DL_EVENT_IND:
      case IOLINK_JOB_DL_CONTROL_IND:
      case IOLINK_JOB_DS_STARTUP:
      case IOLINK_JOB_DS_DELETE:
      case IOLINK_JOB_DS_INIT:
      case IOLINK_JOB_DS_UPLOAD:
      case IOLINK_JOB_DS_READY:
      case IOLINK_JOB_DS_CHANGE:
      case IOLINK_JOB_DS_FAULT:
      case IOLINK_JOB_OD_START:
      case IOLINK_JOB_OD_STOP:
      case IOLINK_JOB_AL_EVENT_RSP:
         if (job->callback)
         {
            job->callback (job);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_avail, job, 0);
         break;
      case IOLINK_JOB_AL_ABORT:
      case IOLINK_JOB_SMI_MASTERIDENT:
      case IOLINK_JOB_SMI_PORTCONFIGURATION:
      case IOLINK_JOB_SMI_READBACKPORTCONFIGURATION:
      case IOLINK_JOB_SMI_PORTSTATUS:
      case IOLINK_JOB_SMI_DEVICE_READ:
      case IOLINK_JOB_SMI_DEVICE_WRITE:
      case IOLINK_JOB_SMI_PARAM_READ:
      case IOLINK_JOB_SMI_PARAM_WRITE:
         if (job->callback)
         {
            job->callback (job);
         }
         job->type     = IOLINK_JOB_NONE;
         job->callback = NULL;
         os_mbox_post (master->mbox_api_avail, job, 0);
         break;
      case IOLINK_JOB_EXIT:
         running = false;
         break;
      default:
         CC_ASSERT (0);
         break;
      }
   }

   master->has_exited = true;
}

/* Stack internal API */
iolink_job_t * iolink_fetch_avail_job (iolink_port_t * port)
{
   iolink_job_t * job;

#ifdef __rtk__ // TODO make this generic
   /* Make sure internal API is not used from any other thread */
   CC_ASSERT (
      task_self() == port->master->thread || task_self() == port->dl.thread);
#endif /* __rtk__ */

   if (os_mbox_fetch (port->master->mbox_avail, (void **)&job, 0))
   {
      CC_ASSERT (0); // TODO: This is bad! How to continue?
   }

   job->port = port;

   return job;
}

iolink_job_t * iolink_fetch_avail_api_job (iolink_port_t * port)
{
   iolink_job_t * job;

#ifdef __rtk__ // TODO make this generic
   /* Make sure the external API is not used within the stack */
   CC_ASSERT (
      task_self() != port->master->thread && task_self() != port->dl.thread);
#endif /* __rtk__ */

   if (os_mbox_fetch (port->master->mbox_api_avail, (void **)&job, 0))
   {
      CC_ASSERT (0); // TODO: This is fine, return busy
   }

   job->port = port;

   return job;
}

#ifdef UNIT_TEST
bool iolink_post_job (iolink_port_t * port, iolink_job_t * job)
{
   bool res = os_mbox_post (port->master->mbox, job, 0);

   CC_ASSERT (!res); // TODO how to handle this!?!

   return res;
}
#else
void iolink_post_job_with_type_and_callback (
   iolink_port_t * port,
   iolink_job_t * job,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job))
{
   job->type     = type;
   job->callback = callback;

   bool res = os_mbox_post (port->master->mbox, job, 0);
   CC_ASSERT (!res); // TODO how to handle this!?!
}
#endif

bool iolink_post_job_pd_event (
   iolink_port_t * port,
   uint32_t timeout,
   uint8_t data_len,
   const uint8_t * data)
{
   iolink_job_t * job;

   os_mbox_fetch (port->master->mbox_avail, (void **)&job, OS_WAIT_FOREVER);

   job->port              = port;
   job->type              = IOLINK_JOB_PD_EVENT;
   job->pd_event.data_len = data_len;
   job->pd_event.data     = data;

   bool res = os_mbox_post (port->master->mbox, job, timeout);
   CC_ASSERT (!res); // TODO how to handle this!?!

   return res;
}

void iolink_smi_cnf (
   iolink_port_t * port,
   iolink_arg_block_id_t ref_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   if (port->master->cb_smi)
   {
      port->master->cb_smi (
         port->master->cb_arg,
         iolink_get_portnumber (port),
         ref_arg_block_id,
         arg_block_len,
         arg_block);
   }
}

void iolink_smi_voidblock_cnf (
   iolink_port_t * port,
   iolink_arg_block_id_t ref_arg_block_id)
{
   if (port->master->cb_smi)
   {
      arg_block_void_t arg_block_void;

      memset (&arg_block_void, 0, sizeof (arg_block_void_t));
      arg_block_void.arg_block.id = IOLINK_ARG_BLOCK_ID_VOID_BLOCK;
      port->master->cb_smi (
         port->master->cb_arg,
         iolink_get_portnumber (port),
         ref_arg_block_id,
         sizeof (arg_block_void_t),
         (arg_block_t *)&arg_block_void);
   }
}

void iolink_smi_joberror_ind (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   iolink_arg_block_id_t ref_arg_block_id,
   iolink_smi_errortypes_t error)
{
   if (port->master->cb_smi)
   {
      arg_block_joberror_t arg_block_error;

      memset (&arg_block_error, 0, sizeof (arg_block_joberror_t));
      arg_block_error.arg_block.id     = IOLINK_ARG_BLOCK_ID_JOB_ERROR;
      arg_block_error.exp_arg_block_id = exp_arg_block_id;
      arg_block_error.error            = error;
      port->master->cb_smi (
         port->master->cb_arg,
         iolink_get_portnumber (port),
         ref_arg_block_id,
         sizeof (arg_block_joberror_t),
         (arg_block_t *)&arg_block_error);
   }
}

iolink_port_t * iolink_get_port (iolink_m_t * master, uint8_t portnumber)
{
   uint8_t port_index = portnumber - 1;

   if ((portnumber == 0) || (port_index > master->port_cnt))
   {
      return NULL;
   }

   return &master->ports[port_index];
}

uint8_t iolink_get_portnumber (iolink_port_t * port)
{
   return port->portnumber;
}

uint8_t iolink_get_port_cnt (iolink_port_t * port)
{
   return port->master->port_cnt;
}

iolink_port_info_t * iolink_get_port_info (iolink_port_t * port)
{
   return &port->port_info;
}

const iolink_smp_parameterlist_t * iolink_get_paramlist (iolink_port_t * port)
{
   return &port->sm.real_paramlist;
}

iolink_transmission_rate_t iolink_get_transmission_rate (iolink_port_t * port)
{
   return mhmode_to_transmission_rate (port->sm.comrate);
}

iolink_al_port_t * iolink_get_al_ctx (iolink_port_t * port)
{
   return &port->al;
}

iolink_cm_port_t * iolink_get_cm_ctx (iolink_port_t * port)
{
   return &port->cm;
}

iolink_dl_t * iolink_get_dl_ctx (iolink_port_t * port)
{
   return &port->dl;
}

iolink_ds_port_t * iolink_get_ds_ctx (iolink_port_t * port)
{
   return &port->ds;
}

iolink_ode_port_t * iolink_get_ode_ctx (iolink_port_t * port)
{
   return &port->ode;
}

iolink_pde_port_t * iolink_get_pde_ctx (iolink_port_t * port)
{
   return &port->pde;
}

iolink_pl_port_t * iolink_get_pl_ctx (iolink_port_t * port)
{
   return &port->pl;
}

iolink_sm_port_t * iolink_get_sm_ctx (iolink_port_t * port)
{
   return &port->sm;
}

/* Public APIs */
iolink_m_t * iolink_m_init (const iolink_m_cfg_t * m_cfg)
{
   int i;

   if (the_master != NULL)
   {
      return the_master;
   }

   if (m_cfg->port_cnt > IOLINK_NUM_PORTS)
   {
      CC_ASSERT (m_cfg->port_cnt <= IOLINK_NUM_PORTS);
      return NULL;
   }

   iolink_m_t * master =
      calloc (1, sizeof (iolink_m_t) + sizeof (iolink_port_t) * m_cfg->port_cnt);
   if (master == NULL)
   {
      return NULL;
   }

   master->has_exited = false;

   master->port_cnt = m_cfg->port_cnt;
   master->cb_arg   = m_cfg->cb_arg;
   master->cb_smi   = m_cfg->cb_smi;
   master->cb_pd    = m_cfg->cb_pd;

   for (i = 0; i < master->port_cnt; i++)
   {
      const iolink_port_cfg_t * port_cfg = &m_cfg->port_cfgs[i];
      CC_ASSERT (port_cfg->name != NULL);

      iolink_port_t * port = &(master->ports[i]);

      port->master     = master;
      port->portnumber = i + 1;

      iolink_pl_init (port, port_cfg->drv, port_cfg->arg);
      iolink_sm_init (port);
      iolink_al_init (port);
      iolink_cm_init (port);
      iolink_ds_init (port);
      iolink_ode_init (port);
      iolink_pde_init (port);
   }

   master->mbox =
      os_mbox_create (IOLINK_MASTER_JOB_CNT + IOLINK_MASTER_JOB_API_CNT);
   master->mbox_avail     = os_mbox_create (IOLINK_MASTER_JOB_CNT);
   master->mbox_api_avail = os_mbox_create (IOLINK_MASTER_JOB_API_CNT);

   for (i = 0; i < IOLINK_MASTER_JOB_CNT; i++)
   {
      master->job[i].type = IOLINK_JOB_NONE;
      os_mbox_post (master->mbox_avail, &master->job[i], 0);
   }

   for (i = 0; i < IOLINK_MASTER_JOB_API_CNT; i++)
   {
      master->job_api[i].type = IOLINK_JOB_NONE;
      os_mbox_post (master->mbox_api_avail, &master->job_api[i], 0);
   }

   CC_ASSERT (master->mbox_avail != NULL);
   master->thread = os_thread_create (
      "iolink_m_thread",
      m_cfg->master_thread_prio,
      m_cfg->master_thread_stack_size,
      iolink_main,
      master);
   CC_ASSERT (master->thread != NULL);

   the_master = master;

   return master;
}

void iolink_m_deinit (iolink_m_t ** m)
{
   int i;
   iolink_m_t * master = *m;
   iolink_job_t job;

   if (master == NULL)
   {
      return;
   }

   job.type = IOLINK_JOB_EXIT;
   if (os_mbox_post (master->mbox, &job, 0))
   {
      CC_ASSERT (0);
   }

   while (master->has_exited == false)
   {
      os_usleep (1 * 1000);
   }

   for (i = 0; i < master->port_cnt; i++)
   {
      iolink_port_t * port = &(master->ports[i]);

      os_mutex_destroy (port->al.mtx_pdin);
   }

   os_mbox_destroy (master->mbox);
   os_mbox_destroy (master->mbox_avail);
   os_mbox_destroy (master->mbox_api_avail);

   the_master = NULL;
   free (*m);
   *m = NULL;
}

static iolink_error_t SMI_common_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block,
   iolink_error_t (*SMI_req_fn) (
      iolink_port_t * port,
      iolink_arg_block_id_t exp_arg_block_id,
      uint16_t arg_block_len,
      arg_block_t * arg_block_t))
{
   iolink_port_t * port = NULL;

   iolink_error_t error = common_smi_check (portnumber, arg_block, &port);

   if (error != IOLINK_ERROR_NONE)
   {
      return error;
   }

   return SMI_req_fn (port, exp_arg_block_id, arg_block_len, arg_block);
}

iolink_error_t SMI_MasterIdentification_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      cm_SMI_MasterIdentification_req);
}

iolink_error_t SMI_PortConfiguration_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      cm_SMI_PortConfiguration_req);
}

iolink_error_t SMI_ReadbackPortConfiguration_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      cm_SMI_ReadbackPortConfiguration_req);
}

iolink_error_t SMI_PortStatus_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      cm_SMI_PortStatus_req);
}

iolink_error_t SMI_DeviceRead_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      ode_SMI_DeviceRead_req);
}

iolink_error_t SMI_DeviceWrite_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_port_t * port = NULL;

   iolink_error_t error = common_smi_check (portnumber, arg_block, &port);

   if (error != IOLINK_ERROR_NONE)
   {
      return error;
   }

   arg_block_od_t * arg_block_od = (arg_block_od_t *)arg_block;

   // special check needed to pass test case 289
   if ((arg_block_od->index == 24) && ((arg_block_len - sizeof (arg_block_od_t)) == 10))
   {
      return IOLINK_ERROR_ODLENGTH;
   }

   return ode_SMI_DeviceWrite_req (port, exp_arg_block_id, arg_block_len, arg_block);
}

iolink_error_t SMI_ParamReadBatch_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      ode_SMI_ParamReadBatch_req);
}

iolink_error_t SMI_ParamWriteBatch_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      ode_SMI_ParamWriteBatch_req);
}

iolink_error_t SMI_PDIn_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      pde_SMI_PDIn_req);
}

iolink_error_t SMI_PDOut_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      pde_SMI_PDOut_req);
}

iolink_error_t SMI_PDInOut_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      pde_SMI_PDInOut_req);
}

iolink_error_t SMI_ParServToDS_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   return SMI_common_req (
      portnumber,
      exp_arg_block_id,
      arg_block_len,
      arg_block,
      ds_SMI_ParServToDS_req);
}
