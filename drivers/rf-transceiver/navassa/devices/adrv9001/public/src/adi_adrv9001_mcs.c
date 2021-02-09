/**
* \file
* \brief FPGA9001 Multi-Chip Synchronization (MCS) functions
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/
#include "adi_adrv9001_hal.h"
#include "adi_adrv9001_user.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_spi.h"
#include "adrv9001_arm_macros.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"
#include "adrv9001_bf.h"
#include "adi_adrv9001_radio.h"

/* Reads back PLL MCS status register and populates adi_adrv9001_pllSyncStatus_t */
static int32_t adi_adrv9001_Mcs_PllStatus_Get (adi_adrv9001_Device_t *adrv9001,
                                               adi_adrv9001_Pll_e pll,
                                               adi_adrv9001_PllSyncStatus_t *pllStatus)
{
    uint8_t mcsStatusRead = 0;
    adrv9001_BfNvsPllMemMap_e instance;
    static const adrv9001_BfNvsPllMemMap_e instances[] = {
        ADRV9001_BF_RF1_PLL,
        ADRV9001_BF_RF2_PLL,
        ADRV9001_BF_AUX_PLL,
        ADRV9001_BF_CLK_PLL,
        ADRV9001_BF_CLK_PLL_LP
    };

    ADI_NULL_PTR_RETURN(&adrv9001->common, pllStatus);
    /* Check PLL Id is valid */
    ADI_RANGE_CHECK(adrv9001, pll, ADI_ADRV9001_PLL_LO1 , ADI_ADRV9001_PLL_CLK_LP);

    instance = instances[pll];

	ADI_EXPECT(adrv9001_NvsPllMemMap_McsSpiStatus_Get, adrv9001, instance, &mcsStatusRead);

    pllStatus->jesdSyncComplete                   = (bool)((mcsStatusRead & 0x01u) >> 0x00);
    pllStatus->digitalClocksSyncComplete          = (bool)((mcsStatusRead & 0x02u) >> 0x01);
    pllStatus->clockGenDividerSyncComplete        = (bool)((mcsStatusRead & 0x04u) >> 0x02);
    pllStatus->sdmClockDividerSyncComplete        = (bool)((mcsStatusRead & 0x08u) >> 0x03);
    pllStatus->referenceClockDividerSyncComplete  = (bool)((mcsStatusRead & 0x10u) >> 0x04);

    ADI_API_RETURN(adrv9001);
}

/* Reads back digital MCS status register and populates adi_adrv9001_McsStatus_t */
static int32_t adi_adrv9001_Mcs_DigitalStatus_Get (adi_adrv9001_Device_t *adrv9001,
                                                   adi_adrv9001_McsStatus_t *mcsStatus)
{
    uint8_t mcsStatusRead = 0;

    ADI_NULL_PTR_RETURN(&adrv9001->common, mcsStatus);

    /* Read first MCS capture received status bit */
    ADI_EXPECT(adrv9001_NvsRegmapCore_McsCaptureReceived_Get, adrv9001, &mcsStatusRead);
    mcsStatus->firstDigitalSyncComplete  = (bool)mcsStatusRead;

    /* Read second MCS capture received bit */
    ADI_EXPECT(adrv9001_NvsRegmapCore_McsSecondCaptureReceived_Get, adrv9001, &mcsStatusRead);
    mcsStatus->secondDigitalSyncComplete = (bool)mcsStatusRead;

    ADI_API_RETURN(adrv9001);
}

