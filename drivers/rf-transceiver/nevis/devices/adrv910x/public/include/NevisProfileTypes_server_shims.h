/* Auto-generated file */

#ifndef _NEVISPROFILETYPES_SERVER_SHIMS_H_
#define _NEVISPROFILETYPES_SERVER_SHIMS_H_

#include "civetweb.h"
#include "rapidjson/document.h"

using namespace rapidjson;


#include "device_profile_algo_dp_t.h"
#include "device_profile_bundle_t.h"
#include "device_profile_channels_t.h"
#include "device_profile_init_cals_t.h"
#include "device_profile_logen_t.h"
#include "device_profile_loop_filter_t.h"
#include "device_profile_mcs_t.h"
#include "device_profile_nco_drv_t.h"
#include "device_profile_pfir_buffer_t.h"
#include "device_profile_pfir_common_t.h"
#include "device_profile_pfir_interp_t.h"
#include "device_profile_pfir_mag_t.h"
#include "device_profile_pfir_pulse_t.h"
#include "device_profile_pfir_wbnb_t.h"
#include "device_profile_phase_sync_nco_t.h"
#include "device_profile_phase_sync_t.h"
#include "device_profile_pll_common_t.h"
#include "device_profile_ps2_t.h"
#include "device_profile_rx_dp_t.h"
#include "device_profile_rx_gaincontrol_t.h"
#include "device_profile_rxnb_dem_t.h"
#include "device_profile_ssi_t.h"
#include "device_profile_top_t.h"
#include "device_profile_tx_dp_t.h"
#include "device_profile_version_t.h"

void route_functions_NevisProfileTypes(struct mg_context *ctx);


