/**
 * \file mykonos_dbgjesd.h
 *
 * \brief Contains macro definitions and global structure declarations for mykonos_dbgjesd.c
 *
 * Mykonos API version: 1.5.2.3566
 */

/**
* \page Disclaimer Legal Disclaimer
* Copyright 2015-2017 Analog Devices Inc.
* Released under the AD9371 API license, for more information see the "LICENSE.txt" file in this zip file.
*
*/

#ifndef MYKONOS_DBGJESD_H_
#define MYKONOS_DBGJESD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mykonos.h"
#include "t_mykonos_gpio.h"
#include "t_mykonos_dbgjesd.h"
#include "mykonos_user.h"

/*
 *****************************************************************************
 * JESD debug functions
 *****************************************************************************
 */
mykonosDbgErr_t MYKONOS_deframerRd2Stat(mykonosDevice_t *device, uint8_t *defStat2);

mykonosDbgErr_t MYKONOS_deframerSetErrThrs(mykonosDevice_t *device, uint8_t errThrs);
mykonosDbgErr_t MYKONOS_deframerGetErrThrs(mykonosDevice_t *device, uint8_t *errThrs);

mykonosDbgErr_t MYKONOS_deframerSetIrqMask(mykonosDevice_t *device, uint8_t errIrqMsk);
mykonosDbgErr_t MYKONOS_deframerGetIrq(mykonosDevice_t *device, uint8_t *errIrq);

mykonosDbgErr_t MYKONOS_deframerSetSyncMask(mykonosDevice_t *device, uint8_t errSyncMask);
mykonosDbgErr_t MYKONOS_deframerGetSyncMask(mykonosDevice_t *device, uint8_t *errSyncMask);

mykonosDbgErr_t MYKONOS_deframerRdErrCntr(mykonosDevice_t *device, mykonosLaneSel_t laneSel, mykonosLaneErr_t *laneErr);

mykonosDbgErr_t MYKONOS_deframerGetErrLane(mykonosDevice_t *device, mykonosErrType_t errType, uint8_t *lane);

mykonosDbgErr_t MYKONOS_deframerRstErrCntr(mykonosDevice_t *device, mykonosErrType_t errType, mykonosLaneSel_t lane);
mykonosDbgErr_t MYKONOS_deframerRstErrIrq(mykonosDevice_t *device, mykonosErrType_t errType, mykonosLaneSel_t lane);

mykonosDbgErr_t MYKONOS_deframerGetEnLanes(mykonosDevice_t *device, uint8_t *lane);

mykonosDbgErr_t MYKONOS_deframerForceSyncReq(mykonosDevice_t *device, uint8_t syncReq);

mykonosDbgErr_t MYKONOS_framerSetPatternGen(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint32_t pattern, uint8_t enable, uint8_t toggle);
mykonosDbgErr_t MYKONOS_framerGetPatternGen(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint32_t *pattern, uint8_t *enable, uint8_t *toggle);

mykonosDbgErr_t MYKONOS_framerSetZeroData(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, mykonosLaneSel_t lane);
mykonosDbgErr_t MYKONOS_framerGetZeroData(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, mykonosLaneSel_t *lane);

/* Helper functions for indirect access to internal register */
mykonosDbgErr_t MYKONOS_jesdIndWrReg(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint8_t intAddr, uint8_t data);
mykonosDbgErr_t MYKONOS_jesdIndRdReg(mykonosDevice_t *device, mykonos_jesdcore_t jesdCore, uint8_t intAddr, uint8_t *data);

mykonosDbgErr_t MYKONOS_framerCoreSel(mykonos_jesdcore_t jesdCore, uint16_t rxFramerAdd, uint16_t *baseAddr);


const char* getDbgJesdMykonosErrorMessage(mykonosDbgErr_t errorCode);

#ifdef __cplusplus
}
#endif

#endif /* MYKONOS_DBGJESD_H_ */
