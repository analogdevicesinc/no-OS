/***************************************************************************//**
 *   @file   ade7880.h
 *   @brief  Header file of ADE7880 Driver.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL ANALOG DEVICES, INC. BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/
#ifndef __ADE7880_H__
#define __ADE7880_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

/* SPI commands */
#define ADE7880_SPI_READ_CMD        	0x01
#define ADE7880_SPI_WRITE_CMD       	0x00

#define ENABLE                  	0x0001
#define DISABLE                 	0x0000

/* ADE7880 Register Map */
#define ADE7880_REG_AIGAIN		0x4380
#define ADE7880_REG_AVGAIN		0x4381
#define ADE7880_REG_BIGAIN		0x4382
#define ADE7880_REG_BVGAIN		0x4383
#define ADE7880_REG_CIGAIN		0x4384
#define ADE7880_REG_CVGAIN		0x4385
#define ADE7880_REG_NIGAIN		0x4386
#define ADE7880_REG_DICOEFF		0x4388
#define ADE7880_REG_APGAIN		0x4389
#define ADE7880_REG_AWATTOS		0x438A
#define ADE7880_REG_BPGAIN		0x438B
#define ADE7880_REG_BWATTOS		0x438C
#define ADE7880_REG_CPGAIN		0x438D
#define ADE7880_REG_CWATTOS		0x438E
#define ADE7880_REG_AIRMSOS 		0x438F
#define ADE7880_REG_AVRMSOS 		0x4390
#define ADE7880_REG_BIRMSOS		0x4391
#define ADE7880_REG_BVRMSOS		0x4392
#define ADE7880_REG_CIRMSOS		0x4393
#define ADE7880_REG_CVRMSOS 		0x4394
#define ADE7880_REG_NIRMSOS 		0x4395
#define ADE7880_REG_HPGAIN		0x4398
#define ADE7880_REG_ISUMLVL		0x4399
#define ADE7880_REG_VLEVEL		0x439F
#define ADE7880_REG_AFWATTOS		0x43A2
#define ADE7880_REG_BFWATTOS		0x43A3
#define ADE7880_REG_CFWATTOS		0x43A4
#define ADE7880_REG_AFVAROS		0x43A5
#define ADE7880_REG_BFVAROS     	0x43A6
#define ADE7880_REG_CFVAROS		0x43A7
#define ADE7880_REG_AFIRMSOS		0x43A8
#define ADE7880_REG_BFIRMSOS		0x43A9
#define ADE7880_REG_CFIRMSOS		0x43AA
#define ADE7880_REG_AFVRMSOS		0x43AB
#define ADE7880_REG_BFVRMSOS		0x43AC
#define ADE7880_REG_CFVRMSOS		0x43AD
#define ADE7880_REG_HXWATTOS		0x43AE
#define ADE7880_REG_HYWATTOS		0x43AF
#define ADE7880_REG_HZWATTOS		0x43B0
#define ADE7880_REG_HXVAROS 		0x43B1
#define ADE7880_REG_HYVAROS		0x43B2
#define ADE7880_REG_HZVAROS		0x43B3
#define ADE7880_REG_HXIRMSOS		0x43B4
#define ADE7880_REG_HYIRMSOS		0x43B5
#define ADE7880_REG_HZIRMSOS		0x43B6
#define ADE7880_REG_HXVRMSOS		0x43B7
#define ADE7880_REG_HYVRMSOS		0x43B8
#define ADE7880_REG_HZVRMSOS		0x43B9
#define ADE7880_REG_AIRMS		0x43C0
#define ADE7880_REG_AVRMS		0x43C1
#define ADE7880_REG_BIRMS		0x43C2
#define ADE7880_REG_BVRMS		0x43C3
#define ADE7880_REG_CIRMS		0x43C4
#define ADE7880_REG_CVRMS		0x43C5
#define ADE7880_REG_NIRMS		0x43C6
#define ADE7880_REG_ISUM		0x43C7

/* Internal DSP memory RAM registers*/
#define ADE7880_REG_RUN			0xE228

/* Billable registers*/
#define ADE7880_REG_AWATTHR		0xE400
#define ADE7880_REG_BWATTHR		0xE401
#define ADE7880_REG_CWATTHR		0xE402
#define ADE7880_REG_AFWATTHR		0xE403
#define ADE7880_REG_BFWATTHR		0xE404
#define ADE7880_REG_CFWATTHR		0xE405
#define ADE7880_REG_AFVARHR		0xE409
#define ADE7880_REG_BFVARHR		0xE40A
#define ADE7880_REG_CFVARHR		0xE40B
#define ADE7880_REG_AVAHR		0xE40C
#define ADE7880_REG_BVAHR		0xE40D
#define ADE7880_REG_CVAHR		0xE40E

