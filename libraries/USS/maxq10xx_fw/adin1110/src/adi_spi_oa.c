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

#include "adi_spi_oa.h"

/*! Size of data buffer used in SPI transactions, in bytes. */
#define BUFFERSIZE 2000

/*! SPI receive data buffer. */
DMA_BUFFER_ALIGN(static uint8_t spiRxBuf[BUFFERSIZE], 4);
/*! SPI transmit data buffer. */
DMA_BUFFER_ALIGN(static uint8_t spiTxBuf[BUFFERSIZE], 4);

/* Function prototypes. */
static adi_eth_Result_e     oaSpiProcess            (adi_mac_Device_t *hDevice);
static adi_eth_Result_e     oaCreateNextChunk       (adi_mac_Device_t *hDevice, uint8_t *pBuf, bool txEn);
static adi_eth_Result_e     oaCtrlSetup             (uint8_t *pBuf, uint32_t wnr, uint32_t regAddr, uint32_t *pRegData, uint32_t *pLen);
static void                 oaCtrlCmdHeader         (uint8_t *p, uint32_t wnr, uint32_t addr, uint32_t cnt);
static adi_eth_Result_e     oaCtrlCmdReadData       (uint32_t *dst, uint8_t *src, uint32_t cnt);
static adi_eth_Result_e     oaCtrlCmdWriteData      (uint8_t *dst, uint32_t *src, uint32_t cnt);
static adi_eth_Result_e     oaSpiIntHandle          (adi_mac_Device_t *hDevice);
static adi_eth_Result_e     oaPhyRegReadStart       (adi_mac_Device_t *hDevice, ADI_MAC_MDIOACC_0__t *mdioCmd, uint32_t prtad, uint32_t regAddr);
static adi_eth_Result_e     oaPhyRegReadStep        (adi_mac_Device_t *hDevice, ADI_MAC_MDIOACC_0__t *mdioCmd);


/*!
 * @brief           MAC-PHY interrupt service routine.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          None
 *
 * @details         Called from the INT_N interrupt handler. Executes the OA state machine.
 *
 * @sa              oaStateMachine()
 */
void oaIrqHandler(adi_mac_Device_t *hDevice)
{
    /* If SPI is busy, the interrupt is not required because all relevant info comes in the footer. */
    if (hDevice->spiState == ADI_MAC_SPI_STATE_READY)
    {
        ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);
        hDevice->state = ADI_MAC_STATE_IRQ_START;

        oaStateMachine(hDevice);
    }
}

/*!
 * @brief           SPI callback function.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          None
 *
 * @details         Called from the SPI interrupt handler (if SPI uses interrupts) or DMA interrupt handler
 *                  (if SPI uses DMA). Executes the OA state machine.
 *
 * @sa              oaStateMachine()
 */
void spiCallback(void *pCBParam, uint32_t Event, void *pArg)
{
    adi_mac_Device_t        *hDevice = (adi_mac_Device_t *)pCBParam;

    oaStateMachine(hDevice);
}

/*!
 * @brief           OPEN Alliance SPI state machine.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *                  - #ADI_ETH_INVALID_PARAM        Configured memory size too small.
 *                  - #ADI_ETH_COMM_ERROR           MDIO communication failure.
 *                  - #ADI_ETH_UNSUPPORTED_DEVICE   Device not supported.
 *
 * @details         Implements the OPEN Alliance SPI protocol.
 *
 */
