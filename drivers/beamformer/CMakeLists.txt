# adar300x
no_os_sources_ifdef(CONFIG_BEAMFORMER_ADAR300X ${CMAKE_CURRENT_SOURCE_DIR}/adar300x/adar300x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adar300x)
