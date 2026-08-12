/***************************************************************************//**
 *   @file   basic_example.c
 *   @brief  ADIOL100 basic IO-Link example — bare-metal, no stack.
 *   @author Liviu Stan (liviu.stan@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
*******************************************************************************/

#include "basic_example.h"
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "adiol100.h"
#include "common_data.h"
#include "no_os_delay.h"
#include "no_os_print_log.h"
#include "no_os_spi.h"

/**
 * @brief Send an IO-Link master frame via the ADIOL100 framer.
 * @param dev - ADIOL100 device handle.
 * @param ch - Channel (A or B).
 * @param mc - MC byte (direction | channel | address).
 * @param ckt - CKT byte (type bits | checksum seed).
 * @param od - On-request data bytes to send (NULL if none).
 * @param od_len - Number of OD bytes.
 * @param rx_bytes - Number of bytes to receive in the device response.
 * @param keep - Whether to keep the response in the RX FIFO.
 * @return 0 on success, negative error code otherwise.
 */
static int iol_send_frame(struct adiol100_dev *dev, enum adiol100_channel ch,
			  uint8_t mc, uint8_t ckt,
			  uint8_t *od, uint8_t od_len,
			  uint8_t rx_bytes, enum adiol100_keep_msg keep)
{
	uint8_t tx[IOL_MAX_OD + 2] = {0}; /* +2 for MC + CKT header */

	tx[0] = mc;
	tx[1] = ckt;
	if (od_len)
		memcpy(&tx[2], od, od_len);

	/* +2 for MC + CKT header */
	int ret = adiol100_load_and_send_msg(dev, ch, tx, 2 + od_len,
					     rx_bytes, keep);
	if (ret)
		return ret;

	/* Conservative delay; sufficient for all COM rates. */
	no_os_mdelay(20);
	return 0;
}

/**
 * @brief Send an IO-Link master frame and read the device response.
 * @param dev - ADIOL100 device handle.
 * @param ch - Channel (A or B).
 * @param mc - MC byte.
 * @param ckt - CKT byte.
 * @param od - On-request data bytes to send (NULL if none).
 * @param od_len - Number of OD bytes.
 * @param rx_bytes - Expected response length.
 * @param keep - Whether to keep the response in the RX FIFO.
 * @param rx - Buffer to receive the response (must be >= rx_bytes).
 * @return 0 on success, negative error code otherwise.
 */
static int iol_send_frame_and_read(struct adiol100_dev *dev,
				   enum adiol100_channel ch,
				   uint8_t mc, uint8_t ckt,
				   uint8_t *od, uint8_t od_len,
				   uint8_t rx_bytes, enum adiol100_keep_msg keep,
				   uint8_t *rx)
{
	int ret;

	ret = iol_send_frame(dev, ch, mc, ckt, od, od_len, rx_bytes, keep);
	if (ret)
		return ret;

	return adiol100_read_msg(dev, ch, rx, rx_bytes);
}

/**
 * @brief Run the basic ADIOL100 example.
 *
 * Demonstrates a full IO-Link communication cycle without an IO-Link stack:
 *   1. STARTUP - establish communication, read device identity from DPP
 *   2. PREOPERATE - read ISDU parameters (vendor name, serial number, etc.)
 *   3. OPERATE - exchange cyclic process data
 *
 * @return 0 on success, negative error code otherwise.
 */
