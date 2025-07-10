#include "FaultState.h"
#include <algorithm>
#include <imgui_internal.h>
#include "RegData.h"
#include "UART_DEC.h"
#include "RegMap.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"

bool mIsTestMode = false; //初始化测试模式标志位

FaultState::FaultState() {
    
    ImGuiIO& io = ImGui::GetIO();

    // 字体加载优化（关键修改）
    ImFontConfig config;
    config.SizePixels = 14.0f;  // 稍大的基础字号
    config.OversampleH = 3;     // 水平超采样
    config.OversampleV = 1;     // 垂直超采样
    smallFont = io.Fonts->AddFontDefault(&config);
}

FaultState::~FaultState() {}

void FaultState::RenderStatusBox(const char* label, int state) {
    ImGui::PushFont(smallFont);

    // 更鲜明的红绿配色，左红右绿
    ImVec4 color = state ? ImVec4(0.86f, 0.26f, 0.26f, 1.0f) : ImVec4(0.26f, 0.76f, 0.26f, 1.0f);

    // 获取单元格完整尺寸（减去1像素避免覆盖分割线）
    ImVec2 cellSize = ImVec2(
        ImGui::GetContentRegionAvail().x / 8.0f - 1.0f,
        ImGui::GetTextLineHeightWithSpacing() * 1.8f - 1.0f
    );

    // 绘制填满的圆角矩形（留1像素边距）
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        ImVec2(pos.x + 5, pos.y + 2),
        ImVec2(pos.x + cellSize.x*8.0f - 1, pos.y + cellSize.y - 2),
        ImGui::GetColorU32(color),
        6.0f // 增大圆角半径
    );

    // 自动调整文字大小
    ImVec2 textSize = ImGui::CalcTextSize(label);
    float scale =  max(1.2f, min(
        (cellSize.x * 0.85f) / textSize.x,
        (cellSize.y * 0.7f) / textSize.y
    ));

    // 精确居中定位
    ImVec2 textPos(
        pos.x - (cellSize.x - textSize.x * scale) * 0.2f,
        pos.y + (cellSize.y - textSize.y * scale) * 0.5f
    );

    // 文字渲染
    drawList->AddText(
        //smallFont,
        //smallFont->FontSize * scale,
        textPos,
        IM_COL32(255, 255, 255, 255),
        label
    );

    ImGui::Dummy(cellSize);
    ImGui::PopFont();
}

