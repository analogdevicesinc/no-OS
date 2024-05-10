/**
* Copyright 2015 - 2022 Analog Devices Inc.
* Released under the ADRV904X API license, for more information
* see the "LICENSE.pdf" file in this zip file.
*/

/**
* \file adi_adrv904x_radioctrl.c
* \brief Contains CPU features related function implementation defined in
* adi_adrv904x_radioctrl.h
*
* ADRV904X API Version: 2.10.0.4
*/

#include <math.h>

#include "adi_adrv904x_radioctrl.h"
#include "adi_adrv904x_cals.h"
#include "adi_adrv904x_cpu.h"
#include "adi_adrv904x_tx.h"
#include "adi_adrv904x_core.h"
#include "adi_adrv904x_types.h"
#include "adi_adrv904x_version_types.h"

#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/bf/adrv904x_bf_pll_mem_map.h"
#include "../../private/include/adrv904x_cpu_scratch_registers.h"
#include "../../private/include/adrv904x_cpu.h"
#include "../../private/include/adrv904x_rx.h"
#include "../../private/include/adrv904x_tx.h"
#include "../../private/include/adrv904x_gpio.h"
#include "../../private/include/adrv904x_radioctrl.h"
#include "../../private/bf/adrv904x_bf_tx_funcs.h"
#include "../../private/bf/adrv904x_bf_streamproc.h"
#include "../../private/include/adrv904x_init.h"
#include "../../private/include/adrv904x_stream_proc_types.h"


#include "../../private/bf/adrv904x_bf_sequencer.h"
#include "adi_adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#include "../../private/include/adrv904x_dfe_cfr.h"
#include "../../private/include/adrv904x_dfe_cpu.h"
#include "../../private/include/adrv904x_dfe_svc_radio_ctrl_sequencer_t.h"
#include "../../private/include/adrv904x_dfe_svc_bbic_bridge_t.h"
#include "../../private/include/adrv904x_dfe_framework_cmd_t.h"
#include "../../private/include/adrv904x_cpu_memory.h"
#include "../../private/bf/adrv904x_bf_streamproc_channel.h"
#include "../../private/include/adrv904x_stream_id_types.h"

#define ADI_FILE    ADI_ADRV904X_FILE_PUBLIC_RADIOCTRL


/****************************************************************************
 * Helper functions
 ****************************************************************************
 */

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the size and offset of a single slice processor.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice data in stream_image.bin
* \param binaryImageSize Pointer to a short where the slice size in bytes will be placed
* \param binaryImageOffset Pointer to a short that will store the slice image offset 
*                           in the stream_image.bin file
*/
static void adrv904x_BinaryImageInfoGet(const uint8_t   binary[],
                                        uint32_t* const binaryImageSize,
                                        uint32_t* const binaryImageOffset)
{
    /* populating variables from binary array */
    *binaryImageSize = ((((uint32_t)binary[3U]) << 24U) | (((uint32_t)binary[2U]) << 16U) | (((uint32_t)binary[1U]) << 8U) | ((uint32_t)binary[0U]));
    *binaryImageOffset =  ((((uint32_t)binary[7U]) << 24U) | (((uint32_t)binary[6U]) << 16U) | (((uint32_t)binary[5U]) << 8U) | ((uint32_t)binary[4U]));
}

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the base address of a single slice.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice image in stream_image.bin
* \param word Pointer to a word where the slice base address will be stored
*/
static void adrv904x_BinaryParamsGetBaseAddr(const uint8_t binary[], uint32_t* const word)
{
    /* populating variables from binary array */
    *word = (((uint32_t)binary[3U]) << 24U) | (((uint32_t)binary[2U]) << 16U) | (((uint32_t)binary[1U]) << 8U) | (uint32_t)(binary[0U]);
}

/**
* \brief    Based off the structure of the stream_image.bin this function extracts
*           the number of streams and slice image size.
*
* \dep_begin
* \dep{device->common.devHalInfo}
* \dep_end
*
* \param binary Pointer to the start of slice image in stream_image.bin
* \param numberStreams Pointer to a byte that will store the number of streams for this slice
* \param imageSize Pointer to a short that will store the slice size in bytes
*/
static void adrv904x_BinaryParamsGetNumberStreamImageSize(  const uint8_t   binary[],
                                                            uint8_t* const  numberStreams,
                                                            uint32_t* const imageSize)
{
    *numberStreams = binary[0U];
    *imageSize = (((uint32_t)binary[3U]) << 8) | (uint32_t)(binary[2U]);
}

static adi_adrv904x_GpioPinSel_e  adrv904x_StreamGleanGpioNumberGet(uint32_t gpioSelectionArr[], adrv904x_StreamGpioFeatureSelection_e featureSelect)
{
    uint32_t gpioIdx = 0U;
    
    if (gpioSelectionArr == NULL)
    {
        return ADI_ADRV904X_GPIO_INVALID;
    }
    
    switch (featureSelect)
    {
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT0:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT1:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT2:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT3:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT4:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT5:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT6:
        case ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT7:

        case ADRV904X_STREAM_GPIO_ALARM_INPUT_0:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_1:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_2:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_3:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_4:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_5:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_6:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_7:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_8:
        case ADRV904X_STREAM_GPIO_ALARM_INPUT_9:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_0:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_1:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_2:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_3:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_4:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_5:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_6:
        case ADRV904X_STREAM_GPIO_PA_EN_OUT_7:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_0:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_1:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_2:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_3:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_4:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_5:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_6:
        case ADRV904X_STREAM_GPIO_LNA_EN_OUT_7:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_0:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_1:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_2:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_3:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_4:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_5:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_6:
        case ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_7:
        case ADRV904X_STREAM_GPIO_VSWR_DIR_ORX0:
        case ADRV904X_STREAM_GPIO_VSWR_DIR_ORX1:
        case ADRV904X_STREAM_GPIO_VSWR_DIR_ORX_CMN:
        case ADRV904X_STREAM_GPIO_SBET_LATCH_DPD_MODEL_INDEX:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT0:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT1:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT2:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT3:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT4:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT5:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT0:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT1:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT2:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT3:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT4:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT5:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT0:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT1:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT2:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT3:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT4:
        case ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT5:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_0:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_1:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_2:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_3:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_4:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_5:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_6:
        case ADRV904X_STREAM_GPIO_DTX_INPUT_7:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX0:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX1:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX2:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX3:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX4:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX5:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX6:
        case ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX7:
        case ADRV904X_STREAM_GPIO_TDD_SW:
        case ADRV904X_STREAM_GPIO_PREDRIVE_EN:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN0:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN1:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN2:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN3:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN4:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN5:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN6:
        case ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN7:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_0:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_1:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_2:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_3:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_4:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_5:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_6:
        case ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_7:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_0:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_1:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_2:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_3:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_4:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_5:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_6:
        case ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_7:
            break;

        case ADRV904X_STREAM_GPIO_TX_ANTENNA_CAL:
        case ADRV904X_STREAM_GPIO_RX_ANTENNA_CAL:
        case ADRV904X_STREAM_GPIO_TX_PAP_FOR_EXT_LO0_UNLOCK:
        case ADRV904X_STREAM_GPIO_TX_PAP_FOR_EXT_LO1_UNLOCK:
            /* Fall Through */

        default:
            return ADI_ADRV904X_GPIO_INVALID;
            break;
    }

    for (gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_COUNT; gpioIdx++)
    {
        if (gpioSelectionArr[gpioIdx] == (uint32_t)featureSelect)
        {
            return (adi_adrv904x_GpioPinSel_e)gpioIdx;
        }
    }
    
    return ADI_ADRV904X_GPIO_INVALID;
}


static adi_adrv904x_GpioAnaPinSel_e  adrv904x_StreamGleanAnaGpioNumberGet(uint32_t gpioSelectionArr[],
                                                                          adrv904x_StreamAnaGpioFeatureSelection_e featureSelect)
{
    uint32_t gpioIdx = 0U;
    
    if (gpioSelectionArr == NULL)
    {
        return ADI_ADRV904X_GPIO_ANA_INVALID;
    }
    
    switch (featureSelect)
    {
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_0:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_1:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_2:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_3:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_4:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_5:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_6:
        case ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_7:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_0:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_1:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_2:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_3:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_4:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_5:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_6:
        case ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_7:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_0:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_1:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_2:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_3:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_4:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_5:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_6:
        case ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_7:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT0:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT1:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT2:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT3:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT4:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT5:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT0:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT1:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT2:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT3:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT4:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT5:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT0:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT1:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT2:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT3:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT4:
        case ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT5:
        case ADRV904X_STREAM_ANALOG_GPIO_TDD_SW:
        case ADRV904X_STREAM_ANALOG_GPIO_PREDRIVE_EN:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN0:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN1:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN2:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN3:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN4:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN5:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN6:
        case ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN7:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_0:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_1:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_2:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_3:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_4:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_5:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_6:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_7:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_0:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_1:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_2:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_3:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_4:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_5:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_6:
        case ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_7:
            break;
        default:
            return ADI_ADRV904X_GPIO_ANA_INVALID;
            break;
    }

    for (gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_COUNT; gpioIdx++)
    {
        if (gpioSelectionArr[gpioIdx] == (uint32_t)featureSelect)
        {
            return (adi_adrv904x_GpioAnaPinSel_e)gpioIdx;
        }
    }
    return ADI_ADRV904X_GPIO_ANA_INVALID;
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamImageWrite(adi_adrv904x_Device_t* const    device,
                                                               uint32_t                        byteOffset,
                                                               const uint8_t                   binary[],
                                                               uint32_t                        byteCount)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t binaryImageSize = 0U;
    uint32_t binaryImageOffset = 0U;
    uint32_t i = 0U;
    uint32_t addr = 0U;
    uint32_t mask = 0U;
    uint32_t word1 = 0U;
    uint32_t binOffset = 0U;
    uint32_t  streamCtl = 0U;
    uint8_t  byte1 = 0U;
    uint32_t imageSize = 0U;
    uint32_t imageOffset = 0U;
    uint32_t streamCtlAddr[] = {
        ADRV904X_ADDR_MAIN_STREAM_CTL,
        //todo- this value needs to be update when the kfa stream_reset bit is enabled
        ADRV904X_ADDR_KFA_STREAM_CTL,
        ADRV904X_ADDR_TX0_STREAM_CTL,
        ADRV904X_ADDR_TX1_STREAM_CTL,
        ADRV904X_ADDR_TX2_STREAM_CTL,
        ADRV904X_ADDR_TX3_STREAM_CTL,
        ADRV904X_ADDR_TX4_STREAM_CTL,
        ADRV904X_ADDR_TX5_STREAM_CTL,
        ADRV904X_ADDR_TX6_STREAM_CTL,
        ADRV904X_ADDR_TX7_STREAM_CTL,
        ADRV904X_ADDR_RX0_STREAM_CTL,
        ADRV904X_ADDR_RX1_STREAM_CTL,
        ADRV904X_ADDR_RX2_STREAM_CTL,
        ADRV904X_ADDR_RX3_STREAM_CTL,
        ADRV904X_ADDR_RX4_STREAM_CTL,
        ADRV904X_ADDR_RX5_STREAM_CTL,
        ADRV904X_ADDR_RX6_STREAM_CTL,
        ADRV904X_ADDR_RX7_STREAM_CTL,
        ADRV904X_ADDR_ORX0_STREAM_CTL,
        ADRV904X_ADDR_ORX1_STREAM_CTL
    };
    
    /* Default value assigned by hardware */
    uint32_t coreSliceControlDefaultVal = 0x80U;
    
    /* Number of bytes in the stream_image.bin header that contains
     * all the meta-data required to parse stream_image.bin
     * This also indicates the byte address where the core processor
     * starts.
     */
    static const uint32_t STREAM_METADATA_HEADER_SIZE_START_BYTE                            = 24U;
    static const uint32_t STREAM_METADATA_HEADER_SIZE_DATA_LENGTH                           = 4U;
    static const uint32_t STREAM_IMAGE_SLICE_INFO_START_BYTE                                = 28U;
    static const uint32_t STREAM_PROCESSOR_META_DATA_SIZE_BYTES                             = 8U;
    static const uint32_t STREAM_IMAGE_GPIO00_START_BYTE                                    = 190U;
    static const uint32_t STREAM_IMAGE_TXORXMAPPINGMODE_START_BYTE                          = 287U;
    static const uint32_t STREAM_IMAGE_TXORXOBSERVABILITY_START_BYTE                        = 289U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX0_00_START_BYTE             = 291U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX1_00_START_BYTE             = 355U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXATTENENABLE_START_BYTE     = 419U;
    static const uint32_t STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXNCOENABLE_START_BYTE       = 420U;


    static const uint32_t STREAM_IMAGE_VSWRDIRORX0GPIOPOLARITY_START_BYTE                   = 437U;
    static const uint32_t STREAM_IMAGE_VSWRDIRORX1GPIOPOLARITY_START_BYTE                   = 438U;
    static const uint32_t STREAM_IMAGE_VSWRDIRCOMMONGPIOPOLARITY_START_BYTE                 = 439U;
    static const uint32_t STREAM_IMAGE_ANA_GPIO00_START_BYTE                                = 440U;
    static const uint32_t STREAM_IMAGE_EA_PA_EN_PIN_MODE_START_BYTE                         = 504U;
    static const uint32_t STREAM_IMAGE_EA_LNA_EN_PIN_MODE_START_BYTE                        = 508U;
    static const uint32_t STREAM_IMAGE_EA_OC_FUSE_EN_PIN_MODE_START_BYTE                    = 508U;
    static const uint32_t STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE                   = 512U;
    static const uint32_t STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE                  = 563U;
    static const uint32_t STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE                  = 579U;
    static const uint32_t STREAM_IMAGE_DTX_EVENT_WAIT_TIME_NS                               = 596U;

    uint32_t FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES = 0U;

    uint32_t gpioAssignments[ADI_ADRV904X_GPIO_COUNT] = { 0U };

    uint32_t anaGpioAssignments[ADI_ADRV904X_GPIO_ANALOG_COUNT] = { 0U };
    adi_adrv904x_GpioPinSel_e tmpDig = ADI_ADRV904X_GPIO_INVALID;
    adi_adrv904x_GpioAnaPinSel_e tmpAna = ADI_ADRV904X_GPIO_ANA_INVALID;
    adi_adrv904x_GpioGenericPinSel_e tmpGen = ADI_ADRV904X_GPIO_GENERIC_INVALID;
    int bitIdx = 0U;
    uint32_t gpioIdx = 0U;
    uint32_t idx = 0U;
    
    
    /* CPU stream download order: main, kfa, tx0/1/2/3/4/5/6/7, rx0/1/2/3/4/5/6/7, orx0/1 */
    static const uint32_t streamChannel[] = {
        0xFFFFFFFFU,
        0x0U,
        ADI_ADRV904X_TX0 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX1 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX2 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX3 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX4 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX5 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX6 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_TX7 << ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET,
        ADI_ADRV904X_RX0,
        ADI_ADRV904X_RX1,
        ADI_ADRV904X_RX2,
        ADI_ADRV904X_RX3,
        ADI_ADRV904X_RX4,
        ADI_ADRV904X_RX5,
        ADI_ADRV904X_RX6,
        ADI_ADRV904X_RX7,
        ADI_ADRV904X_ORX0,
        ADI_ADRV904X_ORX1
    };
    
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, binary, cleanup);

    if ((byteCount == 0U) ||
        ((byteCount % 4U) > 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                byteCount,
                                "Invalid byteCount. Must be multiple of 4.");
        goto cleanup;
    }

    if ((byteOffset % 4U) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                byteCount,
                                "Invalid byteOffset. Must be multiple of 4.");
        goto cleanup;
    }

    /* When byteOffset is zero that indicates the first time this function.
     * has been called for a particular stream_image.bin so the header info
     * must be extracted. We need all the information extracted in a single
     * call which is why min chunk size is the same as FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES.
     * */
    if (byteOffset == 0U)
    {
        /* Check that enough data was passed in to read the stream header size from the stream */
        if (byteCount < STREAM_METADATA_HEADER_SIZE_START_BYTE + STREAM_METADATA_HEADER_SIZE_DATA_LENGTH)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    byteCount,
                                    "Must pass in chunk size greater than current stream image header.");
            goto cleanup;
        }
        
        /* Glean stream header size from the binary itself */
        FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES = adrv904x_CpuIntFromBytesGet(binary + STREAM_METADATA_HEADER_SIZE_START_BYTE, 
                                                                           STREAM_METADATA_HEADER_SIZE_DATA_LENGTH);
        
        /* Verify that enough data was passed in to read the entire stream header */
        if (byteCount < FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    byteCount,
                                    "Must pass in chunk size greater than current stream image header.");
            goto cleanup;
        }

#if ADI_ADRV904X_API_VERSION_RANGE_CHECK > 0
        {
            adi_adrv904x_Version_t version = { 0U, 0U, 0U, 0U };
            adi_adrv904x_Version_t minVersion = { 0U, 0U, 0U, 0U };
            static const uint32_t STREAM_IMAGE_VERSION_ID_START_BYTE = 8U;

                        version.majorVer        = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 0U, 4U);
            version.minorVer        = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 4U, 4U);
            version.maintenanceVer  = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 8U, 4U);
            version.buildVer        = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VERSION_ID_START_BYTE + 12U, 4U);

            recoveryAction = adi_adrv904x_ApiVersionGet(device, &minVersion);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Binary Version Get Issue");
                goto cleanup;
            }

            recoveryAction = adrv904x_ApiVersionRangeCheck(device, &version, &minVersion, &minVersion);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Stream Binary Range Check Issue");
                goto cleanup;
            }
        }
