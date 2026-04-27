/*
 * Copyright (c) 2026 Analog Devices, Inc.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file
 * @brief Common API One-Wire Master (OWM)
 *
 * The 1-Wire protocol is a low-speed, half-duplex serial communication protocol that uses a
 * single data line plus ground for communication and power. Each device on the 1-Wire bus has a
 * unique 64-bit ROM ID, allowing for easy identification and communication. The OWM module manages
 * the lower-level details (including timing and drive modes) required by the 1-Wire protocol,
 * enabling the CPU to communicate over the 1-Wire bus at a logical data level.
 *
 * Typical usage:
 *  1. Initialize the controller with capi_owm_init().
 *  2. Reset the bus with capi_owm_reset_bus() to detect slave presence.
 *  3. Use ROM commands to discover or select slave devices.
 *  4. Perform data transfers using bit, byte, or block operations.
 *  5. Deinitialize with capi_owm_deinit() when done.
 */

#ifndef _CAPI_OWM_H_
#define _CAPI_OWM_H_

#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/** Size of a 1-Wire ROM ID in bytes (64 bits). */
#define CAPI_OWM_ROM_ID_SIZE 8U

/**
 * @brief 1-Wire bus speed mode.
 */
enum capi_owm_speed {
	CAPI_OWM_SPEED_NORMAL,    /**< Normal (standard) speed */
	CAPI_OWM_SPEED_OVERDRIVE, /**< Overdrive (high) speed */
};

/**
 * @brief Async event types reported via callback.
 */
enum capi_owm_async_event {
	CAPI_OWM_EVENT_RESET_DONE,     /**< Bus reset sequence completed */
	CAPI_OWM_EVENT_TX_DONE,        /**< Transmit operation completed */
	CAPI_OWM_EVENT_RX_DONE,        /**< Receive operation completed */
	CAPI_OWM_EVENT_PRESENCE_PULSE, /**< Slave(s) detected after reset */
	CAPI_OWM_EVENT_LINE_SHORT,     /**< Short circuit detected on bus */
	CAPI_OWM_EVENT_ERROR,          /**< Communication error */
};

/**
 * @brief OWM controller configuration.
 */
struct capi_owm_config {
	/** Platform specific implementation of the API. (Mandatory field, but can be NULL and we'll
	 * make sure it gets auto populated when using our build system.) */
	const struct capi_owm_ops *ops;
	/** OWM controller identifier:
	 * - Base address for internal controllers.
	 * - Device specific address for external controllers.
	 */
	uint64_t identifier;
	/** Frequency of peripheral clock in Hz */
	uint32_t clk_freq_hz;
	/** Enable internal pullup resistor. */
	bool int_pullup_enable;
	/** Enable external strong pullup driver. */
	bool ext_pullup_enable;
	/** Initial bus speed: normal or overdrive. */
	enum capi_owm_speed speed;
	/** Optional platform specific extra configuration. */
	void *extra;
};

/**
 * @brief OWM controller handle.
 *
 * Drivers may need own handle type to handle internals.
 * Driver developer shall declare this as the first field of private handle structure.
 */
struct capi_owm_handle {
	const struct capi_owm_ops *ops; /**< set and used by capi thin layer */
	bool init_allocated;            /**< If true, the driver is owner of handle memory. */
	void *priv;                     /**< set and used by user optionally */
};

/**
 * @brief OWM callback type for async events.
 *
 * @param[in] event     Async event type.
 * @param[in] arg       Pointer to user specific data.
 * @param[in] event_extra Optional, platform/driver specific extra information for event.
 */
typedef void (*capi_owm_callback)(enum capi_owm_async_event event, void *arg, int event_extra);

/* -------------------------------------------------------------------------- */
/* Initialization / Deinitialization                                          */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize an instance of the OWM controller.
 *
 * @param [in,out] handle Points to a pointer of the context structure. If this
 *                 pointer is set to NULL, then the controller will allocate
 *                 the context structure and be the owner. If the pointer
 *                 is not NULL, the application has allocated the structure and is
 *                 the owner.
 * @param [in] config Points to the configuration for the OWM controller.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_init(struct capi_owm_handle **handle, const struct capi_owm_config *config);

/**
 * @brief Deinitialize the OWM controller, disable, and bring to default settings.
 *
 * @param [in] handle Points to the OWM controller context.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_deinit(struct capi_owm_handle *handle);

/* -------------------------------------------------------------------------- */
/* Configuration                                                              */
/* -------------------------------------------------------------------------- */

