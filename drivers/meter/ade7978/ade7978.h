/***************************************************************************//**
 *   @file   ade7978.h
 *   @brief  Header file of ADE7978 Driver.
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
#ifndef __ADE7978_H__
#define __ADE7978_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/* SPI commands */
#define ADE7978_SPI_READ_CMD        	0x01
#define ADE7978_SPI_WRITE_CMD       	0x00

#define ENABLE                  	0x0001
#define DISABLE                 	0x0000

/* ADE7978 Register Map */
#define ADE7978_REG_AIGAIN		0x4380
#define ADE7978_REG_AVGAIN		0x4381
#define ADE7978_REG_AV2GAIN		0x4382
#define ADE7978_REG_BIGAIN		0x4383
#define ADE7978_REG_BVGAIN		0x4384
#define ADE7978_REG_BV2GAIN		0x4385
#define ADE7978_REG_CIGAIN		0x4386
#define ADE7978_REG_CVGAIN		0x4387
#define ADE7978_REG_CV2GAIN		0x4388
#define ADE7978_REG_NIGAIN		0x4389
#define ADE7978_REG_NVGAIN		0x438A
#define ADE7978_REG_NV2GAIN		0x438B
#define ADE7978_REG_AIRMSOS		0x438C
#define ADE7978_REG_AVRMSOS		0x438D
#define ADE7978_REG_AV2RMSOS		0x438E
#define ADE7978_REG_BIRMSOS 		0x438F
#define ADE7978_REG_BVRMSOS 		0x4390
#define ADE7978_REG_BV2RMSOS		0x4391
#define ADE7978_REG_CIRMSOS		0x4392
#define ADE7978_REG_CVRMSOS		0x4393
#define ADE7978_REG_CV2RMSOS 		0x4394
#define ADE7978_REG_NIRMSOS 		0x4395
#define ADE7978_REG_NVRMSOS 		0x4396
#define ADE7978_REG_NV2RMSOS 		0x4397
#define ADE7978_REG_ISUMLVL		0x4398
#define ADE7978_REG_APGAIN		0x4399
#define ADE7978_REG_BPGAIN		0x439A
#define ADE7978_REG_CPGAIN		0x439B
#define ADE7978_REG_AWATTOS		0x439C
#define ADE7978_REG_BWATTOS		0x439D
#define ADE7978_REG_CWATTOS		0x439E
#define ADE7978_REG_AVAROS		0x439F
#define ADE7978_REG_BVAROS		0x43A0
#define ADE7978_REG_CVAROS		0x43A1
#define ADE7978_REG_AFWATTOS		0x43A3
#define ADE7978_REG_BFWATTOS		0x43A4
#define ADE7978_REG_CFWATTOS		0x43A5
#define ADE7978_REG_AFVAROS		0x43A6
#define ADE7978_REG_BFVAROS     	0x43A7
#define ADE7978_REG_CFVAROS		0x43A8
#define ADE7978_REG_AFIRMSOS		0x43A9
#define ADE7978_REG_BFIRMSOS		0x43AA
#define ADE7978_REG_CFIRMSOS		0x43AB
#define ADE7978_REG_AFVRMSOS		0x43AC
#define ADE7978_REG_BFVRMSOS		0x43AD
#define ADE7978_REG_CFVRMSOS		0x43AE
#define ADE7978_REG_TEMPCO		0x43AF
#define ADE7978_REG_ATEMP0		0x43B0
#define ADE7978_REG_BTEMP0		0x43B1
#define ADE7978_REG_CTEMP0 		0x43B2
#define ADE7978_REG_NTEMPO		0x43B3
#define ADE7978_REG_ATGAIN		0x43B4
#define ADE7978_REG_BTGAIN		0x43B5
#define ADE7978_REG_CTGAIN		0x43B6
#define ADE7978_REG_NTGAIN		0x43B7
#define ADE7978_REG_AIRMS		0x43C0
#define ADE7978_REG_AVRMS		0x43C1
#define ADE7978_REG_AV2RMS		0x43C2
#define ADE7978_REG_BIRMS		0x43C3
#define ADE7978_REG_BVRMS		0x43C4
#define ADE7978_REG_BV2RMS		0x43C5
#define ADE7978_REG_CIRMS		0x43C6
#define ADE7978_REG_CVRMS		0x43C7
#define ADE7978_REG_CV2RMS		0x43C8
#define ADE7978_REG_NIRMS		0X43C9
#define ADE7978_REG_ISUM		0X43CA
#define ADE7978_REG_ATEMP		0X43CB
#define ADE7978_REG_BTEMP		0X43CC
#define ADE7978_REG_CTEMP		0X43CD
#define ADE7978_REG_NTEMP		0X43CE

