/**
 * \file mykonos_dbgjesd.c
 *
 * \brief Contains Mykonos APIs for Jesd debug facilities.
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#include <stdint.h>
#include <stddef.h>

#include "mykonos_dbgjesd.h"
#include "mykonos_macros.h"


/**
 * \brief Helper function for return of character string based on 32-bit mykonosDbgErr_t enum value
 *
 * To save codespace, these error strings are ifdef'd out unless the user
 * adds a define MYKONOS_VERBOSE to their workspace.  This function can be
 * useful for debug.  Each function also returns unique error codes to
 * make it easier to determine where the code broke.
 *
 * \param errorCode is enumerated error code value
 *
 * \return Returns character string based on enumerated value
 */
const char* getDbgJesdMykonosErrorMessage(mykonosDbgErr_t errorCode)
{

#if MYKONOS_VERBOSE == 0
    return "";
#else

    switch (errorCode)
    {
        case MYKONOS_ERR_DBG_OK:
            return "No error\n";

        case MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER:
            return "Pointer to passed parameter is NULL\n";

        case MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE:
            return "Illegal JESD core passed, valid options for the JESD code are in mykonos_jesdcore_t\n";

        case MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE:
            return "Illegal error type passed, valid error types are given in mykonosErrType_t\n";

        case MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED:
            return "Illegal errType passed, valid errType are BADDISP, NIT and UEKC\n";

        case MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER:
            return "Illegal lane number passed, valid lanes are given in mykonosLaneSel_t\n";

        case MYKONOS_ERR_DBG_ERROR_THRESHOLD:
            return "Illegal error threshold passed, valid error thresholds are in the range 0x00 to 0xFF\n";

        case MYKONOS_ERR_DBG_ERROR_SYNC_MASK:
            return "Illegal Sync Mask passed, valid Sync Masks are in the range 0x00 to 0x07\n";

        case MYKONOS_ERR_DBG_ERROR_IRQ_MASK:
            return "Illegal IRQ mask passed, valid IRQ Masks are in the range 0x00 to 0x1FF\n";

        case MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN:
            return "Function parameter pattern is a NULL pointer\n";

        case MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE:
            return "Function parameter enable is a NULL pointer\n";

        case MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE:
            return "Function parameter toggle is a NULL pointer\n";

        case MYKONOS_ERR_DBG_ILLEGAL_TOGGLE:
            return "Toggle is not valid, valid values are 0 and 1.\n";

        case MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN:
            return "Pattern passed is outside the range valid range from 0x00000 to 0xFFFFF\n";

        case MYKONOS_ERR_DBG_ILLEGAL_ENABLE:
            return "Enable not valid, valid values are 0 and 1.\n";

        case MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE:
            return "the lane selection is out of range, valid range is 0x00 to 0x0F in MYKONOS_framerSetZeroData().\n";

        case MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL:
            return "Function parameter lane is a NULL pointer passed to MYKONOS_framerGetZeroData().\n";

        case MYKONOS_ERR_DBG_FRAMER_SEL_BASE_ADD_NULL:
            return "base address is null in MYKONOS_framerCoreSel().\n";

        case MYKONOS_ERR_DBG_FRAMER_ILLEGAL_JESD_CORE:
            return "Only valid JESD cores are FRAMER and OBS_FRAMER in MYKONOS_framerCoreSel().\n";

        default:
            return "";
    }

#endif
}

/**
 * \brief Performs indirect write access to internal JESD register.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore JESD core selection
 * \param intAddr Internal address to access in JESD core
 * \param data Data to write into internal address
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE An illegal JESD core passed, valid cores are given in mykonos_jesdcore_t
 */
mykonosDbgErr_t MYKONOS_jesdIndWrReg(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint8_t intAddr, uint8_t data)
{
    uint16_t baseAddr = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdIndWrReg()\n");
#endif

    /* Check for jesdCore and address assignment */
    switch (jesdCore)
    {
        case MYK_FRAMER:
            baseAddr = MYKONOS_ADDR_FRAMER_ADDR;
        break;

        case MYK_DEFRAMER:
            baseAddr = MYKONOS_ADDR_DEFRAMER_ADDR;
        break;

        case MYK_OBS_FRAMER:
            baseAddr = MYKONOS_ADDR_OBS_FRAMER_ADDR;
        break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE));
             return MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE;
    }

    /* Perform indirect write */
    CMB_SPIWriteByte(device->spiSettings, baseAddr, intAddr);
    CMB_SPIWriteByte(device->spiSettings, baseAddr + 1, data);
    CMB_SPIWriteByte(device->spiSettings, baseAddr + 2, 0x01);

    return MYKONOS_ERR_DBG_OK;
}

