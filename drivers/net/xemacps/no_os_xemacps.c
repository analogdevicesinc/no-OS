/***************************************************************************//**
 *   @file   no_os_xemacps.c
 *   @brief  Standalone MAC driver for the Xilinx PS GEM (XEmacPs).
 *
 *   Provides init/remove/send/recv/poll for the Zynq-7000 / ZynqMP PS
 *   Gigabit Ethernet MAC. Uses polling (no IRQs) and copy-mode DMA.
 *   No network stack dependency - the upper layer (e.g. lwIP glue)
 *   calls xemacps_send/recv with raw Ethernet frame buffers.
 *
 *   BD (buffer descriptor) memory is placed in a dedicated uncached region
 *   following the Xilinx reference implementation (xemacpsif_dma.c).
 *   Xil_SetTlbAttributes() only works with L1 page entries (1MB),
 *   so a 1 MB static array is reserved and marked DEVICE_MEMORY.
 *
 *   This driver is limited to a single instance due to the static buffer
 *   arrays (bd_space, tx_buf, rx_buf).
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
********************************************************************************
 * Copyright 2025(c) Analog Devices, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. "AS IS" AND ANY EXPRESS OR
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

#include <string.h>
#include <errno.h>
#include <stdio.h>

#include <xparameters.h>
#include <xemacps.h>
#include <xil_cache.h>
#include <xil_mmu.h>

#include "no_os_xemacps.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"

/* Maximum time to wait for PHY link-up (ms) */
#define XGEM_PHY_LINK_TIMEOUT_MS	5000

/* Uncached memory region for DMA buffer descriptors */
static uint8_t bd_space[0x100000] __attribute__((aligned(0x100000)));
static volatile uint32_t bd_space_index;
static volatile uint32_t bd_space_attr_set;

/* DMA frame data buffers - static, cache-line aligned */
static uint8_t tx_buf[XGEM_BUFF_SIZE]
__attribute__((aligned(XGEM_CACHE_LINE_SIZE)));
static uint8_t rx_buf[XGEM_RX_BD_COUNT][XGEM_BUFF_SIZE]
__attribute__((aligned(XGEM_CACHE_LINE_SIZE)));

/**
 * @struct xemacps_desc
 * @brief  Internal driver state for the XEmacPs MAC driver.
 */
struct xemacps_desc {
	XEmacPs emac;

	uint8_t *tx_bd_space;
	uint32_t tx_bd_len;

	uint8_t *rx_bd_space;
	uint32_t rx_bd_len;

	uint32_t last_rx_frms_cntr;
};

/**
 * @brief Wait for the on-board PHY to complete auto-negotiation.
 * @param emac     - XEmacPs instance.
 * @param phy_addr - MDIO address of the PHY.
 * @return Link speed in Mbps, or negative error code on timeout.
 */
static int32_t xgem_phy_setup(XEmacPs *emac, uint32_t phy_addr)
{
	uint16_t bmcr, bmsr, anlpar;
	uint32_t timeout = XGEM_PHY_LINK_TIMEOUT_MS;
	int32_t ret;

	ret = XEmacPs_PhyRead(emac, phy_addr, PHY_REG_BMCR, &bmcr);
	if (ret != XST_SUCCESS)
		return -EIO;

	bmcr |= PHY_REG_BMCR_ANEGEN | PHY_REG_BMCR_RSTANEG;
	ret = XEmacPs_PhyWrite(emac, phy_addr, PHY_REG_BMCR, bmcr);
	if (ret != XST_SUCCESS)
		return -EIO;

	do {
		XEmacPs_PhyRead(emac, phy_addr, PHY_REG_BMSR, &bmsr);
		ret = XEmacPs_PhyRead(emac, phy_addr, PHY_REG_BMSR, &bmsr);
		if (ret != XST_SUCCESS)
			return -EIO;

		if ((bmsr & PHY_REG_BMSR_LSTATUS) &&
		    (bmsr & PHY_REG_BMSR_ANEGCMPL))
			break;

		no_os_mdelay(1);
		timeout--;
	} while (timeout);

	if (!timeout) {
		printf("xemacps: PHY link timeout\n");
		return -ETIMEDOUT;
	}

	{
		uint16_t stat1000;

		XEmacPs_PhyRead(emac, phy_addr, PHY_REG_1000STAT, &stat1000);
		if (stat1000 & (PHY_REG_1000STAT_LP_FD | PHY_REG_1000STAT_LP_HD))
			return 1000;
	}

	XEmacPs_PhyRead(emac, phy_addr, PHY_REG_ANLPAR, &anlpar);
	if (anlpar & (PHY_REG_ANLPAR_100FD | PHY_REG_ANLPAR_100HD))
		return 100;

	return 10;
}

