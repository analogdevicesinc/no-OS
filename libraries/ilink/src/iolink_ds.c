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
#define iolink_fetch_avail_job mock_iolink_fetch_avail_job
#define iolink_post_job        mock_iolink_post_job
#define DS_Ready               mock_DS_Ready
#define DS_Change              mock_DS_Change
#define DS_Fault               mock_DS_Fault
#define AL_Read_req            mock_AL_Read_req
#define AL_Write_req           mock_AL_Write_req
#endif /* UNIT_TEST */

#define DS_INDEX_LIST_MAX_ENTRIES 70

#define DS_PARAM_INDEX 3

#define DS_PARAM_SUBINDEX_CMD        1
#define DS_PARAM_SUBINDEX_STATE      2
#define DS_PARAM_SUBINDEX_SIZE       3
#define DS_PARAM_SUBINDEX_CHECKSUM   4
#define DS_PARAM_SUBINDEX_INDEX_LIST 5

#include "iolink_ds.h"
#include "iolink_cm.h" /* DS_Ready, DS_Change, DS_Fault */
#include "iolink_al.h" /* AL_Write_req */

#include "osal_log.h"

#include "iolink_types.h"
#include "iolink_main.h" /* iolink_fetch_avail_job, iolink_post_job, iolink_get_portnumber, iolink_get_port_info */

/**
 * @file
 * @brief Data storage layer
 *
 */

static const char * const iolink_ds_state_literals[] = {
   "CheckActivationState",
   "WaitingOnDSActivity",
   "Off",
   /* UploadeDownload_2 */
   "CheckIdentity",
   "CheckMemSize",
   "CheckUpload",
   "CheckDSValidity",
   "CheckChecksum",
   "DS_Ready",
   "DS_Fault",
   /* Upload_7 */
   "Decompose_IL",
   "ReadParameter",
   "StoreDataSet",
   "UploadFault",
   /* Download_10 */
   "Decompose_Set",
   "Write_Parameter",
   "Download_Done",
   "Download_Fault",
   "LAST",
};

static const char * const iolink_ds_event_literals[] = {
   "NONE",        "ENABLE", /* T1 */
   "STARTUP",               /* T2 or T14 */
   "READY",                 /* T25, T26 or T27; + T3 */
   "UPLOAD",                /* T4 or T13 */
   "FAULT_DONE",            /* T5 */
   "FAULT_ID",              /* T15 */
   "FAULT_SIZE",            /* T17 */
   "FAULT_UL",              /* T23 */
   "FAULT_DL",              /* T28 */
   "FAULT_LOCK",            /* T29 */
   "ENABLE_COM",            /* T6 */
   "CLR_DISA",              /* T7 */
   "ENABLE_NO_C",           /* T8 */
   "DELETE",                /* T9 or T11 */
   "CLEAR",                 /* T11 */
   "DISABLE",               /* T12 */
   "PASSED",                /* T16, T18 or T22 */
   "DO_UPLOAD",             /* T19 or T21 */
   "NO_UPLOAD",             /* T20 or T43 */
   "DOWNLOAD",              /* T24 */
   "MORE_DATA",             /* T30 or T37 */
   "READ_DONE",             /* T31 */
   "STORE_DATA",            /* T35 */
   "UL_DONE",               /* T26 */
   "DEV_ERR",               /* T32 or T39 */
   "COM_ERR",               /* T33, T34, T36, T40 or T42 */
   "WR_DONE",               /* T38 */
   "DL_DONE",               /* T41 */
   "INIT",                  /* Not in spec. */
   "LAST",
};

typedef struct iolink_fsm_ds_transition
{
   iolink_fsm_ds_event_t event;
   iolink_ds_state_t next_state;
   iolink_fsm_ds_event_t (
      *action) (iolink_port_t * port, iolink_fsm_ds_event_t event);
} iolink_fsm_ds_transition_t;

typedef struct iolink_fsm_ds_state_transitions
{
   const uint8_t number_of_trans;
   const iolink_fsm_ds_transition_t * transitions;
} iolink_fsm_ds_state_transitions_t;

typedef struct iolink_fsm_ds
{
   iolink_ds_state_t next;
   iolink_fsm_ds_event_t (
      *action) (iolink_port_t * port, iolink_fsm_ds_event_t event);
} iolink_fsm_ds_t;
/**
 * Trigger DS FSM state transition
 *
 * This function triggers a DS state transition according to the event.
 */
static void iolink_ds_event (iolink_port_t * port, iolink_fsm_ds_event_t event);

