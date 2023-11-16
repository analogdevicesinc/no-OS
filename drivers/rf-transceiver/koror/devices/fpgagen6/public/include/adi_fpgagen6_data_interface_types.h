/**
* Copyright 2015 - 2021 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
 * \file adi_fpgagen6_data_interface_types.h
 * \brief Contains FPGA data interface enum and struct definitions
 *
 * FPGA API Version: 2.9.0.4
 */

#ifndef _ADI_FPGAGEN6_DATA_INTERFACE_TYPES_H_
#define _ADI_FPGAGEN6_DATA_INTERFACE_TYPES_H_

#include "adi_library_types.h"

#define ADI_FPGAGEN6_MAX_FRAMERS                    2U      /*!< FPGA Number of Framers */
#define ADI_FPGAGEN6_MAX_DEFRAMERS                  3U      /*!< FPGA Number of Deframers */
#define ADI_FPGAGEN6_MAX_DEFRAMERS_LS               5U      /*!< FPGA Number of possible deframer configurations including link sharing */
#define ADI_FPGAGEN6_TOT_FRAMERS_DEFRAMERS          (ADI_FPGAGEN6_MAX_FRAMERS + ADI_FPGAGEN6_MAX_DEFRAMERS) /*!< FPGA Total Number of Data Interfaces */

#define ADI_FPGAGEN6_LANES_PHYSICAL_NUM             8U      /*!< FPGA Maximum Physical Number of Lanes per Deframer/Framer */
#define ADI_FPGAGEN6_LANES_LOOPBACK_NUM             8U      /*!< FPGA Maximum Loopback Number of Lanes per Deframer/Framer */

#define ADI_FPGAGEN6_LANES_DEFRAMER0_NUM            8U      /*!< FPGA Deframer 0 to Device Framer 0 */
#define ADI_FPGAGEN6_LANES_DEFRAMER1_NUM            4U      /*!< FPGA Deframer 1 to Device Framer 1 */
#define ADI_FPGAGEN6_LANES_DEFRAMER2_NUM            2U      /*!< FPGA Deframer 2 to Device Framer 2 */
#define ADI_FPGAGEN6_LANES_DEFRAMER_MAX             ADI_FPGAGEN6_LANES_DEFRAMER0_NUM

#define ADI_FPGAGEN6_INTERFACE_TYPES_NUM            2U      /*!< Total Number of Interface Types */

#define ADI_FPGAGEN6_JESD204B_IDX                   0U      /*!< Array Index for JESD204B */
#define ADI_FPGAGEN6_JESD204C_IDX                   1U      /*!< Array Index for JESD204C */

#define ADI_FPGAGEN6_DEFRAMER0_IDX                  0U      /*!< Array Index for Deframer0 Data */
#define ADI_FPGAGEN6_DEFRAMER1_IDX                  1U      /*!< Array Index for Deframer1 Data */
#define ADI_FPGAGEN6_DEFRAMER2_IDX                  2U      /*!< Array Index for Deframer2 Data */

#define ADI_FPGAGEN6_FRAMER0_IDX                    0U      /*!< Array Index for Framer0 Data */
#define ADI_FPGAGEN6_FRAMER1_IDX                    1U      /*!< Array Index for Framer1 Data */

#define ADI_FPGAGEN6_CLKPMA                         2U /*< Value to select the CLK PCS for Tx & Rx Out Clk Sel bitfield*/

/* Deframer & Framer Reset Poll Settings */
#define ADI_FPGAGEN6_CORE_RESET_10MS                10U
#define ADI_FPGAGEN6_RESET_FIXED_LOOP               5U
#define ADI_FPGAGEN6_RESET_POLL_STEP                10U

#define ADI_FPGAGEN6_LINK_LANE_UNUSED               0x1FU

#define ADI_FPGAGEN6_16_GHZ                         16220160

