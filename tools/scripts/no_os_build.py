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
import shlex
import shutil
import signal
import subprocess
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path


USE_TTY = sys.stdout.isatty()


def _resolve_cmake():
    """Return the first cmake on PATH that is not the Vitis-bundled one.

    Sourcing settings64.sh prepends Vitis's ancient bundled cmake, which is
    linked against libs absent on modern distros and fails to run. Fall back to
    'cmake' if no other candidate exists.
    """
    for directory in os.environ.get("PATH", "").split(os.pathsep):
        if not directory:
            continue
        candidate = os.path.join(directory, "cmake")
        if not (os.path.isfile(candidate) and os.access(candidate, os.X_OK)):
            continue
        parts = Path(candidate).resolve().parts
        # Vitis bundles cmake at <root>/tps/lnx64/cmake-<ver>/bin/cmake.
        if "tps" in parts and any(p.startswith("cmake-") for p in parts):
            continue
        return candidate
    return "cmake"


CMAKE = _resolve_cmake()


def combo_build_dir(build_dir_base, combo):
    """Return the build directory path for a given combination."""
    name = f"{combo['project']}-{combo['variant']}-{combo['board']}"
    return build_dir_base / name


def open_vscode_workspace(repo_root):
    """Open the generated VS Code workspace, if present and an editor is found.

    The workspace is generated at the repo root by the IDE backend during
    configure. VS Code does not reliably prompt to open it, so --open launches
    it explicitly.
    """
    workspace = repo_root / "no-os.code-workspace"
    if not workspace.exists():
        print(f"--open: workspace not found at {workspace} "
              "(it is generated during configure).", file=sys.stderr)
        return
    editor = shutil.which("code") or shutil.which("codium")
    if not editor:
        print(f"--open: 'code' not found on PATH. Open it manually:\n"
              f"  code {workspace}", file=sys.stderr)
        return
    print(f"Opening VS Code workspace: {workspace}")
    try:
        subprocess.run([editor, str(workspace)], check=True)
    except subprocess.CalledProcessError as e:
        print(f"--open: failed to launch '{editor}': {e}", file=sys.stderr)


def read_cmake_cache_value(build_dir, key):
    """Return the value of a CMakeCache.txt entry (KEY:TYPE=VALUE), or None."""
    cache = build_dir / "CMakeCache.txt"
    if not cache.exists():
        return None
    prefix = f"{key}:"
    with open(cache) as f:
        for line in f:
            if line.startswith(prefix) and "=" in line:
                return line.split("=", 1)[1].strip()
    return None


def harvest_compile_manifest(build_dir):
    """Extract the source/include/define set from compile_commands.json.

    CMake writes compile_commands.json (CMAKE_EXPORT_COMPILE_COMMANDS) at the
    build-dir root with one entry per compiled translation unit. It is the
    ground truth for what the no-OS ELF is built from: the union of the project
    target and the `no-os` OBJECT library. We parse the -I/-D/-include flags
    out of each command so the Vitis app component indexes headers and macros
    exactly as ninja did.

    Returns a dict {"sources": [...], "includes": [...], "defines": [...],
    "force_includes": [...]} with duplicates removed and insertion order
    preserved, or None if the compile database is missing.
    """
    cc = build_dir / "compile_commands.json"
    if not cc.exists():
        return None
    with open(cc) as f:
        entries = json.load(f)

    sources, includes, defines, force_includes = [], [], [], []
    seen_src, seen_inc, seen_def, seen_finc = set(), set(), set(), set()
    for e in entries:
        src = e.get("file")
        if src and src not in seen_src:
            seen_src.add(src)
            sources.append(src)
        tokens = shlex.split(e.get("command", ""))
        i = 0
        while i < len(tokens):
            tok = tokens[i]
            if tok.startswith("-I"):
                val = tok[2:] or (tokens[i + 1] if tok == "-I" else "")
                if tok == "-I":
                    i += 1
                if val and val not in seen_inc:
                    seen_inc.add(val)
                    includes.append(val)
            elif tok.startswith("-D"):
                val = tok[2:] or (tokens[i + 1] if tok == "-D" else "")
                if tok == "-D":
                    i += 1
                if val and val not in seen_def:
                    seen_def.add(val)
                    defines.append(val)
            elif tok == "-include":
                i += 1
                if i < len(tokens):
                    val = tokens[i]
                    if val and val not in seen_finc:
                        seen_finc.add(val)
                        force_includes.append(val)
            i += 1
    return {"sources": sources, "includes": includes, "defines": defines,
            "force_includes": force_includes}


