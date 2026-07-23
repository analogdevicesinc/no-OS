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

/**
 * @file
 * @brief Data storage layer
 *
 */

#ifndef IOLINK_DS_H
#define IOLINK_DS_H

#include "iolink_types.h"
#include "iolink_main.h"

#include "sys/osal_cc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IOLINK_DS_MAX_SIZE 2048

#define DS_STATE_PROPERTY_STATE_MASK     (3 << 1)
#define DS_STATE_PROPERTY_STATE_UPLOAD   (1 << 1)
#define DS_STATE_PROPERTY_STATE_DOWNLOAD (2 << 1)
#define DS_STATE_PROPERTY_STATE_LOCKED   (3 << 1)
#define DS_STATE_PROPERTY_UPLOAD_REQ     BIT (7)

typedef enum iolink_ds_command
{
   DS_CMD_RES      = 0x00,
   DS_CMD_UL_START = 0x01,
   DS_CMD_UL_END   = 0x02,
   DS_CMD_DL_START = 0x03,
   DS_CMD_DL_END   = 0x04,
   DS_CMD_BREAK    = 0x05,
} iolink_ds_command_t;

typedef enum iolink_ds_state
{
   DS_STATE_CheckActivationState = 0,
   DS_STATE_WaitingOnDSActivity,
   DS_STATE_Off,
   /* UploadeDownload_2 */
   DS_STATE_CheckIdentity,
   DS_STATE_CheckMemSize,
   DS_STATE_CheckUpload,
   DS_STATE_CheckDSValidity,
   DS_STATE_CheckChecksum,
   DS_STATE_DS_Ready,
   DS_STATE_DS_Fault,
   /* Upload_7 */
   DS_STATE_Decompose_IL,
   DS_STATE_ReadParameter,
   DS_STATE_StoreDataSet,
   DS_STATE_UploadFault,
   /* Download_10 */
   DS_STATE_Decompose_Set,
   DS_STATE_Write_Parameter,
   DS_STATE_Download_Done,
   DS_STATE_Download_Fault,
   DS_STATE_LAST,
} iolink_ds_state_t;

CC_PACKED_BEGIN
typedef struct CC_PACKED ds_index_list_entry
{
   uint16_t index;
   uint8_t subindex;
} ds_index_list_entry_t;
CC_PACKED_END

typedef struct iolink_ds_port
{
   iolink_ds_state_t state;
   iolink_ds_fault_t fault;
   bool ds_enable;
   bool ds_upload;
   bool ds_download;

   struct
   {
      uint32_t cs;
      uint32_t size;
      uint32_t size_max;
      uint8_t state_property;
   } device_ds;
   struct
   {
      uint32_t cs;
      uint16_t vid;
      uint32_t did;
      uint16_t fid;
      bool valid;
      uint32_t size;
      uint32_t size_max;

      uint16_t pos; /* Position in data */
      uint8_t data[IOLINK_DS_MAX_SIZE];
   } master_ds;
   struct
   {
      uint8_t count;
      uint8_t pos; /* Position in entries */
      ds_index_list_entry_t entries[70];
   } index_list;

   iolink_ds_command_t command;
   uint16_t current_index;
   uint8_t current_subindex;

   struct
   {
      uint16_t vendorid;
      uint32_t deviceid;
   } pending_id;
} iolink_ds_port_t;

/**
 * DS state-machine events.
 *
 * DS events trigger state transitions in the DS FSM.
 */
typedef enum iolink_fsm_ds_event
{
   DS_EVENT_NONE,
   DS_EVENT_ENABLE,      /* T1 */
   DS_EVENT_STARTUP,     /* T2 or T14 */
   DS_EVENT_READY,       /* T25, T26 or T27; + T3 */
   DS_EVENT_UPLOAD,      /* T4 or T13 */
   DS_EVENT_FAULT_DONE,  /* T5 */
   DS_EVENT_FAULT_ID,    /* T15 */
   DS_EVENT_FAULT_SIZE,  /* T17 */
   DS_EVENT_FAULT_UL,    /* T23 */
   DS_EVENT_FAULT_DL,    /* T28 */
   DS_EVENT_FAULT_LOCK,  /* T29 */
   DS_EVENT_ENABLE_COM,  /* T6 */
   DS_EVENT_CLR_DISA,    /* T7 */
   DS_EVENT_ENABLE_NO_C, /* T8 */
   DS_EVENT_DELETE,      /* T9 + T10 */
   DS_EVENT_CLEAR,       /* T11 */
   DS_EVENT_DISABLE,     /* T12 */
   DS_EVENT_PASSED,      /* T16, T18 or T22 */
   DS_EVENT_DO_UPLOAD,   /* T19 or T21 */
   DS_EVENT_NO_UPLOAD,   /* T20 or T43 */
   DS_EVENT_DOWNLOAD,    /* T24 */

   DS_EVENT_MORE_DATA,  /* T30 or T37 */
   DS_EVENT_READ_DONE,  /* T31 */
   DS_EVENT_STORE_DATA, /* T35 */
   DS_EVENT_UL_DONE,    /* T26 */
   DS_EVENT_DEV_ERR,    /* T32 or T39 */
   DS_EVENT_COM_ERR,    /* T33, T34, T36, T40 or T42 */

   DS_EVENT_WR_DONE, /* T38 */
   DS_EVENT_DL_DONE, /* T41 */
   DS_EVENT_INIT,    /* Not in spec. */
   DS_EVENT_LAST,
} iolink_fsm_ds_event_t;

void iolink_ds_init (iolink_port_t * port);

iolink_error_t DS_Delete (iolink_port_t * port);

iolink_error_t DS_Startup (iolink_port_t * port);

iolink_error_t DS_Upload (iolink_port_t * port);

iolink_error_t DS_Init (
   iolink_port_t * port, // Not in spec
   const portconfiglist_t * cfg_list);

bool DS_Chk_Cfg (iolink_port_t * port, const portconfiglist_t * cfg_list);

iolink_error_t ds_SMI_ParServToDS_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t ds_SMI_DSToParServ_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);
#ifdef __cplusplus
}
#endif

#endif /* IOLINK_DS_H */