int basic_example_main(void)
{
	struct adiol100_dev *dev;
	uint8_t rx[IOL_MAX_RX_FRAME];
	int ret;

	ret = adiol100_init(&dev, &adiol100_ip);
	if (ret)
		return ret;

	pr_info("-----ADIOL100 Basic Example-----\n");

	/* ════════════════════════════════════════════════════════════════════════
	 *  STARTUP
	 * ══════════════════════════════════════════════════════════════════════ */

	/* Configure transceiver */
	pr_info("Configuring transceiver...\n");
	ret = adiol100_config_lp(dev, ADIOL100_CH_A, ADIOL100_LP_EN,
				 ADIOL100_LP_REV_EN);
	if (ret)
		goto err;

	/* Delay for device wake-up readiness after L+ power-on (spec max 300ms). */
	no_os_mdelay(300);

	ret = adiol100_config_cq(dev, ADIOL100_CH_A, ADIOL100_CQ_PUSHPULL,
				 ADIOL100_CQ_DRV_EN, ADIOL100_SINKSEL_5MA,
				 ADIOL100_CQSLEW_250NS);
	if (ret)
		goto err;
	ret = adiol100_config_framer(dev, ADIOL100_CH_A, ADIOL100_CHKS_EN,
				     ADIOL100_FRAMER_EN, ADIOL100_CKTERR_SEND,
				     ADIOL100_MSGREP_DIS, ADIOL100_BDELAY_3TBIT,
				     0x08, ADIOL100_TSIZEERR_SEND);
	if (ret)
		goto err;

	/* Send EstablishCommunication command */
	pr_info("Sending EstablishCommunication...\n");
	ret = adiol100_estcom(dev, ADIOL100_CH_A);
	if (ret) {
		pr_info("EstCom failed\n");
		goto err;
	}

	uint8_t comrt;
	ret = adiol100_get_comrt(dev, ADIOL100_CH_A, &comrt);
	if (ret)
		goto err;
	pr_info("Slave responded at COM%d\n", comrt);

	/* Read required DPP parameters */
	pr_info("Reading DPP parameters...\n");
	uint8_t min_cyc, mseq_cap, pd_in, pd_out;

	ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
				      IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_MINCYCLETIME),
				      IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG, rx);
	if (ret)
		goto err;
	min_cyc = rx[0];

	ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
				      IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_MSEQCAP),
				      IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG, rx);
	if (ret)
		goto err;
	mseq_cap = rx[0];

	ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
				      IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_PDIN),
				      IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG, rx);
	if (ret)
		goto err;
	pd_in = rx[0];

	ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
				      IOL_MC(IOL_READ, IOL_CH_PAGE, IOL_DPP_PDOUT),
				      IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG, rx);
	if (ret)
		goto err;
	pd_out = rx[0];

	pr_info("MinCycleTime=0x%02X  MSeqCap=0x%02X  PDIn=0x%02X  PDOut=0x%02X\n",
		min_cyc, mseq_cap, pd_in, pd_out);

	int preop_od_bytes = iol_decode_preop_od(mseq_cap);
	int op_od_bytes = iol_decode_operate_od(mseq_cap);
	int pd_in_bytes = iol_decode_pd_len(pd_in);
	int pd_out_bytes = iol_decode_pd_len(pd_out);
	pr_info("PreOp OD=%d  Operate OD=%d  PDIn=%d  PDOut=%d\n",
		preop_od_bytes, op_od_bytes, pd_in_bytes, pd_out_bytes);

	/* Send MasterIdent command */
	uint8_t cmd = IOL_CMD_MASTERIDENT;
	ret = iol_send_frame(dev, ADIOL100_CH_A,
			     IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
			     IOL_CKT_TYPE0, &cmd, 1, 1, ADIOL100_DISCARD_MSG);
	if (ret)
		goto err;
	pr_info("MasterIdent sent\n");

	/* Read optional identification DPP 0x07-0x0D */
	static const struct {
		uint8_t addr;
		const char *name;
	} dpp_id[] = {
		{ IOL_DPP_VENDORID_MSB,   "VendorID MSB"  },
		{ IOL_DPP_VENDORID_LSB,   "VendorID LSB"  },
		{ IOL_DPP_DEVICEID_B1,    "DeviceID b1"   },
		{ IOL_DPP_DEVICEID_B2,    "DeviceID b2"   },
		{ IOL_DPP_DEVICEID_B3,    "DeviceID b3"   },
		{ IOL_DPP_FUNCTIONID_MSB, "FunctionID MSB"},
		{ IOL_DPP_FUNCTIONID_LSB, "FunctionID LSB"},
	};

	for (int i = 0; i < (int)NO_OS_ARRAY_SIZE(dpp_id); i++) {
		uint8_t val;
		ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
					      IOL_MC(IOL_READ, IOL_CH_PAGE, dpp_id[i].addr),
					      IOL_CKT_TYPE0, NULL, 0, 2, ADIOL100_DISCARD_MSG, rx);
		if (ret)
			goto err;
		val = rx[0];
		pr_info("DPP 0x%02X %-16s = 0x%02X\n",
			dpp_id[i].addr, dpp_id[i].name, val);
	}

	/* Send Preoperate command */
	cmd = IOL_CMD_PREOPERATE;
	ret = iol_send_frame(dev, ADIOL100_CH_A,
			     IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
			     IOL_CKT_TYPE0, &cmd, 1, 1, ADIOL100_DISCARD_MSG);
	if (ret)
		goto err;
	pr_info("SetPreOp sent\n");

	/* ════════════════════════════════════════════════════════════════════════
	 *  PREOPERATE
	 * ══════════════════════════════════════════════════════════════════════ */

	/* Read ISDU pages (OPTIONAL) */
	pr_info("ISDU reads...\n");
	{
		static const int isdu_indices[] = {
			IOL_ISDU_SERIALNUMBER, IOL_ISDU_VENDORNAME, IOL_ISDU_VENDORTEXT,
			IOL_ISDU_PRODUCTNAME,  IOL_ISDU_PRODUCTID,  IOL_ISDU_PRODUCTTEXT,
		};

		for (int p = 0; p < (int)NO_OS_ARRAY_SIZE(isdu_indices); p++) {
			uint8_t od[IOL_MAX_OD + 1] = {0}; /* +1 for CKS */
			uint8_t result[IOL_MAX_ISDU_DATA];
			int result_len = 0;
			int data_len;
			int data_start;
			int polls;

			/* Send ISDU read request */
			memcpy(od, (uint8_t[])IOL_ISDU_READ_CMD(isdu_indices[p]), 3);
			ret = iol_send_frame(dev, ADIOL100_CH_A,
					     IOL_MC(IOL_WRITE, IOL_CH_ISDU, IOL_ISDU_START),
					     IOL_CKT_TYPE1, od, preop_od_bytes, 1,
					     ADIOL100_DISCARD_MSG);
			if (ret) {
				pr_info("  ISDU %d: send failed\n", isdu_indices[p]);
				continue;
			}

			/* Poll until device is ready */
			polls = 0;
			do {
				ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
							      IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_START),
							      IOL_CKT_TYPE1, NULL, 0,
							      preop_od_bytes + 1, ADIOL100_DISCARD_MSG, od);
				if (ret)
					break;
				polls++;
			} while (od[0] == IOL_ISDU_BUSY && polls < 100);

			if (ret || od[0] == IOL_ISDU_BUSY) {
				pr_info("  ISDU %d: %s\n", isdu_indices[p],
					ret ? "read failed" : "timeout");
				ret = iol_send_frame(dev, ADIOL100_CH_A,
						     IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
						     IOL_CKT_TYPE1, NULL, 0,
						     preop_od_bytes + 1, ADIOL100_DISCARD_MSG);
				if (ret)
					goto err;
				ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
				if (ret)
					goto err;
				continue;
			}

			/* Parse response header */
			if ((od[0] & IOL_ISDU_ISERVICE_MSK) == IOL_ISDU_READ_ERROR) {
				ret = iol_send_frame(dev, ADIOL100_CH_A,
						     IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
						     IOL_CKT_TYPE1, NULL, 0,
						     preop_od_bytes + 1, ADIOL100_DISCARD_MSG);
				if (ret)
					goto err;
				ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
				if (ret)
					goto err;
				pr_info("  ISDU %d: error\n", isdu_indices[p]);
				continue;
			}

			if ((od[0] & IOL_ISDU_LEN_MSK) == IOL_ISDU_LONG_FORM) {
				data_len = od[1] - IOL_ISDU_LONG_HDR_OVERHEAD;
				data_start = 2;
			} else {
				data_len = (od[0] & IOL_ISDU_LEN_MSK) - IOL_ISDU_SHORT_HDR_OVERHEAD;
				data_start = 1;
			}

			for (int i = data_start; i < preop_od_bytes; i++)
				result[result_len++] = od[i];

			/* Collect continuation pages */
			int page = 1;
			while (result_len < data_len) {
				ret = iol_send_frame_and_read(dev, ADIOL100_CH_A,
							      IOL_MC(IOL_READ, IOL_CH_ISDU, page),
							      IOL_CKT_TYPE1, NULL, 0,
							      preop_od_bytes + 1, ADIOL100_DISCARD_MSG, od);
				if (ret)
					break;
				page++;
				for (int i = 0; i < preop_od_bytes && result_len < data_len; i++)
					result[result_len++] = od[i];
			}

			/* Close ISDU transaction */
			ret = iol_send_frame(dev, ADIOL100_CH_A,
					     IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
					     IOL_CKT_TYPE1, NULL, 0,
					     preop_od_bytes + 1, ADIOL100_DISCARD_MSG);
			if (ret)
				goto err;
			ret = adiol100_reset_rx_fifo(dev, ADIOL100_CH_A);
			if (ret)
				goto err;

			pr_info("  ISDU %d (len=%d): ", isdu_indices[p], data_len);
			for (int i = 0; i < data_len; i++)
				pr_info("%c", result[i]);
			pr_info("\n");
		}
	}

	/* Set cycle time */
	{
		uint8_t od[IOL_MAX_OD] = {0};
		od[0] = min_cyc;
		ret = iol_send_frame(dev, ADIOL100_CH_A,
				     IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_CYCLETIME),
				     IOL_CKT_TYPE1, od, preop_od_bytes, 1, ADIOL100_DISCARD_MSG);
		if (ret)
			goto err;
		pr_info("SetCycleTime=0x%02X\n", min_cyc);
	}

	/* Send Operate command */
	{
		uint8_t od[IOL_MAX_OD] = {0};
		od[0] = IOL_CMD_OPERATE;
		ret = iol_send_frame(dev, ADIOL100_CH_A,
				     IOL_MC(IOL_WRITE, IOL_CH_PAGE, IOL_DPP_MASTERCMD),
				     IOL_CKT_TYPE1, od, preop_od_bytes, 1, ADIOL100_DISCARD_MSG);
		if (ret)
			goto err;
		pr_info("SetOperate sent\n");
	}

	/* ════════════════════════════════════════════════════════════════════════
	 *  OPERATE
	 * ══════════════════════════════════════════════════════════════════════ */

	/* Request cyclic transfers */
	pr_info("Starting cyclic process data exchange...\n");
	ret = adiol100_enable_cycle_timer(dev, ADIOL100_CH_A);
	if (ret)
		goto err;
	uint8_t cyc_len = op_od_bytes + pd_in_bytes + 1;

	ret = iol_send_frame(dev, ADIOL100_CH_A,
			     IOL_MC(IOL_READ, IOL_CH_ISDU, IOL_ISDU_IDLE),
			     IOL_CKT_TYPE2, NULL, 0, cyc_len, ADIOL100_KEEP_MSG);
	if (ret)
		goto err;

	for (int cyc = 0; cyc < 20; cyc++) {
		ret = adiol100_read_msg(dev, ADIOL100_CH_A, rx, cyc_len);
		if (ret) {
			/* Poll interval between read attempts. */
			no_os_mdelay(50);
			continue;
		}
		int16_t dist = (int16_t)no_os_get_unaligned_be16(&rx[op_od_bytes]);
		pr_info("Cyclic #%d dist=%dmm raw(%d):", cyc, dist, cyc_len);
		for (int i = 0; i < cyc_len; i++)
			pr_info(" %02X", rx[i]);
		pr_info("\n");
		/* Poll interval between read attempts. */
		no_os_mdelay(50);
	}

	adiol100_remove(dev);
	return 0;

err:
	adiol100_remove(dev);
	return ret;
}
