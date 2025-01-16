/*
 *---------------------------------------------------------------------------
 *
 * Copyright (c) 2020, 2021 Analog Devices, Inc. All Rights Reserved.
 * This software is proprietary to Analog Devices, Inc.
 * and its licensors.By using this software you agree to the terms of the
 * associated Analog Devices Software License Agreement.
 *
 *---------------------------------------------------------------------------
 */

/** @addtogroup phy PHY Definitions
 *  @{
 */

#ifndef ADI_PHY_H
#define ADI_PHY_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#if defined(ADIN1100)
#include "ADIN1100_addr_rdef.h"
#if defined(MDIO_CL22)
#include "ADIN1100_addr_rdef_22.h"
#endif
#elif defined(ADIN1110)
#include "ADIN1110_phy_addr_rdef.h"
#elif defined(ADIN2111)
#include "ADIN2111_phy_addr_rdef.h"
#endif

#include "adi_eth_common.h"
#include "hal.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! Size of the PHY device structure, in bytes. Needs to be a multiple of 4. */
#define ADI_PHY_DEVICE_SIZE             (48)

/*! Hardware reset value of MMD1_DEV_ID1 register, used for device identification. */
#define ADI_PHY_DEVID1                  (0x0283)
/*! Hardware reset value of MMD1_DEV_ID2 register (OUI field), used for device identification. */
#define ADI_PHY_DEVID2_OUI              (0x2F)

/*! CRSM interrupt sources showing a hardware error that requires a reset/reconfiguration of the device. */
#define ADI_PHY_CRSM_HW_ERROR           (0x2BFF)

/*!
 * @brief PHY driver states.
 */
typedef enum
{
    ADI_PHY_STATE_UNINITIALIZED = 0,        /*!< Software driver has not been initialized.                        */
    ADI_PHY_STATE_HW_RESET,                 /*!< PHY has been through hardware reset and needs to be initialized. */
    ADI_PHY_STATE_SOFTWARE_POWERDOWN,       /*!< PHY is in software powerdown.                                    */
    ADI_PHY_STATE_OPERATION,                /*!< PHY is in operation mode.                                        */
    ADI_PHY_STATE_DIAGNOSTIC,               /*!< PHY is in diagnostic mode.                                       */
    ADI_PHY_STATE_ERROR,                    /*!< Error state.                                                     */
} adi_phy_State_e;

/*!
 * @brief Advertised transmit operating mode.
 */
typedef enum
{
    ADI_PHY_AN_ADV_TX_REQ_1P0V = 0,         /*!< Request 1.0V transmit signal amplitude.                        */
    ADI_PHY_AN_ADV_TX_REQ_2P4V,             /*!< Request 2.4V transmit signal amplitude.                        */
    ADI_PHY_AN_ADV_TX_REQ_1P0V_ABLE_2P4V,   /*!< Request 1.0V transmit signal amplitude but able to do 2.4V.    */
} adi_phy_AnAdvTxMode_e;

/*!
 * @brief Resolved transmit operating mode.
 */
typedef enum
{
    ADI_PHY_AN_TX_LEVEL_RESOLUTION_NOT_RUN = 0,/*!< Auto-negotiation not run.              */
    ADI_PHY_AN_TX_LEVEL_RESERVED,          /*!< Auto-negotiation configuration fault.  */
    ADI_PHY_AN_TX_LEVEL_1P0V,         /*!< Resolved 1.0V transmit signal amplitude.                        */
    ADI_PHY_AN_TX_LEVEL_2P4V,             /*!< Resolved 2.4V transmit signal amplitude.                        */
} adi_phy_AnTxMode_e;

/*!
 * @brief Auto-negotiation master-slave advertisement.
 */
typedef enum
{
    ADI_PHY_AN_ADV_FORCED_MASTER = 0,       /*!< Force master.          */
    ADI_PHY_AN_ADV_FORCED_SLAVE,            /*!< Force slave.           */
    ADI_PHY_AN_ADV_PREFFERED_MASTER,        /*!< Preferred master.      */
    ADI_PHY_AN_ADV_PREFFERED_SLAVE,         /*!< Preferred slave.       */
} adi_phy_AnAdvMasterSlaveCfg_e;

