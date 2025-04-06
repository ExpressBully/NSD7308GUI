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

UART_DEC::UART_DEC()
    : mIsPortOpen(false)
{
    memset(mSendBuffer, 0, sizeof(mSendBuffer));  // 初始化发送缓冲区
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
    PortData portdata;
    ImVec2 main_window_pos = ImGui::GetMainViewport()->Pos;

    // 设置窗口 "UART" 的位置，相对于主窗口的左上角
    ImGui::SetNextWindowPos(ImVec2(main_window_pos.x + 0, main_window_pos.y + 0), ImGuiCond_Always); // 偏移 (10, 10)

    // 设置窗口 "UART" 的大小
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiCond_Always); // 设置初始大小为 500x400

    // 创建窗口
    ImGui::Begin("UART", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);

    // 绘制 "Select Port" 文本，并设置灰色背景
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // 设置文本颜色为黑色
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f)); // 设置背景颜色为灰色
    ImGui::BeginChild("SelectPortLabel", ImVec2(100, ImGui::GetTextLineHeight()), false, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Select Port");
    ImGui::EndChild();
    ImGui::PopStyleColor(2); // 恢复样式

    // 将下拉框放在 "Select Port" 文本的右侧
    ImGui::SameLine(); // 将后续控件放在同一行
    if (ImGui::BeginCombo("##SelectPortCombo", mSelectedPort.empty() ? "None" : mSelectedPort.c_str()))
    {
        for (const auto& port : mPortList)
        {
            if (ImGui::Selectable(port.c_str(), port == mSelectedPort))
            {
                mSelectedPort = port;
            }
        }
        ImGui::EndCombo();
    }

    // 打开/关闭串口按钮
    if (mIsPortOpen)
    {
        if (ImGui::Button("Close Port"))
        {
            ClosePort();
        }
    }
    else
    {
        if (ImGui::Button("Open Port"))
        {
            OpenPort();
        }
    }

    // 将 "Refresh Port List" 按钮放在 "Open Port" 按钮的右侧
    ImGui::SameLine(); // 将后续控件放在同一行
    if (ImGui::Button("Refresh Port List"))
    {
        RefreshPortList();
    }

    // 发送数据部分
    ImGui::Text("Send Data:"); // 将 "Send Data" 文本放在左边

    // 创建两个并排的输入框：左侧为地址，右侧为数据
    ImGui::Columns(2, "SendDataColumns", false); // 分为两列，不显示分隔线

    // 左侧：地址输入框
    ImGui::Text("Address"); // 标注地址
    static char addressBuffer[256] = { 0 }; // 地址输入缓冲区
    ImGui::InputText("##Address", addressBuffer, sizeof(addressBuffer), ImGuiInputTextFlags_CharsHexadecimal);

    // 切换到右侧列
    ImGui::NextColumn();

    // 右侧：数据输入框
    ImGui::Text("Data"); // 标注数据
    static char dataBuffer[256] = { 0 }; // 数据输入缓冲区
    ImGui::InputText("##Data", dataBuffer, sizeof(dataBuffer), ImGuiInputTextFlags_CharsHexadecimal);

    // 结束分列布局
    ImGui::Columns(1);
    
    
    /////////////
// 发送和读取按钮
    bool isSend = ImGui::Button("Send");
    ImGui::SameLine(); // 让按钮在同一行显示
    bool isRead = ImGui::Button("Read");

    if (isSend || isRead)
    {
        // 根据按钮设置命令值
        uint8_t Command = isSend ? 0xA3 : 0xA4;

        // 将地址和数据转换为字节流
        std::vector<uint8_t> addressBytesDis = HexStringToBytes(addressBuffer);
        std::vector<uint8_t> dataBytesDis = HexStringToBytes(dataBuffer);

        // 如果地址或数据为空，默认值为 0x00
        if (addressBytesDis.empty()) addressBytesDis.push_back(0x00);
        if (dataBytesDis.empty()) dataBytesDis.push_back(0x00);

        // 拼接地址和数据
        std::vector<uint8_t> combinedBytes;
        combinedBytes.insert(combinedBytes.end(), addressBytesDis.begin(), addressBytesDis.end());
        combinedBytes.insert(combinedBytes.end(), dataBytesDis.begin(), dataBytesDis.end());

        uint8_t addressBytes = ConvertHexStringToUint8(addressBuffer);
        uint8_t dataBytes = ConvertHexStringToUint8(dataBuffer);
        portdata.DataSendAnalysis(mSendBuffer, Command, addressBytes, dataBytes);

        // 调试输出
        std::cout << "mSendBuffer Length1= " << sizeof(mSendBuffer) << std::endl;
        std::cout << "mSendBuffer (hex) = ";
        for (size_t i = 0; i < 5; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<unsigned int>(static_cast<uint8_t>(mSendBuffer[i])) << " ";
        }
        std::cout << std::endl;

        // 发送数据并记录历史
        if (mIsPortOpen)
        {
            SendData();
            std::string combinedHex = BytesToHexString(combinedBytes);
            mSendHistory.push_back(combinedHex);
        }
    }

    // 创建两个并排的区域：左侧为 "Send Data" 框，右侧为 "Receive Data" 框
    ImGui::Columns(2, "DataColumns", false); // 分为两列，不显示分隔线

    // 左侧：Send Data 部分
    ImGui::Text("Send Data History:"); // 将 "Send Data History" 文本放在框的上边
    ImGui::BeginChild("SendDataBox", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);

    // 显示发送数据历史记录
    for (const auto& data : mSendHistory)
    {
        ImGui::TextWrapped("%s", data.c_str()); // 显示数据
    }

    // 如果历史记录更新，滚动条跳至最底部
    static size_t lastSendHistorySize = 0; // 用于跟踪上次发送历史记录的大小
    if (mSendHistory.size() != lastSendHistorySize)
    {
        ImGui::SetScrollY(ImGui::GetScrollMaxY() + 100); // 滚动条跳至最底部
        lastSendHistorySize = mSendHistory.size(); // 更新历史记录大小
    }

    ImGui::EndChild();

    // 在 "Send Data" 框下边添加清除按钮
    if (ImGui::Button("Clear Send Data", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        mSendHistory.clear(); // 清空发送历史记录
    }

    // 切换到右侧列
    ImGui::NextColumn();

    // 右侧：Receive Data 部分
    ImGui::Text("Received Data:"); // 将 "Received Data" 文本放在框的上边
    ImGui::BeginChild("ReceiveDataBox", ImVec2(0, 150), true, ImGuiWindowFlags_HorizontalScrollbar);

    ReceiveData();
    uint8_t add, data;
    bool flag = portdata.DataRecAnalysis(mReceiveBuffer, add, data); //将接收到的帧，解析出地址和数据

    if(!mReceiveBuffer.empty()) //接收到数据后，将地址和数据填写进显示用的buffer。mReceiveBufferDis
    { 
        mReceiveBufferDis += add;
        mReceiveBufferDis += data;
    }

    std::string hexData = ToHexString(mReceiveBufferDis);

    ImGui::TextWrapped("%s", hexData.c_str()); // 显示接收到的数据以及提取的 ADD 和 DATA
    // 如果接收到新数据，滚动条跳至最底部
    static size_t lastReceiveDataSize = 0; // 用于跟踪上次接收到的数据大小
    if (hexData.size() != lastReceiveDataSize)
    {
        ImGui::SetScrollY(ImGui::GetScrollMaxY() + 100); // 滚动条跳至最底部
        lastReceiveDataSize = hexData.size(); // 更新数据大小
    }

    ImGui::EndChild();

    // 在 "Receive Data" 框下边添加清除按钮
    if (ImGui::Button("Clear Received Data", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mReceiveBufferDis.clear(); // 清空接收数据
    }

    // 结束分列布局
    ImGui::Columns(1);
    mReceiveBuffer.clear();
    hexData.clear();
    // 结束窗口
    ImGui::End();
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

void UART_DEC::SendData()
{
    if (mIsPortOpen && strlen(mSendBuffer) > 0)
    {
        PortManager::GetInstance().SendData(mSendBuffer,5); //两个参数，一个char mSendBuffer[1024]。另一个数据长度，也就是16进制数的个数。

        ///////////完成mSendBuffer帧拼接赋值后，进行16进制输出校验/////////////////
        std::cout << "mSendBuffer2 (hex) = ";
        for (size_t i = 0; i < 5; i++) {
            std::cout << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<unsigned int>(static_cast<uint8_t>(mSendBuffer[i])) << " ";
        }
        ////////////////////////////////////////////////////////////////////////////////

        //std::cout << "mSendBuffer Length2=  " << strlen(mSendBuffer) << std::endl;
        std::cout << "Data sent: " << mSendBuffer << std::endl;
        strcpy(DataSended, mSendBuffer);
        memset(mSendBuffer, 0, sizeof(mSendBuffer));  // 清空发送缓冲区
    }
}

void UART_DEC::ReceiveData()
{
    if (mIsPortOpen)
    {
        std::string data;
        PortManager::GetInstance().ReceiveData(data);

        if (!data.empty())
        {
            std::lock_guard<std::mutex> lock(mMutex);
            mReceiveBuffer += data;  // 追加接收到的数据
            std::cout << "Data received: " << data << std::endl;
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