#endif

        /* Glean the 24 GPIO selections made in the Stream Binary */
        for (gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_COUNT; gpioIdx++)
        {
            gpioAssignments[gpioIdx] = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_GPIO00_START_BYTE + (gpioIdx * 4U), 4U);

            /* If stream feature is a basic stream input that requires no special handling, add it to generic Stream triggers list */
            switch (gpioAssignments[gpioIdx])
            {
                case (ADRV904X_STREAM_GPIO_TX_ANTENNA_CAL):             /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_RX_ANTENNA_CAL):             /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_TX_PAP_FOR_EXT_LO0_UNLOCK):  /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_TX_PAP_FOR_EXT_LO1_UNLOCK):  /* Fallthrough */

                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_0):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_1):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_2):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_3):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_4):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_5):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_6):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_7):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_8):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ALARM_INPUT_9):              /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_RS_PATTERN_SWITCHING):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX0):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX1):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX2):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX3):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX4):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX5):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX6):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX7):     /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_SBET_LATCH_DPD_MODEL_INDEX): /* Fallthrough */
                    device->devStateInfo.streamGpioMapping.streamGpInput[gpioIdx] = (adi_adrv904x_GpioPinSel_e)gpioIdx;
                    break;


                // The RS_SSB_SYNC_PIN input to ADRV904X stream processor GP input is enabled dynamically by API which starts RS 
                case (ADRV904X_STREAM_GPIO_RS_SSB_SYNC_PIN):            /* Fallthrough */

                // Default: Do Nothing
                default:
                    device->devStateInfo.streamGpioMapping.streamGpInput[gpioIdx] = ADI_ADRV904X_GPIO_INVALID;
                    break;
            }


            switch (gpioAssignments[gpioIdx])
            {
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN0):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN1):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN2):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN3):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN4):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN5):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN6):       /* Fallthrough */
                case (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN7):       /* Fallthrough */
                        
                    device->devStateInfo.streamGpioMapping.streamGpDigOutput[gpioIdx] = (adi_adrv904x_GpioPinSel_e)gpioIdx;
                    break;

                // Default: Do Nothing
                default:
                    device->devStateInfo.streamGpioMapping.streamGpDigOutput[gpioIdx] = ADI_ADRV904X_GPIO_INVALID;
                    break;
            }
        }


        for (gpioIdx = 0U; gpioIdx < ADI_ADRV904X_GPIO_ANALOG_COUNT; gpioIdx++)
        {
            anaGpioAssignments[gpioIdx] = adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_ANA_GPIO00_START_BYTE + (gpioIdx * 4U), 4U);

            switch (anaGpioAssignments[gpioIdx])
            {
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN0):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN1):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN2):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN3):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN4):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN5):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN6):       /* Fallthrough */
                case (ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN7):       /* Fallthrough */
                        
                    device->devStateInfo.streamGpioMapping.streamGpAnaOutput[gpioIdx] = (adi_adrv904x_GpioAnaPinSel_e)gpioIdx;
                    break;

                // Default: Do Nothing
                default :
                    device->devStateInfo.streamGpioMapping.streamGpAnaOutput[gpioIdx] = ADI_ADRV904X_GPIO_ANA_INVALID;
                    break;
            }
        }

        /* Update device handle Tx Orx Mapping config */
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT0);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT1);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT2);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT3);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT4);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT5);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT6);
        device->devStateInfo.txToOrxMappingConfig.gpioSelect[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TX_TO_ORX_MAPPING_BIT7);


        device->devStateInfo.alarmGpioConfig.alarmInputGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_0);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_1);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_2);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_3);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_4);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_5);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_6);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_7);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[8U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_8);
        device->devStateInfo.alarmGpioConfig.alarmInputGpio[9U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_ALARM_INPUT_9);

        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_LNA_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PA_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutDigGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_OC_FUSE_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_0);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_1);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_2);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_3);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_4);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_5);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_6);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutDigGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP0_OUT_7);

        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_0);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_1);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_2);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_3);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_4);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_5);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_6);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutDigGpio[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_AUX_GRP1_OUT_7);

        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[0U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[1U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[2U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[3U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[4U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[5U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[6U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[7U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_LNA_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[0U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[1U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[2U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[3U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[4U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[5U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[6U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[7U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_PA_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[0U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_0);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[1U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_1);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[2U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_2);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[3U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_3);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[4U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_4);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[5U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_5);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[6U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_6);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutAnaGpio[7U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANA_GPIO_OC_FUSE_EN_OUT_7);

        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[0U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_0);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[1U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_1);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[2U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_2);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[3U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_3);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[4U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_4);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[5U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_5);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[6U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_6);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutAnaGpio[7U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP0_OUT_7);

        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[0U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_0);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[1U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_1);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[2U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_2);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[3U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_3);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[4U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_4);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[5U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_5);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[6U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_6);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutAnaGpio[7U] = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_AUX_GRP1_OUT_7);

        /* Update device handle alarm input config */
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[0U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 0, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[1U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 2, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[2U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 4, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[3U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 6, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[4U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 8, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[5U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 10, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[6U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 12, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[7U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 14, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[8U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 16, 2U);
        device->devStateInfo.alarmGpioConfig.alarmInputGpioPinMode[9U] =
                            (adi_adrv904x_AlarmInputGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_ALARM_INPUT_PIN_MODE_START_BYTE + 18, 2U);

        /* Update device handle alarm output config */
        device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_PA_EN_PIN_MODE_START_BYTE, 4U);
        device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_LNA_EN_PIN_MODE_START_BYTE, 4U);
        device->devStateInfo.alarmGpioConfig.ocFuseEnOutGpioPinMode =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_OC_FUSE_EN_PIN_MODE_START_BYTE, 4U);

        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[0U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 0, 2U);  /* Check this >?>?>?*/
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[1U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 2, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[2U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 4, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[3U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 6, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[4U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 8, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[5U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 10, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[6U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 12, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp0OutGpioPinMode[7U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP0_OUT_PIN_MODE_START_BYTE + 14, 2U);

        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[0U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 0, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[1U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 2, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[2U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 4, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[3U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 6, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[4U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 8, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[5U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 10, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[6U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 12, 2U);
        device->devStateInfo.alarmGpioConfig.auxGrp1OutGpioPinMode[7U] =
                            (adi_adrv904x_OutGpioPinModes_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_EA_AUX_GRP1_OUT_PIN_MODE_START_BYTE + 14, 2U);

        /* Update device handle DTX input config */
        device->devStateInfo.dtxInputMapping.dtxInput[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_0);
        device->devStateInfo.dtxInputMapping.dtxInput[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_1);
        device->devStateInfo.dtxInputMapping.dtxInput[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_2);
        device->devStateInfo.dtxInputMapping.dtxInput[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_3);
        device->devStateInfo.dtxInputMapping.dtxInput[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_4);
        device->devStateInfo.dtxInputMapping.dtxInput[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_5);
        device->devStateInfo.dtxInputMapping.dtxInput[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_6);
        device->devStateInfo.dtxInputMapping.dtxInput[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_DTX_INPUT_7);
        device->devStateInfo.dtxInputMapping.dtxEventWaitTimeNs = (uint32_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_DTX_EVENT_WAIT_TIME_NS, 4U);

        /* Update device handle GPIO Model Switch input config */
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[0U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX0);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[1U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX1);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[2U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX2);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[3U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX3);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[4U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX4);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[5U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX5);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[6U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX6);
        device->devStateInfo.modelSwitchInputMapping.modelSwitchInput[7U] = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_MODEL_SWITCH_INPUT_TX7);

        /* Update GPIO TDD SW to either Digital or Analog */
        device->devStateInfo.digGpioTddSw = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_TDD_SW);
        device->devStateInfo.anaGpioTddSw = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_TDD_SW);

        /* Update device handle SBET Latch Power index input config */
        device->devStateInfo.sbetLatchModelIndex = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_SBET_LATCH_DPD_MODEL_INDEX);
        
        /* Update GPIO Predrive to either Digital or Analog */
        device->devStateInfo.digGpioPredrive = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_PREDRIVE_EN);
        device->devStateInfo.anaGpioPredrive = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, ADRV904X_STREAM_ANALOG_GPIO_PREDRIVE_EN);
        
        /* Decipher DFE Ctrl Mapping GPIO selections into final Cfg value */
        for (bitIdx = 0U; bitIdx < ADI_ADRV904X_DFE_CTRL_TX_TO_ORX_MAPPING_GPIO_MAX; bitIdx++)
        {
            adrv904x_StreamGpioFeatureSelection_e    featureCmnDig  = ADRV904X_STREAM_GPIO_UNUSED;
            adrv904x_StreamGpioFeatureSelection_e    featureOrx0Dig = ADRV904X_STREAM_GPIO_UNUSED;
            adrv904x_StreamGpioFeatureSelection_e    featureOrx1Dig = ADRV904X_STREAM_GPIO_UNUSED;

            adrv904x_StreamAnaGpioFeatureSelection_e featureCmnAna  = ADRV904X_STREAM_ANA_GPIO_UNUSED;
            adrv904x_StreamAnaGpioFeatureSelection_e featureOrx0Ana = ADRV904X_STREAM_ANA_GPIO_UNUSED;
            adrv904x_StreamAnaGpioFeatureSelection_e featureOrx1Ana = ADRV904X_STREAM_ANA_GPIO_UNUSED;

            if (bitIdx < 4)
            {
                featureCmnDig  = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT0          + bitIdx);
                featureOrx0Dig = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT0             + bitIdx);
                featureOrx1Dig = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT0             + bitIdx);

                featureCmnAna  = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT0   + bitIdx);
                featureOrx0Ana = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT0      + bitIdx);
                featureOrx1Ana = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT0      + bitIdx);
            }
            else
            {
                featureCmnDig  = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT4          + (bitIdx - 4));
                featureOrx0Dig = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX0_MAP_BIT4             + (bitIdx - 4));
                featureOrx1Dig = (adrv904x_StreamGpioFeatureSelection_e)(ADRV904X_STREAM_GPIO_DFE_TX_TO_ORX1_MAP_BIT4             + (bitIdx - 4));

                featureCmnAna  = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX_CMN_MAP_BIT4   + (bitIdx - 4));
                featureOrx0Ana = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX0_MAP_BIT4      + (bitIdx - 4));
                featureOrx1Ana = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_DFE_TX_TO_ORX1_MAP_BIT4      + (bitIdx - 4));
            }

            /* Look for CMN mapping bit for this bitIdx first */
            tmpDig = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, featureCmnDig);
            tmpAna = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, featureCmnAna);
            tmpGen = 
                (tmpDig != ADI_ADRV904X_GPIO_INVALID)       ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_DIG_START + tmpDig) :
                (tmpAna != ADI_ADRV904X_GPIO_ANA_INVALID)   ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_ANA_START + tmpAna) : 
                (ADI_ADRV904X_GPIO_GENERIC_INVALID);
            
            device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx0[bitIdx] = tmpGen;
            device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx1[bitIdx] = tmpGen;
            
            /* If we didn't find a valid CMN mapping bit selected for bitIdx, then look for orx-specific mapping bits */
            if (device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx0[bitIdx] == ADI_ADRV904X_GPIO_GENERIC_INVALID)
            {
                tmpDig = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, featureOrx0Dig);
                tmpAna = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, featureOrx0Ana);
                tmpGen = 
                    (tmpDig != ADI_ADRV904X_GPIO_INVALID)       ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_DIG_START + tmpDig) :
                    (tmpAna != ADI_ADRV904X_GPIO_ANA_INVALID)   ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_ANA_START + tmpAna) : 
                    (ADI_ADRV904X_GPIO_GENERIC_INVALID);
                
                device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx0[bitIdx] = tmpGen;
            }
            
            if (device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx1[bitIdx] == ADI_ADRV904X_GPIO_GENERIC_INVALID)
            {
                tmpDig = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, featureOrx1Dig);
                tmpAna = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, featureOrx1Ana);
                tmpGen = 
                    (tmpDig != ADI_ADRV904X_GPIO_INVALID)       ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_DIG_START + tmpDig) :
                    (tmpAna != ADI_ADRV904X_GPIO_ANA_INVALID)   ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_ANA_START + tmpAna) : 
                    (ADI_ADRV904X_GPIO_GENERIC_INVALID);
                    
                device->devStateInfo.dfeCtrlTxToOrxMappingConfig.gpioSelectOrx1[bitIdx] = tmpGen;
            }
        }
        
        /* Decipher Stream Output Antenna Cal GPIO selections into final Cfg value */
        for (bitIdx = 0U; bitIdx < ADI_ADRV904X_MAX_STREAM_GPIO_ANTENNA_CAL_OUT_PINS; bitIdx++)
        {
            /* Check for Digital GPIO selection. If not, check for Analog GPIO selection. If neither are found, default to INVALID (which means none) */
            adrv904x_StreamGpioFeatureSelection_e    featureDig  = (adrv904x_StreamGpioFeatureSelection_e)   (ADRV904X_STREAM_GPIO_ANTENNA_CAL_OUT_PIN0          + bitIdx);
            adrv904x_StreamAnaGpioFeatureSelection_e featureAna  = (adrv904x_StreamAnaGpioFeatureSelection_e)(ADRV904X_STREAM_ANALOG_GPIO_ANTENNA_CAL_OUT_PIN0   + bitIdx);

            tmpDig = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, featureDig);
            tmpAna = adrv904x_StreamGleanAnaGpioNumberGet(anaGpioAssignments, featureAna);
            tmpGen = 
                (tmpDig != ADI_ADRV904X_GPIO_INVALID)       ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_DIG_START + tmpDig) :
                (tmpAna != ADI_ADRV904X_GPIO_ANA_INVALID)   ? (adi_adrv904x_GpioGenericPinSel_e)(ADI_ADRV904X_GPIO_GENERIC_ANA_START + tmpAna) : 
                (ADI_ADRV904X_GPIO_GENERIC_INVALID);
            device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] = tmpGen;
        }

        device->devStateInfo.txToOrxMappingConfig.txObservability = (uint16_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXORXOBSERVABILITY_START_BYTE, 2U);

        device->devStateInfo.txToOrxMappingConfig.mode = (adi_adrv904x_TxToOrxMappingMode_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXORXMAPPINGMODE_START_BYTE, 1U);

        for (idx = 0U; idx < ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE; idx++)
        {
            device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx] = (adi_adrv904x_TxToOrxMappingPinTable_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX0_00_START_BYTE + (idx * 4U), 4U);

            device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx] = (adi_adrv904x_TxToOrxMappingPinTable_e)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINGPINTABLEORX1_00_START_BYTE + (idx * 4U), 4U);
        }
        device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxAttenEnable = (uint8_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXATTENENABLE_START_BYTE, 1U);

        device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxNcoEnable = (uint8_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_TXTOORXMAPPINAUTOSWITCHORXNCOENABLE_START_BYTE, 1U);


        device->devStateInfo.txToOrxMappingConfig.vswrDirOrx0Gpio   = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_VSWR_DIR_ORX0);
        device->devStateInfo.txToOrxMappingConfig.vswrDirOrx1Gpio   = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_VSWR_DIR_ORX1);
        device->devStateInfo.txToOrxMappingConfig.vswrDirCommonGpio = adrv904x_StreamGleanGpioNumberGet(gpioAssignments, ADRV904X_STREAM_GPIO_VSWR_DIR_ORX_CMN);

        device->devStateInfo.txToOrxMappingConfig.vswrDirOrx0GpioPolarity   = (uint8_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VSWRDIRORX0GPIOPOLARITY_START_BYTE, 1U);
        device->devStateInfo.txToOrxMappingConfig.vswrDirOrx1GpioPolarity   = (uint8_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VSWRDIRORX1GPIOPOLARITY_START_BYTE, 1U);
        device->devStateInfo.txToOrxMappingConfig.vswrDirCommonGpioPolarity = (uint8_t)adrv904x_CpuIntFromBytesGet(binary + STREAM_IMAGE_VSWRDIRCOMMONGPIOPOLARITY_START_BYTE, 1U);
        
        /* Duplicate the common values from RCI mode Mapping Config into DFE CTRL mode Mapping Config */
        device->devStateInfo.dfeCtrlTxToOrxMappingConfig.txObservability            = device->devStateInfo.txToOrxMappingConfig.txObservability;
        device->devStateInfo.dfeCtrlTxToOrxMappingConfig.mode                       = device->devStateInfo.txToOrxMappingConfig.mode;
        device->devStateInfo.dfeCtrlTxToOrxMappingConfig.autoSwitchOrxAttenEnable   = device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxAttenEnable;
        device->devStateInfo.dfeCtrlTxToOrxMappingConfig.autoSwitchOrxNcoEnable     = device->devStateInfo.txToOrxMappingConfig.autoSwitchOrxNcoEnable;
        for (idx = 0U; idx < ADI_ADRV904X_TX_TO_ORX_MAPPING_PIN_TABLE_SIZE; idx++)
        {
            device->devStateInfo.dfeCtrlTxToOrxMappingConfig.pinTableOrx0[idx]      = device->devStateInfo.txToOrxMappingConfig.pinTableOrx0[idx];
            device->devStateInfo.dfeCtrlTxToOrxMappingConfig.pinTableOrx1[idx]      = device->devStateInfo.txToOrxMappingConfig.pinTableOrx1[idx];
        }

        for (i = 0; i < ADI_ADRV904X_STREAM_MAX; ++i)
        {
            adrv904x_BinaryImageInfoGet(&binary[STREAM_IMAGE_SLICE_INFO_START_BYTE + i * STREAM_PROCESSOR_META_DATA_SIZE_BYTES], &binaryImageSize, &binaryImageOffset);
            device->devStateInfo.chunkStreamImageSize[i] = binaryImageSize;
            device->devStateInfo.chunkStreamImageOffset[i] = binaryImageOffset;
        }

        /* Variables used to track the status of stream_image.bin parsing
         * across multiple calls to this function. 
         * */
        device->devStateInfo.currentStreamImageIndex = 0U;
        device->devStateInfo.currentStreamBinBaseAddr = 0U;
        device->devStateInfo.currentStreamBaseAddr = 0U;
        device->devStateInfo.currentStreamImageSize = 0U;
        device->devStateInfo.currentStreamNumberStreams = 0U;
    }

    for (i = device->devStateInfo.currentStreamImageIndex; i < ADI_ADRV904X_STREAM_MAX; ++i)
    {
        imageOffset = device->devStateInfo.chunkStreamImageOffset[i];
        
        /* NOTE: the i == 0 if-statements are for the most part required because core
         * slice control registers are in Direct SPI address range instead of AHB memory. 
         * */
        
        /* The core processor binary header starts after the stream_image.bin header.
         * But what gets written to the core AHB address includes the stream_image.bin
         * header. */
        if (i == 0U)
        {
            imageOffset = FINAL_STREAM_IMAGE_HEADER_SIZE_BYTES;
        }
        
        /* The if-statements below are to see if the current chunk of stream_image.bin
         * includes certain necessary information for a particular slice, e.g. the binary
         * base address which is the first word of a slice binary image. */
        if ((byteOffset <= imageOffset) &&
            ((byteOffset + byteCount) >= imageOffset + 4U))
        {
            binOffset = imageOffset - byteOffset;
            adrv904x_BinaryParamsGetBaseAddr(&binary[binOffset], &word1);
            device->devStateInfo.currentStreamBinBaseAddr = word1;
            device->devStateInfo.currentStreamImageSize = device->devStateInfo.chunkStreamImageSize[i];
        }

        if ((byteOffset <= imageOffset + 4U) &&
            ((byteOffset + byteCount) >= imageOffset + 8U))
        {
            binOffset = imageOffset + 4U - byteOffset;
            adrv904x_BinaryParamsGetBaseAddr(&binary[binOffset], &word1);
            device->devStateInfo.currentStreamBaseAddr = word1;
        }

        if ((byteOffset <= imageOffset + 8U) &&
            ((byteOffset + byteCount) >= imageOffset + 12U))
        {
            binOffset = imageOffset + 8U - byteOffset;
            adrv904x_BinaryParamsGetNumberStreamImageSize(&binary[binOffset], &byte1, &word1);
            device->devStateInfo.currentStreamNumberStreams = byte1;
        }
        
        /* If these tracking variables have been set for the current slice that means we are in a state
         * where we can write the chunk to ARM memory.
         * */
        if ((device->devStateInfo.currentStreamBinBaseAddr > 0U) &&
            (device->devStateInfo.currentStreamImageSize > 0U))
        {
            addr = streamCtlAddr[i];

            /* This if-statement will only be true the first time we have encounter a slice image because
             * currentStreamImageSize gets decremented on each function call, so we can use this as a way
             * to initialize slice startup items, e.g. putting the slice into reset while it's image is getting
             * written. */
            if (device->devStateInfo.currentStreamImageSize == device->devStateInfo.chunkStreamImageSize[i])
            {
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0U)
                {
                    //todo: this assumes kfa stream_reset is in spi-only re.g.s
                    if ((i == 0U) || (i == 1U))
                    {
                        streamCtl = coreSliceControlDefaultVal | (1U << ADRV904X_STREAM_RESET_BIT);
                        mask = 0xFFU;
                    }
                    else
                    {
                        /* Default for Rx stream_config_0 is zero*/
                        streamCtl = (1U << ADRV904X_RX_STREAM_RESET_BIT);
                        mask = 0xFFFFFFFFU;
                    }
                    recoveryAction = adi_adrv904x_Register32Write(device, NULL, addr, streamCtl, mask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl");
                        goto cleanup;
                    }
                }
            }

            /* This if statement is in the case a byte chunk has a boundary between two slice images so we only write
             * the remaining bytes of the current slice. 
             * */
            if (byteCount >= device->devStateInfo.currentStreamImageSize)
            {
                imageSize = device->devStateInfo.currentStreamImageSize;
            }
            else
            {
                imageSize = byteCount;
            }

            /* Load the stream image to memory, starting at the memory address specified by Stream_binary_base
             * if the corresponding channel has been initialized. 
             * */
            if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0)
            {
                            recoveryAction = adi_adrv904x_RegistersByteWrite(device,
                                                                 NULL,
                                                                 device->devStateInfo.currentStreamBinBaseAddr,
                                                                 binary,
                                                                 imageSize);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write stream binary");
                    goto cleanup;
                }
                        }
            else
            {
                //stream is disabled but no need to report an error
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }

            byteCount -= imageSize;
            byteOffset += imageSize;
            binary += imageSize;
            device->devStateInfo.currentStreamImageSize -= imageSize;
            device->devStateInfo.currentStreamBinBaseAddr += imageSize;

            /* After an entire slice image has been written we need to set the corresponding stream control register. */
            if (device->devStateInfo.currentStreamImageSize == 0U)
            {
                
                if ((device->devStateInfo.initializedChannels & streamChannel[i]) > 0U)
                {    
                    //todo: this assumes kfa stream_reset is in spi-only regs
                    if((i == 0U) || (i == 1U))
                    {
                        streamCtl = coreSliceControlDefaultVal;
                        mask = 0xFFU;
                        
                        /* Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base. */
                        recoveryAction = adi_adrv904x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV904X_STREAM_BASE_BYTE0_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamBaseAddr),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Lower Byte");
                            goto cleanup;
                        }

                        recoveryAction = adi_adrv904x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV904X_STREAM_BASE_BYTE1_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamBaseAddr >> 8U),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Upper Byte");
                            goto cleanup;
                        }

                        /* Populate register  (last_stream_num)with the No_of_streams value. */
                        recoveryAction = adi_adrv904x_Register32Write(device,
                                                                      NULL,
                                                                      addr + ADRV904X_LAST_STREAM_NUMBER_REG_OFFSET,
                                                                      (uint8_t)(device->devStateInfo.currentStreamNumberStreams - 1U),
                                                                      mask);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl - Number of Streams");
                            goto cleanup;
                        }
                        
                    }
                    else 
                    {
                        mask = 0xFFFFFFFFU;
                        streamCtl = 0U;
                        
                        /* Populate registers(stream_base_byte0) and (stream_base_byte1)with the lower 16 bits of the Stream_base. */
                        streamCtl |= (device->devStateInfo.currentStreamBaseAddr & 0xFFU) << ADRV904X_RX_STREAM_BASE_BYTE0_BIT;
                        streamCtl |= ((device->devStateInfo.currentStreamBaseAddr >> 8U) & 0xFFU) << ADRV904X_RX_STREAM_BASE_BYTE1_BIT;

                        /* Populate register  (last_stream_num)with the No_of_streams value. */
                        streamCtl |= ((device->devStateInfo.currentStreamNumberStreams - 1U) & 0xFFU) << ADRV904X_RX_LAST_STREAM_NUMBER_BIT;
                        
                        /* Stream config 1 holds the base address & last stream number */
                        recoveryAction = adi_adrv904x_Register32Write(device, NULL, addr + 4U, streamCtl, mask);
                        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                        {
                            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl 1");
                            goto cleanup;
                        }
                        
                        /* Just clear reset bit on other channel processors */
                        streamCtl = 0U;
                        
                    }

                    recoveryAction = adi_adrv904x_Register32Write(device, NULL, addr, streamCtl, mask);
                    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                    {
                        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to set Stream Ctrl");
                        goto cleanup;
                    }
                }

                ++device->devStateInfo.currentStreamImageIndex;
                device->devStateInfo.currentStreamBinBaseAddr = 0U;
                device->devStateInfo.currentStreamBaseAddr = 0U;
                device->devStateInfo.currentStreamImageSize = 0U;
                device->devStateInfo.currentStreamNumberStreams = 0U;
            }
            else
            {
                //stream is disabled but no need to report an error
                recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            }
        }
        else
        {
            break;
        }

        if (byteCount == 0U)
        {
            break;
        }
    }

    if (device->devStateInfo.currentStreamImageIndex == ADI_ADRV904X_STREAM_MAX)
    {
        device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_STREAMLOADED);
        recoveryAction  = ADI_ADRV904X_ERR_ACT_NONE;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxEnableSet(adi_adrv904x_Device_t* const   device,
                                                            const uint32_t                 orxChannelMask,
                                                            const uint32_t                 orxChannelEnable,
                                                            const uint32_t                 rxChannelMask,
                                                            const uint32_t                 rxChannelEnable,
                                                            const uint32_t                 txChannelMask,
                                                            const uint32_t                 txChannelEnable)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv904x_RxTxEnableSetRangeCheck(device, orxChannelMask, orxChannelEnable, rxChannelMask, rxChannelEnable, txChannelMask, txChannelEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxTxEnableSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /*Enable requested Rx Channel signal chains*/
    recoveryAction = adrv904x_RxEnableSet(device, rxChannelMask, rxChannelEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxEnableSet Issue");
        goto cleanup;
    }

    /* Enable requested ORx Channel signal chains */
    recoveryAction = adrv904x_OrxEnableSet(device, orxChannelMask, orxChannelEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxEnableSet Issue");
        goto cleanup;
    }

    /* Enable requested Tx Channel signal chains */
    recoveryAction = adrv904x_TxEnableSet(device, txChannelMask, txChannelEnable);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxEnableSet Issue");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxEnableGet(adi_adrv904x_Device_t* const    device,
                                                            uint32_t* const                 orxChannelMask,
                                                            uint32_t* const                 rxChannelMask,
                                                            uint32_t* const                 txChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxChannelMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);

    /* Get RX Channel Enabling bit mask */
    recoveryAction =  adrv904x_Core_RadioControlInterfaceRxSpiEn_BfGet(device,
                                                                       NULL,
                                                                       (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                       &regVal);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RX Channel status get failed.");
        goto cleanup;
    }
    *rxChannelMask = (uint32_t) regVal;

    /* Get ORX Channel Enabling bit mask */
    recoveryAction =  adrv904x_Core_RadioControlInterfaceOrxSpiEn_BfGet(device,
                                                                        NULL,
                                                                        (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                        &regVal);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "ORX Channel status get failed.");
        goto cleanup;
    }
    *orxChannelMask = ((uint32_t) regVal) << ADI_ADRV904X_MAX_RX_ONLY;
    
    /* Get TX Channel Enabling bit mask */
    recoveryAction =  adrv904x_Core_RadioControlInterfaceTxSpiEn_BfGet(device,
                                                                       NULL,
                                                                       (adrv904x_BfCoreChanAddr_e) ADRV904X_BF_CORE_ADDR,
                                                                       &regVal);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TX Channel status get failed.");
        goto cleanup;
    }
    *txChannelMask = (uint32_t) regVal;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_ChannelEnableGet(adi_adrv904x_Device_t* const    device,
                                                               uint32_t* const                orxChannelMask,
                                                               uint32_t* const                rxChannelMask,
                                                               uint32_t* const                txChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t                  tmpByte        = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, orxChannelMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);

    *orxChannelMask = 0U;
    *rxChannelMask = 0U;
    *txChannelMask = 0U;
    
    recoveryAction = adrv904x_Core_RadioControlInterfaceTxonReadback_BfGet(device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &tmpByte);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxonReadback issue");
        goto cleanup;
    }

    *txChannelMask = tmpByte;

    recoveryAction = adrv904x_Core_RadioControlInterfaceRxonReadback_BfGet(device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &tmpByte);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxonReadback issue");
        goto cleanup;
    }

    *rxChannelMask = tmpByte;

    recoveryAction = adrv904x_Core_RadioControlInterfaceOrxonReadback_BfGet(device,
                                                                            NULL,
                                                                            ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                            &tmpByte);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxonReadback issue");
        goto cleanup;
    }

    *orxChannelMask = tmpByte;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlCfgSet(adi_adrv904x_Device_t* const            device,
                                                              adi_adrv904x_RadioCtrlModeCfg_t* const  radioCtrlCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, radioCtrlCfg, cleanup);

#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv904x_RadioCtrlCfgSetRangeCheck(device, radioCtrlCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RadioCtrlCfgSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    if (((device->devStateInfo.profilesValid & ADI_ADRV904X_RX_PROFILE_VALID) == ADI_ADRV904X_RX_PROFILE_VALID) &&
         (radioCtrlCfg->rxRadioCtrlModeCfg.rxEnableMode != ADI_ADRV904X_RX_EN_INVALID_MODE) &&
         (radioCtrlCfg->rxRadioCtrlModeCfg.rxChannelMask != ADI_ADRV904X_RXOFF))
    {
        recoveryAction = adrv904x_RxRadioCtrlCfgSet(device, &radioCtrlCfg->rxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxRadioCtrlCfgSet Issue");
            goto cleanup;
        }
    }

    if (((device->devStateInfo.profilesValid & ADI_ADRV904X_TX_PROFILE_VALID) == ADI_ADRV904X_TX_PROFILE_VALID) &&
         (radioCtrlCfg->txRadioCtrlModeCfg.txEnableMode != ADI_ADRV904X_TX_EN_INVALID_MODE) &&
         (radioCtrlCfg->txRadioCtrlModeCfg.txChannelMask != ADI_ADRV904X_TXOFF))
    {
        recoveryAction = adrv904x_TxRadioCtrlCfgSet(device, &radioCtrlCfg->txRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxRadioCtrlCfgSet Issue");
            goto cleanup;
        } 
    }
    
    if (((device->devStateInfo.profilesValid & ADI_ADRV904X_ORX_PROFILE_VALID) == ADI_ADRV904X_ORX_PROFILE_VALID) &&
     (radioCtrlCfg->orxRadioCtrlModeCfg.orxEnableMode != ADI_ADRV904X_ORX_EN_INVALID_MODE) &&
     (radioCtrlCfg->orxRadioCtrlModeCfg.orxChannelMask != ADI_ADRV904X_RXOFF))
    {
        recoveryAction = adrv904x_OrxRadioCtrlCfgSet(device, &radioCtrlCfg->orxRadioCtrlModeCfg);
        if(recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxRadioCtrlCfgSet Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlCfgGet(adi_adrv904x_Device_t* const            device,
                                                              const adi_adrv904x_RxChannels_e         rxChannel,
                                                              const adi_adrv904x_TxChannels_e         txChannel,
                                                              adi_adrv904x_RadioCtrlModeCfg_t* const  radioCtrlCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, radioCtrlCfg, cleanup);

    if (rxChannel > ADI_ADRV904X_ORX1)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, rxChannel, "Invalid Rx/ORx Channel");
        goto cleanup;
    }

    if (txChannel > ADI_ADRV904X_TX7)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "Invalid Tx Channel");
        goto cleanup;
    }
    
    if ((txChannel == ADI_ADRV904X_TXOFF) &&
        (rxChannel == ADI_ADRV904X_RXOFF))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, txChannel, "No valid Tx and Rx channels");
        goto cleanup;
    }

    if ((rxChannel >= ADI_ADRV904X_RX0) && (rxChannel <= ADI_ADRV904X_RX7))
    {
        recoveryAction = adrv904x_RxRadioCtrlCfgGet(device, rxChannel, &radioCtrlCfg->rxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

    if ((txChannel >= ADI_ADRV904X_TX0) && (txChannel <= ADI_ADRV904X_TX7))
    {
        recoveryAction = adrv904x_TxRadioCtrlCfgGet(device, txChannel, &radioCtrlCfg->txRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "TxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

    if ((rxChannel >= ADI_ADRV904X_ORX0) && (rxChannel <= ADI_ADRV904X_ORX1))
    {
        recoveryAction = adrv904x_OrxRadioCtrlCfgGet(device, rxChannel, &radioCtrlCfg->orxRadioCtrlModeCfg);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "OrxRadioCtrlCfgGet Issue");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoFrequencySet(adi_adrv904x_Device_t* const          device,
                                                             const adi_adrv904x_LoConfig_t* const  loConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;

    adrv904x_CpuCmd_SetLoFreq_t loInfo;
    adrv904x_CpuCmd_SetLoFreqResp_t cmdRsp;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    uint8_t writeGpIntPin1Bool = 0U;
    uint8_t writeGpIntPin0Bool = 0U;
    uint8_t writePllUnlockMaskBool[ADI_ADRV904X_MAX_TXCHANNELS];

    uint8_t tmpPllRampDownMask = 0U;
    uint8_t origPllRampDownMask[ADI_ADRV904X_MAX_TXCHANNELS];
    uint8_t i = 0U;
    uint8_t pllUnlockRampDownMask = 0U;
    uint32_t tmpTxChannelMask = 0U;

    uint8_t  gpIntMaskRegsOrig[4] = { 0U, 0U, 0U, 0U };
    uint8_t  gpIntMaskRegsTmp[4]  = { 0U, 0U, 0U, 0U };
    uint8_t* gpIntMaskPin1Byte9   = &gpIntMaskRegsOrig[0];
    uint8_t* gpIntMaskPin0Byte9   = &gpIntMaskRegsOrig[2];

    static const uint8_t GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK = (1U << 4U);
    static const uint8_t GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK = (1U << 5U);
    static const uint8_t GPINT_RF1_PLL_UNLOCK_BYTE9_MASK    = (1U << 7U);
    static const uint8_t GPINT_RF0_PLL_UNLOCK_BYTE10_MASK   = (1U << 0U);
        /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, loConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loInfo, 0, sizeof(adrv904x_CpuCmd_SetLoFreq_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_SetLoFreqResp_t));
    ADI_LIBRARY_MEMSET(&writePllUnlockMaskBool, 0, sizeof(writePllUnlockMaskBool));
    ADI_LIBRARY_MEMSET(&origPllRampDownMask, 0, sizeof(origPllRampDownMask));

#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv904x_LoFrequencySetRangeCheck(device, loConfig);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "LoFrequencySetRangeCheck Issue");
        goto cleanup;
    }
#endif

        /* Read GPINT registers */
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, NULL, 2U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read GPINT_PIN1 issue");
        goto cleanup;
    }
    recoveryAction = adi_adrv904x_RegistersByteRead(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, NULL, 2U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Read GPINT_PIN0 issue");
        goto cleanup;
    }
    ADI_LIBRARY_MEMCPY(gpIntMaskRegsTmp, gpIntMaskRegsOrig, sizeof(gpIntMaskRegsOrig));
    gpIntMaskPin1Byte9 = &gpIntMaskRegsTmp[0];
    gpIntMaskPin0Byte9 = &gpIntMaskRegsTmp[2];

    /* Based on loName determine the GPINT and PLL-unlock Tx rampdown to disable */
    switch (loConfig->loName) 
    {
    case ADI_ADRV904X_LO0:
        pllUnlockRampDownMask = (uint8_t)ADI_ADRV904X_RDT_RF0_PLL_UNLOCK;
        gpIntMaskPin1Byte9[1] |= GPINT_RF0_PLL_UNLOCK_BYTE10_MASK;
        gpIntMaskPin0Byte9[1] |= GPINT_RF0_PLL_UNLOCK_BYTE10_MASK;
        break;
    case ADI_ADRV904X_LO1:
        pllUnlockRampDownMask = (uint8_t)ADI_ADRV904X_RDT_RF1_PLL_UNLOCK;
        gpIntMaskPin1Byte9[0] |= GPINT_RF1_PLL_UNLOCK_BYTE9_MASK;
        gpIntMaskPin0Byte9[0] |= GPINT_RF1_PLL_UNLOCK_BYTE9_MASK;
        break;
            default:
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, loConfig->loName, "Invalid LO selected for setting LO frequency");
        goto cleanup;
    }

    pllUnlockRampDownMask >>= 3U;

    /* Disable various GP INTs while changing frequency */
    gpIntMaskPin1Byte9[0] |= (GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK | GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK);
    gpIntMaskPin0Byte9[0] |= (GPINT_RF0_PLL_OVERRANGE_BYTE9_MASK | GPINT_RF1_PLL_OVERRANGE_BYTE9_MASK);
        /* Write GPINT registers if Byte9 or Byte10 have been changed */
    writeGpIntPin1Bool = (gpIntMaskPin1Byte9[0] != gpIntMaskRegsOrig[0]) || (gpIntMaskPin1Byte9[1] != gpIntMaskRegsOrig[1]);
    writeGpIntPin0Bool = (gpIntMaskPin0Byte9[0] != gpIntMaskRegsOrig[2]) || (gpIntMaskPin0Byte9[1] != gpIntMaskRegsOrig[3]);
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN1 issue");
            goto cleanup;
        }
    }
    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN0 issue");
            goto cleanup;
        }
    }

    /* Store current PLL Unlock mask for ramp down configuration for each channel and update them temporarily */
    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i)
    {
        tmpTxChannelMask = (uint32_t)((uint32_t)1U << i);

        if (((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & tmpTxChannelMask) == 0U)
        {
            /* skip for uninitialized channels */
            continue;
        }

        /* Retrieve the base address for selected tx channel */
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)tmpTxChannelMask, &txBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, tmpTxChannelMask, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        /* Store pll unlock mask */
        recoveryAction = adrv904x_TxFuncs_PllUnlockMask_BfGet(device, NULL, txBaseAddr, &origPllRampDownMask[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Tx Pll Unlock Mask for ramp-down config");
            goto cleanup;
        }

        tmpPllRampDownMask =  origPllRampDownMask[i] | pllUnlockRampDownMask; /* 1 means disabled */
        /* store boolean on whether temporary values to be written are different from the original */
        writePllUnlockMaskBool[i] = (tmpPllRampDownMask != origPllRampDownMask[i]);

        /* update pll unlock mask if tmp value is different */
        if (writePllUnlockMaskBool[i])
        {
            recoveryAction = adrv904x_TxFuncs_PllUnlockMask_BfSet(device, NULL, txBaseAddr, tmpPllRampDownMask);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing temporary Tx Pll Unlock Mask for ramp-down config");
                goto cleanup;
            }
        }
    }

        /* Prepare the command payload */
    loInfo.loName = (adi_adrv904x_LoName_t) loConfig->loName;
    loInfo.loConfigSel = (adi_adrv904x_LoOption_t) ADRV904X_HTOCL(loConfig->loConfigSel) ;
    loInfo.loFrequency_Hz = (uint64_t) ADRV904X_HTOCLL(loConfig->loFrequency_Hz);
        /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend( device,
                                          ADI_ADRV904X_CPU_TYPE_0,
                                          ADRV904X_LINK_ID_0,
                                          ADRV904X_CPU_CMD_ID_SET_LO_FREQUENCY,
                                          (void*)&loInfo,
                                          sizeof(loInfo),
                                          (void*)&cmdRsp,
                                          sizeof(cmdRsp),
                                          &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

        /* Write back original GPINT settings */
    gpIntMaskPin1Byte9 = &gpIntMaskRegsOrig[0];
    gpIntMaskPin0Byte9 = &gpIntMaskRegsOrig[2];
    if (writeGpIntPin1Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN1_BYTE9, gpIntMaskPin1Byte9, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN1 issue");
            goto cleanup;
        }
    }
    if (writeGpIntPin0Bool)
    {
        recoveryAction = adi_adrv904x_RegistersByteWrite(device, NULL, ADRV904X_ADDR_GPINT_MASK_PIN0_BYTE9, gpIntMaskPin0Byte9, 2U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Registers32Write GPINT_PIN0 issue");
            goto cleanup;
        }
    }

    /* Restore Pll unlock masks for ramp-down configuration */
    for (i = 0; i < ADI_ADRV904X_MAX_TXCHANNELS; ++i) 
    {
        tmpTxChannelMask = (uint32_t)((uint32_t)1U << i);

        if ((((device->devStateInfo.initializedChannels >> ADI_ADRV904X_TX_INITIALIZED_CH_OFFSET) & tmpTxChannelMask) == 0U) ||
            (writePllUnlockMaskBool[i] == 0U))
        {
            /* skip for uninitialized channels or unchanged Pll unlock masks*/
            continue;
        }

        /* Retrieve the base address for selected tx channel */
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)tmpTxChannelMask, &txBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, tmpTxChannelMask, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        recoveryAction = adrv904x_TxFuncs_PllUnlockMask_BfSet(device, NULL, txBaseAddr, origPllRampDownMask[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing temporary Tx Pll Unlock Mask for ramp-down config");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoFrequencyGet(adi_adrv904x_Device_t* const            device,
                                                             adi_adrv904x_LoConfigReadback_t* const  loConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetLoFreqResp_t cmdRsp;
    adrv904x_CpuCmd_GetLoFreq_t loCmd;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, loConfig, cleanup);

    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_GetLoFreqResp_t));
    ADI_LIBRARY_MEMSET(&loCmd, 0, sizeof(adrv904x_CpuCmd_GetLoFreq_t));
    
    /* Executing the GET LO Freq command */ 
    loCmd.loName = (adi_adrv904x_LoName_t) loConfig->loName;

    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_GET_LO_FREQUENCY,
                                            (void*)&loCmd,
                                            sizeof(loCmd),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    loConfig->loFrequency_Hz = (uint64_t) ADRV904X_CTOHLL(cmdRsp.loFrequency_Hz);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_CfgPllToChanCtrl(adi_adrv904x_Device_t* const device, 
	                                                            uint8_t rf0MuxTx0_3,
	                                                            uint8_t rf0MuxTx4_7, 
	                                                            uint8_t rf0MuxRx0_3,
	                                                            uint8_t rf0MuxRx4_7)
{
		adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
	adrv904x_CpuCmd_ChanCtrlToPlls_t ChanCtrlToPllsPayload; 
	adrv904x_CpuCmd_ChanCtrlToPllsResp_t cmdRsp; 
	adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
	adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

	/* check Device pointer is not null */	
	ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
	ADI_ADRV904X_API_ENTRY(&device->common);
	
	ADI_LIBRARY_MEMSET(&ChanCtrlToPllsPayload, 0, sizeof(adrv904x_CpuCmd_ChanCtrlToPlls_t));
	ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_ChanCtrlToPllsResp_t));

	/* Prepare the command Payload */
	ChanCtrlToPllsPayload.rf0MuxTx0_3 = (uint8_t) rf0MuxTx0_3;
	ChanCtrlToPllsPayload.rf0MuxTx4_7 = (uint8_t) rf0MuxTx4_7;
	ChanCtrlToPllsPayload.rf0MuxRx0_3 = (uint8_t) rf0MuxRx0_3;
	ChanCtrlToPllsPayload.rf0MuxRx4_7 = (uint8_t) rf0MuxRx4_7;

	/* Send Command and Receive Response */
	recoveryAction = adrv904x_CpuCmdSend(device, 
		                                 ADI_ADRV904X_CPU_TYPE_0, 
		                                 ADRV904X_LINK_ID_0, 
		                                 ADRV904X_CPU_CMD_ID_SET_CHAN_TO_PLLS, 
		                                 (void*)&ChanCtrlToPllsPayload, 
		                                 sizeof(ChanCtrlToPllsPayload),
		                                 (void*)&cmdRsp,
		                                 sizeof(cmdRsp),
		                                 &cmdStatus);
	if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
	{
		ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
	}

