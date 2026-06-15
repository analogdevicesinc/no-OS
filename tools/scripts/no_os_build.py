#!/usr/bin/env python3
"""
no-OS CMake Build Utility

Discovers valid project/variant/board combinations from CMakePresets and
project directory structure, and orchestrates cmake configure + build steps.

Usage:
    python tools/scripts/no_os_build.py list [--project X] [--board X] [--variant X]
    python tools/scripts/no_os_build.py build --project X [--variant X] [--board X] [options]
"""

import argparse
import itertools
import json
import os
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path


USE_TTY = sys.stdout.isatty()


def combo_build_dir(build_dir_base, combo):
    """Return the build directory path for a given combination."""
    name = f"{combo['project']}-{combo['variant']}-{combo['board']}"
    return build_dir_base / name


class Spinner:
    """Braille spinner shown only when stdout is a tty."""

    FRAMES = "⠋⠙⠹⠸⠼⠴⠦⠧⠇⠏"

    def __init__(self, label):
        self._label = label
        self._stop = threading.Event()
        self._thread = threading.Thread(target=self._spin, daemon=True)

    def __enter__(self):
        if USE_TTY:
            self._thread.start()
        else:
            print(f"{self._label}...", flush=True)
        return self

    def __exit__(self, *_):
        self._stop.set()
        if USE_TTY and self._thread.is_alive():
            self._thread.join()

    def _spin(self):
        for frame in itertools.cycle(self.FRAMES):
            if self._stop.is_set():
                break
            print(f"\r{frame} {self._label}...", end="", flush=True)
            time.sleep(0.08)

    def finish(self, result):
        self._stop.set()
        if USE_TTY and self._thread.is_alive():
            self._thread.join()
        if USE_TTY:
            print(f"\r  {self._label}... {result}")
        else:
            print(result)


def find_repo_root():
    """Walk up from script location to find the repo root (contains CMakePresets.json)."""
    path = Path(__file__).resolve().parent
    while path != path.parent:
        if (path / "CMakePresets.json").exists() and (path / "projects").is_dir():
            return path
        path = path.parent
    sys.exit("Error: Could not find no-OS repo root (no CMakePresets.json found)")


def load_presets(repo_root):
    """Parse root CMakePresets.json and all included files to extract board presets.

    Returns dict: preset_name -> {name, board, platform, description, ...}
    """
    presets = {}

    def parse_file(filepath):
        with open(filepath) as f:
            data = json.load(f)

        # Process includes relative to the file's directory
        for inc in data.get("include", []):
            inc_path = repo_root / inc
            if inc_path.exists():
                parse_file(inc_path)

        for preset in data.get("configurePresets", []):
            if preset.get("hidden", False):
                continue
            cache = preset.get("cacheVariables", {})
            board = cache.get("BOARD", "")
            platform = cache.get("PLATFORM", "")
            if board and platform:
                presets[preset["name"]] = {
                    "name": preset["name"],
                    "board": board,
                    "platform": platform,
                    "description": preset.get("description", ""),
                }

    parse_file(repo_root / "CMakePresets.json")
    return presets


def discover_projects(repo_root):
    """Scan projects/ for directories containing CMakeLists.txt.

    Returns list of project names, excluding template and no-os-sample-project.
    """
    projects_dir = repo_root / "projects"
    skip = {"template", "no-os-sample-project"}
    projects = []
    for entry in sorted(projects_dir.iterdir()):
        if entry.is_dir() and entry.name not in skip:
            if (entry / "CMakeLists.txt").exists():
                projects.append(entry.name)
    return projects


def discover_variants(repo_root, project):
    """Find variant .conf files in a project directory.

    Returns list of variant names (filename without .conf extension).
    """
    project_dir = repo_root / "projects" / project
    variants = []
    for conf in sorted(project_dir.glob("*.conf")):
        variants.append(conf.stem)
    return variants


def discover_boards_for_variant(repo_root, project, variant):
    """Find valid boards for a project/variant combination.

    Priority:
    1. Per-variant dir: projects/<project>/boards/<variant>/*.conf
    2. Flat (legacy): projects/<project>/boards/*.conf
    3. No .conf files found: returns empty list, meaning the
       configuration is not available for this example.

    Returns (list of board names, source_type) where source_type is
    'variant', 'flat', or 'none'.
    """
    boards_dir = repo_root / "projects" / project / "boards"

    # Try variant-specific directory first
    variant_dir = boards_dir / variant
    if variant_dir.is_dir():
        boards = sorted(f.stem for f in variant_dir.glob("*.conf"))
        if boards:
            return boards, "variant"

    # Fall back to flat boards/
    if boards_dir.is_dir():
        # Check that there are actual .conf files (not just subdirectories)
        flat_boards = sorted(
            f.stem for f in boards_dir.glob("*.conf") if f.is_file()
        )
        if flat_boards:
            return flat_boards, "flat"

    return [], "none"


