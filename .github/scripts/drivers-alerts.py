#!/usr/bin/env python3

# File name: drivers-alerts.py
# Author: Popovici Iuliu-Antoniu
# Description:
#   Python script to parse alerts (wanings, errors) from the build drivers output and generate a markdown table 
#   for containing the source files names and the messages
# Environment variables:
#   GITHUB_WORKSPACE: Absolute path of the working directory containing the drivers

import os
import subprocess
import re
from tabulate import tabulate
from html_to_markdown import convert
from actions_toolkit import core

def parse_env():
    global WORKSPACE

    WORKSPACE = os.environ["GITHUB_WORKSPACE"]

    core.info(f"Workspace: {WORKSPACE}")

def build_drivers():
    global RETURN_ERR
    RETURN_ERR = False

    try:
        # Run build drivers to output file
        with open("drivers_build.log", "w") as logfile:
            core.info("Building the drivers ...")
            subprocess.run(
                ["make", "-k", "-C", f"{WORKSPACE}/drivers", "-f", "Makefile"], 
                stdout=logfile, 
                stderr=subprocess.STDOUT,
                text=True
            )

        # Parse content from the output file and filter alerts
        with open("drivers_build.log", "r") as logfile:
            core.info("Filtering alerts from build output ...")
            alerts_lines = []
            alerts_storage = []
            alert_type = ":warning:warning"

            # Detects alerts from source (.c) files
            for line in logfile.readlines():
                print(line, end="")     # output build content to console
                
                # Check line is about compilation of a new source file or it reaches the end
                if re.search("^CC.*.c$", line) or "make: Leaving directory" in line:
                    curr_file = line.split(" ")[1].replace("\n", "")

                    # Store the previous alerts lines if the storage was in progress and clean it up
                    if len(alerts_lines) > 0:
                        alert_dict = {}
                        alert_dict["file"] = stored_file
                        alert_dict["type"] = alert_type
                        alerts_lines.insert(0, "<ul>")
                        alerts_lines.append("</ul>")
                        alert_dict["message"] = ''.join(alerts_lines)
                        alerts_storage.append(alert_dict)
                        alerts_lines = []
                        alert_type = ":warning:warning"

                    # In case a file contains alerts messages, it needs to be stored prematurely
                    stored_file = curr_file
                # Ignore first make line, make error final prompt and positions suggestions
                elif "make: Entering directory" not in line and "make: Target 'all'" not in line and "^" not in line:
                    # Start storing alerts messages
                    alerts_lines.append(f"<li>``{line}``</li>")

                    # Detect build errors on changed files
                    if "error:" in line:
                        RETURN_ERR = True
                        alert_type = ":x:error"
                         
    except Exception as e:
        core.set_failed(e)

    return alerts_storage

def export_md(alerts):
    headers = ["File", "Type", "Alert"]
    table = []
    
    core.info("Generate html report ...")
    try:
        if len(alerts) == 0:
            core.info("No alerts detected. Not generating the report.")
        else:
            for alert in alerts:
                new_table = [alert["file"], alert["type"], alert["message"]]
                table.append(new_table)
            html = tabulate(table, headers, tablefmt="html")

            core.info("Convert report to markdown ...")
            md = str(convert(html).content).replace("\_", "_")
            with open("drivers_alerts.md", "w") as f:
                f.write("## Drivers build report\n\n")
                f.write(md)
    except Exception as e:
        core.set_failed(e)

def main():
    core.start_group("Parse evironment variables")
    parse_env()
    core.end_group()

    core.start_group("Build drivers and store logs")
    alerts = build_drivers()
    core.end_group()

    core.start_group("Generate and export report to markdown")
    export_md(alerts)
    core.end_group()

    if RETURN_ERR:  core.set_failed("Error building drivers!")

if __name__ == "__main__":
    main()
