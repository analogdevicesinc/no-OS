/***************************************************************************//**
 *   @file   adas1000.h
 *   @brief  Header file of ADAS1000 Driver.
 *   @author ACozma (andrei.cozma@analog.com)
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2012-2021(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*******************************************************************************/

#ifndef _ADAS1000_H_
#define _ADAS1000_H_

/*****************************************************************************/
/***************************** Include Files *********************************/
/*****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "no_os_spi.h"

/******************************************************************************/
/* ADAS1000 SPI Registers Memory Map */
/******************************************************************************/

#define  ADAS1000_COMM_WRITE  0x80  /* SPI Write command */
#define  ADAS1000_RDY_MASK    0x40  /* READY bit mask */
#define  ADAS1000_ALL_CH_MASK 0x00  /* Word mask for activating all channels */
#define  ADAS1000_WD_CNT_MASK 0x01  /* Word count mask */

#define	ADAS1000_NOP			0x00	/* NOP (No operation) */
#define	ADAS1000_ECGCTL		0x01	/* ECG Setting Register */
#define	ADAS1000_LOFFCTL		0x02	/* Leads off Control Register  */
#define	ADAS1000_RESPCTL		0x03	/* Respiration Control Register */
#define	ADAS1000_PACECTL		0x04	/* Pace Detection Control Register */
#define	ADAS1000_CMREFCTL		0x05	/* Common Mode Reference and Shield Drive Control Register */
#define	ADAS1000_GPIOCTL		0x06	/* GPIO Control Register */
#define	ADAS1000_PACEAMPTH	0x07	/* Pace Amplitude Threshold2 */
#define	ADAS1000_TESTTONE		0x08	/* Test Tone */
#define	ADAS1000_CALDAC		0x09	/* Calibration DAC */
#define	ADAS1000_FRMCTL		0x0A	/* Frame Control Register */
#define	ADAS1000_FILTCTL		0x0B	/* Filter Control Register */
#define	ADAS1000_LOFFUTH		0x0C	/* Leads off Upper Threshold */
#define	ADAS1000_LOFFLTH		0x0D	/* Leads off Lower Threshold */
#define	ADAS1000_PACEEDGETH	0x0E	/* Pace Edge Threshold */
#define	ADAS1000_PACELVLTH	0x0F	/* Pace Level Threshold */
#define	ADAS1000_LADATA		0x11	/* LA or LEAD I Data */
#define	ADAS1000_LLDATA		0x12	/* LL or LEAD II Data */
#define	ADAS1000_RADATA		0x13	/* RA or LEAD III Data */
#define	ADAS1000_V1DATA		0x14	/* V1 or V1 Data */
#define	ADAS1000_V2DATA		0x15	/* V2 or V2 Data */
#define	ADAS1000_PACEDATA		0x1A	/* Read Pace Detection Data */
#define	ADAS1000_RESPMAG		0x1B	/* Read Respiration Data Magnitude */
#define	ADAS1000_RESPPH		0x1C	/* Read Respiration Data Phase */
#define	ADAS1000_LOFF			0x1D	/* Leads Off Status */
#define	ADAS1000_DCLEADSOFF	0x1E	/* DC Leads off Register */
#define	ADAS1000_EXTENDSW		0x20	/* Extended Switch for respiration inputs */
#define	ADAS1000_CALLA			0x21	/* User gain calibration LA */
#define	ADAS1000_CALLL			0x22	/* User gain calibration LL */
#define	ADAS1000_CALRA			0x23	/* User gain calibration RA */
#define	ADAS1000_CALV1			0x24	/* User gain calibration V1 */
#define	ADAS1000_CALV2			0x25	/* User gain calibration V2 */
#define	ADAS1000_LOAMLA		0x31	/* Leads off Amplitude for LA */
#define	ADAS1000_LOAMLL		0x32	/* Leads off Amplitude for LL */
#define	ADAS1000_LOAMRA		0x33	/* Leads off Amplitude for RA */
#define	ADAS1000_LOAMV1		0x34	/* Leads off Amplitude for V1 */
#define	ADAS1000_LOAMV2		0x35	/* Leads off Amplitude for V2 */
#define	ADAS1000_PACE1_DATA	0x3A	/* Pace1 Width & Amplitude2 */
#define	ADAS1000_PACE2_DATA	0x3B	/* Pace2 Width & Amplitude2 */
#define	ADAS1000_PACE3_DATA	0x3C	/* Pace3 Width & Amplitude2 */
#define	ADAS1000_FRAMES		0x40	/* Frame Header - Read Data Frames */
#define	ADAS1000_CRC			0x41	/* Frame CRC */

/******************************************************************************/
/* ECG Setting Register */
/******************************************************************************/
/* ECG Channel Enable, shuts down power to the channel, the input is
   now HiZ : 1 = enabled, 0 = disabled */
#define ADAS1000_ECGCTL_LAEN			         (1ul << 23)
/* ECG Channel Enable, shuts down power to the channel, the input is
   now HiZ : 1 = enabled, 0 = disabled */
#define ADAS1000_ECGCTL_LLEN			         (1ul << 22)
/* ECG Channel Enable, shuts down power to the channel, the input is
   now HiZ : 1 = enabled, 0 = disabled */
#define ADAS1000_ECGCTL_RAEN			         (1ul << 21)
/* ECG Channel Enable, shuts down power to the channel, the input is
   now HiZ : 1 = enabled, 0 = disabled */
#define ADAS1000_ECGCTL_V1EN			         (1ul << 20)
/* ECG Channel Enable, shuts down power to the channel, the input is
   now HiZ : 1 = enabled, 0 = disabled */
#define ADAS1000_ECGCTL_V2EN			         (1ul << 19)
/* Setting this bit selects the differential AFE input:
   0 = Single Ended Input Digital Lead Mode or Electrode Mode,
   1 = Differential Input Analog Lead Mode */
#define ADAS1000_ECGCTL_CHCONFIG		         (1ul << 10)
/* Pre-Amp & Anti-Aliasing Filter Overall Gain:
   00 = GAIN 0 = x1.4, 01 = GAIN 1 = x2.1,
   10 = GAIN 2 = x2.8, 11 = GAIN 3 = x4.2 */
#define ADAS1000_ECGCTL_GAIN 			         (1ul << 8)
/* VREF Buffer Enable:   0 = Disabled, 1 = Enabled (when using internal
   VREF, the VREFBUF must be enabled) */
#define ADAS1000_ECGCTL_VREFBUF 		         (1ul << 7)
/* Use external clock instead of crystal oscillator. The crystal oscillator
   is automatically disabled if configured as SLAVE in Gang mode and the
   Slave device should receive the CLK from the Master device:
   0 = XTAL is CLK source, 1 = CLK_IO is CLK source. */
#define ADAS1000_ECGCTL_CLKEXT		         (1ul << 6)
/* In gang mode, this bit selects the master (SYNC_GANG pin is configured
   as an output). When in Single Channel Mode (GANG = 0), this bit is ignored:
   0 = Slave, 1 = Master  */
#define ADAS1000_ECGCTL_MASTER		         (1ul << 5)
/* Enable gang mode. Setting this bit causes the CLK_IO and to be activated:
   0 = Single Channel mode,  1 = Gang Mode */