def open_vitis_workspace(repo_root, build_dir, combo):
    """Populate and open a Vitis Unified IDE workspace for a Xilinx project.

    Xilinx debugging/browsing happens in the Vitis GUI, not VS Code + OpenOCD.
    Unlike the BSP-only xsa_work that config_xilinx_sdk leaves behind, this
    materializes a real, openable workspace under
    <build>/projects/<project>/xsa_work/ide/workspace containing:

      - the hw0 platform (BSP), and
      - an 'app' component whose sources are the no-OS files CMake actually
        compiled, referenced in place (import_files is_skip_copy_sources) with
        the include paths and -D defines harvested from compile_commands.json.

    This mirrors the intent of the legacy `make` flow (which symlinked the
    required no-OS sources under build/app so they showed up in Vitis), adapted
    to the CMake build: CMake still owns the flashed ELF; Vitis gets a browsable
    /buildable view of the same sources. Then launches `vitis -w <workspace>`.
    """
    proj_bin = build_dir / "projects" / combo["project"]
    xsa_work = proj_bin / "xsa_work"
    workspace = xsa_work / "ide" / "workspace"
    if not xsa_work.exists():
        print(f"--open: BSP work dir not found at {xsa_work} "
              "(build the project first).", file=sys.stderr)
        return
    vitis = shutil.which("vitis")
    if not vitis:
        print("--open: 'vitis' not found on PATH. Source settings64.sh from "
              f"your Vitis install, then re-run --open.", file=sys.stderr)
        return

    manifest = harvest_compile_manifest(build_dir)
    if manifest is None:
        print(f"--open: compile_commands.json not found in {build_dir} "
              "(configure/build the project first).", file=sys.stderr)
        return
    # The BSP CPU/arch is resolved by the toolchain and cached; pass it through
    # so util.py need not depend on a freshly staged arch.txt.
    arch = read_cmake_cache_value(build_dir, "XILINX_ARCH")
    if arch:
        manifest["arch"] = arch
    manifest_path = xsa_work / "ide_manifest.json"
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    with open(manifest_path, "w") as f:
        json.dump(manifest, f, indent="\t")

    # xsa_work holds a copy of the .xsa and arch.txt (staged by
    # config_xilinx_sdk); util.py reads both from hw_path.
    xsa_files = list(xsa_work.glob("*.xsa"))
    if not xsa_files:
        print(f"--open: no .xsa found in {xsa_work}.", file=sys.stderr)
        return
    util_py = repo_root / "tools" / "scripts" / "platform" / "xilinx" / "util.py"

    print(f"Populating Vitis workspace ({len(manifest['sources'])} sources)...")
    try:
        subprocess.run(
            [vitis, "-s", str(util_py), "create_ide_workspace",
             str(xsa_work), str(xsa_work), xsa_files[0].name,
             str(manifest_path)],
            check=True, cwd=str(repo_root))
    except subprocess.CalledProcessError as e:
        print(f"--open: failed to populate Vitis workspace: {e}",
              file=sys.stderr)
        return

    print(f"Opening Vitis IDE workspace: {workspace}")
    try:
        subprocess.run([vitis, "-w", str(workspace)], check=True)
    except subprocess.CalledProcessError as e:
        print(f"--open: failed to launch 'vitis': {e}", file=sys.stderr)


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


def _read_conf_string(conf_path, symbol):
    """Return the value of a CONFIG_<symbol>="..." assignment in a .conf file.

    Kconfig string fragments look like CONFIG_FOO="bar". Returns the unquoted
    value, or None if the file or symbol is absent. Deliberately a plain text
    scan so it runs before (and without) any cmake/Kconfig invocation.
    """
    if not conf_path.is_file():
        return None
    key = f"CONFIG_{symbol}"
    for line in conf_path.read_text().splitlines():
        line = line.strip()
        if line.startswith("#") or "=" not in line:
            continue
        name, _, value = line.partition("=")
        if name.strip() == key:
            return value.strip().strip('"')
    return None


