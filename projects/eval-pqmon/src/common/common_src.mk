INCS +=		$(PROJECT)/src/common/afe_config.h 	\
		$(PROJECT)/src/common/afe_calibration.h	\
		$(PROJECT)/src/common/pqlib_example.h	\
		$(PROJECT)/src/common/pqlib_afe.h	\
		$(PROJECT)/src/common/pqlib_convert.h	\
		$(PROJECT)/src/common/status.h 		\
		$(PROJECT)/src/common/common_data.h	\
		$(PROJECT)/src/common/iio_pqm.h		\
		$(PROJECT)/src/common/flash_storage.h	\
		$(PROJECT)/src/common/uart_export.h	\
		$(PROJECT)/src/common/waveform_dma.h	\
		$(PROJECT)/src/common/usb_raw_stream.h

SRCS +=		$(PROJECT)/src/common/afe_config.c 	\
		$(PROJECT)/src/common/afe_calibration.c	\
		$(PROJECT)/src/common/pqlib_example.c	\
		$(PROJECT)/src/common/pqlib_afe.c	\
		$(PROJECT)/src/common/pqlib_convert.c	\
		$(PROJECT)/src/common/common_data.c	\
		$(PROJECT)/src/common/iio_pqm.c		\
		$(PROJECT)/src/common/flash_storage.c	\
		$(PROJECT)/src/common/uart_export.c	\
		$(PROJECT)/src/common/waveform_dma.c

ifeq ($(INTERFACE),usb)
SRCS +=		$(PROJECT)/src/common/usb_raw_stream.c
endif
