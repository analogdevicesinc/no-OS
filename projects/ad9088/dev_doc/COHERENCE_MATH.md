# Coherence estimator — verifying a captured tone is the expected tone

How the AD9088 DMA example decides whether a captured buffer contains *the tone
we asked for*, rather than merely *a signal*. Covers the datapath background, the
derivation, worked examples, the integer implementation, and the VCU118
measurements that confirm each prediction.

Implementation: `projects/ad9088/src/examples/dma/dma_example.c`.
Code is referenced by function and identifier name rather than line number, since
line numbers drift with every edit. Last reconciled against the file 2026-08-07.

---

## 0. Glossary

| term | meaning |
|---|---|
| **CDDC / FDDC** | Coarse / Fine Digital **Down** Converter — RX blocks. Mix by an NCO and **decimate**. 2 CDDCs, 4 FDDCs per side. |
| **CDUC / FDUC** | Coarse / Fine Digital **Up** Converter — TX blocks. Mix by an NCO and **interpolate**. |
| **CNCO / FNCO** | The Coarse / Fine NCO *inside* those blocks — a digital oscillator generating the complex phasor the mixer multiplies by. Programmed via an FTW, not directly in Hz. |
| **`c`** | The real constant from `MIXER_TEST_MODE_VAL`, injected at the mixer in test mode. Sets amplitude only. |
| **`drc_ratio`** | **D**ata **R**ate **C**onversion ratio — decimation on RX, interpolation on TX. |
| **`z[n]`** | One complex captured sample, `I[n] + jQ[n]`. |
| **`r[n]`** | The unit-magnitude reference phasor, `exp(-j*w_hat*n)`. |
| **`X`** | A single complex number: the correlation sum over the whole capture. |
| **`N`** | Complex samples per capture = samples per converter. **16384** here. |
| **`Fs_cap`** | Sample rate of the data in DDR. **2.5 GHz** here. See §9. |
| **bin** | One resolvable frequency slot — a *rate of rotation*, not a place in the data. Width `Fs/N` = 152.6 kHz here, and there are exactly `N` of them tiling the whole band. See §7. |
| **`step`** | The 32-bit phase increment per sample. This is what selects the bin; `sin_table_q15` is only the lookup. See §7. |

FNCO FTW is 48-bit, CNCO 32-bit. Test-mode value limits: FNCO `0x1FFF` (RX) /
`0x7FFF` (TX), CNCO `0x7FF` (RX) / `0x1FFF` (TX)
(`ad9088_runtime.c:61,107`).

## 1. Why peak-to-peak was not enough

The first version of the example judged a capture by its peak-to-peak amplitude
against an idle noise floor. That gave the RX tone path 65x over the floor (a
clear pass) and the external loopback 1.34x (a clear fail), and then could say
nothing more. Peak-to-peak is `max - min`: one scalar, no frequency content. A
tone at the wrong frequency, a swapped I/Q pair, or a wrong assumption about the
sample rate would all have passed silently.

Two further problems made the loopback result unresolvable:

- A p-p ratio cannot distinguish "no signal" from "signal 30 dB down".
- `ad9088_get_fnco_freq()` (`drivers/rf-transceiver/apollo/ad9088.c:289-300`)
  reads `nco->nco_phase_inc` back out of the cached profile struct — the value
  software just wrote. It is **not** a hardware readback, so nothing in the port
  had yet proven the FTW registers take effect.

What follows replaces amplitude with a frequency-selective measurement, and turns
the NCO tone into the first real test of the FTW write path.

Peak-to-peak has since been **removed from the example entirely**, along with the
clip detection that rode along in the same scan. Coherence subsumed the amplitude
question — `dma_example_coherence()` returns a mean envelope in LSB via its
`amplitude` out-param, which is an RMS figure over the whole capture rather than a
two-sample extreme a single glitch can set. The p-p values that remained were
printed but never gated, so they were decoration rather than test.

## 2. Datapath background

### Where the signal becomes complex

```
RX:  ADC ──> CDDC ──> FDDC ──> JESD204 framer  ──> FPGA ──> DMA ──> DDR
             ↑CNCO    ↑FNCO
             real->    complex
             complex

TX:  DDR ──> DMA ──> FPGA ──> JESD204 deframer ──> FDUC ──> CDUC ──> DAC
                                                   ↑FNCO    ↑CNCO
                                                   complex   complex->real
```

The ADC produces one **real** stream. The CDDC mixer multiplies it by a complex
phasor, and from that point on the signal is complex — I and Q as two parallel
buses. There is **no separate "split into I and Q" stage**: the mixer makes it
complex, and the pair travels together thereafter.

Note the coarse block always sits nearest the converter, where the rate is
highest, and the fine block nearest the JESD link. Two-stage conversion exists
because doing it in one step would need an impractically long filter. One CDDC
feeds two FDDCs, which is why `ad9088_set_fnco_freq()` derives the CDDC index
from the FDDC index as `cddc_pi = (fddc_num / 2) % ADI_APOLLO_CDUCS_PER_SIDE`
(`ad9088.c:223`).

### Where complex becomes two converters

JESD204 has no notion of "complex". Its transport layer knows only **M
converters**, each carrying real samples. So the framer maps the complex pair
onto two adjacent **virtual converters** via `conv_xbar_sel[]`
(`apollo_cpu_device_profile_types.h:2160`), selecting from `VCONV_0..VCONV_15`. That crossbar mapping — not a DSP operation — is the actual
"split".

This is why `M=4` carries only **two** complex channels: FDDC0's I/Q on
conv0/conv1, another FDDC's on conv2/conv3. And why the estimator recombines in
software:

```c
z[n] = buf[n*M + TONE_CONV_I] + j*buf[n*M + TONE_CONV_Q]
```

**Which converter carries I is a configuration choice, not a law.**
`dma_example.c` hardcodes `TONE_CONV_I 0` / `TONE_CONV_Q 1`. If the
crossbar had them reversed the tone would appear to rotate backwards —
indistinguishable from a genuine spectral inversion. The sweep's sign check
verifies the crossbar mapping and the sign convention simultaneously.

### The TX mirror

