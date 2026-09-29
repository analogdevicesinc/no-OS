#!/usr/bin/env python3

# File name: drivers-warnings.py
# Author: Popovici Iuliu-Antoniu
# Description:
#   Python script to parse warnings from the build drivers output and generate a markdown table for containing the source
#   files names and the messages
# Environment variables:
#   GITHUB_WORKSPACE: Absolute path of the working directory containing the drivers

import os
import subprocess
from tabulate import tabulate
from html_to_markdown import convert
from actions_toolkit import core

def parse_env():
    global WORKSPACE

    WORKSPACE = os.environ["GITHUB_WORKSPACE"]

    core.info(f"Workspace: {WORKSPACE}")

def build_drivers():
    try:
        # Run build drivers to output file
        with open("drivers_build.log", "w") as logfile:
            core.info("Building the drivers ...")
            subprocess.run(
                ["make", "-C", f"{WORKSPACE}/drivers", "-f", "Makefile"], 
                stdout=logfile, 
                stderr=subprocess.STDOUT,
                text=True
            )

        # Parse content from the output file and filter warnings
        with open("drivers_build.log", "r") as logfile:
            core.info("Filtering warnings from build output ...")
            warning_lines = []
            warning_storage = []

            # Detects warnings about a changed .c file
            for line in logfile.readlines():
                core.info(line)     # output build content to console
                if "CC" in line:
                    curr_file = line.split(" ")[1].replace("\n", "")

                    # Store the previous warning lines if the storage was in progress and clean it up
                    if len(warning_lines) > 0:
                        warning_dict = {}
                        warning_dict["file"] = stored_file
                        warning_dict["message"] = ''.join(warning_lines)
                        warning_storage.append(warning_dict)
                        warning_lines = []

                    # In case a file contains warning messages, it needs to be stored prematurely
                    stored_file = curr_file
                elif "make" not in line:
                    # Start storing warning messages
                    warning_lines.append(line)
                # Detect build errors on changed files
                elif "Error 1" in line: core.set_failed("Error building drivers!")
                         
    except Exception as e:
        core.set_failed(e)

    return warning_storage

def export_md(warnings):
    headers = ["File", "Warning"]
    table = []
    
    core.info("Generate html report ...")
    try:
        if len(warnings) == 0:
            core.info("No warnings detect. Not generating the report.")
        else:
            for warning in warnings:
                new_table = [warning["file"], warning["message"]]
                table.append(new_table)
            html = tabulate(table, headers, tablefmt="html")

            core.info("Convert report to markdown ...")
            md = convert(html).content
            with open("drivers_warnings.md", "w") as f:
                f.write(md)
    except Exception as e:
        core.set_failed(e)

def main():
    core.start_group("Parse evironment variables")
    parse_env()
    core.end_group()

    core.start_group("Build drivers and store logs")
    warnings = build_drivers()
    core.end_group()

    core.start_group("Generate and export report to markdown")
    export_md(warnings)
    core.end_group()

if __name__ == "__main__":
    main()
