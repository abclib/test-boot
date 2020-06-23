#include "sign.h"

#define PUBKEYS 5
#define SIGNATURES 3
#define FLASH_META_START 0x08008000
#define FLASH_META_CODELEN (FLASH_META_START + 0x0004)
#define FLASH_META_SIGINDEX1 (FLASH_META_START + 0x0008)
#define FLASH_META_SIGINDEX2 (FLASH_META_START + 0x0009)
#define FLASH_META_SIGINDEX3 (FLASH_META_START + 0x000A)
#define FLASH_META_SIG1 (FLASH_META_START + 0x0040)
#define FLASH_META_SIG2 (FLASH_META_START + 0x0080)
#define FLASH_META_SIG3 (FLASH_META_START + 0x00C0)

const uint32_t FIRMWARE_MAGIC_NEW = 0x4B434241;  // ABCK

static const uint8_t *const pubkey[PUBKEYS] = {
  (const uint8_t *)"\04\xbe\xc3\xec\xd7\x52\x3b\xc1\xc6\x82\x37\x80\x6e\x0c\x1d\x13\xbf\x94\x02\x39\x26\x34\xc6\xc7\x1a\x80\x91\x4a\x36\xb5\xab\x6d\x22\x08\xf3\xb5\xc2\x7e\x52\x6c\xd7\x16\x22\xb6\x1b\x2b\x8d\x78\x38\xc0\xff\xe5\xa2\x3a\x9d\x91\x60\x61\x4a\x64\x00\xcd\xce\x15\x8f",
  (const uint8_t *)"\04\x9f\x7a\x7c\xef\x7e\x46\x61\x26\xfd\xf4\xe4\xb7\x75\xf5\x97\xd3\xc0\x2a\x83\xa5\x20\x6c\x5f\x8a\x12\xb0\x4f\x75\x21\x56\x5b\x26\xaf\x30\x9b\x9f\x22\x2b\xed\x6a\x94\xa7\xe7\xe8\x85\xf7\xab\x32\xc3\x84\xca\x6a\x22\x5a\x04\x62\xfd\xda\x2a\x85\x40\x5a\x42\x49",
  (const uint8_t *)"\04\xba\x5d\x88\x68\x99\xdf\xb5\xb0\xe3\x04\xe5\x64\xa2\x16\xa6\xd1\x4b\xa9\x65\x18\x3b\xa0\xe2\x23\xd0\x6b\xec\x3a\x71\xfc\x32\x17\xa8\xcf\x35\xf4\x54\x62\xb0\xe8\xbc\xb1\x5e\xe9\x1b\xd6\x8e\xd1\x12\xd3\x2a\x53\x6f\x50\x65\xf6\xef\x21\xea\x4a\xcd\xbc\x20\xe3",
  (const uint8_t *)"\x04\x3c\xef\x1c\x20\xe6\xd3\x93\xa4\x39\x7f\x48\xf9\x30\x0f\x64\xdf\x37\xc4\x8a\xf6\xa9\xa4\x2e\xb2\xad\xc0\x50\x86\xfe\x9f\x38\xe5\x88\x29\x48\x44\x98\xc8\x91\x23\x86\x6a\x3e\x71\xf2\xfb\xc1\xd1\x51\xb6\x8e\x17\xcc\x9a\xc1\x4d\x1b\xb5\x4f\x3e\x28\xd8\x28\xeb",
  (const uint8_t *)"\x04\x09\x41\x34\x4f\x8e\xf5\x0d\x28\xd9\x02\xd8\x7e\x9c\x67\xf3\x10\x04\x94\x4c\xab\x71\x44\x43\x90\x69\xae\x50\x77\xeb\x8b\x52\x68\x3b\xf1\xdc\x0e\xba\xd5\xd1\xc3\x10\x74\xd6\xa4\xe8\x1c\xf6\xe3\xd0\x65\x93\x8e\xc1\xf5\xc9\x80\x05\x72\x60\x07\x20\x03\x59\xee"};

void sha_fw(const image_header *hdr, uint8_t hash[SHA256_DIGEST_LENGTH]) {
  image_header copy = {0};
  memcpy(&copy, hdr, sizeof(image_header));
  memzero(copy.sig1, sizeof(copy.sig1));
  memzero(copy.sig2, sizeof(copy.sig2));
  memzero(copy.sig3, sizeof(copy.sig3));
  copy.sigindex1 = 0;
  copy.sigindex2 = 0;
  copy.sigindex3 = 0;
  sha256_Raw((const uint8_t *)&copy, sizeof(image_header), hash);
}

/**
 * webees 201909051952
 */
BOOL has_fw(void) {
  const image_header *hdr = (const image_header *)FLASH_PTR(FLASH_A_HEAD);
  if (hdr->magic != FIRMWARE_MAGIC_NEW) return FALSE;
  if (hdr->codelen > FLASH_S_CORE) return FALSE;
  if (hdr->codelen < 4096) return FALSE;
  return TRUE;
}

/**
 * webees 201909052124
 * Verify that the firmware is ABCKEY production
 **/
BOOL sign_fw(const image_header *hdr, uint8_t store_fingerprint[SHA256_DIGEST_LENGTH]) {
  uint8_t hash[SHA256_DIGEST_LENGTH];
  sha_fw(hdr, hash);
  if (store_fingerprint) memcpy(store_fingerprint, hash, SHA256_DIGEST_LENGTH);
  if (hdr->sigindex1 < 1 || hdr->sigindex1 > PUBKEYS) return FALSE;                                     // invalid index
  if (hdr->sigindex2 < 1 || hdr->sigindex2 > PUBKEYS) return FALSE;                                     // invalid index
  if (hdr->sigindex3 < 1 || hdr->sigindex3 > PUBKEYS) return FALSE;                                     // invalid index
  if (hdr->sigindex1 == hdr->sigindex2) return FALSE;                                                   // duplicate use
  if (hdr->sigindex1 == hdr->sigindex3) return FALSE;                                                   // duplicate use
  if (hdr->sigindex2 == hdr->sigindex3) return FALSE;                                                   // duplicate use
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey[hdr->sigindex1 - 1], hdr->sig1, hash)) return FALSE;  // failure
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey[hdr->sigindex2 - 1], hdr->sig2, hash)) return FALSE;  // failure
  if (0 != ecdsa_verify_digest(&secp256k1, pubkey[hdr->sigindex3 - 1], hdr->sig3, hash)) return FALSE;  // failure
  return TRUE;
}

/**
 * webees 201909091203
 **/
int check_fw(const image_header *hdr) {
  uint8_t hash[32] = {0};
  // check hash of the first code chunk
  sha256_Raw(FLASH_PTR(FLASH_A_CORE), (64 - 1) * 1024, hash);
  if (0 != memcmp(hash, hdr->hashes, 32)) return FALSE;
  uint32_t total_len   = FLASH_S_HEAD + hdr->codelen;  // check remaining used chunks
  int      used_chunks = total_len / FW_CHUNK_SIZE;
  if (total_len % FW_CHUNK_SIZE > 0) used_chunks++;
  for (int i = 1; i < used_chunks; i++) {
    sha256_Raw(FLASH_PTR(FLASH_A_HEAD + (64 * i) * 1024), 64 * 1024, hash);
    if (0 != memcmp(hdr->hashes + 32 * i, hash, 32)) return FALSE;
  }
  // check unused chunks
  for (int i = used_chunks; i < 16; i++) {
    if (FALSE == ram_is_empty(hdr->hashes + 32 * i, 32)) return FALSE;
  }
  // all OK
  return TRUE;
}
