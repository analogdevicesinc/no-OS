#!/usr/bin/env python3
# Copyright 2026(c) Analog Devices, Inc.
#
# SPDX short identifier: BSD-1-Clause
"""Select the minimal set of CI builds for a change set.

CI builds every (project, variant, board) combination on every push, which is
wasteful for a PR that only touches one project or one driver. This script maps
the changed files to the combinations that actually need rebuilding and writes a
filter file that build_projects.py consumes to restrict its work.

The mapping is entirely static (no CMake configure needed):

  * A build's driver manifest is its project/<variant>.conf: the CONFIG_*=y
    symbols it enables. This is the source of truth for "which build uses which
    driver".
  * A driver source file maps to a CONFIG symbol via the
    no_os_sources_ifdef(CONFIG_X <path>) calls in each driver CMakeLists.txt.
  * Kconfig `select` edges close the gap where a conf enables a high-level
    symbol (e.g. CONFIG_MOTOR_IIO_TMC5240) that selects the one a changed file
    is gated on (CONFIG_MOTOR_TMC5240) without naming it.

Classification of a changed path, first match wins:

  doc/**, **/*.md, **/*.rst          -> docs build only, no project builds
  tests/**                           -> unit tests only
  projects/<P>/**                    -> all builds of project <P>
  drivers/platform/<plat>/**         -> all builds whose platform is <plat>
  drivers|iio|network|jesd204/**     -> builds enabling that dir's CONFIG symbols
  libraries/**, cmake/libraries/**   -> builds enabling that library's symbol
  include/**, capi/**, util/**       -> BUILD ALL (linked into every target)
  cmake/**, CMakeLists.txt, Kconfig, -> BUILD ALL (build-system change)
    CMakePresets.json, tools/scripts/no_os_build.py, ci/**
  anything else                      -> BUILD ALL (fail-safe)

Every ambiguity resolves to *more* building, never less: an unknown path, a
parse failure, or an empty result set builds everything. A missed build breaks
the merge; an extra build only costs time.

Output is a JSON filter file:

  {
    "build_all": false,
    "build_docs": true,
    "run_tests": false,
    "combos": [{"project": "admt4000", "variant": "basic", "board": "sdp-ck1z"},
               ...]
  }

When build_all is true, "combos" is omitted and consumers build the full set.
"""
import argparse
import json
import re
import subprocess
import sys
from pathlib import Path

# no_os_build.py guards its CLI behind __main__, so importing it here is safe
# and reuses the exact combo-discovery the builder uses.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from no_os_build import (
    find_repo_root,
    load_presets,
    discover_all_combinations,
)

# Source subtrees whose files are gated on CONFIG symbols via
# no_os_sources_ifdef, so a change maps to specific builds.
GATED_TREES = ("drivers", "iio", "network", "jesd204")

# libraries/<dir> -> the CONFIG symbol whose confs pull that library in.
# pico-sdk is fetched by the toolchain for every pico build, so it maps to the
# platform rather than a CONFIG symbol (handled separately).
LIB_DIR_TO_SYMBOL = {
    "azure": "CONFIG_AZURE",
    "esh": "CONFIG_ESH",
    "free_rtos": "CONFIG_FREERTOS",
    "lvgl": "CONFIG_LVGL",
    "lwip": "CONFIG_LWIP",
    "mbedtls": "CONFIG_MBEDTLS",
    "mqtt": "CONFIG_MQTT",
    "fatfs": "CONFIG_FATFS",
    # precision-converters-library and tmc have no top-level enable symbol;
    # they are pulled in by driver/example symbols, so a change to them is
    # matched through the driver CMakeLists that references their path.
}

# no_os_sources_ifdef(CONFIG_X path...) / no_os_sources_dir_ifdef(CONFIG_X dir)
_IFDEF_RE = re.compile(
    r"no_os_(?:sources|sources_dir|sources_dir_recurse|include_dir"
    r"|include_dir_recurse)_ifdef\s*\(\s*(CONFIG_[A-Z0-9_]+)(.*?)\)",
    re.DOTALL,
)
# ${CMAKE_CURRENT_SOURCE_DIR}/foo/bar.c  or  ${CMAKE_SOURCE_DIR}/libraries/...
_PATH_TOKEN_RE = re.compile(r"\$\{(CMAKE_CURRENT_SOURCE_DIR|CMAKE_SOURCE_DIR)\}/([^\s\)]+)")


