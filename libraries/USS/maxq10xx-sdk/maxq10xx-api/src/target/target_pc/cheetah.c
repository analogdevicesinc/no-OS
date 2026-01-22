/*=========================================================================
| Cheetah Interface Library
|--------------------------------------------------------------------------
| Copyright (c) 2004-2008 Total Phase, Inc.
| All rights reserved.
| www.totalphase.com
|
| Redistribution and use in source and binary forms, with or without
| modification, are permitted provided that the following conditions
| are met:
|
| - Redistributions of source code must retain the above copyright
|   notice, this list of conditions and the following disclaimer.
|
| - Redistributions in binary form must reproduce the above copyright
|   notice, this list of conditions and the following disclaimer in the
|   documentation and/or other materials provided with the distribution.
|
| - Neither the name of Total Phase, Inc. nor the names of its
|   contributors may be used to endorse or promote products derived from
|   this software without specific prior written permission.
|
| THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
| "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
| LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
| FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
| COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
| INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
| BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
| LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
| CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
| LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
| ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
| POSSIBILITY OF SUCH DAMAGE.
|--------------------------------------------------------------------------
| To access Total Phase Cheetah devices through the API:
|
| 1) Use one of the following shared objects:
|      cheetah.so       --  Linux shared object
|          or
|      cheetah.dll      --  Windows dynamic link library
|
| 2) Along with one of the following language modules:
|      cheetah.c/h      --  C/C++ API header file and interface module
|      cheetah_py.py    --  Python API
|      cheetah.bas      --  Visual Basic 6 API
|      cheetah.cs       --  C# .NET source
|      cheetah_net.dll  --  Compiled .NET binding
 ========================================================================*/
typedef int make_iso_compilers_happy;

#if ((PC_INTERFACE==CHEETAH) && (defined(USE_PROTOCOL_I2C) || defined(USE_PROTOCOL_SPI)))


/*=========================================================================
| INCLUDES
 ========================================================================*/
/* This #include can be customized to conform to the user's build paths. */
#include "cheetah.h"


/*=========================================================================
| VERSION CHECK
 ========================================================================*/
#define CH_CFILE_VERSION   0x0306   /* v3.06 */
#define CH_REQ_SW_VERSION  0x0306   /* v3.06 */

/*
 * Make sure that the header file was included and that
 * the version numbers match.
 */
#ifndef CH_HEADER_VERSION
#  error Unable to include header file. Please check include path.

#elif CH_HEADER_VERSION != CH_CFILE_VERSION
#  error Version mismatch between source and header files.

#endif


/*=========================================================================
| DEFINES
 ========================================================================*/
#define API_NAME                     "cheetah"
#define API_DEBUG                    CH_DEBUG
#define API_OK                       CH_OK
#define API_UNABLE_TO_LOAD_LIBRARY   CH_UNABLE_TO_LOAD_LIBRARY
#define API_INCOMPATIBLE_LIBRARY     CH_INCOMPATIBLE_LIBRARY
#define API_UNABLE_TO_LOAD_FUNCTION  CH_UNABLE_TO_LOAD_FUNCTION
#define API_HEADER_VERSION           CH_HEADER_VERSION
#define API_REQ_SW_VERSION           CH_REQ_SW_VERSION


/*=========================================================================
| LINUX AND DARWIN SUPPORT
 ========================================================================*/
#if defined(__APPLE_CC__) && !defined(DARWIN)
#define DARWIN
#endif

#if defined(linux) || defined(DARWIN)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#ifdef DARWIN
#define DLOPEN_NO_WARN
extern int _NSGetExecutablePath (char *buf, unsigned long *bufsize);
#endif

#include <dlfcn.h>

#define DLL_HANDLE  void *
#define MAX_SO_PATH 256

static char SO_NAME[MAX_SO_PATH+1] = "lib" API_NAME ".so";

