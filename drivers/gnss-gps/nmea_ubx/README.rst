GNSS/GPS Driver for no-OS Framework
====================================

This directory contains GNSS/GPS drivers for the no-OS framework, providing unified support for various GNSS devices with clean abstraction layers.

Architecture Overview
---------------------

The GNSS subsystem follows no-OS framework patterns with three distinct layers:

.. code-block:: text

   ┌─────────────────────────────────────────────────────────┐
   │                    Application Layer                     │
   │              (projects/eval-ublox-gnss)                 │
   └─────────────────────┬───────────────────────────────────┘
                         │ Uses no_os_gnss_* API
   ┌─────────────────────▼───────────────────────────────────┐
   │                  API Layer                              │
   │         (drivers/api/no_os_gnss.c)                      │
   │           + (include/no_os_gnss.h)                      │
   └─────────────────────┬───────────────────────────────────┘
                         │ Calls platform_ops->*()
   ┌─────────────────────▼───────────────────────────────────┐
   │               Platform Layer                            │
   │       (drivers/platform/generic/nmea_ubx_gnss.c)       │
   └─────────────────────┬───────────────────────────────────┘
                         │ Uses nmea_ubx driver functions
   ┌─────────────────────▼───────────────────────────────────┐
   │                Device Driver Layer                      │
   │           (drivers/gnss-gps/nmea_ubx/)                  │
   └─────────────────────────────────────────────────────────┘

Driver Directories
------------------

``nmea_ubx/``
~~~~~~~~~~~~~

Universal NMEA/UBX driver supporting both protocols:

- **UBX Protocol**: Modern u-blox devices with CFG-VALSET/VALGET support (nanosecond precision)
- **NMEA Protocol**: Legacy and multi-vendor GPS devices (millisecond precision)

**Supported Device Types:**

:UBX-Capable Devices:
  - u-blox 9 series (ZED-F9P, NEO-M9N, etc.)
  - Modern u-blox modules with current UBX protocol

:NMEA-Only Devices:
  - MediaTek GPS modules
  - Garmin GPS modules
  - SiRF GPS modules
  - Legacy GPS devices
  - Any NMEA 0183 compatible device

**Key Features:**

- Unified timing API with best available precision
- Position and fix quality data
- Configurable PPS output
- Multi-constellation support (GPS, GLONASS, Galileo, BeiDou)
- Automatic protocol detection and configuration

API Usage
---------

**Basic Usage Pattern:**

.. code-block:: c

   #include "no_os_gnss.h"
   #include "nmea_ubx_gnss.h"

   // 1. Configure platform-specific parameters
   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .uart_init = &uart_init,
           .device_type = GNSS_DEVICE_UBX_CAPABLE,
           .baud_rate = 38400
       }
   };

   // 2. Configure API-level parameters
   struct no_os_gnss_init_param api_init = {
       .device_id = 0,
       .pps_config = { .pps_enable = true, .frequency = 1 },
       .platform_ops = &nmea_ubx_gnss_ops,
       .extra = &platform_init
   };

   // 3. Initialize and use
   struct no_os_gnss_desc *gnss;
   no_os_gnss_init(&gnss, &api_init);

   struct no_os_gnss_precise_time timing;
   no_os_gnss_refresh_timing_data(gnss);
   no_os_gnss_get_unified_timing(gnss, &timing);

**Device Configuration Examples:**

UBX-Capable Device (Modern u-blox):

.. code-block:: c

   .gnss_init_param = {
       .device_type = GNSS_DEVICE_UBX_CAPABLE,
       .ubx_input_enable = ENABLE,
       .ubx_output_enable = ENABLE,
       .baud_rate = 38400
   }

NMEA GPS-Only Device (Legacy):

.. code-block:: c

   .gnss_init_param = {
       .device_type = GNSS_DEVICE_NMEA_ONLY,
       .nmea_constellation_type = NMEA_GPS_ONLY,  // $GPxxx
       .baud_rate = 9600
   }

NMEA Multi-Constellation Device (Modern):

.. code-block:: c

   .gnss_init_param = {
       .device_type = GNSS_DEVICE_NMEA_ONLY,
       .nmea_constellation_type = NMEA_GNSS_MULTI,  // $GNxxx
       .baud_rate = 9600
   }

