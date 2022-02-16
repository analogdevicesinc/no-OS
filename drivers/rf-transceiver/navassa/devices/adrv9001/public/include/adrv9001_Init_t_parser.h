/* Auto-generated file - DO NOT MANUALLY EDIT */
/* Filename: C:\Jenkins\workspace\vice-driver_profile-types_master\include\adrv9001_Init_t_parser.h */
/* Created on: 1/26/2022 1:23:20 PM */

/**
 * Contains auto-generated C macros for loading fields from a JSON file
 * into an instance of adrv9001_Init_t struct
 * working in tandem with the common PMAG macros in adi_pmag_macros.h
 */

/** 
 * Copyright 2022 Analog Devices Inc.
 */

#include "adi_pmag_macros.h"

/* ---- ADRV9001_PFIRMAG13BUFFER_T ---- */
#define ADRV9001_PFIRMAG13BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance.numCoeff,                     "numCoeff"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance.coefficients,                 "coefficients"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PFIRMAG13BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance, adrv9001_PfirMag13Buffer_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PFIRMAG13BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG13BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance, adrv9001_PfirMag13Buffer_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PFIRMAG13BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_PfirMag13Buffer_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag13Buffer_tName, ARRAY_STRUCT_INDEXER(adrv9001_PfirMag13Buffer_t));



/* ---- ADRV9001_PFIRMAG21BUFFER_T ---- */
#define ADRV9001_PFIRMAG21BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance.numCoeff,                     "numCoeff"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance.coefficients,                 "coefficients"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PFIRMAG21BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance, adrv9001_PfirMag21Buffer_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PFIRMAG21BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance, adrv9001_PfirMag21Buffer_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PFIRMAG21BUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_PfirMag21Buffer_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirMag21Buffer_tName, ARRAY_STRUCT_INDEXER(adrv9001_PfirMag21Buffer_t));



/* ---- ADRV9001_PFIRPULSEBUFFER_T ---- */
#define ADRV9001_PFIRPULSEBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance.numCoeff,                     "numCoeff"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance.symmetricSel,                 "symmetricSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance.taps,                         "taps"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance.gainSel,                      "gainSel"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance.coefficients,                 "coefficients"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PFIRPULSEBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance, adrv9001_PfirPulseBuffer_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PFIRPULSEBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PFIRPULSEBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance, adrv9001_PfirPulseBuffer_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PFIRPULSEBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_PfirPulseBuffer_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirPulseBuffer_tName, ARRAY_STRUCT_INDEXER(adrv9001_PfirPulseBuffer_t));



/* ---- ADRV9001_PFIRWBNBBUFFER_T ---- */
#define ADRV9001_PFIRWBNBBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance.numCoeff,                      "numCoeff"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance.symmetricSel,                  "symmetricSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance.tapsSel,                       "tapsSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance.gainSel,                       "gainSel"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance.coefficients,                  "coefficients"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance, adrv9001_PfirWbNbBuffer_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PFIRWBNBBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PFIRWBNBBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance, adrv9001_PfirWbNbBuffer_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PFIRWBNBBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_PfirWbNbBuffer_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirWbNbBuffer_tName, ARRAY_STRUCT_INDEXER(adrv9001_PfirWbNbBuffer_t));



