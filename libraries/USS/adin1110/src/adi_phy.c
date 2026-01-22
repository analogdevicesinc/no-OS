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

#include "adi_phy.h"
#include "hal.h"

static adi_eth_Result_e         PHY_Init                    (adi_phy_Device_t **hPhyDevice, adi_phy_DriverConfig_t *cfg, void *adinDevice, HAL_ReadFn_t readFn, HAL_WriteFn_t writeFn);
static adi_eth_Result_e         PHY_UnInit                  (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         PHY_ReInitPhy               (adi_phy_Device_t *hDevice);

static adi_eth_Result_e         PHY_RegisterCallback        (adi_phy_Device_t *hDevice, adi_eth_Callback_t cbFunc, uint32_t cbEvents, void *cbParam);
static adi_eth_Result_e         PHY_ReadIrqStatus           (adi_phy_Device_t *hDevice, uint32_t *status);

static adi_eth_Result_e         PHY_AnAdvTxMode             (adi_phy_Device_t *hDevice, adi_phy_AnAdvTxMode_e txMode);
static adi_eth_Result_e         PHY_AnAdvMstSlvCfg          (adi_phy_Device_t *hDevice, adi_phy_AnAdvMasterSlaveCfg_e msCfg);
static adi_eth_Result_e         PHY_AnEnable                (adi_phy_Device_t *hDevice, bool enable);

static adi_eth_Result_e         PHY_EnterSoftwarePowerdown  (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         PHY_ExitSoftwarePowerdown   (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         PHY_GetSoftwarePowerdown    (adi_phy_Device_t *hDevice, bool *enable);

static adi_eth_Result_e         PHY_GetLinkStatus           (adi_phy_Device_t *hDevice, adi_phy_LinkStatus_e *status);

static adi_eth_Result_e         PHY_Renegotiate             (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         PHY_GetAnStatus             (adi_phy_Device_t *hDevice, adi_phy_AnStatus_t *status);

static adi_eth_Result_e         PHY_Reset                   (adi_phy_Device_t *hDevice, adi_phy_ResetType_e resetType);

static adi_eth_Result_e         PHY_SetLoopbackMode         (adi_phy_Device_t *hDevice, adi_phy_LoopbackMode_e loopbackMode);
static adi_eth_Result_e         PHY_SetTestMode             (adi_phy_Device_t *hDevice, adi_phy_TestMode_e testMode);

static adi_eth_Result_e         PHY_LedEn                   (adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, bool enable);
static adi_eth_Result_e         PHY_LedBlinkTime            (adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, uint32_t blinkOn, uint32_t blinkOff);
static adi_eth_Result_e         PHY_GetCapabilities         (adi_phy_Device_t *hDevice, uint16_t *capabilities);

static adi_eth_Result_e         PHY_Write                   (adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t data);
static adi_eth_Result_e         PHY_Read                    (adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t *data);

static adi_eth_Result_e         PHY_GetMseLinkQuality       (adi_phy_Device_t *hDevice, adi_phy_MseLinkQuality_t *mseLinkQuality);

static adi_eth_Result_e         PHY_FrameGenEn              (adi_phy_Device_t *hDevice, bool enable);
static adi_eth_Result_e         PHY_FrameGenSetMode         (adi_phy_Device_t *hDevice, adi_phy_FrameGenMode_e mode);
static adi_eth_Result_e         PHY_FrameGenSetFrameCnt     (adi_phy_Device_t *hDevice, uint32_t frameCnt);
static adi_eth_Result_e         PHY_FrameGenSetFramePayload (adi_phy_Device_t *hDevice, adi_phy_FrameGenPayload_e payload);
static adi_eth_Result_e         PHY_FrameGenSetFrameLen     (adi_phy_Device_t *hDevice, uint16_t frameLen);
static adi_eth_Result_e         PHY_FrameGenSetIfgLen       (adi_phy_Device_t *hDevice, uint16_t ifgLen);
static adi_eth_Result_e         PHY_FrameGenRestart         (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         PHY_FrameGenDone            (adi_phy_Device_t *hDevice, bool *fgDone);

static adi_eth_Result_e         PHY_FrameChkEn              (adi_phy_Device_t *hDevice, bool enable);
static adi_eth_Result_e         PHY_FrameChkSourceSelect    (adi_phy_Device_t *hDevice, adi_phy_FrameChkSource_e source);
static adi_eth_Result_e         PHY_FrameChkReadFrameCnt    (adi_phy_Device_t *hDevice, uint32_t *cnt);
static adi_eth_Result_e         PHY_FrameChkReadRxErrCnt    (adi_phy_Device_t *hDevice, uint16_t *cnt);
static adi_eth_Result_e         PHY_FrameChkReadErrorCnt    (adi_phy_Device_t *hDevice, adi_phy_FrameChkErrorCounters_t *cnt);

/* Forward declarations. */
static adi_eth_Result_e         checkIdentity               (adi_phy_Device_t *hDevice, uint32_t *modelNum, uint32_t *revNum);
#if defined(ADIN1100)
static adi_eth_Result_e         waitMdio                    (adi_phy_Device_t *hDevice);
#endif
static adi_eth_Result_e         phyInit                     (adi_phy_Device_t *hDevice);
static adi_eth_Result_e         phyStaticConfig             (adi_phy_Device_t *hDevice, uint32_t modelNum, uint32_t revNum);
static void                     irqCb                       (void *pCBParam, uint32_t Event, void *pArg);

static const uint16_t convMseToSqi[ADI_PHY_SQI_NUM_ENTRIES - 1] = {
    0x0A74, 0x084E, 0x0698, 0x053D, 0x0429, 0x034E, 0x02A0
};

/*!
 * @brief           PHY device initialization.
 *
 * @param [in]      hDevice     Device driver handle.
 *
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_Init(adi_phy_Device_t **phDevice, adi_phy_DriverConfig_t *cfg, void *adinDevice, HAL_ReadFn_t readFn, HAL_WriteFn_t writeFn)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    adi_phy_Device_t    *hDevice;

    if (cfg->devMemSize < sizeof(adi_phy_Device_t))
    {
        return ADI_ETH_INVALID_PARAM;
    }

    /* Implies state is uninitialized */
    memset(cfg->pDevMem, 0, cfg->devMemSize);

    *phDevice = (adi_phy_Device_t *)cfg->pDevMem;
    hDevice = *phDevice;
    hDevice->phyAddr = cfg->addr;
    hDevice->irqPending = false;

    hDevice->readFn = readFn;
    hDevice->writeFn = writeFn;

    /* Reset callback settings */
    hDevice->cbFunc = NULL;
    hDevice->cbEvents = 0;

    hDevice->adinDevice = adinDevice;

    /* Disable IRQ whether the interrupt is enabled or not */
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    /* Only required if the driver is configured to use the PHY interrupt */
    if (cfg->enableIrq)
    {
        ADI_HAL_REGISTER_CALLBACK(hDevice->adinDevice, (HAL_Callback_t const *)(irqCb), hDevice );
    }

    result = phyInit(hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        hDevice->state = ADI_PHY_STATE_ERROR;
        goto end;
    }

    if (cfg->enableIrq)
    {
        /* Enable IRQ */
        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);

        /* We may have a pending IRQ that will be services as soon as the IRQ is enabled, */
        /* set pending IRQ to false and if needed, service it here. */
        hDevice->irqPending = false;
    }

end:
    return result;
}

/*!
 * @brief           PHY device uninitialization.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_UnInit(adi_phy_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    if (hDevice == NULL)
    {
        result = ADI_ETH_INVALID_HANDLE;
        goto end;
    }

    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    hDevice->state = ADI_PHY_STATE_UNINITIALIZED;

end:
    return result;
}


adi_eth_Result_e PHY_ReInitPhy(adi_phy_Device_t *hDevice)
{
    return phyInit(hDevice);
}


/*!
 * @brief           Local function called by PHY initialization APIs.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return          - ADI_ETH_SUCCESS
 *                  - ADI_ETH_HW_ERROR
 *
 * @details
 *
 * @sa
 */
static adi_eth_Result_e phyInit(adi_phy_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;
    uint32_t            irqMask;
    bool                flag;
    int32_t             iter;
    uint32_t            modelNum;
    uint32_t            revNum;

    hDevice->state = ADI_PHY_STATE_UNINITIALIZED;

#ifdef ADIN1100
    /* Wait until MDIO interface is up, in case this function is called during the powerup sequence of the PHY. */
    result = waitMdio(hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        hDevice->state = ADI_PHY_STATE_ERROR;
        goto end;
    }
#endif

    /* Checks the identity of the device based on reading of hardware ID registers */
    /* Ensures the device is supported by the driver, otherwise an error is reported. */
    result = checkIdentity(hDevice, &modelNum, &revNum);
    if (result != ADI_ETH_SUCCESS)
    {
        hDevice->state = ADI_PHY_STATE_ERROR;
        goto end;
    }

    /* Go to software powerdown, this may already be achieved through pin strap options. */
    /* Note this is not using the driver function because we use a different timeout     */
    /* scheme to account for the powerup sequence of the system included in this step.   */
    val16 = 1 << BITP_CRSM_SFT_PD_CNTRL_CRSM_SFT_PD;
    result = PHY_Write(hDevice, ADDR_CRSM_SFT_PD_CNTRL, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    iter = ADI_PHY_SYS_RDY_ITER;
    do
    {
        result = PHY_GetSoftwarePowerdown(hDevice, &flag);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    } while (!flag && (--iter));

    /* Values of event enums are identical to respective interrupt masks */
    /* Hardware reset and hardware error interrupts are always enabled   */
    irqMask = ADI_PHY_CRSM_HW_ERROR | BITM_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN | hDevice->cbEvents;
    result = PHY_Write(hDevice, ADDR_CRSM_IRQ_MASK, (irqMask & 0xFFFF));
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = PHY_Write(hDevice, ADDR_PHY_SUBSYS_IRQ_MASK, ((irqMask >> 16) & 0xFFFF));
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Read IRQ status bits to clear them before enabling IRQ. */
    /* Hardware errors could be asserted if for, we don't care about the contents */
    /* so we just discard the read values. */
    result = PHY_Read(hDevice, ADDR_CRSM_IRQ_STATUS, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    if (val16 & ADI_PHY_CRSM_HW_ERROR)
    {
        result = ADI_ETH_HW_ERROR;
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_PHY_SUBSYS_IRQ_STATUS, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    hDevice->irqPending = false;

    /* Static configuration: default settings that are different from hardware reset values */
    result = phyStaticConfig(hDevice, modelNum, revNum);
    if (result != ADI_ETH_SUCCESS)
    {
        result = ADI_ETH_PLACEHOLDER_ERROR;
        goto end;
    }

    /* Make sure auto-negotiation is enabled. */
    result = PHY_AnEnable(hDevice, true);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* At then end of successful initialization, the PHY is in software powerdown. */
    hDevice->state = ADI_PHY_STATE_SOFTWARE_POWERDOWN;

end:
    return result;
}

/**************************************************/
/***                                            ***/
/***             Interrupt Handling             ***/
/***                                            ***/
/**************************************************/

/*
 * @brief
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_RegisterCallback(adi_phy_Device_t *hDevice, adi_eth_Callback_t cbFunc, uint32_t cbEvents, void *cbParam)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    hDevice->cbFunc = cbFunc;
    hDevice->cbEvents = cbEvents;
    hDevice->cbParam = cbParam;

    /* Values of event enums are identical to respective interrupt masks */
    /* Hardware reset and hardware error interrupts are always enabled   */
    uint32_t irqMask = ADI_PHY_CRSM_HW_ERROR | BITM_CRSM_IRQ_MASK_CRSM_HRD_RST_IRQ_EN | hDevice->cbEvents;

    result = PHY_Write(hDevice, ADDR_CRSM_IRQ_MASK, (irqMask & 0xFFFF));
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    result = PHY_Write(hDevice, ADDR_PHY_SUBSYS_IRQ_MASK, ((irqMask >> 16) & 0xFFFF));
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}


/*
 * @brief           PHY interrupt callback.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
static void irqCb(void *pCBParam, uint32_t Event, void *pArg)
{
    adi_phy_Device_t    *hDevice = (adi_phy_Device_t *)pCBParam;

    (void)Event;
    (void)pArg;

    /* Set this flag to ensure the IRQ is handled before other actions are taken */
    /* The interrupt may be triggered by a hardware reset, in which case the PHY */
    /* will likely need to be reconfigured.                                      */
    /* The flag is cleared when interrupt status registers are read. Taking      */
    /* appropriate action is the responsibility of the caller.                   */
    hDevice->irqPending = true;
    if (hDevice->cbFunc != NULL)
    {
        hDevice->cbFunc(hDevice->cbParam, 0, NULL);
    }
}

/*
 * @brief           Read interrupt status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_ReadIrqStatus(adi_phy_Device_t *hDevice, uint32_t *status)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    *status = 0;

    result = PHY_Read(hDevice, ADDR_CRSM_IRQ_STATUS, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    else
    {
        *status = val16;

        result = PHY_Read(hDevice, ADDR_PHY_SUBSYS_IRQ_STATUS, &val16);
        if (result != ADI_ETH_SUCCESS)
        {
            /* Only CRSM_IRQ_STATUS values returned are valid */
            result = ADI_ETH_COMM_ERROR_SECOND;
        }
        *status |= (val16 << 16);

        hDevice->irqPending = false;
    }

end:
    return result;
}

/**************************************************/
/***                                            ***/
/***                Configuration               ***/
/***                                            ***/
/**************************************************/

/*
 * @brief           Configure the advertised transmit operating mode for auto-negotiation.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_AnAdvTxMode(adi_phy_Device_t *hDevice, adi_phy_AnAdvTxMode_e txMode)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_AN_ADV_ABILITY_H, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    val16 &= ~(BITM_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_REQ | BITM_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_ABL);
    if ((txMode == ADI_PHY_AN_ADV_TX_REQ_2P4V) || (txMode == ADI_PHY_AN_ADV_TX_REQ_1P0V_ABLE_2P4V))
    {
      val16 |= (1 << BITP_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_ABL);
    }
    if (txMode == ADI_PHY_AN_ADV_TX_REQ_2P4V)
    {
      val16 |= (1 << BITP_AN_ADV_ABILITY_H_AN_ADV_B10L_TX_LVL_HI_REQ);
    }
    result = PHY_Write(hDevice, ADDR_AN_ADV_ABILITY_H, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
end:
    return result;
}


/*
 * @brief           Configure the advertised master/slave configuration for auto-negotiation.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_AnAdvMstSlvCfg(adi_phy_Device_t *hDevice, adi_phy_AnAdvMasterSlaveCfg_e msCfg)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    /* Forced/Preferred */
    result = PHY_Read(hDevice, ADDR_AN_ADV_ABILITY_L, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    val16 &= ~BITM_AN_ADV_ABILITY_L_AN_ADV_FORCE_MS;
    if ((msCfg == ADI_PHY_AN_ADV_FORCED_MASTER) || (msCfg == ADI_PHY_AN_ADV_FORCED_SLAVE))
    {
        val16 |= (1 << BITP_AN_ADV_ABILITY_L_AN_ADV_FORCE_MS);
    }
    result = PHY_Write(hDevice, ADDR_AN_ADV_ABILITY_L, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Master/Slave */
    result = PHY_Read(hDevice, ADDR_AN_ADV_ABILITY_M, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    val16 &= ~BITM_AN_ADV_ABILITY_M_AN_ADV_MST;
    if ((msCfg == ADI_PHY_AN_ADV_FORCED_MASTER) || (msCfg == ADI_PHY_AN_ADV_PREFFERED_MASTER))
    {
        val16 |= (1 << BITP_AN_ADV_ABILITY_M_AN_ADV_MST);
    }
    result = PHY_Write(hDevice, ADDR_AN_ADV_ABILITY_M, val16);

end:
    return result;
}

/**************************************************/
/***                                            ***/
/***                  Autoneg                   ***/
/***                                            ***/
/**************************************************/

/*
 * @brief           Enable/disable auto-negotiation.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details         It is STRONGLY recommended to never disable auto-negotiation!
 *
 * @sa
 */
adi_eth_Result_e PHY_AnEnable(adi_phy_Device_t *hDevice, bool enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    /* The only other bit in this register is AN_RESTART, need to write 0 to it */
    val16 = (enable? 1: 0) << BITP_AN_CONTROL_AN_EN;
    result = PHY_Write(hDevice, ADDR_AN_CONTROL, val16);

end:
    return result;
}

/*
 * @brief           Restart auto-negotiation.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_Renegotiate(adi_phy_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    /* AN_EN should be 1 or AN_RESTART is ignored. Instead of read-modify-write */
    /* and/or reporting an error, we force AN_EN=1.                             */
    val16 = (1 << BITP_AN_CONTROL_AN_EN) |
            (1 << BITP_AN_CONTROL_AN_RESTART);
    result = PHY_Write(hDevice, ADDR_AN_CONTROL, val16);

end:
    return result;
}




/*
 * @brief           Get auto-negotiation status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_GetAnStatus(adi_phy_Device_t *hDevice, adi_phy_AnStatus_t *status)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    result = PHY_Read(hDevice, ADDR_AN_STATUS, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    status->anComplete = (val16 & BITM_AN_STATUS_AN_COMPLETE) >> BITP_AN_STATUS_AN_COMPLETE;
    if (!status->anComplete)
    {
        goto end;
    }

    /* Use the LL value for the link status. If it indicatesthe link is down,   */
    /* the application will read the link status on its own use GetLinkStatus() */
    status->anLinkStatus = (val16 & BITM_AN_STATUS_AN_LINK_STATUS)? ADI_PHY_LINK_STATUS_UP: ADI_PHY_LINK_STATUS_DOWN;

    /* Master/slave resolution */
    result = PHY_Read(hDevice, ADDR_AN_STATUS_EXTRA, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    switch ((val16 & BITM_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN) >> BITP_AN_STATUS_EXTRA_AN_MS_CONFIG_RSLTN)
    {
        case 0:
            status->anMsResolution = ADI_PHY_AN_MS_RESOLUTION_NOT_RUN;
            break;

        case 1:
            status->anMsResolution = ADI_PHY_AN_MS_RESOLUTION_FAULT;
            break;

        case 2:
            status->anMsResolution = ADI_PHY_AN_MS_RESOLUTION_SLAVE;
            break;

        case 3:
            status->anMsResolution = ADI_PHY_AN_MS_RESOLUTION_MASTER;
            break;
    }

    switch ((val16 & BITM_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN) >> BITP_AN_STATUS_EXTRA_AN_TX_LVL_RSLTN)
    {
        case 0:
            status->anTxMode = ADI_PHY_AN_TX_LEVEL_RESOLUTION_NOT_RUN;
            break;

        case 1:
            status->anTxMode = ADI_PHY_AN_TX_LEVEL_RESERVED;
            break;

      case 2:
            status->anTxMode = ADI_PHY_AN_TX_LEVEL_1P0V;
            break;

        case 3:
            status->anTxMode = ADI_PHY_AN_TX_LEVEL_2P4V;
            break;
    }

end:
    return result;
}



/**************************************************/
/***                                            ***/
/***                Miscellanous                ***/
/***                                            ***/
/**************************************************/

/*
 * @brief           Get PHY device capabilities.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_GetCapabilities(adi_phy_Device_t *hDevice, uint16_t *capabilities)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    /* Read capabilities and store in driver struct */
    *capabilities = ADI_PHY_CAP_NONE;
    result = PHY_Read(hDevice, ADDR_B10L_PMA_STAT, &val16);
    if (result == ADI_ETH_SUCCESS)
    {
        if (val16 & BITM_B10L_PMA_STAT_B10L_TX_LVL_HI_ABLE)
        {
            *capabilities |= ADI_PHY_CAP_TX_HIGH_LEVEL;
        }
        if (val16 & BITM_B10L_PMA_STAT_B10L_LB_PMA_LOC_ABLE)
        {
            *capabilities |= ADI_PHY_CAP_PMA_LOCAL_LOOPBACK;
        }
    }

end:
    return result;
}

static adi_eth_Result_e setSoftwarePowerdown(adi_phy_Device_t *hDevice, bool enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;
    uint16_t            bitval;
    bool                swpd;
    int32_t             iter = ADI_PHY_SOFT_PD_ITER;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    bitval = (enable)? 1: 0;
    val16 = bitval << BITP_CRSM_SFT_PD_CNTRL_CRSM_SFT_PD;
    result = PHY_Write(hDevice, ADDR_CRSM_SFT_PD_CNTRL, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Wait with timeout for the PHY device to enter the desired state before returning. */
    do
    {
        result = PHY_GetSoftwarePowerdown(hDevice, &swpd);
    } while ((val16 != (uint16_t)swpd) && (--iter));

    if (iter <= 0)
    {
        result = ADI_ETH_READ_STATUS_TIMEOUT;
        goto end;
    }

    if (enable)
    {
        hDevice->state = ADI_PHY_STATE_SOFTWARE_POWERDOWN;
    }
    else
    {
        hDevice->state = ADI_PHY_STATE_OPERATION;
    }

end:
    return result;
}

/*
 * @brief           Enter software powerdown.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_EnterSoftwarePowerdown(adi_phy_Device_t *hDevice)
{
    return setSoftwarePowerdown(hDevice, true);
}

/*
 * @brief           Exit software powerdown.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_ExitSoftwarePowerdown(adi_phy_Device_t *hDevice)
{
    return setSoftwarePowerdown(hDevice, false);
}

/*
 * @brief           Get powerdown status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_GetSoftwarePowerdown(adi_phy_Device_t *hDevice, bool *enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    val16 = 0;
    result = PHY_Read(hDevice, ADDR_CRSM_STAT, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        return ADI_ETH_COMM_ERROR;
    }

    *enable = ((val16 & BITM_CRSM_STAT_CRSM_SFT_PD_RDY) == (1 << BITP_CRSM_STAT_CRSM_SFT_PD_RDY));

end:
    return result;
}

/*
 * @brief           Get link status.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_GetLinkStatus(adi_phy_Device_t *hDevice, adi_phy_LinkStatus_e *status)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    /* Default to link down, applicable when there are errors, */
    /* such as MDIO communication errors.                      */
    *status = ADI_PHY_LINK_STATUS_DOWN;

    /* Reading AN_STATUS register clears all latched bits, take this into account */
    result = PHY_Read(hDevice, ADDR_AN_STATUS, &val16);
    if (result == ADI_ETH_SUCCESS)
    {
        if (val16 & BITM_AN_STATUS_AN_LINK_STATUS)
        {
            *status = ADI_PHY_LINK_STATUS_UP;
        }
        else
        {
            /* Read it again, first record the dropped link in the stats  */
            hDevice->stats.linkDropped++;
            result = PHY_Read(hDevice, ADDR_AN_STATUS, &val16);
            if (result == ADI_ETH_SUCCESS)
            {
                *status = (val16 & BITM_AN_STATUS_AN_LINK_STATUS)? ADI_PHY_LINK_STATUS_UP: ADI_PHY_LINK_STATUS_DOWN;
            }
        }
    }

end:
    return result;
}


/*
 * @brief           Static configuration of the PHY.
 *
 * @param [in]      hDevice     Device driver handle.
 * @return
 *
 * @details         This function is called during the initialization of the driver
 *                  or after a reset event. It configures a subset of the PHY registers
 *                  to change the default behaviour of the device, increase performance, etc.
 *
 * @sa
 */
static adi_eth_Result_e phyStaticConfig(adi_phy_Device_t *hDevice, uint32_t modelNum, uint32_t revNum)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    if (revNum == 0)
    {

        result = PHY_Write(hDevice, 0x1E8C81, 0x0001);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        if (modelNum == 10)
        {
            result = PHY_Write(hDevice, 0x1E8C80, 0x0001);
        }
        else
        {
            result = PHY_Write(hDevice, 0x1E8C80, 0x3636);
        }
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, 0x1E881F, 0x0000);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, 0x018154, 0x00F9);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, 0x1E8C40, 0x000B);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, 0x018008, 0x0003);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x018009, 0x0008);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x018167, 0x2000);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x018168, 0x0008);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x01816B, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181BD, 0x2000);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181BE, 0x0008);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181C2, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181DB, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181E1, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181E7, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x0181EB, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
        result = PHY_Write(hDevice, 0x018143, 0x0400);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        if (modelNum == 10)
        {
            result = PHY_Write(hDevice, 0x1EA400, 0x0001);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
            result = PHY_Write(hDevice, 0x1EA407, 0x0001);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
        }

    }

