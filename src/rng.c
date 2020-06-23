
#include "rng.h"

uint32_t random32(void) {
  uint32_t u32_arr[32] = {0};
  for (size_t i = 0; i < 32; i++) {
    u32_arr[i] = rng_get_random_blocking();  // 32 random numbers are generated in succession
  }
  uint32_t x = u32_arr[0] % 31 + 1;  // the 1nd to 31th array indexes are used randomly.
  return u32_arr[x];
}
