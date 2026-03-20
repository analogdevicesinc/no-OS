.. no-os-doxygen::


Overview
--------

The MCS GPIO driver is meant to provide the infrastructure for adding a GPIO to
a jesd204 topology for projects where one wants to do multi-chip synchronization.
The driver includes a state op that can be called from the jesd-fsm that toggles
the respective gpio at the necessary time to synchronize the devices.

Supported Devices
-----------------

Any GPIO required in a project featuring jesd and multi-chip synchronization.

Applications
------------

* MXFE Multi-Chip Synchronization

Configuration
-------------

The mcs_gpio driver init param only requires a no_os_gpio_init_param.

MCS_GPIO Driver Initialization Example
--------------------------------------

.. code-block:: c

        struct mcs_gpio_dev *mcs_dev;
        struct no_os_gpio_init_param gpio_req_init = {
                .number = PHY_SYNC,
                .platform_ops = &xil_gpio_ops,
                .extra = &xil_gpio_param
        };
        struct mcs_gpio_init_param mcs_gpio_init_params = {
                .gpio_req = &gpio_req_init,
        };

        status = mcs_gpio_init(&mcs_dev, &mcs_gpio_init_params);
        if (status) {
                printf("mcs_gpio_init() error: %" PRId32 "\n", status);
                return status;
        }

        /* Other devices initialization and configuration for jesd204 topology */

        struct jesd204_topology_dev devs[] = {
                /* Other devices in the topology */
                {
                        .jdev = mcs_dev->jdev,
                        .link_ids = {DEFRAMER_LINK0_TX},
                        .links_number = 1,
                },
        };
