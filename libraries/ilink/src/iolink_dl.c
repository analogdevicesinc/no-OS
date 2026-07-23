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

#include "iolink_dl.h"
/* DL_Mode_ind_baud, DL_Mode_ind, DL_Read_cnf, DL_Write_cnf */
#include "iolink_sm.h"
/* DL_Control_ind, DL_Event_ind, DL_PDInputTransport_ind, DL_ReadParam_cnf,
 * DL_WriteParam_cnf, DL_ISDUTransport_cnf
 */
#include "iolink_al.h"
#include "iolink_pl.h"
/* iolink_pl_init_sdci, iolink_pl_get_data, iolink_pl_get_error,
 * iolink_pl_get_baudrate, iolink_pl_get_cycletime, iolink_pl_handler,
 * iolink_configure_pl_event,
 * PL_DisableCycleTimer, PL_Transfer_req, PL_Resend, PL_EnableCycleTimer,
 * PL_MessageDownload_req
 */
#include "osal_log.h"

#include "iolink_main.h" /* iolink_get_portnumber */
#include "osal.h"

#include <string.h> /* memset */
#include <stdio.h>  /* snprintf */
#include <sys/time.h>

// TODO: Events in preop
// TODO: ISDU Timer
// TODO: Test doing a command while fetching an event

/**
 * @file
 * @brief Data link layer
 *
 */

#define T_BIT_COM1_NS             208330
#define T_BIT_COM2_NS             26040
#define T_BIT_COM3_NS             4340
#define IOLINK_ISDU_RESPONSE_TIME (5000 * 1000)

#define IOLINK_MAX_RETRY 2

#define IOLINK_DL_THREAD_NAME_LENGTH 12

static char dl_thread_names[IOLINK_NUM_PORTS][IOLINK_DL_THREAD_NAME_LENGTH];

static const char * const iolink_dl_mh_st_literals[] = {
   "INACTIVE_0",
   "AW_REPLY_1",
   "STARTUP_2",
   "RESPONSE_3",
   "AW_REPLY_4",
   "ERRORHANDLING_5",
   "PREOPERATE_6",
   "GETOD_7",
   "RESPONSE_8",
   "AW_REPLY_9",
   "ERRORHANDLING_10",
   "CHECKHANDLER_11",
   "OPERATE_12",
   "GETPD_13",
   "GETOD_14",
   "RESPONSE_15",
   "AW_REPLY_16",
   "ERRORHANDLING_17",
};

static uint32_t get_T_initcyc (iolink_dl_t * dl);
static uint8_t calcCHKPDU (uint8_t * data, uint8_t length);
static iolink_controlcode_t getCKSPDIn (iolink_dl_t * dl);
static bool getCKSEvFlag (iolink_dl_t * dl);
static bool valid_isdu_checksum (iolink_dl_t * dl);
static iservice_t get_isdu_iserv (iolink_dl_t * dl);
static uint8_t get_isdu_len (iolink_dl_t * dl);
static bool get_isdu_total_len (iolink_dl_t * dl, uint8_t * isdu_total_len);
static bool get_isdu_data_length (iolink_dl_t * dl, uint8_t * isdu_data_length);
static uint8_t * get_isdu_response_data (iolink_dl_t * dl);
static uint8_t get_isdu_total_segments (iolink_dl_t * dl);
static bool valid_isdu_header (iolink_dl_t * dl);
static void start_timer_initcyc (iolink_dl_t * dl);
static void isdu_timer_reset (iolink_dl_t * dl);
static void isdu_timer_start (iolink_dl_t * dl);
static bool isdu_timer_elapsed (iolink_dl_t * dl);
static void interleave_reset_od_sizes (iolink_dl_t * dl);
static void set_mseq_change (iolink_dl_t * dl);
static void CH_Conf (iolink_port_t * port, CHCmd_t cmd);
static void OH_Conf (iolink_port_t * port, OHCmd_t cmd);
static void PH_Conf (iolink_port_t * port, PHCmd_t cmd);
static void IH_Conf (iolink_port_t * port, IHCmd_t cmd);
static void EH_Conf (iolink_port_t * port, EHCmd_t cmd);
static void MH_Conf (iolink_port_t * port, MHCmd_t cmd);
static void set_OH_IH_EH_Conf_active (iolink_port_t * port, bool active);
static void write_master_command (iolink_port_t * port, iolink_status_t errorinfo);
static void iolink_dl_mh_handle_com_lost (iolink_port_t * port);
static iolink_error_t OD_req (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection,
   iolink_comchannel_t comchannel,
   uint8_t addressctrl,
   uint8_t length,
   uint8_t * data);
static iolink_error_t PD_req (
   iolink_port_t * port,
   uint8_t pdinaddress,
   uint8_t pdinlength,
   uint8_t * pdout,
   uint8_t pdoutaddress,
   uint8_t pdoutlength);
static iolink_error_t OD_cnf (
   iolink_dl_t * dl,
   uint8_t * data,
   uint8_t length,
   iolink_status_t errorinfo);
static iolink_error_t PDInStatus_ind (
   iolink_port_t * port,
   iolink_controlcode_t status);
static iolink_error_t EventFlag_ind (iolink_dl_t * dl, bool eventflag);
static iolink_error_t MHInfo_ind (iolink_port_t * port, iolink_mhinfo_t mhinfo);
static iolink_error_t DL_ReadWriteParam_req (
   iolink_port_t * port,
   uint16_t address,
   uint8_t value,
   bool write);
static void dl_timer_timeout (os_timer_t * timer, void * arg);
static void dl_timer_tcyc_timeout (os_timer_t * timer, void * arg);
static void iolink_dl_wurq_recv (iolink_port_t * port);
static void iolink_dl_handle_error (iolink_port_t * port);
static void iolink_dl_handle_reset (iolink_port_t * port);
static void dl_main (void * arg);

/* State machine of the Master DL-mode handler */
static void iolink_dl_mode_h_sm (iolink_port_t * port);
static void iolink_dl_mode_h_sm_idle0 (iolink_port_t * port);
static void iolink_dl_mode_h_sm_establish_com1 (void);
static void iolink_dl_mode_h_sm_startup2 (iolink_port_t * port);
static void iolink_dl_mode_h_sm_preoperate3 (iolink_port_t * port);
static void iolink_dl_mode_h_sm_operate4 (iolink_port_t * port);
static void iolink_dl_mode_h_sm_goto_idle (
   iolink_port_t * port,
   iolink_mhmode_t mode);
static void iolink_dl_mode_h_sm_goto_startup (iolink_port_t * port);
static void iolink_dl_mode_h_sm_goto_operate (iolink_port_t * port);

/* State machine of the Master message handler */
static void iolink_dl_message_h_sm (iolink_port_t * port);
static void iolink_dl_message_h_sm_inactive0 (iolink_port_t * port);
static void iolink_dl_message_h_sm_await_reply1 (iolink_port_t * port);
static void iolink_dl_message_h_sm_startup2 (iolink_port_t * port);
static void iolink_dl_message_h_sm_response3 (iolink_port_t * port);
static void iolink_dl_message_h_sm_await_reply4 (iolink_port_t * port);
static void iolink_dl_message_h_sm_error_handling5 (iolink_port_t * port);
static void iolink_dl_message_h_sm_preoperate6 (iolink_port_t * port);
static void iolink_dl_message_h_sm_get_od7 (iolink_port_t * port);
static void iolink_dl_message_h_sm_response8 (iolink_port_t * port);
static void iolink_dl_message_h_sm_await_reply9 (iolink_port_t * port);
static void iolink_dl_message_h_sm_error_handling10 (iolink_port_t * port);
static void iolink_dl_message_h_sm_check_handler11 (void);
static void iolink_dl_message_h_sm_operate12 (iolink_port_t * port);
static void iolink_dl_message_h_sm_get_pd13 (iolink_port_t * port);
static void iolink_dl_message_h_sm_get_od14 (iolink_port_t * port);
static void iolink_dl_message_h_sm_response15 (iolink_port_t * port);
static void iolink_dl_message_h_sm_await_reply16 (iolink_port_t * port);
static void iolink_dl_message_h_sm_error_handling17 (iolink_port_t * port);

/* State machine of the Master Process Data handler */
static void iolink_dl_pd_h_sm (iolink_port_t * port);
static void iolink_dl_pd_h_sm_inactive0 (iolink_port_t * port);
static void iolink_dl_pd_h_sm_pd_single1 (iolink_port_t * port);
static void iolink_dl_pd_h_sm_pd_in_interleave2 (iolink_port_t * port);
static void iolink_dl_pd_h_sm_pd_out_interleave3 (iolink_port_t * port);

/* State machine of the Master On-request Data handler */
static void iolink_dl_od_h_sm (iolink_port_t * port);
static void iolink_dl_od_h_sm_inactive0 (iolink_port_t * port);
static void iolink_dl_od_h_sm_isdu1 (iolink_port_t * port);
static void iolink_dl_od_h_sm_command2 (iolink_port_t * port);
static void iolink_dl_od_h_sm_event3 (iolink_port_t * port);

/* State machine of the Master ISDU handler */
static void iolink_dl_isdu_h_sm (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_inactive0 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_idle1 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_isdu_request2 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_isdu_wait3 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_isdu_error4 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_isdu_response5 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_enter_isduerror4 (iolink_port_t * port);
static void iolink_dl_isdu_h_sm_reception_complete (iolink_port_t * port);

/* State machine of the Master command handler */
static void iolink_dl_cmd_h_sm (iolink_port_t * port);

/* State machine of the Master Event handler */
static void iolink_dl_ev_h_sm (iolink_port_t * port);
static void iolink_dl_ev_h_sm_inactive0 (iolink_port_t * port);
static void iolink_dl_ev_h_sm_idle1 (iolink_port_t * port);
static void iolink_dl_ev_h_sm_read_event2 (iolink_port_t * port);
static void iolink_dl_ev_h_sm_read_event2_for_device_status_code (iolink_port_t * port);
static void iolink_dl_ev_h_sm_read_event2_for_device_event_memory_byte (iolink_port_t * port);
static void iolink_dl_ev_h_sm_signal_event3 (iolink_port_t * port);

static uint32_t get_T_initcyc (iolink_dl_t * dl)
{
   uint32_t us = 0;

   switch (dl->baudrate)
   {
   case IOLINK_BAUDRATE_COM1:
      us = 100 * T_BIT_COM1_NS;
      break;
   case IOLINK_BAUDRATE_COM2:
      us = 100 * T_BIT_COM2_NS;
      break;
   case IOLINK_BAUDRATE_COM3:
      us = 100 * T_BIT_COM3_NS;
      break;
   default:
      break;
   }

   return (us / 1000) + 1;
}

static uint8_t calcCHKPDU (uint8_t * data, uint8_t length)
{
   uint8_t idx, chksum = 0;

   if (length > 0)
   {
      chksum = data[0];

      for (idx = 1; idx < length; idx++)
      {
         chksum ^= data[idx];
      }
   }

   return chksum;
}

static iolink_controlcode_t getCKSPDIn (iolink_dl_t * dl)
{
   return ((dl->message_handler.cks & BIT (6)) != 0) ? IOLINK_CONTROLCODE_INVALID
                                                     : IOLINK_CONTROLCODE_VALID;
}

static bool getCKSEvFlag (iolink_dl_t * dl)
{
   return ((dl->message_handler.cks & BIT (7)) != 0);
}

static bool valid_isdu_checksum (iolink_dl_t * dl)
{
   return (
      calcCHKPDU (dl->isdu_handler.isdu_data, dl->isdu_handler.total_isdu_len) ==
      0);
}

static iservice_t get_isdu_iserv (iolink_dl_t * dl)
{
   return ((dl->isdu_handler.isdu_data[0] >> 4) & 0x0F);
}

static uint8_t get_isdu_len (iolink_dl_t * dl)
{
   return (dl->isdu_handler.isdu_data[0] & 0x0F);
}

static bool get_isdu_total_len (iolink_dl_t * dl, uint8_t * isdu_total_len)
{
   uint8_t len = get_isdu_len (dl);

   if (len == 1)
   {
      len = dl->isdu_handler.isdu_data[1];
   }

   if ((len < 2) || (len == 16))
   {
      return false;
   }

   *isdu_total_len = len;

   return true;
}

static bool get_isdu_data_length (iolink_dl_t * dl, uint8_t * isdu_data_length)
{
   uint8_t ohlen = 2;
   uint8_t isdu_tot_len;

   if (!get_isdu_total_len (dl, &isdu_tot_len))
   {
      return false;
   }

   if (get_isdu_len (dl) == 1)
   {
      ohlen = 3;
   }

   *isdu_data_length = isdu_tot_len - ohlen;

   return true;
}

static uint8_t * get_isdu_response_data (iolink_dl_t * dl)
{
   uint8_t offset = (get_isdu_len (dl) == 1) ? 2 : 1;

   return &dl->isdu_handler.isdu_data[offset];
}

static uint8_t get_isdu_total_segments (iolink_dl_t * dl)
{
   return (dl->isdu_handler.total_isdu_len + dl->message_handler.od_len - 1) /
          dl->message_handler.od_len;
}

static bool valid_isdu_header (iolink_dl_t * dl)
{
   iservice_t iserv = get_isdu_iserv (dl);
   uint8_t len      = get_isdu_len (dl);
   bool result      = false;

   if (iserv == IOL_ISERVICE_DEVICE_NO_SERVICE)
   {
      result = (len > 1) ? false : true;
   }
   else if (
      (iserv == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG) ||
      (iserv == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS) ||
      (iserv == IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG) ||
      (iserv == IOL_ISERVICE_DEVICE_READ_RESPONSE_POS))
   {
      if (
         ((len == 0) || (len > 15)) ||
         (dl->isdu_handler.data_dir_read &&
          ((iserv == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_NEG) ||
           (iserv == IOL_ISERVICE_DEVICE_WRITE_RESPONSE_POS))) ||
         (!dl->isdu_handler.data_dir_read &&
          ((iserv == IOL_ISERVICE_DEVICE_READ_RESPONSE_NEG) ||
           (iserv == IOL_ISERVICE_DEVICE_READ_RESPONSE_POS))))
      {
         result = false;
      }
      else
      {
         result = true;
      }
   }

   return result;
}

static void start_timer_initcyc (iolink_dl_t * dl)
{
   uint32_t timer_val = get_T_initcyc (dl);
   os_timer_stop (dl->timer);

#ifdef __rtk__
   if (timer_val < 1000)
   {
      // For COM3 will get_T_initcyc return 435, this is divided by 1000 -> 0
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL: %s: get_T_initcyc() < 1000 (%d)\n",
         __func__,
         (int)timer_val);
      timer_val = 1000;
   }
#endif
   os_timer_set (dl->timer, timer_val);
   dl->timer_elapsed = false;
   os_timer_start (dl->timer);
   dl->timer_type = IOL_DL_TIMER_TINITCYC_MH;
}

