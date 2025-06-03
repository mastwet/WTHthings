//
// Created by Fir on 2024/2/2.
//

#include "launcher.h"
#include "pinmap.h"

unsigned long lastDebounceTime = 0;
bool key_flag = false;

namespace astra {

void Launcher::popInfo(std::string _info, uint16_t _time) {
  static const uint64_t beginTime = this->time;
  static bool onRender = true;

  while (onRender) {
    time++;

    static float wPop = HAL::getFontWidth(_info) + 2 * getUIConfig().popMargin;  //宽度
    static float hPop = HAL::getFontHeight() + 2 * getUIConfig().popMargin;  //高度

    static float yPop = 0 - hPop - 8; //从屏幕上方滑入
    static float yPopTrg = 0;
    if (time - beginTime < _time) yPopTrg = (HAL::getSystemConfig().screenHeight - hPop) / 3;  //目标位置 中间偏上
    else yPopTrg = 0 - hPop - 8;  //滑出

    static float xPop = (HAL::getSystemConfig().screenWeight - wPop) / 2;  //居中

    HAL::canvasClear();
    /*渲染一帧*/
    currentPage->render(camera->getPosition());
    selector->render(camera->getPosition());
    camera->update(currentPage, selector);
    /*渲染一帧*/

    HAL::setDrawType(0);
    HAL::drawRBox(xPop - 4, yPop - 4, wPop + 8, hPop + 8, getUIConfig().popRadius + 2);
    HAL::setDrawType(1);  //反色显示
    HAL::drawRFrame(xPop - 1, yPop - 1, wPop + 2, hPop + 2, getUIConfig().popRadius);  //绘制一个圆角矩形
    HAL::drawEnglish(xPop + getUIConfig().popMargin, yPop + getUIConfig().popMargin + HAL::getFontHeight(), _info);  //绘制文字

    HAL::canvasUpdate();

    animation(&yPop, yPopTrg, getUIConfig().popSpeed);  //动画

    //todo 这里条件可以加上一个如果按键按下 就退出
    if (time - beginTime >= _time && yPop == 0 - hPop - 8) onRender = false;  //退出条件
  }
}

void Launcher::init(Menu *_rootPage) {
  currentPage = _rootPage;

  camera = new Camera(0, 0);
  _rootPage->init(camera->getPosition());

  selector = new Selector();
  selector->inject(_rootPage);
  //selector->go(_rootPage->selectIndex);

  selector->go(3);

  //open();
}

/**
 * @brief 打开选中的页面
 *
 * @return 是否成功打开
 * @warning 仅可调用一次
 */
bool Launcher::open() {
  //todo 打开和关闭都还没写完 应该还漏掉了一部分内容

  //如果当前页面指向的当前item没有后继 那就返回false
  if (currentPage->getNext() == nullptr) { popInfo("unreferenced page!", 600); return false; }
  if (currentPage->getNext()->getItemNum() == 0) { popInfo("empty page!", 600); return false; }
  ESP_LOGI(TAG, "1");
  currentPage->deInit();  //先析构（退场动画）再挪动指针
ESP_LOGI(TAG, "2");
  currentPage = currentPage->getNext();ESP_LOGI(TAG, "3");
  currentPage->init(camera->getPosition());ESP_LOGI(TAG, "4");

  selector->inject(currentPage);ESP_LOGI(TAG, "5");
  //selector->go(currentPage->selectIndex);

  return true;
}

/**
 * @brief 关闭选中的页面
 *
 * @return 是否成功关闭
 * @warning 仅可调用一次
 */
bool Launcher::close() {
  if (currentPage->getPreview() == nullptr) { popInfo("unreferenced page!", 600); return false; }
  if (currentPage->getPreview()->getItemNum() == 0) { popInfo("empty page!", 600); return false; }

  currentPage->deInit();  //先析构（退场动画）再挪动指针

  currentPage = currentPage->getPreview();
  currentPage->init(camera->getPosition());

  selector->inject(currentPage);
  //selector->go(currentPage->selectIndex);

  return true;
}

// 按键消抖函数
bool Launcher::myReadButton(int buttonPin) {
    static bool lastButtonState = HIGH;
    bool buttonState;
    bool reading = digitalRead(buttonPin);

    // 如果按键状态发生变化，重置消抖计时器
    if (reading != lastButtonState && !key_flag) {
        key_flag = true; 
        lastDebounceTime = millis();
        ESP_LOGI(TAG, "lastDebounceTime=%d", lastDebounceTime);
    }

    // 如果按键状态稳定超过50毫秒，认为按键状态有效
    if ((millis() - lastDebounceTime) > 40) {
      if (reading != lastButtonState) {
          ESP_LOGI(TAG, "确认按下=%d", millis());
          key_flag = false;
          return true; 
      }
    }
    // 超过时处理
    if ((millis() - lastDebounceTime) > 100) {
      key_flag = false;
    }
    return false;
}

void Launcher::update() {
    HAL::canvasClear();

    currentPage->render(camera->getPosition());
    selector->render(camera->getPosition());
    camera->update(currentPage, selector);

    // if (time == 100) selector->go(4);  //test
    // if (time == 150) open();  //test
    // if (time == 200) selector->go(1);  //test
    // if (time == 300) selector->go(3);  //test
    // if (time == 400) selector->go(4);  //test
    // if (time == 500) selector->go(1);  //test
    // if (time == 600) selector->go(0);  //test
    // if (time == 2500) open();  //test
    // if (time == 2900) close();
    // if (time == 700) selector->go(0);  //test
    // if (time >= 3250) time = 0;  //test
    // time++;
    // ESP_LOGI(TAG, "time=%d", time);
    HAL::canvasUpdate();


  //todo 看一下Rachel的按键扫描函数是怎么实现的
      // 读取按键1的状态并进行消抖处理
    if (myReadButton(KEY_PIN_1)) {
        ESP_LOGI(TAG, "m_index=%d", m_index);
        open();
    }
    if (myReadButton(KEY_PIN_2)) {
        ESP_LOGI(TAG, "m_index=%d", m_index);
        close();
    }
    if (myReadButton(KEY_PIN_3)) {
        m_index ++;
        ESP_LOGI(TAG, "m_index=%d", m_index);
        selector->go( m_index );
    }

    // 读取按键2的状态并进行消抖处理
    if (myReadButton(KEY_PIN_4)) {
        m_index --;
        ESP_LOGI(TAG, "m_index=%d", m_index);
        selector->go( m_index );
    }
  }
}