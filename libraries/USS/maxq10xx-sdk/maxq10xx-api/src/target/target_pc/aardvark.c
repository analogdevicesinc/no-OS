/*=========================================================================
| Aardvark Interface Library
|--------------------------------------------------------------------------
| Copyright (c) 2002-2008 Total Phase, Inc.
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
| To access Aardvark devices through the API:
|
| 1) Use one of the following shared objects:
|      aardvark.so      --  Linux shared object
|      aardvark.dll     --  Windows dynamic link library
|
| 2) Along with one of the following language modules:
|      aardvark.c/h     --  C/C++ API header file and interface module
|      aardvark_py.py   --  Python API
|      aardvark.bas     --  Visual Basic 6 API
|      aardvark.cs      --  C# .NET source
|      aardvark_net.dll --  Compiled .NET binding
 ========================================================================*/
typedef int make_iso_compilers_happy;

#if ((PC_INTERFACE==AARDVARK) && (defined(USE_PROTOCOL_I2C) || defined(USE_PROTOCOL_SPI)))

/*=========================================================================
| INCLUDES
 ========================================================================*/
/* This #include can be customized to conform to the user's build paths. */
#include "aardvark.h"

/* #include <dlfcn.h> */
/* #define WIN32 */

/*=========================================================================
| VERSION CHECK
 ========================================================================*/
#define AA_CFILE_VERSION   0x050a   /* v5.10 */
#define AA_REQ_SW_VERSION  0x050a   /* v5.10 */

/*
 * Make sure that the header file was included and that
 * the version numbers match.
 */
#ifndef AA_HEADER_VERSION
#  error Unable to include header file. Please check include path.

#elif AA_HEADER_VERSION != AA_CFILE_VERSION
#  error Version mismatch between source and header files.

#endif


/*=========================================================================
| DEFINES
 ========================================================================*/
#define API_DEBUG                    AA_DEBUG
#define API_OK                       AA_OK
#define API_UNABLE_TO_LOAD_LIBRARY   AA_UNABLE_TO_LOAD_LIBRARY
#define API_INCOMPATIBLE_LIBRARY     AA_INCOMPATIBLE_LIBRARY
#define API_UNABLE_TO_LOAD_FUNCTION  AA_UNABLE_TO_LOAD_FUNCTION
#define API_HEADER_VERSION           AA_HEADER_VERSION
#define API_REQ_SW_VERSION           AA_REQ_SW_VERSION


/*=========================================================================
| LINUX AND DARWIN SUPPORT
 ========================================================================*/
#if defined(__APPLE_CC__) && !defined(DARWIN)
#define DARWIN 
#endif

#if defined(linux) || defined(DARWIN)
#define API_NAME                     "libaardvark"
//#define __USE_XOPEN2K
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

static char SO_NAME[MAX_SO_PATH+1] = API_NAME ".so";

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

    /* Check if the file is readable */
    sprintf(filename, "%s/%s", path, SO_NAME);
    fd = open(filename, O_RDONLY);
    if (fd >= 0) {
        strncpy(SO_NAME, filename, MAX_SO_PATH);
        close(fd);
    }

    /* Clean up and exit */
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
#define API_NAME                     "aardvark"

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
static void _setSearchPath (void){
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

#if API_DEBUG
   //         fprintf(stderr, "Loading %s\n", name);
#endif
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
        *(void**)(&version) = dlsym(handle, "aa_c_version");
    #pragma GCC diagnostic pop    //Save actual diagnostics state

        if (version == 0) {
#if API_DEBUG
            fprintf(stderr, "Unable to bind aa_c_version() in %s\n",
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
static int (*c_aa_find_devices) (int, u16 *) = 0;
int aa_find_devices (
    int   num_devices,
    u16 * devices
)
{
    if (c_aa_find_devices == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_find_devices) = _loadFunction("c_aa_find_devices", &res)))
            return res;
    }
    return c_aa_find_devices(num_devices, devices);
}


