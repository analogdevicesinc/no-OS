/**
 * \file
 * \brief Contains ADRV910X Rx related private function prototypes for
 *        adrv910X_rx.c which help adi_adrv910x_rx.c
 *
 * ADRV910X API Version: $ADI_ADRV910X_API_VERSION$
 */

 /**
 * Copyright 2022 Analog Devices Inc.
 * Released under the ADRV910X API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV910X_RX_H_
#define _ADRV910X_RX_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "adi_adrv910x_rx_types.h"
#include "adrv910x_bf.h"

/**
* \brief Formats gain table structure input into a format expected by ARM DMA
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* Addr 8 x gainIndex     : {11'b0 , ext_control[1:0], rx_fe_gain[7:0], dig_gain[10:0]}
* Addr 8 x gainIndex + 4 : {10'b0, tia_control[4:0], adc_control, phase_offset[15:0]}
*
* \param device Structure pointer to the ADRV910X device data structure
* \param gainTablePtr Input gain table
* \param formattedGainTablePtr memory to store formatted gain table data
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_RxGainTableFormat(adi_adrv910x_Device_t *device,
                                   adi_adrv910x_RxGainTableRow_t *gainTablePtr, 
                                   uint8_t *formattedGainTablePtr,
                                   uint16_t numGainIndicesInTable);

/**
* \brief Parses gain table in ARM DMA data format into a gain table row structure
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* armDmaDataGainIndex[8 x gainIndex]     : {11'b0 , ext_control[1:0], rx_fe_gain[7:0], dig_gain[10:0]}
* armDmaDataGainIndex[8 x gainIndex + 4] : {10'b0, tia_control[4:0], adc_control, phase_offset[15:0]}
*
* \param device Structure pointer to the ADRV910X device data structure
* \param gainTablePtr pointer to gain table row entry structure where parsed data is stored
* \param armDmaDataGainTablePtr memory containing data from ARM DMA
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_RxGainTableParse(adi_adrv910x_Device_t *device,
                                  adi_adrv910x_RxGainTableRow_t *gainTablePtr, 
                                  uint8_t *armDmaDataGainTablePtr,
                                  uint16_t numGainIndicesInTable);

/**
* \brief Writes resampler phase registers directly via SPI (bypasses bitfield layer)
*
* \param device Structure pointer to the ADRV910X device data structure
* \param channel RX channel (ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB)
* \param resampleInputI I-channel resampler phase value
* \param resampleInputQ Q-channel resampler phase value
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_Rx_ResampPhase_Set(adi_adrv910x_Device_t *device,
                                           adi_common_ChannelNumber_e channel,
                                           int16_t resampleInputI,
                                           int16_t resampleInputQ);

/**
* \brief Reads resampler phase registers directly via SPI (bypasses bitfield layer)
*
* \param device Structure pointer to the ADRV910X device data structure
* \param channel RX channel (ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB)
* \param resampleInputI Pointer to store I-channel resampler phase value
* \param resampleInputQ Pointer to store Q-channel resampler phase value
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_Rx_ResampPhase_Get(adi_adrv910x_Device_t *device,
                                          adi_common_ChannelNumber_e channel,
                                          int16_t *resampleInputI,
                                          int16_t *resampleInputQ);

/**
* \brief Directly writes the PFIR bank select register via SPI (no wrapper overhead)
*
* \param device Pointer to the device structure
* \param channel ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB
* \param bankSel PFIR bank to activate (ADI_ADRV910X_RX_PFIR_BANK_A through ADI_ADRV910X_RX_PFIR_BANK_F)
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_Rx_PfirBankSelect_Set(adi_adrv910x_Device_t *device,
                                      adi_adrv910x_MailboxChannel_e channel,
                                      adi_adrv910x_Rx_PfirBank_e bankSel);

/**
* \brief Directly reads the PFIR bank select register via SPI (no wrapper overhead)
*
* \param device Pointer to the device structure
* \param channel ADI_ADRV910X_RX1 or ADI_ADRV910X_RXNB
* \param bankSel Pointer to store the currently active PFIR bank
*
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv910x_Rx_PfirBankSelect_Get(adi_adrv910x_Device_t *device,
                                      adi_adrv910x_MailboxChannel_e channel,
                                      adi_adrv910x_Rx_PfirBank_e *bankSel);

#ifdef __cplusplus
}
#endif

#endif