void FaultState::Render() {
    if (!mIsRender) { //TestMode Page的时候不渲染
        return;
    }

    // 设置窗口位置和大小（主窗口左侧40%，高度25%）
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 pos(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);
    ImVec2 size(viewport->WorkSize.x * 0.4f, viewport->WorkSize.y * 0.25f);

    // 窗口样式设置（白底无边框）
    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 纯白背景

    if (ImGui::Begin("Fault Table", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove ))
        
       // | ImGuiWindowFlags_NoTitleBar
    {

        //为各个状态赋值，判断是否报出错误
        status.FAULT = ((FAULT_SUMMARY & 0x20) != 0) ? 1 : 0;
        status.VMOV = ((FAULT_SUMMARY & 0x10) != 0) ? 1 : 0;
        status.VMUV = ((FAULT_SUMMARY & 0x08) != 0) ? 1 : 0;
        status.TSD = ((FAULT_SUMMARY & 0x02) != 0) ? 1 : 0;
        status.SPI_ERR = ((FAULT_SUMMARY & 0x80) != 0) ? 1 : 0;

        status.POR = ((FAULT_SUMMARY & 0x40) != 0) ? 1 : 0;
        status.ITRIP_CMP = ((STATUS1 & 0x20) != 0) ? 1 : 0;
        status.OLP_CMP = ((STATUS2 & 0x01) != 0) ? 1 : 0;
        status.DRVOFF_STAT = ((STATUS2 & 0x80) != 0) ? 1 : 0;

        status.OLA1 = ((STATUS1 & 0x80) != 0) ? 1 : 0;
        status.OLA2 = ((STATUS1 & 0x40) != 0) ? 1 : 0;
        status.OLA = ((status.OLA2 | status.OLA1) != 0) ? 1 : 0;

        status.OCP = ((FAULT_SUMMARY & 0x04) != 0) ? 1 : 0;
        status.OCP_H1 = ((STATUS1 & 0x08) != 0) ? 1 : 0;
        status.OCP_H2 = ((STATUS1 & 0x04) != 0) ? 1 : 0;
        status.OCP_L1 = ((STATUS1 & 0x02) != 0) ? 1 : 0;
        status.OCP_L2 = ((STATUS1 & 0x01) != 0) ? 1 : 0;


        // 获取表格绘制区域
        ImVec2 tableStart = ImGui::GetCursorScreenPos();
        ImDrawList* drawList = ImGui::GetWindowDrawList();

        // 表格内容（3行8列）
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
        if (ImGui::BeginTable("FaultTable", 8, ImGuiTableFlags_None))
        {
            // 隐藏所有原生边框
            ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_TableBorderLight, ImVec4(0, 0, 0, 0));

            // FAULT SUMMARY行
            ImGui::TableNextRow();
            const char* row1[] = { "SPI_ERR", " POR", "FAULT", " VMOV", " VMUV", " OCP", " TSD", " OLA" };
            int states1[] = { status.SPI_ERR, status.POR, status.FAULT, status.VMOV, status.VMUV, status.OCP, status.TSD, status.OLA };
            for (int col = 0; col < 8; col++) {
                ImGui::TableSetColumnIndex(col);
                RenderStatusBox(row1[col], states1[col]);
            }

            // STATUS1行
            ImGui::TableNextRow();
            const char* row2[] = { "OLA1", "OLA2", "ITRIP", "ACTIVE", "OCP_H1", "OCP_L1", "OCP_H2", "OCP_L2" };
            int states2[] = { status.OLA1, status.OLA2, status.ITRIP_CMP, 1, status.OCP_H1, status.OCP_L1, status.OCP_H2, status.OCP_L2 };
            for (int col = 0; col < 8; col++) {
                ImGui::TableSetColumnIndex(col);
                RenderStatusBox(row2[col], states2[col]);
            }

            // STATUS2行
            ImGui::TableNextRow();
            const char* row3[] = { "DRV_STAT", " N/A", " N/A", "ACTIVE", " N/A", " N/A", " N/A", " OLP" };
            int states3[] = { status.DRVOFF_STAT, 0, 0, 1, 0, 0, 0, status.OLP_CMP };
            for (int col = 0; col < 8; col++) {
                ImGui::TableSetColumnIndex(col);
                RenderStatusBox(row3[col], states3[col]);
            }

            ImGui::EndTable();
            ImGui::PopStyleColor(2);
        }
        ImGui::PopStyleVar();

        // 自定义绘制内部虚线（湖蓝色）
        ImVec2 tableEnd = ImGui::GetItemRectMax();
        const ImU32 lineColor = IM_COL32(69, 130, 181, 255); // 湖蓝色 (RGB: 69,130,181)

        // 虚线绘制函数（局部Lambda）
        auto DrawDashedLine = [&](const ImVec2& p1, const ImVec2& p2, float thickness) {
            const float dashLength = 5.0f;    // 虚线片段长度
            const float gapLength = 3.0f;    // 虚线间隔长度
            ImVec2 dir = ImVec2(p2.x - p1.x, p2.y - p1.y);
            float lineLength = sqrtf(dir.x * dir.x + dir.y * dir.y);
            dir = ImVec2(dir.x / lineLength, dir.y / lineLength); // 单位方向向量

            float currentLength = 0.0f;
            while (currentLength < lineLength) {
                float segmentLength = fminf(dashLength, lineLength - currentLength);
                ImVec2 segmentStart = ImVec2(
                    p1.x + dir.x * currentLength,
                    p1.y + dir.y * currentLength
                );
                ImVec2 segmentEnd = ImVec2(
                    segmentStart.x + dir.x * segmentLength,
                    segmentStart.y + dir.y * segmentLength
                );
                drawList->AddLine(segmentStart, segmentEnd, lineColor, thickness);
                currentLength += dashLength + gapLength;
            }
            };

        // 水平虚线（两条）
        float rowHeight = (tableEnd.y - tableStart.y) / 3.0f;
        for (int i = 1; i < 3; ++i) {
            float y = tableStart.y + rowHeight * i;
            DrawDashedLine(
                ImVec2(tableStart.x, y),
                ImVec2(tableEnd.x, y),
                1.2f // 线宽
            );
        }

        // 垂直虚线（七条）
        float colWidth = (tableEnd.x - tableStart.x) / 8.0f;
        for (int i = 1; i < 8; ++i) {
            float x = tableStart.x + colWidth * i;
            DrawDashedLine(
                ImVec2(x, tableStart.y),
                ImVec2(x, tableEnd.y),
                1.2f // 线宽
            );
        }
        // ========== Clear Fault按钮 ==========

        ImGui::SetCursorPos(ImVec2(
            ImGui::GetWindowWidth() - 130,  // 设置按钮相对子窗口的x位置
            ImGui::GetWindowHeight() - 35   // 设置按钮相对子窗口的y位置
        ));

        // 设置按钮样式
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f);    // 圆角半径
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 5)); // 内边距

        // 湖蓝色系按钮颜色 (RGB: 0.27, 0.51, 0.71)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.27f, 0.51f, 0.71f, 0.9f));  // 正常状态
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.60f, 0.80f, 1.0f));  // 悬停状态
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.20f, 0.45f, 0.65f, 1.0f)); // 按下状态
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 10.0f)); // 白色文字

        if (ImGui::Button("Clear Fault", ImVec2(120, 30))) {
            printf("Clear Fault\n");
            if (mIsPortOpen) {
                UART_DEC::GetInstance().WakeUp_CLR_FLT(); //进行清错
            }
        }
        //========================================//
                // 恢复样式
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);

    }

    ImGui::End();

    // 恢复样式
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}







