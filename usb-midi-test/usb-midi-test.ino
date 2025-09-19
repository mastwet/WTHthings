/*
 * SPDX-FileCopyrightText: 2019 Ha Thach (tinyusb.org)
 *
 * SPDX-License-Identifier: MIT
 *
 * SPDX-FileContributor: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-FileContributor: 2023 esp32beans@gmail.com
 */

//重要：一定要记得设置:
//工具->USBMODE:USB_OTG
//工具->USB_CDC_ON_BOOT:DISABLE

// 这个程序基于ESP-IDF的USB MIDI TinyUSB示例代码，经过最小化的修改后
// 可以在Arduino-esp32环境中运行。
#if ARDUINO_USB_MODE
#warning This sketch must be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"

#include "esp32-hal-tinyusb.h"

static const char *TAG = "usbdmidi";

/** TinyUSB 设备描述符 **/

extern "C" uint16_t tusb_midi_load_descriptor(uint8_t *dst, uint8_t *itf) {
  uint8_t str_index = tinyusb_add_string_descriptor("TinyUSB MIDI");
  uint8_t ep_num = tinyusb_get_free_duplex_endpoint();
  TU_VERIFY(ep_num != 0);
  uint8_t descriptor[TUD_MIDI_DESC_LEN] = {
      // 当前MIDI接口的编号, 字符串描述符索引, EP Out & EP In 地址, EP 大小
      TUD_MIDI_DESCRIPTOR(*itf, str_index, ep_num, (uint8_t)(0x80 | ep_num),
                          64)};
  *itf += 1;
  memcpy(dst, descriptor, TUD_MIDI_DESC_LEN);
  return TUD_MIDI_DESC_LEN;
}

//MIDI 数据结构
/* 定义了 USB_MIDI_t 结构体，用于存储MIDI数据包。数据包包含4字节：前两字节用于标识MIDI代码和电缆号，后三字节用于存储MIDI消息。*/
typedef struct __attribute__((__packed__)) {
  uint8_t code_index_number : 4;
  uint8_t cable_number : 4;
  uint8_t MIDI_0;
  uint8_t MIDI_1;
  uint8_t MIDI_2;
} USB_MIDI_t;

static void midi_task_read_example(void *arg) {
/*这是一个循环任务，负责读取MIDI数据。通过 tud_midi_available() 检查是否有可读的MIDI数据包，使用 tud_midi_packet_read() 读取4字节的MIDI数据包，并打印到串口输出。*/

  uint8_t packet[4];
  bool read = false;
  for (;;) {
    delay(1);
    while (tud_midi_available()) {
      read = tud_midi_packet_read(packet);
      if (read) {
        ESP_LOGI(TAG,
                 "Read - Time (ms since boot): %lld, Data: %02hhX %02hhX "
                 "%02hhX %02hhX",
                 esp_timer_get_time(), packet[0], packet[1], packet[2],
                 packet[3]);
        USB_MIDI_t *m = (USB_MIDI_t *)packet;
        Serial.printf(
            "%lld: Cable: %d Code: %01hhX, Data: %02hhX %02hhX %02hhX\n",
            esp_timer_get_time(), m->cable_number, m->code_index_number,
            m->MIDI_0, m->MIDI_1, m->MIDI_2);
      }
    }
  }
}

// 基础midi信息
#define NOTE_OFF 0x80
#define NOTE_ON 0x90

static void periodic_midi_write_example_cb(void *arg) {
  // midi示例程序，示例旋律
  uint8_t const note_sequence[] = {
      74, 78, 81, 86,  90, 93, 98, 102, 57, 61,  66, 69, 73, 78, 81, 85,
      88, 92, 97, 100, 97, 92, 88, 85,  81, 78,  74, 69, 66, 62, 57, 62,
      66, 69, 74, 78,  81, 86, 90, 93,  97, 102, 97, 93, 90, 85, 81, 78,
      73, 68, 64, 61,  56, 61, 64, 68,  74, 78,  81, 86, 90, 93, 98, 102};

  static uint8_t const cable_num = 0; // 与 USB 端点关联的 MIDI 插孔
  static uint8_t const channel = 0;   // 0 for 通道 1
  static uint32_t note_pos = 0;

  // 在音符序列中的前一个位置。
  int previous = note_pos - 1;

  // 如果当前在位置 0，则将前一个位置设置为序列中的最后一个音符。
  if (previous < 0) {
    previous = sizeof(note_sequence) - 1;
  }

  // 在通道 1 上以全速（127）发送当前音符的 Note On。
  ESP_LOGI(TAG, "Writing MIDI data %d", note_sequence[note_pos]);

  if (tud_midi_mounted()) {
    uint8_t note_on[3] = {NOTE_ON | channel, note_sequence[note_pos], 127};
    tud_midi_stream_write(cable_num, note_on, 3);

    // 为前一个音符发送 Note Off
    uint8_t note_off[3] = {NOTE_OFF | channel, note_sequence[previous], 0};
    tud_midi_stream_write(cable_num, note_off, 3);
  }

  // 递增位置
  note_pos++;

  // 如果到达序列末尾，则重新开始。
  if (note_pos >= sizeof(note_sequence)) {
    note_pos = 0;
  }
}

void app_main(void) {
  // 定期发送 MIDI 数据包
  int const tempo = 286;
  const esp_timer_create_args_t periodic_midi_args = {
      .callback = &periodic_midi_write_example_cb,
      /* nname 是可选的，但在调试时可能有助于识别定时器 */
      .name = "periodic_midi"};

  ESP_LOGI(TAG, "MIDI write task init");
  esp_timer_handle_t periodic_midi_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_midi_args, &periodic_midi_timer));
  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_midi_timer, tempo * 1000));

  // 读取接收到的 MIDI 数据包
  ESP_LOGI(TAG, "MIDI read task init");
  xTaskCreate(midi_task_read_example, "midi_task_read_example", 2 * 1024, NULL,
              5, NULL);
}

static void usbEventCallback(void *arg, esp_event_base_t event_base,
                             int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_EVENTS) {
    arduino_usb_event_data_t *data = (arduino_usb_event_data_t *)event_data;
    switch (event_id) {
    case ARDUINO_USB_STARTED_EVENT:
      Serial.println("USB PLUGGED");
      break;
    case ARDUINO_USB_STOPPED_EVENT:
      Serial.println("USB UNPLUGGED");
      break;
    case ARDUINO_USB_SUSPEND_EVENT:
      Serial.printf("USB SUSPENDED: remote_wakeup_en: %u\n",
                    data->suspend.remote_wakeup_en);
      break;
    case ARDUINO_USB_RESUME_EVENT:
      Serial.println("USB RESUMED");
      break;

    default:
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);

  USB.onEvent(usbEventCallback);
  tinyusb_enable_interface(USB_INTERFACE_MIDI, TUD_MIDI_DESC_LEN,
                           tusb_midi_load_descriptor);
  USB.begin();
  while (!Serial && millis() < 5000)
    delay(10);
  app_main();
}

void loop() {}
#endif /* ARDUINO_USB_MODE */