/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_error_types.h
 *
 * \brief Device Specific Error Types
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_ERROR_TYPES_H_
#define _ADI_FPGAGEN6_ERROR_TYPES_H_

#include "adi_common_error.h"


/* General Error Codes; Reserved: 0 - 99 */
#define ADI_FPGAGEN6_ERRCODE_NO_ERROR                                       0UL       /* General No Error Code */
#define ADI_FPGAGEN6_ERRCODE_REGISTER_IO                                    1UL       /* General Register IO Error Code */

/* Axi ADV SPI Error Codes; Reserved: 100 - 199 */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADV_SPI_TEST                               100UL       /* Example */

/* Data Interface Error Codes; Reserved: 200 - 299 */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER0        200UL       /* Deframer 0 LaneXbar Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER1        201UL       /* Deframer 1 LaneXbar Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER2        202UL       /* Deframer 2 LaneXbar Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER0          203UL       /* Framer 0 LaneXbar Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER1          204UL       /* Framer 1 LaneXbar Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_GET             205UL       /* Deframer LaneXbar Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_SET             206UL       /* Deframer LaneXbar Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_GET             207UL       /* Framer LaneXbar Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_SET             208UL       /* Framer LaneXbar Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_GET                209UL       /* Deframer Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_SET                210UL       /* Deframer Config Set Error Code  */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_GET                  211UL       /* Framer Config Get Error Code  */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_SET                  212UL       /* Framer Config Set Error Code  */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET              213UL       /* Serializer Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET              214UL       /* Serializer Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_SET            215UL       /* Serializer Reset Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_DONE_GET       216UL       /* Serializer Reset Done Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET            217UL       /* Deserializer Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET            218UL       /* Deserializer Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_SET          219UL       /* Deserializer Reset Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_DONE_GET     220UL       /* Deserializer Reset Done Error Code  */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_SET              221UL       /* Deframer Reset Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_RELEASE          222UL       /* Deframer Reset Release Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_POLL             223UL       /* Deframer Reset Poll Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_SET                224UL       /* Framer Reset Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_RELEASE            225UL       /* Framer Reset Release Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_POLL               226UL       /* Framer Reset Poll Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_ENABLE_SET               227UL       /* SysRef Enable Set Error Code  */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_INTERNAL_SYSREF_ENABLE_SET      228UL       /* Internal Sysref Enable Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_INTERNAL_SYSREF_REQUEST         229UL       /* Internal Sysref Request Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET                 230UL       /* Link Status Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET                     231UL       /* SyncBar Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET                     232UL       /* SyncBar Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DATALINK_CFG_SET                233UL       /* DataLinke Configuration Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET              234UL       /* SysREF Configuration Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET              235UL       /* SysREF Configuration Get Error Code*/

/* Hal Error Codes; Reserved: 300 - 399 */
#define ADI_FPGAGEN6_ERRCODE_HAL_RAM_READ                                   300UL       /* HAL RAM Read Error Code */
#define ADI_FPGAGEN6_ERRCODE_HAL_RAM_WRITE                                  301UL       /* HAL RAM Write Error Code */
#define ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_READ                             302UL       /* Register Field Read Error Code */
#define ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_WRITE                            303UL       /* Register Field Write Error Code */

/* Rx Error Codes; Reserved: 400 - 499 */
#define ADI_FPGAGEN6_ERRCODE_RX_TRANSPORT_CFG_SET                           400UL       /* Rx Transport Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_TRANSPORT_CFG_GET                           401UL       /* Rx Transport Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_START                               402UL       /* Rx Capture Start Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_WAIT                                403UL       /* Rx Capture Wait Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET                             404UL       /* Rx Datachain Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_DMA_HALT_TIMEOUT                            405UL       /* Rx DMA Halt Timeout Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET                                 406UL       /* Rx DMA Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET                                 407UL       /* Rx DMA Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG                               408UL       /* Rx Datachain Configure Error Code */

/* Tx Error Codes; Reserved: 500 - 599 */
#define ADI_FPGAGEN6_ERRCODE_TX_DATA_START                                  500UL       /* Tx Data Start Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DATA_STOP                                   501UL       /* Tx Data Stop Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET                             502UL       /* Tx Capture Wait Error Code*/
#define ADI_FPGAGEN6_ERRCODE_TX_DMA_HALT_TIMEOUT                            503UL       /* Tx DMA Halt Timeout Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET                                 504UL       /* Tx DMA Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET                                 505UL       /* Tx DMA Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG                               506UL       /* Tx Datachain Configure Error Code*/
#define ADI_FPGAGEN6_ERRCODE_TX_DATA_MM_ENABLE                              507UL       /* Tx Data Manual Mode Enable Error Code */
#define ADI_FPGAGEN6_ERRCODE_TX_DATA_MM_ADVANCE                             508UL       /* Tx Data Manual Mode Advance Error Code */

