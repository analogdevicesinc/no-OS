/** 
 * \file adrv904x_bf_actrl_orx_west_regmap_types.h Automatically generated file with generator ver 0.0.16.0.
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

#ifndef _ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_
#define _ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_

typedef enum adrv904x_BfActrlOrxWestRegmapChanAddr
{
    ADRV904X_BF_SLICE_ORX_0__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP    =    0x6106A400,
    ADRV904X_BF_SLICE_ORX_1__ORX_ANALOG_ADC_8B_VENUS_ACTRL_WEST_REGMAP    =    0x6116A400
} adrv904x_BfActrlOrxWestRegmapChanAddr_e;

/** 
 * \brief Enumeration for trmAtten
 */

typedef enum adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten
{
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_0DB                       =    0,  /*!< 0dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_1DB                       =    1,  /*!< 1dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_2DB                       =    2,  /*!< 2dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_3DB                       =    3,  /*!< 3dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_4DB                       =    4,  /*!< 4dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_5DB                       =    5,  /*!< 5dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_6DB                       =    6,  /*!< 6dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_7DB                       =    7,  /*!< 7dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_8DB                       =    8,  /*!< 8dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_9DB                       =    9,  /*!< 9dB Attenuation                                                                                                     */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_10DB                      =   10,  /*!< 10dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_11DB                      =   11,  /*!< 11dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_12DB                      =   12,  /*!< 12dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_13DB                      =   13,  /*!< 13dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_14DB                      =   14,  /*!< 14dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_15DB                      =   15,  /*!< 15dB Attenuation                                                                                                    */
    ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TRM_ATTEN_ATTEN_16DB                      =   16   /*!< 16dB Attenuation                                                                                                    */
} adrv904x_Bf_ActrlOrxWestRegmap_TrmAtten_e;

#endif // _ADRV904X_BF_ACTRL_ORX_WEST_REGMAP_TYPES_H_

