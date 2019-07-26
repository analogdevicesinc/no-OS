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
			    uint32_t line_rate)
{
	struct xilinx_xcvr_cpll_config cpll_conf;
	struct xilinx_xcvr_qpll_config qpll_conf = {0};
	uint32_t ref_rate, clk25_div;
	uint32_t i;
	int32_t ret;

	ret = xilinx_xcvr_get_qpll_next_config(&xcvr->xlx_xcvr, &ref_rate, line_rate, &qpll_conf);


	return ret;
}
