/*! *****************************************************************************
 * @file:  AD74412.C
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

#include "AD74412.H"
#include <string.h>
#include <ad74412_regs.h>
#include "error.h"
#include "spi.h"
#include "delay.h"

#define GET_FIELD(regname,fieldname,regvalue) ((regvalue & BITM_##regname##_##fieldname) >> BITP_##regname##_##fieldname)
#define GET_FIELD_NS(regname,fieldname,regvalue) ((regvalue & BITM_##regname##_##fieldname))
#define SET_FIELD(regname,fieldname,regvalue,fieldvalue)  regvalue = ((regvalue & ~BITM_##regname##_##fieldname) | ((uint16_t)fieldvalue << BITP_##regname##_##fieldname))
#define SET_FIELD_NS(regname,fieldname,regvalue,fieldvalue)  regvalue = ((regvalue & ~BITM_##regname##_##fieldname) | ((uint16_t)fieldvalue))


#define SPI_READ_FRAMES_DELAY 0x1 /* The read of the register appears this number of frames later */

#define BA_GET_ADDR(array) (uint8_t)(array[0] & 0x7F)
#define BA_GET_DATA(array) (uint16_t)  ((((uint16_t)array[1]) << 8 ) | array[2])
#define BA_GET_CRC(array)  (uint8_t)  array[3]

#define LAST_ADDR_INDEX   REG_AD74412_SILICON_REV
#define NUM_AD74412_REGS (LAST_ADDR_INDEX + 1)  // Not all of these are used



#define REG_STATUS_UPDT  0x1 /* Value has been read from the chip */
#define REG_STATUS_DIRTY 0x2 /* Value needs to be written to chip */

#define CHANNEL_MINIMUM_TIME_us 130

#define RSENSE_VAL  100
#define RSENSEH_VAL 2000
#define RSENSEL_VAL 2000

#define MAX_ADC_CODE 0xFFFF

/*! API storage definition */
typedef struct AD74412_API
{
    bool                inUse;    /*! Currently in use (or not) */
    AD74412_API_REF     anAPI_Ref;    /*! The API reference for this definition */
    bool                APIisInitialised;
    uint16_t            RegMap   [NUM_AD74412_REGS];
    uint16_t            RegStatus[NUM_AD74412_REGS];

    uint16_t            CommittedRegChFuncSetup[4];
    uint16_t            CommittedRegAdcConfig  [4];

    pfnNewInputValue    fnNewInputValue;
    pfnStatusChange     fnStatusChange;
    bool                bREAD_SELECTStatusResults;
    bool                nADC_DATA_RDY;

    bool                bALDO1V8_Below_Threshold;
    bool                bDVCC_Below_Threshold;
    bool                bAVDD_Below_Threshold;
    bool                bALDO5V_Below_Threshold;

} AD74412_API, *AD74412_API_Ptr;


/*! API storage for tracking API definitions */
static AD74412_API APIDefinitions[MAX_SUPPORTED_AD74412];

/*! Retrieve API definition structure related to reference */
#define getPtrFromRef(x) (((x!=0) && (x <= MAX_SUPPORTED_AD74412))?(&APIDefinitions[x-1]):0)

/*! The API consumer only sees an AD74412_API_REF, we need to relate this to an element in APIDefinitions in both directions */
#define GENERATE_API_REF_FROM_DEFINDEX(index) (index+1)

static void SPI_AddWriteFrame32(  AD74412_API_Ptr      API_Ptr,
                                  tSPITransactionInfo *pExChng,
                                  uint8_t              Adr,
                                  uint16_t             Data);

static void SPI_AddNOP(AD74412_API_Ptr API_Ptr, tSPITransactionInfo *pExChng);
static tAD74412Result SPI_SoftwareReset(AD74412_API_Ptr API_Ptr);

static void SPI_AddReadRegister(AD74412_API_Ptr       API_Ptr,
                                tSPITransactionInfo *pExChng,
                                uint8_t               Adr,
                                bool_t                bFlush);

static tAD74412Result AD74412API_ApplyOutputs(AD74412_API_Ptr API_Ptr);
static tAD74412Result API_ReadModifiedRegisters               (AD74412_API_Ptr);
static tAD74412Result API_BlockReadResultsAndStatus(AD74412_API_Ptr API_Ptr);
static void InitialiseMemoryMap                   (AD74412_API_Ptr API_Ptr);
static void SPI_InitExchange(tSPITransactionInfo   *pExChng,
                             AD74412_API_Ptr        API_Ptr,
                             tAPIEvent        TID,
                             uint8_t               *pData,
                             uint16_t               DataStorageLen,
                             uint16_t               FrameSize);
static void initialiseAPI                         (AD74412_API_Ptr API_Ptr, AD74412_API_REF anAPI_Ref);

/* Functions that operate on the raw byte array */
static bool_t   BA_CRC      (uint8_t *pFrameBA, bool_t bUpdate);
static bool_t   BA_CRC16    (uint8_t *pFrameBA);
static bool_t   BA_Validate (uint8_t *pFrameBA, uint8_t nAdr);
static void     BA_Create   (uint8_t *pFrameBA, uint8_t nAdr, uint16_t nData);

/*!
 * @brief      Initialise an SPI Exchange Packet.
 *
 * @details    The application gets handed a package of SPI frames to transmit.
 *
 *  \param [in] pExChng Parameter_Description
 *  \param [in] API_Ptr Parameter_Description
 *  \param [in] TID Parameter_Description
 *  \param [in] pData Parameter_Description
 *  \param [in] DataStorageLen Parameter_Description
 *  \param [in] FrameSize Parameter_Description
 *
 * @return     None
 */
static void SPI_InitExchange(tSPITransactionInfo   *pExChng,
                             AD74412_API_Ptr        API_Ptr,
                             tAPIEvent              TID,
                             uint8_t               *pData,
                             uint16_t               DataStorageLen,
                             uint16_t               FrameSize)
{
    pExChng->TID            = TID;
    pExChng->anAPI_Ref      = API_Ptr->anAPI_Ref;
    pExChng->BytesToTx      = 0;
    pExChng->BytesRcvd      = 0;
    pExChng->pData          = pData;
    pExChng->DataStorageLen = DataStorageLen;
    pExChng->FrameSize      = FrameSize;
    pExChng->Result         = AD74412_RESULT_OK;
}

/*!
 * @brief       Add a write frame to a transaction instance
 *
 * @details     Each 32 bit word frame is a write of 16 bit data to a register
  *
 * @param [in]  API_Ptr            : Pointer to API definition to initialise.
 * @param [in]  pExChng            : SPI exchange definition
 * @param [in]  Adr                : Address of register to write to.
 * @param [in]  Data               : Register Data tp write
 *
 */
static void SPI_AddWriteFrame32(  AD74412_API_Ptr      API_Ptr,
                                  tSPITransactionInfo *pExChng,
                                  uint8_t              Adr,
                                  uint16_t             Data)
{
    if (pExChng && pExChng->pData && ((pExChng->BytesToTx + sizeof(uint32_t)) <= pExChng->DataStorageLen))
    {
        if (pExChng->FrameSize == SPI_WR_FRAME_BYTES)
        {
            BA_Create(pExChng->pData + pExChng->BytesToTx, Adr, Data);
            pExChng->BytesToTx += SPI_WR_FRAME_BYTES;
        }
        else
        {
            if (pExChng->FrameSize > SPI_WR_FRAME_BYTES)
            {
                /* A burst read */
                BA_Create(pExChng->pData + pExChng->BytesToTx, Adr, Data);
                pExChng->BytesToTx += pExChng->FrameSize;
            }
            else
            {
                /* An SPI transaction size of less than 4 is not expected.
                   Any existing error should take precedence.
                */
                if(pExChng->Result == AD74412_RESULT_OK)
                    pExChng->Result = AD74412_RESULT_INVALID_REQUEST;
            }
        }

    }
    else
    {
        /* We weren't able to store this write as insufficient resources were available
           Any existing error should take precedence.
        */
        if(pExChng->Result == AD74412_RESULT_OK)
            pExChng->Result = AD74412_RESULT_NO_RESOURCES;
    }
}

/*!
 * @brief      Add a NOP write to flush out data
 *
 * @details    Read the contents of an individual register.
 *
 *             Two SPI frames are used to read a register location.
 *             1) First, write address to READ_SELECT register.
 *             2) Second, any write address to any register (NOP or READ_SELECT or ...)
 *                will return on SDO the contents of the desired register.
 *
 * @param [in]  API_Ptr            : Pointer to API definition to initialise.
 * @param [in]  pExChng            : SPI exchange definition
  *
 * @return     None
 */
static void SPI_AddNOP(AD74412_API_Ptr API_Ptr, tSPITransactionInfo *pExChng)
{
    SPI_AddWriteFrame32(API_Ptr, pExChng, REG_AD74412_NOP, 0x0000);
}

