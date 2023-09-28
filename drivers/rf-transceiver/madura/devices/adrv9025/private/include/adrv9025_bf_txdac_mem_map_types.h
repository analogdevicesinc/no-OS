/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_bf_txdac_mem_map_types.h Automatically generated file with generator ver 0.0.1.0.
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

#ifndef __ADRV9025_BF_TXDAC_MEM_MAP_TYPES_H__
#define __ADRV9025_BF_TXDAC_MEM_MAP_TYPES_H__

#ifdef __cplusplus
extern "C"{
#endif

typedef enum adrv9025_BfTxdacMemMapChanAddr
{
     ADRV9025_BF_ALL_TXDAC    =    0x1000,
     ADRV9025_BF_TXDAC_CH1    =    0x7400,
     ADRV9025_BF_TXDAC_CH2    =    0x7600,
     ADRV9025_BF_TXDAC_CH3    =    0x7800,
     ADRV9025_BF_TXDAC_CH0    =    0x7A00
} adrv9025_BfTxdacMemMapChanAddr_e;

/**
 * \brief Enumeration for txdacCalAddrI
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalAddrI
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISBREF     =    0,  /*!< ISB_REF (Reference For Calibration, Does not affect INL/DNL)       */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB0       =    1,  /*!< ISB0                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB1       =    2,  /*!< ISB1                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB2       =    3,  /*!< ISB2                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB3       =    4,  /*!< ISB3                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB4       =    5,  /*!< ISB4                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB5       =    6,  /*!< ISB5                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB6       =    7,  /*!< ISB6                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB7       =    8,  /*!< ISB7                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB8       =    9,  /*!< ISB8                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB9       =   10,  /*!< ISB9                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB10      =   11,  /*!< ISB10                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB11      =   12,  /*!< ISB11                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB12      =   13,  /*!< ISB12                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB13      =   14,  /*!< ISB13                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISB14      =   15,  /*!< ISB14                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_LSBSUM     =   16,  /*!< LSB Sum                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED0    =   17,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED1    =   18,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED2    =   19,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED3    =   20,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED4    =   21,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED5    =   22,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED6    =   23,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSBCOMPPOS =   24,  /*!< MSB Comparator Positive                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSBCOMPNEG =   25,  /*!< MSB Comparator Negative                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISBCOMPPOS =   26,  /*!< ISB Comparator Positive                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_ISBCOMPNEG =   27,  /*!< ISB Comparator Negative                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED7    =   28,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED8    =   29,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_UNUSED9    =   30,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSBREF     =   31,  /*!< MSB Reference (Reference for Calibration, Does not affect INL/DNL) */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSBGAIN    =   32,  /*!< MSB Gain (Fullscale Current Adjustment, Does not affect INL/DNL)   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB0       =   33,  /*!< MSB0                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB1       =   34,  /*!< MSB1                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB2       =   35,  /*!< MSB2                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB3       =   36,  /*!< MSB3                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB4       =   37,  /*!< MSB4                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB5       =   38,  /*!< MSB5                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB6       =   39,  /*!< MSB6                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB7       =   40,  /*!< MSB7                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB8       =   41,  /*!< MSB8                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB9       =   42,  /*!< MSB9                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB10      =   43,  /*!< MSB10                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB11      =   44,  /*!< MSB11                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB12      =   45,  /*!< MSB12                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB13      =   46,  /*!< MSB13                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB14      =   47,  /*!< MSB14                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB15      =   48,  /*!< MSB15                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB16      =   49,  /*!< MSB16                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB17      =   50,  /*!< MSB17                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB18      =   51,  /*!< MSB18                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB19      =   52,  /*!< MSB19                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB20      =   53,  /*!< MSB20                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB21      =   54,  /*!< MSB21                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB22      =   55,  /*!< MSB22                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB23      =   56,  /*!< MSB23                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB24      =   57,  /*!< MSB24                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB25      =   58,  /*!< MSB25                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB26      =   59,  /*!< MSB26                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB27      =   60,  /*!< MSB27                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB28      =   61,  /*!< MSB28                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB29      =   62,  /*!< MSB29                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_CALADDR_MSB30      =   63   /*!< MSB30                                                              */
} adrv9025_BfTxdacMemMapTxdacCalAddrI_e;
/**
 * \brief Enumeration for txdacCalAddrQ
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalAddrQ
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISBREF     =    0,  /*!< ISB_REF (Reference For Calibration, Does not affect INL/DNL)       */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB0       =    1,  /*!< ISB0                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB1       =    2,  /*!< ISB1                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB2       =    3,  /*!< ISB2                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB3       =    4,  /*!< ISB3                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB4       =    5,  /*!< ISB4                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB5       =    6,  /*!< ISB5                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB6       =    7,  /*!< ISB6                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB7       =    8,  /*!< ISB7                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB8       =    9,  /*!< ISB8                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB9       =   10,  /*!< ISB9                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB10      =   11,  /*!< ISB10                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB11      =   12,  /*!< ISB11                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB12      =   13,  /*!< ISB12                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB13      =   14,  /*!< ISB13                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISB14      =   15,  /*!< ISB14                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_LSBSUM     =   16,  /*!< LSB Sum                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED0    =   17,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED1    =   18,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED2    =   19,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED3    =   20,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED4    =   21,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED5    =   22,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED6    =   23,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSBCOMPPOS =   24,  /*!< MSB Comparator Positive                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSBCOMPNEG =   25,  /*!< MSB Comparator Negative                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISBCOMPPOS =   26,  /*!< ISB Comparator Positive                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_ISBCOMPNEG =   27,  /*!< ISB Comparator Negative                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED7    =   28,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED8    =   29,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_UNUSED9    =   30,  /*!< Unassigned                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSBREF     =   31,  /*!< MSB Reference (Reference for Calibration, Does not affect INL/DNL) */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSBGAIN    =   32,  /*!< MSB Gain (Fullscale Current Adjustment, Does not affect INL/DNL)   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB0       =   33,  /*!< MSB0                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB1       =   34,  /*!< MSB1                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB2       =   35,  /*!< MSB2                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB3       =   36,  /*!< MSB3                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB4       =   37,  /*!< MSB4                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB5       =   38,  /*!< MSB5                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB6       =   39,  /*!< MSB6                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB7       =   40,  /*!< MSB7                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB8       =   41,  /*!< MSB8                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB9       =   42,  /*!< MSB9                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB10      =   43,  /*!< MSB10                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB11      =   44,  /*!< MSB11                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB12      =   45,  /*!< MSB12                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB13      =   46,  /*!< MSB13                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB14      =   47,  /*!< MSB14                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB15      =   48,  /*!< MSB15                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB16      =   49,  /*!< MSB16                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB17      =   50,  /*!< MSB17                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB18      =   51,  /*!< MSB18                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB19      =   52,  /*!< MSB19                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB20      =   53,  /*!< MSB20                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB21      =   54,  /*!< MSB21                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB22      =   55,  /*!< MSB22                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB23      =   56,  /*!< MSB23                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB24      =   57,  /*!< MSB24                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB25      =   58,  /*!< MSB25                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB26      =   59,  /*!< MSB26                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB27      =   60,  /*!< MSB27                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB28      =   61,  /*!< MSB28                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB29      =   62,  /*!< MSB29                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_CALADDR_MSB30      =   63   /*!< MSB30                                                              */
} adrv9025_BfTxdacMemMapTxdacCalAddrQ_e;
/**
 * \brief Enumeration for txdacCalDebugI
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalDebugI
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_IDAC_NORMAL         =    0,  /*!< State machine operates normally and performs I DAC calibration.                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_IDAC_DEBUG          =    1   /*!< State machine ONLY performs a SAR operation of the CALDAC specified by txdac_cal_addr_i, then saves the SAR value to the specified CALDAC. */
} adrv9025_BfTxdacMemMapTxdacCalDebugI_e;
/**
 * \brief Enumeration for txdacCalDebugQ
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalDebugQ
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_QDAC_NORMAL         =    0,  /*!< State machine operates normally and performs Q DAC calibration.                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_QDAC_DEBUG          =    1   /*!< State machine ONLY performs a SAR operation of the CALDAC specified by txdac_cal_addr_q, then saves the SAR value to the specified CALDAC. */
} adrv9025_BfTxdacMemMapTxdacCalDebugQ_e;
/**
 * \brief Enumeration for txdacCalStartI
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalStartI
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_START_I_DISABLE     =    0,  /*!< Calibration state machine is immediately stopped and reset.                                                                                */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_START_I_TRIGGER     =    1   /*!< 0 to 1 transition starts DAC calibration.                                                                                                  */
} adrv9025_BfTxdacMemMapTxdacCalStartI_e;
/**
 * \brief Enumeration for txdacCalStartQ
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalStartQ
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_START_Q_DISABLE     =    0,  /*!< Calibration state machine is immediately stopped and reset.                                                                                */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CAL_START_Q_TRIGGER     =    1   /*!< 0 to 1 transition starts DAC calibration.                                                                                                  */
} adrv9025_BfTxdacMemMapTxdacCalStartQ_e;
/**
 * \brief Enumeration for txdacCalclkDiv
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCalclkDiv
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_OFF              =    0,  /*!< Calibration clock is OFF                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV17            =    1,  /*!< DAC clock divided by 2^17                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV16            =    2,  /*!< DAC clock divided by 2^16                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV15            =    3,  /*!< DAC clock divided by 2^15                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV14            =    4,  /*!< DAC clock divided by 2^14                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV13            =    5,  /*!< DAC clock divided by 2^13                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV12            =    6,  /*!< DAC clock divided by 2^12                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV11            =    7,  /*!< DAC clock divided by 2^11                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV10            =    8,  /*!< DAC clock divided by 2^10                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV9             =    9,  /*!< DAC clock divided by 2^9                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV8             =   10,  /*!< DAC clock divided by 2^8                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV7             =   11,  /*!< DAC clock divided by 2^7                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV6             =   12,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV6_13          =   13,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV6_14          =   14,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALCLK_DIV6_15          =   15   /*!< DAC clock divided by 2^6                                                                                                                   */
} adrv9025_BfTxdacMemMapTxdacCalclkDiv_e;
/**
 * \brief Enumeration for txdacCaldacFsc
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCaldacFsc
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_STEP_0P04        =    0,  /*!< 0.04 LSB Step Size (0.67x)                                                                                                                 */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_STEP_0P06        =    1,  /*!< 0.06 LSB Step Size (1x)                                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_STEP_0P08        =    2,  /*!< 0.08 LSB Step Size (1.33x)                                                                                                                 */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_STEP_0P12        =    3   /*!< 0.12 LSB Step Size (2x)                                                                                                                    */
} adrv9025_BfTxdacMemMapTxdacCaldacFsc_e;
/**
 * \brief Enumeration for txdacCaldacOffset
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCaldacOffset
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_OFFSET_0P0       =    0,  /*!< Offset = 0 LSB                                                                                                                             */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_OFFSET_3P5       =    1,  /*!< Offset = 3.5 LSB                                                                                                                           */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_OFFSET_4P0       =    2,  /*!< Offset = 4 LSB                                                                                                                             */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_OFFSET_4P5       =    3,  /*!< Offset = 4.5 LSB                                                                                                                           */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CALDAC_OFFSET_5P0       =    4   /*!< Offset = 5 LSB                                                                                                                             */
} adrv9025_BfTxdacMemMapTxdacCaldacOffset_e;
/**
 * \brief Enumeration for txdacCmCtrl
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCmCtrl
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_240MV                =    0,  /*!< Vcommon = 240mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_270MV                =    1,  /*!< Vcommon = 270mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_300MV                =    2,  /*!< Vcommon = 300mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_330MV                =    3,  /*!< Vcommon = 330mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_360MV                =    4,  /*!< Vcommon = 360mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_390MV                =    5,  /*!< Vcommon = 390mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_420MV                =    6,  /*!< Vcommon = 420mV                                                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_CM_450MV                =    7   /*!< Vcommon = 450mV                                                                                                                            */
} adrv9025_BfTxdacMemMapTxdacCmCtrl_e;
/**
 * \brief Enumeration for txdacCompGainIsb
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCompGainIsb
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_ISB_LOW       =    0,  /*!< Low Accuracy, Fast Settling                                                                                                                */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_ISB_HIGH      =    1   /*!< High Accuracy, Slow Settling                                                                                                               */
} adrv9025_BfTxdacMemMapTxdacCompGainIsb_e;
/**
 * \brief Enumeration for txdacCompGainMsb
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCompGainMsb
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_MSB_LOWER     =    0,  /*!< Low Accuracy, Fast Settling                                                                                                                */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_MSB_LOW       =    1,  /*!< Medium Accuracy/Settling                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_MSB_HIGH      =    2,  /*!< Not Allowed                                                                                                                                */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_GAIN_MSB_HIGHER    =    3   /*!< High Accuracy, Slow Settling                                                                                                               */
} adrv9025_BfTxdacMemMapTxdacCompGainMsb_e;
/**
 * \brief Enumeration for txdacCompIsbavg
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCompIsbavg
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_1           =    0,  /*!< run SAR once                                                                                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_2           =    1,  /*!< average 2 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_4           =    2,  /*!< average 4 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_8           =    3,  /*!< average 8 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_16          =    4,  /*!< average 16 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_32          =    5,  /*!< average 32 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_64          =    6,  /*!< average 64 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_ISBAVG_128         =    7   /*!< average 128 SAR runs                                                                                                                       */
} adrv9025_BfTxdacMemMapTxdacCompIsbavg_e;
/**
 * \brief Enumeration for txdacCompMsbavg
 */
