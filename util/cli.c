/***************************************************************************//**
*   @file   cli.c
*   @brief  CLI source.
*   @author Andrei Drimbarean (andrei.drimbarean@analog.com)
********************************************************************************
* Copyright 2018(c) Analog Devices, Inc.
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
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "error.h"
#include "uart_extra.h"
#include "irq.h"

/******************************************************************************/
/************************** Variable Definitions ******************************/
/******************************************************************************/

static uint8_t uart_current_line[CLI_WIDTH];
static uint8_t uart_previous_line[CLI_WIDTH];
static uint8_t uart_line_index, uart_buff;
static volatile uint8_t uart_written = 0;
static uint8_t uart_cmd;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

/**
 * @brief Disable the other interrupts aside from the UART interrupt to not
 *        interfere with the UART reception.
 * @param void
 * @return void
 */
static inline void cli_disable_int(void)
{
	/* Only disable interrupts enabled by the application.
	 * Example: NVIC_DisableIRQ(TMR0_EVT_IRQn); */
}

/**
 * @brief Enable back the interrupts disabled by cli_disable_int()
 * @param void
 * @return void
 */
static inline void cli_enable_int(void)
{
	/* Only enable interrupts disabled by the cli_disable_int().
	 * Example: NVIC_EnableIRQ(TMR0_EVT_IRQn); */
}

/**
 * @brief Use the UART driver APIs to write a strng to the terminal.
 * @param [in] dev - Pointer to the device descriptor.
 * @param [in] str - Pointer to the string to be displayed.
 * @return 0 in case of success, error code otherwise.
 */
int32_t cli_write_string(struct cli_desc *dev, uint8_t *str)
{
	uart_write_nonblocking(dev->uart_device, str, strlen((char *)str));

	while(!uart_written);
	uart_written = 0;

	return SUCCESS;
}

/**
 * @brief UART callback specific to UART.
 * @param app_param - Application supplied CLI handler.
 * @param event - Interrupt code.
 * @param data - Not used here.
 */
void cli_uart_callback(void *app_param, uint32_t event,
		       void *data)
{
	struct cli_desc *cli_ptr = app_param;

	switch(event) {
	case READ_DONE:
		cli_parse(cli_ptr, uart_buff);
		uart_read_nonblocking(cli_ptr->uart_device, &uart_buff, 1);
		break;
	case WRITE_DONE:
		uart_written = 1;
		break;
	case ERROR:
		break;
	}
}

/**
 * @brief Setup the CLI module of the application. Allocate memory for the
 *        driver handler and initialize the physical layer.
 * @param [out] device    - Pointer to the device handler structure.
 * @param [in] init_param - Pointer to the device initialization structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t cli_setup(struct cli_desc **device, struct cli_init_param *init_param)
{
	int32_t ret;

	*device = (struct cli_desc *)calloc(1, sizeof(**device));
	if (!(*device))
		return -1;

	ret = uart_init(&((*device)->uart_device), &(init_param->uart_init));
	if (ret != SUCCESS)
		goto error_dev;

	ret = uart_read_nonblocking((*device)->uart_device, &uart_buff, 1);
	if (ret != SUCCESS)
		goto error_uart;

	return SUCCESS;

error_uart:
	uart_remove((*device)->uart_device);
error_dev:
	free(*device);

	return FAILURE;
}

/**
 * @brief Free resources allocated by cli_setup().
 * @param [in] dev - Pointer to the device descriptor.
 * @return 0 if success, negative error code otherwise.
 */
int32_t cli_remove(struct cli_desc *dev)
{
	int32_t ret;

	if(!dev)
		return -1;

	ret = uart_remove(dev->uart_device);
	if(ret != SUCCESS)
		return ret;

	free(dev);

	return ret;
}

/**
 * @brief Implements CLI feedback.
 * @param [in] dev - The device structure.
 * @param [in] c   - The newest character to be parsed.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t cli_parse(struct cli_desc *dev, uint8_t c)
{
	int8_t i;

	switch(c) {
	case _CR:
	case _LF:
		uart_cmd = 1;
		break;
	case _BS:
		if(uart_line_index == 0)
			break;
		uart_line_index--;
		break;
	case _TB:
		cli_write_string(dev, (uint8_t*)"\n>");
		i = 0;
		do {
			uart_current_line[i] = uart_previous_line[i];
		} while(uart_previous_line[i++] != '\0');
		cli_write_string(dev, uart_current_line);
		uart_line_index = --i;
		break;
	case _NC:
		break;
	default:
		uart_current_line[uart_line_index++] = c;
		if(uart_line_index == CLI_WIDTH) {
			uart_line_index--;
		}
	}

	uart_current_line[uart_line_index] = '\0';

	return SUCCESS;
}

/**
 * @brief Implements the CLI logic.
 * @param [in] dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t cli_process(struct cli_desc *dev)
{
	cmd_func func = NULL;
	uint8_t i = 0;
	int32_t ret;

	/* Check if <ENTER> key was pressed */
	if (uart_cmd == 1) {
		do {
			uart_previous_line[i] = uart_current_line[i];
		} while(uart_current_line[i++] != '\0');
		/* Find needed function based on typed command */
		cli_find_command(dev, uart_current_line, &func);

		/* Check if there is a valid command */
		if (func) {
			cli_write_string(dev, (uint8_t*)"\n");
			/* Call the desired function */
			(*func)(dev->device_descriptor,
				(uint8_t*)strchr((char*)uart_current_line, ' ') + 1);
		} else if(strncmp((char*)uart_current_line, "exit", 5) == 0) {
			dev->device_descriptor = NULL;

			/* Check if there is no match for typed command */
		} else if (strlen((char *)uart_current_line) != 0) {
			cli_write_string(dev, (uint8_t*)"\n");
			/* Display a message for unknown command */
			cli_write_string(dev,
					 (uint8_t*)"Unknown command!");
			cli_write_string(dev, (uint8_t*)"\n");
		}
		/* Prepare for next <ENTER> */
		uart_cmd = 0;
		ret = cli_cmd_prompt(dev, NULL);
		if(ret != SUCCESS)
			return ret;
	}

	return SUCCESS;
}

