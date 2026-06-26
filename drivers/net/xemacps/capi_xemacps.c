/***************************************************************************//**
 *   @file   capi_xemacps.c
 *   @brief  CAPI MAC driver for the Xilinx PS GEM (XEmacPs).
 *
 *   Provides a capi_eth_mac_ops implementation for the Zynq-7000 / ZynqMP
 *   PS Gigabit Ethernet MAC.  Uses polling (no IRQs) and copy-mode DMA.
 *   No PHY knowledge — applications wire a CAPI PHY driver to the MDIO
 *   bus exposed via xemacps_mdio_read/xemacps_mdio_write.
 *
 *   BD (buffer descriptor) memory is placed in a dedicated uncached
 *   region following the Xilinx reference implementation
 *   (xemacpsif_dma.c).  Xil_SetTlbAttributes() works on 1 MB L1 page
 *   entries, so a 1 MB static array is reserved and marked
 *   DEVICE_MEMORY.  This static buffer constraint limits the driver to
 *   a single instance.
 *
 *   @author Nicolae-Daniel Deaconescu (Nicolae-daniel.Deaconescu@analog.com)
********************************************************************************
 * Copyright 2026(c) Analog Devices, Inc.
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

#include "capi_xemacps.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include <capi_eth_phy.h>		/* for the shared capi_eth_interface enum */

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
 * @struct xemacps_priv
 * @brief  Private state for the XEmacPs CAPI MAC driver.
 *
 * The first field MUST be `struct capi_eth_mac_handle base` so that
 * the CAPI layer can cast a handle pointer to this private type.
 */
struct xemacps_priv {
	struct capi_eth_mac_handle base;

	XEmacPs emac;
	uint8_t mac_addr[6];

	uint8_t *tx_bd_space;
	uint32_t tx_bd_len;
	uint8_t *rx_bd_space;
	uint32_t rx_bd_len;

	uint32_t last_rx_frms_cntr;

	/* RX peek cache: get_rx_frame_size dequeues a BD from the hardware
	 * "completed" list and stashes it here so the subsequent read_frame
	 * can consume it without doing a second FromHwRx (which wouldn't
	 * see the same BD twice). */
	XEmacPs_Bd *pending_rx_bd;
	uint32_t    pending_rx_len;
};

/* ------------------------------------------------------------------------- */
/* Helpers                                                                   */
/* ------------------------------------------------------------------------- */

static inline struct xemacps_priv *to_priv(struct capi_eth_mac_handle *handle)
{
	return (struct xemacps_priv *)handle;
}

static uint16_t mac_speed_to_mbps(enum capi_eth_mac_speed speed)
{
	switch (speed) {
	case CAPI_ETH_MAC_SPEED_1G:
		return 1000;
	case CAPI_ETH_MAC_SPEED_100M:
		return 100;
	case CAPI_ETH_MAC_SPEED_10M:
		return 10;
	default:
		return 0;
	}
}

/**
 * @brief Re-post RX BDs to hardware after harvesting completed frames.
 */
static int32_t xgem_rx_submit(struct xemacps_priv *d, uint32_t count,
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
 * @brief Run the GEM v2 RX-lockup errata workaround (AR# 52028).
 *
 * Reads RXSR, on `BUFFNA` either flushes the RX DPRAM (always) or
 * toggles RXEN (if the RX frame counter has stopped advancing). On
 * GEM v3+ this is a no-op apart from clearing RXSR. Called from the
 * RX path when no frame is currently available, so the workaround
 * runs every poll cycle without an explicit external entry point.
 */
static void xgem_run_rx_errata(struct xemacps_priv *d)
{
	uint32_t rxsr, rxcnt, regctrl;

	rxsr = XEmacPs_ReadReg(d->emac.Config.BaseAddress, XEMACPS_RXSR_OFFSET);

	if (d->emac.Version != 2)
		goto clear_rxsr;

	if (rxsr & XEMACPS_RXSR_BUFFNA_MASK) {
		regctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
					  XEMACPS_NWCTRL_OFFSET);
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET,
				 regctrl | XEMACPS_NWCTRL_FLUSH_DPRAM_MASK);
	}

	rxcnt = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				XEMACPS_RXCNT_OFFSET);

	if ((rxsr & XEMACPS_RXSR_BUFFNA_MASK) && !rxcnt &&
	    !d->last_rx_frms_cntr) {
		regctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
					  XEMACPS_NWCTRL_OFFSET);
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET,
				 regctrl & ~XEMACPS_NWCTRL_RXEN_MASK);
		dsb();
		(void)XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				      XEMACPS_NWCTRL_OFFSET);
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET,
				 regctrl | XEMACPS_NWCTRL_RXEN_MASK);
		dsb();
	}

	d->last_rx_frms_cntr = rxcnt;

