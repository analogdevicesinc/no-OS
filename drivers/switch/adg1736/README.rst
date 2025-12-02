ADG736/ADG1736 no-OS driver
===========================

Supported Devices
-----------------

- `ADG736 <https://www.analog.com/en/products/adg736.html>`_
- `ADG1736 <https://www.analog.com/en/products/adg1736.html>`_

Overview
--------

The ADG736 and ADG1736 are monolithic devices comprising two independently
selectable CMOS single pole, double throw (SPDT) switches. Each switch can
connect its input to one of two outputs (A or B) based on the control signal.

The devices operate from 1.8 V to 5.5 V single supply and feature:
- Low on resistance (2.5 Ω typical)
- Fast switching times (tON: 16 ns, tOFF: 8 ns)
- Break-before-make switching action
- TTL-/CMOS-compatible control inputs

**Key Difference:**

- **ADG736**: Control via IN1 and IN2 pins only
- **ADG1736**: Adds optional EN pin for enabling/disabling the mux logic

Applications
------------

- Audio and video switching
- USB 1.1 signal switching
- Sample-and-hold systems
- Communications systems
- Mechanical reed relay replacement

Device Configuration
--------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (GPIO for the control pins IN1 and IN2).

For **ADG1736**, you can optionally provide an EN pin to enable/disable the
mux logic. If not provided, the EN pin is assumed to be tied high externally.

For **ADG736**, the EN pin is not available and must not be specified.

The first API to be called is **adg1736_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Switch Control
~~~~~~~~~~~~~~

Use **adg1736_set_switch_state()** to control which output each switch connects to:
- ADG1736_CONNECT_A: Connect to output A (IN = HIGH)
- ADG1736_CONNECT_B: Connect to output B (IN = LOW)

Use **adg1736_get_switch_state()** to read the current switch position.

Enable/Disable Control (ADG1736 only)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If using ADG1736 with EN pin configured, use:
- **adg1736_enable()**: Enable the mux logic (EN = HIGH)
- **adg1736_disable()**: Disable the mux logic (EN = LOW)

Driver Initialization Examples
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**ADG736 Example:**

.. code-block:: c

        struct adg1736_dev *adg736_device;
        struct adg1736_init_param adg736_init_param = {
                .type = ADG736,
                .gpio_in1 = {
                        .number = 1,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_in2 = {
                        .number = 2,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_en = NULL,  // ADG736 does not have EN pin
        };

        ret = adg1736_init(&adg736_device, &adg736_init_param);
        if (ret != 0)
                return ret;

        // Set switch 1 to connect to output A
        ret = adg1736_set_switch_state(adg736_device, ADG1736_SW1, ADG1736_CONNECT_A);
        if (ret != 0)
                return ret;

**ADG1736 Example (without EN pin control):**

.. code-block:: c

        struct adg1736_dev *adg1736_device;
        struct adg1736_init_param adg1736_init_param = {
                .type = ADG1736,
                .gpio_in1 = {
                        .number = 1,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_in2 = {
                        .number = 2,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_en = NULL,  // EN pin not controlled (tied externally)
        };

        ret = adg1736_init(&adg1736_device, &adg1736_init_param);
        if (ret != 0)
                return ret;

**ADG1736 Example (with EN pin control):**

.. code-block:: c

        struct adg1736_dev *adg1736_device;
        struct no_os_gpio_init_param en_gpio = {
                .number = 3,
                .platform_ops = &gpio_platform_ops,
                .extra = NULL,
        };
        struct adg1736_init_param adg1736_init_param = {
                .type = ADG1736,
                .gpio_in1 = {
                        .number = 1,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_in2 = {
                        .number = 2,
                        .platform_ops = &gpio_platform_ops,
                        .extra = NULL,
                },
                .gpio_en = &en_gpio,  // EN pin controlled
        };

        ret = adg1736_init(&adg1736_device, &adg1736_init_param);
        if (ret != 0)
                return ret;

        // Mux is enabled by default during init

        // Set switch 1 to connect to output A
        ret = adg1736_set_switch_state(adg1736_device, ADG1736_SW1, ADG1736_CONNECT_A);
        if (ret != 0)
                return ret;

        // Disable the mux
        ret = adg1736_disable(adg1736_device);
        if (ret != 0)
                return ret;

        // Re-enable the mux
        ret = adg1736_enable(adg1736_device);
        if (ret != 0)
                return ret;