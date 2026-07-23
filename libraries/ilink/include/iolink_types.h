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

#ifndef IOLINK_TYPES_H
#define IOLINK_TYPES_H

#include <stdint.h>
#include <string.h>

/**
 * @file
 *
 */

#define IOLINK_RXTX_BUFFER_SIZE 64

#define IOLINK_OD_MAX_SIZE        32
#define IOLINK_ISDU_MAX_DATA_SIZE 238
/* I-Service and len + ExtLen + CHKPDU = 3 */
#define IOLINK_ISDU_MAX_SIZE (IOLINK_ISDU_MAX_DATA_SIZE + 3)

#define IOL_DIR_PARAM_REV_V10 0x10
#define IOL_DIR_PARAM_REV_V11 0x11

typedef enum
{
   IOLINK_TARGET_MODE_INACTIVE,
   IOLINK_TARGET_MODE_DI,
   IOLINK_TARGET_MODE_DO,
   IOLINK_TARGET_MODE_COM1,
   IOLINK_TARGET_MODE_COM2,
   IOLINK_TARGET_MODE_COM3,
} iolink_target_mode_t;

typedef enum
{
   IOLINK_SMTARGET_MODE_CFGCOM = 0,
   IOLINK_SMTARGET_MODE_AUTOCOM,
   IOLINK_SMTARGET_MODE_INACTIVE,
   IOLINK_SMTARGET_MODE_DI,
   IOLINK_SMTARGET_MODE_DO,
} iolink_sm_target_mode_t;

typedef enum
{
   IOLINK_DLMODE_INACTIVE,
   IOLINK_DLMODE_STARTUP,
   IOLINK_DLMODE_PREOPERATE,
   IOLINK_DLMODE_OPERATE,
} iolink_dl_mode_t;

typedef enum
{
   IOLINK_BAUDRATE_NONE,
   IOLINK_BAUDRATE_AUTO,
   IOLINK_BAUDRATE_COM1,
   IOLINK_BAUDRATE_COM2,
   IOLINK_BAUDRATE_COM3,
} iolink_baudrate_t;

typedef enum
{
   IOLINK_INSPECTIONLEVEL_NO_CHECK,
   IOLINK_INSPECTIONLEVEL_TYPE_COMP,
   IOLINK_INSPECTIONLEVEL_IDENTICAL, /* NOTE: IO-Link Interface Spec v1.1.3
                                      * Chapter 9.2.2.2 Optional, not
                                      * recommended for new developments
                                      */
} iolink_inspectionlevel_t;

typedef enum
{
   IOLINK_MHMODE_INACTIVE,
   IOLINK_MHMODE_COM1,
   IOLINK_MHMODE_COM2,
   IOLINK_MHMODE_COM3,
   IOLINK_MHMODE_COMLOST,
   IOLINK_MHMODE_ESTABCOM,
   IOLINK_MHMODE_STARTUP,
   IOLINK_MHMODE_PREOPERATE,
   IOLINK_MHMODE_OPERATE,
} iolink_mhmode_t;

typedef enum
{
   IOLINK_SM_PORTMODE_INACTIVE,
   IOLINK_SM_PORTMODE_DI,
   IOLINK_SM_PORTMODE_DO,
   IOLINK_SM_PORTMODE_COMREADY,
   IOLINK_SM_PORTMODE_OPERATE,
   IOLINK_SM_PORTMODE_COMLOST,
   IOLINK_SM_PORTMODE_REVISION_FAULT,
   IOLINK_SM_PORTMODE_COMP_FAULT,
   IOLINK_SM_PORTMODE_SERNUM_FAULT,
   IOLINK_SM_PORTMODE_CYCTIME_FAULT,
} iolink_sm_portmode_t;

typedef enum
{
   IOLINK_STATUS_NO_ERROR = 0,
   IOLINK_STATUS_PARITY_ERROR,
   IOLINK_STATUS_FRAMING_ERROR,
   IOLINK_STATUS_OVERRUN,
   IOLINK_STATUS_NO_COMM,
   IOLINK_STATUS_STATE_CONFLICT,
   IOLINK_STATUS_VALID,
   IOLINK_STATUS_INVALID,
   IOLINK_STATUS_PARAMETER_CONFLICT,
   IOLINK_STATUS_ISDU_TIMEOUT,
   IOLINK_STATUS_ISDU_NOT_SUPPORTED,
   IOLINK_STATUS_VALUE_OUT_OF_RANGE,
   IOLINK_STATUS_ISDU_CHECKSUM_ERROR,
   IOLINK_STATUS_ISDU_ILLEGAL_SERVICE_PRIMITIVE,
   IOLINK_STATUS_ISDU_ABORT,
} iolink_status_t;

