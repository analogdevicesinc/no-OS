/**
 * Copyright 2020 - 2025 Analog Devices Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

/**
* \file adi_AD9528_user.h
* \brief Contains top level AD9528 related function prototypes for
*        adi_ad9528.c
*
*/


#ifndef _ADI_AD9528_USER_H_
#define _ADI_AD9528_USER_H_

/*
 *****************************************
 * AD9528 Interface Macros
 ******************************************
 */

#ifndef ADI_AD9528_VERBOSE
#define ADI_AD9528_VERBOSE 1                            /* Use strings 0 not use, 1 use */
#endif /* !ADI_AD9528_VERBOSE */

#ifndef ADI_AD9528_VARIABLE_USAGE
#define ADI_AD9528_VARIABLE_USAGE 1                     /* Use strings 0 not use, 1 use */
#endif /* !ADI_AD9528_VARIABLE_USAGE */

#ifndef ADI_AD9528_LOGGING
#define ADI_AD9528_LOGGING (uint32_t) (ADI_HAL_LOG_ALL) /* LogLevel Set to All */
#endif /* !ADI_AD9528_LOGGING */

#ifndef ADI_AD9528_PLL_DEBOUNCE_TIME_MS
#define ADI_AD9528_PLL_DEBOUNCE_TIME_MS 50U             /* Debounce Time */
#endif /* !ADI_AD9528_PLL_DEBOUNCE_TIME_MS */

#ifndef ADI_AD9528_PLL_DEBOUNCE_INT_MS
#define ADI_AD9528_PLL_DEBOUNCE_INT_MS 10U              /* Debounce Interval */
#endif /* !ADI_AD9528_PLL_DEBOUNCE_INT_MS */

#endif /* _ADI_AD9528_USER_H_ */
