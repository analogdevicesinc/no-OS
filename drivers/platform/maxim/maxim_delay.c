#include "no-os/delay.h"
#include "mxc_delay.h"

void udelay(uint32_t usecs)
{
	mxc_delay(MXC_DELAY_USEC(usecs));
}

void mdelay(uint32_t msecs)
{
	mxc_delay(MXC_DELAY_MSEC(msecs));
}

