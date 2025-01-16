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

#include "adi_mac.h"
#if defined(SPI_OA_EN)
#include "adi_spi_oa.h"
#else
#include "adi_spi_generic.h"
#endif

#define PSEUDO_MODULO(N, D) (((N) < (D)) ? (N) : ((N) - (D)))

#define DWORD               (4)

static adi_eth_Result_e     MAC_Init                    (adi_mac_Device_t **hDevice, adi_mac_DriverConfig_t *cfg, void *adinDevice);
static adi_eth_Result_e     MAC_UnInit                  (adi_mac_Device_t *hDevice);

static adi_eth_Result_e     MAC_Reset                   (adi_mac_Device_t *hDevice, adi_eth_ResetType_e resetType);
static adi_eth_Result_e     MAC_SyncConfig              (adi_mac_Device_t *hDevice);

static adi_eth_Result_e     MAC_RegisterCallback        (adi_mac_Device_t *hDevice, adi_eth_Callback_t cbFunc, adi_mac_InterruptEvt_e cbEvent, void *cbParam);

static adi_eth_Result_e     MAC_GetLinkStatus           (adi_mac_Device_t *hDevice, uint32_t port, adi_eth_LinkStatus_e *linkStatus);
static adi_eth_Result_e     MAC_GetStatCounters         (adi_mac_Device_t *hDevice, uint32_t port, adi_eth_MacStatCounters_t *stat);
static adi_eth_Result_e     MAC_AddAddressFilter        (adi_mac_Device_t *hDevice, uint8_t *macAddr, uint8_t *macAddrMask, uint16_t addrRule);
static adi_eth_Result_e     MAC_ClearAddressFilter      (adi_mac_Device_t *hDevice, uint32_t addrIndex);

static adi_eth_Result_e     MAC_SubmitTxBuffer          (adi_mac_Device_t *hDevice, adi_mac_FrameHeader_t header, adi_eth_BufDesc_t *pBufDesc);
static adi_eth_Result_e     MAC_SubmitRxBuffer          (adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc);
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
static adi_eth_Result_e     MAC_SubmitRxBufferHp        (adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc);
#endif

static adi_eth_Result_e     MAC_SetPromiscuousMode      (adi_mac_Device_t *hDevice, bool bFlag);
static adi_eth_Result_e     MAC_GetPromiscuousMode      (adi_mac_Device_t *hDevice, bool *pFlag);

static adi_eth_Result_e     MAC_PhyRead                 (adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t *data);
static adi_eth_Result_e     MAC_PhyWrite                (adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t data);

static adi_eth_Result_e     waitMdioReady               (adi_mac_Device_t *hDevice, uint16_t addrOffset);

static adi_eth_Result_e     macInit             (adi_mac_Device_t *hDevice);
static adi_eth_Result_e     waitDeviceReady     (adi_mac_Device_t *hDevice);
static adi_eth_Result_e     MAC_ProcessTxQueue  (adi_mac_Device_t *hDevice);

//#define DEBUG_MAC_RW

/****************************************/
/****************************************/
/***                                  ***/
/***            Rx/Tx Queues          ***/
/***                                  ***/
/****************************************/
/****************************************/

/* Implementation of Tx/Rx queues used by the MAC driver. */

void queueInit(adi_mac_Queue_t *pQueue, adi_mac_FrameStruct_t *pEntries, uint32_t numEntries)
{
    /* This is the _RAW value */
    pQueue->pEntries = pEntries;
    pQueue->numEntries = numEntries;
    pQueue->head = 0;
    pQueue->tail = 0;

    for (uint32_t i = 0; i < numEntries; i++)
    {
        pQueue->pEntries[i].header.VALUE16 = 0x0000;
        pQueue->pEntries[i].pBufDesc = NULL;
    }
}

static inline uint32_t queueCount(adi_mac_Queue_t *pQueue)
{
    uint32_t head = pQueue->head;
    uint32_t tail = pQueue->tail;
    uint32_t n = head + pQueue->numEntries - tail;

    return PSEUDO_MODULO(n, pQueue->numEntries);
}


uint32_t queueAvailable(adi_mac_Queue_t *pQueue)
{
    return (pQueue->numEntries - 1) - queueCount(pQueue);
}

bool queueIsFull(adi_mac_Queue_t *pQueue)
{
    return (pQueue->numEntries - 1) == queueCount(pQueue);
}

bool queueIsEmpty(adi_mac_Queue_t *pQueue)
{
    uint32_t head = pQueue->head;
    uint32_t tail = pQueue->tail;

    return head == tail;
}

void queueAdd(adi_mac_Queue_t *pQueue)
{
    uint32_t n = pQueue->head + 1;
    pQueue->head = PSEUDO_MODULO(n, pQueue->numEntries);
}

void queueRemove(adi_mac_Queue_t *pQueue)
{
    uint32_t n = pQueue->tail + 1;
    pQueue->tail = PSEUDO_MODULO(n, pQueue->numEntries);
}

/****************************************/
/****************************************/
/***                                  ***/
/***            IRQ Callback          ***/
/***                                  ***/
/****************************************/
/****************************************/