clear_rxsr:
	if (rxsr)
		XEmacPs_WriteReg(d->emac.Config.BaseAddress,
				 XEMACPS_RXSR_OFFSET, rxsr);
}

static void xgem_init_controller(XEmacPs_Config *cfg)
{
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_NWCTRL_OFFSET, 0x00);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_NWCTRL_OFFSET,
			 XEMACPS_NWCTRL_STATCLR_MASK);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXSR_OFFSET, 0x0F);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXSR_OFFSET, 0xFF);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_IDR_OFFSET, 0x7FFFEFF);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXQBASE_OFFSET, 0x0);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXQBASE_OFFSET, 0x0);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_RXQ1BASE_OFFSET, 0x0);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_TXQ1BASE_OFFSET, 0x0);
}

static void xgem_config_controller(XEmacPs_Config *cfg)
{
	uint32_t nwcfg, dmacr, nwctrl;

	nwcfg = XEmacPs_ReadReg(cfg->BaseAddress, XEMACPS_NWCFG_OFFSET);
	nwcfg |= GEM_NWCFG_FEATURES;
	nwcfg &= ~(XEMACPS_NWCFG_SGMIIEN_MASK | XEMACPS_NWCFG_PCSSEL_MASK);
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_NWCFG_OFFSET, nwcfg);

	dmacr = XEmacPs_ReadReg(cfg->BaseAddress, XEMACPS_DMACR_OFFSET);
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
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_DMACR_OFFSET, dmacr);

	nwctrl = XEmacPs_ReadReg(cfg->BaseAddress, XEMACPS_NWCTRL_OFFSET);
	nwctrl |= XEMACPS_NWCTRL_MDEN_MASK;
	XEmacPs_WriteReg(cfg->BaseAddress, XEMACPS_NWCTRL_OFFSET, nwctrl);

	no_os_mdelay(1);
}

/* ------------------------------------------------------------------------- */
/* CAPI MAC ops                                                              */
/* ------------------------------------------------------------------------- */

