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
 * @brief Data link layer
 *
 */

#ifndef IOLINK_DL_H
#define IOLINK_DL_H

#include <iolink_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iolink_dl iolink_dl_t;
#include "iolink_main.h"
#include "iolink_options.h" /* IOLINK_MAX_EVENTS */
#include "osal.h"

/**
 * @file
 *
 */

#define IOLINK_PL_EVENT              BIT (0)
#define IOLINK_PL_EVENT_RXRDY        BIT (1)
#define IOLINK_PL_EVENT_RXERR        BIT (2)
#define IOLINK_PL_EVENT_TXERR        BIT (3)
#define IOLINK_PL_EVENT_WURQ         BIT (4)
#define IOLINK_PL_EVENT_STATUS       BIT (5)
#define IOLINK_DL_EVENT_RESET        BIT (9)
#define IOLINK_DL_EVENT_TIMEOUT      BIT (10)
#define IOLINK_DL_EVENT_TIMEOUT_TCYC BIT (11)
// #define IOLINK_DL_EVENT_ODH            BIT(12)
// #define IOLINK_DL_EVENT_PDH            BIT(13)
#define IOLINK_DL_EVENT_MDH BIT (14)
#define IOLINK_DL_EVENT_MH  BIT (15)

typedef enum
{
   IOL_DL_TIMER_NONE,
   IOL_DL_TIMER_TINITCYC_MH,
   IOL_DL_TIMER_ISDUTIME_IH,

   IOL_DL_TIMER_TDMT,
   IOL_DL_TIMER_TDSIO,
   IOL_DL_TIMER_TDWU,
   IOL_DL_TIMER_TSD,
} dl_timer_t;

/*
 * State machine states
 */
typedef enum
{
   IOL_DL_MDH_ST_IDLE_0,
   IOL_DL_MDH_ST_ESTCOM_1,
   IOL_DL_MDH_ST_STARTUP_2,
   IOL_DL_MDH_ST_PREOPERATE_3,
   IOL_DL_MDH_ST_OPERATE_4,
} dl_mdh_st_t;

typedef enum
{
   IOL_DL_MH_ST_INACTIVE_0,
   IOL_DL_MH_ST_AW_REPLY_1,
   IOL_DL_MH_ST_STARTUP_2,
   IOL_DL_MH_ST_RESPONSE_3,
   IOL_DL_MH_ST_AW_REPLY_4,
   IOL_DL_MH_ST_ERRORHANDLING_5,
   IOL_DL_MH_ST_PREOPERATE_6,
   IOL_DL_MH_ST_GETOD_7,
   IOL_DL_MH_ST_RESPONSE_8,
   IOL_DL_MH_ST_AW_REPLY_9,
   IOL_DL_MH_ST_ERRORHANDLING_10,
   IOL_DL_MH_ST_CHECKHANDLER_11,
   IOL_DL_MH_ST_OPERATE_12,
   IOL_DL_MH_ST_GETPD_13,
   IOL_DL_MH_ST_GETOD_14,
   IOL_DL_MH_ST_RESPONSE_15,
   IOL_DL_MH_ST_AW_REPLY_16,
   IOL_DL_MH_ST_ERRORHANDLING_17,
} dl_mh_st_t;

typedef enum
{
   IOL_DL_INTERLEAVE_NONE,
   IOL_DL_INTERLEAVE_PD,
   IOL_DL_INTERLEAVE_OD,
} dl_interleave_t;

typedef enum
{
   IOL_DL_PDH_ST_INACTIVE_0,
   IOL_DL_PDH_ST_PDSINGLE_1,
   IOL_DL_PDH_ST_PDININTERLEAVE_2,
   IOL_DL_PDH_ST_PDOUTINTERLEAVE_3,
} dl_pdh_st_t;

typedef enum
{
   IOL_DL_ODH_ST_INACTIVE_0,
   IOL_DL_ODH_ST_ISDU_1,
   IOL_DL_ODH_ST_COMMAND_2,
   IOL_DL_ODH_ST_EVENT_3,
} dl_odh_st_t;

