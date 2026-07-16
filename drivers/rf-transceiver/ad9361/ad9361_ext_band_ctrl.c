/*
 * Copyright (c) 2025-2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "ad9361.h"
#include "ad9361_util.h"
#include "ad9361_ext_band_ctrl.h"
#include "no_os_gpio.h"
#include "no_os_delay.h"
#include "no_os_alloc.h"
#include "no_os_spi.h"

static struct ad9361_band_setting *
find_setting(struct ad9361_band_setting **list, uint32_t count, uint64_t freq)
{
	uint32_t i;

	for (i = 0; i < count; i++) {
		if (list[i]->freq_min <= freq && freq < list[i]->freq_max)
			return list[i];
	}
	return NULL;
}

static int apply_gpio_settings(struct ad9361_rf_phy *phy,
			       const struct ad9361_band_setting *new_sett,
			       const struct ad9361_band_setting *curr)
{
	struct ad9361_ext_band_ctl *ctl = phy->ext_band_ctl;
	uint32_t i;
	int ret;

	if (!new_sett->gpio_values || ctl->ngpios == 0)
		return 0;

	for (i = 0; i < (uint32_t)ctl->ngpios; i++) {
		uint32_t new_val = new_sett->gpio_values[i];
		uint32_t cur_val = (curr && curr->gpio_values)
				   ? curr->gpio_values[i]
				   : AD9361_EXT_BAND_CTL_OP_NOP;

		if (curr && cur_val == new_val)
			continue;

		if (new_val == AD9361_EXT_BAND_CTL_OP_NOP)
			continue;

		if (!ctl->gpios[i]) {
			dev_err(&phy->spi->dev,
				"%s: GPIO[%u] is NULL", __func__, (unsigned)i);
			return -EINVAL;
		}

		switch (new_val) {
		case AD9361_EXT_BAND_CTL_OP_LOW:
			ret = no_os_gpio_direction_output(ctl->gpios[i], 0);
			break;
		case AD9361_EXT_BAND_CTL_OP_HIGH:
			ret = no_os_gpio_direction_output(ctl->gpios[i], 1);
			break;
		case AD9361_EXT_BAND_CTL_OP_INPUT:
			ret = no_os_gpio_direction_input(ctl->gpios[i]);
			break;
		default:
			dev_err(&phy->spi->dev,
				"%s: unknown op %u on pin %u",
				__func__, (unsigned)new_val, (unsigned)i);
			return -EINVAL;
		}

		if (ret < 0) {
			dev_err(&phy->spi->dev,
				"%s: GPIO[%u] op %u failed: %d",
				__func__, (unsigned)i, (unsigned)new_val, ret);
			return ret;
		}

		dev_dbg(&phy->spi->dev,
			"%s: GPIO[%u] op=%u",
			__func__, (unsigned)i, (unsigned)new_val);
	}

	return 0;
}

static int apply_setting(struct ad9361_rf_phy *phy,
			 const struct ad9361_band_setting *new_sett,
			 const struct ad9361_band_setting **curr_ptr)
{
	const struct ad9361_band_setting *curr = curr_ptr ? *curr_ptr : NULL;
	int ret;

	dev_dbg(&phy->spi->dev, "%s: applying '%s'",
		__func__, new_sett->name ? new_sett->name : "<unnamed>");

	/* Apply pre-sequence steps */
	if (new_sett->pre_seq && new_sett->pre_seq_len > 0) {
		uint32_t i;
		for (i = 0; i < new_sett->pre_seq_len; i++) {
			ret = apply_setting(phy, &new_sett->pre_seq[i], NULL);
			if (ret < 0)
				return ret;
			if (new_sett->pre_seq[i].delay_us)
				no_os_udelay(new_sett->pre_seq[i].delay_us);
		}
	}

	/* Drive GPIOs */
	ret = apply_gpio_settings(phy, new_sett, curr);
	if (ret < 0)
		return ret;

	/* Switch RX RF port if specified */
	if (new_sett->rf_rx_input_sel != AD9361_EXT_BAND_NO_PORT_SEL) {
		if (!curr ||
		    curr->rf_rx_input_sel != new_sett->rf_rx_input_sel) {
			ret = ad9361_rf_port_setup(phy, false,
						   new_sett->rf_rx_input_sel,
						   0);
			if (ret < 0) {
				dev_err(&phy->spi->dev,
					"%s: RX port %u failed: %d",
					__func__,
					(unsigned)new_sett->rf_rx_input_sel,
					ret);
				return ret;
			}
			dev_dbg(&phy->spi->dev,
				"%s: RX port -> %u",
				__func__,
				(unsigned)new_sett->rf_rx_input_sel);
		}
	}

	/* Switch TX RF port if specified */
	if (new_sett->rf_tx_output_sel != AD9361_EXT_BAND_NO_PORT_SEL) {
		if (!curr ||
		    curr->rf_tx_output_sel != new_sett->rf_tx_output_sel) {
			ret = ad9361_rf_port_setup(phy, true, 0,
						   new_sett->rf_tx_output_sel);
			if (ret < 0) {
				dev_err(&phy->spi->dev,
					"%s: TX port %u failed: %d",
					__func__,
					(unsigned)new_sett->rf_tx_output_sel,
					ret);
				return ret;
			}
			dev_dbg(&phy->spi->dev,
				"%s: TX port -> %u",
				__func__,
				(unsigned)new_sett->rf_tx_output_sel);
		}
	}

	if (curr_ptr)
		*curr_ptr = new_sett;

	/* Apply post-sequence steps */
	if (new_sett->post_seq && new_sett->post_seq_len > 0) {
		uint32_t i;
		for (i = 0; i < new_sett->post_seq_len; i++) {
			ret = apply_setting(phy, &new_sett->post_seq[i], NULL);
			if (ret < 0)
				return ret;
			if (new_sett->post_seq[i].delay_us)
				no_os_udelay(new_sett->post_seq[i].delay_us);
		}
	}

	dev_dbg(&phy->spi->dev, "%s: applied '%s'",
		__func__, new_sett->name ? new_sett->name : "<unnamed>");

	return 0;
}

