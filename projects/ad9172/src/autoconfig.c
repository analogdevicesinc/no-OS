#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <xil_io.h>
#include "util.h"
#include "platform_drivers.h"
#include "xilinx_transceiver.h"
#include "axi_adxcvr.h"
#include "autoconfig.h"

int32_t autoconfig(struct adxcvr *xcvr,
			    uint32_t lane_rate)
{
	static struct xilinx_xcvr_cpll_config cpll_conf = {0};
	static struct xilinx_xcvr_qpll_config qpll_conf = {0};
	uint32_t ref_rate;
	int32_t ret = FAILURE;
	xcvr->cpll_enable = 0;
	ret = xilinx_xcvr_get_qpll_next_config(&xcvr->xlx_xcvr, &ref_rate, lane_rate, &qpll_conf);
	if(ret < 0) {
		xcvr->cpll_enable = 1;
		ret = xilinx_xcvr_get_cpll_next_config(&xcvr->xlx_xcvr, &ref_rate, lane_rate, &cpll_conf);
		if(ret < 0)
			return ret;
	}
	xcvr->lane_rate_khz = lane_rate;
	xcvr->ref_rate_khz = ref_rate;

	return ret;
}