#if !defined(SPI_OA_EN)
static void macCallback(void *pCBParam, uint32_t Event, void *pArg)
{
    adi_mac_Device_t        *hDevice = (adi_mac_Device_t *)pCBParam;
    adi_eth_Result_e        result;
    ADI_MAC_STATUS0_t       status0;
    ADI_MAC_STATUS1_t       status1;
    ADI_MAC_STATUS0_t       status0Masked;
    ADI_MAC_STATUS1_t       status1Masked;
    ADI_MAC_STATUS1_t       status1Clear;
    uint32_t                irqMask1 = 0;
    adi_mac_TimestampRdy_t  timestampReady;
    uint16_t                val16;

    (void)Event;
    (void)pArg;

    /* Read interrupt status and immediately clear the status bits */
    result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS0, &status0.VALUE32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_STATUS0, status0.VALUE32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS1, &status1.VALUE32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    status0Masked.VALUE32 = status0.VALUE32 & ~hDevice->irqMask0;
    status1Masked.VALUE32 = status1.VALUE32 & ~hDevice->irqMask1;

    hDevice->statusRegisters.p1StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
    hDevice->statusRegisters.p1Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#if defined(ADIN2111)
    hDevice->statusRegisters.p2StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
    hDevice->statusRegisters.p2Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#endif

     /* If PHYINT is asserted, read PHY status registers (cleared on read) */
    if (hDevice->statusRegisters.status0Masked & BITM_MAC_STATUS0_PHYINT)
    {
        result = MAC_PhyRead(hDevice, 1, ADDR_CRSM_IRQ_STATUS, &val16);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Record state of CRSM_IRQ_STATUS */
        hDevice->statusRegisters.p1Status &= 0xFFFF0000;
        hDevice->statusRegisters.p1Status |= (val16 & 0x0000FFFF);
        hDevice->statusRegisters.p1StatusMasked &= 0xFFFF0000;
        hDevice->statusRegisters.p1StatusMasked |= (hDevice->statusRegisters.p1Status & hDevice->phyIrqMask & 0x0000FFFF);

        result = MAC_PhyRead(hDevice, 1, ADDR_PHY_SUBSYS_IRQ_STATUS, &val16);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Record state of PHY_SUBSYS_IRQ_STATUS */
        hDevice->statusRegisters.p1Status &= 0x0000FFFF;
        hDevice->statusRegisters.p1Status |= ((val16 << 16) & 0xFFFF0000);
        hDevice->statusRegisters.p1StatusMasked &= 0x0000FFFF;
        hDevice->statusRegisters.p1StatusMasked |= ((hDevice->statusRegisters.p1Status & hDevice->phyIrqMask & 0xFFFF0000) << 16);

    }

#if defined(ADIN2111)
    if (hDevice->statusRegisters.status1Masked & BITM_MAC_STATUS1_P2_PHYINT)
    {
        result = MAC_PhyRead(hDevice, 2, ADDR_CRSM_IRQ_STATUS, &val16);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Record state of CRSM_IRQ_STATUS */
        hDevice->statusRegisters.p2Status &= 0xFFFF0000;
        hDevice->statusRegisters.p2Status |= (val16 & 0x0000FFFF);
        hDevice->statusRegisters.p2StatusMasked &= 0xFFFF0000;
        hDevice->statusRegisters.p2StatusMasked |= (hDevice->statusRegisters.p2Status & hDevice->phyIrqMask & 0x0000FFFF);

        result = MAC_PhyRead(hDevice, 2, ADDR_PHY_SUBSYS_IRQ_STATUS, &val16);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Record state of PHY_SUBSYS_IRQ_STATUS */
        hDevice->statusRegisters.p2Status &= 0x0000FFFF;
        hDevice->statusRegisters.p2Status |= ((val16 << 16) & 0xFFFF0000);
        hDevice->statusRegisters.p2StatusMasked &= 0x0000FFFF;
        hDevice->statusRegisters.p2StatusMasked |= ((hDevice->statusRegisters.p2Status & hDevice->phyIrqMask & 0xFFFF0000) << 16);
    }

#endif

#if !defined(ADIN2111)

    /* Link status and link status change behave differently in ADIN2111 */
    if (status1Masked.LINK_CHANGE)
    {
        if (hDevice->cbFunc[ADI_MAC_EVT_LINK_CHANGE] != NULL)
        {
            adi_eth_LinkStatus_e    linkStatus = (adi_eth_LinkStatus_e)status1.P1_LINK_STATUS;
            hDevice->cbFunc[ADI_MAC_EVT_LINK_CHANGE](hDevice->cbParam[ADI_MAC_EVT_LINK_CHANGE], ADI_MAC_EVT_LINK_CHANGE, (void *)&linkStatus);
        }
    }
#endif

    /* Captured timestamp availability flags are different in ADIN2111 */
#if defined(ADIN2111)
    if ((status0Masked.TTSCAA | status0Masked.TTSCAB | status0Masked.TTSCAC) | (status1Masked.P2_TTSCAA | status1Masked.P2_TTSCAB | status1Masked.P2_TTSCAC))
#else
    if (status0Masked.TTSCAA | status0Masked.TTSCAB | status0Masked.TTSCAC)
#endif
    {
        if (hDevice->cbFunc[ADI_MAC_EVT_TIMESTAMP_RDY] != NULL)
        {
#if defined(ADIN2111)
            timestampReady.p1TimestampReadyA = (bool)status0Masked.TTSCAA;
            timestampReady.p1TimestampReadyB = (bool)status0Masked.TTSCAB;
            timestampReady.p1TimestampReadyC = (bool)status0Masked.TTSCAC;
            timestampReady.p2TimestampReadyA = (bool)status1Masked.P2_TTSCAA;
            timestampReady.p2TimestampReadyB = (bool)status1Masked.P2_TTSCAB;
            timestampReady.p2TimestampReadyC = (bool)status1Masked.P2_TTSCAC;
#else
            timestampReady.timestampReadyA = (bool)status0Masked.TTSCAA;
            timestampReady.timestampReadyB = (bool)status0Masked.TTSCAB;
            timestampReady.timestampReadyC = (bool)status0Masked.TTSCAC;
#endif
            hDevice->cbFunc[ADI_MAC_EVT_TIMESTAMP_RDY](hDevice->cbParam[ADI_MAC_EVT_TIMESTAMP_RDY], ADI_MAC_EVT_TIMESTAMP_RDY, (void *)&timestampReady);
        }
    }

    /* General status callback will fire even if callbacks were executed for specific status bits  */
    if (status0Masked.VALUE32 || status1Masked.VALUE32)
    {
        if (hDevice->cbFunc[ADI_MAC_EVT_STATUS] != NULL)
        {
            hDevice->cbFunc[ADI_MAC_EVT_STATUS](hDevice->cbParam[ADI_MAC_EVT_STATUS], ADI_MAC_EVT_STATUS, (void *)&hDevice->statusRegisters);
        }
    }

    irqMask1 = hDevice->irqMask1;
    status1Clear.VALUE32 = status1.VALUE32;
    if (!hDevice->pendingCtrl)
    {
        /* If RX_RDY is asserted, do not clear TX_RDY, as we prioritize reading frames */
#if defined(ADIN2111)
        if (status1.P1_RX_RDY || status1.P2_RX_RDY)
#else
        if (status1.P1_RX_RDY)
#endif
        {
#if ADI_PAUSE_RX_IF_NO_BUFFERS
            if (queueIsEmpty(hDevice->pRxQueue))
            {
                /* P*_RX_RDY interrupt sources stay asserted for as long as there is something  */
                /* to read in RxFIFO, but if there are no buffers to read to, the IRQ will keep */
                /* being asserted. To avoid a lockup, the P*_RX_RDY interrupts sources are      */
                /* masked until an Rx buffer is submitted to the driver.                        */
#if defined(ADIN2111)
                irqMask1 |= ((1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK) | (1 << BITP_MAC_IMASK1_P2_RX_RDY_MASK));
#else
                irqMask1 |= (1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK);
#endif
            }
            else
#endif
            {
                /* Received buffer will be read from the FIFO, do not clear TX_RDY yet. */
                status1Clear.TX_RDY = 0;
            }
        }
    }
    else
    {
        status1Clear.TX_RDY = 0;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_STATUS1, status1Clear.VALUE32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

#if ADI_PAUSE_RX_IF_NO_BUFFERS || ADI_EDGE_SENSITIVE_IRQ
#if ADI_EDGE_SENSITIVE_IRQ
    /* This will update the IRQ status mask and reassert the pending IRQ, if needed when */
    /* the IRQ is edge-triggered. It has no effect when the IRQ is level sensitive.      */
    result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK1, 0xFFFFFFFF);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
#endif

    if (ADI_EDGE_SENSITIVE_IRQ || (hDevice->irqMask1 != irqMask1))
    {
        result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK1, irqMask1);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }
#endif

    if (hDevice->pendingCtrl)
    {
        ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);
        goto end;
    }

#if defined(ADIN2111)
    if (!queueIsEmpty(hDevice->pRxQueue) && (status1.P1_RX_RDY || status1.P2_RX_RDY))
    {
        ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

        if (status1.P2_RX_RDY)
        {
            MAC_RecvFrame(hDevice, 1);
        }
        else
        {
            MAC_RecvFrame(hDevice, 0);

        }

        /* Make sure the priority frames are being read first */
        if (status1.P1_RX_RDY_HI)
        {
            MAC_RecvFrame(hDevice, 0);
        }
        else
        {
            if (status1.P2_RX_RDY_HI)
            {
                MAC_RecvFrame(hDevice, 1);
            }
            else
            {
                if (status1.P1_RX_RDY)
                {
                    MAC_RecvFrame(hDevice, 0);
                }
                else
                {
                    MAC_RecvFrame(hDevice, 1);
                }
            }
        }

    }
#else
    if (!queueIsEmpty(hDevice->pRxQueue) && (status1.P1_RX_RDY))
    {
        ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

        MAC_RecvFrame(hDevice, 0);
        status1.TX_RDY = 0;
    }
#endif
    else
    {
        if (status1.TX_RDY)
        {
            if (hDevice->state == ADI_MAC_STATE_READY)
            {
                result = MAC_ProcessTxQueue(hDevice);
                if (result != ADI_ETH_SUCCESS)
                {
                    goto end;
                }
            }
            else
            {
                ADI_HAL_SET_PENDING_IRQ(hDevice->adinDevice);
            }
        }
    }

    if (!queueIsEmpty(&hDevice->txQueue))
    {
        result = MAC_ProcessTxQueue(hDevice);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }

end:

    if (result == ADI_ETH_SUCCESS)
    {
        hDevice->irqMask1 = irqMask1;
    }

}
#else

static void macCallback(void *pCBParam, uint32_t Event, void *pArg)
{
    adi_mac_Device_t    *hDevice = (adi_mac_Device_t *)pCBParam;

    (void)Event;
    (void)pArg;

    oaIrqHandler(hDevice);

}
#endif

/****************************************/
/****************************************/
/***                                  ***/
/***             Driver API           ***/
/***                                  ***/
/****************************************/
/****************************************/