typedef enum
{
   IOL_DL_ISDUH_ST_INACTIVE_0,
   IOL_DL_ISDUH_ST_IDLE_1,
   IOL_DL_ISDUH_ST_ISDUREQUEST_2,
   IOL_DL_ISDUH_ST_ISDUWAIT_3,
   IOL_DL_ISDUH_ST_ISDUERROR_4,
   IOL_DL_ISDUH_ST_ISDURESPONSE_5,
} dl_isduh_st_t;

typedef enum
{
   IOL_DL_CMDH_ST_INACTIVE_0,
   IOL_DL_CMDH_ST_IDLE_1,
   IOL_DL_CMDH_ST_MASTERCOMMAND_2,
} dl_cmdh_st_t;

typedef enum
{
   IOL_DL_EVH_ST_INACTIVE_0,
   IOL_DL_EVH_ST_IDLE_1,
   IOL_DL_EVH_ST_READEVENT_2,
   IOL_DL_EVH_ST_SIGNALEVENT_3,
   IOL_DL_EVH_ST_EVENTCONFIRMATION_4,
} dl_evh_st_t;

/*
 * Internals
 */
typedef enum
{
   IOL_CHCMD_INACTIVE,
   IOL_CHCMD_ACTIVE,
} CHCmd_t;

typedef enum
{
   IOL_OHCMD_INACTIVE,
   IOL_OHCMD_ACTIVE,
} OHCmd_t;

typedef enum
{
   IOL_PHCMD_INACTIVE,
   IOL_PHCMD_ACTIVE,
   IOL_PHCMD_SINGLE,
   IOL_PHCMD_INTERLEAVE,
} PHCmd_t;

typedef enum
{
   IOL_IHCMD_INACTIVE,
   IOL_IHCMD_ACTIVE,
} IHCmd_t;

typedef enum
{
   IOL_EHCMD_INACTIVE,
   IOL_EHCMD_ACTIVE,
} EHCmd_t;

typedef enum
{
   IOL_MHCMD_INACTIVE,
   IOL_MHCMD_COM1,
   IOL_MHCMD_COM2,
   IOL_MHCMD_COM3,
   IOL_MHCMD_ACTIVE,
   IOL_MHCMD_STARTUP,
   IOL_MHCMD_PREOPERATE,
   IOL_MHCMD_OPERATE,
} MHCmd_t;

typedef enum
{
   IOL_MHRW_NONE,
   IOL_MHRW_WRITE,
   IOL_MHRW_READ,
   IOL_MHRW_WRITEPARAM,
   IOL_MHRW_READPARAM,
   IOL_MHRW_ISDUTRANSPORT,
   IOL_MHRW_ISDUABORT,
   IOL_MHRW_WRITEDEVICEMODE,
} MHRWCmd;

typedef enum
{
   IOL_TRIGGERED_NONE,
   IOL_TRIGGERED_DEVICE_MESSAGE,
   IOL_TRIGGERED_MASTER_MESSAGE,
} iol_trigger_t;

typedef struct
{
   dl_pdh_st_t state;
   PHCmd_t phcmd;
   iol_trigger_t trigger;
   uint8_t pd_address;
   uint8_t pd_rxlen;
   uint8_t pd_txlen;
   uint8_t pdoutdata[IOLINK_PD_MAX_SIZE];
   uint8_t pdindata[IOLINK_PD_MAX_SIZE];
   uint8_t * pdout_buffer;
   bool pd_valid;
} pd_handler_t;

typedef struct
{
   dl_odh_st_t state;
   OHCmd_t ohcmd;
   iol_trigger_t trigger;
   uint8_t od_rxlen;
   uint8_t od_txlen;
   uint8_t data_addr;
   uint8_t data_value;
   uint8_t * odout_buffer;
} od_handler_t;

typedef struct
{
   dl_mh_st_t state;
   MHCmd_t mhcmd;
   MHRWCmd rwcmd;
   uint8_t pd_rxlen;
   uint8_t pd_txlen;
   uint8_t od_len;
   uint8_t cks;
   uint8_t retry;
   dl_interleave_t interleave;
} message_h_t;

typedef struct
{
   dl_mdh_st_t state;
   iolink_dl_mode_t dl_mode;
   iolink_mhinfo_t mhinfo;
} mode_h_t;

