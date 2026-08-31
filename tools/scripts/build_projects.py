#!/usr/bin/env python3

import argparse
import json
import os
import subprocess
import multiprocessing
import sys
import filecmp
import re
import time
# This file can be downloaded from the wiki-scripts repository
# https://raw.githubusercontent.com/analogdevicesinc/wiki-scripts/refs/heads/main/utils/cloudsmith_utils/cloudsmith_helper.py
from cloudsmith_helper import *
from pathlib import Path
# Discovery helpers for the CMake build system. no_os_build.py guards its CLI
# under "if __name__ == '__main__':", so importing it has no side effects.
from no_os_build import (
	CMAKE,
	load_presets,
	discover_all_combinations,
	filter_combinations,
	combo_build_dir,
	xilinx_hardware_name,
)

# Platforms handled by the CMake build system (the only ones with board
# presets under board_configs/). A project's builds.json is expected to carry
# only the platforms NOT in this set; its CMake combos cover these.
CMAKE_PLATFORMS = {'maxim', 'stm32', 'pico', 'aducm3029', 'xilinx'}

TGREEN =  '\033[32m' # Green Text	
TBLUE =  '\033[34m' # Green Text	
TRED =  '\033[31m' # Red Text	
TWHITE = '\033[39m' #Withe text

description_help='''Build noos projects
Examples:\n
	Build all noos projects
    	>python build_projects.py ..\.. export_dir log_dir
	Build all configurations for iio_demo
	>python build_projects.py /home/user/noos /home/export_dir log_dir -project=iio_demo
	Build all configurations for iio_demo
	>python tools/scripts/build_projects.py . export logs -project=iio_demo -platform=xilinx -build_name=iio_zed

	Note: HDF_SERVER should be sent as enviroment variables:
		Ex: export HDF_SERVER=ala.bala.com/hdf_builds

	Note: When using custom location for the build output (i.e. export_dir, log_dir, builds_dir) specify the entire path.
'''

def parse_input():
	parser = argparse.ArgumentParser(description=description_help,\
				formatter_class=argparse.RawTextHelpFormatter)
	parser.add_argument('noos_location', help="Path to noos location")
	parser.add_argument('-export_dir', default=(os.getcwd() + '/exports'), help="Path where to save files")
	parser.add_argument('-log_dir', default=(os.getcwd() + '/logs'), help="Path where to save log files")
	parser.add_argument(
		'-projects',
		help="List of projects to be built",
		nargs='+'
	)
	parser.add_argument('-platform', help="Name of platform to be built")
	parser.add_argument('-builds_dir', default=(os.getcwd() +'/builds'), help="Directory where to build projects")
	parser.add_argument('-hdl_branch', default='main', help="Name of hdl_branch from which to downlaod hardware or \
					 we can also specify a timestamp folder from the specific branch but needs to have a specific format, \
					 of 'branch_name/YYYY_mm_dd-HH_MM_SS' example: main/2023_09_20-06_52_29")
	args = parser.parse_args()

	return (args.noos_location, args.export_dir, args.log_dir, 
		 args.builds_dir, args.platform, args.hdl_branch,args.projects)

ERR = 0
LOG_START = " -> "
TOKEN = os.environ.get('TOKEN')
BRANCH = os.environ.get('BRANCH')
blacklist_url = str(os.environ.get('BLACKLIST_URL')).format(BRANCH)
environment_path_files = os.environ.get('ENVIRONMENT_PATH_FILES')

def log(msg):
	print(TGREEN + LOG_START + TWHITE + msg)

def log_err(msg):
	print(TRED + LOG_START + msg + TWHITE)

def log_success(msg):
	print(TGREEN + LOG_START + msg + TWHITE)

DEFAULT_LOG_FILE = 'log.txt'
log_file = DEFAULT_LOG_FILE

def ensure_dir(path):
	# Silent mkdir -p; not routed through run_cmd so it neither logs a bogus
	# build step nor leaks the "test -d ... ||" text to stdout.
	os.makedirs(path, exist_ok=True)

def shell_source(script):
	"""
	Sometime you want to emulate the action of "source" in bash,
	settings some environment variables. Here is a way to do it.
	"""

	pipe = subprocess.Popen(". %s && env -0" % script, stdout=subprocess.PIPE, shell=True, executable="/bin/bash")
	output = pipe.communicate()[0].decode('utf-8')
	output = output[:-1] # fix for index out for range in 'env[ line[0] ] = line[1]'

	env = {}
	# split using null char
	for line in output.split('\x00'):
		line = line.split( '=', 1)
		#print(line)
		env[ line[0] ] = line[1]

	os.environ.update(env)