/* Configuration and PQ registers */
#define ADE7880_REG_IPEAK		0xE500
#define ADE7880_REG_VPEAK		0xE501
#define ADE7880_REG_STATUS0		0xE502
#define ADE7880_REG_STATUS1		0xE503
#define ADE7880_REG_AIMAV	 	0xE504
#define ADE7880_REG_BIMAV 		0xE505
#define ADE7880_REG_CIMAV	 	0xE506
#define ADE7880_REG_OILVL	 	0xE507
#define ADE7880_REG_OVLVL	 	0xE508
#define ADE7880_REG_SAGLVL	 	0xE509
#define ADE7880_REG_MASK0	 	0xE50A
#define ADE7880_REG_MASK1	 	0xE50B
#define ADE7880_REG_IAWV	 	0xE50C
#define ADE7880_REG_IBWV	 	0xE50D
#define ADE7880_REG_ICWV	 	0xE50E
#define ADE7880_REG_INWV	 	0xE50F
#define ADE7880_REG_VAWV		0xE510
#define ADE7880_REG_VBWV		0xE511
#define ADE7880_REG_VCWV		0xE512
#define ADE7880_REG_AWATT		0xE513
#define ADE7880_REG_BWATT		0xE514
#define ADE7880_REG_CWATT		0xE515
#define ADE7880_REG_AVA			0xE519
#define ADE7880_REG_BVA			0xE51A
#define ADE7880_REG_CVA			0xE51B
#define ADE7880_REG_CHECKSUM		0xE51F
#define ADE7880_REG_VNOM		0xE520
#define ADE7880_REG_LAST_RWDATA32	0xE5FF
#define ADE7880_REG_PHSTATUS		0xE600
#define ADE7880_REG_ANGLE0		0xE601
#define ADE7880_REG_ANGLE1		0xE602
#define ADE7880_REG_ANGLE2		0xE603
#define ADE7880_REG_PHNOLOAD		0xE608
#define ADE7880_REG_LINECYC		0xE60C
#define ADE7880_REG_ZXTOUT		0xE60D
#define ADE7880_REG_COMPMODE		0xE60E
#define ADE7880_REG_GAIN		0xE60F
#define ADE7880_REG_CFMODE		0xE610
#define ADE7880_REG_CF1DEN		0xE611
#define ADE7880_REG_CF2DEN		0xE612
#define ADE7880_REG_CF3DEN		0xE613
#define ADE7880_REG_APHCAL		0xE614
#define ADE7880_REG_BPHCAL		0xE615
#define ADE7880_REG_CPHCAL		0xE616
#define ADE7880_REG_PHSIGN 		0xE617
#define ADE7880_REG_CONFIG		0xE618
#define ADE7880_REG_MMODE		0xE700
#define ADE7880_REG_ACCMODE		0xE701
#define ADE7880_REG_LCYCMODE		0xE702
#define ADE7880_REG_PEAKCYC		0xE703
#define ADE7880_REG_SAGCYC		0xE704
#define ADE7880_REG_CFCYC		0xE705
#define ADE7880_REG_HSDC_CFG		0xE706
#define ADE7880_REG_VERSION		0xE707
#define ADE7880_REG_RESERVED		0xE7E4
#define ADE7880_REG_LAST_RWDATA8	0xE7FD
#define ADE7880_REG_FVRMS		0xE880
#define ADE7880_REG_FIRMS		0xE881
#define ADE7880_REG_FWATT		0xE882
#define ADE7880_REG_FVAR		0xE883
#define ADE7880_REG_FVA 		0xE884
#define ADE7880_REG_FPF			0xE885
#define ADE7880_REG_VTHD		0xE886
#define ADE7880_REG_ITHD		0xE887
#define ADE7880_REG_HXVRMS		0xE888
#define ADE7880_REG_HXIRMS		0xE889
#define ADE7880_REG_HXWATT		0xE88A
#define ADE7880_REG_HXVAR		0xE88B
#define ADE7880_REG_HXVA		0xE88C
#define ADE7880_REG_HXPF		0xE88D
#define ADE7880_REG_HXVHD		0xE88E
#define ADE7880_REG_HXIHD		0xE88F
#define ADE7880_REG_HYVRMS		0xE890
#define ADE7880_REG_HYIRMS		0xE891
#define ADE7880_REG_HYWATT		0xE892
#define ADE7880_REG_HFVAR		0xE893
#define ADE7880_REG_HYVA		0xE894
#define ADE7880_REG_HYPF		0xE895
#define ADE7880_REG_HYVHD		0xE896
#define ADE7880_REG_HYIHD		0xE897
#define ADE7880_REG_HZVRMS		0xE898
#define ADE7880_REG_HZIRMS		0xE899
#define ADE7880_REG_HZWATT		0xE89A
#define ADE7880_REG_HZVAR		0xE89B
#define ADE7880_REG_HZVA		0xE89C
#define ADE7880_REG_HZPF		0xE89D
#define ADE7880_REG_HZVHD		0xE89E
#define ADE7880_REG_HZIHD		0xE89F
#define ADE7880_REG_HCONFIG		0xE900
#define ADE7880_REG_APF			0xE902
#define ADE7880_REG_BPF			0xE903
#define ADE7880_REG_CPF			0xE904
#define ADE7880_REG_APERIOD		0xE905
#define ADE7880_REG_BPERIOD		0xE906
#define ADE7880_REG_CPERIOD		0xE907
#define ADE7880_REG_APNOLOAD		0xE908
#define ADE7880_REG_VARNOLOAD		0xE909
#define ADE7880_REG_VANOLOAD		0xE90A
#define ADE7880_REG_LAST_ADD		0xE9FE
#define ADE7880_REG_LAST_RWDATA16	0xE9FF
#define ADE7880_REG_CONFIG3		0xEA00
#define ADE7880_REG_LAST_OP		0xEA01
#define ADE7880_REG_WTHR		0xEA02
#define ADE7880_REG_VARTHR 		0xEA03
#define ADE7880_REG_VATHR		0xEA04
#define ADE7880_REG_HX			0xEA08
#define ADE7880_REG_HY 			0xEA09
#define ADE7880_REG_HZ			0xEA0A
#define ADE7880_REG_LPOILVL		0xEC00
#define ADE7880_REG_CONFIG2 		0xEC01

