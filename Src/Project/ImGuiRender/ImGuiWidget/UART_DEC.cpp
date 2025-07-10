#include <iostream>
#include "UART_DEC.h"
#include "imgui/imgui.h"
#include "../../Port/PortManager.h"
#include "../../Port/PortData.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cstdint>
#include <stdexcept>

bool mIsPortOpen = false;
bool TestModePageFlag = true;
std::string AllDataDisplayBuffer; //用于显示的全局变量

UART_DEC::UART_DEC()
{
    memset(UARTSendBuffer, 0, sizeof(UARTSendBuffer));  // 初始化发送缓冲区
    RefreshPortList();  // 初始化时刷新串口列表
}
UART_DEC::~UART_DEC()
{
    if (mIsPortOpen)
    {
        PortManager::GetInstance().SetCurrentPort("");  // 关闭串口
    }
}

void UART_DEC::Render()
{
    // 获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 main_window_size = viewport->WorkSize;
    ImVec2 main_window_pos = viewport->Pos;

    // 设置窗口尺寸（父窗口的30%宽度，50%高度）
    const float width_ratio = 0.3f;
    const float height_ratio = 0.5f;
    ImVec2 window_size(
        main_window_size.x * width_ratio,
        main_window_size.y * height_ratio
    );

    // 固定在左上角（无额外偏移）
    ImVec2 window_pos(
        main_window_pos.x,
        main_window_pos.y
    );

    // 设置窗口属性
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    // 窗口标志：无折叠箭头+禁止手动调整+无标题栏
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar;

    // 开始绘制窗口
    if (ImGui::Begin("UART", nullptr, window_flags))
    {
        PortData portdata;

        // --- Select Port 部分 ---
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::BeginChild("SelectPortLabel", ImVec2(100, ImGui::GetTextLineHeight()), false, ImGuiWindowFlags_NoScrollbar);
        ImGui::Text("Select Port");
        ImGui::EndChild();
        ImGui::PopStyleColor(2);

        ImGui::SameLine();
        if (ImGui::BeginCombo("##SelectPortCombo", mSelectedPort.empty() ? "None" : mSelectedPort.c_str()))
        {
            for (const auto& port : mPortList) {
                if (ImGui::Selectable(port.c_str(), port == mSelectedPort)) {
                    mSelectedPort = port;
                }
            }
            ImGui::EndCombo();
        }

        // --- 按钮行 ---
        if (mIsPortOpen) {
            if (ImGui::Button("Close Port")) {
                ClosePort();
            }
        }
        else {
            if (ImGui::Button("Open Port")) {
                OpenPort();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh Port List")) {
            RefreshPortList();
        }

        // --- 发送数据输入 ---
        ImGui::Text("Send Data:");
        ImGui::Columns(2, "SendDataColumns", false);

        // 地址输入
        ImGui::Text("Address");
        static char addressBuffer[256] = { 0 };
        ImGui::InputText("##Address", addressBuffer, sizeof(addressBuffer), ImGuiInputTextFlags_CharsHexadecimal);

        ImGui::NextColumn();

        // 数据输入
        ImGui::Text("Data");
        static char dataBuffer[256] = { 0 };
        ImGui::InputText("##Data", dataBuffer, sizeof(dataBuffer), ImGuiInputTextFlags_CharsHexadecimal);

        ImGui::Columns(1);

        // --- 写/读取按钮 ---
        isSend = ImGui::Button("Write");
        ImGui::SameLine();
        isRead = ImGui::Button("Read");


        if (isSend || isRead) {
            if (mIsPortOpen) {
                uint8_t Command = isSend ? 0xA3 : 0xA4;
                uint8_t addressBytes = ConvertHexStringToUint8(addressBuffer);
                uint8_t dataBytes = ConvertHexStringToUint8(dataBuffer);
                portdata.DataSendAnalysis(UARTSendBuffer, Command, addressBytes, dataBytes);
                // --- 发送数据 ---
                SendData(UARTSendBuffer);
                //std::vector<uint8_t> combinedBytes = { addressBytes, dataBytes };
               // mSendHistory.push_back(BytesToHexString(combinedBytes));

                // 如果是读取操作，设置标志位
                if (isRead) {
                    mIsReading = true;  // 设置读取标志位
                }
            }
            else
            {
                AllDataDisplayBuffer += "Please Open Port!!!\n"; //要求打开串口
            }
        }

        // 只有读取标志位为1时才显示接收数据
        //if (mIsReading) {

            ReceiveData(UARTReceiveBuffer);

            uint8_t addRec, dataRec;
            bool flag = portdata.DataRecAnalysis(UARTReceiveBuffer, addRec, dataRec);
            mIsReading = false; //清除读取标志位

            UARTReceiveBuffer.clear(); //清空接收到的数据(但并不会清空显示用的mReceiveBufferDis)
        //}

        // --- 数据历史区域 ---
        //ImGui::Columns(2, "DataColumns", false);

        // 发送历史
        ImGui::Text("Data History");
        ImGui::BeginChild("##DataBox", ImVec2(0, window_size.y * 0.35f), true);

        ImGui::TextWrapped("%s", AllDataDisplayBuffer.c_str()); // 集体显示数据

        static size_t last_buffer_size = 0; //长度计数器，如果长度发生改变，那就聚焦到最下边的词条
        if (AllDataDisplayBuffer.size() != last_buffer_size) {
            last_buffer_size = AllDataDisplayBuffer.size();
            ImGui::SetScrollY(last_buffer_size +10.f); //视角移到最下边的一行
        }

        ImGui::EndChild();
        if (ImGui::Button("Clear Data History", ImVec2(window_size.x * 0.4f, 0))) {
            AllDataDisplayBuffer.clear(); //清空文本
            last_buffer_size = 0;   //清空长度计数器
        }
        ImGui::SameLine();
        if(ImGui::Button("Save Data to CSV", ImVec2(window_size.x * 0.4f, 0))) {
            SaveDataToCSV(AllDataDisplayBuffer); // 调用导出函数
        }

        //ImGui::NextColumn();

        //// 接收历史
        //ImGui::Text("Read Data");
        //ImGui::BeginChild("ReceiveDataBox", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.6f), true);

        // --- 接收数据 ---
        //ReceiveData(mReceiveBuffer);
        //ReceiveData();



        //// 显示接收到的数据
        //ImGui::TextWrapped("%s", ToHexString(mReceiveBufferDis).c_str());

        //ImGui::EndChild();
        //if (ImGui::Button("Clear Received Data", ImVec2(-FLT_MIN, 0))) {
        //    std::lock_guard<std::mutex> lock(mMutex);
        //    mReceiveBufferDis.clear();
        //}

        //ImGui::Columns(1);
  

        // ========== Test Mode按钮 ==========//

        ImGui::SetCursorPos(ImVec2(
            ImGui::GetWindowWidth() - 300,  // 设置按钮相对子窗口的x位置
            ImGui::GetWindowHeight() - 35   // 设置按钮相对子窗口的y位置
        ));

        //// 设置按钮样式
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);    // 圆角半径
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5, 5)); // 内边距

        // 红色系按钮颜色
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.86f, 0.26f, 0.26f, 0.9f));      // 正常状态：暗红色
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.92f, 0.18f, 0.18f, 1.0f)); // 悬停状态：亮红色
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.72f, 0.16f, 0.16f, 1.0f));  // 按下状态：深红色
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.00f, 1.00f, 1.00f, 1.0f));          // 白色文字（修正Alpha值为1.0）

        if (ImGui::Button("TestMode Page", ImVec2(140, 30))) {
            printf("Run TestMode\n");
            if (TestModePageFlag)
            {
                mCallBack(false); //将mIsRender改成false，渲染测试模式
                TestModePageFlag = false;
            }
            else
            {
                mCallBack(true); //将mIsRender改成true，退出渲染测试模式
                TestModePageFlag = true;
            }
        }
        // 恢复样式
        ImGui::PopStyleColor(4);
        ImGui::PopStyleVar(2);
        //========================================//

        ImGui::End();
    }
}

