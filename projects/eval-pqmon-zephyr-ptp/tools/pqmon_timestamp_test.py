#!/usr/bin/env python3
"""
pqmon_timestamp_test.py — Validate PTP-stamped measurements across PQMON boards

Connects to one or two AD-PQMON-SL boards via libiio (TCP or USB) and verifies:
  1. PTP convergence status
  2. Measurement PTP timestamps advance and are sane
  3. Cross-board timestamp alignment (two-board mode)
  4. Event timestamps (dips/swells) correlation across boards
  5. Time-aligned RMS snapshot reconstruction

Usage:
  # Single board via TCP (T1L)
  python3 pqmon_timestamp_test.py --uri ip:192.0.2.1

  # Two boards via TCP
  python3 pqmon_timestamp_test.py --uri ip:192.0.2.1 --uri2 ip:192.0.2.2

  # Single board via USB
  python3 pqmon_timestamp_test.py --uri serial:/dev/ttyACM0,115200,8n1

  # Two boards: one USB, one TCP
  python3 pqmon_timestamp_test.py --uri serial:/dev/ttyACM0,115200,8n1 \
                                   --uri2 ip:192.0.2.2

  # Continuous monitoring (prints every --interval seconds)
  python3 pqmon_timestamp_test.py --uri ip:192.0.2.1 --uri2 ip:192.0.2.2 \
                                   --continuous --interval 1.0
"""

import argparse
import os
import sys
import time
from dataclasses import dataclass, field
from typing import Optional

try:
    import iio
except ImportError:
    print("ERROR: pylibiio not installed. Run: pip3 install pylibiio")
    sys.exit(1)

# Unbuffered stdout for continuous mode under pipes/timeout
sys.stdout.reconfigure(line_buffering=True)


# ── Helpers ──────────────────────────────────────────────────────────────

def parse_ptp_timestamp(s: str) -> Optional[float]:
    """Parse 'sec.nsec' string → float seconds, or None if invalid."""
    s = s.strip()
    if not s or s == "0.000000000":
        return None
    try:
        if '.' in s:
            parts = s.split('.', 1)
            sec = int(parts[0])
            nsec = int(parts[1])
            return sec + nsec / 1e9
        else:
            return float(s)
    except (ValueError, IndexError):
        return None


def ts_str(ts: Optional[float]) -> str:
    """Format timestamp for display."""
    if ts is None:
        return "(none)"
    sec = int(ts)
    nsec = int((ts - sec) * 1e9)
    return f"{sec}.{nsec:09d}"


@dataclass
class BoardSnapshot:
    """A single point-in-time capture from one board."""
    label: str
    capture_time: float = 0.0  # host wall time of read

    # PTP status
    ptp_state: str = ""
    ptp_offset_ns: int = 0
    ptp_mean_delay_ns: int = 0
    ptp_converged: bool = False

    # Measurement timestamps
    meas_ts: Optional[float] = None      # 1-cycle PTP timestamp
    meas_1012_ts: Optional[float] = None  # 10/12-cycle PTP timestamp
    event_time_base: str = ""

    # RMS values (V/I per phase)
    vrms: list = field(default_factory=lambda: [0.0, 0.0, 0.0])
    irms: list = field(default_factory=lambda: [0.0, 0.0, 0.0])

    # Event data
    dip_count: int = 0
    dip_start_times: str = ""
    dip_end_times: str = ""
    swell_count: int = 0
    swell_start_times: str = ""
    swell_end_times: str = ""
    intrpt_count: int = 0
    intrpt_start_times: str = ""
    intrpt_end_times: str = ""
    rvc_count: int = 0
    rvc_start_times: str = ""
    rvc_end_times: str = ""


def read_attr(dev, attr_name: str) -> str:
    """Read a device attribute with retry on timeout.

    Firmware holds pqm_data_mutex during PQLib processing (~variable ms).
    If our read arrives during processing, it blocks until the mutex is
    released.  The pylibiio serial timeout is ~1000ms.  We retry up to
    3 times with increasing backoff.
    """
    for attempt in range(3):
        try:
            return dev.attrs[attr_name].value
        except Exception as e:
            err = str(e)
            if attempt < 2 and ("timedout" in err or "-110" in err):
                time.sleep(0.1 * (attempt + 1))  # 100ms, 200ms
                continue
            return f"ERROR:{e}"
    return "ERROR:retry_exhausted"


def write_attr(dev, attr_name: str, value: str) -> bool:
    """Write a device attribute with retry on timeout."""
    for attempt in range(3):
        try:
            dev.attrs[attr_name].value = value
            return True
        except Exception as e:
            err = str(e)
            if attempt < 2 and ("timedout" in err or "-110" in err):
                time.sleep(0.1 * (attempt + 1))
                continue
            return False
    return False


