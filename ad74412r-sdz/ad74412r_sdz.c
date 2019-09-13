/*! *****************************************************************************
 * @file:  ad74412_sdz.c
 * @brief: Demo Application demonstrating AD74412 Device Abstraction Layer (DAL)
 -----------------------------------------------------------------------------
Copyright (c) 2019 Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors
    manufactured by or for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner
    that would cause the software to become subject to terms and conditions
    which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one
    or more patent holders.  This license does not release you from the
    requirement that you obtain separate licenses from these patent holders
    to use this software.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-
INFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF
CLAIMS OF INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/
#include "AD74412.h"
#include <platform.h>
#include "error.h"
#include <string.h>
#include <stdio.h>

typedef struct
{
    volatile         tValue          LoadValue;
    tValue           Value;

    volatile         tChannelFunction LoadFunction;
    tChannelFunction Function;
} tChannelConfig;

static AD74412_API_REF       anAPI_Ref;


// HIGH_IMPEDANCE
// VOLTAGE_OUTPUT
// CURRENT_OUTPUT
// VOLTAGE_INPUT
// CURRENT_INPUT_EXTERNALLY_POWERED
// CURRENT_INPUT_LOOP_POWERED,
// RESISTANCE_MEASUREMENT
// DIGITAL_INPUT_LOGIC
// DIGITAL_INPUT_LOOP_POWERED

static tChannelConfig        ChnlCfgs[NUM_AD74412_CHANNELS] =
{
    // Channel A
    {
        .LoadFunction   = RESISTANCE_MEASUREMENT,

        .LoadValue      = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        },

        .Function       = HIGH_IMPEDANCE,
        .Value          = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        }
    },

    // Channel B
    {
        .LoadFunction   = HIGH_IMPEDANCE,

        .LoadValue      = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        },

        .Function       = HIGH_IMPEDANCE,
        .Value          = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        }
    },

    // Channel C
    {
        .LoadFunction   = HIGH_IMPEDANCE,

        .LoadValue      = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        },

        .Function       = HIGH_IMPEDANCE,
        .Value          = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        }
    },

    // Channel D
    {
        .LoadFunction   = HIGH_IMPEDANCE,

        .LoadValue      = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        },

        .Function       = HIGH_IMPEDANCE,
        .Value          = {
            .Type   = ChannelValueTypeFree,
            .aValue = 0.0f
        }
    },

};

static char szChannelMsgs[NUM_AD74412_CHANNELS][64] = {{0}, {0}, {0}, {0}};

#define MAX_STRING_SIZE 256
#define SHOW_ADC_RESULTS  1
static char UartBuffer[MAX_STRING_SIZE];

tAD74412Result UioServiceChannelChanges(bool_t force);
void IsrADC_RDY  (void);
void IsrAlert    (void);

tAD74412Result vnALERTTask            (void *pvParameters);
tAD74412Result TaskADC_RDY            (void *pvParameters);
tAD74412Result vnHKTask               (void *pvParameters);
void vUIOTask               (void *pvParameters);

tAD74412Result APP_NewInputValue(AD74412_API_REF anAPI_Ref, tChannelValue *channelValue);
tAD74412Result APP_StatusChange( AD74412_API_REF     anAPI_Ref,
                                 tNotification       Notification);

tAD74412Result UioSystemInit(void);
tAD74412Result UioUpdateFunctions(bool_t force);
tAD74412Result UioUpdateOutputs(bool_t force);
tAD74412Result UioServiceChannelChanges(bool_t force);

volatile bool_t assert_nADC_RDY = FALSE;
volatile bool_t assert_nALERT = FALSE;

/**
 *  \brief Notification from the interrupt system.
 *
 *  \return None
 *
 *  \details The nADC_RDY pin has asserted.
 *           This function is being called from outside the FreeRTOS scheduler
 *           by an interrupt handler.
 *
 *           A new ADC result is ready. Raise the "flag" that TaskADC_RDY()
 *           is waiting on to go retrieve them.
 */
void IsrADC_RDY(void)
{
    assert_nADC_RDY = TRUE;
}

