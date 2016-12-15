#include "quantum.h"

__attribute__ ((weak))
bool process_action_kb(keyrecord_t *record) {
  return true;
}

__attribute__ ((weak))
bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
  return process_record_user(keycode, record);
}

__attribute__ ((weak))
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  return true;
}

void reset_keyboard(void) {
  clear_keyboard();
#ifdef AUDIO_ENABLE
  stop_all_notes();
  shutdown_user();
#endif
  wait_ms(250);
#ifdef CATERINA_BOOTLOADER
  *(uint16_t *)0x0800 = 0x7777; // these two are a-star-specific
#endif
  bootloader_jump();
}

// Shift / paren setup

#ifndef LSPO_KEY
  #define LSPO_KEY KC_9
#endif
#ifndef RSPC_KEY
  #define RSPC_KEY KC_0
#endif

static bool shift_interrupted[2] = {0, 0};

bool process_record_quantum(keyrecord_t *record) {

  /* This gets the keycode from the key pressed */
  keypos_t key = record->event.key;
  uint16_t keycode;

  #if !defined(NO_ACTION_LAYER) && defined(PREVENT_STUCK_MODIFIERS)
    uint8_t layer;

    if (record->event.pressed) {
      layer = layer_switch_get_layer(key);
      update_source_layers_cache(key, layer);
    } else {
      layer = read_source_layers_cache(key);
    }
    keycode = keymap_key_to_keycode(layer, key);
  #else
    keycode = keymap_key_to_keycode(layer_switch_get_layer(key), key);
  #endif

    // This is how you use actions here
    // if (keycode == KC_LEAD) {
    //   action_t action;
    //   action.code = ACTION_DEFAULT_LAYER_SET(0);
    //   process_action(record, action);
    //   return false;
    // }

  if (!(
    process_record_kb(keycode, record) &&
  #ifdef MIDI_ENABLE
    process_midi(keycode, record) &&
  #endif
  #ifdef AUDIO_ENABLE
    process_music(keycode, record) &&
  #endif
  #ifdef TAP_DANCE_ENABLE
    process_tap_dance(keycode, record) &&
  #endif
  #ifndef DISABLE_LEADER
    process_leader(keycode, record) &&
  #endif
  #ifndef DISABLE_CHORDING
    process_chording(keycode, record) &&
  #endif
  #ifdef UNICODE_ENABLE
    process_unicode(keycode, record) &&
  #endif
      true)) {
    return false;
  }

  // Shift / paren setup

  switch(keycode) {
    case RESET:
      if (record->event.pressed) {
        reset_keyboard();
      }
	  return false;
      break;
    case DEBUG:
      if (record->event.pressed) {
          print("\nDEBUG: enabled.\n");
          debug_enable = true;
      }
	  return false;
      break;
	#ifdef RGBLIGHT_ENABLE
	case RGB_TOG:
		if (record->event.pressed) {
			rgblight_toggle();
      }
	  return false;
      break;
	case RGB_MOD:
		if (record->event.pressed) {
			rgblight_step();
      }
	  return false;
      break;
	case RGB_HUI:
		if (record->event.pressed) {
			rgblight_increase_hue();
      }
	  return false;
      break;
	case RGB_HUD:
		if (record->event.pressed) {
			rgblight_decrease_hue();
      }
	  return false;
      break;
	case RGB_SAI:
		if (record->event.pressed) {
			rgblight_increase_sat();
      }
	  return false;
      break;
	case RGB_SAD:
		if (record->event.pressed) {
			rgblight_decrease_sat();
      }
	  return false;
      break;
	case RGB_VAI:
		if (record->event.pressed) {
			rgblight_increase_val();
      }
	  return false;
      break;
	case RGB_VAD:
		if (record->event.pressed) {
			rgblight_decrease_val();
      }
	  return false;
      break;
	#endif
    case MAGIC_SWAP_CONTROL_CAPSLOCK ... MAGIC_UNSWAP_ALT_GUI:
      if (record->event.pressed) {
        // MAGIC actions (BOOTMAGIC without the boot)
        if (!eeconfig_is_enabled()) {
            eeconfig_init();
        }
        /* keymap config */
        keymap_config.raw = eeconfig_read_keymap();
        if (keycode == MAGIC_SWAP_CONTROL_CAPSLOCK) {
            keymap_config.swap_control_capslock = 1;
        } else if (keycode == MAGIC_CAPSLOCK_TO_CONTROL) {
            keymap_config.capslock_to_control = 1;
        } else if (keycode == MAGIC_SWAP_LALT_LGUI) {
            keymap_config.swap_lalt_lgui = 1;
        } else if (keycode == MAGIC_SWAP_RALT_RGUI) {
            keymap_config.swap_ralt_rgui = 1;
        } else if (keycode == MAGIC_NO_GUI) {
            keymap_config.no_gui = 1;
        } else if (keycode == MAGIC_SWAP_GRAVE_ESC) {
            keymap_config.swap_grave_esc = 1;
        } else if (keycode == MAGIC_SWAP_BACKSLASH_BACKSPACE) {
            keymap_config.swap_backslash_backspace = 1;
        } else if (keycode == MAGIC_HOST_NKRO) {
            keymap_config.nkro = 1;
        } else if (keycode == MAGIC_SWAP_ALT_GUI) {
            keymap_config.swap_lalt_lgui = 1;
            keymap_config.swap_ralt_rgui = 1;
        }
        /* UNs */
        else if (keycode == MAGIC_UNSWAP_CONTROL_CAPSLOCK) {
            keymap_config.swap_control_capslock = 0;
        } else if (keycode == MAGIC_UNCAPSLOCK_TO_CONTROL) {
            keymap_config.capslock_to_control = 0;
        } else if (keycode == MAGIC_UNSWAP_LALT_LGUI) {
            keymap_config.swap_lalt_lgui = 0;
        } else if (keycode == MAGIC_UNSWAP_RALT_RGUI) {
            keymap_config.swap_ralt_rgui = 0;
        } else if (keycode == MAGIC_UNNO_GUI) {
            keymap_config.no_gui = 0;
        } else if (keycode == MAGIC_UNSWAP_GRAVE_ESC) {
            keymap_config.swap_grave_esc = 0;
        } else if (keycode == MAGIC_UNSWAP_BACKSLASH_BACKSPACE) {
            keymap_config.swap_backslash_backspace = 0;
        } else if (keycode == MAGIC_UNHOST_NKRO) {
            keymap_config.nkro = 0;
        } else if (keycode == MAGIC_UNSWAP_ALT_GUI) {
            keymap_config.swap_lalt_lgui = 0;
            keymap_config.swap_ralt_rgui = 0;
        }
        eeconfig_update_keymap(keymap_config.raw);
        return false;
      }
      break;
    case KC_LSPO: {
      if (record->event.pressed) {
        shift_interrupted[0] = false;
        register_mods(MOD_BIT(KC_LSFT));
      }
      else {
        #ifdef DISABLE_SPACE_CADET_ROLLOVER
          if (get_mods() & MOD_BIT(KC_RSFT)) {
            shift_interrupted[0] = true;
            shift_interrupted[1] = true;
          }
        #endif
        if (!shift_interrupted[0]) {
          register_code(LSPO_KEY);
          unregister_code(LSPO_KEY);
        }
        unregister_mods(MOD_BIT(KC_LSFT));
      }
      return false;
      break;
    }

    case KC_RSPC: {
      if (record->event.pressed) {
        shift_interrupted[1] = false;
        register_mods(MOD_BIT(KC_RSFT));
      }
      else {
        #ifdef DISABLE_SPACE_CADET_ROLLOVER
          if (get_mods() & MOD_BIT(KC_LSFT)) {
            shift_interrupted[0] = true;
            shift_interrupted[1] = true;
          }
        #endif
        if (!shift_interrupted[1]) {
          register_code(RSPC_KEY);
          unregister_code(RSPC_KEY);
        }
        unregister_mods(MOD_BIT(KC_RSFT));
      }
      return false;
      break;
    }
    default: {
      shift_interrupted[0] = true;
      shift_interrupted[1] = true;
      break;
    }
  }

  return process_action_kb(record);
}

