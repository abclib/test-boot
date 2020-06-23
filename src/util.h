#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>
#include <stdlib.h>

#include "abckey.h"
#include "rng.h"

void     uint32hex(uint32_t num, char *str);  // converts uint32 to hexa (8 digits)
void     int2hex(uint32_t num, char *str, size_t size);
void     delay(uint32_t wait);
void     delay_random(void);
void     msleep(uint32_t wait);
uint32_t randint(uint32_t x, uint32_t y);
BOOL     ram_is_empty(const uint8_t *src, uint32_t len);

#endif
