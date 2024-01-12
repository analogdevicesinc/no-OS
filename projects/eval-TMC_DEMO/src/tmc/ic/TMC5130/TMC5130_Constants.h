/*******************************************************************************
* Copyright © 2018 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC_IC_TMC5130_TMC5130_CONSTANTS_H_
#define TMC_IC_TMC5130_TMC5130_CONSTANTS_H_

#include "tmc/helpers/Constants.h"

#define TMC5130_REGISTER_COUNT   TMC_REGISTER_COUNT
#define TMC5130_MOTORS           1
#define TMC5130_WRITE_BIT        TMC_WRITE_BIT
#define TMC5130_ADDRESS_MASK     TMC_ADDRESS_MASK
#define TMC5130_MAX_VELOCITY     8388096
#define TMC5130_MAX_ACCELERATION u16_MAX

// ramp modes (Register TMC5161_RAMPMODE)
#define TMC5130_MODE_POSITION  0
#define TMC5130_MODE_VELPOS    1
#define TMC5130_MODE_VELNEG    2
#define TMC5130_MODE_HOLD      3

// limit switch mode bits (Register TMC5130_SWMODE)
#define TMC5130_SW_STOPL_ENABLE    0x0001
#define TMC5130_SW_STOPR_ENABLE    0x0002
#define TMC5130_SW_STOPL_POLARITY  0x0004
#define TMC5130_SW_STOPR_POLARITY  0x0008
#define TMC5130_SW_SWAP_LR         0x0010
#define TMC5130_SW_LATCH_L_ACT     0x0020
#define TMC5130_SW_LATCH_L_INACT   0x0040
#define TMC5130_SW_LATCH_R_ACT     0x0080
#define TMC5130_SW_LATCH_R_INACT   0x0100
#define TMC5130_SW_LATCH_ENC       0x0200
#define TMC5130_SW_SG_STOP         0x0400
#define TMC5130_SW_SOFTSTOP        0x0800

// Status bits (Register TMC5130_RAMPSTAT)
#define TMC5130_RS_STOPL          0x0001
#define TMC5130_RS_STOPR          0x0002
#define TMC5130_RS_LATCHL         0x0004
#define TMC5130_RS_LATCHR         0x0008
#define TMC5130_RS_EV_STOPL       0x0010
#define TMC5130_RS_EV_STOPR       0x0020
#define TMC5130_RS_EV_STOP_SG     0x0040
#define TMC5130_RS_EV_POSREACHED  0x0080
#define TMC5130_RS_VELREACHED     0x0100
#define TMC5130_RS_POSREACHED     0x0200
#define TMC5130_RS_VZERO          0x0400
#define TMC5130_RS_ZEROWAIT       0x0800
#define TMC5130_RS_SECONDMOVE     0x1000
#define TMC5130_RS_SG             0x2000

// Encoderbits (Register TMC5130_ENCMODE)
#define TMC5130_EM_DECIMAL     0x0400
#define TMC5130_EM_LATCH_XACT  0x0200
#define TMC5130_EM_CLR_XENC    0x0100
#define TMC5130_EM_NEG_EDGE    0x0080
#define TMC5130_EM_POS_EDGE    0x0040
#define TMC5130_EM_CLR_ONCE    0x0020
#define TMC5130_EM_CLR_CONT    0x0010
#define TMC5130_EM_IGNORE_AB   0x0008
#define TMC5130_EM_POL_N       0x0004
#define TMC5130_EM_POL_B       0x0002
#define TMC5130_EM_POL_A       0x0001

#endif /* TMC_IC_TMC5130_TMC5130_CONSTANTS_H_ */