def discover_all_combinations(repo_root, presets):
    """Build the full list of valid (project, variant, board, platform) tuples."""
    # Map board name -> preset info
    board_to_preset = {}
    for preset in presets.values():
        board_to_preset[preset["board"]] = preset

    combinations = []
    for project in discover_projects(repo_root):
        variants = discover_variants(repo_root, project)
        if not variants:
            continue

        for variant in variants:
            boards, source = discover_boards_for_variant(
                repo_root, project, variant
            )
            # No .conf files in the boards sub-project means this
            # configuration is not available for the example - skip it.
            if not boards:
                continue

            for board in boards:
                if board in board_to_preset:
                    p = board_to_preset[board]
                    combinations.append(
                        {
                            "project": project,
                            "variant": variant,
                            "board": board,
                            "platform": p["platform"],
                            "preset": p["name"],
                        }
                    )

    return combinations


def filter_combinations(combinations, project=None, variant=None, board=None):
    """Filter combinations by any subset of criteria."""
    result = combinations
    if project:
        result = [c for c in result if c["project"] == project]
    if variant:
        result = [c for c in result if c["variant"] == variant]
    if board:
        result = [c for c in result if c["board"] == board]
    return result


def print_table(combinations):
    """Print combinations as a formatted table."""
    if not combinations:
        print("No matching combinations found.")
        return

    headers = ["PROJECT", "VARIANT", "BOARD", "PLATFORM"]
    # Compute column widths
    widths = [len(h) for h in headers]
    for c in combinations:
        widths[0] = max(widths[0], len(c["project"]))
        widths[1] = max(widths[1], len(c["variant"]))
        widths[2] = max(widths[2], len(c["board"]))
        widths[3] = max(widths[3], len(c["platform"]))

    fmt = "  ".join(f"{{:<{w}}}" for w in widths)
    separator = "  ".join("\u2500" * w for w in widths)

    print(fmt.format(*headers))
    print(separator)
    for c in combinations:
        print(fmt.format(c["project"], c["variant"], c["board"], c["platform"]))

    print(f"\n{len(combinations)} combination(s)")


def append_log(log_path, section, result):
    """Append a labelled section of captured output to the build log."""
    with open(log_path, "a") as f:
        f.write(f"=== {section} ===\n")
        if result.stdout:
            f.write(result.stdout)
        if result.stderr:
            f.write(result.stderr)
        f.write("\n")


def run_build(repo_root, combo, build_dir_base, jobs, clean, dry_run, probe=None, flash=False):
    """Run cmake configure + build (and optionally flash) for a single combination.

    Returns (combo, success, error_message).
    """
    project = combo["project"]
    variant = combo["variant"]
    board = combo["board"]
    preset = combo["preset"]

    build_dir = combo_build_dir(build_dir_base, combo)
    log_path = build_dir / "build.log"

    if clean and build_dir.exists() and not dry_run:
        import shutil
        shutil.rmtree(build_dir)

    defconfig = f"{project}/{variant}.conf"

    configure_cmd = [
        "cmake",
        "-B", str(build_dir),
        "--preset", preset,
        f"-DPROJECT_DEFCONFIG={defconfig}",
    ]
    if probe:
        configure_cmd.append(f"-DPROBE={probe}")

    build_cmd = [
        "cmake",
        "--build", str(build_dir),
        "--target", project,
    ]
    if jobs:
        build_cmd.extend(["-j", str(jobs)])

    flash_cmd = [
        "cmake",
        "--build", str(build_dir),
        "--target", "flash",
    ]

    if dry_run:
        print(f"  {' '.join(configure_cmd)}")
        print(f"  {' '.join(build_cmd)}")
        if flash:
            print(f"  {' '.join(flash_cmd)}")
        return combo, True, ""

    label = f"{project}/{variant} for {board}"

    with Spinner(f"Configuring {label}") as spinner:
        try:
            result = subprocess.run(
                configure_cmd,
                cwd=str(repo_root),
                check=True,
                capture_output=True,
                text=True,
            )
            append_log(log_path, "Configure", result)
            spinner.finish("OK")
        except subprocess.CalledProcessError as e:
            append_log(log_path, "Configure", e)
            spinner.finish("FAILED")
            return combo, False, f"Configure failed:\n{e.stderr[-500:]}"

    with Spinner(f"Building {label}") as spinner:
        try:
            result = subprocess.run(
                build_cmd,
                cwd=str(repo_root),
                check=True,
                capture_output=True,
                text=True,
            )
            append_log(log_path, "Build", result)
            spinner.finish("OK")
        except subprocess.CalledProcessError as e:
            append_log(log_path, "Build", e)
            spinner.finish("FAILED")
            return combo, False, f"Build failed:\n{e.stderr[-500:]}"

    if flash:
        try:
            subprocess.run(
                flash_cmd,
                cwd=str(repo_root),
                check=True,
                text=True,
            )
        except subprocess.CalledProcessError as e:
            stderr_tail = e.stderr[-500:] if e.stderr else "(see terminal output above)"
            return combo, False, f"Flash failed:\n{stderr_tail}"

    return combo, True, str(build_dir / "build")


