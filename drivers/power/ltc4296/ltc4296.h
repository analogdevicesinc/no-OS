/***************************************************************************//**
 *   @file   ltc4296.h
 *   @brief  Header file for the LTC4296 Driver
 *   @author Antoniu Miclaus (antoniu.miclaus@analog.com)
********************************************************************************
 * Copyright 2024(c) Analog Devices, Inc.
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
*******************************************************************************/

#ifndef __LTC4296_H__
#define __LTC4296_H__

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <stdint.h>
#include "no_os_util.h"
#include "no_os_spi.h"
#include "no_os_gpio.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define LTC4296_REG_GFLTEV        	0x02
#define LTC4296_REG_GFLTMSK       	0x03
#define LTC4296_REG_GCAP          	0x06
#define LTC4296_REG_GIOST         	0x07
#define LTC4296_REG_GCMD          	0x08
#define LTC4296_REG_GCFG          	0x09
#define LTC4296_REG_GADCCFG       	0x0A
#define LTC4296_REG_GADCDAT       	0x0B
#define LTC4296_REG_P0EV          	0x10
#define LTC4296_REG_P0ST          	0x12
#define LTC4296_REG_P0CFG0        	0x13
#define LTC4296_REG_P0CFG1        	0x14
#define LTC4296_REG_P0ADCCFG      	0x15
#define LTC4296_REG_P0ADCDAT      	0x16
#define LTC4296_REG_P0SELFTEST    	0x17
#define LTC4296_REG_P1EV          	0x20
#define LTC4296_REG_P1ST          	0x22
#define LTC4296_REG_P1CFG0        	0x23
#define LTC4296_REG_P1CFG1        	0x24
#define LTC4296_REG_P1ADCCFG      	0x25
#define LTC4296_REG_P1ADCDAT      	0x26
#define LTC4296_REG_P1SELFTEST    	0x27
#define LTC4296_REG_P2EV          	0x30
#define LTC4296_REG_P2ST          	0x32
#define LTC4296_REG_P2CFG0        	0x33
#define LTC4296_REG_P2CFG1       	0x34
#define LTC4296_REG_P2ADCCFG      	0x35
#define LTC4296_REG_P2ADCDAT      	0x36
#define LTC4296_REG_P2SELFTEST   	0x37
#define LTC4296_REG_P3EV          	0x40
#define LTC4296_REG_P3ST          	0x42
#define LTC4296_REG_P3CFG0        	0x43
#define LTC4296_REG_P3CFG1        	0x44
#define LTC4296_REG_P3ADCCFG      	0x45
#define LTC4296_REG_P3ADCDAT     	0x46
#define LTC4296_REG_P3SELFTEST    	0x47
#define LTC4296_REG_P4EV          	0x50
#define LTC4296_REG_P4ST          	0x52
#define LTC4296_REG_P4CFG0        	0x53
#define LTC4296_REG_P4CFG1      	0x54
#define LTC4296_REG_P4ADCCFG      	0x55
#define LTC4296_REG_P4ADCDAT     	0x56
#define LTC4296_REG_P4SELFTEST    	0x57

/* LTC4296_REG_GFLTEV */
#define LTC4296_UVLO_DIGITAL_MSK	NO_OS_BIT(4)
#define LTC4296_COMMAND_FAULT_MSK	NO_OS_BIT(3)
#define LTC4296_PEC_FAULT_MSK		NO_OS_BIT(2)
#define LTC4296_MEMORY_FAULT_MSK	NO_OS_BIT(1)
#define LTC4296_LOW_CKT_BRK_FAULT_MSK	NO_OS_BIT(0)

/* LTC4296_REG_GCAP */
#define LTC4296_SCCP_SUPPORT_MSK             NO_OS_BIT(6)
#define LTC4296_WAKE_FWD_SUPPORT_MSK         NO_OS_BIT(5)
#define LTC4296_NUMPORTS_MSK                 NO_OS_GENMASK(4, 0)