/*
 * These functions allow the Linux behavior to emulate
 * the Windows behavior as specified below in the Windows
 * support section.
 * 
 * First, search for the shared object in the application
 * binary path, then in the current working directory.
 * 
 * Searching the application binary path requires /proc
 * filesystem support, which is standard in 2.4.x kernels.
 * 
 * If the /proc filesystem is not present, the shared object
 * will not be loaded from the execution path unless that path
 * is either the current working directory or explicitly
 * specified in LD_LIBRARY_PATH.
 */
static int _checkPath (const char *path) {
    char *filename = (char *)malloc(strlen(path) +1 + strlen(SO_NAME) +1);
    int   fd;

    // Check if the file is readable
    sprintf(filename, "%s/%s", path, SO_NAME);
    fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        strncpy(SO_NAME, filename, MAX_SO_PATH);
        close(fd);
    }

    // Clean up and exit
    free(filename);
    return (fd >= 0);
}

static int _getExecPath (char *path, unsigned long maxlen) {
#ifdef linux
    return readlink("/proc/self/exe", path, maxlen);
#endif

#ifdef DARWIN
    _NSGetExecutablePath(path, &maxlen);
    return maxlen;
#endif
}

static void _setSearchPath (void) {
    char  path[MAX_SO_PATH+1];
    int   count;
    char *p;

    /* Make sure that SO_NAME is not an absolute path. */
    if (SO_NAME[0] == '/')  return;

    /* Check the execution directory name. */
    memset(path, 0, sizeof(path));
    count = _getExecPath(path, MAX_SO_PATH);

    if (count > 0) {
        char *pp = strrchr(path, '/');
        if (pp == path)  ++pp;
        if (pp != 0)     *pp = '\0';

        /* If there is a match, return immediately. */
        if (_checkPath(path))  return;
    }

    /* Check the current working directory. */
    p = getcwd(path, MAX_SO_PATH);
    if (p != 0)  _checkPath(path);
}

#endif


/*=========================================================================
| WINDOWS SUPPORT
 ========================================================================*/
#if defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)

#include <stdio.h>
#include <windows.h>

#define DLL_HANDLE           HINSTANCE
#define dlopen(name, flags)  LoadLibraryA(name)
#define dlsym(handle, name)  GetProcAddress(handle, name)
#define dlerror()            "Exiting program"
#define SO_NAME              API_NAME ".dll"

/*
 * Use the default Windows DLL loading rules:
 *   1.  The directory from which the application binary was loaded.
 *   2.  The application's current directory.
 *   3a. [Windows NT/2000/XP only] 32-bit system directory
 *       (default: c:\winnt\System32)
 *   3b. 16-bit system directory
 *       (default: c:\winnt\System or c:\windows\system)
 *   4.  The windows directory
 *       (default: c:\winnt or c:\windows)
 *   5.  The directories listed in the PATH environment variable
 */
static void _setSearchPath (void) {
    /* Do nothing */
}

#endif


/*=========================================================================
| SHARED LIBRARY LOADER
 ========================================================================*/
