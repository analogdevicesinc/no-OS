/***************************************************************************//**
 *   @file   admt4000.h
 *   @brief  Header Implementation of ADMT4000 Driver.
 *   @author Marc Sosa (marcpaolo.sosa@analog.com)
 *   @author Jose Ramon San Buenaventura (jose.sanbuenaventura@analog.com)
 *   @author Louijie Compo  (louijie.compo@analog.com)
********************************************************************************
 * Copyright 2020(c) Analog Devices, Inc.
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

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __ADMT4000_H__
#define __ADMT4000_H__

#include <stdlib.h>
#include <stdbool.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"
#include "no_os_util.h"

/*ADMT4000 Registers*/
#define ADMT4000_REG_RST                0x00

/*ADMT4000 Agnostic Page Registers*/
#define ADMT4000_AGP_REG_CNVPAGE        0x01
#define ADMT4000_AGP_REG_ABSANGLE       0x03
#define ADMT4000_AGP_REG_DGIO           0x04
#define ADMT4000_AGP_REG_ANGLE          0x05
#define ADMT4000_AGP_REG_FAULT          0x06
#define ADMT4000_AGP_REG_ANGLESEC       0x08

/*ADMT 0x00 Page Registers*/
#define ADMT4000_RAW_ANGLE_REG(x)       (0x10 + (x))
#define ADMT4000_00_REG_SINE       		0x10
#define ADMT4000_00_REG_COSINE       	0x11
#define ADMT4000_00_REG_RADIUS          0x18
#define ADMT4000_00_REG_DIAG1           0x1D
#define ADMT4000_00_REG_DIAG2           0x1E
#define ADMT4000_00_REG_TMP0            0x20
#define ADMT4000_00_REG_TMP1            0x23

/*ADMT 0x02 Page Registers*/
#define ADMT4000_02_REG_GENERAL         0x10 // ECC0
#define ADMT4000_02_REG_DIGIOEN         0x12 // ECC0
#define ADMT4000_02_REG_ANGLECK         0x13 // ECC0
#define ADMT4000_02_REG_CNVCNT          0x14
#define ADMT4000_02_REG_H1MAG           0x15 // ECC0
#define ADMT4000_02_REG_H1PH            0x16 // ECC0
#define ADMT4000_02_REG_H2MAG           0x17 // ECC0
#define ADMT4000_02_REG_H2PH            0x18 // ECC0
#define ADMT4000_02_REG_H3MAG           0x19 // ECC1/0
#define ADMT4000_02_REG_H3PH            0x1A // ECC1
#define ADMT4000_02_REG_H8MAG           0x1B // ECC1
#define ADMT4000_02_REG_H8PH            0x1C // ECC1
#define ADMT4000_02_REG_ECCEDC          0x1D
#define ADMT4000_02_REG_UNIQD0          0x1E
#define ADMT4000_02_REG_UNIQD1          0x1F
#define ADMT4000_02_REG_UNIQD2          0x20
#define ADMT4000_02_REG_UNIQD3          0x21
#define ADMT4000_02_REG_ECCDIS          0x23

/* ADMT4000 Other Macros, Masks etc. */
#define ADMT4000_RW_MASK                NO_OS_GENMASK(5, 0)
#define ADMT4000_WR_EN                  NO_OS_BIT(6)
#define ADMT4000_FAULT_MASK             NO_OS_BIT(7)
#define ADMT4000_LIFE_CTR               NO_OS_GENMASK(6, 5)
#define ADMT4000_RCV_CRC                NO_OS_GENMASK(4, 0)

#define ADMT4000_MIN_PAGE               0x00
#define ADMT4000_MAX_PAGE               0x02

/* Upper and Lower Byte Masking */
#define ADMT4000_LOW_BYTE               NO_OS_GENMASK(7, 0)
#define ADMT4000_HI_BYTE                NO_OS_GENMASK(15, 8)

