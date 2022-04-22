/**
 * The JESD204 framework - finite state machine logic
 *
 * Copyright (c) 2019 Analog Devices Inc.
 */

#include "no_os_error.h"
#include "jesd204-priv.h"

/* no-OS specific */
int jesd204_fsm_start(struct jesd204_dev *jdev, unsigned int link_idx)
{
	struct jesd204_dev_top *jdev_top = jesd204_dev_get_topology_top_dev(jdev);
	enum jesd204_state_op_reason reason = JESD204_STATE_OP_REASON_INIT;
	enum jesd204_dev_op op;
	int lnk_id;

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		for (lnk_id = 0; lnk_id < jdev_top->num_links; lnk_id++) {
			if (jdev->dev_data->state_ops[op].per_device)
				jdev->dev_data->state_ops[op].per_device(jdev, reason);
			if (jdev->dev_data->state_ops[op].per_link)
				jdev->dev_data->state_ops[op].per_link(jdev, reason,
						&jdev_top->active_links[lnk_id].link);
		};
	}

	return 0;
}
