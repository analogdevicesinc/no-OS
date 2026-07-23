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
 * @brief Process Data Exchange layer
 *
 */

#ifndef IOLINK_PDE_H
#define IOLINK_PDE_H

#include "iolink_main.h" /* iolink_job_t */
#include "iolink_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PDE FSM states
 */
typedef enum iolink_pde_state
{
   PD_STATE_Inactive = 0,
   PD_STATE_PDactive,
   PD_STATE_LAST
} iolink_pde_state_t;

typedef struct iolink_pde_port
{
   iolink_pde_state_t state;
   bool output_enable;
} iolink_pde_port_t;

void iolink_pde_init (iolink_port_t * port);

iolink_error_t PD_Start (iolink_port_t * port);
iolink_error_t PD_Stop (iolink_port_t * port);
iolink_error_t AL_NewInput_ind (iolink_port_t * port);
iolink_error_t AL_Control_ind (
   iolink_port_t * port,
   iolink_controlcode_t controlcode);

iolink_error_t pde_SMI_PDIn_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t pde_SMI_PDOut_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);

iolink_error_t pde_SMI_PDInOut_req (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);
#ifdef __cplusplus
}
#endif

#endif /* IOLINK_PDE_H */
