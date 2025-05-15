
clear
rm -rf build
# make reset

find . \( -name '*.c' -o -name '*.h' \) -exec astyle --style=linux -n {} +

export MAXIM_LIBRARIES=/home/rbudai/MaximSDK/Libraries
export PLATFORM=maxim
export TARGET=max32662

make -j8
make run