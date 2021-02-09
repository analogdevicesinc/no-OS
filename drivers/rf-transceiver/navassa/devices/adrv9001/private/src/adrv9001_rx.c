/**
 * \file
 * \brief Contains ADRV9001 Rx related private function implementations
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

/* "adi_adrv9001_user.h" contains the #define that other header file use */
#include "adi_adrv9001_user.h"

/* Header file corresponding to the C file */
#include "adrv9001_rx.h"

/* ADI specific header files */
#include "adi_adrv9001_error.h"
#include "adi_adrv9001_gpio.h"
#include "adrv9001_init.h"
#include "adrv9001_reg_addr_macros.h"

int32_t adrv9001_RxGainTableFormat(adi_adrv9001_Device_t *device,
                                   adi_adrv9001_RxGainTableRow_t *gainTablePtr,
                                   uint8_t *formattedGainTablePtr,
                                   uint16_t numGainIndicesInTable)
{
    static const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8;
    static const uint16_t DIG_GAIN_LSB_MASK = 0x00FF;
    static const uint16_t DIG_GAIN_MSB_MASK = 0x0300;
    static const uint8_t  DIG_GAIN_MSB_SHIFT = 8;
    static const uint16_t DIG_GAIN_SIGN_MASK = 0x0400;
    static const uint8_t  DIG_GAIN_SIGN_SHIFT = 8;
    static const uint8_t  RXFE_GAIN_LSB_MASK = 0x1F;
    static const uint8_t  RXFE_GAIN_LSB_SHIFT = 3;
    static const uint8_t  RXFE_GAIN_MSB_MASK = 0xE0;
    static const uint8_t  RXFE_GAIN_MSB_SHIFT = 5;
    static const uint8_t  EXT_CTRL_MASK = 0x03;
    static const uint8_t  EXT_CTRL_SHIFT = 3;
    static const uint16_t PHASE_OFFSET_LSB_MASK = 0x00FF;
    static const uint16_t PHASE_OFFSET_MSB_MASK = 0xFF00;
    static const uint8_t  PHASE_OFFSET_MSB_SHIFT = 8;
    static const uint8_t  ADC_TIA_GAIN_MASK = 0x3F;
    static const uint8_t  ADC_TIA_GAIN_SHIFT = 0;

    uint32_t gainIndex = 0;
    uint32_t formattedGainIndex = 0;
    uint16_t sign  = 0;
    uint16_t magnitude = 0;

    /*Perform NULL checks on pointers*/
    ADI_ENTRY_PTR_EXPECT(device, gainTablePtr);

    ADI_NULL_PTR_RETURN(&device->common, formattedGainTablePtr);

    if (numGainIndicesInTable == 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            arraySize,
            "Invalid numGainIndicesInTable. Valid range is > 0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Format gain table*/
    for (gainIndex = 0; gainIndex < numGainIndicesInTable; gainIndex++)
    {
        /*Format Gain Table Data as follows :
        No. of Bytes per Gain Table Entry = 8
        @Addr 8 x gainIndex     : {11'b0 , ext_control[1:0], rx_fe_gain[7:0], dig_gain[10:0]}
        @Addr 8 x gainIndex + 4 : {10'b0, tia_control[4:0], adc_control, phase_offset[15:0]}
        */
        formattedGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);

        if (gainTablePtr[gainIndex].digGain < 0)
        {
            sign = DIG_GAIN_SIGN_MASK; /* set bit 11 */
            magnitude = (((~(uint16_t)gainTablePtr[gainIndex].digGain) + 1) & 0x3FF);
        }
        else
        {
            sign = 0;
            magnitude = gainTablePtr[gainIndex].digGain & 0x3FF;
        }

        formattedGainTablePtr[formattedGainIndex] = (uint8_t)(magnitude & DIG_GAIN_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 1] = (uint8_t)((sign & DIG_GAIN_SIGN_MASK) >> DIG_GAIN_SIGN_SHIFT) |
                                                        (uint8_t)((magnitude & DIG_GAIN_MSB_MASK) >> DIG_GAIN_MSB_SHIFT) |
                                                        (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_LSB_MASK) << RXFE_GAIN_LSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 2] = (uint8_t)((gainTablePtr[gainIndex].rxFeGain & RXFE_GAIN_MSB_MASK) >> RXFE_GAIN_MSB_SHIFT) |
                                                        (uint8_t)((gainTablePtr[gainIndex].extControl & EXT_CTRL_MASK) << EXT_CTRL_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 3] = 0;
        formattedGainTablePtr[formattedGainIndex + 4] = (uint8_t)(gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_LSB_MASK);
        formattedGainTablePtr[formattedGainIndex + 5] = (uint8_t)((gainTablePtr[gainIndex].phaseOffset & PHASE_OFFSET_MSB_MASK) >> PHASE_OFFSET_MSB_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 6] = (uint8_t)((gainTablePtr[gainIndex].adcTiaGain & ADC_TIA_GAIN_MASK) << ADC_TIA_GAIN_SHIFT);
        formattedGainTablePtr[formattedGainIndex + 7] = 0;
    }

    ADI_API_RETURN(device);
}


