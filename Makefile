TARGET         = boot
V              = 0
FLASH_LOCKED     = 0
DEBUG          = 0
###########################################################
LIB_DIR  = lib
SRC_DIR  = src
TMP_DIR  = tmp
###########################################################
_CRYPTO_     = $(SRC_DIR)/crypto
_LIBOPENCM3_ = $(LIB_DIR)/libopencm3
###########################################################
_O += $(SRC_DIR)/abckey.o
_O += $(SRC_DIR)/boot.o
_O += $(SRC_DIR)/btn.o
_O += $(SRC_DIR)/fla.o
_O += $(SRC_DIR)/led.o
_O += $(SRC_DIR)/mpu.o
_O += $(SRC_DIR)/msg.o
_O += $(SRC_DIR)/rng.o
_O += $(SRC_DIR)/sign.o
_O += $(SRC_DIR)/usb.o
_O += $(SRC_DIR)/util.o
###########################################################
_O += $(_CRYPTO_)/bignum.small.o
_O += $(_CRYPTO_)/ecdsa.small.o
_O += $(_CRYPTO_)/secp256k1.small.o
_O += $(_CRYPTO_)/sha2.small.o
_O += $(_CRYPTO_)/memzero.small.o
###########################################################
C_FLAGS += -DUSE_PRECOMPUTED_IV=0
C_FLAGS += -DUSE_PRECOMPUTED_CP=0
LD_SCRIPT = $(SRC_DIR)/abckey.ld
###########################################################
OPT_FLAGS ?= -Os
###########################################################
include Makefile.mk
