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
#include <stdbool.h>
#include <stdio.h>

#include <xparameters.h>
#include <xemacps.h>
#include <xil_cache.h>
#include <xil_mmu.h>

#include "no_os_xemacps.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include <capi_eth_phy.h>

#define XGEM_BD_TO_INDEX(ringptr, bdptr) \
	(((UINTPTR)(bdptr) - (UINTPTR)(ringptr)->BaseBdAddr) / (ringptr)->Separation)

/* Uncached memory region for DMA buffer descriptors */
static uint8_t bd_space[0x100000] __attribute__((aligned(0x100000)));
static volatile uint32_t bd_space_index;

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
	bool     link_up;

	struct capi_eth_phy_handle *phy;
};

/* Single-instance XEmacPs reference for MDIO callbacks */
static XEmacPs *xgem_mdio_emac;

static int xgem_mdio_read(uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
	if (!xgem_mdio_emac)
		return -EINVAL;
	if (XEmacPs_PhyRead(xgem_mdio_emac, phy_addr, reg_addr, data) != XST_SUCCESS)
		return -EIO;
	return 0;
}

static int xgem_mdio_write(uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
	if (!xgem_mdio_emac)
		return -EINVAL;
	if (XEmacPs_PhyWrite(xgem_mdio_emac, phy_addr, reg_addr, data) != XST_SUCCESS)
		return -EIO;
	return 0;
}

/**
 * @brief Detect PHY address by scanning MDIO addresses 0-31.
 *
 * Reads PHY ID registers 2 and 3 at each address. A valid PHY returns
 * non-zero, non-0xFFFF values for both registers.
 *
 * @param emac - XEmacPs instance.
 * @return PHY address (0-31) on success, negative error code if not found.
 */
static int8_t xgem_detect_phy(XEmacPs *emac)
{
	uint16_t phyreg1, phyreg2;
	uint8_t addr;
	int ret;

	for (addr = 0; addr <= 31; addr++) {
		ret = XEmacPs_PhyRead(emac, addr,
				      PHY_REG_PHYID1, &phyreg1);
		ret |= XEmacPs_PhyRead(emac, addr,
				       PHY_REG_PHYID2, &phyreg2);

		if ((ret == XST_SUCCESS) &&
		    (phyreg1 > 0x0000) && (phyreg1 < 0xFFFF) &&
		    (phyreg2 > 0x0000) && (phyreg2 < 0xFFFF)) {
			printf("xemacps: PHY found at addr %u "
			       "(ID=0x%04x%04x)\n",
			       addr, phyreg1, phyreg2);
			return addr;
		}
	}

	return -ENODEV;
}

static uint16_t xgem_capi_speed_to_mbps(enum capi_eth_phy_speed speed)
{
	switch (speed) {
	case CAPI_ETH_PHY_SPEED_1G:
		return 1000;
	case CAPI_ETH_PHY_SPEED_100M:
		return 100;
	case CAPI_ETH_PHY_SPEED_10M:
		return 10;
	default:
		return 0;
	}
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
}


void xgem_init_controller(XEmacPs_Config* cfg)
{
	XEmacPs_WriteReg(cfg->BaseAddress,
			 XEMACPS_NWCTRL_OFFSET, 0x00);

	XEmacPs_WriteReg(cfg->BaseAddress,
			 XEMACPS_NWCTRL_OFFSET, XEMACPS_NWCTRL_STATCLR_MASK);

	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXSR_OFFSET, 0x0F);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXSR_OFFSET, 0xFF);

	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_IDR_OFFSET, 0x7FFFEFF);

	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXQBASE_OFFSET, 0x0);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXQBASE_OFFSET, 0x0);

	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXQ1BASE_OFFSET, 0x0);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXQ1BASE_OFFSET, 0x0);
}

