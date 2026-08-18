/***************************************************************************//**
 *   @file   ade9039.h
 *   @brief  Header file of ADE9039 Driver.
 *   @author REtz (radu.etz@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __ADE9039_H__
#define __ADE9039_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_print_log.h"

/* SPI commands */
#define ADE9039_SPI_READ		NO_OS_BIT(3)

#define ENABLE                  	0x0001
#define DISABLE                 	0x0000

/* ADE9039 Register Map */
#define ADE9039_REG_AIGAIN           	0x000 /* Phase A current gain adjust. */
#define ADE9039_REG_AIGAIN0          	0x001 /* Phase A multipoint gain correction factor. If multipoint gain and phase enabled, with MTEN = 1 in the CONFIG0 register,
						an factor, AIGAIN0 through AIGAIN4, is applied based on the RMS amplitude, and the MTTHR_Lx and MTTHR_Hx register */
#define ADE9039_REG_AIGAIN1         	0x002 /* See the AIGAIN0 description. */
#define ADE9039_REG_AIGAIN2         	0x003 /* See the AIGAIN0 description. */
#define ADE9039_REG_AIGAIN3         	0x004 /* See the AIGAIN0 description. */
#define ADE9039_REG_AIGAIN4         	0x005 /* See the AIGAIN0 description. */
#define ADE9039_REG_APHCAL0         	0x006 /* Phase A multipoint phase correction factor. If multipoint phase and gain disabled, with MTEN = 0 in the CONFIG0 register,
						the compensation is applied. If multipoint phase and gain enabled, with MTEN = 1, the APHCAL0 through APHCAL4 applied based
						on the AIRMS current RMS amplitude, and the MTTHR_Hx register values. */
#define ADE9039_REG_APHCAL1          	0x007 /* See the APHCAL0 description. */
#define ADE9039_REG_APHCAL2          	0x008 /* See the APHCAL0 description. */
#define ADE9039_REG_APHCAL3          	0x009 /* See the APHCAL0 description. */
#define ADE9039_REG_APHCAL4          	0x00A /* See the APHCAL0 description. */
#define ADE9039_REG_AVGAIN           	0x00B /* Phase A voltage gain adjust. */
#define ADE9039_REG_AIRMSOS          	0x00C /* Phase A current RMS offset for the filter-based AIRMS calculation. */
#define ADE9039_REG_AVRMSOS          	0x00D /* Phase A voltage RMS offset for the filter-based AVRMS calculation. */
#define ADE9039_REG_APGAIN          	0x00E /* Phase A power gain adjust for the AWATT, AVA, AFWATT, AFVA, and */
#define ADE9039_REG_AWATTOS         	0x00F /* Phase A total active power offset correction for the AWATT calculation. */
#define ADE9039_REG_AFWATTOS        	0x011 /* Phase A fundamental active power offset correction for the AFWATT */
#define ADE9039_REG_AFVAROS          	0x012 /* Phase A fundamental reactive power offset correction for the AFVAR */
#define ADE9039_REG_AIFRMSOS         	0x013 /* Phase A current RMS offset for the fundamental current RMS, AIFRMS */
#define ADE9039_REG_AVFRMSOS         	0x014 /* Phase A voltage RMS offset for the fundamental voltage RMS, AVFRMS */
#define ADE9039_REG_AVRMSONEOS       	0x015 /* Phase A voltage RMS offset for the fast AVRMSONE calculation. */
#define ADE9039_REG_AIRMSONEOS       	0x016 /* Phase A current RMS offset for the fast AIRMSONE calculation. */
#define ADE9039_REG_AECCOEF          	0x019 /* Phase A coupling coefficient used to compensate for the electrostatic the Analog Devices PCB-based di/dt sensor. Format is 5.27. */
#define ADE9039_REG_BIGAIN           	0x020 /* Phase B current gain adjust. */
#define ADE9039_REG_BIGAIN0          	0x021 /* Phase B multipoint gain correction factor. If multipoint gain and phase enabled, with MTEN = 1 in the CONFIG0 register, an factor,
						BIGAIN0 through BIGAIN4, is applied based on the RMS amplitude, and the MTTHR_Lx and MTTHR_Hx register */
#define ADE9039_REG_BIGAIN1          	0x022 /* See the BIGAIN0 description. */
#define ADE9039_REG_BIGAIN2          	0x023 /* See the BIGAIN0 description. */
#define ADE9039_REG_BIGAIN3          	0x024 /* See the BIGAIN0 description. */
#define ADE9039_REG_BIGAIN4          	0x025 /* See the BIGAIN0 description. */
#define ADE9039_REG_BPHCAL0          	0x026 /* Phase B multipoint phase correction factor. If multipoint phase and gain disabled, with MTEN = 0 in the CONFIG0 register, the */
#define ADE9039_REG_BPHCAL1          	0x027 /* See the BPHCAL0 description. */
#define ADE9039_REG_BPHCAL2          	0x028 /* See the BPHCAL0 description. */
#define ADE9039_REG_BPHCAL3          	0x029 /* See the BPHCAL0 description. */
#define ADE9039_REG_BPHCAL4          	0x02A /* See the BPHCAL0 description. */
#define ADE9039_REG_BVGAIN           	0x02B /* Phase B voltage gain adjust. */
#define ADE9039_REG_BIRMSOS          	0x02C /* Phase B current RMS offset for the BIRMS calculation. */
#define ADE9039_REG_BVRMSOS          	0x02D /* Phase B voltage RMS offset for the BVRMS calculation. */
#define ADE9039_REG_BPGAIN           	0x02E /* Phase B power gain adjust for the BWATT, BVA, BFWATT, BFVA, and */
#define ADE9039_REG_BWATTOS          	0x02F /* Phase B total active power offset correction for the BWATT calculation. */
#define ADE9039_REG_BFWATTOS         	0x031 /* Phase B fundamental active power offset correction for the BFWATT */
#define ADE9039_REG_BFVAROS          	0x032 /* Phase B fundamental reactive power offset correction for the BFVAR */
#define ADE9039_REG_BIFRMSOS         	0x033 /* Phase B current RMS offset for the fundamental current RMS, BIFRMS */
#define ADE9039_REG_BVFRMSOS         	0x034 /* Phase B voltage RMS offset for the fundamental voltage RMS, BVFRMS */
#define ADE9039_REG_BVRMSONEOS       	0x035 /* Phase B voltage RMS offset for the fast BVRMSONE calculation. */
#define ADE9039_REG_BIRMSONEOS       	0x036 /* Phase B current RMS offset for the fast BIRMSONE calculation. */
#define ADE9039_REG_BECCOEF          	0x039 /* Phase B coupling coefficient used to compensate for the electrostatic the Analog Devices PCB-based di/dt sensor. Format is 5.27. */
#define ADE9039_REG_CIGAIN           	0x040 /* Phase C current gain adjust. */
#define ADE9039_REG_CIGAIN0          	0x041 /* Phase C multipoint gain correction factor. If multipoint gain and phase enabled, with MTEN = 1 in the CONFIG0 register, an factor,
						CIGAIN0 through CIGAIN4, is applied based on the RMS amplitude, and the MTTHR_Lx and MTTHR_Hx register */
#define ADE9039_REG_CIGAIN1          	0x042 /* See the CIGAIN0 description. */
#define ADE9039_REG_CIGAIN2          	0x043 /* See the CIGAIN0 description. */
#define ADE9039_REG_CIGAIN3          	0x044 /* See the CIGAIN0 description. */
#define ADE9039_REG_CIGAIN4          	0x045 /* See the CIGAIN0 description. */
#define ADE9039_REG_CPHCAL0          	0x046 /* Phase C multipoint phase correction factor. If multipoint phase and gain disabled, with MTEN = 0 in the CONFIG0 register, the
						compensation is applied. If multipoint phase and gain enabled, with MTEN = 1, the CPHCAL0 through CPHCAL4 applied, based on the
						CIRMS current RMS amplitude, and the MTTHR_Hx register values. */
#define ADE9039_REG_CPHCAL1          	0x047 /* See the CPHCAL0 description. */
#define ADE9039_REG_CPHCAL2          	0x048 /* See the CPHCAL0 description. */
#define ADE9039_REG_CPHCAL3          	0x049 /* See the CPHCAL0 description. */
#define ADE9039_REG_CPHCAL4          	0x04A /* See the CPHCAL0 description. */
#define ADE9039_REG_CVGAIN           	0x04B /* Phase C voltage gain adjust. */
#define ADE9039_REG_CIRMSOS          	0x04C /* Phase C current RMS offset for the CIRMS calculation. */
#define ADE9039_REG_CVRMSOS          	0x04D /* Phase C voltage RMS offset for the CVRMS calculation. */
#define ADE9039_REG_CPGAIN           	0x04E /* Phase C power gain adjust for the CWATT, CVA, CFWATT, CFVA, and */
#define ADE9039_REG_CWATTOS          	0x04F /* Phase C total active power offset correction for the CWATT calculation. */
#define ADE9039_REG_CFWATTOS         	0x051 /* Phase C fundamental active power offset correction for the CFWATT */
#define ADE9039_REG_CFVAROS          	0x052 /* Phase C fundamental reactive power offset correction for the CFVAR */
#define ADE9039_REG_CIFRMSOS         	0x053 /* Phase C current RMS offset for the fundamental current RMS, CIFRMS */
#define ADE9039_REG_CVFRMSOS         	0x054 /* Phase C voltage RMS offset for the fundamental voltage RMS, CVFRMS */
#define ADE9039_REG_CVRMSONEOS       	0x055 /* Phase C voltage RMS offset for the fast CVRMSONE calculation. */
#define ADE9039_REG_CIRMSONEOS       	0x056 /* Phase C current RMS offset for the fast CIRMSONE calculation. */
#define ADE9039_REG_CECCOEF          	0x059 /* Phase C coupling coefficient used to compensate for the electrostatic the Analog Devices PCB-based di/dt sensor. Format is 5.27. */
#define ADE9039_REG_CONFIG0          	0x060 /* Configuration Register 0. DSP measurements configuration register. */
#define ADE9039_REG_MTTHR_L0         	0x061 /* Multipoint phase/gain threshold. If MTEN = 1 in the CONFIG0 register, the MTTHR_Hx registers set up the ranges in which to apply of
						corrections, allowing hysteresis. */
#define ADE9039_REG_MTTHR_L1         	0x062 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_L2         	0x063 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_L3         	0x064 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_L4         	0x065 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_H0         	0x066 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_H1         	0x067 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_H2         	0x068 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_H3         	0x069 /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_MTTHR_H4         	0x06A /* Multipoint phase/gain threshold. See MTTHR_L0 for more information. */
#define ADE9039_REG_NIRMSOS          	0x06B /* Neutral current RMS offset for the NIRMS calculation. */
#define ADE9039_REG_ISUMRMSOS        	0x06C /* Offset correction for the ISUMRMS calculation based on the sum of IA + IB + IN. */
#define ADE9039_REG_NIGAIN           	0x06D /* Neutral current gain adjust. */
#define ADE9039_REG_NPHCAL           	0x06E /* Neutral current phase compensation. */
#define ADE9039_REG_NIRMSONEOS       	0x06F /* Neutral current RMS offset for the fast RMS½ NIRMSONE calculation. */
#define ADE9039_REG_VNOM             	0x071 /* Nominal phase voltage RMS used in the computation of apparent power, the VNOMx_EN bit is set in the CONFIG0 register. */
#define ADE9039_REG_DICOEFF          	0x072 /* Value used in the digital integrator algorithm. If the integrator is turned INTEN or ININTEN equal to one in the CONFIG0 register,
						 it is set this value to 0x0. */
