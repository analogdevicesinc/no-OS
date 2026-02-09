#!/bin/bash
# Install xsdb WSL2 fix for Vitis 2025.1
# This fixes the rlwrap segfault that prevents debugging on WSL2
#
# Usage:
#   sudo ./install_xsdb_wsl2_fix.sh [/path/to/Vitis/bin]
#
# This script:
# 1. Backs up the original xsdb
# 2. Replaces it with a wrapper that bypasses rlwrap
# 3. This is a system-wide fix, only needs to be run once
#
# Arguments:
#   $1 - Optional: Path to Vitis bin directory (default: /xilinx/2025.1/Vitis/bin)

# Use provided path or default to standard installation
VITIS_BIN="${1:-/xilinx/2025.1/Vitis/bin}"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "ERROR: This script must be run as root (use sudo)"
    exit 1
fi

# Check if Vitis is installed
if [ ! -d "$VITIS_BIN" ]; then
    echo "ERROR: Vitis 2025.1 not found at $VITIS_BIN"
    echo "Please install Vitis 2025.1 or update the VITIS_BIN path in this script"
    exit 1
fi

# Check if xsdb wrapper exists
if [ ! -f "$SCRIPT_DIR/xsdb-nowrap" ]; then
    echo "ERROR: xsdb-nowrap not found in $SCRIPT_DIR"
    exit 1
fi

echo "Installing xsdb WSL2 fix for Vitis 2025.1..."
echo ""

# Backup original if not already backed up
if [ ! -f "$VITIS_BIN/xsdb.original" ]; then
    echo "Backing up original xsdb..."
    cp "$VITIS_BIN/xsdb" "$VITIS_BIN/xsdb.original"
    echo "  ✓ Original backed up to: $VITIS_BIN/xsdb.original"
else
    echo "  ℹ Original xsdb already backed up (skipping)"
fi

# Install wrapper
echo "Installing xsdb wrapper..."
cp "$SCRIPT_DIR/xsdb-nowrap" "$VITIS_BIN/xsdb"
chmod +x "$VITIS_BIN/xsdb"
echo "  ✓ xsdb wrapper installed"

# Verify installation
echo ""
echo "Verifying installation..."
if [ -f "$VITIS_BIN/xsdb" ] && [ -x "$VITIS_BIN/xsdb" ]; then
    echo "  ✓ xsdb wrapper installed successfully"
else
    echo "  ✗ Error: xsdb wrapper installation failed"
    exit 1
fi

echo ""
echo "===== Installation Complete ====="
echo ""
echo "The xsdb WSL2 fix has been installed system-wide."
echo "Vitis IDE debugging should now work without segfaults on WSL2."
echo ""
echo "To restore original xsdb:"
echo "  sudo cp $VITIS_BIN/xsdb.original $VITIS_BIN/xsdb"
echo ""