void UART_DEC::RefreshPortList()
{
    std::lock_guard<std::mutex> lock(mMutex);
    PortManager::GetInstance().RefreshPortList();
    mPortList = PortManager::GetInstance().GetPortList();
}

void UART_DEC::OpenPort()
{
    if (!mSelectedPort.empty())
    {
        if (PortManager::GetInstance().SetCurrentPort(mSelectedPort))
        {
            mIsPortOpen = true;
            std::cout << "Port opened: " << mSelectedPort << std::endl;
        }
        else
        {
            std::cerr << "Failed to open port: " << mSelectedPort << std::endl;
        }
    }
}

void UART_DEC::ClosePort()
{
    PortManager::GetInstance().SetCurrentPort("");
    mIsPortOpen = false;
    std::cout << "Port closed." << std::endl;
}

void UART_DEC::SendData(char mSendBuffer[1024])
{
    if (mIsPortOpen && strlen(mSendBuffer) > 0)
    {
        PortManager::GetInstance().SendData(mSendBuffer,FrameLen); //两个参数，第一个是char mSendBuffer[1024]。第二个数据长度，也就是16进制数的个数。

        ///////////完成mSendBuffer帧拼接赋值后，进行16进制输出校验/////////////////
        /*std::cout << "mSendBuffer2 (hex) = ";
        for (size_t i = 0; i < FrameLen; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<unsigned int>(static_cast<uint8_t>(mSendBuffer[i])) << " ";
        }*/
        ////////////////////////////////////////////////////////////////////////////////

        strcpy(DataSended, mSendBuffer);
        //memset(mSendBuffer, 0, sizeof(mSendBuffer));  // 清空发送缓冲区
    }
}