/**
 * @brief Set the 1-Wire bus speed.
 *
 * Switches the controller between normal and overdrive timing.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] speed  Desired bus speed.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_set_speed(struct capi_owm_handle *handle, enum capi_owm_speed speed);

/**
 * @brief Get the current 1-Wire bus speed setting.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] speed  Current bus speed.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_get_speed(struct capi_owm_handle *handle, enum capi_owm_speed *speed);

/**
 * @brief Control the strong pullup.
 *
 * When enabled, the strong pullup resistor is activated immediately after the next
 * written data (byte or block). This is used for parasitic-power slaves that require
 * extra current during operations such as temperature conversion or EEPROM writes.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] enable true: arm strong pullup after next write; false: disable.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_set_strong_pullup(struct capi_owm_handle *handle, bool enable);

/* -------------------------------------------------------------------------- */
/* Bus Reset                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Reset the 1-Wire bus.
 *
 * Transmits a reset pulse (short or long depending on the configured speed) and
 * listens for the presence pulse from slave device(s).
 *
 * @param [in]  handle   Points to the OWM controller context.
 * @param [out] presence Set to true if one or more slaves responded with a presence
 *                       pulse, false otherwise.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_reset_bus(struct capi_owm_handle *handle, bool *presence);

/* -------------------------------------------------------------------------- */
/* ROM Commands                                                               */
/* -------------------------------------------------------------------------- */

/**
 * @brief Search for slave devices on the bus.
 *
 * Performs the 1-Wire Search ROM algorithm to discover slave devices. Call with
 * @p new_search set to true to start a fresh search, then call repeatedly with
 * @p new_search set to false to enumerate additional devices. When no more devices
 * remain, @p found is set to false.
 *
 * An optional @p family_code filter may be applied. Set to 0 to search all families.
 *
 * @param [in]     handle      Points to the OWM controller context.
 * @param [in]     new_search  true: start new search; false: continue previous.
 * @param [in]     family_code Family code filter (0 = no filter).
 * @param [out]    rom_id      Buffer of at least CAPI_OWM_ROM_ID_SIZE bytes to
 *                             receive the discovered ROM ID.
 * @param [out]    found       true if a ROM ID was found, false if search is done.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_search(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
		    uint8_t *rom_id, bool *found);

/**
 * @brief Search for slave devices with active alarms.
 *
 * Identical to capi_owm_search() but uses the Alarm Search ROM command (0xEC) to
 * discover only those slaves that have an active alarm condition.
 *
 * @param [in]     handle      Points to the OWM controller context.
 * @param [in]     new_search  true: start new search; false: continue previous.
 * @param [in]     family_code Family code filter (0 = no filter).
 * @param [out]    rom_id      Buffer of at least CAPI_OWM_ROM_ID_SIZE bytes to
 *                             receive the discovered ROM ID.
 * @param [out]    found       true if a ROM ID was found, false if search is done.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_search_alarm(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
			  uint8_t *rom_id, bool *found);

/**
 * @brief Read the ROM ID from a single slave device.
 *
 * Sends the Read ROM command (0x33). This may only be used when a single slave is
 * present on the bus.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] rom_id Buffer of at least CAPI_OWM_ROM_ID_SIZE bytes to receive
 *                     the 64-bit ROM ID.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_read_rom(struct capi_owm_handle *handle, uint8_t *rom_id);

/**
 * @brief Select a slave device by ROM ID.
 *
 * Sends the Match ROM command (0x55) followed by the 64-bit ROM ID to select a
 * specific slave for subsequent communication.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] rom_id Pointer to the 64-bit ROM ID of the target slave.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_match_rom(struct capi_owm_handle *handle, const uint8_t *rom_id);

/**
 * @brief Skip ROM selection.
 *
 * Sends the Skip ROM command (0xCC). In single-drop configurations this addresses
 * the sole device without requiring its ROM ID. On a multi-drop bus this sends a
 * broadcast.
 *
 * @param [in] handle Points to the OWM controller context.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_skip_rom(struct capi_owm_handle *handle);

/**
 * @brief Resume communication with the most recently addressed device.
 *
 * Sends the Resume ROM command (0xA5). This re-selects the last device that was
 * addressed by a Match ROM or Search ROM command without having to resend the full
 * 64-bit ROM ID.
 *
 * @param [in] handle Points to the OWM controller context.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_resume(struct capi_owm_handle *handle);

/**
 * @brief Select a slave device using Overdrive Match ROM.
 *
 * Sends the Overdrive Match ROM command (0x69) at normal speed followed by the
 * 64-bit ROM ID at overdrive speed. Both master and the addressed slave switch to
 * overdrive timing.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] rom_id Pointer to the 64-bit ROM ID of the target slave.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_overdrive_match_rom(struct capi_owm_handle *handle, const uint8_t *rom_id);

/**
 * @brief Enter overdrive mode using Overdrive Skip ROM.
 *
 * Sends the Overdrive Skip ROM command (0x3C) at normal speed. All overdrive-capable
 * slaves on the bus switch to overdrive timing.
 *
 * @param [in] handle Points to the OWM controller context.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_overdrive_skip_rom(struct capi_owm_handle *handle);

/* -------------------------------------------------------------------------- */
/* Bit-Level Operations                                                       */
/* -------------------------------------------------------------------------- */