def read_ch_attr(ch, attr_name: str) -> str:
    """Read a channel attribute with retry on timeout."""
    for attempt in range(3):
        try:
            return ch.attrs[attr_name].value
        except Exception as e:
            err = str(e)
            if attempt < 2 and ("timedout" in err or "-110" in err):
                time.sleep(0.1 * (attempt + 1))
                continue
            return f"ERROR:{e}"
    return "ERROR:retry_exhausted"


# Channel cache to avoid repeated lookups
_channel_cache = {}

def find_channel(dev, name: str):
    """Find channel by name (cached)."""
    key = (id(dev), name)
    if key not in _channel_cache:
        _channel_cache[key] = None
        for ch in dev.channels:
            if ch.name == name or ch.id == name:
                _channel_cache[key] = ch
                break
    return _channel_cache[key]


def capture_snapshot_light(ctx, label: str, read_events: bool = False) -> BoardSnapshot:
    """Light capture: 1-cycle ts + voltage RMS (+ events/1012 when requested)."""
    snap = BoardSnapshot(label=label)
    snap.capture_time = time.time()

    dev = ctx.find_device("pqm")
    if dev is None:
        print(f"  [{label}] ERROR: 'pqm' device not found")
        return snap

    # Always read: 1-cycle timestamp + 3x voltage RMS = 4 serial reads
    snap.meas_ts = parse_ptp_timestamp(read_attr(dev, "measurement_ptp_timestamp"))

    for i, name in enumerate(["ua", "ub", "uc"]):
        ch = find_channel(dev, name)
        if ch:
            try:
                snap.vrms[i] = float(read_ch_attr(ch, "rms"))
            except ValueError:
                pass

    # Extended reads: 10/12 timestamp + event counts (when requested)
    if read_events:
        snap.meas_1012_ts = parse_ptp_timestamp(
            read_attr(dev, "measurement_1012_ptp_timestamp"))

        for ch_name, count_attr, start_attr, end_attr in [
            ("dips", "dip_count", "dip_start_times", "dip_end_times"),
            ("swells", "swell_count", "swell_start_times", "swell_end_times"),
            ("intrpt", "intrpt_count", "intrpt_start_times", "intrpt_end_times"),
            ("rvc", "rvc_count", "rvc_start_times", "rvc_end_times"),
        ]:
            ch = find_channel(dev, ch_name)
            if ch:
                try:
                    count = int(read_ch_attr(ch, "countEvent"))
                    setattr(snap, count_attr, count)
                except ValueError:
                    pass
                if getattr(snap, count_attr) > 0:
                    setattr(snap, start_attr, read_ch_attr(ch, "startTime"))
                    setattr(snap, end_attr, read_ch_attr(ch, "endTime"))

    return snap


def capture_snapshot(ctx, label: str) -> BoardSnapshot:
    """Full capture: all attributes (slower over USB serial)."""
    snap = BoardSnapshot(label=label)
    snap.capture_time = time.time()

    dev = ctx.find_device("pqm")
    if dev is None:
        print(f"  [{label}] ERROR: 'pqm' device not found")
        return snap

    # PTP status
    snap.ptp_state = read_attr(dev, "ptp_state")
    try:
        snap.ptp_offset_ns = int(read_attr(dev, "ptp_offset_ns"))
    except ValueError:
        pass
    try:
        snap.ptp_mean_delay_ns = int(read_attr(dev, "ptp_mean_delay_ns"))
    except ValueError:
        pass
    snap.ptp_converged = read_attr(dev, "ptp_converged") == "1"

    # Measurement PTP timestamps
    snap.meas_ts = parse_ptp_timestamp(read_attr(dev, "measurement_ptp_timestamp"))
    snap.meas_1012_ts = parse_ptp_timestamp(
        read_attr(dev, "measurement_1012_ptp_timestamp"))
    snap.event_time_base = read_attr(dev, "event_time_base")

    # RMS values per phase
    phase_names_v = ["ua", "ub", "uc"]
    phase_names_i = ["ia", "ib", "ic"]
    for i, name in enumerate(phase_names_v):
        ch = find_channel(dev, name)
        if ch:
            try:
                snap.vrms[i] = float(read_ch_attr(ch, "rms"))
            except ValueError:
                pass
    for i, name in enumerate(phase_names_i):
        ch = find_channel(dev, name)
        if ch:
            try:
                snap.irms[i] = float(read_ch_attr(ch, "rms"))
            except ValueError:
                pass

    # Event data
    dips_ch = find_channel(dev, "dips")
    if dips_ch:
        try:
            snap.dip_count = int(read_ch_attr(dips_ch, "countEvent"))
        except ValueError:
            pass
        snap.dip_start_times = read_ch_attr(dips_ch, "startTime")
        snap.dip_end_times = read_ch_attr(dips_ch, "endTime")

    swells_ch = find_channel(dev, "swells")
    if swells_ch:
        try:
            snap.swell_count = int(read_ch_attr(swells_ch, "countEvent"))
        except ValueError:
            pass
        snap.swell_start_times = read_ch_attr(swells_ch, "startTime")
        snap.swell_end_times = read_ch_attr(swells_ch, "endTime")

    return snap