typedef enum
{
   IOLINK_RWDIRECTION_READ  = 0x80,
   IOLINK_RWDIRECTION_WRITE = 0x00,
} iolink_rwdirection_t;

typedef enum
{
   IOLINK_COMCHANNEL_PROCESS   = 0x00,
   IOLINK_COMCHANNEL_PAGE      = 0x20,
   IOLINK_COMCHANNEL_DIAGNOSIS = 0x40,
   IOLINK_COMCHANNEL_ISDU      = 0x60,
} iolink_comchannel_t;

typedef enum
{
   IOLINK_MHINFO_NONE,
   IOLINK_MHINFO_COMLOST,
   IOLINK_MHINFO_ILLEGAL_MESSAGETYPE,
   IOLINK_MHINFO_CHECKSUM_MISMATCH,
} iolink_mhinfo_t;

typedef enum
{
   IOLINK_MSEQTYPE_TYPE_NONE = 0x00,
   IOLINK_MSEQTYPE_TYPE_0    = 0x01,
   IOLINK_MSEQTYPE_TYPE_1_1  = 0x11,
   IOLINK_MSEQTYPE_TYPE_1_2  = 0x12,
   IOLINK_MSEQTYPE_TYPE_1_V  = 0x1E,
   IOLINK_MSEQTYPE_TYPE_1_X  = 0x1F,
   IOLINK_MSEQTYPE_TYPE_2_1  = 0x21,
   IOLINK_MSEQTYPE_TYPE_2_2  = 0x22,
   IOLINK_MSEQTYPE_TYPE_2_3  = 0x23,
   IOLINK_MSEQTYPE_TYPE_2_4  = 0x24,
   IOLINK_MSEQTYPE_TYPE_2_5  = 0x25,
   IOLINK_MSEQTYPE_TYPE_2_6  = 0x26, /**< removed from spec in 1.1.3, but some legacy devices use it */
   IOLINK_MSEQTYPE_TYPE_2_V = 0x2E,
   IOLINK_MSEQTYPE_TYPE_2_X = 0x2F,
} iolink_msequencetype_t;

typedef enum
{
   IOLINK_CONTROLCODE_NONE,
   IOLINK_CONTROLCODE_VALID,
   IOLINK_CONTROLCODE_INVALID,
   IOLINK_CONTROLCODE_PDOUTVALID,
   IOLINK_CONTROLCODE_PDOUTINVALID,
   IOLINK_CONTROLCODE_DEVICEMODE,
} iolink_controlcode_t;

typedef enum
{
   IOLINK_TRANSPORTSTATUS_YES,
   IOLINK_TRANSPORTSTATUS_NO,
} iolink_transportstatus_t;

typedef enum
{
   IOLINK_EVENT_MODE_RESERVED    = 0,
   IOLINK_EVENT_MODE_SINGLE_SHOT = 1,
   IOLINK_EVENT_MODE_DISAPPEARS  = 2,
   IOLINK_EVENT_MODE_APPEARS     = 3,
} iolink_event_mode_t;

typedef enum
{
   IOLINK_EVENT_TYPE_RESERVED     = 0,
   IOLINK_EVENT_TYPE_NOTIFICATION = 1,
   IOLINK_EVENT_TYPE_WARNING      = 2,
   IOLINK_EVENT_TYPE_ERROR        = 3,
} iolink_event_type_t;

typedef enum
{
   IOLINK_EVENT_INSTANCE_UNKNOWN     = 0,
   IOLINK_EVENT_INSTANCE_RESERVED_1  = 1,
   IOLINK_EVENT_INSTANCE_RESERVED_2  = 2,
   IOLINK_EVENT_INSTANCE_RESERVED_3  = 3,
   IOLINK_EVENT_INSTANCE_APPLICATION = 4,
   IOLINK_EVENT_INSTANCE_RESERVED_5  = 5,
   IOLINK_EVENT_INSTANCE_RESERVED_6  = 6,
   IOLINK_EVENT_INSTANCE_RESERVED_7  = 7,
} iolink_event_instance_t;