typedef enum adrv9025_BfTxdacMemMapTxdacCompMsbavg
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_1           =    0,  /*!< run SAR once                                                                                                                               */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_2           =    1,  /*!< average 2 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_4           =    2,  /*!< average 4 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_8           =    3,  /*!< average 8 SAR runs                                                                                                                         */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_16          =    4,  /*!< average 16 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_32          =    5,  /*!< average 32 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_64          =    6,  /*!< average 64 SAR runs                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_COMP_MSBAVG_128         =    7   /*!< average 128 SAR runs                                                                                                                       */
} adrv9025_BfTxdacMemMapTxdacCompMsbavg_e;
/**
 * \brief Enumeration for txdacFscBoostI
 */
typedef enum adrv9025_BfTxdacMemMapTxdacFscBoostI
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC4MA             =    0,  /*!< FSC = 4mA                                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_IDAC_FSC5P6MA           =    1   /*!< FSC = 5.6mA                                                                                                                                */
} adrv9025_BfTxdacMemMapTxdacFscBoostI_e;
/**
 * \brief Enumeration for txdacFscBoostQ
 */
typedef enum adrv9025_BfTxdacMemMapTxdacFscBoostQ
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_FSC4MA             =    0,  /*!< FSC = 4mA                                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_QDAC_FSC5P6MA           =    1   /*!< FSC = 5.6mA                                                                                                                                */
} adrv9025_BfTxdacMemMapTxdacFscBoostQ_e;
/**
 * \brief Enumeration for txdacPowerdownClkdiv
 */
