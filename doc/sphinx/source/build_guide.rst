*****************
No-OS Build Guide
*****************

 
.. note::
    These build guides are valid for the projects found in the no-OS/projects folder. If your project resides elsewhere under the no-OS repository tree, it is a legacy project. 
    A build guide for legacy projects can be found `Build no-OS with GNU make <https://wiki.analog.com/resources/fpga/no-os_make/software_setup>`_.

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
