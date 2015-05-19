
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include "v4l2_config.h"

/***************************************************************************//**
* @brief v4l2_cfg_query_dv_timings
*******************************************************************************/
int v4l2_cfg_query_dv_timings(const char *dev_path,
							  struct v4l2_dv_timings *dv_timings)
{
	int dev_fd;
	int ret;

	dev_fd = open(dev_path, O_RDWR | O_NONBLOCK, 0);
	if (dev_fd == -1)
		return -1;

	ret = ioctl(dev_fd, VIDIOC_QUERY_DV_TIMINGS, dv_timings);
	if (ret == -1)
		goto err;

	close(dev_fd);

#ifdef INFO
	printf("%s: detected resolution: %dx%d\n", dev_path,
				dv_timings->bt.width, dv_timings->bt.height);
	printf("%s: detected pixel clock: %d.%d MHz\n", dev_path, 
				(int)(dv_timings->bt.pixelclock / 1000000),
				(int)((dv_timings->bt.pixelclock % 1000000) / 1000));
#endif

	return 0;

err:
	close(dev_fd);
	return -1;
}

/***************************************************************************//**
* @brief v4l2_cfg_get_dv_timings
*******************************************************************************/
int v4l2_cfg_get_dv_timings(const char *dev_path,
							struct v4l2_dv_timings *dv_timings)
{
	int dev_fd;
	int ret;

	dev_fd = open(dev_path, O_RDWR | O_NONBLOCK, 0);
	if (dev_fd == -1)
		return -1;

	ret = ioctl(dev_fd, VIDIOC_G_DV_TIMINGS, dv_timings);
	if (ret == -1)
		goto err;

	close(dev_fd);

#ifdef INFO
	printf("%s: configured resolution: %dx%d\n", dev_path,
				dv_timings->bt.width, dv_timings->bt.height);
	printf("%s: configured pixel clock: %d.%d MHz\n", dev_path, 
				(int)(dv_timings->bt.pixelclock / 1000000),
				(int)((dv_timings->bt.pixelclock % 1000000) / 1000));
#endif

	return 0;

err:
	close(dev_fd);
	return -1;
}

/***************************************************************************//**
* @brief v4l2_cfg_set_dv_timings
*******************************************************************************/
int v4l2_cfg_set_dv_timings(const char *dev_path,
							struct v4l2_dv_timings *dv_timings)
{
	int dev_fd;
	int ret;

	dev_fd = open(dev_path, O_RDWR | O_NONBLOCK, 0);
	if (dev_fd == -1)
		return -1;

	ret = ioctl(dev_fd, VIDIOC_S_DV_TIMINGS, dv_timings);
	if (ret == -1)
		goto err;

	close(dev_fd);

	return 0;

err:
	close(dev_fd);
	return -1;
}