/* Internal DSP memory RAM registers */
#define ADE7978_REG_RUN			0xE228

/* Billable registers */
#define ADE7978_REG_AWATTHR		0xE400
#define ADE7978_REG_BWATTHR		0xE401
#define ADE7978_REG_CWATTHR		0xE402
#define ADE7978_REG_AFWATTHR		0xE403
#define ADE7978_REG_BFWATTHR		0xE404
#define ADE7978_REG_CFWATTHR		0xE405
#define ADE7978_REG_AVARHR		0xE406
#define ADE7978_REG_BVARHR		0xE407
#define ADE7978_REG_CVARHR		0xE408
#define ADE7978_REG_AFVARHR		0xE409
#define ADE7978_REG_BFVARHR		0xE40A
#define ADE7978_REG_CFVARHR		0xE40B
#define ADE7978_REG_AVAHR		0xE40C
#define ADE7978_REG_BVAHR		0xE40D
#define ADE7978_REG_CVAHR		0xE40E

/* Configuration and PQ registers */
#define ADE7978_REG_IPEAK		0xE500
#define ADE7978_REG_VPEAK		0xE501
#define ADE7978_REG_STATUS0		0xE502
#define ADE7978_REG_STATUS1		0xE503
#define ADE7978_REG_OILVL	 	0xE507
#define ADE7978_REG_OVLVL	 	0xE508
#define ADE7978_REG_SAGLVL	 	0xE509
#define ADE7978_REG_MASK0	 	0xE50A
#define ADE7978_REG_MASK1	 	0xE50B
#define ADE7978_REG_IAWV	 	0xE50C
#define ADE7978_REG_IBWV	 	0xE50D
#define ADE7978_REG_ICWV	 	0xE50E
#define ADE7978_REG_INWV	 	0xE50F
#define ADE7978_REG_VAWV		0xE510
#define ADE7978_REG_VBWV		0xE511
#define ADE7978_REG_VCWV		0xE512
#define ADE7978_REG_VA2WV		0xE513
#define ADE7978_REG_VB2WV		0xE514
#define ADE7978_REG_VC2WV		0xE515
#define ADE7978_REG_VNWV		0xE516
#define ADE7978_REG_VN2WV		0xE517
#define ADE7978_REG_AWATT		0xE518
#define ADE7978_REG_BWATT		0xE519
#define ADE7978_REG_CWATT		0xE51A
#define ADE7978_REG_AVAR	    	0xE51B
#define ADE7978_REG_BVAR		0xE51C
#define ADE7978_REG_CVAR		0xE51D
#define ADE7978_REG_AVA			0xE51E
#define ADE7978_REG_BVA			0xE51F
#define ADE7978_REG_CVA			0xE520
#define ADE7978_REG_AVTHD		0xE521
#define ADE7978_REG_AITHD		0xE522
#define ADE7978_REG_BVTHD		0xE523
#define ADE7978_REG_BITHD		0xE524
#define ADE7978_REG_CVTHD		0xE525
#define ADE7978_REG_CITHD		0xE526
#define ADE7978_REG_NVRMS		0xE530
#define ADE7978_REG_NV2RMS		0xE531
#define ADE7978_REG_CHECKSUM		0xE532
#define ADE7978_REG_VNOM		0xE533
#define ADE7978_REG_AFIRMS		0xE537
#define ADE7978_REG_AFVRMS 		0xE538
#define ADE7978_REG_BFIRMS		0xE539
#define ADE7978_REG_BFVRMS		0xE53A
#define ADE7978_REG_CFIRMS		0xE53B
#define ADE7978_REG_CFVRMS		0xE53C
#define ADE7978_REG_LAST_RWDATA32	0xE5FF
#define ADE7978_REG_PHSTATUS		0xE600
#define ADE7978_REG_ANGLE0		0xE601
#define ADE7978_REG_ANGLE1		0xE602
#define ADE7978_REG_ANGLE2		0xE603
#define ADE7978_REG_PHNOLOAD		0xE608
#define ADE7978_REG_LINECYC 		0xE60C
#define ADE7978_REG_ZXTOUT		0xE60D
#define ADE7978_REG_COMPMODE		0xE60E
#define ADE7978_REG_CFMODE      	0xE610
#define ADE7978_REG_CF1DEN		0xE611
#define ADE7978_REG_CF2DEN		0xE612
#define ADE7978_REG_CF3DEN		0xE613
#define ADE7978_REG_APHCAL		0xE614
#define ADE7978_REG_BPHCAL		0xE615
#define ADE7978_REG_CPHCAL		0xE616
#define ADE7978_REG_PHSIGN		0xE617
#define ADE7978_REG_CONFIG		0xE618
#define ADE7978_REG_MMODE		0xE700
#define ADE7978_REG_ACCMODE		0xE701
#define ADE7978_REG_LCYCMODE		0xE702
#define ADE7978_REG_PEAKCYC		0xE703
#define ADE7978_REG_SAGCYC		0xE704
#define ADE7978_REG_CFCYC		0xE705
#define ADE7978_REG_HSDC_CFG		0xE706
#define ADE7978_REG_VERSION		0xE707
#define ADE7978_REG_CONFIG3		0xE708
#define ADE7978_REG_ATEMPOS		0xE709
#define ADE7978_REG_BTEMPOS		0xE70A
#define ADE7978_REG_CTEMPOS		0xE70B
#define ADE7978_REG_NTEMPOS		0xE70C
#define ADE7978_REG_LAST_RWDATA8    	0xE7FD
#define ADE7978_REG_APF 		0xE902
#define ADE7978_REG_BPF 		0xE903
#define ADE7978_REG_CPF 		0xE904
#define ADE7978_REG_APERIOD		0xE905
#define ADE7978_REG_BPERIOD		0xE906
#define ADE7978_REG_CPERIOD		0xE907
#define ADE7978_REG_APNOLOAD		0xE908
#define ADE7978_REG_VARNOLOAD		0xE909
#define ADE7978_REG_VANOLOAD		0xE90A
#define ADE7978_REG_LAST_ADD		0xE9FE
#define ADE7978_REG_LAST_RWDATA16	0xE9FF
#define ADE7978_REG_CONFIG2		0xEA00
#define ADE7978_REG_LAST_OP		0xEA01
#define ADE7978_REG_WTHR		0xEA02
#define ADE7978_REG_VARTHR		0xEA03
#define ADE7978_REG_VATHR		0xEA04

