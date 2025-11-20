/**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */
 
 /**
 * \file adrv904x_cpu_object_ids_types.h
 * 
 * \brief   Contains CPU Object ID definitions
 *
 * \details Contains CPU Object ID definitions
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_OBJECT_IDS_TYPES_H__
#define __ADRV904X_CPU_OBJECT_IDS_TYPES_H__

#include <stdint.h>

typedef enum adrv904x_CpuObjectId
{
    /* Initial calibrations */
    ADRV904X_CPU_OBJID_IC_START                     = 0x00u, /*!< Start of Initial Calibration OBJID */
    ADRV904X_CPU_OBJID_IC_RC_TUNER                  = 0x00u, /*!< RC Tuner                       */
    ADRV904X_CPU_OBJID_IC_RX_DC_OFFSET              = 0x01u, /*!< Rx DC offset                   */
    ADRV904X_CPU_OBJID_IC_ADC_RX                    = 0x02u, /*!< ADC Rx                         */
    ADRV904X_CPU_OBJID_IC_ADC_ORX                   = 0x03u, /*!< ADC ORx                        */
    ADRV904X_CPU_OBJID_IC_ADC_TXLB                  = 0x04u, /*!< ADC Tx Loopback                */
    ADRV904X_CPU_OBJID_IC_TXDAC                     = 0x05u, /*!< TX DAC                         */   
    ADRV904X_CPU_OBJID_IC_TXBBF                     = 0x06u, /*!< TXBBF                          */
    ADRV904X_CPU_OBJID_IC_TXLB_PATH_DLY             = 0x07u, /*!< TxLB Path delay                */
    ADRV904X_CPU_OBJID_IC_TX_ATTEN_CAL              = 0x08u, /*!< TX Atten CAL (Gain/Phase)      */
    ADRV904X_CPU_OBJID_IC_HRM                       = 0x09u, /*!< HRM                            */
    ADRV904X_CPU_OBJID_IC_TXQEC                     = 0x0Au, /*!< TxQEC init                     */
    ADRV904X_CPU_OBJID_IC_TXLOL                     = 0x0Bu, /*!< LOL Internal Init              */
    ADRV904X_CPU_OBJID_IC_SERDES                    = 0x0Cu, /*!< SERDES Initial Cal             */
    ADRV904X_CPU_OBJID_IC_RX_TIA                    = 0x0Du, /*!< RX TIA                         */
    ADRV904X_CPU_OBJID_IC_TXLB_FILTER               = 0x0Eu, /*!< TxLB Filter cal                */
    ADRV904X_CPU_OBJID_IC_TEST_0                    = 0x0Fu, /*!< Test cal 0, Initial Cal Framework test */
    ADRV904X_CPU_OBJID_IC_TEST_1                    = 0x10u, /*!< Test cal 1, Initial Cal Framework test */
    ADRV904X_CPU_OBJID_IC_TXRX_PHASE                = 0x11u, /*!< Tx/Rx Phase Cal                */
    ADRV904X_CPU_OBJID_IC_RXSPUR                    = 0x12u, /*!< RxSpur Initial Cal             */
    ADRV904X_CPU_OBJID_IC_END                       = 0x12u, /*!< End of Initial Calibration OBJID */
    ADRV904X_CPU_OBJID_IC_NUM_CALS                  = (ADRV904X_CPU_OBJID_IC_END - ADRV904X_CPU_OBJID_IC_START + 1u), /*!< number of initial cals */
   
    /* Tracking calibrations */
    ADRV904X_CPU_OBJID_TC_START                     = 0x30u, /*!< Start of Tracking Calibration OBJID */
    ADRV904X_CPU_OBJID_TC_RXQEC                     = 0x30u, /*!< Rx QEC                 */
    ADRV904X_CPU_OBJID_TC_TX_LOL                    = 0x31u, /*!< TX LOL                 */
    ADRV904X_CPU_OBJID_TC_TXQEC                     = 0x32u, /*!< Tx QEC                 */
    ADRV904X_CPU_OBJID_TC_SERDES                    = 0x33u, /*!< SERDES                 */
    ADRV904X_CPU_OBJID_TC_INTERNAL_START            = 0x34u, /*!< Start of Internal Tracking Calibration OBJID */
    ADRV904X_CPU_OBJID_TC_RX_ADC                    = 0x34u, /*!< Rx ADC                 */
    ADRV904X_CPU_OBJID_TC_TXLB_ADC                  = 0x35u, /*!< TxLb ADC               */
    ADRV904X_CPU_OBJID_TC_ORX_ADC                   = 0x36u, /*!< ORx ADC                */
    ADRV904X_CPU_OBJID_TC_END                       = 0x36u, /*!< End of Tracking Calibration OBJID */
    ADRV904X_CPU_OBJID_TC_NUM_CALS                  = (ADRV904X_CPU_OBJID_TC_END - ADRV904X_CPU_OBJID_TC_START + 1u), /*!< number of tracking cals */
    
    /* Configuration objects */
    ADRV904X_CPU_OBJID_CFG_DEVICE_PROFILE           = 0x80u, /*!< Configuration profile       */
    ADRV904X_CPU_OBJID_CFG_RADIO_EVENT              = 0x81u, /*!< Radio events module         */
    ADRV904X_CPU_OBJID_CFG_INITIAL_CALS             = 0x82u, /*!< Initial Calibration framework configuration */
    ADRV904X_CPU_OBJID_CFG_CAL_SCHEDULER            = 0x83u, /*!< Calibration scheduler */
    ADRV904X_CPU_OBJID_CFG_HM                       = 0x84u, /*!< HM Timer Control */    
    ADRV904X_CPU_OBJID_CFG_PARITY_ERROR_CHECK       = 0x85u, /*!< Configurable objects for memory refresh */
    ADRV904X_CPU_OBJID_CFG_SYSTEM_CONFIG            = 0x86u, /*!< System configuration */
    ADRV904X_CPU_OBJID_CFG_TRACKING_CALS            = 0x87u, /*!< Set tracking cal framework configuration */    
    ADRV904X_CPU_OBJID_CFG_TELEM                    = 0x8Cu, /*!< Configure the telemetry logging */
    ADRV904X_CPU_OBJID_CFG_RESERVED3                = 0x8Du, /*!< reserved */
    ADRV904X_CPU_OBJID_CFG_RESERVED4                = 0x8Eu, /*!< reserved */
    ADRV904X_CPU_OBJID_CFG_RESERVED5                = 0x8Fu, /*!< reserved */    

    /* Driver objects ( 0xB0 - 0xBF ) */
    ADRV904X_CPU_OBJID_DRV_NCO                      = 0xB0u, /*!< NCO     */
    ADRV904X_CPU_OBJID_DRV_STREAM                   = 0xB1u, /*!< Stream  */
    ADRV904X_CPU_OBJID_DRV_FSC                      = 0xB2u, /*!< FSC  */
    ADRV904X_CPU_OBJID_DRV_MASTER_BIAS              = 0xB3u, /*!< Master Bias */
    ADRV904X_CPU_OBJID_DRV_LDO                      = 0xB4u, /*!< LDO */
    ADRV904X_CPU_OBJID_DRV_DWT                      = 0xB5u, /*!< DWT */
    ADRV904X_CPU_OBJID_DRV_TEMP                     = 0xB6u, /*!< TEMP */
    ADRV904X_CPU_OBJID_DRV_PLL                      = 0xB7u, /*!< PLL */
    ADRV904X_CPU_OBJID_DRV_JESD                     = 0xB8u, /*!< JESD */
    ADRV904X_CPU_OBJID_DRV_UART                     = 0xB9u, /*!< UART */	
    ADRV904X_CPU_OBJID_DRV_TXATTEN                  = 0xBAu, /*!< TXATTEN  */
    ADRV904X_CPU_OBJID_DRV_TXLOL                    = 0xBBu, /*!< TX LOL Accumulator */
    ADRV904X_CPU_OBJID_DRV_RX_CFG                   = 0xBCu, /*!< Rx Config */
    ADRV904X_CPU_OBJID_DRV_GPIO                     = 0xBDu, /*!< GPIO */
    ADRV904X_CPU_OBJID_DRV_RADIO_SEQUENCER          = 0xBEu, /*!< Radio Sequencer */
    ADRV904X_CPU_OBJID_CFG_CARRIER_RECONFIG         = 0xBFu, /*!< Carrier Reconfigure */
    ADRV904X_CPU_OBJID_HAL_RADIO                    = 0xC0u, /*!< HAL_RADIO */
    ADRV904X_CPU_OBJID_DRV_DUMMY                    = 0xF0u, /*!< Dummy driver object */
    ADRV904X_CPU_OBJID_DRV_DDCC                     = 0xF1u, /*!< Dummy driver object */

    /* System objects */
    ADRV904X_CPU_OBJID_SYSTEM_START                 = 0xFFu, /*!< Start of System OBJID */
    ADRV904X_CPU_OBJID_SYSTEM_ERROR                 = 0xFFu, /*!< System error */
    ADRV904X_CPU_OBJID_SYSTEM_END                   = 0xFFu  /*!< End of System OBJID */

} adrv904x_CpuObjectId_e;