#define ADAS1000_ECGCTL_GANG			         (1ul << 4)
/* Selects the noise/power performance, this bit controls the ADC sampling
   frequency. See specifications for further details:
   0 = 1MSPS - low power,  1 = 2 MSPS - High performance/low noise */
#define ADAS1000_ECGCTL_HP			            (1ul << 3)
/* Convert Enable - Setting this bit enables the ADC conversion and filters:
   0 = Idle, 1 = Conversion Enable */
#define ADAS1000_ECGCTL_CNVEN			         (1ul << 2)
/* Power Enable - clearing this bit powers down the device. All analog blocks
   are powered down and the external crystal is disabled:
   0 = Power Down,  1 = Power Enable */
#define ADAS1000_ECGCTL_PWREN			         (1ul << 1)
/* Software Reset - setting this bit clears all registers to their reset value.
   This bit automatically clears itself. The software reset requires a NOP
   command to complete the reset: 0 = NOP,  1 = Reset */

#define ADAS1000_ECGCTL_SWRST			         (1ul << 0)
/******************************************************************************/
/* Leads off Control Register */
/******************************************************************************/
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_LAPH 		         (1ul << 23)
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_LLPH 		         (1ul << 22)
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_RAPH			         (1ul << 21)
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_V1PH			         (1ul << 20)
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_V2PH			         (1ul << 19)
/* AC Leads Off Phase: 0 = in phase, 1 = 180deg out of phase */
#define ADAS1000_LOFFCTL_CEPH			         (1ul << 18)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_LAACLOEN	         (1ul << 17)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_LLACLOEN	         (1ul << 16)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_RAACLOEN	         (1ul << 15)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_V1ACLOEN	         (1ul << 14)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_V2ACLOEN	         (1ul << 13)
/* Individual electrode AC Leads off enable. AC Leads off enables are
   the OR of ACSEL and the individual AC Leads off Channel enables.
   0 = AC Leads off disabled, 1 = AC Leads off enabled */
#define ADAS1000_LOFFCTL_CEACLOEN	         (1ul << 12)
/* Set Current level for AC leads off (only active for ACSEL = 1).
   00 = 12.5nA rms, 01  =  25nA rms,
   10  =  50nA rms, 11  =  100nA rms */
#define ADAS1000_LOFFCTL_ACCURREN	         (1ul << 7)
/* Set Current level for DC leads off (only active for ACSEL = 0)
   000 = 0nA, 001  =  10nA, 010  =  20nA, 011  =  30nA,
   100  =  40nA, 101  =  50nA, 110  =  60nA, 111  =  70nA */
#define ADAS1000_LOFFCTL_DCCURRENT	         (1ul << 2)
/* DC or AC (out of band) Leads Off Detection. If LOFFEN = 0, this bit
   is don't care. If LOFFEN = 1, 0 = DC Leads Off Detection enabled.
   (Individual AC leads off may be enabled through bits 12-17),
   1  = DC Leads off Detection disabled. AC Leads Off Detection enabled
  (all electrodes except CE electrode). */
#define ADAS1000_LOFFCTL_ACSEL 		         (1ul << 1)
/* Enable Leads Off Detection:
   0 = Leads Off Disabled, 1 = Leads Off Enabled */
#define ADAS1000_LOFFCTL_LOFFEN		         (1ul << 0)

/******************************************************************************/
/* Respiration Control Register */
/******************************************************************************/
/* Set to one to enable the MSB of the respiration DAC to be driven
   out onto the GPIO[3] pin. This 64kHz signal can be used to
   synchronize an external generator to the respiration carrier.
   0 = normal GPIO3 function, 1 = MSB of RESPDAC driven onto GPIO[3] */
#define ADAS1000_RESPCTL_RESPEXTSYNC		   (1ul << 15)
/* For use with external instrumentation amplifier with respiration
   circuit. Bypasses the on chip amplifier stage and input directly
   to the ADC. */
#define ADAS1000_RESPCTL_RESPEXTAMP		      (1ul << 14)
/* Selects external respiration drive output. RESPDAC_RA is
   automatically selected when RESPCAP = 1, 0 = RESPDAC _LL, 1 = RESPDAC_LA */
#define ADAS1000_RESPCTL_RESPOUT		         (1ul << 13)
/* Selects source of Respiration Capacitors.
   0 = Use internal capacitors, 1 = Use external capacitors */
#define ADAS1000_RESPCTL_RESPCAP		         (1ul << 12)
/* Respiration Inamp Gain (saturates at 10):
   0000 = x1 gain, 0001 = x2 gain, 0010 = x3 gain,
   ...
   1000 = x9 gain, 1001 = x10 gain, 11xx = x10 gain */
#define ADAS1000_RESPCTL_RESPGAIN		      (1ul << 8)
/* Selects between EXT_RESP _LA or EXT_RESP_LL paths. Only applies
   if External Respiration is selected in RESPSEL. EXT_RESP_RA
   automatically gets enabled. 0 = EXT_RESP_LL, 1 = EXT_RESP _LA */
#define ADAS1000_RESPCTL_RESPEXTSEL 		   (1ul << 7)
/* Set Leads for Respiration Measurement:
   00 = Lead I, 01 = Lead II,
   10 = Lead III, 11 = External Respiration path */
#define ADAS1000_RESPCTL_RESPSEL		         (1ul << 5)
/* Set the test tone amplitude for respiration:
   00 = Amplitude/8, 01  = Amplitude/4,
   10  = Amplitude/2, 11  = Amplitude */
#define ADAS1000_RESPCTL_RESPAMP    		   (1ul << 3)
/* Set Frequency for Respiration:
   00 = 56kHz, 01 = 54kHz, 10 = 52kHz, 11 = 50kHz */
#define ADAS1000_RESPCTL_RESPFREQ	   	   (1ul << 1)
/* Enable Respiration:
   0 = Respiration Disabled, 1  = Respiration Enabled */
#define ADAS1000_RESPCTL_RESPEN			      (1ul << 0)

#define ADAS1000_RESPCTL_RESPGAIN_MASK		   (0x0000000Ful << 8)
#define ADAS1000_RESPCTL_RESPSEL_MASK		   (0x00000003ul << 5)

/******************************************************************************/
/* Pace Detection Control Register */
/******************************************************************************/
/* Pace width Filter:
   0 = Filter Disabled, 1 = Filter Enabled */
#define ADAS1000_PACECTL_PACEFILTW		      (1ul << 11)
/* Pace validation filter 2:
   0 = Filter Disabled, 1 = Filter Enabled */
#define ADAS1000_PACECTL_PACETFILT2		      (1ul << 10)
/* Pace validation filter 1:
   0 = Filter Disabled, 1 = Filter Enabled */
#define ADAS1000_PACECTL_PACETFILT1		      (1ul << 9)
/* Set Lead for Pace Detection Measurement:
   00 = Lead I, 01 = Lead II, 10 = Lead III, 11 = Lead aVF */
#define ADAS1000_PACECTL_PACE3SEL 		      (1ul << 7)
/* Set Lead for Pace Detection Measurement:
   00 = Lead I, 01 = Lead II, 10 = Lead III, 11 = Lead aVF */
#define ADAS1000_PACECTL_PACE2SEL 		      (1ul << 5)
/* Set Lead for Pace Detection Measurement:
   00 = Lead I, 01 = Lead II, 10 = Lead III, 11 = Lead aVF */
