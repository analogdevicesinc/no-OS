/**
 * \file
 * \brief Contains ADRV9001 Frequency Hopping data types
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_FH_TYPES_H_
#define _ADI_ADRV9001_FH_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif
#include "adi_adrv9001_gpio_types.h"

/**
 * \brief Max number of GPIO pins for frequency selection. 
 */
#define ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS 6u /* 6 pins is enough to index all 64 frequencies in table */

/**
 * \brief Max number of hop tables.
 */
#define ADI_ADRV9001_FH_MAX_HOP_TABLE_SIZE 64u

/**
 * \brief Max number of GPIO pins for gain selection.
 */
#define  ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS 3u

/**
 * \brief Max number of entries in Rx gain and Tx attenuation tables.
 */
#define  ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES 8u

/**
 * \brief Minimum carrier frequency supported in frequency hopping mode
 */
#define ADI_ADRV9001_FH_MIN_CARRIER_FREQUENCY_HZ 25000000llu    /* 25 MHz */

/**
 * \brief Maximum carrier frequency supported in frequency hopping mode
 */
#define ADI_ADRV9001_FH_MAX_CARRIER_FREQUENCY_HZ 6000000000llu  /* 6 GHz */

/**
 * \brief Maximum Tx analog front end delay in hop frames
 */
#define ADI_ADRV9001_FH_MAX_TX_FE_POWERON_FRAME_DELAY 64u

/**
 * \brief Enumeration of frequency hopping modes
 */
typedef enum {
    ADI_ADRV9001_FHMODE_LO_MUX_PREPROCESS          = 0u,   /*!< Frequency hopping with LO muxing. Frequency hopping table is processed before the frequency hopping operation. */
    ADI_ADRV9001_FHMODE_LO_MUX_REALTIME_PROCESS    = 1u,   /*!< Frequency hopping with LO muxing. Frequency hopping table is processed during frequency hopping operation. */
    ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS = 2u,   /*!< Frequency hopping with LO retuning during transition time. Frequency hopping table is processed during frequency hopping operation. */
    ADI_ADRV9001_FHMODE_LO_RETUNE_REALTIME_PROCESS_DUAL_HOP = 3u, /*!< 2 HOP signals are used to control Channel 1 and Channel 2 independently. Frequency hopping with LO retuning during transition time.
                                                                       Frequency hopping table is processed during frequency hopping operation. */
} adi_adrv9001_FhMode_e;

typedef enum {
    ADI_ADRV9001_TABLEINDEXCTRL_AUTO_LOOP      = 0u, /*!< Automatically increment through a frequency hopping table and wrap around once end has been reached */
    ADI_ADRV9001_TABLEINDEXCTRL_AUTO_PING_PONG = 1u, /*!< Ping pong operation between ADI_ADRV9001_FHHOPTABLE_A and ADI_ADRV9001_FHHOPTABLE_B. 
                                                          Automatically increment through one frequency hopping table and switch to the other 
                                                          once end has been reached */
    ADI_ADRV9001_TABLEINDEXCTRL_GPIO           = 2u, /*!< Use GPIO pins to index entries within a frequency hopping table */
} adi_adrv9001_TableIndexCtrl_e;

/**
 * \brief Enumeration of frequency hopping table ID's 
 */
typedef enum {
    ADI_ADRV9001_FHHOPTABLE_A = 0u, /*!< ID for hop table A in arm memory */
    ADI_ADRV9001_FHHOPTABLE_B = 1u, /*!< ID for hop table B in arm memory */
} adi_adrv9001_FhHopTable_e;

/**
 * \brief Enumeration of hop frame Indices
 */
typedef enum {
    ADI_ADRV9001_FHFRAMEINDEX_CURRENT_FRAME  = 0u, /*!< Hop frame currently in progress */
    ADI_ADRV9001_FHFRAMEINDEX_UPCOMING_FRAME = 1u, /*!< The upcoming hop frame after the current. 
                                                        Only applicable in LO_MUX, will match CURRENT in LO_RETUNE*/
} adi_adrv9001_FhFrameIndex_e;

/**
 * \brief Enumeration of FH per dynamic load type used by FPGA in ADI eval system, not necesary in customer application
 */