/*!
 * @brief      Add a read packet to the list of spi transactions.
 *
 * @details    Read the contents of an individual register.
 *
 *             Two SPI frames are used to read a register location.
 *             1) First, write address to READ_SELECT register.
 *             2) Second, any write address to any register (NOP or READ_SELECT or ...)
 *                will return on SDO the contents of the desired register.
 *
 * @param [in]  API_Ptr            : Pointer to API definition to initialise.
 * @param [in]  pExChng            : SPI exchange definition
 * @param [in]  Adr                : Address of register to read from.
 * @param [in]  bFlush             : Add a dummy write to the NOP register to flush out data.
 *
 * @return     None
 */
static void SPI_AddReadRegister(AD74412_API_Ptr       API_Ptr,
                                tSPITransactionInfo *pExChng,
                                uint8_t               Adr,
                                bool_t                bFlush)
{

    SPI_AddWriteFrame32(API_Ptr, pExChng, REG_AD74412_READ_SELECT, ((uint16_t)Adr) | BITM_READ_SELECT_AUTO_RD_EN);
    if (bFlush)
        SPI_AddNOP(API_Ptr, pExChng);
}





/*!
 * @brief      Exchange a SPI Packet
 *
 * @param [in]  anAPI_Ref          : API reference
 * @param [in]  pExChng            : SPI exchange definition
 *
 * @return     None
 */
tAD74412Result SPIExchange(AD74412_API_REF anAPI_Ref, tSPITransactionInfo *pExChng)
{
    tAD74412Result myReturn = AD74412_RESULT_OK;

    /* Interface is free.
       Perform blocking transfer of the next SPI frame.
       We will stored the received data in the buffer that
       we transmit from.
    */
    do
    {
        if(SUCCESS != spi_write_and_read(NULL, pExChng->pData + pExChng->BytesRcvd, (uint8_t)pExChng->FrameSize))
        {
            myReturn = AD74412_RESULT_SPI_FAILURE;
            break;
        }
        pExChng->BytesRcvd += pExChng->FrameSize;
    }
    while(pExChng->BytesToTx != pExChng->BytesRcvd);

    /* Transfer this to the API now. */
    if(myReturn == AD74412_RESULT_OK)
    {
        myReturn = AD74412API_ReceivedSPIPacket(anAPI_Ref, pExChng);
    }

    return myReturn;
}


/*!
 * @brief      Perform a software reset of the AD74412
 *
 * @details    Perform a software reset of the AD74412
 *
 * @param [in]  API_Ptr : Pointer to API definition to initialise.
 *
 * @return     None
 */
static tAD74412Result SPI_SoftwareReset(AD74412_API_Ptr API_Ptr)
{
    tAD74412Result                  myReturn;
    tSPITransactionInfo             FrameInfo;
    uint8_t                         FrameBytes[SPI_WR_FRAME_BYTES * 2];

    SPI_InitExchange(&FrameInfo,
                     API_Ptr,
                     TID_SoftwareReset,
                     (uint8_t *)FrameBytes,
                     sizeof(FrameBytes),
                     SPI_WR_FRAME_BYTES);

    /*
    We have to write a sequence of values into the REG_AD74412_CMD_KEY
    register to effect the reset.
    */
    SPI_AddWriteFrame32(API_Ptr,
                        &FrameInfo,
                        REG_AD74412_CMD_KEY,
                        ENUM_CMD_KEY_SW_RST_KEY1);

    SPI_AddWriteFrame32(API_Ptr,
                        &FrameInfo,
                        REG_AD74412_CMD_KEY,
                        ENUM_CMD_KEY_SW_RST_KEY2);

    myReturn = FrameInfo.Result;
    if(myReturn == AD74412_RESULT_OK)
    {
        myReturn = SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);
    }
    return myReturn;
}


/*!
 * @brief      Read all registers for the API instanace.
 *
 * @details    Read the contents of all registers in the API definition
 *
 * @param [in]  API_Ptr : Pointer to API definition to initialise.
 *
  * @return     None
 */
static tAD74412Result API_ReadModifiedRegisters(AD74412_API_Ptr API_Ptr)
{
    tAD74412Result   myReturn = AD74412_RESULT_OK;
    tSPITransactionInfo     FrameInfo;
    uint8_t                 FrameBytes[SPI_WR_FRAME_BYTES * (NUM_AD74412_REGS + 1)];

    SPI_InitExchange(&FrameInfo,
                     API_Ptr,
                     TID_ReadModified,
                     FrameBytes,
                     sizeof(FrameBytes),
                     SPI_WR_FRAME_BYTES);

    for(uint8_t ReadAdr = REG_AD74412_NOP; ReadAdr <= REG_AD74412_SILICON_REV; ReadAdr++)
    {
        if((API_Ptr->RegStatus[ReadAdr] & REG_STATUS_UPDT) == 0x0)
            SPI_AddReadRegister(API_Ptr, &FrameInfo, ReadAdr, false);
    }
    if (FrameInfo.BytesToTx)
    {
        /* Add a final NOP to flush out the last register read */
        SPI_AddNOP(API_Ptr, &FrameInfo);

        myReturn = FrameInfo.Result;
        if(myReturn == AD74412_RESULT_OK)
        {
            myReturn = SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);
        }
    }
    return myReturn;
}

/*!
 * @brief      Read all registers for the API instanace.
 *
 * @details    Read the contents of all registers in the API definition
 *
 * @param [in]  API_Ptr : Pointer to API definition to initialise.
 *
  * @return     None
 */
#define BURST_NUM_BYTES (4 + (((REG_AD74412_LIVE_STATUS - REG_AD74412_ADC_RESULT0)+1) * 3))
static tAD74412Result API_BlockReadResultsAndStatus(AD74412_API_Ptr API_Ptr)
{
    tAD74412Result   myReturn = AD74412_RESULT_OK;
    tSPITransactionInfo     FrameInfo;
    uint8_t                 FrameBytes[BURST_NUM_BYTES];

    if (!API_Ptr->bREAD_SELECTStatusResults)
    {
        /* Read Select is not at the start of the block for us to do a bust read */
        tSPITransactionInfo     FrameInfoRESELECT;
        uint8_t                 FrameBytesRESELECT[SPI_WR_FRAME_BYTES];

        /* A single write to the REG_AD74412_READ_SELECT to position it for a burst read */
        SPI_InitExchange(&FrameInfoRESELECT,
                         API_Ptr,
                         TID_BurstReadInit,
                         FrameBytesRESELECT,
                         sizeof(FrameBytesRESELECT),
                         SPI_WR_FRAME_BYTES);

        SPI_AddWriteFrame32(API_Ptr, &FrameInfoRESELECT, REG_AD74412_READ_SELECT, REG_AD74412_ADC_RESULT0 | BITM_READ_SELECT_AUTO_RD_EN);

        myReturn = FrameInfoRESELECT.Result;
        if(myReturn == AD74412_RESULT_OK)
        {
            myReturn = SPIExchange(API_Ptr->anAPI_Ref, &FrameInfoRESELECT);
        }
    }

    if(myReturn == AD74412_RESULT_OK)
    {
        /* Now try the burst read */
        SPI_InitExchange(&FrameInfo,
                         API_Ptr,
                         TID_BurstReadResults,
                         FrameBytes,
                         sizeof(FrameBytes),
                         BURST_NUM_BYTES);

        memset(FrameBytes, 0x55, sizeof(FrameBytes))    ;
        SPI_AddWriteFrame32(API_Ptr, &FrameInfo, REG_AD74412_NOP, 0x0);

        myReturn = FrameInfo.Result;
        if(myReturn == AD74412_RESULT_OK)
        {
            myReturn = SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);
        }
    }
    return myReturn;
}

/*!
 * @brief      Initialise an API definition.
 *
 * @details    Prepare an API def for first use.
 *
 * @param [in]  API_Ptr : Pointer to API definition to initialise.
  *
 * @return     None
 */