#define ADAS1000_PACECTL_PACE1SEL 		      (1ul << 3)
/* Enable Pace Detection Algorithm:
   0 = Pace Detection Disabled, 1  = Pace Detection Enabled */
#define ADAS1000_PACECTL_PACE3EN		         (1ul << 2)
/* Enable Pace Detection Algorithm:
   0 = Pace Detection Disabled, 1  = Pace Detection Enabled */
#define ADAS1000_PACECTL_PACE2EN		         (1ul << 1)
/* Enable Pace Detection Algorithm:
   0 = Pace Detection Disabled, 1  = Pace Detection Enabled */
#define ADAS1000_PACECTL_PACE1EN		         (1ul << 0)
#define ADAS1000_PACECTL_PACE3SEL_MASK		   (0x00000003ul << 7)
#define ADAS1000_PACECTL_PACE2SEL_MASK		   (0x00000003ul << 5)
#define ADAS1000_PACECTL_PACE1SEL_MASK		   (0x00000003ul << 3)

/******************************************************************************/
/* Common Mode Reference and Shield Drive Control Register */
/******************************************************************************/
/* Common Mode Electrode Select */
#define ADAS1000_CMREFCTL_LACM			      (1ul << 23)
/* Any combination of the 5 input electrodes can be used to create the  */
#define ADAS1000_CMREFCTL_LLCM		         (1ul << 22)
/* Common Mode signal, or the Common Mode signal can be driven from the */
#define ADAS1000_CMREFCTL_RACM		   	   (1ul << 21)
/* internal reference. Bits 23:19 are ignored when bit 2 is selected.   */
#define ADAS1000_CMREFCTL_V1CM			      (1ul << 20)
/* The Common Mode is the average of the selected electrodes. When a 	 */
/* single electrode is selected, the Common Mode is the signal level of */
/* that electrode alone. */
/* 0 = does not contribute to the common mode */
/* 1 = contributes to the common mode */
#define ADAS1000_CMREFCTL_V2CM			      (1ul << 19)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1  = contributes to RLD input */
#define ADAS1000_CMREFCTL_LARLD			      (1ul << 14)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1   = contributes to RLD input */
#define ADAS1000_CMREFCTL_LLRLD			      (1ul << 13)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1 = contributes to RLD input */
#define ADAS1000_CMREFCTL_RARLD			      (1ul << 12)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1   = contributes to RLD input */
#define ADAS1000_CMREFCTL_V1RLD			      (1ul << 11)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1 = contributes to RLD input */
#define ADAS1000_CMREFCTL_V2RLD			      (1ul << 10)
/* RLD Summing Junction
   0 = does not contribute to RLD input
   1 = contributes to RLD input */
#define ADAS1000_CMREFCTL_CERLD			      (1ul << 9)
/* Common Electrode Reference
   0 = Common Electrode disabled
   1 = Common Electrode enabled */
#define ADAS1000_CMREFCTL_CEREFEN		      (1ul << 8)
/* Select electrode for reference drive
   0000 = RL, 0001 = LA, 0010 = LL,
   0011 = RA, 0100 = V1, 0101 = V2,
   0110 to 1111 = Reserved */
#define ADAS1000_CMREFCTL_RLDSEL		         (1ul << 4)
/* Common mode output - when set, the internally derived common mode
   signal is driven out the common mode pin. This bit has no effect
   if external common mode is selected.
   0 = common mode is not driven out
   1   = common mode is driven out the external common mode pin */
#define ADAS1000_CMREFCTL_DRVCM			      (1ul << 3)
/* Select the source of Common Mode
   (use when operating multiple devices together)
   0 = Internal Common Mode selected
   1 = External Common Mode selected */
#define ADAS1000_CMREFCTL_EXTCM			      (1ul << 2)
/* Enable Right Leg Drive Reference Electrode
   0 = Disabled
   1  = Enabled */
#define ADAS1000_CMREFCTL_RLD_EN		         (1ul << 1)
/* Enable Shield Drive
   0 = Shield Drive Disabled
   1  = Shield Drive Enabled */
#define ADAS1000_CMREFCTL_SHLDEN             (1ul << 0)

#define ADAS1000_CMREFCTL_RLDSEL_MASK 		   (0x0000000Ful << 4)

/******************************************************************************/
/* GPIO Control Register */
/******************************************************************************/
/* Frame secondary SPI words with chip select
   0 = MCS asserted for entire frame
   1 = MCS asserted for individual word */
#define ADAS1000_GPIOCTL_SPIFW			      (1ul << 18)
/* Secondary SPI Enable (ADAS1000 and ADAS1000-2 only) (SPI interface
   providing ECG data at 128kHz data rate for external digital pace
   algorithm detection � uses GPIO0, GPIO1, GPIO2 pins)
   0 = Disabled
   1 = Enabled. The individual control bits for GPIO0, GPIO1,
   GPIO2 are ignored. GPIO3 is not affected by SPIEN */
#define ADAS1000_GPIOCTL_SPIEN			      (1ul << 16)
/* State of GPIO<3>
   00 = High Impedance, 01 = Input,
   10 = Output, 11 = Open Drain */
#define ADAS1000_GPIOCTL_G3CTL			      (1ul << 14)
/* Output Value to be written to GPIO<3> when pad is configured as an
   output or open drain
   0  = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G3OUT			      (1ul << 13)
/* (Read Only) Input Value read from GPIO<3> when pad is configured as an input
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G3IN		      	   (1ul << 12)
/* State of GPIO<2>
   00 = High Impedance, 01 = Input,
   10 = Output, 11 = Open Drain */
#define ADAS1000_GPIOCTL_G2CTL 			      (1ul << 10)
/* Output Value to be written to GPIO<2> when pad is configured as an
   output or open drain
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G2OUT			      (1ul << 9)
/* (Read Only) Input Value read from GPIO<2> when pad is configured as an input.
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G2IN			         (1ul << 8)
/* State of GPIO<1>
   00 = High Impedance, 01 = Input,
   10 = Output, 11 = Open Drain */
#define ADAS1000_GPIOCTL_G1CTL			      (1ul << 6)
/* Output Value to be written to GPIO<1> when pad is configured as an
   output or open drain.
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G1OUT			      (1ul << 5)
/* (Read Only) Input Value read from GPIO<1> when pad is configured as an input.
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G1IN			         (1ul << 4)
/* State of GPIO<0>
   00 = High Impedance, 01 = Input,
   10 = Output, 11 = Open Drain */
#define ADAS1000_GPIOCTL_G0CTL			      (1ul << 2)
/* Output Value to be written to GPIO<0> when pad is configured as an
   output or open drain.
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G0OUT			      (1ul << 1)
/* (Read Only) Input Value read from GPIO<0> when pad is configured
   as an input
   0 = Low Value
   1 = High Value */
#define ADAS1000_GPIOCTL_G0IN			         (1ul << 0)
#define ADAS1000_GPIOCTL_G3CTL_MASK		      (0x00000003ul << 14)
#define ADAS1000_GPIOCTL_G2CTL_MASK		      (0x00000003ul << 10)
#define ADAS1000_GPIOCTL_G1CTL_MASK		      (0x00000003ul << 6)
#define ADAS1000_GPIOCTL_G0CTL_MASK		      (0x00000003ul << 2)

