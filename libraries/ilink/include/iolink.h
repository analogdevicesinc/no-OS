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

#ifndef IOLINK_H
#define IOLINK_H

// TODO fix this......
#ifndef CC_PACKED_BEGIN
#define CC_PACKED_BEGIN
#endif

#ifndef CC_PACKED_END
#define CC_PACKED_END
#endif

#include <sys/osal_cc.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "iolink_options.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @brief Main API for IO-Link stack
 *
 */

#define IOLINK_REVISION_1_1 0x11

#define IOLINK_PD_MAX_SIZE 32 //!< Maximum number of bytes in Process Data

typedef struct iolink_m_cfg iolink_m_cfg_t;
typedef struct iolink_m iolink_m_t;
typedef uint8_t iolink_port_qualifier_info_t; //!< Port Qualifier Information
                                              //!< (PQI)
typedef uint8_t iolink_port_quality_info_t;   //!< Port Quality Information

/**
 * IO-Link PL modes
 */
typedef enum
{
   /** Inactive */
   iolink_mode_INACTIVE = 0,

   /** Digital out */
   iolink_mode_DO       = 1,

   /** Digital in */
   iolink_mode_DI       = 2,

   /** IO-Link signal mode */
   iolink_mode_SDCI     = 3,
} iolink_pl_mode_t;

/**
 * IO-Link general error types
 */
typedef enum
{
   IOLINK_ERROR_NONE = 0,
   IOLINK_ERROR_PDINLENGTH,
   IOLINK_ERROR_PDOUTLENGTH,
   IOLINK_ERROR_ODLENGTH,
   IOLINK_ERROR_ADDRESS_INVALID,
   IOLINK_ERROR_COMCHANNEL_INVALID,
   IOLINK_ERROR_MODE_INVALID,
   IOLINK_ERROR_STATUS_INVALID,
   IOLINK_ERROR_CONTROLCODE_INVALID,
   IOLINK_ERROR_CONDITIONS_NOT_CORRECT,
   IOLINK_ERROR_INCORRECT_DATA,
   IOLINK_ERROR_STATE_INVALID,
   IOLINK_ERROR_OUT_OF_MEMORY,
   IOLINK_ERROR_PARAMETER_CONFLICT,
   IOLINK_ERROR_STATE_CONFLICT,
} iolink_error_t;

