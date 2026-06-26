# Common digital potentiometer base layer
no_os_sources_ifdef(CONFIG_POTENTIOMETER ${CMAKE_CURRENT_SOURCE_DIR}/common/dpot.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/common)

# ad5110
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5110 ${CMAKE_CURRENT_SOURCE_DIR}/ad5110/ad5110.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5110)

# ad514x (AD5141/AD5142/AD5143/AD5144)
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD514X
    ${CMAKE_CURRENT_SOURCE_DIR}/ad514x/ad5141.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad514x/ad5142.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad514x/ad5143.c
    ${CMAKE_CURRENT_SOURCE_DIR}/ad514x/ad5144.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad514x)

# ad5161
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5161 ${CMAKE_CURRENT_SOURCE_DIR}/ad5161/ad5161.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5161)

# ad5165
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5165 ${CMAKE_CURRENT_SOURCE_DIR}/ad5165/ad5165.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5165)

# ad5171
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5171 ${CMAKE_CURRENT_SOURCE_DIR}/ad5171/ad5171.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5171)

# ad5242
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5242 ${CMAKE_CURRENT_SOURCE_DIR}/ad5242/ad5242.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5242)

# ad5246
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5246 ${CMAKE_CURRENT_SOURCE_DIR}/ad5246/ad5246.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5246)

# ad5259
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5259 ${CMAKE_CURRENT_SOURCE_DIR}/ad5259/ad5259.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5259)

# ad525x
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD525X ${CMAKE_CURRENT_SOURCE_DIR}/ad525x/ad525x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad525x)

# ad5293
no_os_sources_ifdef(CONFIG_POTENTIOMETER_AD5293 ${CMAKE_CURRENT_SOURCE_DIR}/ad5293/ad5293.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ad5293)