/* Register 01 */
#define ADMT4000_CNV_EDGE_MASK          NO_OS_GENMASK(15, 14)
#define ADMT4000_PAGE_MASK              NO_OS_GENMASK(4, 0)
#define ADMT4000_FALLING_EDGE           0x00
#define ADMT4000_RISING_EDGE            0x3

/* Register 03 */
#define ADMT4000_TURN_CNT_MASK          NO_OS_GENMASK(15, 10)
//#define ADMT4000_TURN_CNT_MASK          NO_OS_GENMASK(15, 8)
#define ADMT4000_ABS_ANGLE_MASK         NO_OS_GENMASK(9, 0)
//#define ADMT4000_INVALID_TURN           0x36
#define ADMT4000_INVALID_TURN           0x53

/* Register 04*/
#define ADMT4000_MAX_GPIO_INDEX         5
#define ADMT4000_GPIO_LOGIC(x)          NO_OS_BIT(x)

/* Register 05 and 08 */
#define ADMT4000_ANGLE_MASK             NO_OS_GENMASK(15, 4)

/* Register 06 */
#define ADMT4000_ALL_FAULTS             NO_OS_GENMASK(15, 0)
#define ADMT4000_AGP_INDIV_FAULT(x)     NO_OS_BIT((x))

/* Register 0x08/10/11/12/13/18, page 0 */
#define ADMT4000_ANGLE_STAT_MASK        NO_OS_BIT(0)

/* Register 0x10/11/12/13, page 0 */
#define ADMT4000_RAW_ANGLE_MASK         NO_OS_GENMASK(15, 2)
#define ADMT4000_RAW_COSINE_MASK        NO_OS_GENMASK(15, 2)
#define ADMT4000_RAW_SINE_MASK        	NO_OS_GENMASK(15, 2)

/* Register 18, page 0 */
#define ADMT4000_RADIUS_MASK            NO_OS_GENMASK(15, 1)

/* Register 0x1D, page 0 */
#define ADMT4000_MTDIAG1_MASK           NO_OS_GENMASK(15, 8)
#define ADMT4000_AFEDIAG2_MASK          NO_OS_GENMASK(7, 0)

/* Register 0x1E, page 0 */
#define ADMT4000_AFEDIAG1_MASK          NO_OS_GENMASK(15, 8)
#define ADMT4000_REF_RES(x)             NO_OS_BIT(8 + (x))
#define ADMT4000_AFEDIAG0_MASK          NO_OS_GENMASK(7, 0)

/* Register 0x20, page 0 */
#define ADMT4000_TEMP_MASK              NO_OS_GENMASK(15, 4)

/* Register 0x10, page 2 */
#define ADMT4000_STORAGE_MSB_XTRACT     NO_OS_BIT(7)
#define ADMT4000_STORAGE_BIT6_XTRACT    NO_OS_BIT(6)
#define ADMT4000_STORAGE_MASK1_XTRACT   NO_OS_GENMASK(5, 3)
#define ADMT4000_STORAGE_MASK0_XTRACT   NO_OS_GENMASK(2, 0)
#define ADMT4000_STORAGE_MSB            NO_OS_BIT(15)
#define ADMT4000_STORAGE_BIT6           NO_OS_BIT(11)
#define ADMT4000_STORAGE_MASK1          NO_OS_GENMASK(8, 6)
#define ADMT4000_STORAGE_MASK0          NO_OS_GENMASK(3, 1)
#define ADMT4000_STORAGE_MASK_FULL      ADMT4000_STORAGE_MSB | \
                                        ADMT4000_STORAGE_BIT6 | \
                                        ADMT4000_STORAGE_MASK1 | \
                                        ADMT4000_STORAGE_MASK0

#define ADMT4000_CNV_SYNC_MASK          NO_OS_GENMASK(14, 13)
#define ADMT4000_ANGL_FILT_MASK         NO_OS_BIT(12)
#define ADMT4000_H8_CTRL_MASK           NO_OS_BIT(10)
#define ADMT4000_SEQ_MODE_MASK          NO_OS_GENMASK(5, 4)
#define ADMT4000_CNV_MODE_MASK          NO_OS_BIT(0)

