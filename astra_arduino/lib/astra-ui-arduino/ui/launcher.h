//
// Created by Fir on 2024/2/2.
//
#pragma once
#ifndef ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_
#define ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_

#include "element/page/item.h"

namespace astra {

class Launcher : public Animation {
private:
  Menu* currentPage;
  Selector* selector;
  Camera* camera;

  uint64_t time;
  uint8_t m_index = 0;
public:
  void popInfo(std::string _info, uint16_t _time);

  void init(Menu* _rootPage);

  bool open();
  bool close();

  void update();
  bool myReadButton(int buttonPin);
  Camera* getCamera() { return camera; }
  Selector* getSelector() { return selector; }
};
}

#endif //ASTRA_CORE_SRC_ASTRA_UI_SCHEDULER_H_