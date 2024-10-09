
#include "parameters.h"

struct ftd2xx_spi_init ad74416h_spi_extra_ip  = {
	.channel_config_pin =
		((1 << 6) |	 /*  BIT7 - BIT0:   Initial direction of the pins */
		(1 << 6) << 8 |	 /* BIT15 - BIT8:   Initial values of the pins	  */
		(1 << 6) << 16 | /* BIT23 - BIT16: Final direction of the pins	  */
		(1 << 6) << 24), /* BIT31 - BIT24: Final values of the pins	  */
};
