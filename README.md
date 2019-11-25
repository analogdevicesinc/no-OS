
# no-OS


[Analog Devices Inc.](http://www.analog.com/en/index.html) Software drivers for systems without OS.


## Software

The majority of ADI's products are peripherals to a non-ADI digital engine (FPGA, microprocessor, or microcontroller). While there is major work underway on FPGAs (Xilinx and Intel/Altera) and microprocessors (running an operating system like Linux), the efforts on microcontrollers are fragmented due to the diverse nature of the microcontroller market. The goal of these projects (microcontroller no-OS) is to be able to provide reference projects for lower end processors, which can't run Linux or aren't running a specific operating system, to help customers using FPGA/microcontrollers with ADI parts.

## Build

### Prerequisites(Linux)

Before generating the binaries, it is required to define some paths that are mandatory when using the provided Makefile. Those paths give access to certain utilities and compilers that come with the carrier's SDK and are used in the building process.

The following example presents the usage of the environment scripts.

Supposing the SDK is installed in /tools directory, the folders would be organized in a similar structure:

```
tools
├── intelFPGA
│   └── 18.1
└── Xilinx
    ├── DocNav
    ├── Downloads
    ├── SDK
    │   ├── 2017.4
    │   └── 2018.3
    ├── Vivado
    │   ├── 2017.4
    │   └── 2018.3
    └── xic

```
When adding the SDK's utilities path to the _PATH_ envvar, the following scripts come in handy:

***Intel***
```shell
sergiu@analog:~$ cd no-OS/tools/scripts/platform/intel/
sergiu@analog:~/no-OS/tools/scripts/platform/intel$ source environment.sh /tools/intelFPGA/ 18.1
```

***Xilinx***
```shell
sergiu@analog:~$ cd no-OS/tools/scripts/platform/xilinx/
sergiu@analog:~/no-OS/tools/scripts/platform/xilinx$ source environment.sh /tools/Xilinx/ 2017.4
```
or
```shell
sergiu@analog:~/no-OS/tools/scripts/platform/xilinx$ source environment.sh /tools/Xilinx/ 2018.3
```

### Compiling the sources

Go in the project directory that should be built.
```shell
sergiu@analog:~$ cd no-OS/projects/adrv9009/
sergiu@analog:~/no-OS/projects/adrv9009$ ls
Makefile  profiles  src  src.mk
```
Copy the generated hardware files (hdf/sof & sopcinfo) in the project folder and run make.

***Intel***
```shell
sergiu@analog:~/no-OS/projects/adrv9009$ ls
Makefile  profiles  src  src.mk  system_bd.sopcinfo  adrv9009_a10gx.sof	
sergiu@analog:~/no-OS/projects/adrv9009$ make
[11:39:43] Evaluating hardware : system_bd.sopcinfo
[11:39:43] Platform : altera
[11:39:49] [CC] headless.c 
[11:34:52] [CC] ad9528.c 
[11:34:52] [CC] no_os_hal.c 

...

[11:39:53] [LD] ad9528.d ad9528.o altera_a10_atx_pll.d altera_a10_atx_pll.o altera_a10_cdr_pll.d altera_a10_cdr_pll.o altera_adxcvr.d altera_adxcvr.o axi_adc_core.d axi_adc_core.o axi_dac_core.d axi_dac_core.o axi_dmac.d axi_dmac.o axi_io.d axi_io.o axi_jesd204_rx.d axi_jesd204_rx.o axi_jesd204_tx.d axi_jesd204_tx.o clk_altera_a10_fpll.d clk_altera_a10_fpll.o delay.d delay.o gpio.d gpio.o headless.d headless.o no_os_hal.d no_os_hal.o spi.d spi.o talise_agc.d talise_agc.o talise_arm.d talise_arm.o talise_cals.d talise_cals.o talise_config.d talise_config.o talise.d talise_error.d talise_error.o talise_gpio.d talise_gpio.o talise_hal.d talise_hal.o talise_jesd204.d talise_jesd204.o talise.o talise_radioctrl.d talise_radioctrl.o talise_rx.d talise_rx.o talise_tx.d talise_tx.o talise_user.d talise_user.o util.d util.o 
[11:39:49] Done 
```
***Xilinx***
```shell
sergiu@analog:~/no-OS/projects/adrv9009$ ls
Makefile  profiles  src  src.mk system_top.hdf
sergiu@analog:~/no-OS/projects/adrv9009$ make
[11:34:16] Evaluating hardware : system_top.hdf
[11:34:16] Found platform : xilinx
[11:34:24] Building hardware specification and bsp 
[11:34:52] [CC] headless.c
[11:34:52] [CC] ad9528.c 
[11:34:52] [CC] no_os_hal.c 

...

[11:34:59] [CC] delay.c 
[11:34:59] [LD] ad9528.d ad9528.o axi_adc_core.d axi_adc_core.o axi_adxcvr.d axi_adxcvr.o axi_dac_core.d axi_dac_core.o axi_dmac.d axi_dmac.o axi_io.d axi_io.o axi_jesd204_rx.d axi_jesd204_rx.o axi_jesd204_tx.d axi_jesd204_tx.o clk_axi_clkgen.d clk_axi_clkgen.o delay.d delay.o gpio.d gpio.o headless.d headless.o no_os_hal.d no_os_hal.o spi.d spi.o talise_agc.d talise_agc.o talise_arm.d talise_arm.o talise_cals.d talise_cals.o talise_config.d talise_config.o talise.d talise_error.d talise_error.o talise_gpio.d talise_gpio.o talise_hal.d talise_hal.o talise_jesd204.d talise_jesd204.o talise.o talise_radioctrl.d talise_radioctrl.o talise_rx.d talise_rx.o talise_tx.d talise_tx.o talise_user.d talise_user.o util.d util.o xilinx_transceiver.d xilinx_transceiver.o 
[11:34:52] Done 

```

Now, after the project has been succesfully compiled, the output folder will look like the following:

```shell
sergiu@analog:~/no-OS/projects/adrv9009$ tree build -L 1
build
├── app
├── bsp
├── obj
├── release.elf
└── tmp
```

Additionaly, after the .elf file has been generated, the software can be uploaded to the board:

```
sergiu@analog:~/no-OS/projects/adrv9009$ make run
[11:56:39] Evaluating hardware : system_bd.sopcinfo
Searching for SOF file:
in .
  adrv9009_a10gx.sof

Info: *******************************************************************
Info: Running Quartus Prime Programmer
Info: Command: quartus_pgm --no_banner --mode=jtag -o p;./adrv9009_a10gx.sof
Info (213045): Using programming cable "USB-BlasterII [3-1.7.3]"
Info (213011): Using programming file ./adrv9009_a10gx.sof with checksum 0x312C6B7A for device 10AX115S2F45@1
Info (209060): Started Programmer operation at Mon Nov 25 11:56:49 2019
Info (209016): Configuring device index 1
Info (209017): Device 1 contains JTAG ID code 0x02E060DD
Info (209007): Configuration succeeded -- 1 device(s) configured
Info (209011): Successfully performed operation(s)
Info (209061): Ended Programmer operation at Mon Nov 25 11:57:04 2019
Info: Quartus Prime Programmer was successful. 0 errors, 0 warnings
    Info: Peak virtual memory: 1670 megabytes
    Info: Processing ended: Mon Nov 25 11:57:04 2019
    Info: Elapsed time: 00:00:25
    Info: Total CPU time (on all processors): 00:00:11
Using cable "USB-BlasterII [3-1.7.3]", device 1, instance 0x00
Resetting and pausing target processor: OK
Initializing CPU cache (if present)
OK
Downloaded 465KB in 0.6s (775.0KB/s)
Verified OK                         
Starting processor at address 0x00000244
nios2-terminal: connected to hardware target using JTAG UART on cable
nios2-terminal: "USB-BlasterII [3-1.7.3]", device 1, instance 0
nios2-terminal: (Use the IDE stop button or Ctrl-C to terminate)

Hello
rx_device_clk_pll: FPLL PLL calibration OK (1200 us)
...
```

## Which branch should I use?

  * If you want to use the most stable code base, always use the [latest release branch]( https://github.com/analogdevicesinc/no-OS/branches/all?utf8=%E2%9C%93&query=201).

  * If you want to use the greatest and latest, check out the master branch.


## Support

Feel free to ask any question at [EngineerZone]( https://ez.analog.com/linux-device-drivers/microcontroller-no-os-drivers/f/q-a).
