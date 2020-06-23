#include "util.h"

static const char *hexdigits = "0123456789ABCDEF";

void uint32hex(uint32_t num, char *str) {
  for (uint32_t i = 0; i < 8; i++) {
    str[i] = hexdigits[(num >> (28 - i * 4)) & 0xF];
  }
}

void int2hex(uint32_t num, char *str, size_t size) {
  for (size_t i = 0; i < size; i++) str[i] = (num >> (size - i - 1) * 8) & 0xFF;  // webees 201909080000
}

inline void delay(uint32_t wait) {
  while (--wait > 0) __asm__("nop");
}

uint32_t randint(uint32_t x, uint32_t y) {
  return random32() % y + x;
}

/*
 * Generates a delay of random length.
 * Use this to protect sensitive code against fault injection.
 */
void delay_random(void) {
  int          wait = random32() & 0xff;
  volatile int i    = 0;
  volatile int j    = wait;
  while (i < wait) {
    if (i + j != wait)
      shutdown();
    ++i;
    --j;
  }
  if (i != wait || j != 0) shutdown();  // Double-check loop completion.
}

void msleep(uint32_t wait) {
  while (--wait > 0) delay(15555);
}

BOOL ram_is_empty(const uint8_t *src, uint32_t len) {
  for (uint32_t i = 0; i < len; i++) {
    if (src[i]) return FALSE;
  }
  return TRUE;
}