cleanup:
	ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);		
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoLoopFilterSet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_LoName_e loName,
                                                              const adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_SetLoopfilter_t loopFilterPayload;
    adrv904x_CpuCmd_SetLoopfilterResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, loLoopFilterConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loopFilterPayload, 0, sizeof(adrv904x_CpuCmd_SetLoopfilter_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_SetLoopfilterResp_t));

#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
        recoveryAction = adrv904x_LoLoopFilterSetRangeCheck(device, loName, loLoopFilterConfig);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "LoLoopFilterRangeCheck Issue");
            goto cleanup;
        }
#endif

    loopFilterPayload.loName = (adi_adrv904x_LoName_t)(loName);
    loopFilterPayload.loopBandwidth = ADRV904X_HTOCL((uint32_t)(loLoopFilterConfig->loopBandwidth_kHz * 1000U));
    loopFilterPayload.phaseMargin = ADRV904X_HTOCL((uint32_t)loLoopFilterConfig->phaseMargin_degrees);
    
    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_SET_LOOPFILTER,
                                            (void*)&loopFilterPayload,
                                            sizeof(loopFilterPayload),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_LoLoopFilterGet(adi_adrv904x_Device_t* const device,
                                                              const adi_adrv904x_LoName_e loName,
                                                              adi_adrv904x_LoLoopFilterCfg_t* const loLoopFilterConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction     = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetLoopfilter_t loopFilterPayload;
    adrv904x_CpuCmd_GetLoopfilterResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, loLoopFilterConfig, cleanup);

    ADI_LIBRARY_MEMSET(&loopFilterPayload, 0, sizeof(adrv904x_CpuCmd_GetLoopfilter_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_GetLoopfilterResp_t));

    if ((loName != ADI_ADRV904X_LO0) && (loName != ADI_ADRV904X_LO1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, loName, "Invalid LO Name");
        goto cleanup;
    }
    loopFilterPayload.loName = (adi_adrv904x_LoName_t)(loName); 
    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_GET_LOOPFILTER,
                                            (void*)&loopFilterPayload,
                                            sizeof(loopFilterPayload),
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    loLoopFilterConfig->loopBandwidth_kHz = (uint16_t) (ADRV904X_CTOHL(cmdRsp.loopBandwidth) / 1000);
    loLoopFilterConfig->phaseMargin_degrees = (uint8_t) ADRV904X_CTOHL(cmdRsp.phaseMargin);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}
                                                              
ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_PllStatusGet(adi_adrv904x_Device_t* const device,
                                                           uint32_t* const        pllLockStatus)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint8_t  CLK_PLL_LOCK_STATUS_SHIFT = 0U;
    static const uint8_t  LO0_LOCK_STATUS_SHIFT = 1U;
    static const uint8_t  LO1_LOCK_STATUS_SHIFT = 2U;
    static const uint8_t  SERDES_PLL_LOCK_STATUS_SHIFT = 3U;
    static const uint32_t LOCK_BIT_MASK = 0x00000001U;

    uint8_t pllLockStatusRead = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pllLockStatus, cleanup);

    /* Clear status of all PLLs */
    *pllLockStatus = 0U;

    /* Read CLK Pll status */
    recoveryAction =  adrv904x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV904X_BF_DIGITAL_CORE_JESD_CLKPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get CLK PLL Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 0 with Clk Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << CLK_PLL_LOCK_STATUS_SHIFT;

    /* Read LO0 Pll status */
    recoveryAction =  adrv904x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV904X_BF_DIGITAL_CORE_EAST_RFPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get LO0 Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 1 with LO0 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << LO0_LOCK_STATUS_SHIFT;

    /* Read LO1 Pll status */
    recoveryAction =  adrv904x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV904X_BF_DIGITAL_CORE_WEST_RFPLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get LO1 Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 2 with LO1 Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << LO1_LOCK_STATUS_SHIFT;

    /* Read SERDES Pll status */
    recoveryAction =  adrv904x_PllMemMap_SynLock_BfGet(device,
                                                       NULL,
                                                       ADRV904X_BF_DIGITAL_CORE_JESD_SERDES_PLL,
                                                       &pllLockStatusRead);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API Get SERDES PLL Lock Status Failed.");
        goto cleanup;
    }

    /* Update pllLockStatus bit 3 with Serdes Pll Status */
    *pllLockStatus |= ((uint32_t)pllLockStatusRead & LOCK_BIT_MASK) << SERDES_PLL_LOCK_STATUS_SHIFT;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RxTxLoFreqGet(adi_adrv904x_Device_t* const            device,
                                                            adi_adrv904x_RxTxLoFreqReadback_t* const  rxTxLoFreq)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetRxTxLoFreqResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint8_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxTxLoFreq, cleanup);

    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_GetRxTxLoFreqResp_t));
    ADI_LIBRARY_MEMSET(rxTxLoFreq, 0, sizeof(adi_adrv904x_RxTxLoFreqReadback_t));
    
    /* Send command and receive response */
    recoveryAction = adrv904x_CpuCmdSend(   device,
                                            ADI_ADRV904X_CPU_TYPE_0,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_CPU_CMD_ID_GET_RXTXLOFREQ,
                                            (void*) NULL, /* No CMD load needed for this cmd */
                                            0U,
                                            (void*)&cmdRsp,
                                            sizeof(cmdRsp),
                                            &cmdStatus);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
    }

    /* Copying CPU responses back to user structure */
    for(i = 0 ; i < ADI_ADRV904X_MAX_RX_ONLY ; i++)
    {
        rxTxLoFreq->rxLoName[i] =  (adi_adrv904x_LoName_e)ADRV904X_CTOHL((uint32_t)cmdRsp.rxLoName[i]);
        rxTxLoFreq->rxFreq_Khz[i] =  ADRV904X_CTOHL(cmdRsp.rxFreq[i]);
    }

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {
        rxTxLoFreq->txLoName[i] =  (adi_adrv904x_LoName_e)ADRV904X_CTOHL((uint32_t)cmdRsp.txLoName[i]);
        rxTxLoFreq->txFreq_Khz[i] =  ADRV904X_CTOHL(cmdRsp.txFreq[i]);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlTxRxEnCfgSet(adi_adrv904x_Device_t* const                    device,
                                                                    const adi_adrv904x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg,
                                                                    uint8_t                                         pinIndex,
                                                                    uint8_t                                         configSel)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;

    adrv904xCoreBfSet8FnPtr_t adrvCoreRadioControlInterfaceTRxEnPinXBfSet[4][ADI_ADRV904X_TRX_CTRL_PIN_COUNT] = 
    {
        {
            adrv904x_Core_RadioControlInterfaceTxEnPin0_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin1_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin2_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin3_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin4_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin5_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin6_BfSet,
            adrv904x_Core_RadioControlInterfaceTxEnPin7_BfSet
        },
        {
            adrv904x_Core_RadioControlInterfaceTxAntEnPin0_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin1_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin2_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin3_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin4_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin5_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin6_BfSet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin7_BfSet 
        },
        { 
            adrv904x_Core_RadioControlInterfaceRxEnPin0_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin1_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin2_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin3_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin4_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin5_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin6_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin7_BfSet 
        },
        { 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin0_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin1_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin2_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin3_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin4_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin5_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin6_BfSet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin7_BfSet
        }
    };

    adi_adrv904x_TxRxEnPin_e arrayTxRxPin[ADI_ADRV904X_TRX_CTRL_PIN_COUNT] = 
    {
        ADI_ADRV904X_TXRXEN_PIN0,
        ADI_ADRV904X_TXRXEN_PIN1,
        ADI_ADRV904X_TXRXEN_PIN2,
        ADI_ADRV904X_TXRXEN_PIN3,
        ADI_ADRV904X_TXRXEN_PIN4,
        ADI_ADRV904X_TXRXEN_PIN5,
        ADI_ADRV904X_TXRXEN_PIN6,
        ADI_ADRV904X_TXRXEN_PIN7
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRxEnCfg, cleanup);

    if (pinIndex < ADI_ADRV904X_TXRXEN_PIN0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, pinIndex, "Invalid pinIndex");
        goto cleanup;
    }

    if ((configSel < ADI_ADRV904X_TXRXEN_TX_ENABLE_MAP) ||
        (configSel > ADI_ADRV904X_TXRXEN_ALL))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,  recoveryAction, configSel, "Invalid configSel");
        goto cleanup;
    }

    for (i = 0; i < ADI_ADRV904X_TRX_CTRL_PIN_COUNT; i++)
    {
        if (ADRV904X_BF_EQUAL(pinIndex, arrayTxRxPin[i]))
        {
            if (ADRV904X_BF_EQUAL(configSel, ADI_ADRV904X_TXRXEN_TX_ENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[0][i](device,
                                                                                   NULL,
                                                                                   ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->txEnMapping[i]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control En Pin Failed.");
                    goto cleanup;
                }
            }

            if (ADRV904X_BF_EQUAL(configSel, (uint8_t)ADI_ADRV904X_TXRXEN_TX_ALTENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[1][i](device,
                                                                                   NULL,
                                                                                   ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->txAltMapping[i]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Alternate En Pin Failed.");
                    goto cleanup; 
                }
            }

            if (ADRV904X_BF_EQUAL(configSel, ADI_ADRV904X_TXRXEN_RX_ENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[2][i](device,
                                                                                   NULL,
                                                                                   ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->rxEnMapping[i]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control En Pin Failed.");
                    goto cleanup;
                }
            }

            if (ADRV904X_BF_EQUAL(configSel, ADI_ADRV904X_TXRXEN_RX_ALTENABLE_MAP))
            {
                recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfSet[3][i](device,
                                                                                   NULL,
                                                                                   ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                                   txRxEnCfg->rxAltMapping[i]);
                if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
                {
                    ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Alternate En Pin Failed.");
                    goto cleanup;
                }
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlTxRxEnCfgGet(adi_adrv904x_Device_t* const              device,
                                                                    adi_adrv904x_RadioCtrlTxRxEnCfg_t* const  txRxEnCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t i = 0U;

    adrv904xCoreBfGet8FnPtr_t adrvCoreRadioControlInterfaceTRxEnPinXBfGet[4][ADI_ADRV904X_TRX_CTRL_PIN_COUNT] = 
    {
        {
            adrv904x_Core_RadioControlInterfaceTxEnPin0_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin1_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin2_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin3_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin4_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin5_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin6_BfGet,
            adrv904x_Core_RadioControlInterfaceTxEnPin7_BfGet
        },
        {
            adrv904x_Core_RadioControlInterfaceTxAntEnPin0_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin1_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin2_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin3_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin4_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin5_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin6_BfGet, 
            adrv904x_Core_RadioControlInterfaceTxAntEnPin7_BfGet 
        },
        { 
            adrv904x_Core_RadioControlInterfaceRxEnPin0_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin1_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin2_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin3_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin4_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin5_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin6_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxEnPin7_BfGet 
        },
        { 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin0_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin1_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin2_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin3_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin4_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin5_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin6_BfGet, 
            adrv904x_Core_RadioControlInterfaceRxAntEnPin7_BfGet
        }
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txRxEnCfg, cleanup);

    for (i = 0; i < ADI_ADRV904X_TRX_CTRL_PIN_COUNT; i++)
    {
        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[0][i](device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->txEnMapping[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[1][i](device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->txAltMapping[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API TX Radio Control Alternate En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[2][i](device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->rxEnMapping[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control En Pin Failed.");
            goto cleanup;
        }

        recoveryAction = adrvCoreRadioControlInterfaceTRxEnPinXBfGet[3][i](device,
                                                                           NULL,
                                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                           &txRxEnCfg->rxAltMapping[i]);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "API RX Radio Control Alternate En Pin Failed.");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureGet(adi_adrv904x_Device_t* const        device,
                                                             const uint16_t                      avgMask,
                                                             adi_adrv904x_DevTempData_t* const   deviceTemperature)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetDevTemp_t devTempCmd;
    adrv904x_CpuCmd_GetDevTempResp_t devTempCmdResp;
    adrv904x_CpuCmd_GetDevTempResp_t devTempCmdResp0;
    adrv904x_CpuCmd_GetDevTempResp_t devTempCmdResp1;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    const int16_t ADRV904X_TEMP_UNUSED = -274;
    uint32_t cpuTypeIdx = 0U;
    uint32_t curTempSensor = 0U;
    uint16_t avgDiv = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, deviceTemperature, cleanup);

    ADI_LIBRARY_MEMSET(&devTempCmd, 0, sizeof(adrv904x_CpuCmd_GetDevTemp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp, 0, sizeof(adrv904x_CpuCmd_GetDevTempResp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp0, 0, sizeof(adrv904x_CpuCmd_GetDevTempResp_t));
    ADI_LIBRARY_MEMSET(&devTempCmdResp1, 0, sizeof(adrv904x_CpuCmd_GetDevTempResp_t));

    /* Set ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command parameters */
    devTempCmd.avgMask = ADRV904X_HTOCS(avgMask);

    for (cpuTypeIdx = (uint32_t) ADI_ADRV904X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response */
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE,
                                                (void*)&devTempCmd,
                                                sizeof(devTempCmd),
                                                (void*)&devTempCmdResp,
                                                sizeof(devTempCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(devTempCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            if (cpuTypeIdx == (uint32_t) ADI_ADRV904X_CPU_TYPE_0)
            {
                devTempCmdResp0 = devTempCmdResp;
            }
            if (cpuTypeIdx == (uint32_t) ADI_ADRV904X_CPU_TYPE_1)
            {
                devTempCmdResp1 = devTempCmdResp;
            }
        }
    }

    /* Copy/translate the temperature data to the caller's buffer */
    deviceTemperature->avgMask = ADRV904X_CTOHS(devTempCmdResp0.tempData.avgMask | devTempCmdResp1.tempData.avgMask);
    deviceTemperature->tempDegreesCelsiusAvg = 0;
    for( curTempSensor = 0U; curTempSensor < ADI_ADRV904X_DEVTEMP_MAX_SENSORS; curTempSensor++)
    {
        if ((int16_t)ADRV904X_CTOHS(devTempCmdResp0.tempData.tempDegreesCelsius[curTempSensor]) == ADRV904X_TEMP_UNUSED )
        {
            deviceTemperature->tempDegreesCelsius[curTempSensor] = ADRV904X_CTOHS(devTempCmdResp1.tempData.tempDegreesCelsius[curTempSensor]);
        }
        else
        {
            deviceTemperature->tempDegreesCelsius[curTempSensor] = ADRV904X_CTOHS(devTempCmdResp0.tempData.tempDegreesCelsius[curTempSensor]);
        }

        if ((deviceTemperature->tempDegreesCelsius[curTempSensor] != ADRV904X_TEMP_UNUSED) && 
            (deviceTemperature->avgMask & (0x0001 << curTempSensor)))
        {
            deviceTemperature->tempDegreesCelsiusAvg += deviceTemperature->tempDegreesCelsius[curTempSensor];
            ++avgDiv;
        }
    }

    if ((avgDiv > 0) && (deviceTemperature->avgMask > 0))
    {
        deviceTemperature->tempDegreesCelsiusAvg /= avgDiv;
    }
    else
    {
        deviceTemperature->tempDegreesCelsiusAvg = ADRV904X_TEMP_UNUSED;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureEnableGet(adi_adrv904x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_GetDevTempSnsEnResp_t devTempEnCmdResp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;

    /* Check for pointer not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(tempEnData);

    ADI_LIBRARY_MEMSET(&devTempEnCmdResp, 0, sizeof(adrv904x_CpuCmd_GetDevTempSnsEnResp_t));
    *tempEnData = 0;

    for (cpuTypeIdx = (uint32_t) ADI_ADRV904X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response*/
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_GET_ENABLED_TEMPSENSORS,
                                                (void*) NULL, /* No CMD load needed for this cmd */
                                                0U,
                                                (void*)&devTempEnCmdResp,
                                                sizeof(devTempEnCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(devTempEnCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            /* Copy/translate the temperature data to the caller's buffer */
            *tempEnData |= (ADRV904X_CTOHS(devTempEnCmdResp.tempEnData));
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TemperatureEnableSet(adi_adrv904x_Device_t* const        device,
                                                                   uint16_t* const                     tempEnData)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_CpuCmd_SetDevTempSnsEn_t devTempCmd;
    adrv904x_CpuCmd_SetDevTempSnsEnResp_t devTempEnCmdResp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_SIMULATED_ERROR;
    uint32_t cpuTypeIdx = 0U;

    /* Check for pointer not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(tempEnData);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&devTempEnCmdResp, 0, sizeof(adrv904x_CpuCmd_SetDevTempSnsEnResp_t));
    /* Set ADRV904X_CPU_CMD_ID_GET_DEVICE_TEMPERATURE command parameters */ 
    devTempCmd.tempEnData = ADRV904X_HTOCS(*tempEnData);
    *tempEnData = 0;

    for (cpuTypeIdx = (uint32_t) ADI_ADRV904X_CPU_TYPE_0; cpuTypeIdx < (uint32_t) ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuTypeIdx)
    {
        /* Send command and receive response; cpu0*/
        recoveryAction = adrv904x_CpuCmdSend(   device,
                                                (adi_adrv904x_CpuType_e)cpuTypeIdx,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_CPU_CMD_ID_SET_ENABLED_TEMPSENSORS,
                                                (void*)&devTempCmd,
                                                sizeof(devTempCmd), 
                                                (void*)&devTempEnCmdResp,
                                                sizeof(devTempEnCmdResp),
                                                &cmdStatus);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(devTempEnCmdResp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
        }
        else
        {
            *tempEnData |= ADRV904X_CTOHS(devTempEnCmdResp.tempEnData);
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamGpioConfigSet(adi_adrv904x_Device_t* const          device,
                                                                  const adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{  
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    int idx = 0;

    adi_adrv904x_GpioSignal_e signals[ADI_ADRV904X_MAX_STREAMGPIO] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23 
    };


    adi_adrv904x_GpioSignal_e digOutputSignals[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_15,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_16,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_17,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_18,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_19,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_20,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_21,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_22,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_23 
    };

    adi_adrv904x_GpioSignal_e anaOutputSignals[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_15
    };

    adi_adrv904x_GpioAnaPinSel_e analogPinSelect = ADI_ADRV904X_GPIO_ANA_INVALID;

    adi_adrv904x_GpioPinSel_e pinSelect = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamGpioPinCfg, cleanup);

#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv904x_StreamGpioConfigSetRangeCheck(device,
                                                            streamGpioPinCfg);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "StreamGpioSignalSetRangeCheck issue");
        goto cleanup;
    }
#endif

    /* Release all Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV904X_MAX_STREAMGPIO; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, signals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   idx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        /* Release if pinSelect is valid */
        if (pinSelect != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease(device, pinSelect, signals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalRelease issue");
                goto cleanup;
            }
        }
    }

    /* Config Stream GPIO if valid */
    for (idx = 0U; idx < ADI_ADRV904X_MAX_STREAMGPIO; idx++)
    {
        /* Set if pinSelect is valid */
        if (streamGpioPinCfg->streamGpInput[idx] != ADI_ADRV904X_GPIO_INVALID)
        {    
            pinSelect = (adi_adrv904x_GpioPinSel_e)(idx);

            /* Set the signal for this gpio */
            recoveryAction = adrv904x_GpioSignalSet(device, pinSelect, signals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalSet issue");
                goto cleanup;
            }
        }
    }



    /* Release Digital Output GPIO used by Stream Processor */
    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, digOutputSignals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   idx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        /* Release if pinSelect is valid */
        if (pinSelect != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalRelease(device, pinSelect, digOutputSignals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalRelease issue");
                goto cleanup;
            }
        }
    }

    /* Config Digital Output GPIO used by Stream Processor */
    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG; idx++)
    {
        /* Set if pinSelect is valid */
        if (streamGpioPinCfg->streamGpDigOutput[idx] != ADI_ADRV904X_GPIO_INVALID)
        {    
            pinSelect = (adi_adrv904x_GpioPinSel_e)(idx);

            /* Set the signal for this gpio */
            recoveryAction = adrv904x_GpioSignalSet(device, pinSelect, digOutputSignals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalSet issue");
                goto cleanup;
            }
        }
    }

    /* Release Analog Output GPIO used by Stream Processor */
    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA; idx++)
    {
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioAnalogSignalFind(device, &analogPinSelect, anaOutputSignals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   idx,
                                   "GpioSignalFind issue");
            goto cleanup;
        }

        /* Release if analogPinSelect is valid */
        if (analogPinSelect != ADI_ADRV904X_GPIO_ANA_INVALID)
        {
            recoveryAction = adrv904x_GpioAnalogSignalRelease(device, analogPinSelect, anaOutputSignals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalRelease issue");
                goto cleanup;
            }
        }
    }

    /* Config Analog Output GPIO used by Stream Processor */
    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA; idx++)
    {
        /* Set if analogPinSelect is valid */
        if (streamGpioPinCfg->streamGpAnaOutput[idx] != ADI_ADRV904X_GPIO_ANA_INVALID)
        {    
            analogPinSelect = (adi_adrv904x_GpioAnaPinSel_e)(idx);

            /* Set the signal for this gpio */
            recoveryAction = adrv904x_GpioAnalogSignalSet(device, analogPinSelect, anaOutputSignals[idx], ADI_ADRV904X_CHOFF);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       idx,
                                       "GpioSignalSet issue");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamGpioConfigGet(adi_adrv904x_Device_t* const         device,
                                                                  adi_adrv904x_StreamGpioPinCfg_t* const streamGpioPinCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    int idx = 0;
    adi_adrv904x_GpioSignal_e signals[ADI_ADRV904X_MAX_STREAMGPIO] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_15,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_16,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_17,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_18,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_19,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_20,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_21,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_22,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_PIN_23 
    };


    adi_adrv904x_GpioSignal_e digOutputSignals[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_15,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_16,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_17,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_18,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_19,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_20,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_21,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_22,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_DIGITAL_OUT_23 
    };

    adi_adrv904x_GpioSignal_e anaOutputSignals[ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA] = { 
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_GPIO_STREAM_PROC_ANALOG_OUT_15
    };

    adi_adrv904x_GpioAnaPinSel_e analogPinSelect = ADI_ADRV904X_GPIO_ANA_INVALID;

    adi_adrv904x_GpioPinSel_e pinSelect = ADI_ADRV904X_GPIO_INVALID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamGpioPinCfg, cleanup);

    for (idx = 0U; idx < ADI_ADRV904X_MAX_STREAMGPIO; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, signals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "GpioSignalGet issue");
            goto cleanup;
        }

        streamGpioPinCfg->streamGpInput[idx] = pinSelect;
    }


    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_DIG; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioSignalFind(device, &pinSelect, digOutputSignals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "GpioSignalGet issue for digital output");
            goto cleanup;
        }
        streamGpioPinCfg->streamGpDigOutput[idx] = pinSelect;
    }

    for (idx = 0U; idx < ADI_ADRV904X_DFE_CTRL_MAX_GPIO_ANA; idx++)
    {    
        /* Find the signal for this gpio index */
        recoveryAction = adrv904x_GpioAnalogSignalFind(device, &analogPinSelect, anaOutputSignals[idx], ADI_ADRV904X_CHOFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "GpioSignalGet issue for analog output");
            goto cleanup;
        }
        streamGpioPinCfg->streamGpAnaOutput[idx] = analogPinSelect;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_OrxNcoFreqCalculate(adi_adrv904x_Device_t* const    device,
                                                                  const adi_adrv904x_RxChannels_e orxChannel,
                                                                  const uint32_t                  txSynthesisBwLower_kHz,
                                                                  const uint32_t                  txSynthesisBwUpper_kHz,
                                                                  int32_t* const                  ncoShiftFreqAdc_kHz,
                                                                  int32_t* const                  ncoShiftFreqDatapath_kHz)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    uint32_t orxChannelIdx = 0U;
    uint32_t centreTxSynthesisBw_kHz = (txSynthesisBwLower_kHz + txSynthesisBwUpper_kHz)/2U;
    uint32_t boundaryDistanceLower = 0U;
    uint32_t boundaryDistanceUpper = 0U;
    uint32_t nyquistZone1SynthesisBwUpper = 0U;
    uint32_t nyquistZone1SynthesisBwLower = 0U;
    uint32_t orxAdcSampleRate_kHz = 0U;
    int32_t  passbandEdgeShift = 0;
    int32_t  localNcoShiftFreqAdc_kHz = 0;
    uint8_t  validRangeSynthesisBw = 0U;

    const uint32_t ORX_ADC_RATE_GUARD_REGION_KHZ = 100000U; /* Distance between the RF synthesis BW edges and multiples of FS/2 */
    const float    HB_FILTER_PASSBAND_EDGE       = 0.44f;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, ncoShiftFreqAdc_kHz, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, ncoShiftFreqDatapath_kHz, cleanup);

    /* Range check orxChannel enum */
    if ((orxChannel != ADI_ADRV904X_ORX0) &&
        (orxChannel != ADI_ADRV904X_ORX1))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, orxChannel, "ORx Channel selected is invalid. Must select ORx0-1.");
        goto cleanup;
    }

    orxChannelIdx = (orxChannel == ADI_ADRV904X_ORX0) ? 0U : 1U;
    orxAdcSampleRate_kHz = device->initExtract.orx.orxChannelCfg[orxChannelIdx].orxAdcSampleRate_kHz;
    /* Pick only the data that lies in the 0 to Fs region and shift that to DC */
    localNcoShiftFreqAdc_kHz = -(int32_t)(centreTxSynthesisBw_kHz % orxAdcSampleRate_kHz);
    /* Recalculate the upper and lower synthesis Bw edges such that it lies in the 0 to Fs region*/
    nyquistZone1SynthesisBwUpper = txSynthesisBwUpper_kHz % orxAdcSampleRate_kHz;
    nyquistZone1SynthesisBwLower = txSynthesisBwLower_kHz % orxAdcSampleRate_kHz;

    /* if the RF synthesis BW lies across an Fs boundary, the values of the upper synthesis BW
     * would end up lower than the lower synthesis BW edge */
    if (nyquistZone1SynthesisBwUpper < nyquistZone1SynthesisBwLower)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Invalid relation between Tx synthesis bandwidth upper and lower frequencies in the first Nyquist Zone");
        goto cleanup;
    }

    /* Make sure the Synthesis Bandwidth is at least 100 MHz away from DC and Fs/2 or Fs/2 and Fs */
    /* If the signal lies in a odd nyquist zones (0 to 0.5*Fs) we need a -ve frequency shift at the NCO */
    if ((nyquistZone1SynthesisBwLower >= (ORX_ADC_RATE_GUARD_REGION_KHZ)) &&
        (nyquistZone1SynthesisBwUpper <= ((orxAdcSampleRate_kHz / 2U) - ORX_ADC_RATE_GUARD_REGION_KHZ)))
    {
        validRangeSynthesisBw = ADI_TRUE;
        boundaryDistanceUpper = (orxAdcSampleRate_kHz / 2U) - nyquistZone1SynthesisBwUpper;
        boundaryDistanceLower = nyquistZone1SynthesisBwLower;
    }

    /* If the signal lies in a odd nyquist zones (0.5*Fs to Fs, 1.5*Fs to 2*Fs, 2.5Fs to 3*Fs)
     * we need a +ve frequency shift at the NCO */
    if ((nyquistZone1SynthesisBwLower >= ((orxAdcSampleRate_kHz / 2U) + ORX_ADC_RATE_GUARD_REGION_KHZ)) &&
        (nyquistZone1SynthesisBwUpper <= (orxAdcSampleRate_kHz - ORX_ADC_RATE_GUARD_REGION_KHZ)))
    {
        validRangeSynthesisBw = ADI_TRUE;
        boundaryDistanceUpper = orxAdcSampleRate_kHz - nyquistZone1SynthesisBwUpper;
        boundaryDistanceLower = nyquistZone1SynthesisBwLower - (orxAdcSampleRate_kHz / 2U);
    }

    /* Case when the Synthesis Bandwidth is very close to the Fs or Fs/2 boundary*/
    if (validRangeSynthesisBw == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Invalid range for Tx synthesis bandwidth upper and lower frequencies");
        goto cleanup;
    }

    passbandEdgeShift = (int32_t)((HB_FILTER_PASSBAND_EDGE *
                                  device->initExtract.orx.orxChannelCfg[orxChannelIdx].orxOutputRate_kHz) -
                                  ((float)(txSynthesisBwUpper_kHz - centreTxSynthesisBw_kHz)));
    /* If passbandEdgeShift is less than 0, the synthesis BW is greater than the 88% bandwidth supported by the ORx*/
    if (passbandEdgeShift < 0)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txSynthesisBwUpper_kHz,
                               "Tx synthesis bandwidth is greater than the maximum supported by the ORX channel");
        goto cleanup;
    }

    if (boundaryDistanceLower < boundaryDistanceUpper)
    {
        /* If the closest image is on the lower side of the SBW, we shift the
         * observed synthesis BW to the lower edge the passband (- 0.44 * Fs)*/
        *ncoShiftFreqAdc_kHz = localNcoShiftFreqAdc_kHz - passbandEdgeShift;
        *ncoShiftFreqDatapath_kHz = passbandEdgeShift;
    }
    else
    {
        /* If the closest image is on the upper side of the SBW, we shift the
         * observed synthesis BW to the upper edge the passband (+ 0.44 * Fs)*/
        *ncoShiftFreqAdc_kHz = localNcoShiftFreqAdc_kHz + passbandEdgeShift;
        *ncoShiftFreqDatapath_kHz = -passbandEdgeShift;
    }

    /* Success */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingInit(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    

    if (device->devStateInfo.txToOrxMappingConfig.mode != ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        /* Initializes the RCI Tx To Orx Mapping Config, including its GPIOs */
        recoveryAction = adrv904x_TxToOrxMappingInit(device, &device->devStateInfo.txToOrxMappingConfig);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RCI Tx To Orx Mapping Init Failed");
            goto cleanup;
        }
    }
    else
    {
        /* Initializes the DFE Ctrl Tx To Orx Mapping Config, including its GPIOs */
        recoveryAction = adrv904x_DfeCtrlTxToOrxMappingInit(device, &device->devStateInfo.dfeCtrlTxToOrxMappingConfig);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "DFE Ctrl Tx To Orx Mapping Init Failed");
            goto cleanup;
        }
    }
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingConfigGet(adi_adrv904x_Device_t* const                  device,
                                                                      adi_adrv904x_TxToOrxMappingConfig_t* const    mappingConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    adi_adrv904x_TxToOrxMappingMode_e lclMode = ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL;
    
        /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common) ;
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, mappingConfig, cleanup);
    

    /* Check that Mapping Mode is NOT DFE CTRL. If it is, throw an error */
    lclMode = device->devStateInfo.txToOrxMappingConfig.mode;
    if (lclMode == ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lclMode,
                               "Device Tx to Orx Mapping is in DFE Ctrl Mode. This function can only be used in RCI mapping modes.");
        goto cleanup;
    }

    /* Grab configuration stored in the device handle*/
    ADI_LIBRARY_MEMCPY(mappingConfig, &device->devStateInfo.txToOrxMappingConfig, sizeof(adi_adrv904x_TxToOrxMappingConfig_t));

    /*Success*/
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingSet(adi_adrv904x_Device_t* const    device,
                                                                const uint8_t                   mapping)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t modeMask = 0U;
    

    /* Always set VSWR FWD direction for non-VSWR tx-to-orx mapping commands on both Orx0/1 */
    uint8_t vswrDirection = ((uint8_t)ADI_ADRV904X_DFE_VSWR_FWD << 0U) | ((uint8_t)ADI_ADRV904X_DFE_VSWR_FWD << 1U);
    
    /* Check that Mapping Mode is NOT DFE CTRL. If it is, throw an error */
    adi_adrv904x_TxToOrxMappingMode_e lclMode = device->devStateInfo.txToOrxMappingConfig.mode;
    if (lclMode == ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lclMode,
                               "Device Tx to Orx Mapping is in DFE Ctrl Mode. This function can only be used in RCI mapping modes.");
        goto cleanup;
    }
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Get modeMask from the mode*/
    switch (device->devStateInfo.txToOrxMappingConfig.mode)
    {
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_2BIT:
            modeMask = 0x03U;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_3BIT:
            modeMask = 0x07U;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_4BIT:
            modeMask = 0x0FU;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_6BIT:
            modeMask = 0x3FU;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_8BIT: /* Fallthrough */
        default:
            modeMask = 0xFFU;
            break;
    }

    /* Range check mapping byte against Mapping Mode */
    if ((mapping & modeMask) != mapping)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               mapping,
                               "Tx to Orx Mapping byte requested is not valid for use with the device's Mapping Mode configuration");
        goto cleanup;
    }

    /* Write mapping byte to scratch reg for the stream to readback */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_CPU_TX_ORX_MAPPING_SET_SCRATCH_REG_ID,
                                                    mapping);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting Tx to ORx mapping selection.");
        goto cleanup;
    }


    /* Always set VSWR FWD direction for non-VSWR tx-to-orx mapping commands */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_DFE_VSWR_DIRECTION_SCRATCH_REG_ID,
                                                    vswrDirection);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting Vswr direction scrathpad register");
        goto cleanup;
    }

    /* Trigger stream */
    recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_MAIN_TX_TO_ORX_MAP);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while triggering Core Stream to apply Tx to Orx mapping");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingGet(adi_adrv904x_Device_t* const        device,
                                                                const adi_adrv904x_RxChannels_e     orxChannel,
                                                                adi_adrv904x_TxChannels_e* const    txChannel)
{
        static const uint8_t NIBBLE_TX_MASK   = 0x07U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t readNibble = 0U;
    uint8_t txIdx = 0U;
    uint8_t noMapState = 0U;
    uint32_t txChanSel = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common) ;
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txChannel, cleanup);

    /* Range check orxChannel enum */
    if ((orxChannel != ADI_ADRV904X_ORX0) &&
        (orxChannel != ADI_ADRV904X_ORX1))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               orxChannel,
                               "ORx Channel selected is invalid. Must select ORx0-1.") ;
        goto cleanup;
    }

    /* Readback current mapping */
    if (orxChannel == ADI_ADRV904X_ORX0)
    {
        recoveryAction = adrv904x_Core_RadioControlInterfaceOrx0Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &readNibble);
    }
    else
    {
        recoveryAction = adrv904x_Core_RadioControlInterfaceOrx1Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &readNibble);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving current Tx to ORx map for selected ORx channel.");
        goto cleanup;
    }

    if (noMapState )
    {
        *txChannel = ADI_ADRV904X_TXOFF;
    }
    else
    {
        txIdx = NIBBLE_TX_MASK & readNibble;
        txChanSel = 1U << (uint32_t)txIdx;
        *txChannel = (adi_adrv904x_TxChannels_e)(txChanSel);
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenSet( adi_adrv904x_Device_t* const    device,
                                                                            const uint32_t                  mapping,
                                                                            const uint8_t                   presetAtten_dB,
                                                                            const uint8_t                   immediateUpdate)
{
        static const uint8_t MAX_ORX_ATTEN_DB = 16U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t txIdx = 0U;
    uint32_t txChannelMask = (mapping & (~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
    uint32_t extendedMappingFlag = (mapping & TX_TO_ORX_EXTENDED_MAPPING_FLAG);
    uint8_t tmpMask = 0U;
    adi_adrv904x_Channels_e tmpChannel = ADI_ADRV904X_CHOFF;
    uint8_t sendToCpu[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { ADI_FALSE, ADI_FALSE };
    uint8_t txChanMaskForCpu[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { 0U };
    uint32_t cpuIdx = 0U;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    adrv904x_CpuCmd_SetTxToOrxPresetAtten_t cmd;
    adrv904x_CpuCmd_SetTxToOrxPresetAttenResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_MAILBOX_ERROR;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv904x_CpuCmd_SetTxToOrxPresetAtten_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_SetTxToOrxPresetAttenResp_t));



    if (extendedMappingFlag != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                mapping,
                                "Requested Mapping is not supported");
        goto cleanup;

    }

    /* Check that requested mapping are valid */
    if ((txChannelMask  != (txChannelMask & 0x00FFU))       ||  /* Value Not in Range 0x00 to 0xFF */
        (mapping        == TX_TO_ORX_EXTENDED_MAPPING_FLAG) ||  /* Extended Flag & 0x00 Bitmask */
        (txChannelMask  == 0U))                                 /* 0x00 Bitmask */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txChannelMask,
                                "Requested Mapping is Invalid");
        goto cleanup;
    }

    /* Check preset value is valid */
    if (presetAtten_dB > MAX_ORX_ATTEN_DB)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetAtten_dB,
                                "presetAtten_dB outside maximum range");
        goto cleanup;
    }
    
    /* Check that immediateUpdate is valid */
    if ((immediateUpdate != ADI_TRUE) &&
        (immediateUpdate != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "immediateUpdate is invalid. Must be 0 or 1.");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid for State Mappings */
    if ((immediateUpdate        == ADI_TRUE)                            &&
        (extendedMappingFlag    == TX_TO_ORX_EXTENDED_MAPPING_FLAG))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "Immediate Update Cannot Be Performed with a State Mapping");
        goto cleanup;
    }

    if (extendedMappingFlag == TX_TO_ORX_EXTENDED_MAPPING_FLAG)
    {
        /* State Mapping Specific Command */
        cmd.extendedMappingFlag = ADI_TRUE;
        sendToCpu[0U]           = ADI_TRUE;
        cmd.chanSelect          = txChannelMask;
    }
    else
    {
        for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++txIdx)
        {
            /* Get channel number enum for this txIdx */
            tmpMask = (1U << txIdx);
            tmpMask = (tmpMask & ((uint8_t) txChannelMask));
            tmpChannel = (adi_adrv904x_Channels_e)(tmpMask);

            /* If channel enum is TXOFF, skip this channel */
            if (tmpChannel == ADI_ADRV904X_CHOFF)
            {
                continue;
            }

            /* Get the CPU assigned to this channel*/
            recoveryAction = adrv904x_CpuChannelMappingGet( device, tmpChannel, ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
                goto cleanup;
            }

            /* Set bit for target CPU */
            cpuIdx = (uint8_t)(cpuType);
            sendToCpu[cpuIdx] = ADI_TRUE;
            txChanMaskForCpu[cpuIdx] |= tmpMask;
        }
    }

    /* Loop through CPUs and send CPU cmd to only those required */
    for (cpuIdx = 0U; cpuIdx < (uint32_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO; cpuIdx++)
    {
        if (sendToCpu[cpuIdx] == ADI_TRUE)
        {
            /* Set cpuType */
            cpuType = (adi_adrv904x_CpuType_e)cpuIdx;

            /* Setup cmd-specific payload */
            if (extendedMappingFlag != TX_TO_ORX_EXTENDED_MAPPING_FLAG)
            {
                cmd.chanSelect = txChanMaskForCpu[cpuIdx];
            }
            cmd.presetAtten_dB = (uint8_t) presetAtten_dB;
            cmd.immediateUpdate = (uint8_t) immediateUpdate;

            /* Send command and receive response */
            recoveryAction = adrv904x_CpuCmdSend(   device,
                                                    cpuType,
                                                    ADRV904X_LINK_ID_0,
                                                    ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_ATTEN,
                                                    (void*)&cmd,
                                                    sizeof(cmd),
                                                    (void*)&cmdRsp,
                                                    sizeof(cmdRsp),
                                                    &cmdStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenGet_v2(  adi_adrv904x_Device_t* const                device,
                                                                                const adi_adrv904x_TxToOrxMappingPinTable_e mapping,
                                                                                uint8_t* const                              presetAtten_dB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegId = 0U;
    uint8_t scratchAttenVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, presetAtten_dB, cleanup);

    *presetAtten_dB = 0U;

    /* Check that requested Mapping is valid and determine scratchpadRegId for selected Mapping */
    switch (mapping)
    {
        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX0:
            scratchRegId = ADRV904X_CPU_TX_0_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX1:
            scratchRegId = ADRV904X_CPU_TX_1_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX2:
            scratchRegId = ADRV904X_CPU_TX_2_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX3:
            scratchRegId = ADRV904X_CPU_TX_3_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX4:
            scratchRegId = ADRV904X_CPU_TX_4_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX5:
            scratchRegId = ADRV904X_CPU_TX_5_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX6:
            scratchRegId = ADRV904X_CPU_TX_6_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX7:
            scratchRegId = ADRV904X_CPU_TX_7_ORX_ATTEN;
            break;


        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX0:
            scratchRegId = ADRV904X_CPU_MAPVAL_8_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX1:
            scratchRegId = ADRV904X_CPU_MAPVAL_9_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX2:
            scratchRegId = ADRV904X_CPU_MAPVAL_A_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX3:
            scratchRegId = ADRV904X_CPU_MAPVAL_B_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX4:
            scratchRegId = ADRV904X_CPU_MAPVAL_C_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX5:
            scratchRegId = ADRV904X_CPU_MAPVAL_D_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX6:
            scratchRegId = ADRV904X_CPU_MAPVAL_E_ATTEN;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX7:
            scratchRegId = ADRV904X_CPU_MAPVAL_F_ATTEN;
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mapping,
                                    "Requested Mapping is Invalid - Check adi_adrv904x_TxToOrxMapping_e for Valid Values");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    scratchRegId,
                                                    &scratchAttenVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Atten");
        goto cleanup;
    }

    /* Convert to dB, same as in OrxAttenGet*/
    recoveryAction = adrv904x_ORxTrmAttenToDb(device, scratchAttenVal, presetAtten_dB);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to resolve Orx Atten value in dB for Tx to Orx Preset");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetAttenGet( adi_adrv904x_Device_t* const    device,
                                                                            const adi_adrv904x_TxChannels_e txChannel,
                                                                            uint8_t* const                  presetAtten_dB)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegId = 0U;
    uint8_t scratchAttenVal = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, presetAtten_dB, cleanup);

    /* Check that requested Tx channel is valid and determine scratchpadRegId for selected channel */
    switch (txChannel)
    {
        case ADI_ADRV904X_TX0:
            scratchRegId = ADRV904X_CPU_TX_0_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX1:
            scratchRegId = ADRV904X_CPU_TX_1_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX2:
            scratchRegId = ADRV904X_CPU_TX_2_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX3:
            scratchRegId = ADRV904X_CPU_TX_3_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX4:
            scratchRegId = ADRV904X_CPU_TX_4_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX5:
            scratchRegId = ADRV904X_CPU_TX_5_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX6:
            scratchRegId = ADRV904X_CPU_TX_6_ORX_ATTEN;
            break;

        case ADI_ADRV904X_TX7:
            scratchRegId = ADRV904X_CPU_TX_7_ORX_ATTEN;
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     txChannel,
                                     "Requested Tx channel is invalid. Must be Tx Channel 0-7.");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    scratchRegId,
                                                    &scratchAttenVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Atten");
        goto cleanup;
    }

    /* Convert to dB, same as in OrxAttenGet*/
    recoveryAction = adrv904x_ORxTrmAttenToDb(device, scratchAttenVal, presetAtten_dB);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to resolve Orx Atten value in dB for Tx to Orx Preset");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoSet(   adi_adrv904x_Device_t* const                        device,
                                                                            const uint32_t                                      mapping,
                                                                            const adi_adrv904x_TxToOrxMappingPresetNco_t* const presetNco,
                                                                            const uint8_t                                       immediateUpdate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t txIdx = 0U;
    uint32_t txChannelMask = (mapping & (~TX_TO_ORX_EXTENDED_MAPPING_FLAG));
    uint32_t extendedMappingFlag = (mapping & TX_TO_ORX_EXTENDED_MAPPING_FLAG);
    uint8_t tmpMask = 0U;
    adi_adrv904x_Channels_e tmpChannel = ADI_ADRV904X_CHOFF;
    uint8_t sendToCpu[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { ADI_FALSE, ADI_FALSE };
    uint8_t txChanMaskForCpu[ADI_ADRV904X_CPU_TYPE_MAX_RADIO] = { 0U };
    uint32_t cpuIdx = 0U;
    adi_adrv904x_CpuType_e cpuType = ADI_ADRV904X_CPU_TYPE_UNKNOWN;

    adrv904x_CpuCmd_SetTxToOrxPresetNco_t cmd;
    adrv904x_CpuCmd_SetTxToOrxPresetNcoResp_t cmdRsp;
    adrv904x_CpuCmdStatus_e cmdStatus = ADRV904X_CPU_CMD_STATUS_GENERIC;
    adrv904x_CpuErrorCode_e cpuErrorCode = ADRV904X_CPU_SYSTEM_MAILBOX_ERROR;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common) ;
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(&cmd, 0, sizeof(adrv904x_CpuCmd_SetTxToOrxPresetNco_t));
    ADI_LIBRARY_MEMSET(&cmdRsp, 0, sizeof(adrv904x_CpuCmd_SetTxToOrxPresetNcoResp_t));



    if (extendedMappingFlag != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                mapping,
                                "Requested Mapping is not supported");
        goto cleanup;
    }

    /* Check that requested mapping are valid */
    if ((txChannelMask  != (txChannelMask & 0x00FFU))       ||  /* Value Not in Range 0x00 to 0xFF */
        (mapping        == TX_TO_ORX_EXTENDED_MAPPING_FLAG) ||  /* Extended Flag & 0x00 Bitmask */
        (txChannelMask  == 0U))                                 /* 0x00 Bitmask */
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                txChannelMask,
                                "Requested Mapping is Invalid");
        goto cleanup;
    }

    /* Check preset adc nco freq value is valid */
    if ((presetNco->ncoFreqAdc_Khz > 8388607) ||
        (presetNco->ncoFreqAdc_Khz < -8388608))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetNco->ncoFreqAdc_Khz,
                                "presetNco->ncoFreqAdc_Khz outside maximum range. Must be in range -8388608 to 8388607");
        goto cleanup;
    }

    /* Check preset datapath nco freq value is valid */
    if ((presetNco->ncoFreqDatapath_Khz > 8388607) ||
        (presetNco->ncoFreqDatapath_Khz < -8388608))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                presetNco->ncoFreqDatapath_Khz,
                                "presetNco->ncoFreqDatapath_Khz outside maximum range. Must be in range -8388608 to 8388607");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid */
    if ((immediateUpdate != ADI_TRUE) &&
        (immediateUpdate != ADI_FALSE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "immediateUpdate is invalid. Must be 0 or 1.");
        goto cleanup;
    }

    /* Check that immediateUpdate is valid for State Mappings */
    if ((immediateUpdate        == ADI_TRUE)                            &&
        (extendedMappingFlag    == TX_TO_ORX_EXTENDED_MAPPING_FLAG))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                immediateUpdate,
                                "Immediate Update Cannot Be Performed with a State Mapping");
        goto cleanup;
    }

    if (extendedMappingFlag == TX_TO_ORX_EXTENDED_MAPPING_FLAG)
    {
        /* State Mapping Specific Command */
        cmd.extendedMappingFlag = ADI_TRUE;
        sendToCpu[0U]           = ADI_TRUE;
        cmd.chanSelect          = txChannelMask;
    }
    else
    {
        for (txIdx = 0U; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; ++txIdx)
        {
            /* Get channel number enum for this txIdx */
            tmpMask = (1U << txIdx);
            tmpMask &= txChannelMask;
            tmpChannel = (adi_adrv904x_Channels_e)(tmpMask);

            /* If channel enum is TXOFF, skip this channel */
            if (tmpChannel == ADI_ADRV904X_CHOFF)
            {
                continue;
            }

            /* Get the CPU assigned to this channel */
            recoveryAction = adrv904x_CpuChannelMappingGet(device, tmpChannel, ADRV904X_CPU_OBJID_SYSTEM_END, &cpuType);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to get CPU channel mapping");
                goto cleanup;
            }

            /* Set bit for target CPU */
            cpuIdx = (uint8_t)(cpuType);
            sendToCpu[cpuIdx] = ADI_TRUE;
            txChanMaskForCpu[cpuIdx] |= tmpMask;
        }
    }

    /* Loop through CPUs and send ARM cmd to only those required */
    for (cpuIdx = 0U; cpuIdx < (uint32_t)ADI_ADRV904X_CPU_TYPE_MAX_RADIO; ++cpuIdx)
    {
        if (sendToCpu[cpuIdx] == ADI_TRUE)
        {
            /* Set cpuType */
            cpuType = (adi_adrv904x_CpuType_e)cpuIdx;

            /* Setup cmd-specific payload */
            if (extendedMappingFlag != TX_TO_ORX_EXTENDED_MAPPING_FLAG)
            {
                cmd.chanSelect = txChanMaskForCpu[cpuIdx];
            }
            cmd.ncoFreqAdc_Khz = ADRV904X_HTOCL(presetNco->ncoFreqAdc_Khz);
            cmd.ncoFreqDatapath_Khz = ADRV904X_HTOCL(presetNco->ncoFreqDatapath_Khz);
            cmd.immediateUpdate = immediateUpdate;

            /* Send command and receive response */
            recoveryAction = adrv904x_CpuCmdSend(   device,
                                                    cpuType,
                                                    ADRV904X_LINK_ID_0,
                                                    ADRV904X_CPU_CMD_ID_SET_TX_TO_ORX_PRESET_NCO,
                                                    (void*)&cmd,
                                                    sizeof(cmd),
                                                    (void*)&cmdRsp,
                                                    sizeof(cmdRsp),
                                                    &cmdStatus);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_ADRV904X_CPU_CMD_RESP_CHECK_GOTO(cmdRsp.status, cmdStatus, cpuErrorCode, recoveryAction, cleanup);
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoGet_v2(adi_adrv904x_Device_t* const                    device,
                                                                            const adi_adrv904x_TxToOrxMappingPinTable_e     mapping,
                                                                            adi_adrv904x_TxToOrxMappingPresetNco_t* const   presetNco)
{
        const uint8_t NCO_NUM_BYTES = 3U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegIdStart_adc = 0U;
    uint8_t scratchRegIdStart_dp = 0U;
    uint8_t tmpByte = 0U;
    uint32_t tmpAdc = 0U;
    uint32_t tmpDp = 0U;
    uint8_t idx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(presetNco, 0, sizeof(adi_adrv904x_TxToOrxMappingPresetNco_t));

    /* Check that requested Tx channel is valid and determine scratchpadRegIdStart for selected channel */
    switch (mapping)
    {
        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX0:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX1:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX2:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX3:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX4:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX5:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX6:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_ORX_MAPPING_PIN_TABLE_TX7:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB;
            break;


        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX0:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_8_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_8_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX1:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_9_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_9_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX2:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_A_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_A_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX3:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_B_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_B_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX4:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_C_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_C_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX5:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_D_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_D_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX6:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_E_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_E_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX_TO_ORX_VSWR_MAPPING_TX7:
            scratchRegIdStart_adc = ADRV904X_CPU_MAPVAL_F_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_MAPVAL_F_NCO_DP_FREQ_MSB;
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                    recoveryAction,
                                    mapping,
                                    "Requested Mapping is Invalid - Check adi_adrv904x_TxToOrxMappingPinTable_e for Valid Values");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad values */
    for (idx = 0U; idx < NCO_NUM_BYTES; ++idx)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_adc - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx ADC NCO");
            goto cleanup;
        }
        tmpAdc <<= 8U;
        tmpAdc |= ((uint32_t)tmpByte);

        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_dp - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Datapath NCO");
            goto cleanup;
        }
        tmpDp <<= 8U;
        tmpDp |= ((uint32_t)tmpByte);
    }

    /* Readback values are 24bits. Returned results are int32_t, so sign extend data as needed */
    if ((tmpAdc & 0x00800000U) != 0U)
    {
        tmpAdc |= 0xFF000000U;
    }
    
    if ((tmpDp & 0x00800000U) != 0U)
    {
        tmpDp |= 0xFF000000U;
    }

    /* Store results */
    presetNco->ncoFreqAdc_Khz = (int32_t)tmpAdc;
    presetNco->ncoFreqDatapath_Khz = (int32_t)tmpDp;

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingPresetNcoGet(   adi_adrv904x_Device_t* const                    device,
                                                                            const adi_adrv904x_TxChannels_e                 txChannel,
                                                                            adi_adrv904x_TxToOrxMappingPresetNco_t* const   presetNco)
{
        const uint8_t NCO_NUM_BYTES = 3U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchRegIdStart_adc = 0U;
    uint8_t scratchRegIdStart_dp = 0U;
    uint8_t tmpByte = 0U;
    uint32_t tmpAdc = 0U;
    uint32_t tmpDp = 0U;
    uint8_t idx = 0U;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, presetNco, cleanup);

    ADI_LIBRARY_MEMSET(presetNco, 0, sizeof(adi_adrv904x_TxToOrxMappingPresetNco_t));

    /* Check that requested Tx channel is valid and determine scratchpadRegIdStart for selected channel */
    switch (txChannel)
    {
        case ADI_ADRV904X_TX0:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_0_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_0_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX1:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_1_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_1_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX2:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_2_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_2_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX3:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_3_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_3_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX4:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_4_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_4_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX5:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_5_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_5_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX6:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_6_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_6_ORX_NCO_DP_FREQ_MSB;
            break;

        case ADI_ADRV904X_TX7:
            scratchRegIdStart_adc = ADRV904X_CPU_TX_7_ORX_NCO_ADC_FREQ_MSB;
            scratchRegIdStart_dp = ADRV904X_CPU_TX_7_ORX_NCO_DP_FREQ_MSB;
            break;

        default:
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                     recoveryAction,
                                     txChannel,
                                     "Requested Tx channel is invalid. Must be Tx Channel 0-7.");
            goto cleanup;
            break;
    }

    /* Readback Scratchpad values */
    for (idx = 0U; idx < NCO_NUM_BYTES; idx++)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_adc - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx ADC NCO");
            goto cleanup;
        }
        tmpAdc <<= 8U;
        tmpAdc |= ((uint32_t) tmpByte);

        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                        (scratchRegIdStart_dp - idx),
                                                        &tmpByte);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read Tx to Orx Preset value for Orx Datapath NCO");
            goto cleanup;
        }
        tmpDp <<= 8U;
        tmpDp |= ((uint32_t) tmpByte);
    }

    /* Readback values are 24bits. Returned results are int32_t, so sign extend data as needed */
    if ((tmpAdc & 0x00800000U) != 0U)
    {
        tmpAdc |= 0xFF000000U;
    }
    
    if ((tmpDp & 0x00800000U) != 0U)
    {
        tmpDp |= 0xFF000000U;
    }

    /* Store results */
    presetNco->ncoFreqAdc_Khz = (int32_t)tmpAdc;
    presetNco->ncoFreqDatapath_Khz = (int32_t)tmpDp;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamVersionGet(adi_adrv904x_Device_t* const        device,
                                                               adi_adrv904x_Version_t* const      streamVersion)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t ver[4U] = { 0U };
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamVersion, cleanup);

    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_STREAMLOADED) != ADI_ADRV904X_STATE_STREAMLOADED)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Stream binary must be loaded before getting stream version");
        goto cleanup;
    }

    /* Read the StreamVersion */
    recoveryAction = adi_adrv904x_Registers32Read(device, NULL, ADI_ADRV904X_STREAM_VERSION_ADDR, ver, NULL, 4U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue during Reading Stream Version registers");
        goto cleanup;
    }
    /* Store the StreamVersion */
    streamVersion->majorVer       = ver[0U];
    streamVersion->minorVer       = ver[1U];
    streamVersion->maintenanceVer = ver[2U];
    streamVersion->buildVer       = ver[3U];

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigSet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx)
{

        static const uint32_t txScratchPad2[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad2[] = { (uint32_t)ADRV904X_ADDR_RX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX7_STREAM_SCRATCH2 };

    static const uint32_t TX_SCRATCH2_WRITE_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_SHIFT = 24U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2WriteValue = 0U;
    uint32_t rxScratchWriteValue = 0U;
    adi_adrv904x_RxGain_t rxGain;
    uint16_t attenRegVal = 0U;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    adrv904x_BfTxCfrRegChanAddr_e txCfrBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_DFE_TX_CFR_TX_CFR_REG;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    /*Check that if requested Rx Channel valid*/
    if ((configRx->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configRx->rxChannelMask,
                               "Invalid channel is selected for Rx config. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if ((configTx->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid channel is selected for Tx config. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    txScratch2WriteValue = (uint32_t)(configTx->txNcoFreqKhz) & TX_SCRATCH2_WRITE_MASK;
    /* Change the sign of the frequency because there is an I/Q swap in the HW */
    rxScratchWriteValue = (((uint32_t)configRx->rxGainIndex & 0x000000FF) << RX_SCRATCH_WRITE_SHIFT) |
                           ((uint32_t)(-configRx->rxNcoFreqKhz) & 0x00FFFFFF);

    /* for each Tx channel in txChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_TXCHANNELS; chanId++)
    {
        if ((configTx->txChannelMask & (1U << chanId)) == 0U)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        if (configTx->txAttenuation_mdB > ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   configTx->txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        chanSel = (1U << chanId);
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }


        recoveryAction = adrv904x_CfrRegsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txCfrBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine CFR base address");
            goto cleanup;
        }

        /* Conversion from the requested atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        attenRegVal = (configTx->txAttenuation_mdB / ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);

        recoveryAction = adrv904x_TxFuncs_TxAttenuation_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
            goto cleanup;
        }


        /* storing Tx attenuation for AC. This value is only used when SBET is enabled */
         recoveryAction = adrv904x_TxCfrReg_Cfr1Spare_BfSet(device,
                                                                NULL,
                                                                txCfrBaseAddr,
                                                                (uint32_t)attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Cfr1 Spare register");
            goto cleanup;
        }

        recoveryAction = adi_adrv904x_Register32Write(device, NULL, txScratchPad2[chanId], txScratch2WriteValue, TX_SCRATCH2_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 2");
            goto cleanup;
        }
    }

    /* for each Rx channel in rxChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_RX_ONLY; chanId++)
    {
        if ((configRx->rxChannelMask & (1U << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        rxGain.gainIndex = configRx->rxGainIndex;
        rxGain.rxChannelMask = configRx->rxChannelMask;

        recoveryAction = adrv904x_RxGainSetRangeCheck(device, &rxGain, 1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx gain is out of range");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, rxScratchPad2[chanId], rxScratchWriteValue, RX_SCRATCH_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 2");
            goto cleanup;
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigGet(adi_adrv904x_Device_t* const device,
                                                                       adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                       adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx)
{

        static const uint32_t txScratchPad2[] = {
        (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH2
    };

    static const uint32_t rxScratchPad2[] = {
        (uint32_t)ADRV904X_ADDR_RX0_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX1_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX2_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX3_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX4_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX5_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX6_STREAM_SCRATCH2,
        (uint32_t)ADRV904X_ADDR_RX7_STREAM_SCRATCH2
    };

    static const uint32_t TX_SCRATCH2_READ_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH2_READ_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH2_READ_SHIFT = 24U;
    static const uint32_t NCO_SIGN_MASK = 0x00800000U;
    
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2ReadValue = 0U;
    uint32_t rxScratch2ReadValue = 0U;
    uint16_t attenRegVal = 0U;
    uint32_t txChanId = 0U;
    uint32_t rxChanId = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    if ((configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RXOFF) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX0) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX1) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX2) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX3) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX4) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX5) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX6) &&
        (configRx->rxChannelMask != (uint32_t)ADI_ADRV904X_RX7))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   configRx->rxChannelMask,
                                   "Invalid Rx channel is selected. User should select one channel or CHOFF only");
            goto cleanup;
        }
    
    if ((configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TXOFF) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX0) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX1) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX2) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX3) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX4) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX5) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX6) &&
        (configTx->txChannelMask != (uint32_t)ADI_ADRV904X_TX7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid Tx channel is selected. User should select one channel or CHOFF only");
        goto cleanup;
    }

    if (configTx->txChannelMask == (uint32_t)ADI_ADRV904X_TXOFF) 
    {
        configTx->txAttenuation_mdB = 0U;
        configTx->txNcoFreqKhz = 0;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        chanSel = configTx->txChannelMask;
        txChanId = adrv904x_TxChannelsToId((adi_adrv904x_TxChannels_e)chanSel);
        
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanSel, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_TxFuncs_TxAttenuation_BfGet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              &attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to read Tx slice attenuation");
            goto cleanup;
        }
        
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, txScratchPad2[txChanId], &txScratch2ReadValue, TX_SCRATCH2_READ_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 2");
            goto cleanup;
        }

        if ((txScratch2ReadValue & NCO_SIGN_MASK) > 0)
        {
            /* handle the negative case */
            configTx->txNcoFreqKhz = (int32_t)((~txScratch2ReadValue & 0x00FFFFFF) + 1) * (-1);
        }
        else
        {
            /* handle the positive case */
            configTx->txNcoFreqKhz = (int32_t)(txScratch2ReadValue & 0x00FFFFFF);
        }

        /* Conversion from the atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        configTx->txAttenuation_mdB = (attenRegVal * ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);
    }
    
    if (configRx->rxChannelMask == (uint32_t)ADI_ADRV904X_RXOFF)
    {
        configRx->rxGainIndex = 0U;
        configRx->rxNcoFreqKhz = 0;
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    }
    else
    {
        chanSel = configRx->rxChannelMask;
        rxChanId = adrv904x_RxChannelsToId((adi_adrv904x_RxChannels_e)chanSel);
    
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, rxScratchPad2[rxChanId], &rxScratch2ReadValue, RX_SCRATCH2_READ_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Rx scratch pad register 2");
            goto cleanup;
        }

        if ((rxScratch2ReadValue & NCO_SIGN_MASK) > 0)
        {
            /* handle the negative case */
            configRx->rxNcoFreqKhz = (int32_t)((~rxScratch2ReadValue & 0x00FFFFFF) + 1) * (-1);
        }
        else
        {
            /* handle the positive case */
            configRx->rxNcoFreqKhz = (int32_t)(rxScratch2ReadValue & 0x00FFFFFF);
        }
        configRx->rxNcoFreqKhz *= (-1); /* Change the sign of the frequency because there is an I/Q swap in the HW */
        
        configRx->rxGainIndex  = (rxScratch2ReadValue >> RX_SCRATCH2_READ_SHIFT);
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalErrorGet(adi_adrv904x_Device_t* const device,
                                                                       const uint32_t channelSel,
                                                                       uint8_t * const errStatus)
{
        static const uint32_t txScratchPad1[] = {
        (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH1,
        (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH1
    };

    static const uint32_t rxScratchPad0[] = {
        (uint32_t)ADRV904X_ADDR_RX0_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX1_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX2_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX3_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX4_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX5_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX6_STREAM_SCRATCH0,
        (uint32_t)ADRV904X_ADDR_RX7_STREAM_SCRATCH0
    };

    static const uint32_t TX_ANT_CAL_ERROR_READ_MASK  = 0x000000D8U;
    static const uint32_t RX_ANT_CAL_ERROR_READ_MASK  = 0x00000CC0U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch1ReadValue = 0U;
    uint32_t rxScratch0ReadValue = 0U;
    uint32_t chanId = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, errStatus, cleanup);

    if ((channelSel != (uint32_t)ADI_ADRV904X_CH0) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH1) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH2) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH3) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH4) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH5) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH6) &&
        (channelSel != (uint32_t)ADI_ADRV904X_CH7))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               channelSel,
                               "Invalid channel is selected. User should select one channel only");
        goto cleanup;
            
    }

    chanId = adrv904x_TxChannelsToId((adi_adrv904x_TxChannels_e)channelSel);
    
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, txScratchPad1[chanId], &txScratch1ReadValue, TX_ANT_CAL_ERROR_READ_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Tx scratch pad register 1");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_Register32Read(device, NULL, rxScratchPad0[chanId], &rxScratch0ReadValue, RX_ANT_CAL_ERROR_READ_MASK);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot read Rx scratch pad register 0");
        goto cleanup;
    }

    /* In scratch register, Bit 3-4 has Tx Low High stream errors - Bit 6-7 has Tx Ant Cal Low High stream errors */
    txScratch1ReadValue = ((txScratch1ReadValue & 0xC0U) >> 6U) | ((txScratch1ReadValue & 0x18U) >> 1U);

    /* In scratch register, Bit 10-11 has Rx Low High stream errors - Bit 6-7 has Tx Ant Cal Low High stream errors */
    rxScratch0ReadValue = ((rxScratch0ReadValue & 0xC0U) >> 6U) | ((rxScratch0ReadValue & 0xC00U) >> 8U);

    /* Store Tx error bits in LSB and Rx error bits in MSB */
    *errStatus = (txScratch1ReadValue) | (rxScratch0ReadValue << 4U);

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_StreamProcErrorGet(   adi_adrv904x_Device_t* const            device,
                                                                    adi_adrv904x_StreamErrArray_t* const    streamErr)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t isErrInStream = 0U;
    uint16_t streamErrorCode = 0U; 
    uint8_t i = 0U;

    /*struct to aid separate processor type and id for each stream processor*/
    typedef struct adrv904x_streamProcAttribs
    {
        adrv904x_BfStreamprocChanAddr_e chanAddr;
        adi_adrv904x_StreamProcType_e   procType;
        uint8_t                         procId;
    } adrv904x_streamProcAttribs_t;

    /*Creates the NUM_STREAM_PROCS size array for the stream processors. 
        *Each element is a struct that has the address, the type and the id of the stream processor*/
    static const adrv904x_streamProcAttribs_t streamProcAttribs[ADI_ADRV904X_STREAM_MAX] = {
        {ADRV904X_BF_DIGITAL_CORE_MAIN_STREAM_PROC,     ADI_ADRV904X_STREAM_MAIN, 0},
        {ADRV904X_BF_DIGITAL_CORE_KFA_STREAM_PROC_REGS, ADI_ADRV904X_STREAM_KFA, 0},
        {ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 0},
        {ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 1},
        {ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 2},
        {ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 3},
        {ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 4},
        {ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 5},
        {ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 6},
        {ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_RX, 7},
        {ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 0},
        {ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 1},
        {ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 2},
        {ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 3},
        {ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 4},
        {ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 5},
        {ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 6},
        {ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_TX, 7},
        {ADRV904X_BF_SLICE_ORX_0__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_ORX, 0},
        {ADRV904X_BF_SLICE_ORX_1__SLICE_AHB_STREAM_PROC, ADI_ADRV904X_STREAM_ORX, 1}
    };

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, streamErr, cleanup);

    ADI_LIBRARY_MEMSET(streamErr, 0, sizeof(adi_adrv904x_StreamErrArray_t));

    for (i = 0U; i < ADI_ADRV904X_STREAM_MAX; i++)
    {
        /*Initializes the streamErr with the streamProcType and streamProcId for each stream processor*/
        streamErr->streamProcArray[i].streamProcType = streamProcAttribs[i].procType;
        streamErr->streamProcArray[i].streamProcId = streamProcAttribs[i].procId;

        recoveryAction = adrv904x_Streamproc_DbgRdbkMode_BfSet(device, NULL, streamProcAttribs[i].chanAddr, 1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue setting Stream Error to volatile status");
            goto cleanup;
        }
        
        recoveryAction = adrv904x_Streamproc_StreamError_BfGet(device, NULL, streamProcAttribs[i].chanAddr, &isErrInStream);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading status error bit");
            goto cleanup;
        }
        if (isErrInStream == ADI_TRUE)
        {
            /* Since an error was detected, the stream number that triggered the error and the error 
                * code are saved in the stream processor struct. After storing these informations, the error
                * status is reset*/
            recoveryAction = adrv904x_Streamproc_ErroredStreamNumber_BfGet( device, 
                                                                            NULL, 
                                                                            streamProcAttribs[i].chanAddr,
                                                                            &streamErr->streamProcArray[i].erroredStreamNumber);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading the stream number that triggered the error");
                goto cleanup;
            }
            recoveryAction = adrv904x_Streamproc_RdbkErrorVal_BfGet(device,
                                                                    NULL,
                                                                    streamProcAttribs[i].chanAddr,
                                                                    &streamErrorCode);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue reading the error code for the stream error");
                goto cleanup;
            }
            streamErr->streamProcArray[i].streamErrEnum = (adi_adrv904x_StreamError_e)streamErrorCode;

            /*Clear the error in streams:*/
            recoveryAction = adrv904x_Streamproc_ErroredStreamNumber_BfSet( device, 
                                                                            NULL, 
                                                                            streamProcAttribs[i].chanAddr, 
                                                                            0xFU); /*is cleared when writing a dummy value*/ 
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue resetting the stream error number");
                goto cleanup;
            }
            recoveryAction = adrv904x_Streamproc_StreamError_BfSet( device,
                                                                    NULL,
                                                                    streamProcAttribs[i].chanAddr,
                                                                    1U); /*Writing 1 clears error bit*/ 
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Issue resetting the status error bit");
                goto cleanup;
            }
        }
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioChannelSet(adi_adrv904x_Device_t* const device,
                                                                            const uint32_t txChannelMask,
                                                                            const uint32_t rxChannelMask) 
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    if ((txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "Invalid Tx channel is selected. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }
    
    if ((rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               rxChannelMask,
                               "Invalid Rx channel is selected. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    /* Write Tx Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_GPIO_ANTCAL_TX_MASK,
                                                    (uint8_t)txChannelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write GPIO antenna cal Tx selection to scratchpad register");
        goto cleanup;
    }

    /* Write Rx Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_GPIO_ANTCAL_RX_MASK,
                                                    (uint8_t)rxChannelMask);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write GPIO antenna cal Rx selection to scratchpad register");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioChannelGet(adi_adrv904x_Device_t* const device,
                                                                            uint32_t * const txChannelMask,
                                                                            uint32_t * const rxChannelMask)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t scratchPadValue = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txChannelMask, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, rxChannelMask, cleanup);

    /* Read Tx Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_GPIO_ANTCAL_TX_MASK,
                                                    &scratchPadValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read GPIO antenna cal Tx selection to scratchpad register");
        goto cleanup;
    }
    *txChannelMask = (uint32_t)scratchPadValue;

    /* Read Rx Scratchpad value */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_GPIO_ANTCAL_RX_MASK,
                                                    &scratchPadValue);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to read GPIO antenna cal Rx selection to scratchpad register");
        goto cleanup;
    }
    *rxChannelMask = (uint32_t)scratchPadValue;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
    
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalConfigSet_v2(adi_adrv904x_Device_t* const device,
                                                                          adi_adrv904x_RxRadioCtrlAntennaCalConfig_t * const configRx,
                                                                          adi_adrv904x_TxRadioCtrlAntennaCalConfig_t * const configTx,
                                                                          const uint8_t                                      rxGainValue,
                                                                          const uint8_t                                      enableFreeze)
{

        static const uint32_t txScratchPad2[] = { (uint32_t)ADRV904X_ADDR_TX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_TX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad2[] = { (uint32_t)ADRV904X_ADDR_RX0_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX1_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX2_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX3_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX4_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX5_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX6_STREAM_SCRATCH2,
                                              (uint32_t)ADRV904X_ADDR_RX7_STREAM_SCRATCH2 };

    static const uint32_t rxScratchPad5[] = { (uint32_t)ADRV904X_ADDR_RX0_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX1_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX2_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX3_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX4_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX5_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX6_STREAM_SCRATCH5,
                                              (uint32_t)ADRV904X_ADDR_RX7_STREAM_SCRATCH5 };

    static const uint32_t TX_SCRATCH2_WRITE_MASK  = 0x00FFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_MASK  = 0xFFFFFFFFU;
    static const uint32_t RX_SCRATCH_WRITE_SHIFT = 24U;
    static const uint32_t RX_SCRATCH5_GAIN_SHIFT = 8U;
    static const uint32_t RX_SCRATCH5_WRITE_MASk = 0xFF01U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t txScratch2WriteValue = 0U;
    uint32_t rxScratchWriteValue = 0U;
    uint32_t rxScratch5WriteValue = 0U;
    adi_adrv904x_RxGain_t rxGain;
    uint16_t attenRegVal = 0U;
    uint32_t chanId = 0U;
    uint32_t chanSel = 0U;
    adrv904x_BfTxFuncsChanAddr_e txBaseAddr = ADRV904X_BF_SLICE_TX_0__TX_FUNCS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configRx, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, configTx, cleanup);

    ADI_LIBRARY_MEMSET(&rxGain, 0, sizeof(rxGain));

    /* Check enable parameter is valid */
    if ((enableFreeze != ADI_ENABLE) && (enableFreeze != ADI_DISABLE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               enableFreeze,
                               "enableFreeze parameter must be either 0 for disabled or 1 for enabled");
        goto cleanup;
    }

    /*Check that if requested Rx Channel valid*/
    if ((configRx->rxChannelMask & (~(uint32_t)ADI_ADRV904X_RX_MASK_ALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configRx->rxChannelMask,
                               "Invalid channel is selected for Rx config. Valid values are 0 or any combinations of Rx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    if ((configTx->txChannelMask & (~(uint32_t)ADI_ADRV904X_TXALL)) != 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               configTx->txChannelMask,
                               "Invalid channel is selected for Tx config. Valid values are 0 or any combinations of Tx0/1/2/3/4/5/6/7");
        goto cleanup;
    }

    txScratch2WriteValue = (uint32_t)(configTx->txNcoFreqKhz) & TX_SCRATCH2_WRITE_MASK;
    /* Change the sign of the frequency because there is an I/Q swap in the HW */
    rxScratchWriteValue = (((uint32_t)configRx->rxGainIndex & 0x000000FF) << RX_SCRATCH_WRITE_SHIFT) |
                           ((uint32_t)(-configRx->rxNcoFreqKhz) & 0x00FFFFFF);

    /* for each Tx channel in txChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_TXCHANNELS; chanId++)
    {
        if ((configTx->txChannelMask & (1U << chanId)) == 0U)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        if (configTx->txAttenuation_mdB > ADRV904X_TX_ATTEN_VALUE_MILLI_DB_MAX)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   configTx->txAttenuation_mdB,
                                   "Invalid txAttenuation_mdB value");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        chanSel = (1U << chanId);
        recoveryAction = adrv904x_TxFuncsBitfieldAddressGet(device, (adi_adrv904x_TxChannels_e)(chanSel), &txBaseAddr);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, chanId, "Invalid Tx Channel used to determine SPI address");
            goto cleanup;
        }

        /* Conversion from the atten level (milli-dB) to equivalent
         * TxAttenTable index is always done based on a step size of 0.05.
         * Other step sizes are not supported. */
        attenRegVal = (configTx->txAttenuation_mdB / ADRV904X_TX_ATTEN_STEP_SIZE_DIV_0P05);

        recoveryAction = adrv904x_TxFuncs_TxAttenuation_BfSet(device,
                                                              NULL,
                                                              txBaseAddr,
                                                              attenRegVal);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to write Tx PA Protect Input Select");
            goto cleanup;
        }

        recoveryAction = adi_adrv904x_Register32Write(device, NULL, txScratchPad2[chanId], txScratch2WriteValue, TX_SCRATCH2_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Tx scratch pad register 2");
            goto cleanup;
        }
    }

    /* for each Rx channel in rxChannelMask */
    for (chanId = 0U; chanId < ADI_ADRV904X_MAX_RX_ONLY; chanId++)
    {
        /* Write freeze and gainvalue to each scratchpad register */
        rxScratch5WriteValue = (((uint32_t)enableFreeze)  | ((uint32_t)rxGainValue) << RX_SCRATCH5_GAIN_SHIFT);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, rxScratchPad5[chanId], rxScratch5WriteValue, RX_SCRATCH5_WRITE_MASk);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 5");
            goto cleanup;
        }

        if ((configRx->rxChannelMask & (1U << chanId)) == 0)
        {
            /* Skip channel. It's not in chanMask. */
            continue;
        }

        rxGain.gainIndex = configRx->rxGainIndex;
        rxGain.rxChannelMask = configRx->rxChannelMask;

        recoveryAction = adrv904x_RxGainSetRangeCheck(device, &rxGain, 1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Rx gain is out of range");
            goto cleanup;
        }

        /* ChanId is selected in chanMask */
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, rxScratchPad2[chanId], rxScratchWriteValue, RX_SCRATCH_WRITE_MASK);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Cannot write Rx scratch pad register 2");
            goto cleanup;
        }

    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerImageWrite(adi_adrv904x_Device_t* const    device,
                                                                       uint32_t                        byteOffset,
                                                                       const uint8_t                   binary[],
                                                                       uint32_t                        byteCount)
{
        adi_adrv904x_ErrAction_e  recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_RadioSeqImageHeader_t rsImageHeader;
    uint32_t imageSize = 0U;
    uint32_t imageOffset = 0U;
    uint32_t i = 0U;
    uint8_t finalCrc = 0U;    
    uint8_t binaryZero[4] = {0U};    
    uint32_t xbarImageSize = 0U;   
    uint32_t lookupImageSize = 0U; 
    uint32_t sequenceImageSize = 0U; 
    uint32_t xbarImageOffset = 0U; 
    uint32_t lookupImageOffset = 0U; 
    uint32_t sequenceImageOffset = 0U;
    uint32_t address[1] = { 0U };
    static const uint32_t RS_IMAGE_VERSION_ID_START_BYTE = ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, revision);

    static const uint32_t FINAL_RS_IMAGE_HEADER_SIZE_BYTES    = sizeof(adi_adrv904x_RadioSeqImageHeader_t); 


    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, binary, cleanup);

    if ((byteCount == 0U) ||
        ((byteCount % 4U) > 0U))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               byteCount,
                               "Invalid byteCount. Must be multiple of 4.");
        goto cleanup;
    }
    if ((byteOffset % 4U) > 0U)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               byteCount,
                               "Invalid byteOffset. Must be multiple of 4.");
        goto cleanup;
    }

    /* When byteOffset is zero that indicates the first time this function.
     * has been called for a particular radioSeqBinary.bin so the header info
     * must be extracted. We need all the information extracted in a single
     * call which is why min chunk size is the same as FINAL_RS_IMAGE_HEADER_SIZE_BYTES.
     * 
     */
    if (byteOffset == 0U)
    {
        if (byteCount < FINAL_RS_IMAGE_HEADER_SIZE_BYTES)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   byteCount,
                                   "Must pass in chunk size greater than current radio sequencer image header.");
            goto cleanup;
        }

        rsImageHeader.magicNumber = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, magicNumber)], 4U);
        rsImageHeader.revision.majorVer = adrv904x_CpuIntFromBytesGet(binary + RS_IMAGE_VERSION_ID_START_BYTE + 0U, 4U);
        rsImageHeader.revision.minorVer = adrv904x_CpuIntFromBytesGet(binary + RS_IMAGE_VERSION_ID_START_BYTE + 4U, 4U);
        rsImageHeader.revision.maintenanceVer = adrv904x_CpuIntFromBytesGet(binary + RS_IMAGE_VERSION_ID_START_BYTE + 8U, 4U);
        rsImageHeader.revision.buildVer = adrv904x_CpuIntFromBytesGet(binary + RS_IMAGE_VERSION_ID_START_BYTE + 12U, 4U);
        rsImageHeader.numberPatterns = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, numberPatterns)], 4U);        
        rsImageHeader.sequenceSize = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, sequenceSize)], 4U);
        rsImageHeader.seqStartAddr = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, seqStartAddr)], 4U);
        rsImageHeader.lookupCrc = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, lookupCrc)], 4U);
        rsImageHeader.sequenceCrc = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, sequenceCrc)], 4U);
        rsImageHeader.fileImageCrc = adrv904x_CpuIntFromBytesGet(&binary[ADI_LIBRARY_OFFSETOF(adi_adrv904x_RadioSeqImageHeader_t, fileImageCrc)], 4U);       

        if (rsImageHeader.magicNumber != ADI_ADRV904X_RADIO_SEQ_MAGIC_NUMBER)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsImageHeader.magicNumber, "Unsupported Radio Sequencer image file detected");
            goto cleanup;
        }

