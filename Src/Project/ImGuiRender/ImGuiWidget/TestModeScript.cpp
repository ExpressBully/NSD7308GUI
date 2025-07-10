#include "TestModeScript.h"
#include <imgui.h>
#include <fstream>
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")

TestModeScript::TestModeScript()
{
    // 初始化数组
    memset(Add, 0, sizeof(Add));
    memset(Data, 0, sizeof(Data));
    memset(Delay, 0, sizeof(Delay));
    for (uint8_t i = 0;i < ScriptLinesNumInput;i++) Delay[i] = DelayDefault_us; //将每个delay改为默认10
    memset(IsRead, 0, sizeof(IsRead));
}

TestModeScript::~TestModeScript()
{
    // 析构函数无需特殊操作
}

void TestModeScript::Render()
{
    if (mIsRender) { //TestMode Page的时候进行渲染
        return;
    }

    // 获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // 设置子窗口大小（主窗口下边50%）
    ImVec2 window_size(viewport->WorkSize.x, viewport->WorkSize.y * 0.5f);
    ImVec2 window_pos(viewport->WorkPos.x, viewport->WorkPos.y + viewport->WorkSize.y * 0.5f);

    // 创建子窗口
    ImGui::SetNextWindowPos(window_pos);
    ImGui::SetNextWindowSize(window_size);
    ImGui::Begin("Script Window", nullptr,
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    // 四列布局（占子窗口80%空间）
    float column_width = window_size.x * 0.8f / 4.0f;

    // 第一列 (0-7)
    ImGui::BeginChild("Column1", ImVec2(column_width, 0), true);
    RenderColumnHeader();
    for (int i = 0; i < 8; ++i) {
        RenderScriptRow(i);
    }
    ImGui::EndChild();

    ImGui::SameLine();
    //ImGui::Separator();
   // ImGui::SameLine();

    // 第二列 (8-15)
    ImGui::BeginChild("Column2", ImVec2(column_width, 0), true);
    RenderColumnHeader();
    for (int i = 8; i < 16; ++i) {
        RenderScriptRow(i);
    }
    ImGui::EndChild();

    ImGui::SameLine();
    //ImGui::Separator();
    //ImGui::SameLine();

    // 第三列 (16-23)
    ImGui::BeginChild("Column3", ImVec2(column_width, 0), true);
    RenderColumnHeader();
    for (int i = 16; i < 24; ++i) {
        RenderScriptRow(i);
    }
    ImGui::EndChild();

    ImGui::SameLine();
    //ImGui::Separator();
   // ImGui::SameLine();

    // 第四列 (24-31)
    ImGui::BeginChild("Column4", ImVec2(column_width, 0), true);
    RenderColumnHeader();
    for (int i = 24; i < 32; ++i) {
        RenderScriptRow(i);
    }
    ImGui::EndChild();

    ///////////////////////说明书/////////////////////////
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.02, window_size.y * 0.8));
    ImGui::Text("SPI comand('Add'<0xF0): 'Add': SPI1 / 'Data': SPI2 / 'Delay': Interval time to next SPI or GPIO action / 'RW': if used, 'Add'|0x40");
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.02, window_size.y * 0.9));
    ImGui::Text("GPIO command('Add'>0xF0):  'Add': 0xFF->PTB9, 0xFE->PTB8 / 'Data': data -> GPIO status / 'Delay': interval time");

    //////////////////////////////////////////////////////


    ////////////////////////////////开始绘制功能右边按钮//////////////////////////////////
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.2)); 

    //脚本数据包下发 按钮。点击后数据包下发至MCU存储
    if (ImGui::Button("Send Script Pack", ImVec2(150, 30))) {
        UpdateScriptNum(&ScriptNum);
        printf("Send Script\n");
        SendScriptAllData(Add, Data, Delay, IsRead, ScriptNum);
    }

    //下位机单次发送开始 按钮
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.4)); 

    if (ImGui::Button("Single Script Start", ImVec2(150, 30))) {
        printf("Script Start\n");
        ScriptStart();
    }

    //下位机循环发送时间间隔  输入框及文本标识
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.83, window_size.y * 0.6)); 
    ImGui::Text("CyclicDelay(ms)");
    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    //int delayValue = Delay[index];
    if (ImGui::InputInt(
        (std::string("##CyclicDelay")).c_str(),
        &CyclicScriptDelay_ms,
        0, 0,
        ImGuiInputTextFlags_CharsDecimal))
    {
        if (CyclicScriptDelay_ms < 1) CyclicScriptDelay_ms = 1;
        else if (CyclicScriptDelay_ms > 255) CyclicScriptDelay_ms = 255;
    }

    //下位机循环发送开始  按钮
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.5)); 

    if (ImGui::Button("Cyclic Script Start", ImVec2(150, 30))) {
        UpdateScriptNum(&ScriptNum);
        printf("Cyclic Script Start\n Delay=%d ms \n", CyclicScriptDelay_ms);
        CyclicScriptStart(CyclicScriptDelay_ms);
    }

    //下位机循环发送停止  按钮

    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.7));

    if (ImGui::Button("Cyclic Script Stop", ImVec2(150, 30))) {
        printf("Cyclic Script Stop\n");
        CyclicScriptStop();
    }

    //脚本内容清除，同时清除GUI和下位机。Script Clear

    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.85));

    if (ImGui::Button("Script Clear", ImVec2(150, 30))) {
        printf("Script Clear\n");
        ScriptGUIClear(); //清除GUI中内容
        ScriptMCUClear(); //清除MCU中脚本内容
    }

    //// 调试用打印数组 按钮
    //ImGui::SetCursorPos(ImVec2(window_size.x * 0.9, window_size.y * 0.9));

    //if (ImGui::Button("Debug Print", ImVec2(120, 30))) {
    //    UpdateScriptNum(&ScriptNum);
    //    printf("Valid Script Lines: %d\n", ScriptNum);
    //    for (int i = 0; i < 32; ++i) {
    //        printf("Row %02d: Add=0x%02X, Data=0x%02X, Delay=%d, R/W=%c\n",
    //            i + 1, Add[i], Data[i], Delay[i], IsRead[i] ? 'R' : 'W');
    //    }
    //    printf("/////////////////////////////////////\n");
    //    printf("/////////////////////////////////////\n");
    //}
    // 绘制 File 功能，包含 Save  Save As   Open ,生成和导入txt文件
    ImGui::SetCursorPos(ImVec2(window_size.x * 0.87, window_size.y * 0.01));
    if (ImGui::Button("File", ImVec2(150, 30))) {
        ImGui::OpenPopup("file_menu");
    }

    if (ImGui::BeginPopup("file_menu")) {
        //if (ImGui::MenuItem("Save")) {
        //    SaveScriptToTXT(false); // 不提示文件名
        //}
        if (ImGui::MenuItem("Save")) {
            SaveScriptToTXT(true); // 提示文件名
        }
        if (ImGui::MenuItem("Open...")) {
            LoadScriptFromTXT();
        }
        ImGui::EndPopup();
    }

    //////////////////////////////////////////////////////////////////////////////////////////

    ImGui::End();

}





    void TestModeScript::RenderColumnHeader()
    {
      ImGui::Text("     "); // 对齐行号位置
      ImGui::SameLine();

      ImGui::PushItemWidth(80); // 压缩Add列宽度
      ImGui::Text("Add");
      ImGui::SameLine();

      ImGui::PushItemWidth(40); // 压缩Data列宽度
      ImGui::Text("Data");
      ImGui::SameLine();

      ImGui::PushItemWidth(60);
      ImGui::Text("Delay/us");
      ImGui::SameLine();

       ImGui::Text("R/W");
    }

