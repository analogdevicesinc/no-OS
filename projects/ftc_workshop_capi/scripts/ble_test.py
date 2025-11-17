#!/usr/bin/env python3
"""
Simple BLE Test Script
Quick test to connect to your FTC Workshop BLE device
"""

import asyncio
import sys
import os
import time
from datetime import datetime
from bleak import BleakScanner, BleakClient

# Check for required libraries
try:
    from rich.console import Console
    from rich.panel import Panel
    from rich.layout import Layout
    from rich.live import Live
    from rich.table import Table
    from rich.text import Text
    from rich.spinner import Spinner
    from rich.progress import Progress, SpinnerColumn, TextColumn
    from rich.align import Align
    import bleak
except ImportError as e:
    print(f"❌ Missing required library: {e}")
    print("Install with: pip3 install bleak rich")
    sys.exit(1)

console = Console()

class FitDeviceMonitor:
    def __init__(self):
        self.battery_level = "Unknown"
        self.step_count = "Unknown"
        self.connection_status = "Disconnected"
        self.device_address = ""
        self.device_name = ""
        self.last_update = ""
        self.services_count = 0

    def create_layout(self):
        # Create the main layout
        layout = Layout()

        # Split into header and body
        layout.split_row(
            Layout(name="main")
        )

        # Create connection status panel
        connection_text = Text()
        if "Connected" in self.connection_status:
            connection_text.append("●", style="bright_green")
            connection_text.append(f" {self.connection_status.upper()}", style="bold bright_green")
        elif "Connecting" in self.connection_status:
            connection_text.append("●", style="bright_yellow")
            connection_text.append(f" {self.connection_status.upper()}", style="bold bright_yellow")
        elif "Reconnecting" in self.connection_status:
            connection_text.append("●", style="bright_blue")
            connection_text.append(f" {self.connection_status.upper()}", style="bold bright_blue")
        else:
            connection_text.append("●", style="bright_red")
            connection_text.append(f" {self.connection_status.upper()}", style="bold bright_red")

        connection_panel = Panel(
            Align.center(connection_text),
            title="Step Counter Device",
            border_style="blue",
            height=3
        )

        # Create device info table
        device_table = Table(show_header=False, box=None, pad_edge=False)
        device_table.add_column("Label", style="cyan", width=15)
        device_table.add_column("Value", style="white")

        device_table.add_row("Device Name:", self.device_name or "Unknown")
        device_table.add_row("Address:", self.device_address or "Unknown")
        device_table.add_row("Services:", str(self.services_count) if self.services_count else "Unknown")
        device_table.add_row("Last Update:", self.last_update or "Never")

        device_panel = Panel(
            device_table,
            title="Device Information",
            border_style="magenta",
            height=8
        )

        # Create metrics table
        metrics_table = Table(show_header=False, box=None, pad_edge=False)
        metrics_table.add_column("Metric", style="cyan", width=15)
        metrics_table.add_column("Value", style="bold white")

        # Format battery level with color coding
        if self.battery_level != "Unknown":
            try:
                battery_val = int(self.battery_level.replace('%', ''))
                if battery_val > 50:
                    battery_style = "bright_green"
                elif battery_val > 20:
                    battery_style = "yellow"
                else:
                    battery_style = "bright_red"
                battery_text = f"{self.battery_level}"
            except:
                battery_style = "white"
                battery_text = f"{self.battery_level}"
        else:
            battery_style = "dim"
            battery_text = "Unknown"

        metrics_table.add_row("Battery Level:", Text(battery_text, style=battery_style))
        metrics_table.add_row("Step Count:", f"{self.step_count}")

        metrics_panel = Panel(
            metrics_table,
            title="Real-time Metrics",
            border_style="green",
            height=6
        )

        # Combine panels vertically
        layout.split_column(
            Layout(connection_panel, name="status", size=3),
            Layout(device_panel, name="device", size=8),
            Layout(metrics_panel, name="metrics", size=6)
        )

        return layout

    def update_connection_status(self, status, device_name="", device_address="", extra_info=""):
        self.connection_status = status
        if device_name:
            self.device_name = device_name
        if device_address:
            self.device_address = device_address
        self.last_update = datetime.now().strftime("%H:%M:%S")
        if extra_info:
            self.connection_status = f"{status} ({extra_info})"

    def update_battery(self, level):
        self.battery_level = f"{level}%"
        self.last_update = datetime.now().strftime("%H:%M:%S")

    def update_steps(self, steps):
        self.step_count = str(steps)
        self.last_update = datetime.now().strftime("%H:%M:%S")

    def set_services_count(self, count):
        self.services_count = count

