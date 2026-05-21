**************************
Xilinx Build Prerequisites
**************************

Prior to building a no-OS project for Xilinx platforms, it is required to set up
the Vitis toolchain so that the build process may find the necessary tools
(compiler, linker, xsct etc.).

Linux
=====

Prerequisites
-------------
Assuming the Vitis 2022.2 is installed at this path:

.. code-block::

    /path/to/xilinx
    ├── DocNav
    ├── Downloads
    └── Vitis
        └── 2022.2

Run:

.. code-block:: bash

    $ source /path/to/xilinx/Vitis/2022.2/settings64.sh

Building a project
------------------

Copy the **.xsa** file into the project folder.

.. code-block:: bash

    $ ls
    Makefile  profiles  src  src.mk  system_top.xsa
    $ make

    # Alternatively you may select an .xsa file explicitly by:
    $ make HARDWARE=path/to/file.xsa

The build process creates a **build** directory in the project folder:

.. code-block::

    build
    ├── app
    ├── bsp
    ├── obj
    ├── project_name.elf
    └── tmp

Running/Debugging
-----------------

Once the **.elf** file has been generated, make sure the board is powered on,
JTAG cable connected and use the following commands to upload the program to
the board or debug.

Uploading the binary to target is achieved with:

.. code-block:: bash

    $ make run

To open the Vitis IDE for debugging:

.. code-block:: bash

    $ make sdkopen

.. tip::
   For detailed Vitis IDE debugging instructions, see
   :doc:`build_xilinx_vitis2025`.

Booting from SD Card
^^^^^^^^^^^^^^^^^^^^

You may also boot a Xilinx project from an SD card. Copy the generated
``build/BOOT.BIN`` file onto the first partition of the card, ensuring it is
formatted as FAT32. Insert the card, set the jumpers for SD boot, and power
on the system.

Remote Host
^^^^^^^^^^^

For Xilinx projects you can flash the board connected to a remote host. On the
remote host make sure to start ``hw_server``. On your development environment
run:

.. code-block:: bash

    $ export XSCT_REMOTE_HOST=<remote host ip>
    $ export XSCT_REMOTE_PORT=<remote host hw_server port>
    $ make run

.. note::
   By default the ``hw_server`` port should be 3121.

Windows
=======

Prerequisites
-------------

.. important::
   Use Git Bash to run these commands.

Assuming the Vitis 2022.2 is installed at this path:

.. code-block::

    C:\Xilinx
    ├── DocNav
    ├── Downloads
    └── Vitis
        └── 2022.2

From the no-OS root directory, run:

.. code-block:: bash

    $ source tools/scripts/git-bash-paths.sh /c/Xilinx/Vitis/2022.2/settings64.sh

Or alternatively, add the desired paths manually:

.. code-block:: bash

    $ export PATH=/c/Xilinx/Vitis/2022.2/bin:/c/Xilinx/Vitis/2022.2/gnu/aarch64/nt/aarch64-none/bin/:$PATH

Building a project
------------------

Copy the **.xsa** file into the project folder and run:

.. code-block:: bash

    $ ls
    Makefile  profiles  src  src.mk  system_top.xsa
    $ make

    # Alternatively you may select an .xsa file explicitly by:
    $ make HARDWARE=path/to/file.xsa

Running/Debugging
-----------------

The running and debugging steps are the same as on Linux. See the Linux section
above for ``make run``, ``make sdkopen``, SD card boot, and remote host
instructions.

.. toctree::
   :hidden:

   build_xilinx_vitis2025