#define ADI_FPGAGEN6_LINK_STATUS_BIT                0U
#define ADI_FPGAGEN6_BUFFER_OVERFLOW_STATUS_BIT     1U

#define   FPGA_VERSION_ADDR                               0x43000000

/**
*  \brief FPGAGEN6 Framer Select
*/
typedef enum adi_fpgagen6_FramerSel
{
    ADI_FPGAGEN6_FRAMER_NONE    = 0x00U,                                            /*!< No Framer Selected */
    ADI_FPGAGEN6_FRAMER_0       = 0x01U,                                            /*!< Deframer 0 selection */
    ADI_FPGAGEN6_FRAMER_1       = 0x02U,                                            /*!< Deframer 1 selection */
    ADI_FPGAGEN6_FRAMER_ALL     = (ADI_FPGAGEN6_FRAMER_0 | ADI_FPGAGEN6_FRAMER_1)   /*!< All Framers selected */
} adi_fpgagen6_FramerSel_e;

/**
*  \brief FPGAGEN6 Deframer Select
*/
typedef enum adi_fpgagen6_DeframerSel
{
    ADI_FPGAGEN6_DEFRAMER_NONE  = 0x00U,                                                                            /*< No Deframer Selected */
    ADI_FPGAGEN6_DEFRAMER_0     = 0x01U,                                                                            /*!< Framer 0 selected */
    ADI_FPGAGEN6_DEFRAMER_1     = 0x02U,                                                                            /*!< Framer 1 selected */
    ADI_FPGAGEN6_DEFRAMER_2     = 0x04U,                                                                            /*!< Framer 2 selected */
    ADI_FPGAGEN6_DEFRAMER_ALL   = (ADI_FPGAGEN6_DEFRAMER_0 | ADI_FPGAGEN6_DEFRAMER_1 | ADI_FPGAGEN6_DEFRAMER_2)     /*!< All Deframers  selected */
} adi_fpgagen6_DeframerSel_e;

/**
*  \brief FPGAGEN6 Data Interface Type
*/
typedef enum adi_fpgagen6_InterfaceType
{
    ADI_FPGAGEN6_INTERFACE_NOT_SET  = 0U,
    ADI_FPGAGEN6_INTERFACE_JESD204B,
    ADI_FPGAGEN6_INTERFACE_JESD204C,
    ADI_FPGAGEN6_INTERFACE_UNKNOWN
} adi_fpgagen6_InterfaceType_e;

/**
* \brief Enumerated list of clk sources available to the Serializer/deserializer 
*/
typedef enum adi_fpgagen6_LaneClkSel
{
    ADI_FPGAGEN6_CPLL     = 0x0U,   /*!< Serializer Path */
    ADI_FPGAGEN6_QPLLCLK0 = 0x3U,   /*!< Deserializer Path */
    ADI_FPGAGEN6_QPLLCLK1 = 0x2U    /*!< Deserializer Path */
} adi_fpgagen6_LaneClkSel_e;

/**
*  \brief Data structure to hold the common DMA fields
*/
typedef struct adi_fpgagen6_DmaCfg
{    
    /* Configuration settings. Must be set for CfgSet functions */
    uint64_t startingAddress;           /*!< The absolute memory address of where this DMA will start writing/reading in the next transaction */
    uint8_t qThreshold;                 /*!< DMA Transaction Queue Threshold. This bit may be set to the desired outstanding transactions in the data mover queue. This allows multiple transactions posted in the AXI bus. The number of outstanding transactions is value + 1 for up to 8.*/
    uint8_t alignToSof;                 /*!< Align to Start of Frame. Allows DMA to align the captured data to the first valid data after the Start of Frame is asserted.*/
    uint32_t transactionSize;           /*!< DMA Transaction Size. Limits the transaction size of the DMA to this value and split up the transaction to satisfy the LENGTH above. This register must be set to a value less than or equal to the LENGTH. */
    uint8_t arbitrationSelect;          /*!< DMA Arbitration Select. This allows arbitration of transactions among multiple DMA cores. See FPGA register map for more info. */
    uint32_t dataMaskSelect;            /*!< Data Mask Select. The incoming data stream can be masked based on one of 32 signals such as TDD enables. */
    
    /* Below are status bits */
    uint8_t busy;                       /*!< Read Only: When this bit is 1, the DMA is actively transferring data.*/
    uint8_t complete;                   /*!< Read Only: When this bit is 1, the transfer has completed successfully.  This bit will clear when run_stop is cleared to '0'. */

} adi_fpgagen6_DmaCfg_t;

