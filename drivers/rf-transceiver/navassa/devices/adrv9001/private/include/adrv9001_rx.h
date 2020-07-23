/**
 * \file
 * \brief Contains ADRV9001 Rx related private function prototypes for
 *        adrv9001_rx.c which help adi_adrv9001_rx.c
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2015 - 2018 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADRV9001_RX_H_
#define _ADRV9001_RX_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "adi_adrv9001_rx_types.h"
#include "adrv9001_bf.h"

/**
* \brief Formats gain table structure input into a format expected by ARM DMA
*
* Format Gain Table Data as follows :
* No. of Bytes per Gain Table Entry = 8
* Addr 8 x gainIndex     : {11'b0 , ext_control[1:0], rx_fe_gain[7:0], dig_gain[10:0]}
* Addr 8 x gainIndex + 4 : {10'b0, tia_control[4:0], adc_control, phase_offset[15:0]}
*
* \param device Structure pointer to the ADRV9001 device data structure
* \param gainTablePtr Input gain table
* \param formattedGainTablePtr memory to store formatted gain table data
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_RxGainTableFormat(adi_adrv9001_Device_t *device,
                                   adi_adrv9001_RxGainTableRow_t *gainTablePtr, 
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
* \param device Structure pointer to the ADRV9001 device data structure
* \param gainTablePtr pointer to gain table row entry structure where parsed data is stored
* \param armDmaDataGainTablePtr memory containing data from ARM DMA
* \param numGainIndicesInTable number of gain table entries
*
* \retval ADI_COMMON_ERR_NULL_PARAM Recovery action for NULL parameter check
* \retval ADI_COMMON_ACT_NO_ACTION Function completed successfully, no action required
*/
int32_t adrv9001_RxGainTableParse(adi_adrv9001_Device_t *device,
                                  adi_adrv9001_RxGainTableRow_t *gainTablePtr, 
                                  uint8_t *armDmaDataGainTablePtr,
                                  uint16_t numGainIndicesInTable);

#ifdef __cplusplus
}
#endif

#endif