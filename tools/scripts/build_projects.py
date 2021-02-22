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

description_help='''Build all noos projects
Examples:\n
    >python build_projects.py ..\.. aducm3029
	>python build_projects.py /home/user/noos xilinx
'''

def parse_input():
    parser = argparse.ArgumentParser(description=description_help,\
			    formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('noos_location', help="Path to noos location")
    parser.add_argument('export_dir', help="Path where to save files")
    args = parser.parse_args()

    return (args.noos_location, args.export_dir)

ERR = 0

def run_cmd(cmd):
	log_file = 'log.txt'
	print(cmd)
	sys.stdout.flush()
	err = os.system(cmd + ' > %s 2>&1' % log_file)
	if (err != 0):
		print(TGREEN + "Error" + TWHITE)
		print("See log:")
		os.system("cat %s" % log_file)
		ERR = 1

def to_blue(str):
	return TBLUE + str + TWHITE

def main():
	create_dir_cmd = "test -d {0} || mkdir -p {0}"
	(noos, export_dir) = parse_input()
	projets = os.path.join(noos,'projects')
	run_cmd(create_dir_cmd.format(export_dir))

	for project in os.listdir(projets):
		project_export = os.path.join(export_dir, project)
		projet_dir = os.path.join(projets, project)
		build_file = os.path.join(projet_dir, 'builds.json')
		if os.path.isfile(build_file):
			run_cmd(create_dir_cmd.format(project_export))
			fp = open(build_file)
			configs = json.loads(fp.read())
			for (platform, config) in configs.items():
				for (build_name, flags) in config.items():
					print("Runing build %s on project %s for platform %s" %
						(to_blue(build_name),
						to_blue(project),
						to_blue(platform)))
					build_dir_name = 'build_%s_%s' % (platform, build_name)
					binary = os.path.join(build_dir_name, "%s_%s.elf" % (project, build_name))
					export_file = os.path.join(projet_dir, binary)
					if (platform == 'aducm3029'):
						export_file = export_file.replace('.elf', '.hex')
					cmd = 'make -C %s %s BUILD_DIR_NAME=%s \
LOCAL_BUILD=n LINK_SRCS=n BINARY=%s VERBOSE=y -j%d ' % (projet_dir, flags, build_dir_name, binary, multiprocessing.cpu_count() - 1)
					run_cmd(cmd + 'ra')
					run_cmd("cp %s %s" % (export_file, project_export))
					print(TGREEN + "DONE" + TWHITE)
			fp.close()
			run_cmd("zip -jm %s.zip %s" % (project_export, os.path.join(project_export, "*")))
			run_cmd("rm -rf %s" % project_export)
		
main()
exit(ERR)