/**
 *  \brief The nADC_RDY pin has asserted and the semaphore is
 *
 *  \param [in] pvParameters Description for pvParameters
 *  \return Return description
 *
 *  \details More details
 */
tAD74412Result TaskADC_RDY (void *pvParameters)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    if (assert_nADC_RDY == TRUE)
    {
        Result = AD74412API_PinHandler(anAPI_Ref,
                                       ADC_DATA_RDY);
        assert_nADC_RDY = FALSE;
    }
    return Result;
}

/**
 *  \brief BSP Interrupt System has flagged an interrupt on nALERT
 *
 *  \return None
 *
 *  \details In the event that we have interrupt capability connected to nALERT
 *           this function will be called to set a flag for us to query the chip
 *
 *           Aletrnatively we could poll the pin in the service loop and set this flag.
 */
void IsrAlert(void)
{
    assert_nALERT = TRUE;
}

/**
 *  \brief Brief description
 *
 *  \param [in] pvParameters Description for pvParameters
 *  \return Return description
 *
 *  \details More details
 */
tAD74412Result vnALERTTask (void *pvParameters)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    if (assert_nALERT == TRUE)
    {
        Result = AD74412API_PinHandler(anAPI_Ref,
                                       ALERT);
        assert_nALERT = FALSE;
    }
    return Result;
}

/**
 *  \brief Brief description
 *
 *  \param [in] pvParameters Description for pvParameters
 *  \return Return description
 *
 *  \details More details
 */
tAD74412Result vnHKTask (void *pvParameters)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    Result = AD74412API_TimerExpiryNotification(anAPI_Ref,
             TID_HouseKeeping);

    return Result;
}

/**
 *  \brief Brief
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] channelValue Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result APP_NewInputValue(AD74412_API_REF anAPI_Ref, tChannelValue *channelValue)
{
    // Move any new values into the UIO_APP storage to be displayed by main.
    tAD74412Result  Result    = AD74412_RESULT_OK;
    char szChannelMsg [64];

    if (channelValue)
    {
        if(channelValue->ChannelID <= Channel_D)
        {
            switch(channelValue->ChannelValue.Type)
            {
            case ChannelValueTypeResistance:
                sprintf(szChannelMsg, "Channel %1d Resistance measured :  %01.0f Ohm",
                        channelValue->ChannelID,
                        channelValue->ChannelValue.aValue);
                break;
            case ChannelValueTypeCurrent:
                sprintf(szChannelMsg, "Channel %1d    Current measured :  %01.1f mA",
                        channelValue->ChannelID,
                        channelValue->ChannelValue.aValue * 1000);
                break;
            case ChannelValueTypeVoltage:
                sprintf(szChannelMsg, "Channel %1d    Voltage measured :  %01.2f V",
                        channelValue->ChannelID,
                        channelValue->ChannelValue.aValue);
                break;
            default:
                sprintf(szChannelMsg, "Channel %1d UNKNOWN",
                        channelValue->ChannelID);
                break;
            }

            if(strcmp(szChannelMsgs[channelValue->ChannelID], szChannelMsg))
            {
                strcpy(szChannelMsgs[channelValue->ChannelID], szChannelMsg);
            }

            sprintf(UartBuffer,
                    "%021lu %s\n",
                    0UL,
                    szChannelMsg);
            (void)BSP_UARTTxBlocking((uint8_t *)UartBuffer, strlen(UartBuffer));

        }
        else
            Result = AD74412_RESULT_CHANNEL_ERROR;
    }

    return Result;
}

/**
 *  \brief Chip is notifying us about a condition on the chip
 *
 *  \param [in] anAPI_Ref The chip API handle that the notification is originating from
 *  \param [in] Notification
 *  \return None
 *
 *  \details
 */