void TestModeScript::RenderScriptRow(int index)
{
    // 生成唯一ID后缀
    char idSuffix[ScriptLinesNumInput];
    snprintf(idSuffix, sizeof(idSuffix), "_%02d", index);

    // 行标签 (01, 02...)
    ImGui::Text("%02d", index + 1);
    ImGui::SameLine();

    // Add输入框 (16进制) - 宽度压缩到40
    ImGui::PushItemWidth(40);
    ImGui::InputScalar(
        (std::string("##Add") + idSuffix).c_str(),
        ImGuiDataType_U8, &Add[index],
        NULL, NULL, "%02X",
        ImGuiInputTextFlags_CharsHexadecimal
    );
    ImGui::SameLine();

    // Data输入框 (16进制) - 宽度压缩到40
    ImGui::PushItemWidth(40);

    ImGui::InputScalar(
        (std::string("##Data") + idSuffix).c_str(),
        ImGuiDataType_U8, &Data[index],
        NULL, NULL, "%02X",
        ImGuiInputTextFlags_CharsHexadecimal
    );
    ImGui::SameLine();

    // Delay输入框 (十进制)
    ImGui::PushItemWidth(60);
    int delayValue = Delay[index];
    if (ImGui::InputInt(
        (std::string("##Delay") + idSuffix).c_str(),
        &delayValue,
        0,0, 
        ImGuiInputTextFlags_CharsDecimal))
    {
        if (Add[index] > ScriptGPIOJudgeLine) {
            if (delayValue < DelayGPIOMinimum_us) delayValue = DelayGPIOMinimum_us;
            else if (delayValue > DelayMaximum_us) delayValue = DelayMaximum_us;
        }
        else
        {
            if (delayValue < DelayMinimum_us) delayValue = DelayMinimum_us;
            else if (delayValue > DelayMaximum_us) delayValue = DelayMaximum_us;
        }

        Delay[index] = static_cast<uint16_t>(delayValue);
    }
    ImGui::SameLine();

    // R/W状态显示 (带勾选框)
    ImGui::Checkbox((std::string("##RW") + idSuffix).c_str(), &IsRead[index]);
    ImGui::SameLine();
    ImGui::Text(IsRead[index] ? "R" : "W"); // 显示R/W文字状态
}