/**
 * IO-Link ArgBlock IDs
 *
 * IO-Link Interface Spec v1.1.3 Table E.1 ArgBlockIDs
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_arg_block_id
{
   IOLINK_ARG_BLOCK_ID_MASTERIDENT         = 0x0001,
   IOLINK_ARG_BLOCK_ID_FS_MASTER_ACCESS    = 0x0100,
   IOLINK_ARG_BLOCK_ID_W_MASTER_CFG        = 0x0200,
   IOLINK_ARG_BLOCK_ID_PD_IN               = 0x1001,
   IOLINK_ARG_BLOCK_ID_PD_OUT              = 0x1002,
   IOLINK_ARG_BLOCK_ID_PD_IN_OUT           = 0x1003,
   IOLINK_ARG_BLOCK_ID_SPDU_IN             = 0x1101,
   IOLINK_ARG_BLOCK_ID_SPDU_OUT            = 0x1102,
   IOLINK_ARG_BLOCK_ID_PD_IN_IQ            = 0x1FFE,
   IOLINK_ARG_BLOCK_ID_PD_OUT_IQ           = 0x1FFF,
   IOLINK_ARG_BLOCK_ID_OD_WR               = 0x3000,
   IOLINK_ARG_BLOCK_ID_OD_RD               = 0x3001,
   IOLINK_ARG_BLOCK_ID_DS_DATA             = 0x7000,
   IOLINK_ARG_BLOCK_ID_DEV_PAR_BAT         = 0x7001,
   IOLINK_ARG_BLOCK_ID_PORT_INXDEX_LIST    = 0x7002,
   IOLINK_ARG_BLOCK_ID_PORT_POWER          = 0x7003,
   IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST       = 0x8000,
   IOLINK_ARG_BLOCK_ID_FS_PORT_CFG_LIST    = 0x8100,
   IOLINK_ARG_BLOCK_ID_W_TRACK_CFG_LIST    = 0x8200,
   IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST    = 0x9000,
   IOLINK_ARG_BLOCK_ID_FS_PORT_STATUS_LIST = 0x9100,
   IOLINK_ARG_BLOCK_ID_W_TRACK_STATUS_LIST = 0x9200,
   IOLINK_ARG_BLOCK_ID_W_TRACK_SCAN_RES    = 0x9201,
   IOLINK_ARG_BLOCK_ID_DEV_EVENT           = 0xA000,
   IOLINK_ARG_BLOCK_ID_PORT_EVENT          = 0xA001,
   IOLINK_ARG_BLOCK_ID_VOID_BLOCK          = 0xFFF0,
   IOLINK_ARG_BLOCK_ID_JOB_ERROR           = 0xFFFF,
} iolink_arg_block_id_t;
CC_PACKED_END

/**
 * IO-Link EventCodes
 *
 * IO-Link Interface Spec v1.1.3 Table D.2 EventCodes for Ports
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_eventcode
{
   IOLINK_EVENTCODE_NONE              = 0x0000,
   IOLINK_EVENTCODE_NO_DEV            = 0x1800,
   IOLINK_EVENTCODE_START_PARAM_ERR   = 0x1801,
   IOLINK_EVENTCODE_BAD_VID           = 0x1802,
   IOLINK_EVENTCODE_BAD_DID           = 0x1803,
   IOLINK_EVENTCODE_SHORT_CIR_CQ      = 0x1804,
   IOLINK_EVENTCODE_PHY_OVERTEMP      = 0x1805,
   IOLINK_EVENTCODE_SHORT_CIR_L       = 0x1806,
   IOLINK_EVENTCODE_OVERCUR_L         = 0x1807,
   IOLINK_EVENTCODE_DEV_EVENT_OVERF   = 0x1808,
   IOLINK_EVENTCODE_BACKUP_INCON_SIZE = 0x1809,
   IOLINK_EVENTCODE_BACKUP_INCON_ID   = 0x180A,
   IOLINK_EVENTCODE_BACKUP_INCON      = 0x180B,
   IOLINK_EVENTCODE_BACKUP_INCON_UL   = 0x180C,
   IOLINK_EVENTCODE_BACKUP_INCON_DL   = 0x180D,
   IOLINK_EVENTCODE_P24               = 0x180E,
   IOLINK_EVENTCODE_P24_SHORT_CIR     = 0x180F,
   IOLINK_EVENTCODE_SHORT_CIR_IQ      = 0x1810,
   IOLINK_EVENTCODE_SHORT_CIR_DO_CQ   = 0x1811,
   IOLINK_EVENTCODE_OVERCUR_IQ        = 0x1812,
   IOLINK_EVENTCODE_OVERCUR_CQ        = 0x1813,

   IOLINK_EVENTCODE_INVAL_CYCTIME = 0x6000,
   IOLINK_EVENTCODE_REV_FAULT     = 0x6001,
   IOLINK_EVENTCODE_ISDU_FAIL     = 0x6002,

   IOLINK_EVENTCODE_PORT_STATUS_CHANGE = 0xFF26,
   IOLINK_EVENTCODE_DS_UPLOAD_DONE     = 0xFF27,

   IOLINK_EVENTCODE_DEV_DS_UPLOAD_REQ = 0xFF91, /* From D.1 EventCodes for
                                                   Devices */
} iolink_eventcode_t;
CC_PACKED_END

