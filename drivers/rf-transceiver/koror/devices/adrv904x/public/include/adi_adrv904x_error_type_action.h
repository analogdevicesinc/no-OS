 /**
 * Disclaimer Legal Disclaimer
 * Copyright 2019 - 2021 Analog Devices Inc.
 * Released under the ADRV904X API license, for more information
 * see the "LICENSE.PDF" file in this zip file.
 */

 /**
 * \file adi_adrv904x_error_type_action.h
 *
 * \brief Device Recovery Action Type
 *
 * ADRV904X API Version: 2.10.0.4
 */

#ifndef _ADI_ADRV904X_ERROR_TYPE_ACTION_H_
#define _ADI_ADRV904X_ERROR_TYPE_ACTION_H_


/**
*  \brief ADI ADRV904X Recovery Actions
*/
typedef enum adi_adrv904x_ErrAction
{
	ADI_ADRV904X_ERR_ACT_RESET_DEVICE       = -500,                             /*!< Device NOK: HW/SW Reset Required */

    /* All Reset Feature Unique Codes exist between -500 & -400 */
    ADI_ADRV904X_ERR_ACT_RESET_FEATURE      = -400,                             /*!< API NOK: Feature Reset Required */
    
    /* All Reset Interface Unique Codes exist between -400 & -300 */
    ADI_ADRV904X_ERR_ACT_RESET_INTERFACE    = -300,                             /*!< API NOK: Interface Reset Required */
    
    /* All Check Feature Unique Codes exist between -300 & -200 */
    ADI_ADRV904X_ERR_ACT_CHECK_FEATURE      = -200,                             /*!< API OK; Feature is reporting an Error */

    /* All Check Interface Unique Codes exist between -200 & -100 */
    ADI_ADRV904X_ERR_ACT_CHECK_INTERFACE    = -100,                             /*!< API OK; Interface is reporting an Error */
    
    /* All Development Unique Codes exist between -100 & -1 */
    ADI_ADRV904X_ERR_ACT_OPEN_DEVICE        = -10,                              /*!< API OK; Device Not Open */
    ADI_ADRV904X_ERR_ACT_CHECK_MULTIVERSIONING = -3,                            /*!< API OK: Note: No other error data set */
    ADI_ADRV904X_ERR_ACT_CHECK_PARAM        = -1,                               /*!< API OK; Invalid Parameter passed to function */
    
    /* No Error/Recovery Action */
    ADI_ADRV904X_ERR_ACT_NONE               = 0                                 /*!< API OK; No Action Required */
} adi_adrv904x_ErrAction_e;

#endif /* _ADI_ADRV904X_ERROR_TYPE_ACTION_H_ */