/* ADE7978_REG_IPEAK Bit Definition */
#define ADE7978_IPPHASE2		NO_OS_BIT(26)
#define ADE7978_IPPHASE1		NO_OS_BIT(25)
#define ADE7978_IPPHASE0		NO_OS_BIT(24)
#define ADE7978_IPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE7978_REG_VPEAK Bit Definition */
#define ADE7978_VPPHASE2		NO_OS_BIT(26)
#define ADE7978_VPPHASE1		NO_OS_BIT(25)
#define ADE7978_VPPHASE0		NO_OS_BIT(24)
#define ADE7978_VPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE7978_REG_STATUS0 Bit Definition */
#define ADE7978_STATUS0_REVPSUM3	NO_OS_BIT(18)
#define ADE7978_STATUS0_DREADY		NO_OS_BIT(17)
#define ADE7978_STATUS0_CF3		NO_OS_BIT(16)
#define ADE7978_STATUS0_CF2		NO_OS_BIT(15)
#define ADE7978_STATUS0_CF1		NO_OS_BIT(14)
#define ADE7978_STATUS0_REVPSUM2	NO_OS_BIT(13)
#define ADE7978_STATUS0_REVRPC		NO_OS_BIT(12)
#define ADE7978_STATUS0_REVRPB 		NO_OS_BIT(11)
#define ADE7978_STATUS0_REVRPA 		NO_OS_BIT(10)
#define ADE7978_STATUS0_REVPSUM1 	NO_OS_BIT(9)
#define ADE7978_STATUS0_REVAPC 		NO_OS_BIT(8)
#define ADE7978_STATUS0_REVAPB 		NO_OS_BIT(7)
#define ADE7978_STATUS0_REVAPA 		NO_OS_BIT(6)
#define ADE7978_STATUS0_LENERGY 	NO_OS_BIT(5)
#define ADE7978_STATUS0_VAEHF 		NO_OS_BIT(4)
#define ADE7978_STATUS0_FREHF 		NO_OS_BIT(3)
#define ADE7978_STATUS0_FAEHF 		NO_OS_BIT(1)
#define ADE7978_STATUS0_AEHF 		NO_OS_BIT(0)