end:
    return result;
}


/*
 * @brief           Reset the PHY device.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_Reset(adi_phy_Device_t *hDevice, adi_phy_ResetType_e resetType)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (hDevice->irqPending)
    {
        result = ADI_ETH_IRQ_PENDING;
        goto end;
    }

    switch (resetType)
    {
        case ADI_PHY_RESET_TYPE_SW:

            val16 = (1 << BITP_CRSM_SFT_RST_CRSM_SFT_RST);
            result = PHY_Write(hDevice, ADDR_CRSM_SFT_RST, val16);
            if (result != ADI_ETH_SUCCESS)
            {
                result = ADI_ETH_COMM_ERROR;
            }
            else
            {
                result = phyInit(hDevice);
            }

            break;

        default:
            result = ADI_ETH_UNSUPPORTED_FEATURE;
    }

end:
    return result;
}

/*
 * @brief           Configures a PHY loopback mode.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_SetLoopbackMode(adi_phy_Device_t *hDevice, adi_phy_LoopbackMode_e loopbackMode)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            valMacIfLoopback;
    uint16_t            valB10lPcsCntrl;
    uint16_t            valB10lPmaCntrl;
    bool                prevModePmaPcs = false;

    result = PHY_Read(hDevice, ADDR_MAC_IF_LOOPBACK, &valMacIfLoopback);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    /* Disable loopback */
    valMacIfLoopback &= ~((1 << BITP_MAC_IF_LOOPBACK_MAC_IF_LB_EN) | (1 << BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN));

    result = PHY_Read(hDevice, ADDR_B10L_PCS_CNTRL, &valB10lPcsCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    if (valB10lPcsCntrl & BITM_B10L_PCS_CNTRL_B10L_LB_PCS_EN)
    {
        prevModePmaPcs = true;
    }
    /* Disable loopback */
    valB10lPcsCntrl &= ~(1 << BITP_B10L_PCS_CNTRL_B10L_LB_PCS_EN);

    result = PHY_Read(hDevice, ADDR_B10L_PMA_CNTRL, &valB10lPmaCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    if (valB10lPmaCntrl & BITM_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN)
    {
        prevModePmaPcs = true;
    }
    /* Disable loopback */
    valB10lPmaCntrl &= ~(1 << BITP_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN);

    if (prevModePmaPcs || (loopbackMode == ADI_PHY_LOOPBACK_PCS) || (loopbackMode == ADI_PHY_LOOPBACK_PMA))
    {
        result = PHY_EnterSoftwarePowerdown(hDevice);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        if ((loopbackMode == ADI_PHY_LOOPBACK_PCS) || (loopbackMode == ADI_PHY_LOOPBACK_PMA))
        {
            result = PHY_Write(hDevice, ADDR_AN_CONTROL, 0);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
            result = PHY_Write(hDevice, ADDR_AN_FRC_MODE_EN, 1);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
        }
        else
        {
            result = PHY_Write(hDevice, ADDR_AN_FRC_MODE_EN, 0);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
            result = PHY_Write(hDevice, ADDR_AN_CONTROL, 1);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
        }

    }

    switch (loopbackMode)
    {
        case ADI_PHY_LOOPBACK_NONE:
            break;
        case ADI_PHY_LOOPBACK_PCS:
            valB10lPcsCntrl |= (1 << BITP_B10L_PCS_CNTRL_B10L_LB_PCS_EN);
            break;
        case ADI_PHY_LOOPBACK_PMA:
            valB10lPmaCntrl |= (1 << BITP_B10L_PMA_CNTRL_B10L_LB_PMA_LOC_EN);
            break;
        case ADI_PHY_LOOPBACK_MACIF:
            valMacIfLoopback |= (1 << BITP_MAC_IF_LOOPBACK_MAC_IF_LB_EN);
            break;
        case ADI_PHY_LOOPBACK_MACIF_SUPPRESS_TX:
            valMacIfLoopback |= ((1 << BITP_MAC_IF_LOOPBACK_MAC_IF_LB_EN) | (1 << BITP_MAC_IF_LOOPBACK_MAC_IF_LB_TX_SUP_EN));
            break;
        case ADI_PHY_LOOPBACK_MACIF_REMOTE:
            valMacIfLoopback |= (1 << BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN);
            break;
        case ADI_PHY_LOOPBACK_MACIF_REMOTE_SUPPRESS_RX:
            valMacIfLoopback |= ((1 << BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_EN) | (1 << BITP_MAC_IF_LOOPBACK_MAC_IF_REM_LB_RX_SUP_EN));
            break;
        default:
            result = ADI_ETH_INVALID_PARAM;
            goto end;

    }

    result = PHY_Write(hDevice, ADDR_MAC_IF_LOOPBACK, valMacIfLoopback);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Write(hDevice, ADDR_B10L_PCS_CNTRL, valB10lPcsCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Write(hDevice, ADDR_B10L_PMA_CNTRL, valB10lPmaCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (prevModePmaPcs || (loopbackMode == ADI_PHY_LOOPBACK_PCS) || (loopbackMode == ADI_PHY_LOOPBACK_PMA))
    {
        result = PHY_ExitSoftwarePowerdown(hDevice);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }
end:
    return result;
}

/*
 * @brief           Configures a PHY test mode.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details
 *
 * @sa
 */
adi_eth_Result_e PHY_SetTestMode(adi_phy_Device_t *hDevice, adi_phy_TestMode_e testMode)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            b10lPmaCntrl = 0;
    uint16_t            testModeCntrl = 0;

    if ((testMode != ADI_PHY_TEST_MODE_NONE) && (testMode != ADI_PHY_TEST_MODE_1) &&
        (testMode != ADI_PHY_TEST_MODE_2) && (testMode != ADI_PHY_TEST_MODE_3) &&
        (testMode != ADI_PHY_TEST_MODE_TX_DISABLE))
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    result = PHY_EnterSoftwarePowerdown(hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_B10L_PMA_CNTRL, &b10lPmaCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* For most test modes we want to disable the transmit disable mode */
    b10lPmaCntrl &= ~BITM_B10L_PMA_CNTRL_B10L_TX_DIS_MODE_EN;

    if (testMode == ADI_PHY_TEST_MODE_NONE)
    {
        /* No need for read-modify-write, register has only one bitfield */
        result = PHY_Write(hDevice, ADDR_B10L_TEST_MODE_CNTRL, ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_NONE);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, ADDR_AN_FRC_MODE_EN, 0);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, ADDR_AN_CONTROL, 1);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

    }
    else
    {
        result = PHY_Write(hDevice, ADDR_AN_CONTROL, 0);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, ADDR_AN_FRC_MODE_EN, 1);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        if (testMode == ADI_PHY_TEST_MODE_TX_DISABLE)
        {
            b10lPmaCntrl |= BITM_B10L_PMA_CNTRL_B10L_TX_DIS_MODE_EN;
        }
        else
        {
            switch (testMode)
            {
                case ADI_PHY_TEST_MODE_1:
                    testModeCntrl = (ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_JITTER << BITP_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE);
                    break;

                case ADI_PHY_TEST_MODE_2:
                    testModeCntrl = (ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_DROOP << BITP_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE);
                    break;

                case ADI_PHY_TEST_MODE_3:
                    testModeCntrl = (ENUM_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE_IEEE_TX_TM_IDLE << BITP_B10L_TEST_MODE_CNTRL_B10L_TX_TEST_MODE);
                    break;

                case ADI_PHY_TEST_MODE_NONE:
                case ADI_PHY_TEST_MODE_TX_DISABLE:
                    break;

            }

            /* No need for read-modify-write, register has only one bitfield */
            result = PHY_Write(hDevice, ADDR_B10L_TEST_MODE_CNTRL, testModeCntrl);
            if (result != ADI_ETH_SUCCESS)
            {
                goto end;
            }
        }
    }

    result = PHY_Write(hDevice, ADDR_B10L_PMA_CNTRL, b10lPmaCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_ExitSoftwarePowerdown(hDevice);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

/*
 * @brief           Enable or disable the LED.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [in]      ledSel      Selection of the LED port.
 * @param [in]      enable      Enable/disable the LED.
 * @return
 *
 * @details
 *
 * @sa
 */
static adi_eth_Result_e PHY_LedEn(adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, bool enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;
    uint16_t            bitval;

    bitval = (enable)? 1: 0;

    result = PHY_Read(hDevice, ADDR_LED_CNTRL, &val16);

    if(ledSel == ADI_PHY_LED_0)
    {
      val16 &= ~BITM_LED_CNTRL_LED0_EN;
      val16 |= bitval << BITP_LED_CNTRL_LED0_EN;
    }
    if(ledSel == ADI_PHY_LED_1)
    {
      val16 &= ~BITM_LED_CNTRL_LED1_EN;
      val16 |= bitval << BITP_LED_CNTRL_LED1_EN;
    }
    result = PHY_Write(hDevice, ADDR_LED_CNTRL, val16);

    return result;
}

/*
 * @brief
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @param [in]      ledSel      Selection of the LED port.
 * @param [in]      blinkOn     LEN blink ON time.
 * @param [in]      blinkOff    LEN blink OFF time.
 * @return
 *
 * @details
 *
 * @sa
 */
static adi_eth_Result_e PHY_LedBlinkTime(adi_phy_Device_t *hDevice, adi_phy_LedPort_e ledSel, uint32_t blinkOn, uint32_t blinkOff)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    uint16_t            val16;

    if ((blinkOn > 255) || (blinkOff > 255))
    {
        result = ADI_ETH_INVALID_PARAM;
        goto end;
    }

    if(ledSel == ADI_PHY_LED_0)
    {
      val16 = ((blinkOn << BITP_LED0_BLINK_TIME_CNTRL_LED0_ON_N4MS) | (blinkOff << BITP_LED0_BLINK_TIME_CNTRL_LED0_OFF_N4MS));
      /* Full register write, no need for read-modify-write. */
      result = PHY_Write(hDevice, ADDR_LED0_BLINK_TIME_CNTRL, val16);
    }
    if(ledSel == ADI_PHY_LED_0)
    {
      val16 = ((blinkOn << BITP_LED1_BLINK_TIME_CNTRL_LED1_ON_N4MS) | (blinkOff << BITP_LED1_BLINK_TIME_CNTRL_LED1_OFF_N4MS));
      /* Full register write, no need for read-modify-write. */
      result = PHY_Write(hDevice, ADDR_LED1_BLINK_TIME_CNTRL, val16);
    }

end:
    return result;
}

/*
* @brief MDIO Write, according to clause 45
 *
 * @param [in] dev - Pointer to the HW driver.
 * @param [in] regAddr - PHY MDIO Register address to write.
 * @param [out] data -  Data to write.
 *
 * @return 0 in case of success, positive error code otherwise.
*/
adi_eth_Result_e PHY_Write(adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t data)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            backup;

    backup = ADI_HAL_GET_ENABLE_IRQ(hDevice->adinDevice);
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    if (hDevice->writeFn(hDevice->phyAddr, regAddr, data) != ADI_HAL_SUCCESS)
    {
        result = ADI_ETH_COMM_ERROR;
    }

    if (backup)
    {
        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
    }

    return result;
}

