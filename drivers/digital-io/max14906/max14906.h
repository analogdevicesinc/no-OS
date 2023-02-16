#ifndef _MAX14906_H
#define _MAX14906_H

#include <stdint.h>
#include <stdbool.h>
#include "no_os_gpio.h"
#include "no_os_spi.h"
#include "no_os_util.h"

#define MAX14906_FRAME_SIZE     2
#define MAX14906_CHANNELS       4

#define MAX14906_SETOUT_REG         	0x0
#define MAX14906_SETLED_REG         	0x1
#define MAX14906_DOILEVEL_REG       	0x2
#define MAX14906_INT_REG            	0x3
#define MAX14906_OVR_LD_REG		0x4
#define MAX14906_OPN_WIR_FLT_REG	0x5
#define MAX14906_SHD_VDD_FLT_REG	0x6
#define MAX14906_GLOBAL_FLT_REG     	0x7
#define MAX14906_CONFIG1_REG        	0xA
#define MAX14906_CONFIG2_REG        	0xB
#define MAX14906_CONFIG_DI_REG      	0xC
#define MAX14906_CONFIG_DO_REG      	0xD

/* DoiLevel register */
#define MAX14906_DOI_LEVEL_MASK(x)	NO_OS_BIT(x)

/* SetOUT register */
#define MAX14906_HIGHO_MASK(x)		NO_OS_BIT(x)

#define MAX14906_CHIP_ADDR_MASK		NO_OS_GENMASK(7, 6)
#define MAX14906_ADDR_MASK		NO_OS_GENMASK(4, 1)
#define MAX14906_RW_MASK		NO_OS_BIT(0)

#define MAX14906_DO_MASK(x)		(NO_OS_GENMASK(1, 0) << (2 * x))
#define MAX14906_CH_DIR_MASK(x)		NO_OS_BIT((x) + 4)
#define MAX14906_CH(x)			(x)
#define MAX14906_IEC_TYPE_MASK		NO_OS_BIT(7)

/* Config1 register */
#define MAX14906_SLED_MASK          NO_OS_BIT(1)
#define MAX14906_FLED_MASK          NO_OS_BIT(0)

enum max14906_iec_type {
	MAX14906_TYPE_1_3,
	MAX14906_TYPE_2,
};

enum max14906_function {
	MAX14906_OUT,
	MAX14906_IN,
	MAX14906_HIGH_Z
};

enum max14906_do_mode {
	MAX14906_HIGH_SIDE,
	MAX14906_HIGH_SIDE_INRUSH,
	MAX14906_PUSH_PULL_CLAMP,
	MAX14906_PUSH_PULL
};

struct max14906_ch_config {
	bool enable;
	enum max14906_function function;
	/** The value set to the GPIO in case it's configured as output */
	uint32_t val;
};

struct max14906_init_param {
	uint32_t chip_address;
	struct no_os_spi_init_param *comm_param;
	struct max14906_ch_config ch_config[MAX14906_CHANNELS];
};

struct max14906_desc {
	uint32_t chip_address;
	struct no_os_spi_desc *comm_desc;
	struct no_os_gpio_desc *dio[MAX14906_CHANNELS];
	uint8_t buff[MAX14906_FRAME_SIZE];
};

int max14906_reg_write(struct max14906_desc *, uint32_t, uint32_t);
int max14906_reg_read(struct max14906_desc *, uint32_t, uint32_t *);
int max14906_reg_update(struct max14906_desc *, uint32_t, uint32_t, uint32_t);
int max14906_ch_get(struct max14906_desc *, uint32_t, uint32_t *);
int max14906_ch_set(struct max14906_desc *, uint32_t, uint32_t);
int max14906_ch_func(struct max14906_desc *, uint32_t, enum max14906_function);
int max14906_init(struct max14906_desc **, struct max14906_init_param *);
int max14906_remove(struct max14906_desc *);

#endif