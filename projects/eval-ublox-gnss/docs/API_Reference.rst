no-OS GNSS Driver API Reference
================================

This document provides a complete API reference for the no-OS GNSS driver with unified support for UBX-capable devices (modern u-blox) and NMEA-only devices (various manufacturers).

Table of Contents
-----------------

1. `Core API Functions`_
2. `Data Structures`_
3. `Platform Operations`_
4. `Constants and Enumerations`_
5. `Error Codes`_
6. `Usage Examples`_
7. `Platform Integration`_
8. `Thread Safety`_

Core API Functions
------------------

Device Management
~~~~~~~~~~~~~~~~~

``no_os_gnss_init()``
^^^^^^^^^^^^^^^^^^^^^

Initialize a GNSS device with platform-specific operations.

.. code-block:: c

   int32_t no_os_gnss_init(struct no_os_gnss_desc **desc,
                           const struct no_os_gnss_init_param *init_param);

**Parameters:**

- ``desc``: Pointer to GNSS device descriptor pointer (output)
- ``init_param``: API-level initialization parameters

**Returns:** 0 on success, negative error code on failure

**Example:**

.. code-block:: c

   #include "no_os_gnss.h"
   #include "nmea_ubx_gnss.h"

   struct no_os_gnss_desc *gnss_desc;

   // Platform-specific parameters
   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .uart_init = &uart_gnss_ip,
           .device_type = GNSS_DEVICE_UBX_CAPABLE,
           .baud_rate = 38400
       }
   };

   // API-level initialization
   struct no_os_gnss_init_param init_param = {
       .device_id = 0,
       .pps_config = {
           .pps_enable = true,
           .frequency = 1,
           .pulse_length = 100
       },
       .platform_ops = &nmea_ubx_gnss_ops,
       .extra = &platform_init
   };

   ret = no_os_gnss_init(&gnss_desc, &init_param);

``no_os_gnss_remove()``
^^^^^^^^^^^^^^^^^^^^^^^^

Free resources allocated by ``no_os_gnss_init()``.

.. code-block:: c

   int32_t no_os_gnss_remove(struct no_os_gnss_desc *desc);

**Parameters:**

- ``desc``: Device descriptor

**Returns:** 0 on success, negative error code on failure

Unified Timing API
~~~~~~~~~~~~~~~~~~

``no_os_gnss_refresh_timing_data()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Refresh timing data from the GNSS device. Must be called before accessing timing information.

.. code-block:: c

   int32_t no_os_gnss_refresh_timing_data(struct no_os_gnss_desc *desc);

**Parameters:**

- ``desc``: Device descriptor

**Returns:** 0 on success, negative error code on failure

**Usage Notes:**

- Call this function periodically to update timing data
- Required before using any timing getter functions
- For UBX devices: polls NAV-PVT and NAV-TIMEUTC messages
- For NMEA devices: parses GPRMC/GNRMC sentences for timing

``no_os_gnss_is_timing_valid()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Check if current timing data is valid and can be used for synchronization.

.. code-block:: c

   int32_t no_os_gnss_is_timing_valid(struct no_os_gnss_desc *desc, bool *valid);

**Parameters:**

- ``desc``: Device descriptor
- ``valid``: Pointer to validity flag (output)

**Returns:** 0 on success, negative error code on failure

**Example:**

.. code-block:: c

   bool timing_valid;
   int ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
   if (ret == 0 && timing_valid) {
       // Timing data is valid, safe to use for synchronization
   }

``no_os_gnss_get_unified_timing()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Get comprehensive timing information with best available precision.

.. code-block:: c

   int32_t no_os_gnss_get_unified_timing(struct no_os_gnss_desc *desc,
                                         struct no_os_gnss_precise_time *timing);

**Parameters:**

- ``desc``: Device descriptor
- ``timing``: Pointer to timing structure (output)

**Returns:** 0 on success, negative error code on failure

**Precision Notes:**

- **UBX devices**: Nanosecond precision timing (~25ns accuracy)
- **NMEA devices**: Millisecond precision converted to unified format

``no_os_gnss_get_unix_epoch_unified()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Get Unix epoch timestamp with best available sub-second precision.

.. code-block:: c

   uint32_t no_os_gnss_get_unix_epoch_unified(struct no_os_gnss_desc *desc,
                                              uint32_t *fractional_seconds);

**Parameters:**

- ``desc``: Device descriptor
- ``fractional_seconds``: Pointer to fractional seconds (output)

**Returns:** Unix epoch time in seconds, or 0 on error

**Fractional Format:**

