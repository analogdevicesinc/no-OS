#!/usr/bin/env python3
"""
Bootloader firmware upload script
Uploads a binary file to a device via UART using a menu-driven bootloader protocol
"""

import serial
import sys
import time
import os
import re
from typing import Optional

def wait_for_pattern(ser: serial.Serial, patterns: list[str], timeout: float = 5.0) -> Optional[str]:
    """
    Wait for one of the patterns to appear in the serial output
    Returns the matched text or None on timeout
    """
    buffer = ""
    start_time = time.time()
    
    while time.time() - start_time < timeout:
        if ser.in_waiting:
            data = ser.read(ser.in_waiting).decode('utf-8', errors='ignore')
            buffer += data
            print(data, end='', flush=True)
            
            # Check for pattern matches
            for pattern in patterns:
                if re.search(pattern, buffer, re.IGNORECASE | re.DOTALL):
                    return buffer
        time.sleep(0.01)
    
    return None

def send_menu_choice(ser: serial.Serial, choice: str, timeout: float = 10.0) -> bool:
    """Send a menu choice and wait for response"""
    # Clear any pending data first
    time.sleep(0.1)
    if ser.in_waiting:
        ser.read(ser.in_waiting)
    
    # Send a newline to trigger menu display if needed
    ser.write(b"\n")
    time.sleep(0.5)
    
    # Wait for menu prompt
    if not wait_for_pattern(ser, ["Select a loader operation:", "operation:"], timeout=timeout):
        print("\nError: Menu prompt not found")
        return False
    
    # Send choice
    ser.write(f"{choice}\n".encode())
    time.sleep(0.1)
    return True

def exit_loader_mode(ser: serial.Serial):
    """Exit loader mode by sending character 13"""
    print("\n--- Exiting loader mode ---")
    ser.write(b"13\n")
    time.sleep(0.5)

def upload_firmware(port: str, baudrate: int, filename: str, offset: int, chunk_size: int = 16384):
    """Upload firmware to device"""
    
    # Validate input file
    if not os.path.exists(filename):
        print(f"Error: File '{filename}' not found")
        return False
    
    filesize = os.path.getsize(filename)
    print(f"Firmware file: {filename}")
    print(f"File size: {filesize} bytes")
    print(f"Start offset: 0x{offset:X} ({offset} decimal)")
    print(f"Chunk size: {chunk_size} bytes")
    print(f"Port: {port}")
    print(f"Baudrate: {baudrate}")
    print()
    
    # Open serial connection
    try:
        ser = serial.Serial(port, baudrate, timeout=1)
        time.sleep(0.5)  # Give device time to initialize
        ser.reset_input_buffer()
    except Exception as e:
        print(f"Error opening serial port: {e}")
        return False
    
    try:
        # Put interface in loader mode by sending any character
        print("\n--- Entering loader mode ---")
        ser.write(b"l")  # Send any character to enter loader mode
        time.sleep(1.0)  # Give more time for mode transition
        
        # Clear any existing output
        ser.reset_input_buffer()
        
        # Send Enter to get menu
        ser.write(b"\n")
        time.sleep(0.5)
        
        # Step 1: Start Download (menu option 1)
        print("\n--- Starting download mode ---")
        if not send_menu_choice(ser, "1"):
            exit_loader_mode(ser)
            return False
        
        # Wait for success message
        response = wait_for_pattern(ser, ["Success\\. Ready to accept", "Failed to start"])
        if not response or "Failed" in response:
            print("\nError: Failed to start download")
            return False
        
        # Step 2: Download image blocks (menu option 2)
        print("\n--- Uploading firmware ---")
        
        with open(filename, 'rb') as f:
            current_offset = offset
            total_sent = 0
            
            while True:
                # Read chunk
                data = f.read(chunk_size)
                if not data:
                    break
                
                chunk_len = len(data)
                print(f"\nSending block: offset=0x{current_offset:X}, length={chunk_len}")
                
                # Send menu choice 2 (with longer timeout after flash writes)
                if not send_menu_choice(ser, "2", timeout=20.0):
                    return False
                
                # Wait for offset prompt
                if not wait_for_pattern(ser, ["Enter offset \\(hex\\):"]):
                    print("\nError: Offset prompt not found")
                    return False
                
                # Send offset in hex format
                ser.write(f"0x{current_offset:X}\n".encode())
                time.sleep(0.1)
                
                # Wait for length prompt
                if not wait_for_pattern(ser, ["Enter length:"]):
                    print("\nError: Length prompt not found")
                    return False
                
                # Send length
                ser.write(f"{chunk_len}\n".encode())
                time.sleep(0.1)
                
                # Wait for data prompt
                if not wait_for_pattern(ser, ["Send block:"]):
                    print("\nError: Send block prompt not found")
                    return False
                
                # Send data
                ser.write(data)
                ser.flush()
                
                # Wait for response (60s timeout for large blocks)
                response = wait_for_pattern(ser, ["Success\\. Block receiv", "Success.*Block", "Failed to download"], timeout=60)
                if not response or "Failed" in response:
                    print("\nError: Failed to download block")
                    return False
                
                current_offset += chunk_len
                total_sent += chunk_len
                
                # Progress indicator
                progress = (total_sent / filesize) * 100
                print(f"Progress: {total_sent}/{filesize} bytes ({progress:.1f}%)")
        
        # Step 3: Complete download (menu option 4)
        print("\n--- Completing download ---")
        if not send_menu_choice(ser, "4"):
            return False
        
        # Wait for completion
        response = wait_for_pattern(ser, ["Success\\. Download complete", "Failed to complete"])
        if not response or "Failed" in response:
            print("\nError: Failed to complete download")
            return False
        
        print("\n--- Firmware upload successful! ---")
        exit_loader_mode(ser)
        return True
        
    except Exception as e:
        print(f"\nError during upload: {e}")
        exit_loader_mode(ser)
        return False
    
    finally:
        ser.close()

def main():
    if len(sys.argv) < 5:
        print("Usage: bootloader_upload.py <port> <baudrate> <firmware.bin> <offset> [chunk_size]")
        print("  port:       Serial port (e.g., /dev/ttyACM0)")
        print("  baudrate:   Baud rate (e.g., 115200)")
        print("  firmware:   Binary file to upload")
        print("  offset:     Start offset in flash (hex format, e.g., 0x8000)")
        print("  chunk_size: Optional chunk size in bytes (default: 16384)")
        sys.exit(1)
    
    port = sys.argv[1]
    baudrate = int(sys.argv[2])
    filename = sys.argv[3]
    offset = int(sys.argv[4], 16)  # Parse as hexadecimal
    chunk_size = int(sys.argv[5]) if len(sys.argv) > 5 else 16384
    
    success = upload_firmware(port, baudrate, filename, offset, chunk_size)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()