static struct ad9361_band_setting **grow_settings_array(
	struct ad9361_band_setting **old_arr,
	uint32_t old_count,
	struct ad9361_band_setting *new_elem)
{
	struct ad9361_band_setting **new_arr;
	uint32_t i;

	new_arr = (struct ad9361_band_setting **)
		  no_os_calloc(old_count + 1, sizeof(*new_arr));
	if (!new_arr)
		return NULL;

	for (i = 0; i < old_count; i++)
		new_arr[i] = old_arr[i];

	new_arr[old_count] = new_elem;

	no_os_free(old_arr);
	return new_arr;
}

/**
 * ad9361_band_setting_alloc() - Allocate a band setting descriptor.
 * @name:     Human-readable name (copied; may be NULL).
 * @freq_min: Inclusive lower bound of the frequency range (Hz).
 * @freq_max: Exclusive upper bound of the frequency range (Hz).
 * @ngpios:   Number of GPIO lines this setting may drive (0 is valid).
 *
 * Returns a pointer to the new setting, or NULL on allocation failure.
 * Release with ad9361_band_setting_free().
 */
struct ad9361_band_setting *
ad9361_band_setting_alloc(const char *name, uint64_t freq_min,
			  uint64_t freq_max, uint32_t ngpios)
{
	struct ad9361_band_setting *s;
	uint32_t i;

	s = (struct ad9361_band_setting *)
	    no_os_calloc(1, sizeof(*s));
	if (!s)
		return NULL;

	if (name) {
		char *name_copy = (char *)no_os_calloc(1, strlen(name) + 1);
		if (!name_copy) {
			no_os_free(s);
			return NULL;
		}
		strcpy(name_copy, name);
		s->name = name_copy;
	}

	s->freq_min         = freq_min;
	s->freq_max         = freq_max;
	s->rf_rx_input_sel  = AD9361_EXT_BAND_NO_PORT_SEL;
	s->rf_tx_output_sel = AD9361_EXT_BAND_NO_PORT_SEL;

	if (ngpios > 0) {
		s->gpio_values = (uint32_t *)
				 no_os_calloc(ngpios, sizeof(uint32_t));
		if (!s->gpio_values) {
			no_os_free((void *)s->name);
			no_os_free(s);
			return NULL;
		}
		for (i = 0; i < ngpios; i++)
			s->gpio_values[i] = AD9361_EXT_BAND_CTL_OP_NOP;
		s->ngpios = ngpios;
	}

	return s;
}