async def scan_for_fit_device():
    with console.status("[bold blue]Scanning for BLE devices...", spinner="dots"):
        devices = await BleakScanner.discover(timeout=8.0)

    if not devices:
        console.print("[bold red] No BLE devices found[/]")
        console.print("Make sure Bluetooth is enabled and devices are advertising")
        return None

    adv_name = os.environ['BLE_ADV_NAME']

    print("adv name = {}", adv_name)
    for d in devices:
        print("Found ", d.name)
    # Find the device
    fit_devices = [d for d in devices if d.name and adv_name.lower() == d.name.lower()]

    if not fit_devices:
        console.print("[bold red] No 'Fit' devices found[/]")
        console.print("\n[yellow]Make sure:[/]")
        console.print("- Your device is powered on")
        console.print("- BLE firmware is running")
        console.print("- Device is advertising as 'Fit'")
        return None

    console.print(f"[bold green] Found {len(fit_devices)} Fit device(s)[/]")
    return fit_devices[0]

async def connect_and_monitor(monitor, live, fit_device):
    """Connect to device and handle monitoring with connection state checks"""
    battery_uuid = "00002a19-0000-1000-8000-00805f9b34fb"
    rsc_uuid = "00002a53-0000-1000-8000-00805f9b34fb"

    try:
        monitor.update_connection_status("Connecting", fit_device.name, fit_device.address)
        live.update(monitor.create_layout())

        async with BleakClient(fit_device) as client:
            # Wait for connection to be established
            await asyncio.sleep(0.5)

            if not client.is_connected:
                raise Exception("Failed to establish connection")

            monitor.update_connection_status("Connected", fit_device.name, fit_device.address)

            # Discover services
            services = client.services
            monitor.set_services_count(len(list(services)))
            live.update(monitor.create_layout())

            # Set up notification handler for step data
            def notification_handler(sender, data):
                try:
                    steps = int.from_bytes(data[2:4], byteorder='big', signed=False)
                    monitor.update_steps(steps)
                    live.update(monitor.create_layout())
                except Exception as e:
                    # If step parsing fails, just show raw data
                    monitor.update_steps(f"Raw: {data.hex()}")
                    live.update(monitor.create_layout())

            # Read initial battery level
            try:
                battery_data = await client.read_gatt_char(battery_uuid)
                battery_level = int.from_bytes(battery_data, byteorder='little')
                monitor.update_battery(battery_level)
                live.update(monitor.create_layout())
            except Exception as e:
                monitor.battery_level = f"Error: {str(e)}"
                live.update(monitor.create_layout())

            # Start step count notifications
            try:
                await client.start_notify(rsc_uuid, notification_handler)
            except Exception as e:
                monitor.step_count = f"Notify Error: {str(e)}"
                live.update(monitor.create_layout())

            # Main monitoring loop with connection state checks
            connection_check_counter = 0
            last_successful_read = asyncio.get_event_loop().time()
            read_timeout_threshold = 15  # seconds - if no successful read in 15s, assume connection dead
            consecutive_failures = 0
            max_consecutive_failures = 3

            while True:
                current_time = asyncio.get_event_loop().time()

                # Check connection state every few iterations
                if connection_check_counter % 3 == 0:  # Every 9 seconds
                    if not client.is_connected:
                        raise Exception("Connection lost - client reports disconnected")

                # Check for read timeout (no successful operations for too long)
                if current_time - last_successful_read > read_timeout_threshold:
                    raise Exception(f"Connection timeout - no successful reads for {read_timeout_threshold}s")

                # Periodically refresh battery level (also serves as connection keep-alive)
                await asyncio.sleep(3)

                try:
                    # Add timeout to the read operation to prevent hanging
                    battery_data = await asyncio.wait_for(
                        client.read_gatt_char(battery_uuid),
                        timeout=5.0  # 5 second timeout for read operation
                    )
                    battery_level = int.from_bytes(battery_data, byteorder='little')
                    monitor.update_battery(battery_level)
                    live.update(monitor.create_layout())

                    # Reset failure tracking on success
                    consecutive_failures = 0
                    last_successful_read = current_time
                    connection_check_counter += 1

                except asyncio.TimeoutError:
                    consecutive_failures += 1
                    raise Exception(f"Read operation timed out (attempt {consecutive_failures})")

                except Exception as read_error:
                    consecutive_failures += 1

                    # If we have too many consecutive failures, assume connection is dead
                    if consecutive_failures >= max_consecutive_failures:
                        raise Exception(f"Too many consecutive read failures ({consecutive_failures}): {read_error}")

                    # Battery read failed - check connection state
                    if not client.is_connected:
                        raise Exception(f"Connection lost during battery read: {read_error}")
                    else:
                        # Just a read error, continue monitoring but track failures
                        connection_check_counter += 1
                        # Don't update last_successful_read on failure

    except KeyboardInterrupt:
        raise  # Re-raise to be handled by caller
    except Exception as e:
        # Connection failed or was lost
        raise Exception(f"Connection error: {str(e)}")