adi_eth_Result_e MAC_Init(adi_mac_Device_t **phDevice, adi_mac_DriverConfig_t *cfg, void *adinDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    adi_mac_Device_t    *hDevice;

    if (cfg->devMemSize < sizeof(adi_mac_Device_t))
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    /* Disable IRQ for situations when this is called after device initialization */
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    /* Implies state is uninitialized */
    memset(cfg->pDevMem, 0, cfg->devMemSize);

    *phDevice = (adi_mac_Device_t *)cfg->pDevMem;
    hDevice = *phDevice;
    hDevice->state = ADI_MAC_STATE_UNINITIALIZED;
    hDevice->adinDevice = adinDevice;
    hDevice->spiState = ADI_MAC_SPI_STATE_READY;
    hDevice->spiErr = 0;

    for (uint32_t i = 0; i < ADI_MAC_EVT_MAX; i++)
    {
        hDevice->cbFunc[i] = NULL;
        hDevice->cbParam[i] = NULL;
    }

    hDevice->pendingCtrl = false;

    hDevice->addrFilterActive = 0;
    hDevice->configSync = false;
    hDevice->fcsCheckEn = cfg->fcsCheckEn;
    hDevice->timestampFormat = ADI_MAC_TS_FORMAT_NONE;

    hDevice->statusRegisters.status0Masked = 0;
    hDevice->statusRegisters.status0 = 0;
    hDevice->statusRegisters.status1Masked = 0;
    hDevice->statusRegisters.status1 = 0;
    hDevice->statusRegisters.p1StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
    hDevice->statusRegisters.p1Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#if defined(ADIN2111)
    hDevice->statusRegisters.p2StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
    hDevice->statusRegisters.p2Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#endif

    /* Initialize both queues. */
    queueInit(&hDevice->txQueue, &hDevice->txQueueFrames[0], TX_QUEUE_NUM_ENTRIES_RAW);
    queueInit(&hDevice->rxQueueLp, &hDevice->rxQueueLpFrames[0], RX_QUEUE_NUM_ENTRIES_RAW);
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
    queueInit(&hDevice->rxQueueHp, &hDevice->rxQueueHpFrames[0], RX_QUEUE_NUM_ENTRIES_RAW);
#endif
    /* Default Rx queue is always the low priority */
    hDevice->pRxQueue = &hDevice->rxQueueLp;

    result = (adi_eth_Result_e)ADI_HAL_SPI_REGISTER_CALLBACK(hDevice->adinDevice, (HAL_Callback_t const *)spiCallback, hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* State needs to advance from uninitialized */
    hDevice->state = ADI_MAC_STATE_INITIALIZED;

#if defined(SPI_OA_EN)
    /* Initialize with maximum number of Tx credits */
    hDevice->oaTxc = 31;
    /* Initialize with no Rx chunks available */
    hDevice->oaRca = 0;
    /* The index in the buffer is 0 */
    hDevice->oaTxCurBufByteOffset = 0;
    hDevice->oaRxCurBufByteOffset = 0;
    hDevice->oaTxCurBufIdx = 0;
    hDevice->oaRxCurBufIdx = 0;
    hDevice->oaCps = 6;
    hDevice->oaMaxChunkCount = ADI_OA_MAX_CHUNK64_COUNT;
    hDevice->oaRxUseBackupBuf = false;
    hDevice->oaValidFlag = ADI_MAC_OA_VALID_FLAG_NONE;
    hDevice->oaTimestampSplit = false;
    hDevice->oaErrorStats.fdCount = 0;
    hDevice->oaErrorStats.invalidSvCount = 0;
    hDevice->oaErrorStats.invalidEvCount = 0;
    hDevice->oaErrorStats.ftrParityErrorCount = 0;
    hDevice->oaErrorStats.hdrParityErrorCount = 0;
    hDevice->oaErrorStats.syncErrorCount = 0;
#endif

    /* Perform a software reset of the MAC. */
    result = MAC_Reset(hDevice, ADI_ETH_RESET_TYPE_MAC_PHY);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    hDevice->state = ADI_MAC_STATE_READY;

end:
    return result;

}

static adi_eth_Result_e macInit(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            val32;

    /* Eventually we need to enable at least the PHY_INT in addition to the   */
    /* existing interrupt sources.   */

    /* IMASK0 */
    /* All interrupt sources are unmasked, writing them individually for clarity */
    /* Exception is the PHY interrupt, it is too early to enable it here as this */
    /* is run before the PHY initialization.                                     */
    hDevice->irqMask0 = 0xFFFFFFFF;
    hDevice->irqMask0 &= ~(BITM_MAC_IMASK0_TXPEM |
                           BITM_MAC_IMASK0_TXBOEM |
                           BITM_MAC_IMASK0_TXBUEM |
                           BITM_MAC_IMASK0_RXBOEM |
                           BITM_MAC_IMASK0_LOFEM |
                           BITM_MAC_IMASK0_HDREM |
                           BITM_MAC_IMASK0_RESETCM |
                           BITM_MAC_IMASK0_TXFCSEM |
                           BITM_MAC_IMASK0_CDPEM);

    result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK0, hDevice->irqMask0);

    /* IMASK1 */
    hDevice->irqMask1 = 0xFFFFFFFF;
    hDevice->irqMask1 &= ~(
#if !defined(SPI_OA_EN)
                            BITM_MAC_IMASK1_TX_RDY_MASK |
                            BITM_MAC_IMASK1_P1_RX_RDY_MASK |
#if defined(ADIN2111)
                            BITM_MAC_IMASK1_P2_RX_RDY_MASK |
#endif
#endif
                            BITM_MAC_IMASK1_P1_RX_IFG_ERR_MASK |
                            BITM_MAC_IMASK1_SPI_ERR_MASK |
                            BITM_MAC_IMASK1_RX_ECC_ERR_MASK |
                            BITM_MAC_IMASK1_TX_ECC_ERR_MASK
#if defined(ADIN2111)
                            | BITM_MAC_IMASK1_P2_TXFCSEM
#endif
                            );

    result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK1, hDevice->irqMask1);

    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Although the callback is registered here,  */
    result = (adi_eth_Result_e)ADI_HAL_REGISTER_CALLBACK(hDevice->adinDevice, (HAL_Callback_t const *)macCallback, hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS0, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS1, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Configure the MAC to generate and append FCS to the frame, or expect the FCS to be appended by the host */
    if (hDevice->fcsCheckEn)
    {
        result = (adi_eth_Result_e)ADI_HAL_FCS_INIT(hDevice->adinDevice);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_CONFIG0, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (hDevice->fcsCheckEn)
    {
        val32 |= BITM_MAC_CONFIG0_TXFCSVE;

    }
    else
    {
        val32 &= ~BITM_MAC_CONFIG0_TXFCSVE;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_CONFIG0, val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_CONFIG2, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (hDevice->fcsCheckEn)
    {
        val32 &= ~BITM_MAC_CONFIG2_CRC_APPEND;
    }
    else
    {
        val32 |= BITM_MAC_CONFIG2_CRC_APPEND;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_CONFIG2, val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

/*
 * @brief           MAC device uninitialization.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return 0 in case of success, positive error code otherwise.
 *
 */
adi_eth_Result_e MAC_UnInit(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    hDevice->state = ADI_MAC_STATE_UNINITIALIZED;

end:
    return result;
}

/*
 * @brief Reset MAC IC
 *
 * @param [in] dev - Pointer to the HW driver.
 *
 * @return 0 in case of success, positive error code otherwise.
*/
adi_eth_Result_e MAC_Reset(adi_mac_Device_t *hDevice, adi_eth_ResetType_e resetType)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            keys[2];
    uint32_t            retryCount;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    /* Disable IRQ here to prevent RESETC from being cleared in the IRQ handler */
    /* Because a reset needs to be followed up by reconfiguration of the device */
    /* and a call to SyncConfig(), the IRQ will be enabled by the latter so it  */
    /* can be left in a disabled state by this reset function.                  */
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    /* When the reset is executed as part of the initialization routine, here   */
    /* is the first register access, and it may not be fully reset/powered up.  */
    /* To prevent a premature exit with an error, the reset is repeated several */
    /* times if the register write is not successful. This is particularly      */
    /* important when using OPEN Alliance protocol, because the comparison      */
    /* between the control header and the echoed control header will fail       */
    /* immediately if the device is sending invalid data (all 0s) to the host.  */
    retryCount = 0;
    do
    {
        switch (resetType)
        {
            case ADI_ETH_RESET_TYPE_MAC_ONLY:
                keys[0] = RST_MAC_ONLY_KEY1;
                keys[1] = RST_MAC_ONLY_KEY2;

                /* No checking of results, see comment above. */
                result = MAC_WriteRegister(hDevice, ADDR_MAC_SOFT_RST, keys[0]);
                if (result == ADI_ETH_SUCCESS)
                {
                    result = MAC_WriteRegister(hDevice, ADDR_MAC_SOFT_RST, keys[1]);
                }

                break;

            case ADI_ETH_RESET_TYPE_MAC_PHY:

                /* No checking of results, see comment above. */
                result = MAC_WriteRegister(hDevice, ADDR_MAC_RESET, (1 << BITP_MAC_RESET_SWRESET));
                break;

            default:
                result = ADI_ETH_INVALID_PARAM;
                goto end;
        }
    } while ((result != ADI_ETH_SUCCESS) && (retryCount++ < ADI_MAC_IF_UP_MAX_RETRIES));

    if (result != ADI_ETH_SUCCESS)
    {
        result = ADI_ETH_SW_RESET_TIMEOUT;
        goto end;
    }

    if ((resetType == ADI_ETH_RESET_TYPE_MAC_ONLY) || (resetType == ADI_ETH_RESET_TYPE_MAC_PHY))
    {
        /* Wait for MAC reset to finish */
        result = waitDeviceReady(hDevice);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }
    else
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    /* After reset, the configuration is no longer synchronized */
    hDevice->configSync = false;
    /* Initialize IRQs and other registers */
    macInit(hDevice);

end:
    return result;
}


/*
 * @brief Register callback function for MAC.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return 0 in case of success, positive error code otherwise.
 *
 */
adi_eth_Result_e MAC_RegisterCallback(adi_mac_Device_t *hDevice, adi_eth_Callback_t cbFunc, adi_mac_InterruptEvt_e cbEvent, void *cbParam)
{
    adi_eth_Result_e            result = ADI_ETH_SUCCESS;
    bool                        irqMaskModified = false;

    hDevice->cbFunc[cbEvent] = cbFunc;
    hDevice->cbParam[cbEvent] = cbParam;

    switch (cbEvent)
    {
        case ADI_MAC_EVT_LINK_CHANGE:
             /* Link status and link status change behave differently in ADIN2111 */
#if !defined(ADIN2111)
            if (hDevice->irqMask1 & BITM_MAC_IMASK1_LINK_CHANGE_MASK)
            {
                hDevice->irqMask1 &= ~BITM_MAC_IMASK1_LINK_CHANGE_MASK;
                irqMaskModified = true;
            }
#endif
            break;

        case ADI_MAC_EVT_TIMESTAMP_RDY:
            if (hDevice->timestampFormat == ADI_MAC_TS_FORMAT_NONE)
            {
                result = ADI_ETH_NO_TS_FORMAT;
                goto end;
            }
#if defined(ADIN2111)
            if (hDevice->irqMask1 & (BITM_MAC_IMASK1_P2_TTSCAAM | BITM_MAC_IMASK1_P2_TTSCABM | BITM_MAC_IMASK1_P2_TTSCACM))
            {
                hDevice->irqMask1 &= ~(BITM_MAC_IMASK1_P2_TTSCAAM | BITM_MAC_IMASK1_P2_TTSCABM | BITM_MAC_IMASK1_P2_TTSCACM);
                irqMaskModified = true;
            }
#endif
            if (hDevice->irqMask0 & (BITM_MAC_IMASK0_TTSCAAM | BITM_MAC_IMASK0_TTSCABM | BITM_MAC_IMASK0_TTSCACM))
            {
                hDevice->irqMask0 &= ~(BITM_MAC_IMASK0_TTSCAAM | BITM_MAC_IMASK0_TTSCABM | BITM_MAC_IMASK0_TTSCACM);
                irqMaskModified = true;
            }
            break;

        default:
            irqMaskModified = false;
    }

    if (irqMaskModified)
    {
        result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK0, hDevice->irqMask0);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_WriteRegister(hDevice, ADDR_MAC_IMASK1, hDevice->irqMask1);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }

end:
    return result;
}

adi_eth_Result_e MAC_ReadRegister(adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t *regData)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint8_t             buf[ADI_MAC_SPI_ACCESS_SIZE];
    uint32_t            backup = 0;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    ADI_HAL_ENTER_CRITICAL_SECTION();
    if (hDevice->spiState != ADI_MAC_SPI_STATE_READY)
    {
        /* Function was called while a SPI transaction is ongoing (non-blocking, using DMA).        */
        /* In this situation we want to re-enable the IRQ, it will always be disabled at this point.*/
        backup = 1;
    }
    else
    {
        backup = ADI_HAL_GET_ENABLE_IRQ(hDevice->adinDevice);
    }
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);
    hDevice->pendingCtrl = true;
    ADI_HAL_EXIT_CRITICAL_SECTION();

#if !defined(SPI_OA_EN)
    result = MAC_Read(hDevice, regAddr, &buf, ADI_MAC_SPI_ACCESS_SIZE, true);

#if (ADI_MAC_SPI_ACCESS_SIZE == 2)
    *regData = HTON16((*(uint16_t *)buf));
#elif (ADI_MAC_SPI_ACCESS_SIZE == 4)
    *regData = HTON32((*(uint32_t *)buf));
#else
    result = ADI_ETH_PLACEHOLDER_ERROR;
#endif

#else

    (void)buf;

    hDevice->wnr = ADI_MAC_SPI_READ;
    hDevice->regAddr = regAddr;
    hDevice->pRegData = regData;
    hDevice->cnt = 1;

    hDevice->state = ADI_MAC_STATE_CONTROL_START;

    oaStateMachine(hDevice);

    while (hDevice->state != ADI_MAC_STATE_READY)
      ;

    if (hDevice->spiErr)
    {
        result = ADI_ETH_SPI_ERROR;
    }

#endif

end:

#ifdef DEBUG_MAC_RW
    printf("adin1110 red_reg      addr=0x%X val=0x%X\n", regAddr, *regData);
#endif
    hDevice->pendingCtrl = false;

    if (backup)
    {
        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
    }

    return result;
}

