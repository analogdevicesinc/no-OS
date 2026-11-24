/**
 * \file
 * \brief Contains ADRV910X Rx related private function implementations
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#include "adi_adrv910x_user.h"
#include "adrv910x_rx.h"
#include "adi_adrv910x_spi.h"

#define ADRV910X_RX1_RESAMP_EN_ADDR      0x0E82u
#define ADRV910X_RX1_RESAMP_PHASE_ADDR   0x0E83u
#define ADRV910X_RXNB_RESAMP_EN_ADDR      0x128Fu
#define ADRV910X_RXNB_RESAMP_PHASE_ADDR   0x12C5u

/* PFIR Bank Select register addresses */
#define ADRV910X_PFIR_RX1_BANKSEL_ADDR    0x0383u
#define ADRV910X_PFIR_RX2_BANKSEL_ADDR    0x0384u
#define ADRV910X_PFIR_BANKSEL_MASK        0x07u

/* Stream processor spare registers for RF_ON persistence */
#define ADRV910X_RX1_SW_SPARE3_ADDR       0x11ABu
#define ADRV910X_RXNB_SW_SPARE3_ADDR      0x15ABu

#define ADRV910X_RESAMP_PHASE_ADDR_INIT(base) { (base), (uint16_t)((base) + 1u), (uint16_t)((base) + 2u), (uint16_t)((base) + 3u) }

int32_t adrv910x_RxGainTableFormat(adi_adrv910x_Device_t *device,
                                   adi_adrv910x_RxGainTableRow_t *gainTablePtr,
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


int32_t adrv910x_RxGainTableParse(adi_adrv910x_Device_t *device,
                                  adi_adrv910x_RxGainTableRow_t *gainTablePtr,
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

int32_t adrv910x_Rx_ResampPhase_Set(adi_adrv910x_Device_t *device,
                                           adi_common_ChannelNumber_e channel,
                                           int16_t resampleInputI,
                                           int16_t resampleInputQ)
{
    uint16_t enAddr;
    uint16_t phaseAddr;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    if (channel == ADI_ADRV910X_RX1)
    {
        enAddr    = ADRV910X_RX1_RESAMP_EN_ADDR;
        phaseAddr = ADRV910X_RX1_RESAMP_PHASE_ADDR;
    }
    else
    {
        enAddr    = ADRV910X_RXNB_RESAMP_EN_ADDR;
        phaseAddr = ADRV910X_RXNB_RESAMP_PHASE_ADDR;
    }

    /* Bits 7:1 are reserved read-only; safe to write full byte */
    ADI_EXPECT(adi_adrv910x_spi_Byte_Write, device, enAddr, 0x01u);

    uint16_t addrArray[4] = ADRV910X_RESAMP_PHASE_ADDR_INIT(phaseAddr);
    uint8_t dataArray[4] = {
        (uint8_t)(resampleInputI & 0xFFu),
        (uint8_t)((uint16_t)resampleInputI >> 8u),
        (uint8_t)(resampleInputQ & 0xFFu),
        (uint8_t)((uint16_t)resampleInputQ >> 8u)
    };
    ADI_EXPECT(adi_adrv910x_spi_Bytes_Write, device, addrArray, dataArray, 4u);

    ADI_API_RETURN(device);
}

int32_t adrv910x_Rx_ResampPhase_Get(adi_adrv910x_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          int16_t *resampleInputI,
                                          int16_t *resampleInputQ)
{
    uint16_t phaseAddr;

    ADI_NULL_DEVICE_PTR_RETURN(device);

    /* Both are dereferenced unconditionally below. Checked here to match the
     * convention the SPI layer already uses for its own output parameters
     * (adi_adrv910x_spi_Byte_Read's ADI_ENTRY_PTR_EXPECT, Bytes_Read's
     * ADI_NULL_PTR_RETURN) - without this a NULL from a caller is a crash
     * rather than an error return. */
    ADI_NULL_PTR_RETURN(&device->common, resampleInputI);
    ADI_NULL_PTR_RETURN(&device->common, resampleInputQ);

    if (channel == ADI_ADRV910X_RX1)
    {
        phaseAddr = ADRV910X_RX1_RESAMP_PHASE_ADDR;
    }
    else
    {
        phaseAddr = ADRV910X_RXNB_RESAMP_PHASE_ADDR;
    }

    uint8_t dataArray[4] = {0};
    uint16_t addrArray[4] = ADRV910X_RESAMP_PHASE_ADDR_INIT(phaseAddr);
    ADI_EXPECT(adi_adrv910x_spi_Bytes_Read, device, addrArray, &dataArray[0], 4u);

    *resampleInputI = (int16_t)((uint16_t)dataArray[1] << 8u | (uint16_t)dataArray[0]);
    *resampleInputQ = (int16_t)((uint16_t)dataArray[3] << 8u | (uint16_t)dataArray[2]);
    
    ADI_API_RETURN(device);
}


int32_t adrv910x_Rx_PfirBankSelect_Set(adi_adrv910x_Device_t *device,
    adi_adrv910x_MailboxChannel_e channel,
    adi_adrv910x_Rx_PfirBank_e bankSel)
{
    ADI_NULL_DEVICE_PTR_RETURN(device);

    if (channel == ADI_ADRV910X_RX1)
    {
        ADI_EXPECT(adi_adrv910x_spi_Byte_Write, device, ADRV910X_RX1_SW_SPARE3_ADDR, (uint8_t)bankSel);
        ADI_EXPECT(adi_adrv910x_spi_Byte_Write, device, ADRV910X_PFIR_RX1_BANKSEL_ADDR, (uint8_t)bankSel);
    }
    else
    {
        ADI_EXPECT(adi_adrv910x_spi_Byte_Write, device, ADRV910X_RXNB_SW_SPARE3_ADDR, (uint8_t)bankSel);
        ADI_EXPECT(adi_adrv910x_spi_Byte_Write, device, ADRV910X_PFIR_RX2_BANKSEL_ADDR, (uint8_t)bankSel);
    }

    ADI_API_RETURN(device);
}

int32_t adrv910x_Rx_PfirBankSelect_Get(adi_adrv910x_Device_t *device,
                                      adi_adrv910x_MailboxChannel_e channel,
                                      adi_adrv910x_Rx_PfirBank_e *bankSel)
{
    uint16_t bankAddr;
    uint8_t regVal = 0u;

    ADI_NULL_DEVICE_PTR_RETURN(device);
    ADI_NULL_PTR_RETURN(&device->common, bankSel);

    if (channel == ADI_ADRV910X_RX1)
    {
        bankAddr = ADRV910X_PFIR_RX1_BANKSEL_ADDR;
    }
    else
    {
        bankAddr = ADRV910X_PFIR_RX2_BANKSEL_ADDR;
    }

    ADI_EXPECT(adi_adrv910x_spi_Byte_Read, device, bankAddr, &regVal);
    *bankSel = (adi_adrv910x_Rx_PfirBank_e)(regVal & ADRV910X_PFIR_BANKSEL_MASK);

    ADI_API_RETURN(device);
}
