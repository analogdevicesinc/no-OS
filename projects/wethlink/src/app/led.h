#include <stdbool.h>
#include <stdint.h>

int led_init(void);
void led_tx_lock(bool on);
void led_rx_lock(bool on);
void led_tx_det_green(bool on);
void led_tx_det_red(bool on);
void led_rx_det(int brightness);