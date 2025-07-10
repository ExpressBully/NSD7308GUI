#include "RegOperateInt.h"
#include <imgui.h>
#include <iostream>
#include <bitset>
#include "RegData.h"
#include "RegMap.h"

bool m_IsRegChange = false; //初始化为false

RegOperateInt::RegOperateInt() : m_currentPage(0)
{
    // 初始化寄存器值为0
    COMMAND_INT = 0;
    SPI_IN_INT = 0;
    CONFIG1_INT = 0;
    CONFIG2_INT = 0;
    CONFIG3_INT = 0;
    CONFIG4_INT = 0;
    InitializeConfig();

    // 初始化选中选项
    for (const auto& page : m_pages) {
        std::vector<int> pageOptions;
        for (const auto& reg : page.registers) {
            pageOptions.push_back(0);
        }
        m_selectedOptions.push_back(pageOptions);
    }

}

RegOperateInt::~RegOperateInt()
{
}

void RegOperateInt::InitializeConfig()
{
    // COMMAND Register - 单独页面
    PageConfig command;
    command.name = "COMMAND";
    command.registers = {
        {"CLR_FLT[7]", "CLR_FLT", "", 1, {"0", "1"}},
        {"SPI_IN_LOCK[4:3]", "SPI_IN_LOCK", "", 2, {"00b LOCK", "01b LOCK", "10b UNLOCK", "11b LOCK"}},
        {"REG_LOCK[1:0]", "REG_LOCK", "", 2, {"00b LOCK", "01b LOCK", "10b UNLOCK", "11b LOCK"}}
    };
    m_pages.push_back(command);

    // SPI_IN Register - 单独页面
    PageConfig spi_in;
    spi_in.name = "SPI_IN";
    spi_in.registers = {
        {"S_DRVOFF[3]", "S_DRVOFF", "", 1, {"0", "1"}},
        {"S_DRVOFF2[2]", "S_DRVOFF2", "", 1, {"0", "1"}},
        {"S_EN_IN1[1]", "S_EN_IN1", "", 1, {"0", "1"}},
        {"S_PH_IN2[0]", "S_PH_IN2", "", 1, {"0", "1"}}
    };
    m_pages.push_back(spi_in);

    // CONFIG1 Register - 单独页面
    PageConfig config1;
    config1.name = "CONFIG1";
    config1.registers = {
        {"EN_OLA[7]", "EN_OLA", "", 1, {"0", "1"}},
        {"VMOV_SEL[6:5]", "VMOV_SEL", "VM>35V", 2, {"VM>35V", "VM>28V", "VM>18V", "VMOV_DIS"}},
        {"SSC_DIS[4]", "SSC_DIS", "SSC_DIS", 1, {"0", "1"}},
        {"OCP_RETRY[3]", "OCP_RETRY", "", 1, {"0", "1"}},
        {"OTD_RETRY[2]", "OTD_RETRY", "", 1, {"0", "1"}},
        {"VMOV_RETRY[1]", "VMOV_RETRY", "", 1, {"0", "1"}},
        {"OLA_RETRY[0]", "OLA_RETRY", "", 1, {"0", "1"}}
    };
    m_pages.push_back(config1);

    // CONFIG2 Register - 单独页面
    PageConfig config2;
    config2.name = "CONFIG2";
    config2.registers = {
        {"PWM_EXTEND[7]", "PWM_EXTEND", "", 1, {"0", "1"}},
        {"S_DIAG[6:5]", "S_DIAG", "", 2, {"0", "1", "2", "3"}},
        {"S_ITRIP[2:0]", "S_ITRIP", "Regulation Diabled", 3, {"Regulation Diasbled", "VITRIP=1.18V","VITRIP=1.41V","VITRIP=1.65V","VITRIP=1.98V","VITRIP=2.31V","VITRIP=2.64V","VITRIP=2.97V"}}
    };
    m_pages.push_back(config2);

    // CONFIG3 Register - 单独页面
    PageConfig config3;
    config3.name = "CONFIG3";
    config3.registers = {
        {"TOFF[7:6]", "TOFF", "", 2, {"TOFF=20us", "TOFF=30us", "TOFF=40us", "TOFF=50us"}},
        {"S_SR[4:2]", "S_SR", "", 3, {"SR=3'b000", "SR=3'b001", "SR=3'b010", "SR=3'b011", "SR=3'b100", "SR=3'b101", "SR=3'b110", "SR=3'b111"}},
        {"SMODE[1:0]", "SMODE", "", 2, {"PH/EN Mode", "Independent Mode", "PWM Mode"}}
    };
    m_pages.push_back(config3);

    // CONFIG4 Register - 单独页面
    PageConfig config4;
    config4.name = "CONFIG4";
    config4.registers = {
        {"TOCP_SEL[7:6]", "TOCP_SEL", "", 2, {"tOCP=6us", "tOCP=3us", "tOCP=1.5us", "tOCP=Minimum"}},
        {"OCP_SEL[4:3]", "OCP_SEL", "", 2, {"IOCP=100%", "IOCP=50%", "IOCP=50%", "IOCP=75%"}},
        {"DRVOFF_SEL[2]", "DRVOFF_SEL(SPI_IN unlocked)", "", 1, {"DRV=DRVOFF PIN (OR) S_DRVOFF2", "DRV=DRVOFF PIN (AND) S_DRVOFF2"}},
        {"EN_IN1_SEL[1]", "EN_IN1_SEL", "", 1, {"0", "1"}},
        {"PH_IN2_SEL[0]", "PH_IN2_SEL", "", 1, {"0", "1"}}
    };
    m_pages.push_back(config4);
}

