#See No-OS/tool/scripts/src_model.mk for variable description

SRC_DIRS += $(PROJECT)/srcs			\
		$(NO-OS)/drivers/dac/ad3552r	\
		$(NO-OS)/drivers/api		\
		$(PLATFORM_DRIVERS)		\
		$(NO-OS)/util			\
		$(INCLUDE)
