#!/usr/bin/env python3
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2026 Analog Devices, Inc. All rights reserved.

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
pattern = r'\d{4}_\d{2}_\d{2}-\d{2}_\d{2}_\d{2}'
timestamp_match = re.search(pattern, HDL_SERVER_BASE_PATH)

# hardware -> HDL timestamp ("YYYY_MM_DD-HH_MM_SS") of the .xsa we staged.
# Written as a manifest at the end so build_projects.py can flag a
# found-but-stale HDL output (one whose newest published .xsa is old).
provenance = {}

if timestamp_match:
    for hardware in unique_hardware_list:
        # Skip a failing hardware rather than aborting the whole scan.
        try:
            file_path = HDL_SERVER_BASE_PATH + hardware + '/'
            if 'system_top.xsa' in get_files(package_version=file_path, repo='sdg-hdl'):
                os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
                get_artifacts_from_location(package_version=file_path, package_name= 'system_top.xsa', repo='sdg-hdl')
                os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
                provenance[hardware] = timestamp_match.group()
            else:
                log_warn("Missing " + hardware + " from specific timestamp " + timestamp_match.group())
        except Exception as e:
            log_warn("Error while downloading " + hardware + " from specific timestamp " + timestamp_match.group() + ": " + repr(e))
else:
    # Hardware-first resolution: fetch the full "<timestamp>/<hardware>/" map
    # for HDL_SERVER_BASE_PATH once, then for each hardware pick the NEWEST
    # timestamp that actually contains a system_top.xsa. Unlike a fixed
    # timestamp-folder window, this never "ages out" a design that simply has
    # not been rebuilt recently -- it always resolves the latest published
    # .xsa, however old (its provenance is logged below).
    structure = get_folder_and_files_structure(package_version=HDL_SERVER_BASE_PATH, repo='sdg-hdl')

    # hardware -> newest "<timestamp>" whose folder holds a system_top.xsa.
    # Timestamp names (YYYY_MM_DD-HH_MM_SS) compare chronologically as strings.
    latest_ts = {}
    for rel_path, files in structure.items():
        if 'system_top.xsa' not in files:
            continue
        parts = rel_path.strip('/').split('/')
        if len(parts) != 2:
            continue
        ts, hardware_name = parts
        if hardware_name not in latest_ts or ts > latest_ts[hardware_name]:
            latest_ts[hardware_name] = ts

    for hardware in unique_hardware_list:
        ts = latest_ts.get(hardware)
        if ts is None:
            log_warn("Project " + hardware + " was not found on server")
            continue
        d_path = HDL_SERVER_BASE_PATH + ts + "/" + hardware + "/"
        # Skip a failing hardware rather than aborting the whole scan.
        try:
            os.system("mkdir -p %s" % (str(new_harware_dir) + '/' + hardware))
            get_artifacts_from_location(package_version=d_path, package_name='system_top.xsa', repo='sdg-hdl')
            os.system("mv ./system_top.xsa %s" % (str(new_harware_dir) + '/' + hardware))
        except Exception as e:
            log_warn("Error while downloading " + hardware + " from timestamp " + ts + ": " + repr(e))
            continue
        provenance[hardware] = ts
        # Provenance of the .xsa we will build against. Best-effort: a missing
        # tag must never fail the download.
        git_sha = "unknown"
        try:
            props = get_item_properties_as_dict(package_version=d_path, package_name='system_top.xsa', repo='sdg-hdl')
            if props.get('git_sha'):
                git_sha = props['git_sha'][0]
        except Exception as e:
            log_warn("Could not read properties for " + hardware + " on timestamp " + ts + ": " + repr(e))
        print("Building " + hardware + ": using system_top.xsa from timestamp " + ts + " (git sha " + git_sha + ")")

# Persist the hardware -> HDL-timestamp map next to the staged .xsa files so
# build_projects.py can flag a found-but-stale HDL output. Best-effort: a write
# failure must not fail the download.
manifest_path = os.path.join(new_harware_dir, 'xsa_provenance.json')
try:
    with open(manifest_path, 'w') as f:
        json.dump(provenance, f)
except OSError as e:
    log_warn("Could not write xsa provenance manifest: " + repr(e))