/* ---- ADRV9001_PFIRBUFFER_T ---- */
#define ADRV9001_PFIRBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxWbNbChFilterCoeff_A,         "pfirRxWbNbChFilterCoeff_A"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxWbNbChFilterCoeff_B,         "pfirRxWbNbChFilterCoeff_B"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxWbNbChFilterCoeff_C,         "pfirRxWbNbChFilterCoeff_C"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxWbNbChFilterCoeff_D,         "pfirRxWbNbChFilterCoeff_D"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxWbNbPulShpCoeff_A,           "pfirTxWbNbPulShpCoeff_A"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxWbNbPulShpCoeff_B,           "pfirTxWbNbPulShpCoeff_B"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxWbNbPulShpCoeff_C,           "pfirTxWbNbPulShpCoeff_C"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRWBNBBUFFER_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxWbNbPulShpCoeff_D,           "pfirTxWbNbPulShpCoeff_D"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRPULSEBUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxNbPulShp,                    "pfirRxNbPulShp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagLowTiaLowSRHp,            "pfirRxMagLowTiaLowSRHp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagLowTiaHighSRHp,           "pfirRxMagLowTiaHighSRHp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagHighTiaHighSRHp,          "pfirRxMagHighTiaHighSRHp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagLowTiaLowSRLp,            "pfirRxMagLowTiaLowSRLp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagLowTiaHighSRLp,           "pfirRxMagLowTiaHighSRLp"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagHighTiaHighSRLp,          "pfirRxMagHighTiaHighSRLp"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxMagComp1,                    "pfirTxMagComp1"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRMAG21BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxMagComp2,                    "pfirTxMagComp2"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG13BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirTxMagCompNb,                   "pfirTxMagCompNb"); \
ADI_PROCESS_ARRAY_ADRV9001_PFIRMAG13BUFFER_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance.pfirRxMagCompNb,                   "pfirRxMagCompNb"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PFIRBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance, adrv9001_PfirBuffer_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PFIRBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PFIRBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance, adrv9001_PfirBuffer_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PFIRBUFFER_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_PfirBuffer_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_PfirBuffer_tName, ARRAY_STRUCT_INDEXER(adrv9001_PfirBuffer_t));



/* ---- ADRV9001_PLLMODULUS_T ---- */
#define ADRV9001_PLLMODULUS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance) \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance.modulus,                           "modulus"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance.dmModulus,                         "dmModulus"); \

#define ADI_PROCESS_STRUCT_ADRV9001_PLLMODULUS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance, adrv9001_pllModulus_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_PLLMODULUS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_PLLMODULUS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance, adrv9001_pllModulus_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_PLLMODULUS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_pllModulus_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_pllModulus_tName, ARRAY_STRUCT_INDEXER(adrv9001_pllModulus_t));



/* ---- ADRV9001_DEVICESYSCONFIG_T ---- */
#define ADRV9001_DEVICESYSCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.duplexMode,                   "duplexMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.fhModeOn,                     "fhModeOn"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.numDynamicProfiles,           "numDynamicProfiles"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.mcsMode,                      "mcsMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.mcsInterfaceType,             "mcsInterfaceType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.adcTypeMonitor,               "adcTypeMonitor"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.pllLockTime_us,               "pllLockTime_us"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.pllPhaseSyncWait_us,          "pllPhaseSyncWait_us"); \
ADI_PROCESS_STRUCT_ADRV9001_PLLMODULUS_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.pllModulus,                   "pllModulus"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance.warmBootEnable,               "warmBootEnable"); \

#define ADI_PROCESS_STRUCT_ADRV9001_DEVICESYSCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance, adrv9001_DeviceSysConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_DEVICESYSCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_DEVICESYSCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance, adrv9001_DeviceSysConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_DEVICESYSCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_DeviceSysConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_DeviceSysConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_DeviceSysConfig_t));



/* ---- ADRV9001_TXIQDMDUC_T ---- */
#define ADRV9001_TXIQDMDUC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmDucMode,                        "iqdmDucMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmDev,                            "iqdmDev"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmDevOffset,                      "iqdmDevOffset"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmScalar,                         "iqdmScalar"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmThreshold,                      "iqdmThreshold"); \
ADI_PROCESS_STRUCT_ADRV9001_NCODPCONFIG_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance.iqdmNco,                            "iqdmNco"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXIQDMDUC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance, adrv9001_TxIqdmDuc_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXIQDMDUC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXIQDMDUC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance, adrv9001_TxIqdmDuc_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXIQDMDUC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxIqdmDuc_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIqdmDuc_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxIqdmDuc_t));