static void InitialiseMemoryMap(AD74412_API_Ptr API_Ptr)
{
    API_Ptr->APIisInitialised = FALSE;
    /* Initialise the memory map */
    memset(API_Ptr->RegMap, 0, sizeof(API_Ptr->RegMap));
    API_Ptr->RegMap[REG_AD74412_NOP] = REG_AD74412_NOP_RESET;
    API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP0] = REG_AD74412_CH_FUNC_SETUP0_RESET;
    API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP1] = REG_AD74412_CH_FUNC_SETUP1_RESET;
    API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP2] = REG_AD74412_CH_FUNC_SETUP2_RESET;
    API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP3] = REG_AD74412_CH_FUNC_SETUP3_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0] = REG_AD74412_ADC_CONFIG0_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONFIG1] = REG_AD74412_ADC_CONFIG1_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONFIG2] = REG_AD74412_ADC_CONFIG2_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONFIG3] = REG_AD74412_ADC_CONFIG3_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0] = REG_AD74412_DIN_CONFIG0_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_CONFIG1] = REG_AD74412_DIN_CONFIG1_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_CONFIG2] = REG_AD74412_DIN_CONFIG2_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_CONFIG3] = REG_AD74412_DIN_CONFIG3_RESET;
    API_Ptr->RegMap[REG_AD74412_GPO_PARALLEL] = REG_AD74412_GPO_PARALLEL_RESET;
    API_Ptr->RegMap[REG_AD74412_GPO_CONFIG0] = REG_AD74412_GPO_CONFIG0_RESET;
    API_Ptr->RegMap[REG_AD74412_GPO_CONFIG1] = REG_AD74412_GPO_CONFIG1_RESET;
    API_Ptr->RegMap[REG_AD74412_GPO_CONFIG2] = REG_AD74412_GPO_CONFIG2_RESET;
    API_Ptr->RegMap[REG_AD74412_GPO_CONFIG3] = REG_AD74412_GPO_CONFIG3_RESET;
    API_Ptr->RegMap[REG_AD74412_OUTPUT_CONFIG0] = REG_AD74412_OUTPUT_CONFIG0_RESET;
    API_Ptr->RegMap[REG_AD74412_OUTPUT_CONFIG1] = REG_AD74412_OUTPUT_CONFIG1_RESET;
    API_Ptr->RegMap[REG_AD74412_OUTPUT_CONFIG2] = REG_AD74412_OUTPUT_CONFIG2_RESET;
    API_Ptr->RegMap[REG_AD74412_OUTPUT_CONFIG3] = REG_AD74412_OUTPUT_CONFIG3_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CODE0] = REG_AD74412_DAC_CODE0_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CODE1] = REG_AD74412_DAC_CODE1_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CODE2] = REG_AD74412_DAC_CODE2_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CODE3] = REG_AD74412_DAC_CODE3_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CLR_CODE0] = REG_AD74412_DAC_CLR_CODE0_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CLR_CODE1] = REG_AD74412_DAC_CLR_CODE1_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CLR_CODE2] = REG_AD74412_DAC_CLR_CODE2_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_CLR_CODE3] = REG_AD74412_DAC_CLR_CODE3_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_ACTIVE0] = REG_AD74412_DAC_ACTIVE0_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_ACTIVE1] = REG_AD74412_DAC_ACTIVE1_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_ACTIVE2] = REG_AD74412_DAC_ACTIVE2_RESET;
    API_Ptr->RegMap[REG_AD74412_DAC_ACTIVE3] = REG_AD74412_DAC_ACTIVE3_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_THRESH] = REG_AD74412_DIN_THRESH_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] = REG_AD74412_ADC_CONV_CTRL_RESET;
    API_Ptr->RegMap[REG_AD74412_DIAG_ASSIGN] = REG_AD74412_DIAG_ASSIGN_RESET;
    API_Ptr->RegMap[REG_AD74412_DIN_COMP_OUT] = REG_AD74412_DIN_COMP_OUT_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_RESULT0] = REG_AD74412_ADC_RESULT0_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_RESULT1] = REG_AD74412_ADC_RESULT1_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_RESULT2] = REG_AD74412_ADC_RESULT2_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_RESULT3] = REG_AD74412_ADC_RESULT3_RESET;
    API_Ptr->RegMap[REG_AD74412_DIAG_RESULT0] = REG_AD74412_DIAG_RESULT0_RESET;
    API_Ptr->RegMap[REG_AD74412_DIAG_RESULT1] = REG_AD74412_DIAG_RESULT1_RESET;
    API_Ptr->RegMap[REG_AD74412_DIAG_RESULT2] = REG_AD74412_DIAG_RESULT2_RESET;
    API_Ptr->RegMap[REG_AD74412_DIAG_RESULT3] = REG_AD74412_DIAG_RESULT3_RESET;
    API_Ptr->RegMap[REG_AD74412_ALERT_STATUS] = REG_AD74412_ALERT_STATUS_RESET;
    API_Ptr->RegMap[REG_AD74412_LIVE_STATUS] = REG_AD74412_LIVE_STATUS_RESET;
    API_Ptr->RegMap[REG_AD74412_ALERT_MASK] = REG_AD74412_ALERT_MASK_RESET;
    API_Ptr->RegMap[REG_AD74412_READ_SELECT] = REG_AD74412_READ_SELECT_RESET;
    API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL_40SPS] = REG_AD74412_ADC_CONV_CTRL_40SPS_RESET;
    API_Ptr->RegMap[REG_AD74412_THERM_RST] = REG_AD74412_THERM_RST_RESET;
    API_Ptr->RegMap[REG_AD74412_CMD_KEY] = REG_AD74412_CMD_KEY_RESET;
    API_Ptr->RegMap[REG_AD74412_SCRATCH] = REG_AD74412_SCRATCH_RESET;
    API_Ptr->RegMap[REG_AD74412_SILICON_REV] = REG_AD74412_SILICON_REV_RESET;

    /* Initialise the memory map status */
    memset(API_Ptr->RegStatus, 0, sizeof(API_Ptr->RegStatus));

    /* Initialise the committed configuration cache for adc result processing */
    memset(API_Ptr->CommittedRegChFuncSetup, 0, sizeof(API_Ptr->CommittedRegChFuncSetup));
    memset(API_Ptr->CommittedRegAdcConfig, 0, sizeof(API_Ptr->CommittedRegAdcConfig));


    for(uint8_t ReadAdr = REG_AD74412_NOP; ReadAdr <= REG_AD74412_SILICON_REV; ReadAdr++)
    {
        switch (ReadAdr)
        {
        case REG_AD74412_ALERT_STATUS:
        case REG_AD74412_LIVE_STATUS:
            break;
        default:
            /* No need to read these from the chip..accept reset */
            API_Ptr->RegStatus[ReadAdr] |= REG_STATUS_UPDT;
            break;
        }
    }

}

/*!
 * @brief      Initialise an API definition.
 *
 * @details    Prepare an API def for first use.
 *
 * @param [in]  API_Ptr : Pointer to API definition to initialise.
 *
 * @param [in]  anAPI_Ref   : The API definition reference for this API definition
 *
 * @return     None
 */
static void initialiseAPI(AD74412_API_Ptr API_Ptr, AD74412_API_REF anAPI_Ref)
{
    if (API_Ptr)
    {
        memset(API_Ptr, 0, sizeof(AD74412_API));
        API_Ptr->inUse = TRUE;
        API_Ptr->anAPI_Ref = anAPI_Ref;
        InitialiseMemoryMap(API_Ptr);
    }
}


/*********************************************************************************
                                    API IMPLEMENTATIONS
*********************************************************************************/
/**
 *  \brief After acquiring the API, it must be initialised
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] fnNewInputValue Parameter_Description
 *  \param [in] fnStatusChange Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result AD74412API_Init(AD74412_API_REF anAPI_Ref,
                               pfnNewInputValue    fnNewInputValue,
                               pfnStatusChange     fnStatusChange)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr  = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        if (API_Ptr->inUse)
        {
            InitialiseMemoryMap(API_Ptr);
            API_Ptr->fnNewInputValue = fnNewInputValue;
            API_Ptr->fnStatusChange = fnStatusChange;
            Result = SPI_SoftwareReset(API_Ptr);
        }
        else
        {
            /* releasing something that is not acquired is an indication
               of something wrong */
            Result = AD74412_RESULT_INACTIVE_CHIP;
        }
    }
    else
        Result = AD74412_RESULT_UNKNOWN_CHIP;
    return Result;
}


/*!
 * @brief      Reserve and allocate a AD74412_API_REF
 *
 * @details    The number of AD74412 chips that the API can handle is finite and is defined
 *             at compilation time. This can be increased to suit the number of chips in the
 *             system by modifying MAX_SUPPORTED_AD74412.
 *
 * @return     AD74412_API_REF
 *                - #AD74412_API_REF_INVALID             All available AD74412API are in use
 *                - Non-zero value                       Valid AD74412_API_REF
 */
AD74412_API_REF AD74412API_Acquire()
{
    AD74412_API_REF anAPI_Ref = AD74412_API_REF_INVALID;
    uint32_t        Index;

    for (Index = 0; Index < MAX_SUPPORTED_AD74412; Index++)
    {
        if (!APIDefinitions[Index].inUse)
        {
            anAPI_Ref = GENERATE_API_REF_FROM_DEFINDEX(Index);
            initialiseAPI(getPtrFromRef(anAPI_Ref), anAPI_Ref);
            break;
        }
    }

    return anAPI_Ref;
}
/*!
 * @brief      Release an AD74412_API_REF (obtained via acquireAD74412API)
 *
 * @details    The number of AD74412 chips that the API can handle is finite and is defined
 *             at compilation time. This can be increased to suit the number of chips in the
 *             system by modifying MAX_SUPPORTED_AD74412.
 *             Call acquireAD74412API() to acquire one of these definitions and release ito
 *             with a call to releaseAD4412API
 *
 * @return     tAD74412Result
 *                - #AD74412_API_REF_INVALID             All available AD74412API are in use
 *                - Non-zero value                       Valid AD74412_API_REF
  */