#define ADE9039_REG_ISUMLVL          	0x073 /* Threshold to compare ISUMRMS against. Configure this register to receive indication in the STATUS0 register if ISUMRMS exceeds this */
#define ADE9039_REG_AI_PCF           	0x20A /* Instantaneous Phase A current channel waveform processed by the DSP at */
#define ADE9039_REG_AV_PCF           	0x20B /* Instantaneous Phase A voltage channel waveform processed by the DSP at */
#define ADE9039_REG_AIRMS            	0x20C /* Phase A filter-based current RMS value, updated at 4 kSPS. */
#define ADE9039_REG_AVRMS            	0x20D /* Phase A filter-based voltage RMS value, updated at 4 kSPS. */
#define ADE9039_REG_AIFRMS           	0x20E /* Phase A current fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_AVFRMS           	0x20F /* Phase A voltage fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_AWATT            	0x210 /* Phase A low-pass filtered total active power, updated at 4 kSPS. */
#define ADE9039_REG_AVA              	0x212 /* Phase A total apparent power, updated at 4 kSPS. */
#define ADE9039_REG_AFWATT           	0x213 /* Phase A fundamental active power, updated at 4 kSPS. */
#define ADE9039_REG_AFVAR            	0x214 /* Phase A fundamental reactive power, updated at 4 kSPS. */
#define ADE9039_REG_AFVA             	0x215 /* Phase A fundamental apparent power, updated at 4 kSPS. */
#define ADE9039_REG_APF              	0x216 /* Phase A total power factor, updated every 1.024 sec. */
#define ADE9039_REG_AVTHD            	0x217 /* Phase A voltage THD, updated every 1.024 sec. */
#define ADE9039_REG_AITHD            	0x218 /* Phase A current THD, updated every 1.024 sec. */
#define ADE9039_REG_AIRMSONE         	0x219 /* Phase A current fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_AVRMSONE         	0x21A /* Phase A voltage fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_AMTREGION        	0x21D /* If multipoint gain and phase compensation is enabled, with MTEN = 1 in the this register indicates which AIGAINx and APHCALx is used. */
#define ADE9039_REG_AFPF             	0x21E /* Phase A fundamental power factor, updated every 1.024 sec. */
#define ADE9039_REG_AQUADINFO        	0x21F /* Phase A power quadrant measurement information. */
#define ADE9039_REG_BI_PCF           	0x22A /* Instantaneous Phase B current channel waveform processed by the DSP at */
#define ADE9039_REG_BV_PCF           	0x22B /* Instantaneous Phase B voltage channel waveform processed by the DSP at */
#define ADE9039_REG_BIRMS            	0x22C /* Phase B filter-based current RMS value, updated at 4 kSPS. */
#define ADE9039_REG_BVRMS            	0x22D /* Phase B filter-based voltage RMS value, updated at 4 kSPS. */
#define ADE9039_REG_BIFRMS           	0x22E /* Phase B current fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_BVFRMS           	0x22F /* Phase B voltage fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_BWATT            	0x230 /* Phase B low-pass filtered total active power, updated at 4 kSPS. */
#define ADE9039_REG_BVA              	0x232 /* Phase B total apparent power, updated at 4 kSPS. */
#define ADE9039_REG_BFWATT           	0x233 /* Phase B fundamental active power, updated at 4 kSPS. */
#define ADE9039_REG_BFVAR            	0x234 /* Phase B fundamental reactive power, updated at 4 kSPS. */
#define ADE9039_REG_BFVA             	0x235 /* Phase B fundamental apparent power, updated at 4 kSPS. */
#define ADE9039_REG_BPF              	0x236 /* Phase B total power factor, updated every 1.024 sec. */
#define ADE9039_REG_BVTHD            	0x237 /* Phase B voltage THD, updated every 1.024 sec. */
#define ADE9039_REG_BITHD            	0x238 /* Phase B current THD, updated every 1.024 sec. */
#define ADE9039_REG_BIRMSONE         	0x239 /* Phase B current fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_BVRMSONE         	0x23A /* Phase B voltage fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_BMTREGION        	0x23D /* If multipoint gain and phase compensation is enabled, with MTEN = 1 in the this register indicates which BIGAINx and BPHCALx is used. */
#define ADE9039_REG_BFPF             	0x23E /* Phase B fundamental power factor, updated every 1.024 sec. */
#define ADE9039_REG_BQUADINFO        	0x23F /* Phase B power quadrant measurement information. */
#define ADE9039_REG_CI_PCF           	0x24A /* Instantaneous Phase C current channel waveform processed by the DSP at */
#define ADE9039_REG_CV_PCF           	0x24B /* Instantaneous Phase C voltage channel waveform processed by the DSP at */
#define ADE9039_REG_CIRMS            	0x24C /* Phase C filter-based current RMS value, updated at 4 kSPS. */
#define ADE9039_REG_CVRMS            	0x24D /* Phase C filter-based voltage RMS value, updated at 4 kSPS. */
#define ADE9039_REG_CIFRMS           	0x24E /* Phase C current fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_CVFRMS           	0x24F /* Phase C voltage fundamental RMS, updated at 4 kSPS. */
#define ADE9039_REG_CWATT            	0x250 /* Phase C low-pass filtered total active power, updated at 4 kSPS. */
#define ADE9039_REG_CVA              	0x252 /* Phase C total apparent power, updated at 4 kSPS. */
#define ADE9039_REG_CFWATT           	0x253 /* Phase C fundamental active power, updated at 4 kSPS. */
#define ADE9039_REG_CFVAR            	0x254 /* Phase C fundamental reactive power, updated at 4 kSPS. */
#define ADE9039_REG_CFVA             	0x255 /* Phase C fundamental apparent power, updated at 4 kSPS. */
#define ADE9039_REG_CPF              	0x256 /* Phase C total power factor, updated every 1.024 sec. */
#define ADE9039_REG_CVTHD            	0x257 /* Phase C voltage THD, updated every 1.024 sec. */
#define ADE9039_REG_CITHD            	0x258 /* Phase C current total THD, updated every 1.024 sec. */
#define ADE9039_REG_CIRMSONE         	0x259 /* Phase C current fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_CVRMSONE         	0x25A /* Phase C voltage fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_CMTREGION        	0x25D /* If multipoint gain and phase compensation is enabled, with MTEN = 1 in register, these bits indicate which CIGAINx and CPHCALx is used. */
#define ADE9039_REG_CFPF             	0x25E /* Phase C fundamental power factor, updated every 1.024 sec. */
#define ADE9039_REG_CQUADINFO        	0x25F /* Phase C quadrant measurement information. */
#define ADE9039_REG_NI_PCF          	0x265 /* Instantaneous neutral current channel waveform processed by the DSP at 4 */
#define ADE9039_REG_NIRMS            	0x266 /* Neutral current filter-based RMS value. */
#define ADE9039_REG_NIRMSONE         	0x267 /* Neutral current fast RMS½ calculation, one cycle RMS updated every half */
#define ADE9039_REG_ISUMRMS          	0x269 /* Filter-based RMS based on the sum of IA + IB + IC ± IN. */
#define ADE9039_REG_VERSION2         	0x26A /* This register indicates the version of the metrology algorithms after the user = 1 to start the measurements. */
#define ADE9039_REG_RESAMPLE_RESET_A 	0x26B /* the waveform buffer when WF_CAP_EN bit is reset or disabled. */
#define ADE9039_REG_RESAMPLE_LAST_CY 	0x26C /* cycle index. A value between 0 to 7. This register contains the the last cycle resampled. */
#define ADE9039_REG_TWATTHR          	0x26D /* The sum of the total active energy of Phase A, Phase B, and Phase C when bits are enabled in the CONFIG0. */
#define ADE9039_REG_TFVARHR          	0x26E /* The sum of the fundamental reactive energy of Phase A, Phase B, and when the related bits are enabled in the CONFIG0. */
#define ADE9039_REG_AWATT_ACC        	0x2E5 /* Phase A accumulated total active power, updated after PWR_TIME 4 kSPS */
#define ADE9039_REG_AWATTHR_LO       	0x2E6 /* Phase A accumulated total active energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AWATTHR_HI       	0x2E7 /* Phase A accumulated total active energy, MSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AVA_ACC          	0x2F9 /* Phase A accumulated total apparent power, updated after PWR_TIME 4 */
#define ADE9039_REG_AVAHR_LO         	0x2FA /* Phase A accumulated total apparent energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AVAHR_HI         	0x2FB /* Phase A accumulated total apparent energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFWATT_ACC       	0x303 /* Phase A accumulated fundamental active power, updated after PWR_TIME 4 */
#define ADE9039_REG_AFWATTHR_LO      	0x304 /* Phase A accumulated fundamental active energy, LSB. Updated according to in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFWATTHR_HI      	0x305 /* Phase A accumulated fundamental active energy, MSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFVAR_ACC        	0x30D /* Phase A accumulated fundamental reactive power, updated after kSPS samples. */
#define ADE9039_REG_AFVARHR_LO       	0x30E /* Phase A accumulated fundamental reactive energy, LSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFVARHR_HI       	0x30F /* Phase A accumulated fundamental reactive energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFVA_ACC         	0x317 /* Phase A accumulated fundamental apparent power, updated after kSPS samples. */
#define ADE9039_REG_AFVAHR_LO        	0x318 /* Phase A accumulated fundamental apparent energy, LSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_AFVAHR_HI        	0x319 /* Phase A accumulated fundamental apparent energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BWATT_ACC        	0x321 /* Phase B accumulated total active power, updated after PWR_TIME 4 kSPS */
#define ADE9039_REG_BWATTHR_LO       	0x322 /* Phase B accumulated total active energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BWATTHR_HI       	0x323 /* Phase B accumulated total active energy, MSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BVA_ACC          	0x335 /* Phase B accumulated total apparent power, updated after PWR_TIME 4 */
#define ADE9039_REG_BVAHR_LO         	0x336 /* Phase B accumulated total apparent energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BVAHR_HI         	0x337 /* Phase B accumulated total apparent energy, MSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFWATT_ACC       	0x33F /* Phase B accumulated fundamental active power, updated after PWR_TIME 4 */
#define ADE9039_REG_BFWATTHR_LO      	0x340 /* Phase B accumulated fundamental active energy, LSB. Updated according to in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFWATTHR_HI      	0x341 /* Phase B accumulated fundamental active energy, MSB. Updated according settings in EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFVAR_ACC        	0x349 /* Phase B accumulated fundamental reactive power, updated after kSPS samples. */
#define ADE9039_REG_BFVARHR_LO       	0x34A /* Phase B accumulated fundamental reactive energy, LSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFVARHR_HI       	0x34B /* Phase B accumulated fundamental reactive energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFVA_ACC         	0x353 /* Phase B accumulated fundamental apparent power, updated after kSPS samples. */
#define ADE9039_REG_BFVAHR_LO        	0x354 /* Phase B accumulated fundamental apparent energy, LSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_BFVAHR_HI        	0x355 /* Phase B accumulated fundamental apparent energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CWATT_ACC        	0x35D /* Phase C accumulated total active power, updated after PWR_TIME 4 kSPS */
#define ADE9039_REG_CWATTHR_LO       	0x35E /* Phase C accumulated total active energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CWATTHR_HI       	0x35F /* Phase C accumulated total active energy, MSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CVA_ACC          	0x371 /* Phase C accumulated total apparent power, updated after PWR_TIME 4 */
#define ADE9039_REG_CVAHR_LO         	0x372 /* Phase C accumulated total apparent energy, LSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CVAHR_HI         	0x373 /* Phase C accumulated total apparent energy, MSB. Updated according to the the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFWATT_ACC       	0x37B /* Phase C accumulated fundamental active power, updated after PWR_TIME samples. */
#define ADE9039_REG_CFWATTHR_LO      	0x37C /* Phase C accumulated fundamental active energy, LSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFWATTHR_HI      	0x37D /* Phase C accumulated fundamental active energy, MSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFVAR_ACC        	0x385 /* Phase C accumulated fundamental reactive power, updated after kSPS samples. */
#define ADE9039_REG_CFVARHR_LO       	0x386 /* Phase C accumulated fundamental reactive energy, LSB. Updated according settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFVARHR_HI       	0x387 /* Phase C accumulated fundamental reactive energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFVA_ACC         	0x38F /* Phase C accumulated fundamental apparent power, updated after kSPS samples. */
#define ADE9039_REG_CFVAHR_LO        	0x390 /* Phase C accumulated fundamental apparent energy, LSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_CFVAHR_HI        	0x391 /* Phase C accumulated fundamental apparent energy, MSB. Updated the settings in the EP_CFG and EGY_TIME registers. */
#define ADE9039_REG_PWATT_ACC        	0x397 /* Accumulated positive total active power, MSB, from AWATT, BWATT, and updated after PWR_TIME 4 kSPS samples. */
#define ADE9039_REG_NWATT_ACC        	0x39B /* Accumulated Negative total active power, MSB, from AWATT, BWATT, and updated after PWR_TIME 4 kSPS samples. */
#define ADE9039_REG_IPEAK            	0x400 /* Current peak register. */
#define ADE9039_REG_VPEAK            	0x401 /* Voltage peak register. */
#define ADE9039_REG_STATUS0          	0x402 /* Status Register 0. */
#define ADE9039_REG_STATUS1          	0x403 /* Status Register 1. */
#define ADE9039_REG_EVENT_STATUS     	0x404 /* Event status register. */
#define ADE9039_REG_MASK0            	0x405 /* Interrupt Enable Register 0. */
#define ADE9039_REG_MASK1            	0x406 /* Interrupt Enable Register 1. */
#define ADE9039_REG_EVENT_MASK       	0x407 /* Event enable register. */
#define ADE9039_REG_OILVL            	0x409 /* Overcurrent detection threshold level. */
#define ADE9039_REG_OIA              	0x40A /* Phase A overcurrent RMS½ value. If a phase is enabled, with the OC_EN bit set in the CONFIG3 register and AIRMSONE greater
						 than the this value is updated. */
