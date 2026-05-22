/***************************************************************************//**
 *   @file   adin1140.c
 *   @brief  Source file of the ADIN1140 driver.
 *   @author Ciprian Regus (ciprian.regus@analog.com)
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

#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "adin1140.h"
#include "no_os_alloc.h"
#include "no_os_util.h"

/**
 * @brief Write a register's value.
 * @param desc - the device descriptor.
 * @param addr - MMS-encoded register address.
 * @param data - register value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_reg_write(struct adin1140_desc *desc, uint32_t addr, uint32_t data)
{
	return oa_tc6_reg_write(desc->oa_desc, addr, data);
}

/**
 * @brief Read a register's value.
 * @param desc - the device descriptor.
 * @param addr - MMS-encoded register address.
 * @param data - pointer to store the register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_reg_read(struct adin1140_desc *desc, uint32_t addr, uint32_t *data)
{
	return oa_tc6_reg_read(desc->oa_desc, addr, data);
}

/**
 * @brief Update a register's value based on a mask.
 * @param desc - the device descriptor.
 * @param addr - MMS-encoded register address.
 * @param mask - the bits that may be modified.
 * @param data - new value for the masked bits.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_reg_update(struct adin1140_desc *desc, uint32_t addr,
			uint32_t mask, uint32_t data)
{
	uint32_t val;
	int ret;

	ret = adin1140_reg_read(desc, addr, &val);
	if (ret)
		return ret;

	val &= ~mask;
	val |= mask & data;

	return adin1140_reg_write(desc, addr, val);
}

/**
 * @brief Map a C45 MMD device number to the ADIN1140 MMS index.
 * @param devnum - MMD device number.
 * @return MMS index, or negative error code if unsupported.
 */
static int adin1140_get_phy_c45_mms(uint32_t devnum)
{
	switch (devnum) {
	case ADIN1140_MMD_PCS:
		return ADIN1140_PHY_C45_PCS_MMS;
	case ADIN1140_MMD_PMA_PMD:
		return ADIN1140_PHY_C45_PMA_PMD_MMS;
	case ADIN1140_MMD_VEND2:
		return ADIN1140_PHY_C45_VS_PLCA_MMS;
	default:
		return -EINVAL;
	}
}

/**
 * @brief Read a PHY register using clause 45 via MMS.
 * @param desc - the device descriptor.
 * @param devnum - MMD device number (1=PMA/PMD, 3=PCS, 31=VEND2).
 * @param reg - register address within the MMD.
 * @param data - pointer to store the register value.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_mdio_read_c45(struct adin1140_desc *desc, uint32_t devnum,
			   uint16_t reg, uint16_t *data)
{
	uint32_t val;
	int mms;
	int ret;

	mms = adin1140_get_phy_c45_mms(devnum);
	if (mms < 0)
		return mms;

	ret = oa_tc6_reg_read(desc->oa_desc, OA_MMS_REG(mms, reg), &val);
	if (ret)
		return ret;

	*data = (uint16_t)(val & 0xFFFF);

	return 0;
}

/**
 * @brief Write a PHY register using clause 45 via MMS.
 * @param desc - the device descriptor.
 * @param devnum - MMD device number (1=PMA/PMD, 3=PCS, 31=VEND2).
 * @param reg - register address within the MMD.
 * @param data - value to write.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_mdio_write_c45(struct adin1140_desc *desc, uint32_t devnum,
			    uint16_t reg, uint16_t data)
{
	int mms;

	mms = adin1140_get_phy_c45_mms(devnum);
	if (mms < 0)
		return mms;

	return oa_tc6_reg_write(desc->oa_desc, OA_MMS_REG(mms, reg),
				(uint32_t)data);
}

/**
 * @brief Write a MAC address filter with mask to a specific slot.
 * @param desc - the device descriptor.
 * @param addr - 6-byte MAC address.
 * @param mask - 6-byte MAC address mask.
 * @param slot - filter slot index (0 to ADIN1140_MAC_FILT_MAX_SLOT - 1).
 * @return 0 in case of success, negative error code otherwise.
 */
