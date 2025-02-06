include $(PROJECT)/src/platform/$(PLATFORM)/platform_src.mk

CFLAGS += -D_DEBUG
CFLAGS += -DGMSL_LOG_LEVEL=GMSL_LOG_LEVEL_INFO

INCS += $(INCLUDE)/no_os_delay.h     \
		$(INCLUDE)/no_os_error.h     \
		$(INCLUDE)/no_os_i2c.h       \
		$(INCLUDE)/no_os_print_log.h \
		$(INCLUDE)/no_os_util.h \
		$(INCLUDE)/no_os_alloc.h \
        $(INCLUDE)/no_os_mutex.h

SRCS += $(DRIVERS)/api/no_os_i2c.c  \
		$(NO-OS)/util/no_os_util.c \
		$(NO-OS)/util/no_os_alloc.c \
        $(NO-OS)/util/no_os_mutex.c

INCS += $(DRIVERS)/gmsl/common/gmsl_reg_access.h \
		$(DRIVERS)/gmsl/common/gmsl_cam_des.h \
		$(DRIVERS)/gmsl/common/gmsl_cam_ser.h \
	        $(DRIVERS)/gmsl/common/gmsl_common.h \
	        $(DRIVERS)/gmsl/common/gmsl_disp_ser.h \
	        $(DRIVERS)/gmsl/common/gmsl_disp_des.h \
		$(DRIVERS)/gmsl/common/gmsl_oldi_des.h \
		$(DRIVERS)/gmsl/common/gmsl_dp_des.h \
		$(DRIVERS)/gmsl/common/gmsl_dp_ser.h \


SRCS += $(DRIVERS)/gmsl/common/gmsl_reg_access.c 

INCS += $(DRIVERS)/gmsl/gmsl_dbg/gmsl_dbg.h


INCS += $(PROJECT)/src/common/gmsl_app_common_data.h
SRCS += $(PROJECT)/src/common/gmsl_app_common_data.c

SRCS += $(PROJECT)/src/diag_mngr/diag_mngr.c
INCS += $(PROJECT)/src/diag_mngr/diag_mngr.h

SRCS += $(PROJECT)/src/platform/$(PLATFORM)/main.c \
		$(PROJECT)/src/platform/$(PLATFORM)/parameters.c \
		$(PROJECT)/src/platform/$(PLATFORM)/log.c

INCS += $(PROJECT)/src/platform/platform_includes.h
INCS += $(PROJECT)/src/platform/$(PLATFORM)/parameters.h

ifeq (y,$(strip $(CFG_DIAG_MAX96792_MAX96793_UC_EX)))
CFLAGS += -DCFG_DIAG_MAX96792_MAX96793_UC_EX_ENABLED

INCS += $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792.h \
        $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792_csi.h \
        $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792_regs.h \
		$(DRIVERS)/gmsl/deserializer/camera/max96792/max96792_diag.h \
		$(DRIVERS)/gmsl/deserializer/camera/max96792/max96792.h \

SRCS += $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792_diag.c\
        $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792_csi.c \
        $(DRIVERS)/gmsl/deserializer/camera/max96792/max96792.c \

INCS += $(DRIVERS)/gmsl/serializer/camera/max96793/max96793.h \
		$(DRIVERS)/gmsl/serializer/camera/max96793/max96793_diag.h \
		$(DRIVERS)/gmsl/serializer/camera/max96793/max96793_regs.h \
		$(DRIVERS)/gmsl/serializer/camera/max96793/max96793_csi.h \

SRCS += $(DRIVERS)/gmsl/serializer/camera/max96793/max96793.c \
		$(DRIVERS)/gmsl/serializer/camera/max96793/max96793_diag.c \
		$(DRIVERS)/gmsl/serializer/camera/max96793/max96793_csi.c \

SRCS += $(PROJECT)/src/examples/max96792_max96793_example/max96792_max96793_pixel_mode_6G_linkA_linkB_example.c 
#SRCS += $(PROJECT)/src/examples/max96792_max96793_example/max96792_max96793_tunnel_mode_12G_linkA_example.c 
INCS += $(PROJECT)/src/examples/max96792_max96793_example/max96792_max96793_example.h

INCS += $(PROJECT)/src/examples/max96792_max96793_example/diag_max96793_config.h
INCS += $(PROJECT)/src/examples/max96792_max96793_example/diag_max96792_config.h

endif
