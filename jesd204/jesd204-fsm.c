/**
 * The JESD204 framework - finite state machine logic
 *
 * Copyright (c) 2022 Analog Devices Inc.
 */

#include "no_os_error.h"
#include "jesd204-priv.h"

/* no-OS specific */
int jesd204_fsm_start(struct jesd204_topology *topology, unsigned int link_idx)
{
	enum jesd204_state_op_reason reason = JESD204_STATE_OP_REASON_INIT;
	struct jesd204_dev_top *jdev_top = topology->dev_top;
	bool per_device_op_done[16];
	enum jesd204_dev_op op;
	int lnk_dev;
	int lnk_id;
	int dev;

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		for (dev = 0; dev < topology->devs_number; dev++)
			per_device_op_done[dev] = false;

		for (lnk_id = 0; lnk_id < jdev_top->num_links; lnk_id++) {
			for (dev = 0; dev < topology->devs_number; dev++) {
				for (lnk_dev = 0; lnk_dev < topology->devs[dev].links_number; lnk_dev++) {
					if (topology->devs[dev].link_ids[lnk_dev] == jdev_top->link_ids[lnk_id]) {
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_device
						    && !per_device_op_done[dev]) {
							topology->devs[dev].jdev->dev_data->state_ops[op].per_device(
								topology->devs[dev].jdev, reason);
							per_device_op_done[dev] = true;
						}
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_link)
							topology->devs[dev].jdev->dev_data->state_ops[op].per_link(
								topology->devs[dev].jdev, reason,
								&jdev_top->active_links[lnk_id].link);
					}
				}
			}
			if (jdev_top->jdev->dev_data->state_ops[op].per_link) {
				jdev_top->jdev->dev_data->state_ops[op].per_link(jdev_top->jdev, reason,
						&jdev_top->active_links[lnk_id].link);
				if (jdev_top->jdev->dev_data->state_ops[op].post_state_sysref)
					jesd204_sysref_async(jdev_top->jdev);
			}
		}
		if (jdev_top->jdev->dev_data->state_ops[op].per_device) {
			jdev_top->jdev->dev_data->state_ops[op].per_device(jdev_top->jdev, reason);
			if (jdev_top->jdev->dev_data->state_ops[op].post_state_sysref)
				jesd204_sysref_async(jdev_top->jdev);
		}
	}

	return 0;
}

/* no-OS specific */
int jesd204_fsm_stop(struct jesd204_topology *topology, unsigned int link_idx)
{
	enum jesd204_state_op_reason reason = JESD204_STATE_OP_REASON_UNINIT;
	struct jesd204_dev_top *jdev_top = topology->dev_top;
	bool per_device_op_done[16];
	int lnk_dev;
	int lnk_id;
	int dev;
	int op;

	for (op = __JESD204_MAX_OPS - 1; op >= 0; op--) {
		for (dev = topology->devs_number - 1; dev >= 0 ; dev--)
			per_device_op_done[dev] = false;

		if (jdev_top->jdev->dev_data->state_ops[op].per_device)
			jdev_top->jdev->dev_data->state_ops[op].per_device(jdev_top->jdev, reason);

		for (lnk_id = jdev_top->num_links - 1; lnk_id >= 0; lnk_id--) {
			if (jdev_top->jdev->dev_data->state_ops[op].per_link)
				jdev_top->jdev->dev_data->state_ops[op].per_link(jdev_top->jdev, reason,
						&jdev_top->active_links[lnk_id].link);
			for (dev = topology->devs_number - 1; dev >= 0; dev--) {
				for (lnk_dev = topology->devs[dev].links_number - 1; lnk_dev >= 0; lnk_dev--) {
					if (topology->devs[dev].link_ids[lnk_dev] == jdev_top->link_ids[lnk_id]) {
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_device
						    && !per_device_op_done[dev]) {
							topology->devs[dev].jdev->dev_data->state_ops[op].per_device(
								topology->devs[dev].jdev, reason);
							per_device_op_done[dev] = true;
						}
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_link)
							topology->devs[dev].jdev->dev_data->state_ops[op].per_link(
								topology->devs[dev].jdev, reason,
								&jdev_top->active_links[lnk_id].link);
					}
				}
			}
		}
	}

	return 0;
}