#define ADE9039_REG_OIB              	0x40B /* Phase B overcurrent RMS½ value. If a phase is enabled, with the OC_EN bit set in the CONFIG3 register and BIRMSONE greater
						 than the this value is updated. */
#define ADE9039_REG_OIC             	0x40C /* Phase C overcurrent RMS½ value. If a phase is enabled, with the OC_EN bit set in the CONFIG3 register and CIRMSONE greater
						 than the this value is updated. */
#define ADE9039_REG_OIN              	0x40D /* Neutral current overcurrent RMS½ value. If enabled, with the OC_EN neutral set in the CONFIG3 register and NIRMSONE greater
						 than the this value is updated. */
#define ADE9039_REG_USER_PERIOD      	0x40E /* User configured line period value used for resampling and fast RMS½ when bit in the CONFIG2 register is set. */
#define ADE9039_REG_VLEVEL           	0x40F /* Register used in the algorithm that computes the fundamental active, apparent powers as well as the fundamental IRMS and VRMS */
#define ADE9039_REG_DIP_LVL          	0x410 /* Voltage RMS½ dip detection threshold level. */
#define ADE9039_REG_DIPA             	0x411 /* Phase A voltage RMS½ value during a dip condition. */
#define ADE9039_REG_DIPB             	0x412 /* Phase B voltage RMS½ value during a dip condition. */
#define ADE9039_REG_DIPC             	0x413 /* Phase C voltage RMS½ value during a dip condition. */
#define ADE9039_REG_SWELL_LVL        	0x414 /* Voltage RMS½ swell detection threshold level. */
#define ADE9039_REG_SWELLA           	0x415 /* Phase A voltage RMS½ value during a swell condition. */
#define ADE9039_REG_SWELLB           	0x416 /* Phase B voltage RMS½ value during a swell condition. */
#define ADE9039_REG_SWELLC           	0x417 /* Phase C voltage RMS½ value during a swell condition. */
#define ADE9039_REG_APERIOD          	0x418 /* Line period on Phase A voltage. */
#define ADE9039_REG_BPERIOD          	0x419 /* Line period on Phase B voltage. */
#define ADE9039_REG_CPERIOD          	0x41A /* Line period on Phase C voltage. */
#define ADE9039_REG_COM_PERIOD       	0x41B /* Line period measurement on combined signal from Phase A, Phase B, and voltages. */
#define ADE9039_REG_ACT_NL_LVL       	0x41C /* No load threshold in the total and fundamental active power datapath. */
#define ADE9039_REG_REACT_NL_LVL     	0x41D /* No load threshold in the fundamental reactive power datapath. */
#define ADE9039_REG_APP_NL_LVL       	0x41E /* No load threshold in the total and fundamental apparent power datapath. */
#define ADE9039_REG_PHNOLOAD         	0x41F /* Phase no load register. */
#define ADE9039_REG_WTHR             	0x420 /* Sets the maximum output rate from the digital to frequency converter for the fundamental active power for the CFx calibration
						 pulse output. It is write WTHR = 0x0010_0000. */
#define ADE9039_REG_FVARTHR          	0x421 /* Sets the maximum output rate from the digital to frequency converter for reactive power for the CFx calibration pulse output.
						 It is write VARTHR = 0x0010_0000. */
#define ADE9039_REG_VATHR            	0x422 /* Sets the maximum output rate from the digital to frequency converter for the fundamental apparent power for the CFx calibration
						 pulse output. It to write VATHR = 0x0010_0000. */
