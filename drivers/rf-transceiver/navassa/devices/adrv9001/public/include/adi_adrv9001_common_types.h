#ifndef _ADI_ADRV9001_COMMON_TYPES_H_
#define _ADI_ADRV9001_COMMON_TYPES_H_

/**
 * \brief Enumeration of channel numbers
 *
 * \note These enums may be masked (OR'd) to specify multiple channels at once
 */
typedef enum adi_common_ChannelNumber
{
    ADI_CHANNEL_1 = 0x1, /* In hex to indicate maskable value */
    ADI_CHANNEL_2 = 0x2
} adi_common_ChannelNumber_e;

/**
 * \brief Enumeration of port types
 *
 * \note These enums may NOT be masked (OR'd) to specify multiple ports at once
 */
typedef enum adi_common_Port
{
    ADI_RX = 0, /* In decimal to indicate NON-maskable value */
    ADI_TX = 1,
    ADI_ORX = 2,
    ADI_ILB = 3,
    ADI_ELB = 4
} adi_common_Port_e;

#endif