/******************************************************************************/
/* Pace Amplitude Threshold2 Register */
/******************************************************************************/
/* Pace Amplitude Thresold			*/
#define ADAS1000_PACEAMPTH_PACE3AMPTH		   (1ul << 16)
/* Threshold = N - VREF/GAIN/216 */
#define ADAS1000_PACEAMPTH_PACE2AMPTH		      (1ul << 8)
#define ADAS1000_PACEAMPTH_PACE1AMPTH		      (1ul << 0)

#define ADAS1000_PACEAMPTH_PACE3AMPTH_MASK      (0x000000FFul << 16)
#define ADAS1000_PACEAMPTH_PACE2AMPTH_MASK	   (0x000000FFul << 8)
#define ADAS1000_PACEAMPTH_PACE1AMPTH_MASK	   (0x000000FFul << 0)

/******************************************************************************/
/* Test Tone Register */
/******************************************************************************/
/* Tone Select */
#define ADAS1000_TESTTONE_TONLA			         (1ul << 23)
/* 0 = 1.3V VCM_REF */
#define ADAS1000_TESTTONE_TONLL			         (1ul << 22)
/* 1 = 1mV sinewave or squarewave for toneint, no connect for tonext */
#define ADAS1000_TESTTONE_TONRA			         (1ul << 21)
#define ADAS1000_TESTTONE_TONV1			         (1ul << 20)
#define ADAS1000_TESTTONE_TONV2			         (1ul << 19)
/* 00 = 10Hz Sine Wave
   01 = 150Hz Sine Wave
   1x = 1Hz 1mV Square Wave */
#define ADAS1000_TESTTONE_TONTYPE		         (1ul << 3)
/* Test Tone Internal or External
   0 = External Test Tone
   1 = Internal Test Tone */
#define ADAS1000_TESTTONE_TONINT		            (1ul << 2)
/* Test Tone out Enable
   0 = disconnects test tone from CAL_DAC_IO during internal mode only
   1 = Connects CAL_DAC_IO to test tone during internal mode. */
#define ADAS1000_TESTTONE_TONOUT		            (1ul << 1)
/* Enables an internal test tone to drive entire signal chain, from
   pre-amp to SPI interface. This tone comes from the CAL DAC and goes
   to the pre-amps through the internal mux. When TONEN (CALDAC) is
   enabled, AC Leads off is disabled.
   0 = Disable the test tone
   1 = Enable the CALDAC 1mV SineWave test tone (Cal Mode has priority) */
#define ADAS1000_TESTTONE_TONEN			         (1ul << 0)

#define ADAS1000_TESTTONE_TONTYPE_MASK		      (0x00000003ul << 3)

/******************************************************************************/
/* Calibration DAC Register */
/******************************************************************************/
/* Calibration Chop Clock Enable. The CALDAC output can be chopped to
   lower 1/f noise. Chopping is done at 256kHz.
   0 = Disabled
   1  = Enabled. */
#define ADAS1000_CALDAC_CALCHPEN		            (1ul << 13)
/* Calibration Mode Enable
   0 = Disable Calibration mode
   1 = Enable Calibration mode - connect CAL DAC_IO,
   begin data acquisition on ECG channels. */
#define ADAS1000_CALDAC_CALMODEEN		         (1ul << 12)
/* Calibration Internal or External
   0 = External Cal - calibration to be performed externally by
   looping CAL_DAC_IO around into ECG channels.
   1 = Internal Cal - disconnects external switches for all ECG
   channels and connects CALDAC internally to all ECG channels. */
#define ADAS1000_CALDAC_CALINT			         (1ul << 11)
/* Enable 10-bit calibration DAC for cal mode or external use.
   0 = Disable CALDAC
   1 = Enable CALDAC, if a master device and not in calibration
   mode then also connects CAL_DAC out to its_IO pin for
   external use, if in Slave mode, the CALDAC will disable to
   allow master to drive CAL_DAC_IO pin. When CALDAC is enabled,
   AC Leads off is disabled. */
#define ADAS1000_CALDAC_CALDACEN		            (1ul << 10)
/* Set the CAL DAC value */
#define ADAS1000_CALDAC_CALDATA			         (1ul << 0)

#define ADAS1000_CALDAC_CALDATA_MASK		      (0x000003FFul << 0)

/******************************************************************************/
/* Frame Control Register */
/******************************************************************************/
/* Include/Exclude word from ECG data frame, if electrode/lead is
   included in the data word and the electrode falls off, then the
   data word will be undefined.
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_LEAD_I_LADIS		      (1ul << 23)
#define ADAS1000_FRMCTL_LEAD_II_LLDIS		      (1ul << 22)
#define ADAS1000_FRMCTL_LEAD_III_RADIS		      (1ul << 21)
#define ADAS1000_FRMCTL_V1DIS			            (1ul << 20)
#define ADAS1000_FRMCTL_V2DIS			            (1ul << 19)
/* Include/Exclude word from ECG data frame
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_PACEDIS			         (1ul << 14)
/* Respiration Magnitude
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_RESPMDIS		            (1ul << 13)
/* Respiration Phase
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_RESPPHDIS		         (1ul << 12)
/* Leads Off Status
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_LOFFDIS			         (1ul << 11)
/* GPIO Word disable
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_GPIODIS			         (1ul << 10)
/* CRC  Word disable
   0 = Included in Frame
   1 = Exclude from Frame */
#define ADAS1000_FRMCTL_CRCDIS			         (1ul << 9)
/* In a master device configured for Lead Mode, the ECG data will
   be signed. When in slave mode (electrode format), the ECG data
   format is unsigned. Use this bit when using multiple devices to
   make the slave device signed data.
   0 = unsigned data (default)
   1 = signed data */
#define ADAS1000_FRMCTL_SIGNEDEN		            (1ul << 8)
/* Automatically disable PACE, RESP, LOFF words if their flags are
   not set in the header.
   0 = fixed frame format
   1 = auto disable words */
#define ADAS1000_FRMCTL_ADIS			            (1ul << 7)
/* Ready Repeat � if this bit is set and the frame header indicates
   data is not ready, the frame header is continuously sent until
   data is ready.
   0 = always send entire frame
   1 = repeat frame header until ready */
#define ADAS1000_FRMCTL_RDYRPT			         (1ul << 6)
/* Sets the Output Data Format
   0 = Lead/Vector Format
   (only available in 2kHz & 16kHz data rates)
   1  = Electrode Format */
#define ADAS1000_FRMCTL_DATAFMT			         (1ul << 4)
/* Skip interval - this field provides a way to decimate the data
   00 = output every frame
   01 = output every other frame
   1x = output every 4th frame */
#define ADAS1000_FRMCTL_SKIP			            (1ul << 2)
/* Sets the Output Data Rate to 2 kHz */
#define ADAS1000_FRMCTL_FRMRATE_2KHZ		      0x00
/* Sets the Output Data Rate to 16 kHz */
#define ADAS1000_FRMCTL_FRMRATE_16KHZ		      0x01
/* Sets the Output Data Rate to 128 kHz */
#define ADAS1000_FRMCTL_FRMRATE_128KHZ		      0x10
/* Sets the Output Data Rate to 31.25 Hz */
#define ADAS1000_FRMCTL_FRMRATE_31_25HZ		   0x11

