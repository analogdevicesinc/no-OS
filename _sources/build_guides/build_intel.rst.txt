*********************************
Intel/Quartus Build Prerequisites
*********************************

Prior to building a no-OS project for Intel FPGA platforms, it is required to
set up the Quartus/NIOS II toolchain.

.. note::
   Intel platform builds are supported on Linux only.

Prerequisites
=============

Assuming the Intel FPGA SDK is installed at this path:

.. code-block::

    /path/to/intel
    └── intelFPGA
        └── 18.1

Run:

.. code-block:: bash

    $ source no-OS/tools/scripts/platform/intel/environment.sh /path/to/intel/intelFPGA 18.1

Building a project
==================

Copy the **.sof** and **.sopcinfo** files into the project folder.

.. code-block:: bash

    $ ls
    Makefile  profiles  src  src.mk  system_bd.sopcinfo  adrv9009_a10gx.sof
    $ make

    # Alternatively you may select a .sopcinfo file explicitly by:
    $ make HARDWARE=path/to/system_bd.sopcinfo

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
JTAG cable connected and use the following commands to upload the program to
the board or debug.

Uploading the binary to target is achieved with:

.. code-block:: bash

    $ make run
