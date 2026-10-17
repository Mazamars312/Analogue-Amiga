/*
 * Host-side unit tests for mouse_math.h -- the dock-mouse/analog-stick
 * scaling logic that shipped three real bugs in a row before this existed:
 *
 *   1. Decoding cont4_joy/cont4_trig as a genuine 16-bit value (it's an
 *      8-bit signed HID delta at bits[15:8], not 16 bits).
 *   2. A single fast report's scaled step going straight into the output
 *      queue uncapped, building a multi-frame backlog ("pointer moves to
 *      random places" during fast movement).
 *   3. Over-correcting that with a report cap so tight (16) that ordinary
 *      fast movement -- not just extreme flicks -- got squashed to the
 *      same flat ceiling ("pointer feels much slower than the mouse").
 *
 * Each of those is exactly the kind of thing that's obvious once you see
 * it on hardware and easy to get wrong again in the next tuning pass.
 * These tests pin down the properties we ended up reasoning about by hand
 * each time, so a regression shows up here instead of costing another
 * build-flash-play round trip.
 *
 * Build/run: `make -C src/MPUBIOS/drivers/KMIO/test` (uses the host cc,
 * NOT the riscv32 cross-compiler -- this is pure portable C, no MPU
 * register access, no hardware needed).
 */

#include <stdio.h>
#include <string.h>
#include "../mouse_math.h"

static int failures = 0;

#define CHECK(cond, ...) do { \
  if (!(cond)) { \
    failures++; \
    printf("FAIL %s:%d: ", __FILE__, __LINE__); \
    printf(__VA_ARGS__); \
    printf("\n"); \
  } \
} while (0)

/* --- mouse_decode_delta ------------------------------------------------ */

static void test_decode_delta(void) {
  /* bits[15:8] = 0x00 -> 0 */
  CHECK(mouse_decode_delta(0x00000000u) == 0, "zero delta decoded as %d", mouse_decode_delta(0x00000000u));
  /* bits[15:8] = 0x05 -> +5 */
  CHECK(mouse_decode_delta(0x00000500u) == 5, "+5 decoded as %d", mouse_decode_delta(0x00000500u));
  /* bits[15:8] = 0xFB (-5 signed) -> -5 */
  CHECK(mouse_decode_delta(0x0000FB00u) == -5, "-5 decoded as %d", mouse_decode_delta(0x0000FB00u));
  /* max positive: 0x7F -> 127 */
  CHECK(mouse_decode_delta(0x00007F00u) == 127, "max positive decoded as %d", mouse_decode_delta(0x00007F00u));
  /* max negative: 0x80 -> -128 */
  CHECK(mouse_decode_delta(0x00008000u) == -128, "max negative decoded as %d", mouse_decode_delta(0x00008000u));
  /* garbage in the reserved low byte must not affect the result */
  CHECK(mouse_decode_delta(0x000005FFu) == 5, "low-byte garbage changed +5 to %d", mouse_decode_delta(0x000005FFu));
  /* other bits in the register (e.g. button bits at [18:16]) must not leak in */
  CHECK(mouse_decode_delta(0x00070500u) == 5, "button bits leaked into delta: %d", mouse_decode_delta(0x00070500u));
}

/* --- mouse_scale_step: no sub-count truncation (regression for problem B2) */