void xgem_config_controller(struct xemacps_desc* d, XEmacPs_Config* cfg)
{
	/* Program NWCFG */
	{
		uint32_t nwcfg;

		nwcfg = XEmacPs_ReadReg(cfg->BaseAddress,
					XEMACPS_NWCFG_OFFSET);
		nwcfg |= GEM_NWCFG_FEATURES;

		/* SGMII/PCS only valid on GEM v3+ (ZynqMP) */
		nwcfg &= ~(XEMACPS_NWCFG_SGMIIEN_MASK |
			   XEMACPS_NWCFG_PCSSEL_MASK);

		XEmacPs_WriteReg(cfg->BaseAddress,
				 XEMACPS_NWCFG_OFFSET, nwcfg);
	}

	/* Program DMACR */
	{
		uint32_t dmacr;

		dmacr = XEmacPs_ReadReg(cfg->BaseAddress,
					XEMACPS_DMACR_OFFSET);

		/* RX buffer size in 64-byte units */
		dmacr = (dmacr & ~XEMACPS_DMACR_RXBUF_MASK) |
			(((XGEM_BUFF_SIZE / 64U) << XEMACPS_DMACR_RXBUF_SHIFT)
			 & XEMACPS_DMACR_RXBUF_MASK);

		dmacr |= XEMACPS_DMACR_TCPCKSUM_MASK
			 | XEMACPS_DMACR_TXSIZE_MASK
			 | XEMACPS_DMACR_RXSIZE_MASK;

		dmacr &= ~(XEMACPS_DMACR_ENDIAN_MASK);
		dmacr = (dmacr & ~XEMACPS_DMACR_BLENGTH_MASK)
			| XEMACPS_DMACR_INCR16_AHB_BURST;

#ifdef __aarch64__
		dmacr |= XEMACPS_DMACR_ADDR_WIDTH_64;
#endif

		XEmacPs_WriteReg(cfg->BaseAddress,
				 XEMACPS_DMACR_OFFSET, dmacr);
	}

	{
		uint32_t nwctrl;

		nwctrl = XEmacPs_ReadReg(cfg->BaseAddress,
					 XEMACPS_NWCTRL_OFFSET);

		nwctrl |= XEMACPS_NWCTRL_MDEN_MASK;

		XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_NWCTRL_OFFSET, nwctrl);
	}

	no_os_mdelay(1);
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
	int ret;

	printf("xemacps: init start\n");

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

#if defined (ARMR5)
	Xil_SetTlbAttributes((int32_t)bd_space, STRONG_ORDERD_SHARED | PRIV_RW_USER_RW);
#else
#if defined __aarch64__
	Xil_SetTlbAttributes((uint64_t)bd_space, NORM_NONCACHE | INNER_SHAREABLE);
#else
	Xil_SetTlbAttributes((int32_t)bd_space, DEVICE_MEMORY);