/* ADE7880_REG_IPEAK Bit Definition */
#define ADE7880_IPPHASE2		NO_OS_BIT(26)
#define ADE7880_IPPHASE1		NO_OS_BIT(25)
#define ADE7880_IPPHASE0		NO_OS_BIT(24)
#define ADE7880_IPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE7880_REG_VPEAK Bit Definition */
#define ADE7880_VPPHASE2		NO_OS_BIT(26)
#define ADE7880_VPPHASE1		NO_OS_BIT(25)
#define ADE7880_VPPHASE0		NO_OS_BIT(24)
#define ADE7880_VPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE7880_REG_STATUS0 Bit Definition */
#define ADE7880_STATUS0_HREADY		NO_OS_BIT(19)
#define ADE7880_STATUS0_REVPSUM3	NO_OS_BIT(18)
#define ADE7880_STATUS0_DREADY		NO_OS_BIT(17)
#define ADE7880_STATUS0_CF3		NO_OS_BIT(16)
#define ADE7880_STATUS0_CF2		NO_OS_BIT(15)
#define ADE7880_STATUS0_CF1		NO_OS_BIT(14)
#define ADE7880_STATUS0_REVPSUM2	NO_OS_BIT(13)
#define ADE7880_STATUS0_REVFRPC		NO_OS_BIT(12)
#define ADE7880_STATUS0_REVFRPB 	NO_OS_BIT(11)
#define ADE7880_STATUS0_REVFRPA 	NO_OS_BIT(10)
#define ADE7880_STATUS0_REVPSUM1 	NO_OS_BIT(9)
#define ADE7880_STATUS0_REVAPC 		NO_OS_BIT(8)
#define ADE7880_STATUS0_REVAPB 		NO_OS_BIT(7)
#define ADE7880_STATUS0_REVAPA 		NO_OS_BIT(6)
#define ADE7880_STATUS0_LENERGY 	NO_OS_BIT(5)
#define ADE7880_STATUS0_VAEHF 		NO_OS_BIT(4)
#define ADE7880_STATUS0_FREHF 		NO_OS_BIT(3)
#define ADE7880_STATUS0_FAEHF 		NO_OS_BIT(1)
#define ADE7880_STATUS0_AEHF 		NO_OS_BIT(0)

/* ADE7880_REG_STATUS1 Bit Definition */
#define ADE7880_STATUS1_CRC		NO_OS_BIT(25)
#define ADE7880_STATUS1_PKV		NO_OS_BIT(24)
#define ADE7880_STATUS1_PKI		NO_OS_BIT(23)
#define ADE7880_STATUS1_MISMTCH		NO_OS_BIT(20)
#define ADE7880_STATUS1_SEQERR		NO_OS_BIT(19)
#define ADE7880_STATUS1_OV		NO_OS_BIT(18)
#define ADE7880_STATUS1_OI		NO_OS_BIT(17)
#define ADE7880_STATUS1_SAG		NO_OS_BIT(16)
#define ADE7880_STATUS1_RSTDONE		NO_OS_BIT(15)
#define ADE7880_STATUS1_ZXIC		NO_OS_BIT(14)
#define ADE7880_STATUS1_ZXIB		NO_OS_BIT(13)
#define ADE7880_STATUS1_ZXIA		NO_OS_BIT(12)
#define ADE7880_STATUS1_ZXVC		NO_OS_BIT(11)
#define ADE7880_STATUS1_ZXVB		NO_OS_BIT(10)
#define ADE7880_STATUS1_ZXVA		NO_OS_BIT(9)
#define ADE7880_STATUS1_ZXTOIC		NO_OS_BIT(8)
#define ADE7880_STATUS1_ZXTOIB		NO_OS_BIT(7)
#define ADE7880_STATUS1_ZXTOIA		NO_OS_BIT(6)
#define ADE7880_STATUS1_ZXTOVC		NO_OS_BIT(5)
#define ADE7880_STATUS1_ZXTOVB		NO_OS_BIT(4)
#define ADE7880_STATUS1_ZXTOVA		NO_OS_BIT(3)
#define ADE7880_STATUS1_VANLOAD		NO_OS_BIT(2)
#define ADE7880_STATUS1_FNLOAD		NO_OS_BIT(1)
#define ADE7880_STATUS1_NLOAD		NO_OS_BIT(0)

