***************************
Windows Build Prerequisites
***************************

Prior to building a no-OS project, it is required to set up some environment variables so that the build process may find the necessary tools (compiler, linker, libraries etc.).

Use the following commands to prepare your environment for building no-OS projects:

    You only need to do this once per Git Bash installation: 
        - Open up a Git Bash as Administrator once and run the **no-os/tools/scripts/git-bash.sh** script.
        - Close the window.

    Use Git Bash (unelevated) for the rest of your development.


Install MSYS2 GCC
=================

In order for proper installation of the MSYS2 GCC one needs to follow the official guide : https://www.msys2.org/

After GCC is fully and correctly installed through the msys2 binaries,
its location must be well known because we'll use the said binaries
further in Git Bash through our build/debug/run. 
The **$PATH** environment variable needs to be set.

*Use Git Bash to run these commands.*

If you have installed MSYS2 in default location :

    .. code-block::
        
        $ export PATH=/c/msys64/mingw64/bin:$PATH

Otherwise, in case of a custom installation :

    .. code-block::

        $ export PATH=/path/to/msys/mingw64/bin:$PATH

Finally test if the compiler is set, one might run :

    .. code-block::
        
        $ which gcc

Building a project
==================
*Use Git Bash to run these commands.*

1) Go in the project directory that should be built.
    .. code-block::
        
        $ cd no-OS/projects/<project_name>

    It should contain make-related files and source files:

    .. code-block::
        
        ./no-OS/projects/<project_name>
        ├── builds.json
        ├── Makefile
        ├── src
        └── src.mk
        
2) Set the PLATFORM environment variable with the following command:
    .. code-block::

        $ export PLATFORM=win;

3) Build the project.
    .. code-block::

        $ make

The build process creates a build directory in the project folder:
    .. code-block::

        build
        ├── app
        ├── bsp
        ├── obj
        ├── project_name.out
        └── tmp


Running/Debugging
==================
Once the **.out** file has been generated, make sure the board is powered on, cable connected and use the following commands to upload the program to the board or debug.

Uploading the binary to target is generically achieved with:
    .. code-block::

        $ make run 

For debugging the code, one should head straight into the app folder and open VSCode.
    .. code-block::

        $ cd build/app 
        $ code . 
