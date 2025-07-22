#include "ExtStateControl.h"
#include <imgui.h>
#include <imgui_internal.h>
#define STB_IMAGE_IMPLEMENTATION // 必须定义一次
#include "stb_image.h"     
#include <glad/glad.h>  // 定义GLuint等OpenGL类型

Timer timer;

ExtStateControl::ExtStateControl()
    : m_currentMode(OperationMode::PH_EN_MODE) // 使用枚举值
    , m_startStopState(StartStopState::START)
    , PWM_IN1(50)
    , PWM_IN2(50)
    , m_arrowDirection(ArrowDirection::LEFT_ARROW)
{
    if (!LoadTexture("../../Resource/Picture/Funtion.png")) {
    //if (!LoadTexture(RESOURCE_DIR "/Picture/Funtion.png")) {
        printf("Warning: Failed to load circuit diagram texture\n");
    }
}

ExtStateControl::~ExtStateControl()
{
    // 释放资源
    if (m_circuitTexture) {
        GLuint texId = (GLuint)(intptr_t)m_circuitTexture;
        glDeleteTextures(1, &texId);
    }
}



void ExtStateControl::Render()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // 设置子窗口位置和大小 (横40~80%, 纵向50%~100%)
    ImVec2 window_pos(
        viewport->Pos.x + viewport->Size.x * 0.4f,
        viewport->Pos.y + viewport->Size.y * 0.5f
    );
    ImVec2 window_size(
        viewport->Size.x * 0.4f,
        viewport->Size.y * 0.5f
    );

    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    ImGui::Begin("External State Control", nullptr,
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // 左侧控件区域
    ImGui::BeginChild("##LeftControls", ImVec2(window_size.x * 0.5f, 0), true);
    {
        // 模式选择按钮 - 横向排列
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 15));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f); // 圆角半径

        // 计算按钮宽度
        float buttonWidth = (ImGui::GetContentRegionAvail().x - 10) / 3.0f;

        ImGui::BeginGroup();
        {
            // PWM Mode 按钮
            if (m_currentMode == OperationMode::PWM_MODE) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.7f, 1.0f, 1.0f)); // 青绿色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // 黑色文字
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 白色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));  // 淡灰色文字
            }

            if (ImGui::Button("PWM\nMode", ImVec2(buttonWidth, 50)))
            {
                if (m_arrowDirection == ArrowDirection::LEFT_ARROW)
                {
                    PWM_IN1 = 50;
                    PWM_IN2 = 0;
                }
                else
                {
                    PWM_IN1 = 0;
                    PWM_IN2 = 50;
                }
                StopPWM(); //停止PWM
                Config3_Ext = 0x02;
                CONFIG3 &= 0xFC;//先把S_MODE置00
                CONFIG3 |= Config3_Ext; //赋值给底层
                SetModeWrite(CONFIG3);  //写MODE寄存器
                m_currentMode = OperationMode::PWM_MODE;
                m_startStopState = StartStopState::START;

            }

            ImGui::PopStyleColor(2);

            ImGui::SameLine();

            // Independent Mode 按钮
            if (m_currentMode == OperationMode::INDEPENDENT_MODE) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.7f, 1.0f, 1.0f)); // 青绿色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // 黑色文字
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 白色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));  // 淡灰色文字
            }

            if (ImGui::Button("Independent\nMode", ImVec2(buttonWidth, 50)))
            {
                PWM_IN1 = 50;
                PWM_IN2 = 50;
                StopPWM(); //停止PWM
                Config3_Ext = 0x01;
                CONFIG3 &= 0xFC;//先把S_MODE置00
                CONFIG3 |= Config3_Ext; //赋值给底层
                SetModeWrite(CONFIG3);  //写MODE寄存器
                m_currentMode = OperationMode::INDEPENDENT_MODE;
                m_startStopState = StartStopState::START;
            }

            ImGui::PopStyleColor(2);

            ImGui::SameLine();

            // PH/EN Mode 按钮
            if (m_currentMode == OperationMode::PH_EN_MODE) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.7f, 1.0f, 1.0f)); // 青绿色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));  // 黑色文字
            }
            else {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 白色
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));  // 淡灰色文字
            }

            if (ImGui::Button("PH/EN\nMode", ImVec2(buttonWidth, 50)))
            {
                if (m_arrowDirection == ArrowDirection::LEFT_ARROW)
                {
                    PWM_IN1 = 50;
                    PWM_IN2 = 100;
                }
                else
                {
                    PWM_IN1 = 50;
                    PWM_IN2 = 0;
                }
                StopPWM(); //停止PWM
                Config3_Ext = 0x00;
                CONFIG3 &= 0xFC;//先把S_MODE置00
                CONFIG3 |= Config3_Ext; //赋值给底层
                SetModeWrite(CONFIG3);  //写MODE寄存器
                m_currentMode = OperationMode::PH_EN_MODE;
                m_startStopState = StartStopState::START;
            }

            ImGui::PopStyleColor(2);
        }
        ImGui::EndGroup();

        ImGui::PopStyleVar(2); // 恢复样式

        //--------------Start/Stop 双态按钮-------------------//
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f); // 圆角

        ImGui::SetCursorPos(ImVec2(window_size.x * 0.02, window_size.y * 0.2));

        if (m_startStopState == StartStopState::START) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.7f, 1.0f, 1.0f)); // 天蓝色
            if (ImGui::Button("START", ImVec2(-1, 40)))
            {
                Set_PWMFreq(SelectedFreq);
                timer.Reset();
                while (timer.ElapsedMillis() < 5.0f);//间隔5ms

                Set_IN1PWMDuty(PWM_IN1);

                timer.Reset();
                while (timer.ElapsedMillis() < 5.0f);//间隔5ms

                Set_IN2PWMDuty(PWM_IN2);
                timer.Reset();
                while (timer.ElapsedMillis() < 5.0f);//间隔5ms

                StartPWM(); //开始PWM
                m_startStopState = StartStopState::STOP;
                printf("m_startStopState = %d", m_startStopState);
            }

            // 绘制向右三角形
            ImVec2 p = ImGui::GetItemRectMin();
            p.x += 20;
            p.y += 12;
            ImGui::GetWindowDrawList()->AddTriangleFilled(
                p,
                ImVec2(p.x, p.y + 16),
                ImVec2(p.x + 16, p.y + 8),
                IM_COL32(255, 255, 255, 255)
            );
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f)); // 透明
            ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.5f, 0.7f, 1.0f, 1.0f)); // 淡蓝色边框
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

            if (ImGui::Button("STOP", ImVec2(-1, 40)))
            {
                StopPWM(); //开始PWM
                m_startStopState = StartStopState::START;
            }


            // 绘制正方形
            ImVec2 p = ImGui::GetItemRectMin();
            p.x += 20;
            p.y += 12;
            ImGui::GetWindowDrawList()->AddRectFilled(
                p,
                ImVec2(p.x + 16, p.y + 16),
                IM_COL32(128, 179, 255, 255)
            );

            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
        }

        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
        //----------------------------------------------------//


        //--------------左右箭头部分-------------------//
        ImGui::BeginGroup();
        {
            // 方向标题 (DIRECTION)
            ImVec2 DirectionPos = ImVec2(window_size.x * 0.1, window_size.y * 0.35);
            ImGui::SetCursorPos(DirectionPos);
            ImGui::TextColored(ImVec4(0, 0, 0, 1), "DIRECTION");

            // 箭头按钮容器
        if(!(m_currentMode == OperationMode::INDEPENDENT_MODE))
        {
            ImGui::BeginGroup();
            {
                // 计算居中位置
                float totalWidth = 200.0f; // 左右箭头+文本的总宽度
                ImGui::SetCursorPos(ImVec2(DirectionPos.x - window_size.x * 0.08, DirectionPos.y + window_size.y * 0.05));

                // 箭头按钮 (透明背景)
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1, 1, 1, 1)); // 悬停白色
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1, 1, 1, 1)); // 点击保持白色
                if (ImGui::Button("##left_btn", ImVec2(60, 40))) {
                    m_arrowDirection = ArrowDirection::LEFT_ARROW;
                    m_startStopState = StartStopState::START;
                    StopPWM(); //停止PWM
                    if (m_currentMode == OperationMode::PWM_MODE) //保证左右切换的时候IN1 IN2重新初始化
                    {
                        PWM_IN1 = 50;
                        PWM_IN2 = 0;
                    }
                    else if (m_currentMode == OperationMode::PH_EN_MODE)
                    {
                        PWM_IN2 = 100;
                    }
                }

                // 绘制左箭头 (淡蓝色)
                ImVec2 p = ImGui::GetItemRectMin();
                ImU32 arrowColor = (m_arrowDirection == ArrowDirection::LEFT_ARROW) ?
                    IM_COL32(0, 100, 255, 255) : // 选中状态淡蓝色
                    IM_COL32(200, 200, 200, 255);  // 未选中灰色

                ImGui::GetWindowDrawList()->AddTriangleFilled(
                    ImVec2(p.x + 25, p.y + 10),
                    ImVec2(p.x + 25, p.y + 30),
                    ImVec2(p.x + 5, p.y + 20),
                    arrowColor
                );

                //ImGui::SameLine();

                // 中间文本 (Forward)
                ImGui::SetCursorPos(ImVec2(DirectionPos.x + window_size.x * 0.01, DirectionPos.y + window_size.y * 0.08));
                if (m_arrowDirection == ArrowDirection::LEFT_ARROW)
                {
                    ImGui::TextColored(
                        ImVec4(0, 0, 0, 1), // 黑色
                        "FORWARD"
                    );
                }
                else
                {
                    ImGui::TextColored(
                        ImVec4(0, 0, 0, 1), // 黑色
                        "REVERSE"
                    );
                }


                //ImGui::SameLine();

                // 右箭头按钮 (透明背景)
                ImGui::SetCursorPos(ImVec2(DirectionPos.x + window_size.x * 0.17, DirectionPos.y + window_size.y * 0.05));

                if (ImGui::Button("##right_btn", ImVec2(40, 40))) {
                    m_arrowDirection = ArrowDirection::RIGHT_ARROW;
                    m_startStopState = StartStopState::START;
                    StopPWM(); //停止PWM
                    if (m_currentMode == OperationMode::PWM_MODE) //保证左右切换的时候IN1 IN2重新初始化
                    {
                        PWM_IN1 = 0;
                        PWM_IN2 = 50;
                    }
                    else if (m_currentMode == OperationMode::PH_EN_MODE)
                    {
                        PWM_IN2 = 0;
                    }
                }

                // 绘制右箭头 (淡蓝色)
                p = ImGui::GetItemRectMin();
                arrowColor = (m_arrowDirection == ArrowDirection::RIGHT_ARROW) ?
                    IM_COL32(0, 100, 200, 255) : // 选中深蓝色
                    IM_COL32(200, 200, 200, 255);  // 未选中灰色

                ImGui::GetWindowDrawList()->AddTriangleFilled(
                    ImVec2(p.x + 10, p.y + 10),
                    ImVec2(p.x + 10, p.y + 30),
                    ImVec2(p.x + 30, p.y + 20),
                    arrowColor
                );

                ImGui::PopStyleColor(3);
            }
            ImGui::EndGroup();
        }
        }
        ImGui::EndGroup();
        //----------------------------------------------------------------//
        
        //---------------判断处于什么MODE，对应进行PWM控制部分的渲染---------------//
        ImVec2 IN1IN2PosRatio = ImVec2(0.04, 0.52);
        ImVec2 IN1IN2Pos = ImVec2(window_size.x * IN1IN2PosRatio.x, window_size.y * IN1IN2PosRatio.y);
        ImGui::SetCursorPos(IN1IN2Pos);
        ImGui::Text("IN1&IN2 Duty Cycle(%%)");
        switch (m_currentMode)
        {
        case OperationMode::PWM_MODE :
            {
            if (m_arrowDirection == ArrowDirection::LEFT_ARROW)
            {
                // PWM1控制
                RenderPwmControl("IN1", &PWM_IN1, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y+0.07),SliderReleasedFlag_PWM1);
                ImGui::SetCursorPos(ImVec2(window_size.x * (IN1IN2PosRatio.x - 0.02), window_size.y * (IN1IN2PosRatio.y + 0.17)));
                ImGui::Text("PH/IN2 = Low");
                PWM_IN2 = 0;
            }
            else
            {
                // PWM2控制
                RenderPwmControl("IN2", &PWM_IN2, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y + 0.17), SliderReleasedFlag_PWM2);
                ImGui::SetCursorPos(ImVec2(window_size.x* (IN1IN2PosRatio.x - 0.02), window_size.y* (IN1IN2PosRatio.y + 0.07)));
                ImGui::Text("IN1 = Low");
                PWM_IN1 = 0;
            }
            }break;
        case OperationMode::INDEPENDENT_MODE:
            {
            // PWM控制
            RenderPwmControl("IN1", &PWM_IN1, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y + 0.07), SliderReleasedFlag_PWM1);

            RenderPwmControl("IN2", &PWM_IN2, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y + 0.17), SliderReleasedFlag_PWM2);
            }break;
        case OperationMode::PH_EN_MODE:
            {
            if (m_arrowDirection == ArrowDirection::LEFT_ARROW)
            {
                RenderPwmControl("IN1", &PWM_IN1, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y + 0.07), SliderReleasedFlag_PWM1);
                ImGui::SetCursorPos(ImVec2(window_size.x* (IN1IN2PosRatio.x - 0.02), window_size.y* (IN1IN2PosRatio.y + 0.17)));
                ImGui::Text("IN2 = High");
                PWM_IN2 = 100;
            }
            else
            {
                RenderPwmControl("IN1", &PWM_IN1, window_size, ImVec2(IN1IN2PosRatio.x - 0.02, IN1IN2PosRatio.y + 0.07), SliderReleasedFlag_PWM1);
                ImGui::SetCursorPos(ImVec2(window_size.x* (IN1IN2PosRatio.x - 0.02), window_size.y* (IN1IN2PosRatio.y + 0.17)));
                ImGui::Text("IN2 = Low");
                PWM_IN2 = 0;
            }
            }break;

        default:;
        }
        //经过判断后打入PWM
        if((SliderReleasedFlag_PWM2 || SliderReleasedFlag_PWM1) && (m_startStopState == StartStopState::STOP))
        {
            Set_IN1PWMDuty(PWM_IN1);
            timer.Reset();
            while (timer.ElapsedMillis() < 5.0f);//间隔5ms
            Set_IN2PWMDuty(PWM_IN2);

            SliderReleasedFlag_PWM1 = false;    //清除滑块释放标志位
            SliderReleasedFlag_PWM2 = false;    //清除滑块释放标志位
        }


        //----------------------------------------------------------------//

        if (DrawCustomDropdown(dropdownconfig,  window_size)) //执行频率下拉框绘制
        {
            printf("PWM Frequency = %u", SelectedFreq); //打印当前选择的PWM频率
            Set_PWMFreq(SelectedFreq); //点击拉框就触发频率设置
            timer.Reset();
            while (timer.ElapsedMillis() < 10.0f);//间隔5ms

            Set_IN1PWMDuty(PWM_IN1);

            timer.Reset();
            while (timer.ElapsedMillis() < 10.0f);//间隔5ms

            Set_IN2PWMDuty(PWM_IN2);

            timer.Reset();
            while (timer.ElapsedMillis() < 10.0f);//间隔5ms

            StartPWM();
        }
        
    }
    ImGui::EndChild();

    // 右侧图片区域
    ImGui::SameLine();
    ImGui::BeginChild("##RightImage", ImVec2(0, 0), true);
    {

            // 计算保持比例的显示尺寸
            float availWidth = ImGui::GetContentRegionAvail().x;
            float availHeight = ImGui::GetContentRegionAvail().y *0.8f; // 80%高度

             float aspectRatio = (float)m_imageWidth / (float)m_imageHeight;
            float displayWidth = availWidth;
            float displayHeight = displayWidth / aspectRatio;

            if (displayHeight > availHeight) {
                displayHeight = availHeight;
               displayWidth = displayHeight * aspectRatio;
            }

            // 居中显示
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - displayWidth) * 0.5f);

            if (m_circuitTexture) {
            // 绘制图片
            ImGui::Image(
                m_circuitTexture,
                ImVec2(displayWidth, displayHeight)
            );
        }
        else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Error] Circuit diagram not loaded!");
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

