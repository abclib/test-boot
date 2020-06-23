
#ifndef __ABCKEY_H__
#define __ABCKEY_H__

#include <stdint.h>

#define __MANUFACTURER__ "ABCKEY.COM"
#define __PRODUCT__ "ABCKEY BOOT"
#define __MODEL__ "1"

#define VERSION_MAJOR_CHAR "\x00"
#define VERSION_MINOR_CHAR "\x00"
#define VERSION_PATCH_CHAR "\x01"

typedef enum {
  TRUE  = 0x55555555U,
  FALSE = !TRUE
} BOOL;

//*********************************************************
// defined in abckey.S
extern void ramset(void* start, void* stop, uint32_t val);
extern void __attribute__((noreturn)) shutdown(void);
//*********************************************************
// defined in abckey.ld
extern uint32_t __stack_chk_guard;
extern uint8_t  _ram_start[], _ram_end[];
//*********************************************************
#endif
