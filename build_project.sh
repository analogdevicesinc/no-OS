#!/bin/bash
# Build helper script for unified preset workflow
# Usage: ./build_project.sh <board> <project> [target] [config]

set -e

BOARD=${1:-max78000_fthr}
PROJECT=${2:-iio_demo}
TARGET=${3:-}
CONFIG=${4:-}

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <board> [project] [target] [config]"
    echo ""
    echo "Available boards:"
    echo "  max78000_fthr     - MAX78000 feather board"
    echo "  max32650_fthr     - MAX32650 feather board"
    echo "  apard32690        - APARD32690 board"
    echo "  stm32f756_nucleo  - STM32F756 Nucleo board"
    echo ""
    echo "Available projects:"
    echo "  iio_demo               - IIO Demo project (default)"
    echo "  eval-adxl355-pmdz      - ADXL355 PMDZ project (multiple targets available)"
    echo "  ftc_workshop           - FTC Workshop project"
    echo ""
    echo "ADXL355 targets (when project=eval-adxl355-pmdz):"
    echo "  dummy                  - Dummy example (default)"
    echo "  iio                    - IIO example"
    echo "  iio_trigger            - IIO trigger example"
    echo "  all                    - All examples"
    echo ""
    echo "Examples:"
    echo "  $0 max78000_fthr iio_demo"
    echo "  $0 max32650_fthr eval-adxl355-pmdz dummy"
    echo "  $0 apard32690 eval-adxl355-pmdz iio project.conf"
    echo "  $0 stm32f756_nucleo ftc_workshop"
    exit 1
fi

# Determine build preset based on project and target
if [ "$PROJECT" = "eval-adxl355-pmdz" ]; then
    case "${TARGET:-dummy}" in
        "dummy")
            BUILD_PRESET="eval_adxl355_pmdz_dummy"
            ;;
        "iio")
            BUILD_PRESET="eval_adxl355_pmdz_iio"
            ;;
        "iio_trigger")
            BUILD_PRESET="eval_adxl355_pmdz_iio_trigger"
            ;;
        "all")
            BUILD_PRESET="eval_adxl355_pmdz_all"
            ;;
        *)
            echo "❌ Unknown target '$TARGET' for eval-adxl355-pmdz project"
            echo "Available targets: dummy, iio, iio_trigger, all"
            exit 1
            ;;
    esac
else
    BUILD_PRESET="$PROJECT"
    if [ -n "$TARGET" ]; then
        echo "⚠️  Warning: Target '$TARGET' ignored for project '$PROJECT'"
    fi
fi

echo "🚀 Building project: $PROJECT for board: $BOARD"
if [ -n "$TARGET" ]; then
    echo "   Target: $TARGET"
fi
if [ -n "$CONFIG" ]; then
    echo "   Config: $CONFIG"
fi
echo ""

# Build configure command
CONFIGURE_CMD="cmake -B build --preset $BOARD -DPROJECT_NAME=$PROJECT"
if [ -n "$CONFIG" ]; then
    CONFIGURE_CMD="$CONFIGURE_CMD -DPROJECT_DEFCONFIG=$CONFIG"
fi

# Configure with unified preset
echo "📋 Configuring build..."
eval "$CONFIGURE_CMD"

echo ""
echo "🔨 Building project..."
cmake --build build --preset "$BUILD_PRESET"

echo ""
echo "✅ Build completed successfully!"
echo "   Board: $BOARD"
echo "   Project: $PROJECT"
if [ -n "$TARGET" ]; then
    echo "   Target: $TARGET"
fi
echo "   Build files: build/"