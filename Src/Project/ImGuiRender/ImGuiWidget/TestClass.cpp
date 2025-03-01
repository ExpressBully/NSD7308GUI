#include "TestClass.h"
#include <iostream>
#include "imgui/imgui.h"

TestClass::TestClass()
{
}

TestClass::~TestClass()
{
}

void TestClass::Render()
{
    // 创建窗口
    ImGui::Begin("Please Use", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // 结束窗口
    ImGui::End();

}


