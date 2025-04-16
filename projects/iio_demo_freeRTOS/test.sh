clear
export MAXIM_LIBRARIES=/home/rbudai/MaximSDK/Libraries
export PLATFORM=maxim
export TARGET=max78000
export FREERTOS=y
export FREERTOS_KERNEL=/home/rbudai/Workspace_no_os/FreeRTOS-Kernel
rm -rf build
make -j8 
make run