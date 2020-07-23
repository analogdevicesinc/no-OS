/**
 * \file
 * \brief Global definitions for ADRV9001 API
 * \copyright Analog Devices Inc. 2019. All rights reserved.
 * Released under the ADRV9001 API license, for more information see "LICENSE.txt" in the SDK
 */

#ifndef _ADI_ADRV9001_DEFINES_H_
#define _ADI_ADRV9001_DEFINES_H_

#define ADI_ADRV9001_MAX_NUM_CHANNELS         4

#define ADI_ADRV9001_MAX_TXCHANNELS           2
#define ADI_ADRV9001_MAX_RXCHANNELS           8
#define ADI_ADRV9001_MAX_RX_ONLY              2
#define ADI_ADRV9001_MAX_ORX_ONLY             2
#define ADI_ADRV9001_MAX_ILB_ONLY             2
#define ADI_ADRV9001_MAX_ELB_ONLY             2

#define ADI_ADRV9001_MAX_TX_CHANNEL_START     0
#define ADI_ADRV9001_MAX_TX_CHANNEL_END       1
#define ADI_ADRV9001_MAX_RX_CHANNEL_START     0
#define ADI_ADRV9001_MAX_RX_CHANNEL_END       1
#define ADI_ADRV9001_MAX_ORX_CHANNEL_START    2
#define ADI_ADRV9001_MAX_ORX_CHANNEL_END      3
#define ADI_ADRV9001_MAX_LB_CHANNEL_START     4
#define ADI_ADRV9001_MAX_LB_CHANNEL_END       7

#define ADI_ADRV9001_MAX_RX_ORX_CHANNELS	  ( ADI_ADRV9001_MAX_RX_ONLY + ADI_ADRV9001_MAX_ORX_ONLY )

#endif /* _ADI_ADRV9001_DEFINES_H_ */
