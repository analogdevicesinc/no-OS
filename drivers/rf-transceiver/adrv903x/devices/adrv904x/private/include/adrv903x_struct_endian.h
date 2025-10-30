/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _ADRV903X_STRUCT_ENDIAN_H_
#define _ADRV903X_STRUCT_ENDIAN_H_

#include "adrv903x_platform_byte_order.h"
#include "adrv903x_cpu_cmd_run_serdes_eye_sweep.h"

#if ADRV903X_LITTLE_ENDIAN == 1
#define CTOH_STRUCT(struct_name, struct_type)
#else
#define CTOH_STRUCT(struct_name, struct_type) struct_type##_swap(&(struct_name))
#endif 

#define HTOC_STRUCT CTOH_STRUCT

void adrv903x_SerdesInitCalStatusCmdResp_swap(struct adrv903x_SerdesInitCalStatusCmdResp* _struct);
void adrv903x_SerdesTrackingCalStatusCmdResp_swap(struct adrv903x_SerdesTrackingCalStatusCmdResp* _struct);
void adrv903x_SerdesTrackingCalStatus_swap(struct adi_adrv903x_SerdesTrackingCalStatus* _struct);
void adrv903x_SerdesInitCalStatus_swap(struct adi_adrv903x_SerdesInitCalStatus* _struct);
void adrv903x_CpuCmd_SerdesCalStatusGet_swap(struct adrv903x_CpuCmd_SerdesCalStatusGet* _struct);
#endif