/**
 * Port modes
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_portmode
{
   IOLINK_PORTMODE_DEACTIVE = 0,
   IOLINK_PORTMODE_IOL_MAN  = 1,
   IOLINK_PORTMODE_IOL_AUTO = 2,
   IOLINK_PORTMODE_DI_CQ    = 3,
   IOLINK_PORTMODE_DO_CQ    = 4,
} iolink_portmode_t;
CC_PACKED_END

/**
 * Validation check values
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_validation_check
{
   IOLINK_VALIDATION_CHECK_NO              = 0,
   IOLINK_VALIDATION_CHECK_V10             = 1,
   IOLINK_VALIDATION_CHECK_V11             = 2,
   IOLINK_VALIDATION_CHECK_V11_BAK_RESTORE = 3,
   IOLINK_VALIDATION_CHECK_V11_RESTORE     = 4,
} iolink_validation_check_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_iq_behavior
{
   IOLINK_IQ_BEHAVIOR_NO_SUPPORT = 0,
   IOLINK_IQ_BEHAVIOR_DI         = 1,
   IOLINK_IQ_BEHAVIOR_DO         = 2,
   IOLINK_IQ_BEHAVIOR_RES1       = 3,
   IOLINK_IQ_BEHAVIOR_RES2       = 4,
   IOLINK_IQ_BEHAVIOR_POWER2     = 5,
} iolink_iq_behavior_t;
CC_PACKED_END

typedef enum
{
   IOLINK_PORT_TYPES_A            = 0,
   IOLINK_PORT_TYPES_A_POWER      = 1,
   IOLINK_PORT_TYPES_B            = 2,
   IOLINK_PORT_TYPES_FS_A_NO_OSSD = 3,
   IOLINK_PORT_TYPES_FS_A_OSSD    = 4,
   IOLINK_PORT_TYPES_FS_B         = 5,
   IOLINK_PORT_TYPES_W_MASTER     = 6,
} iolink_port_types_t;

/**
 * Port Status Information
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_port_status_info
{
   IOLINK_PORT_STATUS_INFO_NO_DEV      = 0,
   IOLINK_PORT_STATUS_INFO_DEACTIVATED = 1,
   IOLINK_PORT_STATUS_INFO_PORT_DIAG   = 2,
   IOLINK_PORT_STATUS_INFO_PREOP       = 3,
   IOLINK_PORT_STATUS_INFO_OP          = 4,
   IOLINK_PORT_STATUS_INFO_DI          = 5,
   IOLINK_PORT_STATUS_INFO_DO          = 6,
   IOLINK_PORT_STATUS_INFO_POWER_OFF   = 254,
   IOLINK_PORT_STATUS_INFO_NOT_AVAIL   = 255,
} iolink_port_status_info_t;
CC_PACKED_END

/* Port Quality Info bits, iolink_port_quality_info_t */
/* PDIn bit0 */
#define IOLINK_PORT_QUALITY_INFO_BIT     0
#define IOLINK_PORT_QUALITY_INFO_VALID   0
#define IOLINK_PORT_QUALITY_INFO_INVALID BIT (IOLINK_PORT_QUALITY_INFO_BIT)
/* PDOut bit1 */
#define IOLINK_PORT_QUALITY_INFO_PDO_BIT   1
#define IOLINK_PORT_QUALITY_INFO_PDO_VALID 0
#define IOLINK_PORT_QUALITY_INFO_PDO_INVALID                                   \
   BIT (IOLINK_PORT_QUALITY_INFO_PDO_BIT)

/* Port Qualifier Information bits, iolink_port_qualifier_info_t */
/* DevCom bit5 */
#define IOLINK_PORT_QUALIFIER_INFO_COM_BIT   5
#define IOLINK_PORT_QUALIFIER_INFO_COM_VALID 0
#define IOLINK_PORT_QUALIFIER_INFO_COM_INVALID                                 \
   BIT (IOLINK_PORT_QUALIFIER_INFO_COM_BIT)
/* DevErr bit6 */
#define IOLINK_PORT_QUALIFIER_INFO_ERR_BIT   6
#define IOLINK_PORT_QUALIFIER_INFO_ERR_VALID 0
#define IOLINK_PORT_QUALIFIER_INFO_ERR_INVALID                                 \
   BIT (IOLINK_PORT_QUALIFIER_INFO_ERR_BIT)
/* PQ bit7 */
#define IOLINK_PORT_QUALIFIER_INFO_PQ_BIT   7
#define IOLINK_PORT_QUALIFIER_INFO_PQ_VALID 0
#define IOLINK_PORT_QUALIFIER_INFO_PQ_INVALID                                  \
   BIT (IOLINK_PORT_QUALIFIER_INFO_PQ_BIT) //!< Port Qualifier bit in QPI

/**
 * Transmission rates
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_transmission_rate
{
   IOLINK_TRANSMISSION_RATE_NOT_DETECTED = 0,
   IOLINK_TRANSMISSION_RATE_COM1         = 1,
   IOLINK_TRANSMISSION_RATE_COM2         = 2,
   IOLINK_TRANSMISSION_RATE_COM3         = 3,
} iolink_transmission_rate_t;
CC_PACKED_END

/**
 * SMI error types
 * IO-Link Interface Spec v1.1.3
 * Table C.1 ErrorTypes, C.2 Derived ErrorTypes and C.3 SMI related ErrorTypes
 */
CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_smi_errortypes
{
   IOLINK_SMI_ERRORTYPE_NONE = 0x0000,
   /* Table C.1 ErrorTypes */
   IOLINK_SMI_ERRORTYPE_APP_DEV               = 0x8000,
   IOLINK_SMI_ERRORTYPE_IDX_NOTAVAIL          = 0x8011,
   IOLINK_SMI_ERRORTYPE_SUBIDX_NOTAVAIL       = 0x8012,
   IOLINK_SMI_ERRORTYPE_SERV_NOTAVAIL         = 0x8020,
   IOLINK_SMI_ERRORTYPE_SERV_NOTAVAIL_LOCCTRL = 0x8021,
   IOLINK_SMI_ERRORTYPE_SERV_NOTAVAIL_DEVCTRL = 0x8022,
   IOLINK_SMI_ERRORTYPE_IDX_NOT_ACCESSIBLE    = 0x8023,
   IOLINK_SMI_ERRORTYPE_PAR_VALOUTOFRNG       = 0x8030,
   IOLINK_SMI_ERRORTYPE_PAR_VALGTLIM          = 0x8031,
   IOLINK_SMI_ERRORTYPE_PAR_VALLTLIM          = 0x8032,
   IOLINK_SMI_ERRORTYPE_VAL_LENOVRRUN         = 0x8033, /* Also in C.2 */
   IOLINK_SMI_ERRORTYPE_VAL_LENUNDRUN         = 0x8034,
   IOLINK_SMI_ERRORTYPE_FUNC_NOTAVAIL         = 0x8035,
   IOLINK_SMI_ERRORTYPE_FUNC_UNAVAILTEMP      = 0x8036,
   IOLINK_SMI_ERRORTYPE_PAR_SETINVALID        = 0x8040,
   IOLINK_SMI_ERRORTYPE_PAR_SETINCONSIST      = 0x8041,
   IOLINK_SMI_ERRORTYPE_APP_DEVNOTRDY         = 0x8082,
   IOLINK_SMI_ERRORTYPE_UNSPECIFIC            = 0x8100,
   /* Table C.2 Derivced ErrorTypes */
   IOLINK_SMI_ERRORTYPE_COM_ERR           = 0x1000,
   IOLINK_SMI_ERRORTYPE_I_SERVICE_TIMEOUT = 0x1100,
   IOLINK_SMI_ERRORTYPE_M_ISDU_CHECKSUM   = 0x5600,
   IOLINK_SMI_ERRORTYPE_M_ISDU_ILLEGAL    = 0x5700,

   /* Table C.3 SMI related ErrorTypes */
   IOLINK_SMI_ERRORTYPE_ARGBLOCK_NOT_SUPPORTED   = 0x4001,
   IOLINK_SMI_ERRORTYPE_ARGBLOCK_INCONSISTENT    = 0x4002,
   IOLINK_SMI_ERRORTYPE_DEV_NOT_ACCESSIBLE       = 0x4003,
   IOLINK_SMI_ERRORTYPE_SERVICE_NOT_SUPPORTED    = 0x4004,
   IOLINK_SMI_ERRORTYPE_DEV_NOT_IN_OPERATE       = 0x4005,
   IOLINK_SMI_ERRORTYPE_MEMORY_OVERRUN           = 0x4006,
   IOLINK_SMI_ERRORTYPE_PORT_NUM_INVALID         = 0x4011,
   IOLINK_SMI_ERRORTYPE_ARGBLOCK_LENGTH_INVALID  = 0x4034,
   IOLINK_SMI_ERRORTYPE_SERVICE_TEMP_UNAVAILABLE = 0x4036,
} iolink_smi_errortypes_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef enum CC_PACKED iolink_master_type
{
   IOLINK_MASTER_TYPE_UNSPEC     = 0,
   IOLINK_MASTER_TYPE_RESERVED   = 1,
   IOLINK_MASTER_TYPE_MASTER_ACC = 2,
   IOLINK_MASTER_TYPE_FS         = 3,
   IOLINK_MASTER_TYPE_W          = 4,
} iolink_master_type_t;
CC_PACKED_END