const bool ascii_to_qwerty_shift_lut[0x80] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0
};

const uint8_t ascii_to_qwerty_keycode_lut[0x80] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 0,
    KC_BSPC, KC_TAB, KC_ENT, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, KC_ESC, 0, 0, 0, 0,
    KC_SPC, KC_1, KC_QUOT, KC_3, KC_4, KC_5, KC_7, KC_QUOT,
    KC_9, KC_0, KC_8, KC_EQL, KC_COMM, KC_MINS, KC_DOT, KC_SLSH,
    KC_0, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7,
    KC_8, KC_9, KC_SCLN, KC_SCLN, KC_COMM, KC_EQL, KC_DOT, KC_SLSH,
    KC_2, KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G,
    KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O,
    KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W,
    KC_X, KC_Y, KC_Z, KC_LBRC, KC_BSLS, KC_RBRC, KC_6, KC_MINS,
    KC_GRV, KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G,
    KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O,
    KC_P, KC_Q, KC_R, KC_S, KC_T, KC_U, KC_V, KC_W,
    KC_X, KC_Y, KC_Z, KC_LBRC, KC_BSLS, KC_RBRC, KC_GRV, KC_DEL
};

/* for users whose OSes are set to Colemak */
#if 0
#include "keymap_colemak.h"

