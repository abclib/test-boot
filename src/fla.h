#ifndef FLASH_H
#define FLASH_H

/*
   name    |          range          |  size   |  type
-----------+-------------------------+---------+--------
 Sector  0 | 0x08000000 - 0x08003FFF |  16 KiB |  boot
 Sector  1 | 0x08004000 - 0x08007FFF |  16 KiB |  boot
-----------+-------------------------+---------+--------
 Sector  2 | 0x08008000 - 0x0800BFFF |  16 KiB |  data
 Sector  3 | 0x0800C000 - 0x0800FFFF |  16 KiB |  data
-----------+-------------------------+---------+--------
 Sector  4 | 0x08010000 - 0x0801FFFF |  64 KiB |  core
 Sector  5 | 0x08020000 - 0x0803FFFF | 128 KiB |  core
 Sector  6 | 0x08040000 - 0x0805FFFF | 128 KiB |  core
 Sector  7 | 0x08060000 - 0x0807FFFF | 128 KiB |  core
 */

#include <libopencm3/stm32/flash.h>

#define FLASH_PTR(x) (const uint8_t *)(x)

// A = ADDRESS
// S = SIZE
// SF = SECTOR FIRST
// SL = SECTOR LAST
#define FW_CHUNK_SIZE 65536  // 64kb = 65536b

#define FLASH_A (0x08000000)
#define FLASH_S (1024 * 1024)

#define FLASH_A_BOOT (FLASH_A)
#define FLASH_S_BOOT (0x8000)  // 32kb = 32768b

#define FLASH_A_DATA (FLASH_A_BOOT + FLASH_S_BOOT)
#define FLASH_S_DATA (0x8000)  // 32kb = 32768b

#define FLASH_A_HEAD (FLASH_A_DATA + FLASH_S_DATA)
#define FLASH_S_HEAD (0x400)  // 1kb = 1024b

#define FLASH_A_CORE (FLASH_A_HEAD + FLASH_S_HEAD)
#define FLASH_S_CORE (FLASH_S - (FLASH_A_CORE - FLASH_A))

#define FLASH_SF_BOOT 0
#define FLASH_SL_BOOT 1

#define FLASH_SF_DATA 2
#define FLASH_SL_DATA 3

#define FLASH_SF_CORE 4
#define FLASH_SL_CORE 11

#define FLASH_OPT_KEY_1 (*(const uint64_t *)0x1FFFC000)
#define FLASH_OPT_KEY_2 (*(const uint64_t *)0x1FFFC008)

void fla_conf(void);
void fla_on(void);
void fla_off(void);
void fla_erase_data(void);
void fla_erase_core(void);

#endif
