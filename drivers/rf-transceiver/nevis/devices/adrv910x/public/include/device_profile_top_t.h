/*! 
 * @brief   Device Profile external data declarations
 *
 * @details This file contains all the typedef device profile prototypes
 *           referenced globally.
 */
/*******************************************************************************
  Copyright(c) 2017-2018 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
********************************************************************************/

#ifndef __DEVICE_PROFILE_TOP_T_HEADER__
#define __DEVICE_PROFILE_TOP_T_HEADER__

#include "adi_device_profile_pack.h"
#include "device_profile_channels_t.h"
#include "device_profile_algo_dp_t.h"
#include "device_profile_tx_dp_t.h"
#include "device_profile_rx_dp_t.h"
#include "device_profile_ssi_t.h"
#include "device_profile_pll_common_t.h"
#include "device_profile_mcs_t.h"
#include "device_profile_version_t.h"
#include "device_profile_rx_gaincontrol_t.h"


/*!
 *  @addtogroup device_profile
 *  @ingroup drivers
 *  @{
 */

#define PROFILE_RX_CHAN_MASK            (0x03U)
#define PROFILE_RX_CHAN_POS             (0U)

#define PROFILE_TX_CHAN_MASK            (0x0CU)
#define PROFILE_TX_CHAN_POS             (2U)

#define PROFILE_ORX_CHAN_MASK           (0x30U)
#define PROFILE_ORX_CHAN_POS            (4U)

#define PROFILE_ILB_CHAN_MASK           (0xC0U)
#define PROFILE_ILB_CHAN_POS            (6U)

#define PROFILE_ELB_CHAN_MASK           (0x300U)
#define PROFILE_ELB_CHAN_POS            (8U)

#define PROFILE_RX_PIN_MASK             (0xC00U)
#define PROFILE_RX_PIN_POS              (10U)

#define PROFILE_CHAN_CONFIG_MASK        (PROFILE_RX_CHAN_MASK    \
                                         | PROFILE_TX_CHAN_MASK  \
                                         | PROFILE_ORX_CHAN_MASK \
                                         | PROFILE_ILB_CHAN_MASK \
                                         | PROFILE_ELB_CHAN_MASK  )
                                         
#define PROFILE_CHAN_2_MASK ((PROFILE_RX_CHAN_MASK  & 0x002u) |\
                             (PROFILE_TX_CHAN_MASK  & 0x008u) |\
                             (PROFILE_ORX_CHAN_MASK & 0x020u) |\
                             (PROFILE_ILB_CHAN_MASK & 0x080U) |\
                             (PROFILE_ELB_CHAN_MASK & 0x200u))

typedef enum
{
    Default = 0,    /*!< ADC configuration Default */
    Option3 = 3,    /*!< ADC configuration Option 3 */
    Option4 = 4,    /*!< ADC configuration Option 4 */
    Option5 = 5,    /*!< ADC configuration Option 5 */
    Option6 = 6,    /*!< ADC configuration Option 6 */
    Option7 = 7,    /*!< ADC configuration Option 7 */
} ADI_NEVIS_PACK_ENUM adcOption_e;

typedef enum
{
    TDD,
    FDD,
} ADI_NEVIS_PACK_ENUM duplexMode_e;

typedef enum
{
    LP,         /*!< Low Power ADC */
    HP,         /*!< High Power ADC */
} ADI_NEVIS_PACK_ENUM adcType_e;

//typedef enum
//{
//    LPADC_PERIODIC,
//    LPADC_CONTINUOUS,
//} ADI_NEVIS_PACK_ENUM lpAdcCalMode_e; //REMOVED

ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t        enableRiseToOnDelay;        /*!< Delay from T/RX_enable rising  edge until LNA is powered up, 0 to 2^24-1 ARM clock cycles */
    uint32_t        enableRiseToAnalogOnDelay;  /*!< Delay from T/RX_enable rising  edge until T/RX analog power up procedure commences (this may or may not include PLL retuning), 0 to 2^24-1 ARM clock cycles */
    uint32_t        enableFallToOffDelay;       /*!< Delay from T/RX_enable falling edge until LNA is powered down, 0 to 2^24-1 ARM clock cycles */
    uint32_t        enableGuardDelay;           /*!< Delay from T/RX_enable rising  edge until first valid data is received over the air, 0 to 2^24-1 ARM clock cycles */
    uint32_t        enableHoldDelay;            /*!< Delay from T/RX_enable falling edge until the T/Rx interface is disabled, 0 to 2^24-1 ARM clock cycles */
} enableDelay_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    duplexMode_e    duplexMode;
    uint8_t         fhModeOn;
    uint8_t         numDynamicProfile;          /*!< Number of Profile. =1 means only one profile and no switching */
    mcsMode_e       mcsMode;                    /*!< MCS mode selection: 0 - Disable, 1 - MCS Only, 2 - MCS + RFPLL phase sync*/
    uint16_t        pllLockTime_us;             /*!< Required lock time in microseconds for PLLs, based on ref_clk and loop bandwidth */
    pllModulus_t    pllModuli;                  /*!< PLL moduli */
    uint16_t        pllPhaseSyncWait_us;        /*!< Worst case phase sync wait time in FH */
    mcsInf_e        mcsInterfaceType;           /*!< NEW  0-Disabled, 1-CMOS, 2-LVDS */
    uint8_t         warmBootOn;                 /*!< Flag to enable Init Cal warmboot: 0 -Disable */    
    uint8_t         padding[2u];               /*!< Reserved for future feature */
} deviceSysConfig_t;
ADI_NEVIS_PACK_FINISH

