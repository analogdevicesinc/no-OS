*********************
no-OS's documentation
*********************

Welcome to **no-OS**, Analog Devices' bare-metal software framework.

no-OS started as a result of customers' need to configure ADI's hardware
components from bare-metal software environments. What began as a request for
software drivers has grown into a complete framework that generates full
applications, highly customizable according to users' requirements. It provides:

- **Platform-agnostic device drivers** for ADI components that run without an
  operating system.
- **A hardware abstraction layer** so the same driver works across Xilinx,
  Intel, Maxim, STM32, ADuCM3029, Raspberry Pi Pico and more.
- **Ready-to-build project examples** you can flash to supported evaluation
  boards to get started quickly.

.. note::

   We are in the process of moving the no-OS documentation from `wiki.analog.com <https://wiki.analog.com/resources/no-os>`_
   to  `analogdevicesinc.github.io/no-OS <http://analogdevicesinc.github.io/no-OS/>`_.

   We apologize for any inconvenience that may occur due to this migration.

Getting started
---------------

New to no-OS? Start here:

#. **Clone the repository** (with submodules) and set up your toolchain by
   following the :doc:`build_guide` for your platform.
#. **Pick a project** from the :doc:`projects_doc` that matches your evaluation
   board, then build and flash it.
#. **Explore the drivers** you need in the :doc:`drivers_doc`, and read the
   :doc:`drivers_guide` if you plan to write or adapt one.

Clone no-OS with the **--recursive** flag to pull in all submodules:

.. code-block::

    git clone --recursive https://github.com/analogdevicesinc/no-OS

Documentation guide
-------------------

Not sure where to look? Use this map:

- :doc:`build_guide` — set up your toolchain and build no-OS for your platform
  (Xilinx, Intel, Maxim, STM32, ADuCM3029, Raspberry Pi Pico, Lattice and
  more), including Windows, macOS and WSL2 instructions.
- :doc:`projects_doc` — browse the ready-to-build project examples, grouped by
  application area (ADCs, DACs, RF, power, sensors and others).
- :doc:`drivers_doc` — reference documentation for every no-OS device driver,
  organized by device category.
- :doc:`drivers_guide` — learn how no-OS drivers are structured and how to
  create or port one.
- :doc:`contributing` — the submission checklist and coding style expected for
  pull requests to the no-OS repository.

Contents
--------

.. toctree::
   :maxdepth: 2

   getting_started
   architecture
   iio_framework
   drivers_guide
   porting_guide
   drivers_doc
   projects_doc
   build_guide
   cmake_cheatsheet
   configuration_guide
   contributing
   troubleshooting

Additional resources
--------------------

- **Source code**: `github.com/analogdevicesinc/no-OS <https://github.com/analogdevicesinc/no-OS>`_
- **API reference**: for code documentation automatically generated from the
  sources, see our `No-OS Doxygen Documentation <https://analogdevicesinc.github.io/no-OS/doxygen>`_.
- **Community support**: ask questions on the `EngineerZone <https://ez.analog.com/>`_ forums.