/* ---- ADRV9001_TXINTTOPFREQDEVMAP_T ---- */
#define ADRV9001_TXINTTOPFREQDEVMAP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.rrc2Frac,                  "rrc2Frac"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.mpll,                      "mpll"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.nchLsw,                    "nchLsw"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.nchMsb,                    "nchMsb"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.freqDevMapEn,              "freqDevMapEn"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance.txRoundEn,                 "txRoundEn"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXINTTOPFREQDEVMAP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance, adrv9001_TxIntTopFreqDevMap_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXINTTOPFREQDEVMAP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXINTTOPFREQDEVMAP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance, adrv9001_TxIntTopFreqDevMap_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXINTTOPFREQDEVMAP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxIntTopFreqDevMap_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTopFreqDevMap_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxIntTopFreqDevMap_t));



/* ---- ADRV9001_TXINTTOP_T ---- */
#define ADRV9001_TXINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.interpBy3Blk44p1En,                  "interpBy3Blk44p1En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.sinc3Blk44En,                        "sinc3Blk44En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.sinc2Blk42En,                        "sinc2Blk42En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.interpBy3Blk40En,                    "interpBy3Blk40En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.interpBy2Blk38En,                    "interpBy2Blk38En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance.interpBy2Blk36En,                    "interpBy2Blk36En"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance, adrv9001_TxIntTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance, adrv9001_TxIntTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxIntTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxIntTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxIntTop_t));



/* ---- ADRV9001_TXNBINTTOP_T ---- */
#define ADRV9001_TXNBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk20En,                "txInterpBy2Blk20En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk18En,                "txInterpBy2Blk18En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk16En,                "txInterpBy2Blk16En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk14En,                "txInterpBy2Blk14En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk12En,                "txInterpBy2Blk12En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy3Blk10En,                "txInterpBy3Blk10En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txInterpBy2Blk8En,                 "txInterpBy2Blk8En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txScicBlk32En,                     "txScicBlk32En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance.txScicBlk32DivFactor,              "txScicBlk32DivFactor"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXNBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance, adrv9001_TxNbIntTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXNBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXNBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance, adrv9001_TxNbIntTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXNBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxNbIntTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxNbIntTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxNbIntTop_t));



/* ---- ADRV9001_TXWBINTTOP_T ---- */
#define ADRV9001_TXWBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txInterpBy2Blk30En,                "txInterpBy2Blk30En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txInterpBy2Blk28En,                "txInterpBy2Blk28En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txInterpBy2Blk26En,                "txInterpBy2Blk26En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txInterpBy2Blk24En,                "txInterpBy2Blk24En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txInterpBy2Blk22En,                "txInterpBy2Blk22En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance.txWbLpfBlk22p1En,                  "txWbLpfBlk22p1En"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXWBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance, adrv9001_TxWbIntTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXWBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXWBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance, adrv9001_TxWbIntTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXWBINTTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxWbIntTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxWbIntTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxWbIntTop_t));



/* ---- ADRV9001_TXPREPROC_T ---- */
#define ADRV9001_TXPREPROC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcSymbol0,                   "txPreProcSymbol0"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcSymbol1,                   "txPreProcSymbol1"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcSymbol2,                   "txPreProcSymbol2"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcSymbol3,                   "txPreProcSymbol3"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcSymMapDivFactor,           "txPreProcSymMapDivFactor"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcMode,                      "txPreProcMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcWbNbPfirIBankSel,          "txPreProcWbNbPfirIBankSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance.txPreProcWbNbPfirQBankSel,          "txPreProcWbNbPfirQBankSel"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXPREPROC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance, adrv9001_TxPreProc_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXPREPROC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXPREPROC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance, adrv9001_TxPreProc_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXPREPROC_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxPreProc_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxPreProc_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxPreProc_t));



