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
 * @brief Application layer
 *
 */

#ifndef IOLINK_AL_H
#define IOLINK_AL_H

#include "iolink_main.h" /* iolink_job_t */
#include "iolink.h"
#include "iolink_types.h"
#include "iolink_options.h" /* IOLINK_MAX_EVENTS */
#include "osal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum iolink_al_od_state
{
   AL_OD_STATE_OnReq_Idle = 0,
   AL_OD_STATE_Build_DL_Service,
   AL_OD_STATE_Await_DL_param_cnf,
   AL_OD_STATE_Await_DL_ISDU_cnf,
   AL_OD_STATE_LAST
} iolink_al_od_state_t;

typedef enum iolink_al_event_state
{
   AL_EVENT_STATE_Event_inactive = 0,
   AL_EVENT_STATE_Event_idle,
   AL_EVENT_STATE_Read_Event_Set,
   AL_EVENT_STATE_DU_Event_handling,
   AL_EVENT_STATE_LAST
} iolink_al_event_state_t;

typedef struct iolink_al_port
{
   iolink_al_od_state_t od_state;
   iolink_al_event_state_t event_state;

   uint8_t pdin_data[IOLINK_PD_MAX_SIZE];
   uint8_t pdin_data_len;
   os_mutex_t * mtx_pdin;
   struct
   {
      uint16_t index;
      uint8_t subindex;
      iolink_rwdirection_t direction;
      uint8_t data_read[IOLINK_ISDU_MAX_SIZE];
      const uint8_t * data_write;
      uint8_t data_len;
      iolink_status_t errinfo;
      iservice_t qualifier;
      iolink_job_t * job_req;
      union
      {
         void (*al_read_cnf_cb) (
            iolink_port_t * port,
            uint8_t len,
            const uint8_t * data,
            iolink_smi_errortypes_t errortype);
         void (*al_write_cnf_cb) (
            iolink_port_t * port,
            iolink_smi_errortypes_t errortype);
      };
   } service;

   struct
   {
      uint8_t event_cnt;
      diag_entry_t events[IOLINK_MAX_EVENTS];
   } event;
} iolink_al_port_t;

/**
 * AL OD state-machine events.
 *
 * AL events trigger state transitions in the OD FSM.
 */
typedef enum iolink_fsm_al_od_event
{
   AL_OD_EVENT_NONE = 0,
   AL_OD_EVENT_service,           /* T1, T16, T8 and T12 */
   AL_OD_EVENT_arg_err,           /* T2 */
   AL_OD_EVENT_param_read,        /* T3 */
   AL_OD_EVENT_param_write_1,     /* T4 */
   AL_OD_EVENT_param_write_2,     /* T5 */
   AL_OD_EVENT_isdu_read,         /* T6 */
   AL_OD_EVENT_isdu_write,        /* T7 */
   AL_OD_EVENT_abort,             /* T9 and T11 */
   AL_OD_EVENT_rwparam,           /* T10 */
   AL_OD_EVENT_readparam_cnf,     /* T13 */
   AL_OD_EVENT_writeparam_cnf,    /* T14 */
   AL_OD_EVENT_isdutransport_cnf, /* T15 */
   AL_OD_EVENT_abort_port,        /* T17 */
   AL_OD_EVENT_LAST
} iolink_fsm_al_od_event_t;

/**
 * AL Event state-machine events.
 *
 * AL events trigger state transitions in the Event FSM.
 */
typedef enum iolink_fsm_al_event_event
{
   AL_EVENT_EVENT_NONE = 0,
   AL_EVENT_EVENT_dl_event_ind_more, /* T3 or T4 */
   AL_EVENT_EVENT_dl_event_ind_done, /* T5 or T3 + T5 */
   AL_EVENT_EVENT_al_event_rsp,      /* T6 */
   AL_EVENT_EVENT_al_event_req,      /* T7 */
   AL_EVENT_EVENT_LAST
} iolink_fsm_al_event_event_t;

iolink_error_t AL_Read_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   void (*al_read_cnf_cb) (
      iolink_port_t * port,
      uint8_t len,
      const uint8_t * data,
      iolink_smi_errortypes_t errortype));
iolink_error_t AL_Write_req (
   iolink_port_t * port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   const uint8_t * data,
   void (*al_write_cnf_cb) (
      iolink_port_t * port,
      iolink_smi_errortypes_t errortype));
iolink_error_t AL_SetOutput_req (iolink_port_t * port, uint8_t * data);
iolink_error_t AL_Control_req (
   iolink_port_t * port,
   iolink_controlcode_t controlcode);
iolink_error_t AL_GetInput_req (iolink_port_t * port, uint8_t * len, uint8_t * data);
iolink_error_t AL_GetInputOutput_req (
   iolink_port_t * port,
   uint8_t * len,
   uint8_t * data);
void DL_Event_ind (
   iolink_port_t * port,
   uint16_t eventcode,
   uint8_t event_qualifier,
   uint8_t eventsleft);
void DL_Control_ind (iolink_port_t * port, iolink_controlcode_t controlcode);
void DL_ReadParam_cnf (iolink_port_t * port, uint8_t value, iolink_status_t errinfo);
void DL_WriteParam_cnf (iolink_port_t * port, iolink_status_t errinfo);
void DL_ISDUTransport_cnf (
   iolink_port_t * port,
   uint8_t * data,
   uint8_t length,
   iservice_t qualifier,
   iolink_status_t errinfo);
void DL_PDInputTransport_ind (
   iolink_port_t * port,
   uint8_t * inputdata,
   uint8_t length);
void AL_Event_ind (
   iolink_port_t * port,
   uint8_t event_cnt,
   diag_entry_t events[6]); // TODO where to put this?
iolink_error_t AL_Abort (iolink_port_t * port);
iolink_error_t AL_Event_rsp (iolink_port_t * port);

void iolink_al_init (iolink_port_t * port);

#ifdef UNIT_TEST
// TODO: A more logical location for this function is in iolink_main, but for
// some
//       unknown reason will some unit tests fail when placing it there??
void iolink_post_job_with_type_and_callback (
   iolink_port_t * port,
   iolink_job_t * job,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job));
#endif

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_AL_H */