#endif
#endif

	cfg = XEmacPs_LookupConfig(param->device_id);
	if (!cfg) {
		printf("xemacps: LookupConfig failed for device %u\n",
		       param->device_id);
		ret = -ENODEV;
		goto free_desc;
	}

	ret = XEmacPs_CfgInitialize(&d->emac, cfg, cfg->BaseAddress);
	if (ret != XST_SUCCESS) {
		printf("xemacps: CfgInitialize failed (%d)\n", ret);
		ret = -EIO;
		goto free_desc;
	}
	xgem_init_controller(cfg);

	xgem_config_controller(d, cfg);

	XEmacPs_SetMacAddress(&d->emac, param->hwaddr, 1);
	XEmacPs_SetMdioDivisor(&d->emac, MDC_DIV_224);

	xgem_mdio_emac = &d->emac;

	/* Detect or use caller-provided PHY address */
	uint8_t phy_addr = param->phy_addr;
	if (!phy_addr) {
		int8_t detected = xgem_detect_phy(&d->emac);
		if (detected < 0) {
			printf("xemacps: no PHY found on GEM 0x%lx\n",
			       (unsigned long)cfg->BaseAddress);
			ret = -ENODEV;
			goto free_desc;
		}
		phy_addr = (uint8_t)detected;
	}

	/* Initialize PHY through the CAPI layer */
	struct capi_eth_phy_init_config phy_cfg = {
		.phy_addr = phy_addr,
		.fn_read  = xgem_mdio_read,
		.fn_write = xgem_mdio_write,
		.extra    = param->phy_extra,
		.ops      = param->phy_ops,
	};

	ret = capi_eth_phy_init(&d->phy, &phy_cfg);
	if (ret) {
		printf("xemacps: PHY init failed (%d)\n", ret);
		goto free_desc;
	}

	ret = capi_eth_phy_set_interface(d->phy, CAPI_ETH_INTERFACE_RGMII);
	if (ret) {
		printf("xemacps: PHY set_interface failed (%d)\n", ret);
		goto free_phy;
	}

	/* Allocate BD space */
	d->rx_bd_len = XEmacPs_BdRingMemCalc(XEMACPS_DMABD_MINIMUM_ALIGNMENT,
					     XGEM_RX_BD_COUNT);
	bd_space_index = no_os_align(bd_space_index,
				     XEMACPS_DMABD_MINIMUM_ALIGNMENT);
	d->rx_bd_space = &bd_space[bd_space_index];
	bd_space_index += d->rx_bd_len;

	d->tx_bd_len = XEmacPs_BdRingMemCalc(XEMACPS_DMABD_MINIMUM_ALIGNMENT,
					     XGEM_TX_BD_COUNT);
	bd_space_index = no_os_align(bd_space_index,
				     XEMACPS_DMABD_MINIMUM_ALIGNMENT);
	d->tx_bd_space = &bd_space[bd_space_index];
	bd_space_index += d->tx_bd_len;

	XEmacPs_Bd *bdrxterminate = NULL;
	XEmacPs_Bd *bdtxterminate = NULL;
	if (d->emac.Version > 2) {
		bdrxterminate = (XEmacPs_Bd *)&bd_space[bd_space_index];
		bd_space_index += 0x10000;
		bdtxterminate = (XEmacPs_Bd *)&bd_space[bd_space_index];
		bd_space_index += 0x10000;
	}

	/* RX BD ring */
	XEmacPs_Bd bd_template;

	rx_ring = &XEmacPs_GetRxRing(&d->emac);
	ret = XEmacPs_BdRingCreate(rx_ring, (UINTPTR)d->rx_bd_space,
				   (UINTPTR)d->rx_bd_space,
				   XEMACPS_DMABD_MINIMUM_ALIGNMENT,
				   XGEM_RX_BD_COUNT);
	if (ret != XST_SUCCESS) {
		printf("xemacps: RX BdRingCreate failed (%d)\n", ret);
		ret = -EIO;
		goto free_phy;
	}

	XEmacPs_BdClear(&bd_template);
	ret = XEmacPs_BdRingClone(rx_ring, &bd_template, XEMACPS_RECV);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_phy;
	}

	/* Allocate and write buffer address to RX BDs */
	{
		XEmacPs_Bd *rxbd;
		uint32_t bdindex;
		uint32_t *temp;

		for (uint32_t i = 0; i < XGEM_RX_BD_COUNT; i++) {
			ret = XEmacPs_BdRingAlloc(rx_ring, 1, &rxbd);
			if (ret != XST_SUCCESS) {
				ret = -EIO;
				goto free_phy;
			}

			ret = XEmacPs_BdRingToHw(rx_ring, 1, rxbd);
			if (ret != XST_SUCCESS) {
				ret = -EIO;
				goto free_phy;
			}

			bdindex = XGEM_BD_TO_INDEX(rx_ring, rxbd);
			temp = (uint32_t *)rxbd;
			*temp = 0;
			if (bdindex == (XGEM_RX_BD_COUNT - 1))
				*temp = XEMACPS_RXBUF_WRAP_MASK;
			temp++;
			*temp = 0;
			dsb();

			Xil_DCacheInvalidateRange((UINTPTR)rx_buf[bdindex],
						  XGEM_BUFF_SIZE);
			XEmacPs_BdSetAddressRx(rxbd, (UINTPTR)rx_buf[bdindex]);
		}
	}

	/* TX BD ring */
	XEmacPs_BdClear(&bd_template);
	XEmacPs_BdSetStatus(&bd_template, XEMACPS_TXBUF_USED_MASK);

	tx_ring = &XEmacPs_GetTxRing(&d->emac);
	ret = XEmacPs_BdRingCreate(tx_ring, (UINTPTR)d->tx_bd_space,
				   (UINTPTR)d->tx_bd_space,
				   XEMACPS_DMABD_MINIMUM_ALIGNMENT,
				   XGEM_TX_BD_COUNT);
	if (ret != XST_SUCCESS) {
		printf("xemacps: TX BdRingCreate failed (%d)\n", ret);
		ret = -EIO;
		goto free_phy;
	}

	ret = XEmacPs_BdRingClone(tx_ring, &bd_template, XEMACPS_SEND);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto free_phy;
	}

	/* Program queue base registers */
	XEmacPs_SetQueuePtr(&d->emac, d->emac.RxBdRing.BaseBdAddr, 0, XEMACPS_RECV);
	if (d->emac.Version > 2) {
		XEmacPs_SetQueuePtr(&d->emac, d->emac.TxBdRing.BaseBdAddr, 1, XEMACPS_SEND);
	} else {
		XEmacPs_SetQueuePtr(&d->emac, d->emac.TxBdRing.BaseBdAddr, 0, XEMACPS_SEND);
	}

	/* Park unused queues on GEM v3+ (ZynqMP) */
	if (d->emac.Version > 2) {
		XEmacPs_BdClear(bdrxterminate);
		XEmacPs_BdSetAddressRx(bdrxterminate,
				       (XEMACPS_RXBUF_NEW_MASK |
					XEMACPS_RXBUF_WRAP_MASK));
		XEmacPs_Out32(cfg->BaseAddress + XEMACPS_RXQ1BASE_OFFSET,
			      (UINTPTR)bdrxterminate);

		XEmacPs_BdClear(bdtxterminate);
		XEmacPs_BdSetStatus(bdtxterminate,
				    (XEMACPS_TXBUF_USED_MASK |
				     XEMACPS_TXBUF_WRAP_MASK));
		XEmacPs_Out32(cfg->BaseAddress + XEMACPS_TXQBASE_OFFSET,
			      (UINTPTR)bdtxterminate);
	}
	dsb();

	printf("xemacps: configuring PHY mode\n");
	ret = capi_eth_phy_set_mode(d->phy, &param->phy_mode);
	if (ret)
		printf("xemacps: PHY set_mode failed (%d)\n", ret);

	ret = capi_eth_phy_power_control(d->phy, CAPI_ETH_PHY_POWER_FULL);
	if (ret)
		printf("xemacps: PHY power_control failed (%d)\n", ret);

	XEmacPs_Start(&d->emac);

	printf("xemacps: init complete, MAC started\n");
	*desc = d;
	return 0;