def run_cmd(cmd):
	global ERR
	log(cmd)
	sys.stdout.flush()
	err = os.system('echo %s >> %s' % (cmd, log_file))
	if err != 0:
		ERR = 1
		return err
	err = os.system(cmd + ' >> %s 2>&1' % log_file)
	if err != 0:
		log_err("ERROR")
		log("See log %s " \
		    "-- Use cat (linux) or type (windows) to see colored output"
		    % log_file)
		ERR = 1

	return err

def to_blue(str):
	return TBLUE + str + TWHITE

SKIP_DOWNLOAD = None
key = 'SKIP_DOWNLOAD'
if key in os.environ:
	SKIP_DOWNLOAD = int(os.environ[key])
else:
	SKIP_DOWNLOAD = 0

HW_DIR_NAME = 'hardware'
NEW_HW_DIR_NAME = 'new_hardware'

def process_blacklist():
	blacklist = []
	log('Fetching blacklist from %s' % blacklist_url)
	result = subprocess.run(
		['curl', '-f', '-L',
		 '-H', 'Accept: application/vnd.github.v3.raw',
		 '-H', 'Authorization: Bearer %s' % TOKEN,
		 '-o', 'blacklist.txt', blacklist_url],
		capture_output=True)
	if result.returncode != 0:
		fallback_url = str(os.environ.get('BLACKLIST_URL')).format('main')
		if fallback_url != blacklist_url:
			log('Blacklist for branch %s not found, falling back to main' % BRANCH)
			result = subprocess.run(
				['curl', '-f', '-L',
				 '-H', 'Accept: application/vnd.github.v3.raw',
				 '-H', 'Authorization: Bearer %s' % TOKEN,
				 '-o', 'blacklist.txt', fallback_url],
				capture_output=True)
		if result.returncode != 0:
			log_err('Failed to download blacklist (curl exit %d) -- proceeding without filtering'
				% result.returncode)
			return blacklist
	try:
		with open('blacklist.txt', 'r') as f:
			for line in f:
				project = line.split('#')[0].rstrip().replace('.', '_')
				if project != '':
					blacklist.append(project)
	finally:
		if os.path.isfile('blacklist.txt'):
			os.remove('blacklist.txt')
	if blacklist:
		log('Blacklist loaded: %d project(s) excluded' % len(blacklist))
	else:
		log('Blacklist file is empty -- no projects excluded')
	return blacklist

def configfile_and_download_all_hw(_platform, noos, _builds_dir, hdl_branch, projects = list):
	server_base_path = "hdl/"
	hdl_repo = 'sdg-hdl'
	pattern = r'\d{4}_\d{2}_\d{2}-\d{2}_\d{2}_\d{2}'
	blacklist = []
	timestamp_match = re.search(pattern, hdl_branch)
	if timestamp_match:
		hdl_branch = hdl_branch.split('/')[0]
		timestamp_folder = timestamp_match.group()

	if hdl_branch == "main":
		hdl_branch_path = hdl_branch + '/hdl_output/'
	else:
		if check_path(package_version=server_base_path + 'releases/' + hdl_branch + '/', repo=hdl_repo):
			hdl_branch_path = 'releases/' + hdl_branch + '/hdl_output/'
		elif check_path(package_version=server_base_path + 'dev/' + hdl_branch + '/', repo=hdl_repo):
			hdl_branch_path = 'dev/' + hdl_branch + '/hdl_output/'
		else:
			log_err("HDL branch '%s' not found in releases/ or dev/" % hdl_branch)
			sys.exit(1)

	if timestamp_match:
		if check_path(package_version=server_base_path + hdl_branch_path + timestamp_folder + '/', repo=hdl_repo):
			hdl_branch_path += timestamp_folder + '/'
		else:
			log_err("Timestamp folder '%s' not found in hdl_branch '%s'" % (timestamp_folder, hdl_branch))
			sys.exit(1)

	builds_dir = _builds_dir + '_' + hdl_branch
	ensure_dir(builds_dir)
	if SKIP_DOWNLOAD == 1:
		return (builds_dir, [])
	hardwares = os.path.join(builds_dir, HW_DIR_NAME)
	ensure_dir(hardwares)
	server_full_path = server_base_path + hdl_branch_path
	if (_platform is None or _platform == 'xilinx'):
		if not TOKEN:
			log('Skipping blacklist: TOKEN not set')
		elif not blacklist_url or blacklist_url == 'None':
			log('Skipping blacklist: BLACKLIST_URL not set')
		else:
			blacklist = process_blacklist()
		new_hardwares = os.path.join(builds_dir, NEW_HW_DIR_NAME)
		ensure_dir(new_hardwares)
		
		projects_list = ','.join(projects)

		download_cmd = [
			os.path.join(noos, 'tools', 'scripts', 'download_files.py'),
			noos, builds_dir, server_full_path, str(blacklist), projects_list]
		result = subprocess.run(download_cmd)
		if result.returncode != 0:
			log_err("Hardware download failed (exit %d)" % result.returncode)
			sys.exit(1)
	return (builds_dir, blacklist)

