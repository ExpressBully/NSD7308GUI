#include "PortData.h"
#include "PortManager.h"
#include <iostream>
#include <thread>

namespace Unit
{
    static std::vector<std::string> GetAvailablePorts()
    {
        std::vector<std::string> portList;

        // 打开注册表中的串口键
        HKEY hKey;
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        {
            std::cerr << "Failed to open registry key." << std::endl;
            return portList;
        }

        // 枚举注册表中的值
        char valueName[256];
        char portName[256];
        DWORD valueNameSize, portNameSize, type;
        DWORD index = 0;

        while (true)
        {
            valueNameSize = sizeof(valueName);
            portNameSize = sizeof(portName);
            if (RegEnumValueA(hKey, index, valueName, &valueNameSize, NULL, &type, (LPBYTE)portName, &portNameSize) != ERROR_SUCCESS)
            {
                break;  // 枚举结束
            }

            if (type == REG_SZ)
            {
                portList.push_back(portName);
            }

            index++;
        }

        // 关闭注册表键
        RegCloseKey(hKey);

        return portList;
    }
}

PortManager& PortManager::GetInstance()
{
    static PortManager instance;
    return instance;
}

PortManager::PortManager() {}

PortManager::~PortManager() {}

void PortManager::RefreshPortList()
{
    std::lock_guard<std::mutex> lock(mMutex);
    mPortList.clear();
    mPortList = Unit::GetAvailablePorts();
}

bool PortManager::SetCurrentPort(const std::string& portName)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (std::find(mPortList.begin(), mPortList.end(), portName) == mPortList.end())
    {
        return false;
    }

    HANDLE hPort = CreatePort(portName);
    DCB dcb = GetDefaultPortConfig();

    mActivePort = std::make_unique<PortData>(hPort, dcb);
    mCurrentPort = portName;
    return true;
}

HANDLE PortManager::CreatePort(const std::string& portName) const
{
    // 将串口名称转换为 Windows API 所需的格式
    std::string fullPortName = "\\\\.\\" + portName;;  // 对于 COM10 及以上的串口，需要添加前缀 "\\.\"

    // 使用 CreateFileA 打开串口
    HANDLE hPort = CreateFileA(
        fullPortName.c_str(),               // 串口名称
        GENERIC_READ | GENERIC_WRITE,       // 读写权限
        0,                                  // 共享模式（0 表示独占）
        NULL,                               // 安全属性
        OPEN_EXISTING,                      // 打开已存在的设备
        FILE_ATTRIBUTE_NORMAL,              // 文件属性
        NULL                                // 模板文件句柄
    );

    // 检查串口是否成功打开
    if (hPort == INVALID_HANDLE_VALUE)
    {
        DWORD error = GetLastError();
        std::cerr << "Failed to open port " << portName << ", error code: " << error << std::endl;
        return INVALID_HANDLE_VALUE;
    }

    // 获取默认串口配置
    DCB dcb = GetDefaultPortConfig();

    // 应用串口配置
    if (!SetCommState(hPort, &dcb))
    {
        std::cerr << "Failed to set comm state for port " << portName << std::endl;
        CloseHandle(hPort);
        return INVALID_HANDLE_VALUE;
    }

    // 设置串口超时参数
    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;         // 字符间超时
    timeouts.ReadTotalTimeoutConstant = 50;    // 读操作固定超时
    timeouts.WriteTotalTimeoutConstant = 50;   // 写操作固定超时

    if (!SetCommTimeouts(hPort, &timeouts))
    {
        std::cerr << "Failed to set comm timeouts for port " << portName << std::endl;
        CloseHandle(hPort);
        return INVALID_HANDLE_VALUE;
    }

    // 返回成功打开的串口句柄
    return hPort;
}

DCB PortManager::GetDefaultPortConfig() const
{
    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);
    dcb.BaudRate = CBR_115200;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    return dcb;
}

void PortManager::SendData(const std::string& data)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mActivePort)
    {
        mActivePort->SendData(data);
    }
}

void PortManager::ReceiveData(std::string& data)
{
    std::lock_guard<std::mutex> lock(mMutex);
    if (mActivePort)
    {
        mActivePort->ReceiveData(data);
    }
}

void PortManager::SetDataReceivedCallback(DataReceivedCallback callback)
{
    if (mActivePort)
    {
        mActivePort->SetDataReceivedCallback(callback);
    }
}

void PortManager::SetErrorCallback(ErrorCallback callback)
{
    if (mActivePort)
    {
        mActivePort->SetErrorCallback(callback);
    }
}

bool PortManager::IsPortOpen() const
{
    return mActivePort && mActivePort->IsOpen();
}

const std::string& PortManager::GetCurrentPort() const noexcept
{
    return mCurrentPort;
}

const std::vector<std::string>& PortManager::GetPortList() const noexcept
{
    return mPortList;
}