TX is the mirror image, with the collapse to real happening at the **CDUC** — the
last block before the DAC, which has one physical output and can only accept a
real stream. The FDUC, nearest the link, is still fully complex.

The collapse is a **real mix**, not a merge of two streams:

```
out[n] = Re{ (I[n] + jQ[n]) * exp(+j*w*n) } = I[n]*cos(w*n) - Q[n]*sin(w*n)
```

The code names this explicitly: `drc_mxr_sel` selects `ADI_APOLLO_DRC_MIXER_REAL`
or `ADI_APOLLO_DRC_MIXER_COMPLEX` (`apollo_cpu_device_profile_types.h:661-664`),
written via `BF_DRC_MIXER_SEL` (`adi_apollo_cnco.c:191`), a per-NCO profile field plumbed identically for both terminals (`adi_apollo_tx.c:148,229`,
`adi_apollo_rx.c:141,225`).

Taking a real part discards the imaginary half, so the spectrum becomes
conjugate-symmetric — **a real mix produces both sidebands**, at `±f_nco`.

**Not verified:** that CDUC is configured `MIXER_REAL` and FDUC `MIXER_COMPLEX`
in this profile. Both are settable per-NCO on either terminal and the values come
from the profile blob. Physics requires the collapse somewhere before the DAC and
the CDUC is the only candidate positioned for it, but this is an inference.
`adi_apollo_cnco.c:245` and `adi_apollo_fnco.c:322` are `bf_get` readbacks of
exactly this field via the `*_inspect` calls — two SPI reads would settle it, and
it matters for §11.

These blocks handle *digital* frequency translation only. Any analog mixing to an
RF carrier is external. The FNCO test tone is injected inside the digital RX
datapath, downstream of the ADC, so it validates nothing about the analog front
end — closing that gap is what the loopback leg is for.

## 3. What the hardware emits in test mode

`ADI_APOLLO_MXR_TEST_MODE` (`apollo_cpu_device_profile_types.h:601-605`) is a
*mixer mode*, not a flag. In test mode the mixer's input is replaced by the
constant in `MIXER_TEST_MODE_VAL`, and the NCO still rotates it:

```
z[n] = c * exp(-j*w*n),     w = 2*pi*f_nco / Fs
```

### Multiplication, not addition

A mixer has two inputs and one output. The NCO phasor is never a datapath signal
in its own right — it only ever appears as a multiplicand:

```
normal:   out[n] = in[n] * exp(-j*w*n)      in[n] is ADC data via the CDDC
test:     out[n] = c     * exp(-j*w*n)      c is MIXER_TEST_MODE_VAL
```

So `c` does not offset an existing tone. The tone **is** `c * phasor`. Two
independent knobs: `c` sets amplitude (a real scalar), the NCO sets frequency.
Multiplying a constant by a unit-magnitude rotating phasor yields a
constant-envelope tone.

The measured data rules out the additive reading. An added offset with an idle
ADC would give DC at 4096 riding on noise: mean ~4096, p-p ~352. Measured on
conv0:

| point | min | max | mean | p-p |
|---|---|---|---|---|
| Fs/512 | -11523 | +11522 | **0** | 23045 |
| Fs/64 | -11510 | +11509 | **0** | 23019 |
| Fs/8 | -10648 | +10647 | **0** | 21295 |
| -Fs/64 | -11510 | +11509 | **0** | 23019 |

`mean = 0` exactly at every point, symmetric about zero, p-p = 2x amplitude. No
DC term anywhere, and amplitude scales with `c`: 11519/4096 = 2.81.

**Open question — replaced or summed?** The data proves the output is
`(something) * phasor` rather than `something + DC`. It does not distinguish:

```
(a) input replaced:  out = c * exp(-j*w*n)
(b) input summed:    out = (in + c) * exp(-j*w*n)
```

With only noise at the ADC both give mean 0 and amplitude proportional to `c`. An
earlier working note asserted test mode *discards* its input — hence the claim
that the tone and loopback captures were mutually exclusive by design. That claim
is **assumed, not demonstrated**. Settle it during the loopback leg: drive TX and
capture once with the tone on and once off, at well-separated frequencies. Two
distinct tones present means summed (the tone can stay on as an in-band marker);
only the test tone means replaced (it must be off for loopback measurements).

### Measured phase progression

Programmed `f = Fs/8`, so `w` = 45 deg/sample:

| n | I | Q | phase | step |
|---|---|---|---|---|
| 0 | +4412 | +10645 | +67.50 deg | |
| 1 | +10640 | +4407 | +22.50 deg | -45.00 |
| 2 | +10641 | -4407 | -22.50 deg | -45.00 |
| 3 | +4407 | -10642 | -67.50 deg | -45.00 |
| 4 | -4407 | -10641 | -112.50 deg | -45.00 |
| 5 | -10641 | -4407 | -157.50 deg | -45.00 |
| 6 | -10641 | +4408 | +157.50 deg | -45.00 |
| 7 | -4407 | +10641 | +112.50 deg | -45.00 |

Exactly -45.00 deg/sample, envelope constant at 11517-11523. Not "a signal is
present" — the predicted waveform, sample for sample. Neither converter alone is
"the tone"; each is a projection. Only the *pair* has a phase, and only the pair
can distinguish +f from -f.

### NCO frequency vs output frequency

The relationship is **1:1 with a sign flip**: `f_out = -f_nco` on RX. No scaling,
no offset, no harmonics.

Because the constant contributes no frequency of its own — a real scalar has zero
phase progression — all time variation comes from the phasor. This is not special
to test mode: normal operation gives `f_out = f_in - f_nco`, and test mode is that
same equation with `f_in = 0`.

| programmed | expected | measured |
|---|---|---|
| +Fs/512 = +4.882812 MHz | -4.882812 MHz | coh 1.000 |
| +Fs/64 = +39.0625 MHz | -39.0625 MHz | coh 1.000 |
| +Fs/8 = +312.5 MHz | -312.5 MHz | **-45.00 deg/sample** |
| -Fs/64 = -39.0625 MHz | +39.0625 MHz | coh 1.000 |