static int xemacps_op_init(struct capi_eth_mac_handle **handle,
			   const struct capi_eth_mac_init_config *config)
{
	XEmacPs_BdRing *tx_ring, *rx_ring;
	struct xemacps_priv *d;
	XEmacPs_Config *cfg;
	XEmacPs_Bd bd_template;
	XEmacPs_Bd *bdrxterminate = NULL;
	XEmacPs_Bd *bdtxterminate = NULL;
	int ret;

	if (!handle || !config || !config->ops)
		return -EINVAL;

	if (*handle) {
		d = to_priv(*handle);
		d->base.init_allocated = false;
	} else {
		d = no_os_calloc(1, sizeof(*d));
		if (!d)
			return -ENOMEM;
		d->base.init_allocated = true;
	}

	d->base.ops = config->ops;

#if defined (ARMR5)
	Xil_SetTlbAttributes((int32_t)bd_space, STRONG_ORDERD_SHARED | PRIV_RW_USER_RW);
#else
#if defined __aarch64__
	Xil_SetTlbAttributes((uint64_t)bd_space, NORM_NONCACHE | INNER_SHAREABLE);
#else
	Xil_SetTlbAttributes((int32_t)bd_space, DEVICE_MEMORY);
#endif
#endif

#ifdef SDT
	cfg = XEmacPs_LookupConfig((UINTPTR)config->identifier);
#else
	cfg = XEmacPs_LookupConfig((uint16_t)config->identifier);
#endif
	if (!cfg) {
		printf("xemacps: LookupConfig failed for identifier 0x%lx\n",
		       (unsigned long)config->identifier);
		ret = -ENODEV;
		goto err_free;
	}

	ret = XEmacPs_CfgInitialize(&d->emac, cfg, cfg->BaseAddress);
	if (ret != XST_SUCCESS) {
		printf("xemacps: CfgInitialize failed (%d)\n", ret);
		ret = -EIO;
		goto err_free;
	}

	xgem_init_controller(cfg);
	xgem_config_controller(cfg);

	if (config->mac_address) {
		memcpy(d->mac_addr, *config->mac_address, sizeof(d->mac_addr));
		XEmacPs_SetMacAddress(&d->emac, d->mac_addr, 1);
	}
	XEmacPs_SetMdioDivisor(&d->emac, MDC_DIV_224);

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

	if (d->emac.Version > 2) {
		bdrxterminate = (XEmacPs_Bd *)&bd_space[bd_space_index];
		bd_space_index += 0x10000;
		bdtxterminate = (XEmacPs_Bd *)&bd_space[bd_space_index];
		bd_space_index += 0x10000;
	}

	rx_ring = &XEmacPs_GetRxRing(&d->emac);
	ret = XEmacPs_BdRingCreate(rx_ring, (UINTPTR)d->rx_bd_space,
				   (UINTPTR)d->rx_bd_space,
				   XEMACPS_DMABD_MINIMUM_ALIGNMENT,
				   XGEM_RX_BD_COUNT);
	if (ret != XST_SUCCESS) {
		printf("xemacps: RX BdRingCreate failed (%d)\n", ret);
		ret = -EIO;
		goto err_free;
	}

	XEmacPs_BdClear(&bd_template);
	ret = XEmacPs_BdRingClone(rx_ring, &bd_template, XEMACPS_RECV);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto err_free;
	}

	{
		XEmacPs_Bd *rxbd;
		uint32_t bdindex;
		uint32_t *temp;

		for (uint32_t i = 0; i < XGEM_RX_BD_COUNT; i++) {
			ret = XEmacPs_BdRingAlloc(rx_ring, 1, &rxbd);
			if (ret != XST_SUCCESS) {
				ret = -EIO;
				goto err_free;
			}
			ret = XEmacPs_BdRingToHw(rx_ring, 1, rxbd);
			if (ret != XST_SUCCESS) {
				ret = -EIO;
				goto err_free;
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
		goto err_free;
	}

	ret = XEmacPs_BdRingClone(tx_ring, &bd_template, XEMACPS_SEND);
	if (ret != XST_SUCCESS) {
		ret = -EIO;
		goto err_free;
	}

	XEmacPs_SetQueuePtr(&d->emac, d->emac.RxBdRing.BaseBdAddr, 0,
			    XEMACPS_RECV);
	XEmacPs_SetQueuePtr(&d->emac, d->emac.TxBdRing.BaseBdAddr, 0,
			    XEMACPS_SEND);

	/* GEM v3+ has additional queues 1..N which are unused by this
	 * driver but still walked by the controller.  Park them on
	 * "USED+WRAP" terminators so the GEM sees a stable, complete BD
	 * ring on those queues and never tries to DMA from garbage. */
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
		XEmacPs_Out32(cfg->BaseAddress + XEMACPS_TXQ1BASE_OFFSET,
			      (UINTPTR)bdtxterminate);
	}
	dsb();

	*handle = &d->base;
	return 0;

err_free:
	if (d->base.init_allocated)
		no_os_free(d);
	return ret;
}

static int xemacps_op_deinit(struct capi_eth_mac_handle *handle)
{
	struct xemacps_priv *d;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	XEmacPs_Stop(&d->emac);
	if (d->base.init_allocated)
		no_os_free(d);

	return 0;
}

static int xemacps_op_power_control(struct capi_eth_mac_handle *handle,
				    enum capi_eth_mac_power_state state)
{
	struct xemacps_priv *d;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);

	switch (state) {
	case CAPI_ETH_MAC_POWER_FULL:
		XEmacPs_Start(&d->emac);
		return 0;
	case CAPI_ETH_MAC_POWER_OFF:
	case CAPI_ETH_MAC_POWER_LOW:
		XEmacPs_Stop(&d->emac);
		return 0;
	default:
		return -EINVAL;
	}
}

