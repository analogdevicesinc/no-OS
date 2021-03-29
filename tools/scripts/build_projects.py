#!/bin/python

import argparse
import json
import os
import multiprocessing
import sys
import filecmp

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
'''

def parse_input():
	parser = argparse.ArgumentParser(description=description_help,\
				formatter_class=argparse.RawTextHelpFormatter)
	parser.add_argument('noos_location', help="Path to noos location")
	parser.add_argument('-export_dir', default='exports', help="Path where to save files")
	parser.add_argument('-log_dir', default='logs', help="Path where to save log files")
	parser.add_argument('-project', help="Name of project to be built")
	parser.add_argument('-platform', help="Name of platform to be built")
	parser.add_argument('-build_name', help="Name of built type to be built")
	parser.add_argument('-builds_dir', help="Directory where to build projects")
	args = parser.parse_args()

	return (args.noos_location, args.export_dir, args.log_dir, args.project,
		args.platform, args.build_name, args.builds_dir)

ERR = 0
LOG_START = " -> "

def log(msg):
	print(TGREEN + LOG_START + TWHITE + msg)

def log_err(msg):
	print(TRED + LOG_START + msg + TWHITE)

def log_success(msg):
	print(TGREEN + LOG_START + msg + TWHITE)

DEFAULT_LOG_FILE = 'log.txt'
log_file = DEFAULT_LOG_FILE

def run_cmd(cmd):
	tmp = cmd.split(' ')
	if tmp[0] == 'make':
		log('make ' + tmp[-1])
	else:
		log(cmd)
	sys.stdout.flush()
	err = os.system('echo %s >> %s 2>&1' % (cmd, log_file))
	if err == 0:
		err = os.system(cmd + ' >> %s 2>&1' % log_file)
	if err != 0:
		global ERR
		log_err("ERROR")
		log("See log %s " \
		    "-- Use cat (linux) or type (windows) to see colored output"
		    % log_file)
		ERR = 1

	return err
def to_blue(str):
	return TBLUE + str + TWHITE

HDF_SERVER = os.environ['HDF_SERVER']

def get_hardware(hardware, platform, builds_dir):
	if platform == 'xilinx':
		ext = 'hdf'
		base_name = 'system_top'
	else:
		ext = 'sopcinfo'
		base_name = 'system_bd'

	new_hdf = 0
	sever_file = "%s/%s.%s" % (hardware, base_name, ext)
	local_file = "%s.%s" % (hardware, ext)

	filename = os.path.join(builds_dir, local_file)
	tmp_filename = filename + '_tmp'
	cmd = 'wget %s/%s -O %s' % (HDF_SERVER, sever_file, tmp_filename)
	log("Get %s" % sever_file)
	err = os.system(cmd + ' > /dev/null 2>&1')
	if (err != 0):
		global ERR
		log_err("ERROR")
		ERR = 1
		return ('', 0, err)

	if os.path.isfile(filename):
		#If equal
		if filecmp.cmp(filename, tmp_filename):
			log("Same hardware from last build, use existing bsp")
			return (filename, 0, err)
	
	err = run_cmd('cp %s %s' % (tmp_filename, filename))
	if err != 0:
		return err

	log("Hardware changed from last build")

	return (filename, 1, err)

class BuildConfig:
	def __init__(self, project_dir, platform, flags, build_name, hardware,
	             _builds_dir, log_dir):
		self.project_dir = project_dir
		if _builds_dir is None:
			self.builds_dir = project_dir
		else:
			self.builds_dir = _builds_dir
		self.log_dir = log_dir
		self.project = os.path.basename(project_dir)
		self.platform = platform
		self.flags = flags
		self.build_name = build_name
		self.hardware = hardware
		short_build_dir = 'build_%s' % platform
		self._binary = "%s_%s_%s.elf" % (self.project, platform, build_name)
		if hardware != '':
			short_build_dir = short_build_dir + '_' + hardware
			self._binary = "%s_%s_%s_%s.elf" % (
				self.project, platform, build_name, hardware)
		self.build_dir = os.path.join(self.builds_dir, short_build_dir)
		self.binary = os.path.join(self.build_dir, self._binary)
		self.export_file = os.path.join(self.build_dir, self.binary)
		if (platform == 'aducm3029'):
			self.export_file = self.export_file.replace('.elf', '.hex')

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
			" LINK_SRCS=n" + \
			" VERBOSE=y " + self.flags
			
		if self.hardware != '':
			log("\thardware : %s" % to_blue(self.hardware))
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
		
		if new_hdf:
			err = run_cmd(cmd + ' clean_all')
			if err != 0:
				return err

		err = run_cmd(cmd + ' update_srcs')
		if err != 0:
			return err
		err = run_cmd(cmd + ' -j%d all' % (multiprocessing.cpu_count() - 1))
		if err != 0:
			return err
		
		log_success("DONE")
		log_file = DEFAULT_LOG_FILE

		return 0

def main():
	create_dir_cmd = "test -d {0} || mkdir -p {0}"
	(noos, export_dir, log_dir, _project, _platform, _build_name, _builds_dir) = parse_input()
	projets = os.path.join(noos,'projects')
	run_cmd(create_dir_cmd.format(export_dir))
	run_cmd(create_dir_cmd.format(log_dir))
	if _builds_dir is not None:
		run_cmd(create_dir_cmd.format(_builds_dir))

	for project in os.listdir(projets):
		binary_created = False
		if _project is not None:
			if _project != project:
				continue
		project_dir = os.path.join(projets, project)
		build_file = os.path.join(project_dir, 'builds.json')
		if not os.path.isfile(build_file):
			continue

		project_export = os.path.join(export_dir, project)
		run_cmd(create_dir_cmd.format(project_export))
		fp = open(build_file)
		configs = json.loads(fp.read())
		for (platform, config) in configs.items():
			if _platform is not None:
				if _platform != platform:
					continue
			for (build_name, params) in config.items():
				if _build_name is not None:
					if _build_name != build_name:
						continue
				flags = params['flags']
				if 'hardware' in params:
					hardwares = params['hardware']
				else:
					hardwares = [""]
				
				for hardware in hardwares:
					new_build = BuildConfig(project_dir,
								platform,
								flags,
								build_name,
								hardware,
								_builds_dir, 
								log_dir)
					err = new_build.build()
					if err != 0:
						if err == 2:
							#Keyboard interrupt
							exit()
						continue
					run_cmd("cp %s %s" % 
						(new_build.export_file, project_export))
					binary_created = True
			
		fp.close()
		if binary_created:
			run_cmd("zip -jm %s.zip %s" % (project_export, os.path.join(project_export, "*")))

main()

if ERR != 0:
	exit("Error occured: %d" % ERR)
