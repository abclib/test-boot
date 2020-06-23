#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/vector.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rng.h>

#include "abckey.h"
#include "btn.h"
#include "fla.h"
#include "led.h"
#include "mpu.h"
#include "msg.h"
#include "sign.h"
#include "usb.h"

static inline void setup(void) {
  // set SCB_CCR STKALIGN bit to make sure 8-byte stack alignment on exception
  // entry is in effect. This is here to comply with guidance from section 3.3.3
  // "Binary compatibility with other Cortex processors" of the ARM Cortex-M3
  // Processor Technical Reference Manual. According to section 4.4.2 and 4.4.7 of the
  // "STM32F10xxx/20xxx/21xxx/L1xxxx Cortex-M3 programming manual", STM32F2
  // series MCUs are r2p0 and always have this bit set on reset already.
  // Ensure 8-byte alignment of stack pointer on interrupts
  // Enabled by default on most Cortex-M parts, but not M3 r1
  SCB_CCR |= SCB_CCR_STKALIGN;
  // setup clock
  struct rcc_clock_scale clock = rcc_hse_8mhz_3v3[RCC_CLOCK_3V3_120MHZ];
  rcc_clock_setup_hse_3v3(&clock);
  // enable clock
  rcc_periph_clock_enable(RCC_GPIOA);  // USB
  rcc_periph_clock_enable(RCC_GPIOC);  // BTN
  rcc_periph_clock_enable(RCC_GPIOC);  // LED
  rcc_periph_clock_enable(RCC_RNG);    // RNG
  rcc_periph_clock_enable(RCC_OTGFS);  // OTG
  // enable CSS (Clock Security System)
  rcc_css_enable();
  // enable RNG
  rng_enable();
  // enable LED
  gpio_mode_setup(LED_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_GPIO_PINS);
  // enable BTN
  gpio_mode_setup(BTN_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_NONE, BTN_GPIO_PINS);
  // enable OTG_FS
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO10);
  gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO11 | GPIO12);
  gpio_set_af(GPIOA, GPIO_AF10, GPIO10 | GPIO11 | GPIO12);
  // clear USB OTG_FS peripheral dedicated RAM
  ramset((void *)0x50020000, (void *)0x50020500, 0);
}

static inline void load_core(void) {
  const image_header *hdr             = (const image_header *)FLASH_PTR(FLASH_A_HEAD);
  uint8_t             fingerprint[32] = {0};
  BOOL                signed_fw       = sign_fw(hdr, fingerprint);
  if (TRUE != signed_fw) return;
  //if (TRUE != check_fw(hdr)) return;
  mpu_off();
  ramset(_ram_start, _ram_end, 0);  // zero out SRAM
  const vector_table_t *ivt = (const vector_table_t *)FLASH_A_CORE;
  SCB_VTOR                  = (uint32_t)ivt;                     // relocate vector table
  __asm__ volatile("msr msp, %0" ::"r"(ivt->initial_sp_value));  // Set stack pointer
  ivt->reset();                                                  // Jump to address
  for (;;) {
    // Prevent compiler from generating stack protector code (which causes CPU fault because the stack is moved )
  }
}

int main(void) {
  setup();
  fla_conf();
  mpu_conf();
  if (FALSE == has_fw()) usb_loop();
  load_core();
  return 0;
}

void svc_handler_main(void) {}  // Do not delete.