static int xemacps_op_get_capabilities(struct capi_eth_mac_handle *handle,
				       struct capi_eth_mac_capabilities *caps)
{
	if (!handle || !caps)
		return -EINVAL;

	memset(caps, 0, sizeof(*caps));
	caps->media_interface = CAPI_ETH_INTERFACE_RGMII;
	caps->mac_address = 1;

	/* GEM TCP/UDP/IP checksum offload — DMACR.TCPCKSUM and NWCFG.RXCHKSUMEN
	 * are enabled by xgem_config_controller(). */
	caps->checksum_offload_rx_ip4 = 1;
	caps->checksum_offload_rx_udp = 1;
	caps->checksum_offload_rx_tcp = 1;
	caps->checksum_offload_tx_ip4 = 1;
	caps->checksum_offload_tx_udp = 1;
	caps->checksum_offload_tx_tcp = 1;

	return 0;
}

static int xemacps_op_get_address(struct capi_eth_mac_handle *handle,
				  capi_eth_mac_addr addr)
{
	struct xemacps_priv *d;

	if (!handle || !addr)
		return -EINVAL;

	d = to_priv(handle);
	memcpy(addr, d->mac_addr, sizeof(d->mac_addr));
	return 0;
}

static int xemacps_op_set_address(struct capi_eth_mac_handle *handle,
				  const capi_eth_mac_addr addr)
{
	struct xemacps_priv *d;

	if (!handle || !addr)
		return -EINVAL;

	d = to_priv(handle);
	memcpy(d->mac_addr, addr, sizeof(d->mac_addr));
	XEmacPs_SetMacAddress(&d->emac, d->mac_addr, 1);
	return 0;
}

static int xemacps_op_set_address_filter(struct capi_eth_mac_handle *handle,
		const capi_eth_mac_addr *addr_list,
		uint32_t num_addr)
{
	struct xemacps_priv *d;
	uint32_t i;

	if (!handle)
		return -EINVAL;
	if (num_addr > 0 && !addr_list)
		return -EINVAL;

	d = to_priv(handle);

	XEmacPs_WriteReg(d->emac.Config.BaseAddress,
			 XEMACPS_HASHL_OFFSET, 0);
	XEmacPs_WriteReg(d->emac.Config.BaseAddress,
			 XEMACPS_HASHH_OFFSET, 0);

	for (i = 0; i < num_addr; i++)
		XEmacPs_SetHash(&d->emac, (uint8_t *)addr_list[i]);

	return 0;
}

