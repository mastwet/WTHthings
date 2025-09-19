#ifndef __RP_SETTING__
#define __RP_SETTING__

class RP_Settings {
    private:
        int* data_note;
        int* data_cc;
        const int dataSize = 9; // 数组的大小
        const int eepromStart = 0; // EEPROM中的起始地址

    public:
        RP_Settings(int* dataNote,int* dataCC) {
            data_note = dataNote;
            data_cc = dataCC;
        }

        void writeToEEPROM() {
            for (int i = 0; i < dataSize; i++) {
                EEPROM.write(eepromStart + i, data_note[i]);
            }
            if (EEPROM.commit()) {
                Serial.println("EEPROM successfully committed");
            } else {
                Serial.println("ERROR! EEPROM commit failed");
            }
        }

        void readFromEEPROM() {
            for (int i = 0; i < dataSize; i++) {
                data_note[i] = EEPROM.read(eepromStart + i);
            }
        }

        void bindArray(int* dataArray) {
            data_note = dataArray;
        }
};

#endif