/*
* @brief MDIO Read, according to clause 45
 *
 * @param [in] instance - Pointer to the HW driver.
 * @param [in] hwAddr - PHY MDIO Hardware address.
 * @param [in] regAddr - PHY MDIO Register address to read.
 * @param [out] data - Pointer to the data buffer.
 *
 * @return 0 in case of success, positive error code otherwise.
*/
adi_eth_Result_e PHY_Read(adi_phy_Device_t *hDevice, uint32_t regAddr, uint16_t *data)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint32_t            backup;

    backup = ADI_HAL_GET_ENABLE_IRQ(hDevice->adinDevice);
    ADI_HAL_DISABLE_IRQ(hDevice->adinDevice);

    /* The only error returned by the HAL function is caused by 2nd TA bit */
    /* not being pulled low, which indicates the MDIO interface on the PHY */
    /* device is not operational.                                          */
    if (hDevice->readFn(hDevice->phyAddr, regAddr, data) != ADI_HAL_SUCCESS)
    {
        result = ADI_ETH_COMM_ERROR;
    }

    if (backup)
    {
        ADI_HAL_ENABLE_IRQ(hDevice->adinDevice);
    }

    return result;
}

adi_eth_Result_e PHY_GetMseLinkQuality(adi_phy_Device_t *hDevice, adi_phy_MseLinkQuality_t *mseLinkQuality)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Read(hDevice, ADDR_MSE_VAL, &mseLinkQuality->mseVal);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    if (mseLinkQuality->mseVal > ADI_PHY_LINK_QUALITY_THR_POOR)
    {
        mseLinkQuality->linkQuality = ADI_PHY_LINK_QUALITY_POOR;
    }
    else
    {
        if (mseLinkQuality->mseVal > ADI_PHY_LINK_QUALITY_THR_MARGINAL)
        {
            mseLinkQuality->linkQuality = ADI_PHY_LINK_QUALITY_MARGINAL;
        }
        else
        {
            mseLinkQuality->linkQuality = ADI_PHY_LINK_QUALITY_GOOD;
        }
    }

    for (mseLinkQuality->sqi = 0; mseLinkQuality->sqi < ADI_PHY_SQI_NUM_ENTRIES - 1; mseLinkQuality->sqi++)
    {
        if (mseLinkQuality->mseVal > convMseToSqi[mseLinkQuality->sqi])
        {
            break;
        }
    }

