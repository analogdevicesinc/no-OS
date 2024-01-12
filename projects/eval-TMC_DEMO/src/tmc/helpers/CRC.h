/*******************************************************************************
* Copyright © 2017 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC_HELPERS_CRC_H_
#define TMC_HELPERS_CRC_H_

	#include "Types.h"

	// Amount of CRC tables available
	// Each table takes ~260 bytes (257 bytes, one bool and structure padding)
	#define CRC_TABLE_COUNT 2

	uint8_t tmc_fillCRC8Table(uint8_t polynomial, bool isReflected, uint8_t index);
	uint8_t tmc_CRC8(uint8_t *data, uint32_t bytes, uint8_t index);

	uint8_t tmc_tableGetPolynomial(uint8_t index);
	bool  tmc_tableIsReflected(uint8_t index);

#endif /* TMC_HELPERS_CRC_H_ */
