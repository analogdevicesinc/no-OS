# Eval-PowRMS Project

## Overview
The Eval-PowRMS project is designed to evaluate and demonstrate the functionality of power RMS (Root Mean Square) measurement systems. It includes source code, examples, and platform-specific implementations for the Maxim MAX32662 microcontroller.

## Folder Structure
The project is organized as follows:

```
├── build/                # Build artifacts
├── src/                 # Source code
│   ├── common/          # Common utilities and data
│   ├── examples/        # Example applications
│   └── platform/        # Platform-specific code
├── noos/                # No-OS drivers and libraries
│   ├── drivers/         # Hardware drivers
│   ├── include/         # Header files
│   ├── libraries/       # External libraries (e.g., LVGL)
│   └── util/            # Utility functions
├── objs/                # Object files
├── lv_conf.h            # LVGL configuration
├── Makefile             # Build system entry point
├── README.md            # Project documentation
├── src.mk               # Source file list for the build system
└── test.sh              # Test script
```

## Changes needed: 

```diff
index 2ebcec56c2..70c83abdd6 100644
--- a/iio/iio_app/iio_app.c
+++ b/iio/iio_app/iio_app.c
@@ -241,7 +241,7 @@ static int32_t uart_setup(struct no_os_uart_desc **uart_desc,
                /* TODO: remove this ifdef when asynchrounous rx is implemented on every platform. */
 #if defined(STM32_PLATFORM) || defined(MAXIM_PLATFORM) || defined(ADUCM_PLATFORM) || defined(PICO_PLATFORM)
                .irq_id = uart_init_par->irq_id,
-               .asynchronous_rx = true,
+               .asynchronous_rx = false,
 #endif
                .baud_rate = UART_BAUDRATE_DEFAULT,
                .size = NO_OS_UART_CS_8,
diff --git a/libraries/esh/esh b/libraries/esh/esh
```


## Build and Run Instructions

### Prerequisites
- A Linux-based operating system
- `make` build system
- Maxim MAX32662 development environment

### Build the Project
To build the project for the Maxim MAX32662 platform, run the following task:

```bash
make -C /home/rbudai/Workspace_no_os/no-os/projects/eval-powrms PLATFORM=maxim TARGET=max32662
```

Alternatively, you can use the VS Code task labeled **maxim Build**.

### Run the Project
To run the project, execute:

```bash
make run -C /home/rbudai/Workspace_no_os/no-os/projects/eval-powrms PLATFORM=maxim TARGET=max32662
```

Alternatively, you can use the VS Code task labeled **maxim Run**.

## Dependencies
- [LVGL](https://lvgl.io/) for graphical user interface components
- Maxim SDK for platform-specific drivers and libraries

## Contributing
Contributions are welcome! Please follow these steps:
1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Commit your changes with clear and concise messages.
4. Submit a pull request.

## License
This project is licensed under the [MIT License](LICENSE).