const bool ascii_to_colemak_shift_lut[0x80] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 1, 1, 1, 1, 1, 1, 0,
    1, 1, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 1, 0, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 1, 0, 0, 0, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 1, 1, 1, 1, 0
};

const uint8_t ascii_to_colemak_keycode_lut[0x80] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 0,
    KC_BSPC, KC_TAB, KC_ENT, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, KC_ESC, 0, 0, 0, 0,
    KC_SPC, KC_1, KC_QUOT, KC_3, KC_4, KC_5, KC_7, KC_QUOT,
    KC_9, KC_0, KC_8, KC_EQL, KC_COMM, KC_MINS, KC_DOT, KC_SLSH,
    KC_0, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7,
    KC_8, KC_9, CM_SCLN, CM_SCLN, KC_COMM, KC_EQL, KC_DOT, KC_SLSH,
    KC_2, CM_A, CM_B, CM_C, CM_D, CM_E, CM_F, CM_G,
    CM_H, CM_I, CM_J, CM_K, CM_L, CM_M, CM_N, CM_O,
    CM_P, CM_Q, CM_R, CM_S, CM_T, CM_U, CM_V, CM_W,
    CM_X, CM_Y, CM_Z, KC_LBRC, KC_BSLS, KC_RBRC, KC_6, KC_MINS,
    KC_GRV, CM_A, CM_B, CM_C, CM_D, CM_E, CM_F, CM_G,
    CM_H, CM_I, CM_J, CM_K, CM_L, CM_M, CM_N, CM_O,
    CM_P, CM_Q, CM_R, CM_S, CM_T, CM_U, CM_V, CM_W,
    CM_X, CM_Y, CM_Z, KC_LBRC, KC_BSLS, KC_RBRC, KC_GRV, KC_DEL
};

#endif

void send_string(const char *str) {
    while (1) {
        uint8_t keycode;
        uint8_t ascii_code = pgm_read_byte(str);
        if (!ascii_code) break;
        keycode = pgm_read_byte(&ascii_to_qwerty_keycode_lut[ascii_code]);
        if (pgm_read_byte(&ascii_to_qwerty_shift_lut[ascii_code])) {
            register_code(KC_LSFT);
            register_code(keycode);
            unregister_code(keycode);
            unregister_code(KC_LSFT);
        }
        else {
            register_code(keycode);
            unregister_code(keycode);
        }
        ++str;
    }
}

