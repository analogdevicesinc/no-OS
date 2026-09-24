#!/usr/bin/env python3

import os
import subprocess
import sys
from tabulate import tabulate
from html_to_markdown import convert
from actions_toolkit import core

def pars_env():
    global FILES, WORKSPACE, SRCS

    FILES = os.environ["CHANGED_FILES"]
    WORKSPACE = os.environ["GITHUB_WORKSPACE"]

def build_drivers():
    # Export new SRCS to environment
    os.environ["SRCS_MAKE"] = FILES

    os.chdir(WORKSPACE)
    try:
        # Run build drivers to output file
        with open("drivers_build.log", "w") as logfile:
            proc = subprocess.run(
                ["make", "-C", f"{WORKSPACE}/drivers", "-f", "Makefile"], 
                stdout=logfile, 
                stderr=subprocess.STDOUT,
                text=True,
                check=True
            )
        # Parse content from the output file
        with open("drivers_build.log", "r") as logfile:
            # Filter warnings
            warning_start = False
            warning_lines = []
            warning_storage = []

            for line in logfile.readlines():
                # Detects warning about a .c file and ignore make commands info
                if "CC" not in line and not line.endswith(".c") and "make:" not in line:
                    # Start storing the warning message from multiple lines
                    if not warning_start: 
                        warning_start = True
                        warning_lines = []      # Reset warnings storage
                    warning_lines.append(line)
                else:
                    # Detects the end of the warning loop
                    if warning_start:
                        warning_start = False
                        warning_dict = {}
                        warning_dict["file"] = file
                        warning_dict["message"] = ''.join(warning_lines)
                        warning_storage.append(warning_dict)

                    file = line.split(" ")[1].replace("\n", "")

    except Exception as e:
        core.set_failed(e)

    return warning_storage

def export_md(warnings):
    headers = ["File", "Warning"]
    table = []
    for warning in warnings:
        new_table = [warning["file"], warning["message"]]
        table.append(new_table)

    html = tabulate(table, headers, tablefmt="html")
    md = convert(html).content

    with open("drivers_warnings.md", "w") as f:
        f.write(md)

def main():
    pars_env()
    warnings = build_drivers()
    export_md(warnings)

if __name__ == "__main__":
    main()