#define ADE9039_REG_LAST_DATA_32     	0x423 /* This register holds the data read or written during the last 32-bit transaction SPI port. */
#define ADE9039_REG_ADC_REDIRECT     	0x424 /* This register allows any ADC output to be redirected to any digital datapath. */
#define ADE9039_REG_CF_LCFG          	0x425 /* CFx calibration pulse width configuration register. */
#define ADE9039_REG_PART_ID          	0x472 /* This register identifies the IC. If the ADE9039_ID bit = 1, after the run bit has the IC is the ADE9039. */
#define ADE9039_REG_TEMP_TRIM        	0x474 /* Temperature sensor gain and offset, calculated during the manufacturing */
#define ADE9039_REG_RUN              	0x480 /* This is the DSP run register. Write 0x1 to this register to start measurements. */
#define ADE9039_REG_CONFIG1          	0x481 /* Configuration Register 1. */
#define ADE9039_REG_ANGL_VA_VB       	0x482 /* Time between positive to negative zero crossings on Phase A and Phase B */
#define ADE9039_REG_ANGL_VB_VC       	0x483 /* Time between positive to negative zero crossings on Phase B and Phase C */
#define ADE9039_REG_ANGL_VA_VC       	0x484 /* Time between positive to negative zero crossings on Phase A and Phase C */
#define ADE9039_REG_ANGL_VA_IA       	0x485 /* Time between positive to negative zero crossings on Phase A voltage and */
#define ADE9039_REG_ANGL_VB_IB       	0x486 /* Time between positive to negative zero crossings on Phase B voltage and */
#define ADE9039_REG_ANGL_VC_IC       	0x487 /* Time between positive to negative zero crossings on Phase C voltage and */
#define ADE9039_REG_ANGL_IA_IB       	0x488 /* Time between positive to negative zero crossings on Phase A and Phase B */
#define ADE9039_REG_ANGL_IB_IC       	0x489 /* Time between positive to negative zero crossings on Phase B and Phase C */
#define ADE9039_REG_ANGL_IA_IC       	0x48A /* Time between positive to negative zero crossings on Phase A and Phase C */
#define ADE9039_REG_DIP_CYC          	0x48B /* Voltage RMS½ dip detection cycle configuration. */
#define ADE9039_REG_SWELL_CYC        	0x48C /* Voltage RMS½ swell detection cycle configuration. */
#define ADE9039_REG_OISTATUS         	0x48F /* Overcurrent status register. */
#define ADE9039_REG_CFMODE           	0x490 /* CFx configuration register. */
#define ADE9039_REG_COMPMODE         	0x491 /* Computation mode register. */
#define ADE9039_REG_ACCMODE          	0x492 /* Accumulation mode register. */
#define ADE9039_REG_CONFIG3          	0x493 /* Configuration Register 3. */
#define ADE9039_REG_CF1DEN           	0x494 /* CF1 denominator register. */
#define ADE9039_REG_CF2DEN           	0x495 /* CF2 denominator register. */
#define ADE9039_REG_CF3DEN           	0x496 /* CF3 denominator register. */
#define ADE9039_REG_CF4DEN           	0x497 /* CF4 denominator register. */
#define ADE9039_REG_ZXTOUT           	0x498 /* Zero-crossing timeout configuration register. */
#define ADE9039_REG_ZXTHRSH          	0x499 /* Voltage channel zero-crossing threshold register. */
#define ADE9039_REG_ZX_LP_SEL        	0x49A /* This register selects which zero crossing and which line period measurement for other calculations. */
#define ADE9039_REG_SEQ_CYC          	0x49C /* Number of line cycles used for phase sequence detection. It is recommended this register to 1. */
#define ADE9039_REG_PHSIGN           	0x49D /* Power sign register. */
#define ADE9039_REG_WFB_CFG          	0x4A0 /* Waveform buffer configuration register. */
#define ADE9039_REG_WFB_PG_IRQEN     	0x4A1 /* This register enables interrupts to occur after specific pages of the waveform filled. */
#define ADE9039_REG_WFB_TRG_CFG      	0x4A2 /* This register enables events to trigger a capture in the waveform buffer. */
#define ADE9039_REG_WFB_TRG_STAT     	0x4A3 /* This register indicates the last page that was filled in the waveform buffer and of trigger events. */
#define ADE9039_REG_CONFIG5          	0x4A4 /* Configuration Register 5. */
#define ADE9039_REG_CRC_RSLT         	0x4A8 /* This register holds the CRC of the configuration registers. */
#define ADE9039_REG_CRC_SPI          	0x4A9 /* This register holds the 16-bit CRC of the data sent out on the MOSI pin last SPI register read. */
#define ADE9039_REG_LAST_DATA_16     	0x4AC /* This register holds the data read or written during the last 16-bit transaction SPI port. */
#define ADE9039_REG_LAST_CMD         	0x4AE /* This register holds the address and read/write operation request the last transaction on the SPI port. */
#define ADE9039_REG_CONFIG2          	0x4AF /* Configuration Register 2. */
#define ADE9039_REG_EP_CFG           	0x4B0 /* Energy and power accumulation configuration. */
#define ADE9039_REG_PWR_TIME         	0x4B1 /* Power update time configuration. */
#define ADE9039_REG_EGY_TIME         	0x4B2 /* Energy accumulation update time configuration. */
#define ADE9039_REG_CRC_FORCE        	0x4B4 /* This register forces an update of the CRC of configuration registers. */
#define ADE9039_REG_CRC_OPTEN        	0x4B5 /* This register selects which registers are optionally included in the CRC feature. */
#define ADE9039_REG_TEMP_CFG         	0x4B6 /* Temperature sensor configuration register. */
#define ADE9039_REG_TEMP_RSLT        	0x4B7 /* Temperature measurement result. */
#define ADE9039_REG_PGA_GAIN         	0x4B9 /* This register configures the PGA gain for each ADC. */
#define ADE9039_REG_CHNL_DIS         	0x4BA /* ADC channel enable/disable. */
#define ADE9039_REG_WR_LOCK          	0x4BF /* This register enables the configuration lock feature. */
#define ADE9039_REG_VERSION          	0x4FE /* This is the chip version of the ADE9039 IC. */
#define ADE9039_REG_AI_SINC_DAT      	0x500 /* Current Channel A ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_AV_SINC_DAT      	0x501 /* Voltage Channel A ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_BI_SINC_DAT      	0x502 /* Current Channel B ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_BV_SINC_DAT      	0x503 /* Voltage Channel B ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_CI_SINC_DAT      	0x504 /* Current Channel C ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_CV_SINC_DAT      	0x505 /* Voltage Channel C ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_NI_SINC_DAT      	0x506 /* Neutral current channel ADC waveforms from the sinc4 output at 16 kSPS. */
#define ADE9039_REG_AI_LPF_DAT       	0x510 /* Current Channel A ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_AV_LPF_DAT       	0x511 /* Voltage Channel A ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_BI_LPF_DAT       	0x512 /* Current Channel B ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_BV_LPF_DAT       	0x513 /* Voltage Channel B ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_CI_LPF_DAT       	0x514 /* Current Channel C ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_CV_LPF_DAT       	0x515 /* Voltage Channel C ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_NI_LPF_DAT       	0x516 /* Neutral current channel ADC waveforms from the sinc4 + IIR LPF output at 4 */
#define ADE9039_REG_AV_PCF_1         	0x600 /* SPI burst read accessible. Registers organized functionally. See AV_PCF. */
#define ADE9039_REG_BV_PCF_1         	0x601 /* SPI burst read accessible. Registers organized functionally. See BV_PCF. */
#define ADE9039_REG_CV_PCF_1         	0x602 /* SPI burst read accessible. Registers organized functionally. See CV_PCF. */
#define ADE9039_REG_NI_PCF_1         	0x603 /* SPI burst read accessible. Registers organized functionally. See NI_PCF. */
#define ADE9039_REG_AI_PCF_1         	0x604 /* SPI burst read accessible. Registers organized functionally. See AI_PCF. */
#define ADE9039_REG_BI_PCF_1         	0x605 /* SPI burst read accessible. Registers organized functionally. See BI_PCF. */
#define ADE9039_REG_CI_PCF_1         	0x606 /* SPI burst read accessible. Registers organized functionally. See CI_PCF. */
#define ADE9039_REG_AIRMS_1          	0x607 /* SPI burst read accessible. Registers organized functionally. See AIRMS. */
#define ADE9039_REG_BIRMS_1          	0x608 /* SPI burst read accessible. Registers organized functionally. See BIRMS. */
#define ADE9039_REG_CIRMS_1          	0x609 /* SPI burst read accessible. Registers organized functionally. See CIRMS. */
#define ADE9039_REG_AVRMS_1          	0x60A /* SPI burst read accessible. Registers organized functionally. See AVRMS. */
#define ADE9039_REG_BVRMS_1          	0x60B /* SPI burst read accessible. Registers organized functionally. See BVRMS. */
#define ADE9039_REG_CVRMS_1          	0x60C /* SPI burst read accessible. Registers organized functionally. See CVRMS. */
#define ADE9039_REG_NIRMS_1          	0x60D /* SPI burst read accessible. Registers organized functionally. See NIRMS. */
#define ADE9039_REG_AWATT_1          	0x60E /* SPI burst read accessible. Registers organized functionally. See AWATT. */
#define ADE9039_REG_BWATT_1          	0x60F /* SPI burst read accessible. Registers organized functionally. See BWATT. */
#define ADE9039_REG_CWATT_1          	0x610 /* SPI burst read accessible. Registers organized functionally. See CWATT. */
#define ADE9039_REG_AVA_1            	0x611 /* SPI burst read accessible. Registers organized functionally. See AVA. */
#define ADE9039_REG_BVA_1            	0x612 /* SPI burst read accessible. Registers organized functionally. See BVA. */
#define ADE9039_REG_CVA_1            	0x613 /* SPI burst read accessible. Registers organized functionally. See CVA. */
#define ADE9039_REG_AFVAR_1          	0x617 /* SPI burst read accessible. Registers organized functionally. See AFVAR. */
#define ADE9039_REG_BFVAR_1          	0x618 /* SPI burst read accessible. Registers organized functionally. See BFVAR. */
#define ADE9039_REG_CFVAR_1          	0x619 /* SPI burst read accessible. Registers organized functionally. See CFVAR. */
#define ADE9039_REG_APF_1            	0x61A /* SPI burst read accessible. Registers organized functionally. See APF. */
#define ADE9039_REG_BPF_1            	0x61B /* SPI burst read accessible. Registers organized functionally. See BPF. */
#define ADE9039_REG_CPF_1            	0x61C /* SPI burst read accessible. Registers organized functionally. See CPF. */
#define ADE9039_REG_AVTHD_1          	0x61D /* SPI burst read accessible. Registers organized functionally. See AVTHD. */
#define ADE9039_REG_BVTHD_1          	0x61E /* SPI burst read accessible. Registers organized functionally. See BVTHD. */
#define ADE9039_REG_CVTHD_1          	0x61F /* SPI burst read accessible. Registers organized functionally. See CVTHD. */
#define ADE9039_REG_AITHD_1          	0x620 /* SPI burst read accessible. Registers organized functionally. See AITHD. */
#define ADE9039_REG_BITHD_1          	0x621 /* SPI burst read accessible. Registers organized functionally. See BITHD. */
#define ADE9039_REG_CITHD_1          	0x622 /* SPI burst read accessible. Registers organized functionally. See CITHD. */
#define ADE9039_REG_AFWATT_1         	0x623 /* SPI burst read accessible. Registers organized functionally. See AFWATT. */
#define ADE9039_REG_BFWATT_1         	0x624 /* SPI burst read accessible. Registers organized functionally. See BFWATT. */
#define ADE9039_REG_CFWATT_1         	0x625 /* SPI burst read accessible. Registers organized functionally. See CFWATT. */
#define ADE9039_REG_AFVA_1           	0x626 /* SPI burst read accessible. Registers organized functionally. See AFVA. */
#define ADE9039_REG_BFVA_1           	0x627 /* SPI burst read accessible. Registers organized functionally. See BFVA. */
#define ADE9039_REG_CFVA_1           	0x628 /* SPI burst read accessible. Registers organized functionally. See CFVA. */
#define ADE9039_REG_AFIRMS_1         	0x629 /* SPI burst read accessible. Registers organized functionally. See AFIRMS. */
#define ADE9039_REG_BFIRMS_1         	0x62A /* SPI burst read accessible. Registers organized functionally. See BFIRMS. */
#define ADE9039_REG_CFIRMS_1         	0x62B /* SPI burst read accessible. Registers organized functionally. See CFIRMS. */
#define ADE9039_REG_AFVRMS_1         	0x62C /* SPI burst read accessible. Registers organized functionally. See AFVRMS. */
#define ADE9039_REG_BFVRMS_1         	0x62D /* SPI burst read accessible. Registers organized functionally. See BFVRMS. */
#define ADE9039_REG_CFVRMS_1         	0x62E /* SPI burst read accessible. Registers organized functionally. See CFVRMS. */
#define ADE9039_REG_AIRMSONE_1       	0x62F /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_BIRMSONE_1       	0x630 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_CIRMSONE_1       	0x631 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_AVRMSONE_1       	0x632 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_BVRMSONE_1       	0x633 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_CVRMSONE_1       	0x634 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_NIRMSONE_1       	0x635 /* SPI burst read accessible. Registers organized functionally. See */
#define ADE9039_REG_AV_PCF_2         	0x680 /* SPI burst read accessible. Registers organized by phase. See AV_PCF. */
#define ADE9039_REG_AI_PCF_2         	0x681 /* SPI burst read accessible. Registers organized by phase. See AI_PCF. */
#define ADE9039_REG_AIRMS_2          	0x682 /* SPI burst read accessible. Registers organized by phase. See AIRMS. */
#define ADE9039_REG_AVRMS_2          	0x683 /* SPI burst read accessible. Registers organized by phase. See AVRMS. */
#define ADE9039_REG_AWATT_2          	0x684 /* SPI burst read accessible. Registers organized by phase. See AWATT. */
#define ADE9039_REG_AVA_2            	0x685 /* SPI burst read accessible. Registers organized by phase. See AVA. */
#define ADE9039_REG_AFVAR_2          	0x687 /* SPI burst read accessible. Registers organized by phase. See AFVAR. */
#define ADE9039_REG_APF_2            	0x688 /* SPI burst read accessible. Registers organized by phase. See APF. */
#define ADE9039_REG_AVTHD_2          	0x689 /* SPI burst read accessible. Registers organized by phase. See AVTHD. */
#define ADE9039_REG_AITHD_2          	0x68A /* SPI burst read accessible. Registers organized by phase. See AITHD. */
#define ADE9039_REG_AFWATT_2         	0x68B /* SPI burst read accessible. Registers organized by phase. See AFWATT. */
#define ADE9039_REG_AFVA_2           	0x68C /* SPI burst read accessible. Registers organized by phase. See AFVA. */
#define ADE9039_REG_AFIRMS_2         	0x68D /* SPI burst read accessible. Registers organized by phase. See AFIRMS. */
#define ADE9039_REG_AFVRMS_2         	0x68E /* SPI burst read accessible. Registers organized by phase. See AFVRMS. */
#define ADE9039_REG_AIRMSONE_2       	0x68F /* SPI burst read accessible. Registers organized by phase. See AIRMSONE. */
#define ADE9039_REG_AVRMSONE_2       	0x690 /* SPI burst read accessible. Registers organized by phase. See AVRMSONE. */
#define ADE9039_REG_BV_PCF_2         	0x693 /* SPI burst read accessible. Registers organized by phase. See BV_PCF. */
#define ADE9039_REG_BI_PCF_2         	0x694 /* SPI burst read accessible. Registers organized by phase. See BI_PCF. */
#define ADE9039_REG_BIRMS_2          	0x695 /* SPI burst read accessible. Registers organized by phase. See BIRMS. */
#define ADE9039_REG_BVRMS_2          	0x696 /* SPI burst read accessible. Registers organized by phase. See BVRMS. */
#define ADE9039_REG_BWATT_2          	0x697 /* SPI burst read accessible. Registers organized by phase. See BWATT. */
#define ADE9039_REG_BVA_2            	0x698 /* SPI burst read accessible. Registers organized by phase. See BVA. */
#define ADE9039_REG_BFVAR_2          	0x69A /* SPI burst read accessible. Registers organized by phase. See BFVAR. */
#define ADE9039_REG_BPF_2            	0x69B /* SPI burst read accessible. Registers organized by phase. See BPF. */
#define ADE9039_REG_BVTHD_2          	0x69C /* SPI burst read accessible. Registers organized by phase. See BVTHD. */
#define ADE9039_REG_BITHD_2          	0x69D /* SPI burst read accessible. Registers organized by phase. See BITHD. */
#define ADE9039_REG_BFWATT_2         	0x69E /* SPI burst read accessible. Registers organized by phase. See BFWATT. */
#define ADE9039_REG_BFVA_2           	0x69F /* SPI burst read accessible. Registers organized by phase. See BFVA. */
#define ADE9039_REG_BFIRMS_2         	0x6A0 /* SPI burst read accessible. Registers organized by phase. See BFIRMS. */
#define ADE9039_REG_BFVRMS_2         	0x6A1 /* SPI burst read accessible. Registers organized by phase. See BFVRMS. */
#define ADE9039_REG_BIRMSONE_2       	0x6A2 /* SPI burst read accessible. Registers organized by phase. See BIRMSONE. */
#define ADE9039_REG_BVRMSONE_2       	0x6A3 /* SPI burst read accessible. Registers organized by phase. See BVRMSONE. */
#define ADE9039_REG_CV_PCF_2         	0x6A6 /* SPI burst read accessible. Registers organized by phase. See CV_PCF. */
#define ADE9039_REG_CI_PCF_2         	0x6A7 /* SPI burst read accessible. Registers organized by phase. See CI_PCF. */
#define ADE9039_REG_CIRMS_2          	0x6A8 /* SPI burst read accessible. Registers organized by phase. See CIRMS. */
#define ADE9039_REG_CVRMS_2          	0x6A9 /* SPI burst read accessible. Registers organized by phase. See CVRMS. */
#define ADE9039_REG_CWATT_2          	0x6AA /* SPI burst read accessible. Registers organized by phase. See CWATT. */
#define ADE9039_REG_CVA_2            	0x6AB /* SPI burst read accessible. Registers organized by phase. See CVA. */
#define ADE9039_REG_CFVAR_2          	0x6AD /* SPI burst read accessible. Registers organized by phase. See CFVAR. */
#define ADE9039_REG_CPF_2            	0x6AE /* SPI burst read accessible. Registers organized by phase. See CPF. */
#define ADE9039_REG_CVTHD_2          	0x6AF /* SPI burst read accessible. Registers organized by phase. See CVTHD. */
#define ADE9039_REG_CITHD_2          	0x6B0 /* SPI burst read accessible. Registers organized by phase. See CITHD. */
#define ADE9039_REG_CFWATT_2         	0x6B1 /* SPI burst read accessible. Registers organized by phase. See CFWATT. */
#define ADE9039_REG_CFVA_2           	0x6B2 /* SPI burst read accessible. Registers organized by phase. See CFVA. */
#define ADE9039_REG_CFIRMS_2         	0x6B3 /* SPI burst read accessible. Registers organized by phase. See CFIRMS. */
#define ADE9039_REG_CFVRMS_2         	0x6B4 /* SPI burst read accessible. Registers organized by phase. See CFVRMS. */
#define ADE9039_REG_CIRMSONE_2       	0x6B5 /* SPI burst read accessible. Registers organized by phase. See CIRMSONE. */
#define ADE9039_REG_CVRMSONE_2       	0x6B6 /* SPI burst read accessible. Registers organized by phase. See CVRMSONE. */
#define ADE9039_REG_NI_PCF_2         	0x6B9 /* SPI burst read accessible. Registers organized by phase. See NI_PCF. */
#define ADE9039_REG_NIRMS_2          	0x6BA /* SPI burst read accessible. Registers organized by phase. See NIRMS. */
#define ADE9039_REG_NIRMSONE_2       	0x6BB /* SPI burst read accessible. Registers organized by phase. See NIRMSONE. */

/* ADE9039_REG_CONFIG0 Bit Definition */
#define ADE9039_C_TFVARHR_EN		NO_OS_BIT(19)
#define ADE9039_B_TFVARHR_EN		NO_OS_BIT(18)
#define ADE9039_A_TFVARHR_EN		NO_OS_BIT(17)
#define ADE9039_C_TWATTHR_EN		NO_OS_BIT(16)
#define ADE9039_B_TWATTHR_EN		NO_OS_BIT(15)
#define ADE9039_A_TWATTHR_EN		NO_OS_BIT(14)
#define ADE9039_DISAPLPF		NO_OS_BIT(12)
#define ADE9039_ININTEN			NO_OS_BIT(11)
#define ADE9039_VNOMC_EN		NO_OS_BIT(10)
#define ADE9039_VNOMB_EN		NO_OS_BIT(9)
#define ADE9039_VNOMA_EN 		NO_OS_BIT(8)
#define ADE9039_RMS_SRC_SEL 		NO_OS_BIT(7)
#define ADE9039_ZX_SRC_SEL 		NO_OS_BIT(6)
#define ADE9039_INTEN 			NO_OS_BIT(5)
#define ADE9039_MTEN 			NO_OS_BIT(4)
#define ADE9039_HPFDIS 			NO_OS_BIT(3)
#define ADE9039_ISUM_CFG		NO_OS_GENMASK(1, 0)

