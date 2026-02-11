Usage Examples and Integration Guide
====================================

Overview
--------

This guide provides practical examples and integration patterns for the no-OS GNSS driver. Each example includes complete, compilable code with detailed explanations.

Table of Contents
-----------------

1. `Basic Integration Examples`_
2. `Advanced Use Cases`_
3. `Integration Patterns`_
4. `Platform-Specific Examples`_
5. `Troubleshooting Common Issues`_

---

Basic Integration Examples
--------------------------

Example 1: Simple Timing Synchronization
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Use Case:** Basic RTC synchronization using GNSS timing data

.. code-block:: c

   #include "no_os_gnss.h"
   #include "no_os_delay.h"
   #include "no_os_print_log.h"

   // Global device descriptor
   struct no_os_gnss_desc *gnss_desc;

   int basic_timing_sync_example(void)
   {
       struct gnss_precise_time precise_time;
       uint32_t fractional_seconds;
       bool timing_valid;
       int32_t ret;

       // Initialize GNSS device with UBX configuration
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .baud_rate = 38400
           },
           .platform_ops = &ublox_gnss_ops
       };

       // Initialize device
       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("GNSS initialization failed: %d\n", ret);
           return ret;
       }

       pr_info("GNSS device initialized successfully\n");
       pr_info("Waiting for GPS fix and valid timing...\n");

       // Main synchronization loop
       while (1) {
           // Refresh timing data from device
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               pr_warning("Failed to refresh timing data: %d\n", ret);
               no_os_mdelay(5000);
               continue;
           }

           // Check if timing is valid
           ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
           if (ret || !timing_valid) {
               pr_info("Waiting for valid GPS timing...\n");
               no_os_mdelay(5000);
               continue;
           }

           // Get comprehensive timing information
           ret = no_os_gnss_get_unified_timing(gnss_desc, &precise_time);
           if (ret) {
               pr_err("Failed to get unified timing: %d\n", ret);
               no_os_mdelay(5000);
               continue;
           }

           // Get Unix epoch with sub-second precision
           uint32_t unix_epoch = no_os_gnss_get_unix_epoch_unified(gnss_desc,
                                                                   &fractional_seconds);

           // Display timing information
           pr_info("=== GPS Timing Data ===\n");
           pr_info("Date: %04d-%02d-%02d %02d:%02d:%02d\n",
                  precise_time.year, precise_time.month, precise_time.day,
                  precise_time.hour, precise_time.minute, precise_time.second);
           pr_info("Unix Epoch: %lu.%06lu\n", unix_epoch, fractional_seconds);
           pr_info("Nanoseconds: %ld\n", precise_time.nanoseconds);
           pr_info("Time Accuracy: %lu ns\n", precise_time.time_accuracy);

           // Synchronize RTC (example implementation)
           sync_rtc_from_gps_time(&precise_time);

           // Wait before next update
           no_os_mdelay(10000); // 10 second interval
       }

       // Cleanup
       no_os_gnss_remove(gnss_desc);
       return 0;
   }

   // Example RTC synchronization function
   void sync_rtc_from_gps_time(struct gnss_precise_time *gps_time)
   {
       // Implementation depends on your RTC hardware
       pr_info("Synchronizing RTC with GPS time...\n");

       // Example: Set RTC date and time
       // rtc_set_time(gps_time->year, gps_time->month, gps_time->day,
       //              gps_time->hour, gps_time->minute, gps_time->second);

       pr_info("RTC synchronized successfully\n");
   }

Example 2: NMEA Device with Position Monitoring
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Use Case:** GPS tracking with position and fix quality monitoring

