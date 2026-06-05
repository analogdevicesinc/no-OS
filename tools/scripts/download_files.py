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

print("=" * 60, flush=True)
print("download_files.py starting", flush=True)
print("  sys.argv: %s" % sys.argv, flush=True)
print("=" * 60, flush=True)

NOOS_PATH = sys.argv[1]
BUILD_PATH = sys.argv[2]
HDL_SERVER_BASE_PATH = sys.argv[3]
print("  NOOS_PATH: %s" % NOOS_PATH, flush=True)
print("  BUILD_PATH: %s" % BUILD_PATH, flush=True)
print("  HDL_SERVER_BASE_PATH: %s" % HDL_SERVER_BASE_PATH, flush=True)
print("  Raw blacklist arg (sys.argv[4]): %r" % sys.argv[4], flush=True)

try:
    blacklist = ast.literal_eval(sys.argv[4])
    print("  Parsed blacklist: %s" % blacklist, flush=True)
except Exception as e:
    print("ERROR: Failed to parse blacklist: %s" % e, flush=True)
    print("ERROR: sys.argv[4] was: %r" % sys.argv[4], flush=True)
    sys.exit(1)

NEW_HW_DIR_NAME = 'new_hardware'
FOLDERS_NR = 30 #number of folders to check for missing hardware file

print("Scanning projects for hardware requirements...", flush=True)
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
print("  Found %d hardware references (with duplicates)" % len(list_hardware), flush=True)

new_harware_dir= os.path.join(BUILD_PATH, NEW_HW_DIR_NAME)
print("  new_harware_dir: %s" % new_harware_dir, flush=True)
os.system("rm -rf %s/*" % (new_harware_dir))
unique_hardware_list = set(list_hardware)
print("  Unique hardware count (before blacklist): %d" % len(unique_hardware_list), flush=True)
for item in blacklist:
    if item in unique_hardware_list:
        unique_hardware_list.remove(item)
print("  Unique hardware count (after blacklist): %d" % len(unique_hardware_list), flush=True)
pattern= '\d{4}_\d{2}_\d{2}-\d{2}_\d{2}_\d{2}'
timestamp_match = re.search(pattern, HDL_SERVER_BASE_PATH)
print("  Timestamp match in HDL_SERVER_BASE_PATH: %s" % (timestamp_match.group() if timestamp_match else "None"), flush=True)

if timestamp_match:
    for hardware in unique_hardware_list:
        file_path = HDL_SERVER_BASE_PATH + hardware + '/'
        if 'system_top.xsa' in get_files(package_version=file_path, repo='sdg-hdl'):
            os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
            get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
            os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
        else:
            log_warn("Missing " + hardware + " from specific timestamp " + timestamp_match.group())
else:
    print("  No timestamp in path, fetching timestamp folders from server...", flush=True)
    try:
        timestamp_folders = get_subfolders(package_version='hdl/main/hdl_output/', repo='sdg-hdl')
        print("  get_subfolders returned %d folders" % len(timestamp_folders), flush=True)
        if len(timestamp_folders) == 0:
            print("ERROR: get_subfolders returned empty list!", flush=True)
            sys.exit(1)
    except Exception as e:
        print("ERROR: get_subfolders failed: %s" % e, flush=True)
        sys.exit(1)
    timestamp_folders = (timestamp_folders[len(timestamp_folders) - FOLDERS_NR:])
    timestamp_folders.reverse()
    latest = timestamp_folders[0]
    print("  Latest timestamp folder: %s" % latest, flush=True)
    release_link = HDL_SERVER_BASE_PATH + latest + "/"
    print("  release_link: %s" % release_link, flush=True)

    for hardware in unique_hardware_list:
        FOUND = False
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
                    file_properties = get_item_properties(package_version=d_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                    if file_properties and len(file_properties) >= 2:
                        commit_date = file_properties[0].split('-', 1)[1]
                        git_sha = file_properties[1].split('-', 1)[1]
                        log_warn("Hardware " + hardware + " found on next timestamp " + timestamp_folder + " with next properties git sha: " + \
                            str(git_sha) + " commit date: " + str(commit_date))
                    else:
                        log_warn("Hardware " + hardware + " found on next timestamp " + timestamp_folder)
                    FOUND = True
                    break
        if FOUND is False:
            log_warn("Project " + hardware + " was not found on server")
