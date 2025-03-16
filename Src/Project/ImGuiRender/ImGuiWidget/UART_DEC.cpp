#include <iostream>
#include "UART_DEC.h"
#include "imgui/imgui.h"
#include "../../Port/PortManager.h"
#include <string>
#include <sstream>
#include <iomanip>

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
    // 绘制串口选择下拉框
    if (ImGui::BeginCombo("Select Port", mSelectedPort.empty() ? "None" : mSelectedPort.c_str()))
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

    // 刷新串口列表按钮
    if (ImGui::Button("Refresh Port List"))
    {
        RefreshPortList();
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

    // 发送数据输入框和按钮
    ImGui::InputText("Send Data", mSendBuffer, sizeof(mSendBuffer));
    if (ImGui::Button("Send"))
    {
        SendData();
    }

    ReceiveData();
    std::string hexData = ToHexString(mReceiveBuffer);

    // 接收数据显示
    ImGui::Text("Received Data:");
    ImGui::TextWrapped("%s", hexData.c_str());

    // 清空接收数据按钮
    if (ImGui::Button("Clear Received Data"))
    {
        std::lock_guard<std::mutex> lock(mMutex);
        mReceiveBuffer.clear();
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
        PortManager::GetInstance().SendData(mSendBuffer);
        std::cout << "Data sent: " << mSendBuffer << std::endl;
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