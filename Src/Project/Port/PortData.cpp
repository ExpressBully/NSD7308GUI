#include "PortData.h"
#include <iostream>
#include <thread>

PortData::PortData() : mhPort(INVALID_HANDLE_VALUE) {}

PortData::PortData(HANDLE hPort, DCB& dcb)
    : mhPort(hPort), mDcb(dcb)
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

void PortData::ResetPort(HANDLE hPort, DCB& dcb)
{
    ClosePort();
    mhPort = hPort;
    mDcb = dcb;
    if (!OpenPort())
    {
        throw std::runtime_error("Failed to reconfigure port");
    }
}

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

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutConstant = 50;
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

void PortData::SendData(const std::string& data)
{
    SendData(data.c_str(), static_cast<int>(data.size()));
}

void PortData::SendData(const char* data, int len)
{
    if (mhPort == INVALID_HANDLE_VALUE) return;

    DWORD bytesWritten;
    if (!WriteFile(mhPort, data, len, &bytesWritten, NULL))
    {
        if (mErrorCallback)
        {
            mErrorCallback("WriteFile failed: " + std::to_string(GetLastError()));
        }
    }
}

void PortData::ReceiveData(std::string& data)
{
    static char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    DWORD bytesRead = 0;
    if (ReadFile(mhPort, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        data.assign(buffer, bytesRead);
        if (mDataReceivedCallback)
        {
            mDataReceivedCallback(data);
        }
    }
    else
    {
        data.clear();
    }
}

void PortData::ReceiveData(char* data, int len)
{
    DWORD bytesRead;
    if (!ReadFile(mhPort, data, len, &bytesRead, NULL))
    {
        memset(data, 0, len);
    }
}

void PortData::SetDataReceivedCallback(DataReceivedCallback callback)
{
    mDataReceivedCallback = callback;

}

void PortData::SetErrorCallback(ErrorCallback callback)
{
    mErrorCallback = callback;
}

bool PortData::IsOpen() const
{
    return mhPort != INVALID_HANDLE_VALUE;
}