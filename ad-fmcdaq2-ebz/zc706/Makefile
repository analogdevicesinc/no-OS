## Makefile
## Default:
##    <some-directory>/hdl
##    <some-directory>/no-OS

M_ROOT_DIR := $(abspath $(lastword $(MAKEFILE_LIST)))
M_ROOT_DIR := $(subst /no-OS, ,$(M_ROOT_DIR))
M_ROOT_DIR := $(firstword $(M_ROOT_DIR))

## To change root directory, use make ROOT-DIR=<my-directory>
## Note - both hdl & no-OS must be present at the root.

ifeq ($(ROOT-DIR),)
  ROOT-DIR := $(M_ROOT_DIR)
endif

## To change individually (i.e. separate directories for no-OS and hdl),
## use make NOOS-DIR=<my-noos-directory> HDL-DIR=<my-hdl-directory>

ifeq ($(NOOS-DIR),)
  NOOS-DIR := $(M_ROOT_DIR)/no-OS
endif

ifeq ($(HDL-DIR),)
  HDL-DIR := $(M_ROOT_DIR)/hdl
endif

## To change HDF file, use make HDF-FILE=<my-hdf-file>

M_HDF_FILE := $(HDL-DIR)/projects/daq2/zc706/daq2_zc706.sdk/system_top.hdf

## To build, use make
## To run on hardware, use make run

include $(NOOS-DIR)/ad-fmcdaq2-ebz/ad_fmcdaq2_ebz.mk
include $(NOOS-DIR)/scripts/zynq.mk