adi_eth_Result_e MAC_WriteRegister(adi_mac_Device_t *hDevice, uint16_t regAddr, uint32_t regData)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint8_t             buf[ADI_MAC_SPI_ACCESS_SIZE];
    uint32_t            backup = 0;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    ADI_HAL_ENTER_CRITICAL_SECTION();
    if (hDevice->spiState != ADI_MAC_SPI_STATE_READY)
    {
        /* Function was called while a SPI transaction is ongoing (non-blocking, using DMA).        */
        /* In this situation we want to re-enable the IRQ, it will always be disabled at this point.*/
        backup = 1;
    }
    else
    {
        backup = ADI_HAL_GET_ENABLE_IRQ(hDevice->adinDevice);
    }
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);
    hDevice->pendingCtrl = true;
    ADI_HAL_EXIT_CRITICAL_SECTION();

#ifdef DEBUG_MAC_RW
    printf("adin1110 \twrite_reg addr=0x%X val=0x%X\n", regAddr, regData);
#endif

#if !defined(SPI_OA_EN)

#if (ADI_MAC_SPI_ACCESS_SIZE == 2)
    *(uint16_t *)buf = HTON16(regData);
#elif (ADI_MAC_SPI_ACCESS_SIZE == 4)
    *(uint32_t *)buf = HTON32(regData);
#else
    result = ADI_ETH_PLACEHOLDER_ERROR;
    goto end;
#endif

    result = MAC_Write(hDevice, regAddr, &buf, ADI_MAC_SPI_ACCESS_SIZE, false);

#else

    (void)buf;

    hDevice->wnr = ADI_MAC_SPI_WRITE;
    hDevice->regAddr = regAddr;
    hDevice->pRegData = &regData;
    hDevice->cnt = 1;
    hDevice->state = ADI_MAC_STATE_CONTROL_START;

    oaStateMachine(hDevice);

    while (hDevice->state != ADI_MAC_STATE_READY)
      ;

    if (hDevice->spiErr)
    {
        result = ADI_ETH_SPI_ERROR;
    }

#endif

end:
    hDevice->pendingCtrl = false;
    if (backup)
    {
        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
    }

    return result;
}

adi_eth_Result_e waitMdioReady(adi_mac_Device_t *hDevice, uint16_t addrOffset)
{
    adi_eth_Result_e        result = ADI_ETH_SUCCESS;
    ADI_MAC_MDIOACC_0__t    mdioCmd;
    uint32_t                retryCount = 0;
    bool                    mdioRdy = false;

    do
    {
        result = MAC_ReadRegister(hDevice, addrOffset, &mdioCmd.VALUE32);
        if (result != ADI_ETH_SUCCESS)
        {
            break;
        }
        else if (mdioCmd.MDIO_TRDONE)
        {
            mdioRdy = true;
        }
    } while (((result != ADI_ETH_SUCCESS) || !mdioRdy) && (retryCount++ < ADI_MAC_MDIO_MAX_RETRIES));

    result = ((result == ADI_ETH_SUCCESS) && mdioRdy) ? ADI_ETH_SUCCESS: ADI_ETH_MDIO_TIMEOUT;

    return result;
}

adi_eth_Result_e waitDeviceReady(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            retryCount = 0;
    bool                resetDone = false;
    uint32_t            status0;
    bool                commOk = false;
    uint32_t            phyId;

    /* Poll PHYID register to establish the device has been brought up (powered up, out of reset). */
    retryCount = 0;
    commOk = false;
    while ((!commOk) && (retryCount++ < ADI_MAC_INIT_MAX_RETRIES))
    {
        result = MAC_ReadRegister(hDevice, ADDR_MAC_PHYID, &phyId);
        if ((result == ADI_ETH_SUCCESS) && (phyId == RSTVAL_MAC_PHYID))
        {
            commOk = true;
        }
    }

    if (!commOk)
    {
        result = ADI_ETH_COMM_TIMEOUT;
        goto end;
    }

    /* Now we can check RESETC without worrying about status0 comming back as all 0xF due to MAC-PHY still in reset. */
    while ((!resetDone) && (retryCount++ < ADI_MAC_IF_UP_MAX_RETRIES))
    {
        result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS0, &status0);
        if ((result == ADI_ETH_SUCCESS) && ((status0 & BITM_MAC_STATUS0_RESETC) == BITM_MAC_STATUS0_RESETC))
        {
            resetDone = true;
            result = MAC_WriteRegister(hDevice, ADDR_MAC_STATUS0, BITM_MAC_STATUS0_RESETC);
        }
    }
    if (!resetDone)
    {
        result = ADI_ETH_SW_RESET_TIMEOUT;
    }

end:
    return result;
}

