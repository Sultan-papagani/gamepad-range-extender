
#include "Adafruit_TinyUSB.h"
#include "xinput_host.h"

extern "C" {
  const usbh_class_driver_t* usbh_app_driver_get_cb(uint8_t* driver_count) {
    *driver_count = 1; 
    return &usbh_xinput_driver;
  }
}

Adafruit_USBH_Host USBHost;

#define UART_BAUDRATE 500000
#define MAX_REPORT_SIZE 64

bool deviceConnected = false; 

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial1.begin(UART_BAUDRATE);
  USBHost.begin(0);
}

void loop() {
  USBHost.task();
}

void tuh_xinput_report_received_cb(uint8_t dev_addr, uint8_t instance, xinputh_interface_t const* xid_itf, uint16_t len) {
  const xinput_gamepad_t *pad = &xid_itf->pad;

  if (!xid_itf->connected) {
    tuh_xinput_receive_report(dev_addr, instance);
    return;
  }

  deviceConnected = true;
  digitalWrite(LED_BUILTIN, HIGH);

  if (xid_itf->new_pad_data) {
    uint8_t report[12];
    report[0]  = pad->wButtons & 0xFF;
    report[1]  = (pad->wButtons >> 8) & 0xFF;
    report[2]  = pad->bLeftTrigger;
    report[3]  = pad->bRightTrigger;
    report[4]  = pad->sThumbLX & 0xFF;
    report[5]  = (pad->sThumbLX >> 8) & 0xFF;
    report[6]  = pad->sThumbLY & 0xFF;
    report[7]  = (pad->sThumbLY >> 8) & 0xFF;
    report[8]  = pad->sThumbRX & 0xFF;
    report[9]  = (pad->sThumbRX >> 8) & 0xFF;
    report[10] = pad->sThumbRY & 0xFF;
    report[11] = (pad->sThumbRY >> 8) & 0xFF;

    Serial1.write(0xAA);
    Serial1.write(0xBB);
    Serial1.write((uint8_t)12);
    Serial1.write(report, 12);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }

  tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_mount_cb(uint8_t dev_addr, uint8_t instance, const xinputh_interface_t *xid_itf) {
  Serial1.print("\n[PICO] XInput Mount type=");
  Serial1.println(xid_itf->type);
  tuh_xinput_receive_report(dev_addr, instance);
}

void tuh_xinput_umount_cb(uint8_t dev_addr, uint8_t instance) {
  deviceConnected = false;
  digitalWrite(LED_BUILTIN, LOW);
  Serial1.println("\n[PICO] XInput Unmounted");
}