static int adin1140_mac_filter_write(struct adin1140_desc *desc,
				     const uint8_t *addr,
				     const uint8_t *mask, uint8_t slot)
{
	uint32_t val;
	int ret;

	if (slot >= ADIN1140_MAC_FILT_MAX_SLOT)
		return -EINVAL;

	val = no_os_get_unaligned_be16((uint8_t *)&addr[0]);
	val |= ADIN1140_MAC_ADDR_APPLY2PORT1 | ADIN1140_MAC_ADDR_TO_HOST;
	ret = adin1140_reg_write(desc, ADIN1140_MAC_ADDR_FILT_UPR_REG(slot), val);
	if (ret)
		return ret;

	val = no_os_get_unaligned_be32((uint8_t *)&addr[2]);
	ret = adin1140_reg_write(desc, ADIN1140_MAC_ADDR_FILT_LWR_REG(slot), val);
	if (ret)
		return ret;

	val = no_os_get_unaligned_be16((uint8_t *)&mask[0]);
	ret = adin1140_reg_write(desc, ADIN1140_MAC_ADDR_MASK_UPR_REG(slot), val);
	if (ret)
		return ret;

	val = no_os_get_unaligned_be32((uint8_t *)&mask[2]);

	return adin1140_reg_write(desc, ADIN1140_MAC_ADDR_MASK_LWR_REG(slot), val);
}