Precision: FNCO FTW is 48-bit (`calc_nco_ftw(..., bits=48)`, `ad9088.c:241`),
step `Fs/2^48` ~ 8.9 uHz, relative error 1.8e-12. Integer truncation of the sweep
point itself (`Fs/512` -> 4882812 not 4882812.5, `dma_example.c`) is 1.0e-07.
Coherence only degrades as relative error approaches `1/N` = 6.1e-05, so both sit
orders of magnitude below the threshold of relevance.

Caveats: on TX the FDUC upconverts, so `f_out = +f_nco` — and if the CDUC is
`MIXER_REAL`, energy appears at **both** `+-f_nco`. Cascaded NCOs add: with a
nonzero CNCO the output is at `-(f_cnco + f_fnco)`, since the mixers are in
series. The clamp in `calc_nco_ftw()` (`ad9088.c:44,49-50`) limits each
individually, not the sum.

## 4. The measurement: normalised projection

"Is this the tone we asked for" is a question about **direction**, not size. Treat
the capture as a vector in C^N and take the normalised inner product with the
reference `r[n] = exp(-j*w_hat*n)`:

```
                |sum_n z[n] r[n]|^2          |X|^2
    coherence = -------------------- = ----------------
                 N * sum_n |z[n]|^2      N * power
```

This is Cauchy-Schwarz normalised to [0, 1]: it equals 1 **iff** `z` is a scalar
multiple of `r`, and nothing else. The `N` appears because `||r||^2 = N`.

### What X is

**`X` is a single complex number** — the correlation sum, condensing all N
samples into one value. In the code it is the pair `acc_re` / `acc_im`
(`dma_example.c`).

- `|X|` — how strongly the capture matches the probed frequency.
- `arg X` — the tone's starting phase. Computed but never used; only `|X|^2` is
  needed, which is why there is no `atan2` anywhere.

`X` is **one bin of a DFT** — the bin at `f_hat`, and *only* that bin. A full
16384-point DFT would compute all of them; this computes only the one predicted,
Goertzel-style. One bin costs N complex MACs against `N*log2(N)` ~ 14x more for an
FFT whose other 16383 bins would be discarded.

The sum runs over all N samples, but that is summing over **time**, not over
frequency — see §7 for why one iteration is one sample rather than one bin. If `X`
covered the whole band the ratio would always be 1 and say nothing.

### The two accumulators differ fundamentally

Both traverse the same N samples in the same loop. The asymmetry is that one
multiplies by a rotating phasor first:

| | `acc_re`/`acc_im` -> `X` | `power` |
|---|---|---|
| loop range | all N samples | all N samples |
| multiplied by | `dma_example_cos_q15(...)` | nothing |
| frequency-selective? | **yes** — one bin | **no** — all bins |
| how it accumulates | **coherently** — signed, phases can cancel | **incoherently** — always positive, nothing cancels |
| grows like | N (matched) or sqrt(N) (noise) | N always |
| result covers | energy at *one* frequency | energy at *all* frequencies |

That asymmetry is the entire mechanism. `power` cannot distinguish signal from
noise — every sample adds something. `X` can, because a mismatched signal's terms
point in different directions and destroy each other. The 42 dB processing gain
is exactly this gap: N versus sqrt(N).

The identical loop range is what makes the pair look symmetric at first glance. It
is the phasor multiplication, not the range, that band-limits `X` to a single bin.

## 5. Understanding the normalisation

### Why raw correlation is useless alone

`|X|` grows with things we do not care about:

| | raw \|X\| | coherence |
|---|---|---|
| amp=100, N=4 | 400 | 1.0000 |
| amp=200, N=4 | 800 | 1.0000 |
| amp=100, N=16 | 1600 | 1.0000 |

Same *quality* of match in all three, but `|X|` varies 4x. No threshold can be
set on that. Normalising divides out both amplitude and capture length.

### Worked example, N=4, tone at Fs/4

The phasor cycles `1, j, -1, -j` (90 deg/sample), so this is checkable on paper.

```
signal  z[n] = 100*exp(+j90n)  ->   100,  100j,  -100,  -100j
probe   r[n] =     exp(-j90n)  ->     1,    -j,    -1,     j
```

Term by term:

```
    n=0:  ( 100 +   0j) * ( 1 + 0j)  =  +100
    n=1:  (   0 + 100j) * ( 0 - 1j)  =  +100
    n=2:  (-100 +   0j) * (-1 + 0j)  =  +100
    n=3:  (   0 - 100j) * ( 0 + 1j)  =  +100
                                        -----
                                   X =  +400
```

**This is the whole trick.** The probe is the conjugate of the signal, so each
product's phase cancels to zero and all four terms come out `+100`, pointing the
same direction. They **add**: 4 x 100 = 400.

```
    numerator     |X|^2      = 400^2      = 160000
    power         sum|z[n]|^2 = 4 * 100^2 =  40000
    normaliser    N * power  = 4 * 40000  = 160000

    coherence = 160000 / 160000 = 1.0000
```

### Why `N * power` is the right divisor

It is the **theoretical maximum** the numerator could reach. If all N terms
align:

```
    |X|   <= N * A
    |X|^2 <= N^2 * A^2
```

and for a constant-envelope signal where `power = N * A^2`:

```
    N * power = N * (N * A^2) = N^2 * A^2      <- the same bound
```

So the achieved value is divided by the best possible value, confining the result
to [0, 1] and reaching 1 only when the signal *is* the probe.

### Nothing about the transmitted signal enters

A common misreading is that the denominator represents "the signal we sent". It
does not. **Both numbers come from the received samples.** The only thing supplied
is a *frequency* — no amplitude, no reference power. `r[n]` has `|r[n]| = 1` at
every sample; it is pure phase.

| received amplitude | \|X\| | N*power | coherence |
|---|---|---|---|
| 10 | 640 | 409600 | 1.0000 |
| 100 | 6400 | 40960000 | 1.0000 |
| 11519 | 737216 | 5.4e11 | 1.0000 |

Numerator and denominator both scale by A^2, so it cancels exactly. That is why
a tone 30 dB down still scored 862 — the estimator never needs to know how strong
the tone should be. This is what makes the primitive reusable across the tone
path, the loopback, and any amplitude, with the expected frequency as its only
per-case input.

### The denominator is the sum of all DFT bins

