/*
 * Copyright(c) 2021-2022 Analog Devices, Inc.All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc. and its licensors.
 */

#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef _MSC_VER
#define _ALLOW_KEYWORD_MACROS
#define nullptr __nullptr
#include "nlohmann/json.hpp"
#undef nullptr
#undef _ALLOW_KEYWORD_MACROS
#else
#include "nlohmann/json.hpp"
#endif

#include "fifo_map.hpp"
#include "device_profile_bundle_t.h"
#include "device_profile_init_cals_t.h"
#include "device_profile_ps2_t.h"

template<class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
using ordered_json = nlohmann::basic_json<my_workaround_fifo_map>;
void to_json(ordered_json& j, const deviceProfileBundle_t& p);
void from_json(const ordered_json& j, deviceProfileBundle_t& p);

void to_json(ordered_json& j, const deviceProfile_t& p);
void from_json(const ordered_json& j, deviceProfile_t& p);

void to_json(ordered_json& j, const profileVersion_t& p);
void from_json(const ordered_json& j, profileVersion_t& p);

void to_json(ordered_json& j, const loConfig_t& p);
void from_json(const ordered_json& j, loConfig_t& p);

void to_json(ordered_json& j, const txConfig_t& p);
void from_json(const ordered_json& j, txConfig_t& p);

void to_json(ordered_json& j, const txBbfConfig_t& p);
void from_json(const ordered_json& j, txBbfConfig_t& p);

void to_json(ordered_json& j, const txPredistConfig_t& p);
void from_json(const ordered_json& j, txPredistConfig_t& p);

void to_json(ordered_json& j, const txElbConfig_t& p);
void from_json(const ordered_json& j, txElbConfig_t& p);

void to_json(ordered_json& j, const ssiConfig_t& p);
void from_json(const ordered_json& j, ssiConfig_t& p);

void to_json(ordered_json& j, const enableDelay_t& p);
void from_json(const ordered_json& j, enableDelay_t& p);

void to_json(ordered_json& j, const rxConfig_t& p);
void from_json(const ordered_json& j, rxConfig_t& p);

void to_json(ordered_json& j, const tx1Config_t& p);
void from_json(const ordered_json& j, tx1Config_t& p);

void to_json(ordered_json& j, const txDpProfile_t& p);
void from_json(const ordered_json& j, txDpProfile_t& p);

void to_json(ordered_json& j, const txPreProc_t& p);
void from_json(const ordered_json& j, txPreProc_t& p);

void to_json(ordered_json& j, const txWbIntTop_t& p);
void from_json(const ordered_json& j, txWbIntTop_t& p);

void to_json(ordered_json& j, const txNbIntTop_t& p);
void from_json(const ordered_json& j, txNbIntTop_t& p);

void to_json(ordered_json& j, const txIntTop_t& p);
void from_json(const ordered_json& j, txIntTop_t& p);

void to_json(ordered_json& j, const txIntTopFreqDevMap_t& p);
void from_json(const ordered_json& j, txIntTopFreqDevMap_t& p);

void to_json(ordered_json& j, const txIqdmDuc_t& p);
void from_json(const ordered_json& j, txIqdmDuc_t& p);

void to_json(ordered_json& j, const ncoDpConfig_t& p);
void from_json(const ordered_json& j, ncoDpConfig_t& p);

void to_json(ordered_json& j, const rx1Config_t& p);
void from_json(const ordered_json& j, rx1Config_t& p);

void to_json(ordered_json& j, const rxDpProfile_t& p);
void from_json(const ordered_json& j, rxDpProfile_t& p);

void to_json(ordered_json& j, const rxNbDecTop_t& p);
void from_json(const ordered_json& j, rxNbDecTop_t& p);

void to_json(ordered_json& j, const rxWbDecTop_t& p);
void from_json(const ordered_json& j, rxWbDecTop_t& p);

