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

/** @addtogroup adin1110 ADIN1110 MAC-PHY Software Driver
 *  @{
 */

#ifndef ADIN1110_H
#define ADIN1110_H

#include "adi_phy.h"
#include "adi_mac.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*! ADIN1110 driver major version. */
#define ADIN1110_VERSION_MAJOR      (1)
/*! ADIN1110 driver minor version. */
#define ADIN1110_VERSION_MINOR      (1)
/*! ADIN1110 driver patch version. */
#define ADIN1110_VERSION_PATCH      (0)
/*! ADIN1110 driver extra version. */
#define ADIN1110_VERSION_EXTRA      (0)

/*! ADIN1110 driver version. */
#define ADIN1110_VERSION            ((ADIN1110_VERSION_MAJOR << 24) | \
                                     (ADIN1110_VERSION_MINOR << 16) | \
                                     (ADIN1110_VERSION_PATCH << 8) | \
                                     (ADIN1110_VERSION_EXTRA))

/*! ADIN1110 device descriptor structure, in bytes. */
#define ADIN1110_DEVICE_SIZE        (ADI_PHY_DEVICE_SIZE + ADI_MAC_DEVICE_SIZE)

/*! PHY address for the ADIN1110, this is fixed in hardware. */
#define ADIN1110_PHY_ADDR           (1)

/*! PHY interrupt mask. */
#define ADIN1110_PHY_IRQ_MASK       (ADI_PHY_CRSM_HW_ERROR | BITM_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN)

/*!
 * @brief ADIN1110 device identification.
 */
typedef struct
{
    union {
        struct {
            uint32_t    revNum      : 4;    /*!< Revision number.           */
            uint32_t    modelNum    : 6;    /*!< Model number.              */
            uint32_t    oui         : 22;   /*!< OUI.                       */
        };
        uint32_t phyId;
    };
    uint16_t        digRevNum;              /*!< Digital revision number.   */
    uint8_t         pkgType;                /*!< Package type.              */
} adin1110_DeviceId_t;

/*!
 * @brief ADIN1110 device driver structure.
 */
typedef struct
{
    adi_phy_Device_t        *pPhyDevice;    /*!< Pointer to the PHY driver instance. */
    adi_mac_Device_t        *pMacDevice;    /*!< Pointer to the MAC driver instance. */
    void                    *pUserContext;  /*!< Pointer to user context.            */
} adin1110_DeviceStruct_t;

/*!
 * @brief ADIN1110 driver configuration.
 */
typedef struct
{
    void            *pDevMem;           /*!< Pointer to memory area used by the driver.                                                             */
    uint32_t        devMemSize;         /*!< Size of the memory used by the driver. Needs to be greater than or equal to ADIN1110_DEVICE_SIZE.      */
    bool            fcsCheckEn;         /*!< Configure the driver to check FCS on frame receive to the host. Note this is a check for SPI 
                                         *   transaction integrity; the FCS is previously checked by the MAC hardware on frame receive to the MAC.  */
} adin1110_DriverConfig_t;

/*!
 * @brief ADIN1110 device handle.
 */
typedef adin1110_DeviceStruct_t*    adin1110_DeviceHandle_t;

