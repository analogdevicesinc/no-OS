#ifndef _ADI_ADRV9001_COMMON_H_
#define _ADI_ADRV9001_COMMON_H_

#include "adi_adrv9001_common_types.h"
#ifndef __KERNEL__
#include <stdint.h>
#else
#include <linux/types.h>
#endif

#ifndef __maybe_unused
#define __maybe_unused		__attribute__((__unused__))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CLIENT_IGNORE

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

#endif

#ifdef __cplusplus
}
#endif

#endif  /* _ADI_ADRV9001_COMMON_H_ */
