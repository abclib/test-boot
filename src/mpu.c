#include "mpu.h"

void mpu_off(void) {
  MPU_CTRL = 0;  // Disable MPU
  __asm__ volatile("dsb");
  __asm__ volatile("isb");
}

void mpu_conf(void) {
  MPU_CTRL = 0;  // Disable MPU
  // Note: later entries overwrite previous ones
  // Everything (0x00000000 - 0xFFFFFFFF, 4 GiB, read-write)
  MPU_RBAR = 0 | MPU_RBAR_VALID | (0 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_FLASH | MPU_RASR_SIZE_4GB | MPU_RASR_ATTR_AP_PRW_URW;
  // Flash (0x8007FE0 - 0x08007FFF, 32 B, no-access)
  MPU_RBAR = (FLASH_BASE + 0x7FE0) | MPU_RBAR_VALID | (1 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_FLASH | MPU_RASR_SIZE_32B | MPU_RASR_ATTR_AP_PNO_UNO;
  // SRAM (0x20000000 - 0x2001FFFF, read-write, execute never)
  MPU_RBAR = SRAM_BASE | MPU_RBAR_VALID | (2 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_SRAM | MPU_RASR_SIZE_128KB | MPU_RASR_ATTR_AP_PRW_URW | MPU_RASR_ATTR_XN;
  // Peripherals (0x40000000 - 0x4001FFFF, read-write, execute never)
  MPU_RBAR = PERIPH_BASE | MPU_RBAR_VALID | (3 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_PERIPH | MPU_RASR_SIZE_128KB | MPU_RASR_ATTR_AP_PRW_URW | MPU_RASR_ATTR_XN;
  // Peripherals (0x40020000 - 0x40023FFF, read-write, execute never)
  MPU_RBAR = 0x40020000 | MPU_RBAR_VALID | (4 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_PERIPH | MPU_RASR_SIZE_16KB | MPU_RASR_ATTR_AP_PRW_URW | MPU_RASR_ATTR_XN;
  // Don't enable DMA controller access
  // Peripherals (0x50000000 - 0x5007ffff, read-write, execute never)
  MPU_RBAR = 0x50000000 | MPU_RBAR_VALID | (5 << MPU_RBAR_REGION_LSB);
  MPU_RASR = MPU_RASR_ENABLE | MPU_RASR_ATTR_PERIPH | MPU_RASR_SIZE_512KB | MPU_RASR_ATTR_AP_PRW_URW | MPU_RASR_ATTR_XN;
  MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_HFNMIENA;  // Enable MPU
  SCB_SHCSR |= SCB_SHCSR_MEMFAULTENA;              // Enable memory fault handler
  __asm__ volatile("dsb");
  __asm__ volatile("isb");
}
