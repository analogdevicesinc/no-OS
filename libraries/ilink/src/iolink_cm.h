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
 * @brief Configuration Manager
 *
 */

#ifndef IOLINK_CM_H
#define IOLINK_CM_H

#include "iolink_types.h"
#include "iolink_main.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * CM FSM states
 */
typedef enum iolink_cm_state
{
   CM_STATE_CheckPortMode = 0,
   CM_STATE_SM_Startup,
   CM_STATE_DS_ParamManager,
   CM_STATE_PortFault,
   CM_STATE_WaitingOnOperate,
   CM_STATE_Port_Active,
   CM_STATE_Port_DIDO,
   CM_STATE_Port_Deactivated,
   CM_STATE_LAST,
} iolink_cm_state_t;

typedef struct iolink_cm_port
{
   iolink_cm_state_t state;
   iolink_smi_service_req_t smi_req;
   portconfiglist_t cfg_list;
   iolink_ds_fault_t ds_fault;
} iolink_cm_port_t;

/**
 * CM state-machine events.
 *
 * CM events trigger state transitions in the CM FSM.
 */
typedef enum iolink_fsm_cm_event
{
   CM_EVENT_NONE,
   CM_EVENT_IOL_MANUAL,    /* T1 */
   CM_EVENT_IOL_AUTOSTART, /* T2 */
   CM_EVENT_SM_COMREADY,   /* T3 */
   CM_EVENT_SM_FAULT,      /* T4 */
   CM_EVENT_DS_Ready,      /* T5 */
   CM_EVENT_DS_Fault,      /* T6 */
   CM_EVENT_SM_OPERATE,    /* T7 */
   CM_EVENT_SM_COMLOST,    /* T8 */
   CM_EVENT_DI_CQ,
   /* T9 */ // TODO merge
   CM_EVENT_DO_CQ,
   /* T10 */               // TODO merge
   CM_EVENT_DEACTIVATED,   /* T11 */
   CM_EVENT_CFG_CHANGE,    /* T12, T13 and T15 */
   CM_EVENT_DS_CHANGE,     /* T14 */
   CM_EVENT_UNKNOWN,       /* T16 */
   CM_EVENT_SM_COMLOST_DS, /* T8 */
   CM_EVENT_LAST
} iolink_fsm_cm_event_t;

void iolink_cm_init (iolink_port_t * port);

void SM_PortMode_ind (iolink_port_t * port, iolink_sm_portmode_t mode);

void DS_Ready (iolink_port_t * port);
void DS_Change (iolink_port_t * port);
void DS_Fault (iolink_port_t * port, iolink_ds_fault_t fault);

iolink_error_t cm_SMI_MasterIdentification_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t cm_SMI_PortConfiguration_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t cm_SMI_ReadbackPortConfiguration_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t cm_SMI_PortStatus_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_CM_H */
