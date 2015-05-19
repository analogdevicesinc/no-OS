
#ifndef V4L2_CONFIG_H_
#define V4L2_CONFIG_H_

/******************************************************************************/
/************************ Functions Declarations ******************************/
/******************************************************************************/
int v4l2_cfg_query_dv_timings(const char *dev_path,
							  struct v4l2_dv_timings *dv_timings);
int v4l2_cfg_get_dv_timings(const char *dev_path,
							struct v4l2_dv_timings *dv_timings);
int v4l2_cfg_set_dv_timings(const char *dev_path,
							struct v4l2_dv_timings *dv_timings);
#endif
