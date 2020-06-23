#ifndef _BTN_H_
#define _BTN_H_

#include "string.h"

#include <libopencm3/stm32/gpio.h>

#include "abckey.h"
#include "led.h"
#include "rng.h"
#include "usb.h"
#include "util.h"

#define BTN_GPIO_PORT GPIOC
#define BTN_GPIO_PINS GPIO3

#define TOUCH_BTN_TIMEOUT 9  // Unit: second
#define TOUCH_BTN_RANGE 5    // Unit: second

typedef enum {
  BTN_S_NULL = 0,
  BTN_S_DOWN = 1,
  BTN_S_UP   = 2,
} BTN_STATUS;  // Button Physical State

typedef enum {
  BTN_OK_0  = 0,  // Return TRUE directly.
  BTN_OK_2  = 2,
  BTN_OK_3  = 3,
  BTN_OK_5  = 5,
  BTN_OK_7  = 7,
  BTN_OK_11 = 11,
  BTN_OK_13 = 13,
  BTN_OK_17 = 17,
  BTN_OK_19 = 19,
} BTN_OK_TIMER;  // Button Security Level

BOOL wait_btn(BTN_OK_TIMER btn_ok_timer);
BOOL btn_init(void);

#endif