/**
 * @brief Clear a MAC address filter slot.
 * @param desc - the device descriptor.
 * @param slot - filter slot index.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adin1140_mac_filter_clear(struct adin1140_desc *desc, uint8_t slot)
{
	uint8_t mask[ADIN1140_ETH_ALEN];
	uint8_t addr[ADIN1140_ETH_ALEN];

	memset(mask, 0xFF, ADIN1140_ETH_ALEN);
	memset(addr, 0x00, ADIN1140_ETH_ALEN);

	return adin1140_mac_filter_write(desc, addr, mask, slot);
}

/**
 * @brief Set the unicast MAC address filter.
 * @param desc - the device descriptor.
 * @param mac_address - the MAC address to filter on.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_set_mac_addr(struct adin1140_desc *desc,
			  uint8_t mac_address[ADIN1140_ETH_ALEN])
{
	uint8_t mask[ADIN1140_ETH_ALEN];

	memcpy(desc->mac_address, mac_address, ADIN1140_ETH_ALEN);
	memset(mask, 0xFF, ADIN1140_ETH_ALEN);

	return adin1140_mac_filter_write(desc, mac_address, mask,
					 ADIN1140_MAC_FILT_UNICAST_SLOT);
}

/**
 * @brief Enable/disable the broadcast filter.
 * @param desc - the device descriptor.
 * @param enabled - true to enable, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_broadcast_filter(struct adin1140_desc *desc, bool enabled)
{
	uint8_t bcast[ADIN1140_ETH_ALEN];
	uint8_t mask[ADIN1140_ETH_ALEN];

	if (!enabled)
		return adin1140_mac_filter_clear(desc,
						 ADIN1140_MAC_FILT_BROADCAST_SLOT);

	memset(bcast, 0xFF, ADIN1140_ETH_ALEN);
	memset(mask, 0xFF, ADIN1140_ETH_ALEN);

	return adin1140_mac_filter_write(desc, bcast, mask,
					 ADIN1140_MAC_FILT_BROADCAST_SLOT);
}

/**
 * @brief Enable/disable the all-multicast filter.
 *        Matches any frame with the multicast bit set in the destination MAC.
 * @param desc - the device descriptor.
 * @param enabled - true to enable, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_multicast_filter(struct adin1140_desc *desc, bool enabled)
{
	uint8_t mcast_addr[ADIN1140_ETH_ALEN] = {0x01, 0x00, 0x00,
						  0x00, 0x00, 0x00};

	if (!enabled)
		return adin1140_mac_filter_clear(desc,
						 ADIN1140_MAC_FILT_MULTICAST_SLOT);

	return adin1140_mac_filter_write(desc, mcast_addr, mcast_addr,
					 ADIN1140_MAC_FILT_MULTICAST_SLOT);
}

/**
 * @brief Set/clear promiscuous mode via CONFIG2 FWD_UNK2HOST.
 * @param desc - the device descriptor.
 * @param promisc - true to enable promiscuous mode, false to disable.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_set_promisc(struct adin1140_desc *desc, bool promisc)
{
	return adin1140_reg_update(desc, ADIN1140_CONFIG2_REG,
				   ADIN1140_CONFIG2_FWD_UNK2HOST,
				   promisc ? ADIN1140_CONFIG2_FWD_UNK2HOST : 0);
}

/**
 * @brief Apply the MAC CONFIG0 bitfields exposed via struct adin1140_mac_cfg.
 *        TXFCSVE is always cleared so the MAC appends the FCS itself.
 *        SYNC is preserved (read-modify-write).
 * @param desc - the device descriptor.
 * @param cfg - configuration values to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_mac_set_cfg(struct adin1140_desc *desc,
			 const struct adin1140_mac_cfg *cfg)
{
	uint32_t mask;
	uint32_t val;

	mask = ADIN1140_CONFIG0_TXFCSVE |
	       ADIN1140_CONFIG0_CSARFE |
	       ADIN1140_CONFIG0_ZARFE |
	       ADIN1140_CONFIG0_TXCTHRESH_MASK |
	       ADIN1140_CONFIG0_TXCTE |
	       ADIN1140_CONFIG0_RXCTE |
	       ADIN1140_CONFIG0_FTSE |
	       ADIN1140_CONFIG0_FTSS |
	       ADIN1140_CONFIG0_CPS_MASK;

	val = no_os_field_prep(ADIN1140_CONFIG0_TXCTHRESH_MASK, cfg->txcthresh) |
	      no_os_field_prep(ADIN1140_CONFIG0_CPS_MASK, cfg->cps);

	if (cfg->csarfe)
		val |= ADIN1140_CONFIG0_CSARFE;
	if (cfg->zarfe)
		val |= ADIN1140_CONFIG0_ZARFE;
	if (cfg->txcte)
		val |= ADIN1140_CONFIG0_TXCTE;
	if (cfg->rxcte)
		val |= ADIN1140_CONFIG0_RXCTE;
	if (cfg->ftse)
		val |= ADIN1140_CONFIG0_FTSE;
	if (cfg->ftss)
		val |= ADIN1140_CONFIG0_FTSS;

	return adin1140_reg_update(desc, ADIN1140_CONFIG0_REG, mask, val);
}

/**
 * @brief Read back the MAC CONFIG0 bitfields into struct adin1140_mac_cfg.
 * @param desc - the device descriptor.
 * @param cfg - pointer to store the configuration values.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_mac_get_cfg(struct adin1140_desc *desc,
			 struct adin1140_mac_cfg *cfg)
{
	uint32_t val;
	int ret;

	ret = adin1140_reg_read(desc, ADIN1140_CONFIG0_REG, &val);
	if (ret)
		return ret;

	cfg->cps = no_os_field_get(ADIN1140_CONFIG0_CPS_MASK, val);
	cfg->txcthresh = no_os_field_get(ADIN1140_CONFIG0_TXCTHRESH_MASK, val);
	cfg->csarfe = !!(val & ADIN1140_CONFIG0_CSARFE);
	cfg->zarfe = !!(val & ADIN1140_CONFIG0_ZARFE);
	cfg->txcte = !!(val & ADIN1140_CONFIG0_TXCTE);
	cfg->rxcte = !!(val & ADIN1140_CONFIG0_RXCTE);
	cfg->ftse = !!(val & ADIN1140_CONFIG0_FTSE);
	cfg->ftss = !!(val & ADIN1140_CONFIG0_FTSS);

	return 0;
}

/**
 * @brief Trigger a MAC-PHY soft reset via OA-TC6 SWRESET, then re-assert
 *        the CONFIG0.SYNC bit so the device is ready to handle traffic.
 * @param desc - the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_sw_reset(struct adin1140_desc *desc)
{
	int ret;

	ret = oa_tc6_sw_reset(desc->oa_desc);
	if (ret)
		return ret;

	return adin1140_reg_update(desc, ADIN1140_CONFIG0_REG,
				   ADIN1140_CONFIG0_SYNC,
				   ADIN1140_CONFIG0_SYNC);
}

/**
 * @brief Write a frame to the TX FIFO via OA TC6.
 * @param desc - the device descriptor.
 * @param eth_buff - the frame to be transmitted.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_write_fifo(struct adin1140_desc *desc,
			struct adin1140_eth_buff *eth_buff)
{
	struct oa_tc6_frame_buffer *oa_frame;
	uint32_t frame_offset = 0;
	int ret;

	ret = oa_tc6_get_tx_frame(desc->oa_desc, &oa_frame);
	if (ret)
		return ret;

	memcpy(&oa_frame->data[frame_offset], &eth_buff->mac_dest[0],
	       ADIN1140_ETH_HDR_LEN);
	frame_offset += ADIN1140_ETH_HDR_LEN;

	memcpy(&oa_frame->data[frame_offset], eth_buff->payload,
	       eth_buff->len - ADIN1140_ETH_HDR_LEN);

	if (eth_buff->len < 64)
		oa_frame->len = 64;
	else
		oa_frame->len = eth_buff->len;

	oa_frame->vs = 0;

	oa_tc6_put_tx_frame(desc->oa_desc, oa_frame);

	return oa_tc6_thread(desc->oa_desc);
}

/**
 * @brief Read a frame from the RX FIFO via OA TC6.
 * @param desc - the device descriptor.
 * @param eth_buff - buffer to receive the frame.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_read_fifo(struct adin1140_desc *desc,
		       struct adin1140_eth_buff *eth_buff)
{
	struct oa_tc6_frame_buffer *frame;
	uint32_t field_offset = 0;
	int ret;

	ret = oa_tc6_get_rx_frame(desc->oa_desc, &frame);
	if (ret)
		return ret;

	memcpy(&eth_buff->mac_dest[0], &frame->data[field_offset],
	       ADIN1140_ETH_HDR_LEN);
	field_offset += ADIN1140_ETH_HDR_LEN;

	memcpy(eth_buff->payload, &frame->data[field_offset],
	       frame->len - ADIN1140_ETH_HDR_LEN);
	eth_buff->len = frame->len;

	oa_tc6_put_rx_frame(desc->oa_desc, frame);

	return 0;
}

void adin1140_set_irq_flag(struct adin1140_desc *desc)
{
	desc->irq_pending = true;
}

int adin1140_poll(struct adin1140_desc *desc)
{
	if (!desc->irq_pending)
		return 0;

	desc->irq_pending = false;

	return oa_tc6_thread(desc->oa_desc);
}

/**
 * @brief Set the PLCA configuration.
 * @param desc - the device descriptor.
 * @param cfg - PLCA configuration to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_plca_set_cfg(struct adin1140_desc *desc,
			  const struct adin1140_plca_cfg *cfg)
{
	uint32_t val;
	int ret;

	/* CTRL1: node_id and node_cnt */
	val = no_os_field_prep(ADIN1140_PLCA_CTRL1_NCNT_MASK, cfg->node_cnt) |
	      no_os_field_prep(ADIN1140_PLCA_CTRL1_ID_MASK, cfg->node_id);
	ret = adin1140_reg_write(desc, ADIN1140_PLCA_CTRL1_REG, val);
	if (ret)
		return ret;

	/* TOTMR: transmit opportunity timer */
	val = no_os_field_prep(ADIN1140_PLCA_TOTMR_MASK, cfg->to_tmr);
	ret = adin1140_reg_write(desc, ADIN1140_PLCA_TOTMR_REG, val);
	if (ret)
		return ret;

	/* BURST: max burst count and burst timer */
	val = no_os_field_prep(ADIN1140_PLCA_BURST_MAXBC_MASK, cfg->burst_cnt) |
	      no_os_field_prep(ADIN1140_PLCA_BURST_BTMR_MASK, cfg->burst_tmr);
	ret = adin1140_reg_write(desc, ADIN1140_PLCA_BURST_REG, val);
	if (ret)
		return ret;

	/* CTRL0: enable/reset */
	val = cfg->enabled ? ADIN1140_PLCA_CTRL0_EN : 0;
	ret = adin1140_reg_write(desc, ADIN1140_PLCA_CTRL0_REG, val);
	if (ret)
		return ret;

	return 0;
}