# ── Test functions ───────────────────────────────────────────────────────

def test_ptp_status(snap: BoardSnapshot) -> bool:
    """Check PTP convergence. Returns True if converged."""
    ok = True
    print(f"\n  [{snap.label}] PTP Status:")
    print(f"    state:        {snap.ptp_state}")
    print(f"    offset_ns:    {snap.ptp_offset_ns}")
    print(f"    mean_delay:   {snap.ptp_mean_delay_ns}")
    print(f"    converged:    {snap.ptp_converged}")

    if not snap.ptp_converged:
        # Master won't report converged (it IS the time source)
        if "TIME_TRANSMITTER" in snap.ptp_state.upper() or \
           "MASTER" in snap.ptp_state.upper():
            print(f"    → OK (time transmitter, offset N/A)")
        else:
            print(f"    → WARNING: not converged yet")
            ok = False
    else:
        if abs(snap.ptp_offset_ns) > 1000:
            print(f"    → WARNING: offset > 1μs ({snap.ptp_offset_ns}ns)")
            ok = False
        else:
            print(f"    → OK (offset within 1μs)")
    return ok


def test_timestamps_sane(snap: BoardSnapshot) -> bool:
    """Check that measurement timestamps look reasonable."""
    ok = True
    print(f"\n  [{snap.label}] Measurement Timestamps:")
    print(f"    1-cycle ts:     {ts_str(snap.meas_ts)}")
    print(f"    10/12-cycle ts: {ts_str(snap.meas_1012_ts)}")
    print(f"    event_time_base: {snap.event_time_base}")

    if snap.meas_ts is None:
        print(f"    → WARNING: no 1-cycle timestamp (AFE not running?)")
        ok = False
    elif snap.meas_ts < 1.0:
        print(f"    → WARNING: timestamp near zero (PTP not synced?)")
        ok = False
    else:
        print(f"    → OK (timestamp present)")

    if snap.meas_1012_ts is None:
        print(f"    → INFO: no 10/12-cycle timestamp yet (normal at startup)")
    elif snap.meas_1012_ts < 1.0:
        print(f"    → WARNING: 10/12-cycle timestamp near zero")
        ok = False

    return ok


def test_timestamps_advance(ctx, label: str, delay: float = 0.5) -> bool:
    """Read measurement timestamp twice and verify it advances."""
    print(f"\n  [{label}] Timestamp Advancement (delay={delay}s):")

    dev = ctx.find_device("pqm")
    if not dev:
        print(f"    ERROR: device not found")
        return False

    ts1 = parse_ptp_timestamp(read_attr(dev, "measurement_ptp_timestamp"))
    time.sleep(delay)
    ts2 = parse_ptp_timestamp(read_attr(dev, "measurement_ptp_timestamp"))

    print(f"    t1: {ts_str(ts1)}")
    print(f"    t2: {ts_str(ts2)}")

    if ts1 is None or ts2 is None:
        print(f"    → FAIL: one or both timestamps missing")
        return False

    delta = ts2 - ts1
    print(f"    delta: {delta*1000:.3f} ms")

    if delta <= 0:
        print(f"    → FAIL: timestamp did not advance!")
        return False

    # At 50Hz, 1-cycle = 20ms. After 0.5s, we expect ~25 cycles = ~500ms advancement
    # Allow wide tolerance since we don't know exact timing
    if delta < delay * 0.5:
        print(f"    → WARNING: advanced less than expected "
              f"({delta*1000:.1f}ms in {delay}s)")
    else:
        print(f"    → OK (timestamp advancing normally)")

    return True


def test_cross_board_alignment(snap1: BoardSnapshot, snap2: BoardSnapshot,
                                max_offset_ms: float = 150.0) -> bool:
    """Compare measurement timestamps between two boards.

    Note: over USB serial, sequential reads add ~80-120ms latency.
    The tolerance accounts for this — a real supervisory client over
    TCP (or parallel USB threads) would see sub-cycle alignment.
    """
    print(f"\n  Cross-Board Timestamp Alignment:")
    print(f"    [{snap1.label}] meas_ts: {ts_str(snap1.meas_ts)}")
    print(f"    [{snap2.label}] meas_ts: {ts_str(snap2.meas_ts)}")

    if snap1.meas_ts is None or snap2.meas_ts is None:
        print(f"    → SKIP: one or both timestamps missing")
        return False

    delta_ms = (snap1.meas_ts - snap2.meas_ts) * 1000
    print(f"    delta: {delta_ms:+.3f} ms")
    print(f"    (note: ~80-120ms is expected from sequential USB serial reads)")

    if abs(delta_ms) <= max_offset_ms:
        print(f"    → OK (within {max_offset_ms}ms tolerance for serial transport)")
        return True
    else:
        print(f"    → WARNING: offset > {max_offset_ms}ms "
              f"(exceeds serial read latency — possible clock issue)")
        return False


