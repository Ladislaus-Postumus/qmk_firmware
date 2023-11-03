#include QMK_KEYBOARD_H

typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_SINGLE_TAP,
    TD_SINGLE_HOLD,
    TD_DOUBLE_TAP,
    TD_DOUBLE_HOLD,
    TD_DOUBLE_SINGLE_TAP, // Send two single taps
    TD_TRIPLE_TAP,
    TD_TRIPLE_HOLD
} td_state_t;

typedef struct {
    bool is_press_action;
    td_state_t state;
} td_tap_t;

// Tap dance enums
enum {
    X_CTL,
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
};

td_state_t cur_dance(tap_dance_state_t *state);

// For the x tap dance. Put it here so it can be used in any keymap
void x_finished(tap_dance_state_t *state, void *user_data);
void x_reset(tap_dance_state_t *state, void *user_data);

tap_dance_action_t tap_dance_actions[] = {
    [X_CTL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, x_finished, x_reset),
    [A] = ACTION_TAP_DANCE_DOUBLE(KC_A, LSFT(KC_A)),
    [B] = ACTION_TAP_DANCE_DOUBLE(KC_B, LSFT(KC_B)),
    [C] = ACTION_TAP_DANCE_DOUBLE(KC_C, LSFT(KC_C)),
    [D] = ACTION_TAP_DANCE_DOUBLE(KC_D, LSFT(KC_D)),
    [E] = ACTION_TAP_DANCE_DOUBLE(KC_E, LSFT(KC_E)),
    [F] = ACTION_TAP_DANCE_DOUBLE(KC_F, LSFT(KC_F)),
    [G] = ACTION_TAP_DANCE_DOUBLE(KC_G, LSFT(KC_G)),
    [H] = ACTION_TAP_DANCE_DOUBLE(KC_H, LSFT(KC_H)),
    [I] = ACTION_TAP_DANCE_DOUBLE(KC_I, LSFT(KC_I)),
    [J] = ACTION_TAP_DANCE_DOUBLE(KC_J, LSFT(KC_J)),
    [K] = ACTION_TAP_DANCE_DOUBLE(KC_K, LSFT(KC_K)),
    [L] = ACTION_TAP_DANCE_DOUBLE(KC_L, LSFT(KC_L)),
    [M] = ACTION_TAP_DANCE_DOUBLE(KC_M, LSFT(KC_M)),
    [N] = ACTION_TAP_DANCE_DOUBLE(KC_N, LSFT(KC_N)),
    [O] = ACTION_TAP_DANCE_DOUBLE(KC_O, LSFT(KC_O)),
    [P] = ACTION_TAP_DANCE_DOUBLE(KC_P, LSFT(KC_P)),
    [Q] = ACTION_TAP_DANCE_DOUBLE(KC_Q, LSFT(KC_Q)),
    [R] = ACTION_TAP_DANCE_DOUBLE(KC_R, LSFT(KC_R)),
    [S] = ACTION_TAP_DANCE_DOUBLE(KC_S, LSFT(KC_S)),
    [T] = ACTION_TAP_DANCE_DOUBLE(KC_T, LSFT(KC_T)),
    [U] = ACTION_TAP_DANCE_DOUBLE(KC_U, LSFT(KC_U)),
    [V] = ACTION_TAP_DANCE_DOUBLE(KC_V, LSFT(KC_V)),
    [W] = ACTION_TAP_DANCE_DOUBLE(KC_W, LSFT(KC_W)),
    [X] = ACTION_TAP_DANCE_DOUBLE(KC_X, LSFT(KC_X)),
    [Y] = ACTION_TAP_DANCE_DOUBLE(KC_Y, LSFT(KC_Y)),
    [Z] = ACTION_TAP_DANCE_DOUBLE(KC_Z, LSFT(KC_Z)),
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[0] = LAYOUT_split_3x5_2(//
	    TD(Q), TD(W), TD(F), TD(P), TD(B),
	                      TD(KC_BSPC), TD(J), TD(L), TD(U), TD(Y),
	    TD(A), TD(R), TD(S), TD(T), TD(G),
	                      TD(O), TD(I), TD(E), TD(N), TD(M),
	    TD(Z), TD(X), TD(C), TD(D), TD(V),
	                      XXXXXXX, XXXXXXX, OSM(MOD_MEH), TD(H), TD(K),
	    //thumb row
	    OSL(1), KC_ENT, //  :.;,/   . tap   : hold   ; taptap   , taphold   / taptaptap
	                      KC_SPC, KC_ESC
	),
};

td_state_t cur_dance(tap_dance_state_t *state) {
    if (state->count == 1) {
        if (state->interrupted || !state->pressed) return TD_SINGLE_TAP;
        // Key has not been interrupted, but the key is still held. Means you want to send a 'HOLD'.
        else return TD_SINGLE_HOLD;
    } else if (state->count == 2) {
        // TD_DOUBLE_SINGLE_TAP is to distinguish between typing "pepper", and actually wanting a double tap
        // action when hitting 'pp'. Suggested use case for this return value is when you want to send two
        // keystrokes of the key, and not the 'double tap' action/macro.
        if (state->interrupted) return TD_DOUBLE_SINGLE_TAP;
        else if (state->pressed) return TD_DOUBLE_HOLD;
        else return TD_DOUBLE_TAP;
    }
    // Assumes no one is trying to type the same letter three times (at least not quickly).
    // If your tap dance key is 'KC_W', and you want to type "www." quickly - then you will need to add
    // an exception here to return a 'TD_TRIPLE_SINGLE_TAP', and define that enum just like 'TD_DOUBLE_SINGLE_TAP'
    if (state->count == 3) {
        if (state->interrupted || !state->pressed) return TD_TRIPLE_TAP;
        else return TD_TRIPLE_HOLD;
    } else return TD_UNKNOWN;
}

// Create an instance of 'td_tap_t' for the 'x' tap dance.
static td_tap_t xtap_state = {
    .is_press_action = true,
    .state = TD_NONE
};

void x_finished(tap_dance_state_t *state, void *user_data) {
    xtap_state.state = cur_dance(state);
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: register_code(KC_X); break;
        case TD_SINGLE_HOLD: register_code(KC_LCTL); break;
        case TD_DOUBLE_TAP: register_code(KC_ESC); break;
        case TD_DOUBLE_HOLD: register_code(KC_LALT); break;
        // Last case is for fast typing. Assuming your key is `f`:
        // For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
        // In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
        case TD_DOUBLE_SINGLE_TAP: tap_code(KC_X); register_code(KC_X); break;
        default: break;
    }
}

void x_reset(tap_dance_state_t *state, void *user_data) {
    switch (xtap_state.state) {
        case TD_SINGLE_TAP: unregister_code(KC_X); break;
        case TD_SINGLE_HOLD: unregister_code(KC_LCTL); break;
        case TD_DOUBLE_TAP: unregister_code(KC_ESC); break;
        case TD_DOUBLE_HOLD: unregister_code(KC_LALT); break;
        case TD_DOUBLE_SINGLE_TAP: unregister_code(KC_X); break;
        default: break;
    }
    xtap_state.state = TD_NONE;
}

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {

};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)

