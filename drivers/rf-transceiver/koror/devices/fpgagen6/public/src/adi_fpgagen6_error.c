/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_error.c
*
* \brief Device Specific Abstractions for Common Error Definitions
*
* FPGA API Version: 2.9.0.4
*/
#include "adi_fpgagen6_error.h"

#define ADI_FILE    ADI_FPGAGEN6_FILE_PUBLIC_ERROR

/* Error Code Tables */

static const fpga_ErrCodes_t fpga_ErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_NO_ERROR,                                        ADI_STRING("No Error"),                                 ADI_FPGAGEN6_ERR_ACT_NONE },
    { ADI_FPGAGEN6_ERRCODE_REGISTER_IO,                                     ADI_STRING("FPGA Register IO Issue"),                   ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_AxiAdvSpiErrCodes_t fpgagen6_AxiAdvSpiErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_AXI_ADV_SPI_TEST,                                ADI_STRING("Test"),                                     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_DataInterfaceErrCodes_t fpgagen6_DataInterfaceErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER0,         ADI_STRING("Rx LaneXBar Link 0 Reset Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER1,         ADI_STRING("Rx LaneXBar Link 1 Reset Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_DEFRAMER2,         ADI_STRING("Rx LaneXBar Link 2 Reset Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER0,           ADI_STRING("Tx LaneXBar Link 0 Reset Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LANEXBAR_RESET_FRAMER1,           ADI_STRING("Tx LaneXBar Link 1 Reset Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_GET,              ADI_STRING("Rx LaneXBar Configuration Get Issue"),      ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_RX_LANEXBAR_CFG_SET,              ADI_STRING("Rx LaneXBar Configuration Set Issue"),      ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_GET,              ADI_STRING("Tx LaneXBar Configuration Get Issue"),      ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_TX_LANEXBAR_CFG_SET,              ADI_STRING("Tx LaneXBar Configuration Set Issue"),      ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_GET,                 ADI_STRING("Deframer Configuration Get Issue"),         ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_CFG_SET,                 ADI_STRING("Deframer Configuration Set Issue"),         ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_GET,                   ADI_STRING("Framer Configuration Get Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_CFG_SET,                   ADI_STRING("Framer Configuration Set Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_GET,               ADI_STRING("Serializer Configuration Get Issue"),       ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_CFG_SET,               ADI_STRING("Serializer Configuration Set Issue"),       ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_SET,             ADI_STRING("Serializer Reset Set Issue"),               ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SERIALIZER_RESET_DONE_GET,        ADI_STRING("Serializer Reset Done Get Issue"),          ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_GET,             ADI_STRING("Deserializer Configuration Get Issue"),     ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_CFG_SET,             ADI_STRING("Deserializer Configuration Set Issue"),     ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_SET,           ADI_STRING("Deserializer Reset Set Issue"),             ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DESERIALIZER_RESET_DONE_GET,      ADI_STRING("Deserializer Reset Done Get Issue"),        ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_SET,               ADI_STRING("Deframer Reset Set Issue"),                 ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_RELEASE,           ADI_STRING("Deframer Reset Release Issue"),             ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DEFRAMER_RESET_POLL,              ADI_STRING("Deframer Reset Poll Issue"),                ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_SET,                 ADI_STRING("Framer Reset Set Issue"),                   ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_RELEASE,             ADI_STRING("Framer Reset Release Issue"),               ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_FRAMER_RESET_POLL,                ADI_STRING("Framer Reset Poll Get Issue"),              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_LINK_STATUS_GET,                  ADI_STRING("Link Status Get Issue"),                    ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_SET,                      ADI_STRING("Sync Bar Set Issue"),                       ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYNCBAR_GET,                      ADI_STRING("Sync Bar Get Issue"),                       ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_DATALINK_CFG_SET,                 ADI_STRING("Data Link Set Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_SET,               ADI_STRING("SysRef Control Set Issue"),                 ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_DATA_INTERFACE_SYSREF_CONTROL_GET,               ADI_STRING("SysRef Control Get Issue"),                 ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE }
};

static const fpgagen6_PrbsErrCodes_t fpgagen6_PrbsErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ENABLE,                          ADI_STRING("PRBS Serializer Set Issue"),                ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_PRBS_DESERIALIZER_ENABLE,                        ADI_STRING("PRBS Deserializer Set Issue"),              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_PRBS_SERIALIZER_ERROR_INJECT,                    ADI_STRING("PRBS Serializer Error Inject Issue"),       ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_COUNT_GET,                            ADI_STRING("PRBS Error Count Get Issue"),               ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_PRBS_ERROR_CLEAR,                                ADI_STRING("PRBS Error Clear Issue"),                   ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE }
};

static const fpgagen6_RxErrCodes_t fpgagen6_RxErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_RX_TRANSPORT_CFG_SET,            ADI_STRING("Rx Transport Cfg Set Issue"),                           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_TRANSPORT_CFG_GET,            ADI_STRING("Rx Transport Cfg Get Issue"),                           ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_START,                ADI_STRING("Rx Capture Start Issue"),                               ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_CAPTURE_WAIT,                 ADI_STRING("Rx Capture Wait Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_RESET,              ADI_STRING("Rx Datachain Reset Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_DMA_HALT_TIMEOUT,             ADI_STRING("Rx DMA Halt Timeout Issue"),                            ADI_FPGAGEN6_ERR_ACT_RESET_DEVICE },
    { ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_SET,                  ADI_STRING("Rx DMA Config Set Issue"),                              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_DMA_CFG_GET,                  ADI_STRING("Rx DMA Config Get Issue"),                              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_RX_DATACHAIN_CFG,                ADI_STRING("Rx Datachain Configure Issue"),                         ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE }
};