/* ADE9039_REG_AMTREGION Bit Definition */
#define ADE9039_AREGION			NO_OS_GENMASK(3, 0)

/* ADE9039_REG_BMTREGION Bit Definition */
#define ADE9039_BREGION			NO_OS_GENMASK(3, 0)

/* ADE9039_REG_CMTREGION Bit Definition */
#define ADE9039_CREGION			NO_OS_GENMASK(3, 0)

/* ADE9039_REG_IPEAK Bit Definition */
#define ADE9039_IPPHASE			NO_OS_GENMASK(26, 24)
#define ADE9039_IPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_VPEAK Bit Definition */
#define ADE9039_VPPHASE			NO_OS_GENMASK(26, 24)
#define ADE9039_VPEAKVAL		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_STATUS0 Bit Definition */
#define ADE9039_STATUS0_TEMP_RDY	NO_OS_BIT(25)
#define ADE9039_STATUS0_MISMTCH		NO_OS_BIT(24)
#define ADE9039_STATUS0_COH_CYCLE_DONE	NO_OS_BIT(23)
#define ADE9039_STATUS0_WFB_TRIG	NO_OS_BIT(22)
#define ADE9039_STATUS0_THD_PF_RDY	NO_OS_BIT(21)
#define	ADE9039_STATUS0_RMSONEREDY	NO_OS_BIT(19)
#define ADE9039_STATUS0_PWRRDY		NO_OS_BIT(18)
#define ADE9039_STATUS0_PAGE_FULL	NO_OS_BIT(17)
#define ADE9039_STATUS0_WFB_TRIG_IRQ	NO_OS_BIT(16)
#define ADE9039_STATUS0_DREADY		NO_OS_BIT(15)
#define ADE9039_STATUS0_CF4 		NO_OS_BIT(14)
#define ADE9039_STATUS0_CF3 		NO_OS_BIT(13)
#define ADE9039_STATUS0_CF2 		NO_OS_BIT(12)
#define ADE9039_STATUS0_CF1 		NO_OS_BIT(11)
#define ADE9039_STATUS0_REVPSUM4 	NO_OS_BIT(10)
#define ADE9039_STATUS0_REVPSUM3 	NO_OS_BIT(9)
#define ADE9039_STATUS0_REVPSUM2 	NO_OS_BIT(8)
#define ADE9039_STATUS0_REVPSUM1 	NO_OS_BIT(7)
#define ADE9039_STATUS0_REVRPC 		NO_OS_BIT(6)
#define ADE9039_STATUS0_REVRPB 		NO_OS_BIT(5)
#define ADE9039_STATUS0_REVRPA 		NO_OS_BIT(4)
#define ADE9039_STATUS0_REVAPC 		NO_OS_BIT(3)
#define ADE9039_STATUS0_REVAPB 		NO_OS_BIT(2)
#define ADE9039_STATUS0_REVAPA 		NO_OS_BIT(1)
#define ADE9039_STATUS0_EGYRDY		NO_OS_BIT(0)

/* ADE9039_REG_STATUS1 Bit Definition */
#define ADE9039_STATUS1_ERROR3		NO_OS_BIT(31)
#define ADE9039_STATUS1_ERROR2		NO_OS_BIT(30)
#define ADE9039_STATUS1_ERROR1		NO_OS_BIT(29)
#define ADE9039_STATUS1_ERROR0		NO_OS_BIT(28)
#define ADE9039_STATUS1_CRC_DONE	NO_OS_BIT(27)
#define ADE9039_STATUS1_CRC_CHG		NO_OS_BIT(26)
#define ADE9039_STATUS1_DIPC		NO_OS_BIT(25)
#define ADE9039_STATUS1_DIPB		NO_OS_BIT(24)
#define ADE9039_STATUS1_DIPA		NO_OS_BIT(23)
#define ADE9039_STATUS1_SWELLC		NO_OS_BIT(22)
#define ADE9039_STATUS1_SWELLB		NO_OS_BIT(21)
#define ADE9039_STATUS1_SWELLA		NO_OS_BIT(20)
#define ADE9039_STATUS1_SEQERR		NO_OS_BIT(18)
#define ADE9039_STATUS1_OI		NO_OS_BIT(17)
#define ADE9039_STATUS1_RSTDONE		NO_OS_BIT(16)
#define ADE9039_STATUS1_ZXIC		NO_OS_BIT(15)
#define ADE9039_STATUS1_ZXIB		NO_OS_BIT(14)
#define ADE9039_STATUS1_ZXIA		NO_OS_BIT(13)
#define ADE9039_STATUS1_ZXCOMB		NO_OS_BIT(12)
#define ADE9039_STATUS1_ZXVC		NO_OS_BIT(11)
#define ADE9039_STATUS1_ZXVB		NO_OS_BIT(10)
#define ADE9039_STATUS1_ZXVA		NO_OS_BIT(9)
#define ADE9039_STATUS1_ZXTOVC		NO_OS_BIT(8)
#define ADE9039_STATUS1_ZXTOVB		NO_OS_BIT(7)
#define ADE9039_STATUS1_ZXTOVA		NO_OS_BIT(6)
#define ADE9039_STATUS1_VAFNOLOAD	NO_OS_BIT(5)
#define ADE9039_STATUS1_RFNOLOAD	NO_OS_BIT(4)
#define ADE9039_STATUS1_AFNOLOAD	NO_OS_BIT(3)
#define ADE9039_STATUS1_VANLOAD		NO_OS_BIT(2)
#define ADE9039_STATUS1_ANLOAD		NO_OS_BIT(0)

/* ADE9039_REG_EVENT_STATUS Bit Definition */
#define ADE9039_EVENT_DREADY		NO_OS_BIT(16)
#define ADE9039_EVENT_VAFNOLOAD		NO_OS_BIT(15)
#define ADE9039_EVENT_RFNOLOAD 		NO_OS_BIT(14)
#define ADE9039_EVENT_AFNOLOAD		NO_OS_BIT(13)
#define ADE9039_EVENT_VANLOAD		NO_OS_BIT(12)
#define ADE9039_EVENT_RNLOAD		NO_OS_BIT(11)
#define ADE9039_EVENT_ANLOAD		NO_OS_BIT(10)
#define ADE9039_EVENT_REVPSUM4		NO_OS_BIT(9)
#define ADE9039_EVENT_REVPSUM3		NO_OS_BIT(8)
#define ADE9039_EVENT_REVPSUM2		NO_OS_BIT(7)
#define ADE9039_EVENT_REVPSUM1		NO_OS_BIT(6)
#define ADE9039_EVENT_SWELLC		NO_OS_BIT(5)
#define ADE9039_EVENT_SWELLB		NO_OS_BIT(4)
#define ADE9039_EVENT_SWELLA		NO_OS_BIT(3)
#define ADE9039_EVENT_DIPC		NO_OS_BIT(2)
#define ADE9039_EVENT_DIPB		NO_OS_BIT(1)
#define ADE9039_EVENT_DIPA		NO_OS_BIT(0)

/* ADE9039_REG_MASK0 Bit Definition */
#define ADE9039_MASK0_TEMP_RDY_MASK	NO_OS_BIT(25)
#define ADE9039_MASK0_MISMTCH		NO_OS_BIT(24)
#define ADE9039_MASK0_COH_WFB_FULL 	NO_OS_BIT(23)
#define ADE9039_MASK0_WFB_TRIG		NO_OS_BIT(22)
#define ADE9039_MASK0_THD_PF_RDY	NO_OS_BIT(21)
#define ADE9039_MASK0_PWRRDY		NO_OS_BIT(18)
#define ADE9039_MASK0_PAGE_FULL		NO_OS_BIT(17)
#define ADE9039_MASK0_WFB_TRIG_IRQ	NO_OS_BIT(16)
#define ADE9039_MASK0_DREADY		NO_OS_BIT(15)
#define ADE9039_MASK0_CF4		NO_OS_BIT(14)
#define ADE9039_MASK0_CF3		NO_OS_BIT(13)
#define ADE9039_MASK0_CF2	 	NO_OS_BIT(12)
#define ADE9039_MASK0_CF1		NO_OS_BIT(11)
#define ADE9039_MASK0_REVPSUM4		NO_OS_BIT(10)
#define ADE9039_MASK0_REVPSUM3		NO_OS_BIT(9)
#define ADE9039_MASK0_REVPSUM2		NO_OS_BIT(8)
#define ADE9039_MASK0_REVPSUM1		NO_OS_BIT(7)
#define ADE9039_MASK0_REVRPC		NO_OS_BIT(6)
#define ADE9039_MASK0_REVRPB		NO_OS_BIT(5)
#define ADE9039_MASK0_REVRPA		NO_OS_BIT(4)
#define ADE9039_MASK0_REVAPC		NO_OS_BIT(3)
#define ADE9039_MASK0_REVAPB		NO_OS_BIT(2)
#define ADE9039_MASK0_REVAPA		NO_OS_BIT(1)
#define ADE9039_MASK0_EGYRDY		NO_OS_BIT(0)

/* ADE9039_REG_MASK1 Bit Definition */
#define ADE9039_MASK1_ERROR3		NO_OS_BIT(31)
#define ADE9039_MASK1_ERROR2		NO_OS_BIT(30)
#define ADE9039_MASK1_ERROR1		NO_OS_BIT(29)
#define ADE9039_MASK1_ERROR0		NO_OS_BIT(28)
#define ADE9039_MASK1_CRC_DONE		NO_OS_BIT(27)
#define ADE9039_MASK1_CRC_CHG		NO_OS_BIT(26)
#define ADE9039_MASK1_DIPC		NO_OS_BIT(25)
#define ADE9039_MASK1_DIPB		NO_OS_BIT(24)
#define ADE9039_MASK1_DIPA		NO_OS_BIT(23)
#define ADE9039_MASK1_SWELLC		NO_OS_BIT(22)
#define ADE9039_MASK1_SWELLB		NO_OS_BIT(21)
#define ADE9039_MASK1_SWELLA		NO_OS_BIT(20)
#define ADE9039_MASK1_SEQERR		NO_OS_BIT(18)
#define ADE9039_MASK1_OI		NO_OS_BIT(17)
#define ADE9039_MASK1_ZXIC		NO_OS_BIT(15)
#define ADE9039_MASK1_ZXIB		NO_OS_BIT(14)
#define ADE9039_MASK1_ZXIA		NO_OS_BIT(13)
#define ADE9039_MASK1_ZXCOMB		NO_OS_BIT(12)
#define ADE9039_MASK1_ZXVC		NO_OS_BIT(11)
#define ADE9039_MASK1_ZXVB 		NO_OS_BIT(10)
#define ADE9039_MASK1_ZXVA		NO_OS_BIT(9)
#define ADE9039_MASK1_ZXTOVC		NO_OS_BIT(8)
#define ADE9039_MASK1_ZXTOVB		NO_OS_BIT(7)
#define ADE9039_MASK1_ZXTOVA		NO_OS_BIT(6)
#define ADE9039_MASK1_VAFNOLOAD		NO_OS_BIT(5)
#define ADE9039_MASK1_AFNOLOAD		NO_OS_BIT(3)
#define ADE9039_MASK1_VANLOAD		NO_OS_BIT(2)
#define ADE9039_MASK1_ANLOAD		NO_OS_BIT(0)

