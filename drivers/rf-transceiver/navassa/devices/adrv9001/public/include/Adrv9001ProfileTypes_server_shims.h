/* Auto-generated file */

#ifndef _ADRV9001PROFILETYPES_SERVER_SHIMS_H_
#define _ADRV9001PROFILETYPES_SERVER_SHIMS_H_

#include "civetweb.h"
#include "rapidjson/document.h"

using namespace rapidjson;


#include "adi_adrv9001_cals_types.h"
#include "adi_adrv9001_clockSettings_types.h"
#include "adi_adrv9001_deviceSysConfig_types.h"
#include "adi_adrv9001_dynamicProfile_types.h"
#include "adi_adrv9001_pfirBuffer_types.h"
#include "adi_adrv9001_profile_types.h"
#include "adi_adrv9001_rxSettings_types.h"
#include "adi_adrv9001_txSettings_types.h"

void route_functions_Adrv9001ProfileTypes(struct mg_context *ctx);


int read_adi_adrv9001_InitCals_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_InitCals_t *data);
void write_adi_adrv9001_InitCals_t_struct(Value &obj, adi_adrv9001_InitCals_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_InitCals_t_struct(adi_adrv9001_InitCals_t * data);
int read_adi_adrv9001_TrackingCals_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TrackingCals_t *data);
void write_adi_adrv9001_TrackingCals_t_struct(Value &obj, adi_adrv9001_TrackingCals_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TrackingCals_t_struct(adi_adrv9001_TrackingCals_t * data);
int read_adi_adrv9001_ClockSettings_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_ClockSettings_t *data);
void write_adi_adrv9001_ClockSettings_t_struct(Value &obj, adi_adrv9001_ClockSettings_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_ClockSettings_t_struct(adi_adrv9001_ClockSettings_t * data);
int read_adi_adrv9001_pllModulus_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_pllModulus_t *data);
void write_adi_adrv9001_pllModulus_t_struct(Value &obj, adi_adrv9001_pllModulus_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_pllModulus_t_struct(adi_adrv9001_pllModulus_t * data);
int read_adi_adrv9001_DeviceSysConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_DeviceSysConfig_t *data);
void write_adi_adrv9001_DeviceSysConfig_t_struct(Value &obj, adi_adrv9001_DeviceSysConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_DeviceSysConfig_t_struct(adi_adrv9001_DeviceSysConfig_t * data);
int read_adi_adrv9000_RxDynamicProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9000_RxDynamicProfile_t *data);
void write_adi_adrv9000_RxDynamicProfile_t_struct(Value &obj, adi_adrv9000_RxDynamicProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9000_RxDynamicProfile_t_struct(adi_adrv9000_RxDynamicProfile_t * data);
int read_adi_adrv9000_TxDynamicProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9000_TxDynamicProfile_t *data);
void write_adi_adrv9000_TxDynamicProfile_t_struct(Value &obj, adi_adrv9000_TxDynamicProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9000_TxDynamicProfile_t_struct(adi_adrv9000_TxDynamicProfile_t * data);
int read_adi_adrv9000_DynamicProfileChannel_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9000_DynamicProfileChannel_t *data);
void write_adi_adrv9000_DynamicProfileChannel_t_struct(Value &obj, adi_adrv9000_DynamicProfileChannel_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9000_DynamicProfileChannel_t_struct(adi_adrv9000_DynamicProfileChannel_t * data);
int read_adi_adrv9000_DynamicProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9000_DynamicProfile_t *data);
void write_adi_adrv9000_DynamicProfile_t_struct(Value &obj, adi_adrv9000_DynamicProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9000_DynamicProfile_t_struct(adi_adrv9000_DynamicProfile_t * data);
int read_adi_adrv9001_PfirWbNbBuffer_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirWbNbBuffer_t *data);
void write_adi_adrv9001_PfirWbNbBuffer_t_struct(Value &obj, adi_adrv9001_PfirWbNbBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirWbNbBuffer_t_struct(adi_adrv9001_PfirWbNbBuffer_t * data);
int read_adi_adrv9001_PfirPulseBuffer_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirPulseBuffer_t *data);
void write_adi_adrv9001_PfirPulseBuffer_t_struct(Value &obj, adi_adrv9001_PfirPulseBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirPulseBuffer_t_struct(adi_adrv9001_PfirPulseBuffer_t * data);
int read_adi_adrv9001_PfirMag21Buffer_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirMag21Buffer_t *data);
void write_adi_adrv9001_PfirMag21Buffer_t_struct(Value &obj, adi_adrv9001_PfirMag21Buffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirMag21Buffer_t_struct(adi_adrv9001_PfirMag21Buffer_t * data);
int read_adi_adrv9001_PfirMag13Buffer_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirMag13Buffer_t *data);
void write_adi_adrv9001_PfirMag13Buffer_t_struct(Value &obj, adi_adrv9001_PfirMag13Buffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirMag13Buffer_t_struct(adi_adrv9001_PfirMag13Buffer_t * data);
int read_adi_adrv9001_PfirBuffer_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirBuffer_t *data);
void write_adi_adrv9001_PfirBuffer_t_struct(Value &obj, adi_adrv9001_PfirBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirBuffer_t_struct(adi_adrv9001_PfirBuffer_t * data);
int read_adi_adrv9001_PfirCoeff_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirCoeff_t *data);
void write_adi_adrv9001_PfirCoeff_t_struct(Value &obj, adi_adrv9001_PfirCoeff_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirCoeff_t_struct(adi_adrv9001_PfirCoeff_t * data);
int read_adi_adrv9001_PfirWbNbConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_PfirWbNbConfig_t *data);
void write_adi_adrv9001_PfirWbNbConfig_t_struct(Value &obj, adi_adrv9001_PfirWbNbConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_PfirWbNbConfig_t_struct(adi_adrv9001_PfirWbNbConfig_t * data);
int read_adi_adrv9001_Init_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_Init_t *data);
void write_adi_adrv9001_Init_t_struct(Value &obj, adi_adrv9001_Init_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_Init_t_struct(adi_adrv9001_Init_t * data);
int read_adi_adrv9001_RxNbDecTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxNbDecTop_t *data);
void write_adi_adrv9001_RxNbDecTop_t_struct(Value &obj, adi_adrv9001_RxNbDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxNbDecTop_t_struct(adi_adrv9001_RxNbDecTop_t * data);
int read_adi_adrv9001_RxWbDecTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxWbDecTop_t *data);
void write_adi_adrv9001_RxWbDecTop_t_struct(Value &obj, adi_adrv9001_RxWbDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxWbDecTop_t_struct(adi_adrv9001_RxWbDecTop_t * data);
int read_adi_adrv9001_RxDecTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxDecTop_t *data);
void write_adi_adrv9001_RxDecTop_t_struct(Value &obj, adi_adrv9001_RxDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxDecTop_t_struct(adi_adrv9001_RxDecTop_t * data);
int read_adi_adrv9001_RxSincHbTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxSincHbTop_t *data);
void write_adi_adrv9001_RxSincHbTop_t_struct(Value &obj, adi_adrv9001_RxSincHbTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxSincHbTop_t_struct(adi_adrv9001_RxSincHbTop_t * data);
int read_adi_adrv9001_RxDpInFifoConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxDpInFifoConfig_t *data);
void write_adi_adrv9001_RxDpInFifoConfig_t_struct(Value &obj, adi_adrv9001_RxDpInFifoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxDpInFifoConfig_t_struct(adi_adrv9001_RxDpInFifoConfig_t * data);
int read_adi_adrv9001_NcoDpConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_NcoDpConfig_t *data);
void write_adi_adrv9001_NcoDpConfig_t_struct(Value &obj, adi_adrv9001_NcoDpConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_NcoDpConfig_t_struct(adi_adrv9001_NcoDpConfig_t * data);
int read_adi_adrv9001_RxNbNcoConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxNbNcoConfig_t *data);
void write_adi_adrv9001_RxNbNcoConfig_t_struct(Value &obj, adi_adrv9001_RxNbNcoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxNbNcoConfig_t_struct(adi_adrv9001_RxNbNcoConfig_t * data);
int read_adi_adrv9001_RxWbNbCompPFir_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxWbNbCompPFir_t *data);
void write_adi_adrv9001_RxWbNbCompPFir_t_struct(Value &obj, adi_adrv9001_RxWbNbCompPFir_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxWbNbCompPFir_t_struct(adi_adrv9001_RxWbNbCompPFir_t * data);
int read_adi_adrv9001_RxResampConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxResampConfig_t *data);
void write_adi_adrv9001_RxResampConfig_t_struct(Value &obj, adi_adrv9001_RxResampConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxResampConfig_t_struct(adi_adrv9001_RxResampConfig_t * data);
int read_adi_adrv9001_RxNbDemConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxNbDemConfig_t *data);
void write_adi_adrv9001_RxNbDemConfig_t_struct(Value &obj, adi_adrv9001_RxNbDemConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxNbDemConfig_t_struct(adi_adrv9001_RxNbDemConfig_t * data);
int read_adi_adrv9001_RxDpProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxDpProfile_t *data);
void write_adi_adrv9001_RxDpProfile_t_struct(Value &obj, adi_adrv9001_RxDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxDpProfile_t_struct(adi_adrv9001_RxDpProfile_t * data);
int read_adi_adrv9001_RxLnaConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxLnaConfig_t *data);
void write_adi_adrv9001_RxLnaConfig_t_struct(Value &obj, adi_adrv9001_RxLnaConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxLnaConfig_t_struct(adi_adrv9001_RxLnaConfig_t * data);
int read_adi_adrv9001_SsiConfig_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_SsiConfig_t *data);
void write_adi_adrv9001_SsiConfig_t_struct(Value &obj, adi_adrv9001_SsiConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_SsiConfig_t_struct(adi_adrv9001_SsiConfig_t * data);
int read_adi_adrv9001_RxProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxProfile_t *data);
void write_adi_adrv9001_RxProfile_t_struct(Value &obj, adi_adrv9001_RxProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxProfile_t_struct(adi_adrv9001_RxProfile_t * data);
int read_adi_adrv9001_RxChannelCfg_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxChannelCfg_t *data);
void write_adi_adrv9001_RxChannelCfg_t_struct(Value &obj, adi_adrv9001_RxChannelCfg_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxChannelCfg_t_struct(adi_adrv9001_RxChannelCfg_t * data);
int read_adi_adrv9001_RxSettings_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_RxSettings_t *data);
void write_adi_adrv9001_RxSettings_t_struct(Value &obj, adi_adrv9001_RxSettings_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_RxSettings_t_struct(adi_adrv9001_RxSettings_t * data);
int read_adi_adrv9001_TxPreProc_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxPreProc_t *data);
void write_adi_adrv9001_TxPreProc_t_struct(Value &obj, adi_adrv9001_TxPreProc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxPreProc_t_struct(adi_adrv9001_TxPreProc_t * data);
int read_adi_adrv9001_TxNbIntTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxNbIntTop_t *data);
void write_adi_adrv9001_TxNbIntTop_t_struct(Value &obj, adi_adrv9001_TxNbIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxNbIntTop_t_struct(adi_adrv9001_TxNbIntTop_t * data);
int read_adi_adrv9001_TxWbIntTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxWbIntTop_t *data);
void write_adi_adrv9001_TxWbIntTop_t_struct(Value &obj, adi_adrv9001_TxWbIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxWbIntTop_t_struct(adi_adrv9001_TxWbIntTop_t * data);
int read_adi_adrv9001_TxIntTop_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxIntTop_t *data);
void write_adi_adrv9001_TxIntTop_t_struct(Value &obj, adi_adrv9001_TxIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxIntTop_t_struct(adi_adrv9001_TxIntTop_t * data);
int read_adi_adrv9001_TxIntTopFreqDevMap_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxIntTopFreqDevMap_t *data);
void write_adi_adrv9001_TxIntTopFreqDevMap_t_struct(Value &obj, adi_adrv9001_TxIntTopFreqDevMap_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxIntTopFreqDevMap_t_struct(adi_adrv9001_TxIntTopFreqDevMap_t * data);
int read_adi_adrv9001_TxIqdmDuc_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxIqdmDuc_t *data);
void write_adi_adrv9001_TxIqdmDuc_t_struct(Value &obj, adi_adrv9001_TxIqdmDuc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxIqdmDuc_t_struct(adi_adrv9001_TxIqdmDuc_t * data);
int read_adi_adrv9001_TxDpProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxDpProfile_t *data);
void write_adi_adrv9001_TxDpProfile_t_struct(Value &obj, adi_adrv9001_TxDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxDpProfile_t_struct(adi_adrv9001_TxDpProfile_t * data);
int read_adi_adrv9001_TxProfile_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxProfile_t *data);
void write_adi_adrv9001_TxProfile_t_struct(Value &obj, adi_adrv9001_TxProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxProfile_t_struct(adi_adrv9001_TxProfile_t * data);
int read_adi_adrv9001_TxSettings_t_struct(struct mg_connection *conn, Value &obj, adi_adrv9001_TxSettings_t *data);
void write_adi_adrv9001_TxSettings_t_struct(Value &obj, adi_adrv9001_TxSettings_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_adi_adrv9001_TxSettings_t_struct(adi_adrv9001_TxSettings_t * data);

#endif /* _ADRV9001PROFILETYPES_SERVER_SHIMS_H_ */