static void isdu_timer_reset (iolink_dl_t * dl)
{
   dl->isdu_handler.isdu_timer = 0;
}

static void isdu_timer_start (iolink_dl_t * dl)
{
   dl->isdu_handler.isdu_timer = os_get_current_time_us();

   if (dl->isdu_handler.isdu_timer == 0)
   {
      // Timer value 0 is used as timer off
      dl->isdu_handler.isdu_timer = 1;
   }
}

static bool isdu_timer_elapsed (iolink_dl_t * dl)
{
   return (
      ((os_get_current_time_us() - dl->isdu_handler.isdu_timer) >
       IOLINK_ISDU_RESPONSE_TIME) &&
      (dl->isdu_handler.isdu_timer != 0));
}

static void interleave_reset_od_sizes (iolink_dl_t * dl)
{
   dl->od_handler.od_rxlen = 0;
   dl->od_handler.od_txlen = 0;
}

static void set_mseq_change (iolink_dl_t * dl)
{
   if (dl->message_handler.interleave != IOL_DL_INTERLEAVE_NONE)
   {
      if (dl->message_handler.pd_txlen > 0)
      {
         dl->pd_handler.pd_txlen = 2;
      }

      if (dl->message_handler.pd_rxlen > 0)
      {
         dl->pd_handler.pd_rxlen = 2;
      }

      dl->od_handler.odout_buffer = dl->pd_handler.pdout_buffer;
   }
   else
   {
      dl->pd_handler.pd_txlen = dl->message_handler.pd_txlen;
      dl->pd_handler.pd_rxlen = dl->message_handler.pd_rxlen;
      dl->od_handler.odout_buffer =
         dl->pd_handler.pdout_buffer + dl->pd_handler.pd_txlen;
   }

   if (
      (dl->message_handler.interleave != IOL_DL_INTERLEAVE_NONE) &&
      (dl->pd_handler.pd_rxlen == 0))
   {
      // Start in pdout interleave, since no rx process data.
      dl->pd_handler.state = IOL_DL_PDH_ST_PDOUTINTERLEAVE_3;
   }

   memset (dl->txbuffer, 0, IOLINK_RXTX_BUFFER_SIZE);
}

static void CH_Conf (iolink_port_t * port, CHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->cmd_handler.chcmd = cmd;
   iolink_dl_cmd_h_sm (port);
}

static void OH_Conf (iolink_port_t * port, OHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->od_handler.ohcmd = cmd;
   iolink_dl_od_h_sm (port);
}

static void PH_Conf (iolink_port_t * port, PHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->pd_handler.phcmd = cmd;

   dl->message_handler.interleave = (cmd == IOL_PHCMD_INTERLEAVE)
                                       ? IOL_DL_INTERLEAVE_PD
                                       : IOL_DL_INTERLEAVE_NONE;

   iolink_dl_pd_h_sm (port);
}

static void IH_Conf (iolink_port_t * port, IHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->isdu_handler.ihcmd = cmd;
   iolink_dl_isdu_h_sm (port);
}

static void EH_Conf (iolink_port_t * port, EHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->event_handler.ehcmd = cmd;
   iolink_dl_ev_h_sm (port);
}

static void MH_Conf (iolink_port_t * port, MHCmd_t cmd)
{
   iolink_dl_t * dl = iolink_get_dl_ctx(port);

   dl->message_handler.mhcmd = cmd;
   iolink_dl_message_h_sm(port);
}

static void iolink_dl_mode_h_sm_goto_operate (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mseq & IOLINK_MSEQTYPE_TYPE_2_X)
   {
      PH_Conf (port, IOL_PHCMD_SINGLE);
   }
   else if (dl->mseq == IOLINK_MSEQTYPE_TYPE_1_1)
   {
      PH_Conf (port, IOL_PHCMD_INTERLEAVE);
   }

   MH_Conf (port, IOL_MHCMD_OPERATE);
   dl->mode_handler.state = IOL_DL_MDH_ST_OPERATE_4;
   DL_Mode_ind (port, IOLINK_MHMODE_OPERATE);
}

static void set_OH_IH_EH_Conf_active (iolink_port_t * port, bool active)
{
   OH_Conf (port, (active) ? IOL_OHCMD_ACTIVE : IOL_OHCMD_INACTIVE);
   IH_Conf (port, (active) ? IOL_IHCMD_ACTIVE : IOL_IHCMD_INACTIVE);
   EH_Conf (port, (active) ? IOL_EHCMD_ACTIVE : IOL_EHCMD_INACTIVE);
}

static void iolink_dl_mode_h_sm_goto_idle (
   iolink_port_t * port,
   iolink_mhmode_t mode)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   set_OH_IH_EH_Conf_active (port, false);
   CH_Conf (port, IOL_CHCMD_INACTIVE);
   MH_Conf (port, IOL_MHCMD_INACTIVE);
   dl->mode_handler.mhinfo = IOLINK_MHINFO_NONE;
   dl->mode_handler.state  = IOL_DL_MDH_ST_IDLE_0;
   DL_Mode_ind (port, mode);
}

static void iolink_dl_mode_h_sm_goto_startup (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   set_OH_IH_EH_Conf_active (port, false);
   MH_Conf (port, IOL_MHCMD_STARTUP);
   dl->mode_handler.state = IOL_DL_MDH_ST_STARTUP_2;
   DL_Mode_ind (port, IOLINK_MHMODE_STARTUP);
}

static void iolink_dl_mode_h_sm_idle0 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mode_handler.dl_mode == IOLINK_DLMODE_STARTUP) // T1
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      // T15, T16, T17, T18, T19
      if (iolink_pl_init_sdci (port))
      {
         dl->mode_handler.state = IOL_DL_MDH_ST_ESTCOM_1;
      }
      else
      {
         // TODO FIXME: handle this!
         LOG_WARNING (
            IOLINK_DL_LOG,
            "DL (%u): %s: Unable to start SDCI\n",
            iolink_get_portnumber (port),
            __func__);
      }
#endif
   }
   else if (dl->mode_handler.dl_mode != IOLINK_DLMODE_INACTIVE)
   {
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: IDLE_0: Unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_mode_h_sm_establish_com1 (void)
{
#if IOLINK_HW == IOLINK_HW_MAX14819
   // Substate machine handled by max14819
#endif
}

static void iolink_dl_mode_h_sm_startup2 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   // TODO: Get ID from device
   if (dl->mode_handler.dl_mode == IOLINK_DLMODE_PREOPERATE) // T3
   {
      set_OH_IH_EH_Conf_active (port, true);
      MH_Conf (port, IOL_MHCMD_PREOPERATE);
      dl->mode_handler.state = IOL_DL_MDH_ST_PREOPERATE_3;
      DL_Mode_ind (port, IOLINK_MHMODE_PREOPERATE);
   }
   else if (dl->mode_handler.dl_mode == IOLINK_DLMODE_OPERATE) // T5
   {
      set_OH_IH_EH_Conf_active (port, true);
      iolink_dl_mode_h_sm_goto_operate (port);
   }
   else if (dl->mode_handler.dl_mode == IOLINK_DLMODE_INACTIVE) // Not in
                                                                  // spec.
   {
      iolink_dl_mode_h_sm_goto_idle (port, IOLINK_MHMODE_INACTIVE);
   }
   else if (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST) // Not in spec.
   {
      iolink_dl_mode_h_sm_goto_idle (port, IOLINK_MHMODE_COMLOST);
   }
   else
   {
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: STARTUP_2: Unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_mode_h_sm_preoperate3 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mode_handler.dl_mode == IOLINK_DLMODE_STARTUP) // T6
   {
      CH_Conf (port, IOL_CHCMD_INACTIVE);
      iolink_dl_mode_h_sm_goto_startup (port);
   }
   else if (dl->mode_handler.dl_mode == IOLINK_DLMODE_INACTIVE) // SDCI_TC_0214
   {
      MH_Conf (port, IOL_MHCMD_OPERATE);
   }
   else if (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST) // T9
   {
      iolink_dl_mode_h_sm_goto_idle (port, IOLINK_MHMODE_COMLOST);
   }
   else if (dl->mode_handler.dl_mode == IOLINK_DLMODE_OPERATE) // T10      {
   {
      iolink_dl_mode_h_sm_goto_operate (port);
   }
   else
   {
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: PREOPERATE_3: Unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_mode_h_sm_operate4 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mode_handler.dl_mode == IOLINK_DLMODE_STARTUP) // T12
   {
      PH_Conf (port, IOL_PHCMD_INACTIVE);
      iolink_dl_mode_h_sm_goto_startup (port);
   }
   else if (
      (dl->mode_handler.dl_mode == IOLINK_DLMODE_INACTIVE) || // T13
      (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST))     // T14
   {
      PH_Conf (port, IOL_PHCMD_INACTIVE);
      iolink_dl_mode_h_sm_goto_idle (
         port,
         (dl->mode_handler.dl_mode == IOLINK_DLMODE_INACTIVE)
            ? IOLINK_MHMODE_INACTIVE
            : IOLINK_MHMODE_COMLOST);
   }
   else if (dl->mode_handler.dl_mode != IOLINK_DLMODE_OPERATE)
   {
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: OPERATE_4: Unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_mode_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->mode_handler.state)
   {
   case IOL_DL_MDH_ST_IDLE_0:
      iolink_dl_mode_h_sm_idle0 (port);
      break;
   case IOL_DL_MDH_ST_ESTCOM_1:
      iolink_dl_mode_h_sm_establish_com1 ();
      break;
   case IOL_DL_MDH_ST_STARTUP_2:
      iolink_dl_mode_h_sm_startup2 (port);
      break;
   case IOL_DL_MDH_ST_PREOPERATE_3:
      iolink_dl_mode_h_sm_preoperate3 (port);
      break;
   case IOL_DL_MDH_ST_OPERATE_4:
      iolink_dl_mode_h_sm_operate4 (port);
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d:\n",
         iolink_get_portnumber (port),
         __func__,
         dl->mode_handler.state);
      break;
   }
}

static void write_master_command (iolink_port_t * port, iolink_status_t errorinfo)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   os_mutex_lock (dl->mtx);
   uint8_t master_command         = dl->cmd_handler.master_command;
   dl->cmd_handler.master_command = 0;
   os_mutex_unlock (dl->mtx);

   switch (master_command)
   {
   /* Not in spec. */
   case IOL_MASTERCMD_DEVICE_OPERATE:
   case IOL_MASTERCMD_DEVICE_PREOP:
   case IOL_MASTERCMD_FALLBACK:
      DL_Write_Devicemode_cnf (port, errorinfo, master_command); /* Not in spec.
                                                                  */
      break;
   default:
      break;
   }
}

static void iolink_dl_mh_handle_com_lost (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->timer_type = IOL_DL_TIMER_NONE;
   MHInfo_ind (port, IOLINK_MHINFO_COMLOST);
   dl->message_handler.rwcmd = IOL_MHRW_NONE;

   LOG_ERROR (
      IOLINK_DL_LOG,
      "DL (%u): %s: Communication lost (MH state: %s)\n",
      iolink_get_portnumber (port),
      __func__,
      iolink_dl_mh_st_literals[dl->message_handler.state]);

   dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
   os_event_set (dl->event, IOLINK_DL_EVENT_MDH);
}

static void iolink_dl_message_h_sm_inactive0 (iolink_port_t * port)
{
#if IOLINK_HW == IOLINK_HW_MAX14819
   PL_DisableCycleTimer (port);
#endif
}

static void iolink_dl_message_h_sm_await_reply1 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

#if IOLINK_HW == IOLINK_HW_MAX14819
   if (dl->baudrate != IOLINK_BAUDRATE_NONE) // T2
   {
      dl->message_handler.state = IOL_DL_MH_ST_STARTUP_2;
      DL_Read_cnf (port, dl->cycbyte, IOLINK_STATUS_NO_ERROR);
      dl->tinitcyc = 0;
   }
   else // T3
   {
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
   }
#endif
}

