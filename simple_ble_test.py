#!/usr/bin/env python3
"""
Simple BLE Test Script
Quick test to connect to your FTC Workshop BLE device
"""

import asyncio
import sys
from bleak import BleakScanner, BleakClient

async def quick_test():
    print("🔍 Quick BLE Device Test")
    print("=" * 30)

    # Scan for devices
    print("Scanning for BLE devices...")
    devices = await BleakScanner.discover(timeout=8.0)

    if not devices:
        print("❌ No BLE devices found")
        print("Make sure Bluetooth is enabled and devices are advertising")
        return

    # Sort devices by RSSI (signal strength) - strongest signal first

    print(f"\n📡 Found {len(devices)} BLE devices")
    print("🏆 Nearest 5 devices (by signal strength):")
    print("-" * 60)

    periph_device = None
    # for i, device in enumerate(devices):
    #     device_name = device.name or "Unknown Device"

    #     print(f"{i+1}. 📱 {device_name}")
    #     print(f"   📍 {device.address}")
    #     print()

    #     # Check if this is our target device
    #     if device.name and "periph" in device.name.lower():
    #         periph_device = device
    #         print(f"🎯 ⭐ This is your FTC Workshop device! ⭐")
    #         print()

    # if len(devices) > 5:
    #     print(f"... and {len(devices) - 5} more devices")
    #     print()

    # Show all devices with 'periph' in name
    periph_devices = [d for d in devices if d.name and "fit" in d.name.lower()]
    if periph_devices and not periph_device:
        print("🔍 Found Periph-like devices:")
        for device in periph_devices:
            print(f"   🎯 {device.name} - {device.address}")
            periph_device = periph_devices[0]  # Use the first one
        print()

    if not periph_device:
        print("❌ 'Fit' device not found")
        print("\nMake sure:")
        print("- Your device is powered on")
        print("- BLE firmware is running")
        print("- Device is advertising as 'Fit'")
        return

    # Connect and test
    print(f"\n🔗 Connecting to {periph_device.address}...")

    async with BleakClient(periph_device) as client:
        print("✅ Connected!")

        # Perform service discovery first
        print("🔍 Discovering services...")
        services = client.services
        print(f"✅ Discovered {len(list(services))} services")

        # Set up notification handler for step data
        step_data_received = []
        def notification_handler(sender, data):
            try:
                decoded = data.decode('utf-8', errors='ignore')
                print(f"📨 Notification received: {decoded}")
                step_data_received.append(decoded)
            except Exception as e:
                print(f"📨 Notification (raw): {data.hex()}")
                step_data_received.append(data.hex())

        # Try reading without pairing first
        print("📊 Attempting to read characteristics without pairing...")

        # Read battery level first (doesn't require pairing)
        try:
            battery_uuid = "00002a19-0000-1000-8000-00805f9b34fb"
            battery_data = await client.read_gatt_char(battery_uuid)
            battery_level = int.from_bytes(battery_data, byteorder='little')
            print(f"🔋 Battery Level: {battery_level}%")
            battery_read_success = True
        except Exception as e:
            print(f"⚠️  Battery read failed: {e}")
            battery_read_success = False

        # Try pairing only if we couldn't read characteristics
        if not battery_read_success:
            try:
                print("🔐 Pairing with device...")
                if client.is_connected:
                    await client.pair()
                    print("✅ Paired successfully!")
                else:
                    print("Failed to connect first")
            except Exception as e:
                print(f"⚠️  Pairing failed: {e}")
                print("Continuing without pairing...")

        # Read step counting data from Wireless Profile Service
        try:
            print("🔍 Looking for Wireless Profile Service...")
            wp_char = None

            # Find the Wireless Profile Service characteristic
            for service in client.services:
                for char in service.characteristics:
                    # Look for writeable characteristic in WP service
                    if "write" in char.properties:
                        wp_char = char
                        print(f"📡 Found Wireless Profile characteristic: {char.uuid}")
                        break
                if wp_char:
                    break

            if wp_char:
                # Enable notifications if supported
                if "notify" in wp_char.properties:
                    await client.start_notify(wp_char, notification_handler)
                    print(f"🔔 Enabled notifications for {wp_char.uuid}")

                # Send a command to get step data
                command = "get_steps"
                await client.write_gatt_char(wp_char, command.encode())
                print(f"📤 Sent command: {command}")

                # Wait for response (either notification or direct read)
                await asyncio.sleep(1.0)

                # Try to read the response (if characteristic supports read)
                try:
                    if "read" in wp_char.properties:
                        step_data = await client.read_gatt_char(wp_char)
                        step_response = step_data.decode('utf-8', errors='ignore')
                        print(f"🚶 Step Data: {step_response}")

                        # Parse step data if in expected format
                        if "steps:" in step_response:
                            parts = step_response.split(',')
                            for part in parts:
                                if part.startswith('steps:'):
                                    steps = part.split(':')[1]
                                    print(f"👟 Current Steps: {steps}")
                    else:
                        print("📋 Characteristic doesn't support read - waiting for notifications")

                    # Check if we received any notifications
                    if step_data_received:
                        print(f"📊 Total notifications received: {len(step_data_received)}")
                        for i, data in enumerate(step_data_received):
                            print(f"  [{i+1}] {data}")

                except Exception as read_e:
                    print(f"📋 Could not read response: {read_e}")

                # Clean up notifications
                if "notify" in wp_char.properties:
                    await client.stop_notify(wp_char)

            else:
                print("❌ Wireless Profile Service characteristic not found")

        except Exception as e:
            print(f"⚠️  Step counting failed: {e}")

        # List all services
        print(f"\n📋 Available services:")
        for service in client.services:
            print(f"   🔧 {service.uuid} - {service.description}")

        print("\n✅ Test completed successfully!")

if __name__ == "__main__":
    try:
        import bleak
    except ImportError:
        print("❌ Missing bleak library")
        print("Install with: pip3 install bleak")
        sys.exit(1)

    asyncio.run(quick_test())