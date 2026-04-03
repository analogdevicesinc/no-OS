# Makefile fragment to parse Lattice sys_env.xml into make variables.

# Only load/generate parsed sys_env values when needed.

PHONY += sys-env
sys-env: $(LATTICE_SYS_ENV_VARS)
	$(call print,[Parsed sys_env.xml variables from: $(LATTICE_SYS_ENV_VARS) ...])
	@cat "$(LATTICE_SYS_ENV_VARS)"

PHONY += sys-env-check
sys-env-check: $(LATTICE_SYS_ENV_VARS)
	$(call print,[Checking sys env variables from: "$(LATTICE_SYS_ENV_VARS)" for consistency with $(LATTICE_RISCV_ARCH) and version variables ...])
	@. "$<"; \
	status=0; \
	arch_from_ext=rv32i; \
	case "$$EXT_M" in True|true|1|yes|YES) arch_from_ext=$${arch_from_ext}m;; esac; \
	case "$$EXT_A" in True|true|1|yes|YES) arch_from_ext=$${arch_from_ext}a;; esac; \
	case "$$EXT_F" in True|true|1|yes|YES) arch_from_ext=$${arch_from_ext}f;; esac; \
	case "$$EXT_C" in True|true|1|yes|YES) arch_from_ext=$${arch_from_ext}c;; esac; \
	arch_req=$$(printf '%s' "$(LATTICE_RISCV_ARCH)" | tr 'A-Z' 'a-z'); \
	if [ -n "$(LATTICE_RISCV_ARCH)" ] && [ "$$arch_from_ext" != "$$arch_req" ]; then \
		if [ "$(LATTICE_IGNORE_ARCH_CHECK)" = "y" ] || [ "$(LATTICE_IGNORE_ARCH_CHECK)" = "Y" ]; then \
			echo "arch mismatch ignored: sys_env encodes $$arch_from_ext (C=$$EXT_C M=$$EXT_M A=$$EXT_A F=$$EXT_F), expected $$arch_req"; \
		else \
			echo "ERROR: arch mismatch: sys_env encodes $$arch_from_ext (C=$$EXT_C M=$$EXT_M A=$$EXT_A F=$$EXT_F), expected $$arch_req"; \
			status=1; \
		fi; \
	fi; \
	if [ -n "$(LATTICE_PROPEL_VERSION)" ] && [ "$$PROPEL_VERSION" != "$(LATTICE_PROPEL_VERSION)" ]; then \
		if [ "$(LATTICE_IGNORE_VERSION_CHECK)" = "y" ] || [ "$(LATTICE_IGNORE_VERSION_CHECK)" = "Y" ]; then \
			echo "version mismatch ignored: Propel is $$PROPEL_VERSION, expected $(LATTICE_PROPEL_VERSION)"; \
		else \
			echo "ERROR: version mismatch: Propel is $$PROPEL_VERSION, expected $(LATTICE_PROPEL_VERSION)"; \
			status=1; \
		fi; \
	fi; \
	if [ -n "$(LATTICE_RISCV_RTOS_VERSION)" ] && [ "$$RISCV_RTOS_VERSION" != "$(LATTICE_RISCV_RTOS_VERSION)" ]; then \
		if [ "$(LATTICE_IGNORE_CPU_VERSION_CHECK)" = "y" ] || [ "$(LATTICE_IGNORE_CPU_VERSION_CHECK)" = "Y" ]; then \
			echo "version mismatch ignored: riscv_rtos is $$RISCV_RTOS_VERSION, expected $(LATTICE_RISCV_RTOS_VERSION)"; \
		else \
			echo "ERROR: version mismatch: riscv_rtos is $$RISCV_RTOS_VERSION, expected $(LATTICE_RISCV_RTOS_VERSION)"; \
			status=1; \
		fi; \
	fi; \
	exit $$status
	$(call print,[$(LATTICE_SYS_ENV_VARS) OK])

