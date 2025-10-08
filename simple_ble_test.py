#!/usr/bin/env python3
"""
Simple BLE Test Script
Quick test to connect to your FTC Workshop BLE device
"""

import asyncio
import sys
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
        if self.connection_status == "Connected":
            connection_text.append("●", style="bright_green")
            connection_text.append(" CONNECTED", style="bold bright_green")
        elif self.connection_status == "Connecting":
            connection_text.append("●", style="bright_yellow")
            connection_text.append(" CONNECTING", style="bold bright_yellow")
        else:
            connection_text.append("●", style="bright_red")
            connection_text.append(" DISCONNECTED", style="bold bright_red")

        connection_panel = Panel(
            Align.center(connection_text),
            title="🏃 Fit Device Monitor",
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
            title="📱 Device Information",
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
                battery_text = f"🔋 {self.battery_level}"
            except:
                battery_style = "white"
                battery_text = f"🔋 {self.battery_level}"
        else:
            battery_style = "dim"
            battery_text = "🔋 Unknown"

        metrics_table.add_row("Battery Level:", Text(battery_text, style=battery_style))
        metrics_table.add_row("Step Count:", f"👟 {self.step_count}")

        metrics_panel = Panel(
            metrics_table,
            title="📊 Real-time Metrics",
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

    def update_connection_status(self, status, device_name="", device_address=""):
        self.connection_status = status
        self.device_name = device_name
        self.device_address = device_address
        self.last_update = datetime.now().strftime("%H:%M:%S")

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

    # Find fit devices
    fit_devices = [d for d in devices if d.name and "fit" in d.name.lower()]

    if not fit_devices:
        console.print("[bold red] No 'Fit' devices found[/]")
        console.print("\n[yellow]Make sure:[/]")
        console.print("- Your device is powered on")
        console.print("- BLE firmware is running")
        console.print("- Device is advertising as 'Fit'")
        return None

    console.print(f"[bold green] Found {len(fit_devices)} Fit device(s)[/]")
    return fit_devices[0]

async def monitor_fit_device():
    monitor = FitDeviceMonitor()

    # Initial scan
    console.print("[bold cyan] BLE Fit Device Monitor[/]")
    console.print("=" * 50)

    fit_device = await scan_for_fit_device()
    if not fit_device:
        return

    console.print(f"[bold green] Found device: {fit_device.name} ({fit_device.address})[/]")

    # Start the live display
    with Live(monitor.create_layout(), refresh_per_second=2, screen=True) as live:
        try:
            monitor.update_connection_status("Connecting", fit_device.name, fit_device.address)
            live.update(monitor.create_layout())

            async with BleakClient(fit_device) as client:
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

                # Read battery level
                try:
                    battery_uuid = "00002a19-0000-1000-8000-00805f9b34fb"
                    battery_data = await client.read_gatt_char(battery_uuid)
                    battery_level = int.from_bytes(battery_data, byteorder='little')
                    monitor.update_battery(battery_level)
                    live.update(monitor.create_layout())
                except Exception as e:
                    monitor.battery_level = f"Error: {str(e)}"
                    live.update(monitor.create_layout())

                # Start step count notifications
                try:
                    rsc_uuid = "00002a53-0000-1000-8000-00805f9b34fb"
                    await client.start_notify(rsc_uuid, notification_handler)
                except Exception as e:
                    monitor.step_count = f"Error: {str(e)}"
                    live.update(monitor.create_layout())

                # Keep monitoring
                try:
                    while True:
                        # Periodically refresh battery level
                        await asyncio.sleep(30)  # Update every 30 seconds
                        try:
                            battery_data = await client.read_gatt_char(battery_uuid)
                            battery_level = int.from_bytes(battery_data, byteorder='little')
                            monitor.update_battery(battery_level)
                            live.update(monitor.create_layout())
                        except:
                            pass  # Ignore battery read errors during monitoring

                except KeyboardInterrupt:
                    console.print("\n[yellow] Monitoring stopped by user[/]")

        except Exception as e:
            monitor.update_connection_status("Disconnected")
            monitor.battery_level = f"Connection Error"
            monitor.step_count = f"Connection Error"
            live.update(monitor.create_layout())
            await asyncio.sleep(2)  # Show error state briefly
            console.print(f"\n[bold red] Connection failed: {e}[/]")

if __name__ == "__main__":
    try:
        asyncio.run(monitor_fit_device())
    except KeyboardInterrupt:
        console.print("\n[yellow] Goodbye![/]")