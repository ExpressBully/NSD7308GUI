#include <winreg.h>
#include <tchar.h>
#include <stdexcept>
#include <vector>
#include <string>
#include <mutex>
#include <memory>
#include <algorithm>
#include <iostream>
#include "PortData.h"

#include "PortManager.h"

PortManager& PortManager::GetInstance() 
{
    static PortManager instance;
    return instance;
}

PortManager::PortManager()
{

}


void PortManager::RefreshPortList() {
    std::lock_guard<std::mutex> lock(mMutex);
    mPortList.clear();

    HKEY hKey;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
        _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
    {
        throw std::runtime_error("Failed to access registry");
    }

    TCHAR portValue[256];
    DWORD index = 0;
    DWORD type = REG_SZ;
    DWORD size = sizeof(portValue);

    while (ERROR_SUCCESS == RegEnumValue(
        hKey,
        index++,
        nullptr,    // 不获取值名称
        nullptr,     // 不获取名称长度
        nullptr,     // 保留参数必须为NULL
        &type,       // 接收注册表值类型
        reinterpret_cast<LPBYTE>(portValue),
        &size        // 输入时为缓冲区总字节数，输出时为实际数据字节数
    )) {
        // 添加终止符
        portValue[size / sizeof(TCHAR)] = _T('\0');

        if (type == REG_SZ) {
#ifdef UNICODE
            std::wstring ws(portValue);
#else
            std::string ws(portValue);
#endif
            mPortList.push_back(ws);
        }
        size = sizeof(portValue); // 重置为字节数
    }
    RegCloseKey(hKey);
}

/******************** 端口操作 ​********************/
void PortManager::SetCurrentPort(const std::string& portName)
{
    std::lock_guard<std::mutex> lock(mMutex);

    // 验证端口有效性
    if (std::find(mPortList.begin(), mPortList.end(), portName) == mPortList.end())
    {
        throw std::runtime_error("Port not available: " + portName);
    }


    // 创建新端口实例
    HANDLE hPort = CreatePortHandle(portName);
    DCB dcb = GetDefaultPortConfig();

    mActivePort->ResetPort(hPort, dcb);
}

HANDLE PortManager::CreatePortHandle(const std::string& portName) const 
{
    const std::string fullName = "\\\\.\\" + portName;
    HANDLE hPort = CreateFileA(
        fullName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (hPort == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("CreateFile failed (0x" +
            std::to_string(GetLastError()) + ")");
    }
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

/******************** 数据通信 ​********************/
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

const std::string& PortManager::GetCurrentPort() const noexcept
{
    return mCurrentPort;
}

const std::vector<std::string>& PortManager::GetPortList() const noexcept 
{
    return mPortList;
}