static const fpgagen6_TxErrCodes_t fpgagen6_TxErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_TX_DATA_START,                   ADI_STRING("Tx Capture Start Issue"),                               ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_TX_DATA_STOP,                    ADI_STRING("Tx Capture Wait Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_RESET,              ADI_STRING("Tx Datachain Reset Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_TX_DMA_HALT_TIMEOUT,             ADI_STRING("Tx DMA Halt Timeout Issue"),                            ADI_FPGAGEN6_ERR_ACT_RESET_DEVICE },
    { ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_SET,                  ADI_STRING("Tx DMA Config Set Issue"),                              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_TX_DMA_CFG_GET,                  ADI_STRING("Tx DMA Config Get Issue"),                              ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
    { ADI_FPGAGEN6_ERRCODE_TX_DATACHAIN_CFG,                ADI_STRING("Tx Datachain Configure Issue"),                         ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE },
};

static const fpgagen6_TddErrCodes_t fpgagen6_TddErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_SET,           ADI_STRING("TRX Manual Enable Configuration Issue"),                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_SET,           ADI_STRING("ORX Manual Enable Configuration Issue"),                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_SET,          ADI_STRING("GPIO Manual Enable Configuration Issue"),               ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_SET,           ADI_STRING("External Manual Enable Configuration Issue"),           ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_TRX_GET,           ADI_STRING("TRX Manual Enable Configuration Fetch Issue"),          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_ORX_GET,           ADI_STRING("ORX Manual Enable Configuration Fetch Issue"),          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_GPIO_GET,          ADI_STRING("GPIO Manual Enable Configuration Fetch Issue"),         ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_MANUAL_EN_EXT_GET,           ADI_STRING("External Manual Enable Configuration Fetch Issue"),     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_FRAME_LOOP,                  ADI_STRING("TDD Frame Looping Disable Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_FRAME_COUNT,                 ADI_STRING("TDD Frame Count Reset Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_STATE_MACHINE_DISABLE,       ADI_STRING("External Manual Enable Configuration Fetch Issue"),     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_TDD_ENABLE_CTRL_DISABLE,         ADI_STRING("TDD Enable Control Disable Issue"),                     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_AxiAds10v1ErrCodes_t fpgagen6_AxiAds10v1ErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_GET,     ADI_STRING("SMA Pin Configuration Get Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_SMA_PIN_CFG_SET,     ADI_STRING("SMA Pin Configuration Set Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_GLOBAL_RESET,        ADI_STRING("ADS10 Global Reset Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_GET,         ADI_STRING("LED Configuration Get Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_AXI_ADS10V1_LED_CFG_SET,         ADI_STRING("LED Configuration Set Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_UtilityErrCodes_t fpgagen6_UtilityErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_MMCM_CONFIGURE_LOCK,   ADI_STRING("MMCM Configure & Lock Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_DESERIALIZER_CFG,      ADI_STRING("Deserializer Config Issue"),                            ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_SERIALIZER_CFG,        ADI_STRING("Serializer Config Issue"),                              ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_LANEXBAR_RESET_CFG,    ADI_STRING("LaneXBar Reset Config Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_DEFRAMER_CFG,          ADI_STRING("Deframer Config Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_FRAMER_CFG,            ADI_STRING("Framer Config Issue"),                                  ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_CFG,              ADI_STRING("Init Config Issue"),                                    ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_UTILITIES_INIT_SET,              ADI_STRING("Init Set Issue"),                                       ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_CoreErrCodes_t fpgagen6_CoreErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE_SETUP,      ADI_STRING("DRP Field Write Setup Issue"),                          ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_WRITE,            ADI_STRING("DRP Field Write Issue"),                                ADI_FPGAGEN6_ERR_ACT_RESET_INTERFACE },
    { ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ_SETUP,       ADI_STRING("DRP Field Read Setup Issue"),                           ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CORE_DRP_FIELD_READ,             ADI_STRING("DRP Field Read Issue"),                                 ADI_FPGAGEN6_ERR_ACT_RESET_INTERFACE }
};

static const fpgagen6_ClkErrCodes_t fpgagen6_ClkErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_SET,                ADI_STRING("MMCM Config Set Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_MMCM_CFG_GET,                ADI_STRING("MMCM Config Get Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_MMCM_RESET,                  ADI_STRING("MMCM Reset Issue"),                                     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_MMCM_LOCK_GET,               ADI_STRING("MMCM Lock Get Issue"),                                  ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_SET,                ADI_STRING("QPLL Config Set Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_QPLL_CFG_GET,                ADI_STRING("QPLL Config Get Issue"),                                ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_QPLL_LOCK_GET,               ADI_STRING("QPLL Lock Set Issue"),                                  ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_CLK_LANE_PWR_GOOD_GET,           ADI_STRING("Lane Power Good Get Issue"),                            ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_GpioErrCodes_t fpgagen6_GpioErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_GPIO_MODE_SET,                   ADI_STRING("GPIO Mode Set Issue"),                                  ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_INPUT_DIR_SET,              ADI_STRING("GPIO Input Direction Issue"),                           ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_SET,             ADI_STRING("GPIO Output Direction Set Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_OUTPUT_DIR_GET,             ADI_STRING("GPIO Output Direction Get Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_WRITE,                      ADI_STRING("GPIO Write Issue"),                                     ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_READ,                       ADI_STRING("GPIO Read Issue"),                                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_SET,                ADI_STRING("GPIO TDD Mux Set Issue"),                               ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_GET,                ADI_STRING("GPIO TDD Mux Get Issue"),                               ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_TDD_MUX_CLEAR,              ADI_STRING("GPIO TDD Mux Clear Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_SPI2_MODE_SET,              ADI_STRING("GPIO SPI2 Mode Set Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_JTAG_MODE_SET,              ADI_STRING("GPIO JTAG Mode Set Issue"),                             ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_SET,            ADI_STRING("GP Interrupt Config Set Issue"),                        ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_CFG_GET,            ADI_STRING("GP Interrupt Config Get Issue"),                        ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_GET,         ADI_STRING("GP Interrupt Status Get Issue"),                        ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_STATUS_CLEAR,       ADI_STRING("GP Interrupt Status Clear Issue"),                      ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_GPIO_GP_INTR_COUNTER_GET,        ADI_STRING("GP Interrupt Counter Get Issue"),                       ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};

static const fpgagen6_HalErrCodes_t fpgagen6_HalErrTable[] =
{
    { ADI_FPGAGEN6_ERRCODE_HAL_RAM_READ,                    ADI_STRING("HAL RAM Read Issue"),                                   ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_HAL_RAM_WRITE,                   ADI_STRING("HAL RAM Write Issue"),                                  ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_READ,              ADI_STRING("Register Field Read Issue"),                            ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM },
    { ADI_FPGAGEN6_ERRCODE_HAL_REG_FIELD_WRITE,             ADI_STRING("Register Field Write Issue"),                           ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM }
};


/*****************************************************************************/
/***** Local functions' prototypes *******************************************/
/*****************************************************************************/
static adi_fpgagen6_ErrAction_e fpgagen6_ErrLookup( const int64_t                   errCode,
                                                    const char** const              errMsgPtr,
                                                    adi_fpgagen6_ErrAction_e* const actionCodePtr);

/**
* \brief    Service to Lookup a specified Error Table
*
* \param[in]    errTable        Error Table to be Looked Up
* \param[in]    errTableSize    Error Table Size
* \param[in]    errCode         Error Code to be Looked Up
* \param[out]   errMsgPtr       Associated Error Message from Look Up Table
* \param[out]   errActionPtr    Associated Error Action from Look Up Table
*
* \retval adi_fpgagen6_ErrAction_e - ADI_FPGAGEN6_ERR_ACT_NONE if Successful
*/
static adi_fpgagen6_ErrAction_e fpgagen6_ErrTableLookUp(const adi_fpgagen6_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_fpgagen6_ErrAction_e* const     errActionPtr);

/*****************************************************************************/
/***** Local data types ******************************************************/
/*****************************************************************************/

/*****************************************************************************/
/***** Functions' definition *************************************************/
/*****************************************************************************/

static adi_fpgagen6_ErrAction_e fpgagen6_ErrTableLookUp(const adi_fpgagen6_ErrTableRow_t    errTable[],
                                                        const size_t                        errTableSize,
                                                        const uint32_t                      errCode,
                                                        const char** const                  errMsgPtr,
                                                        adi_fpgagen6_ErrAction_e* const     actionCodePtr)
{
        adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint32_t                    idx             = 0U;

    ADI_FPGAGEN6_NULL_PTR_RETURN(errMsgPtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(actionCodePtr);

    if (errTableSize >= sizeof(adi_fpgagen6_ErrTableRow_t))
    {
        for (idx = 0U; (idx < (errTableSize / sizeof(adi_fpgagen6_ErrTableRow_t))); ++idx)
        {
            if (errCode == errTable[idx].errCode)
            {
                *errMsgPtr      = errTable[idx].errMsg;
                *actionCodePtr  = errTable[idx].action;
                recoveryAction  = ADI_FPGAGEN6_ERR_ACT_NONE;
                break;
            }
        }
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_ErrInfoGet(   const adi_fpgagen6_ErrSource_e  errSrc,
                                                            const int64_t                   errCode,
                                                            const char** const              errMsgPtr,
                                                            adi_fpgagen6_ErrAction_e* const actionCodePtr)
{
    adi_fpgagen6_ErrAction_e recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    /* Validate Arguments */
    ADI_FPGAGEN6_NULL_PTR_RETURN(errMsgPtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(actionCodePtr);

    /* Default Values */
    *errMsgPtr      = NULL;
    *actionCodePtr  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;

    switch (errSrc)
    {
        case ADI_FPGAGEN6_ERRSRC_API:
        case ADI_FPGAGEN6_ERRSRC_DEVICEHAL:
        case ADI_FPGAGEN6_ERRSRC_DEVICEBF:
            /* Common Sources using Common Errors */
            recoveryAction = (adi_fpgagen6_ErrAction_e) adi_common_ErrorGet(errCode,
                                                                            errMsgPtr,
                                                                            (adi_common_ErrAction_e* const) actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_GEN:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrLookup(errCode, errMsgPtr, actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_CORE:
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_CoreErrTable,
                                                        sizeof(fpgagen6_CoreErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_AXI_ADV_SPI:
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_AxiAdvSpiErrTable,
                                                        sizeof(fpgagen6_AxiAdvSpiErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_UTILITIES:
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_UtilityErrTable,
                                                        sizeof(fpgagen6_UtilityErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_PRBS:
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_PrbsErrTable,
                                                        sizeof(fpgagen6_PrbsErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;
        case ADI_FPGAGEN6_ERRSRC_RX:
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_RxErrTable,
                                                        sizeof(fpgagen6_RxErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_TX:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_TxErrTable,
                                                        sizeof(fpgagen6_TxErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_DATA_INTERFACE:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_DataInterfaceErrTable,
                                                        sizeof(fpgagen6_DataInterfaceErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_TDD:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_TddErrTable,
                                                        sizeof(fpgagen6_TddErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_AXI_ADS10V1:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_AxiAds10v1ErrTable,
                                                        sizeof(fpgagen6_AxiAds10v1ErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_GPIO:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_GpioErrTable,
                                                        sizeof(fpgagen6_GpioErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_CLK:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_ClkErrTable,
                                                        sizeof(fpgagen6_ClkErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_HAL:
            /* Fall Through */
            recoveryAction = fpgagen6_ErrTableLookUp(   fpgagen6_HalErrTable,
                                                        sizeof(fpgagen6_HalErrTable),
                                                        errCode,
                                                        errMsgPtr,
                                                        actionCodePtr);
            break;

        case ADI_FPGAGEN6_ERRSRC_NONE:
        case ADI_FPGAGEN6_ERRSRC_UNKNOWN:
            /* Fall Through */

        default:
            /* Invalid Error Source; No Lookup can be performed */
            recoveryAction = ADI_FPGAGEN6_ERR_ACT_CHECK_FEATURE;
            break;
    }

    return recoveryAction;
}

ADI_API adi_fpgagen6_ErrAction_e adi_fpgagen6_ErrDataGet(   const adi_fpgagen6_Device_t* const  device,
                                                            const adi_common_ErrFrameId_e       frameId,
                                                            adi_fpgagen6_ErrSource_e* const     errSrcPtr,
                                                            int64_t* const                      errCodePtr,
                                                            const char** const                  errMsgPtr,
                                                            adi_fpgagen6_ErrAction_e* const     actionPtr)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    adi_common_ErrFrame_t       errFrame        = { ADI_COMMON_ERR_ACT_NONE, { 0, NULL, 0U }, { 0U, NULL, 0U }, { NULL, 0 } };

    /* Validate Arguments */
    ADI_FPGAGEN6_NULL_DEVICE_PTR_RETURN(device);

    ADI_FPGAGEN6_NULL_PTR_RETURN(errSrcPtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(errCodePtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(errMsgPtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(actionPtr);

    /* Parse Error Frame */
    recoveryAction = (adi_fpgagen6_ErrAction_e) ADI_ERROR_FRAME_GET(&device->common, frameId, &errFrame);
    if (ADI_FPGAGEN6_ERR_ACT_NONE == recoveryAction)
    {
        /* Convert to fpga Error Source */
        *errSrcPtr      = (adi_fpgagen6_ErrSource_e) errFrame.errInfo.errSrc;
        *errCodePtr     = errFrame.errInfo.errCode;
        recoveryAction  = adi_fpgagen6_ErrInfoGet(*errSrcPtr, *errCodePtr, errMsgPtr, actionPtr);
    }

    return recoveryAction;
}

static adi_fpgagen6_ErrAction_e fpgagen6_ErrLookup( const int64_t                   errCode,
                                                    const char** const              errMsgPtr,
                                                    adi_fpgagen6_ErrAction_e* const actionPtr)
{
    adi_fpgagen6_ErrAction_e    recoveryAction  = ADI_FPGAGEN6_ERR_ACT_CHECK_PARAM;
    uint8_t                     idx             = 0U;

    ADI_FPGAGEN6_NULL_PTR_RETURN(actionPtr);

    ADI_FPGAGEN6_NULL_PTR_RETURN(errMsgPtr);

    for (idx = 0U; (idx < (sizeof(fpga_ErrTable) / sizeof(fpga_ErrCodes_t))); ++idx)
    {
        if (errCode == fpga_ErrTable[idx].errCode)
        {
            *errMsgPtr      = fpga_ErrTable[idx].errMsg;
            *actionPtr      = fpga_ErrTable[idx].action;
            recoveryAction  = ADI_FPGAGEN6_ERR_ACT_NONE;
            break;
        }
    }

    return recoveryAction;
}