uint8_t RegOperateInt::ComposeRegisterValue(int pageIndex)
{
    uint8_t value = 0;
    int definedBits = 0;
    std::vector<bool> bitCoverage(8, false); // 跟踪哪些位已被定义

    // 先收集所有已定义的位
    for (size_t i = 0; i < m_pages[pageIndex].registers.size(); ++i) {
        const auto& reg = m_pages[pageIndex].registers[i];
        int startBit = 0;

        // 从寄存器名称中提取位位置
        size_t bracketPos = reg.address.find('[');
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

        // 标记这些位已被定义
        for (int j = 0; j < reg.bitWidth; ++j) {
            if (startBit - j >= 0 && startBit - j < 8) {
                bitCoverage[startBit - j] = true;
            }
        }
    }

    // 从最高位(7)到最低位(0)处理
    for (int currentBit = 7; currentBit >= 0; --currentBit) {
        bool bitDefined = false;
        int bitValue = 0;

        // 检查这个位是否在某个寄存器定义中
        for (size_t i = 0; i < m_pages[pageIndex].registers.size(); ++i) {
            const auto& reg = m_pages[pageIndex].registers[i];
            int startBit = 0;
            size_t bracketPos = reg.address.find('[');
            if (bracketPos != std::string::npos) {
                std::string bitRange = reg.address.substr(bracketPos + 1);
                bitRange = bitRange.substr(0, bitRange.find(']'));

                size_t colonPos = bitRange.find(':');
                if (colonPos != std::string::npos) {
                    // 多位范围 [x:y]
                    startBit = std::stoi(bitRange.substr(0, colonPos));
                    int endBit = std::stoi(bitRange.substr(colonPos + 1));

                    if (currentBit <= startBit && currentBit >= endBit) {
                        int offset = startBit - currentBit;
                        bitValue = (m_selectedOptions[pageIndex][i] >> offset) & 0x1;
                        bitDefined = true;
                        break;
                    }
                }
                else {
                    // 单一位 [x]
                    startBit = std::stoi(bitRange);
                    if (currentBit == startBit) {
                        bitValue = m_selectedOptions[pageIndex][i] & 0x1;
                        bitDefined = true;
                        break;
                    }
                }
            }
        }

        // 如果这个位没有被任何寄存器定义，则保持为0
        if (!bitDefined) {
            bitValue = 0;
        }

        value |= (bitValue << currentBit);
    }

    return value;
}