end:
    return result;
}

adi_eth_Result_e PHY_FrameGenEn(adi_phy_Device_t *hDevice, bool enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            frameCntrl;

    if (enable)
    {
        /* Set frame counter to 0 */
        result = PHY_FrameGenSetFrameCnt(hDevice, 0);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Enable frame generator, single bitfield in the register */
        result = PHY_Write(hDevice, ADDR_FG_EN, (1 << BITP_FG_EN_FG_EN));
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }
    }
    else
    {
        /* Backup of the current value in frame control */
        result = PHY_Read(hDevice, ADDR_FG_CNTRL_RSTRT, &frameCntrl);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Disable frame generation by setting frame control to NONE */
        result = PHY_FrameGenSetFramePayload(hDevice, ADI_PHY_FRAME_GEN_PAYLOAD_NONE);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        /* Note: we may want to wait a frame length (plus interpacket gap) before checking FG_DONE */
        /* FG_DONE can only be checked if the frame generator is not already stopped */
#if 0
        bool                fgDone;
        do
        {
            result = PHY_FrameGenDone(hDevice, &fgDone);
        } while ((result != ADI_ETH_SUCCESS) || !fgDone);
#endif

        /* Disable frame generator and restore the frame control setting */
        result = PHY_Write(hDevice, ADDR_FG_EN, (0 << BITP_FG_EN_FG_EN));
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

        result = PHY_Write(hDevice, ADDR_FG_CNTRL_RSTRT, frameCntrl);
        if (result != ADI_ETH_SUCCESS)
        {
            goto end;
        }

    }