def cmd_list(args, repo_root, presets):
    """Handle the 'list' subcommand."""
    combinations = discover_all_combinations(repo_root, presets)
    filtered = filter_combinations(
        combinations,
        project=args.project,
        variant=args.variant,
        board=args.board,
    )
    print_table(filtered)


def cmd_build(args, repo_root, presets):
    """Handle the 'build' subcommand."""
    combinations = discover_all_combinations(repo_root, presets)
    filtered = filter_combinations(
        combinations,
        project=args.project,
        variant=args.variant,
        board=args.board,
    )

    if not filtered:
        sys.exit("Error: No matching build combinations found.")

    build_dir_base = Path(args.build_dir) if args.build_dir else repo_root
    total = len(filtered)

    if args.dry_run:
        print(f"Dry run: {total} combination(s)\n")

    # Group by board to identify parallel vs sequential
    by_board = {}
    for c in filtered:
        by_board.setdefault(c["board"], []).append(c)

    passed = 0
    failed = 0
    failures = []

    if args.parallel and len(by_board) > 1:
        # Phase 1: sequential configure for all combinations
        # (avoids racing on submodule cloning)
        print(f"Phase 1: Configuring {total} combination(s) sequentially...")
        configure_failed = set()
        idx = 0
        for board, combos in by_board.items():
            for combo in combos:
                idx += 1
                label = f"{combo['project']}/{combo['variant']} for {combo['board']}"

                defconfig = f"{combo['project']}/{combo['variant']}.conf"
                build_dir = combo_build_dir(build_dir_base, combo)

                if args.clean and build_dir.exists() and not args.dry_run:
                    import shutil
                    shutil.rmtree(build_dir)

                configure_cmd = [
                    "cmake",
                    "-B", str(build_dir),
                    "--preset", combo["preset"],
                    f"-DPROJECT_DEFCONFIG={defconfig}",
                ]
                if args.probe:
                    configure_cmd.append(f"-DPROBE={args.probe}")

                if args.dry_run:
                    print(f"  [{idx}/{total}] {' '.join(configure_cmd)}")
                    continue

                log_path = build_dir / "build.log"
                print(f"  [{idx}/{total}] Configuring {label}...", end=" ", flush=True)
                try:
                    result = subprocess.run(
                        configure_cmd,
                        cwd=str(repo_root),
                        check=True,
                        capture_output=True,
                        text=True,
                    )
                    append_log(log_path, "Configure", result)
                    print("OK")
                except subprocess.CalledProcessError as e:
                    append_log(log_path, "Configure", e)
                    print("FAILED")
                    configure_failed.add((combo["project"], combo["variant"], combo["board"]))
                    failures.append((combo, f"Configure failed:\n{e.stderr[-500:]}"))
                    failed += 1

        # Phase 2: parallel build across boards
        if not args.dry_run:
            print(f"\nPhase 2: Building in parallel across {len(by_board)} board(s)...")

        build_tasks = []
        for combo in filtered:
            key = (combo["project"], combo["variant"], combo["board"])
            if key in configure_failed:
                continue
            build_tasks.append(combo)

        def do_build(combo):
            board = combo["board"]
            build_dir = combo_build_dir(build_dir_base, combo)
            log_path = build_dir / "build.log"
            build_cmd = [
                "cmake",
                "--build", str(build_dir),
                "--target", combo["project"],
            ]
            if args.jobs:
                build_cmd.extend(["-j", str(args.jobs)])

            flash_cmd = [
                "cmake",
                "--build", str(build_dir),
                "--target", "flash",
            ]

            if args.dry_run:
                lines = f"  {' '.join(build_cmd)}"
                if args.flash:
                    lines += f"\n  {' '.join(flash_cmd)}"
                return combo, True, lines

            try:
                result = subprocess.run(
                    build_cmd,
                    cwd=str(repo_root),
                    check=True,
                    capture_output=True,
                    text=True,
                )
                append_log(log_path, "Build", result)
            except subprocess.CalledProcessError as e:
                append_log(log_path, "Build", e)
                return combo, False, f"Build failed:\n{e.stderr[-500:]}"

            artifacts_msg = f"Build artifacts: {build_dir / 'build'}"

            if args.flash:
                try:
                    subprocess.run(
                        flash_cmd,
                        cwd=str(repo_root),
                        check=True,
                        text=True,
                    )
                except subprocess.CalledProcessError as e:
                    stderr_tail = e.stderr[-500:] if e.stderr else "(see terminal output above)"
                    return combo, False, f"Flash failed:\n{stderr_tail}"

            return combo, True, artifacts_msg

        with ThreadPoolExecutor(max_workers=len(by_board)) as executor:
            futures = {executor.submit(do_build, c): c for c in build_tasks}
            for i, future in enumerate(as_completed(futures), 1):
                combo, success, msg = future.result()
                label = f"{combo['project']}/{combo['variant']} for {combo['board']}"
                if args.dry_run:
                    print(msg)
                elif success:
                    print(f"  [{passed + failed + 1}/{total}] Building {label}... OK")
                    if msg:
                        print(f"  {msg}")
                    passed += 1
                else:
                    print(f"  [{passed + failed + 1}/{total}] Building {label}... FAILED")
                    failures.append((combo, msg))
                    failed += 1

        # Count passes from configure-only successes in dry-run
        if args.dry_run:
            return

        passed = total - failed

    else:
        # Sequential execution
        for idx, combo in enumerate(filtered, 1):
            if not args.dry_run and total > 1:
                print(f"[{idx}/{total}]")

            combo_result, success, msg = run_build(
                repo_root, combo, build_dir_base, args.jobs, args.clean, args.dry_run,
                probe=args.probe, flash=args.flash,
            )

            if args.dry_run:
                print()
                continue

            if success:
                print(f"  Build artifacts: {msg}")
                passed += 1
            else:
                failures.append((combo, msg))
                failed += 1

        if args.dry_run:
            return

    # Summary
    print(f"\nSummary: {passed} passed, {failed} failed")
    if failures:
        print("\nFailed builds:")
        for combo, msg in failures:
            print(f"  - {combo['project']}/{combo['variant']} for {combo['board']}")
            if msg:
                for line in msg.strip().split("\n")[-3:]:
                    print(f"    {line}")
        sys.exit(1)


