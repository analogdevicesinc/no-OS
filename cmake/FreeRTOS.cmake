include(FetchContent)

FetchContent_Declare(
        freertos
        GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
        GIT_TAG V11.2.0
)

set(FREERTOS_HEAP ${CONFIG_FREERTOS_HEAP_ALLOCATOR} CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
if (CMAKE_CROSSCOMPILING)
  set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
endif()

add_library(freertos_config INTERFACE)

if(NOT ${CONFIG_FREERTOS_CONF_PATH})
  target_include_directories(freertos_config SYSTEM
          INTERFACE
          ${NO_OS_DIR}/drivers/platform/freeRTOS
  )
endif()

message(STATUS "Cloning FreeRTOS... 📥")
FetchContent_MakeAvailable(freertos)

target_link_libraries(no-os freertos_kernel)