/*! *****************************************************************************
 * @file:  AD74412.H
 * @brief: AD74412 Device Abstraction Layer (DAL)
 -----------------------------------------------------------------------------
Copyright (c) 2017,2018 Analog Devices, Inc.

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


/** @addtogroup adi_ad74412 Device Abstraction Layer
 *  @ingroup AD74412_API
 *  @{
 *
 *  @brief   Device Abstraction Layer (DAL) API
 *  @details The set of functions in this module provide an abstraction layer
 *           between the host MCU and an AD74412.
 */


#ifndef AD74412_H
#define AD74412_H

/* obtain integer types ... */
#include <stdint.h>

/* obtain boolean types ... */
#include <stdbool.h>

/* define required types that are not provided by stdint.h or stdbool.h ... */
typedef bool                bool_t;
typedef char                char_t;

#define TRUE true
#define FALSE false

#define SPI_WR_FRAME_BYTES 4


                
#define VOLTAGE_OUTPUT_MIN 0.0
#define VOLTAGE_OUTPUT_MAX 11.0
                
#define CURRENT_OUTPUT_MIN 0.0
#define CURRENT_OUTPUT_MAX 0.025
                    
#define OUTPUT_RANGE_11V        (0x1FFF)
#define OUTPUT_RANGE_10V        ((OUTPUT_RANGE_11V * 10) / 11)

/*********************************************************************************
                                    API CUSTOMISATION
*********************************************************************************/

/*! Define how many Chips this application instance needs to support. */
#define MAX_SUPPORTED_AD74412                 (2)

/*! Set to 1 if this is a big endian system. */
#define BIG_ENDIAN                            (0)
/*********************************************************************************
                                    API IMPLEMENTATION
*********************************************************************************/

typedef uint16_t AD74412_API_REF;

/*! Invalid Chip API Reference */
#define AD74412_API_REF_INVALID 0

// The longest SPI tranaction is one where we read all registers (plus one additional frame)
// times the frame length
#define MAX_SPI_TRANSACTION_SIZE (0x00000048 * sizeof(uint32_t))

typedef enum {
    Channel_A = 0,
    Channel_B = 1,
    Channel_C = 2,
    Channel_D = 3
} tAD74412ChannelID;
#define NUM_AD74412_CHANNELS 4

typedef enum {
    ChannelValueTypeFree,
    ChannelValueTypeResistance,
    ChannelValueTypeCurrent,
    ChannelValueTypeVoltage
} tChannelValueType;

typedef struct {
    tChannelValueType Type;
    float             aValue; // 0..300K.
} tValue;

typedef struct {
    tAD74412ChannelID   ChannelID;
    tValue              ChannelValue;
} tChannelValue;

// Definitions of any/All externals Pins that the chip can signal that interaction is required with it.
typedef enum {
    ADC_DATA_RDY,
    ALERT
} tExternalPin;

typedef enum {
    NotificationNone,
    NotificationResetOccurred,
    NotificationCalibrationMemoryError,
    NotificationSPICRCErrorDetected,
    NotificationSPISCLKCountErrorDetected,
    NotificationADCSaturationError,
    NotificationADCConversionError,

    ALDO1V8_Below_Threshold,     /*! ALDO18V below threshold */
    DVCC_Below_Threshold,        /*! DVCC    below threshold */
    AVDD_Below_Threshold,        /*! AVDD    below threshold */
    ALDO5V_Below_Threshold,      /*! ALDO5V  below threshold */
    ALDO1V8_Above_Threshold,     /*! ALDO18V above threshold */
    DVCC_Above_Threshold,        /*! DVCC    above threshold */
    AVDD_Above_Threshold,        /*! AVDD    above threshold */
    ALDO5V_Above_Threshold,      /*! ALDO5V  above threshold */

    NotificationChargePumpError,
    NotificationHighTemperature,
    NotificationChannelAError,
    NotificationChannelBError,
    NotificationChannelCError,
    NotificationChannelDError
} tNotificationID;


typedef struct {
    tNotificationID NID;
    union {
        union {
            uint16_t aVoltage;
         } Output;
    } NotificationData;
} tNotification;

