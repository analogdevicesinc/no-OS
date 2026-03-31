#!/bin/bash
# Build script for ARM32 cross-compilation - nanopb CAPI Example

set -e  # Exit on error

echo "=== ARM32 Cross-Compilation Build Script (nanopb CAPI Example) ==="
echo ""

# Check if cross-compiler is installed
if ! command -v arm-linux-gnueabihf-gcc &> /dev/null; then
    echo "Error: ARM32 cross-compiler not found!"
    echo "Install with: sudo apt-get install gcc-arm-linux-gnueabihf"
    exit 1
fi

# Paths
NANOPB_ROOT="/home/ruben/nanopb"
PROJECT_ROOT="/home/ruben/nanopb/examples/capi_example"
BUILD_DIR="${PROJECT_ROOT}/build-arm32"
TRANSPORT_DIR="${PROJECT_ROOT}/transport"

echo "Step 1: Creating build directory..."
mkdir -p "${BUILD_DIR}"

echo ""
echo "Step 2: Generating protobuf files..."

# Generate proto files (need host protoc, not cross-compiled)
if ! command -v protoc &> /dev/null; then
    echo "Error: protoc not found! Install with: apt-get install protoc"
    exit 1
fi

# Use existing generated files if available, otherwise generate
if [ -f "${BUILD_DIR}/capi.pb.c" ]; then
    echo "✓ capi.pb.c already exists"
else
    echo "Generating capi.pb.c/h..."
    cd "${PROJECT_ROOT}"
    protoc --plugin=protoc-gen-nanopb="${NANOPB_ROOT}/generator/protoc-gen-nanopb" \
           --nanopb_out="${BUILD_DIR}" capi.proto
    cd -
fi

if [ -f "${BUILD_DIR}/capi_rpc.c" ]; then
    echo "✓ capi_rpc.c already exists"
else
    echo "Generating capi_rpc.c/h..."
    cd "${PROJECT_ROOT}"
    python3 utils/nanopb_rpc_generator.py capi.proto --output "${BUILD_DIR}/capi_rpc"
    cd -
fi

echo ""
echo "Step 3: Cross-compiling for ARM32..."

# Cross-compile flags - optimized for size and bare metal
ARM32_CC="arm-linux-gnueabihf-gcc"
ARM32_CFLAGS="-Wall -Os -march=armv7-a -mfpu=neon -static"

# Compile client
echo "Compiling client..."
"${ARM32_CC}" ${ARM32_CFLAGS} \
    -I"${NANOPB_ROOT}" \
    -I"${TRANSPORT_DIR}" \
    -I"${PROJECT_ROOT}" \
    -I"${BUILD_DIR}" \
    -I../../../../api \
    -o "${BUILD_DIR}/client" \
    "${PROJECT_ROOT}/main_client.c" \
    "${PROJECT_ROOT}/platform/nanopb/capi_i2c_nanopbClient_driver.c" \
    "${PROJECT_ROOT}/transport/transport.c" \
    "${BUILD_DIR}/capi_rpc.c" \
    "${BUILD_DIR}/capi.pb.c" \
    ../../../../src/capi_i2c.c \
    "${NANOPB_ROOT}/pb_encode.c" \
    "${NANOPB_ROOT}/pb_decode.c" \
    "${NANOPB_ROOT}/pb_common.c"

# Compile server
echo "Compiling server..."
"${ARM32_CC}" ${ARM32_CFLAGS} \
    -I"${NANOPB_ROOT}" \
    -I"${TRANSPORT_DIR}" \
    -I"${PROJECT_ROOT}" \
    -I"${BUILD_DIR}" \
    -I../../../../api \
    -o "${BUILD_DIR}/server" \
    "${PROJECT_ROOT}/server.c" \
    "${PROJECT_ROOT}/platform/linux/capi_i2c_linux_driver.c" \
    "${PROJECT_ROOT}/transport/transport.c" \
    "${BUILD_DIR}/capi_rpc.c" \
    "${BUILD_DIR}/capi.pb.c" \
    ../../../../src/capi_i2c.c \
    "${NANOPB_ROOT}/pb_encode.c" \
    "${NANOPB_ROOT}/pb_decode.c" \
    "${NANOPB_ROOT}/pb_common.c"

echo ""
echo "=== Build Complete! ==="
echo ""
echo "Stripping debug symbols..."
arm-linux-gnueabihf-strip "${BUILD_DIR}/client" "${BUILD_DIR}/server"

echo ""
echo "ARM32 binaries:"
ls -lh "${BUILD_DIR}"/client "${BUILD_DIR}"/server
echo ""
echo "Verify architecture:"
file "${BUILD_DIR}/client"
file "${BUILD_DIR}/server"
echo ""
echo "Copy to ARM device:"
echo "  scp ${BUILD_DIR}/client analog@10.87.54.119:/home/analog/ruben-workspace/nanopb_capi_example"
echo "  scp ${BUILD_DIR}/server analog@10.87.54.119:/home/analog/ruben-workspace/nanopb_capi_example"
echo ""
