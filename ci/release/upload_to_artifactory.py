########################################################################
#
# File name: upload_to_artifactory.py
# Author: Raus Stefan, Andrisan Andreea
#
# This script is used to upload files to ADI internal artifactory server
#
# Copyright (C) 2019-2023 Analog Devices Inc.
#
#######################################################################

import os
import argparse
import glob
import shutil
import sys
from glob import glob

LOCAL_PATHS_LIST = []
# If you try to upload files in a different folder than the ones in below list, there will be printed a message and files won't be uploaded
SERVER_FOLDERS_LIST = [ "hdl", "linux", "linux_rpi", "arm_trusted_firmware", "boot_partition", "rootfs", "u-boot", "HighSpeedConverterToolbox", "TransceiverToolbox", "SD_card_image", "no-OS"]

########### Define arguments and help section #################

parser = argparse.ArgumentParser(
   description='This script is uploading files or folders to artifactory server.Parameter order doesn\'t matter.', \
   formatter_class=argparse.RawDescriptionHelpFormatter,
   epilog='Examples: '\
                + '\n-> "./upload_to_artifactory.py --server_path="hdl/main/2020_12_12/pluto" --local_path="../projects/pluto/log_file.txt"'\
                + ' --properties="git_sha=928ggraf;git_commmit_date=2020_12_12" --no_rel_path" will upload file "log_file.txt" to '\
                + ' <UPLOAD_BASE_PATH>/hdl/main/2020_12_12/pluto/log_file.txt and add properties git_sha=928ggraf and git_commit_date=2010_12_12 on it.'\
                + '\n-> "./upload_to_artifactory.py --server_path="linux" --local_path="main/2020_11_25/arm/zynq_zed_adv7511.dtb"" will upload dtb'\
                + ' file to <UPLOAD_BASE_PATH>/linux/main/2020_11_25/arm/zynq_zed_adv7511.dtb')
parser.add_argument("--base_path",   help="Artifactory Base Path - Internal ADI Artifactory server and development folder")
parser.add_argument("--server_path", help="Artifactory folder where the files/folders will be saved, for example 'hdl' or 'linux'.")
parser.add_argument("--local_path",  help="Local path to file/folder to upload. It can be relative or absolute.")
parser.add_argument("--properties",  help="Properties to be added to file/folder. If multiple ones, split them by ';'.")
parser.add_argument("--no_rel_path", help="If this exists, the relative path until local file will be appended to artifactory path", action="store_true")
parser.add_argument("--props_level", help="Set for how many levels of folders to set specified properties, by default just on file.")
parser.add_argument("--token",       help="Artifactory authentication token. Otherwise you can export API_TOKEN in terminal before calling this script.")
parser.add_argument("--log_file",    help="Local file where to save the logs from curl command, if no file is specified the logs will be printed in terminal")
args = parser.parse_args()
parser.parse_args()

########## Check if required and optional arguments are set  #################
if args.token:
   API_TOKEN = args.token
else:
   if "API_TOKEN" in os.environ:
      API_TOKEN = os.environ['API_TOKEN']
   else:
      print('\nError:Parameter "--token" is not set. This is Artifactory Authentication Token and can be set even using parameter "--token" on upload command, even by exporting API_TOKEN variable in terminal, before calling upload script.')
      exit(1)

if args.base_path:
   UPLOAD_BASE_PATH = args.base_path
else:
   if "UPLOAD_BASE_PATH" in os.environ:
      UPLOAD_BASE_PATH = os.environ['UPLOAD_BASE_PATH']
   else:
      print('\nError:Parameter "--base_path" is not set. This is ADI Internal Artifactory Server plus first level of folders. It can be set even using parameter "--base_path" on upload command, even by exporting UPLOAD_BASE_PATH variable in terminal, before calling upload script.')
      exit(1)

if args.server_path:
   # take first folder from server path and check if matches with folders from artifactory server (hdl, linux, SD_card_image etc)
   SERVER_PATH = args.server_path
   SERVER_PATH = SERVER_PATH[1:] if SERVER_PATH.startswith('/') else SERVER_PATH
   SERVER_FOLDER = SERVER_PATH.split("/", 1)[0]
   if SERVER_FOLDER not in SERVER_FOLDERS_LIST:
     print('\nError:Parameter "--server_path" must contain an already existing folder, for example "hdl", "linux", "SD_card_image" etc.' +
     'If you want to add new folders, please edit "upload_to_artifactory.py" or contact script owner.')
     exit(1)
else:
   print('\nError:Parameter "--server_path" is required. It should be set to server location where the files/folder will be uploaded. Check help section.')
   exit(1)

if args.local_path:
   LOCAL_PATH = os.path.abspath(args.local_path) if '../' in args.local_path else args.local_path
   # if there was given a dir as local_path parameter, get all the files inside it in a list
   if os.path.isdir(LOCAL_PATH):
      print('IS DIR')
      for dpath, dnames, fnames in os.walk(LOCAL_PATH):
         for i, FILE_NAME in enumerate([os.path.join(dpath, fname) for fname in fnames]):
            print(FILE_NAME)
            LOCAL_PATHS_LIST.append(str(FILE_NAME))
   elif os.path.isfile(LOCAL_PATH):
      LOCAL_PATHS_LIST = [LOCAL_PATH]
      print('IS FILE' + LOCAL_PATHS_LIST)
   else:
      print('\nError:It looks that parameter "--local_path" is wrong defined/does not exists. Plese check: ' + LOCAL_PATH)
      exit(1)
else:
   print('\nParameter "--local_path" is required. It should point to local file/folder to upload.')
   exit(1)

if args.properties:
   PROPS = args.properties
else:
   PROPS = ''

if args.no_rel_path:
   NO_REL_PATH = True
else:
   NO_REL_PATH = False

if args.props_level:
   PROP_LEVEL = int(args.props_level)
else:
   PROP_LEVEL = 0

if args.log_file:
   LOG_FILE = ">> " + args.log_file
else:
   LOG_FILE = ''

########## Upload files ##########
# If files with same name already exists at specified server path, they will be overwritten

for FILE in LOCAL_PATHS_LIST:
   if NO_REL_PATH:
        FILE_NAME = os.path.basename(FILE)
        ART_PATH = UPLOAD_BASE_PATH + "/" + SERVER_PATH + "/" + FILE_NAME
   else:
        ART_PATH = UPLOAD_BASE_PATH + "/" + SERVER_PATH + "/" + FILE
   upload_cmd = "curl -H \"X-JFrog-Art-Api:" + API_TOKEN + "\" -X PUT \"" + ART_PATH + ";" + PROPS + "\" -T \"" + FILE + "\" " + LOG_FILE
   os.system(upload_cmd)

########## Upload properties on folders #########

if NO_REL_PATH:
   ART_PATH = UPLOAD_BASE_PATH + "/" + SERVER_PATH
else:
   ART_PATH = UPLOAD_BASE_PATH + "/" + SERVER_PATH + "/" + os.path.split(LOCAL_PATHS_LIST[0])[0]

i = 0
while ( i < int(PROP_LEVEL)):
   set_folder_props_cmd = "curl -H \"X-JFrog-Art-Api:" + API_TOKEN + "\" -X PUT \"" + ART_PATH + "/;" + PROPS + "\" " + LOG_FILE
   os.system(set_folder_props_cmd)
   i = i + 1
   ART_PATH = os.path.split(ART_PATH)[0]

#################################################
