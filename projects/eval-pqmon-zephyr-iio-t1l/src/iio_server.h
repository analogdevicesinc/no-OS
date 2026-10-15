/*
 * iio_server.h - IIOD server with selectable transport for Zephyr
 */
#ifndef IIO_SERVER_H
#define IIO_SERVER_H

#include <zephyr/kernel.h>

/**
 * Mutex protecting PQLib output data from torn reads.
 * The main loop must hold this while calling pqm_one_cycle() and
 * reading pqlibExample.output for printing.
 * IIO attribute read/write callbacks acquire it internally.
 */
extern struct k_mutex pqm_data_mutex;

/**
 * Initialize transport and start the IIOD server thread.
 * Returns 0 on success, negative errno on failure.
 */
int iio_server_init(void);

#endif /* IIO_SERVER_H */
