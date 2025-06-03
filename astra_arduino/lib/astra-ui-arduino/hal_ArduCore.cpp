//
// Created by mastwet on 24-8-17.
//
#include "hal.h"
#include "hal_ArduCore.h"


//原生用法，尚不明确如何获取buffer
//原生也足够好用了，没啥必要（爆论）
// void HALArduCore::_u8g2_init(){
//    U8G2_SSD1306_128X64_NONAME_F_HW_I2C *u8g2;
//    u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, DISPLAY_SCL, DISPLAY_SDA);
    // ESP_LOGI(TAG, "u8g2_init");
    // U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2( U8G2_R0, /* clock=*/ DISPLAY_SCL, /* data=*/ DISPLAY_SDA, /* reset=*/ U8X8_PIN_NONE );
    // u8g2.setFont( u8g2_font_wqy12_t_gb2312 );
    // u8g2.setFont(u8g2_font_luBIS08_tf);   // choose a suitable font
    // u8g2.begin();
    // u8g2.clearBuffer();                   // clear the internal memory
    // u8g2.drawStr(0,10,"Hello Seeed!");    // write something to the internal memory
    // u8g2.drawStr(0,30,"Hello Studio!");    // write something to the internal memory
    // u8g2.drawStr(0,50,"Hello World!");    // write something to the internal memory
    // u8g2.sendBuffer();                    // transfer internal memory to the display
//     delay(100);
//     ESP_LOGI(TAG, "u8g2 end");
// }

void HALArduCore::_u8g2_init(){
    ESP_LOGI(TAG, "u8g2_init");

    u8x8_SetPin_HW_I2C(u8g2_GetU8x8(&canvasBuffer), U8X8_PIN_NONE, DISPLAY_SCL, DISPLAY_SDA);
    // u8x8_SetPin_SW_I2C(u8g2_GetU8x8(&canvasBuffer), DISPLAY_SCL, DISPLAY_SDA, U8X8_PIN_NONE);
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&canvasBuffer,
                                            U8G2_R0,
                                            // u8x8_byte_sw_i2c,
                                            u8x8_byte_arduino_hw_i2c,
                                            u8x8_gpio_and_delay_arduino);  // 初始化 u8g2 结构体
                                            
    ESP_LOGI(TAG, "setup done");
    /*T-ODO：根据几个接口多态化init函数，例如u8x8_SetPin_4Wire_SW_SPI这个接口等等*/

    //以下代码等于u8g2.begin()
    u8g2_InitDisplay(&canvasBuffer); // 根据所选的芯片进行初始化工作，初始化完成后，显示器处于关闭状态
    u8g2_SetPowerSave(&canvasBuffer, 0); // 打开显示器
    ESP_LOGI(TAG, "init display done");
    //u8g2_ClearBuffer(&canvasBuffer);


    u8g2_SetFontMode(&canvasBuffer, 1); /*字体模式选择*/
    u8g2_SetFontDirection(&canvasBuffer, 0); /*字体方向选择*/
    //u8g2_SetFont(&canvasBuffer, u8g2_font_inb24_mf); /*字库选择*/
    ESP_LOGI(TAG, "setfont done");
    //delay(1000);
    //请找一个合适的字体，随意咯
    // u8g2_DrawStr(&canvasBuffer, 0, 20, "U");

    // u8g2_SetFontDirection(&canvasBuffer, 1);
    // u8g2_SetFont(&canvasBuffer, u8g2_font_inb30_mn);
    // u8g2_DrawStr(&canvasBuffer, 21,8,"8");
        
    // u8g2_SetFontDirection(&canvasBuffer, 0);
    // u8g2_SetFont(&canvasBuffer, u8g2_font_inb24_mf);
    // u8g2_DrawStr(&canvasBuffer, 51,30,"g");
    // u8g2_DrawStr(&canvasBuffer, 67,30,"\xb2");
    
    // u8g2_DrawHLine(&canvasBuffer, 2, 35, 47);
    // u8g2_DrawHLine(&canvasBuffer, 3, 36, 47);
    // u8g2_DrawVLine(&canvasBuffer, 45, 32, 12);
    // u8g2_DrawVLine(&canvasBuffer, 46, 33, 12);
  
    // u8g2_SetFont(&canvasBuffer, u8g2_font_4x6_tr);
    // u8g2_DrawStr(&canvasBuffer, 1,54,"github.com/olikraus/u8g2");
    // u8g2_SendBuffer(&canvasBuffer);
    ESP_LOGI(TAG, "u8g2 end");
}


void HALArduCore::_screenOn() {
    //关闭屏幕
}

void HALArduCore::_screenOff() {
    //开启屏幕
}

