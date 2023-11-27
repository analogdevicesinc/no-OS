#include <stdbool.h>
#include <stdint.h>

enum rj45_led {
	rj45_led_off,
	rj45_led_green,
	rj45_led_yellow,
};

int led_init(void);
void led_tx_lock(bool on);
void led_rx_lock(bool on);
void led_tx_det_green(bool on);
void led_tx_det_red(bool on);
void led_rx_det_green(bool on);
void led_rx_det_red(bool on);
void led_rj45(enum rj45_led state);
void led_blink_all(unsigned int times, unsigned int duration);