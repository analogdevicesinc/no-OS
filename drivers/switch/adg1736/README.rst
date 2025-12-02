ADG1736 no-OS driver
====================

Supported Devices
-----------------

- `ADG1736 <https://www.analog.com/en/products/adg1736.html>`_

Overview
--------

The ADG1736 is a monolithic device comprising two independently selectable
CMOS single pole, double throw (SPDT) switches. Each switch can connect
its input to one of two outputs (A or B) based on the control signal.

The device operates from 1.8 V to 5.5 V single supply and features:
- Low on resistance (2.5 Ω typical)
- Fast switching times (tON: 16 ns, tOFF: 8 ns)
- Break-before-make switching action
- TTL-/CMOS-compatible control inputs

Applications
------------

- Audio and video switching
- USB 1.1 signal switching
- Sample-and-hold systems
- Communications systems
- Mechanical reed relay replacement

ADG1736 Device Configuration
----------------------------

Driver Initialization
~~~~~~~~~~~~~~~~~~~~~

In order to be able to use the device, you will have to provide the support
for the communication protocol (GPIO for the control pins IN1 and IN2).

You can optionally provide an EN pin to enable/disable the mux logic.
If not provided, the EN pin is assumed to be tied high externally.

The first API to be called is **adg1736_init**. Make sure that it returns 0,
which means that the driver was initialized correctly.

Switch Control
~~~~~~~~~~~~~~

Use **adg1736_set_switch_state()** to control which output each switch connects to:
- ADG1736_CONNECT_A: Connect to output A (IN = HIGH)
- ADG1736_CONNECT_B: Connect to output B (IN = LOW)

Use **adg1736_get_switch_state()** to read the current switch position.

Enable/Disable Control
~~~~~~~~~~~~~~~~~~~~~~

If EN pin is configured, use:
- **adg1736_enable()**: Enable the mux logic (EN = HIGH)
- **adg1736_disable()**: Disable the mux logic (EN = LOW)

Driver Initialization Examples
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**Without EN pin control:**

.. code-block:: c

        struct adg1736_dev *adg1736_device;
        struct adg1736_init_param adg1736_init_param = {
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

        // Set switch 1 to connect to output A
        ret = adg1736_set_switch_state(adg1736_device, ADG1736_SW1, ADG1736_CONNECT_A);
        if (ret != 0)
                return ret;

**With EN pin control:**

.. code-block:: c

        struct adg1736_dev *adg1736_device;
        struct no_os_gpio_init_param en_gpio = {
                .number = 3,
                .platform_ops = &gpio_platform_ops,
                .extra = NULL,
        };
        struct adg1736_init_param adg1736_init_param = {
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