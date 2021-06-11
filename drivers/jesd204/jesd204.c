#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "jesd204.h"

static struct jesd204_link ** _links = NULL;
static uint16_t _links_count = 0;
static struct jesd204_dev ** _devices = NULL;
static uint16_t _devices_count = 0;

int jesd204_link(const char *name, struct jesd204_link **link, struct jesd204_link_param param)
{
	struct jesd204_link ** links;
	struct jesd204_link * lnk;
	uint16_t new_links_count;

	// TODO: Test each link init param for correct range/format.
	// Potentially return -EINVAL here.

	lnk = malloc(sizeof(struct jesd204_link));
	if (!lnk)
		return -ENOMEM;

	lnk->name = name;
	lnk->link_id = _links_count;
	lnk->param = param;

	new_links_count = _links_count + 1;
	links = realloc(_links, new_links_count * sizeof(struct jesd204_link *));
	if (!links)
		goto error1;

	_links = links;
	_links[_links_count] = lnk;
	_links_count = new_links_count;

	*link = lnk;
	return 0;
error1:
	free(lnk);
	return -ENOMEM;
}

static struct jesd204_dev * _jesd204_top_device(struct jesd204_dev *dev)
{
	struct jesd204_dev *devi = dev;

	if (!dev)
		return NULL;

	while(devi->outputs_count)
		devi = devi->outputs[0];
	
	return devi;
}

struct jesd204_dev * jesd204_device(const char *name, struct jesd204_dev *output,
		     struct jesd204_link *link, struct jesd204_dev_info *info)
{
	struct jesd204_dev *dev;
	struct jesd204_dev **devices;
	uint32_t new_devices_count;
	
	if (!info)
		return NULL;

	dev = calloc(1, sizeof(struct jesd204_dev));
	if (!dev)
		return NULL;
	
	new_devices_count = _devices_count + 1;
	devices = realloc(_devices, new_devices_count * sizeof(struct jesd204_dev*));
	if (!devices)
		goto error1;

	dev->outputs = calloc(1, sizeof(struct jesd204_dev *));
	if (!dev->outputs)
		goto error2;

	if (link) {
		dev->links = calloc(1, sizeof(struct jesd204_link *));
		if (!dev->links)
			goto error3;
		dev->links[0] = link;
		dev->links_count = 1;
	}

	dev->name = name;
	dev->info = info;
	dev->id = _devices_count;
	dev->outputs[0] = output;

	if (output) {
		dev->outputs_count = 1;
		uint32_t new_inputs_count = output->inputs_count + 1;
		struct jesd204_dev **inputs = realloc(output->inputs,
			new_inputs_count * sizeof(struct jesd204_dev *));
		if (!inputs)
			goto error4;
		output->inputs = inputs;
		output->inputs[output->inputs_count] = dev;
		output->inputs_count = new_inputs_count;
	}

	_devices = devices;
	_devices[_devices_count] = dev;
	_devices_count = new_devices_count;

	return dev;
error4:
	free(dev->links);
error3:
	free(dev->outputs);
error2:
	free(devices);
error1:
	free(dev);
	return NULL;
}

int jesd204_connect(struct jesd204_dev *out, struct jesd204_dev *in, struct jesd204_link *link)
{
	int ret;
	uint32_t new_outputs_count, new_inputs_count, new_links_count;

	if (!out || !in)
		return -EINVAL;
	
	new_outputs_count = out->outputs_count + 1;
	struct jesd204_dev **outputs = realloc(out->outputs,
		new_outputs_count * sizeof(struct jesd204_dev *));
	if (!outputs)
		return -ENOMEM;

	new_inputs_count = in->inputs_count + 1;
	struct jesd204_dev **inputs = realloc(in->inputs,
		new_inputs_count * sizeof(struct jesd204_dev *));
	if (!inputs) {
		ret = -ENOMEM;
		goto error1;
	}

	if (link) {
		new_links_count = out->links_count + 1;
		struct jesd204_link **links = realloc(out->links,
			new_links_count * sizeof(struct jesd204_link *));
		if (!links) {
			ret = -ENOMEM;
			goto error2;
		}
		out->links = links;
		out->links[out->links_count] = link;
		out->links_count = new_links_count;
	}

	out->outputs = outputs;
	out->outputs[out->outputs_count] = in;
	out->outputs_count = new_outputs_count;

	in->inputs = inputs;
	in->inputs[in->inputs_count] = out;
	in->inputs_count = new_inputs_count;

	return 0;
error2:
	free(inputs);
error1:
	free(outputs);
	return ret;
}