/**
 * ad9361_band_setting_free() - Release a band setting and its sub-sequences.
 */
void ad9361_band_setting_free(struct ad9361_band_setting *s)
{
	uint32_t i;

	if (!s)
		return;

	no_os_free((void *)s->name);
	no_os_free(s->gpio_values);

	if (s->pre_seq) {
		for (i = 0; i < s->pre_seq_len; i++)
			no_os_free(s->pre_seq[i].gpio_values);
		no_os_free(s->pre_seq);
	}

	if (s->post_seq) {
		for (i = 0; i < s->post_seq_len; i++)
			no_os_free(s->post_seq[i].gpio_values);
		no_os_free(s->post_seq);
	}

	no_os_free(s);
}

/**
 * ad9361_band_setting_set_gpio() - Set the operation for one GPIO pin.
 * @s:        The band setting.
 * @gpio_idx: Index in the controller's gpios[] array (0-based).
 * @op:       AD9361_EXT_BAND_CTL_OP_LOW/HIGH/INPUT/NOP.
 *
 * Returns 0 on success, -EINVAL if gpio_idx is out of range.
 */
int ad9361_band_setting_set_gpio(struct ad9361_band_setting *s,
				 uint32_t gpio_idx, uint32_t op)
{
	if (!s || !s->gpio_values || gpio_idx >= s->ngpios)
		return -EINVAL;
	s->gpio_values[gpio_idx] = op;
	return 0;
}

/** ad9361_band_setting_set_rx_port() - Set the RX RF port for this band. */
void ad9361_band_setting_set_rx_port(struct ad9361_band_setting *s,
				     uint32_t port)
{
	if (s)
		s->rf_rx_input_sel = port;
}

/** ad9361_band_setting_set_tx_port() - Set the TX RF port for this band. */
void ad9361_band_setting_set_tx_port(struct ad9361_band_setting *s,
				     uint32_t port)
{
	if (s)
		s->rf_tx_output_sel = port;
}

/**
 * ad9361_band_setting_set_delay() - Set the post-step delay (microseconds).
 * Used when this setting is a member of a pre_seq[] or post_seq[].
 */
void ad9361_band_setting_set_delay(struct ad9361_band_setting *s,
				   uint32_t delay_us)
{
	if (s)
		s->delay_us = delay_us;
}

/**
 * ad9361_ext_band_ctl_alloc() - Allocate an external band controller.
 *
 * Returns a pointer to the zeroed controller, or NULL on failure.
 */
struct ad9361_ext_band_ctl *ad9361_ext_band_ctl_alloc(void)
{
	return (struct ad9361_ext_band_ctl *)
	       no_os_calloc(1, sizeof(struct ad9361_ext_band_ctl));
}

/**
 * ad9361_ext_band_ctl_free() - Release a controller and all registered settings.
 * GPIO descriptors are NOT freed -- the caller owns them.
 */
void ad9361_ext_band_ctl_free(struct ad9361_ext_band_ctl *ctl)
{
	uint32_t i;

	if (!ctl)
		return;

	for (i = 0; i < ctl->rx_count; i++)
		ad9361_band_setting_free(ctl->rx_settings[i]);

	for (i = 0; i < ctl->tx_count; i++)
		ad9361_band_setting_free(ctl->tx_settings[i]);

	no_os_free(ctl->rx_settings);
	no_os_free(ctl->tx_settings);
	no_os_free(ctl);
}

/**
 * ad9361_ext_band_ctl_add_gpio() - Register a GPIO with the controller.
 * @ctl:  The controller.
 * @gpio: A configured GPIO descriptor.
 *
 * GPIOs are referenced by add order (0 = first added).
 * Returns 0 on success, -EINVAL or -ENOMEM on error.
 */
int ad9361_ext_band_ctl_add_gpio(struct ad9361_ext_band_ctl *ctl,
				 struct no_os_gpio_desc *gpio)
{
	if (!ctl || !gpio)
		return -EINVAL;

	if (ctl->ngpios >= AD9361_EXT_BAND_MAX_GPIOS)
		return -ENOMEM;

	ctl->gpios[ctl->ngpios] = gpio;
	ctl->ngpios++;
	return 0;
}