- **UBX devices**: Microseconds (0-999999)
- **NMEA devices**: Milliseconds × 1000 (0-999000, steps of 1000)

Position Functions (NMEA Devices)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``no_os_gnss_get_position_data()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Get GPS fix quality and position information for NMEA-only devices.

.. code-block:: c

   int32_t no_os_gnss_get_position_data(struct no_os_gnss_desc *desc,
                                        struct no_os_gnss_position_data *position_data);

**Parameters:**

- ``desc``: Device descriptor
- ``position_data``: Pointer to position structure (output)

**Returns:** 0 on success, negative error code on failure

**Usage Notes:**

- Only available for NMEA-only devices
- Position data refreshed at 1/10th rate of timing data (optimization for timing applications)
- Provides fix quality, satellite count, and coordinates

PPS Configuration
~~~~~~~~~~~~~~~~~

``no_os_gnss_configure_pps()``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configure PPS output with platform-specific parameters.

.. code-block:: c

   int32_t no_os_gnss_configure_pps(struct no_os_gnss_desc *desc,
                                    const void *pps_config);

**Parameters:**

- ``desc``: Device descriptor
- ``pps_config``: Platform-specific PPS configuration structure

**Returns:** 0 on success, negative error code on failure

**Example (UBX devices):**

.. code-block:: c

   struct ublox_pps_config pps_config = {
       .pulse_mode = UBLOX_PPS_PULSE_MODE_PERIOD,
       .period_us = 1000000,  // 1Hz
       .length_us = 100000,   // 100ms pulse width
       .enable = UBLOX_PPS_ENABLE,
       .polarity = UBLOX_PPS_POLARITY_RISING
   };
   ret = no_os_gnss_configure_pps(gnss_desc, &pps_config);

Data Structures
---------------

Core API Structures
~~~~~~~~~~~~~~~~~~~

``struct no_os_gnss_init_param``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

API-level initialization parameters for GNSS device.

.. code-block:: c

   struct no_os_gnss_init_param {
       uint32_t device_id;                           // Device identifier
       struct no_os_gnss_pps_config pps_config;     // PPS configuration
       const struct no_os_gnss_platform_ops *platform_ops;  // Platform operations
       void *extra;                                  // Platform-specific parameters
   };

**Fields:**

- ``device_id``: Unique identifier for the device instance
- ``pps_config``: PPS output configuration settings
- ``platform_ops``: Pointer to platform operations structure (e.g., ``&nmea_ubx_gnss_ops``)
- ``extra``: Platform-specific initialization parameters

``struct no_os_gnss_pps_config``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configuration structure for Pulse-Per-Second (PPS) output.

.. code-block:: c

   struct no_os_gnss_pps_config {
       bool pps_enable;           // Enable/disable PPS output
       uint32_t frequency;        // PPS frequency (Hz)
       uint32_t pulse_length;     // PPS pulse length (ms)
   };

``struct no_os_gnss_desc``
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Device descriptor structure (internal use).

.. code-block:: c

   struct no_os_gnss_desc {
       uint32_t device_id;                           // Device identifier
       const struct no_os_gnss_platform_ops *platform_ops;  // Platform operations
       struct no_os_mutex_desc *mutex;              // Thread safety mutex
       void *extra;                                  // Platform-specific data
   };

Timing Data Structures
~~~~~~~~~~~~~~~~~~~~~~~

``struct no_os_gnss_precise_time``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

High-precision timing information structure.

.. code-block:: c

   struct no_os_gnss_precise_time {
       uint32_t unix_epoch;           // Unix timestamp (seconds)
       int32_t nanoseconds;           // Nanosecond fraction
       uint32_t time_accuracy;        // Time accuracy estimate (ns)
       bool time_valid;               // Time validity flag
       bool time_fully_resolved;      // Full resolution flag
       bool date_valid;               // Date validity flag
       uint16_t year, month, day;     // Date components
       uint8_t hour, minute, second;  // Time components
   };

**Fields:**

- ``unix_epoch``: Standard Unix timestamp in seconds since epoch
- ``nanoseconds``: Sub-second precision (UBX: actual ns, NMEA: ms × 1000000)
- ``time_accuracy``: Timing accuracy estimate in nanoseconds
- ``time_valid``: Indicates if timing data is valid and usable
- ``time_fully_resolved``: Indicates if time is fully resolved (UBX only)
- ``date_valid``: Indicates if date information is valid
- ``year``, ``month``, ``day``: Date components (UTC)
- ``hour``, ``minute``, ``second``: Time components (UTC)

Position Data Structures
~~~~~~~~~~~~~~~~~~~~~~~~~

