#!/bin/python

import argparse
import json
import os
import multiprocessing
import sys

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
	args = parser.parse_args()

	return (args.noos_location, args.export_dir, args.log_dir, args.project,
		args.platform, args.build_name)

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

def get_hardware(hardware, platform, projet_dir):
	if platform == 'xilinx':
		ext = 'hdf'
		base_name = 'system_top'
	else:
		ext = 'sopcinfo'
		base_name = 'system_bd'

	sever_file = "%s/%s.%s" % (hardware, base_name, ext)
	local_file = "%s.%s" % (hardware, ext)

	if os.path.isfile(os.path.join(projet_dir, local_file)):
		return local_file

	cmd = 'wget %s/%s -O %s/%s' % (HDF_SERVER, sever_file, projet_dir, local_file)
	log("Get %s" % sever_file)
	err = os.system(cmd + ' > /dev/null 2>&1')
	if (err != 0):
		global ERR
		log_err("ERROR")
		ERR = 1

	return local_file

class BuildConfig:
	def __init__(self, project_dir, platform, flags, build_name, hardware, log_dir):
		self.project_dir = project_dir
		self.log_dir = log_dir
		self.project = os.path.basename(project_dir)
		self.platform = platform
		self.flags = flags
		self.build_name = build_name
		self.hardware = hardware
		self.build_dir_name = 'build_%s_%s' % (platform, build_name)
		self._binary = "%s_%s_%s.elf" % (self.project, platform, build_name)
		if hardware != '':
			self.build_dir_name += '_' + hardware
			self._binary = "%s_%s_%s_%s.elf" % (
				self.project, platform, build_name, hardware)
		self.binary = os.path.join(self.build_dir_name, self._binary)
		self.export_file = os.path.join(project_dir, self.binary)
		if (platform == 'aducm3029'):
			self.export_file = self.export_file.replace('.elf', '.hex')

	def build(self):
		global log_file
	
		log_file = self._binary.replace('.elf', '.txt')
		log_file = os.path.join(self.log_dir, log_file)

		log("Runing build:" )
		log("\tname : %s" % to_blue(self.build_name))
		log("\tproject : %s" % to_blue(self.project))
		log("\tplatform : %s" % to_blue(self.platform))
			
		cmd = "make -C " + self.project_dir + \
			" PLATFORM=" + self.platform + \
			" BUILD_DIR_NAME=" + self.build_dir_name + \
			" BINARY=" + self.binary + \
			" LOCAL_BUILD=n" + \
			" LINK_SRCS=n" + \
			" VERBOSE=y " + self.flags + \
			" -j%d" % (multiprocessing.cpu_count() - 1)
			
		if self.hardware != '':
			log("\thardware : %s" % to_blue(self.hardware))
			hardware_file = get_hardware(self.hardware,
						self.platform, self.project_dir)
			cmd = cmd + ' HARDWARE=%s' % hardware_file
		
		cmd = cmd + ' ra'
		err = run_cmd(cmd)
		if err != 0:
			return err
		
		log_success("DONE")
		log_file = DEFAULT_LOG_FILE

		return 0

def main():
	create_dir_cmd = "test -d {0} || mkdir -p {0}"
	(noos, export_dir, log_dir, _project, _platform, _build_name) = parse_input()
	projets = os.path.join(noos,'projects')
	run_cmd(create_dir_cmd.format(export_dir))
	run_cmd(create_dir_cmd.format(log_dir))

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