/**
 * @brief Get the current PLCA configuration.
 * @param desc - the device descriptor.
 * @param cfg - pointer to store the PLCA configuration.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_plca_get_cfg(struct adin1140_desc *desc,
			  struct adin1140_plca_cfg *cfg)
{
	uint32_t val;
	int ret;

	ret = adin1140_reg_read(desc, ADIN1140_PLCA_CTRL0_REG, &val);
	if (ret)
		return ret;
	cfg->enabled = !!(val & ADIN1140_PLCA_CTRL0_EN);

	ret = adin1140_reg_read(desc, ADIN1140_PLCA_CTRL1_REG, &val);
	if (ret)
		return ret;
	cfg->node_cnt = no_os_field_get(ADIN1140_PLCA_CTRL1_NCNT_MASK, val);
	cfg->node_id = no_os_field_get(ADIN1140_PLCA_CTRL1_ID_MASK, val);

	ret = adin1140_reg_read(desc, ADIN1140_PLCA_TOTMR_REG, &val);
	if (ret)
		return ret;
	cfg->to_tmr = no_os_field_get(ADIN1140_PLCA_TOTMR_MASK, val);

	ret = adin1140_reg_read(desc, ADIN1140_PLCA_BURST_REG, &val);
	if (ret)
		return ret;
	cfg->burst_cnt = no_os_field_get(ADIN1140_PLCA_BURST_MAXBC_MASK, val);
	cfg->burst_tmr = no_os_field_get(ADIN1140_PLCA_BURST_BTMR_MASK, val);

	return 0;
}

/**
 * @brief Get the PLCA status (whether PLCA is active).
 * @param desc - the device descriptor.
 * @param active - pointer to store the status (true if PLCA is active).
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_plca_get_status(struct adin1140_desc *desc, bool *active)
{
	uint32_t val;
	int ret;

	ret = adin1140_reg_read(desc, ADIN1140_PLCA_STATUS_REG, &val);
	if (ret)
		return ret;

	*active = !!(val & ADIN1140_PLCA_STATUS_PST);

	return 0;
}

/**
 * @brief Get the link state.
 * @param desc - the device descriptor.
 * @param state - pointer to store the link state (1 = up, 0 = down).
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_link_state(struct adin1140_desc *desc, uint32_t *state)
{
	int ret;

	ret = adin1140_reg_read(desc, ADIN1140_STATUS1_REG, state);
	if (ret)
		return ret;

	*state = no_os_field_get(ADIN1140_STATUS1_LINK_STATE, *state);

	return 0;
}

/**
 * @brief Configure the MAC: apply user CONFIG0 settings, disable loopback,
 *        and install default filters.
 * @param desc - the device descriptor.
 * @param cfg - MAC CONFIG0 settings to apply.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adin1140_setup_mac(struct adin1140_desc *desc,
			      const struct adin1140_mac_cfg *cfg)
{
	int ret;

	ret = adin1140_mac_set_cfg(desc, cfg);
	if (ret)
		return ret;

	/* Disable MAC loopback */
	ret = adin1140_reg_write(desc, ADIN1140_MAC_P1_LOOP_REG, 0x0);
	if (ret)
		return ret;

	/* Install default filters: broadcast + unicast MAC */
	ret = adin1140_broadcast_filter(desc, true);
	if (ret)
		return ret;

	return adin1140_set_mac_addr(desc, desc->mac_address);
}

