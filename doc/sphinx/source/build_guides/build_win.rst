***************************
Windows Build Prerequisites
***************************

Prior to building a no-OS project, it is required to set up some environment variables so that the build process may find the necessary tools (compiler, linker, SDK etc.).

Use the following commands to prepare your environment for building no-OS projects:

    You only need to do this once per Git Bash installation: 
        - Open up a Git Bash as Administrator once and run the **no-os/tools/scripts/git-bash.sh** script.
        - Close the window.

    Use Git Bash (unelevated) for the rest of your development.


Install MSYS2 GCC
=================

In order for proper installation of the MSYS2 GCC one might follow the official guide : https://www.msys2.org/
After GCC is fully and correctly installed, its location must be well known, and
the $PATH environment variable needs to be set, i. e. :

    .. code-block::
        
        $ export PATH=/c/msys64/mingw64/bin:$PATH

And last step is to make a copy of the *gcc* and rename it to *cc* using the
Windows File Explorer if *cc* its not to be found.
Finally test if the compiler is set, one might run :

    .. code-block::
        
        $ which cc

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
