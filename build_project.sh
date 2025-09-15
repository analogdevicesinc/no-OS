#!/bin/bash
# Build helper script for unified preset workflow
# Usage: ./build_project.sh <board> <project>

set -e

BOARD=${1:-max78000_fthr}
PROJECT=${2:-iio_demo}

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <board> [project]"
    echo ""
    echo "Available boards:"
    echo "  max78000_fthr     - MAX78000 feather board"
    echo "  max32650_fthr     - MAX32650 feather board"
    echo "  apard32690        - APARD32690 board"
    echo "  stm32f756_nucleo  - STM32F756 Nucleo board"
    echo ""
    echo "Available projects:"
    echo "  iio_demo          - IIO Demo project (default)"
    echo "  eval_adxl355_pmdz  - ADXL355 PMDZ project"
    echo "  ftc_workshop      - FTC Workshop project"
    echo ""
    echo "Examples:"
    echo "  $0 max78000_fthr iio_demo"
    echo "  $0 max32650_fthr eval_adxl355_pmdz"
    exit 1
fi

echo "🚀 Building project: $PROJECT for board: $BOARD"
echo ""

# Configure with unified preset
echo "📋 Configuring build..."
cmake -B build --preset "$BOARD" -DPROJECT_NAME="$PROJECT"

echo ""
echo "🔨 Building project..."
cmake --build build --preset "$PROJECT"

echo ""
echo "✅ Build completed successfully!"
echo "   Board: $BOARD"
echo "   Project: $PROJECT"
echo "   Build files: build/"