def test_cross_board_1012(snap1: BoardSnapshot, snap2: BoardSnapshot) -> bool:
    """Compare 10/12-cycle timestamps between two boards."""
    print(f"\n  Cross-Board 10/12-Cycle Alignment:")
    print(f"    [{snap1.label}] 1012_ts: {ts_str(snap1.meas_1012_ts)}")
    print(f"    [{snap2.label}] 1012_ts: {ts_str(snap2.meas_1012_ts)}")

    if snap1.meas_1012_ts is None or snap2.meas_1012_ts is None:
        print(f"    → SKIP: one or both timestamps missing")
        return False

    delta_ms = (snap1.meas_1012_ts - snap2.meas_1012_ts) * 1000
    print(f"    delta: {delta_ms:+.3f} ms")

    # 10/12-cycle period is 200/240ms; allow wider tolerance
    max_offset_ms = 250.0
    if abs(delta_ms) <= max_offset_ms:
        print(f"    → OK (within {max_offset_ms}ms)")
        return True
    else:
        print(f"    → WARNING: large offset")
        return False


def test_event_correlation(snap1: BoardSnapshot, snap2: BoardSnapshot):
    """Compare event timestamps across boards if events are present."""
    print(f"\n  Event Correlation:")
    print(f"    [{snap1.label}] dips={snap1.dip_count}, swells={snap1.swell_count}")
    print(f"    [{snap2.label}] dips={snap2.dip_count}, swells={snap2.swell_count}")

    if snap1.dip_count > 0 and snap2.dip_count > 0:
        print(f"\n    Dip Start Times (PQLib ms from PTP epoch):")
        print(f"      [{snap1.label}]: {snap1.dip_start_times}")
        print(f"      [{snap2.label}]: {snap2.dip_start_times}")
        # Parse and compare first event
        try:
            t1 = int(snap1.dip_start_times.split()[0])
            t2 = int(snap2.dip_start_times.split()[0])
            delta = t1 - t2
            print(f"      First dip delta: {delta:+d} ms")
            if abs(delta) < 100:
                print(f"      → CORRELATED (same grid event within 100ms)")
            else:
                print(f"      → Different events or propagation delay")
        except (ValueError, IndexError):
            print(f"      → Could not parse event times")
    elif snap1.dip_count > 0 or snap2.dip_count > 0:
        print(f"    → Dip on one board only (different grid points)")
    else:
        print(f"    → No dip events (normal operation)")

    if snap1.swell_count > 0 and snap2.swell_count > 0:
        print(f"\n    Swell Start Times (PQLib ms from PTP epoch):")
        print(f"      [{snap1.label}]: {snap1.swell_start_times}")
        print(f"      [{snap2.label}]: {snap2.swell_start_times}")
        try:
            t1 = int(snap1.swell_start_times.split()[0])
            t2 = int(snap2.swell_start_times.split()[0])
            delta = t1 - t2
            print(f"      First swell delta: {delta:+d} ms")
        except (ValueError, IndexError):
            print(f"      → Could not parse event times")


def print_rms_snapshot(snap: BoardSnapshot):
    """Print RMS values with timestamp."""
    print(f"    [{snap.label}] @ {ts_str(snap.meas_ts)}")
    print(f"      V_rms: A={snap.vrms[0]:.2f}  B={snap.vrms[1]:.2f}  "
          f"C={snap.vrms[2]:.2f}")
    print(f"      I_rms: A={snap.irms[0]:.2f}  B={snap.irms[1]:.2f}  "
          f"C={snap.irms[2]:.2f}")


def print_aligned_snapshot(snap1: BoardSnapshot, snap2: BoardSnapshot):
    """Print a time-aligned grid snapshot from two boards."""
    print(f"\n  Time-Aligned Grid Snapshot:")
    print_rms_snapshot(snap1)
    print_rms_snapshot(snap2)
    if snap1.meas_ts and snap2.meas_ts:
        delta_us = (snap1.meas_ts - snap2.meas_ts) * 1e6
        print(f"    Measurement time delta: {delta_us:+.1f} μs")


# ── Main ─────────────────────────────────────────────────────────────────