/**
 * @brief Re-post RX BDs to hardware after harvesting completed frames.
 * @param d     - Driver descriptor.
 * @param count - Number of BDs to re-submit.
 * @param bd    - First BD of the set (from a prior BdRingAlloc).
 * @return 0 on success, negative error code otherwise.
 */
static int32_t xgem_rx_submit(struct xemacps_desc *d, uint32_t count,
			      XEmacPs_Bd *bd)
{
	XEmacPs_BdRing *rx_ring = &XEmacPs_GetRxRing(&d->emac);
	XEmacPs_Bd *cur_bd = bd;
	uint8_t *buf;
	uint32_t i;

	for (i = 0; i < count; i++) {
		buf = (uint8_t *)(uintptr_t)(XEmacPs_BdRead(cur_bd, 0U) & ~0x3UL);
		Xil_DCacheInvalidateRange((uintptr_t)buf, XGEM_BUFF_SIZE);

		XEmacPs_BdWrite(cur_bd, XEMACPS_BD_STAT_OFFSET, 0);
		dsb();

		XEmacPs_BdClearRxNew(cur_bd);

		if (i < count - 1)
			cur_bd = XEmacPs_BdRingNext(rx_ring, cur_bd);
	}

	dsb();

	return XEmacPs_BdRingToHw(rx_ring, count, bd);
}

/**
 * @brief Zynq-7000 GEM v2 silicon errata workaround (AR# 52028).
 *
 * Under heavy traffic of small frames the GEM v2 RX path can lock up
 * when the controller encounters a large number of resource errors.
 * @param d    - Driver descriptor.
 * @param rxsr - Current value of RXSR (already read by the caller).
 */
static void xgem_resetrx_on_no_rxdata(struct xemacps_desc *d, uint32_t rxsr)
{
#ifndef PLATFORM_ZYNQ
	(void)d;
	(void)rxsr;
	return;
#else
	uint32_t rxcnt, regctrl;

	/*
	 * Flush RX packet on every resource error to free
	 * space in the packet buffer
	 */
	if (rxsr & XEMACPS_RXSR_BUFFNA_MASK) {
		regctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
					  XEMACPS_NWCTRL_OFFSET);
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET,
				 regctrl |
				 XEMACPS_NWCTRL_FLUSH_DPRAM_MASK);
	}

	/*
	 * Monitor the frame counter for inactivity. If
	 * BUFFNA is set and the counter has not incremented for two
	 * consecutive reads, the RX data path is locked. Toggle
	 * RXEN to reset it.
	 */
	rxcnt = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				XEMACPS_RXCNT_OFFSET);

	if (!(rxsr & XEMACPS_RXSR_BUFFNA_MASK) || rxcnt ||
	    d->last_rx_frms_cntr)
		goto out;

	regctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				  XEMACPS_NWCTRL_OFFSET);
	regctrl &= ~XEMACPS_NWCTRL_RXEN_MASK;
	XEmacPs_WriteReg(d->emac.Config.BaseAddress,
			 XEMACPS_NWCTRL_OFFSET, regctrl);

	dsb();
	(void)XEmacPs_ReadReg(d->emac.Config.BaseAddress,
			      XEMACPS_NWCTRL_OFFSET);

	regctrl |= XEMACPS_NWCTRL_RXEN_MASK;
	XEmacPs_WriteReg(d->emac.Config.BaseAddress,
			 XEMACPS_NWCTRL_OFFSET, regctrl);
	dsb();

out:
	d->last_rx_frms_cntr = rxcnt;
#endif /* PLATFORM_ZYNQ */
}

/**
 * @brief Initialize the XEmacPs GEM and bring the PHY link up.
 * @param desc  - Set to newly allocated descriptor on success.
 * @param param - Initialization parameters.
 * @return 0 on success, negative error code otherwise.
 */