#if ADI_ADRV904X_API_VERSION_RANGE_CHECK > 0
        {
            adi_adrv904x_Version_t minRevision = { 0U, 0U, 0U, 0U };

                        recoveryAction = adi_adrv904x_ApiVersionGet(device, &minRevision);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Sequencer Binary Version Get Issue");
                goto cleanup;
            }

            recoveryAction = adrv904x_ApiVersionRangeCheck(device, &(rsImageHeader.revision), &minRevision, &minRevision);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Radio Sequencer Binary Range Check Issue");
                goto cleanup;
            }
                    }
#endif

        if (rsImageHeader.sequenceSize > (ADI_ADRV904X_RADIO_SEQ_IMAGE_MAX_SIZE / sizeof(uint32_t)))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsImageHeader.sequenceSize, "Unsupported Radio Sequencer image file detected");
            goto cleanup;
        }

        if (rsImageHeader.numberPatterns > ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsImageHeader.numberPatterns, "Unsupported Radio Sequencer image file detected");
            goto cleanup;
        }

        if (rsImageHeader.seqStartAddr != ADI_ADRV904X_RADIO_SEQ_IMAGE_MEM_REGION_START)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, rsImageHeader.seqStartAddr, "Unsupported Radio Sequencer image file detected");
            goto cleanup;
        }

        /* Save to use later in case of multiple call to this API */
        ADI_LIBRARY_MEMCPY(&device->devStateInfo.rsImageHeader, &rsImageHeader, sizeof(device->devStateInfo.rsImageHeader));

        /* Init CRC to 0 */
        device->devStateInfo.lookupCrcRun = 0U; 
        device->devStateInfo.sequenceCrcRun = 0U;
        device->devStateInfo.fileImageCrcRun = 0U;

        /* Run file image CRC */
        device->devStateInfo.fileImageCrcRun = adrv904x_Crc32ForChunk(binary, FINAL_RS_IMAGE_HEADER_SIZE_BYTES-4U, device->devStateInfo.fileImageCrcRun, 0U);
        device->devStateInfo.fileImageCrcRun = adrv904x_Crc32ForChunk(binaryZero, 4U, device->devStateInfo.fileImageCrcRun, 0U);

        imageSize = FINAL_RS_IMAGE_HEADER_SIZE_BYTES;
        byteOffset += imageSize;
        binary += imageSize;
        byteCount -= imageSize;

        if (byteCount == 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            goto cleanup;
        }
    }

    /* Run file image CRC */  
    device->devStateInfo.fileImageCrcRun = adrv904x_Crc32ForChunk(binary, byteCount, device->devStateInfo.fileImageCrcRun, 0U);
   
    xbarImageSize = ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE;
    xbarImageOffset = FINAL_RS_IMAGE_HEADER_SIZE_BYTES;

    lookupImageSize = device->devStateInfo.rsImageHeader.numberPatterns * sizeof(uint32_t);
    lookupImageOffset = xbarImageOffset + ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE;

    sequenceImageSize = device->devStateInfo.rsImageHeader.sequenceSize * sizeof(uint32_t);
    sequenceImageOffset = lookupImageOffset + lookupImageSize;

    /* Save xbarSettings */
    if ((byteOffset >= xbarImageOffset) && 
        (byteOffset < lookupImageOffset))
    {
        imageOffset = byteOffset - xbarImageOffset;
        imageSize = xbarImageSize - imageOffset;
        if (byteCount < imageSize)
        {
            imageSize = byteCount;
        }


        for (i = 0; i < imageSize; i++)
        {
            device->devStateInfo.xbarSettings[imageOffset + i] = binary[i];
        }
        
        byteOffset += imageSize;
        binary += imageSize;
        byteCount -= imageSize;

        if (byteCount == 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            goto cleanup;
        }
    }

    /* Save sequence lookup table */
    if ((byteOffset >= lookupImageOffset) && 
        (byteOffset < sequenceImageOffset))
    {        
        imageOffset = byteOffset - lookupImageOffset;
        imageSize = lookupImageSize - imageOffset;
        
        if (byteCount < imageSize)
        {
            imageSize = byteCount;            
            finalCrc = 0U;
        }
        else
        {
            finalCrc = 1U;
        }
        
        device->devStateInfo.lookupCrcRun = adrv904x_Crc32ForChunk(binary, imageSize, device->devStateInfo.lookupCrcRun, finalCrc);

        if ((finalCrc == 1U) && 
            (device->devStateInfo.lookupCrcRun != device->devStateInfo.rsImageHeader.lookupCrc))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, device->devStateInfo.lookupCrcRun, "Unsupported Radio Sequencer image file detected (file is corrupt)");
            goto cleanup;
        }

        for (i = 0; i < imageSize; i = i + sizeof(uint32_t))
        {
            device->devStateInfo.lookupTable[(imageOffset + i) / sizeof(uint32_t)] = adrv904x_CpuIntFromBytesGet(&binary[i], sizeof(uint32_t));
        }
        
        byteOffset += imageSize;
        binary += imageSize;
        byteCount -= imageSize;
        
        if (byteCount == 0U)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            goto cleanup;
        }
    }


    /* write sequence Instruction */
    if ((byteOffset >= sequenceImageOffset))
    {
        imageOffset = byteOffset - sequenceImageOffset;
        imageSize = sequenceImageSize - imageOffset;
        
        if (byteCount < imageSize)
        {
            imageSize = byteCount;            
            finalCrc = 0U;
        }
        else
        {
            finalCrc = 1U;
        }
        
        device->devStateInfo.sequenceCrcRun = adrv904x_Crc32ForChunk(binary, imageSize, device->devStateInfo.sequenceCrcRun, finalCrc);

        if ((finalCrc == 1U) && 
            (device->devStateInfo.sequenceCrcRun != device->devStateInfo.rsImageHeader.sequenceCrc))
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, device->devStateInfo.sequenceCrcRun, "Unsupported Radio Sequencer image file detected (file is corrupt)");
            goto cleanup;
        }

        address[0U] = device->devStateInfo.rsImageHeader.seqStartAddr + imageOffset;
        recoveryAction = adi_adrv904x_RegistersByteWrite(device,
                                                         NULL,
                                                         address[0],
                                                         binary,
                                                         imageSize);
                                                           
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failed to write radio sequence Instruction");
            goto cleanup;
        }

        if (finalCrc == 1U)
        {
            /* Run file image CRC */
            device->devStateInfo.fileImageCrcRun = adrv904x_Crc32ForChunk(binary, 0U, device->devStateInfo.fileImageCrcRun, 1U);
            if (device->devStateInfo.fileImageCrcRun != device->devStateInfo.rsImageHeader.fileImageCrc)
            {
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, device->devStateInfo.fileImageCrcRun, "Unsupported Radio Sequencer image file detected (file is corrupt)");
                goto cleanup;
            }

            device->devStateInfo.devState = (adi_adrv904x_ApiStates_e)(device->devStateInfo.devState | ADI_ADRV904X_STATE_RSLOADED);
        }
    }

    /* Write radio sequencer image size to the radio processor for CRC verificaiton */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADRV904X_DM_RSEQ_IMAGE_SIZE, sequenceImageSize, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing radio sequencer image size value");
        goto cleanup;
    }

    /* Write radio sequencer CRC to the radio processor for run-time verification */
    recoveryAction = adi_adrv904x_Register32Write(device, NULL, ADRV904X_DM_RSEQ_IMAGE_CRC, device->devStateInfo.sequenceCrcRun, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while writing radio sequencer image size value");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPhaseSet(adi_adrv904x_Device_t* const device, uint32_t phaseAdjustment)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
#if ADI_ADRV904X_RADIOCTRL_RANGE_CHECK > 0
    recoveryAction = adrv904x_RadioSequencerPhaseSetRangeCheck(device, phaseAdjustment);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "RadioSequencerPhaseSetRangeCheck Issue");
        goto cleanup;
    }