def xilinx_hardware_name(repo_root, project, variant, board):
    """Compose the HDL hardware name for a Xilinx (project, variant, board).

    The name is <CONFIG_XILINX_HDL_DESIGN>_<board> (e.g. adv7511_zed), matching
    the artifact-server folder that holds system_top.xsa. The design prefix
    lives in the variant .conf; the board suffix is the CMake board. Returns
    None when the variant declares no design (i.e. not a Xilinx build).
    """
    conf = repo_root / "projects" / project / f"{variant}.conf"
    design = _read_conf_string(conf, "XILINX_HDL_DESIGN")
    if not design:
        return None
    return f"{design}_{board}"


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


def quote_cmd(cmd):
    """Render a command list as a copy-pasteable string.

    Tokens containing spaces or shell/backslash characters are wrapped in
    double quotes so the printed command works when pasted into a shell on
    both POSIX and Windows (where build paths routinely contain spaces and
    backslashes, e.g. C:\\Users\\John Doe\\no-os).
    """
    parts = []
    for token in cmd:
        if token and not any(ch in token for ch in ' \t"\\'):
            parts.append(token)
        else:
            parts.append('"' + token.replace('"', '\\"') + '"')
    return " ".join(parts)


def echo_cmd(cmd):
    """Echo a cmake command to stdout before it is executed."""
    print(f"  $ {quote_cmd(cmd)}", flush=True)


def append_log(log_path, section, result, note=None):
    """Append a labelled section of captured output to the build log."""
    with open(log_path, "a") as f:
        f.write(f"=== {section} ===\n")
        if note:
            f.write(f"[{note}]\n")
        if result and result.stdout:
            f.write(result.stdout)
        if result and result.stderr:
            f.write(result.stderr)
        f.write("\n")


def _run_cmd(cmd, cwd, timeout, capture=True, check=True):
    """Run a command with a hard timeout, killing its whole process tree on expiry.

    The Xilinx BSP generation (`vitis -s util.py create_project`) can hang
    forever when no JTAG hardware is attached: vitis's XSDB server launch
    stalls on target discovery and nothing above it times out. The child runs
    in its own session/process group so killpg() reaps the vitis/java
    grandchildren that would otherwise orphan and hold workspace/TCF locks.
    """
    kwargs = dict(cwd=cwd, text=True, start_new_session=True)
    if capture:
        kwargs["stdout"] = subprocess.PIPE
        kwargs["stderr"] = subprocess.PIPE
    with subprocess.Popen(cmd, **kwargs) as proc:
        try:
            stdout, stderr = proc.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            # proc.pid is the process-group id (start_new_session=True):
            # SIGKILL the whole group so vitis/java servers spawned by cmake
            # die too instead of orphaning and holding workspace/TCF locks.
            try:
                os.killpg(proc.pid, signal.SIGKILL)
            except (ProcessLookupError, PermissionError):
                pass
            proc.wait()
            raise
        retcode = proc.poll()
        if check and retcode:
            raise subprocess.CalledProcessError(
                retcode, cmd, output=stdout, stderr=stderr)
        return subprocess.CompletedProcess(cmd, retcode, stdout, stderr)


