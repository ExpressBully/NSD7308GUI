#ifndef REGMAP_H
#define REGMAP_H

#include "WidgetBase.h"
#include <cstdint>

class RegMap : public WidgetBase
{
public:
    RegMap();
    virtual ~RegMap();

    virtual void Render() override;

public://定义各个寄存器数组
    uint8_t DEVICE_ID;
    uint8_t FAULT_SUMMARY;
    uint8_t STATUS1;
    uint8_t STATUS2;
    uint8_t COMMAND;
    uint8_t SPI_IN;
    uint8_t CONFIG1;
    uint8_t CONFIG2;
    uint8_t CONFIG3;
    uint8_t CONFIG4;

private:
    void RenderRegisterRow(const char* name, uint8_t address, uint8_t* reg, bool editable);
};


#endif
#pragma once