end:
    return result;
}

adi_eth_Result_e PHY_FrameGenSetMode(adi_phy_Device_t *hDevice, adi_phy_FrameGenMode_e mode)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Write(hDevice, ADDR_FG_CONT_MODE_EN, (mode << BITP_FG_CONT_MODE_EN_FG_CONT_MODE_EN));

    return result;
}

adi_eth_Result_e PHY_FrameGenSetFrameCnt(adi_phy_Device_t *hDevice, uint32_t frameCnt)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    val16 = (frameCnt >> 16) & 0xFFFF;
    result = PHY_Write(hDevice, ADDR_FG_NFRM_H, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    val16 = frameCnt & 0xFFFF;
    result = PHY_Write(hDevice, ADDR_FG_NFRM_L, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e PHY_FrameGenSetFramePayload(adi_phy_Device_t *hDevice, adi_phy_FrameGenPayload_e payload)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    /* This includes setting FG_RSTRT=0 */
    val16 = 0x0000;
    val16 |= (payload << BITP_FG_CNTRL_RSTRT_FG_CNTRL);
    result = PHY_Write(hDevice, ADDR_FG_CNTRL_RSTRT, val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e PHY_FrameGenSetFrameLen(adi_phy_Device_t *hDevice, uint16_t frameLen)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Write(hDevice, ADDR_FG_FRM_LEN, (frameLen << BITP_FG_FRM_LEN_FG_FRM_LEN));

    return result;
}

adi_eth_Result_e PHY_FrameGenSetIfgLen(adi_phy_Device_t *hDevice, uint16_t ifgLen)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    /* Single 16-bit bitfield */
    result = PHY_Write(hDevice, ADDR_FG_IFG_LEN, ifgLen);

    return result;
}

adi_eth_Result_e PHY_FrameGenRestart(adi_phy_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            frameCntrl;
    bool                fgDone;

    /* Before restart, clear FG_DONE explicitly in case it was set before and not cleared */
    /* Discard the read value, only use of the read is to clear FG_DONE */
    result = PHY_FrameGenDone(hDevice, &fgDone);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    (void)fgDone;

    result = PHY_Read(hDevice, ADDR_FG_CNTRL_RSTRT, &frameCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    frameCntrl |= (1 << BITP_FG_CNTRL_RSTRT_FG_RSTRT);
    result = PHY_Write(hDevice, ADDR_FG_CNTRL_RSTRT, frameCntrl);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

end:
    return result;
}

adi_eth_Result_e PHY_FrameGenDone(adi_phy_Device_t *hDevice, bool *fgDone)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    result = PHY_Read(hDevice, ADDR_FG_DONE, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    *fgDone = (BITM_FG_DONE_FG_DONE & val16);

end:
    return result;
}

adi_eth_Result_e PHY_FrameChkEn(adi_phy_Device_t *hDevice, bool enable)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    if (enable)
    {
        val16 = 1 << BITP_FC_EN_FC_EN;
    }
    else
    {
        val16 = 0 << BITP_FC_EN_FC_EN;
    }

    result = PHY_Write(hDevice, ADDR_FC_EN, val16);

    return result;
}

adi_eth_Result_e PHY_FrameChkSourceSelect(adi_phy_Device_t *hDevice, adi_phy_FrameChkSource_e source)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Write(hDevice, ADDR_FC_TX_SEL, (source << BITP_FC_TX_SEL_FC_TX_SEL));

    return result;
}

