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
 * @brief System Management layer
 *
 */

#ifndef IOLINK_SM_H
#define IOLINK_SM_H

#include "iolink_main.h"
#include "iolink_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Direct Parameter page, see IO-Link Interface Spec v1.1.3 chapter B.1.1 */
#define IOL_DIR_PARAMA_MASTER_CMD  0x00
#define IOL_DIR_PARAMA_MASTER_CYCL 0x01
#define IOL_DIR_PARAMA_MIN_CYCL    0x02
#define IOL_DIR_PARAMA_MSEQ_CAP    0x03
#define IOL_DIR_PARAMA_REV_ID      0x04
#define IOL_DIR_PARAMA_PDI         0x05
#define IOL_DIR_PARAMA_PDO         0x06
#define IOL_DIR_PARAMA_VID_1       0x07 /* MSB */
#define IOL_DIR_PARAMA_VID_2       0x08 /* LSB */
#define IOL_DIR_PARAMA_DID_1       0x09 /* MSB */
#define IOL_DIR_PARAMA_DID_2       0x0A
#define IOL_DIR_PARAMA_DID_3       0x0B /* LSB */
#define IOL_DIR_PARAMA_FID_1       0x0C /* MSB */
#define IOL_DIR_PARAMA_FID_2       0x0D /* LSB */
#define IOL_DIR_PARAMA_SYS_CMD     0x0F
#define IOL_DIR_PARAMA_DUMMY_WURQ  0xFF /* Dummy used for first DL_Read_cnf */

/* MasterCommand, see IO-Link Interface Spec v1.1.3 chapter B.1.2 */
#define IOL_MASTERCMD_NONE             0x00
#define IOL_MASTERCMD_FALLBACK         0x5A
#define IOL_MASTERCMD_MASTER_IDENT     0x95
#define IOL_MASTERCMD_DEVICE_IDENT     0x96
#define IOL_MASTERCMD_DEVICE_STARTUP   0x97
#define IOL_MASTERCMD_DEVICE_PD_OUT_OP 0x98
#define IOL_MASTERCMD_DEVICE_OPERATE   0x99
#define IOL_MASTERCMD_DEVICE_PREOP     0x9A

/* Device parameter (Index assignment of data objects),
 * see IO-Link Interface Spec v1.1.3 chapter B.2.1
 */
#define IOL_DEV_PARAMA_DIRECT_PAR_P1   0x0000
#define IOL_DEV_PARAMA_DIRECT_PAR_P2   0x0001
#define IOL_DEV_PARAMA_SYSTEM_CMD      0x0002
#define IOL_DEV_PARAMA_DATA_STOREAGE   0x0003
#define IOL_DEV_PARAMA_DEV_ACCESS_LOCK 0x000C
#define IOL_DEV_PARAMA_VENDOR_NAME     0x0010
#define IOL_DEV_PARAMA_VENDOR_TEXT     0x0011
#define IOL_DEV_PARAMA_PRODUCT_NAME    0x0012
#define IOL_DEV_PARAMA_PRODUCT_ID      0x0013
#define IOL_DEV_PARAMA_PRODUCT_TEXT    0x0014
#define IOL_DEV_PARAMA_SERIAL_NUMBER   0x0015
#define IOL_DEV_PARAMA_HARDWARE_REV    0x0016
#define IOL_DEV_PARAMA_FIRMWARE_REV    0x0017
#define IOL_DEV_PARAMA_ERROR_COUNT     0x0020
#define IOL_DEV_PARAMA_DEVICE_STATUS   0x0024

typedef enum iolink_sm_state
{
   SM_STATE_PortInactive = 0,
   // SM_STATE_checkCompatibility,
   SM_STATE_waitonDLPreoperate,
   SM_STATE_checkSerNum,
   SM_STATE_wait,
   SM_STATE_SMOperate,
   SM_STATE_InspectionFault,
   SM_STATE_waitonDLOperate,
   SM_STATE_DIDO,
   SM_STATE_ReadComParameter,
   SM_STATE_CheckCompV10,
   SM_STATE_CheckVxy,
   SM_STATE_CheckComp,
   SM_STATE_RestartDevice,
   SM_STATE_waitForFallback,   /* Not in spec. */
   SM_STATE_write_master_cycl, /* Not in spec. */
   SM_STATE_wait_devmode_cnf,  /* NOt in spec. */
   SM_STATE_LAST
} iolink_sm_state_t;