typedef enum adrv9025_BfTxdacMemMapTxdacPowerdownClkdiv
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_OFF               =    0,  /*!< Powerdown state machine clock is OFF                                                                                                       */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV17             =    1,  /*!< DAC clock divided by 2^17                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV16             =    2,  /*!< DAC clock divided by 2^16                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV15             =    3,  /*!< DAC clock divided by 2^15                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV14             =    4,  /*!< DAC clock divided by 2^14                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV13             =    5,  /*!< DAC clock divided by 2^13                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV12             =    6,  /*!< DAC clock divided by 2^12                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV11             =    7,  /*!< DAC clock divided by 2^11                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV10             =    8,  /*!< DAC clock divided by 2^10                                                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV9              =    9,  /*!< DAC clock divided by 2^9                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV8              =   10,  /*!< DAC clock divided by 2^8                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV7              =   11,  /*!< DAC clock divided by 2^7                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV6              =   12,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV6_13           =   13,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV6_14           =   14,  /*!< DAC clock divided by 2^6                                                                                                                   */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_PDCLK_DIV6_15           =   15   /*!< DAC clock divided by 2^6                                                                                                                   */
} adrv9025_BfTxdacMemMapTxdacPowerdownClkdiv_e;
/**
 * \brief Enumeration for txdacSelCalToPin
 */
