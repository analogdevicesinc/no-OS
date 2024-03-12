/***************************************************************************//**
*   @file   cli.h
*   @brief  CLI header.
*
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright (c) 2020 Analog Devices, Inc. All Rights Reserved.
* This software is proprietary to Analog Devices, Inc. and its licensors.
*
*******************************************************************************/

#ifndef CLI_H_
#define CLI_H_

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include "no_os_uart.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/

#define _NC 0
#define _BS 8
#define _CR 13
#define _LF 10
#define _TB 9

#define CLI_WIDTH 80

/******************************************************************************/
/*************************** Types Declarations *******************************/
/******************************************************************************/

/** CLI command type. All CLI commands must abide to this format. */
typedef int32_t (*cmd_func)(void *, uint8_t *);

/**
 * @struct cli_init_param
 * @brief Initialization structure for CLI.
 */
struct cli_init_param {
	/** UART initialization structure. */
	struct no_os_uart_init_param uart_init;
};

/**
 * @struct cli_desc
 * @brief CLI module descriptor.
 */
struct cli_desc {
	/** UART driver descriptor. */
	struct no_os_uart_desc	*uart_device;
	/** Application module descriptor for the application containing the
	 *  CLI. */
	void			*device_descriptor;
	/** CLI ASCII command table. */
	uint8_t			**cmd_commands;
	/** CLI command function table. */
	cmd_func		*v_cmd_fun;
	/** CLI command function table. */
	uint8_t			*command_size;
	uint8_t			*uart_buff;
	uint8_t			interrupt_set;
};

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/

/** Setup the CLI module of the application. */
int32_t cli_setup(struct cli_desc **device, struct cli_init_param *init_param);

/** Free resources allocated by cli_setup(). */
int32_t cli_remove(struct cli_desc *dev);

/** Use the UART driver APIs to write a strng to the terminal. */
int32_t cli_write_string(struct cli_desc *dev, uint8_t *str);

/** Callback of CLI module. */
void cli_uart_callback(void *app_param, uint32_t event,
		       void *data);
void cli_rx_callback(void *ctx);
/** Implements CLI feedback. */
int32_t cli_parse(struct cli_desc *dev, uint8_t c);

/** Implements the CLI logic. */
int32_t cli_process(struct cli_desc *dev);

/** Get the CLI commands and correlate them to functions. */
void cli_find_command(struct cli_desc *dev, uint8_t *command,
		      cmd_func* function);

/** Display command prompt for the user on the CLI at the beginning of the
 *  program. */
int32_t cli_cmd_prompt(struct cli_desc *dev, uint8_t *app_name);

/** Update the current command vector. */
void cli_load_command_vector(struct cli_desc *dev, cmd_func *command_vector);

/** Update the current command call vector. */
void cli_load_command_calls(struct cli_desc *dev, uint8_t **command_calls);

/** Update the current command size vector. */
void cli_load_command_sizes(struct cli_desc *dev, uint8_t *command_sizes);

/** Update the current handler pointer. */
void cli_load_descriptor_pointer(struct cli_desc *dev,
				 void *command_descriptor);

/** Get the newest character in the CLI FIFO. */
void cli_get_last_character(struct cli_desc *dev, uint8_t *new_char);

/** Set the newest character in the CLI FIFO. */
void cli_set_last_character(struct cli_desc *dev, uint8_t new_char);

/** Get the CLI FIFO line index. */
void cli_get_fifo_indx(struct cli_desc *dev, uint8_t *index);

/** Set the CLI FIFO line index. */
void cli_set_fifo_indx(struct cli_desc *dev, uint8_t index);

#endif /* CLI_H_ */
