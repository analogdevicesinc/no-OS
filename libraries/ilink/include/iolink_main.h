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
 * @brief Handler
 *
 */

#ifndef IOLINK_MAIN_H
#define IOLINK_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "iolink_types.h"
#include "iolink.h"

#include <stdint.h>

/**
 * Structure containing all information for one IO-Link port.
 */
typedef struct iolink_port iolink_port_t;

typedef enum iservice
{
   IOL_ISERVICE_MASTER_NO_SERVICE         = 0b0000,
   IOL_ISERVICE_DEVICE_NO_SERVICE         = 0b0000,
   IOL_ISERVICE_MASTER_WRITE_8I           = 0b0001,
   IOL_ISERVICE_MASTER_WRITE_8I_8SI       = 0b0010,
   IOL_ISERVICE_MASTER_WRITE_16I_16SI     = 0b0011,
   IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG = 0b0100,
   IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS = 0b0101,
   IOL_ISERVICE_MASTER_READ_8I            = 0b1001,
   IOL_ISERVICE_MASTER_READ_8I_8SI        = 0b1010,
   IOL_ISERVICE_MASTER_READ_16I_16SI      = 0b1011,
   IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG  = 0b1100,
   IOL_ISERVICE_DEVICE_READ_RESPONSE_POS  = 0b1101,
   IOL_ISERVICE_MAX                       = 0b1111,
} iservice_t;

typedef struct iolink_smi_service_req
{
   iolink_arg_block_id_t exp_arg_block_id;
   uint16_t arg_block_len;
   arg_block_t * arg_block;
   iolink_smi_errortypes_t result;
} iolink_smi_service_req_t;

typedef enum iolink_job_type
{
   IOLINK_JOB_NONE,
   IOLINK_JOB_PD_EVENT,
   IOLINK_JOB_SM_OPERATE_REQ,
   IOLINK_JOB_SM_SET_PORT_CFG_REQ,
   IOLINK_JOB_DL_MODE_IND,
   IOLINK_JOB_DL_READ_CNF,
   IOLINK_JOB_DL_WRITE_CNF,
   IOLINK_JOB_DL_WRITE_DEVMODE_CNF,
   IOLINK_JOB_DL_READPARAM_CNF,
   IOLINK_JOB_DL_WRITEPARAM_CNF,
   IOLINK_JOB_DL_ISDU_TRANS_CNF,
   IOLINK_JOB_DL_PDINPUT_TRANS_IND,
   IOLINK_JOB_SM_PORT_MODE_IND,
   IOLINK_JOB_DL_EVENT_IND,
   IOLINK_JOB_DL_CONTROL_IND,
   IOLINK_JOB_DS_STARTUP,
   IOLINK_JOB_DS_DELETE,
   IOLINK_JOB_DS_INIT,
   IOLINK_JOB_DS_UPLOAD,
   IOLINK_JOB_DS_READY,
   IOLINK_JOB_DS_CHANGE,
   IOLINK_JOB_DS_FAULT,
   IOLINK_JOB_OD_START,
   IOLINK_JOB_OD_STOP,
   IOLINK_JOB_AL_CONTROL_CNF,
   IOLINK_JOB_AL_READ_REQ,
   IOLINK_JOB_AL_READ_CNF,
   IOLINK_JOB_AL_WRITE_CNF,
   IOLINK_JOB_AL_WRITE_REQ,

   IOLINK_JOB_AL_ABORT,
   IOLINK_JOB_AL_EVENT_RSP,
   IOLINK_JOB_AL_EVENT_REQ,
   IOLINK_JOB_SMI_MASTERIDENT,
   IOLINK_JOB_SMI_PORTCONFIGURATION,
   IOLINK_JOB_SMI_READBACKPORTCONFIGURATION,
   IOLINK_JOB_SMI_PORTSTATUS,
   IOLINK_JOB_SMI_DEVICE_WRITE,
   IOLINK_JOB_SMI_DEVICE_READ,
   IOLINK_JOB_SMI_PARAM_READ,
   IOLINK_JOB_SMI_PARAM_WRITE,

   IOLINK_JOB_PERIODIC,

   IOLINK_JOB_EXIT,
} iolink_job_type_t;

