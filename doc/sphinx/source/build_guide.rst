*****************
No-OS Build Guide
*****************

Clone no-OS with **--recursive** flag:
   
    .. code-block::

        git clone --recursive https://github.com/analogdevicesinc/no-OS



If however you've already cloned no-OS without the **--recursive** flag, you may initialize all the submodules in an existing no-OS clone with:
    
    .. code-block::

        git submodule update --recursive --init


MAXIM
======
.. toctree::
   :maxdepth: 1
   
   build_guides/build_maxim


FTD2XX
======
.. toctree::
    :maxdepth: 1

    build_guides/build_ftd2xx

WIN
===
.. toctree::
    :maxdepth: 1

    build_guides/build_win

MAC
======
.. toctree::
    :maxdepth: 1

    build_guides/build_mac

XILINX VITIS 2025.1
===================
.. toctree::
    :maxdepth: 1

    build_guides/build_xilinx_vitis2025