/* ---- ADRV9001_TXDPPROFILE_T ---- */
#define ADRV9001_TXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_TXPREPROC_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txPreProc,                        "txPreProc"); \
ADI_PROCESS_STRUCT_ADRV9001_TXWBINTTOP_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txWbIntTop,                       "txWbIntTop"); \
ADI_PROCESS_STRUCT_ADRV9001_TXNBINTTOP_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txNbIntTop,                       "txNbIntTop"); \
ADI_PROCESS_STRUCT_ADRV9001_TXINTTOP_T                             (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txIntTop,                         "txIntTop"); \
ADI_PROCESS_STRUCT_ADRV9001_TXINTTOPFREQDEVMAP_T                   (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txIntTopFreqDevMap,               "txIntTopFreqDevMap"); \
ADI_PROCESS_STRUCT_ADRV9001_TXIQDMDUC_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance.txIqdmDuc,                        "txIqdmDuc"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance, adrv9001_TxDpProfile_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance, adrv9001_TxDpProfile_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxDpProfile_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxDpProfile_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxDpProfile_t));



/* ---- ADRV9001_TXPROFILE_T ---- */
#define ADRV9001_TXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.primarySigBandwidth_Hz,             "primarySigBandwidth_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txInputRate_Hz,                     "txInputRate_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txInterfaceSampleRate_Hz,           "txInterfaceSampleRate_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txOffsetLo_kHz,                     "txOffsetLo_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.validDataDelay,                     "validDataDelay"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txBbf3dBCorner_kHz,                 "txBbf3dBCorner_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.outputSignaling,                    "outputSignaling"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txPdBiasCurrent,                    "txPdBiasCurrent"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txPdGainEnable,                     "txPdGainEnable"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txPrePdRealPole_kHz,                "txPrePdRealPole_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txPostPdRealPole_kHz,               "txPostPdRealPole_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txBbfPower,                         "txBbfPower"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txExtLoopBackType,                  "txExtLoopBackType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txExtLoopBackForInitCal,            "txExtLoopBackForInitCal"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txPeakLoopBackPower,                "txPeakLoopBackPower"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.frequencyDeviation_Hz,              "frequencyDeviation_Hz"); \
ADI_PROCESS_STRUCT_ADRV9001_TXDPPROFILE_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txDpProfile,                        "txDpProfile"); \
ADI_PROCESS_STRUCT_ADRV9001_SSICONFIG_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance.txSsiConfig,                        "txSsiConfig"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance, adrv9001_TxProfile_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance, adrv9001_TxProfile_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxProfile_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxProfile_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxProfile_t));



/* ---- ADRV9001_TXSETTINGS_T ---- */
#define ADRV9001_TXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance.txInitChannelMask,                 "txInitChannelMask"); \
ADI_PROCESS_ARRAY_ADRV9001_TXPROFILE_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance.txProfile,                         "txProfile"); \

#define ADI_PROCESS_STRUCT_ADRV9001_TXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance, adrv9001_TxSettings_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_TXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_TXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance, adrv9001_TxSettings_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_TXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_TxSettings_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_TxSettings_tName, ARRAY_STRUCT_INDEXER(adrv9001_TxSettings_t));



/* ---- ADRV9001_SSICONFIG_T ---- */
#define ADRV9001_SSICONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.ssiType,                            "ssiType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.ssiDataFormatSel,                   "ssiDataFormatSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.numLaneSel,                         "numLaneSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.strobeType,                         "strobeType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lsbFirst,                           "lsbFirst"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.qFirst,                             "qFirst"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.txRefClockPin,                      "txRefClockPin"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lvdsIBitInversion,                  "lvdsIBitInversion"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lvdsQBitInversion,                  "lvdsQBitInversion"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lvdsStrobeBitInversion,             "lvdsStrobeBitInversion"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lvdsUseLsbIn12bitMode,              "lvdsUseLsbIn12bitMode"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.lvdsRxClkInversionEn,               "lvdsRxClkInversionEn"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.cmosDdrPosClkEn,                    "cmosDdrPosClkEn"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.cmosClkInversionEn,                 "cmosClkInversionEn"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.ddrEn,                              "ddrEn"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance.rxMaskStrobeEn,                     "rxMaskStrobeEn"); \