/* ADE7880_REG_MASK0 Bit Definition */
#define ADE7880_MASK0_HREADY		NO_OS_BIT(19)
#define ADE7880_MASK0_REVPSUM3 		NO_OS_BIT(18)
#define ADE7880_MASK0_DREADY		NO_OS_BIT(17)
#define ADE7880_MASK0_CF3		NO_OS_BIT(16)
#define ADE7880_MASK0_CF2		NO_OS_BIT(15)
#define ADE7880_MASK0_CF1		NO_OS_BIT(14)
#define ADE7880_MASK0_REVPSUM2		NO_OS_BIT(13)
#define ADE7880_MASK0_REVFRPC		NO_OS_BIT(12)
#define ADE7880_MASK0_REVFRPB		NO_OS_BIT(11)
#define ADE7880_MASK0_REVFRPA		NO_OS_BIT(10)
#define ADE7880_MASK0_REVPSUM1	 	NO_OS_BIT(9)
#define ADE7880_MASK0_REVAPC		NO_OS_BIT(8)
#define ADE7880_MASK0_REVAPB		NO_OS_BIT(7)
#define ADE7880_MASK0_REVAPA		NO_OS_BIT(6)
#define ADE7880_MASK0_LENERGY		NO_OS_BIT(5)
#define ADE7880_MASK0_VAEHF		NO_OS_BIT(4)
#define ADE7880_MASK0_FREHF		NO_OS_BIT(3)
#define ADE7880_MASK0_FAEHF		NO_OS_BIT(1)
#define ADE7880_MASK0_AEHF		NO_OS_BIT(0)

/* ADE7880_REG_MASK1 Bit Definition */
#define ADE7880_MASK1_CRC		NO_OS_BIT(25)
#define ADE7880_MASK1_PKV		NO_OS_BIT(24)
#define ADE7880_MASK1_PKI		NO_OS_BIT(23)
#define ADE7880_MASK1_MISMTCH		NO_OS_BIT(20)
#define ADE7880_MASK1_SEQERR		NO_OS_BIT(19)
#define ADE7880_MASK1_OV		NO_OS_BIT(18)
#define ADE7880_MASK1_OI		NO_OS_BIT(17)
#define ADE7880_MASK1_SAG		NO_OS_BIT(16)
#define ADE7880_MASK1_RSTDONE		NO_OS_BIT(15)
#define ADE7880_MASK1_ZXIC		NO_OS_BIT(14)
#define ADE7880_MASK1_ZXIB		NO_OS_BIT(13)
#define ADE7880_MASK1_ZXIA		NO_OS_BIT(12)
#define ADE7880_MASK1_ZXVC 		NO_OS_BIT(11)
#define ADE7880_MASK1_ZXVB		NO_OS_BIT(10)
#define ADE7880_MASK1_ZXVA		NO_OS_BIT(9)
#define ADE7880_MASK1_ZXTOIC		NO_OS_BIT(8)
#define ADE7880_MASK1_ZXTOIB		NO_OS_BIT(7)
#define ADE7880_MASK1_ZXTOIA		NO_OS_BIT(6)
#define ADE7880_MASK1_ZXTOVC		NO_OS_BIT(5)
#define ADE7880_MASK1_ZXTOVB		NO_OS_BIT(4)
#define ADE7880_MASK1_ZXTOVA		NO_OS_BIT(3)
#define ADE7880_MASK1_VANLOAD		NO_OS_BIT(2)
#define ADE7880_MASK1_FNLOAD		NO_OS_BIT(1)
#define ADE7880_MASK1_NLOAD		NO_OS_BIT(0)

/* ADE7880_REG_PHSTATUS Bit Definition */
#define ADE7880_VSPHASE2		NO_OS_BIT(14)
#define ADE7880_VSPHASE1		NO_OS_BIT(13)
#define ADE7880_VSPHASE0		NO_OS_BIT(12)
#define ADE7880_OVPHASE2		NO_OS_BIT(11)
#define ADE7880_OVPHASE1		NO_OS_BIT(10)
#define ADE7880_OVPHASE0		NO_OS_BIT(9)
#define ADE7880_OIPHASE2		NO_OS_BIT(5)
#define ADE7880_OIPHASE1		NO_OS_BIT(4)
#define ADE7880_OIPHASE0		NO_OS_BIT(3)

/* ADE7880_REG_PHNOLOAD Bit Definition */
#define ADE7880_VANLPHASE2		NO_OS_BIT(8)
#define ADE7880_VANLPHASE1		NO_OS_BIT(7)
#define ADE7880_VANLPHASE0		NO_OS_BIT(6)
#define ADE7880_FNLPHASE2		NO_OS_BIT(5)
#define ADE7880_FNLPHASE1		NO_OS_BIT(4)
#define ADE7880_FNLPHASE0		NO_OS_BIT(3)
#define ADE7880_NLPHASE2		NO_OS_BIT(2)
#define ADE7880_NLPHASE1		NO_OS_BIT(1)
#define ADE7880_NLPHASE0		NO_OS_BIT(0)

