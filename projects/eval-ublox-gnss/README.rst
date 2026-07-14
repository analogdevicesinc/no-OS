no-OS GNSS API - Unified GPS/GNSS Driver
========================================

no-OS compatible GNSS driver providing unified support for both **UBX-capable devices** (modern u-blox) and **NMEA-only devices** (MediaTek, Garmin, SiRF, etc.) with timing synchronization, position data, and PPS output.

**Key Features**
--------------------

- **Unified API**: Single interface for UBX and NMEA protocols
- **Timing Synchronization**: Precise time data for RTC/PTP applications
- **Position Data**: GPS fix quality, coordinates, satellite information
- **PPS Output**: Configurable pulse-per-second timing signals
- **Multi-Protocol**: UBX (nanosecond) and NMEA (millisecond) precision

**Supported Devices**
------------------------

**UBX-Capable Devices** (Nanosecond Precision)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- u-blox 9 series (ZED-F9P, NEO-M9N, etc.) with CFG-VALSET/VALGET support
- Modern u-blox devices supporting the current UBX configuration protocol
- **Note**: Older u-blox devices (8 series and earlier) with legacy binary UBX protocol are not supported in UBX mode, but can use NMEA mode

**NMEA-Only Devices** (Millisecond Precision)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- MediaTek GPS modules
- Garmin GPS modules
- SiRF GPS modules
- Legacy GPS devices with NMEA output
- Any GPS device with standard NMEA 0183 protocol

**Quick Start**
------------------

**1. Include Headers**
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "no_os_gnss.h"
   #include "nmea_ubx_gnss.h"  // Platform driver for NMEA/UBX devices

**2. Configure Device**
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // Platform-specific parameters
   struct nmea_ubx_gnss_init_param platform_init = {
       .gnss_init_param = {
           .uart_init = &uart_gnss_ip,
           .gpio_reset = gpio_gnss_reset_desc,
           .device_type = GNSS_DEVICE_UBX_CAPABLE,  // or GNSS_DEVICE_NMEA_ONLY
           .nmea_constellation_type = NMEA_GPS_ONLY,  // for NMEA devices
           .ubx_input_enable = ENABLE,
           .ubx_output_enable = ENABLE,
           .baud_rate = 38400
       }
   };

   // API-level initialization
   struct no_os_gnss_init_param gnss_init_param = {
       .device_id = 0,
       .pps_config = {
           .pps_enable = true,
           .frequency = GNSS_PPS_1HZ,
           .pulse_length = GNSS_PPS_LENGTH_100MS
       },
       .platform_ops = &nmea_ubx_gnss_ops,
       .extra = &platform_init
   };

**3. Initialize and Use**
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   struct no_os_gnss_desc *gnss_desc;
   struct no_os_gnss_precise_time timing;
   struct no_os_gnss_position_data position;
   uint32_t unix_epoch, fractional_seconds;
   bool timing_valid;

   // Initialize
   ret = no_os_gnss_init(&gnss_desc, &gnss_init_param);

   // Refresh timing data
   ret = no_os_gnss_refresh_timing_data(gnss_desc);

   // Check if timing is valid
   ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);

   // Get precise timing
   ret = no_os_gnss_get_unified_timing(gnss_desc, &timing);
   ret = no_os_gnss_get_unix_epoch_unified(gnss_desc, &unix_epoch, &fractional_seconds);

   // Get position data
   ret = no_os_gnss_get_position_data(gnss_desc, &position);

   // Cleanup
   no_os_gnss_remove(gnss_desc);

**Core API Functions**
-------------------------

**Initialization**
~~~~~~~~~~~~~~~~~~

- ``no_os_gnss_init()`` - Initialize GNSS device
- ``no_os_gnss_remove()`` - Free resources

**Timing Functions**
~~~~~~~~~~~~~~~~~~~~

- ``no_os_gnss_refresh_timing_data()`` - Update timing data from device
- ``no_os_gnss_is_timing_valid()`` - Check if timing data is valid
- ``no_os_gnss_get_unified_timing()`` - Get comprehensive timing info
- ``no_os_gnss_get_unix_epoch_unified()`` - Get Unix timestamp with precision

**Position Functions**
~~~~~~~~~~~~~~~~~~~~~~

- ``no_os_gnss_get_position_data()`` - Get GPS fix quality and coordinates

**Device Configuration Examples**
-------------------------------------

**UBX-Capable Device** (Modern u-blox modules)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   .device_type = GNSS_DEVICE_UBX_CAPABLE,
   .ubx_input_enable = ENABLE,
   .ubx_output_enable = ENABLE,
   .nmea_output_enable = DISABLE,
   .baud_rate = 38400