#define ADI_PROCESS_STRUCT_ADRV9001_SSICONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance, adrv9001_SsiConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_SSICONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_SSICONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance, adrv9001_SsiConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_SSICONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_SsiConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_SsiConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_SsiConfig_t));



/* ---- ADRV9001_RXLNACONFIG_T ---- */
#define ADRV9001_RXLNACONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance) \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.externalLnaPresent,               "externalLnaPresent"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.gpioSourceSel,                    "gpioSourceSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.externalLnaPinSel,                "externalLnaPinSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.settlingDelay,                    "settlingDelay"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.numberLnaGainSteps,               "numberLnaGainSteps"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.lnaGainSteps_mdB,                 "lnaGainSteps_mdB"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.lnaDigitalGainDelay,              "lnaDigitalGainDelay"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance.minGainIndex,                     "minGainIndex"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXLNACONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance, adrv9001_RxLnaConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXLNACONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXLNACONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance, adrv9001_RxLnaConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXLNACONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxLnaConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxLnaConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxLnaConfig_t));



/* ---- ADRV9001_RXRESAMPCONFIG_T ---- */
#define ADRV9001_RXRESAMPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance.rxResampEn,                    "rxResampEn"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance.resampPhaseI,                  "resampPhaseI"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance.resampPhaseQ,                  "resampPhaseQ"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXRESAMPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance, adrv9001_RxResampConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXRESAMPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXRESAMPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance, adrv9001_RxResampConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXRESAMPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxResampConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxResampConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxResampConfig_t));



/* ---- ADRV9001_RXWBNBCOMPPFIR_T ---- */
#define ADRV9001_RXWBNBCOMPPFIR_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance.bankSel,                       "bankSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance.rxWbNbCompPFirInMuxSel,        "rxWbNbCompPFirInMuxSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance.rxWbNbCompPFirEn,              "rxWbNbCompPFirEn"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXWBNBCOMPPFIR_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance, adrv9001_RxWbNbCompPFir_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXWBNBCOMPPFIR_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXWBNBCOMPPFIR_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance, adrv9001_RxWbNbCompPFir_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXWBNBCOMPPFIR_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxWbNbCompPFir_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbNbCompPFir_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxWbNbCompPFir_t));



/* ---- ADRV9001_NCODPCONFIG_T ---- */
#define ADRV9001_NCODPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance.freq,                             "freq"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance.sampleFreq,                       "sampleFreq"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance.phase,                            "phase"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance.realOut,                          "realOut"); \

#define ADI_PROCESS_STRUCT_ADRV9001_NCODPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance, adrv9001_NcoDpConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_NCODPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_NCODPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance, adrv9001_NcoDpConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_NCODPCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_NcoDpConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_NcoDpConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_NcoDpConfig_t));



/* ---- ADRV9001_RXNBNCOCONFIG_T ---- */
#define ADRV9001_RXNBNCOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance.rxNbNcoEn,                      "rxNbNcoEn"); \
ADI_PROCESS_STRUCT_ADRV9001_NCODPCONFIG_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance.rxNbNcoConfig,                  "rxNbNcoConfig"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXNBNCOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance, adrv9001_RxNbNcoConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXNBNCOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXNBNCOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance, adrv9001_RxNbNcoConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXNBNCOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxNbNcoConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbNcoConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxNbNcoConfig_t));



/* ---- ADRV9001_RXDPINFIFOCONFIG_T ---- */
#define ADRV9001_RXDPINFIFOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance.dpInFifoEn,                  "dpInFifoEn"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance.dpInFifoMode,                "dpInFifoMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance.dpInFifoTestDataSel,         "dpInFifoTestDataSel"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXDPINFIFOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance, adrv9001_RxDpInFifoConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXDPINFIFOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXDPINFIFOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance, adrv9001_RxDpInFifoConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXDPINFIFOCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxDpInFifoConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpInFifoConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxDpInFifoConfig_t));