static int (*c_aa_find_devices_ext) (int, u16 *, int, u32 *) = 0;
int aa_find_devices_ext (
    int   num_devices,
    u16 * devices,
    int   num_ids,
    u32 * unique_ids
)
{
    if (c_aa_find_devices_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_find_devices_ext) = _loadFunction("c_aa_find_devices_ext", &res)))
            return res;
    }
    return c_aa_find_devices_ext(num_devices, devices, num_ids, unique_ids);
}


static Aardvark (*c_aa_open) (int) = 0;
Aardvark aa_open (
    int port_number
)
{
    if (c_aa_open == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_open) = _loadFunction("c_aa_open", &res)))
            return res;
    }
    return c_aa_open(port_number);
}


static Aardvark (*c_aa_open_ext) (int, AardvarkExt *) = 0;
Aardvark aa_open_ext (
    int           port_number,
    AardvarkExt * aa_ext
)
{
    if (c_aa_open_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_open_ext) = _loadFunction("c_aa_open_ext", &res)))
            return res;
    }
    return c_aa_open_ext(port_number, aa_ext);
}


static int (*c_aa_close) (Aardvark) = 0;
int aa_close (
    Aardvark aardvark
)
{
    if (c_aa_close == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_close) = _loadFunction("c_aa_close", &res)))
            return res;
    }
    return c_aa_close(aardvark);
}


static int (*c_aa_port) (Aardvark) = 0;
int aa_port (
    Aardvark aardvark
)
{
    if (c_aa_port == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_port) = _loadFunction("c_aa_port", &res)))
            return res;
    }
    return c_aa_port(aardvark);
}


static int (*c_aa_features) (Aardvark) = 0;
int aa_features (
    Aardvark aardvark
)
{
    if (c_aa_features == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_features) = _loadFunction("c_aa_features", &res)))
            return res;
    }
    return c_aa_features(aardvark);
}


static u32 (*c_aa_unique_id) (Aardvark) = 0;
u32 aa_unique_id (
    Aardvark aardvark
)
{
    if (c_aa_unique_id == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_unique_id) = _loadFunction("c_aa_unique_id", &res)))
            return res;
    }
    return c_aa_unique_id(aardvark);
}


static const char * (*c_aa_status_string) (int) = 0;
const char * aa_status_string (
    int status
)
{
    if (c_aa_status_string == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_status_string) = _loadFunction("c_aa_status_string", &res)))
            return 0;
    }
    return c_aa_status_string(status);
}


static int (*c_aa_log) (Aardvark, int, int) = 0;
int aa_log (
    Aardvark aardvark,
    int      level,
    int      handle
)
{
    if (c_aa_log == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_log) = _loadFunction("c_aa_log", &res)))
            return res;
    }
    return c_aa_log(aardvark, level, handle);
}


static int (*c_aa_version) (Aardvark, AardvarkVersion *) = 0;
int aa_version (
    Aardvark          aardvark,
    AardvarkVersion * version
)
{
    if (c_aa_version == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_version) = _loadFunction("c_aa_version", &res)))
            return res;
    }
    return c_aa_version(aardvark, version);
}


static int (*c_aa_configure) (Aardvark, AardvarkConfig) = 0;
int aa_configure (
    Aardvark       aardvark,
    AardvarkConfig config
)
{
    if (c_aa_configure == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_configure) = _loadFunction("c_aa_configure", &res)))
            return res;
    }
    return c_aa_configure(aardvark, config);
}


static int (*c_aa_target_power) (Aardvark, u08) = 0;
int aa_target_power (
    Aardvark aardvark,
    u08      power_mask
)
{
    if (c_aa_target_power == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_target_power) = _loadFunction("c_aa_target_power", &res)))
            return res;
    }
    return c_aa_target_power(aardvark, power_mask);
}