By **Parseval's theorem**:

```
    sum_n |z[n]|^2 = (1/N) sum_k |X_k|^2   =>   N * power = sum_k |X_k|^2
```

Verified numerically on a two-tone signal (N=64, tones at bins 8 and 20):

```
    time domain     sum|z[n]|^2       =    742400
    freq domain     (1/N) sum|X_k|^2  =    742400      <- equal

    our denominator N * power         =  47513600
    sum over all bins  sum|X_k|^2     =  47513600      <- identical
```

So the ratio is exactly **one bin's energy over all bins' energy**:

| bin | energy | fraction |
|---|---|---|
| 8 | 40960000 | **0.8621** |
| 20 | 6553600 | 0.1379 |
| all others | 0 | 0.0000 |

and coherence at bin 8 computes to 0.8621 — the fraction directly.

**But it is computed in the time domain**, with no transform and no frequency
involved (`dma_example.c`):

```c
env = (uint64_t)((int64_t)s_i * s_i + (int64_t)s_q * s_q);   /* |z[n]|^2 */
power += env;                                               /* running total */
```

Two lines, and that is the whole power calculation: square I, square Q, add,
accumulate. No table lookup, no use of `phase`, no knowledge of which frequency
was probed. Geometrically it is `|z[n]|^2`, the squared magnitude of each complex
sample, so `sum |z[n]|^2` is the total energy in the capture.

That is the practical payoff. Getting "fraction of energy at one frequency" would
normally need every bin — a full FFT. Parseval supplies the denominator from N
time-domain squares instead. Numerator: one bin. Denominator: the total of all
bins, obtained without leaving the time domain. Both accumulators are filled in a
single pass over the buffer, with `env` reused for the power sum and
the envelope statistics — which is why it is a named local rather
than inlined.

Types matter here, since this is where the earlier overflow bug lived. `s_i`/`s_q`
are `int32_t` holding sign-extended 16-bit samples — the `(int16_t)` cast on the
buffer read is what makes `0xd708` read as -11512 rather than 55048. The products
are promoted to `int64_t` explicitly: `11519^2` ~ 1.3e8 would fit in `int32_t`, but
the accumulation over 16384 samples would not. `power` is `uint64_t`, worst case
`16384 * 2 * 32768^2` ~ 3.5e13 — comfortable. `power` was never the overflow risk;
`X` was (§10).

### Three cases with identical total power

Only the *distribution* differs:

| received | \|X\|^2 at bin 8 | N*power | coh |
|---|---|---|---|
| pure tone at bin 8 | 40960000 | 40960000 | **1.0000** |
| pure tone at bin 20 | ~0 | 40960000 | **0.0000** |
| half at each | 20473815 | 40947630 | **0.5000** |

Row 2 is the key one: the same energy as row 1, but none of it at the probed
frequency. The denominator cannot tell them apart; the numerator can.

## 6. Why a wrong frequency collapses

Substitute a tone at `w` while probing `w_hat`, and let `D = w - w_hat`. The sum
becomes a geometric series:

```
    sum_{n=0}^{N-1} exp(j*D*n) = (1 - exp(j*D*N)) / (1 - exp(j*D))

                     1   ( sin(N*D/2) )^2
    => coherence = ----- ( ---------- )
                    N^2  (  sin(D/2)  )
```

The Dirichlet kernel. At `D = 0` it is `N^2/N^2 = 1`. Off-frequency the terms
rotate to different phases and **cancel** instead of accumulating.

Quantitative, against synthetic data at N = 16384:

| probe error | predicted (per-mille) | measured |
|---|---|---|
| 0 % | 1000 | 999 |
| 0.1 % | 802 | 801 |
| 1 % | 14.9 | 14 |
| 100 % | 0 | 0 |

Agreement to one part in 800 on the 0.1% row.

## 7. Bins

The single most confusing part of this estimator, so it is built up from scratch
here. Four misconceptions are worth clearing first, because each one blocks the
next step:

1. The capture is **not** "around the NCO frequency" — it spans the whole band.
2. Bins are **not** windows placed around something — they tile the entire band.
3. A bin index is **not** a count of tiles — it identifies one tile.
4. One loop iteration is **not** one bin — it is one sample in time.

### It starts with the time window

Nothing about a bin comes from the NCO. It comes from the fact that the capture
spans a **finite window of time**:

```
    window = N / Fs = 16384 / 2.5 GHz = 6.554 us
```

Within a 6.554 us window, which frequencies can be told apart at all? Only those
completing a **whole number of cycles** in that window. A whole number of cycles
closes on itself over the window; a fractional count does not, so it cannot be
represented by any single one of them and spreads across neighbours instead.

Tiny example — N = 8, Fs = 8 Hz, window = 1 second:

| k | cycles in window | frequency `k*Fs/N` |
|---|---|---|
| 0 | 0 | 0 Hz |
| 1 | 1 | 1 Hz |
| 2 | 2 | 2 Hz |
| ... | ... | ... |
| 7 | 7 | 7 Hz |

Eight samples, eight distinguishable frequencies, spaced 1 Hz apart — and 1 Hz is
exactly **one cycle per window**. So:

```
    bin width = 1 cycle per window = 1 / (N/Fs) = Fs/N
```

`Fs/N` is not a separate formula. It is "one cycle per window" rearranged. For the
real capture: `1 / 6.554 us` = **152.6 kHz**.

### The capture spans the whole band

The FDDC already downconverted to baseband, so what lands in DDR is a wideband
snapshot: the tone, the noise, and anything else present — across
`-Fs/2 .. +Fs/2` = **-1.25 .. +1.25 GHz**. The NCO decided *where in that band*
the tone sits, not what the capture covers.

### Bins tile the band

They are a complete partition — end to end, no gaps:

```
  bin index:  -8192 ......... -256 ....... 0 ....... +256 ......... +8191
  frequency:  -1.25GHz ..... -39MHz ..... DC ..... +39MHz ....... +1.25GHz
              |<------- 16384 bins, 152.6 kHz each ------->|

  16384 x 152.6 kHz = 2.5 GHz = Fs        <- exactly the full band
```