adi_eth_Result_e    adin1110_Init                    (adin1110_DeviceHandle_t hDevice, adin1110_DriverConfig_t *pCfg);
adi_eth_Result_e    adin1110_UnInit                  (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_GetDeviceId             (adin1110_DeviceHandle_t hDevice, adin1110_DeviceId_t *pDevId);
adi_eth_Result_e    adin1110_Enable                  (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_Disable                 (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_Reset                   (adin1110_DeviceHandle_t hDevice, adi_eth_ResetType_e resetType);
adi_eth_Result_e    adin1110_SyncConfig              (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_GetLinkStatus           (adin1110_DeviceHandle_t hDevice, adi_eth_LinkStatus_e *linkStatus);
adi_eth_Result_e    adin1110_GetStatCounters         (adin1110_DeviceHandle_t hDevice, adi_eth_MacStatCounters_t *stat);
adi_eth_Result_e    adin1110_LedEn                   (adin1110_DeviceHandle_t hDevice, bool enable);
adi_eth_Result_e    adin1110_SetLoopbackMode         (adin1110_DeviceHandle_t hDevice, adi_phy_LoopbackMode_e loopbackMode);
adi_eth_Result_e    adin1110_SetTestMode             (adin1110_DeviceHandle_t hDevice, adi_phy_TestMode_e testMode);

adi_eth_Result_e    adin1110_AddAddressFilter        (adin1110_DeviceHandle_t hDevice, uint8_t *macAddr, uint8_t *macAddrMask, uint32_t priority);
adi_eth_Result_e    adin1110_ClearAddressFilter      (adin1110_DeviceHandle_t hDevice, uint32_t addrIndex);
adi_eth_Result_e    adin1110_SubmitTxBuffer          (adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc);
adi_eth_Result_e    adin1110_SubmitRxBuffer          (adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc);
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
adi_eth_Result_e    adin1110_SubmitRxBufferHp        (adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc);
#endif
adi_eth_Result_e    adin1110_SetPromiscuousMode      (adin1110_DeviceHandle_t hDevice, bool bFlag);
adi_eth_Result_e    adin1110_GetPromiscuousMode      (adin1110_DeviceHandle_t hDevice, bool *pFlag);
#if defined(SPI_OA_EN)
adi_eth_Result_e    adin1110_SetChunkSize            (adin1110_DeviceHandle_t hDevice, adi_mac_OaCps_e cps);
adi_eth_Result_e    adin1110_GetChunkSize            (adin1110_DeviceHandle_t hDevice, adi_mac_OaCps_e *pCps);
#endif
adi_eth_Result_e    adin1110_SetCutThroughMode       (adin1110_DeviceHandle_t hDevice, bool txcte, bool rxcte);
adi_eth_Result_e    adin1110_GetCutThroughMode       (adin1110_DeviceHandle_t hDevice, bool *pTxcte, bool *pRxcte);
adi_eth_Result_e    adin1110_SetFifoSizes            (adin1110_DeviceHandle_t hDevice, adi_mac_FifoSizes_t fifoSizes);
adi_eth_Result_e    adin1110_GetFifoSizes            (adin1110_DeviceHandle_t hDevice, adi_mac_FifoSizes_t *pFifoSizes);
adi_eth_Result_e    adin1110_ClearFifos              (adin1110_DeviceHandle_t hDevice, adi_mac_FifoClrMode_e clearMode);

adi_eth_Result_e    adin1110_TsEnable                (adin1110_DeviceHandle_t hDevice, adi_mac_TsFormat_e format);
adi_eth_Result_e    adin1110_TsClear                 (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_TsTimerStart            (adin1110_DeviceHandle_t hDevice, adi_mac_TsTimerConfig_t *pTimerConfig);
adi_eth_Result_e    adin1110_TsTimerStop             (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_TsSetTimerAbsolute      (adin1110_DeviceHandle_t hDevice, uint32_t seconds, uint32_t nanoseconds);
adi_eth_Result_e    adin1110_TsSyncClock             (adin1110_DeviceHandle_t hDevice, int64_t tError, uint64_t referenceTimeNsDiff, uint64_t localTimeNsDiff);
adi_eth_Result_e    adin1110_TsGetExtCaptTimestamp   (adin1110_DeviceHandle_t hDevice, adi_mac_TsTimespec_t *pCapturedTimespec);
adi_eth_Result_e    adin1110_TsGetEgressTimestamp    (adin1110_DeviceHandle_t hDevice, adi_mac_EgressCapture_e egressReg, adi_mac_TsTimespec_t *pCapturedTimespec);
adi_eth_Result_e    adin1110_TsConvert               (uint32_t timestampLowWord, uint32_t timestampHighWord, adi_mac_TsFormat_e format, adi_mac_TsTimespec_t *pTimespec);
int64_t             adin1110_TsSubtract              (adi_mac_TsTimespec_t *pTsA, adi_mac_TsTimespec_t *pTsB);

adi_eth_Result_e    adin1110_RegisterCallback        (adin1110_DeviceHandle_t hDevice, adi_eth_Callback_t cbFunc, adi_mac_InterruptEvt_e cbEvent);
adi_eth_Result_e    adin1110_SetUserContext          (adin1110_DeviceHandle_t hDevice, void *pContext);
void *              adin1110_GetUserContext          (adin1110_DeviceHandle_t hDevice);

adi_eth_Result_e    adin1110_WriteRegister           (adin1110_DeviceHandle_t hDevice, uint16_t regAddr, uint32_t regData);
adi_eth_Result_e    adin1110_ReadRegister            (adin1110_DeviceHandle_t hDevice, uint16_t regAddr, uint32_t *regData);
adi_eth_Result_e    adin1110_PhyWrite                (adin1110_DeviceHandle_t hDevice, uint32_t regAddr, uint16_t regData);
adi_eth_Result_e    adin1110_PhyRead                 (adin1110_DeviceHandle_t hDevice, uint32_t regAddr, uint16_t *regData);


adi_eth_Result_e    adin1110_GetMseLinkQuality      (adin1110_DeviceHandle_t hDevice, adi_phy_MseLinkQuality_t *mseLinkQuality);

adi_eth_Result_e    adin1110_FrameGenEn             (adin1110_DeviceHandle_t hDevice, bool enable);
adi_eth_Result_e    adin1110_FrameGenSetMode        (adin1110_DeviceHandle_t hDevice, adi_phy_FrameGenMode_e mode);
adi_eth_Result_e    adin1110_FrameGenSetFrameCnt    (adin1110_DeviceHandle_t hDevice, uint32_t frameCnt);
adi_eth_Result_e    adin1110_FrameGenSetFramePayload(adin1110_DeviceHandle_t hDevice, adi_phy_FrameGenPayload_e payload);
adi_eth_Result_e    adin1110_FrameGenSetFrameLen    (adin1110_DeviceHandle_t hDevice, uint16_t frameLen);
adi_eth_Result_e    adin1110_FrameGenSetIfgLen      (adin1110_DeviceHandle_t hDevice, uint16_t ifgLen);
adi_eth_Result_e    adin1110_FrameGenRestart        (adin1110_DeviceHandle_t hDevice);
adi_eth_Result_e    adin1110_FrameGenDone           (adin1110_DeviceHandle_t hDevice, bool *fgDone);

adi_eth_Result_e    adin1110_FrameChkEn             (adin1110_DeviceHandle_t hDevice, bool enable);
adi_eth_Result_e    adin1110_FrameChkSourceSelect   (adin1110_DeviceHandle_t hDevice, adi_phy_FrameChkSource_e source);
adi_eth_Result_e    adin1110_FrameChkReadFrameCnt   (adin1110_DeviceHandle_t hDevice, uint32_t *cnt);
adi_eth_Result_e    adin1110_FrameChkReadRxErrCnt   (adin1110_DeviceHandle_t hDevice, uint16_t *cnt);
adi_eth_Result_e    adin1110_FrameChkReadErrorCnt   (adin1110_DeviceHandle_t hDevice, adi_phy_FrameChkErrorCounters_t *cnt);

#ifdef __cplusplus
}
#endif

#endif /* ADIN1110_H */

/** @}*/