void to_json(ordered_json& j, const rxDecTop_t& p);
void from_json(const ordered_json& j, rxDecTop_t& p);

void to_json(ordered_json& j, const rxSincHbTop_t& p);
void from_json(const ordered_json& j, rxSincHbTop_t& p);

void to_json(ordered_json& j, const rxnbDemConfig_t& p);
void from_json(const ordered_json& j, rxnbDemConfig_t& p);

void to_json(ordered_json& j, const rxNbNcoConfig_t& p);
void from_json(const ordered_json& j, rxNbNcoConfig_t& p);

void to_json(ordered_json& j, const rxWbNbCompPFir_t& p);
void from_json(const ordered_json& j, rxWbNbCompPFir_t& p);

void to_json(ordered_json& j, const rxReSampConfig_t& p);
void from_json(const ordered_json& j, rxReSampConfig_t& p);

void to_json(ordered_json& j, const txNbConfig_t& p);
void from_json(const ordered_json& j, txNbConfig_t& p);

void to_json(ordered_json& j, const txNbDpProfile_t& p);
void from_json(const ordered_json& j, txNbDpProfile_t& p);

void to_json(ordered_json& j, const txNbIntTopNew_t& p);
void from_json(const ordered_json& j, txNbIntTopNew_t& p);

void to_json(ordered_json& j, const rxNbConfig_t& p);
void from_json(const ordered_json& j, rxNbConfig_t& p);

void to_json(ordered_json& j, const rxNbDpProfile_t& p);
void from_json(const ordered_json& j, rxNbDpProfile_t& p);

void to_json(ordered_json& j, const rxNbDecTopNb_t& p);
void from_json(const ordered_json& j, rxNbDecTopNb_t& p);

void to_json(ordered_json& j, const rxNbSincHbTop_t& p);
void from_json(const ordered_json& j, rxNbSincHbTop_t& p);

void to_json(ordered_json& j, const rxnbDemConfigNb_t& p);
void from_json(const ordered_json& j, rxnbDemConfigNb_t& p);

void to_json(ordered_json& j, const dpInFifoConfig_t& p);
void from_json(const ordered_json& j, dpInFifoConfig_t& p);

void to_json(ordered_json& j, const rx_gainControl_t& p);
void from_json(const ordered_json& j, rx_gainControl_t& p);

void to_json(ordered_json& j, const rxNbAgcHighThresholdSettings_t& p);
void from_json(const ordered_json& j, rxNbAgcHighThresholdSettings_t& p);

void to_json(ordered_json& j, const deviceSysConfig_t& p);
void from_json(const ordered_json& j, deviceSysConfig_t& p);

void to_json(ordered_json& j, const pllModulus_t& p);
void from_json(const ordered_json& j, pllModulus_t& p);

void to_json(ordered_json& j, const pfirBuffer_t& p);
void from_json(const ordered_json& j, pfirBuffer_t& p);

void to_json(ordered_json& j, const pfirWbNbBuffer_t& p);
void from_json(const ordered_json& j, pfirWbNbBuffer_t& p);

void to_json(ordered_json& j, const pfirPulseBuffer_t& p);
void from_json(const ordered_json& j, pfirPulseBuffer_t& p);

void to_json(ordered_json& j, const pfirMag21Buffer_t& p);
void from_json(const ordered_json& j, pfirMag21Buffer_t& p);

void to_json(ordered_json& j, const pfirMag13Buffer_t& p);
void from_json(const ordered_json& j, pfirMag13Buffer_t& p);

void to_json(ordered_json& j, const trackingCals_t& p);
void from_json(const ordered_json& j, trackingCals_t& p);

void to_json(ordered_json& j, const initCals_t& p);
void from_json(const ordered_json& j, initCals_t& p);

void to_json(ordered_json& j, const deviceProfilePS2_t& p);
void from_json(const ordered_json& j, deviceProfilePS2_t& p);

