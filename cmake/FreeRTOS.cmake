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

if(NOT CONFIG_FREERTOS_CONF_PATH)
  target_include_directories(freertos_config SYSTEM
          INTERFACE
          ${NO_OS_DIR}/drivers/platform/freeRTOS
  )
else()
  # Handle CONFIG_FREERTOS_CONF_PATH - make it relative to appropriate base directory
  # For native builds, use NO_OS_DIR; for external builds, use CMAKE_SOURCE_DIR
  if("${CMAKE_SOURCE_DIR}" STREQUAL "${NO_OS_DIR}")
    # Native build - path relative to NO_OS_DIR
    target_include_directories(freertos_config SYSTEM
            INTERFACE
            ${NO_OS_DIR}/${CONFIG_FREERTOS_CONF_PATH}
    )
  else()
    # External build - path relative to CMAKE_SOURCE_DIR
    target_include_directories(freertos_config SYSTEM
            INTERFACE
            ${CMAKE_SOURCE_DIR}/${CONFIG_FREERTOS_CONF_PATH}
    )
  endif()
endif()

message(STATUS "Cloning FreeRTOS... 📥")
FetchContent_MakeAvailable(freertos)

target_link_libraries(no-os freertos_kernel)