tAD74412Result AD74412API_Release(AD74412_API_REF anAPI_Ref)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr  = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        if (API_Ptr->inUse)
        {
            API_Ptr->inUse = FALSE;
        }
        else
        {
            /* releasing something that is not acquired is an indication
               of something wrong */
            Result = AD74412_RESULT_INACTIVE_CHIP;
        }
    }
    return Result;
}

/**
 *  \brief Brief
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] theActivePin Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result AD74412API_PinHandler(AD74412_API_REF anAPI_Ref, tExternalPin theActivePin)
{
    tAD74412Result myReturnCode = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        // Valid ptr.
        /* Looks inside data Pointer and decides what to do about the Pin */
        switch (theActivePin)
        {

        case ADC_DATA_RDY:
            API_Ptr->nADC_DATA_RDY = true;
        case ALERT:
        {
            API_Ptr->RegStatus[REG_AD74412_ALERT_STATUS]  &= ~REG_STATUS_UPDT;
            API_Ptr->RegStatus[REG_AD74412_LIVE_STATUS]  &= ~REG_STATUS_UPDT;
            /* Read all the marked registers */
            myReturnCode = API_BlockReadResultsAndStatus(API_Ptr);
        }
        break;
        default:
        {
            myReturnCode = AD74412_RESULT_PIN_ERROR;
        }
        break;
        }
    }

    return myReturnCode;
}

/**
 *  \brief Brief
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] ch Parameter_Description
 *  \param [in] ChannelType Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
static void EnableChannelConversions(AD74412_API_Ptr    API_Ptr,
                                     tAD74412ChannelID  Channel,
                                     bool               bEnable)
{
    uint16_t    Mask = 1UL << (BITP_ADC_CONV_CTRL_CH_A_EN + Channel);

    if(bEnable)
    {
        API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] |= Mask;
    }
    else
    {
        API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] &= ~Mask;
    }
    API_Ptr->RegStatus[REG_AD74412_ADC_CONV_CTRL]  |= REG_STATUS_DIRTY;
}


/**
 *  \brief Brief
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] ch Parameter_Description
 *  \param [in] ChannelType Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result AD74412API_ConfigureChannelAs(AD74412_API_REF    anAPI_Ref,
        tAD74412ChannelID  Channel,
        tChannelFunction     ChannelType)
{

    tAD74412Result theReturnCode = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        // Valid ptr.
        SET_FIELD(CH_FUNC_SETUP,
                  CH_FUNC,
                  API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP0 + Channel],
                  ChannelType);

        API_Ptr->RegStatus[REG_AD74412_CH_FUNC_SETUP0 + Channel]  |= REG_STATUS_DIRTY;

        /* Some ADC_CONFIG and DIN_CONFIG bitfields will update automatically
           once the CH_FUNC register is written...so we don't have to write this reisgter
           unless we modifiy different bits.
        */
        switch (ChannelType)
        {
        case HIGH_IMPEDANCE:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0 - 10V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_10V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            /* Ensure DAC_CODEn for channel set to zero */
            API_Ptr->RegMap     [REG_AD74412_DAC_CODE0 + Channel] = 0;
            API_Ptr->RegStatus  [REG_AD74412_DAC_CODE0 + Channel] |= REG_STATUS_DIRTY;
            EnableChannelConversions(API_Ptr, Channel, false);
            break;
        case VOLTAGE_OUTPUT:
            /* Voltage across sense resistor */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_100OHM);
            /* -2.5V - +2.5V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_NEG2P5_2P5V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            /* Measures the current through the RSENSE resistor in a 2.5mA to
               +2.5mA range
            */
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case CURRENT_OUTPUT:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0 - 10V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_10V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case VOLTAGE_INPUT:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0 - 10V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_10V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case CURRENT_INPUT_EXTERNALLY_POWERED:
            /* Voltage across sense resistor */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_100OHM);
            /* -2.5V - +0V This comment needs to be considered given the change below */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_2P5V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case CURRENT_INPUT_LOOP_POWERED:
            /* Voltage across sense resistor */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_100OHM);
            /* 0V - +2.5V  Again, check the comment */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_NEG2P5_0V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            EnableChannelConversions(API_Ptr, Channel, false);
            break;
        case RESISTANCE_MEASUREMENT:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0V - +2.5V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_2P5V);
            /* Comparator Disabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         0);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case DIGITAL_INPUT_LOGIC:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0 - +10V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_10V);
            /* Comparator Enabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         BITM_DIN_CONFIG_COMPARATOR_EN);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        case DIGITAL_INPUT_LOOP_POWERED:
            /* Voltage across screw terminals */
            SET_FIELD_NS(ADC_CONFIG,
                         ADC_MUX,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_V_ACROSS_SCREW);
            /* 0 - +10V */
            SET_FIELD_NS(ADC_CONFIG,
                         RANGE,
                         API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0 + Channel],
                         ENUM_ADC_CONFIG_RNG_0_10V);
            /* Comparator Enabled */
            SET_FIELD_NS(DIN_CONFIG,
                         COMPARATOR_EN,
                         API_Ptr->RegMap[REG_AD74412_DIN_CONFIG0 + Channel],
                         BITM_DIN_CONFIG_COMPARATOR_EN);
            EnableChannelConversions(API_Ptr, Channel, true);
            break;
        }
        // & mark for updating.
        API_Ptr->RegStatus[REG_AD74412_ADC_CONFIG0      + Channel] |= REG_STATUS_DIRTY;
        API_Ptr->RegStatus[REG_AD74412_DIN_CONFIG0      + Channel] |= REG_STATUS_DIRTY;
        API_Ptr->RegStatus[REG_AD74412_CH_FUNC_SETUP0   + Channel] |= REG_STATUS_DIRTY;
    }

    return theReturnCode;
}

/**
 *  \brief ADC Result is ready
 *
 *  \param [in] Channel The channel to look at
 *  \return None
 *
 *  \details Results on a channel are available. Process them.
 */
void HandleADCResult(AD74412_API_Ptr API_Ptr, tAD74412ChannelID Channel)
{
    tChannelFunction Function;
    uint16_t       ADCData;
    tChannelValue  Value;
    float          Rsense = (float)RSENSE_VAL;
    float          VoltageMin;
    float          VoltageMax;
    float          VoltageRange;
    uint16_t       RangeCode;
    float          fRes    = 0.0;

    Value.ChannelID = Channel;
    Value.ChannelValue.Type = ChannelValueTypeVoltage;
    Value.ChannelValue.aValue = 0.0;

    switch (Channel)
    {
    case Channel_A:
    case Channel_B:
    case Channel_C:
    case Channel_D:
        Function = (tChannelFunction)GET_FIELD(CH_FUNC_SETUP,
                                               CH_FUNC,
                                               API_Ptr->CommittedRegChFuncSetup[Channel]);
        ADCData = API_Ptr->RegMap[REG_AD74412_ADC_RESULT0 + Channel];
        RangeCode = GET_FIELD_NS(ADC_CONFIG,
                                 RANGE,
                                 API_Ptr->CommittedRegAdcConfig[Channel]);
        break;
    default:
        Function = HIGH_IMPEDANCE;
        ADCData = 0;
        RangeCode = ENUM_ADC_CONFIG_RNG_0_10V;
        break;
    }

    switch (RangeCode)
    {
    default:
    case ENUM_ADC_CONFIG_RNG_0_10V:            /*  RANGE: 0V to 10V */
        VoltageMin = 0;
        VoltageMax = 10;
        break;
    case ENUM_ADC_CONFIG_RNG_0_2P5V:            /*  RANGE: 0V to 2.5V */
        VoltageMin = 0;
        VoltageMax = 2.5;
        break;
    case ENUM_ADC_CONFIG_RNG_NEG2P5_0V:            /*  RANGE: -2.5V to 0V */
        VoltageMin = -2.5;
        VoltageMax = 0;
        break;
    case ENUM_ADC_CONFIG_RNG_NEG2P5_2P5V:            /*  RANGE: -2.5V to 2.5V */
        VoltageMin = -2.5;
        VoltageMax = 2.5;
        break;
    }

    VoltageRange = VoltageMax - VoltageMin;

    switch (Function)
    {
    case HIGH_IMPEDANCE:
    {
        /* By default, the ADC is configured to measure
           the voltage across the screw terminals
           in a 0 V to 10 V range.
        */
        Value.ChannelValue.Type = ChannelValueTypeVoltage;
        Value.ChannelValue.aValue = ((float)ADCData / (float)MAX_ADC_CODE) * VoltageRange;
    }
    break;
    case VOLTAGE_OUTPUT:
        /* By default, the ADC is configured to measure the current
           through the RSENSE resistor in a -25 mA to +25 mA range */
    {
        Value.ChannelValue.Type = ChannelValueTypeCurrent;
        Value.ChannelValue.aValue = (VoltageMin + (((float)ADCData / MAX_ADC_CODE) * VoltageRange)) / Rsense;
    }
    break;
    case CURRENT_OUTPUT:
    {
        /* By default, the ADC is configured to measure
          the voltage across the screw terminals in a 0 V to 10 V range.
        */
        Value.ChannelValue.Type = ChannelValueTypeVoltage;
        Value.ChannelValue.aValue = ((float)ADCData / (float)MAX_ADC_CODE) * VoltageRange;
    }
    break;
    case DIGITAL_INPUT_LOGIC:
    case DIGITAL_INPUT_LOOP_POWERED:
    /* IF ADC is enabled for this function, then interpret the same as VIN */
    case VOLTAGE_INPUT:
    {
        fRes = ((float)ADCData) * VoltageRange / (float)MAX_ADC_CODE;
        Value.ChannelValue.Type = ChannelValueTypeVoltage;
        Value.ChannelValue.aValue = fRes;
    }
    break;
    case CURRENT_INPUT_EXTERNALLY_POWERED:
    {
        fRes = (ADCData * VoltageRange / MAX_ADC_CODE) / RSENSE_VAL;

        Value.ChannelValue.Type = ChannelValueTypeCurrent;
        Value.ChannelValue.aValue = fRes;
    }


    break;
    case CURRENT_INPUT_LOOP_POWERED:
    {
        /* By default, the ADC is configured to measure current
           flowing from the AD74412R into the screw terminal,
           through the RSENSE resistor in a 25 mA range.  */
        Value.ChannelValue.Type = ChannelValueTypeCurrent;
        Value.ChannelValue.aValue = (((float)ADCData / (float)MAX_ADC_CODE) * VoltageRange) / Rsense;
    }
    break;
    case RESISTANCE_MEASUREMENT:
        /*
        The 16-bit sigma delta (∑-∆) ADC is automatically configured
        to digitize the voltage across the RTD in a 2.5 V range.
        */
        /* Resistance measurement of an external 2-wire RTD
        */
    {

        float       Rpullup = (float)(RSENSE_VAL + RSENSEH_VAL);
        float Vadc = (float)ADCData * Rpullup;

        fRes = Vadc / (MAX_ADC_CODE - ADCData);

        Value.ChannelValue.Type = ChannelValueTypeResistance;
        Value.ChannelValue.aValue = fRes;
    }
    break;

    default:
        break;
    }
    (void)API_Ptr->fnNewInputValue(API_Ptr->anAPI_Ref, &Value);
}