/* ADE7880_REG_COMPMODE Bit Definition */
#define ADE7880_SELFREQ			NO_OS_BIT(14)
#define ADE7880_VNOMCEN			NO_OS_BIT(13)
#define ADE7880_VNOMBEN			NO_OS_BIT(12)
#define ADE7880_VNOMAEN			NO_OS_BIT(11)
#define ADE7880_ANGLESEL		NO_OS_GENMASK(10, 9)
#define ADE7880_TERMSEL3_2		NO_OS_BIT(8)
#define ADE7880_TERMSEL3_1		NO_OS_BIT(7)
#define ADE7880_TERMSEL3_0		NO_OS_BIT(6)
#define ADE7880_TERMSEL2_2		NO_OS_BIT(5)
#define ADE7880_TERMSEL2_1		NO_OS_BIT(4)
#define ADE7880_TERMSEL2_0		NO_OS_BIT(3)
#define ADE7880_TERMSEL1_2		NO_OS_BIT(2)
#define ADE7880_TERMSEL1_1		NO_OS_BIT(1)
#define ADE7880_TERMSEL1_0		NO_OS_BIT(0)

/* ADE7880_REG_GAIN Bit Definition */
#define ADE7880_PGA3			NO_OS_GENMASK(8, 6)
#define ADE7880_PGA2			NO_OS_GENMASK(5, 3)
#define ADE7880_PGA1			NO_OS_GENMASK(2, 0)

/* ADE7880_REG_CFMODE Bit Definition */
#define ADE7880_CF3LATCH		NO_OS_BIT(14)
#define ADE7880_CF2LATCH		NO_OS_BIT(13)
#define ADE7880_CF1LATCH		NO_OS_BIT(12)
#define ADE7880_CF3DIS			NO_OS_BIT(11)
#define ADE7880_CF2DIS			NO_OS_BIT(10)
#define ADE7880_CF1DIS			NO_OS_BIT(9)
#define ADE7880_CF3SEL			NO_OS_GENMASK(8, 6)
#define ADE7880_CF2SEL			NO_OS_GENMASK(5, 3)
#define ADE7880_CF1SEL			NO_OS_GENMASK(2, 0)

/* ADE7880_REG_APHCAL, ADE7880_REG_BPHCAL,
ADE7880_REG_CPHCAL, Bit Definition */
#define ADE7880_PHCALVAL		NO_OS_GENMASK(9, 0)

/* ADE7880_REG_PHSIGN Bit Definition */
#define ADE7880_SUM3SIGN		NO_OS_BIT(8)
#define ADE7880_SUM2SIGN		NO_OS_BIT(7)
#define ADE7880_CFVARSIGN		NO_OS_BIT(6)
#define ADE7880_BFVARSIGN		NO_OS_BIT(5)
#define ADE7880_AFVARSIGN		NO_OS_BIT(4)
#define ADE7880_SUM1SIGN		NO_OS_BIT(3)
#define ADE7880_CWSIGN			NO_OS_BIT(2)
#define ADE7880_BWSIGN			NO_OS_BIT(1)
#define ADE7880_AWSIGN			NO_OS_BIT(0)

/* ADE7880_REG_CONFIG Bit Definition */
#define ADE7880_VTOIC			NO_OS_GENMASK(13, 12)
#define ADE7880_VTOIB			NO_OS_GENMASK(11, 10)
#define ADE7880_VTOIA			NO_OS_GENMASK(9, 8)
#define ADE7880_SWRST 			NO_OS_BIT(7)
#define ADE7880_HSDCEN 			NO_OS_BIT(6)
#define ADE7880_MOD2SHORT 		NO_OS_BIT(5)
#define ADE7880_MOD1SHORT 		NO_OS_BIT(4)
#define ADE7880_SWAP 			NO_OS_BIT(3)
#define ADE7880_CF2DIS 			NO_OS_BIT(2)
#define ADE7880_INTEN 			NO_OS_BIT(0)

/* ADE7880_REG_MMODE Bit Definition */
#define ADE7880_PEAKSEL2		NO_OS_BIT(4)
#define ADE7880_PEAKSEL1		NO_OS_BIT(3)
#define ADE7880_PEAKSEL0		NO_OS_BIT(2)

/* ADE7880_REG_ACCMODE Bit Definition */
#define ADE7880_REVAPSEL	 	NO_OS_BIT(6)
#define ADE7880_CONSEL	 		NO_OS_GENMASK(5, 4)
#define ADE7880_VARACC	 		NO_OS_GENMASK(3, 2)
#define ADE7880_WATTACC	 		NO_OS_GENMASK(1, 0)

/* ADE7880_REG_LCYCMODE Bit Definition */
#define ADE7880_PFMODE	 		NO_OS_BIT(7)
#define ADE7880_RSTREAD	 		NO_OS_BIT(6)
#define ADE7880_ZXSEL2	 		NO_OS_BIT(5)
#define ADE7880_ZXSEL1	 		NO_OS_BIT(4)
#define ADE7880_ZXSEL0	 		NO_OS_BIT(3)
#define ADE7880_LVA	 		NO_OS_BIT(2)
#define ADE7880_LVAR	 		NO_OS_BIT(1)
#define ADE7880_LWATT	 		NO_OS_BIT(0)

