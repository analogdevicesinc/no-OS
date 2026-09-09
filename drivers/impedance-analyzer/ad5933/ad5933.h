/***************************************************************************//**
 *   @file   AD5933.h
 *   @brief  Header file of AD5933 Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/
#ifndef __AD5933_H__
#define __AD5933_H__

#include <stdint.h>
#include "no_os_i2c.h"
#include "no_os_util.h"
#include "no_os_delay.h"

/* AD5933 Registers */
#define AD5933_REG_CONTROL_HB       0x80    // HB of the Control register, R/W 1 byte
#define AD5933_REG_CONTROL_LB       0x81    // LB of the Control register, R/W 1 byte
#define AD5933_REG_FREQ_START       0x82    // Start frequency, R/W 3 bytes
#define AD5933_REG_FREQ_INC         0x85    // Frequency increment, R/W 3 bytes
#define AD5933_REG_INC_NUM          0x88    // Number of increments, R/W 2 bytes
#define AD5933_REG_SETTLING_CYCLES  0x8A    // Number of settling time cycles, R/W 2 bytes
#define AD5933_REG_STATUS           0x8F    // Status, R 1 byte
#define AD5933_REG_TEMP_DATA        0x92    // Temperature data, R 2 bytes
#define AD5933_REG_REAL_DATA        0x96    // Real data, R 2 bytes
#define AD5933_REG_IMAG_DATA        0x94    // Imaginary data, R 2 bytes

/* AD5933_REG_CONTROL_HB Bits */
#define AD5933_CONTROL_FUNCTION(x)  ((x) << 4)
#define AD5933_CONTROL_RANGE(x)     ((x) << 1)
#define AD5933_CONTROL_PGA_GAIN(x)  ((x) << 0)

/* AD5933_REG_CONTROL_LB Bits */
#define AD5933_CONTROL_RESET        NO_OS_BIT(4)
#define AD5933_CONTROL_INT_SYSCLK   (0x0 << 3)
#define AD5933_CONTROL_EXT_SYSCLK   NO_OS_BIT(3)

/* AD5933_CONTROL_FUNCTION(x) options */
#define AD5933_FUNCTION_NOP                 0x0
#define AD5933_FUNCTION_INIT_START_FREQ     0x1
#define AD5933_FUNCTION_START_SWEEP         0x2
#define AD5933_FUNCTION_INC_FREQ            0x3
#define AD5933_FUNCTION_REPEAT_FREQ         0x4
#define AD5933_FUNCTION_MEASURE_TEMP        0x9
#define AD5933_FUNCTION_POWER_DOWN          0xA
#define AD5933_FUNCTION_STANDBY             0xB

enum ad5933_clock_source {
	AD5933_CLOCK_SOURCE_INTERNAL = AD5933_CONTROL_INT_SYSCLK,
	AD5933_CLOCK_SOURCE_EXTERNAL = AD5933_CONTROL_EXT_SYSCLK
};

/* AD5933_CONTROL_RANGE(x) options */
enum ad5933_output_range {
	AD5933_RANGE_2000mVpp,
	AD5933_RANGE_200mVpp,
	AD5933_RANGE_400mVpp,
	AD5933_RANGE_1000mVpp
};

/* AD5933_CONTROL_PGA_GAIN(x) options */
enum ad5933_pga_gain {
	AD5933_GAIN_X5,
	AD5933_GAIN_X1
};

/* AD5933 Default number of settling cycles */
#define AD5933_SETTLING_CYCLES		50

/* AD5933 settling cycles multiplier (settling register bits D10:D9) */
enum ad5933_settling_cycle_multiplier {
	AD5933_SETTLING_X1 = 0,
	AD5933_SETTLING_X2 = 1,
	AD5933_SETTLING_X4 = 3
};

/* AD5933 measurement mode for EVAL-AD5933ARDZ*/
enum meas_mode {
	AD5933_MEAS_MODE_2W,			// Default
	AD5933_MEAS_MODE_4W,
};

/* AD5933 max 9-bit (D8:D0) settling cycle count 511 x 4x multiplier*/
#define AD5933_MAX_SETTLING_CYCLES  0x7FC	// 2044

/* AD5933_REG_STATUS Bits */
#define AD5933_STAT_TEMP_VALID      NO_OS_BIT(0)
#define AD5933_STAT_DATA_VALID      NO_OS_BIT(1)
#define AD5933_STAT_SWEEP_DONE      NO_OS_BIT(2)

/* AD5933 Default I2C Address */
#define AD5933_ADDRESS              0x0D

#define AD5933_ADDR_POINTER         0xB0

/* AD5933 Specifications */
#define AD5933_INTERNAL_SYS_CLK     16776000ul		// 16.776 MHz
#define AD5933_MAX_INC_NUM          511				// Maximum increment number
#define AD5933_MAX_RETRIES          3000			// Maximum number of retries for waiting for a status bit to be set
#define AD5933_STATUS_POLL_MS		1				// Delay between status polls, in milliseconds
#define AD5933_POWERUP_RETRIES      300				// Retries for the power-up I2C probe (cold-start supply ramp)
#define AD5933_POWERUP_RETRY_MS     10				// Delay between power-up probe attempts, in milliseconds

/*
 * Accuracy bounds: the 1024-point DFT buffer must hold >=1 excitation period,
 * ADC sampling at MCLK/16  =>  f_min = MCLK / (16 * 1024).
 * Also, f_max = MCLK / 32.
 * It is possible to lower the minimum output frequency than 1 kHz by scaling
 * the provided external clock.
*/
#define AD5933_MIN_OUTPUT_FREQ(clk)   	((double)(clk) / 16384.0)
#define AD5933_MAX_OUTPUT_FREQ(clk)   	((double)(clk) / 32.0)
#define AD5933_MIN_FREQ_INC				0.1f			// 0.1 Hz