static void iolink_dl_message_h_sm_startup2 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.mhcmd == IOL_MHCMD_PREOPERATE) // T12
   {
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_PREOPERATE_6;
   }
   else if (dl->message_handler.mhcmd == IOL_MHCMD_OPERATE) // T39
   {
      start_timer_initcyc (dl);
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_OPERATE_12;
   }
   else if (
      (dl->message_handler.rwcmd == IOL_MHRW_READ) ||
      (dl->message_handler.rwcmd == IOL_MHRW_WRITE))
   {
      dl->message_handler.retry = 0;
      dl->dataready             = false;
#if IOLINK_HW == IOLINK_HW_MAX14819
      if (dl->tinitcyc++ < 4)
      {
         os_usleep ((get_T_initcyc (dl) + 4000) / 2);
      }
      PL_Transfer_req (
         port,
         dl->od_handler.od_rxlen + 1, // T5, T6
         dl->od_handler.od_txlen + 2,
         dl->txbuffer);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_RESPONSE_3;
   }
   else if (dl->message_handler.mhcmd != IOL_MHCMD_INACTIVE)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_message_h_sm_response3 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_4;
   iolink_dl_message_h_sm_await_reply4 (port);
}

static void iolink_dl_message_h_sm_await_reply4 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->dataready) // T10
   {
      dl->message_handler.state = IOL_DL_MH_ST_STARTUP_2;

      if (dl->message_handler.rwcmd == IOL_MHRW_READ)
      {
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         DL_Read_cnf (port, dl->rxbuffer[0], IOLINK_STATUS_NO_ERROR);
      }
      else if (dl->message_handler.rwcmd == IOL_MHRW_WRITE)
      {
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         DL_Write_cnf (port, IOLINK_STATUS_NO_ERROR);
      }
   }
   else if (
      (dl->rxtimeout) || // T7
      (dl->rxerror) ||   // T8
      (dl->txerror))
   {
      start_timer_initcyc (dl);
      dl->message_handler.state = IOL_DL_MH_ST_ERRORHANDLING_5;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_message_h_sm_error_handling5 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.retry >= IOLINK_MAX_RETRY) // T11
   {
      dl->timer_type = IOL_DL_TIMER_NONE;
      MHInfo_ind (port, IOLINK_MHINFO_COMLOST);
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
      os_event_set (dl->event, IOLINK_DL_EVENT_MDH);

      if (dl->message_handler.rwcmd == IOL_MHRW_READ)
      {
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         DL_Read_cnf (port, 0, IOLINK_STATUS_NO_COMM);
      }
      else if (dl->message_handler.rwcmd == IOL_MHRW_WRITE)
      {
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         DL_Write_cnf (port, IOLINK_STATUS_NO_COMM);
      }
   }
   else if (dl->timer_elapsed && (dl->timer_type == IOL_DL_TIMER_TINITCYC_MH)) // T9
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_Resend (port);
#endif
      dl->message_handler.retry++;
      dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_4;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_message_h_sm_preoperate6 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.mhcmd == IOL_MHCMD_OPERATE) // T26
   {
      start_timer_initcyc (dl);
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_OPERATE_12;
   }
   else if (dl->message_handler.mhcmd == IOL_MHCMD_STARTUP) // T37
   {
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_STARTUP_2;
   }
   else if (dl->message_handler.mhcmd == IOL_MHCMD_INACTIVE) // T36
   {
      MHInfo_ind (port, IOLINK_MHINFO_COMLOST);
      os_event_set (dl->event, IOLINK_DL_EVENT_MDH);
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_Transfer_req (
         port,
         dl->od_handler.od_rxlen + 1,
         dl->od_handler.od_txlen + 2,
         dl->txbuffer);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
   }
   else if (
      (dl->message_handler.rwcmd == IOL_MHRW_READPARAM) ||
      (dl->message_handler.rwcmd == IOL_MHRW_WRITEPARAM) ||
      (dl->message_handler.rwcmd == IOL_MHRW_ISDUTRANSPORT) ||
      (dl->cmd_handler.master_command == IOL_MASTERCMD_DEVICE_OPERATE) ||
      dl->event_handler.event_flag)
   {
      dl->message_handler.retry = 0;
      // OD_Trig
      dl->od_handler.trigger    = IOL_TRIGGERED_MASTER_MESSAGE;
      dl->message_handler.state = IOL_DL_MH_ST_GETOD_7; // T13, T14, T15,
                                                         // T17, T16
      start_timer_initcyc (dl);
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_message_h_sm_get_od7 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   iolink_dl_od_h_sm (port);
   dl->message_handler.retry = 0;
   dl->dataready             = false;
   // start_timer_initcyc(dl);
#if IOLINK_HW == IOLINK_HW_MAX14819
   PL_Transfer_req (
      port,
      dl->od_handler.od_rxlen + 1,
      dl->od_handler.od_txlen + 2,
      dl->txbuffer);
   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: Message sent (PreOp)\n",
      iolink_get_portnumber (port),
      __func__);
   dl->message_handler.state = IOL_DL_MH_ST_RESPONSE_8; // T18
#endif
}

static void iolink_dl_message_h_sm_response8 (iolink_port_t * port)
{
   iolink_dl_message_h_sm_await_reply9 (port);
}

static void iolink_dl_message_h_sm_await_reply9 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->dataready)
   {
      // ODTrig
      dl->od_handler.trigger = IOL_TRIGGERED_DEVICE_MESSAGE;
      EventFlag_ind (dl, getCKSEvFlag (dl));
      iolink_dl_od_h_sm (port);

      if (dl->isdu_handler.state == IOL_DL_ISDUH_ST_IDLE_1)
      {
         if (dl->message_handler.mhcmd == IOL_MHCMD_OPERATE)
         {
            start_timer_initcyc (dl);
            dl->message_handler.state =
               IOL_DL_MH_ST_AW_REPLY_16; // SDCI_TC_0196
         }
         else if (!dl->event_handler.event_flag)
         {
            os_timer_stop (dl->timer);
            dl->message_handler.state = IOL_DL_MH_ST_PREOPERATE_6; // T23,
                                                                     // T25
         }
      }
      else
      {
         start_timer_initcyc (dl);
         dl->od_handler.trigger    = IOL_TRIGGERED_MASTER_MESSAGE;
         dl->message_handler.state = IOL_DL_MH_ST_GETOD_7; // T23, T24
      }
   }
   else if (
      dl->rxtimeout || // T19
      dl->rxerror ||   // T20
      dl->txerror)
   {
      start_timer_initcyc (dl);
      dl->message_handler.state = IOL_DL_MH_ST_ERRORHANDLING_10;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
      dl->cqerr  = 0;
      dl->devdly = 0;
      iolink_pl_get_error (port, &dl->cqerr, &dl->devdly);
   }
}

static void iolink_dl_message_h_sm_error_handling10 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.retry >= IOLINK_MAX_RETRY) // T22
   {
      write_master_command (port, IOLINK_STATUS_NO_COMM);

      switch (dl->message_handler.rwcmd)
      {
      case IOL_MHRW_READ:
         DL_Read_cnf (port, 0, IOLINK_STATUS_NO_COMM);
         break;
      case IOL_MHRW_WRITE:
         DL_Write_cnf (port, IOLINK_STATUS_NO_COMM);
         break;
      case IOL_MHRW_READPARAM:
         DL_ReadParam_cnf (port, 0, IOLINK_STATUS_NO_COMM);
         break;
      case IOL_MHRW_WRITEPARAM:
         DL_WriteParam_cnf (port, IOLINK_STATUS_NO_COMM);
         break;
      default:
         break;
      }

      iolink_dl_mh_handle_com_lost (port);
   }
   else if (dl->timer_elapsed && (dl->timer_type == IOL_DL_TIMER_TINITCYC_MH)) // T9
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_Resend (port);
#endif
      dl->message_handler.retry++;
      dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_9; // T21
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_message_h_sm_check_handler11 (void)
{
   // Empty state
}

static void iolink_dl_message_h_sm_operate12 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.mhcmd == IOL_MHCMD_STARTUP) // T38
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_DisableCycleTimer (port);
#endif
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_STARTUP_2;
   }
   else if (dl->message_handler.mhcmd == IOL_MHCMD_INACTIVE) // T35
   {
      MHInfo_ind (port, IOLINK_MHINFO_COMLOST);
      os_event_set (dl->event, IOLINK_DL_EVENT_MDH);
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_DisableCycleTimer (port);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
   }
   else if (dl->timer_elapsed) // Initial waiting time before starting PD
                                 // cycle
   {
      // PD Trig
      dl->pd_handler.trigger = IOL_TRIGGERED_MASTER_MESSAGE;
      iolink_dl_message_h_sm_get_pd13 (port);
   }
   else
   {
      iolink_dl_message_h_sm_get_pd13 (port);
   }
}

static void iolink_dl_message_h_sm_get_pd13 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   iolink_dl_pd_h_sm (port);

   if (dl->message_handler.interleave == IOL_DL_INTERLEAVE_NONE)
   {
      // OD Trig
      dl->od_handler.trigger = IOL_TRIGGERED_MASTER_MESSAGE;
   }
   iolink_dl_message_h_sm_get_od14 (port);
}

static void iolink_dl_message_h_sm_get_od14 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.interleave == IOL_DL_INTERLEAVE_NONE)
   {
      iolink_dl_od_h_sm (port);
   }
   dl->message_handler.retry = 0;
   dl->dataready             = false;
#if IOLINK_HW == IOLINK_HW_MAX14819
   iolink_pl_set_cycletime (port, dl->cycbyte);
   PL_EnableCycleTimer (port);
   PL_Transfer_req (
      port,
      dl->od_handler.od_rxlen + dl->pd_handler.pd_rxlen + 1,
      dl->od_handler.od_txlen + dl->pd_handler.pd_txlen + 2,
      dl->txbuffer);
   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: Message sent\n",
      iolink_get_portnumber (port),
      __func__);
#endif
   dl->message_handler.state = IOL_DL_MH_ST_RESPONSE_15; // T29
}

static void iolink_dl_message_h_sm_response15 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_16;
   iolink_dl_message_h_sm_await_reply16 (port);
}

static void iolink_dl_message_h_sm_await_reply16 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.mhcmd == IOL_MHCMD_STARTUP) // T38
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_DisableCycleTimer (port);
#endif
      set_mseq_change (dl);
      dl->message_handler.state = IOL_DL_MH_ST_STARTUP_2;
   }
   else if (dl->message_handler.mhcmd == IOL_MHCMD_INACTIVE) // T35
   {
      MHInfo_ind (port, IOLINK_MHINFO_COMLOST);
      os_event_set (dl->event, IOLINK_DL_EVENT_MDH);
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_DisableCycleTimer (port);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
   }
   else if (dl->message_handler.rwcmd == IOL_MHRW_WRITEPARAM) // SDCI_TC_0196
   {
      dl->od_handler.trigger = IOL_TRIGGERED_MASTER_MESSAGE;
      iolink_dl_od_h_sm (port);
      dl->message_handler.retry = 0;
      dl->dataready             = false;
#if IOLINK_HW == IOLINK_HW_MAX14819
      os_mutex_lock (dl->mtx);
      PL_MessageDownload_req (
         port,
         dl->od_handler.od_rxlen + dl->pd_handler.pd_rxlen + 1,
         dl->od_handler.od_txlen + dl->pd_handler.pd_txlen + 2,
         dl->txbuffer);
      os_mutex_unlock (dl->mtx);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_9;
   }
   else if (dl->dataready)
   {
      dl->message_handler.cks =
         dl->rxbuffer[dl->od_handler.od_rxlen + dl->pd_handler.pd_rxlen];
      EventFlag_ind (dl, getCKSEvFlag (dl));
      PDInStatus_ind (port, getCKSPDIn (dl));

      if (dl->message_handler.interleave != IOL_DL_INTERLEAVE_OD)
      {
         // PD Trig
         dl->pd_handler.trigger = IOL_TRIGGERED_DEVICE_MESSAGE;
         iolink_dl_pd_h_sm (port);

         if (dl->message_handler.interleave == IOL_DL_INTERLEAVE_PD)
         {
            dl->message_handler.interleave = IOL_DL_INTERLEAVE_OD;
         }
      }

      if (dl->message_handler.interleave != IOL_DL_INTERLEAVE_PD)
      {
         // OD Trig
         dl->od_handler.trigger = IOL_TRIGGERED_DEVICE_MESSAGE;
         iolink_dl_od_h_sm (port);

         if (dl->message_handler.interleave == IOL_DL_INTERLEAVE_OD)
         {
            dl->message_handler.interleave = IOL_DL_INTERLEAVE_PD;
         }
      }

      // Prepare next message
      // PD Trig
      dl->pd_handler.trigger = IOL_TRIGGERED_MASTER_MESSAGE;
      iolink_dl_pd_h_sm (port);
      // OD Trig
      dl->od_handler.trigger = IOL_TRIGGERED_MASTER_MESSAGE;
      iolink_dl_od_h_sm (port);
#if IOLINK_HW == IOLINK_HW_MAX14819
      os_mutex_lock (dl->mtx);
      PL_MessageDownload_req (
         port,
         dl->od_handler.od_rxlen + dl->pd_handler.pd_rxlen + 1,
         dl->od_handler.od_txlen + dl->pd_handler.pd_txlen + 2,
         dl->txbuffer);
      os_mutex_unlock (dl->mtx);
#endif
      dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_16; // T34
      dl->dataready             = false;
   }
   else if (
      dl->rxtimeout || // T30
      dl->rxerror ||   // T31
      dl->txerror)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: Error rx-timeout: %d, rx: %d, tx: %d\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state],
         dl->rxtimeout, dl->rxerror, dl->txerror);

      if (dl->message_handler.retry >= IOLINK_MAX_RETRY) // T33
      {
         iolink_dl_mh_handle_com_lost (port);
      }

      dl->message_handler.retry++;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
      dl->cqerr  = 0;
      dl->devdly = 0;
      iolink_pl_get_error (port, &dl->cqerr, &dl->devdly);
   }
}