/* ADE7880_REG_HSDC_CFG Bit Definition */
#define ADE7880_HSAPOL			NO_OS_BIT(5)
#define ADE7880_HXFER	 		NO_OS_GENMASK(4, 3)
#define ADE7880_HGAP			NO_OS_BIT(2)
#define ADE7880_HSIZE			NO_OS_BIT(1)
#define ADE7880_HCLK			NO_OS_BIT(0)

/* ADE7880_REG_CONFIG3 Bit Definition */
#define ADE7880_ININTEN			NO_OS_BIT(3)
#define ADE7880_INSEL			NO_OS_BIT(2)
#define ADE7880_LPFSEL			NO_OS_BIT(1)
#define ADE7880_HPFEN			NO_OS_BIT(0)

/* ADE7880_REG_HCONFIG Bit Definition */
#define ADE7880_ACTPHSEL		NO_OS_GENMASK(9, 8)
#define ADE7880_HRATE			NO_OS_GENMASK(7, 5)
#define ADE7880_HSTIME			NO_OS_GENMASK(4, 3)
#define ADE7880_HPHASE			NO_OS_GENMASK(2, 1)
#define ADE7880_HRCFG			NO_OS_BIT(0)

/* ADE7880_REG_LPOILVL Bit Definition */
#define ADE7880_LPLINE			NO_OS_GENMASK(7, 3)
#define ADE7880_LPOIL			NO_OS_GENMASK(2, 0)

/* ADE7880_REG_CONFIG2 Bit Definition */
#define ADE7880_I2C_LOCK		NO_OS_BIT(1)
#define ADE7880_EXTREFEN		NO_OS_BIT(0)

/* Miscellaneous Definitions */
#define ADE7880_CHIP_ID			0x0EA0
#define ADE7880_RESET_RECOVER   	100
#define ADE7880_RAM_PROTECTION1 	0xE7FE
#define ADE7880_RAM_PROTECTION2 	0xE7E3
#define ADE7880_RAM_PROT_VAL1   	0xAD
#define ADE7880_RAM_PROT_VAL2   	0x80
#define ADE7880_SET_SPI_ADDR        	0xEBFF
#define ADE7880_DUMB_VAL            	0x01


/*Configuration registers*/
/* Set DICOEFF= 0xFFFF8000 when integrator is enabled*/
#define ADE7880_DICOEFF 		0x00000000
#define ADE7880_VLEVEL_VAL      	0x007A1200
/*Constant Definitions***/
/*DSP ON*/
#define ADE7880_RUN_ON 			0x0001
// /*Full scale Codes (FS) referred from Datasheet.*/
// /*Respective digital codes are produced when ADC inputs*/
// /*are at full scale. Do not Change. */
#define ADE7880_RMS_FS_CODES  		5326737

/* Assuming a transformer ratio of 1000:1 and 10 ohms burden resistance value */
#define ADE7880_BURDEN_RES              10
#define ADE7880_CURRENT_TR_RATIO        1000
#define ADE7880_CURRENT_TR_FCN          (ADE7880_CURRENT_TR_RATIO / ADE7880_BURDEN_RES)
/* Assuming a voltage divider with Rlow 1k and Rup 1MEG */
#define ADE7880_UP_RES                	1000000
#define ADE7880_DOWN_RES		1000
#define ADE7880_VOLTAGE_TR_FCN		((ADE7880_DOWN_RES + ADE7880_UP_RES) / ADE7880_DOWN_RES)

// 0.5V full scale * 0.707 * 10000 for mili units
#define ADE7880_FS_VOLTAGE           	3535

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum ade7880_anglesel_e
 * @brief ADE7880 angles measurement.
 */
enum ade7880_anglesel_e {
	/* The angles between phase voltages and phase currents are measured. */
	ADE7880_ANGLESEL_V_I,
	/* The angles between phase voltages are measured */
	ADE7880_ANGLESEL_V,
	/* The angles between phase currents are measured. */
	ADE7880_ANGLESEL_I,
	/* no angles are measured. */
	ADE7880_NO_ANGLESEL
};

/**
 * @enum ade7880_pga_gain_e
 * @brief ADE7880 phase currents/neutral current/phase voltages gain selection
    is currently being used.
 */
enum ade7880_pga_gain_e {
	/* gain 1 */
	ADE7880_PGAGAIN_1,
	/* gain 2 */
	ADE7880_PGAGAIN_2,
	/* gain 4 */
	ADE7880_PGAGAIN_4,
	/* gain 8 */
	ADE7880_PGAGAIN_8,
	/* gain 16 */
	ADE7880_PGAGAIN_16,
};

/**
 * @enum ade7880_cfxsel_e
 * @brief ADE7880 These bits indicate the value the CFx frequency
    is proportional to
 */
