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

Host libraries
^^^^^^^^^^^^^^

The Vitis tools depend on a few host libraries that the Vitis installer does
not ship. The installer ends with a reminder to install them, and the build
cannot read the ``.xsa`` hardware file without them. Starting with Vitis
2026.1 this includes ``libncurses.so.5``, which current distributions no
longer install by default.

Run the script provided by Vitis once after installing it:

.. code-block:: bash

    $ sudo /path/to/xilinx/2026.1/Vitis/scripts/installLibs.sh

Then check that the library is available:

.. code-block:: bash

    $ ldconfig -p | grep libncurses.so.5

The script only supports the distributions on the Vitis support list, so the
check above can still come back empty:

.. list-table::
   :header-rows: 1
   :widths: 35 65

   * - Distribution
     - What ``installLibs.sh`` does
   * - Ubuntu
     - Installs the ``libncurses5`` and ``libtinfo5`` packages. On releases
       that no longer provide these packages, ``apt-get`` reports an error
       and the script continues without installing them.
   * - RHEL, AlmaLinux, CentOS, Rocky Linux
     - Installs ``ncurses-compat-libs``.
   * - Kali, Debian and other distributions
     - Nothing. The distribution is not recognized and no package is
       installed.

If the library is still missing, provide it from the ncurses 6 library that
the distribution already installs. On Debian-based distributions (Kali,
Debian, recent Ubuntu):

.. code-block:: bash

    $ sudo ln -s /usr/lib/x86_64-linux-gnu/libncurses.so.6 \
          /usr/lib/x86_64-linux-gnu/libncurses.so.5
    $ sudo ln -s /usr/lib/x86_64-linux-gnu/libtinfo.so.6 \
          /usr/lib/x86_64-linux-gnu/libtinfo.so.5

Skip a line if that file already exists. The links only add the old library
names; nothing that is already installed changes, and removing the two links
undoes the step.

When a host library is missing, configuring a project stops with an error
that names it, for example:

.. code-block:: text

    Vitis needs the host library libncurses.so.5, which is not installed.

Building a project
------------------

no-OS uses a **CMake** build driven by board presets and Kconfig defconfigs,
orchestrated by ``tools/scripts/no_os_build.py``. Configure with the target
board preset, the project/variant defconfig, and the ``.xsa`` hardware file,
then build the project target:

.. code-block:: bash

    $ cmake -B build-<project> --preset <board> \
          -DPROJECT_DEFCONFIG=<project>/<variant>.conf \
          -DHARDWARE=path/to/system_top.xsa
    $ cmake --build build-<project> --target <project>

Or, in one step with the helper (pass the ``.xsa`` via ``--hardware``):

.. code-block:: bash

    $ python tools/scripts/no_os_build.py build \
          --project <project> --variant <variant> --board <board> \
          --hardware path/to/system_top.xsa

The firmware (``build/<project>.elf``) and the intermediate Vitis BSP/FSBL work
directories are created under the configured build directory — the one passed to
``cmake -B`` above, or ``build/<project>-<variant>-<board>/`` when using the
helper. See the
:doc:`../cmake_cheatsheet` for the full set of build, configure and cleanup
commands.

Running/Debugging
-----------------

Once the **.elf** file has been generated, make sure the board is powered on
and the JTAG cable connected. Uploading the binary to the target is done with
the ``flash`` target:

.. code-block:: bash

    $ cmake --build build-<project> --target flash

This programs the FPGA bitstream (extracted from the ``.xsa``), initialises the
PS, and downloads and runs the ELF over JTAG. It is driven by the Vitis Python
API in ``tools/scripts/platform/xilinx/util.py`` (invoked via ``vitis -s``); for
``cortexa53`` (ZynqMP) and ``cortexr5`` targets an FSBL is generated
automatically first. The equivalent helper one-liner appends ``--flash``:

.. code-block:: bash

    $ python tools/scripts/no_os_build.py build \
          --project <project> --variant <variant> --board <board> \
          --hardware path/to/system_top.xsa --flash --probe openocd

.. note::
   ``no_os_build.py --flash`` still requires a ``--probe`` value even though the
   Xilinx JTAG flow ignores it. Invoking the ``flash`` target through ``cmake``
   directly avoids the dummy probe argument.

If multiple JTAG probes are connected, or a specific core must be targeted, set
the ``JTAG_CABLE_ID`` and/or ``TARGET_CPU`` cache variables at configure time:

.. code-block:: bash

    $ cmake -B build-<project> --preset <board> \
          -DPROJECT_DEFCONFIG=<project>/<variant>.conf \
          -DHARDWARE=path/to/system_top.xsa \
          -DJTAG_CABLE_ID=<id> -DTARGET_CPU=<cpu>

For interactive debugging, use the Vitis IDE.

.. tip::
   For detailed Vitis IDE debugging instructions, see
   :doc:`build_xilinx_vitis2025`.

Remote Host
^^^^^^^^^^^

For Xilinx projects you can flash a board connected to a remote host. On the
remote host make sure to start ``hw_server``. On your development environment,
export the host and port before running the ``flash`` target; the Vitis JTAG
session picks them up automatically:

.. code-block:: bash

    $ export XSCT_REMOTE_HOST=<remote host ip>
    $ export XSCT_REMOTE_PORT=<remote host hw_server port>
    $ cmake --build build-<project> --target flash

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

The build steps are the same as on Linux — configure with the board preset,
project/variant defconfig and ``.xsa`` hardware, then build the project target:

.. code-block:: bash

    $ cmake -B build-<project> --preset <board> \
          -DPROJECT_DEFCONFIG=<project>/<variant>.conf \
          -DHARDWARE=path/to/system_top.xsa
    $ cmake --build build-<project> --target <project>

Running/Debugging
-----------------

The running and debugging steps are the same as on Linux. See the Linux section
above for the ``flash`` target, the ``JTAG_CABLE_ID`` / ``TARGET_CPU`` cache
variables, the Vitis IDE, and remote host instructions.

.. toctree::
   :hidden:

   build_xilinx_vitis2025
