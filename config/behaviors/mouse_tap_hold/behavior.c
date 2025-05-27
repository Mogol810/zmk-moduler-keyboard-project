#include <device.h>
#include <drivers/behavior.h>
#include <zephyr/kernel.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>
#include <zmk/hid.h>
#include <zmk/keycode.h>
#include <zmk/behavior.h>

#define TAPPING_TERM_MS 200

struct tap_hold_state {
    int64_t press_time;
};

static struct tap_hold_state state;

static int bhv_press(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    state.press_time = k_uptime_get();
    return 0;
}

static int bhv_release(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    int64_t elapsed = k_uptime_get() - state.press_time;

    if (elapsed < TAPPING_TERM_MS) {
        // param1 → タップ側 → マウスボタン
        zmk_hid_mouse_button_press(binding->param1);
        zmk_hid_mouse_button_release(binding->param1);
    } else {
        // param2 → ホールド側 → キーコード
        zmk_hid_keycode_press(binding->param2);
        zmk_hid_keycode_release(binding->param2);
    }

    return 0;
}

static const struct behavior_driver_api bhv_driver = {
    .binding_pressed = bhv_press,
    .binding_released = bhv_release,
};

DEVICE_DT_INST_DEFINE(0, NULL, NULL, NULL, NULL,
                      APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,
                      &bhv_driver);
