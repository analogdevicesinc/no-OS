#ifndef MWC_H
#define MWC_H

#include "no_os_gpio.h"
#include "no_os_eeprom.h"
#include "iio.h"
#include "iio_hmc630x.h"
#include "hmc630x.h"
#include "no_os_pid.h"

enum admv96xx_id {
	ID_ADMV96X1 = 1,
	ID_ADMV96X3 = 3,
	ID_ADMV96X5 = 5,
};

struct mwc_iio_dev {
	struct iio_device *iio_dev;
	struct hmc630x_iio_dev *tx_iiodev;
	struct hmc630x_iio_dev *rx_iiodev;
	struct no_os_gpio_desc *reset_gpio;
	bool tx_autotuning;
	unsigned int tx_target;
	unsigned int tx_tolerance;
	bool rx_autotuning;
	unsigned int rx_target;
	unsigned int rx_tolerance;
	bool tx_auto_ifvga;
	bool rx_auto_ifvga_rflna;
	uint8_t (*temp_correlation)[5];
	struct no_os_pid *tx_pid;
	struct no_os_pid *rx_pid;
	enum admv96xx_id id;
	bool hbtx;
	const uint8_t *crc8;
	struct no_os_eeprom_desc *eeprom;
	struct adin1300_desc *adin1300;
	struct max24287_desc *max24287;
};

struct mwc_iio_init_param {
	struct no_os_gpio_init_param *reset_gpio_ip;
	bool tx_autotuning;
	unsigned int tx_target;
	unsigned int tx_tolerance;
	bool rx_autotuning;
	unsigned int rx_target;
	unsigned int rx_tolerance;
	bool tx_auto_ifvga;
	bool rx_auto_ifvga_rflna;
	uint8_t (*temp_correlation)[5];
	enum admv96xx_id id;
	bool hbtx;
	const uint8_t *crc8;
	struct no_os_eeprom_desc *eeprom;
	struct adin1300_desc *adin1300;
	struct max24287_desc *max24287;
};

enum mwc_iio_attr_id {
	MWC_IIO_ATTR_TX_AUTOTUNING,
	MWC_IIO_ATTR_TX_TARGET,
	MWC_IIO_ATTR_TX_TOLERANCE,
	MWC_IIO_ATTR_RX_AUTOTUNING,
	MWC_IIO_ATTR_RX_TARGET,
	MWC_IIO_ATTR_RX_TOLERANCE,
	MWC_IIO_ATTR_TX_AUTO_IFVGA,
	MWC_IIO_ATTR_RX_AUTO_IFVGA_RFLNA,
	MWC_IIO_ATTR_RESET,
	MWC_IIO_ATTR_SAVE,
};

struct nvmp {
	char hw_version[10];
	char hw_serial[10];
	char carrier_model[20];
	char carrier_version[10];
	char carrier_serial[10];
	bool tx_autotuning;
	unsigned int tx_target;
	unsigned int tx_tolerance;
	bool rx_autotuning;
	unsigned int rx_target;
	unsigned int rx_tolerance;
	bool tx_auto_ifvga;
	bool rx_auto_ifvga_rflna;
	uint8_t temp_correlation[2][4][5];

	bool hmc6300_enabled;
	uint64_t hmc6300_vco[2]; // {lbtx vco, hbtx vco}
	uint8_t hmc6300_if_attn;
	uint8_t hmc6300_rf_attn;

	bool hmc6301_enabled;
	uint64_t hmc6301_vco[2]; // {lbtx vco, hbtx vco}
	uint8_t hmc6301_if_attn;
	enum hmc6301_lna_attn hmc6301_lna_attn;
	enum hmc6301_bb_attn hmc6301_bb_attn1;
	enum hmc6301_bb_attn hmc6301_bb_attn2;
	enum hmc6301_bb_attn_fine hmc6301_bb_attni_fine;
	enum hmc6301_bb_attn_fine hmc6301_bb_attnq_fine;
};

extern const struct nvmp factory_defaults;

int mwc_iio_init(struct mwc_iio_dev **iio_dev,
			struct mwc_iio_init_param *init_param);
int mwc_iio_remove(struct mwc_iio_dev *desc);
int mwc_tx_rx_reset(struct mwc_iio_dev *mwc);

int mwc_algorithms(struct mwc_iio_dev *mwc);
int mwc_save_to_eeprom(struct mwc_iio_dev *mwc);

#endif