/*
 * @brief PHY Register Read via SPI<->MDIO bridge
 *
 * @param [in] instance - Pointer to the HW driver.
 * @param [in] hwAddr - PHY MDIO Hardware address.
 * @param [in] regAddr - PHY MDIO Register address to read.
 * @param [out] data - Pointer to the data buffer.
 *
 * @return 0 in case of success, positive error code otherwise.
*/
adi_eth_Result_e MAC_PhyRead(adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t *regData)
{
    adi_eth_Result_e       result = ADI_ETH_SUCCESS;
    ADI_MAC_MDIOACC_0__t   mdioCmd;

    /* Use the first MDIO register for the address operation.   */
    /* MDIO Speed defaults to 2.5MHz per CONFIG2.MSPEED.        */
    mdioCmd.MDIO_DEVAD = DEVTYPE(regAddr);
    mdioCmd.MDIO_ST = ENUM_MAC_MDIOACC_N__MDIO_ST_CLAUSE45;
    mdioCmd.MDIO_PRTAD = hwAddr;
    mdioCmd.MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_ADDR;
    mdioCmd.MDIO_DATA = REGADDR(regAddr);
    mdioCmd.MDIO_TRDONE = 0;
    if((result = MAC_WriteRegister(hDevice, ADDR_MAC_MDIOACC_0_, mdioCmd.VALUE32)) != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Use the next MDIO register for the read operation.       */
    mdioCmd.MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_RD;
    mdioCmd.MDIO_DATA = 0;
    mdioCmd.MDIO_TRDONE = 0;
    if ((result = MAC_WriteRegister(hDevice, ADDR_MAC_MDIOACC_1_, mdioCmd.VALUE32)) != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = waitMdioReady(hDevice, ADDR_MAC_MDIOACC_1_);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, ADDR_MAC_MDIOACC_1_, &mdioCmd.VALUE32);
    *regData = mdioCmd.MDIO_DATA;

end:
    return result;
}

adi_eth_Result_e MAC_PhyWrite(adi_mac_Device_t *hDevice,  uint8_t hwAddr, uint32_t regAddr, uint16_t data)
{
    adi_eth_Result_e       result = ADI_ETH_SUCCESS;
    ADI_MAC_MDIOACC_0__t   mdioCmd;

    /* Use the first MDIO register for the address operation.   */
    /* MDIO Speed defaults to 2.5MHz per CONFIG2.MSPEED.        */
    mdioCmd.MDIO_DEVAD = DEVTYPE(regAddr);
    mdioCmd.MDIO_ST = ENUM_MAC_MDIOACC_N__MDIO_ST_CLAUSE45;
    mdioCmd.MDIO_PRTAD = hwAddr;
    mdioCmd.MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_ADDR;
    mdioCmd.MDIO_DATA = REGADDR(regAddr);
    mdioCmd.MDIO_TRDONE = 0;
    if((result = MAC_WriteRegister(hDevice, ADDR_MAC_MDIOACC_0_, mdioCmd.VALUE32)) != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Use the next MDIO register for the write operation.      */
    mdioCmd.MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_WR;
    mdioCmd.MDIO_DATA = data;
    mdioCmd.MDIO_TRDONE = 0;
    if ((result = MAC_WriteRegister(hDevice, ADDR_MAC_MDIOACC_1_, mdioCmd.VALUE32)) != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = waitMdioReady(hDevice, ADDR_MAC_MDIOACC_1_);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e MAC_SubmitTxBuffer(adi_mac_Device_t *hDevice, adi_mac_FrameHeader_t header, adi_eth_BufDesc_t *pBufDesc)
{
    adi_eth_Result_e          result = ADI_ETH_SUCCESS;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    if (pBufDesc == NULL)
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    if ((pBufDesc->trxSize < MIN_FRAME_SIZE) || (pBufDesc->trxSize > MAX_FRAME_SIZE))
    {
        result = ADI_ETH_PARAM_OUT_OF_RANGE;
        goto end;
    }

    /* If FCS is calculated on the host, the buffer needs 4 bytes more to store the FCS. */
    if (hDevice->fcsCheckEn)
    {
        if ((pBufDesc->bufSize < pBufDesc->trxSize) || (pBufDesc->bufSize - pBufDesc->trxSize < FCS_SIZE))
        {
            result = ADI_ETH_PARAM_OUT_OF_RANGE;
            goto end;
        }
    }

    if (queueIsFull(&hDevice->txQueue))
    {
        result = ADI_ETH_QUEUE_FULL;
        goto end;
    }

    hDevice->txQueue.pEntries[hDevice->txQueue.head].header.VALUE16 = header.VALUE16;

    if (hDevice->fcsCheckEn)
    {
        uint32_t fcs = ADI_HAL_FCS_CALCULATE(hDevice->adinDevice, &pBufDesc->pBuf[0], pBufDesc->trxSize);
        memcpy(&pBufDesc->pBuf[pBufDesc->trxSize], &fcs , FCS_SIZE);
        pBufDesc->trxSize += FCS_SIZE;
    }
    hDevice->txQueue.pEntries[hDevice->txQueue.head].pBufDesc = pBufDesc;

    queueAdd(&hDevice->txQueue);

end:

    if (!queueIsEmpty(&hDevice->txQueue))
    {
        if (hDevice->state == ADI_MAC_STATE_READY)
        {
            MAC_ProcessTxQueue(hDevice);
        }
    }

    return result;
}


adi_eth_Result_e MAC_SubmitRxBuffer(adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
#if !defined(SPI_OA_EN)
#if ADI_PAUSE_RX_IF_NO_BUFFERS
    uint32_t            rxRdyMask;
#endif
#endif

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    /* For Rx we only need to check for minimum size. */
    /* The buffer will always have the FCS appended. */
    if (pBufDesc->bufSize < MIN_FRAME_SIZE + FCS_SIZE)
    {
        result = ADI_ETH_PARAM_OUT_OF_RANGE;
        goto end;
    }

    if (queueIsFull(&hDevice->rxQueueLp))
    {
        result = ADI_ETH_QUEUE_FULL;
        goto end;
    }

    hDevice->rxQueueLp.pEntries[hDevice->rxQueueLp.head].header.VALUE16 = 0x0000;
    hDevice->rxQueueLp.pEntries[hDevice->rxQueueLp.head].pBufDesc = pBufDesc;

    queueAdd(&hDevice->rxQueueLp);

#if !defined(SPI_OA_EN)
#if ADI_PAUSE_RX_IF_NO_BUFFERS
    /* If RX_RDY interrupt sources were masked before   */
    /* due to no available Rx buffers, unmask them now. */
#if defined(ADIN2111)
    rxRdyMask = ((1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK) | (1 << BITP_MAC_IMASK1_P2_RX_RDY_MASK));
#else
    rxRdyMask = (1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK);
#endif

    if (hDevice->irqMask1 & rxRdyMask)
    {
        hDevice->irqMask1 &= ~rxRdyMask;
        ADI_HAL_SET_PENDING_IRQ(hDevice->adinDevice);
    }

    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
#endif
#else

    /* If idle, try to start a data transaction. */
    if (hDevice->state == ADI_MAC_STATE_READY)
    {
        hDevice->state = ADI_MAC_STATE_DATA_START;
        result = oaStateMachine(hDevice);
    }
#endif

end:
    return result;
}

#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
adi_eth_Result_e MAC_SubmitRxBufferHp(adi_mac_Device_t *hDevice, adi_eth_BufDesc_t *pBufDesc)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
#if !defined(SPI_OA_EN)
#if ADI_PAUSE_RX_IF_NO_BUFFERS
    uint32_t            rxRdyMask;
#endif
#endif

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    /* For Rx we only need to check for minimum size. */
    /* The buffer will always have the FCS appended. */
    if (pBufDesc->bufSize < MIN_FRAME_SIZE + FCS_SIZE)
    {
        result = ADI_ETH_PARAM_OUT_OF_RANGE;
        goto end;
    }

    if (queueIsFull(&hDevice->rxQueueHp))
    {
        result = ADI_ETH_QUEUE_FULL;
        goto end;
    }

    hDevice->rxQueueHp.pEntries[hDevice->rxQueueHp.head].header.VALUE16 = 0x0000;
    hDevice->rxQueueHp.pEntries[hDevice->rxQueueHp.head].pBufDesc = pBufDesc;

    queueAdd(&hDevice->rxQueueHp);

#if !defined(SPI_OA_EN)
#if ADI_PAUSE_RX_IF_NO_BUFFERS
    /* If RX_RDY interrupt sources were masked before   */
    /* due to no available Rx buffers, unmask them now. */
#if defined(ADIN2111)
    rxRdyMask = ((1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK) | (1 << BITP_MAC_IMASK1_P2_RX_RDY_MASK));
#else
    rxRdyMask = (1 << BITP_MAC_IMASK1_P1_RX_RDY_MASK);
#endif

    if (hDevice->irqMask1 & rxRdyMask)
    {
        hDevice->irqMask1 &= ~rxRdyMask;
        ADI_HAL_SET_PENDING_IRQ(hDevice->adinDevice);
    }

    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
#endif
#else

    /* If idle, try to start a data transaction. */
    if (hDevice->state == ADI_MAC_STATE_READY)
    {
        hDevice->state = ADI_MAC_STATE_DATA_START;
        result = oaStateMachine(hDevice);
    }
#endif

end:
    return result;
}
#endif

adi_eth_Result_e MAC_ProcessTxQueue(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e        result = ADI_ETH_SUCCESS;
    uint32_t                backup;

    ADI_HAL_ENTER_CRITICAL_SECTION(hDevice->adinDevice);
    backup = ADI_HAL_GET_ENABLE_IRQ(hDevice->adinDevice);
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);
    ADI_HAL_EXIT_CRITICAL_SECTION(hDevice->adinDevice);

    if (!queueIsEmpty(&hDevice->txQueue))
    {
        if (hDevice->state == ADI_MAC_STATE_READY)
        {
#if 0
            MXC_DEBUG_MSG("Tx h=%d t=%d\n",
                        hDevice->txQueue.head, hDevice->txQueue.tail);
#endif
            result = MAC_SendFrame(hDevice, &hDevice->txQueue.pEntries[hDevice->txQueue.tail]);
        }

        if (result != ADI_ETH_SUCCESS)
        {
            ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
        }
    }
    else
    {
        if (backup)
        {
            ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
        }
    }

    return result;
}

/*
 * @brief  MAC Get Statistics Counters
 *
 * @param [in] dev - Pointer to the HW driver.
 * @param [out] stat - statistics counters.
 *
 * @return 0 in case of success, positive error code otherwise.
*/
adi_eth_Result_e MAC_GetStatCounters(adi_mac_Device_t *hDevice, uint32_t port, adi_eth_MacStatCounters_t *stat)
{
    adi_eth_Result_e            result      = ADI_ETH_SUCCESS;
    uint16_t                    baseAddr    = 0;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

#if defined(SPI_OA_EN)
#if defined(ADIN2111)
    if (port)
    {
        baseAddr = ADDR_MAC_P2_RX_FRM_CNT - ADDR_MAC_P1_RX_FRM_CNT;
    }
    else
    {
        baseAddr = 0;
    }
#endif
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_FRM_CNT, &stat->RX_FRM_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_BCAST_CNT, &stat->RX_BCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_MCAST_CNT, &stat->RX_MCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_UCAST_CNT, &stat->RX_UCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_CRC_ERR_CNT, &stat->RX_CRC_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_ALGN_ERR_CNT, &stat->RX_ALGN_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_LS_ERR_CNT, &stat->RX_LS_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_PHY_ERR_CNT, &stat->RX_PHY_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_TX_FRM_CNT, &stat->TX_FRM_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_TX_BCAST_CNT, &stat->TX_BCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_TX_MCAST_CNT, &stat->TX_MCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_TX_UCAST_CNT, &stat->TX_UCAST_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_DROP_FULL_CNT, &stat->RX_DROP_FULL_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_ReadRegister(hDevice, baseAddr + ADDR_MAC_P1_RX_DROP_FILT_CNT, &stat->RX_DROP_FILT_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
#else
    /* Use burst read for Generic SPI. */
    adi_eth_MacStatCounters_t   locStat;

#if defined(ADIN2111)
    if (port)
    {
        baseAddr = ADDR_MAC_P2_RX_FRM_CNT;
    }
    else
    {
        baseAddr = ADDR_MAC_P1_RX_FRM_CNT;
    }
#else
        baseAddr = ADDR_MAC_P1_RX_FRM_CNT;
#endif

    result = MAC_Read(hDevice, baseAddr, &locStat, sizeof(adi_eth_MacStatCounters_t), false);

    stat->RX_FRM_CNT         = HTON32(locStat.RX_FRM_CNT);
    stat->RX_BCAST_CNT       = HTON32(locStat.RX_BCAST_CNT);
    stat->RX_MCAST_CNT       = HTON32(locStat.RX_MCAST_CNT);
    stat->RX_UCAST_CNT       = HTON32(locStat.RX_UCAST_CNT);
    stat->RX_CRC_ERR_CNT     = HTON32(locStat.RX_CRC_ERR_CNT);
    stat->RX_ALGN_ERR_CNT    = HTON32(locStat.RX_ALGN_ERR_CNT);
    stat->RX_LS_ERR_CNT      = HTON32(locStat.RX_LS_ERR_CNT);
    stat->RX_PHY_ERR_CNT     = HTON32(locStat.RX_PHY_ERR_CNT);
    stat->TX_FRM_CNT         = HTON32(locStat.TX_FRM_CNT);
    stat->TX_BCAST_CNT       = HTON32(locStat.TX_BCAST_CNT);
    stat->TX_MCAST_CNT       = HTON32(locStat.TX_MCAST_CNT);
    stat->TX_UCAST_CNT       = HTON32(locStat.TX_UCAST_CNT);
    stat->RX_DROP_FULL_CNT   = HTON32(locStat.RX_DROP_FULL_CNT);
    stat->RX_DROP_FILT_CNT   = HTON32(locStat.RX_DROP_FILT_CNT);
#endif

end:
    return result;
}

adi_eth_Result_e MAC_AddAddressFilter(adi_mac_Device_t *hDevice, uint8_t *macAddr, uint8_t *macAddrMask, uint16_t addrRule)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            addrIndex = ADI_MAC_ADDR_NUM_ENTRIES;
    uint32_t            maxAddrIndex;
    uint16_t            addrOffset;
    uint32_t            val32;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    maxAddrIndex = ADI_MAC_ADDR_NUM_ENTRIES;
    if (macAddrMask != NULL)
    {
        maxAddrIndex = ADI_MAC_ADDR_MASK_MAX;
    }

    for (uint32_t i = 0; i < maxAddrIndex; i++)
    {
        if (!(hDevice->addrFilterActive & (1 << i)))
        {
            addrIndex = i;
            break;
        }
    }

    if (addrIndex == maxAddrIndex)
    {
        result = ADI_ETH_ADDRESS_FILTER_TABLE_FULL;
        goto end;
    }

    addrOffset = addrIndex * (ADDR_MAC_ADDR_FILT_UPR_1_ - ADDR_MAC_ADDR_FILT_UPR_0_);

    /* Write MAC address and address rules. Note MAC_ADDR_FILT_UPR needs to be written before MAC_ADDR_FILT_LWR. */
    val32 = (addrRule << 16) | (macAddr[0] << 8) | macAddr[1];
    result = MAC_WriteRegister(hDevice, addrOffset + ADDR_MAC_ADDR_FILT_UPR_0_, val32);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val32 = (macAddr[2] << 24) | (macAddr[3] << 16) | (macAddr[4] << 8) | macAddr[5];
    result = MAC_WriteRegister(hDevice, addrOffset + ADDR_MAC_ADDR_FILT_LWR_0_, val32);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Write MAC address mask. Note MAC_ADDR_MSK_UPR needs to be written before MAC_ADDR_MSK_LWR. */
    /* Allow special case of NULL translating to a mask of all 1s. */
    if (macAddrMask == NULL)
    {
        val32 = 0xFFFF;
    }
    else
    {
        val32 = (macAddrMask[0] << 8) | macAddrMask[1];
    }
    result = MAC_WriteRegister(hDevice, addrOffset + ADDR_MAC_ADDR_MSK_UPR_0_, val32);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (macAddrMask == NULL)
    {
        val32 = 0xFFFFFFFF;
    }
    else
    {
        val32 = (macAddrMask[2] << 24) | (macAddrMask[3] << 16) | (macAddrMask[4] << 8) | macAddrMask[5];
    }
    result = MAC_WriteRegister(hDevice, addrOffset + ADDR_MAC_ADDR_MSK_LWR_0_, val32);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Mark the address filter entry as used. */
    hDevice->addrFilterActive |= (1 << addrIndex);

end:
    return result;
}

adi_eth_Result_e MAC_ClearAddressFilter(adi_mac_Device_t *hDevice, uint32_t addrIndex)
{
    adi_eth_Result_e        result = ADI_ETH_SUCCESS;
    uint16_t                addrOffset;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    if (hDevice->state == ADI_MAC_STATE_UNINITIALIZED)
    {
        result = ADI_ETH_DEVICE_UNINITIALIZED;
        goto end;
    }

    if (addrIndex >= ADI_MAC_ADDR_NUM_ENTRIES)
    {
        result = ADI_ETH_PARAM_OUT_OF_RANGE;
        goto end;
    }

    addrOffset = addrIndex * (ADDR_MAC_ADDR_FILT_UPR_1_ - ADDR_MAC_ADDR_FILT_UPR_0_);

    result = MAC_WriteRegister(hDevice, addrOffset + ADDR_MAC_ADDR_FILT_UPR_0_, 0);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    hDevice->addrFilterActive &= ~(1 << addrIndex);

end:

    return result;
}

adi_eth_Result_e MAC_GetLinkStatus(adi_mac_Device_t *hDevice, uint32_t port, adi_eth_LinkStatus_e *linkStatus)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

#if defined(ADIN2111)
    /* For the ADIN2111 we need to read link status from the PHY registers */
    result = ADI_ETH_NOT_IMPLEMENTED;
#else
    uint32_t            val32;

    (void)port;

    result = MAC_ReadRegister(hDevice, ADDR_MAC_STATUS1, &val32);
    if(result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    *linkStatus = (adi_eth_LinkStatus_e)((val32 & BITM_MAC_STATUS1_P1_LINK_STATUS) >> BITP_MAC_STATUS1_P1_LINK_STATUS);

end:
#endif
    return result;
}

#if defined(SPI_OA_EN)
adi_eth_Result_e MAC_SetChunkSize(adi_mac_Device_t *hDevice, adi_mac_OaCps_e cps)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr = ADDR_MAC_CONFIG0;
    uint32_t            mask = BITM_MAC_CONFIG0_CPS;
    uint32_t            val;

    if (hDevice->configSync)
    {
        result = ADI_ETH_CONFIG_SYNC_ERROR;
        goto end;
    }

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val = (val & ~mask) | (cps & mask);

    result = MAC_WriteRegister(hDevice, addr, val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    hDevice->oaMaxChunkCount = (cps == ADI_MAC_OA_CPS_64BYTE) ? ADI_OA_MAX_CHUNK64_COUNT : ADI_OA_MAX_CHUNK_COUNT;
    hDevice->oaCps = (uint32_t)cps;
end:
    return result;
}

adi_eth_Result_e MAC_GetChunkSize(adi_mac_Device_t *hDevice, adi_mac_OaCps_e *pCps)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr = ADDR_MAC_CONFIG0;
    uint32_t            mask = BITM_MAC_CONFIG0_CPS;
    uint32_t            val;

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val &= mask;
    *pCps = (adi_mac_OaCps_e)val;

    if (val != hDevice->oaCps)
    {
        result = ADI_ETH_VALUE_MISMATCH_ERROR;
        goto end;
    }

end:
    return result;
}
#endif

adi_eth_Result_e MAC_SetCutThroughMode(adi_mac_Device_t *hDevice, bool txcte, bool rxcte)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr = ADDR_MAC_CONFIG0;
    uint32_t            setMask = (rxcte << BITP_MAC_CONFIG0_RXCTE) | (txcte << BITP_MAC_CONFIG0_TXCTE);
    uint32_t            clearMask = (BITM_MAC_CONFIG0_RXCTE | BITM_MAC_CONFIG0_TXCTE);
    uint32_t            val;

#if !defined(SPI_OA_EN)
    /* Cut-through on receive is not supported in hardware for Generic SPI */
    if (rxcte)
    {
        return ADI_ETH_NOT_IMPLEMENTED;
    }
#endif

    if (hDevice->configSync)
    {
        result = ADI_ETH_CONFIG_SYNC_ERROR;
        goto end;
    }

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val = (val & ~clearMask) | setMask;

    result = MAC_WriteRegister(hDevice, addr, val);

end:
    return result;
}

