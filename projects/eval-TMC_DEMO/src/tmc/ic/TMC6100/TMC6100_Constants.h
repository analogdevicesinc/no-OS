/*******************************************************************************
* Copyright © 2018 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC_IC_TMC6100_TMC6100_CONSTANTS_H_
#define TMC_IC_TMC6100_TMC6100_CONSTANTS_H_

#include "tmc/helpers/Constants.h"

#define TMC6100_MOTORS           1
#define TMC6100_WRITE_BIT        TMC_WRITE_BIT
#define TMC6100_ADDRESS_MASK     TMC_ADDRESS_MASK
#define TMC6100_MAX_VELOCITY     8388096
#define TMC6100_MAX_ACCELERATION u16_MAX

#endif /* TMC_IC_TMC6100_TMC6100_CONSTANTS_H_ */