def run_build(repo_root, combo, build_dir_base, jobs, clean, dry_run, probe=None, flash=False, fresh=False, hardware=None, timeout=1800):
    """Run cmake configure + build (and optionally flash) for a single combination.

    Returns (combo, success, detail). On failure, detail is the error message.
    On success, detail is the build artifacts path (empty for a dry run).
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

    # append_log() opens in append mode; on a reused build_dir that accumulates
    # stale output. Start each run with a fresh log.
    if not dry_run and log_path.exists():
        log_path.unlink()

    defconfig = f"{project}/{variant}.conf"

    configure_cmd = [
        CMAKE,
        "-B", str(build_dir),
        "--preset", preset,
        f"-DPROJECT_DEFCONFIG={defconfig}",
    ]
    if fresh:
        configure_cmd.append("--fresh")
    if probe:
        configure_cmd.append(f"-DPROBE={probe}")
    if hardware:
        configure_cmd.append(f"-DHARDWARE={Path(hardware).resolve()}")

    build_cmd = [
        CMAKE,
        "--build", str(build_dir),
        "--target", project,
    ]
    if jobs:
        build_cmd.extend(["-j", str(jobs)])

    flash_cmd = [
        CMAKE,
        "--build", str(build_dir),
        "--target", "flash",
    ]

    if dry_run:
        print(f"  {quote_cmd(configure_cmd)}")
        print(f"  {quote_cmd(build_cmd)}")
        if flash:
            print(f"  {quote_cmd(flash_cmd)}")
        return combo, True, ""

    label = f"{project}/{variant} for {board}"

    with Spinner(f"Configuring {label}") as spinner:
        echo_cmd(configure_cmd)
        try:
            result = _run_cmd(configure_cmd, str(repo_root), timeout)
            append_log(log_path, "Configure", result)
            spinner.finish("OK")
        except subprocess.TimeoutExpired:
            append_log(log_path, "Configure", None, note=f"TIMEOUT after {timeout}s")
            spinner.finish("TIMEOUT")
            return combo, False, (
                f"Configure timed out after {timeout}s (vitis BSP generation hung; "
                f"no JTAG hardware attached?). See {log_path} for details. "
                f"Kill leftover vitis processes: pkill -f 'RigelApp'")
        except subprocess.CalledProcessError as e:
            append_log(log_path, "Configure", e)
            spinner.finish("FAILED")
            return combo, False, f"Configure failed:\n{e.stderr[-500:]}"

    with Spinner(f"Building {label}") as spinner:
        echo_cmd(build_cmd)
        try:
            result = _run_cmd(build_cmd, str(repo_root), timeout)
            append_log(log_path, "Build", result)
            spinner.finish("OK")
        except subprocess.TimeoutExpired:
            append_log(log_path, "Build", None, note=f"TIMEOUT after {timeout}s")
            spinner.finish("TIMEOUT")
            return combo, False, f"Build timed out after {timeout}s"
        except subprocess.CalledProcessError as e:
            append_log(log_path, "Build", e)
            spinner.finish("FAILED")
            return combo, False, f"Build failed:\n{e.stderr[-500:]}"

    if flash:
        echo_cmd(flash_cmd)
        try:
            _run_cmd(flash_cmd, str(repo_root), timeout, capture=False)
        except subprocess.TimeoutExpired:
            return combo, False, f"Flash timed out after {timeout}s"
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

    # Resolve --build-dir to an absolute path. cmake runs with cwd=repo_root,
    # so a relative --build-dir is anchored there (not the invocation CWD) to
    # keep the directory Python cleans/logs to identical to the one cmake builds
    # in. Absolute paths are used as-is.
    if args.build_dir:
        build_dir_base = Path(args.build_dir)
        if not build_dir_base.is_absolute():
            build_dir_base = repo_root / build_dir_base
    else:
        build_dir_base = repo_root / "build"
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
                    CMAKE,
                    "-B", str(build_dir),
                    "--preset", combo["preset"],
                    f"-DPROJECT_DEFCONFIG={defconfig}",
                ]
                if args.fresh:
                    configure_cmd.append("--fresh")
                if args.probe:
                    configure_cmd.append(f"-DPROBE={args.probe}")
                if args.hardware:
                    configure_cmd.append(f"-DHARDWARE={Path(args.hardware).resolve()}")

                if args.dry_run:
                    print(f"  [{idx}/{total}] {quote_cmd(configure_cmd)}")
                    continue

                log_path = build_dir / "build.log"
                print(f"  [{idx}/{total}] Configuring {label}...", flush=True)
                echo_cmd(configure_cmd)
                try:
                    result = _run_cmd(configure_cmd, str(repo_root), args.timeout)
                    append_log(log_path, "Configure", result)
                    print("OK")
                except subprocess.TimeoutExpired:
                    append_log(log_path, "Configure", None,
                               note=f"TIMEOUT after {args.timeout}s")
                    print("TIMEOUT")
                    configure_failed.add((combo["project"], combo["variant"], combo["board"]))
                    failures.append((combo, f"Configure timed out after {args.timeout}s "
                                           f"(vitis BSP generation hung; no JTAG hardware attached?)"))
                    failed += 1
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

        # Builds run concurrently across boards, but flashing must not: all
        # boards typically share a single debug probe (and host USB), so two
        # `cmake --target flash` invocations at once would contend for it and
        # fail or program the wrong target. Serialize the flash step with a
        # lock while keeping the compile step parallel.
        flash_lock = threading.Lock()
        if args.flash and len(build_tasks) > 1:
            print("Note: building in parallel, but flashing is serialized "
                  "(boards share a single debug probe).")

        def do_build(combo):
            board = combo["board"]
            build_dir = combo_build_dir(build_dir_base, combo)
            log_path = build_dir / "build.log"
            build_cmd = [
                CMAKE,
                "--build", str(build_dir),
                "--target", combo["project"],
            ]
            if args.jobs:
                build_cmd.extend(["-j", str(args.jobs)])

            flash_cmd = [
                CMAKE,
                "--build", str(build_dir),
                "--target", "flash",
            ]

            if args.dry_run:
                lines = f"  {quote_cmd(build_cmd)}"
                if args.flash:
                    lines += f"\n  {quote_cmd(flash_cmd)}"
                return combo, True, lines

            echo_cmd(build_cmd)
            try:
                result = _run_cmd(build_cmd, str(repo_root), args.timeout)
                append_log(log_path, "Build", result)
            except subprocess.TimeoutExpired:
                append_log(log_path, "Build", None, note=f"TIMEOUT after {args.timeout}s")
                return combo, False, f"Build timed out after {args.timeout}s"
            except subprocess.CalledProcessError as e:
                append_log(log_path, "Build", e)
                return combo, False, f"Build failed:\n{e.stderr[-500:]}"

            artifacts_msg = f"Build artifacts: {build_dir / 'build'}"

            if args.flash:
                echo_cmd(flash_cmd)
                # Hold the lock for the whole flash so only one board is
                # programmed at a time, even though builds run in parallel.
                with flash_lock:
                    try:
                        _run_cmd(flash_cmd, str(repo_root), args.timeout, capture=False)
                    except subprocess.TimeoutExpired:
                        return combo, False, f"Flash timed out after {args.timeout}s"
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

        # Dry run only prints the planned commands; nothing to summarize.
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
                probe=args.probe, flash=args.flash, fresh=args.fresh,
                hardware=args.hardware, timeout=args.timeout,
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

    if args.open:
        # --open targets a single project's IDE. When the filter matched exactly
        # one combination, open that; otherwise fall back to the repo-root VS
        # Code workspace (the multi-project view).
        if len(filtered) == 1:
            combo = filtered[0]
            build_dir = combo_build_dir(build_dir_base, combo)
            if combo["platform"] == "xilinx":
                open_vitis_workspace(repo_root, build_dir, combo)
            else:
                open_vscode_workspace(repo_root)
        else:
            open_vscode_workspace(repo_root)


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
        "--build-dir", help="Base directory where <project>-<variant>-<board> directories are created (default: build/ at the repo root)"
    )
    build_parser.add_argument(
        "--jobs", "-j", type=int, help="Parallel jobs for cmake --build"
    )
    build_parser.add_argument(
        "--timeout",
        type=int,
        default=1800,
        help="Max seconds for each cmake configure/build step before it is "
             "killed with a clear error (default: 1800). vitis BSP generation "
             "can legitimately take several minutes, but hangs forever when no "
             "JTAG hardware is attached, so a bound is needed.",
    )
    build_parser.add_argument(
        "--clean", action="store_true", help="Remove build dir before configure"
    )
    build_parser.add_argument(
        "--fresh", action="store_true", help="Pass --fresh to cmake configure (removes CMakeCache.txt and CMakeFiles/)"
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
        "--hardware",
        help="Path to a Xilinx .xsa hardware file; passed to cmake as "
             "-DHARDWARE=<abs path> (required for xilinx builds)",
    )
    build_parser.add_argument(
        "--flash",
        action="store_true",
        help="Flash the firmware after a successful build (requires --probe)",
    )
    build_parser.add_argument(
        "--open",
        action="store_true",
        help="Open the generated VS Code workspace after a successful build",
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
