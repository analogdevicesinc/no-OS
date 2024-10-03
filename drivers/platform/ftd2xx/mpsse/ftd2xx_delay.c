
#include <stdint.h>
#include <unistd.h>

void no_os_udelay(uint32_t usecs)
{
	usleep(usecs);
}

void no_os_mdelay(uint32_t msecs)
{
	usleep(msecs * 1000);
}