static void iolink_dl_message_h_sm_error_handling17 (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);

   if (dl->message_handler.retry >= IOLINK_MAX_RETRY) // T33
   {
      iolink_dl_mh_handle_com_lost (port);
   }
   else if (dl->timer_tcyc_elapsed) // T32
   {
#if IOLINK_HW == IOLINK_HW_MAX14819
      PL_Resend (port);
#endif
      dl->message_handler.retry++;
      dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_16;
   }
   else
   {
      if (dl->dataready)
      {
         LOG_ERROR (
            IOLINK_DL_LOG,
            "DL (%u): %s: %s: Rxdata arrived too late. Trying to recover\n",
            iolink_get_portnumber (port),
            __func__,
            iolink_dl_mh_st_literals[dl->message_handler.state]);
         dl->message_handler.cks =
            dl->rxbuffer[dl->od_handler.od_rxlen + dl->pd_handler.pd_rxlen];
         PDInStatus_ind (port, getCKSPDIn (dl));
         // PD Trig
         dl->pd_handler.trigger = IOL_TRIGGERED_DEVICE_MESSAGE;
         iolink_dl_pd_h_sm (port);
         // OD Trig
         dl->od_handler.trigger = IOL_TRIGGERED_DEVICE_MESSAGE;
         EventFlag_ind (dl, getCKSEvFlag (dl));
         iolink_dl_od_h_sm (port);
         dl->message_handler.state = IOL_DL_MH_ST_OPERATE_12; // T34
      }
      else
      {
         LOG_ERROR (
            IOLINK_DL_LOG,
            "DL (%u): %s: %s: unknown event triggered\n",
            iolink_get_portnumber (port),
            __func__,
            iolink_dl_mh_st_literals[dl->message_handler.state]);
      }
   }
}

static void iolink_dl_message_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl    = iolink_get_dl_ctx (port);
   dl_mh_st_t previous = dl->message_handler.state;

   switch (dl->message_handler.state)
   {
   case IOL_DL_MH_ST_INACTIVE_0:
      iolink_dl_message_h_sm_inactive0 (port);
      break;
   case IOL_DL_MH_ST_AW_REPLY_1:
      iolink_dl_message_h_sm_await_reply1 (port);
      break;
   case IOL_DL_MH_ST_STARTUP_2:
      iolink_dl_message_h_sm_startup2 (port);
      break;
   case IOL_DL_MH_ST_RESPONSE_3:
      iolink_dl_message_h_sm_response3 (port);
      break;
   case IOL_DL_MH_ST_AW_REPLY_4:
      iolink_dl_message_h_sm_await_reply4 (port);
      break;
   case IOL_DL_MH_ST_ERRORHANDLING_5:
      iolink_dl_message_h_sm_error_handling5 (port);
      break;
   case IOL_DL_MH_ST_PREOPERATE_6:
      iolink_dl_message_h_sm_preoperate6 (port);
      break;
   case IOL_DL_MH_ST_GETOD_7:
      iolink_dl_message_h_sm_get_od7 (port);
      break;
   case IOL_DL_MH_ST_RESPONSE_8:
      iolink_dl_message_h_sm_response8 (port);
      break;
   case IOL_DL_MH_ST_AW_REPLY_9:
      iolink_dl_message_h_sm_await_reply9 (port);
      break;
   case IOL_DL_MH_ST_ERRORHANDLING_10:
      iolink_dl_message_h_sm_error_handling10 (port);
      break;
   case IOL_DL_MH_ST_CHECKHANDLER_11:
      iolink_dl_message_h_sm_check_handler11 ();
      break;
   case IOL_DL_MH_ST_OPERATE_12:
      iolink_dl_message_h_sm_operate12 (port);
      break;
   case IOL_DL_MH_ST_GETPD_13:
      iolink_dl_message_h_sm_get_pd13 (port);
      break;
   case IOL_DL_MH_ST_GETOD_14:
      iolink_dl_message_h_sm_get_od14 (port);
      break;
   case IOL_DL_MH_ST_RESPONSE_15:
      iolink_dl_message_h_sm_response15 (port);
      break;
   case IOL_DL_MH_ST_AW_REPLY_16:
      iolink_dl_message_h_sm_await_reply16 (port);
      break;
   case IOL_DL_MH_ST_ERRORHANDLING_17:
      iolink_dl_message_h_sm_error_handling17 (port);
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d:\n",
         iolink_get_portnumber (port),
         __func__,
         dl->message_handler.state);
      break;
   }

   if (dl->message_handler.state != previous)
   {
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: state change: %s -> %s\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[previous],
         iolink_dl_mh_st_literals[dl->message_handler.state]);
   }
}

static void iolink_dl_pd_h_sm_inactive0 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->pd_handler.phcmd == IOL_PHCMD_SINGLE)
   {
      dl->pd_handler.state = IOL_DL_PDH_ST_PDSINGLE_1; // T2
   }
   else if (dl->pd_handler.phcmd == IOL_PHCMD_INTERLEAVE)
   {
      dl->pd_handler.state = IOL_DL_PDH_ST_PDININTERLEAVE_2; // T4
   }
   else if (dl->pd_handler.trigger != IOL_TRIGGERED_NONE) // T1
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;
      PD_req (port, 0, 0, NULL, 0, 0);
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: INACTIVE_0: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_pd_h_sm_pd_single1 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->pd_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE) // T3
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;
      PD_req (
         port,
         0,
         dl->pd_handler.pd_rxlen,
         dl->pd_handler.pdoutdata,
         0,
         dl->pd_handler.pd_txlen);
   }
   else if (dl->pd_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE) // T3
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;
      DL_PDInputTransport_ind (
         port,
         &dl->rxbuffer[dl->od_handler.od_rxlen],
         dl->pd_handler.pd_rxlen);
   }
   else if (dl->pd_handler.phcmd == IOL_PHCMD_INACTIVE)
   {
      dl->pd_handler.state = IOL_DL_PDH_ST_INACTIVE_0; // T9
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: PDSINGLE_1: unknown event triggered (t:%d, c:%d)\n",
         iolink_get_portnumber (port),
         __func__,
         dl->pd_handler.trigger,
         dl->pd_handler.phcmd);
   }
}

static void iolink_dl_pd_h_sm_pd_in_interleave2 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->pd_handler.phcmd == IOL_PHCMD_INACTIVE)
   {
      dl->pd_handler.state = IOL_DL_PDH_ST_INACTIVE_0; // T10
   }
   else if (dl->pd_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE) // T5
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;
      OD_req (
         port,
         IOLINK_RWDIRECTION_READ,
         IOLINK_COMCHANNEL_PROCESS,
         dl->pd_handler.pd_address,
         0,
         NULL);
      interleave_reset_od_sizes (dl);
   }
   else if (dl->pd_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      memcpy (
         &dl->pd_handler.pdindata[dl->pd_handler.pd_address],
         dl->rxbuffer,
         2);
      dl->pd_handler.pd_address += 2;

      if (dl->pd_handler.pd_address >= dl->pd_handler.pd_rxlen)
      {
         dl->pd_handler.pd_address = 0;
         DL_PDInputTransport_ind (
            port,
            dl->pd_handler.pdindata,
            dl->pd_handler.pd_rxlen);
         // Stay in PDININTERLEAVE since no tx-data

         if (dl->pd_handler.pd_txlen > 0)
         {
            dl->pd_handler.state = IOL_DL_PDH_ST_PDOUTINTERLEAVE_3; // T6
         }
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: PDININTERLEAVE_2: unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_pd_h_sm_pd_out_interleave3 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->pd_handler.phcmd == IOL_PHCMD_INACTIVE)
   {
      dl->pd_handler.state = IOL_DL_PDH_ST_INACTIVE_0; // T11
   }
   else if (dl->pd_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE) // T7
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;
      PD_req (port, 0, 0, dl->pd_handler.pdoutdata, dl->pd_handler.pd_address, 2);
      OD_req (
         port,
         IOLINK_RWDIRECTION_WRITE,
         IOLINK_COMCHANNEL_PROCESS,
         dl->pd_handler.pd_address,
         0,
         NULL);
      interleave_reset_od_sizes (dl);
      dl->pd_handler.pd_address += 2;
   }
   else if (dl->pd_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      dl->pd_handler.trigger = IOL_TRIGGERED_NONE;

      if (dl->pd_handler.pd_address >= dl->pd_handler.pd_rxlen)
      {
         dl->pd_handler.pd_address = 0;
         // Stay in PDOUTINTERLEAVE since no rx-data

         if (dl->pd_handler.pd_rxlen > 0)
         {
            dl->pd_handler.state = IOL_DL_PDH_ST_PDININTERLEAVE_2; // T8
         }
      }
      else
      {
         // T7 response
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: PDOUTINTERLEAVE_3: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_pd_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->pd_handler.state)
   {
   case IOL_DL_PDH_ST_INACTIVE_0:
      iolink_dl_pd_h_sm_inactive0 (port);
      break;
   case IOL_DL_PDH_ST_PDSINGLE_1:
      iolink_dl_pd_h_sm_pd_single1 (port);
      break;
   case IOL_DL_PDH_ST_PDININTERLEAVE_2:
      iolink_dl_pd_h_sm_pd_in_interleave2 (port);
      break;
   case IOL_DL_PDH_ST_PDOUTINTERLEAVE_3:
      iolink_dl_pd_h_sm_pd_out_interleave3 (port);
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->pd_handler.state);
      break;
   }
}

static void iolink_dl_isdu_h_sm_isdu_error4 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->isdu_handler.ihcmd == IOL_IHCMD_INACTIVE)
   {
      dl->isdu_handler.state = IOL_DL_ISDUH_ST_INACTIVE_0; // T15
   }
   else
   {
      dl->isdu_handler.total_isdu_seg = 0;
      OD_req (
         port,
         IOLINK_RWDIRECTION_READ,
         IOLINK_COMCHANNEL_ISDU,
         IOLINK_FLOWCTRL_ABORT,
         0,
         NULL);

      iservice_t qualifier    = IOL_ISERVICE_MASTER_NO_SERVICE;
      iolink_status_t errinfo = IOLINK_STATUS_INVALID;

      if (isdu_timer_elapsed (dl))
      {
         errinfo = IOLINK_STATUS_ISDU_TIMEOUT;
      }
      else if (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST)
      {
         errinfo = IOLINK_STATUS_NO_COMM;
      }
      else if (dl->message_handler.rwcmd == IOL_MHRW_ISDUABORT)
      {
         errinfo = IOLINK_STATUS_ISDU_ABORT;
      }
      else if (!valid_isdu_header (dl))
      {
         qualifier = get_isdu_iserv (dl);
         errinfo   = IOLINK_STATUS_ISDU_ILLEGAL_SERVICE_PRIMITIVE;
      }

      DL_ISDUTransport_cnf (port, NULL, 0, qualifier, errinfo);

      dl->message_handler.rwcmd = IOL_MHRW_NONE;
      dl->isdu_handler.state    = IOL_DL_ISDUH_ST_IDLE_1; // T11
   }
}

static void iolink_dl_isdu_h_sm_enter_isduerror4 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->isdu_handler.ihcmd == IOL_IHCMD_INACTIVE)
   {
      dl->isdu_handler.total_isdu_seg = 0;
      DL_ISDUTransport_cnf (port, NULL, 0, 0, IOLINK_STATUS_NO_COMM); // Not in
                                                                      // spec
      dl->isdu_handler.state = IOL_DL_ISDUH_ST_INACTIVE_0;            // T15
   }
   else
   {
      if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
      {
         iolink_dl_isdu_h_sm_isdu_error4 (port);
      }
      else
      {
         dl->isdu_handler.state = IOL_DL_ISDUH_ST_ISDUERROR_4;
      }
   }
}

static void iolink_dl_isdu_h_sm_reception_complete (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);
   uint8_t isdu_data_length;

   if (!get_isdu_data_length (dl, &isdu_data_length))
   {
      DL_ISDUTransport_cnf (
         port,
         get_isdu_response_data (dl),
         0,
         get_isdu_iserv (dl),
         IOLINK_STATUS_INVALID);
   }
   else
   {
      iolink_status_t errinfo = IOLINK_STATUS_NO_ERROR;

      if (!valid_isdu_checksum (dl))
      {
         // Checksum error. Signal to AL, but continue without sending ABORT to
         // device
         errinfo = IOLINK_STATUS_ISDU_CHECKSUM_ERROR;
      }

      DL_ISDUTransport_cnf (
         port,
         get_isdu_response_data (dl),
         isdu_data_length,
         get_isdu_iserv (dl),
         errinfo);
   }

   dl->isdu_handler.current_isdu_seg = 0;
   dl->isdu_handler.total_isdu_seg   = 0;
   dl->message_handler.rwcmd         = IOL_MHRW_NONE;
   dl->isdu_handler.state            = IOL_DL_ISDUH_ST_IDLE_1; // T8
}

