#include <iostream>
#include "imgui/imgui.h"
#include "RegMap.h"
#include "../ImGuiWidget/UART_DEC.h"

RegMap::RegMap()
{
    // 初始化寄存器
    DEVICE_ID = 0x01;
    FAULT_SUMMARY = 0x00;
    STATUS1 = 0x00;
    STATUS2 = 0x00;
    COMMAND = 0x00;
    SPI_IN = 0x00;
    CONFIG1 = 0x00;
    CONFIG2 = 0x00;
    CONFIG3 = 0x00;
    CONFIG4 = 0x00;
}

RegMap::~RegMap()
{
}

void RegMap::Render()
{
    if (ImGui::Begin("Register Map"))
    {
        // 设置表格样式
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 4)); // 增加单元格内边距
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // 所有文字为黑色

        // 绘制表格
        if (ImGui::BeginTable("Register Table", 12, ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInnerV))
        {
            // 表头
            ImGui::TableSetupColumn("Register Name", ImGuiTableColumnFlags_WidthFixed, 120.0f);
            ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 80.0f);
            for (int i = 7; i >= 0; --i)
            {
                ImGui::TableSetupColumn(std::to_string(i).c_str(), ImGuiTableColumnFlags_WidthFixed, 40.0f);
            }
            ImGui::TableHeadersRow();

            // STATUS 分区
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "STATUS"); // STATUS 标题为黑色
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImVec4(0.9f, 0.9f, 0.9f, 1.0f))); // 设置分区背景色
            RenderRegisterRow("DEVICE_ID", 0x00, &DEVICE_ID, false);
            RenderRegisterRow("FAULT_SUMMARY", 0x01, &FAULT_SUMMARY, false);
            RenderRegisterRow("STATUS1", 0x02, &STATUS1, false);
            RenderRegisterRow("STATUS2", 0x03, &STATUS2, false);

            // CONTROL 分区
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "CONTROL"); // CONTROL 标题为黑色
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(ImVec4(0.9f, 0.9f, 0.9f, 1.0f))); // 设置分区背景色
            RenderRegisterRow("COMMAND", 0x08, &COMMAND, true);
            RenderRegisterRow("SPI_IN", 0x09, &SPI_IN, true);
            RenderRegisterRow("CONFIG1", 0x0A, &CONFIG1, true);
            RenderRegisterRow("CONFIG2", 0x0B, &CONFIG2, true);
            RenderRegisterRow("CONFIG3", 0x0C, &CONFIG3, true);
            RenderRegisterRow("CONFIG4", 0x0D, &CONFIG4, true);

            ImGui::EndTable();
        }

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
                    bit = !bit;
                    if (bit)
                        *reg |= (1 << i); // 设置 bit
                    else
                        *reg &= ~(1 << i); // 清除 bit

                    value = *reg; // 更新寄存器的值
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