/*! ****************************************************************************
 *
 * @file:    device_profile_logen_spare_reg_t.h
 *
 * @brief:   exported Logen functions
 *
 * @details: This file contains all the exported Logen driver prototypes
 *           referenced globally.
 *
 *
 * @date:    $Date:
 *
 *******************************************************************************
  Copyright(c) 2017 Analog Devices, Inc. All Rights Reserved. This software is
  proprietary & confidential to Analog Devices, Inc. and its licensors. By using
  this software you agree to the terms of the associated Analog Devices License
  Agreement.
 *******************************************************************************
 */
#ifndef __DEVICE_PROFILE_LOGEN_SPARE_REG_HEADER__
#define __DEVICE_PROFILE_LOGEN_SPARE_REG_HEADER__

#include "adi_device_profile_pack.h"

/* Absolute address */
#define ADDR_SPI_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER ADDR_SPI_CORE_1_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_6
#define IDX_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER      IDX_CORE_1_SOFTWARE_SPARE_REGISTERS_SOFTWARE_SPARE_6

#define pREG_SPI_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER    ((volatile       uint8_t *)      ADDR_SPI_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER)
#define pREG_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER(base)  ((volatile       uint8_t *)      ((base)+IDX_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER))

/* Bitfield definition */
/* ----------------------------------------------------------------------------------------------------
    STREAM_FLAGS_LO_MAPPING_REGISTER       Value             Description
   ---------------------------------------------------------------------------------------------------- */
#define BITP_CORE_1_STREAM_FLAG_LO_MAPPING             (0U)           /* No description provided */
#define BITL_CORE_1_STREAM_FLAG_LO_MAPPING             (4U)           /* No description provided */
#define BITM_CORE_1_STREAM_FLAG_LO_MAPPING             (0X0FU)        /* No description provided */
#define BITP_CORE_1_STREAM_FLAG_HOP1_RETUNE_LO_SEL     (4U)           /* No description provided */
#define BITL_CORE_1_STREAM_FLAG_HOP1_RETUNE_LO_SEL     (1U)           /* No description provided */
#define BITM_CORE_1_STREAM_FLAG_HOP1_RETUNE_LO_SEL     (0X10U)        /* No description provided */
#define BITP_CORE_1_STREAM_FLAG_HOP2_RETUNE_LO_SEL     (5U)           /* No description provided */
#define BITL_CORE_1_STREAM_FLAG_HOP2_RETUNE_LO_SEL     (1U)           /* No description provided */
#define BITM_CORE_1_STREAM_FLAG_HOP2_RETUNE_LO_SEL     (0X20U)        /* No description provided */

/* INST_SPI_CORE_1 */
#define WRITE_CORE_1_STREAM_FLAG_LO_MAPPING(base,val)    WRITE_BF_8BIT_REG(\
    pREG_CORE_1_STREAM_FLAGS_LO_MAPPING_REGISTER(base), \
    BITP_CORE_1_STREAM_FLAG_LO_MAPPING, \
    BITM_CORE_1_STREAM_FLAG_LO_MAPPING, \
    (val))

#endif /* __DEVICE_PROFILE_LOGEN_SPARE_REG_HEADER__ */
