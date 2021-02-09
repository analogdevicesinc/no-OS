/**
* \file
* \brief Contains ADI common interface.
*
* ADI common lib Version: $ADI_COMMON_LIB_VERSION$
*/

/**
* Copyright 2015 - 2018 Analog Devices Inc.
* Released under the API license, for more information
* see the "LICENSE.txt" file in this zip file.
*/

#ifndef _ADI_COMMON_H_
#define _ADI_COMMON_H_

#include "adi_common_error.h"
#include "adi_common_hal.h"
#include "adi_common_log.h"
#include "adi_common_types.h"

#ifndef __maybe_unused
#define __maybe_unused		__attribute__((__unused__))
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Convert an adi_common_Port_e into a numeric index
 *
 * \param[in]  port     The port to convert
 * \param[out] index    The corresponding numeric index
 *
 * \retval 0 success
 * \retval -2 invalid parameter
 */
int32_t adi_common_port_to_index(adi_common_Port_e port, uint8_t *index);

/**
 * \brief Convert an adi_common_ChannelNumber_e into a numeric index
 *
 * \param[in]  channel  The channel number to convert
 * \param[out] index    The corresponding numeric index
 *
 * \retval 0 success
 * \retval -2 invalid parameter
 */
int32_t adi_common_channel_to_index(adi_common_ChannelNumber_e channel, uint8_t *index);

/**
 * \brief Convert a numeric index to an adi_common_Port_e
 *
 * \param[in]  index    The numeric index to convert
 * \param[out] port     The adi_common_Port_e corresponding to the numeric index
 *
 * \retval 0 success
 * \retval -2 invalid parameter
 */
int32_t adi_common_index_to_port(uint8_t index, adi_common_Port_e *port);

/**
 * \brief Convert a numeric index to an adi_common_ChannelNumber_e
 *
 * \param[in]  index    The numeric index to convert
 * \param[out] channel  The adi_common_ChannelNumber_e corresponding to the numeric index
 *
 * \retval 0 success
 * \retval -2 invalid parameter
 */
int32_t adi_common_index_to_channel(uint8_t index, adi_common_ChannelNumber_e *channel);

#ifdef __cplusplus
}
#endif

#endif  /* _ADI_COMMON_H_ */
