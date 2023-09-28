/* SPDX-License-Identifier: GPL-2.0 */
/**
 * \file adrv9025_shared_resource_manager_types.h
 * \brief Contains ADRV9025 shared resource related private data prototypes for
 *        adrv9025_shared_resource_manager.c
 *
 * ADRV9025 API Version: 6.4.0.14
 */

 /**
 * Copyright 2015 - 2020 Analog Devices Inc.
 * Released under the ADRV9025 API license, for more information
 * see the "LICENSE.pdf" file in this zip file.
 */

#ifndef _ADRV9025_SHARED_RESOURCE_MANAGER_TYPES_H_
#define _ADRV9025_SHARED_RESOURCE_MANAGER_TYPES_H_

#ifdef __KERNEL__
#include <linux/kernel.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define ADI_ADRV9025_NUM_GPIOS_IN_AGCOOVERLOADINDICATOR_FEATURE 12

/* Macros defining the maximum semaphore count for a given feature */
#define ADRV9025_FEATURE_RX_GAIN_CTRL_PIN_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_RX_PIN_CTRL_ATTN_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_RX_DUALBAND_LNA_CTRL_WORD_OUTPUT_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_TX_ATTEN_UPDATE_PIN_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_SPI2_CTRL_PIN_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_RX_EXT_SLICER_CTRL_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_AUX_DAC_OUT_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_GPIO_STREAM_TRIGGER_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_FOVR_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_PCA_MAX_SEMAPHORE_COUNT 4
#define ADRV9025_FEATURE_RX_DUALBAND_CTRL_OUTPUT_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_ARM_GPIO_PIN_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN_MAX_SEMAPHORE_COUNT 1
#define ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION_TO_GPIOS_OUT_MAX_SEMAPHORE_COUNT 11
#define ADRV9025_FEATURE_SYNC_AGC_PIN_COUNT 1
/*** Please add shared resource maximum semaphore count above this line ****/

typedef enum adrv9025_SharedResourceType
{
    ADRV9025_SHARED_RESOURCE_GPIO = 0,        /*!< Select GPIO as the Shared Resource type*/
    ADRV9025_SHARED_RESOURCE_GPIO_ANALOG = 1, /*!< Select GPIO Analog as the Shared Resource type*/
    /***Please add shared resource type above this line****/
    ADRV9025_NUM_SHARED_RESOURCE_TYPES
} adrv9025_SharedResourceType_e;

typedef enum adrv9025_SharedResourceID
{
    /******START OF GPIO********/
    ADRV9025_GPIO_00 = 0,       /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_00*/
    ADRV9025_GPIO_01,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_01*/
    ADRV9025_GPIO_02,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_02*/
    ADRV9025_GPIO_03,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_03*/
    ADRV9025_GPIO_04,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_04*/
    ADRV9025_GPIO_05,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_05*/
    ADRV9025_GPIO_06,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_06*/
    ADRV9025_GPIO_07,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_07*/
    ADRV9025_GPIO_08,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_08*/
    ADRV9025_GPIO_09,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_09*/
    ADRV9025_GPIO_10,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_11*/
    ADRV9025_GPIO_11,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_12*/
    ADRV9025_GPIO_12,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_13*/
    ADRV9025_GPIO_13,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_14*/
    ADRV9025_GPIO_14,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_15*/
    ADRV9025_GPIO_15,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_16*/
    ADRV9025_GPIO_16,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_17*/
    ADRV9025_GPIO_17,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_18*/
    ADRV9025_GPIO_18,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_19*/
    /******START OF GPIO Analog********/
    ADRV9025_GPIO_ANA_00,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_00*/
    ADRV9025_GPIO_ANA_01,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_01*/
    ADRV9025_GPIO_ANA_02,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_02*/
    ADRV9025_GPIO_ANA_03,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_03*/
    ADRV9025_GPIO_ANA_04,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_04*/
    ADRV9025_GPIO_ANA_05,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_05*/
    ADRV9025_GPIO_ANA_06,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_06*/
    ADRV9025_GPIO_ANA_07,           /*!< Unique Shared Resource ID for ADI_ADRV9025_GPIO_ANA_07*/
    /***Please add shared resources above this line***/
    ADRV9025_NUM_SHARED_RESOURCES,
    ADRV9025_SHARED_RESOURCE_INVALID
} adrv9025_SharedResourceID_e;

