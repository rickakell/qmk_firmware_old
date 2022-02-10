#include QMK_KEYBOARD_H

#include "oneshot.h"
#include "swapper.h"

#define TABL C(S(KC_TAB))
#define TABR C(KC_TAB)
#define WINB S(KC_TAB)
#define CPY C(KC_C)
#define PST C(KC_V)
#define UND C(KC_Z)
#define RED C(KC_Y)
#define FND C(KC_F)
#define WN_SNIP G(S(KC_S))
#define RC_CLIP G(A(KC_G))
#define LA_SYM MO(SYM)
#define LA_NAV MO(NAV)
#define TO_GME TO(GME)
#define TO_DEF TO(DEF)

enum layers {
    DEF,
    SYM,
    NAV,
    NUM,
    GME,
};

enum keycodes {
    // Custom oneshot mod implementation with no timers.
    OS_SHFT = SAFE_RANGE,
    OS_CTRL,
    OS_ALT,
    OS_CMD,

    SW_WINF,  // Switch to next window        (alt-tab)
    SW_WINB,  // Switch to previous window    (alt-shift-tab)
    SW_LANG, // Switch to next input language (ctl-spc)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [DEF] = LAYOUT_split_3x5_3(
        KC_QUOT, KC_COMMA,KC_DOT,  KC_P,    KC_Y,    KC_F,    KC_G,    KC_C,    KC_R,    KC_L,
        KC_A,    KC_O,    KC_E,    KC_U,    KC_I,    KC_D,    KC_H,    KC_T,    KC_N,    KC_S,
        KC_SCLN, KC_Q,    KC_J,    KC_K,    KC_X,    KC_B,    KC_M,    KC_W,    KC_V,    KC_Z,
                          TO_GME,  LA_NAV,  KC_LSFT, KC_SPC,  LA_SYM, RC_CLIP
    ),

    [SYM] = LAYOUT_split_3x5_3(
        KC_GRV,  KC_LBRC, KC_LPRN, KC_LCBR, KC_TILD, KC_CIRC, KC_RCBR, KC_RPRN, KC_RBRC, KC_GRV,
        KC_MINS, KC_ASTR, KC_EQL,  KC_UNDS, KC_DLR,  KC_HASH, OS_SHFT, OS_CTRL, OS_CMD,  OS_ALT,
        KC_PLUS, KC_PIPE, KC_AT,   KC_BSLS, KC_PERC, KC_AMPR, KC_SLSH, KC_QUES, KC_COLN, KC_EXLM,
                          KC_F13,  _______, _______, KC_F14,  _______, KC_F15
    ),

    [NAV] = LAYOUT_split_3x5_3(
        TABL,    TABR,    SW_WINF, SW_WINB, KC_VOLU, KC_CAPS, KC_HOME, KC_UP,   KC_END,  KC_DEL,
        OS_ALT,  KC_LGUI, OS_CTRL, OS_SHFT, KC_TAB,  KC_ESC,  KC_LEFT, KC_DOWN, KC_RGHT, KC_BSPC,
        UND,     RED,     CPY,     PST,     KC_VOLD, FND,     KC_PGDN, KC_PGUP, SW_LANG, KC_ENT,
                          WN_SNIP, _______, _______, _______, _______, _______
    ),

    [NUM] = LAYOUT_split_3x5_3(
        XXXXXXX, KC_6,    KC_5,    KC_4,    XXXXXXX, KC_F8,   KC_F9,   KC_F10, KC_F11,  KC_F12,
        KC_0,    KC_3,    KC_2,    KC_1,    XXXXXXX, KC_F7,   OS_SHFT, OS_CTRL, OS_CMD,  OS_ALT,
        XXXXXXX, KC_9,    KC_8,    KC_7,    XXXXXXX, KC_F6,   KC_F5,   KC_F4,   KC_F3,   KC_F2,
                          _______, _______, _______, _______, _______, KC_F1
    ),

    [GME] = LAYOUT_split_3x5_3(
        KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,   KC_Y,    KC_UP,   KC_I,    KC_O,
        KC_LSFT, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,   KC_LEFT, KC_DOWN, KC_RGHT, KC_L,
        KC_TAB,  KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,   KC_N,    KC_M,    KC_COMM, KC_DOT,
                          TO_DEF, KC_LCTL, KC_SPACE, _______, XXXXXXX, _______
    ),
};

bool is_oneshot_cancel_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
        return true;
    default:
        return false;
    }
}

bool is_oneshot_ignored_key(uint16_t keycode) {
    switch (keycode) {
    case LA_SYM:
    case LA_NAV:
    case TO_DEF:
    case KC_LSFT:
    case OS_SHFT:
    case OS_CTRL:
    case OS_ALT:
    case OS_CMD:
        return true;
    default:
        return false;
    }
}

bool sw_win_active = false;
bool sw_win_back_active = false;

oneshot_state os_shft_state = os_up_unqueued;
oneshot_state os_ctrl_state = os_up_unqueued;
oneshot_state os_alt_state = os_up_unqueued;
oneshot_state os_cmd_state = os_up_unqueued;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    update_swapper(&sw_win_active, KC_LALT, KC_TAB, SW_WINF, keycode, record);
    update_swapper_back(&sw_win_back_active, KC_LALT, KC_LSFT, KC_TAB, SW_WINB, keycode, record);

    update_oneshot(&os_shft_state, KC_LSFT, OS_SHFT, keycode, record);
    update_oneshot(&os_ctrl_state, KC_LCTL, OS_CTRL, keycode, record);
    update_oneshot(&os_alt_state, KC_LALT, OS_ALT, keycode, record);
    update_oneshot(&os_cmd_state, KC_LCMD, OS_CMD, keycode, record);

    return true;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, SYM, NAV, NUM);
}

#ifdef OLED_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  if (is_keyboard_master())
    return OLED_ROTATION_180;
  return rotation;
}

static void render_status(void) {
   switch (get_highest_layer(layer_state)) {
    case DEF:
      oled_write_P(PSTR("D S Na Nu G\n"), false);
      oled_write_P(PSTR("^\n"), false);
      oled_write_P(PSTR("Dvorak\n"), false);
      break;
    case SYM:
      oled_write_P(PSTR("D S Na Nu G\n"), false);
      oled_write_P(PSTR("  ^\n"), false);
      oled_write_P(PSTR("Symbol\n"), false);
      break;
    case NAV:
      oled_write_P(PSTR("D S Na Nu G\n"), false);
      oled_write_P(PSTR("    ^\n"), false);
      oled_write_P(PSTR("Navigation\n"), false);
      break;
    case NUM:
      oled_write_P(PSTR("D S Na Nu G\n"), false);
      oled_write_P(PSTR("       ^\n"), false);
      oled_write_P(PSTR("Number\n"), false);
      break;
    case GME:
      oled_write_P(PSTR("D S Na Nu G\n"), false);
      oled_write_P(PSTR("          ^\n"), false);
      oled_write_P(PSTR("Game\n"), false);
    default:
      break;
  }
}

bool oled_task_user(void) {
  render_status();
  return true;
}
#endif