/* Reads back digital Rx LVDS MCS status register and populates adi_adrv9001_RxLvdsSyncStatus_t */
static int32_t adi_adrv9001_Mcs_RxLvdsStatus_Get(adi_adrv9001_Device_t *adrv9001,
                                                 adi_common_ChannelNumber_e channel,
                                                 adi_adrv9001_RxLvdsSyncStatus_t *lvdsStatus)
{
    uint8_t firstLvdsMcsStatusRead, secondLvdsMcsStatusRead = 0;

    ADI_NULL_PTR_RETURN(&adrv9001->common, lvdsStatus);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1 , ADI_CHANNEL_2);

    if (channel == ADI_CHANNEL_1)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1LvdsMcsCaptureReceived_Get, adrv9001, &firstLvdsMcsStatusRead);
        ADI_EXPECT(adrv9001_NvsRegmapCore_Rx1LvdsMcsSecondCaptureReceived_Get, adrv9001, &secondLvdsMcsStatusRead);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2LvdsMcsCaptureReceived_Get, adrv9001, &firstLvdsMcsStatusRead);
        ADI_EXPECT(adrv9001_NvsRegmapCore_Rx2LvdsMcsSecondCaptureReceived_Get, adrv9001, &secondLvdsMcsStatusRead);
    }
    lvdsStatus->lvdsFirstSyncComplete   = (bool)firstLvdsMcsStatusRead;
    lvdsStatus->lvdsSecondSyncComplete  = (bool)secondLvdsMcsStatusRead;

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Mcs_Status_Get(adi_adrv9001_Device_t *adrv9001,
	                                adi_adrv9001_McsStatus_t *mcsStatus)
{
    ADI_ENTRY_EXPECT(adrv9001);
    /* TODO: Consider validating the state. However, since this API is for debug, it might be better to
             keep it flexible.
             This function could be used outside of the MCS_READY state to check the status of the
             registers.
    */

    /* Get analog (PLL) synchronization status */
    ADI_EXPECT(adi_adrv9001_Mcs_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_LO1,    &mcsStatus->rf1PllSyncStatus);   /* RF1 PLL Sync status */
    ADI_EXPECT(adi_adrv9001_Mcs_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_LO2,    &mcsStatus->rf2PllSyncStatus);   /* RF2 PLL Sync status */
    ADI_EXPECT(adi_adrv9001_Mcs_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_CLK,    &mcsStatus->clkPllSyncStatus);   /* CLK PLL Sync status */
    ADI_EXPECT(adi_adrv9001_Mcs_PllStatus_Get, adrv9001, ADI_ADRV9001_PLL_CLK_LP, &mcsStatus->clkPllLpSyncStatus); /* LP CLK PLL Sync status */
    /* Get Rx LVDS Synchronization status */
    ADI_EXPECT(adi_adrv9001_Mcs_RxLvdsStatus_Get, adrv9001, ADI_CHANNEL_1,        &mcsStatus->rx1LvdsSyncStatus);  /* Rx1 LVDS Sync status */
    ADI_EXPECT(adi_adrv9001_Mcs_RxLvdsStatus_Get, adrv9001, ADI_CHANNEL_2,        &mcsStatus->rx2LvdsSyncStatus);  /* Rx2 LVDS Sync status */
    /* Get digital synchronization status */
    ADI_EXPECT(adi_adrv9001_Mcs_DigitalStatus_Get, adrv9001, mcsStatus); /* Digital sync status */

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Mcs_SwStatus_Get(adi_adrv9001_Device_t *adrv9001,
                                      adi_adrv9001_McsSwStatus_e *mcsSwStatus)
{
    adi_adrv9001_RadioState_t currentState = { 0 };

    ADI_ENTRY_EXPECT(adrv9001);
    ADI_EXPECT(adi_adrv9001_Radio_State_Get, adrv9001, &currentState);
    /* If system state is MCS, then ARM repurposes the bootstate registers
       to reflect the MCS status.

       Validation could be added to check if we are in the MCS system state.
       However, validation is left out for now since this is a debug/test function.
       It may be inconvenient for the user/developer if we check the state.
    */
    if ((currentState.systemState == ADI_ADRV9001_ARM_SYSTEM_MCS)
      &&(currentState.mcsState    == ADI_ADRV9001_ARMMCSSTATES_READY))
    {
        *mcsSwStatus = ADI_ADRV9001_MCSSWSTATUS_READY;
    }
    else
    {
        /* As long as the condition above is not true, the we can set the mcsStatus
           to whatever is in the register.
           If the system state is not MCS, then the mcsStatus field is not valid.

           The bootState is offset by 1 to account for the API enum definition
           not exactly matching the register values due to the addition of ADI_ADRV9001_MCSSTATUS_READY.
        */
        *mcsSwStatus = (adi_adrv9001_McsSwStatus_e)(currentState.bootState + 1);
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Mcs_TxMcsToStrobeSampleLatency_Get (adi_adrv9001_Device_t *adrv9001,
                                                         adi_common_ChannelNumber_e channel,
                                                         uint16_t *latency)
{
    ADI_ENTRY_EXPECT(adrv9001);
	ADI_NULL_PTR_RETURN(&adrv9001->common, latency);
    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1McsVsDataValidCnt_Get, adrv9001, latency);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2McsVsDataValidCnt_Get, adrv9001, latency);
    }

    ADI_API_RETURN(adrv9001);
}

int32_t adi_adrv9001_Mcs_TxMcsSampleDelay_Set (adi_adrv9001_Device_t *adrv9001,
                                               adi_common_ChannelNumber_e channel,
                                               uint16_t delay)
{
    ADI_ENTRY_EXPECT(adrv9001);
    ADI_RANGE_CHECK(adrv9001, channel, ADI_CHANNEL_1 , ADI_CHANNEL_2);

    if (ADI_CHANNEL_1 == channel)
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Tx1McsDelayCnt_Set, adrv9001, delay);
    }
    else
    {
        ADI_EXPECT(adrv9001_NvsRegmapCore_Tx2McsDelayCnt_Set, adrv9001, delay);
    }

    ADI_API_RETURN(adrv9001);
}