#define ADAS1000_FRMCTL_WORD_MASK		         (ADAS1000_FRMCTL_LEAD_I_LADIS 	| \
						                              ADAS1000_FRMCTL_LEAD_II_LLDIS 	| \
						                              ADAS1000_FRMCTL_LEAD_III_RADIS  | \
						                              ADAS1000_FRMCTL_V1DIS 		| \
						                              ADAS1000_FRMCTL_V2DIS 		| \
						                              ADAS1000_FRMCTL_PACEDIS 	| \
						                              ADAS1000_FRMCTL_RESPMDIS 	| \
						                              ADAS1000_FRMCTL_RESPPHDIS	| \
						                              ADAS1000_FRMCTL_LOFFDIS 	| \
					                           	   ADAS1000_FRMCTL_GPIODIS 	| \
						                              ADAS1000_FRMCTL_CRCDIS)
#define ADAS1000_FRMCTL_SKIP_MASK		         (0x00000003ul << 2)
#define ADAS1000_FRMCTL_FRMRATE_MASK		      (0x00000003ul << 0)

/******************************************************************************/
/* Filter Control Register */
/******************************************************************************/
/* 2kHz notch bypass for SPI Master
   0 = notch filter bypassed
   1 = notch filter present */
#define ADAS1000_FILTCTL_MN2K			            (1ul << 5)
/* 2kHz notch bypass
   0 = notch filter present
   1 = notch filter bypassed */
#define ADAS1000_FILTCTL_N2KBP			         (1ul << 4)
/* 00 = 40Hz
   01 = 150Hz
   10 = 250 Hz
   11 = 450Hz */
#define ADAS1000_FILTCTL_LPF			            (1ul << 2)

#define ADAS1000_FILTCTL_LPF_MASK		         (0x00000003ul << 2)

/******************************************************************************/
/* Leads off Upper Threshold Register */
/******************************************************************************/
/* ADC over range threshold. An ADC out-of-range error will be flagged
   if the ADC output is greater than the over range threshold.
   The over range threshold is offset from the maximum value.
   Threshold = max_value � ADCOVER*2^6
   0000 = max value (disabled)
   0001 = max_value - 64
   0010 = max_value - 128
   ...
   1111: max_value - 960 */
#define ADAS1000_LOFFUTH_ADCOVER		            (1ul << 16)
/* AC Leads off upper Threshold. Leads off will be detected if the DC
   or AC output is = N * 2 * VREF/GAIN/2^16. 0 = 0V */
#define ADAS1000_LOFFUTH_LOFFUTH		            (1ul << 0)

#define ADAS1000_LOFFUTH_ADCOVER_MASK		      (0x0000000Ful << 16)
#define ADAS1000_LOFFUTH_LOFFUTH_MASK		      (0x0000FFFFul << 0)

/******************************************************************************/
/* Leads off Lower Threshold Register */
/******************************************************************************/
/* ADC under range threshold. An ADC out-of-range error will be flagged
   if the ADC output is less than the under range threshold.
   Threshold = min_value + ADCUNDR�2^6
   0000 = min value (disabled)
   0001 = min_value + 64
   0010 = min _value + 128
   ...
   1111: min _value + 960 */
#define ADAS1000_LOFFLTH_ADCUNDR		            (1ul << 16)
/* AC Leads off lower Threshold. Leads off will be detected if the DC
   or AC output is = N * 2 * VREF/GAIN/2^16. 0 = 0V */
#define ADAS1000_LOFFLTH_LOFFLTH		            (1ul << 0)

#define ADAS1000_LOFFLTH_ADCUNDR_MASK		      (0x0000000Ful << 16)
#define ADAS1000_LOFFLTH_LOFFLTH_MASK		      (0x0000FFFFul << 0)

/******************************************************************************/
/* Pace Edge Threshold Register	*/
/******************************************************************************/
/* Pace edge trigger threshold */
#define ADAS1000_PACEEDGETH_PACE3EDGTH		      (1ul << 16)
/* 0 = PACEAMPTH/2 */
#define ADAS1000_PACEEDGETH_PACE2EDGTH		      (1ul << 8)
/* 1 = VREF/GAIN/2^16		*/
#define ADAS1000_PACEEDGETH_PACE1EDGTH		      (1ul << 0)
/* N = N * VREF/GAIN/2^16	*/

#define ADAS1000_PACEEDGETH_PACE3EDGTH_MASK	   (0x000000FFul << 16)
#define ADAS1000_PACEEDGETH_PACE2EDGTH_MASK	   (0x000000FFul << 8)
#define ADAS1000_PACEEDGETH_PACE1EDGTH_MASK	   (0x000000FFul << 0)

/******************************************************************************/
/* Pace Level Threshold Register */
/******************************************************************************/
/* Pace level threshold. This is a signed value. */
#define ADAS1000_PACELVLTH_PACE3LVLTH		      (1ul << 16)
/* -1 = 0xFFF = -VREF/GAIN/2^16	 */
#define ADAS1000_PACELVLTH_PACE2LVLTH		      (1ul << 8)
/* 0 = 0x0000 = 0V */
#define ADAS1000_PACELVLTH_PACE1LVLTH		      (1ul << 0)
/* +1 = 0x001 = +VREF/GAIN/2^16 */
/* N = N * VREF/GAIN/2^16 */

#define ADAS1000_PACELVLTH_PACE3LVLTH_MASK	   (0x000000FFul << 16)
#define ADAS1000_PACELVLTH_PACE2LVLTH_MASK	   (0x000000FFul << 8)
#define ADAS1000_PACELVLTH_PACE1LVLTH_MASK	   (0x000000FFul << 0)

/***********************************************************************************/
/* LA or LEAD I, LL or LEAD II, RA or LEAD III, V1 or V1�, V2 or V2� Data Register */
/***********************************************************************************/
/* 0x11 : LA or LEAD I
   0x12 : LL or LEAD II
   0x13 : RA or LEAD II
   0x14 : V1 or V1
   0x15 : V2 or V2 */
#define ADAS1000_LADATA_ADDRESS		            (1ul << 24)
/* Channel Data Value. Data left justified (MSB) irrespective of data
   rate. In electrode format, the value is an unsigned integer.
   In Vector format, the value is a signed 2�s complement integer format.
   Vector format had 2x range compared to electrode format since it can
   swing from +VREF to -VREF, therefore the LSB size is double.
   Electrode Format:
	Min value (000...) = 0V
	Max value (1111...) = VREF/GAIN
	LSB = (VREF/GAIN)/2N
	Lead/Vector Format
	Min value (1000...) = -(VREF/GAIN)
	Max value (0111...) = +VREF/GAIN
	LSB = 2^(VREF/GAIN)/2N
	Where N = # of data bits, 16 for 128kHz data rate or 24 for
	2kHz/16kHz data rate. */

#define ADAS1000_LADATA_ECG_DATA                (1ul << 0)
#define ADAS1000_LADATA_ADDRESS_MASK		      (0x000000FFul << 24)
#define ADAS1000_LADATA_ECG_DATA_MASK		      (0x00FFFFFFul << 0)

/******************************************************************************/
/* Read Pace Detection Data Register */
/******************************************************************************/
/* 0001 1010 = Pace Detection */
#define ADAS1000_PACEDATA_ADDRESS		         (1ul << 24)
/* Pace 3 detected. This bit will be set once a pace pulse is
   detected. This bit is set on the trailing edge of the pace pulse.
   0 = Pace pulse not detected in current frame
   1 = Pace pulse detected in this frame */
