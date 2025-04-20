#ifndef REGMAP_H
#define REGMAP_H

#include "WidgetBase.h"
#include <cstdint>
#include "../ImGuiWidget/RegData.h"

class RegMap : public WidgetBase
{
public:
    RegMap();
    virtual ~RegMap();

    virtual void Render() override;

public://定义各个寄存器数组
    uint8_t DEVICE_ID_MAP;
    uint8_t FAULT_SUMMARY_MAP;
    uint8_t STATUS1_MAP;
    uint8_t STATUS2_MAP;
    uint8_t COMMAND_MAP;
    uint8_t SPI_IN_MAP;
    uint8_t CONFIG1_MAP;
    uint8_t CONFIG2_MAP;
    uint8_t CONFIG3_MAP;
    uint8_t CONFIG4_MAP;

private:
    void RenderRegisterRow(const char* name, uint8_t address, uint8_t* reg, bool editable);
};


#endif
#pragma once
