#include <iostream>
#include "ExamplePage.h"
#include "imgui/imgui.h"
ExamplePage::ExamplePage()
{
}

ExamplePage::~ExamplePage()
{
}

void ExamplePage::Render()
{

    //ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x + 200, ImGui::GetIO().DisplaySize.y + 150), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_FirstUseEver);
    ////ImGui Example
        // Create a window to hold all the controls
    ImGui::Begin("Dear ImGui Controls");

    // Text
    ImGui::Text("Hello, world!");

    // Button
    if (ImGui::Button("Click Me")) {
        std::cout << "Button clicked!" << std::endl;
    }

    // Checkbox
    static bool checkbox_value = false;
    ImGui::Checkbox("Checkbox", &checkbox_value);

    // Radio buttons
    static int radio_value = 0;
    ImGui::RadioButton("Option 1", &radio_value, 0);
    ImGui::SameLine();
    ImGui::RadioButton("Option 2", &radio_value, 1);

    // Slider
    static float slider_value = 0.5f;
    ImGui::SliderFloat("Slider", &slider_value, 0.0f, 1.0f);

    // Input text
    static char text_input[128] = "Type here";
    ImGui::InputText("Input Text", text_input, IM_ARRAYSIZE(text_input));

    // Combo box
    static const char* items[] = { "Item 1", "Item 2", "Item 3" };
    static int combo_selection = 0;
    ImGui::Combo("Combo", &combo_selection, items, IM_ARRAYSIZE(items));

    // Color picker
    static ImVec4 color_picker_value = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImGui::ColorEdit4("Color Picker", (float*)&color_picker_value);

    // Tree nodes
    if (ImGui::TreeNode("Tree Node")) {
        ImGui::Text("This is a tree node.");
        if (ImGui::TreeNode("Child Node")) {
            ImGui::Text("This is a child node.");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    // Tables
    if (ImGui::BeginTable("Table", 3)) {
        // Headers
        ImGui::TableSetupColumn("Column 1");
        ImGui::TableSetupColumn("Column 2");
        ImGui::TableSetupColumn("Column 3");
        ImGui::TableHeadersRow();

        // Rows
        for (int row = 0; row < 5; row++) {
            ImGui::TableNextRow();
            for (int col = 0; col < 3; col++) {
                ImGui::TableSetColumnIndex(col);
                ImGui::Text("Row %d, Col %d", row, col);
            }
        }
        ImGui::EndTable();
    }

    // Progress bar
    static float progress_value = 0.0f;
    ImGui::ProgressBar(progress_value, ImVec2(0.0f, 0.0f));
    progress_value += 0.01f;
    if (progress_value > 1.0f) progress_value = 0.0f;

    // End the window
    ImGui::End();

// 假设你已经完成了ImGui的初始化和配置

//// 设置窗口在右下角显示
//    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 200, ImGui::GetIO().DisplaySize.y - 150), ImGuiCond_Always);
//    //ImGui::SetNextWindowSize(ImVec2(180, 100), ImGuiCond_Always);
//
//    // 创建窗口
//    ImGui::Begin("Use Please", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
//
//    // 创建按钮
//    if (ImGui::Button("Begin")) {
//        // 点击按钮后弹出小窗口
//        ImGui::OpenPopup("Begin to Use");
//    }
//
//    // 创建弹窗
//    if (ImGui::BeginPopupModal("Begin to Use", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
//        ImGui::Text("begin to use");
//        if (ImGui::Button("close")) {
//            ImGui::CloseCurrentPopup();
//        }
//        ImGui::EndPopup();
//    }
//
//    // 结束窗口
//    ImGui::End();
//
//    // 假设你已经完成了ImGui的渲染和清理


}
