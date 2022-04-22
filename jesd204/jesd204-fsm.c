/**
 * The JESD204 framework - finite state machine logic
 *
 * Copyright (c) 2019 Analog Devices Inc.
 */

#include "no_os_error.h"
#include "jesd204-priv.h"

/* no-OS specific */
int jesd204_fsm_start(struct jesd204_topology *topology, unsigned int link_idx)
{
	struct jesd204_dev_top *jdev_top = topology->dev_top;
	enum jesd204_state_op_reason reason = JESD204_STATE_OP_REASON_INIT;
	enum jesd204_dev_op op;
	int lnk_id;
	int lnk_dev;
	int dev;

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		for (lnk_id = 0; lnk_id < jdev_top->num_links; lnk_id++) {
			for (dev = 0; dev < topology->devs_number; dev++) {
				for (lnk_dev = 0; lnk_dev < topology->devs[dev].links_number; lnk_dev++) {
					if (topology->devs[dev].link_ids[lnk_dev] == jdev_top->link_ids[lnk_id]) {
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_device)
							topology->devs[dev].jdev->dev_data->state_ops[op].per_device(topology->devs[dev].jdev, reason);
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_link)
							topology->devs[dev].jdev->dev_data->state_ops[op].per_link(topology->devs[dev].jdev, reason,
									&jdev_top->active_links[lnk_id].link);
					}
				}
			}
			if (jdev_top->jdev->dev_data->state_ops[op].per_device)
				jdev_top->jdev->dev_data->state_ops[op].per_device(jdev_top->jdev, reason);
			if (jdev_top->jdev->dev_data->state_ops[op].per_link)
				jdev_top->jdev->dev_data->state_ops[op].per_link(jdev_top->jdev, reason,
						&jdev_top->active_links[lnk_id].link);
			if (jdev_top->jdev->dev_data->state_ops[op].post_state_sysref)
				jesd204_sysref_async(jdev_top->jdev);
		};
	}

	return 0;
}
