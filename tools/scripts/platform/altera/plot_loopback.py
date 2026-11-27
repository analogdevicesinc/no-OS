#!/usr/bin/env python3
"""Analyse and plot a DAC-to-ADC loopback capture taken with capture.tcl.

Reads capture_ch<N>.csv from a transmit and a receive directory, reports the tone
and the spur environment, and writes a four-panel figure.

  ./plot_loopback.py --tx dma_test/tx --rx dma_test/rx --out dma_test/zoom.png

The tone bin is found from the receive capture unless --bin says otherwise. The
spur table walks harmonics of the sample rate over --sysref-div: SYSREF is derived
from the converter clock by that divisor, so its harmonics land on multiples of
that bin and a tone placed on one of them cannot be told apart from leakage.
"""

import argparse
import os
import sys

import numpy as np

PALETTE = ['#2a78d6', '#eb6834', '#1baf7a', '#eda100',
           '#e87ba4', '#008300', '#4a3aa7', '#00a0b0']
INK, INK2, INK3, SURFACE = '#0b0b0b', '#52514e', '#8a8983', '#fcfcfb'


def load(directory, channels):
    out = []
    for ch in range(1, channels + 1):
        path = os.path.join(directory, 'capture_ch%d.csv' % ch)
        raw = np.loadtxt(path, dtype=np.int64)
        out.append(np.where(raw >= 32768, raw - 65536, raw).astype(float))
    return out


def spectrum(x, full_scale=32768.0):
    n = len(x)
    mag = np.abs(np.fft.rfft(x * np.hanning(n))) / (n / 4.0)
    return 20 * np.log10(np.maximum(mag, 1e-12) / full_scale)


def tone_stats(x, bin_):
    n = len(x)
    mag = np.abs(np.fft.rfft(x * np.hanning(n)))
    mag[0] = 0.0
    total = mag[1:] @ mag[1:]
    lo, hi = max(1, bin_ - 2), bin_ + 3
    peak = mag[lo:hi] @ mag[lo:hi]
    return peak / total, 10 * np.log10(peak / max(total - peak, 1e-30))


def phase_amp(x, bin_):
    n = len(x)
    z = np.sum(x * np.exp(-2j * np.pi * bin_ * np.arange(n) / n))
    return np.degrees(np.angle(z)), abs(z) / n * 2


def wrap180(deg):
    return ((deg + 180.0) % 360.0) - 180.0


def local_peak(db, bin_, radius=3):
    lo = max(0, bin_ - radius)
    j = int(np.argmax(db[lo:bin_ + radius + 1])) + lo
    return j, db[j]