void RegOperateInt::Render()
{
    if (!mIsRender) { //TestMode Page的时候不渲染
        return;
    }

    //COMMAND = 0x01;
// 窗口设置保持不变
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 window_size(viewport->WorkSize.x * 0.2f, viewport->WorkSize.y * 0.8f);
    ImVec2 window_pos(viewport->WorkPos.x + viewport->WorkSize.x - window_size.x, viewport->WorkPos.y);

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    ImGui::Begin("Register Configuration", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // 顶部按钮区域
    ImGui::BeginChild("##TopButtons", ImVec2(0, 90), false);
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 5));

        // 第一行按钮 (前3个)
        ImGui::BeginGroup();
        for (int i = 0; i < 3; ++i) {
            if (i >= m_pages.size()) {
                ImGui::EndGroup(); // 结束group
                break;
            }
            bool isCurrent = (m_currentPage == i);
            // 为当前选中的按钮设置样式
            if (isCurrent) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.6f));
            }

            if (ImGui::Button(m_pages[i].name.c_str(), ImVec2(80, 25))) {
                m_currentPage = i;
                RegIntDataUpdate_From_RegData();
                UpdateUIFromRegisters();
            }

            // 恢复当前选中按钮的样式
            if (isCurrent) {
                ImGui::PopStyleColor();
            }

            if (i < 2) ImGui::SameLine();
        }
        ImGui::EndGroup();

        // 第二行按钮 (后3个)
        ImGui::BeginGroup();
        for (int i = 3; i < 6; ++i) {
            if (i >= m_pages.size()) {
                ImGui::EndGroup(); // 确保结束group
                break;
            }
            bool isCurrent = (m_currentPage == i);
            // 为当前选中的按钮设置样式
            if (isCurrent) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.6f));
            }

            if (ImGui::Button(m_pages[i].name.c_str(), ImVec2(80, 25))) {
                m_currentPage = i;
                RegIntDataUpdate_From_RegData();
                UpdateUIFromRegisters();
            }

            // 恢复当前选中按钮的样式
            if (isCurrent) {
                ImGui::PopStyleColor();
            }

            if (i < 5) ImGui::SameLine();
        }
        ImGui::EndGroup();

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();

    // 每次渲染前自动更新UI显示
    RegIntDataUpdate_From_RegData();
    UpdateUIFromRegisters();

    // 主内容区
    ImGui::BeginChild("##Content", ImVec2(0, -35), true);
    {
        if (m_currentPage >= 0 && m_currentPage < m_pages.size()) {
            const auto& page = m_pages[m_currentPage];
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 8));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));

            for (size_t i = 0; i < page.registers.size(); ++i) {
                const auto& reg = page.registers[i];
                ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", reg.address.c_str());
                ImGui::TextColored(ImVec4(0.26f, 0.59f, 0.98f, 1.0f), "%s", reg.displayName.c_str());

                if (reg.bitWidth == 1) {
                    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
                    bool val = m_selectedOptions[m_currentPage][i] != 0;
                    if (ImGui::Checkbox(("##" + std::to_string(i)).c_str(), &val)) {
                        m_selectedOptions[m_currentPage][i] = val ? 1 : 0;
                        // 用户修改UI时立即更新寄存器
                        uint8_t regValue = ComposeRegisterValue(m_currentPage);
                      switch (m_currentPage) {
                        case 0:
                        {
                            COMMAND_INT = regValue;
                            ClickAddress = 0x08;
                        }break;
                        case 1:
                        {
                            SPI_IN_INT = regValue;
                            ClickAddress = 0x09;
                        }break;
                        case 2: 
                        {
                            CONFIG1_INT = regValue;
                            ClickAddress = 0x0A;
                        }break;
                        case 3:
                        {
                            CONFIG2_INT = regValue;
                            ClickAddress = 0x0B;
                        }break;
                        case 4:
                        {
                            CONFIG3_INT = regValue;
                            ClickAddress = 0x0C;
                        }break;
                        case 5:
                        {
                            CONFIG4_INT = regValue;
                            ClickAddress = 0x0D;
                        }break;
                       }
                        RegIntDataUpdate_to_RegData(); //更新到底层寄存器
                        m_IsRegChange = true; //表明修改寄存器，如果RegMap开了 立即写 功能，就依靠这个标志位判断
                        
                       
                    }
                    ImGui::PopStyleVar();

                    if (!reg.description.empty()) {
                        ImGui::SameLine();
                        ImGui::Text("%s", reg.description.c_str());
                    }
                }
                else {
                    std::string preview = reg.options[m_selectedOptions[m_currentPage][i]];
                    if (ImGui::BeginCombo(("##combo_" + std::to_string(i)).c_str(), preview.c_str())) {
                        for (int j = 0; j < reg.options.size(); ++j) {
                            bool isSelected = (m_selectedOptions[m_currentPage][i] == j);
                            if (ImGui::Selectable(reg.options[j].c_str(), isSelected)) {
                                m_selectedOptions[m_currentPage][i] = j;
                                // 用户修改UI时立即更新寄存器
                                uint8_t regValue = ComposeRegisterValue(m_currentPage);
                              switch (m_currentPage) {
                                case 0:
                                {
                                    COMMAND_INT = regValue;
                                    ClickAddress = 0x08;
                                }break;
                                case 1:
                                {
                                    SPI_IN_INT = regValue;
                                    ClickAddress = 0x09;
                                }break;
                                case 2:
                                {
                                    CONFIG1_INT = regValue;
                                    ClickAddress = 0x0A;
                                }break;
                                case 3:
                                {
                                    CONFIG2_INT = regValue;
                                    ClickAddress = 0x0B;
                                }break;
                                case 4:
                                {
                                    CONFIG3_INT = regValue;
                                    ClickAddress = 0x0C;
                                }break;
                                case 5:
                                {
                                    CONFIG4_INT = regValue;
                                    ClickAddress = 0x0D;
                                }break;
                               }
                                RegIntDataUpdate_to_RegData(); //更新到底层寄存器
                                m_IsRegChange = true; //表明修改寄存器，如果RegMap开了 立即写 功能，就依靠这个标志位判断
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                        
                    }
                }
                ImGui::Dummy(ImVec2(0, 6));
            }
            ImGui::PopStyleVar(2);
        }
        RegIntDataUpdate_to_RegData(); //更新到底层寄存器
    }
    ImGui::EndChild();

    // 底部只有一个刷新按钮
    ImGui::SetCursorPosY(ImGui::GetWindowHeight() - 30);
    if (ImGui::Button("Refresh", ImVec2(-1, 25))) {

        ////////////打印所有寄存器值////////////
        std::cout << "COMMAND: "
            << "Binary: " << std::bitset<8>(COMMAND)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)COMMAND << std::dec
            << std::endl;
        std::cout << "COMMAND_INT: "
            << "Binary: " << std::bitset<8>(COMMAND_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)COMMAND_INT << std::dec
            << std::endl;
        std::cout << "SPI_IN_INT: "
            << "Binary: " << std::bitset<8>(SPI_IN_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)SPI_IN_INT << std::dec
            << std::endl;
        std::cout << "CONFIG1_INT: "
            << "Binary: " << std::bitset<8>(CONFIG1_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG1_INT << std::dec
            << std::endl;
        std::cout << "CONFIG2_INT: "
            << "Binary: " << std::bitset<8>(CONFIG2_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG2_INT << std::dec
            << std::endl;
        std::cout << "CONFIG3_INT: "
            << "Binary: " << std::bitset<8>(CONFIG3_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG3_INT << std::dec
            << std::endl;
        std::cout << "CONFIG4_INT: "
            << "Binary: " << std::bitset<8>(CONFIG4_INT)
            << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG4_INT << std::dec
            << std::endl;
        /////////////////////////////////
        RegIntDataUpdate_to_RegData();
    }

    ImGui::End();
}