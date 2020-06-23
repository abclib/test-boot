#ifndef __MSG_H__
#define __MSG_H__

#include "stdint.h"
#include "string.h"

#include "abckey.h"
#include "util.h"

#include "crypto/memzero.h"

#define __MSG_L__ 64
#define __MSG_L_HEAD__ 1
#define __MSG_L_FLAG__ 2
#define __MSG_L_TYPE__ 2
#define __MSG_L_SIZE__ 4
#define __MSG_O_TYPE__ 3
#define __MSG_O_SIZE__ 5
#define __MSG_O_DATA__ 9
#define __MSG_S_HEAD__ "$"
#define __MSG_S_FLAG__ "##"
#define __MSG_S_LAST__ "###"
#define __MSG_C_HEAD__ '$'
#define __MSG_C_FLAG__ '#'

typedef enum {
  Type_Success       = 2,
  Type_Failure       = 3,
  Type_Features      = 17,
  Type_ButtonRequest = 26
} MSG_TYPE;

extern char* Message_Null;
extern char* Message_Features;
extern char* Message_Failure_FirmwareError;

uint8_t* enMsg(MSG_TYPE type, char* payload);

#endif
