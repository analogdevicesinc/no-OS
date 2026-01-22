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

#include "adin1110.h"

/** @addtogroup adin1110 ADIN1110 MAC-PHY Software Driver
 *  @{
 */

/*! @cond PRIVATE */

adin1110_DeviceHandle_t pDeviceHandle;


static uint32_t PhyWrite(uint8_t hwAddr, uint32_t regAddr, uint16_t data)
{
    return (uint32_t)macDriverEntry.PhyWrite(pDeviceHandle->pMacDevice , hwAddr, regAddr, data);
}

static uint32_t PhyRead(uint8_t hwAddr, uint32_t regAddr, uint16_t *data)
{
    return (uint32_t)macDriverEntry.PhyRead(pDeviceHandle->pMacDevice , hwAddr, regAddr, data);
}

/*! @endcond */

/////////////////////////////

/*!
 * @brief           ADIN1110 driver initialization.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      pCfg        Pointer to device configuration structure.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *                  - #ADI_ETH_INVALID_PARAM        Configured memory size too small.
 *                  - #ADI_ETH_UNSUPPORTED_DEVICE   Device not supported.
 *
 * @details         Initializes the driver and the ADIN1110 hardware.
 *                  The configuration structure includes a pointer to a memory chunk to be used for the driver structure.
 *
 *                  When the function finishes execution, the PHY device is in software powerdown state.
 *                  Use adin1110_Enable() to establish the link.
 *
 * @sa              adin1110_UnInit()
 */
adi_eth_Result_e adin1110_Init(adin1110_DeviceHandle_t hDevice, adin1110_DriverConfig_t *pCfg)
{
    adi_eth_Result_e        result = ADI_ETH_SUCCESS;
    adi_mac_DriverConfig_t  macDrvConfig;
    adi_phy_DriverConfig_t  phyDrvConfig;

    if (pCfg->devMemSize < ADIN1110_DEVICE_SIZE)
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    hDevice->pUserContext = NULL;

    /* Initialize the MAC configuration structure. */
    macDrvConfig.pDevMem = (void *)pCfg->pDevMem;
    macDrvConfig.devMemSize = ADI_MAC_DEVICE_SIZE;
    macDrvConfig.fcsCheckEn = pCfg->fcsCheckEn;

    /* Initialize the PHY configuration structure. */
    phyDrvConfig.pDevMem = (void *)((uint8_t *)pCfg->pDevMem + ADI_MAC_DEVICE_SIZE);
    phyDrvConfig.devMemSize = ADI_PHY_DEVICE_SIZE;
    phyDrvConfig.enableIrq  = false;

    pDeviceHandle = hDevice;

    ADI_HAL_INIT(hDevice);

    result = macDriverEntry.Init(&hDevice->pMacDevice, &macDrvConfig, (void *)hDevice);
    if (result == ADI_ETH_SUCCESS)
    {
        /* PHY address, used in internal MDIO accesses, is fixed in hardware. */
        phyDrvConfig.addr = ADIN1110_PHY_ADDR;
        result = phyDriverEntry.Init(&hDevice->pPhyDevice, &phyDrvConfig, hDevice, PhyRead, PhyWrite);

        hDevice->pMacDevice->phyAddr = phyDrvConfig.addr;
        hDevice->pMacDevice->phyIrqMask = ADIN1110_PHY_IRQ_MASK;
    }

end:
    return result;
}

/*!
 * @brief           ADIN1110 driver uninitialization.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Uninitializes the driver and the ADIN1110 hardware.
 *
 * @sa              adin1110_Init()
 */