``struct no_os_gnss_position_data``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Position and fix quality information (NMEA devices).

.. code-block:: c

   struct no_os_gnss_position_data {
       uint8_t fix_quality;          // Fix quality: 0=invalid, 1=GPS, 2=DGPS
       uint8_t satellites_used;      // Number of satellites in use
       float hdop;                   // Horizontal dilution of precision
       float altitude;               // Altitude above mean sea level (meters)
       float latitude;               // Latitude in decimal degrees
       float longitude;              // Longitude in decimal degrees
       bool position_valid;          // Position data validity
       bool fix_valid;               // GPS fix validity
   };

Platform Operations Structure
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``struct no_os_gnss_platform_ops``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Platform-specific operation function pointers.

.. code-block:: c

   struct no_os_gnss_platform_ops {
       int32_t (*init)(struct no_os_gnss_desc **,
                      const struct no_os_gnss_init_param *);
       int32_t (*refresh_timing_data)(struct no_os_gnss_desc *);
       int32_t (*is_timing_valid)(struct no_os_gnss_desc *, bool *);
       int32_t (*get_unified_timing)(struct no_os_gnss_desc *,
                                    struct no_os_gnss_precise_time *);
       uint32_t (*get_unix_epoch_unified)(struct no_os_gnss_desc *, uint32_t *);
       int32_t (*get_position_data)(struct no_os_gnss_desc *,
                                   struct no_os_gnss_position_data *);
       int32_t (*configure_pps)(struct no_os_gnss_desc *, const void *);
       int32_t (*remove)(struct no_os_gnss_desc *);
   };

Constants and Enumerations
---------------------------

Device Types
~~~~~~~~~~~~

.. code-block:: c

   enum gnss_device_type {
       GNSS_DEVICE_UBX_CAPABLE,    // Modern u-blox devices with UBX protocol
       GNSS_DEVICE_NMEA_ONLY       // NMEA-only devices (any manufacturer)
   };

   enum nmea_constellation_support {
       NMEA_GPS_ONLY,      // Legacy GPS-only devices ($GPxxx sentences)
       NMEA_GNSS_MULTI     // Multi-constellation devices ($GNxxx sentences)
   };

PPS Configuration Constants
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #define GNSS_PPS_1HZ         1
   #define GNSS_PPS_LENGTH_100MS 100

UBX-Specific Enumerations (Platform-specific)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   enum ublox_pps_pulse_mode {
       UBLOX_PPS_PULSE_MODE_PERIOD = 0,
       UBLOX_PPS_PULSE_MODE_FREQUENCY = 1
   };

   enum ublox_pps_polarity {
       UBLOX_PPS_POLARITY_FALLING = 0,
       UBLOX_PPS_POLARITY_RISING = 1
   };

Error Codes
-----------

+-----------------+------------------+-----------------------------------------------+
| Code            | Name             | Description                                   |
+=================+==================+===============================================+
| ``0``           | Success          | Operation completed successfully              |
+-----------------+------------------+-----------------------------------------------+
| ``-EINVAL``     | Invalid argument | NULL pointer or invalid parameter             |
+-----------------+------------------+-----------------------------------------------+
| ``-ENOMEM``     | Out of memory    | Memory allocation failed                      |
+-----------------+------------------+-----------------------------------------------+
| ``-EIO``        | I/O error        | Communication error with device               |
+-----------------+------------------+-----------------------------------------------+
| ``-ETIMEDOUT``  | Timeout          | Operation timed out                           |
+-----------------+------------------+-----------------------------------------------+
| ``-ENODATA``    | No data          | Valid response received but no data available |
+-----------------+------------------+-----------------------------------------------+
| ``-ENOTSUP``    | Not supported    | Operation not supported by platform          |
+-----------------+------------------+-----------------------------------------------+

Usage Examples
--------------