static void iolink_dl_isdu_h_sm_inactive0 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->isdu_handler.ihcmd == IOL_IHCMD_ACTIVE)
   {
      dl->isdu_handler.total_isdu_seg   = 0;
      dl->isdu_handler.current_isdu_seg = 0;
      dl->isdu_handler.state            = IOL_DL_ISDUH_ST_IDLE_1; // T1
   }
   else if (dl->isdu_handler.ihcmd != IOL_IHCMD_INACTIVE)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_isdu_h_sm_idle1 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->isdu_handler.ihcmd == IOL_IHCMD_INACTIVE)
   {
      if (dl->isdu_handler.total_isdu_seg)
      {
         DL_ISDUTransport_cnf (port, NULL, 0, 0, IOLINK_STATUS_NO_COMM); // Not in spec
         dl->isdu_handler.total_isdu_seg = 0;
      }

      dl->isdu_handler.state = IOL_DL_ISDUH_ST_INACTIVE_0; // T16
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
   {
      switch (dl->message_handler.rwcmd)
      {
      case IOL_MHRW_ISDUTRANSPORT:
         OD_req (
            port,
            IOLINK_RWDIRECTION_WRITE,
            IOLINK_COMCHANNEL_ISDU,
            IOLINK_FLOWCTRL_START,
            dl->message_handler.od_len,
            dl->isdu_handler.isdu_data);
         dl->isdu_handler.current_isdu_seg = 1;
         dl->isdu_handler.state = IOL_DL_ISDUH_ST_ISDUREQUEST_2; // T2
         break;
      case IOL_MHRW_READPARAM: // T13
         OD_req (
            port,
            IOLINK_RWDIRECTION_READ,
            IOLINK_COMCHANNEL_PAGE,
            dl->od_handler.data_addr,
            0,
            0);
         break;
      case IOL_MHRW_WRITEPARAM: // T13
         OD_req (
            port,
            IOLINK_RWDIRECTION_WRITE,
            IOLINK_COMCHANNEL_PAGE,
            dl->od_handler.data_addr,
            1,
            &dl->od_handler.data_value);
         break;
      default: // T14
         OD_req (
            port,
            IOLINK_RWDIRECTION_READ,
            IOLINK_COMCHANNEL_ISDU,
            IOLINK_FLOWCTRL_IDLE_1,
            0,
            0);
         break;
      }
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      switch (dl->message_handler.rwcmd)
      {
      case IOL_MHRW_READPARAM: // T13 response
         DL_ReadParam_cnf (port, dl->rxbuffer[0], IOLINK_STATUS_NO_ERROR);
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         break;
      case IOL_MHRW_WRITEPARAM: // T13 response
         DL_WriteParam_cnf (port, IOLINK_STATUS_NO_ERROR);
         dl->message_handler.rwcmd = IOL_MHRW_NONE;
         break;
      default: // T14 response
         break;
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IDLE_1: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_isdu_h_sm_isdu_request2 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (
      (dl->message_handler.rwcmd == IOL_MHRW_ISDUABORT) ||
      (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST))
   {
      iolink_dl_isdu_h_sm_enter_isduerror4 (port); // T12 and T19
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
   {
      OD_req (
         port,
         IOLINK_RWDIRECTION_WRITE,
         IOLINK_COMCHANNEL_ISDU,
         dl->isdu_handler.current_isdu_seg % 16,
         dl->message_handler.od_len,
         &dl->isdu_handler.isdu_data
               [dl->message_handler.od_len * dl->isdu_handler.current_isdu_seg]);
      dl->isdu_handler.current_isdu_seg++; // T3
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      if (dl->isdu_handler.current_isdu_seg >= dl->isdu_handler.total_isdu_seg)
      {
         isdu_timer_start (dl);
         dl->isdu_handler.current_isdu_seg = 0;
         dl->isdu_handler.state = IOL_DL_ISDUH_ST_ISDUWAIT_3; // T4
      }
      else
      {
         // T3 response
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ISDUH_ST_ISDUREQUEST_2: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_isdu_h_sm_isdu_wait3 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->message_handler.rwcmd == IOL_MHRW_ISDUABORT)
   {
      isdu_timer_reset (dl);
      iolink_dl_isdu_h_sm_enter_isduerror4 (port); // T17
   }
   else if (isdu_timer_elapsed (dl) || (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST))
   {
      iolink_dl_isdu_h_sm_enter_isduerror4 (port); // T9
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
   {
      OD_req (
         port,
         IOLINK_RWDIRECTION_READ,
         IOLINK_COMCHANNEL_ISDU,
         IOLINK_FLOWCTRL_START,
         dl->message_handler.od_len,
         dl->isdu_handler.isdu_data); // T5
      // T5
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      memcpy (
         &dl->isdu_handler.isdu_data
               [dl->isdu_handler.current_isdu_seg * dl->message_handler.od_len],
         dl->rxbuffer,
         dl->message_handler.od_len);

      if (dl->isdu_handler.isdu_data[0] == ((IOL_ISERVICE_DEVICE_NO_SERVICE << 4) | 0x01))
      {
         // Device busy
         // T5 response
      }
      else if (dl->isdu_handler.isdu_data[0] == 0)
      {
         // No service
         LOG_INFO (
            IOLINK_DL_LOG,
            "DL (%u): %s: ISDUWait. No service!\n",
            iolink_get_portnumber (port),
            __func__);
         isdu_timer_reset (dl);
         iolink_dl_isdu_h_sm_enter_isduerror4 (port); // T9
      }
      else
      {
         if (!valid_isdu_header (dl))
         {
            isdu_timer_reset (dl);
            iolink_dl_isdu_h_sm_enter_isduerror4 (port); // T9
         }
         else
         {
            uint8_t isdu_total_len = 0;
            get_isdu_total_len (dl, &isdu_total_len);
            dl->isdu_handler.total_isdu_len   = isdu_total_len;
            dl->isdu_handler.current_isdu_seg = 1;
            dl->isdu_handler.total_isdu_seg   = get_isdu_total_segments (dl);

            isdu_timer_reset (dl);

            if (dl->isdu_handler.current_isdu_seg >= dl->isdu_handler.total_isdu_seg)
            {
               iolink_dl_isdu_h_sm_reception_complete (port); // T6, T8
            }
            else
            {
               dl->isdu_handler.state = IOL_DL_ISDUH_ST_ISDURESPONSE_5; // T6
            }
         }
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ISDUH_ST_ISDUWAIT_3: unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_isdu_h_sm_isdu_response5 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (
      (dl->message_handler.rwcmd == IOL_MHRW_ISDUABORT) || // T18
      (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST))  // T10
   {
      iolink_dl_isdu_h_sm_enter_isduerror4 (port);
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
   {
      OD_req (
         port,
         IOLINK_RWDIRECTION_READ,
         IOLINK_COMCHANNEL_ISDU,
         dl->isdu_handler.current_isdu_seg % 16,
         dl->message_handler.od_len,
         &dl->isdu_handler.isdu_data
               [dl->message_handler.od_len * dl->isdu_handler.current_isdu_seg]);
      // T7
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE)
   {
      uint8_t isdu_total_len = 0;
      get_isdu_total_len (dl, &isdu_total_len);
      dl->isdu_handler.total_isdu_len = isdu_total_len;
      dl->isdu_handler.total_isdu_seg = get_isdu_total_segments (dl);
      memcpy (
         &dl->isdu_handler.isdu_data
               [dl->isdu_handler.current_isdu_seg * dl->message_handler.od_len],
         dl->rxbuffer,
         dl->message_handler.od_len);
      dl->isdu_handler.current_isdu_seg++; // T7 response

      if (dl->isdu_handler.current_isdu_seg >= dl->isdu_handler.total_isdu_seg)
      {
         iolink_dl_isdu_h_sm_reception_complete (port);
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ISDUH_ST_ISDURESPONSE_5: unknown event triggered.\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_isdu_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->isdu_handler.state)
   {
   case IOL_DL_ISDUH_ST_INACTIVE_0:
      iolink_dl_isdu_h_sm_inactive0 (port);
      break;
   case IOL_DL_ISDUH_ST_IDLE_1:
      iolink_dl_isdu_h_sm_idle1 (port);
      break;
   case IOL_DL_ISDUH_ST_ISDUREQUEST_2:
      iolink_dl_isdu_h_sm_isdu_request2 (port);
      break;
   case IOL_DL_ISDUH_ST_ISDUWAIT_3:
      iolink_dl_isdu_h_sm_isdu_wait3 (port);
      break;
   case IOL_DL_ISDUH_ST_ISDUERROR_4:
      iolink_dl_isdu_h_sm_isdu_error4 (port);
      break;
   case IOL_DL_ISDUH_ST_ISDURESPONSE_5:
      iolink_dl_isdu_h_sm_isdu_response5 (port);
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d:\n",
         iolink_get_portnumber (port),
         __func__,
         dl->isdu_handler.state);
      break;
   }
}

static void iolink_dl_cmd_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   // T2 handled in PDInStatus_ind
   switch (dl->cmd_handler.state)
   {
   case IOL_DL_CMDH_ST_INACTIVE_0:
      if (dl->cmd_handler.chcmd == IOL_CHCMD_ACTIVE)
      {
         dl->cmd_handler.state = IOL_DL_CMDH_ST_IDLE_1; // T1
      }
      else if (dl->cmd_handler.chcmd != IOL_CHCMD_INACTIVE)
      {
         LOG_ERROR (
            IOLINK_DL_LOG,
            "DL (%u): %s: INACTIVE_0: unknown event triggered\n",
            iolink_get_portnumber (port),
            __func__);
      }
      break;
   case IOL_DL_CMDH_ST_IDLE_1:
      if (dl->cmd_handler.chcmd == IOL_CHCMD_INACTIVE)
      {
         dl->cmd_handler.state = IOL_DL_CMDH_ST_INACTIVE_0; // T6
      }
      else if (
         (dl->cmd_handler.control_code == IOLINK_CONTROLCODE_PDOUTVALID) ||
         (dl->cmd_handler.control_code == IOLINK_CONTROLCODE_PDOUTINVALID))
      {
         os_mutex_lock (dl->mtx);
         dl->cmd_handler.master_command =
            (dl->cmd_handler.control_code == IOLINK_CONTROLCODE_PDOUTVALID)
               ? IOL_MASTERCMD_DEVICE_PD_OUT_OP
               : IOL_MASTERCMD_DEVICE_OPERATE;
         os_mutex_unlock (dl->mtx);
         dl->cmd_handler.state = IOL_DL_CMDH_ST_MASTERCOMMAND_2; // T3
      }
      else if (dl->cmd_handler.control_code == IOLINK_CONTROLCODE_DEVICEMODE)
      {
         // Master command already set
         dl->cmd_handler.state = IOL_DL_CMDH_ST_MASTERCOMMAND_2; // T4
      }
      else
      {
         LOG_ERROR (
            IOLINK_DL_LOG,
            "DL (%u): %s: IDLE_1: unknown event triggered\n",
            iolink_get_portnumber (port),
            __func__);
      }
      break;
   case IOL_DL_CMDH_ST_MASTERCOMMAND_2:
      OD_req (
         port,
         IOLINK_RWDIRECTION_WRITE,
         IOLINK_COMCHANNEL_PAGE,
         0,
         1,
         &dl->cmd_handler.master_command);
      dl->cmd_handler.control_code = IOLINK_CONTROLCODE_NONE;
      dl->cmd_handler.state        = IOL_DL_CMDH_ST_IDLE_1; // T5
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_IDLE_1: Invalid state: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->cmd_handler.state);
      break;
   }
}

static void iolink_dl_ev_h_sm_inactive0 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->event_handler.ehcmd == IOL_EHCMD_ACTIVE)
   {
      dl->event_handler.state = IOL_DL_EVH_ST_IDLE_1; // T1
   }
   else if (dl->event_handler.ehcmd != IOL_EHCMD_INACTIVE)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_IDLE_1: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_ev_h_sm_idle1 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->event_handler.ehcmd == IOL_EHCMD_INACTIVE)
   {
      dl->event_handler.state = IOL_DL_EVH_ST_INACTIVE_0; // T10
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE)
   {
      if (dl->event_handler.event_confirmation)
      {
         LOG_DEBUG (
            IOLINK_DL_LOG,
            "DL (%u): %s: IOL_DL_EVH_ST_IDLE_1: Event confirmation.\n",
            iolink_get_portnumber (port),
            __func__);
         dl->event_handler.state = IOL_DL_EVH_ST_EVENTCONFIRMATION_4; // T7

         if (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST)
         {
            dl->event_handler.state = IOL_DL_EVH_ST_INACTIVE_0; // T9
         }
         else
         {
            LOG_DEBUG (
               IOLINK_DL_LOG,
               "DL (%u): %s: IOL_DL_EVH_ST_IDLE_1: Written %d to StatusCode.\n",
               iolink_get_portnumber (port),
               __func__,
               dl->txbuffer[0]);
            OD_req (
               port,
               IOLINK_RWDIRECTION_WRITE,
               IOLINK_COMCHANNEL_DIAGNOSIS,
               0,
               1,
               dl->txbuffer);
            dl->event_handler.event_confirmation = false;
            dl->event_handler.state = IOL_DL_EVH_ST_IDLE_1; // T8
         }
      }
      else
      {
         dl->event_handler.ev_addr     = 0;
         dl->event_handler.num_entries = 0;
         dl->event_handler.ev_current  = 0;
         memset (
            dl->event_handler.events,
            0,
            sizeof (event_t) * IOLINK_MAX_EVENTS);
         OD_req (
            port,
            IOLINK_RWDIRECTION_READ,
            IOLINK_COMCHANNEL_DIAGNOSIS,
            dl->event_handler.ev_addr,
            0,
            0);
         dl->event_handler.state = IOL_DL_EVH_ST_READEVENT_2; // T2
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_IDLE_1: unknown event triggered."
         " Trigger: %d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.trigger);
   }
}

static void iolink_dl_ev_h_sm_read_event2 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mode_handler.mhinfo == IOLINK_MHINFO_COMLOST)
   {
      dl->event_handler.state = IOL_DL_EVH_ST_INACTIVE_0; // T6
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE) // T3
   {
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Issue read address %d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->event_handler.ev_addr);
      OD_req (
         port,
         IOLINK_RWDIRECTION_READ,
         IOLINK_COMCHANNEL_DIAGNOSIS,
         dl->event_handler.ev_addr,
         0,
         0);
   }
   else if (dl->od_handler.trigger == IOL_TRIGGERED_DEVICE_MESSAGE) // T3
   {
      if (dl->event_handler.ev_addr == 0)
      {
         iolink_dl_ev_h_sm_read_event2_for_device_status_code (port);
      }
      else
      {
         iolink_dl_ev_h_sm_read_event2_for_device_event_memory_byte (port);
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_ev_h_sm_read_event2_for_device_status_code (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->event_handler.status_code = dl->rxbuffer[0];

   if (dl->event_handler.status_code & 0x80) // Details (StatusCode
                                             // type 2)
   {
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Read address %d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->event_handler.ev_addr);
      // Event details
      for (dl->event_handler.ev_cnt = 0;
            dl->event_handler.ev_cnt < IOLINK_MAX_EVENTS;
            dl->event_handler.ev_cnt++)
      {
         if (dl->event_handler.status_code & (1 << dl->event_handler.ev_cnt))
         {
            dl->event_handler.num_entries++;

            if (dl->event_handler.ev_addr == 0)
            {
               dl->event_handler.ev_addr =
                  1 + dl->event_handler.ev_cnt * 3;
            }
         }
      }
      dl->event_handler.ev_cnt = (dl->event_handler.ev_addr - 1) / 3;
   }
   else // No details (StatusCode type 1)
   {
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: No details event (StatusCode: 0x%X)\n",
         iolink_get_portnumber (port),
         __func__,
         dl->event_handler.status_code);

      dl->pd_handler.pd_valid =
         ((dl->event_handler.status_code & BIT (6)) == 0);

      // Note: There are only 5 events for type 1
      uint8_t i;
      for (i = 0; i < IOLINK_MAX_EVENTS - 1; i++)
      {
         if ((dl->event_handler.status_code & BIT (i)) != 0)
         {
            dl->event_handler.num_entries++;
         }
      }

      iolink_dl_ev_h_sm_signal_event3 (port);
   }
}

static void iolink_dl_ev_h_sm_read_event2_for_device_event_memory_byte (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->event_handler.ev_addr - (dl->event_handler.ev_cnt * 3))
   {
   case 1:
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Read qualifier: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->rxbuffer[0]);
      dl->event_handler.events[dl->event_handler.ev_current]
         .event_qualifier = dl->rxbuffer[0];
      dl->event_handler.ev_addr++;
      break;
   case 2:
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Read MSB: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->rxbuffer[0]);
      dl->event_handler.events[dl->event_handler.ev_current]
         .event_code_msb = dl->rxbuffer[0];
      dl->event_handler.ev_addr++;
      break;
   case 3:
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Read LSB: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->rxbuffer[0]);
      dl->event_handler.events[dl->event_handler.ev_current]
         .event_code_lsb = dl->rxbuffer[0];
      dl->event_handler.ev_current++;

      while (dl->event_handler.ev_cnt < IOLINK_MAX_EVENTS)
      {
         dl->event_handler.ev_cnt++;

         if (dl->event_handler.status_code & (1 << dl->event_handler.ev_cnt))
         {
            dl->event_handler.ev_addr = 1 + dl->event_handler.ev_cnt * 3;
            break;
         }
      }

      if (dl->event_handler.ev_cnt >= (IOLINK_MAX_EVENTS))
      {
         if (dl->event_handler.ev_current != dl->event_handler.num_entries)
         {
            LOG_ERROR (
               IOLINK_DL_LOG,
               "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2:"
               " Mismatch between number of events indicated and "
               "read\n",
               iolink_get_portnumber (port),
               __func__);
         }
         dl->event_handler.ev_current = 0;

         iolink_dl_ev_h_sm_signal_event3 (port);
      }
      break;
   default:
      /* This should never happen */
      break;
   }
}

