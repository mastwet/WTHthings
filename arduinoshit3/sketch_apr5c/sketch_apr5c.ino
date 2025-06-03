#include <Arduino.h>
#include <U8g2lib.h>
#include <ArduinoJson.h>

const size_t capacity = JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(4) + 4 * 4;


U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/18, /* data=*/17);  // ESP32 Thing, HW I2C with pin remapping

void openOTAtosetting();
void reloadConfig();
void wirelessOTAupdate();
void ui_main_prog();

char *menu_main[5] = { "key mode:", "light mode:", "open USB-OTA setting", "reload config file", "wireless OTA upload" };
char *menu_pad_mode[3] = { "mpcpad", "piano", "keyboard" };
char *menu_light_mode[3] = { "nolight", "fluid", "shine" };
void (*menu_func_ptr[3])(void *) = { openOTAtosetting, reloadConfig, wirelessOTAupdate };
void (*current_menu_function)() = &ui_main_prog;

// Define the struct
struct Percussion {
  char name[16];
  int notes[4][4];
};

// Create an array of structs
Percussion percussion[3];



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

// MIDI Keypad Configuration
typedef struct {
  MidiKeypadOutputMode output_mode;  // The output mode (piano, MPC pad, or keyboard)
  MidiKeypadLightMode light_mode;    // The light mode (no light, fluid, or shine)
} MidiKeypadConfig;

MidiKeypadConfig runmode_config = { KEYBOARD, FLUID };

char current_ui = 0;
int key_scan() {
}


// change the current menu function pointer, and will execute the function in the main loop
void exec_function(int number) {
  switch (number) {
    case 1:
      current_menu_function = &menu_func_ptr[1];
      break;
    case 2:
      current_menu_function = &menu_func_ptr[2];
      break;
    case 3;
      current_menu_function = &menu_func_ptr[3];
      break;
  }
}

char* light_runmode_menu_showstiring_output(int runmode){
    char* name = menu_main[current_ui];
    char* mode;
    if(rpkey.key_msg.id == 3){
    switch(current_ui){
    case 1:
      if (runmode_config.output_mode < 3) {
        runmode_config.output_mode += 1;
      } else {
        runmode_config.output_mode = 0;
      }
      mode = menu_output_mode[runmode_config.output_mode];
      exec_modechange_function();
      break;
    case 2:
      if (runmode_config.output_mode < 3) {
        runmode_config.output_mode += 1;
      } else {
        runmode_config.output_mode = 0;
      }
      mode = menu_light_mode[runmode_config.light_mode];
      exec_lightchange_function();
      break;
  }
  return;
}
}
char *mode = menu_pad_mode[runmode_config.output_mode];
  char *result = malloc(strlen(mode) + strlen(name) + 1);
  if (!result) {
    return NULL;
  }
  strcpy(result, mode);
  strcat(result, name);
return result;
}

void ui_main_prog() {
  if (key_scan() == 1 && current_ui < 5) {
    current_ui += 1;
  } else if (key_scan() == 1 && current_ui == 5) {
    current_ui == 0;
  } else if (key_scan() == 2 && current_ui > 0) {
    current_ui -= 1;
  } else if (key_scan() == 2 && current_ui == 0) {
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
    if (key_scan() == 3) {
      exec_function(current_ui - 1);
    }
  }
  u8g2.sendBuffer();
}

show_ui();
}

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_unifont_t_chinese2);  // use chinese2 for all the glyphs of "你好世界"
  u8g2.setFontDirection(0);
}

void loop() {
  (*current_menu_function)();
}


void openOTAtosetting() {

}

void reloadConfig() {
  StaticJsonDocument<capacity> doc;
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file");
    return;
  }

  // Parse the JSON file
  DeserializationError error = deserializeJson(doc, configFile);
  if (error) {
    Serial.print("Failed to parse config file: ");
    Serial.println(error.c_str());
    return;
  }

  // Close the JSON file
  configFile.close();

  // Extract the data from the JSON document
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
