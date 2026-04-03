# FPGA programming via Lattice Radiant Programmer

# -----------------------------------------------------------------------
# User-configurable variables
# -----------------------------------------------------------------------
# File path configuration
LATTICE_BITSTREAM_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

# LATTICE_DEVICE can be empty: the recipe will fallback to LATTICE_DEVICE_NAME from sys_env.xml
LATTICE_DEVICE        ?=
LATTICE_BIT           ?= $(firstword $(wildcard ./*.bit))
LATTICE_TCK_DELAY     ?= 4
LATTICE_CABLE         ?= USB2
LATTICE_PORT          ?= FTUSB-0
LATTICE_XCF_TMPL      ?= $(LATTICE_BITSTREAM_DIR)/template.xcf

# -----------------------------------------------------------------------
# Derived paths
# -----------------------------------------------------------------------
# Programmer paths: find pgrcmd from PATH
LATTICE_PGRCMD_BIN = $(shell command -v pgrcmd 2>/dev/null)
LATTICE_PROG_ROOT  = $(if $(strip $(LATTICE_PGRCMD_BIN)),$(shell dirname "$(LATTICE_PGRCMD_BIN)" | xargs dirname | xargs dirname),)

# Programmer-relative paths
LATTICE_XDF = $(LATTICE_PROG_ROOT)/data/vmdata/database/Jedi/ispVM_027.xdf

# Local paths (Makefile könyvtárában)
LATTICE_OUT_XCF ?= $(BUILD_DIR)/_out.xcf

# -----------------------------------------------------------------------
# Device lookup from XDF
# -----------------------------------------------------------------------
LATTICE_PROG_INFO := $(BUILD_DIR)/prog_info.tmp

# Build a parsed device info fragment from XDF and sys_env.xml.
$(LATTICE_PROG_INFO): $(LATTICE_SYS_ENV_VARS) $(LATTICE_XDF)
	$(if $(LATTICE_PROG_ROOT),,$(error 'ERROR: PROG_ROOT is empty (pgrcmd not found in PATH)'))
	$(call print,[Parsing programming info from: $(LATTICE_XDF) to $(LATTICE_PROG_INFO) ...])
	@mkdir -p $(dir $@)
	@tmp="$@.tmp"; rm -f "$$tmp"; \
	{ \
	  . "$(LATTICE_SYS_ENV_VARS)"; \
	  dev_name="$(LATTICE_DEVICE)"; \
	  [ -n "$$dev_name" ] || dev_name="$$LATTICE_DEVICE_NAME"; \
	  [ -n "$$dev_name" ] || { echo "ERROR: device name is empty (set LATTICE_DEVICE or provide LATTICE_DEVICE_NAME in sys_env.xml)"; exit 1; }; \
	  [ -n "$(LATTICE_PGRCMD_BIN)" ] || { echo "ERROR: pgrcmd not found in PATH"; exit 1; }; \
	  [ -f "$(LATTICE_XDF)" ] || { echo "ERROR: XDF not found: $(LATTICE_XDF)"; exit 1; }; \
	  match=$$(awk -v dev="$$dev_name" '\
	    /Family name=/ { \
	      if (match($$0, /name="[^"]*"/)) \
	        fam = substr($$0, RSTART + 6, RLENGTH - 7); \
	    } \
	    index($$0, "Device name=\"" dev "\"") { in_dev = 1; next } \
	    in_dev && /<JtagID>/ { \
	      if (match($$0, /0x[0-9A-Fa-f]+/)) \
	        id = tolower(substr($$0, RSTART, RLENGTH)); \
	      print fam "|" id; \
	      exit; \
	    } \
	    END { if (!in_dev) print "|" } \
	  ' "$(LATTICE_XDF)"); \
	  family=$${match%%|*}; \
	  idcode=$${match#*|}; \
	  printf 'FAMILY=%s\nIDCODE=%s\nDEVICE_NAME=%s\nPON=%s\n' "$$family" "$$idcode" "$$dev_name" "$$dev_name"; \
	} > "$$tmp" && mv -f "$$tmp" "$@" || { rc=$$?; rm -f "$$tmp"; exit $$rc; }

# Convert BIT only on Cygwin; keep as-is on Linux.
IS_CYGWIN = $(filter CYGWIN%,$(shell uname -s 2>/dev/null))
LATTICE_BIT_PATH = $(if $(IS_CYGWIN),$(if $(LATTICE_BIT),$(shell cygpath -m "$(LATTICE_BIT)"),),$(LATTICE_BIT))
LATTICE_XCF_PATH = $(if $(IS_CYGWIN),$(if $(LATTICE_OUT_XCF),$(shell cygpath -m "$(abspath $(LATTICE_OUT_XCF))"),),$(LATTICE_OUT_XCF))

define LATTICE_USB_HELPERS
detect_lattice_ftdi_ifaces() { \
	ifaces=""; \
	for drv in /sys/bus/usb/devices/*:1.*/driver; do \
		[ -L "$$drv" ] && [ "$$(readlink -f "$$drv")" = "/sys/bus/usb/drivers/ftdi_sio" ] && { \
			iface=$${drv%/driver}; iface=$${iface##*/}; \
			ifaces="$$ifaces $$iface"; \
		}; \
	done; \
	printf '%s\n' "$${ifaces# }"; \
}; \
run_lattice_usb_cmd() { \
	cmd_path="$$1"; \
	cmd_name=$${cmd_path##*/}; \
	log_name="$$cmd_name"; \
	shift 1; \
	"$$cmd_path" "$$@"; \
	rc=$$?; \
	[ $$rc -eq 0 ] && exit 0; \
	if [ "$$(uname -s 2>/dev/null)" = "Linux" ] && command -v strace >/dev/null 2>&1; then \
		strace_log="$(BUILD_DIR)/$$log_name-strace.log"; \
		mkdir -p "$(BUILD_DIR)"; \
		strace -f -e trace=openat,ioctl -o "$$strace_log" "$$cmd_path" "$$@" >/dev/null 2>&1; \
		if grep -Eq 'EACCES|EBUSY' "$$strace_log"; then \
			printf '\n%s\n' "ERROR: $$cmd_name USB access errors (details: $$strace_log):"; \
			if grep -q EACCES "$$strace_log"; then \
				printf '%s\n' "ERROR: EACCES (permission denied):"; \
				nodes=$$(grep -oE '/dev/bus/usb/[0-9]{3}/[0-9]{3}' "$$strace_log" | sort -u); \
				if [ -n "$$nodes" ]; then \
					for node in $$nodes; do \
						printf '%s\n%s\n' "NOTE: For USB access, you can run the following command once:" \
							"sudo chown $$USER $$node"; \
					done; \
				else \
					printf '%s\n' "NOTE: Check $$strace_log for the denied USB node."; \
				fi; \
				printf '\n%s%s%s\n' "NOTE: You can also check https://www.latticesemi.com/en/Support/AnswerDatabase/6/1/6/6162, " \
					"but note that the solution there will leave the UART driver detached, and you will need " \
					"to find the USB interfaces manually and rebind them if required after programming."; \
			fi; \
			if grep -q EBUSY "$$strace_log"; then \
				printf '\n%s\n' "ERROR: EBUSY (device busy):"; \
				ifaces="$$(detect_lattice_ftdi_ifaces)"; \
				if [ -n "$$ifaces" ]; then \
					printf '\n%s%s\n' "NOTE: Detected USB interfaces currently bound to ftdi_sio. " \
						"If $$cmd_name uses one of these, unbind before retrying:"; \
					for iface in $$ifaces; do \
						printf '%s\n' "echo $$iface | sudo tee /sys/bus/usb/drivers/ftdi_sio/unbind >/dev/null"; \
					done; \
					printf '\n%s%s\n' "NOTE: If you need the default drivers (for example UART/ttyUSB*) after programming, " \
						"use the following to rebind the interfaces:"; \
					for iface in $$ifaces; do \
						printf '%s\n' "echo $$iface | sudo tee /sys/bus/usb/drivers/ftdi_sio/bind >/dev/null"; \
					done; \
					printf '\n'; \
				else \
					printf '\n%s%s\n' "NOTE: No ftdi_sio-bound USB interfaces were detected automatically. " \
						"Check $$strace_log for the busy device."; \
				fi; \
				printf '\n%s%s%s\n' "NOTE: You can also check https://www.latticesemi.com/en/Support/AnswerDatabase/6/1/6/6162, " \
					"but note that the solution there will leave the UART driver detached, and you will need " \
					"to find the USB interfaces manually and rebind them if required after programming."; \
			fi; \
			printf '\n'; \
		else \
			printf '%s\n' "ERROR: $$cmd_name failed (no actionable USB access error detected); see details in: $$strace_log"; \
		fi; \
	fi; \
	exit $$rc; \
}
endef

PHONY += program check-params

# Validate inputs and derived values before generating XCF or programming.
check-params: sys-env sys-env-check $(LATTICE_PROG_INFO)
	$(call print,[Checking parameters for programming from: $(LATTICE_PROG_INFO) ...])
	@. "$(LATTICE_SYS_ENV_VARS)"; \
	. "$(LATTICE_PROG_INFO)"; \
	dev_name="$${DEVICE_NAME:-$$LATTICE_DEVICE_NAME}"; \
	echo "Device  : $$dev_name"; \
	echo "Family  : $$FAMILY"; \
	echo "IDCode  : $$IDCODE"; \
	echo "Bitfile : $(LATTICE_BIT_PATH)"; \
	echo "Cable   : $(LATTICE_CABLE)  Port: $(LATTICE_PORT)"; \
	[ -n "$$FAMILY" ]    || { echo "ERROR: device '$$dev_name' not found in XDF"; exit 1; }; \
	[ -n "$$IDCODE" ]    || { echo "ERROR: IDCode not found for '$$dev_name'"; exit 1; }
	@[ -n "$(LATTICE_BIT)" ]       || (echo "ERROR: no .bit file found (LATTICE_BIT is empty)"; exit 1)
	@[ -n "$(LATTICE_BIT_PATH)" ]  || (echo "ERROR: LATTICE_BIT_PATH is empty (cygpath conversion failed?)"; exit 1)
	@[ -n "$(LATTICE_XCF_PATH)" ]  || (echo "ERROR: LATTICE_XCF_PATH is empty (abspath/cygpath conversion failed?)"; exit 1)
	$(call print,[$(LATTICE_PROG_INFO) OK])

# Generate the XCF programming script from the template and resolved values.
$(LATTICE_OUT_XCF): $(LATTICE_XCF_TMPL) $(LATTICE_BIT) $(LATTICE_PROG_INFO) | check-params
	$(call print,[Patching XCF... input: $(LATTICE_XCF_TMPL), output: $(LATTICE_OUT_XCF) ...])
	@cp "$(LATTICE_XCF_TMPL)" "$(LATTICE_OUT_XCF)"
	@. "$(LATTICE_PROG_INFO)"; \
	. "$(LATTICE_SYS_ENV_VARS)"; \
	dev_name="$${DEVICE_NAME:-$$LATTICE_DEVICE_NAME}"; \
	sed -i \
		-e 's|<Family>[^<]*</Family>|<Family>'"$$FAMILY"'</Family>|' \
		-e 's|<Name>[^<]*</Name>|<Name>'"$$dev_name"'</Name>|' \
		-e 's|<IDCode>[^<]*</IDCode>|<IDCode>'"$$IDCODE"'</IDCode>|' \
		-e 's|<PON>[^<]*</PON>|<PON>'"$$PON"'</PON>|' \
		-e 's|<File>[^<]*</File>|<File>$(LATTICE_BIT_PATH)</File>|' \
		-e 's|<TCKDelay>[^<]*</TCKDelay>|<TCKDelay>$(LATTICE_TCK_DELAY)</TCKDelay>|' \
		-e 's|<CableName>[^<]*</CableName>|<CableName>$(LATTICE_CABLE)</CableName>|' \
		-e 's|<PortAdd>[^<]*</PortAdd>|<PortAdd>$(LATTICE_PORT)</PortAdd>|' \
		"$(LATTICE_OUT_XCF)"
	$(call print,[XCF written to: $(LATTICE_OUT_XCF)])

# Program the FPGA via pgrcmd using the generated XCF.
program: $(LATTICE_OUT_XCF)
	@$(LATTICE_USB_HELPERS); \
	. "$(LATTICE_SYS_ENV_VARS)"; \
	. "$(LATTICE_PROG_INFO)"; \
	dev_name="$${DEVICE_NAME:-$$LATTICE_DEVICE_NAME}"; \
	$(call print,[Downloading $(LATTICE_BIT_PATH) to $$dev_name ...]); \
	run_lattice_usb_cmd pgrcmd -infile "$(LATTICE_XCF_PATH)"