adi_eth_Result_e MAC_GetCutThroughMode(adi_mac_Device_t *hDevice, bool *pTxcte, bool *pRxcte)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr    = ADDR_MAC_CONFIG0;
    uint32_t            rxMask  = BITM_MAC_CONFIG0_RXCTE;
    uint32_t            txMask  = BITM_MAC_CONFIG0_TXCTE;
    uint32_t            val;

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    *pRxcte = (bool)(val & rxMask);
    *pTxcte = (bool)(val & txMask);

end:
    return result;
}

adi_eth_Result_e MAC_SetFifoSizes(adi_mac_Device_t *hDevice, uint32_t writeVal)
{
    adi_eth_Result_e    result  = ADI_ETH_SUCCESS;

    if (hDevice->configSync)
    {
        result = ADI_ETH_CONFIG_SYNC_ERROR;
        goto end;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_FIFO_SIZE, writeVal);

end:
    return result;
}

adi_eth_Result_e MAC_GetFifoSizes(adi_mac_Device_t *hDevice, uint32_t *readVal)
{
    return MAC_ReadRegister(hDevice, ADDR_MAC_FIFO_SIZE, readVal);
}

adi_eth_Result_e MAC_ClearFifos(adi_mac_Device_t *hDevice, adi_mac_FifoClrMode_e clearMode)
{
    adi_eth_Result_e    result  = ADI_ETH_SUCCESS;
    uint16_t            addr    = ADDR_MAC_FIFO_CLR;

    result = MAC_WriteRegister(hDevice, addr, (uint32_t)clearMode);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e MAC_SetPromiscuousMode(adi_mac_Device_t *hDevice, bool bFlag)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr = ADDR_MAC_CONFIG2;
    uint32_t            mask = BITM_MAC_CONFIG2_P1_FWD_UNK2HOST;;
    uint32_t            val;

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (bFlag)
    {
        val |= mask;
    }
    else
    {
        val &= ~mask;
    }

    result = MAC_WriteRegister(hDevice, addr, val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e MAC_GetPromiscuousMode(adi_mac_Device_t *hDevice, bool *pFlag)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            addr = ADDR_MAC_CONFIG2;
    uint32_t            mask = BITM_MAC_CONFIG2_P1_FWD_UNK2HOST;;
    uint32_t            val;

    result = MAC_ReadRegister(hDevice, addr, &val);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    *pFlag = (val & mask) ? true: false;

end:
    return result;
}


adi_eth_Result_e MAC_TsEnable(adi_mac_Device_t *hDevice, adi_mac_TsFormat_e format)
{
    adi_eth_Result_e     result = ADI_ETH_SUCCESS;
    uint32_t             tsCfg;
    uint32_t             config0;

    if (hDevice->configSync)
    {
        result = ADI_ETH_CONFIG_SYNC_ERROR;
        goto end;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_CFG, &tsCfg);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_CONFIG0, &config0);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    config0 |= BITM_MAC_CONFIG0_FTSE;
    switch(format)
    {
        case ADI_MAC_TS_FORMAT_NONE:
            tsCfg &= ~BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT;
            config0 &= ~BITM_MAC_CONFIG0_FTSE;
            break;
        case ADI_MAC_TS_FORMAT_32B_FREE:
            tsCfg |= BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT;
            config0 &= ~BITM_MAC_CONFIG0_FTSS;
            break;
        case ADI_MAC_TS_FORMAT_32B_1588:
            tsCfg &= ~BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT;
            config0 &= ~BITM_MAC_CONFIG0_FTSS;
            break;
        case ADI_MAC_TS_FORMAT_64B_1588:
            tsCfg &= ~BITM_MAC_TS_CFG_TS_CAPT_FREE_CNT;
            config0 |= BITM_MAC_CONFIG0_FTSS;
            break;
        default:
            result = ADI_ETH_INVALID_PARAM;
            goto end;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_CONFIG0, config0);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    tsCfg |= BITM_MAC_TS_CFG_TS_EN;
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_CFG, tsCfg);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    hDevice->timestampFormat = format;

