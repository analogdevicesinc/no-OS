#!/usr/bin/env python3
# Convert a Madura TES ".profile" (JSON) into the C header the adrv902x
# firmware expects: a single string literal named json_profile_active_use_case,
# one source line per continued C line.
#
# The firmware reads its JESD/Rx/Tx use-case from
#   src/common/firmware/<PROFILE>/ActiveUseCase_profile.h
# which must stay in sync with the matching Linux firmware/.profile the same
# bitstream is validated against (see scripts/xsa_profile.sh for how a build
# selects <PROFILE>). Regenerate a header whenever its .profile changes:
#
#   scripts/profile_to_header.py <src.profile> <out/ActiveUseCase_profile.h>
#
# With no output path the header is written to stdout. The transform is
# whitespace-exact: round-tripping an unchanged .profile reproduces the
# committed header byte-for-byte.

import sys


def profile_to_header(src_text):
    """Return the C-header text for the given .profile JSON string."""
    lines = src_text.split("\n")
    out = []
    last = len(lines) - 1
    for i, line in enumerate(lines):
        esc = line.replace("\\", "\\\\").replace('"', '\\"')
        prefix = 'const char *json_profile_active_use_case = "' if i == 0 else ""
        # Every line becomes a continued C string line ("... \n\"); the final
        # line closes the literal instead ("...\n";").
        suffix = '\\n";' if i == last else " \\n\\"
        out.append(prefix + esc + suffix)
    return "\n".join(out) + "\n"


def main(argv):
    if not 2 <= len(argv) <= 3:
        sys.exit("usage: profile_to_header.py <src.profile> [out.h]")
    with open(argv[1], "r", newline="") as f:
        header = profile_to_header(f.read())
    if len(argv) == 3:
        with open(argv[2], "w", newline="") as f:
            f.write(header)
    else:
        sys.stdout.write(header)


if __name__ == "__main__":
    main(sys.argv)