adi_eth_Result_e adin1110_UnInit(adin1110_DeviceHandle_t hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = phyDriverEntry.UnInit(hDevice->pPhyDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = macDriverEntry.UnInit(hDevice->pMacDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    ADI_HAL_UNINIT(hDevice);

    hDevice->pPhyDevice = NULL;
    hDevice->pMacDevice = NULL;

end:
    return result;
}

/*!
 * @brief           ADIN1110 driver enable.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Enable the MAC-PHY operation by bringing PHY out of software powerdown and establishing link.
 *
 * @sa              adin1110_Disable()
 */
adi_eth_Result_e adin1110_Enable(adin1110_DeviceHandle_t hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = phyDriverEntry.ExitSoftwarePowerdown(hDevice->pPhyDevice);

    return result;
}

/*!
 * @brief           ADIN1110 driver disable.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Disable the MAC-PHY operation by bringing PHY into software powerdown.
 *
 * @sa              adin1110_Enable()
 */
adi_eth_Result_e adin1110_Disable(adin1110_DeviceHandle_t hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = phyDriverEntry.EnterSoftwarePowerdown(hDevice->pPhyDevice);

    return result;
}

/*!
 * @brief           Get device identity.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     pDevId      Device identity.
 * 
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         This function reads device identity from port 1 PHY.
 *
 */
adi_eth_Result_e adin1110_GetDeviceId(adin1110_DeviceHandle_t hDevice, adin1110_DeviceId_t *pDevId)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    result = adin1110_PhyRead(hDevice, ADDR_MMD1_DEV_ID1, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    pDevId->phyId = (val16 << 16);

    result = adin1110_PhyRead(hDevice, ADDR_MMD1_DEV_ID2, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    pDevId->phyId |= val16;

    result = adin1110_PhyRead(hDevice, ADDR_MGMT_PRT_PKG, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    pDevId->pkgType = val16;

    result = adin1110_PhyRead(hDevice, 0x1E900E, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    pDevId->digRevNum = val16 & 0xFF;

end:
    return result;
}

/*!
 * @brief           ADIN1110 reset.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      resetType   Reset type.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Software reset. Reset type can specify a MAC-only reset or combined MAC-PHY reset.
 *                  Resetting the MAC clears all user-configured MAC settings, and reinitializes interrupt masks to the driver defaults.
 *                  Before resuming frame transmission/reception, reconfigure the MAC as needed and then call adin1110_SyncConfig().
 *
 * @sa
 */
adi_eth_Result_e adin1110_Reset(adin1110_DeviceHandle_t hDevice, adi_eth_ResetType_e resetType)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = macDriverEntry.Reset(hDevice->pMacDevice, resetType);

    /* Re-initialize the PHY if needed */
    if ((result == ADI_ETH_SUCCESS) && (resetType == ADI_ETH_RESET_TYPE_MAC_PHY))
    {
        result = phyDriverEntry.ReInitPhy(hDevice->pPhyDevice);
    }

    return result;
}

/*!
 * @brief           ADIN1110 configuration synchronization.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         This is a requirement for both OPEN Alliance SPI and Generic SPI. It will set \ref adi_mac_Device_t.configSync to TRUE, 
 *                  which prevents modification of device configuration options via the APIs listed below until adin1110_Reset() or adin1110_Init() is called:
 *                  - adin1110_SetChunkSize()
 *                  - adin1110_SetFifoSizes()
 *                  - adin1110_SetCutThroughMode()
 *                  - adin1110_TsEnable()
 * 
 *                  For OPEN Alliance SPI, this function will aditionally set the bit CONFIG0.SYNC to indicate the device is configured.
 *                  CONFIG0.SYNC is cleared on reset, and the MAC device will not transmit or receive frames until CONFIG0.SYNC is set.
 *
 * @sa              adin1110_Reset()
 */
adi_eth_Result_e adin1110_SyncConfig(adin1110_DeviceHandle_t hDevice)
{
    return macDriverEntry.SyncConfig(hDevice->pMacDevice);
}

/*!
 * @brief           Read link status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     linkStatus  Link status.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         This function uses the link status information available
 *                  in the MAC STATUS register. It does not read from the PHY registers.
 *
 * @sa
 */
adi_eth_Result_e adin1110_GetLinkStatus(adin1110_DeviceHandle_t hDevice, adi_eth_LinkStatus_e *linkStatus)
{
    return macDriverEntry.GetLinkStatus(hDevice->pMacDevice, 0, linkStatus);

}

/*!
 * @brief           Read MAC statistics counters.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     stat        Statistics counters values.
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e adin1110_GetStatCounters(adin1110_DeviceHandle_t hDevice, adi_eth_MacStatCounters_t *stat)
{
    return macDriverEntry.GetStatCounters(hDevice->pMacDevice, 1, stat);
}

/*!
 * @brief           Enable/disable the status LED.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     enable      Enable/disable the status LED.
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e adin1110_LedEn(adin1110_DeviceHandle_t hDevice, bool enable)
{
  return phyDriverEntry.LedEn(hDevice->pPhyDevice, ADI_PHY_LED_0,  enable);
}

/*!
 * @brief           Set loopback mode.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      loopbackMode    Loopback mode.
 *
 * @details         The ADIN1110 features a number of loopback modes that can
 *                  be configured with this function:
 *
 *                  - #ADI_PHY_LOOPBACK_NONE
 *                  - #ADI_PHY_LOOPBACK_PCS
 *                  - #ADI_PHY_LOOPBACK_PMA
 *                  - #ADI_PHY_LOOPBACK_MACIF
 *                  - #ADI_PHY_LOOPBACK_MACIF_SUPPRESS_TX
 *                  - #ADI_PHY_LOOPBACK_MACIF_REMOTE
 *                  - #ADI_PHY_LOOPBACK_MACIF_REMOTE_SUPPRESS_RX
 *
 *                  To return to normal operation, set loopback mode to #ADI_PHY_LOOPBACK_NONE.
 */
adi_eth_Result_e adin1110_SetLoopbackMode(adin1110_DeviceHandle_t hDevice, adi_phy_LoopbackMode_e loopbackMode)
{
    return phyDriverEntry.SetLoopbackMode(hDevice->pPhyDevice, loopbackMode);
}

/*!
 * @brief           Set test mode.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      testMode        Test mode.
 *
 * @details         The ADIN1110 features a number of test modes that can
 *                  be configured with this function:
 *
 *                  - #ADI_PHY_TEST_MODE_NONE
 *                  - #ADI_PHY_TEST_MODE_1
 *                  - #ADI_PHY_TEST_MODE_2
 *                  - #ADI_PHY_TEST_MODE_3
 *                  - #ADI_PHY_TEST_MODE_TX_DISABLE
 *
 *                  To return to normal operation, set loopback mode to #ADI_PHY_TEST_MODE_NONE.
 *
 */
adi_eth_Result_e adin1110_SetTestMode(adin1110_DeviceHandle_t hDevice, adi_phy_TestMode_e testMode)
{
    return phyDriverEntry.SetTestMode(hDevice->pPhyDevice, testMode);
}

/*!
 * @brief           Set up MAC address filter and corresponding address rules.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      macAddr         MAC address.
 * @param [in]      macAddrMask     MAC address mask.
 * @param [in]      priority        FIFO priority.
 *
 * @details         Search for an available entry in the address filter table implemented in hardware. If no available entry is found,
 *                  it returns ADI_ETH_ADDRESS_FILTER_TABLE_FULL, otherwise the MAC address and corresponding priority
 *                  will be written to the hardware registers. The MAC address mask can be used to indicate how many bits, 
 *                  from left to right, the filter checks against the MAC address.
 *
 *                  If the address filter table is full, an entry can be made available using adin1110_ClearAddressFilter(). 
 *
 * @sa              adin1110_ClearAddressFilter()
 */
adi_eth_Result_e adin1110_AddAddressFilter(adin1110_DeviceHandle_t hDevice, uint8_t *macAddr, uint8_t *macAddrMask, uint32_t priority)
{
    adi_mac_AddressRule_t   addrRule;

    addrRule.VALUE16 = 0;
    /* Address rules only make sense if TO_HOST=1, because dropping frames is the default behaviour. */
    addrRule.TO_HOST = 1;
    addrRule.HOST_PRI = priority & (BITM_MAC_ADDR_FILT_UPR_N__HOST_PRI >> BITP_MAC_ADDR_FILT_UPR_N__HOST_PRI);
    addrRule.APPLY2PORT1 = 1;

    return macDriverEntry.AddAddressFilter(hDevice->pMacDevice, macAddr, macAddrMask, addrRule.VALUE16);
}

/*!
 * @brief           Clear MAC address filter entry from a specific index.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      addrIndex       Address table index.
 *
 * @details         Frees up an entry in the hardware address filter table by resetting the address rules.
 *
 * @sa              adin1110_AddAddressFilter()
 */
adi_eth_Result_e adin1110_ClearAddressFilter(adin1110_DeviceHandle_t hDevice, uint32_t addrIndex)
{
    return macDriverEntry.ClearAddressFilter(hDevice->pMacDevice, addrIndex);
}

/*!
 * @brief           Submit Tx buffer.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pBufDesc        Pointer to the buffer descriptor.
 *
 * @details         Submits a new Tx buffer to the Tx queue, to be scheduled for transmission.
 *                  The buffer information is supplied in the form of a buffer descriptor.
 *                  If the Tx queue is full, an error code is returned to the caller.
 *                  Once the buffer is successfully downloaded to the ADIN1110 Tx FIFO, the
 *                  callback configured as part of the buffer descriptor (if not NULL) will be called.
 *                  Note this indicates the buffer is in the Tx FIFO, not that the transmission was
 *                  successful!
 *
 * @sa
 */
adi_eth_Result_e adin1110_SubmitTxBuffer(adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc)
{
    adi_mac_FrameHeader_t   header;

    header.VALUE16 = 0x0000;
    header.EGRESS_CAPTURE = pBufDesc->egressCapt;

    /* For the ADIN1110, reference counter is always 1 */
    pBufDesc->refCount = 1;

    return macDriverEntry.SubmitTxBuffer(hDevice->pMacDevice, header, pBufDesc);
}

/*!
 * @brief           Submit Rx buffer.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pBufDesc        Pointer to the buffer descriptor.
 *
 * @details         Submits a new Rx buffer to the Rx queue. On receiving of a new frame, the buffer
 *                  descriptor will be populated with frame contents before a callback is used to notify the
 *                  application the buffer descriptor is available.
 *
 *                  If high priority queue is enabled by defining the symbol ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO,
 *                  this will submit the buffer to the low (normal) priority queue.
 *
 * @sa
 */
adi_eth_Result_e adin1110_SubmitRxBuffer(adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc)
{
    return macDriverEntry.SubmitRxBuffer(hDevice->pMacDevice, pBufDesc);
}

#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
/*!
 * @brief           Submit Rx buffer to the high priority queue.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pBufDesc        Pointer to the buffer descriptor.
 *
 * @details         Submits a new Rx buffer to the high priority Rx queue. On receiving of a new frame, the buffer
 *                  descriptor will be populated with frame contents before a callback is used to notify the
 *                  application the buffer descriptor is available.
 *
 * @sa
 */
adi_eth_Result_e adin1110_SubmitRxBufferHp(adin1110_DeviceHandle_t hDevice, adi_eth_BufDesc_t *pBufDesc)
{
    return macDriverEntry.SubmitRxBufferHp(hDevice->pMacDevice, pBufDesc);
}
#endif

#if defined(SPI_OA_EN)
/*!
 * @brief           Configure the chunk size used in OPEN Alliance frame transfers.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [out]     cps             Chunk payload selector (CPS) value to use.
 *
 * @details         Chunk size must be set before the configuration is synchronized.
 *                  Attempting to set chunk size after calling adin1110_SyncConfig() will return
 *                  an #ADI_ETH_CONFIG_SYNC_ERROR.
 *
 * @sa              adin1110_GetChunkSize()
 */
adi_eth_Result_e adin1110_SetChunkSize(adin1110_DeviceHandle_t hDevice, adi_mac_OaCps_e cps)
{
    return macDriverEntry.SetChunkSize(hDevice->pMacDevice, cps);
}

/*!
 * @brief           Get current chunk size used in OPEN Alliance frame transfers.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [out]     pCps            Returns the current chunk payload selector (CPS) value.
 *
 * @details
 *
 * @sa              adin1110_SetChunkSize()
 */
adi_eth_Result_e adin1110_GetChunkSize(adin1110_DeviceHandle_t hDevice, adi_mac_OaCps_e *pCps)
{
    return macDriverEntry.GetChunkSize(hDevice->pMacDevice, pCps);
}
#endif

/*!
 * @brief           Enable or disable cut-through mode.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      txcte           Enable cut-through in transmit.
 * @param [in]      rxcte           Enable cut-through in receive. Only supported in OPEN Alliance SPI.
 *
 * @details         Cut-through mode must be set before the configuration is synchronized.
 *                  Attempting to set cut-through mode after calling adin1110_SyncConfig() will return
 *                  an #ADI_ETH_CONFIG_SYNC_ERROR.
 *
 * @sa              adin1110_GetCutThroughMode()
 */
adi_eth_Result_e adin1110_SetCutThroughMode(adin1110_DeviceHandle_t hDevice, bool txcte, bool rxcte)
{
    return macDriverEntry.SetCutThroughMode(hDevice->pMacDevice, txcte, rxcte);
}

/*!
 * @brief           Get cut-through mode.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pTxcte          Returns true if cut-through is enabled in transmit.
 * @param [in]      pRxcte          Returns true if cut-through is enabled in receive. Only supported in OPEN Alliance SPI.
 *
 * @details
 *
 * @sa              adin1110_SetCutThroughMode()
 */
adi_eth_Result_e adin1110_GetCutThroughMode(adin1110_DeviceHandle_t hDevice, bool *pTxcte, bool *pRxcte)
{
    return macDriverEntry.GetCutThroughMode(hDevice->pMacDevice, pTxcte, pRxcte);
}

/*!
 * @brief           Set the sizes of the FIFOs.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      fifoSizes       Set of FIFO size enumerations to use.
 *
 * @details         FIFO sizes must be set before the configuration is synchronized.
 *                  Attempting to set FIFO sizes after calling adin1110_SyncConfig() will return
 *                  an #ADI_ETH_CONFIG_SYNC_ERROR.
 *
 * @sa              adin1110_GetFifoSizes()
 */
adi_eth_Result_e adin1110_SetFifoSizes(adin1110_DeviceHandle_t hDevice, adi_mac_FifoSizes_t fifoSizes)
{
    uint32_t writeVal;
    uint32_t totalSize = (2 * (uint32_t)fifoSizes.txSize) + (2 * (uint32_t)fifoSizes.rxLoSize) + (2 * (uint32_t)fifoSizes.rxHiSize);

    if (totalSize > ADI_MAC_FIFO_MAX_SIZE)
    {
        return ADI_ETH_FIFO_SIZE_ERROR;
    }
    else
    {
        writeVal = (fifoSizes.txSize << BITP_MAC_FIFO_SIZE_HTX_SIZE) |
                (fifoSizes.rxLoSize << BITP_MAC_FIFO_SIZE_P1_RX_LO_SIZE) |
                (fifoSizes.rxHiSize << BITP_MAC_FIFO_SIZE_P1_RX_HI_SIZE);

        return macDriverEntry.SetFifoSizes(hDevice->pMacDevice, writeVal);
    }
}

/*!
 * @brief           Get the current sizes of the FIFOs.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pFifoSizes      Returns the current set of FIFO size enumerations.
 *
 * @details
 *
 * @sa              adin1110_SetFifoSizes()
 */
adi_eth_Result_e adin1110_GetFifoSizes(adin1110_DeviceHandle_t hDevice, adi_mac_FifoSizes_t *pFifoSizes)
{
    adi_eth_Result_e    result  = ADI_ETH_SUCCESS;
    uint32_t            readVal;

    result = macDriverEntry.GetFifoSizes(hDevice->pMacDevice, &readVal);
    pFifoSizes->txSize    = (adi_mac_HtxFifoSize_e)((readVal & BITM_MAC_FIFO_SIZE_HTX_SIZE) >> BITP_MAC_FIFO_SIZE_HTX_SIZE);
    pFifoSizes->rxLoSize  = (adi_mac_RxFifoSize_e)((readVal & BITM_MAC_FIFO_SIZE_P1_RX_LO_SIZE) >> BITP_MAC_FIFO_SIZE_P1_RX_LO_SIZE);
    pFifoSizes->rxHiSize  = (adi_mac_RxFifoSize_e)((readVal & BITM_MAC_FIFO_SIZE_P1_RX_HI_SIZE) >> BITP_MAC_FIFO_SIZE_P1_RX_HI_SIZE);
    return result;
}

/*!
 * @brief           Clear receive and/or transmit FIFOs.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      clearMode       Sets which FIFOs will be cleared. Multiple options can be selected with an OR operation.
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e adin1110_ClearFifos(adin1110_DeviceHandle_t hDevice, adi_mac_FifoClrMode_e clearMode)
{
    return macDriverEntry.ClearFifos(hDevice->pMacDevice, clearMode);
}

/*!
 * @brief           Enable/disable promiscuous mode.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      bFlag           Enable promiscuous mode if true, disable if false.
 *
 * @details         Configures the MAC to forward to host all frames whose destination address
 *                  does not match any of of the addresses configured in the address filters.
 *                  It does not change the existing address filters configuration.
 *                  Also, frames considered invalid by the MAC are not forwarded to the host and
 *                  instead are dropped by the hardware.
 *
 * @sa              adin1110_GetPromiscuousMode()
 */
adi_eth_Result_e adin1110_SetPromiscuousMode(adin1110_DeviceHandle_t hDevice, bool bFlag)
{
    return macDriverEntry.SetPromiscuousMode(hDevice->pMacDevice, bFlag);
}

/*!
 * @brief           Get promiscuous mode status.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [out]     pFlag           Returns true if promiscuous mode is enabled, false if disable.
 *
 * @details
 *
 * @sa              adin1110_SetPromiscuousMode()
 */
adi_eth_Result_e adin1110_GetPromiscuousMode(adin1110_DeviceHandle_t hDevice, bool *pFlag)
{
    return macDriverEntry.GetPromiscuousMode(hDevice->pMacDevice, pFlag);
}

/*!
 * @brief           Enable timestamp counters and capture of receive timestamps.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      format          Timestamp format:
 *                                  - 32b free-running counter: 1 lsb = 1/120MHz = 8.33ns.
 *                                  - 32b 1588 timestamp: [31:30] seconds count, [29:0] nanoseconds count.
 *                                  - 64b 1588 timestamp: [63:32] seconds count, [31:30] zero, [29:0] nanoseconds count.
 *
 * @details         Timestamps must be configured before the configuration is synchronized.
 *                  Attempting to enable timestamps after calling adin1110_SyncConfig() will return
 *                  an #ADI_ETH_CONFIG_SYNC_ERROR.
 *
 * @sa
 */
adi_eth_Result_e adin1110_TsEnable(adin1110_DeviceHandle_t hDevice, adi_mac_TsFormat_e format)
{
    return macDriverEntry.TsEnable(hDevice->pMacDevice, format);
}

/*!
 * @brief           Synchronously clear all timestamp counters.
 *
 * @param [in]      hDevice         Device driver handle.
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e adin1110_TsClear(adin1110_DeviceHandle_t hDevice)
{
    return macDriverEntry.TsClear(hDevice->pMacDevice);
}

/*!
 * @brief           Configure and start TS_TIMER waveform generation.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      pTimerConfig    Pointer to a timer configuration structure.
 *
 * @details         It is required to run adin1110_TsEnable() to enable the timer block before running this function.
 *                  Use the timer configuration to set the period (in nanoseconds), duty cycle, idle state, and start time (in nanoseconds).
 *                  Start time must be greater than or equal to 16. Waveform generation will begin when the nanosecond counter 
 *                  is equal to the start time.
 *
 * @sa              adin1110_TsEnable()
 * @sa              adin1110_TsTimerStop()
 */
adi_eth_Result_e adin1110_TsTimerStart(adin1110_DeviceHandle_t hDevice, adi_mac_TsTimerConfig_t *pTimerConfig)
{
    return macDriverEntry.TsTimerStart(hDevice->pMacDevice, pTimerConfig);
}

/*!
 * @brief           Halt the TS_TIMER waveform generation.
 *
 * @param [in]      hDevice         Device driver handle.
 *
 * @details
 *
 * @sa              adin1110_TsTimerStart()
 */
adi_eth_Result_e adin1110_TsTimerStop(adin1110_DeviceHandle_t hDevice)
{
    return macDriverEntry.TsTimerStop(hDevice->pMacDevice);
}

/*!
 * @brief           Set the internal seconds and nanoseconds counters to the given values.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [in]      seconds         Seconds value to program to the seconds counter.
 * @param [in]      nanoseconds     Nanoseconds value to program to the nanoseconds counter.
 *
 * @details         It is required to run adin1110_TsEnable() to enable the timer block before running this function.
 *                  Nanoseconds value will automatically be coerced to the resolution and limits of the counter.
 *                  Use adin1110_TsSyncClock() for fine frequency adjustment.
 *
 * @sa              adin1110_TsEnable()
 * @sa              adin1110_TsSyncClock()
 */
adi_eth_Result_e adin1110_TsSetTimerAbsolute(adin1110_DeviceHandle_t hDevice, uint32_t seconds, uint32_t nanoseconds)
{
    return macDriverEntry.TsSetTimerAbsolute(hDevice->pMacDevice, seconds, nanoseconds);
}

/*!
 * @brief           Calculate and adjust the counter accumulator addend to adjust its frequency to zero out the given time difference.
 *
 * @param [in]      hDevice                 Device driver handle.
 * @param [in]      tError                  Time difference to correct, e.g. reference time minus local time. Positive or negative value.
 * @param [in]      referenceTimeNsDiff     Current time minus previous time from the reference clock source. Positive value.
 * @param [in]      localTimeNsDiff         Current time minus previous time from the local clock source. Positive value.
 *
 * @details         It is required to run adin1110_TsEnable() to enable the timer block before running this function.
 *                  Use adin1110_TsSetTimerAbsolute() for coarse timer adjustment to an absolute number of seconds and nanoseconds.
 *                  Use adin1110_TsSubtract() to calculate the difference between parsed timestamps stored in timespec structs.
 *
 * @sa              adin1110_TsEnable()
 * @sa              adin1110_TsSetTimerAbsolute()
 */
adi_eth_Result_e adin1110_TsSyncClock(adin1110_DeviceHandle_t hDevice, int64_t tError, uint64_t referenceTimeNsDiff, uint64_t localTimeNsDiff)
{
    return macDriverEntry.TsSyncClock(hDevice->pMacDevice, tError, referenceTimeNsDiff, localTimeNsDiff);
}

/*!
 * @brief           Retrieve and parse the TS_EXT_CAPT timestamp that is captured when the TS_CAPT input pin is asserted.
 *
 * @param [in]      hDevice             Device driver handle.
 * @param [in]      pCapturedTimespec   Pointer to a timespec struct to hold the parsed timestamp data.
 *
 * @details         It is required to run adin1110_TsEnable() to enable the timer block before running this function.
 *                  Uses the timestamp format that was set via adin1110_TsEnable() and adin1110_TsConvert() to parse.
 *
 * @sa              adin1110_TsEnable()
 * @sa              adin1110_TsConvert()
 */
adi_eth_Result_e adin1110_TsGetExtCaptTimestamp(adin1110_DeviceHandle_t hDevice, adi_mac_TsTimespec_t *pCapturedTimespec)
{
    return macDriverEntry.TsGetExtCaptTimestamp(hDevice->pMacDevice, pCapturedTimespec);
}

/*!
 * @brief           Retrieve and parse the TTSC* transmit timestamp that is captured if directed by the frame header.
 *
 * @param [in]      hDevice             Device driver handle.
 * @param [in]      egressReg           Enumeration value to select readback of TTSCA, TTSCB, or TTSCC.
 * @param [in]      pCapturedTimespec   Pointer to a timespec struct to hold the parsed timestamp data.
 *
 * @details         The MAC-PHY can be directed to capture a transmit timestamp by setting the \ref adi_eth_BufDesc_t.egressCapt field.
 *                  It is required to run adin1110_TsEnable() to enable the timer block before running this function.
 *                  Uses the timestamp format that was set via adin1110_TsEnable() and adin1110_TsConvert() to parse.
 *
 * @sa              adin1110_TsEnable()
 * @sa              adin1110_TsConvert()
 */
adi_eth_Result_e adin1110_TsGetEgressTimestamp(adin1110_DeviceHandle_t hDevice, adi_mac_EgressCapture_e egressReg, adi_mac_TsTimespec_t *pCapturedTimespec)
{
    return macDriverEntry.TsGetEgressTimestamp(hDevice->pMacDevice, egressReg, pCapturedTimespec);
}

/*!
 * @brief           Parses the a timestamp in a specific format.
 *
 * @param [in]      timestampLowWord    Lower 32 bits of timestamp.
 * @param [in]      timestampHighWord   Upper 32 bits of timestamp. Can be zero if using a 32-bit format.
 * @param [in]      format              Enumeration value to select the timestamp format to be used to parse the raw values.
 * @param [in]      pTimespec           Pointer to a timespec struct to hold the parsed timestamp data.
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e adin1110_TsConvert(uint32_t timestampLowWord, uint32_t timestampHighWord, adi_mac_TsFormat_e format, adi_mac_TsTimespec_t *pTimespec)
{
    return macDriverEntry.TsConvert(timestampLowWord, timestampHighWord, format, pTimespec);
}

/*!
 * @brief           Calculate the difference between two parsed timestamps in nanoseconds.
 *
 * @param [in]      pTsA        Pointer to timestamp value A.
 * @param [in]      pTsB        Pointer to timestamp value B.
 *
 * @returns         Nanoseconds value representing TsA - TsB.
 *
 * @details         Difference will be negative if TsB is greater than TsA.
 *
 * @sa
 */
int64_t adin1110_TsSubtract(adi_mac_TsTimespec_t *pTsA, adi_mac_TsTimespec_t *pTsB)
{
    return macDriverEntry.TsSubtract(pTsA, pTsB);
}

/*!
 * @brief           Register callback for driver events.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      cbFunc      Callback function.
 * @param [in]      cbEvent     Callback event.
 *
 * @details         This is separate from the callbacks configured for the Tx/Rx frames.
 *                  It notifies the application when configured events occur.
 *
 * @sa
 */
adi_eth_Result_e adin1110_RegisterCallback(adin1110_DeviceHandle_t hDevice, adi_eth_Callback_t cbFunc, adi_mac_InterruptEvt_e cbEvent)
{
    return macDriverEntry.RegisterCallback(hDevice->pMacDevice, cbFunc, cbEvent, (void *)hDevice);
}

/*!
 * @brief           Set user context for the device.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      pContext    Pointer to user context.
 *
 * @details         This is a user-defined variable to allow differentiation between instances
 *                  of the device driver and provide context information specific to a specific
 *                  instance. The driver does not use the user context information at any stage.
 *
 * @sa              adin1110_GetUserContext()
 */
adi_eth_Result_e adin1110_SetUserContext(adin1110_DeviceHandle_t hDevice, void *pContext)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if ((hDevice->pPhyDevice == NULL) || (hDevice->pPhyDevice->state == ADI_PHY_STATE_UNINITIALIZED) ||
        (hDevice->pMacDevice == NULL) || (hDevice->pMacDevice->state == ADI_MAC_STATE_UNINITIALIZED))
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    hDevice->pUserContext = pContext;

end:
    return result;
}

/*!
 * @brief           Get user context for the device.
 *
 * @param [in]      hDevice     Device driver handle.
 * @retval          Pointer to user context.
 *
 * @details         Returns the pointer to user context information stored in the device driver.
 *                  If not previously set using adin1110_SetUserContext(), or if driver not initialized,
 *                  it returns NULL.
 *
 * @sa              adin1110_SetUserContext()
 */
void *adin1110_GetUserContext(adin1110_DeviceHandle_t hDevice)
{
    void    *result = NULL;

    if (hDevice == NULL)
    {
        result = NULL;
        goto end;
    }

    if ((hDevice->pPhyDevice == NULL) || (hDevice->pPhyDevice->state == ADI_PHY_STATE_UNINITIALIZED) ||
        (hDevice->pMacDevice == NULL) || (hDevice->pMacDevice->state == ADI_MAC_STATE_UNINITIALIZED))
    {
        result = NULL;
        goto end;
    }

    result = hDevice->pUserContext;

end:
    return result;

}

/*!
 * @brief           Write to a MAC register.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      regAddr     Register address.
 * @param [in]      regData     Register data.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details
 *
 * @sa              adin1110_ReadRegister()
 */
adi_eth_Result_e adin1110_WriteRegister(adin1110_DeviceHandle_t hDevice, uint16_t regAddr, uint32_t regData)
{
    return macDriverEntry.WriteRegister(hDevice->pMacDevice, regAddr, regData);
}

/*!
 * @brief           Read from a MAC register.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      regAddr     Register address.
 * @param [out]     regData     Register data.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details
 *
 * @sa              adin1110_WriteRegister()
 */
adi_eth_Result_e adin1110_ReadRegister(adin1110_DeviceHandle_t hDevice, uint16_t regAddr, uint32_t *regData)
{
    return macDriverEntry.ReadRegister(hDevice->pMacDevice, regAddr, regData);
}

/*!
 * @brief           Write to a PHY register.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      regAddr     Register address.
 * @param [in]      regData     Register data.
 *
 * @details
 *
 * @sa              adin1110_PhyRead()
 */
adi_eth_Result_e adin1110_PhyWrite(adin1110_DeviceHandle_t hDevice, uint32_t regAddr, uint16_t regData)
{
    return macDriverEntry.PhyWrite(hDevice->pMacDevice, hDevice->pPhyDevice->phyAddr, regAddr, regData);
}

/*!
 * @brief           Read from a PHY register.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      regAddr     Register address.
 * @param [out]     regData     Register data.
 *
 * @details
 *
 * @sa              adin1110_PhyWrite()
 */
adi_eth_Result_e adin1110_PhyRead(adin1110_DeviceHandle_t hDevice, uint32_t regAddr, uint16_t *regData)
{
    return macDriverEntry.PhyRead(hDevice->pMacDevice, hDevice->pPhyDevice->phyAddr, regAddr, regData);
}


/*!
 * @brief           Get link quality measure based on the Mean Square Error (MSE) value.
 *
 * @param [in]      hDevice         Device driver handle.
 * @param [out]     mseLinkQuality  Link quality struct.
 *
 * @details
 *
 */
adi_eth_Result_e adin1110_GetMseLinkQuality(adin1110_DeviceHandle_t hDevice, adi_phy_MseLinkQuality_t *mseLinkQuality)
{
    return phyDriverEntry.GetMseLinkQuality(hDevice->pPhyDevice, mseLinkQuality);
}


/*!
 * @brief           Enable/disable frame generator.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      enable      Enable flag.
 *
 * @details         Enables/disables the frame generator based on the flag value.
 *
 */
adi_eth_Result_e adin1110_FrameGenEn(adin1110_DeviceHandle_t hDevice, bool enable)
{
    return phyDriverEntry.FrameGenEn(hDevice->pPhyDevice, enable);
}

/*!
 * @brief           Set frame generator mode.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      mode        Frame generator mode.
 *
 * @details         Sets one of the following frame generator modes:
 *                  - #ADI_PHY_FRAME_GEN_MODE_BURST: burst mode
 *                  - #ADI_PHY_FRAME_GEN_MODE_CONT: continuous mode
 *
 */
adi_eth_Result_e adin1110_FrameGenSetMode(adin1110_DeviceHandle_t hDevice, adi_phy_FrameGenMode_e mode)
{
    return phyDriverEntry.FrameGenSetMode(hDevice->pPhyDevice, mode);
}

/*!
 * @brief           Set frame generator frame counter.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      frameCnt    Frame count.
 *
 * @details         Updates the frame counter to the given value.
 *
 */
adi_eth_Result_e adin1110_FrameGenSetFrameCnt(adin1110_DeviceHandle_t hDevice, uint32_t frameCnt)
{
    return phyDriverEntry.FrameGenSetFrameCnt(hDevice->pPhyDevice, frameCnt);
}

/*!
 * @brief           Set frame generator payload.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      payload     Payload type.
 *
 * @details         The valid payloads are:
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_NONE
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_RANDOM
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_0X00
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_0XFF
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_0x55
 *                  - #ADI_PHY_FRAME_GEN_PAYLOAD_DECR
 *
 */
adi_eth_Result_e adin1110_FrameGenSetFramePayload(adin1110_DeviceHandle_t hDevice, adi_phy_FrameGenPayload_e payload)
{
    return phyDriverEntry.FrameGenSetFramePayload(hDevice->pPhyDevice, payload);
}

/*!
 * @brief           Set frame generator frame length.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      frameLen    Frame length.
 *
 * @details         Note the length of frames produced by the generator
 *                  has an additional overhead of 18 bytes: 6 bytes for source address,
 *                  6 bytes for destination address, 2 bytes for the length field and
 *                  4 bytes for the FCS field.
 *
 *                  If the frame length is set to a value less than 46 bytes, which
 *                  results in frames shorter than the minimum Ethernet frame
 *                  (64 bytes), the generator will not implement any padding.
 */
adi_eth_Result_e adin1110_FrameGenSetFrameLen(adin1110_DeviceHandle_t hDevice, uint16_t frameLen)
{
    return phyDriverEntry.FrameGenSetFrameLen(hDevice->pPhyDevice, frameLen);
}

/*!
 * @brief           Set frame generator interframe gap.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      ifgLen      Interframe gap.
 *
 * @details
 *
 */
adi_eth_Result_e adin1110_FrameGenSetIfgLen(adin1110_DeviceHandle_t hDevice, uint16_t ifgLen)
{
    return phyDriverEntry.FrameGenSetIfgLen(hDevice->pPhyDevice, ifgLen);
}

/*!
 * @brief           Restart frame generator.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @details         Restarts the frame generator. This is required
 *                  to enable frame generation. Before restart,
 *                  the function will read the FG_DONE bit to clear it,
 *                  in case it is still set from a previous run.
 *
 */
adi_eth_Result_e adin1110_FrameGenRestart(adin1110_DeviceHandle_t hDevice)
{
    return phyDriverEntry.FrameGenRestart(hDevice->pPhyDevice);
}

/*!
 * @brief           Read frame generator status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     fgDone      Frame generator status.
 *
 * @details         Read the FG_DONE bit to determine if the frame generation is complete.
 *
 */
adi_eth_Result_e adin1110_FrameGenDone(adin1110_DeviceHandle_t hDevice, bool *fgDone)
{
    return phyDriverEntry.FrameGenDone(hDevice->pPhyDevice, fgDone);
}

/*!
 * @brief           Enable/disable frame checker.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      enable      Enable flag.
 *
 * @details         Enables/disables the frame checker based on the flag value.
 *
 */
adi_eth_Result_e adin1110_FrameChkEn(adin1110_DeviceHandle_t hDevice, bool enable)
{
    return phyDriverEntry.FrameChkEn(hDevice->pPhyDevice, enable);
}

/*!
 * @brief           Select frame checker source.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      source      Frame checker source.
 *
 * @details         Sets one of the following frame checker sources:
 *                  - #ADI_PHY_FRAME_CHK_SOURCE_PHY
 *                  - #ADI_PHY_FRAME_CHK_SOURCE_MAC
 *
 */
 adi_eth_Result_e adin1110_FrameChkSourceSelect(adin1110_DeviceHandle_t hDevice, adi_phy_FrameChkSource_e source)
{
    return phyDriverEntry.FrameChkSourceSelect(hDevice->pPhyDevice, source);
}

/*!
 * @brief           Read frame checker frame count.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     cnt         Frame count.
 *
 * @details         Note the frame count value is latched when reading RX_CNT_ERR, therefore
 *                  a call to this function should be preceded by a call to adin1110_FrameChkReadRxErrCnt().
 *
 * @sa              adin1110_FrameChkReadRxErrCnt()
 */
adi_eth_Result_e adin1110_FrameChkReadFrameCnt(adin1110_DeviceHandle_t hDevice, uint32_t *cnt)
{
    return phyDriverEntry.FrameChkReadFrameCnt(hDevice->pPhyDevice, cnt);
}

/*!
 * @brief           Read frame checker receive errors.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     cnt         Error counter.
 *
 * @details         Note the frame counter/error counter values are latched when reading RX_CNT_ERR, therefore
 *                  a call to this function should preceded calls to adin1110_FrameChkReadFrameCnt()
 *                  or adin1110_FrameChkReadErrorCnt().
 *
 * @sa              adin1110_FrameChkReadFrameCnt()
 * @sa              adin1110_FrameChkReadErrorCnt()
 */
adi_eth_Result_e adin1110_FrameChkReadRxErrCnt(adin1110_DeviceHandle_t hDevice, uint16_t *cnt)
{
    return phyDriverEntry.FrameChkReadRxErrCnt(hDevice->pPhyDevice, cnt);
}

/*!
 * @brief           Read frame checker error counters.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]     cnt         Error counters.
 *
 * @details         Note the error counter values are latched when reading RX_CNT_ERR, therefore
 *                  a call to this function should be preceded by a call to adin1110_FrameChkReadRxErrCnt().
 *
 * @sa              adin1110_FrameChkReadRxErrCnt()
 */
adi_eth_Result_e adin1110_FrameChkReadErrorCnt(adin1110_DeviceHandle_t hDevice, adi_phy_FrameChkErrorCounters_t *cnt)
{
    return phyDriverEntry.FrameChkReadErrorCnt(hDevice->pPhyDevice, cnt);
}

/** @}*/


