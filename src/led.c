#include "led.h"

void led_toggle(void) {
  msleep(1000);
  gpio_toggle(LED_GPIO_PORT, LED_GPIO_PINS);
}

void led(BOOL open) {
  msleep(22);
  if (open) {
    gpio_set(LED_GPIO_PORT, LED_GPIO_PINS);
  } else {
    gpio_clear(LED_GPIO_PORT, LED_GPIO_PINS);
  }
}