/* ---- ADRV9001_RXNBDEMCONFIG_T ---- */
#define ADRV9001_RXNBDEMCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_RXDPINFIFOCONFIG_T                     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.dpInFifo,                       "dpInFifo"); \
ADI_PROCESS_STRUCT_ADRV9001_RXNBNCOCONFIG_T                        (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.rxNbNco,                        "rxNbNco"); \
ADI_PROCESS_STRUCT_ADRV9001_RXWBNBCOMPPFIR_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.rxWbNbCompPFir,                 "rxWbNbCompPFir"); \
ADI_PROCESS_STRUCT_ADRV9001_RXRESAMPCONFIG_T                       (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.resamp,                         "resamp"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.gsOutMuxSel,                    "gsOutMuxSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.rxOutSel,                       "rxOutSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.rxRoundMode,                    "rxRoundMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance.dpArmSel,                       "dpArmSel"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXNBDEMCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance, adrv9001_RxNbDemConfig_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXNBDEMCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXNBDEMCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance, adrv9001_RxNbDemConfig_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXNBDEMCONFIG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxNbDemConfig_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDemConfig_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxNbDemConfig_t));



/* ---- ADRV9001_RXSINCHBTOP_T ---- */
#define ADRV9001_RXSINCHBTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.sincGainMux,                      "sincGainMux"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.sincMux,                          "sincMux"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.hbMux,                            "hbMux"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.isGainCompEnabled,                "isGainCompEnabled"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.gainComp9GainI,                   "gainComp9GainI"); \
ADI_PROCESS_ARRAY_INT(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance.gainComp9GainQ,                   "gainComp9GainQ"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXSINCHBTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance, adrv9001_RxSincHbTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXSINCHBTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXSINCHBTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance, adrv9001_RxSincHbTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXSINCHBTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxSincHbTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSincHbTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxSincHbTop_t));



/* ---- ADRV9001_RXDECTOP_T ---- */
#define ADRV9001_RXDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance.decBy3Blk15En,                       "decBy3Blk15En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance.decBy2Hb3Blk17p1En,                  "decBy2Hb3Blk17p1En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance.decBy2Hb4Blk17p2En,                  "decBy2Hb4Blk17p2En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance.decBy2Hb5Blk19p1En,                  "decBy2Hb5Blk19p1En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance.decBy2Hb6Blk19p2En,                  "decBy2Hb6Blk19p2En"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance, adrv9001_RxDecTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance, adrv9001_RxDecTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxDecTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDecTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxDecTop_t));



/* ---- ADRV9001_RXWBDECTOP_T ---- */
#define ADRV9001_RXWBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.decBy2Blk25En,                     "decBy2Blk25En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.decBy2Blk27En,                     "decBy2Blk27En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.decBy2Blk29En,                     "decBy2Blk29En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.decBy2Blk31En,                     "decBy2Blk31En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.decBy2Blk33En,                     "decBy2Blk33En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance.wbLpfBlk33p1En,                    "wbLpfBlk33p1En"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXWBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance, adrv9001_RxWbDecTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXWBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXWBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance, adrv9001_RxWbDecTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXWBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxWbDecTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxWbDecTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxWbDecTop_t));



/* ---- ADRV9001_RXNBDECTOP_T ---- */
#define ADRV9001_RXNBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.scicBlk23En,                       "scicBlk23En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.scicBlk23DivFactor,                "scicBlk23DivFactor"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.scicBlk23LowRippleEn,              "scicBlk23LowRippleEn"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk35En,                     "decBy2Blk35En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk37En,                     "decBy2Blk37En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk39En,                     "decBy2Blk39En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk41En,                     "decBy2Blk41En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk43En,                     "decBy2Blk43En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy3Blk45En,                     "decBy3Blk45En"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance.decBy2Blk47En,                     "decBy2Blk47En"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXNBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance, adrv9001_RxNbDecTop_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXNBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXNBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance, adrv9001_RxNbDecTop_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXNBDECTOP_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxNbDecTop_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxNbDecTop_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxNbDecTop_t));