typedef enum adi_adrv9001_FhPerDynamicLoad
{
    ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_ONE = 0,    /*!< 1 HOP per Dynamic Load */
    ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_FOUR = 1,   /*!< 4 HOP per Dynamic Load */
    ADI_ADRV9001_FH_HOP_PER_DYNAMIC_LOAD_EIGHT = 2   /*!< 8 HOP per Dynamic Load */
} adi_adrv9001_FhPerDynamicLoad_e;

/**
 * \brief Enumeration of hop signals
 */
typedef enum adi_adrv9001_FhHopSignal
{
    ADI_ADRV9001_FH_HOP_SIGNAL_1 = 0, /*!< Hop 1 signal */
    ADI_ADRV9001_FH_HOP_SIGNAL_2 = 1  /*!< Hop 2 signal */
} adi_adrv9001_FhHopSignal_e;

/**
 * \brief Enumeration of frequency hopping table select modes 
 */
typedef enum {
    ADI_ADRV9001_FHHOPTABLESELECTMODE_INDEPENDENT = 0u,     /*!< Each assigned GPIO pin controls the corresponding channel */
    ADI_ADRV9001_FHHOPTABLESELECTMODE_COMMON = 1u,          /*!< hopTableSelectGpioConfig[0] is used to control both channels, where applicable */
} adi_adrv9001_FhHopTableSelectMode_e;

/**
 * \brief Settings for HOP frame information
 */
typedef struct {
    int32_t  rx1OffsetFrequencyHz;   /*!< Rx1 Offset frequency. This field is ignored by firmware if frame is not Rx */
    int32_t  rx2OffsetFrequencyHz;   /*!< Rx2 Offset frequency. This field is ignored by firmware if frame is not Rx */
    uint8_t tx1Attenuation_fifthdB;  /*!< Tx1 attenuation expressed in 0.2dBs. LSB = 0.2dB. Range 0 to 209 (0 to 41.8 dB). This field is ignored if frame is Rx */
	uint8_t tx2Attenuation_fifthdB;  /*!< Tx2 attenuation expressed in 0.2dBs. Range 0 to 209 (0 to 41.8 dB). This field is ignored if frame is Rx */
    uint8_t  rx1GainIndex;           /*!< Starting Rx1 gain index for hop frame. This field is ignored if frame is Tx */
	uint8_t  rx2GainIndex;           /*!< Starting Rx2 gain index for hop frame. This field is ignored if frame is Tx */
    uint64_t hopFrequencyHz; /*!< Operating frequency in Hz */
} adi_adrv9001_FhHopFrame_t;

/**
 * \brief Frequency hopping gain setup by pin config
 *  Up to 8 Rx gains and 8 Tx attenuation levels can be loaded to ARM memory and
 *  indexed during frequency hopping operation by up to 3 GPIO pins.
 *  This structure provides the configuration data for GPIO assignment and 
 *  gain/attenuation table loading. 
 * 
 *  The user can load any valid Rx gain index or Tx attenuation level
 *  in the tables.
 */
typedef struct { 
    /* Rx gain table information */
    uint8_t                          numRxGainTableEntries;         /*!< Number of gain levels in Rx gain table. Ignored if gainSetupByPin is false. */
    uint8_t                          rxGainTable[ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES];  /*!< Pointer to Rx gain table used in frequency hopping. Ignored if gainSetupByPin is false. */
    /* Tx attenuation table information */
    uint8_t                          numTxAttenTableEntries;        /*!< Number of attenuation levels in Tx attenuation table. Ignored if gainSetupByPin is false. */
    uint16_t                         txAttenTable[ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_ENTRIES]; /*!< Pointer to Tx attenuation table used in frequency hopping. Ignored if gainSetupByPin is false. */

    /* GPIO configuration */
    uint8_t                          numGainCtrlPins;               /*!< Number of pins assigned for gain control. Ignored if gainSetupByPin in adi_adrv9001_FhCfg_t is false. */
    adi_adrv9001_GpioCfg_t           gainSelectGpioConfig[ADI_ADRV9001_FH_MAX_NUM_GAIN_SELECT_PINS];  /*!< Pin configuration for gain select. Ignored if gainSetupByPin is false. */
} adi_adrv9001_FhGainSetupByPinCfg_t; 

/**
 * \brief Hop Table Select configurations.
 */