typedef enum adrv9025_BfTxdacMemMapTxdacSelCalToPin
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISBREF_TO_PIN           =    0,  /*!< isb_ref                                                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISB_TO_PIN              =    1,  /*!< isb                                                                                                                                        */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_MSBREF_TO_PIN           =    2,  /*!< msb_ref                                                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_MSB_TO_PIN              =    3   /*!< msb                                                                                                                                        */
} adrv9025_BfTxdacMemMapTxdacSelCalToPin_e;
/**
 * \brief Enumeration for txdacStartupMansel
 */
typedef enum adrv9025_BfTxdacMemMapTxdacStartupMansel
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_STARTUP_SEL_DACCLK      =    0,  /*!< Normal mode: use txdac_powerdown_clkdiv to generate clock                                                                                  */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_STARTUP_SEL_SPI         =    1   /*!< Debug mode: use txdac_startup_manclk to directly drive the clock                                                                           */
} adrv9025_BfTxdacMemMapTxdacStartupMansel_e;
/**
 * \brief Enumeration for txdacThrowIsbCtrl
 */
typedef enum adrv9025_BfTxdacMemMapTxdacThrowIsbCtrl
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISBTHROW_0UA            =    0,  /*!< 0% ISB throw away current (worst performance)                                                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISBTHROW_600UA          =    1,  /*!< 33% ISB throw away current (worse performance)                                                                                             */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISBTHROW_1200UA         =    2,  /*!< 66% ISB throw away current (better performance)                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_ISBTHROW_1800UA         =    3   /*!< 100% ISB throw away current (best performance)                                                                                             */
} adrv9025_BfTxdacMemMapTxdacThrowIsbCtrl_e;
/**
 * \brief Enumeration for txdacThrowLsbCtrl
 */
