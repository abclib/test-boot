/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __FLASH_SIGN_H__
#define __FLASH_SIGN_H__

#include <stdint.h>
#include <string.h>

#include "crypto/ecdsa.h"
#include "crypto/memzero.h"
#include "crypto/secp256k1.h"
#include "crypto/sha2.h"

#include "abckey.h"
#include "fla.h"
#include "util.h"

extern const uint32_t FIRMWARE_MAGIC_NEW;  // TRZF

// we don't use the field sig
// and rather introduce fields sig1, sig2, sig3
// immediately following the chunk hashes
typedef struct {
  uint32_t magic;
  uint32_t hdrlen;
  uint32_t expiry;
  uint32_t codelen;
  uint32_t version;
  uint32_t fix_version;
  uint8_t  __reserved1[8];
  uint8_t  hashes[512];
  uint8_t  sig1[64];
  uint8_t  sig2[64];
  uint8_t  sig3[64];
  uint8_t  sigindex1;
  uint8_t  sigindex2;
  uint8_t  sigindex3;
  uint8_t  __reserved2[220];
  uint8_t  __sigmask;
  uint8_t  __sig[64];
} __attribute__((packed)) image_header;

BOOL has_fw(void);
BOOL sign_fw(const image_header *hdr, uint8_t store_fingerprint[32]);
void sha_fw(const image_header *hdr, uint8_t hash[32]);
int  check_fw(const image_header *hdr);

#endif