/*!
 * @brief Auto-negotiation master-slave resolution.
 */
typedef enum
{
    ADI_PHY_AN_MS_RESOLUTION_NOT_RUN   = ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_NOT_RUN,    /*!< Auto-negotiation not run.              */
    ADI_PHY_AN_MS_RESOLUTION_FAULT     = ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_FAULT,      /*!< Auto-negotiation configuration fault.  */
    ADI_PHY_AN_MS_RESOLUTION_SLAVE     = ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_SLAVE,      /*!< Success, PHY configured as slave.      */
    ADI_PHY_AN_MS_RESOLUTION_MASTER    = ENUM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN_MS_MASTER,     /*!< Success, PHY configured as master.     */
} adi_phy_AnMsResolution_e;

/*!
 * @brief PHY reset types.
 */
typedef enum
{
    ADI_PHY_RESET_TYPE_SW = 0,      /*!< Software reset.                                        */
    ADI_PHY_RESET_TYPE_HW,          /*!< Hardware reset. Note this is currently not supported.  */
} adi_phy_ResetType_e;

/*!
 * @brief PHY interrupt events.
 */
typedef enum
{
    ADI_PHY_EVT_HW_RESET            = (1 << BITP_CRSM_IRQ_STATUS_CRSM_HRD_RST_IRQ_LH),
    ADI_PHY_EVT_SW_RESET            = (1 << BITP_CRSM_IRQ_STATUS_CRSM_SW_IRQ_LH),
    ADI_PHY_EVT_LINK_STAT_CHANGE    = (1 << (BITP_PHY_SUBSYS_IRQ_STATUS_LINK_STAT_CHNG_LH + 16)),
    ADI_PHY_EVT_MAC_IF_BUF          = (1 << (BITP_PHY_SUBSYS_IRQ_STATUS_MAC_IF_EBUF_ERR_IRQ_LH + 16)),
    ADI_PHY_EVT_CRSM_HW_ERROR       = ADI_PHY_CRSM_HW_ERROR,
} adi_phy_InterruptEvt_e;

/*!
 * @name PHY Capabilities
 * List of PHY capabilities.
 */
/** @{ */
#define ADI_PHY_CAP_NONE                    (0)         /*!< No PHY support (base value).                  */
#define ADI_PHY_CAP_TX_HIGH_LEVEL           (1 << 1)    /*!< PHY supports 10BASE-T1L 2.4V transmit level.  */
#define ADI_PHY_CAP_PMA_LOCAL_LOOPBACK      (1 << 2)    /*!< PHY supports 10BASE-T1L PMA local loopback.   */
/** @} */

/*!
* @brief Link status.
*/
typedef enum
{
    ADI_PHY_LINK_STATUS_DOWN        = (0),         /*!< Link down.  */
    ADI_PHY_LINK_STATUS_UP          = (1),         /*!< Link up.    */
} adi_phy_LinkStatus_e;

/*!
* @brief PHY loopback modes.
*/
typedef enum
{
    ADI_PHY_LOOPBACK_NONE = 0,                  /*!< No loopback (normal operation).                        */
    ADI_PHY_LOOPBACK_PCS,                       /*!< 10BASE-T1L PCS loopback.                               */
    ADI_PHY_LOOPBACK_PMA,                       /*!< 10BASE-T1L PMA loopback.                               */
    ADI_PHY_LOOPBACK_MACIF,                     /*!< MAC interface loopback, Tx data is looped back to Rx.  */
    ADI_PHY_LOOPBACK_MACIF_SUPPRESS_TX,         /*!< MAC interface loopback, Tx data is looped back to Rx.
                                                     Suppress Tx to the PHY.                                */
    ADI_PHY_LOOPBACK_MACIF_REMOTE,              /*!< MAC interface remote loopback. Rx data is looped back to Tx. */
    ADI_PHY_LOOPBACK_MACIF_REMOTE_SUPPRESS_RX,  /*!< MAC interface remote loopback. Rx data is looped back to Tx.
                                                     Suppress Rx to the MAC.                                      */
} adi_phy_LoopbackMode_e;

