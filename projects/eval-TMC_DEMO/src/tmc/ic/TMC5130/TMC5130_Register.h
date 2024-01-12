/*******************************************************************************
* Copyright © 2016 TRINAMIC Motion Control GmbH & Co. KG
* (now owned by Analog Devices Inc.),
*
* Copyright © 2023 Analog Devices Inc. All Rights Reserved. This software is
* proprietary & confidential to Analog Devices, Inc. and its licensors.
*******************************************************************************/


#ifndef TMC5130_REGISTER_H
#define TMC5130_REGISTER_H

// ===== TMC5130 register set =====

#define TMC5130_GCONF       0x00
#define TMC5130_GSTAT       0x01
#define TMC5130_IFCNT       0x02
#define TMC5130_SLAVECONF   0x03
#define TMC5130_IOIN        0x04
#define TMC5130_X_COMPARE   0x05

#define TMC5130_IHOLD_IRUN  0x10
#define TMC5130_TPOWERDOWN  0x11
#define TMC5130_TSTEP       0x12
#define TMC5130_TPWMTHRS    0x13
#define TMC5130_TCOOLTHRS   0x14
#define TMC5130_THIGH       0x15

#define TMC5130_RAMPMODE    0x20
#define TMC5130_XACTUAL     0x21
#define TMC5130_VACTUAL     0x22
#define TMC5130_VSTART      0x23
#define TMC5130_A1          0x24
#define TMC5130_V1          0x25
#define TMC5130_AMAX        0x26
#define TMC5130_VMAX        0x27
#define TMC5130_DMAX        0x28
#define TMC5130_D1          0x2A
#define TMC5130_VSTOP       0x2B
#define TMC5130_TZEROWAIT   0x2C
#define TMC5130_XTARGET     0x2D

#define TMC5130_VDCMIN      0x33
#define TMC5130_SWMODE      0x34
#define TMC5130_RAMPSTAT    0x35
#define TMC5130_XLATCH      0x36

#define TMC5130_ENCMODE     0x38
#define TMC5130_XENC        0x39
#define TMC5130_ENC_CONST   0x3A
#define TMC5130_ENC_STATUS  0x3B
#define TMC5130_ENC_LATCH   0x3C

#define TMC5130_MSLUT0      0x60
#define TMC5130_MSLUT1      0x61
#define TMC5130_MSLUT2      0x62
#define TMC5130_MSLUT3      0x63
#define TMC5130_MSLUT4      0x64
#define TMC5130_MSLUT5      0x65
#define TMC5130_MSLUT6      0x66
#define TMC5130_MSLUT7      0x67
#define TMC5130_MSLUTSEL    0x68
#define TMC5130_MSLUTSTART  0x69
#define TMC5130_MSCNT       0x6A
#define TMC5130_MSCURACT    0x6B

#define TMC5130_CHOPCONF    0x6C
#define TMC5130_COOLCONF    0x6D
#define TMC5130_DCCTRL      0x6E
#define TMC5130_DRVSTATUS   0x6F
#define TMC5130_PWMCONF     0x70
#define TMC5130_PWMSTATUS   0x71
#define TMC5130_ENCM_CTRL   0x72
#define TMC5130_LOST_STEPS  0x73

#endif /* TMC5130_REGISTER_H */