/* ADE9039_REG_EVENT_MASK Bit Definition */
#define ADE9039_EVENT_DREADY_MSK	NO_OS_BIT(16)
#define ADE9039_EVENT_VAFNOLOAD_MSK	NO_OS_BIT(15)
#define ADE9039_EVENT_RFNOLOAD_MSK	NO_OS_BIT(14)
#define ADE9039_EVENT_AFNOLOAD_MSK	NO_OS_BIT(13)
#define ADE9039_EVENT_VANLOAD_MSK	NO_OS_BIT(12)
#define ADE9039_EVENT_ANLOAD_MSK	NO_OS_BIT(10)
#define ADE9039_EVENT_REVPSUM4_MSK	NO_OS_BIT(9)
#define ADE9039_EVENT_REVPSUM3_MSK	NO_OS_BIT(8)
#define ADE9039_EVENT_REVPSUM2_MSK	NO_OS_BIT(7)
#define ADE9039_EVENT_REVPSUM1_MSK	NO_OS_BIT(6)
#define ADE9039_EVENT_SWELLCEN_MSK	NO_OS_BIT(5)
#define ADE9039_EVENT_SWELLBEN_MSK	NO_OS_BIT(4)
#define ADE9039_EVENT_SWELLAEN_MSK	NO_OS_BIT(3)
#define ADE9039_EVENT_DIPCEN_MSK	NO_OS_BIT(2)
#define ADE9039_EVENT_DIPBEN_MSK	NO_OS_BIT(1)
#define ADE9039_EVENT_DIPAEN_MSK	NO_OS_BIT(0)

/* ADE9039_REG_OILVL Bit Definition */
#define ADE9039_OILVL_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_OIA Bit Definition */
#define ADE9039_OIA_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_OIB Bit Definition */
#define ADE9039_OIB_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_OIC Bit Definition */
#define ADE9039_OIC_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_OIN Bit Definition */
#define ADE9039_OIN_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_VLEVEL Bit Definition */
#define ADE9039_VLEVEL_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_DIP_LVL Bit Definition */
#define ADE9039_REG_DIPLVL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_DIPA */
#define ADE9039_DIPA_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_DIPB */
#define ADE9039_DIPB_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_DIPC */
#define ADE9039_DIPC_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_SWELL_LVL  */
#define ADE9039_SWELLLVL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_SWELLA */
#define ADE9039_SWELLA_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_SWELLB */
#define ADE9039_SWELLB_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_SWELLC */
#define ADE9039_SWELLC_VAL 		NO_OS_GENMASK(23, 0)

/* ADE9039_REG_PHNOLOAD Bit Definition */
#define ADE9039_CFVANL			NO_OS_BIT(17)
#define ADE9039_CFVARNL			NO_OS_BIT(16)
#define ADE9039_CFWATTNL		NO_OS_BIT(15)
#define ADE9039_CVANL			NO_OS_BIT(14)
#define ADE9039_CWATTNL			NO_OS_BIT(12)
#define ADE9039_BFVANL			NO_OS_BIT(11)
#define ADE9039_BFVARNL			NO_OS_BIT(10)
#define ADE9039_BFWATTNL		NO_OS_BIT(9)
#define ADE9039_BVANL			NO_OS_BIT(8)
#define ADE9039_BWATTNL			NO_OS_BIT(6)
#define ADE9039_AFVANL			NO_OS_BIT(5)
#define ADE9039_AFVARNL			NO_OS_BIT(4)
#define ADE9039_AVANL			NO_OS_BIT(2)
#define ADE9039_AWATTNL			NO_OS_BIT(0)


/* ADE9039_REG_ADC_REDIRECT Bit Definition */
#define ADE9039_VC_DIN	 		NO_OS_GENMASK(20, 18)
#define ADE9039_VB_DIN	 		NO_OS_GENMASK(17, 15)
#define ADE9039_VA_DIN	 		NO_OS_GENMASK(14, 12)
#define ADE9039_IN_DIN	 		NO_OS_GENMASK(11, 9)
#define ADE9039_IC_DIN	 		NO_OS_GENMASK(8, 6)
#define ADE9039_IB_DIN	 		NO_OS_GENMASK(5, 3)
#define ADE9039_IA_DIN	 		NO_OS_GENMASK(2, 0)

/* ADE9039_REG_CF_LCFG Bit Definition */
#define ADE9039_CF4_LT			NO_OS_BIT(22)
#define ADE9039_CF3_LT			NO_OS_BIT(21)
#define ADE9039_CF2_LT			NO_OS_BIT(20)
#define ADE9039_CF1_LT			NO_OS_BIT(19)
#define ADE9039_CF_LTMR			NO_OS_GENMASK(18, 0)

/* ADE9039_REG_PART_ID Bit Definition */
#define ADE9039_ADE9039_ID		NO_OS_BIT(20)

/* ADE9039_REG_TEMP_TRIM Bit Definition */
#define ADE9039_TEMP_OFFSET		NO_OS_GENMASK(31, 16)
#define ADE9039_TEMP_GAIN		NO_OS_GENMASK(15, 0)

/* ADE9039_REG_CONFIG1 Bit Definition */
#define ADE9039_EXT_REF			NO_OS_BIT(15)
#define ADE9039_IRQ0_ON_IRQ1		NO_OS_BIT(12)
#define ADE9039_BURST_EN		NO_OS_BIT(11)
#define ADE9039_DIP_SWELL_IRQ_MODE	NO_OS_BIT(10)
#define ADE9039_PWR_SETTLE		NO_OS_GENMASK(9, 8)
#define ADE9039_CF_ACC_CLR		NO_OS_BIT(5)
#define ADE9039_CF4_CFG			NO_OS_GENMASK(3, 2)
#define ADE9039_CF3_CFG			NO_OS_BIT(1)
#define ADE9039_SWRST			NO_OS_BIT(0)

/* ADE9039_REG_OISTATUS */
#define ADE9039_OIPHASE			NO_OS_GENMASK(3, 0)

/* ADE9039_REG_CFMODE Bit Definition */
#define ADE9039_CF4DIS			NO_OS_BIT(15)
#define ADE9039_CF3DIS			NO_OS_BIT(14)
#define ADE9039_CF2DIS			NO_OS_BIT(13)
#define ADE9039_CF1DIS			NO_OS_BIT(12)
#define ADE9039_CF4SEL			NO_OS_GENMASK(11, 9)
#define ADE9039_CF3SEL			NO_OS_GENMASK(8, 6)
#define ADE9039_CF2SEL			NO_OS_GENMASK(5, 3)
#define ADE9039_CF1SEL 			NO_OS_GENMASK(2, 0)

/* ADE9039_REG_COMPMODE Bit Definition */
#define ADE9039_TERMSEL4		NO_OS_GENMASK(11, 9)
#define ADE9039_TERMSEL3		NO_OS_GENMASK(8, 6)
#define ADE9039_TERMSEL2		NO_OS_GENMASK(5, 3)
#define ADE9039_TERMSEL1 		NO_OS_GENMASK(2, 0)

/* ADE9039_REG_ACCMODE Bit Definition */
#define ADE9039_SELFREQ			NO_OS_BIT(8)
#define ADE9039_ICONSEL			NO_OS_BIT(7)
#define ADE9039_VCONSEL			NO_OS_GENMASK(6, 4)
#define ADE9039_VARACC	 		NO_OS_GENMASK(3, 2)
#define ADE9039_WATTACC			NO_OS_GENMASK(1, 0)

/* ADE9039_REG_CONFIG3 Bit Definition */
#define ADE9039_OC_EN 			NO_OS_GENMASK(15, 12)
#define ADE9039_PEAKSEL	 		NO_OS_GENMASK(4, 2)

/* ADE9039_REG_ZX_LP_SEL Bit Definition */
#define ADE9039_LP_SEL			NO_OS_GENMASK(4, 3)
#define ADE9039_ZX_SEL	 		NO_OS_GENMASK(2, 1)

/* ADE9039_REG_PHSIGN Bit Definition */
#define ADE9039_SUM4SIGN		NO_OS_BIT(9)
#define ADE9039_SUM3SIGN		NO_OS_BIT(8)
#define ADE9039_SUM2SIGN		NO_OS_BIT(7)
#define ADE9039_SUM1SIGN		NO_OS_BIT(6)
#define ADE9039_CVARSIGN		NO_OS_BIT(5)
#define ADE9039_CWSIGN			NO_OS_BIT(4)
#define ADE9039_BVARSIGN 		NO_OS_BIT(3)
#define ADE9039_BWSIGN			NO_OS_BIT(2)
#define ADE9039_AVARSIGN		NO_OS_BIT(1)
#define ADE9039_AWSIGN			NO_OS_BIT(0)

/* ADE9039_REG_WFB_CFG Bit Definition */
#define ADE9039_WF_IN_EN		NO_OS_BIT(12)
#define ADE9039_WF_SRC			NO_OS_GENMASK(9, 8)
#define ADE9039_WF_MODE			NO_OS_GENMASK(7, 6)
#define ADE9039_WF_CAP_SEL		NO_OS_BIT(5)
#define ADE9039_WF_CAP_EN		NO_OS_BIT(4)
#define ADE9039_BURST_CHAN		NO_OS_GENMASK(3, 0)

/* ADE9039_REG_WFB_TRG_CFG Bit Definition */
#define ADE9039_TRIG_FORCE		NO_OS_BIT(10)
#define ADE9039_ZXCOMB			NO_OS_BIT(9)
#define ADE9039_ZXVC			NO_OS_BIT(8)
#define ADE9039_ZXVB 			NO_OS_BIT(7)
#define ADE9039_ZXVA			NO_OS_BIT(6)
#define ADE9039_ZXIC			NO_OS_BIT(5)
#define ADE9039_ZXIB			NO_OS_BIT(4)
#define ADE9039_ZXIA			NO_OS_BIT(3)
#define ADE9039_OI			NO_OS_BIT(2)
#define ADE9039_SWELL			NO_OS_BIT(1)
#define ADE9039_DIP 			NO_OS_BIT(0)

/* ADE9039_REG_WFB_TRG_STAT Bit Definition */
#define ADE9039_WFB_LAST_PAGE		NO_OS_GENMASK(15, 12)
#define ADE9039_WFB_TRIG_ADDR		NO_OS_GENMASK(10, 0)

/* ADE9039_REG_CONFIG2 Bit Definition */
#define ADE9039_UPERIOD_SEL		NO_OS_BIT(12)
#define ADE9039_HPF_CRN			NO_OS_GENMASK(11, 9)

/* ADE9039_REG_EP_CFG Bit Definition */
#define ADE9039_NOLOAD_TMR		NO_OS_GENMASK(15, 13)
#define ADE9039_PWR_SIGN_SEL_1		NO_OS_BIT(7)
#define ADE9039_PWR_SIGN_SEL_0		NO_OS_BIT(6)
#define ADE9039_EGY_LD_ACCUM		NO_OS_BIT(4)
#define ADE9039_EGY_TMR_MODE		NO_OS_BIT(1)
#define ADE9039_EGY_PWR_EN		NO_OS_BIT(0)

/* ADE9039_REG_CRC_FORCE Bit Definition */
#define ADE9039_FORCE_CRC_UPDATE	NO_OS_BIT(0)

/* ADE9039_REG_CRC_OPTEN Bit Definition */
#define ADE9039_CRC_WFB_TRG_CFG_EN	NO_OS_BIT(15)
#define ADE9039_CRC_WFB_PG_IRQEN	NO_OS_BIT(14)
#define ADE9039_CRC_WFB_CFG_EN		NO_OS_BIT(13)
#define ADE9039_CRC_SEQ_CYC_EN		NO_OS_BIT(12)
#define ADE9039_CRC_ZXLPSEL_EN		NO_OS_BIT(11)
#define ADE9039_CRC_ZXTOUT_EN		NO_OS_BIT(10)
#define ADE9039_CRC_APP_NL_LVL_EN	NO_OS_BIT(9)
#define ADE9039_CRC_REACT_NL_LVL_EN	NO_OS_BIT(8)
#define ADE9039_CRC_ACT_NL_LVL_EN	NO_OS_BIT(7)
#define ADE9039_CRC_SWELL_CYC_EN	NO_OS_BIT(6)
#define ADE9039_CRC_SWELL_LVL_EN	NO_OS_BIT(5)
#define ADE9039_CRC_DIP_CYC_EN		NO_OS_BIT(4)
#define ADE9039_CRC_DIP_LVL_EN		NO_OS_BIT(3)
#define ADE9039_CRC_EVENT_MASK_EN	NO_OS_BIT(2)
#define ADE9039_CRC_MASK1_EN		NO_OS_BIT(1)
#define ADE9039_CRC_MASK0_EN		NO_OS_BIT(0)

/* ADE9039_REG_TEMP_CFG Bit Definition */
#define ADE9039_TEMP_START		NO_OS_BIT(3)
#define ADE9039_TEMP_EN			NO_OS_BIT(2)
#define ADE9039_TEMP_TIME		NO_OS_GENMASK(1, 0)

/* ADE9039_REG_TEMP_RSLT Bit Definition */
#define ADE9039_TEMP_RSLT		NO_OS_GENMASK(11, 0)

