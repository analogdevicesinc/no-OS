ADE9153A no-OS Driver
=====================

Supported Devices
-----------------

- :adi:`ADE9153A``

Overview
--------

The ADE9153A is a high precision energy metering IC from Analog Devices
designed for single-phase applications. This device offers active,
reactive, and apparent power calculations, effective current and voltage
RMS measurements, and power factor computation. Featuring
autocalibration, it ensures accurate current and voltage measurements
without external calibration sources. The IC includes SPI and UART
communication interfaces, and provides multiple interrupts for
monitoring power values, data, and temperature. Configurable gain and
high-pass filtering options make the ADE9153A ideal for implementation
in energy meters, suitable for modern energy monitoring and smart grid
applications.

Applications
------------

- Single-phase energy meters
- Energy and power measurement
- Street lighting
- Smart power distribution system
- Machine health

Operation Mode
--------------

+-------------------+-------------------------+--------------------------------+---------------------------+
| Mode Name         | Description             | Configuration                  | Typical Use               |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Normal Mode       | Standard operating mode | ade9153a_acalmode_e::NORMAL    | Regular energy            |
|                   | for channels A and B.   |                                | measurements.             |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Turbo Mode        | High-performance mode   | ade9153a_acalmode_e::TURBO     | Faster calibration and    |
|                   | for precise measurement |                                | operational efficiency    |
|                   | processes.              |                                | needed.                   |
+-------------------+-------------------------+--------------------------------+---------------------------+
| 50 Hz Frequency   | Configures the device   | ade9153a_selfreq_e::F_50_HZ    | Regions where the power   |
| Mode              | to operate at a 50 Hz   |                                | grid operates at 50 Hz.   |
|                   | frequency.              |                                |                           |
+-------------------+-------------------------+--------------------------------+---------------------------+
| 60 Hz Frequency   | Sets the device to      | ade9153a_selfreq_e::F_60_HZ    | Regions where 60 Hz is    |
| Mode              | function at a 60 Hz     |                                | the standard frequency.   |
|                   | frequency.              |                                |                           |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Signed            | Accumulates both        | ade9153a_accmode_e::           | When both directions of   |
| Accumulation Mode | positive and negative   | ADE9153A_SIGNED_ACC_MODE       | energy flow need to be    |
|                   | values for energy       |                                | accounted for.            |
|                   | registers and CFx       |                                |                           |
|                   | pulses.                 |                                |                           |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Absolute Value    | Accumulates the         | ade9153a_accmode_e::           | Applications requiring    |
| Accumulation Mode | absolute values of      | ADE9153A_ABSOLUTE_VAL_ACC_MODE | energy total irrespective |
|                   | power, disregarding the |                                | of direction.             |
|                   | sign.                   |                                |                           |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Positive          | Only positive energy    | ade9153a_accmode_e::           | Setups where energy       |
| Accumulation Mode | values are accumulated. | ADE9153A_POSITIVE_ACC_MODE     | billing is based solely   |
|                   |                         |                                | on consumption.           |
+-------------------+-------------------------+--------------------------------+---------------------------+
| Negative          | Only negative energy    | ade9153a_accmode_e::           | Applications focusing on  |
| Accumulation Mode | values are accumulated. | ADE9153A_NEGATIVE_ACC_MODE     | energy generation         |
|                   |                         |                                | tracking.                 |
+-------------------+-------------------------+--------------------------------+---------------------------+

Driver Functions
----------------

Initialization
~~~~~~~~~~~~~~

The ``ade9153a_init`` function initializes the ADE9153A with GPIO and
IRQ configurations, handling memory allocation and configuring
interrupts. It ensures the device is correctly reset and operational.
``ade9153a_setup`` fine-tunes device settings such as gain, frequency
mode, and interrupts, ensuring readiness for operation.

Configuration
~~~~~~~~~~~~~

The ``ade9153a_write`` function customizes device registers, enabling
configuration of energy output on CF pins, frequency settings, and
accumulation modes. Functions like ``ade9153a_cf1sel``,
``ade9153a_cf2sel``, ``ade9153a_selfreq``, and ``ade9153a_varacc``
provide extensive customization for energy monitoring and metering
applications.

Data Processing
~~~~~~~~~~~~~~~

The ``ade9153a_read`` extracts register data, ensuring accuracy in
measurements. Functions like ``ade9153a_energy_vals`` and
``ade9153a_power_vals`` calculate energy and power metrics, providing
active, reactive, and apparent power data. RMS value functions compute
current and voltage metrics for thorough data analysis.

Communication
~~~~~~~~~~~~~

Communication is managed via SPI interfaces configured by
``ade9153a_set_interface_spi`` and related functions, facilitating
reliable data transfer. This setup ensures seamless integration into
larger systems, enabling real-time data analysis and control for
metering applications.

Driver Initialization Example
-----------------------------

.. code-block:: C

   #include "ade9153a.h"
   #include "common_data.h"

   int main()
   {
       struct ade9153a_dev *ade9153a_dev;
       struct ade9153a_init_param ade9153a_ip;
       int ret;

       // Initialize UART for standard I/O
       no_os_uart_stdio(uart_desc);
       
       pr_info("\n");
       pr_info("\n");
       pr_info("ADE9153A SPI example \n");

       // Init ADE9153A struct
       ade9153a_dev = (struct ade9153a_dev *)no_os_calloc(1, sizeof(*ade9153a_dev));
       if (!ade9153a_dev)
           return -ENOMEM;

       // Initialize the device with the values stored in the initialization structure
       ret = ade9153a_init(&ade9153a_dev, ade9153a_ip);
       if (ret)
           goto free_dev;

       // Burst mode disable
       ade9153a_dev->burst_en = 0;

       no_os_mdelay(RESET_TIME);

       ret = ade9153a_setup(ade9153a_dev, ade9153a_ip);
       if (ret)
           goto free_dev;

       // Read the autocalibration values and update the registers
       ret = autocalibration_read_vals(ade9153a_dev);
       if (ret)
           goto free_dev;

       // Set the temperature sample time
       ret = ade9153a_temp_time(ade9153a_dev, ADE9153A_TEMP_TIME_SAMPLES_1024);
       if (ret)
           goto free_dev;

       while (1) {
           // Time delay between readings
           no_os_mdelay(READ_INTERVAL);
           // Read and print the ADE9153A measured values
           ret = read_measurements(ade9153a_dev);
           if (ret)
               goto free_dev;
           ret = interface_toggle_led(gpio_desc);
           if (ret)
               goto free_dev;
       }

   free_dev:
       no_os_free(ade9153a_dev);
   remove_reset:
       no_os_gpio_remove(reset_desc);
   remove_sck:
       no_os_gpio_remove(sck_desc);
   remove_ss:
       no_os_gpio_remove(ss_desc);
   remove_led:
       no_os_gpio_remove(gpio_desc);
   remove_uart:
       no_os_uart_remove(uart_desc);
   remove_irq:
       no_os_irq_ctrl_remove(ade9153a_nvic_desc);

   error:
       pr_err("ERROR\n");
       return ret;
   }


   #define ADE9153A_VLEVEL 0x002C11E8
   #define ADE9153A_RSMALL 0x03E8
   #define ADE9153A_NO_SAMPLES 0x0F9F
   #define ADE9153A_AIGAIN -(1 << 28)

   struct ade9153a_init_param {
       struct no_os_spi_init_param *spi_init;
       struct no_os_gpio_init_param *gpio_rdy;
       struct no_os_gpio_init_param *gpio_reset;
       struct no_os_spi_init_param *gpio_ss;
       struct no_os_spi_init_param *gpio_sck;
       uint8_t spi_en;
       struct no_os_irq_ctrl_desc *irq_ctrl;
       uint8_t ai_swap;
       enum ade9153a_ai_gain_e ai_pga_gain;
       enum ade9153a_hpf_crn_e hpf_crn;
       enum ade9153a_selfreq_e freq;
       uint32_t vlevel;
       uint32_t rsmall;
       uint32_t no_samples;
       uint32_t ai_gain;
       void (*drdy_callback)(void *context);
   };
