################################################################################
#                                                                              #
#     Basic example makefile for ADMT4000                                     #
#                                                                              #
################################################################################

# No IIO support needed for basic example
IIOD=n

# Add the basic example source files
SRCS += $(PROJECT)/src/examples/basic/basic_example.c
INCS += $(PROJECT)/src/examples/basic/basic_example.h

# Common data - already included by main src.mk
SRCS += $(PROJECT)/src/common/common_data.c
INCS += $(PROJECT)/src/common/common_data.h