/**
 * \brief Performs indirect read access to internal JESD register.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore JESD core selection
 * \param intAddr Internal address to access in JESD core
 * \param data Pointer to store data
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed is a NULL pointer
 */
mykonosDbgErr_t MYKONOS_jesdIndRdReg(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint8_t intAddr, uint8_t *data)
{
    uint16_t baseAddr = 0;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdIndRdReg()\n");
#endif

    /* Check for valid data pointer */
    if(data == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
         return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Check for jesdCore and address assignment */
    switch (jesdCore)
    {
        case MYK_FRAMER:
            baseAddr = MYKONOS_ADDR_FRAMER_ADDR;
        break;

        case MYK_DEFRAMER:
            baseAddr = MYKONOS_ADDR_DEFRAMER_ADDR;
        break;

        case MYK_OBS_FRAMER:
            baseAddr = MYKONOS_ADDR_OBS_FRAMER_ADDR;
        break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE));
             return MYKONOS_ERR_DBG_ILLEGAL_JESD_CORE;
    }

    /* Perform indirect read */
    CMB_SPIWriteByte(device->spiSettings, baseAddr, intAddr);
    CMB_SPIReadByte(device->spiSettings, baseAddr + 1, data);

    return MYKONOS_ERR_DBG_OK;
}

/**
 * \brief Performs reads to JESD Deframer secondary status register.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param defStat2 Pointer to store the deframer secondary status
 *
 *  Bit mask  |  Description of defStat2
 * -----------|---------------
 *      2     | DeframerSYSREF Phase Error, when set, this bit reports that an incoming SYSREF is not aligned to the current LMFC boundary.
 *      1     | Deframer Det Lat FIFO RD, this bit is the status of rd_enable for deterministic sample FIFO. If set deterministic latency is achieved.
 *      0     | Deframer Lane FIFO PTR Delta Changed. If set this bit reports that the offset between the read and write FIFO pointers has changed.
 *
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed defStat2 is NULL
 */
mykonosDbgErr_t MYKONOS_deframerRd2Stat(mykonosDevice_t *device, uint8_t *defStat2)
{
    uint8_t regRd = 0x00;

    const uint8_t SHIFT_MASK = 0x05;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdRdDefrmSndStat()\n");
#endif

    /* Check for valid defStat2 pointer */
    if (defStat2 == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
        return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Read deframer secondary status */
    CMB_SPIReadByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STATUS_2, &regRd);

    *defStat2 = regRd >> SHIFT_MASK;

    /* Refresh status registers */
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x01);
    CMB_SPIWriteByte(device->spiSettings, MYKONOS_ADDR_DEFRAMER_STAT_STRB, 0x00);

    return MYKONOS_ERR_DBG_OK;
}

/**
 * \brief Performs read back lanes in error for the given errType of the JESD Deframer.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errType which error counter to read back
 * \param lane Pointer to store which lane has reached the error threshold.
 * Every bit shows per lane which error count has reached the threshold count.
 * A value of 0x0F will represent that all the lanes has reached the error threshold.
 *
 *    Value      |  Description of Lane
 * --------------|-----------------------
 *      8        | Lane 3 has reached the error threshold
 *      4        | Lane 2 has reached the error threshold
 *      2        | Lane 1 has reached the error threshold
 *      1        | Lane 0 has reached the error threshold
 *      0        | No errors in lanes
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed lane is NULL
 * \retval MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED Illegal errType passed, valid errType are BADDISP, NIT and UEKC
 */
mykonosDbgErr_t MYKONOS_deframerGetErrLane(mykonosDevice_t *device, mykonosErrType_t errType, uint8_t *lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdIndRdReg()\n");
#endif

    /* Check for valid Lane pointer */
    if(lane == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
         return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Check for valid error type */
    switch (errType)
    {
        case MYK_BADDISP:
        case MYK_NIT:
        case MYK_UEKC:
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED));
            return MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED;
    }

    /* Read error and store it in lane */
    error = MYKONOS_jesdIndRdReg(device, jesdCore, (uint8_t)errType, lane);

    return error;
}

