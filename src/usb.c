#include "usb.h"

#include "usb_hid.h"

#include "usb_cb.h"

static usbd_device *__USBD_DEVICE__;
static uint8_t      hid_control_buffer[256] __attribute__((aligned(2)));

/**
 * webees 201909121724
 **/
static enum usbd_request_return_codes hid_control_request(usbd_device *                   usbd_dev,
                                                          struct usb_setup_data *         req,
                                                          uint8_t **                      buf,
                                                          uint16_t *                      len,
                                                          usbd_control_complete_callback *complete) {
  (void)complete;
  (void)usbd_dev;
  delay_random();  // Use this to protect sensitive code against fault injection.
  if ((req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
      (req->bmRequestType != (USB_REQ_TYPE_IN | USB_REQ_TYPE_INTERFACE)) ||
      (req->wValue != USB_DT_REPORT << 8)) return USBD_REQ_NOTSUPP;
  /* Handle the HID report descriptor. */
  *buf = (uint8_t *)hid_report_descriptor;
  *len = sizeof(hid_report_descriptor);
  return USBD_REQ_HANDLED;
}

/**
 * webees 201909121733
 **/
static void hid_set_config_callback(usbd_device *hid_device, uint16_t wValue) {
  (void)wValue;
  usbd_ep_setup(hid_device, ENDPOINT_ADDRESS_IN, USB_ENDPOINT_ATTR_INTERRUPT, MAX_PACKET_SIZE, 0);
  usbd_ep_setup(hid_device, ENDPOINT_ADDRESS_OUT, USB_ENDPOINT_ATTR_INTERRUPT, MAX_PACKET_SIZE, hid_endpoint_callback);
  usbd_register_control_callback(hid_device,
                                 USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                                 USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
                                 hid_control_request);
}

/**
 * webees 20190917
 **/
static void unique_id(void) {
  char     id_str[3][9]   = {0x00};
  char     id_str_arr[24] = {0x00};
  uint32_t u32id[3];
  desig_get_unique_id(u32id);
  for (size_t i = 0; i < 3; i++) {
    uint32hex(u32id[i], (char *)&id_str[i]);
    strcat(id_str_arr, (char *)&id_str[i]);
  }
  if (id_str_arr != NULL) hid_strings_descriptor[2] = id_str_arr;
}

/**
 * webees 201911180112
 **/
static inline void usbInit(void) {
  unique_id();
  __USBD_DEVICE__ = usbd_init(&otgfs_usb_driver,
                              &device_descriptor,
                              &hid_config_descriptor,
                              hid_strings_descriptor,
                              sizeof(hid_strings_descriptor) / sizeof(const char *),
                              hid_control_buffer,
                              sizeof(hid_control_buffer));

  // OTG_FS_GCCFG |= OTG_GCCFG_NOVBUSSENS | OTG_GCCFG_PWRDWN;  // STM32F407 luozhuopeng 201909061149
  usbd_register_set_config_callback(__USBD_DEVICE__, hid_set_config_callback);
}

/**
 * webees 201909062017
 **/
void usb_send(MSG_TYPE type, char *payload) {
  uint8_t *en_msg = enMsg(type, payload);
  while (usbd_ep_write_packet(__USBD_DEVICE__, ENDPOINT_ADDRESS_IN, en_msg, MAX_PACKET_SIZE) != __MSG_L__) {
  }
}

void usb_loop(void) {
  usbInit();
  for (;;) {
    usbd_poll(__USBD_DEVICE__);
  }
}
