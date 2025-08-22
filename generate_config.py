import kconfiglib
import sys
import os
import argparse
from pathlib import Path

print("Running generate_config.py")

parser = argparse.ArgumentParser(description="Generate config.cmake from defconfig files")

parser.add_argument("--root_dir", type=str, help="Path to the CAPI directory")
parser.add_argument("--build_dir", type=str, help="Path to the build directory")
parser.add_argument("--defconfig", type=str, help="Path to a defconfig file", nargs='+')
parser.add_argument("--update", action='store_true', help="Append the defconfig file to the existing .config file", default=False)
parser.add_argument("--native", action='store_true', help="Is No-OS the top directory?", default=False)
parser.add_argument("--verbose", type=bool, help="Enable verbose output", default=False)

args = parser.parse_args(sys.argv[1:])
print(args.update)

src_dir = Path(args.root_dir)
build_dir = Path(args.build_dir)

kconf = kconfiglib.Kconfig(src_dir.joinpath("Kconfig"))

main_config = build_dir.joinpath(".config")
if not os.path.exists(main_config):
        Path(main_config).touch()

kconf.load_config(main_config)

if args.update:
        if args.defconfig != None:
                for defconfig in args.defconfig:
                        kconf.load_config(src_dir.joinpath(defconfig), replace=False)
        kconf.write_config(build_dir.joinpath(".config"))

try:
        print("Opening config.cmake")
        with open(build_dir.joinpath("config.cmake"), "w") as cmake_file:
                cmake_file.write("#Generated based on .config\n")
                if len(kconf.unique_defined_syms) == 0:
                        print("Empty .config?")

                for sym in kconf.unique_defined_syms:
                        if sym.orig_type is kconfiglib.STRING:
                                value = sym.str_value
                        elif sym.orig_type is kconfiglib.INT:
                                value = sym.str_value
                        elif sym.tri_value == 2:
                                value = "ON"
                        elif sym.tri_value == 0:
                                value = "OFF"

                        cmake_file.write(f"set(CONFIG_{sym.name} {value})\n")
                        if args.native == False:
                                cmake_file.write(f"set(CONFIG_{sym.name} {value} PARENT_SCOPE)\n")

                print("Generated config.cmake")

except Exception as e:
        print(e)


if True:
        print("Config files used:")
        if args.defconfig != None:
                for defconfig in args.defconfig:
                        print("{}".format(defconfig))