/**
 * \brief Performs reset of the selected error type for the selected lane.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errType Which error counter to be reset
 * \param lane Which lane error counter to be reset
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER Illegal lane passed, valid lanes are given in mykonosLaneSel_t
 * \retval MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED Illegal errType passed, valid errType are BADDISP, NIT and UEKC
 */
mykonosDbgErr_t MYKONOS_deframerRstErrCntr(mykonosDevice_t *device, mykonosErrType_t errType, mykonosLaneSel_t lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;

    const uint8_t rstCntBit = 0x20;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdIndRdReg()\n");
#endif

    /* Check for valid Lane */
    switch (lane)
    {
        case MYK_LANE_0:
        case MYK_LANE_1:
        case MYK_LANE_2:
        case MYK_LANE_3:
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER));
            return MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER;
    }

    /* Check for error type */
    switch (errType)
    {
        case MYK_BADDISP:
        case MYK_NIT:
        case MYK_UEKC:
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED));
            return MYKONOS_ERR_DBG_ILLEGAL_ERROR_SELECTED;
    }

    /* Perform reset counter */
    error = MYKONOS_jesdIndWrReg(device, jesdCore, (uint8_t)errType, (rstCntBit | ((uint8_t)lane)));

    return error;
}

/**
 * \brief Performs reset of the selected error type IRQ for the given lane.
 *
 *<B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errType Which error counter to be reset, for the full list
 * \param lane Which lane error counter to be reset
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER Illegal lane passed, valid lanes are given in mykonosLaneSel_t
 * \retval MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE Illegal errType passed, valid errType are given in mykonosErrType_t
 */
mykonosDbgErr_t MYKONOS_deframerRstErrIrq(mykonosDevice_t *device, mykonosErrType_t errType, mykonosLaneSel_t lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0;
    uint8_t rstIrqBit = 0x80;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdIndRdReg()\n");
#endif

    /* Check for valid Lane */
    switch (lane)
    {
        case MYK_LANE_0:
        case MYK_LANE_1:
        case MYK_LANE_2:
        case MYK_LANE_3:
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER));
            return MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER;
    }

    /* Check for error type */
    switch (errType)
    {
        case MYK_CMM:
            /* CMM has no lane selection */
            if(MYKONOS_ERR_DBG_OK != (error = MYKONOS_jesdIndRdReg(device, jesdCore, (uint8_t)errType, &regRd)))
            {
                return error;
            }

            lane = MYK_LANE_0;
            rstIrqBit = 0x10 | regRd;
            break;

        case MYK_BADDISP:
        case MYK_NIT:
        case MYK_UEKC:
            break;

        default:
           CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE,
                   getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE));
           return MYKONOS_ERR_DBG_ILLEGAL_ERROR_TYPE;
    }

    /* Reset selected IRQ */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, (uint8_t)errType, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, (uint8_t)errType, (rstIrqBit | ((uint8_t)lane)))) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, (uint8_t)errType, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}


/**
 * \brief Performs read back of the error counters for selected lane of the JESD Deframer.
 *
 * This function reads the counters per the given lane for the bad disparity, not in table and unexpected K character errors and stores them in the
 * structure laneErr.
 * The structure laneErr must be initialised in the calling function as it will contain the values of the counters per error type for the given lane.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - laneErr structure must be initialised
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param laneSel which JESD lane error counter to be read
 * \param laneErr Pointer to a mykonosLaneErr_t structure type to store counter values for different errors
 *
 * \retval MYKONOS_ERR_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed laneErr is NULL
 * \retval MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER Illegal laneSel parameter passed, valid laneSel are given in mykonosLaneSel_t
 */
mykonosDbgErr_t MYKONOS_deframerRdErrCntr(mykonosDevice_t *device, mykonosLaneSel_t laneSel, mykonosLaneErr_t *laneErr)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t counterSel = 0;

    const uint8_t ERR_CNT_ADDR = 0x6B;

