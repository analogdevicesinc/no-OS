****************************
Build against FTD2XX library
****************************

Prior to building a no-OS project, after setting up the environment for the
used platform, in order to use the FTD2XX library, one needs to have
Python installed on the machine in order for the automated script
for library installation to work.

In order to properly install Python 3, one can follow the next guide : `Python Installation Guide <https://wiki.python.org/moin/BeginnersGuide/Download>`_

The library at the time only supports the `linux`` platform, since the
architecture of the used library is a x86-based one.

Install FTD2XX libraries
========================
When running the first `make` on a 'fresh' clone of the repository, the script
will automatically download the required libraries for building the project,
if the LIBRARIES environment variable has the 'ftd2xx' value attributed along
other used libraries (documented at step 2 of the "Building a project"
section).

Otherwise, one can run the python script manually, by specifying the ftd2xx
libraries subfolder as a parameter:

	.. code-block::

		cd no-OS/tools/scripts
		python config_ftd2xx.py ../../libraries/ftd2xx/
      
- (Optional) For visual debugging and building, install Visual Studio Code, and the C/C++ extension.

Building a project
==================
*Use Git Bash to run these commands if running the make on Windows OS.*

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
        
2) Set the PLATFORM and LIBRARIES environment variables with the following command:
    .. code-block::

        $ export PLATFORM=linux; export LIBRARIES+=ftd2xx

	- **OPTIONAL**: One can specifiy the `LIBRARIES+=ftd2xx` line in the Makefile of the project, so a environment variable won't be necessary for every terminal window opened.
	- **IMPORTANT**:  Setting the environment variables must be done every time a new terminal window is opened.

3) Build the project.
    .. code-block::

        $ make

The build process creates a build directory in the project folder:
    .. code-block::

        build
        ├── app
        ├── objs
        └── project_name.out

Running
=======
Once the **.out** file has been generated, executing the binary is generically achieved with:
    .. code-block::

        $ make run 
