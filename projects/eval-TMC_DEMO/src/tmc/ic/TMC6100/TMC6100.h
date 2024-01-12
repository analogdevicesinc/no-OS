/*******************************************************************************
* Copyright © 2019 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC_IC_TMC6100_H_
#define TMC_IC_TMC6100_H_

#include "tmc/helpers/API_Header.h"
#include "TMC6100_Constants.h"
#include "TMC6100_Register.h"
#include "TMC6100_Fields.h"

int32_t tmc6100_readInt(uint8_t motor, uint8_t address);
void tmc6100_writeInt(uint8_t motor, uint8_t address, int32_t value);

#endif /* TMC_IC_TMC6630_H_ */