#if (MYKONOS_VERBOSE == 1)
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdRdErrCntr()\n");
#endif

    /* Check for valid laneErr pointer */
    if(laneErr == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
        return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Check for valid laneSel */
    switch (laneSel)
    {
        case MYK_LANE_0:
        case MYK_LANE_1:
        case MYK_LANE_2:
        case MYK_LANE_3:
            break;

        default:
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER));
             return MYKONOS_ERR_DBG_ILLEGAL_LANE_NUMBER;
    }

    /* Reading Bad disparity Counter */
    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, ERR_CNT_ADDR, (((uint8_t)(laneSel) << 4) | counterSel++))) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, ERR_CNT_ADDR, &laneErr->badDispCntr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    /* Reading Not in table Counter */
    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, ERR_CNT_ADDR, (((uint8_t)(laneSel) << 4) | counterSel++))) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, ERR_CNT_ADDR, &laneErr->nitCntr))!= MYKONOS_ERR_DBG_OK)
    {
        return error;
    }

    /* Reading unexpected K character Counter */
    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, ERR_CNT_ADDR, (((uint8_t)(laneSel) << 4) | counterSel))) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, ERR_CNT_ADDR, &laneErr->uekcCntr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}


/**
 * \brief Performs set JESD Deframer error thresholds.
 *
 * The value for the error threshold will be applied to all the error counters for all the enabled lanes.
 * Bad disparity, NIT disparity and Unexpected K char. Errors are counted and compared to the errThrs value.
 * When the count is equal, then the user has the ability of raising an IRQ, see MYKONOS_deframerSetIrqMask,
 * or asserted the SYNC signal per the mask register settings, see MYKONOS_deframerSetSyncMask,
 * or do nothing upon threshold reach.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errThrs is the value that will be written to the error threshold, valid error threshold is in the range 0x00 to 0xFF.
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_deframerSetErrThrs(mykonosDevice_t *device, uint8_t errThrs)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;

    const uint8_t DFM_ERROR_THRES = 0x7C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdGetErrThrs()\n");
#endif

    /* Writing internal registers 0x7C */
    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, DFM_ERROR_THRES, errThrs)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}

/**
 * \brief Performs read JESD Deframer error thresholds.
 *
 * The value for the error threshold is common to all the error counters.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 * - laneErr structure must be initialised
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errThrs Pointer to store the error threshold value read back
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed errThrs is NULL
 */
mykonosDbgErr_t MYKONOS_deframerGetErrThrs(mykonosDevice_t *device, uint8_t *errThrs)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;

    const uint8_t DFM_ERROR_THRES = 0x7C;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdGetErrThrs()\n");
#endif

    /* Check for null parameter */
    if (errThrs == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
        return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Reading internal registers */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, DFM_ERROR_THRES, errThrs)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}


/**
 * \brief Performs set JESD Deframer Sync mask.
 *
 * This function sets the syncMask that will be programmed in to Mykonos.
 * When the error counter reaches the error threshold value programmed by MYKONOS_deframerSetErrThrs function, if the syncMask is set for the particular
 * errors type then the SYNC~ is asserted.
 * The syncMask is applied to all the enabled lanes.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errSyncMask is the mask that will be used in order to assert SYNC~
 * Setting the syncMask bits causes the deframer to assert the SYNC~ when the count of the corresponding error reaches the
 * errThrs value.
 *
 *   Sync       |   Bit mask   |  Description of sync
 * -------------|--------------|-----------------------
 * SYNC_BADDISP |     2        | Bad disparity mask
 * SYNC_NIT     |     1        | Not in table
 * SYNC_UEKC    |     0        | Unexpected K character
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully.
 * \retval MYKONOS_ERR_DBG_ERROR_SYNC_MASK Passed errSyncMask parameter is outside its boundaries, valid sync mask are in the range 0x00 to 0x07.
 */
mykonosDbgErr_t MYKONOS_deframerSetSyncMask(mykonosDevice_t *device, uint8_t errSyncMask)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0x00;
    uint8_t regWr = 0x00;

    const uint8_t SYNC_MASK_CTRL = 0x7B;
    const uint8_t SYNC_MASK = 0x07;
    const uint8_t MASK = 0xE0;
    const uint8_t SYNC_SHIFT = 5;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdSetSyncMask()\n");
#endif

    /* Check for valid errSyncMask */
    if (errSyncMask & ~SYNC_MASK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ERROR_SYNC_MASK,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ERROR_SYNC_MASK));
        return MYKONOS_ERR_DBG_ERROR_SYNC_MASK;
    }

    /* Writing internal registers do a read modify write */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, SYNC_MASK_CTRL, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }
    regRd &= ~(MASK);

    regWr = (uint8_t)(regRd | (errSyncMask << SYNC_SHIFT));

    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, SYNC_MASK_CTRL, regWr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}

