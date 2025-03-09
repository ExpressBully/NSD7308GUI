#include <iostream>
#include "TestClass.h"
#include "imgui/imgui.h"

TestClass::TestClass()
{
}

TestClass::~TestClass()
{
}

void TestClass::Render()
{
    // 设置窗口在右下角显示
    //ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x + 100, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_FirstUseEver);
    //// 创建窗口
    //ImGui::Begin("AAAA", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    //ImGui::Button("Begin");

    //// 结束窗口
    //ImGui::End();


// 设置窗口在右下角显示
    //ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_FirstUseEver);

    //// 创建窗口
    //ImGui::Begin("AAAA", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    //// 创建按钮
    //if (ImGui::Button("Begin")) {
    //    // 点击按钮后弹出小窗口
    //    ImGui::OpenPopup("Begin to Use");
    //}

    //// 创建弹窗
    //if (ImGui::BeginPopupModal("Begin to Use", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
    //    ImGui::Text("begin to use");
    //    if (ImGui::Button("close")) {
    //        ImGui::CloseCurrentPopup();
    //    }
    //    ImGui::EndPopup();
    //}

    //// 结束窗口
    //ImGui::End();
    
    ////////////////////////////////////////////////
// 设置窗口在右下角显示
// 获取主窗口的左上角坐标
    ImVec2 main_window_pos = ImGui::GetMainViewport()->Pos;

    // 设置窗口 "BBB" 的位置，相对于主窗口的左上角
    ImGui::SetNextWindowPos(ImVec2(main_window_pos.x + 10, main_window_pos.y + 10), ImGuiCond_Always); // 偏移 (10, 10)
    //ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver); // 设置初始大小

    // 创建窗口
    ImGui::Begin("AAA", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

         // 创建按钮
        if (ImGui::Button("Begin")) {
            // 点击按钮后弹出小窗口
            ImGui::OpenPopup("Begin to Use");
        }

        // 获取主窗口的中心位置
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();

        // 设置弹窗的位置在主窗口中心
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f)); // 以弹窗的中心对齐主窗

        // 创建弹窗
        if (ImGui::BeginPopupModal("Begin to Use", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("begin to use");
            if (ImGui::Button("close")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


    //// 结束窗口
    ImGui::End();

}