static u32 (*c_aa_sleep_ms) (u32) = 0;
u32 aa_sleep_ms (
    u32 milliseconds
)
{
    if (c_aa_sleep_ms == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_sleep_ms) = _loadFunction("c_aa_sleep_ms", &res)))
            return res;
    }
    return c_aa_sleep_ms(milliseconds);
}


static int (*c_aa_async_poll) (Aardvark, int) = 0;
int aa_async_poll (
    Aardvark aardvark,
    int      timeout
)
{
    if (c_aa_async_poll == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_async_poll) = _loadFunction("c_aa_async_poll", &res)))
            return res;
    }
    return c_aa_async_poll(aardvark, timeout);
}


static int (*c_aa_i2c_free_bus) (Aardvark) = 0;
int aa_i2c_free_bus (
    Aardvark aardvark
)
{
    if (c_aa_i2c_free_bus == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_free_bus) = _loadFunction("c_aa_i2c_free_bus", &res)))
            return res;
    }
    return c_aa_i2c_free_bus(aardvark);
}


static int (*c_aa_i2c_bitrate) (Aardvark, int) = 0;
int aa_i2c_bitrate (
    Aardvark aardvark,
    int      bitrate_khz
)
{
    if (c_aa_i2c_bitrate == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_bitrate) = _loadFunction("c_aa_i2c_bitrate", &res)))
            return res;
    }
    return c_aa_i2c_bitrate(aardvark, bitrate_khz);
}


static int (*c_aa_i2c_bus_timeout) (Aardvark, u16) = 0;
int aa_i2c_bus_timeout (
    Aardvark aardvark,
    u16      timeout_ms
)
{
    if (c_aa_i2c_bus_timeout == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_bus_timeout) = _loadFunction("c_aa_i2c_bus_timeout", &res)))
            return res;
    }
    return c_aa_i2c_bus_timeout(aardvark, timeout_ms);
}


static int (*c_aa_i2c_read) (Aardvark, u16, AardvarkI2cFlags, u16, u08 *) = 0;
int aa_i2c_read (
    Aardvark         aardvark,
    u16              slave_addr,
    AardvarkI2cFlags flags,
    u16              num_bytes,
    u08 *            data_in
)
{
    if (c_aa_i2c_read == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_read) = _loadFunction("c_aa_i2c_read", &res)))
            return res;
    }
    return c_aa_i2c_read(aardvark, slave_addr, flags, num_bytes, data_in);
}


static int (*c_aa_i2c_read_ext) (Aardvark, u16, AardvarkI2cFlags, u16, u08 *, u16 *) = 0;
int aa_i2c_read_ext (
    Aardvark         aardvark,
    u16              slave_addr,
    AardvarkI2cFlags flags,
    u16              num_bytes,
    u08 *            data_in,
    u16 *            num_read
)
{
    if (c_aa_i2c_read_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_read_ext) = _loadFunction("c_aa_i2c_read_ext", &res)))
            return res;
    }
    return c_aa_i2c_read_ext(aardvark, slave_addr, flags, num_bytes, data_in, num_read);
}


static int (*c_aa_i2c_write) (Aardvark, u16, AardvarkI2cFlags, u16, const u08 *) = 0;
int aa_i2c_write (
    Aardvark         aardvark,
    u16              slave_addr,
    AardvarkI2cFlags flags,
    u16              num_bytes,
    const u08 *      data_out
)
{
    if (c_aa_i2c_write == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_write) = _loadFunction("c_aa_i2c_write", &res)))
            return res;
    }
    return c_aa_i2c_write(aardvark, slave_addr, flags, num_bytes, data_out);
}


static int (*c_aa_i2c_write_ext) (Aardvark, u16, AardvarkI2cFlags, u16, const u08 *, u16 *) = 0;
int aa_i2c_write_ext (
    Aardvark         aardvark,
    u16              slave_addr,
    AardvarkI2cFlags flags,
    u16              num_bytes,
    const u08 *      data_out,
    u16 *            num_written
)
{
    if (c_aa_i2c_write_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_write_ext) = _loadFunction("c_aa_i2c_write_ext", &res)))
            return res;
    }
    return c_aa_i2c_write_ext(aardvark, slave_addr, flags, num_bytes, data_out, num_written);
}