.. code-block:: c

   #include "no_os_gnss.h"
   #include "no_os_delay.h"

   int position_monitoring_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct gnss_precise_time timing;
       struct gnss_nmea_position position;
       bool timing_valid;
       int32_t ret;

       // Initialize NMEA device (e.g., MediaTek GPS module)
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = false,  // No PPS for NMEA device
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_NMEA_ONLY,
               .nmea_constellation_type = NMEA_GPS_ONLY,
               .baud_rate = 9600
           },
           .platform_ops = &ublox_gnss_ops
       };

       // Initialize GNSS device
       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("Failed to initialize NMEA GPS device: %d\n", ret);
           return ret;
       }

       pr_info("NMEA GPS device initialized\n");
       pr_info("Device Type: GPS-only NMEA ($GPRMC/$GPGGA)\n");

       // Tracking loop
       while (1) {
           // Refresh data from GPS
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               pr_warning("Data refresh failed: %d\n", ret);
               no_os_mdelay(5000);
               continue;
           }

           // Check timing validity
           ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
           if (ret || !timing_valid) {
               pr_info("Acquiring GPS fix...\n");
               no_os_mdelay(5000);
               continue;
           }

           // Get timing information
           ret = no_os_gnss_get_unified_timing(gnss_desc, &timing);
           if (ret) {
               pr_err("Failed to get timing: %d\n", ret);
               continue;
           }

           // Display timing (millisecond precision for NMEA)
           pr_info("=== GPS Time ===\n");
           pr_info("UTC: %04d-%02d-%02d %02d:%02d:%02d\n",
                  timing.year, timing.month, timing.day,
                  timing.hour, timing.minute, timing.second);

           // Get position data (updated every 10th cycle)
           ret = no_os_gnss_get_position_data(gnss_desc, &position);
           if (ret == 0) {
               pr_info("=== GPS Position ===\n");
               pr_info("Fix Quality: %d (0=none, 1=GPS, 2=DGPS)\n", position.fix_quality);
               pr_info("Satellites: %d\n", position.satellites_used);
               pr_info("HDOP: %.1f\n", position.hdop);
               pr_info("Position: %.6f°N, %.6f°E\n", position.latitude, position.longitude);
               pr_info("Altitude: %.1f m\n", position.altitude);
               pr_info("Fix Valid: %s\n", position.fix_valid ? "YES" : "NO");
           } else {
               pr_info("Position data not available (refreshed every 10 cycles)\n");
           }

           pr_info("========================\n\n");

           // Update interval
           no_os_mdelay(5000);
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

Example 3: Multi-Device Configuration Selection
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Use Case:** Automatic device type detection and configuration

.. code-block:: c

   #include "no_os_gnss.h"

   // Device configuration attempts in priority order
   static const struct device_config {
       enum device_type type;
       enum nmea_constellation_support nmea_type;
       uint32_t baudrate;
       const char *description;
   } device_configs[] = {
       {GNSS_DEVICE_UBX_CAPABLE, 0, 38400, "u-blox UBX (38400)"},
       {GNSS_DEVICE_UBX_CAPABLE, 0, 9600,  "u-blox UBX (9600)"},
       {GNSS_DEVICE_NMEA_ONLY, NMEA_GNSS_MULTI, 9600, "GNSS Multi-constellation"},
       {GNSS_DEVICE_NMEA_ONLY, NMEA_GPS_ONLY, 9600, "GPS-only NMEA"},
       {GNSS_DEVICE_NMEA_ONLY, NMEA_GNSS_MULTI, 4800, "GNSS Multi (4800)"},
       {GNSS_DEVICE_NMEA_ONLY, NMEA_GPS_ONLY, 4800, "GPS-only (4800)"}
   };

   int auto_detect_example(void)
   {
       struct no_os_gnss_desc *gnss_desc = NULL;
       int32_t ret;
       size_t config_count = sizeof(device_configs) / sizeof(device_configs[0]);

       pr_info("Starting automatic GNSS device detection...\n");

       // Try each configuration until one works
       for (size_t i = 0; i < config_count; i++) {
           pr_info("Trying configuration %zu: %s\n", i + 1, device_configs[i].description);

           struct no_os_gnss_init_param init_param = {
               .device_id = 0,
               .pps_config = {
                   .pps_enable = (device_configs[i].type == GNSS_DEVICE_UBX_CAPABLE),
                   .frequency = GNSS_PPS_1HZ,
                   .pulse_length = GNSS_PPS_LENGTH_100MS
               },
               .gnss_init_param = {
                   .uart_init = &uart_gnss_ip,
                   .device_type = device_configs[i].type,
                   .nmea_constellation_type = device_configs[i].nmea_type,
                   .baud_rate = device_configs[i].baudrate
               },
               .platform_ops = &ublox_gnss_ops
           };

           // Attempt initialization
           ret = no_os_gnss_init(&gnss_desc, &init_param);
           if (ret) {
               pr_info("Configuration failed: %d\n", ret);
               continue;
           }

           // Test communication by attempting data refresh
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               pr_info("Communication test failed: %d\n", ret);
               no_os_gnss_remove(gnss_desc);
               gnss_desc = NULL;
               continue;
           }

           // Success!
           pr_info("=== Device Detected Successfully ===\n");
           pr_info("Configuration: %s\n", device_configs[i].description);
           pr_info("Device Type: %s\n",
                  (device_configs[i].type == GNSS_DEVICE_UBX_CAPABLE) ?
                  "UBX-capable" : "NMEA-only");
           if (device_configs[i].type == GNSS_DEVICE_NMEA_ONLY) {
               pr_info("NMEA Type: %s\n",
                      (device_configs[i].nmea_type == NMEA_GPS_ONLY) ?
                      "GPS-only" : "Multi-constellation");
           }
           pr_info("Baudrate: %lu\n", device_configs[i].baudrate);
           break;
       }

       if (!gnss_desc) {
           pr_err("Failed to detect any compatible GNSS device\n");
           return -1;
       }

       // Use the detected device...
       // (continue with your application logic)

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