adi_eth_Result_e oaStateMachine(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e                result = ADI_ETH_SUCCESS;
    uint32_t                        len;
    bool                            useDma;
    uint32_t                        tail;
    uint32_t                        byteOffset;
    uint8_t                         *pRxBuf;
    volatile adi_mac_OaTxHeader_t   oaTxHeader;
    volatile adi_mac_OaRxFooter_t   oaRxFooter;
    uint32_t                        chunkSize = (1 << hDevice->oaCps);
    uint32_t                        exst;
    uint32_t                        val32;
    uint32_t                        mem32;
    uint32_t                        expectedFcs = 0;
    uint32_t                        cHdr;
    uint32_t                        eHdr;
    uint32_t                        Event = ADI_MAC_CALLBACK_STATUS_OK;
    uint8_t                         timestampBytes[8];
    static ADI_MAC_MDIOACC_0__t     mdioCmd;

    exst = 0;

    switch (hDevice->state)
    {
        case ADI_MAC_STATE_CONTROL_START:

            len = hDevice->cnt;

            result = oaCtrlSetup(&hDevice->ctrlTxBuf[0], hDevice->wnr, hDevice->regAddr, hDevice->pRegData, &len);

            useDma = (len >= MIN_SIZE_FOR_DMA);
            hDevice->state = ADI_MAC_STATE_CONTROL_END;
            /* OA is full duplex, can chose either Rx or Tx */
            hDevice->spiState = ADI_MAC_SPI_STATE_RX;

            ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, &hDevice->ctrlTxBuf[0], &hDevice->ctrlRxBuf[0], len, useDma);

            break;

        case ADI_MAC_STATE_CONTROL_END:

            /* Check header vs. echoed header for errors */
            cHdr = *(uint32_t *)&hDevice->ctrlTxBuf[0];
            eHdr = *(uint32_t *)&hDevice->ctrlRxBuf[ADI_SPI_HEADER_SIZE];

            if (cHdr != eHdr)
            {
                hDevice->spiErr = 1;
            }
            else
            {
                hDevice->spiErr = 0;
                if (hDevice->wnr == ADI_MAC_SPI_READ)
                {
                    result = oaCtrlCmdReadData((uint32_t *)hDevice->pRegData, &hDevice->ctrlRxBuf[2 * ADI_SPI_HEADER_SIZE], hDevice->cnt);

                }
            }
            hDevice->spiState = ADI_MAC_SPI_STATE_READY;
            hDevice->state = ADI_MAC_STATE_READY;

            break;

        case ADI_MAC_STATE_DATA_START:

            result = oaSpiProcess(hDevice);

            if ((result == ADI_ETH_SUCCESS) && hDevice->oaTrxSize)
            {
                /* Determine if it's worth using DMA based on the transaction size. */
                useDma = (hDevice->oaTrxSize >= MIN_SIZE_FOR_DMA);

                hDevice->state = ADI_MAC_STATE_DATA_END;
                /* Rx/Tx are the same in OA SPI, reusing the state names from   */
                /* generic SPI. Different state names also help with debugging. */
                hDevice->spiState = ADI_MAC_SPI_STATE_TX_FRAME;

                /* Platform dependent function*/
                ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, spiTxBuf, spiRxBuf, hDevice->oaTrxSize, useDma);
            }
            else
            {
                hDevice->state = ADI_MAC_STATE_READY;
                hDevice->spiState = ADI_MAC_SPI_STATE_READY;

                if (!hDevice->pendingCtrl)
                {
                    ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
                }
            }

            break;

        case ADI_MAC_STATE_DATA_END:

            for (uint32_t chunkStart = 0; chunkStart < hDevice->oaTrxSize; chunkStart += ADI_SPI_HEADER_SIZE + chunkSize)
            {
                oaRxFooter.VALUE32 = *(uint32_t *)&spiRxBuf[chunkStart + chunkSize];
                oaRxFooter.VALUE32 = HTON32(oaRxFooter.VALUE32);

                if (oaRxFooter.VALUE32 == ADI_OA_HEADER_BAD)
                {
                    /* Parity error on transmitted header */
                    hDevice->oaErrorStats.hdrParityErrorCount++;
                }
                else
                {
                    /* Ignore the chunk if the footer parity check fails. */
                    if (!MAC_CalculateParity((uint8_t *)&oaRxFooter.VALUE32, 4))
                    {
                        hDevice->oaErrorStats.ftrParityErrorCount++;
                    }
                    else
                    {
                        if (!oaRxFooter.SYNC)
                        {
                            hDevice->oaErrorStats.syncErrorCount++;
                        }
                        else
                        {
                            exst |= oaRxFooter.EXST;

                            hDevice->oaTxc = oaRxFooter.TXC;
                            hDevice->oaRca = oaRxFooter.RCA;

                            if (oaRxFooter.DV && !hDevice->oaRxUseBackupBuf)
                            {
                                pRxBuf = hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->pBuf;
                                byteOffset = 0;
                                if (hDevice->oaTimestampSplit)
                                {
                                    /* If the timestamp was split into two chunks, the remaining 32b will be at the start of this chunk. */
                                    hDevice->oaTimestampSplit = false;
                                    memcpy(&mem32, &spiRxBuf[chunkStart], 4);
                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestamp = HTON32(mem32);
                                    memcpy(&timestampBytes[0], &spiRxBuf[chunkStart], 4);
                                    byteOffset += 4;
                                    /* MAC_CalculateParity returns 1 if timestampBytes has odd parity, and oaTimestampParity is 0 if timestampBytes has odd parity. */
                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampValid = (MAC_CalculateParity(timestampBytes, 8) != hDevice->oaTimestampParity);
                                }

                                if (oaRxFooter.EV)
                                {
                                    /* If FD = 1, frame needs to be dropped */
                                    if (oaRxFooter.FD)
                                    {
                                        /* Reset the receive index to reuse current buffer for next frame */
                                        hDevice->oaRxCurBufByteOffset = 0;

                                        hDevice->oaErrorStats.fdCount++;
                                    }
                                    else
                                    {
                                        /* Special case: a full frame is sent in a single chunk, this can occur */
                                        /* when frame is 64 bytes (including FCS) and the chunk is 64 bytes.    */
                                        /* This case is handled by SV=1 case.                                   */
                                        uint32_t ebo = oaRxFooter.EBO;
                                        uint32_t sbo = oaRxFooter.SWO * 4;
                                        if (!(oaRxFooter.SV && (ebo > sbo)))
                                        {
                                            if (hDevice->oaValidFlag != ADI_MAC_OA_VALID_FLAG_START)
                                            {
                                                /* No prior SV was received, this is an error */
                                                /* Reset the receive index to reuse current buffer for next frame */
                                                hDevice->oaRxCurBufByteOffset = 0;

                                                hDevice->oaErrorStats.invalidEvCount++;
                                            }
                                            else
                                            {
                                                if (hDevice->oaRxCurBufByteOffset + ebo + 1 > hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->bufSize)
                                                {
                                                    /* Rx buffer too small for the incoming frame, notify the user. */
                                                    Event |= ADI_MAC_CALLBACK_STATUS_RX_BUF_OVF;
                                                }
                                                else
                                                {
                                                    memcpy(&pRxBuf[hDevice->oaRxCurBufByteOffset], &spiRxBuf[chunkStart], ebo + 1);
                                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize = hDevice->oaRxCurBufByteOffset + ebo + 1;

                                                    /* Adjust buffer size for FCS */
                                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize -= FCS_SIZE;

                                                    if (hDevice->fcsCheckEn)
                                                    {
                                                        uint32_t actualFcs;
                                                        memcpy(&actualFcs, &pRxBuf[hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize], FCS_SIZE);
                                                        expectedFcs = ADI_HAL_FCS_CALCULATE(hDevice->adinDevice, pRxBuf,
                                                                        hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize);
                                                        if (expectedFcs != actualFcs)
                                                        {
                                                            Event |= ADI_MAC_CALLBACK_STATUS_FCS_ERROR;
                                                        }
                                                    }
                                                }
                                                tail = hDevice->pRxQueue->tail;
                                                queueRemove(hDevice->pRxQueue);

    #if defined(ADIN2111)
                                                /* Update the dynamic forwarding table */
                                                /* If there was an FCS error, it will be passed on via the callback argument, but it will be ignored by the callback. */
                                                if (hDevice->cbFunc[ADI_MAC_EVT_DYN_TBL_UPDATE] != NULL)
                                                {
                                                    hDevice->cbFunc[ADI_MAC_EVT_DYN_TBL_UPDATE](hDevice->adinDevice, Event, hDevice->pRxQueue->pEntries[tail].pBufDesc);
                                                }
    #endif
                                                if (hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc)
                                                {
                                                    hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc(hDevice->adinDevice, Event, hDevice->pRxQueue->pEntries[tail].pBufDesc);
                                                }

                                                hDevice->oaRxCurBufByteOffset = 0;
                                            }
                                            hDevice->oaValidFlag = ADI_MAC_OA_VALID_FLAG_END;
                                        }
                                    }
                                }

                                if (oaRxFooter.SV)
                                {
                                    if ((hDevice->oaValidFlag != ADI_MAC_OA_VALID_FLAG_NONE) && (hDevice->oaValidFlag != ADI_MAC_OA_VALID_FLAG_END))
                                    {
                                        /* No prior EV was received, this is an error */
                                        hDevice->oaErrorStats.invalidSvCount++;
                                    }

                                    uint32_t vs = oaRxFooter.VS;
                                    adi_mac_RxFifoPrio_e prio = (adi_mac_RxFifoPrio_e)((vs & 0x2) >> 1);
#if defined(ADI_MAC_ENABLE_RX_QUEUE_HI_PRIO)
                                    if (prio == ADI_MAC_RX_FIFO_PRIO_LOW)
                                    {
                                        hDevice->pRxQueue = &hDevice->rxQueueLp;
                                    }
                                    else
                                    {
                                        hDevice->pRxQueue = &hDevice->rxQueueHp;
                                    }
#endif
                        			if (queueIsEmpty(hDevice->pRxQueue) && !hDevice->oaRxUseBackupBuf)
                        			{
                                        /* If a new frame is being received (SV=1) but there are no Rx buffers queued,  */
                                        /* we drop the incoming data until Rx buffers are added to the queue. This is   */
                                        /* simpler, but may drop extra frames if the application is slow to refill the  */
                                        /* queue. The alternative is to use a backup buffer while the Rx queue is empty */
                                        /* and switch back to parsing chunks from spiRxBuf when new buffers are added   */
                                        /* to the Rx queue. There are hooks for this in the code but the switching      */
                                        /* between oaRxBackupBuf and spiRxBuf is not implemented yet. Perhaps an option */
                                        /* to use one option or the other can be useful to serve different use cases.   */
			                            hDevice->oaRxUseBackupBuf = false;
			                            hDevice->oaRxBufChunkStart = chunkStart;
                                        hDevice->oaRxBufTrxSize = hDevice->oaTrxSize;
			                        }
			                        else
			                        {
                        				hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->prio = prio;
#if defined(ADIN2111)
                        				hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->port = vs & 0x1;
#endif
                        				pRxBuf = hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->pBuf;

                                        hDevice->oaRxCurBufByteOffset = 0;
                    					/* SWO is multiple of words */
                                        byteOffset = oaRxFooter.SWO * 4;

                                        if (oaRxFooter.RTSA)
	                                    {
                                            /* Store the parity in case the timestamp is split across multiple chunks. */
                                            /* RTSP is only valid for the chunk in which RTSA is 1. */
                                            hDevice->oaTimestampParity = oaRxFooter.RTSP;
                                            if (hDevice->timestampFormat == ADI_MAC_TS_FORMAT_64B_1588)
                                            {
                                                memcpy(&mem32, &spiRxBuf[chunkStart + byteOffset], 4);
                                                hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampExt = HTON32(mem32);
                                                memcpy(&timestampBytes[4], &spiRxBuf[chunkStart + byteOffset], 4);
                                                byteOffset += 4;
                                                /* If there is not room in the chunk for the lower 32b of the timestamp, those bits will be in the next chunk. */
                                                if ((chunkStart + byteOffset + 4) > (chunkStart + chunkSize))
                                                {
                                                    hDevice->oaTimestampSplit = true;
                                                }
                                                else
                                                {
                                                    /* The lower 32b of the timestamp are in the same chunk. */
                                                    hDevice->oaTimestampSplit = false;
                                                    memcpy(&mem32, &spiRxBuf[chunkStart + byteOffset], 4);
                                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestamp = HTON32(mem32);
                                                    memcpy(&timestampBytes[0], &spiRxBuf[chunkStart + byteOffset], 4);
                                                    byteOffset += 4;
                                                    /* MAC_CalculateParity returns 1 if timestampBytes has odd parity, and oaTimestampParity is 0 if timestampBytes has odd parity. */
                                                    hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampValid = (MAC_CalculateParity(timestampBytes, 8) != hDevice->oaTimestampParity);
                                                }
                                            }
                                            else
                                            {
                                                /* Start of the frame is at a multiple of 4 bytes, so a 32-bit timestamp will always be contained in the same chunk. */
                                                hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampExt = 0;
                                                memcpy(&mem32, &spiRxBuf[chunkStart + byteOffset], 4);
                                                hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestamp = HTON32(mem32);
                                                memcpy(&timestampBytes[0], &spiRxBuf[chunkStart + byteOffset], 4);
                                                byteOffset += 4;
                                                /* MAC_CalculateParity returns 1 if timestampBytes has odd parity, and oaTimestampParity is 0 if timestampBytes has odd parity. */
                                                hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampValid = (MAC_CalculateParity(timestampBytes, 4) != hDevice->oaTimestampParity);
                                            }
	                                    }
	                                    else
	                                    {
	                                        hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->timestampValid = false;
	                                    }

	                                    if (chunkSize - byteOffset)
	                                    {
	                                        /* Rx buffer minimum size is greater than the maximum chunk size, no buffer overflow. */
	                                        memcpy(&pRxBuf[0], &spiRxBuf[chunkStart + byteOffset], chunkSize - byteOffset);
	                                        hDevice->oaRxCurBufByteOffset += chunkSize - byteOffset;
	                                    }

                                        /* Special case: full frame inside a single chunk. */
                                        uint32_t ebo = oaRxFooter.EBO;
                                        uint32_t sbo = oaRxFooter.SWO * 4;
                                        if (oaRxFooter.EV && (ebo > sbo))
                                        {
                                            hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize = ebo + 1 - sbo - FCS_SIZE;

                                            if (hDevice->fcsCheckEn)
                                            {
                                                uint32_t actualFcs;
                                                memcpy(&actualFcs, &pRxBuf[hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize], FCS_SIZE);
                                                expectedFcs = ADI_HAL_FCS_CALCULATE(hDevice->adinDevice, pRxBuf,
                                                                hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->trxSize);

                                                if (expectedFcs != actualFcs)
                                                {
                                                    Event |= ADI_MAC_CALLBACK_STATUS_FCS_ERROR;
                                                }
                                            }
                                            tail = hDevice->pRxQueue->tail;
                                            queueRemove(hDevice->pRxQueue);

#if defined(ADIN2111)
                                            /* Update the dynamic forwarding table */
                                            /* If there was an FCS error, it will be passed on via the callback argument, but it will be ignored by the callback. */
                                            if (hDevice->cbFunc[ADI_MAC_EVT_DYN_TBL_UPDATE] != NULL)
                                            {
                                                hDevice->cbFunc[ADI_MAC_EVT_DYN_TBL_UPDATE](hDevice->adinDevice, Event, hDevice->pRxQueue->pEntries[tail].pBufDesc);
                                            }
#endif
                                            if (hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc)
                                            {
                                                hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc(hDevice->adinDevice, Event, hDevice->pRxQueue->pEntries[tail].pBufDesc);
                                            }

                                            hDevice->oaRxCurBufByteOffset = 0;

                                            hDevice->oaValidFlag = ADI_MAC_OA_VALID_FLAG_END;
                                        }
                                        else
                                        {
                                            hDevice->oaValidFlag = ADI_MAC_OA_VALID_FLAG_START;
                                        }
                                    }
                                }
                                else
                                {
                                    if (!oaRxFooter.EV && (hDevice->oaValidFlag == ADI_MAC_OA_VALID_FLAG_START))
                                    {
                                        if (hDevice->oaRxCurBufByteOffset + chunkSize > hDevice->pRxQueue->pEntries[hDevice->pRxQueue->tail].pBufDesc->bufSize)
                                        {
                                            /* Rx buffer too small for the incoming frame, notify the user. */
                                            /* At this point the current frame reception finishes and callback is executed. */
                                            Event |= ADI_MAC_CALLBACK_STATUS_RX_BUF_OVF;

                                            tail = hDevice->pRxQueue->tail;
                                            queueRemove(hDevice->pRxQueue);

                                            if (hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc)
                                            {
                                                hDevice->pRxQueue->pEntries[tail].pBufDesc->cbFunc(hDevice->adinDevice, Event, hDevice->pRxQueue->pEntries[tail].pBufDesc);
                                            }

                                            hDevice->oaRxCurBufByteOffset = 0;
                                        }
                                        else
                                        {
                                            memcpy(&pRxBuf[hDevice->oaRxCurBufByteOffset], &spiRxBuf[chunkStart + byteOffset], chunkSize - byteOffset);
                                            hDevice->oaRxCurBufByteOffset += chunkSize - byteOffset;
                                        }
                                    }
                                }
                            }

                            memcpy((void *)&oaTxHeader.VALUE32, &spiTxBuf[chunkStart], 4);
                            oaTxHeader.VALUE32 = HTON32(oaTxHeader.VALUE32);

                            if (oaTxHeader.EV)
                            {
                                tail = hDevice->txQueue.tail;
                                queueRemove(&hDevice->txQueue);

                                /* Decrement the reference count, and call the callback function only if the reference  */
                                /* count is 0. This ensures that if the intent was to send the buffer to both ports, it */
                                /* will be returned to the buffer pool only after sending to both ports has completed.  */
                                hDevice->txQueue.pEntries[tail].pBufDesc->refCount--;
                                if (hDevice->txQueue.pEntries[tail].pBufDesc->cbFunc && (!hDevice->txQueue.pEntries[tail].pBufDesc->refCount))
                                {
                                    hDevice->txQueue.pEntries[tail].pBufDesc->cbFunc(hDevice->adinDevice, Event, hDevice->txQueue.pEntries[tail].pBufDesc);
                                }
                            }
                        }
                    }
                }
            }
            if (exst || !oaRxFooter.SYNC)
            {

                /* Read status registers */
                len = 2;

                result = oaCtrlSetup(&hDevice->ctrlTxBuf[0], ADI_MAC_SPI_READ, ADDR_MAC_STATUS0, hDevice->pRegData, &len);

                useDma = (len >= MIN_SIZE_FOR_DMA);
                hDevice->state = ADI_MAC_STATE_DATA_READ_STATUS;
                /* Rx/Tx are the same in OA SPI, reusing the state names from   */
                /* generic SPI. Different state names also help with debugging. */
                hDevice->spiState = ADI_MAC_SPI_STATE_RX;

                ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, &hDevice->ctrlTxBuf[0], &hDevice->ctrlRxBuf[0], len, useDma);
            }
            else
            {
                if ((!queueIsEmpty(&hDevice->txQueue) && hDevice->oaTxc) || (!queueIsEmpty(hDevice->pRxQueue) && hDevice->oaRca))
                {
                    result = oaSpiProcess(hDevice);

                    /* Determine if it's worth using DMA based on the transaction size. */
                    useDma = (hDevice->oaTrxSize >= MIN_SIZE_FOR_DMA);

                    hDevice->state = ADI_MAC_STATE_DATA_END;
                    /* Rx/Tx are the same in OA SPI, reusing the state names from   */
                    /* generic SPI. Different state names also help with debugging. */
                    hDevice->spiState = ADI_MAC_SPI_STATE_RX_FRAME;

                    /* Platform dependent function*/
                    ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, spiTxBuf, spiRxBuf, hDevice->oaTrxSize, useDma);

                }
                else
                {
                    hDevice->state = ADI_MAC_STATE_READY;
                    hDevice->spiState = ADI_MAC_SPI_STATE_READY;

                    if (!hDevice->pendingCtrl)
                    {
                        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
                    }
                }
            }

            break;

        case ADI_MAC_STATE_IRQ_START:
            /* Single data chunk is response to an IRQ */
            oaTxHeader.VALUE32 = 0x00000000;
            oaTxHeader.DNC = 1;
            oaTxHeader.NORX = 1;
            oaTxHeader.P = 1;
            val32 = oaTxHeader.VALUE32 >> 1;
            for (uint32_t i = 0; i < 31; i++)
            {
                oaTxHeader.P = oaTxHeader.P ^ (val32 & 0x1);
                val32 >>= 1;
            }

            mem32 = HTON32(oaTxHeader.VALUE32);
            memcpy(&spiTxBuf[0], &mem32, 4);

            hDevice->oaTrxSize = ADI_SPI_HEADER_SIZE + (1 << hDevice->oaCps);

            /* Determine if it's worth using DMA based on the transaction size. */
            useDma = (hDevice->oaTrxSize >= MIN_SIZE_FOR_DMA);

            hDevice->state = ADI_MAC_STATE_DATA_END;
            /* Rx/Tx are the same in OA SPI, reusing the state names from   */
            /* generic SPI. Different state names also help with debugging. */
            hDevice->spiState = ADI_MAC_SPI_STATE_RX;

            /* Platform dependent function*/
            ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, spiTxBuf, spiRxBuf, hDevice->oaTrxSize, useDma);

            break;

        case ADI_MAC_STATE_DATA_READ_STATUS:

            result = oaCtrlCmdReadData((uint32_t *)&hDevice->statusRegisters.status0, &hDevice->ctrlRxBuf[2 * ADI_SPI_HEADER_SIZE], 2);

            hDevice->statusRegisters.status0Masked = hDevice->statusRegisters.status0 & ~hDevice->irqMask0;
            hDevice->statusRegisters.status1Masked = hDevice->statusRegisters.status1 & ~hDevice->irqMask1;
            hDevice->statusRegisters.p1StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
            hDevice->statusRegisters.p1Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#if defined(ADIN2111)
            hDevice->statusRegisters.p2StatusMasked = ADI_MAC_PHY_STATUS_INIT_VAL;
            hDevice->statusRegisters.p2Status = ADI_MAC_PHY_STATUS_INIT_VAL;