static int (*c_aa_i2c_write_read) (Aardvark, u16, AardvarkI2cFlags, u16, const u08 *, u16 *, u16, u08 *, u16 *) = 0;
int aa_i2c_write_read (
    Aardvark         aardvark,
    u16              slave_addr,
    AardvarkI2cFlags flags,
    u16              out_num_bytes,
    const u08 *      out_data,
    u16 *            num_written,
    u16              in_num_bytes,
    u08 *            in_data,
    u16 *            num_read
)
{
    if (c_aa_i2c_write_read == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_write_read) = _loadFunction("c_aa_i2c_write_read", &res)))
            return res;
    }
    return c_aa_i2c_write_read(aardvark, slave_addr, flags, out_num_bytes, out_data, num_written, in_num_bytes, in_data, num_read);
}


static int (*c_aa_i2c_slave_enable) (Aardvark, u08, u16, u16) = 0;
int aa_i2c_slave_enable (
    Aardvark aardvark,
    u08      addr,
    u16      maxTxBytes,
    u16      maxRxBytes
)
{
    if (c_aa_i2c_slave_enable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_enable) = _loadFunction("c_aa_i2c_slave_enable", &res)))
            return res;
    }
    return c_aa_i2c_slave_enable(aardvark, addr, maxTxBytes, maxRxBytes);
}


static int (*c_aa_i2c_slave_disable) (Aardvark) = 0;
int aa_i2c_slave_disable (
    Aardvark aardvark
)
{
    if (c_aa_i2c_slave_disable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_disable) = _loadFunction("c_aa_i2c_slave_disable", &res)))
            return res;
    }
    return c_aa_i2c_slave_disable(aardvark);
}


static int (*c_aa_i2c_slave_set_response) (Aardvark, u08, const u08 *) = 0;
int aa_i2c_slave_set_response (
    Aardvark    aardvark,
    u08         num_bytes,
    const u08 * data_out
)
{
    if (c_aa_i2c_slave_set_response == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_set_response) = _loadFunction("c_aa_i2c_slave_set_response", &res)))
            return res;
    }
    return c_aa_i2c_slave_set_response(aardvark, num_bytes, data_out);
}


static int (*c_aa_i2c_slave_write_stats) (Aardvark) = 0;
int aa_i2c_slave_write_stats (
    Aardvark aardvark
)
{
    if (c_aa_i2c_slave_write_stats == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_write_stats) = _loadFunction("c_aa_i2c_slave_write_stats", &res)))
            return res;
    }
    return c_aa_i2c_slave_write_stats(aardvark);
}


static int (*c_aa_i2c_slave_read) (Aardvark, u08 *, u16, u08 *) = 0;
int aa_i2c_slave_read (
    Aardvark aardvark,
    u08 *    addr,
    u16      num_bytes,
    u08 *    data_in
)
{
    if (c_aa_i2c_slave_read == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_read) = _loadFunction("c_aa_i2c_slave_read", &res)))
            return res;
    }
    return c_aa_i2c_slave_read(aardvark, addr, num_bytes, data_in);
}


static int (*c_aa_i2c_slave_write_stats_ext) (Aardvark, u16 *) = 0;
int aa_i2c_slave_write_stats_ext (
    Aardvark aardvark,
    u16 *    num_written
)
{
    if (c_aa_i2c_slave_write_stats_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_write_stats_ext) = _loadFunction("c_aa_i2c_slave_write_stats_ext", &res)))
            return res;
    }
    return c_aa_i2c_slave_write_stats_ext(aardvark, num_written);
}