enum ade7880_cfxsel_e {
	/* the CFx frequency is proportional to the sum of total active powers on
	each phase */
	ADE7880_CFXSEL_0,
	/* the CFx frequency is proportional to the sum of apparent powers on
	each phase */
	ADE7880_CFXSEL_1 = 2,
	/* the CFx frequency is proportional to the sum of fundamental active
	powers on each phase */
	ADE7880_CFXSEL_2,
	/* the CFx frequency is proportional to the sum of fundamental reactive
	powers on each phase */
	ADE7880_CFXSEL_3
};

/**
 * @enum ade7880_vtoia_e
 * @brief ADE7880 These bits decide what phase voltage is considered
    together with Phase A current in the power path.
 */
enum ade7880_vtoia_e {
	/* Phase A voltage */
	ADE7880_VTOIA_A,
	/* Phase B voltage */
	ADE7880_VTOIA_B,
	/* Phase C voltage */
	ADE7880_VTOIA_C
};

/**
 * @enum ade7880_vtoib_e
 * @brief ADE7880 These bits decide what phase voltage is considered
    together with Phase B current in the power path.
 */
enum ade7880_vtoib_e {
	/* Phase B voltage */
	ADE7880_VTOIB_B,
	/* Phase C voltage */
	ADE7880_VTOIB_C,
	/* Phase A voltage */
	ADE7880_VTOIB_A
};

/**
 * @enum ade7880_vtoic_e
 * @brief ADE7880 These bits decide what phase voltage is considered
    together with Phase C current in the power path.
 */
enum ade7880_vtoic_e {
	/* Phase C voltage */
	ADE7880_VTOIC_C,
	/* Phase A voltage */
	ADE7880_VTOIC_A,
	/* Phase B voltage */
	ADE7880_VTOIC_B
};

/**
 * @enum ade7880_wattacc_e
 * @brief ADE7880 These bits decide the accumulation mode of fundamental active powers
 */
enum ade7880_wattacc_e {
	/* Signed accumulation mode of the total and fundamental active powers */
	ADE7880_WATTACC_SIGNED_ACC,
	/* Positive only accumulation mode of the total and fundamental active powers */
	ADE7880_WATTACC_POSITIVE_ACC,
	/* Reserved. When set, the device behaves like WATTACC[1:0] = 00. */
	ADE7880_WATTACC_RESERVED,
	/* Absolute accumulation mode of the total and fundamental active powers */
	ADE7880_WATTACC_ABSOLUTE_ACC
};

/**
 * @enum ade7880_varacc_e
 * @brief ADE7880 These bits decide the accumulation mode of funamental reactive powers
 */
enum ade7880_varacc_e {
	/* Signed accumulation mode of the fundamental reactive powers */
	ADE7880_VARACC_SIGNED_ACC,
	/* reserved. When set, the device behaves like VARACC[1:0] = 00. */
	ADE7880_VARACC_RESERVED,
	/* The fundamental reactive power is accumulated, depending on the sign of the
	fundamental active power */
	ADE7880_VARACC_SIGN_WATTACC,
	/* Absolute accumulation mode of the fundamental reactive powers */
	ADE7880_VARACC_ABSOLUTE_ACC
};

/**
 * @enum ade7880_consel_e
 * @brief ADE7880 These bits select the inputs to the energy accumulation registers. IA’, IB’, and IC’ are IA, IB, and
    IC shifted respectively by −90°
 */
enum ade7880_consel_e {
	/* 3-phase four wires with three voltage sensors */
	ADE7880_CONSEL_3P_3W,
	/* 3-phase three wires delta connection. In this mode, BVRMS register contains the rms
	value of VA-VC */
	ADE7880_CONSEL_3P_3W_DELTA,
	/* 3-phase four wires with two voltage sensors. */
	ADE7880_CONSEL_3P_4W,
	/* 3-phase four wires delta connection. */
	ADE7880_CONSEL_3P_4W_DELTA
};

/**
 * @enum ade7880_hxfer_e
 * @brief ADE7880 These bits select the data transmitted on HSDC
 */
enum ade7880_hxfer_e {
	/* HSDC transmits sixteen 32-bit words in the following order: IAWV, VAWV, IBWV, VBWV, ICWV,
	VCWV, INWV, AVA, BVA, CVA, AWATT, BWATT, CWATT, AFVAR, BFVAR, and CFVAR. */
	ADE7880_HXFER_16,
	/* HSDC transmits seven instantaneous values of currents and voltages: IAWV, VAWV,
	IBWV, VBWV, ICWV, VCWV, and INWV. */
	ADE7880_HXFER_7,
	/* HSDC transmits nine instantaneous values of phase powers: AVA, BVA, CVA, AWATT,
	BWATT, CWATT, AFVAR, BFVAR, and CFVAR. */
	ADE7880_HXFER_9,
	/* 11 = reserved. If set, the ADE7880 behaves as if HXFER[1:0] = 00. */
	ADE7880_HXFER_RESERVED
};

/**
 * @enum ade7880_hphase_e
 * @brief ADE7880 These bits decide what phase or neutral is analyzed by the harmonic calculations block.
 */
