To build the libiio libraries that should be linked into this project, we need to do a few steps.
In libiio, use this branch until it gets merged to main:

```
git checkout pcercuei/libtinyiiod
```

We will be cross-compiling so let's create `cross-compile.cmake`  that will tell cmake what compiler to use for cross-compilation with the following contents.
```
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

if (NOT DEFINED CMAKE_SYSROOT)
	set(CMAKE_SYSROOT $ENV{TOOLCHAIN_SYSROOT})
endif()

if (NOT DEFINED CMAKE_FIND_ROOT_PATH)
	set(CMAKE_FIND_ROOT_PATH $ENV{TOOLCHAIN_SYSROOT})
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

set(ENV{PKG_CONFIG_SYSROOT_DIR} $ENV{TOOLCHAIN_SYSROOT})

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_LINKER arm-none-eabi-gcc)

set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)

if (NOT DEFINED CMAKE_INSTALL_PREFIX)
	set(CMAKE_INSTALL_PREFIX /)
endif()
```

Note that this file uses the system's arm-none-eabi-gcc for libiio compilation. Make sure the no-OS project is also built using the exact same compiler, otherwise you may expect weird things to happen (didn't try myself).

```
mkdir build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../cross-compile.cmake ..
```

Now enable/disable stuff with `ccmake ..` so that it looks like this:
```
 AVAHI_CLIENT_LIBRARIES           /lib/libavahi-client.so
 AVAHI_COMMON_LIBRARIES           /lib/libavahi-common.so
 BUILD_SHARED_LIBS                OFF
 CMAKE_BUILD_TYPE                 RelWithDebInfo
 CMAKE_INSTALL_PREFIX             /usr/local
 CMAKE_TOOLCHAIN_FILE             /home/dari/Work/libiio/cross-compile.cmake
 COMPILE_WARNING_AS_ERROR         OFF
 CPP_BINDINGS                     OFF
 CSHARP_BINDINGS                  OFF
 ENABLE_IPV6                      OFF
 ENABLE_PACKAGING                 OFF
 HAVE_DNS_SD                      OFF
 LIBIIO_COMPAT                    OFF
 LIBZSTD_INCLUDE_DIR              LIBZSTD_INCLUDE_DIR-NOTFOUND
 LIBZSTD_LIBRARIES                /lib/libzstd.so
 LOG_LEVEL                        Info
 LibXml2_DIR                      LibXml2_DIR-NOTFOUND
 MAX_LOG_LEVEL                    Debug
 NO_THREADS                       ON
 PTHREAD_LIBRARIES                /lib/libpthread.a
 PYTHON_BINDINGS                  OFF
 WITH_DOC                         OFF
 WITH_EXAMPLES                    OFF
 WITH_EXTERNAL_BACKEND            ON
 WITH_GCOV                        OFF
 WITH_LIBTINYIIOD                 ON
 WITH_MAN                         OFF
 WITH_MODULES                     OFF
 WITH_NETWORK_BACKEND             OFF
 WITH_SERIAL_BACKEND              OFF
 WITH_USB_BACKEND                 OFF
 WITH_UTILS                       OFF
 WITH_XML_BACKEND                 OFF
 WITH_ZSTD                        OFF
```
One additional step here is the requirement to add `-mcpu=cortex-m4` or equivalent flag to the `CMAKE_C_FLAGS` variable under ccmake advanced view, accesible by pressing `T`. Without this flag (which is defined when compiling no-OS projects) the library init will fail with very strange behavior.

Now, compile the `libiio.a`, `libiiod-responder.a` and `libtinyiiod.a` with:
```
make
```

Build the no-OS project as usual which will link to these newly built libraries and create the executable. Run it on the target and verify it's working by doing:
```
iio_info -u serial:/dev/ttyACM0,115200,8n1n
```