$(LATTICE_SYS_ENV_VARS): $(LATTICE_SYS_ENV)
	$(call print,[Parsing variables from: $(LATTICE_SYS_ENV) to $(LATTICE_SYS_ENV_VARS) ...])
	@mkdir -p $(dir $@)
	@tmp="$@.tmp"; rm -f "$$tmp"; \
	xml="$<"; \
	propel_version=$$(grep -oP '<lattice:version>\K[^<]+' "$$xml" | head -n1 || true); \
	device_name=$$(grep -oP '<lattice:deviceName>\K[^<]+' "$$xml" | head -n1 || true); \
	riscv_vendor=""; riscv_library=""; riscv_name=""; riscv_version=""; \
	inst_tag=$$(grep -oP '<lattice:instanceName[^>]*name="riscv_rtos"[^>]*>' "$$xml" | head -n1 || true); \
	riscv_vendor=$$(echo "$$inst_tag" | sed -n 's/.*vendor="\([^"]*\)".*/\1/p'); \
	riscv_library=$$(echo "$$inst_tag" | sed -n 's/.*library="\([^"]*\)".*/\1/p'); \
	riscv_name=$$(echo "$$inst_tag" | sed -n 's/.*name="\([^"]*\)".*/\1/p'); \
	riscv_version=$$(echo "$$inst_tag" | sed -n 's/.*version="\([^"]*\)".*/\1/p'); \
	ext_c=$$(awk '/<lattice:name[^>]*>C_EXT<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	ext_m=$$(awk '/<lattice:name[^>]*>M_EXT<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	ext_a=$$(awk '/<lattice:name[^>]*>A_EXT<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	ext_f=$$(awk '/<lattice:name[^>]*>F_EXT<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	jtag_type=$$(awk '/<lattice:name[^>]*>JTAG_TYPE<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	jtag_soft=$$(awk '/<lattice:name[^>]*>SOFT_JTAG<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	jtag_chan_ext=$$(awk '/<lattice:name[^>]*>JTAG_CHANNEL_EXTEND<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	jtag_chan=$$(awk '/<lattice:name[^>]*>JTAG_CHANNEL<\/lattice:name>/{f=1;next} \
		f&&/<lattice:value>/{v=$$0;sub(/.*<lattice:value>/,"",v);sub(/<\/lattice:value>.*/,"",v);print v;exit}' "$$xml" | head -n1); \
	printf 'PROPEL_VERSION=%s\n' "$$propel_version" > "$$tmp"; \
	printf 'LATTICE_DEVICE_NAME=%s\n' "$$device_name" >> "$$tmp"; \
	printf 'RISCV_RTOS_VLNV=%s\n' "$$riscv_vendor:$$riscv_library:$$riscv_name:$$riscv_version" >> "$$tmp"; \
	printf 'RISCV_RTOS_VERSION=%s\n' "$$riscv_version" >> "$$tmp"; \
	printf 'LATTICE_RISCV_ARCH=%s\n' "$(LATTICE_RISCV_ARCH)" >> "$$tmp"; \
	printf 'EXT_C=%s\n' "$$ext_c" >> "$$tmp"; \
	printf 'EXT_M=%s\n' "$$ext_m" >> "$$tmp"; \
	printf 'EXT_A=%s\n' "$$ext_a" >> "$$tmp"; \
	printf 'EXT_F=%s\n' "$$ext_f" >> "$$tmp"; \
	printf 'JTAG_TYPE=%s\n' "$$jtag_type" >> "$$tmp"; \
	printf 'SOFT_JTAG=%s\n' "$$jtag_soft" >> "$$tmp"; \
	printf 'JTAG_CHANNEL_EXTEND=%s\n' "$$jtag_chan_ext" >> "$$tmp"; \
	printf 'JTAG_CHANNEL=%s\n' "$$jtag_chan" >> "$$tmp"; \
	mv -f "$$tmp" "$@"