static int xemacps_op_send_frame(struct capi_eth_mac_handle *handle,
				 const uint8_t *frame, uint32_t len,
				 const struct capi_eth_mac_tx_frame_config *cfg)
{
	struct xemacps_priv *d;
	XEmacPs_Bd *bd;
	uint32_t timeout;

	(void)cfg;

	if (!handle || !frame || len == 0)
		return -EINVAL;

	d = to_priv(handle);
	bd = (XEmacPs_Bd *)(void *)d->tx_bd_space;

	memcpy(tx_buf, frame, len);
	Xil_DCacheFlushRange((uintptr_t)tx_buf, len);

	XEmacPs_WriteReg(d->emac.Config.BaseAddress, XEMACPS_TXSR_OFFSET,
			 XEMACPS_TXSR_USEDREAD_MASK | XEMACPS_TXSR_TXCOMPL_MASK);

	XEmacPs_BdSetAddressTx(bd, (UINTPTR)tx_buf);
	XEmacPs_BdWrite(bd, XEMACPS_BD_STAT_OFFSET,
			XEMACPS_TXBUF_WRAP_MASK | XEMACPS_TXBUF_LAST_MASK |
			(len & XEMACPS_TXBUF_LEN_MASK));
	dsb();

	XEmacPs_Transmit(&d->emac);

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

static int xemacps_op_get_rx_frame_size(struct capi_eth_mac_handle *handle,
					uint32_t *size)
{
	struct xemacps_priv *d;
	XEmacPs_BdRing *rx_ring;
	XEmacPs_Bd *bd;

	if (!handle || !size)
		return -EINVAL;

	d = to_priv(handle);

	if (d->pending_rx_bd) {
		*size = d->pending_rx_len;
		return 0;
	}

	rx_ring = &XEmacPs_GetRxRing(&d->emac);

	*size = 0;
	if (XEmacPs_BdRingFromHwRx(rx_ring, 1, &bd) == 0)
		return 0;

	d->pending_rx_bd  = bd;
	d->pending_rx_len = XEmacPs_BdGetLength(bd);
	*size = d->pending_rx_len;
	return 0;
}

static int xemacps_op_read_frame(struct capi_eth_mac_handle *handle,
				 uint8_t *frame, uint32_t len,
				 struct capi_eth_mac_time *timestamp)
{
	struct xemacps_priv *d;
	XEmacPs_BdRing *rx_ring;
	XEmacPs_Bd *bd;
	uint8_t *src;
	uint32_t actual;
	int ret;

	(void)timestamp;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	rx_ring = &XEmacPs_GetRxRing(&d->emac);

	if (d->pending_rx_bd) {
		bd = d->pending_rx_bd;
		actual = d->pending_rx_len;
		d->pending_rx_bd  = NULL;
		d->pending_rx_len = 0;
	} else {
		if (XEmacPs_BdRingFromHwRx(rx_ring, 1, &bd) == 0) {
			/* No frame available — opportunistically run the
			 * GEM v2 RX-lockup errata workaround so the caller
			 * does not need a separate poll entry point. */
			xgem_run_rx_errata(d);
			return 0;
		}
		actual = XEmacPs_BdGetLength(bd);
	}

	src = (uint8_t *)(uintptr_t)(XEmacPs_BdRead(bd, 0U) & ~0x3UL);

	Xil_DCacheInvalidateRange((uintptr_t)src, actual);

	if (frame && len > 0) {
		if (actual > len)
			actual = len;
		memcpy(frame, src, actual);
	}

	XEmacPs_BdClearRxNew(bd);
	XEmacPs_BdRingFree(rx_ring, 1, bd);

	ret = XEmacPs_BdRingAlloc(rx_ring, 1, &bd);
	if (ret != XST_SUCCESS)
		return -ENOMEM;

	ret = xgem_rx_submit(d, 1, bd);
	if (ret != XST_SUCCESS)
		return -EIO;

	return (int)actual;
}

static int xemacps_op_configure(struct capi_eth_mac_handle *handle,
				const struct capi_eth_mac_config *cfg)
{
	struct xemacps_priv *d;
	uint32_t nwcfg;
	uint16_t mbps;

	if (!handle || !cfg)
		return -EINVAL;

	d = to_priv(handle);

	mbps = mac_speed_to_mbps(cfg->speed);
	if (mbps)
		XEmacPs_SetOperatingSpeed(&d->emac, mbps);

	nwcfg = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				XEMACPS_NWCFG_OFFSET);

	if (cfg->duplex == CAPI_ETH_MAC_DUPLEX_FULL)
		nwcfg |= XEMACPS_NWCFG_FDEN_MASK;
	else if (cfg->duplex == CAPI_ETH_MAC_DUPLEX_HALF)
		nwcfg &= ~XEMACPS_NWCFG_FDEN_MASK;

	/* Loopback (internal MAC loopback via NWCTRL.LOCALLOOPBACK) is not
	 * wired up here — leave to follow-up work; ignore the request. */
	(void)cfg->loopback;

	if (cfg->address_broadcast)
		nwcfg &= ~XEMACPS_NWCFG_BCASTDI_MASK;
	else
		nwcfg |= XEMACPS_NWCFG_BCASTDI_MASK;

	if (cfg->address_multicast)
		nwcfg |= XEMACPS_NWCFG_MCASTHASHEN_MASK;
	else
		nwcfg &= ~XEMACPS_NWCFG_MCASTHASHEN_MASK;

	if (cfg->address_all)
		nwcfg |= XEMACPS_NWCFG_COPYALLEN_MASK;
	else
		nwcfg &= ~XEMACPS_NWCFG_COPYALLEN_MASK;

	XEmacPs_WriteReg(d->emac.Config.BaseAddress, XEMACPS_NWCFG_OFFSET, nwcfg);

	return 0;
}