int read_rxAlgDpGainCompSlicer_t_struct(struct mg_connection *conn, Value &obj, rxAlgDpGainCompSlicer_t *data);
void write_rxAlgDpGainCompSlicer_t_struct(Value &obj, rxAlgDpGainCompSlicer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxAlgDpGainCompSlicer_t_struct(rxAlgDpGainCompSlicer_t * data);
int read_rxAlgDpQec_t_struct(struct mg_connection *conn, Value &obj, rxAlgDpQec_t *data);
void write_rxAlgDpQec_t_struct(Value &obj, rxAlgDpQec_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxAlgDpQec_t_struct(rxAlgDpQec_t * data);
int read_rxAlgDpQecObs_t_struct(struct mg_connection *conn, Value &obj, rxAlgDpQecObs_t *data);
void write_rxAlgDpQecObs_t_struct(Value &obj, rxAlgDpQecObs_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxAlgDpQecObs_t_struct(rxAlgDpQecObs_t * data);
int read_txAlgDpDpdActuator_t_struct(struct mg_connection *conn, Value &obj, txAlgDpDpdActuator_t *data);
void write_txAlgDpDpdActuator_t_struct(Value &obj, txAlgDpDpdActuator_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpDpdActuator_t_struct(txAlgDpDpdActuator_t * data);
int read_txAlgDpPnGen_t_struct(struct mg_connection *conn, Value &obj, txAlgDpPnGen_t *data);
void write_txAlgDpPnGen_t_struct(Value &obj, txAlgDpPnGen_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpPnGen_t_struct(txAlgDpPnGen_t * data);
int read_txAlgDpDataSrcNco12_t_struct(struct mg_connection *conn, Value &obj, txAlgDpDataSrcNco12_t *data);
void write_txAlgDpDataSrcNco12_t_struct(Value &obj, txAlgDpDataSrcNco12_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpDataSrcNco12_t_struct(txAlgDpDataSrcNco12_t * data);
int read_txAlgDpTxCorrNcoPfir_t_struct(struct mg_connection *conn, Value &obj, txAlgDpTxCorrNcoPfir_t *data);
void write_txAlgDpTxCorrNcoPfir_t_struct(Value &obj, txAlgDpTxCorrNcoPfir_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpTxCorrNcoPfir_t_struct(txAlgDpTxCorrNcoPfir_t * data);
int read_txAlgDpRssiCrCorrClgcSel_t_struct(struct mg_connection *conn, Value &obj, txAlgDpRssiCrCorrClgcSel_t *data);
void write_txAlgDpRssiCrCorrClgcSel_t_struct(Value &obj, txAlgDpRssiCrCorrClgcSel_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpRssiCrCorrClgcSel_t_struct(txAlgDpRssiCrCorrClgcSel_t * data);
int read_txAlgDpPathDelay_t_struct(struct mg_connection *conn, Value &obj, txAlgDpPathDelay_t *data);
void write_txAlgDpPathDelay_t_struct(Value &obj, txAlgDpPathDelay_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpPathDelay_t_struct(txAlgDpPathDelay_t * data);
int read_txAlgDpTxQecLolCorr_t_struct(struct mg_connection *conn, Value &obj, txAlgDpTxQecLolCorr_t *data);
void write_txAlgDpTxQecLolCorr_t_struct(Value &obj, txAlgDpTxQecLolCorr_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgDpTxQecLolCorr_t_struct(txAlgDpTxQecLolCorr_t * data);
int read_txAlgoConnect_t_struct(struct mg_connection *conn, Value &obj, txAlgoConnect_t *data);
void write_txAlgoConnect_t_struct(Value &obj, txAlgoConnect_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txAlgoConnect_t_struct(txAlgoConnect_t * data);
int read_rxDdc_t_struct(struct mg_connection *conn, Value &obj, rxDdc_t *data);
void write_rxDdc_t_struct(Value &obj, rxDdc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxDdc_t_struct(rxDdc_t * data);
int read_deviceProfileBundle_t_struct(struct mg_connection *conn, Value &obj, deviceProfileBundle_t *data);
void write_deviceProfileBundle_t_struct(Value &obj, deviceProfileBundle_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_deviceProfileBundle_t_struct(deviceProfileBundle_t * data);
int read_initCals_t_struct(struct mg_connection *conn, Value &obj, initCals_t *data);
void write_initCals_t_struct(Value &obj, initCals_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_initCals_t_struct(initCals_t * data);
int read_trackingCals_t_struct(struct mg_connection *conn, Value &obj, trackingCals_t *data);
void write_trackingCals_t_struct(Value &obj, trackingCals_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_trackingCals_t_struct(trackingCals_t * data);
int read_loConfig_t_struct(struct mg_connection *conn, Value &obj, loConfig_t *data);
void write_loConfig_t_struct(Value &obj, loConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_loConfig_t_struct(loConfig_t * data);
int read_LoopFilterResult_t_struct(struct mg_connection *conn, Value &obj, LoopFilterResult_t *data);
void write_LoopFilterResult_t_struct(Value &obj, LoopFilterResult_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_LoopFilterResult_t_struct(LoopFilterResult_t * data);
int read_ncoDpConfig_t_struct(struct mg_connection *conn, Value &obj, ncoDpConfig_t *data);
void write_ncoDpConfig_t_struct(Value &obj, ncoDpConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_ncoDpConfig_t_struct(ncoDpConfig_t * data);
int read_txSrcNcoPhaseOffsetDc_t_struct(struct mg_connection *conn, Value &obj, txSrcNcoPhaseOffsetDc_t *data);
void write_txSrcNcoPhaseOffsetDc_t_struct(Value &obj, txSrcNcoPhaseOffsetDc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txSrcNcoPhaseOffsetDc_t_struct(txSrcNcoPhaseOffsetDc_t * data);
int read_bbfNcoConfig_t_struct(struct mg_connection *conn, Value &obj, bbfNcoConfig_t *data);
void write_bbfNcoConfig_t_struct(Value &obj, bbfNcoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_bbfNcoConfig_t_struct(bbfNcoConfig_t * data);
int read_pfirBuffer_t_struct(struct mg_connection *conn, Value &obj, pfirBuffer_t *data);
void write_pfirBuffer_t_struct(Value &obj, pfirBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pfirBuffer_t_struct(pfirBuffer_t * data);
int read_pfirMag21Buffer_t_struct(struct mg_connection *conn, Value &obj, pfirMag21Buffer_t *data);
void write_pfirMag21Buffer_t_struct(Value &obj, pfirMag21Buffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pfirMag21Buffer_t_struct(pfirMag21Buffer_t * data);
int read_pfirMag13Buffer_t_struct(struct mg_connection *conn, Value &obj, pfirMag13Buffer_t *data);
void write_pfirMag13Buffer_t_struct(Value &obj, pfirMag13Buffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pfirMag13Buffer_t_struct(pfirMag13Buffer_t * data);
int read_pfirPulseBuffer_t_struct(struct mg_connection *conn, Value &obj, pfirPulseBuffer_t *data);
void write_pfirPulseBuffer_t_struct(Value &obj, pfirPulseBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pfirPulseBuffer_t_struct(pfirPulseBuffer_t * data);
int read_pfirWbNbBuffer_t_struct(struct mg_connection *conn, Value &obj, pfirWbNbBuffer_t *data);
void write_pfirWbNbBuffer_t_struct(Value &obj, pfirWbNbBuffer_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pfirWbNbBuffer_t_struct(pfirWbNbBuffer_t * data);
int read_phaseSyncNcoConfig_t_struct(struct mg_connection *conn, Value &obj, phaseSyncNcoConfig_t *data);
void write_phaseSyncNcoConfig_t_struct(Value &obj, phaseSyncNcoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_phaseSyncNcoConfig_t_struct(phaseSyncNcoConfig_t * data);
int read_pllModulus_t_struct(struct mg_connection *conn, Value &obj, pllModulus_t *data);
void write_pllModulus_t_struct(Value &obj, pllModulus_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_pllModulus_t_struct(pllModulus_t * data);
int read_deviceProfilePS2_t_struct(struct mg_connection *conn, Value &obj, deviceProfilePS2_t *data);
void write_deviceProfilePS2_t_struct(Value &obj, deviceProfilePS2_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_deviceProfilePS2_t_struct(deviceProfilePS2_t * data);
int read_rxNbDecTop_t_struct(struct mg_connection *conn, Value &obj, rxNbDecTop_t *data);
void write_rxNbDecTop_t_struct(Value &obj, rxNbDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbDecTop_t_struct(rxNbDecTop_t * data);
int read_rxNbDecTopNb_t_struct(struct mg_connection *conn, Value &obj, rxNbDecTopNb_t *data);
void write_rxNbDecTopNb_t_struct(Value &obj, rxNbDecTopNb_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbDecTopNb_t_struct(rxNbDecTopNb_t * data);
int read_rxWbDecTop_t_struct(struct mg_connection *conn, Value &obj, rxWbDecTop_t *data);
void write_rxWbDecTop_t_struct(Value &obj, rxWbDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxWbDecTop_t_struct(rxWbDecTop_t * data);
int read_rxDecTop_t_struct(struct mg_connection *conn, Value &obj, rxDecTop_t *data);
void write_rxDecTop_t_struct(Value &obj, rxDecTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxDecTop_t_struct(rxDecTop_t * data);
int read_rxSincHbTop_t_struct(struct mg_connection *conn, Value &obj, rxSincHbTop_t *data);
void write_rxSincHbTop_t_struct(Value &obj, rxSincHbTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxSincHbTop_t_struct(rxSincHbTop_t * data);
int read_rxNbSincHbTop_t_struct(struct mg_connection *conn, Value &obj, rxNbSincHbTop_t *data);
void write_rxNbSincHbTop_t_struct(Value &obj, rxNbSincHbTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbSincHbTop_t_struct(rxNbSincHbTop_t * data);
int read_rxDpProfile_t_struct(struct mg_connection *conn, Value &obj, rxDpProfile_t *data);
void write_rxDpProfile_t_struct(Value &obj, rxDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxDpProfile_t_struct(rxDpProfile_t * data);
int read_rxNbDpProfile_t_struct(struct mg_connection *conn, Value &obj, rxNbDpProfile_t *data);
void write_rxNbDpProfile_t_struct(Value &obj, rxNbDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbDpProfile_t_struct(rxNbDpProfile_t * data);
int read_rxNbAgcHighThresholdSettings_t_struct(struct mg_connection *conn, Value &obj, rxNbAgcHighThresholdSettings_t *data);
void write_rxNbAgcHighThresholdSettings_t_struct(Value &obj, rxNbAgcHighThresholdSettings_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbAgcHighThresholdSettings_t_struct(rxNbAgcHighThresholdSettings_t * data);
int read_rx_gainControl_t_struct(struct mg_connection *conn, Value &obj, rx_gainControl_t *data);
void write_rx_gainControl_t_struct(Value &obj, rx_gainControl_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rx_gainControl_t_struct(rx_gainControl_t * data);
int read_dpInFifoConfig_t_struct(struct mg_connection *conn, Value &obj, dpInFifoConfig_t *data);
void write_dpInFifoConfig_t_struct(Value &obj, dpInFifoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_dpInFifoConfig_t_struct(dpInFifoConfig_t * data);
int read_rxNbNcoConfig_t_struct(struct mg_connection *conn, Value &obj, rxNbNcoConfig_t *data);
void write_rxNbNcoConfig_t_struct(Value &obj, rxNbNcoConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbNcoConfig_t_struct(rxNbNcoConfig_t * data);
int read_rxWbNbCompPFir_t_struct(struct mg_connection *conn, Value &obj, rxWbNbCompPFir_t *data);
void write_rxWbNbCompPFir_t_struct(Value &obj, rxWbNbCompPFir_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxWbNbCompPFir_t_struct(rxWbNbCompPFir_t * data);
int read_rxReSampConfig_t_struct(struct mg_connection *conn, Value &obj, rxReSampConfig_t *data);
void write_rxReSampConfig_t_struct(Value &obj, rxReSampConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxReSampConfig_t_struct(rxReSampConfig_t * data);
int read_rxnbDemConfig_t_struct(struct mg_connection *conn, Value &obj, rxnbDemConfig_t *data);
void write_rxnbDemConfig_t_struct(Value &obj, rxnbDemConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxnbDemConfig_t_struct(rxnbDemConfig_t * data);
int read_rxnbDemConfigNb_t_struct(struct mg_connection *conn, Value &obj, rxnbDemConfigNb_t *data);
void write_rxnbDemConfigNb_t_struct(Value &obj, rxnbDemConfigNb_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxnbDemConfigNb_t_struct(rxnbDemConfigNb_t * data);
int read_ssiConfig_t_struct(struct mg_connection *conn, Value &obj, ssiConfig_t *data);
void write_ssiConfig_t_struct(Value &obj, ssiConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_ssiConfig_t_struct(ssiConfig_t * data);
int read_enableDelay_t_struct(struct mg_connection *conn, Value &obj, enableDelay_t *data);
void write_enableDelay_t_struct(Value &obj, enableDelay_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_enableDelay_t_struct(enableDelay_t * data);
int read_deviceSysConfig_t_struct(struct mg_connection *conn, Value &obj, deviceSysConfig_t *data);
void write_deviceSysConfig_t_struct(Value &obj, deviceSysConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_deviceSysConfig_t_struct(deviceSysConfig_t * data);
int read_rxConfig_t_struct(struct mg_connection *conn, Value &obj, rxConfig_t *data);
void write_rxConfig_t_struct(Value &obj, rxConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxConfig_t_struct(rxConfig_t * data);
int read_rx1Config_t_struct(struct mg_connection *conn, Value &obj, rx1Config_t *data);
void write_rx1Config_t_struct(Value &obj, rx1Config_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rx1Config_t_struct(rx1Config_t * data);
int read_rxNbConfig_t_struct(struct mg_connection *conn, Value &obj, rxNbConfig_t *data);
void write_rxNbConfig_t_struct(Value &obj, rxNbConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_rxNbConfig_t_struct(rxNbConfig_t * data);
int read_txPredistConfig_t_struct(struct mg_connection *conn, Value &obj, txPredistConfig_t *data);
void write_txPredistConfig_t_struct(Value &obj, txPredistConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txPredistConfig_t_struct(txPredistConfig_t * data);
int read_txElbConfig_t_struct(struct mg_connection *conn, Value &obj, txElbConfig_t *data);
void write_txElbConfig_t_struct(Value &obj, txElbConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txElbConfig_t_struct(txElbConfig_t * data);
int read_txBbfConfig_t_struct(struct mg_connection *conn, Value &obj, txBbfConfig_t *data);
void write_txBbfConfig_t_struct(Value &obj, txBbfConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txBbfConfig_t_struct(txBbfConfig_t * data);
int read_txConfig_t_struct(struct mg_connection *conn, Value &obj, txConfig_t *data);
void write_txConfig_t_struct(Value &obj, txConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txConfig_t_struct(txConfig_t * data);
int read_tx1Config_t_struct(struct mg_connection *conn, Value &obj, tx1Config_t *data);
void write_tx1Config_t_struct(Value &obj, tx1Config_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_tx1Config_t_struct(tx1Config_t * data);
int read_txNbConfig_t_struct(struct mg_connection *conn, Value &obj, txNbConfig_t *data);
void write_txNbConfig_t_struct(Value &obj, txNbConfig_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txNbConfig_t_struct(txNbConfig_t * data);
int read_deviceProfile_t_struct(struct mg_connection *conn, Value &obj, deviceProfile_t *data);
void write_deviceProfile_t_struct(Value &obj, deviceProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_deviceProfile_t_struct(deviceProfile_t * data);
int read_txPreProc_t_struct(struct mg_connection *conn, Value &obj, txPreProc_t *data);
void write_txPreProc_t_struct(Value &obj, txPreProc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txPreProc_t_struct(txPreProc_t * data);
int read_txNbIntTop_t_struct(struct mg_connection *conn, Value &obj, txNbIntTop_t *data);
void write_txNbIntTop_t_struct(Value &obj, txNbIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txNbIntTop_t_struct(txNbIntTop_t * data);
int read_txNbIntTopNew_t_struct(struct mg_connection *conn, Value &obj, txNbIntTopNew_t *data);
void write_txNbIntTopNew_t_struct(Value &obj, txNbIntTopNew_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txNbIntTopNew_t_struct(txNbIntTopNew_t * data);
int read_txWbIntTop_t_struct(struct mg_connection *conn, Value &obj, txWbIntTop_t *data);
void write_txWbIntTop_t_struct(Value &obj, txWbIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txWbIntTop_t_struct(txWbIntTop_t * data);
int read_txIntTop_t_struct(struct mg_connection *conn, Value &obj, txIntTop_t *data);
void write_txIntTop_t_struct(Value &obj, txIntTop_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txIntTop_t_struct(txIntTop_t * data);
int read_txIntTopFreqDevMap_t_struct(struct mg_connection *conn, Value &obj, txIntTopFreqDevMap_t *data);
void write_txIntTopFreqDevMap_t_struct(Value &obj, txIntTopFreqDevMap_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txIntTopFreqDevMap_t_struct(txIntTopFreqDevMap_t * data);
int read_txIqdmDuc_t_struct(struct mg_connection *conn, Value &obj, txIqdmDuc_t *data);
void write_txIqdmDuc_t_struct(Value &obj, txIqdmDuc_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txIqdmDuc_t_struct(txIqdmDuc_t * data);
int read_txDpProfile_t_struct(struct mg_connection *conn, Value &obj, txDpProfile_t *data);
void write_txDpProfile_t_struct(Value &obj, txDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txDpProfile_t_struct(txDpProfile_t * data);
int read_txNbDpProfile_t_struct(struct mg_connection *conn, Value &obj, txNbDpProfile_t *data);
void write_txNbDpProfile_t_struct(Value &obj, txNbDpProfile_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txNbDpProfile_t_struct(txNbDpProfile_t * data);
int read_txIntToneGenCfg_t_struct(struct mg_connection *conn, Value &obj, txIntToneGenCfg_t *data);
void write_txIntToneGenCfg_t_struct(Value &obj, txIntToneGenCfg_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_txIntToneGenCfg_t_struct(txIntToneGenCfg_t * data);
int read_profileVersion_t_struct(struct mg_connection *conn, Value &obj, profileVersion_t *data);
void write_profileVersion_t_struct(Value &obj, profileVersion_t *data, MemoryPoolAllocator<> &jsonAllocator);
void free_profileVersion_t_struct(profileVersion_t * data);

#endif /* _NEVISPROFILETYPES_SERVER_SHIMS_H_ */