/**
 * ad9361_ext_band_ctl_add_rx_setting() - Append an RX band setting.
 * Settings are matched first-to-last; the first frequency match wins.
 */
int ad9361_ext_band_ctl_add_rx_setting(struct ad9361_ext_band_ctl *ctl,
				       struct ad9361_band_setting *s)
{
	struct ad9361_band_setting **new_arr;

	if (!ctl || !s)
		return -EINVAL;

	new_arr = grow_settings_array(ctl->rx_settings, ctl->rx_count, s);
	if (!new_arr)
		return -ENOMEM;

	ctl->rx_settings = new_arr;
	ctl->rx_count++;
	return 0;
}

/**
 * ad9361_ext_band_ctl_add_tx_setting() - Append a TX band setting.
 */
int ad9361_ext_band_ctl_add_tx_setting(struct ad9361_ext_band_ctl *ctl,
				       struct ad9361_band_setting *s)
{
	struct ad9361_band_setting **new_arr;

	if (!ctl || !s)
		return -EINVAL;

	new_arr = grow_settings_array(ctl->tx_settings, ctl->tx_count, s);
	if (!new_arr)
		return -ENOMEM;

	ctl->tx_settings = new_arr;
	ctl->tx_count++;
	return 0;
}

/**
 * ad9361_register_ext_band_control() - Attach a controller to a PHY.
 *
 * Returns 0 on success, -EINVAL if either argument is NULL.
 */
int32_t ad9361_register_ext_band_control(struct ad9361_rf_phy *phy,
		struct ad9361_ext_band_ctl *ctl)
{
	if (!phy || !ctl)
		return -EINVAL;

	phy->ext_band_ctl = ctl;

	dev_dbg(&phy->spi->dev,
		"%s: registered (%u RX / %u TX settings, %d GPIOs)",
		__func__,
		(unsigned)ctl->rx_count,
		(unsigned)ctl->tx_count,
		ctl->ngpios);

	return 0;
}

/**
 * ad9361_unregister_ext_band_control() - Detach the controller from a PHY.
 * Does not free anything.
 */
void ad9361_unregister_ext_band_control(struct ad9361_rf_phy *phy)
{
	if (phy)
		phy->ext_band_ctl = NULL;
}

/**
 * ad9361_adjust_rx_ext_band_settings() - Switch RX band for a new LO freq.
 *
 * Called automatically from ad9361_rfpll_set_rate() on every RX LO change.
 * Applications may also call this directly.
 *
 * @phy:  The AD9361 state structure.
 * @freq: New RX LO frequency in Hz.
 *
 * Returns 0 on success (including "no matching setting"), or a negative
 * error code on hardware failure.
 */
int32_t ad9361_adjust_rx_ext_band_settings(struct ad9361_rf_phy *phy,
		uint64_t freq)
{
	struct ad9361_ext_band_ctl *ctl;
	struct ad9361_band_setting *sett;

	if (!phy)
		return -EINVAL;

	if (!phy->ext_band_ctl)
		return 0;

	ctl = phy->ext_band_ctl;

	sett = find_setting(ctl->rx_settings, ctl->rx_count, freq);
	if (!sett)
		return 0;

	if (ctl->current_rx_setting == sett)
		return 0;

	return apply_setting(phy, sett,
			     (const struct ad9361_band_setting **)
			     &ctl->current_rx_setting);
}

/**
 * ad9361_adjust_tx_ext_band_settings() - Switch TX band for a new LO freq.
 *
 * @phy:  The AD9361 state structure.
 * @freq: New TX LO frequency in Hz.
 *
 * Returns 0 on success, or a negative error code on hardware failure.
 */
int32_t ad9361_adjust_tx_ext_band_settings(struct ad9361_rf_phy *phy,
		uint64_t freq)
{
	struct ad9361_ext_band_ctl *ctl;
	struct ad9361_band_setting *sett;

	if (!phy)
		return -EINVAL;

	if (!phy->ext_band_ctl)
		return 0;

	ctl = phy->ext_band_ctl;

	sett = find_setting(ctl->tx_settings, ctl->tx_count, freq);
	if (!sett)
		return 0;

	if (ctl->current_tx_setting == sett)
		return 0;

	return apply_setting(phy, sett,
			     (const struct ad9361_band_setting **)
			     &ctl->current_tx_setting);
}