adi_eth_Result_e PHY_FrameChkReadFrameCnt(adi_phy_Device_t *hDevice, uint32_t *cnt)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

    result = PHY_Read(hDevice, ADDR_FC_FRM_CNT_H, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    *cnt = (val16 << 16);

    result = PHY_Read(hDevice, ADDR_FC_FRM_CNT_L, &val16);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    *cnt |= val16;

end:
    return result;
}

adi_eth_Result_e PHY_FrameChkReadRxErrCnt(adi_phy_Device_t *hDevice, uint16_t *cnt)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Read(hDevice, ADDR_RX_ERR_CNT, cnt);

    return result;
}

adi_eth_Result_e PHY_FrameChkReadErrorCnt(adi_phy_Device_t *hDevice, adi_phy_FrameChkErrorCounters_t *cnt)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;

    result = PHY_Read(hDevice, ADDR_FC_LEN_ERR_CNT, &cnt->LEN_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_ALGN_ERR_CNT, &cnt->ALGN_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_SYMB_ERR_CNT, &cnt->SYMB_ERR_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_OSZ_CNT, &cnt->OSZ_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_USZ_CNT, &cnt->USZ_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_ODD_CNT, &cnt->ODD_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_ODD_PRE_CNT, &cnt->ODD_PRE_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    result = PHY_Read(hDevice, ADDR_FC_FALSE_CARRIER_CNT, &cnt->FALSE_CARRIER_CNT);
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
end:
    return result;
}

