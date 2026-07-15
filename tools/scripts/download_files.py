import sys
import json
import os
import re
import ast
# This file can be downloaded from the wiki-scripts repository
# https://raw.githubusercontent.com/analogdevicesinc/wiki-scripts/refs/heads/main/utils/cloudsmith_utils/cloudsmith_helper.py
from cloudsmith_helper import *

TYELLOW = '\033[33m'  # Yellow Text
TWHITE = '\033[39m' # White text

def log_warn(msg):
    print(TYELLOW + msg + TWHITE)

NOOS_PATH = sys.argv[1]
BUILD_PATH = sys.argv[2]
HDL_SERVER_BASE_PATH = sys.argv[3]
blacklist = ast.literal_eval(sys.argv[4])
NEW_HW_DIR_NAME = 'new_hardware'
FOLDERS_NR = 30 #number of folders to check for missing hardware file

list_hardware = []
for dir in os.listdir(NOOS_PATH + '/projects'):
    file = NOOS_PATH + '/projects/' + str(dir) + '/builds.json'
    if os.path.exists(file):
        with open(file) as f:
            data = json.load(f)
        for (platform, config) in data.items():
            for (build_name, params) in config.items():
                if 'hardware' in params:
                    for hardware_value in params['hardware']:
                        list_hardware.append(hardware_value)

# CMake-migrated Xilinx projects carry no 'hardware' array in builds.json (the
# xilinx entry is removed on migration). Their .xsa is instead described by
# CONFIG_XILINX_HDL_DESIGN in the variant .conf, combined with the CMake board
# to form <design>_<board> (e.g. adv7511_zed). Collect those too so migrated
# projects stay in the bulk download set. Guarded: a discovery import failure
# must not break the legacy builds.json path above.
try:
    sys.path.insert(0, os.path.join(NOOS_PATH, 'tools', 'scripts'))
    from no_os_build import (
        load_presets, discover_projects, discover_variants,
        discover_boards_for_variant, xilinx_hardware_name,
    )
    from pathlib import Path
    _root = Path(NOOS_PATH)
    # Restrict to boards with a xilinx preset; a variant's HDL_DESIGN otherwise
    # composes bogus names for its non-xilinx boards, wasting API walk-backs.
    _xilinx_boards = {p['board'] for p in load_presets(_root).values()
                      if p.get('platform') == 'xilinx'}
    for project in discover_projects(_root):
        for variant in discover_variants(_root, project):
            boards, _ = discover_boards_for_variant(_root, project, variant)
            for board in boards:
                if board not in _xilinx_boards:
                    continue
                name = xilinx_hardware_name(_root, project, variant, board)
                if name:
                    list_hardware.append(name)
except Exception as e:
    log_warn("Could not scan CMake .conf for xilinx hardware: %s" % e)

new_harware_dir= os.path.join(BUILD_PATH, NEW_HW_DIR_NAME)
os.system("rm -rf %s/*" % (new_harware_dir))
unique_hardware_list = set(list_hardware)
for item in blacklist:
    if item in unique_hardware_list:
        unique_hardware_list.remove(item)
pattern= '\d{4}_\d{2}_\d{2}-\d{2}_\d{2}_\d{2}'
timestamp_match = re.search(pattern, HDL_SERVER_BASE_PATH)

if timestamp_match:
    for hardware in unique_hardware_list:
        # Skip a failing hardware rather than aborting the whole scan.
        try:
            file_path = HDL_SERVER_BASE_PATH + hardware + '/'
            if 'system_top.xsa' in get_files(package_version=file_path, repo='sdg-hdl'):
                os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
                get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
            else:
                log_warn("Missing " + hardware + " from specific timestamp " + timestamp_match.group())
        except Exception as e:
            log_warn("Error while downloading " + hardware + " from specific timestamp " + timestamp_match.group() + ": " + repr(e))
else:
    timestamp_folders = get_subfolders(package_version='hdl/main/hdl_output/', repo='sdg-hdl')
    timestamp_folders = (timestamp_folders[len(timestamp_folders) - FOLDERS_NR:])
    timestamp_folders.reverse()
    latest = timestamp_folders[0]
    release_link = HDL_SERVER_BASE_PATH + latest + "/"

    for hardware in unique_hardware_list:
        FOUND = False
        # Skip a failing hardware rather than aborting the whole scan.
        try:
            file_path = release_link + hardware + "/"
            if 'system_top.xsa' in get_files(package_version=file_path, repo='sdg-hdl'):
                os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
                get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
                FOUND = True
            else:
                log_warn("Missing " + hardware + " from latest timestamp " + latest)
                for timestamp_folder in timestamp_folders[1:]:
                    d_path = HDL_SERVER_BASE_PATH + timestamp_folder + "/" + hardware + "/"
                    if 'system_top.xsa' in get_files(package_version=d_path, repo='sdg-hdl'):
                        os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
                        get_artifacts_from_location(package_version=d_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                        os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
                        # Properties only enrich the log; never let them fail the download.
                        try:
                            file_properties = get_item_properties(package_version=d_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                        except Exception as e:
                            file_properties = None
                            log_warn("Could not read properties for " + hardware + " on timestamp " + timestamp_folder + ": " + repr(e))
                        if file_properties and len(file_properties) >= 2:
                            commit_date = file_properties[0].split('-', 1)[1]
                            git_sha = file_properties[1].split('-', 1)[1]
                            log_warn("Hardware " + hardware + " found on next timestamp " + timestamp_folder + " with next properties git sha: " + \
                                str(git_sha) + " commit date: " + str(commit_date))
                        else:
                            log_warn("Hardware " + hardware + " found on next timestamp " + timestamp_folder)
                        FOUND = True
                        break
        except Exception as e:
            log_warn("Error while downloading " + hardware + ": " + repr(e))
            continue
        if FOUND is False:
            log_warn("Project " + hardware + " was not found on server")