void UART_DEC::ReceiveData(std::string& mReceiveBuffer)
{
    if (mIsPortOpen)
    {
        std::string data;
        PortManager::GetInstance().ReceiveData(data);
        if (!data.empty())
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mReceiveBuffer += data;  // 追加接收到的数据
            //std::cout << "mReceiveBuffer.Rec (hex) = ";
            //for (size_t i = 0; i < mReceiveBuffer.size(); i++) {
            //    std::cout << std::hex << std::setw(2) << std::setfill('0')
            //        << static_cast<unsigned int>(static_cast<uint8_t>(mReceiveBuffer[i])) << " ";
            //}
            //std::cout << "\n" << std::endl;
           // mReceiveBuffer.clear();
        }
    }
}

std::string UART_DEC::ToHexString(const std::string& data) { //16进制转换函数
    std::stringstream hexStream;
    int byteCount = 0;

    for (size_t i = 0; i < data.size(); ++i) {
        // 将当前字节转换为两位十六进制数
        hexStream << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(static_cast<unsigned char>(data[i])) << " ";
        byteCount++;

        // 每 2 个字节换行
        if (byteCount % 2 == 0) {
            hexStream << "\n";
        }
    }

    // 如果最后一行不足 2 个字节，补全换行
    if (byteCount % 2 != 0) {
        hexStream << "\n";
    }

    return hexStream.str();
}


// 将16进制字符串转换为字节流
std::vector<uint8_t> UART_DEC::HexStringToBytes(const std::string& hexString)
{
    std::vector<uint8_t> bytes;
    std::stringstream ss;
    ss << std::hex << hexString;
    uint32_t byte;
    while (ss >> byte)
    {
        bytes.push_back(static_cast<uint8_t>(byte));
    }
    return bytes;
}

// 将字节流转换为16进制字符串
std::string UART_DEC::BytesToHexString(const std::vector<uint8_t>& bytes)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (uint8_t byte : bytes)
    {
        ss << std::setw(2) << static_cast<int>(byte) << " ";
    }
    return ss.str();
}

uint8_t UART_DEC::ConvertHexStringToUint8(const std::string& hexStr) {
    if (hexStr.empty()) {
        return 0x00; // 默认返回 0x00
    }

    // 检查长度是否合法（最多 2 个字符）
    if (hexStr.length() > 2) {
        throw std::invalid_argument("Hex string too long (max 2 chars)");
    }

    // 转换为 uint8_t
    uint8_t value = static_cast<uint8_t>(std::stoi(hexStr, nullptr, 16));
    return value;
}





// 将发送的 SPI 数据格式化后追加到全局字符串 AllDataDisplay（带换行）
void  UART_DEC::AddSendSPIDataToDisplay(uint16_t SPISend_Display, std::string& AllDataDisplay) {
    // 解析数据
    uint8_t add = (SPISend_Display >> 8) & 0xBF;  // 高字节（Add）并且置第6位的读写位为0
    uint8_t data = SPISend_Display & 0xFF;        // 低字节（Data）
    bool isRead = (SPISend_Display & 0x4000) != 0; // 检查最高位

    // 格式化字符串
    std::ostringstream oss;
    oss << (isRead ? "R" : "W") << ";  "
        << "Add: 0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(add) << "; "
        << "Data: 0x" << std::uppercase << std::setw(2)<< static_cast<int>(data) << "; "
        << "SEND: 0x" << std::uppercase<< std::setw(4)<< SPISend_Display << ";\n"; // 注意末尾的换行符

    // 追加到传入的字符串
    AllDataDisplay += oss.str();
}


