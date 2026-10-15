/*
 * uart_export.h — UART2 CSV data export
 *
 * Exports PQ measurements as CSV over UART2, compatible with
 * the no-OS eval-pqmon UART export format.
 */
#ifndef __UART_EXPORT_H__
#define __UART_EXPORT_H__

/**
 * @brief Initialize UART2 for CSV export
 * @return 0 on success, negative errno on failure
 */
int uart_export_init(void);

/**
 * @brief Send one CSV line with current PQ measurements
 *
 * Format: PQM,VA,VB,VC,IA,IB,IC,IN,THD_VA,...,power/energy/pf\r\n
 * Call once per second from main loop.
 */
void uart_export_send(void);

#ifdef CONFIG_WAVEFORM_UART_EXPORT
/**
 * @brief Initialize waveform ring buffer and start UART2 binary export thread
 *
 * Requires uart_export_init() to have been called first (for UART2 device).
 * Spawns a dedicated preemptible thread that streams binary waveform frames.
 */
void waveform_uart_export_init(void);
#endif

#endif /* __UART_EXPORT_H__ */