/**
 *  \brief Chip if a chip situation needs to be dealt with.
 *
 *  \param [in] anAPI_Ref Reference to the API instance.
 *  \return tAD74412Result
 *
 *  \details We have updated our copy of the the memory map with
 *           result and status registers. Now, look at these and
 *           perform some operations based on what we see there.
 */
static tAD74412Result AnalyseChipSituation(AD74412_API_REF anAPI_Ref)
{
    tAD74412Result          myReturn = AD74412_RESULT_OK;
    AD74412_API_Ptr         API_Ptr = getPtrFromRef(anAPI_Ref);
    uint8_t                 RegAdr;
    tSPITransactionInfo     ClearStatus;
    uint8_t                 StatusClearWrite[SPI_WR_FRAME_BYTES * 2];
    tNotification           Notification;

    memset(&Notification, 0x0, sizeof(tNotification));
    Notification.NID = NotificationNone;

    SPI_InitExchange(&ClearStatus,
                     API_Ptr,
                     TID_StatusClear,
                     StatusClearWrite,
                     sizeof(StatusClearWrite),
                     SPI_WR_FRAME_BYTES);

    /* Look at the status values */
    RegAdr = REG_AD74412_ALERT_STATUS;
    if (API_Ptr->RegStatus[RegAdr] & REG_STATUS_UPDT)
    {
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_RESET_OCCURRED)
        {
            /*  Reset Occurred. */
            Notification.NID = NotificationResetOccurred;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_CAL_MEM_ERR)
        {
            /*  Calibration Memory Error. */
            Notification.NID = NotificationCalibrationMemoryError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_SPI_CRC_ERR)
        {
            /*  SPI CRC Error Detected */
            Notification.NID = NotificationSPICRCErrorDetected;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_SPI_SCLK_CNT_ERR)
        {
            /*  SPI SCLK Count Error Detected */
            Notification.NID = NotificationSPISCLKCountErrorDetected;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_ADC_SAT_ERR)
        {
            /*  ADC Saturation Error */
            Notification.NID = NotificationADCSaturationError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }

        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_ADC_CONV_ERR)
        {
            /*  ADC Conversion Error */
            Notification.NID = NotificationADCConversionError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_ALDO1V8_ERR)
        {
            /*  ALDO1V8 Power Supply Monitor Error */
            if(!API_Ptr->bALDO1V8_Below_Threshold)
            {
                /* Send a notification if we haven't already */
                API_Ptr->bALDO1V8_Below_Threshold = true;
                Notification.NID = ALDO1V8_Below_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_DVCC_ERR)
        {
            /*  DVCC Power Supply Monitor Error */
            if(!API_Ptr->bDVCC_Below_Threshold)
            {
                /* Send a notification if we haven't already */
                API_Ptr->bDVCC_Below_Threshold = true;
                Notification.NID = DVCC_Below_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }

        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_AVDD_ERR)
        {
            /*  AVDD Power Supply Monitor Error */
            if(!API_Ptr->bAVDD_Below_Threshold)
            {
                /* Send a notification if we haven't already */
                API_Ptr->bAVDD_Below_Threshold = true;
                Notification.NID = AVDD_Below_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_ALDO5V_ERR)
        {
            /*  ALDO5V Power Supply Monitor Error */
            if(!API_Ptr->bALDO5V_Below_Threshold)
            {
                /* Send a notification if we haven't already */
                API_Ptr->bALDO5V_Below_Threshold = true;
                Notification.NID = ALDO5V_Below_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_CHARGE_PUMP_ERR)
        {
            /*  Charge Pump Error Detected */
            Notification.NID = NotificationChargePumpError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);

        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_HI_TEMP_ERR)
        {
            /*  High Temperature Detected */
            Notification.NID = NotificationHighTemperature;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_VI_ERR_D)
        {
            /*  Voltage or Current Error Detected on ChannelD. */
            Notification.NID = NotificationChannelDError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_VI_ERR_C)
        {
            /*  Voltage or Current Error Detected on ChannelC. */
            Notification.NID = NotificationChannelCError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_VI_ERR_B)
        {
            /*  Voltage or Current Error Detected on ChannelB. */
            Notification.NID = NotificationChannelBError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_ALERT_STATUS_VI_ERR_A)
        {
            /*  Voltage or Current Error Detected on ChannelA. */
            Notification.NID = NotificationChannelAError;
            API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
        }
        API_Ptr->RegStatus[RegAdr]  &= ~REG_STATUS_UPDT;

        if (API_Ptr->RegMap[RegAdr])
        {
            /* Write back the bits to clear them */
            SPI_AddWriteFrame32(API_Ptr, &ClearStatus, RegAdr, API_Ptr->RegMap[RegAdr]);
        }
    }

    RegAdr = REG_AD74412_LIVE_STATUS;
    if (API_Ptr->RegStatus[RegAdr] & REG_STATUS_UPDT)
    {
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_ADC_DATA_RDY)
        {

            /* We read these in a burst so POSSIBLY the resulst became ready
               after we read the RESULT register but before we read this status register.
               If the ADC pin asserted before we read it then we know we have the
               latest data. Otherwise, we will read the block again.
            */
            if(API_Ptr->nADC_DATA_RDY)
            {
                /*  ADC Data Ready. Only 1 that is W1C*/
                SPI_AddWriteFrame32(API_Ptr, &ClearStatus, RegAdr, BITM_LIVE_STATUS_ADC_DATA_RDY);

                /* Look at what channels were enabled*/
                if(API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] & BITM_ADC_CONV_CTRL_CH_A_EN)
                {
                    HandleADCResult(API_Ptr, Channel_A);
                }
                if(API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] & BITM_ADC_CONV_CTRL_CH_B_EN)
                {
                    HandleADCResult(API_Ptr, Channel_B);
                }
                if(API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] & BITM_ADC_CONV_CTRL_CH_C_EN)
                {
                    HandleADCResult(API_Ptr, Channel_C);
                }
                if(API_Ptr->RegMap[REG_AD74412_ADC_CONV_CTRL] & BITM_ADC_CONV_CTRL_CH_D_EN)
                {
                    HandleADCResult(API_Ptr, Channel_D);
                }
                API_Ptr->nADC_DATA_RDY = false;
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_ADC_BUSY)
        {
            /*  ADC Busy Status Bit. */
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_ADC_CH_CURR)
        {
            /*  Address of the Channel Currently Being Converted by the ADC. */
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_ALDO1V8_STATUS)
        {
            /*  Current Status of ALDO1V8_ERR */
        }
        else
        {
            if(API_Ptr->bALDO1V8_Below_Threshold)
            {
                /* Condition that was there has been removed*/
                API_Ptr->bALDO1V8_Below_Threshold = false;
                Notification.NID = ALDO1V8_Above_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_DVCC_STATUS)
        {
            /*  Current Status of DVCC_ERR */
        }
        else
        {
            if(API_Ptr->bDVCC_Below_Threshold)
            {
                /* Condition that was there has been removed*/
                API_Ptr->bDVCC_Below_Threshold = false;
                Notification.NID = DVCC_Above_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_AVDD_STATUS)
        {
            /*  Current Status of AVDD_ERR */
        }
        else
        {
            if(API_Ptr->bAVDD_Below_Threshold)
            {
                /* Condition that was there has been removed*/
                API_Ptr->bAVDD_Below_Threshold = false;
                Notification.NID = AVDD_Above_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_ALDO5V_STATUS)
        {
            /*  Current Status of ALDO5V_ERR */
        }
        else
        {
            if(API_Ptr->bALDO5V_Below_Threshold)
            {
                /* Condition that was there has been removed*/
                API_Ptr->bALDO5V_Below_Threshold = false;
                Notification.NID = ALDO5V_Above_Threshold;
                API_Ptr->fnStatusChange(API_Ptr->anAPI_Ref, Notification);
            }
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_CHARGE_PUMP_STATUS)
        {
            /*  Current Status of CHARGE_PUMP_ERR. */
        }
        if (API_Ptr->RegMap[RegAdr] &BITM_LIVE_STATUS_HI_TEMP_STATUS )
        {
            /*  Current Status of HI_TEMP_ERR */
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_VI_ERR_CURR_D)
        {
            /*  Current Status of VI_ERR_D */
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_VI_ERR_CURR_C)
        {
            /*  Current Status of VI_ERR_C */
        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_VI_ERR_CURR_B)
        {
            /*  Current Status of VI_ERR_B */

        }
        if (API_Ptr->RegMap[RegAdr] & BITM_LIVE_STATUS_VI_ERR_CURR_A)
        {
            /*  Current Status of VI_ERR_A */
        }

        API_Ptr->RegStatus[RegAdr]  &= ~REG_STATUS_UPDT;

        myReturn = ClearStatus.Result;
        if(myReturn == AD74412_RESULT_OK)
        {
            if (ClearStatus.BytesToTx)
                myReturn = SPIExchange(API_Ptr->anAPI_Ref, &ClearStatus);
        }
    }
    return myReturn;
}

/**
 *  \brief Determine if a channel function is being modified.
 *
 *  \param [in] anAPI_Ref The API definition reference for this API definition
 *  \param [in] Channel Channel identifier.
 *  \return true if a channel function is being changed, else false.
 *
 *  \details A write to the CH_FUNC_SETUPn register associated with the Channel
 *           indicates that were are intending to modify that channel.
 */
bool AD74412API_IsChannelFunctionModified(AD74412_API_Ptr API_Ptr, tAD74412ChannelID       Channel)
{
    bool             Modified = false;

    if (API_Ptr)
    {
        if(API_Ptr->RegStatus[REG_AD74412_CH_FUNC_SETUP0 + Channel] & REG_STATUS_DIRTY)
            Modified = true;
    }
    return Modified;
}

/**
 *  \brief Notification of a timer timeout expiration
 *
 *  \param [in] anAPI_Ref The API definition reference for this API definition
 *  \param [in] TID Transaction Identifier
 *  \return Return_Description
 *
 *  \details We are being notified that a time period that we requested has expired
 *           so we are perform the next step associated with that transaction id (if any)
 */
tAD74412Result AD74412API_TimerExpiryNotification(AD74412_API_REF anAPI_Ref, tAPIEvent TID)
{
    tAD74412Result  myReturn = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        bool                    aState = false;
        tSPITransactionInfo     FrameInfo;
        uint8_t                 FrameBytes[(NUM_AD74412_REGS + 1) * SPI_WR_FRAME_BYTES] = {0};
        uint8_t                 RegAdr;
        tAD74412ChannelID       Channel;

        switch (TID)
        {
        case TID_HouseKeeping:
            API_Ptr->RegStatus[REG_AD74412_ALERT_STATUS]  &= ~REG_STATUS_UPDT;
            API_Ptr->RegStatus[REG_AD74412_LIVE_STATUS]  &= ~REG_STATUS_UPDT;

        case TID_SoftwareReset:
            /* Read all the marked registers */
            myReturn = API_ReadModifiedRegisters(API_Ptr);
            break;
        case TID_CommitConfigurations_WaitBeforeChannelFunctionChange:
            /* We waited an addtional 130us after being requested to
               change the channel config to ensure that we are AT LEAST
               130 us in the existing mode before changing to a new one.

               Now we will start the transition to the new function by
               stopping the ADC conversions (if converting) and clearing
               the DACS of the changed channels.
               */
            SPI_InitExchange(&FrameInfo,
                             API_Ptr,
                             TID_CommitConfigurations_InitialPreparation,
                             (uint8_t *)FrameBytes,
                             sizeof(FrameBytes),
                             SPI_WR_FRAME_BYTES);

            /* Stop the ADC converting with a temporary value write.
               This value will be dirtied with the final value within
               TID_CommitConfigurations_Wait_In_HighImpedance
            */
            (void)SPI_AddWriteFrame32(API_Ptr,
                                      &FrameInfo,
                                      REG_AD74412_ADC_CONV_CTRL,
                                      REG_AD74412_ADC_CONV_CTRL_RESET);

            /* Zero the DACs in each CHANGING channel. These should not be
               altered in within
               TID_CommitConfigurations_Wait_In_HighImpedance */
            for (Channel = Channel_A; Channel <= Channel_D; Channel++)
            {
                if(AD74412API_IsChannelFunctionModified(API_Ptr, Channel))
                {
                    (void)SPI_AddWriteFrame32(API_Ptr,
                                              &FrameInfo,
                                              REG_AD74412_DAC_CODE0 + Channel,
                                              REG_AD74412_DAC_CLR_CODEn_RESET);
                    /* This overrides any PREVIOUS caller intent. User must call
                       AD74412API_SetOutput after configuring the function with the
                       new intent.
                    */
                    API_Ptr->RegMap[REG_AD74412_DAC_CODE0 + Channel] = REG_AD74412_DAC_CLR_CODEn_RESET;
                    API_Ptr->RegStatus[RegAdr] &= ~REG_STATUS_DIRTY;
                }
            }
            /* Write the command to load the DAC(s) with any changed DAC Codes above */
            (void)SPI_AddWriteFrame32(API_Ptr,
                                      &FrameInfo,
                                      REG_AD74412_CMD_KEY,
                                      ENUM_CMD_KEY_LDAC_KEY);

            /* Set the channel function to high impedence (i.e. reset state)
               This value will be dirtied with the final value within
               TID_CommitConfigurations_Wait_In_HighImpedance */
            for (Channel = Channel_A; Channel <= Channel_D; Channel++)
            {
                if(AD74412API_IsChannelFunctionModified(API_Ptr, Channel))
                    (void)SPI_AddWriteFrame32(API_Ptr,
                                              &FrameInfo,
                                              REG_AD74412_CH_FUNC_SETUP0 + Channel,
                                              REG_AD74412_CH_FUNC_SETUPn_RESET);
            }
            /* Queue these writes to the SPI transaction layer.
               We get a TID_CommitConfigurations_InitialPreparation
               notification in AD74412API_ReceivedSPIPacket
               once this is completed and can trigger the next step.
            */
            SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);
            /* Once we get notification that this SPI frames have been sent, we will wait
               a short time in this interim mode. Any new calls to the API will return BUSY while
               we are waiting on the event.
             */
            break;
        case TID_CommitConfigurations_Wait_In_HighImpedance:
            // Set the channel function to the desired function.
            SPI_InitExchange(&FrameInfo,
                             API_Ptr,
                             TID_CommitConfigurations_SetToDesiredState,
                             (uint8_t *)FrameBytes,
                             sizeof(FrameBytes),
                             SPI_WR_FRAME_BYTES);

            /* If any one of the channels are enabled, then enable the ADC to
               perform continuous conversions.
            */
            RegAdr = REG_AD74412_ADC_CONV_CTRL;
            if((API_Ptr->RegMap[RegAdr] & BITM_ADC_CONV_CTRL_CH_A_EN) ||
                    (API_Ptr->RegMap[RegAdr] & BITM_ADC_CONV_CTRL_CH_B_EN) ||
                    (API_Ptr->RegMap[RegAdr] & BITM_ADC_CONV_CTRL_CH_C_EN) ||
                    (API_Ptr->RegMap[RegAdr] & BITM_ADC_CONV_CTRL_CH_D_EN))
            {
                aState = true;
            }
            SET_FIELD_NS(ADC_CONV_CTRL,
                         CONV_SEQ,
                         API_Ptr->RegMap[RegAdr],
                         aState ? ENUM_ADC_CONV_CTRL_CONTINUOUS : ENUM_ADC_CONV_CTRL_IDLE);
            API_Ptr->RegStatus[RegAdr]  |= REG_STATUS_DIRTY;

            /* Any registers that are marked as dirty are now written in the order of
               their appearance in the register map. This suits our need.
            */
            for(RegAdr = REG_AD74412_NOP; RegAdr <= REG_AD74412_SILICON_REV ; RegAdr++)
            {
                if(API_Ptr->RegStatus[RegAdr] & REG_STATUS_DIRTY)
                {
                    (void)SPI_AddWriteFrame32(API_Ptr,
                                              &FrameInfo,
                                              RegAdr,
                                              API_Ptr->RegMap[RegAdr]);
                    API_Ptr->RegStatus[RegAdr] &= ~REG_STATUS_DIRTY;
                }
            }

            /* Cache the committed configuration registers*/
            API_Ptr->CommittedRegChFuncSetup[Channel_A] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP0];
            API_Ptr->CommittedRegChFuncSetup[Channel_B] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP1];
            API_Ptr->CommittedRegChFuncSetup[Channel_C] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP2];
            API_Ptr->CommittedRegChFuncSetup[Channel_D] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP3];

            API_Ptr->CommittedRegAdcConfig[Channel_A] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0];
            API_Ptr->CommittedRegAdcConfig[Channel_B] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG1];
            API_Ptr->CommittedRegAdcConfig[Channel_C] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG2];
            API_Ptr->CommittedRegAdcConfig[Channel_D] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG3];


            /* Queue these writes to the SPI transaction layer.
               We get a TID_CommitConfigurations_SetToDesiredState
               notification in AD74412API_ReceivedSPIPacket
               once this is completed and can trigger the next step.
            */
            SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);

            /* Now when we know that the SPI frame has been sent, we will wait
              a short time in this interim mode before updating the LDACs. Any new calls to the API will return BUSY while
              we are waiting on the event.
            */

            break;
        case TID_CommitConfigurations_Wait_UpdateDAC:
            /* Apply any deferred AD74412API_SetOutput DAC loading */
            break;

        default:
            break;
        }
    }
    return myReturn;
}

