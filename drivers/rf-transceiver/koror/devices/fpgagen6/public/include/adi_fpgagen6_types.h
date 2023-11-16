/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_fpgagen6_types.h
* \brief Contains FPGAGEN6 API configuration and run-time type definitions
*
* FPGA API Version: 2.9.0.4
*/

#ifndef _ADI_FPGAGEN6_TYPES_H_
#define _ADI_FPGAGEN6_TYPES_H_

#include "adi_common.h"
#include "adi_fpgagen6_data_interface_types.h"
#include "adi_fpgagen6_rx_types.h"
#include "adi_fpgagen6_tx_types.h"
#include "adi_fpgagen6_gpio_types.h"
#include "adi_fpgagen6_prbs_types.h"
#include "adi_fpgagen6_clk_types.h"

typedef uint32_t adi_fpgagen6_DmaMask_t;

#define ADI_FPGAGEN6_DRP_REGISTER_WIDTH             16U
#define ADI_FPGAGEN6_DRP_TIMEOUT_MS                 200U
#define ADI_FPGAGEN6_DRP_READY_DATA_ADDRESS_START   0x4101014CU
#define ADI_FPGAGEN6_DRP_READY_DATA_ADDRESS_STRIDE  0x00000100U
#define ADI_FPGAGEN6_DRP_READY_START_BIT            16U
#define ADI_FPGAGEN6_DRP_ERROR_START_BIT            17U
#define ADI_FPGAGEN6_DRP_DATA_MASK                  (uint16_t)0xFFFFU
#define ADI_FPGAGEN6_HBM_PAGE_SIZE                  (0x40000000U)

#define ADI_FPGAGEN6_DEFAULT_REF_CLK_KHZ            491520U
#define ADI_FPGAGEN6_DEFAULT_LANE_RATE_KHZ          32440320U

/**
*  \brief Valid DRP Interfaces
*/
typedef enum adi_fpgagen6_DrpInterface
{
    ADI_FPGAGEN6_DRP_INTERFACE_CH0 = 0U,
    ADI_FPGAGEN6_DRP_INTERFACE_CH1,
    ADI_FPGAGEN6_DRP_INTERFACE_CH2,
    ADI_FPGAGEN6_DRP_INTERFACE_CH3,
    ADI_FPGAGEN6_DRP_INTERFACE_CH4,
    ADI_FPGAGEN6_DRP_INTERFACE_CH5,
    ADI_FPGAGEN6_DRP_INTERFACE_CH6,
    ADI_FPGAGEN6_DRP_INTERFACE_CH7,
    ADI_FPGAGEN6_DRP_INTERFACE_QUAD0,
    ADI_FPGAGEN6_DRP_INTERFACE_QUAD1
} adi_fpgagen6_DrpInterface_e;

/**
* \brief Enumerated list of DRP mask values
*/
typedef enum adi_fpgagen6_DrpMask
{
    ADI_FPGAGEN6_DRP_MASK_TXCLK25DIV        = 0xF800U,
    ADI_FPGAGEN6_DRP_MASK_TXPHDLY_CFG1      = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_TXPI_CFG3         = 0x0040U,
    ADI_FPGAGEN6_DRP_MASK_TX_PROGDIV_CFG    = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_RXCLK25DIV        = 0x00F8U,
    ADI_FPGAGEN6_DRP_MASK_PMA_RSV1          = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_RXCDR_CFG2        = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_RXDFE_GC_CFG1     = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_RXPI_CFG4         = 0x0200U,
    ADI_FPGAGEN6_DRP_MASK_RXPI_CFG5         = 0x0100U,
    ADI_FPGAGEN6_DRP_MASK_RX_EN_HI_LR       = 0x0400U,
    ADI_FPGAGEN6_DRP_MASK_RX_WIDEMODE_CDR   = 0x0004U,
    ADI_FPGAGEN6_DRP_MASK_RX_PROGDIV_CFG    = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_N            = 0x00FFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_M            = 0x0F80U,
    ADI_FPGAGEN6_DRP_MASK_QPLL_LPF          = 0x03FFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_CFG4         = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_CP           = 0x03FFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_FBDIV        = 0x00FFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_LOCK_CFG     = 0xFFFFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_LOCK_CFG_G3  = 0x03FFU,
    ADI_FPGAGEN6_DRP_MASK_QPLL_REFCLK_DIV   = 0x0F80U,
    ADI_FPGAGEN6_DRP_MASK_QPLL_CLKOUT_RATE  = 0x0001U,
    ADI_FPGAGEN6_DRP_MASK_TXGEARBOX_EN      = 0x2000U,
    ADI_FPGAGEN6_DRP_MASK_TXBUF_EN          = 0x0080U
} adi_fpgagen6_DrpMask_e;

