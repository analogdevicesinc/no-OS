clear
rm -rf build
make reset

export MAXIMSDK_PATH=/home/rbudai/MaximSDK
export PLATFORM=maxim
export TARGET=max32625
export TARGET_REV=0x4132
make -j8
make run