def git_changed_files(repo_root, base, head):
    """Return the list of paths changed between base and head (repo-relative)."""
    rng = f"{base}...{head}" if base and head else "HEAD~1...HEAD"
    out = subprocess.run(
        ["git", "-C", str(repo_root), "diff", "--name-only", rng],
        capture_output=True, text=True, check=True,
    )
    return [line.strip() for line in out.stdout.splitlines() if line.strip()]


def build_conf_symbol_index(repo_root, combos):
    """symbol -> set of combo keys, from every combo's <variant>.conf.

    A combo's driver manifest is projects/<project>/<variant>.conf; the board
    .conf only adds platform wiring (bus instances) and never a driver, so it
    is not indexed for driver mapping.
    """
    index = {}
    conf_cache = {}
    for combo in combos:
        conf = repo_root / "projects" / combo["project"] / f"{combo['variant']}.conf"
        if conf not in conf_cache:
            conf_cache[conf] = _read_enabled_symbols(conf)
        key = _combo_key(combo)
        for sym in conf_cache[conf]:
            index.setdefault(sym, set()).add(key)
    return index


def _read_enabled_symbols(conf_path):
    """Return the set of CONFIG_* symbols set to y in a .conf file."""
    syms = set()
    if not conf_path.is_file():
        return syms
    for line in conf_path.read_text().splitlines():
        line = line.strip()
        m = re.match(r"(CONFIG_[A-Z0-9_]+)\s*=\s*y\b", line)
        if m:
            syms.add(m.group(1))
    return syms


def build_path_symbol_index(repo_root):
    """Map each gated source directory to the CONFIG symbols compiled from it.

    Directory-granular by design: a changed file is attributed to every symbol
    referenced from files under the same driver leaf directory. This slightly
    over-selects within a single driver dir (safe) and is robust to path
    spelling, globs, and multi-source ifdef calls.

    Each CMakeLists.txt's own directory is also indexed with the union of every
    symbol it references. This keeps an aggregating file that lists many drivers
    (e.g. drivers/afe/CMakeLists.txt, or its sibling Kconfig, which resolves to
    the same dir via nearest-ancestor lookup) mapped to just that subtree's
    symbols rather than falling back to build-all. A change to a leaf source
    still resolves to the more specific leaf dir, since the lookup walks up to
    the nearest indexed directory.

    Returns dict: repo-relative dir (posix str) -> set(symbols).
    """
    dir_to_syms = {}
    for tree in GATED_TREES:
        tree_root = repo_root / tree
        if not tree_root.is_dir():
            continue
        for cml in tree_root.rglob("CMakeLists.txt"):
            cml_dir = cml.parent
            try:
                cml_key = cml_dir.relative_to(repo_root).as_posix()
            except ValueError:
                cml_key = None
            text = cml.read_text()
            for m in _IFDEF_RE.finditer(text):
                symbol = m.group(1)
                # The CMakeLists (and its sibling Kconfig) belongs to every
                # symbol it gates: a change to it may affect any of them.
                if cml_key is not None:
                    dir_to_syms.setdefault(cml_key, set()).add(symbol)
                for pm in _PATH_TOKEN_RE.finditer(m.group(2)):
                    anchor, rel = pm.group(1), pm.group(2)
                    base = cml_dir if anchor == "CMAKE_CURRENT_SOURCE_DIR" else repo_root
                    target = (base / rel).resolve()
                    # Attribute the enclosing directory of the referenced path.
                    src_dir = target.parent if target.suffix else target
                    try:
                        key = src_dir.relative_to(repo_root).as_posix()
                    except ValueError:
                        continue
                    dir_to_syms.setdefault(key, set()).add(symbol)
    return dir_to_syms