# Xilinx BSP freshness is validated per-build-dir via xsa_work/.bsp_stamp
# in config_xilinx_sdk (cmake/xilinx/xilinx_platform_sdk.cmake).

def get_hardware(hardware, platform, builds_dir):
	if platform == 'xilinx':
		ext = 'xsa'
		base_name = 'system_top'
	else:
		ext = 'sopcinfo'
		base_name = 'system_bd'

	new_name = "%s.%s" % (base_name, ext)
	tmp_filename = os.path.join(builds_dir, NEW_HW_DIR_NAME, hardware, new_name)
	old_name = "%s.%s" % (hardware, ext)
	filename = os.path.join(builds_dir, HW_DIR_NAME, old_name)

	if os.path.isfile(filename):
		#If equal
		if filecmp.cmp(filename, tmp_filename):
			log("Same hardware from last build, use existing bsp")
			return (filename, 0, 0)

	err = run_cmd('cp %s %s' % (tmp_filename, filename))
	if err != 0:
		return ('', 1, err)

	log("Hardware changed from last build")

	return (filename, 1, err)

def build_cmake_project(noos, project, _platform, export_dir, log_dir, cmake_builds_dir, builds_dir):
	"""Build the CMake/Kconfig (Maxim/STM32/Pico/Xilinx) side of a project.

	Discovers the project's project/variant/board combinations from the board
	presets and per-project *.conf files (reusing no_os_build.py for discovery)
	and builds each by invoking no_os_build.py as a subprocess, so the CI console
	shows the invocation while its output is redirected into the per-combination
	log.

	For xilinx combos the per-(variant, board) hardware name is resolved and its
	downloaded .xsa (in <builds_dir>/hardware/, placed by get_hardware) is passed
	through to cmake as --hardware. builds_dir is the hardware-cache root (the
	same dir configfile_and_download_all_hw populated).

	Returns 1 if all combinations succeeded, 0 if any failed, or None if there
	were no combinations for the requested platform (so the caller can tell a
	genuine no-op from a real build and avoid emitting a misleading status).
	"""
	global ERR, log_file

	presets = load_presets(Path(noos))
	combos = discover_all_combinations(Path(noos), presets)
	combos = filter_combinations(combos, project=project)
	# Only the CMake platforms are built here; the rest live in builds.json.
	# Honor the platform / build-name (variant) filters the CLI already supports.
	combos = [c for c in combos if c['platform'] in CMAKE_PLATFORMS]
	if _platform is not None:
		combos = [c for c in combos if c['platform'] == _platform]

	if not combos:
		return None

	project_export = os.path.join(export_dir, project)
	ensure_dir(project_export)

	build_dir_base = Path(cmake_builds_dir)
	ok = 1
	for combo in combos:
		variant = combo['variant']
		board = combo['board']
		platform = combo['platform']

		name = "%s-%s-%s" % (project, variant, board)
		build_dir = combo_build_dir(build_dir_base, combo)
		out_dir = build_dir / 'build'
		elf = out_dir / ('%s.elf' % project)

		log("Building %10s (%8s) -- %s -- %s" % (
			to_blue(project), to_blue(variant), to_blue(platform), to_blue(board)))

		# Bring in the platform SDK environment (MAXIM_LIBRARIES, STM32CUBEMX, ...).
		env = dict(os.environ)
		
		# The final link + .hex/.bin generation runs as a custom command whose
		# failure does NOT report as a non-zero exit. So the .elf is the source
		# of truth: remove any stale one, build, then require it.
		if elf.is_file():
			elf.unlink()

		# Xilinx builds need the board .xsa: resolve the hardware name for this
		# (variant, board) and pass the downloaded file through to cmake. The
		# .xsa was fetched into <builds_dir>/new_hardware/<name>/system_top.xsa
		# by configfile_and_download_all_hw; get_hardware copies/renames it into
		# <builds_dir>/hardware/<name>.xsa and returns that path.
		hardware_arg = ""
		new_hdf = False
		if platform == 'xilinx':
			hw_name = xilinx_hardware_name(Path(noos), project, variant, board)
			if not hw_name:
				log_err("ERROR")
				log("%s/%s/%s: no CONFIG_XILINX_HDL_DESIGN in %s.conf" % (
					project, variant, board, variant))
				ERR = 1
				ok = 0
				os.environ.clear(); os.environ.update(env)
				continue
			(hardware_file, new_hdf, hw_err) = get_hardware(hw_name, 'xilinx', builds_dir)
			if hw_err != 0 or not hardware_file:
				log_err("ERROR")
				log("%s: could not resolve .xsa for hardware '%s' (not downloaded?)" % (
					project, hw_name))
				ERR = 1
				ok = 0
				os.environ.clear(); os.environ.update(env)
				continue
			hardware_arg = " --hardware %s" % hardware_file

		# Delegate the actual build to no_os_build.py. Suppress its
		# spinner/summary (not useful on CI); the real cmake output lands in
		# build.log inside the build dir and is copied into dst_log below.
		dst_log = os.path.join(log_dir, '%s_%s_%s_%s.txt' % (project, platform, variant, board))
		jobs = int(multiprocessing.cpu_count() / 2) or 1

		# Pass an absolute --build-dir: no_os_build anchors a relative one to the
		# repo root, which would not match the build_dir we clean/probe here.
		# Xilinx: reuse cached BSP unless the .xsa changed (new_hdf) or no cache file exists yet. 
		# Shared build dirs mean CMakeCache paths won't match the current checkout, but the BSP
		# artifacts don't depend on source paths - only the .xsa matters.
		fresh_flag = " --fresh"
		if _platform == 'xilinx':
			cmake_cache_file = build_dir / 'CMakeCache.txt'
			if (not new_hdf and os.path.isfile(cmake_cache_file)): fresh_flag = ""

			# Update cmake cache with current working directory
			if fresh_flag == "":
				cmake_cache_file_new = build_dir / 'CMakeCache_new.txt'
				cmake_toolchain_new = f"CMAKE_TOOLCHAIN_FILE:FILEPATH={noos}/drivers/platform/xilinx/toolchain.cmake\n"
				with open(cmake_cache_file, "r") as old:
					lines = old.readlines()
					with open(cmake_cache_file_new, "w") as new:
						for line in lines:
							if "CMAKE_TOOLCHAIN_FILE:FILEPATH=" in line:
								line = cmake_toolchain_new
							elif "no-os_SOURCE_DIR:STATIC" in line:
								line = f"no-os_SOURCE_DIR:STATIC={noos}\n"
							elif "CMAKE_HOME_DIRECTORY:INTERNAL" in line:
								line = f"CMAKE_HOME_DIRECTORY:INTERNAL={noos}\n"
							new.write(line)
					os.remove(cmake_cache_file)
					os.rename(cmake_cache_file_new, cmake_cache_file)

		build_cmd = ("python3 %s/tools/scripts/no_os_build.py build"
			     " --project %s --variant %s --board %s"
			     " --build-dir %s --jobs %d --probe openocd%s%s"
			     % (noos, project, variant, board,
				os.path.abspath(build_dir_base), jobs, fresh_flag, hardware_arg))
		log(build_cmd)
		sys.stdout.flush()
		err = os.system(build_cmd + ' > /dev/null 2>&1')
		success = err == 0

		os.environ.clear()
		os.environ.update(env)

		# Copy the cmake configure+build log (written by no_os_build into the
		# build dir) into the per-combo CI log artifact.
		cmake_log = build_dir / 'build.log'
		if cmake_log.is_file():
			import shutil
			shutil.copy(str(cmake_log), dst_log)
		else:
			open(dst_log, 'w').close()

		if not success:
			log_err("ERROR")
			log("See log %s" % dst_log)
			ERR = 1

		# The final link + .hex/.bin runs as a cmake custom command whose failure
		# does NOT propagate as a non-zero exit. Check for the .elf explicitly.
		if success and not elf.is_file():
			log_err("ERROR")
			log("See log %s -- no .elf produced (link likely failed)" % dst_log)
			ERR = 1
			success = False

		if not success:
			ERR = 1
			ok = 0
			continue

		for ext in ('elf', 'hex', 'bin', 'uf2'):
			src = out_dir / ('%s.%s' % (project, ext))
			if src.is_file():
				run_cmd("cp %s %s" % (src, os.path.join(project_export, '%s.%s' % (name, ext))))

		log_success("DONE")

	return ok