static void iolink_dl_ev_h_sm_signal_event3 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Signal events\n",
      iolink_get_portnumber (port),
      __func__);
   dl->event_handler.state = IOL_DL_EVH_ST_SIGNALEVENT_3; // T4

   if (dl->event_handler.status_code & 0x80) // Details (StatusCode type 2)
   {
      for (dl->event_handler.ev_current = 0;
            dl->event_handler.ev_current <
            dl->event_handler.num_entries;
            dl->event_handler.ev_current++)
      {
         event_t * event =
            &dl->event_handler.events[dl->event_handler.ev_current];

         DL_Event_ind (
            port,
            event->event_code_lsb + (event->event_code_msb << 8),
            event->event_qualifier,
            dl->event_handler.num_entries -
               dl->event_handler.ev_current - 1);
      }
   }
   else // No details (StatusCode type 1)
   {
      uint8_t i;
      typedef struct
      {
         uint16_t code;
         iolink_event_type_t type;
      } event_info_t;

      const event_info_t event_info[] = {
         {0xFF80u, IOLINK_EVENT_TYPE_NOTIFICATION},
         {0xFF80u, IOLINK_EVENT_TYPE_WARNING},
         {0x6320u, IOLINK_EVENT_TYPE_ERROR},
         {0xFF80u, IOLINK_EVENT_TYPE_ERROR},
         {0xFF10u, IOLINK_EVENT_TYPE_ERROR}};

      for (i = 0; i < (IOLINK_MAX_EVENTS - 1); i++)
      {
         if ((dl->event_handler.status_code & BIT (i)) != 0)
         {
            dl->event_handler.num_entries--;
            uint8_t event_qualifier =
               IOLINK_EVENT_INSTANCE_APPLICATION |
               (IOLINK_EVENT_SOURCE_DEVICE << 3) |
               (event_info[i].type << 4) |
               (IOLINK_EVENT_MODE_SINGLE_SHOT << 6);
            DL_Event_ind (
               port,
               event_info[i].code,
               event_qualifier,
               dl->event_handler.num_entries);
         }
      }
   }

   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: IOL_DL_EVH_ST_READEVENT_2: Return to IDLE\n",
      iolink_get_portnumber (port),
      __func__);
   dl->event_handler.event_confirmation = true;
   dl->event_handler.state = IOL_DL_EVH_ST_IDLE_1; // T5
}

static void iolink_dl_ev_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->event_handler.state)
   {
   case IOL_DL_EVH_ST_INACTIVE_0:
      iolink_dl_ev_h_sm_inactive0 (port);
      break;
   case IOL_DL_EVH_ST_IDLE_1:
      iolink_dl_ev_h_sm_idle1 (port);
      break;
   case IOL_DL_EVH_ST_READEVENT_2:
      iolink_dl_ev_h_sm_read_event2 (port);
      break;
   case IOL_DL_EVH_ST_SIGNALEVENT_3:
   case IOL_DL_EVH_ST_EVENTCONFIRMATION_4:
      // Not used. Inlined
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_EVH_ST_%s: State not used, yet entered.\n",
         iolink_get_portnumber (port),
         __func__,
         (dl->event_handler.state == IOL_DL_EVH_ST_SIGNALEVENT_3)
            ? "SIGNALEVENT_3"
            : "EVENTCONFIRMATION_4");
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->event_handler.state);
      break;
   }
}

static void iolink_dl_od_h_sm_inactive0 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->od_handler.ohcmd == IOL_OHCMD_ACTIVE)
   {
      dl->od_handler.state = IOL_DL_ODH_ST_ISDU_1; // T1
   }
   else if (dl->od_handler.ohcmd != IOL_OHCMD_INACTIVE)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: INACTIVE_0: unknown event triggered\n",
         iolink_get_portnumber (port),
         __func__);
   }
}

static void iolink_dl_od_h_sm_isdu1 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->od_handler.ohcmd == IOL_OHCMD_INACTIVE)
   {
      dl->od_handler.state = IOL_DL_ODH_ST_INACTIVE_0; // T13
   }
   else if (
      (dl->cmd_handler.control_code != IOLINK_CONTROLCODE_NONE) &&
      (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE))
   {
      iolink_dl_cmd_h_sm (port);
      dl->od_handler.state = IOL_DL_ODH_ST_COMMAND_2; // T3
   }
   else if (dl->event_handler.event_flag)
   {
      iolink_dl_isdu_h_sm (port);
      dl->od_handler.state = IOL_DL_ODH_ST_EVENT_3; // T5
   }
   else if (dl->od_handler.trigger != IOL_TRIGGERED_NONE) // T2
   {
      iolink_dl_isdu_h_sm (port);
      dl->od_handler.trigger = IOL_TRIGGERED_NONE;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ODH_ST_ISDU_1: unknown event triggered. Trigger: %d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.trigger);
   }
}

static void iolink_dl_od_h_sm_command2 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->od_handler.ohcmd == IOL_OHCMD_INACTIVE)
   {
      dl->cmd_handler.state = IOL_DL_CMDH_ST_INACTIVE_0; // T11
   }
   else if (dl->od_handler.trigger != IOL_TRIGGERED_NONE) // T9
   {
      dl->od_handler.trigger = IOL_TRIGGERED_NONE;

      if (dl->cmd_handler.control_code == IOLINK_CONTROLCODE_NONE)
      {
         write_master_command (port, IOLINK_STATUS_NO_ERROR);

         if (dl->event_handler.event_flag)
         {
            dl->od_handler.state = IOL_DL_ODH_ST_EVENT_3; // T8
         }
         else
         {
            dl->od_handler.state = IOL_DL_ODH_ST_ISDU_1; // T4
         }
      }
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ODH_ST_COMMAND_2: unknown event triggered. Trigger: "
         "%d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.trigger);
   }
}

static void iolink_dl_od_h_sm_event3 (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->od_handler.ohcmd == IOL_OHCMD_INACTIVE)
   {
      dl->od_handler.state = IOL_DL_ODH_ST_INACTIVE_0; // T12
   }
   else if (
      (dl->cmd_handler.control_code != IOLINK_CONTROLCODE_NONE) &&
      (dl->od_handler.trigger == IOL_TRIGGERED_MASTER_MESSAGE))
   {
      iolink_dl_cmd_h_sm (port);
      dl->od_handler.state = IOL_DL_ODH_ST_COMMAND_2; // T7
   }
   else if (!dl->event_handler.event_flag)
   {
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: Event ended. Back to ISDU. Trigger: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.trigger);
      dl->od_handler.state = IOL_DL_ODH_ST_ISDU_1; // T6
   }
   else if (dl->od_handler.trigger != IOL_TRIGGERED_NONE) // T10
   {
      iolink_dl_ev_h_sm (port);
      dl->od_handler.trigger = IOL_TRIGGERED_NONE;
   }
   else
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: IOL_DL_ODH_ST_EVENT_3: unknown event triggered. Trigger: "
         "%d.\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.trigger);
   }
}

static void iolink_dl_od_h_sm (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->od_handler.state)
   {
   case IOL_DL_ODH_ST_INACTIVE_0:
      iolink_dl_od_h_sm_inactive0 (port);
      break;
   case IOL_DL_ODH_ST_ISDU_1:
      iolink_dl_od_h_sm_isdu1 (port);
      break;
   case IOL_DL_ODH_ST_COMMAND_2:
      iolink_dl_od_h_sm_command2 (port);
      break;
   case IOL_DL_ODH_ST_EVENT_3:
      iolink_dl_od_h_sm_event3 (port);
      break;
   default:
      /* This should never happen */
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Invalid state: %d:\n",
         iolink_get_portnumber (port),
         __func__,
         dl->od_handler.state);
      break;
   }
}

/*
 * DL-A services
 */
static iolink_error_t OD_req (
   iolink_port_t * port,
   iolink_rwdirection_t rwdirection,
   iolink_comchannel_t comchannel,
   uint8_t addressctrl,
   uint8_t length,
   uint8_t * data)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (length > 32)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: length not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         length);

      return IOLINK_ERROR_ODLENGTH;
   }

   if (addressctrl > 31)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: address not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         addressctrl);

      return IOLINK_ERROR_ADDRESS_INVALID;
   }

   if (
      (comchannel != IOLINK_COMCHANNEL_DIAGNOSIS) &&
      (comchannel != IOLINK_COMCHANNEL_PAGE) &&
      (comchannel != IOLINK_COMCHANNEL_ISDU) &&
      (comchannel != IOLINK_COMCHANNEL_PROCESS))
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: com channel not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         comchannel);

      return IOLINK_ERROR_COMCHANNEL_INVALID;
   }

   dl->txbuffer[0] = rwdirection | comchannel | addressctrl;
   dl->txbuffer[1] = (dl->mseq & 0xF0) << 2;
   // Interleave mode, PD-data-sequence

   if (comchannel != IOLINK_COMCHANNEL_PROCESS)
   {
      if (rwdirection == IOLINK_RWDIRECTION_WRITE)
      {
         dl->od_handler.od_txlen = dl->message_handler.od_len;
         dl->od_handler.od_rxlen = 0;
         os_mutex_lock (dl->mtx);
         memcpy (dl->od_handler.odout_buffer, data, length);

         if (length < dl->message_handler.od_len)
         {
            memset (
               &dl->od_handler.odout_buffer[length],
               0,
               dl->message_handler.od_len - length);
         }

         os_mutex_unlock (dl->mtx);
      }
      else if (rwdirection == IOLINK_RWDIRECTION_READ)
      {
         dl->od_handler.od_txlen = 0;
         dl->od_handler.od_rxlen = dl->message_handler.od_len;
      }
   }

   return IOLINK_ERROR_NONE;
}

static iolink_error_t PD_req (
   iolink_port_t * port,
   uint8_t pdinaddress,
   uint8_t pdinlength,
   uint8_t * pdout,
   uint8_t pdoutaddress,
   uint8_t pdoutlength)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (pdinlength > 32)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: pdinlength not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         pdinlength);

      return IOLINK_ERROR_PDINLENGTH;
   }
   else if (pdoutlength > 32)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: pdoutlength not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         pdoutlength);

      return IOLINK_ERROR_PDOUTLENGTH;
   }
   else if (pdout != NULL)
   {
      os_mutex_lock (dl->mtx);
      memcpy (dl->pd_handler.pdout_buffer, &pdout[pdoutaddress], pdoutlength);
      os_mutex_unlock (dl->mtx);
   }

   return IOLINK_ERROR_NONE;
}

