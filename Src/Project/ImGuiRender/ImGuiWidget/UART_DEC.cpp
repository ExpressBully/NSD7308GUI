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
    // 获取主视口信息
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 main_window_size = viewport->WorkSize;
    ImVec2 main_window_pos = viewport->WorkPos;

    // 设置窗口尺寸（父窗口的40%宽度，50%高度）
    const float width_ratio = 0.3f;
    const float height_ratio = 0.5f;
    ImVec2 window_size(
        main_window_size.x * width_ratio,
        main_window_size.y * height_ratio
    );

    // 固定在左上角（可设置偏移量）
    const float offset_x = 10.0f; // 水平偏移
    const float offset_y = 10.0f; // 垂直偏移
    ImVec2 window_pos(
        main_window_pos.x + offset_x,
        main_window_pos.y + offset_y
    );

    // 设置窗口属性
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(window_size, ImGuiCond_Always);

    // 窗口标志：无折叠箭头+禁止手动调整
    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove;

    // 开始绘制窗口
    if (ImGui::Begin("UART", nullptr, window_flags))
    {
        /* 以下是您原有的UI内容（完全保留） */

        // --- Select Port 部分 ---
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::BeginChild("SelectPortLabel", ImVec2(100, ImGui::GetTextLineHeight()), false);
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
            if (ImGui::Button("Close Port")) ClosePort();
        }
        else {
            if (ImGui::Button("Open Port")) OpenPort();
        }
        ImGui::SameLine();
        if (ImGui::Button("Refresh Port List")) RefreshPortList();

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

        // --- 发送/读取按钮 ---
        bool isSend = ImGui::Button("Send");
        ImGui::SameLine();
        bool isRead = ImGui::Button("Read");

        // --- 数据历史区域 ---
        ImGui::Columns(2, "DataColumns", false);

        // 发送历史
        ImGui::Text("Send History");
        ImGui::BeginChild("SendDataBox", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.6f), true);
        for (const auto& data : mSendHistory) {
            ImGui::TextWrapped("%s", data.c_str());
        }
        ImGui::EndChild();
        if (ImGui::Button("Clear Send Data", ImVec2(-FLT_MIN, 0))) {
            mSendHistory.clear();
        }

        ImGui::NextColumn();

        // 接收历史
        ImGui::Text("Received Data");
        ImGui::BeginChild("ReceiveDataBox", ImVec2(0, ImGui::GetContentRegionAvail().y * 0.6f), true);
        ImGui::TextWrapped("%s", ToHexString(mReceiveBufferDis).c_str());
        ImGui::EndChild();
        if (ImGui::Button("Clear Received Data", ImVec2(-FLT_MIN, 0))) {
            mReceiveBufferDis.clear();
        }

        ImGui::Columns(1);

        /* 以上是您原有的UI内容 */

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