/* The error conditions can be customized depending on the application. */
static void *_loadFunction (const char *name, int *result) {
    static DLL_HANDLE handle = 0;
    void * function = 0;

    /* Load the shared library if necessary */
    if (handle == 0) {
        u32 (*version) (void);
        u16 sw_version;
        u16 api_version_req;

        _setSearchPath();
        handle = dlopen(SO_NAME, RTLD_LAZY);
        if (handle == 0) {
#if API_DEBUG
            fprintf(stderr, "Unable to load %s\n", SO_NAME);
            fprintf(stderr, "%s\n", dlerror());
#endif
            *result = API_UNABLE_TO_LOAD_LIBRARY;
            return 0;
        }

        #pragma GCC diagnostic push    //Save actual diagnostics state
        #pragma GCC diagnostic ignored "-Wpedantic"    //Disable pedantic
        *(void**)(&version) = dlsym(handle, "ch_c_version");
        #pragma GCC diagnostic pop    //Save actual diagnostics state

        if (version == 0) {
#if API_DEBUG
            fprintf(stderr, "Unable to bind ch_c_version() in %s\n",
                    SO_NAME);
            fprintf(stderr, "%s\n", dlerror());
#endif
            handle  = 0;
            *result = API_INCOMPATIBLE_LIBRARY;
            return 0;
        }

        sw_version      = (u16)((version() >>  0) & 0xffff);
        api_version_req = (u16)((version() >> 16) & 0xffff);
        if (sw_version  < API_REQ_SW_VERSION ||
            API_HEADER_VERSION < api_version_req)
        {
#if API_DEBUG
            fprintf(stderr, "\nIncompatible versions:\n");

            fprintf(stderr, "  Header version  = v%d.%02d  ",
                    (API_HEADER_VERSION >> 8) & 0xff, API_HEADER_VERSION & 0xff);

            if (sw_version < API_REQ_SW_VERSION)
                fprintf(stderr, "(requires library >= %d.%02d)\n",
                        (API_REQ_SW_VERSION >> 8) & 0xff,
                        API_REQ_SW_VERSION & 0xff);
            else
                fprintf(stderr, "(library version OK)\n");
                        
                   
            fprintf(stderr, "  Library version = v%d.%02d  ",
                    (sw_version >> 8) & 0xff,
                    (sw_version >> 0) & 0xff);

            if (API_HEADER_VERSION < api_version_req)
                fprintf(stderr, "(requires header >= %d.%02d)\n",
                        (api_version_req >> 8) & 0xff,
                        (api_version_req >> 0) & 0xff);
            else
                fprintf(stderr, "(header version OK)\n");
#endif
            handle  = 0;
            *result = API_INCOMPATIBLE_LIBRARY;
            return 0;
        }
    }

    /* Bind the requested function in the shared library */
    #pragma GCC diagnostic push    //Save actual diagnostics state
    #pragma GCC diagnostic ignored "-Wpedantic"    //Disable pedantic
    function = (void *)dlsym(handle, name);
    #pragma GCC diagnostic pop    //Save actual diagnostics state
    *result  = function ? API_OK : API_UNABLE_TO_LOAD_FUNCTION;
    return function;
}


/*=========================================================================
| FUNCTIONS
 ========================================================================*/
static int (*c_ch_find_devices) (int, u16 *) = 0;
int ch_find_devices (
    int   num_devices,
    u16 * devices
)
{
    if (c_ch_find_devices == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_find_devices) = _loadFunction("c_ch_find_devices", &res)))
            return res;
    }
    return c_ch_find_devices(num_devices, devices);
}


static int (*c_ch_find_devices_ext) (int, u16 *, int, u32 *) = 0;
int ch_find_devices_ext (
    int   num_devices,
    u16 * devices,
    int   num_ids,
    u32 * unique_ids
)
{
    if (c_ch_find_devices_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_find_devices_ext) = _loadFunction("c_ch_find_devices_ext", &res)))
            return res;
    }
    return c_ch_find_devices_ext(num_devices, devices, num_ids, unique_ids);
}


static Cheetah (*c_ch_open) (int) = 0;
Cheetah ch_open (
    int port_number
)
{
    if (c_ch_open == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_open) = _loadFunction("c_ch_open", &res)))
            return res;
    }
    return c_ch_open(port_number);
}


static Cheetah (*c_ch_open_ext) (int, CheetahExt *) = 0;
Cheetah ch_open_ext (
    int          port_number,
    CheetahExt * ch_ext
)
{
    if (c_ch_open_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_open_ext) = _loadFunction("c_ch_open_ext", &res)))
            return res;
    }
    return c_ch_open_ext(port_number, ch_ext);
}


static int (*c_ch_close) (Cheetah) = 0;
int ch_close (
    Cheetah cheetah
)
{
    if (c_ch_close == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_close) = _loadFunction("c_ch_close", &res)))
            return res;
    }
    return c_ch_close(cheetah);
}


static int (*c_ch_port) (Cheetah) = 0;
int ch_port (
    Cheetah cheetah
)
{
    if (c_ch_port == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_port) = _loadFunction("c_ch_port", &res)))
            return res;
    }
    return c_ch_port(cheetah);
}