// 将接收的 SPI 数据格式化后追加到全局字符串 AllDataDisplay（带换行）
void  UART_DEC::AddRecSPIDataToDisplay(uint16_t SPIRec_Display, std::string& AllDataDisplay) {
    // 解析数据
    uint8_t add = (SPIRec_Display >> 8) & 0xFF;  // 高字节（Add）
    uint8_t data = SPIRec_Display & 0xFF;        // 低字节（Data）
    //bool isRead = (SPISend_Display & 0x4000) != 0; // 检查最高位

    // 格式化字符串
    std::ostringstream oss;
    oss<< "Status:   0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(add) << "; "
        <<"Data: 0x" << std::uppercase << std::setw(2)<< static_cast<int>(data)  << "; "
        << "REC: 0x" << std::uppercase << std::setw(4)<< SPIRec_Display << ";\n "
        << "--------------------------\n"; // 注意末尾的换行符

    // 追加到传入的字符串
    AllDataDisplay += oss.str();
}
// 导出数据到 CSV 文件（按指定格式分列）
void UART_DEC::SaveDataToCSV(const std::string& dataBuffer) {
    // 初始化文件对话框
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
    ofn.lpstrDefExt = "csv";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    // 弹出保存对话框
    if (GetSaveFileNameA(&ofn)) {
        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            // 写入CSV表头
            outFile << "Type,FullData,Add/Status,Data,R/W\n";

            std::istringstream iss(dataBuffer);
            std::string line;
            bool isSendBlock = false;

            while (std::getline(iss, line)) {
                // 跳过无效行
                if (line.find("Please Open Port!!!") != std::string::npos ||
                    line.find("Data saved to:") != std::string::npos ||
                    line.find("--------------------------") != std::string::npos) {
                    continue;
                }

                // 解析SEND行
                if (line.find("SEND:") != std::string::npos) {
                    isSendBlock = true;

                    // 提取各部分数据
                    size_t addPos = line.find("Add:");
                    size_t dataPos = line.find("Data:");
                    size_t sendPos = line.find("SEND:");
                    size_t rwPos = line.find(";");

                    if (addPos != std::string::npos && dataPos != std::string::npos && sendPos != std::string::npos) {
                        std::string rw = line.substr(0, rwPos);
                        std::string add = line.substr(addPos + 5, 4); // "Add: 0xXX" 取XX
                        std::string data = line.substr(dataPos + 6, 4); // "Data: 0xXX" 取XX
                        std::string fullData = line.substr(sendPos + 6, 6); // "SEND: 0xXXXX" 取XXXX

                        // 写入CSV行（Send数据）
                        outFile << "SEND,"
                            << "0x" << fullData.substr(fullData.size() - 4) << ","
                            << add << ","
                            << data << ","
                            << (rw.find("R") != std::string::npos ? "Read" : "Write")
                            << "\n";
                    }
                }
                // 解析REC行
                else if (line.find("REC:") != std::string::npos && isSendBlock) {
                    isSendBlock = false;

                    // 提取各部分数据
                    size_t statusPos = line.find("Status:");
                    size_t dataPos = line.find("Data:");
                    size_t recPos = line.find("REC:");

                    if (statusPos != std::string::npos && dataPos != std::string::npos && recPos != std::string::npos) {
                        std::string status = line.substr(statusPos + 10, 4); // "Status: 0xXX" 取XX
                        std::string data = line.substr(dataPos + 6, 4); // "Data: 0xXX" 取XX
                        std::string fullData = line.substr(recPos + 5, 6); // "REC: 0xXXXX" 取XXXX

                        // 写入CSV行（Rec数据）
                        outFile << "REC,"
                            << "0x" << fullData.substr(fullData.size() - 4) << ","
                            << status << ","
                            << data << ","
                            << "N/A\n"; // 接收数据没有R/W状态
                    }
                }
            }

            outFile.close();
            AllDataDisplayBuffer += "Data saved to: " + std::string(filePath) + "\n";
        }
        else {
            AllDataDisplayBuffer += "Failed to save file!\n";
        }
    }
}

/*
// 导出数据到 CSV 文件（过滤无效行）
void UART_DEC::SaveDataToCSV(const std::string& dataBuffer) {
    // 初始化文件对话框
    OPENFILENAMEA ofn;
    char filePath[MAX_PATH] = { 0 };
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "CSV Files (*.csv)\0*.csv\0All Files (*.*)\0*.*\0";
    ofn.lpstrDefExt = "csv";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    // 弹出保存对话框
    if (GetSaveFileNameA(&ofn)) {
        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            // 写入数据（同前文逻辑）
            std::istringstream iss(dataBuffer);
            std::string line;
            while (std::getline(iss, line)) {
                if ((line.find("Please Open Port!!!") != std::string::npos) || (line.find("--------------------------") != std::string::npos)) {
                    continue;
                }
                std::replace(line.begin(), line.end(), ';', ',');
                outFile << line << "\n";
            }
            outFile.close();
            AllDataDisplayBuffer += "Data saved to: " + std::string(filePath) + "\n";
        }
        else {
            AllDataDisplayBuffer += "Failed to save file!\n";
        }
    }
}*/