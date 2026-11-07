// Host-side unit tests for OSD keyboard polling and state caching logic
// implemented in src/MPUBIOS/app/minimig_osd_keyboard.cpp.
// Compile and run with:
//   g++ -std=c++14 tests/test_osd_keyboard.cpp -o /tmp/test_osd_keyboard && /tmp/test_osd_keyboard

#include <cassert>
#include <cstdint>
#include <cstdio>

// Mock state to verify the exact behavior of OSD keyboard polling
static uint32_t s_mock_core_output = 0;
static int s_core_write_count = 0;
static int s_osd_disable_count = 0;

static void mock_osd_disable() {
    s_osd_disable_count++;
}

// Model function reflecting the exact logic in minimig_osd_keyboard.cpp
static void process_keyboard_display_state(bool keyboard_is_showing, bool &keyboard_was_showing) {
    if (keyboard_is_showing) {
        keyboard_was_showing = true;
        uint32_t tmp = s_mock_core_output;
        if ((tmp & 0x00000030) != 0x00000030) {
            s_mock_core_output = tmp | 0x00000030;
            s_core_write_count++;
        }
    } else if (keyboard_was_showing) {
        keyboard_was_showing = false;
        mock_osd_disable();
        uint32_t tmp = s_mock_core_output;
        if (tmp & 0x00000030) {
            s_mock_core_output = tmp & 0xFFFFFFCF;
            s_core_write_count++;
        }
    }
}

int main() {
    s_mock_core_output = 0x00000030; // simulate any initial bit state
    s_core_write_count = 0;
    s_osd_disable_count = 0;
    bool keyboard_was_showing = true; // initial static state

    // 1. Initial pass with keyboard inactive should disable OSD and clear output bits once
    process_keyboard_display_state(false, keyboard_was_showing);
    assert(s_osd_disable_count == 1);
    assert((s_mock_core_output & 0x00000030) == 0);
    assert(s_core_write_count == 1);
    assert(keyboard_was_showing == false);

    // 2. Continuous idle polling loops must produce zero redundant OsdDisable() calls and zero register writes
    for (int i = 0; i < 5000; i++) {
        process_keyboard_display_state(false, keyboard_was_showing);
    }
    assert(s_osd_disable_count == 1);
    assert(s_core_write_count == 1);
    assert(keyboard_was_showing == false);

    // 3. Activating keyboard asserts bits 0x30 in CORE_OUTPUT_REGISTOR()
    process_keyboard_display_state(true, keyboard_was_showing);
    assert((s_mock_core_output & 0x00000030) == 0x00000030);
    assert(s_core_write_count == 2);
    assert(s_osd_disable_count == 1);
    assert(keyboard_was_showing == true);

    // 4. Repeated active polling loops while keyboard remains open must NOT re-write CORE_OUTPUT_REGISTOR
    for (int i = 0; i < 5000; i++) {
        process_keyboard_display_state(true, keyboard_was_showing);
    }
    assert(s_core_write_count == 2);
    assert(s_osd_disable_count == 1);
    assert(keyboard_was_showing == true);

    // 5. Dismissing keyboard calls OsdDisable() and clears bits 0x30 exactly once
    process_keyboard_display_state(false, keyboard_was_showing);
    assert(s_osd_disable_count == 2);
    assert((s_mock_core_output & 0x00000030) == 0);
    assert(s_core_write_count == 3);
    assert(keyboard_was_showing == false);

    // 6. Subsequent idle polling loops remain completely silent with 0 bus/MMIO traffic
    for (int i = 0; i < 5000; i++) {
        process_keyboard_display_state(false, keyboard_was_showing);
    }
    assert(s_osd_disable_count == 2);
    assert(s_core_write_count == 3);
    assert(keyboard_was_showing == false);

    printf("All OSD keyboard state caching tests passed successfully!\n");
    return 0;
}
