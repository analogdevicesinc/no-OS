*********************************************************************************
There are three supported Platforms:

Xilinx  :	Zynq, Microblaze
Linux   :	Userspace using UIO, spidev, and sysfs GPIO
Generic :	Skeleton

*********************************************************************************

To build for Xilinx:


Export your SDK_Export folder:

  On Linux:
  dave@HAL9000:~/devel/git/ad9361/sw$ export SDK_EXPORT=path_to_your/SDK/SDK_Export/hw

  On Windows: (SDK->Xilinx Tools->Launch Shell)
  C:\tmp\ad9361\sw>set SDK_EXPORT=path_to_your\SDK\SDK_Export\hw

Build for ZYNQ:
  Linux:
  dave@HAL9000:~/devel/git/ad9361/sw$ make -f Makefile.zynq [clean]

  Windows:
  C:\tmp\ad9361\sw>make -f Makefile.zynq [clean]

Build for Microblaze:

  Linux:
  dave@HAL9000:~/devel/git/ad9361/sw$ make -f Makefile.microblaze [clean]

  Windows:
  C:\tmp\ad9361\sw>make -f Makefile.microblaze [clean]

*********************************************************************************

To build for Linux:
dave@HAL9000:~/devel/git/ad9361/sw$ make -f Makefile.linux [clean]

*********************************************************************************

To build the skeleton:
dave@HAL9000:~/devel/git/ad9361/sw$ make -f Makefile.generic [clean]
