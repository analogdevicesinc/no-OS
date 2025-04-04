import kconfiglib
import sys
import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description="Generate config.cmake from defconfig files")

parser.add_argument("--root_dir", type=str, help="Path to the CAPI directory", default=Path(__file__).parent.resolve())
parser.add_argument("--defconfig", type=str, help="Path to a defconfig file", nargs='+')
parser.add_argument("--update", type=bool, help="Append the defconfig file to the existing .config file", default=False)
parser.add_argument("--verbose", type=bool, help="Enable verbose output", default=False)

args = parser.parse_args(sys.argv[1:])
print(args.update)

capi_dir = args.root_dir
kconf = kconfiglib.Kconfig(capi_dir.joinpath("Kconfig"))

if args.update:
        kconf.load_config(capi_dir.joinpath(".config"))

for defconfig in args.defconfig:
        kconf.load_config(capi_dir.joinpath(defconfig), replace=False)

kconf.write_config(capi_dir.joinpath(".config"))

with open(capi_dir.joinpath("config.cmake"), "w") as cmake_file:
        cmake_file.write("#Generated based on .config\n")

        for sym in kconf.unique_defined_syms: 
                if sym.str_value:
                        value = sym.str_value
                elif sym.tri_value == 2:
                        value = "ON"
                elif sym.tri_value == 0:
                        value = "OFF"

                cmake_file.write(f"set(CONFIG_{sym.name} {value})\n")

print("Generated config.cmake")

if True:
        print("Config files used:")
        for defconfig in args.defconfig:
                print("{}".format(defconfig))