---

Advanced Use Cases
------------------

Example 4: High-Precision Network Time Server
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Use Case:** PTP (Precision Time Protocol) slave implementation

.. code-block:: c

   #include "no_os_gnss.h"
   #include "no_os_timer.h"

   // PTP synchronization structure
   struct ptp_sync_data {
       struct gnss_precise_time last_gps_time;
       uint32_t last_sync_timestamp;
       bool sync_active;
       int32_t drift_correction;
   };

   static struct ptp_sync_data ptp_data = {0};

   int precision_time_server_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct gnss_precise_time precise_time;
       bool timing_valid;
       int32_t ret;

       // Configure high-precision UBX device
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_10MS  // Short pulse for precision
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .ubx_output_enable = ENABLE,
               .nmea_output_enable = DISABLE,  // UBX only for precision
               .baud_rate = 38400
           },
           .platform_ops = &ublox_gnss_ops
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("Failed to initialize precision GNSS: %d\n", ret);
           return ret;
       }

       pr_info("High-precision time server initialized\n");
       pr_info("PPS output: 1Hz with 10ms pulse width\n");

       // Main synchronization loop
       while (1) {
           // High-frequency timing updates for precision
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               no_os_mdelay(1000);
               continue;
           }

           ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
           if (ret || !timing_valid) {
               pr_warning("GPS timing not valid, maintaining local clock\n");
               no_os_mdelay(1000);
               continue;
           }

           ret = no_os_gnss_get_unified_timing(gnss_desc, &precise_time);
           if (ret) {
               no_os_mdelay(1000);
               continue;
           }

           // Update PTP synchronization
           update_ptp_synchronization(&precise_time);

           // High-precision logging
           if (precise_time.time_accuracy < 50) {  // < 50ns accuracy
               pr_debug("High precision: %lu.%09ld ±%luns\n",
                       precise_time.unix_epoch,
                       precise_time.nanoseconds,
                       precise_time.time_accuracy);
           }

           // Fast update for time server applications
           no_os_mdelay(100);  // 10Hz update rate
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

   void update_ptp_synchronization(struct gnss_precise_time *gps_time)
   {
       uint32_t current_timestamp = get_local_timestamp();

       if (ptp_data.sync_active) {
           // Calculate drift and apply correction
           int32_t time_diff = current_timestamp - ptp_data.last_sync_timestamp;
           int32_t gps_diff = (gps_time->unix_epoch * 1000000) +
                             (gps_time->nanoseconds / 1000) -
                             (ptp_data.last_gps_time.unix_epoch * 1000000) -
                             (ptp_data.last_gps_time.nanoseconds / 1000);

           ptp_data.drift_correction = gps_diff - time_diff;

           // Apply drift correction to local clock
           adjust_local_clock(ptp_data.drift_correction);
       }

       // Update synchronization data
       ptp_data.last_gps_time = *gps_time;
       ptp_data.last_sync_timestamp = current_timestamp;
       ptp_data.sync_active = true;
   }

Example 5: Mobile Asset Tracking System
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Use Case:** Vehicle/asset tracking with position logging