struct ad5933_dev {
	/* I2C */
	struct no_os_i2c_desc	*i2c_desc;
	/* Current system clock frequency */
	uint32_t clock_freq;
	enum ad5933_clock_source clock_source;
	enum ad5933_pga_gain pga_gain;
	enum ad5933_output_range output_range;
	uint16_t settling_cycles;
	double start_freq;
	double freq_increment;
	uint16_t num_increments;
	uint16_t sweep_point;
	double current_output_freq;
};

struct ad5933_init_param {
	/* I2C */
	struct no_os_i2c_init_param	i2c_init;
	/* Device Settings */
	uint32_t clock_freq;
	enum ad5933_clock_source clock_source;
	enum ad5933_pga_gain pga_gain;
	enum ad5933_output_range output_range;
	uint16_t settling_cycles;
};

/*! Initializes the communication peripheral. */
int ad5933_init(struct ad5933_dev **device,
		struct ad5933_init_param *init_param);

/*! Setup the device clock, range and gain. */
int ad5933_setup(struct ad5933_dev *dev);

/*! Free the resources allocated by ad5933_init(). */
int ad5933_remove(struct ad5933_dev *dev);

/*! Writes a single byte into a register. */
int ad5933_reg_write(struct ad5933_dev *dev, uint8_t reg, uint8_t val);

/*! Reads a single byte from a register. */
int ad5933_reg_read(struct ad5933_dev *dev, uint8_t reg, uint8_t *val);

/*! Writes a multi-byte, MSB-first value into a register. */
int ad5933_set_register_value(struct ad5933_dev *dev,
			      uint8_t register_address,
			      uint32_t register_value,
			      uint8_t bytes_number);

/*! Reads a multi-byte, MSB-first value from a register. */
int ad5933_get_register_value(struct ad5933_dev *dev,
			      uint32_t *data,
			      uint8_t register_address,
			      uint8_t bytes_number);

/*! Resets the device. */
int ad5933_reset(struct ad5933_dev *dev);

/*! Selects the source of the system clock. */
int ad5933_set_system_clk(struct ad5933_dev *dev,
			  enum ad5933_clock_source clk_source,
			  uint32_t ext_clk_freq);

/*! Selects the range and gain of the device. */
int ad5933_set_range_and_gain(struct ad5933_dev *dev,
			      enum ad5933_output_range range,
			      enum ad5933_pga_gain gain);

/*! Selects the range of the device. */
int ad5933_set_range(struct ad5933_dev *dev, enum ad5933_output_range range);

/*! Selects the gain of the device. */
int ad5933_set_gain(struct ad5933_dev *dev, enum ad5933_pga_gain gain);

/*! Reads the temperature value and converts it to degrees Celsius. */
int ad5933_get_temperature(struct ad5933_dev *dev, float *temperature);

/*! Configures the sweep parameters. */
int ad5933_config_sweep(struct ad5933_dev *dev,
			double start_freq,
			double inc_freq,
			uint16_t num_increments);

/*! Reads the real and imaginary value for a given frequency function. */
int ad5933_get_data(struct ad5933_dev *dev,
		    uint8_t freq_function,
		    int16_t *real,
		    int16_t *imag);

/*! Reads the real and the imaginary data and calculates the Gain Factor. */
int ad5933_calculate_gain_factor(struct ad5933_dev *dev,
				 double *gain_factor,
				 double calibration_impedance,
				 enum meas_mode measurement_mode);

/*! Reads the real and the imaginary data and calculates the Impedance. */
int ad5933_calculate_impedance(struct ad5933_dev *dev,
			       double gain_factor,
			       double *impedance,
			       enum meas_mode measurement_mode);

/*! Selects the number of settling cycles. */
int ad5933_set_settling_time(struct ad5933_dev *dev,
			     uint16_t number_cycles);

/*! Polls the status register until (status & mask) or timeout. */
int ad5933_wait_status(struct ad5933_dev *dev, uint8_t mask,
		       uint8_t *status_out);

/*! Reads the raw temperature (14-bit two's complement) from the part. */
int ad5933_get_raw_temperature(struct ad5933_dev *dev, int32_t *temperature);

/*! Reads the real and imaginary data and calculates the magnitude. */
int ad5933_get_magnitude(struct ad5933_dev *dev,
			 double *magnitude);

/*! Reads the real and imaginary data and calculates the phase (radians). */
int ad5933_get_phase(struct ad5933_dev *dev,
		     double system_phase,
		     double *phase_rad);

int ad5933_initialize_sweep(struct ad5933_dev *dev);

int ad5933_init_start_freq(struct ad5933_dev *dev);

int ad5933_start_sweep(struct ad5933_dev *dev);

int ad5933_increment_freq(struct ad5933_dev *dev);

int ad5933_repeat_freq(struct ad5933_dev *dev);

int ad5933_power_down(struct ad5933_dev *dev);

int ad5933_standby(struct ad5933_dev *dev);

uint32_t ad5933_convert_freq_to_reg(struct ad5933_dev *dev, double frequency);

/*! Reads the real and imaginary value from register. */
int ad5933_get_current_data(struct ad5933_dev *dev, int16_t *real,
			    int16_t *imag);

int ad5933_sweep_done(struct ad5933_dev *dev, int32_t *done);

#endif /* __AD5933_H__ */