/* Generic arg-block

 * This is the "base class" for all other arg-blocks. All arg-blocks structs
 * must contain this as their first element.
 */
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block
{
  iolink_arg_block_id_t id;
} arg_block_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_masterident_head
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_MASTERIDENT
   uint16_t vendorid;
   uint32_t masterid;
   iolink_master_type_t master_type;
   uint8_t features_1;
   uint8_t features_2; // reserved
   uint8_t max_number_of_ports;
} arg_block_masterident_head_t;
CC_PACKED_END
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_masterident
{
   arg_block_masterident_head_t h;
   iolink_port_types_t port_types[IOLINK_NUM_PORTS];
} arg_block_masterident_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdin_head
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PD_IN
   iolink_port_qualifier_info_t port_qualifier_info;
   uint8_t len;
} arg_block_pdin_head_t;
CC_PACKED_END
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdin
{
   arg_block_pdin_head_t h;
   uint8_t data[IOLINK_PD_MAX_SIZE];
} arg_block_pdin_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdout_head
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PD_OUT
   uint8_t oe;
   uint8_t len;
} arg_block_pdout_head_t;
CC_PACKED_END
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdout
{
   arg_block_pdout_head_t h;
   uint8_t data[IOLINK_PD_MAX_SIZE];
} arg_block_pdout_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdinout_head
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PD_IN_OUT
   iolink_port_qualifier_info_t port_qualifier_info;
   uint8_t oe;
} arg_block_pdinout_head_t;
CC_PACKED_END
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_pdinout
{
   arg_block_pdinout_head_t h;
   /* data[0] = in_len
    * data[1] = in_data0, data[2] = in_data1 ... data[in_len] = in_dataN
    * data[in_len + 1] = out_len
    * data[in_len + 2] = out_data0, data[in_len + 3] = out_data1 ... data[in_len
    * + out_len = 1] = out_dataM
    */
   uint8_t data[1 + IOLINK_PD_MAX_SIZE + 1 + IOLINK_PD_MAX_SIZE];
} arg_block_pdinout_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_od
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_OD_{WR,RD}
   uint16_t index;
   uint8_t subindex;
   uint8_t data[];
} arg_block_od_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_ds_data
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_DS_DATA
   uint32_t checksum;
   uint16_t vid;
   uint32_t did;
   uint16_t fid;

   uint8_t ds_data[];
} arg_block_ds_data_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED portconfiglist
{
   iolink_portmode_t portmode;
   iolink_validation_check_t validation_backup;
   iolink_iq_behavior_t iq_behavior;
   uint8_t port_cycle_time;
   uint16_t vendorid;
   uint32_t deviceid;
   uint8_t in_buffer_len;
   uint8_t out_buffer_len;
} portconfiglist_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_portconfiglist
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PORT_CFG_LIST
   portconfiglist_t configlist;
} arg_block_portconfiglist_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED diag_entry
{
   uint8_t event_qualifier;
   iolink_eventcode_t event_code;
} diag_entry_t;
CC_PACKED_END
CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_portstatuslist
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PORT_STATUS_LIST
   iolink_port_status_info_t port_status_info;
   iolink_port_quality_info_t port_quality_info;
   uint8_t revision_id;
   iolink_transmission_rate_t transmission_rate;
   uint8_t master_cycle_time;
   uint8_t reserved;
   uint16_t vendorid;
   uint32_t deviceid;
   uint8_t number_of_diags;
   diag_entry_t * diag_entries;
} arg_block_portstatuslist_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_devevent
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_DEV_EVENT
   diag_entry_t diag_entry[6];         // TODO diag_entry_t diag_entry
   uint8_t event_count;                // TODO remove
} arg_block_devevent_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_portevent
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_PORT_EVENT
   diag_entry_t event;
} arg_block_portevent_t;
CC_PACKED_END

typedef struct arg_block_void
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_VOID_BLOCK
} arg_block_void_t;

CC_PACKED_BEGIN
typedef struct CC_PACKED arg_block_joberror
{
   arg_block_t arg_block; // IOLINK_ARG_BLOCK_ID_JOB_ERROR
   iolink_arg_block_id_t exp_arg_block_id;
   iolink_smi_errortypes_t error;
} arg_block_joberror_t;
CC_PACKED_END

static_assert (sizeof (iolink_portmode_t) == 1, "");
static_assert (sizeof (iolink_validation_check_t) == 1, "");
static_assert (sizeof (iolink_iq_behavior_t) == 1, "");
static_assert (sizeof (iolink_arg_block_id_t) == 2, "");
static_assert (sizeof (iolink_port_status_info_t) == 1, "");
static_assert (sizeof (iolink_transmission_rate_t) == 1, "");

static_assert (sizeof (diag_entry_t) == 3, "");

static_assert (sizeof (arg_block_masterident_head_t) == 12, "");
static_assert (
   sizeof (arg_block_masterident_t) ==
      sizeof (arg_block_masterident_head_t) +
         sizeof (iolink_port_types_t) * IOLINK_NUM_PORTS,
   "");

