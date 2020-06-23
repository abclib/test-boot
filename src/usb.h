#ifndef __USB_H__
#define __USB_H__

#include <stdlib.h>

#include <libopencm3/stm32/desig.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/usb/dwc/otg_common.h>
#include <libopencm3/usb/dwc/otg_fs.h>
#include <libopencm3/usb/hid.h>
#include <libopencm3/usb/usbd.h>

#include "crypto/memzero.h"
#include "crypto/sha2.h"

#include "abckey.h"
#include "btn.h"
#include "msg.h"
#include "rng.h"
#include "sign.h"
#include "util.h"

#define MAX_PACKET_SIZE 64
#define INTERFACE_NUMBER 0
#define ENDPOINT_ADDRESS_IN 0x81
#define ENDPOINT_ADDRESS_OUT 0x01
#define ID_VENDOR 0x1209
#define ID_PRODUCT 0xABC1

void usb_loop(void);
void usb_send(MSG_TYPE type, char *payload);

#endif
