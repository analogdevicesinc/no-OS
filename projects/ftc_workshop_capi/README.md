# FTC Workshop Project

This project demonstrates Bluetooth Low Energy functionality using the Cordio library with support for two different operation modes.

## BLE Operation Modes

### 1. Peripheral Mode (Default)
The device operates as a BLE peripheral that advertises and can accept connections from central devices.

**Build command:**
```bash
cmake --preset apard32690 -B build -DPROJECT_DEFCONFIG=ftc_workshop/project.conf
cmake --build build --target ftc_workshop
```

**Features:**
- Advertises as "Periph"
- Accepts BLE connections
- Implements GATT services
- Full BLE stack functionality

### 2. HCI-UART Mode
The device operates as an HCI controller accessible via UART interface, allowing external host applications to control BLE functionality.

**Build command:**
```bash
cmake --preset apard32690 --fresh -B build -DPROJECT_DEFCONFIG=ftc_workshop/project_hci_uart.conf
cmake --build build --target ftc_workshop
```

**Features:**
- Exposes Bluetooth controller via HCI commands over UART
- Allows external applications to send custom BLE data
- Compatible with standard HCI tools and applications (e.g., bluetoothctl, hcitool)
- Proper WSF (Wireless Software Foundation) initialization
- Full Link Layer (LL) and Baseband (BB) configuration
- Ready to receive standard HCI commands for advertising, scanning, connections
- Runs as FreeRTOS task with preserved USB console functionality
- Useful for implementing custom host applications

## Configuration

The BLE mode is selected through Kconfig options:

- `CONFIG_BLE_MODE_PERIPHERAL=y` - Peripheral mode (default)
- `CONFIG_BLE_MODE_HCI_UART=y` - HCI-UART mode

## Hardware Requirements

- MAX32690 development board (EVKIT, FTHR, or APARD32690-SL)
- USB connection for programming and console output
- For HCI-UART mode: UART connection for HCI communication

## Usage

### Peripheral Mode
1. Flash the peripheral mode firmware
2. Open a serial terminal at 115200 baud
3. The device will start advertising and can be discovered by BLE scanners

### HCI-UART Mode
1. Flash the HCI-UART mode firmware
2. Connect to the HCI UART interface
3. Use HCI commands to control the Bluetooth controller
4. Send custom BLE data through HCI interface

## Notes

- Both modes use the same hardware platform
- Configuration is determined at build time
- The project uses FreeRTOS for task management
- Cordio library provides the BLE stack implementation