/**
 * @brief Write a single bit to the 1-Wire bus.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] bit    Bit value to write (0 or 1).
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_write_bit(struct capi_owm_handle *handle, uint8_t bit);

/**
 * @brief Read a single bit from the 1-Wire bus.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] bit    Pointer to store the read bit value (0 or 1).
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_read_bit(struct capi_owm_handle *handle, uint8_t *bit);

/* -------------------------------------------------------------------------- */
/* Byte-Level Operations                                                      */
/* -------------------------------------------------------------------------- */

/**
 * @brief Write a single byte to the 1-Wire bus.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] byte   Byte to write.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_write_byte(struct capi_owm_handle *handle, uint8_t byte);

/**
 * @brief Read a single byte from the 1-Wire bus.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] byte   Pointer to store the read byte.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_read_byte(struct capi_owm_handle *handle, uint8_t *byte);

/* -------------------------------------------------------------------------- */
/* Block-Level Operations                                                     */
/* -------------------------------------------------------------------------- */

/**
 * @brief Sync/Blocking write of a data block to the 1-Wire bus.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] buf    Pointer to the transmit buffer.
 * @param [in] len    Number of bytes to write.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_write_block(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len);

/**
 * @brief Sync/Blocking read of a data block from the 1-Wire bus.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] buf    Pointer to the receive buffer.
 * @param [in]  len    Number of bytes to read.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_read_block(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len);

/**
 * @brief Async/Non-blocking write of a data block to the 1-Wire bus.
 *
 * Completion is reported via the registered callback with CAPI_OWM_EVENT_TX_DONE.
 *
 * @param [in] handle Points to the OWM controller context.
 * @param [in] buf    Pointer to the transmit buffer. The buffer must remain valid
 *                    until the callback is invoked.
 * @param [in] len    Number of bytes to write.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_write_block_async(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len);

/**
 * @brief Async/Non-blocking read of a data block from the 1-Wire bus.
 *
 * Completion is reported via the registered callback with CAPI_OWM_EVENT_RX_DONE.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] buf    Pointer to the receive buffer. The buffer must remain valid
 *                     until the callback is invoked.
 * @param [in]  len    Number of bytes to read.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_read_block_async(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len);

/* -------------------------------------------------------------------------- */
/* Utility                                                                    */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get the number of slave devices on the bus.
 *
 * Performs a full search ROM enumeration and returns the total count. This is a
 * convenience wrapper around capi_owm_search().
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [out] count  Number of slaves discovered on the bus.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_get_device_count(struct capi_owm_handle *handle, uint32_t *count);

/**
 * @brief Compute the 1-Wire CRC-8 over a data buffer.
 *
 * Uses the standard DOW CRC polynomial (x^8 + x^5 + x^4 + 1). This can be used
 * to verify ROM IDs and data integrity.
 *
 * @param [in]  handle Points to the OWM controller context.
 * @param [in]  data   Pointer to the data buffer.
 * @param [in]  len    Length of the data buffer in bytes.
 * @param [out] crc    Pointer to store the computed CRC-8 value.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_crc8(struct capi_owm_handle *handle, const uint8_t *data, uint32_t len, uint8_t *crc);

/* -------------------------------------------------------------------------- */
/* Callback / Async Support                                                   */
/* -------------------------------------------------------------------------- */

