/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/

/**
* \file adi_adrv903x_utilities_types.h
* \brief Contains ADRV903X API Utilities data types
*
* ADRV903X API Version: 2.12.1.4
*/

#ifndef _ADI_ADRV903X_UTILITIES_TYPES_H_
#define _ADI_ADRV903X_UTILITIES_TYPES_H_

#include "adi_adrv903x_datainterface_types.h"
#include "adi_adrv903x_radioctrl_types.h"
#include "adi_adrv903x_cals_types.h"
#include "adi_adrv903x_gpio_types.h"
#include "adi_adrv903x_user.h"
#include "adi_library_types.h"

#define ADI_ADRV903X_MAX_NUM_CPUS               2U                                  /* Max Number of ADRV903X CPU's */
#define ADI_ADRV903X_PRODUCT_ID_MASK            0x0100U                             /* Product ID, used in Memdump Tools*/

#define ADI_ADRV903X_MAX_NUM_LO                 2U                                  /* Max Number of ADRV903X LO's */

#define ADI_ADRV903X_LOAD_ALL_CPUS              ADI_ADRV903X_MAX_NUM_CPUS           /* Load All Available CPU's */

#define ADI_ADRV903X_LOAD_ALL_RXGAIN_TABLES     ADI_ADRV903X_RX_GAIN_TABLE_ARR_MAX  /* Load All Rx Gain Tables */
#define ADI_ADRV903X_MAX_FILE_LENGTH 256U                                           /* Max file length for all API files */
#define ADI_ADRV903X_VRAM_ONLY_SIZE 4U                                              /* vram only table size */

#define ADI_ADRV903X_VERSION_PREREL_SIZE 30U                                        /* Size of Prerel string for driver/firmware version */

#define ADI_ADRV903X_FIRMWARE_TYPE_ID_SHIFT             24U                         /* Type shift for FW ID*/

#define ADI_ADRV903X_ETF_RAM_SIZE_OFFSET                2U                          /* Amount to shift (<<) ETF RSZ value */
#define ADI_ADRV903X_TRCIDR1_ARCH_SHIFT                 4U                          /* Amount to shift (>>) TRCIDR1 to get ETM Architecture */
#define ADI_ADRV903X_TRCIDR1_ARCH_MASK                  0x00000FF0U                 /* Mask for TRCIDR1 ETM Architecture positions */
#define ADI_ADRV903X_TRACE_FORMAT_CORESIGHT             0x1U                        /* Coresight format for trace data */

#define ADI_ADRV903X_MEMDUMP_DEVICE_DRIVER_HEADER_SIZE          56U                 /* size of memdump device driver header */
#define ADI_ADRV903X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE       44U                 /* size of memdump firmware header */
#define ADI_ADRV903X_MEMDUMP_CPU_RAM_HEADER_SIZE                24U                 /* size of memdump CPU RAM header */
#define ADI_ADRV903X_MEMDUMP_REGISTER_HEADER_SIZE               16U                 /* size of memdump register header */
#define ADI_ADRV903X_MEMDUMP_TELEM_BUFFER_HEADER_SIZE           16U                 /* size of memdump basic telemetry header */
#define ADI_ADRV903X_MEMDUMP_TRACE_BUFFER_HEADER_SIZE           12U                 /* size of memdump basic trace header */

/**
* \brief Macro to restore wrOnly bit
*
* \param commonDev      Device variable having wrOnly bit to be restored
* \param wrOnlyBit      wrOnly bit value to restore
*/
#define ADI_ADRV903X_WRONLY_SET(commonDev, wrOnlyBit)   commonDev.wrOnly = wrOnlyBit

/**
* \brief Extract Init Data function Output. Informs user if 
* the profile bin does not contain the init structures.
*/
typedef enum adi_adrv903x_ExtractInitDataOutput
{
    ADI_ADRV903X_EXTRACT_INIT_DATA_NOT_POPULATED = 0x00,
    /*!< Default value when value is not ran yet */
    ADI_ADRV903X_EXTRACT_INIT_DATA_POPULATED = 0x02,
    /*!< Profile bin contains init structures and were populated */
    ADI_ADRV903X_EXTRACT_INIT_DATA_LEGACY_PROFILE_BIN = 0x4 
    /*!< Profile bin does NOT contains init structures and function exited without triggering errors */
} adi_adrv903x_ExtractInitDataOutput_e;