static int (*c_aa_i2c_slave_read_ext) (Aardvark, u08 *, u16, u08 *, u16 *) = 0;
int aa_i2c_slave_read_ext (
    Aardvark aardvark,
    u08 *    addr,
    u16      num_bytes,
    u08 *    data_in,
    u16 *    num_read
)
{
    if (c_aa_i2c_slave_read_ext == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_slave_read_ext) = _loadFunction("c_aa_i2c_slave_read_ext", &res)))
            return res;
    }
    return c_aa_i2c_slave_read_ext(aardvark, addr, num_bytes, data_in, num_read);
}


static int (*c_aa_i2c_monitor_enable) (Aardvark) = 0;
int aa_i2c_monitor_enable (
    Aardvark aardvark
)
{
    if (c_aa_i2c_monitor_enable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_monitor_enable) = _loadFunction("c_aa_i2c_monitor_enable", &res)))
            return res;
    }
    return c_aa_i2c_monitor_enable(aardvark);
}


static int (*c_aa_i2c_monitor_disable) (Aardvark) = 0;
int aa_i2c_monitor_disable (
    Aardvark aardvark
)
{
    if (c_aa_i2c_monitor_disable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_monitor_disable) = _loadFunction("c_aa_i2c_monitor_disable", &res)))
            return res;
    }
    return c_aa_i2c_monitor_disable(aardvark);
}


static int (*c_aa_i2c_monitor_read) (Aardvark, u16, u16 *) = 0;
int aa_i2c_monitor_read (
    Aardvark aardvark,
    u16      num_bytes,
    u16 *    data
)
{
    if (c_aa_i2c_monitor_read == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_monitor_read) = _loadFunction("c_aa_i2c_monitor_read", &res)))
            return res;
    }
    return c_aa_i2c_monitor_read(aardvark, num_bytes, data);
}


static int (*c_aa_i2c_pullup) (Aardvark, u08) = 0;
int aa_i2c_pullup (
    Aardvark aardvark,
    u08      pullup_mask
)
{
    if (c_aa_i2c_pullup == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_i2c_pullup) = _loadFunction("c_aa_i2c_pullup", &res)))
            return res;
    }
    return c_aa_i2c_pullup(aardvark, pullup_mask);
}


static int (*c_aa_spi_bitrate) (Aardvark, int) = 0;
int aa_spi_bitrate (
    Aardvark aardvark,
    int      bitrate_khz
)
{
    if (c_aa_spi_bitrate == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_bitrate) = _loadFunction("c_aa_spi_bitrate", &res)))
            return res;
    }
    return c_aa_spi_bitrate(aardvark, bitrate_khz);
}


static int (*c_aa_spi_configure) (Aardvark, AardvarkSpiPolarity, AardvarkSpiPhase, AardvarkSpiBitorder) = 0;
int aa_spi_configure (
    Aardvark            aardvark,
    AardvarkSpiPolarity polarity,
    AardvarkSpiPhase    phase,
    AardvarkSpiBitorder bitorder
)
{
    if (c_aa_spi_configure == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_configure) = _loadFunction("c_aa_spi_configure", &res)))
            return res;
    }
    return c_aa_spi_configure(aardvark, polarity, phase, bitorder);
}


static int (*c_aa_spi_write) (Aardvark, u16, const u08 *, u16, u08 *) = 0;
int aa_spi_write (
    Aardvark    aardvark,
    u16         out_num_bytes,
    const u08 * data_out,
    u16         in_num_bytes,
    u08 *       data_in
)
{
    if (c_aa_spi_write == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_write) = _loadFunction("c_aa_spi_write", &res)))
            return res;
    }
    return c_aa_spi_write(aardvark, out_num_bytes, data_out, in_num_bytes, data_in);
}


static int (*c_aa_spi_slave_enable) (Aardvark) = 0;
int aa_spi_slave_enable (
    Aardvark aardvark
)
{
    if (c_aa_spi_slave_enable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_slave_enable) = _loadFunction("c_aa_spi_slave_enable", &res)))
            return res;
    }
    return c_aa_spi_slave_enable(aardvark);
}