/**
 *  \brief Brief
 *
 *  \param [in] anAPI_Ref Parameter_Description
 *  \param [in] pExChng Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result AD74412API_ReceivedSPIPacket(AD74412_API_REF anAPI_Ref, tSPITransactionInfo *pExChng)
{
    tAD74412Result myReturn = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        // Valid ptr.
        int               ByteReadIndex = 0;

        API_Ptr->bREAD_SELECTStatusResults = false;

        switch (pExChng->TID)
        {
        case TID_ConfigureChannels:
            /* Cache the committed configuration registers*/
            API_Ptr->CommittedRegChFuncSetup[Channel_A] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP0];
            API_Ptr->CommittedRegChFuncSetup[Channel_B] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP1];
            API_Ptr->CommittedRegChFuncSetup[Channel_C] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP2];
            API_Ptr->CommittedRegChFuncSetup[Channel_D] = API_Ptr->RegMap[REG_AD74412_CH_FUNC_SETUP3];

            API_Ptr->CommittedRegAdcConfig[Channel_A] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG0];
            API_Ptr->CommittedRegAdcConfig[Channel_B] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG1];
            API_Ptr->CommittedRegAdcConfig[Channel_C] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG2];
            API_Ptr->CommittedRegAdcConfig[Channel_D] = API_Ptr->RegMap[REG_AD74412_ADC_CONFIG3];
            break;
        case TID_SoftwareReset:
            /* This is the first SPI command sent
               We need to delay a little bit after the reset
            */
            udelay(50000);
            myReturn = AD74412API_TimerExpiryNotification(anAPI_Ref, pExChng->TID);
            break;
        case TID_BurstReadInit:
            API_Ptr->bREAD_SELECTStatusResults = true;
            break;
        case TID_BurstReadResults:
        {
            API_Ptr->bREAD_SELECTStatusResults = true;
            /* We have read the results block and the status registers all in one block.
               This might have been trigger for a number of reasons

               ALERT pin assertion
               ADC_RDY assertion.
               etc.
            */
            ByteReadIndex = 0;
            for(uint8_t ReadAdr = REG_AD74412_ADC_RESULT0; ReadAdr <= REG_AD74412_LIVE_STATUS ; ReadAdr++)
            {
                // First one is like any other read
                if (ReadAdr == REG_AD74412_ADC_RESULT0)
                {
                    // Validate each packet's CRC and then transfer to memory.
                    if (BA_Validate(pExChng->pData + ByteReadIndex, ReadAdr))
                    {
                        API_Ptr->RegMap[ReadAdr]      = BA_GET_DATA((pExChng->pData + ByteReadIndex));
                        API_Ptr->RegStatus[ReadAdr]  |= REG_STATUS_UPDT;
                    }
                    ByteReadIndex += SPI_WR_FRAME_BYTES;
                }
                else
                {
                    // Validate each packet's CRC and then transfer to memory.
                    // We don't have the first byte here (address) of the burst read
                    // but they follow sequentially.
                    if (BA_CRC16(pExChng->pData + ByteReadIndex))
                    {
                        API_Ptr->RegMap[ReadAdr]      = BA_GET_DATA((pExChng->pData + ByteReadIndex - 1));
                        API_Ptr->RegStatus[ReadAdr]  |= REG_STATUS_UPDT;
                    }
                    ByteReadIndex += SPI_WR_FRAME_BYTES - 1;
                }
            }
            myReturn = AnalyseChipSituation(anAPI_Ref);
        }
        break;
        case TID_ReadModified:
        {
            /* This is a request to read all registers that are currently marked
               as unknown */
            ByteReadIndex = SPI_READ_FRAMES_DELAY * SPI_WR_FRAME_BYTES;
            for(uint8_t ReadAdr = REG_AD74412_NOP; ReadAdr <= REG_AD74412_SILICON_REV ; ReadAdr++)
            {
                if((API_Ptr->RegStatus[ReadAdr] & REG_STATUS_UPDT) == 0x0)
                {
                    // Validate each packet's CRC and then transfer to memory.
                    if (BA_Validate(pExChng->pData + ByteReadIndex, ReadAdr))
                    {
                        if((API_Ptr->RegStatus[ReadAdr]  & REG_STATUS_DIRTY) == 0x0)
                        {
                            API_Ptr->RegMap[ReadAdr]      = BA_GET_DATA((pExChng->pData + ByteReadIndex));
                            API_Ptr->RegStatus[ReadAdr]  |= REG_STATUS_UPDT;
                        }
                    }
                    ByteReadIndex += SPI_WR_FRAME_BYTES;
                }
            }
            myReturn = AnalyseChipSituation(anAPI_Ref);
        }

        break;
        case TID_CommitConfigurations_InitialPreparation:
            // The interim HIGH_IMPEDANCE channel function(s) have been applied by this SPI transaction.
            // Now we should wait 130 us in this state before migrating to the final state.
            udelay(CHANNEL_MINIMUM_TIME_us);
            /* Now that we know that the SPI frame has been sent, we will wait
              a short time in this interim mode. Any new calls to the API will return BUSY while
              we are waiting on the event.
            */
            myReturn = AD74412API_TimerExpiryNotification(anAPI_Ref,
                       TID_CommitConfigurations_Wait_In_HighImpedance);
            break;
        case TID_CommitConfigurations_SetToDesiredState:
            // The updated channel function(s) have been applied by this SPI transaction.
            // Now we should wait another 150 us before allowing the DAC codes to be updated.
            udelay(150);
            myReturn = AD74412API_TimerExpiryNotification(anAPI_Ref,
                       TID_CommitConfigurations_Wait_UpdateDAC);
            break;
        default:
            break;
        }
    }

    return myReturn;
}