#define ADAS1000_PACEDATA_PACE3_DETECTED	      (1ul << 23)
/* This is the log2(height) of the pace pulse
   N: height = 2^N * VREF / GAIN / 2^16 */
#define ADAS1000_PACEDATA_PACE_CH3_HEIGHT	      (1ul << 16)
/* This is log2(Width)-1 of the pace pulse.
   N: Width = 2^(N+1) / 128kHz */
#define ADAS1000_PACEDATA_PACE_CH3_WIDTH  	   (1ul << 20)
/* Pace 2 detected. This bit will be set once a pace pulse is
   detected. This bit is set on the trailing edge of the pace pulse.
   0 = Pace pulse not detected in current frame
   1 = Pace pulse detected in this frame*/
#define ADAS1000_PACEDATA_PACE2_DETECTED	      (1ul << 15)
/* This is log2(Width)-1 of the pace pulse.
   N: Width = 2^(N+1) / 128kHz */
#define ADAS1000_PACEDATA_PACE_CH2_WIDTH  	   (1ul << 12)
/* This is the log2(height) of the pace pulse
   N: height = 2^N * VREF / GAIN / 2^16 */
#define ADAS1000_PACEDATA_PACE_CH2_HEIGHT	      (1ul << 8)
/* Pace 1 detected. This bit will be set once a pace pulse is
   detected. This bit is set on the trailing edge of the pace pulse.
   0 = Pace pulse not detected in current frame
   1 = Pace pulse detected in this frame */
#define ADAS1000_PACEDATA_PACE1_DETECTED	      (1ul << 7)
/* "This is log2(Width)-1 of the pace pulse.
   N: Width = 2^(N+1) / 128kHz */
#define ADAS1000_PACEDATA_PACE_CH1_WIDTH	      (1ul << 4)
/* This is the log2(height) of the pace pulse
   N: height = 2^N * VREF / GAIN / 2^16 */
#define ADAS1000_PACEDATA_CH1_HEIGHT		      (1ul << 0)

#define ADAS1000_PACEDATA_ADDRESS_MASK		      (0x000000FFul << 24)
#define ADAS1000_PACEDATA_PACE_CH3_WIDTH_MASK	(0x00000007ul << 20)
#define ADAS1000_PACEDATA_PACE_CH3_HEIGHT_MASK	(0x0000000Ful << 16)
#define ADAS1000_PACEDATA_PACE_CH2_WIDTH_MASK	(0x00000007ul << 12)
#define ADAS1000_PACEDATA_PACE_CH2_HEIGHT_MASK	(0x0000000Ful << 8)
#define ADAS1000_PACEDATA_PACE_CH1_WIDTH_MASK	(0x00000007ul << 4)
#define ADAS1000_PACEDATA_PACE_CH1_HEIGHT_MASK	(0x0000000Ful << 0)

/******************************************************************************/
/* Read Respiration Data Magnitude Register */
/******************************************************************************/
/* 0001 1011 = Respiration Magnitude */
#define ADAS1000_RESPMAG_ADDRESS		            (1ul << 24)
/* Magnitude of respiration signal. This is an unsigned value. */
#define ADAS1000_RESPMAG_MAGNITUDE		         (1ul << 0)

#define ADAS1000_RESPMAG_ADDRESS_MASK		      (0x000000FFul << 24)
#define ADAS1000_RESPMAG_MAGNITUDE_MASK		   (0x00FFFFFFul << 0)

/******************************************************************************/
/* Read Respiration Data Phase Register	*/
/******************************************************************************/
/* 0001 1100 = Respiration Phase */
#define ADAS1000_RESPPH_ADDRESS			         (1ul << 24)
/* Phase of respiration signal. Can be interpreted as either signed or
   unsigned value. If unsigned, the range is from 0 to 2pi. If as a
   signed value, the range is from �pi to +pi.
   0x000000 = 0
   0x000001 = 2pi / 2^24
   0x400000 = pi/2
   0x800000 = +pi = -pi
   0xC00000 = +3pi/2 = -pi/2
   0xFFFFFF = +2pi(1 - 2^(-24)) = -2p / 2^24 */
#define ADAS1000_RESPPH_PHASE			            (1ul << 0)

#define ADAS1000_RESPPH_ADDRESS_MASK		      (0x000000FFul << 24)
#define ADAS1000_RESPPH_PHASE_MASK		         (0x00FFFFFFul << 0)

/******************************************************************************/
/* Leads Off Status Register */
/******************************************************************************/
/* Address bits define the word data 0001 1101 = Leads Off */
#define ADAS1000_LOFF_ADDRESS			            (1ul << 24)
/* Electrode Connection Status. If either DC or AC leads off
   If both DC and AC leads off are enabled, these bits reflect
   only the AC leads off status. DC leads off is available in
   the DCLEADSOFF register. The common electrodes only have DC
   leads off detection. An AC leads off signal can be injected
   into the common electrode, but there is no ADC input to measure
   its amplitude. If the common electrode is off, it will affect
   the AC leads off amplitude of the other electrodes. These bits
   accumulate in the frame buffer and are cleared when the frame
   buffer is loaded into the SPI buffer.
   0 = Electrode is connected
   1 = Electrode is disconnected*/
#define ADAS1000_LOFF_RL_LEADS_OFF_STATUS       (1ul << 23)
#define ADAS1000_LOFF_LA_LEADS_OFF_STATUS    	(1ul << 22)
#define ADAS1000_LOFF_LL_LEADS_OFF_STATUS    	(1ul << 21)
#define ADAS1000_LOFF_RA_LEADS_OFF_STATUS    	(1ul << 20)
#define ADAS1000_LOFF_V1_LEADS_OFF_STATUS    	(1ul << 19)
#define ADAS1000_LOFF_V2_LEADS_OFF_STATUS    	(1ul << 18)
#define ADAS1000_LOFF_CELO			               (1ul << 13)
/* ADC out of range error.
   These status bits indicate the resulting ADC code is out of
   range. These bits accumulate in the frame buffer and are
   cleared when the frame buffer is loaded into the SPI buffer. */
#define ADAS1000_LOFF_LAADCOR			            (1ul << 12)
#define ADAS1000_LOFF_LLADCOR			            (1ul << 11)
#define ADAS1000_LOFF_RAADCOR			            (1ul << 10)
#define ADAS1000_LOFF_V1ADCOR			            (1ul << 9)
#define ADAS1000_LOFF_V2ADCOR			            (1ul << 8)

#define ADAS1000_LOFF_ADDRESS_MASK		         (0x000000FFul << 24)

/******************************************************************************/
/* DC Leads off Register */
/******************************************************************************/
/* Address bits define the word data 0001 1110 = DC Leads Off */
#define ADAS1000_DCLEADSOFF_ADDRESS		         (1ul << 24)
/* The DC leads off detection is comparator based and compares
   to a fixed level. Per electrode bits flag if the DC leads off
   comparator threshold level has been exceeded.
   0 = electrode < overrange threshold, 2.4 V
   1 = electrode > overrange threshold, 2.4 V */
#define ADAS1000_DCLEADSOFF_RL_INPUT_OVERRANGE	(1ul << 23)
#define ADAS1000_DCLEADSOFF_LA_INPUT_OVERRANGE	(1ul << 22)
#define ADAS1000_DCLEADSOFF_LL_INPUT_OVERRANGE	(1ul << 21)
#define ADAS1000_DCLEADSOFF_RA_INPUT_OVERRANGE	(1ul << 20)
#define ADAS1000_DCLEADSOFF_CE_INPUT_OVERRANGE	(1ul << 13)

