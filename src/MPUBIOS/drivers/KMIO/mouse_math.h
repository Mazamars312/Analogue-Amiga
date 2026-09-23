/*
 * Copyright 2022 Murray Aickin
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef MOUSE_MATH_H
#define MOUSE_MATH_H

// Pure integer math for dock-mouse/analog-stick scaling, deliberately kept
// free of any MPU/APF register access so it can be unit-tested on the host
// (see test/test_mouse_math.c) with the exact same code the firmware ships --
// not a reimplementation that could drift. Only #include <stdint.h>.
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// APF controller-4 dock-mouse report: cont4_joy[15:0] (X) and
// cont4_trig[15:0] (Y) are NOT a genuine 16-bit little-endian value despite
// Analogue's docs describing them that way -- USB boot-mouse reports only
// ever carry an 8-bit signed relative delta per axis, and that's what's
// here: bits[15:8], sign-extended, with bits[7:0] reserved/always zero.
// (Cross-checked on real hardware against a sibling openFPGA core, which
// independently confirmed the same "meaningful byte on top" packing on its
// own dock-mouse AND keyboard-modifier decode -- a consistent Analogue
// convention, not a one-off. Treating the field as a real 16-bit value
// reads a small movement as a huge one and saturates instantly.)
static inline int8_t mouse_decode_delta(uint32_t reg) {
  return (int8_t)((reg >> 8) & 0xFF);
}

// The Amiga only ever samples JOY0DAT (an 8-bit counter) once per video
// frame (~20ms PAL), but we poll every 2ms, so no single source may hand
// more than a few counts to a single poll or a fast burst can exceed what
// a frame can represent and read back as movement in the wrong direction.
// Every source funnels its scaled deltas through this shared queue: enqueue
// adds a poll's worth (clamped so a hard flick can't build unbounded lag),
// drain hands out at most MOUSE_MAX_PER_POLL counts per call and keeps the
// remainder queued for the next poll.
// A single report's delta is signed 8-bit (max 127) and "Mouse Speed" is a
// 1-32 slider (dist/Cores/Mazamars312.Amiga/interact.json). These three
// numbers used to be one knob (a big MOUSE_DOCK_DIV) doing two different
// jobs at once: everyday sensitivity, and stopping one extreme flick from
// injecting a lump the queue takes many frames to drain. Conflating them
// meant fixing the backlog (DIV 8->32) quartered sensitivity for ALL
// movement, not just extreme flicks ("pointer feels slower than the mouse").
// Now they're separate: MOUSE_DOCK_DIV sets normal-speed feel (back to the
// original, felt-correct value); MOUSE_REPORT_CAP clips only a single
// report's contribution before it's queued, so a fast flick can't build a
// multi-frame backlog in the first place; MOUSE_MAX_PER_POLL keeps real
// margin under the Amiga's ~127-counts/frame ceiling -- Lemmings won't
// sample at a perfectly steady 20ms if it drops a frame under load, and
// skimming right against 127 (12/poll * 10 polls = 120) meant a slightly
// wider real gap between two game-side reads could still push the total
// past 127 and wrap into a small negative delta: "down, up a notch."
// DIV=8 made the "Mouse Speed" slider's default (10, dist/Cores/Mazamars312.Amiga/interact.json,
// id 124, range 1-32) feel too fast for at least one real mouse -- setting
// 5 was the validated sweet spot on hardware. Rather than move the default
// down into a cramped corner of a 1-32 range (wasting most of the slider's
// resolution above it), DIV is doubled here so the *existing* default of
// 10 reproduces that validated speed=5-at-DIV=8 feel (dx*10/16 == dx*5/8
// for any dx), leaving the whole 1-32 range usable. This is a straight
// derivation from tested data, not a fresh guess -- but individual mice
// still vary (see README "mouse feels too fast/slow" -- DPI isn't exposed
// over USB HID boot protocol, so no per-mouse auto-compensation is
// possible here), so still worth reconfirming on hardware.
#define MOUSE_DOCK_DIV      16   // tune by feel -- normal-speed sensitivity only
// MOUSE_REPORT_CAP=16 was far too tight: at DIV=8/default speed=10, ANY
// report demanding more than ~13 raw counts (out of a possible 127) hit
// this same flat ceiling, so nearly all real fast movement -- not just
// extreme flicks -- was being squashed to one low output regardless of
// how fast the hand actually moved ("much slower than I perceive the
// input to be"). 80 = MOUSE_MAX_PER_POLL*10 polls/frame, so even a fully
// clipped report still drains within about one frame; it only bites on
// truly extreme single-report demand (well above normal fast movement),
// not everyday fast motion.
// MOUSE_REPORT_CAP=80 / MOUSE_PENDING_CLAMP=200 used to be two different
// numbers, which made the pending clamp (being larger) pointless for a
// single report -- mouse_enqueue's own clamp only bites once several
// reports have piled up. And 200 itself was sized to hold "a couple of
// capped reports," the wrong mental model: a SUSTAINED fast drag (not
// just one flick) keeps feeding the queue faster than it drains for as
// long as the drag continues, so the backlog saturates at the clamp and
// STAYS there for the whole drag -- then takes clamp/MOUSE_MAX_PER_POLL
// polls to empty after the hand stops ("I stop moving the mouse but the
// pointer keeps going"). One shared limit is simpler and bounds both
// cases the same way: 64 sits just above what typical (non-extreme) fast
// movement needs per report, so ordinary dragging isn't clipped here, but
// worst-case coast after release is bounded to ~7 polls (~14ms, well
// under one frame) instead of 200/10 = 20 polls (~40ms).
#define MOUSE_REPORT_CAP    64
#define MOUSE_MAX_PER_POLL  10   // ~100 counts/frame at 10 polls/frame -- real margin under 127
#define MOUSE_PENDING_CLAMP 64
#define MOUSE_STICK_DIV     64   // matches the original analog-stick >>6 scale

// Scale a raw per-report delta by speed/div with remainder carry (so slow,
// precise motion -- e.g. a 1-count move at default speed -- isn't truncated
// away to zero), then clip this single report's contribution to +-cap
// (excess simply dropped, not carried forward in *rem -- carrying it would
// just recreate the backlog the cap exists to prevent). Returns the step to
// hand to mouse_enqueue().
static inline int32_t mouse_scale_step(int32_t *rem, int16_t raw_delta, uint16_t speed, int32_t div, int32_t cap) {
  *rem += (int32_t)raw_delta * (int32_t)speed;
  int32_t step = *rem / div;
  *rem -= step * div;
  if (step >  cap) step =  cap;
  if (step < -cap) step = -cap;
  return step;
}

static inline void mouse_enqueue(int32_t *pending, int32_t step) {
  *pending += step;
  if (*pending >  MOUSE_PENDING_CLAMP) *pending =  MOUSE_PENDING_CLAMP;
  if (*pending < -MOUSE_PENDING_CLAMP) *pending = -MOUSE_PENDING_CLAMP;
}

static inline int32_t mouse_drain(int32_t *pending) {
  int32_t step = (*pending >  MOUSE_MAX_PER_POLL) ?  MOUSE_MAX_PER_POLL :
                 (*pending < -MOUSE_MAX_PER_POLL) ? -MOUSE_MAX_PER_POLL : *pending;
  *pending -= step;
  return step;
}

#ifdef __cplusplus
}
#endif

#endif // MOUSE_MATH_H