**NMEA GPS-Only Device** (Legacy GPS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   .device_type = GNSS_DEVICE_NMEA_ONLY,
   .nmea_constellation_type = NMEA_GPS_ONLY,  // $GPRMC/$GPGGA
   .baud_rate = 9600

**NMEA Multi-Constellation Device** (Modern GNSS)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   .device_type = GNSS_DEVICE_NMEA_ONLY,
   .nmea_constellation_type = NMEA_GNSS_MULTI,  // $GNRMC/$GNGGA
   .baud_rate = 9600

**Data Structures**
----------------------

**Precise Timing** (``struct no_os_gnss_precise_time``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   struct no_os_gnss_precise_time {
       uint32_t unix_epoch;           // Unix timestamp (seconds)
       int32_t nanoseconds;           // Nanosecond fraction
       uint32_t time_accuracy;        // Time accuracy (ns)
       bool time_valid;               // Time validity flag
       bool time_fully_resolved;      // Full resolution flag
       bool date_valid;               // Date validity flag
       uint16_t year, month, day;     // Date components
       uint8_t hour, minute, second;  // Time components
   };

**Position Data** (``struct no_os_gnss_position_data``)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

**Hardware Configuration**
-----------------------------

**UART Configuration**
~~~~~~~~~~~~~~~~~~~~~~

- **Console**: UART0 at 115200 baud (debug output)
- **GNSS**: UART1 at configurable baud rate (device communication)

**GPIO Configuration**
~~~~~~~~~~~~~~~~~~~~~~

- **Reset GPIO**: Optional hardware reset control
- **Status LED**: Optional status indicator

**PPS Configuration**
~~~~~~~~~~~~~~~~~~~~~

- **Frequency**: 1Hz (configurable)
- **Pulse Length**: 100ms (configurable)
- **Polarity**: Rising edge (standard)

**Advanced Features**
------------------------

**Multi-Rate Data Refresh**
~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **GPRMC**: Every refresh cycle (timing data)
- **GPGGA**: Every 10th cycle (position data)

**Error Handling**
~~~~~~~~~~~~~~~~~~

- Comprehensive error codes
- Non-blocking operations
- Fallback mechanisms for device limitations

**Building**
---------------

**Maxim Build Command**
~~~~~~~~~~~~~~~~~~~~~~~

For toolchain setup and prerequisites, see the
`Maxim CMake build guide <https://analogdevicesinc.github.io/no-OS/build_guides/build_maxim_cmake.html>`__.

Available variants: ``basic``.
Available boards: ``ad-apard32690-sl``.
Replace ``--variant`` / ``--board`` accordingly.

.. code-block:: bash

   export MAXIM_LIBRARIES=</path/to/MaximSDK/Libraries>
   # Windows (PowerShell): $env:MAXIM_LIBRARIES = "C:\MaximSDK\Libraries"

   cd no-OS

   # build the project (basic example on the ad-apard32690-sl board)
   python tools/scripts/no_os_build.py build \
      --project eval-ublox-gnss --variant basic --board ad-apard32690-sl

   # build and flash (requires a connected debug probe)
   python tools/scripts/no_os_build.py build \
      --project eval-ublox-gnss --variant basic --board ad-apard32690-sl \
      --probe openocd --flash

**Source Files**
~~~~~~~~~~~~~~~~

::

   no-OS/
   ├── drivers/api/
   │   └── no_os_gnss.c                    # Generic API implementation
   ├── drivers/platform/generic/
   │   ├── nmea_ubx_gnss.h                 # Platform driver header
   │   └── nmea_ubx_gnss.c                 # Platform driver implementation
   ├── drivers/gnss-gps/nmea_ubx/
   │   ├── nmea_ubx.h                      # Device driver header
   │   └── nmea_ubx.c                      # Device driver implementation
   ├── include/
   │   └── no_os_gnss.h                    # Public API header
   └── projects/eval-ublox-gnss/
       ├── CMakeLists.txt                  # Build configuration
       ├── Kconfig                         # Build options
       ├── basic.conf                      # Variant defconfig
       ├── boards/                         # Per-board overlays
       └── src/
           ├── main.c                      # Example application
           ├── interrupt/                  # Interrupt handling
           └── platform/                   # Platform utilities

**Example Output**
---------------------

**UBX Device Output**
~~~~~~~~~~~~~~~~~~~~~

::

   Device Type: UBX-capable (nanosecond precision)
   Date/Time: 2025-12-09 14:35:42
   Unix Epoch: 1765123542.123456
   Nanoseconds: 123456789
   Time Accuracy: 25 ns
   Flags: Valid=YES, Resolved=YES, Date Valid=YES

**NMEA Device Output**
~~~~~~~~~~~~~~~~~~~~~~

::

   Device Type: NMEA-only (millisecond precision)
   Date/Time: 2025-04-21 10:03:34
   Unix Epoch: 1745143414.000000
   GPS Fix: Quality=1, Satellites=11, HDOP=1.0
   Position: Lat=46.772339°, Lon=23.623056°, Alt=388.5m
   Fix Status: YES, Position Valid: YES

**Known Limitations**
------------------------

**UBX Protocol Compatibility**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **Supported**: Modern u-blox devices with CFG-VALSET/VALGET commands
- **Not Supported**: Legacy u-blox devices with older binary UBX protocol
- **Workaround**: Use NMEA mode for older u-blox devices

**NMEA Position Update Rate**
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **GPGGA data**: Refreshed every 10 cycles (10:1 ratio with timing data)
- **Reason**: Optimization for timing-focused applications

**Use Cases**
-----------------

**Perfect For:**
~~~~~~~~~~~~~~~~

- **Timing Synchronization**: RTC calibration, PTP slave applications
- **Position Monitoring**: Asset tracking with fix quality indicators
- **Multi-Device Support**: Projects requiring different GPS hardware
- **Legacy Hardware**: Existing systems with older GPS modules

**Applications:**
~~~~~~~~~~~~~~~~~

- Industrial timing systems
- Network time servers
- Asset tracking devices
- Measurement equipment
- Test and measurement systems