#endif

#if defined(ADIN2111)
            if ((hDevice->statusRegisters.status0Masked & BITM_MAC_STATUS0_PHYINT) || (hDevice->statusRegisters.status1Masked & BITM_MAC_STATUS1_P2_PHYINT))
#else
            if (hDevice->statusRegisters.status0Masked & BITM_MAC_STATUS0_PHYINT)
#endif
            {
                /* Read PHY interrupt status registers */
                /* PHY register reads are as implemented in MAC_PhyRead(). */
                uint32_t prtad;
                if (hDevice->statusRegisters.status0Masked & BITM_MAC_STATUS0_PHYINT)
                {
                    prtad = 1;
                }
                else
                {
                    prtad = 2;
                }
                oaPhyRegReadStart(hDevice, &mdioCmd, prtad, ADDR_CRSM_IRQ_STATUS);
            }

            else
            {
                oaSpiIntHandle(hDevice);
            }

            break;

        case ADI_MAC_STATE_DATA_READ_PHY_REGISTER:

            /* Check header vs. echoed header for errors */
            memcpy(&cHdr, &hDevice->ctrlTxBuf[0], 4);
            memcpy(&eHdr, &hDevice->ctrlRxBuf[ADI_SPI_HEADER_SIZE], 4);

            if (cHdr != eHdr)
            {
                hDevice->spiErr = 1;
                oaSpiIntHandle(hDevice);
            }
            else
            {
                hDevice->spiErr = 0;

                if (hDevice->regAddr == ADDR_MAC_MDIOACC_0_)
                {
                    /* Second register write */
                    hDevice->wnr = ADI_MAC_SPI_WRITE;
                    oaPhyRegReadStep(hDevice, &mdioCmd);
                }
                else
                {
                    if (hDevice->wnr == ADI_MAC_SPI_WRITE)
                    {
                        /* Both writes completed, now read */
                        hDevice->wnr = ADI_MAC_SPI_READ;
                        oaPhyRegReadStep(hDevice, &mdioCmd);
                    }
                    else
                    {
                        /* Poll until TRDONE bit is set in the MDIO access register */
                        result = oaCtrlCmdReadData((uint32_t *)hDevice->pRegData, &hDevice->ctrlRxBuf[2 * ADI_SPI_HEADER_SIZE], hDevice->cnt);
                        if (*hDevice->pRegData & BITM_MAC_MDIOACC_N__MDIO_TRDONE)
                        {
                            /* Successful read, record the status */
                            if ((hDevice->statusRegisters.status0Masked & BITM_MAC_STATUS0_PHYINT) &&
                                ((hDevice->statusRegisters.p1Status & 0xFFFF0000) == (ADI_MAC_PHY_STATUS_INIT_VAL & 0xFFFF0000)))
                            {
                                /* Was CRSM_IRQ_STATUS read yet? */
                                if ((hDevice->statusRegisters.p1StatusMasked & 0x0000FFFF) == (ADI_MAC_PHY_STATUS_INIT_VAL & 0x0000FFFF))
                                {
                                    /* Record state of CRSM_IRQ_STATUS */
                                    hDevice->statusRegisters.p1Status &= 0xFFFF0000;
                                    hDevice->statusRegisters.p1Status |= (*hDevice->pRegData & 0x0000FFFF);
                                    hDevice->statusRegisters.p1StatusMasked &= 0xFFFF0000;
                                    hDevice->statusRegisters.p1StatusMasked |= (hDevice->statusRegisters.p1Status & hDevice->phyIrqMask & 0x0000FFFF);

                                    /* Move on to PHY_SUBSYS_IRQ_STATUS */
                                    oaPhyRegReadStart(hDevice, &mdioCmd, 1, ADDR_PHY_SUBSYS_IRQ_STATUS);

                                }
                                else
                                {
                                    /* Record state of PHY_SUBSYS_IRQ_STATUS */
                                    hDevice->statusRegisters.p1Status &= 0x0000FFFF;
                                    hDevice->statusRegisters.p1Status |= ((*hDevice->pRegData << 16) & 0xFFFF0000);
                                    hDevice->statusRegisters.p1StatusMasked &= 0x0000FFFF;
                                    hDevice->statusRegisters.p1StatusMasked |= ((hDevice->statusRegisters.p1Status & hDevice->phyIrqMask & 0xFFFF0000) << 16);

#if defined(ADIN2111)
                                    if (hDevice->statusRegisters.status1Masked & BITM_MAC_STATUS1_P2_PHYINT)
                                    {
                                        /* At this point we haven't read anything so if the PHYINT is set, we need to go ahead and read from PHY registers */
                                        oaPhyRegReadStart(hDevice, &mdioCmd, 2, ADDR_CRSM_IRQ_STATUS);
                                    }
                                    else
                                    {
                                        oaSpiIntHandle(hDevice);
                                    }
#else
                                    oaSpiIntHandle(hDevice);
#endif
                                }
                            }
#if defined(ADIN2111)
                            else
                            {
                                if ((hDevice->statusRegisters.status1Masked & BITM_MAC_STATUS1_P2_PHYINT) &&
                                    ((hDevice->statusRegisters.p2Status & 0xFFFF0000) == (ADI_MAC_PHY_STATUS_INIT_VAL & 0xFFFF0000)))
                               {

                                    /* Was CRSM_IRQ_STATUS read yet? */
                                    if ((hDevice->statusRegisters.p2StatusMasked & 0x0000FFFF) == (ADI_MAC_PHY_STATUS_INIT_VAL & 0x0000FFFF))
                                    {
                                        /* Record state of CRSM_IRQ_STATUS */
                                        hDevice->statusRegisters.p2Status &= 0xFFFF0000;
                                        hDevice->statusRegisters.p2Status |= (*hDevice->pRegData & 0x0000FFFF);
                                        hDevice->statusRegisters.p2StatusMasked &= 0xFFFF0000;
                                        hDevice->statusRegisters.p2StatusMasked |= (hDevice->statusRegisters.p2Status & hDevice->phyIrqMask & 0x0000FFFF);

                                        /* Move on to PHY_SUBSYS_IRQ_STATUS */
                                        oaPhyRegReadStart(hDevice, &mdioCmd, 2, ADDR_PHY_SUBSYS_IRQ_STATUS);

                                    }
                                    else
                                    {
                                        /* Record state of PHY_SUBSYS_IRQ_STATUS */
                                        hDevice->statusRegisters.p2Status &= 0x0000FFFF;
                                        hDevice->statusRegisters.p2Status |= ((*hDevice->pRegData << 16) & 0xFFFF0000);
                                        hDevice->statusRegisters.p2StatusMasked &= 0x0000FFFF;
                                        hDevice->statusRegisters.p2StatusMasked |= ((hDevice->statusRegisters.p2Status & hDevice->phyIrqMask & 0xFFFF0000) << 16);

                                        oaSpiIntHandle(hDevice);

                                   }
                               }
                            }
#endif
                        }
                        else
                        {
                            /* Keep polling until TRDONE is set */
                            hDevice->wnr = ADI_MAC_SPI_READ;
                            oaPhyRegReadStep(hDevice, &mdioCmd);
                        }

                    }
                }
            }

            break;

        default:
            hDevice->state = ADI_MAC_STATE_READY;
            hDevice->spiState = ADI_MAC_SPI_STATE_READY;

            ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
            break;
    }

    return result;
}

