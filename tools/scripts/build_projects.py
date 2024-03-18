#!/bin/python

import argparse
import json
import os
import subprocess
import multiprocessing
import sys
import filecmp
import requests
import re

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
	parser.add_argument('-project', help="Name of project to be built")
	parser.add_argument('-platform', help="Name of platform to be built")
	parser.add_argument('-hardware', help="Name of hardware to be built")
	parser.add_argument('-build_name', help="Name of built type to be built")
	parser.add_argument('-builds_dir', default=(os.getcwd() +'/builds'), help="Directory where to build projects")
	parser.add_argument('-hdl_branch', default='main', help="Name of hdl_branch from which to downlaod hardware or \
					 we can also specify a timestamp folder from the specific branch but needs to have a specific format, \
					 of 'branch_name/YYYY_mm_dd-HH_MM_SS' example: main/2023_09_20-06_52_29")
	args = parser.parse_args()

	return (args.noos_location, args.export_dir, args.log_dir, args.project,
		args.platform, args.build_name, args.builds_dir, args.hardware, args.hdl_branch)

ERR = 0
LOG_START = " -> "
USER = os.environ.get('USER')
TOKEN = os.environ.get('TOKEN')
BRANCH = os.environ.get('BRANCH')
blacklist_url = str(os.environ.get('BLACKLIST_URL')).format(BRANCH)

def log(msg):
	print(TGREEN + LOG_START + TWHITE + msg)

def log_err(msg):
	print(TRED + LOG_START + msg + TWHITE)

def log_success(msg):
	print(TGREEN + LOG_START + msg + TWHITE)

DEFAULT_LOG_FILE = 'log.txt'
log_file = DEFAULT_LOG_FILE
create_dir_cmd = "test -d {0} || mkdir -p {0}"

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

def re_run_stm32(cmd):
	global ERR
	log("Project first failed, rebuild to check if the error persists")
	log("make reset")
	cmd_reset = cmd.replace("all", "reset")
	err = os.system(cmd_reset + ' >> %s 2>&1' % log_file)
	if err != 0:
		ERR = 1
		log_err("ERROR")
		return err

	log("make all")
	err = os.system(cmd + ' >> %s 2>&1' % log_file)
	if err != 0:
		log("Error persits, not a random fail, please check!")
	else:
		log("First fail was possibly a random one")
	return err

def run_cmd(cmd):
	global ERR
	tmp = cmd.split(' ')
	if tmp[0] == 'make':
		log('make ' + tmp[-1])
	else:
		if tmp[2] == 'make':
			log('make ' + tmp[-1])
		else:
			log(cmd)
	sys.stdout.flush()
	err = os.system('echo %s >> %s' % (cmd, log_file))
	if err != 0:
		ERR = 1
		return err
	err = os.system(cmd + ' >> %s 2>&1' % log_file)
	if err != 0:
		if 'timeout' in cmd:
			err = re_run_stm32(cmd)
			if err == 0:
				return err

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
	err = os.system('wget --http-user={} --http-password={} --auth-no-challenge \'{}\' -O blacklist.txt >> {} 2>&1'
				.format(USER, TOKEN, blacklist_url, log_file))
	if err != 0 or (not os.path.isfile('blacklist.txt')):
		log_err('Can not download blacklist file')
		return blacklist
	file = open('blacklist.txt', 'r')
	for line in file.readlines():
		project = line.split('#')[0].rstrip().replace('.', '_')
		if project != '':
			blacklist.append(project)
	return blacklist

