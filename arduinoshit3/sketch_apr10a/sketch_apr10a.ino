// TinyUSB lib
#include "Adafruit_TinyUSB.h"

// Pin D+ for host, D- = D+ + 1
#define HOST_PIN_DP       20

// Pin for enabling Host VBUS. comment out if not used
#define HOST_PIN_VBUS_EN        22
#define HOST_PIN_VBUS_EN_STATE  1

Adafruit_USBH_Host USBHost;
Adafruit_USBH_CDC  SerialHost;

void setup() {
  Serial1.begin(115200);

  Serial.begin(115200);
  while ( !Serial ) delay(10);

  Serial.println("TinyUSB Host Serial Echo Example");
}

void loop()
{
  uint8_t buf[64];

  // Serial -> SerialHost
  if (Serial.available()) {
    size_t count = Serial.read(buf, sizeof(buf));
    if ( SerialHost && SerialHost.connected() ) {
      SerialHost.write(buf, count);
      SerialHost.flush();
    }
  }

  if ( SerialHost.connected() && SerialHost.available() ) {
    size_t count = SerialHost.read(buf, sizeof(buf));
    Serial.write(buf, count);
  }
}


void setup1() {
  while ( !Serial ) delay(10);   // wait for native usb
  Serial.println("Core1 setup to run TinyUSB host with pio-usb");

  // Check for CPU frequency, must be multiple of 120Mhz for bit-banging USB
  uint32_t cpu_hz = clock_get_hz(clk_sys);
  if ( cpu_hz != 120000000UL && cpu_hz != 240000000UL ) {
    while ( !Serial ) {
      delay(10);   // wait for native usb
    }
    Serial.printf("Error: CPU Clock = %u, PIO USB require CPU clock must be multiple of 120 Mhz\r\n", cpu_hz);
    Serial.printf("Change your CPU Clock to either 120 or 240 Mhz in Menu->CPU Speed \r\n", cpu_hz);
    while(1) {
      delay(1);
    }
  }

#ifdef HOST_PIN_VBUS_EN
  pinMode(HOST_PIN_VBUS_EN, OUTPUT);

  // power off first
  digitalWrite(HOST_PIN_VBUS_EN, 1-HOST_PIN_VBUS_EN_STATE);
  delay(1);

  // power on
  digitalWrite(HOST_PIN_VBUS_EN, HOST_PIN_VBUS_EN_STATE);
  delay(10);
#endif

  pio_usb_configuration_t pio_cfg = PIO_USB_DEFAULT_CONFIG;
  pio_cfg.pin_dp = HOST_PIN_DP;
  USBHost.configure_pio_usb(1, &pio_cfg);

  // run host stack on controller (rhport) 1
  // Note: For rp2040 pico-pio-usb, calling USBHost.begin() on core1 will have most of the
  // host bit-banging processing works done in core1 to free up core0 for other works
  USBHost.begin(1);
}

void loop1()
{
  USBHost.task();

  // periodically flush SerialHost if connected
  if ( SerialHost && SerialHost.connected() ) {
    SerialHost.flush();
  }
}

//--------------------------------------------------------------------+
// TinyUSB Host callbacks
//--------------------------------------------------------------------+

// Invoked when a device with CDC interface is mounted
// idx is index of cdc interface in the internal pool.
void tuh_cdc_mount_cb(uint8_t idx) {
  // bind SerialHost object to this interface index
  SerialHost.setInterfaceIndex(idx);
  SerialHost.begin(115200);

  Serial.println("SerialHost is connected to a new CDC device");
}

// Invoked when a device with CDC interface is unmounted
void tuh_cdc_umount_cb(uint8_t idx) {
  if (idx == SerialHost.getInterfaceIndex()) {
    // unbind SerialHost if this interface is unmounted
    SerialHost.end();

    Serial.println("SerialHost is disconnected");
  }
}
