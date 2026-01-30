/**
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * \file adrv904x_dfe_memory.h
 * \brief This file contains DFE memory definitions
 *
 * DFE SDK API Version: 2.15.0.4
 */

#ifndef __ADRV904X_DFE_MEMORY_H__
#define __ADRV904X_DFE_MEMORY_H__

/* Starting address of DFE RAM */
#define ADRV904X_DFE_CODE_MEM_REGION_START    (0xC0000000U)

/* Ending address of DFE RAM */
#define ADRV904X_DFE_CODE_MEM_REGION_END      (0xC0800000U)

/* Ending address of DFE RAM */
#define ADRV904X_DFE_CODE_MEM_REGION_SIZE     (ADRV904X_DFE_CODE_MEM_REGION_END - ADRV904X_DFE_CODE_MEM_REGION_START)

#endif /* __ADRV904X_DFE_MEMORY_H__*/


