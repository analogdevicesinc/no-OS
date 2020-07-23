/**
 * \file
 * \brief Contains functions to configure AUX DAC channels on the ADRV9001 device
 *
 * ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
 */

 /**
 * Copyright 2019 Analog Devices Inc.
 * Released under the ADRV9001 API license, for more information
 * see the "LICENSE.txt" file in this zip file.
 */

#ifndef _ADI_ADRV9001_AUXDAC_TYPES_H_
#define _ADI_ADRV9001_AUXDAC_TYPES_H_

/**
*  \brief Enum to select AuxDAC
*/
typedef enum adi_adrv9001_AuxDac
{
    ADI_ADRV9001_AUXDAC0, /*!< AuxDAC0 */
    ADI_ADRV9001_AUXDAC1, /*!< AuxDAC1 */
    ADI_ADRV9001_AUXDAC2, /*!< AuxDAC2 */
    ADI_ADRV9001_AUXDAC3, /*!< AuxDAC3 */
} adi_adrv9001_AuxDac_e;

#endif /* _ADI_ADRV9001_AUXDAC_TYPES_H_ */