/* ADE7978_REG_STATUS1 Bit Definition */
#define ADE7978_STATUS1_CRC		NO_OS_BIT(25)
#define ADE7978_STATUS1_PKV		NO_OS_BIT(24)
#define ADE7978_STATUS1_PKI		NO_OS_BIT(23)
#define ADE7978_STATUS1_MISMTCH		NO_OS_BIT(20)
#define ADE7978_STATUS1_SEQERR		NO_OS_BIT(19)
#define ADE7978_STATUS1_OV		NO_OS_BIT(18)
#define ADE7978_STATUS1_OI		NO_OS_BIT(17)
#define ADE7978_STATUS1_SAG		NO_OS_BIT(16)
#define ADE7978_STATUS1_RSTDONE		NO_OS_BIT(15)
#define ADE7978_STATUS1_ZXIC		NO_OS_BIT(14)
#define ADE7978_STATUS1_ZXIB		NO_OS_BIT(13)
#define ADE7978_STATUS1_ZXIA		NO_OS_BIT(12)
#define ADE7978_STATUS1_ZXVC		NO_OS_BIT(11)
#define ADE7978_STATUS1_ZXVB		NO_OS_BIT(10)
#define ADE7978_STATUS1_ZXVA		NO_OS_BIT(9)
#define ADE7978_STATUS1_ZXTOIC		NO_OS_BIT(8)
#define ADE7978_STATUS1_ZXTOIB		NO_OS_BIT(7)
#define ADE7978_STATUS1_ZXTOIA		NO_OS_BIT(6)
#define ADE7978_STATUS1_ZXTOVC		NO_OS_BIT(5)
#define ADE7978_STATUS1_ZXTOVB		NO_OS_BIT(4)
#define ADE7978_STATUS1_ZXTOVA		NO_OS_BIT(3)
#define ADE7978_STATUS1_VANLOAD		NO_OS_BIT(2)
#define ADE7978_STATUS1_FNLOAD		NO_OS_BIT(1)
#define ADE7978_STATUS1_NLOAD		NO_OS_BIT(0)

/* ADE7978_REG_MASK0 Bit Definition */
#define ADE7978_MASK0_HREADY		NO_OS_BIT(19)
#define ADE7978_MASK0_REVPSUM3 		NO_OS_BIT(18)
#define ADE7978_MASK0_DREADY		NO_OS_BIT(17)
#define ADE7978_MASK0_CF3		NO_OS_BIT(16)
#define ADE7978_MASK0_CF2		NO_OS_BIT(15)
#define ADE7978_MASK0_CF1		NO_OS_BIT(14)
#define ADE7978_MASK0_REVPSUM2		NO_OS_BIT(13)
#define ADE7978_MASK0_REVFRPC		NO_OS_BIT(12)
#define ADE7978_MASK0_REVFRPB		NO_OS_BIT(11)
#define ADE7978_MASK0_REVFRPA		NO_OS_BIT(10)
#define ADE7978_MASK0_REVPSUM1	 	NO_OS_BIT(9)
#define ADE7978_MASK0_REVAPC		NO_OS_BIT(8)
#define ADE7978_MASK0_REVAPB		NO_OS_BIT(7)
#define ADE7978_MASK0_REVAPA		NO_OS_BIT(6)
#define ADE7978_MASK0_LENERGY		NO_OS_BIT(5)
#define ADE7978_MASK0_VAEHF		NO_OS_BIT(4)
#define ADE7978_MASK0_FREHF		NO_OS_BIT(3)
#define ADE7978_MASK0_REHF		NO_OS_BIT(2)
#define ADE7978_MASK0_FAEHF		NO_OS_BIT(1)
#define ADE7978_MASK0_AEHF		NO_OS_BIT(0)

/* ADE7978_REG_MASK1 Bit Definition */
#define ADE7978_MASK1_CRC		NO_OS_BIT(25)
#define ADE7978_MASK1_PKV		NO_OS_BIT(24)
#define ADE7978_MASK1_PKI		NO_OS_BIT(23)
#define ADE7978_MASK1_MISMTCH		NO_OS_BIT(20)
#define ADE7978_MASK1_SEQERR		NO_OS_BIT(19)
#define ADE7978_MASK1_OV		NO_OS_BIT(18)
#define ADE7978_MASK1_OI		NO_OS_BIT(17)
#define ADE7978_MASK1_SAG		NO_OS_BIT(16)
#define ADE7978_MASK1_RSTDONE		NO_OS_BIT(15)
#define ADE7978_MASK1_ZXIC		NO_OS_BIT(14)
#define ADE7978_MASK1_ZXIB		NO_OS_BIT(13)
#define ADE7978_MASK1_ZXIA		NO_OS_BIT(12)
#define ADE7978_MASK1_ZXVC 		NO_OS_BIT(11)
#define ADE7978_MASK1_ZXVB		NO_OS_BIT(10)
#define ADE7978_MASK1_ZXVA		NO_OS_BIT(9)
#define ADE7978_MASK1_ZXTOIC		NO_OS_BIT(8)
#define ADE7978_MASK1_ZXTOIB		NO_OS_BIT(7)
#define ADE7978_MASK1_ZXTOIA		NO_OS_BIT(6)
#define ADE7978_MASK1_ZXTOVC		NO_OS_BIT(5)
#define ADE7978_MASK1_ZXTOVB		NO_OS_BIT(4)
#define ADE7978_MASK1_ZXTOVA		NO_OS_BIT(3)
#define ADE7978_MASK1_VANLOAD		NO_OS_BIT(2)
#define ADE7978_MASK1_FNLOAD		NO_OS_BIT(1)
#define ADE7978_MASK1_NLOAD		NO_OS_BIT(0)