enum ade7880_hphase_e {
	/* Phase A voltage and current */
	ADE7880_HPHASE_A,
	/* Phase B voltage and current */
	ADE7880_HPHASE_B,
	/* Phase C voltage and current */
	ADE7880_HPHASE_C,
	/* Neutral current */
	ADE7880_HPHASE_N
};

/**
 * @enum ade7880_hstime_e
 * @brief ADE7880 These bits decide the delay period after which, if HRCFG bit is set to 0,
    Bit 19 (HREADY) in the STATUS0 register is set to 1.
 */
enum ade7880_hstime_e {
	/* 500 ms */
	ADE7880_HSTIM_500,
	/* 750 ms */
	ADE7880_HSTIM_700,
	/* 1000 ms */
	ADE7880_HSTIM_1000,
	/* 1250 ms */
	ADE7880_HSTIM_1250
};

/**
 * @enum ade7880_hrate_e
 * @brief ADE7880 These bits manage the update rate of the harmonic registers.
 */
enum ade7880_hrate_e {
	/* 125 µs (8 kHz rate) */
	ADE7880_HRATE_8K,
	/* 250 µs (4 kHz rate) */
	ADE7880_HRATE_4K,
	/* 1 ms (1 kHz rate) */
	ADE7880_HRATE_1K,
	/* 16 ms (62.5 Hz rate) */
	ADE7880_HRATE_62_5,
	/* 128 ms (7.8125 Hz rate) */
	ADE7880_HRATE_7_8125,
	/* 512 ms (1.953125 Hz rate) */
	ADE7880_HRATE_1_953125,
	/* 1.024 sec (0.9765625 Hz rate) */
	ADE7880_HRATE_0_9765625,
	/* harmonic calculations disabled */
	ADE7880_HRATE_DISABLED
};

/**
 * @enum ade7880_actphsel_e
 * @brief ADE7880 These bits select the phase voltage used as time base for harmonic calculations.
 */
enum ade7880_actphsel_e {
	/* Phase A voltage */
	ADE7880_ACTPHSEL_A,
	/* Phase B voltage */
	ADE7880_ACTPHSEL_B,
	/* Phase C voltage */
	ADE7880_ACTPHSEL_C,
	/* reserved. If selected, phase C voltage is used */
	ADE7880_ACTPHSEL_RESERVED
};

/**
 * @enum ade7880_freq_sel_e
 * @brief ADE7880 Freq value
 */
enum ade7880_freq_sel_e {
	/* 50 Hz */
	ADE7880_SELFREQ_50,
	/* 60 Hz */
	ADE7880_SELFREQ_60,
};

/**
 * @enum ade7880_phase
 * @brief ADE7880 available phases.
 */
enum ade7880_phase {
	ADE7880_PHASE_A,
	ADE7880_PHASE_B,
	ADE7880_PHASE_C
};

/**
 * @enum ade7880_power_mode_e
 * @brief ADE7880 Power mode selection
 */
enum ade7880_power_mode_e {
	/* SPI is not available in PSM2 & PSM3*/
	/* PSM0 normal mode */
	NORMAL_MODE,
	/* PSM1 measure the mean absolute values (mav)
	of the 3-phase currents */
	REDUCED_POWER_MODE,
	/* PSM2 Current peak detect mode */
	LOW_POWER_MODE,
	/* PSM3 Sleep mode */
	SLEEP_MODE
};

/**
 * @struct ade7880_init_param
 * @brief ADE7880 Device initialization parameters.
 */
struct ade7880_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/* psm0 descriptor */
	struct no_os_gpio_desc      	*psm0_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc      	*psm1_desc;
	/* reset descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* Variable for mode selection */
	uint8_t             		power_mode;
};

/**
 * @struct ade7880_dev
 * @brief ADE7880 Device structure.
 */
struct ade7880_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/* psm0 descriptor */
	struct no_os_gpio_desc      	*psm0_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc      	*psm1_desc;
	/* reset descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* Variable storing the IRMS value */
	uint32_t			irms_val;
	/* Variable storing the VRMS value */
	uint32_t			vrms_val;
	/* Variable for mode selection */
	uint8_t             		power_mode;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/* Read device register. */
int ade7880_read(struct ade7880_dev *dev, uint16_t reg_addr,
		 uint32_t *reg_data);

/* Write device register. */
int ade7880_write(struct ade7880_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Set power mode */
int ade7880_set_power_mode(struct ade7880_dev *dev);

/* Update specific register bits. */
int ade7880_update_bits(struct ade7880_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Read Energy/Power for specific phase */
int ade7880_read_data_ph(struct ade7880_dev *dev, enum ade7880_phase phase);

/* Initialize the device. */
int ade7880_init(struct ade7880_dev **device,
		 struct ade7880_init_param init_param);

/* Setup the device */
int ade7880_setup(struct ade7880_dev *dev);

/* Remove the device and release resources. */
int ade7880_remove(struct ade7880_dev *dev);

/* Get interrupt indicator from STATUS0 register. */
int ade7880_get_int_status0(struct ade7880_dev *dev, uint32_t msk,
			    uint8_t *status);

#endif // __ADE7880_H__