/* The DC leads off detection is comparator based and compares
   to a fixed level. Per electrode bits flag if the DC leads off
   comparator threshold level has been exceeded.
   0 = electrode > underrange threshold, 0.2 V
   1 = electrode < underrange threshold, 0.2 V */
#define ADAS1000_DCLEADSOFF_RL_INPUT_UNDERRANGE	(1ul << 12)
#define ADAS1000_DCLEADSOFF_LA_INPUT_UNDERRANGE	(1ul << 11)
#define ADAS1000_DCLEADSOFF_LL_INPUT_UNDERRANGE	(1ul << 10)
#define ADAS1000_DCLEADSOFF_RA_INPUT_UNDERRANGE	(1ul << 9)
#define ADAS1000_DCLEADSOFF_CE_INPUT_UNDERRANGE	(1ul << 2)

#define ADAS1000_DCLEADSOFF_ADDRESS_MASK	      (0x000000FFul << 24)

/******************************************************************************/
/* Extended Switch for Respiration Inputs Register */
/******************************************************************************/
/* External Respiration electrode input switch to channel
   electrode input.
   0 = switch open
   1 = switch closed */
#define ADAS1000_EXTENDSW_EXTRESP_RA_LA		   (1ul << 23)
#define ADAS1000_EXTENDSW_EXTRESP_RA_LL		   (1ul << 22)
#define ADAS1000_EXTENDSW_EXTRESP_RA_RA		   (1ul << 21)
#define ADAS1000_EXTENDSW_EXTRESP_RA_V1		   (1ul << 20)
#define ADAS1000_EXTENDSW_EXTRESP_RA_V2		   (1ul << 19)
#define ADAS1000_EXTENDSW_EXTRESP_LL_LA		   (1ul << 18)
#define ADAS1000_EXTENDSW_EXTRESP_LL_LL		   (1ul << 17)
#define ADAS1000_EXTENDSW_EXTRESP_LL_RA		   (1ul << 16)
#define ADAS1000_EXTENDSW_EXTRESP_LL_V1		   (1ul << 15)
#define ADAS1000_EXTENDSW_EXTRESP_LL_V2		   (1ul << 14)
#define ADAS1000_EXTENDSW_EXTRESP_LA_LA		   (1ul << 13)
#define ADAS1000_EXTENDSW_EXTRESP_LA_LL		   (1ul << 12)
#define ADAS1000_EXTENDSW_EXTRESP_LA_RA		   (1ul << 11)
#define ADAS1000_EXTENDSW_EXTRESP_LA_V1		   (1ul << 10)
#define ADAS1000_EXTENDSW_EXTRESP_LA_V2		   (1ul << 9)

/* V1 and V2 electrodes may be used for measurement purposes
   other than ECG. To achieve this, they need to be disconnected
   from the patient VCM voltage provided from the internal common
   mode buffer and instead connected to the internal VCM_REF level
   of 1.3V. Set FREE_Vx bits high to connect negative input of V1
   channel will be tied to internal VCM_REF level. This allows user
   to make alternative measurements on V1 channel relative to the
   VCM_REF level. If using Digital lead mode, uses these bits in
   conjunction with NO_MATH_Vx bits [6:5]. */
#define ADAS1000_EXTENDSW_FREE_V1		         (1ul << 8)
#define ADAS1000_EXTENDSW_FREE_V2		         (1ul << 7)
/* In Digital Lead Mode, the digital core calculates the math on V1
   and V2 with respect to WCT (LA+LL+RA)/3 providing V1 and V2.
   Where V1 or V2 are used for measurement of something other than
   ECG, then the math calculation needs to be disabled. These bits
   are most likely used in conjunction with bits FREE_Vx [8:7].
   Set NOMATH_Vx bits high to disable the math calculation in V1
   and V2 respectively. */
#define ADAS1000_EXTENDSW_NOMATH_V1		         (1ul << 6)
#define ADAS1000_EXTENDSW_NOMATH_V2		         (1ul << 5)

/******************************************************************************/
/* User gain calibration LA, LL, RA, V1, V2	Register						  */
/******************************************************************************/
/* 0x21 : CAL LA */
/* 0x22 : CAL LL */
/* 0x23 : CAL RA */
/* 0x24 : CAL V1 */
/* 0x25 : CAL V2 */
#define	ADAS1000_CAL_ADDRESS			            (1ul << 24)
/* User can choose between:
   0 = default calibration values
   1 = user calibration values */
#define	ADAS1000_CAL_USRCAL			            (1ul << 23)
/* Gain Calibration value.
   Result = data * (1 + GAIN * 2^(-17))
   The value read from this register is the current gain calibration value.
   If the USRCAL bit is clear, this register returns the default value for
   the current gain setting.
   0x7FF (+2047) = *1.00000011111111111b
   0x001 (+1) 	  = *1.00000000000000001b
   0x000 (0)     = *1.00000000000000000b
   0xFFF (-1)    = *0.11111111111111111b
   0x800 (-2048) = *0.11111100000000000b */
#define	ADAS1000_CAL_CALVALUE			         (1ul << 0)

#define	ADAS1000_CAL_ADDRESS_MASK		         (0x000000FFul << 24)
#define	ADAS1000_CAL_CALVALUE_MASK		         (0x00000FFFul << 0)

/******************************************************************************/
/* Leads off Amplitude for LA, LL, RA, V1, V2 Register */
/******************************************************************************/
/* 0x31 : LA AC Leads off Magnitude
   0x32 : LL AC Leads off Magnitude
   0x33 : RA AC Leads off Magnitude
   0x34 : V1 AC Leads off Magnitude
   0x35 : V2 AC Leads off Magnitude */
#define	ADAS1000_LOAM_ADDRESS			         (1ul << 24)
/* Measured Amplitude.
   When AC leads off is selected, the data is the average of the rectified
   2kHz bandpass filter with an update rate of 8Hz and cutoff frequency at
   2Hz. The output is the amplitude of the 2kHz signal scaled by 2/pi
   approximately = 0.6 (average of rectified sine wave). To convert to RMS,
   scale the output by pi / (2*sqrt(2)).
   Leads off (unsigned):
	Min 0x0000 = 0V
	LSB 0x0001= VREF / GAIN / 2^16
	Max 0xFFFF = VREF / GAIN */
#define	ADAS1000_LOAM_LOFFAM			            (1ul << 0)

#define	ADAS1000_LOAM_ADDRESS_MASK		         (0x000000FFul << 24)
#define	ADAS1000_LOAM_LOFFAM_MASK		         (0x0000FFFFul << 0)

/******************************************************************************/
/* Pace1, Pace2, Pace3 Width & Amplitude2 Register */
/******************************************************************************/
/* 0x3A : PACE1DATA
   0x3B : PACE2DATA
   0x3C : PACE3DATA */
#define	ADAS1000_PACE_DATA_ADDRESS		         (1ul << 24)
/* Measured pace height in signed 2�s complement value
   0 = 0
   1 = VREF / GAIN / 2^16
   N = N * VREF / GAIN / 2^16 */
#define	ADAS1000_PACE_DATA_HEIGHT		         (1ul << 8)
/* Measured pace width in 128kHz samples
   N:   N / 128kHz   = width
   12:  12 / 128kHz  = 93us
   255: 255 / 128kHz = 2.0ms */