def report(tx, rx, bin_, sysref_div):
    print('tone bin %d, %d samples/channel\n' % (bin_, len(rx[0])))
    for name, data in (('TX', tx), ('RX', rx)):
        for i, x in enumerate(data):
            frac, snr = tone_stats(x, bin_)
            print('  %s ch%d: min %7.0f max %7.0f rms %8.1f  '
                  'in-bin %8.4f%%  SNR %5.1f dB'
                  % (name, i + 1, x.min(), x.max(), x.std(), 100 * frac, snr))
        print()

    print('  envelope, rms per 2048 samples (RX ch1):')
    n = len(rx[0])
    print('   ', [round(float(rx[0][k:k + 2048].std()), 1)
                  for k in range(0, n, 2048)], '\n')

    db = spectrum(rx[0])
    _, tone_db = local_peak(db, bin_)
    print('  RX ch1 spurs, relative to the tone at %.1f dBFS:' % tone_db)
    for m in range(1, 9):
        k = sysref_div * m
        if k >= len(db):
            break
        j, v = local_peak(db, k)
        print('    %dx SYSREF  bin %5d  %8.1f dBFS  %+7.1f dBc'
              % (m, j, v, v - tone_db))
    for h in (2, 3, 5):
        if bin_ * h < len(db):
            j, v = local_peak(db, bin_ * h)
            print('    tone %dx    bin %5d  %8.1f dBFS  %+7.1f dBc'
                  % (h, j, v, v - tone_db))
    print('    noise floor (median)   %8.1f dBFS\n'
          % float(np.median(db)))

    print('  bin %d phase and gain, RX against TX:' % bin_)
    for i, (a, b) in enumerate(zip(tx, rx)):
        pt, at = phase_amp(a, bin_)
        pr, ar = phase_amp(b, bin_)
        print('    ch%d: dphi %+8.2f deg  gain %6.2f dB'
              % (i + 1, wrap180(pr - pt), 20 * np.log10(ar / at)))
    print('  quadrature, RX pairs:',
          [round(wrap180(phase_amp(rx[2 * p + 1], bin_)[0]
                         - phase_amp(rx[2 * p], bin_)[0]), 2)
           for p in range(len(rx) // 2)])


def style(ax, title, subtitle):
    ax.set_facecolor(SURFACE)
    for side in ('top', 'right'):
        ax.spines[side].set_visible(False)
    for side in ('left', 'bottom'):
        ax.spines[side].set_color(INK3)
        ax.spines[side].set_linewidth(0.8)
    ax.tick_params(colors=INK2, labelsize=9, length=3)
    ax.grid(True, color='#e6e5e0', lw=0.7, zorder=0)
    ax.set_axisbelow(True)
    ax.text(0, 1.16, title, transform=ax.transAxes, color=INK,
            fontsize=12, fontweight='bold', va='bottom')
    ax.text(0, 1.045, subtitle, transform=ax.transAxes, color=INK2,
            fontsize=9.5, va='bottom')


def plot(tx, rx, bin_, sysref_div, out, tx_addr, rx_addr):
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    n = len(rx[0])
    period = n / float(bin_)
    span = int(round(2 * period))
    db_rx = spectrum(rx[0])
    _, tone_db = local_peak(db_rx, bin_)
    gains = [20 * np.log10(phase_amp(b, bin_)[1] / phase_amp(a, bin_)[1])
             for a, b in zip(tx, rx)]
    snrs = [tone_stats(x, bin_)[1] for x in rx]

    fig = plt.figure(figsize=(13, 13), facecolor=SURFACE)
    gs = fig.add_gridspec(4, 1, height_ratios=[1, 1, 1, 1.1], hspace=0.62,
                          left=0.075, right=0.975, top=0.885, bottom=0.055)

    ax = fig.add_subplot(gs[0])
    style(ax, 'TX buffer%s — two periods' % tx_addr,
          '%d of %d samples · period %.1f samples (bin %d) · '
          'converters carrying the same pair overlay each other'
          % (span, n, period, bin_))
    for i, x in enumerate(tx):
        ax.plot(x[:span], color=PALETTE[i % 8], lw=1.4, label='ch%d' % (i + 1))
    ax.axhline(0, color=INK3, lw=0.8)
    ax.set_xlim(0, span - 1)
    ax.set_ylabel('LSB', color=INK2, fontsize=10)
    ax.set_xlabel('sample', color=INK2, fontsize=10)
    ax.legend(frameon=False, labelcolor=INK2, fontsize=8.5, ncol=8,
              loc='upper center', bbox_to_anchor=(0.5, 1.05),
              handlelength=1.3, columnspacing=1.1)

    ax = fig.add_subplot(gs[1])
    style(ax, 'RX buffer%s — same two periods' % rx_addr,
          'markers on every sample · gain %.1f to %.1f dB · SNR %.1f to %.1f dB'
          % (min(gains), max(gains), min(snrs), max(snrs)))
    for i, x in enumerate(rx):
        ax.plot(x[:span], '-o', color=PALETTE[i % 8], lw=1.3, ms=3.0)
    ax.axhline(0, color=INK3, lw=0.8)
    ax.set_xlim(0, span - 1)
    ax.set_ylabel('LSB', color=INK2, fontsize=10)
    ax.set_xlabel('sample', color=INK2, fontsize=10)

    ax = fig.add_subplot(gs[2])
    win = 2048
    blocks = [float(rx[0][k:k + win].std()) for k in range(0, n - win + 1, win)]
    style(ax, 'RX envelope over the whole record, converter 1',
          'rms per %d-sample window: %.1f to %.1f, a spread of %.2f%% — '
          'no gating and no dropouts'
          % (win, min(blocks), max(blocks),
             100 * (max(blocks) - min(blocks)) / max(blocks)))
    step = 64
    trimmed = rx[0][:(n // step) * step].reshape(-1, step)
    edge = np.arange(len(trimmed)) * step
    ax.fill_between(edge, trimmed.min(1), trimmed.max(1),
                    color=PALETTE[0], alpha=0.30, lw=0,
                    label='min/max per %d samples' % step)
    ax.plot(np.arange(len(blocks)) * win + win / 2, blocks,
            color=PALETTE[1], lw=1.6, marker='o', ms=4,
            label='rms per %d samples' % win)
    ax.axhline(0, color=INK3, lw=0.8)
    ax.set_xlim(0, n)
    ax.set_ylabel('LSB', color=INK2, fontsize=10)
    ax.set_xlabel('sample', color=INK2, fontsize=10)
    ax.legend(frameon=False, labelcolor=INK2, fontsize=9.5, ncol=2,
              loc='lower right')

    ax = fig.add_subplot(gs[3])
    style(ax, 'RX spectrum, all converters',
          'Hann, %d points · dashed: harmonics of the sample rate over %d, '
          'where SYSREF and its harmonics land' % (n, sysref_div))
    for i, x in enumerate(rx):
        ax.plot(spectrum(x), color=PALETTE[i % 8], lw=0.6, alpha=0.8)
    for m in range(1, len(db_rx) // sysref_div + 1):
        ax.axvline(sysref_div * m, color=INK3, lw=0.6, ls=(0, (3, 4)),
                   alpha=0.55, zorder=1)
    ax.axvline(bin_, color=INK, lw=1.0)
    ax.annotate('tone, bin %d\n%.1f dBFS' % (bin_, tone_db),
                xy=(bin_, tone_db), xytext=(bin_ + n / 12.0, tone_db + 14),
                color=INK, fontsize=10,
                arrowprops=dict(arrowstyle='-', color=INK3, lw=0.9))
    ax.set_xlim(0, len(db_rx) - 1)
    ax.set_ylim(-135, -3)
    ax.set_xlabel('FFT bin', color=INK2, fontsize=10)
    ax.set_ylabel('dBFS', color=INK2, fontsize=10)

    fig.text(0.075, 0.955, 'AD9084-EBZ / Agilex 5 — DAC to ADC loopback',
             color=INK, fontsize=15, fontweight='bold')
    fig.text(0.075, 0.926,
             'Tone on bin %d, %d bins clear of the nearest SYSREF harmonic. '
             'Continuous on all %d converters, phase-locked to the transmit buffer.'
             % (bin_, abs(bin_ - int(round(bin_ / float(sysref_div))) * sysref_div),
                len(rx)),
             color=INK2, fontsize=10.5)
    fig.savefig(out, dpi=125, facecolor=SURFACE)
    print('\nwrote %s' % out)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--tx', required=True)
    ap.add_argument('--rx', required=True)
    ap.add_argument('--out', default='zoom.png')
    ap.add_argument('--channels', type=int, default=8)
    ap.add_argument('--bin', type=int, default=None,
                    help='tone bin; taken from the receive capture if omitted')
    ap.add_argument('--sysref-div', type=int, default=128,
                    help='converter clock over SYSREF, default 128')
    ap.add_argument('--tx-addr', default='')
    ap.add_argument('--rx-addr', default='')
    args = ap.parse_args()

    tx = load(args.tx, args.channels)
    rx = load(args.rx, args.channels)
    if len(tx[0]) != len(rx[0]):
        sys.exit('capture lengths differ: tx %d, rx %d'
                 % (len(tx[0]), len(rx[0])))

    bin_ = args.bin
    if bin_ is None:
        mag = np.abs(np.fft.rfft(rx[0] * np.hanning(len(rx[0]))))
        mag[0] = 0.0
        bin_ = int(np.argmax(mag))

    report(tx, rx, bin_, args.sysref_div)
    plot(tx, rx, bin_, args.sysref_div, args.out,
         (' ' + args.tx_addr) if args.tx_addr else '',
         (' ' + args.rx_addr) if args.rx_addr else '')


if __name__ == '__main__':
    main()
