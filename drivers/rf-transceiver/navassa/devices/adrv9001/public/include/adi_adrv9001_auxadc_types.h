/**
 * \file
 * \brief Contains functions to configure AUX ADC channels on the ADRV9001 device
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2020 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_AUXADC_TYPES_H_
#define _ADI_ADRV9001_AUXADC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
*  \brief Enum to select AuxADC
*/
typedef enum adi_adrv9001_AuxAdc
{
    ADI_ADRV9001_AUXADC0, /*!< AuxADC0 */
    ADI_ADRV9001_AUXADC1, /*!< AuxADC1 */
    ADI_ADRV9001_AUXADC2, /*!< AuxADC2 */
    ADI_ADRV9001_AUXADC3, /*!< AuxADC3 */
} adi_adrv9001_AuxAdc_e;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_AUXADC_TYPES_H_ */
