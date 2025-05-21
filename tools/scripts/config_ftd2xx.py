#!/bin/python

import argparse
import urllib.request
import zipfile
import tarfile
import io
import os
import shutil

description_help='''Download ftd2xx libraries
Examples:\n
    	>python config_ftd2xx.py .
'''

def parse_input():
	parser = argparse.ArgumentParser(description=description_help,\
				formatter_class=argparse.RawTextHelpFormatter)
	parser.add_argument('ftd2xx_lib', help="Path to ftd2xx library location")
	args = parser.parse_args()

	return (args.ftd2xx_lib)

(ftd2xx_lib) = parse_input()

filename = os.path.join(ftd2xx_lib + 'LibMPSSE_1.0.7.zip')
urllib.request.urlretrieve('https://ftdichip.com/wp-content/uploads/2025/02/LibMPSSE_1.0.7.zip', filename)
with zipfile.ZipFile(filename, 'r') as zip_ref:
	zip_ref.extractall(ftd2xx_lib)

os.remove(filename)

from sys import platform
if platform == "linux" or platform == "linux2":
	file = os.path.join(ftd2xx_lib, './LibMPSSE_1.0.7/Windows/libmpsse-windows-1.0.7.zip')
	with zipfile.ZipFile(file, 'r') as zip_ref:
		zip_ref.extractall(ftd2xx_lib)
	shutil.rmtree(os.path.join(ftd2xx_lib, 'LibMPSSE_1.0.7'))
elif platform == "darwin":
	file = os.path.join(ftd2xx_lib, './LibMPSSE_1.0.7/Windows/libmpsse-windows-1.0.7.zip')
	with zipfile.ZipFile(file, 'r') as zip_ref:
		zip_ref.extractall(ftd2xx_lib)
	shutil.rmtree(os.path.join(ftd2xx_lib, 'LibMPSSE_1.0.7'))
	file = os.path.join(ftd2xx_lib, './release/source/memcpy.c')
	os.remove(file)
elif platform == "win32":
	file = os.path.join(ftd2xx_lib, './LibMPSSE_1.0.7/Windows/libmpsse-windows-1.0.7.zip')
	with zipfile.ZipFile(file, 'r') as zip_ref:
		zip_ref.extractall(ftd2xx_lib)
	shutil.rmtree(os.path.join(ftd2xx_lib, 'LibMPSSE_1.0.7'))