/**
 * \brief Performs get JESD Deframer SyncMask.
 *
 * This function read the SyncMask that is programmed into Mykonos.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errSyncMask Pointer to store the sync mask currently being used:
 *
 *   Sync       |   Bit mask   |  Description of errSyncMask
 *--------------|--------------|-----------------------
 * SYNC_BADDISP |     2        | Bad disparity mask
 * SYNC_NIT     |     1        | Not in table
 * SYNC_UEKC    |     0        | Unexpected K character
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed errSyncMask is NULL
 */
mykonosDbgErr_t MYKONOS_deframerGetSyncMask(mykonosDevice_t *device, uint8_t *errSyncMask)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0x00;

    const uint8_t SYNC_MASK_CTRL = 0x7B;
    const uint8_t SYNC_MASK = 0x07;
    const uint8_t SYNC_SHIFT = 5;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdGetSyncMask()\n");
#endif

    /* Check for valid errSyncMask pointer */
    if (errSyncMask == NULL)
    {
       CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
               getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
       return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /*read sync mask internal reg */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, SYNC_MASK_CTRL, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    *errSyncMask = (regRd >> SYNC_SHIFT) & SYNC_MASK;

    return error;
}


/**
 * \brief Performs read of enabled JESD Deframer lanes.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param lane Pointer to store the current enabled lanes
 *
 *  Value        |  Description of Lane
 * --------------|-----------------------
 *      8        | Lane 3 is enabled
 *      4        | Lane 2 is enabled
 *      2        | Lane 1 is enabled
 *      1        | Lane 0 is enabled
 *      0        | No lanes are enabled
 *
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed lane is NULL
 */
