#ifndef _LED_H_
#define _LED_H_

#include <libopencm3/stm32/gpio.h>

#include "abckey.h"
#include "util.h"

#define LED_GPIO_PORT GPIOC
#define LED_GPIO_PINS GPIO2

void led_toggle(void);
void led(BOOL open);

#endif