int32_t adrv9001_RxGainTableParse(adi_adrv9001_Device_t *device,
                                  adi_adrv9001_RxGainTableRow_t *gainTablePtr,
                                  uint8_t *armDmaDataGainTablePtr,
                                  uint16_t numGainIndicesInTable)
{
    static const uint32_t NUM_BYTES_PER_GAIN_TABLE_ENTRY = 8;
    static const uint16_t DIG_GAIN_MSB_MASK = 0x0003;
    static const uint8_t  DIG_GAIN_MSB_SHIFT = 8;
    static const uint8_t  RXFE_GAIN_LSB_MASK = 0xF8;
    static const uint8_t  RXFE_GAIN_LSB_SHIFT = 3;
    static const uint8_t  RXFE_GAIN_MSB_MASK = 0x07;
    static const uint8_t  RXFE_GAIN_MSB_SHIFT = 5;
    static const uint8_t  EXT_CTRL_MASK = 0x18;
    static const uint8_t  EXT_CTRL_SHIFT = 3;
    static const uint8_t  PHASE_OFFSET_MSB_SHIFT = 8;
    static const uint8_t  ADC_TIA_GAIN_MASK = 0x3F;
    static const uint8_t  ADC_TIA_GAIN_SHIFT = 0;
    /*11th bit for digital gain is the sign bit*/
    static const uint8_t DIG_GAIN_SIGN_MASK = 0x04;
    static const int16_t DIG_GAIN_SIGN_EXT = 0xFC00;

    uint32_t gainIndex = 0;
    uint32_t armDmaDataGainIndex = 0;
    int16_t digGain  = 0;
    uint16_t magnitude = 0;

    /*Perform NULL checks on pointers*/
    ADI_ENTRY_PTR_EXPECT(device, gainTablePtr);

    ADI_NULL_PTR_RETURN(&device->common, armDmaDataGainTablePtr);

    if (numGainIndicesInTable == 0)
    {
        ADI_ERROR_REPORT(&device->common,
            ADI_COMMON_ERRSRC_API,
            ADI_COMMON_ERR_INV_PARAM,
            ADI_COMMON_ACT_ERR_CHECK_PARAM,
            arraySize,
            "Invalid numGainIndicesInTable. Valid range is > 0");
        ADI_ERROR_RETURN(device->common.error.newAction);
    }

    /*Parse Gain Table Data in ARM DMA format and store it in gain table row struct*/
    for (gainIndex = 0; gainIndex < numGainIndicesInTable; gainIndex++)
    {
        /*Parse Gain Table Data :
        No. of Bytes per Gain Table Entry = 8
        @armDmaDataGainIndex[8 x gainIndex]    : {11'b0 , ext_control[1:0], rx_fe_gain[7:0], dig_gain[10:0]}
        @armDmaDataGainIndex[8 x gainIndex + 4]: {10'b0, tia_control[4:0], adc_control, phase_offset[15:0]}
        */
        armDmaDataGainIndex = (gainIndex * NUM_BYTES_PER_GAIN_TABLE_ENTRY);

        magnitude = ((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex] |
                    (((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & DIG_GAIN_MSB_MASK) << DIG_GAIN_MSB_SHIFT));
        if ((armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & DIG_GAIN_SIGN_MASK) == DIG_GAIN_SIGN_MASK)
        {
            /*Since digGain is a 11bit no. for ARM, 11th bit is the signed bit. However since it is of signed half word type for API use, hence has to be sign extended*/
            digGain = (int16_t)(~(uint16_t)(magnitude - 1));
            digGain |= DIG_GAIN_SIGN_EXT;
        }
        else
        {
            digGain = (int16_t)magnitude;
        }

        gainTablePtr[gainIndex].digGain = digGain;
        gainTablePtr[gainIndex].rxFeGain = (((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 1] & RXFE_GAIN_LSB_MASK) >> RXFE_GAIN_LSB_SHIFT) |
                                           (((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 2] & RXFE_GAIN_MSB_MASK) << RXFE_GAIN_MSB_SHIFT);
        gainTablePtr[gainIndex].extControl = ((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 2] & EXT_CTRL_MASK) >> EXT_CTRL_SHIFT;

        gainTablePtr[gainIndex].phaseOffset = (uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 4] |
                                              ((uint16_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 5] << PHASE_OFFSET_MSB_SHIFT);
        gainTablePtr[gainIndex].adcTiaGain = ((uint8_t)armDmaDataGainTablePtr[armDmaDataGainIndex + 6] & ADC_TIA_GAIN_MASK) >> ADC_TIA_GAIN_SHIFT;
    }

    ADI_API_RETURN(device);
}
