/***************************************************************************//**
 *   @file   console.c
 *   @brief  Implementation of Console Driver.
 *   @author DBogdan (dragos.bogdan@analog.com)
********************************************************************************
 * Copyright 2013(c) Analog Devices, Inc.
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

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "console.h"

/***************************************************************************//**
 * @brief Initializes the UART communication peripheral. If the value of the
 *          baud rate is not equal with the ipcore's baud rate the
 *          initialization going to FAIL.
 *
 * @param baudRate - Baud rate value.
 *                   Example: 9600 - 9600 bps.
 *
 * @return status  - Result of the initialization procedure.
 *                   Example: 0 - if initialization was successful;
 *                           -1 - if initialization was unsuccessful.
 *
*******************************************************************************/
char uart_init(unsigned long baudRate)
{
	return 0;
}

/***************************************************************************//**
 * @brief Writes one character to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void uart_write_char(char data)
{
	putchar(data);
}

/***************************************************************************//**
 * @brief Reads one character from UART.
 *
 * @return received_char   - Read character.
 *
 * Note: Blocking function - Waits until get a valid data.
*******************************************************************************/
void uart_read_char(char * data)
{
	*data = getchar();
}

/***************************************************************************//**
 * @brief Writes one character string to UART.
 *
 * @param data - Character to write.
 *
 * @return None.
*******************************************************************************/
void uart_write_string(const char* string)
{
	while(*string)
	{
		uart_write_char(*string++);
	}
}

/***************************************************************************//**
 * @brief Converts an integer number to a string of ASCII characters string.
 *
 * @param number - Integer number.
 * @param base   - Numerical base used to represent the integer number as
 *                 string.
 *
 * @return Pointer to the string of ASCII characters.
*******************************************************************************/
char *int_to_str(long number, char base)
{
	unsigned long pos_number = 0;
	char		  neg_sign   = 0;
	const char	  digits[17] = "0123456789ABCDEF";
	static char   buffer[17] = "                ";
	char*		  buffer_ptr = &buffer[16];

	if((number < 0) && (base == 10))
	{
		neg_sign = 1;
		pos_number = -1 * number;
	}
	else
	{
		pos_number = (unsigned long)number;
	}
	do
	{
		*buffer_ptr-- = digits[pos_number % base];
		pos_number /= base;
	}
	while(pos_number != 0);
	if(neg_sign)
	{
		*buffer_ptr-- = '-';
	}
	*buffer_ptr++;

	return buffer_ptr;
}

/***************************************************************************//**
 * @brief Prints formatted data to console.
 *
 * @param str - String to be printed.
 *
 * @return None.
*******************************************************************************/
void console_print(char* str, ...)
{
	char*		  string_ptr;
	char		  first_param  = 0;
	char		  second_param = 0;
	unsigned long x_mask	   = 0;
	unsigned long d_mask	   = 0;
	char		  ch_number	   = 0;
	unsigned long multiplier   = 1;
	char*		  str_arg;
	long		  long_arg;
	double		  double_arg;
	va_list		  argp;

	va_start(argp, str);
	for(string_ptr = str; *string_ptr != '\0'; string_ptr++)
	{
		if(*string_ptr!='%')
		{
			uart_write_char(*string_ptr);
			continue;
		}
		string_ptr++;
		first_param = 0;
		while((*string_ptr >= 0x30) & (*string_ptr <= 0x39))
		{
			first_param *= 10;
			first_param += (*string_ptr - 0x30);
			string_ptr++;
		}
		if(*string_ptr == '.')
		{
			string_ptr++;
			second_param = 0;
			while((*string_ptr >= 0x30) & (*string_ptr <= 0x39))
			{
				second_param *= 10;
				second_param += (*string_ptr - 0x30);
				string_ptr++;
			}
		}
		switch(*string_ptr)
		{
		case 'c':
			long_arg = va_arg(argp, long);
			uart_write_char((char)long_arg);
			break;
		case 's':
			str_arg = va_arg(argp, char*);
			uart_write_string(str_arg);
			break;
		case 'd':
			long_arg = va_arg(argp, long);
			uart_write_string(int_to_str(long_arg, 10));
			break;
		case 'x':
			long_arg = va_arg(argp, long);
			x_mask = 268435456;
			ch_number = 8;
			while(x_mask > long_arg)
			{
				x_mask /= 16;
				ch_number--;
			}
			while(ch_number < first_param)
			{
				uart_write_char('0');
				ch_number++;
			}
			if(long_arg != 0)
			{
				uart_write_string(int_to_str(long_arg, 16));
			}
			break;
		case 'f':
			double_arg = va_arg(argp, double);
			if(second_param == 0)
			{
				second_param = 3;
			}
			ch_number = second_param;
			while(ch_number > 0)
			{
				multiplier *= 10;
				ch_number--;
			}
			double_arg *= multiplier;
			if(double_arg < 0)
			{
				double_arg *= -1;
				uart_write_char('-');
			}
			long_arg = (long)double_arg;
			uart_write_string(int_to_str((long_arg / multiplier), 10));
			uart_write_char('.');
			d_mask = 1000000000;
			ch_number = 10;
			while(d_mask > (long)(long_arg % multiplier))
			{
				d_mask /= 10;
				ch_number--;
			}
			while(ch_number < second_param)
			{
				uart_write_char('0');
				ch_number++;
			}
			if((long_arg % multiplier) != 0)
			{
				uart_write_string(int_to_str((long_arg % multiplier), 10));
			}
			break;
		}
	}
	va_end(argp);
}

