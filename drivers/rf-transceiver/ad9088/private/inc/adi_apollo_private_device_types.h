/*!
 * \brief     API private device types header file
 *
 * \copyright copyright(c) 2024 analog devices, inc. all rights reserved.
 *            This software is proprietary to Analog Devices, Inc. and its
 *            licensor. By using this software you agree to the terms of the
 *            associated analog devices software license agreement.
 */

/*!
 * \addtogroup ADI_APOLLO_PRIVATE_DEVICE
 * @{
 */
#ifndef __ADI_APOLLO_PRIVATE_DEVICE_TYPES_H__
#define __ADI_APOLLO_PRIVATE_DEVICE_TYPES_H__

/*============= I N C L U D E S ============*/

/**
* \brief Device feature lockouts
*/
typedef enum
{
    /* Rx and Tx terminals */
    ADI_APOLLO_EC_CONV_1X1X_LOCK             = 0,    /*! bit=1 lock out all Rx/Tx 1x-1x (raw data) modes. */
    ADI_APOLLO_EC_CONV_QC_LOCK               = 1,    /*! bit=1 lock out manual ADC JESD programming.   */
    ADI_APOLLO_EC_CONV_LOCK                  = 2,    /*! bit=1 force shut down all ADC channels.       */
    ADI_APOLLO_EC_CONV_FFH_LOCK              = 3,    /*! bit=1 lock out ADC FFH.  */
    ADI_APOLLO_EC_NLC_LOCK                   = 4,    /*! bit=1 lock out Rx/Tx NLC.   */
    ADI_APOLLO_EC_PFILT_LOCK                 = 5,    /*! bit=1 lock out Rx/Tx PFILT. */
    ADI_APOLLO_EC_CFIR_LOCK                  = 6,    /*! bit=1 lock out Rx/Tx CFIR.  */
    ADI_APOLLO_EC_FSRC_LOCK                  = 7,    /*! bit=1 lock out Rx/Tx FSRC.  */
    ADI_APOLLO_EC_DYN_CFG_LOCK               = 8,    /*! bit=1 lock out Rx/Tx dynamic config. */
    ADI_APOLLO_EC_USR_LOCK                   = 9,    /*! bit=1 lock out Rx/Tx USR.   */
    ADI_APOLLO_EC_VSR_LOCK                   = 10,   /*! bit=1 lock out Rx/Tx VSR.   */
    ADI_APOLLO_EC_CDRC_LT8X_LOCK             = 11,   /*! bit=1 lock out all CDDC/CDUC ratios < 8x (1x, 2x, 3x, 4x, 6x). */

    /* Rx terminal */
    ADI_APOLLO_EC_FFT_SPECTURMSNIFFER_LOCK  = 12,   /*! bit=1 lock out fft spectrum sniffer. */

    /* Tx terminal */
    ADI_APOLLO_EC_LPBK0_LOCK                = 12,   /*! bit=1 lock out lpbk mode 0. */
    ADI_APOLLO_EC_LPBK123_LOCK              = 13,   /*! bit=1 lock out lpbk mode 1, 2, and 3. */
} adi_apollo_private_device_feat_lockout_e;

#endif /* __ADI_APOLLO_PRIVATE_DEVICE_TYPES_H__ */
/*! @} */