/* Actions taken when transitioning to a new state. See iolink_ds_event(). */
static iolink_fsm_ds_event_t ds_delete (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_todo (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_ul_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_dl_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_off (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_chk_id (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_chk_mem_size (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_chk_upload (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_upload (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_chk_validity (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_chk_csum (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_write_param (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_wait_ds_act (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_dl_done (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_readparam (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_store_data (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_ready (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_decompose_set (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_init_dl (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);
static iolink_fsm_ds_event_t ds_reinit (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event);

/* Callback functions to run on main thread */
static void ds_read_cnf_cb (iolink_job_t * job);
static void ds_write_cnf_cb (iolink_job_t * job);
static void ds_startup_cb (iolink_job_t * job);
static void ds_delete_cb (iolink_job_t * job);
static void ds_init_cb (iolink_job_t * job);
static void ds_upload_cb (iolink_job_t * job);

/* Other functions */
static bool ds_check_vid_did (iolink_port_t * port);
static void ds_AL_Read_cnf (
   iolink_port_t * port,
   uint8_t len,
   const uint8_t * data,
   iolink_smi_errortypes_t errortype);
static void ds_AL_Read_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex);
static void ds_AL_Write_ds_cmd_req (
   iolink_port_t * port,
   iolink_ds_command_t cmd);
static void ds_AL_Write_cnf (
   iolink_port_t * port,
   iolink_smi_errortypes_t errortype);

static void ds_AL_Read_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   ds->current_index    = index;
   ds->current_subindex = subindex;
   AL_Read_req (port, index, subindex, ds_AL_Read_cnf);
}

static void ds_AL_Write_ds_cmd_req (
   iolink_port_t * port,
   iolink_ds_command_t cmd)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   ds->current_index    = DS_PARAM_INDEX;
   ds->current_subindex = DS_PARAM_SUBINDEX_CMD;
   ds->command          = cmd;
   AL_Write_req (
      port,
      ds->current_index,
      ds->current_subindex,
      1,
      (uint8_t *)&ds->command,
      ds_AL_Write_cnf);
}

static iolink_fsm_ds_event_t ds_delete (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   ds->master_ds.vid   = 0;
   ds->master_ds.did   = 0;
   ds->master_ds.cs    = 0;
   ds->master_ds.valid = false;
   ds->master_ds.pos   = 0;

   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_todo (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   CC_ASSERT (0);
   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_ul_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   return DS_EVENT_FAULT_UL;
}

static iolink_fsm_ds_event_t ds_dl_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   return DS_EVENT_FAULT_DL;
}

static iolink_fsm_ds_event_t ds_fault (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);

   switch (event)
   {
   case DS_EVENT_FAULT_ID:
   case DS_EVENT_FAULT_SIZE:
      DS_Fault (
         port,
         (event == DS_EVENT_FAULT_ID) ? IOLINK_DS_FAULT_ID
                                      : IOLINK_DS_FAULT_SIZE);
      res = DS_EVENT_FAULT_DONE; /* T5 */
      break;
   case DS_EVENT_FAULT_UL:
   case DS_EVENT_FAULT_DL:
      ds->fault = (event == DS_EVENT_FAULT_UL) ? IOLINK_DS_FAULT_UP
                                               : IOLINK_DS_FAULT_DOWN;
      ds_AL_Write_ds_cmd_req (port, DS_CMD_BREAK); /* T5 */
      break;
   case DS_EVENT_FAULT_LOCK:
      ds->ds_enable = false;
      DS_Fault (port, IOLINK_DS_FAULT_LOCK);
      res = DS_EVENT_FAULT_DONE; /* T5 */
      break;
   default:
      break;
   }

   return res;
}

static iolink_fsm_ds_event_t ds_off (iolink_port_t * port, iolink_fsm_ds_event_t event)
{
   if (event == DS_EVENT_STARTUP) /* T14 */
   {
      DS_Ready (port);
   }
   else if (event == DS_EVENT_UPLOAD) /* T13 */
   {
      /* No need to confirm event DS_Upload, since we confirm all events
       * immediately in DL event handler */
   }

   return DS_EVENT_NONE;
}

static bool ds_check_vid_did (iolink_port_t * port)
{
   iolink_port_info_t * port_info = iolink_get_port_info (port);
   iolink_ds_port_t * ds          = iolink_get_ds_ctx (port);

   if (
      (ds->master_ds.vid == 0) || ((ds->master_ds.vid == port_info->vendorid) &&
                                   (ds->master_ds.did == port_info->deviceid)))
   {
      return true;
   }

   return false;
}

static iolink_fsm_ds_event_t ds_chk_id (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_fsm_ds_event_t res;

   if (event == DS_EVENT_UPLOAD) /* T4 */
   {
      /* No need to confirm event DS_Upload, since we confirm all events
       * immediately in DL event handler */
   }

   if (ds_check_vid_did (port))
   {
      ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_SIZE); /* T16 */
      res = DS_EVENT_NONE;
   }
   else
   {
      res = DS_EVENT_FAULT_ID; /* T15 */
   }

   return res;
}

static iolink_fsm_ds_event_t ds_chk_mem_size (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;

   if (ds->master_ds.size_max < ds->device_ds.size_max)
   {
      res = DS_EVENT_FAULT_SIZE; /* T17 */
   }
   else
   {
      ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_STATE); /* T18 */
   }

   return res;
}

static iolink_fsm_ds_event_t ds_chk_upload (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);
   bool upload_flag          = ds->device_ds.state_property &
                      DS_STATE_PROPERTY_UPLOAD_REQ;

   if ((ds->device_ds.state_property & DS_STATE_PROPERTY_STATE_MASK) == DS_STATE_PROPERTY_STATE_LOCKED)
   {
      res = DS_EVENT_FAULT_LOCK; /* T29 */
   }
   else if (upload_flag)
   {
      if (ds->ds_upload)
      {
         ds_AL_Write_ds_cmd_req (port, DS_CMD_UL_START); /* T19 */
      }
      else
      {
         /* T43 */
         /* Read checksum */
         ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_CHECKSUM);
      }
   }
   else
   {
      /* T20 */
      /* Read checksum */
      ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_CHECKSUM);
   }

   return res;
}

static iolink_fsm_ds_event_t ds_upload (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);

   if (event == DS_EVENT_DO_UPLOAD)
   {
      ds->master_ds.pos    = 0;
      ds->index_list.pos   = 0;
      ds->current_index    = DS_PARAM_INDEX;
      ds->current_subindex = DS_PARAM_SUBINDEX_INDEX_LIST;
      res                  = DS_EVENT_MORE_DATA; /* T30 */
   }
   else if (event == DS_EVENT_READ_DONE)
   {
      if (ds->index_list.pos < ds->index_list.count)
      {
         /* More data */
         ds_index_list_entry_t * il_entry =
            &ds->index_list.entries[ds->index_list.pos];

         ds->current_index    = il_entry->index;
         ds->current_subindex = il_entry->subindex;
         ds->index_list.pos++;
         res = DS_EVENT_MORE_DATA; /* T30 */
      }
      else
      {
         /* No more data */
         ds->master_ds.size = ds->master_ds.pos;
         /* T35 */
         /* Read checksum */
         ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_CHECKSUM);
         res = DS_EVENT_STORE_DATA;
      }
   }

   return res;
}

static iolink_fsm_ds_event_t ds_chk_validity (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;

   if (ds->master_ds.valid)
   {
      res = DS_EVENT_PASSED; /* T22 */
   }
   else
   {
      ds_AL_Write_ds_cmd_req (port, DS_CMD_UL_START); /* T21 */
   }

   return res;
}

static iolink_fsm_ds_event_t ds_chk_csum (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_fsm_ds_event_t res;
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   if ((ds->ds_download) && (ds->master_ds.cs != ds->device_ds.cs))
   {
      res = DS_EVENT_DOWNLOAD; /* T24 */
   }
   else
   {
      res = DS_EVENT_READY; /* T25 */
   }

   return res;
}

static iolink_fsm_ds_event_t ds_write_param (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   uint16_t index = ds->master_ds.data[ds->master_ds.pos] << 8;
   ds->master_ds.pos++;
   index += ds->master_ds.data[ds->master_ds.pos];
   ds->master_ds.pos++;
   uint8_t subindex = ds->master_ds.data[ds->master_ds.pos];
   ds->master_ds.pos++;
   uint8_t len = ds->master_ds.data[ds->master_ds.pos];
   ds->master_ds.pos++;
   uint8_t * data = &ds->master_ds.data[ds->master_ds.pos];
   ds->master_ds.pos += len;

   ds->current_index    = DS_PARAM_INDEX;
   ds->current_subindex = subindex;

   AL_Write_req (port, index, subindex, len, data, ds_AL_Write_cnf);

   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_wait_ds_act (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_dl_done (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   ds_AL_Write_ds_cmd_req (port, DS_CMD_DL_END); /* T41 */

   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_readparam (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   ds_AL_Read_req (port, ds->current_index, ds->current_subindex);

   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_store_data (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   /* Wait for Read Parameter Checksum */
   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_ready (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   DS_Ready (port);

   return DS_EVENT_READY;
}

static iolink_fsm_ds_event_t ds_decompose_set (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds     = iolink_get_ds_ctx (port);
   iolink_fsm_ds_event_t res = DS_EVENT_NONE;

   if (event == DS_EVENT_DOWNLOAD)
   {
      ds->master_ds.pos = 0;
   }

   if (ds->master_ds.pos >= ds->master_ds.size)
   {
      res = DS_EVENT_DL_DONE;
   }
   else
   {
      res = DS_EVENT_MORE_DATA;
   }

   return res;
}

static iolink_fsm_ds_event_t ds_init_dl (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   ds_AL_Write_ds_cmd_req (port, DS_CMD_DL_START); /* T24 */

   return DS_EVENT_NONE;
}

static iolink_fsm_ds_event_t ds_reinit (
   iolink_port_t * port,
   iolink_fsm_ds_event_t event)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   if (ds->ds_enable)
   {
      return DS_EVENT_ENABLE; /* T1 */
   }
   else
   {
      return DS_EVENT_CLR_DISA; /* T7 */
   }
}

/* DS FSM state transitions, IO-Link Interface Spec v1.1.3 Chapter 11.4.4 */
/* since we iterate through the list on events put the most likely in the top of
 * the list. */
static const iolink_fsm_ds_transition_t ds_trans_s0[] = {
   /* CheckActivationState_0 */
   {DS_EVENT_ENABLE, DS_STATE_WaitingOnDSActivity, ds_wait_ds_act}, /* T1 */
   {DS_EVENT_CLR_DISA, DS_STATE_Off, ds_off},                       /* T7 */

   {DS_EVENT_DELETE, DS_STATE_CheckActivationState, ds_delete}, /* Not in spec.
                                                                 */
   {DS_EVENT_INIT, DS_STATE_CheckActivationState, ds_reinit}, /* Not in spec. */
};
static const iolink_fsm_ds_transition_t ds_trans_s1[] = {
   /* WaitingOnDsActivity_1 */
   {DS_EVENT_STARTUP, DS_STATE_CheckIdentity, ds_chk_id},      /* T2 */
   {DS_EVENT_UPLOAD, DS_STATE_CheckIdentity, ds_chk_id},       /* T4 */
   {DS_EVENT_DELETE, DS_STATE_WaitingOnDSActivity, ds_delete}, /* T9 */
   {DS_EVENT_CLEAR, DS_STATE_Off, ds_todo},                    /* T11 */
   {DS_EVENT_DISABLE, DS_STATE_Off, ds_todo},                  /* T12 */

   {DS_EVENT_INIT, DS_STATE_CheckActivationState, ds_reinit}, /* Not in spec. */
};
static const iolink_fsm_ds_transition_t ds_trans_s2[] = {
   /* Off_3 */
   {DS_EVENT_ENABLE_COM, DS_STATE_CheckIdentity, ds_todo},        /* T6 */
   {DS_EVENT_ENABLE_NO_C, DS_STATE_WaitingOnDSActivity, ds_todo}, /* T8 */
   {DS_EVENT_DELETE, DS_STATE_Off, ds_delete},                    /* T10 */
   {DS_EVENT_UPLOAD, DS_STATE_Off, ds_off},                       /* T13 */
   {DS_EVENT_STARTUP, DS_STATE_Off, ds_off},                      /* T14 */

   {DS_EVENT_INIT, DS_STATE_CheckActivationState, ds_reinit}, /* Not in spec. */
};
static const iolink_fsm_ds_transition_t ds_trans_s3[] = {
   /* CheckIdentity_4 */
   {DS_EVENT_PASSED, DS_STATE_CheckMemSize, ds_chk_mem_size}, /* T16 */
   {DS_EVENT_FAULT_ID, DS_STATE_DS_Fault, ds_fault},          /* T15 */

   {DS_EVENT_INIT, DS_STATE_CheckActivationState, ds_reinit}, /* Not in spec. */
};
static const iolink_fsm_ds_transition_t ds_trans_s4[] = {
   /* CheckMemSize_5 */
   {DS_EVENT_PASSED, DS_STATE_CheckUpload, ds_chk_upload}, /* T18 */
   {DS_EVENT_FAULT_SIZE, DS_STATE_DS_Fault, ds_fault},     /* T17 */

   {DS_EVENT_INIT, DS_STATE_CheckActivationState, ds_reinit}, /* Not in spec. */
};
static const iolink_fsm_ds_transition_t ds_trans_s5[] = {
   /* CheckUpload_6 */
   {DS_EVENT_DO_UPLOAD, DS_STATE_Decompose_IL, ds_upload},          /* T19 */
   {DS_EVENT_NO_UPLOAD, DS_STATE_CheckDSValidity, ds_chk_validity}, /* T20 or
                                                                       T43 */
   {DS_EVENT_FAULT_LOCK, DS_STATE_DS_Fault, ds_fault},              /* T29 */
};
static const iolink_fsm_ds_transition_t ds_trans_s6[] = {
   /* CheckDSValidity_8 */
   {DS_EVENT_DO_UPLOAD, DS_STATE_Decompose_IL, ds_upload}, /* T21 */
   {DS_EVENT_PASSED, DS_STATE_CheckChecksum, ds_chk_csum}, /* T22 */
};
static const iolink_fsm_ds_transition_t ds_trans_s7[] = {
   /* CheckChecksum_9 */
   {DS_EVENT_DOWNLOAD, DS_STATE_Decompose_Set, ds_init_dl}, /* T24 */
   {DS_EVENT_READY, DS_STATE_DS_Ready, ds_ready},           /* T25 + T3 */
};
static const iolink_fsm_ds_transition_t ds_trans_s8[] = {
   /* DS_Ready_11 */
   {DS_EVENT_READY, DS_STATE_WaitingOnDSActivity, ds_wait_ds_act}, /* T3 */
};
static const iolink_fsm_ds_transition_t ds_trans_s9[] = {
   /* DS_Fault_12 */
   {DS_EVENT_FAULT_DONE, DS_STATE_WaitingOnDSActivity, ds_wait_ds_act}, /* T5 */
   {DS_EVENT_COM_ERR, DS_STATE_WaitingOnDSActivity, ds_wait_ds_act},
   /* Not in spec. */ // TODO need to call DS_Fault()?
};
static const iolink_fsm_ds_transition_t ds_trans_s10[] = {
   /* Upload_7 + Decompose_IL_13 */
   {DS_EVENT_MORE_DATA, DS_STATE_ReadParameter, ds_readparam},  /* T30 */
   {DS_EVENT_STORE_DATA, DS_STATE_StoreDataSet, ds_store_data}, /* T35 */
   {DS_EVENT_COM_ERR, DS_STATE_UploadFault, ds_ul_fault},       /* T34 */

};
static const iolink_fsm_ds_transition_t ds_trans_s11[] = {
   /* Upload_7 + ReadParameter_14 */
   {DS_EVENT_READ_DONE, DS_STATE_Decompose_IL, ds_upload}, /* T31 */
   {DS_EVENT_DEV_ERR, DS_STATE_UploadFault, ds_ul_fault},  /* T32 */
   {DS_EVENT_COM_ERR, DS_STATE_UploadFault, ds_ul_fault},  /* T33 */
};
static const iolink_fsm_ds_transition_t ds_trans_s12[] = {
   /* Upload_7 + StoreDataSet_15 */
   {DS_EVENT_UL_DONE, DS_STATE_DS_Ready, ds_ready},       /* T26 + T3 */
   {DS_EVENT_COM_ERR, DS_STATE_UploadFault, ds_ul_fault}, /* T36 */
};
static const iolink_fsm_ds_transition_t ds_trans_s13[] = {
   /* Upload_7 + Upload_Fault_16 */
   {DS_EVENT_FAULT_UL, DS_STATE_DS_Fault, ds_fault}, /* T23 */
};
static const iolink_fsm_ds_transition_t ds_trans_s14[] = {
   /* Download_10 + Decompose_Set_17 */
   {DS_EVENT_MORE_DATA, DS_STATE_Write_Parameter, ds_write_param}, /* T37 */
   {DS_EVENT_DOWNLOAD, DS_STATE_Decompose_Set, ds_decompose_set},  /* T24 */
   {DS_EVENT_DL_DONE, DS_STATE_Download_Done, ds_dl_done},         /* T41 */
};
static const iolink_fsm_ds_transition_t ds_trans_s15[] = {
   /* Download_10 + Write_Parameter_18 */
   {DS_EVENT_WR_DONE, DS_STATE_Decompose_Set, ds_decompose_set}, /* T38 */
   {DS_EVENT_DEV_ERR, DS_STATE_Download_Fault, ds_dl_fault},     /* T39 */
   {DS_EVENT_COM_ERR, DS_STATE_Download_Fault, ds_dl_fault},     /* T40 */
};
static const iolink_fsm_ds_transition_t ds_trans_s16[] = {
   /* Download_10 + Download_Done_19 */
   {DS_EVENT_READY, DS_STATE_DS_Ready, ds_ready},            /* T27 + T3 */
   {DS_EVENT_COM_ERR, DS_STATE_Download_Fault, ds_dl_fault}, /* T42 */
};
static const iolink_fsm_ds_transition_t ds_trans_s17[] = {
   /* Download_10 + Download_Fault_20 */
   {DS_EVENT_FAULT_DL, DS_STATE_DS_Fault, ds_fault}, /* T28 */
};

/* The index is the state in this array */
static const iolink_fsm_ds_state_transitions_t iolink_ds_fsm[] = {
   {/* CheckActivationState_0 */
    .number_of_trans = NELEMENTS (ds_trans_s0),
    .transitions     = ds_trans_s0},
   {/* WaitingOnDsActivity_1 */
    .number_of_trans = NELEMENTS (ds_trans_s1),
    .transitions     = ds_trans_s1},
   {/* Off_3 */
    .number_of_trans = NELEMENTS (ds_trans_s2),
    .transitions     = ds_trans_s2},
   {/* CheckIdentity_4 */
    .number_of_trans = NELEMENTS (ds_trans_s3),
    .transitions     = ds_trans_s3},
   {/* CheckMemSize_5 */
    .number_of_trans = NELEMENTS (ds_trans_s4),
    .transitions     = ds_trans_s4},
   {/* CheckUpload_6 */
    .number_of_trans = NELEMENTS (ds_trans_s5),
    .transitions     = ds_trans_s5},
   {/* CheckDSValidity_8 */
    .number_of_trans = NELEMENTS (ds_trans_s6),
    .transitions     = ds_trans_s6},
   {/* CheckChecksum_9 */
    .number_of_trans = NELEMENTS (ds_trans_s7),
    .transitions     = ds_trans_s7},
   {/* DS_Ready_11 */
    .number_of_trans = NELEMENTS (ds_trans_s8),
    .transitions     = ds_trans_s8},
   {/* DS_Fault_12 */
    .number_of_trans = NELEMENTS (ds_trans_s9),
    .transitions     = ds_trans_s9},
   {/* Upload_7 + Decompose_IL_13 */
    .number_of_trans = NELEMENTS (ds_trans_s10),
    .transitions     = ds_trans_s10},
   {/* Upload_7 + ReadParameter_14 */
    .number_of_trans = NELEMENTS (ds_trans_s11),
    .transitions     = ds_trans_s11},
   {/* Upload_7 + StoreDataSet_15 */
    .number_of_trans = NELEMENTS (ds_trans_s12),
    .transitions     = ds_trans_s12},
   {/* Upload_7 + Upload_Fault_16 */
    .number_of_trans = NELEMENTS (ds_trans_s13),
    .transitions     = ds_trans_s13},
   {/* Download_10 + Decompose_Set_17 */
    .number_of_trans = NELEMENTS (ds_trans_s14),
    .transitions     = ds_trans_s14},
   {/* Download_10 + Write_Parameter_18 */
    .number_of_trans = NELEMENTS (ds_trans_s15),
    .transitions     = ds_trans_s15},
   {/* Download_10 + Download_Done_19 */
    .number_of_trans = NELEMENTS (ds_trans_s16),
    .transitions     = ds_trans_s16},
   {/* Download_10 + Download_Fault_20 */
    .number_of_trans = NELEMENTS (ds_trans_s17),
    .transitions     = ds_trans_s17},
};

static void iolink_ds_event (iolink_port_t * port, iolink_fsm_ds_event_t event)
{
   do
   {
      int i;
      iolink_ds_port_t * ds                         = iolink_get_ds_ctx (port);
      iolink_ds_state_t previous                    = ds->state;
      const iolink_fsm_ds_transition_t * next_trans = NULL;

      for (i = 0; i < iolink_ds_fsm[previous].number_of_trans; i++)
      {
         if (iolink_ds_fsm[previous].transitions[i].event == event)
         {
            if (!next_trans)
            {
               next_trans = &iolink_ds_fsm[previous].transitions[i];
               break;
            }
         }
      }
      if (!next_trans)
      {
         LOG_ERROR (
            IOLINK_DS_LOG,
            "%u: next_trans == NULL: DS state %s - DS event %s\n",
            iolink_get_portnumber (port),
            iolink_ds_state_literals[previous],
            iolink_ds_event_literals[event]);
         return;
      }

      /* Transition to next state */
      ds->state = next_trans->next_state;
      LOG_DEBUG (
         IOLINK_DS_LOG,
         "DS (%u): event: %s, state transition: %s -> %s\n",
         iolink_get_portnumber (port),
         iolink_ds_event_literals[event],
         iolink_ds_state_literals[previous],
         iolink_ds_state_literals[ds->state]);
      event = next_trans->action (port, event);
   } while (event != DS_EVENT_NONE);
}

/* DS AL_Read_cnf() and AL_Write_cnf functions */
static void ds_read_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   if (job->al_read_cnf.errortype == IOLINK_SMI_ERRORTYPE_COM_ERR)
   {
      iolink_ds_event (port, DS_EVENT_COM_ERR);

      return;
   }

   switch (ds->state)
   {
   case DS_STATE_CheckIdentity:
      if (
         job->al_read_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE ||
         job->al_read_cnf.data_len < sizeof (uint32_t))
      {
         iolink_ds_event (port, DS_EVENT_FAULT_ID); /* T15 */
      }
      else
      {
         ds->device_ds.size_max =
            (job->al_read_cnf.data[0] << 24) + (job->al_read_cnf.data[1] << 16) +
            (job->al_read_cnf.data[2] << 8) + job->al_read_cnf.data[3];
         if (ds->device_ds.size_max > IOLINK_DS_MAX_SIZE)
         {
            LOG_DEBUG (
               IOLINK_DS_LOG,
               "%u: DS: Maximum size is %u, got %lu\n",
               iolink_get_portnumber (port),
               IOLINK_DS_MAX_SIZE,
               (long unsigned int)ds->device_ds.size_max);
         }
         iolink_ds_event (port, DS_EVENT_PASSED);
      }
      break;
   case DS_STATE_CheckMemSize:
      if (
         job->al_read_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE ||
         job->al_read_cnf.data_len != sizeof (uint8_t))
      {
         iolink_ds_event (port, DS_EVENT_FAULT_SIZE); /* T17 */
      }
      else
      {
         ds->device_ds.state_property = job->al_read_cnf.data[0];
         iolink_ds_event (port, DS_EVENT_PASSED);
      }
      break;
   case DS_STATE_ReadParameter:
      if (job->al_read_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE)
      {
         iolink_ds_event (port, DS_EVENT_DEV_ERR); /* T32 */
      }
      else
      {
         if (
            (ds->current_index == DS_PARAM_INDEX) &&
            (ds->current_subindex == DS_PARAM_SUBINDEX_INDEX_LIST))
         {
            uint8_t i              = 0;
            uint16_t read_data_pos = 0;

            while (i < DS_INDEX_LIST_MAX_ENTRIES)
            {
               uint16_t index = job->al_read_cnf.data[read_data_pos] << 8;
               read_data_pos++;
               index += job->al_read_cnf.data[read_data_pos];
               read_data_pos++;

               if (index == 0) /* Found termination entry */
               {
                  ds->index_list.count = i;
                  break;
               }
               else
               {
                  uint8_t subindex = job->al_read_cnf.data[read_data_pos];
                  read_data_pos++;

                  ds->index_list.entries[i].index    = index;
                  ds->index_list.entries[i].subindex = subindex;
               }
               i++;
            }
         }
         else
         {
            uint16_t len = job->al_read_cnf.data_len;

            if ((ds->master_ds.pos + 2 + 1 + len) < IOLINK_DS_MAX_SIZE)
            {
               ds->master_ds.data[ds->master_ds.pos] = ds->current_index >> 8;
               ds->master_ds.pos++;
               ds->master_ds.data[ds->master_ds.pos] = ds->current_index;
               ds->master_ds.pos++;
               ds->master_ds.data[ds->master_ds.pos] = ds->current_subindex;
               ds->master_ds.pos++;
               ds->master_ds.data[ds->master_ds.pos] = len;
               ds->master_ds.pos++;
               memcpy (
                  &ds->master_ds.data[ds->master_ds.pos],
                  job->al_read_cnf.data,
                  len);
               ds->master_ds.pos += len;
            }
            else
            {
               LOG_DEBUG (
                  IOLINK_DS_LOG,
                  "%u: DS: not enough space in data store, max_size = %lu, pos "
                  "= %u, len = %u\n",
                  iolink_get_portnumber (port),
                  (unsigned long)ds->master_ds.size_max,
                  ds->master_ds.pos,
                  len);
            }
         }
         iolink_ds_event (port, DS_EVENT_READ_DONE);
      }
      break;
   case DS_STATE_CheckUpload:
      /* Checksum */
      if (job->al_read_cnf.data_len < sizeof (uint32_t))
      {
         iolink_ds_event (port, DS_EVENT_COM_ERR); /* T36 */
      }
      ds->device_ds.cs =
         (job->al_read_cnf.data[0] << 24) + (job->al_read_cnf.data[1] << 16) +
         (job->al_read_cnf.data[2] << 8) + job->al_read_cnf.data[3];

      iolink_ds_event (port, DS_EVENT_NO_UPLOAD); /* T20 or T43 */
      break;
   case DS_STATE_StoreDataSet:
      /* Checksum */
      if (job->al_read_cnf.data_len < sizeof (uint32_t))
      {
         iolink_ds_event (port, DS_EVENT_COM_ERR); /* T36 */
      }
      ds->device_ds.cs =
         (job->al_read_cnf.data[0] << 24) + (job->al_read_cnf.data[1] << 16) +
         (job->al_read_cnf.data[2] << 8) + job->al_read_cnf.data[3];
      ds_AL_Write_ds_cmd_req (port, DS_CMD_UL_END); /* T26 */
      break;
   case DS_STATE_Download_Done:
      /* Checksum */
      if (job->al_read_cnf.data_len < sizeof (uint32_t))
      {
         iolink_ds_event (port, DS_EVENT_COM_ERR); /* T42 */
      }
      ds->device_ds.cs =
         (job->al_read_cnf.data[0] << 24) + (job->al_read_cnf.data[1] << 16) +
         (job->al_read_cnf.data[2] << 8) + job->al_read_cnf.data[3];

      iolink_ds_event (port, DS_EVENT_READY); /* T41 */
      break;
   default:
      break;
   }
}

static void ds_write_cnf_cb (iolink_job_t * job)
{
   iolink_port_t * port  = job->port;
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   if (job->al_write_cnf.errortype != IOLINK_SMI_ERRORTYPE_NONE)
   {
      iolink_ds_event (port, DS_EVENT_COM_ERR);
   }
   else
   {
      switch (ds->state)
      {
      case DS_STATE_CheckUpload:                     /* T19 */
      case DS_STATE_CheckDSValidity:                 /* T21 */
         iolink_ds_event (port, DS_EVENT_DO_UPLOAD); /* T19 */
         break;
      case DS_STATE_Write_Parameter:
         iolink_ds_event (port, DS_EVENT_WR_DONE);
         break;
      case DS_STATE_StoreDataSet:
         ds->master_ds.vid   = ds->pending_id.vendorid;
         ds->master_ds.did   = ds->pending_id.deviceid;
         ds->master_ds.cs    = ds->device_ds.cs;
         ds->master_ds.valid = true;
         iolink_ds_event (port, DS_EVENT_UL_DONE); /* T26 */
         break;
      case DS_STATE_Decompose_Set:
         iolink_ds_event (port, DS_EVENT_DOWNLOAD); /* T24 */
         break;
      case DS_STATE_Download_Done:
         /* T41 */
         ds_AL_Read_req (port, DS_PARAM_INDEX, DS_PARAM_SUBINDEX_CHECKSUM);
         break;
      case DS_STATE_DS_Fault:
         DS_Fault (port, ds->fault);
         iolink_ds_event (port, DS_EVENT_FAULT_DONE); /* T5 */
         break;
      default:
         break;
      }
   }
}

static void ds_AL_Read_cnf (
   iolink_port_t * port,
   uint8_t len,
   const uint8_t * data,
   iolink_smi_errortypes_t errortype)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   job->al_read_cnf.data      = data;
   job->al_read_cnf.data_len  = len;
   job->al_read_cnf.errortype = errortype;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_READ_CNF,
      ds_read_cnf_cb);
}

static void ds_AL_Write_cnf (iolink_port_t * port, iolink_smi_errortypes_t errortype)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   job->al_write_cnf.errortype = errortype;

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_AL_WRITE_CNF,
      ds_write_cnf_cb);
}

/* DS job callback functions */
static void ds_startup_cb (iolink_job_t * job)
{
   iolink_ds_event (job->port, DS_EVENT_STARTUP);
}

static void ds_delete_cb (iolink_job_t * job)
{
   iolink_ds_event (job->port, DS_EVENT_DELETE);
}

static void ds_init_cb (iolink_job_t * job)
{
   iolink_port_t * port        = job->port;
   iolink_ds_port_t * ds       = iolink_get_ds_ctx (port);
   portconfiglist_t * cfg_list = &job->ds_init.portcfg;

   ds->ds_upload   = false;
   ds->ds_download = false;
   ds->fault       = IOLINK_DS_FAULT_NONE;

   if (cfg_list->portmode == IOLINK_PORTMODE_IOL_MAN)
   {
      switch (cfg_list->validation_backup)
      {
      case IOLINK_VALIDATION_CHECK_V11_BAK_RESTORE: /* upload and download */
         ds->ds_upload = true;
         /* fall through */
      case IOLINK_VALIDATION_CHECK_V11_RESTORE: /* only download */
         ds->ds_download = true;
         ds->ds_enable   = true;
         break;
      case IOLINK_VALIDATION_CHECK_V11:
         break;
      default:
         break;
      }
   }

   ds->pending_id.vendorid = cfg_list->vendorid;
   ds->pending_id.deviceid = cfg_list->deviceid;

   iolink_ds_event (port, DS_EVENT_INIT);
}

static void ds_upload_cb (iolink_job_t * job)
{
   iolink_ds_event (job->port, DS_EVENT_UPLOAD);
}

/* Stack internal API */
void iolink_ds_init (iolink_port_t * port)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   memset (ds, 0, sizeof (iolink_ds_port_t));

   ds->state = DS_STATE_CheckActivationState;

   ds->master_ds.size_max = IOLINK_DS_MAX_SIZE;
}

iolink_error_t DS_Delete (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_DELETE,
      ds_delete_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DS_Startup (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_STARTUP,
      ds_startup_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DS_Upload (iolink_port_t * port)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_UPLOAD,
      ds_upload_cb);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DS_Init (iolink_port_t * port, const portconfiglist_t * cfg_list)
{
   iolink_job_t * job = iolink_fetch_avail_job (port);

   memcpy (&job->ds_init.portcfg, cfg_list, sizeof (portconfiglist_t));

   iolink_post_job_with_type_and_callback (
      port,
      job,
      IOLINK_JOB_DS_STARTUP,
      ds_init_cb);

   return IOLINK_ERROR_NONE;
}

bool DS_Chk_Cfg (iolink_port_t * port, const portconfiglist_t * cfg_list)
{
   iolink_ds_port_t * ds = iolink_get_ds_ctx (port);

   if (
      (ds->master_ds.vid) && (ds->master_ds.vid == cfg_list->vendorid) &&
      (ds->master_ds.did == cfg_list->deviceid))
   {
      return true;
   }

   return false;
}

iolink_error_t ds_SMI_ParServToDS_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block)
{
   iolink_arg_block_id_t ref_arg_block_id = arg_block->id;
   iolink_error_t error;
   iolink_ds_port_t * ds         = iolink_get_ds_ctx (port);
   uint8_t arg_block_ds_data_len = arg_block_len - sizeof (arg_block_ds_data_t);
   iolink_smi_errortypes_t smi_error;

   if (exp_arg_block_id != IOLINK_ARG_BLOCK_ID_VOID_BLOCK || ref_arg_block_id != IOLINK_ARG_BLOCK_ID_DS_DATA)
   {
      smi_error = IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED; // TODO what to
                                                               // use?
      iolink_smi_joberror_ind (port, exp_arg_block_id, ref_arg_block_id, smi_error);
      error = IOLINK_ERROR_PARAMETER_CONFLICT;
   }
   else if (arg_block_ds_data_len > ds->master_ds.size_max)
   {
      smi_error = IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT; // TODO what to
                                                              // use?
      iolink_smi_joberror_ind (port, exp_arg_block_id, ref_arg_block_id, smi_error);
      error = IOLINK_ERROR_PARAMETER_CONFLICT;
   }
   else
   {
      arg_block_ds_data_t * arg_block_ds_data = (arg_block_ds_data_t *)arg_block;

      memcpy (ds->master_ds.data, arg_block_ds_data->ds_data, arg_block_ds_data_len);

      ds->master_ds.cs    = 0; // TODO?
      ds->master_ds.vid   = arg_block_ds_data->vid;
      ds->master_ds.did   = arg_block_ds_data->did;
      ds->master_ds.fid   = arg_block_ds_data->fid;
      ds->master_ds.valid = true;
      ds->master_ds.size  = arg_block_ds_data_len;

      iolink_smi_voidblock_cnf (port, ref_arg_block_id);

      error = IOLINK_ERROR_NONE;
   }

   return error;
}
