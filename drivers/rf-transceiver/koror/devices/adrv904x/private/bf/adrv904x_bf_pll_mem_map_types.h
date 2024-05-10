/** 
 * \file adrv904x_bf_pll_mem_map_types.h Automatically generated file with generator ver 0.0.16.0.
 * 
 * \brief Contains BitField functions to support ADRV904X transceiver device.
 * 
 * ADRV904X BITFIELD VERSION: 0.0.0.11
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2021 Analog Devices Inc.
 * 
 * Released under the ADRV904X API license, for more information see the "LICENSE.PDF" file in this zip file.
 */

#ifndef _ADRV904X_BF_PLL_MEM_MAP_TYPES_H_
#define _ADRV904X_BF_PLL_MEM_MAP_TYPES_H_

typedef enum adrv904x_BfPllMemMapChanAddr
{
    ADRV904X_BF_DIGITAL_CORE_EAST_RFPLL    =    0x47300000,
    ADRV904X_BF_DIGITAL_CORE_WEST_RFPLL    =    0x47400000,
    ADRV904X_BF_DIGITAL_CORE_JESD_SERDES_PLL    =    0x48060000,
    ADRV904X_BF_DIGITAL_CORE_JESD_CLKPLL    =    0x48090000
} adrv904x_BfPllMemMapChanAddr_e;

/** 
 * \brief Enumeration for calper
 */

typedef enum adrv904x_Bf_PllMemMap_Calper
{
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER1          =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER2          =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER4          =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER8          =    3,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER16         =    4,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER32         =    5,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER64         =    6,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALPER_CALPER128        =    7   /*!< No description provided */
} adrv904x_Bf_PllMemMap_Calper_e;

/** 
 * \brief Enumeration for caltyp
 */

typedef enum adrv904x_Bf_PllMemMap_Caltyp
{
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_IDLE             =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_ICAL             =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_TCAL             =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_CTCAL            =    3,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_CRCAL            =    4,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_ICALC            =    5,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_TCALC            =    6,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_CALTYP_CTCALC           =    7   /*!< No description provided */
} adrv904x_Bf_PllMemMap_Caltyp_e;

/** 
 * \brief Enumeration for icalwait
 */

typedef enum adrv904x_Bf_PllMemMap_Icalwait
{
    ADRV904X_BF_PLL_MEM_MAP_ICALWAIT_ICALWAIT_1K      =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_ICALWAIT_ICALWAIT_2K      =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_ICALWAIT_ICALWAIT_4K      =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_ICALWAIT_ICALWAIT_8K      =    3   /*!< No description provided */
} adrv904x_Bf_PllMemMap_Icalwait_e;

/** 
 * \brief Enumeration for qthr
 */

typedef enum adrv904x_Bf_PllMemMap_Qthr
{
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR0            =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR1            =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR2            =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR3            =    3,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR4            =    4,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR5            =    5,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR6            =    6,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR7            =    7,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR8            =    8,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR9            =    9,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR10           =   10,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR11           =   11,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR12           =   12,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR13           =   13,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR14           =   14,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_QTHR_QTHR15           =   15   /*!< No description provided */
} adrv904x_Bf_PllMemMap_Qthr_e;

/** 
 * \brief Enumeration for tsprsc
 */

typedef enum adrv904x_Bf_PllMemMap_Tsprsc
{
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_1          =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_2          =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_4          =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_8          =    3,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_16         =    4,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_32         =    5,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_64         =    6,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_128        =    7,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_256        =    8,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_512        =    9,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_1K         =   10,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_2K         =   11,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_4K         =   12,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_8K         =   13,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_16K        =   14,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_TSPRSC_TPRSC_32K        =   15   /*!< No description provided */
} adrv904x_Bf_PllMemMap_Tsprsc_e;

/** 
 * \brief Enumeration for vcoCalInitDel
 */

typedef enum adrv904x_Bf_PllMemMap_VcoCalInitDel
{
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_1K   =    0,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_2K   =    1,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_4K   =    2,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_8K   =    3,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_16K  =    4,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_32K  =    5,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_64K  =    6,  /*!< No description provided */
    ADRV904X_BF_PLL_MEM_MAP_VCO_CAL_INIT_DEL_VCOCCAL_DEL_128K =    7   /*!< No description provided */
} adrv904x_Bf_PllMemMap_VcoCalInitDel_e;

#endif // _ADRV904X_BF_PLL_MEM_MAP_TYPES_H_