/* LTC4296_REG_GIOST */
#define LTC4296_PG_OUT4_MSK			NO_OS_BIT(8)
#define LTC4296_PG_OUT3_MSK			NO_OS_BIT(7)
#define LTC4296_PG_OUT2_MSK			NO_OS_BIT(6)
#define LTC4296_PG_OUT1_MSK			NO_OS_BIT(5)
#define LTC4296_PG_OUT0_MSK			NO_OS_BIT(4)
#define LTC4296_PAD_AUTO_MSK			NO_OS_BIT(3)
#define LTC4296_PAD_WAKEUP_MSK			NO_OS_BIT(2)
#define LTC4296_PAD_WAKEUP_DRIVE_MSK		NO_OS_BIT(1)

/* LTC4296_REG_GCMD */
#define LTC4296_SW_RESET_MSK			NO_OS_GENMASK(15, 8)
#define LTC4296_WRITE_PROTECT_MSK		NO_OS_GENMASK(7, 0)

/* LTC4296_REG_GCFG */
#define LTC4296_MASK_LOWFAULT_MSK		NO_OS_BIT(5)
#define LTC4296_TLIM_DISABLE_MSK		NO_OS_BIT(4)
#define LTC4296_TLIM_TIMER_SLEEP_MSK		NO_OS_GENMASK(3, 2)
#define LTC4296_REFRESH_MSK			NO_OS_BIT(1)
#define LTC4296_SW_VIN_PGOOD_MSK		NO_OS_BIT(0)

/* LTC4296_REG_GADCCFG */
#define LTC4296_GADC_SAMPLE_MODE_MSK		NO_OS_GENMASK(6, 5)
#define LTC4296_GADC_SEL_MSK			NO_OS_GENMASK(4, 0)

/* LTC4296_REG_GADCDAT */
#define LTC4296_GADC_MISSED_MSK			NO_OS_BIT(13)
#define LTC4296_GADC_NEW_MSK			NO_OS_BIT(12)
#define LTC4296_GADC_MSK			NO_OS_GENMASK(11, 0)

/* LTC4296_REG_PXEV */
#define LTC4296_VALID_SIGNATURE_MSK		NO_OS_BIT(9)
#define LTC4296_INVALID_SIGNATURE_MSK		NO_OS_BIT(8)
#define LTC4296_TOFF_TIMER_DONE_MSK		NO_OS_BIT(7)
#define LTC4296_OVERLOAD_DETECTED_ISLEEP_MSK	NO_OS_BIT(6)
#define LTC4296_OVERLOAD_DETECTED_IPOWERED_MSK	NO_OS_BIT(5)
#define LTC4296_MFVS_TIMEOUT_MSK		NO_OS_BIT(4)
#define LTC4296_TINRUSH_TIMER_DONE_MSK		NO_OS_BIT(3)
#define LTC4296_PD_WAKEUP_MSK			NO_OS_BIT(2)
#define LTC4296_LSNS_FORWARD_FAULT_MSK		NO_OS_BIT(1)
#define LTC4296_LSNS_REVERSE_FAULT_MSK		NO_OS_BIT(0)

/* LTC4296_REG_PXST */
#define LTC4296_DET_VHIGH_MSK			NO_OS_BIT(13)
#define LTC4296_DET_VLOW_MSK			NO_OS_BIT(12)
#define LTC4296_POWER_STABLE_HI_MSK		NO_OS_BIT(11)
#define LTC4296_POWER_STABLE_LO_MSK		NO_OS_BIT(10)
#define LTC4296_POWER_STABLE_MSK		NO_OS_BIT(9)
#define LTC4296_OVERLOAD_HELD_MSK		NO_OS_BIT(8)
#define LTC4296_PI_SLEEPING_MSK			NO_OS_BIT(7)
#define LTC4296_PI_PREBIASED_MSK		NO_OS_BIT(6)
#define LTC4296_PI_DETECTING_MSK		NO_OS_BIT(5)
#define LTC4296_PI_POWERED_MSK			NO_OS_BIT(4)
#define LTC4296_PI_DISCHARGE_EN_MSK		NO_OS_BIT(3)
#define LTC4296_PSE_STATUS_MSK			NO_OS_GENMASK(2, 0)