/* Register 0x12, page 2 */
#define ADMT4000_DIG_IO_EN(x)           NO_OS_BIT(8 + (x))
#define ADMT4000_GPIO_FUNC(x)           NO_OS_BIT(x)

/* Register 0x13, page 2 */
#define ADMT4000_ANGL_CHK_MASK          NO_OS_GENMASK(9, 0)

/* Register 0x13, page 2 */
#define ADMT4000_CNV_CTR_MASK           NO_OS_GENMASK(7, 0)

/* Register 0x15 / 0x18, page 2 */
#define ADMT4000_H_11BIT_MAG_MASK       NO_OS_GENMASK(10, 0)
#define ADMT4000_11BIT_MAX              2047
#define ADMT4000_H_12BIT_PHA_MASK       NO_OS_GENMASK(11, 0)
#define ADMT4000_12BIT_MAX              4095

/* Register 0x1A / 0x1B, page 2 */
#define ADMT4000_H_8BIT_MAG_MASK        NO_OS_GENMASK(7, 0)
#define ADMT4000_8BIT_MAX               127

/* Register 0x1D, page 2 */
#define ADMT4000_ECC_CFG1               NO_OS_GENMASK(15, 8)
#define ADMT4000_ECC_CFG0               NO_OS_GENMASK(7, 0)

/* Register 0x1E 0x1F 0x20 0x21 , page 2 */
#define ADMT4000_ID0_MASK               NO_OS_GENMASK(14, 0)
#define ADMT4000_ID_PROD_MASK           NO_OS_GENMASK(10, 8)
#define ADMT4000_ID_SUPPLY_MASK         NO_OS_GENMASK(7, 6)
#define ADMT4000_ID_ASIL_MASK           NO_OS_GENMASK(5, 3)
#define ADMT4000_ID_SIL_REV_MAS         NO_OS_GENMASK(2, 0)

/* Register 0x23, page 2 */
#define ADMT4000_ECC_EN_COMM            0x0000
#define ADMT4000_ECC_DIS_COMM           0x4D54

/* Scaling factor */
#define ADMT4000_SF                10000000 // 10M

/* Angle to degree conversion */
#define ADMT4000_ABS_ANGLE_RES          3515625 // (360.0 / 1024.0 * ADMT4000_SF) 
#define ADMT4000_ANGLE_RES              878906 // (360.0 / 4096.0  * ADMT4000_SF) Truncated

/* Turn Count Conversion */
#define ADMT4000_TURN_CNT_THRES         0x35
#define ADMT4000_TURN_CNT_TWOS          64

/* Conversion Factors */
#define ADMT4000_RADIUS_RES             9240 // (0.000924 * ADMT4000_SF) // mV/V unit
#define ADMT4000_FIXED_VOLT_3P3V_RES    32220 // (0.003222 * ADMT4000_SF) // mV/bit
#define ADMT4000_FIXED_VOLT_5V_RES      48828 // (0.0048828 * ADMT4000_SF) // mV/bit
#define ADMT4000_8BIT_THRES             0x7f // 2's complement threhsold
#define ADMT4000_8BIT_TWOS              0x100 // 2's complement factor
#define ADMT4000_DIAG_RESISTOR_RES      7812500 // (0.78125 * ADMT4000_SF) // in percent
#define ADMT4000_CORDIC_SCALER          6072000 // (0.6072 * ADMT4000_SF) // for hmag hpha
#define ADMT4000_HMAG_RES               54930 // (0.005493 * ADMT4000_SF) // hmag resolution
#define ADMT4000_HPHA_RES               878910 // (0.087891 * ADMT4000_SF) // hpha resolution