static u32 (*c_ch_unique_id) (Cheetah) = 0;
u32 ch_unique_id (
    Cheetah cheetah
)
{
    if (c_ch_unique_id == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_unique_id) = _loadFunction("c_ch_unique_id", &res)))
            return res;
    }
    return c_ch_unique_id(cheetah);
}


static const char * (*c_ch_status_string) (int) = 0;
const char * ch_status_string (
    int status
)
{
    if (c_ch_status_string == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_status_string) = _loadFunction("c_ch_status_string", &res)))
            return 0;
    }
    return c_ch_status_string(status);
}


static int (*c_ch_version) (Cheetah, CheetahVersion *) = 0;
int ch_version (
    Cheetah          cheetah,
    CheetahVersion * version
)
{
    if (c_ch_version == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_version) = _loadFunction("c_ch_version", &res)))
            return res;
    }
    return c_ch_version(cheetah, version);
}


static u32 (*c_ch_sleep_ms) (u32) = 0;
u32 ch_sleep_ms (
    u32 milliseconds
)
{
    if (c_ch_sleep_ms == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_sleep_ms) = _loadFunction("c_ch_sleep_ms", &res)))
            return res;
    }
    return c_ch_sleep_ms(milliseconds);
}


static int (*c_ch_target_power) (Cheetah, u08) = 0;
int ch_target_power (
    Cheetah cheetah,
    u08     power_flag
)
{
    if (c_ch_target_power == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_target_power) = _loadFunction("c_ch_target_power", &res)))
            return res;
    }
    return c_ch_target_power(cheetah, power_flag);
}


static int (*c_ch_host_ifce_speed) (Cheetah) = 0;
int ch_host_ifce_speed (
    Cheetah cheetah
)
{
    if (c_ch_host_ifce_speed == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_host_ifce_speed) = _loadFunction("c_ch_host_ifce_speed", &res)))
            return res;
    }
    return c_ch_host_ifce_speed(cheetah);
}


static int (*c_ch_dev_addr) (Cheetah) = 0;
int ch_dev_addr (
    Cheetah cheetah
)
{
    if (c_ch_dev_addr == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_dev_addr) = _loadFunction("c_ch_dev_addr", &res)))
            return res;
    }
    return c_ch_dev_addr(cheetah);
}


static int (*c_ch_spi_bitrate) (Cheetah, int) = 0;
int ch_spi_bitrate (
    Cheetah cheetah,
    int     bitrate_khz
)
{
    if (c_ch_spi_bitrate == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_bitrate) = _loadFunction("c_ch_spi_bitrate", &res)))
            return res;
    }
    return c_ch_spi_bitrate(cheetah, bitrate_khz);
}


static int (*c_ch_spi_configure) (Cheetah, CheetahSpiPolarity, CheetahSpiPhase, CheetahSpiBitorder, u08) = 0;
int ch_spi_configure (
    Cheetah            cheetah,
    CheetahSpiPolarity polarity,
    CheetahSpiPhase    phase,
    CheetahSpiBitorder bitorder,
    u08                ss_polarity
)
{
    if (c_ch_spi_configure == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_configure) = _loadFunction("c_ch_spi_configure", &res)))
            return res;
    }
    return c_ch_spi_configure(cheetah, polarity, phase, bitorder, ss_polarity);
}


static int (*c_ch_spi_queue_clear) (Cheetah) = 0;
int ch_spi_queue_clear (
    Cheetah cheetah
)
{
    if (c_ch_spi_queue_clear == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_clear) = _loadFunction("c_ch_spi_queue_clear", &res)))
            return res;
    }
    return c_ch_spi_queue_clear(cheetah);
}


static int (*c_ch_spi_queue_oe) (Cheetah, u08) = 0;
int ch_spi_queue_oe (
    Cheetah cheetah,
    u08     oe
)
{
    if (c_ch_spi_queue_oe == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_oe) = _loadFunction("c_ch_spi_queue_oe", &res)))
            return res;
    }
    return c_ch_spi_queue_oe(cheetah, oe);
}