.. code-block:: c

   #include "no_os_gnss.h"
   #include "no_os_eeprom.h"  // For position logging

   #define MAX_POSITION_LOG_ENTRIES    1000
   #define TRACKING_UPDATE_INTERVAL    30000  // 30 seconds

   struct position_log_entry {
       uint32_t timestamp;
       float latitude;
       float longitude;
       float altitude;
       uint8_t fix_quality;
       uint8_t satellites;
   };

   static struct position_log_entry position_log[MAX_POSITION_LOG_ENTRIES];
   static uint16_t log_index = 0;

   int asset_tracking_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct gnss_precise_time timing;
       struct gnss_nmea_position position;
       bool timing_valid;
       int32_t ret;

       // Configure for NMEA device with position tracking
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = false,  // No PPS needed for tracking
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .device_type = GNSS_DEVICE_NMEA_ONLY,
               .nmea_constellation_type = NMEA_GNSS_MULTI,  // Multi-constellation for better accuracy
               .baud_rate = 9600
           },
           .platform_ops = &ublox_gnss_ops
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("Asset tracking GPS initialization failed: %d\n", ret);
           return ret;
       }

       pr_info("Asset tracking system initialized\n");
       pr_info("Update interval: %d seconds\n", TRACKING_UPDATE_INTERVAL / 1000);

       while (1) {
           // Get current position and timing
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           if (ret) {
               pr_warning("GPS data refresh failed: %d\n", ret);
               no_os_mdelay(5000);
               continue;
           }

           ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
           if (ret || !timing_valid) {
               pr_info("Waiting for GPS fix...\n");
               no_os_mdelay(5000);
               continue;
           }

           ret = no_os_gnss_get_unified_timing(gnss_desc, &timing);
           ret |= no_os_gnss_get_position_data(gnss_desc, &position);

           if (ret == 0 && position.fix_valid) {
               // Log position data
               log_position_data(&timing, &position);

               // Display current status
               pr_info("=== Asset Tracking Update ===\n");
               pr_info("Time: %04d-%02d-%02d %02d:%02d:%02d UTC\n",
                      timing.year, timing.month, timing.day,
                      timing.hour, timing.minute, timing.second);
               pr_info("Position: %.6f°N, %.6f°E\n", position.latitude, position.longitude);
               pr_info("Altitude: %.1f m\n", position.altitude);
               pr_info("GPS Quality: %d, Satellites: %d\n",
                      position.fix_quality, position.satellites_used);
               pr_info("HDOP: %.1f\n", position.hdop);
               pr_info("Log entries: %d/%d\n", log_index, MAX_POSITION_LOG_ENTRIES);

               // Check for geofence violations or other conditions
               check_tracking_conditions(&position);

           } else {
               pr_warning("Position data not available or invalid\n");
           }

           // Wait for next tracking interval
           no_os_mdelay(TRACKING_UPDATE_INTERVAL);
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

   void log_position_data(struct gnss_precise_time *timing,
                         struct gnss_nmea_position *position)
   {
       if (log_index >= MAX_POSITION_LOG_ENTRIES) {
           // Wrap around or save to persistent storage
           pr_warning("Position log full, wrapping around\n");
           save_position_log_to_storage();
           log_index = 0;
       }

       // Create log entry
       struct position_log_entry *entry = &position_log[log_index];
       entry->timestamp = timing->unix_epoch;
       entry->latitude = position->latitude;
       entry->longitude = position->longitude;
       entry->altitude = position->altitude;
       entry->fix_quality = position->fix_quality;
       entry->satellites = position->satellites_used;

       log_index++;
   }

   void check_tracking_conditions(struct gnss_nmea_position *position)
   {
       // Example: Simple geofence check
       const float geofence_lat = 46.7723;  // Example coordinates
       const float geofence_lon = 23.6230;
       const float geofence_radius = 0.01;  // ~1km radius

       float lat_diff = position->latitude - geofence_lat;
       float lon_diff = position->longitude - geofence_lon;
       float distance = sqrt(lat_diff * lat_diff + lon_diff * lon_diff);

       if (distance > geofence_radius) {
           pr_warning("GEOFENCE VIOLATION: Asset outside allowed area!\n");
           pr_warning("Distance from center: %.3f degrees\n", distance);
           // Trigger alert or notification
           trigger_geofence_alert(position);
       }

       // Check for low satellite count
       if (position->satellites_used < 4) {
           pr_warning("LOW SATELLITE COUNT: %d satellites\n", position->satellites_used);
       }

       // Check for poor accuracy
       if (position->hdop > 3.0) {
           pr_warning("POOR GPS ACCURACY: HDOP = %.1f\n", position->hdop);
       }
   }

   void trigger_geofence_alert(struct gnss_nmea_position *position)
   {
       // Implementation depends on your alert system
       pr_info("Triggering geofence alert...\n");
       // Could send notification, log to file, activate LED, etc.
   }

   void save_position_log_to_storage(void)
   {
       // Save position log to EEPROM, SD card, or other persistent storage
       pr_info("Saving position log to persistent storage...\n");

       // Example: Save to EEPROM
       // eeprom_write_data(POSITION_LOG_ADDRESS, (uint8_t*)position_log,
       //                  sizeof(position_log));

       pr_info("Position log saved successfully\n");
   }

---

Integration Patterns
--------------------

Pattern 1: Interrupt-Driven Operation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "no_os_irq.h"

   static volatile bool gnss_data_ready = false;

   // UART receive interrupt handler
   void gnss_uart_irq_handler(void *context)
   {
       // Set flag indicating new data available
       gnss_data_ready = true;
   }

   int interrupt_driven_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       int32_t ret;

       // Initialize with interrupt-driven UART
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip,
               .irq_ctrl = &gnss_nvic_desc,
               .irq_cb = {
                   .callback = gnss_uart_irq_handler,
                   .ctx = NULL,
                   .event = NO_OS_EVT_UART_RX_COMPLETE
               },
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .baud_rate = 38400
           },
           .platform_ops = &ublox_gnss_ops
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) return ret;

       // Main application loop
       while (1) {
           // Wait for interrupt-driven data
           if (gnss_data_ready) {
               gnss_data_ready = false;

               // Process GNSS data
               ret = no_os_gnss_refresh_timing_data(gnss_desc);
               if (ret == 0) {
                   process_gnss_timing(gnss_desc);
               }
           }

           // Other application tasks
           perform_other_tasks();

           // Small delay to prevent busy waiting
           no_os_mdelay(10);
       }

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

