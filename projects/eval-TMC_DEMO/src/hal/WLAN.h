/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef __WLAN_H_
#define __WLAN_H_

#include "RXTX.h"

extern RXTXTypeDef WLAN;

typedef enum
{
	BUFFER_CLEAR,
	BUFFER_WRITE,
	BUFFER_EXECUTE
} BufferCommandTypedef;

typedef enum
{
	WLAN_INIT_CMD_MODE,  // wait time inbetween sending $$$ and entering command mode - writing is disabled completely. rx doesn't read out data - we enter this mode only after a clearBuffer() anyways
	WLAN_CMD_MODE,       // Command mode - writing is disabled for the HAL tx function, rawTx still writes. rx doesn't read out data, rawRx does
	WLAN_DATA_MODE       // Data mode - HAL tx/rx functions works normally
} WLANStateTypedef;

void UART0_RX_TX_IRQHandler_WLAN(void);
uint32_t checkReadyToSend();
void enableWLANCommandMode();
uint32_t checkCmdModeEnabled();
uint32_t handleWLANCommand(BufferCommandTypedef cmd, uint32_t value);
uint32_t getCMDReply();

#endif /* __WLAN_H_ */