typedef enum adrv9025_BfTxdacMemMapTxdacThrowLsbCtrl
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_LSBTHROW_0UA            =    0,  /*!< 0% LSB throw away current (worst performance)                                                                                              */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_LSBTHROW_400UA          =    1,  /*!< 33% LSB throw away current (worse performance)                                                                                             */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_LSBTHROW_800UA          =    2,  /*!< 66% LSB throw away current (better performance)                                                                                            */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_LSBTHROW_1200UA         =    3   /*!< 100% LSB throw away current (best performance)                                                                                             */
} adrv9025_BfTxdacMemMapTxdacThrowLsbCtrl_e;
/**
 * \brief Enumeration for txdacWalkPeriod
 */
typedef enum adrv9025_BfTxdacMemMapTxdacWalkPeriod
{
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_WALK_PERIOD_20          =    0,  /*!< Update rate = DAC clock divided by 2^20                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_WALK_PERIOD_23          =    1,  /*!< Update rate = DAC clock divided by 2^23                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_WALK_PERIOD_26          =    2,  /*!< Update rate = DAC clock divided by 2^26                                                                                                    */
    ADRV9025_BF_TXDAC_MEM_MAP_TXDAC_WALK_PERIOD_29          =    3   /*!< Update rate = DAC clock divided by 2^29                                                                                                    */
} adrv9025_BfTxdacMemMapTxdacWalkPeriod_e;
#ifdef __cplusplus
}
#endif

#endif // __ADRV9025_BF_TXDAC_MEM_MAP_TYPES_H__

/*  EOF: adrv9025_bf_txdac_mem_map_types.h */