/* signalType_e has been moved to pll_common_t.h */

typedef enum
{
	RX_SIGNAL_TYPE_IQ,
	RX_SIGNAL_TYPE_FREQUENCY_DEVIATION,
	RX_SIGNAL_TYPE_SYMBOLS,
} ADI_NEVIS_PACK_ENUM rxSignalType_e;

/****************************************************************************
* Profile information that will define the Receiver (RX/ORX) channel.
****************************************************************************/
ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t            primaryBw_Hz;           /*!< RX primary Bandwidth */
    uint32_t            rxOutputRate;           /*!< RX Output data rate */
    uint32_t            rxInterfaceSampleRate;  /*!< RX sample rate at serial interface */
    int32_t             offsetLo_Hz;            /*!< Offset in Hz. 0: On LO */
    uint8_t             ncoEn;                  /*!< !0: DDC NCO is enabled  0: no DDC */   
    rxSignalType_e      outputSignalingSel;     /*!< Output to BBIC signal type */
    uint32_t            adcCorner;              /*!< ADC 3dB corner frequency in Hz */
    uint32_t            adcClk_Hz;              /*!< ADC clock 2.2G/1.47G/1.1G */
    uint32_t            corner3dB;              /*!< TIA bandwidth */
    componentPowerLevel_e  tiaPower;               /*!< RX TIA Power setting */
    ssiConfig_t         rxSsiConfig;            /*!< RX Serial data interface config */
    enableDelay_t       rxEnableDelay;          /*!< RX enable delay timing */
    rxGainTableType_e   gainTableType;          /*!< type of gain table loaded during ADRV910X initialization */
    LoId_e              LoSelect;               /*!< The selected LO for this channel */
} rxConfig_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    rxDpProfile_t       rxDpProfile;            /*!< RX digital data path config */
    uint8_t             wbSelNbDemod;    
    uint8_t             reserved[3u];           /*!< Reserved for future feature */
} rx1Config_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    rxNbDpProfile_t     rxNbDpProfile;            /*!< RX digital data path config */
    adcOption_e         adcOption;              /*!< ADC option */
    rx_gainControl_t    gainControl;            /*!< The selected gain control config */
} rxNbConfig_t;
ADI_NEVIS_PACK_FINISH

/****************************************************************************
* Profile information that will define the Transmitter channel.
****************************************************************************/
ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t             pdBiasCurrent;          /*!< pre-distorter programmable bias current*/
    uint8_t             pdGainEn;               /*!< TX Pre-distortion gain enable */
    uint32_t            prePDRealPole;          /*!< TX Pre-distortion filter Fc */
    uint32_t            postPDRealPole;         /*!< Post-distorter (i.e. interstage) filter Fc  */
    uint8_t             padding[2u];            
} txPredistConfig_t;    
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    uint8_t             extLoopBackType;        /*!< 0: No external loopback connect,  1: loopback before PA, 2: loopback after PA. */
    uint8_t             extLoopBackForInitCal;  /*!< 0: ext loop back should not be used for init cal */
    int16_t             peakLoopBackPower;      /*!< target RMS signal power at input to receiver plus its peak-to-average ratio, -40 to +5 dBm, 0.1dB resolution */
} txElbConfig_t;   
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t            butterFilterBw;         /*!< TX butter filter BW for TxBBF */    
    componentPowerLevel_e  txBbfPowerMode;      /*!< TXBBF  filter power mode */
    uint8_t             padding[3u];
} txBbfConfig_t;   
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    uint32_t            primaryBw_Hz;           /*!< TX primary Bandwidth */
    uint32_t            txInputRate;            /*!< TX Input data rate */
    uint32_t            txInterfaceSampleRate;  /*!< TX sample rate at serial interface */
    int32_t             offsetLo_Hz;            /*!< Frequency of LO relative to carrier, positive means LO higher */
    signalType_e        outputSignalingSel;     /*!< Output to Analog signal type */
    txBbfConfig_t       txBbfConfig;            /*!< TX BBF config*/
    txPredistConfig_t   txPredistConfig;        /*!< Predistorter config */
    txElbConfig_t       txElbConfig;            /*!< External Loopback configuration*/
    uint32_t            freqDev;                /*!< freq dev value in Hz for both FM_IQ and FM_DM.*/
    ssiConfig_t         txSsiConfig;            /*!< TX Serial data interface config */
    enableDelay_t       txEnableDelay;          /*!< TX enable delay timing */
    LoId_e              LoSelect;               /*!< The selected LO for this channel */
    uint8_t             padding[2u];            /*!< padding for 32 bit alignment */
} txConfig_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    txDpProfile_t       txDpProfile;            /*!< TX digital data path config */
    uint32_t            reserved[1u];           /*!< Reserved for future feature */
} tx1Config_t;
ADI_NEVIS_PACK_FINISH