tAD74412Result APP_StatusChange( AD74412_API_REF     anAPI_Ref,
                                 tNotification       Notification)
{
    // Move any new values into the UIO_APP storage to be displayed by main.
    tAD74412Result  Result    = AD74412_RESULT_OK;

    switch(Notification.NID)
    {
    case NotificationNone:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationNone\n",
                0UL);
        break;
    case NotificationResetOccurred:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationResetOccurred\n",
                0UL);
        break;
    case NotificationCalibrationMemoryError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationCalibrationMemoryError\n",
                0UL);
        break;
    case NotificationSPICRCErrorDetected:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationSPICRCErrorDetected\n",
                0UL);
        break;
    case NotificationSPISCLKCountErrorDetected:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationSPISCLKCountErrorDetected\n",
                0UL);
        break;
    case NotificationADCSaturationError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationADCSaturationError\n",
                0UL);
        break;
    case NotificationADCConversionError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationADCConversionError\n",
                0UL);
        break;
    case ALDO1V8_Below_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change ALDO1V8_Below_Threshold\n",
                0UL);
        break;
    case DVCC_Below_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change DVCC_Below_Threshold\n",
                0UL);
        break;
    case AVDD_Below_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change AVDD_Below_Threshold\n",
                0UL);
        break;
    case ALDO5V_Below_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change ALDO5V_Below_Threshold\n",
                0UL);
        break;

    case ALDO1V8_Above_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change ALDO1V8_Above_Threshold\n",
                0UL);
        break;
    case DVCC_Above_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change DVCC_Above_Threshold\n",
                0UL);
        break;
    case AVDD_Above_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change AVDD_Above_Threshold\n",
                0UL);
        break;
    case ALDO5V_Above_Threshold:
        sprintf(UartBuffer,
                "%021lu Status Change ALDO5V_Above_Threshold\n",
                0UL);
        break;
    case NotificationChargePumpError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationChargePumpError\n",
                0UL);
        break;
    case NotificationHighTemperature:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationHighTemperature\n",
                0UL);
        break;
    case NotificationChannelAError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationChannelAError\n",
                0UL);
        break;
    case NotificationChannelBError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationChannelBError\n",
                0UL);
        break;
    case NotificationChannelCError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationChannelCError\n",
                0UL);
        break;
    case NotificationChannelDError:
        sprintf(UartBuffer,
                "%021lu Status Change NotificationChannelDError\n",
                0UL);
        break;
    default:
        sprintf(UartBuffer,
                "%021lu Status Change UNKNOWN\n",
                0UL);
        break;
    }
    (void)BSP_UARTTxBlocking((uint8_t *)UartBuffer, strlen(UartBuffer));

    return Result;
}


/*!
 * @brief      Initialise the Ad74412 system.
 *
 * @details    Initialise the Ad74412 system.
 *
 * @return     tAD74412Result.
 */
tAD74412Result UioSystemInit(void)
{
    ADI_BSP_RESULT  BspResult       = ADI_BSP_SUCCESS;
    tAD74412Result  AD74412Result   = AD74412_RESULT_OK;

    /* Platform Initialisation */
    BspResult = BSP_Initialisation(IsrADC_RDY, IsrAlert);
    if (BspResult != ADI_BSP_SUCCESS)
    {
        return AD74412_RESULT_BSP_FAILURE;
    }

    anAPI_Ref = AD74412API_Acquire();
    AD74412Result = AD74412API_Init(anAPI_Ref,
                                    APP_NewInputValue,
                                    APP_StatusChange);
    AD74412Result  = UioServiceChannelChanges(TRUE);



    return AD74412Result;
}

/**
 *  \brief Update functions of channels
 *
 *  \param [in] force Force an update
 *  \return Return description
 *
 *  \details
 */
tAD74412Result UioUpdateFunctions(bool_t force)
{
    tAD74412Result      AD74412Result   = AD74412_RESULT_OK;
    uint8_t             toCommit;
    /* Any channel function change to apply  */
    toCommit = 0;
    for (tAD74412ChannelID  Channel = Channel_A; (Channel <=  Channel_D) &&  (AD74412Result == AD74412_RESULT_OK); Channel++)
    {
        if (force || (ChnlCfgs[Channel].LoadFunction != ChnlCfgs[Channel].Function))
        {
            if((AD74412Result = AD74412API_ConfigureChannelAs(anAPI_Ref,
                                Channel,
                                ChnlCfgs[Channel].LoadFunction)) != AD74412_RESULT_OK)
            {
                return AD74412Result;
            }
            toCommit += 1;
            ChnlCfgs[Channel].Function = ChnlCfgs[Channel].LoadFunction;
        }
    }
    if (toCommit > 0)
    {
        if((AD74412Result = AD74412API_CommitConfigurations(anAPI_Ref)) != AD74412_RESULT_OK)
        {
            return AD74412Result;
        }
    }
    return AD74412_RESULT_OK;
}

