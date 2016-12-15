#ifndef QUANTUM_H
#define QUANTUM_H

#if defined(__AVR__)
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#endif
#include "wait.h"
#include "matrix.h"
#include "keymap.h"
#ifdef BACKLIGHT_ENABLE
    #include "backlight.h"
#endif
#ifdef RGBLIGHT_ENABLE
  #include "rgblight.h"
#endif

#include "action_layer.h"
#include "eeconfig.h"
#include <stddef.h>
#include "bootloader.h"
#include "timer.h"
#include "config_common.h"
#include "led.h"
#include "action_util.h"
#include <stdlib.h>


extern uint32_t default_layer_state;

#ifndef NO_ACTION_LAYER
	extern uint32_t layer_state;
#endif

#ifdef MIDI_ENABLE
	#include <lufa.h>
	#include "process_midi.h"
#endif

#ifdef AUDIO_ENABLE
 	#include "audio.h"
	#include "process_music.h"
#endif

#ifndef DISABLE_LEADER
	#include "process_leader.h"
#endif

#define DISABLE_CHORDING
#ifndef DISABLE_CHORDING
	#include "process_chording.h"
#endif

#ifdef UNICODE_ENABLE
	#include "process_unicode.h"
#endif

#include "process_tap_dance.h"

#define SEND_STRING(str) send_string(PSTR(str))
void send_string(const char *str);

// For tri-layer
void update_tri_layer(uint8_t layer1, uint8_t layer2, uint8_t layer3);

void tap_random_base64(void);

#define IS_LAYER_ON(layer)  (layer_state & (1UL << (layer)))
#define IS_LAYER_OFF(layer) (~layer_state & (1UL << (layer)))

void matrix_init_kb(void);
void matrix_scan_kb(void);
void matrix_init_user(void);
void matrix_scan_user(void);
bool process_action_kb(keyrecord_t *record);
bool process_record_kb(uint16_t keycode, keyrecord_t *record);
bool process_record_user(uint16_t keycode, keyrecord_t *record);

void reset_keyboard(void);

void startup_user(void);
void shutdown_user(void);

#ifdef BACKLIGHT_ENABLE
void backlight_init_ports(void);

#ifdef BACKLIGHT_BREATHING
void breathing_enable(void);
void breathing_pulse(void);
void breathing_disable(void);
void breathing_self_disable(void);
void breathing_toggle(void);
bool is_breathing(void);

void breathing_defaults(void);
void breathing_intensity_default(void);
void breathing_speed_default(void);
void breathing_speed_set(uint8_t value);
void breathing_speed_inc(uint8_t value);
void breathing_speed_dec(uint8_t value);
#endif

#endif

void led_set_user(uint8_t usb_led);
void led_set_kb(uint8_t usb_led);

#endif