static int (*c_ch_spi_queue_delay_cycles) (Cheetah, int) = 0;
int ch_spi_queue_delay_cycles (
    Cheetah cheetah,
    int     cycles
)
{
    if (c_ch_spi_queue_delay_cycles == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_delay_cycles) = _loadFunction("c_ch_spi_queue_delay_cycles", &res)))
            return res;
    }
    return c_ch_spi_queue_delay_cycles(cheetah, cycles);
}


static int (*c_ch_spi_queue_delay_ns) (Cheetah, int) = 0;
int ch_spi_queue_delay_ns (
    Cheetah cheetah,
    int     nanoseconds
)
{
    if (c_ch_spi_queue_delay_ns == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_delay_ns) = _loadFunction("c_ch_spi_queue_delay_ns", &res)))
            return res;
    }
    return c_ch_spi_queue_delay_ns(cheetah, nanoseconds);
}


static int (*c_ch_spi_queue_ss) (Cheetah, u08) = 0;
int ch_spi_queue_ss (
    Cheetah cheetah,
    u08     active
)
{
    if (c_ch_spi_queue_ss == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_ss) = _loadFunction("c_ch_spi_queue_ss", &res)))
            return res;
    }
    return c_ch_spi_queue_ss(cheetah, active);
}


static int (*c_ch_spi_queue_byte) (Cheetah, int, u08) = 0;
int ch_spi_queue_byte (
    Cheetah cheetah,
    int     count,
    u08     data
)
{
    if (c_ch_spi_queue_byte == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_byte) = _loadFunction("c_ch_spi_queue_byte", &res)))
            return res;
    }
    return c_ch_spi_queue_byte(cheetah, count, data);
}


static int (*c_ch_spi_queue_array) (Cheetah, int, const u08 *) = 0;
int ch_spi_queue_array (
    Cheetah     cheetah,
    int         num_bytes,
    const u08 * data_out
)
{
    if (c_ch_spi_queue_array == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_queue_array) = _loadFunction("c_ch_spi_queue_array", &res)))
            return res;
    }
    return c_ch_spi_queue_array(cheetah, num_bytes, data_out);
}


static int (*c_ch_spi_batch_length) (Cheetah) = 0;
int ch_spi_batch_length (
    Cheetah cheetah
)
{
    if (c_ch_spi_batch_length == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_batch_length) = _loadFunction("c_ch_spi_batch_length", &res)))
            return res;
    }
    return c_ch_spi_batch_length(cheetah);
}


static int (*c_ch_spi_batch_shift) (Cheetah, int, u08 *) = 0;
int ch_spi_batch_shift (
    Cheetah cheetah,
    int     num_bytes,
    u08 *   data_in
)
{
    if (c_ch_spi_batch_shift == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_batch_shift) = _loadFunction("c_ch_spi_batch_shift", &res)))
            return res;
    }
    return c_ch_spi_batch_shift(cheetah, num_bytes, data_in);
}


static int (*c_ch_spi_async_submit) (Cheetah) = 0;
int ch_spi_async_submit (
    Cheetah cheetah
)
{
    if (c_ch_spi_async_submit == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_async_submit) = _loadFunction("c_ch_spi_async_submit", &res)))
            return res;
    }
    return c_ch_spi_async_submit(cheetah);
}


static int (*c_ch_spi_async_collect) (Cheetah, int, u08 *) = 0;
int ch_spi_async_collect (
    Cheetah cheetah,
    int     num_bytes,
    u08 *   data_in
)
{
    if (c_ch_spi_async_collect == 0) {
        int res = 0;
        if (!(*(void**)(&c_ch_spi_async_collect) = _loadFunction("c_ch_spi_async_collect", &res)))
            return res;
    }
    return c_ch_spi_async_collect(cheetah, num_bytes, data_in);
}


#endif
