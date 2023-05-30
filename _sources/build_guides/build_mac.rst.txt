*************************
MAC Build Prerequisites
*************************

Prior to building a no-OS project, it is required to install an App Store
application that contains all required files for compiling, and setup some
environment variables for using the corect compiler.
Use MAC terminal to run the following commands under **Install Xcode.App**

Install Xcode.app
=================
1. Step one is to go to the App Store and install Xcode.app application
2. After installation the PATH envrionment variable needs setting, i. e. :

    .. code-block::

        $ export PATH=/Applications/Xcode.app/Contents/Developer/usr/bin:$PATH

3. In order to test the right environment variable set of PATH, one might run :

    .. code-block::

        $ which cc

If the displayed path is the one set by step two, then the compiler was set
correctly. Otherwise, a faulty installation of the Xcode.app might have
occured or an alternative path has been applied at installation, in which
case the following folder path might be the case :

    .. code-block::

        $ cd /Applications/Xcode.app/Contents/Developer/Toolchains

Building a project
==================
Use MAC terminal to run these commands

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

        $ export PLATFORM=mac

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
Once the **.out** file has been generated, one can choose to execute the binary
or use it for debugging through VSCode.

1. Uploading the binary to target is generically achieved with:
    .. code-block::

        $ make run 

2. Debugging using VSCode
	For VSCode debugging one needs to install the LLDB DAP extension, or any
	other LLDB supporting extension from VSCode marketplace.

	After installing the extension, navigate to the following directory,
	and open VSCode in there:

    .. code-block::

        $ cd build/app
	$ code .

	With VSCode opened, Run & Debug option using LLDB (launch) option should
	be available and ready for debugging.