/**
 * @brief Verify the PHY identity by reading the MACPHY ID register.
 * @param desc - the device descriptor.
 * @return 0 in case of success, negative error code otherwise.
 */
static int adin1140_setup_phy(struct adin1140_desc *desc)
{
	uint32_t phy_id;
	int ret;

	ret = adin1140_reg_read(desc, ADIN1140_MACPHY_ID_REG, &phy_id);
	if (ret)
		return ret;

	if (phy_id != ADIN1140_PHY_ID)
		return -EINVAL;

	return 0;
}

/**
 * @brief Initialize the ADIN1140 device.
 * @param desc - pointer to the device descriptor to be initialized.
 * @param param - the device's initialization parameters.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_init(struct adin1140_desc **desc,
		  struct adin1140_init_param *param)
{
	struct oa_tc6_init_param oa_param;
	struct adin1140_desc *d;
	int ret;

	if (!param->mac_address)
		return -EINVAL;

	d = no_os_calloc(1, sizeof(*d));
	if (!d)
		return -ENOMEM;

	ret = no_os_spi_init(&d->comm_desc, &param->comm_param);
	if (ret)
		goto free_desc;

	memcpy(d->mac_address, param->mac_address, ADIN1140_ETH_ALEN);

	oa_param.comm_desc = d->comm_desc;
	oa_param.prote_spi = true;
	ret = oa_tc6_init(&d->oa_desc, &oa_param);
	if (ret)
		goto free_spi;

	ret = adin1140_setup_mac(d, &param->mac_cfg);
	if (ret)
		goto free_oa;

	ret = adin1140_setup_phy(d);
	if (ret)
		goto free_oa;

	ret = adin1140_plca_set_cfg(d, &param->plca_cfg);
	if (ret)
		goto free_oa;

	ret = adin1140_reg_update(d, ADIN1140_CONFIG0_REG,
				  ADIN1140_CONFIG0_SYNC,
				  ADIN1140_CONFIG0_SYNC);
	if (ret)
		goto free_oa;

	adin1140_set_promisc(d, true);

	*desc = d;

	return 0;

free_oa:
	oa_tc6_remove(d->oa_desc);
free_spi:
	no_os_spi_remove(d->comm_desc);
free_desc:
	no_os_free(d);

	return ret;
}

/**
 * @brief Free the ADIN1140 device descriptor.
 * @param desc - the device descriptor to be removed.
 * @return 0 in case of success, negative error code otherwise.
 */
int adin1140_remove(struct adin1140_desc *desc)
{
	if (!desc)
		return -EINVAL;

	oa_tc6_remove(desc->oa_desc);
	no_os_spi_remove(desc->comm_desc);
	no_os_free(desc);

	return 0;
}