/**
* \brief SerDes Common TX & RX Configuration
*/
typedef struct adi_fpgagen6_SerDesCfg
{
    uint32_t                    laneRate_kHz;           /*!< Lanerate for this channel. Used for DRP functions */
    adi_fpgagen6_LaneClkSel_e   pllClkSel;              /*!< Selects the PLL to drive the transmitter data path. 00: CPLL; 01: reserved; 10: QPLL1; 11: QPLL0 */
    uint8_t                     encoder8b10bEnable;     /*!< Set this bit to a one to enable the 8b10b encoder in the Channel transmiter - this is needed for JESD204B operation. Clear this bit to bypass the 8b10b encoder - this is needed for JESD204C operation. */
    uint8_t                     polarity;               /*!< Controls a transmit channel's polarity. When set to 1, the corresponding transmit channel is inverted. When cleared to 0, the channel is not inverted. */
    uint8_t                     powerDown;              /*!< 00: P0 (normal operation) 01: P0s (low recovery time power down) 10: P1 (longer recovery time; Receiver Detection still on) 11: P2 (lowest power state) Attributes can control the transition times between these power-down states. */
} adi_fpgagen6_SerDesCfg_t;

/**
* \brief Enumerated list of Gth Serializer/deserializer each is identified by a separate bit
*/
typedef struct adi_fpgagen6_SerializerCfg
{
    adi_fpgagen6_SerDesCfg_t commonCfg; /*!< Common Serializer Rx/Tx Settings */
    uint8_t txOutClkSel;                /*!< Selects the source of the output clock for the transmit path. 000: reserved; 001: TXOUTCLKPCS; 010: TXOUTCLKPMA; 011: TXPLLREFCLK_DIV1; 100: TXPLLREFCLK_DIV2; 101: TXPROGDIVCLK; */
    uint8_t precursor;                  /*!< Transmitter pre-cursor TX pre-emphasis control. See FPGA documentation for more information and valid values. */
    uint8_t postcursor;                 /*!< Transmitter post-cursor TX pre-emphasis control. See FPGA documentation for more information and valid values. */
    uint8_t diffCtrl;                   /*!< Driver Swing/ Amplitude Control. See FPGA documentation for more information and valid values. */
} adi_fpgagen6_SerializerCfg_t;

/**
* \brief 
*/
typedef struct adi_fpgagen6_DeserializerCfg
{
    adi_fpgagen6_SerDesCfg_t commonCfg; /*!< Common Deserializer Rx/Tx Settings */
    uint8_t rxOutClkSel;                /*!< Selects the source of the output clock. 0x2 for 204B operation; 0x5 for 204C operation (default) */
    uint8_t rxDfeLpmEn;                 /*!< Controls a receive channel's rx_dfelpm_en signal. When set to 1, the LPM equalizer is enabled. When cleared to 0, the DFE equalizer is enabled.. */
    uint8_t cdrSetting;                 /*!< TBD */
} adi_fpgagen6_DeserializerCfg_t;