int xemacps_init(struct xemacps_desc **desc, struct xemacps_init_param *param)
{
	XEmacPs_BdRing *tx_ring, *rx_ring;
	struct xemacps_desc *d;
	XEmacPs_Config *cfg;
	XEmacPs_Bd *bd;
	int32_t speed;
	int ret;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	if (!bd_space_attr_set) {
		Xil_SetTlbAttributes((INTPTR)bd_space, DEVICE_MEMORY);
		bd_space_attr_set = 1;
	}

	d->tx_bd_len = XEmacPs_BdRingMemCalc(XEMACPS_BD_ALIGNMENT, 1);
	d->tx_bd_space = &bd_space[bd_space_index];
	bd_space_index += d->tx_bd_len;

	d->rx_bd_len = XEmacPs_BdRingMemCalc(XEMACPS_BD_ALIGNMENT,
					     XGEM_RX_BD_COUNT);
	d->rx_bd_space = &bd_space[bd_space_index];
	bd_space_index += d->rx_bd_len;

	cfg = XEmacPs_LookupConfig(param->device_id);
	if (!cfg) {
		ret = -ENODEV;
		goto free_desc;
	}

	ret = XEmacPs_CfgInitialize(&d->emac, cfg, cfg->BaseAddress);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_desc;
	}

	XEmacPs_SetMacAddress(&d->emac, param->hwaddr, 1);
	XEmacPs_SetMdioDivisor(&d->emac, MDC_DIV_224);

	speed = xgem_phy_setup(&d->emac, param->phy_addr);
	if (speed < 0) {
		ret = speed;
		goto free_desc;
	}

	printf("xemacps: PHY link up at %ldMbps\n", speed);
	XEmacPs_SetOperatingSpeed(&d->emac, (uint16_t)speed);

	/* Program NWCFG */
	{
		uint32_t nwcfg;

		nwcfg = XEmacPs_ReadReg(cfg->BaseAddress,
					XEMACPS_NWCFG_OFFSET);
		nwcfg |= XEMACPS_NWCFG_FDEN_MASK
			 | XEMACPS_NWCFG_MCASTHASHEN_MASK
			 | XEMACPS_NWCFG_RXCHKSUMEN_MASK
			 | XEMACPS_NWCFG_PAUSEEN_MASK;
		XEmacPs_WriteReg(cfg->BaseAddress,
				 XEMACPS_NWCFG_OFFSET, nwcfg);
	}

	/* Program DMACR */
	{
		uint32_t dmacr;

		dmacr = XEmacPs_ReadReg(cfg->BaseAddress,
					XEMACPS_DMACR_OFFSET);
		dmacr |= XEMACPS_DMACR_TCPCKSUM_MASK;
		dmacr = (dmacr & ~XEMACPS_DMACR_BLENGTH_MASK)
			| XEMACPS_DMACR_INCR16_AHB_BURST;
		XEmacPs_WriteReg(cfg->BaseAddress,
				 XEMACPS_DMACR_OFFSET, dmacr);
	}

	no_os_mdelay(1);

	/* TX BD ring (1 BD) */
	tx_ring = &XEmacPs_GetTxRing(&d->emac);
	ret = XEmacPs_BdRingCreate(tx_ring, (UINTPTR)d->tx_bd_space,
				   (UINTPTR)d->tx_bd_space,
				   XEMACPS_BD_ALIGNMENT, 1);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_desc;
	}

	{
		XEmacPs_Bd bd_template;

		memset(&bd_template, 0, sizeof(bd_template));
		XEmacPs_BdSetTxUsed(&bd_template);
		ret = XEmacPs_BdRingClone(tx_ring, &bd_template, XEMACPS_SEND);
		if (ret != XST_SUCCESS) {
			ret = -EIO;
			goto free_desc;
		}
	}

	dsb();

	/* RX BD ring */
	rx_ring = &XEmacPs_GetRxRing(&d->emac);
	ret = XEmacPs_BdRingCreate(rx_ring, (UINTPTR)d->rx_bd_space,
				   (UINTPTR)d->rx_bd_space,
				   XEMACPS_BD_ALIGNMENT, XGEM_RX_BD_COUNT);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_desc;
	}

	{
		XEmacPs_Bd bd_template;

		memset(&bd_template, 0, sizeof(bd_template));
		ret = XEmacPs_BdRingClone(rx_ring, &bd_template, XEMACPS_RECV);
		if (ret != XST_SUCCESS) {
			ret = -EIO;
			goto free_desc;
		}
	}

	ret = XEmacPs_BdRingAlloc(rx_ring, XGEM_RX_BD_COUNT, &bd);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_desc;
	}

	{
		XEmacPs_Bd *cur_bd = bd;
		uint32_t i;

		for (i = 0; i < XGEM_RX_BD_COUNT; i++) {
			XEmacPs_BdSetAddressRx(cur_bd, (UINTPTR)rx_buf[i]);
			if (i < XGEM_RX_BD_COUNT - 1)
				cur_bd = XEmacPs_BdRingNext(rx_ring, cur_bd);
		}
	}

	ret = xgem_rx_submit(d, XGEM_RX_BD_COUNT, bd);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_desc;
	}

	/* Clear stale status bits from any previous session */
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXSR_OFFSET, 0x0F);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXSR_OFFSET, 0xFF);

	XEmacPs_Start(&d->emac);

	*desc = d;
	return 0;

free_desc:
	no_os_free(d);
	return ret;
}

/**
 * @brief Stop the GEM and release the driver descriptor.
 * @param desc - Descriptor returned by xemacps_init.
 * @return 0 on success.
 */