end:
    return result;
}

adi_eth_Result_e MAC_TsClear(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e     result = ADI_ETH_SUCCESS;
    uint32_t             tsCfg;

    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_CFG, &tsCfg);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    tsCfg |= BITM_MAC_TS_CFG_TS_CLR;
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_CFG, tsCfg);

end:
    return result;
}

adi_eth_Result_e MAC_TsTimerStart(adi_mac_Device_t *hDevice, adi_mac_TsTimerConfig_t *pTimerConfig)
{
    adi_eth_Result_e     result = ADI_ETH_SUCCESS;
    uint32_t             remainder16;
    uint32_t             timeHi;
    uint32_t             timeLo;
    uint32_t             tsCfg;

    /* TS_TIMER register values must be greater than or equal to ADI_MAC_TS_MIN_NS */
    if (pTimerConfig->startTimeNs < ADI_MAC_TS_MIN_NS)
    {
        pTimerConfig->startTimeNs = ADDR_MAC_TS_NS_CNT;
    }
    if (pTimerConfig->periodNs < (2 * ADI_MAC_TS_MIN_NS))
    {
        pTimerConfig->periodNs = 2 * ADI_MAC_TS_MIN_NS;
    }

    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_CFG, &tsCfg);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (!(tsCfg & BITM_MAC_TS_CFG_TS_EN))
    {
        result = ADI_ETH_TS_COUNTERS_DISABLED;
        goto end;
    }

    tsCfg = (tsCfg & ~BITM_MAC_TS_CFG_TS_TIMER_DEF) | ((uint32_t)pTimerConfig->idleState << BITP_MAC_TS_CFG_TS_TIMER_DEF);
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_CFG, tsCfg);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Need to use quantization error correction if the period is not a multiple of ADI_MAC_TS_MIN_NS */
    remainder16 = pTimerConfig->periodNs & ADI_MAC_TS_QE_MASK;
    if (remainder16)
    {
        result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_TIMER_QE_CORR, remainder16);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }

    timeHi = (uint32_t)((pTimerConfig->periodNs & ~ADI_MAC_TS_QE_MASK) * pTimerConfig->dutyCycle);
    timeLo = (pTimerConfig->periodNs & ~ADI_MAC_TS_QE_MASK) - timeHi;
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_TIMER_HI, timeHi & ~ADI_MAC_TS_QE_MASK);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_TIMER_LO, timeLo & ~ADI_MAC_TS_QE_MASK);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_TIMER_START, pTimerConfig->startTimeNs);

end:
    return result;
}

adi_eth_Result_e MAC_TsTimerStop(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            val32;

    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_CFG, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val32 |= BITM_MAC_TS_CFG_TS_TIMER_STOP;
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_CFG, val32);

end:
    return result;
}

adi_eth_Result_e MAC_TsConvert(uint32_t timestampLowWord, uint32_t timestampHighWord, adi_mac_TsFormat_e format, adi_mac_TsTimespec_t *pTimespec)
{
    adi_eth_Result_e result = ADI_ETH_SUCCESS;
    /* Floating point nanoseconds value for free-running counter conversion. Double to reduce rounding errors.  */
    double nsecF;

    switch(format)
    {
        case ADI_MAC_TS_FORMAT_32B_FREE:
            /* Convert LSB to nanoseconds. */
            nsecF = timestampLowWord * ADI_MAC_TS_FREE_CNT_LSB_IN_NS;
            /* Extract number of whole seconds. */
            pTimespec->sec = (uint32_t)(nsecF / ADI_MAC_TS_ONE_SECOND_IN_NS);
            /* Subtract off number of whole seconds to get number of whole nanoseconds. Fractional value is lost. */
            pTimespec->nsec = (uint32_t)(nsecF - (pTimespec->sec * ADI_MAC_TS_ONE_SECOND_IN_NS));
            break;
        case ADI_MAC_TS_FORMAT_32B_1588:
            /* Mask and shift to get number of seconds (2 bits). */
            pTimespec->sec = (timestampLowWord & ADI_MAC_TS_1588_SEC_MASK) >> ADI_MAC_TS_1588_SEC_POS;
            /* Mask to get number of nanoseconds (30 bits). */
            pTimespec->nsec = timestampLowWord & ADI_MAC_TS_1588_NS_MASK;
            break;
        case ADI_MAC_TS_FORMAT_64B_1588:
            /* All 32b of upper words are whole seconds. */
            pTimespec->sec = timestampHighWord;
            /* Mask to get number of nanoseconds (30 bits). */
            pTimespec->nsec = timestampLowWord & ADI_MAC_TS_1588_NS_MASK;
            break;
        default:
            result = ADI_ETH_NO_TS_FORMAT;
            break;
    }

    return result;
}