void update_tri_layer(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void tap_random_base64(void) {
  #if defined(__AVR_ATmega32U4__)
    uint8_t key = (TCNT0 + TCNT1 + TCNT3 + TCNT4) % 64;
  #else
    uint8_t key = rand() % 64;
  #endif
  switch (key) {
    case 0 ... 25:
      register_code(KC_LSFT);
      register_code(key + KC_A);
      unregister_code(key + KC_A);
      unregister_code(KC_LSFT);
      break;
    case 26 ... 51:
      register_code(key - 26 + KC_A);
      unregister_code(key - 26 + KC_A);
      break;
    case 52:
      register_code(KC_0);
      unregister_code(KC_0);
      break;
    case 53 ... 61:
      register_code(key - 53 + KC_1);
      unregister_code(key - 53 + KC_1);
      break;
    case 62:
      register_code(KC_LSFT);
      register_code(KC_EQL);
      unregister_code(KC_EQL);
      unregister_code(KC_LSFT);
      break;
    case 63:
      register_code(KC_SLSH);
      unregister_code(KC_SLSH);
      break;
  }
}

void matrix_init_quantum() {
  #ifdef BACKLIGHT_ENABLE
    backlight_init_ports();
  #endif
  matrix_init_kb();
}

void matrix_scan_quantum() {
  #ifdef AUDIO_ENABLE
    matrix_scan_music();
  #endif

  #ifdef TAP_DANCE_ENABLE
    matrix_scan_tap_dance();
  #endif
  matrix_scan_kb();
}

#if defined(BACKLIGHT_ENABLE) && defined(BACKLIGHT_PIN)

static const uint8_t backlight_pin = BACKLIGHT_PIN;

#if BACKLIGHT_PIN == B7
#  define COM1x1 COM1C1
#  define OCR1x  OCR1C
#elif BACKLIGHT_PIN == B6
#  define COM1x1 COM1B1
#  define OCR1x  OCR1B
#elif BACKLIGHT_PIN == B5
#  define COM1x1 COM1A1
#  define OCR1x  OCR1A
#else
#  error "Backlight pin not supported - use B5, B6, or B7"
#endif

__attribute__ ((weak))
void backlight_init_ports(void)
{

  // Setup backlight pin as output and output low.
  // DDRx |= n
  _SFR_IO8((backlight_pin >> 4) + 1) |= _BV(backlight_pin & 0xF);
  // PORTx &= ~n
  _SFR_IO8((backlight_pin >> 4) + 2) &= ~_BV(backlight_pin & 0xF);

  // Use full 16-bit resolution.
  ICR1 = 0xFFFF;

  // I could write a wall of text here to explain... but TL;DW
  // Go read the ATmega32u4 datasheet.
  // And this: http://blog.saikoled.com/post/43165849837/secret-konami-cheat-code-to-high-resolution-pwm-on

  // Pin PB7 = OCR1C (Timer 1, Channel C)
  // Compare Output Mode = Clear on compare match, Channel C = COM1C1=1 COM1C0=0
  // (i.e. start high, go low when counter matches.)
  // WGM Mode 14 (Fast PWM) = WGM13=1 WGM12=1 WGM11=1 WGM10=0
  // Clock Select = clk/1 (no prescaling) = CS12=0 CS11=0 CS10=1

  TCCR1A = _BV(COM1x1) | _BV(WGM11); // = 0b00001010;
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // = 0b00011001;

  backlight_init();
  #ifdef BACKLIGHT_BREATHING
    breathing_defaults();
  #endif
}

__attribute__ ((weak))
void backlight_set(uint8_t level)
{
  // Prevent backlight blink on lowest level
  // PORTx &= ~n
  _SFR_IO8((backlight_pin >> 4) + 2) &= ~_BV(backlight_pin & 0xF);

  if ( level == 0 ) {
    // Turn off PWM control on backlight pin, revert to output low.
    TCCR1A &= ~(_BV(COM1x1));
    OCR1x = 0x0;
  } else if ( level == BACKLIGHT_LEVELS ) {
    // Turn on PWM control of backlight pin
    TCCR1A |= _BV(COM1x1);
    // Set the brightness
    OCR1x = 0xFFFF;
  } else {
    // Turn on PWM control of backlight pin
    TCCR1A |= _BV(COM1x1);
    // Set the brightness
    OCR1x = 0xFFFF >> ((BACKLIGHT_LEVELS - level) * ((BACKLIGHT_LEVELS + 1) / 2));
  }

  #ifdef BACKLIGHT_BREATHING
    breathing_intensity_default();
  #endif
}


#ifdef BACKLIGHT_BREATHING

#define BREATHING_NO_HALT  0
#define BREATHING_HALT_OFF 1
#define BREATHING_HALT_ON  2

static uint8_t breath_intensity;
static uint8_t breath_speed;
static uint16_t breathing_index;
static uint8_t breathing_halt;

void breathing_enable(void)
{
    if (get_backlight_level() == 0)
    {
        breathing_index = 0;
    }
    else
    {
        // Set breathing_index to be at the midpoint (brightest point)
        breathing_index = 0x20 << breath_speed;
    }

    breathing_halt = BREATHING_NO_HALT;

    // Enable breathing interrupt
    TIMSK1 |= _BV(OCIE1A);
}

void breathing_pulse(void)
{
    if (get_backlight_level() == 0)
    {
        breathing_index = 0;
    }
    else
    {
        // Set breathing_index to be at the midpoint + 1 (brightest point)
        breathing_index = 0x21 << breath_speed;
    }

    breathing_halt = BREATHING_HALT_ON;

    // Enable breathing interrupt
    TIMSK1 |= _BV(OCIE1A);
}

void breathing_disable(void)
{
    // Disable breathing interrupt
    TIMSK1 &= ~_BV(OCIE1A);
    backlight_set(get_backlight_level());
}

void breathing_self_disable(void)
{
    if (get_backlight_level() == 0)
    {
        breathing_halt = BREATHING_HALT_OFF;
    }
    else
    {
        breathing_halt = BREATHING_HALT_ON;
    }

    //backlight_set(get_backlight_level());
}

void breathing_toggle(void)
{
    if (!is_breathing())
    {
        if (get_backlight_level() == 0)
        {
            breathing_index = 0;
        }
        else
        {
            // Set breathing_index to be at the midpoint + 1 (brightest point)
            breathing_index = 0x21 << breath_speed;
        }

        breathing_halt = BREATHING_NO_HALT;
    }

    // Toggle breathing interrupt
    TIMSK1 ^= _BV(OCIE1A);

    // Restore backlight level
    if (!is_breathing())
    {
        backlight_set(get_backlight_level());
    }
}

bool is_breathing(void)
{
    return (TIMSK1 && _BV(OCIE1A));
}

void breathing_intensity_default(void)
{
    //breath_intensity = (uint8_t)((uint16_t)100 * (uint16_t)get_backlight_level() / (uint16_t)BACKLIGHT_LEVELS);
    breath_intensity = ((BACKLIGHT_LEVELS - get_backlight_level()) * ((BACKLIGHT_LEVELS + 1) / 2));
}

void breathing_intensity_set(uint8_t value)
{
    breath_intensity = value;
}

void breathing_speed_default(void)
{
    breath_speed = 4;
}

void breathing_speed_set(uint8_t value)
{
    bool is_breathing_now = is_breathing();
    uint8_t old_breath_speed = breath_speed;

    if (is_breathing_now)
    {
        // Disable breathing interrupt
        TIMSK1 &= ~_BV(OCIE1A);
    }

    breath_speed = value;

    if (is_breathing_now)
    {
        // Adjust index to account for new speed
        breathing_index = (( (uint8_t)( (breathing_index) >> old_breath_speed ) ) & 0x3F) << breath_speed;

        // Enable breathing interrupt
        TIMSK1 |= _BV(OCIE1A);
    }

}

void breathing_speed_inc(uint8_t value)
{
    if ((uint16_t)(breath_speed - value) > 10 )
    {
        breathing_speed_set(0);
    }
    else
    {
        breathing_speed_set(breath_speed - value);
    }
}

void breathing_speed_dec(uint8_t value)
{
    if ((uint16_t)(breath_speed + value) > 10 )
    {
        breathing_speed_set(10);
    }
    else
    {
        breathing_speed_set(breath_speed + value);
    }
}

void breathing_defaults(void)
{
    breathing_intensity_default();
    breathing_speed_default();
    breathing_halt = BREATHING_NO_HALT;
}

/* Breathing Sleep LED brighness(PWM On period) table
 * (64[steps] * 4[duration]) / 64[PWM periods/s] = 4 second breath cycle
 *
 * http://www.wolframalpha.com/input/?i=%28sin%28+x%2F64*pi%29**8+*+255%2C+x%3D0+to+63
 * (0..63).each {|x| p ((sin(x/64.0*PI)**8)*255).to_i }
 */
static const uint8_t breathing_table[64] PROGMEM = {
  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   2,   4,   6,  10,
 15,  23,  32,  44,  58,  74,  93, 113, 135, 157, 179, 199, 218, 233, 245, 252,
255, 252, 245, 233, 218, 199, 179, 157, 135, 113,  93,  74,  58,  44,  32,  23,
 15,  10,   6,   4,   2,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

ISR(TIMER1_COMPA_vect)
{
    // OCR1x = (pgm_read_byte(&breathing_table[ ( (uint8_t)( (breathing_index++) >> breath_speed ) ) & 0x3F ] )) * breath_intensity;


    uint8_t local_index = ( (uint8_t)( (breathing_index++) >> breath_speed ) ) & 0x3F;

    if (((breathing_halt == BREATHING_HALT_ON) && (local_index == 0x20)) || ((breathing_halt == BREATHING_HALT_OFF) && (local_index == 0x3F)))
    {
        // Disable breathing interrupt
        TIMSK1 &= ~_BV(OCIE1A);
    }

    OCR1x = (uint16_t)(((uint16_t)pgm_read_byte(&breathing_table[local_index]) * 257)) >> breath_intensity;

}



#endif // breathing

#else // backlight

__attribute__ ((weak))
void backlight_init_ports(void)
{

}

__attribute__ ((weak))
void backlight_set(uint8_t level)
{

}

#endif // backlight



__attribute__ ((weak))
void led_set_user(uint8_t usb_led) {

}

__attribute__ ((weak))
void led_set_kb(uint8_t usb_led) {
    led_set_user(usb_led);
}

__attribute__ ((weak))
void led_init_ports(void)
{

}

__attribute__ ((weak))
void led_set(uint8_t usb_led)
{

  // Example LED Code
  //
    // // Using PE6 Caps Lock LED
    // if (usb_led & (1<<USB_LED_CAPS_LOCK))
    // {
    //     // Output high.
    //     DDRE |= (1<<6);
    //     PORTE |= (1<<6);
    // }
    // else
    // {
    //     // Output low.
    //     DDRE &= ~(1<<6);
    //     PORTE &= ~(1<<6);
    // }

  led_set_kb(usb_led);
}


//------------------------------------------------------------------------------
// Override these functions in your keymap file to play different tunes on
// different events such as startup and bootloader jump

__attribute__ ((weak))
void startup_user() {}

__attribute__ ((weak))
void shutdown_user() {}

//------------------------------------------------------------------------------