mykonosDbgErr_t MYKONOS_deframerGetEnLanes(mykonosDevice_t *device, uint8_t *lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0x00;

    const uint8_t DFM_LANE_EN = 0x7D;
    const uint8_t LANE_MASK = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdGetEnLanes()\n");
#endif

    /* Check for valid lane pointer */
    if (lane == NULL)
    {
       CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
               getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
       return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Read enable lanes internal reg */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, DFM_LANE_EN, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    *lane = regRd & LANE_MASK;

    return error;
}


/**
 * \brief Performs Deframer Force SYNC requests
 *
 * Using this feature, the user can force SYNC request on the Deframer
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param syncReq sync Request parameter
 * Setting the syncReq causes the deframer to assert the SYNC~.
 * Clearing syncReq causes the deframer to de-assert the SYNC~.
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_deframerForceSyncReq(mykonosDevice_t *device, uint8_t syncReq)
{
    uint8_t syncReqWr = 0x00;

    const uint8_t SYNC_REQ_MASK = 0x01;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_deframerForceSyncReq()\n");
#endif

    syncReqWr = (syncReq > 0) ? 0x01 : 0x00;

    CMB_SPIWriteField(device->spiSettings, MYKONOS_ADDR_DEFRAMER_TEST, syncReqWr, SYNC_REQ_MASK, 0);

    return MYKONOS_ERR_DBG_OK;
}

/**
 * \brief Performs set JESD Deframer IRQ mask that will be used to generates interrupts
 *
 * Bad disparity, NIT disparity and Unexpected K char. Errors are counted and compared to the errThrs value.
 * When the count is equal, either an IRQ is generated if the mask for the particular error is set.
 * Configuration mismatch flag does not have a counter.
 * Function is performed in all lanes.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errIrqMsk the value of the mask applied to the IRQ generation, possible combinations are:
 *
 *   IRQ       |   Bit mask   |  Description of errIrq
 * ------------|--------------|-----------------------
 *   CMM       |     3        | Configuration mismatch flag 0 if is CMM ok, 1 if CMM is detected
 *   BADDISP   |     2        | Incorrect disparity flag 0 if is BADDISP ok, 1 if BADDISP is detected
 *   NIT       |     1        | Not in table flag  0 if is NIT ok, 1 if NIT character is detected
 *   UEKC      |     0        | Unexpected K character flag 0 if is UEKC ok, 1 if UEKC is detected
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ERROR_IRQ_MASK Illegal parameter passed for errIrqMsk, valid parameter is in the range of 0x00 to 0x1FF
 */
mykonosDbgErr_t MYKONOS_deframerSetIrqMask(mykonosDevice_t *device, uint8_t errIrqMsk)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0x0000;
    uint8_t regWr = 0x0000;

    const uint8_t IRQ_MSK_CTRL_ADDR = 0x7A;
    const uint8_t IRQ_MSK = 0x0F;
    const uint8_t CMMIRQ_MSK = 0x08;
    const uint8_t CMMIRQ_CTRL = 0x7B;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdSetErrMask()\n");
#endif

    /* Check for valid errIrqMask */
    if (errIrqMsk & ~IRQ_MSK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ERROR_IRQ_MASK,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ERROR_IRQ_MASK));
        return MYKONOS_ERR_DBG_ERROR_IRQ_MASK;
    }

    /* Setting required IRQs */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, CMMIRQ_CTRL, &regRd)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    regWr = (CMMIRQ_MSK & errIrqMsk) | (regRd & ~CMMIRQ_MSK);

    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, CMMIRQ_CTRL, regWr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    regWr = (errIrqMsk & 0x07) << 5;
    if ((error = MYKONOS_jesdIndWrReg(device, jesdCore, IRQ_MSK_CTRL_ADDR, regWr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
    }

    return error;
}

/**
 * \brief Performs a read of the JESD IRQ vector for the deframer.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param errIrq Pointer to store the deframer IRQ status, the different bits mask are:
 *
*    IRQ       |   Bit mask   |  Description of errIrq
 * ------------|--------------|-----------------------
 *   CMM       |     3        | Configuration mismatch 0 if is CMM ok, 1 if CMM is detected
 *   BADDISP   |     2        | Incorrect disparity 0 if is BADDISP ok, 1 if BADDISP character is detected
 *   NIT       |     1        | Not in table 0 if is NIT ok, 1 if NIT character is detected
 *   UEKC      |     0        | Unexpected K character 0 if is UEKC ok, 1 if UEKC character is detected
 *
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 * \retval MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER Data pointer passed errIrqMask is NULL
 */
mykonosDbgErr_t MYKONOS_deframerGetIrq(mykonosDevice_t *device, uint8_t *errIrq)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_OK;
    mykonos_jesdcore_t jesdCore = MYK_DEFRAMER;
    uint8_t regRd = 0x00;
    uint8_t irqRd = 0x00;

    const uint8_t IRQ_MSK_CTRL_ADDR = 0x7A;
    const uint8_t CMMIRQ_CTRL = 0x7B;
    const uint8_t CMMIRQ_MASK_REG = 0x10;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_jesdGetErrMask()\n");
#endif

    /* Check for valid errIrq pointer */
    if (errIrq == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER));
        return MYKONOS_ERR_DBG_ILLEGAL_DATA_POINTER;
    }

    /* Reading main IRQ */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, IRQ_MSK_CTRL_ADDR, &regRd))!= MYKONOS_ERR_DBG_OK)
    {
        return error;
    }

    irqRd = (regRd & 0xE0) >> 5;


    /* Reading CMMIRQ */
    if ((error = MYKONOS_jesdIndRdReg(device, jesdCore, CMMIRQ_CTRL, &regRd))!= MYKONOS_ERR_DBG_OK)
    {
        return error;
    }

    irqRd |= ((regRd & CMMIRQ_MASK_REG) >> 1);

    /* Computing final value */
    *errIrq = irqRd;

    return error;
}