free_phy:
	capi_eth_phy_deinit(d->phy);
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
	capi_eth_phy_deinit(desc->phy);
	xgem_mdio_emac = NULL;
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
 * @brief Poll PHY link state via CAPI PHY layer.
 *
 * @param d - Driver descriptor.
 */
static int xgem_poll_link(struct xemacps_desc *d)
{
	struct capi_eth_link_info info;
	bool was_up = d->link_up, is_up;
	uint16_t speed_mbps;
	int ret;

	ret = capi_eth_phy_get_link_info(d->phy, &info);
	if (ret)
		return ret;

	is_up = (info.state == CAPI_ETH_LINK_UP);

	if (is_up == was_up)
		return 0;

	if (is_up) {
		uint32_t nwcfg;

		speed_mbps = xgem_capi_speed_to_mbps(info.speed);
		printf("xemacps: PHY link up at %uMbps %s-duplex\n",
		       speed_mbps,
		       info.duplex == CAPI_ETH_PHY_DUPLEX_FULL ? "full" : "half");
		if (speed_mbps)
			XEmacPs_SetOperatingSpeed(&d->emac, speed_mbps);

		nwcfg = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
					XEMACPS_NWCFG_OFFSET);
		if (info.duplex == CAPI_ETH_PHY_DUPLEX_FULL)
			nwcfg |= XEMACPS_NWCFG_FDEN_MASK;
		else
			nwcfg &= ~XEMACPS_NWCFG_FDEN_MASK;
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCFG_OFFSET, nwcfg);
	} else {
		printf("xemacps: PHY link down\n");
	}

	d->link_up = is_up;

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
	int ret;

	ret = xgem_poll_link(desc);
	if (ret)
		return ret;

	if (!desc->link_up)
		return 0;

	rxsr = XEmacPs_ReadReg(desc->emac.Config.BaseAddress,
			       XEMACPS_RXSR_OFFSET);

	if (desc->emac.Version == 2) {
		xgem_resetrx_on_no_rxdata(desc, rxsr);
	}

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

/**
 * @brief Return the current PHY link state.
 * @param desc - Driver descriptor.
 * @return true if link is up, false otherwise.
 */
bool xemacps_get_link_state(struct xemacps_desc *desc)
{
	return desc->link_up;
}