/* LTC4296_REG_PXCFG0 */
#define LTC4296_SW_INRUSH_MSK			NO_OS_BIT(15)
#define LTC4296_END_CLASSIFICATION_MSK		NO_OS_BIT(14)
#define LTC4296_SET_CLASSIFICATION_MODE_MSK	NO_OS_BIT(13)
#define LTC4296_DISABLE_DETECTION_PULLUP_MSK	NO_OS_BIT(12)
#define LTC4296_TDET_DISABLE_MSK		NO_OS_BIT(11)
#define LTC4296_FOLDBACK_DISABLE_MSK		NO_OS_BIT(10)
#define LTC4296_SOFT_START_DISABLE_MSK		NO_OS_BIT(9)
#define LTC4296_TOFF_TIMER_DISABLE_MSK		NO_OS_BIT(8)
#define LTC4296_TMFVDO_TIMER_DISABLE_MSK	NO_OS_BIT(7)
#define LTC4296_SW_PSE_READY_MSK		NO_OS_BIT(6)
#define LTC4296_SW_POWER_AVAILABLE_MSK		NO_OS_BIT(5)
#define LTC4296_UPSTREAM_WAKEUP_DISABLE_MSK	NO_OS_BIT(4)
#define LTC4296_DOWNSTREAM_WAKEUP_DISABLE_MSK	NO_OS_BIT(3)
#define LTC4296_SW_PSE_WAKEUP_MSK		NO_OS_BIT(2)
#define LTC4296_HW_EN_MASK_MSK			NO_OS_BIT(1)
#define LTC4296_SW_EN_MSK			NO_OS_BIT(0)

/* LTC4296_REG_PXCFG1 */
#define LTC4296_PREBIAS_OVERRIDE_GOOD_MSK	NO_OS_BIT(8)
#define LTC4296_TLIM_TIMER_TOP_MSK		NO_OS_GENMASK(7, 6)
#define LTC4296_TOD_TRESTART_TIMER_MSK		NO_OS_GENMASK(5, 4)
#define LTC4296_TINRUSH_TIMER_MSK		NO_OS_GENMASK(3, 2)
#define LTC4296_SIG_OVERRIDE_BAD_MSK		NO_OS_BIT(1)
#define LTC4296_SIG_OVERRIDE_GOOD_MSK		NO_OS_BIT(0)

/* LTC4296_REG_PXADCCFG */
#define LTC4296_MFVS_THRESHOLD_MSK		NO_OS_GENMASK(7, 0)

/* LTC4296_REG_PXADCDAT */
#define LTC4296_MISSED_MSK			NO_OS_BIT(13)
#define LTC4296_NEW_MSK				NO_OS_BIT(12)
#define LTC4296_SOURCE_CURRENT_MSK		NO_OS_GENMASK(11, 0)

/* Miscellaneous Definitions*/
#define LTC4296_SPI_READ			0x01
#define LTC4296_SPI_WRITE			0x00

#define LTC4296_RESET_CODE			0x73
#define LTC4296_UNLOCK_KEY			0x05
#define LTC4296_LOCK_KEY			0xA0

#define LTC4296_ADC_OFFSET			2049

#define LTC4296_VGAIN				35230 / 1000
#define LTC4296_IGAIN				1 / 10

#define LTC4296_VMAX         			1
#define LTC4296_VMIN        			0

#define LTC4296_MAX_PORTS			5

#define RTESTLOAD              			200 /*(ohm)*/

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @brief Device Structure
 */
struct ltc4296_dev {
	struct no_os_spi_desc *spi_desc;
};

/**
 * @brief Initialization Structure
 */
struct ltc4296_init_param {
	struct no_os_spi_init_param *spi_init;
};

/**
 * @brief Device State
 */
enum ltc4296_state {
	LTC_UNLOCKED = 0,
	LTC_LOCKED
};

/**
 * @brief Device Port Numbers
 */
enum ltc4296_port {
	LTC_PORT0 = 0,
	LTC_PORT1,
	LTC_PORT2,
	LTC_PORT3,
	LTC_PORT4,
	LTC_NO_PORT
};

/**
 * @brief Device Port Status
 */
enum ltc4296_port_status {
	LTC_PORT_DISABLED = 0,
	LTC_PORT_ENABLED
};

/**
 * @brief Device PSE Status
 */