ADI_NEVIS_PACK_START
typedef struct
{
    txNbDpProfile_t     txNbDpProfile;            /*!< TX digital data path config */
    uint32_t            reserved[1u];           /*!< Reserved for future feature */
} txNbConfig_t;
ADI_NEVIS_PACK_FINISH

/****************************************************************************
* Profile information provided by BBIC at init via CONFIG command
*
* Profile struct holds clock rates, divider ratios, sampling rates, etc.
* Populated at boot via CONFIG command.
*
* If this structure is initialized to all 0's the linker puts this in the
* stack section therefore initializing devclk_hz to 1
****************************************************************************/
ADI_NEVIS_PACK_START
typedef struct
{
    profileVersion_t ver;                  /*!< Version information */
    uint32_t         vcoFreq_daHz;             /*!< BBPLL VCO frequency of the system as set by API, in dekaHz */
    uint32_t         hsDigFreq_Hz;             /*!< HS Dig as calc by API */
    uint32_t         devClock_Hz;              /*!< reference clock */
	uint32_t         ps1ArmClkDiv;             /*!< PS1 ARM clock divider */
    uint8_t          armPowerSavingClkDiv;     /*!< ARM power saving clock divider 1/N. (0: Div1 (184MHz); 1: Div2 ... 255: Div256) */
    uint8_t          refClockOutEn;            /*!< reference clock output disable/enable */
    componentPowerLevel_e   clkPllPowerLevel;  /*!< Clock Pll power level */
    uint8_t          clkGenDis;                /*!< Disable clock PLL and use device clock only*/
    uint8_t          clkDoublerEn;             /*!< flag to enable clock doubler */
    uint8_t          ps2En;                    /*!< Processor 2 is enabled */
    uint8_t          ps2ArmClkDiv;             /*!< PS2 ARM clock = hsDigFreq_Hz /  ps2ArmClkDiv*/
    uint8_t          loPhaseSync;              /*!< LO Phase Sync mode */
    loConfig_t       loConfig[NUM_EXT_LOS];    /*!< LO configuration */

    uint16_t         chanConfig;               /*!< Channel configuration
                                                    D0 -  Rx1           0 = Disabled, 1 = Enabled
                                                    D1 -  RxNb          0 = Disabled, 1 = Enabled
                                                    D2 -  Tx1           0 = Disabled, 1 = Enabled
                                                    D3 -  TXNb			0 = Disabled, 1 = Enabled
                                                    D4 -  ORx1          0 = Disabled, 1 = Enabled
                                                    D5 -  ORxNb          0 = Disabled, 1 = Enabled
                                                    D6 -  ILB1          0 = Disabled, 1 = Enabled
                                                    D7 -  ILBNb          0 = Disabled, 1 = Enabled
                                                    D8 -  ELB1          0 = Disabled, 1 = Enabled
                                                    D9 -  ELBNb         0 = Disabled, 1 = Enabled
                                                */
    uint8_t          observationMixer;          /*!< RX1 mixer = 0, RXNB mixer = 1, both ORX and ELB use the same mixer*/
    uint8_t          padding[2u];        
    txConfig_t       txConfig[NUM_TX_CHANNELS];  /*!< TX configuration */
    rxConfig_t       rxConfig[NUM_RX_CHANNELS];  /*!< RX configuration */
    rxConfig_t       orxConfig[NUM_RX_CHANNELS]; /*!< ORX configuration */
    rxConfig_t       ilbConfig[NUM_RX_CHANNELS]; /*!< Internal Loopback configuration */
    rxConfig_t       elbConfig[NUM_RX_CHANNELS]; /*!< External Loopback configuration */
    
    tx1Config_t      tx1Config;
    rx1Config_t      rx1Config;  /*!< RX configuration */
    rx1Config_t      orx1Config; /*!< ORX configuration */
    rx1Config_t      ilb1Config; /*!< Internal Loopback configuration */
    rx1Config_t      elb1Config; /*!< External Loopback configuration */ 
	     
    txNbConfig_t     txNbConfig;                 
    rxNbConfig_t     rxNbConfig;  /*!< RX configuration */
    rxNbConfig_t     orxNbConfig; /*!< ORX configuration */
    rxNbConfig_t     ilbNbConfig; /*!< Internal Loopback configuration */
    rxNbConfig_t     elbNbConfig; /*!< External Loopback configuration */    
    
    deviceSysConfig_t sysConfig;
    uint32_t         checksum;                   /*!< Device profile checksum */
} deviceProfile_t; //Initially TO BE REMOVED, but actually no
ADI_NEVIS_PACK_FINISH

/*! @} */

#endif /*! __DEVICE_PROFILE_TOP_T_HEADER__ */