/* ---- ADRV9001_RXDPPROFILE_T ---- */
#define ADRV9001_RXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_RXNBDECTOP_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance.rxNbDecTop,                       "rxNbDecTop"); \
ADI_PROCESS_STRUCT_ADRV9001_RXWBDECTOP_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance.rxWbDecTop,                       "rxWbDecTop"); \
ADI_PROCESS_STRUCT_ADRV9001_RXDECTOP_T                             (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance.rxDecTop,                         "rxDecTop"); \
ADI_PROCESS_STRUCT_ADRV9001_RXSINCHBTOP_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance.rxSincHBTop,                      "rxSincHBTop"); \
ADI_PROCESS_STRUCT_ADRV9001_RXNBDEMCONFIG_T                        (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance.rxNbDem,                          "rxNbDem"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance, adrv9001_RxDpProfile_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance, adrv9001_RxDpProfile_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXDPPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxDpProfile_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxDpProfile_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxDpProfile_t));



/* ---- ADRV9001_RXPROFILE_T ---- */
#define ADRV9001_RXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.primarySigBandwidth_Hz,             "primarySigBandwidth_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxOutputRate_Hz,                    "rxOutputRate_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxInterfaceSampleRate_Hz,           "rxInterfaceSampleRate_Hz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxOffsetLo_kHz,                     "rxOffsetLo_kHz"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxNcoEnable,                        "rxNcoEnable"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.outputSignaling,                    "outputSignaling"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.filterOrder,                        "filterOrder"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.filterOrderLp,                      "filterOrderLp"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.hpAdcCorner,                        "hpAdcCorner"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.lpAdcCorner,                        "lpAdcCorner"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.adcClk_kHz,                         "adcClk_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxCorner3dB_kHz,                    "rxCorner3dB_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxCorner3dBLp_kHz,                  "rxCorner3dBLp_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.tiaPower,                           "tiaPower"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.tiaPowerLp,                         "tiaPowerLp"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.channelType,                        "channelType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.adcType,                            "adcType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.lpAdcCalMode,                       "lpAdcCalMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.gainTableType,                      "gainTableType"); \
ADI_PROCESS_STRUCT_ADRV9001_RXDPPROFILE_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxDpProfile,                        "rxDpProfile"); \
ADI_PROCESS_STRUCT_ADRV9001_RXLNACONFIG_T                          (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.lnaConfig,                          "lnaConfig"); \
ADI_PROCESS_STRUCT_ADRV9001_SSICONFIG_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance.rxSsiConfig,                        "rxSsiConfig"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance, adrv9001_RxProfile_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance, adrv9001_RxProfile_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXPROFILE_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxProfile_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxProfile_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxProfile_t));



/* ---- ADRV9001_RXCHANNELCFG_T ---- */
#define ADRV9001_RXCHANNELCFG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_RXPROFILE_T                            (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance.profile,                         "profile"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXCHANNELCFG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance, adrv9001_RxChannelCfg_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXCHANNELCFG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXCHANNELCFG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance, adrv9001_RxChannelCfg_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXCHANNELCFG_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxChannelCfg_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxChannelCfg_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxChannelCfg_t));



/* ---- ADRV9001_RXSETTINGS_T ---- */
#define ADRV9001_RXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance.rxInitChannelMask,                 "rxInitChannelMask"); \
ADI_PROCESS_ARRAY_ADRV9001_RXCHANNELCFG_T                         (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance.rxChannelCfg,                      "rxChannelCfg"); \

#define ADI_PROCESS_STRUCT_ADRV9001_RXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance, adrv9001_RxSettings_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_RXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_RXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance, adrv9001_RxSettings_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_RXSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_RxSettings_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_RxSettings_tName, ARRAY_STRUCT_INDEXER(adrv9001_RxSettings_t));



