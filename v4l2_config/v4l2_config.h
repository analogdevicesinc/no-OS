
#ifndef V4L2_CONFIG_H_
#define V4L2_CONFIG_H_

#include <linux/videodev2.h>

/******************************************************************************/
/******** Temporary defines until <linux/videodev2.h> will be updated *********/
/******************************************************************************/
#define V4L2_EVENT_SOURCE_CHANGE		5

struct v4l2_event_src_change_new {
	__u32 changes;
};

struct v4l2_event_motion_det_new {
	__u32 flags;
	__u32 frame_sequence;
	__u32 region_mask;
};

struct v4l2_event_new {
	__u32				type;
	union {
		struct v4l2_event_vsync		vsync;
		struct v4l2_event_ctrl		ctrl;
		struct v4l2_event_frame_sync	frame_sync;
		struct v4l2_event_src_change_new	src_change_new;
		struct v4l2_event_motion_det_new	motion_det;
		__u8				data[64];
	} u;
	__u32				pending;
	__u32				sequence;
	struct timespec			timestamp;
	__u32				id;
	__u32				reserved[8];
};

/******************************************************************************/
/***************************** Controls IDs ***********************************/
/******************************************************************************/
#define V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR_MANUAL	\
											(V4L2_CID_DV_CLASS_BASE + 0x1001)
#define V4L2_CFG_CID_ADV7611_FREE_RUN_COLOR			\
											(V4L2_CID_DV_CLASS_BASE + 0x1002)
#define V4L2_CFG_CID_ADV7611_FREE_RUN_MODE			\
											(V4L2_CID_DV_CLASS_BASE + 0x1003)

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int v4l2_cfg_query_dv_timings(int dev_fd,
							  struct v4l2_dv_timings *dv_timings);
int v4l2_cfg_get_dv_timings(int dev_fd,
							struct v4l2_dv_timings *dv_timings);
int v4l2_cfg_set_dv_timings(int dev_fd,
							struct v4l2_dv_timings *dv_timings);
int v4l2_cfg_query_ctrls(int dev_fd,
						 struct v4l2_queryctrl *ctrls,
						 int *ctrls_no);
int v4l2_cfg_subscribe_event(int dev_fd,
							 unsigned int event_type,
							 unsigned int event_id);
int v4l2_cfg_unsubscribe_event(int dev_fd,
							 unsigned int event_type,
							 unsigned int event_id);
int v4l2_cfg_dequeue_event(int dev_fd,
						   struct v4l2_event_new *dqevent);
int v4l2_cfg_get_control_value(int dev_fd,
							   int ctrl_id,
							   int *value);
int v4l2_cfg_set_control_value(int dev_fd,
							   int ctrl_id,
							   int value);
#endif
