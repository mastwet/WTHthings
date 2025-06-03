//
// Created by mastwet on 24-8-17.
//

#ifndef __HAL_ARDUCORE__
#define __HAL_ARDUCORE__

#include <Arduino.h>
#include "hal.h"
#include <U8g2lib.h>
#include "U8x8lib.h"
#include "pinmap.h"
// #include "stdio.h"
// #include "esp_log.h"

// static const char *TAG = "example";
class HALArduCore : public HAL {
private:
    //似乎原声astra不兼容arduino传统用法
    //U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2;
    u8g2_t canvasBuffer {};
    void _u8g2_init();
    //void _key_init();

public:
    inline void init() override {
        _u8g2_init();
    }

//protected:
//    void _ssd1306_transmit_cmd(unsigned char _cmd);
//    void _ssd1306_transmit_data(unsigned char _data, unsigned char _mode);
//    void _ssd1306_reset(bool _state);
//    void _ssd1306_set_cursor(unsigned char _x, unsigned char _y);
//    void _ssd1306_fill(unsigned char _data);
//像这一个部分已经被u8g2抽象掉了，所以不用实现


public:
    void _screenOn() override;
    void _screenOff() override;

public:
    void* _getCanvasBuffer() override;
    unsigned char _getBufferTileHeight() override;
    unsigned char _getBufferTileWidth() override;
    void _canvasUpdate() override;
    void _canvasClear() override;
    void _setFont(const unsigned char * _font) override;
    unsigned char _getFontWidth(std::string& _text) override;
    unsigned char _getFontHeight() override;
    void _setDrawType(unsigned char _type) override;
    void _drawPixel(float _x, float _y) override;
    void _drawEnglish(float _x, float _y, const std::string& _text) override;
    void _drawChinese(float _x, float _y, const std::string& _text) override;
    void _drawVDottedLine(float _x, float _y, float _h) override;
    void _drawHDottedLine(float _x, float _y, float _l) override;
    void _drawVLine(float _x, float _y, float _h) override;
    void _drawHLine(float _x, float _y, float _l) override;
    void _drawBMP(float _x, float _y, float _w, float _h, const unsigned char* _bitMap) override;
    void _drawBox(float _x, float _y, float _w, float _h) override;
    void _drawRBox(float _x, float _y, float _w, float _h, float _r) override;
    void _drawFrame(float _x, float _y, float _w, float _h) override;
    void _drawRFrame(float _x, float _y, float _w, float _h, float _r) override;

public:
    void _delay(unsigned long _mill) override;
    unsigned long _millis() override;
    unsigned long _getTick() override;
    unsigned long _getRandomSeed() override;

public:
    void _beep(float _freq) override;
    void _beepStop() override;
    void _setBeepVol(unsigned char _vol) override;

public:
    bool _getKey(key::KEY_INDEX _keyIndex) override;

public:
    void _updateConfig() override;
};

#endif //UNTITLED1_HAL_ARDUCORE_H
