#ifndef __MPU_H__
#define __MPU_H__

#include <libopencm3/cm3/mpu.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/gpio.h>

#define MPU_RASR_SIZE_32B (0x04UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_1KB (0x09UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_4KB (0x0BUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_8KB (0x0CUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_16KB (0x0DUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_32KB (0x0EUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_64KB (0x0FUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_128KB (0x10UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_256KB (0x11UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_512KB (0x12UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_1MB (0x13UL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_512MB (0x1CUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_SIZE_4GB (0x1FUL << MPU_RASR_SIZE_LSB)
#define MPU_RASR_ATTR_FLASH (MPU_RASR_ATTR_C)  // http://infocenter.arm.com/help/topic/com.arm.doc.dui0552a/BABDJJGF.html
#define MPU_RASR_ATTR_SRAM (MPU_RASR_ATTR_C | MPU_RASR_ATTR_S)
#define MPU_RASR_ATTR_PERIPH (MPU_RASR_ATTR_B | MPU_RASR_ATTR_S)
#define FLASH_BASE (0x08000000U)
#define SRAM_BASE (0x20000000U)

void mpu_off(void);
void mpu_conf(void);

#endif
