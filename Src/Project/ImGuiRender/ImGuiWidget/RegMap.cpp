#include <iostream>
#include "imgui/imgui.h"
#include "RegMap.h"
#include "RegData.h"
#include "UART_DEC.h"
#include "../ImGuiWidget/UART_DEC.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <bitset>
#include <iostream>
#include "UART_DEC.h"
#include "imgui/imgui.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <stdexcept>
#include "RegOperateInt.h"

uint8_t DEVICE_ID = 0x01;
uint8_t FAULT_SUMMARY = 0x00;
uint8_t STATUS1 = 0x00;
uint8_t STATUS2 = 0x00;
uint8_t COMMAND = 0x00;
uint8_t SPI_IN = 0x00;
uint8_t CONFIG1 = 0x00;
uint8_t CONFIG2 = 0x00;
uint8_t CONFIG3 = 0x00;
uint8_t CONFIG4 = 0x00;

bool ClickFlag = false;
uint8_t ClickAddress = 0; //记录click按下的时候变更的寄存器地址
Timer timerwriteimmediately;

RegMap::RegMap()
{
    //ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontFromFileTTF("your_font.ttf", 12.0f); // 正常字体
    //smallFont = io.Fonts->AddFontFromFileTTF("your_font.ttf", 5.0f); // 小号字体
    // 初始化寄存器
    RegMapDataUpdate_From_RegData();
}

RegMap::~RegMap()
{
}