//FaultState::FaultState() {}
//
//FaultState::~FaultState() {}
//
//void FaultState::RenderStatusBox(const char* label, int state, float width, float height) {
//    // 状态颜色 (红色或绿色)
//    ImVec4 color = state ? ImVec4(0.8f, 0.1f, 0.1f, 1.0f) : ImVec4(0.1f, 0.8f, 0.1f, 1.0f);
//
//    // 绘制圆角矩形
//    ImVec2 pos = ImGui::GetCursorScreenPos();
//    ImDrawList* drawList = ImGui::GetWindowDrawList();
//
//    // 背景
//    drawList->AddRectFilled(
//        pos,
//        ImVec2(pos.x + width, pos.y + height),
//        ImGui::GetColorU32(color),
//        5.0f // 圆角半径
//    );
//
//    // 文字居中
//    ImVec2 textSize = ImGui::CalcTextSize(label);
//    ImVec2 textPos(
//        pos.x + (width - textSize.x) * 0.5f,
//        pos.y + (height - textSize.y) * 0.5f
//    );
//    drawList->AddText(textPos, IM_COL32(255, 255, 255, 255), label);
//
//    // 占用空间（使用Dummy替代ItemSize）
//    ImGui::Dummy(ImVec2(width, height));
//}
//
//void FaultState::Render() {
//    // 设置窗口位置和大小（左下角40%宽度，50%高度）
//    ImGuiViewport* viewport = ImGui::GetMainViewport();
//    float width = viewport->WorkSize.x * 0.4f;
//    float height = viewport->WorkSize.y * 0.5f;
//    ImVec2 pos(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y - height);
//
//    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
//    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);
//
//    // 窗口样式
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
//    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
//
//    if (ImGui::Begin("Fault Indicators", nullptr,
//        ImGuiWindowFlags_NoCollapse |
//        ImGuiWindowFlags_NoResize |
//        ImGuiWindowFlags_NoMove))
//    {
//        // 第一行状态框 (FAULT, VMOV, VMUV, TSD, SPI_ERR)
//        ImGui::BeginGroup();
//        RenderStatusBox("FAULT", status.FAULT);
//        ImGui::SameLine();
//        RenderStatusBox("VMOV", status.VMOV);
//        ImGui::SameLine();
//        RenderStatusBox("VMUV", status.VMUV);
//        ImGui::SameLine();
//        RenderStatusBox("TSD", status.TSD);
//        ImGui::SameLine();
//        RenderStatusBox("SPI_ERR", status.SPI_ERR);
//        ImGui::EndGroup();
//
//        ImGui::Spacing();
//
//        // 第二行状态框 (POR, ITRIP_CMP, OLP_CMP, DRVOFF_STAT)
//        float stateWidth = width * 0.2f;
//        ImGui::BeginGroup();
//        RenderStatusBox("POR", status.POR, stateWidth);
//        ImGui::SameLine();
//        RenderStatusBox("ITRIP_CMP", status.ITRIP_CMP, stateWidth);
//        ImGui::SameLine();
//        RenderStatusBox("OLP_CMP", status.OLP_CMP, stateWidth);
//        ImGui::SameLine();
//        RenderStatusBox("DRVOFF_STAT", status.DRVOFF_STAT, stateWidth*1.2);
//        ImGui::EndGroup();
//
//        ImGui::Spacing();
//
//        // OLA系列状态框
//        float olaWidth = width * 0.25f;
//        float smallWidth = olaWidth * 0.5f;
//
//        ImGui::BeginGroup();
//        RenderStatusBox("OLA", status.OLA, olaWidth);
//
//        // OLA1和OLA2在OLA下方
//        ImGui::SetCursorPosX(ImGui::GetCursorPosX());
//        RenderStatusBox("OLA1", status.OLA1, smallWidth);
//        ImGui::SameLine();
//        RenderStatusBox("OLA2", status.OLA2, smallWidth);
//        ImGui::EndGroup();
//
//        ImGui::SameLine();
//
//        // OCP系列状态框（横向排列）
//        float ocpWidth = width * 0.6f;
//        float ocpItemWidth = ocpWidth / 4.4f;
//
//        ImGui::BeginGroup();
//        RenderStatusBox("OCP", status.OCP, ocpWidth);
//
//        // OCP_H1/H2和OCP_L1/L2在OCP下方横向排列
//        ImGui::SetCursorPosX(ImGui::GetCursorPosX());
//        RenderStatusBox("OCP_H1", status.OCP_H1, ocpItemWidth);
//        ImGui::SameLine();
//        RenderStatusBox("OCP_H2", status.OCP_H2, ocpItemWidth);
//        ImGui::SameLine();
//        RenderStatusBox("OCP_L1", status.OCP_L1, ocpItemWidth);
//        ImGui::SameLine();
//        RenderStatusBox("OCP_L2", status.OCP_L2, ocpItemWidth);
//        ImGui::EndGroup();
//    }
//    ImGui::End();
//    ImGui::PopStyleVar(2);
//}