/* ADE7978_REG_PHSTATUS Bit Definition */
#define ADE7978_VSPHASE2		NO_OS_BIT(14)
#define ADE7978_VSPHASE1		NO_OS_BIT(13)
#define ADE7978_VSPHASE0		NO_OS_BIT(12)
#define ADE7978_OVPHASE2		NO_OS_BIT(11)
#define ADE7978_OVPHASE1		NO_OS_BIT(10)
#define ADE7978_OVPHASE0		NO_OS_BIT(9)
#define ADE7978_OIPHASE2		NO_OS_BIT(5)
#define ADE7978_OIPHASE1		NO_OS_BIT(4)
#define ADE7978_OIPHASE0		NO_OS_BIT(3)

/* ADE7978_REG_PHNOLOAD Bit Definition */
#define ADE7978_VANLPHASE2		NO_OS_BIT(8)
#define ADE7978_VANLPHASE1		NO_OS_BIT(7)
#define ADE7978_VANLPHASE0		NO_OS_BIT(6)
#define ADE7978_FNLPHASE2		NO_OS_BIT(5)
#define ADE7978_FNLPHASE1		NO_OS_BIT(4)
#define ADE7978_FNLPHASE0		NO_OS_BIT(3)
#define ADE7978_NLPHASE2		NO_OS_BIT(2)
#define ADE7978_NLPHASE1		NO_OS_BIT(1)
#define ADE7978_NLPHASE0		NO_OS_BIT(0)

/* ADE7978_REG_COMPMODE Bit Definition */
#define ADE7978_SELFREQ			NO_OS_BIT(14)
#define ADE7978_VNOMCEN			NO_OS_BIT(13)
#define ADE7978_VNOMBEN			NO_OS_BIT(12)
#define ADE7978_VNOMAEN			NO_OS_BIT(11)
#define ADE7978_ANGLESEL		NO_OS_GENMASK(10, 9)
#define ADE7978_TERMSEL3_2		NO_OS_BIT(8)
#define ADE7978_TERMSEL3_1		NO_OS_BIT(7)
#define ADE7978_TERMSEL3_0		NO_OS_BIT(6)
#define ADE7978_TERMSEL2_2		NO_OS_BIT(5)
#define ADE7978_TERMSEL2_1		NO_OS_BIT(4)
#define ADE7978_TERMSEL2_0		NO_OS_BIT(3)
#define ADE7978_TERMSEL1_2		NO_OS_BIT(2)
#define ADE7978_TERMSEL1_1		NO_OS_BIT(1)
#define ADE7978_TERMSEL1_0		NO_OS_BIT(0)

/* ADE7978_REG_CFMODE Bit Definition */
#define ADE7978_CF3LATCH		NO_OS_BIT(14)
#define ADE7978_CF2LATCH		NO_OS_BIT(13)
#define ADE7978_CF1LATCH		NO_OS_BIT(12)
#define ADE7978_CF3DIS			NO_OS_BIT(11)
#define ADE7978_CF2DIS			NO_OS_BIT(10)
#define ADE7978_CF1DIS			NO_OS_BIT(9)
#define ADE7978_CF3SEL			NO_OS_GENMASK(8, 6)
#define ADE7978_CF2SEL			NO_OS_GENMASK(5, 3)
#define ADE7978_CF1SEL			NO_OS_GENMASK(2, 0)

/* ADE7978_REG_APHCAL, ADE7978_REG_BPHCAL,
ADE7978_REG_CPHCAL, Bit Definition */
#define ADE7978_PHCALVAL		NO_OS_GENMASK(9, 0)

/* ADE7978_REG_PHSIGN Bit Definition */
#define ADE7978_SUM3SIGN		NO_OS_BIT(8)
#define ADE7978_SUM2SIGN		NO_OS_BIT(7)
#define ADE7978_CVARSIGN		NO_OS_BIT(6)
#define ADE7978_BVARSIGN		NO_OS_BIT(5)
#define ADE7978_AVARSIGN		NO_OS_BIT(4)
#define ADE7978_SUM1SIGN		NO_OS_BIT(3)
#define ADE7978_CWSIGN			NO_OS_BIT(2)
#define ADE7978_BWSIGN			NO_OS_BIT(1)
#define ADE7978_AWSIGN			NO_OS_BIT(0)