/* ADE9039_PGA_GAIN Bit Definition */
#define ADE9039_VC_GAIN			NO_OS_GENMASK(13, 12)
#define ADE9039_VB_GAIN			NO_OS_GENMASK(11, 10)
#define ADE9039_VA_GAIN			NO_OS_GENMASK(9, 8)
#define ADE9039_IN_GAIN			NO_OS_GENMASK(7, 6)
#define ADE9039_IC_GAIN			NO_OS_GENMASK(5, 4)
#define ADE9039_IB_GAIN			NO_OS_GENMASK(3, 2)
#define ADE9039_IA_GAIN			NO_OS_GENMASK(1, 0)

/* ADE9039_CHNL_DIS Bit Definition */
#define ADE9039_VC_DISADC		NO_OS_BIT(6)
#define ADE9039_VB_DISADC		NO_OS_BIT(5)
#define ADE9039_VA_DISADC		NO_OS_BIT(4)
#define ADE9039_IN_DISADC		NO_OS_BIT(3)
#define ADE9039_IC_DISADC		NO_OS_BIT(2)
#define ADE9039_IB_DISADC		NO_OS_BIT(1)
#define ADE9039_IA_DISADC		NO_OS_BIT(0)

/* Miscellaneous Definitions */
#define ADE9039_CHIP_ID			0xFE
#define ADE9039_PART_ID         	1
#define ADE9039_RESET_RECOVER   	100

/*Configuration registers*/
/*PGA@0x0000. Gain of all channels=1*/
#define ADE9039_PGA_GAIN 		0x0000
/*Integrator disabled*/
#define ADE9039_CONFIG0 		0x00000000
/*CF3/ZX pin outputs Zero crossing */
#define ADE9039_CONFIG1 		0x0002
/*Default High pass corner frequency of 12Hz*/
#define ADE9039_CONFIG2 		0x0A00
/*Peak and overcurrent detection disabled*/
#define ADE9039_CONFIG3 		0x0000
/*50Hz operation, 3P4W Wye configuration, signed accumulation*/
#define ADE9039_ACCMODE 		0x0000
/*Line period and zero crossing obtained from combined signals VA,VB and VC*/
#define ADE9039_ZX_LP_SEL 		0x001E
/*Enable EGYRDY interrupt*/
#define ADE9039_MASK0 			0x00000001
/*MASK1 interrupts disabled*/
#define ADE9039_MASK1 			0x00000000
/*Events disabled */
#define ADE9039_EVENT_MASK 		0x00000000
/*Assuming Vnom=1/2 of full scale.*/
/*Refer Technical reference manual for detailed calculations.*/
#define ADE9039_VLEVEL  		0x00144084
/* Set DICOEFF= 0xFFFFE000 when integrator is enabled*/
#define ADE9039_DICOEFF 		0x00000000
/*Constant Definitions***/
/*DSP ON*/
#define ADE9039_RUN_ON 			0x0001
/* CF1 indicates the total active power output of Phase A, Phase B, and Phase C */
#define ADE9039_COMPMODE 		0x0007
/*Waveform buffer Settings*/
/*Neutral current samples enabled, Resampled data enabled*/
/*Burst all channels*/
#define ADE9039_WFB_CFG 		0x1000
/*size of buffer to read. 512 Max.Each element IA,VA...IN has max 512 points*/
/*[Size of waveform buffer/number of sample sets = 2048/4 = 512]*/
/*(Refer ADE9039 technical reference manual for more details)*/
#define WFB_ELEMENT_ARRAY_SIZE 		512
/*Full scale Codes (FS) referred from Datasheet.*/
/*Respective digital codes are produced when ADC inputs*/
/*are at full scale. Do not Change. */
#define ADE9039_RMS_FS_CODES  		52866837
#define ADE9039_WATT_FS_CODES 		20823646

/* Assuming a transformer ratio of 1000:1 and 10 ohms burden resistance value */
#define ADE9039_BURDEN_RES              10
#define ADE9039_CURRENT_TR_RATIO        1000
#define ADE9039_CURRENT_TR_FCN          (ADE9039_CURRENT_TR_RATIO / ADE9039_BURDEN_RES)
/* Assuming a voltage divider with Rlow 1k and Rup 990k */
#define ADE9039_UP_RES                	990000
#define ADE9039_DOWN_RES		1000
#define ADE9039_VOLTAGE_TR_FCN		((ADE9039_DOWN_RES + ADE9039_UP_RES) / ADE9039_DOWN_RES)

// 0.707V rms full scale * 1000 for mili units
#define ADE9039_FS_VOLTAGE           	707

/**
 * @enum ade9039_isum_cfg_e
 * @brief ADE9039 isum calculation configuration.
 */
enum ade9039_isum_cfg_e {
	/* Approximated neutral current rms calculation */
	ADE9039_ISUM_APROX_N,
	/* Determine positive mismatch between neutral and
	phase currents */
	ADE9039_ISUM_DET_MISM_POS,
	/* determine negative mismatch between neutral and
	phase currents */
	ADE9039_ISUM_DET_MISM_NEG,
	/* approximated neutral current rms calculation */
	ADE9039_ISUM_APROX_N_RMS
};

/**
 * @enum ade9039_aregion_sel_e
 * @brief ADE9039 These bits indicate which AIGAINx and APHCALx
    is currently being used.
 */
enum ade9039_aregion_sel_e {
	/* 0 */
	ADE9039_AIGAIN_APHCAL_0,
	/* 1 */
	ADE9039_AIGAIN_APHCAL_1,
	/* 2 */
	ADE9039_AIGAIN_APHCAL_2,
	/* 3 */
	ADE9039_AIGAIN_APHCAL_3,
	/* 4 */
	ADE9039_AIGAIN_APHCAL_4,
	/* function disabled */
	ADE9039_AIGAIN_APHCAL_DISABLE = 15
};

/**
 * @enum ade9039_bregion_sel_e
 * @brief ADE9039 These bits indicate which BIGAINx and BPHCALx
    is currently being used.
 */
enum ade9039_bregion_sel_e {
	/* 0 */
	ADE9039_BIGAIN_BPHCAL_0,
	/* 1 */
	ADE9039_BIGAIN_BPHCAL_1,
	/* 2 */
	ADE9039_BIGAIN_BPHCAL_2,
	/* 3 */
	ADE9039_BIGAIN_BPHCAL_3,
	/* 4 */
	ADE9039_BIGAIN_BPHCAL_4,
	/* function disabled */
	ADE9039_BIGAIN_BPHCAL_DISABLE = 15
};

/**
 * @enum ade9039_cregion_sel_e
 * @brief ADE9039 These bits indicate which CIGAINx and CPHCALx
    is currently being used.
 */
enum ade9039_cregion_sel_e {
	/* 0 */
	ADE9039_CIGAIN_CPHCAL_0,
	/* 1 */
	ADE9039_CIGAIN_CPHCAL_1,
	/* 2 */
	ADE9039_CIGAIN_CPHCAL_2,
	/* 3 */
	ADE9039_CIGAIN_CPHCAL_3,
	/* 4 */
	ADE9039_CIGAIN_CPHCAL_4,
	/* function disabled */
	ADE9039_CIGAIN_CPHCAL_DISABLE = 15
};

/**
 * @enum ade9039_cf4_pin_out_cfg_e
 * @brief ADE9039 These bits indicate which function
    to output on CF4 pin
 */
enum ade9039_cf4_pin_out_cfg_e {
	/* Digital to freq converter */
	ADE9039_CF4_D_F_CONV,
	/* Digital to freq converter */
	ADE9039_CF4_D_F_CONV2,
	/* Event */
	ADE9039_CF4_EVENT,
	/* Dready */
	ADE9039_CF4_DREADY,
};

/**
 * @enum ade9039_pwr_settle_e
 * @brief ADE9039 Power settle time
 */
enum ade9039_pwr_settle_e {
	/* 64 ms */
	ADE9039_PWR_SETTLE_0,
	/* 128 ms */
	ADE9039_PWR_SETTLE_1,
	/* 256 ms */
	ADE9039_PWR_SETTLE_2,
	/* 0 ms */
	ADE9039_PWR_SETTLE_3
};

/**
 * @enum ade9039_oiphase_e
 * @brief ADE9039 indicates that a phase is above OILVL
 */
enum ade9039_oiphase_e {
	/* Phase A */
	ADE9039_OIPHASE_A,
	/* Phase B */
	ADE9039_OIPHASE_B,
	/* Phase C */
	ADE9039_OIPHASE_C,
	/* Phase N */
	ADE9039_OIPHASE_N
};

/**
 * @enum ade9039_cf4_sel_e
 * @brief ADE9039 Type of energy output on the CF4 pin. Configure
    TERMSEL4 in the COMPMODE register to select
    which phases are included
 */
enum ade9039_cf4_sel_e {
	/* Total active power */
	ADE9039_CF4_SEL_ACTIV_P,
	/* Total apparent power */
	ADE9039_CF4_SEL_APPARENT_P = 2,
	/* Fundamental active power */
	ADE9039_CF4_SEL_FUNDAMENTAL_ACTIVE_P,
	/* Fundamental reactive power */
	ADE9039_CF4_SEL_FUN_REACTIVE_P,
	/* Fundamental apparent power */
	ADE9039_CF4_SEL_FUN_APPARENT_P,
	/* Total active power */
	ADE9039_CF4_SEL_TOTAL_ACTIVE_P,
	/* Total active power2 */
	ADE9039_CF4_SEL_TOTAL_ACTIVE_P_2,
};

/**
 * @enum ade9039_freq_sel_e
 * @brief ADE9039 Freq value
 */
enum ade9039_freq_sel_e {
	/* 50 Hz */
	ADE9039_SELFREQ_50,
	/* 60 Hz */
	ADE9039_SELFREQ_60,
};

/**
 * @enum ade9039_vconsel_e
 * @brief ADE9039 3-wire and 4-wire hardware configuration selection
 */
enum ade9039_vconsel_e {
	/* 4 wire wye */
	ADE9039_4WIRE_WYE,
	/* 3-wire delta. VB' = VA − VC */
	ADE9039_3WIRE_DELTA,
	/* 4-wire wye, nonBlondel compliant. VB' = −VA − VC */
	ADE9039_4WIRE_WYE_VA_VC,
	/* 4-wire delta, nonBlondel compliant. VB' = −VA */
	ADE9039_4WIRE_WYE_VA,
	/* 3-wire delta. VA' = VA − VB; VB' = VA − VC; VC' = VC − VB*/
	ADE9039_3WIRE_DELTA_2
};

/**
 * @enum ade9039_var_acc_mode_e
 * @brief ADE9039 Total and fundamental reactive power accumulation
     mode for energy registers and CFx pulses.
 */
enum ade9039_var_acc_mode_e {
	/* signed acc mode */
	ADE9039_ACC_SIGNED,
	/* absolute value acc mode */
	ADE9039_ACC_ABSOLUTE,
	/* positive acc mode */
	ADE9039_ACC_POSITIVE,
	/* negative acc mode */
	ADE9039_ACC_NEGATIVE
};

/**
 * @enum ade9039_line_period_sel_e
 * @brief Selects line period measurement used for
    VRMS½ cycle, 10 cycle rms/12 cycle rms, and
    resampling.
 */
enum ade9039_line_period_sel_e {
	/* signed acc mode */
	ADE9039_APERIOD,
	/* absolute value acc mode */
	ADE9039_BPERIOD,
	/* positive acc mode */
	ADE9039_CPERIOD,
	/* negative acc mode */
	ADE9039_COM_PERIOD
};

/**
 * @enum ade9039_zx_select_e
 * @brief Selects the zero-crossing signal, which can be
    routed to the CF3/ZX output pin and used for
    line cycle energy accumulation
 */
enum ade9039_zx_select_e {
	/* Phase A voltage zero-crossing signal */
	ADE9039_ZXVA_SEL,
	/* Phase B voltage zero-crossing signal */
	ADE9039_ZXVB_SEL,
	/* Phase C voltage zero-crossing signal */
	ADE9039_ZXVC_SEL,
	/* Zero-crossing on combined signal from VA, VB, and VC */
	ADE9039_ZXCOMB_SEL
};

/**
 * @enum ade9039_wf_src_e
 * @brief Waveform buffer source and DREADY (data
    ready update rate) selection
 */
enum ade9039_wf_src_e {
	/* Sinc4 output at 16 kSPS */
	ADE9039_SRC_SINC4,
	/* Sinc4 + IIR LPF output at 4 kSPS */
	ADE9039_SRC_SINC4_IIR = 2,
	/* Current and voltage channel waveform samples,
	processed by the DSP (xI_PCF, xV_PCF) at 4 kSPS */
	ADE9039_SRC_DSP
};