#endif

    /* Check that Radio Sequencer has been loaded before setting multiframer phase offset */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before setting multiframer phase offset");
        goto cleanup;
    }

    /* Set multi frame phase adjustment */
    recoveryAction = adrv904x_Sequencer_MultiframePhaseAdjust_BfSet(device,
                                                                    NULL,
                                                                    sequencerBaseAddr,
                                                                    phaseAdjustment);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting multiframe phase adjustment");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPhaseGet(adi_adrv904x_Device_t* const device, uint32_t* phaseAdjustment)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    const uint32_t RADIO_SEQUENCER_GLOBAL_CONFIG_1_REG_OFFSET = 0x8U;
    const uint32_t SSB_SYNC_PHASE_BITM = 0x01FFFFFFU;
    uint32_t regVal = 0;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, phaseAdjustment, cleanup);

    /* Check that Radio Sequencer has been loaded before getting SSB phase offset */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before getting SSB phase adjustment");
        goto cleanup;
    }

    /* Get ssb_sync_phase */
    recoveryAction = adi_adrv904x_Register32Read(device,
                                                 NULL,
                                                 ((uint32_t) sequencerBaseAddr + RADIO_SEQUENCER_GLOBAL_CONFIG_1_REG_OFFSET),
                                                 &regVal,
                                                 0xFFFFFFFFU);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while getting SSB sync phase adjustment");
        goto cleanup;
    }

    *phaseAdjustment = regVal & SSB_SYNC_PHASE_BITM;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPreMcsCfg(adi_adrv904x_Device_t* const    device)
{
        const adi_adrv904x_GpioSignal_e sigDigitalOut[ADI_ADRV904X_GPIO_COUNT] = {
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_15,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_16,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_17,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_18,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_19,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_20,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_21,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_22,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_DIGITAL_OUT_23
    };
    const adi_adrv904x_GpioSignal_e sigAnalogOut[ADI_ADRV904X_GPIO_ANALOG_COUNT] = {
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_0,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_1,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_2,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_3,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_4,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_5,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_6,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_7,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_8,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_9,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_10,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_11,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_12,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_13,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_14,
        ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_ANALOG_OUT_15
    };
    
    adi_adrv904x_ErrAction_e  recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;
    uint32_t chanId = 0u;
    uint32_t gpio = 0u;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer Image is not loaded.");
        goto cleanup;
    }

    /* Set sample clk divide ratio */
    recoveryAction = adrv904x_Core_RsSampleClkDivideRatio_BfSet(device,
                                                                NULL,
                                                                ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                                device->initExtract.rs.sampleClkDiv);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while rs sample clk divide ratio.");
        goto cleanup;
    }
        
    /* Set sample clk enable */
    recoveryAction = adrv904x_Core_RsSampleClkEnable_BfSet(device,
                                                           NULL,
                                                           ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                           device->initExtract.rs.sampleClkEn);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while rs sample clk divide enable.");
        goto cleanup;
    }

    /* Set pointer init */
    for (chanId = 0; chanId < ADI_ADRV904X_MAX_RADIOSEQUENCER; chanId++)
    {
        recoveryAction = adrv904x_Sequencer_PointerInit_BfSet(device,
                                                              NULL,
                                                              sequencerBaseAddr,
                                                              chanId,
                                                              ADI_ADRV904X_RADIO_SEQ_IMAGE_MEM_REGION_START >> 4u);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting pointer init.");
            goto cleanup;
        }
    }

    /* Allocate and setup all GPIOs used by the radio sequencer. */
    /* Note: These will never be de-allocated from use by this feature during runtime. */

    /* Set SSB SYNC GPIO Pin*/
    if (device->initExtract.radioCtrlPreInit.radioSequencerSsbSyncGpioCtrl != ADI_ADRV904X_GPIO_INVALID)
    {
        recoveryAction = adrv904x_GpioSignalSet(device,
                                                device->initExtract.radioCtrlPreInit.radioSequencerSsbSyncGpioCtrl,
                                                ADI_ADRV904X_GPIO_SIGNAL_RADIO_SEQ_SSB_SYNC,
                                                0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to assign Radio Sequencer SSB SYNC input signal to GPIO");
            goto cleanup;
        }
    }

    /* Setup Digital GPIO Output Pins for Radio Sequencer */
    for (gpio = 0U; gpio < ADI_ADRV904X_GPIO_COUNT; gpio++)
    {
        if (device->initExtract.radioCtrlPreInit.radioSequencerGpioDigOut[gpio] != ADI_ADRV904X_GPIO_INVALID)
        {
            recoveryAction = adrv904x_GpioSignalSet(device,
                                                    device->initExtract.radioCtrlPreInit.radioSequencerGpioDigOut[gpio],
                                                    sigDigitalOut[gpio],
                                                    0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to assign Radio Sequencer Output signal to Digital GPIO");
                goto cleanup;
            }
        }
    }

    /* Setup Analog GPIO Output Pins for Radio Sequencer */
    for (gpio = 0U; gpio < ADI_ADRV904X_GPIO_ANALOG_COUNT; gpio++)
    {
        if (device->initExtract.radioCtrlPreInit.radioSequencerGpioAnaOut[gpio] != ADI_ADRV904X_GPIO_ANA_INVALID)
        {
            recoveryAction = adrv904x_GpioAnalogSignalSet(device,
                                                          device->initExtract.radioCtrlPreInit.radioSequencerGpioAnaOut[gpio],
                                                          sigAnalogOut[gpio],
                                                          0U);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Failure to assign Radio Sequencer Output signal to Analog GPIO");
                goto cleanup;
            }
        }
    }

    /* Set XBAR */
    for (chanId = 0; chanId < ADI_ADRV904X_RADIO_SEQ_XBAR_SIZE; chanId++)
    {     
        recoveryAction = adrv904x_Sequencer_XbarCtrlSel_BfSet(device,
                                                              NULL,
                                                              sequencerBaseAddr,
                                                              chanId,
                                                              (device->devStateInfo.xbarSettings[chanId] >> 4u));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting xbar control sel.");
            goto cleanup;
        }

        recoveryAction = adrv904x_Sequencer_XbarSequencerSel_BfSet(device,
                                                                   NULL,
                                                                   sequencerBaseAddr,
                                                                   chanId,
                                                                   (device->devStateInfo.xbarSettings[chanId] & 0x0F));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting xbar sequencer sel.");
            goto cleanup;
        }
    }

    /* Set ssb sync tracking mode */
    recoveryAction = adrv904x_Sequencer_SsbSyncTrackingMode_BfSet(device,
                                                                  NULL,
                                                                  sequencerBaseAddr,
                                                                  device->initExtract.rs.ssbSyncMode);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting ssb sycn tracking mode.");
        goto cleanup;
    }

    /* Set ssb sync period */
    recoveryAction = adrv904x_Sequencer_SsbSyncPeriod_BfSet(device,
                                                            NULL,
                                                            sequencerBaseAddr,
                                                            device->initExtract.rs.ssbSyncPeriod);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting ssb sycn period.");
        goto cleanup;
    }

    /* Set short symbol width  */
    recoveryAction = adrv904x_Sequencer_ShortSymbolWidth_BfSet(device,
                                                               NULL,
                                                               sequencerBaseAddr,
                                                               device->initExtract.rs.symWidth);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting short symbol width.");
        goto cleanup;
    }

    /* Set extended symbol width  */
    recoveryAction = adrv904x_Sequencer_ExtendedSymbolWidth_BfSet(device,
                                                               NULL,
                                                               sequencerBaseAddr,
                                                               device->initExtract.rs.extSymWidth);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting extended symbol width.");
        goto cleanup;
    }

    /* Set extended symbol spacing  */
    recoveryAction = adrv904x_Sequencer_ExtendedSymbolSpacing_BfSet(device,
                                                                    NULL,
                                                                    sequencerBaseAddr,
                                                                    device->initExtract.rs.extSymSpacing);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting extended symbol spacing.");
        goto cleanup;
    }

    /* Set global reset */
    recoveryAction = adrv904x_Sequencer_GlobalReset_BfSet(device,
                                                          NULL,
                                                          sequencerBaseAddr,
                                                          0u);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting global reset.");
        goto cleanup;
    }

    /* Set all Tx/Rx/ORx and antenna enables to be under RS control */
    recoveryAction = adrv904x_Core_TxRsModeSel_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     0xFFu);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Tx control to RS mode");
        goto cleanup;
    }
    recoveryAction = adrv904x_Core_RxRsModeSel_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     0xFFu);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Rx control to RS mode");
        goto cleanup;
    }
    recoveryAction = adrv904x_Core_OrxRsModeSel_BfSet(device,
                                                      NULL,
                                                      ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                      0x3u);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting ORx control to RS mode");
        goto cleanup;
    }
    recoveryAction = adrv904x_Core_TxAntRsModeSel_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        0xFFu);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Tx Antenna control to RS mode");
        goto cleanup;
    }
    recoveryAction = adrv904x_Core_RxAntRsModeSel_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        0xFFu);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Rx Antenna control to RS mode");
        goto cleanup;
    }

    /* Send the pattern lookup table to the DFE */
    recoveryAction = adrv904x_RadioSequencerDfePtrnLookupTableSet(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting pattern lookup table.");
        goto cleanup;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerPostMcsCfg(adi_adrv904x_Device_t* const    device)
{
        adi_adrv904x_ErrAction_e  recoveryAction      = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer Image is not loaded.");
        goto cleanup;
    }

    /* All Done */
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerRadioStart(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t sequencerId = 0x0U;
    uint32_t patternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NULL_PATTERN_GLOBAL_PTRN_ID;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded before getting RS error info */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before starting Radio Sequencer");
        goto cleanup;
    }

    if (device->devStateInfo.rsRadioRunning == ADI_TRUE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }
    else
    {
        /* First, enable/start all radio sequencers */
        recoveryAction = adrv904x_RadioSequencerStart(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while starting radio sequencer.\n");
            goto cleanup;
        }

        /* Set the patternId for all the radio sequencers */
        for (sequencerId = ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_0; sequencerId <= ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_9; sequencerId++)
        {
            if (device->devStateInfo.specialSequencerPatternInfo.specialSequencer[sequencerId].specialPatternUsed == ADI_FALSE)
            {
                patternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_DEFAULT_FRAMING_TIMING_SEQ_PTRN_ID;
            }
            else
            {
                patternId = device->devStateInfo.specialSequencerPatternInfo.specialSequencer[sequencerId].specialPatternId;
            }

            patternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_SEQ_PTRN_ID_TO_GLOBAL_ID(sequencerId, patternId);
            recoveryAction = adrv904x_RadioSequencerPatternSet(device, sequencerId, patternId);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS pattern. \n");
                goto cleanup;
            }
        }

        recoveryAction = adrv904x_RadioSequencerApply(device);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Apply. \n");
            goto cleanup;
        }

        recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                                ADRV904X_LINK_ID_0,
                                                ADRV904X_DFE_SVC_CMD_ID_RADIO_START,
                                                NULL,
                                                0,
                                                NULL,
                                                0,
                                                NULL);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while sending RS Radio Start. \n");
            goto cleanup;
        }

        device->devStateInfo.rsRadioRunning = ADI_TRUE;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerRadioStop(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t sequencerId = 0x0U;
    uint32_t patternId = ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NULL_PATTERN_GLOBAL_PTRN_ID;
    adrv904x_BfSequencerChanAddr_e sequencerBaseAddr = ADRV904X_BF_DIGITAL_CORE_SEQUENCER_REGS;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);

    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check that Radio Sequencer Enable */
    if (device->initExtract.rs.radioSeqEn == ADI_FALSE)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
        goto cleanup;
    }

    /* Check that Radio Sequencer has been loaded before getting RS error info */
    if ((device->devStateInfo.devState & ADI_ADRV904X_STATE_RSLOADED) != ADI_ADRV904X_STATE_RSLOADED)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Radio Sequencer should have been loaded before starting Radio Sequencer");
        goto cleanup;
    }

    if (device->initExtract.rs.ssbSyncMode == 0)
    {
        /* Check that Radio Sequencer was started */
        if (device->devStateInfo.rsRadioRunning == ADI_FALSE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
            goto cleanup;
        }

        /* Set the patternId for all the radio sequencers */
        for (sequencerId = ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_0;
             sequencerId <= ADI_ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_FRAME_TIMING_9; sequencerId++)
        {
            recoveryAction = adrv904x_RadioSequencerPatternSet(device, sequencerId, patternId);
            if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
            {
                ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS pattern. \n");
                goto cleanup;
            }
        }
    }
    else
    {
        recoveryAction = adrv904x_Sequencer_GlobalReset_BfSet(device, NULL, sequencerBaseAddr, 1U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Global Reset. \n");
            goto cleanup;
        }
    }

    recoveryAction = adrv904x_RadioSequencerApplyImmediately(device);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting RS Apply Immediately. \n");
        goto cleanup;
    }

    recoveryAction = adrv904x_DfeSvcCmdSend(device,
                                            ADRV904X_LINK_ID_0,
                                            ADRV904X_DFE_SVC_CMD_ID_RADIO_STOP,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            NULL);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while sending RS Radio Stop. \n");
        goto cleanup;
    }

    if (device->initExtract.rs.ssbSyncMode != 0)
    {
        device->devStateInfo.rsStarted = ADI_FALSE;
    }
    device->devStateInfo.rsRadioRunning = ADI_FALSE;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternSet(adi_adrv904x_Device_t* const                   device,
                                                                                  const adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t patternUsed = 0U;
    uint32_t patternId = 0U;
    uint32_t i = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, specialPatternInfo, cleanup);

    /* Config special pattern ID into device structure for further usage */
    for (i = 0U; i < ADI_ADRV904X_MAX_CUSTOMERCONFIGSEQUENCER; i++)
    {
        patternUsed = specialPatternInfo->specialSequencer[i].specialPatternUsed;
        patternId = specialPatternInfo->specialSequencer[i].specialPatternId;

        if (patternId >= ADRV904X_DFE_SVC_RADIO_CTRL_SEQUENCER_NUM_PATTERNS_PER_SEQUENCER)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Invalid input pattern id.\n");
            goto cleanup;
        }

        if (patternUsed == ADI_FALSE)
        {
            /* Clear special radio sequencer pattern info */
            device->devStateInfo.specialSequencerPatternInfo.specialSequencer[i].specialPatternUsed = ADI_FALSE;
            device->devStateInfo.specialSequencerPatternInfo.specialSequencer[i].specialPatternId = 0U;
        }
        else
        {
            device->devStateInfo.specialSequencerPatternInfo.specialSequencer[i].specialPatternUsed = ADI_TRUE;
            device->devStateInfo.specialSequencerPatternInfo.specialSequencer[i].specialPatternId = patternId;
        }
    }

    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
   ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternGet(adi_adrv904x_Device_t* const             device,
                                                                                  adi_adrv904x_RsSpecialSequencer_t* const specialPatternInfo)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, specialPatternInfo, cleanup);

    ADI_LIBRARY_MEMCPY((void*)specialPatternInfo, (void*)(&(device->devStateInfo.specialSequencerPatternInfo)), sizeof(adi_adrv904x_RsSpecialSequencer_t));
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