/**
 *  \brief Initiate the transfer of the changed channel configurations
 *
 *  \param [in] anAPI_Ref API Pointer reference
 *  \return tAD74412Result
 *
 *  \details The switch to a new channel function needs to follow the
 *           recommended datasheet sequence.
 */
tAD74412Result AD74412API_CommitConfigurations(AD74412_API_REF anAPI_Ref)
{
    tAD74412Result  Result    = AD74412_RESULT_OK;
    AD74412_API_Ptr API_Ptr   = getPtrFromRef(anAPI_Ref);

    if (API_Ptr)
    {
        /* Datasheet requires us to be in an existing mode for
           a certain amount of time before moving to another.*/
        udelay(CHANNEL_MINIMUM_TIME_us);
        /* Once we get notification of this timer expiration, we will start the
           transition to the new mode. Any new calls to the API will return BUSY while
           we are waiting on the event.
           */
        Result = AD74412API_TimerExpiryNotification(anAPI_Ref,
                 TID_CommitConfigurations_WaitBeforeChannelFunctionChange);
    }
    else
        Result = AD74412_RESULT_UNKNOWN_CHIP;

    return Result;
}


/**
 *  \brief Apply and update DAC values to the device
 *
 *  \param [in] AD74412_API_REF API Pointer reference
 *  \return tAD74412Result
 *
 *  \details Details
 */
