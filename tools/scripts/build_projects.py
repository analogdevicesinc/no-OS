#!/bin/python

import argparse
import json
import os


description_help='''Build all noos projects
Examples:\n
    >python build_projects.py ..\.. aducm3029
	>python build_projects.py /home/user/noos xilinx
'''

def parse_input():
    parser = argparse.ArgumentParser(description=description_help,\
			    formatter_class=argparse.RawTextHelpFormatter)
    parser.add_argument('noos_location', help="Path to noos location")
    parser.add_argument('platform', help="Platform to build for",
				choices=['aducm3029', 'xilinx', 'stm32', 'altera'])
    args = parser.parse_args()

    return (args.noos_location, args.platform)

def main():
	(noos, platform) = parse_input()
	projets = os.path.join(noos,'projects')
	for project in os.listdir(projets):
		projet_dir = os.path.join(projets, project)
		build_file = os.path.join(projet_dir, 'builds.json')
		if os.path.isfile(build_file):
			fp = open(build_file)
			configs = json.loads(fp.read())
			for (cur_platform, config) in configs.items():
				if (cur_platform == platform):
					for (build_name, flags) in config.items():
						print("Runing build %s on project %s for platform %s" %
							(build_name, project, cur_platform))
						workspace = 'build_%s_%s' % (cur_platform, build_name)
						cmd = 'make -C %s %s WORKSPACE=%s BUILD_DIR=%s\
 LOCAL_BUILD=n LINK_SRCS=n VERBOSE=y ra -j' % (projet_dir, flags, workspace, workspace)
						#print(cmd)
						os.system(cmd)
			fp.close()

main()