Pattern 2: State Machine Integration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   typedef enum {
       GNSS_STATE_INIT,
       GNSS_STATE_ACQUIRING,
       GNSS_STATE_LOCKED,
       GNSS_STATE_SYNCHRONIZED,
       GNSS_STATE_ERROR
   } gnss_state_t;

   static gnss_state_t current_state = GNSS_STATE_INIT;

   int state_machine_example(void)
   {
       struct no_os_gnss_desc *gnss_desc = NULL;
       int32_t ret;

       while (1) {
           switch (current_state) {
           case GNSS_STATE_INIT:
               ret = initialize_gnss_device(&gnss_desc);
               if (ret == 0) {
                   current_state = GNSS_STATE_ACQUIRING;
                   pr_info("GNSS initialized, acquiring fix...\n");
               } else {
                   current_state = GNSS_STATE_ERROR;
               }
               break;

           case GNSS_STATE_ACQUIRING:
               ret = check_gnss_fix(gnss_desc);
               if (ret == 0) {
                   current_state = GNSS_STATE_LOCKED;
                   pr_info("GPS fix acquired, timing locked\n");
               } else if (ret < -10) {  // Critical error
                   current_state = GNSS_STATE_ERROR;
               }
               break;

           case GNSS_STATE_LOCKED:
               ret = monitor_timing_quality(gnss_desc);
               if (ret == 0) {
                   current_state = GNSS_STATE_SYNCHRONIZED;
               } else if (ret == -ENODATA) {
                   current_state = GNSS_STATE_ACQUIRING;
                   pr_warning("Lost GPS fix, re-acquiring...\n");
               }
               break;

           case GNSS_STATE_SYNCHRONIZED:
               ret = perform_synchronized_operations(gnss_desc);
               if (ret != 0) {
                   current_state = GNSS_STATE_LOCKED;
                   pr_warning("Synchronization quality degraded\n");
               }
               break;

           case GNSS_STATE_ERROR:
               handle_gnss_error();
               current_state = GNSS_STATE_INIT;  // Retry initialization
               break;
           }

           no_os_mdelay(1000);
       }

       if (gnss_desc) {
           no_os_gnss_remove(gnss_desc);
       }
       return 0;
   }

---

Platform-Specific Examples
---------------------------