static int (*c_aa_spi_slave_disable) (Aardvark) = 0;
int aa_spi_slave_disable (
    Aardvark aardvark
)
{
    if (c_aa_spi_slave_disable == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_slave_disable) = _loadFunction("c_aa_spi_slave_disable", &res)))
            return res;
    }
    return c_aa_spi_slave_disable(aardvark);
}


static int (*c_aa_spi_slave_set_response) (Aardvark, u08, const u08 *) = 0;
int aa_spi_slave_set_response (
    Aardvark    aardvark,
    u08         num_bytes,
    const u08 * data_out
)
{
    if (c_aa_spi_slave_set_response == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_slave_set_response) = _loadFunction("c_aa_spi_slave_set_response", &res)))
            return res;
    }
    return c_aa_spi_slave_set_response(aardvark, num_bytes, data_out);
}


static int (*c_aa_spi_slave_read) (Aardvark, u16, u08 *) = 0;
int aa_spi_slave_read (
    Aardvark aardvark,
    u16      num_bytes,
    u08 *    data_in
)
{
    if (c_aa_spi_slave_read == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_slave_read) = _loadFunction("c_aa_spi_slave_read", &res)))
            return res;
    }
    return c_aa_spi_slave_read(aardvark, num_bytes, data_in);
}


static int (*c_aa_spi_master_ss_polarity) (Aardvark, AardvarkSpiSSPolarity) = 0;
int aa_spi_master_ss_polarity (
    Aardvark              aardvark,
    AardvarkSpiSSPolarity polarity
)
{
    if (c_aa_spi_master_ss_polarity == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_spi_master_ss_polarity) = _loadFunction("c_aa_spi_master_ss_polarity", &res)))
            return res;
    }
    return c_aa_spi_master_ss_polarity(aardvark, polarity);
}


static int (*c_aa_gpio_direction) (Aardvark, u08) = 0;
int aa_gpio_direction (
    Aardvark aardvark,
    u08      direction_mask
)
{
    if (c_aa_gpio_direction == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_gpio_direction) = _loadFunction("c_aa_gpio_direction", &res)))
            return res;
    }
    return c_aa_gpio_direction(aardvark, direction_mask);
}


static int (*c_aa_gpio_pullup) (Aardvark, u08) = 0;
int aa_gpio_pullup (
    Aardvark aardvark,
    u08      pullup_mask
)
{
    if (c_aa_gpio_pullup == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_gpio_pullup) = _loadFunction("c_aa_gpio_pullup", &res)))
            return res;
    }
    return c_aa_gpio_pullup(aardvark, pullup_mask);
}


static int (*c_aa_gpio_get) (Aardvark) = 0;
int aa_gpio_get (
    Aardvark aardvark
)
{
    if (c_aa_gpio_get == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_gpio_get) = _loadFunction("c_aa_gpio_get", &res)))
            return res;
    }
    return c_aa_gpio_get(aardvark);
}


static int (*c_aa_gpio_set) (Aardvark, u08) = 0;
int aa_gpio_set (
    Aardvark aardvark,
    u08      value
)
{
    if (c_aa_gpio_set == 0) {
        int res = 0;
        if (!(*(void**)(&c_aa_gpio_set) = _loadFunction("c_aa_gpio_set", &res)))
            return res;
    }
    return c_aa_gpio_set(aardvark, value);
}


static int (*c_aa_gpio_change) (Aardvark, u16) = 0;
int aa_gpio_change (
    Aardvark aardvark,
    u16      timeout
)
{
    if (c_aa_gpio_change == 0) {
        int res = 0;
        if (!( *(void**)(&c_aa_gpio_change) = _loadFunction("c_aa_gpio_change", &res)))
            return res;
    }
    return c_aa_gpio_change(aardvark, timeout);
}

#endif

