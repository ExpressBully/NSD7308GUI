#ifndef REGOPERATEINT_H
#define REGOPERATEINT_H

#include "WidgetBase.h"
#include <cstdint>
#include <vector>
#include <string>
#include "../ImGuiWidget/RegData.h"
#include "RegMap.h"

extern bool m_IsRegChange; //给立即写用的，当RegMap立即写打开，这里只要修改了

class RegOperateInt : public WidgetBase
{
public:
    RegOperateInt();
    virtual ~RegOperateInt();

    virtual void Render() override;


    uint8_t COMMAND_INT;
    uint8_t SPI_IN_INT;
    uint8_t CONFIG1_INT;
    uint8_t CONFIG2_INT;
    uint8_t CONFIG3_INT;
    uint8_t CONFIG4_INT;


private:

    void RegIntDataUpdate_From_RegData() {
        COMMAND_INT = COMMAND;
        SPI_IN_INT = SPI_IN;
        CONFIG1_INT = CONFIG1;
        CONFIG2_INT = CONFIG2;
        CONFIG3_INT = CONFIG3;
        CONFIG4_INT = CONFIG4;
    };

    void RegIntDataUpdate_to_RegData() {

        COMMAND = COMMAND_INT;
        SPI_IN = SPI_IN_INT;
        CONFIG1 = CONFIG1_INT;
        CONFIG2 = CONFIG2_INT;
        CONFIG3 = CONFIG3_INT;
        CONFIG4 = CONFIG4_INT;
    };

    void RegOperateInt::UpdateUIFromRegisters()
    {
        // 遍历所有页面
        for (int pageIndex = 0; pageIndex < m_pages.size(); ++pageIndex) {
            uint8_t regValue = 0;

            // 获取当前页面对应的寄存器值
            switch (pageIndex) {
            case 0: regValue = COMMAND_INT; break;
            case 1: regValue = SPI_IN_INT; break;
            case 2: regValue = CONFIG1_INT; break;
            case 3: regValue = CONFIG2_INT; break;
            case 4: regValue = CONFIG3_INT; break;
            case 5: regValue = CONFIG4_INT; break;
            }

            // 解析寄存器值到UI选项
            int currentBit = 7; // 从最高位开始
            for (size_t i = 0; i < m_pages[pageIndex].registers.size(); ++i) {
                const auto& reg = m_pages[pageIndex].registers[i];
                int startBit = 0;
                size_t bracketPos = reg.address.find('[');

                // 提取位位置
                if (bracketPos != std::string::npos) {
                    std::string bitRange = reg.address.substr(bracketPos + 1);
                    bitRange = bitRange.substr(0, bitRange.find(']'));

                    size_t colonPos = bitRange.find(':');
                    if (colonPos != std::string::npos) {
                        // 多位范围 [x:y]
                        startBit = std::stoi(bitRange.substr(0, colonPos));
                    }
                    else {
                        // 单一位 [x]
                        startBit = std::stoi(bitRange);
                    }
                }

                // 提取对应的位值
                int value = 0;
                for (int j = 0; j < reg.bitWidth; ++j) {
                    if (startBit - j >= 0 && startBit - j < 8) {
                        int bitPos = startBit - j;
                        int bitValue = (regValue >> bitPos) & 0x1;
                        value |= (bitValue << j);
                    }
                }

                // 更新UI选项
                m_selectedOptions[pageIndex][i] = value;
            }
        }
    }

    struct RegisterConfig {
        std::string address;      // 如 "CONTROL/CONFIG1/EN_OLA[7]"
        std::string displayName;  // 如 "EN_ÓLA"
        std::string description;  // 如 "VM>35V"（部分寄存器有额外描述）
        int bitWidth;
        std::vector<std::string> options;
    };

    // 页面配置
    struct PageConfig {
        std::string name;
        std::vector<RegisterConfig> registers;
    };

    // 当前选中的页面
    int m_currentPage;

    // 所有页面配置
    std::vector<PageConfig> m_pages;

    // 当前选中的选项（按页面和寄存器索引）
    std::vector<std::vector<int>> m_selectedOptions;

    // 初始化配置
    void InitializeConfig();

    // 将选项组合成寄存器值
    uint8_t ComposeRegisterValue(int pageIndex);
};

#endif // REGOPERATEINT_H