/*
 * DL-B services
 */
static iolink_error_t OD_cnf (
   iolink_dl_t * dl,
   uint8_t * data,
   uint8_t length,
   iolink_status_t errorinfo)
{
   return IOLINK_ERROR_NONE;
}

static iolink_error_t PDInStatus_ind (
   iolink_port_t * port,
   iolink_controlcode_t status)
{
   if (status == IOLINK_CONTROLCODE_NONE)
   {
      return IOLINK_ERROR_NONE;
   }
   else if ((status == IOLINK_CONTROLCODE_VALID) || (status == IOLINK_CONTROLCODE_INVALID)) // cmdh_T2
   {
      DL_Control_ind (port, status);

      return IOLINK_ERROR_NONE;
   }

   return IOLINK_ERROR_STATUS_INVALID;
}

static iolink_error_t EventFlag_ind (iolink_dl_t * dl, bool eventflag)
{
   dl->event_handler.event_flag = eventflag;

   return IOLINK_ERROR_NONE;
}

static iolink_error_t MHInfo_ind (iolink_port_t * port, iolink_mhinfo_t mhinfo)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);
   dl->mode_handler.mhinfo = mhinfo;
   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: Mode H triggered by MHInfo\n",
      iolink_get_portnumber (port),
      __func__);
   os_event_set (dl->event, IOLINK_DL_EVENT_MDH);

   return IOLINK_ERROR_NONE;
}

static iolink_error_t DL_ReadWrite_req (
   iolink_port_t * port,
   uint8_t address,
   uint8_t value,
   bool write)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->message_handler.state != IOL_DL_MH_ST_STARTUP_2)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s (%s): State not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         (write) ? "Write" : "Read",
         dl->message_handler.state);

      if (write)
      {
         DL_Write_cnf (port, IOLINK_STATUS_STATE_CONFLICT);
      }
      else
      {
         DL_Read_cnf (port, 0, IOLINK_STATUS_STATE_CONFLICT);
      }

      return IOLINK_ERROR_STATE_INVALID;
   }
   else if (address > 15)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s (%s): Address not valid. Must be 15 or less: %d\n",
         iolink_get_portnumber (port),
         __func__,
         (write) ? "Write" : "Read",
         address);

      if (write)
      {
         DL_Write_cnf (port, IOLINK_STATUS_VALUE_OUT_OF_RANGE);
      }
      else
      {
         DL_Read_cnf (port, 0, IOLINK_STATUS_VALUE_OUT_OF_RANGE);
      }

      return IOLINK_ERROR_ADDRESS_INVALID;
   }

#if IOLINK_HW == IOLINK_HW_MAX14819
   if (!write)
   {
      /* MAX14819 will make the initial read of MIN_CYCL. Hence, ignore this
       * first read req. */
      if (address == IOL_DIR_PARAMA_MIN_CYCL)
      {
         if (dl->first_read_min_cycl)
         {
            dl->first_read_min_cycl = false;
            DL_Read_cnf (port, dl->cycbyte, IOLINK_STATUS_NO_ERROR);

            return IOLINK_ERROR_NONE;
         }
      }
   }
#endif /* IOLINK_HW_MAX14819 */

   dl->txbuffer[0] =
      ((write) ? IOLINK_RWDIRECTION_WRITE : IOLINK_RWDIRECTION_READ) |
      IOLINK_COMCHANNEL_PAGE | address;
   dl->txbuffer[1] = (IOLINK_MSEQTYPE_TYPE_0 & 0xF0) << 2;

   if (write)
   {
      dl->txbuffer[2] = value;
   }

   dl->od_handler.od_rxlen   = (write) ? 0 : 1;
   dl->od_handler.od_txlen   = (write) ? 1 : 0;
   dl->message_handler.rwcmd = (write) ? IOL_MHRW_WRITE : IOL_MHRW_READ;
   // LOG_DEBUG(IOLINK_DL_LOG, "DL (%u): Message H triggered by DL_%s\n", iolink_get_portnumber (port), (write) ?
   // "Write" : "Read");
   os_event_set (dl->event, IOLINK_DL_EVENT_MH);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_Read_req (iolink_port_t * port, uint8_t address)
{
   return DL_ReadWrite_req (port, address, 0, false);
}

iolink_error_t DL_Write_req (iolink_port_t * port, uint8_t address, uint8_t value)
{
   return DL_ReadWrite_req (port, address, value, true);
}

iolink_error_t DL_SetMode_req (
   iolink_port_t * port,
   iolink_dl_mode_t mode,
   iolink_mode_vl_t * valuelist)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (
      (mode != IOLINK_DLMODE_INACTIVE) && (mode != IOLINK_DLMODE_STARTUP) &&
      (mode != IOLINK_DLMODE_PREOPERATE) && (mode != IOLINK_DLMODE_OPERATE))
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Mode not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         mode);

      return IOLINK_ERROR_MODE_INVALID;
   }

   dl->mode_handler.dl_mode = mode;
   dl->mseq                 = valuelist->type;
   dl->cycbyte              = valuelist->time;

   /* Don't assign the message_handler members if we switch to inactive.
    * When we switch to inactive, the communication is still active while we
    * assign the message_handler members below. For inactive, we will set
    * pd_rxlen and od_len to 0. In iolink_pl_get_data() there is a sanity check
    * of length of incoming data, based on the value of pd_rxlen and od_len.
    */
   if (mode != IOLINK_DLMODE_INACTIVE)
   {
      dl->message_handler.pd_rxlen = valuelist->pdinputlength;
      dl->message_handler.pd_txlen = valuelist->pdoutputlength;
      dl->message_handler.od_len   = valuelist->onreqdatalengthpermessage;
   }

   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s: Mode H triggered by DL_SetMode (mode = %d)\n",
      iolink_get_portnumber (port),
      __func__,
      mode);
   os_event_set (dl->event, IOLINK_DL_EVENT_MDH);

   return IOLINK_ERROR_NONE;
}

static iolink_error_t DL_ReadWriteParam_req (
   iolink_port_t * port,
   uint16_t address,
   uint8_t value,
   bool write)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->message_handler.rwcmd = (write) ? IOL_MHRW_WRITEPARAM
                                       : IOL_MHRW_READPARAM;

   if (
      (dl->message_handler.state != IOL_DL_MH_ST_PREOPERATE_6) &&
      (dl->message_handler.state != IOL_DL_MH_ST_AW_REPLY_16))
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s (%s): State not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         (write) ? "Write" : "Read",
         dl->message_handler.state);

      if (write)
      {
         DL_WriteParam_cnf (port, IOLINK_STATUS_STATE_CONFLICT);
      }
      else
      {
         DL_ReadParam_cnf (port, 0, IOLINK_STATUS_STATE_CONFLICT);
      }

      return IOLINK_ERROR_STATE_INVALID;
   }
   else if (address > 31)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s (%s): Address not valid. Must be 31 or less: %d\n",
         iolink_get_portnumber (port),
         __func__,
         (write) ? "Write" : "Read",
         address);

      if (write)
      {
         DL_WriteParam_cnf (port, IOLINK_STATUS_VALUE_OUT_OF_RANGE);
      }
      else
      {
         DL_ReadParam_cnf (port, 0, IOLINK_STATUS_VALUE_OUT_OF_RANGE);
      }

      return IOLINK_ERROR_ADDRESS_INVALID;
   }

   os_mutex_lock (dl->mtx);
   dl->od_handler.data_addr = address;

   if (write)
   {
      dl->od_handler.data_value = value;
   }

   os_mutex_unlock (dl->mtx);

   if (dl->mode_handler.state == IOL_DL_MDH_ST_PREOPERATE_3)
   {
      os_event_set (dl->event, IOLINK_DL_EVENT_MH);
   }

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_ReadParam_req (iolink_port_t * port, uint16_t address)
{
   return DL_ReadWriteParam_req (port, address, 0, false);
}

iolink_error_t DL_WriteParam_req (iolink_port_t * port, uint16_t address, uint8_t value)
{
   return DL_ReadWriteParam_req (port, address, value, true);
}

iolink_error_t DL_ISDUTransport_req (
   iolink_port_t * port,
   iolink_isdu_vl_t * valuelist)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   uint8_t iserv       = 0;
   uint8_t length      = 0;
   uint8_t overheadlen = 0;
   uint8_t dataoffset  = 1;

   if (
      (dl->mode_handler.state == IOL_DL_MDH_ST_IDLE_0) ||
      (dl->mode_handler.state == IOL_DL_MDH_ST_ESTCOM_1))
   {
      DL_ISDUTransport_cnf (port, 0, 0, 0, IOLINK_STATUS_NO_COMM);

      return IOLINK_ERROR_COMCHANNEL_INVALID;
   }
   else if (valuelist->index < 2)
   {
      DL_ISDUTransport_cnf (port, 0, 0, 0, IOLINK_STATUS_VALUE_OUT_OF_RANGE);

      return IOLINK_ERROR_ADDRESS_INVALID;
   }
   else if (dl->isdu_handler.state != IOL_DL_ISDUH_ST_IDLE_1)
   {
      DL_ISDUTransport_cnf (port, 0, 0, 0, IOLINK_STATUS_STATE_CONFLICT);

      return IOLINK_ERROR_INCORRECT_DATA;
   }

   dl->isdu_handler.data_dir_read = false;

   if (valuelist->readwrite == IOLINK_RWDIRECTION_READ)
   {
      dl->isdu_handler.data_dir_read = true;
      iserv                          = 0x08;
   }

   if (valuelist->index < 256)
   {
      if (valuelist->subindex == 0)
      {
         iserv |= 0x01;
         overheadlen = 3;
      }
      else
      {
         iserv |= 0x02;
         overheadlen = 4;
      }
   }
   else
   {
      iserv |= 0x03;
      overheadlen = 5;
   }

   os_mutex_lock (dl->mtx);
   memset (dl->isdu_handler.isdu_data, 0, IOLINK_OD_MAX_SIZE);

   if (valuelist->length <= 15 - overheadlen)
   {
      length = valuelist->length + overheadlen;
   }
   else
   {
      length = 1;
      overheadlen++;
      dl->isdu_handler.isdu_data[1] = valuelist->length + overheadlen;
      dataoffset                    = 2;
   }

   dl->isdu_handler.isdu_data[0] = (iserv << 4) | (length & 0x0F);

   if (valuelist->index > 255)
   {
      dl->isdu_handler.isdu_data[dataoffset] = ((valuelist->index >> 8) & 0xFF);
      dl->isdu_handler.isdu_data[dataoffset + 1] = (valuelist->index) & 0xFF;
      dl->isdu_handler.isdu_data[dataoffset + 2] = valuelist->subindex;
      dataoffset += 3;
   }
   else
   {
      dl->isdu_handler.isdu_data[dataoffset] = (valuelist->index) & 0xFF;
      dataoffset++;

      if (valuelist->subindex > 0)
      {
         dl->isdu_handler.isdu_data[dataoffset] = valuelist->subindex;
         dataoffset++;
      }
   }

   memcpy (
      &dl->isdu_handler.isdu_data[dataoffset],
      valuelist->data_write,
      valuelist->length);
   dataoffset += valuelist->length;
   dl->isdu_handler.isdu_data[dataoffset] =
      calcCHKPDU (dl->isdu_handler.isdu_data, dataoffset);
   memset (
      &dl->isdu_handler.isdu_data[dataoffset + 1],
      0,
      IOLINK_ISDU_MAX_SIZE - (dataoffset + 1));
   dl->isdu_handler.total_isdu_len   = valuelist->length + overheadlen;
   dl->isdu_handler.total_isdu_seg   = get_isdu_total_segments (dl);
   dl->isdu_handler.current_isdu_seg = 0;
   dl->message_handler.rwcmd         = IOL_MHRW_ISDUTRANSPORT;
   os_mutex_unlock (dl->mtx);

   if (dl->mode_handler.state == IOL_DL_MDH_ST_PREOPERATE_3)
   {
      os_event_set (dl->event, IOLINK_DL_EVENT_MH);
   }

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_Control_req (iolink_port_t * port, iolink_controlcode_t controlcode)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (
      (controlcode != IOLINK_CONTROLCODE_VALID) &&
      (controlcode != IOLINK_CONTROLCODE_INVALID) &&
      (controlcode != IOLINK_CONTROLCODE_PDOUTVALID) &&
      (controlcode != IOLINK_CONTROLCODE_PDOUTINVALID))
   {
      return IOLINK_ERROR_CONTROLCODE_INVALID;
   }

   dl->cmd_handler.control_code = controlcode;
   iolink_dl_cmd_h_sm (port);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_Write_Devicemode_req (
   iolink_port_t * port,
   iolink_dl_mode_t devicemode)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->mode_handler.state == IOL_DL_MDH_ST_STARTUP_2)
   {
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: State not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->mode_handler.state);
      DL_Write_Devicemode_cnf (
         port,
         IOLINK_STATUS_STATE_CONFLICT,
         IOL_MASTERCMD_NONE); /* Not in spec. */

      return IOLINK_ERROR_STATE_INVALID;
   }

   os_mutex_lock (dl->mtx);

   switch (devicemode)
   {
   case IOLINK_DLMODE_INACTIVE:
      dl->cmd_handler.master_command = IOL_MASTERCMD_FALLBACK;
      break;
   case IOLINK_DLMODE_STARTUP: // cmd_handler is not started yet....
      dl->cmd_handler.master_command = IOL_MASTERCMD_DEVICE_STARTUP;
      break;
   case IOLINK_DLMODE_PREOPERATE:
      dl->cmd_handler.master_command = IOL_MASTERCMD_DEVICE_PREOP;
      break;
   case IOLINK_DLMODE_OPERATE:
      dl->cmd_handler.master_command = IOL_MASTERCMD_DEVICE_OPERATE;
      break;
   default:
      os_mutex_unlock (dl->mtx);

      return IOLINK_ERROR_INCORRECT_DATA; // TODO correct error?
   }
   os_mutex_unlock (dl->mtx);

   dl->cmd_handler.control_code = IOLINK_CONTROLCODE_DEVICEMODE;
   iolink_dl_cmd_h_sm (port);

   if (dl->mode_handler.state == IOL_DL_MDH_ST_PREOPERATE_3)
   {
      if (devicemode == IOLINK_DLMODE_INACTIVE)
      {
         /* Respond with Fallback when we request INACTIVE and we are in preop
          * Problem seen after TC_0276
          */
         DL_Write_Devicemode_cnf (
            port,
            IOLINK_STATUS_NO_ERROR,
            IOL_MASTERCMD_FALLBACK); /* Not in spec. */
      }
      else
      {
         LOG_DEBUG (
            IOLINK_DL_LOG,
            "DL (%u): %s: Message H triggered by DL_Write_Devicemode_req\n",
            iolink_get_portnumber (port),
            __func__);
         os_event_set (dl->event, IOLINK_DL_EVENT_MH);
      }
   }

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_EventConf_req (iolink_port_t * port) // TODO not used
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   dl->event_handler.event_confirmation = true;

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_PDOutputGet_req (iolink_port_t * port, uint8_t * len, uint8_t * data)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   os_mutex_lock (dl->mtx);
   memcpy (data, dl->pd_handler.pdoutdata, dl->pd_handler.pd_txlen);
   *len = dl->pd_handler.pd_txlen;
   os_mutex_unlock (dl->mtx);

   return IOLINK_ERROR_NONE;
}

