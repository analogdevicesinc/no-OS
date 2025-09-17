include(FetchContent)

FetchContent_Declare(
        lwip
        GIT_REPOSITORY https://github.com/lwip-tcpip/lwip
        GIT_TAG STABLE-2_2_1_RELEASE
)

message(STATUS "Cloning lwip... 📥")
FetchContent_MakeAvailable(lwip)

set (LWIP_INCLUDE_DIRS
    "${lwip_SOURCE_DIR}/src/include"
    "${lwip_SOURCE_DIR}/contrib"
    "${lwip_SOURCE_DIR}/contrib/examples/example_app"
)

target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip/configs)
target_include_directories(lwipcore PUBLIC ${NO_OS_DIR}/libraries/lwip)
target_include_directories(lwipcore PUBLIC ${LWIP_INCLUDE_DIRS})

target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip/configs)
target_include_directories(lwipallapps PRIVATE ${NO_OS_DIR}/libraries/lwip)
target_include_directories(lwipallapps PRIVATE ${LWIP_INCLUDE_DIRS})

target_compile_definitions(no-os PUBLIC -DNO_OS_LWIP_NETWORKING=1)
target_compile_definitions(no-os PUBLIC -DDISABLE_SECURE_SOCKET=1)

target_link_libraries(no-os lwipallapps lwipcore)