def build_select_closure(repo_root):
    """Map each symbol S to the set of symbols that (transitively) select S.

    A conf may enable only a high-level symbol that `select`s the one a changed
    file is gated on. To rebuild that combo, when file->S we must also match
    confs enabling any symbol that reaches S through select edges.

    Returns dict: symbol -> set(symbols that pull it in, including itself).
    """
    # Parse `config NAME` blocks and their `select TARGET` lines across all
    # Kconfig files.
    selects = {}  # selector -> set(selected)
    for kconfig in repo_root.rglob("Kconfig"):
        # Skip fetched dependency trees and worktrees.
        parts = kconfig.relative_to(repo_root).parts
        if parts[0] in ("libraries", ".claude", "build") or "build" in parts:
            # libraries/*/Kconfig belong to vendored deps; no-OS never sources
            # them for project config.
            if parts[0] == "libraries":
                continue
        current = None
        try:
            lines = kconfig.read_text().splitlines()
        except (OSError, UnicodeDecodeError):
            continue
        for line in lines:
            cm = re.match(r"\s*(?:menuconfig|config)\s+([A-Z0-9_]+)", line)
            if cm:
                current = "CONFIG_" + cm.group(1)
                continue
            sm = re.match(r"\s*select\s+([A-Z0-9_]+)", line)
            if sm and current:
                selects.setdefault(current, set()).add("CONFIG_" + sm.group(1))

    # Invert and transitively close: for each target, who reaches it?
    reachers = {}  # target -> set(selectors reaching it)
    for selector, targets in selects.items():
        for t in targets:
            reachers.setdefault(t, set()).add(selector)

    def closure(target):
        seen = {target}
        stack = list(reachers.get(target, ()))
        while stack:
            s = stack.pop()
            if s in seen:
                continue
            seen.add(s)
            stack.extend(reachers.get(s, ()))
        return seen

    return {sym: closure(sym) for sym in set(reachers)}


def _combo_key(combo):
    return (combo["project"], combo["variant"], combo["board"])


def classify(repo_root, changed, combos, conf_index, path_index, select_closure):
    """Return (build_all, build_docs, run_tests, set-of-combo-keys)."""
    build_docs = False
    run_tests = False
    selected = set()

    platforms = {c["platform"] for c in combos}
    combos_by_project = {}
    combos_by_platform = {}
    for c in combos:
        combos_by_project.setdefault(c["project"], []).append(c)
        combos_by_platform.setdefault(c["platform"], []).append(c)

    def symbols_to_combos(symbols):
        keys = set()
        for sym in symbols:
            # Expand through select-closure: any symbol that pulls sym in.
            for reacher in select_closure.get(sym, {sym}):
                keys |= conf_index.get(reacher, set())
            keys |= conf_index.get(sym, set())
        return keys

    for path in changed:
        p = path.replace("\\", "/")
        parts = p.split("/")

        # --- docs: build docs only, contributes no project builds ---
        if p.startswith("doc/") or p.endswith(".md") or p.endswith(".rst"):
            build_docs = True
            continue

        # --- unit tests ---
        if p.startswith("tests/"):
            run_tests = True
            continue

        # --- a specific project ---
        if p.startswith("projects/") and len(parts) >= 2:
            proj = parts[1]
            for c in combos_by_project.get(proj, []):
                selected.add(_combo_key(c))
            continue

        # --- a platform: every build on that platform ---
        if p.startswith("drivers/platform/") and len(parts) >= 3:
            plat = parts[2]
            # Kconfig platform dir names don't always equal preset PLATFORM
            # strings (e.g. free_rtos). Match known platforms; unknown -> all.
            if plat in platforms:
                for c in combos_by_platform.get(plat, []):
                    selected.add(_combo_key(c))
                continue
            return True, build_docs, run_tests, set()

        # --- a gated driver / subsystem source tree ---
        if parts[0] in GATED_TREES:
            # Directory-granular: find the nearest indexed dir at or above the
            # changed file.
            syms = _lookup_dir_symbols(p, path_index)
            if syms:
                selected |= symbols_to_combos(syms)
                continue
            # A file in a gated tree we couldn't map (e.g. a shared header) is
            # too risky to drop.
            return True, build_docs, run_tests, set()

        # --- a bundled library ---
        if p.startswith("libraries/") and len(parts) >= 2:
            libdir = parts[1]
            sym = LIB_DIR_TO_SYMBOL.get(libdir)
            if sym:
                selected |= symbols_to_combos({sym})
                continue
            if libdir == "pico-sdk":
                for c in combos_by_platform.get("pico", []):
                    selected.add(_combo_key(c))
                continue
            # precision-converters-library / tmc have no enable symbol; a driver
            # CMakeLists references their path directly (e.g. TMC-API sources
            # gated on CONFIG_MOTOR_TMC5240), so path_index may attribute them.
            syms = _lookup_dir_symbols(p, path_index)
            if syms:
                selected |= symbols_to_combos(syms)
                continue
            # Unattributable library change: build everywhere to be safe.
            return True, build_docs, run_tests, set()

        # --- cmake/libraries/<lib>.cmake: same mapping as the lib dir ---
        if p.startswith("cmake/libraries/") and p.endswith(".cmake"):
            stem = Path(p).stem.lower()
            alias = {"freertos": "free_rtos", "pcl": None, "tmc": None}
            libdir = alias.get(stem, stem)
            sym = LIB_DIR_TO_SYMBOL.get(libdir) if libdir else None
            if sym:
                selected |= symbols_to_combos({sym})
                continue
            return True, build_docs, run_tests, set()

        # --- everything else that is core / build-system -> BUILD ALL ---
        return True, build_docs, run_tests, set()

    return False, build_docs, run_tests, selected