enum ltc4296_pse_status {
	LTC_PSE_STATUS_DISABLED = 0,      /*  000b Port is disabled                */
	LTC_PSE_STATUS_SLEEPING,          /*  001b Port is in sleeping             */
	LTC_PSE_STATUS_DELIVERING,        /*  010b Port is delivering power        */
	LTC_PSE_STATUS_SEARCHING,         /*  011b Port is searching               */
	LTC_PSE_STATUS_ERROR,             /*  100b Port is in error                */
	LTC_PSE_STATUS_IDLE,              /*  101b Port is idle                    */
	LTC_PSE_STATUS_PREPDET,           /*  110b Port is preparing for detection */
	LTC_PSE_STATUS_UNKNOWN            /*  111b Port is in an unknown state     */
};

/**
 * @brief Board Classes
 */
enum ltc4296_board_class {
	SPOE_CLASS10 = 0,
	SPOE_CLASS11,
	SPOE_CLASS12,
	SPOE_CLASS13,
	SPOE_CLASS14,
	SPOE_CLASS15,
	APL_CLASSA,
	APL_CLASSA_NOAUTONEG,
	APL_CLASSC,
	APL_CLASS3,
	PRODUCTION_POWER_TEST,
	APL_CLASSA_OLD_DEMO,
	SPOE_OFF,
	PRODUCTION_DATA_TEST,
	RESERVED,
	DEBUGMODE
};

/**
 * @brief Device Configuration Modes
 */
enum ltc4296_config {
	LTC_CFG_SCCP_MODE = 0,
	LTC_CFG_APL_MODE,
	LTC_CFG_RESET
};

/**
 * @brief Device Port Register offsets
 */
enum ltc4296_port_reg_offset_e {
	LTC_PORT_EVENTS   = 0,
	LTC_PORT_STATUS   = 2,
	LTC_PORT_CFG0     = 3,
	LTC_PORT_CFG1     = 4,
	LTC_PORT_ADCCFG   = 5,
	LTC_PORT_ADCDAT   = 6,
	LTC_PORT_SELFTEST = 7
};

/**
 * @brief Device Result Codes
 */