iolink_error_t DL_PDOutputUpdate_req (iolink_port_t * port, uint8_t * outputdata)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   if (dl->pd_handler.state == IOL_DL_PDH_ST_INACTIVE_0)
   {
      return IOLINK_ERROR_CONDITIONS_NOT_CORRECT;
   }

   os_mutex_lock (dl->mtx);
   memcpy (dl->pd_handler.pdoutdata, outputdata, dl->pd_handler.pd_txlen);

   if ((dl->pd_handler.pd_txlen & 1) == 1) // Odd number of bytes, add
                                           // additional 0
   { // (needed for interleaved communication)
      dl->pd_handler.pdoutdata[dl->pd_handler.pd_txlen] = 0;
   }

   os_mutex_unlock (dl->mtx);

   return IOLINK_ERROR_NONE;
}

static void dl_timer_timeout (os_timer_t * timer, void * arg)
{
   os_timer_stop (timer);

   iolink_dl_t * dl = (iolink_dl_t *)arg;
   os_event_set (dl->event, IOLINK_DL_EVENT_TIMEOUT);
}

static void dl_timer_tcyc_timeout (os_timer_t * timer, void * arg)
{
   os_timer_stop (timer);

   iolink_dl_t * dl = (iolink_dl_t *)arg;
   os_event_set (dl->event, IOLINK_DL_EVENT_TIMEOUT_TCYC);
}

static void iolink_dl_wurq_recv (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);
   dl->baudrate     = iolink_pl_get_baudrate (port);

   if (dl->baudrate == IOLINK_BAUDRATE_NONE)
   {
      // WURQ Failed
      dl->mode_handler.state  = IOL_DL_MDH_ST_IDLE_0; // mdh_T5
      dl->mode_handler.mhinfo = IOLINK_MHINFO_NONE;
      DL_Mode_ind (port, IOLINK_MHMODE_INACTIVE);

      return;
   }

   dl->cycbyte               = iolink_pl_get_cycletime (port);
   dl->mode_handler.state    = IOL_DL_MDH_ST_STARTUP_2; // mdh_T2/T3/T4
   dl->message_handler.state = IOL_DL_MH_ST_AW_REPLY_1; // mh_T1
   iolink_dl_message_h_sm (port);
   CH_Conf (port, IOL_CHCMD_ACTIVE);
   DL_Mode_ind (port, IOLINK_MHMODE_STARTUP);

   switch (dl->baudrate)
   {
   case IOLINK_BAUDRATE_COM1: // mdh_T2
      DL_Mode_ind_baud (port, IOLINK_MHMODE_COM1);
      break;
   case IOLINK_BAUDRATE_COM2: // mdh_T3
      DL_Mode_ind_baud (port, IOLINK_MHMODE_COM2);
      break;
   case IOLINK_BAUDRATE_COM3: // mdh_T4
      DL_Mode_ind_baud (port, IOLINK_MHMODE_COM3);
      break;
   default:
      LOG_ERROR (
         IOLINK_DL_LOG,
         "DL (%u): %s: Baudrate not valid: %d\n",
         iolink_get_portnumber (port),
         __func__,
         dl->baudrate);
      DL_Mode_ind_baud (port, IOLINK_MHMODE_INACTIVE);
      dl->mode_handler.state    = IOL_DL_MDH_ST_IDLE_0;
      dl->message_handler.state = IOL_DL_MH_ST_INACTIVE_0;
      CH_Conf (port, IOL_CHCMD_INACTIVE);
      break;
   }

   iolink_pl_get_error (port, &dl->cqerr, &dl->devdly);
   os_event_set (dl->event, IOLINK_DL_EVENT_MH);
}

static void iolink_dl_handle_error (iolink_port_t * port)
{
#if IOLINK_HW == IOLINK_HW_MAX14819
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   iolink_pl_get_error (port, &dl->cqerr, &dl->devdly);

   if ((dl->devdly & BIT (7)) != 0) // DelayErr
   {
      dl->rxtimeout = true;
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: Reception timeout: %x\n",
         iolink_get_portnumber (port),
         __func__,
         dl->devdly);
   }
   if ((dl->cqerr & (BIT (3) | BIT (1) | BIT (0))) != 0) // RChksmEr/FrameErr/ParityErr
   {
      dl->rxerror = true;
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: Reception error: %x\n",
         iolink_get_portnumber (port),
         __func__,
         dl->cqerr);
   }
   if ((dl->cqerr & (BIT (7) | BIT (6) | BIT (5) | BIT (4))) != 0) // TransmErrA/TCyclErrA/TChksmErA/TSizeErrA
   {
      dl->txerror = true;
      LOG_WARNING (
         IOLINK_DL_LOG,
         "DL (%u): %s: Transmission error: %x\n",
         iolink_get_portnumber (port),
         __func__,
         dl->cqerr);
   }
   if (dl->rxerror || dl->rxtimeout || dl->txerror)
   {
      iolink_dl_message_h_sm (port);
      dl->rxtimeout = false;
      dl->rxerror = false;
      dl->txerror = false;
   }
#endif
}

static void iolink_dl_handle_timer_timeout (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   switch (dl->timer_type)
   {
   case IOL_DL_TIMER_TINITCYC_MH:
      LOG_DEBUG (
         IOLINK_DL_LOG,
         "DL (%u): %s: TInitcyc timed out. IOL_DL_MH state: %s\n",
         iolink_get_portnumber (port),
         __func__,
         iolink_dl_mh_st_literals[dl->message_handler.state]);
      dl->timer_elapsed = true;
      iolink_dl_message_h_sm (port);
      dl->timer_elapsed = false;
      break;
   default:
      break;
   }

   dl->timer_type = IOL_DL_TIMER_NONE;
}

/*
 * Thread handling one DL and associated PL
 */
static void dl_main (void * arg)
{
   iolink_port_t * port = arg;
   iolink_dl_t * dl     = iolink_get_dl_ctx (port);

   uint32_t mask = IOLINK_PL_EVENT | IOLINK_PL_EVENT_RXRDY |
                   IOLINK_PL_EVENT_RXERR | IOLINK_PL_EVENT_TXERR |
                   IOLINK_PL_EVENT_WURQ | IOLINK_DL_EVENT_MDH |
                   IOLINK_DL_EVENT_MH | IOLINK_DL_EVENT_TIMEOUT |
                   IOLINK_DL_EVENT_TIMEOUT_TCYC | IOLINK_DL_EVENT_RESET;
   uint32_t event_timeout = (uint32_t)-1;

   /* Main loop */
   while (true)
   {
      if (!os_event_wait (dl->event, mask, &dl->triggered_events, event_timeout))
      {
         os_event_clr (dl->event, dl->triggered_events);

         if (dl->triggered_events & IOLINK_PL_EVENT)
         {
            // Event handler for hardware interrupt
            iolink_pl_handler (port);
         }

         if (dl->triggered_events & IOLINK_PL_EVENT_RXRDY)
         {
            // Event handler for data ready
            if (iolink_pl_get_data (
                   port,
                   dl->rxbuffer,
                   dl->message_handler.od_len + dl->message_handler.pd_rxlen + 1))
            {
               dl->dataready = true;
               iolink_dl_message_h_sm (port);
               dl->dataready = false;
            }
         }

         if (dl->triggered_events & IOLINK_PL_EVENT_RXERR)
         {
            // Event handler for receive error
            iolink_dl_handle_error (port);
         }

         if (dl->triggered_events & IOLINK_PL_EVENT_TXERR)
         {
            // Event handler for transmit error
            iolink_dl_handle_error (port);
         }

         if (dl->triggered_events & IOLINK_PL_EVENT_WURQ)
         {
            iolink_dl_wurq_recv (port);
         }

         if (dl->triggered_events & IOLINK_PL_EVENT_STATUS)
         {
         }

         if (dl->triggered_events & IOLINK_DL_EVENT_MDH)
         {
            // Event to DL mode handler
            iolink_dl_mode_h_sm (port);
         }

         if (dl->triggered_events & IOLINK_DL_EVENT_MH)
         {
            // Event to message handler
            iolink_dl_message_h_sm (port);
         }

         if (dl->triggered_events & IOLINK_DL_EVENT_TIMEOUT)
         {
            // Timer timeout
            iolink_dl_handle_timer_timeout (port);
         }

         if (dl->triggered_events & IOLINK_DL_EVENT_RESET)
         {
            iolink_dl_handle_reset (port);
         }
      }
   }
}

void iolink_dl_reset (iolink_port_t * port)
{
   LOG_DEBUG (
      IOLINK_DL_LOG,
      "DL (%u): %s\n",
      iolink_get_portnumber (port),
      __func__);
   iolink_dl_t * dl = iolink_get_dl_ctx (port);
   os_event_set (dl->event, IOLINK_DL_EVENT_RESET);
}

static void iolink_dl_handle_reset (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   memset (&dl->mode_handler, 0, sizeof (mode_h_t));
   memset (&dl->message_handler, 0, sizeof (message_h_t));
   memset (&dl->pd_handler, 0, sizeof (pd_handler_t));
   memset (&dl->od_handler, 0, sizeof (od_handler_t));
   memset (&dl->cmd_handler, 0, sizeof (command_h_t));
   memset (&dl->isdu_handler, 0, sizeof (isdu_h_t));
   memset (&dl->event_handler, 0, sizeof (event_h_t));

   dl->pd_handler.pdout_buffer = &dl->txbuffer[2];
   dl->od_handler.odout_buffer = &dl->txbuffer[2];
   dl->baudrate                = IOLINK_BAUDRATE_NONE;
   dl->cycbyte                 = iolink_pl_get_cycletime (port);
   dl->mseq                    = IOLINK_MSEQTYPE_TYPE_0;
   dl->triggered_events        = 0;
   dl->timer_type              = IOL_DL_TIMER_NONE;
   dl->timer_elapsed           = false;
   dl->timer_tcyc_elapsed      = false;
   dl->dataready               = false;
   dl->rxerror                 = false;
   dl->txerror                 = false;
   dl->rxtimeout               = false;
   dl->devdly                  = 0;
   dl->cqerr                   = 0;
   dl->first_read_min_cycl     = true;

   if (dl->timer_tcyc != NULL)
   {
      os_timer_stop (dl->timer_tcyc);
   }

   if (dl->timer != NULL)
   {
      os_timer_stop (dl->timer);
   }
}

/*
 * Instantiates a specific port object (DL and PL)
 */
void iolink_dl_instantiate (
   iolink_port_t * port,
   unsigned int thread_prio,
   size_t thread_stack_size)
{
   iolink_dl_handle_reset (port);

   iolink_dl_t * dl = iolink_get_dl_ctx (port);
   dl->mtx          = os_mutex_create();
   dl->event        = os_event_create();
   dl->timer        = os_timer_create (1000, dl_timer_timeout, dl, true);
   dl->timer_tcyc   = os_timer_create (1000, dl_timer_tcyc_timeout, dl, true);

   uint8_t portnumber = iolink_get_portnumber (port);
   snprintf (
      dl_thread_names[portnumber - 1],
      IOLINK_DL_THREAD_NAME_LENGTH,
      "iolport%d",
      portnumber);
   dl->thread = os_thread_create (
      dl_thread_names[portnumber - 1],
      thread_prio,
      thread_stack_size,
      dl_main,
      port);

   iolink_configure_pl_event (port, dl->event, IOLINK_PL_EVENT);
}

bool iolink_dl_get_pd_valid_status (iolink_port_t * port)
{
   iolink_dl_t * dl = iolink_get_dl_ctx (port);

   return dl->pd_handler.pd_valid;
}
