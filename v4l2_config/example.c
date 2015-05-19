
/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
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
	struct v4l2_dv_timings dv_timings;
	int ret;

	/* Get the detected timings from ADV7611 */
	ret = v4l2_cfg_query_dv_timings(ADV7611_SUBDEV, &dv_timings);
	if (ret)
		return ret;

	/* Set the detected timings to both devices: ADV7611 and ADV7511 */
	ret = v4l2_cfg_set_dv_timings(ADV7611_SUBDEV, &dv_timings);
	if (ret)
		return ret;
	ret = v4l2_cfg_set_dv_timings(ADV7511_SUBDEV, &dv_timings);
	if (ret)
		return ret;

	/* Get the configured timings */
	ret = v4l2_cfg_get_dv_timings(ADV7611_SUBDEV, &dv_timings);
	if (ret)
		return ret;
	ret = v4l2_cfg_get_dv_timings(ADV7511_SUBDEV, &dv_timings);
	if (ret)
		return ret;

	return 0;
}

