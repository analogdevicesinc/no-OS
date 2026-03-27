import pylink
import argparse
import sys
import re

# Flash base addresses by device family (pattern → address).
# Used when flashing .bin files and no explicit --addr is provided.
FLASH_BASE_ADDRESSES = [
    (r"^MAX\d",       0x10000000),  # Maxim MAX32xxx / MAX78xxx
    (r"^STM32",       0x08000000),  # STM32 families
    (r"^ADUCM",       0x00000000),  # ADuCM3029 / ADuCM4050
    (r"^RP2040",      0x10000000),  # Raspberry Pi Pico (XIP flash)
]

DEFAULT_FLASH_ADDR = 0x00000000

def get_flash_base(device_name):
    """Return the default flash base address for a device name."""
    upper = device_name.upper()
    for pattern, addr in FLASH_BASE_ADDRESSES:
        if re.match(pattern, upper):
            return addr
    return DEFAULT_FLASH_ADDR

def flash_progress(action, progress_string, percent):
    print(f"{action}: {percent}%")

def main(device_name, firmware_file, start_addr):
    """
    Connects to a J-Link and flashes a firmware file to the target MCU.

    Args:
        device_name (str): The target device name (e.g., 'MAX32690', 'STM32F756ZG').
        firmware_file (str): The path to the .hex or .bin firmware file.
        start_addr (int): The memory address to start flashing at (for .bin files).
    """
    jlink = None
    try:
        jlink = pylink.JLink()
        print("Connecting to J-Link...")

        jlink.open()
        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)

        print(f"Connecting to device: {device_name}")
        jlink.connect(device_name, verbose=True)
        print("Device connected.")

        print(jlink.target_connected())

        # For .hex files, the address is ignored as it's embedded in the file.
        # For .bin files, the address is where flashing starts.
        print(f"Flashing file: {firmware_file} at 0x{start_addr:08X}")
        jlink.flash_file(firmware_file, addr=start_addr, on_progress=flash_progress)
        print("Flashing complete.")

        print("Resetting and running target...")
        jlink.reset(halt=False)
        print("Device is running.")

    except pylink.errors.JLinkException as e:
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        print("Closing J-Link connection.")
        jlink.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Flash an MCU using pylink-square.')
    parser.add_argument('-d', '--device', required=True,
                        help='Target device name (e.g., MAX32690, STM32F756ZG).')
    parser.add_argument('-f', '--file', required=True,
                        help='Path to the firmware file (.hex or .bin).')
    parser.add_argument('--addr', type=lambda x: int(x, 0), default=None,
                        help='Start address for flashing (for .bin files). '
                             'Supports hex (0x...). Auto-detected from device name if omitted.')

    args = parser.parse_args()

    start_addr = args.addr if args.addr is not None else get_flash_base(args.device)
    main(args.device, args.file, start_addr)
