import re
import sys
import json
import os

launch_data = {}

debug_file = open(os.path.join(os.path.dirname(__file__), "debug_config"), mode="r")
run_file = open(os.path.join(os.path.dirname(__file__), "run_config"), mode="r")

binary = sys.argv[1]
project = sys.argv[2]
pico_sdk = sys.argv[3]
target = sys.argv[4]
server_path = sys.argv[5]
# Optional extra args. The default probe is cmsis-dap (a second Pico running
# debugprobe/picoprobe firmware); pass "jlink" to select a SEGGER J-Link.
probe = sys.argv[6] if len(sys.argv) > 6 else "cmsis-dap"
openocd_path = sys.argv[7] if len(sys.argv) > 7 else "openocd"
gdb_path = "gdb-multiarch"

run_conf = run_file.read()
debug_conf = debug_file.read()
debug_conf = re.sub("BINARY", binary, debug_conf)
run_conf = re.sub("PROJECT", project, run_conf)
run_conf = re.sub("YYYY", target, run_conf)
debug_config = json.loads(debug_conf)
run_config = json.loads(run_conf)

debug_config["gdbPath"] = os.path.join(gdb_path)

# Clear TIMER_DBGPAUSE (TIMER_BASE 0x40054000 + 0x2c). By default the RP2040
# timer is paused whenever a core is halted by the debugger, so busy_wait_us()/
# busy_wait_until() loops (e.g. the UART setup delay in hardware_uart/uart.c, or
# any sleep) spin forever under debug because the timer never advances.
#
# It must be cleared on every *halt*, not just at reset: the SDK runtime brings
# the TIMER block out of reset during startup, which restores DBGPAUSE to its
# 0x7 default *after* any connect-/reset-time clear. Clearing on the "halted"
# event re-applies it at every breakpoint/step (including run-to-main), so it
# sticks through the firmware's own init.
if probe == "jlink":
    # The "jlink" servertype talks to JLinkGDBServer directly and has no OpenOCD
    # event system. postLaunch/postReset GDB writes are the best available hook
    # (clears at launch and after each reset).
    debug_config["serverpath"] = server_path
    _dbgpause_clear = ["set {unsigned int}0x4005402c = 0"]
    debug_config["postLaunchCommands"] = _dbgpause_clear
    debug_config["postResetCommands"] = _dbgpause_clear
else:
    # Default: second Pico (debugprobe/picoprobe) via OpenOCD. The "openocd"
    # servertype uses configFiles for the interface and target.
    debug_config["servertype"] = "openocd"
    debug_config["serverpath"] = openocd_path
    debug_config["configFiles"] = [
        os.path.join("interface", "cmsis-dap.cfg"),
        os.path.join("target", target + ".cfg"),
    ]
    # OpenOCD "halted" event handler: clears DBGPAUSE every time the core halts,
    # independent of Cortex-Debug's command ordering and robust against the SDK
    # runtime re-arming it during startup.
    debug_config["openOCDLaunchCommands"] = [
        target + ".core0 configure -event halted { mww 0x4005402c 0 }"
    ]
debug_config["svdFile"] = os.path.join(
    pico_sdk, "src", target, "hardware_regs", target.upper() + ".svd"
)

default_debug_config = {"configurations": [debug_config]}
default_run_config = run_config

tasks_path = os.path.join(project, ".vscode")
tasks_path = os.path.join(tasks_path, "tasks.json")

launch_path = os.path.join(project, ".vscode")
launch_path = os.path.join(launch_path, "launch.json")


def write_debug_config():
    config_size = 0
    config_exists = False
    config_file_exists = os.path.exists(launch_path)

    if not os.path.exists(os.path.join(project, ".vscode")):
        os.mkdir(os.path.join(project, ".vscode"))
        launch_file = open(launch_path, "w+")
        launch_file.close()
    else:
        if config_file_exists:
            config_size = os.path.getsize(launch_path)
        else:
            launch_file = open(launch_path, "w+")
            launch_file.close()

    launch_file = open(launch_path, "r+")
    if not config_file_exists or config_size == 0:
        json.dump(default_debug_config, launch_file, indent=8)
        return
    launch_data = json.load(launch_file)
    for i, e in enumerate(launch_data["configurations"]):
        if e["name"] == "Pico Debug":
            launch_data["configurations"][i] = default_debug_config["configurations"][0]
            config_exists = True
            break
    if config_exists == False:
        launch_data["configurations"].append(default_debug_config["configurations"][0])

    launch_file.close()

    launch_file = open(launch_path, "w")
    json.dump(launch_data, launch_file, indent=8)
    launch_file.close()


def write_run_config():
    config_size = 0
    run_config_exists = False
    build_config_exists = False
    config_file_exists = os.path.exists(tasks_path)

    if not os.path.exists(os.path.join(project, ".vscode")):
        os.mkdir(os.path.join(project, ".vscode"))
        tasks_file = open(tasks_path, "w+")
        tasks_file.close()
    else:
        if config_file_exists:
            config_size = os.path.getsize(tasks_path)
        else:
            tasks_file = open(tasks_path, "w+")
            tasks_file.close()

    tasks_file = open(tasks_path, "r+")
    if not config_file_exists or config_size == 0:
        json.dump(default_run_config, tasks_file, indent=8)
        return
    tasks_data = json.load(tasks_file)
    for i, e in enumerate(tasks_data["tasks"]):
        if e["label"] == "Pico Run":
            tasks_data["tasks"][i] = default_run_config["tasks"][0]
            run_config_exists = True
        if e["label"] == "Pico Build":
            tasks_data["tasks"][i] = default_run_config["tasks"][1]
            build_config_exists = True
    if run_config_exists == False:
        tasks_data["tasks"].append(default_run_config["tasks"][0])
    if build_config_exists == False:
        tasks_data["tasks"].append(default_run_config["tasks"][1])

    tasks_file.close()

    tasks_file = open(tasks_path, "w")
    json.dump(tasks_data, tasks_file, indent=8)
    tasks_file.close()


write_debug_config()
write_run_config()
