
enum {
  __NULL__,
  __READY__,
  __ERASED__,
  __FLASHING__,
  __CHECK__,
};

static BOOL     old_fw_signed = FALSE;
static char     FLASH_STATE   = __NULL__;
static uint32_t flash_pos     = 0;
uint32_t        flash_len     = 0;
static uint32_t chunk_idx     = 0;

static uint32_t FW_HEADER[FLASH_S_HEAD / sizeof(uint32_t)];
static uint32_t FW_CHUNK[FW_CHUNK_SIZE / sizeof(uint32_t)];

static void check_and_write_chunk(void) {
  uint32_t offset    = (chunk_idx == 0) ? FLASH_S_HEAD : 0;
  uint32_t chunk_pos = flash_pos % FW_CHUNK_SIZE;
  if (chunk_pos == 0) chunk_pos = FW_CHUNK_SIZE;
  uint8_t    hash[32] = {0};
  SHA256_CTX ctx      = {0};
  sha256_Init(&ctx);
  sha256_Update(&ctx, (const uint8_t *)FW_CHUNK + offset, chunk_pos - offset);
  if (chunk_pos < __MSG_L__ * 1024) {
    for (uint32_t i = chunk_pos; i < __MSG_L__ * 1024; i += 4) {  // pad with FF
      sha256_Update(&ctx, (const uint8_t *)"\xFF\xFF\xFF\xFF", 4);
    }
  }
  sha256_Final(&ctx, hash);
  const image_header *hdr = (const image_header *)FW_HEADER;
  if (0 != memcmp(hash, hdr->hashes + chunk_idx * 32, 32)) {  // invalid chunk sent
    fla_erase_data();                                         // erase storage
    return usb_send(Type_Failure, Message_Failure_FirmwareError);
  }
  fla_on();
  for (uint32_t i = offset / sizeof(uint32_t); i < chunk_pos / sizeof(uint32_t); i++) {
    flash_program_word(FLASH_A_HEAD + chunk_idx * FW_CHUNK_SIZE + i * sizeof(uint32_t), FW_CHUNK[i]);
  }
  fla_off();
  if (flash_len == flash_pos) {                               // all done
    for (uint32_t i = chunk_idx + 1; i < 16; i++) {           // check remaining chunks if any
      if (FALSE == ram_is_empty(hdr->hashes + 32 * i, 32)) {  // hash should be empty if the chunk is unused
        return usb_send(Type_Failure, Message_Failure_FirmwareError);
      }
    }
  }
  memzero(FW_CHUNK, sizeof(FW_CHUNK));
  chunk_idx++;
}

// read protobuf integer and advance pointer
static BOOL read_pb_int(const uint8_t **ptr, uint32_t *result) {
  *result = 0;
  for (int i = 0; i <= 3; ++i) {
    *result += (**ptr & 0x7F) << (7 * i);
    if ((**ptr & 0x80) == 0) {
      (*ptr)++;
      return TRUE;
    }
    (*ptr)++;
  }
  if (**ptr & 0xF0) {
    *result = 0;                    // result does not fit into uint32_t
    while (**ptr & 0x80) (*ptr)++;  // skip over the rest of the integer
    (*ptr)++;
    return FALSE;
  }
  *result += (uint32_t)(**ptr) << 28;
  (*ptr)++;
  return TRUE;
}