Maxim Platform (MAX32690)
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   #include "maxim_uart.h"
   #include "maxim_gpio.h"
   #include "maxim_irq.h"

   // Platform-specific initialization for Maxim MAX32690
   struct no_os_uart_init_param uart_gnss_ip_maxim = {
       .device_id = 1,           // UART1
       .baud_rate = 38400,
       .size = NO_OS_UART_CS_8,
       .parity = NO_OS_UART_PAR_NO,
       .stop = NO_OS_UART_STOP_1_BIT,
       .platform_ops = &max_uart_ops
   };

   struct no_os_gpio_init_param gpio_gnss_reset_ip_maxim = {
       .port = 2,                // GPIO2
       .number = 0,              // Pin 0
       .mode = NO_OS_GPIO_OUT,
       .platform_ops = &max_gpio_ops
   };

   int maxim_platform_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       struct no_os_irq_ctrl_desc *nvic_desc;
       int32_t ret;

       // Initialize IRQ controller
       struct no_os_irq_init_param nvic_ip = {
           .platform_ops = &max_irq_ops,
       };

       ret = no_os_irq_ctrl_init(&nvic_desc, &nvic_ip);
       if (ret) return ret;

       // Configure GNSS with Maxim platform specifics
       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip_maxim,
               .gpio_reset = &gpio_gnss_reset_ip_maxim,
               .irq_ctrl = nvic_desc,
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .baud_rate = 38400
           },
           .platform_ops = &ublox_gnss_ops
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("Maxim GNSS initialization failed: %d\n", ret);
           goto error;
       }

       pr_info("GNSS initialized on Maxim MAX32690 platform\n");

       // Platform-specific application logic
       while (1) {
           ret = no_os_gnss_refresh_timing_data(gnss_desc);
           // ... continue with timing operations
           no_os_mdelay(5000);
       }

       no_os_gnss_remove(gnss_desc);

   error:
       no_os_irq_ctrl_remove(nvic_desc);
       return ret;
   }

STM32 Platform
~~~~~~~~~~~~~~

.. code-block:: c

   #include "stm32_uart.h"
   #include "stm32_gpio.h"

   // STM32-specific UART configuration
   struct no_os_uart_init_param uart_gnss_ip_stm32 = {
       .device_id = 2,           // UART2
       .baud_rate = 38400,
       .size = NO_OS_UART_CS_8,
       .parity = NO_OS_UART_PAR_NO,
       .stop = NO_OS_UART_STOP_1_BIT,
       .platform_ops = &stm32_uart_ops
   };

   struct no_os_gpio_init_param gpio_gnss_reset_ip_stm32 = {
       .port = 1,                // GPIOB
       .number = 5,              // PB5
       .mode = NO_OS_GPIO_OUT,
       .platform_ops = &stm32_gpio_ops
   };

   int stm32_platform_example(void)
   {
       struct no_os_gnss_desc *gnss_desc;
       int32_t ret;

       struct no_os_gnss_init_param init_param = {
           .device_id = 0,
           .pps_config = {
               .pps_enable = true,
               .frequency = GNSS_PPS_1HZ,
               .pulse_length = GNSS_PPS_LENGTH_100MS
           },
           .gnss_init_param = {
               .uart_init = &uart_gnss_ip_stm32,
               .gpio_reset = &gpio_gnss_reset_ip_stm32,
               .device_type = GNSS_DEVICE_UBX_CAPABLE,
               .baud_rate = 38400
           },
           .platform_ops = &ublox_gnss_ops
       };

       ret = no_os_gnss_init(&gnss_desc, &init_param);
       if (ret) {
           pr_err("STM32 GNSS initialization failed: %d\n", ret);
           return ret;
       }

       pr_info("GNSS initialized on STM32 platform\n");

       // STM32-specific implementation
       // ... (similar to other examples)

       no_os_gnss_remove(gnss_desc);
       return 0;
   }

---

Troubleshooting Common Issues
------------------------------

