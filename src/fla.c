#include "fla.h"

void fla_conf(void) {
#if FLASH_LOCKED
  // Reference STM32F205 Flash programming manual revision 5
  // http://www.st.com/resource/en/programming_manual/cd00233952.pdf Section 2.6
  // Option bytes set RDP level 2 WRP for sectors 0 and 1 flash option control register matches
  if (((FLASH_OPT_KEY_1 & 0xFFEC) == 0xCCEC) &&
      ((FLASH_OPT_KEY_2 & 0xFFF) == 0xFFC) &&
      (FLASH_OPTCR == 0x0FFCCCED)) return;  // already set up correctly - bail out
  flash_unlock();
  for (int i = FLASH_SF_DATA; i <= FLASH_SL_DATA; i++) flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  flash_lock();
  flash_unlock_option_bytes();
  // Section 2.8.6 Flash option control register (FLASH_OPTCR)
  // Bits 31:28 Reserved, must be kept cleared.
  // Bits 27:16 nWRP: Not write protect: write protect bootloader code in
  // flash main memory sectors 0 and 1 (Section 2.3; table 2) Bits 15:8 RDP:
  // Read protect: level 2 chip read protection active Bits 7:5 USER: User
  // option bytes: no reset on standby, no reset on stop, software watchdog
  // Bit 4 Reserved, must be kept cleared.
  // Bits 3:2 BOR_LEV: BOR reset Level: BOR off
  // Bit 1 OPTSTRT: Option start: ignored by flash_program_option_bytes
  // Bit 0 OPTLOCK: Option lock: ignored by flash_program_option_bytes
  flash_program_option_bytes(0x0FFCCCEC);
  flash_lock_option_bytes();
#endif
}

void fla_on(void) {
  flash_wait_for_last_operation();
  flash_clear_status_flags();
  flash_unlock();
}

void fla_off(void) {
  flash_wait_for_last_operation();
  flash_lock();
}

void fla_erase_data(void) {
  fla_on();
  for (int i = FLASH_SF_DATA; i <= FLASH_SL_DATA; i++) {
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  fla_off();
}

void fla_erase_core(void) {
  fla_on();
  for (int i = FLASH_SF_CORE; i <= FLASH_SL_CORE; i++) {
    flash_erase_sector(i, FLASH_CR_PROGRAM_X32);
  }
  fla_off();
}
