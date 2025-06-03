#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#include <ArduinoJson.h>

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"
#define CONFIG_SOC_SDMMC_USE_GPIO_MATRIX 1
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 1

#define CONFIG_EXAMPLE_PIN_D0 GPIO_NUM_39
#define CONFIG_EXAMPLE_PIN_CMD GPIO_NUM_41
#define CONFIG_EXAMPLE_PIN_CLK GPIO_NUM_45
#define CONFIG_EXAMPLE_PIN_D1 GPIO_NUM_38
#define CONFIG_EXAMPLE_PIN_D2 GPIO_NUM_40
#define CONFIG_EXAMPLE_PIN_D3 GPIO_NUM_42

struct wifi_config {
  String wifi_ssid;
  String wifi_passwd;
};
wifi_config g_wifi_conf;


void setup() {
  Serial.begin(115200);
  esp_err_t ret;
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
    .format_if_mount_failed = true,
#else
    .format_if_mount_failed = false,
#endif
    .max_files = 5,
    .allocation_unit_size = 16 * 1024
  };
  sdmmc_card_t *card;
  const char mount_point[] = MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");
  ESP_LOGI(TAG, "Using SDMMC peripheral");
  sdmmc_host_t host = SDMMC_HOST_DEFAULT();
  sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

  slot_config.width = 4;
  slot_config.width = 1;
  slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
  slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
  slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
  slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
  slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
  slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;

  slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
  Serial.println("Mounting filesystem");
  ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config, &mount_config, &card);
  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      Serial.println("Failed to mount filesystem.If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      Serial.println("Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.");
    }
    return;
  }
  Serial.println("Filesystem mounted");

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);
  const char *file_hello = MOUNT_POINT "/wifi.json";

  Serial.println("Opening file");
  FILE *f = fopen(file_hello, "r");
  if (f == NULL) {
    Serial.println("Failed to open file for writing");
    return;
  }

  //char line[1024];
  char *line = new char[1024]();
  fgets(line, sizeof(char) * 1024, f);
  char *temp = new char[100]();
  while (fgets(temp, 100, f) != NULL) {
    line = strncat(line, temp, 100);
    //printf("%s", RdStrng);
  }
  //Serial.println(line);
  delete[] temp;
  StaticJsonDocument<1024> doc;
  deserializeJson(doc, line);
  String _wifi_ssid = doc["ssid"];
  String _wifi_passwd = doc["passwd"];
  if (_wifi_ssid != "null"){
    g_wifi_conf.wifi_ssid = _wifi_ssid;
    Serial.println(g_wifi_conf.wifi_ssid);
  }
  if (_wifi_passwd != "null"){
    g_wifi_conf.wifi_passwd = _wifi_passwd;
    Serial.println(g_wifi_conf.wifi_passwd);
  }
  fclose(f);
  delete[] line;

/*
  if (doc["wifi_ssid"] != 0) {
    g_wifi_conf.wifi_ssid = doc["wifi_ssid"];
  }
  if (doc["wifi_passwd"] != 0) {
    g_wifi_conf.wifi_passwd = doc["wifi_passwd"];
  }
  */

  String output;
  serializeJson(doc, output);
  Serial.println(output);
}

void loop() {
  // put your main code here, to run repeatedly:
}
