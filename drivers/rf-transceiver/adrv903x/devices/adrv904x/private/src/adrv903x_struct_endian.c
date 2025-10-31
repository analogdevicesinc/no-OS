/**
* Copyright 2015 - 2025 Analog Devices Inc.
* SPDX-License-Identifier: Apache-2.0
*/


#include "../../private/include/adrv903x_struct_endian.h"

#ifndef ADI_LIBRARY_RM_FLOATS
static float byteSwapFloat(float val)
{
    float retVal;
    char *valAsCharArr = (char*) &val;
    char *retValAsCharArr = (char*) &retVal;

    retValAsCharArr[0] = valAsCharArr[3];
    retValAsCharArr[1] = valAsCharArr[2];
    retValAsCharArr[2] = valAsCharArr[1];
    retValAsCharArr[3] = valAsCharArr[0];

    return retVal;
}
#endif

void adrv903x_CpuCmd_SerdesCalStatusGet_swap(struct adrv903x_CpuCmd_SerdesCalStatusGet* _struct)
{
    (void) _struct;

    /* _struct->lane << Field length is 1 byte no endianness swap required */
}


void adrv903x_SerdesInitCalStatusCmdResp_swap(struct adrv903x_SerdesInitCalStatusCmdResp* _struct)
{
    (void) _struct;

    adrv903x_SerdesInitCalStatus_swap(&_struct->details);

    _struct->cpuErrorCode = ADRV903X_BYTESWAP_L(_struct->cpuErrorCode);
}

void adrv903x_SerdesTrackingCalStatusCmdResp_swap(struct adrv903x_SerdesTrackingCalStatusCmdResp* _struct)
{
    (void) _struct;

    adrv903x_SerdesTrackingCalStatus_swap(&_struct->details);

    _struct->cpuErrorCode = ADRV903X_BYTESWAP_L(_struct->cpuErrorCode);
}

void adrv903x_SerdesTrackingCalStatus_swap(struct adi_adrv903x_SerdesTrackingCalStatus* _struct)
{
    (void) _struct;

    _struct->temperature = ADRV903X_BYTESWAP_S(_struct->temperature);

    /* _struct->pd << Field length is 1 byte no endianness swap required */

    /* _struct->dllPeakPd << Field length is 1 byte no endianness swap required */

    /* _struct->dllPeakPdDelta << Field length is 1 byte no endianness swap required */

    /* _struct->innerUp << Field length is 1 byte no endianness swap required */

    /* _struct->innerDown << Field length is 1 byte no endianness swap required */

    /* _struct->outerUp << Field length is 1 byte no endianness swap required */

    /* _struct->outerDown << Field length is 1 byte no endianness swap required */

    /* _struct->b << Field length is 1 byte no endianness swap required */

#ifndef ADI_LIBRARY_RM_FLOATS
    for(int i = 0; i < 2; i++)
    {
        _struct->ps[i] = byteSwapFloat(_struct->ps[i]);
    }

    for(int i = 0; i < 16; i++)
    {
        _struct->yVector[i] = byteSwapFloat(_struct->yVector[i]);
    }
#else
    for(int i = 0; i < 2; i++)
    {
        _struct->ps_float[i] = ADRV903X_BYTESWAP_L(_struct->ps_float[i]);
    }

    for(int i = 0; i < 16; i++)
    {
        _struct->yVector_float[i] = ADRV903X_BYTESWAP_L(_struct->yVector_float[i]);
    }
#endif
}

void adrv903x_SerdesInitCalStatus_swap(struct adi_adrv903x_SerdesInitCalStatus* _struct)
{
    (void) _struct;

    _struct->temperature = ADRV903X_BYTESWAP_S(_struct->temperature);

    /* _struct->lpfIndex << Field length is 1 byte no endianness swap required */

    /* _struct->ctleIndex << Field length is 1 byte no endianness swap required */

    /* _struct->numEyes << Field length is 1 byte no endianness swap required */

    _struct->bsum = ADRV903X_BYTESWAP_S(_struct->bsum);

    _struct->bsum_dc = ADRV903X_BYTESWAP_S(_struct->bsum_dc);

    /* _struct->spoLeft << Field length is 1 byte no endianness swap required */

    /* _struct->spoRight << Field length is 1 byte no endianness swap required */

    _struct->eom = ADRV903X_BYTESWAP_S(_struct->eom);

    _struct->eomMax = ADRV903X_BYTESWAP_S(_struct->eomMax);

    /* _struct->pd << Field length is 1 byte no endianness swap required */

    /* _struct->innerUp << Field length is 1 byte no endianness swap required */

    /* _struct->innerDown << Field length is 1 byte no endianness swap required */

    /* _struct->outerUp << Field length is 1 byte no endianness swap required */

    /* _struct->outerDown << Field length is 1 byte no endianness swap required */

    /* _struct->b << Field length is 1 byte no endianness swap required */
}