void RegMap::Render()
{
    if (!mIsRender) {
        return;
    }
    // 获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 main_window_size = viewport->WorkSize;
    ImVec2 main_window_pos = viewport->WorkPos;

    // UART窗口参数（与UART_DEC::Render()保持一致）
    const float uart_width_ratio = 0.3f;  // UART窗口宽度占30%
    const float uart_height_ratio = 0.5f; // UART窗口高度占50%
    const float RegMap_width_ratio = 0.5f;

    const float uart_width = main_window_size.x * uart_width_ratio;
    const float spacing = 0.0f;          // 两窗口间距

    // 设置寄存器窗口尺寸
    ImVec2 table_window_size(
        main_window_size.x * RegMap_width_ratio,
        main_window_size.y * uart_height_ratio
    );

    // 计算位置（UART窗口右侧+间距）
    ImVec2 table_window_pos(
        main_window_pos.x + uart_width + spacing,
        main_window_pos.y + spacing
    );

    // 设置窗口属性
    ImGui::SetNextWindowPos(table_window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(table_window_size, ImGuiCond_Always);

    // 窗口标志
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

    ////Write immediately 情况下的更新
    //if (WriteImmediatylyFlag && (ClickFlag || m_IsRegChange)) //当开启WriteImmediately,只要双击寄存器进行值修改，就会理解对改寄存器进行写入
    //{
    //    RegMapDiff2RegDataWrite(m_IsRegChange); //表明，点了寄存器，对点击的寄存器进行写操作
    //    ClickFlag = false;
    //    m_IsRegChange = false;
    //}


    RegMapDataUpdate_From_RegData();

    // 开始绘制窗口
    if (ImGui::Begin("Register Map", nullptr, window_flags))
    {
        /////////////////////////////////////////////上半区按钮////////////////////////////////////////
        // 添加两个圆角矩形按钮
        ImGui::BeginGroup();
        {
            // 设置按钮样式
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // 圆角半径
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15, 5));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 5));

            // 按钮颜色样式
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f)); // 正常状态颜色
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.26f, 0.59f, 0.98f, 1.00f)); // 悬停状态颜色
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.06f, 0.53f, 0.98f, 1.00f)); // 按下状态颜色
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.00f, 0.00f, 0.00f, 1.00f)); // 文字颜色

            // Write ALL Register 按钮
            if (ImGui::Button("Write ALL Register", ImVec2(160, 30))) {
                // 这里可以添加写入所有寄存器的逻辑
                //RegMAPDataUpdate_to_RegData();
                if (mIsPortOpen) {
                    WriteALLRegister();
                }
                //例如调用 REGMAP_DdataUpdate_to_RegData();
            }

            ImGui::SameLine();


            //----------------- Read ALL Register 按钮-----------------//
            if (mKeepReading) { //勾起keep reading的时候不允许点击Read ALL Register 按钮
                // 禁用状态样式
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.6f, 0.6f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 0.8f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.5f)); // 悬停颜色与正常状态相同
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 0.5f));  // 点击状态颜色
                //ImGui::PushFont(smallFont); // 使用预定义的小号字体
                ImGui::Button("Read ALL Register", ImVec2(160, 30)); // 普通按钮，但通过样式禁用
                //ImGui::PopFont();
                ImGui::PopStyleColor(4);
            }
            else {
                // 正常状态（原代码）
                if (ImGui::Button("Read ALL Register", ImVec2(160, 30))) {
                    //待添加检测串口打开的语句
                    //挨个发送寄存器地址进行读取
                    if (mIsPortOpen) {
                        ReadALLRegister(); //读取所有的寄存器值到底层寄存器变量中
                        RegMapDataUpdate_From_RegData();//把底层寄存器变量的值更新到用于显示的寄存器变量中。
                    }
                    //std::cout << "ReadALLRegister" << std::endl;
                    // 更新所有寄存器显示值
                }
            }
            //------------------------------------------------------------//

            ImGui::SameLine();

            // Keep Read 复选框 - 文字在右侧
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0)); // 减少间距
            ImGui::Checkbox("##KeepReading", &mKeepReading); // 隐藏标签
            ImGui::SameLine();
            ImGui::Text("Keep Read");
            ImGui::PopStyleVar();

            ImGui::SameLine();

            // Write Immediately 复选框 - 文字在右侧
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 0)); // 减少间距
            ImGui::Checkbox("##WriteImmediately", &mWriteImmediately); // 隐藏标签
            ImGui::SameLine();
            ImGui::Text("Write with Click");
            ImGui::PopStyleVar();


            // 恢复样式
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar(3);
        }
        ImGui::EndGroup();


        // 如果Write Immediately被选中，则在面对MAP值和底层值不同的时候，寄存器
        if (mWriteImmediately && mIsPortOpen)
        {
            WriteImmediatylyFlag = true;
        }
        else
        {
            WriteImmediatylyFlag = false;
        }
        ////////////////////////////////////////////////////////////////////////////////////////////


        // 样式设置
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 4));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        if (ImGui::BeginTable("Register Table", 11,
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_SizingStretchSame))
        {
            // 计算可用宽度（减去滚动条和右侧padding）
            float available_width = ImGui::GetContentRegionAvail().x
                - ImGui::GetStyle().ScrollbarSize
                - ImGui::GetStyle().WindowPadding.x;

            // 修改2：更精确的宽度计算（3 * 2 + 8 = 14单位）
            const float unit_width = available_width / 14.0f;
            const float wide_column_width = unit_width * 2;
            const float Register_Name_column_width = unit_width * 3;
            const float bit_column_width = unit_width;

            // 设置列宽
            ImGui::TableSetupColumn("Register Name", 0, Register_Name_column_width);
            ImGui::TableSetupColumn("Address", 0, wide_column_width);
            ImGui::TableSetupColumn("Value", 0, wide_column_width);

            // 设置后8个bit列（等宽）
            for (int i = 7; i >= 0; --i) {
                ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthStretch, bit_column_width);
            }

            // 应用列宽设置
            ImGui::TableSetupScrollFreeze(0, 1); // 冻结表头
            ImGui::TableHeadersRow();

            // ===== STATUS寄存器分区 =====
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "STATUS");
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(230, 230, 230, 255));

            // 渲染STATUS寄存器行
            RenderRegisterRow("DEVICE_ID", 0x00, &DEVICE_ID_MAP, false);
            RenderRegisterRow("FAULT_SUMMARY", 0x01, &FAULT_SUMMARY_MAP, false);
            RenderRegisterRow("STATUS1", 0x02, &STATUS1_MAP, false);
            RenderRegisterRow("STATUS2", 0x03, &STATUS2_MAP, false);

            // ===== CONTROL寄存器分区 =====
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "CONTROL");
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(230, 230, 230, 255));

            // 渲染CONTROL寄存器行
            RenderRegisterRow("COMMAND", 0x08, &COMMAND_MAP, true);
            RenderRegisterRow("SPI_IN", 0x09, &SPI_IN_MAP, true);
            RenderRegisterRow("CONFIG1", 0x0A, &CONFIG1_MAP, true);
            RenderRegisterRow("CONFIG2", 0x0B, &CONFIG2_MAP, true);
            RenderRegisterRow("CONFIG3", 0x0C, &CONFIG3_MAP, true);
            RenderRegisterRow("CONFIG4", 0x0D, &CONFIG4_MAP, true);

            ImGui::EndTable();
        }
        //Write immediately 情况下的更新
        if (WriteImmediatylyFlag && (ClickFlag || m_IsRegChange)) //当开启WriteImmediately,只要双击寄存器进行值修改，就会理解对改寄存器进行写入
        {

            if(ClickFlag) RegMAPDataUpdate_to_RegData();
            WriteCurrentRegister(ClickAddress); //写当前寄存器
            //RegMapDiff2RegDataWrite(m_IsRegChange); //表明，点了寄存器，对点击的寄存器进行写操作
            ClickFlag = false;
            m_IsRegChange = false;
            timerwriteimmediately.Reset(); 

        }

        // 如果Keep Read被选中，则自动读取寄存器
        if (mKeepReading && mIsPortOpen && timerwriteimmediately.ElapsedMillis() > 200.0f) { //要和立即写做一个间隔，不然会让MCU死机
            Timer timerregmap;
            if (timerregmap.ElapsedMillis() > 1000.0f);//间隔1s
            {
                ReadALLRegister();
                RegMapDataUpdate_From_RegData();
                timerregmap.Reset();
            }

        }

        RegMAPDataUpdate_to_RegData();
        // 恢复样式
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void RegMap::RenderRegisterRow(const char* name, uint8_t address, uint8_t* reg, bool editable)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::Text("%s", name);

    ImGui::TableNextColumn();
    ImGui::Text("0x%02X", address);

    ImGui::TableNextColumn();
    uint8_t value = *reg;
    ImGui::Text("0x%02X", value);

    // 从 bit7 到 bit0 显示
    for (int i = 7; i >= 0; --i)
    {
        ImGui::TableNextColumn();
        bool bit = (value >> i) & 1;

        // 为每个 bit 生成唯一的 ID
        ImGui::PushID(i);

        if (editable)
        {
            // 可编辑的 bit，使用 Selectable
            char label[32];
            snprintf(label, sizeof(label), "%s##%s_bit%d", bit ? "1" : "0", name, i);

            if (ImGui::Selectable(label, false, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_DontClosePopups))
            {
                if (ImGui::IsMouseDoubleClicked(0))
                {
                    ClickFlag = true; //表明进行了寄存器更改，如果立即写勾上，会对被更改的寄存器立即写
                    ClickAddress = address; //记录更改的寄存器地址

                    bit = !bit;
                    if (bit)
                        *reg |= (1 << i); // 设置 bit
                    else
                        *reg &= ~(1 << i); // 清除 bit

                    value = *reg; // 更新寄存器的值

                    ////////////打印所有寄存器值////////////
                    std::cout << "COMMAND_MAP: "
                        << "Binary: " << std::bitset<8>(COMMAND_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)COMMAND_MAP << std::dec
                        << std::endl;
                    std::cout << "SPI_IN_MAP: "
                        << "Binary: " << std::bitset<8>(SPI_IN_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)SPI_IN_MAP << std::dec
                        << std::endl;
                    std::cout << "CONFIG1_MAP: "
                        << "Binary: " << std::bitset<8>(CONFIG1_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG1_MAP << std::dec
                        << std::endl;
                    std::cout << "CONFIG2_MAP: "
                        << "Binary: " << std::bitset<8>(CONFIG2_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG2_MAP << std::dec
                        << std::endl;
                    std::cout << "CONFIG3_MAP: "
                        << "Binary: " << std::bitset<8>(CONFIG3_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG3_MAP << std::dec
                        << std::endl;
                    std::cout << "CONFIG4_MAP: "
                        << "Binary: " << std::bitset<8>(CONFIG4_MAP)
                        << " | Hex: 0x" << std::hex << std::uppercase << (int)CONFIG4_MAP << std::dec
                        << std::endl;
                    /////////////////////////////////

                }
            }
        }
        else
        {
            // 只读的 bit，使用黑色文字显示
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "%d", bit);
        }

        ImGui::PopID();
    }
}