/**
 * SM state-machine events.
 *
 * SM events trigger state transitions.
 */
// TODO: rename
typedef enum iolink_fsm_sm_event
{
   SM_EVENT_NONE = 0,
   SM_EVENT_DL_Mode_STARTUP,                    /* T1 */
   SM_EVENT_CompOK,                             /* T2 */
   SM_EVENT_DL_Mode_COMLOST,                    /* T3 */
   SM_EVENT_V10CompOK,                          /* T4 */
   SM_EVENT_V10CompFault,                       /* T5 */
   SM_EVENT_RevisionFault,                      /* T6 */
   SM_EVENT_CompFault,                          /* T7 */
   SM_EVENT_DL_Mode_PREOPERATE,                 /* T8 */
   SM_EVENT_DL_Mode_OPERATE,                    /* T9 and T13 */
   SM_EVENT_SerNumOK,                           /* T10 */
   SM_EVENT_SerNumFault,                        /* T11 */
   SM_EVENT_SM_Operate,                         /* T12 */
   SM_EVENT_SM_Operate_v10,                     /* T12 for v1.0 device*/
   SM_EVENT_SM_SetPortConfig_INACTIVE,          /* T14 */
   SM_EVENT_SM_SetPortConfig_CFGCOM_or_AUTOCOM, /* T15 */
   SM_EVENT_SM_SetPortConfig_DI_or_DO,          /* T16 */
   SM_EVENT_CycTimeFault,                       /* T17 */
   SM_EVENT_CycTimeFault_V10,                   /* T18 */
   SM_EVENT_V10,                                /* T20 */
   SM_EVENT_NOT_V10,                            /* T21 */
   SM_EVENT_REVISION_OK,                        /* T22 */
   SM_EVENT_RETRY_STARTUP,                      /* T23 and T25 */
   SM_EVENT_WriteDone,                          /* T24 */
   SM_EVENT_CNF_COMLOST, /* T3, COMLOST when waiting on DL_{Read,Write}_cnf() or
                            AL_Read_cnf() */
   SM_EVENT_WRITE_MASTER_CYCL_REQ,  /* Not in spec */
   SM_EVENT_WRITE_MASTER_CYCL_DONE, /* Not in spec */
   SM_EVENT_LAST,
} iolink_fsm_sm_event_t;

typedef struct iolink_sm_port
{
   iolink_sm_state_t state;
   iolink_mhmode_t mh_mode;
   iolink_mhmode_t comrate;
   iolink_smp_parameterlist_t config_paramlist;
   iolink_smp_parameterlist_t real_paramlist;
   iolink_dev_com_t dev_com;
   uint8_t dl_addr;
   uint8_t CompRetry;
   iolink_fsm_sm_event_t error_event;
} iolink_sm_port_t;

void DL_Mode_ind_baud (iolink_port_t * port, iolink_mhmode_t realmode);
void DL_Mode_ind (iolink_port_t * port, iolink_mhmode_t realmode);
iolink_error_t SM_Operate (iolink_port_t * port);
iolink_error_t SM_SetPortConfig_req (
   iolink_port_t * port,
   iolink_smp_parameterlist_t * parameterlist);
void DL_Read_cnf (iolink_port_t * port, uint8_t value, iolink_status_t errorinfo);
void DL_Write_cnf (iolink_port_t * port, iolink_status_t errorinfo);

void DL_Write_Devicemode_cnf (
   iolink_port_t * port,
   iolink_status_t errorinfo,
   iolink_dl_mode_t devicemode);

/**
 * Initialise SM state-machine
 *
 * This function initialises the SM state-machine for one IO-Link port and
 * should be called when the stack is started.
 *
 * @param port           port handle
 */
void iolink_sm_init (iolink_port_t * port);

#ifdef __cplusplus
}
#endif

#endif /* IOLINK_SM_H */
