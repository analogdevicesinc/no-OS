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
 * @brief On-request Data Exchange layer
 *
 */

#ifndef IOLINK_ODE_H
#define IOLINK_ODE_H

#include "iolink_main.h" /* iolink_job_t */
#include "iolink_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum iolink_ode_state
{
   ODE_STATE_Inactive = 0,
   ODE_STATE_ODactive,
   ODE_STATE_ODblocked,
   ODE_STATE_LAST
} iolink_ode_state_t;

typedef struct iolink_ode_port
{
   iolink_ode_state_t state;
   iolink_smi_service_req_t smi_req;
   iolink_job_t * job_smi_req_busy;
} iolink_ode_port_t;

void iolink_ode_init (iolink_port_t * port);

iolink_error_t OD_Start (iolink_port_t * port);
iolink_error_t OD_Stop (iolink_port_t * port);

iolink_error_t ode_SMI_DeviceRead_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t ode_SMI_DeviceWrite_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t ode_SMI_ParamReadBatch_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t ode_SMI_ParamWriteBatch_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);
#ifdef __cplusplus
}
#endif

#endif /* IOLINK_ODE_H */