/**
 *  \brief Update output values of channels configured as VOLTAGE or CURRENT_OUTPUT
 *
 *  \param [in] force Force an update
 *  \return Return description
 *
 *  \details
 */
tAD74412Result UioUpdateOutputs(bool_t force)
{
    tAD74412Result      AD74412Result   = AD74412_RESULT_OK;
    uint8_t             toCommit;
    tChannelValue       Value;

    toCommit = 0;
    for (tAD74412ChannelID  Channel = Channel_A; Channel <=  Channel_D; Channel++)
    {
        tChannelValueType ExpectedType = ChannelValueTypeVoltage;
        Value.ChannelID = Channel;

        ExpectedType = ChannelValueTypeVoltage;

        switch (ChnlCfgs[Channel].LoadFunction)
        {
        case CURRENT_OUTPUT:
            ExpectedType = ChannelValueTypeCurrent;
        case VOLTAGE_OUTPUT:
            if((ChnlCfgs[Channel].LoadValue.Type == ExpectedType) && (force || (ChnlCfgs[Channel].LoadValue.aValue != ChnlCfgs[Channel].Value.aValue)))
            {
                toCommit += 1;
                Value.ChannelValue.aValue = ChnlCfgs[Channel].LoadValue.aValue;
                if((AD74412Result = AD74412API_SetOutput(anAPI_Ref,
                                    Value, QueueValue)) != AD74412_RESULT_OK)
                {
                    return AD74412Result;
                }
                ChnlCfgs[Channel].Value = ChnlCfgs[Channel].LoadValue;
            }
            break;
        default:
            break;
        }
    }
    if (toCommit > 0)
    {
        /* Queued up values can be applied in a block now */
        if((AD74412Result = AD74412API_SetOutput(anAPI_Ref,
                            Value, WriteQueuedValues)) != AD74412_RESULT_OK)
        {
            return AD74412Result;
        }
    }

    return AD74412_RESULT_OK;
}


/**
 *  \brief Service any requested channel changes
 *
 *  \return AD74412_RESULT_OK, or error code
 *
 *  \details Channel functions and outputs will be applied in a block to the part.
 */
tAD74412Result UioServiceChannelChanges(bool_t force)
{
    tAD74412Result      AD74412Result   = AD74412_RESULT_OK;

    if((AD74412Result = UioUpdateFunctions(force)) != AD74412_RESULT_OK)
    {
        return AD74412Result;
    }

    if((AD74412Result = UioUpdateOutputs(force)) != AD74412_RESULT_OK)
    {
        return AD74412Result;
    }

    return AD74412_RESULT_OK;
}


/**
 *  \brief Service Loop for AD74412 interaction
 *
 *  \return None
 *
 *  \details Handling configuration, and events from the AD74412
 */
void vUIOTask (void *pvParameters)
{
    tAD74412Result  AD74412Result   = AD74412_RESULT_OK;

    if(UioSystemInit() == AD74412_RESULT_OK)
    {
        /* Service the UIO needs */
        while(1)
        {
            if (AD74412Result == AD74412_RESULT_OK)
            {
                AD74412Result = UioServiceChannelChanges(FALSE);
            }
            if (AD74412Result == AD74412_RESULT_OK)
            {
                AD74412Result = vnALERTTask(NULL);
            }
            if (AD74412Result == AD74412_RESULT_OK)
            {
                AD74412Result = TaskADC_RDY(NULL);
            }
            if (AD74412Result == AD74412_RESULT_OK)
            {
                AD74412Result = vnHKTask(NULL);
            }
            if (AD74412Result != AD74412_RESULT_OK)
            {
                while(1);
            }
        }
    }
}

/**
 *  \brief main
 *
 *  \return Configure and service an AD74412
 *
 *  \details main
 */
int main(void)
{
    vUIOTask(NULL);

    for(;;);
}