/*!
* @brief PHY test modes.
*/
typedef enum
{
    ADI_PHY_TEST_MODE_NONE = 0,                 /*!< No test mode (normal operation).                                           */
    ADI_PHY_TEST_MODE_1,                        /*!< Test mode 1 from IEEE 802.3cgTM-2019, subclause 146.5.2.                   */
    ADI_PHY_TEST_MODE_2,                        /*!< Test mode 2 from IEEE 802.3cgTM-2019, subclause 146.5.2.                   */
    ADI_PHY_TEST_MODE_3,                        /*!< Test mode 3 from IEEE 802.3cgTM-2019, subclause 146.5.2.                   */
    ADI_PHY_TEST_MODE_TX_DISABLE,               /*!< Transmite disable mode from IEEE 802.3cgTM-2019, subclause 45.2.1.186a.2.  */
} adi_phy_TestMode_e;

/*!
* @brief Link quality.
*/
typedef enum
{
    ADI_PHY_LINK_QUALITY_POOR = 0,
    ADI_PHY_LINK_QUALITY_MARGINAL,
    ADI_PHY_LINK_QUALITY_GOOD,
} adi_phy_LinkQuality_e;

/*!
* @brief Link quality.
*/
typedef struct
{
    uint16_t                mseVal;             /*!< Raw MSE value read from the MSE_VAL register.  */
    adi_phy_LinkQuality_e   linkQuality;        /*!< Link quality measure.                          */
    uint8_t                 sqi;                /*!< Signal Quality Indicator.                      */
} adi_phy_MseLinkQuality_t;

/*!
 * @brief PHY LED ports.
 */
typedef enum
{
    ADI_PHY_LED_0 = 0,                          /*!< LED 0. */
    ADI_PHY_LED_1,                              /*!< LED 1. */
} adi_phy_LedPort_e;

/*!
 * @brief PHY frame generator mode.
 */
typedef enum
{
    ADI_PHY_FRAME_GEN_MODE_BURST = 0,           /*!< Burst mode.        */
    ADI_PHY_FRAME_GEN_MODE_CONT,                /*!< Continuous mode.   */
} adi_phy_FrameGenMode_e;

/*!
 * @brief PHY frame generator payload.
 */
typedef enum
{
    ADI_PHY_FRAME_GEN_PAYLOAD_NONE          = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_NONE,                 /*!< No frames after current frame.     */
    ADI_PHY_FRAME_GEN_PAYLOAD_RANDOM        = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_RANDOM_PAYLOAD,       /*!< Random payload.                    */
    ADI_PHY_FRAME_GEN_PAYLOAD_0X00          = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0X00_PAYLOAD,         /*!< Payload of 0x00 repeated.          */
    ADI_PHY_FRAME_GEN_PAYLOAD_0XFF          = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0XFF_PAYLOAD,         /*!< Payload of 0xFF repeated.          */
    ADI_PHY_FRAME_GEN_PAYLOAD_0x55          = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_0X55_PAYLOAD,         /*!< Payload of 0x55 repeated.          */
    ADI_PHY_FRAME_GEN_PAYLOAD_DECR          = ENUM_FG_CNTRL_RSTRT_FG_CNTRL_FG_GEN_DECR_PAYLOAD,         /*!< Payload in decrementing bytes.     */
} adi_phy_FrameGenPayload_e;

/*!
 * @brief PHY frame check error counters.
 */
typedef struct
{
    uint16_t LEN_ERR_CNT;           /*!< Frame checker length error count.              */
    uint16_t ALGN_ERR_CNT;          /*!< Frame checker alignment error count.           */
    uint16_t SYMB_ERR_CNT;          /*!< Frame checker symbol error count.              */
    uint16_t OSZ_CNT;               /*!< Frame checker oversized frame error count.     */
    uint16_t USZ_CNT;               /*!< Frame checker undersized frame error count.    */
    uint16_t ODD_CNT;               /*!< Frame checker odd nibble frame count.          */
    uint16_t ODD_PRE_CNT;           /*!< Frame checker odd preamble packet counter.     */
    uint16_t FALSE_CARRIER_CNT;     /*!< Frame Checker False Carrier Count Register.    */
} adi_phy_FrameChkErrorCounters_t;

/*!
 * @brief PHY frame checker source.
 */
