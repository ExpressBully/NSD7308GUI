#ifndef REGDATA_H
#define REGDATA_H

#define FrameLength 10


#include "WidgetBase.h"
#include <cstdint>

// 声明全局寄存器变量
extern uint8_t DEVICE_ID;
extern uint8_t FAULT_SUMMARY;
extern uint8_t STATUS1;
extern uint8_t STATUS2;
extern uint8_t COMMAND;
extern uint8_t SPI_IN;
extern uint8_t CONFIG1;
extern uint8_t CONFIG2;
extern uint8_t CONFIG3;
extern uint8_t CONFIG4;

//声明全局寄存器所用的标志位和变量
extern bool ClickFlag; //记录GUI寄存器变量是否更改的Flag
extern uint8_t ClickAddress; //记录click按下的时候变更的寄存器地址

class RegData : public WidgetBase
{
    // 类定义保持不变（如果需要）
};

#endif