def main():
    parser = argparse.ArgumentParser(
        description="no-OS CMake Build Utility",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    # list subcommand
    list_parser = subparsers.add_parser("list", help="List valid build combinations")
    list_parser.add_argument("--project", help="Filter by project name")
    list_parser.add_argument("--variant", help="Filter by variant name")
    list_parser.add_argument("--board", help="Filter by board name")

    # build subcommand
    build_parser = subparsers.add_parser("build", help="Build project combinations")
    build_parser.add_argument("--project", help="Project to build")
    build_parser.add_argument("--variant", help="Variant to build")
    build_parser.add_argument("--board", help="Board to build for")
    build_parser.add_argument(
        "--build-dir", help="Base directory where build-<board> directories are created (default: repo root)"
    )
    build_parser.add_argument(
        "--jobs", "-j", type=int, help="Parallel jobs for cmake --build"
    )
    build_parser.add_argument(
        "--clean", action="store_true", help="Remove build dir before configure"
    )
    build_parser.add_argument(
        "--dry-run", action="store_true", help="Print cmake commands without executing"
    )
    build_parser.add_argument(
        "--parallel",
        action="store_true",
        help="Build different boards in parallel (configure stays sequential)",
    )
    build_parser.add_argument(
        "--probe",
        choices=["jlink", "openocd"],
        help="Debug probe type; sets -DPROBE=<value> at configure time",
    )
    build_parser.add_argument(
        "--flash",
        action="store_true",
        help="Flash the firmware after a successful build (requires --probe)",
    )

    args = parser.parse_args()

    repo_root = find_repo_root()
    presets = load_presets(repo_root)

    if args.command == "list":
        cmd_list(args, repo_root, presets)
    elif args.command == "build":
        if args.flash and not args.probe:
            parser.error("--flash requires --probe")
        cmd_build(args, repo_root, presets)


if __name__ == "__main__":
    main()