Issue 1: Initialization Failures
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // Robust initialization with multiple attempts and diagnostics
   int robust_gnss_init(struct no_os_gnss_desc **gnss_desc)
   {
       const uint32_t baudrates[] = {38400, 9600, 19200, 4800};
       const size_t baudrate_count = sizeof(baudrates) / sizeof(baudrates[0]);
       int32_t ret;

       for (size_t i = 0; i < baudrate_count; i++) {
           pr_info("Attempting GNSS init with baudrate: %lu\n", baudrates[i]);

           struct no_os_gnss_init_param init_param = {
               .device_id = 0,
               .pps_config = {
                   .pps_enable = true,
                   .frequency = GNSS_PPS_1HZ,
                   .pulse_length = GNSS_PPS_LENGTH_100MS
               },
               .gnss_init_param = {
                   .uart_init = &uart_gnss_ip,
                   .device_type = GNSS_DEVICE_UBX_CAPABLE,
                   .baud_rate = baudrates[i]
               },
               .platform_ops = &ublox_gnss_ops
           };

           ret = no_os_gnss_init(gnss_desc, &init_param);
           if (ret == 0) {
               // Test communication
               ret = no_os_gnss_refresh_timing_data(*gnss_desc);
               if (ret == 0) {
                   pr_info("GNSS initialized successfully at %lu baud\n", baudrates[i]);
                   return 0;
               } else {
                   pr_warning("Communication test failed: %d\n", ret);
                   no_os_gnss_remove(*gnss_desc);
                   *gnss_desc = NULL;
               }
           }
       }

       pr_err("Failed to initialize GNSS with any configuration\n");
       return -EIO;
   }

Issue 2: Timing Validation Problems
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // Enhanced timing validation with detailed diagnostics
   int validate_gnss_timing(struct no_os_gnss_desc *gnss_desc)
   {
       struct gnss_precise_time timing;
       bool timing_valid;
       int32_t ret;
       static uint32_t invalid_count = 0;

       ret = no_os_gnss_refresh_timing_data(gnss_desc);
       if (ret) {
           pr_err("Data refresh failed: %d\n", ret);
           return ret;
       }

       ret = no_os_gnss_is_timing_valid(gnss_desc, &timing_valid);
       if (ret) {
           pr_err("Timing validity check failed: %d\n", ret);
           return ret;
       }

       if (!timing_valid) {
           invalid_count++;
           pr_warning("Timing invalid (count: %lu)\n", invalid_count);

           // Get timing data anyway for diagnostics
           ret = no_os_gnss_get_unified_timing(gnss_desc, &timing);
           if (ret == 0) {
               pr_debug("Diagnostic - Time: %04d-%02d-%02d %02d:%02d:%02d\n",
                       timing.year, timing.month, timing.day,
                       timing.hour, timing.minute, timing.second);
               pr_debug("Diagnostic - Validity flags: time=%s, date=%s, resolved=%s\n",
                       timing.time_valid ? "OK" : "BAD",
                       timing.date_valid ? "OK" : "BAD",
                       timing.time_fully_resolved ? "OK" : "BAD");
           }

           return -ENODATA;
       }

       // Reset counter on valid timing
       if (invalid_count > 0) {
           pr_info("Timing valid again after %lu invalid readings\n", invalid_count);
           invalid_count = 0;
       }

       return 0;
   }

Issue 3: Communication Recovery
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: c

   // Communication recovery with device reset
   int recover_gnss_communication(struct no_os_gnss_desc *gnss_desc)
   {
       int32_t ret;

       pr_info("Attempting GNSS communication recovery...\n");

       // Try software reset for UBX devices
       if (gnss_desc->gnss_device->device_type == GNSS_DEVICE_UBX_CAPABLE) {
           pr_info("Attempting UBX software reset...\n");
           ret = gnss_ubx_sw_reset(gnss_desc->gnss_device);
           if (ret == 0) {
               no_os_mdelay(5000);  // Wait for reset completion

               // Test communication
               ret = no_os_gnss_refresh_timing_data(gnss_desc);
               if (ret == 0) {
                   pr_info("Communication recovered via software reset\n");
                   return 0;
               }
           }
       }

       // Try hardware reset if available
       if (gnss_desc->gnss_device->gpio_reset) {
           pr_info("Attempting hardware reset...\n");
           ret = gnss_hw_reset(gnss_desc->gnss_device);
           if (ret == 0) {
               no_os_mdelay(10000);  // Wait for reset and initialization

               // Test communication
               ret = no_os_gnss_refresh_timing_data(gnss_desc);
               if (ret == 0) {
                   pr_info("Communication recovered via hardware reset\n");
                   return 0;
               }
           }
       }

       pr_err("Communication recovery failed\n");
       return -EIO;
   }

---

This comprehensive usage guide provides practical examples for integrating the no-OS GNSS driver into various applications, from basic timing synchronization to advanced precision time servers and asset tracking systems. Each example includes complete, compilable code with detailed explanations and error handling.