/**
* \brief Enumerated list of DRP mask values
*/
typedef enum adi_fpgagen6_DrpAddress
{
    ADI_FPGAGEN6_DRP_ADDRESS_DRP_TXCLK25DIV         = 0x007AU,
    ADI_FPGAGEN6_DRP_ADDRESS_TXPHDLY_CFG1           = 0x006FU,
    ADI_FPGAGEN6_DRP_ADDRESS_TXPI_CFG3              = 0x009CU,
    ADI_FPGAGEN6_DRP_ADDRESS_TX_PROGDIV_CFG         = 0x0057U,
    ADI_FPGAGEN6_DRP_ADDRESS_RXCLK25DIV             = 0x006DU,
    ADI_FPGAGEN6_DRP_ADDRESS_PMA_RSV1               = 0x0095U,
    ADI_FPGAGEN6_DRP_ADDRESS_RXCDR_CFG2             = 0x0010U,
    ADI_FPGAGEN6_DRP_ADDRESS_RXDFE_GC_CFG1          = 0x00CCU,
    ADI_FPGAGEN6_DRP_ADDRESS_RXPI_CFG4              = 0x009DU,
    ADI_FPGAGEN6_DRP_ADDRESS_RXPI_CFG5              = 0x009DU,
    ADI_FPGAGEN6_DRP_ADDRESS_RX_EN_HI_LR            = 0x0052U,
    ADI_FPGAGEN6_DRP_ADDRESS_RX_WIDEMODE_CDR        = 0x0066U,
    ADI_FPGAGEN6_DRP_ADDRESS_RX_PROGDIV_CFG         = 0x00C6U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_N             = 0x0014U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_M             = 0x0018U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_LPF           = 0x0019U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_CFG4          = 0x0030U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_CP            = 0x0016U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_FBDIV         = 0x0014U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_LOCK_CFG      = 0x0012U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_LOCK_CFG_G3   = 0x001DU,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_REFCLK_DIV    = 0x0018U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK0_CLKOUT_RATE   = 0x000EU,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_N             = 0x0094U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_M             = 0x0098U,
    ADI_FPGAGEN6_DRP_ADDRESS_QPLLCLK1_CLKOUT_RATE   = 0x008EU,
    ADI_FPGAGEN6_DRP_ADDRESS_TXPRBS_SETTINGS        = 0x007CU
} adi_fpgagen6_DrpAddress_e;

/**
* \brief The data path includes the serializer/deserializer, framers/deframers, transport, tollgate and DMA
*/
typedef struct adi_fpgagen6_DatapathCfg
{
    adi_fpgagen6_SerializerCfg_t    serCfg[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];                /*!< Holds configuration for the tx GTH serializer 1..8 */
    adi_fpgagen6_DeserializerCfg_t  desCfg[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];                /*!< Holds configuration for the rx GTH deserializer 1..8 */
    adi_fpgagen6_FramerCfg_t        framerCfg[ADI_FPGAGEN6_MAX_FRAMERS];                    /*!< Holds configuration for the tx framers 1,2 */
    adi_fpgagen6_DeframerCfg_t      deframerCfg[ADI_FPGAGEN6_MAX_DEFRAMERS];                /*!< Holds configuration for the Rx deframers 1,2,3 */
    adi_fpgagen6_RxDmaCfg_t        rxDmaCfg[ADI_FPGAGEN6_RX_DMA_COUNT];                     /*!< Holds configuration for the rx digital data paths 1..16 */
    adi_fpgagen6_TxDmaCfg_t        txDmaCfg[ADI_FPGAGEN6_TX_DMA_COUNT];                     /*!< Holds configuration for the tx digital data paths 1..8*/
    adi_fpgagen6_SyncXBar_t         syncXBarCfg;                                            /*!< Holds configuration for SyncBar */
    adi_fpgagen6_RxTransportCfg_t   rxTransport[ADI_FPGAGEN6_RX_TRANSPORT_COUNT]; /*!< Holds configuration for transport layer */
    uint8_t                         pcbSerLaneSwap[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];        /*!< Indices correspond to serializer lane numbers. Values are lane number after applying PCB swap */
    uint8_t                         pcbDesLaneSwap[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];        /*!< Indices correspond to deserializer lane numbers. Values are lane number after applying PCB swap */
    uint8_t                         desLanePnInvert[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];       /*!< Indices correspond to serializer lane numbers. Values are lane number after applying PCB swap */
    uint8_t                         serLanePnInvert[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];       /*!< Indices correspond to deserializer lane numbers. Values are lane number after applying PCB swap */
} adi_fpgagen6_DatapathCfg_t;


