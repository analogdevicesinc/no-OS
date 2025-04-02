import kconfiglib
import sys
from pathlib import Path

if len(sys.argv) <= 2:
        sys.exit("Error: Invalid number of parameters (expected 3, provided {})".format(len(sys.argv) - 1))

capi_dir = Path(sys.argv[1])
board_defconfig = sys.argv[2]
project_defconfig = sys.argv[3]

kconf = kconfiglib.Kconfig(capi_dir.joinpath("Kconfig"))

kconf.load_config(capi_dir.joinpath(board_defconfig))
kconf.load_config(capi_dir.joinpath(project_defconfig), replace=False)
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