typedef struct {
    adi_adrv9001_FhHopTableSelectMode_e     hopTableSelectMode;             /*!< Mode of control for hop table select. Independent or common pin contorl */
    adi_adrv9001_GpioCfg_t                  hopTableSelectGpioConfig[2u];   /*!< Pin configuration for hop table select. Not an error if unassigned, just ignored.*/
} adi_adrv9001_FhhopTableSelectCfg_t;

/**
 * \brief Frequency hopping configuration data
 */
typedef struct {
    adi_adrv9001_FhMode_e              mode;                        /*!< Frequency hopping mode */
    adi_adrv9001_FhHopSignal_e         rxPortHopSignals[ADI_ADRV9001_NUM_CHANNELS]; /*!< Hop Signals for each RF port [0] = Rx1, [1] = Rx2 */
    adi_adrv9001_FhHopSignal_e         txPortHopSignals[ADI_ADRV9001_NUM_CHANNELS]; /*!< Hop Signals for each RF port [0] = Tx1, [1] = Tx2 */
    bool                               rxZeroIfEnable;              /*!< Configure the Rx ports for zero-IF operation. 
                                                                         FALSE:  LO = hopFrequencyHz + rx(n)OffsetFrequencyHz.  
                                                                         TRUE:   LO = hopFrequencyHz */
    adi_adrv9001_GpioCfg_t             hopSignalGpioConfig[2u];     /*!< Pin configuration for HOP1 and HOP2 signals. If GPIO is unassigned, HOP signal source is defaulted to SPI mode*/                      
    adi_adrv9001_FhhopTableSelectCfg_t hopTableSelectConfig;        /*!< Hop table select signal configuration */ 
    adi_adrv9001_TableIndexCtrl_e      tableIndexCtrl;              /*!< Select control type for frequency hopping table */
    uint8_t                            minRxGainIndex;              /*!< Minimum Rx gain index for FH operation. Used for calibration over specified gain range. Valid range is from 0 to maxGainIndex  */
    uint8_t                            maxRxGainIndex;              /*!< Maximum Rx gain index for FH operation. Used for calibration over specified gain range. Valid range is from minGainIndex to 255 */
    uint16_t                           minTxAtten_mdB;              /*!< Minimum Tx attenuation level for FH operation. Used for calibration over specified attenuation range. Valid range is from 0 to maxTxAtten_mdB */
    uint16_t                           maxTxAtten_mdB;              /*!< Maximum Tx attenuation level for FH operation. Used for calibration over specified attenuation range. Valid range is from minTxAtten_mdB to 41950 */
    uint64_t                           minOperatingFrequency_Hz;    /*!< Minimum frequency used during FH operation. Used for calibration. Valid range is from 30Mhz to maxOperatingFrequency_Hz */
    uint64_t                           maxOperatingFrequency_Hz;    /*!< Maximum frequency used during FH operation. Used for calibration. Valid range is from minOperatingFrequency_Hz to 3.2GHz */
    uint32_t                           minFrameDuration_us;         /*!< Minimum frame duration to be supported during FH operation, in microseconds. */
    uint8_t                            txAnalogPowerOnFrameDelay;   /*!< A delay specified in terms of hop frames. Tx analog front end can be delayed relative to the first Tx setup rising edge. 
                                                                        Set this field to greater than 0 to support use cases where Tx propagation delay is longer than a single hop frame duration. */
    uint8_t                            numTableIndexPins;               /*!< Number of pins for table indexing */
    adi_adrv9001_GpioCfg_t             tableIndexGpioConfig[ADI_ADRV9001_FH_MAX_NUM_FREQ_SELECT_PINS]; /*!< Pin configuration for table index select. Ignored if tableIndexCtrl is not ADI_ADRV9001_TABLEINDEXCTRL_GPIO */
    
    bool                               gainSetupByPin;            /*!< Use GPIO Pins to provide a Tx/Rx gain index for next hop frame. If false, gain information is provided in hop table*/
	adi_adrv9001_FhGainSetupByPinCfg_t gainSetupByPinConfig[ADI_ADRV9001_NUM_CHANNELS];    /*!< Configuration information for gain setup by pin. This structure is ignored if gainSetupByPin is false */
} adi_adrv9001_FhCfg_t;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_FH_TYPES_H_ */