/**
 * \brief Performs set JESD framer Pattern Generator.
 *
 * The Pattern Generator is a function for JESD testing. The JESD framer will be transmitting
 * the "pattern" once the enabled.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore selection for FRAMER and OBS_FRAMER
 * \param pattern pattern to be transmitted by the Framer, is a 20 bit field having a range from 0x00000 to 0xFFFFF
 * \param enable Bit used to enable the Pattern Generator facility
 * \param toggle if 0 the transfer data will be not modified, if 1 the transfered data will be toggling
 *
 * \retval MYKONOS_ERR_DBG_ILLEGAL_TOGGLE Toggle is not valid, valid values are 0 and 1.
 * \retval MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN Pattern passed is outside the range valid range from 0x00000 to 0xFFFFF
 * \retval MYKONOS_ERR_DBG_ILLEGAL_ENABLE Enable not valid, valid values are 0 and 1.
 * \retval MYKONOS_ERR_DBG_PATTERN_GEN_ILLEGAL_JESD_CORE
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_framerSetPatternGen(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint32_t pattern, uint8_t enable, uint8_t toggle)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_FAIL;
    uint16_t baseAddr = 0x00;
    uint8_t rgWr = 0x00;

    const uint32_t PATTERN_MSK = 0xFFFFF;
    const uint8_t ENABLE_MASK = 0x01;
    const uint8_t TOGGLE_MASK = 0x02;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_framerSetPatternGen()\n");
#endif

    /* JESD framer core selection */
    if ((error = MYKONOS_framerCoreSel(jesdCore, MYKONOS_ADDR_FRAMER_PATTERN_GEN_EN, &baseAddr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    /* Check for valid enable */
    if (enable == 1)
    {
        rgWr = ENABLE_MASK;
        /* check for valid pattern */
        if (pattern & ~PATTERN_MSK)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN));
            return MYKONOS_ERR_DBG_ILLEGAL_FRAMER_PATTERN;
        }

        /* Check for valid toggle */
        if (toggle == 1)
        {
            rgWr += TOGGLE_MASK;

        }
        else if (toggle > 1)
        {
            CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_TOGGLE,
                    getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_TOGGLE));
            return MYKONOS_ERR_DBG_ILLEGAL_TOGGLE;
        }
    }
    else if (enable ==0)
    {
        rgWr = 0x00;
        pattern = 0x5A5A5;
    }
    else if (enable > 1)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ILLEGAL_ENABLE,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ILLEGAL_ENABLE));
        return MYKONOS_ERR_DBG_ILLEGAL_ENABLE;
    }

    /* Write pattern and enable */
    CMB_SPIWriteByte(device->spiSettings, baseAddr, rgWr);
    CMB_SPIWriteByte(device->spiSettings, (baseAddr + 1), (uint8_t)(pattern & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, (baseAddr + 2), (uint8_t)((pattern >> 8) & 0xFF));
    CMB_SPIWriteByte(device->spiSettings, (baseAddr + 3), (uint8_t)((pattern >> 16) & 0x0F));

    return MYKONOS_ERR_DBG_OK;
}