static void hid_endpoint_callback(usbd_device *dev, uint8_t ep) {
  (void)ep;
  static uint16_t msg_id = 0xFFFF;
  static uint8_t  buf[__MSG_L__] __attribute__((aligned(4)));
  static uint32_t w;
  static int      wi;
  if (usbd_ep_read_packet(dev, ENDPOINT_ADDRESS_OUT, buf, __MSG_L__) != __MSG_L__) return;
  if (FLASH_STATE != __FLASHING__) {
    if (buf[0] != __MSG_C_HEAD__ || buf[1] != __MSG_C_FLAG__ || buf[2] != __MSG_C_FLAG__) return;  // invalid start - discard
    msg_id = (buf[3] << 8) + buf[4];                                                               // struct.unpack(">HL") => msg, size
  } else {
    if (buf[0] != __MSG_C_HEAD__) return usb_send(Type_Failure, Message_Failure_FirmwareError);  // invalid contents
  }
  //*****************************************************************************************************
  // Initialize (id 0) || GetFeatures  (id 55)
  if (msg_id == 0x0000 || msg_id == 0x0037) {
    FLASH_STATE = __READY__;
    return usb_send(Type_Features, Message_Features);
  }
  //*****************************************************************************************************
  // Ping message (id 1)
  if (msg_id == 0x0001) return usb_send(Type_Success, Message_Null);
  //*****************************************************************************************************
  // WipeDevice message (id 5)
  if (msg_id == 0x0005) {
    BOOL btn_ok = wait_btn(BTN_OK_3);  // Do you really want to wipe the device? All data will be lost.
    if (TRUE == btn_ok) {
      fla_erase_data();
      fla_erase_core();
      return usb_send(Type_Success, Message_Null);  // Device successfully wiped.
    } else {
      return usb_send(Type_Failure, Message_Null);  // Device wipe aborted.
    }
  }
  //*****************************************************************************************************
  // FirmwareErase message (id 6)
  if (FLASH_STATE == __READY__) {
    if (msg_id == 0x0006) {
      BOOL ok_btn = TRUE;
      BOOL ok_FW  = has_fw();
      if (TRUE == ok_FW) {
        ok_btn = wait_btn(BTN_OK_3);  // Continue Install new firmware?Never do this without your recovery card!
      } else {
        old_fw_signed = FALSE;
      }
      if (FALSE == ok_btn) return usb_send(Type_Failure, Message_Null);  // Firmware installation aborted.
      if (TRUE == ok_FW) {
        const image_header *hdr = (const image_header *)FLASH_PTR(FLASH_A_HEAD);
        old_fw_signed           = sign_fw(hdr, NULL) & check_fw(hdr);  // check whether the current firmware is signed (old or new method)
      }
      fla_erase_core();
      FLASH_STATE = __ERASED__;
      return usb_send(Type_Success, Message_Null);
    }
  }
  //*****************************************************************************************************
  // FirmwareUpload message (id 7)
  if (FLASH_STATE == __ERASED__) {
    if (msg_id == 0x0007) {
      BOOL result = TRUE;
      if (buf[9] != 0x0a) result = FALSE;                           // invalid contents
      const uint8_t *p = buf + 10;                                  // read payload length
      if (TRUE != read_pb_int(&p, &flash_len)) result = FALSE;      // integer too large
      if (flash_len <= FLASH_S_HEAD) result = FALSE;                // firmware is too small
      if (flash_len > FLASH_S_HEAD + FLASH_S_CORE) result = FALSE;  // firmware is too big
      if (memcmp(p, &FIRMWARE_MAGIC_NEW, 4) != 0) result = FALSE;   // check firmware magic
      if (FALSE == result) return usb_send(Type_Failure, Message_Failure_FirmwareError);
      memzero(FW_HEADER, sizeof(FW_HEADER));
      memzero(FW_CHUNK, sizeof(FW_CHUNK));
      FLASH_STATE = __FLASHING__;
      flash_pos   = 0;
      chunk_idx   = 0;
      w           = 0;
      while (p < buf + __MSG_L__) {
        w = (w >> 8) | (((uint32_t)*p) << 24);  // assign byte to first byte of uint32_t w
        wi++;
        if (wi == 4) {
          FW_HEADER[flash_pos / 4] = w;
          flash_pos += 4;
          wi = 0;
        }
        p++;
      }
      return;
    }
  }
  //*****************************************************************************************************
  // Multi-page data
  if (FLASH_STATE == __FLASHING__) {
    const uint8_t *p = buf + 1;
    while (p < buf + __MSG_L__ && flash_pos < flash_len) {
      w = (w >> 8) | (((uint32_t)*p) << 24);  // assign byte to first byte of uint32_t w
      wi++;
      if (wi == 4) {
        if (flash_pos < FLASH_S_HEAD) {
          FW_HEADER[flash_pos / 4] = w;
        } else {
          FW_CHUNK[(flash_pos % FW_CHUNK_SIZE) / 4] = w;
        }
        flash_pos += 4;
        wi = 0;
        if (flash_pos % FW_CHUNK_SIZE == 0) check_and_write_chunk();  // finished the whole chunk
      }
      p++;
    }
    if (flash_pos == flash_len) {                                  // flashing done
      if (flash_pos % FW_CHUNK_SIZE > 0) check_and_write_chunk();  // flush remaining data in the last chunk
      const image_header *hdr = (const image_header *)FW_HEADER;
      FLASH_STATE             = __CHECK__;
      if (TRUE != sign_fw(hdr, NULL)) return usb_send(Type_Failure, Message_Failure_FirmwareError);
    } else {
      return;
    }
  }
  if (FLASH_STATE == __CHECK__) {
    const image_header *hdr           = (const image_header *)FW_HEADER;  // use the firmware header from RAM
    BOOL                new_fw_signed = FALSE;
    if (TRUE == sign_fw(hdr, NULL)) new_fw_signed = true;
    // wipe storage if:
    // 1) old firmware was unsigned or not present
    // 2) signatures are not OK
    // 3) hashes are not OK
    if (TRUE != old_fw_signed || TRUE != sign_fw(hdr, NULL) || TRUE != check_fw(hdr)) {
      fla_erase_data();
      uint8_t hash[32] = {0};
      sha256_Raw(FLASH_PTR(FLASH_A_DATA), FLASH_S_DATA, hash);
      if (memcmp(hash, "\x2d\x86\x4c\x0b\x78\x9a\x43\x21\x4e\xee\x85\x24\xd3\x18\x20\x75\x12\x5e\x5c\xa2\xcd\x52\x7f\x35\x82\xec\x87\xff\xd9\x40\x76\xbc", 32) != 0) {
        return usb_send(Type_Failure, Message_Failure_FirmwareError);  // Check for complete erasure
      }
    }

    fla_on();
    if (new_fw_signed) {  // write firmware header only when hash was confirmed
      for (size_t i = 0; i < FLASH_S_HEAD / sizeof(uint32_t); i++) {
        flash_program_word(FLASH_A_HEAD + i * sizeof(uint32_t), FW_HEADER[i]);
      }
    } else {
      for (size_t i = 0; i < FLASH_S_HEAD / sizeof(uint32_t); i++) {
        flash_program_word(FLASH_A_HEAD + i * sizeof(uint32_t), 0);
      }
    }
    fla_off();

    if (new_fw_signed) {
      return usb_send(Type_Success, Message_Null);
    } else {
      return usb_send(Type_Failure, Message_Null);
    }
  }
}
