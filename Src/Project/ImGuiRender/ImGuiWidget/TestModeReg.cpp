#include "TestModeReg.h"
#include <algorithm>



TestModeReg::TestModeReg()
{
    InitRegisterConfigs();
    InitRegisterDisplayConfigs();
}

TestModeReg::~TestModeReg()
{
}

void TestModeReg::Render()
{
    if (mIsRender) { //TestMode Page的时候进行渲染
        return;
    }

    // 窗口设置（调整为主窗口70%宽度,50%的高度，并靠在右上角）
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 window_size(viewport->WorkSize.x * 0.7f, viewport->WorkSize.y * 0.5f);
    ImVec2 window_pos(viewport->WorkPos.x + viewport->WorkSize.x - window_size.x, viewport->WorkPos.y);

    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);
    ImGui::Begin("TestMode Register", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // 获取子窗口可用区域
    //ImVec2 windowSize = ImGui::GetContentRegionAvail();

    // 开始子窗口
    //ImGui::BeginChild("TestMode Register", window_size, true);

    //////////////////////// 绘制所有下拉框控件////////////////////////
    for (const auto& config1 : regConfigs) {
        DrawLabelAndCombo(config1,window_size);
        if (config1.mIsChangeFlag && mIsWriteImediately) //立马写 判断条件：下拉框进行了修改，且立即写的勾选框勾上
        {
            TMRegWriteImediately(); //立马进行写入
        }
    }
    ////////////////////////////////////////////////////////////


    ////////////////// 绘制所有只读显示控件////////////////////////////
    for (const auto& config2 : regDisplayConfigs) {
        DrawLabelAndResult(config2, window_size);
    }
    ////////////////////////////////////////////////////////
    
    
     //////在左下角添加"Run TestMode"按钮////////
    ImGui::SetCursorPos(ImVec2(20, window_size.y - 50));
    if (ImGui::Button("Run TestMode", ImVec2(120, 25))) {
        // 按钮点击处理逻辑
        printf("Run TestMode button clicked\n");
        RunTestMode();
    }
    ///////////////////////////////////////////////

    //////在左下角右侧添加"Exit TestMode"按钮////////
    ImGui::SameLine();
    ImGui::SetCursorPos(ImVec2(160, window_size.y - 50));
    if (ImGui::Button("Exit TestMode", ImVec2(120, 25))) {
        // 按钮点击处理逻辑
        printf("Exit TestMode button clicked\n");
        ExitTestMode();
    }
    ///////////////////////////////////////////////

    ImGui::SameLine();


    ///// 定义ECC按钮 A状态配置/////////
    const ButtonState eccEnabledState{
        IM_COL32(255, 0, 0, 255),    // 红色
        IM_COL32(255, 50, 50, 255),
        "ECC(EN)",
        [this]() {this->DIS_ECC(); } //A状态向B状态切换时运行函数
    };
    ////// 定义ECC按钮 B状态配置/////////
    const ButtonState eccDisabledState{
        IM_COL32(0, 255, 0, 255),    // 绿色
        IM_COL32(50, 255, 50, 255),
        "ECC(DIS)",
        [this]() {this->EN_ECC(); }//B状态向A状态切换时运行函数
    };
    // ECC双状态按钮绘制
    DrawToggleButton(
        window_size,
        //ImVec2(320, ImGui::GetWindowSize().y - 50),
        ImVec2(0.5f, 0.75f),
        ImVec2(100, 25),
        m_IsECC_EN,
        eccEnabledState,
        eccDisabledState);
        
    //////////////////////////////////////////////////////////

    /////////////OCP EN/DIS切换按钮//////////////////////////
    ////// 定义OCP按钮 A状态配置/////////
    const ButtonState OCPEnabledState{
        IM_COL32(0, 255, 0, 255),    // 绿色
        IM_COL32(50, 255, 50, 255),
        "OCP(EN)",
        [this]() {this->DIS_OCP(&m_IsTSD_EN); } //A状态向B状态切换时运行函数
    };
    ////// 定义OCP按钮 B状态配置/////////
    const ButtonState OCPDisabledState{
        IM_COL32(255, 0, 0, 255),    // 红色
        IM_COL32(255, 50, 50, 255),  //鼠标悬停时颜色
        "OCP(DIS)",
        [this]() {this->EN_OCP(&m_IsTSD_EN); }//B状态向A状态切换时运行函数
    };
    // OCP双状态按钮绘制
    DrawToggleButton(
        window_size,
        //ImVec2(320, ImGui::GetWindowSize().y - 50),
        ImVec2(0.65f, 0.75f),
        ImVec2(100, 25),
        m_IsOCP_EN,
        OCPEnabledState,
        OCPDisabledState);

    //////////////////////////////////////////////////////////

    /////////////TSD EN/DIS切换按钮////////////////////////////////////
    ////// 定义TSD按钮 A状态配置/////////
    const ButtonState TSDEnabledState{
        IM_COL32(0, 255, 0, 255),    // 绿色
        IM_COL32(120, 255, 120, 255),
        "TSD(EN)",
        [this]() {this->DIS_OCP(&m_IsOCP_EN); } //A状态向B状态切换时运行函数
    };
    ////// 定义TSD按钮 B状态配置/////////
    const ButtonState TSDDisabledState{
        IM_COL32(255, 0, 0, 255),    // 红色
        IM_COL32(255, 120, 50, 255),  //鼠标悬停时颜色
        "TSD(DIS)",
        [this]() {this->EN_OCP(&m_IsOCP_EN); }//B状态向A状态切换时运行函数
    };
    // OCP双状态按钮绘制
    DrawToggleButton(
        window_size,
        //ImVec2(320, ImGui::GetWindowSize().y - 50),
        ImVec2(0.8f, 0.75f),
        ImVec2(100, 25),
        m_IsTSD_EN,
        TSDEnabledState,
        TSDDisabledState);

    //////////////////////////////////////////////////////////

    ///////////////添加立即写勾选框////////////
    DrawLabelWithCheckbox(
        window_size,
        "Write Immidiately",
        ImVec2(0.04f, 0.01f), //文本位置
        ImVec2(0.001f, 0.01f),   //勾选框位置
        &mIsWriteImediately,
        ImVec2(12,12)
        );

    // 在右下角添加"Read All"和"Write All"按钮
    ImGui::SetCursorPos(ImVec2(window_size.x - 360, window_size.y - 50));
    if (ImGui::Button("Read All TestReg", ImVec2(160, 25))) {
        // 按钮点击处理逻辑
        printf("Read All Registers button clicked\n");

    }

    ImGui::SameLine();

    if (ImGui::Button("Write All TestReg", ImVec2(160, 25))) {
        // 按钮点击处理逻辑
        printf("Write All Registers button clicked\n");
        WriteALLTMReg();
    }
    ////////////////////////////////////////////////////////////


    /*
        
    */
     ///////////绘制 ECC标签名字和16进制输入框////////////////
    DrawLabelAndInputControl(
        "ECC<5:0> = 0x",
        window_size,
        ImVec2(0.05f, 0.75f),    // 标签位置
        ImVec2(0.183f, 0.7455f), // 输入框位置
        80.0f,                   // 输入框宽度
        ECCValue,
        g_eccInputActive,
        6);
    ///////////////////////////////////////////////

        //////在ECC输入框右边添加"WriteECC"按钮////////
    ImGui::SetCursorPos(ImVec2(window_size.x*0.3f, window_size.y* 0.7455f));
    if (ImGui::Button("WriteECC", ImVec2(120, 25))) {
        // 按钮点击处理逻辑
        printf("Write ECC\n");
        Write_ECC();
    }
    ///////////////////////////////////////////////

 ///////////绘制 TM_REGA标签名字和16进制输入框////////////////
    DrawLabelAndInputControl(
        "TM_REGA<7:0> = 0x",
        window_size,
        ImVec2(0.05f, 0.65f),    // 标签位置
        ImVec2(0.2f, 0.6455f), // 输入框位置
        80.0f,                   // 输入框宽度
        TMREGAValue,
        g_TMREGAInputActive,
        8);
    ///////////////////////////////////////////////

    //////在TM_REGA输入框右边添加"Write TMREGA"按钮////////
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.3f, window_size.y * 0.6455f));
    if (ImGui::Button("Write TMREGA", ImVec2(120, 25))) {
        // 按钮点击处理逻辑
        printf("Write TMREGA\n");
        Write_TM_REGA();
    }
    ///////////////////////////////////////////////

   ///////////绘制 TM_REGB标签名字和16进制输入框////////////////
    DrawLabelAndInputControl(
        "TM_REGB<7:0> = 0x",
        window_size,
        ImVec2(0.5f, 0.65f),    // 标签位置
        ImVec2(0.65f, 0.6455f), // 输入框位置
        80.0f,                   // 输入框宽度
        TMREGBValue,
        g_TMREGBInputActive,
        8);
    ///////////////////////////////////////////////

    //////在TM_REGB输入框右边添加"Write TMREGB"按钮////////
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.76f, window_size.y * 0.6455f));
    if (ImGui::Button("Write TMREGB", ImVec2(120, 25))) {
        // 按钮点击处理逻辑
        printf("Write TMREGB\n");
        Write_TM_REGB();
    }
    ///////////////////////////////////////////////



    // 结束子窗口
    //ImGui::EndChild();
    ImGui::End();
}

