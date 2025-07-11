import pylink
import argparse
import sys

def flash_progress(action, progress_string, percent):
    print(f"{action}: {percent}%")

def main(device_name, firmware_file, start_addr=0x10000000):
    """
    Connects to a J-Link and flashes a firmware file to the target MCU.

    Args:
        device_name (str): The name of the target MCU (e.g., 'MAX32690').
        firmware_file (str): The path to the .hex or .bin firmware file.
        start_addr (int): The memory address to start flashing at (for .bin files).
    """
    jlink = None
    try:
        # 1. Instantiate the JLink object
        jlink = pylink.JLink()
        print("Connecting to J-Link...")
        # On Linux, you may need to provide the path to the J-Link library
        # jlink = pylink.JLink(lib_path='/opt/SEGGER/JLink/libjlinkarm.so')

        # 2. Open a connection to the J-Link
        jlink.open()
        jlink.set_tif(pylink.enums.JLinkInterfaces.SWD)

        # 3. Connect to the target MCU
        print(f"Connecting to device: {device_name}")
        jlink.connect(device_name, verbose=True)
        print("Device connected.")

        print(jlink.target_connected())

        # 4. Flash the file
        print(f"Flashing file: {firmware_file}")
        # For .hex files, the address is ignored as it's in the file.
        # For .bin files, the address is where flashing starts.
        jlink.flash_file(firmware_file, addr=start_addr, on_progress=flash_progress)
        print("Flashing complete.")

        # 5. Reset and run the target
        print("Resetting and running target...")
        jlink.reset(halt=False)
        print("Device is running.")

    except pylink.errors.JLinkException as e:
        print(f"An error occurred: {e}", file=sys.stderr)
        sys.exit(1)
    finally:
        # 6. Ensure the connection is always closed
        print("Closing J-Link connection.")
        jlink.close()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Flash an MCU using pylink-square.')
    parser.add_argument('-d', '--device', required=True, help='Target device name (e.g., MAX32690).')
    parser.add_argument('-f', '--file', required=True, help='Path to the firmware file (.hex or .bin).')
    parser.add_argument('--addr', type=lambda x: int(x, 0), default=0x10000000,
                        help='Start address for flashing (for .bin files). Supports hex (0x...).')

    args = parser.parse_args()
    main(args.device, args.file, args.addr)