/* ADE7978_REG_CONFIG Bit Definition */
#define ADE7978_INSEL           	NO_OS_BIT(14)
#define ADE7978_VTOIC			NO_OS_GENMASK(13, 12)
#define ADE7978_VTOIB			NO_OS_GENMASK(11, 10)
#define ADE7978_VTOIA			NO_OS_GENMASK(9, 8)
#define ADE7978_SWRST 			NO_OS_BIT(7)
#define ADE7978_HSDCEN 			NO_OS_BIT(6)
#define ADE7978_LPFSEL 		    	NO_OS_BIT(5)
#define ADE7978_HPFEN 		    	NO_OS_BIT(4)
#define ADE7978_SWAP 			NO_OS_BIT(3)
#define ADE7978_ZX_DREADY		NO_OS_GENMASK(1, 0)

/* ADE7978_REG_MMODE Bit Definition */
#define ADE7978_PEAKSEL2		NO_OS_BIT(4)
#define ADE7978_PEAKSEL1		NO_OS_BIT(3)
#define ADE7978_PEAKSEL0		NO_OS_BIT(2)
#define ADE7978_REVRPSEL        	NO_OS_BIT(1)
#define ADE7978_REVAPSEL       		NO_OS_BIT(0)

/* ADE7978_REG_ACCMODE Bit Definition */
#define ADE7978_SAGCFG	 	    	NO_OS_BIT(6)
#define ADE7978_CONSEL	 		NO_OS_GENMASK(5, 4)
#define ADE7978_VARACC	 		NO_OS_GENMASK(3, 2)
#define ADE7978_WATTACC	 		NO_OS_GENMASK(1, 0)

/* ADE7978_REG_LCYCMODE Bit Definition */
#define ADE7978_PFMODE	 		NO_OS_BIT(7)
#define ADE7978_RSTREAD	 		NO_OS_BIT(6)
#define ADE7978_ZXSEL2	 		NO_OS_BIT(5)
#define ADE7978_ZXSEL1	 		NO_OS_BIT(4)
#define ADE7978_ZXSEL0	 		NO_OS_BIT(3)
#define ADE7978_LVA	 		NO_OS_BIT(2)
#define ADE7978_LVAR	 		NO_OS_BIT(1)
#define ADE7978_LWATT	 		NO_OS_BIT(0)

/* ADE7978_REG_HSDC_CFG Bit Definition */
#define ADE7978_HSAPOL			NO_OS_BIT(5)
#define ADE7978_HXFER	 		NO_OS_GENMASK(4, 3)
#define ADE7978_HGAP			NO_OS_BIT(2)
#define ADE7978_HSIZE			NO_OS_BIT(1)
#define ADE7978_HCLK			NO_OS_BIT(0)

/* ADE7978_REG_CONFIG3 Bit Definition */
#define ADE7978_ADE7933_SWRST		NO_OS_BIT(7)
#define ADE7978_CLKOUT_DIS  		NO_OS_BIT(6)
#define ADE7978_VN2_EN			NO_OS_BIT(3)
#define ADE7978_VC2_EN			NO_OS_BIT(2)
#define ADE7978_VB2_EN			NO_OS_BIT(1)
#define ADE7978_VA2_EN			NO_OS_BIT(0)

/* ADE7978_REG_CONFIG2 Bit Definition */
#define ADE7978_I2C_LOCK		NO_OS_BIT(0)

/* Miscellaneous Definitions */
#define IS_8BITS_REG(x) ((x >= ADE7978_REG_MMODE && x <= ADE7978_REG_LAST_RWDATA8) \
        || (x >= ADE7978_REG_CONFIG2 && x <= ADE7978_SET_SPI_ADDR))
#define IS_16BITS_REG(x) ((x >= ADE7978_REG_PHSTATUS && x <= ADE7978_REG_CONFIG) \
        || (x >= ADE7978_REG_APF && x <= ADE7978_REG_LAST_RWDATA16) \
        || (x == ADE7978_REG_RUN))
#define ADE7978_CHIP_ID			0x0E88
#define ADE7978_RESET_RECOVER   	100
#define ADE7978_SET_SPI_ADDR      	0xEBFF
#define ADE7978_DUMB_VAL        	0x01
#define ADE7978_RAM_PROTECTION1 	0xE7FE
#define ADE7978_RAM_PROTECTION2 	0xE7E3
#define ADE7978_RAM_PROT_VAL1   	0xAD
#define ADE7978_RAM_PROT_VAL2   	0x80

