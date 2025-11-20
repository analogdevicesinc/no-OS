/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_utilities_types.h
* \brief Contains ADRV904X API Utilities data types
*
* ADRV904X API Version: 2.10.0.4
*/

#ifndef _ADI_ADRV904X_UTILITIES_TYPES_H_
#define _ADI_ADRV904X_UTILITIES_TYPES_H_

#include "adi_adrv904x_datainterface_types.h"
#include "adi_adrv904x_radioctrl_types.h"
#include "adi_adrv904x_cals_types.h"
#include "adi_adrv904x_gpio_types.h"
#include "adi_adrv904x_user.h"
#include "adi_library_types.h"

#include "adi_adrv904x_dfe_cpu_types.h"

    #define ADI_ADRV904X_MAX_NUM_CPUS           3U                                  /* Max Number of ADRV904X CPU's */
    #define ADI_ADRV904X_PRODUCT_ID_MASK        0x0200U                             /* Product ID, used in Memdump Tools*/

#define ADI_ADRV904X_MAX_NUM_LO                 2U                                  /* Max Number of ADRV904X LO's */

#define ADI_ADRV904X_LOAD_ALL_CPUS              ADI_ADRV904X_MAX_NUM_CPUS           /* Load All Available CPU's */

#define ADI_ADRV904X_LOAD_ALL_RXGAIN_TABLES     ADI_ADRV904X_RX_GAIN_TABLE_ARR_MAX  /* Load All Rx Gain Tables */
#define ADI_ADRV904X_MAX_FILE_LENGTH 256U                                           /* Max file length for all API files */
#define ADI_ADRV904X_VRAM_ONLY_SIZE 4U                                              /* vram only table size */

#define ADI_ADRV904X_VERSION_PREREL_SIZE 30U                                        /* Size of Prerel string for driver/firmware version */

#define ADI_ADRV904X_FIRMWARE_TYPE_ID_SHIFT             24U                         /* Type shift for FW ID*/

#define ADI_ADRV904X_ETM_RECORD_SIZE                    8U                          /* Record size per-core for ETM Trace record */
#define ADI_ADRV904X_MAX_DFE_ETM                        4U                          /* Max number of ADRV904X DFE ETM's */
#define ADI_ADRV904X_ETF_RAM_SIZE_OFFSET                2U                          /* Amount to shift (<<) ETF RSZ value */
#define ADI_ADRV904X_TRCIDR1_ARCH_SHIFT                 4U                          /* Amount to shift (>>) TRCIDR1 to get ETM Architecture */
#define ADI_ADRV904X_TRCIDR1_ARCH_MASK                  0x00000FF0U                 /* Mask for TRCIDR1 ETM Architecture positions */
#define ADI_ADRV904X_TRACE_FORMAT_CORESIGHT             0x1U                        /* Coresight format for trace data */

#define ADI_ADRV904X_MEMDUMP_DEVICE_DRIVER_HEADER_SIZE          56U                 /* size of memdump device driver header */
#define ADI_ADRV904X_MEMDUMP_FIRMWARE_VERSION_HEADER_SIZE       44U                 /* size of memdump firmware header */
#define ADI_ADRV904X_MEMDUMP_CPU_RAM_HEADER_SIZE                24U                 /* size of memdump CPU RAM header */
#define ADI_ADRV904X_MEMDUMP_REGISTER_HEADER_SIZE               16U                 /* size of memdump register header */
#define ADI_ADRV904X_MEMDUMP_TELEM_BUFFER_HEADER_SIZE           16U                 /* size of memdump basic telemetry header */
#define ADI_ADRV904X_MEMDUMP_TRACE_BUFFER_HEADER_SIZE           12U                 /* size of memdump basic trace header */

/**
* \brief Macro to restore wrOnly bit
*
* \param commonDev      Device variable having wrOnly bit to be restored
* \param wrOnlyBit      wrOnly bit value to restore
*/
#define ADI_ADRV904X_WRONLY_SET(commonDev, wrOnlyBit)   commonDev.wrOnly = wrOnlyBit

