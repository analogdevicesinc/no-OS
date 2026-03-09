/***************************************************************************//**
 *   @file   adrv903x.c
 *   @brief  ADRV903X (ADRV9030/ADRV9032) no-OS driver implementation.
 *   @author Analog Devices Inc.
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders. This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "adrv903x.h"
#include "adi_adrv903x_core.h"
#include "adi_adrv903x_utilities.h"
#include "adi_common_error.h"
#include "no_os_alloc.h"
#include "no_os_clk.h"
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "app_config.h"
#include <string.h>

/******************************************************************************/
/********************** Functions Implementation ******************************/
/******************************************************************************/

/**
 * @brief Initialize ADRV903X device.
 *
 * Step 3: Extends Step 2 (SPI verification) with full firmware loading.
 *
 * Sequence:
 *   1. Allocate and configure device structure
 *   2. adi_adrv903x_HwOpen        — open SPI bus and GPIO
 *   3. adi_adrv903x_HwReset       — hardware reset pulse
 *   4. adi_adrv903x_SpiVerify     — loopback SPI test
 *   5. adi_adrv903x_ApiVersionGet — confirm device responds
 *   6. adi_adrv903x_InitDataExtract — extract init structs from profile binary
 *   7. adi_adrv903x_PreMcsInit    — load stream/FW/profile/gain tables; start ARM CPU
 *   8. adi_adrv903x_PreMcsInit_NonBroadcast — per-device non-broadcast config
 *
 * @param phy        - Output pointer to the allocated device structure.
 * @param init_param - Initialization parameters.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_init(struct adrv903x_rf_phy **phy,
		  const struct adrv903x_init_param *init_param)
{
	const char *dev_name = "ADRV903X";
	adi_adrv903x_ExtractInitDataOutput_e extractOutput;
	adi_adrv903x_CpuFwVersion_t armVersion = { 0 };
	adi_adrv903x_Version_t streamVersion = { 0 };
	adi_adrv903x_Version_t apiVersion = { 0 };
	adi_common_ErrData_t *errPtr;
	struct adrv903x_rf_phy *p;
	int ret;

	if (!phy || !init_param)
		return -EINVAL;

	p = no_os_calloc(1, sizeof(*p));
	if (!p)
		return -ENOMEM;

	p->dev_clk     = init_param->dev_clk;
	p->palmaDevice = &p->adi_adrv903x_device;
	p->spiSettings = *adrv903x_spi_settings_get();

	/* Point the API device's HAL handle at our hal struct */
	p->palmaDevice->common.devHalInfo = &p->hal;

	/* Allocate error data block required by the API */
	errPtr = no_os_calloc(1, sizeof(*errPtr));
	if (!errPtr) {
		ret = -ENOMEM;
		goto error_free_phy;
	}
	p->palmaDevice->common.errPtr           = errPtr;
	p->palmaDevice->common.deviceInfo.id   = 0x9030;
	p->palmaDevice->common.deviceInfo.name = dev_name;
	p->palmaDevice->common.deviceInfo.type = 0x00;

	/* Set up file paths for embedded firmware binaries */
	strncpy((char *)p->trxBinaryInfoPtr.cpuProfile.filePath,
		ADRV903X_PROFILE_FILE,
		sizeof(p->trxBinaryInfoPtr.cpuProfile.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.cpu.filePath,
		ADRV903X_CPU_FW_FILE,
		sizeof(p->trxBinaryInfoPtr.cpu.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.stream.filePath,
		ADRV903X_STREAM_FILE,
		sizeof(p->trxBinaryInfoPtr.stream.filePath) - 1);
	strncpy((char *)p->trxBinaryInfoPtr.rxGainTable[0].filePath,
		ADRV903X_RX_GAIN_TABLE_FILE,
		sizeof(p->trxBinaryInfoPtr.rxGainTable[0].filePath) - 1);
	p->trxBinaryInfoPtr.rxGainTable[0].channelMask = ADRV903X_RX_GAIN_TABLE_MASK;

	if (p->dev_clk) {
		ret = no_os_clk_enable(p->dev_clk);
		if (ret)
			goto error_free_errptr;
	}

	/* Open SPI bus and configure GPIO reset */
	ret = adi_adrv903x_HwOpen(p->palmaDevice, &p->spiSettings);
	if (ret) {
		pr_err("adrv903x: HwOpen failed (%d)\n", ret);
		goto error_disable_clk;
	}

	/* Pulse hardware reset */
	ret = adi_adrv903x_HwReset(p->palmaDevice);
	if (ret) {
		pr_err("adrv903x: HwReset failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Verify SPI communication via scratchpad loopback */
	ret = adi_adrv903x_SpiVerify(p->palmaDevice);
	if (ret) {
		pr_err("adrv903x: SpiVerify failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Read API version to confirm device is responding */
	ret = adi_adrv903x_ApiVersionGet(p->palmaDevice, &apiVersion);
	if (ret) {
		pr_err("adrv903x: ApiVersionGet failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x-phy Rev %d, API version: %u.%u.%u.%u found\n",
		p->palmaDevice->devStateInfo.deviceSiRev,
		apiVersion.majorVer, apiVersion.minorVer,
		apiVersion.maintenanceVer, apiVersion.buildVer);

	/* Extract init structs from the embedded device profile binary */
	ret = adi_adrv903x_InitDataExtract(p->palmaDevice,
					   &p->trxBinaryInfoPtr.cpuProfile,
					   &apiVersion, &armVersion,
					   &streamVersion,
					   &p->deviceInitStruct,
					   &p->utilityInit, &extractOutput);
	if (ret) {
		pr_err("adrv903x: InitDataExtract failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x: FW %u.%u.%u.%u, stream %u.%u.%u.%u\n",
		armVersion.commVer.majorVer, armVersion.commVer.minorVer,
		armVersion.commVer.maintenanceVer, armVersion.commVer.buildVer,
		streamVersion.majorVer, streamVersion.minorVer,
		streamVersion.maintenanceVer, streamVersion.buildVer);

	/*
	 * PreMcsInit: load stream image, ARM CPU firmware, device profile and
	 * RX gain tables into the chip; start the ARM CPU.
	 */
	ret = adi_adrv903x_PreMcsInit(p->palmaDevice, &p->deviceInitStruct,
				      &p->trxBinaryInfoPtr);
	if (ret) {
		pr_err("adrv903x: PreMcsInit failed (%d)\n", ret);
		goto error_hw_close;
	}

	/* Non-broadcast portion of pre-MCS initialization */
	ret = adi_adrv903x_PreMcsInit_NonBroadcast(p->palmaDevice,
						    &p->deviceInitStruct);
	if (ret) {
		pr_err("adrv903x: PreMcsInit_NonBroadcast failed (%d)\n", ret);
		goto error_hw_close;
	}

	pr_info("adrv903x: firmware loaded, ARM CPU running\n");

	p->is_initialized = true;
	*phy = p;
	return 0;

error_hw_close:
	adi_hal_HwClose(p->palmaDevice->common.devHalInfo);
error_disable_clk:
	if (p->dev_clk)
		no_os_clk_disable(p->dev_clk);
error_free_errptr:
	no_os_free(errPtr);
error_free_phy:
	no_os_free(p);
	return ret;
}

/**
 * @brief Free resources allocated by adrv903x_init().
 *
 * @param phy - Pointer to device structure.
 *
 * @return 0 on success, negative error code on failure.
 */
int adrv903x_remove(struct adrv903x_rf_phy *phy)
{
	if (!phy)
		return -EINVAL;

	adi_hal_HwClose(phy->palmaDevice->common.devHalInfo);

	if (phy->dev_clk)
		no_os_clk_disable(phy->dev_clk);

	no_os_free(phy->palmaDevice->common.errPtr);
	no_os_free(phy);
	return 0;
}