/**************************************************/
/***                                            ***/
/***              Static Functions              ***/
/***                                            ***/
/**************************************************/

/*
 * @brief           Check thedevice identity.
 *
 * @param [in]      hDevice     Device driver handle.
 * @param [out]
 * @return
 *
 * @details         This function is called after a reset event and before the
 *                  initialization of the device is performed.
 *                  It reads the MMD1_DEV_ID1/MMD1_DEV_ID2 registers and compares
 *                  them with expected values. If comparison is not successful,
 *                  return ADI_PHY_UNSUPPORTED_DEVID
 *
 * @sa
 */
static adi_eth_Result_e checkIdentity(adi_phy_Device_t *hDevice, uint32_t *modelNum, uint32_t *revNum)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    uint16_t            val16;

#if defined(MDIO_CL22)
    result = PHY_Read(hDevice, ADDR_MI_PHY_ID1, &val16);
#else
      result = PHY_Read(hDevice, ADDR_MMD1_DEV_ID1, &val16);
#endif
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }
    if (val16 != ADI_PHY_DEVID1)
    {
        result = ADI_ETH_UNSUPPORTED_DEVICE;
        goto end;
    }

#if defined(MDIO_CL22)
      result = PHY_Read(hDevice, ADDR_MI_PHY_ID2, &val16);