/**
 * @enum ade9039_wf_mode_e
 * @brief Fixed data rate waveforms filling and trigger
    based modes.
 */
enum ade9039_wf_mode_e {
	/* Stop when waveform buffer is full */
	ADE9039_MODE_STOP_FULL,
	/* Continuous fill—stop only on enabled trigger
	events */
	ADE9039_MODE_TRIG_EN_EVENTS,
	/* Continuous filling—center capture around
	enabled trigger events. */
	ADE9039_MODE_CENTER_CAPTURE,
	/* Continuous fill—save event address of enabled
	trigger events */
	ADE9039_MODE_SAVE_EVENT_ADDR
};

/**
 * @enum ade9039_burst_ch_e
 * @brief Selects which data to read out of the waveform
    buffer through SPI
 */
enum ade9039_burst_ch_e {
	/* All channels */
	ADE9039_BURST_ALL_CH,
	/* IA and VA */
	ADE9039_BURST_IA_VA,
	/* IB and VB */
	ADE9039_BURST_IB_VB,
	/* IC and VC */
	ADE9039_BURST_IC_VC,
	/* IA */
	ADE9039_BURST_IA = 8,
	/* VA */
	ADE9039_BURST_VA,
	/* IB */
	ADE9039_BURST_IB,
	/* VB */
	ADE9039_BURST_VB,
	/* IC */
	ADE9039_BURST_IC,
	/* VC */
	ADE9039_BURST_VC,
	/* IN if WF_IN_EN = 1*/
	ADE9039_BURST_IN,
	/* Burst Disable read single addr */
	ADE9039_BURST_DISABLED
};

/**
 * @enum ade9039_hpf_freq_e
 * @brief High-pass filter corner (f3dB) enabled when the
    HPFDIS bit in the CONFIG0 register is equal to zero
 */
enum ade9039_hpf_freq_e {
	/* 0.2 Hz. */
	ADE9039_HPF_0_2,
	/* 1 Hz. */
	ADE9039_HPF_1,
	/* 3 Hz. */
	ADE9039_HPF_3,
	/* 6 Hz. */
	ADE9039_HPF_6,
	/* 10 Hz. */
	ADE9039_HPF_10,
	/* 12 Hz. */
	ADE9039_HPF_12,
	/* 18 Hz. */
	ADE9039_HPF_18,
	/* 21.6 Hz. */
	ADE9039_HPF_21_6
};

/**
 * @enum ade9039_no_load_tmr_e
 * @brief This register configures how many 4 kSPS
    samples to evaluate the no load condition over
 */
enum ade9039_no_load_tmr_e {
	/* 64 samples */
	ADE9039_NOLOAD_SAMPLES_64,
	/* 128 samples */
	ADE9039_NOLOAD_SAMPLES_128,
	/* 256 samples */
	ADE9039_NOLOAD_SAMPLES_256,
	/* 512 samples */
	ADE9039_NOLOAD_SAMPLES_512,
	/* 1024 samples */
	ADE9039_NOLOAD_SAMPLES_1024,
	/* 2048 samples */
	ADE9039_NOLOAD_SAMPLES_2048,
	/* 4096 samples */
	ADE9039_NOLOAD_SAMPLES_4096,
	/* disable no load threshold */
	ADE9039_NOLOAD_SAMPLES_DISABLE
};

/**
 * @enum ade9039_temp_time_e
 * @brief ADE9039 Temperature measurement time
 */
enum ade9039_temp_time_e {
	/* 1 sample every 1ms */
	ADE9039_TEMP_TIME_1,
	/* 256 samples new measurement every 256ms */
	ADE9039_TEMP_TIME_256,
	/* 512 samples new measurement every 512 ms */
	ADE9039_TEMP_TIME_512,
	/* 1024 samoples new measurement every 1 sec */
	ADE9039_TEMP_TIME_1024
};

/**
 * @enum ade9039_vc_gain_e
 * @brief ADE9039 PGA gain for Voltage Channel C ADC
 */
enum ade9039_vc_gain_e {
	/* Gain = 1 */
	ADE9039_VC_GAIN_1,
	/* Gain = 2 */
	ADE9039_VC_GAIN_2,
	/* Gain = 3 */
	ADE9039_VC_GAIN_4,
	/* Gain = 4 */
	ADE9039_VC_GAIN_4_2
};

/**
 * @enum ade9039_phase
 * @brief ADE9039 available phases.
 */
enum ade9039_phase {
	ADE9039_PHASE_A,
	ADE9039_PHASE_B,
	ADE9039_PHASE_C
};

/**
 * @enum ade9039_egy_model
 * @brief ADE9039 available user energy use models.
 */
enum ade9039_egy_model {
	ADE9039_EGY_WITH_RESET,
	ADE9039_EGY_HALF_LINE_CYCLES,
	ADE9039_EGY_NR_SAMPLES
};

/**
 * @enum ade9039_power_mode_e
 * @brief ADE9039 Power mode selection
 */
enum ade9039_power_mode_e {
	/* SPI is not available in PSM2 & PSM3*/
	/* PSM0 normal mode */
	NORMAL_MODE,
	/* PSM1 Tamper measurement mode */
	TAMPER_MODE,
	/* PSM2 Current peak detect mode */
	CURRENT_PEAK_DETECT_MODE,
	/* PSM3 IDLE */
	IDLE_MODE
};

/**
 * @struct ade9039_init_param
 * @brief ADE9039 Device initialization parameters.
 */
struct ade9039_init_param {
	/** Device communication descriptor */
	struct no_os_spi_init_param 	*spi_init;
	/* psm0 descriptor */
	struct no_os_gpio_desc      	*psm0_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc      	*psm1_desc;
	/* reset descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* Variable for mode selection */
	uint8_t             power_mode;
	/* Nominal frequency */
	uint8_t				freq;
	/** Enable the temperature */
	bool				temp_en;
};

/**
 * @struct ade9039_dev
 * @brief ADE9039 Device structure.
 */
struct ade9039_dev {
	/** Device communication descriptor */
	struct no_os_spi_desc		*spi_desc;
	/* psm0 descriptor */
	struct no_os_gpio_desc      	*psm0_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc      	*psm1_desc;
	/* psm1 descriptor */
	struct no_os_gpio_desc      	*reset_desc;
	/* Variable storing the WATT value */
	uint32_t			watt_val;
	/* Variable storing the VA value */
	uint32_t			va_val;
	/* Variable storing the FWATT value */
	uint32_t			fwatt_val;
	/* Variable storing the FVAR value */
	uint32_t			fvar_val;
	/* Variable storing the FVA value */
	uint32_t			fva_val;
	/* Variable storing the WATTHR value */
	uint32_t			watthr_val;
	/* Variable storing the VARHR value */
	uint32_t			varhr_val;
	/* Variable storing the FWATTHR value */
	uint32_t			fwatthr_val;
	/* Variable storing the FVARHR value */
	uint32_t			fvarhr_val;
	/* Variable storing the FVARAHR value*/
	uint32_t			fvarahr_val;
	/* Variable storing the PF value */
	uint32_t			pf_val;
	/* Variable storing the FPF value */
	uint32_t			fpf_val;
	/* Variable storing the IRMS value */
	uint32_t			irms_val;
	/* Variable storing the VRMS value */
	uint32_t			vrms_val;
	/* Variable for mode selection */
	uint8_t             power_mode;
	/* Nominal frequency */
	uint8_t				freq;
	/* Temperature enable flag */
	uint8_t				temp_en;
	/* Temperature offset */
	uint32_t				temp_offset;
	/* Temperature gain */
	uint32_t				temp_gain;
	/** Variable storing the temperature value */
	int32_t				temp_raw;
};

// total active power registers for each phase
static const uint16_t watt_regs[] = {ADE9039_REG_AWATT, ADE9039_REG_BWATT, ADE9039_REG_CWATT};
// total apparent power registers for each phase
static const uint16_t va_regs[] = {ADE9039_REG_AVA, ADE9039_REG_BVA, ADE9039_REG_CVA};
// fundamental active power registers for each phase
static const uint16_t fwatt_regs[] = {ADE9039_REG_AFWATT, ADE9039_REG_BFWATT, ADE9039_REG_CFWATT};
// fundamental reactive power registers for each phase
static const uint16_t fvar_regs[] = {ADE9039_REG_AFVAR, ADE9039_REG_BFVAR, ADE9039_REG_CFVAR};
// fundamental apparent power registers for each phase
static const uint16_t fva_regs[] = {ADE9039_REG_AFVA, ADE9039_REG_BFVA, ADE9039_REG_CFVA};
// total power factor registers for each phase
static const uint16_t pf_regs[] = {ADE9039_REG_APF, ADE9039_REG_BPF, ADE9039_REG_CPF};
// fundamental power factor registers for each phase
static const uint16_t fpf_regs[] = {ADE9039_REG_AFPF, ADE9039_REG_BFPF, ADE9039_REG_CFPF};
// accumulated total active energy registers for each phase
static const uint16_t watthr_hi_regs[] = {ADE9039_REG_AWATTHR_HI, ADE9039_REG_BWATTHR_HI, ADE9039_REG_CWATTHR_HI};
// accumulated total apparent energy registers for each phase
static const uint16_t varhr_hi_regs[] = {ADE9039_REG_AVAHR_HI, ADE9039_REG_BVAHR_HI, ADE9039_REG_CVAHR_HI};
// accumulated fundamental active energy registers for each phase
static const uint16_t fwatthr_hi_regs[] = {ADE9039_REG_AFWATTHR_HI, ADE9039_REG_BFWATTHR_HI, ADE9039_REG_CFWATTHR_HI};
// accumulated fundamental reactive energy registers for each phase
static const uint16_t fvarhr_hi_regs[] = {ADE9039_REG_AFVARHR_HI, ADE9039_REG_BFVARHR_HI, ADE9039_REG_CFVARHR_HI};
// accumulated fundamental apparent energy registers for each phase
static const uint16_t fvarahr_hi_regs[] = {ADE9039_REG_AFVAHR_HI, ADE9039_REG_BFVAHR_HI, ADE9039_REG_CFVAHR_HI};
// voltage rms registers for each phase
static const uint16_t vrms_regs[] = {ADE9039_REG_AVRMS, ADE9039_REG_BVRMS, ADE9039_REG_CVRMS};
// current rms registers for each phase
static const uint16_t irms_regs[] = {ADE9039_REG_AIRMS, ADE9039_REG_BIRMS, ADE9039_REG_CIRMS};

/* Read device register. */
int ade9039_read(struct ade9039_dev *dev, uint16_t reg_addr,
		 uint32_t *reg_data);

/* Write device register. */
int ade9039_write(struct ade9039_dev *dev, uint16_t reg_addr,
		  uint32_t reg_data);

/* Set power mode */
int ade9039_set_power_mode(struct ade9039_dev *dev);

/* Update specific register bits. */
int ade9039_update_bits(struct ade9039_dev *dev, uint16_t reg_addr,
			uint32_t mask, uint32_t reg_data);

/* Read the rms values for a specific phase. */
int ade9039_read_rms_ph(struct ade9039_dev *dev, enum ade9039_phase phase);

/* Read the power values for a specific phase. */
int ade9039_read_power_ph(struct ade9039_dev *dev, enum ade9039_phase phase);

/* Read the energy values for a specific phase. */
int ade9039_read_energy_ph(struct ade9039_dev *dev, enum ade9039_phase phase);

/* Read the power factor for a specific phase. */
int ade9039_read_power_factor_ph(struct ade9039_dev *dev,
				 enum ade9039_phase phase);

/* Set User Energy use model */
int ade9039_set_egy_model(struct ade9039_dev *dev, enum ade9039_egy_model model,
			  uint16_t value);

/* Initialize the device. */
int ade9039_init(struct ade9039_dev **device,
		 struct ade9039_init_param init_param);

/* Setup the device */
int ade9039_setup(struct ade9039_dev *dev);

/* Remove the device and release resources. */
int ade9039_remove(struct ade9039_dev *dev);

/* Read temperature */
int ade9039_read_temp(struct ade9039_dev *dev);

/* Get interrupt indicator from STATUS0 register. */
int ade9039_get_int_status0(struct ade9039_dev *dev, uint32_t msk,
			    uint8_t *status);

int ade9039_check_bit_status(struct ade9039_dev *dev, uint16_t reg_addr,
			     uint32_t bit_mask, uint8_t *status);

#endif // __ADE9039_H__
