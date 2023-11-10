#ifndef __MAXQ1065_H
#define __MAXQ1065_H

#include <stdint.h>
#include "no_os_spi.h"
#include "no_os_gpio.h"

#include "MXQ_API.h"
#include "MXQ_Error.h"
#include "MXQ_Host.h"
#include "MXQ_Types.h"
#include "MXQ_Config.h"

struct maxq1065_init_param {
	uint32_t device_id;
	struct no_os_spi_init_param comm_param;
	struct no_os_gpio_init_param reset_gpio_param;
	struct no_os_gpio_init_param rdy_gpio_param;
};

struct maxq1065_desc {
	uint32_t device_id;
	struct no_os_spi_desc *comm_desc;
	struct no_os_gpio_desc *reset_gpio;
	struct no_os_gpio_desc *rdy_gpio;
	uint8_t tx_buff[512];
	uint8_t rx_buff[512];
};

extern const struct maxq1065_no_os_ops maxq1065_ops;

#endif // __MAXQ1065_H