/** Generic job */
typedef struct iolink_job
{
   iolink_job_type_t type;
   iolink_port_t * port;
   void (*callback) (struct iolink_job * job);
   union
   {
      iolink_mhmode_t dl_mode;
      struct
      {
         uint8_t data_len;
         const uint8_t * data;
      } pd_event;
      struct
      {
         iolink_smp_parameterlist_t paramlist;
      } sm_setportcfg_req;
      struct
      {
         uint8_t addr;
         uint8_t val; /* Only used for Read */
         iolink_status_t stat;
         iservice_t qualifier;
      } dl_rw_cnf;
      struct
      {
         const uint8_t * data;
         uint8_t data_len;
         iolink_smi_errortypes_t errortype;
      } al_read_cnf;
      struct
      {
         iolink_smi_errortypes_t errortype;
      } al_write_cnf;
      struct
      {
         uint16_t index;
         uint8_t subindex;
         uint8_t val;
         void (*al_read_cb) (
            iolink_port_t * port,
            uint8_t len,
            const uint8_t * data,
            iolink_smi_errortypes_t errortype);
      } al_read_req;
      struct
      {
         uint16_t index;
         uint8_t subindex;
         uint8_t length;
         const uint8_t * data;
         void (*al_write_cb) (
            iolink_port_t * port,
            iolink_smi_errortypes_t errortype);
      } al_write_req;
      struct
      {
         iolink_controlcode_t controlcode;
      } dl_control_ind;
      struct
      {
         iolink_status_t errorinfo;
         iolink_dl_mode_t mode;
      } dl_write_devicemode_cnf;
      struct
      {
         diag_entry_t event;
         uint8_t eventsleft;
      } dl_event_ind;
      struct
      {
         iolink_ds_fault_t fault;
      } ds_fault;
      struct
      {
         iolink_sm_portmode_t mode;
      } sm_port_mode_ind;
      struct
      {
         portconfiglist_t portcfg;
      } ds_init;
      iolink_smi_service_req_t smi_req;
   };
} iolink_job_t;

typedef struct
{
   iolink_mhmode_t mh_mode;
   iolink_mhmode_t comrate;
} iolink_sm_temp_t;

typedef struct iolink_al_port iolink_al_port_t;
typedef struct iolink_cm_port iolink_cm_port_t;
typedef struct iolink_ds_port iolink_ds_port_t;
typedef struct iolink_ode_port iolink_ode_port_t;
typedef struct iolink_pde_port iolink_pde_port_t;
typedef struct iolink_pl_port iolink_pl_port_t;
typedef struct iolink_sm_port iolink_sm_port_t;
typedef struct iolink_dl iolink_dl_t;

typedef struct iolink_port_info
{
   uint32_t deviceid;
   uint16_t vendorid;
   uint16_t functionid;
   uint8_t revisionid;
   uint8_t cycletime;
   iolink_port_quality_info_t port_quality_info;
   iolink_port_status_info_t port_status_info;
   iolink_transmission_rate_t transmission_rate;
   uint8_t serialnumber[16];
} iolink_port_info_t;

iolink_job_t * iolink_fetch_avail_job (iolink_port_t * port);
iolink_job_t * iolink_fetch_avail_api_job (iolink_port_t * port);
#ifdef UNIT_TEST
bool iolink_post_job (iolink_port_t * port, iolink_job_t * job);
#else
void iolink_post_job_with_type_and_callback (
   iolink_port_t * port,
   iolink_job_t * job,
   iolink_job_type_t type,
   void (*callback) (struct iolink_job * job));
#endif
bool iolink_post_job_pd_event (
   iolink_port_t * port,
   uint32_t timeout,
   uint8_t data_len,
   const uint8_t * data);

void iolink_smi_cnf (
   iolink_port_t * port,
   iolink_arg_block_id_t ref_arg_block_id,
   uint16_t arg_block_len,
   arg_block_t * arg_block);
void iolink_smi_voidblock_cnf (
   iolink_port_t * port,
   iolink_arg_block_id_t ref_arg_block_id);
void iolink_smi_joberror_ind (
   iolink_port_t * port,
   iolink_arg_block_id_t exp_arg_block_id,
   iolink_arg_block_id_t ref_arg_block_id,
   iolink_smi_errortypes_t error);

/**
 * Get port information
 *
 * This function returns the port information struct for the given port.
 *
 * @param master           Master information struct
 * @param portnumber       Number of the port
 * @return                 Pointer to the port information struct
 */
iolink_port_t * iolink_get_port (iolink_m_t * master, uint8_t portnumber);

uint8_t iolink_get_portnumber (iolink_port_t * port);

uint8_t iolink_get_port_cnt (iolink_port_t * port);

iolink_port_info_t * iolink_get_port_info (iolink_port_t * port);

const iolink_smp_parameterlist_t * iolink_get_paramlist (iolink_port_t * port);

iolink_transmission_rate_t iolink_get_transmission_rate (iolink_port_t * port);

iolink_al_port_t * iolink_get_al_ctx (iolink_port_t * port);
iolink_cm_port_t * iolink_get_cm_ctx (iolink_port_t * port);

/**
 * Get the data link layer context
 *
 * @param port           Port information
 * @return               Pointer to data link layer struct
 */
iolink_dl_t * iolink_get_dl_ctx (iolink_port_t * port);

iolink_ds_port_t * iolink_get_ds_ctx (iolink_port_t * port);
iolink_ode_port_t * iolink_get_ode_ctx (iolink_port_t * port);
iolink_pde_port_t * iolink_get_pde_ctx (iolink_port_t * port);
iolink_pl_port_t * iolink_get_pl_ctx (iolink_port_t * port);
iolink_sm_port_t * iolink_get_sm_ctx (iolink_port_t * port);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_MAIN_H */
