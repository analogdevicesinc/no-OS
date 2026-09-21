/**
 * The JESD204 framework - finite state machine logic
 *
 * Copyright (c) 2022 Analog Devices Inc.
 */

#include "no_os_error.h"
#include "no_os_print_log.h"
#include "no_os_util.h"
#include "jesd204-priv.h"

/* Index-matched to enum jesd204_dev_op. */
static const char * const jesd204_dev_op_str[] = {
	"DEVICE_INIT",
	"LINK_INIT",
	"LINK_SUPPORTED",
	"LINK_PRE_SETUP",
	"CLK_SYNC_STAGE1",
	"CLK_SYNC_STAGE2",
	"CLK_SYNC_STAGE3",
	"CLK_SYNC_STAGE4",
	"LINK_SETUP",
	"OPT_SETUP_STAGE1",
	"OPT_SETUP_STAGE2",
	"OPT_SETUP_STAGE3",
	"OPT_SETUP_STAGE4",
	"OPT_SETUP_STAGE5",
	"CLOCKS_ENABLE",
	"LINK_ENABLE",
	"LINK_RUNNING",
	"OPT_POST_SETUP_STAGE1",
	"OPT_POST_SETUP_STAGE2",
	"OPT_POST_SETUP_STAGE3",
	"OPT_POST_RUNNING_STAGE",
};

_Static_assert(NO_OS_ARRAY_SIZE(jesd204_dev_op_str) == __JESD204_MAX_OPS,
	       "jesd204_dev_op_str is out of sync with enum jesd204_dev_op");

static const char *jesd204_op_str(int op)
{
	if (op < 0 || op >= __JESD204_MAX_OPS)
		return "?";

	return jesd204_dev_op_str[op];
}

/*
 * Callbacks return enum jesd204_state_change_result (DONE = 1, DEFER = 0,
 * ERROR = -1) or a negative errno, so only a negative value is a failure.
 *
 * A failure is reported but does not abort the transition: this FSM runs every
 * op unconditionally and both entry points return 0 regardless, unlike the
 * Linux framework they mirror. Until that changes, these messages are the only
 * evidence that a state did not actually complete.
 *
 * DEFER means "not finished, call me again", which this FSM never does - it has
 * no retry loop - so a deferral is a silent stall and is warned about.
 */
static void jesd204_fsm_report(int op, const char *scope, int dev,
			       int link_id, int ret)
{
	if (ret < 0) {
		pr_err("jesd204 fsm: %s %s dev%d link%d failed (%d)\n",
		       jesd204_op_str(op), scope, dev, link_id, ret);
		return;
	}

	if (ret == JESD204_STATE_CHANGE_DEFER) {
		pr_warning("jesd204 fsm: %s %s dev%d link%d deferred, not retried\n",
			   jesd204_op_str(op), scope, dev, link_id);
		return;
	}

#ifdef JESD204_FSM_DEBUG
	pr_info("jesd204 fsm:   %s dev%d link%d done\n", scope, dev, link_id);
#endif
}

static void jesd204_fsm_report_state(int op,
				     enum jesd204_state_op_reason reason)
{
#ifdef JESD204_FSM_DEBUG
	pr_info("jesd204 fsm: [%02d] %s (%s)\n", op, jesd204_op_str(op),
		jesd204_state_op_reason_str(reason));
#else
	(void)op;
	(void)reason;
#endif
}

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
	int ret;

	for (op = 0; op < __JESD204_MAX_OPS; op++) {
		jesd204_fsm_report_state(op, reason);

		for (dev = 0; dev < topology->devs_number; dev++)
			per_device_op_done[dev] = false;

		for (lnk_id = 0; lnk_id < jdev_top->num_links; lnk_id++) {
			for (dev = 0; dev < topology->devs_number; dev++) {
				for (lnk_dev = 0; lnk_dev < topology->devs[dev].links_number; lnk_dev++) {
					if (topology->devs[dev].link_ids[lnk_dev] == jdev_top->link_ids[lnk_id]) {
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_device
						    && !per_device_op_done[dev]) {
							ret = topology->devs[dev].jdev->dev_data->state_ops[op].per_device(
								      topology->devs[dev].jdev, reason);
							jesd204_fsm_report(op, "per_device", dev,
									   jdev_top->link_ids[lnk_id], ret);
							per_device_op_done[dev] = true;
						}
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_link) {
							ret = topology->devs[dev].jdev->dev_data->state_ops[op].per_link(
								      topology->devs[dev].jdev, reason,
								      &jdev_top->active_links[lnk_id].link);
							jesd204_fsm_report(op, "per_link", dev,
									   jdev_top->link_ids[lnk_id], ret);
						}
					}
				}
			}
			if (jdev_top->jdev->dev_data->state_ops[op].per_link) {
				ret = jdev_top->jdev->dev_data->state_ops[op].per_link(jdev_top->jdev, reason,
						&jdev_top->active_links[lnk_id].link);
				jesd204_fsm_report(op, "top per_link", -1,
						   jdev_top->link_ids[lnk_id], ret);
				if (jdev_top->jdev->dev_data->state_ops[op].post_state_sysref)
					jesd204_sysref_async(jdev_top->jdev);
			}
		}
		if (jdev_top->jdev->dev_data->state_ops[op].per_device) {
			ret = jdev_top->jdev->dev_data->state_ops[op].per_device(jdev_top->jdev, reason);
			jesd204_fsm_report(op, "top per_device", -1, -1, ret);
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
	int ret;

	for (op = __JESD204_MAX_OPS - 1; op >= 0; op--) {
		jesd204_fsm_report_state(op, reason);

		for (dev = topology->devs_number - 1; dev >= 0 ; dev--)
			per_device_op_done[dev] = false;

		if (jdev_top->jdev->dev_data->state_ops[op].per_device) {
			ret = jdev_top->jdev->dev_data->state_ops[op].per_device(jdev_top->jdev, reason);
			jesd204_fsm_report(op, "top per_device", -1, -1, ret);
		}

		for (lnk_id = jdev_top->num_links - 1; lnk_id >= 0; lnk_id--) {
			if (jdev_top->jdev->dev_data->state_ops[op].per_link) {
				ret = jdev_top->jdev->dev_data->state_ops[op].per_link(jdev_top->jdev, reason,
						&jdev_top->active_links[lnk_id].link);
				jesd204_fsm_report(op, "top per_link", -1,
						   jdev_top->link_ids[lnk_id], ret);
			}
			for (dev = topology->devs_number - 1; dev >= 0; dev--) {
				for (lnk_dev = topology->devs[dev].links_number - 1; lnk_dev >= 0; lnk_dev--) {
					if (topology->devs[dev].link_ids[lnk_dev] == jdev_top->link_ids[lnk_id]) {
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_device
						    && !per_device_op_done[dev]) {
							ret = topology->devs[dev].jdev->dev_data->state_ops[op].per_device(
								      topology->devs[dev].jdev, reason);
							jesd204_fsm_report(op, "per_device", dev,
									   jdev_top->link_ids[lnk_id], ret);
							per_device_op_done[dev] = true;
						}
						if (topology->devs[dev].jdev->dev_data->state_ops[op].per_link) {
							ret = topology->devs[dev].jdev->dev_data->state_ops[op].per_link(
								      topology->devs[dev].jdev, reason,
								      &jdev_top->active_links[lnk_id].link);
							jesd204_fsm_report(op, "per_link", dev,
									   jdev_top->link_ids[lnk_id], ret);
						}
					}
				}
			}
		}
	}

	return 0;
}