adi_eth_Result_e oaPhyRegReadStart(adi_mac_Device_t *hDevice, ADI_MAC_MDIOACC_0__t *mdioCmd, uint32_t prtad, uint32_t regAddr)
{
    adi_eth_Result_e        result;
    uint32_t                len;
    bool                    useDma;

    mdioCmd->VALUE32 = 0;
    mdioCmd->MDIO_DEVAD = DEVTYPE(regAddr);
    mdioCmd->MDIO_ST = ENUM_MAC_MDIOACC_N__MDIO_ST_CLAUSE45;
    mdioCmd->MDIO_PRTAD = prtad;
    mdioCmd->MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_ADDR;
    mdioCmd->MDIO_DATA = REGADDR(regAddr);

    hDevice->wnr = ADI_MAC_SPI_WRITE;
    hDevice->regAddr = ADDR_MAC_MDIOACC_0_;
    hDevice->pRegData = &mdioCmd->VALUE32;
    hDevice->cnt = 1;

    len = hDevice->cnt;
    result = oaCtrlSetup(&hDevice->ctrlTxBuf[0], hDevice->wnr, hDevice->regAddr, hDevice->pRegData, &len);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    useDma = (len >= MIN_SIZE_FOR_DMA);
    hDevice->state = ADI_MAC_STATE_DATA_READ_PHY_REGISTER;
    /* Rx/Tx are the same in OA SPI, reusing the state names from   */
    /* generic SPI. Different state names also help with debugging. */
    hDevice->spiState = ADI_MAC_SPI_STATE_RX;

    ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, &hDevice->ctrlTxBuf[0], &hDevice->ctrlRxBuf[0], len, useDma);