/* TDD Error Codes; Reserved: 600 - 699 */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_SET                          600UL       /* TRX Manual Enable Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_SET                          601UL       /* ORX Manual Enable Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_SET                         602UL       /* GPIO Manual Enable Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_SET                          603UL       /* External Manual Enable Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_GET                          604UL       /* TRX Manual Enable Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_GET                          605UL       /* ORX Manual Enable Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_GET                         606UL       /* GPIO Manual Enable Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_GET                          607UL       /* External Manual Enable Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_FRAME_LOOP                                 608UL       /* Frame Loop Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_FRAME_COUNT                                609UL       /* Frame Count Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_STATE_MACHINE_DISABLE                      610UL       /* TDD State Machine Disable Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_STATE_MACHINE_ENABLE                       611UL       /* TDD State Machine Enable Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_GET                                612UL       /* TDD State Machine Configuration Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_FSM_CFG_SET                                613UL       /* TDD State Machine Configuration Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE                        614UL       /* TDD Enable Control Disable Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC                                  615UL       /* TDD GPIO Sync Enable Error Code */
#define ADI_FPGAGEN6_ERRCODE_TDD_GPIO_SYNC_SELECT                           616UL       /* TDD GPIO Sync Select Error Code */

/* ADS10v1 Error Codes; Reserved: 700 - 799 */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_GET                    700UL       /* SMA Pin Configuration Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET                    701UL       /* SMA Pin Configuration Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_GLOBAL_RESET                       702UL       /* ADS10 Global Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET                        703UL       /* LED Configuration Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET                        704UL       /* LED Configuration Set Error Code */

/* PRBS Error Codes; Reserved: 800 - 899 */
#define ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE                         800UL       /* PRBS Serializer Enable Error Code */
#define ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE                       801UL       /* PRBS Deserializer Enable Error Code */
#define ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ERROR_INJECT                   802UL       /* PRBS Serializer Error Inject Error Code */
#define ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_COUNT_GET                           803UL       /* PRBS Error Count Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_CLEAR                               804UL       /* PRBS Error Clear Error Code */

/* Utility Error Codes; Reserved: 900 - 999 */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK                  900UL       /* MMCM Configure & Lock Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_DESERIALIZER_CFG                     901UL       /* Deserializer Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_SERIALIZER_CFG                       902UL       /* Serializer Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_LANEXBAR_RESET_CFG                   903UL       /* LaneXBar Reset Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_DEFRAMER_CFG                         904UL       /* Deframer Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_FRAMER_CFG                           905UL       /* Framer Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG                             906UL       /* Init Config Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET                             907UL       /* Init Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_UTILITIES_FPGA_VERSION_INVALID                 908UL       /* Init Set Error Code */

/* Core Error Codes; Reserved: 1000 - 1099 */
#define ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP                     1000UL      /* Drp Field Write Setup Error Code */
#define ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE                           1001UL      /* Drp Field Write Error Code */
#define ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP                      1002UL      /* Drp Field Read Setup Error Code */
#define ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ                            1003UL      /* Drp Field Read Error Code */
#define ADI_FPGAGEN6_ERRCODE_CORE_HW_RESET                                  1004UL      /* HW Reset Error Code */

/* GPIO Error Codes; Reserved: 1100 - 1199 */
#define ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET                                  1100UL      /* GPIO Mode Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_INPUT_DIR_SET                             1101UL      /* GPIO Input Direction Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_SET                            1102UL      /* GPIO Output Direction Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_GET                            1103UL      /* GPIO Mode Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_WRITE                                     1104UL      /* GPIO Write Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_READ                                      1105UL      /* GPIO Read Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_SET                               1106UL      /* GPIO TDD Mux Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_GET                               1107UL      /* GPIO TDD Mux Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_CLEAR                             1108UL      /* GPIO TDD Mux Clear Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SPI2_MODE_SET                             1109UL      /* GPIO SPI2 Mode Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET                             1110UL      /* GPIO JTAG Mode Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_SET                           1111UL      /* GP Interrupt Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_GET                           1112UL      /* GP Interrupt Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_GET                        1113UL      /* GP Interrupt Status Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_CLEAR                      1114UL      /* GP Interrupt Status Clear Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_COUNTER_GET                       1115UL      /* GP Interrupt Counter Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_EXT_TRIGGER_SET                       1116UL      /* GP Select External Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_EXT_TRIGGER_GET                       1117UL      /* GP Select External Trigger Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_RXDP_TRIGGER_SET                      1118UL      /* GP Select Rxdp Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_RXDP_TRIGGER_GET                      1119UL      /* GP Select Rxdp Trigger Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_TXDP_TRIGGER_SET                      1120UL      /* GP Select Txdp Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_SEL_TXDP_TRIGGER_GET                      1121UL      /* GP Select Txdp Trigger Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_EXT_TRIGGER_SET                     1122UL      /* GP Delay External Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_EXT_TRIGGER_GET                     1123UL      /* GP Delay External Trigger Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_RXDP_TRIGGER_SET                    1124UL      /* GP Delay RXDP Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_RXDP_TRIGGER_GET                    1125UL      /* GP Delay RXDP Trigger Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_TXDP_TRIGGER_SET                    1126UL      /* GP Delay TXDP Trigger Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_GPIO_DELAY_TXDP_TRIGGER_GET                    1127UL      /* GP Delay TXDP Trigger Get Error Code */