enum admt4000_faults {
	ADMT4000_FAULT_VDD_UV,
	ADMT4000_FAULT_VDD_OV,
	ADMT4000_FAULT_VDRIVE_UV,
	ADMT4000_FAULT_VDRIVE_OV,
	ADMT4000_FAULT_AFEDIAG,
	ADMT4000_FAULT_NVM_CRC,
	ADMT4000_FAULT_RESERVED,
	ADMT4000_FAULT_ECC_DOUBLE,
	ADMT4000_FAULT_OSC_DRIFT,
	ADMT4000_FAULT_GMR_REF_RES,
	ADMT4000_FAULT_ANGLESEC_RANGE,
	ADMT4000_FAULT_GMR_ELEMENTS,
	ADMT4000_FAULT_MTDIAG,
	ADMT4000_FAULT_TURN_CTRS,
	ADMT4000_FAULT_AMR_RAD_AMP,
	ADMT4000_FAULT_SEQ_WATCHDOG,
};

enum admt4000_vdd {
	ADMT4000_3P3V,
	ADMT4000_5V,
};

enum admt4000_cnv_sync {
	ADMT4000_SEQ_START,
	ADMT4000_SYNC_EDGE = 0x3,
};

enum admt4000_seq_mode {
	ADMT4000_MODE2,
	ADMT4000_MODE1 = 0x3,
};

enum admt4000_angle_type {
	ADMT4000_RAW_SINE,
	ADMT4000_RAW_COSINE,
	ADMT4000_RAW_SECANGLI,
	ADMT4000_RAW_SECANGLEQ,
};

enum admt400_angle_eck_type {
	ADMT4000_ALWAYS_FLAG,
	ADMT4000_VALID,
	ADMT4000_DISABLE_CHECK,
};

struct admt4000_gpio {
	bool logic_state;
	bool is_alt_pin;
};

struct admt4000_dev {
	struct no_os_spi_desc *spi_desc;
	struct no_os_gpio_desc *gpio_coil_rs;
	struct no_os_gpio_desc *gpio_gpio0_busy;
	struct no_os_gpio_desc *gpio_shdn_n;
	struct no_os_gpio_desc *gpio_cnv;
	struct no_os_gpio_desc *gpio_acalc;

	/* Boolean indicator if setup in page 0 or not */
	bool is_page_zero;

	/* Tracker for CNV bits if falling or rising */
	bool edge;

	/* Contains GPIO information: logic level and function*/
	struct admt4000_gpio gpios[6];

	/* VDD Setting indicator: 5V or 3.3V variant only */
	enum admt4000_vdd dev_vdd;

	/* Tracker for conversion type: one shot or continuous */
	bool is_one_shot;

	/* Conversion factor for fixed voltage measurements (uV) */
	uint32_t fixed_conv_factor_mv;

	/* Tracker for type of angle threshold set */
	enum admt400_angle_eck_type eck_type;
};

struct admt4000_init_param {
	struct no_os_spi_init_param spi_init;
	struct no_os_gpio_init_param gpio_coil_rs;
	struct no_os_gpio_init_param gpio_gpio0_busy;
	struct no_os_gpio_init_param gpio_shdn_n;
	struct no_os_gpio_init_param gpio_cnv;
	struct no_os_gpio_init_param gpio_acalc;

	/* Specifier for ADMT4000 variant based on VDD */
	enum admt4000_vdd dev_vdd;
};

int test(char* in_buf, char*out_buf);

