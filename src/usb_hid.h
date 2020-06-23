#ifndef __USB_HID_H__
#define __USB_HID_H__

static const struct usb_device_descriptor device_descriptor = {
  .bLength            = USB_DT_DEVICE_SIZE,
  .bDescriptorType    = USB_DT_DEVICE,
  .bcdUSB             = 0x0200,
  .bDeviceClass       = 0,  // Miscellaneous Device
  .bDeviceSubClass    = 0,  // Common Class
  .bDeviceProtocol    = 0,  // Interface Association
  .bMaxPacketSize0    = MAX_PACKET_SIZE,
  .idVendor           = ID_VENDOR,
  .idProduct          = ID_PRODUCT,
  .bcdDevice          = 0x0100,
  .iManufacturer      = 1,
  .iProduct           = 2,
  .iSerialNumber      = 3,
  .bNumConfigurations = 1,
};

/*
 * luozhuopeng 20191114 update content
 * */
static const uint8_t hid_report_descriptor[] = {
  0x06, 0x00, 0xff,  // USAGE_PAGE (Vendor Defined)
  0x09, 0x01,        // USAGE (1)
  0xa1, 0x01,        // COLLECTION (Application)
  0x09, 0x20,        // USAGE (Input Report Data)
  0x15, 0x00,        // LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
  0x75, 0x08,        // REPORT_SIZE (8)
  0x95, 0x40,        // REPORT_COUNT (64)
  0x81, 0x02,        // INPUT (Data,Var,Abs)
  0x09, 0x21,        // USAGE (Output Report Data)
  0x15, 0x00,        // LOGICAL_MINIMUM (0)
  0x26, 0xff, 0x00,  // LOGICAL_MAXIMUM (255)
  0x75, 0x08,        // REPORT_SIZE (8)
  0x95, 0x40,        // REPORT_COUNT (64)
  0x91, 0x02,        // OUTPUT (Data,Var,Abs)
  0xc0               // END_COLLECTION
};

static const struct {
  struct usb_hid_descriptor hid_descriptor;
  struct {
    uint8_t  bReportDescriptorType;
    uint16_t wDescriptorLength;
  } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
  .hid_descriptor = {
    .bLength         = sizeof(hid_function),
    .bDescriptorType = USB_DT_HID,
    .bcdHID          = 0x0100,
    .bCountryCode    = 0,
    .bNumDescriptors = 1,
  },
  .hid_report = {
    .bReportDescriptorType = USB_DT_REPORT,
    .wDescriptorLength     = sizeof(hid_report_descriptor),
  },
};

/**
 * webees 201909121738
 **/
static const struct usb_endpoint_descriptor hid_endpoint_descriptor[] = {
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDPOINT_ADDRESS_OUT,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = MAX_PACKET_SIZE,
    .bInterval        = 1,
  },
  {
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = ENDPOINT_ADDRESS_IN,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = MAX_PACKET_SIZE,
    .bInterval        = 1,
  },
};

/**
 * webees 201909121738
 **/
static const struct usb_interface_descriptor hid_interface_descriptor[] = {{
  .bLength            = USB_DT_INTERFACE_SIZE,
  .bDescriptorType    = USB_DT_INTERFACE,
  .bInterfaceNumber   = INTERFACE_NUMBER,
  .bAlternateSetting  = 0,
  .bNumEndpoints      = 2,
  .bInterfaceClass    = USB_CLASS_HID,
  .bInterfaceSubClass = 0,  // no boot interface
  .bInterfaceProtocol = 0,
  .iInterface         = 0,
  .endpoint           = hid_endpoint_descriptor,
  .extra              = &hid_function,
  .extralen           = sizeof(hid_function),
}};

static const struct usb_interface hid_interface[] = {{
  .num_altsetting = 1,
  .altsetting     = hid_interface_descriptor,
}};

static const struct usb_config_descriptor hid_config_descriptor = {
  .bLength             = USB_DT_CONFIGURATION_SIZE,
  .bDescriptorType     = USB_DT_CONFIGURATION,
  .wTotalLength        = 0,
  .bNumInterfaces      = 1,
  .bConfigurationValue = 1,
  .iConfiguration      = 0,
  .bmAttributes        = 0x80,
  .bMaxPower           = 0x32,
  .interface           = hid_interface,
};

static const char *hid_strings_descriptor[] = {
  __MANUFACTURER__,
  __PRODUCT__,
  "0",
};

#endif