/* Clk Error Codes; Reserved: 1200 - 1299 */
#define ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET                               1200UL      /* MMCM Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET                               1201UL      /* MMCM Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_MMCM_RESET                                 1202UL      /* MMCM Reset Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_MMCM_LOCK_GET                              1203UL      /* MMCM Lock Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET                               1204UL      /* QPLL Config Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET                               1205UL      /* QPLL Config Get Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_QPLL_LOCK_GET                              1206UL      /* QPLL Lock Set Error Code */
#define ADI_FPGAGEN6_ERRCODE_CLK_LANE_PWR_GOOD_GET                          1207UL      /* Lane Power Good Get Error Code */

/*
*  \brief   FPGAGEN6 File Abstractions
*/
typedef enum
{
    ADI_FPGAGEN6_FILE_PUBLIC_CORE   = 500U,
    ADI_FPGAGEN6_FILE_PUBLIC_HAL,
    ADI_FPGAGEN6_FILE_PUBLIC_RX,
    ADI_FPGAGEN6_FILE_PUBLIC_TX,
    ADI_FPGAGEN6_FILE_PUBLIC_TDD,
    ADI_FPGAGEN6_FILE_PRIVATE_TDD,
    ADI_FPGAGEN6_FILE_PUBLIC_AXI_ADV_SPI,
    ADI_FPGAGEN6_FILE_PRIVATE_RX,
    ADI_FPGAGEN6_FILE_PRIVATE_TX,
    ADI_FPGAGEN6_FILE_PUBLIC_DATA_INTERFACE,
    ADI_FPGAGEN6_FILE_PRIVATE_DATA_INTERFACE,
    ADI_FPGAGEN6_FILE_PUBLIC_UTILITIES,
    ADI_FPGAGEN6_FILE_PRIVATE_TEST,
    ADI_FPGAGEN6_FILE_PRIVATE_CORE,
    ADI_FPGAGEN6_FILE_PUBLIC_CLK,
    ADI_FPGAGEN6_FILE_PUBLIC_GPIO,
    ADI_FPGAGEN6_FILE_PUBLIC_AXI_ADS10V1,
    ADI_FPGAGEN6_FILE_PUBLIC_PRBS,
    ADI_FPGAGEN6_FILE_PRIVATE_DRP_QPLL_COMMON,
    ADI_FPGAGEN6_FILE_PRIVATE_DRP_QPLL_CHANNEL,
    ADI_FPGAGEN6_FILE_PUBLIC_ERROR
} adi_fpgagen6_File_e;

