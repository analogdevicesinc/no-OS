#!/usr/bin/env bash
# Run the ad9088 host-simulation examples and compare their console output
# with the recorded baselines in scripts/golden/.
#
#   scripts/sim_check.sh [--update] [build-root]
#
# build-root defaults to build-sim at the no-OS root; each example is taken
# from <build-root>/ad9088-<example>-native-i386/build/ad9088.elf (configure
# and build them as described in README.rst, "Host simulation"). Examples
# that are not built are skipped.
#
# The simulation models' "SIM:" lines are dropped, and DMA buffer addresses
# (host pointers, which change from run to run) and the no-OS checkout path
# are masked; everything else must match line for line. dma_example parks in
# while (1) after its capture and iio_example serves IIOD until stopped, so
# both are stopped once their last expected line is printed. --update
# rewrites the baselines.
#
# Exit status: 0 when every built example matches, 1 otherwise.

set -u

here=$(cd "$(dirname "$0")" && pwd)
golden_dir=$here/golden
update=0
if [ "${1:-}" = "--update" ]; then
	update=1
	shift
fi
noos=$(cd "$here/../../.." && pwd)
root=${1:-$noos/build-sim}

# example  stop-after-this-line (empty: run to exit)  timeout [s]
examples=(
	"basic_example||120"
	"dma_example|DMA_EXAMPLE Rx side 0:|60"
	"iio_example|IIO: rx |60"
)

# Drop model lines; mask DMA buffer addresses and the checkout path (driver
# messages print __FILE__), so baselines compare on any machine.
normalize() {
	tr -d '\r' < "$1" | grep -v '^SIM:' |
		sed -E -e 's/address=0x[0-9a-fA-F]+/address=<addr>/g' \
		       -e "s#$noos/#<no-OS>/#g"
}

# Run $1, stopping it once a line matching $2 appears (or at exit / timeout).
run_example() {
	local elf=$1 stop=$2 secs=$3 log=$4 pid i

	"$elf" > "$log" 2>&1 &
	pid=$!
	for ((i = 0; i < secs * 10; i++)); do
		if ! kill -0 "$pid" 2> /dev/null; then
			wait "$pid"
			return $?
		fi
		if [ -n "$stop" ] && grep -q -- "$stop" "$log"; then
			sleep 0.2
			kill "$pid" 2> /dev/null
			wait "$pid" 2> /dev/null
			return 0
		fi
		sleep 0.1
	done
	kill "$pid" 2> /dev/null
	wait "$pid" 2> /dev/null
	echo "TIMEOUT after ${secs} s" >> "$log"
	return 124
}

status=0
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
mkdir -p "$golden_dir"

for entry in "${examples[@]}"; do
	IFS='|' read -r name stop secs <<< "$entry"
	elf=$root/ad9088-$name-native-i386/build/ad9088.elf
	golden=$golden_dir/$name.txt

	if [ ! -x "$elf" ]; then
		echo "SKIP:  $name (not built: $elf)"
		continue
	fi

	run_example "$elf" "$stop" "$secs" "$tmp/$name.log"
	rc=$?
	normalize "$tmp/$name.log" > "$tmp/$name.txt"

	if [ "$rc" -ne 0 ]; then
		echo "FAIL:  $name (exit status $rc)"
		tail -n 5 "$tmp/$name.txt" | sed 's/^/       /'
		status=1
		continue
	fi

	if [ "$update" -eq 1 ]; then
		cp "$tmp/$name.txt" "$golden"
		echo "SAVED: $name ($(wc -l < "$golden") lines)"
	elif [ ! -f "$golden" ]; then
		echo "FAIL:  $name (no baseline $golden; run with --update)"
		status=1
	elif diff -u "$golden" "$tmp/$name.txt" > "$tmp/$name.diff"; then
		echo "MATCH: $name"
	else
		echo "DIFF:  $name"
		head -n 40 "$tmp/$name.diff" | sed 's/^/       /'
		status=1
	fi
done

exit $status
