import sys
import json
import os
import requests
import re
import ast
from cloudsmith_helper_1 import *

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
        file_path = HDL_SERVER_BASE_PATH + hardware + '/'
        print(file_path)
        if 'system_top.xsa' in get_files(package_version=file_path, repo='sdg-hdl'):
            os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
            get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
            os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
        else:
            log_warn("Missing " + hardware + " from specific timestamp " + timestamp_match.group())
else:
    timestamp_folders = get_subfolders(package_version='hdl/main/hdl_output/', repo='sdg-hdl')
    timestamp_folders = (timestamp_folders[len(timestamp_folders) - FOLDERS_NR:])
    timestamp_folders.reverse()
    latest = timestamp_folders[0]
    release_link = HDL_SERVER_BASE_PATH + latest + "/"

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
                d_path = HDL_SERVER_BASE_PATH + "/" + timestamp_folder + "/" + hardware + "/"
                if 'system_top.xsa' in get_files(package_version=d_path, repo='sdg-hdl'):
                    os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
                    get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
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