/* ---- ADRV9001_CLOCKSETTINGS_T ---- */
#define ADRV9001_CLOCKSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance) \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.deviceClock_kHz,                "deviceClock_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.clkPllVcoFreq_daHz,             "clkPllVcoFreq_daHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.clkPllHsDiv,                    "clkPllHsDiv"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.clkPllMode,                     "clkPllMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.clk1105Div,                     "clk1105Div"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.armClkDiv,                      "armClkDiv"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.armPowerSavingClkDiv,           "armPowerSavingClkDiv"); \
ADI_PROCESS_BOOL     (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.refClockOutEnable,              "refClockOutEnable"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.auxPllPower,                    "auxPllPower"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.clkPllPower,                    "clkPllPower"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.padRefClkDrv,                   "padRefClkDrv"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.extLo1OutFreq_kHz,              "extLo1OutFreq_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.extLo2OutFreq_kHz,              "extLo2OutFreq_kHz"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rfPll1LoMode,                   "rfPll1LoMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rfPll2LoMode,                   "rfPll2LoMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.ext1LoType,                     "ext1LoType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.ext2LoType,                     "ext2LoType"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx1RfInputSel,                  "rx1RfInputSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx2RfInputSel,                  "rx2RfInputSel"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.extLo1Divider,                  "extLo1Divider"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.extLo2Divider,                  "extLo2Divider"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rfPllPhaseSyncMode,             "rfPllPhaseSyncMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx1LoSelect,                    "rx1LoSelect"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx2LoSelect,                    "rx2LoSelect"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.tx1LoSelect,                    "tx1LoSelect"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.tx2LoSelect,                    "tx2LoSelect"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx1LoDivMode,                   "rx1LoDivMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.rx2LoDivMode,                   "rx2LoDivMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.tx1LoDivMode,                   "tx1LoDivMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.tx2LoDivMode,                   "tx2LoDivMode"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.loGen1Select,                   "loGen1Select"); \
ADI_PROCESS_INT      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance.loGen2Select,                   "loGen2Select"); \

#define ADI_PROCESS_STRUCT_ADRV9001_CLOCKSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance, adrv9001_ClockSettings_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_CLOCKSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_CLOCKSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance, adrv9001_ClockSettings_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_CLOCKSETTINGS_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_ClockSettings_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_ClockSettings_tName, ARRAY_STRUCT_INDEXER(adrv9001_ClockSettings_t));



/* ---- ADRV9001_INIT_T ---- */
#define ADRV9001_INIT_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance) \
ADI_PROCESS_STRUCT_ADRV9001_CLOCKSETTINGS_T                        (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance.clocks,                                  "clocks"); \
ADI_PROCESS_STRUCT_ADRV9001_RXSETTINGS_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance.rx,                                      "rx"); \
ADI_PROCESS_STRUCT_ADRV9001_TXSETTINGS_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance.tx,                                      "tx"); \
ADI_PROCESS_STRUCT_ADRV9001_DEVICESYSCONFIG_T                      (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance.sysConfig,                               "sysConfig"); \
ADI_PROCESS_STRUCT_ADRV9001_PFIRBUFFER_T                           (tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance.pfirBuffer,                              "pfirBuffer"); \

#define ADI_PROCESS_STRUCT_ADRV9001_INIT_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance, adrv9001_Init_tName) \
ADI_PROCESS_STRUCT_X(ADRV9001_INIT_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance), tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tName);

#define ADI_PROCESS_ARRAY_ADRV9001_INIT_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance, adrv9001_Init_tName) \
ADI_PROCESS_ARRAY_STRUCT_X(ADRV9001_INIT_T(tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tInstance[ARRAY_STRUCT_INDEXER(adrv9001_Init_t)]), \
                           tokenArray, tokenIndex, jsonBuffer, parsingBuffer, adrv9001_Init_tName, ARRAY_STRUCT_INDEXER(adrv9001_Init_t));