typedef struct
{
   dl_cmdh_st_t state;
   CHCmd_t chcmd;
   iolink_controlcode_t control_code;
   uint8_t master_command;
} command_h_t;

typedef struct
{
   dl_isduh_st_t state;
   IHCmd_t ihcmd;
   uint8_t total_isdu_len;
   uint8_t current_isdu_seg;
   uint8_t total_isdu_seg;
   uint8_t isdu_data[IOLINK_ISDU_MAX_SIZE];
   uint32_t isdu_timer;
   bool data_dir_read;
} isdu_h_t;

typedef struct
{
   uint8_t event_qualifier;
   uint8_t event_code_msb;
   uint8_t event_code_lsb;
} event_t;

typedef struct
{
   dl_evh_st_t state;
   EHCmd_t ehcmd;
   bool event_flag;
   bool event_confirmation;
   uint8_t ev_addr;
   uint8_t status_code;
   uint8_t num_entries;
   uint8_t ev_cnt;
   uint8_t ev_current;
   event_t events[IOLINK_MAX_EVENTS];
} event_h_t;

typedef struct iolink_dl
{
   mode_h_t mode_handler;
   message_h_t message_handler;
   pd_handler_t pd_handler;
   od_handler_t od_handler;
   command_h_t cmd_handler;
   isdu_h_t isdu_handler;
   event_h_t event_handler;

   iolink_baudrate_t baudrate;
   uint8_t cycbyte;
   iolink_msequencetype_t mseq;

   os_mutex_t * mtx;
   os_thread_t * thread;
   os_event_t * event;
   uint32_t triggered_events;
   os_timer_t * timer;
   dl_timer_t timer_type;
   bool timer_elapsed;
   os_timer_t * timer_tcyc;
   bool timer_tcyc_elapsed;

   bool dataready;
   bool rxerror;
   bool txerror;
   bool rxtimeout;

#if IOLINK_HW == IOLINK_HW_MAX14819
   bool first_read_min_cycl;
   uint8_t devdly;
   uint8_t cqerr;
   uint8_t txbuffer[IOLINK_RXTX_BUFFER_SIZE];
   uint8_t rxbuffer[IOLINK_RXTX_BUFFER_SIZE];
   uint8_t tinitcyc;
#endif
} iolink_dl_t;

/*
 * DL-B services
 */
iolink_error_t DL_Read_req (iolink_port_t * port, uint8_t address);
iolink_error_t DL_Write_req (iolink_port_t * port, uint8_t address, uint8_t value);
iolink_error_t DL_SetMode_req (
   iolink_port_t * port,
   iolink_dl_mode_t mode,
   iolink_mode_vl_t * valuelist);
iolink_error_t DL_Write_Devicemode_req (
   iolink_port_t * port,
   iolink_dl_mode_t devicemode);
iolink_error_t DL_ReadParam_req (iolink_port_t * port, uint16_t address);
iolink_error_t DL_WriteParam_req (
   iolink_port_t * port,
   uint16_t address,
   uint8_t value);
iolink_error_t DL_ISDUTransport_req (
   iolink_port_t * port,
   iolink_isdu_vl_t * valuelist);
iolink_error_t DL_Control_req (
   iolink_port_t * port,
   iolink_controlcode_t controlcode);
iolink_error_t DL_EventConf_req (iolink_port_t * port);
iolink_error_t DL_PDOutputUpdate_req (iolink_port_t * port, uint8_t * outputdata);
iolink_error_t DL_PDOutputGet_req (
   iolink_port_t * port,
   uint8_t * len,
   uint8_t * data);

/*
 * Function interface
 */

/**
 * This function instantiates the data link layer for one port,
 * and starts a thread handling that port.
 *
 * @param port               Resulting port information struct
 * @param thread_prio        Priority of the thread
 * @param thread_stack_size  Stack size of the thread
 */
void iolink_dl_instantiate (
   iolink_port_t * port,
   unsigned int thread_prio,
   size_t thread_stack_size);

/**
 * This function resets the data link layer for one port to an initial state.
 *
 * @param port               Port information struct
 */
void iolink_dl_reset (iolink_port_t * port);

bool iolink_dl_get_pd_valid_status (iolink_port_t * port);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_DL_H */