/**
* \brief Extract Init Data function Output. Informs user if 
* the profile bin does not contain the init structures.
*/
typedef enum adi_adrv904x_ExtractInitDataOutput
{
    ADI_ADRV904X_EXTRACT_INIT_DATA_NOT_POPULATED = 0x00,
    /*!< Default value when value is not ran yet */
    ADI_ADRV904X_EXTRACT_INIT_DATA_POPULATED = 0x02,
    /*!< Profile bin contains init structures and were populated */
    ADI_ADRV904X_EXTRACT_INIT_DATA_LEGACY_PROFILE_BIN = 0x4 
    /*!< Profile bin does NOT contains init structures and function exited without triggering errors */
} adi_adrv904x_ExtractInitDataOutput_e;

/**
* \brief Accomodate generic strings to cross client RPC interface
*/
typedef struct adi_adrv904x_GenericStrBuf
{
    uint8_t c_str[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< String contents */
} adi_adrv904x_GenericStrBuf_t;

/**
* \brief Stream Binary Info Data Structure
*/
typedef struct adi_adrv904x_streamBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for Stream Binary Image */
} adi_adrv904x_streamBinaryInfo_t;

/**
* \brief CPU Binary Info Data Structure
*/
typedef struct adi_adrv904x_cpuBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for CPU Core Binary Image */
} adi_adrv904x_cpuBinaryInfo_t;

/**
* \brief CPU Profile Info Data Structure
*/
typedef struct adi_adrv904x_CpuProfileBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for CPU Profile Binary Image*/
} adi_adrv904x_CpuProfileBinaryInfo_t;

/**
* \brief CPU Memory Dump Info Data Structure
*/
typedef struct adi_adrv904x_CpuMemDumpBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for CPU Memory Dump Binary Image*/
} adi_adrv904x_CpuMemDumpBinaryInfo_t;

/**
* \brief RX Gain Table Info Data Structure
*/
typedef struct adi_adrv904x_RxGainTableInfo
{
    uint32_t    channelMask;    /*!< Channel Mask (i.e. Specify Channels, that are initialized, with the given Rx Gain Table */
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for RX Gain Table */
} adi_adrv904x_RxGainTableInfo_t;

/**
* \brief Radio Sequencer Binary Info Data Structure
*/
typedef struct adi_adrv904x_RadioSequencerBinaryInfo
{
    uint8_t filePath[ADI_ADRV904X_MAX_FILE_LENGTH]; /*!< File Path for Radio Sequencer Binary Image */
} adi_adrv904x_RadioSequencerBinaryInfo_t;

/**
* \brief TRX File Info Data Structure
*/
typedef struct adi_adrv904x_TrxFileInfo
{
    adi_adrv904x_streamBinaryInfo_t     stream;                                             /*!< Stream File Settings */
    adi_adrv904x_cpuBinaryInfo_t        cpu;                                                /*!< CPU File Settings */
    adi_adrv904x_CpuProfileBinaryInfo_t cpuProfile;                                         /*!< CPU Profile File Settings (may also contain init data structures) */
    adi_adrv904x_RxGainTableInfo_t      rxGainTable[ADI_ADRV904X_RX_GAIN_TABLE_ARR_MAX];    /*!< Rx Gain Table Settings */
    adi_adrv904x_DfeCpuBinaryInfo_t     dfeCpu;                                             /*!< DFE CPU File Settings */
    adi_adrv904x_RadioSequencerBinaryInfo_t     radioSequencer;                             /*!< Radio Sequencer File Settings */
} adi_adrv904x_TrxFileInfo_t;

/**
* \brief Data structure to hold Utility Init structures
*/
typedef struct adi_adrv904x_PostMcsInit
{
    adi_adrv904x_RadioCtrlModeCfg_t   radioCtrlCfg;             /*!< Holds the setup for the Radio Control Mode (SPI vs PIN mode) used for Radio Control */
    adi_adrv904x_RadioCtrlTxRxEnCfg_t radioCtrlGpioCfg;         /*!< Holds the setup for each of the GPIO pins used for Radio Control */
    uint8_t radioCtrlTxRxEnPinSel;                              /*!< Holds the setup for the TxRxEn pin select used for Radio Control */
    uint8_t radioCtrlTxRxEnCfgSel;                              /*!< Holds the setup for the TxRxEn config select used for Radio Control */
    adi_adrv904x_GpIntPinMaskCfg_t gpIntPostInit;               /*!< Holds the setup for GP Interrupt Pin Mask Config to be applied after initialization*/
    adi_adrv904x_InitCals_t initCals;                           /*!< Holds the setup for Cals Initialization configuration */
} adi_adrv904x_PostMcsInit_t;

#endif /* _ADI_ADRV904X_TX_TYPES_H_ */
