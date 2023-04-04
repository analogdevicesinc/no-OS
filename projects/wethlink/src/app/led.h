#include <stdbool.h>

int led_init(void);
int led_tx_lock(bool on);
int led_rx_lock(bool on);
int led_tx_det_green(bool on);
int led_tx_det_red(bool on);