/**
 * @brief Get the CLI commands and correlate them to functions.
 * @param [in] dev       - The device structure.
 * @param [in] command   - Command received from the CLI.
 * @param [out] function - Pointer to the corresponding function.
 * @return 0 in case of success, negative error code otherwise.
 */
void cli_find_command(struct cli_desc *dev, uint8_t *command,
		      cmd_func* function)
{
	uint8_t i = 0;

	while (dev->v_cmd_fun[i/2] != NULL) {
		if(strncmp((char *)command,
			   (char *)dev->cmd_commands[i],
			   dev->command_size[i]) == 0 ||
		    strncmp((char *)command,
			    (char *)dev->cmd_commands[i + 1],
			    dev->command_size[i+1]) == 0) {
			if(dev->command_size == 0) {
				break;
			}
			*function = dev->v_cmd_fun[i / 2];
			break;
		}
		i += 2;
	}
}

/**
 * @brief Display command prompt for the user on the CLI at the beginning of the
 *        program.
 * @param [in] dev - The device structure.
 * @return 0 in case of success, negative error code otherwise.
 */
int32_t cli_cmd_prompt(struct cli_desc *dev, uint8_t *app_name)
{
	int32_t ret;
	static uint8_t count = 0;

	ret = cli_write_string(dev, (uint8_t*)"\n");

	/* Check first <ENTER> is pressed after reset */
	if(count == 0) {
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################         ###     ###    ###    ###     ###       #######      ####### \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################        #####    ####   ###    ####    ###      ###    ###  ###    ### \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################       ##  ##    #####  ###   ##  ##   ###     ###      ##  ##          \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####    ##################       ##   ##   ##  ## ###  ##   ##   ###     ##       ## ###   #####  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####       ###############      #########  ##   #####  ########  ###     ###      ##  ##      ##  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####          ############      ##     ##  ##    #### ###    ### ###      ###   ####  ###    ###  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####             #########     ##      ### ##     ### ##      ## ########   ######      ###### #  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####                ######                                                                        \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####                ######     # ####      #######   #      ##  #     ####     #######    ####    \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####             #########     #########  ######### ###    ### ###  ###  ###  ########  ###  ###  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####          ############     ##     ##  ##         ##    ##  ### ###     ## ##       ###        \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####       ###############     ##     ### ########   ###  ###  ### ##         ########  ######    \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " #####    ##################     ##      ## ###         ##  ##   ### ##         ###          ###### \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################     ##     ### ##           ####    ### ###     ## ##       ##      ## \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################     #########  #########    ####    ###  ########  ######### ########  \n");
		cli_write_string(dev,
				 (uint8_t*)
				 " ###########################                                             ###                ####   \n");
		cli_write_string(dev,
				 (uint8_t*)"\n");

		cli_write_string(dev, (uint8_t*)"\tWelcome to the ");
		cli_write_string(dev, app_name);
		cli_write_string(dev, (uint8_t*)" application!\n");
		cli_write_string(dev,
				 (uint8_t*)"Type <help> or <h> to see available commands...\n");
		cli_write_string(dev, (uint8_t*)"\n");
		count++;
	}

	if(ret == 0)
		uart_write_nonblocking(dev->uart_device, (uint8_t *)">", 1);

	uart_line_index = 0;

	return ret;
}

/**
 * @brief Update the current command vector. The CLI has a vector of function
 *        pointers that can be called as commands using the physical layer as an
 *        input. Load this vector pointer.
 * @param [in] dev            - Handler structure pointer.
 * @param [in] command_vector - Pointer to the new command vector.
 * @return void
 */
void cli_load_command_vector(struct cli_desc *dev, cmd_func *command_vector)
{
	dev->v_cmd_fun = command_vector;
}

/**
 * @brief Update the current command call vector. Load the pointer to the list
 *        of recognized CLI commands.
 * @param [in] dev           - Handler structure pointer.
 * @param [in] command_calls - Pointer to the new command call vector.
 * @return void
 */
void cli_load_command_calls(struct cli_desc *dev, uint8_t **command_calls)
{
	dev->cmd_commands = command_calls;
}

/**
 * @brief Update the current command size vector. Load the pointer to the list
 *        of recognized CLI commands sizes.
 * @param [in] dev           - Handler structure pointer.
 * @param [in] command_sizes - Pointer to the new command call sizes vector.
 * @return void
 */
void cli_load_command_sizes(struct cli_desc *dev, uint8_t *command_sizes)
{
	dev->command_size = command_sizes;
}

/**
 * @brief Update the current handler pointer. Load the pointer to the handler
 *        that must be passed as argument to the command functions.
 * @param [in] dev           - Handler structure pointer.
 * @param [in] command_sizes - Pointer to the new command call sizes vector.
 * @return void
 */
void cli_load_descriptor_pointer(struct cli_desc *dev, void *command_descriptor)
{
	dev->device_descriptor = command_descriptor;
}