static_assert (sizeof (arg_block_pdin_head_t) == 4, "");
static_assert (
   sizeof (arg_block_pdin_t) ==
      sizeof (arg_block_pdin_head_t) + IOLINK_PD_MAX_SIZE,
   "");

static_assert (sizeof (arg_block_pdout_head_t) == 4, "");
static_assert (
   sizeof (arg_block_pdout_t) ==
      sizeof (arg_block_pdout_head_t) + IOLINK_PD_MAX_SIZE,
   "");

static_assert (sizeof (arg_block_pdinout_head_t) == 4, "");
static_assert (
   sizeof (arg_block_pdinout_t) ==
      sizeof (arg_block_pdinout_head_t) + 2 + 2 * IOLINK_PD_MAX_SIZE,
   "");

static_assert (sizeof (arg_block_od_t) == 5, "");
static_assert (sizeof (arg_block_ds_data_t) == 2 + 12, "");

static_assert (sizeof (portconfiglist_t) == 12, "");
static_assert (
   sizeof (arg_block_portconfiglist_t) == 2 + sizeof (portconfiglist_t),
   "");

static_assert (
   sizeof (arg_block_portstatuslist_t) == sizeof (diag_entry_t *) + 15,
   "");
static_assert (sizeof (arg_block_devevent_t) == 3 + sizeof (diag_entry_t) * 6, "");
static_assert (sizeof (arg_block_portevent_t) == 5, "");
static_assert (sizeof (arg_block_void_t) == 2, "");
static_assert (sizeof (arg_block_joberror_t) == 6, "");

typedef struct iolink_hw_drv iolink_hw_drv_t;

/** Port configuration */
typedef struct iolink_port_cfg
{
   /** Port name */
   const char * name;

   /** Port mode */
   iolink_pl_mode_t * mode;

   /** Driver */
   iolink_hw_drv_t * drv;

   /** User argument */
   void * arg;
} iolink_port_cfg_t;

/** IO-Link master stack configuration */
typedef struct iolink_m_cfg
{
   /** Callback opaque argument */
   void * cb_arg;

   /** SMI cnf/ind callback function */
   void (*cb_smi) (
      void * arg,
      uint8_t portnumber,
      iolink_arg_block_id_t ref_arg_block_id,
      uint16_t arg_block_len,
      arg_block_t * arg_block);

   /** Periodic data callback function */
   void (*cb_pd) (
      uint8_t portnumber,
      void * arg,
      uint8_t data_len,
      const uint8_t * data);

   /** Number of connected IO-Link ports */
   uint8_t port_cnt;

   /** Configuration (name and mode) of the connected IO-Link ports */
   const iolink_port_cfg_t * port_cfgs;

   /** Priority of the master thread */
   unsigned int master_thread_prio;

   /** Stack size (in bytes) of the master thread */
   size_t master_thread_stack_size;

   /** Priority of the DL thread */
   unsigned int dl_thread_prio;

   /** Stack size (in bytes) of the DL thread */
   size_t dl_thread_stack_size;
} iolink_m_cfg_t;

/**
 * Initialise IO-Link master
 *
 * This function initialises the SM state-machine and should be
 * called when the stack is started.
 *
 * @param m_cfg           Master stack configuration
 * @return                Pointer to master information struct
 */
iolink_m_t * iolink_m_init (const iolink_m_cfg_t * m_cfg);

/**
 * De-initialise IO-Link master
 *
 * @param m          Master information struct
 */
void iolink_m_deinit (iolink_m_t ** m);

iolink_error_t SMI_MasterIdentification_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Set port configuration
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_PortConfiguration_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Get port configuration
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_ReadbackPortConfiguration_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Get port status
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_PortStatus_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Retrieve parameters from data storage
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_DSToParServ_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Send parameters to data storage
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_ParServToDS_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Read acyclic data from device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_DeviceRead_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Write acyclic data to device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_DeviceWrite_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Batch read parameters from device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_ParamReadBatch_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Batch write parameters from device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_ParamWriteBatch_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Read cyclic data from the device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_PDIn_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Write cyclic data to the device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_PDOut_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

/**
 * Read and write cyclic data to the device
 *
 * @param portnumber          Port number
 * @param exp_arg_block_id    Block ID
 * @param arg_block_len       Block length
 * @param arg_block           Block
 * @return                    Error type
 */
iolink_error_t SMI_PDInOut_req (
   uint8_t portnumber,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_H */
