
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "v4l2_config.h"

/***************************************************************************//**
* @brief v4l2_cfg_query_dv_timings
*******************************************************************************/
int v4l2_cfg_query_dv_timings(int dev_fd,
							  struct v4l2_dv_timings *dv_timings)
{
	int ret;

	ret = ioctl(dev_fd, VIDIOC_QUERY_DV_TIMINGS, dv_timings);
	if (ret == -1)
		return ret;

#ifdef INFO
	printf("detected resolution: %dx%d\n",
				dv_timings->bt.width, dv_timings->bt.height);
	printf("detected pixel clock: %d.%d MHz\n", 
				(int)(dv_timings->bt.pixelclock / 1000000),
				(int)((dv_timings->bt.pixelclock % 1000000) / 1000));
#endif

	return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_get_dv_timings
*******************************************************************************/
int v4l2_cfg_get_dv_timings(int dev_fd,
							struct v4l2_dv_timings *dv_timings)
{
	int ret;

	ret = ioctl(dev_fd, VIDIOC_G_DV_TIMINGS, dv_timings);
	if (ret == -1)
		return ret;

#ifdef INFO
	printf("configured resolution: %dx%d\n",
		dv_timings->bt.width, dv_timings->bt.height);
	printf("configured pixel clock: %d.%d MHz\n",
		(int)(dv_timings->bt.pixelclock / 1000000),
		(int)((dv_timings->bt.pixelclock % 1000000) / 1000));
#endif

	return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_set_dv_timings
*******************************************************************************/
int v4l2_cfg_set_dv_timings(int dev_fd,
							struct v4l2_dv_timings *dv_timings)
{
	int ret;

	ret = ioctl(dev_fd, VIDIOC_S_DV_TIMINGS, dv_timings);
	if (ret == -1)
		return ret;

	return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_query_ctrls
*******************************************************************************/
int v4l2_cfg_query_ctrls(int dev_fd,
						 struct v4l2_queryctrl *ctrls,
						 int *ctrls_no)
{
	int ret;

	*ctrls_no = 0;
	ctrls[*ctrls_no].id = V4L2_CTRL_FLAG_NEXT_CTRL;
	while(1) {
		ret = ioctl(dev_fd, VIDIOC_QUERYCTRL, &ctrls[*ctrls_no]);
		if (ret == -1)
			break;
		ctrls[*ctrls_no + 1].id =
			(ctrls[*ctrls_no].id | V4L2_CTRL_FLAG_NEXT_CTRL);
		*ctrls_no += 1;
	}

	if (*ctrls_no == 0)
		return -1;
	else
		return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_subscribe_event
*******************************************************************************/
int v4l2_cfg_subscribe_event(int dev_fd,
							 unsigned int event_type,
							 unsigned int event_id)
{
	struct v4l2_event_subscription sub;
	int ret;
	memset(&sub, 0, sizeof(sub));
	sub.type = event_type;
	sub.id = event_id;
	if (event_type == V4L2_EVENT_CTRL)
		sub.flags = V4L2_EVENT_SUB_FL_SEND_INITIAL;

	ret = ioctl(dev_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	if (ret == -1)
		return -1;

	return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_unsubscribe_event
*******************************************************************************/
int v4l2_cfg_unsubscribe_event(int dev_fd,
							 unsigned int event_type,
							 unsigned int event_id)
{
	struct v4l2_event_subscription sub;
	int ret;
	memset(&sub, 0, sizeof(sub));
	sub.type = event_type;
	sub.id = event_id;

	ret = ioctl(dev_fd, VIDIOC_UNSUBSCRIBE_EVENT, &sub);
	if (ret == -1)
		return -1;

	return 0;
}

/***************************************************************************//**
* @brief v4l2_cfg_dequeue_event
*******************************************************************************/
int v4l2_cfg_dequeue_event(int dev_fd,
						   struct v4l2_event_new *dqevent)
{
	return ioctl(dev_fd, VIDIOC_DQEVENT, dqevent);
}

#if 0
/***************************************************************************//**
* @brief v4l2_poll_for_ctrl_event
*******************************************************************************/
int v4l2_poll_for_source_change_event(const char *dev_path,
									  unsigned int event_id)
{
	struct v4l2_event_subscription sub;
	struct v4l2_event_new dqevent;
	int ret;

	memset(&sub, 0, sizeof(sub));
	sub.type = V4L2_EVENT_SOURCE_CHANGE;

	ret = ioctl(dev_fd, VIDIOC_SUBSCRIBE_EVENT, &sub);
	if (ret == -1)
		return -1;

	while (1) {
		if (!ioctl(dev_fd, VIDIOC_DQEVENT, &dqevent)) {
			printf("source_change: pad/input=%d changes: %x\n", dqevent.id, dqevent.u.src_change.changes);
		}
	}

	return 0;
}
#endif