Every bin exists whether or not anything is in it. In this capture bin -256 holds
the tone and the other 16383 hold noise, roughly 1/16384 of the total energy each
— which is precisely why the noise-floor coherence is `1000/N + 1` ~ 1 per-mille.
Not an empirical fudge, a direct consequence of the tiling.

### Number of bins = N

```
    number of bins = total span / bin width = Fs / (Fs/N) = N
```

Check: `2.5 GHz / 152.6 kHz = 16384`. It is the information argument again — N
numbers in, N numbers out. A transform cannot create or destroy information.

Only two of the three quantities are independent:

| quantity | value here | formula |
|---|---|---|
| number of bins | 16384 | **N** |
| bin width | 152.6 kHz | `Fs/N` |
| total span | 2.5 GHz | `Fs` |

So there is a fixed trade: **more bins means narrower bins, never a wider span.**
Doubling N to 32768 halves the bin width to 76.3 kHz over the *same* 2.5 GHz.

A note on "baseband": if that means the positive half only, it is `N/2` = 8192
bins over DC..1.25 GHz. That framing suits a **real** signal, whose spectrum is
conjugate-symmetric so the negative half is redundant. This capture is **complex**
(I/Q), so positive and negative frequencies carry independent information and all
N bins are distinct. That is exactly why bin -256 is a real, separate tile from
bin +256, and why the sign error of §8 was detectable at all.

### A bin index is an index, and also a cycle count

Bin 256 is **one tile** — the one 256 slots up from DC, centred at
`256 x 152.6 kHz` = 39.0625 MHz. There are 16384 tiles in total; 256 says which.

The index and the cycle count are the same number by construction:

```
    k = f*N/Fs = f * (N/Fs) = f * window = cycles in the window
```

So for the tone at `Fs/64`: tile number 256 counting from DC, **and** 256 whole
cycles inside the 6.554 us window. Both readings give 256. That is not a
coincidence — tile *k* is *defined* as "the frequency doing k cycles per window".

Bin 0 is DC, so indices start at zero like array indices — which is literally what
they are in an FFT output. Negative indices are real tiles; in an unsigned FFT
convention bin -256 appears as bin `16384 - 256 = 16128`.

### How bins relate to the time domain

This is the inversion that makes the loop make sense. **One iteration is one
sample in time, not one bin.** The loop never visits a second bin:

```c
for (i = 0; i < samples; i++)     /* i indexes TIME, not frequency */
```

`step` is computed once *before* the loop (`dma_example.c`) from one
frequency, and every iteration uses that same value. Summing over samples is not
summing over frequencies.

Traced for N = 16 probing bin 3, the reference phase advances a fixed
`360*3/16` = 67.5 deg per iteration:

| iter | time | sample (I,Q) | ref phase | running acc_re |
|---|---|---|---|---|
| 0 | 0/Fs | (1000, 0) | 0.0 deg | 1000.0 |
| 1 | 1/Fs | (383, 924) | 67.5 deg | 2000.2 |
| 2 | 2/Fs | (-707, 707) | 135.0 deg | 3000.1 |
| 3 | 3/Fs | (-924, -383) | 202.5 deg | 4000.3 |
| ... | ... | ... | ... | ... |
| 15 | 15/Fs | (383, -924) | 292.5 deg | **16001.3** |

Sixteen iterations consumed sixteen **samples**, still only bin 3. `acc_re` climbs
by ~1000 each time — every sample adding constructively into the same bin's
accumulator. (Bin count and sample count both being 16 here is a coincidence of N,
and part of what invites the confusion.)

**So a bin is a rate of rotation applied while walking through time**, not a place
in the data:

| bin | phase step per sample | meaning |
|---|---|---|
| 0 | 0 deg | DC — the reference does not rotate |
| 3 (example above) | 67.5 deg | 3 cycles over the window |
| 256 (this capture) | 5.625 deg | 256 cycles over the 6.554 us window |

67.5 deg x 16 samples = 1080 deg = **3 full turns** — the reference completes
exactly `k` rotations while traversing the capture, which is the same statement as
"bin k does k cycles in the window", now visible as accumulating phase.

Running the same loop with a different `step` gives a different bin. For a tone at
bin 3, N=16:

| bin k | \|X\| |
|---|---|
| 0 | 0.0 |
| 1 | 0.0 |
| 2 | 0.0 |
| **3** | **16001.3** |
| 4 | 0.0 |
| 5 | 0.0 |

Each row summed over all 16 samples; each row is one bin. A full DFT is all 16
rows — an outer loop over `k` wrapping this one, `N^2` operations. The estimator
runs the inner loop once, at the single `k` predicted from the NCO frequency.

Which means: **it is not "each sample tells you about one bin". Every sample
contributes to every bin, and all N samples are needed to determine any single
one.** Bin 3's value depends on all 16 samples; drop one and `X` changes. That is
why a longer capture gives finer bins — more evidence about each frequency, so
rotation rates that are more similar can still be separated. At N=16 only
rotations differing by 22.5 deg/sample are distinguishable; at N=16384, by
0.022 deg/sample.

### The bin lives in `step`, not in the table

```c
step   = f * 2^32 / Fs                       /* selects the bin */
phase += step                                /* walks the rotation */
idx    = phase >> SIN_TABLE_PHASE_SHIFT      /* 23: top 9 bits of the accumulator */
ref_re = dma_example_cos_q15(idx)            /* reads it out */
```

`sin_table_q15` is a fixed 512-entry sine, identical bytes whatever bin is
probed. What changes is **how fast it is traversed**:

| bin | step | traversal |
|---|---|---|
| 0 (DC) | 0 | stuck on entry 0 |
| 256 (this capture) | `2^32/64` | 8 entries per sample, wraps every 64 |
| N/2 (Nyquist) | `2^31` | 256 entries per sample — alternating +-1 |

One table, one `step` per bin: the table is the machinery, `step` is the choice.
This is why 512 entries suffice for 16384 bins — the table stores one cycle of
sine, and any bin is reached by stepping through it at the right rate. The
32-bit accumulator offers far more distinguishable rates than the capture can
resolve; only the top 9 bits index the table.

The same number in five costumes, for the tone at Point 1:

| form | value |
|---|---|
| frequency | 39.0625 MHz |
| `step` | `2^32/64` = 67108864 |
| phase rate | 5.625 deg/sample |
| bin index | 256 |
| cycles in window | 256 |

It also explains why the sign fix was a single character: negating the frequency
changes only `step`, which wraps to the top half of the accumulator and rotates
the reference backwards — probing bin -256 instead of +256. Same table, same loop.

### Resolution consequences

Two tones closer than one bin cannot be separated by any processing — in 6.554 us
they have not drifted far enough apart in phase. The only fix is a longer capture.
The reciprocal relationship:

| N | window | bin width |
|---|---|---|
| 16 | 6.4 ns | 156 MHz |
| 16384 | 6.554 us | 152.6 kHz |

This is exactly why the 16-sample re-scoring in §8 behaved oddly: at N=16 the bin
width is 156 MHz, so +-39 MHz fall inside the same bin and the wrong sign cannot
be rejected. At N=16384 they are 512 bins apart and rejection is total.

It is also why the sweep uses exact power-of-two divisions of `Fs`: those land on
integer bins (32, 256, 2048), where all the energy sits in one bin and coherence
reaches 1.0. Falling between bin centres spreads energy across neighbours — the
Dirichlet kernel of §6, and why a 1% error scores 14/1000. Bins are *centred* on
`k*Fs/N` and do not have hard walls; the tiling is exact only for tones landing on
centres.

### Where N comes from

`N` is `samples_per_conv` (`dma_example.c`), clamped to the static buffer
but not limited here: `16384 * 4 = 65536` entries against a buffer of
`16384 * 8 = 131072`. The loop reads two entries per iteration, so **N counts I/Q
pairs, not buffer entries** — 32768 of the 65536 captured values, with conv2/conv3
feeding the idle-converter check.

Three figures quoted elsewhere are just N in different clothing: bin width `Fs/N`
= 152.6 kHz, processing gain `10*log10(N)` = 42.1 dB, and the noise-floor
coherence `1000/N + 1` ~ 1 per-mille.

## 8. The sign convention — the one that bit us

`r[n]` and its conjugate are *orthogonal*. Probing `+w` against a tone at `-w`
gives `D = -2w`, far off-frequency, so coherence goes to 0.

An RX FDDC downconverts: it multiplies by `exp(-jwn)` so a signal at `+f_nco`
lands at DC. The test constant is injected *ahead* of that mixer, so it emerges
rotating at **`-f_nco`**. The first implementation probed at `+f_nco` and scored 0
on all four sweep points **while the datapath was working perfectly**.

Re-scoring the captured samples both ways:

| point | programmed | coh(+f) | coh(-f) |
|---|---|---|---|
| 0 | +Fs/512 | 0.987 | **1.000** |
| 1 | +Fs/64 | 0.407 | **1.000** |
| 2 | +Fs/8 | 0.000 | **1.000** |
| 3 | -Fs/64 | 0.407 | **1.000** |

The fix negates at the call site (`dma_example_tone_point()`,
`dma_example.c`) rather than inside the estimator, because a **TX FDUC
upconverts and needs the opposite sign**. The floor reference is negated to match.

**Note on those coh(+f) values.** They come from re-scoring only the **16 samples
printed per converter**, i.e. N = 16, where +-f sit 0.06 / 0.5 / 4 bins apart and
the Dirichlet prediction is exactly 0.987 / 0.407 / 0.000. At the full N = 16384
those frequencies are 64 / 512 / 4096 bins apart and the wrong sign would have
scored **0.000 everywhere**. An earlier draft attributed the 0.987 to
insufficient bin separation while quoting the N=16384 bin counts — right cause,
wrong N. The practical lesson: the 16-sample dump is fine for eyeballing a
waveform but useless for frequency discrimination; anything quantitative needs
the full buffer.

The sensitivity is a feature — it is what lets the negative sweep point catch a
swapped I/Q pair or a spectrally inverted datapath, which any magnitude-only
measure is blind to.

## 9. Sample rate

Every predicted frequency depends on `Fs`, so it is derived rather than assumed
(`dma_example.c`):

```
    Fs_cap = adc_sampling_rate_Hz / (cddc_dcm * fddc_dcm)
```

using `adi_apollo_cddc_dcm_bf_to_val()` (`adi_apollo_cddc.h:107`) and
`adi_apollo_fddc_dcm_bf_to_val()` (`adi_apollo_fddc.h:95`) on the `drc_ratio`
fields the JESD204 FSM populated.

**Three distinct rates exist** and must not be conflated:

| rate | meaning |
|---|---|
| `adc_sampling_rate_Hz` | the ADC's own rate, from the profile |
| `adc_rate / cddc_dcm` | the rate the **FNCO mixes at** (CDDC output) |
| `adc_rate / (cddc_dcm * fddc_dcm)` | the rate of the samples **in DDR** — what the estimator needs |

The FNCO mixes at the CDDC output — *not* also divided by `fddc_dcm` — as visible
in `ad9088_get_fnco_freq()` (`ad9088.c:289-300`) and corroborated by the clamp in
`adi_ad9088_calc_nco_ftw()` (`ad9088.c:44,49-50`). The FDDC decimates after the
mixer, so the *captured* rate carries both ratios.

### What determines it in this project

**Root cause: the profile blob.** `usecase.bin` is byte-identical (md5
`541fec25a164947396810d19ebebf3ee`) to **`204C_M4_L8_NP16_20p0_4x2.bin`**, linked
into the ELF as a binary symbol and parsed at `ad9088_dt.c:81-83`. It sets
`adc_sampling_rate_Hz` and the `drc_ratio` fields. The name is the spec: 204C,
M=4 converters, L=8 lanes, NP=16 bits, 20 Gbps class, 4x2 arrangement.

Link arithmetic, which makes 2.5 GHz consistent:

```
    20.625 Gbps x 8 lanes         = 165 Gbps
    x 64/66 (JESD204C encoding)   = 160 Gbps payload
    / (4 converters x 16 bits)    = 2.5 GSps per converter    <- exact
```

`20.625 Gbps` is `AD9088_LANE_RATE_KHZ` (`common_data.h:68`).

Physical clock tree:

| stage | value | where |
|---|---|---|
| Reference | 125 MHz | `common_data.c:88` |
| ADF4382 -> device clock | **20 GHz** | `common_data.c:89` |
| HMC7044 VCXO / PLL2 | 125 MHz / 2.5 GHz | `common_data.c:134-136` |
| SERDES reference | 312.5 MHz | `common_data.c:196,207` |

312.5 MHz x 66 = 20.625 Gbps, closing the loop. The 20 GHz device clock also
appears literally in the blob (offset 23 and elsewhere), so blob and clock config
agree.

### Cross-check and caveat

Checked against `rx_adc->clock_hz`, computed by `axi_adc_init_finish()` from the
TPL core's own `CLK_FREQ * CLK_RATIO` registers
(`drivers/axi_core/axi_adc_core/axi_adc_core.c:621-624`) — an independent
measurement. Measured: TPL 2499963 kHz vs profile-derived 2500000 kHz, 0.0015%
apart (a real PLL offset), so the model is right and the 5% warning correctly
stayed silent. This cross-check is only available because the example calls plain
`axi_adc_init()`; the earlier `axi_adc_init_begin()` fallback skipped
`init_finish()` and left `clock_hz` at 0.

**`Fs_cap = 2.5 GHz` holds only because decimation is 1:1 here**, which is
consistent with the link arithmetic working out exactly. All three rates above
collapse to one number in this configuration. Under a profile with real
decimation they diverge, and using the wrong one scales every predicted frequency
by an integer factor — hence deriving it at runtime rather than hardcoding. The
`Rates:` line the example prints is the authoritative statement of the actual
ratios.

## 10. Integer implementation

Two constraints: no floating point (the apollo driver uses none, and MicroBlaze
would pull in soft-float), and no overflow.

### Reference phasor

A 512-entry signed Q15 sine table (`sin_table_q15`) with a 32-bit phase
accumulator, `step = f * 2^32 / Fs`. Three conveniences:

- **Negative frequencies need no special case.** `-w` wraps to the top half of
  the accumulator, which *is* the conjugate reference.
- **One table serves both parts.** `cos(x) = sin(x + 90deg)`, so
  `dma_example_cos_q15(idx)` is `dma_example_sin_q15(idx + SIN_TABLE_QUARTER)` —
  one add, no second table. The imaginary part is `-dma_example_sin_q15(idx)`,
  negated explicitly for the conjugate reference `exp(-jwn)`.
- **The table is signed**, so a lookup is a mask and an index. (no-OS ships a
  shared `no_os_sine_lut_16[512]` in `util/no_os_sin_lut.c`, but it is offset
  binary — `0x8000` at the zero crossings — and has no header, so every consumer
  needs a bare `extern` plus an offset subtraction. The local table avoids both.)

The table is *sampled* rather than iterated as a rotation: a recurrence
accumulates rounding error over 16384 samples, a lookup cannot drift. Phase
quantisation of `2*pi/512` is well below the 5e-5 of coherence the previous
256-entry table cost — negligible against a 0.9 threshold either way. At the
`Fs/512` test tone the accumulator advances **exactly one table entry per
sample**, so the reference is read at its exact values with no interpolation.

### Overflow: divide before squaring

`|X|` reaches `N*A = 2^29` for full-scale A, so `|X|^2` needs 58 bits and
`|X|^2 * 1000` overflows `int64_t`. The fix is algebraic:

```
      |X|^2         |X/N|^2
    --------- =  -------------
     N * P          P / N
```

Both sides become per-sample quantities bounded by full scale: `|X/N| <= 2^15`,
so `mag <= 2^31` and `mag * 1000 <= 2^41` — comfortably inside `int64_t`. Same
value, overflow impossible (`dma_example.c`, via `no_os_div_s64()` /
`no_os_div_u64()`). On the N=4 example: `|X/N| = 100`, `P/N = 10000`,
`100^2/10000 = 1.0` — identical.

Output is scaled by `COHERENCE_SCALE = 1000` (per-mille) with
`COHERENCE_PASS = 900`.

The first version divided by N once instead of twice *and* overflowed, returning
4.2e9 instead of ~1000. Worth noting the failure mode: discrimination was already
correct (wrong cases ~30, right cases ~4.2e9) — only the calibration was broken.
A bug that preserves ordering but destroys scale survives a plausibility glance,
which is why the estimator was validated against synthetic tones on the host
before being trusted on hardware.

`dma_example_sqrt64()` is a restoring integer square root, used only for
the reported amplitude and envelope spread, not in the coherence path.

## 11. Supporting checks and their calibration

### Amplitude

`amp` is `sqrt(mean(I^2 + Q^2))` — RMS of the envelope. For a complex tone the
envelope *is* the peak, so RMS equals peak with no sqrt(2) factor.

Predicted `|c| = offset * 2*sqrt(2)` = 11585 for `offset = 0x1000`; measured
11519, 0.6% off. **Caveat: the `2*sqrt(2)` is an empirical fit** from an earlier
measurement, not traced through the datapath's gain stages. Consistent across two
runs, but treat it as a calibration constant rather than a derivation. The
printed "expected" value is 11583 because `2828/1000` approximates `2*sqrt(2)` in
integers — two LSB on a diagnostic print.

### Envelope spread

Any single complex tone has constant `I^2 + Q^2`, so spread is a shape check
independent of frequency. But it is a min/max **extreme-value** statistic, so
noise dominates it:

| noise | envelope spread | coherence |
|---|---|---|
| 176 LSB (measured floor) | 41 | 999 |
| 350 LSB | 80 | 999 |
| 700 LSB | 154 | 997 |

An originally planned limit of 50 would have produced false failures.
`ENVELOPE_SPREAD_MAX` is therefore 250 — loose enough to catch a grossly
non-constant envelope, while coherence does the strict work.

### Peak-to-peak and clip detection: removed

Both are gone from the example. The p-p values (tone pair, idle converters, floor)
were printed but never gated, so they contributed nothing to the verdict, and the
`amplitude` out-param already answers the level question with an RMS figure rather
than a two-sample extreme. Point 2 of the old sweep showed why p-p was the wrong
primitive: it read 21295 against 23019 elsewhere, ratio 0.9251, purely because at
`Fs/8` only 8 samples per cycle land off-peak and `cos(22.5 deg) = 0.9239` — a
frequency-dependent metric masquerading as an amplitude check. Envelope amplitude
read 11518 at every point regardless.