typedef enum
{
   IOLINK_EVENT_SOURCE_DEVICE = 0,
   IOLINK_EVENT_SOURCE_MASTER = 1,
} iolink_event_source_t;

typedef enum
{
   IOLINK_FLOWCTRL_START  = 0x10,
   IOLINK_FLOWCTRL_IDLE_1 = 0x11,
   IOLINK_FLOWCTRL_IDLE_2 = 0x12,
   IOLINK_FLOWCTRL_ABORT  = 0x1F,
} iolink_isdu_flowctrl_t;

typedef struct
{
   uint8_t time;
   iolink_msequencetype_t type;
   uint8_t pdinputlength;
   uint8_t pdoutputlength;
   uint8_t onreqdatalengthpermessage;
} iolink_mode_vl_t;

typedef struct
{
   uint16_t index;
   uint8_t subindex;
   union
   {
      uint8_t * data_read;
      const uint8_t * data_write;
   };
   uint8_t length;
   iolink_rwdirection_t readwrite;
} iolink_isdu_vl_t;

typedef struct
{
   uint8_t portnumber;
   uint8_t cycletime;
   iolink_sm_target_mode_t mode;
   uint8_t revisionid;
   iolink_inspectionlevel_t inspectionlevel;
   uint16_t vendorid;
   uint32_t deviceid;
   uint16_t functionid;
   uint8_t serialnumber[16];
} iolink_smp_parameterlist_t;

typedef enum
{
   IOLINK_DS_FAULT_NONE = 0,
   IOLINK_DS_FAULT_ID,
   IOLINK_DS_FAULT_SIZE,
   IOLINK_DS_FAULT_UP,
   IOLINK_DS_FAULT_DOWN,
   IOLINK_DS_FAULT_COM_ERR,
   IOLINK_DS_FAULT_LOCK,
} iolink_ds_fault_t;

typedef struct iolink_dev_com
{
   uint8_t mincycle;
   uint8_t mseq_cap;
   uint8_t pdi;
   uint8_t pdo;
} iolink_dev_com_t;

typedef enum
{
   IOLINK_MASTER_MB_TRIG_AL_READ_REQ,
   IOLINK_MASTER_MB_TRIG_AL_WRITE_REQ,
   IOLINK_MASTER_MB_TRIG_AL_ABORT_REQ,
   IOLINK_MASTER_MB_TRIG_AL_CONTROL_REQ,
   IOLINK_MASTER_MB_TRIG_AL_EVENT_REQ,
   IOLINK_MASTER_MB_TRIG_AL_GETINPUT_REQ,
   IOLINK_MASTER_MB_TRIG_AL_SETOUTPUT_REQ,
   IOLINK_MASTER_MB_TRIG_DL_CONTROL_IND,
   IOLINK_MASTER_MB_TRIG_DL_EVENT_IND,
   IOLINK_MASTER_MB_TRIG_DL_PDINPUTTRANSPORT_IND,
   IOLINK_MASTER_MB_TRIG_DL_PDCYCLE,
   IOLINK_MASTER_MB_TRIG_DL_READPARAM_CNF,
   IOLINK_MASTER_MB_TRIG_DL_WRITEPARAM_CNF,
   IOLINK_MASTER_MB_TRIG_DL_READ_CNF,
   IOLINK_MASTER_MB_TRIG_DL_WRITE_CNF,
   IOLINK_MASTER_MB_TRIG_DL_ISDUTRAANSPORT_CNF,
   IOLINK_MASTER_MB_TRIG_DL_PDOUTPUTUPDATE_CNF,
   IOLINK_MASTER_MB_TRIG_DL_SETMODE_CNF,
   IOLINK_MASTER_MB_TRIG_DL_MODE_IND,
   IOLINK_MASTER_MB_TRIG_AL_READ_CNF,
   IOLINK_MASTER_MB_TRIG_SM_SETPORTCONFIG_REQ,
   IOLINK_MASTER_MB_TRIG_SM_GETPORTCONFIG_REQ,
   IOLINK_MASTER_MB_TRIG_SM_OPERATE_REQ,

} iolink_master_mbox_trigger_t;

typedef union
{
   uint32_t data;
   struct
   {
      uint16_t extdata;
      uint8_t port;
      iolink_master_mbox_trigger_t trigger;
   };
} iolink_master_mbox_payload_t;
#endif /* IOLINK_TYPES_H */
