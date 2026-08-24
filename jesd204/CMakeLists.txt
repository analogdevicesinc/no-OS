# JESD204 transport-layer core/state-machine. Consumed by the axi_core
# JESD204 drivers (axi_jesd204_rx/tx call jesd204_dev_register), so gate it on
# the same symbol. The public API lives in include/jesd204.h (already on the
# global include path); jesd204-priv.h is local to this dir.
no_os_sources_ifdef(CONFIG_AXI_CORE_JESD204
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204-core.c
    ${CMAKE_CURRENT_SOURCE_DIR}/jesd204-fsm.c)
target_include_directories(no-os PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