typedef uint32_t adrv904x_CpuObjectId_t;

/* Object Groups */
typedef enum adrv904x_CpuCalibrationsObjectGroup
{
    ADRV904X_CPU_OBJECT_CONFIG_INIT_CALS         = 0u,    /*!< Group 0 range 0x00 - 0x1f */
    ADRV904X_CPU_OBJECT_CONFIG_TRACKING_CALS,             /*!< Group 1 range 0x20 - 0x3f */
    ADRV904X_CPU_OBJECT_CONFIG_CTRL,                      /*!< Group 4 range 0x80 - 0x9f */
    ADRV904X_CPU_NUM_OBJECT_GROUPS
} adrv904x_CpuCalibrationsObjectGroup_e;


/* Mask to get the Initial cal object ID */
#define ADRV904X_CPU_INITCAL_OBJECT_ID_MASK       (0x1Fu)

/* Subtract the first tracking cal object ID to convert object ID to array index */
#define ADRV904X_CPU_TRACKING_CAL_OBJ_TO_IDX(x) ((x) - ADRV904X_CPU_OBJID_TC_START)

/* Add the first tracking cal object ID to convert array index to object ID */
#define ADRV904X_CPU_TRACKING_CAL_IDX_TO_OBJ(x) ((x) + ADRV904X_CPU_OBJID_TC_START)

/* Add the first system status object ID to convert array index to object ID */
#define ADRV904X_CPU_SYS_STATUS_IDX_TO_OBJ(x) ((x) + ADRV904X_CPU_OBJID_SYSTEM_START)

/* Convert an init cal object ID to a cal bitmask */
#define ADRV904X_CPU_INITCAL_OBJID_TO_CAL_MASK(x) (1ull << (x))

/* Convert an tracking cal object ID to a cal ID */
#define ADRV904X_CPU_TRACKING_OBJID_TO_CAL_ID(x) ((x) - ADRV904X_CPU_OBJID_TC_START)

/* Convert an tracking cal object ID to cal bitmask */
#define ADRV904X_CPU_TRACKING_OBJID_TO_CAL_MASK(x) (1ull << ((x) - ADRV904X_CPU_OBJID_TC_START))

/* Compare an cal object ID to ORX. Adding any new ORX object ID here */
#define ADRV904X_CPU_OBJID_IS_ORX(x) (((x) == ADRV904X_CPU_OBJID_IC_ADC_ORX) || ((x) == ADRV904X_CPU_OBJID_TC_ORX_ADC))

#endif /* __ADRV904X_CPU_OBJECT_IDS_TYPES_H__ */