/*
*  \brief   FPGAGEN6 Error Source Map
*/
typedef enum
{
    ADI_FPGAGEN6_ERRSRC_NONE            = ADI_COMMON_ERRSRC_NONE_IDX,       /* No Error/Source; Needs to Match Common Error Source */
    ADI_FPGAGEN6_ERRSRC_API             = ADI_COMMON_ERRSRC_API_IDX,        /* API;             Needs to Match Common Error Source */
    ADI_FPGAGEN6_ERRSRC_HAL             = ADI_COMMON_ERRSRC_HAL_IDX,        /* HAL;             Needs to Match Common Error Source */
    ADI_FPGAGEN6_ERRSRC_DEVICEHAL       = ADI_COMMON_ERRSRC_DEVICEHAL_IDX,  /* Device HAL;      Needs to Match Common Error Source */
    ADI_FPGAGEN6_ERRSRC_DEVICEBF        = ADI_COMMON_ERRSRC_DEVICEBF_IDX,   /* Device BF;       Needs to Match Common Error Source */
    ADI_FPGAGEN6_ERRSRC_GEN,                                                /* FPGA General Errors */
    ADI_FPGAGEN6_ERRSRC_AXI_ADV_SPI,                                        /* AXI ADV SPI Errors */
    ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE,                                     /* Data Interface Errors */
    ADI_FPGAGEN6_ERRSRC_RX,                                                 /* Rx Errors */
    ADI_FPGAGEN6_ERRSRC_PRBS,                                               /* PRBS Errors */
    ADI_FPGAGEN6_ERRSRC_TDD,                                                /* TDD Errors */
    ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1,                                        /* AXI ADS10v1 Errors */
    ADI_FPGAGEN6_ERRSRC_UTILITIES,                                          /* Utility Errors */
    ADI_FPGAGEN6_ERRSRC_CORE,                                               /* Core Errors */
    ADI_FPGAGEN6_ERRSRC_GPIO,                                               /* GPIO Errors */
    ADI_FPGAGEN6_ERRSRC_CLK,                                                /* Clock Errors */
    ADI_FPGAGEN6_ERRSRC_TX,                                                 /* Tx Errors */
    ADI_FPGAGEN6_ERRSRC_UNKNOWN                                             /* Unknown */
} adi_fpgagen6_ErrSource_e;

/*
*  \brief ADI FPGAGEN6 Recovery Actions
*/
typedef enum
{
    ADI_FPGAGEN6_ERR_ACT_RESET_DEVICE       = -500,         /* Device NOK: HW/SW Reset Required */
        /* All Reset Feature Unique Codes exist between -500 & -400 */
    ADI__FPGAGEN6_ERR_ACT_RESET_FEATURE     = -400,         /* API NOK: Feature Reset Required */
        /* All Reset Interface Unique Codes exist between -400 & -300 */
    ADI_FPGAGEN6_ERR_ACT_RESET_INTERFACE    = -300,         /* API NOK: Interface Reset Required */
        /* All Check Feature Unique Codes exist between -300 & -200 */
    ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE      = -200,         /* API OK; Feature is reporting an Error */
        /* All Check Interface Unique Codes exist between -200 & -100 */
    ADI_FPGAGEN6_ERR_ACT_CHECK_INTERFACE    = -100,         /* API OK; Interface is reporting an Error */
    ADI_FPGAGEN6_ERR_ACT_OPEN_DEVICE        = -10,          /* API OK; Device Not Open */
    ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM        = -1,           /* API OK; Invalid Parameter passed to function */
    ADI_FPGAGEN6_ERR_ACT_NONE               = 0             /* API OK; No Action Required */
} adi_fpgagen6_ErrAction_e;

/*
*  \brief ADI FPGAGEN6 Error Table Row
*/
typedef struct
{
    const int64_t                   errCode;
    const char* const               errMsg;
    const adi_fpgagen6_ErrAction_e  action;
} adi_fpgagen6_ErrTableRow_t;

/*
*  \brief ADI FPGAGEN6 Device Error Table Row
*/
typedef struct
{
    const int64_t                   errCode;
    const char* const               errMsg;
    const char* const               errCause;
    const adi_fpgagen6_ErrAction_e  actionCode;
    const char* const               actionMsg;
} adi_fpgagen6_DeviceErrTableRow_t;

/*
*  \brief ADI FPGAGEN6 Error Information
*/
typedef struct
{
    int64_t                     errCode;
    const char*                 errMsg;
    const char*                 errCause;
    adi_fpgagen6_ErrAction_e    actionCode;
    const char*                 actionMsg;
} adi_fpgagen6_ErrorInfo_t;

/*
*  \brief ADI FPGAGEN6 Error Action Union
*/
typedef union
{
    adi_fpgagen6_ErrAction_e    deviceAction;   /* FPGAGEN6 Error Action Type */
    adi_common_ErrAction_t      common;         /* Common Error Action Union */
} adi_fpgagen6_ErrAction_t;

/* Error Table for API to relate FPGA Error Codes to Strings/Recovery Actions */
typedef adi_fpgagen6_ErrTableRow_t      fpga_ErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_AxiAdvSpiErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_DataInterfaceErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_PrbsErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_RxErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_TddErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_AxiAds10v1ErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_UtilityErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_CoreErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_ClkErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_GpioErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_HalErrCodes_t;

typedef adi_fpgagen6_ErrTableRow_t      fpgagen6_TxErrCodes_t;

#endif  /* _ADI_FPGA_ERROR_TYPES_H_ */

