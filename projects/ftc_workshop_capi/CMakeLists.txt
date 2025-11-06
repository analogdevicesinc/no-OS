add_library(project_lib STATIC)
add_executable(ftc_workshop)

if(${CONFIG_EXERCISE_1})
        target_sources(project_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/exercise_1/main.c)
elseif (${CONFIG_EXERCISE_2})
        target_sources(project_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/exercise_2/main.c)
elseif (${CONFIG_EXERCISE_3})
        target_sources(project_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/exercise_3/main.c)
elseif (${CONFIG_EXERCISE_4})
        target_sources(project_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/exercise_4/main.c)
elseif (${CONFIG_EXERCISE_5})
        target_sources(project_lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/exercise_5/main.c)
endif()

target_include_directories(project_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/${PLATFORM}
)

target_link_libraries(project_lib no-os)
target_link_libraries(ftc_workshop project_lib)

config_platform_sdk(ftc_workshop)
add_flash_target(ftc_workshop)
post_build_config(ftc_workshop)
