import kconfiglib
import sys
import os
import argparse
import filecmp
import shutil
from pathlib import Path


def write_if_changed(tmp_path, final_path):
        """Replace final_path only if content differs from tmp_path.
        Preserves timestamps when content is unchanged, avoiding
        unnecessary CMake reconfigures."""
        if final_path.exists() and filecmp.cmp(tmp_path, final_path, shallow=False):
                tmp_path.unlink()
        else:
                shutil.move(str(tmp_path), str(final_path))

print("Running generate_config.py")

parser = argparse.ArgumentParser(description="Generate config.cmake from defconfig files")

parser.add_argument("--root_dir", type=str, help="Path to the CAPI directory")
parser.add_argument("--build_dir", type=str, help="Path to the build directory")
parser.add_argument("--defconfig", type=str, help="Path to a defconfig file", nargs='+')
parser.add_argument("--update", action='store_true', help="Append the defconfig file to the existing .config file", default=False)
parser.add_argument("--native", action='store_true', help="Is No-OS the top directory?", default=False)
parser.add_argument("--verbose", type=bool, help="Enable verbose output", default=False)

args = parser.parse_args(sys.argv[1:])

src_dir = Path(args.root_dir)
build_dir = Path(args.build_dir)

kconf = kconfiglib.Kconfig(src_dir.joinpath("Kconfig"))

main_config = build_dir.joinpath(".config")
if not os.path.exists(main_config):
        Path(main_config).touch()

kconf.load_config(main_config)

if args.update:
        if args.defconfig is not None:
                for defconfig in args.defconfig:
                        kconf.load_config(src_dir.joinpath(defconfig), replace=False)
        kconf.write_config(build_dir.joinpath(".config"))

try:
        cmake_tmp = build_dir.joinpath("config.cmake.tmp")
        cmake_final = build_dir.joinpath("config.cmake")
        with open(cmake_tmp, "w") as cmake_file:
                cmake_file.write("#Generated based on .config\n")
                if len(kconf.unique_defined_syms) == 0:
                        print("Empty .config?")

                for sym in kconf.unique_defined_syms:
                        if sym.orig_type is kconfiglib.STRING:
                                value = sym.str_value
                        elif sym.orig_type is kconfiglib.INT:
                                value = sym.str_value
                        elif sym.tri_value == 2:  # y
                                value = "ON"
                        else:  # n (0) or m (1)
                                value = "OFF"

                        cmake_file.write(f"set(CONFIG_{sym.name} {value})\n")
                        if args.native == False:
                                cmake_file.write(f"set(CONFIG_{sym.name} {value} PARENT_SCOPE)\n")

        write_if_changed(cmake_tmp, cmake_final)
        print("Generated config.cmake")

except Exception as e:
        print(f"Error generating config.cmake: {e}", file=sys.stderr)
        sys.exit(1)

# Generate C header file.
# All symbols get a CONFIG_FOO define.
# Symbols defined under drivers/ only get CONFIG_FOO (to avoid collisions
# with driver identifiers such as enum values that share the symbol name).
# All other symbols (project, library, network, platform) also get an
# unprefixed FOO alias for backward compatibility with the Make build system.
try:
        header_tmp = build_dir.joinpath("no_os_config.h.tmp")
        header_final = build_dir.joinpath("no_os_config.h")
        drivers_path = "drivers/"
        with open(header_tmp, "w") as hf:
                hf.write("/* Auto-generated configuration header — do not edit */\n")
                hf.write("#ifndef NO_OS_CONFIG_H\n")
                hf.write("#define NO_OS_CONFIG_H\n\n")
                for sym in kconf.unique_defined_syms:
                        if sym.orig_type is kconfiglib.STRING:
                                val = f'"{sym.str_value}"'
                        elif sym.orig_type is kconfiglib.INT:
                                val = sym.str_value
                        elif sym.tri_value == 2:  # y
                                val = "1"
                        else:
                                continue  # n/m: omit

                        hf.write(f"#define CONFIG_{sym.name} {val}\n")

                        # Emit unprefixed alias only for non-driver symbols
                        in_drivers = any(
                                str(node.filename).startswith(drivers_path)
                                for node in sym.nodes
                        )
                        if not in_drivers:
                                hf.write(f"#define {sym.name} {val}\n")

                hf.write("\n#endif /* NO_OS_CONFIG_H */\n")
        write_if_changed(header_tmp, header_final)
        print("Generated no_os_config.h")

except Exception as e:
        print(f"Error generating no_os_config.h: {e}", file=sys.stderr)
        sys.exit(1)


if args.verbose:
        print("Config files used:")
        if args.defconfig is not None:
                for defconfig in args.defconfig:
                        print(f"  {defconfig}")