#else
      result = PHY_Read(hDevice, ADDR_MMD1_DEV_ID2, &val16);
#endif
    if (result != ADI_ETH_SUCCESS)
    {
        goto end;
    }

    /* Check if the value of MMD1_DEV_ID2.OUI matches expected value */
    if ((val16 & BITM_MMD1_DEV_ID2_MMD1_DEV_ID2_OUI) != (ADI_PHY_DEVID2_OUI << BITP_MMD1_DEV_ID2_MMD1_DEV_ID2_OUI))
    {
        result = ADI_ETH_UNSUPPORTED_DEVICE;
        goto end;
    }

    *modelNum = (uint32_t)((val16 & BITM_MMD1_DEV_ID2_MMD1_MODEL_NUM) >> BITP_MMD1_DEV_ID2_MMD1_MODEL_NUM);
    *revNum = (uint32_t)((val16 & BITM_MMD1_DEV_ID2_MMD1_REV_NUM) >> BITP_MMD1_DEV_ID2_MMD1_REV_NUM);

end:
    return result;
}

#if defined(ADIN1100)
static adi_eth_Result_e waitMdio(adi_phy_Device_t *hDevice)
{
    adi_eth_Result_e    result = ADI_ETH_SUCCESS;
    int32_t             iter = ADI_PHY_MDIO_POWERUP_ITER;
    uint16_t            val16;

    /* Wait until MDIO interface is up */
    /* The MDIO reads will return all 1s as data values before the interface is up, poll DEVID1 register. */
    do
    {
#if defined(MDIO_CL22)
        result = PHY_Read(hDevice, ADDR_MI_PHY_ID1, &val16);
#else
        result = PHY_Read(hDevice, ADDR_MMD1_DEV_ID1, &val16);
#endif

    } while ((result == ADI_ETH_COMM_ERROR) && (--iter));

    if(result == ADI_ETH_COMM_ERROR)
    {
        goto end;
    }

end:
    return result;
}
#endif

adi_phy_DriverEntry_t phyDriverEntry =
{
    PHY_Init,
    PHY_UnInit,
    PHY_ReInitPhy,
    PHY_RegisterCallback,
    PHY_ReadIrqStatus,
    PHY_EnterSoftwarePowerdown,
    PHY_ExitSoftwarePowerdown,
    PHY_GetSoftwarePowerdown,
    PHY_GetLinkStatus,
    PHY_AnAdvTxMode,
    PHY_AnAdvMstSlvCfg,
    PHY_AnEnable,
    PHY_Renegotiate,
    PHY_GetAnStatus,
    PHY_Reset,
    PHY_SetLoopbackMode,
    PHY_SetTestMode,
    PHY_LedEn,
    PHY_LedBlinkTime,
    PHY_GetCapabilities,
    PHY_Read,
    PHY_Write,
    PHY_GetMseLinkQuality,
    PHY_FrameGenEn,
    PHY_FrameGenSetMode,
    PHY_FrameGenSetFrameCnt,
    PHY_FrameGenSetFramePayload,
    PHY_FrameGenSetFrameLen,
    PHY_FrameGenSetIfgLen,
    PHY_FrameGenRestart,
    PHY_FrameGenDone,
    PHY_FrameChkEn,
    PHY_FrameChkSourceSelect,
    PHY_FrameChkReadFrameCnt,
    PHY_FrameChkReadRxErrCnt,
    PHY_FrameChkReadErrorCnt,
};