/**
* \brief Deframer Configuration
*/
typedef struct adi_fpgagen6_DeframerCfg
{
    uint8_t                 laneXBar[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];     /*!< Indices correspond to the XBar output. Array values correspond to the input selected */
    uint8_t                 sysRefDelay;                                    /*!< Add additional delay to SYSREF re-alignment of LMFC counter 1111 = 15 core_clk cycles */
    uint8_t                 subclassV;                                      /*!< JESD204b subclass: Subclass 2 = 0x10 Subclass 1 = 0x01 Subclass 0 = 0x00. JESD204c subclass: 0 = Subclass 0; 1 = Subclass 1 */
    uint8_t                 lanesInUse;                                     /*!< Each bit corresponds to a single lane, when set to 1 lane is active. Due to Lane XBars will always fill from bit 0 */
    uint16_t                ctrlRxBufAdv;                                   /*!< Advance the release of the receiver buffer by N 64-bit words */

    /* JESD204B */
    uint8_t                 bankId[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];       /*!< JESD204b Configuration Bank ID extension to Device ID. Read only, captured by ILA sequence */
    uint8_t                 laneId[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];       /*!< Read only, captured by ILA sequence  */
    uint8_t                 scramblingEn;                                  /*!< Set to 1 to enable scrambling. Clear to disable */
    uint16_t                ilaMultiFrames;                                /*!< Set to 1 to enable scrambling. Clear to disable */
    uint16_t                cfgF;                                          /*!< Octets per frame. Register programmed with value - 1 */
    uint8_t                 cfgK;                                          /*!< Frames per multiframe. Register programmed with value - 1 */
    uint8_t                 ilaNp[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];        /*!< N' (Totals bits per Sample) */
    uint8_t                 ilaN[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];         /*!< N Converter Resolution */
    uint8_t                 ilaS[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];         /*!< Number of samples per converter per frame. Read only, captured by ILA sequence. Register programmed with value - 1 */
    uint8_t                 ilaM[ADI_FPGAGEN6_LANES_DEFRAMER_MAX];         /*!< Number of converters per device. Read only, captured by ILA sequence. Register programmed with value - 1 */
    uint8_t                 errReportingEnable;                           /*!< Enable Error Reporting Using SYNC Interface */
    uint8_t                 ilaSupportEn;                                  /*!< ILA Support Enable */
    uint8_t                 sysRefAlways;                                  /*!< SYSREF Always 1 = Core re-aligns LMFC counter on all SYSREF events */
    uint8_t                 sysRefRequiredOnResync;                        /*!< 1 = A SYSREF event is required following a Link Re-Sync event */

    /* JESD204C */
    uint8_t                 enableCmdInterface;                            /*!< 1 = Enables AXI4-Stream Cmd interface; 0 = Cmd words will be zeroed */
    uint8_t                 enableDataInterface;                           /*!< 1 = Enables AXI4-Stream Data interface; 0 = Link will be transmitting scrambled zeros */
    uint8_t                 cfgE;                                          /*!< CTRL_MB_IN_EMB- Number of multi-blocks in an extended multi-block. Programmed with actual value (0 is invalid) */
    uint8_t                 ctrlMetaMode;                                  /*!< 0 = Cmd; 1 = CRC; 2 = FEC [TBD] */
    uint8_t                 ctrlRxMBlockTh;                                /*!< MB lock threshold */
    uint8_t                 laneBufferDelay; /*!< Calculated using the following formula: ((E-1)*32)+16 */
} adi_fpgagen6_DeframerCfg_t;