end:
    return result;
}

adi_eth_Result_e oaPhyRegReadStep(adi_mac_Device_t *hDevice, ADI_MAC_MDIOACC_0__t *mdioCmd)
{
    adi_eth_Result_e        result;
    uint32_t                len;
    bool                    useDma;

    mdioCmd->MDIO_OP = ENUM_MAC_MDIOACC_N__MDIO_OP_MD_RD;
    mdioCmd->MDIO_DATA = 0;
    mdioCmd->MDIO_TRDONE = 0;

    hDevice->regAddr = ADDR_MAC_MDIOACC_1_;
    hDevice->pRegData = &mdioCmd->VALUE32;
    hDevice->cnt = 1;

    len = hDevice->cnt;
    result = oaCtrlSetup(&hDevice->ctrlTxBuf[0], hDevice->wnr, hDevice->regAddr, hDevice->pRegData, &len);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    useDma = (len >= MIN_SIZE_FOR_DMA);
    hDevice->state = ADI_MAC_STATE_DATA_READ_PHY_REGISTER;
    /* Rx/Tx are the same in OA SPI, reusing the state names from   */
    /* generic SPI. Different state names also help with debugging. */
    hDevice->spiState = ADI_MAC_SPI_STATE_RX;

    ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, &hDevice->ctrlTxBuf[0], &hDevice->ctrlRxBuf[0], len, useDma);

