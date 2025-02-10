/*******************************************************************************
 *   @file   log.c
 *   @brief  Utilities required by GMSL drivers and applications for logging in linux platform
 *   @author Automotive Software and Systems team, Bangalore, India
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

#include <time.h>
#include <string.h>
#include "gmsl_dbg.h"
#include "no_os_print_log.h"

/* Buffer to store the Log message */
uint8_t gmsl_uc_log_msg_buffer[MAX_LOG_MSG_SIZE];
uint32_t gmsl_uc_log_msg_buf_length;

/**
 * @brief Logs an error message with a timestamp.
 *
 * This function logs an error message with the current time in the format
 * HH:MM:SS ERROR: <message>. The message is stored in a buffer and then
 * printed using the pr_info function.
 *
 * @param tx_data Pointer to the data to be logged.
 * @param size Size of the data to be logged.
 */
void gmsl_uc_log_error(uint8_t *tx_data, uint32_t size)
{
	time_t current_time;
	time(&current_time);
	char logData[700];
	memset(&logData, (int) 0, sizeof(logData));
	sprintf(logData, "%02d:%02d:%02d ERROR: %s\r\n",
		localtime(&current_time)->tm_hour, localtime(&current_time)->tm_min,
		localtime(&current_time)->tm_sec, tx_data);
	pr_info("%s", logData);
}

/**
 * @brief Logs an informational message with a timestamp.
 *
 * This function logs an informational message with the current time in the format
 * HH:MM:SS INFO: <message>. The message is stored in a buffer and then
 * printed using the pr_info function.
 *
 * @param tx_data Pointer to the data to be logged.
 * @param size Size of the data to be logged.
 */
void gmsl_uc_log_info(uint8_t *tx_data, uint32_t size)
{
	time_t current_time;
	time(&current_time);
	char logData[700];
	memset(&logData, (int) 0, sizeof(logData));
	sprintf(logData, "%02d:%02d:%02d INFO: %s\r\n",
		localtime(&current_time)->tm_hour, localtime(&current_time)->tm_min,
		localtime(&current_time)->tm_sec, tx_data);
	pr_info("%s", logData);
}

/**
 * @brief Logs a debug message with a timestamp.
 *
 * This function logs a debug message with the current time in the format
 * HH:MM:SS DEBUG: <message>. The message is stored in a buffer and then
 * printed using the pr_info function.
 *
 * @param tx_data Pointer to the data to be logged.
 * @param size Size of the data to be logged.
 */
void gmsl_uc_log_debug(uint8_t *tx_data, uint32_t size)
{
	time_t current_time;
	time(&current_time);
	char logData[700];
	memset(&logData, (int) 0, sizeof(logData));
	sprintf(logData, "%02d:%02d:%02d DEBUG: %s\r\n",
		localtime(&current_time)->tm_hour, localtime(&current_time)->tm_min,
		localtime(&current_time)->tm_sec, tx_data);
	pr_info("%s", logData);
}

/**
 * @brief Logs a warning message with a timestamp.
 *
 * This function logs a warning message with the current time in the format
 * HH:MM:SS WARNING: <message>. The message is stored in a buffer and then
 * printed using the pr_info function.
 *
 * @param tx_data Pointer to the data to be logged.
 * @param size Size of the data to be logged.
 */
void gmsl_uc_log_warning(uint8_t *tx_data, uint32_t size)
{
	time_t current_time;
	time(&current_time);
	char logData[700];
	memset(&logData, (int) 0, sizeof(logData));
	sprintf(logData, "%02d:%02d:%02d WARNING: %s\r\n",
		localtime(&current_time)->tm_hour, localtime(&current_time)->tm_min,
		localtime(&current_time)->tm_sec, tx_data);
	pr_info("%s", logData);
}

/** Allows any layer to notify a fatal error to the application */

/**
 * @brief Handles fatal errors by entering an infinite loop.
 *
 * This function is called to notify the application of a fatal error.
 * It enters an infinite loop, effectively halting the program.
 */
void gmsl_uc_set_fatal_error(void)
{
	while (true) {
		; /* Do nothing */
	}
}