#ifndef WIFI_DRIVER_H
# define WIFI_DRIVER_H

#include <network_private.h>
#include <stdint.h>

struct wifi_desc;

struct wifi_init_param {
	struct uart_init_param	*uart_param;
};

int32_t wifi_init(struct wifi_desc **desc, struct wifi_init_param *param);
int32_t wifi_remove(struct wifi_desc *desc);
int32_t wifi_connect(struct wifi_desc *desc, const char *ssid,
			const char *pass);
int32_t wifi_disconnect(struct wifi_desc *desc);
int32_t wifi_get_network_interface(struct wifi_desc *desc,
		struct network_interface **net);

#endif
