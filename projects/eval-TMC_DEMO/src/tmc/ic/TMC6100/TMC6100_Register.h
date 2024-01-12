/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC6100_REGISTER_H
#define TMC6100_REGISTER_H

	// ===== TMC6100 register set =====

	#define TMC6100_GCONF          0x00
	#define TMC6100_GSTAT          0x01
	#define TMC6100_IOIN_OUTPUT    0x04
	#define TMC6100_OTP_PROG       0x06
	#define TMC6100_OTP_READ       0x07
	#define TMC6100_FACTORY_CONF   0x08
	#define TMC6100_SHORT_CONF     0x09
	#define TMC6100_DRV_CONF       0x0A

#endif /* TMC6100_REGISTER_H */