/**
 * @brief Register callback for async events.
 *
 * @param [in] handle       Points to the OWM controller context.
 * @param [in] callback     User callback function.
 * @param [in] callback_arg User callback argument.
 *
 * @return int 0 for success or negative error code.
 */
int capi_owm_register_callback(struct capi_owm_handle *handle, capi_owm_callback const callback,
			       void *const callback_arg);

/* -------------------------------------------------------------------------- */
/* ISR                                                                        */
/* -------------------------------------------------------------------------- */

/**
 * @brief OWM driver interrupt handler.
 *
 * If interrupt vectors are managed and implemented by user, then user shall call
 * this function in the relevant interrupt vector function.
 *
 * @param [in] handle Points to the OWM controller context.
 */
void capi_owm_isr(void *handle);

/* -------------------------------------------------------------------------- */
/* Ops Table                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief Structure holding OWM function pointers that point to the platform
 * specific function. See API functions for relevant descriptions.
 */
struct capi_owm_ops {
	/** See capi_owm_init() */
	int (*init)(struct capi_owm_handle **handle, const struct capi_owm_config *config);
	/** See capi_owm_deinit() */
	int (*deinit)(struct capi_owm_handle *handle);
	/** See capi_owm_set_speed() */
	int (*set_speed)(struct capi_owm_handle *handle, enum capi_owm_speed speed);
	/** See capi_owm_get_speed() */
	int (*get_speed)(struct capi_owm_handle *handle, enum capi_owm_speed *speed);
	/** See capi_owm_set_strong_pullup() */
	int (*set_strong_pullup)(struct capi_owm_handle *handle, bool enable);
	/** See capi_owm_reset_bus() */
	int (*reset_bus)(struct capi_owm_handle *handle, bool *presence);
	/** See capi_owm_search() */
	int (*search)(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
		      uint8_t *rom_id, bool *found);
	/** See capi_owm_search_alarm() */
	int (*search_alarm)(struct capi_owm_handle *handle, bool new_search, uint8_t family_code,
			    uint8_t *rom_id, bool *found);
	/** See capi_owm_read_rom() */
	int (*read_rom)(struct capi_owm_handle *handle, uint8_t *rom_id);
	/** See capi_owm_match_rom() */
	int (*match_rom)(struct capi_owm_handle *handle, const uint8_t *rom_id);
	/** See capi_owm_skip_rom() */
	int (*skip_rom)(struct capi_owm_handle *handle);
	/** See capi_owm_resume() */
	int (*resume)(struct capi_owm_handle *handle);
	/** See capi_owm_overdrive_match_rom() */
	int (*overdrive_match_rom)(struct capi_owm_handle *handle, const uint8_t *rom_id);
	/** See capi_owm_overdrive_skip_rom() */
	int (*overdrive_skip_rom)(struct capi_owm_handle *handle);
	/** See capi_owm_write_bit() */
	int (*write_bit)(struct capi_owm_handle *handle, uint8_t bit);
	/** See capi_owm_read_bit() */
	int (*read_bit)(struct capi_owm_handle *handle, uint8_t *bit);
	/** See capi_owm_write_byte() */
	int (*write_byte)(struct capi_owm_handle *handle, uint8_t byte);
	/** See capi_owm_read_byte() */
	int (*read_byte)(struct capi_owm_handle *handle, uint8_t *byte);
	/** See capi_owm_write_block() */
	int (*write_block)(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len);
	/** See capi_owm_read_block() */
	int (*read_block)(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_owm_write_block_async() */
	int (*write_block_async)(struct capi_owm_handle *handle, const uint8_t *buf, uint32_t len);
	/** See capi_owm_read_block_async() */
	int (*read_block_async)(struct capi_owm_handle *handle, uint8_t *buf, uint32_t len);
	/** See capi_owm_get_device_count() */
	int (*get_device_count)(struct capi_owm_handle *handle, uint32_t *count);
	/** See capi_owm_crc8() */
	int (*crc8)(struct capi_owm_handle *handle, const uint8_t *data, uint32_t len,
		    uint8_t *crc);
	/** See capi_owm_register_callback() */
	int (*register_callback)(struct capi_owm_handle *handle, capi_owm_callback const callback,
				 void *const callback_arg);
	/** See capi_owm_isr() */
	void (*isr)(void *handle);
};

#if defined(__cplusplus)
}
#endif

#endif /* _CAPI_OWM_H_ */