typedef enum
{
    ADI_PHY_FRAME_CHK_SOURCE_PHY = 0,           /*!< Frame checker source is the PHY.   */
    ADI_PHY_FRAME_CHK_SOURCE_MAC,               /*!< Frame checker source is the MAC.   */
} adi_phy_FrameChkSource_e;

/*!
* @brief PHY driver configuration.
*/
typedef struct
{
    uint32_t        addr;               /*!< Device address on the MDIO bus.                                                                    */
    void            *pDevMem;           /*!< Pointer to memory area used by the driver.                                                         */
    uint32_t        devMemSize;         /*!< Size of the memory used by the driver. Needs to be greater than or equal to #ADI_PHY_DEVICE_SIZE.  */
    bool            enableIrq;          /*!< Controls if the driver should   */
} adi_phy_DriverConfig_t;


/*!
 * @brief Auto-negotiation status.
 */
typedef struct {
    bool                            anComplete;         /*!< Auto-negotiation complete.                 */
    adi_phy_LinkStatus_e            anLinkStatus;       /*!< Link status.                               */
    adi_phy_AnMsResolution_e        anMsResolution;     /*!< Auto-negotiation master-slave resolution.  */
    adi_phy_AnTxMode_e              anTxMode;           /*!< Transmite operating mode.                  */
} adi_phy_AnStatus_t;

/*! 
 * @brief Pointer to function to use to read PHY registers. 
 */
typedef uint32_t (*HAL_ReadFn_t)    (uint8_t hwAddr, uint32_t regAddr, uint16_t *data);
/*! 
 * @brief Pointer to function to use to write PHY registers. 
 */
typedef uint32_t (*HAL_WriteFn_t)   (uint8_t hwAddr, uint32_t regAddr, uint16_t data);


/** @}*/

/*! @cond PRIVATE */

/*! Timeout for MDIO interface bringup after a powerup event or equivalent, in miliseconds. */
#define ADI_PHY_MDIO_POWERUP_TIMEOUT        (100)
/*! Timeout for system ready after a powerup event or equivalent, in miliseconds. */
#define ADI_PHY_SYS_RDY_TIMEOUT             (700)

/*! Timeout for MDIO interface bringup, in number of iterations. */
#define ADI_PHY_MDIO_POWERUP_ITER           (uint32_t)(ADI_PHY_MDIO_POWERUP_TIMEOUT * 1000 / ADI_HAL_MDIO_READ_DURATION)
/*! Timeout for system ready, in number of iterations. */
#define ADI_PHY_SYS_RDY_ITER                (uint32_t)(ADI_PHY_SYS_RDY_TIMEOUT * 1000 / ADI_HAL_MDIO_READ_DURATION)

/*! Number of retries allowed for software powerdown entry, in MDIO read count. */
#define ADI_PHY_SOFT_PD_ITER                (10)

/*! Number of SQI values. */
#define ADI_PHY_SQI_NUM_ENTRIES             (8)

/*! MSE threshold for POOR link quality. */
#define ADI_PHY_LINK_QUALITY_THR_POOR       (0x0766)
/*! MSE threshold for MARGINAL link quality. */
#define ADI_PHY_LINK_QUALITY_THR_MARGINAL   (0x05E1)

typedef struct
{
    uint32_t linkDropped;
} adi_phy_Stats_t;

typedef struct
{
    uint32_t                phyAddr;
    adi_phy_State_e         state;
    HAL_ReadFn_t            readFn;
    HAL_WriteFn_t           writeFn;
    adi_phy_LinkStatus_e    linkStatus;
    adi_eth_Callback_t      cbFunc;
    uint32_t                cbEvents;
    void                    *cbParam;
    void                    *adinDevice;
    uint32_t                irqMask;
    bool                    irqPending;
    adi_phy_Stats_t         stats;
} adi_phy_Device_t;

