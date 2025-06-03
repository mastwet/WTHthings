#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#include <ArduinoJson.h>

// This sketch provide the functionality of OTA Firmware Upgrade
#include "WiFi.h"
#include <Update.h>

WiFiClient client;

// Variables to validate
// response from S3
long contentLength = 0;
bool isValidContentType = false;

// Your SSID and PSWD that the chip needs
// to connect to
const char* SSID = "YOUR-SSID";
const char* PSWD = "YOUR-SSID-PSWD";

// S3 Bucket Config
String host = "bucket-name.s3.ap-south-1.amazonaws.com"; // Host => bucket-name.s3.region.amazonaws.com
int port = 80; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
String bin = "/sketch-name.ino.bin"; // bin file name with a slash in front.

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

static const char *TAG = "example";

static const char *url = "https://example.com/firmware.bin";  //state url of your firmware image

#define MOUNT_POINT "/sdcard"
#define CONFIG_SOC_SDMMC_USE_GPIO_MATRIX 1
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 1

#define CONFIG_EXAMPLE_PIN_D0 GPIO_NUM_5
#define CONFIG_EXAMPLE_PIN_CMD GPIO_NUM_7
#define CONFIG_EXAMPLE_PIN_CLK GPIO_NUM_6
#define CONFIG_EXAMPLE_PIN_D1 GPIO_NUM_4
#define CONFIG_EXAMPLE_PIN_D2 GPIO_NUM_16
#define CONFIG_EXAMPLE_PIN_D3 GPIO_NUM_15

struct wifi_config {
  String wifi_ssid;
  String wifi_passwd;
};
wifi_config g_wifi_conf;

void setup() {

  // put your setup code here, to run once:
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
  if (_wifi_ssid != "null") {
    g_wifi_conf.wifi_ssid = _wifi_ssid;
    Serial.println(g_wifi_conf.wifi_ssid);
  }
  if (_wifi_passwd != "null") {
    g_wifi_conf.wifi_passwd = _wifi_passwd;
    Serial.println(g_wifi_conf.wifi_passwd);
  }
  fclose(f);
  delete[] line;

  /*
  //char line[1024];
  char *line = new char[2048]();
  fgets(line, sizeof(char) * 2048, f);
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
  if (_wifi_ssid != "null") {
    g_wifi_conf.wifi_ssid = _wifi_ssid;
    Serial.println(g_wifi_conf.wifi_ssid);
  }
  if (_wifi_passwd != "null") {
    g_wifi_conf.wifi_passwd = _wifi_passwd;
    Serial.println(g_wifi_conf.wifi_passwd);
  }
  fclose(f);
  delete[] line;
*/



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


void execOTA() {
  Serial.println("Connecting to: " + String(host));
  // Connect to S3
  if (client.connect(host.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + host + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    Serial.println("Connection to " + String(host) + " failed. Please check your setup");
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          ESP.restart();
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      Serial.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }
}

void vTaskDeletAll() {
}

void device_update() {
  vTaskDeletAll();
  //oled begin

  Serial.println("Confirn Update Firmware?");
  //wifi begin
  WiFi.begin(g_wifi_conf.wifi_ssid.c_str(), g_wifi_conf.wifi_passwd.c_str());

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  execOTA();

}

void loop() {
  // put your main code here, to run repeatedly:
}
