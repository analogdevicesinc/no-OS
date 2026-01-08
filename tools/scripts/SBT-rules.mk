# This file is used by the Makefile system to extend the functionality
# of certain recipes to integrate with the Secure Boot Tools (SBT).
# It adds a few Make goals specific to the SBTs.
PROJECT_NAME=ad-ethernetAPLdevice-sl
$(info **********************************************************************)
$(info BUILD_DIR:  $(BUILD_DIR))
$(info PROJECT_NAME:    $(PROJECT_NAME))
$(info TARGET_SEC: $(TARGET_SEC))
$(info TEST_KEY:   $(TEST_KEY))
$(info **********************************************************************)
# The SLA target.
# "make sla" will generate a .sbin file, and then generate scp packets
# using the "build_scp_session" tool.
.PHONY: sla
sla: all
	$(info **********************************************************************)
	$(info **********************       sla: all       **************************)
	$(info **********************************************************************)
	@echo " "
	arm-none-eabi-objcopy $(BUILD_DIR)/$(PROJECT_NAME).elf -R .sig -O binary $(BUILD_DIR)/$(PROJECT_NAME).bin
	@echo " "
	$(CA_SIGN_BUILD) -c $(TARGET_SEC) key_file=$(TEST_KEY) ca=$(BUILD_DIR)/$(PROJECT_NAME).bin sca=$(BUILD_DIR)/$(PROJECT_NAME).sbin
	@echo " "
	arm-none-eabi-objcopy  $(BUILD_DIR)/$(PROJECT_NAME).elf --update-section .sig=$(BUILD_DIR)/$(PROJECT_NAME).sig
	@echo " "
	$(BUILD_SESSION) -c $(TARGET_SEC) key_file=$(TEST_KEY) ${SCP_PACKETS} $(BUILD_DIR)/$(PROJECT_NAME).sbin

# The SCPA target.
# "make scpa" is a special rule for SCPA applet programs, which are
# special examples that load a program into RAM to extend the secure
# ROM functionality.  It is mostly the same as the sla rule, except
# some special modifications are made to the srec file and scp packets.
# It depends on an "scp_script.txt" file
.PHONY:scpa
scpa: all
	@echo " "
	arm-none-eabi-objcopy -O srec -j .text -j .data -j.scpa_header -j.scpa_init -j.scpa_ops --srec-forceS3 --srec-len=128 $(BUILD_DIR)/$(PROJECT_NAME).elf  $(BUILD_DIR)/$(PROJECT_NAME).srec
	@echo " "
	@echo "Generating SCP Package"
	$(BUILD_SESSION) -c $(TARGET_SEC) key_file=$(TEST_KEY) script_file=scp_script.txt ${SCP_PACKETS}