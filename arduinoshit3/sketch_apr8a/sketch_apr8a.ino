#include <Arduino.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>
#include "key.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"

#define MOUNT_POINT "/sdcard"
#define CONFIG_SOC_SDMMC_USE_GPIO_MATRIX 1
#define CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4 1

#define CONFIG_EXAMPLE_PIN_D0 GPIO_NUM_39
#define CONFIG_EXAMPLE_PIN_CMD GPIO_NUM_41
#define CONFIG_EXAMPLE_PIN_CLK GPIO_NUM_45
#define CONFIG_EXAMPLE_PIN_D1 GPIO_NUM_38
#define CONFIG_EXAMPLE_PIN_D2 GPIO_NUM_40
#define CONFIG_EXAMPLE_PIN_D3 GPIO_NUM_42

const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(4) + 4 * 4;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/18, /* data=*/17);  // ESP32 Thing, HW I2C with pin remapping

void openOTAtosetting();
void reloadConfig();
void wirelessOTAupdate();
void ui_main_prog();

char *menu_main[5] = { "key mode:", "light mode:", "open USB-OTA setting", "reload config file", "wireless OTA upload" };
char *menu_pad_mode[3] = { "mpcpad", "piano", "keyboard" };
char *menu_light_mode[3] = { "nolight", "fluid", "shine" };
void (*menu_func_ptr[3])() = { openOTAtosetting, reloadConfig, wirelessOTAupdate };
void (*current_menu_function)() = &ui_main_prog;

/*----------------device config struct -----------------------*/
// MIDI Channel Define from the struct
struct Percussion {
  char name[16];
  int notes[4][4];
};

typedef enum {
  PIANO,
  MPC_PAD,
  KEYBOARD
} MidiKeypadOutputMode;

// MIDI Keypad Light Modes
typedef enum {
  NO_LIGHT,
  FLUID,
  SHINE
} MidiKeypadLightMode;

typedef struct {
  int output_mode;  // The output mode (piano, MPC pad, or keyboard)
  int light_mode;   // The light mode (no light, fluid, or shine)
} MidiKeypadConfig;

/*----------------end of device config struct -----------------------*/

char current_ui = 0;
Percussion percussion[3];

MidiKeypadConfig device_config = { KEYBOARD, FLUID };

RPKey rpkey;

void setup() {
  // put your setup code here, to run once:
  u8g2.begin();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFontDirection(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  (*current_menu_function)();
}

void ui_main_prog() {
  if (rpkey.key_msg.id == 1 && rpkey.key_msg.pressed && current_ui < 5) {
    current_ui += 1;
  } else if (rpkey.key_msg.id == 1 && rpkey.key_msg.pressed && current_ui == 5) {
    current_ui == 0;
  } else if (rpkey.key_msg.id == 2 && rpkey.key_msg.pressed && current_ui > 0) {
    current_ui -= 1;
  } else if (rpkey.key_msg.id == 2 && rpkey.key_msg.pressed && current_ui == 0) {
    current_ui = 5;
  }


  u8g2.clearBuffer();
  u8g2.setCursor(0, 15);
  if (current_ui == 0 && current_ui == 1) {
    char *show_str = light_runmode_menu_showstiring_output();
    if (show_str != NULL) {
      u8g2.print(show_str);
      free(show_str);
    }
  } else {
    u8g2.print(menu_main[current_ui]);
    if (rpkey.key_msg.id == 3 && rpkey.key_msg.pressed) {
      exec_function(current_ui - 1);
    }
  }
  u8g2.sendBuffer();
}

void exec_modechange_function() {
  switch (device_config.output_mode) {
    case PIANO:
      break;
    case MPC_PAD:
      break;
    case KEYBOARD:
      break;
  }
}

void exec_lightchange_function() {
  switch (device_config.light_mode) {
    case NO_LIGHT:
      break;
    case FLUID:
      break;
    case SHINE:
      break;
  }
}

char *light_runmode_menu_showstiring_output() {
  char *name = menu_main[current_ui];
  char *mode;
  switch (current_ui) {
    case 1:
      mode = menu_pad_mode[device_config.output_mode];
      if (rpkey.key_msg.id == 3 && rpkey.key_msg.pressed == true) {
        if (device_config.output_mode < 3) {
          device_config.output_mode += 1;
        } else {
          device_config.output_mode = 0;
        }
        exec_modechange_function();
      }
      break;
    case 2:
      mode = menu_light_mode[device_config.light_mode];
      if (rpkey.key_msg.id == 3 && rpkey.key_msg.pressed == true) {
        if (device_config.light_mode < 3) {
          device_config.light_mode += 1;
        } else {
          device_config.light_mode = 0;
        }
        exec_lightchange_function();
      }
      break;
  }
  char *result = (char *)malloc(30);
  strcpy(result, mode);
  strcat(result, name);
  return result;
}

void exec_function(int number) {
  switch (number) {
    case 1:
      current_menu_function = menu_func_ptr[1];
      break;
    case 2:
      current_menu_function = menu_func_ptr[2];
      break;
    case 3:
      current_menu_function = menu_func_ptr[3];
      break;
  }
}

void openOTAtosetting() {
  
}

void reloadConfig() {
  StaticJsonDocument<capacity> doc;

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
  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);
  const char *file_hello = MOUNT_POINT "/settings.json";

  Serial.println("Opening file");
  FILE *f = fopen(file_hello, "r");
  if (f == NULL) {
    Serial.println("Failed to open file for writing");
    return;
  }

  char *line = new char[1024]();
  fgets(line, sizeof(char) * 1024, f);
  char *temp = new char[100]();
  while (fgets(temp, 100, f) != NULL) {
    line = strncat(line, temp, 100);
    //printf("%s", RdStrng);
  }
  delete[] temp;

  // Parse the JSON file
  DeserializationError error = deserializeJson(doc, line);
  if (error) {
    Serial.print("Failed to parse config file: ");
    Serial.println(error.c_str());
    return;
  }
  fclose(f);
  delete[] line;

  JsonArray settings = doc["settings"];
  int i = 0;
  for (JsonObject setting : settings) {
    // Extract the name
    const char *name = setting["name"];
    strncpy(percussion[i].name, name, sizeof(percussion[i].name));

    // Extract the notes
    JsonArray notes = setting["notes"];
    int j = 0;
    for (JsonArray row : notes) {
      int k = 0;
      for (int note : row) {
        percussion[i].notes[j][k] = note;
        k++;
      }
      j++;
    }

    i++;
  }

  /*
  // Print the data
  for (i = 0; i < 3; i++) {
    Serial.print("Percussion ");
    Serial.println(percussion[i].name);
    for (int j = 0; j < 4; j++) {
      for (int k = 0; k < 4; k++) {
        Serial.print(percussion[i].notes[j][k]);
        Serial.print(" ");
      }
      Serial.println();
    }
    Serial.println();
  }
  */
}

void wirelessOTAupdate() {
  
}
