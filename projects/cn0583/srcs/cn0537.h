/***************************************************************************//**
*   @file   cn0537.h
*   @brief  CN0537 application header.
*
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright (c) 2020 Analog Devices, Inc. All Rights Reserved.
* This software is proprietary to Analog Devices, Inc. and its licensors.

*******************************************************************************/

#ifndef CN0537_H_
#define CN0537_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "adpd188.h"
#include "cli.h"
#include "no_os_rtc.h"
#include "sd.h"
#include "ff.h"
#include "diskio.h"
#include "no_os_irq.h"
#include "no_os_gpio.h"
#include "max31875.h"
#include "no_os_delay.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define CN0537_ACTIVE_CHANNELS_PER_TIMESLOT 2
#define CN0537_TOTAL_CHANNELS_PER_TIMESLOT 4
#define CN0537_TIMESLOTS_ACTIVE 2

#define HELP_SHORT_COMMAND true
#define HELP_LONG_COMMAND false

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/**
 * @enum cn0537_stream_mode
 * @brief Stream mode IDs.
 */
enum cn0537_stream_mode {
	/** Code mode ID */
	CN0537_CODE,
	/** PTR mode ID */
	CN0537_PTR,
	CN0537_RAW,
};

/**
 * @struct cn0537_dev
 * @brief Application handler structure.
 */
struct cn0537_dev {
	/** ADPD188 device driver handler. */
	struct adpd188_dev *adpd188_handler;
	/** CLI module handler. */
	struct cli_desc *cli_handler;
	/** RTC device driver handler. */
	struct no_os_rtc_desc *rtc_handler;
	/** SD module handler. */
	struct sd_desc *sd_handler;
	/** Interrupt controller device handler. */
	struct no_os_irq_ctrl_desc *irq_handler;
	struct no_os_irq_ctrl_desc *gpio_irq_handler;
	/** Alarm GPIO driver handler. */
	struct no_os_gpio_desc *gpio_alrm;
	/** Button GPIO driver handler. */
	struct no_os_gpio_desc *gpio_btn;
	/** Temperature sensor driver handler. */
	struct max31875_dev *max31875_handler;
	/** ADICUP3029 LED1 GPIO driver. */
	struct no_os_gpio_desc *gpio_cup1;
	/** ADICUP3029 LED2 GPIO driver. */
	struct no_os_gpio_desc *gpio_cup2;
	/** Heater GPIO TODO implement */
	struct no_os_gpio_desc *gpio_heater;
	/** Stream enable flag. */
	bool stream_en;
	/** Latest blue code data. */
	uint32_t blue_data;
	/** Latest IR code data. */
	uint32_t ir_data;
	/** Latest blue code data. */
	uint32_t blue_raw;
	/** Latest IR code data. */
	uint32_t ir_raw;
	/** Latest blue PTR data. */
	float blue_ptr;
	/** Latest IR PTR data. */
	float ir_ptr;
	/** IR PTR value after a median filter has been applied */
	float ir_ptr_median;
	/** Current stream mode. */
	enum cn0537_stream_mode stream_mode;
	/** Latest RTC counter read. */
	uint32_t rtc_count;
	/** Latest sample timestamp in seconds. */
	float time_stamp;
	/** Filesystem mount object. */
	FATFS ff_obj;
	/** Current file object. */
	FIL curr_file;
	/** File open flag. */
	bool file_open;
	/** Idle flag. */
	uint8_t idle;
	/** Stream counter flag. */
	bool stream_cnt_en;
	/** Stream counter. */
	uint32_t stream_cnt;
	/** Latest temperature code sample. */
	uint16_t raw_temp;
	/** Latest humidity code sample. */
	uint16_t raw_humidity;
	/** Latest temperature sample in Celsius. */
	float temp_celsius;
	/** Latest humidity sample in percent. */
	float hum_percent;
	struct no_os_time timestamp;
};

/**
 * @struct cn0537_init_param
 * @brief Application initialization structure.
 */
struct cn0537_init_param {
	/** ADPD188 initialization structure. */
	struct adpd188_init_param adpd188_init;
	/** CLI initialization structure. */
	struct cli_init_param cli_init;
	/** RTC initialization structure. */
	struct no_os_rtc_init_param rtc_init;
	/** SD module initialization structure. */
	struct sd_init_param sd_init;
	/** Interrupt controller initialization structure. */
	struct no_os_irq_init_param irq_init;
	struct no_os_irq_init_param gpio_irq_init;
	/** Alarm GPIO initialization structure. */
	struct no_os_gpio_init_param gpio_alrm;
	/** Button GPIO initialization structure. */
	struct no_os_gpio_init_param gpio_btn;
	/** Temperature sensor initialization structure. */
	struct max31875_init_param max31875_init;
	/** ADICUP329 LED1 GPIO initialization structure. */
	struct no_os_gpio_init_param gpio_cup1;
	/** ADICUP329 LED2 GPIO initialization structure. */
	struct no_os_gpio_init_param gpio_cup2;
	/** Heater GPIO initialization structure. */
	struct no_os_gpio_init_param gpio_heater;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Return the default initial configuration for the application. */
void cn0537_get_config(struct cn0537_init_param *init_param);

/** Initialize the application. */
int32_t cn0537_init(struct cn0537_dev **device,
		    struct cn0537_init_param *init_param);

/** Free memory allocated by cn0537_init(). */
int32_t cn0537_remove(struct cn0537_dev *dev);

/** Calculate time in seconds using RTC counter readout. */
int32_t cn0537_process(struct cn0537_dev *dev);

/** CLI help command. Display help tooltip on the console. */
int32_t cn0537_help(struct cn0537_dev *dev, uint8_t *arg);

/** CLI command to read and display a device register. */
int32_t cn0537_reg_read(struct cn0537_dev *dev, uint8_t *arg);

/** CLI command to write a device register. */
int32_t cn0537_reg_write(struct cn0537_dev *dev, uint8_t *arg);

/** CLI command to start streaming data from the smoke detector. */
int32_t cn0537_stream(struct cn0537_dev *dev, uint8_t *arg);

/** CLI command to stop streaming and/or reading smoke data. */
int32_t cn0537_idle(struct cn0537_dev *dev, uint8_t *arg);

/** Set stream mode to code or PTR. */
int32_t cn0537_mode_set(struct cn0537_dev *dev, uint8_t *arg);

/** Read and disply all ADPD device registers. */
int32_t cn0537_register_dump(struct cn0537_dev *dev, uint8_t *arg);

/** Calculate and display the output data rate. */
int32_t cn0537_odr_get(struct cn0537_dev *dev, uint8_t *arg);

/** Set output data rate. */
int32_t cn0537_odr_set(struct cn0537_dev *dev, uint8_t *arg);

/** Implement test procedure and display PASS if the system is working and FAIL
 *  if there is a problem. */
int32_t cn0537_func_test(struct cn0537_dev *dev, uint8_t *arg);

/** CLI command to reset alarm condition. Same as the reset button. */
int32_t cn0537_reset_alarm(struct cn0537_dev *dev, uint8_t *arg);

/** Display custom note on the terminal and print it to SD card log. */
int32_t cn0537_note(struct cn0537_dev *dev, uint8_t *arg);

/** Open a file on the SD card and start logging. Only one file may be open at a
 *  time. */
int32_t cn0537_file_open(struct cn0537_dev *dev, uint8_t *arg);

/** Close the opened file. */
int32_t cn0537_file_close(struct cn0537_dev *dev, uint8_t *arg);

#endif /* CN0537_H_ */