async def monitor_fit_device():
    monitor = FitDeviceMonitor()
    max_reconnect_attempts = 10
    reconnect_delay = 5  # seconds

    # Initial scan
    console.print("[bold cyan] BLE Fit Device Monitor with Auto-Reconnect[/]")
    console.print("=" * 60)

    fit_device = await scan_for_fit_device()
    if not fit_device:
        return

    console.print(f"[bold green] Found device: {fit_device.name} ({fit_device.address})[/]")

    # Start the live display
    with Live(monitor.create_layout(), refresh_per_second=2, screen=True) as live:
        attempt = 1

        while attempt <= max_reconnect_attempts:
            try:
                if attempt > 1:
                    console.print(f"\n[bold yellow] Reconnection attempt {attempt}/{max_reconnect_attempts}[/]")

                # Attempt to connect and monitor
                await connect_and_monitor(monitor, live, fit_device)

            except KeyboardInterrupt:
                console.print("\n[yellow] Monitoring stopped by user[/]")
                break

            except Exception as e:
                monitor.update_connection_status("Disconnected", extra_info=f"Attempt {attempt}")
                monitor.battery_level = "Connection Lost"
                monitor.step_count = "Connection Lost"
                live.update(monitor.create_layout())

                if attempt >= max_reconnect_attempts:
                    console.print(f"\n[bold red] Max reconnection attempts reached. Final error: {e}[/]")
                    break

                console.print(f"\n[bold red] Connection error: {e}[/]")
                console.print(f"[yellow] Waiting {reconnect_delay} seconds before reconnection attempt {attempt + 1}...[/]")

                # Show reconnecting status during wait
                monitor.update_connection_status("Reconnecting", extra_info=f"in {reconnect_delay}s")
                live.update(monitor.create_layout())

                # Wait before reconnecting with countdown
                for i in range(reconnect_delay, 0, -1):
                    monitor.update_connection_status("Reconnecting", extra_info=f"in {i}s")
                    live.update(monitor.create_layout())
                    await asyncio.sleep(1)

                attempt += 1

                # Optionally rescan for device in case it moved or restarted
                if attempt % 3 == 0:  # Every 3rd attempt, rescan
                    console.print("[cyan] Rescanning for device...[/]")
                    new_device = await scan_for_fit_device()
                    if new_device:
                        fit_device = new_device
                        console.print(f"[green] Device found at: {fit_device.address}[/]")

        if attempt > max_reconnect_attempts:
            console.print(f"\n[bold red] Unable to maintain connection after {max_reconnect_attempts} attempts[/]")
            console.print("[yellow] Please check:[/]")
            console.print("- Device is powered on and in range")
            console.print("- BLE firmware is running properly")
            console.print("- No interference from other devices")

if __name__ == "__main__":
    try:
        asyncio.run(monitor_fit_device())
    except KeyboardInterrupt:
        console.print("\n[yellow] Goodbye![/]")
