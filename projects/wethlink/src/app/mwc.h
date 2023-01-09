#ifndef MWC_H
#define MWC_H

#include "no_os_gpio.h"
#include "iio.h"
#include "iio_hmc630x.h"
#include "hmc630x.h"
#include "no_os_pid.h"

struct mwc_iio_dev {
	struct iio_device *iio_dev;
	struct hmc630x_iio_dev *tx_iiodev;
	struct hmc630x_iio_dev *rx_iiodev;
	struct no_os_gpio_desc *reset_gpio;
	bool tx_autotuning;
	unsigned int tx_target;
	bool rx_autotuning;
	unsigned int rx_target;
	bool tx_auto_ifvga;
	bool rx_auto_ifvga_rflna;
	bool hbtx;
	struct no_os_pid *tx_pid;
	struct no_os_pid *rx_pid;
};

struct mwc_iio_init_param {
	struct no_os_gpio_init_param *reset_gpio_ip;
	bool tx_autotuning;
	unsigned int tx_target;
	bool rx_autotuning;
	unsigned int rx_target;
	bool tx_auto_ifvga;
	bool rx_auto_ifvga_rflna;
	bool hbtx;
};

enum mwc_iio_attr_id {
	MWC_IIO_ATTR_TX_AUTOTUNING,
	MWC_IIO_ATTR_TX_TARGET,
	MWC_IIO_ATTR_RX_AUTOTUNING,
	MWC_IIO_ATTR_RX_TARGET,
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
