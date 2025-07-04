//
// Created by Fir on 2024/1/25.
//

#pragma once
#ifndef ASTRA_CORE_SRC_SYSTEM_H_
#define ASTRA_CORE_SRC_SYSTEM_H_

#include "cstdint"
#include <U8g2lib.h>

namespace astra {
/**
 * @brief config of astra ui. astra ui的配置结构体
 */
struct config {
  float tileAnimationSpeed = 80;
  float listAnimationSpeed = 65;
  float selectorYAnimationSpeed = 52;
  float selectorXAnimationSpeed = 70;
  float selectorWidthAnimationSpeed = 70;
  float selectorHeightAnimationSpeed = 55;
  float windowAnimationSpeed = 25;
  float sideBarAnimationSpeed = 15;
  float fadeAnimationSpeed = 100;
  float cameraAnimationSpeed = 80;
  float logoAnimationSpeed = 70;

  bool tileUnfold = true;
  bool listUnfold = true;

  bool tileLoop = true;
  bool menuLoop = true;

  bool backgroundBlur = true;
  bool lightMode = false;

  float listBarWeight = 5;
  float listTextHeight = 8;
  float listTextMargin = 4; //文字边距
  float listLineHeight = 16;
  float selectorRadius = 0.5f;
  float selectorMargin = 4; //选择框与文字左边距
  float selectorTopMargin = 2; //选择框与文字上边距

  uint8_t listPageTurningMode = 1; //0: 翻页模式 1: 滚动模式

  float tilePicWidth = 30;
  float tilePicHeight = 30;
  float tilePicMargin = 8;
  float tilePicTopMargin = 8; //图标上边距
  float tileArrowWidth = 6;
  float tileArrowMargin = 4; //箭头边距

  //todo 如果有问题 给下面这三个分别+1
  float tileDottedLineBottomMargin = 18; //虚线下边距(top: 46)
  float tileArrowBottomMargin = 8; //箭头下边距(top: 56)
  float tileTextBottomMargin = 12; //标题下边距(top: 52)

  float tileBarHeight = 2; //磁贴进度条高度

  float tileSelectBoxLineLength = 5;  //磁贴选择框线长
  float tileSelectBoxMargin = 3; //选择框边距
  float tileSelectBoxWidth = tileSelectBoxMargin * 2 + tilePicWidth; //选择框宽
  float tileSelectBoxHeight = tileSelectBoxMargin * 2 + tilePicHeight; //选择框高
  float tileTitleHeight = 8; //磁贴标题高度

  float tileBtnMargin = 16; //按钮边距

  float popMargin = 4; //弹窗边距
  float popRadius = 2; //弹窗圆角半径
  float popSpeed = 90; //弹窗动画速度

  float logoStarLength = 2; //logo星星长度
  float logoTextHeight = 14; //logo文字高度
  float logoCopyRightHeight = 8; //logo文字高度
  uint8_t logoStarNum = 16; //logo星星数量
  const uint8_t *logoTitleFont = u8g2_font_4x6_tr;
  const uint8_t *logoCopyRightFont = u8g2_font_4x6_tr;

  const uint8_t *mainFont = u8g2_font_4x6_tr;
};

static config &getUIConfig() {
  static config astraConfig;
  return astraConfig;
}
}
#endif //ASTRA_CORE_SRC_SYSTEM_H_