adi_eth_Result_e MAC_TsGetExtCaptTimestamp(adi_mac_Device_t *hDevice, adi_mac_TsTimespec_t *pCapturedTimespec)
{
    adi_eth_Result_e result = ADI_ETH_SUCCESS;
    uint32_t timestampLowWord;
    uint32_t timestampHighWord;

    /* 64b timestamp and free-running counter are both always captured. Return only one depending on the configured format. */
    if ((hDevice->timestampFormat == ADI_MAC_TS_FORMAT_32B_1588) || (hDevice->timestampFormat == ADI_MAC_TS_FORMAT_64B_1588))
    {
        result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_EXT_CAPT0, &timestampLowWord);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_EXT_CAPT1, &timestampHighWord);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_TsConvert(timestampLowWord, timestampHighWord, ADI_MAC_TS_FORMAT_64B_1588, pCapturedTimespec);
    }
    else if (hDevice->timestampFormat == ADI_MAC_TS_FORMAT_32B_FREE)
    {
        timestampHighWord = 0;
        result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_FREECNT_CAPT, &timestampLowWord);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_TsConvert(timestampLowWord, timestampHighWord, hDevice->timestampFormat, pCapturedTimespec);
    }
    else
    {
        result = ADI_ETH_NO_TS_FORMAT;
    }

end:
    return result;

}

adi_eth_Result_e MAC_TsGetEgressTimestamp(adi_mac_Device_t *hDevice, adi_mac_EgressCapture_e egressReg, adi_mac_TsTimespec_t *pCapturedTimespec)
{
    adi_eth_Result_e result = ADI_ETH_SUCCESS;
    uint32_t timestampLowWord;
    uint32_t timestampHighWord;
    uint32_t regAddrL;
    uint32_t regAddrH;

    switch(egressReg)
    {
        case ADI_MAC_EGRESS_CAPTURE_A:
            regAddrL = ADDR_MAC_TTSCAL;
            regAddrH = ADDR_MAC_TTSCAH;
            break;
        case ADI_MAC_EGRESS_CAPTURE_B:
            regAddrL = ADDR_MAC_TTSCBL;
            regAddrH = ADDR_MAC_TTSCBH;
            break;
        case ADI_MAC_EGRESS_CAPTURE_C:
            regAddrL = ADDR_MAC_TTSCCL;
            regAddrH = ADDR_MAC_TTSCCH;
            break;
        default:
            result = ADI_ETH_NO_TS_FORMAT;
            break;
    }

    if (result == ADI_ETH_SUCCESS)
    {
        result = MAC_ReadRegister(hDevice, regAddrL, &timestampLowWord);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_ReadRegister(hDevice, regAddrH, &timestampHighWord);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = MAC_TsConvert(timestampLowWord, timestampHighWord, hDevice->timestampFormat, pCapturedTimespec);
    }
end:
    return result;
}

adi_eth_Result_e MAC_TsSetTimerAbsolute(adi_mac_Device_t *hDevice, uint32_t seconds, uint32_t nanoseconds)
{
    adi_eth_Result_e result = ADI_ETH_SUCCESS;
    uint32_t addend;

    /* Stop clock while updating. */
    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_ADDEND, &addend);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_ADDEND, 0);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_SEC_CNT, seconds);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Nanoseconds value must be greater than ADI_MAC_TS_MIN_NS and divisible by ADI_MAC_TS_MIN_NS. */
    if (nanoseconds < ADI_MAC_TS_MIN_NS)
    {
        nanoseconds = ADI_MAC_TS_MIN_NS;
    }
    /* Nanoseconds value must be less than the number of nanoseconds in a second. */
    if (nanoseconds >= ADI_MAC_TS_ONE_SECOND_IN_NS)
    {
        nanoseconds = ADI_MAC_TS_ONE_SECOND_IN_NS - 1;
    }
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_NS_CNT, nanoseconds & ~ADI_MAC_TS_QE_MASK);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_ADDEND, addend);

end:
    return result;
}

adi_eth_Result_e MAC_TsSyncClock(adi_mac_Device_t *hDevice, int64_t tError, uint64_t referenceTimeNsDiff, uint64_t localTimeNsDiff)
{
    adi_eth_Result_e result = ADI_ETH_SUCCESS;
    int64_t referenceNsDiff;
    int64_t localNsDiff;
    int64_t val64;
    uint32_t addend;

    /* Adjust to prevent overflow in computation */
    referenceNsDiff = referenceTimeNsDiff;
    localNsDiff = localTimeNsDiff;
    while (referenceNsDiff > 0x7FFFFFFF)
    {
        referenceNsDiff >>= 1;
        localNsDiff >>= 1;
    }

    /* Compute syntonization factor. */
    result = MAC_ReadRegister(hDevice, ADDR_MAC_TS_ADDEND, &addend);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    val64 = (int64_t) addend;
    if ((referenceNsDiff != 0) && (localNsDiff != 0)) {
        val64 = ((referenceNsDiff * val64) / localNsDiff);
    }

    /* Apply correction. */
    val64 += tError;

    /* Coerce to register limits and write. */
    addend = (uint32_t)val64;
    result = MAC_WriteRegister(hDevice, ADDR_MAC_TS_ADDEND, addend);

end:
    return result;
}

int64_t MAC_TsSubtract(adi_mac_TsTimespec_t *pTsA, adi_mac_TsTimespec_t *pTsB)
{
    int64_t tsAns;
    int64_t tsBns;

    /* Convert s to ns and add in ns */
    tsAns = pTsA->sec;
    tsAns *= ADI_MAC_TS_ONE_SECOND_IN_NS;
    tsAns += pTsA->nsec;

    /* Convert s to ns and add in ns */
    tsBns = pTsB->sec;
    tsBns *= ADI_MAC_TS_ONE_SECOND_IN_NS;
    tsBns += pTsB->nsec;

    /* Return the difference in ns */
    return tsAns - tsBns;
}


adi_eth_Result_e MAC_SyncConfig(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            val32;

    result = MAC_ReadRegister(hDevice, ADDR_MAC_CONFIG0, &val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val32 |= BITM_MAC_CONFIG0_SYNC;

    result = MAC_WriteRegister(hDevice, ADDR_MAC_CONFIG0, val32);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* CONFIG0.SYNC is set, we can now enable the IRQ. */
    ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
    hDevice->configSync = true;

end:
    return result;
}

/*
 * @brief           Calculates the parity over a number of bytes.
 *
 * @param [in]      p           Pointer to the input data.
 * @param [in]      nBytes      Size of the input data, in bytes.
 *
 * @return          Returns 1 if input data has odd parity. Otherwise 0.
 *
 * @details         Calculates the odd parity of the input data. Used for the
 *                  header/footer parity fields defined by the OPEN Alliance specification
 *                  and for timestamp parity checking in both SPI protocols.
 *
 */
uint8_t MAC_CalculateParity(uint8_t *p, uint32_t nBytes)
{
    uint32_t i;

    uint8_t parity = 0;
    for (i = 0; i < nBytes; i++)
    {
        parity ^= p[i];
    }

    uint8_t prt = 0;
    for (uint32_t i = 0; i < 8; i++)
    {
        prt = prt ^ (parity & 0x1);
        parity >>= 1;
    }

    return prt & 0x1;
}

adi_mac_DriverEntry_t macDriverEntry =
{
    MAC_Init,
    MAC_UnInit,
    MAC_Reset,
    MAC_SyncConfig,
    MAC_RegisterCallback,
    MAC_GetLinkStatus,
    MAC_GetStatCounters,
    MAC_AddAddressFilter,
    MAC_ClearAddressFilter,
    MAC_SubmitTxBuffer,
    MAC_SubmitRxBuffer,
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
    MAC_SubmitRxBufferHp,
#endif
    MAC_SetPromiscuousMode,
    MAC_GetPromiscuousMode,
#if defined(SPI_OA_EN)
    MAC_SetChunkSize,
    MAC_GetChunkSize,
#endif
    MAC_SetCutThroughMode,
    MAC_GetCutThroughMode,
    MAC_SetFifoSizes,
    MAC_GetFifoSizes,
    MAC_ClearFifos,
    MAC_TsEnable,
    MAC_TsClear,
    MAC_TsTimerStart,
    MAC_TsTimerStop,
    MAC_TsSetTimerAbsolute,
    MAC_TsSyncClock,
    MAC_TsGetExtCaptTimestamp,
    MAC_TsGetEgressTimestamp,
    MAC_TsConvert,
    MAC_TsSubtract,
    MAC_ReadRegister,
    MAC_WriteRegister,
    MAC_PhyRead,
    MAC_PhyWrite
};
