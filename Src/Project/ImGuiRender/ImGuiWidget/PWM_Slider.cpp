#include <iostream>
#include "PWM_Slider.h"
#include "imgui/imgui.h"


/************************************************************************
This code creates a slider window positioned at the top - right corner of the main window.
The slider ranges from 0% to 100%, with a red handle and a white track.
The slider's name is displayed above it, and its current value is shown to the right.
The window is non-resizable, non-movable, and has no title bar or scrollbar.
*********************************************************************/


PWM_Slider::PWM_Slider()
{
}

PWM_Slider::~PWM_Slider()
{
}

void PWM_Slider::Render()
{
    static float slider_value = 50.0f;
    ImVec2 main_window_pos = ImGui::GetMainViewport()->Pos;

    // 设置窗口 "BBB" 的位置，相对于主窗口的左上角
    ImGui::SetNextWindowPos(ImVec2(main_window_pos.x + 1000, main_window_pos.y + 10), ImGuiCond_Always); // 偏移 (10, 10)
    ImGui::SetNextWindowSize(ImVec2(400, 100), ImGuiCond_FirstUseEver); // 设置初始大小

    // 开始一个窗口
    ImGui::Begin("##RedSliderWindow", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    // 显示滑块的名字在上方
    ImGui::Text("%s", "OUT1 PWM Duty");

    // 设置滑块的样式
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 滑槽背景颜色（白色）
    ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 滑块颜色（红色）
    ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(1.0f, 0.0f, 0.0f, 1.0f)); // 滑块激活颜色（红色）

    // 创建滑块
    ImGui::SliderFloat("##Slider", &slider_value, 0.0f, 100.0f, "");

    // 恢复默认样式
    ImGui::PopStyleColor(3);

    // 显示滑块当前的值在右侧
    ImGui::SameLine();
    ImGui::Text("%.1f%%", slider_value);

    // 结束窗口
    ImGui::End();

}