void *HALArduCore::_getCanvasBuffer() {
    return u8g2_GetBufferPtr(&canvasBuffer);
}

unsigned char HALArduCore::_getBufferTileHeight() {
    return u8g2_GetBufferTileHeight(&canvasBuffer);
}

unsigned char HALArduCore::_getBufferTileWidth() {
    return u8g2_GetBufferTileWidth(&canvasBuffer);
}

void HALArduCore::_canvasUpdate() {
    u8g2_SendBuffer(&canvasBuffer);
}

void HALArduCore::_canvasClear() {
    u8g2_ClearBuffer(&canvasBuffer);
}

void HALArduCore::_setFont(const unsigned char *_font) {
    u8g2_SetFontMode(&canvasBuffer, 1); /*字体模式选择*/
    u8g2_SetFontDirection(&canvasBuffer, 0); /*字体方向选择*/
    u8g2_SetFont(&canvasBuffer, _font);
}

unsigned char HALArduCore::_getFontWidth(std::string &_text) {
    return u8g2_GetUTF8Width(&canvasBuffer, _text.c_str());
}

unsigned char HALArduCore::_getFontHeight() {
    return u8g2_GetMaxCharHeight(&canvasBuffer);
}

void HALArduCore::_setDrawType(unsigned char _type) {
    u8g2_SetDrawColor(&canvasBuffer, _type);
}

void HALArduCore::_drawPixel(float _x, float _y) {
    u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y));
}

void HALArduCore::_drawEnglish(float _x, float _y, const std::string &_text) {
    u8g2_DrawStr(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void HALArduCore::_drawChinese(float _x, float _y, const std::string &_text) {
    u8g2_DrawUTF8(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), _text.c_str());
}

void HALArduCore::_drawVDottedLine(float _x, float _y, float _h) {
    for (unsigned char i = 0; i < (unsigned char)std::round(_h); i++) {
        if (i % 8 == 0 | (i - 1) % 8 == 0 | (i - 2) % 8 == 0) continue;
        u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y) + i);
    }
}

void HALArduCore::_drawHDottedLine(float _x, float _y, float _l) {
    for (unsigned char i = 0; i < _l; i++) {
        if (i % 8 == 0 | (i - 1) % 8 == 0 | (i - 2) % 8 == 0) continue;
        u8g2_DrawPixel(&canvasBuffer, (int16_t)std::round(_x) + i, (int16_t)std::round(_y));
    }
}

void HALArduCore::_drawVLine(float _x, float _y, float _h) {
    u8g2_DrawVLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_h));
}

void HALArduCore::_drawHLine(float _x, float _y, float _l) {
    u8g2_DrawHLine(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_l));
}

void HALArduCore::_drawBMP(float _x, float _y, float _w, float _h, const unsigned char *_bitMap) {
    u8g2_DrawXBMP(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), _bitMap);
}

void HALArduCore::_drawBox(float _x, float _y, float _w, float _h) {
    u8g2_DrawBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

void HALArduCore::_drawRBox(float _x, float _y, float _w, float _h, float _r) {
    u8g2_DrawRBox(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}

void HALArduCore::_drawFrame(float _x, float _y, float _w, float _h) {
    u8g2_DrawFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h));
}

void HALArduCore::_drawRFrame(float _x, float _y, float _w, float _h, float _r) {
    u8g2_DrawRFrame(&canvasBuffer, (int16_t)std::round(_x), (int16_t)std::round(_y), (int16_t)std::round(_w), (int16_t)std::round(_h), (int16_t)std::round(_r));
}


bool HALArduCore::_getKey(key::KEY_INDEX _keyIndex) {
    if (_keyIndex == key::KEY_0) return digitalRead(KEY_PIN_1);
    if (_keyIndex == key::KEY_1) return !digitalRead(KEY_PIN_2);;
}

void HALArduCore::_updateConfig() {
    HAL::_updateConfig();
}

void HALArduCore::_setBeepVol(unsigned char _vol) {
    HAL::_setBeepVol(_vol);
}

void HALArduCore::_beepStop() {
    HAL::_beepStop();
}

void HALArduCore::_beep(float _freq) {
    HAL::_beep(_freq);
}

unsigned long HALArduCore::_getRandomSeed() {
    return analogRead(0);
}

//get_tick是否和millis冲突了
unsigned long HALArduCore::_getTick() {
    return _millis();
}

unsigned long HALArduCore::_millis() {
    return millis();
}

void HALArduCore::_delay(unsigned long _mill) {
    delay(_mill);
}
//有关于外设初始化在主函数，不放在astra内
/*void HALArduCore::_key_init() {
    pinMode(KEY_PIN_1,INPUT);
    pinMode(KEY_PIN_2,INPUT);
}*/
