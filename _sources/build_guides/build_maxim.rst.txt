*************************
Maxim Build Prerequisites
*************************

Prior to building a no-OS project, it is required to set up some environment variables so that the build process may find the necessary tools (compiler, linker, SDK etc.).

Use the following commands to prepare your environment for building no-OS projects:

    You only need to do this once per Git Bash installation: 
        - Open up a Git Bash as Administrator once and run the **no-os/tools/scripts/git-bash.sh** script.
        - Close the window. 

    Use Git Bash (unelevated) for the rest of your development.


Install Maxim SDK
=================
The Maxim Microcontrollers SDK (MSDK) contains the necessary software and tools to develop firmware for the MAX32xxx and MAX78xxx Microcontrollers. That includes register and 
system startup files to enable low-level development for its supported parts.

- Install the `Maxim Micros SDK <https://www.maximintegrated.com/en/design/software-description.html/swpart=SFW0010820A>`_ to a path without whitespaces like **C:\\MaximSDK**.
- Set the MAXIM_LIBRARIES environment variable by running: 
    .. code-block::

        export MAXIM_LIBRARIES=/c/MaximSDK/Libraries
        
- (Optional) For visual debugging and building, install Visual Studio Code, and the Cortex-Debug extension.


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
        
2) Set the PLATFORM and TARGET environment variables with the following command:
    .. code-block::

        $ export PLATFORM=maxim; export TARGET=...

    - The **TARGET** specifies the chip for which the project is built. If it is missing, **max32660** will be used. At the moment, the available targets are: *max32650*, *max32655*, *max32660*, *max32665*, *max32670*, *max32690* and *max78000*.
    - **IMPORTANT**:  Setting the environment variables must be done every time a new Bash window is opened.

3) Build the project.
    .. code-block::

        $ make

The build process creates a build directory in the project folder:
    .. code-block::

        build
        ├── app
        ├── bsp
        ├── obj
        ├── project_name.elf
        └── tmp


Running/Debugging
==================
Once the **.elf**, **.hex** or **.bin** file has been generated, make sure the board is powered on, JTAG (Joined Test Action Group - low level interface to cpu) cable connected and use the following commands to upload the program to the board or debug.

Uploading the binary to target is generically achieved with:
    .. code-block::

        $ make run 