typedef enum {
    AD74412_RESULT_OK,
    AD74412_RESULT_NO_RESOURCES,                    /*! Not enough resources available */
    AD74412_RESULT_BSP_FAILURE,                     /*! A failure associated with initialising the MCU . */
    AD74412_RESULT_FAILURE,                         /*! Unknown failure. */
    AD74412_RESULT_UNKNOWN_CHIP,                    // this Chip is not known.
    AD74412_RESULT_INACTIVE_CHIP,                   // this Chip is not initialised.
    AD74412_RESULT_CHANNEL_ERROR,                   // this channel doesn't exist
    AD74412_RESULT_PIN_ERROR,                       // this Pin type doesn't exist
    AD74412_RESULT_CHANNEL_TYPE_ERROR,              // this channel type doesn't exist/notSupported.
    AD74412_RESULT_WRONG_CHANNEL_ACTION_FOR_TYPE,   // this channel isn't configured to do this, setOutput for an Input???
    AD74412_RESULT_SPI_FAILURE,                     /*! Unknown failure. */
    AD74412_RESULT_INVALID_REQUEST,                 /*! Invalid request */
} tAD74412Result;

/*! SPI Transaction ID */
typedef enum {
    TID_NONE,                        /*! Invalid transaction ID */
    TID_HouseKeeping,                /*! Housekeeping */
    TID_SoftwareReset,               /*! Software reset */
    TID_ReadModified,                /*! Reading Modified registers */
    TID_StatusClear,                 /*! Clearing of bits in status register(s) */
    TID_ConfigureChannels,           /*! Configuration of channels */
    TID_BurstReadInit,               /*! Preparation for subsequent burst reads */
    TID_BurstReadResults,            /*! Burst read of results and status registers */
    TID_SetOutput,
    TID_CommitConfigurations_WaitBeforeChannelFunctionChange,
    TID_CommitConfigurations_InitialPreparation,
    TID_CommitConfigurations_Wait_In_HighImpedance,
    TID_CommitConfigurations_SetToDesiredState,
    TID_CommitConfigurations_Wait_UpdateDAC
} tAPIEvent;


/*! Transport layer static data */
typedef struct {
    tAPIEvent               TID;                /*! Identifier which indicates the origin of the transaction request from within the API reference. */
    AD74412_API_REF         anAPI_Ref;              /*! Identifier which indicates the originating API reference. */
    uint16_t                BytesToTx;          /*! Number of bytes pointed to by pData  */
    uint16_t                FrameSize;          /*! The BytesToTx number of bytes will be transmitted in FrameSize chunks framed by chip select  */
    uint16_t                BytesRcvd;          /*! Number of bytes received, and by implication transmitted  */
    uint16_t                DataStorageLen;     /*! Size of buffer (in bytes) pointer to by pData */
    uint8_t*                pData;              /*! Pointer to numberOfSPIPackets of generic spi packet frames  */
    tAD74412Result          Result;             /*! Result associated with the transaction.  */
} tSPITransactionInfo;



typedef enum {
    HIGH_IMPEDANCE = 0,
    VOLTAGE_OUTPUT = 1,
    CURRENT_OUTPUT = 2,
    VOLTAGE_INPUT = 3,
    CURRENT_INPUT_EXTERNALLY_POWERED = 4,
    CURRENT_INPUT_LOOP_POWERED = 5,
    RESISTANCE_MEASUREMENT = 6,
    DIGITAL_INPUT_LOGIC = 7,
    DIGITAL_INPUT_LOOP_POWERED = 8,
} tChannelFunction;

typedef enum {
    WriteValue = 0,
    QueueValue = 1,
    WriteQueuedValues = 2
} tSetOutputAction;

typedef tAD74412Result (* pfnNewInputValue )(AD74412_API_REF, tChannelValue *);
typedef tAD74412Result (* pfnStatusChange )(AD74412_API_REF, tNotification);

AD74412_API_REF     AD74412API_Acquire          (void);
tAD74412Result      AD74412API_Release          (AD74412_API_REF);
tAD74412Result      AD74412API_Init             (AD74412_API_REF     anAPI_Ref,
                            pfnNewInputValue    fnNewInputValue,
                            pfnStatusChange     fnStatusChange);

tAD74412Result      AD74412API_PinHandler       (AD74412_API_REF, tExternalPin);
tAD74412Result      AD74412API_ReceivedSPIPacket(AD74412_API_REF, tSPITransactionInfo*);

tAD74412Result      AD74412API_TimerExpiryNotification(AD74412_API_REF anAPI_Ref, tAPIEvent TID);
tAD74412Result      AD74412API_ConfigureChannelAs(AD74412_API_REF, tAD74412ChannelID, tChannelFunction);

tAD74412Result      AD74412API_SetOutput                       (AD74412_API_REF, tChannelValue, tSetOutputAction);
tAD74412Result      AD74412API_CommitConfigurations            (AD74412_API_REF);

#endif // AD74412_H
/*@}*/