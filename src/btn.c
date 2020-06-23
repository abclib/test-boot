#include "btn.h"

BOOL btn_init() {
  return TRUE;
}

// The physical state of the touch button: down, up
static inline BTN_STATUS btn_status(void) {
  BTN_STATUS old_status   = BTN_S_NULL;
  BTN_STATUS new_status   = BTN_S_NULL;
  size_t     status_times = 0;
  for (;;) {
    new_status = gpio_get(BTN_GPIO_PORT, BTN_GPIO_PINS) == 0 ? BTN_S_DOWN : BTN_S_UP;
    msleep(22);  // Avoid jitter
    if (new_status == old_status) {
      status_times++;
    } else {
      old_status   = new_status;
      status_times = 1;
    }
    if (status_times > 3) return old_status;  // The state is determined by the consistency of 3 samples.
  }
}

// The logical state of the touch button
BOOL wait_btn(BTN_OK_TIMER btn_ok_timer) {
  if (BTN_OK_0 == btn_ok_timer) return TRUE;
  usb_send(Type_ButtonRequest, Message_Null);
  uint32_t timeout  = btn_ok_timer + TOUCH_BTN_TIMEOUT;
  int32_t  timer    = -1;                                      // Unit: second
  int32_t  timer_ok = randint(btn_ok_timer, TOUCH_BTN_RANGE);  // Random time
  BOOL     result   = FALSE;
  for (;;) {
    led_toggle();
    timeout--;
    if (timeout == 0) break;
    BTN_STATUS status = btn_status();
    if (timer == -1 && BTN_S_UP == status) timer = 0;
    if (timer == -1 && BTN_S_DOWN == status) continue;
    if (timer > 0 && BTN_S_UP == status) break;
    if (BTN_S_DOWN == status) {
      timer++;
      timeout++;
    }
    if (timer >= timer_ok) {
      result = TRUE;
      break;
    }
  }
  if (TRUE == result) {
    usb_send(Type_Success, Message_Null);
  } else {
    usb_send(Type_Failure, Message_Null);
  }
  led(FALSE);
  return result;
}