/**
* \brief Framer Configuration
*/
typedef struct adi_fpgagen6_FramerCfg
{
    uint8_t                 laneXBar[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];  /*!< Indices correspond to the XBar output. Array values correspond to the input selected */
    uint8_t                 sysRefDelay;                                /*!< Add additional delay to SYSREF re-alignment of LMFC counter 1111 = 15 core_clk cycles */
    uint8_t                 subclassV;                                  /*!< JESD204b subclass: Subclass 2 = 0x10 Subclass 1 = 0x01 Subclass 0 = 0x00. JESD204c subclass: 0 = Subclass 0; 1 = Subclass 1 */
    uint8_t                 lanesInUse;                                 /*!< Each bit corresponds to a single lane, when set to 1 lane is active. Due to Lane XBars will always fill from bit 0 */

    /* JESD204B */
    uint8_t                 deviceId;                                   /*!< JESD204b Configuration Device ID.  Sets value for all lanes */
    uint8_t                 bankId;                                     /*!< JESD204b Configuration Bank ID extension to Device ID. Sets value for all lanes */
    uint8_t                 laneId[ADI_FPGAGEN6_LANES_PHYSICAL_NUM];    /*!< JESD204b Configuration Lane ID per lane */
    uint8_t                 scramblingEn;                               /*!< Set to 1 to enable scrambling. Clear to disable */
    uint16_t                ilaMultiFrames;                             /*!< Multiframes in the Transmitted ILAS. Register programmed with value - 1 */
    uint16_t                cfgF;                                       /*!< Octets per frame. Register programmed with value - 1 */
    uint8_t                 cfgK;                                       /*!< Frames per multiframe. Register programmed with value - 1 */
    uint8_t                 ilaM;                                       /*!< Number of converters per device. Sets value for all lanes. Register programmed with value - 1 */
    uint8_t                 ilaN;                                       /*!< Converter resolution.  Sets value for all lanes. Register programmed with value - 1 */
    uint8_t                 ilaCs;                                      /*!< Control bits per sample.  Sets value for all lanes */
    uint8_t                 ilaNp;                                      /*!< Total bits per sample.  Sets value for all lanes. Register programmed with value - 1 */
    uint8_t                 ilaS;                                       /*!< Samples per converter per frame.  Sets value for all lanes. Register programmed with value - 1 */
    uint8_t                 ilaCf;                                      /*!< Control words per frame.  Sets value for all lanes */
    uint8_t                 ilaHd;                                      /*!< High density format.  Sets value for all lanes */
    uint8_t                 ilaSupportEn;                               /*!< ILA Support Enable */
    uint8_t                 sysRefAlways;                               /*!< SYSREF Always 1 = Core re-aligns LMFC counter on all SYSREF events */
    uint8_t                 sysRefRequiredOnResync;                     /*!< 1 = A SYSREF event is required following a Link Re-Sync event */

    /* JESD204C */
    uint8_t                 enableCmdInterface;                         /*!< 1 = Enables AXI4-Stream Cmd interface; 0 = Cmd words will be zeroed */
    uint8_t                 enableDataInterface;                        /*!< 1 = Enables AXI4-Stream Data interface; 0 = Link will be transmitting scrambled zeros */
    uint8_t                 cfgE;                                       /*!< CTRL_MB_IN_EMB- Number of multi-blocks in an extended multi-block. Programmed with actual value (0 is invalid) */
    uint8_t                 ctrlMetaMode;                               /*!< 0 = Cmd; 1 = CRC; 2 = FEC [TBD] */
} adi_fpgagen6_FramerCfg_t;

/**
* \brief LaneXBar Reset Mask Data Structure
*/
typedef struct adi_fpgagen6_LaneXBarResetMask
{
    uint8_t                         deframerSelect;     /*!< Deframer Select */
    uint16_t                        deframer0;          /*!< Deframer 0 */
    uint16_t                        deframer1;          /*!< Deframer 1 */
    uint16_t                        deframer2;          /*!< Deframer 2 */
    uint8_t                         framerSelect;       /*!< Framer Select */
    uint16_t                        framer1;            /*!< Framer 0 */
    uint16_t                        framer0;            /*!< Framer 1 */
} adi_fpgagen6_LaneXBarResetMask_t;

