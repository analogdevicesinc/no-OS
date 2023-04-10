#ifndef MWC_H
#define MWC_H

#include "no_os_gpio.h"
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
	uint8_t **tx_auto_if_correlation;
	uint8_t tx_auto_if_correlation_entries;
	bool rx_auto_ifvga_rflna;
	uint8_t **rx_auto_if_lna_correlation;
	uint8_t rx_auto_if_lna_correlation_entries;
	struct no_os_pid *tx_pid;
	struct no_os_pid *rx_pid;
	enum admv96xx_id id;
	bool hbtx;
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
	uint8_t **tx_auto_if_correlation;
	uint8_t tx_auto_if_correlation_entries;
	bool rx_auto_ifvga_rflna;
	uint8_t **rx_auto_if_lna_correlation;
	uint8_t rx_auto_if_lna_correlation_entries;
	enum admv96xx_id id;
	bool hbtx;
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
};

int mwc_iio_init(struct mwc_iio_dev **iio_dev,
			struct mwc_iio_init_param *init_param);
int mwc_iio_remove(struct mwc_iio_dev *desc);
int mwc_tx_rx_reset(struct mwc_iio_dev *mwc);

int mwc_algorithms(struct mwc_iio_dev *mwc);

#endif