def check_built_projects(projects = dict):
	new_projects = {}
	for project, status in projects.items():
		if "not_built" in status:
			new_projects[project] = status
	if bool(new_projects):
		return new_projects
	
	return False

def build_loop(projects, noos_dir, projects_dir, export_dir, log_dir, builds_dir, platform):
	new_projects = {}
	for project, status in projects.items():
		project_dir = os.path.join(projects_dir, project)

		# CMake is the only build system; skip projects without a CMakeLists.txt.
		if not os.path.isfile(os.path.join(project_dir, 'CMakeLists.txt')):
			continue
	
		all_status = os.path.join(log_dir, 'all_builds.txt')

		# CMake/Kconfig side: combos discovered from the board presets.
		# build_cmake_project filters to CMAKE_PLATFORMS internally and returns
		# None when the current -platform job has nothing to build here.
		cmake_builds_dir = builds_dir + '_cmake'
		ensure_dir(cmake_builds_dir)

		lockfile_path = os.path.join(cmake_builds_dir, f"{project}_{platform}.lock")
		if not os.path.isfile(lockfile_path):
			lockfile_validation_path = os.path.join(noos_dir, f"{project}_{platform}.lock")
			try:
				open(lockfile_path, "w").close()
				open(lockfile_validation_path, "w").close()

				cmake_ok = build_cmake_project(noos_dir, project, platform, export_dir, log_dir, cmake_builds_dir, builds_dir)
				if cmake_ok is not None:
					status = 'OK' if cmake_ok == 1 else 'Fail'
					os.system('echo Project %20s -- %s >> %s' % (project, status, all_status))
			finally:
				if os.path.isfile(lockfile_path):
					os.remove(lockfile_path)
				if os.path.isfile(lockfile_validation_path):
					os.remove(lockfile_validation_path)

			new_projects[project] = "done"
		else:
			new_projects[project] = "not_built"
			log("%s is already in another build process. Skipping and building it later ..." % project)

	return new_projects