/**
* \brief Accomodate generic strings to cross client RPC interface
*/
typedef struct adi_adrv903x_GenericStrBuf
{
    uint8_t c_str[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< String contents */
} adi_adrv903x_GenericStrBuf_t;

/**
* \brief Stream Binary Info Data Structure
*/
typedef struct adi_adrv903x_streamBinaryInfo
{
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for Stream Binary Image */
} adi_adrv903x_streamBinaryInfo_t;

/**
* \brief CPU Binary Info Data Structure
*/
typedef struct adi_adrv903x_cpuBinaryInfo
{
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for CPU Core Binary Image */
} adi_adrv903x_cpuBinaryInfo_t;

/**
* \brief CPU Profile Info Data Structure
*/
typedef struct adi_adrv903x_CpuProfileBinaryInfo
{
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for CPU Profile Binary Image*/
} adi_adrv903x_CpuProfileBinaryInfo_t;

/**
* \brief CPU Memory Dump Info Data Structure
*/
typedef struct adi_adrv903x_CpuMemDumpBinaryInfo
{
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for CPU Memory Dump Binary Image*/
} adi_adrv903x_CpuMemDumpBinaryInfo_t;

/**
* \brief RX Gain Table Info Data Structure
*/
typedef struct adi_adrv903x_RxGainTableInfo
{
    uint32_t    channelMask;    /*!< Channel Mask (i.e. Specify Channels, that are initialized, with the given Rx Gain Table */
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for RX Gain Table */
} adi_adrv903x_RxGainTableInfo_t;

/**
* \brief TRX File Info Data Structure
*/
typedef struct adi_adrv903x_TrxFileInfo
{
    adi_adrv903x_streamBinaryInfo_t     stream;                                             /*!< Stream File Settings */
    adi_adrv903x_cpuBinaryInfo_t        cpu;                                                /*!< CPU File Settings */
    adi_adrv903x_CpuProfileBinaryInfo_t cpuProfile;                                         /*!< CPU Profile File Settings (may also contain init data structures) */
    adi_adrv903x_RxGainTableInfo_t      rxGainTable[ADI_ADRV903X_RX_GAIN_TABLE_ARR_MAX];    /*!< Rx Gain Table Settings */
} adi_adrv903x_TrxFileInfo_t;

/**
* \brief Data structure to hold Utility Init structures
*/
typedef struct adi_adrv903x_PostMcsInit
{
    adi_adrv903x_RadioCtrlModeCfg_t   radioCtrlCfg;             /*!< Holds the setup for the Radio Control Mode (SPI vs PIN mode) used for Radio Control */
    adi_adrv903x_RadioCtrlTxRxEnCfg_t radioCtrlGpioCfg;         /*!< Holds the setup for each of the GPIO pins used for Radio Control */
    uint8_t radioCtrlTxRxEnPinSel;                              /*!< Holds the setup for the TxRxEn pin select used for Radio Control */
    uint8_t radioCtrlTxRxEnCfgSel;                              /*!< Holds the setup for the TxRxEn config select used for Radio Control */
    adi_adrv903x_GpIntPinMaskCfg_t gpIntPostInit;               /*!< Holds the setup for GP Interrupt Pin Mask Config to be applied after initialization*/
    adi_adrv903x_InitCals_t initCals;                           /*!< Holds the setup for Cals Initialization configuration */
} adi_adrv903x_PostMcsInit_t;

/**
* \brief Standby mode recover data structure
*/
typedef struct adi_adrv903x_StandbyRecover
{
    adi_adrv903x_TrackingCalEnableMasks_t tcEnableMasks;            /*!< Tracking cals to be re-enabled on Power up */
    uint32_t                              orxEnabledMask;           /*!< Corresponds to the enabled ORX channels to Recover */
    uint32_t                              rxEnabledMask;            /*!< Corresponds to the enabled RX channels to Recover */
    uint32_t                              txEnabledMask;            /*!< Corresponds to the enabled TX channels to Recover */
    uint32_t                              lo0PllPowerDownCfg;       /*!< RF LO0 PLL Power Down Recover Config */
    uint32_t                              lo1PllPowerDownCfg;       /*!< RF LO1 PLL Power Down Recover Config */
    uint32_t                              serdesPllPowerDownCfg;    /*!< Serdes PLL Power Down Recover Config */
    uint32_t                              clkPllPowerDownCfg;       /*!< Clk PLL Power Down Recover Config */
    uint32_t                              rxAdcPowerDownCtrl;       /*!< Rx ADC Power Down Recover Config */
    uint32_t                              txLbAdcTrmPowerDownCtrl;  /*!< Tx Loopback ADC Power Down Recover Config */
    uint16_t                              deserializerPowerDownReg; /*!< Deserializer Power Down Recover Config */
} adi_adrv903x_StandbyRecover_t;


/**
* \brief Jrx Repair Mode, offer options that are applicable for adi_adrv903x_JrxRepairExecute function
*/
typedef enum adi_adrv903x_JrxRepairRunMode
{
    ADI_ADRV903X_JRX_REPAIR_NORMAL_MODE = 0U, /*!< Normal mode, in where the bias survey is executed */
    ADI_ADRV903X_JRX_REPAIR_FAST_MODE   = 1U, /*!< Fast mode, in where the SwC is applied directly and the links are tested */
} adi_adrv903x_JrxRepairRunMode_e;


/**
* \brief Jrx Repair status check flags
*/
typedef enum adi_adrv903x_JrxRepairHistoryCheck
{
    ADI_ADRV903X_JRX_REPAIR_CHECKED               = 0x001U, /*!< Check function not ran */
    ADI_ADRV903X_JRX_REPAIR_LOAD_HISTORY          = 0x002U, /*!< Jrx Repair History to Load */
    ADI_ADRV903X_JRX_REPAIR_ASSESS_LANES          = 0x004U, /*!< Perform Lane Assessment */
    ADI_ADRV903X_JRX_REPAIR_FAULTY_VCM_AMP        = 0x006U, /*!< Possible faulty VCM AMP identified */
    ADI_ADRV903X_JRX_REPAIR_SCREENED              = 0x008U, /*!< Screened part */
    ADI_ADRV903X_JRX_REPAIR_UNKNOWN_ERROR         = 0x010U, /*!< No Errors or unidentified Error */
    ADI_ADRV903X_JRX_REPAIR_TEMP_GT_LAST          = 0x020U, /*!< Temperature greater than Last History */
    ADI_ADRV903X_JRX_REPAIR_SWC_ALREADY_ENABLED   = 0x040U, /*!< SWC already enabled */
    ADI_ADRV903X_JRX_REPAIR_NO_LANE_ERRORS        = 0x080U, /*!< No bad or weak lanes discovered */
    ADI_ADRV903X_JRX_REPAIR_APPLY_SUCCESS         = 0x100U, /*!< Faulty VCM AMP issue has been fixed */
} adi_adrv903x_JrxRepairHistoryCheck_e;

/**
* \brief Jrx Repair Bias Ctrl
*
*/
typedef enum adi_adrv903x_JrxRepairBiasCtrlFix
{
    ADI_ADRV903X_JRX_REPAIR_MIN_BIAS = 3U,
    ADI_ADRV903X_JRX_REPAIR_MAX_BIAS = 4U,
    ADI_ADRV903X_JRX_REPAIR_BIAS_NUM = (ADI_ADRV903X_JRX_REPAIR_MAX_BIAS - ADI_ADRV903X_JRX_REPAIR_MIN_BIAS + 1U)
} adi_adrv903x_JrxRepairBiasCtrlFix_e;

/**
* \brief Jrx Repair history data from the last time a repair was executed
*
*/
typedef struct adi_adrv903x_JrxRepairHistory
{
    int16_t lastTemp;     /*!< readback temperature */
    uint8_t badLaneMask;  /*!< Lane mask for bad performance lanes */
    uint8_t weakLaneMask; /*!< Lane mask for weak performance lanes */
    uint8_t goodLaneMask; /*!< Lane mask for good performance lanes */
} adi_adrv903x_JrxRepairHistory_t;

/**
* \brief Jrx Repair Lane Test Result data
*
*/
typedef struct adi_adrv903x_JrxRepairTest
{
    uint32_t laneErrors[ADI_ADRV903X_MAX_DESERIALIZER_LANES]; /*!< Number of errors recorded on each lane */
} adi_adrv903x_JrxRepairTest_t;

/**
* \brief Jrx Repair Lane Bias Survey Result data
*
*/
typedef struct adi_adrv903x_JrxRepairBiasSurvey
{
    adi_adrv903x_JrxRepairTest_t biasTests[ADI_ADRV903X_JRX_REPAIR_BIAS_NUM]; /*!< Jrx Repair Test result for each bias value */
} adi_adrv903x_JrxRepairBiasSurvey_t;

/**
* \brief Jrx Repair Binary Info Data Structure
*
*/
typedef struct adi_adrv903x_JrxRepairBinaryInfo
{
    uint8_t filePath[ADI_ADRV903X_MAX_FILE_LENGTH]; /*!< File Path for Jrx Repair History Binary File */
} adi_adrv903x_JrxRepairBinaryInfo_t;

/**
* \brief Jrx Repair Struct that holds all the required data for using the Jrx repair facility
*
*/
typedef struct adi_adrv903x_JrxRepair
{
    adi_adrv903x_JrxRepairBiasSurvey_t   biasSurvey;        /*!< Jrx Repair Lane Bias Survey Result data from PRBS */
    adi_adrv903x_JrxRepairHistory_t      history;           /*!< Jrx Repair history data, can be given by user, or loaded from file or calculated */
    adi_adrv903x_DfrmPrbsCfg_t           dfrmPrbsCfg;       /*!< Jrx Repair PRBS configuration */
    uint8_t                              tcEnabledLaneMask; /*!< Enabled Lanes for serdes tracking calibration before JrxRepairEnter */
    uint32_t                             historyCheck;      /*!< Jrx Repair history check result */
    adi_adrv903x_JrxRepairBinaryInfo_t   fileInfo;          /*!< File Path for stored binary adi_adrv903x_JrxRepairHistory_t */
    adi_adrv903x_JrxRepairRunMode_e      runMode;           /*!< Run Mode for the adi_adrv903x_JrxRepairExecute function */
} adi_adrv903x_JrxRepair_t;

#endif /* _ADI_ADRV903X_TX_TYPES_H_ */
