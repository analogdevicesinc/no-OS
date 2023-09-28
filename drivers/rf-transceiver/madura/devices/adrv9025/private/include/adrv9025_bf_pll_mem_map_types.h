/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_pll_mem_map_types.h Automatically generated file with generator ver 0.0.1.0.
 *
 * \brief Contains BitField functions to support ADRV9025 transciever device.
 *
 * ADRV9025 BITFIELD VERSION: 0.0.0.1
 */

/**
 * Disclaimer Legal Disclaimer
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef __ADRV9025_BF_PLL_MEM_MAP_TYPES_H__
#define __ADRV9025_BF_PLL_MEM_MAP_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfPllMemMapChanAddr
{
     ADRV9025_BF_ALL_PLLS    =    0x800,
     ADRV9025_BF_AUX_PLL    =    0x2600,
     ADRV9025_BF_CLK_PLL    =    0x2800,
     ADRV9025_BF_RF1_PLL    =    0x2A00,
     ADRV9025_BF_RF2_PLL    =    0x6400
} adrv9025_BfPllMemMapChanAddr_e;

/**
 * \brief Enumeration for calper
 */
typedef enum adrv9025_BfPllMemMapCalper
{
    ADRV9025_BF_PLL_MEM_MAP_CALPER1              =    0,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER2              =    1,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER4              =    2,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER8              =    3,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER16             =    4,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER32             =    5,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER64             =    6,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CALPER128            =    7   /*!< No description provided */
} adrv9025_BfPllMemMapCalper_e;
/**
 * \brief Enumeration for caltyp
 */
typedef enum adrv9025_BfPllMemMapCaltyp
{
    ADRV9025_BF_PLL_MEM_MAP_IDLE                 =    0,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_ICAL                 =    1,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_TCAL                 =    2,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CTCAL                =    3,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CRCAL                =    4,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_ICALC                =    5,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_TCALC                =    6,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_CTCALC               =    7   /*!< No description provided */
} adrv9025_BfPllMemMapCaltyp_e;
/**
 * \brief Enumeration for icalwait
 */
typedef enum adrv9025_BfPllMemMapIcalwait
{
    ADRV9025_BF_PLL_MEM_MAP_ICALWAIT_1K          =    0,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_ICALWAIT_2K          =    1,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_ICALWAIT_4K          =    2,  /*!< No description provided */
    ADRV9025_BF_PLL_MEM_MAP_ICALWAIT_8K          =    3   /*!< No description provided */
} adrv9025_BfPllMemMapIcalwait_e;
/**
 * \brief Enumeration for mcsPulseDelayDigSerdes
 */
typedef enum adrv9025_BfPllMemMapMcsPulseDelayDigSerdes
{
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_DELAY_DIG_SERDES_2 =    0,  /*!< mcs_pulse_delay_dig_serdes = 2 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_DELAY_DIG_SERDES_3 =    1,  /*!< mcs_pulse_delay_dig_serdes = 3 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_DELAY_DIG_SERDES_4 =    2,  /*!< mcs_pulse_delay_dig_serdes = 4 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_DELAY_DIG_SERDES_5 =    3   /*!< mcs_pulse_delay_dig_serdes = 5 prescaler clocks */
} adrv9025_BfPllMemMapMcsPulseDelayDigSerdes_e;
/**
 * \brief Enumeration for mcsPulseWidthDigSerdes
 */
typedef enum adrv9025_BfPllMemMapMcsPulseWidthDigSerdes
{
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_WIDTH_DIG_SERDES_2 =    0,  /*!< mcs_pulse_delay_dig_serdes = 2 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_WIDTH_DIG_SERDES_3 =    1,  /*!< mcs_pulse_delay_dig_serdes = 3 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_WIDTH_DIG_SERDES_4 =    2,  /*!< mcs_pulse_delay_dig_serdes = 4 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_WIDTH_DIG_SERDES_5 =    3   /*!< mcs_pulse_delay_dig_serdes = 5 prescaler clocks */
} adrv9025_BfPllMemMapMcsPulseWidthDigSerdes_e;
/**
 * \brief Enumeration for mcsSerdesAlign
 */
typedef enum adrv9025_BfPllMemMapMcsSerdesAlign
{
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_ALIGN_DIG_SERDES_2 =    0, /*!< mcs_pulse_delay_dig_serdes = 2 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_ALIGN_DIG_SERDES_3 =    1, /*!< mcs_pulse_delay_dig_serdes = 3 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_ALIGN_DIG_SERDES_4 =    2, /*!< mcs_pulse_delay_dig_serdes = 4 prescaler clocks */
    ADRV9025_BF_PLL_MEM_MAP_MCS_PULSE_ALIGN_DIG_SERDES_5 =    3   /*!< mcs_pulse_delay_dig_serdes = 5 prescaler clocks */
} adrv9025_BfPllMemMapMcsSerdesAlign_e;
/**
 * \brief Enumeration for qthr
 */
typedef enum adrv9025_BfPllMemMapQthr
{
    ADRV9025_BF_PLL_MEM_MAP_QTHR0                        =    0,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR1                        =    1,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR2                        =    2,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR3                        =    3,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR4                        =    4,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR5                        =    5,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR6                        =    6,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR7                        =    7,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR8                        =    8,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR9                        =    9,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR10                       =   10,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR11                       =   11,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR12                       =   12,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR13                       =   13,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR14                       =   14,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_QTHR15                       =   15   /*!< No description provided                         */
} adrv9025_BfPllMemMapQthr_e;
/**
 * \brief Enumeration for tsprsc
 */
typedef enum adrv9025_BfPllMemMapTsprsc
{
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_1                      =    0,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_2                      =    1,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_4                      =    2,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_8                      =    3,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_16                     =    4,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_32                     =    5,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_64                     =    6,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_TPRSC_128                    =    7   /*!< No description provided                         */
} adrv9025_BfPllMemMapTsprsc_e;
/**
 * \brief Enumeration for vcoCalInitDel
 */
typedef enum adrv9025_BfPllMemMapVcoCalInitDel
{
    ADRV9025_BF_PLL_MEM_MAP_VCOCCAL_DEL_32               =    0,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_VCOCCAL_DEL_64               =    1,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_VCOCCAL_DEL_128              =    2,  /*!< No description provided                         */
    ADRV9025_BF_PLL_MEM_MAP_VCOCCAL_DEL_256              =    3   /*!< No description provided                         */
} adrv9025_BfPllMemMapVcoCalInitDel_e;
#ifdef __cplusplus
}
#endif

#endif // __ADRV9025_BF_PLL_MEM_MAP_TYPES_H__

/*  EOF: adrv9025_bf_pll_mem_map_types.h */
