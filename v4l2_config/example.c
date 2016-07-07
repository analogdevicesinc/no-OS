
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include "v4l2_config.h"

/******************************************************************************/
/********************** Macros and Constants Definitions **********************/
/******************************************************************************/
#define ADV7611_SUBDEV	"/dev/v4l-subdev0"
#define ADV7511_SUBDEV	"/dev/v4l-subdev1"

/***************************************************************************//**
* @brief main
*******************************************************************************/
int main(void)
{
	int adv7611_subdev_fd;
	int adv7511_subdev_fd;
	struct v4l2_dv_timings dv_timings;
	struct v4l2_queryctrl ctrls[20];
	struct v4l2_event_new event;
	int ctrls_no;
	int index;
	int ret;

	/* Open subdevices */
	adv7611_subdev_fd = open(ADV7611_SUBDEV, O_RDWR | O_NONBLOCK, 0);
	if (adv7611_subdev_fd == -1)
		return -1;

	adv7511_subdev_fd = open(ADV7511_SUBDEV, O_RDWR | O_NONBLOCK, 0);
	if (adv7511_subdev_fd == -1)
		goto err1;

	/* Get the detected timings from ADV7611 */
	ret = v4l2_cfg_query_dv_timings(adv7611_subdev_fd, &dv_timings);
	if (ret)
		goto err2;

	/* Set the detected timings to both devices: ADV7611 and ADV7511 */
	ret = v4l2_cfg_set_dv_timings(adv7611_subdev_fd, &dv_timings);
	if (ret)
		goto err2;
	ret = v4l2_cfg_set_dv_timings(adv7511_subdev_fd, &dv_timings);
	if (ret)
		goto err2;

	/* Get the configured timings */
	ret = v4l2_cfg_get_dv_timings(adv7611_subdev_fd, &dv_timings);
	if (ret)
		goto err2;
	ret = v4l2_cfg_get_dv_timings(adv7511_subdev_fd, &dv_timings);
	if (ret)
		goto err2;

	/*
	 * This is an example of subscribing to a source change event
	 */

	/* Subscribe to V4L2_EVENT_SOURCE_CHANGE event */
	ret = v4l2_cfg_subscribe_event(adv7611_subdev_fd, V4L2_EVENT_SOURCE_CHANGE, 0);
	if (ret)
		goto err2;

	printf("Poll for \"SRC_CH_RESOLUTION\" event...\n");
	while (1) {
		if (!v4l2_cfg_dequeue_event(adv7611_subdev_fd, &event)) {
			printf("V4L2_EVENT_SOURCE_CHANGE event detected\n");
			/* Get the detected timings from ADV7611 */
			while (v4l2_cfg_query_dv_timings(adv7611_subdev_fd, &dv_timings) != 0);

			/* Set the detected timings to both devices: ADV7611 and ADV7511 */
			ret = v4l2_cfg_set_dv_timings(adv7611_subdev_fd, &dv_timings);
			if (ret) {
				goto err2;
			}
			ret = v4l2_cfg_set_dv_timings(adv7511_subdev_fd, &dv_timings);
			if (ret) {
				goto err2;
			}
		}
	}

	/*
	 * This is an example of subscribing to a control event
	 */

	/* Querry and print the ADV7511_SUBDEV's controls */
	ret = v4l2_cfg_query_ctrls(adv7511_subdev_fd, ctrls, &ctrls_no);
	if (ret)
		goto err2;
	printf("Controls found:\n");
	for (index = 0; index < ctrls_no; index++) {
		printf("\t%s (ID: 0x%x)\n", ctrls[index].name, ctrls[index].id);
	}

	/* Subscribe to "Hotplug Present" control event */
	ret = v4l2_cfg_subscribe_event(adv7511_subdev_fd, V4L2_EVENT_CTRL, ctrls[1].id);
	if (ret)
		goto err2;

	printf("Poll for \"%s\" event...\n", ctrls[1].name);
	while (1) {
		if (!v4l2_cfg_dequeue_event(adv7511_subdev_fd, &event)) {
			if (event.u.ctrl.changes & V4L2_EVENT_CTRL_CH_VALUE) {
				if (event.u.ctrl.type == V4L2_CTRL_TYPE_INTEGER64)
					printf("value = 0x%llx\n", event.u.ctrl.value64);
				else
					printf("value = 0x%x\n", event.u.ctrl.value);
			}
		}
	}

	/*
	 * This is an example of setting controls' values
	 */

	/* Querry and print the ADV7611_SUBDEV's controls */
	ret = v4l2_cfg_query_ctrls(adv7611_subdev_fd, ctrls, &ctrls_no);
	if (ret)
		goto err2;
	printf("Controls found:\n");
	for (index = 0; index < ctrls_no; index++) {
		printf("\t%s (ID: 0x%x)\n", ctrls[index].name, ctrls[index].id);
	}

	/* Disable free running mode */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
									V4L2_CFG_CID_ADV7611_FREE_RUN_MODE, 0);
	if (ret)
		goto err2;

	/* Enable Automatic free running mode */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
									V4L2_CFG_CID_ADV7611_FREE_RUN_MODE, 2);
	if (ret)
		goto err2;

	/* Force free running mode */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
									V4L2_CFG_CID_ADV7611_FREE_RUN_MODE, 1);
	if (ret)
		goto err2;

	/* Specify your own color for free running mode instead of using the
	   default blue color */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
							V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR_MANUAL, 1);
	if (ret)
		goto err2;

	/* Custom free running color can be set in GRB 24-bit format */
	/* Green */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
							V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR, 0xFF0000);
	if (ret)
		goto err2;
	/* Red */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
							V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR, 0x00FF00);
	if (ret)
		goto err2;
	/* Blue */
	ret = v4l2_cfg_set_control_value(adv7611_subdev_fd,
							V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR, 0x0000FF);
	if (ret)
		goto err2;

	close(adv7611_subdev_fd);
	close(adv7511_subdev_fd);

	return 0;

err2:
	close(adv7511_subdev_fd);
err1:
	close(adv7611_subdev_fd);
	return -1;
}