enum adi_ltc_result {
	ADI_LTC_SUCCESS = 0,                  /*!< Success                                                    */
	ADI_LTC_DISCONTINUE_SCCP,             /*!< Discontinue the SCCP configuration cycle.                  */
	ADI_LTC_SCCP_COMPLETE,                /*!< Complete SCCP configuration cycle.                         */
	ADI_LTC_SCCP_PD_DETECTION_FAILED,     /*!< PD Detection failed                                        */
	ADI_LTC_SCCP_PD_NOT_PRESENT,          /*!< SCCP  PD not present                                       */
	ADI_LTC_SCCP_PD_RES_INVALID,          /*!< PD Response is invalid                                     */
	ADI_LTC_SCCP_PD_PRESENT,              /*!< PD is present.                                             */
	ADI_LTC_SCCP_PD_CLASS_COMPATIBLE,     /*!< PD Class is compatible                                     */
	ADI_LTC_SCCP_PD_CLASS_NOT_SUPPORTED,  /*!< PD Class is out of range                                   */
	ADI_LTC_SCCP_PD_CLASS_NOT_COMPATIBLE, /*!< PD Class is not compatible                                 */
	ADI_LTC_SCCP_PD_LINE_NOT_HIGH,        /*!< PD line has not gone HIGH                                  */
	ADI_LTC_SCCP_PD_LINE_NOT_LOW,         /*!< PD line has not gone LOW                                   */
	ADI_LTC_SCCP_PD_CRC_FAILED,           /*!< CRC received from PD is incorrect                          */
	ADI_LTC_APL_COMPLETE,                 /*!< Complete APL configuration cycle.                          */
	ADI_LTC_DISCONTINUE_APL,              /*!< Discontinue the APL configuration cycle.                   */
	ADI_LTC_INVALID_ADC_VOLTAGE,          /*!< Invalid ADC Accumulation result.                           */
	ADI_LTC_INVALID_ADC_PORT_CURRENT,     /*!< Invalid ADC Port Current                                   */
	ADI_LTC_TEST_COMPLETE,                /*!< LTC Test complete.                                         */
	ADI_LTC_DISCONTINUE_TEST,             /*!< LTC Discontinue Test.                                      */
	ADI_LTC_TEST_FAILED,                  /*!< LTC Test Failed.                                           */
	ADI_LTC_INVALID_VIN                   /*!< VIN is invalid                                             */
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Register Write */
int ltc4296_reg_write(struct ltc4296_dev *dev, uint8_t reg, uint16_t data);

/** Register Read */
int ltc4296_reg_read(struct ltc4296_dev *dev, uint8_t reg, uint16_t *data);

/** Register Write */
int ltc4296_reset(struct ltc4296_dev *dev);

/** Get port address */
int ltc4296_get_port_addr(enum ltc4296_port port_no,
			  enum ltc4296_port_reg_offset_e port_offset,
			  uint8_t *port_addr);

/** Clear global faults */
int ltc4296_clear_global_faults(struct ltc4296_dev *dev);

/** Clear circuit break faults */
int ltc4296_clear_ckt_breaker(struct ltc4296_dev *dev);

/** Read global faults */
int ltc4296_read_global_faults(struct ltc4296_dev *dev, uint16_t *g_events);

/** Device unlock */
int ltc4296_unlock(struct ltc4296_dev *dev);

/** Check device state */
int ltc4296_is_locked(struct ltc4296_dev *dev, enum ltc4296_state *state);

/** Read global ADC */
int ltc4296_read_gadc(struct ltc4296_dev *dev, int *port_voltage_mv);

/** Set the global ADC to measure voltage */
int ltc4296_set_gadc_vin(struct ltc4296_dev *dev);

/** Check if input voltage is valid */
int ltc4296_is_vin_valid(struct ltc4296_dev *dev, int port_vin_mv,
			 enum ltc4296_board_class ltcboard_class, bool *vin_valid);

/** Check if output voltage is valid. */
int ltc4296_is_vout_valid(struct ltc4296_dev *dev, int port_vout_mv,
			  enum ltc4296_board_class ltcboard_class, bool *vout_valid);

/** Disable the global ADC */
int ltc4296_disable_gadc(struct ltc4296_dev *dev);

/** Read Port Events */
int ltc4296_read_port_events(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_events);

/** Clear Port Events */
int ltc4296_clear_port_events(struct ltc4296_dev *dev,
			      enum ltc4296_port port_no);

/** Read Port Status */
int ltc4296_read_port_status(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     uint16_t *port_status);

/** Check if port is enabled/disabled */
int ltc4296_is_port_disabled(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			     enum ltc4296_port_status *port_chk);

/** Check if port is disabled */
int ltc4296_port_disable(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Check if port delivers power */
int ltc4296_is_port_deliver_pwr(struct ltc4296_dev *dev,
				enum ltc4296_port port_no,
				enum ltc4296_pse_status *pwr_status);

/** Check if port has stable power */
int ltc4296_is_port_pwr_stable(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no, bool *pwr_status);

/** Read port ADC */
int ltc4296_read_port_adc(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			  int *port_i_out_ma);

/** Stimulate valid wake-up signature for a port */
int ltc4296_port_prebias(struct ltc4296_dev *dev, enum ltc4296_port port_no,
			 enum ltc4296_config mode);

/** Enable specific port */
int ltc4296_port_en(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Enable specific port and classify */
int ltc4296_port_en_and_classification(struct ltc4296_dev *dev,
				       enum ltc4296_port port_no);

/** Set port to maintain full voltage signature. */
int ltc4296_set_port_mfvs(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Set port to power mode. */
int ltc4296_set_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Force port to power mode. */
int ltc4296_force_port_pwr(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Set port is able to source power. */
int ltc4296_port_pwr_available(struct ltc4296_dev *dev,
			       enum ltc4296_port port_no);

/** Configure Global ADC to read output voltage */
int ltc4296_set_gadc_vout(struct ltc4296_dev *dev, enum ltc4296_port port_no);

/** Print global faults */
int ltc4296_print_global_faults(uint16_t g_events);

/** Print port events */
int ltc4296_print_port_events(enum ltc4296_port port_no, uint16_t port_events);

/** Check global events */
int ltc4296_chk_global_events(struct ltc4296_dev *dev);

/** Check port events */
int ltc4296_chk_port_events(struct ltc4296_dev *dev,
			    enum ltc4296_port ltc4296_port);

/** Device initialization */
int ltc4296_init(struct ltc4296_dev** device,
		 struct ltc4296_init_param* init_param);

/** Free allocated resources */
int ltc4296_remove(struct ltc4296_dev* dev);

#endif