typedef struct
{
    adi_eth_Result_e (*Init)                    (adi_phy_Device_t **hPhyDevice, adi_phy_DriverConfig_t *cfg, void *adinDevice, HAL_ReadFn_t readFn, HAL_WriteFn_t writeFn);
    adi_eth_Result_e (*UnInit)                  (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*ReInitPhy)               (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*RegisterCallback)        (adi_phy_Device_t *hDevice, adi_eth_Callback_t cbFunc, uint32_t cbEvents, void *cbParam);
    adi_eth_Result_e (*ReadIrqStatus)           (adi_phy_Device_t *hDevice, uint32_t *status);
    adi_eth_Result_e (*EnterSoftwarePowerdown)  (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*ExitSoftwarePowerdown)   (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*GetSoftwarePowerdown)    (adi_phy_Device_t *hDevice, bool *enable);
    adi_eth_Result_e (*GetLinkStatus)           (adi_phy_Device_t *hDevice, adi_phy_LinkStatus_e *status);
    adi_eth_Result_e (*AnAdvTxMode)             (adi_phy_Device_t *hDevice, adi_phy_AnAdvTxMode_e txMode);
    adi_eth_Result_e (*AnAdvMstSlvCfg)          (adi_phy_Device_t *hDevice, adi_phy_AnAdvMasterSlaveCfg_e msCfg);
    adi_eth_Result_e (*AnEnable)                (adi_phy_Device_t *hDevice, bool enable);
    adi_eth_Result_e (*Renegotiate)             (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*GetAnStatus)             (adi_phy_Device_t *hDevice, adi_phy_AnStatus_t *status);
    adi_eth_Result_e (*Reset)                   (adi_phy_Device_t *hDevice, adi_phy_ResetType_e resetType);
    adi_eth_Result_e (*SetLoopbackMode)         (adi_phy_Device_t *hDevice, adi_phy_LoopbackMode_e loopbackMode);
    adi_eth_Result_e (*SetTestMode)             (adi_phy_Device_t *hDevice, adi_phy_TestMode_e testMode);
    adi_eth_Result_e (*LedEn)                   (adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, bool enable);
    adi_eth_Result_e (*LedBlinkTime)            (adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, uint32_t blinkOn, uint32_t blinkOff);
    adi_eth_Result_e (*GetCapabilities)         (adi_phy_Device_t *hDevice, uint16_t *capabilities);
    adi_eth_Result_e (*PHY_Read)                (adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t *data);
    adi_eth_Result_e (*PHY_Write)               (adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t data);
    adi_eth_Result_e (*GetMseLinkQuality)       (adi_phy_Device_t *hDevice, adi_phy_MseLinkQuality_t *mseLinkQuality);
    adi_eth_Result_e (*FrameGenEn)              (adi_phy_Device_t *hDevice, bool enable);
    adi_eth_Result_e (*FrameGenSetMode)         (adi_phy_Device_t *hDevice, adi_phy_FrameGenMode_e mode);
    adi_eth_Result_e (*FrameGenSetFrameCnt)     (adi_phy_Device_t *hDevice, uint32_t frameCnt);
    adi_eth_Result_e (*FrameGenSetFramePayload) (adi_phy_Device_t *hDevice, adi_phy_FrameGenPayload_e frameCtrl);
    adi_eth_Result_e (*FrameGenSetFrameLen)     (adi_phy_Device_t *hDevice, uint16_t frameLen);
    adi_eth_Result_e (*FrameGenSetIfgLen)       (adi_phy_Device_t *hDevice, uint16_t ifgLen);
    adi_eth_Result_e (*FrameGenRestart)         (adi_phy_Device_t *hDevice);
    adi_eth_Result_e (*FrameGenDone)            (adi_phy_Device_t *hDevice, bool *fgDone);
    adi_eth_Result_e (*FrameChkEn)              (adi_phy_Device_t *hDevice, bool enable);
    adi_eth_Result_e (*FrameChkSourceSelect)    (adi_phy_Device_t *hDevice, adi_phy_FrameChkSource_e source);
    adi_eth_Result_e (*FrameChkReadFrameCnt)    (adi_phy_Device_t *hDevice, uint32_t *cnt);
    adi_eth_Result_e (*FrameChkReadRxErrCnt)    (adi_phy_Device_t *hDevice, uint16_t *cnt);
    adi_eth_Result_e (*FrameChkReadErrorCnt)    (adi_phy_Device_t *hDevice, adi_phy_FrameChkErrorCounters_t *cnt);
} adi_phy_DriverEntry_t;

extern adi_phy_DriverEntry_t phyDriverEntry;

/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ADI_PHY_H */


