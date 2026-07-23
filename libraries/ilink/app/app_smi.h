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

#ifndef APP_SMI_H
#define APP_SMI_H

#include "iolink.h"
#include "app_handler.h"

iolink_smi_errortypes_t app_smi_device_write (
   app_port_ctx_t * app_port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   const uint8_t * data);

iolink_smi_errortypes_t app_smi_device_read (
   app_port_ctx_t * app_port,
   uint16_t index,
   uint8_t subindex,
   uint8_t len,
   uint8_t * data,
   uint8_t * actual_len);

uint8_t app_smi_pdout (
   app_port_ctx_t * app_port,
   bool output_enable,
   uint8_t len,
   const uint8_t * data);

int8_t app_smi_pdin (app_port_ctx_t * app_port, bool * valid, uint8_t * pdin);

uint8_t app_smi_pdinout (app_port_ctx_t * app_port);

#endif // APP_SMI_H