/* Constant Definitions */
/* DSP ON */
#define ADE7978_RUN_ON 			0x0001
/* Full scale Codes (FS) referred from Datasheet.*/
/* Respective digital codes are produced when ADC inputs
are at full scale. Do not Change. */
#define ADE7978_WAVE_FS_CODES  		5320000
#define ADE7978_RMS_FS_CODES  		3761808

/* 0.5V full scale * 0.707 * 10000 for mili units */
#define ADE7978_FS_VOLTAGE_RMS          3535
/* 31.25 mV full scale * 0.707 * 1000 for micro units */
#define ADE7978_FS_CURRENT_RMS          22094
/* 31.25 mV full scale * 1000 for micro units */
#define ADE7978_FS_CURRENT              31250
/* 0.5V full scale * 1000 for mili units */
#define ADE7978_FS_VOLTAGE           	500

/**
 * @enum ade7978_cfxsel_e
 * @brief ADE7978 These bits indicate the value the CFx frequency
    is proportional to
 */
enum ade7978_cfxsel_e {
	/* the CFx frequency is proportional to the sum of total active powers on
	each phase */
	ADE7978_CFXSEL_0,
	/* the CFx frequency is proportional to the sum of the total reactive
	powers on each phase */
	ADE7978_CFXSEL_1,
	/* the CFx frequency is proportional to the sum of apparent powers
	on each phase */
	ADE7978_CFXSEL_2,
	/* the CFx frequency is proportional to the sum of the fundamental
	active powers on each phase */
	ADE7978_CFXSEL_3,
	/* the CFx frequency is proportional to the sum of the fundamental
	reactive powers on each phase */
	ADE7978_CFXSEL_4,
};

/**
 * @enum ade7978_zx_dready_e
 * @brief ADE7978 These bits manage the output signal at the ZX/DREADY pin
 */
enum ade7978_zx_dready_e {
	/* DREADY functionality is enabled */
	ADE7978_DREADY_EN,
	/* ZX functionality is generated by the Phase A voltage */
	ADE7978_ZX_PHASE_A,
	/* ZX functionality is generated by the Phase B voltage */
	ADE7978_ZX_PHASE_B,
	/* ZX functionality is generated by the Phase C voltage */
	ADE7978_ZX_PHASE_C
};

/**
 * @enum ade7978_vtoia_e
 * @brief ADE7978 These bits decide what phase voltage is considered
    together with Phase A current in the power path.
 */
enum ade7978_vtoia_e {
	/* Phase A voltage */
	ADE7978_VTOIA_A,
	/* Phase B voltage */
	ADE7978_VTOIA_B,
	/* Phase C voltage */
	ADE7978_VTOIA_C
};

/**
 * @enum ade7978_vtoib_e
 * @brief ADE7978 These bits decide what phase voltage is considered
    together with Phase B current in the power path.
 */
enum ade7978_vtoib_e {
	/* Phase B voltage */
	ADE7978_VTOIB_B,
	/* Phase C voltage */
	ADE7978_VTOIB_C,
	/* Phase A voltage */
	ADE7978_VTOIB_A
};

/**
 * @enum ade7978_vtoic_e
 * @brief ADE7978 These bits decide what phase voltage is considered
    together with Phase C current in the power path.
 */
enum ade7978_vtoic_e {
	/* Phase C voltage */
	ADE7978_VTOIC_C,
	/* Phase A voltage */
	ADE7978_VTOIC_A,
	/* Phase B voltage */
	ADE7978_VTOIC_B
};

/**
 * @enum ade7978_wattacc_e
 * @brief ADE7978 These bits decide the accumulation mode of fundamental
    active powers
 */
enum ade7978_wattacc_e {
	/* Signed accumulation mode of the total and fundamental active
	powers */
	ADE7978_WATTACC_SIGNED_ACC,
	/* Positive only accumulation mode of the total and fundamental
	active powers */
	ADE7978_WATTACC_POSITIVE_ACC,
	/* Reserved. When set, the device behaves like WATTACC[1:0] = 00. */
	ADE7978_WATTACC_RESERVED,
	/* Absolute accumulation mode of the total and fundamental
	active powers */
	ADE7978_WATTACC_ABSOLUTE_ACC
};

/**
 * @enum ade7978_varacc_e
 * @brief ADE7978 These bits decide the accumulation mode of funamental
    reactive powers
 */
