clear
export MAXIM_LIBRARIES=/home/rbudai/MaximSDK/Libraries
export PLATFORM=maxim
export TARGET=max78000
rm -rf build
make -j8 
make run