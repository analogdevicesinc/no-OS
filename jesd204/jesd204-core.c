/**
 * The JESD204 framework - core logic
 *
 * Copyright (c) 2019 Analog Devices Inc.
 */

#include <stdlib.h>
#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "jesd204-priv.h"

/* no-OS specific */
struct jesd204_dev_top dev_top;

/* no-OS specific */
static int jesd204_dev_alloc_links(struct jesd204_dev_top *jdev_top)
{
	struct jesd204_link_opaque *links;
	size_t mem_size;
	unsigned int i;

	mem_size = jdev_top->num_links * sizeof(*links);

	links = calloc(1, mem_size);
	if (!links)
		return -ENOMEM;
	jdev_top->active_links = links;

	for (i = 0; i < jdev_top->num_links; i++) {
		links[i].jdev_top = jdev_top;
		links[i].link_idx = i;
		links[i].link.link_id = jdev_top->link_ids[i];
	}

	return 0;
}

/* no-OS specific */
int jesd204_dev_register(struct jesd204_dev **jdev,
			 const struct jesd204_dev_data *i)
{
	dev_top.num_links = 2;
	dev_top.link_ids[0] = 0;
	dev_top.link_ids[1] = 2;

	jesd204_dev_alloc_links(&dev_top);

	dev_top.jdev.dev_data = i;
	dev_top.jdev.is_top = true;

	*jdev = &dev_top.jdev;

	return 0;
}

/* no-OS specific */
struct jesd204_dev_top *jesd204_dev_get_topology_top_dev(struct jesd204_dev *jdev)
{
	return jesd204_dev_top_dev(jdev);
}

static int jesd204_link_validate_params(const struct jesd204_link *lnk)
{
	if (!lnk->num_lanes) {
		pr_err("link[%u], number of lanes is zero\n", lnk->link_id);
		return -EINVAL;
	}

	if (!lnk->num_converters) {
		pr_err("link[%u], number of converters is zero\n", lnk->link_id);
		return -EINVAL;
	}

	if (!lnk->bits_per_sample) {
		pr_err("link[%u], bits-per-sample is zero\n", lnk->link_id);
		return -EINVAL;
	}

	if (!lnk->sample_rate) {
		pr_err("link[%u], sample rate is zero\n", lnk->link_id);
		return -EINVAL;
	}

	return 0;
}

int jesd204_link_get_rate(struct jesd204_link *lnk, uint64_t *lane_rate_hz)
{
	uint64_t rate, encoding_n, encoding_d;
	uint32_t sample_rate_div;
	int ret;

	ret = jesd204_link_validate_params(lnk);
	if (ret)
		return ret;

	switch (lnk->jesd_version) {
	case JESD204_VERSION_C:
		switch (lnk->jesd_encoder) {
		case JESD204_ENCODER_64B66B:
			encoding_n = 66; /* JESD 204C */
			encoding_d = 64;
			break;
		case JESD204_ENCODER_8B10B:
			encoding_n = 10; /* JESD 204C */
			encoding_d = 8;
			break;
		case JESD204_ENCODER_64B80B:
			encoding_n = 80; /* JESD 204C */
			encoding_d = 64;
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		encoding_n = 10; /* JESD 204AB */
		encoding_d = 8;
		break;
	}

	sample_rate_div = lnk->sample_rate_div ? lnk->sample_rate_div : 1;

	rate = lnk->num_converters * lnk->bits_per_sample *
		encoding_n * lnk->sample_rate;
	no_os_do_div(&rate, lnk->num_lanes * encoding_d * sample_rate_div);

	*lane_rate_hz = rate;

	return 0;
}

int jesd204_link_get_rate_khz(struct jesd204_link *lnk,
			      unsigned long *lane_rate_khz)
{
	uint64_t lane_rate_hz;
	int ret;

	ret = jesd204_link_get_rate(lnk, &lane_rate_hz);
	if (ret)
		return ret;

	*lane_rate_khz = NO_OS_DIV_ROUND_CLOSEST_ULL(lane_rate_hz, 1000);

	return ret;
}

void jesd204_copy_link_params(struct jesd204_link *dst,
			      const struct jesd204_link *src)
{
	dst->is_transmit = src->is_transmit;
	dst->num_lanes = src->num_lanes;
	dst->num_converters = src->num_converters;
	dst->octets_per_frame = src->octets_per_frame;
	dst->frames_per_multiframe = src->frames_per_multiframe;
	dst->num_of_multiblocks_in_emb = src->num_of_multiblocks_in_emb;
	dst->bits_per_sample = src->bits_per_sample;
	dst->converter_resolution = src->converter_resolution;
	dst->jesd_version = src->jesd_version;
	dst->jesd_encoder = src->jesd_encoder;
	dst->subclass = src->subclass;
	dst->device_id = src->device_id;
	dst->bank_id = src->bank_id;
	dst->scrambling = src->scrambling;
	dst->high_density = src->high_density;
	dst->ctrl_words_per_frame_clk = src->ctrl_words_per_frame_clk;
	dst->ctrl_bits_per_sample = src->ctrl_bits_per_sample;
	dst->samples_per_conv_frame = src->samples_per_conv_frame;
	dst->dac_adj_resolution_steps = src->dac_adj_resolution_steps;
	dst->dac_adj_direction = src->dac_adj_direction;
	dst->dac_phase_adj = src->dac_phase_adj;
	dst->sysref.mode = dst->sysref.mode;
	dst->sysref.capture_falling_edge = dst->sysref.capture_falling_edge;
	dst->sysref.valid_falling_edge = dst->sysref.valid_falling_edge;
	dst->sysref.lmfc_offset = dst->sysref.lmfc_offset;
}

int jesd204_sysref_async(struct jesd204_dev *jdev)
{
	struct jesd204_dev_top *jdev_top = jesd204_dev_get_topology_top_dev(jdev);
	const struct jesd204_dev_data *dev_data;

	if (!jdev_top)
		return -EFAULT;

	/* No SYSREF registered for this topology */
	if (!jdev_top->jdev_sysref)
		return 0;

	if (!jdev_top->jdev_sysref->dev_data)
		return -EFAULT;

	dev_data = jdev_top->jdev_sysref->dev_data;

	/* By now, this should have been validated to have sysref_cb() */
	return dev_data->sysref_cb(jdev_top->jdev_sysref);
}

int jesd204_sysref_async_force(struct jesd204_dev *jdev)
{
	struct jesd204_dev_top *jdev_top = jesd204_dev_get_topology_top_dev(jdev);
	const struct jesd204_dev_data *dev_data;

	if (!jdev_top)
		return -EFAULT;

	/* Primary SYSREF registered for this topology? */
	if (jdev_top->jdev_sysref)
		return jesd204_sysref_async(jdev);

	/* No SYSREF registered for this topology */
	if (!jdev_top->jdev_sysref_sec)
		return 0;

	if (!jdev_top->jdev_sysref_sec->dev_data)
		return -EFAULT;

	dev_data = jdev_top->jdev_sysref_sec->dev_data;

	/* By now, this should have been validated to have sysref_cb() */
	return dev_data->sysref_cb(jdev_top->jdev_sysref_sec);
}

bool jesd204_dev_is_top(struct jesd204_dev *jdev)
{
	return jdev && jdev->is_top;
}

void *jesd204_dev_priv(struct jesd204_dev *jdev)
{
	return jdev->priv;
}