/**
 * \brief Gets JESD framer Pattern generator
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore selection for FRAMER and OBS_FRAMER
 * \param pattern Pointer to store the programmed pattern
 * \param enable Pointer to store the programmed enable
 * \param toggle Pointer to store the programmed toggle
 *
 * \retval MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN Function parameter pattern is a NULL pointer
 * \retval MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE Function parameter enable is a NULL pointer
 * \retval MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE Function parameter toggle is a NULL pointer
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_framerGetPatternGen(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint32_t *pattern, uint8_t *enable, uint8_t *toggle)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_FAIL;
    uint16_t baseAddr = 0x00;
    uint8_t regRd = 0x00;
    uint32_t patRd = 0x00000;
    uint8_t toggleRd = 0x00;
    uint8_t enableRd = 0x00;

    const uint8_t ENABLE_MASK = 0x01;
    const uint8_t TOGGLE_MASK = 0x02;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_framerGetPatternGen()\n");
#endif

    /* Check null parameter passed */
    if (pattern == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN));
        return MYKONOS_ERR_DBG_PATTERN_GEN_NULL_PATTERN;
    }

    /* Check null parameter passed */
    if (enable == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE));
        return MYKONOS_ERR_DBG_PATTERN_GEN_NULL_ENABLE;
    }

    /* Check null parameter passed */
    if (toggle == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE));
        return MYKONOS_ERR_DBG_PATTERN_GEN_NULL_TOGGLE;
    }

    /* JESD framer core selection */
    if ((error = MYKONOS_framerCoreSel(jesdCore, MYKONOS_ADDR_FRAMER_PATTERN_GEN_EN, &baseAddr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    /* Read back pattern generator */
    CMB_SPIReadByte(device->spiSettings, baseAddr, &regRd);
    toggleRd = (regRd & TOGGLE_MASK)>>1;
    enableRd = regRd & ENABLE_MASK;

    CMB_SPIReadByte(device->spiSettings, (baseAddr + 1), &regRd);
    patRd = regRd;

    CMB_SPIReadByte(device->spiSettings, (baseAddr + 2), &regRd);
    patRd |= (regRd << 8);

    CMB_SPIReadByte(device->spiSettings, (baseAddr + 3), &regRd);
    patRd |= ((regRd & 0x0F) << 16);

    /* Write to pointers */
    *pattern = patRd;
    *toggle = toggleRd;
    *enable = enableRd;

    return MYKONOS_ERR_DBG_OK;
}


/**
 * \brief Performs set JESD framer Zero data samples per lane.
 *
 * This functions will zero the Framer data samples for the specify lane or lanes.
 * For the specified Framer or ObsFramer.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore selection for FRAMER and OBS_FRAMER
 * \param lane lanes to zero the Framer data samples
 *
 * \retval MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE the lane selection is out of range
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_framerSetZeroData(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, mykonosLaneSel_t lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_FAIL;
    uint16_t baseAddr = 0x00;

    const uint32_t LANE_MSK = 0x0F;

#if MYKONOS_VERBOSE == 1
    CMB_writeToLog(ADIHAL_LOG_MESSAGE, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_framerSetZeroData()\n");
#endif

    /* check for valid pattern */
    if (lane & ~LANE_MSK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE));
        return MYKONOS_ERR_DBG_ZERO_DATA_INV_LANE;
    }

    /* JESD framer core selection */
    if ((error = MYKONOS_framerCoreSel(jesdCore, MYKONOS_ADDR_FRAMER_DATA_SAMPLE_CTL, &baseAddr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    /* Write Framer Data Zero samples */
    CMB_SPIWriteByte(device->spiSettings, baseAddr, lane);

    return MYKONOS_ERR_DBG_OK;
}


/**
 * \brief Performs get JESD framer Zero data samples.
 *
 * This functions will get the settings for zero the Framer data samples.
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param device Pointer to Mykonos device data structure containing settings
 * \param jesdCore selection for MYK_FRAMER and MYK_OBS_FRAMER
 * \param lane Pointer to store the programmed pattern
 *
 * \retval MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL Function parameter lane is a NULL pointer
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_framerGetZeroData(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, mykonosLaneSel_t *lane)
{
    mykonosDbgErr_t error = MYKONOS_ERR_DBG_FAIL;
    uint16_t baseAddr = 0x00;
    uint8_t regRd = 0x00;

    const uint8_t LANE_MASK = 0x0F;

#ifdef MYKONOS_VERBOSE
    CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_OK, "MYKONOS_framerGetZeroData()\n");
#endif

    /* Check null parameter passed */
    if (lane == NULL)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL,
                getDbgJesdMykonosErrorMessage(MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL));
        return MYKONOS_ERR_DBG_ZERO_DATA_LANE_NULL;
    }

    /* JESD framer core selection */
    if ((error = MYKONOS_framerCoreSel(jesdCore, MYKONOS_ADDR_FRAMER_DATA_SAMPLE_CTL, &baseAddr)) != MYKONOS_ERR_DBG_OK)
    {
        CMB_writeToLog(ADIHAL_LOG_ERROR, device->spiSettings->chipSelectIndex, error,
                getDbgJesdMykonosErrorMessage(error));
        return error;
    }

    /* Read back data zeroed register */
    CMB_SPIReadByte(device->spiSettings, baseAddr, &regRd);
    regRd = (regRd & LANE_MASK);

    /* Write to pointers */
    *lane = (mykonosLaneSel_t)regRd;

    return MYKONOS_ERR_DBG_OK;
}


/**
 * \brief Helper function to perform JESD framer core selection.
 *
 *
 * <B>Dependencies</B>
 * - device->spiSettings->chipSelectIndex
 *
 * \param jesdCore selection for FRAMER and OBS_FRAMER
 * \param rxFramerAdd base address for Framer selection
 * \param baseAddr Pointer to store the selected address base
 *
 * \retval MYKONOS_ERR_DBG_FRAMER_SEL_BASE_ADD_NULL base address is null
 * \retval MYKONOS_ERR_DBG_FRAMER_ILLEGAL_JESD_CORE Only valid JESD cores are FRAMER and OBS_FRAMER
 * \retval MYKONOS_ERR_DBG_OK Function completed successfully
 */
mykonosDbgErr_t MYKONOS_framerCoreSel(mykonos_jesdcore_t jesdCore, uint16_t rxFramerAdd, uint16_t *baseAddr)
{
    const uint16_t OFFSET = 0xD67;

    /* Check null parameter passed */
    if (baseAddr == NULL)
    {
        return MYKONOS_ERR_DBG_FRAMER_SEL_BASE_ADD_NULL;
    }

    /* Check for jesdCore and address assignment */
    switch (jesdCore)
    {
        case MYK_FRAMER:
            *baseAddr = rxFramerAdd;
            break;

        case MYK_OBS_FRAMER:
            *baseAddr = rxFramerAdd + OFFSET;
            break;

        case MYK_DEFRAMER:
        default:
            return MYKONOS_ERR_DBG_FRAMER_ILLEGAL_JESD_CORE;
    }

    return MYKONOS_ERR_DBG_OK;
}