cleanup:
   ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioSequencerFrameTimingPatternClear(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Clear special pattern used flag and pattern ID array */
    ADI_LIBRARY_MEMSET((void*)(&(device->devStateInfo.specialSequencerPatternInfo)), 0U, sizeof(adi_adrv904x_RsSpecialSequencer_t));
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;

   ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingAndVswrDirSet(adi_adrv904x_Device_t* const    device,
                                                                          const uint8_t                   mapping,
                                                                          const uint8_t                   vswrDirection)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    static const uint8_t VSWR_DIRECTION_MAX = 3U;
    uint8_t modeMask = 0U;
    
    /* Check that Mapping Mode is NOT DFE CTRL. If it is, throw an error */
    adi_adrv904x_TxToOrxMappingMode_e lclMode = device->devStateInfo.txToOrxMappingConfig.mode;
    if (lclMode == ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lclMode,
                               "Device Tx to Orx Mapping is in DFE Ctrl Mode. This function can only be used in RCI mapping modes.");
        goto cleanup;
    }
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Get modeMask from the mode*/
    switch (device->devStateInfo.txToOrxMappingConfig.mode)
    {
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_2BIT:
            modeMask = 0x03U;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_3BIT:
            modeMask = 0x07U;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_4BIT:
            modeMask = 0x0FU;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_6BIT:
            modeMask = 0x3FU;
            break;
        case ADI_ADRV904X_TX_ORX_MAPPING_MODE_8BIT: /* Fallthrough */
        default:
            modeMask = 0xFFU;
            break;
    }
    
    /* Range check mapping byte against Mapping Mode */
    if ((mapping & modeMask) != mapping)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 mapping,
                                "Tx to Orx Mapping byte requested is not valid for use with the device's Mapping Mode configuration");
        goto cleanup;
    }

    if (vswrDirection > VSWR_DIRECTION_MAX) 
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               vswrDirection,
                               "VSWR direction must be a 2bit value, Orx0:Bit0 , Orx1:Bit1");
        goto cleanup;
    }

    /* Write mapping byte to scratch reg for the stream to readback */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_CPU_TX_ORX_MAPPING_SET_SCRATCH_REG_ID,
                                                    mapping);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while setting Tx to ORx mapping selection.");
        goto cleanup;
    }

    /* Set VSWR direction to scrathpad register */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    ADRV904X_DFE_VSWR_DIRECTION_SCRATCH_REG_ID,
                                                    vswrDirection);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while setting Vswr direction scrathpad register");
        goto cleanup;
    }

    /* Trigger stream */
    recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_MAIN_TX_TO_ORX_MAP);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while triggering Core Stream to apply Tx to Orx mapping");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_TxToOrxMappingAndVswrDirGet(adi_adrv904x_Device_t* const              device,
                                                                          const adi_adrv904x_RxChannels_e           orxChannel,
                                                                          adi_adrv904x_TxChannels_e* const          txChannel,
                                                                          adi_adrv904x_DfeVswrDirection_e* const    vswrDirection)
{
        static const uint8_t NIBBLE_TX_MASK   = 0x07U;

    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t readNibble = 0U;
    uint8_t orxIdx = 0U;
    uint8_t txIdx = 0U;
    uint16_t observabilityBit = 0U;
    uint8_t observable = 0U;
    uint8_t noMapState = 0U;
    uint32_t txChanSel = 0U;
    uint8_t vswrDir = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common) ;
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, txChannel, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, vswrDirection, cleanup);
    
    /* Range check orxChannel enum */
    if( (orxChannel != ADI_ADRV904X_ORX0) &&
        (orxChannel != ADI_ADRV904X_ORX1) )
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 orxChannel,
                                 "ORx Channel selected is invalid. Must select ORx0-1.") ;
        goto cleanup;
    }

    /* Readback current mapping */
    if (orxChannel == ADI_ADRV904X_ORX0)
    {
        orxIdx = 0U;
        recoveryAction = adrv904x_Core_RadioControlInterfaceOrx0Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &readNibble);
    }
    else
    {
        orxIdx = 1U;
        recoveryAction = adrv904x_Core_RadioControlInterfaceOrx1Map_BfGet(device,
                                                                          NULL,
                                                                          (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                                          &readNibble);
    }

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while retrieving current Tx to ORx map for selected ORx channel.");
        goto cleanup;
    }

    txIdx = NIBBLE_TX_MASK & readNibble;
    observabilityBit = 1U << (2U*txIdx + orxIdx);
    observable = (observabilityBit & device->devStateInfo.txToOrxMappingConfig.txObservability) ? ADI_TRUE : ADI_FALSE;

    if (noMapState || 
        !observable )
    {
        *txChannel = ADI_ADRV904X_TXOFF;
    }
    else
    {
        txChanSel = 1U << (uint32_t)txIdx;
        *txChannel = (adi_adrv904x_TxChannels_e)(txChanSel);
    }

    if (orxChannel == ADI_ADRV904X_ORX0)
    {
        recoveryAction = adrv904x_Core_VswrFwdRev0Sel_BfGet(device,
                                                            NULL,
                                                            (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                            &vswrDir);
    }
    else
    {
        recoveryAction = adrv904x_Core_VswrFwdRev1Sel_BfGet(device,
                                                            NULL,
                                                            (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                            &vswrDir);
    }
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while retrieving current Vswr direction bit for selected ORx channel.");
        goto cleanup;
    }

    *vswrDirection = (adi_adrv904x_DfeVswrDirection_e)vswrDir;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlPaSelectInputSet(adi_adrv904x_Device_t* const        device,
                                                                        const uint32_t                      paEnOutput,
                                                                        const adi_adrv904x_PaEnInputSel_e   paEnInput)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t newPaEnDtxShadow = 0U;
    uint8_t curPaEnDtxShadow = 0U;
    uint8_t bitPos = 0U;
    uint8_t bitVal = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check paEnOutput value according to adi_adrv904x_PaEnOutputs_e enum */
    if ((paEnOutput >> 8U) != 0U)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnOutput,
        "paEnOutput selected value is invalid. Only bit 0-7 is allowed.");
        goto cleanup;
    }
    
    /* Range check paEnInput enum */
    if ((paEnInput >> 1U) != 0U)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnInput,
                               "paEnInput selected value is invalid. Must either 0 or 1.");
        goto cleanup;
    }
    
    /* Read back the current shadow value */
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_WITH_DTX_OUTPUT_SHADOW,
                                                     &curPaEnDtxShadow);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while retrieving current PA En DTX shadow register.");
        goto cleanup;
    }
    
    newPaEnDtxShadow = curPaEnDtxShadow;
    for (bitPos = 0; bitPos < PA_EN_OUT_MAX; bitPos++)
    {
        bitVal = 1 << bitPos;
        /* Skip unselected bitmask */
        if ((paEnOutput & bitVal) == 0U)
        {
            continue; 
        }
        
        if (paEnInput == ADI_ADRV904X_PA_EN_INPUT_SEL_RS)
        {
            if ((newPaEnDtxShadow & bitVal) != 0)
            {
                newPaEnDtxShadow &= ~bitVal;
            }
        }
        else if (paEnInput == ADI_ADRV904X_PA_EN_INPUT_SEL_DTX_RS)
        {
            if ((newPaEnDtxShadow & bitVal) == 0)
            {
                newPaEnDtxShadow |= bitVal;
            }
        }
    }
    
    /* Write the new shadow value */
    recoveryAction =  adrv904x_Core_ScratchReg_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_WITH_DTX_OUTPUT_SHADOW,
                                                     newPaEnDtxShadow);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while setting new PA En DTX shadow register.");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlPaSelectInputGet(adi_adrv904x_Device_t* const          device,
                                                                        const adi_adrv904x_PaEnOutputs_e      paEnOutput,
                                                                        adi_adrv904x_PaEnInputSel_e * const   paEnInput)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t curPaEnDtxShadow = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, paEnInput, cleanup);
      
    /* Range check paEnOutput value according to adi_adrv904x_PaEnOutputs_e enum and only one bit is set */
    if (((paEnOutput >> 8U) != 0U) ||
        ((paEnOutput & (paEnOutput -1)) != 0) ||
        (paEnOutput == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnOutput,
                               "paEnOutput selected value is invalid. Only one bit 0-7 is allowed.");
        goto cleanup;
    }
    
    /* Read back the current shadow value */
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_WITH_DTX_OUTPUT_SHADOW,
                                                     &curPaEnDtxShadow);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while retrieving current PA En DTX shadow register.");
        goto cleanup;
    }
    
    if ((curPaEnDtxShadow & paEnOutput) == 0U)
    {
        *paEnInput = ADI_ADRV904X_PA_EN_INPUT_SEL_RS;
    }
    else 
    {
        *paEnInput = ADI_ADRV904X_PA_EN_INPUT_SEL_DTX_RS;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigSet(adi_adrv904x_Device_t* const    device,
                                                                                const uint32_t txChannelMask,
                                                                                const adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t nonzeroFanoutBm = 0x00U;

    uint8_t txIdx = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxFanoutCfg, cleanup);

    if (txChannelMask > (uint32_t)ADI_ADRV904X_TXALL)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txChannelMask,
                               "txChannelMask selected value is invalid. Only bit 0-7 is allowed.");
        goto cleanup;
    }

    if (device->devStateInfo.dtxInputMapping.dtxEventWaitTimeNs > 0)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               device->devStateInfo.dtxInputMapping.dtxEventWaitTimeNs,
                               "The DTX Event Wait Time cannot be set when DTX Fanout method is used.");
        goto cleanup;
    }

    for (txIdx = 0; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
    {
        /* Skip unselected TX channels */
        if (((uint32_t)txChannelMask & (1 << txIdx) ) == 0U)
        {
            continue;
        }
        
        /* Range check DTX fanout value according to adi_adrv904x_PaEnOutputs_e enum */
        if (dtxFanoutCfg->fanout[txIdx] > ADI_ADRV904X_PA_EN_OUT_ALL)
        {
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   dtxFanoutCfg->fanout[txIdx],
                                   "Selected DTX fanout value is invalid. Only bit 0-7 is allowed.");
            goto cleanup;
        }

        /* Write the requested DTX fanout to scratchpad table */
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX0 + txIdx),
                                                        dtxFanoutCfg->fanout[txIdx]);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                recoveryAction,
                "Error while writing the value of DTX fanout to PA_EN.");
            goto cleanup;
        }

        if (dtxFanoutCfg->fanout[txIdx] != 0U)
        {
            nonzeroFanoutBm |= ((1 << txIdx));
        }
    }

    /* Write the requested DTX fanout to scratchpad table */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (ADRV904X_NONZERO_DTX_FANOUT_TO_PA_EN_FOR_TX_BM),
                                                    nonzeroFanoutBm);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Error while writing the value of DTX fanout bitmap.");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDtxFanoutToPaEnConfigGet(adi_adrv904x_Device_t* const    device,
                                                                                adi_adrv904x_DtxFanoutToPaEnCfg_t * const dtxFanoutCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regValue = 0U;
    uint8_t txIdx = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, dtxFanoutCfg, cleanup);

    ADI_LIBRARY_MEMSET(dtxFanoutCfg, 0, sizeof(adi_adrv904x_DtxFanoutToPaEnCfg_t));

    for (txIdx = 0; txIdx < ADI_ADRV904X_MAX_TXCHANNELS; txIdx++)
    {
        /* Read DTX fanout to PA_EN */
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (ADRV904X_DTX_FANOUT_TO_PA_EN_TBL_TX0 + txIdx),
                                                        &regValue);
        
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                recoveryAction,
                                "Error while reading the value of DTX fanout to PA_EN.");
            goto cleanup;
        }

        dtxFanoutCfg->fanout[txIdx] = regValue;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmConfigSet(adi_adrv904x_Device_t* const                      device,
                                                                         const uint32_t                                    alarmMask,
                                                                         const adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t extendedCoreScratchRegBaseAddress = 0U;
    uint32_t tempAddress = 0U;
    uint8_t origEccState = 0U;
    uint16_t  scratchId = 0U;
    uint8_t alarmIdx = 0U;
    uint16_t alarmVal = 0U;
    uint8_t regValue = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, alarmCfg, cleanup);
    
    /* Range check alarmMask value according to adi_adrv904x_Alarms_e enum */
    if (((alarmMask >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
         (alarmMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmMask,
                               "alarmMask selected value is invalid. Only bit 0-9 is allowed.");
        goto cleanup;
    }
    
    /* Check alarm type */
    if ((alarmCfg->alarmType == ADI_ADRV904X_ALARM_TYPE_NONE) && 
        ((alarmCfg->lnaEnOut != 0) || (alarmCfg->paEnOut !=0) || (alarmCfg->auxGrp0Out != 0) || (alarmCfg->auxGrp1Out != 0)))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            alarmMask,
            "alarmType selected value is invalid.");
        goto cleanup;
    }

    for (alarmIdx = 0; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        alarmVal = (uint16_t)1 << alarmIdx;
        
        /* Skip unselected bitmask */
        if ((alarmMask & alarmVal) == 0U)
        {
            continue; 
        }
        
        /* Range check alarmType value. */
        if ((uint8_t) alarmCfg->alarmType > ADI_ADRV904X_MAX_ALARM_TYPE)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                recoveryAction,
                alarmCfg->alarmType,
                "alarmType selected value is invalid. Only value less than 2 is allowed.");
            goto cleanup;
        }

        /* Read current value of scratchpad register */
        scratchId = ADRV904X_EA_TYPE_TBL_ALARM_0 + alarmIdx;
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        scratchId,
                                                        &regValue);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                recoveryAction,
                "Error while reading scratchpad register value.");
            goto cleanup;
        }

        /* Only 3 Most Significant Bits are used for Alarm Type */
        regValue = (regValue & 0x1F) | (alarmCfg->alarmType << 5);

        /* Write the Alarm Type */
        scratchId = ADRV904X_EA_TYPE_TBL_ALARM_0 + alarmIdx;
        recoveryAction =  adrv904x_Core_ScratchReg_BfSet(device,
                                                         NULL,
                                                         ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                         scratchId,
                                                         regValue);
    
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 "Error while setting new External Alarm type.");
            goto cleanup;
        }
        
        /* Read the base address for extended core scratch registers located in M4 heap*/
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADRV904X_PM_CORE_SCRATCH_EXT_PTR, &extendedCoreScratchRegBaseAddress, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading extended core stream scratch base addr");
            goto cleanup;
        }

        /* Read the current ECC scrubbing state in M4 before disabling ECC */
        recoveryAction = adi_adrv904x_EccEnableGet(device, &origEccState);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading ECC state of M4");
            return recoveryAction;
        }

        /* Disable ECC before we write to M4 memory */
        recoveryAction = adi_adrv904x_EccEnableSet(device, 0U);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while disabling ECC scrubbing on M4");
            return recoveryAction;
        }

        /* Write LNA En */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_0 + alarmIdx);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, alarmCfg->lnaEnOut, 0x000000FF);
    
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 "Error while setting new LNA En value.");
            goto cleanup;
        }

        /* Write PA En */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_PA_EN_TBL_ALARM_0 + alarmIdx);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, alarmCfg->paEnOut, 0x000000FF);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 "Error while setting new PA En value.");
            goto cleanup;
        }

        /* Write OC Fuse En Out */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_0 + alarmIdx);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, alarmCfg->ocFuseEnOut, 0x000000FF);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 "Error while setting new OC Fuse En Out value.");
            goto cleanup;
        }

        /* Write Aux Grp0 */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_0 + alarmIdx);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, alarmCfg->auxGrp0Out, 0x000000FF);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while setting new Aux Grp0 Out value.");
            goto cleanup;
        }

        /* Write Aux Grp1 */
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_0 + alarmIdx);
        recoveryAction = adi_adrv904x_Register32Write(device, NULL, tempAddress, alarmCfg->auxGrp1Out, 0x000000FF);

        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while setting new Aux Grp1 Out value.");
            goto cleanup;
        }

        /* Restore ECC scrubbing before we exit */
        recoveryAction = adi_adrv904x_EccEnableSet(device, origEccState);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while restoring ECC scrubbing on M4");
            return recoveryAction;
        }
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmConfigGet(adi_adrv904x_Device_t* const                device,
                                                                         const adi_adrv904x_Alarms_e                 alarmId,
                                                                         adi_adrv904x_RadioCtrlAlarmConfig_t * const alarmCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t extendedCoreScratchRegBaseAddress = 0U;
    uint32_t tempAddress = 0U;
    uint16_t  scratchId = 0U;
    uint8_t alarmIdx = 0U;
    uint8_t regValue = 0U;
    uint32_t regValue32 = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, alarmCfg, cleanup);
    
    ADI_LIBRARY_MEMSET(alarmCfg, 0, sizeof(adi_adrv904x_RadioCtrlAlarmConfig_t));
    
    /* Range check alarmId value according to adi_adrv904x_Alarms_e enum */
    if (((alarmId >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
        ((alarmId & (alarmId -1)) != 0) ||
         (alarmId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmId,
                               "alarmId selected value is invalid.");
        goto cleanup;
    }
    
    for (alarmIdx = 0; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        if (((uint32_t)alarmId & ((uint32_t)1 << alarmIdx)) > 0)
        {
            break; 
        }
    }
  
    if (alarmIdx >= ADI_ADRV904X_ALARM_MAX)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmId,
                               "alarmId selected value is invalid.");
        goto cleanup;
    }
    
    /* Read the Alarm Type */
    scratchId = ADRV904X_EA_TYPE_TBL_ALARM_0 + alarmIdx;
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     scratchId,
                                                     &regValue);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading new External Alarm type.");
        goto cleanup;
    }

    /* Only 3 Most Significant Bits are used for Alarm Type */
    alarmCfg->alarmType = (adi_adrv904x_AlarmType_e)((regValue & 0xE0) >> 5);        

    recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADRV904X_PM_CORE_SCRATCH_EXT_PTR, &extendedCoreScratchRegBaseAddress, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading extended core stream scratch base addr");
        goto cleanup;
    }

    /* Read LNA En */
    tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_LNA_EN_TBL_ALARM_0 + alarmIdx);
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &regValue32, 0xFFFFFFFF);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading new LNA En value.");
        goto cleanup;
    }
    alarmCfg->lnaEnOut = (uint8_t)regValue32;
        
    /* Read PA En */
    tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_PA_EN_TBL_ALARM_0 + alarmIdx);
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &regValue32, 0xFFFFFFFF);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading new PA En value.");
        goto cleanup;
    }
    alarmCfg->paEnOut = (uint8_t)regValue32;
    
    /* Read OC Fuse En Out */
    tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_OC_FUSE_EN_TBL_ALARM_0 + alarmIdx);
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &regValue32, 0xFFFFFFFF);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading new OC Fuse En Out value.");
        goto cleanup;
    }
    alarmCfg->ocFuseEnOut = (uint8_t)regValue32;

    /* Read Aux Grp0 */
    tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_AUX_GRP0_TBL_ALARM_0 + alarmIdx);
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &regValue32, 0xFFFFFFFF);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading new Aux Grp0 value.");
        goto cleanup;
    }
    alarmCfg->auxGrp0Out = (uint8_t)regValue32;


    /* Read Aux Grp1 */
    tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_EA_IMP_AUX_GRP1_TBL_ALARM_0 + alarmIdx);
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &regValue32, 0xFFFFFFFF);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading new Aux Grp1 value.");
        goto cleanup;
    }
    alarmCfg->auxGrp1Out = (uint8_t)regValue32;

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmStateGet(adi_adrv904x_Device_t* const       device,
                                                                        const adi_adrv904x_Alarms_e        alarmId,
                                                                        adi_adrv904x_AlarmState_e * const  alarmState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint16_t  scratchId = 0U;
    uint8_t alarmIdx = 0U;
    uint8_t activePaEnAlarm = 0U;
    uint8_t activeLnaEnAlarm = 0U;
    uint8_t activeAuxGrp0OutAlarm = 0U;
    uint8_t activeAuxGrp1OutAlarm = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, alarmState, cleanup);
    
    /* Range check alarmId value according to adi_adrv904x_Alarms_e enum */
    if (((alarmId >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
        ((alarmId & (alarmId - 1)) != 0) ||
         (alarmId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmId,
                               "alarmId selected value is invalid.");
        goto cleanup;
    }
    
    for (alarmIdx = 0; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        if (((uint32_t)alarmId & ((uint32_t)1 << alarmIdx)) > 0)
        {
            break; 
        }
    }

    /* Read the current active PA En Alarm */
    scratchId = ADRV904X_EA_CURR_AFCTD_PA_EN_TBL_ALARM_0  + alarmIdx;
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     scratchId,
                                                     &activePaEnAlarm);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading EA_CURR_AFCTD_PA_EN_TBL.");
        goto cleanup;
    }
   
    /* Read the current active LNA En Alarm */
    scratchId = ADRV904X_EA_CURR_AFCTD_LNA_EN_TBL_ALARM_0  + alarmIdx;
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     scratchId,
                                                     &activeLnaEnAlarm);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading EA_CURR_AFCTD_LNA_EN_TBL.");
        goto cleanup;
    }

    /* Read the current active AUX GRP0 output Alarm */
    scratchId = ADRV904X_EA_CURR_AFCTD_AUX_GRP0_TBL_ALARM_0 + alarmIdx;
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    scratchId,
                                                    &activeAuxGrp0OutAlarm);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading EA_CURR_AFCTD_AUX_GRP0_OUT_TBL.");
        goto cleanup;
    }

    /* Read the current active AUX GRP1 output Alarm */
    scratchId = ADRV904X_EA_CURR_AFCTD_AUX_GRP1_TBL_ALARM_0 + alarmIdx;
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    scratchId,
                                                    &activeAuxGrp1OutAlarm);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading EA_CURR_AFCTD_AUX_GRP1_OUT_TBL.");
        goto cleanup;
    }

    *alarmState = ADI_ADRV904X_ALARM_STATE_INACTIVE;
    if ((activePaEnAlarm != 0U) || (activeLnaEnAlarm != 0U) || (activeAuxGrp0OutAlarm != 0U) || (activeAuxGrp1OutAlarm != 0U))
    {
        *alarmState = ADI_ADRV904X_ALARM_STATE_ACTIVE;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmGpioStateGet(adi_adrv904x_Device_t* const       device,
                                                                            const adi_adrv904x_Alarms_e        alarmId,
                                                                            adi_adrv904x_AlarmState_e * const  alarmState)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioInPinLevel = 0U;
    uint8_t alarmIdx = 0U;
    uint8_t gpioPin = 0U;
    uint16_t gpioPinPolarity = 0U;
    uint8_t tempReg = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, alarmState, cleanup);
    
    /* Range check alarmId value according to adi_adrv904x_Alarms_e enum */
    if (((alarmId >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
        ((alarmId & (alarmId - 1)) != 0) ||
         (alarmId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
            recoveryAction,
            alarmId,
            "alarmId selected value is invalid.");
        goto cleanup;
    }
    
    /* Initialize the alarm state to inactive */
    *alarmState = ADI_ADRV904X_ALARM_STATE_INACTIVE;

    for (alarmIdx = 0; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        if ((uint32_t)(alarmId & ((uint32_t)1 << alarmIdx)) > 0)
        {
            break; 
        }
    }

    /* Find which GPIO pin is assigned as an input to this alarm */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
        NULL,
        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
        (ADRV904X_EA_TYPE_TBL_ALARM_0 + alarmIdx),
        &gpioPin);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Error while reading External Alarm type table.");
        goto cleanup;
    }

    /* Only 5 LSB bits are used as GPIO pin number */
    gpioPin &= 0x1F;
    
    if (gpioPin == 0x1F)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                             "No GPIO input pin assigned to the alarm.");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_GpioSpiRead_BfGet(device,
                                                     NULL,
                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                     &gpioInPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, 
                             recoveryAction, 
                             "Error while reading back GPIO Manual Mode Input Word values.");
        goto cleanup;
    }

    /* Find the polarity of the pin assigned to this alarm */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_INPUT_PIN_POLARITY_BM_0,
                                                    &tempReg);
    gpioPinPolarity = tempReg;
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading External Alarm pin polarity, byte0.");
        goto cleanup;
    }
    
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_INPUT_PIN_POLARITY_BM_1,
                                                    &tempReg);
    gpioPinPolarity |= (((uint16_t)tempReg << 8U) & 0xFF00U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                            "Error while reading External Alarm pin polarity, byte1.");
        goto cleanup;
    }

    gpioPinPolarity &= (1 << alarmIdx);
    gpioInPinLevel &= (1 << gpioPin);

    /* Check the state of the GPIO pin assigned to this alarm. */
    if (((gpioInPinLevel == 0) && (gpioPinPolarity == 0)) ||
        ((gpioInPinLevel != 0) && (gpioPinPolarity != 0)))
    {
        *alarmState = ADI_ADRV904X_ALARM_STATE_ACTIVE;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlExtAlarmStateClear(adi_adrv904x_Device_t* const      device,
                                                                          const adi_adrv904x_Alarms_e       alarmId)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t gpioInPinLevel = 0U;
    uint16_t gpioPinPolarity = 0U;
    uint8_t alarmIdx = 0U;
    uint8_t gpioPin = 0U;
    uint8_t regVal = 0U;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check alarmId value according to adi_adrv904x_Alarms_e enum */
    if (((alarmId >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
        ((alarmId & (alarmId - 1)) != 0) ||
         (alarmId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmId,
                               "alarmId selected value is invalid.");
        goto cleanup;
    }

    for (alarmIdx = 0; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        if (((uint32_t)alarmId & ((uint32_t)1 << alarmIdx)) > 0)
        {
            break; 
        }
    }

    /* Find which GPIO pin is assigned as an input to this alarm */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (ADRV904X_EA_TYPE_TBL_ALARM_0 + alarmIdx),
                                                    &gpioPin);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading External Alarm type table.");
        goto cleanup;
    }

    /* Only 5 LSB bits are used as GPIO pin number */
    gpioPin &= 0x1F;

    if (gpioPin == 0x1F)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             ADI_ADRV904X_ERR_ACT_CHECK_PARAM,
                             "No GPIO input pin assigned to the alarm.");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_GpioSpiRead_BfGet(device,
                                                     NULL,
                                                     (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                     &gpioInPinLevel);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, 
                             recoveryAction, 
                             "Error while reading back GPIO Manual Mode Input Word values.");
        goto cleanup;
    }

    /* Find the polarity of the pin assigned to this alarm */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_INPUT_PIN_POLARITY_BM_0,
                                                    &regVal);
    gpioPinPolarity = regVal;
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading External Alarm pin polarity, byte0.");
        goto cleanup;
    }

    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_INPUT_PIN_POLARITY_BM_1,
                                                    &regVal);
    gpioPinPolarity |= (((uint16_t)regVal << 8U) & 0xFF00U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while reading External Alarm pin polarity, byte1.");
        goto cleanup;
    }
    
    gpioPinPolarity &= (1 << alarmIdx);
    gpioInPinLevel &= (1 << gpioPin);

    /* Check the state of the GPIO pin assigned to this alarm. If still active, report an error */
    if (((gpioInPinLevel == 0) && (gpioPinPolarity == 0)) ||
        ((gpioInPinLevel != 0) && (gpioPinPolarity != 0)))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_API_ERROR_REPORT(&device->common, 
                             recoveryAction, 
                             "Incorrect state if the GPIO pin assigned as alarm input.");
        goto cleanup;
    }
     
    /* Set the Alarm ID  */
    regVal = (uint8_t) alarmIdx;
    recoveryAction =  adrv904x_Core_ScratchReg_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM,
                                                     regVal);
    
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while setting alarm id.");
        goto cleanup;
    }
        
    /* Trigger stream */
    recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_EXT_ALARM_STATE_CLEAR);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while triggering Core Stream to apply Ext Alarm State Clear.");
        goto cleanup;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOnSet(adi_adrv904x_Device_t* const      device,
                                                                         const uint32_t lnaEnMask,
                                                                         const uint8_t activate)
{
        const uint8_t MAX_SIGNAL = 8U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lnaEnWithAssignedGpiosBitmask = 0;
    uint8_t signalOffset = 0U;
    uint8_t isDigital = 0U;
    uint8_t lnaEnAlwaysOffFlags = 0U;
    uint8_t lnaEnAlwaysOnFlags = 0U;
    uint8_t lnaEnAffectedByAlarm = 0U;
    uint8_t lnaEnOutputBm = 0U;
    adi_adrv904x_OutGpioPinStates_e gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check lnaEnMask value according to adi_adrv904x_LnaEnOutputs_e enum */
    if (((lnaEnMask >> 8U) != 0U) ||
         (lnaEnMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               lnaEnMask,
                               "lnaEnMask selected value is invalid.");
        goto cleanup;
    }

    /* Get the ALWAYS OFF flag for all LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_OFF,
                                                    &lnaEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting LNA EN ALWAYS OFF.");
        goto cleanup;
    }

    /* Get the ALWAYS ON flag for all LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_ON,
                                                    &lnaEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting LNA EN ALWAYS ON.");
        goto cleanup;
    }

    /* Create a bitmap of all LNA_EN which have GPIO pin assigned */
    for (signalOffset = 0; signalOffset < MAX_SIGNAL; signalOffset++)
    {
        if ((device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[signalOffset] == ADI_ADRV904X_GPIO_INVALID) &&
            (device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[signalOffset] == ADI_ADRV904X_GPIO_ANA_INVALID))
        {
            lnaEnWithAssignedGpiosBitmask &= ~(1 << signalOffset);
        }
        else
        {
            lnaEnWithAssignedGpiosBitmask |= (1 << signalOffset);
        }
    }

    if (activate == ADI_TRUE)
    {
        /* Check if any if the requested LNA_EN signals has ALWAYS_OFF flag already set */
        if ((lnaEnMask & lnaEnAlwaysOffFlags) != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                recoveryAction,
                lnaEnMask,
                "One of the selected LNA EN signals has ALWAYS OFF flag set.");
            goto cleanup;
        }

        /* Update the Always ON flag */
        lnaEnAlwaysOnFlags |= lnaEnMask;

        /* Set pin state to the value selected in the device profile */
        switch (device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode,
                                       "Unexpected LNA EN pin mode.");
                goto cleanup;
        }

        /* Update the output for all LNA_EN specified in the request which have GPIO pin assigned and set the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnMask & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            2);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN ALWAYS ON outputs.");
            goto cleanup;
        }
    }
    else
    {
        /* Update the Always ON flag */
        lnaEnAlwaysOnFlags &= ~lnaEnMask;

        /* Only update the LNA_EN outputs which are not frozen by Always OFF flag */
        lnaEnOutputBm = lnaEnMask & ~lnaEnAlwaysOffFlags;

        /* Check which LNA_EN outputs are affected by an active alarm */
        recoveryAction = adrv904x_RadioCtrlDbgGpioGetLnaEnAffectedByAlarms(device,
                                                                           &lnaEnAffectedByAlarm);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting bitmap of LNA_EN outputs currently affected by active alarm.");
            goto cleanup;
        }

        /* First, handle the LNA_EN outputs affected by active alarms */

        /* Change pin state to the deactive state */
        switch (device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode,
                                       "Unexpected LNA EN pin mode.");
                goto cleanup;
        }

        /* Update the LNA_EN outputs affected by active alarms which have GPIO pin assigned and clear the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnOutputBm & lnaEnAffectedByAlarm & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN outputs affected by alarms.");
            goto cleanup;
        }

        /* Then, the rest of the outputs can be set to normal operation */

        /* Set pin state to Normal Operation (RS controlled) and clear the debug flag */
        gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnOutputBm & ~lnaEnAffectedByAlarm & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN outputs not affected by alarms.");
            goto cleanup;
        }
    }

    /* Update the ALWAYS ON flag for LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_ON,
                                                    lnaEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting LNA EN ALWAYS ON.");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOnGet(adi_adrv904x_Device_t* const      device,
                                                                         const adi_adrv904x_LnaEnOutputs_e lnaEnId,
                                                                         uint8_t * const activate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, activate, cleanup);
    
    *activate = ADI_FALSE;
    
    /* Range check lnaEnId value according to adi_adrv904x_LnaEnOutputs_e enum */
    if (((lnaEnId >> 8U) != 0U) ||
        ((lnaEnId & (lnaEnId - 1)) != 0) ||
         (lnaEnId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lnaEnId,
                               "lnaEnId selected value is invalid.");
        goto cleanup;
    }
     
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_LNA_EN_ALWAYS_ON,
                                                     &regVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while getting LNA EN ALWAYS ON.");
        goto cleanup;
    }
    
    if ((regVal & lnaEnId) != 0)
    {
        *activate = ADI_TRUE;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOffSet(adi_adrv904x_Device_t* const      device,
                                                                         const uint32_t lnaEnMask,
                                                                         const uint8_t activate)
{
        const uint8_t MAX_SIGNAL = 8U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t lnaEnWithAssignedGpiosBitmask = 0;
    uint8_t signalOffset = 0U;
    uint8_t isDigital = 0U;
    uint8_t lnaEnAlwaysOffFlags = 0U;
    uint8_t lnaEnAlwaysOnFlags = 0U;
    uint8_t lnaEnAffectedByAlarm = 0U;
    uint8_t lnaEnOutputBm = 0U;
    adi_adrv904x_OutGpioPinStates_e gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check lnaEnMask value according to adi_adrv904x_LnaEnOutputs_e enum */
    if (((lnaEnMask >> 8U) != 0U) ||
         (lnaEnMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lnaEnMask,
                               "lnaEnMask selected value is invalid.");
        goto cleanup;
    }

    /* Get the ALWAYS OFF flag for all LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_OFF,
                                                    &lnaEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting LNA EN ALWAYS OFF.");
        goto cleanup;
    }

    /* Get the ALWAYS ON flag for all LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_ON,
                                                    &lnaEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting LNA EN ALWAYS ON.");
        goto cleanup;
    }

    /* Create a bitmap of all LNA_EN which have GPIO pin assigned */
    for (signalOffset = 0; signalOffset < MAX_SIGNAL; signalOffset++)
    {
        if ((device->devStateInfo.alarmGpioConfig.lnaEnOutDigGpio[signalOffset] == ADI_ADRV904X_GPIO_INVALID) &&
            (device->devStateInfo.alarmGpioConfig.lnaEnOutAnaGpio[signalOffset] == ADI_ADRV904X_GPIO_ANA_INVALID))
        {
            lnaEnWithAssignedGpiosBitmask &= ~(1 << signalOffset);
        }
        else
        {
            lnaEnWithAssignedGpiosBitmask |= (1 << signalOffset);
        }
    }

    if (activate == ADI_TRUE)
    {
        /* Check if any if the reuested LNA_EN signals has ALWAYS_ON flag already set */
        if ((lnaEnMask & lnaEnAlwaysOnFlags) != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   lnaEnMask,
                                   "One of the selected LNA EN signals has ALWAYS ON flag set.");
            goto cleanup;
        }

        /* Update the Always OFF flag */
        lnaEnAlwaysOffFlags |= lnaEnMask;
        
        /* Set pin state to the value selected in the device profile */
        switch (device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode,
                                       "Unexpected LNA EN pin mode.");
                goto cleanup;
        }

        /* Update the output for all LNA_EN specified in the request which have GPIO pin assigned and set the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnMask & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            2);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN ALWAYS ON outputs.");
            goto cleanup;
        }
    }
    else
    {
        /* Update the Always OFF flag */
        lnaEnAlwaysOffFlags &= ~lnaEnMask;


        /* Only update the LNA_EN outputs which are not frozen by Always ON flag */
        lnaEnOutputBm = lnaEnMask & ~lnaEnAlwaysOnFlags;

        /* Check which LNA_EN outputs are affected by an active alarm */
        recoveryAction = adrv904x_RadioCtrlDbgGpioGetLnaEnAffectedByAlarms(device,
                                                                           &lnaEnAffectedByAlarm);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting bitmap of LNA_EN outputs currently affected by active alarm.");
            goto cleanup;
        }

        /* First, handle the LNA_EN outputs affected by active alarms */
 
        /* Change pin state to the deactive state */
        switch (device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.lnaEnOutGpioPinMode,
                                       "Unexpected LNA EN pin mode.");
                goto cleanup;
        }

        /* Update the LNA_EN outputs affected by active alarms which have GPIO pin assigned and clear the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnOutputBm & lnaEnAffectedByAlarm & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN outputs affected by alarms.");
            goto cleanup;
        }

        /* Then, the rest of the outputs can be set to normal operation */

        /* Set pin state to Normal Operation (RS controlled) and clear the debug flag */
        gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_LNA_EN,
                                                            (lnaEnOutputBm & ~lnaEnAffectedByAlarm & lnaEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating LNA EN outputs not affected by alarms.");
            goto cleanup;
        }
    }

    /* Write back the value of the ALWAYS OFF flag for LNA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_LNA_EN_ALWAYS_OFF,
                                                    lnaEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting LNA EN ALWAYS OFF.");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgLnaAlwaysOffGet(adi_adrv904x_Device_t* const      device,
                                                                         const adi_adrv904x_LnaEnOutputs_e lnaEnId,
                                                                         uint8_t * const activate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, activate, cleanup);
    
    *activate = ADI_FALSE;
    
    /* Range check lnaEnId value according to adi_adrv904x_LnaEnOutputs_e enum */
    if (((lnaEnId >> 8U) != 0U) ||
        ((lnaEnId & (lnaEnId - 1)) != 0) ||
         (lnaEnId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lnaEnId,
                               "lnaEnId selected value is invalid.");
        goto cleanup;
    }
     
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_LNA_EN_ALWAYS_OFF,
                                                     &regVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading LNA EN ALWAYS OFF.");
        goto cleanup;
    }
    
    if ((regVal & lnaEnId) != 0)
    {
        *activate = ADI_TRUE;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOnSet(adi_adrv904x_Device_t* const      device,
                                                                        const uint32_t paEnMask,
                                                                        const uint8_t activate)
{
        const uint8_t MAX_SIGNAL = 8U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t paEnWithAssignedGpiosBitmask = 0;
    uint8_t signalOffset = 0U;
     uint8_t isDigital = 0U;
    uint8_t paEnAlwaysOffFlags = 0U;
    uint8_t paEnAlwaysOnFlags = 0U;
    uint8_t paEnAffectedByAlarm = 0U;
    uint8_t paEnOutputBm = 0U;
    adi_adrv904x_OutGpioPinStates_e gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check lnaEnMask value according to adi_adrv904x_PaEnOutputs_e enum */
    if (((paEnMask >> 8U) != 0U) ||
         (paEnMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnMask,
                               "paEnMask selected value is invalid.");
        goto cleanup;
    }

    /* Get the ALWAYS OFF flag for all PA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_PA_EN_ALWAYS_OFF,
                                                    &paEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting PA EN ALWAYS OFF.");
        goto cleanup;
    }

    /* Get the ALWAYS ON flag for all PA_EN signals */
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_ALWAYS_ON,
                                                     &paEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting PA EN ALWAYS ON.");
        goto cleanup;
    }

    /* Create a bitmap of all PA_EN which have GPIO pin assigned */
    for (signalOffset = 0; signalOffset < MAX_SIGNAL; signalOffset++)
    {
        if ((device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[signalOffset] == ADI_ADRV904X_GPIO_INVALID) &&
            (device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[signalOffset] == ADI_ADRV904X_GPIO_ANA_INVALID))
        {
            paEnWithAssignedGpiosBitmask &= ~(1 << signalOffset);
        }
        else
        {
            paEnWithAssignedGpiosBitmask |= (1 << signalOffset);
        }
    }

    if (activate == ADI_TRUE)
    {
        /* Check if any if the requested PA_EN signals has ALWAYS_OFF flag already set */
        if ((paEnMask & paEnAlwaysOffFlags) != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT( &device->common,
                                   recoveryAction,
                                   paEnMask,
                                   "One of the selected PA EN signals has ALWAYS OFF flag set.");
            goto cleanup;
        }
        
        /* Update the Always ON flag */
        paEnAlwaysOnFlags |= paEnMask;

        /* Set pin state to the value selected in the device profile */
        switch (device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode,
                                       "Unexpected PA EN pin mode.");
                goto cleanup;
        }

        /* Update the output for all PA_EN specified in the request which have GPIO pin assigned and set the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnMask & paEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            2);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN ALWAYS ON outputs.");
            goto cleanup;
        }
    }
    else
    {
        /* Update the Always ON flag */
        paEnAlwaysOnFlags &= ~paEnMask;

        /* Only update the PA_EN outputs which are not frozen by Always OFF flag */
        paEnOutputBm = paEnMask & ~paEnAlwaysOffFlags;

        /* Check which PA_EN outputs are affected by an active alarm */
        recoveryAction = adrv904x_RadioCtrlDbgGpioGetPaEnAffectedByAlarms(device,
                                                                          &paEnAffectedByAlarm);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting bitmap of PA_EN outputs currently affected by active alarm.");
            goto cleanup;
        }

        /* First, handle the PA_EN outputs affected by active alarms */
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM,
                                                        (paEnOutputBm & paEnAffectedByAlarm));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while setting ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM.");
            goto cleanup;
        }

        /* Trigger stream to mute TXes affected by active alarms */
        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_EXT_ALARM_UPDATE_ALARM_OUTPUTS);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while triggering Core Stream to update alarm outputs.");
            goto cleanup;
        }

        /* Change pin state to the deactive state */
        switch (device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode,
                                       "Unexpected PA EN pin mode.");
                goto cleanup;
        }

        /* Update the PA_EN outputs affected by active alarms which have GPIO pin assigned and clear the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnOutputBm & paEnAffectedByAlarm & paEnWithAssignedGpiosBitmask), 
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN outputs affected by alarms.");
            goto cleanup;
        }

        /* Then, the rest of the outputs can be set to normal operation */

        /* Set pin state to Normal Operation (RS controlled) and clear the debug flag */
        gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnOutputBm & ~paEnAffectedByAlarm & paEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN outputs not affected by alarms.");
            goto cleanup;
        } 
    }

    /* Write back the value of the ALWAYS ON flag for PA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_PA_EN_ALWAYS_ON,
                                                    paEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting PA EN ALWAYS ON.");
        goto cleanup;
    }

    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOnGet(adi_adrv904x_Device_t* const      device,
                                                                        const adi_adrv904x_PaEnOutputs_e  paEnId,
                                                                        uint8_t * const                   activate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, activate, cleanup);
    
    *activate = ADI_FALSE;
    
    /* Range check lnaEnId value according to adi_adrv904x_PaEnOutputs_e enum */
    if (((paEnId >> 8U) != 0U) ||
        ((paEnId & (paEnId - 1)) != 0) ||
         (paEnId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnId,
                               "paEnId selected value is invalid.");
        goto cleanup;
    }
     
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_ALWAYS_ON,
                                                     &regVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading PA EN ALWAYS ON.");
        goto cleanup;
    }
    
    if ((regVal & paEnId) != 0)
    {
        *activate = ADI_TRUE;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOffSet(adi_adrv904x_Device_t* const      device,
                                                                         const uint32_t                    paEnMask,
                                                                         const uint8_t                     activate) 
{
        const uint8_t MAX_SIGNAL = 8U;
    adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t paEnWithAssignedGpiosBitmask = 0;
    uint8_t signalOffset = 0U;
    uint8_t isDigital = 0U;
    uint8_t paEnAlwaysOffFlags = 0U;
    uint8_t paEnAlwaysOnFlags = 0U;
    uint8_t paEnAffectedByAlarm = 0U;
    uint8_t paEnOutputBm = 0U;
    adi_adrv904x_OutGpioPinStates_e gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check lnaEnMask value according to adi_adrv904x_PaEnOutputs_e enum */
    if (((paEnMask >> 8U) != 0U) ||
         (paEnMask == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnMask,
                               "paEnMask selected value is invalid.");
        goto cleanup;
    }

    /* Get the ALWAYS OFF flag for all PA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_PA_EN_ALWAYS_OFF,
                                                    &paEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting PA EN ALWAYS OFF.");
        goto cleanup;
    }

    /* Get the ALWAYS ON flag for all PA_EN signals */
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_ALWAYS_ON,
                                                     &paEnAlwaysOnFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while getting PA EN ALWAYS ON.");
        goto cleanup;
    }

    /* Create a bitmap of all PA_EN which have GPIO pin assigned */
    for (signalOffset = 0; signalOffset < MAX_SIGNAL; signalOffset++)
    {
        if ((device->devStateInfo.alarmGpioConfig.paEnOutDigGpio[signalOffset] == ADI_ADRV904X_GPIO_INVALID) &&
            (device->devStateInfo.alarmGpioConfig.paEnOutAnaGpio[signalOffset] == ADI_ADRV904X_GPIO_ANA_INVALID))
        {
            paEnWithAssignedGpiosBitmask &= ~(1 << signalOffset);
        }
        else
        {
            paEnWithAssignedGpiosBitmask |= (1 << signalOffset);
        }
    }

    if (activate == ADI_TRUE)
    {   
        /* Check if any if the reuested PA_EN signals has ALWAYS_ON flag already set */
        if ((paEnMask & paEnAlwaysOnFlags) != 0)
        {
            recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
            ADI_PARAM_ERROR_REPORT(&device->common,
                                   recoveryAction,
                                   paEnMask,
                                   "One of the selected PA EN signals has ALWAYS ON flag set.");
            goto cleanup;
        }

        /* Update the Always OFF flag */
        paEnAlwaysOffFlags |= paEnMask;

        /* Set pin state to the value selected in the device profile */
        switch (device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode,
                                       "Unexpected PA EN pin mode.");
                goto cleanup;
        }

        /* Update the output for all PA_EN specified in the request which have GPIO pin assigned and set the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnMask & paEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            2);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN outputs.");
            goto cleanup;
        }
    }
    else
    {   
        /* Update the Always OFF flag */
        paEnAlwaysOffFlags &= ~paEnMask;
        
        /* Only update the PA_EN outputs which are not frozen by Always ON flag */
        paEnOutputBm = paEnMask & ~paEnAlwaysOnFlags;

        /* Check which PA_EN outputs are affected by active alarms */
        recoveryAction = adrv904x_RadioCtrlDbgGpioGetPaEnAffectedByAlarms(device,
                                                                          &paEnAffectedByAlarm);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while getting bitmap of PA_EN outputs currently affected by active alarms.");
            goto cleanup;
        }

        /* First, handle the PA_EN outputs affected by active alarms */        
        recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM,
                                                        (paEnOutputBm & paEnAffectedByAlarm));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while setting ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM.");
            goto cleanup;
        }

        /* Trigger stream to mute TXes affected by active alarms */
        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_EXT_ALARM_UPDATE_ALARM_OUTPUTS);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while triggering Core Stream to update alarm outputs.");
            goto cleanup;
        }

        /* Change pin state to the deactive state */
        switch (device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode)
        {
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_LOW):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_OFF;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_ACTIVE_HIGH):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_ON;
                break;
            case (ADI_ADRV904X_OUT_GPIO_PIN_MODE_HIGH_Z):
                gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_HIGH_Z;
                break;
            default:
                recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
                ADI_PARAM_ERROR_REPORT(&device->common,
                                       recoveryAction,
                                       device->devStateInfo.alarmGpioConfig.paEnOutGpioPinMode,
                                       "Unexpected PA EN pin mode.");
                goto cleanup;
        }

        /* Update the PA_EN outputs affected by active alarms which have GPIO pin assigned and clear the debug flag */
        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnOutputBm & paEnAffectedByAlarm & paEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN outputs affected by alarms.");
            goto cleanup;
        }

        /* Then, the rest of the outputs can be set to normal operation */

        /* Set pin state to Normal Operation (RS controlled) and clear the debug flag */
        gpioPinstate = ADI_ADRV904X_OUT_GPIO_PIN_STATE_NORMAL_OP;

        recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device,
                                                            ADI_ADRV904X_OUT_GPIO_SIG_TYPE_PA_EN,
                                                            (paEnOutputBm & ~paEnAffectedByAlarm & paEnWithAssignedGpiosBitmask),
                                                            gpioPinstate,
                                                            isDigital,
                                                            1);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while updating PA EN outputs not affected by alarms.");
            goto cleanup;
        }
    }

    /* Update the ALWAYS OFF flag for PA_EN signals */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_EA_PA_EN_ALWAYS_OFF,
                                                    paEnAlwaysOffFlags);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting PA EN ALWAYS OFF.");
        goto cleanup;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgPaAlwaysOffGet(adi_adrv904x_Device_t* const      device,
                                                                         const adi_adrv904x_PaEnOutputs_e  paEnId,
                                                                         uint8_t * const                   activate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, activate, cleanup);
    
    *activate = ADI_FALSE;
    
    /* Range check lnaEnId value according to adi_adrv904x_PaEnOutputs_e enum */
    if (((paEnId >> 8U) != 0U) ||
        ((paEnId & (paEnId - 1)) != 0) ||
         (paEnId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               paEnId,
                               "paEnId selected value is invalid.");
        goto cleanup;
    }
     
    recoveryAction =  adrv904x_Core_ScratchReg_BfGet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_PA_EN_ALWAYS_OFF,
                                                     &regVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while reading PA EN ALWAYS OFF.");
        goto cleanup;
    }
    
    if ((regVal & paEnId) != 0)
    {
        *activate = ADI_TRUE;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgGpioOutputSet(adi_adrv904x_Device_t* const device,
                                                                        const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                        const uint8_t signalMask,
                                                                        const adi_adrv904x_OutGpioPinStates_e gpioPinstate)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t isDigital = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Validate the input parameters */
    recoveryAction = adrv904x_RadioCtrlDbgGpioOutputRangeCheck(device, gpioSigType, signalMask, ADI_TRUE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Invalid parameters.");
        goto cleanup;
    }

    /* Setting the registers */
    recoveryAction = adrv904x_RadioCtrlDbgGpioOutputSet(device, gpioSigType, signalMask, gpioPinstate, isDigital, 0);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
            recoveryAction,
            "Failed to set registers.");
        goto cleanup;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

    ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlDbgGpioOutputGet(adi_adrv904x_Device_t* const device,
                                                                            const adi_adrv904x_OutGpioSigTypes_e gpioSigType,
                                                                            const uint8_t signalMask,
                                                                            adi_adrv904x_OutGpioPinStates_e * const gpioPinstate)
    {
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t gpioIdx = 0U;
    uint8_t isDigital = 0U;
    uint8_t i = 0U;
    uint8_t signalOffset = 0U;
    const uint8_t MAX_SIGNAL = 8U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, gpioPinstate, cleanup);
    
    /* Validate the input parameters */
    recoveryAction = adrv904x_RadioCtrlDbgGpioOutputRangeCheck(device, gpioSigType, signalMask, ADI_FALSE);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                                recoveryAction,
                                "Invalid parameters.");
        goto cleanup;
    }
    
    /* Find the signal offset based on mask selection */
    for (i = 0; i < MAX_SIGNAL; i++)
    {
        if ((signalMask & (1 << i)) != 0U)
        {
            signalOffset = i;
            break;
        }
    }
        
    /* Based on the input parameters, find the GPIO Index in the GPIO Pins Configuration */
    recoveryAction = adrv904x_RadioCtrlDbgGpioOutputFind(device, gpioSigType, signalOffset, &gpioIdx, &isDigital);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                                recoveryAction,
                                "Failed to find GPIO based on the input parameters.");
        goto cleanup;
    }
    
    /* Get the GPIO Pin State */
    recoveryAction = adrv904x_RadioCtrlDbgGpioOutputGet(device, gpioSigType, signalOffset, gpioPinstate, gpioIdx, isDigital);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                                recoveryAction,
                                "Failed to get the GPIO Pin State.");
        goto cleanup;
    }
        
        cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);

}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlOcFuseEnOutSet(adi_adrv904x_Device_t* const      device,
                                                                      const adi_adrv904x_Alarms_e       alarmId,
                                                                      const adi_adrv904x_OutputState_e  state)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint8_t regVal = 0U;
    uint8_t alarmIdx = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check alarmId value according to adi_adrv904x_Alarms_e enum */
    if (((alarmId >> ADI_ADRV904X_ALARM_MAX) != 0U) ||
        ((alarmId & (alarmId - 1)) != 0) ||
         (alarmId == 0U))
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               alarmId,
                               "alarmId selected value is invalid.");
        goto cleanup;
    }
     
    for (alarmIdx = 0U; alarmIdx < ADI_ADRV904X_ALARM_MAX; alarmIdx++)
    {
        if (((uint32_t)alarmId & ((uint32_t)1 << alarmIdx)) > 0)
        {
            break; 
        }
    }
    
    regVal = alarmIdx;
    if (state == ADI_ADRV904X_OUTPUT_ENABLE)
    {
        /* set the msb if it is enable. */
        regVal |= 0x80U; 
    }
    
    /* Write alarm ID and state */
    recoveryAction =  adrv904x_Core_ScratchReg_BfSet(device,
                                                     NULL,
                                                     ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                     ADRV904X_EA_TMP_ALARM_ID_LNA_PA_BM,
                                                     regVal);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while setting Alarm Id and state.");
        goto cleanup;
    }
     
    /* Trigger stream */
    recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_OC_FUSE_EN_OUT);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                             recoveryAction,
                             "Error while triggering Core Stream to apply OC Fuse En Out State.");
        goto cleanup;
    }
    
    cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgInitSet(adi_adrv904x_Device_t* const device,
                                                                               const adi_adrv904x_RadioCtrlAntCalCarrierCfgInit_t* const antCalCarrierCfgInit)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U;
    
    const adrv904x_BfStreamprocChannelChanAddr_e rxStreamProcChannelBaseAddr[ADI_ADRV904X_MAX_RX_ONLY] =
    {
        ADRV904X_BF_SLICE_RX_0__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_1__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_2__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_3__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_4__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_5__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_6__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_RX_7__SLICE_AHB_STREAM_PROC_CHANNEL
    };

    const adrv904x_BfStreamprocChannelChanAddr_e txStreamProcChannelBaseAddr[ADI_ADRV904X_MAX_TXCHANNELS] =
    {
        ADRV904X_BF_SLICE_TX_0__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_1__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_2__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_3__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_4__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_5__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_6__SLICE_AHB_STREAM_PROC_CHANNEL,
        ADRV904X_BF_SLICE_TX_7__SLICE_AHB_STREAM_PROC_CHANNEL
    };

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierCfgInit, cleanup);

    /* Write common Rx channel mask to scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_COMMON_RX_CHANNEL_MASK,
                                                    (uint8_t)(antCalCarrierCfgInit->commonRxMask));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write common Rx channel mask.");
        goto cleanup;
    }

    /* Write common Tx channel mask to scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_COMMON_TX_CHANNEL_MASK,
                                                    (uint8_t)(antCalCarrierCfgInit->commonTxMask));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write common Tx channel mask.");
        goto cleanup;
    }


    for (i = 0U; i < ADI_ADRV904X_MAX_RX_ONLY; i++)
    {
        if ((antCalCarrierCfgInit->commonRxMask & (1U << i)) == 0U)
        {              
            recoveryAction = adrv904x_StreamprocChannel_MaskChAntRiseStreamToCore_BfSet(device, NULL, rxStreamProcChannelBaseAddr[i], 0x1U);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Rx stream proc channel rise to core failure");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_StreamprocChannel_MaskChAntFallStreamToCore_BfSet(device, NULL, rxStreamProcChannelBaseAddr[i], 0x1U);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Rx stream proc channel fall to core failure");
                goto cleanup;
            }
        }

    }

    for (i = 0U; i < ADI_ADRV904X_MAX_TXCHANNELS; i++)
    {      
        if ((antCalCarrierCfgInit->commonTxMask & (1U << i)) == 0U)
        {
            recoveryAction = adrv904x_StreamprocChannel_MaskChAntRiseStreamToCore_BfSet(device, NULL, txStreamProcChannelBaseAddr[i], 0x1U);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Tx stream proc channel rise to core failure");
                goto cleanup;
            }
            
            recoveryAction = adrv904x_StreamprocChannel_MaskChAntFallStreamToCore_BfSet(device, NULL, txStreamProcChannelBaseAddr[i], 0x1U);
            if (ADI_ADRV904X_ERR_ACT_NONE != recoveryAction)
            {
                ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Tx stream proc channel fall to core failure");
                goto cleanup;
            }
        }
    }

    /* Trigger stream */
    recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_ANT_CAL_UPDATE_SLICE_SCRATCHPAD_REG);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while triggering Core Stream to update the Rx/Tx channel mask");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgSet(adi_adrv904x_Device_t* const device,
                                                                           const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierCfg, cleanup);

    recoveryAction = adi_adrv904x_RadioCtrlAntCalCarrierConfigSet(device, antCalCarrierCfg, 0, ADI_FALSE); /* parameter validation is done in this helper function */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Control Antenna Calibration Carrier Configuration.");
        return recoveryAction;
    }
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgSet_v2(adi_adrv904x_Device_t* const device,
                                                                              const adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg,
                                                                              uint32_t postCalPatternId)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierCfg, cleanup);
    if ((postCalPatternId >= device->devStateInfo.rsImageHeader.numberPatterns) || (postCalPatternId >= ADI_ADRV904X_RADIO_SEQ_LOOKUP_TAB_MAX_SIZE))
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common,
                                recoveryAction,
                                postCalPatternId,
                                "Invalid input postCalPatternId. \n");
        goto cleanup;
    }

    recoveryAction = adi_adrv904x_RadioCtrlAntCalCarrierConfigSet(device, antCalCarrierCfg, postCalPatternId, ADI_TRUE); /* parameter validation is done in this helper function */
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while setting Radio Control Antenna Calibration Carrier Configuration.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierCfgGet(adi_adrv904x_Device_t* const device,
                                                                           adi_adrv904x_RadioCtrlAntCalCarrierCfg_t* const antCalCarrierCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t i = 0U;
    uint8_t value = 0U;
    int32_t carrierGain = 0;
    uint32_t carrierGainReg = 0U;
    const uint32_t DIG_GAIN_MULT = 65536U;
    uint32_t extendedCoreScratchRegBaseAddress = 0U;
    uint32_t tempAddress = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierCfg, cleanup);

    ADI_LIBRARY_MEMSET(antCalCarrierCfg, 0, sizeof(adi_adrv904x_RadioCtrlAntCalCarrierCfg_t));

    /* Read pattern ID from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_PATTERN_ID_TO_SWITCH_BYTE_0,
                                                    &value);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read patternId");
        goto cleanup;
    }

    antCalCarrierCfg->patternId = (uint32_t)value;

    /* Read GPIO number from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_NUMBER_OF_GPIO_TO_SWITCH_CARRIER,
                                                    &(antCalCarrierCfg->numOfGpioToSwitchCarrier));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read numOfGpioToSwitchCarrier");
        goto cleanup;
    }

    /* Read carrier table from scratch reg*/
    for (i = 0U; i < ADI_ADRV904X_CARRIER_TABLE_COUNT; i++)
    {
        recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                        NULL,
                                                        ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                        (uint16_t)(ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_BYTE_0 + i),
                                                        &(antCalCarrierCfg->carrierTable[i]));
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_PARAM_ERROR_REPORT(&device->common, recoveryAction, i, "Error while read carrierTable");
            goto cleanup;
        }
    }

    /* Read first pattern number from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_FIRST_PATTERN_NUMBER_OF_LOOPS,
                                                    &(antCalCarrierCfg->firstPatternNumberOfLoops));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read firstPatternNumberOfLoops");
        goto cleanup;
    }

    /* Increment the value to match the decrement done by the set function */
    antCalCarrierCfg->firstPatternNumberOfLoops += 1;

    /* Read second pattern number from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_SECOND_PATTERN_NUMBER_OF_LOOPS,
                                                    &(antCalCarrierCfg->secondPatternNumberOfLoops));
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read secondPatternNumberOfLoops");
        goto cleanup;
    }

    /* Increment the value to match the decrement done by the set function */
    antCalCarrierCfg->secondPatternNumberOfLoops += 1;
    
    /* Read the base address for extended core scratch registers located in M4 heap*/
    recoveryAction = adi_adrv904x_Register32Read(device, NULL, ADRV904X_PM_CORE_SCRATCH_EXT_PTR, &extendedCoreScratchRegBaseAddress, 0xFFFFFFFF);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading extended core stream scratch base addr");
        goto cleanup;
    }

    for (i = 0U; i < ADI_ADRV904X_MAX_RX_CARRIERS; i++)
    {
        carrierGainReg = 0U;
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_ANTENNA_CAL_RX_CARRIER_0_GAIN + i);
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &carrierGainReg, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Rx carrier gain byte.");
            goto cleanup;
        }
        
        /* Mask first 23 bits of register */
        carrierGainReg &= 0x3FFFFF;
        if (carrierGainReg != 0U)
        {
            /* Convert from 7.16 to mdB.  value in mdB = (1000*20*log10(reg value/2^16)) */
            //carrierGain = (int32_t)(1000U * 20U * (double)log10((double)carrierGainReg / DIG_GAIN_MULT));
        	carrierGain = (int32_t)(20 * log10((100000UL * (double)carrierGainReg) / DIG_GAIN_MULT) - 100);
        }

        antCalCarrierCfg->rxCarrierGainForAntCal[i] = carrierGain;
    }


    for (i = 0U; i < ADI_ADRV904X_MAX_TX_CARRIERS; i++)
    {
        carrierGainReg = 0U;
        tempAddress = extendedCoreScratchRegBaseAddress + 4U * (ADRV904X_ANTENNA_CAL_TX_CARRIER_0_GAIN + i);
        recoveryAction = adi_adrv904x_Register32Read(device, NULL, tempAddress, &carrierGainReg, 0xFFFFFFFF);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while reading Tx carrier gain byte.");
            goto cleanup;
        }
        
        /* Mask first 23 bits of register */
        carrierGainReg &= 0x3FFFFF;

        if (carrierGainReg != 0)
        {
            /* Convert from 7.16 to mdB.  value in mdB = (1000*20*log10(reg value/2^16)) */
            carrierGain = (int32_t)(1000U * 20U * (double)log10((double)carrierGainReg / DIG_GAIN_MULT));
        }

        antCalCarrierCfg->txCarrierGainForAntCal[i] = carrierGain;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierGainCountersClear(adi_adrv904x_Device_t* const device)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    /* Clear GPIO counter */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_GPIO_RISING_EDGE_COUNTER,
                                                    0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write antenna cal GPIO counter");
        goto cleanup;
    }

    /* Clear gain table index */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_INDEX,
                                                    0U);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while write gain table index");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalCarrierGainCountersGet(adi_adrv904x_Device_t* const device,
                                                                                    uint8_t* const antCalGpioCounter,
                                                                                    uint8_t* const antCalCarrierTableIndex)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalGpioCounter, cleanup);
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, antCalCarrierTableIndex, cleanup);

    /* Read GPIO counter from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_GPIO_RISING_EDGE_COUNTER,
                                                    antCalGpioCounter);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read antenna cal GPIO counter");
        goto cleanup;
    }

    /* Read gain table index from scratch reg */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    (uint16_t)ADRV904X_ANTENNA_CAL_CARRIER_GAIN_TABLE_INDEX,
                                                    antCalCarrierTableIndex);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Error while read gain table index");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}



ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingConfigGet(adi_adrv904x_Device_t* const                       device,
                                                                             adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t * const mappingConfig)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxToOrxMappingMode_e lclMode = device->devStateInfo.dfeCtrlTxToOrxMappingConfig.mode;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common) ;
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, mappingConfig, cleanup);
    
    /* Check that Mapping Mode DFE CTRL. Otherwise throw an error */
    if (lclMode != ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                               recoveryAction,
                               lclMode,
                               "Device Tx to Orx Mapping Mode is for a RCI mode. This function can only be used to readback a DFE Ctrl Mode mapping configuration. It cannot be used to readback a RCI mapping configuration.");
        goto cleanup;
    }

    /* Grab configuration stored in the device handle*/
    ADI_LIBRARY_MEMCPY(mappingConfig, &device->devStateInfo.dfeCtrlTxToOrxMappingConfig, sizeof(adi_adrv904x_DfeCtrlTxToOrxMappingConfig_t));

    /*Success*/
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}


ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingSet(adi_adrv904x_Device_t* const     device,
                                                                       const adi_adrv904x_RxChannels_e  orxChannel,
                                                                       const adi_adrv904x_TxChannels_e  txChannel)