def run_tests(args):
    """Run all tests."""
    print(f"{'='*60}")
    print(f"PQMON PTP Timestamp Validation")
    print(f"{'='*60}")

    # Connect to board(s)
    print(f"\nConnecting to board 1: {args.uri}")
    try:
        ctx1 = iio.Context(args.uri)
        print(f"  → Connected: {ctx1.description}")
    except Exception as e:
        print(f"  → FAILED: {e}")
        return False

    ctx2 = None
    if args.uri2:
        print(f"Connecting to board 2: {args.uri2}")
        try:
            ctx2 = iio.Context(args.uri2)
            print(f"  → Connected: {ctx2.description}")
        except Exception as e:
            print(f"  → FAILED: {e}")
            ctx2 = None

    two_board = ctx2 is not None
    all_pass = True

    # ── Test 1: PTP Status ──
    print(f"\n{'─'*60}")
    print(f"TEST 1: PTP Convergence Check")
    print(f"{'─'*60}")

    snap1 = capture_snapshot(ctx1, "Board1" if two_board else "Board")
    if not test_ptp_status(snap1):
        all_pass = False

    if two_board:
        snap2 = capture_snapshot(ctx2, "Board2")
        if not test_ptp_status(snap2):
            all_pass = False

    # ── Test 2: Timestamp Sanity ──
    print(f"\n{'─'*60}")
    print(f"TEST 2: Timestamp Sanity")
    print(f"{'─'*60}")

    if not test_timestamps_sane(snap1):
        all_pass = False
    if two_board and not test_timestamps_sane(snap2):
        all_pass = False

    # ── Test 3: Timestamps Advance ──
    print(f"\n{'─'*60}")
    print(f"TEST 3: Timestamps Advance Over Time")
    print(f"{'─'*60}")

    if not test_timestamps_advance(ctx1, snap1.label):
        all_pass = False
    if two_board and not test_timestamps_advance(ctx2, snap2.label):
        all_pass = False

    # ── Test 4: Cross-Board Alignment (two-board only) ──
    if two_board:
        print(f"\n{'─'*60}")
        print(f"TEST 4: Cross-Board Measurement Alignment")
        print(f"{'─'*60}")

        # Re-capture both boards as close together as possible
        snap1 = capture_snapshot(ctx1, "Board1")
        snap2 = capture_snapshot(ctx2, "Board2")

        if not test_cross_board_alignment(snap1, snap2):
            all_pass = False
        test_cross_board_1012(snap1, snap2)

        # ── Test 5: Event Correlation ──
        print(f"\n{'─'*60}")
        print(f"TEST 5: Event Timestamp Correlation")
        print(f"{'─'*60}")
        test_event_correlation(snap1, snap2)

        # ── Test 6: Time-Aligned Snapshot ──
        print(f"\n{'─'*60}")
        print(f"TEST 6: Time-Aligned Grid Snapshot")
        print(f"{'─'*60}")
        print_aligned_snapshot(snap1, snap2)

    # ── Summary ──
    print(f"\n{'='*60}")
    if all_pass:
        print(f"RESULT: ALL TESTS PASSED")
    else:
        print(f"RESULT: SOME TESTS HAD WARNINGS (see above)")
    print(f"{'='*60}")

    return all_pass


def format_event_line(label1, snap1, label2, snap2):
    """Format event info as extra lines when events are detected."""
    lines = []
    has_events = False

    for snap, label in [(snap1, label1), (snap2, label2)]:
        parts = []
        if snap.dip_count > 0:
            parts.append(f"dips={snap.dip_count} start=[{snap.dip_start_times}]")
            has_events = True
        if snap.swell_count > 0:
            parts.append(f"swells={snap.swell_count} start=[{snap.swell_start_times}]")
            has_events = True
        if snap.intrpt_count > 0:
            parts.append(f"intrpt={snap.intrpt_count} start=[{snap.intrpt_start_times}]")
            has_events = True
        if snap.rvc_count > 0:
            parts.append(f"rvc={snap.rvc_count} start=[{snap.rvc_start_times}]")
            has_events = True
        if parts:
            lines.append(f"         │ {label} EVENTS: {', '.join(parts)}")

    # Cross-board event correlation
    if snap2 is not None:
        for etype, c1, s1, c2, s2 in [
            ("dip", snap1.dip_count, snap1.dip_start_times,
             snap2.dip_count, snap2.dip_start_times),
            ("swell", snap1.swell_count, snap1.swell_start_times,
             snap2.swell_count, snap2.swell_start_times),
            ("intrpt", snap1.intrpt_count, snap1.intrpt_start_times,
             snap2.intrpt_count, snap2.intrpt_start_times),
        ]:
            if c1 > 0 and c2 > 0:
                try:
                    t1 = int(s1.split()[0])
                    t2 = int(s2.split()[0])
                    lines.append(f"         │ ↳ {etype} cross-board Δ: {t1-t2:+d} ms")
                except (ValueError, IndexError):
                    pass

    return lines if has_events else []