typedef enum adrv9025_FeatureID
{
    ADRV9025_FEATURE_UNUSED = 0,                       /*!< Default feature ID associated with a shared resource when unused*/
    ADRV9025_FEATURE_RX_GAIN_CTRL_PIN,                 /*!< Associates Rx Gain Ctrl Pin feature with a shared resource*/
    ADRV9025_FEATURE_RX_PIN_CTRL_ATTN,                 /*!< Associates Rx Parallel Pin Ctrl Attenuation feature with a shared resource*/
    ADRV9025_FEATURE_RX_EXT_CTRL_WORD_OUTPUT,          /*!< Associates Rx External Ctrl Word Output feature with a shared resource*/
    ADRV9025_FEATURE_RX_DUALBAND_LNA_CTRL_WORD_OUTPUT, /*!< Associates Rx Dualband LNA Ctrl Word Output feature with a shared resource*/
    ADRV9025_FEATURE_TX_ATTEN_CTRL_PIN,                /*!< Associates Tx Attenuation Ctrl Pin feature with a shared resource*/
    ADRV9025_FEATURE_SPI2_CTRL_PIN,                    /*!< Associates SPI2 Ctrl Pin feature with a shared resource */
    ADRV9025_FEATURE_ARM_GPIO_PIN,                     /*!< Associates ARM TDD Ctrl feature with a shared resource */
    ADRV9025_FEATURE_RX_EXT_SLICER_CTRL,               /*!< Associates Rx External Slicer Ctrl Word Input feature with a shared resource*/
    ADRV9025_FEATURE_RX_INT_SLICER_CTRL_OUT,           /*!< Associates Rx Internal Slicer Ctrl Word Input feature with a shared resource*/
    ADRV9025_FEATURE_AUX_DAC_OUT,                      /*!< Associates AuxDAC feature with a shared resource*/
    ADRV9025_FEATURE_GPIO_STREAM_TRIGGER,              /*!< Associates GPIO stream trigger mapping feature with a shared resource*/
    ADRV9025_FEATURE_FOVR,                             /*!< Associates FOVR mapping feature with a shared resource*/
    ADRV9025_FEATURE_PCA,                              /*!< Associates PCA mapping feature with a shared resource*/
    ADRV9025_FEATURE_RX_DUALBAND_CTRL_OUTPUT,          /*!< Associates Rx Dualband Ctrl Word Output feature with a shared resource*/
    ADRV9025_FEATURE_TX_ATTEN_UPDATE_PIN,              /*!< Associates Tx Attenuation Update Pin feature with a shared resource*/
    ADRV9025_FEATURE_EXT_DPD_CAPTURE_DONE_PIN,         /*!< Associates External DPD capture done indication GPIO feature with a shared resource*/
    ADRV9025_FEATURE_ADC_OVERLOAD_INDICATION,          /*!< Associates Rx Overload feature with a shared resource */
    ADRV9025_FEATURE_RX_AGC_HYBRID_MODE,               /*!< Associates Rx AGC Hybrid mode feature with a shared resource */
    /*** Please add Feature IDs which use shared resources above this line ***/
    ADRV9025_NUM_FEATURES,
    ADRV9025_FEATURE_INVALID
} adrv9025_FeatureID_e;

typedef struct adrv9025_SharedResourceLut
{
    adrv9025_SharedResourceType_e sharedResourceType;  /*!< Specifies the type of shared resource for a Look Up Table entry*/
    int32_t sharedResource;                                /*!< Specifies the shared resource value associated with the type for a Look Up Table entry*/
    adrv9025_SharedResourceID_e sharedResourceId;      /*!< Specifies the unique shared resource ID associated with the shared resource for a Look Up Table entry*/
} adrv9025_SharedResourceLut_t;

typedef struct adrv9025_SharedResourcePool
{
    adrv9025_FeatureID_e featureID;        /*!< Holds the feature ID of a shared resource pool member */
    uint8_t semaphoreCount;                /*!< Holds the semaphore count of a shared resource pool member */
} adrv9025_SharedResourcePool_t;

#ifdef __cplusplus
}
#endif

#endif /* ! _ADRV9025_SHARED_RESOURCE_MANAGER_TYPES_H_ */
