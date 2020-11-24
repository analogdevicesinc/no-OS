################################################################################
#									       #
#     Shared variables:							       #
#	- PROJECT							       #
#	- DRIVERS							       #
#	- INCLUDE							       #
#	- PLATFORM_DRIVERS						       #
#	- NO-OS								       #
#									       #
################################################################################

# Add to SRCS source files to be build in the project
SRCS += $(PROJECT)/src/main.c

# Add to INCS inlcude files to be build in the porject
INCS += $(PROJECT)/src/main.h

# Add to SRC_DIRS directories to be used in the build. All .c and .h files from
# the directory and subdirectories will be added to the build (recursively)
SRC_DIRS += $(PLATFORM_DRIVERS)

# Add to IGNORE_FILES files to be ignored from build
IGNORED_FILES += $(PLATFORM_DRIVERS)/irq.c

# Add to LIBRARIES the libraries that need to be linked in the build
# LIBRARIES += mqtt
# LIBRARIES += fatfs
# LIBRARIES += mbedtls
LIBRARIES += iio

# If the variable is set to y then iio network backend will be enabled
ENABLE_IIO_NETWORK = y

# If the variable is set to y then iio sources will be added to the projects
# instead of using the library. Will be easier to debug the library with this.
BUILD_IIO_AS_SOURCES = y

# If set, link to noos srcs will be created for new project instead of copy them
# to the project directory. This way modification to the files can be viewed on
# git for example
LINK_SRCS = y