def capture_interleaved(dev1, dev2, tier: str):
    """Interleaved capture from two boards for accurate cross-board correlation.

    Reads the same attribute from B1 then B2 back-to-back (~20ms gap) instead
    of reading all B1 attrs then all B2 attrs (~180ms gap).

    Tiers control how many reads per iteration:
      "light"  — 1-cycle ts + Vrms_A per board (4 reads total)
      "medium" — + 10/12-cycle ts + Vrms_B,C per board (10 reads total)
      "heavy"  — + event counts per board (18 reads max)
    """
    snap1 = BoardSnapshot(label="B1")
    snap2 = BoardSnapshot(label="B2")
    now = time.time()
    snap1.capture_time = now
    snap2.capture_time = now

    # USB CDC/ACM inter-read pause to prevent buffer overflow
    PAUSE = 0.02  # 20ms between attribute groups

    # ── 1-cycle timestamps: B1 → B2 (gap = 1 read ≈ 20ms) ──
    snap1.meas_ts = parse_ptp_timestamp(
        read_attr(dev1, "measurement_ptp_timestamp"))
    snap2.meas_ts = parse_ptp_timestamp(
        read_attr(dev2, "measurement_ptp_timestamp"))

    # ── 10/12-cycle timestamps: B1 → B2 (gap = 1 read ≈ 20ms) ──
    if tier in ("medium", "heavy"):
        time.sleep(PAUSE)
        snap1.meas_1012_ts = parse_ptp_timestamp(
            read_attr(dev1, "measurement_1012_ptp_timestamp"))
        snap2.meas_1012_ts = parse_ptp_timestamp(
            read_attr(dev2, "measurement_1012_ptp_timestamp"))

    # ── Voltage RMS phase A: B1 → B2 ──
    time.sleep(PAUSE)
    ch1_ua = find_channel(dev1, "ua")
    ch2_ua = find_channel(dev2, "ua")
    if ch1_ua:
        try:
            snap1.vrms[0] = float(read_ch_attr(ch1_ua, "rms"))
        except ValueError:
            pass
    if ch2_ua:
        try:
            snap2.vrms[0] = float(read_ch_attr(ch2_ua, "rms"))
        except ValueError:
            pass

    # ── Voltage RMS phases B,C (medium+) ──
    if tier in ("medium", "heavy"):
        time.sleep(PAUSE)
        for i, name in enumerate(["ub", "uc"], 1):
            ch1 = find_channel(dev1, name)
            ch2 = find_channel(dev2, name)
            if ch1:
                try:
                    snap1.vrms[i] = float(read_ch_attr(ch1, "rms"))
                except ValueError:
                    pass
            if ch2:
                try:
                    snap2.vrms[i] = float(read_ch_attr(ch2, "rms"))
                except ValueError:
                    pass

    # ── Event counts (heavy only) ──
    if tier == "heavy":
        time.sleep(PAUSE)
        for ch_name, count_attr, start_attr, end_attr in [
            ("dips", "dip_count", "dip_start_times", "dip_end_times"),
            ("swells", "swell_count", "swell_start_times", "swell_end_times"),
            ("intrpt", "intrpt_count", "intrpt_start_times", "intrpt_end_times"),
            ("rvc", "rvc_count", "rvc_start_times", "rvc_end_times"),
        ]:
            # Read counts interleaved: B1 → B2
            for dev, snap in [(dev1, snap1), (dev2, snap2)]:
                ch = find_channel(dev, ch_name)
                if ch:
                    try:
                        count = int(read_ch_attr(ch, "countEvent"))
                        setattr(snap, count_attr, count)
                    except ValueError:
                        pass

        # Read start/end times only for events that exist
        time.sleep(PAUSE)
        for dev, snap in [(dev1, snap1), (dev2, snap2)]:
            for ch_name, count_attr, start_attr, end_attr in [
                ("dips", "dip_count", "dip_start_times", "dip_end_times"),
                ("swells", "swell_count", "swell_start_times", "swell_end_times"),
                ("intrpt", "intrpt_count", "intrpt_start_times", "intrpt_end_times"),
                ("rvc", "rvc_count", "rvc_start_times", "rvc_end_times"),
            ]:
                if getattr(snap, count_attr) > 0:
                    ch = find_channel(dev, ch_name)
                    if ch:
                        setattr(snap, start_attr, read_ch_attr(ch, "startTime"))
                        setattr(snap, end_attr, read_ch_attr(ch, "endTime"))

    return snap1, snap2


