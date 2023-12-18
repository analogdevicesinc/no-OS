System ID (AXI_SYSID) no-OS Driver
==================================

Overview
--------

The System ID solution (AXI_SYSID) is comprised of 2 cores: the axi_sysid which provides the AXI Lite interface and sysid_rom which acts as a ROM and contains the data. Together they provide the user with information regarding the conditions in which the hardware system was built.
The System ID information is used to provide information about the system’s bit file to facilitate future debugging actions. This information will be stored internally in a ROM and will be made available to the system via the AXI Lite interface. 

Driver Initialization
---------------------

.. code-block:: C

	struct axi_sysid_init_param sysid_init = {
		.base = 0x85000000,
	};

	status = axi_sysid_init(&sysid_core, &sysid_init);
	if (status)
		goto error;

AXI_SYSID Driver Initialization and Usage Example
-------------------------------------------------

.. code-block:: C

	struct axi_sysid *sysid_core;
	char *name = NULL;
	int status;

	struct axi_sysid_init_param sysid_init = {
		.base = 0x85000000,
	};

	status = axi_sysid_init(&sysid_core, &sysid_init);
	if (status)
		goto error;

	name = axi_sysid_get_fpga_board(sysid_core);

	pr_info("Board is %s\n", name);

	status = axi_sysid_remove(sysid_core);
	if (status)
		goto error;

