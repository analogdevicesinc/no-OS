import re
import sys
import json
import os

launch_data = {}

debug_file = open(os.path.join(os.path.dirname(__file__), "debug_config"), mode='r')
run_file = open(os.path.join(os.path.dirname(__file__), "run_config"), mode='r')

no_os = sys.argv[1]
binary = sys.argv[2]
project = sys.argv[3]
maxim_libraries = sys.argv[4]
target = sys.argv[5]

run_conf = run_file.read()
debug_conf = debug_file.read()
debug_conf = re.sub("BINARY", binary, debug_conf)
run_conf = re.sub("PROJECT", project, run_conf)
run_conf = re.sub("YYYY", target, run_conf)
debug_config = json.loads(debug_conf)
run_config = json.loads(run_conf)

debug_config['cwd'] = os.path.join(maxim_libraries, "..", "Tools", "OpenOCD", "scripts")
debug_config['serverpath'] = os.path.join(maxim_libraries, "..", "Tools", "OpenOCD", "openocd")
debug_config['armToolchainPath'] = os.path.join(maxim_libraries, "..", "Tools", "GNUTools", "bin")
debug_config['configFiles'].append(os.path.join("interface", "cmsis-dap.cfg"))
debug_config['configFiles'].append(os.path.join("target", target + ".cfg"))
debug_config['svdFile'] = os.path.join(maxim_libraries, "CMSIS", "Device", "Maxim", target.upper(), "Include", target + ".svd")

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
        for i, e in enumerate(launch_data['configurations']):
                if e['name'] == "Maxim Debug":
                        launch_data['configurations'][i] = default_debug_config['configurations'][0]
                        config_exists = True
                        break
        if config_exists == False:
                launch_data['configurations'].append(default_debug_config['configurations'][0])

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
        for i, e in enumerate(tasks_data['tasks']):
                if e['label'] == "Maxim Run":
                        tasks_data['tasks'][i] = default_run_config['tasks'][0]
                        run_config_exists = True
                if e['label'] == "Maxim Build":
                        tasks_data['tasks'][i] = default_run_config['tasks'][1]
                        build_config_exists = True
        if run_config_exists == False:
                tasks_data['tasks'].append(default_run_config['tasks'][0])
        if build_config_exists == False:
                tasks_data['tasks'].append(default_run_config['tasks'][1])

        tasks_file.close()
        
        tasks_file = open(tasks_path, "w")
        json.dump(tasks_data, tasks_file, indent=8) 
        tasks_file.close()
        
write_debug_config()
write_run_config()