static int xemacps_op_set_tx_enabled(struct capi_eth_mac_handle *handle,
				     bool enable)
{
	struct xemacps_priv *d;
	uint32_t nwctrl;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	nwctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET);
	if (enable)
		nwctrl |= XEMACPS_NWCTRL_TXEN_MASK;
	else
		nwctrl &= ~XEMACPS_NWCTRL_TXEN_MASK;
	XEmacPs_WriteReg(d->emac.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET,
			 nwctrl);
	return 0;
}

static int xemacps_op_set_rx_enabled(struct capi_eth_mac_handle *handle,
				     bool enable)
{
	struct xemacps_priv *d;
	uint32_t nwctrl;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	nwctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET);
	if (enable)
		nwctrl |= XEMACPS_NWCTRL_RXEN_MASK;
	else
		nwctrl &= ~XEMACPS_NWCTRL_RXEN_MASK;
	XEmacPs_WriteReg(d->emac.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET,
			 nwctrl);
	return 0;
}

static int xemacps_op_flush_rx_buffer(struct capi_eth_mac_handle *handle)
{
	struct xemacps_priv *d;
	uint32_t nwctrl;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	nwctrl = XEmacPs_ReadReg(d->emac.Config.BaseAddress,
				 XEMACPS_NWCTRL_OFFSET);
	XEmacPs_WriteReg(d->emac.Config.BaseAddress, XEMACPS_NWCTRL_OFFSET,
			 nwctrl | XEMACPS_NWCTRL_FLUSH_DPRAM_MASK);
	return 0;
}

static int xemacps_op_flush_tx_buffer(struct capi_eth_mac_handle *handle)
{
	(void)handle;
	/* GEM has no separate TX flush — TX BDs drain naturally on Stop/Start. */
	return 0;
}

/* ------------------------------------------------------------------------- */
/* Public helpers (xemacps-specific, not part of CAPI MAC contract)          */
/* ------------------------------------------------------------------------- */

int xemacps_mdio_read(struct capi_eth_mac_handle *handle,
		      uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
{
	struct xemacps_priv *d;

	if (!handle || !data)
		return -EINVAL;

	d = to_priv(handle);
	if (XEmacPs_PhyRead(&d->emac, phy_addr, reg_addr, data) != XST_SUCCESS)
		return -EIO;
	return 0;
}

int xemacps_mdio_write(struct capi_eth_mac_handle *handle,
		       uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
{
	struct xemacps_priv *d;

	if (!handle)
		return -EINVAL;

	d = to_priv(handle);
	if (XEmacPs_PhyWrite(&d->emac, phy_addr, reg_addr, data) != XST_SUCCESS)
		return -EIO;
	return 0;
}

/* ------------------------------------------------------------------------- */
/* Ops table                                                                 */
/* ------------------------------------------------------------------------- */

const struct capi_eth_mac_ops xemacps_capi_mac_ops = {
	.init               = xemacps_op_init,
	.deinit             = xemacps_op_deinit,
	.power_control      = xemacps_op_power_control,
	.get_capabilities   = xemacps_op_get_capabilities,
	.get_address        = xemacps_op_get_address,
	.set_address        = xemacps_op_set_address,
	.set_address_filter = xemacps_op_set_address_filter,
	.send_frame         = xemacps_op_send_frame,
	.get_rx_frame_size  = xemacps_op_get_rx_frame_size,
	.read_frame         = xemacps_op_read_frame,
	.configure          = xemacps_op_configure,
	.set_tx_enabled     = xemacps_op_set_tx_enabled,
	.set_rx_enabled     = xemacps_op_set_rx_enabled,
	.flush_rx_buffer    = xemacps_op_flush_rx_buffer,
	.flush_tx_buffer    = xemacps_op_flush_tx_buffer,
	/* PTP / VLAN / sleep / stats / interrupts: NULL — CAPI wrapper returns -EINVAL */
};