def _finalize_bsp_hash(builds_dir, noos_dir):
	"""Update BSP hash file and remove regeneration marker after build completes."""
	if not noos_dir:
		return
	hash_file = os.path.join(builds_dir, HW_DIR_NAME, '.bsp_deps_hash')
	regen_marker = os.path.join(builds_dir, HW_DIR_NAME, '.bsp_regen_needed')
	# Update hash file with current hash
	current_hash = _get_bsp_deps_hash(noos_dir)
	with open(hash_file, 'w') as f:
		f.write(current_hash)
	# Remove regeneration marker
	if os.path.isfile(regen_marker):
		os.remove(regen_marker)

def main():
	(noos, export_dir, log_dir, _builds_dir, _platform, hdl_branch, _projects) = parse_input()
	projects_dir = os.path.join(noos,'projects')
	ensure_dir(export_dir)
	ensure_dir(log_dir)

	# Create a dictionary for projects to keep the status of currently building projects. If changes are happening
	# on dependency paths, it'll take the whole projects/ list
	if 'all' not in _projects:
		projects = _projects
	else:
		projects = os.listdir(projects_dir)
	projects_dict = {}
	for project in projects:
		projects_dict[project] = "not_built"

	(builds_dir, blacklist) = configfile_and_download_all_hw(_platform, noos, _builds_dir, hdl_branch, projects)

	# Build the projects and re-check for non built ones
	while bool(projects_dict):
		projects_dict = build_loop(projects_dict, noos, projects_dir, export_dir, log_dir, builds_dir, _platform)
		projects_dict = check_built_projects(projects_dict)
		# In case the remaining list on not_build projects is equivalent to the number of currently
		# building ones, prevent loop spamming.
		sys.stdout.flush()
		time.sleep(10)

main()

if ERR != 0:
	exit("Error occured: %d" % ERR)
