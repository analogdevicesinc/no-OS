#ifndef __APP_AD9083_H
#define __APP_AD9083_H

#include <stdint.h>
#include <stdbool.h>
#include "ad9083.h"

/**
 * @struct app_ad9083_init
 * @brief Structure holding the parameters for ad9083 app initialization.
 */
struct app_ad9083_init {
	/* Settings selection */
	uint8_t uc;
	/* jesd receive clock */
	struct no_os_clk *jesd_rx_clk;
};

/**
 * @struct app_ad9083
 * @brief Structure holding ad9083 app descriptor.
 */
struct app_ad9083 {
	/* Structure holding ad9083 device descriptor */
	struct ad9083_phy *ad9083_phy;
};

/* Initialize the ad9083 app. */
int32_t app_ad9083_init(struct app_ad9083 **app,
			struct app_ad9083_init *init_param);

/* Remove the app. */
int32_t app_ad9083_remove(struct app_ad9083 *app);

/* Check SYSREF clock. */
bool app_ad9083_check_sysref_rate(uint32_t lmfc, uint32_t sysref);

/* Check AD9083 subclass 1 status. */
int32_t app_ad9083_subclass1_status(struct app_ad9083 *app);

/* Check AD9083 status. */
int32_t app_ad9083_status(struct app_ad9083 *app);

#endif /* __APP_AD9083_H */