static tAD74412Result AD74412API_ApplyOutputs(AD74412_API_Ptr API_Ptr)
{
    tAD74412Result          myReturn    = AD74412_RESULT_OK;
    tSPITransactionInfo     FrameInfo;
    uint8_t                 FrameBytes[SPI_WR_FRAME_BYTES * 5];
    bool_t                  ValuesToWrite = false;

    SPI_InitExchange(&FrameInfo, API_Ptr, TID_SetOutput, FrameBytes, sizeof(FrameBytes), SPI_WR_FRAME_BYTES);

    for(uint8_t RegDacCodeAdr = REG_AD74412_DAC_CODE0; RegDacCodeAdr <= REG_AD74412_DAC_CODE3 ; RegDacCodeAdr++)
    {
        if(API_Ptr->RegStatus[RegDacCodeAdr] & REG_STATUS_DIRTY)
        {
            ValuesToWrite = true;
            API_Ptr->RegStatus[RegDacCodeAdr] &= ~REG_STATUS_DIRTY;
            SPI_AddWriteFrame32(API_Ptr, &FrameInfo, RegDacCodeAdr, API_Ptr->RegMap[RegDacCodeAdr]);
        }
    }
    myReturn = FrameInfo.Result;
    if(ValuesToWrite && (myReturn == AD74412_RESULT_OK))
    {
        /* One or more values are available to be written.*/
        API_Ptr->RegMap[REG_AD74412_CMD_KEY] = ENUM_CMD_KEY_LDAC_KEY;
        SPI_AddWriteFrame32(API_Ptr, &FrameInfo, REG_AD74412_CMD_KEY, API_Ptr->RegMap[REG_AD74412_CMD_KEY]);
        myReturn = FrameInfo.Result;
        if(myReturn == AD74412_RESULT_OK)
            myReturn = SPIExchange(API_Ptr->anAPI_Ref, &FrameInfo);
    }

    return myReturn;
}

/**
 *  \brief Brief
 *
 *  \param [in] AD74412_API_REF Parameter_Description
 *  \param [in] tChannelValue Parameter_Description
 *  \return Return_Description
 *
 *  \details Details
 */
tAD74412Result AD74412API_SetOutput(AD74412_API_REF anAPI_Ref, tChannelValue Value, tSetOutputAction Action)
{
    tAD74412Result          myReturn    = AD74412_RESULT_OK;
    AD74412_API_Ptr         API_Ptr = getPtrFromRef(anAPI_Ref);
    uint8_t                 RegDacCodeAdr;

    if(API_Ptr)
    {
        RegDacCodeAdr = REG_AD74412_DAC_CODE0 + Value.ChannelID;
        if((Action == QueueValue) || (Action == WriteValue))
        {
            uint8_t                 RegChnlFuncAdr;
            float                   MaxValue;
            float                   MinValue;

            RegChnlFuncAdr = REG_AD74412_CH_FUNC_SETUP0 + Value.ChannelID;
            switch ((API_Ptr->RegMap[RegChnlFuncAdr] & BITM_CH_FUNC_SETUP_CH_FUNC))
            {
            case VOLTAGE_OUTPUT:
                MinValue = VOLTAGE_OUTPUT_MIN;
                MaxValue = VOLTAGE_OUTPUT_MAX;
                break;
            case CURRENT_OUTPUT:
                MinValue = CURRENT_OUTPUT_MIN;
                MaxValue = CURRENT_OUTPUT_MAX;
                break;
            default:
                break;
            }

            if((Value.ChannelValue.aValue >= MinValue) && (Value.ChannelValue.aValue <= MaxValue))
            {
                uint16_t                NewDACValue;
                NewDACValue = (uint16_t)((Value.ChannelValue.aValue / (MaxValue - MinValue)) * BITM_DAC_CODE_DAC_CODE);
                if ((API_Ptr->RegMap[RegDacCodeAdr] & BITM_DAC_CODE_DAC_CODE) != NewDACValue)
                {
                    API_Ptr->RegMap[RegDacCodeAdr] = NewDACValue;
                    API_Ptr->RegStatus[RegDacCodeAdr]  |= REG_STATUS_DIRTY;
                }
            }
            else
            {
                myReturn    = AD74412_RESULT_WRONG_CHANNEL_ACTION_FOR_TYPE;
            }
        }

        if((myReturn == AD74412_RESULT_OK) && (Action != QueueValue))
        {
            myReturn = AD74412API_ApplyOutputs(API_Ptr);
        }
    }
    else
        myReturn = AD74412_RESULT_UNKNOWN_CHIP;


    return myReturn;
}

/*!
 * @brief Calculate the CRC of a 4 bytes frame, and either update it or check it.

               To verify that data has been received
               correctly in noisy environ-ments, the
               AD74412R has a CRC implemented in the
               SPI interface.  This CRC is based on an
               8 bit cyclic redundancy check (CRC-8).
               The device controlling the AD74412R
               generates an 8-bit frame check sequence
               using the following polynomial:
               C(x) = x8 + x2 + x1 + 1
               This sequence is added to the end of
               the data-word, and 32 bits are sent
               to the AD74412R before taking SYNC high.

               E.g. 0x41002E has a CRC of 0x27

               E.g. 0xAEA020 has a CRC of 0x9C
   @param[in]   pFrameBA : Array of bytes to check/update.
   @param[in]   bUpdate  : True if to be updated, False if just to be checked.

  @return     False, if CRC is not valid after call, otherwise True
 */
static bool_t BA_CRC(uint8_t *pFrameBA, bool_t bUpdate)
{
    uint32_t CRCcode = 0x107;
    uint32_t i = 31;
    uint32_t DataOUT = (((uint32_t)pFrameBA[0]) << 24) |
                       (((uint32_t)pFrameBA[1]) << 16) | (((uint32_t)pFrameBA[2]) << 8);

    while (i >= 8)
    {
        if ((DataOUT & (1 << i)) != 0)
        {
            DataOUT ^= (CRCcode << (i - 8));
        }
        i--;
    }

    if(bUpdate)
    {
        pFrameBA[3] = (uint8_t)DataOUT;
        return true;
    }
    else
    {
        if(pFrameBA[3] == (uint8_t)DataOUT)
            return true;
        else
            return false;
    }
}

static bool_t BA_CRC16(uint8_t *pFrameBA)
{
    uint32_t CRCcode = 0x107;
    uint32_t i = 31;
    uint32_t DataOUT = (((uint32_t)pFrameBA[0]) << 16) |
                       (((uint32_t)pFrameBA[1]) << 8);

    while (i >= 8)
    {
        if ((DataOUT & (1 << i)) != 0)
        {
            DataOUT ^= (CRCcode << (i - 8));
        }
        i--;
    }

    if(pFrameBA[2] == (uint8_t)DataOUT)
        return true;
    else
        return false;

}


/**
 *  \brief Check that the response word via that we received is valid
 *
 *  \param [in] nAdr         The address of the register than is expected here
 *  \param [in] pFrameBA     The 4 byte array as received from the device
 *
 *  \return Return_Description
 *
 *  \details Details
 */
static bool_t BA_Validate(uint8_t *pFrameBA, uint8_t nAdr)
{
    bool_t bValid = true;

    // SPI_RD_RET_INFO = 0 !!
    // D31        1
    if ((pFrameBA[0] & 0x80) == 0x00)
        bValid = false;
    // D30:D24    READBACK_ADDR[6:0]
    if (BA_GET_ADDR(pFrameBA) != (nAdr & 0x7F))
        bValid = false;

    // D23:D8     READ DATA
    //Result = (ResponseWord >> 8) & 0xFFFF;
    // D0:D7      CRC

    if(!BA_CRC(pFrameBA, false))
        bValid = false;

    return bValid;
}

/*!
 * @brief       Construct an array of bytes which are in the correct order for
                an SPI transaction representing a regster write
 *
   @param[in]   nAdr : Specifies the address of the register to write to.
   @param[in]   nData: Specifies the data to write to the register.
   @param[out]  pBytesTxOrdered:
  *
   @return
 */
static void BA_Create(uint8_t *pFrameBA, uint8_t nAdr, uint16_t nData)
{
    // D31    : Reserved (will use 0)
    // D30:D24: Address.
    pFrameBA[0] = nAdr;
    // D23:D8 : Data (Big endian)
    pFrameBA[1] = (nData >> 8) & 0xFF;
    pFrameBA[2] = (nData >> 0) & 0xFF;
    // D7 :D0 : CRC

    /* Update the CRC of the Frame */
    (void)BA_CRC(pFrameBA, true);
}