def configfile_and_download_all_hw(_platform, noos, _builds_dir, hdl_branch):
	try:
		with open(os.path.expanduser('~') + '/configure_hdl_new.txt') as configure_file:
			lines = configure_file.readlines()
			server_base_path = lines[0].rstrip()
			environment_path_files = lines[1].rstrip()
	except OSError:
		print("Configuration file needed")

	pattern = '\d{4}_\d{2}_\d{2}-\d{2}_\d{2}_\d{2}'
	blacklist = []
	timestamp_match = re.search(pattern, hdl_branch)
	if timestamp_match:
		hdl_branch = re.split('\/', hdl_branch)[0]
		timestamp_folder = timestamp_match.group()

	if hdl_branch == "main":
		hdl_branch_path = hdl_branch + '/hdl_output'
	else:
		if requests.get(server_base_path + 'releases/' + hdl_branch, stream=True).status_code == 200:
			hdl_branch_path = 'releases/' + hdl_branch + '/hdl_output'
		elif requests.get(server_base_path + 'dev/' + hdl_branch, stream=True).status_code == 200:
			hdl_branch_path = 'dev/' + hdl_branch + '/hdl_output'
		else:
			print("Error related to hdl branch name: " + hdl_branch)
			exit()

	if timestamp_match:
		if requests.get(server_base_path + hdl_branch_path + '/' + timestamp_folder, stream=True).status_code == 200:
			hdl_branch_path += '/' + timestamp_folder
		else:
			print("Error related to timestamp folder: " + timestamp_folder + " not existing in hdl_branch: " + hdl_branch)
			exit()

	builds_dir = _builds_dir + '_' + hdl_branch
	run_cmd(create_dir_cmd.format(builds_dir))
	if SKIP_DOWNLOAD == 1:
		return (environment_path_files, builds_dir)
	hardwares = os.path.join(builds_dir, HW_DIR_NAME)
	run_cmd(create_dir_cmd.format(hardwares))
	server_full_path = server_base_path + hdl_branch_path
	if (_platform is None or _platform == 'xilinx'):
		blacklist = process_blacklist()
		new_hardwares = os.path.join(builds_dir, NEW_HW_DIR_NAME)
		run_cmd(create_dir_cmd.format(new_hardwares))
		err = os.system("python {}/tools/scripts/download_files.py {} {} {} \"{}\""
				  .format(noos, noos, builds_dir, server_full_path, blacklist))
		if err != 0:
			return
	return (environment_path_files, builds_dir, blacklist)

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

class BuildConfig:
	def __init__(self, project_dir, platform, flags, build_name, hardware,
	             _builds_dir, log_dir):
		self.project_dir = project_dir
		self.builds_dir = _builds_dir
		self.log_dir = log_dir
		self.project = os.path.basename(project_dir)
		self.platform = platform
		self.flags = flags
		self.build_name = build_name
		self.hardware = hardware
		short_build_dir = 'build_%s' % platform
		self._binary = "%s_%s_%s.elf" % (self.project, platform, build_name)
		self.boot_dir = "%s_%s_%s" % (self.project, platform, build_name)
		if hardware != '':
			short_build_dir = short_build_dir + '_' + hardware
			self._binary = "%s_%s_%s_%s.elf" % (
				self.project, platform, build_name, hardware)
			self.boot_dir = "%s_%s_%s_%s" % (
				self.project, platform, build_name, hardware)
		self.build_dir = os.path.join(self.builds_dir, short_build_dir)
		self.binary = os.path.join(self.build_dir, self._binary)
		self.export_file = os.path.join(self.build_dir, self.binary)
		if (platform == 'aducm3029' or platform == 'stm32' or platform == 'maxim'):
			self.export_elf_file = self.export_file
			self.export_file = self.export_file.replace('.elf', '.hex')
		if (platform == 'pico'):
			self.export_elf_file = self.export_file
			self.export_file = self.export_file.replace('.elf', '.uf2')
		if (platform == 'mbed'):
			self.export_elf_file = self.export_file
			self.export_file = self.export_file.replace('.elf', '.bin')
		if (platform == 'xilinx'):
			self.export_boot_bin = os.path.join(self.build_dir, "output_boot_bin/BOOT.BIN")
			self.export_archive = os.path.join(self.build_dir, "bootgen_sysfiles.tar.gz")

	def build(self):
		global log_file
	
		log_file = self._binary.replace('.elf', '.txt')
		log_file = os.path.join(self.log_dir, log_file)

		log_str = "Building %10s (%8s) -- %s " % (
			to_blue(self.project), to_blue(self.build_name), to_blue(self.platform))
		if self.hardware != '':
			log_str = log_str + "-- %20s" % to_blue(self.hardware)	
		log(log_str)

		cmd = "make -C " + self.project_dir + \
			" PLATFORM=" + self.platform + \
			" BUILD_DIR=" + self.build_dir + \
			" BINARY=" + self.binary + \
			" LOCAL_BUILD=n" + \
			" LINK_SRCS=y " + self.flags
			
		if self.hardware != '':
			(hardware_file, new_hdf, err) = get_hardware(self.hardware,
						self.platform, self.builds_dir)
			if err != 0:
				return err
			cmd = cmd + ' HARDWARE=%s' % hardware_file
		else:
			if os.path.isdir(self.build_dir):
				new_hdf = False
			else:
				new_hdf = True
		if self.platform == 'stm32':
			err = run_cmd(cmd + ' reset')
			if err != 0:
				return err
			err = run_cmd("timeout 200s " + cmd + ' VERBOSE=y -j%d all' % (multiprocessing.cpu_count() / 2))
			if err != 0:
				if err == 124:
					log("Build not finished, stopped by timeout")
				return err
		else:
			if new_hdf:
				err = run_cmd(cmd + ' reset')
				if err != 0:
					return err

			err = run_cmd(cmd + ' update')
			if err != 0:
				return err
			err = run_cmd(cmd + ' clean')
			if err != 0:
				return err
			err = run_cmd(cmd + ' -j%d all' % (multiprocessing.cpu_count() / 2))
			if err != 0:
				return err
		
		log_success("DONE")
		log_file = DEFAULT_LOG_FILE

		return 0