static inline int _jesd204_for_all(struct jesd204_dev *dev, jesd204_iter_cb callback, void *arg)
{
	int ret;
	uint16_t i;

	if (!dev || !callback)
		return 0;

	for (i = 0; i < _devices_count; i++) {
		// printf("dev %d\n", i);
		ret = callback(_devices[i], arg);
		if (ret < 0)
			break;
	}

	return ret;
}

static int _delete_dev(struct jesd204_dev *dev, void *arg)
{
	if (dev->inputs)
		free(dev->inputs);
	if (dev->outputs)
		free(dev->outputs);
	if (dev->links)
		free(dev->links);
	free(dev);
	return 0;
}

void jesd204_delete(struct jesd204_dev *dev)
{
	struct jesd204_dev *top = _jesd204_top_device(dev);
	_jesd204_for_all(top, _delete_dev, NULL);
	free(_links);
}

static bool _is_in_link(struct jesd204_dev *dev, struct jesd204_link *link)
{
	uint16_t l;

	if (!dev || !link)
		return false;

	for (l = 0; l < dev->links_count; l++)
		if (dev->links[l] == link)
			return true;

	return false;
}

static int _xinit(struct jesd204_dev *jdev, void *arg)
{
	struct jesd204_init_arg *iarg = (struct jesd204_init_arg *)arg;
	enum jesd204_dev_op op = iarg->op;
	enum jesd204_state_op_reason reason = iarg->reason;

	switch(iarg->data->state_ops[op].mode) {
	case JESD204_STATE_OP_MODE_PER_DEVICE:
		if (!iarg->data->state_ops[op].per_device)
			return JESD204_STATE_CHANGE_DONE;
		
		if (iarg->per_device_ran[jdev->id])
			return JESD204_STATE_CHANGE_DONE;

		iarg->per_device_ran[jdev->id] = true;
		return iarg->data->state_ops[op].per_device(jdev, reason);
	case JESD204_STATE_OP_MODE_PER_LINK:
		if (!iarg->data->state_ops[op].per_link)
			return JESD204_STATE_CHANGE_DONE;
		if(!_is_in_link(jdev, iarg->link))
			return JESD204_STATE_CHANGE_DONE;

		return iarg->data->state_ops[op].per_link(jdev, reason, iarg->link);
	default:
		return -EINVAL;
	};
}

int jesd204_init(struct jesd204_dev *jdev, const struct jesd204_dev_data *init_data) {
	int ret;
	uint16_t l;
	enum jesd204_dev_op op;
	struct jesd204_init_arg arg;

	if (!init_data)
		return -EINVAL;

	arg.per_device_ran = calloc(_devices_count, sizeof(bool));
	if (!arg.per_device_ran)
		return -ENOMEM;

	arg.data = init_data;
	arg.reason = JESD204_STATE_OP_REASON_INIT;

	// TODO: add retry mechanism based on init_data->num_retries

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		printf("op %d\n", op);
		for(l = 0; l < _links_count; l++) {
			//printf("link %d\n", l);
			arg.op = op;
			arg.link = _links[l];
			ret = _jesd204_for_all(jdev, _xinit, (void *)&arg);
			if (ret < 0)
				goto uninit;
		}
		// reset the ran flags after each op.
		memset(arg.per_device_ran, 0, _devices_count);
	}

	free(arg.per_device_ran);
	return ret;
uninit:
	arg.reason = JESD204_STATE_OP_REASON_UNINIT;
	for (; op > 0; op--) {
		for(l = 0; l < _links_count; l++) {
			arg.op = op;
			arg.link = _links[l];
			// Rolling back doesn't stop even when any of the states errors out;
			// it is of higher priority to reach back to IDLE state, than to stop when rolling back.
			// Therefore, ignore the error code.
			_jesd204_for_all(jdev, _xinit, (void *)&arg);
		}

		// reset the ran flags after each op.
		memset(arg.per_device_ran, 0, _devices_count);
	}

	free(arg.per_device_ran);
	return ret;
}