{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_DfeCtrlTxToOrxMappingBaseSet(device, orxChannel, txChannel, 0U, ADI_ADRV904X_DFE_VSWR_FWD);
    ADI_API_ERROR_REPORT( &device->common,
                         recoveryAction,
                         "Error while applying mapping.");
    goto cleanup;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingAndVswrDirSet(adi_adrv904x_Device_t* const           device,
                                                                                 const adi_adrv904x_RxChannels_e        orxChannel,
                                                                                 const adi_adrv904x_TxChannels_e        txChannel,
                                                                                 const adi_adrv904x_DfeVswrDirection_e  vswrDirection)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    recoveryAction = adrv904x_DfeCtrlTxToOrxMappingBaseSet(device, orxChannel, txChannel, 1U, vswrDirection);
    ADI_API_ERROR_REPORT( &device->common,
                         recoveryAction,
                         "Error while applying mapping,and VSWR direction.");
    goto cleanup;

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_DfeCtrlTxToOrxMappingTxOffLevelsSet(adi_adrv904x_Device_t* const       device,
                                                                                  const adi_adrv904x_RxChannels_e    orxChannel,
                                                                                  const uint8_t                      txOffGpioLevels)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    adi_adrv904x_TxToOrxMappingMode_e lclMode = device->devStateInfo.dfeCtrlTxToOrxMappingConfig.mode;
    uint16_t channelId = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Check that Mapping Mode DFE CTRL. Otherwise throw an error */
    if (lclMode != ADI_ADRV904X_TX_ORX_MAPPING_MODE_DFE_CTRL)
    {
        ADI_PARAM_ERROR_REPORT( &device->common,
                                 recoveryAction,
                                 lclMode,
                                 "Device Tx to Orx Mapping Mode is a RCI mode. This function can only be used for DFE Control mode.");
        goto cleanup;
    }

    /* Range check txOffGpioLevels */
    if ((txOffGpioLevels & 0x3F) != txOffGpioLevels)
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               txOffGpioLevels,
                               "txOffGpioLevels is invalid. Must select a 6bit value.") ;
        goto cleanup;
    }

    if (orxChannel == ADI_ADRV904X_ORX0)
    {
        channelId = ADRV904X_CPU_TX_ORX_MAPPING_PIN_STATE_ORX0_ALL_OFF;
    }
    else if (orxChannel == ADI_ADRV904X_ORX1)
    {
        channelId = ADRV904X_CPU_TX_ORX_MAPPING_PIN_STATE_ORX1_ALL_OFF;
    }
    else
    {
        ADI_PARAM_ERROR_REPORT(&device->common,
                               recoveryAction,
                               orxChannel,
                               "ORx Channel selected is invalid. Must select ORx0-1.");
        goto cleanup;
    }

    /* Update the requested state to scratch regs */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    (adrv904x_BfCoreChanAddr_e)ADRV904X_BF_CORE_ADDR,
                                                    channelId,
                                                    txOffGpioLevels);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "Error while setting Tx to ORx mapping selection.");
        goto cleanup;
    }

cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalOutGpioCfgGet(adi_adrv904x_Device_t* const                 device, 
                                                                           adi_adrv904x_StreamGpioAntCalOutCfg_t* const pCfg)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    ADI_ADRV904X_NULL_PTR_REPORT_GOTO(&device->common, pCfg, cleanup);
    
    /* Memcpy stored cfg structure from device handle directly to return struct */
    ADI_LIBRARY_MEMCPY(pCfg, &device->devStateInfo.streamGpioAntCalOutCfg, sizeof(adi_adrv904x_StreamGpioAntCalOutCfg_t));

    /*Success*/
    recoveryAction = ADI_ADRV904X_ERR_ACT_NONE;
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalOutGpioLevelSet(adi_adrv904x_Device_t* const   device,
                                                                             const uint32_t                 drive)
{
        adi_adrv904x_ErrAction_e recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
    uint32_t bitIdx = 0U;
    uint32_t bitVal = 0U;
    uint32_t gpioIdx = 0U;
    uint32_t tmpBit = 0U;
    uint32_t digSet = 0U;
    uint32_t digClr = 0U;
    uint32_t anaSet = 0U;
    uint32_t anaClr = 0U;
    
    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    
    ADI_ADRV904X_API_ENTRY(&device->common);
    
    /* Range check drive value */
    if (drive > (1u << ADI_ADRV904X_MAX_STREAM_GPIO_ANTENNA_CAL_OUT_PINS) - 1u)
    {
        recoveryAction = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;
        ADI_PARAM_ERROR_REPORT( &device->common, recoveryAction, drive, "Invalid drive value. Valid range is 0-255");
        goto cleanup;
    }
    
    /* Build up set/clr values to apply: Digital and Analog */
    for (bitIdx = 0U; bitIdx < ADI_ADRV904X_MAX_STREAM_GPIO_ANTENNA_CAL_OUT_PINS; bitIdx++)
    {
        /* Determine bitVal for this bitIdx */
        bitVal = (drive >> bitIdx) & 0x1;
        
        if ((device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] >= ADI_ADRV904X_GPIO_GENERIC_DIG_START) &&
            (device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] <= ADI_ADRV904X_GPIO_GENERIC_DIG_END))
        {
            /* Digital pin was selected for this bitIdx. */
            gpioIdx = (uint32_t)device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] - ADI_ADRV904X_GPIO_GENERIC_DIG_START;
            tmpBit = (1 << gpioIdx);
            
            /* OR in tmpBit to the set or clr value */
            if (bitVal == 1)
            {
                digSet |= tmpBit;
            }
            else
            {
                digClr |= tmpBit;
            }
        }
        else if ((device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] >= ADI_ADRV904X_GPIO_GENERIC_ANA_START) &&
                 (device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] <= ADI_ADRV904X_GPIO_GENERIC_ANA_END))
        {
            /* Analog pin was selected for this bitIdx. */
            gpioIdx = (uint32_t)device->devStateInfo.streamGpioAntCalOutCfg.gpioSelect[bitIdx] - ADI_ADRV904X_GPIO_GENERIC_ANA_START;
            tmpBit = (1 << gpioIdx);
            
            /* OR in tmpBit to the set or clr value */
            if (bitVal == 1)
            {
                anaSet |= tmpBit;
            }
            else
            {
                anaClr |= tmpBit;
            }
        }
        
    }
    
    /* Digital SET */
    recoveryAction = adrv904x_Core_GpioFromMasterSet_BfSet(device, 
                                                           NULL,
                                                           (adrv904x_BfCoreChanAddr_e) 0U,
                                                           digSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Digital GPIO Set issue");
        goto cleanup;
    }
    
    /* Digital CLR */
    recoveryAction = adrv904x_Core_GpioFromMasterClear_BfSet(device, 
                                                             NULL,
                                                             (adrv904x_BfCoreChanAddr_e) 0U,
                                                             digClr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Digital GPIO Clr issue");
        goto cleanup;
    }
    
    /* Analog SET */
    recoveryAction = adrv904x_Core_GpioAnalogFromMasterSet_BfSet(device, 
                                                                 NULL,
                                                                 (adrv904x_BfCoreChanAddr_e) 0U,
                                                                 (uint16_t)anaSet);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Analog GPIO Set issue");
        goto cleanup;
    }
    
    /* Analog CLR */
    recoveryAction = adrv904x_Core_GpioAnalogFromMasterClear_BfSet(device, 
                                                                   NULL,
                                                                   (adrv904x_BfCoreChanAddr_e) 0U,
                                                                   (uint16_t)anaClr);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT(&device->common, recoveryAction, "Analog GPIO Clr issue");
        goto cleanup;
    }
    
cleanup:
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e  adi_adrv904x_RadioCtrlAntCalGpioTruthTableSet(adi_adrv904x_Device_t* const device,
                                                                               uint8_t tableSelection)
{
        adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);

    if(tableSelection > 1)
    {
        ADI_API_ERROR_REPORT(&device->common,
                             recoveryAction,
                             "tableSelection can only be 0 or 1");
        goto cleanup;
    }

    /* Update scratch reg that switch GPIO true table selection */
    recoveryAction = adrv904x_Core_ScratchReg_BfSet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_ANTENNA_CAL_GPIO_TRUTH_TABLE_SEL,
                                                    tableSelection);
    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "adrv904x_Core_ScratchReg_BfSet(): Error");
        goto cleanup;
    }

    /* Trigger stream */
    if(tableSelection == 0)
    {
        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_MAIN_PATH_NORMAL_OPERATION);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while triggering Core Stream to update the GPIO true table");
            goto cleanup;
        }
    }

    if(tableSelection == 1)
    {
        recoveryAction = adrv904x_StreamTrigger(device, ADRV904X_STREAM_ID__CORE__ANT_CAL_SET_OUTPUT_GPIO_SECONDARY_PATH_NORMAL_OPERATION);
        if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
        {
            ADI_API_ERROR_REPORT(&device->common,
                                 recoveryAction,
                                 "Error while triggering Core Stream to update the GPIO true table");
            goto cleanup;
        }
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

ADI_API adi_adrv904x_ErrAction_e adi_adrv904x_RadioCtrlAntCalGpioTruthTableGet(adi_adrv904x_Device_t* const device,
                                                                              uint8_t* pTableSelection)
{
        adi_adrv904x_ErrAction_e  recoveryAction  = ADI_ADRV904X_ERR_ACT_CHECK_PARAM;

    /* Check device pointer is not null */
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(device);
    ADI_ADRV904X_API_ENTRY(&device->common);
    ADI_ADRV904X_NULL_DEVICE_PTR_RETURN(pTableSelection);

    /* Read scratch reg that stores GPIO true table selection */
    recoveryAction = adrv904x_Core_ScratchReg_BfGet(device,
                                                    NULL,
                                                    ADRV904X_BF_DIGITAL_CORE_SPI_ONLY_REGS,
                                                    ADRV904X_ANTENNA_CAL_GPIO_TRUTH_TABLE_SEL,
                                                    pTableSelection);

    if (recoveryAction != ADI_ADRV904X_ERR_ACT_NONE)
    {
        ADI_API_ERROR_REPORT( &device->common,
                               recoveryAction,
                               "Error while getting GPIO true table selection");
        goto cleanup;
    }

cleanup :
    ADI_ADRV904X_API_EXIT(&device->common, recoveryAction);
}

