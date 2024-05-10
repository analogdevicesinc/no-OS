#ifndef _PALAU_STRUCT_ENDIAN_H_
#define _PALAU_STRUCT_ENDIAN_H_

#include "adrv904x_platform_byte_order.h"
#include "adrv904x_cpu_cmd_run_serdes_eye_sweep.h"

#if ADRV904X_LITTLE_ENDIAN == 1
#define CTOH_STRUCT(struct_name, struct_type)
#else
#define CTOH_STRUCT(struct_name, struct_type) struct_type##_swap(&(struct_name))
#endif 

#define HTOC_STRUCT CTOH_STRUCT

void adrv904x_SerdesInitCalStatusCmdResp_swap(struct adrv904x_SerdesInitCalStatusCmdResp* _struct);
void adrv904x_SerdesTrackingCalStatusCmdResp_swap(struct adrv904x_SerdesTrackingCalStatusCmdResp* _struct);
void adrv904x_SerdesTrackingCalStatus_swap(struct adi_adrv904x_SerdesTrackingCalStatus* _struct);
void adrv904x_SerdesInitCalStatus_swap(struct adi_adrv904x_SerdesInitCalStatus* _struct);
void adrv904x_CpuCmd_SerdesCalStatusGet_swap(struct adrv904x_CpuCmd_SerdesCalStatusGet* _struct);
#endif