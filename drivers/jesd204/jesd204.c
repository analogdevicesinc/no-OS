#include <stdlib.h>
#include "util.h"
#include "error.h"
#include "print_log.h"
#include "jesd204.h"
#include "jesd204-priv.h"

void *jesd204_dev_priv(struct jesd204_dev *jdev)
{
	return jdev->priv;
}

void jesd204_set_dev_priv(struct jesd204_dev *jdev, void *priv)
{
	if (jdev)
		jdev->priv = priv;
}

char *jesd204_dev_name(struct jesd204_dev *jdev)
{
	if (jdev)
		return jdev->name;

	return NULL;
}

bool jesd204_dev_is_top(struct jesd204_dev *jdev)
{
	if (jdev)
		return jdev->is_top;

	return false;
}

int jesd204_dev_register(struct jesd204_dev **jdev, char *name, const struct jesd204_dev_data *data)
{
	struct jesd204_dev *pjdev;
	static int id = 0;

	if (jdev == NULL)
		return -EINVAL;

	pjdev = calloc(1, sizeof(struct jesd204_dev));
	if (!pjdev)
		return -ENOMEM;

	pjdev->dev_data = data;
	pjdev->id = id++;
	pjdev->name = name;

	*jdev = pjdev;
	
	return 0;
}

int jesd204_link_register(struct jesd204_dev *topjdev, struct jesd204_link_dev *link_dev)
{
	unsigned int d;

	if (!topjdev || !link_dev)
		return -EINVAL;

	struct jesd204_link_dev **links = realloc(topjdev->links, sizeof(*topjdev->links) * (topjdev->num_links + 1));
	if (!links)
		return -ENOMEM;

	topjdev->links = links;
	topjdev->links[topjdev->num_links] = link_dev;
	topjdev->num_links += 1;

	if (link_dev->top_device)
		link_dev->top_device->is_top = true;

	for (d = 0; d < link_dev->num_devices; d++) {
		struct jesd204_dev *jdev = link_dev->devices[d];
		struct jesd204_link_dev **dlinks = realloc(topjdev->links, sizeof(*topjdev->links) * (topjdev->num_links + 1));
		if (!dlinks)
			return -ENOMEM;

		jdev->links = dlinks;
		jdev->links[jdev->num_links] = link_dev;
		jdev->num_links += 1;
	}
	
	return 0;
}


static int _xinit(struct jesd204_dev *jdev, unsigned int linkidx, enum jesd204_dev_op op, enum jesd204_state_op_reason reason)
{
	switch(jdev->dev_data->state_ops[op].mode) {
	case JESD204_STATE_OP_MODE_PER_DEVICE:
		if (!jdev->dev_data->state_ops[op].per_device)
			return JESD204_STATE_CHANGE_DONE;

		if (jdev->per_device_ran)
			return JESD204_STATE_CHANGE_DONE;

		jdev->per_device_ran = true;
		return jdev->dev_data->state_ops[op].per_device(jdev, reason);
	case JESD204_STATE_OP_MODE_PER_LINK:
		if (!jdev->dev_data->state_ops[op].per_link)
			return JESD204_STATE_CHANGE_DONE;

		return jdev->dev_data->state_ops[op].per_link(jdev, reason, jdev->links[linkidx]->link);
	default:
		return -EINVAL;
	};
}

int jesd204_run(struct jesd204_dev *jdev) {
	int ret;
	uint16_t l, lt, j, jt;
	enum jesd204_dev_op op;

	if (!jdev)
		return -EINVAL;

	// TODO: add retry mechanism based on init_data->num_retries

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		printf("op %d\n", op);

		// clear per_device_ran on each operation
		for (lt = 0; lt < jdev->num_links; lt++)
			for (jt = 0; jt < jdev->links[lt]->num_devices; jt++)
				jdev->links[lt]->devices[jt]->per_device_ran = false;

		// for each device in each link, execute the callback
		for (l = 0; l < jdev->num_links; l++) {
			printf("link %d\n", l);
			
			// do the top device first
			ret = _xinit(jdev, l, op, JESD204_STATE_OP_REASON_INIT);
			if (ret < 0)
				goto uninit;
			
			// do the rest
			for (j = 0; j < jdev->links[l]->num_devices; j++) {
				ret = _xinit(jdev->links[l]->devices[j], l, op, JESD204_STATE_OP_REASON_INIT);
				if (ret < 0)
					goto uninit;
			}
		}
	}

	return ret;
uninit:
	// TODO: fill
	return ret;
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
	do_div(&rate, lnk->num_lanes * encoding_d * sample_rate_div);

	*lane_rate_hz = rate;

	return 0;
}

int jesd204_link_get_lmfc_lemc_rate(struct jesd204_link *lnk,
				    unsigned long *rate_hz)
{
	uint64_t lane_rate_hz;
	uint32_t bkw;
	int ret;

	ret = jesd204_link_get_rate(lnk, &lane_rate_hz);
	if (ret)
		return ret;

	switch (lnk->jesd_version) {
	case JESD204_VERSION_C:
		switch (lnk->jesd_encoder) {
		case JESD204_ENCODER_64B66B:
			bkw = 66; /* JESD 204C */
			/* fall-through */
		case JESD204_ENCODER_64B80B:
			if (lnk->jesd_encoder == JESD204_ENCODER_64B80B)
				bkw = 80; /* JESD 204C */

			if (lnk->num_of_multiblocks_in_emb) {
				do_div(&lane_rate_hz, bkw * 32 *
					lnk->num_of_multiblocks_in_emb);
			} else {
				lane_rate_hz *= 8;
				do_div(&lane_rate_hz, bkw *
					lnk->octets_per_frame *
					lnk->frames_per_multiframe);
			}
			break;
		case JESD204_ENCODER_8B10B:
			do_div(&lane_rate_hz, 10 * lnk->octets_per_frame *
				lnk->frames_per_multiframe);
			break;
		default:
			return -EINVAL;
		}
		break;
	default:
		do_div(&lane_rate_hz, 10 * lnk->octets_per_frame *
			lnk->frames_per_multiframe);
		break;
	}

	*rate_hz = lane_rate_hz;

	return 0;
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
}

int jesd204_link_get_rate_khz(struct jesd204_link *lnk,
			      unsigned long *lane_rate_khz)
{
	uint64_t lane_rate_hz;
	int ret;

	ret = jesd204_link_get_rate(lnk, &lane_rate_hz);
	if (ret)
		return ret;

	*lane_rate_khz = DIV_ROUND_CLOSEST_ULL(lane_rate_hz, 1000);

	return ret;
}
