/**
* \file
* \brief Contains ADRV9001 API ORx datapath data types
*
* ADRV9001 API Version: $ADI_ADRV9001_API_VERSION$
*/

/**
* Copyright 2015 - 2019 Analog Devices Inc.
* Released under the ADRV9001 API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_ADRV9001_ORX_TYPES_H_
#define _ADI_ADRV9001_ORX_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
/* ADI specific header files */
//#include "adi_adrv9001_radioctrl_types.h"


/* Header files related to libraries */


/* System header files */
    
/*
*********************************************************************************************************
*                                             LOCAL DEFINES
*********************************************************************************************************
*/

#define ADI_ADRV9001_ORX_GAIN_INDEX_MIN 2
#define ADI_ADRV9001_ORX_GAIN_INDEX_MAX 14

/*
*********************************************************************************************************
*                                             ENUMs
*********************************************************************************************************
*/

/**
 * \brief Enumeration for ORX BBDC rejection algorithm enable status
 */
typedef enum adi_adrv9001_OrxBbdcRejectionStatus
{
    ADI_ADRV9001_ORX_BBDC_REJECTION_DISABLED = 0,  /*!< ORX baseband DC rejection algorithm disabled */
    ADI_ADRV9001_ORX_BBDC_REJECTION_ENABLED  = 1,  /*!< ORX baseband DC rejection algorithm enabled */
    ADI_ADRV9001_ORX_BBDC_REJECTION_PAUSED   = 2   /*!< ORX baseband DC rejection algorithm paused (but enabled) */
} adi_adrv9001_OrxBbdcRejectionStatus_e;

#ifdef __cplusplus
}
#endif

#endif /* _ADI_ADRV9001_ORX_TYPES_H_ */
