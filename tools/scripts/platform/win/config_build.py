#!/bin/python

import json
import os
import argparse

description_help='''Build noos projects
Examples:\n
    	>python config_build.py ..\..
'''

def parse_input():
	parser = argparse.ArgumentParser(description=description_help,\
				formatter_class=argparse.RawTextHelpFormatter)
	parser.add_argument('noos', help="Path to noos location")
	parser.add_argument('project', help="Project to build")
	parser.add_argument('binary', help="File to debug")
	args = parser.parse_args()

	return (args.noos, args.project, args.binary)

(noos, project, binary) = parse_input()

dir = os.environ['PROJECT_BUILD']
code_dir = os.path.join(dir, '.vscode')

fn = 'launch.json'
file = os.path.join(code_dir, fn)
with open(file, 'r') as fp:
	data = json.loads(fp.read())
data['configurations'][0]['program'] = binary
data['configurations'][0]['cwd'] = dir
with open(file, 'w') as fp:
	json.dump(data, fp, indent=4)

fn = 'tasks.json'
file = os.path.join(code_dir, fn)
with open(file, 'r') as fp:
	data = json.loads(fp.read())
data['tasks'][0]['command'] = 'make -C %s' % project
with open(file, 'w') as fp:
	json.dump(data, fp, indent=4)

flags = os.environ['FLAGS_WITHOUT_D']
incs = os.environ['EXTRA_INC_PATHS']

fn = 'c_cpp_properties.json'
file = os.path.join(code_dir, fn)
with open(file, 'r') as fp:
	data = json.loads(fp.read())

data['configurations'][0]['defines'] = flags.split(' ')
#remove lasta / from incs
incs = incs.split(' ')
for i in range(0, len(incs)):
	incs[i] = incs[i].rstrip('/')

data['configurations'][0]['includePath'] = incs
with open(file, 'w') as fp:
	json.dump(data, fp, indent=4)
