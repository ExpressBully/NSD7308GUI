#include <iostream>
#include <string>
#include "PortData.h"

PortData::PortData() : mhPort(INVALID_HANDLE_VALUE){}

PortData::PortData(HANDLE hPort, DCB& dcb)
    : mhPort(hPort)
{
    if (!OpenPort()) 
    {
        throw std::runtime_error("Failed to configure port");
    }
}

PortData::~PortData() 
{
    ClosePort();
}

// 重新配置串口参数
void PortData::ResetPort(HANDLE hPort, DCB& dcb)
{
    ClosePort();
    mhPort = hPort;
    if (!OpenPort())
    {
        throw std::runtime_error("Failed to reconfigure port");
    }
}

// 核心配置函数
bool PortData::OpenPort()
{
    if (mhPort == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    if (!GetCommState(mhPort, &mDcb))
    {
        return false;
    }

    if (!SetCommState(mhPort, &mDcb))
    {
        return false;
    }

    // 2. 设置超时参数
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;         // 字符间超时
    timeouts.ReadTotalTimeoutConstant = 50;   // 读操作固定超时
    timeouts.WriteTotalTimeoutConstant = 50;  // 写操作固定超时
    return SetCommTimeouts(mhPort, &timeouts);
}

bool PortData::ClosePort() 
{
    if (mhPort != INVALID_HANDLE_VALUE)
    {
        CloseHandle(mhPort);
        mhPort = INVALID_HANDLE_VALUE;
        return true;
    }
    return false;
}

// 数据发送（字符串版本）
void PortData::SendData(const std::string& data) 
{
    SendData(data.c_str(), static_cast<int>(data.size()));
}

// 数据发送（原始字节版本）
void PortData::SendData(const char* data, int len) 
{
    if (mhPort == INVALID_HANDLE_VALUE) return;

    DWORD bytesWritten;
    if (!WriteFile(mhPort, data, len, &bytesWritten, NULL))
    {
        throw std::runtime_error("WriteFile failed: " + std::to_string(GetLastError()));
    }
}

// 数据接收（存储到字符串）
void PortData::ReceiveData(std::string& data) 
{
    static char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    DWORD bytesRead = 0;
    if (ReadFile(mhPort, buffer, sizeof(buffer), &bytesRead, NULL)) 
    {
        data.assign(buffer, bytesRead);
    }
    else
    {
        data.clear();
    }
}

// 数据接收（原始字节版本）
void PortData::ReceiveData(char* data, int len) 
{
    DWORD bytesRead;
    if (!ReadFile(mhPort, data, len, &bytesRead, NULL)) 
    {
        memset(data, 0, len);
    }
}