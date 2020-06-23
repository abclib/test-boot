#include "msg.h"

static uint8_t MSG[__MSG_L__];

char* Message_Null = __MSG_S_LAST__;

char* Message_Features =
  "\x0a\x0a"__MANUFACTURER__     // vendor
  "\xaa\x01\x01"__MODEL__        // model: "1"
  "\x10" VERSION_MAJOR_CHAR      // majorVersion
  "\x18" VERSION_MINOR_CHAR      // minorVersion
  "\x20" VERSION_PATCH_CHAR      // patchVersion
  "\x28\x01"                     // bootloaderMode: true
  "\x90\x01\x00"__MSG_S_LAST__;  // firmwarePresent: false

char* Message_Failure_FirmwareError =
  "\x08"
  "\x63" __MSG_S_LAST__;

static size_t getByte(char* str) {
  if (str[0] == __MSG_C_FLAG__ &&
      str[1] == __MSG_C_FLAG__ &&
      str[2] == __MSG_C_FLAG__) return 0;
  size_t str_byte = 0;
  for (size_t i = 0;; i++) {
    str_byte++;
    if (str[i + 1] == __MSG_C_FLAG__ &&
        str[i + 2] == __MSG_C_FLAG__ &&
        str[i + 3] == __MSG_C_FLAG__) break;
  }
  return str_byte;
};

uint8_t* enMsg(MSG_TYPE type, char* payload) {
  char   str_type[__MSG_L_TYPE__];
  char   str_size[__MSG_L_SIZE__];
  size_t payload_byte = getByte(payload);
  int2hex(type, str_type, __MSG_L_TYPE__);
  int2hex(payload_byte, str_size, __MSG_L_SIZE__);
  memzero(MSG, __MSG_L__);
  memcpy(MSG, __MSG_S_HEAD__, __MSG_L_HEAD__);
  memcpy(MSG + __MSG_L_HEAD__, __MSG_S_FLAG__, __MSG_L_FLAG__);
  memcpy(MSG + __MSG_O_TYPE__, str_type, __MSG_L_TYPE__);
  memcpy(MSG + __MSG_O_SIZE__, str_size, __MSG_L_SIZE__);
  memcpy(MSG + __MSG_O_DATA__, payload, payload_byte);
  return MSG;
}