/***************************************************************************//**
 * @brief Initializes the serial console.
 *
 * @param baud_rate - Baud rate value.
 *                    Example: 9600 - 9600 bps.
 *
 * @return status   - Result of the initialization procedure.
 *                    Example: -1 - if initialization was unsuccessful;
 *                              0 - if initialization was successful.
*******************************************************************************/
char console_init(unsigned long baud_rate)
{
	return uart_init(baud_rate);
}

/***************************************************************************//**
 * @brief Reads one command from console.
 *
 * @param command - Read command.
 *
 * @return None.
*******************************************************************************/
void console_get_command(char* command)
{
	char		  received_char	= 0;
	unsigned char char_number	= 0;

	while((received_char != '\n') && (received_char != '\r'))
	{
		uart_read_char(&received_char);
		command[char_number++] = received_char;
	}
}

/***************************************************************************//**
 * @brief Compares two commands and returns the type of the command.
 *
 * @param received_cmd - Received command.
 * @param expected_cmd - Expected command.
 * @param param        - Parameters' buffer.
 * @param param_no     - Nomber of parameters.
 *
 * @return cmd_type    - Type of the command.
 *                       Example: UNKNOWN_CMD - Commands don't match.
 *                                DO_CMD      - Do command (!).
 *                                READ_CMD    - Read command (?).
 *                                WRITE_CMD   - Write command (=).
*******************************************************************************/
int console_check_commands(char*	   received_cmd,
						   const char* expected_cmd,
						   double*	   param,
						   char*	   param_no)
{
	int			  cmd_type		   = 1;
	unsigned char char_index	   = 0;
	char		  param_string[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char param_index	   = 0;
	unsigned char index			   = 0;
	const char	  digits[17]	   = "0123456789ABCDEF";
	unsigned char digit_index	   = 0;

	while((expected_cmd[char_index] != '!') &&
		  (expected_cmd[char_index] != '?') &&
		  (expected_cmd[char_index] != '=') &&
		  (cmd_type != UNKNOWN_CMD))
	{
		if(expected_cmd[char_index] != received_cmd[char_index])
		{
			cmd_type = UNKNOWN_CMD;
		}
		char_index++;
	}
	if(cmd_type != UNKNOWN_CMD)
	{
		if(expected_cmd[char_index] == '!')
		{
			if(received_cmd[char_index] == '!')
			{
				cmd_type = DO_CMD;
			}
			else
			{
				cmd_type = UNKNOWN_CMD;
			}
		}
		if(expected_cmd[char_index] == '?')
		{
			if(received_cmd[char_index] == '?')
			{
				cmd_type = READ_CMD;
			}
			else
			{
				cmd_type = UNKNOWN_CMD;
			}
		}
		if(expected_cmd[char_index] == '=')
		{
			if(received_cmd[char_index] == '=')
			{
				cmd_type = WRITE_CMD;
			}
			else
			{
				cmd_type = UNKNOWN_CMD;
			}
		}
		if((cmd_type == WRITE_CMD) || (cmd_type == READ_CMD))
		{
			char_index++;
			while((received_cmd[char_index] != '\n') &&
				  (received_cmd[char_index] != '\r'))
			{
				if((received_cmd[char_index] == 0x20))
				{
					*param = 0;
					if((param_string[0] == '0') && (param_string[1] == 'x'))
					{
						for(index = 2; index < param_index; index++)
						{
							for(digit_index = 0; digit_index < 16; digit_index++)
							{
								if(param_string[index] == digits[digit_index])
								{
									*param = *param * 16;
									*param = *param + digit_index;
								}
							}
						}
					}
					else
					{
						if(param_string[0] == '-')
						{
							*param = atof((const char*)(&param_string[1]));
							*param *= (-1);
						}
						else
						{
							*param = atof((const char*)param_string);
						}
					}
					param++;
					*param_no += 1;
					for(param_index = 0; param_index < 10; param_index++)
					{
						param_string[param_index] = 0;
					}
					param_index = 0;
					char_index++;
				}
				else
				{
					param_string[param_index] = received_cmd[char_index];
					char_index++;
					param_index++;
				}
			}
			if(param_index)
			{
				*param = 0;
				if((param_string[0] == '0') && (param_string[1] == 'x'))
				{
					for(index = 2; index < param_index; index++)
					{
						for(digit_index = 0; digit_index < 16; digit_index++)
						{
							if(param_string[index] == digits[digit_index])
							{
								*param *= 16;
								*param += digit_index;
							}
						}
					}
				}
				else
				{
					if(param_string[0] == '-')
					{
						*param = atof((const char*)(&param_string[1]));
						*param *= (-1);
					}
					else
					{
						*param = atof((const char*)param_string);
					}
				}
				*param_no += 1;
			}
		}
	}

	return cmd_type;
}