Data Structures
---------------

**Timing Data** (``struct no_os_gnss_precise_time``):

.. code-block:: c

   struct no_os_gnss_precise_time {
       uint32_t unix_epoch;           // Unix timestamp (seconds)
       int32_t nanoseconds;           // Nanosecond fraction
       uint32_t time_accuracy;        // Time accuracy (ns)
       bool time_valid;               // Time validity
       bool time_fully_resolved;      // Full resolution
       bool date_valid;               // Date validity
       uint16_t year, month, day;     // Date components
       uint8_t hour, minute, second;  // Time components
   };

**Position Data** (``struct no_os_gnss_position_data``):

.. code-block:: c

   struct no_os_gnss_position_data {
       uint8_t fix_quality;          // 0=invalid, 1=GPS, 2=DGPS
       uint8_t satellites_used;      // Number of satellites
       float hdop;                   // Horizontal dilution
       float altitude;               // Altitude (meters)
       float latitude, longitude;    // Coordinates (degrees)
       bool fix_valid;               // Fix status
       bool position_valid;          // Position validity
   };

Platform Integration
--------------------

**Adding New GNSS Drivers:**

To add support for a new GNSS device:

1. **Create Device Driver** (``drivers/gnss-gps/new_device/``):

   .. code-block:: c

      // new_device.h - Device-specific API
      int new_device_init(struct new_device **dev, struct new_device_init_param param);
      int new_device_get_timing(struct new_device *dev, struct timing_data *timing);
      // ... other device functions

2. **Create Platform Driver** (``drivers/platform/generic/new_device_gnss.c``):

   .. code-block:: c

      // Platform adapter implementing no_os_gnss_platform_ops
      static int32_t new_device_platform_init(struct no_os_gnss_desc **desc,
                                              const struct no_os_gnss_init_param *param);
      // ... other platform ops

      const struct no_os_gnss_platform_ops new_device_gnss_ops = {
          .init = new_device_platform_init,
          // ... other ops
      };

3. **Use in Application**:

   .. code-block:: c

      .platform_ops = &new_device_gnss_ops,
      .extra = &new_device_platform_init_param

Example Projects
----------------

**eval-ublox-gnss**
~~~~~~~~~~~~~~~~~~~

Complete example project demonstrating:

- UBX-capable and NMEA-only device support
- Real-time timing synchronization
- Position monitoring with fix quality
- PPS output configuration
- Error handling and validation

**Location:** ``projects/eval-ublox-gnss/``

**Build:** ``make PLATFORM=maxim TARGET=max32690``

File Organization
-----------------

.. code-block:: text

   drivers/gnss-gps/
   ├── README.rst                    # This file
   └── nmea_ubx/                     # NMEA/UBX device driver
       ├── nmea_ubx.h                # Driver interface
       └── nmea_ubx.c                # Driver implementation

   drivers/platform/generic/
   ├── nmea_ubx_gnss.h               # Platform driver interface
   └── nmea_ubx_gnss.c               # Platform driver implementation

   drivers/api/
   └── no_os_gnss.c                  # Generic API implementation

   include/
   └── no_os_gnss.h                  # Public API header

Limitations and Notes
---------------------

**UBX Protocol Support:**

- Supports modern u-blox devices with CFG-VALSET/VALGET commands
- Legacy u-blox devices (8 series and earlier) should use NMEA mode
- UBX protocol provides nanosecond precision timing

**NMEA Protocol Support:**

- Universal compatibility with NMEA 0183 devices
- Millisecond precision timing
- Position data refreshed at 10:1 ratio (optimization for timing apps)

**Performance Considerations:**

- UBX devices: Sub-microsecond timing accuracy
- NMEA devices: ~1ms timing accuracy
- Position updates: Configurable rate, typically 1Hz
- Memory footprint: Minimal, suitable for embedded systems

Contributing
------------

When adding new GNSS drivers:

1. Follow no-OS framework architecture patterns
2. Implement complete platform operations interface
3. Provide comprehensive testing with real hardware
4. Document device-specific limitations and capabilities
5. Include example usage in driver documentation

For questions or contributions, refer to the main no-OS framework documentation.