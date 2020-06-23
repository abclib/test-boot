PREFIX    ?= arm-none-eabi-
CC        := $(PREFIX)gcc
LD        := $(PREFIX)gcc
OBJCOPY   := $(PREFIX)objcopy
OBJDUMP   := $(PREFIX)objdump
AR        := $(PREFIX)ar
AS        := $(PREFIX)as

OPT_FLAGS ?= -O3
DBG_FLAGS ?= -g -DNDEBUG
CPU_FLAGS ?= -mcpu=cortex-m3 -mthumb
FPU_FLAGS ?= -msoft-float
###########################################################
C_FLAGS   += $(OPT_FLAGS) \
             $(DBG_FLAGS) \
             $(CPU_FLAGS) \
             $(FPU_FLAGS) \
             -std=gnu11 \
             -W \
             -Wall \
             -Wextra \
             -Wimplicit-function-declaration \
             -Wredundant-decls \
             -Wstrict-prototypes \
             -Wundef \
             -Wshadow \
             -Wpointer-arith \
             -Wformat \
             -Wreturn-type \
             -Wsign-compare \
             -Wmultichar \
             -Wformat-nonliteral \
             -Winit-self \
             -Wuninitialized \
             -Wformat-security \
             -Wno-missing-braces \
             -Werror \
             -fno-common \
             -fno-exceptions \
             -fvisibility=internal \
             -ffunction-sections \
             -fdata-sections \
             -fstack-protector-all \
             -DSTM32F2 \
             -DCONFIDENTIAL='__attribute__((section("confidential")))' \
             -DRAND_PLATFORM_INDEPENDENT=1 \
             -I$(SRC_DIR) \
             -I$(_CRYPTO_) \
             -I$(_LIBOPENCM3_)/include \
###########################################################
LD_FLAGS  += -L$(SRC_DIR) \
               $(DBG_FLAGS) \
               $(CPU_FLAGS) \
               $(FPU_FLAGS)
###########################################################
LD_FLAGS  += --static \
             -Wl,--start-group \
             -lc \
             -lgcc \
             -lnosys \
             -Wl,--end-group \
             -nostartfiles \
             -Wl,--gc-sections \
             -T$(LD_SCRIPT) \
             -L$(_LIBOPENCM3_)/lib

_LD       += -lopencm3_stm32f2
LIB_DEPS  += $(_LIBOPENCM3_)/lib/libopencm3_stm32f2.a
###########################################################
ifneq ($(V),1)
Q := @
endif

ifeq ($(FLASH_LOCKED), 0)
C_FLAGS   += -DFLASH_LOCKED=0
CPU_FLAGS += -DFLASH_LOCKED=0
$(info FLASH_LOCKED=0)
else
C_FLAGS   += -DFLASH_LOCKED=1
CPU_FLAGS += -DFLASH_LOCKED=1
$(info FLASH_LOCKED=1)
endif

ifeq ($(DEBUG),1)
C_FLAGS += -DDEBUG_RNG=1
else
C_FLAGS += -DDEBUG_RNG=0
endif
###########################################################
all:
	find . -name "*.elf" | xargs rm -f
	find . -name "*.bin" | xargs rm -f
	@+make -C $(_LIBOPENCM3_) lib/stm32/f2
	@+make $(TARGET).bin

$(TARGET).bin: $(TARGET).elf
	@printf "  OBJCOPY $@\n"
	$(Q)$(OBJCOPY) -Obinary $(TARGET).elf $(TARGET).bin

$(TARGET).elf: $(_O) $(LD_SCRIPT) $(LIB_DEPS)
	@printf "  LD      $@\n"
	$(Q)$(LD) -o $(TARGET).elf $(addprefix $(TMP_DIR)/,$(_O)) $(_LD) $(LD_FLAGS)

%.o: %.S
	@if [ ! -d $(TMP_DIR)/$(@D) ]; then mkdir -p $(TMP_DIR)/$(@D); fi;
	@printf "  AS      $@\n"
	$(Q)$(CC) $(CPU_FLAGS) -o $(TMP_DIR)/$@ -c $<

%.o: %.c
	@if [ ! -d $(TMP_DIR)/$(@D) ]; then mkdir -p $(TMP_DIR)/$(@D); fi;
	@printf "  CC      $@\n"
	$(Q)$(CC) $(C_FLAGS) -MMD -MP -o $(TMP_DIR)/$@ -c $<

%.small.o: %.c
	@if [ ! -d $(TMP_DIR)/$(@D) ]; then mkdir -p $(TMP_DIR)/$(@D); fi;
	@printf "  CC      $@\n"
	$(Q)$(CC) $(C_FLAGS) -MMD -MP -o $(TMP_DIR)/$@ -c $<

%.small.d: %.c
	@if [ ! -d $(TMP_DIR)/$(@D) ]; then mkdir -p $(TMP_DIR)/$(@D); fi;
	@printf "  DEP     $@\n"
	$(Q)$(CC) $(C_FLAGS) -MM -MP -MG -o $(TMP_DIR)/$@ $<

clean:
	find . -name "*.a" | xargs rm -f
	find . -name "*.o" | xargs rm -f
	find . -name "*.elf" | xargs rm -f
	find . -name "*.bin" | xargs rm -f

-include $(_O:.o=.d)