#define	ADAS1000_PACE_DATA_WIDTH		         (1ul << 0)

#define	ADAS1000_PACE_DATA_ADDRESS_MASK		   (0x000000FFul << 24)
#define	ADAS1000_PACE_DATA_HEIGHT_MASK		   (0x0000FFFFul << 8)
#define	ADAS1000_PACE_DATA_WIDTH_MASK		      (0x000000FFul << 0)

/******************************************************************************/
/* Frame Header - Read Data Frames Register */
/******************************************************************************/
/* Header marker, set to 1 for the header */
#define ADAS1000_FRAMES_MARKER			         (1ul << 31)
/* Ready bit indicates if ECG frame data is calculated and
   ready for reading.
   0 = Ready, data frame follows
   1 = Busy */
#define ADAS1000_FRAMES_READY_BIT		         (1ul << 30)
/* Overflow bits indicate that since the last frame read,
   a number of frames have been missed. This field saturates
   at the maximum count. The data in the frame including
   this header word is valid but old if the overflow bits
   are > 0. When using Skip mode (FRMCTL register (0x0A)[3:2]),
   the Overflow bit acts as a flag, where a non-zero value
   indicates an overflow.
   00 = 0 missed
   01 = 1 frame missed
   10 = 2 frames missed
   11 = 3 or more frames missed */
#define ADAS1000_FRAMES_OVERFLOW		            (1ul << 28)
/* Internal device error detected.
   0 = normal operation
   1 = error condition	*/
#define ADAS1000_FRAMES_FAULT			            (1ul << 27)
/* PACE 3 Indicates Pacing Artifact was qualified at most
   recent point.
   0 = No Pacing Artifact
   1 = Pacing Artifact Present	*/
#define ADAS1000_FRAMES_PACE3_DETECTED		      (1ul << 26)
/* PACE 2 Indicates Pacing Artifact was qualified at most
   recent point.
   0 = No Pacing Artifact
   1 = Pacing Artifact Present	*/
#define ADAS1000_FRAMES_PACE2_DETECTED		      (1ul << 25)
/* PACE 1 Indicates Pacing Artifact was qualified at most
   recent point.
   0 = No Pacing Artifact
   1 = Pacing Artifact Present	*/
#define ADAS1000_FRAMES_PACE1_DETECTED		      (1ul << 24)
/* 0 = no new respiration data
   1 = respiration data updated */
#define ADAS1000_FRAMES_RESPIRATION		         (1ul << 23)
/* If both DC & AC leads off are enabled, this bit is the
   OR of all the AC leads off detect flags. If only AC or
   DC leads off is enabled (but not both, this bit reflects
   the OR of all DC & AC leads off flags.
   0 = all leads connected
   1 = one or more leads off detected */
#define ADAS1000_FRAMES_LEADS_OFF_DETECTED      (1ul << 22)
/* 0 = all leads connected
   1 = one or more leads off detected */
#define ADAS1000_FRAMES_DC_LEADS_OFF_DETECTED	(1ul << 21)
/* 0 = ADC within range
   1 = ADC out of range */
#define ADAS1000_FRAMES_ADC_OUT_OF_RANGE	      (1ul << 20)
/******************************************************************************/
/* Frame CRC Register */
/******************************************************************************/
/* Cyclic Redundancy Check */
#define ADAS1000_CRC_MASK                       (0x00FFFFFF << 0)

/******************************************************************************/
/* ADAS1000 data rates, word sizes and frame size */
/******************************************************************************/
#define ADAS1000_31_25HZ_FRAME_RATE		         3125
#define ADAS1000_2KHZ_FRAME_RATE		            2000
#define ADAS1000_16KHZ_FRAME_RATE		         16000
#define ADAS1000_128KHZ_FRAME_RATE		         128000

#define ADAS1000_31_25HZ_WORD_SIZE		         32
#define ADAS1000_2KHZ_WORD_SIZE			         32
#define ADAS1000_16KHZ_WORD_SIZE		            32
#define ADAS1000_128KHZ_WORD_SIZE		         16

#define ADAS1000_31_25HZ_FRAME_SIZE 		      12
#define ADAS1000_2KHZ_FRAME_SIZE		            12
#define ADAS1000_16KHZ_FRAME_SIZE		         12
#define ADAS1000_128KHZ_FRAME_SIZE		         15

/******************************************************************************/
/* ADAS1000 CRC constants */
/******************************************************************************/
#define CRC_POLY_2KHZ_16KHZ			            0x005D6DCBul
#define CRC_CHECK_CONST_2KHZ_16KHZ		         0x0015A0BAul

#define CRC_POLY_128KHZ				               0x00001021ul
#define CRC_CHECK_CONST_128KHz			         0x00001D0Ful

struct adas1000_dev {
	/** SPI Descriptor */
	struct no_os_spi_desc *spi_desc;
	/** ADAS1000 frame size in bytes */
	uint32_t frame_size;
	/** ADAS1000 frame rate */
	uint32_t frame_rate;
	/** Number of inactive words in a frame */
	uint32_t inactive_words_no;
};

struct adas1000_init_param {
	/** SPI Initialization Parameters */
	struct no_os_spi_init_param spi_init;
	/** ADAS1000 frame rate */
	uint32_t frame_rate;
};

struct read_param {
	/** Set to true if a the frames read sequence must be started. */
	bool start_read;
	/** Set to true if a the frames read sequence must be sopped
	    when exiting the function. */
	bool stop_read;
	/** Set to true if the function must wait for the READY bit
		 to be set in the header. */
	bool wait_for_ready;
	/** Set to 1 if the device was configured to repeat the
	    header until the READY bit is set. */
	bool ready_repeat;
};


/******************************************************************************/
/* Functions Prototypes */
/******************************************************************************/
/** Compute SPI frequency based on frame rate */
int32_t adas1000_compute_spi_freq(struct adas1000_init_param *init_param,
				  uint32_t *spi_freq);

/* Initializes the communication with ADAS1000 and checks if the device is present.*/
int32_t adas1000_init(struct adas1000_dev **device,
		      const struct adas1000_init_param *init_param);

/* Reads the value of a ADAS1000 register */
int32_t adas1000_read(struct adas1000_dev *device, uint8_t reg_addr,
		      uint32_t *reg_data);

/* Writes a value into a ADAS1000 register */
int32_t adas1000_write(struct adas1000_dev *device, uint8_t reg_addr,
		       uint32_t reg_data);

/* Performs a software reset of the ADAS1000 */
int32_t adas1000_soft_reset(struct adas1000_dev *device);

/* Compute frame size. */
int32_t adas1000_compute_frame_size(struct adas1000_dev *device);

/* Selects which words are not included in a data frame */
int32_t adas1000_set_inactive_framewords(struct adas1000_dev *device,
		uint32_t words_mask);

/* Sets the frame rate */
int32_t adas1000_set_frame_rate(struct adas1000_dev *device, uint32_t rate);

/* Reads the specified number of frames */
int32_t adas1000_read_data(struct adas1000_dev *device, uint8_t *data_buff,
			   uint32_t frame_cnt, struct read_param *read_data_param);

/* Computes the CRC for a frame */
uint32_t adas1000_compute_frame_crc(struct adas1000_dev * device,
				    uint8_t *buff);

#endif /* _ADAS1000_H_ */