void ExtStateControl::RenderPwmControl(const char* label, int* value, 
    ImVec2 windowSize,
    ImVec2 Pos,
    bool& slider_released)
{
    // 设置固定控件尺寸
    const float LABEL_HEIGHT = 20.0f;
    const float SLIDER_WIDTH = 120.0f;
    const float SLIDER_HEIGHT = 20.0f;
    const float INPUT_WIDTH = 35.0f;
    const float INPUT_HEIGHT = 25.0f;
    const float GROUP_PADDING = 5.0f;
    const float ROUNDING = 15.0f; // 圆角半径
    

    ImVec2 pos(ImVec2(windowSize.x * Pos.x, windowSize.y * Pos.y));

    ImGui::SetCursorPos(pos);

    // 开始控件组
    ImGui::BeginGroup();
    {
        // 标签（居中显示）
        ImGui::Text("%s", label);

        ImGui::SetCursorPos(ImVec2(pos.x + windowSize.x*0.08, pos.y - windowSize.x * 0.02));
        // 滑条（固定宽度）
        // 自定义滑条样式
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, ROUNDING);
        ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, 20.0f); // 圆形滑块
        ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(200, 200, 200, 255)); // 滑条背景色
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(210, 210, 210, 255));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(220, 220, 220, 255));
        ImGui::PushStyleColor(ImGuiCol_SliderGrab, IM_COL32(0, 120, 215, 255)); // 滑块颜色
        ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, IM_COL32(0, 150, 255, 255));

        // 滑条控件
        ImGui::SetCursorPosY(pos.y);
        ImGui::PushItemWidth(SLIDER_WIDTH);

        if (ImGui::SliderInt(("##slider_" + std::string(label)).c_str(), value, 0, 100, "%d%%")) {
            *value = ImClamp(*value, 0, 100);
        }

        // 检测滑块是否被释放
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            slider_released = true; // 用户释放滑块时置1
        }

        // 恢复样式
        ImGui::PopStyleColor(5);
        ImGui::PopStyleVar(2);

        // 输入框（固定宽度，居中显示）
        ImGui::SetCursorPos(ImVec2(pos.x + windowSize.x * 0.35, pos.y));
        ImGui::SetNextItemWidth(INPUT_WIDTH);
        if (ImGui::InputInt(("##input_" + std::string(label)).c_str(), value, 0, 0,
            ImGuiInputTextFlags_CharsDecimal | ImGuiInputTextFlags_EnterReturnsTrue)) {
            *value = ImClamp(*value, 0, 100);
            slider_released = true; // 用户修改输入框时也置1
        }

    }
    ImGui::EndGroup();
}