/**
 * @brief Initializes the device structure.
 * @param device - The device structure.
 * @param init_param - The initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_init(struct admt4000_dev **device,
		  struct admt4000_init_param init_param);

/**
 * @brief Deallocates the resources for the device structure.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_remove(struct admt4000_dev *device);

/**
 * @brief Compute CRC for given data stream.
 * @param reg_addr - Register address.
 * @param reg_data - Register data.
 * @param excess - Excess bits.
 * @param is_write - Write flag.
 * @param crc_ret - Computed CRC.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_compute_crc(long reg_addr, uint16_t reg_data, uint8_t excess,
			 bool is_write, uint8_t *crc_ret);

/**
 * @brief Encodes the input data using ECC algorithm of admt4000.
 * @param parity_num - Parity number.
 * @param code_length - Code length.
 * @param code - Code.
 * @param input - Input data.
 * @param size_code - Size of code.
 * @param size_input - Size of input data.
 * @param ecc - ECC output.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_ecc_encode(uint8_t *parity_num, uint8_t *code_length,
			uint8_t *code, uint8_t *input, uint8_t size_code, uint8_t size_input,
			uint8_t *ecc);

/**
 * @brief Hamming Calculator to generate the bit value during encoding.
 * @param position - Position.
 * @param code_length - Code length.
 * @param code - Code.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_hamming_calc(uint8_t position,
			  uint8_t code_length, uint8_t *code);

/**
 * @brief Performs error checking on sequence of interest i.e. ECC bytes.
 * @param parity_num - Parity number.
 * @param code_length - Code length.
 * @param code - Code.
 * @param error_pos - Error position.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_error_check(uint8_t parity_num,
			 uint8_t code_length, uint8_t *code, uint8_t *error_pos);

/**
 * @brief Update ECC contents.
 * @param device - The device structure.
 * @param ecc_val - ECC value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_update_ecc(struct admt4000_dev *device, uint16_t *ecc_val);

/**
 * @brief Read from selected register.
 * @param device - The device structure.
 * @param reg_addr - Register address.
 * @param reg_data - Register data.
 * @param verif - Verification data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_read(struct admt4000_dev *device, uint8_t reg_addr,
		  uint16_t *reg_data, uint8_t *verif);

/**
 * @brief Write to selected register.
 * @param device - The device structure.
 * @param reg_addr - Register address.
 * @param reg_data - Register data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_write(struct admt4000_dev *device, uint8_t reg_addr,
		   uint16_t reg_data);

/**
 * @brief Update register fields.
 * @param device - The device structure.
 * @param reg_addr - Register address.
 * @param update_mask - Update mask.
 * @param update_val - Update value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_reg_update(struct admt4000_dev *device, uint8_t reg_addr,
			uint16_t update_mask, uint16_t update_val);

/**
 * @brief Switch the device to either page zero or page two.
 * @param device - The device structure.
 * @param is_page_zero - Page zero flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_page(struct admt4000_dev *device, bool is_page_zero);

/**
 * @brief Get page configuration.
 * @param device - The device structure.
 * @param is_page_zero - Page zero flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_page(struct admt4000_dev *device, bool *is_page_zero);

/**
 * @brief Sets or resets the CNV bits to mimic fall/rising edge.
 * @param device - The device structure.
 * @param is_rising - Rising edge flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_cnv(struct admt4000_dev *device, bool is_rising);

/**
 * @brief Get CNV bits configuration.
 * @param device - The device structure.
 * @param is_rising - Rising edge flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cnv(struct admt4000_dev *device, bool *is_rising);

/**
 * @brief Gets raw data from ABSANGLE and ANGLE registers.
 * @param device - The device structure.
 * @param angle_data - Angle data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_raw_angle_read(struct admt4000_dev *device, uint16_t *angle_data);

/**
 * @brief Parses raw data from ABSANGLE and ANGLE registers.
 * @param device - The device structure.
 * @param turns - Turns data.
 * @param angle - Angle data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_raw_turns_and_angle(struct admt4000_dev *device,
				     uint8_t *turns,
				     uint16_t *angle);

/**
 * @brief Get selected GPIO's logic level.
 * @param device - The device structure.
 * @param gpio - GPIO number.
 * @param logic - Logic level.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_gpio(struct admt4000_dev *device, uint8_t gpio, bool *logic);

/**
 * @brief Set selected GPIO's logic level.
 * @param device - The device structure.
 * @param gpio - GPIO number.
 * @param logic - Logic level.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_gpio(struct admt4000_dev *device, uint8_t gpio, bool logic);

/**
 * @brief Clear all faults in the fault register.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_clear_all_faults(struct admt4000_dev *device);

/**
 * @brief Reads selected fault's status.
 * @param device - The device structure.
 * @param fault - Fault type.
 * @param is_flagged - Fault status.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_read_fault(struct admt4000_dev *device, enum admt4000_faults fault,
			bool *is_flagged);

/**
 * @brief Get secondary angle raw data.
 * @param device - The device structure.
 * @param angle - Angle data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_secondary_angle(struct admt4000_dev *device, uint16_t *angle);

/**
 * @brief Get Secondary Angle Data (Converted, degrees).
 * @param device - The device structure.
 * @param angle - Angle data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_secondary_angle(struct admt4000_dev *device,
		uint32_t *angle);

/**
 * @brief Get angle raw data.
 * @param device - The device structure.
 * @param raw_angle - Raw angle data.
 * @param angle_type - Angle type.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_angle(struct admt4000_dev *device, uint16_t *raw_angle,
		       enum admt4000_angle_type angle_type);

/**
 * @brief Get radius raw data.
 * @param device - The device structure.
 * @param radius - Radius data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_radius(struct admt4000_dev *device, uint16_t *radius);

/**
 * @brief Get Converted Radius Data (mV/V).
 * @param device - The device structure.
 * @param radius - Radius data.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_radius(struct admt4000_dev *device,
				  uint32_t *radius);

/**
 * @brief Obtain status of selected reference resistor (GMR turn count).
 * @param device - The device structure.
 * @param ref_res_num - Reference resistor number.
 * @param is_ok - Status.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_ref_res_state(struct admt4000_dev *device, uint8_t ref_res_num,
			       bool *is_ok);

/**
 * @brief Get fixed value voltage to GMR turn count sensor.
 * @param device - The device structure.
 * @param fixed_volt - Fixed voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_fixed_voltage(struct admt4000_dev *device,
			       uint8_t *fixed_volt);

/**
 * @brief Get fixed value voltage (Volts).
 * @param device - The device structure.
 * @param fixed_volt - Fixed voltage.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_fixed_voltage(struct admt4000_dev *device,
		uint32_t *fixed_volt);

/**
 * @brief Read from either + or - 57% (or both) diagnostic resistor channels.
 * @param device - The device structure.
 * @param diag_meas - Diagnostic measurement.
 * @param is_pos - Positive channel flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_diag_res(struct admt4000_dev *device, uint16_t *diag_meas,
			  uint8_t is_pos);

/**
 * @brief Read percentage value from diagnostic resistors.
 * @param device - The device structure.
 * @param diag_meas - Diagnostic measurement.
 * @param is_pos - Positive channel flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_diag_res(struct admt4000_dev *device,
				    uint32_t *diag_meas,
				    uint8_t is_pos);

/**
 * @brief Read raw temperature (primary or secondary).
 * @param device - The device structure.
 * @param temp - Temperature data.
 * @param is_primary - Primary temperature flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_temp(struct admt4000_dev *device, uint16_t *temp,
		      bool is_primary);

/**
 * @brief Read converted temperature.
 * @param device - The device structure.
 * @param temp - Temperature data.
 * @param is_primary - Primary temperature flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_temp(struct admt4000_dev *device, uint32_t *temp,
				bool is_primary);

/**
 * @brief Obtain the 8 bit equivalent storage bits from general config register.
 * @param device - The device structure.
 * @param storage - Storage configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_storage_config(struct admt4000_dev *device, uint8_t *storage);

/**
 * @brief Set storage config bits.
 * @param device - The device structure.
 * @param storage - Storage configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_storage_config(struct admt4000_dev *device, uint8_t storage);

/**
 * @brief Get conversion synchronization mode from general config register.
 * @param device - The device structure.
 * @param sync - Synchronization mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cnv_sync(struct admt4000_dev *device,
			  enum admt4000_cnv_sync *sync);

/**
 * @brief Set conversion synchronization mode.
 * @param device - The device structure.
 * @param sync - Synchronization mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_cnv_sync(struct admt4000_dev *device,
			  enum admt4000_cnv_sync sync);

/**
 * @brief Check if filter is enabled on angle conversions.
 * @param device - The device structure.
 * @param is_filtered - Filter status.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_angle_filt(struct admt4000_dev *device, bool *is_filtered);

/**
 * @brief Configure filter setting.
 * @param device - The device structure.
 * @param is_filtered - Filter status.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_angle_filt(struct admt4000_dev *device, bool is_filtered);

/**
 * @brief Get 8th harmonic error correction configuration (user or ADI).
 * @param device - The device structure.
 * @param is_user_supplied - User supplied flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_h8_ctrl(struct admt4000_dev *device, bool *is_user_supplied);

/**
 * @brief Configure the source of 8th harmonic error correction (user or ADI).
 * @param device - The device structure.
 * @param is_user_supplied - User supplied flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_h8_ctrl(struct admt4000_dev *device, bool is_user_supplied);

/**
 * @brief Get the sequencer mode configuration.
 * @param device - The device structure.
 * @param seq - Sequencer mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_seq_mode(struct admt4000_dev *device,
			  enum admt4000_seq_mode *seq);

/**
 * @brief Set the sequencer mode configuration.
 * @param device - The device structure.
 * @param seq - Sequencer mode.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_seq_mode(struct admt4000_dev *device,
			  enum admt4000_seq_mode seq);

/**
 * @brief Get conversion mode configuration.
 * @param device - The device structure.
 * @param is_one_shot - One shot mode flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cnv_mode(struct admt4000_dev *device, bool *is_one_shot);

/**
 * @brief Set conversion mode.
 * @param device - The device structure.
 * @param is_one_shot - One shot mode flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_cnv_mode(struct admt4000_dev *device, bool is_one_shot);

/**
 * @brief En/disables selected GPIO output driver. If disabled, serves as input.
 * @param device - The device structure.
 * @param gpio - GPIO number.
 * @param is_en - Enable flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_io_en(struct admt4000_dev *device, uint8_t gpio, bool is_en);

/**
 * @brief Configures between alt function of GPIO function for selected GPIO.
 * @param device - The device structure.
 * @param gpio - GPIO number.
 * @param is_alt_func - Alternate function flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_gpio_func(struct admt4000_dev *device, uint8_t gpio,
		       bool is_alt_func);

/**
 * @brief Get angle threshold value (Raw).
 * @param device - The device structure.
 * @param thres - Threshold value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_angle_thres(struct admt4000_dev *device, uint16_t *thres);

/**
 * @brief Get angle threshold value (Degrees).
 * @param device - The device structure.
 * @param thres - Threshold value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_angle_thres(struct admt4000_dev *device,
				       uint32_t *thres);

/**
 * @brief Set angle threshold value (Raw).
 * @param device - The device structure.
 * @param thres - Threshold value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_angle_thres(struct admt4000_dev *device, uint16_t thres);

/**
 * @brief Set angle threshold value (Degrees).
 * @param device - The device structure.
 * @param thres - Threshold value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_converted_angle_thres(struct admt4000_dev *device,
				       uint32_t thres);

/**
 * @brief Get conversion count value.
 * @param device - The device structure.
 * @param cnt - Conversion count.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_cnv_cnt(struct admt4000_dev *device, uint8_t *cnt);

/**
 * @brief Set conversion count value.
 * @param device - The device structure.
 * @param cnt - Conversion count.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_cnv_cnt(struct admt4000_dev *device, uint8_t cnt);

/**
 * @brief Get the Nth harmonic error magnitude (raw).
 * @param device - The device structure.
 * @param hmag - Harmonic magnitude.
 * @param mag - Magnitude value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_hmag_config(struct admt4000_dev *device, uint8_t hmag,
			     uint16_t *mag);

/**
 * @brief Get the Nth harmonic error magnitude (converted).
 * @param device - The device structure.
 * @param hmag - Harmonic magnitude.
 * @param mag - Magnitude value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_hmag_config(struct admt4000_dev *device,
				       uint8_t hmag, uint32_t *mag);

/**
 * @brief Set the Nth harmonic error magnitude (raw).
 * @param device - The device structure.
 * @param hmag - Harmonic magnitude.
 * @param mag - Magnitude value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_hmag_config(struct admt4000_dev *device, uint8_t hmag,
			     uint16_t mag);

/**
 * @brief Set the Nth harmonic error magnitude (converted).
 * @param device - The device structure.
 * @param hmag - Harmonic magnitude.
 * @param mag - Magnitude value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_converted_hmag_config(struct admt4000_dev *device,
				       uint8_t hmag, uint32_t mag);

/**
 * @brief Get the Nth harmonic error phase (Raw).
 * @param device - The device structure.
 * @param hpha - Harmonic phase.
 * @param pha - Phase value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_hphase_config(struct admt4000_dev *device, uint8_t hpha,
			       uint16_t *pha);

/**
 * @brief Get the Nth harmonic error phase (Converted).
 * @param device - The device structure.
 * @param hpha - Harmonic phase.
 * @param pha - Phase value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_converted_hphase_config(struct admt4000_dev *device,
		uint8_t hpha, uint32_t *pha);

/**
 * @brief Set the Nth harmonic error phase (Raw).
 * @param device - The device structure.
 * @param hpha - Harmonic phase.
 * @param pha - Phase value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_hphase_config(struct admt4000_dev *device, uint8_t hpha,
			       uint16_t pha);

/**
 * @brief Set the Nth harmonic error phase (Converted).
 * @param device - The device structure.
 * @param hpha - Harmonic phase.
 * @param pha - Phase value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_set_converted_hphase_config(struct admt4000_dev *device,
		uint8_t hpha, uint32_t pha);

/**
 * @brief Read selected device specific ID's.
 * @param device - The device structure.
 * @param id_num - ID number.
 * @param id - ID value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_get_id(struct admt4000_dev *device, uint8_t id_num, uint16_t *id);

/**
 * @brief En/disable the ECC functionality.
 * @param device - The device structure.
 * @param is_en - Enable flag.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_ecc_config(struct admt4000_dev *device, bool is_en);

/**
 * @brief Transitions SDP's GPIO pin to high-logic.
 * @param device - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_sdp_pulse_coil_rs(struct admt4000_dev *device);

/**
 * @brief Get value of ADMT4000 GPIO_BUSY.
 * @param device - The device structure.
 * @param val - GPIO value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_sdp_getval_gpio0_busy(struct admt4000_dev *device, uint8_t *val);

/**
 * @brief Get value of ADMT4000 CNV.
 * @param device - The device structure.
 * @param val - GPIO value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_sdp_getval_cnv(struct admt4000_dev *device, uint8_t *val);

/**
 * @brief Get value of ADMT4000 ACALC.
 * @param device - The device structure.
 * @param val - GPIO value.
 * @return 0 in case of success, negative error code otherwise.
 */
int admt4000_sdp_getval_acalc(struct admt4000_dev *device, uint8_t *val);


/**
 * @brief Get the cosine value from the ADMT4000 device.
 *
 * This function retrieves the cosine value from the ADMT4000 magnetometer
 * device and stores it in the provided memory location.
 *
 * @param device Pointer to the ADMT4000 device structure.
 * @param val Pointer to a variable where the retrieved cosine value will be stored.
 *
 * @return 0 in case of success, or a negative error code in case of failure.
 */
int admt4000_get_cos(struct admt4000_dev *device, uint16_t *val);

/**
 * @brief Retrieve the sine value from the ADMT4000 device.
 *
 * This function reads the sine value from the ADMT4000 magnetometer device
 * and stores it in the provided memory location.
 *
 * @param device Pointer to the ADMT4000 device structure.
 * @param val Pointer to a variable where the sine value will be stored.
 *            The value is returned as a 16-bit unsigned integer.
 *
 * @return 0 in case of success, or a negative error code in case of failure.
 */
int admt4000_get_sin(struct admt4000_dev *device, uint16_t *val);


#endif  /* __ADMT4000_H__ */

#ifdef __cplusplus
}
#endif