def main():
	(noos, export_dir, log_dir, _project,
	 _platform, _build_name, _builds_dir, _hw, hdl_branch) = parse_input()
	projets = os.path.join(noos,'projects')
	run_cmd(create_dir_cmd.format(export_dir))
	run_cmd(create_dir_cmd.format(log_dir))
	(environment_path_files, builds_dir, blacklist) = configfile_and_download_all_hw(_platform, noos, _builds_dir, hdl_branch)
	for project in os.listdir(projets):
		binary_created = False
		if _project is not None:
			if _project != project:
				continue
		project_dir = os.path.join(projets, project)
		build_file = os.path.join(project_dir, 'builds.json')
		if not os.path.isfile(build_file):
			continue

		fp = open(build_file)
		configs = json.loads(fp.read())
		ok = 1
		for (platform, config) in configs.items():
			if _platform is not None:
				if _platform != platform:
					continue
			for (build_name, params) in config.items():
				if _build_name is not None:
					if _build_name != build_name:
						continue
				project_export = os.path.join(export_dir, project)
				run_cmd(create_dir_cmd.format(project_export))
				flags = params['flags']
				if 'hardware' in params:
					hardwares = params['hardware']
				else:
					hardwares = [""]
				
				for hardware in hardwares:
					if _hw is not None:
						if _hw != hardware:
							continue
					if hardware in blacklist:
						continue
					env = dict(os.environ)
					shell_source(environment_path_files + platform + "_environment.sh")

					new_build = BuildConfig(project_dir,
								platform,
								flags,
								build_name,
								hardware,
								builds_dir,
								log_dir)
					err = new_build.build()
					os.environ.clear()
					os.environ.update(env)
					if err != 0:
						ok = 0
						if err == 2:
							#Keyboard interrupt
							exit()
						continue
					else:
						if platform == 'xilinx':
							project_export_dir = os.path.join(project_export, new_build.boot_dir)
							run_cmd(create_dir_cmd.format(project_export_dir))
							run_cmd("cp %s %s" %
								(new_build.export_archive, project_export_dir))
							file = open(os.path.join(new_build.build_dir,"tmp/arch.txt"))
							if 'sys_mb' not in file.read(): #for sys_mb no BOOT.BIN is created
								run_cmd("cp %s %s" %
									(new_build.export_boot_bin, project_export_dir))
							binary_created = True
						else:
							run_cmd("cp %s %s" %
								(new_build.export_file, project_export))
							run_cmd("cp %s %s" %
								(new_build.export_elf_file, project_export))
							binary_created = True
			
		fp.close()

		if ok == 1:
			status = 'OK'
		else:
			status = 'Fail'
		all_status = os.path.join(log_dir, 'all_builds.txt')
		os.system('echo Project %20s -- %s >> %s' % (project, status, all_status))
		if binary_created:
			cwd = os.getcwd()
			os.chdir(project_export)
			os.system("zip -mr -FS %s.zip . >> %s 2>&1" % ("../"+project, "../../"+log_file))
			os.chdir(cwd)
			run_cmd("rm -r %s" % project_export)
main()

if ERR != 0:
	exit("Error occured: %d" % ERR)
