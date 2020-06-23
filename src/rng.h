#ifndef __RNG_H__
#define __RNG_H__

#include <stdlib.h>

#include <libopencm3/cm3/common.h>
#include <libopencm3/stm32/f2/rng.h>

uint32_t random32(void);

#endif
