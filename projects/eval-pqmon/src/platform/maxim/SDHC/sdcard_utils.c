/*******************************************************************************
 * @file sdcard_utils.c
 * @author Robert Budai (robert.budai@analog.com)
 * @brief Source file for the pqm sd card utils functions.
 ********************************************************************************
 * Copyright (c) 2024 Analog Devices, Inc.
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

#include "sdcard_utils.h"

/******************************************************************************/
/************************ Variable Declarations *******************************/
/******************************************************************************/

FATFS *fs;
FATFS fs_obj;
FIL file;
FRESULT err;
FILINFO fno;
DIR dir_tmp;
TCHAR directory[MAXLEN], cwd[MAXLEN], filename[MAXLEN], volume_label[24],
      volume = '0';
TCHAR *FF_ERRORS[20];
DWORD clusters_free = 0, sectors_free = 0, sectors_total = 0, volume_sn = 0;
UINT bytes_written = 0, bytes_read = 0, mounted = 0;
BYTE work[4096];
mxc_gpio_cfg_t SDPowerEnablePin
= { MXC_GPIO3, MXC_GPIO_PIN_5, MXC_GPIO_FUNC_OUT, MXC_GPIO_PAD_NONE,
    MXC_GPIO_VSSEL_VDDIO
  };

bool started_logging = false;
int debugLogCounter = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

int sdhc_init (void)
{
	mxc_sdhc_cfg_t cfg;

	FF_ERRORS[0] = "FR_OK";
	FF_ERRORS[1] = "FR_DISK_ERR";
	FF_ERRORS[2] = "FR_INT_ERR";
	FF_ERRORS[3] = "FR_NOT_READY";
	FF_ERRORS[4] = "FR_NO_FILE";
	FF_ERRORS[5] = "FR_NO_PATH";
	FF_ERRORS[6] = "FR_INVLAID_NAME";
	FF_ERRORS[7] = "FR_DENIED";
	FF_ERRORS[8] = "FR_EXIST";
	FF_ERRORS[9] = "FR_INVALID_OBJECT";
	FF_ERRORS[10] = "FR_WRITE_PROTECTED";
	FF_ERRORS[11] = "FR_INVALID_DRIVE";
	FF_ERRORS[12] = "FR_NOT_ENABLED";
	FF_ERRORS[13] = "FR_NO_FILESYSTEM";
	FF_ERRORS[14] = "FR_MKFS_ABORTED";
	FF_ERRORS[15] = "FR_TIMEOUT";
	FF_ERRORS[16] = "FR_LOCKED";
	FF_ERRORS[17] = "FR_NOT_ENOUGH_CORE";
	FF_ERRORS[18] = "FR_TOO_MANY_OPEN_FILES";
	FF_ERRORS[19] = "FR_INVALID_PARAMETER";

	MXC_GPIO_Config (&SDPowerEnablePin);
	MXC_GPIO_OutClr (MXC_GPIO1, SDPowerEnablePin.mask);
	cfg.bus_voltage = MXC_SDHC_Bus_Voltage_3_3;
	cfg.block_gap = 0;
	cfg.clk_div = 0x0b0;
	if (MXC_SDHC_Init (&cfg) != E_NO_ERROR) {
		printf ("Unable to initialize SDHC driver.\n");
		return 1;
	}

	int ret = MXC_SDHC_Card_Inserted ();
	if (!ret) {
		printf ("SD card not inserted\n\r");
		return -1;
	}

	if (MXC_SDHC_Lib_InitCard (10) != E_NO_ERROR)
		return -1;
	return E_NO_ERROR;
}

int mount ()
{
	fs = &fs_obj;
	if ((err = f_mount (fs, "", 1)) != FR_OK)
		f_mount (NULL, "", 0);
	else
		mounted = 1;
	f_getcwd (cwd, sizeof (cwd));

	return err;
}

int32_t init_and_open_logfile ()
{
	MXC_ICC_Disable ();
	if (sdhc_init () != E_NO_ERROR) {
		printf ("Unable to initialize the SDHC card.\n");
		return -1;
	}
	if ((err = mount ()) != FR_OK) {
		printf ("Error opening SD Card: %s\n\r", FF_ERRORS[err]);
		return -1;
	}
	if ((err = f_setlabel ("MAXIM")) != FR_OK) {
		printf ("Error setting drive label: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	}
	if ((err = f_getfree (&volume, &clusters_free, &fs)) != FR_OK) {
		printf ("Error finding free size of card: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	}

	if ((err = f_getlabel (&volume, volume_label, &volume_sn)) != FR_OK) {
		printf ("Error reading drive label: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	}

	if ((err = f_opendir (&file, "Logs")) != FR_OK) {
		if ((err = f_mkdir ("Logs")) != FR_OK) {
			printf ("Error creating directory: %s\n\r", FF_ERRORS[err]);
			f_mount (NULL, "", 0);
		}
	} else {
		printf ("Opened already existing directory\n\r");
	}

	char file_name[256];
	snprintf (file_name, 256, "Logs/log_%" PRIu64 ".txt",
		  rtcIntrpTimeInMilliSeconds);

	if ((err = f_open (&file, file_name, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK) {
		printf ("Error opening file: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	} else {
		printf ("File opened\n\r");
	}
	return 0;
}

int32_t append_to_logfile (char *msg, uint16_t length)
{
	if ((err = f_write (&file, msg, length, &bytes_written)) != FR_OK) {
		printf ("Error writing file: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	}
	return 0;
}

int32_t close_and_unmount ()
{

	if ((err = f_close (&file)) != FR_OK) {
		printf ("Error closing file: %s\n\r", FF_ERRORS[err]);
		f_mount (NULL, "", 0);
		return -1;
	}
	if ((err = f_mount (NULL, "", 0)) != FR_OK) {
		printf ("Error unmounting volume: %s\n\r", FF_ERRORS[err]);
		return -1;
	}
	printf ("File closed!\n\r");
	return 0;
}
