# adpd188
no_os_sources_ifdef(CONFIG_PHOTO_ELECTRONIC_ADPD188 ${CMAKE_CURRENT_SOURCE_DIR}/adpd188/adpd188.c)
no_os_sources_ifdef(CONFIG_PHOTO_ELECTRONIC_IIO_ADPD188 ${CMAKE_CURRENT_SOURCE_DIR}/adpd188/iio_adpd188.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adpd188)

# adpd410x
no_os_sources_ifdef(CONFIG_PHOTO_ELECTRONIC_ADPD410X ${CMAKE_CURRENT_SOURCE_DIR}/adpd410x/adpd410x.c)
no_os_sources_ifdef(CONFIG_PHOTO_ELECTRONIC_IIO_ADPD410X ${CMAKE_CURRENT_SOURCE_DIR}/adpd410x/iio_adpd410x.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/adpd410x)