Basic Timing Application
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "no_os_gnss.h"
   #include "nmea_ubx_gnss.h"

   int32_t basic_timing_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct no_os_gnss_precise_time timing;
       uint32_t fractional_seconds;
       bool timing_valid;
       int32_t ret;

       // Platform-specific parameters
       struct nmea_ubx_gnss_init_param platform_init = {
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .ubx_input_enable = ENABLE,
               .ubx_output_enable = ENABLE,
               .baud_rate = 38400
           }
       };

       // API-level initialization
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .platform_ops = &nmea_ubx_gnss_ops,
           .extra = &platform_init
       };

       // Initialize device
       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) return ret;

       // Main timing loop
       while (1) {
           // Refresh timing data
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               no_os_mdelay(1000);
               continue;
           }

           // Check validity
           ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
           if (ret || !timing_valid) {
               printf("Waiting for GPS fix...\n");
               no_os_mdelay(5000);
               continue;
           }

           // Get comprehensive timing
           ret = no_os_gnss_get_unified_timing(gnss_desc, &timing);
           if (ret == 0) {
               uint32_t epoch = no_os_gnss_get_unix_epoch_unified(gnss_desc,
                                                                  &fractional_seconds);

               printf("GPS Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                      timing.year, timing.month, timing.day,
                      timing.hour, timing.minute, timing.second);
               printf("Unix Epoch: %lu.%06lu\n", epoch, fractional_seconds);
               printf("Accuracy: %lu ns\n", timing.time_accuracy);
           }

           no_os_mdelay(5000);
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

NMEA Device with Position Monitoring
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   int32_t nmea_position_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct no_os_gnss_position_data position;
       int32_t ret;

       // Platform-specific parameters for NMEA device
       struct nmea_ubx_gnss_init_param platform_init = {
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_NMEA_ONLY,
               .nmea_constellation_type = NMEA_GNSS_MULTI,
               .baud_rate = 9600
           }
       };

       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = false,  // No PPS for NMEA device
           },
           .platform_ops = &nmea_ubx_gnss_ops,
           .extra = &platform_init
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) return ret;

       while (1) {
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret == 0) {
               // Get position data (available every 10th cycle)
               ret = no_os_gnss_get_position_data(gnss_desc, &position);
               if (ret == 0) {
                   printf("Fix Quality: %d, Satellites: %d\n",
                          position.fix_quality, position.satellites_used);
                   printf("Position: %.6f°N, %.6f°E, %.1fm\n",
                          position.latitude, position.longitude, position.altitude);
               }
           }
           no_os_mdelay(5000);
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

Device Configuration Examples
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**UBX-Capable Device (Modern u-blox):**

.. code-block:: c

   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .device_type = GNSS_DEVICE_UBX_CAPABLE,
           .ubx_input_enable = ENABLE,
           .ubx_output_enable = ENABLE,
           .nmea_output_enable = DISABLE,  // UBX only for precision
           .baud_rate = 38400
       }
   };

**NMEA GPS-Only Device (Legacy):**

.. code-block:: c

   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .device_type = GNSS_DEVICE_NMEA_ONLY,
           .nmea_constellation_type = NMEA_GPS_ONLY,
           .baud_rate = 9600
       }
   };

**NMEA Multi-Constellation Device (Modern):**

.. code-block:: c

   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .device_type = GNSS_DEVICE_NMEA_ONLY,
           .nmea_constellation_type = NMEA_GNSS_MULTI,
           .baud_rate = 9600
       }
   };

Platform Integration
--------------------

Adding New GNSS Hardware Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To add support for a new GNSS chipset:

1. **Create Platform Driver** (``drivers/platform/generic/new_device_gnss.c``):

   .. code-block:: c

      // Platform adapter implementing no_os_gnss_platform_ops
      static int32_t new_device_platform_init(struct no_os_gnss_desc **desc,
                                              const struct no_os_gnss_init_param *param);
      // ... other platform operations

      const struct no_os_gnss_platform_ops new_device_gnss_ops = {
          .init = new_device_platform_init,
          .refresh_timing_data = new_device_platform_refresh_timing,
          // ... other operations
      };

2. **Use in Application**:

   .. code-block:: c

      .platform_ops = &new_device_gnss_ops,
      .extra = &new_device_platform_init_param

Thread Safety
-------------

The API provides thread safety through internal mutex protection. All API functions can be called safely from multiple threads:

- **Multiple readers**: Several threads can call getter functions simultaneously
- **Device isolation**: Different device instances can be accessed concurrently
- **Automatic locking**: All operations are automatically protected

**Important Notes:**

- Avoid calling ``no_os_gnss_refresh_timing_data()`` from multiple threads on the same device
- The platform operations themselves must be implemented in a thread-safe manner
- Mutex protection is handled internally by the API layer

Performance Considerations
--------------------------

**Timing Precision:**

- **UBX devices**: Nanosecond precision with ~25ns accuracy
- **NMEA devices**: Millisecond precision, sufficient for most applications

**Update Rates:**

- **Timing data**: Updated every refresh cycle
- **Position data** (NMEA only): Updated every 10th refresh cycle (optimization)

**Communication Efficiency:**

- UBX protocol: Binary, efficient, lower bandwidth
- NMEA protocol: ASCII, universal compatibility, higher bandwidth