Clip detection (`CLIP_THRESHOLD`) went with it, since it rode along as an
out-param of the same per-sample scan. Note what this loses: clipping is a shape
failure that leaves coherence *high*, because a clipped sine still correlates well
with a sine. Envelope spread is now the only guard against it. If a saturating
datapath becomes a live concern, the cheap place to reinstate the check is inside
`dma_example_coherence()`, which already visits every sample.

The interleave check went the same way — it had also never been gated. Only the
FDDC under test is driven, so a tone appearing on converters past the pair would
mean the capture is deinterleaved wrongly; when that was measured, conv0/conv1 sat
at 23045 p-p against 286-367 on conv2/conv3. Coherence cannot see this failure
mode directly: a wrong `num_conv` makes I/Q land on the wrong indices and the score
merely drops without saying why.

## 12. Test point

A single point, `Fs_cap / TEST_TONE_RATE_DIV` with `TEST_TONE_RATE_DIV = 512`,
derived as a fraction of the measured `Fs_cap` so it stays inside the FDDC passband
whatever the profile decimates by. At Fs = 2.5 GHz that is 4882812 Hz, bin +32.

The earlier four-point sweep (`Fs/512`, `Fs/64`, `Fs/8`, `-Fs/64`) has been reduced
to this one point. What that gives up is worth recording: the negative point was
the I/Q ordering check, and it was the only member of the set that could catch a
swapped pair or a spectrally inverted datapath — at `Fs/512` the estimator is still
signed, but nothing in the suite exercises the negative-frequency path any more.
`Fs/8` was the high-end passband check.

512 is also the one divisor that makes the reference exact: the phase accumulator
advances exactly one entry of the 512-entry table per sample, so no index
truncation enters the correlation (measured 0.000 entries of error, against 0.500
for the previous 256-entry table).

A hardcoded Hz value could exceed Nyquist under heavier decimation and produce a
confusing alias instead of a clean failure — hence the divisor.

Retuning cannot disturb test mode: `adi_apollo_fnco_main_pgm()`
(`public/src/adi_apollo_fnco.c:168-197`) writes only `MAIN_PHASE_INC`,
`MAIN_PHASE_OFFSET` and `DRC0_PHASE_INC_FRAC_A/B`, never `BF_DRC_IF_MODE` — only
`fnco_mode_set` / `fnco_pgm` touch the mode (`adi_apollo_fnco.c:417`).

A tone-off reference capture closes the loop: its coherence must collapse to
roughly `COHERENCE_SCALE/N`. Without it, an estimator that returns 1.0 for
anything handed to it would look like a pass. **This now runs first**, so the log
establishes that the estimator rejects noise before it claims to have found a tone.

## 13. Status

**Confirmed on VCU118 (2026-08-05/06, N = 16384, Fs = 2.5 GHz):** the captured
tone tracks the programmed FNCO with the correct rotation direction, constant
envelope, correct converter pair, and amplitude within 0.6% of prediction. The
original four-point sweep re-scored to coherence 1.000 at all four points.

This settles the question the sweep was built to answer: **the FTW writes do
reach hardware** — something the cached-readback `ad9088_get_fnco_freq()` could
never have shown.

**Independently confirmed on HW (2026-08-07):** probing 5 MHz against the
`Fs/512` tone — 0.768 bins off — measured coherence **76**, matching the
prediction from the window response to the digit. That validates the estimator's
*shape*, not just its peak: the phase accumulator, the Q15 table, and the
normalisation all behave as the math says. Note 0.768 bins sits near the first
null, so a deliberate negative test should probe several bins out rather than a
round number of Hz that happens to land there.

**Pending:** an on-target run of the current build (single point, floor-first
ordering, p-p removed).

### Diagnostic map

| symptom | cause |
|---|---|
| tone low, floor also low | estimator or `Fs_cap` wrong, not the datapath — check the TPL rate line |
| coherence 0 but amplitude healthy | signal present at the wrong frequency, or probe/tone sign mismatch |
| coherence unchanged regardless of `f_prog` | FTW write not reaching hardware |
| floor scores above threshold | the measurement itself is untrustworthy |
| amplitude also collapsed | not a frequency problem — no signal reaching the buffer |

## 14. Reuse for the DMA-driven path

The estimator transfers unchanged, with one sign change: a **TX FDUC upconverts**,
so the expected rotation is `+f_nco` where RX is `-f_nco`. And if the TX CDUC is
`MIXER_REAL` (see §2 — unverified), the output carries **both** sidebands, so
energy splits between `+-f_nco` and coherence at either lands near 0.5 rather
than 1.0. Anticipate that rather than reading it as a failure; the two `bf_get`
readbacks named in §2 would settle it first.

`sine_lut_iq` (`drivers/axi_core/axi_dac_core/axi_dac_core.c:140`) is a stronger
reference than the test tone: 1024 entries, exactly 8 cycles per 1024 samples
(period 128), amplitude 9830 = 0.3000 x full scale, `sum(I*Q) = 0` (perfect
quadrature), constant envelope. So the predicted TX baseband frequency is
`Fs_tx/128`, shifted by whatever NCOs are programmed.

Two things this enables that a tone cannot:

- **Cross-correlation against the known LUT.** Cyclic DMA makes the received
  signal periodic with the LUT, so a normalised cross-correlation should peak
  near 1.0 at some lag — validating waveform *shape* and sample ordering, not
  just frequency.
- **Sweeping the TX FNCO with the LUT fixed.** The received frequency must move
  by exactly the programmed shift. If amplitude stays at the floor at *every*
  frequency, the fault is upstream of the mixer — cabling, DAC output enable, or
  the `tx1_en_gpio` / `tx2_en_gpio` that `ad9088.c:1188-1189` only ever removes
  and never asserts.

With 42 dB of processing gain this will also settle whether the earlier 1.34x
loopback result was real attenuation (signal present ~30 dB down) or just a
different noise realisation.