def _lookup_dir_symbols(path, path_index):
    """Symbols for the changed file's nearest indexed ancestor directory."""
    d = Path(path).parent
    while True:
        key = d.as_posix()
        if key in path_index:
            return path_index[key]
        if d == Path("."):
            return set()
        parent = d.parent
        if parent == d:
            return set()
        d = parent


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--flags", metavar="FILTER",
                    help="Read an existing filter JSON and print shell "
                    "assignments (BUILD_ALL / RUN_TESTS) to stdout, then exit. "
                    "Used by CI to gate steps; ignores all other arguments.")
    ap.add_argument("--base", help="Base git ref (PR merge base)")
    ap.add_argument("--head", help="Head git ref (PR tip)")
    ap.add_argument("--changed-file", help="Read changed paths from this file "
                    "(one per line) instead of running git diff")
    ap.add_argument("--output", "-o", help="Path to write the JSON filter")
    ap.add_argument("--print", dest="do_print", action="store_true",
                    help="Also print a human summary to stderr")
    args = ap.parse_args()

    # --flags: read an existing filter and emit shell variable assignments.
    if args.flags:
        data = json.loads(Path(args.flags).read_text())
        build_all = bool(data.get("build_all"))
        run_tests = bool(data.get("run_tests")) or build_all
        print(f"BUILD_ALL={'true' if build_all else 'false'}")
        print(f"RUN_TESTS={'true' if run_tests else 'false'}")
        return

    if not args.output:
        ap.error("--output is required unless --flags is given")

    repo_root = find_repo_root()

    if args.changed_file:
        changed = [l.strip() for l in Path(args.changed_file).read_text().splitlines()
                   if l.strip()]
    else:
        changed = git_changed_files(repo_root, args.base, args.head)

    presets = load_presets(repo_root)
    combos = discover_all_combinations(repo_root, presets)

    if not changed:
        # No changed files resolved: build nothing but stay safe on docs/tests.
        result = {"build_all": False, "build_docs": False, "run_tests": False,
                  "combos": []}
        Path(args.output).write_text(json.dumps(result, indent=2))
        if args.do_print:
            print("No changed files; selecting no builds.", file=sys.stderr)
        return

    conf_index = build_conf_symbol_index(repo_root, combos)
    path_index = build_path_symbol_index(repo_root)
    select_closure = build_select_closure(repo_root)

    build_all, build_docs, run_tests, keys = classify(
        repo_root, changed, combos, conf_index, path_index, select_closure)

    if build_all:
        result = {"build_all": True, "build_docs": build_docs, "run_tests": run_tests}
    else:
        selected_combos = [
            {"project": p, "variant": v, "board": b}
            for (p, v, b) in sorted(keys)
        ]
        result = {"build_all": False, "build_docs": build_docs,
                  "run_tests": run_tests, "combos": selected_combos}

    Path(args.output).write_text(json.dumps(result, indent=2))

    if args.do_print:
        if build_all:
            print(f"BUILD ALL (docs={build_docs}, tests={run_tests})", file=sys.stderr)
        else:
            print(f"{len(keys)} build(s) selected "
                  f"(docs={build_docs}, tests={run_tests})", file=sys.stderr)


if __name__ == "__main__":
    main()
