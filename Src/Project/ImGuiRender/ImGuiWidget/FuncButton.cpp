#include "FuncButton.h"
#include <algorithm>
#include <cstring>
#include <iostream> // 添加iostream头文件用于输出
#include "FaultState.h"

FuncButton::FuncButton() {}

FuncButton::~FuncButton() {}

void FuncButton::RenderToggle(const char* label, bool* state,
    const char* activeText, const char* inactiveText)
{
    // 更新动画位置
    float& animPos = (strcmp(label, "Wake") == 0) ? wakeAnimPos : enableDriverAnimPos;
    float targetPos = *state ? 1.0f : 0.0f;
    animPos = animPos + (targetPos - animPos) * animSpeed;

    ImGui::PushID(label);

    // 左侧标签（黑色文字）
    ImGui::AlignTextToFramePadding();
    ImGui::Text("%s", label);

    // 确保所有按钮起始位置对齐
    float switchStartX = ImGui::GetCursorPosX() + 120.0f;
    ImGui::SameLine(switchStartX);

    // 开关参数
    const float width = 50.0f;
    const float height = 20.0f;
    const float radius = height * 0.6f;
    const float ballSize = height - 6.0f;

    // 绘制开关背景
    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景轨道（浅灰色）
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + width, pos.y + height),
        IM_COL32(220, 220, 220, 255),
        radius
    );

    // 状态颜色填充（半透明）
    drawList->AddRectFilled(
        pos,
        ImVec2(pos.x + width, pos.y + height),
        *state ? IM_COL32(76, 175, 80, 150) : IM_COL32(244, 67, 54, 150),
        radius
    );

    // 绘制滑块圆球（白色）
    float ballPos = pos.x + 3 + animPos * (width - ballSize - 6);
    drawList->AddCircleFilled(
        ImVec2(ballPos + ballSize / 2, pos.y + height / 2),
        ballSize / 2,
        IM_COL32(255, 255, 255, 255)
    );

    // 可点击区域
    ImGui::InvisibleButton("##toggle", ImVec2(width, height));
    if (ImGui::IsItemClicked()) {
        *state = !(*state);
        if (strcmp(label, "Wake") == 0 && *state == 1) {
            WakeUp_nSLEEPH();
            //中间需要delay
            UART_DEC::GetInstance().WakeUp_CLR_FLT();//写CLR_FLT进入STANDBY模式

            // 输出当前状态值
            //std::cout << label << " state changed to: " << (*state ? "ON" : "OFF") << std::endl;
        }

        if (strcmp(label, "Enable Driver") == 0 && *state == 1) DRVOFF_HIGH();
        else if (strcmp(label, "Enable Driver") == 0 && *state == 0) DRVOFF_LOW();

        if (strcmp(label, "Wake") == 0 && onWakeChanged) {
            onWakeChanged(*state);
        }
        else if (strcmp(label, "Enable Driver") == 0 && onEnableDriverChanged) {
            onEnableDriverChanged(*state);
        }
    }

    // 右侧状态文本（严格匹配图片样式）
    ImGui::SameLine();
    ImGui::TextColored(
        *state ? ImVec4(0.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.8f, 0.0f, 0.0f, 1.0f),
        "%s", *state ? activeText : inactiveText
    );

    ImGui::PopID();
}

void FuncButton::Render()
{
    if (!mIsRender) {
        return;
    }
    // 设置窗口位置和大小（右下角20%*20%）
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    float width = viewport->WorkSize.x * 0.2f;
    float height = viewport->WorkSize.y * 0.2f;
    ImVec2 pos(
        viewport->WorkPos.x + viewport->WorkSize.x - width,
        viewport->WorkPos.y + viewport->WorkSize.y - height
    );

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    // 直角窗口样式
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15, 15));

    if (ImGui::Begin("DRIVER CONFIGURATION", nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove))
    {
        // 垂直居中布局
        float startY = (height - ImGui::GetTextLineHeight() * 2 - ImGui::GetStyle().ItemSpacing.y) * 0.5f;
        ImGui::SetCursorPosY(startY);

        // Wake开关（初始状态为Disabled）
        RenderToggle("Wake", &wakeState, "Awake", "Disabled");

        // 间距
        ImGui::Spacing();

        // Enable Driver开关
        RenderToggle("Enable Driver", &enableDriverState, "Enabled", "Disabled");
    }
    ImGui::End();
    ImGui::PopStyleVar(2);
}