bool ExtStateControl::DrawCustomDropdown(const DropdownConfig& config,ImVec2 window_size) {
    // 计算绝对位置
    ImVec2 absolute_pos(window_size.x * config.Pos.x,
        window_size.y * config.Pos.y);

    // 设置光标位置
    ImGui::SetCursorPos(absolute_pos);
    ImGui::Text("%s", config.label);
    ImGui::SameLine();

    // 初始化标志位
    if (config.valuechangedflag) *config.valuechangedflag = false;

    // 转换为ImGui需要的格式
    const char* preview = config.Options[*config.SelectedIndex].c_str();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 5.0f); // 外框圆角
    ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(0, 100, 200, 255)); // 选中项背景色
    //ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(0, 100, 200, 255));   // 下拉按钮背景色（可选）

    // 开始下拉框
    bool modified = false;
    if (ImGui::BeginCombo(("##"+ std::string(config.label)).c_str(), preview)) {
        for (int i = 0; i < config.Options.size(); ++i) {
            bool is_selected = (i == *config.SelectedIndex);
            if (ImGui::Selectable(config.Options[i].c_str(), is_selected)) {
                *config.SelectedIndex = i;
                *config.CurrentSelectedValue = config.values[i];
                modified = true;
            }
            if (is_selected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    // 更新标志位
    if (modified && config.valuechangedflag) {
        *config.valuechangedflag = true;
    }
    // 恢复默认样式
    ImGui::PopStyleColor();
    ImGui::PopStyleVar();

    return modified;
}
void ExtStateControl::StartPWM()
{



    uint8_t Command = 0xF0; //Script脚本数据单次传输 命令
    char StartPWMSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    0x0F,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(StartPWMSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(StartPWMSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(StartPWMSendBuffer);

}

void ExtStateControl::StopPWM()
{
    uint8_t Command = 0xF1; //Script脚本数据单次传输 命令
    char StopPWMSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    0xF0,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(StopPWMSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(StopPWMSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(StopPWMSendBuffer);

}

void ExtStateControl::Set_IN1PWMDuty(uint8_t IN1_Duty)
{
    uint8_t Command = 0xF4; //PTD14-FTM2.CH5
    char Set_IN1PWMDutySendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    IN1_Duty,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(Set_IN1PWMDutySendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(Set_IN1PWMDutySendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(Set_IN1PWMDutySendBuffer);
}

void ExtStateControl::Set_IN2PWMDuty(uint8_t IN2_Duty)
{
    uint8_t Command = 0xF3; //PTD13-FTM2.CH4
    char Set_IN2PWMDutySendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    IN2_Duty,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(Set_IN2PWMDutySendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(Set_IN2PWMDutySendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(Set_IN2PWMDutySendBuffer);
}

void ExtStateControl::Set_PWMFreq(uint16_t PWM_Freq)
{
    StopPWM();
    timer.Reset();
    while (timer.ElapsedMillis() < 50.0f);//间隔5ms

    uint8_t Command = 0xF2; //PTD13-FTM2.CH4
    uint8_t Freq_High = PWM_Freq >> 8;
    uint8_t Freq_Low  = PWM_Freq;

    char Set_PWMFreqSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    Freq_High,
    Freq_Low,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(Set_PWMFreqSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(Set_PWMFreqSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(Set_PWMFreqSendBuffer);

}

void ExtStateControl::SetModeWrite(uint8_t REGDATA)
{
    uint8_t Command = 0xA3; //PTD13-FTM2.CH4
    uint8_t addressbytes = 0x0C;
    uint8_t databytes = REGDATA;
    char SetModeWriteSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    addressbytes,
    databytes,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(SetModeWriteSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(SetModeWriteSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(SetModeWriteSendBuffer);
}

bool ExtStateControl::LoadTexture(const char* path) {
    // 释放旧纹理（如果存在）
    if (m_circuitTexture) {
        GLuint texId = (GLuint)(intptr_t)m_circuitTexture;
        glDeleteTextures(1, &texId);
        m_circuitTexture = (ImTextureID)0;
    }

    // 使用stb_image加载图片
    int channels;
    unsigned char* data = stbi_load(path, &m_imageWidth, &m_imageHeight, &channels, 4);
    if (!data) {
        printf("Failed to load image: %s\n", stbi_failure_reason());
        return false;
    }

    // 创建OpenGL纹理
    GLuint texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 上传像素数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_imageWidth, m_imageHeight,
        0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    // 转换为ImTextureID
    m_circuitTexture = (ImTextureID)(uintptr_t)texId;
    return true;
}