#include <iostream>
#include "imgui/imgui.h"
#include "RegMap.h"
#include "../ImGuiWidget/UART_DEC.h"

RegMap::RegMap()
{
    // 初始化寄存器
    DEVICE_ID_MAP = 0x01;
    FAULT_SUMMARY_MAP = 0x00;
    STATUS1_MAP = 0x00;
    STATUS2_MAP = 0x00;
    COMMAND_MAP = 0x00;
    SPI_IN_MAP = 0x00;
    CONFIG1_MAP = 0x00;
    CONFIG2_MAP = 0x00;
    CONFIG3_MAP = 0x00;
    CONFIG4_MAP = 0x00;
}

RegMap::~RegMap()
{
}

void RegMap::Render()
{
    // 获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 main_window_size = viewport->WorkSize;
    ImVec2 main_window_pos = viewport->WorkPos;

    // UART窗口参数（与UART_DEC::Render()保持一致）
    const float uart_width_ratio = 0.3f;  // UART窗口宽度占30%
    const float uart_height_ratio = 0.5f; // UART窗口高度占50%
    const float RegMap_width_ratio = 0.5f;

    const float uart_width = main_window_size.x * uart_width_ratio;
    const float spacing = 10.0f;          // 两窗口间距

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

    // 开始绘制窗口
    if (ImGui::Begin("Register Map", nullptr, window_flags))
    {
        // 顶部留出10像素空白
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        // 样式设置
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 4));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));

        // 修改1：将表格列数从12改为11（3+8）
        if (ImGui::BeginTable("Register Table", 11,  // 改为11列
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
            const float bit_column_width = unit_width;

            // 设置列宽
            ImGui::TableSetupColumn("Register Name", 0, wide_column_width);
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