/**
* \brief The data path includes the serializer/deserializer, framers/deframers, transport, tollgate and DMA
*/
typedef struct adi_fpgagen6_ClocksCfg
{
    adi_fpgagen6_QpllCfg_t  qpll;                /*!< Holds the init settings for the Qpll clock setup */
    adi_fpgagen6_MmcmCfg_t  coreMmcm;            /*!< Holds the init settings for the Link Layer MMCM clock wizard */
    adi_fpgagen6_MmcmCfg_t  transportMmcm;       /*!< Holds the init settings for the Transport MMCM clock wizard */
} adi_fpgagen6_ClocksCfg_t;


typedef struct adi_fpgagen6_Init
{
    adi_fpgagen6_DatapathCfg_t      datapath;   /*!< Holds the Datapath / JESD204B/C data link and digital datapath settings */
    adi_fpgagen6_SysRefClkCfg_t     sysref;     /*!< Holds the data for the external or internal Sysref */
    adi_fpgagen6_ClocksCfg_t        clocks;     /*!< Holds the settings for the clock configurations */
} adi_fpgagen6_Init_t;

typedef struct adi_fpgagen6_HwOpenCfg
{
    uint32_t            memoryPageSize; /*!< The current page size used when reading/writing RAM */
} adi_fpgagen6_HwOpenCfg_t;

#ifndef CLIENT_IGNORE

typedef struct adi_fpgagen6_Info
{
    uint16_t                        versionMinor;                                                   /*!< FPGA minor version read during FPGA_initialize */
    uint8_t                         versionMajor;                                                   /*!< FPGA major version read during FPGA_initialize */
    uint32_t                        swTest;                                                         /*!< Software testmode signal */
    adi_fpgagen6_InterfaceType_e    interfaceType;                                                  /*!< Data Interface Type */
    uint8_t                         testLoopbackFlag;                                               /*!< Loopback Mode */
    uint32_t                        linkLayerDataWidth;                                             /*!< 40 (bits per lane) for JESD204B; 66 for JESD204C */
    uint32_t                        encodingLowValue;                                               /*!< 8 (from 8b10b) for JESD204B; 64 for JESD204C */
    uint32_t                        encodingHighValue;                                              /*!< 10 (from 8b10b) for JESD204B; 66 for JESD204C */
    uint32_t                        dmaDataWidth;                                                   /*!< Num of bits per word written to RAM from DMA: 32 bits for JESD204B; 64 bits for JESD204C */
    uint32_t                        systemCTxChannelMask;                                           /*!< Need to keep track of channels Tx channels across multiple API calls for SystemC & performTx */
    uint32_t                        systemCRxChannelMask;                                           /*!< Need to keep track of channels Rx channels across multiple API calls for SystemC & performRx */
    adi_fpgagen6_GpioModes_e        gpioPinMode[ADI_FPGAGEN6_GPIO_COUNT];                           /*!< The current mode each GPIO pin is assigned */
    adi_fpgagen6_PrbsStoredData_t   serializerPrbsStoredData[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];      /*!< Keeps the settings that were required to be overwritten in order to enable PRBS so they can be restored later */
    adi_fpgagen6_PrbsStoredData_t   deserializerPrbsStoredData[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];    /*!< Keeps the settings that were required to be overwritten in order to enable PRBS so they can be restored later */
    uint32_t                        outputClkFreq_kHz[ADI_FPGAGEN6_NUM_MMCM_OUTPUT_CLOCKS];         /*! Desired Channel frequencies. */
    uint8_t                         rxDmaInUse[ADI_FPGAGEN6_RX_DMA_COUNT];                          /*! The link number assignment: 0 - Link 0 NLS; 1 - Link 0 LS; 2 - Link 1 NLS; 3 - Link 1 LS, 4 - Link 2 NLS . */
} adi_fpgagen6_Info_t;

/**
* \brief Data structure to hold FPGA device instance settings
*/
typedef struct adi_fpgagen6_Device
{
    adi_common_Device_t common;             /*!< Common layer structure */
    adi_fpgagen6_Info_t devStateInfo;       /*!< FPGA run time state information container */
    uint32_t            memoryPageSize;     /*!< The current page size used when reading/writing RAM */
} adi_fpgagen6_Device_t;

#endif // !CLIENT_IGNORE

#endif