void TestModeScript::UpdateScriptNum(uint8_t* ScriptNum)
{
    *ScriptNum = 0;
    for (int i = 0; i < ScriptLinesNumInput; ++i) {
        if (Add[i] != 0 || Data[i] != 0) {
            *ScriptNum = static_cast<uint8_t>(i + 1); // 转换为1-based计数
        }
    }
}

void TestModeScript::SendScriptNum(uint8_t  ScriptNum)
{
    uint8_t Command = 0xC0; //写入
    char ScriptNumSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,       // 帧头
    Command,    // 命令
    0x00,       // 保留位
    ScriptNum,   // 数据
    0x00,        
    0x00,       //必要格式
    0x00,
    0x00,
    0x00,
    0x00
    };
    // 将帧数据复制到输出缓冲区
    memcpy(ScriptNumSendBuffer, frame, sizeof(frame));
    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptNumSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptNumSendBuffer);
}

void TestModeScript::SendScriptSingleData(uint8_t Add, uint8_t Data, uint16_t Delay, bool IsRead)
{
    uint8_t Command = 0xC1; //Script打包数据下发命令
    uint8_t Delay1 = Delay >> 8;
    uint8_t Delay2 = Delay;
    char ScriptDataSendBuffer[1024];
    uint8_t RWFlag;

    if (IsRead)
        RWFlag = 0x40;
    else 
        RWFlag = 0x00;

    uint8_t frame[10] = {
    0xAA,       // 帧头
    Command,    // 命令
    0x00,        // 保留位
    Add | RWFlag,       // 地址
    Data,
    Delay1,       //延时
    Delay2,       //延时
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(ScriptDataSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptDataSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptDataSendBuffer);

 }

void TestModeScript::SendScriptAllData(uint8_t Add[ScriptLinesNumInput], uint8_t Data[ScriptLinesNumInput], uint16_t Delay[ScriptLinesNumInput], bool IsRead[ScriptLinesNumInput], uint8_t ScriptNum)
{

    Timer timer;

    SendScriptNum(ScriptNum); //发送将要下发的数据数
    timer.Reset();
    while (timer.ElapsedMillis() < 5.0f);//间隔5ms
    for (uint8_t i = 0;i < ScriptNum; i++)
    {
        SendScriptSingleData(Add[i], Data[i], Delay[i], IsRead[i]);
        timer.Reset();
        while (timer.ElapsedMillis() < 5.0f);//间隔5ms
    }

    //////////////////////可以加个传输完成的回执信息/////////////////////

}

void TestModeScript::ScriptStart()
{
    uint8_t Command = 0xC2; //Script脚本数据单次传输 命令
    char ScriptStartSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,       // 帧头
    Command,    // 命令
    0x00,        // 保留位
    0x0F,       // 地址
    0x00,
    0x00,       
    0x00,      
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(ScriptStartSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptStartSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptStartSendBuffer);
}

void TestModeScript::CyclicScriptStart(uint8_t CyclicTime)
{
    uint8_t Command = 0xC3; //Script脚本数据单次传输 命令
    char ScriptStartSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    CyclicTime,  // 间隔时间
    0x00,
    0x00,       
    0x00,      
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(ScriptStartSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptStartSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptStartSendBuffer);
}

void TestModeScript::CyclicScriptStop()
{
    uint8_t Command = 0xC4; //Script脚本数据单次传输 命令
    char ScriptStartSendBuffer[1024];

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
    memcpy(ScriptStartSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptStartSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptStartSendBuffer);
}

void TestModeScript::ScriptGUIClear()
{
   // 清空现有GUI Script数据
    memset(Add, 0, sizeof(Add));
    memset(Data, 0, sizeof(Data));
    memset(Delay, 0, sizeof(Delay));
    for (uint8_t i = 0;i < ScriptLinesNumInput;i++) Delay[i] = DelayDefault_us; //将每个delay改为默认10
    memset(IsRead, 0, sizeof(IsRead));
}

void TestModeScript::ScriptMCUClear()
{
    uint8_t Command = 0xC5; //Script脚本数据单次传输 命令
    char ScriptStartSendBuffer[1024];

    uint8_t frame[10] = {
    0xAA,        // 帧头
    Command,     // 命令
    0x00,        // 保留位
    0x11,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00
    };

    // 将帧数据复制到输出缓冲区
    memcpy(ScriptStartSendBuffer, frame, sizeof(frame));

    // 清空剩余部分 (假设缓冲区大小至少为5字节)
    // 如果str是mSendBuffer(1024字节)，我们可以安全地清空剩余部分
    memset(ScriptStartSendBuffer + sizeof(frame), 0, 1024 - sizeof(frame));

    UART_DEC::GetInstance().SendData(ScriptStartSendBuffer);
}

// 把GUI中的数据保存为TXT文件
void TestModeScript::SaveScriptToTXT(bool promptFilename)
{
    UpdateScriptNum(&ScriptNum);
    if (ScriptNum == 0) {
        printf("No valid script data to save!\n");
        return;
    }

    // 默认文件名
    std::string filename = "script_config.txt";

    // 文件保存对话框
    if (promptFilename) {
        OPENFILENAMEA ofn;
        char szFile[260] = { 0 };
        strcpy(szFile, "script_config.txt");

        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = szFile;
        ofn.nMaxFile = sizeof(szFile);
        ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrDefExt = "txt";
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

        if (GetSaveFileNameA(&ofn) == TRUE) {
            filename = ofn.lpstrFile;
        }
        else {
            return; // 用户取消
        }
    }

    // 写入TXT文件
    std::ofstream file(filename);
    if (!file.is_open()) {
        printf("Failed to create TXT file!\n");
        return;
    }

    // 写入文件头标识
    file << "[NSD7308 Script Configuration]\n";
    file << "Version=1.0\n";
    file << "ScriptLines=" << static_cast<int>(ScriptNum) << "\n";
    file << "CycleDelay=" << CyclicScriptDelay_ms << "\n\n";

    // 写入数据表头
    file << "Index\tAddress\tData\tDelay(us)\tR/W\n";
    file << "----------------------------------------\n";

    // 写入数据行（制表符分隔）
    for (uint8_t i = 0; i < ScriptNum; ++i) {
        file << i + 1 << "\t"
            << "0x" << std::hex << std::uppercase << static_cast<int>(Add[i]) << "\t"
            << "0x" << std::hex << std::uppercase << static_cast<int>(Data[i]) << "\t"
            << std::dec << Delay[i] << "\t"
            << (IsRead[i] ? 'R' : 'W') << "\n";
    }

    file.close();
    printf("Script saved to %s\n", filename.c_str());
}

// 从Txt加载数据到GUI脚本中
void TestModeScript::LoadScriptFromTXT()
{
    OPENFILENAMEA ofn;
    char szFile[260] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn) == TRUE) {
        std::string filename = ofn.lpstrFile;
        std::ifstream file(filename);

        if (!file.is_open()) {
            printf("Failed to open TXT file!\n");
            return;
        }

        // 清空现有数据
        memset(Add, 0, sizeof(Add));
        memset(Data, 0, sizeof(Data));
        memset(Delay, 0, sizeof(Delay));
        memset(IsRead, 0, sizeof(IsRead));

        std::string line;
        uint8_t loadedLines = 0;
        bool headerPassed = false;

        while (std::getline(file, line) && loadedLines < ScriptLinesNumInput) {
            // 跳过空行和注释行
            if (line.empty() || line[0] == '#' || line.find("---") != std::string::npos) {
                continue;
            }

            // 解析配置头
            if (line.find("Version=") != std::string::npos) {
                // 可以添加版本检查逻辑
                continue;
            }
            else if (line.find("CycleDelay=") != std::string::npos) {
                try {
                    CyclicScriptDelay_ms = std::stoi(line.substr(11));
                }
                catch (...) {
                    printf("Invalid CycleDelay format\n");
                }
                continue;
            }
            else if (line.find("Index\tAddress") != std::string::npos) {
                headerPassed = true;
                continue;
            }

            // 解析数据行（制表符分隔）
            if (headerPassed) {
                std::istringstream iss(line);
                std::string token;
                int col = 0;

                try {
                    while (std::getline(iss, token, '\t')) {
                        if (token.empty()) continue;

                        switch (col) {
                        case 0: break; // 跳过Index列
                        case 1: // Address
                            if (token.substr(0, 2) == "0x") {
                                Add[loadedLines] = static_cast<uint8_t>(
                                    std::stoi(token.substr(2), nullptr, 16));
                            }
                            break;
                        case 2: // Data
                            if (token.substr(0, 2) == "0x") {
                                Data[loadedLines] = static_cast<uint8_t>(
                                    std::stoi(token.substr(2), nullptr, 16));
                            }
                            break;
                        case 3: // Delay
                            Delay[loadedLines] = static_cast<uint16_t>(std::stoi(token));
                            break;
                        case 4: // R/W
                            IsRead[loadedLines] = (token == "R");
                            break;
                        }
                        col++;
                    }

                    if (col >= 4) { // 确保至少解析了前4列
                        loadedLines++;
                    }
                }
                catch (...) {
                    printf("Error parsing line: %s\n", line.c_str());
                }
            }
        }

        file.close();
        ScriptNum = loadedLines;
        printf("Loaded %d script lines from %s\n", loadedLines, filename.c_str());
    }
}