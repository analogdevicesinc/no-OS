/** 
 * \file adrv903x_bf_actrl_orx_west_regmap_types.h Automatically generated file with generator ver 0.0.13.0.
 * 
 * \brief Contains BitField functions to support ADRV903X transceiver device.
 * 
 * ADRV903X BITFIELD VERSION: 0.0.0.8
 * 
 * Disclaimer Legal Disclaimer
 * 
 * Copyright 2015 - 2025 Analog Devices Inc.
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_
#define _ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_

typedef enum adrv903x_BfActrlOrxWestRegmapChanAddr
{
    ADRV903X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP    =    0x6106A400,
    ADRV903X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP    =    0x6116A400
} adrv903x_BfActrlOrxWestRegmapChanAddr_e;

/** 
 * \brief Enumeration for trmAtten
 */

typedef enum adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten
{
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_0DB                       =    0,  /*!< 0dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_1DB                       =    1,  /*!< 1dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_2DB                       =    2,  /*!< 2dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_3DB                       =    3,  /*!< 3dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_4DB                       =    4,  /*!< 4dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_5DB                       =    5,  /*!< 5dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_6DB                       =    6,  /*!< 6dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_7DB                       =    7,  /*!< 7dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_8DB                       =    8,  /*!< 8dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_9DB                       =    9,  /*!< 9dB Attenuation                                                                                                     */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_10DB                      =   10,  /*!< 10dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_11DB                      =   11,  /*!< 11dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_12DB                      =   12,  /*!< 12dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_13DB                      =   13,  /*!< 13dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_14DB                      =   14,  /*!< 14dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_15DB                      =   15,  /*!< 15dB Attenuation                                                                                                    */
    ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_16DB                      =   16   /*!< 16dB Attenuation                                                                                                    */
} adrv903x_Bf_ActrlOrxWestRegmap_TrmAtten_e;

#endif // _ADRV903X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_