static void test_scale_step_slow_motion_not_lost(void) {
  /* A repeated 1-count move at a middling speed must eventually produce
     real output via the remainder carry, not be truncated to zero forever
     (the original bug: x/(speed<<5) discarded any single-count move). */
  int32_t rem = 0;
  int32_t total = 0;
  int nonzero_steps = 0;
  for (int i = 0; i < 200; i++) {
    int32_t step = mouse_scale_step(&rem, 1, 10, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
    total += step;
    if (step != 0) nonzero_steps++;
  }
  CHECK(nonzero_steps > 0, "200 reports of a 1-count move produced zero output every time");
  /* Average output should track the exact rational rate (1*10/8 = 1.25/report) */
  double expected = 200.0 * 1.0 * 10.0 / MOUSE_DOCK_DIV;
  CHECK(total > (int32_t)(expected * 0.9) && total < (int32_t)(expected * 1.1) + 1,
        "cumulative output %d far from expected ~%.1f for sustained slow motion", total, expected);
}

/* --- mouse_scale_step: typical fast movement must NOT hit the report cap
   (regression for MOUSE_REPORT_CAP=16 squashing ordinary fast movement) -- */

static void test_scale_step_typical_movement_not_over_clipped(void) {
  /* A brisk but unremarkable single report (well under the 8-bit max of
     127) at the default speed (10) must come through proportionally, not
     get flattened to the cap the way it did when the cap was 16. */
  int32_t rem = 0;
  int32_t step = mouse_scale_step(&rem, 40, 10, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
  int32_t expected = 40 * 10 / MOUSE_DOCK_DIV; /* = 50 at the current tuning */
  CHECK(step == expected, "typical report (dx=40, speed=10) produced %d, expected %d (uncapped)", step, expected);
  CHECK(step < MOUSE_REPORT_CAP, "typical report hit the report cap (%d) -- cap is too tight again", (int)MOUSE_REPORT_CAP);
}

static void test_scale_step_extreme_is_capped(void) {
  /* Max delta at max speed must be clipped, not allowed to inject a huge
     lump (the original coasting/backlog bug). */
  int32_t rem = 0;
  int32_t step = mouse_scale_step(&rem, 127, 32, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
  CHECK(step == MOUSE_REPORT_CAP, "max flick at max speed produced %d, expected exactly the cap %d", step, (int)MOUSE_REPORT_CAP);
}

static void test_scale_step_cap_does_not_leak_into_remainder(void) {
  /* Clipping a step must not let the dropped excess quietly resurface a
     few reports later via the remainder carry -- it should be gone. */
  int32_t rem = 0;
  int32_t first = mouse_scale_step(&rem, 127, 32, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
  CHECK(first == MOUSE_REPORT_CAP, "sanity: first extreme report should be capped");
  int32_t second = mouse_scale_step(&rem, 0, 32, MOUSE_DOCK_DIV, MOUSE_REPORT_CAP);
  CHECK(second == 0, "a zero-delta report after a capped one produced %d (excess leaked back in via *rem)", second);
}

/* --- mouse_enqueue / mouse_drain ---------------------------------------- */

static void test_drain_returns_full_amount_under_cap(void) {
  int32_t pending = 5;
  int32_t out = mouse_drain(&pending);
  CHECK(out == 5, "draining 5 (under MOUSE_MAX_PER_POLL) returned %d", out);
  CHECK(pending == 0, "pending should be empty after draining an under-cap amount, was %d", pending);
}

static void test_drain_caps_per_poll_output(void) {
  int32_t pending = 999;
  int32_t out = mouse_drain(&pending);
  CHECK(out == MOUSE_MAX_PER_POLL, "draining a large backlog returned %d, expected the cap %d", out, (int)MOUSE_MAX_PER_POLL);
  CHECK(pending == 999 - MOUSE_MAX_PER_POLL, "remainder after one drain was %d", pending);
}

static void test_enqueue_drain_conserves_total(void) {
  /* Everything enqueued (while under the pending clamp) must eventually
     come back out via repeated draining -- no silent loss beyond the
     report-cap stage. */
  int32_t pending = 0;
  mouse_enqueue(&pending, 37);
  int32_t total_out = 0;
  for (int i = 0; i < 20; i++) total_out += mouse_drain(&pending);
  CHECK(total_out == 37, "enqueued 37, drained %d over 20 polls", total_out);
  CHECK(pending == 0, "pending should be fully drained, was %d", pending);
}

static void test_pending_clamp_bounds_backlog(void) {
  int32_t pending = 0;
  for (int i = 0; i < 10; i++) mouse_enqueue(&pending, 100); /* 1000 total, way over the clamp */
  CHECK(pending == MOUSE_PENDING_CLAMP, "pending backlog reached %d, expected it clamped at %d", pending, (int)MOUSE_PENDING_CLAMP);

  int32_t pending_neg = 0;
  for (int i = 0; i < 10; i++) mouse_enqueue(&pending_neg, -100);
  CHECK(pending_neg == -MOUSE_PENDING_CLAMP, "negative backlog reached %d, expected clamped at %d", pending_neg, (int)-MOUSE_PENDING_CLAMP);
}

/* --- Regression: MOUSE_PENDING_CLAMP=200 let a SUSTAINED fast drag (not
   just a single flick -- repeated reports each feeding the queue faster
   than it drains, for as long as the drag continues) saturate the backlog
   and keep it saturated for the whole drag, so releasing the mouse still
   left up to 200/MOUSE_MAX_PER_POLL polls of queued motion to play out --
   "I stop moving the mouse but the pointer continues to travel." -------- */

static void test_coast_after_release_is_short(void) {
  /* Worst case: backlog fully saturated at the instant input stops. */
  int32_t pending = MOUSE_PENDING_CLAMP;
  int polls = 0;
  while (pending != 0 && polls < 1000) { mouse_drain(&pending); polls++; }
  const int max_acceptable_polls = 10; /* ~1 frame (20ms); want comfortably under this */
  CHECK(polls <= max_acceptable_polls,
        "draining a fully saturated backlog after release took %d polls (~%dms) -- pointer would visibly coast after the hand stops",
        polls, polls * 2);
}

static void test_typical_movement_not_reclipped_by_pending_clamp(void) {
  /* The clamp that bounds post-release coast must stay above what
     ordinary (non-extreme) fast movement needs per report, or it just
     reintroduces the "everyday movement squashed" bug at a different
     layer than MOUSE_REPORT_CAP. */
  int32_t typical_step = 40 * 10 / MOUSE_DOCK_DIV; /* same case as the report-cap test above */
  CHECK(MOUSE_PENDING_CLAMP > typical_step,
        "MOUSE_PENDING_CLAMP (%d) is at or below typical fast movement's step (%d) -- ordinary dragging would get squashed here too",
        (int)MOUSE_PENDING_CLAMP, (int)typical_step);
}

/* --- The safety invariant the whole design exists for -------------------
   The Amiga samples its 8-bit JOY0DAT counter once per video frame and
   misreads direction if more than ~127 counts land between two reads. A
   real game frame is nominally 10 of our 2ms polls (~20ms PAL), but can be
   wider if a frame is dropped. This directly asserts the property "down,
   up a notch" was a violation of: even a fully-saturated backlog, drained
   over a generously wide window, must not exceed 127. */

static void test_frame_budget_never_exceeds_amiga_ceiling(void) {
  const int32_t AMIGA_FRAME_CEILING = 127;
  const int polls_in_a_wide_frame = 15; /* 30ms -- a dropped-frame-ish window, wider than nominal */

  int32_t pending = MOUSE_PENDING_CLAMP; /* worst case: fully saturated backlog */
  int32_t total = 0;
  for (int i = 0; i < polls_in_a_wide_frame; i++) total += mouse_drain(&pending);
  CHECK(total <= AMIGA_FRAME_CEILING,
        "drained %d over %d polls from a saturated backlog -- exceeds the Amiga's ~%d/frame ceiling and would misread direction",
        total, polls_in_a_wide_frame, (int)AMIGA_FRAME_CEILING);
}

static void test_single_capped_report_drains_within_one_frame(void) {
  const int nominal_polls_per_frame = 10;
  int32_t pending = 0;
  mouse_enqueue(&pending, MOUSE_REPORT_CAP); /* the worst a single report can inject */
  int32_t total = 0;
  for (int i = 0; i < nominal_polls_per_frame; i++) total += mouse_drain(&pending);
  CHECK(pending == 0, "a single max-capped report left %d still queued after a full frame's worth of polls", pending);
  CHECK(total == MOUSE_REPORT_CAP, "a single max-capped report only delivered %d of %d within one frame", total, (int)MOUSE_REPORT_CAP);
}

int main(void) {
  test_decode_delta();
  test_scale_step_slow_motion_not_lost();
  test_scale_step_typical_movement_not_over_clipped();
  test_scale_step_extreme_is_capped();
  test_scale_step_cap_does_not_leak_into_remainder();
  test_drain_returns_full_amount_under_cap();
  test_drain_caps_per_poll_output();
  test_enqueue_drain_conserves_total();
  test_pending_clamp_bounds_backlog();
  test_coast_after_release_is_short();
  test_typical_movement_not_reclipped_by_pending_clamp();
  test_frame_budget_never_exceeds_amiga_ceiling();
  test_single_capped_report_drains_within_one_frame();

  if (failures == 0) {
    printf("PASS: all mouse_math tests passed\n");
    return 0;
  } else {
    printf("FAIL: %d mouse_math test(s) failed\n", failures);
    return 1;
  }
}