def run_continuous(args):
    """Continuous monitoring mode — interleaved timestamps + Vrms + events."""
    print(f"PQMON Continuous Timestamp Monitor (Ctrl+C to stop)")
    print(f"Interval: {args.interval}s")
    print(f"Read tiers: light=every iter, medium=every 5th, heavy=every 20th\n")

    # Suppress libiio's noisy stderr messages (sp_blocking_read_next timeouts)
    # These are handled by retry logic in read_attr/read_ch_attr
    saved_stderr = os.dup(2)
    devnull = os.open(os.devnull, os.O_WRONLY)
    os.dup2(devnull, 2)

    try:
        ctx1 = iio.Context(args.uri)
    except Exception as e:
        os.dup2(saved_stderr, 2)
        print(f"ERROR connecting to board 1: {e}")
        return
    try:
        ctx2 = iio.Context(args.uri2) if args.uri2 else None
    except Exception as e:
        os.dup2(saved_stderr, 2)
        print(f"ERROR connecting to board 2: {e}")
        return
    two_board = ctx2 is not None

    # Clear stale events from boot (interruptions detected during power-up
    # before PTP convergence would have wrong timestamps)
    dev1 = ctx1.find_device("pqm")
    if dev1 and write_attr(dev1, "clear_events", "1"):
        print("Cleared stale events on board 1")
    if two_board:
        dev2 = ctx2.find_device("pqm")
        if dev2 and write_attr(dev2, "clear_events", "1"):
            print("Cleared stale events on board 2")

    # CSV logging
    csv_file = None
    if args.csv:
        csv_file = open(args.csv, "w")
        if two_board:
            csv_file.write("time_s,d1cyc_ms,d1012_ms,"
                           "b1_va,b1_vb,b1_vc,b2_va,b2_vb,b2_vc,"
                           "b1_dips,b1_swells,b1_intrpt,b1_rvc,"
                           "b2_dips,b2_swells,b2_intrpt,b2_rvc\n")
        else:
            csv_file.write("time_s,ts_1cyc,ts_1012,va,vb,vc,"
                           "dips,swells,intrpt,rvc\n")
        print(f"CSV logging to: {args.csv}")

    # Print header
    if two_board:
        # C=cycle match: = means same grid cycle, ~ means adjacent cycle
        print(f"{'Time':>8} │ {'Δ1cyc(ms)':>10} │C│ {'Δ1012(ms)':>10} │C│ "
              f"{'B1 Va':>7} │ {'B2 Va':>7} │ "
              f"{'Ev':>12}")
        print(f"{'─'*8}─┼─{'─'*10}─┼─┼─{'─'*10}─┼─┼─"
              f"{'─'*7}─┼─{'─'*7}─┼─"
              f"{'─'*12}")
    else:
        print(f"{'Time':>8} │ {'1cyc_ts':>20} │ {'1012_ts':>20} │ "
              f"{'Va':>7} │ {'Vb':>7} │ {'Vc':>7} │ {'Ev':>3}")
        print(f"{'─'*8}─┼─{'─'*20}─┼─{'─'*20}─┼─"
              f"{'─'*7}─┼─{'─'*7}─┼─{'─'*7}─┼─{'─'*3}")

    t0 = time.time()
    iteration = 0
    MEDIUM_INTERVAL = 5    # Read 1012 ts + Vrms B,C every 5th iteration
    HEAVY_INTERVAL = args.event_interval  # Read event counts every Nth iteration
    # Carry forward last known Vrms B,C values for display on light iterations
    last_vrms = {"b1": [0.0, 0.0, 0.0], "b2": [0.0, 0.0, 0.0]}

    try:
        while True:
            elapsed = time.time() - t0

            # Determine read tier
            if iteration % HEAVY_INTERVAL == 0:
                tier = "heavy"
            elif iteration % MEDIUM_INTERVAL == 0:
                tier = "medium"
            else:
                tier = "light"

            if two_board:
                dev1 = ctx1.find_device("pqm")
                dev2 = ctx2.find_device("pqm")
                if not dev1 or not dev2:
                    print(f"{elapsed:>8.1f} │ ERROR: pqm device not found")
                    iteration += 1
                    time.sleep(args.interval)
                    continue

                snap1, snap2 = capture_interleaved(dev1, dev2, tier)

                # Carry forward Vrms B,C from medium/heavy reads
                if tier != "light":
                    last_vrms["b1"] = list(snap1.vrms)
                    last_vrms["b2"] = list(snap2.vrms)
                else:
                    # Phase A is always fresh; B,C from last medium read
                    snap1.vrms[1] = last_vrms["b1"][1]
                    snap1.vrms[2] = last_vrms["b1"][2]
                    snap2.vrms[1] = last_vrms["b2"][1]
                    snap2.vrms[2] = last_vrms["b2"][2]

                delta_1c = ""
                cycle_match = ""
                if snap1.meas_ts and snap2.meas_ts:
                    d = (snap1.meas_ts - snap2.meas_ts) * 1000
                    delta_1c = f"{d:+.3f}"
                    # At 50Hz, one cycle = 20ms. If |delta| < 10ms,
                    # both reads caught the same grid cycle.
                    cycle_match = "=" if abs(d) < 10 else "~"

                delta_1012 = ""
                cycle_1012 = ""
                if tier != "light" and snap1.meas_1012_ts and snap2.meas_1012_ts:
                    d = (snap1.meas_1012_ts - snap2.meas_1012_ts) * 1000
                    delta_1012 = f"{d:+.3f}"
                    # At 50Hz, 10/12-cycle = 200/240ms. If |delta| < 100ms,
                    # both reads caught the same 10/12-cycle window.
                    cycle_1012 = "=" if abs(d) < 100 else "~"

                total_ev = (snap1.dip_count + snap1.swell_count +
                            snap1.intrpt_count + snap1.rvc_count +
                            snap2.dip_count + snap2.swell_count +
                            snap2.intrpt_count + snap2.rvc_count)
                ev_flag = str(total_ev) if total_ev > 0 else ""

                # Build event summary: show counts when available
                if tier == "heavy" or HEAVY_INTERVAL <= 2:
                    ev_str = (f"d{snap1.dip_count}+{snap2.dip_count} "
                              f"i{snap1.intrpt_count}+{snap2.intrpt_count} "
                              f"s{snap1.swell_count}+{snap2.swell_count}")
                else:
                    ev_str = ev_flag

                print(f"{elapsed:>8.1f} │ "
                      f"{delta_1c:>10} │{cycle_match}│ "
                      f"{delta_1012:>10} │{cycle_1012}│ "
                      f"{snap1.vrms[0]:>7.2f} │ "
                      f"{snap2.vrms[0]:>7.2f} │ "
                      f"{ev_str}")

                # Print event details when events are detected
                if tier == "heavy":
                    ev_lines = format_event_line("B1", snap1, "B2", snap2)
                    for line in ev_lines:
                        print(line)

                # CSV logging
                if csv_file:
                    d1c = (snap1.meas_ts - snap2.meas_ts) * 1000 if snap1.meas_ts and snap2.meas_ts else ""
                    d1012 = ""
                    if tier != "light" and snap1.meas_1012_ts and snap2.meas_1012_ts:
                        d1012 = (snap1.meas_1012_ts - snap2.meas_1012_ts) * 1000
                    csv_file.write(f"{elapsed:.3f},{d1c},{d1012},"
                                   f"{snap1.vrms[0]:.2f},{snap1.vrms[1]:.2f},{snap1.vrms[2]:.2f},"
                                   f"{snap2.vrms[0]:.2f},{snap2.vrms[1]:.2f},{snap2.vrms[2]:.2f},"
                                   f"{snap1.dip_count},{snap1.swell_count},"
                                   f"{snap1.intrpt_count},{snap1.rvc_count},"
                                   f"{snap2.dip_count},{snap2.swell_count},"
                                   f"{snap2.intrpt_count},{snap2.rvc_count}\n")
                    csv_file.flush()

            else:
                # Single-board mode
                snap1 = capture_snapshot_light(ctx1, "B1",
                                               read_events=(tier == "heavy"))
                if tier != "light":
                    dev1 = ctx1.find_device("pqm")
                    if dev1:
                        snap1.meas_1012_ts = parse_ptp_timestamp(
                            read_attr(dev1, "measurement_1012_ptp_timestamp"))

                total_ev = (snap1.dip_count + snap1.swell_count +
                            snap1.intrpt_count + snap1.rvc_count)
                ev_flag = str(total_ev) if total_ev > 0 else ""
                ts_1012 = ts_str(snap1.meas_1012_ts) if tier != "light" else ""

                print(f"{elapsed:>8.1f} │ "
                      f"{ts_str(snap1.meas_ts):>20} │ "
                      f"{ts_1012:>20} │ "
                      f"{snap1.vrms[0]:>7.2f} │ "
                      f"{snap1.vrms[1]:>7.2f} │ "
                      f"{snap1.vrms[2]:>7.2f} │ "
                      f"{ev_flag:>3}")

                if tier == "heavy":
                    ev_lines = format_event_line("B1", snap1, None,
                                                  BoardSnapshot(label=""))
                    for line in ev_lines:
                        print(line)

                if csv_file:
                    ts1c = snap1.meas_ts if snap1.meas_ts else ""
                    ts1012 = snap1.meas_1012_ts if snap1.meas_1012_ts else ""
                    csv_file.write(f"{elapsed:.3f},{ts1c},{ts1012},"
                                   f"{snap1.vrms[0]:.2f},{snap1.vrms[1]:.2f},{snap1.vrms[2]:.2f},"
                                   f"{snap1.dip_count},{snap1.swell_count},"
                                   f"{snap1.intrpt_count},{snap1.rvc_count}\n")
                    csv_file.flush()

            iteration += 1
            time.sleep(args.interval)
    except KeyboardInterrupt:
        print(f"\nStopped after {iteration} iterations.")
        if csv_file:
            csv_file.close()
            print(f"CSV saved to: {args.csv}")
    finally:
        # Restore stderr
        os.dup2(saved_stderr, 2)
        os.close(devnull)
        os.close(saved_stderr)


def main():
    parser = argparse.ArgumentParser(
        description="PQMON PTP timestamp validation and cross-board correlation")
    parser.add_argument("--uri", required=True,
                        help="Board 1 IIO URI (e.g. ip:192.0.2.1 or "
                             "serial:/dev/ttyACM0,115200,8n1)")
    parser.add_argument("--uri2", default=None,
                        help="Board 2 IIO URI (optional, enables cross-board tests)")
    parser.add_argument("--continuous", action="store_true",
                        help="Continuous monitoring mode")
    parser.add_argument("--interval", type=float, default=1.0,
                        help="Polling interval in seconds (continuous mode)")
    parser.add_argument("--csv", default=None,
                        help="CSV output file for logging (continuous mode)")
    parser.add_argument("--event-interval", type=int, default=20,
                        help="Read event counts every N iterations (default 20, "
                             "use 1 for event testing)")
    args = parser.parse_args()

    if args.continuous:
        run_continuous(args)
    else:
        run_tests(args)


if __name__ == "__main__":
    main()