int xemacps_remove(struct xemacps_desc *desc)
{
	XEmacPs_Stop(&desc->emac);
	no_os_free(desc);

	return 0;
}

/**
 * @brief Transmit a raw Ethernet frame.
 * @param desc - Driver descriptor.
 * @param buf  - Frame data (Ethernet header + payload).
 * @param len  - Frame length in bytes.
 * @return 0 on success, negative error code otherwise.
 */
int xemacps_send(struct xemacps_desc *desc, uint8_t *buf, uint32_t len)
{
	XEmacPs_Bd *bd = (XEmacPs_Bd *)(void *)desc->tx_bd_space;
	uint32_t timeout;

	memcpy(tx_buf, buf, len);
	Xil_DCacheFlushRange((uintptr_t)tx_buf, len);

	XEmacPs_WriteReg(desc->emac.Config.BaseAddress, XEMACPS_TXSR_OFFSET,
			 XEMACPS_TXSR_USEDREAD_MASK | XEMACPS_TXSR_TXCOMPL_MASK);

	XEmacPs_BdSetAddressTx(bd, (UINTPTR)tx_buf);
	XEmacPs_BdWrite(bd, XEMACPS_BD_STAT_OFFSET,
			XEMACPS_TXBUF_WRAP_MASK | XEMACPS_TXBUF_LAST_MASK |
			(len & XEMACPS_TXBUF_LEN_MASK));
	dsb();

	XEmacPs_Transmit(&desc->emac);

	timeout = 200000U;
	do {
		dsb();
		if (XEmacPs_BdRead(bd, XEMACPS_BD_STAT_OFFSET) &
		    XEMACPS_TXBUF_USED_MASK)
			break;
	} while (--timeout);

	if (!timeout)
		return -ETIMEDOUT;

	return 0;
}

/**
 * @brief Receive a single Ethernet frame.
 *
 * Harvests at most one completed RX BD, copies the frame data into
 * the caller's buffer, and reposts the BD to hardware. If no frame
 * is available, returns 0 with *len set to 0.
 *
 * @param desc - Driver descriptor.
 * @param buf  - Buffer to receive frame data (must be >= XEMACPS_MAX_FRAME_SIZE).
 * @param len  - Set to the received frame length, or 0 if no frame available.
 * @return 0 on success, negative error code on BD ring error.
 */
int xemacps_recv(struct xemacps_desc *desc, uint8_t *buf, uint32_t *len)
{
	XEmacPs_BdRing *rx_ring = &XEmacPs_GetRxRing(&desc->emac);
	XEmacPs_Bd *bd;
	uint8_t *frame;
	int ret;

	*len = 0;

	dsb();

	if (XEmacPs_BdRingFromHwRx(rx_ring, 1, &bd) == 0)
		return 0;

	frame = (uint8_t *)(uintptr_t)(XEmacPs_BdRead(bd, 0U) & ~0x3UL);
	*len = XEmacPs_BdGetLength(bd);

	Xil_DCacheInvalidateRange((uintptr_t)frame, *len);

	memcpy(buf, frame, *len);

	/* Return this BD to hardware immediately */
	XEmacPs_BdClearRxNew(bd);
	XEmacPs_BdRingFree(rx_ring, 1, bd);

	ret = XEmacPs_BdRingAlloc(rx_ring, 1, &bd);
	if (ret != XST_SUCCESS)
		return -ENOMEM;

	ret = xgem_rx_submit(desc, 1, bd);
	if (ret != XST_SUCCESS)
		return -EIO;

	return 0;
}

/**
 * @brief Poll the RX status and run the errata workaround.
 *
 * Must be called once per polling cycle, before the xemacps_recv() loop.
 * Reads RXSR, runs the AR# 52028 errata check, and clears any error flags.
 *
 * @param desc - Driver descriptor.
 * @return 0 on success.
 */
int xemacps_poll(struct xemacps_desc *desc)
{
	uint32_t rxsr;

	rxsr = XEmacPs_ReadReg(desc->emac.Config.BaseAddress,
			       XEMACPS_RXSR_OFFSET);

	xgem_resetrx_on_no_rxdata(desc, rxsr);

	if (rxsr)
		XEmacPs_WriteReg(desc->emac.Config.BaseAddress,
				 XEMACPS_RXSR_OFFSET, rxsr);

	return 0;
}

/**
 * @brief Add a multicast MAC address to the GEM hash filter.
 * @param desc - Driver descriptor.
 * @param addr - 6-byte multicast MAC address.
 * @return 0 on success.
 */
int xemacps_set_mcast_hash(struct xemacps_desc *desc, uint8_t *addr)
{
	XEmacPs_SetHash(&desc->emac, addr);

	return 0;
}