end:
    return result;
}

adi_eth_Result_e oaSpiIntHandle(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e        result;
    ADI_MAC_STATUS0_t       status0Masked;
    ADI_MAC_STATUS1_t       status1Masked;
    adi_mac_TimestampRdy_t  timestampReady;
    bool                    useDma;

    status0Masked.VALUE32 = hDevice->statusRegisters.status0Masked;
    status1Masked.VALUE32 = hDevice->statusRegisters.status1Masked;

    /* Link status and link status change behave differently in ADIN2111 */
#if !defined(ADIN2111)
    if (status1Masked.LINK_CHANGE)
    {
        if (hDevice->cbFunc[ADI_MAC_EVT_LINK_CHANGE] != NULL)
        {
            adi_eth_LinkStatus_e    linkStatus = (adi_eth_LinkStatus_e)(hDevice->statusRegisters.status1 & BITM_MAC_STATUS1_P1_LINK_STATUS);
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

    /* Now clear the status bits. Note that we only need to clear the MAC */
    /* status bits, the PHY status bits are cleared on read.              */
    hDevice->wnr = ADI_MAC_SPI_WRITE;
    hDevice->regAddr = ADDR_MAC_STATUS0;
    hDevice->pRegData = (uint32_t *)&hDevice->statusRegisters;
    hDevice->cnt = 2;

    result = oaCtrlSetup(&hDevice->ctrlTxBuf[0], hDevice->wnr, hDevice->regAddr, hDevice->pRegData, &hDevice->cnt);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    useDma = (hDevice->cnt >= MIN_SIZE_FOR_DMA);
    hDevice->state = ADI_MAC_STATE_DATA_START;
    /* Rx/Tx are the same in OA SPI, reusing the state names from   */
    /* generic SPI. Different state names also help with debugging. */
    hDevice->spiState = ADI_MAC_SPI_STATE_RX;

    ADI_HAL_SPI_READ_WRITE(hDevice->adinDevice, &hDevice->ctrlTxBuf[0], &hDevice->ctrlRxBuf[0], hDevice->cnt, useDma);

end:
    return result;
}


/*!
 * @brief           Process transmit queue and create payload for data transactions.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Creates a SPI Tx payload of maximum #ADI_OA_MAX_CHUNK_COUNT chunks,
 *                  based on the available data in the Tx queue and TXC/RCA values
 *                  read from the MAC device.
 *
 *                  This function can be executed as a result of INT_N assertion
 *                  (MAC interrupt handler), in which case it will create a
 *                  one-chunk transaction.
 *
 */
static adi_eth_Result_e oaSpiProcess(adi_mac_Device_t *hDevice)
{
    adi_eth_Result_e        result          = ADI_ETH_SUCCESS;
    uint32_t                chunkCount      = 0;
    uint32_t                txChunkCount    = 0;
    uint32_t                queueByteCount  = 0;
    uint8_t                 *pBuf;

    /* First figure out how many chunks to handle in the SPI transaction */
    if (!queueIsEmpty(&hDevice->txQueue))
    {
        uint32_t txIdx = hDevice->oaTxCurBufIdx;
        queueByteCount = 0;
        do
        {
            queueByteCount += hDevice->txQueue.pEntries[txIdx].pBufDesc->trxSize;
            txIdx++;
            if (txIdx == TX_QUEUE_NUM_ENTRIES_RAW)
            {
                txIdx = 0;
            }
        } while (txIdx != hDevice->txQueue.head);
        /* The current buffer may have been already partially transmitted */
        queueByteCount -= hDevice->oaTxCurBufByteOffset;

        /* Round up the number of chunks */
        chunkCount = (queueByteCount >> hDevice->oaCps) + ((queueByteCount & ((1 << hDevice->oaCps) - 1)) ? 1: 0);

        if (chunkCount > hDevice->oaTxc)
        {
            chunkCount = hDevice->oaTxc;
        }

        /* Save this for later, to indicate if a chunk has Tx data or not. */
        txChunkCount = chunkCount;

    }
    if (hDevice->oaRca > chunkCount)
    {
        chunkCount = hDevice->oaRca;
    }

    if (chunkCount > hDevice->oaMaxChunkCount)
    {
        chunkCount = hDevice->oaMaxChunkCount;
    }

    if (hDevice->oaRxUseBackupBuf && (chunkCount > ADI_OA_RX_BACKUP_BUF_CHUNK_COUNT))
    {
        chunkCount = ADI_OA_RX_BACKUP_BUF_CHUNK_COUNT;
    }

    /* This is from IRQ handler: the host needs to initiate a data transfer in response to an IRQ. */
    if ((!chunkCount) && (hDevice->state == ADI_MAC_STATE_IRQ_START))
    {
        /* Fixed to a minimum transfer size is 1 chunk. */
        chunkCount = 1;
    }

    if (hDevice->oaRxUseBackupBuf)
    {
        pBuf = &hDevice->oaRxBackupBuf[0];
    }
    else
    {
        pBuf = &spiTxBuf[0];
    }

    hDevice->oaTrxSize = 0;
    for (uint32_t i = 0; i < chunkCount; i++)
    {
        bool txEn = (hDevice->state == ADI_MAC_STATE_IRQ_START) ? false: (i < txChunkCount);
        oaCreateNextChunk(hDevice, &pBuf[hDevice->oaTrxSize], txEn);
        hDevice->oaTrxSize += ADI_SPI_HEADER_SIZE + (1 << hDevice->oaCps);
    }

    return result;
}

/*!
 * @brief           Creates SPI transmit payload for one chunk.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      pBuf        Pointer to the destination buffer for the chunk data.
 * @param [in]      txEn        Enables inserting Tx data into the chunk.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Creates the 4-byte transmit header and the chunk payload data for one chunk.
 *
 *                  The chunk purpose may be to read frame data from the RxFIFO without writing
 *                  any new frame data to the TxFIFO, for example if TXC=0 and RCA>0. This is
 *                  flagged via the txEn parameter: if true it will populate frame data into the
 *                  chunk, if false it will only read.
 *
 */
static adi_eth_Result_e oaCreateNextChunk(adi_mac_Device_t *hDevice, uint8_t *pBuf, bool txEn)
{
    adi_eth_Result_e        result = ADI_ETH_SUCCESS;
    adi_mac_FrameStruct_t   *pFrame = NULL;
    uint32_t                chunkSize = (1 << hDevice->oaCps);
    uint32_t                chunkBytesRemaining = chunkSize;
    uint32_t                chunkByteIdx = ADI_SPI_HEADER_SIZE;
    adi_mac_OaTxHeader_t    oaTxHeader;
    bool                    firstChunk = false;
    uint32_t                mem32;

    oaTxHeader.VALUE32 = 0x00000000;
    oaTxHeader.DNC = 1;

    /* If there are no buffers ready for Rx, ask the MAC to not send data to host */
    if ((hDevice->oaRxUseBackupBuf) || queueIsEmpty(hDevice->pRxQueue))
    {
        oaTxHeader.NORX = 1;
    }

    // FIXME: queueIsFull: can this ever be false in the previous implementation?
    if (txEn && (queueIsFull(&hDevice->txQueue) || (hDevice->oaTxCurBufIdx != hDevice->txQueue.head)))
    {
        pFrame = &hDevice->txQueue.pEntries[hDevice->oaTxCurBufIdx];
    }

    if (pFrame == NULL)
    {
        oaTxHeader.DV = 0;
    }
    else
    {
        oaTxHeader.DV = 1;

        /* Have we already transmitted bytes from the frame? */
        if (!hDevice->oaTxCurBufByteOffset)
        {
            firstChunk = true;
        }

        uint32_t bytesRemaining = pFrame->pBufDesc->trxSize - hDevice->oaTxCurBufByteOffset;
        uint32_t    bsize;


        if (bytesRemaining > chunkBytesRemaining)
        {
            bsize = chunkBytesRemaining;
        }
        else
        {
            bsize = bytesRemaining;
        }
        bytesRemaining -= bsize;
        chunkBytesRemaining -= bsize;

        memcpy(&pBuf[chunkByteIdx], &pFrame->pBufDesc->pBuf[hDevice->oaTxCurBufByteOffset], bsize);
        hDevice->oaTxCurBufByteOffset += bsize;

        if (firstChunk)
        {
            oaTxHeader.SV = 1;
            oaTxHeader.SWO = 0;
            oaTxHeader.TMSC = (uint32_t)(pFrame->pBufDesc->egressCapt);

#if defined(ADIN2111)
            oaTxHeader.VS = pFrame->pBufDesc->port & 0x1;
#endif
            firstChunk = false;
        }
        if (bytesRemaining == 0)
        {
            oaTxHeader.EV = 1;
            oaTxHeader.EBO = (bsize - 1);
            hDevice->oaTxCurBufIdx++;
            if (hDevice->oaTxCurBufIdx == TX_QUEUE_NUM_ENTRIES_RAW)
            {
                hDevice->oaTxCurBufIdx = 0;
            }
            hDevice->oaTxCurBufByteOffset = 0;
        }

        if ((chunkBytesRemaining >= 4) && (hDevice->oaTxCurBufIdx != hDevice->txQueue.head))
        {
            /* There is room in the chunk to start transmit next frame */
            /* However we need to make sure this would not lead to a need */
            /* for duplicate SV or EV. */

            /* If there is already a valid SV, do not try to start a new frame */
            if (!oaTxHeader.SV)
            {
                pFrame = &hDevice->txQueue.pEntries[hDevice->oaTxCurBufIdx];
                uint32_t swo = (bsize + 3) / 4;
                /* If we already have a valid EV, make sure the new frame is not shorter */
                /* than the available bytes in the chunk. */
                if (oaTxHeader.EV && (pFrame->pBufDesc->trxSize + swo * 4 <= chunkSize))
                {
                    swo++;
                }

                chunkByteIdx = chunkByteIdx + swo * 4;
                chunkBytesRemaining = chunkSize - swo * 4;
                memcpy(&pBuf[chunkByteIdx], &pFrame->pBufDesc->pBuf[hDevice->oaTxCurBufByteOffset], chunkBytesRemaining);
                hDevice->oaTxCurBufByteOffset += chunkBytesRemaining;

                oaTxHeader.SV = 1;
                oaTxHeader.SWO = swo;

                oaTxHeader.TMSC = (uint32_t)(pFrame->pBufDesc->egressCapt);
            }
        }

    }

    oaTxHeader.P = 1;
    oaTxHeader.P = MAC_CalculateParity((uint8_t *)&oaTxHeader.VALUE32, 4);

    mem32 = HTON32(oaTxHeader.VALUE32);
    memcpy(pBuf, &mem32, 4);

    return result;
}

/*!
 * @brief           Sets up a control transaction.
 *
 * @param [in]      pBuf        Pointer to the destination buffer for the transaction data.
 * @param [in]      wnr         Transaction type.
 * @param [in]      regAddr     Register address.
 * @param [in]      pRegData    Pointer to the register data.
 * @param [out]     pLen        Pointer to the register data length.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Creates a control transaction data, including the control command header.
 *
 */
static adi_eth_Result_e oaCtrlSetup(uint8_t *pBuf, uint32_t wnr, uint32_t regAddr, uint32_t *pRegData, uint32_t *pLen)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            byteLen;

    /* Size of header and echoed header */
    byteLen = 2 * ADI_SPI_HEADER_SIZE;
    /* Convert words to bytes */
    byteLen += ADI_MAC_SPI_ACCESS_SIZE * (*pLen);
#if defined(SPI_PROT_EN)
    /* Protection enabled */
    byteLen += ADI_MAC_SPI_ACCESS_SIZE * (*pLen);
#endif

    if (byteLen > (ADI_MAC_SPI_CTRL_BUF_SIZE - 2))
    {
      result = ADI_ETH_INVALID_PARAM;
      goto end;
    }

    oaCtrlCmdHeader(pBuf, wnr, regAddr, *pLen);

    if (wnr == ADI_MAC_SPI_WRITE)
    {
        result = oaCtrlCmdWriteData(&pBuf[ADI_SPI_HEADER_SIZE], (uint32_t *)pRegData, (*pLen));
    }

    *pLen = byteLen;

end:
    return result;
}

/*!
 * @brief           Creates the control command header.
 *
 * @param [in]      p           Pointer to the destination buffer for the header.
 * @param [in]      wnr         Transaction type.
 * @param [in]      addr        Register address.
 * @param [in]      cnt         Number of registers.
 *
 */
static void oaCtrlCmdHeader(uint8_t *p, uint32_t wnr, uint32_t addr, uint32_t cnt)
{
    adi_mac_OaCtrlCmdHeader_t   header;
    uint32_t                    mem32;

    header.DNC = 0;
    header.HDRB = 0;
    header.WNR = wnr;
    header.AID = 0;
    if (addr >= ADDR_MAC_TX_FSIZE)
    {
        header.MMS = 1;
    }
    else
    {
        header.MMS = 0;
    }
    header.ADDR = addr;
    header.LEN = (cnt - 1);
    header.P = 1;
    header.P = MAC_CalculateParity((uint8_t *)&header.VALUE32, 4);

    mem32 = HTON32(header.VALUE32);
    memcpy(p, &mem32, 4);
}

/*!
 * @brief           Read control data.
 *
 * @param [out]     dst         Pointer to register data buffer.
 * @param [in]      src         Pointer to read transaction data.
 * @param [in]      cnt         Number of registers to read.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *                  - #ADI_ETH_PROTECTION_ERROR     Integrity check failure.
 *
 * @details         Reads the data received from a control transaction and converts it
 *                  to 32-but register data.
 *
 *                  If protection is enabled (#SPI_PROT_EN), it checks the integrity of the
 *                  received data and return #ADI_ETH_PROTECTION_ERROR in case of failure.
 *
 */
static adi_eth_Result_e oaCtrlCmdReadData(uint32_t *dst, uint8_t *src, uint32_t cnt)
{
    adi_eth_Result_e     result = ADI_ETH_SUCCESS;

#if defined(SPI_PROT_EN)
    uint32_t            val32[2];

    /* Protection enabled */
    for (uint32_t i = 0; i < cnt; i++)
    {
        val32[0] = ((uint32_t *)src)[2 * i];
        val32[1] = ((uint32_t *)src)[2 * i + 1];
        if (val32[0] != ~val32[1])
        {
            result = ADI_ETH_PROTECTION_ERROR;
            goto end;
        }
        dst[i] = HTON32(val32[0]);
    }
end:

#else
    /* Protection disabled */
    for (uint32_t i = 0; i < cnt; i++)
    {
        dst[i] = HTON32(((uint32_t *)src)[i]);
    }

#endif

    return result;
}

/*!
 * @brief           Write control data.
 *
 * @param [out]     dst         Pointer to write transaction data.
 * @param [in]      src         Pointer to source register data.
 * @param [in]      cnt         Number of registers to write.

 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Populates the control transaction data with values to be written
 *                  to registers.
 *
 *                  If protection is enabled (#SPI_PROT_EN), it will add the integrity
 *                  check values as defined by the OPEN Alliance specification.
 *
 */
static adi_eth_Result_e oaCtrlCmdWriteData(uint8_t *dst, uint32_t *src, uint32_t cnt)
{
    adi_eth_Result_e     result = ADI_ETH_SUCCESS;

#if defined(SPI_PROT_EN)
      /* Protection enabled */
    for (uint32_t i = 0; i < cnt; i++)
    {
        ((uint32_t *)dst)[2 * i] = HTON32(src[i]);
        ((uint32_t *)dst)[2 * i + 1] = HTON32(~src[i]);
    }

#else
    /* Protection disabled */
    for (uint32_t i = 0; i < cnt; i++)
    {
        ((uint32_t *)dst)[i] = HTON32(src[i]);
    }

#endif

    return result;
}

/*!
 * @brief           Send frame to the MAC.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      pFrame      Pointer to the frame definition.
 *
 * @return          Status
 *                  - #ADI_ETH_SUCCESS              Call completed successfully.
 *
 * @details         Executes the state machine to start sending a frame to the MAC.
 *
 */
adi_eth_Result_e MAC_SendFrame(adi_mac_Device_t *hDevice, adi_mac_FrameStruct_t *pFrame)
{
    adi_eth_Result_e          result = ADI_ETH_SUCCESS;

    hDevice->state = ADI_MAC_STATE_DATA_START;
    result = oaStateMachine(hDevice);

    return result;
}
