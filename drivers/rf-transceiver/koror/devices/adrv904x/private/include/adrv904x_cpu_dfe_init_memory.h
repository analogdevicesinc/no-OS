/**
 * Copyright 2020 - 2021 Analog Devices Inc.
 * Released under the ADRV904X Software license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

/**
 * \file
 * \brief This file contains DFE memory definitions
 *
 * DFE SDK API Version: 2.10.0.4
 */

#ifndef __ADRV904X_CPU_DFE_PROFILE_INIT_MEMORY_H__
#define __ADRV904X_CPU_DFE_PROFILE_INIT_MEMORY_H__

#include "adrv904x_cpu_device_profile_types.h"

#include "adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#define _ADI_ADRV904X_DFE_RS_INIT_SIZE_BYTES    (8200U) /*!< Size of RS init memory placeholder */

/****************************************************************************
* Data structure to be loaded at the end of DFE proc memory before boot time.
* DFE processor will move this block of data to its final destination during boot
* so that this memory region can be reclaimed after boot.
*
* DFE FW can access the members of this structure at the end of L4 memory region
* E.g. DFE profile temporary address can be calculated as follows:
* ADRV904X_DFE_CODE_MEM_REGION_END - sizeof(adrv904x_DfeMemInitTemp_t) + offsetof(adrv904x_DfeMemInitTemp_t, deviceProfile)
****************************************************************************/
ADI_ADRV904X_PACK_START
typedef struct adrv904x_DfeMemInitTemp
{
    adrv904x_DeviceProfile_t deviceProfile;                         /*!< DFE profile */
    uint32_t                 dfeImageCrc;                           /*!< DFE Image CRC */
    adrv904x_DfeSvcRadioCtrlSequencerInitData_t radioSeqInitData;   /*!< Radio sequencer init data */
    uint8_t                                     swBootFlag;         /*!< FW boot flag to be set by BBIC */
} adrv904x_DfeMemInitTemp_t;
ADI_ADRV904X_PACK_FINISH

#endif /* __ADRV904X_CPU_DFE_PROFILE_INIT_MEMORY_H__*/


