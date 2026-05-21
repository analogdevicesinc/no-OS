*************************************
Raspberry Pi Pico Build Prerequisites
*************************************

Prior to building a no-OS project for Raspberry Pi Pico, it is required to
install the Pico SDK and debugging tools.

.. note::
   Pico platform builds are supported on Linux only.

Prerequisites
=============

- Clone the `Raspberry Pi Pico SDK <https://github.com/raspberrypi/pico-sdk>`_.
- Set the PICO_SDK_PATH environment variable to the pico-sdk cloned repository
  path:

    .. code-block:: bash

        $ export PICO_SDK_PATH=/path/to/pico-sdk

- Install the `J-Link software <https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack>`_.
- Set the JLINK_SERVER_PATH environment variable to the JLinkGDBServerCLExe path
  (the default path should be /opt/SEGGER/JLink/JLinkGDBServerCLExe):

    .. code-block:: bash

        $ export JLINK_SERVER_PATH=/opt/SEGGER/JLink/JLinkGDBServerCLExe

- (Optional) For visual debugging and building, install Visual Studio Code, and
  the Cortex-Debug extension.

Building a project
==================

To build a project, type:

.. code-block:: bash

    $ make PLATFORM=pico

The build process creates a **build** directory in the project folder:

.. code-block::

    build
    ├── app
    ├── bsp
    ├── obj
    ├── project_name.elf
    └── tmp

Running/Debugging
=================

Once the **.elf** file has been generated, make sure the board is powered on,
cable connected and use the following commands to upload the program to the
board:

.. code-block:: bash

    $ make run