enum ade7978_varacc_e {
	/* Signed accumulation mode of the fundamental reactive powers */
	ADE7978_VARACC_SIGNED_ACC,
	/* reserved. When set, the device behaves like VARACC[1:0] = 00. */
	ADE7978_VARACC_RESERVED,
	/* The fundamental reactive power is accumulated, depending on
	the sign of the fundamental active power */
	ADE7978_VARACC_SIGN_WATTACC,
	/* Absolute accumulation mode of the fundamental reactive powers */
	ADE7978_VARACC_ABSOLUTE_ACC
};

/**
 * @enum ade7978_consel_e
 * @brief ADE7978 These bits select the inputs to the energy
    accumulation registers.
    IA’, IB’, and IC’ are IA, IB, and IC shifted respectively by −90°
 */
enum ade7978_consel_e {
	/* 3-phase four wires with three voltage sensors */
	ADE7978_CONSEL_3P_3W,
	/* 3-phase three wires delta connection. */
	ADE7978_CONSEL_3P_3W_DELTA,
	/* 3-phase four wires delta connection. */
	ADE7978_CONSEL_3P_4W_DELTA
};

/**
 * @enum ade7978_hxfer_e
 * @brief ADE7978 These bits select the data transmitted on HSDC
 */
enum ade7978_hxfer_e {
	/* HSDC transmits sixteen 32-bit words in the following order:
	IAWV, VAWV, IBWV, VBWV, ICWV, VCWV, INWV, AVA, BVA, CVA, AWATT,
	BWATT, CWATT, AVAR, BVAR, and CVAR */
	ADE7978_HXFER_16,
	/* HSDC transmits seven instantaneous values of currents and
	voltages in the following order: IAWV, VAWV, IBWV, VBWV, ICWV,
	VCWV, and INWV */
	ADE7978_HXFER_7,
	/* HSDC transmits nine instantaneous values of phase powers
	in the following order: AVA, BVA, CVA, AWATT, BWATT, CWATT,
	AVAR, BVAR, and CVAR */
	ADE7978_HXFER_9,
	/* 11 = reserved. If set, the ADE7978 behaves as if
	HXFER[1:0] = 00. */
	ADE7978_HXFER_RESERVED
};

/**
 * @enum ade7978_freq_sel_e
 * @brief ADE7978 Freq value
 */
enum ade7978_freq_sel_e {
	/* 50 Hz */
	ADE7978_SELFREQ_50,
	/* 60 Hz */
	ADE7978_SELFREQ_60,
};

/**
 * @enum ade7978_phase
 * @brief ADE7978 available phases.
 */
enum ade7978_phase {
	ADE7978_PHASE_A,
	ADE7978_PHASE_B,
	ADE7978_PHASE_C
};

/**
 * @struct ade7978_init_param
 * @brief ADE7978 Device initialization parameters.
 */
struct ade7978_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/* reset descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* temperature enable */
	uint8_t             		temp_en;
};

/**
 * @struct ade7978_dev
 * @brief ADE7978 Device structure.
 */
struct ade7978_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/* reset descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* Variable storing the IRMS value in ADC code */
	uint32_t			irms_val;
	/* Variable storing the VRMS value in ADC code */
	uint32_t			vrms_val;
	/* Variable storing the V2RMS value in ADC code */
	uint32_t			v2rms_val;
	/* Variable storing the temperature in ADC code */
	uint32_t            		temperature;
	/* Variable for enabling the temperature measurement*/
	uint8_t             		temp_en;
	/** IRQ device descriptor used to handle interrupt routine */
	struct no_os_irq_ctrl_desc 	*irq_ctrl;
};

/* Read device register. */
int ade7978_read(struct ade7978_dev *dev, uint16_t reg_addr,
		 uint32_t *reg_data);

/* Write device register. */
int ade7978_write(struct ade7978_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Update specific register bits. */
int ade7978_update_bits(struct ade7978_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Read measurements for specific phase */
int ade7978_read_data_ph(struct ade7978_dev *dev, enum ade7978_phase phase);

/* Initialize the device. */
int ade7978_init(struct ade7978_dev **device,
		 struct ade7978_init_param init_param);

/* Setup the device */
int ade7978_setup(struct ade7978_dev *dev);

/* Remove the device and release resources. */
int ade7978_remove(struct ade7978_dev *dev);

/* Get interrupt indicator from STATUS0 register. */
int ade7978_get_int_status0(struct ade7978_dev *dev, uint32_t msk,
			    uint8_t *status);

/* Get interrupt indicator from STATUS01 register. */
int ade7978_get_int_status1(struct ade7978_dev *dev, uint32_t msk,
			    uint8_t *status);

#endif /* __ADE7978_H__ */