/**
* \brief Sync Output Select
*/
typedef enum adi_fpgagen6_SyncOutSel
{
    ADI_FPGAGEN6_SYNCOUT_DEFRAMER0 = 0x0U,      /*!< Deframer 0 */
    ADI_FPGAGEN6_SYNCOUT_DEFRAMER1,             /*!< Deframer 1 */
    ADI_FPGAGEN6_SYNCOUT_DEFRAMER2,             /*!< Deframer 2 */
    ADI_FPGAGEN6_SYNCOUT_SYNCINB0,              /*!< SYNCINB 0 pin */
    ADI_FPGAGEN6_SYNCOUT_SYNCINB1               /*!< SYNCINB 1 pin */
} adi_fpgagen6_SyncOutSel_e;

/**
* \brief Sync Source Select
*/
typedef enum adi_fpgagen6_SyncSrcSel
{
    ADI_FPGAGEN6_SYNCSRC_SYNCINB0 = 0x0U,     /*!< SYNCINB 0 pin */
    ADI_FPGAGEN6_SYNCSRC_SYNCINB1,            /*!< SYNCINB 1 pin */
    ADI_FPGAGEN6_SYNCSRC_DEFRAMER0,           /*!< Deframer 0 */
    ADI_FPGAGEN6_SYNCSRC_DEFRAMER1,           /*!< Deframer 1 */
    ADI_FPGAGEN6_SYNCSRC_DEFRAMER2            /*!< Deframer 2 */
} adi_fpgagen6_SyncSrcSel_e;

/**
* \brief Sync Polarity Select
*/
typedef enum adi_fpgagen6_SyncPolaritySel
{
    ADI_FPGAGEN6_SYNCPOLARITY_NON_INVERT = 0x0U,    /*!< Non Invert Polarity */
    ADI_FPGAGEN6_SYNCPOLARITY_INVERT                /*!< Invert Polarity */
} adi_fpgagen6_SyncPolaritySel_e;

/**
* \brief Sync Output Config
*/
typedef struct adi_fpgagen6_SyncOutCfg
{
    adi_fpgagen6_SyncOutSel_e       select;
    adi_fpgagen6_SyncPolaritySel_e  polarity;
} adi_fpgagen6_SyncOutCfg_t;

/**
* \brief Sync Source Select Config
*/
typedef struct adi_fpgagen6_SyncSrcSelCfg
{
    adi_fpgagen6_SyncSrcSel_e       select;
    adi_fpgagen6_SyncPolaritySel_e  polarity;
} adi_fpgagen6_SyncSrcSelCfg_t;

/**
* \brief SyncXBar Configuration
*/
typedef struct adi_fpgagen6_SyncXBar
{
    adi_fpgagen6_SyncOutCfg_t       deframer[ADI_FPGAGEN6_MAX_DEFRAMERS];
    adi_fpgagen6_SyncSrcSelCfg_t    framer[ADI_FPGAGEN6_MAX_FRAMERS];
} adi_fpgagen6_SyncXBar_t;

/**
* \brief Sync Mode
*/
typedef enum adi_fpgagen6_SyncModeSel
{
    ADI_FPGAGEN6_SYNCMODE_DEFAULT = 0x0U,   /*!< Default SyncBar Mode */
    ADI_FPGAGEN6_SYNCMODE_LOOPBACK_NEAR,    /*!< Near Loopback Mode */
    ADI_FPGAGEN6_SYNCMODE_UNKNOWN
} adi_fpgagen6_SyncModeSel_e;

typedef struct adi_fpgagen6_SysRefClkCfg
{
    uint16_t intSysRefGenPeriod;    /*!< Number of device clock cycles to generate internal SYSREF pulse (Bits 11:0 Used) */
    uint8_t  intSysRefEnable;       /*!< Set this to 1 to enable internal SYSREF */
    uint8_t  extSysRefSampleEdge;   /*!< Set this to 1 to sample external SYSREF on rising edge; 0 for falling edge */
    uint8_t  extSysRefPolarity;     /*!< Set to 1 to invert external SYSREF polarity */
} adi_fpgagen6_SysRefClkCfg_t;


#endif  /*_ADI_FPGAGEN6_DATA_INTERFACE_TYPES_H_ */
